/**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-February-2017
  * @brief   This example describes how to use a timer peripheral to generate a 
  *          PWM output signal and update PWM duty cycle
  *          using the STM32F4xx TIM LL API.
  *          Peripheral initialization done using LL unitary services functions.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "LLU_V2_ESP32.h"
//#include "LLU_V2_IRDA.h"
#include "LLU_V2_LightControl.h"

#include "stdbool.h"   
#include "string.h" 
#include <stdio.h>
#include <stdlib.h>
/** @addtogroup STM32F4xx_LL_Examples
  * @{
  */

/** @addtogroup TIM_PWMOutput
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Number of output compare modes */
// #define TIM_DUTY_CYCLES_NB 11
//
//



// Для идентификации канала влияния при приеме сообщения SetLightLevel]
extern uint8_t u8_Actual_Wht_Uv_Selector ;

extern uint8_t u8_SetSpeedValue ; extern  char c_SetSpeedValue[4];// uint16_t  u16_SetSpeedValue;  
extern bool b_UpdateSetSpeed ;
//
uint8_t u8_ProcessWiFiCommand = WIFI_COMMAND_IDLE;
//
//
//
#define WIFI_RX_BUF_SIZE 250
#define WIFI_RX_BUF_QNT     2
#define WIFI_RX_BUF_A       0
#define WIFI_RX_BUF_B       1
//
uint8_t u8_WiFi_RxArray_A [WIFI_RX_BUF_SIZE];
uint8_t u8_WiFi_RxArray_A_Counter    = 0 ;
bool     b_WiFi_RxBuf_A_Active       =  true; // Готов к записи
bool     b_WiFi_RxBuf_A_ReadyForProc = false; // Готов к обработке
//
uint8_t u8_WiFi_RxArray_B [WIFI_RX_BUF_SIZE];
uint8_t u8_WiFi_RxArray_B_Counter    = 0 ;
bool    b_WiFi_RxBuf_B_Active       = false; // Готов к записи
bool    b_WiFi_RxBuf_B_ReadyForProc = false; // Готов к обработке
//
//
uint8_t u8_WiFi_RxActualBuf = WIFI_RX_BUF_A; // Начинаем прием с А буфера
//
// На тот случай, когда команда была перехвачена из потока консоли и 
// для блокирования ее повторного отправления в консоль с последующей 
// трансляцией модулем в другие модули сети
bool b_FromWiFi = false;
//
bool b_BRT_Update = false;
//
bool b_Restore_AM  = false;
//
//
void ProcessWiFiSerial(void)
{
  #define WIFI_UART_RX_PROCESSING 0
  #define WIFI_UART_TX_PROCESSING 1
  //
  static uint8_t u8_RoleSelector=WIFI_UART_RX_PROCESSING;
  //
  if(  u8_RoleSelector == WIFI_UART_RX_PROCESSING )
  {
    WiFi_ProcessRx(  ) ;   
  }
  else
  { // uint8_t u8_RoleSelector == WIFI_UART_TX_PROCESSING
    WiFi_ProcessTx(  ) ;   
  }
  //
  u8_RoleSelector++; 
  u8_RoleSelector = u8_RoleSelector & 0x1 ;
}


// ВНИМАНИЕ - ЭТО канал связи с UART Через последовательный порт,
// через который перехватывается консоль от модуля и в который загоняется
// массив u8_WIFI_TX_A_Fast, включающий в себя управление светодиодами на плате
// и активацию режима с параметрами. 
// Фазы процесса
#define WIFI_TX_WAIT_FOR_START 0
#define WIFI_TX_SEND_BYTE      1
#define WIFI_TX_WAIT_FOR_SEND  2


//
uint32_t u32_WiFi_Tx_Busy = false;
uint32_t u32_WIFI_TX_ActualChannel = WIFI_TX_BUF_X;
uint32_t u32_WIFI_TX_ActualPhase   = WIFI_TX_WAIT_FOR_START ;
//
// Контроль отправки быстрых пакетов
uint16_t u16_WIFI_TX_SendCounter_A_Fast ;
uint16_t u16_WIFI_TX_ActualSend_A_Fast ;
//
uint16_t u16_WIFI_TX_SendCounter_B_Fast ;
uint16_t u16_WIFI_TX_ActualSend_B_Fast ;
//



// Стандартный контроль готовности( по факту заполения данными) и контроль факта отправки
//         Пакет данный и телеметрии БЫСТРОГО режима      
// 0 - синхроначало. 0
// 1 - синхроначало. 0
// 
// 2 u8_LED1_State  // Состояние коммуникац модуль
// 3 u8_LED2_State  // Статус в иерархии
//
// 4 u8_LED3_State  // Синхро по радио
// 5 u8_LED4_State  // Синхро по ИК
// 6 u8_LED5_State  // ИК пульта
//
// 7 u8_LED6_State  // Батарейка
// 8 u8_LED7_State  // Встроенная клава
// 9 u8_LED8_State  // Качество синхро
//
// 10 u8_Mode_Sqnt_Request // Запросы на синхронизацию
// 11 резрв 1
// 12 резерв 2
//
// 13 - синхростоп 0xff
// 14 - синхростоп 0xFF
// размер пакета управления WIFI_TX_BUF_SIZE - 15 байт 


uint8_t  u8_WIFI_TX_A_Fast  [ WIFI_TX_BUF_SIZE ] ; // Это Part_A 
bool      b_WIFI_TX_Ready_A_Fast              = 0; // НЕ Заполнен данными
bool      b_WIFI_TX_Sended_A_Fast             = 1; // 1 это данные отправлены 
//
//          Полноценный пакет телеметрии канала       
uint8_t  u8_WIFI_TX_B_Fast  [ WIFI_TX_BUF_SIZE ] ; // Это Part_A  
bool      b_WIFI_TX_Ready_B_Fast              = 0; // НЕ Заполнен данными
bool      b_WIFI_TX_Sended_B_Fast             = 1; // 1 это данные отправлены 
// 
//
extern  uint8_t u8_Mode_Strobe_SM;
extern uint16_t u16_Mode_Strob_Counter_ms ;
extern uint16_t u16_Mode_Strob_SubCounter ;
extern uint32_t u32_SyncLostTimer_mks      ; 
//
// Это 8 полубайт для светодиодов.
uint8_t u8_LED1_State = LED_1_DEFAULT; // Состояние коммуникац модуль
uint8_t u8_LED2_State = LED_2_DEFAULT; // Статус в иерархии
//
uint8_t u8_LED3_State = LED_3_DEFAULT; // Синхро по радио
uint8_t u8_LED4_State = LED_4_DEFAULT; // Синхро по ИК
uint8_t u8_LED5_State = LED_5_DEFAULT; // ИК пульта
//
uint8_t u8_LED6_State = LED_6_DEFAULT; // Батарейка
uint8_t u8_LED7_State = LED_7_DEFAULT; // Встроенная клава
uint8_t u8_LED8_State = LED_8_DEFAULT; // Качество синхро
//
// Это 3 полубайта для задания режима и его параметра
uint8_t u8_MODE_State   = MODE_BYTE_IDLE ; // режим
uint8_t u8_MODE_Param_1 = MODE_PARAM_HIGH_NIBBLE_IDLE ; // старшая половина параметра
uint8_t u8_MODE_Param_2 = MODE_PARAM_LOW_NIBBLE_IDLE ;  // младшая половина параметра
//
//
//extern uint8_t u8_Mode_Sqnt_Request  ; //
extern uint8_t u8_Mode_Sqnt_Status  ;
extern uint32_t u32_Mode_Sqnt_SM    ;
//
extern uint8_t u8_Mode_Rnbw_Status  ;
extern uint32_t u32_Mode_Rnbw_SM    ;
//

bool b_Send_ESP32_LED1_Packet    = false;
bool b_Send_ESP32_LED2_Packet    = false;
bool b_Send_ESP32_LED3_Packet    = false;
bool b_Send_ESP32_LED4_Packet    = false;
bool b_Send_ESP32_LED5_Packet    = false;
bool b_Send_ESP32_LED6_Packet    = false;
bool b_Send_ESP32_LED7_Packet    = false;
bool b_Send_ESP32_LED8_Packet    = false;
//
// MODE_BYTE_WIFI_MODE  
bool b_Send_ESP32_WIFI_MODE_Packet = false;
//
// MODE_BYTE_IR_CMD
bool b_Send_ESP32_IR_CMD_Packet = false;
//
// MODE_BYTE_BRT_CMD
bool b_Send_ESP32_BRT_CMD_Packet = false;
//
// для изменения режима
bool b_Send_ESP32_OP_MODE_CMD_Packet = false;



//bool b_Send_ESP32_SeqMode_Packet  = false;
//bool b_Send_ESP32_Rsrv1_Packet    = false;
//bool b_Send_ESP32_Rsrv2_Packet    = false;

extern bool b_Grouped ;

