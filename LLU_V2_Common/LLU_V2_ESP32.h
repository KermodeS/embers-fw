
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LLU_V2_ESP32_H
#define __LLU_V2_ESP32_H

#include "stdbool.h"
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_tim.h"
//
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

// Это универсальная прошивка для LPU_MPU_HPU второй версии


// Управление модулем

// WiFi POWER (Low, Mid, High V2)
#define  WIFI_POWER_PIN                   LL_GPIO_PIN_1
#define  WIFI_POWER_PORT                  GPIOB
#define  WIFI_POWER_GPIO_CLK_ENABLE()     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB) 
//
// WIFI_ENABLE (Low, Mid, High V2). Управление резетом Через линию Enable.
#define  WIFI_ENABLE_PIN                   LL_GPIO_PIN_14
#define  WIFI_ENABLE_PORT                  GPIOC
#define  WIFI_ENABLE_GPIO_CLK_ENABLE()     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)  
//
//
//
// КОММУНИКАЦИЯ С МОДУЛЕМ WiFi
#define WIFI_USART_INSTANCE               USART2
#define WIFI_USART_CLK_ENABLE()           LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
// (Low, Mid, High V2).
#define WIFI_USART_GPIO_CLK_ENABLE()      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)   /* Enable the peripheral clock of GPIOA */
#define WIFI_USART_TX_PIN                 LL_GPIO_PIN_2
#define WIFI_USART_TX_GPIO_PORT           GPIOA
#define WIFI_USART_SET_TX_GPIO_AF()       LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_7)
#define WIFI_USART_RX_PIN                 LL_GPIO_PIN_3
#define WIFI_USART_RX_GPIO_PORT           GPIOA
#define WIFI_USART_SET_RX_GPIO_AF()       LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_7)
#define APB_Div 2
//
// __STATIC_INLINE 
void     WiFi_Power_Init(void);
// __STATIC_INLINE 
void     WiFi_ENABLE_Init(void);
//
//
void ProcessWiFiSerial(void);
void WiFi_ProcessRx( void );
void WiFi_ProcessTx( void );
//
void Process_WiFi_IncomingString( void );
//
//
// ЧТО ЭТО? Нужно как селектор буфера отправки
// На данный момент используется только один буфер
#define WIFI_TX_BUF_A_FAST 0
#define WIFI_TX_BUF_B_FAST 1
//
#define WIFI_TX_BUF_X 2
//
// для быстрых пакетов
// 2 байта - синхроначало
// 11 байт  данных
// 2 байта - синхростоп
#define WIFI_TX_BUF_SIZE   ( 15 )
//
//
void WiFi_USART_Init(void);
//
//
// ничего не приняли или приняли и успели обработать
#define  WIFI_COMMAND_IDLE                   0
// 
#define  WIFI_COMMAND_SET_RGB                1 
#define  WIFI_COMMAND_SET_WHITE_LIGHT_LEVEL  2
#define  WIFI_COMMAND_SET_UV_LIGHT_LEVEL     3
//
#define  WIFI_COMMAND_SET_ACTUAL_WHITE       4
#define  WIFI_COMMAND_SET_ACTUAL_UV          5
//
#define  WIFI_COMMAND_SET_MODE_MANUAL        6
#define  WIFI_COMMAND_SET_MODE_AUTO          7
//
#define  WIFI_COMMAND_SET_SYNC_INDEPENDENT   8
#define  WIFI_COMMAND_SET_SYNC_GROUPED       9
//
#define  WIFI_COMMAND_SET_CHANGE_SPEED      10
//
#define  WIFI_COMMAND_SET_AUTO_PLAY         11
#define  WIFI_COMMAND_SET_AUTO_PAUS         12
#define  WIFI_COMMAND_SET_AUTO_STOP         13
#define  WIFI_COMMAND_SET_SINGLE_STROBE     14
//
//
void Process_ESP32_LEDS(void);
//
__STATIC_INLINE void CreateSend_ESP32_LEDS_Packet(void);

