#include "wifi_sta.h"
#include "nvs_storage.h"

#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_system.h"

#include "lwip/ip4_addr.h"

static const char *TAG = "wifi_sta";

// Slow-retry interval after fast-retry budget is exhausted.
#define WIFI_STA_SLOW_RETRY_MS   30000

// Event-group bits:
//   CONNECTED_BIT set only after IP_EVENT_STA_GOT_IP.
//   SLOW_RETRY_BIT signals the slow-retry task to wake and call esp_wifi_connect.
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_SLOW_RETRY_BIT BIT1

static EventGroupHandle_t s_event_group = NULL;
static int                 s_retry_count = 0;
static esp_netif_t        *s_sta_netif = NULL;
static TaskHandle_t        s_slow_retry_task = NULL;

// Cached SSID for logging (reported in WIFI_CONNECTING / WIFI_CONNECTED).
static char s_ssid_in_use[33] = {0};

// ---------------------------------------------------------------------------
// Credential loading
// ---------------------------------------------------------------------------

static void load_credentials(char *ssid_out, size_t ssid_len,
                             char *pass_out, size_t pass_len)
{
    nvs_get_wifi_ssid(ssid_out, ssid_len);
    nvs_get_wifi_password(pass_out, pass_len);
    ssid_out[ssid_len - 1] = '\0';
    pass_out[pass_len - 1] = '\0';
}

static bool credentials_empty(const char *ssid, const char *pass)
{
    return (ssid == NULL || ssid[0] == '\0' ||
            pass == NULL || pass[0] == '\0');
}

// ---------------------------------------------------------------------------
// Slow-retry task
// ---------------------------------------------------------------------------
//
// After WIFI_STA_MAX_RETRY fast disconnects, this task is signalled to retry
// once every WIFI_STA_SLOW_RETRY_MS forever. On each attempt it resets the
// fast-retry counter, so a successful connection restores the fast-retry
// budget for the next disconnect.
//

static void slow_retry_task(void *arg)
{
    (void)arg;
    for (;;) {
        // Wait for the fast-retry budget to be exhausted.
        xEventGroupWaitBits(s_event_group, WIFI_SLOW_RETRY_BIT,
                            pdFALSE, pdFALSE, portMAX_DELAY);

        // Keep trying until either connected (bit cleared elsewhere) or the
        // event handler clears the slow-retry bit after association.
        while (xEventGroupGetBits(s_event_group) & WIFI_SLOW_RETRY_BIT) {
            vTaskDelay(pdMS_TO_TICKS(WIFI_STA_SLOW_RETRY_MS));
            if ((xEventGroupGetBits(s_event_group) & WIFI_SLOW_RETRY_BIT) == 0) {
                break; // reconnected during the 30 s sleep
            }
            ESP_LOGI(TAG, "WIFI_SLOW_RETRY ssid=%s (interval=%u ms)",
                     s_ssid_in_use, (unsigned)WIFI_STA_SLOW_RETRY_MS);
            s_retry_count = 0;  // give fast-retry budget another chance
            esp_wifi_connect();
        }
    }
}

