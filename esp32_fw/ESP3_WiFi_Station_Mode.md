# ESP-3: WiFi Station Mode

| Field        | Value                                    |
|-------------|----------------------------------------------|
| session     | ESP-3                                      |
| title       | ESP32 WiFi Station Mode + NVS-backed credentials |
| date        | 2026-05-11                                    |
| branch      | main                                           |
| chip        | ESP32-D0WDQ6 rev v1.0, MAC 8C:AA:B5:8E:C7:78 |

---

## 1. Summary

The ESP32 brings up WiFi in station mode after NVS init, reads SSID and password from NVS (namespace `embers`, keys `wifi_ssid` and `wifi_pass`), and falls back to compiled-in credentials when either NVS value is empty or holds the ESP-2 self-test placeholder (`TestSSID` / `TestPassword123`). On disconnect it retries 5 times fast. If the fast-retry budget is exhausted it logs `WIFI_GIVEUP` and enters slow-retry mode: a dedicated FreeRTOS task pings a connection attempt every 30 s indefinitely. The heartbeat loop is never blocked.

---

---

## 2. Files created and modified

| Path (relative to ~/embers_fw/)      | Op     | Purpose                                |
|---------------------------------------|--------|-----------------------------------------------|
| esp32_fw/main/wifi_sta.h             | create | Public API, fallback credentials, retry constant |
| esp32_fw/main/wifi_sta.c             | create | WiFi STA implementation + slow-retry task |
| esp32_fw/main/embers_main.c          | modify | Call wifi_sta_init() after NVS self-test |
| esp32_fw/main/CMakeLists.txt         | modify | Add wifi_sta.c + REQUIRES block         |
| esp32_fw/ESP3_WiFi_Station_Mode.md   | create | This session write-up                      |

---

## 3. Strict spec recap

All these items were fixed at session start and must not drift in future sessions without an explicit change record.

- NVS namespace: `embers`
- NVS keys: `wifi_ssid` (string <= 32 chars), `wifi_pass` (string <= 64 chars)
- Fallback credentials as `#define`s in wifi_sta.h:
  - `WIFI_STA_FALLBACK_SSID` = `Pixel_7654`
  - `WIFI_STA_FALLBACK_PASS` = `hna2eunj68recnt`
  - (2.4 GHz only; ESP32-WROOM-32 does not support 5 GHz)
- Log tag for all WiFi lines: `wifi_sta`
- Required log line prefixes:
  - `WIFI_STARTED`
  - `WIFI_CONNECTING ssid=<name>`
  - `WIFI_CONNECTED ssid=<name> bssid=<xx:xx:xx:xx:xx:xx> channel=<n>`
  - `WIFI_DISCONNECTED reason=<n>`
  - `GOT_IP addr=... mask=... gw=...`
  - `WIFI_GIVEUP` (now followed by slow-retry mode; see Section 4)
  - `WIFI_SLOW_RETRY` (NEW this session)
- Public API: `esp_err_t wifi_sta_init(void)`; `bool wifi_sta_is_connected(void)`.

---

## 4. Deviation from original strict spec (approved mid-session)

Section 9 of the ESP-3 prompt stated:

> On disconnect, retry up to 5 times with esp_wifi_connect(). After 5 failures, log "WIFI_GIVEUP" and stop retrying.

This is wrong for a gallery deployment (a brief blip would brick the connection until power cycle). Mid-session we changed it to:

- 5 fast retries immediately after a disconnect.
- On exhaustion: log `WIFI_GIVEUP` and set a slow-retry event-group bit.
- A dedicated FreeRTOS task (`wifi_slow_retry`, priority 4, 3072 byte stack) waits on the bit, sleeps 30000 ms, calls `esp_wifi_connect()`, and loops forever.
- On `WIFI_EVENT_STA_CONNECTED` or `IP_EVENT_STA_GOT_IP`, the slow-retry bit is cleared.

The fast-retry counter is reset on connect, on `GOT_IP`, and on each slow-retry pulse. This reset-on-slow-pulse is deliberate and causes the actual runtime pattern during a long outage to be "five fast plus thirty quiet" burst, not a single attempt every 30 seconds.

**Actual pattern under sustained outage:**

```
[outage starts]
5 fast retries  (~13 s)
WIFI_GIVEUP
[30 s quiet]
WIFI_SLOW_RETRY pulse (triggers fresh fast-retry budget)
5 fast retries
WIFI_GIVEUP
[30 s quiet]
WIFI_SLOW_RETRY pulse
...
```

This is acceptable for an art-lighting product on mains power and gives faster recovery from brief blips than a pure 30-second-pulse design would.

---

## 5. Key implementation details

- **Event-group bits**: `WIFI_CONNECTED_BIT = BIT0` (set after `GOT_IP`, cleared on disconnect), `WIFI_SLOW_RETRY_BIT = BIT1` (set after fast-retry exhausted, cleared on connect).
- **Auth threshold**: `wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK`. This is the _minimum_ accepted auth mode; WPA3-SAE is accepted and observed (the Pixel hotspot runs WPA3-SAE HUNT_AND_PECK with PMF=1).
- **Fallback applies to both fields together**: if either `wifi_ssid` or `wifi_pass` fails the placeholder/empty check, both fall back to the compiled-in pair. This avoids mixing a real SSID with the wrong password.
- **Idempotent init**: `esp_netif_init` and `esp_event_loop_create_default` both accept `ESP_ERR_INVALID_STATE` (already done) as a non-fatal return.

