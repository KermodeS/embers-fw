#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_storage.h"
#include "wifi_sta.h"
#include "captive_portal.h"

static const char *TAG = "EMBERS";

// ---------------------------------------------------------------------------
// app_main
// ---------------------------------------------------------------------------

void app_main(void)
{
    // Startup banner
    ESP_LOGI(TAG, "====================================");
    ESP_LOGI(TAG, "  Embers Lighting - ESP32 Firmware");
    ESP_LOGI(TAG, "  Build: " __DATE__ " " __TIME__);
    ESP_LOGI(TAG, "====================================");

    // Log MAC address
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG, "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // NVS init
    esp_err_t nvs_err = nvs_storage_init();
    if (nvs_err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed - storage unavailable");
    }

    // Diagnostic: log NVS credential state before provisioning branch
    {
        char dbg_ssid[64] = {0};
        char dbg_pass[128] = {0};
        bool dbg_prov = false;
        nvs_get_wifi_ssid(dbg_ssid, sizeof(dbg_ssid));
        nvs_get_wifi_password(dbg_pass, sizeof(dbg_pass));
        nvs_get_provisioned(&dbg_prov);
        ESP_LOGI(TAG, "NVS_STATE ssid=\'%s\' pass_len=%d provisioned=%d",
                 dbg_ssid, (int)strlen(dbg_pass), (int)dbg_prov);
    }

    // Boot-time provisioning decision
    bool provisioned = false;
    if (nvs_err == ESP_OK) {
        nvs_get_provisioned(&provisioned);
    }

    if (!provisioned) {
        ESP_LOGI(TAG, "prov_state=0: starting captive portal");
        esp_err_t portal_err = captive_portal_start();
        if (portal_err != ESP_OK) {
            ESP_LOGE(TAG, "captive_portal_start failed: %s — continuing",
                     esp_err_to_name(portal_err));
        }
    } else {
        ESP_LOGI(TAG, "prov_state=1: starting WiFi station mode");
        esp_err_t wifi_err = wifi_sta_init();
        if (wifi_err != ESP_OK) {
            ESP_LOGE(TAG, "wifi_sta_init failed: %s - continuing without WiFi",
                     esp_err_to_name(wifi_err));
        }
    }

    // TODO (ESP-5): mDNS init
    // TODO (ESP-6): WebSocket server init
    // TODO (ESP-7): UART-to-STM32 driver init
    // TODO (ESP-8): ESP-NOW init

    // Main loop - heartbeat
    uint32_t heartbeat_count = 0;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        heartbeat_count++;
        ESP_LOGI(TAG, "Heartbeat #%lu", heartbeat_count);
    }
}
