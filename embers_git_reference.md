# Embers Lighting — Git Repository & Workflow Reference
*LLU_V2 Firmware — Claude Working Document*

---

## 1. Project Overview

The Embers Lighting LLU_V2 firmware controls a 5-channel LED art lighting system (Red, Green, Blue, UV, White) running on STM32F411 with WiFi via ESP32. This document defines the file locations, git repository structure, and standard workflow for all development sessions.

### Repository Locations

| Location | Path / URL |
|----------|-----------|
| USB source (read-only) | `/media/workstation/Samsung USB/Embers Lighting/Latest from Upwork/LLU_V2_0250/` |
| Local working copy | `~/embers_fw/` (= `/home/workstation/embers_fw/`) |
| GitHub remote | https://github.com/KermodeS/embers-fw |
| Active branch | `dev/firmware-refactor` |
| Stable branch | `main` |

> The USB drive is read-only reference only. All editing happens in `~/embers_fw/`. Changes are committed locally and pushed to GitHub.

---

## 2. Repository Structure

| Directory | Description |
|-----------|-------------|
| `MPU_V2_Basic/` | Mid Power Unit — primary development target (STM32F411) |
| `LPU_V2_Basic/` | Low Power Unit variant |
| `HPU_V2_Basic/` | High Power Unit variant |
| `LLU_V2_Common/` | Shared source: LightControl, BIOS, ESP32, IRDA, KBD |
| `TIM_PWMOutput/` | Timer/PWM reference project (IAR example base) |
| `softAP_0250/` | ESP32 WiFi firmware (ESP-IDF, separate build system) |
| `LLU_V2_IRDA_0250/` | STM8 IR co-processor (not in working copy — separate project) |

### Key Source Files

| File | Description |
|------|-------------|
| `MPU_V2_Basic/Src/main.c` | System init, LED lookup tables, ADC, I2C, main loop |
| `MPU_V2_Basic/Inc/main.h` | Channel max levels, pin defines, function prototypes |
| `MPU_V2_Basic/Src/stm32f4xx_it.c` | Interrupt handlers (SysTick, DMA, I2C, ADC) |
| `LLU_V2_Common/LLU_V2_LightControl.c` | PWM channel init, state machine, Set*Level functions |
| `LLU_V2_Common/LLU_V2_LightControl.h` | All mode defines, state machine constants, public API |
| `LLU_V2_Common/LLU_V2_ESP32.c/.h` | UART comms with ESP32 WiFi module |
| `LLU_V2_Common/LLU_V2_BIOS.c/.h` | Flash-based settings persistence |
| `LLU_V2_Common/LLU_V2_IRDA.c/.h` | IR remote receive processing |
| `LLU_V2_Common/LLU_V2_KBD.c/.h` | Physical button handling via I2C |

---

## 3. Hardware Summary

| Parameter | Value |
|-----------|-------|
| MCU | STM32F411 @ 84 MHz (LL drivers only, no HAL) |
| WiFi module | ESP32-WROOM-32 (UART bridge) |
| LED channels | R (TIM11_CH1 PB9), G (TIM3_CH3 PB0), B (TIM2_CH2 PA1), UV (TIM10_CH1 PB8), W (TIM4_CH2 PB7) |
| LED driver | Discrete buck converter: MOSFET + inductor + current sense |
| PWM zones | 381 Hz / 4 kHz / 16 kHz — variable frequency for silent operation |
| Current sensing | ADC1 channels 5/6/7/14/15 (UV/B/G/R/W) |
| I2C | I2C3 (PA8 SCL, PC9 SDA) — keyboard + IRDA slave |
| Toolchain | IAR EWARM |
| Programmer | ST-LINK V2 |
| Testing equipment | Extech EX330 multimeter, Strumen LX1010B lux meter |

---

## 4. Known Issues — Baseline Firmware

Identified in code review of factory firmware (git commit `e4f739b` on `main`). These define the refactoring backlog.

