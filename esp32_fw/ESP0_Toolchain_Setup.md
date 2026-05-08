# ESP-0 — ESP-IDF Toolchain Setup & Hello World Baseline

**Project:** Embers Lighting — LLU V2 ESP32 Firmware  
**Session:** ESP-0  
**Date:** 2026-05-08  
**Hardware:** ESP32-WROOM-32 on MPU V2 board  
**MAC:** 8C:AA:B5:8E:C7:78  

---

## 1. ESP-IDF Version and Installation Path

| Item | Value |
|------|-------|
| ESP-IDF version | v5.5.4 |
| Installation path | `~/esp/esp-idf/` |
| Toolchain | xtensa-esp-elf GCC 14.2.0 |
| Python venv | `~/.espressif/python_env/idf5.5_py3.10_env/` |
| Target | `esp32` |

Activate the ESP-IDF environment in any new terminal before using `idf.py`:

```bash
. ~/esp/esp-idf/export.sh
```

This sets `IDF_PATH`, adds `idf.py` to PATH, and activates the Python venv.
**This must be run once in every new terminal session.** Without it, `idf.py` will not be found.

Optional convenience alias — add to `~/.bashrc` so you can just type `idf`:

```bash
alias idf=". ~/esp/esp-idf/export.sh"
```

---

## 2. Project Location

Production firmware directory:

```
~/embers_fw/esp32_fw/
```

Created by copying the ESP-IDF hello_world template:

```bash
cp -r ~/esp/esp-idf/examples/get-started/hello_world ~/embers_fw/esp32_fw
```

All ESP32 firmware development goes in `esp32_fw/` from this point forward.  
`softAP_0250/` in the repo root is the original contractor firmware — read-only reference, never edit.

---

## 3. Build Command

```bash
cd ~/embers_fw/esp32_fw
idf.py build
```

First build takes several minutes (compiles all ESP-IDF components). Subsequent builds are incremental.

Build output location: `~/embers_fw/esp32_fw/build/hello_world.bin`  
Binary size at ESP-0: 136,528 bytes (87% of 1MB partition free)

The `build/` directory is excluded from git via `.gitignore` — it is large and fully reproducible.  
Only source files and `sdkconfig` are committed.

---

## 4. Flash Command

```bash
idf.py -p /dev/ttyUSB0 flash
```

**Always specify `-p /dev/ttyUSB0` explicitly.** Without it, esptool scans all serial ports and
will attempt `/dev/ttyS0` through `/dev/ttyS31` before giving up, wasting time and producing
confusing output.

**Flash method: fully automatic** — no manual boot mode required on this board.  
The FT232RL RTS pin is wired to the ESP32 EN (reset) pin. esptool toggles RTS to reset the chip
into bootloader mode automatically before every flash. No button presses needed.

Flash baud rate: esptool negotiates up to 460800 automatically from the initial 115200 connection.

---

## 5. Monitor Command

```bash
minicom -D /dev/ttyUSB0 -b 115200
```

Exit minicom: `Ctrl-A` then `X`

**Always use minicom for monitoring — do not use `idf.py monitor`.** Minicom is the consistent
monitoring tool across all Embers sessions (both ESP32 and STM32 workflows).

If the screen is blank after starting minicom, press the **EN** button on the board to trigger
a reset. Output should appear within 2 seconds.

---

## 6. Power Sequence (Critical)

This must be followed every session. Reversing the order causes the ESP32 to brown out.

1. Plug FT232RL USB into workstation **first**
2. Apply 12V barrel connector to MPU board **second**
3. Never reverse this order

The ESP32 is powered from the MPU board's 3.3V rail. If the FT232RL UART lines are live (driven
by the workstation) before the board is powered, the ESP32 sees voltage on its IO pins before
its power rail is up — this can cause undefined behaviour or damage over time.

---

## 7. Confirmed Boot Output (Hardware Verified 2026-05-08)

```
ets Jun  8 2016 00:22:57
rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
I (27) boot: ESP-IDF v5.5.4 2nd stage bootloader
I (27) boot: compile time May  8 2026 11:43:24
I (27) boot: Multicore bootloader
I (29) boot: chip revision: v1.0
I (164) cpu_start: cpu freq: 160000000 Hz
I (255) main_task: Started on CPU0
I (265) main_task: Calling app_main()
Hello world!
```

Chip confirmed: ESP32-D0WDQ6 revision v1.0, dual core, 240MHz, 2MB flash (see open issue #1), 40MHz crystal.

---

## 8. Issues Encountered and Resolutions

| # | Issue | Resolution |
|---|-------|------------|
| 1 | First flash attempt failed — `/dev/ttyUSB0` reported "multiple access on port" | minicom was holding the port open from a previous monitoring session. Killed with `sudo fuser -k /dev/ttyUSB0`. Always close minicom before flashing. |
| 2 | Build output text was pasted into terminal as commands after build completed | Cosmetic only — build had already finished successfully. No effect on firmware. |

---

## 9. Hardware and Environment Notes

- `/dev/ttyUSB0` is owned by group `plugdev` on this system (not `dialout` as typical on Ubuntu)
- User is in `dialout` group — sufficient for port access without `sudo`
- Auto-reset via RTS confirmed working — no IO0/EN manual button sequence needed
- If port access is ever denied, also add to `plugdev`: `sudo usermod -aG plugdev $USER`

---

## 10. Open Issues for Future Sessions

- [ ] **Flash size mismatch**: Boot log warns `Detected size(8192k) larger than size in binary image header(2048k)`. Chip has 8MB flash but sdkconfig defaults to 2MB. Fix in ESP-1: `idf.py menuconfig` → Serial flasher config → Flash size → 8MB.
- [ ] **sdkconfig not tuned**: All settings are hello_world template defaults. Production sdkconfig (flash size, log levels, UART config, watchdog timers, partition table) to be configured in ESP-1.
- [ ] **hello_world template code**: `esp32_fw/main/hello_world_main.c` is template code. To be replaced with Embers application scaffold in ESP-1.
- [ ] **pytest_hello_world.py**: Template test file committed with the project. Remove or replace when application code replaces hello_world.

---

## 11. Next Session: ESP-1

**Goal:** Replace hello_world template with minimal Embers application scaffold.

Tasks:
- Rename `hello_world_main.c` to `embers_main.c`, update `CMakeLists.txt`
- Configure sdkconfig: flash size 8MB, log level INFO, UART0 console at 115200
- Implement bare `app_main()` with ESP_LOGI startup message: `"Embers LLU V2 — ESP32 firmware init"`
- Confirm clean boot with no warnings in minicom
- Commit

Reference: PROTO-001 UART Command Protocol Specification (in project docs) — the inter-processor
UART driver (USART2, 115200 8N1) is roadmap item ESP-10 but the protocol is fully specified and
ready for implementation.
