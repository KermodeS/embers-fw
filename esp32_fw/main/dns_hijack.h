#pragma once

#include "esp_err.h"

/**
 * @brief Start the DNS hijack task.
 *
 * Binds a UDP socket to port 53 on all interfaces and answers every
 * DNS A-record query with 192.168.4.1 (the SoftAP gateway IP).
 * This triggers the captive-portal popup on iOS, Android, and Windows
 * clients as soon as they probe for internet connectivity.
 *
 * Must be called AFTER the SoftAP netif is up (i.e. after esp_wifi_start()).
 * Runs in its own FreeRTOS task; never blocks the caller.
 *
 * @return ESP_OK if the task was created, or an error code on failure.
 */
esp_err_t dns_hijack_start(void);
