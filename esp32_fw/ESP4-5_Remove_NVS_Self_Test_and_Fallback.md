# ESP4-5 — Remove NVS Self-Test and Hardcoded WiFi Fallback

## Session Metadata

| Field       | Value                                                        |
|-------------|--------------------------------------------------------------|
| Session ID  | ESP-4.5                                                      |
| Title       | Remove NVS Self-Test and Hardcoded WiFi Fallback             |
| Date        | 2026-05-12                                                   |
| Branch      | main                                                         |
| Chip        | ESP32-D0WDQ6 rev v1.0, MAC 8C:AA:B5:8E:C7:78                |
| Status      | Hardware-verified — both halves PASS, strings check PASS     |

---

## Summary

The ESP-2 `nvs_self_test()` function in `embers_main.c` was written to prove
the NVS storage layer worked on hardware. With ESP-3 (WiFi station mode) and
ESP-4 (captive portal provisioning) both relying on persisted NVS credentials,
the self-test became dead weight: it destructively wrote test values on every
boot (even though it saved/restored `wifi_ssid` and `wifi_pass`), added noise
to the boot log, and left the `CHECK` macro polluting the translation unit. It
is now removed entirely.

The `WIFI_STA_FALLBACK_SSID` / `WIFI_STA_FALLBACK_PASS` macros in `wifi_sta.h`
and the placeholder-detection branch in `wifi_sta.c` (`"TestSSID"` /
`"TestPassword123"` comparisons) are also removed. A provisioned device has
real NVS credentials and needs no fallback. An unprovisioned device enters the
captive portal. The one remaining edge case — NVS corruption between the
`provisioned` read and `wifi_sta_init()` — is handled by a new error path: log
`WIFI_NVS_MISSING`, flip `provisioned=0`, and `esp_restart()` back into the
portal.

A minor fix was applied during hardware verification: `wifi_cfg.sta.threshold.authmode`
was changed from `WIFI_AUTH_WPA2_PSK` to `WIFI_AUTH_WPA_PSK` to permit
WPA3-SAE (HUNT_AND_PECK) negotiation with the Pixel_7654 hotspot, which
advertises WPA2/WPA3-Personal. This is strictly more permissive and does not
affect WPA2-only networks.

---

## Files Modified

| File                          | Change                                                                                          |
|-------------------------------|-------------------------------------------------------------------------------------------------|
| `esp32_fw/main/embers_main.c` | Removed `nvs_self_test()` definition, `CHECK` macro, and call site in `app_main()`             |
| `esp32_fw/main/wifi_sta.h`    | Removed `WIFI_STA_FALLBACK_SSID` and `WIFI_STA_FALLBACK_PASS` defines and associated comments  |
| `esp32_fw/main/wifi_sta.c`    | Removed fallback path, placeholder-detection statics and helpers; added `credentials_empty()` helper and `WIFI_NVS_MISSING` error path; changed authmode threshold to `WIFI_AUTH_WPA_PSK` |

---

## Net Diff Summary

- `embers_main.c`: ~95 lines removed, 0 added
- `wifi_sta.h`: ~10 lines removed, 0 added
- `wifi_sta.c`: ~20 lines removed, ~10 lines added

---

## Strings Check

Command:

```
cd ~/embers_fw/esp32_fw && \
  strings build/embers_fw.elf | \
  grep -E "TestSSID|Pixel_7654|hna2eunj68recnt|nvs_self_test" || \
  echo "CLEAN: removed strings not present in image"
```

Output:

```
CLEAN: removed strings not present in image
```

ELF SHA256 of verified image: `4742d66cc...`

---

## Boot Path (updated)

```
nvs_flash_init
  └─ NVS_STATE diagnostic log (ssid / pass_len / provisioned)
  └─ read provisioned
       ├─ provisioned == 1  →  wifi_sta_init()
       │      └─ load NVS credentials
       │            ├─ credentials present  →  connect (no fallback)
       │            └─ credentials empty    →  log WIFI_NVS_MISSING
       │                                       nvs_store_provisioned(0)
       │                                       esp_restart()  →  portal
       └─ provisioned == 0  →  captive_portal_start()
  └─ heartbeat loop
```

---

## Verification

### Half A — Already-provisioned device connects from NVS

Precondition: device carried NVS credentials from ESP-4 (`Pixel_7654`,
`hna2eunj68recnt`, `provisioned=1`). New image flashed without erasing NVS.