//
extern uint8_t u8_StateMaschine ;
extern uint8_t u8_Mode_Strob_Status;
//
extern bool     b_DirectLightControl ;
extern uint32_t u32_ActualChannel  ;
// Это массив значений процента яркости от 0 до 100
extern int32_t i32_ActualPerc[5] ; // = { 0, 0, 0, 0, 0};
//
extern  uint8_t u8_ManualButton ;
//
extern  bool b_FS_ModeDefined     ;
extern  bool b_FS_ParamDefined[4] ;
extern  bool b_FS_Implemented     ;
//
void WiFi_ProcessRx( void )
{
  static uint8_t u8_received_char = 0;
  //
  if ( LL_USART_IsActiveFlag_ORE(WIFI_USART_INSTANCE ) == SET)
  {
    LL_USART_ClearFlag_ORE(WIFI_USART_INSTANCE );
  }
  //
  if( LL_USART_IsActiveFlag_RXNE(WIFI_USART_INSTANCE ) == SET)
  {
    /* RXNE flag will be cleared by reading of DR register (done in call) */
    /* Call function in charge of handling Character reception */
    //
    u8_received_char = LL_USART_ReceiveData8(WIFI_USART_INSTANCE);
    //
    /* Check if received value is corresponding to specific one : S or s */
    if( u8_received_char == 0x0A ) // 0
    {
      u8_WiFi_RxArray_A [u8_WiFi_RxArray_A_Counter++] =  u8_received_char;
      //
      if (u8_WiFi_RxActualBuf == WIFI_RX_BUF_A) b_WiFi_RxBuf_A_ReadyForProc = true;
      if (u8_WiFi_RxActualBuf == WIFI_RX_BUF_B) b_WiFi_RxBuf_B_ReadyForProc = true;
      //
      Process_WiFi_IncomingString();
    }
    else
    {
      if ( u8_WiFi_RxActualBuf == WIFI_RX_BUF_A)
      { 
        u8_WiFi_RxArray_A [u8_WiFi_RxArray_A_Counter++] =  u8_received_char;
        if (u8_WiFi_RxArray_A_Counter>=WIFI_RX_BUF_SIZE)
          u8_WiFi_RxArray_A_Counter=0;
      }
      else
      {
         u8_WiFi_RxArray_B [u8_WiFi_RxArray_B_Counter++] =  u8_received_char;
                 if (u8_WiFi_RxArray_A_Counter>=WIFI_RX_BUF_SIZE)
          u8_WiFi_RxArray_A_Counter=0;
      }
    }  
  }   
}
//
//
uint8_t CharToHex( uint8_t u8_Char);
uint8_t CharToHex( uint8_t u8_Char)
{
  // 0 30
  // 1 31
  // ..
  // 9 39
  // A 41
  // B 42
  // ..
  // F 46  
  //
  if ( (u8_Char>=0x30 ) && (u8_Char<=0x39) )
  {
    // цифры
    return (u8_Char-0x30);
  }
  else
    if ( (u8_Char>=0x41) && (u8_Char<=0x46) )
    {
      // буквы
      return (u8_Char-0x41+10);
    }
  else
    return 0;

}
//
uint8_t u8_WiFi_MAC_1;
uint8_t u8_WiFi_MAC_2;
uint8_t u8_WiFi_MAC_3;
uint8_t u8_WiFi_MAC_4;
uint8_t u8_WiFi_MAC_5;
uint8_t u8_WiFi_MAC_6;
//
uint32_t u32_LLU_V2_S_N=0;
//
void RetriveMAC( uint8_t *p_MAC_String, uint8_t u8_Offset );
void RetriveMAC( uint8_t *p_MAC_String, uint8_t u8_Offset )
{
  // I (328) MPU_V2: MAC 30-C6-F7-00-80-50

  //
  // p_MAC_String [u8_StrCounter+10]=='M') &&
  // p_MAC_String [u8_StrCounter+11]=='A') &&
  // p_MAC_String [u8_StrCounter+12]=='C') &&
  // p_MAC_String [u8_StrCounter+13]==' ') &&
  u8_WiFi_MAC_1  =  CharToHex( u8_WiFi_RxArray_A [u8_Offset+14] );  u8_WiFi_MAC_1 *= 0x10;
  u8_WiFi_MAC_1 +=  CharToHex( u8_WiFi_RxArray_A [u8_Offset+15] );
  // u8_WiFi_RxArray_A [u8_StrCounter+16]=='-') &&  
  u8_WiFi_MAC_2  =  CharToHex( u8_WiFi_RxArray_A [u8_Offset+17] );  u8_WiFi_MAC_2 *= 0x10;
  u8_WiFi_MAC_2 +=  CharToHex( u8_WiFi_RxArray_A [u8_Offset+18] ); 
  // u8_WiFi_RxArray_A [u8_StrCounter+19]=='-') &&  
  u8_WiFi_MAC_3  =  CharToHex( u8_WiFi_RxArray_A [u8_Offset+20] );  u8_WiFi_MAC_3 *= 0x10;
  u8_WiFi_MAC_3 +=  CharToHex( u8_WiFi_RxArray_A [u8_Offset+21] );
  // u8_WiFi_RxArray_A [u8_StrCounter+22]=='-') &&  
  u8_WiFi_MAC_4  =  CharToHex( u8_WiFi_RxArray_A [u8_Offset+23] );  u8_WiFi_MAC_4 *= 0x10;
  u8_WiFi_MAC_4 +=  CharToHex( u8_WiFi_RxArray_A [u8_Offset+24] );
  // u8_WiFi_RxArray_A [u8_StrCounter+25]=='-') &&  
  u8_WiFi_MAC_5  =  CharToHex( u8_WiFi_RxArray_A [u8_Offset+26] );  u8_WiFi_MAC_5 *= 0x10;
  u8_WiFi_MAC_5 +=  CharToHex( u8_WiFi_RxArray_A [u8_Offset+27] ); 
  // u8_WiFi_RxArray_A [u8_StrCounter+28]=='-')   
  u8_WiFi_MAC_6  =  CharToHex( u8_WiFi_RxArray_A [u8_Offset+29] );  u8_WiFi_MAC_6 *= 0x10; 
  u8_WiFi_MAC_6 +=  CharToHex( u8_WiFi_RxArray_A [u8_Offset+30] );   
  //
}
//
void WiFi_MAC_Error(void);
void WiFi_MAC_Error(void)
{
  ;
}
//
#define LPU_V2_SEAN 1
#define MPU_V2_SEAN 2
#define HPU_V2_SEAN 3
//
#define LPU_V2_OLEG 4
#define MPU_V2_OLEG 5
#define HPU_V2_OLEG 6
//
uint8_t  ProcessMAC (void);
uint8_t  ProcessMAC (void)
{
  //
  #define ACTUAL_WIFI_MAC_QNT 6
  #define WIFI_MAC_SIZE       6
  #define LLU_V2_SERIAL_SIZE  4 
  //
  static const uint8_t WiFi_MACs [ACTUAL_WIFI_MAC_QNT] [WIFI_MAC_SIZE + LLU_V2_SERIAL_SIZE ]  = {
  {0x30, 0xC6, 0xF7, 0x00, 0x80, 0x50,  /* s/n */  0x00, 0x00, 0x00, 0x01},// LPU_V2 Sean  МАSTER
  {0x8C, 0xAA, 0xB5, 0x8E, 0xC7, 0x78,  /* s/n */  0x00, 0x00, 0x00, 0x02},// MPU_V2 Sean VICE_MASTER
  {0x7C, 0x9E, 0xBD, 0x04, 0x6D, 0xC8,  /* s/n */  0x00, 0x00, 0x00, 0x03},// HPU_V2 Sean NODE
  //
  {0x80, 0x7D, 0x3A, 0x89, 0x73, 0xC0,  /* s/n */  0x00, 0x00, 0x00, 0x04},// LPU_V2 Oleg NODE
  {0x24, 0x6F, 0x28, 0x3F, 0x61, 0x70,  /* s/n */  0x00, 0x00, 0x00, 0x05},// MPU_V2 Oleg NODE
  {0x24, 0x6F, 0x28, 0x3F, 0x62, 0x04,  /* s/n */  0x00, 0x00, 0x00, 0x06}// HPU_V2 Oleg NODE
  };
  //
  static uint32_t u32_I=0;
  for ( u32_I = 0; u32_I<ACTUAL_WIFI_MAC_QNT; u32_I++)
  {
    if ( (WiFi_MACs[u32_I][0] == u8_WiFi_MAC_1 ) &&  (WiFi_MACs[u32_I][1] == u8_WiFi_MAC_2 ) &&  (WiFi_MACs[u32_I][2] == u8_WiFi_MAC_3 ) &&  
         (WiFi_MACs[u32_I][3] == u8_WiFi_MAC_4 ) &&  (WiFi_MACs[u32_I][4] == u8_WiFi_MAC_5 ) &&  (WiFi_MACs[u32_I][5] == u8_WiFi_MAC_6 )    )
    return  (u32_I+1) ; // LPU_V2 Sean  МАSTER     
  }
  // Если нет совпадений, то 
  return 0;
/*  
LPU_V2 Sean ( WiFi MAC 30-C6-F7-00-80-50 ) s/n 0000-0000-0000-0001  МАSTER
MPU_V2 Sean ( WiFi MAC 8C-AA-B5-8E-C7-78 ) s/n 0000-0000-0000-0002  VICE_MASTER
HPU_V2 Sean ( WiFi MAC 7C-9E-BD-04-6D-C8 ) s/n 0000-0000-0000-0003  NODE

LPU_V2 Oleg ( WiFi MAC 80-7D-3A-89-73-C0 ) s/n 0000-0000-0000-0004  NODE
MPU_V2 Oleg ( WiFi MAC 24-6F-28-3F-61-70 ) s/n 0000-0000-0000-0005  NODE
HPU_V2 Oleg ( WiFi MAC 24-6F-28-3F-62-04 ) s/n 0000-0000-0000-0006  NODE
*/
}