// ---------------------------------------------------------------------------
// Event handler
// ---------------------------------------------------------------------------

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    (void)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WIFI_STARTED");
        ESP_LOGI(TAG, "WIFI_CONNECTING ssid=%s", s_ssid_in_use);
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t *e = (wifi_event_sta_connected_t *)event_data;
        ESP_LOGI(TAG,
                 "WIFI_CONNECTED ssid=%s bssid=%02x:%02x:%02x:%02x:%02x:%02x channel=%u",
                 s_ssid_in_use,
                 e->bssid[0], e->bssid[1], e->bssid[2],
                 e->bssid[3], e->bssid[4], e->bssid[5],
                 (unsigned)e->channel);
        s_retry_count = 0;
        // Association succeeded; stop the slow-retry pump if it was running.
        if (s_event_group) {
            xEventGroupClearBits(s_event_group, WIFI_SLOW_RETRY_BIT);
        }
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *e = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGW(TAG, "WIFI_DISCONNECTED reason=%u", (unsigned)e->reason);

        if (s_event_group) {
            xEventGroupClearBits(s_event_group, WIFI_CONNECTED_BIT);
        }

        if (s_retry_count < WIFI_STA_MAX_RETRY) {
            s_retry_count++;
            ESP_LOGI(TAG, "WIFI_CONNECTING ssid=%s (retry %d/%d)",
                     s_ssid_in_use, s_retry_count, WIFI_STA_MAX_RETRY);
            esp_wifi_connect();
        } else {
            ESP_LOGE(TAG, "WIFI_GIVEUP (after %d fast retries) - entering slow-retry mode",
                     WIFI_STA_MAX_RETRY);
            if (s_event_group) {
                xEventGroupSetBits(s_event_group, WIFI_SLOW_RETRY_BIT);
            }
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *e = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "GOT_IP addr=" IPSTR " mask=" IPSTR " gw=" IPSTR,
                 IP2STR(&e->ip_info.ip),
                 IP2STR(&e->ip_info.netmask),
                 IP2STR(&e->ip_info.gw));
        s_retry_count = 0;
        if (s_event_group) {
            xEventGroupSetBits(s_event_group, WIFI_CONNECTED_BIT);
            xEventGroupClearBits(s_event_group, WIFI_SLOW_RETRY_BIT);
        }
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

esp_err_t wifi_sta_init(void)
{
    if (s_event_group != NULL) {
        ESP_LOGW(TAG, "wifi_sta_init already called");
        return ESP_OK;
    }

    // Load credentials from NVS. If empty (should not happen on a
    // provisioned device, but guards against NVS corruption), flip
    // provisioned=0 and restart into the captive portal.
    char ssid[33] = {0};
    char pass[65] = {0};
    load_credentials(ssid, sizeof(ssid), pass, sizeof(pass));

    if (credentials_empty(ssid, pass)) {
        ESP_LOGE(TAG, "WIFI_NVS_MISSING — entering provisioning");
        nvs_store_provisioned(0);
        esp_restart();
        return ESP_FAIL; // unreachable; silences compiler warning
    }

    strncpy(s_ssid_in_use, ssid, sizeof(s_ssid_in_use) - 1);

    s_event_group = xEventGroupCreate();
    if (s_event_group == NULL) {
        ESP_LOGE(TAG, "xEventGroupCreate failed");
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = esp_netif_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "esp_netif_init failed: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "esp_event_loop_create_default failed: %s",
                 esp_err_to_name(err));
        return err;
    }

    s_sta_netif = esp_netif_create_default_wifi_sta();
    if (s_sta_netif == NULL) {
        ESP_LOGE(TAG, "esp_netif_create_default_wifi_sta failed");
        return ESP_FAIL;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    if (err != ESP_OK) return err;
    err = esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    if (err != ESP_OK) return err;

    wifi_config_t wifi_cfg = {0};
    strncpy((char *)wifi_cfg.sta.ssid, ssid,
            sizeof(wifi_cfg.sta.ssid) - 1);
    strncpy((char *)wifi_cfg.sta.password, pass,
            sizeof(wifi_cfg.sta.password) - 1);
    wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA_PSK;

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %s", esp_err_to_name(err));
        return err;
    }
    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config failed: %s", esp_err_to_name(err));
        return err;
    }

    // Create the slow-retry task BEFORE esp_wifi_start() so it's ready to
    // receive signals the first time the fast-retry budget is exhausted.
    BaseType_t task_ok = xTaskCreate(
        slow_retry_task, "wifi_slow_retry", 3072, NULL, 4, &s_slow_retry_task);
    if (task_ok != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate(slow_retry_task) failed");
        return ESP_ERR_NO_MEM;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

bool wifi_sta_is_connected(void)
{
    if (s_event_group == NULL) return false;
    EventBits_t bits = xEventGroupGetBits(s_event_group);
    return (bits & WIFI_CONNECTED_BIT) != 0;
}