Key boot log lines:

```
I (541) EMBERS: NVS_STATE ssid='Pixel_7654' pass_len=15 provisioned=1
I (541) EMBERS: prov_state=1: starting WiFi station mode
I (721) wifi_sta: WIFI_STARTED
I (721) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654
I (2021) wifi_sta: WIFI_CONNECTED ssid=Pixel_7654 bssid=fa:b0:a9:d8:21:8e channel=11
I (3041) wifi_sta: GOT_IP addr=10.254.23.184 mask=255.255.255.0 gw=10.254.23.237
I (5721) EMBERS: Heartbeat #1
```

No `--- NVS self-test START ---` line. No PASS/FAIL lines. **PASS.**

### Half B — Virgin device provisions via portal, reboots, connects

Step 1: `idf.py erase-flash` then `idf.py flash`.

First boot log (unprovisioned — captive portal):

```
I (531) EMBERS: NVS_STATE ssid='' pass_len=0 provisioned=0
I (531) EMBERS: prov_state=0: starting captive portal
I (721) cap_portal: PROV_AP_STARTED ssid=Embers-Setup-c779 ip=192.168.4.1
I (731) cap_portal: DNS hijack task running on UDP port 53
I (741) cap_portal: HTTP server started on 192.168.4.1:80
```

Phone connected to `Embers-Setup-c779` (open, no password). Portal page
loaded, scan run, `Pixel_7654` selected, password entered, Save & Connect
pressed. Device wrote credentials to NVS and rebooted automatically.

Second boot log (provisioned — station mode):

```
I (530) EMBERS: NVS_STATE ssid='Pixel_7654' pass_len=15 provisioned=1
I (530) EMBERS: prov_state=1: starting WiFi station mode
I (720) wifi_sta: WIFI_STARTED
I (720) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654
I (3260) wifi:security: WPA3-SAE HUNT_AND_PECK, phy: bgn, rssi: -25
I (3280) wifi_sta: WIFI_CONNECTED ssid=Pixel_7654 bssid=5e:8f:fb:f8:d0:d2 channel=11
I (4310) wifi_sta: GOT_IP addr=10.254.23.184 mask=255.255.255.0 gw=10.254.23.237
I (5720) EMBERS: Heartbeat #1
```

No self-test output. No fallback used (fallback no longer exists in the
binary). Successful connection is unambiguous proof that NVS persistence works
end-to-end. **PASS.**

---

## Known Issues / Deferred Work

- **Long-press-button reprovision trigger** — deferred. Requires STM32
  long-press detection, a PROTO-001 UART command, and an ESP32 handler.
  Must be sequenced after the STM32 UART parser session.

- **No "forget WiFi" web UI button** — will land with the WebSocket control
  UI session.

- **No in-field reprovision path** — if a user's home WiFi changes (new
  router, password change), the only current path back into provisioning is
  `idf.py erase-flash` via the FT232RL adapter. Acceptable for the
  development phase. Will be replaced by the long-press flow.

---

## Reproduction Commands

### Repeat Half A (already-provisioned device):

```
. ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw
idf.py -p /dev/ttyUSB0 -b 115200 flash
minicom -D /dev/ttyUSB0 -b 115200
# Press reset. Expect: NVS_STATE ssid='Pixel_7654' ... provisioned=1
#                      GOT_IP addr=...
```

### Repeat Half B (full provisioning from scratch):

```
. ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw
idf.py -p /dev/ttyUSB0 -b 115200 erase-flash
idf.py -p /dev/ttyUSB0 -b 115200 flash
minicom -D /dev/ttyUSB0 -b 115200
# Press reset. Expect: NVS_STATE ssid='' pass_len=0 provisioned=0
#                      PROV_AP_STARTED ssid=Embers-Setup-c779
# Connect phone to Embers-Setup-c779 (open, no password)
# Browse to http://192.168.4.1/ if portal does not auto-open
# Scan, select network, enter password, press Save & Connect
# After auto-reboot expect: NVS_STATE ssid='<ssid>' pass_len=<n> provisioned=1
#                            GOT_IP addr=...
```

### Strings check (confirm removed strings absent from image):

```
cd ~/embers_fw/esp32_fw && \
  strings build/embers_fw.elf | \
  grep -E "TestSSID|Pixel_7654|hna2eunj68recnt|nvs_self_test" || \
  echo "CLEAN: removed strings not present in image"
```
