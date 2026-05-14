#ifndef UART_RX_H
#define UART_RX_H

#include <stdint.h>

/**
 * uart_rx.h — STM-RX1: PROTO-001 parser for USART2 RX.
 *
 * uart_rx_init()     — Enable USART2 RXNEIE interrupt. Call once during
 *                      peripheral init in main(), after WiFi_USART_Init().
 * uart_rx_poll()     — Process one queued command line per call. Call from
 *                      the main while(1) loop after the IWDG kick.
 * uart_rx_isr_byte() — Feed one received byte into the line buffer.
 *                      Called exclusively from USART2_IRQHandler().
 */

void uart_rx_init(void);
void uart_rx_poll(void);
void uart_rx_isr_byte(uint8_t b);

#endif /* UART_RX_H */
