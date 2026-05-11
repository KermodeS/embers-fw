# ESP4 - Captive Portal Provisioning

## Session Metadata
- **Session ID:** ESP-4
- **Title:** Captive Portal Provisioning
- **Date:** 2026-05-11
- **Status:** Hardware-verified -- both halves PASS, disambiguation PASS

---

## Summary

On first boot (NVS `provisioned` key = 0 or absent), the ESP32 now brings
itself up as an open SoftAP named `Embers-Setup-<last4-of-softap-mac>`,
starts a minimal DNS hijack task on UDP port 53 (triggering iOS/Android
captive-portal popups), and serves a single-page HTTP setup UI on
192.168.4.1:80. The page scans for nearby WiFi networks, accepts SSID and
password from the user, writes them to NVS (wifi_ssid, wifi_pass,
provisioned=1), and reboots into ESP-3 station mode using those credentials.
The NVS self-test was updated to save and restore the wifi credential keys
so provisioned values survive the self-test on subsequent boots.

---

## Files Created

- `esp32_fw/main/captive_portal.c` -- SoftAP init, HTTP server, all route handlers, reboot timer, AP event logging
- `esp32_fw/main/captive_portal.h` -- Public API: captive_portal_start()
- `esp32_fw/main/dns_hijack.c` -- UDP-53 task; answers every DNS A query with 192.168.4.1
- `esp32_fw/main/dns_hijack.h` -- Public API: dns_hijack_start()
- `esp32_fw/main/portal_page.h` -- Self-contained HTML/CSS/JS setup page as a C string literal
- `esp32_fw/ESP4_Captive_Portal_Provisioning.md` -- This document

## Files Modified

- `esp32_fw/main/embers_main.c` -- Boot-time prov_state branch; self-test saves/restores wifi credential keys; NVS_STATE diagnostic log
- `esp32_fw/main/CMakeLists.txt` -- Added captive_portal.c, dns_hijack.c to SRCS; added esp_http_server, freertos, esp_timer to REQUIRES

nvs_storage.c and nvs_storage.h required no changes -- the existing
nvs_store_provisioned / nvs_get_provisioned API (key "provisioned", uint8,
default 0) matched the spec exactly.

---

## Strict Spec Recap

**SoftAP:**
- SSID: Embers-Setup-XXXX where XXXX = last 4 hex chars of ESP_MAC_WIFI_SOFTAP, lowercase
- Auth: WIFI_AUTH_OPEN
- Channel: 1, max connections: 4
- IP/mask/gw: 192.168.4.1 / 255.255.255.0 / 192.168.4.1 (ESP-IDF default)