| Issue | Description |
|-------|-------------|
| No watchdog | WWDG in headers but never configured. Device hangs permanently on I2C lockup with no recovery. |
| Blocking I2C loops | `while(!ubMasterTransferComplete){}` — `USE_TIMEOUT` never defined. Missing ACK hangs system. |
| Duplicate index bug | `u16_*Refference_MP[]` writes index 133 twice, skipping next entry. All 5 channels affected. Brightness curve discontinuity. |
| I2C2/I2C3 mismatch | `TransmitReceive()` calls `GenerateStopCondition(I2C2)` but firmware uses I2C3. Reads never terminate properly. |
| ADC monitoring disabled | `Configure_ADC_DMA` / `Configure_ADC` / `Activate_ADC` commented out in `main()`. Current sensing inactive. |
| Duplicate variable | `u8_NEC_TxCounter` declared twice in `main.c`. |
| No thermal protection | LM75B sensors planned but no thermal shutdown logic. |
| 50+ global variables | No struct encapsulation. Hidden coupling between modules. |
| Tables not const | RepeatFactor and FreqPeriod arrays are mutable globals consuming SRAM. Should be `const` in flash. |
| PWM discontinuities | 3-zone frequency causes visual glitches at DCM/CCM boundary in inductor. |

---

## 5. Git Workflow

### Daily Development

```bash
cd ~/embers_fw
# edit files in MPU_V2_Basic/Src/ or LLU_V2_Common/
git add -p                    # review changes interactively
git commit -m "fix: description"
git push
```

### Promoting to main (after hardware validation)

```bash
git checkout main
git merge dev/firmware-refactor
git push
git checkout dev/firmware-refactor
```

### Creating a Feature Branch

```bash
git checkout -b feature/watchdog-timer
# ... make changes, commit ...
git checkout dev/firmware-refactor
git merge feature/watchdog-timer
```

### Commit Message Convention

| Prefix | Use for |
|--------|---------|
| `fix:` | Bug fixes |
| `feat:` | New features |
| `refactor:` | Code restructuring without behaviour change |
| `test:` | Hardware test results or test code |
| `docs:` | Documentation updates |

---

## 6. Refactoring Priority Order

Ordered by safety and impact. Each step must be hardware-validated before the next begins.

| Priority | Task |
|----------|------|
| 1 — Safe, isolated | Fix duplicate index writes in all 5 brightness reference tables |
| 2 — Safe, isolated | Fix I2C2/I2C3 mismatch in TransmitReceive stop condition |
| 3 — Safe, isolated | Fix duplicate `u8_NEC_TxCounter` declaration |
| 4 — Moderate | Add I2C timeout (`USE_TIMEOUT`, countdown in loops) |
| 5 — Moderate | Enable and validate ADC current sensing |
| 6 — Moderate | Add IWDG watchdog with kick points in main loop |
| 7 — Architecture | Convert RepeatFactor and FreqPeriod tables to `const` |
| 8 — Architecture | Encapsulate channel state into structs |
| 9 — Architecture | Restructure ISRs, reduce global variable count |

---

## 7. Standard Prompt Statement for Claude

Copy and paste the following at the start of every new firmware chat session:

```
PROJECT: Embers Lighting — LLU_V2 Firmware.
USB source (read-only): /media/workstation/Samsung USB/Embers Lighting/Latest from Upwork/LLU_V2_0250/LLU_V2_Basic_0250/
Working copy: ~/embers_fw/ (all edits here).
GitHub: https://github.com/KermodeS/embers-fw
Active branch: dev/firmware-refactor
Primary target: MPU_V2_Basic/ (STM32F411, IAR EWARM, LL drivers only, no HAL)
Shared library: LLU_V2_Common/
ESP32 firmware: softAP_0250/
When providing code changes: give exact paths under ~/embers_fw/, provide git commit messages, never edit USB source files.
```
