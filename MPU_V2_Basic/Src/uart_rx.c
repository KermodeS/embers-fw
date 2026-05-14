/*
 * uart_rx.c -- STM-RX1: PROTO-001 command parser on USART2.
 *
 * Physical link: STM32 PA2=USART2_TX -> ESP32 GPIO16=UART2_RX
 *                STM32 PA3=USART2_RX <- ESP32 GPIO17=UART2_TX
 * Baud: 115200 8N1, no flow control.
 *
 * Architecture:
 *   USART2_IRQHandler calls uart_rx_isr_byte() per received byte.
 *   Bytes accumulate in a 64-byte line buffer until '\n'.
 *   Complete lines are posted to a 4-slot ring queue.
 *   uart_rx_poll() (main loop) pulls one line per call, validates,
 *   and dispatches to existing channel/mode setters.
 *
 * PROTO-001 format: <DIR><CMD>:<VAL>\n
 *   DIR  = 'E' (ESP32->STM32)
 *   CMD  = 3 chars
 *   ':'  = separator
 *   VAL  = ASCII decimal integer
 *
 * DBG lines are temporary -- remove before production build.
 */

#include "uart_rx.h"
#include "main.h"
#include "LLU_V2_LightControl.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* LL drivers */
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_bus.h"

/* Existing STM32 symbols confirmed during resumability */
extern void SetRedLevel(uint16_t u16_LightIndex);
extern void SetGreenLevel(uint16_t u16_LightIndex);
extern void SetBlueLevel(uint16_t u16_LightIndex);
extern void SetUvLevel(uint16_t u16_LightIndex);
extern uint16_t u16_GlobalBrightMax;
extern void UpdateStrobeRawIndices(void);
extern uint8_t u8_StateMaschine;

/* Mode constants (from LLU_V2_LightControl.h) */
/* PROTO-001 EMOD value -> internal SM constant mapping:
 *   0 -> SM_MODE_MANUAL      (10)
 *   1 -> SM_MODE_STROBOSCOPE (50)
 *   2 -> SM_MODE_FUNCTION_A  (20)
 *   3 -> SM_MODE_FUNCTION_B  (30)
 *   4 -> SM_MODE_FUNCTION_C  (40)
 */

/* ── line buffer ──────────────────────────────────────────────────── */
#define LINE_BUF_SIZE   64

static uint8_t  s_linebuf[LINE_BUF_SIZE];
static uint8_t  s_linelen = 0;
static uint8_t  s_overflow = 0;  /* set when line exceeds LINE_BUF_SIZE */

/* ── command queue (ring buffer of fixed 64-byte slots) ───────────── */
#define QUEUE_SLOTS     4

static char     s_queue[QUEUE_SLOTS][LINE_BUF_SIZE];
static uint8_t  s_q_head = 0;   /* consumer index */
static uint8_t  s_q_tail = 0;   /* producer index */
static uint8_t  s_q_count = 0;

/* ── TX helper ────────────────────────────────────────────────────── */
static void uart2_send_str(const char *s)
{
    while (*s) {
        while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
        LL_USART_TransmitData8(USART2, (uint8_t)*s++);
    }
}

/* Sends s + '\n'. Caller passes e.g. "SHBT:1" without newline. */
static void uart2_send_line(const char *s)
{
    uart2_send_str(s);
    while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
    LL_USART_TransmitData8(USART2, '\n');
}

/* ── queue helpers ────────────────────────────────────────────────── */
static void queue_push(const char *line)
{
    if (s_q_count >= QUEUE_SLOTS) {
        /* Queue full -- drop line, report overflow */
        uart2_send_line("SERR:2");
        return;
    }
    strncpy(s_queue[s_q_tail], line, LINE_BUF_SIZE - 1);
    s_queue[s_q_tail][LINE_BUF_SIZE - 1] = '\0';
    s_q_tail = (s_q_tail + 1) % QUEUE_SLOTS;
    s_q_count++;
}

static int queue_pop(char *out)
{
    if (s_q_count == 0) return 0;
    strncpy(out, s_queue[s_q_head], LINE_BUF_SIZE);
    s_q_head = (s_q_head + 1) % QUEUE_SLOTS;
    s_q_count--;
    return 1;
}

/* ── ISR byte feeder (called from USART2_IRQHandler) ─────────────── */
void uart_rx_isr_byte(uint8_t b)
{
    if (b == '\r') return;  /* silently drop CR -- terminals send CRLF */

    if (b == '\n') {
        if (s_overflow) {
            /* Line was too long -- discard and report */
            uart2_send_line("SERR:2");
            s_overflow = 0;
            s_linelen  = 0;
            return;
        }
        if (s_linelen == 0) return;  /* ignore empty lines */
        s_linebuf[s_linelen] = '\0';
        queue_push((const char *)s_linebuf);
        s_linelen = 0;
        return;
    }

    if (s_linelen >= LINE_BUF_SIZE - 1) {
        s_overflow = 1;
        return;
    }
    s_linebuf[s_linelen++] = b;
}

