#include "mdns_hub.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "esp_log.h"
#include "esp_mac.h"
#include "mdns.h"

static const char *TAG = "mdns_hub";

static bool s_started = false;

esp_err_t mdns_hub_start(void)
{
    if (s_started) {
        ESP_LOGW(TAG, "mdns_hub_start called more than once — ignored");
        return ESP_OK;
    }

    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    char suffix[5];
    snprintf(suffix, sizeof(suffix), "%02x%02x", mac[4], mac[5]);

    char hostname[32];
    snprintf(hostname, sizeof(hostname), "embers-hub-%s", suffix);

    char instance[32];
    snprintf(instance, sizeof(instance), "Embers Hub %s", suffix);

    esp_err_t err = mdns_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_init failed: %s", esp_err_to_name(err));
        return err;
    }

    err = mdns_hostname_set(hostname);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_hostname_set failed: %s", esp_err_to_name(err));
        return err;
    }

    err = mdns_instance_name_set(instance);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_instance_name_set failed: %s", esp_err_to_name(err));
        return err;
    }

    mdns_txt_item_t txt[] = {
        { "role",       "hub"   },
        { "fw_version", "1.0.0" },
        { "api",        "0"     },
    };

    err = mdns_service_add(instance, "_embers", "_tcp", 80, txt, 3);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_service_add failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "MDNS_STARTED hostname=%s.local", hostname);
    ESP_LOGI(TAG, "MDNS_SERVICE_ADDED type=_embers._tcp port=80");

    s_started = true;
    return ESP_OK;
}
