/*
 * embers_main.c
 *
 * Embers Lighting — LLU V2
 * ESP32-WROOM-32 Application Firmware
 * Version: 0.1.0
 *
 * This is the top-level application entry point.
 * It initialises logging, reads and logs the device MAC address,
 * then enters the main loop with a 5-second heartbeat.
 *
 * Subsystem init stubs are marked with TODO comments.
 * Each TODO corresponds to a future development session.
 *
 * Hardware: ESP32-WROOM-32 on Embers MPU V2 board
 * Toolchain: ESP-IDF v5.5.4
 * Build: idf.py build  (always separate from flash)
 * Flash: idf.py -p /dev/ttyUSB0 flash
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_mac.h"

/* -------------------------------------------------------------------------
 * Constants
 * ----------------------------------------------------------------------- */

static const char *TAG = "EMBERS";

/* Heartbeat period in milliseconds */
#define HEARTBEAT_PERIOD_MS     5000

/* -------------------------------------------------------------------------
 * app_main
 * ----------------------------------------------------------------------- */

void app_main(void)
{
    /* --- Read MAC address ------------------------------------------------
     * The MAC is the permanent hardware identity of this ESP32 module.
     * The STM32 will later use this as the device serial number (per
     * the original softAP_0250 firmware and PROTO-001 spec).
     */
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    /* --- Startup banner -------------------------------------------------- */
    ESP_LOGI(TAG, "=================================");
    ESP_LOGI(TAG, "Embers Lighting — LLU V2");
    ESP_LOGI(TAG, "ESP32 Firmware v0.1.0");
    ESP_LOGI(TAG, "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ESP_LOGI(TAG, "=================================");

    /* --- Subsystem initialisation (stubs for future sessions) ----------- */

    /* TODO ESP-2: NVS (Non-Volatile Storage) init
     * Required before WiFi credentials or settings can be read/written.
     * nvs_flash_init() must be called before esp_wifi_init().
     */

    /* TODO ESP-3: WiFi init
     * Station mode — connect to home router using credentials from NVS.
     * First-boot captive portal provisioning when no credentials stored.
     */

    /* TODO ESP-4: mDNS announcement
     * Advertise device as embers-<last4mac>.local on the local network.
     */

    /* TODO ESP-5: WebSocket server
     * Real-time bidirectional control channel for the web UI.
     * Listens on ws://<device-ip>/ws
     */

    /* TODO ESP-6: UART driver to STM32
     * Sends light control commands received via WebSocket down to STM32.
     * Receives state updates from STM32 (per PROTO-001 spec).
     * STM32 TX -> ESP32 RX: PA3/USART2_RX
     * STM32 RX -> ESP32 TX: PA2/USART2_TX
     * Baud: 115200, 8N1
     */

    /* TODO ESP-7: ESP-NOW inter-device sync
     * Hub role: discover peers, relay commands for group mode.
     * Replaces old UDP broadcast from softAP_0250.
     */

    /* --- Main loop ------------------------------------------------------- */
    uint32_t heartbeat_count = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_PERIOD_MS));
        heartbeat_count++;
        ESP_LOGI(TAG, "Heartbeat #%lu -- idle, awaiting subsystems", heartbeat_count);
    }
}
