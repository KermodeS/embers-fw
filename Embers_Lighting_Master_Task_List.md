# Embers Lighting — Master Task List
**Last Updated:** 2026-04-27  
**Project:** LLU V2 / MPU_V2_Basic  
**Repo:** https://github.com/KermodeS/embers-fw | Branch: `dev/firmware-refactor`

---

## Priority Key
- 🔴 **Critical** — Safety, stability, or blocks other work
- 🟠 **High** — Product quality, known bugs
- 🟡 **Medium** — Robustness, architecture
- 🟢 **Low** — Polish, cosmetic, deferred
- ✅ **Done**
- ⏸ **Deferred** — PCB revision or resource dependency

---

## FIRMWARE — Safe to Do Now (No PCB Dependency)

| Priority | # | Task | Status | Notes |
|----------|---|------|--------|-------|
| ✅ | 1 | GCC/Makefile build system | Done | Replaces IAR EWARM |
| ✅ | 2 | IWDG watchdog | Done | Kicks in main loop |
| ✅ | 3 | I2C master hang bypass | Done | Non-blocking guard added |
| ✅ | 4 | LED brightness curves (gamma 2.2) | Done | Stage 2 — manual/DirectLightCtrl path |
| ✅ | 5 | Animation engine rewrite | Done | uwTick-driven fade, stage 3 |
| 🔴 | 6 | Fix duplicate index writes in all 5 brightness tables | **Next** | Bug affects all channels — safe isolated fix |
| 🔴 | 7 | Fix I2C2/I2C3 mismatch in TransmitReceive stop condition | Pending | Communication reliability bug |
| 🔴 | 8 | Fix duplicate `u8_NEC_TxCounter` declaration | Pending | Compiler hygiene — safe isolated fix |
| 🟠 | 9 | Fix manual mode brightness resets to zero when returning from animation modes | Pending | Confirmed bug in animation session |
| 🟠 | 10 | Add I2C timeout (`USE_TIMEOUT`, countdown in loops) | Pending | Prevents hang if I2C ever stalls |
| 🟡 | 11 | Convert RepeatFactor and FreqPeriod tables to `const` | Pending | Moves tables to flash, saves RAM |
| 🟡 | 12 | Translate Russian comments to English | Pending | Fold into each file as it is touched |
| 🟢 | 13 | Red zone discontinuity at low brightness | Pending | Cosmetic — 640Hz zone stepping |

---

## FIRMWARE — Untested Features (Test When Equipment Available)

| Priority | # | Task | Blocker |
|----------|---|------|---------|
| 🟠 | 14 | IR remote control via STM8 co-processor | Need IR remote |
| 🟠 | 15 | WiFi / network control (ESP32 softAP) | Need network test setup |
| 🟡 | 16 | Master/Slave device sync | Needs two units powered simultaneously |

---

## FIRMWARE — Defer Until PCB Revision

| Priority | # | Task | Reason |
|----------|---|------|--------|
| ⏸ | 17 | Thermal protection (LM75B) | Sensor on wrong board — sees ambient not LED temp |
| ⏸ | 18 | Enable ADC per-channel current sensing | Routing changes required in next PCB revision |
| ⏸ | 19 | ISR restructure / reduce global variable count | Wait until architecture stable post-revision |
| ⏸ | 20 | Channel state struct encapsulation | Same reason as above |
| ⏸ | 21 | WLED API compatibility layer on ESP32 | Separate ESP32 project — deferred |

---

## HARDWARE — PCB Revision (When Resources Allow)

| Priority | # | Task | Notes |
|----------|---|------|-------|
| 🔴 | 22 | Move LM75B sensor to heatsink/LED board | Required for thermal protection to be meaningful |
| 🔴 | 23 | Per-channel ADC current feedback routing fix | Required for current monitoring feature |
| 🔴 | 24 | Replace 74LVC1G08 AND gate with proper gate driver IC | Burned on MPU already — reliability risk |
| 🔴 | 25 | Overvoltage / reverse polarity protection | 12V incident documented — safety critical |
| 🟠 | 26 | Fixed-frequency buck converter / inductor swap | Eliminates PWM zone discontinuity permanently |
| 🟠 | 27 | Add second LM75B on LED heatsink board | Per hardware requirements doc |
| 🟠 | 28 | USB-C input replacing barrel connector | User experience — modern standard |
| 🟡 | 29 | 2-layer PCB, full SMD conversion | Simplifies manufacturing |
| 🟡 | 30 | Dedicated ground plane | Signal integrity improvement |
| 🟡 | 31 | Thermal vias + copper pour under LEDs | Better thermal path to heatsink |
| 🟡 | 32 | SWD test points retained and clearly labeled | Serviceability for firmware updates |

---

## BUSINESS

| Priority | # | Task | Notes |
|----------|---|------|-------|
| 🔴 | 33 | Solid beta prototype (current focus) | PCB revision + firmware baseline |
| 🟠 | 34 | Find PCB contractor to execute hardware requirements doc | Hardware requirements doc already written |
| 🟠 | 35 | Form LLC / basic business entity | One afternoon, ~$50-150 |
| 🟠 | 36 | Stripe account for payments | Before first sale |
| 🟡 | 37 | Announce beta to community | After solid prototype confirmed |
| 🟡 | 38 | 10-unit beta program | After PCB revision validated in field |

---

## Recommended Execution Order — Firmware (Next 6 Sessions)

```
Session 1:  Task 6  — Fix duplicate index writes (5 brightness tables)
Session 2:  Task 7  — Fix I2C2/I2C3 mismatch
Session 3:  Task 8  — Fix duplicate u8_NEC_TxCounter declaration
Session 4:  Task 9  — Fix manual mode brightness reset bug
Session 5:  Task 10 — Add I2C timeout (USE_TIMEOUT)
Session 6:  Task 11 — Convert tables to const
```

---

## Standard Context Block for Firmware Chats

```
PROJECT: Embers Lighting — LLU_V2 Firmware
USB source (read-only): /media/workstation/Samsung USB/Embers Lighting/Latest from Upwork/LLU_V2_0250/LLU_V2_Basic_0250/
Working copy: ~/embers_fw/ (all edits here)
GitHub: https://github.com/KermodeS/embers-fw
Active branch: dev/firmware-refactor
Primary target: MPU_V2_Basic/ (STM32F401RB, GCC/Makefile, LL drivers only, no HAL)
Shared library: LLU_V2_Common/
ESP32 firmware: softAP_0250/
When providing code changes: give exact paths under ~/embers_fw/, provide git commit messages, never edit USB source files.

FLASH COMMAND:
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program /home/workstation/embers_fw/MPU_V2_Basic/build/MPU_V2_Basic.bin verify reset exit 0x08000000"

RESTORE COMMAND:
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "init; halt; flash write_image erase /home/workstation/mpu_backup_original.bin 0x08000000 bin; reset; exit"
```
