#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// NVS namespace and key definitions
#define NVS_NAMESPACE       "embers"
#define NVS_KEY_WIFI_SSID   "wifi_ssid"
#define NVS_KEY_WIFI_PASS   "wifi_pass"
#define NVS_KEY_DEVICE_NAME "dev_name"
#define NVS_KEY_DEVICE_ROLE "dev_role"
#define NVS_KEY_BRIGHTNESS  "brightness"
#define NVS_KEY_PROVISIONED "provisioned"

// Default values (returned when key is not found — first boot)
#define NVS_DEFAULT_WIFI_SSID   ""
#define NVS_DEFAULT_WIFI_PASS   ""
#define NVS_DEFAULT_DEVICE_NAME "embers-1"
#define NVS_DEFAULT_DEVICE_ROLE ((uint8_t)0)     // 0 = Standalone
#define NVS_DEFAULT_BRIGHTNESS  ((uint16_t)800)  // 0-1000 scale
#define NVS_DEFAULT_PROVISIONED false

/**
 * @brief Initialise the NVS flash subsystem.
 *
 * Must be called once from app_main() before any storage functions are used.
 * Handles erase-and-reinit automatically if the partition is corrupt or
 * a new NVS version is detected.
 *
 * @return ESP_OK on success, or an esp_err_t on failure.
 */
esp_err_t nvs_storage_init(void);

// --- WiFi credentials ---
esp_err_t nvs_store_wifi_ssid(const char *ssid);
esp_err_t nvs_get_wifi_ssid(char *buf, size_t len);

esp_err_t nvs_store_wifi_password(const char *password);
esp_err_t nvs_get_wifi_password(char *buf, size_t len);

// --- Device settings ---
esp_err_t nvs_store_device_name(const char *name);
esp_err_t nvs_get_device_name(char *buf, size_t len);

esp_err_t nvs_store_device_role(uint8_t role);
esp_err_t nvs_get_device_role(uint8_t *role);

esp_err_t nvs_store_brightness(uint16_t brightness);
esp_err_t nvs_get_brightness(uint16_t *brightness);

// --- Provisioning state ---
esp_err_t nvs_store_provisioned(bool provisioned);
esp_err_t nvs_get_provisioned(bool *provisioned);
