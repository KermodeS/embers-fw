#include "captive_portal.h"
#include "portal_page.h"
#include "dns_hijack.h"
#include "nvs_storage.h"

#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static const char *TAG = "cap_portal";

/* -------------------------------------------------------------------------
 * Helpers
 * ---------------------------------------------------------------------- */

/* URL-decode a percent-encoded string in-place. Returns decoded length. */
static int url_decode(char *dst, const char *src, int dst_max)
{
    int di = 0;
    for (int si = 0; src[si] && di < dst_max - 1; si++) {
        if (src[si] == '+') {
            dst[di++] = ' ';
        } else if (src[si] == '%' && src[si+1] && src[si+2]) {
            char hex[3] = { src[si+1], src[si+2], '\0' };
            dst[di++] = (char)strtol(hex, NULL, 16);
            si += 2;
        } else {
            dst[di++] = src[si];
        }
    }
    dst[di] = '\0';
    return di;
}

/* Extract a key=value field from an application/x-www-form-urlencoded body.
 * Writes the decoded value into out_buf (max out_len bytes incl. NUL).
 * Returns true if the key was found. */
static bool form_get_field(const char *body, const char *key,
                            char *out_buf, int out_len)
{
    char search[48];
    snprintf(search, sizeof(search), "%s=", key);
    const char *p = strstr(body, search);
    if (!p) { out_buf[0] = '\0'; return false; }
    p += strlen(search);
    /* Find end of value (next '&' or end of string). */
    const char *end = strchr(p, '&');
    int raw_len = end ? (int)(end - p) : (int)strlen(p);
    char raw[256];
    if (raw_len >= (int)sizeof(raw)) raw_len = (int)sizeof(raw) - 1;
    memcpy(raw, p, raw_len);
    raw[raw_len] = '\0';
    url_decode(out_buf, raw, out_len);
    return true;
}

/* -------------------------------------------------------------------------
 * Reboot timer callback — called 2 s after /save succeeds.
 * ---------------------------------------------------------------------- */
static void reboot_timer_cb(TimerHandle_t xTimer)
{
    ESP_LOGI(TAG, "PROV_REBOOTING");
    esp_restart();
}

static void schedule_reboot(void)
{
    TimerHandle_t t = xTimerCreate("prov_reboot",
                                   pdMS_TO_TICKS(2000),
                                   pdFALSE, NULL,
                                   reboot_timer_cb);
    if (t) xTimerStart(t, 0);
}

/* -------------------------------------------------------------------------
 * HTTP handlers
 * ---------------------------------------------------------------------- */

