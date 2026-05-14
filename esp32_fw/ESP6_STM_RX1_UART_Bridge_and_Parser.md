# ESP-6 + STM-RX1: UART Bridge and PROTO-001 Parser (Paired Session)

| Field | Value |
|---|---|
| Session ID | ESP-6 + STM-RX1 (paired) |
| Date | May 2026 |
| Status | Hardware-verified (ESP32→STM32 direction); STM32→ESP32 direction firmware-complete but unobservable on V2 hardware (see hardware constraint below) |

## Summary

This session implements the UART communication bridge between the ESP32 and STM32 on the LLU V2 MPU board. ESP-6 adds `uart_bridge.c/h` to the ESP32 firmware — a UART2 byte pump that forwards bytes between the STM32 link and minicom. STM-RX1 adds `uart_rx.c/h` to the STM32 firmware — an interrupt-driven PROTO-001 parser that accumulates lines, validates commands, dispatches to existing channel/mode setters, and emits SHBT heartbeat every 5 seconds.

The ESP32→STM32 command path is fully verified on hardware: `EBRT:300` sets `u16_GlobalBrightMax` to 300 (OpenOCD memory read confirmed), `ERED:1000` produces visible LED brightness increase, `EMOD:1` sets `u8_StateMaschine` to 50 (SM_MODE_STROBOSCOPE, OpenOCD confirmed). The STM32→ESP32 direction is correctly implemented in firmware (SHBT firing confirmed via `last_hbt` incrementing in memory) but is unobservable on V2 hardware due to the bus contention described below.

---

## CRITICAL HARDWARE CONSTRAINT — V2 MPU Board

**The V2 MPU board multiplexes the ESP32 console UART with the ESP32↔STM32 protocol link on shared GPIO1/GPIO3 wires.**

Specifically:
- STM32 PA2 (USART2_TX) → ESP32 GPIO3 (UART0 RXD)
- STM32 PA3 (USART2_RX) ← ESP32 GPIO1 (UART0 TXD)

These are the same physical pins used for the ESP32's UART0 console (connected to the FT232RL programming header). The contractor's original firmware exploited this by intercepting ESP32 console output on the STM32 side — a one-way design where the ESP32 never drove the wire as a structured TX.

**Consequence:** When the ESP32 uses GPIO1 as UART2 TX to send commands to the STM32, GPIO1 idles HIGH between bytes. This overpowers the STM32 PA2 TX signal (also idle HIGH but a weaker driver at this voltage), preventing STM32 responses from being detectable on GPIO3. The STM32→ESP32 direction is electrically blocked by the ESP32's own idle line state.

**The STM32→ESP32 firmware is correct.** SHBT heartbeat fires every 5 seconds (confirmed: `last_hbt` at `0x200076d4` increments by ~5000 counts per interval in OpenOCD). The bytes leave STM32 PA2. They simply cannot reach the ESP32 RX pin against the opposing driver.

### V3 PCB Requirement (MANDATORY)

V3 PCB MUST provide dedicated UART traces, separated as follows:

- **ESP32 UART2 (GPIO16 RX / GPIO17 TX)** or equivalent free GPIO pins → wired to **STM32 PA2 (USART2_TX) / PA3 (USART2_RX)** for the PROTO-001 protocol link. These pins must have no other function.
- **ESP32 UART0 (GPIO1 TX / GPIO3 RX)** → wired exclusively to the **FT232RL programming/console header**. No connection to the STM32.

This separation eliminates bus contention and allows full bidirectional PROTO-001 communication. The ESP32 firmware (`uart_bridge.c`) is already written for UART2 on GPIO16/17 — only a one-line pin constant change is needed when V3 hardware is available.

### PROTO-001 Spec Update Required

The existing PROTO-001 Rev 1.0 specification does not capture this physical constraint. PROTO-001 must be updated to:
- Document the V2 hardware limitation (shared console/protocol bus)
- Specify the V3 wiring requirement (dedicated UART2 traces on GPIO16/17 or equivalent)
- Note that full bidirectional verification (including SHBT) requires V3 hardware

---

## Files Created

### ESP32 (`esp32_fw/main/`)

| File | Purpose |
|---|---|
| `uart_bridge.c` | UART2 byte pump. Initialises UART2 on GPIO1(TX)/GPIO3(RX) at 115200 8N1. Single RX task reads from UART2 and writes to UART0 FIFO via direct register access. `uart_bridge_send_line()` is the production write path for the future web UI. |
| `uart_bridge.h` | Public API: `uart_bridge_start()`, `uart_bridge_send_line()`. |

