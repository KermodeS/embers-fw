/**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput/Inc/main.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-February-2017
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

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
#define UNDEF_DEVICE      0 
#define LOW_POWER_DEVICE  1
#define MID_POWER_DEVICE  2
#define HIGH_POWER_DEVICE 3
/**
  * @brief LED2 
  */
// РУДИМЕНТ
//#define LED2_PIN                           LL_GPIO_PIN_1
//#define LED2_GPIO_PORT                     GPIOB
//#define LED2_GPIO_CLK_ENABLE()             LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)  
//
//
// ИК передатчик (Low, Mid, High V2)
#define IRDA_TX_PIN                        LL_GPIO_PIN_6
#define IRDA_TX_GPIO_PORT                  GPIOC
#define IRDA_TX_GPIO_CLK_ENABLE()          LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)  
//
// ИК приемник  (Low, Mid, High V2)
#define IRDA_RX_PIN                        LL_GPIO_PIN_7
#define IRDA_RX_GPIO_PORT                  GPIOC
#define IRDA_RX_GPIO_CLK_ENABLE()          LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)  
//
//   
//    IRDA_CARRIER (Low, Mid, High V2)
#define IRDA_CAR_PIN                        LL_GPIO_PIN_9
#define IRDA_CAR_GPIO_PORT                  GPIOA
#define IRDA_CAR_GPIO_CLK_ENABLE()          LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)  
//  
//
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
//    Только для LPU_V2
#define CH_Ab_PIN                         LL_GPIO_PIN_15
#define CH_Ab_GPIO_PORT                   GPIOC
#define CH_Ab_GPIO_CLK_ENABLE()           LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)  
// 
//
//    Только для LPU_V2
// ВНИМАНИЕ  ------ БЛОК А включается линией, которая на схеме указана как  BATTERY_EN_B
#define BATTERY_EN_A_PIN                  LL_GPIO_PIN_6
#define BATTERY_EN_A_GPIO_PORT            GPIOB
#define BATTERY_EN_A_GPIO_CLK_ENABLE()    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)   
//
// ГДЕ ВТОРАЯ BATTERY ENABLE ????? PC8
//
//

/**
  * @brief Key push-button
  */
#define USER_BUTTON_PIN                         LL_GPIO_PIN_10
#define USER_BUTTON_GPIO_PORT                   GPIOB
#define USER_BUTTON_GPIO_CLK_ENABLE()           LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)   
#define USER_BUTTON_EXTI_LINE                   LL_EXTI_LINE_10
#define USER_BUTTON_EXTI_IRQn                   EXTI15_10_IRQn
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(USER_BUTTON_EXTI_LINE)   
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(USER_BUTTON_EXTI_LINE)   
#define USER_BUTTON_SYSCFG_SET_EXTI()           do {                                                                     \
                                                  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);                  \
                                                  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);  \
                                                } while(0)
#define USER_BUTTON_IRQHANDLER                  EXTI15_10_IRQHandler
                                                  
/*                                                  
#define IRDA_RX_PIN                         LL_GPIO_PIN_7
#define IRDA_RX_GPIO_PORT                   GPIOC
#define IRDA_RX_GPIO_CLK_ENABLE()           LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)   
*/                                              
                                                  

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* IRQ Handler treatment.*/
void UserButton_Callback(void); 

/* TIMx interrupt processing */
void Timer1CaptureCompare_Callback(void); // Carrier
void Timer11CaptureCompare_Callback(void); // RedTimer
void Timer3CaptureCompare_Callback(void) ; // GreenTimer
void Timer2CaptureCompare_Callback(void) ; // BlueTimer
void Timer10CaptureCompare_Callback(void); // UvTimer
void Timer4CaptureCompare_Callback(void) ; // WhtTimer
//

typedef struct _LighLevel_
{
  uint16_t u16_Period;
  uint16_t u16_Freq  ;
};
//
__STATIC_INLINE void IRDA_Rx_Init(void);
//
// для пульта
bool        IR_GetCode ( void );
void ProcessIRCode     ( void );
//  
// Для синхронизации
// Прием RGB свечения 
bool         IR_SyncRGB_GetCode( void );
void Process_IR_SyncRGB_Code   ( void );
//
// Прием параметра
bool         IR_SyncSetParam_GetCode( void );
void Process_IR_SyncSetParam_Code( void );
//
// Прием дискретной команды
bool         IR_SyncSetMode_GetCode ( void );
void Process_IR_SyncSetMode_Code( void );
//
//
void Process_IR_SyncSetActualWhite_Code(void);
void Process_IR_SyncSetActualUv_Code   (void);
//
void Process_IR_SyncSetManual_Code     (void);  
void Process_IR_SyncSetAuto_Code       (void);  
//
void Process_IR_SyncSetIndependent_Code(void);  
void Process_IR_SyncSetGrouped_Code    (void);  
//
void Process_IR_SetAutoPlay_Code( void );
void Process_IR_SetAutoPaus_Code( void );
void Process_IR_SetAutoStop_Code( void );
bool IR_SyncAutoMode_Check      ( void );
//
uint8_t u8_GetByteFrom_IR_RX_capture(void);
//
void TurnOnPattern    (void);
void TurnOffPattern    (void);

