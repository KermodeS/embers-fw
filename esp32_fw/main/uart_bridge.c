/*
 * uart_bridge.c — ESP-6
 *
 * Byte-pump between UART0 (FT232RL / minicom console) and UART2 (STM32
 * USART2 link). Spawned once on GOT_IP from wifi_sta.c.
 *
 * STM32 side: PA2=USART2_TX -> ESP32 GPIO16=UART2_RX
 *             PA3=USART2_RX <- ESP32 GPIO17=UART2_TX
 * Baud: 115200 8N1, no flow control.
 */

#include "uart_bridge.h"

#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ── configuration ─────────────────────────────────────────────────── */
#define BRIDGE_UART_NUM     UART_NUM_2
#define BRIDGE_TX_PIN        1          /* ESP32 GPIO1  -> STM32 PA3 RX (UART0 TXD pin) */
#define BRIDGE_RX_PIN        3          /* ESP32 GPIO3  <- STM32 PA2 TX (UART0 RXD pin) */
#define BRIDGE_BAUD         115200
#define BRIDGE_RX_BUF       1024
#define BRIDGE_CHUNK        64
#define BRIDGE_TIMEOUT_MS   100
#define BRIDGE_TASK_STACK   4096
#define BRIDGE_TASK_PRIO    5

static const char *TAG = "UART_BRIDGE";
static bool s_started = false;

/* ── RX task: UART2 -> UART0 stdout ───────────────────────────────── */
static void uart_bridge_rx_task(void *arg)
{
    uint8_t buf[BRIDGE_CHUNK];
    for (;;) {
        int len = uart_read_bytes(BRIDGE_UART_NUM, buf, sizeof(buf),
                                  pdMS_TO_TICKS(BRIDGE_TIMEOUT_MS));
        if (len > 0) {
            /* Write directly to UART0 FIFO — bypasses the VFS console
             * (NULL with CONFIG_ESP_CONSOLE_NONE). Polls TX FIFO count
             * to avoid overflow. UART0 base address 0x3FF40000. */
            volatile uint32_t *uart0_fifo  = (volatile uint32_t *)0x3FF40000;
            volatile uint32_t *uart0_stats = (volatile uint32_t *)0x3FF4001C;
            for (int i = 0; i < len; i++) {
                /* Wait until TX FIFO has space (TXFIFO_CNT < 127) */
                while (((*uart0_stats >> 16) & 0xFF) >= 126) {}
                *uart0_fifo = (uint32_t)buf[i];
            }
        }
    }
}



/* ── public API ────────────────────────────────────────────────────── */
esp_err_t uart_bridge_start(void)
{
    if (s_started) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Configure and install UART2 driver */
    const uart_config_t cfg = {
        .baud_rate  = BRIDGE_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(BRIDGE_UART_NUM, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(BRIDGE_UART_NUM,
                                 BRIDGE_TX_PIN, BRIDGE_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    /* RX buffer 1024; TX no driver queue (blocking writes) */
    ESP_ERROR_CHECK(uart_driver_install(BRIDGE_UART_NUM,
                                        BRIDGE_RX_BUF, 0, 0, NULL, 0));

    xTaskCreate(uart_bridge_rx_task, "bridge_rx", BRIDGE_TASK_STACK,
                NULL, BRIDGE_TASK_PRIO, NULL);
    /* TX task (keyboard->STM32) deferred to ESP-7 web UI.
     * Use uart_bridge_send_line() to write to STM32 from firmware. */

    s_started = true;

    /* Production: commands sent via uart_bridge_send_line() from web UI (ESP-7). */
    return ESP_OK;
}

void uart_bridge_send_line(const char *s)
{
    if (!s_started || !s) return;
    uart_write_bytes(BRIDGE_UART_NUM, s, strlen(s));
    uart_write_bytes(BRIDGE_UART_NUM, "\n", 1);
}
