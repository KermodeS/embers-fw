# ESP-2: NVS Initialisation and Storage Layer

**Date:** May 2026
**Branch:** main
**Status:** Complete, hardware-verified

## Objective

Implement the Non-Volatile Storage subsystem and a clean key-value
storage API for all future Embers subsystems (WiFi credentials,
device settings, provisioning state).

## What was created

### `main/nvs_storage.h`

Public API plus compile-time constants:

- Namespace: `"embers"`
- Six keys: `wifi_ssid`, `wifi_pass`, `dev_name`, `dev_role`, `brightness`, `provisioned`
- Default values (returned when key absent on first boot):
    - wifi_ssid: ""
    - wifi_pass: ""
    - dev_name: "embers-1"
    - dev_role: 0 (Standalone)
    - brightness: 800 (0-1000 scale)
    - provisioned: false

### `main/nvs_storage.c`

- `nvs_storage_init()` calls `nvs_flash_init()`, erases-and-retries
  on `ESP_ERR_NVS_NO_FREE_PAGES` or `ESP_ERR_NVS_NEW_VERSION_FOUND`,
  logs result.
- Internal helpers `_store_str`, `_get_str`, `_store_u8`, `_get_u8`,
  `_store_u16`, `_get_u16` handle handle open/commit/close.
- All getters fall back to default if the namespace or key is absent
  (returns ESP_OK with the default value written into the output).
- Provisioned bool is stored as uint8 (NVS has no native bool).
- All errors logged with tag `"NVS"`.

### `main/embers_main.c`

- `nvs_storage_init()` called from `app_main()` before main loop.
- `nvs_self_test()` runs once on boot: writes a test value to each
  key, reads it back, verifies match. Marked TEMPORARY in code
  comments — should be removed once NVS is trusted in production.

### `main/CMakeLists.txt`

- `nvs_storage.c` added to `SRCS`.

## Keys defined

| Key | Type | Default | Used for |
|---|---|---|---|
| `wifi_ssid` | string | `""` | WiFi station credentials |
| `wifi_pass` | string | `""` | WiFi station credentials |
| `dev_name` | string | `"embers-1"` | Device hostname / mDNS name |
| `dev_role` | uint8 | 0 (Standalone) | 0=Standalone, 1=Master, 2=Slave |
| `brightness` | uint16 | 800 | Default brightness, 0-1000 scale |
| `provisioned` | uint8 as bool | false | True once WiFi creds saved |

## Hardware verification

Build: clean, no warnings. Binary 0x2e740 bytes (190 KB, ~18% of 1 MB
factory partition).

Flash: clean, three "Hash of data verified" lines, auto-reset via RTS.

Boot log (excerpt):

    I (271) EMBERS: ========================================
    I (271) EMBERS:   Embers Lighting -- ESP32 Firmware
    I (271) EMBERS:   Build: May 11 2026 03:21:54
    I (271) EMBERS: ========================================
    I (281) EMBERS: MAC: 8C:AA:B5:8E:C7:78
    I (321) NVS: NVS initialised OK
    I (321) EMBERS: --- NVS self-test START ---
    I (321) EMBERS: PASS: nvs_store_wifi_ssid returned OK
    I (321) EMBERS: PASS: nvs_get_wifi_ssid returned OK
    I (331) EMBERS: PASS: wifi_ssid value matches
    I (331) EMBERS: PASS: nvs_store_wifi_password returned OK
    I (341) EMBERS: PASS: nvs_get_wifi_password returned OK
    I (341) EMBERS: PASS: wifi_password value matches
    I (341) EMBERS: PASS: nvs_store_device_name returned OK
    I (351) EMBERS: PASS: nvs_get_device_name returned OK
    I (351) EMBERS: PASS: device_name value matches
    I (361) EMBERS: PASS: nvs_store_device_role returned OK
    I (361) EMBERS: PASS: nvs_get_device_role returned OK
    I (371) EMBERS: PASS: device_role value matches
    I (371) EMBERS: PASS: nvs_store_brightness returned OK
    I (381) EMBERS: PASS: nvs_get_brightness returned OK
    I (381) EMBERS: PASS: brightness value matches
    I (391) EMBERS: PASS: nvs_store_provisioned returned OK
    I (391) EMBERS: PASS: nvs_get_provisioned returned OK
    I (401) EMBERS: PASS: provisioned value matches
    I (401) EMBERS: --- NVS self-test COMPLETE: ALL PASS ---
    I (5401) EMBERS: Heartbeat #1

18/18 PASS. Heartbeat continues after self-test as expected.

## Notes / lessons

- ESP-2's source code was written in a prior session but never built
  due to a CMake infinite loop (resolved separately as commit 9da22f9).
  After the loop fix the existing code compiled clean on first attempt.
- Self-test runs unconditionally on every boot. This is fine while
  the storage layer is unproven but should be removed once another
  subsystem (e.g. WiFi credentials) starts depending on NVS persisting
  across power cycles -- the test currently overwrites real data.
- The `provisioned` bool stored as uint8 is a deliberate ESP-IDF
  idiom (NVS has u8/u16/u32/i8/i16/i32/i64/u64/string/blob, no bool).

## Open issues for future sessions

- The self-test overwrites all keys on every boot. Remove or gate
  behind a build-time flag before any subsystem starts depending on
  NVS persistence (ESP-3 onwards).
- No defensive check on input length to `nvs_store_*` string functions.
  WiFi SSID can be up to 32 chars + null; password up to 64. Future
  WiFi provisioning code should clip user input to NVS limits before
  storing.
- NVS partition is the default 24 KB. Adequate for current keys; may
  need expansion if we later store device-specific calibration tables
  or per-channel brightness curves.

## Files touched

- `esp32_fw/main/nvs_storage.h` (new)
- `esp32_fw/main/nvs_storage.c` (new)
- `esp32_fw/main/embers_main.c` (added init call + self-test)
- `esp32_fw/main/CMakeLists.txt` (added nvs_storage.c to SRCS)
