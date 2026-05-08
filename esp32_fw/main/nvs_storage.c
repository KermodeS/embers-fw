#include "nvs_storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "NVS";

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------

esp_err_t nvs_storage_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition problem (%s) — erasing and reinitialising",
                 esp_err_to_name(ret));
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "nvs_flash_erase failed: %s", esp_err_to_name(ret));
            return ret;
        }
        ret = nvs_flash_init();
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "NVS initialised OK");
    }
    return ret;
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

/**
 * Open the "embers" namespace and store a string value, then close the handle.
 */
static esp_err_t _store_str(const char *key, const char *value)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed for key '%s': %s", key, esp_err_to_name(ret));
        return ret;
    }
    ret = nvs_set_str(handle, key, value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_set_str failed for key '%s': %s", key, esp_err_to_name(ret));
    } else {
        ret = nvs_commit(handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "nvs_commit failed for key '%s': %s", key, esp_err_to_name(ret));
        }
    }
    nvs_close(handle);
    return ret;
}

/**
 * Open the "embers" namespace, read a string value, fall back to default if
 * the key is not found, then close the handle.
 */
static esp_err_t _get_str(const char *key, char *buf, size_t len,
                           const char *default_val)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (ret != ESP_OK) {
        // Namespace not yet created — return the default silently
        strncpy(buf, default_val, len - 1);
        buf[len - 1] = '\0';
        return ESP_OK;
    }
    size_t required = len;
    ret = nvs_get_str(handle, key, buf, &required);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        strncpy(buf, default_val, len - 1);
        buf[len - 1] = '\0';
        ret = ESP_OK;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_get_str failed for key '%s': %s", key, esp_err_to_name(ret));
    }
    nvs_close(handle);
    return ret;
}

/**
 * Store a uint8 value.
 */
static esp_err_t _store_u8(const char *key, uint8_t value)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed for key '%s': %s", key, esp_err_to_name(ret));
        return ret;
    }
    ret = nvs_set_u8(handle, key, value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_set_u8 failed for key '%s': %s", key, esp_err_to_name(ret));
    } else {
        ret = nvs_commit(handle);
    }
    nvs_close(handle);
    return ret;
}

/**
 * Read a uint8 value, returning default if key not found.
 */
static esp_err_t _get_u8(const char *key, uint8_t *out, uint8_t default_val)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (ret != ESP_OK) {
        *out = default_val;
        return ESP_OK;
    }
    ret = nvs_get_u8(handle, key, out);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        *out = default_val;
        ret = ESP_OK;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_get_u8 failed for key '%s': %s", key, esp_err_to_name(ret));
    }
    nvs_close(handle);
    return ret;
}

/**
 * Store a uint16 value.
 */
static esp_err_t _store_u16(const char *key, uint16_t value)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed for key '%s': %s", key, esp_err_to_name(ret));
        return ret;
    }
    ret = nvs_set_u16(handle, key, value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_set_u16 failed for key '%s': %s", key, esp_err_to_name(ret));
    } else {
        ret = nvs_commit(handle);
    }
    nvs_close(handle);
    return ret;
}

/**
 * Read a uint16 value, returning default if key not found.
 */
static esp_err_t _get_u16(const char *key, uint16_t *out, uint16_t default_val)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (ret != ESP_OK) {
        *out = default_val;
        return ESP_OK;
    }
    ret = nvs_get_u16(handle, key, out);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        *out = default_val;
        ret = ESP_OK;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_get_u16 failed for key '%s': %s", key, esp_err_to_name(ret));
    }
    nvs_close(handle);
    return ret;
}

// ---------------------------------------------------------------------------
// WiFi credentials
// ---------------------------------------------------------------------------

esp_err_t nvs_store_wifi_ssid(const char *ssid)
{
    return _store_str(NVS_KEY_WIFI_SSID, ssid);
}

esp_err_t nvs_get_wifi_ssid(char *buf, size_t len)
{
    return _get_str(NVS_KEY_WIFI_SSID, buf, len, NVS_DEFAULT_WIFI_SSID);
}

esp_err_t nvs_store_wifi_password(const char *password)
{
    return _store_str(NVS_KEY_WIFI_PASS, password);
}

esp_err_t nvs_get_wifi_password(char *buf, size_t len)
{
    return _get_str(NVS_KEY_WIFI_PASS, buf, len, NVS_DEFAULT_WIFI_PASS);
}

// ---------------------------------------------------------------------------
// Device settings
// ---------------------------------------------------------------------------

esp_err_t nvs_store_device_name(const char *name)
{
    return _store_str(NVS_KEY_DEVICE_NAME, name);
}

esp_err_t nvs_get_device_name(char *buf, size_t len)
{
    return _get_str(NVS_KEY_DEVICE_NAME, buf, len, NVS_DEFAULT_DEVICE_NAME);
}

esp_err_t nvs_store_device_role(uint8_t role)
{
    return _store_u8(NVS_KEY_DEVICE_ROLE, role);
}

esp_err_t nvs_get_device_role(uint8_t *role)
{
    return _get_u8(NVS_KEY_DEVICE_ROLE, role, NVS_DEFAULT_DEVICE_ROLE);
}

esp_err_t nvs_store_brightness(uint16_t brightness)
{
    return _store_u16(NVS_KEY_BRIGHTNESS, brightness);
}

esp_err_t nvs_get_brightness(uint16_t *brightness)
{
    return _get_u16(NVS_KEY_BRIGHTNESS, brightness, NVS_DEFAULT_BRIGHTNESS);
}

// ---------------------------------------------------------------------------
// Provisioning state — stored as uint8 (0/1) since NVS has no bool type
// ---------------------------------------------------------------------------

esp_err_t nvs_store_provisioned(bool provisioned)
{
    return _store_u8(NVS_KEY_PROVISIONED, provisioned ? 1u : 0u);
}

esp_err_t nvs_get_provisioned(bool *provisioned)
{
    uint8_t val;
    esp_err_t ret = _get_u8(NVS_KEY_PROVISIONED, &val, 0u);
    if (ret == ESP_OK) {
        *provisioned = (val != 0);
    }
    return ret;
}
