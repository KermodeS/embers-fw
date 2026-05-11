#pragma once

#include "esp_err.h"

/**
 * @brief Start the captive-portal provisioning flow.
 *
 * Brings up the ESP32 as a SoftAP named "Embers-Setup-<last4mac>",
 * starts an HTTP server on port 80 with all required endpoints,
 * and starts the DNS hijack task on UDP port 53.
 *
 * Call this from app_main() when NVS prov_state == 0 (unprovisioned).
 * Do NOT call wifi_sta_init() in the same boot when this is called.
 *
 * On /save success the firmware writes wifi_ssid, wifi_pass, and
 * prov_state=1 to NVS, then calls esp_restart() after a 2-second delay.
 *
 * @return ESP_OK if AP + HTTP server + DNS started without error.
 *         Any other value indicates a startup failure; the caller should
 *         log but continue (the heartbeat loop must keep running).
 */
esp_err_t captive_portal_start(void);
