#pragma once

#include "esp_err.h"

/**
 * @file mdns_hub.h
 * @brief mDNS hub advertisement for Embers Lighting.
 *
 * Announces this device on the local network as:
 *   hostname:      embers-hub-<last4-of-softap-mac>.local
 *   instance name: Embers Hub <last4-of-softap-mac>
 *   service:       _embers._tcp, port 80
 *   TXT records:   role=hub, fw_version=1.0.0, api=0
 *
 * Call mdns_hub_start() once, from the IP_EVENT_STA_GOT_IP handler
 * (or any point after the STA netif has an IP).
 *
 * Log lines emitted:
 *   MDNS_STARTED hostname=<name>.local
 *   MDNS_SERVICE_ADDED type=_embers._tcp port=80
 */

/**
 * @brief Initialise mDNS and advertise the hub service.
 *
 * Idempotent: if called a second time it logs a warning and returns ESP_OK.
 *
 * @return ESP_OK on success, or an esp_err_t from the mdns subsystem.
 */
esp_err_t mdns_hub_start(void);