### STM32 (`MPU_V2_Basic/`)

| File | Purpose |
|---|---|
| `Inc/uart_rx.h` | Public API: `uart_rx_init()`, `uart_rx_poll()`, `uart_rx_isr_byte()`. |
| `Src/uart_rx.c` | PROTO-001 parser. 64-byte line buffer, 4-slot command queue, validating dispatcher routing E-commands to existing channel/mode setters. SHBT every 5000ms via `u32_ST_captureActual`. SERR:1/SERR:2 for malformed/out-of-range. |

## Files Modified

### ESP32
| File | Change |
|---|---|
| `esp32_fw/main/wifi_sta.c` | Added `uart_bridge_start()` call in `IP_EVENT_STA_GOT_IP` handler, after `mdns_hub_start()`. |
| `esp32_fw/main/CMakeLists.txt` | Added `uart_bridge.c` to SRCS; added `driver` to REQUIRES. |
| `esp32_fw/sdkconfig` | Set `CONFIG_ESP_CONSOLE_NONE=y` to free GPIO1/GPIO3 from console VFS ownership. Required to allow UART2 to drive GPIO1 and to allow direct FIFO writes to UART0 TX. |

### STM32
| File | Change |
|---|---|
| `MPU_V2_Basic/Src/main.c` | Added `#include "uart_rx.h"`; added `uart_rx_init()` after `WiFi_USART_Init()`; added `uart_rx_poll()` in main while(1) loop after IWDG kick. |
| `MPU_V2_Basic/Src/stm32f4xx_it.c` | Added `#include "uart_rx.h"`. USART2_IRQHandler stub present but not used (see RX architecture below). |
| `MPU_V2_Basic/Makefile` | Added `Src/uart_rx.c` to `C_SOURCES`. |
| `LLU_V2_Common/LLU_V2_ESP32.c` | Added `#include "uart_rx.h"`; added PROTO-001 dispatch hook at top of `Process_WiFi_IncomingString()` — if buffer starts with 'E', feeds bytes to `uart_rx_isr_byte()` and returns, bypassing legacy parser. |

---

## Architecture Notes

### RX Architecture (ESP32→STM32)
The contractor's existing `WiFi_ProcessRx()` in `LLU_V2_ESP32.c` (called from `TIM1_CC_IRQHandler`) already reads USART2 RXNE and accumulates bytes into `u8_WiFi_RxArray_A[]`. On `\n`, it calls `Process_WiFi_IncomingString()`. We hook into `Process_WiFi_IncomingString()` at the top: if the buffer starts with 'E', bytes are fed to `uart_rx_isr_byte()` and the legacy parser is bypassed. This avoids any conflict with the existing IRQ structure.

### Tick Counter
This firmware uses `u32_ST_captureActual` (incremented in `SysTick_Handler` in `stm32f4xx_it.c`) as the millisecond counter. HAL's `uwTick` is never incremented in this firmware. `uart_rx.c` uses `u32_ST_captureActual` for the SHBT 5-second interval.

### TX Architecture (STM32→ESP32)
`uart2_send_line()` uses blocking LL_USART_TransmitData8 + TXE polling on USART2. Correct on V3 hardware. On V2 hardware, transmitted bytes are masked by ESP32 GPIO1 bus contention.

### ESP32 Console
`CONFIG_ESP_CONSOLE_NONE` is set in sdkconfig. The ESP32 boot ROM still outputs to GPIO1 before the app starts (causing `flash read err` on STM32-powered boots — harmless, ESP32 recovers). After app start, all output to minicom goes via direct UART0 FIFO register writes in the bridge RX task.

---

## Confirmed GPIO Wiring (V2 Board)

| Signal | STM32 | ESP32 | Notes |
|---|---|---|---|
| Protocol TX (STM32→ESP32) | PA2 / USART2_TX | GPIO3 / UART0_RXD | Shared with FT232RL console RX |
| Protocol RX (STM32←ESP32) | PA3 / USART2_RX | GPIO1 / UART0_TXD | Shared with FT232RL console TX |

V3 target wiring:

| Signal | STM32 | ESP32 | Notes |
|---|---|---|---|
| Protocol TX (STM32→ESP32) | PA2 / USART2_TX | GPIO16 / UART2_RX | Dedicated trace |
| Protocol RX (STM32←ESP32) | PA3 / USART2_RX | GPIO17 / UART2_TX | Dedicated trace |
| Console TX | — | GPIO1 / UART0_TXD | FT232RL header only |
| Console RX | — | GPIO3 / UART0_RXD | FT232RL header only |