uint8_t RetrivePercData ( uint8_t * u8_WiFi_RxArray_A, uint8_t u8_StrCounter, uint8_t u8_Offset);
uint8_t RetrivePercData ( uint8_t * u8_WiFi_RxArray_A, uint8_t u8_StrCounter, uint8_t u8_Offset)
{
  if ( (u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '1') &&
       (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '0') &&
       (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {
      return 100 ; 
  }
  else  
  if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16+0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16+1 + u8_Offset] == '0') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16+2 + u8_Offset] == '0')    )
  {  
    return  0 ; 
  }
  else  
  if (// (u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '1') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {             
    return  10 ; 
  }
  else  
  if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '2') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {    
    return  20 ; 
  }
  else  
  if (// (u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '3') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {  
    return  30 ; 
  }
  else  
  if (// (u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '4') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {  
    return  40 ; 
  }
  else  
  if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '5') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {   
    return  50 ; 
  }
  else  
  if (// (u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '6') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {  
    return  60 ; 
  }
  else  
  if (// (u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '7') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {   
    return  70 ; 
  }
  else  
  if (// (u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '8') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  { 
    return  80 ; 
  }
  else  
  if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16 + 0 + u8_Offset] == '0') &&
         (u8_WiFi_RxArray_A [u8_StrCounter+16 + 1 + u8_Offset] == '9') &&
	 (u8_WiFi_RxArray_A [u8_StrCounter+16 + 2 + u8_Offset] == '0')    )
  {    
    return  90 ; 
  }   
}
//
//
extern uint8_t u8_FS_Mode   ;
extern uint8_t u8_FS_R_Perc ;
extern uint8_t u8_FS_G_Perc ;
extern uint8_t u8_FS_B_Perc ;
extern uint8_t u8_FS_U_Perc ;
extern uint8_t u8_FS_W_Perc ;
//
// Для стробоскопа
extern uint8_t u8_FS_Strobe_ActCh ;
//
extern  uint32_t u32_ActualNetRole ; 
//
void Process_WiFi_IncomingString()
{
  //
  // I KNOW THE EXISTENCE OF FUNCTIONS  sscanf, strcmp and so on
  // 
  // You need to find the word HPU_V2_Node:
  // Controling of  RGB 
  // I (289913) HPU_V2_Node: RED: 122, GREEN: 5, BLUE: 5
  
  static uint8_t u8_StrCounter=0;
  
  u8_StrCounter=0;
  while ( u8_StrCounter < (u8_WiFi_RxArray_A_Counter-1) )
  {
    /*  REDUCE 
    if ( (u8_WiFi_RxArray_A [u8_StrCounter]==')') && (u8_WiFi_RxArray_A [u8_StrCounter+1]==' ')  )
    {
      if (
          (u8_WiFi_RxArray_A [u8_StrCounter+ 2]=='H') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+ 3]=='P') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+ 4]=='U') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+ 5]=='_') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+ 6]=='V') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+ 7]=='2') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+ 8]=='_') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+ 9]=='N') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+10]=='o') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+11]=='d') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+12]=='e') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+13]==':') &&
          (u8_WiFi_RxArray_A [u8_StrCounter+14]==' ') 
                     
          )
      {
        if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='R') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='E') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='D') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]==':') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]==' ')       )
        {
          //
          if (u8_WiFi_RxArray_A [u8_StrCounter+21] == ',')
          { 
            // Однозначное значение
            c_desired_R[0+0] =' ';  
            c_desired_R[0+1] =' '; 
            c_desired_R[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+20];
            c_desired_R[0+3] = 0        ;    
            //
            u8_StrCounter += 21;
          }
          else
            if(u8_WiFi_RxArray_A [u8_StrCounter+22] == ',')
            {
              // Однозначное значение
              c_desired_R[0+0] = ' '; 
              c_desired_R[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+20]; 
              c_desired_R[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+21];
              c_desired_R[0+3] = 0    ;
              //
              u8_StrCounter += 22;
            }
            else
              if(u8_WiFi_RxArray_A [u8_StrCounter+23] == ',')
              {
                // Однозначное значение
                c_desired_R[0+0] = u8_WiFi_RxArray_A[u8_StrCounter+20];  
                c_desired_R[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+21];
                c_desired_R[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+22];
                c_desired_R[0+3] = 0  ; 
                //
                u8_StrCounter += 23;
              }
        //
        //
        if (u8_WiFi_RxArray_A [u8_StrCounter+10] == ',')
        {
          // Однозначное значение
          c_desired_G[0+0] =' ';  
          c_desired_G[0+1] =' '; 
          c_desired_G[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+9];
          c_desired_G[0+3] = 0        ;    
          //
          u8_StrCounter += 10;
        }
        else
          if(u8_WiFi_RxArray_A [u8_StrCounter+11] == ',')
          {
            // Однозначное значение
            c_desired_G[0+0] = ' '; 
            c_desired_G[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+9]; 
            c_desired_G[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+10];
            c_desired_G[0+3] = 0    ;   
            //
            u8_StrCounter += 11;
          }
          else
            if(u8_WiFi_RxArray_A [u8_StrCounter+12] == ',')
            {
              // single digit value
              c_desired_G[0+0] = u8_WiFi_RxArray_A[u8_StrCounter+ 9];  
              c_desired_G[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+10];
              c_desired_G[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+11];
              c_desired_G[0+3] = 0  ;  
              //
              u8_StrCounter += 12;
            }
        //
        //
        if (u8_WiFi_RxArray_A [u8_StrCounter+9] == 0x1B ) //'.' 
        {
          // single digit value
          c_desired_B[0+0] =' ';  
          c_desired_B[0+1] =' '; 
          c_desired_B[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+8];
          c_desired_B[0+3] = 0        ;    
          //
          u8_StrCounter += 9;
        }
        else
          if(u8_WiFi_RxArray_A [u8_StrCounter+10] == 0x1B ) //'.'
          {
            // single digit value
            c_desired_B[0+0] = ' '; 
            c_desired_B[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+8]; 
            c_desired_B[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+9];
            c_desired_B[0+3] = 0    ;   
            //
            u8_StrCounter += 10;
          }
          else
            if(u8_WiFi_RxArray_A [u8_StrCounter+11] == 0x1B ) //'.'
            {
              // single digit value
              c_desired_B[0+0] = u8_WiFi_RxArray_A[u8_StrCounter+8];  
              c_desired_B[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+9];
              c_desired_B[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+10];
              c_desired_B[0+3] = 0  ;  
              //
              u8_StrCounter += 11;
            }
        //
           
        // These are values between 0 and 255
        u8_desired_R = (uint8_t)atoi( c_desired_R);
        u8_desired_G = (uint8_t)atoi( c_desired_G);
        u8_desired_B = (uint8_t)atoi( c_desired_B);

        if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
        u8_ProcessWiFiCommand = WIFI_COMMAND_SET_RGB;
        break;
        }
        //
        //     //
        //I (527253) HPU_V2_Node: f_LightLevel: 0.080000
        if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='f') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='_') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='L') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='i') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='g') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='h') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='t') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='L') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+24]=='v') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+25]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+26]=='l') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+27]==':') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+28]==' ')       )
        {
          if (u8_Actual_Wht_Uv_Selector == SELECTOR_ACTUAL_WHITE)
          {
            //
            c_desired_W[0+0] = u8_WiFi_RxArray_A[u8_StrCounter+29+0]; 
            c_desired_W[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+29+1]; 
            c_desired_W[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+29+2];
            c_desired_W[0+3] = u8_WiFi_RxArray_A[u8_StrCounter+29+3];
            c_desired_W[0+4] = 0                     ;
            //
            // Это значения в диапазоне от 0 до 255
            f_desired_W = atof( c_desired_W);
            f_desired_W= 255.0*f_desired_W; 
            if(f_desired_W>255) f_desired_W = 255;
            if(f_desired_W<0  ) f_desired_W =   0;
            //
            u8_desired_W =(uint8_t) f_desired_W;
            //
            if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_WHITE_LIGHT_LEVEL;
            //        
          }
          //
          if (u8_Actual_Wht_Uv_Selector == SELECTOR_ACTUAL_UV)
          {
            //
            c_desired_U[0+0] = u8_WiFi_RxArray_A[u8_StrCounter+29+0]; 
            c_desired_U[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+29+1]; 
            c_desired_U[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+29+2];
            c_desired_U[0+3] = u8_WiFi_RxArray_A[u8_StrCounter+29+3];
            c_desired_U[0+4] = 0                     ;
            //
            // Это значения в диапазоне от 0 до 255
            f_desired_U = atof( c_desired_U);
            f_desired_U= 255.0*f_desired_U; 
            if(f_desired_U>255) f_desired_U = 255;
            if(f_desired_U<0  ) f_desired_U =   0;
            //
            u8_desired_U = (uint8_t)f_desired_U;
            //
            if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
              u8_ProcessWiFiCommand = WIFI_COMMAND_SET_UV_LIGHT_LEVEL;
            //        
          }
          //          
          break;
        }
      }
     
      //
      // I (480063) HPU_V2_Node: Set_Wht
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='t') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='_') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='W') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='h') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='t')    )
        {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_ACTUAL_WHITE;       
          //          
          break;
        }
     
      // I (482173) HPU_V2_Node: Set_UV
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='t') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='_') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='U') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='V')    )
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_ACTUAL_UV;
          //          
          break;
      }
      // I (390633) HPU_V2_Node: AUTO_PLAY
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='A') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='U') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='T') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='O') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='_') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='P') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='L') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='A') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='Y')    )
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_AUTO_PLAY;
          //          
          break;
      }
      // I (401353) HPU_V2_Node: AUTO_PAUS
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='A') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='U') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='T') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='O') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='_') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='P') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='A') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='U') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='S')    )
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
          u8_ProcessWiFiCommand = WIFI_COMMAND_SET_AUTO_PAUS;
          //          
          break;
      }
      // I (405543) HPU_V2_Node: AUTO_STOP
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='A') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='U') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='T') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='O') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='_') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='S') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='T') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='O') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='P')    )
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_AUTO_STOP;
          //          
          break;
      }
      //
      // I (1106843) HPU_V2_Node: SetSync_Independent
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='t') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='y') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='n') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='c') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='_') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='I') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+24]=='n') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+25]=='d') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+26]=='e') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+27]=='p') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+28]=='e') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+29]=='n') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+30]=='d') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+31]=='e') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+32]=='n') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+33]=='t')      )
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
          u8_ProcessWiFiCommand = WIFI_COMMAND_SET_SYNC_INDEPENDENT;
          //          
          break;
      }
      //
      //
      // I (1116253) HPU_V2_Node: SetSync_____Grouped
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='t') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='y') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='n') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='c') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='_') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+24]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+25]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+26]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+27]=='G') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+28]=='r') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+29]=='o') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+30]=='u') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+31]=='p') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+32]=='e') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+33]=='d')      )
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_SYNC_GROUPED;
          //          
          break;
      }
      //
      // I (731833) HPU_V2_Node: OperMode_Manual
      if (    (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='p') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='r') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='M') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='o') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='d') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='e') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+24]=='M') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+25]=='a') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+26]=='n') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+27]=='u') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+28]=='a') &&   
              (u8_WiFi_RxArray_A [u8_StrCounter+29]=='l')     )                                   
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_MODE_MANUAL;       
          //          
          break;
      }
      //
      // I (735253) HPU_V2_Node: OperMode___Auto
     if (     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='p') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='r') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='M') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='o') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='d') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='e') && 
              (u8_WiFi_RxArray_A [u8_StrCounter+23]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+24]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+25]=='_') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+26]=='A') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+27]=='u') &&  
              (u8_WiFi_RxArray_A [u8_StrCounter+28]=='t') &&   
              (u8_WiFi_RxArray_A [u8_StrCounter+29]=='o')     )
      {
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_MODE_AUTO;      
          //          
          break;
      }
      //
      //
         
      // I (737903) HPU_V2_Node: SetSpeed 047
      //
      if (
              (u8_WiFi_RxArray_A [u8_StrCounter+15]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+16]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+17]=='t') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+18]=='S') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+19]=='p') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+20]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+21]=='e') &&
              (u8_WiFi_RxArray_A [u8_StrCounter+22]=='d')     )
        {
          
          
          c_SetSpeedValue[0+0] = u8_WiFi_RxArray_A[u8_StrCounter+24+0]; 
          c_SetSpeedValue[0+1] = u8_WiFi_RxArray_A[u8_StrCounter+24+1]; 
          c_SetSpeedValue[0+2] = u8_WiFi_RxArray_A[u8_StrCounter+24+2];
          c_SetSpeedValue[0+3] = 0                     ;
          //
          // Это значения в диапазоне от 0 до 99
          u8_SetSpeedValue = atoi( c_SetSpeedValue);
          //
          //
          if (u8_SetSpeedValue < UPDATE_DELAY_MIN_MS)u8_SetSpeedValue = UPDATE_DELAY_MIN_MS;
          if (u8_SetSpeedValue > UPDATE_DELAY_MAX_MS)u8_SetSpeedValue = UPDATE_DELAY_MAX_MS;
          //
          //u8_StateMaschine = SM_MODE_MANUAL;
          b_UpdateSetSpeed = true; // AutoModeActualSpeed  
          //
          if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
            u8_ProcessWiFiCommand = WIFI_COMMAND_SET_CHANGE_SPEED;
          // 
          //          
          break;
        }
      
      
    }// End of While
    REDUCE */
  //  else
    { 
      if ( (u8_WiFi_RxArray_A [u8_StrCounter]==')') && (u8_WiFi_RxArray_A [u8_StrCounter+1]==' ')  )
      {
        if (
            (u8_WiFi_RxArray_A [u8_StrCounter+ 2]=='x') &&
            (u8_WiFi_RxArray_A [u8_StrCounter+ 3]=='P') &&
            (u8_WiFi_RxArray_A [u8_StrCounter+ 4]=='U') &&
            (u8_WiFi_RxArray_A [u8_StrCounter+ 5]=='_') &&
            (u8_WiFi_RxArray_A [u8_StrCounter+ 6]=='V') &&
            (u8_WiFi_RxArray_A [u8_StrCounter+ 7]=='2') &&
            (u8_WiFi_RxArray_A [u8_StrCounter+ 8]==':') &&
            (u8_WiFi_RxArray_A [u8_StrCounter+ 9]==' ')           
            )
        {
          //
          // I (334) xPU_V2: MAC 24-6F-28-3F-61-70
          
          //
          // I (595300) MPU_V2: RadioSync
          if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='a') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='d') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='i') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='o') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='y') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='n') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='c')  
                    )
            {
              u32_SyncLostTimer_mks=0;
              //
              if (  u8_StateMaschine ==   SM_MODE_SEQUENTIAL    )
              {
                // Прием радиосинхронизации в режиме последовательного отображения.
                
              }
              //
              // Эмулируем прием команды от пульта
              //  u8_ManualButton = MANUAL_MODE_BUTTON_STROBOSCOPE; 
              //   
              if (  u8_StateMaschine ==   SM_MODE_STROBOSCOPE    ) //  100
              {
                if ( (u8_Mode_Strob_Status == MODE_STATUS_RUN) ) // && (u16_Mode_Strob_Counter_ms>=STROBE_REINIT_TIME_MS )
                {             
                  //
                  u16_Mode_Strob_SubCounter  = 0;
                  u16_Mode_Strob_Counter_ms  = 0;
                  //
                  u8_Mode_Strobe_SM          = SM_MODE_STROB_ON;
                }
              }
              //
              //
              if (   (u8_LED2_State == LED_2_MASTER)  )   
              {
                b_Grouped = true;
                //
                u8_LED3_State            = LED_3_SentSync;
                b_Send_ESP32_LED3_Packet = true;
              }
              else
                if ( (u8_LED2_State == LED_2_SLAVE ) || ( u8_LED2_State == LED_2_ECHO_SLAVE)  )
                {
                  b_Grouped = true; 
                  //
                  u8_LED3_State            = LED_3_RecieveSync;
                  b_Send_ESP32_LED3_Packet = true;
                }  
                        
              //          
              break;
            } // if  // I (595300) MPU_V2: RadioSync
            else
            // ВАЖНО. 
            // Если устройство работает в автономном режиме, то команда от ИК пульта в модуль НЕ ОТПРАВЛЯЕТСЯ.
            // Когда устройство в группе - команды типа Up Down не передаются. Передаются канал и процент яркости  
            // РУДИМЕНТ    I (704451) MPU_V2: IR_CMD_UP__
            // РУДИМЕНТ    I (707326) MPU_V2: IR_CMD_DOWN
            //  "BRT_R_000", "BRT_R_010", .. "BRT_R_100"	
            //  "BRT_G_000", "BRT_G_010", .. "BRT_G_100"	
            //  "BRT_B_000", "BRT_B_010", .. "BRT_B_100"	
            //  "BRT_U_000", "BRT_U_010", .. "BRT_U_100"	
            //  "BRT_W_000", "BRT_W_010", .. "BRT_W_100"	
            //
            // Прием команды управления красным
            if ( 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='B') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]  =='_')    )
            {
              
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+16] == '1') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 100 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 0 ; 
                   b_BRT_Update = true;   
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '1') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 10 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '2') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{    
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 20 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '3') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 30 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '4') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 40 ; 
                   b_BRT_Update = true;  
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '5') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 50 ; 
                   b_BRT_Update = true; 
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '6') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 60 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '7') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 70 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '8') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{ 
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 80 ; 
                   b_BRT_Update = true; 
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '9') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_RED;
                   i32_ActualPerc[u32_ActualChannel] = 90 ; 
                   b_BRT_Update = true; 
		}   
            }   
            else
            // Прием команды управления GREEN
            if ( 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='B') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]  =='G') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]  =='_')    )
            {
              
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+16] == '1') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 100 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 0 ; 
                   b_BRT_Update = true;   
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '1') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 10 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '2') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{    
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 20 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '3') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 30 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '4') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 40 ; 
                   b_BRT_Update = true;  
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '5') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 50 ; 
                   b_BRT_Update = true; 
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '6') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 60 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '7') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 70 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '8') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{ 
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 80 ; 
                   b_BRT_Update = true; 
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '9') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_GRN;
                   i32_ActualPerc[u32_ActualChannel] = 90 ; 
                   b_BRT_Update = true; 
		}   
            }   
            else
            // Прием команды управления BLUE
            if ( 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='B') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]  =='B') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]  =='_')    )
            {
              
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+16] == '1') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 100 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 0 ; 
                   b_BRT_Update = true;   
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '1') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 10 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '2') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{    
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 20 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '3') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 30 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '4') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 40 ; 
                   b_BRT_Update = true;  
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '5') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 50 ; 
                   b_BRT_Update = true; 
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '6') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 60 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '7') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 70 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '8') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{ 
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 80 ; 
                   b_BRT_Update = true; 
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '9') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_BLU;
                   i32_ActualPerc[u32_ActualChannel] = 90 ; 
                   b_BRT_Update = true; 
		}   
            }   
            else
            // Прием команды управления UV
            if ( 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='B') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]  =='U') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]  =='_')    )
            {
              
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+16] == '1') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 100 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 0 ; 
                   b_BRT_Update = true;   
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '1') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 10 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '2') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{    
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 20 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '3') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 30 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '4') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 40 ; 
                   b_BRT_Update = true;  
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '5') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 50 ; 
                   b_BRT_Update = true; 
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '6') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 60 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '7') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 70 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '8') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{ 
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 80 ; 
                   b_BRT_Update = true; 
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '9') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_UV;
                   i32_ActualPerc[u32_ActualChannel] = 90 ; 
                   b_BRT_Update = true; 
		}   
            }   
             else
            // Прием команды управления WHT
            if ( 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='B') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]  =='W') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]  =='_')    )
            {
              
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+16] == '1') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 100 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '0') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 0 ; 
                   b_BRT_Update = true;   
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '1') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 10 ; 
                   b_BRT_Update = true;
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '2') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{    
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 20 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '3') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 30 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '4') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 40 ; 
                   b_BRT_Update = true;  
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '5') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 50 ; 
                   b_BRT_Update = true; 
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '6') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{  
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 60 ; 
                   b_BRT_Update = true;
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '7') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 70 ; 
                   b_BRT_Update = true;  
		}
		else  
                if (// (u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '8') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{ 
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 80 ; 
                   b_BRT_Update = true; 
		}
		else  
                if ( //(u8_WiFi_RxArray_A [u8_StrCounter+16] == '0') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+17] == '9') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == '0')    )
		{   
                   u32_ActualChannel = _ACTUAL_WHT;
                   i32_ActualPerc[u32_ActualChannel] = 90 ; 
                   b_BRT_Update = true; 
		}   
            }   
            else //        
            // I (709372) MPU_V2: IR_CMD_GRN_
            // I (712444) MPU_V2: IR_CMD_RED_ 
            if ( 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='I') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='C') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]  =='M') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]  =='D') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]  =='_')    )
                  // Прием Команды запуска в режиме последовательного отображения.
            {
                ////  1 ряд из 4 кнопок.
                // 1 -4 
                /* РУДИМЕНТ
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'U') &&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'P') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+19] == '_') &&
	             (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_'))
		{
		   // ESP_LOGI(TAG, "IR_CMD_UP__"); // "IR_CMD_UP__", //#define     1
                  b_FromWiFi =  true;
                  ProcessLightUp() ;    
		}
		else  
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'D')&&
	             (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'O')&&
	             (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'W')&&
	             (u8_WiFi_RxArray_A [u8_StrCounter+20] == 'N')    )
		{
                    // ESP_LOGI(TAG, "IR_CMD_DOWN"); // "IR_CMD_DOWN", //#define     2
                    b_FromWiFi =  true;
                    ProcessLightDown() ;
		}
		else  */
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'O')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'F')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'F')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_OFF_"); // "IR_CMD_OFF_", //#define     3
                   b_FromWiFi =  true;
                   ProcessPowerDown  ();
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'O')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'N')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == '_')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_ON__"); // "IR_CMD_ON__",	//#define    4
                  b_FromWiFi =  true;
                  ProcessPowerUp    ();
		}
		////  2 ряд из 4 кнопок.
		// 5 -8
	/*	else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'E')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'D')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_RED_"); // "IR_CMD_RED_", //#define     5
                  b_FromWiFi =  true;
                  ProcessRedButton      ();
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'N')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_GRN_"); // "IR_CMD_GRN_", //#define     6
                   b_FromWiFi =  true;
                   ProcessGreenButton    ();
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'B')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'L')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'U')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_BLU_"); // "IR_CMD_BLU_", //#define     7
                  b_FromWiFi =  true;
                  ProcessBlueButton     ();
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'W')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'H')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'T')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
		  // ESP_LOGI(TAG, "IR_CMD_WHT_"); // "IR_CMD_WHT_", //#define     8
                  b_FromWiFi =  true;
                  ProcessWhiteButton    ();
		}
		////  3 ряд из 4 кнопок.*/
		// 9 - 12
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'O')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '1')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_ORG1"); // "IR_CMD_ORG1", //#define     9
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'N')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '1')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_GRN1"); // "IR_CMD_GRN1", //#define    10
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'B')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'L')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'U')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '1')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_BLU1"); // "IR_CMD_BLU1", //#define    11
		}
	/*	else
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'F')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == '1')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == '_')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_F1__"); // "IR_CMD_F1__", //#define    12
                  b_FromWiFi =  true;
                  ProcessUvButton       ();
		}*/
		//
		//  4 ряд из 4 кнопок.
		// 13 - 16
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'O')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '2')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_ORG2"); //"IR_CMD_ORG2", //#define    13
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'N')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '2')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_GRN2"); // "IR_CMD_GRN2", //#define    14
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'P')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'P')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'L')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '1')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_PPL1"); // "IR_CMD_PPL1", //#define    15
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'S')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == '1')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == '_')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
		  // ESP_LOGI(TAG, "IR_CMD_S1__"); // "IR_CMD_S1__", //#define    16
                  // S - Верхняя
                  b_FromWiFi =  true;
                  b_Restore_AM = true;
                  u8_ManualButton = MANUAL_MODE_BUTTON_STROBOSCOPE;
                  Process_Mode_Stroboscope_Button ();
		}
		//
		// Диапазон 16-31 передается через второй параметр.
		//  5 ряд из 4 кнопок.
		// 17 - 20
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'O')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '3')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_ORG3"); //"IR_CMD_ORG3", //#define    17
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'N')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '3')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_GRN3"); //"IR_CMD_GRN3", //#define    18
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'P')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'P')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'L')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '2')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_PPL2"); //"IR_CMD_PPL2", //#define    19
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'S')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == '2')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == '_')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_S2__"); //"IR_CMD_S2__", //#define    20
                  // S - нижняя
                  b_FromWiFi =  true;
                  b_Restore_AM = true;
                  u8_ManualButton = MANUAL_MODE_BUTTON_SEQUENTIAL;
                  Process_Mode_Sequential_Button  ();
		}
		////  6 ряд из 4 кнопок.
		// 21 - 24
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'Y')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'L')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'W')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_YLW_"); //"IR_CMD_YLW_", //#define    21
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'G')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'R')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'N')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '4')    )
		{
		   // ESP_LOGI(TAG, "IR_CMD_GRN4"); //"IR_CMD_GRN4", //#define    22
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'P')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == 'P')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == 'L')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '3')    )
		{
		  // ESP_LOGI(TAG, "IR_CMD_PPL3"); //"IR_CMD_PPL3", //#define    23
                  // Сиреневая
                  b_FromWiFi =  true;
                  b_Restore_AM = true;
                  u8_ManualButton = MANUAL_MODE_BUTTON_MANUAL;  
                  Process_Mode_Manual_Button  ();
		}
		else
		if ( (u8_WiFi_RxArray_A [u8_StrCounter+17] == 'F')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+18] == '2')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+19] == '_')&&
		     (u8_WiFi_RxArray_A [u8_StrCounter+20] == '_')    )
		{
                  // ESP_LOGI(TAG, "IR_CMD_F2__"); //"IR_CMD_F2__"  //#define    24
                  // F - самая нижняя
                  b_FromWiFi =  true; 
                  b_Restore_AM = true;
                  u8_ManualButton = MANUAL_MODE_BUTTON_RAINBOW;
                  Process_Mode_Rainbow_Button  ();
		}    
            }
            else
            {
              // I (25126) MPU_V2: RS_R_ON
              if ( 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='R') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='_') &&
                  (  u8_StateMaschine ==   SM_MODE_SEQUENTIAL )    )
                  // Прием Команды запуска в режиме последовательного отображения.
              {
                //
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='R') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='N')     ) 
                {  
                  u32_Mode_Sqnt_SM    = SM_MODE_RED_UP;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN ;
                }
                else
                // I (574915) MPU_V2: RS_R_OF
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='R') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='F')     ) 
                {
                  // Гасим красный
                  u32_Mode_Sqnt_SM    = SM_MODE_RED_DOWN;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN;
                }
                else              
                // I (574915) MPU_V2: RS_G_ON   
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='G') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='N')     ) 
                {
                  // Раскачиваем G
                  u32_Mode_Sqnt_SM    = SM_MODE_GREEN_UP;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN ;

                }
                else 
                // I (488183) MPU_V2: RS_G_OF
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='G') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='F')     ) 
                {
                  // Гасим G
                  u32_Mode_Sqnt_SM    = SM_MODE_GREEN_DOWN;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN;
                }
                else 
                // I (500271) MPU_V2: RS_B_ON
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='B') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='N')     ) 
                {
                  // Раскачиваем B
                  u32_Mode_Sqnt_SM    = SM_MODE_BLUE_UP;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN ;
                }
                else 
                //  I (512373) MPU_V2: RS_B_OF
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='B') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='F')     ) 
                {
                  // Гасим B
                  u32_Mode_Sqnt_SM    = SM_MODE_BLUE_DOWN;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN;
                }
                else 
                // I (512373) MPU_V2: RS_U_ON
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='U') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='N')     ) 
                {
                  // Раскачиваем UV
                  // u8_Mode_Sqnt_Status  = MODE_STATUS_RUN       ; // начинаем выполнение
                  //u8_Mode_Sqnt_Request = SEQ_MODE_REQUEST_IDLE ; // Запрос не отправляем  
                  if (  u8_StateMaschine ==  SM_MODE_SEQUENTIAL     )
                  {
                      u32_Mode_Sqnt_SM    = SM_MODE_UV_UP;
                      u8_Mode_Sqnt_Status = MODE_STATUS_RUN ;
                  }
                }
                // I (536581) MPU_V2: RS_U_OF
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='U') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='F')     ) 
                {
                  // Гасим UV
                  u32_Mode_Sqnt_SM    = SM_MODE_UV_DOWN;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN;
                }
                //
                // I (548685) MPU_V2: RS_W_ON
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='W') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='N')     ) 
                {
                  // Раскачиваем W
                  u32_Mode_Sqnt_SM    = SM_MODE_WHITE_UP;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN ;
                }
                // I (560773) MPU_V2: RS_W_OF
                if ( (u8_WiFi_RxArray_A [u8_StrCounter+13]=='W') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                     (u8_WiFi_RxArray_A [u8_StrCounter+16]=='F')     ) 
                {
                  // Гасим W
                  u32_Mode_Sqnt_SM    = SM_MODE_WHITE_DOWN;
                  u8_Mode_Sqnt_Status = MODE_STATUS_RUN;
                }
              }    // if  // I (25126) MPU_V2: RS_R_ON
              else
              {
                // I (18361) MPU_V2: RNB_R_ON
                if ( 
                    (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='R') && 
                    (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='N') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='B') &&  
                    (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='_') &&
                    (  u8_StateMaschine ==   SM_MODE_RAINBOW )    )
                  // Прием Команды запуска в режиме радуги.
                {
                  //
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='R') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='N')     ) 
                  {
                    // Раскачиваем красный
                    u32_Mode_Rnbw_SM     = SM_MODE_RED_UP;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN ;
                  }
                  else
                  // I (30475) MPU_V2: RNB_G_ON
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='G') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='N')     ) 
                  {
                    // Раскачиваем зеленый
                    u32_Mode_Rnbw_SM     = SM_MODE_GREEN_UP;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN;
                  }
                  else              
                  // I (30555) MPU_V2: RNB_R_OF   
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='R') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='F')     ) 
                  {
                    // Гасим R
                    u32_Mode_Rnbw_SM     = SM_MODE_RED_DOWN;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN ;                    
                  }
                  else 
                  // I (42665) MPU_V2: RNB_B_ON
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='B') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='N')     ) 
                  {
                    // Раскачиваем синий.
                    u32_Mode_Rnbw_SM     = SM_MODE_BLUE_UP;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN;
                  }
                  else 
                  // I (54767) MPU_V2: RNB_G_OF
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='G') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='F')     ) 
                  {
                    // Гасим Зеленый
                    u32_Mode_Rnbw_SM     = SM_MODE_GREEN_DOWN;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN ;
                  }
                  else 
                  // I (66895) MPU_V2: RNB_B_OF
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='B') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='F')     ) 
                  {
                    // Гасим B
                    u32_Mode_Rnbw_SM    = SM_MODE_BLUE_DOWN;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN;
                  }
                  else 
                  // I (78991) MPU_V2: RNB_U_ON
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='U') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='N')     ) 
                  {
                    // Раскачиваем UV
                    u32_Mode_Rnbw_SM     = SM_MODE_UV_UP;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN ;
                  }
                  else
                  // I (91099) MPU_V2: RNB_U_OF
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='U') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='F')     ) 
                  {
                    // Гасим UV
                    u32_Mode_Rnbw_SM     = SM_MODE_UV_DOWN;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN;
                  }
                  else
                  //
                  // I (548685) MPU_V2: RNB_W_ON
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='W') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='N')     ) 
                  {
                    // Раскачиваем W
                    u32_Mode_Rnbw_SM     = SM_MODE_WHITE_UP;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN ;
                  }
                  else
                  // I (560773) MPU_V2: RNB_W_OF
                  if ( (u8_WiFi_RxArray_A [u8_StrCounter+14]=='W') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+16]=='O') &&
                       (u8_WiFi_RxArray_A [u8_StrCounter+17]=='F')     ) 
                  {
                    // Гасим W
                    u32_Mode_Rnbw_SM     = SM_MODE_WHITE_DOWN;
                     u8_Mode_Rnbw_Status = MODE_STATUS_RUN;
                  }
                }
              }
            }
            //   
            //else //?                         
            //I (233342) xPU_V2: MASTER_MANL
            if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='M') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='A') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='M') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='A') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='N') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='L')    
               )
            {
                b_FS_ModeDefined  = true;
                u8_FS_Mode        = FS_MODE_MANUAL   ;         
                break;
            }
            else //?  
            //
            // I (32433) xPU_V2: MASTER_STRB
            if (  (u8_ManualButton                     != MANUAL_MODE_BUTTON_STROBOSCOPE ) &&
                  (u8_StateMaschine                    != SM_MODE_STROBOSCOPE            ) && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='M') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='A') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='S') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='T') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='R') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='B')    
               )
            {
                b_FS_ModeDefined  = true;
                u8_FS_Mode        = FS_MODE_STROBE   ;         
                break;
            }
            else //?   
            // I (37347) xPU_V2: MASTER_SQNT
            if (  (u8_ManualButton                     != MANUAL_MODE_BUTTON_SEQUENTIAL ) &&
                  (u8_StateMaschine                    != SM_MODE_SEQUENTIAL            ) && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='M') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='A') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='S') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='Q') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='N') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='T')    
               )
            {
              //  b_Restore_AM      = true;
                b_FS_ModeDefined  = true;
                u32_Mode_Sqnt_SM            =      SM_MODE_IDLE;
                u8_FS_Mode        = FS_MODE_SEQUENTIAL   ;         
                break;
            }
            else //?  
            // I (97354) xPU_V2: MASTER_RNBW
            if (  (u8_ManualButton                     != MANUAL_MODE_BUTTON_RAINBOW ) &&
                  (u8_StateMaschine                    != SM_MODE_RAINBOW            ) && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='M') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='A') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='R') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='N') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='B') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='W')    
               )
            {
             //   b_Restore_AM      = true;
                b_FS_ModeDefined  = true;
                u32_Mode_Rnbw_SM            =      SM_MODE_IDLE;
                u8_FS_Mode        = FS_MODE_RAINBOW   ;         
                break;
            }
            else //?  
            //I (18308) xPU_V2: RGBUW_030_000_000_000_000 - Параметр ручного режима
            if (  ( b_FS_ParamDefined[FS_PARAM_MANUAL] == false) &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='G') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='B') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='U') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='W') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='_') && // 16 17 18
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='_') && // 20 21 22 
                  (u8_WiFi_RxArray_A [u8_StrCounter+23]=='_') && // 24 25 26  
                  (u8_WiFi_RxArray_A [u8_StrCounter+27]=='_') && // 28 29 30 
                  (u8_WiFi_RxArray_A [u8_StrCounter+31]=='_')    // 32 33 34
               )
            {
          //    if ( b_FS_ParamDefined[FS_PARAM_MANUAL]  == false)
              {
                  b_FS_ParamDefined[FS_PARAM_MANUAL]  = true; 
                  //
                  u8_FS_R_Perc = RetrivePercData(u8_WiFi_RxArray_A,u8_StrCounter, 0) ;
                  u8_FS_G_Perc = RetrivePercData(u8_WiFi_RxArray_A,u8_StrCounter, 4) ;
                  u8_FS_B_Perc = RetrivePercData(u8_WiFi_RxArray_A,u8_StrCounter, 8) ;
                  u8_FS_U_Perc = RetrivePercData(u8_WiFi_RxArray_A,u8_StrCounter,12) ;
                  u8_FS_W_Perc = RetrivePercData(u8_WiFi_RxArray_A,u8_StrCounter,16) ;                  
              }           
              break;
            }
            else //?  
            // I (252001) xPU_V2: STB_WHT параметр стробоскопа
             if ( (b_FS_ParamDefined[FS_PARAM_STROBE]  == false)&&
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='B') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='_') 
               )
            {
            // if ( b_FS_ParamDefined[FS_PARAM_STROBE]  == false)
              {  
                  if (
                      (u8_WiFi_RxArray_A [u8_StrCounter+14]=='R') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+15]=='E') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+16]=='D')   )
                  {
                    //b_Restore_AM = true;
                    u8_FS_Strobe_ActCh = _ACTUAL_RED; 
                    b_FS_ParamDefined[FS_PARAM_STROBE]  = true; 
                  }
                  else
                  if (
                      (u8_WiFi_RxArray_A [u8_StrCounter+14]=='G') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+15]=='R') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+16]=='N')   )
                  {
                  //  b_Restore_AM = true;
                    u8_FS_Strobe_ActCh = _ACTUAL_GRN; 
                    b_FS_ParamDefined[FS_PARAM_STROBE]  = true; 
                  }
                  else
                  if (
                      (u8_WiFi_RxArray_A [u8_StrCounter+14]=='B') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+15]=='L') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+16]=='U')   )
                  {
                   // b_Restore_AM = true;
                    u8_FS_Strobe_ActCh = _ACTUAL_BLU; 
                    b_FS_ParamDefined[FS_PARAM_STROBE]  = true; 
                  }
                  else
                  if (
                      (u8_WiFi_RxArray_A [u8_StrCounter+14]=='_') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+15]=='U') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+16]=='V')   )
                  {
                   // b_Restore_AM = true;
                    u8_FS_Strobe_ActCh = _ACTUAL_UV; 
                    b_FS_ParamDefined[FS_PARAM_STROBE]  = true; 
                  }
                  else
                  if (
                      (u8_WiFi_RxArray_A [u8_StrCounter+14]=='W') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+15]=='H') &&
                      (u8_WiFi_RxArray_A [u8_StrCounter+16]=='T')   )
                  {
                    //b_Restore_AM = true;
                    u8_FS_Strobe_ActCh = _ACTUAL_WHT; 
                    b_FS_ParamDefined[FS_PARAM_STROBE]  = true; 
                  }
                  //                 
              }           
              break;
            }
            else //?  
            // I (381) MPU_V2: ESP_NET_SOFTAP
            if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='P') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='N') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='T') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='S') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='O') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='F') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+21]=='T') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+22]=='A') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+23]=='P')       
               )
            {
              //
              u8_LED1_State            = LED_1_AP_Mode;
              b_Send_ESP32_LED1_Packet = true;
              //          
              break;
            }
            else
              //I (1208) MPU_V2: ESP_NET_NODE
              if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='P') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='N') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='T') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='N') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='O') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='D') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+21]=='E')       
               )
            {
              //
              u8_LED1_State            = LED_1_Node_Mode;
              b_Send_ESP32_LED1_Packet = true; 
              //          
              break;
            }
            else //?  
            // I (585) MPU_V2: ESP_ROLE_MASTER
            if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='P') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='L') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='E') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='M') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='A') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+21]=='S') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+22]=='T') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+23]=='E') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+24]=='R')    
               )
            {
              //
              u8_LED2_State            = LED_2_MASTER;
              b_Send_ESP32_LED2_Packet = true;
              //          
              break;
            }
            else
            //  I (1408) MPU_V2: ESP_ROLE_SLAVE
              if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='P') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='L') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='E') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='S') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='L') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+21]=='A') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+22]=='V') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+23]=='E') 
                 )
              {
                //
                u8_LED2_State            = LED_2_SLAVE;
                b_Send_ESP32_LED2_Packet = true;
                //          
                break;
              }  
              else
              // I (1760) xPU_V2: ESP_ROLE_ECHO_SLAVE
              if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='P') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]=='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]=='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]=='O') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='L') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]=='E') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='E') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]=='C') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+21]=='H') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+22]=='O') &&   
                  (u8_WiFi_RxArray_A [u8_StrCounter+23]=='_') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+24]=='S') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+25]=='L') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+26]=='A') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+27]=='V') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+28]=='E') 
                 )
              {
                //
                u8_LED2_State            = LED_2_ECHO_SLAVE;
                b_Send_ESP32_LED2_Packet = true;
                //          
                break;
              }
            else //?   
          // I (334) MPU_V2: MAC 24-6F-28-3F-61-70
          if (
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]=='M') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]=='A') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]=='C') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]==' ') &&
               // (u8_WiFi_RxArray_A [u8_StrCounter+14]=='0') &&
               // (u8_WiFi_RxArray_A [u8_StrCounter+15]=='1') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]=='-') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+17]=='2') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+18]=='3') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]=='-') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+20]=='4') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+21]=='5') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+22]=='-') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+23]=='6') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+24]=='7') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+25]=='-') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+26]=='8') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+27]=='9') &&  
                  (u8_WiFi_RxArray_A [u8_StrCounter+28]=='-')   
               // (u8_WiFi_RxArray_A [u8_StrCounter+17]=='A') &&  
               // (u8_WiFi_RxArray_A [u8_StrCounter+18]=='B')              
                 )
              {
                //
                RetriveMAC( &u8_WiFi_RxArray_A[0], u8_StrCounter );
                //
                if (  u8_WiFi_MAC_1 || u8_WiFi_MAC_2 || u8_WiFi_MAC_3 || 
                      u8_WiFi_MAC_4 || u8_WiFi_MAC_5 || u8_WiFi_MAC_6    )
                {
                  // Считали ненулевой MAC. Сравниваем с таблицей и выносим решение
                 u32_LLU_V2_S_N   = ProcessMAC();
                  //
                  if (  u32_LLU_V2_S_N )
                  {
                     u8_MODE_State   = MODE_BYTE_WIFI_MODE;
                     u8_MODE_Param_1 = WIFI_MODE_IDLE; // Состояние по умолчанию.
                     //
                     //
                     if( u32_ActualNetRole == ESP32_NET_ROLE_DEFAULT)
                     {
                       if ( u32_LLU_V2_S_N == LPU_V2_SEAN ) { u8_MODE_Param_1 =      WIFI_MODE_NODE ; } // u32_ActualNetRole = ESP32_NET_ROLE_SLAVE     ;
                       if ( u32_LLU_V2_S_N == MPU_V2_SEAN ) { u8_MODE_Param_1 = WIFI_MODE_ECHO_NODE ; } // u32_ActualNetRole = ESP32_NET_ROLE_ECHOSLAVE ;
                       if ( u32_LLU_V2_S_N == HPU_V2_SEAN ) { u8_MODE_Param_1 =      WIFI_MODE_NODE ; } // u32_ActualNetRole = ESP32_NET_ROLE_SLAVE     ; 
                       //
                       if ( u32_LLU_V2_S_N == LPU_V2_OLEG ) { u8_MODE_Param_1 =      WIFI_MODE_NODE ; } // u32_ActualNetRole = ESP32_NET_ROLE_SLAVE     ; 
                       if ( u32_LLU_V2_S_N == MPU_V2_OLEG ) { u8_MODE_Param_1 =      WIFI_MODE_AP   ; } // u32_ActualNetRole = ESP32_NET_ROLE_MASTER    ; 
                       if ( u32_LLU_V2_S_N == HPU_V2_OLEG ) { u8_MODE_Param_1 =      WIFI_MODE_NODE ; } // u32_ActualNetRole = ESP32_NET_ROLE_SLAVE     ;     
                       //
                     }
                     else
                     {
                       if (u32_ActualNetRole == ESP32_NET_ROLE_MASTER    )  u8_MODE_Param_1 = WIFI_MODE_AP        ;  
                       if (u32_ActualNetRole == ESP32_NET_ROLE_ECHOSLAVE )  u8_MODE_Param_1 = WIFI_MODE_ECHO_NODE ; 
                       if (u32_ActualNetRole == ESP32_NET_ROLE_SLAVE     )  u8_MODE_Param_1 = WIFI_MODE_NODE      ;                      
                     }
                     //
                     u8_MODE_Param_2= 0xF0;
                     //
                     b_Send_ESP32_WIFI_MODE_Packet = true;
                  } 
                  else 
                  { 
                    WiFi_MAC_Error();
                  }

                }
                else
                {
                  
                }
                //
              //  u8_LED2_State            = LED_2_SLAVE;
              //  b_Send_ESP32_LED2_Packet = true;
                //          
                break;
              }  
            else
             if ( // ESP32_NET_ROLE_MASTER 
                  (u8_WiFi_RxArray_A [u8_StrCounter+10]  =='E') && 
                  (u8_WiFi_RxArray_A [u8_StrCounter+11]  =='S') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+12]  =='P') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+13]  =='3') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+14]  =='2') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+15]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+16]  =='N') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+17]  =='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+18]  =='T') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+19]  =='_') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+20]  =='R') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+21]  =='O') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+22]  =='L') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+23]  =='E') &&
                  (u8_WiFi_RxArray_A [u8_StrCounter+24]  =='_') &&                  
                  (  u8_StateMaschine ==   SM_MODE_MANUAL )         )
                  // Модуль подтвержид переход в режим задания роли устройства.
              {
                if(
                    (u8_WiFi_RxArray_A [u8_StrCounter+25]  =='M') && 
                    (u8_WiFi_RxArray_A [u8_StrCounter+26]  =='A') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+27]  =='S') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+28]  =='T') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+29]  =='E') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+30]  =='R')       )
                {
                   u8_StateMaschine =   SM_MODE_SERVICE ;
                }
                else
                if(
                    (u8_WiFi_RxArray_A [u8_StrCounter+25]  =='E') && 
                    (u8_WiFi_RxArray_A [u8_StrCounter+26]  =='C') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+27]  =='H') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+28]  =='O') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+29]  =='S') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+30]  =='L')       )
                {
                   u8_StateMaschine =   SM_MODE_SERVICE ;
                }            
                else
                if(
                    (u8_WiFi_RxArray_A [u8_StrCounter+25]  =='S') && 
                    (u8_WiFi_RxArray_A [u8_StrCounter+26]  =='L') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+27]  =='A') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+28]  =='V') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+29]  =='E')       )
                {
                   u8_StateMaschine =   SM_MODE_SERVICE ;
                }
                else
                if(
                    (u8_WiFi_RxArray_A [u8_StrCounter+25]  =='D') && 
                    (u8_WiFi_RxArray_A [u8_StrCounter+26]  =='E') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+27]  =='F') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+28]  =='A') &&
                    (u8_WiFi_RxArray_A [u8_StrCounter+29]  =='U')       )
                {
                   u8_StateMaschine =   SM_MODE_SERVICE ;
                }                
                
              }
        }
      }// End of if ( (u8_WiFi_RxArray_A [u8_StrCounter]==')') && (u8_WiFi_RxArray_A [u8_StrCounter+1]==' ')  )
   }  

    
    u8_StrCounter++   ;
  }
  //  
  //

  

  //
  /*  REDUCE 
  if (u8_WiFi_RxArray_A_Counter == 15)
  {
    // OperMode_Manual
    if (  (u8_WiFi_RxArray_A[ 0] == 'O' ) && 
          (u8_WiFi_RxArray_A[ 1] == 'p' ) && 
          (u8_WiFi_RxArray_A[ 2] == 'e' ) && 
          (u8_WiFi_RxArray_A[ 3] == 'r' ) && 
          (u8_WiFi_RxArray_A[ 4] == 'M' ) && 
          (u8_WiFi_RxArray_A[ 5] == 'o' ) && 
          (u8_WiFi_RxArray_A[ 6] == 'd' ) && 
          (u8_WiFi_RxArray_A[ 7] == 'e' ) && 
          (u8_WiFi_RxArray_A[ 8] == '_' ) &&
          (u8_WiFi_RxArray_A[ 9] == 'M' ) &&  
          (u8_WiFi_RxArray_A[10] == 'a' ) &&  
          (u8_WiFi_RxArray_A[11] == 'n' ) &&  
          (u8_WiFi_RxArray_A[12] == 'u' ) &&  
          (u8_WiFi_RxArray_A[13] == 'a' ) &&  
          (u8_WiFi_RxArray_A[14] == 'l' )        )
    {
   // u8_StateMaschine = SM_MODE_MANUAL;
   //   u8_ManualChannel = MANUAL_MODE_RED;   
      if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
          u8_ProcessWiFiCommand = WIFI_COMMAND_SET_MODE_MANUAL;
    }
    //
    //
    if (u8_WiFi_RxArray_A_Counter == 15)
    {
      // OperMode___Auto
      if (  (u8_WiFi_RxArray_A[ 0] == 'O' ) && 
            (u8_WiFi_RxArray_A[ 1] == 'p' ) && 
            (u8_WiFi_RxArray_A[ 2] == 'e' ) && 
            (u8_WiFi_RxArray_A[ 3] == 'r' ) && 
            (u8_WiFi_RxArray_A[ 4] == 'M' ) && 
            (u8_WiFi_RxArray_A[ 5] == 'o' ) && 
            (u8_WiFi_RxArray_A[ 6] == 'd' ) && 
            (u8_WiFi_RxArray_A[ 7] == 'e' ) && 
            (u8_WiFi_RxArray_A[ 8] == '_' ) &&
            (u8_WiFi_RxArray_A[ 9] == '_' ) &&  
            (u8_WiFi_RxArray_A[10] == '_' ) &&  
            (u8_WiFi_RxArray_A[11] == 'A' ) &&  
            (u8_WiFi_RxArray_A[12] == 'u' ) &&  
            (u8_WiFi_RxArray_A[13] == 't' ) &&  
            (u8_WiFi_RxArray_A[14] == 'o' )      )
      {
   //  i16_LightLevelRed_AM   =   RED_MAX_INDEX; 
     //   i16_LightLevelGreen_AM = GREEN_MAX_INDEX;
      //  i16_LightLevelBlue_AM  =  BLUE_MAX_INDEX;
      //  i16_LightLevelUv_AM    =    UV_MAX_INDEX;
      //  i16_LightLevelWhite_AM = WHITE_MAX_INDEX;
        //
      //  Process_Mode_A_Button();  
        if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
          u8_ProcessWiFiCommand = WIFI_COMMAND_SET_MODE_AUTO;
      }
    }
  }  
  //
  // SetSpeed 018
  if (u8_WiFi_RxArray_A_Counter == 12)
  {
    if (  (u8_WiFi_RxArray_A[ 0] == 'S' ) && 
          (u8_WiFi_RxArray_A[ 1] == 'e' ) && 
          (u8_WiFi_RxArray_A[ 2] == 't' ) && 
          (u8_WiFi_RxArray_A[ 3] == 'S' ) && 
          (u8_WiFi_RxArray_A[ 4] == 'p' ) && 
          (u8_WiFi_RxArray_A[ 5] == 'e' ) && 
          (u8_WiFi_RxArray_A[ 6] == 'e' ) && 
          (u8_WiFi_RxArray_A[ 7] == 'd' ) && 
          (u8_WiFi_RxArray_A[ 8] == ' ' )        )
    {
      c_SetSpeedValue[0+0] = u8_WiFi_RxArray_A[9+0]; 
      c_SetSpeedValue[0+1] = u8_WiFi_RxArray_A[9+1]; 
      c_SetSpeedValue[0+2] = u8_WiFi_RxArray_A[9+2];
      c_SetSpeedValue[0+3] = 0                     ;
      //
      // Это значения в диапазоне от 0 до 99
      u8_SetSpeedValue = atoi( c_SetSpeedValue);
      //
      //
      if (u8_SetSpeedValue < UPDATE_DELAY_MIN_MS)u8_SetSpeedValue = UPDATE_DELAY_MIN_MS;
      if (u8_SetSpeedValue > UPDATE_DELAY_MAX_MS)u8_SetSpeedValue = UPDATE_DELAY_MAX_MS;
      //
      //u8_StateMaschine = SM_MODE_MANUAL;
      b_UpdateSetSpeed = true; // AutoModeActualSpeed  
      //
      if (u8_ProcessWiFiCommand == WIFI_COMMAND_IDLE)
        u8_ProcessWiFiCommand = WIFI_COMMAND_SET_CHANGE_SPEED;
    }  
  }
   REDUCE */
  //
  u8_WiFi_RxArray_A_Counter=0;
}