void ProcessPowerUp   (void);
void ProcessPowerDown (void);
//
void ProcessLightUp       (void);
void ProcessLightDown     (void);
//
void ProcessRedButton     (void);
void ProcessGreenButton   (void);
void ProcessBlueButton    (void);
void ProcessUvButton      (void);
void ProcessWhiteButton   (void);
//
void Process_Mode_A_Button(void);
void Process_Mode_B_Button(void);
void Process_Mode_C_Button(void);
//
void ProcessWiFiSerial(void);
void WiFi_ProcessRx( void );
void WiFi_ProcessTx( void );
//
void Process_WiFi_IncomingString( void );
//


// ЧТО ЭТО? Нужно как селектор буфера отправки
#define WIFI_TX_BUF_A_FAST 0
#define WIFI_TX_BUF_B_FAST 1
//
#define WIFI_TX_BUF_X 2
//
// для быстрых пакетов      
#define WIFI_TX_BUF_SIZE   ( 1024 )


void WiFi_USART_Init(void);


//
void ProcessIRDA_Rx(void);
void ProcessIRDA_Tx(void);
//
void   IrTxIdleProcessing(void);
void   IrTxPrepearingProcessing(void);   
void   IrTxTransmittionProcessing(void); 
void   IrTxWaitForAckProcessing(void);
void   IrTxDefaultProcessing(void);     
//
void Create_IR_Tx_RGB_Packet(    uint8_t   u8_Address,  // IR_TX_BROADBAND,
                                 uint8_t   u8_Type,     // IR_TX_BRIGHT_INFO,
                                uint16_t  u16_Red,      //
                                uint16_t  u16_Green,    //
                                uint16_t  u16_Blue    );//
//
void Create_IR_Tx_White_UV_Packet (  uint8_t   u8_Address,  // IR_TX_BROADBAND,
                                     uint8_t   u8_Type,     // IR_TX_BRIGHT_INFO,
                                    uint16_t  u16_Level );  // White or UV
//
void  Create_IR_Tx_Selector_Packet ( uint8_t  u8_Address,  
                                     uint8_t  u8_Type      ); //
//
void  Create_IR_Tx_ControlMode_Packet ( uint8_t  u8_Address,  
                                        uint8_t  u8_Type      ) ;
//
void  Create_IR_Tx_SuncMode_Packet    ( uint8_t  u8_Address,  
                                        uint8_t  u8_Type      ) ;
//
void Create_IR_Tx_ChangeSpeed_Packet ( uint8_t  u8_Address,  // IR_TX_BROADBAND,
                                       uint8_t  u8_Type,     // IR_TX_BRIGHT_INFO,
                                      uint16_t  u16_Level );// White or UV
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
//
//
// ТИПЫ ПАКЕТА
// Адресность
// Широковещательный
#define IR_TX_BROADBAND 0
// Адресный
// значение более 0 задает адрес
//
// НАЗНАЧЕНИЕ ПАКЕТА
// На данный момент IDLE пакет не предусмотрен
// Управление RGB
#define IR_TX_RGB_INFO             ( WIFI_COMMAND_SET_RGB               )
#define IR_TX_WHITE_INFO           ( WIFI_COMMAND_SET_WHITE_LIGHT_LEVEL )
#define IR_TX_UV_INFO              ( WIFI_COMMAND_SET_UV_LIGHT_LEVEL    )
//
#define IR_TX_SET_ACTUAL_WHITE     ( WIFI_COMMAND_SET_ACTUAL_WHITE      )
#define IR_TX_SET_ACTUAL_UV        ( WIFI_COMMAND_SET_ACTUAL_UV         )
//
#define IR_TX_SET_MODE_MANUAL      ( WIFI_COMMAND_SET_MODE_MANUAL       )
#define IR_TX_SET_MODE_AUTO        ( WIFI_COMMAND_SET_MODE_AUTO         )
//
#define IR_TX_SET_SYNC_INDEPENDENT ( WIFI_COMMAND_SET_SYNC_INDEPENDENT  )
#define IR_TX_SET_SYNC_GROUPED     ( WIFI_COMMAND_SET_SYNC_GROUPED      )
//
#define IR_TX_SET_CHANGE_SPEED     ( WIFI_COMMAND_SET_CHANGE_SPEED      )
//
//
#define IR_TX_SET_AUTO_PLAY     (  WIFI_COMMAND_SET_AUTO_PLAY     )
#define IR_TX_SET_AUTO_PAUS     (  WIFI_COMMAND_SET_AUTO_PAUS     )
#define IR_TX_SET_AUTO_STOP     (  WIFI_COMMAND_SET_AUTO_STOP     )
//
//
//
// Передача индекса
#define IR_TX_INDEX_INFO  11
//
//
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