/* ── command dispatcher ───────────────────────────────────────────── */
static void dispatch(const char *line)
{
    /* Validation order per PROTO-001 Section 9:
     * 1. First char == 'E'
     * 2. Total length >= 6
     * 3. Char at position 4 == ':'
     * 4. Command code (chars 1..3) recognised
     * 5. Value parses as non-negative decimal
     * 6. Value within per-command range
     */
    if (line[0] != 'E') { uart2_send_line("SERR:1"); return; }

    uint8_t len = (uint8_t)strlen(line);
    if (len < 6)         { uart2_send_line("SERR:1"); return; }
    if (line[4] != ':')  { uart2_send_line("SERR:1"); return; }

    /* Extract 3-char command code */
    char cmd[4];
    cmd[0] = line[1]; cmd[1] = line[2]; cmd[2] = line[3]; cmd[3] = '\0';

    /* Parse value -- must be all digits, non-negative */
    const char *valstr = &line[5];
    if (*valstr == '\0') { uart2_send_line("SERR:2"); return; }
    for (const char *p = valstr; *p; p++) {
        if (*p < '0' || *p > '9') { uart2_send_line("SERR:2"); return; }
    }
    uint32_t val = (uint32_t)atoi(valstr);

    char dbg[48];

    /* ── EMOD ── */
    if (strcmp(cmd, "MOD") == 0) {
        if (val > 4) { uart2_send_line("SERR:2"); return; }
        /* Map PROTO-001 0-4 to internal SM constants */
        const uint8_t mode_map[5] = {
            SM_MODE_MANUAL,       /* 0 */
            SM_MODE_STROBOSCOPE,  /* 1 */
            SM_MODE_FUNCTION_A,   /* 2 */
            SM_MODE_FUNCTION_B,   /* 3 */
            SM_MODE_FUNCTION_C    /* 4 */
        };
        u8_StateMaschine = mode_map[val];
        snprintf(dbg, sizeof(dbg), "DBG cmd=EMOD val=%lu ok", val);
        uart2_send_line(dbg);
        return;
    }

    /* ── EBRT ── */
    if (strcmp(cmd, "BRT") == 0) {
        if (val > 1000) { uart2_send_line("SERR:2"); return; }
        u16_GlobalBrightMax = (uint16_t)val;
        UpdateStrobeRawIndices();
        snprintf(dbg, sizeof(dbg), "DBG cmd=EBRT val=%lu ok", val);
        uart2_send_line(dbg);
        return;
    }

    /* ── ERED ── */
    if (strcmp(cmd, "RED") == 0) {
        if (val > 1000) { uart2_send_line("SERR:2"); return; }
        SetRedLevel((uint16_t)val);
        snprintf(dbg, sizeof(dbg), "DBG cmd=ERED val=%lu ok", val);
        uart2_send_line(dbg);
        return;
    }

    /* ── EGRN ── */
    if (strcmp(cmd, "GRN") == 0) {
        if (val > 1000) { uart2_send_line("SERR:2"); return; }
        SetGreenLevel((uint16_t)val);
        snprintf(dbg, sizeof(dbg), "DBG cmd=EGRN val=%lu ok", val);
        uart2_send_line(dbg);
        return;
    }

    /* ── EBLU ── */
    if (strcmp(cmd, "BLU") == 0) {
        if (val > 1000) { uart2_send_line("SERR:2"); return; }
        SetBlueLevel((uint16_t)val);
        snprintf(dbg, sizeof(dbg), "DBG cmd=EBLU val=%lu ok", val);
        uart2_send_line(dbg);
        return;
    }

    /* ── EUVV ── */
    if (strcmp(cmd, "UVV") == 0) {
        if (val > 1000) { uart2_send_line("SERR:2"); return; }
        SetUvLevel((uint16_t)val);
        snprintf(dbg, sizeof(dbg), "DBG cmd=EUVV val=%lu ok", val);
        uart2_send_line(dbg);
        return;
    }

    /* ── EWHT -- V2 hardware disabled, accept silently ── */
    if (strcmp(cmd, "WHT") == 0) {
        if (val > 1000) { uart2_send_line("SERR:2"); return; }
        uart2_send_line("DBG EWHT ignored - V2 WHT hardware-disabled");
        return;
    }

    /* ── ERST ── */
    if (strcmp(cmd, "RST") == 0) {
        if (val != 1) { uart2_send_line("SERR:2"); return; }
        uart2_send_line("DBG ERST -> NVIC reset");
        /* Brief flush delay so the line reaches the ESP32 */
        for (volatile uint32_t i = 0; i < 200000; i++) {}
        NVIC_SystemReset();
        return;
    }

    /* ── EROL ── */
    if (strcmp(cmd, "ROL") == 0) {
        if (val > 2) { uart2_send_line("SERR:2"); return; }
        snprintf(dbg, sizeof(dbg), "DBG EROL val=%lu stubbed", val);
        uart2_send_line(dbg);
        return;
    }

    /* ── ESYN ── */
    if (strcmp(cmd, "SYN") == 0) {
        if (val > 1000) { uart2_send_line("SERR:2"); return; }
        snprintf(dbg, sizeof(dbg), "DBG ESYN val=%lu stubbed", val);
        uart2_send_line(dbg);
        return;
    }

    /* Unknown command */
    uart2_send_line("SERR:1");
}

/* ── public API ───────────────────────────────────────────────────── */

void uart_rx_init(void)
{
    /* USART2 peripheral and GPIO already configured by WiFi_USART_Init().
     * We only need to enable the RXNE interrupt and set NVIC priority. */
    /* RXNEIE not used: WiFi_ProcessRx() in TIM1_CC_IRQHandler already
     * reads USART2 bytes and calls uart_rx_isr_byte() via
     * Process_WiFi_IncomingString(). No separate IRQ needed. */
}

void uart_rx_poll(void)
{
    /* SHBT heartbeat -- every 5000 ms.
     * This firmware uses u32_ST_captureActual (incremented in SysTick_Handler)
     * as the ms tick counter, not HAL uwTick which is never incremented. */
    static uint32_t last_hbt = 0;
    extern uint32_t u32_ST_captureActual;
    if (u32_ST_captureActual - last_hbt >= 5000) {
        uart2_send_line("SHBT:1");
        last_hbt = u32_ST_captureActual;
    }

    /* Process at most one queued line per call */
    char line[LINE_BUF_SIZE];
    if (queue_pop(line)) {
        dispatch(line);
    }
}
