# ESP-0 — ESP-IDF Toolchain Setup & Hello World Baseline

**Project:** Embers Lighting — LLU V2 ESP32 Firmware  
**Session:** ESP-0  
**Date:** [FILL IN]  
**Hardware:** ESP32-WROOM-32 on MPU V2 board  
**MAC:** 8C:AA:B5:8E:C7:78  

---

## 1. ESP-IDF Version and Installation Path

| Item | Value |
|------|-------|
| ESP-IDF version | [FILL IN — output of `idf.py --version`] |
| Installation path | `~/esp/esp-idf/` |
| Toolchain path | `~/.espressif/tools/xtensa-esp-elf/` |
| Python venv | `~/.espressif/python_env/` |
| Target | `esp32` |

Activate environment in any new terminal:
```bash
. ~/esp/esp-idf/export.sh
```

Optional alias in `~/.bashrc`:
```bash
alias idf=". ~/esp/esp-idf/export.sh"
```

---

## 2. Project Location

```
~/embers_fw/esp32_fw/
```

Copied from ESP-IDF hello_world template:
```bash
cp -r ~/esp/esp-idf/examples/get-started/hello_world ~/embers_fw/esp32_fw
```

This is the production firmware directory. All ESP32 firmware goes here going forward.

---

## 3. Build Command

```bash
cd ~/embers_fw/esp32_fw
idf.py build
```

Build output: `~/embers_fw/esp32_fw/build/hello_world.bin`

---

## 4. Flash Command

```bash
idf.py -p /dev/ttyUSB0 flash
```

**Flash method required:** [FILL IN — "automatic (no manual intervention needed)" OR "manual boot mode: hold IO0, press EN, release IO0, then run flash command"]

---

## 5. Monitor Command

```bash
minicom -D /dev/ttyUSB0 -b 115200
```

Exit minicom: `Ctrl-A` then `X`

> **Do not use `idf.py monitor`** — minicom is the standard monitoring tool for all Embers sessions for consistency.

---

## 6. Confirmed Boot Output

```
[PASTE EXACT minicom OUTPUT HERE — the "Hello world!" lines and restart countdown]
```

---

## 7. Issues Encountered and Resolutions

| Issue | Resolution |
|-------|-----------|
| [FILL IN or "None"] | |

---

## 8. Open Issues for Future Sessions

- [ ] `sdkconfig` not yet tuned for production (hello_world defaults, serial output baud, watchdog settings)
- [ ] `esp32_fw/` still contains hello_world template code — to be replaced with Embers application firmware starting ESP-1
- [ ] Serial port (`/dev/ttyUSB0`) requires user to be in `dialout` group — verify: `groups $USER | grep dialout`; if missing: `sudo usermod -aG dialout $USER` then log out/in
- [ ] Flash method (auto vs manual boot mode) to be documented for new contributors once confirmed
- [ ] `softAP_0250/` contractor firmware remains in repo root as read-only reference — do not delete

---

## 9. Power Sequence (Critical — Record for All Future Sessions)

1. Plug FT232RL USB into workstation **first**
2. Apply 12V barrel connector to MPU board **second**
3. Never reverse — ESP32 browns out if UART lines are live before board power

---

## 10. Next Session

**ESP-1:** UART hello — ESP32 sends a single UART message to STM32 on boot, STM32 echoes it back. Validates the inter-processor communication path before any application logic.

Reference: PROTO-001 UART Command Protocol Specification (in project docs).
