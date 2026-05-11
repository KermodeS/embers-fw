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
// NVS self-test
// ---------------------------------------------------------------------------
//
// TEMPORARY: Writes known values to every NVS key, reads them back, logs
// PASS/FAIL. Remove once NVS is confirmed on hardware.
//
// NOTE (ESP-4): The wifi_ssid and wifi_pass tests save and restore the
// pre-existing NVS values so the provisioning credentials written by
// captive_portal.c /save are not clobbered on reboot.
// The provisioned flag is NOT written by the self-test.
//
static void nvs_self_test(void)
{
    ESP_LOGI(TAG, "--- NVS self-test START ---");

    esp_err_t err;
    bool all_pass = true;

#define CHECK(label, condition)                     \
    do {                                            \
        if (condition) {                            \
            ESP_LOGI(TAG, "PASS: %s", label);       \
        } else {                                    \
            ESP_LOGE(TAG, "FAIL: %s", label);       \
            all_pass = false;                       \
        }                                           \
    } while (0)

    // --- wifi_ssid (save/restore so provisioned credentials survive) ---
    {
        char saved_ssid[64] = {0};
        nvs_get_wifi_ssid(saved_ssid, sizeof(saved_ssid));  /* read current */

        const char *write_val = "TestSSID";
        char read_buf[64] = {0};
        err = nvs_store_wifi_ssid(write_val);
        CHECK("nvs_store_wifi_ssid returned OK", err == ESP_OK);
        err = nvs_get_wifi_ssid(read_buf, sizeof(read_buf));
        CHECK("nvs_get_wifi_ssid returned OK", err == ESP_OK);
        CHECK("wifi_ssid value matches", strcmp(read_buf, write_val) == 0);

        /* Restore whatever was there before the test. */
        nvs_store_wifi_ssid(saved_ssid);
    }

    // --- wifi_password (save/restore) ---
    {
        char saved_pass[128] = {0};
        nvs_get_wifi_password(saved_pass, sizeof(saved_pass));

        const char *write_val = "TestPassword123";
        char read_buf[64] = {0};
        err = nvs_store_wifi_password(write_val);
        CHECK("nvs_store_wifi_password returned OK", err == ESP_OK);
        err = nvs_get_wifi_password(read_buf, sizeof(read_buf));
        CHECK("nvs_get_wifi_password returned OK", err == ESP_OK);
        CHECK("wifi_password value matches", strcmp(read_buf, write_val) == 0);

        nvs_store_wifi_password(saved_pass);
    }

    // --- device_name ---
    {
        const char *write_val = "embers-test";
        char read_buf[64] = {0};
        err = nvs_store_device_name(write_val);
        CHECK("nvs_store_device_name returned OK", err == ESP_OK);
        err = nvs_get_device_name(read_buf, sizeof(read_buf));
        CHECK("nvs_get_device_name returned OK", err == ESP_OK);
        CHECK("device_name value matches", strcmp(read_buf, write_val) == 0);
    }

    // --- device_role ---
    {
        uint8_t write_val = 2;
        uint8_t read_val = 0;
        err = nvs_store_device_role(write_val);
        CHECK("nvs_store_device_role returned OK", err == ESP_OK);
        err = nvs_get_device_role(&read_val);
        CHECK("nvs_get_device_role returned OK", err == ESP_OK);
        CHECK("device_role value matches", read_val == write_val);
    }

    // --- brightness ---
    {
        uint16_t write_val = 750;
        uint16_t read_val = 0;
        err = nvs_store_brightness(write_val);
        CHECK("nvs_store_brightness returned OK", err == ESP_OK);
        err = nvs_get_brightness(&read_val);
        CHECK("nvs_get_brightness returned OK", err == ESP_OK);
        CHECK("brightness value matches", read_val == write_val);
    }

    // --- provisioned: NOT written by self-test (managed by captive_portal.c) ---

    _Static_assert(sizeof(NVS_DEFAULT_DEVICE_NAME) > 1,
                   "Default device name must not be empty");

#undef CHECK

    if (all_pass) {
        ESP_LOGI(TAG, "--- NVS self-test COMPLETE: ALL PASS ---");
    } else {
        ESP_LOGE(TAG, "--- NVS self-test COMPLETE: ONE OR MORE FAILURES ---");
    }
}

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

    // TEMPORARY: NVS self-test (wifi keys save/restored; provisioned not touched)
    if (nvs_err == ESP_OK) {
        nvs_self_test();
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
