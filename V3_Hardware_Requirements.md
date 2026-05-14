# V3 Hardware Requirements

**Project:** Embers Lighting — LLU V2 / V3  
**Document:** V3_Hardware_Requirements.md  
**Status:** Living document — append new requirements as V3-REQ-NNN  
**Repo:** https://github.com/KermodeS/embers-fw (git-tracked master)  
**Note:** A copy of this file must also be placed manually in the USB archive
"V3 Redesign" folder, as that folder is outside git and is where the PCB
designer's materials live.

---

## Purpose

This document records hardware requirements that the V3 PCB redesign must
satisfy, derived from firmware development work on the V2 hardware. Each
requirement is assigned a stable identifier (V3-REQ-NNN) and includes the
rationale, recommended implementation, and acceptance criterion.

Requirements are added here as they are discovered during V2 firmware sessions.
They do not constitute a complete V3 design specification — they are constraints
the V3 PCB designer must respect.

---

## V3-REQ-001 — Dedicated ESP32↔STM32 UART

**Status:** OPEN — pending V3 PCB design  
**Source:** ESP-6 + STM-RX1 firmware session, commit 9d3c72e  
**PROTO-001 ref:** Sections 2.3 and 2.4 (Rev 1.1)

### Requirement

The V3 PCB MUST route the ESP32↔STM32 PROTO-001 protocol link on a dedicated
UART, physically separate from the ESP32 console UART and the FT232RL
programming header.

### Rationale

The MPU V2 board has only one UART link between the ESP32 and the STM32. The
schematic (LLU_MidPower_V2.01.01) labels STM32 PA2 as WROOM_Tx_1 and STM32
PA3 as WROOM_Rx_1, connecting to the ESP32's UART0 pins: GPIO1 (U0TXD) and
GPIO3 (U0RXD). These are the same physical pins used for the ESP32's UART0
console and the FT232RL programming header. Three consumers on two wires.

The consequences confirmed on V2 hardware:

1. **ESP32→STM32 command direction works.** Hardware-verified by OpenOCD memory
   reads: EBRT:300 set u16_GlobalBrightMax to 300; EMOD:1 set u8_StateMaschine
   to 50 (SM_MODE_STROBOSCOPE); ERED:1000 produced a visible LED brightness
   increase. The STM32 PROTO-001 parser (uart_rx.c, commit 9d3c72e) is complete
   and functioning.

2. **STM32→ESP32 status direction is firmware-correct but unobservable on V2.**
   The STM32 emits SHBT:1 every 5 seconds (confirmed: last_hbt variable at
   0x200076d4 increments by ~5000 counts per interval under OpenOCD). The bytes
   leave STM32 PA2 but cannot reach the ESP32 RX pin against the opposing idle
   line state on GPIO1. The result is garbled/masked STM32 output at the minicom
   end.

3. **Boot contention.** During ESP32 boot, STM32 activity on PA2 can corrupt
   the ESP32's flash-read sequence, producing an intermittent "flash read err"
   requiring a second boot attempt. The ESP32 recovers automatically; this is
   symptomatic of the shared-wire architecture.

4. **Console workaround is not viable.** Disabling the ESP32 console
   (CONFIG_ESP_CONSOLE_NONE) to free GPIO1/GPIO3 removes all ESP32 boot/log
   visibility, which is unacceptable for ongoing development. That workaround
   was attempted and reverted (commit 28ac8a8).

This is a hardware architecture limitation of the MPU V2 board. The firmware on
both sides is correct. No firmware workaround is practical.

### Required V3 Wiring

| Signal path | ESP32 pin | STM32 pin | Notes |
|---|---|---|---|
| PROTO-001 protocol link (RX) | GPIO16 (UART2 RX) | PA2 (USART2_TX) | Dedicated trace, no other function |
| PROTO-001 protocol link (TX) | GPIO17 (UART2 TX) | PA3 (USART2_RX) | Dedicated trace, no other function |
| Console / programming (TX) | GPIO1 (UART0 TXD) | FT232RL header only | No connection to STM32 on V3 |
| Console / programming (RX) | GPIO3 (UART0 RXD) | FT232RL header only | No connection to STM32 on V3 |

GPIO16 and GPIO17 are the conventional ESP32-WROOM-32 UART2 pins. They are not
strapping pins and are not used by the flash interface. The V3 PCB designer must
confirm their availability against the final V3 schematic and module datasheet.

The ESP32 firmware (uart_bridge.c) is already written for UART2 on GPIO16/17.
Only a one-line pin constant change is needed when V3 hardware is available —
the firmware architecture requires no other modification.

### Acceptance Criterion

STM32 SHBT:1 heartbeat is cleanly received and displayed at the ESP32
console-side monitor without garbling, with the ESP32 console simultaneously
active and producing log output on GPIO1/GPIO3 to the FT232RL header.

---

*Add future requirements below as V3-REQ-002, V3-REQ-003, etc., following the
same format: Status, Source, Requirement, Rationale, Required Implementation,
Acceptance Criterion.*
