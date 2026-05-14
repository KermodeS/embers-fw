#pragma once

#include "esp_err.h"

/**
 * uart_bridge.h — ESP-6: UART2 byte-pump between UART0 (FT232RL console)
 * and UART2 (STM32 USART2 link). PA2/PA3 on STM32 side; GPIO17/GPIO16 on
 * ESP32 side (standard UART2 defaults on ESP32-WROOM-32).
 *
 * Call uart_bridge_start() once from the GOT_IP handler (after mDNS).
 * The two FreeRTOS tasks run permanently for the life of the firmware.
 *
 * uart_bridge_send_line() is the future web-UI write path — writes a
 * null-terminated string plus '\n' to UART2 directly.
 */

/**
 * Initialise UART2 and spawn the two bridge tasks (once only).
 * Safe to call from any task context after the scheduler is running.
 * Returns ESP_OK on success, ESP_ERR_INVALID_STATE if already started.
 */
esp_err_t uart_bridge_start(void);

/**
 * Write a null-terminated string followed by '\n' to UART2.
 * Intended for use by the future web UI layer; not exercised in ESP-6.
 * Must only be called after uart_bridge_start() has returned ESP_OK.
 */
void uart_bridge_send_line(const char *s);