/* GET / — serve the portal page */
static esp_err_t handler_root(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, PORTAL_PAGE_HTML, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* GET /scan — trigger a WiFi scan and return JSON */
static esp_err_t handler_scan(httpd_req_t *req)
{
    /* Start a blocking scan (all channels, passive where allowed). */
    wifi_scan_config_t scan_cfg = {
        .ssid        = NULL,
        .bssid       = NULL,
        .channel     = 0,
        .show_hidden = false,
        .scan_type   = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 100,
        .scan_time.active.max = 300,
    };
    esp_wifi_scan_start(&scan_cfg, true);   /* blocking */

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    if (ap_count > 20) ap_count = 20;       /* cap for response size */

    wifi_ap_record_t *ap_list = NULL;
    char *json = NULL;

    if (ap_count > 0) {
        ap_list = calloc(ap_count, sizeof(wifi_ap_record_t));
        if (!ap_list) { ap_count = 0; goto send; }
        esp_wifi_scan_get_ap_records(&ap_count, ap_list);

        /* Bubble-sort by RSSI descending (simple, small N). */
        for (int i = 0; i < ap_count - 1; i++) {
            for (int j = i + 1; j < ap_count; j++) {
                if (ap_list[j].rssi > ap_list[i].rssi) {
                    wifi_ap_record_t tmp = ap_list[i];
                    ap_list[i] = ap_list[j];
                    ap_list[j] = tmp;
                }
            }
        }

        /* Deduplicate by SSID (keep first = strongest). */
        for (int i = 0; i < ap_count; i++) {
            if (ap_list[i].ssid[0] == '\0') continue;   /* hidden — skip */
            for (int j = i + 1; j < ap_count; j++) {
                if (strcmp((char *)ap_list[i].ssid,
                           (char *)ap_list[j].ssid) == 0) {
                    ap_list[j].ssid[0] = '\0';   /* mark as duplicate */
                }
            }
        }
    }

send:;
    /* Build JSON. Worst case per entry: ~80 chars. 20 entries + envelope = ~1700. */
    int json_sz = 64 + ap_count * 80;
    json = malloc(json_sz);
    if (!json) {
        if (ap_list) free(ap_list);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    int pos = 0;
    pos += snprintf(json + pos, json_sz - pos, "{\"networks\":[");
    bool first = true;
    for (int i = 0; i < ap_count; i++) {
        if (ap_list && ap_list[i].ssid[0] == '\0') continue;  /* hidden/dup */
        if (!first) pos += snprintf(json + pos, json_sz - pos, ",");
        first = false;
        pos += snprintf(json + pos, json_sz - pos,
                        "{\"ssid\":\"%s\",\"rssi\":%d,\"auth\":%d}",
                        ap_list ? (char *)ap_list[i].ssid : "",
                        ap_list ? ap_list[i].rssi : 0,
                        ap_list ? ap_list[i].authmode : 0);
    }
    pos += snprintf(json + pos, json_sz - pos, "]}");

    int visible_count = 0;
    for (int i = 0; i < ap_count; i++) {
        if (ap_list && ap_list[i].ssid[0] != '\0') visible_count++;
    }
    ESP_LOGI(TAG, "PROV_SCAN_RESULTS count=%d", visible_count);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, pos);

    free(json);
    if (ap_list) free(ap_list);
    return ESP_OK;
}

/* POST /save — receive credentials, write NVS, schedule reboot */
static esp_err_t handler_save(httpd_req_t *req)
{
    /* Read body (up to 256 bytes — ssid 32 + pass 63 + encoding overhead). */
    char body[256] = {0};
    int body_len = req->content_len;
    if (body_len <= 0 || body_len >= (int)sizeof(body)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "bad body length");
        ESP_LOGI(TAG, "PROV_VALIDATION_FAIL reason=body_length");
        return ESP_FAIL;
    }
    int received = httpd_req_recv(req, body, body_len);
    if (received <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "recv error");
        ESP_LOGI(TAG, "PROV_VALIDATION_FAIL reason=recv_error");
        return ESP_FAIL;
    }
    body[received] = '\0';

    /* Extract and validate ssid / pass. */
    char ssid[64] = {0};
    char pass[128] = {0};
    form_get_field(body, "ssid", ssid, sizeof(ssid));
    form_get_field(body, "pass", pass, sizeof(pass));

    int ssid_len = strlen(ssid);
    int pass_len = strlen(pass);

    if (ssid_len < 1 || ssid_len > 32) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "ssid must be 1-32 characters");
        ESP_LOGI(TAG, "PROV_VALIDATION_FAIL reason=ssid_length");
        return ESP_FAIL;
    }
    if (pass_len > 63) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "password must be 0-63 characters");
        ESP_LOGI(TAG, "PROV_VALIDATION_FAIL reason=pass_length");
        return ESP_FAIL;
    }

    /* Write credentials and provisioning state to NVS. */
    esp_err_t err;
    err = nvs_store_wifi_ssid(ssid);
    if (err != ESP_OK) goto nvs_fail;
    err = nvs_store_wifi_password(pass);
    if (err != ESP_OK) goto nvs_fail;
    err = nvs_store_provisioned(true);
    if (err != ESP_OK) goto nvs_fail;

    ESP_LOGI(TAG, "PROV_SAVED ssid=%s", ssid);

    /* Respond before rebooting. */
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "Saved. Restarting\xe2\x80\xa6", HTTPD_RESP_USE_STRLEN);

    schedule_reboot();
    return ESP_OK;

nvs_fail:
    ESP_LOGE(TAG, "NVS write failed: %s", esp_err_to_name(err));
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "NVS write failed");
    return ESP_FAIL;
}

