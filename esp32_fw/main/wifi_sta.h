#pragma once

#include "esp_err.h"
#include <stdbool.h>

/**
 * @file wifi_sta.h
 * @brief ESP32 WiFi Station-mode bring-up for Embers Lighting.
 *
 * Reads SSID/password from NVS (namespace "embers", keys "wifi_ssid" /
 * "wifi_pass" as defined in nvs_storage.h).
 *
 * If NVS credentials are empty when wifi_sta_init() is called (which should
 * not happen on a provisioned device but is theoretically reachable on NVS
 * corruption), the function logs WIFI_NVS_MISSING, flips provisioned=0, and
 * restarts into the captive portal.
 *
 * On disconnect, retries esp_wifi_connect() up to WIFI_STA_MAX_RETRY times.
 * After the retry budget is exhausted, logs "WIFI_GIVEUP" and enters
 * slow-retry mode (one attempt every WIFI_STA_SLOW_RETRY_MS forever).
 *
 * Provisioning (captive portal) is handled in captive_portal.c — this
 * module is pure station mode + DHCP.
 */

// ---------------------------------------------------------------------------
// Tuning
// ---------------------------------------------------------------------------
#define WIFI_STA_MAX_RETRY       5

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

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
 *     WIFI_NVS_MISSING       (NVS empty on station-init; restarts into portal)
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
