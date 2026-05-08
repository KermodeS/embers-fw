#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_storage.h"

static const char *TAG = "EMBERS";

// ---------------------------------------------------------------------------
// NVS self-test
// ---------------------------------------------------------------------------
//
// TEMPORARY: This self-test writes known values to every NVS key, reads them
// back, and logs PASS or FAIL for each. Remove this function (and its call in
// app_main) once NVS is confirmed working on real hardware.
//
static void nvs_self_test(void)
{
    ESP_LOGI(TAG, "--- NVS self-test START ---");

    esp_err_t err;
    bool all_pass = true;

#define CHECK(label, condition)                          \
    do {                                                 \
        if (condition) {                                 \
            ESP_LOGI(TAG, "PASS: %s", label);           \
        } else {                                         \
            ESP_LOGE(TAG, "FAIL: %s", label);           \
            all_pass = false;                            \
        }                                                \
    } while (0)

    // --- wifi_ssid ---
    {
        const char *write_val = "TestSSID";
        char read_buf[64] = {0};
        err = nvs_store_wifi_ssid(write_val);
        CHECK("nvs_store_wifi_ssid returned OK", err == ESP_OK);
        err = nvs_get_wifi_ssid(read_buf, sizeof(read_buf));
        CHECK("nvs_get_wifi_ssid returned OK", err == ESP_OK);
        CHECK("wifi_ssid value matches", strcmp(read_buf, write_val) == 0);
    }

    // --- wifi_password ---
    {
        const char *write_val = "TestPassword123";
        char read_buf[64] = {0};
        err = nvs_store_wifi_password(write_val);
        CHECK("nvs_store_wifi_password returned OK", err == ESP_OK);
        err = nvs_get_wifi_password(read_buf, sizeof(read_buf));
        CHECK("nvs_get_wifi_password returned OK", err == ESP_OK);
        CHECK("wifi_password value matches", strcmp(read_buf, write_val) == 0);
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

    // --- provisioned ---
    {
        bool write_val = true;
        bool read_val = false;
        err = nvs_store_provisioned(write_val);
        CHECK("nvs_store_provisioned returned OK", err == ESP_OK);
        err = nvs_get_provisioned(&read_val);
        CHECK("nvs_get_provisioned returned OK", err == ESP_OK);
        CHECK("provisioned value matches", read_val == write_val);
    }

    // --- default fallback: device_name default ---
    // Verify that a fresh get (before any store) returns the correct default.
    // We can't easily test this without erasing, so we just verify the
    // defined default constant is correct here as a compile-time sanity check.
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
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  Embers Lighting — ESP32 Firmware");
    ESP_LOGI(TAG, "  Build: " __DATE__ " " __TIME__);
    ESP_LOGI(TAG, "========================================");

    // Log MAC address (used as device serial number by STM32)
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG, "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // NVS init — required before any subsystem that reads/writes flash storage
    esp_err_t nvs_err = nvs_storage_init();
    if (nvs_err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed — storage unavailable");
        // Continue running; downstream reads will return defaults
    }

    // TEMPORARY: NVS self-test — remove once confirmed on real hardware
    if (nvs_err == ESP_OK) {
        nvs_self_test();
    }

    // TODO (ESP-3): WiFi init
    // TODO (ESP-4): mDNS init
    // TODO (ESP-5): WebSocket server init
    // TODO (ESP-6): UART-to-STM32 driver init
    // TODO (ESP-7): ESP-NOW init

    // Main loop — heartbeat
    uint32_t heartbeat_count = 0;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        heartbeat_count++;
        ESP_LOGI(TAG, "Heartbeat #%lu", heartbeat_count);
    }
}