//
//
void WiFi_ProcessTx( void )
{
  //
  if ( u32_WIFI_TX_ActualPhase == WIFI_TX_WAIT_FOR_START   )
  {
    //   
    // БЫСТРЫЙ БУФЕР A
     if( ( b_WIFI_TX_Ready_A_Fast  == true ) && (u32_WIFI_TX_ActualChannel == WIFI_TX_BUF_X) )
     {
        u32_WIFI_TX_ActualChannel = WIFI_TX_BUF_A_FAST;
          b_WIFI_TX_Ready_A_Fast = false; // Блокируем повторный вход
        u32_WIFI_TX_ActualPhase = WIFI_TX_SEND_BYTE;
        return;
     }
     // БЫСТРЫЙ БУФЕР B
     if( ( b_WIFI_TX_Ready_B_Fast  == true ) && (u32_WIFI_TX_ActualChannel == WIFI_TX_BUF_X) )
     {
         u32_WIFI_TX_ActualChannel = WIFI_TX_BUF_B_FAST;
           b_WIFI_TX_Ready_B_Fast = false; // Блокируем повторный вход
         u32_WIFI_TX_ActualPhase = WIFI_TX_SEND_BYTE;
         return;
     }
  }
  else
  {
    if (u32_WIFI_TX_ActualPhase == WIFI_TX_SEND_BYTE )
    {

      // БЫСТРЫЙ БУФЕР A
      if(u32_WIFI_TX_ActualChannel == WIFI_TX_BUF_A_FAST)
      {
        if (u16_WIFI_TX_SendCounter_A_Fast<u16_WIFI_TX_ActualSend_A_Fast)
        {
           LL_USART_IsActiveFlag_TXE (WIFI_USART_INSTANCE);
           LL_USART_TransmitData8(WIFI_USART_INSTANCE, u8_WIFI_TX_A_Fast[u16_WIFI_TX_SendCounter_A_Fast++]); // USART_SendData
           u32_WIFI_TX_ActualPhase = WIFI_TX_WAIT_FOR_SEND;
        }
        else
        {
          u32_WIFI_TX_ActualPhase          = WIFI_TX_WAIT_FOR_START;
          u16_WIFI_TX_SendCounter_A_Fast   =                      0; // Счетчик следующего элемента   
          u16_WIFI_TX_ActualSend_A_Fast    =                      0; // Емкость отправляемого массива
          //
          b_WIFI_TX_Ready_A_Fast   = 0; // Этот флаг оценивается в сервисе отправки
          b_WIFI_TX_Sended_A_Fast  = 1; // Этот флаг поставится когда очередь будет отправлена   
          //
          u32_WIFI_TX_ActualChannel = WIFI_TX_BUF_X;
          //
          u32_WiFi_Tx_Busy = false;
        }
      }
      // БЫСТРЫЙ БУФЕР B
      if(u32_WIFI_TX_ActualChannel == WIFI_TX_BUF_B_FAST)
      {
        if (u16_WIFI_TX_SendCounter_B_Fast<u16_WIFI_TX_ActualSend_B_Fast)
        { 
           LL_USART_TransmitData8(WIFI_USART_INSTANCE, u8_WIFI_TX_B_Fast[u16_WIFI_TX_SendCounter_B_Fast++]); // USART_SendData
           u32_WIFI_TX_ActualPhase = WIFI_TX_WAIT_FOR_SEND;
        }
        else
        {
          u32_WIFI_TX_ActualPhase   = WIFI_TX_WAIT_FOR_START;
          u16_WIFI_TX_SendCounter_B_Fast   =                      0; // Счетчик следующего элемента   
          u16_WIFI_TX_ActualSend_B_Fast    =                      0; // Емкость отправляемого массива
          //
          b_WIFI_TX_Ready_B_Fast   = 0; // Этот флаг оценивается в сервисе отправки
          b_WIFI_TX_Sended_B_Fast  = 1; // Этот флаг поставится когда очередь будет отправлена   
          //
          u32_WIFI_TX_ActualChannel = WIFI_TX_BUF_X;
        }
      }
      
   }
   else
   {
      if (u32_WIFI_TX_ActualPhase == WIFI_TX_WAIT_FOR_SEND )
      {
        //if (USART_GetFlagStatus(WIFI_USART_INSTANCE, LL_USART_SR_TC) == SET)      
        if (LL_USART_IsActiveFlag_TC (WIFI_USART_INSTANCE) == SET )
        {
          u32_WIFI_TX_ActualPhase = WIFI_TX_SEND_BYTE;
          return;
        } 
      }
        else
        {
          // непредвиденное состояние.
        }      
    }
  }
}



