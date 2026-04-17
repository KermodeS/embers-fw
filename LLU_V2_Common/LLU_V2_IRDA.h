
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LLU_V2_IRDA_H
#define __LLU_V2_IRDA_H

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



// __STATIC_INLINE 
void     IRDA_CARRIER_ChannelInit(void);
//
/*
// __STATIC_INLINE 
void IRDA_Rx_Init(void);
// __STATIC_INLINE 
void IRDA_TX_Init(void);
// __STATIC_INLINE 
void IRDA_CAR_Init(void);
*/

/*
// ИК передатчик (Low, Mid, High V2)
#define IRDA_TX_PIN                        LL_GPIO_PIN_6
#define IRDA_TX_GPIO_PORT                  GPIOC
#define IRDA_TX_GPIO_CLK_ENABLE()          LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)  
*/
// ИК приемник  (Low, Mid, High V2)
/*
#define IRDA_RX_PIN                        LL_GPIO_PIN_7
#define IRDA_RX_GPIO_PORT                  GPIOC
#define IRDA_RX_GPIO_CLK_ENABLE()          LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)  
*/
//
/*   
//    IRDA_CARRIER (Low, Mid, High V2)
#define IRDA_CAR_PIN                        LL_GPIO_PIN_9
#define IRDA_CAR_GPIO_PORT                  GPIOA
#define IRDA_CAR_GPIO_CLK_ENABLE()          LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)  
*/  
//

//
// для пульта

bool        IR_GetCode_V2 (void);
void ProcessIRCode     (void);
//  
// Для синхронизации
// Прием RGB свечения 
// IR_SYNC  bool         IR_SyncRGB_GetCode( void );
// IR_SYNC  void Process_IR_SyncRGB_Code   ( void );
//
// Прием параметра
/* IR_SYNC_CONTROL 
bool         IR_SyncSetParam_GetCode( void );
void Process_IR_SyncSetParam_Code( void ); */
//
// Прием дискретной команды
/* IR_SYNC_CONTROL 
bool         IR_SyncSetMode_GetCode ( void );
void Process_IR_SyncSetMode_Code( void ); */
//
//
/* IR_SYNC_CONTROL
void Process_IR_SyncSetActualWhite_Code(void);
void Process_IR_SyncSetActualUv_Code   (void);
//
void Process_IR_SyncSetManual_Code     (void);  
void Process_IR_SyncSetAuto_Code       (void);  
//

void Process_IR_SyncSetIndependent_Code(void);  
void Process_IR_SyncSetGrouped_Code    (void);  */
//
/* void Process_IR_SetAutoPlay_Code( void );
void Process_IR_SetAutoPaus_Code( void );
 IR_SYNC_CONTROL void Process_IR_SetAutoStop_Code( void ); */
//
/* IR_SYNC_CONTROL void Process_IR_SingleStrobe_Code( void ); */
//
/* IR_SYNC_CONTROL bool IR_SyncAutoMode_Check      ( void ); */
//
uint8_t u8_GetByteFrom_IR_RX_capture(void);
//

//
void ProcessIRDA_Rx(void);
/* IR_SYNC_CONTROL void ProcessIRDA_Tx(void);*/
//
/* IR_SYNC_CONTROL 
void   IrTxIdleProcessing(void);
void   IrTxPrepearingProcessing(void);   
void   IrTxTransmittionProcessing(void); 
void   IrTxWaitForAckProcessing(void);
void   IrTxDefaultProcessing(void); 
*/    
//
/* IR_SYNC_CONTROL
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

void  Create_IR_Tx_SuncMode_Packet    ( uint8_t  u8_Address,  
                                        uint8_t  u8_Type      ) ; */
//
void Create_IR_Tx_ChangeSpeed_Packet ( uint8_t  u8_Address,  // IR_TX_BROADBAND,
                                       uint8_t  u8_Type,     // IR_TX_BRIGHT_INFO,
                                      uint16_t  u16_Level );// White or UV
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
/* IR_SYNC_CONTROL
#define IR_TX_SET_ACTUAL_WHITE     ( WIFI_COMMAND_SET_ACTUAL_WHITE      )
#define IR_TX_SET_ACTUAL_UV        ( WIFI_COMMAND_SET_ACTUAL_UV         )
//

#define IR_TX_SET_MODE_MANUAL      ( WIFI_COMMAND_SET_MODE_MANUAL       )
#define IR_TX_SET_MODE_AUTO        ( WIFI_COMMAND_SET_MODE_AUTO         )
//
#define IR_TX_SET_SYNC_INDEPENDENT ( WIFI_COMMAND_SET_SYNC_INDEPENDENT  )
 IR_SYNC_CONTROL #define IR_TX_SET_SYNC_GROUPED     ( WIFI_COMMAND_SET_SYNC_GROUPED      ) */
//
#define IR_TX_SET_CHANGE_SPEED     ( WIFI_COMMAND_SET_CHANGE_SPEED      )
//
//
#define IR_TX_SET_AUTO_PLAY     (  WIFI_COMMAND_SET_AUTO_PLAY     )
#define IR_TX_SET_AUTO_PAUS     (  WIFI_COMMAND_SET_AUTO_PAUS     )
/* #define IR_TX_SET_AUTO_STOP     (  WIFI_COMMAND_SET_AUTO_STOP     )
IR_SYNC_CONTROL #define IR_TX_SET_SINGLE_STROBE (  WIFI_COMMAND_SET_SINGLE_STROBE ) */
//
//
//
// Передача индекса - ГДЕ ВИДНО ЧТО ДЕЛАЕТ ПРЕДЫДУЩИЙ КОД? 
// ЭТО ВООБЩЕ НЕ ИСПОЛЬЗУЕТСЯ...
#define IR_TX_INDEX_INFO  11
//
//
#endif /* __LLU_V2_IRDA_H */