**HTTP endpoints (port 80):**
- GET /                  -- portal page HTML
- GET /scan             -- JSON networks array, blocking scan, sorted RSSI, hidden/dup dropped
- POST /save            -- fields ssid (1-32 chars) and pass (0-63 chars); writes NVS, 302 after 2s
- GET /generate_204     -- 302 to / (Android captive detect)
- GET /hotspot-detect.html -- 302 to / (iOS captive detect)
- GET /connecttest.txt  -- 302 to / (Windows captive detect)
- GET /*                -- 302 to / (catch-all)

**NVS keys (namespace "embers"):**
- provisioned (uint8): 0 = unprovisioned -> portal, 1 = provisioned -> station
- wifi_ssid (str): written by /save
- wifi_pass (str): written by /save

**Log tag:** cap_portal

**Required log line prefixes:**

    PROV_AP_STARTED ssid=<name> ip=192.168.4.1
    PROV_CLIENT_CONNECTED mac=<xx:xx:xx:xx:xx:xx>
    PROV_CLIENT_DISCONNECTED mac=<xx:xx:xx:xx:xx:xx> reason=<n>
    PROV_SCAN_RESULTS count=<n>
    PROV_SAVED ssid=<name>
    PROV_VALIDATION_FAIL reason=<tag>
    PROV_REBOOTING

---

## Verification

### Half A -- Provisioning AP accepts credentials

Boot log (first boot after idf.py erase-flash + flash):

    I (611) EMBERS: prov_state=0: starting captive portal
    I (791) cap_portal: PROV_AP_STARTED ssid=Embers-Setup-c779 ip=192.168.4.1
    I (801) cap_portal: DNS hijack task running on UDP port 53
    I (811) cap_portal: HTTP server started on 192.168.4.1:80

Phone connected to Embers-Setup-c779, captive portal page loaded, scan
returned 4 networks including Pixel_7654. Credentials submitted:

    I (37791) cap_portal: PROV_CLIENT_CONNECTED mac=92:e5:de:b1:03:8f
    I (57381) cap_portal: PROV_SCAN_RESULTS count=4
    I (93921) cap_portal: PROV_SAVED ssid=Pixel_7654
    I (95921) cap_portal: PROV_REBOOTING

### Half B -- Station mode uses NVS credentials after reboot

Post-reboot boot log:

    I (611) EMBERS: NVS_STATE ssid='Pixel_7654' pass_len=15 provisioned=1
    I (621) EMBERS: prov_state=1: starting WiFi station mode
    I (821) wifi_sta: WIFI_STARTED
    I (821) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654
    I (3401) wifi_sta: WIFI_CONNECTED ssid=Pixel_7654 bssid=7e:f4:08:3c:56:6f channel=11
    I (4421) wifi_sta: GOT_IP addr=10.254.23.184 mask=255.255.255.0 gw=10.254.23.237

No "NVS credentials empty or placeholder" warning -- fallback not used.

### Half B(e) -- Disambiguation (fallback proven unused)

WIFI_STA_FALLBACK_SSID temporarily changed to "FallbackShouldNotBeUsed",
firmware rebuilt and flashed without erasing NVS. Boot log:

    I (611) EMBERS: NVS_STATE ssid='Pixel_7654' pass_len=15 provisioned=1
    I (801) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654
    I (4821) wifi_sta: WIFI_CONNECTED ssid=Pixel_7654 ...
    I (5861) wifi_sta: GOT_IP addr=10.254.23.184 ...

GOT_IP achieved with Pixel_7654 despite fallback set to an invalid SSID.
NVS values confirmed genuine. Fallback reverted to "Pixel_7654" before commit.

---

## Known Issues / Deferred Work

- **NVS self-test interaction:** The self-test now saves and restores
  wifi_ssid and wifi_pass around its test writes, so provisioned credentials
  survive. The self-test should be removed once NVS is fully trusted --
  deferred to a later session.
- **No software reprovision trigger:** The only way to force re-entry into
  the captive portal is idf.py erase-flash (see below). A software-driven
  "forget WiFi" path requires WebSocket/UART control -- deferred to ESP-6
  or later.
- **Captive portal is open (no auth):** Acceptable for a one-time setup
  flow. If hardened later, add a short PIN displayed on the device.
- **Hardcoded fallback in wifi_sta.h is now dead code** on any provisioned
  device. Retained for ESP-3 regression testing; remove once provisioning
  is trusted (probably ESP-5 or ESP-6).
- **SSID uses SoftAP MAC not STA MAC:** The spec example showed c778 (STA
  MAC suffix) but the SoftAP MAC is STA+1, giving c779 on this unit.
  Behaviour is correct; the spec example was illustrative.

---

## Reproduction Commands

### Full provisioning from scratch:

    . ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw
    idf.py -p /dev/ttyUSB0 -b 115200 erase-flash
    idf.py -p /dev/ttyUSB0 -b 115200 flash
    minicom -D /dev/ttyUSB0 -b 115200
    # Press reset. Expect: PROV_AP_STARTED ssid=Embers-Setup-c779
    # Connect phone to Embers-Setup-c779 (open, no password)
    # Browse to http://192.168.4.1/ if portal does not auto-open
    # Press Scan, select network, enter password, press Save & Connect
    # After reboot expect: NVS_STATE ssid='<ssid>' pass_len=<n> provisioned=1
    #                      GOT_IP addr=...

### How to wipe and reprovision (escape hatch):

    . ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw
    idf.py -p /dev/ttyUSB0 -b 115200 erase-flash
    idf.py -p /dev/ttyUSB0 -b 115200 flash
    # Device boots into captive portal on next reset

### Build only (no flash):

    . ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw && idf.py build