---

---

## 6. Verification method

Test SSID: `Pixel_7654` (Pixel hotspot, 2.4 GHz, WPA3-SAE).

Expected log lines (in order) on a clean boot:

1. `NVS credentials empty or placeholder - using fallback`
2. `WIFI_STARTED`
3. `WIFI_CONNECTING ssid=Pixel_7654`
4. `WIFI_CONNECTED ssid=Pixel_7654 bssid=... channel=...`
5. `GOT_IP addr=... mask=... gw=...`
6. `Heartbeat #2` and higher post-GOT_IP.

---

## 7. Boot log (excerpt, happy-path)

```
I (469) EMBERS: MAC: 8C:AA:B5:8E:C7:78
I (599) EMBERS: --- NVS self-test COMPLETE: ALL PASS ---
W (699) wifi_sta: NVS credentials empty or placeholder - using fallback
I (789) wifi_sta: WIFI_STARTED
I (789) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654
I (4969) wifi_sta: WIFI_CONNECTED ssid=Pixel_7654 bssid=6e:bd:51:6d:1e:c1 channel=2
I (5789) EMBERS: Heartbeat #1
I (5999) wifi_sta: GOT_IP addr=10.254.23.184 mask=255.255.255.0 gw=10.254.23.237
I (10789) EMBERS: Heartbeat #2
```

Timing: power-on to GOT_IP ~6 seconds. RSSI -29 dBm.

---

## 8. Boot log (excerpt, slow-retry test)

Trigger: with the firmware connected, Pixel hotspot was powered off for ~60 s then back on.

```
I (192459) wifi:bcn_timeout,ap_probe_send_start
W (194979) wifi_sta: WIFI_DISCONNECTED reason=200
I (194979) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654 (retry 1/5)
W (197399) wifi_sta: WIFI_DISCONNECTED reason=201
I (197399) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654 (retry 2/5)
... (3 more fast retries) ...
E (207059) wifi_sta: WIFI_GIVEUP (after 5 fast retries) - entering slow-retry mode
I (210789) EMBERS: Heartbeat #42
I (235789) EMBERS: Heartbeat #47
I (237059) wifi_sta: WIFI_SLOW_RETRY ssid=Pixel_7654 (interval=30000 ms)
... (5 fast retries, second WIFI_GIVEUP) ...
I (267059) wifi_sta: WIFI_SLOW_RETRY ssid=Pixel_7654 (interval=30000 ms)
I (268469) wifi_sta: WIFI_CONNECTED ssid=Pixel_7654 bssid=32:1e:b4:a5:68:4b channel=11
I (269489) wifi_sta: GOT_IP addr=10.254.23.184 mask=255.255.255.0 gw=10.254.23.237
I (270789) EMBERS: Heartbeat #54
```

Evidence:
- `WIFI_GIVEUP` at t=207059 ms, first `WIFI_SLOW_RETRY` at t=237059 = exactly 30000 ms later.
- Second pulse at t=267059, also exactly 30000 ms apart.
- Heartbeats #42-53 continued throughout the 60 s outage.
- BSSID changed on reconnect (Pixel cycled its radio); SSID and IP preserved.

---
## 9. Known issues and deferred work

- The NVS self-test in `embers_main.c:nvs_self_test()` *still* overwrites `wifi_ssid` and `wifi_pass` with `TestSSID` and `TestPassword123` on every boot. Deferred from ESP-2. The placeholder detector in `wifi_sta.c` catches this, so the firmware connects via the fallback path. Removal deferred to a later session (likely after ESP-4).
- Fallback credentials are compiled in. ESP-4 (captive portal) replaces this with runtime provisioning.
- No reconnect-with-new-credentials path. If NVS is updated while the firmware is running, a reboot is required. Deferred to ESP-4.
- **FT232RL flash baud rate**: the default 460800 is unreliable on this workstation's FT232RL adapter; it stalls partway through the app write. **Always flash with `-b 115200`**. Monitor baud is unchanged. Update the session prompt template (Section 3 - STANDARD COMMANDS) before ESP-4.

---

## 10. Reproduction recipe

Preconditions: `~/embers_fw` present, ESP-IDF v5.5.4, bundled CMake 3.30.2.

Build:

```
. ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw && idf.py build
```

Flash (note `-b 115200`):

```
sudo fuser -k /dev/ttyUSB0
[ sleep 1 ]
. ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw && idf.py -p /dev/ttyUSB0 -b 115200 flash
```

Monitor:

```
minicom -D /dev/ttyUSB0 -b 115200
```

Press reset. Watch for the 6 required log lines (Section 6).

Optional slow-retry test: after GOT_IP, turn hotspot off for 60 s; expect 2 `WIFI_SLOW_RETRY` pulses at exactly 30000 ms apart.

---

## 11. On-horizon

- **ESP-4 captive portal**: softAP + HTTP form, persists SSID/password to NVS.
- **Self-test removal**: delete `nvs_self_test()` and its call.
- **Credential reload**: `wifi_sta_reconnect_with_new_creds()` so provisioning does not require reboot.