/* Captive-portal detection redirect — used by Android, iOS, Windows. */
static esp_err_t handler_redirect(httpd_req_t *req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "http://192.168.4.1/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

/* -------------------------------------------------------------------------
 * WiFi event handlers
 * ---------------------------------------------------------------------- */
static void ap_event_handler(void *arg, esp_event_base_t base,
                              int32_t id, void *data)
{
    if (base != WIFI_EVENT) return;
    if (id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *e = data;
        ESP_LOGI(TAG, "PROV_CLIENT_CONNECTED mac=%02x:%02x:%02x:%02x:%02x:%02x",
                 e->mac[0], e->mac[1], e->mac[2],
                 e->mac[3], e->mac[4], e->mac[5]);
    } else if (id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *e = data;
        ESP_LOGI(TAG, "PROV_CLIENT_DISCONNECTED mac=%02x:%02x:%02x:%02x:%02x:%02x reason=%d",
                 e->mac[0], e->mac[1], e->mac[2],
                 e->mac[3], e->mac[4], e->mac[5], (int)e->reason);
    }
}

/* -------------------------------------------------------------------------
 * captive_portal_start()
 * ---------------------------------------------------------------------- */
esp_err_t captive_portal_start(void)
{
    esp_err_t err;

    /* Build SSID: "Embers-Setup-<last4 of base MAC, lowercase hex>" */
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    char ap_ssid[32];
    snprintf(ap_ssid, sizeof(ap_ssid), "Embers-Setup-%02x%02x",
             mac[4], mac[5]);

    /* --- WiFi init (AP mode) --- */
    err = esp_netif_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "esp_netif_init failed: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "event loop create failed: %s", esp_err_to_name(err));
        return err;
    }

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(err));
        return err;
    }

    /* Register AP station connect/disconnect events. */
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED,
                                ap_event_handler, NULL);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED,
                                ap_event_handler, NULL);

    err = esp_wifi_set_mode(WIFI_MODE_APSTA);   /* APSTA allows scanning */
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %s", esp_err_to_name(err));
        return err;
    }

    wifi_config_t ap_cfg = {
        .ap = {
            .channel        = 1,
            .authmode       = WIFI_AUTH_OPEN,
            .max_connection = 4,
            .beacon_interval = 100,
        },
    };
    strncpy((char *)ap_cfg.ap.ssid, ap_ssid, sizeof(ap_cfg.ap.ssid));
    ap_cfg.ap.ssid_len = (uint8_t)strlen(ap_ssid);

    err = esp_wifi_set_config(WIFI_IF_AP, &ap_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config(AP) failed: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "PROV_AP_STARTED ssid=%s ip=192.168.4.1", ap_ssid);

    /* --- DNS hijack --- */
    err = dns_hijack_start();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "DNS hijack start failed (captive popup may not appear)");
        /* Non-fatal — the portal is still reachable at 192.168.4.1 */
    }

    /* --- HTTP server --- */
    httpd_config_t http_cfg = HTTPD_DEFAULT_CONFIG();
    http_cfg.uri_match_fn = httpd_uri_match_wildcard;

    httpd_handle_t server = NULL;
    err = httpd_start(&server, &http_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed: %s", esp_err_to_name(err));
        return err;
    }

    /* Register routes. */
    static const httpd_uri_t route_root = {
        .uri = "/", .method = HTTP_GET, .handler = handler_root
    };
    static const httpd_uri_t route_scan = {
        .uri = "/scan", .method = HTTP_GET, .handler = handler_scan
    };
    static const httpd_uri_t route_save = {
        .uri = "/save", .method = HTTP_POST, .handler = handler_save
    };
    /* Captive-portal detection endpoints → redirect to /. */
    static const httpd_uri_t route_204 = {
        .uri = "/generate_204", .method = HTTP_GET, .handler = handler_redirect
    };
    static const httpd_uri_t route_hotspot = {
        .uri = "/hotspot-detect.html", .method = HTTP_GET, .handler = handler_redirect
    };
    static const httpd_uri_t route_connecttest = {
        .uri = "/connecttest.txt", .method = HTTP_GET, .handler = handler_redirect
    };
    /* Wildcard catch-all: any other URI → redirect to /. */
    static const httpd_uri_t route_catchall = {
        .uri = "/*", .method = HTTP_GET, .handler = handler_redirect
    };

    httpd_register_uri_handler(server, &route_root);
    httpd_register_uri_handler(server, &route_scan);
    httpd_register_uri_handler(server, &route_save);
    httpd_register_uri_handler(server, &route_204);
    httpd_register_uri_handler(server, &route_hotspot);
    httpd_register_uri_handler(server, &route_connecttest);
    httpd_register_uri_handler(server, &route_catchall);

    ESP_LOGI(TAG, "HTTP server started on 192.168.4.1:80");
    return ESP_OK;
}