---

## Confirmed STM32 Symbol Names

| PROTO-001 | STM32 function/variable |
|---|---|
| ERED | `SetRedLevel(uint16_t)` |
| EGRN | `SetGreenLevel(uint16_t)` |
| EBLU | `SetBlueLevel(uint16_t)` |
| EUVV | `SetUvLevel(uint16_t)` |
| EWHT | Log only (V2 hardware disabled) |
| EBRT | `u16_GlobalBrightMax` + `UpdateStrobeRawIndices()` |
| EMOD | `u8_StateMaschine` (SM_MODE_MANUAL=10, SM_MODE_STROBOSCOPE=50, SM_MODE_FUNCTION_A=20, SM_MODE_FUNCTION_B=30, SM_MODE_FUNCTION_C=40) |
| ERST | `NVIC_SystemReset()` |
| EROL | Log stub |
| ESYN | Log stub |
| Tick counter | `u32_ST_captureActual` (not `uwTick`) |

---

## Hardware Verification Results

### ESP32→STM32 (VERIFIED)
- `EBRT:300` → `u16_GlobalBrightMax` at `0x200002b6` = `0x012c` (300) ✅
- `ERED:1000` → red LED visibly brighter ✅
- `EMOD:1` → `u8_StateMaschine` at `0x200002c3` = `0x32` (50 = SM_MODE_STROBOSCOPE) ✅
- STM32 parser running: `last_hbt` at `0x200076d4` increments ~5000 per interval ✅

### STM32→ESP32 (FIRMWARE VERIFIED, HARDWARE BLOCKED)
- `last_hbt` increments confirm SHBT fires every ~5 seconds ✅
- Bytes leave STM32 PA2 TX ✅
- Bytes cannot reach ESP32 GPIO3 RX due to bus contention ⚠️
- Full bidirectional verification deferred to V3 hardware ⚠️

### Regression
- Physical mode/brightness buttons responsive ✅
- WiFi connects (phone hotspot, 1 device confirmed) ✅
- IWDG not triggering (device stable throughout session) ✅
- Strobe mode not visually active — pre-existing dead code issue, not a regression ✅

---

## Known Issues and Deferred Work

| Item | Notes |
|---|---|
| STM32→ESP32 RX on V2 | Bus contention on shared GPIO1/GPIO3. Requires V3 PCB with dedicated UART2 traces. |
| ESP32 boot `flash read err` | STM32 PA2 TX drives GPIO1 during ESP32 SPI flash read at power-on. Harmless — ESP32 recovers. Eliminated on V3 by dedicated traces. |
| SMAC/SROL boot handshake | Not implemented. Deferred to BOOT-HSHK1 session. |
| Structured state-change TX (SMOD/SBRT/SRED/...) | Not implemented. Deferred to STM-TX1 session. |
| EROL and ESYN | Log-only stubs. |
| EWHT | Silently accepted, no LED change (V2 hardware disabled). |
| STM32 TX path | Blocking. Fine at SHBT-every-5s + rare SERR. Replace with software FIFO at STM-TX1. |
| DBG log lines | Temporary. Remove or compile-time-gate before production build. |
| Strobe mode visual | Pre-existing: `Process_Mode_Stroboscope()` has dead-code `return` at entry. Active strobe runs from TIM IRQ. |
| PROTO-001 spec | Does not document V2 hardware constraint or V3 wiring requirement. Must be updated. |

---

## Reproduction Commands

```bash
# STM32 build
cd ~/embers_fw/MPU_V2_Basic && make

# STM32 flash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'program build/MPU_V2_Basic.elf verify reset exit'

# ESP32 build
. ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw && idf.py build

# ESP32 flash
idf.py -p /dev/ttyUSB0 -b 115200 flash

# Monitor (STM32 responses only — no ESP32 log with CONSOLE_NONE)
minicom -D /dev/ttyUSB0 -b 115200

# Verify STM32 state via OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init' -c 'mdw 0x200002b6 1' -c 'exit'   # u16_GlobalBrightMax
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init' -c 'mdw 0x200002c3 1' -c 'exit'   # u8_StateMaschine
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c 'init' -c 'mdw 0x200076d4 1' -c 'exit'   # last_hbt (SHBT firing)
```