// LED1 - state of the communication module.
// blinking 1 Hz - no connection.
// 1 short pulse per second - works as an access point (is a master)
// 2 short pulses per second - works as a node (connected to the master)
// flashing 3 Hz - changing status
#define LED_1_DEFAULT       0
#define LED_1_NoConnection  1
#define LED_1_AP_Mode       2
#define LED_1_Node_Mode     3
#define LED_1_ChangeState   4
//
// LED2 - Hierarchy status.
// 	flashing 1 Hz - Independent operation.
//      1 short pulse per second - works as a Master (is a master)
//      2 short pulses per second - works as EchoSlave (connected to master)
//      3 short pulses per second - works as a Slave (connected to the master)
//      2+2 are short in 2 seconds - works as autonomous (connected to the master)
#define LED_2_DEFAULT    0
#define LED_2_INDEPENDED 1
#define LED_2_MASTER     2
#define LED_2_ECHO_SLAVE 3
#define LED_2_SLAVE      4
#define LED_2_AUTONOMOUS 5
//
//
//
//      LED3- Receiving a sync message from the radio
//      	does not blink - no synchronization
//      1 short pulse per second - Sent sync (is the master)
//      2 short pulses per second - Received sync (connected to master)
#define LED_3_DEFAULT      0
#define LED_3_NoSync       1
#define LED_3_SentSync     2
#define LED_3_RecieveSync  3
// Время сбоя синхронизации, мс
#define SYNC_LOST_TIME_MKS (uint32_t)2000000
//
//      LED4 - Sync message reception via IR
//      	does not blink - no synchronization
//      1 short pulse per second - Sent sync (is the master)
//      2 short pulses per second - Received sync (connected to master)
#define LED_4_DEFAULT     0
#define LED_4_NoSync      1
#define LED_4_SentSync    2
#define LED_4_RecieveSync 3
//
//      LED5 - Receiving a command from the IR remote control
//      1 short pulse per second - Received a command (is master or autonomous)
#define LED_5_DEFAULT     0
#define LED_5_IR_COMMAND  1
#define LED_5_IR_KNOWN    2
//
//LED6 - Battery status (LPU). Input voltage level (MPU, HPU).
#define LED_6_DEFAULT    0
#define LED_6_SOC_01     1
#define LED_6_SOC_02     2
#define LED_6_SOC_05     3
#define LED_6_SOC_10     4
#define LED_6_SOC_20     5
#define LED_6_SOC_30     6
#define LED_6_SOC_40     7
#define LED_6_SOC_50     8
#define LED_6_SOC_60     9
#define LED_6_SOC_70    10
#define LED_6_SOC_80    11
#define LED_6_SOC_90    12
#define LED_6_SOC_100   13 
//
//
// LED7 - pressing the built-in keyboard
#define LED_7_DEFAULT      0
#define LED_7_KBD_PRESSED  1
#define LED_7_KBD_CLICK    2
#define LED_7_KBD_DBL      3
#define LED_7_KBD_UP       4
#define LED_7_KBD_WAIT_FOR_PRESS_OFF  5
//
//
//
// LED8 - Sync Clock quality
#define LED_8_DEFAULT     0
#define LED_8_Sync_01     1
#define LED_8_Sync_02     2
#define LED_8_Sync_05     3
#define LED_8_Sync_10     4
#define LED_8_Sync_20     5
#define LED_8_Sync_30     6
#define LED_8_Sync_40     7
#define LED_8_Sync_50     8
#define LED_8_Sync_60     9
#define LED_8_Sync_70    10
#define LED_8_Sync_80    11
#define LED_8_Sync_90    12
#define LED_8_Sync_100   13 
//
//
// В модуль идет поток управления светодиодами. В нем есть три резервных байта, вернее три полубайта.
// первый полубайте это управление режимом (смена режима модуля, ретрансляция команды в другие модули)
// второй полубайт это параметр 1 (TBD) и параметр 2 (TBD).
// Первый полубайт. режим.
#define MODE_BYTE_IDLE        0
#define MODE_BYTE_WIFI_MODE   1
#define MODE_BYTE_UNIT_INDEX  2
#define MODE_BYTE_IR_CMD      3
#define MODE_BYTE_BTN_CMD     4
#define MODE_BYTE_BRT_CMD     5
#define MODE_BYTE_OP_MOD_CMD  6
#define MODE_BYTE_RES_6       7
#define MODE_BYTE_RES_7       8
#define MODE_BYTE_RES_8       9
#define MODE_BYTE_RES_9      10
#define MODE_BYTE_RES_10     11
#define MODE_BYTE_RES_11     12
#define MODE_BYTE_RES_12     13
#define MODE_BYTE_RES_13     14
#define MODE_BYTE_RES_14     15
//
// Второй байт с параметром передается по половине в каждом байте. Байт содержит саму половину и ее инверсию.
#define MODE_PARAM_HIGH_NIBBLE_IDLE 0
#define MODE_PARAM_LOW_NIBBLE_IDLE 0
//
//
// для MODE_BYTE_WIFI_MODE  
// Второй полубайт. параметр режима.
// для MODE_BYTE_WIFI_MODE 
#define WIFI_MODE_IDLE  (uint8_t)0
#define WIFI_MODE_AP             1
#define WIFI_MODE_NODE           2
#define WIFI_MODE_ECHO_NODE      3
//
// для MODE_BYTE_UNIT_INDEX 
// Каждому модулю (юниту) при инициализации присваивается свой индекс
// в зависимости от его роли в иерархии. 
// По идее - индекс лежит в диапазоне от 1 до 240.
// индекс может повышаться, понижаться. Для чего? 
// Изначально, на момент включения индекс соответствует серийному номеру.
#define UNIT_INDEX_IDLE (uint8_t)(0)
#define UNIT_INDEX_001  1
#define UNIT_INDEX_002  2
#define UNIT_INDEX_003  3
#define UNIT_INDEX_004  4
#define UNIT_INDEX_005  5
#define UNIT_INDEX_006  6
#define UNIT_INDEX_007  7
#define UNIT_INDEX_008  8
#define UNIT_INDEX_009  9


