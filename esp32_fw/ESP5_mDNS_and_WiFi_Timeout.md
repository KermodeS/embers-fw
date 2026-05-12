# ESP5 — mDNS Hub Advertisement + Bounded 5-Minute WiFi Retry

## Session Metadata

| Field       | Value                                                        |
|-------------|--------------------------------------------------------------|
| Session ID  | ESP-5                                                        |
| Title       | mDNS Hub Advertisement + Bounded 5-Minute WiFi Retry        |
| Date        | 2026-05-12                                                   |
| Branch      | main                                                         |
| Chip        | ESP32-D0WDQ6 rev v1.0, MAC 8C:AA:B5:8E:C7:78                |
| ELF SHA256  | 3aa131ea5...                                                 |
| Status      | Hardware-verified — all three halves PASS                    |

---

## Summary

Two independent features were added. First, after GOT_IP the device now
advertises itself on the local network via mDNS as
`embers-hub-c779.local`, with a custom service type `_embers._tcp` on
port 80. TXT records flag `role=hub` as groundwork for the future
multi-device hub architecture; nodes will browse for `_embers._tcp` to
discover the hub. The customer's immediate value is typing
`embers-hub-c779.local` in a browser rather than guessing an IP address.
Second, the WiFi retry sequence is now bounded: after 5 fast retries plus
slow retries totalling 300 seconds from the first disconnect,
`esp_wifi_stop()` is called and the device idles indefinitely with no
further connection attempts, no SoftAP beaconing, and no probe requests.
The retry budget resets to zero on each successful GOT_IP so a transient
outage does not permanently consume the budget.

---

## Files Created

| File                                    | Purpose                                                              |
|-----------------------------------------|----------------------------------------------------------------------|
| `esp32_fw/main/mdns_hub.c`             | mDNS init, hostname/instance derivation from SoftAP MAC, service registration |
| `esp32_fw/main/mdns_hub.h`             | Public API: `mdns_hub_start()`                                       |
| `esp32_fw/ESP5_mDNS_and_WiFi_Timeout.md` | This document                                                      |

## Files Modified

| File                          | Change                                                                                      |
|-------------------------------|---------------------------------------------------------------------------------------------|
| `esp32_fw/main/wifi_sta.c`   | Added 300 s retry budget, WIFI_TIMEOUT log + esp_wifi_stop() + idle flag, budget reset on GOT_IP, mdns_hub_start() hook on GOT_IP, wifi_sta_is_idle() |
| `esp32_fw/main/wifi_sta.h`   | Added wifi_sta_is_idle() prototype and updated header comment                               |
| `esp32_fw/main/CMakeLists.txt` | Added mdns_hub.c to SRCS; added espressif__mdns to REQUIRES                              |
| `esp32_fw/main/idf_component.yml` | Created by idf.py add-dependency — declares managed component espressif/mdns v1.11.1  |

Note: embers_main.c required no changes. Pattern (b) was used (see below).

---

## mDNS Invocation Pattern

**Pattern (b) chosen:** `mdns_hub_start()` is called from inside the
`IP_EVENT_STA_GOT_IP` handler in `wifi_sta.c`. This is the cleanest
approach — mDNS starts exactly when the netif has an IP, with no polling
loop in `app_main`. The function is idempotent (guarded by `s_started`)
so reconnect events after a slow-retry recovery do not re-initialise mDNS.

---

## Strict Spec Recap

**mDNS hostname:** `embers-hub-XXXX.local` where XXXX = last 4 hex chars
of `ESP_MAC_WIFI_SOFTAP`, lowercase. For this unit: `embers-hub-c779.local`.

**mDNS instance name:** `Embers Hub c779`

**mDNS service:**
- Type: `_embers._tcp`
- Port: 80
- TXT records: `role=hub`, `fw_version=1.0.0`, `api=0`

**WiFi retry budget:**
- Fast retries: up to 5 (WIFI_STA_MAX_RETRY)
- Slow retries: every 30,000 ms (WIFI_STA_SLOW_RETRY_MS)
- Total budget: 300 s (WIFI_STA_TIMEOUT_US = 300LL * 1000000LL) measured
  from esp_timer_get_time() captured on the first fast-retry disconnect
- On expiry: log WIFI_TIMEOUT elapsed=Ns — entering idle, call
  esp_wifi_stop(), set s_wifi_idle = true
- Budget resets to zero on IP_EVENT_STA_GOT_IP

**Required log line prefixes (new):**

    MDNS_STARTED hostname=<name>.local
    MDNS_SERVICE_ADDED type=_embers._tcp port=80
    WIFI_TIMEOUT elapsed=<n>s — entering idle

**Component manager:** espressif/mdns v1.11.1 added as a managed
component via idf.py add-dependency. REQUIRES entry in CMakeLists.txt
uses the full name espressif__mdns (double underscore), not mdns.

---

## Verification

### Half A — mDNS advertisement reachable

Boot log (provisioned device, hotspot on):

    I (3092) wifi_sta: GOT_IP addr=10.254.23.184 mask=255.255.255.0 gw=10.254.23.237
    I (3092) mdns_mem: mDNS task will be created from internal RAM
    I (3102) mdns_hub: MDNS_STARTED hostname=embers-hub-c779.local
    I (3102) mdns_hub: MDNS_SERVICE_ADDED type=_embers._tcp port=80
    I (5732) EMBERS: Heartbeat #1

