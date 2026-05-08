# ESP1 — Embers App Scaffold

**Project:** Embers Lighting — LLU V2 ESP32 Firmware  
**Session:** ESP-1  
**Date:** 2026-05-08  
**Branch:** main  
**Status:** Hardware-confirmed ✅

---

## Objective

Replace the hello_world template with the named, structured Embers application
skeleton. No functional subsystems — this is the clean foundation all future
sessions build on.

---

## Changes Made

### 1. sdkconfig — Flash size (via menuconfig)

**Location:** `esp32_fw/sdkconfig`  
**Change:** Serial flasher config → Flash size → **8 MB**  
**Reason:** Board has 8MB flash (ESP32-WROOM-32 on MPU V2). Default was 2MB,
causing a build-time warning on every build.  
**Effect:** Warning eliminated. Partition table now correctly sized for hardware.

### 2. sdkconfig — Default log level (via menuconfig)

**Location:** `esp32_fw/sdkconfig`  
**Change:** Component config → Log output → Default log verbosity → **Info**  
**Reason:** Debug level produces excessive framework noise that obscures
application output. Info is appropriate for production monitoring.

### 3. main/hello_world_main.c → main/embers_main.c

**Action:** File renamed (deleted old, created new)  
**Reason:** File name must reflect the actual project.

**New file contents:**
- File header: project, version, hardware, toolchain, build/flash commands
- `app_main()`: reads MAC via `esp_read_mac()`, logs startup banner
- Startup banner: project name, version, MAC address
- Subsystem TODO stubs: NVS, WiFi, mDNS, WebSocket, UART-to-STM32, ESP-NOW
- Main loop: `vTaskDelay(5000ms)` + heartbeat log with incrementing counter
- Log tag: `"EMBERS"` throughout

### 4. main/CMakeLists.txt

**Location:** `esp32_fw/main/CMakeLists.txt`  
**Change:** `SRCS` updated from `"hello_world_main.c"` to `"embers_main.c"`  
**Reason:** Required for build system to find the renamed source file.

---

## Build Result
Build complete — no warnings, no errors
ESP-IDF v5.5.4
SPI Flash Size: 8MB confirmed in bootloader

---

## Confirmed minicom Output

I (27) boot: ESP-IDF v5.5.4 2nd stage bootloader
I (27) boot: compile time May  8 2026 12:07:55
I (29) boot: chip revision: v1.0
I (39) boot.esp32: SPI Flash Size : 8MB
I (254) main_task: Calling app_main()
I (254) EMBERS: =================================
I (254) EMBERS: Embers Lighting — LLU V2
I (254) EMBERS: ESP32 Firmware v0.1.0
I (254) EMBERS: MAC: 8C:AA:B5:8E:C7:78
I (264) EMBERS: =================================
I (5264) EMBERS: Heartbeat #1 -- idle, awaiting subsystems
I (10264) EMBERS: Heartbeat #2 -- idle, awaiting subsystems
I (15264) EMBERS: Heartbeat #3 -- idle, awaiting subsystems

---

## Open Issues for Future Sessions

ESP-2: NVS init — required before WiFi credentials can be stored or read
ESP-3: WiFi station mode and captive portal provisioning on first boot
ESP-4: mDNS device announcement
ESP-5: WebSocket server for web UI
ESP-6: UART driver to STM32 per PROTO-001 spec
ESP-7: ESP-NOW inter-device sync and hub role
Note: top-level CMakeLists.txt still says project(hello_world) — rename to project(embers_fw) next session

---

## Git Commit

feat: Embers app scaffold, 8MB flash config, startup banner (esp32_fw/)

Files changed:
esp32_fw/main/embers_main.c (new)
esp32_fw/main/CMakeLists.txt (updated SRCS)
esp32_fw/sdkconfig (flash size, log level)
esp32_fw/ESP1_App_Scaffold.md (this file)