// для MODE_BYTE_IR_CMD     
#define IR_CMD_IDLE     (uint8_t)0
// ПЕРВАЯ ЧАСТЬ ПАРАМЕТРА
// первый ряд из 4 кнопок.
#define IR_CMD_UP      1
#define IR_CMD_DOWN    2
#define IR_CMD_OFF     3 
#define IR_CMD_ON      4
// 2 ряд из 4 кнопок.
#define IR_CMD_RED     5
#define IR_CMD_GRN     6
#define IR_CMD_BLU     7
#define IR_CMD_WHT     8
// 3 ряд из 4 кнопок.
#define IR_CMD_ORG1    9
#define IR_CMD_GRN1    10
#define IR_CMD_BLU1    11
#define IR_CMD_F1      12
#define IR_CMD_UV IR_CMD_F1   
// 4 ряд из 4 кнопок.
#define IR_CMD_ORG2    13
#define IR_CMD_GRN2    14
#define IR_CMD_PRPL1   15 
//
//  Как видно из разрядности - диапазон превысил 4 бита и следовательно
// для полноценной отпаравки потребуется два полубайта.
//
// ВТОРАЯ ЧАСТЬ ПАРАМЕТРА    <<<----- ВНИМАНИЕ. Это в Параметр2.
#define IR_CMD_S1      16
#define IR_CMD_STRB  IR_CMD_S1    
// 5 ряд из 4 кнопок.
#define IR_CMD_ORG3    17
#define IR_CMD_GRN3    18
#define IR_CMD_PRPL2   19
#define IR_CMD_S2      20 
#define IR_CMD_SEQ IR_CMD_S2
// 6 ряд из 4 кнопок.
#define IR_CMD_YLW     21 
#define IR_CMD_GRN4    22  
#define IR_CMD_PRPL3   23
#define IR_CMD_MNL     IR_CMD_PRPL3
#define IR_CMD_F2      24
#define IR_CMD_RNB  IR_CMD_F2   
//
//
// MODE_BYTE_BTN_CMD    
#define BTN_CMD_IDLE (uint8_t)0
//
#define BTN_CMD_UP_CLICK     1
#define BTN_CMD_UP_PRESS     2
#define BTN_CMD_UP_DBL       3
#define BTN_CMD_UP_RELEASE   4
//
#define BTN_CMD_DOWN_CLICK   5  
#define BTN_CMD_DOWN_PRESS   6  
#define BTN_CMD_DOWN_DBL     7  
#define BTN_CMD_DOWN_RELEASE 8  
//
#define BTN_CMD_PREV_CLICK   9  
#define BTN_CMD_PREV_PRESS   10  
#define BTN_CMD_PREV_DBL     11  
#define BTN_CMD_PREV_RELEASE 12  
//
#define BTN_CMD_NEXT_CLICK   13  
#define BTN_CMD_NEXT_PRESS   14  
#define BTN_CMD_NEXT_DBL     15  
#define BTN_CMD_NEXT_RELEASE 16  
//
#define BTN_CMD_MODE_CLICK   17  
#define BTN_CMD_MODE_PRESS   18  
#define BTN_CMD_MODE_DBL     19  
#define BTN_CMD_MODE_RELEASE 20                             
//
//
// Дополнение для управления яркостью
// MODE_BYTE_BRT_CMD 
#define BRT_CMD_IDLE   0
#define BRT_CMD_R      1
#define BRT_CMD_G      2
#define BRT_CMD_B      3
#define BRT_CMD_U      4
#define BRT_CMD_W      5
//
// Данные о канале передаются в первом параметре
// Данные о яркости передаются в втором параметре.
// 0, 1, ... 10 это 0, 10%, ... 100% яркости.
//
//
// Дополнение для управления режимом устройства
#define OP_MODE_CMD_IDLE    0
#define OP_MODE_CMD_REGULAR 1
#define OP_MODE_CMD_SERVICE 2
//
//
//
//
#endif /* __LLU_V2_ESP32_H */