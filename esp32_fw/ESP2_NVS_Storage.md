# ESP2 — NVS Storage Layer

**Session:** ESP-2  
**Date:** 2026-05-08  
**Branch:** main  
**Status:** Pending hardware confirmation

---

## 1. What was created / changed

| File | Action | Notes |
|------|--------|-------|
| `main/nvs_storage.h` | Created | Public API — all NVS keys, defaults, function prototypes |
| `main/nvs_storage.c` | Created | Full implementation with internal helpers |
| `main/embers_main.c` | Modified | NVS init added before main loop; self-test added |
| `main/CMakeLists.txt` | Modified | `nvs_storage.c` added to SRCS list |

### Changes to embers_main.c

- `nvs_storage_init()` called from `app_main()`, replacing the `// TODO: NVS` stub.
- `nvs_self_test()` function added — marked TEMPORARY, to be removed once hardware-confirmed.
- Self-test is skipped if NVS init fails (safe degradation).

---

## 2. NVS keys defined

All keys live under namespace **`"embers"`**.

| Key constant | NVS key string | Type | Default (first boot) |
|---|---|---|---|
| `NVS_KEY_WIFI_SSID` | `"wifi_ssid"` | string | `""` |
| `NVS_KEY_WIFI_PASS` | `"wifi_pass"` | string | `""` |
| `NVS_KEY_DEVICE_NAME` | `"dev_name"` | string | `"embers-1"` |
| `NVS_KEY_DEVICE_ROLE` | `"dev_role"` | uint8 | `0` (Standalone) |
| `NVS_KEY_BRIGHTNESS` | `"brightness"` | uint16 | `800` (0–1000 scale) |
| `NVS_KEY_PROVISIONED` | `"provisioned"` | uint8 (bool) | `false` (0) |

**Note on `provisioned`:** NVS has no native bool type. Stored as `uint8_t` (0 or 1),
exposed as `bool` through the API.

---

## 3. Expected minicom output

```
I (xxx) EMBERS: ========================================
I (xxx) EMBERS:   Embers Lighting — ESP32 Firmware
I (xxx) EMBERS:   Build: May  8 2026 HH:MM:SS
I (xxx) EMBERS: ========================================
I (xxx) EMBERS: MAC: 8C:AA:B5:8E:C7:78
I (xxx) NVS: NVS initialised OK
I (xxx) EMBERS: --- NVS self-test START ---
I (xxx) EMBERS: PASS: nvs_store_wifi_ssid returned OK
I (xxx) EMBERS: PASS: nvs_get_wifi_ssid returned OK
I (xxx) EMBERS: PASS: wifi_ssid value matches
I (xxx) EMBERS: PASS: nvs_store_wifi_password returned OK
I (xxx) EMBERS: PASS: nvs_get_wifi_password returned OK
I (xxx) EMBERS: PASS: wifi_password value matches
I (xxx) EMBERS: PASS: nvs_store_device_name returned OK
I (xxx) EMBERS: PASS: nvs_get_device_name returned OK
I (xxx) EMBERS: PASS: device_name value matches
I (xxx) EMBERS: PASS: nvs_store_device_role returned OK
I (xxx) EMBERS: PASS: nvs_get_device_role returned OK
I (xxx) EMBERS: PASS: device_role value matches
I (xxx) EMBERS: PASS: nvs_store_brightness returned OK
I (xxx) EMBERS: PASS: nvs_get_brightness returned OK
I (xxx) EMBERS: PASS: brightness value matches
I (xxx) EMBERS: PASS: nvs_store_provisioned returned OK
I (xxx) EMBERS: PASS: nvs_get_provisioned returned OK
I (xxx) EMBERS: PASS: provisioned value matches
I (xxx) EMBERS: --- NVS self-test COMPLETE: ALL PASS ---
I (xxx) EMBERS: Heartbeat #1
```

*(Fill in actual timestamps and confirm output here before committing.)*

---

## 4. Open issues for future sessions

| # | Issue |
|---|-------|
| 1 | Self-test writes test values to flash — role and brightness will read back test values (2 and 750) after this session. A future session should either reset keys to defaults or just rely on the provisioning flow overwriting them. |
| 2 | Default-fallback test (first-boot get without prior store) not exercised in self-test — would require NVS erase between boots. Acceptable for now; defaults are verified by code inspection. |
| 3 | NVS key string `"wifi_pass"` is 9 chars — within NVS 15-char key limit. All other keys checked and within limit. |
| 4 | No encryption of WiFi password at rest. Acceptable for local-network product at this stage; revisit if security requirements tighten. |
| 5 | `nvs_self_test()` must be removed before production firmware is finalised. Marked TEMPORARY in source. |

---

## 5. Git commit message

```
feat: NVS storage layer with self-test (nvs_storage.c/h)
```