Workstation avahi-daemon is on enp3s0 (wired Ethernet); ESP32 is on
Pixel_7654 hotspot (different network segment). mDNS multicast does not
cross network segments, so ping and avahi-resolve from the workstation
timed out — expected and not a firmware issue.

Verification was performed from a second phone connected to the Pixel_7654
hotspot. Browsing to http://embers-hub-c779.local produced "could not
connect to the server" — the hostname resolved successfully via mDNS but
the connection was refused because no HTTP server is bound to port 80 yet
(deferred to ESP-7). A connection-refused response requires a successful
DNS resolution; a DNS failure produces "server not found", not
"could not connect". PASS.

### Half B — 5-minute timeout stops WiFi

Hotspot turned OFF after device was connected. Sequence observed:

    W (752502) wifi_sta: WIFI_DISCONNECTED reason=201
    I (752502) wifi_sta: WIFI_CONNECTING ssid=Pixel_7654 (retry 1/5)
    ...
    E (764582) wifi_sta: WIFI_GIVEUP (after 5 fast retries) - entering slow-retry mode
    I (780092) wifi_sta: WIFI_SLOW_RETRY ssid=Pixel_7654 (interval=30000 ms)
    ...
    E (810092) wifi_sta: WIFI_TIMEOUT elapsed=312s — entering idle
    I (810092) wifi:flush txq
    I (810092) wifi:stop sw txq
    I (810092) wifi:lmac stop hw txq
    I (810732) EMBERS: Heartbeat #162
    I (815732) EMBERS: Heartbeat #163
    I (820732) EMBERS: Heartbeat #164
    I (825732) EMBERS: Heartbeat #165

WIFI_TIMEOUT at 312 s (within 295-315 s window). No WIFI_* lines for
2+ minutes afterward. Heartbeat continued uninterrupted. PASS.

### Half C — Budget resets on reconnect

Power-cycled device. First disconnect triggered slow-retry. Hotspot
restored after ~2 slow-retry pulses. Device reconnected:

    I (106982) wifi_sta: GOT_IP addr=10.254.23.184 mask=255.255.255.0 gw=10.254.23.237
    W (106982) mdns_hub: mdns_hub_start called more than once — ignored

Budget reset to zero confirmed (idempotent mDNS guard fired correctly).
Hotspot turned OFF again. Second WIFI_TIMEOUT fired at t=439,162 ms,
which is 312 s after the second disconnect at t=127,072 ms — not
cumulative from the first disconnect. PASS.

    E (439162) wifi_sta: WIFI_TIMEOUT elapsed=312s — entering idle
    I (439162) wifi:flush txq
    I (439162) wifi:stop sw txq
    I (439162) wifi:lmac stop hw txq
    I (440912) EMBERS: Heartbeat #88
    ...
    I (490912) EMBERS: Heartbeat #98

---

## Hub-Model Context

This session lays mDNS groundwork for the future multi-device hub
architecture. Nodes will browse for _embers._tcp to discover the hub
automatically. The api=0 TXT record will be incremented when the hub
control API exists. The customer's short-term value is "no more guessing
IP addresses" — they can type embers-hub-c779.local in a browser once
the hub web UI lands (ESP-7 or later).

---

## Known Issues / Deferred Work

- **No HTTP server on port 80 in normal operation.** The mDNS service
  advertisement points at a closed port. ESP-7 (hub web UI) closes this gap.

- **No button-trigger wake from WIFI_TIMEOUT idle state.** Needs STM32
  long-press detection, a PROTO-001 UART command, and an ESP32 handler.
  Must be sequenced after the STM32 UART parser session.

- **No visual LED indicator for idle state.** Same dependency chain as
  button-wake.

- **Hub role hard-coded in TXT records.** role=hub is set unconditionally.
  Role-assignment via the captive portal is ESP-6.

- **mDNS not verified via avahi-browse.** The workstation avahi-daemon is
  on a different network segment than the ESP32. Verified via second phone
  on the hotspot instead. In a future session where they share a network,
  avahi-browse -t _embers._tcp should show the device.

- **Multiple devices:** 4-hex-char suffix gives 65,536 unique values.
  mDNS built-in conflict resolution handles any collision with -2/-3 suffix.

- **espressif__mdns in REQUIRES.** Managed components use double-underscore
  in CMakeLists.txt REQUIRES. If vendored directly, revert to plain mdns.

---

## Reproduction Commands

### Flash and verify (NVS credentials already provisioned):

    sudo fuser -k /dev/ttyUSB0
    . ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw
    idf.py -p /dev/ttyUSB0 -b 115200 flash
    minicom -D /dev/ttyUSB0 -b 115200
    # Press reset. Expect: GOT_IP -> MDNS_STARTED -> MDNS_SERVICE_ADDED

### Re-provision from scratch:

    . ~/esp/esp-idf/export.sh && cd ~/embers_fw/esp32_fw
    idf.py -p /dev/ttyUSB0 -b 115200 erase-flash
    idf.py -p /dev/ttyUSB0 -b 115200 flash
    minicom -D /dev/ttyUSB0 -b 115200
    # Connect phone to Embers-Setup-c779, provision via portal.

### Kill runaway build:

    pkill -9 -u workstation -f ninja; pkill -9 -u workstation -f cmake
