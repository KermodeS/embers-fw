#pragma once

#include "esp_err.h"
#include <stdbool.h>

/**
 * @file wifi_sta.h
 * @brief ESP32 WiFi Station-mode bring-up for Embers Lighting.
 *
 * On disconnect, retries up to WIFI_STA_MAX_RETRY times (fast), then
 * slow-retries every WIFI_STA_SLOW_RETRY_MS until the total elapsed time
 * from first disconnect reaches 300 s, then calls esp_wifi_stop() and
 * idles. Recovery requires power-cycle. Budget resets on GOT_IP.
 *
 * On GOT_IP, mdns_hub_start() is called automatically (pattern b).
 */

#define WIFI_STA_MAX_RETRY  5

esp_err_t wifi_sta_init(void);
bool      wifi_sta_is_connected(void);

/**
 * @brief Returns true after WIFI_TIMEOUT fires and esp_wifi_stop() is called.
 * Resets to false on the next successful GOT_IP. Safe to call from any task.
 */
bool      wifi_sta_is_idle(void);