/**
  * @brief  This function configures USARTx Instance.
  * @note   This function is used to :
  *         -1- Enable GPIO clock and configures the USART pins.
  *         -2- Enable the USART peripheral clock and clock source.
  *         -3- Configure USART functional parameters.
  *         -4- Enable USART.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void WiFi_USART_Init(void)
{

  /* (1) Enable GPIO clock and configures the USART pins *********************/

  /* Enable the peripheral clock of GPIO Port */
  WIFI_USART_GPIO_CLK_ENABLE();

  /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
  LL_GPIO_SetPinMode(WIFI_USART_TX_GPIO_PORT, WIFI_USART_TX_PIN, LL_GPIO_MODE_ALTERNATE);
  WIFI_USART_SET_TX_GPIO_AF();
  LL_GPIO_SetPinSpeed(WIFI_USART_TX_GPIO_PORT, WIFI_USART_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(WIFI_USART_TX_GPIO_PORT, WIFI_USART_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(WIFI_USART_TX_GPIO_PORT, WIFI_USART_TX_PIN, LL_GPIO_PULL_UP);

  /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
  LL_GPIO_SetPinMode(WIFI_USART_RX_GPIO_PORT, WIFI_USART_RX_PIN, LL_GPIO_MODE_ALTERNATE);
  WIFI_USART_SET_RX_GPIO_AF();
  LL_GPIO_SetPinSpeed(WIFI_USART_RX_GPIO_PORT, WIFI_USART_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(WIFI_USART_RX_GPIO_PORT, WIFI_USART_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(WIFI_USART_RX_GPIO_PORT, WIFI_USART_RX_PIN, LL_GPIO_PULL_UP);

  /* (2) Enable USART peripheral clock and clock source ***********************/
  WIFI_USART_CLK_ENABLE();

  /* (3) Configure USART functional parameters ********************************/
  
  /* Disable USART prior modifying configuration registers */
  /* Note: Commented as corresponding to Reset value */
  // LL_USART_Disable(USARTx_INSTANCE);

  /* TX/RX direction */
  LL_USART_SetTransferDirection(WIFI_USART_INSTANCE, LL_USART_DIRECTION_TX_RX);

  /* 8 data bit, 1 start bit, 1 stop bit, no parity */
  LL_USART_ConfigCharacter(WIFI_USART_INSTANCE, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

  /* No Hardware Flow control */
  /* Reset value is LL_USART_HWCONTROL_NONE */
  // LL_USART_SetHWFlowCtrl(USARTx_INSTANCE, LL_USART_HWCONTROL_NONE);

  /* Oversampling by 16 */
  /* Reset value is LL_USART_OVERSAMPLING_16 */
  // LL_USART_SetOverSampling(USARTx_INSTANCE, LL_USART_OVERSAMPLING_16);

  /* Set Baudrate to 115200 using APB frequency set to 100000000/APB_Div Hz */
  /* Frequency available for USART peripheral can also be calculated through LL RCC macro */
  /* Ex :
      Periphclk = LL_RCC_GetUSARTClockFreq(Instance); or LL_RCC_GetUARTClockFreq(Instance); depending on USART/UART instance
  
      In this example, Peripheral Clock is expected to be equal to 100000000/APB_Div Hz => equal to SystemCoreClock/APB_Div
  */
  LL_USART_SetBaudRate(WIFI_USART_INSTANCE, SystemCoreClock/APB_Div, LL_USART_OVERSAMPLING_8, 115200); //LL_USART_OVERSAMPLING_16

  /* (4) Enable USART *********************************************************/
  LL_USART_Enable(WIFI_USART_INSTANCE);
}



//
//__STATIC_INLINE 
void WiFi_Power_Init(void)
{
  /* Enable the  Clock */
  WIFI_POWER_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED2 */
  LL_GPIO_SetPinMode(WIFI_POWER_PORT, WIFI_POWER_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  LL_GPIO_SetPinOutputType(WIFI_POWER_PORT, WIFI_POWER_PIN, LL_GPIO_OUTPUT_PUSHPULL);//++
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW LL_GPIO_SPEED_FREQ_LOW*/
  LL_GPIO_SetPinSpeed(WIFI_POWER_PORT, WIFI_POWER_PIN, LL_GPIO_SPEED_FREQ_LOW);//++
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
  //
  LL_GPIO_SetPinPull(WIFI_POWER_PORT, WIFI_POWER_PIN, LL_GPIO_PULL_DOWN);
}
//
// __STATIC_INLINE 
void WiFi_ENABLE_Init(void)
{
  /* Enable the  Clock */
  WIFI_ENABLE_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED2 */
  LL_GPIO_SetPinMode(WIFI_ENABLE_PORT, WIFI_ENABLE_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  LL_GPIO_SetPinOutputType(WIFI_ENABLE_PORT, WIFI_ENABLE_PIN, LL_GPIO_OUTPUT_PUSHPULL);//++
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW LL_GPIO_SPEED_FREQ_LOW*/
  LL_GPIO_SetPinSpeed(WIFI_ENABLE_PORT, WIFI_ENABLE_PIN, LL_GPIO_SPEED_FREQ_LOW);//++
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
  //
  LL_GPIO_SetPinPull(WIFI_ENABLE_PORT, WIFI_ENABLE_PIN, LL_GPIO_PULL_DOWN);
}
/*
bool b_Send_ESP32_LED1_Packet    = false;
bool b_Send_ESP32_LED2_Packet    = false;
bool b_Send_ESP32_LED3_Packet    = false;
bool b_Send_ESP32_LED4_Packet    = false;
bool b_Send_ESP32_LED5_Packet    = false;
bool b_Send_ESP32_LED6_Packet    = false;
bool b_Send_ESP32_LED7_Packet    = false;
bool b_Send_ESP32_LED8_Packet    = false;
//
// MODE_BYTE_WIFI_MODE  
bool b_Send_ESP32_WIFI_MODE_Packet = false;
//
// MODE_BYTE_BTN_CMD
bool b_Send_ESP32_BTN_CMD_Packet = false;
//bool b_Send_ESP32_SeqMode_Packet  = false;
//bool b_Send_ESP32_Rsrv1_Packet    = false;
//bool b_Send_ESP32_Rsrv2_Packet    = false;
*/
// Отправка данных о светодиодах из контроллера в модуль.
void Process_ESP32_LEDS(void)
{
  static bool b_CreateSendLedsPacket = false;
  // если не отправили прежний пакет - выходим из подготовки данного пакета
  if (u32_WiFi_Tx_Busy == true)
    return;
  //
  b_CreateSendLedsPacket = false;
  //
  static uint32_t  u32_CallCounter=0;
  u32_CallCounter++;
  if (u32_CallCounter>= (3800))
  {
    // Это 0,05 секунды или 3800 вызовов. Частота вызовов 76000
    u32_CallCounter=0;
    // ПЕРВОНАЧАЛЬНО =  ОБНОВЛЕНИЕ В ПАКЕТАХ - НЕ МУЛЬТИПЛЕКСИРУЕТСЯ.
    // ТО ЕСТЬ - ОТДЕЛЬНЫЙ ПАКЕТ для ОТДЕЛЬНОГО ОБНОВЛЕНИЯ
    // В данном варианте - все обновления в одном пакете.
    //
    if ( b_Send_ESP32_LED1_Packet == true)
    {    b_Send_ESP32_LED1_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    } 
    if ( b_Send_ESP32_LED2_Packet == true)
    {    b_Send_ESP32_LED2_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    } 
    if ( b_Send_ESP32_LED3_Packet == true)
    {    b_Send_ESP32_LED3_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    } 
    if ( b_Send_ESP32_LED4_Packet == true)
    {    b_Send_ESP32_LED4_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    } 
    if ( b_Send_ESP32_LED5_Packet == true)
    {    b_Send_ESP32_LED5_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    } 
    if ( b_Send_ESP32_LED6_Packet == true)
    {    b_Send_ESP32_LED6_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    } 
    if ( b_Send_ESP32_LED7_Packet == true)
    {    b_Send_ESP32_LED7_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    } 
    if ( b_Send_ESP32_LED8_Packet == true )
    {    b_Send_ESP32_LED8_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    }
    if (b_Send_ESP32_WIFI_MODE_Packet == true )
    {   b_Send_ESP32_WIFI_MODE_Packet = false;
        b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    }
    if ( b_Send_ESP32_IR_CMD_Packet == true )
    {    b_Send_ESP32_IR_CMD_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    }
    if ( b_Send_ESP32_BRT_CMD_Packet == true )
    {    b_Send_ESP32_BRT_CMD_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    }
    //
    // Отправка статуса
    if ( b_Send_ESP32_OP_MODE_CMD_Packet == true )
    {    b_Send_ESP32_OP_MODE_CMD_Packet = false;
         b_CreateSendLedsPacket = true ; // CreateSend_ESP32_LEDS_Packet();
    }
    //
    //
    if ( b_CreateSendLedsPacket == true )
    {    b_CreateSendLedsPacket = false;
         CreateSend_ESP32_LEDS_Packet();
    }
    //    else
 /*   if ( b_Send_ESP32_SeqMode_Packet  == true  )
    {
      b_Send_ESP32_SeqMode_Packet  = false  ;
      CreateSend_ESP32_LEDS_Packet();
    }*/
    //
  }
}

//
// Массивы прямого кодирования
uint8_t TxIncode[16] = 
{ 0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69, 0x78, 
  0x87, 0x96, 0xA5, 0xB4, 0xC3, 0xD2, 0xE1, 0xF0 };

       
//
bool b_LEDs_CSP_service_BUSY = false;
__STATIC_INLINE void CreateSend_ESP32_LEDS_Packet(void)
{
  static bool b_IR_Command =false;
  //
  if (b_LEDs_CSP_service_BUSY == true)
  {
    asm ("nop");
  }
  //
  b_LEDs_CSP_service_BUSY = true;
  //
  // Структура пакета 
  // - 0, 0  - синхростарт
  // - 16 половин, 8 байт ( 4 бита данных + 4 бита инверсии )
  // - FF FF - синростоп
  //
  u8_WIFI_TX_A_Fast [0] = 0x00;
  u8_WIFI_TX_A_Fast [1] = 0x01;
  //  A B C D E E
  if ( u8_LED1_State == LED_1_DEFAULT)               u8_WIFI_TX_A_Fast [2] = 0x0F; 
  else
      if ( u8_LED1_State == LED_1_NoConnection)      u8_WIFI_TX_A_Fast [2] = 0x1E;
      else
        if ( u8_LED1_State == LED_1_AP_Mode)         u8_WIFI_TX_A_Fast [2] = 0x2D;
        else
          if ( u8_LED1_State == LED_1_Node_Mode)     u8_WIFI_TX_A_Fast [2] = 0x3C;
          else
            if ( u8_LED1_State == LED_1_ChangeState) u8_WIFI_TX_A_Fast [2] = 0x4B;    
  //
  //  A B C D E E
  if ( u8_LED2_State == LED_2_DEFAULT)               u8_WIFI_TX_A_Fast [3] = 0x0F; 
  else
      if ( u8_LED2_State == LED_2_INDEPENDED)        u8_WIFI_TX_A_Fast [3] = 0x1E;
      else
        if ( u8_LED2_State == LED_2_MASTER)          u8_WIFI_TX_A_Fast [3] = 0x2D;
        else
          if ( u8_LED2_State == LED_2_ECHO_SLAVE)    u8_WIFI_TX_A_Fast [3] = 0x3C;
          else
            if ( u8_LED2_State == LED_2_SLAVE)       u8_WIFI_TX_A_Fast [3] = 0x4B;
            else
              if ( u8_LED2_State == LED_2_AUTONOMOUS)u8_WIFI_TX_A_Fast [3] = 0x5A;    
  //
  //  A B C D E E
  if ( u8_LED3_State == LED_3_DEFAULT)               u8_WIFI_TX_A_Fast [4] = 0x0F; 
  else
      if ( u8_LED3_State == LED_3_NoSync)            u8_WIFI_TX_A_Fast [4] = 0x1E;
      else
        if ( u8_LED3_State == LED_3_SentSync)        u8_WIFI_TX_A_Fast [4] = 0x2D;
        else
          if ( u8_LED3_State == LED_3_RecieveSync)   u8_WIFI_TX_A_Fast [4] = 0x3C;
  //
  //  A B C D E E
  if ( u8_LED4_State == LED_4_DEFAULT)               u8_WIFI_TX_A_Fast [5] = 0x0F; 
  else
      if ( u8_LED4_State == LED_4_NoSync)            u8_WIFI_TX_A_Fast [5] = 0x1E;
      else
        if ( u8_LED4_State == LED_4_SentSync)        u8_WIFI_TX_A_Fast [5] = 0x2D;
        else
          if ( u8_LED4_State == LED_4_RecieveSync)   u8_WIFI_TX_A_Fast [5] = 0x3C;
  //
  //  A B C D E E
  if ( u8_LED5_State == LED_5_DEFAULT)               u8_WIFI_TX_A_Fast [6] = 0x0F; 
  else
      if ( u8_LED5_State == LED_5_IR_COMMAND)        u8_WIFI_TX_A_Fast [6] = 0x1E;
      else
          if ( u8_LED5_State == LED_5_IR_KNOWN)      u8_WIFI_TX_A_Fast [6] = 0x2D;       
  //
  //  A B C D E E
  if ( u8_LED6_State == LED_6_DEFAULT)               u8_WIFI_TX_A_Fast [7] = 0x0F; 
  else
  {
    u8_LED6_State=u8_LED6_State + (u8_LED6_State*0x10)^0xFF;
    /*#define LED_6_SOC_01     1
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
      #define LED_6_SOC_100   13 */
  }
  //
  //  A B C D E E
  if ( u8_LED7_State == LED_7_DEFAULT)                u8_WIFI_TX_A_Fast [8] = 0x0F; 
  else
      if ( u8_LED7_State == LED_7_KBD_PRESSED)        u8_WIFI_TX_A_Fast [8] = 0x1E;
      else
         if ( u8_LED7_State == LED_7_KBD_CLICK)       u8_WIFI_TX_A_Fast [8] = 0x2D; 
         else
            if ( u8_LED7_State == LED_7_KBD_DBL)      u8_WIFI_TX_A_Fast [8] = 0x3C;
            else
                if ( u8_LED7_State == LED_7_KBD_UP)   u8_WIFI_TX_A_Fast [8] = 0x4B;
  //
  //  A B C D E E
  if ( u8_LED8_State == LED_8_DEFAULT)               u8_WIFI_TX_A_Fast [9] = 0x0F; 
  else
  {
    u8_LED8_State=u8_LED8_State + (u8_LED8_State*0x10)^0xFF;
    //
    // LED8 - Sync Clock quality
 /* #define LED_8_DEFAULT     0
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
    #define LED_8_Sync_100   13 */
    //
  } 
  //
  //  A B C D E E
  // Резерв 0
  /*
  if ( u8_MODE_State         == MODE_BYTE_IDLE       )  u8_WIFI_TX_A_Fast [10] = 0x0F; 
  else
    if ( u8_MODE_State       == MODE_BYTE_WIFI_MODE  )  u8_WIFI_TX_A_Fast [10] = 0x1E;     
    else
      if ( u8_MODE_State     == MODE_BYTE_UNIT_INDEX )  u8_WIFI_TX_A_Fast [10] = 0x2D;     
      else
        if ( u8_MODE_State   == MODE_BYTE_IR_CMD     )  u8_WIFI_TX_A_Fast [10] = 0x3C;     
        else
          if ( u8_MODE_State == MODE_BYTE_BTN_CMD    )  u8_WIFI_TX_A_Fast [10] = 0x4B; 
           else
                                                        u8_WIFI_TX_A_Fast [10] = 0x0F;  // Как для IDLE
*/
  // 
  u8_WIFI_TX_A_Fast [10] = 0x0F;  // Как для IDLE
  //
  // Первый полубайт для режима управления WIFI
  if ( u8_MODE_State       == MODE_BYTE_WIFI_MODE  )
  {
    u8_WIFI_TX_A_Fast [10] = 0x1E;  
    //
    if (u8_MODE_Param_1 ==          WIFI_MODE_IDLE )   u8_WIFI_TX_A_Fast [11] = 0x0F;
    else
      if (u8_MODE_Param_1 ==          WIFI_MODE_AP )   u8_WIFI_TX_A_Fast [11] = 0x1E;
      else
        if (u8_MODE_Param_1 ==      WIFI_MODE_NODE )   u8_WIFI_TX_A_Fast [11] = 0x2D;
        else
          if (u8_MODE_Param_1 == WIFI_MODE_ECHO_NODE ) u8_WIFI_TX_A_Fast [11] = 0x3C;
          else
                                                       u8_WIFI_TX_A_Fast [11] = 0x0F;
     //
     // После отправки параметра - выполняем его установку в начальное состояние.
     u8_MODE_State   = MODE_BYTE_IDLE              ;
     u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ;  // младшая половина параметра
     u8_MODE_Param_2 = MODE_PARAM_HIGH_NIBBLE_IDLE ; // старшая половина параметра
  }
  //
  // Первый полубайт для режима задания индекса
  if ( u8_MODE_State       == MODE_BYTE_UNIT_INDEX )
  { 
     u8_WIFI_TX_A_Fast [10] = 0x2D; 
    // Только млашдий полубайт параметра                                                                                
    if (  (u8_MODE_Param_1>= UNIT_INDEX_IDLE ) && ( u8_MODE_Param_1 <= UNIT_INDEX_009 ) )
    {
      u8_WIFI_TX_A_Fast [11]  = TxIncode[u8_MODE_Param_1];
    }
    else
      u8_WIFI_TX_A_Fast [11]  = TxIncode [UNIT_INDEX_IDLE];
    //
    u8_WIFI_TX_A_Fast [12] = 0x0F; // Заглушка 
  }
    
  // Первый полубайт для режима передачи в модуль ИК команды от пульта
  if ( u8_MODE_State       == MODE_BYTE_IR_CMD )
  {
    b_IR_Command = true;
    //
    u8_WIFI_TX_A_Fast [10] = 0x3C;
    //// Младший полубайт параметр
    if  ( (u8_MODE_Param_1>=IR_CMD_IDLE) && (u8_MODE_Param_1 <= IR_CMD_PRPL1) )
    { 
      u8_WIFI_TX_A_Fast [11] = TxIncode[u8_MODE_Param_1];
    }
    else
       u8_WIFI_TX_A_Fast [11] = TxIncode[IR_CMD_IDLE];
    //
    // Старший полубайт параметр
    if   ( (u8_MODE_Param_2>=IR_CMD_S1) && (u8_MODE_Param_2 <= IR_CMD_F2  ) )
    {
       u8_WIFI_TX_A_Fast [12] = TxIncode[u8_MODE_Param_2-15];  
    }
    else
       u8_WIFI_TX_A_Fast [12] = TxIncode[IR_CMD_IDLE]; //TxIncode[u8_MODE_Param_1-16];
      
     /* Это для второго полубайта                                             
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
      #define IR_CMD_F2      24
      #define IR_CMD_RNB  IR_CMD_F2 */  
     //
     // После отправки параметра - выполняем его установку в начальное состояние.
     u8_MODE_State   = MODE_BYTE_IDLE              ;
     u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ; // младшая  половина параметра
     u8_MODE_Param_2 = MODE_PARAM_HIGH_NIBBLE_IDLE ; // старшая  половина параметра
  } 
  else
    b_IR_Command = false;
  // 
  // Первый полубайт для режима передачи состояния кнопки
  if ( u8_MODE_State       == MODE_BYTE_BTN_CMD  )
  {
    u8_WIFI_TX_A_Fast [10] = 0x4B; 
    // Младший полубайт параметр
    if  ( (u8_MODE_Param_1>=BTN_CMD_IDLE) && (u8_MODE_Param_1 <= BTN_CMD_NEXT_DBL ) )
    { 
      u8_WIFI_TX_A_Fast [11] = TxIncode[u8_MODE_Param_1];
    }
    else
       u8_WIFI_TX_A_Fast [11] = TxIncode[BTN_CMD_IDLE];
    //
    // Старший полубайт параметр          16                                               20
    if   ( (u8_MODE_Param_1>=BTN_CMD_NEXT_RELEASE) && (u8_MODE_Param_1 <= BTN_CMD_MODE_RELEASE ) )
    {
       u8_WIFI_TX_A_Fast [12] = TxIncode[u8_MODE_Param_1-16];  
    }
    else
       u8_WIFI_TX_A_Fast [12] = TxIncode[u8_MODE_Param_1-16];
    //
    /*
      #define BTN_CMD_NEXT_RELEASE 16  
     
      // Это для второго полубайта
      #define BTN_CMD_MODE_CLICK   17  
      #define BTN_CMD_MODE_PRESS   18  
      #define BTN_CMD_MODE_DBL     19  
      #define BTN_CMD_MODE_RELEASE 20                             
    */
     // После отправки параметра - выполняем его установку в начальное состояние.
     u8_MODE_State   = MODE_BYTE_IDLE              ;
     u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ; // младшая половина параметра
     u8_MODE_Param_2 = MODE_PARAM_HIGH_NIBBLE_IDLE ; // старшая половина параметра
  }
  //
  // Первый полубайт для режима передачи яркости
  if ( u8_MODE_State       == MODE_BYTE_BRT_CMD  )
  {
    u8_WIFI_TX_A_Fast [10] = 0x5A; 
    // Младший полубайт параметр
    //                                0                                    5    
    if  ( (u8_MODE_Param_1>= BRT_CMD_IDLE ) && (u8_MODE_Param_1 <= BRT_CMD_W ) )
    { 
      /* ошибка
      if (u8_MODE_Param_1>10) u8_MODE_Param_1 = 10;
      // Диапазон для  u8_MODE_Param_1 от 0 до 10 что соответствует 0, 10, .. 100% яркости. */
      // правильно
      if (u8_MODE_Param_2>10) u8_MODE_Param_2 = 10;
      // Диапазон для  u8_MODE_Param_2 от 0 до 10 что соответствует 0, 10, .. 100% яркости.      
      //
      u8_WIFI_TX_A_Fast [11] = TxIncode[u8_MODE_Param_1];
    }  
    else
       u8_WIFI_TX_A_Fast [11] = TxIncode[BTN_CMD_IDLE];
    //
    u8_WIFI_TX_A_Fast [12] = TxIncode[u8_MODE_Param_2];
/*
    // Старший полубайт параметр          16                                               20
    if   ( (u8_MODE_Param_1>=BTN_CMD_NEXT_RELEASE) && (u8_MODE_Param_1 <= BTN_CMD_MODE_RELEASE ) )
    {
       u8_WIFI_TX_A_Fast [12] = TxIncode[u8_MODE_Param_1-16];  
    }
    else
       u8_WIFI_TX_A_Fast [12] = TxIncode[u8_MODE_Param_1-16];
    //
  
      #define BTN_CMD_NEXT_RELEASE 16  
     
      // Это для второго полубайта
      #define BTN_CMD_MODE_CLICK   17  
      #define BTN_CMD_MODE_PRESS   18  
      #define BTN_CMD_MODE_DBL     19  
      #define BTN_CMD_MODE_RELEASE 20                             
    */
     // После отправки параметра - выполняем его установку в начальное состояние.
     u8_MODE_State   = BRT_CMD_IDLE              ;
     u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ;  // младшая половина параметра
     u8_MODE_Param_2 = MODE_PARAM_HIGH_NIBBLE_IDLE ; // старшая половина параметра
  }
  //
  // Первый полубайт для режима передачи режима работы устройства
  if ( u8_MODE_State       == MODE_BYTE_OP_MOD_CMD  )
  {
    u8_WIFI_TX_A_Fast [10] = 0x69; 
    // Младший полубайт параметр
    //                                0                               2    
    if  ( (u8_MODE_Param_1>= OP_MODE_CMD_IDLE ) && (u8_MODE_Param_1 <= OP_MODE_CMD_SERVICE ) )
    { 
      if (u8_MODE_Param_2>ESP32_NET_ROLE_UNDEF) u8_MODE_Param_2 = ESP32_NET_ROLE_UNDEF;
      // 
      u8_WIFI_TX_A_Fast [11] = TxIncode[u8_MODE_Param_1];
    }  
    else
       u8_WIFI_TX_A_Fast [11] = TxIncode[BTN_CMD_IDLE];
    //
    u8_WIFI_TX_A_Fast [12] = TxIncode[u8_MODE_Param_2];
    //
    // После отправки параметра - выполняем его установку в начальное состояние.
    u8_MODE_State   = BRT_CMD_IDLE              ;
    u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ;  // младшая половина параметра
    u8_MODE_Param_2 = MODE_PARAM_HIGH_NIBBLE_IDLE ; // старшая половина параметра
  }
  //
  //
  //
  u8_WIFI_TX_A_Fast [13] = 0xFE;
  u8_WIFI_TX_A_Fast [14] = 0xFF;
  //
  //  
  u16_WIFI_TX_ActualSend_A_Fast = 1+1+8+3+1+1;  // 12+3=15 байт
  u16_WIFI_TX_SendCounter_A_Fast = 0; 
  if (b_WIFI_TX_Ready_A_Fast == false) 
    b_WIFI_TX_Ready_A_Fast         = true;  
  //
  u8_LED5_State = LED_5_DEFAULT;
  //
  if (u8_LED7_State == LED_7_KBD_PRESSED ) // Если был запрос на отправку пресса, то 
    u8_LED7_State = LED_7_KBD_WAIT_FOR_PRESS_OFF; // сигнализируем что отправили путем перехода в данное
    else
  u8_LED7_State         = LED_7_DEFAULT;
  //
  //
  b_LEDs_CSP_service_BUSY = false;
  //
  if (  ( b_IR_Command           == true )
        &&
        ( u8_WIFI_TX_A_Fast [10] !=  0x3C) 
        )
  {
    asm ("nop");
  }
}
//
    
//
//
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
