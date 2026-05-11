#pragma once

#include "esp_err.h"
#include <stdbool.h>

/**
 * @file wifi_sta.h
 * @brief ESP32 WiFi Station-mode bring-up for Embers Lighting.
 *
 * Reads SSID/password from NVS (namespace "embers", keys "wifi_ssid" /
 * "wifi_pass" as defined in nvs_storage.h). When the stored value is empty
 * or matches a known ESP-2 self-test placeholder, falls back to the
 * compiled-in WIFI_STA_FALLBACK_SSID / WIFI_STA_FALLBACK_PASS below.
 *
 * On disconnect, retries esp_wifi_connect() up to WIFI_STA_MAX_RETRY times.
 * After the retry budget is exhausted, logs "WIFI_GIVEUP" and stops retrying;
 * the application loop (heartbeat) keeps running.
 *
 * Provisioning (captive portal) is intentionally NOT handled here -- that is
 * ESP-4. This module is pure station mode + DHCP.
 */

// ---------------------------------------------------------------------------
// Fallback credentials
// --------------------------------------------------------------------------
// Used only when the NVS-stored credentials are empty or hold the ESP-2
// self-test placeholders. Replaced by real provisioning in ESP-4.
//
// NOTE: the target hotspot must be 2.4 GHz -- the ESP32-WROOM-32 does not
// support 5 GHz.
#define WIFI_STA_FALLBACK_SSID   "Pixel_7654"
#define WIFI_STA_FALLBACK_PASS   "hna2eunj68recnt"

// ---------------------------------------------------------------------------
// Tuning
// --------------------------------------------------------------------------
#define WIFI_STA_MAX_RETRY       5

// --------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------

/**
 * @brief Initialise and start the WiFi station-mode subsystem.
 *
 * Preconditions:
 *   - nvs_storage_init() has already been called successfully.
 *   - The default event loop may or may not exist; this function will
 *     create it if needed (idempotent).
 *
 * The call is non-blocking. Connection progress is reported asynchronously
 * via ESP_LOG lines tagged "wifi_sta" with the prefixes:
 *     WIFI_STARTED
 *     WIFI_CONNECTING ssid=<name>
 *     WIFI_CONNECTED  ssid=<name> bssid=<xx:xx:xx:xx:xx:xx> channel=<n>
 *     WIFI_DISCONNECTED reason=<n>
 *     GOT_IP addr=<a.b.c.d> mask=<a.b.c.d> gw=<a.b.c.d>
 *     WIFI_GIVEUP            (only after WIFI_STA_MAX_RETRY failures)
 *
 * @return ESP_OK on success, or an esp_err_t from the underlying esp_wifi/
 *         esp_netif/esp_event initialisation chain.
 */
esp_err_t wifi_sta_init(void);

/**
 * @brief Query whether the station is currently associated AND has an IP.
 *
 * Returns true only after the IP_EVENT_STA_GOT_IP event has fired and before
 * any subsequent disconnect. Safe to call from any task.
 */
bool wifi_sta_is_connected(void);
