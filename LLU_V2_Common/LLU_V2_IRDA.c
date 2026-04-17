 /**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput/Src/main.c
  * @author  
  * @version V1.0.0
  * @date    17-xxxx-2023
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
#include "LLU_V2_IRDA.h"
#include "LLU_V2_ESP32.h"

#include "LLU_V2_LightControl.h"

#include "stdbool.h"   
#include "string.h" 
#include <stdio.h>
#include <stdlib.h>

extern uint16_t u16_Mode_Strob_Counter_ms ;
extern uint16_t u16_Mode_Strob_SubCounter ;


extern  uint8_t u8_Mode_Strobe_SM;
extern  uint8_t u8_ManualButton ;

extern uint16_t   RED_MAX_INDEX  ;
extern uint16_t GREEN_MAX_INDEX  ;
extern uint16_t  BLUE_MAX_INDEX  ;
extern uint16_t    UV_MAX_INDEX  ;
extern uint16_t WHITE_MAX_INDEX  ;

extern uint8_t u8_desired_R ; extern uint16_t u16_LightLevelRed_Desired;    extern  char c_desired_R[4];  
extern uint8_t u8_desired_G ; extern uint16_t u16_LightLevelGreen_Desired;  extern  char c_desired_G[4];  
extern uint8_t u8_desired_B ; extern uint16_t u16_LightLevelBlue_Desired;   extern  char c_desired_B[4];  

extern uint8_t u8_desired_W ; extern float f_desired_W ; extern  uint16_t  u16_LightLevelWhite_Desired;  extern   char c_desired_W[5]; 
extern bool b_UpdateW ;
//
extern uint8_t u8_desired_U ; extern float f_desired_U ; extern  uint16_t  u16_LightLevelUv_Desired; extern char c_desired_U[5];   
extern bool b_UpdateU ;

extern uint8_t u8_StateMaschine ;

/* REDUCE 
extern uint8_t u8_desired_R ; extern char c_desired_R[4]; extern uint16_t u16_LightLevelRed_Desired;  
extern uint8_t u8_desired_G ; extern char c_desired_G[4]; extern uint16_t u16_LightLevelGreen_Desired;  
extern uint8_t u8_desired_B ; extern char c_desired_B[4]; extern uint16_t  u16_LightLevelBlue_Desired;  
 REDUCE */
extern bool b_UpdateRGB ;



extern uint8_t u8_SetSpeedValue ;  //char c_SetSpeedValue[4];// uint16_t  u16_SetSpeedValue;  
extern bool b_UpdateSetSpeed ;
//uint16_t AutoModeActualSpeed = 50; // выражено в %
 
extern  uint32_t TimOutClock ;

// Для идентификации канала влияния при приеме сообщения SetLightLevel]
extern uint8_t u8_Actual_Wht_Uv_Selector ;
//
// режим синхронизации устройств
extern uint8_t u8_SyncModeSelector ;
//
// Общая задержка для работы в автоматическом режиме
extern uint8_t u8_UpdateSet;
//

extern uint8_t u8_ManualChannel;

extern int16_t  i16_LightLevelRed_AM   ; // ~10-20 % from RED_MAX_INDEX
extern int16_t  i16_LightLevelGreen_AM ; // ~10-20 % 
extern int16_t  i16_LightLevelBlue_AM  ; // ~10-20 %
extern int16_t  i16_LightLevelUv_AM    ; // ~10-20 %
extern int16_t  i16_LightLevelWhite_AM ; // ~10-20 %

extern uint8_t u8_Mode_Sqnt_Status  ;
extern uint32_t u32_Mode_Sqnt_SM      ; 

extern uint8_t u8_ProcessWiFiCommand ;

// Переменные хранения данных
uint16_t u16_IR_TX_Red   ;
uint16_t u16_IR_TX_Green ;
uint16_t u16_IR_TX_Blue  ;
//
uint16_t u16_IR_TX_White  ;
uint16_t u16_IR_TX_Uv  ;
//
uint16_t u16_IR_TX_ChangeSpeed ;
//
// Количество тактов для ИК пакета, исходя из рассчетов составит:
// преамбула - 4 такта
//  на 1 байт 2*8 =16 элементов.
// в пакете 2*Адрес, 2*тип, 4*Красный, 4*зеленый 4*синий 2*сумма
// Итого 4+16*( 2+2+4+4+4+2) = 292 элемента
uint16_t u16_IR_Tx_Packet[300];
//
uint16_t u16_IR_TX_PhaseCounter = 0; // Для отсчета фазы отправки
uint16_t u16_IR_TX_SendCounter  = 0;  // Для отсчета такта в фазе
uint16_t u16_IR_TX_WaitCounter  = 0;
//
// Задающий генератор работает с частотой 76 кГц
// Длительность такта 1/76000 = 1,315789473684211e-5 ~ 13.158 мкс.
// 
// 3мс  0,003/1,315789473684211e-5= 228
// 1мс  0,001/1,315789473684211e-5=  76  
//                          456 152
#define IR_TX_6ms_DUTY_TICS ( 2*228       )
#define IR_TX_6ms_DT_MIN    ( 2*228 - 100 )
#define IR_TX_6ms_DT_MAX    ( 2*228 + 100 )
//
//
#define IR_TX_2ms_DUTY_TICS  (2*76)
#define IR_TX_2ms_DT_MIN  (2*76-50)
#define IR_TX_2ms_DT_MAX  (2*76+50)

//
// 250 мкс 19 тактов   7..38
// 500 мкс 38 тактов   28..48
// 750 мкс 57 тактов   39..80
// 1/76000* 50= 658 mkS
#define IR_TX__LOW_DUTY_TICS 50
#define IR_TX__LOW_DT_MIN    12
#define IR_TX__LOW_DT_MAX    90
// 1/76000*150=0,001973 = 2000 mkS
#define IR_TX_HIGH_DUTY_TICS 150
#define IR_TX_HIGH_DT_MIN    91
#define IR_TX_HIGH_DT_MAX    200
// Время передачи бита - не более 2,7mS
// Время передачи байта - не более 22 mS.
//
// Передается пара 50-50 для нуля и 50-150 для единицы
// ... допустима девиация 
#define IR_SYNC_A_MIN  (IR_TX__LOW_DT_MIN)
#define IR_SYNC_A_MAX  (IR_TX__LOW_DT_MAX)
//
// ... допустима девиация 
#define IR_SYNC_B_MIN (IR_TX_HIGH_DT_MIN)
#define IR_SYNC_B_MAX (IR_TX_HIGH_DT_MAX)



uint16_t u16_IR_RX_captureCounter=0;
uint16_t u16_IR_RX_capture [1024];
uint32_t u32_IR_RX_ST_capture [1024];

uint16_t u16_captureActual=0;
uint32_t u32_ST_captureActual=0;

//
bool b_IR_RX_PacketReceived  = false ; 
bool b_IR_RX_PacketProcessed =  true ;
//
/* Private functions ---------------------------------------------------------*/

extern bool b_Send_ESP32_LED7_Packet ;
//
//
extern uint8_t u8_LED1_State;// = LED_1_DEFAULT; // Состояние коммуникац модуль
extern uint8_t u8_LED2_State;// = LED_2_DEFAULT; // Статус в иерархии
//
extern uint8_t u8_LED3_State;// = LED_3_DEFAULT; // Синхро по радио
extern uint8_t u8_LED4_State;// = LED_4_DEFAULT; // Синхро по ИК
extern uint8_t u8_LED5_State;// = LED_5_DEFAULT; // ИК пульта
//
extern uint8_t u8_LED6_State;// = LED_6_DEFAULT; // Батарейка
extern uint8_t u8_LED7_State;// = LED_7_DEFAULT; // Встроенная клава
extern uint8_t u8_LED8_State;// = LED_8_DEFAULT; // Качество синхро
//
// для оценки факта работы в группе и последующей блокировки и исполнения команд с пульта.
extern bool b_Grouped ;


  // 13мкс это (1/38000)/2= 13,2мкс
  // 40*13  =  520 мкс = 0,5  мс
  // 120*13 = 1560 мкс = 1,56 мс
  // Очевидна скважность 1/3.
  // Пауза 3000 это 3000*13= 39 мс

//
uint8_t u8_IR_Byte_0; // Block 1
uint8_t u8_IR_Byte_1; // Block 2
uint8_t u8_IR_Byte_2; // Block 3
uint8_t u8_IR_Byte_3; // Block 4
uint8_t u8_IR_Byte_4; // Block 5
//


  // 13мкс это (1/38000)/2= 13,2мкс
  // 40*13  =  520 мкс = 0,5  мс
  // 120*13 = 1560 мкс = 1,56 мс
  // Очевидна скважность 1/3.
  // Пауза 3000 это 3000*13= 39 мс

// 9мс / 0,0132= 9000 / 13,2= 681
// 4,5мс / 0,0132= 4500 / 13,2= 340
// 560 мкс / 13,2 = 42
// 2250- 560 = 1690  1690/13,2= 128
// ЕСЛИ ЕСТЬ КОЭФИЦИЕНТ /2, то это частота 38 кГц.
// Фактически, получается что 16 кГц, 
// так как длительности положительного и отрицательного фронта преамбулы 340 и 170
// и получаем видимые под отладчиком 21 и 64 такта
// Итого, 9000/340= 26 мкс 
// 40 тактов - около 1 мс
//
#define _9ms_L ( 340 - 40 )
#define _9ms_H ( 340 + 40 )
//
#define _4p5ms_L ( 170 - 40 )
#define _4p5ms_H ( 170 + 40 )
//
#define _FRNT_L ( 5 )
#define _FRNT_H ( 35 )
//
#define _BACK_L ( 36 )
#define _BACK_H ( 80 )
//


bool IR_GetCode_V2(void)
{
  //
  u8_IR_Byte_0 = 0;
  u8_IR_Byte_1 = 0;
  u8_IR_Byte_2 = 0;
  u8_IR_Byte_3 = 0;
  u8_IR_Byte_4 = 0; 
  //
  //
  // Preamble                     630/2                         740/2
  if( ( u16_IR_RX_capture [ 1] < _9ms_L   ) && ( u16_IR_RX_capture [ 1] > _9ms_H   ) ) // Sig. 1  9   ms rising edge
    return false; 
  if( ( u16_IR_RX_capture [ 2] < _4p5ms_L ) && ( u16_IR_RX_capture [ 2] > _4p5ms_H ) ) // Sig. 0  4,5 ms falling edge
    return false; 
  //
  // BLOCK 1 u8_IR_Byte_0
  // Bit_0
  if(  ( u16_IR_RX_capture [ 3] >  _FRNT_L ) && ( u16_IR_RX_capture [ 3] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [ 4] >  _FRNT_L ) && ( u16_IR_RX_capture [ 4] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [ 4] >  _BACK_L ) && ( u16_IR_RX_capture [ 4] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x01; // Log 1.
    }
  }
  // Bit_1        
  if(  ( u16_IR_RX_capture [ 5] >  _FRNT_L ) && ( u16_IR_RX_capture [ 5] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [ 6] >  _FRNT_L ) && ( u16_IR_RX_capture [ 6] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [ 6] >  _BACK_L ) && ( u16_IR_RX_capture [ 6] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x02; // Log 1.
    }
  }
  // Bit_2         
  if(  ( u16_IR_RX_capture [ 7] >  _FRNT_L ) && ( u16_IR_RX_capture [ 7] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [ 8] >  _FRNT_L ) && ( u16_IR_RX_capture [ 8] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [ 8] > _BACK_L ) && ( u16_IR_RX_capture [ 8] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x04; // Log 1.
    }
  }
  
  // Bit_3       
  if(  ( u16_IR_RX_capture [ 9] >  _FRNT_L ) && ( u16_IR_RX_capture [ 9] <  _FRNT_H ) ) // Sig. 1
  {
  /*  if(  ( u16_IR_RX_capture [10] >  _FRNT_L ) && ( u16_IR_RX_capture [10] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [10] > _BACK_L ) && ( u16_IR_RX_capture [10] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x08; // Log 1.
    }
  }
  // Bit_4       
  if(  ( u16_IR_RX_capture [11] >  _FRNT_L ) && ( u16_IR_RX_capture [11] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [12] >  _FRNT_L ) && ( u16_IR_RX_capture [12] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [12] > _BACK_L ) && ( u16_IR_RX_capture [12] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x10; // Log 1.
    }
  }
  // Bit_5       
  if(  ( u16_IR_RX_capture [13] >  _FRNT_L ) && ( u16_IR_RX_capture [13] <  _FRNT_H ) )// Sig. 1
  {
  /*  if(  ( u16_IR_RX_capture [14] >  _FRNT_L ) && ( u16_IR_RX_capture [14] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [14] > _BACK_L ) && ( u16_IR_RX_capture [14] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x20; // Log 1.
    }
  }
  // Bit_6       
  if(  ( u16_IR_RX_capture [15] >  _FRNT_L ) && ( u16_IR_RX_capture [15] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [16] >  _FRNT_L ) && ( u16_IR_RX_capture [16] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [16] > _BACK_L ) && ( u16_IR_RX_capture [16] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x40; // Log 1.
    }
  }
  // Bit_7       
  if(  ( u16_IR_RX_capture [17] >  _FRNT_L ) && ( u16_IR_RX_capture [17] <  _FRNT_H ) )// Sig. 1
  {
  /*  if(  ( u16_IR_RX_capture [18] >  _FRNT_L ) && ( u16_IR_RX_capture [18] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [18] > _BACK_L ) && ( u16_IR_RX_capture [18] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x80; // Log 1.
    }
  }
  //
  // BLOCK 2 u8_IR_Byte_1
  // Bit_0    
  if(  ( u16_IR_RX_capture [19] >  _FRNT_L ) && ( u16_IR_RX_capture [19] <  _FRNT_H ) )// Sig. 1
  {
  /*  if(  ( u16_IR_RX_capture [20] >  _FRNT_L ) && ( u16_IR_RX_capture [20] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [20] > _BACK_L ) && ( u16_IR_RX_capture [20] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x01; // Log 1.
    }
 
  }
  // Bit_1         
  if(  ( u16_IR_RX_capture [21] >  _FRNT_L ) && ( u16_IR_RX_capture [21] <  _FRNT_H ) )// Sig. 1
  {
  /*  if(  ( u16_IR_RX_capture [22] >  _FRNT_L ) && ( u16_IR_RX_capture [22] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [22] > _BACK_L ) && ( u16_IR_RX_capture [22] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x02; // Log 1.
    }
        
  }
  // Bit_2        
  if ( ( u16_IR_RX_capture [23] >  _FRNT_L ) && ( u16_IR_RX_capture [23] <  _FRNT_H ) )// Sig. 1
  {
  /*  if(  ( u16_IR_RX_capture [24] >  _FRNT_L ) && ( u16_IR_RX_capture [24] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [24] > _BACK_L ) && ( u16_IR_RX_capture [24] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x04; // Log 1.
    }
     
  }
  // Bit_3        
  if ( ( u16_IR_RX_capture [25] >  _FRNT_L ) && ( u16_IR_RX_capture [25] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [26] >  _FRNT_L ) && ( u16_IR_RX_capture [26] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [26] > _BACK_L ) && ( u16_IR_RX_capture [26] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x08; // Log 1.
    }
       
  }
  // Bit_4         
  if ( ( u16_IR_RX_capture [27] >  _FRNT_L ) && ( u16_IR_RX_capture [27] <  _FRNT_H ) )// Sig. 1
  {
/*    if(  ( u16_IR_RX_capture [28] >  _FRNT_L ) && ( u16_IR_RX_capture [28] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [28] > _BACK_L ) && ( u16_IR_RX_capture [28] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x10; // Log 1.
    }
       
  }
  // Bit_5        
  if ( ( u16_IR_RX_capture [29] >  _FRNT_L ) && ( u16_IR_RX_capture [29] <  _FRNT_H ) )// Sig. 1
  {
  /*  if(  ( u16_IR_RX_capture [30] >  _FRNT_L ) && ( u16_IR_RX_capture [30] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [30] > _BACK_L ) && ( u16_IR_RX_capture [30] < _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x20; // Log 1.
    }
       
  }
  // Bit_6        
  if ( ( u16_IR_RX_capture [31] >  _FRNT_L ) && ( u16_IR_RX_capture [31] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [32] >  _FRNT_L ) && ( u16_IR_RX_capture [32] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [32] > _BACK_L ) && ( u16_IR_RX_capture [32] < _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x40; // Log 1.
    }
      
  }
  // Bit_7        
  if ( ( u16_IR_RX_capture [33] >  _FRNT_L ) && ( u16_IR_RX_capture [33] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [34] >  _FRNT_L ) && ( u16_IR_RX_capture [34] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [34] > _BACK_L ) && ( u16_IR_RX_capture [34] < _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_1 = u8_IR_Byte_1+0x80; // Log 1.
    }
        
  }
  //
  // BLOCK 3 u8_IR_Byte_2
  // Bit_0         
  if ( ( u16_IR_RX_capture [35] >  _FRNT_L ) && ( u16_IR_RX_capture [35] <  _FRNT_H ) )// Sig. 1
  {
 /*   if( ( u16_IR_RX_capture [36] >  _FRNT_L ) && ( u16_IR_RX_capture [36] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [36] > _BACK_L ) && ( u16_IR_RX_capture [36] < _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x01; // Log 1.
    }
       
  }
  // Bit_1         
  if ( ( u16_IR_RX_capture [37] >  _FRNT_L ) && ( u16_IR_RX_capture [37] <  _FRNT_H ) )// Sig. 1
  {
 /*   if(  ( u16_IR_RX_capture [38] >  _FRNT_L ) && ( u16_IR_RX_capture [38] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if(  ( u16_IR_RX_capture [38] > _BACK_L ) && ( u16_IR_RX_capture [38] < _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x02; // Log 1.
    }
  
  }
  // Bit_2         
  if ( ( u16_IR_RX_capture [39] >  _FRNT_L ) && ( u16_IR_RX_capture [39] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [40] >  _FRNT_L ) && ( u16_IR_RX_capture [40] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [40] > _BACK_L ) && ( u16_IR_RX_capture [40] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x04; // Log 1.
    }
  
  }
  // Bit_3       
  if ( ( u16_IR_RX_capture [41] >  _FRNT_L  ) && ( u16_IR_RX_capture [41] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [42] >  _FRNT_L ) && ( u16_IR_RX_capture [42] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [42] > _BACK_L ) && ( u16_IR_RX_capture [42] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x08; // Log 1.
    }
  
  }
  // Bit_4         
  if ( ( u16_IR_RX_capture [43] >  _FRNT_L  ) && ( u16_IR_RX_capture [43] <  _FRNT_H ) )// Sig. 1
  {
 /*   if( ( u16_IR_RX_capture [44] >  _FRNT_L ) && ( u16_IR_RX_capture [44] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [44] > _BACK_L ) && ( u16_IR_RX_capture [44] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x10; // Log 1.
    }
 
  }
  // Bit_5        
  if ( ( u16_IR_RX_capture [45] >  _FRNT_L  ) && ( u16_IR_RX_capture [45] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [46] >  _FRNT_L ) && ( u16_IR_RX_capture [46] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [46] > _BACK_L ) && ( u16_IR_RX_capture [46] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x20; // Log 1.
    }
 
  }
  // Bit_6         
  if ( ( u16_IR_RX_capture [47] >  _FRNT_L  ) && ( u16_IR_RX_capture [47] <  _FRNT_H ) )// Sig. 1
  {
 /*   if( ( u16_IR_RX_capture [48] >  _FRNT_L ) && ( u16_IR_RX_capture [48] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [48] > _BACK_L ) && ( u16_IR_RX_capture [48] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x40; // Log 1.
    }
 
  }
  // Bit_7         
  if ( ( u16_IR_RX_capture [49] >  _FRNT_L  ) && ( u16_IR_RX_capture [49] <  _FRNT_H ) )// Sig. 1
  {
   /* if( ( u16_IR_RX_capture [50] >  _FRNT_L ) && ( u16_IR_RX_capture [50] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [50] > _BACK_L ) && ( u16_IR_RX_capture [50] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_2 = u8_IR_Byte_2+0x80; // Log 1.
    }

  }
  //
  // BLOCK 4 u8_IR_Byte_3
  // Bit_0          
  if ( ( u16_IR_RX_capture [51] >  _FRNT_L  ) && ( u16_IR_RX_capture [51] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [52] >  _FRNT_L ) && ( u16_IR_RX_capture [52] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [52] > _BACK_L ) && ( u16_IR_RX_capture [52] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3+0x01; // Log 1.
    }
 
  }
  // Bit_1          
  if ( ( u16_IR_RX_capture [53] >  _FRNT_L  ) && ( u16_IR_RX_capture [53] <   _FRNT_H) )// Sig. 1
  {
 /*   if( ( u16_IR_RX_capture [54] >  _FRNT_L ) && ( u16_IR_RX_capture [54] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [54] > _BACK_L ) && ( u16_IR_RX_capture [54] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3+0x02; // Log 1.
    }
 
  }
  // Bit_2          
  if ( ( u16_IR_RX_capture [55] >  _FRNT_L  ) && ( u16_IR_RX_capture [55] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [56] >  _FRNT_L ) && ( u16_IR_RX_capture [56] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [56] > _BACK_L ) && ( u16_IR_RX_capture [56] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3+0x04; // Log 1.
    }

  }
  // Bit_3          
  if ( ( u16_IR_RX_capture [57] >  _FRNT_L  ) && ( u16_IR_RX_capture [57] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [58] >  _FRNT_L ) && ( u16_IR_RX_capture [58] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [58] > _BACK_L ) && ( u16_IR_RX_capture [58] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3+0x08; // Log 1.
    }
 
  }
  // Bit_4          
  if ( ( u16_IR_RX_capture [59] >  _FRNT_L  ) && ( u16_IR_RX_capture [59] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [60] >  _FRNT_L ) && ( u16_IR_RX_capture [60] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [60] > _BACK_L ) && ( u16_IR_RX_capture [60] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3+0x10; // Log 1.
    }
 
  }
  // Bit_5          
  if ( ( u16_IR_RX_capture [61] >  _FRNT_L  ) && ( u16_IR_RX_capture [61] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [62] >  _FRNT_L ) && ( u16_IR_RX_capture [62] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [62] > _BACK_L ) && ( u16_IR_RX_capture [62] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3|0x20; // Log 1.
    }
  }
  // Bit_6         
  if ( ( u16_IR_RX_capture [63] >  _FRNT_L  ) && ( u16_IR_RX_capture [63] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [64] >  _FRNT_L ) && ( u16_IR_RX_capture [64] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [64] > _BACK_L ) && ( u16_IR_RX_capture [64] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3+0x40; // Log 1.
    }
  }
  // Bit_7          
  if ( ( u16_IR_RX_capture [65] >  _FRNT_L  ) && ( u16_IR_RX_capture [65] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [66] >  _FRNT_L ) && ( u16_IR_RX_capture [66] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [66] > _BACK_L ) && ( u16_IR_RX_capture [66] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_3 = u8_IR_Byte_3+0x80; // Log 1.
    }

  }
  //       
  // BLOCK 5  
  //       
  if ( ( u16_IR_RX_capture [67] >  _FRNT_L  ) && ( u16_IR_RX_capture [67] <  _FRNT_H ) )// Sig. 1
  {
  /*  if( ( u16_IR_RX_capture [68] >  _FRNT_L ) && ( u16_IR_RX_capture [68] <  _FRNT_H ) ) // Sig. 0
    {
      u8_IR_Byte_4 = u8_IR_Byte_4; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [68] > _BACK_L ) && ( u16_IR_RX_capture [68] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_4 = u8_IR_Byte_4+0x01; // Log 1.
    }
  }

  
  //       
  if ( ( u16_IR_RX_capture [69] > 680/2  ) && ( u16_IR_RX_capture [69] < 720/2 ) )// Sig. 1
  { 
  /*  if( ( u16_IR_RX_capture [70] >  _FRNT_L ) && ( u16_IR_RX_capture [70] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_4 = u8_IR_Byte_4; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [70] > _BACK_L ) && ( u16_IR_RX_capture [70] < _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_4 = u8_IR_Byte_4+0x02; // Log 1.
    }
  }

 
         
  if ( ( u16_IR_RX_capture [71] >  _FRNT_L  ) && ( u16_IR_RX_capture [71] <  _FRNT_H ) ) // Sig. 1
  { 
  /*  if( ( u16_IR_RX_capture [72] >  _FRNT_L ) && ( u16_IR_RX_capture [72] <  _FRNT_H ) )// Sig. 0
    {
      u8_IR_Byte_4 = u8_IR_Byte_4; // Log 0.
    }*/
    if( ( u16_IR_RX_capture [72] > _BACK_L ) && ( u16_IR_RX_capture [72] <  _BACK_H ) )// Sig. 0
    {
      u8_IR_Byte_4 = u8_IR_Byte_4+0x04; // Log 1.
    }  
  }
  else
    return false; // неверный формат Sigнальное единицы. //invalid signal unit format.
    //   ( u16_IR_RX_capture [72] > 160  ) || ( u16_IR_RX_capture [72] < 185 )
    //   ( u16_IR_RX_capture [73] >  30  ) || ( u16_IR_RX_capture [73] <  55 )
    //   ( u16_IR_RX_capture [74]==5000  ) 
        
 return true;  
  
}


void ProcessIRCode( void )
{
   // РЯД 1
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x05 ) && (u8_IR_Byte_3 == 0xFA ) ) { ProcessLightUp();  u8_LED5_State = LED_5_IR_KNOWN; }
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x04 ) && (u8_IR_Byte_3 == 0xFB ) ) { ProcessLightDown();u8_LED5_State = LED_5_IR_KNOWN; }
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x06 ) && (u8_IR_Byte_3 == 0xF9 ) ) { ProcessPowerDown();u8_LED5_State = LED_5_IR_KNOWN; }
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x07 ) && (u8_IR_Byte_3 == 0xF8 ) ) { ProcessPowerUp();  u8_LED5_State = LED_5_IR_KNOWN; }
  //
  // РЯД 2
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x09 ) && (u8_IR_Byte_3 == 0xF6 ) ) { ProcessRedButton();  u8_LED5_State = LED_5_IR_KNOWN; }
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x08 ) && (u8_IR_Byte_3 == 0xF7 ) ) { ProcessGreenButton();u8_LED5_State = LED_5_IR_KNOWN; }
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x0A ) && (u8_IR_Byte_3 == 0xF5 ) ) { ProcessBlueButton(); u8_LED5_State = LED_5_IR_KNOWN; }
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x0B ) && (u8_IR_Byte_3 == 0xF4 ) ) { ProcessWhiteButton();u8_LED5_State = LED_5_IR_KNOWN; }
  //
  //
  // РЯД 3
  /*  
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == ) && (u8_IR_Byte_3 == ) ) { }
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == ) && (u8_IR_Byte_3 == ) ) { }*/
  if (  (u8_IR_Byte_0 == 0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x0F ) && (u8_IR_Byte_3 == 0xF0 ) ) { ProcessUvButton(); u8_LED5_State = LED_5_IR_KNOWN; }
  //
  //
  // РЯД 4
  /*
  if (  (u8_IR_Byte_0 == ) && (u8_IR_Byte_1 == ) && (u8_IR_Byte_2 == ) && (u8_IR_Byte_3 == ) ) { }
  if (  (u8_IR_Byte_0 == ) && (u8_IR_Byte_1 == ) && (u8_IR_Byte_2 == ) && (u8_IR_Byte_3 == ) ) { }*/
  if (  (u8_IR_Byte_0 ==  0x00 ) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 ==0x17 ) && (u8_IR_Byte_3 == 0xE8 ) ) 
  {  Process_Mode_Stroboscope_Button(); u8_LED5_State = LED_5_IR_KNOWN;    } // Верхняя S кнопка

  //
  // РЯД 5
  /*
  if (  (u8_IR_Byte_0 == ) && (u8_IR_Byte_1 == ) && (u8_IR_Byte_2 == ) && (u8_IR_Byte_3 == ) ) { }
  if (  (u8_IR_Byte_0 == ) && (u8_IR_Byte_1 == ) && (u8_IR_Byte_2 == ) && (u8_IR_Byte_3 == ) ) { }*/
  if (  (u8_IR_Byte_0 == 0x00) && (u8_IR_Byte_1 == 0xFF ) && (u8_IR_Byte_2 == 0x1B ) && (u8_IR_Byte_3 == 0xE4) )
  {  Process_Mode_Sequential_Button();  u8_LED5_State = LED_5_IR_KNOWN; }// Нижняя S кнопка
  //
  // РЯД 6
  /*
  if (  (u8_IR_Byte_0 == ) && (u8_IR_Byte_1 == ) && (u8_IR_Byte_2 == ) && (u8_IR_Byte_3 == ) ) { }*/
  if (  (u8_IR_Byte_0==0x00) && (u8_IR_Byte_1==0xFF) && (u8_IR_Byte_2== 0x12) && (u8_IR_Byte_3==0xED) ) 
  { Process_Mode_Manual_Button();      u8_LED5_State = LED_5_IR_KNOWN;  } // Сереневая кнопка
  if (  (u8_IR_Byte_0==0x00) && (u8_IR_Byte_1==0xFF) && (u8_IR_Byte_2== 0x13) && (u8_IR_Byte_3==0xEC) ) 
  { Process_Mode_Rainbow_Button();     u8_LED5_State = LED_5_IR_KNOWN;  } // F кнопка
  //   
  //
}


//
uint8_t u8_Address;      // Адрес   ( нормально )    
uint8_t u8_Address_Inv ; // Адрес   (  инверсно )
// 
uint8_t u8_Type;         // Тип     ( нормально )               
uint8_t u8_Type_Inv;     // Тип     (  инверсно )       
//
uint16_t u16_Red_L     ; // Красный Младшая( нормально )                
uint16_t u16_Red_L_Inv ; // Красный Младшая(  инверсно )     
uint16_t u16_Red_H     ; // Красный Старшая( нормально )                 
uint16_t u16_Red_H_Inv ; // Красный Старшая(  инверсно )             
//
uint16_t u16_Green_L     ; // Зеленый Младшая( нормально )                
uint16_t u16_Green_L_Inv ; // Зеленый Младшая(  инверсно )     
uint16_t u16_Green_H     ; // Зеленый Старшая( нормально )                 
uint16_t u16_Green_H_Inv ; // Зеленый Старшая(  инверсно )  
//
uint16_t u16_Blue_L     ; // Синий Младшая( нормально )                
uint16_t u16_Blue_L_Inv ; // Синий Младшая(  инверсно )     
uint16_t u16_Blue_H     ; // Синий Старшая( нормально )                 
uint16_t u16_Blue_H_Inv ; // Синий Старшая(  инверсно ) 
//
//
// Для света (Белого или УФ) и для скорости
uint16_t u16_Param_L     ; // Свет Младшая( нормально )                
uint16_t u16_Param_L_Inv ; // Свет Младшая(  инверсно )     
uint16_t u16_Param_H     ; // Свет Старшая( нормально )                 
uint16_t u16_Param_H_Inv ; // Свет Старшая(  инверсно ) 
//
//
//
uint8_t u8_Sum        ; // Сумма   ( нормально )
uint8_t u8_Sum_Inv    ; // Сумма   (  инверсно )            
//
//
uint16_t u16_BitCounter = 0;  
//
/* // IR_SYNC 
bool IR_SyncRGB_GetCode ( void )
{
  // Оцениваем 292 периода сигнала
  // Нужно выделить закодированные байты с данными о яркости RGB каналов
  // 
  // Преамбула
  // 3 мс - Включение
  // IR_TX_6ms_DUTY_TICS;
  // 3 мс - выключение
  // IR_TX_6ms_DUTY_TICS;
  // 1 мс - Включение
  // IR_TX_2ms_DUTY_TICS;
  // 1 мс - выключение
  // IR_TX_2ms_DUTY_TICS;
  //
  u16_BitCounter=1;
   // Преамбула
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX) ) // СИГ. 1  3 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX ) ) // СИГ. 0  3 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //  
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 1  1 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 0  1 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //
  //
  u8_Address     = u8_GetByteFrom_IR_RX_capture(); // Адрес   ( нормально )    
  u8_Address_Inv = u8_GetByteFrom_IR_RX_capture(); // Адрес   (  инверсно )
  // 
  u8_Type       = u8_GetByteFrom_IR_RX_capture();  // Тип     ( нормально )               
  u8_Type_Inv   = u8_GetByteFrom_IR_RX_capture();  // Тип     (  инверсно )       
  //
  u16_Red_L     = u8_GetByteFrom_IR_RX_capture();  // Красный Младшая( нормально )                
  u16_Red_L_Inv = u8_GetByteFrom_IR_RX_capture();  // Красный Младшая(  инверсно )     
  u16_Red_H     = u8_GetByteFrom_IR_RX_capture();  // Красный Старшая( нормально )                 
  u16_Red_H_Inv = u8_GetByteFrom_IR_RX_capture();  // Красный Старшая(  инверсно )             
  //
  u16_Green_L     = u8_GetByteFrom_IR_RX_capture(); // Зеленый Младшая( нормально )                
  u16_Green_L_Inv = u8_GetByteFrom_IR_RX_capture(); // Зеленый Младшая(  инверсно )     
  u16_Green_H     = u8_GetByteFrom_IR_RX_capture(); // Зеленый Старшая( нормально )                 
  u16_Green_H_Inv = u8_GetByteFrom_IR_RX_capture(); // Зеленый Старшая(  инверсно )  
  //
  u16_Blue_L     = u8_GetByteFrom_IR_RX_capture(); // Синий Младшая( нормально )                
  u16_Blue_L_Inv = u8_GetByteFrom_IR_RX_capture(); // Синий Младшая(  инверсно )     
  u16_Blue_H     = u8_GetByteFrom_IR_RX_capture(); // Синий Старшая( нормально )                 
  u16_Blue_H_Inv = u8_GetByteFrom_IR_RX_capture(); // Синий Старшая(  инверсно )  
  //       
  u8_Sum        = u8_GetByteFrom_IR_RX_capture(); // Сумма   ( нормально )
  u8_Sum_Inv    = u8_GetByteFrom_IR_RX_capture(); // Сумма   (  инверсно )            
  //
  if ( (u8_Address== (u8_Address_Inv^0xFF   )  )&&
       //
       (u8_Type   == (u8_Type_Inv^0xFF          )  )&&
       // 
       (u16_Red_L== (u16_Red_L_Inv^0xFF     )  )&&
       (u16_Red_H== (u16_Red_H_Inv^0xFF     )  )&&
       //
       (u16_Green_L== (u16_Green_L_Inv^0xFF )  )&&
       (u16_Green_H== (u16_Green_H_Inv^0xFF )  )&&
       // 
       (u16_Blue_L== (u16_Blue_L_Inv^0xFF   )  )&&
       (u16_Blue_H== (u16_Blue_H_Inv^0xFF   )  )        )
  return true;
        else
          return false;
}
*/
//
//
uint8_t u8_GetByteFrom_IR_RX_capture(void)
{ /*
  // Передается пара 19-19 для нуля и 19-57 для единицы
  // для 19 допустима девиация 6-29
  #define IR_SYNC_A_MIN  6
  #define IR_SYNC_A_MAX 39
  //
  // для 57 допустима девиация 40-80  
  #define IR_SYNC_B_MIN 40
  #define IR_SYNC_B_MAX 80
  */
  //
  static uint8_t u8_IR_Byte_0;
  //
  u8_IR_Byte_0 = 0;
  // Bit_0
  if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x01; // Лог 1.
    }
  }
  u16_BitCounter++;
  //
  // Bit_1        
  if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x02; // Лог 1.
    }
  }
  u16_BitCounter++;
  //
  // Bit_2         
  if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x04; // Лог 1.
    }
  }
  u16_BitCounter++;
  //
  // Bit_3       
  if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x08; // Лог 1.
    }
  }
  u16_BitCounter++;
  //
  // Bit_4       
   if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x10; // Лог 1.
    }
  }
  u16_BitCounter++;
  //
  // Bit_5       
  if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x20; // Лог 1.
    }
  }
  u16_BitCounter++;
  //
  // Bit_6       
  if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x40; // Лог 1.
    }
  }
  u16_BitCounter++;
  //
  // Bit_7       
  if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 1
  {
    u16_BitCounter++;
    if( ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_A_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_A_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0; // Лог 0.
    }
    if(  ( u16_IR_RX_capture [ u16_BitCounter] >=  IR_SYNC_B_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] <=  IR_SYNC_B_MAX ) )// СИГ. 0
    {
      u8_IR_Byte_0 = u8_IR_Byte_0+0x80; // Лог 1.
    }
  }
  //
  //       !!!! ++  !!!!
  u16_BitCounter++;
  //
  return u8_IR_Byte_0;
}
//
//
// // IR_SYNC 
/*
void Process_IR_SyncRGB_Code( void )
{
  uint32_t u32_Temp;
  //
  //
  u8_desired_R = u16_Red_L   ;
  u8_desired_G = u16_Green_L ;
  u8_desired_B = u16_Blue_L  ;
  //  
  // Значения из диапазона 0-255 пересчитываем в значения диапазона 0-максимум канала
  u32_Temp = u8_desired_R *   ( RED_MAX_INDEX-1); u32_Temp = u32_Temp /255;   u16_LightLevelRed_Desired = u32_Temp;
  u32_Temp = u8_desired_G * ( GREEN_MAX_INDEX-1); u32_Temp = u32_Temp /255; u16_LightLevelGreen_Desired = u32_Temp;
  u32_Temp = u8_desired_B *  ( BLUE_MAX_INDEX-1); u32_Temp = u32_Temp /255;  u16_LightLevelBlue_Desired = u32_Temp;
  //
  u8_StateMaschine = SM_MODE_MANUAL;
  b_UpdateRGB = true; 
  //
}
*/
//
/* IR_SYNC_CONTROL 
bool IR_SyncSetParam_GetCode ( void )
{
  // Оцениваем 164 периода сигнала
  // Нужно выделить закодированные байты с данными о яркости Белого (или УФ) каналов
  // 
  // Преамбула
  // 3 мс - Включение
  // IR_TX_6ms_DUTY_TICS;
  // 3 мс - выключение
  // IR_TX_6ms_DUTY_TICS;
  // 1 мс - Включение
  // IR_TX_2ms_DUTY_TICS;
  // 1 мс - выключение
  // IR_TX_2ms_DUTY_TICS;
  //
  u16_BitCounter=1;
  // Преамбула
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX) ) // СИГ. 1  3 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX ) ) // СИГ. 0  3 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //  
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 1  1 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 0  1 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //
  //
  u8_Address     = u8_GetByteFrom_IR_RX_capture(); // Адрес   ( нормально )    
  u8_Address_Inv = u8_GetByteFrom_IR_RX_capture(); // Адрес   (  инверсно )
  // 
  u8_Type       = u8_GetByteFrom_IR_RX_capture();  // Тип     ( нормально )               
  u8_Type_Inv   = u8_GetByteFrom_IR_RX_capture();  // Тип     (  инверсно )       
  //
  u16_Param_L     = u8_GetByteFrom_IR_RX_capture();  // Красный Младшая( нормально )                
  u16_Param_L_Inv = u8_GetByteFrom_IR_RX_capture();  // Красный Младшая(  инверсно )     
  u16_Param_H     = u8_GetByteFrom_IR_RX_capture();  // Красный Старшая( нормально )                 
  u16_Param_H_Inv = u8_GetByteFrom_IR_RX_capture();  // Красный Старшая(  инверсно )             
  //   
  u8_Sum        = u8_GetByteFrom_IR_RX_capture(); // Сумма   ( нормально )
  u8_Sum_Inv    = u8_GetByteFrom_IR_RX_capture(); // Сумма   (  инверсно )            
  //
  if ( (u8_Address== (u8_Address_Inv^0xFF   )  )&&
       //
       (u8_Type   == (u8_Type_Inv^0xFF      )  )&&
       // 
       (u16_Param_L== (u16_Param_L_Inv^0xFF )  )&&
       (u16_Param_H== (u16_Param_H_Inv^0xFF )  )      )
    return true;
        else
    return false;
  //
}
*/
//
/* IR_SYNC_CONTROL 
void Process_IR_SyncSetParam_Code( void )
{
  uint32_t u32_Temp;
  //
  if ( u8_Type == IR_TX_WHITE_INFO )
  {
    u8_desired_W = u16_Param_L;
    //
    // Значения из диапазона 0-255 пересчитываем в значения диапазона 0-максимум канала
    u32_Temp = u8_desired_W * ( WHITE_MAX_INDEX-1); u32_Temp = u32_Temp /255; u16_LightLevelWhite_Desired = u32_Temp;
    //
    u8_StateMaschine = SM_MODE_MANUAL;
    b_UpdateW = true;      
  }
  //
  if ( u8_Type ==    IR_TX_UV_INFO )
  {
    u8_desired_U = u16_Param_L;
    //
    // Значения из диапазона 0-255 пересчитываем в значения диапазона 0-максимум канала
    u32_Temp = u8_desired_U * (    UV_MAX_INDEX-1); u32_Temp = u32_Temp /255; u16_LightLevelUv_Desired = u32_Temp;
    //
    u8_StateMaschine = SM_MODE_MANUAL;
    b_UpdateU = true; 
  }
  //
  if ( u8_Type ==    IR_TX_SET_CHANGE_SPEED )
  {
    // Это значения в диапазоне от 0 до 99
    u8_SetSpeedValue = u16_Param_L;
    //
    if (u8_SetSpeedValue < UPDATE_DELAY_MIN_MS)u8_SetSpeedValue = UPDATE_DELAY_MIN_MS;
    if (u8_SetSpeedValue > UPDATE_DELAY_MAX_MS)u8_SetSpeedValue = UPDATE_DELAY_MAX_MS;
    //
    //u8_StateMaschine = SM_MODE_MANUAL;
    b_UpdateSetSpeed = true; // AutoModeActualSpeed 
  }
}

//
//
bool IR_SyncSetMode_GetCode ( void )
{
  // Оцениваем 164 периода сигнала
  // Нужно выделить закодированные байты с данными о яркости Белого (или УФ) каналов
  // 
  // Преамбула
  // 6 мс - Включение
  // IR_TX_6ms_DUTY_TICS;
  // 6 мс - выключение
  // IR_TX_6ms_DUTY_TICS;
  // 2 мс - Включение
  // IR_TX_2ms_DUTY_TICS;
  // 2 мс - выключение
  // IR_TX_2ms_DUTY_TICS;
  //
  u16_BitCounter=1;
  // Преамбула
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX) ) // СИГ. 1  3 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX ) ) // СИГ. 0  3 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //  
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 1  1 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 0  1 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //
  //
  u8_Address     = u8_GetByteFrom_IR_RX_capture(); // Адрес   ( нормально )    
  u8_Address_Inv = u8_GetByteFrom_IR_RX_capture(); // Адрес   (  инверсно )
  // 
  u8_Type       = u8_GetByteFrom_IR_RX_capture();  // Тип     ( нормально )               
  u8_Type_Inv   = u8_GetByteFrom_IR_RX_capture();  // Тип     (  инверсно )       
  //          
  //   
  u8_Sum        = u8_GetByteFrom_IR_RX_capture(); // Сумма   ( нормально )
  u8_Sum_Inv    = u8_GetByteFrom_IR_RX_capture(); // Сумма   (  инверсно )            
  //
  if ( (u8_Address== (u8_Address_Inv^0xFF   )  )&&
       //
       (u8_Type   == (u8_Type_Inv^0xFF      )  )    )
    return true;
        else
    return false;
  //
}
*/
//
//
/* IR_SYNC_CONTROL
bool IR_SyncAutoMode_Check ( void )
{
  static uint8_t SuccesCounter=0;
  // Оцениваем 100 периода сигнала
  // Нужно выделить закодированные байты с данными о яркости Белого (или УФ) каналов
  // 
  // Преамбула
  // 6 мс - Включение
  // IR_TX_6ms_DUTY_TICS;
  // 6 мс - выключение
  // IR_TX_6ms_DUTY_TICS;
  // 2 мс - Включение
  // IR_TX_2ms_DUTY_TICS;
  // 2 мс - выключение
  // IR_TX_2ms_DUTY_TICS;
  //
  u16_BitCounter=1;
  // Преамбула
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX) ) // СИГ. 1  3 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_6ms_DT_MIN ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_6ms_DT_MAX ) ) // СИГ. 0  3 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //  
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 1  1 мс положительного фронта
    return false;           u16_BitCounter++;
  if( ( u16_IR_RX_capture [ u16_BitCounter] < IR_TX_2ms_DT_MIN  ) && ( u16_IR_RX_capture [ u16_BitCounter] > IR_TX_2ms_DT_MAX ) ) // СИГ. 0  1 мc отрицательного фронта
    return false;           u16_BitCounter++;
  //
  //
  u8_Address     = u8_GetByteFrom_IR_RX_capture(); // Адрес   ( нормально )    
  u8_Address_Inv = u8_GetByteFrom_IR_RX_capture(); // Адрес   (  инверсно )
  // 
  u8_Type       = u8_GetByteFrom_IR_RX_capture();  // Тип     ( нормально )               
  u8_Type_Inv   = u8_GetByteFrom_IR_RX_capture();  // Тип     (  инверсно )       
  //          
  //   
  u8_Sum        = u8_GetByteFrom_IR_RX_capture(); // Сумма   ( нормально )
  u8_Sum_Inv    = u8_GetByteFrom_IR_RX_capture(); // Сумма   (  инверсно )            
  //
  SuccesCounter=0;
  //
  if (u8_Address== (u8_Address_Inv^0xFF   ) )  SuccesCounter++;
  if (u8_Type   == (u8_Type_Inv^0xFF      ) )  SuccesCounter++;
  if (u8_Sum   == (u8_Sum_Inv^0xFF        ) )  SuccesCounter++;
  
  if ( SuccesCounter>=2   )
    return true;
        else
    return false;
  //
}
*/
//
/* IR_SYNC_CONTROL
{
  //
  if ( u8_Type == IR_TX_SET_ACTUAL_WHITE )
  {
    
  }
  //
  //
  if ( u8_Type == IR_TX_SET_ACTUAL_UV )
  {
    
  }
  //
  //
  if ( u8_Type == IR_TX_SET_MODE_MANUAL )
  {
    
  }
  //
  //
  if ( u8_Type == IR_TX_SET_MODE_AUTO )
  {
    
  }
  //
  //
  if ( u8_Type == IR_TX_SET_SYNC_INDEPENDENT )
  {
    
  }
  //
  //
  if ( u8_Type == IR_TX_SET_SYNC_GROUPED )
  {
    
  }
  //
}
*/
//
//
uint16_t u16_Crt_IR_TX_StepCounter=0;
//
void Update_IR_TX_Packet (uint8_t u8_Buf);
void Update_IR_TX_Packet (uint8_t u8_Buf)
{
   // bit_0
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;
    if ((u8_Buf&0x01) == 0x01) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;
    //
    // bit_1
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
    if ((u8_Buf&0x02) == 0x02) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                       
    //
    // bit_2
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
    if ((u8_Buf&0x04) == 0x04) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;
    //
    // bit_3
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
    if ((u8_Buf&0x08) == 0x08) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;
    //
    // bit_4
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
    if ((u8_Buf&0x10) == 0x10) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;
    //
    // bit_5
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
    if ((u8_Buf&0x20) == 0x20) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;
    //
    // bit_6
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
    if ((u8_Buf&0x40) == 0x40) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;
    //
    // bit_7
                               u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
    if ((u8_Buf&0x80) == 0x80) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
                          else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;    
}
// 
/* IR_SYNC_CONTROL
//  6+2+6+2 + 
void Create_IR_Tx_RGB_Packet( uint8_t  u8_Address,  // IR_TX_BROADBAND,
                              uint8_t  u8_Type,     // IR_TX_RGB_INFO,
                              uint16_t  u16_Red,     //
                              uint16_t  u16_Green,   //
                              uint16_t  u16_Blue   ) //
{
  //
  u16_Crt_IR_TX_StepCounter=0;
  //
  uint8_t u8_Buf;
  
  // Формируем блок задания работы формирователя огибающей.
  //
  // 
  // Преамбула
  // 6 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 6 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 2 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  // 2 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  //
  //
  //
  // Адрес   ( нормально )
  u8_Buf = u8_Address;       
  Update_IR_TX_Packet(u8_Buf);
  // Адрес   (  инверсно )
  u8_Buf = u8_Address^0xFF;  
  Update_IR_TX_Packet(u8_Buf);
  //
  // Тип     ( нормально )
  u8_Buf =u8_Type;                   Update_IR_TX_Packet(u8_Buf);
  // Тип     (  инверсно )
  u8_Buf = u8_Type^0xFF;             Update_IR_TX_Packet(u8_Buf);
  //
  // Красный Младшая( нормально )
  u8_Buf = u16_Red ;                 Update_IR_TX_Packet(u8_Buf);
  // Красный Младшая(  инверсно )
  u8_Buf = u16_Red^0xFF ;            Update_IR_TX_Packet(u8_Buf);
  //
  // Красный Старшая(  нормально )
           u16_Red = u16_Red/0x100;  
  u8_Buf = u16_Red ;                 Update_IR_TX_Packet(u8_Buf);
  // Красный Старшая(  инверсно ) 
           u16_Red = u16_Red/0x100; 
  u8_Buf = u16_Red^0xFF ;            Update_IR_TX_Packet(u8_Buf);
  //
  //
  //   
  // Зеленый Младшая ( нормально )
  u8_Buf =u16_Green;                 Update_IR_TX_Packet(u8_Buf);

  // Зеленый Младшая (  инверсно )
  u8_Buf = u8_Buf^0xFF ;             Update_IR_TX_Packet(u8_Buf);
  //
  // Зеленый Старшая ( нормально )
           u16_Green = u16_Green/0x100; 
  u8_Buf = u16_Green;                Update_IR_TX_Packet(u8_Buf);

  // Зеленый Старшая (  инверсно )
              u16_Green = u16_Green/0x100; 
  u8_Buf    = u16_Green^0xFF ;       Update_IR_TX_Packet(u8_Buf);
  //
  //
  // Синий  Младшая ( нормально )
  u8_Buf = u16_Blue;                 Update_IR_TX_Packet(u8_Buf);
  //
  // Синий  Младшая (  инверсно )
  u8_Buf = u16_Blue^0xFF;            Update_IR_TX_Packet(u8_Buf);
  //
  //
  // Синий  Старшая ( нормально )
           u16_Blue = u16_Blue/0x100; 
  u8_Buf = u16_Blue;                 Update_IR_TX_Packet(u8_Buf);
  //
  // Синий Старшая  (  инверсно )
           u16_Blue = u16_Blue/0x100; 
  u8_Buf = u16_Blue^0xFF;            Update_IR_TX_Packet(u8_Buf);
  //
  //
  // Сумма   ( нормально )
  u8_Buf =       u8_Address+  u8_Type+  u16_Red+  u16_Green+  u16_Blue;  
                                     Update_IR_TX_Packet(u8_Buf);
  // Сумма   (  инверсно )
  u8_Buf = u8_Buf^0xFF;              Update_IR_TX_Packet(u8_Buf);
  //
 
}    
//      
void Create_IR_Tx_White_UV_Packet ( uint8_t  u8_Address,  // IR_TX_BROADBAND,
                                    uint8_t  u8_Type,     // IR_TX_BRIGHT_INFO,
                                    uint16_t  u16_Level )// White or UV
{
  //
  u16_Crt_IR_TX_StepCounter=0;
  //
  uint8_t u8_Buf;
  
  // Формируем блок задания работы формирователя огибающей.
  //
  // 
  // Преамбула
  // 4 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 4 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 1 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  // 1 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  //
  //
  // Адрес   ( нормально )
  u8_Buf = u8_Address;       
  Update_IR_TX_Packet(u8_Buf);
  // Адрес   (  инверсно )
  u8_Buf = u8_Address^0xFF;  
  Update_IR_TX_Packet(u8_Buf);
  //
  // Тип     ( нормально )
  u8_Buf =u8_Type;                   Update_IR_TX_Packet(u8_Buf);
  // Тип     (  инверсно )
  u8_Buf = u8_Type^0xFF;             Update_IR_TX_Packet(u8_Buf);
  //
  // ЯРКОСТЬ белого или УФ. Младшая( нормально )
  u8_Buf = u16_Level ;                 Update_IR_TX_Packet(u8_Buf);
  // ЯРКОСТЬ белого или УФ.  Младшая(  инверсно )
  u8_Buf = u16_Level^0xFF ;            Update_IR_TX_Packet(u8_Buf);
  //
  // ЯРКОСТЬ белого или УФ.  Старшая(  нормально )
           u16_Level = u16_Level/0x100;  
  u8_Buf = u16_Level ;                 Update_IR_TX_Packet(u8_Buf);
  // ЯРКОСТЬ белого или УФ.  Старшая(  инверсно ) 
           u16_Level = u16_Level/0x100; 
  u8_Buf = u16_Level^0xFF ;            Update_IR_TX_Packet(u8_Buf);
  //
  //
  //
  // Сумма   ( нормально )
  u8_Buf =       u8_Address+  u8_Type+  u16_Level;  
                                     Update_IR_TX_Packet(u8_Buf);
  // Сумма   (  инверсно )
  u8_Buf = u8_Buf^0xFF;              Update_IR_TX_Packet(u8_Buf);
  //
}    
//

void  Create_IR_Tx_Selector_Packet ( uint8_t  u8_Address,  
                                     uint8_t  u8_Type      ) //
{
  //
  u16_Crt_IR_TX_StepCounter=0;
  //
  uint8_t u8_Buf;
  //
  // Формируем блок задания работы формирователя огибающей.
  //
  // 
  // Преамбула
  // 4 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 4 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 1 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  // 1 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  //
  //
  // Адрес   ( нормально )
  u8_Buf = u8_Address;       
  Update_IR_TX_Packet(u8_Buf);
  // Адрес   (  инверсно )
  u8_Buf = u8_Address^0xFF;  
  Update_IR_TX_Packet(u8_Buf);
  //
  // Тип     ( нормально )
  u8_Buf =u8_Type;                   Update_IR_TX_Packet(u8_Buf);
  // Тип     (  инверсно )
  u8_Buf = u8_Type^0xFF;             Update_IR_TX_Packet(u8_Buf);
  //
  //
  // Сумма   ( нормально )
  u8_Buf =       u8_Address+  u8_Type ;  
                                    Update_IR_TX_Packet(u8_Buf);
  // Сумма   (  инверсно )
  u8_Buf = u8_Buf^0xFF;             Update_IR_TX_Packet(u8_Buf);
  // 
}
//
  
// Фактически - это копия селектора. Я тоже это вижу но не изменяю. 
void  Create_IR_Tx_ControlMode_Packet ( uint8_t  u8_Address,  
                                        uint8_t  u8_Type      ) //
{
  //
  u16_Crt_IR_TX_StepCounter=0;
  //
  uint8_t u8_Buf;
  //
  // Формируем блок задания работы формирователя огибающей.
  //
  // 
  // Преамбула
  // 4 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 4 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 1 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  // 1 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  //
  //
  // Адрес   ( нормально )
  u8_Buf = u8_Address;       
  Update_IR_TX_Packet(u8_Buf);
  // Адрес   (  инверсно )
  u8_Buf = u8_Address^0xFF;  
  Update_IR_TX_Packet(u8_Buf);
  //
  // Тип     ( нормально )
  u8_Buf =u8_Type;                   Update_IR_TX_Packet(u8_Buf);
  // Тип     (  инверсно )
  u8_Buf = u8_Type^0xFF;             Update_IR_TX_Packet(u8_Buf);
  //
  //
  // Сумма   ( нормально )
  u8_Buf =       u8_Address+  u8_Type ;  
                                    Update_IR_TX_Packet(u8_Buf);
  // Сумма   (  инверсно )
  u8_Buf = u8_Buf^0xFF;             Update_IR_TX_Packet(u8_Buf);
  // 
  // Добавляем экстрабит
  // bit_EXTRA
                             u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS;                        
 // if ((u8_Buf&0x80) == 0x80) u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_HIGH_DUTY_TICS;
 //                       else u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX__LOW_DUTY_TICS; 
}
*/
//
//
/* IR_SYNC_CONTROL
void  Create_IR_Tx_SuncMode_Packet    ( uint8_t  u8_Address,  
                                        uint8_t  u8_Type      ) //
{
  //
  u16_Crt_IR_TX_StepCounter=0;
  //
  uint8_t u8_Buf;
  //
  // Формируем блок задания работы формирователя огибающей.
  //
  // 
  // Преамбула
  // 4 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 4 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 1 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  // 1 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  //
  //
  // Адрес   ( нормально )
  u8_Buf = u8_Address;       
  Update_IR_TX_Packet(u8_Buf);
  // Адрес   (  инверсно )
  u8_Buf = u8_Address^0xFF;  
  Update_IR_TX_Packet(u8_Buf);
  //
  // Тип     ( нормально )
  u8_Buf =u8_Type;                   Update_IR_TX_Packet(u8_Buf);
  // Тип     (  инверсно )
  u8_Buf = u8_Type^0xFF;             Update_IR_TX_Packet(u8_Buf);
  //
  //
  // Сумма   ( нормально )
  u8_Buf =       u8_Address+  u8_Type ;  
                                    Update_IR_TX_Packet(u8_Buf);
  // Сумма   (  инверсно )
  u8_Buf = u8_Buf^0xFF;             Update_IR_TX_Packet(u8_Buf);
  // 
}
*/
//
//
// Фактически - это копия Create_IR_Tx_White_UV_Packet. Я тоже это вижу но не изменяю. 
void Create_IR_Tx_ChangeSpeed_Packet (  uint8_t  u8_Address,  // IR_TX_BROADBAND,
                                        uint8_t  u8_Type,     // IR_TX_BRIGHT_INFO,
                                       uint16_t  u16_Level )// White or UV
{
  //
  u16_Crt_IR_TX_StepCounter=0;
  //
  uint8_t u8_Buf;
  //
  // Формируем блок задания работы формирователя огибающей.
  //
  // 
  // Преамбула
  // 6 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 6 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_6ms_DUTY_TICS;
  // 2 мс - Включение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  // 2 мс - выключение
  u16_IR_Tx_Packet[u16_Crt_IR_TX_StepCounter++] = IR_TX_2ms_DUTY_TICS;
  //
  // Адрес   ( нормально )
  u8_Buf = u8_Address;       
  Update_IR_TX_Packet(u8_Buf);
  // Адрес   (  инверсно )
  u8_Buf = u8_Address^0xFF;  
  Update_IR_TX_Packet(u8_Buf);
  //
  // Тип     ( нормально )
  u8_Buf =u8_Type;                   Update_IR_TX_Packet(u8_Buf);
  // Тип     (  инверсно )
  u8_Buf = u8_Type^0xFF;             Update_IR_TX_Packet(u8_Buf);
  //
  // ЯРКОСТЬ белого или УФ. Младшая( нормально )
  u8_Buf = u16_Level ;                 Update_IR_TX_Packet(u8_Buf);
  // ЯРКОСТЬ белого или УФ.  Младшая(  инверсно )
  u8_Buf = u16_Level^0xFF ;            Update_IR_TX_Packet(u8_Buf);
  //
  // ЯРКОСТЬ белого или УФ.  Старшая(  нормально )
           u16_Level = u16_Level/0x100;  
  u8_Buf = u16_Level ;                 Update_IR_TX_Packet(u8_Buf);
  // ЯРКОСТЬ белого или УФ.  Старшая(  инверсно ) 
           u16_Level = u16_Level/0x100; 
  u8_Buf = u16_Level^0xFF ;            Update_IR_TX_Packet(u8_Buf);
  //
  //
  // Сумма   ( нормально )
  u8_Buf =       u8_Address+  u8_Type+  u16_Level;  
                                     Update_IR_TX_Packet(u8_Buf);
  // Сумма   (  инверсно )
  u8_Buf = u8_Buf^0xFF;              Update_IR_TX_Packet(u8_Buf);
  //
}    
//
//
/* IR_SYNC_CONTROL
void Process_IR_SyncSetActualWhite_Code(void)
{
  u8_Actual_Wht_Uv_Selector = SELECTOR_ACTUAL_WHITE;
}
//
void Process_IR_SyncSetActualUv_Code   (void)
{
  u8_Actual_Wht_Uv_Selector = SELECTOR_ACTUAL_UV;
}
//
//
void Process_IR_SyncSetManual_Code     (void)
{
  u8_StateMaschine = SM_MODE_MANUAL;
  u8_ManualChannel = MANUAL_MODE_RED;             
}
//  
//
void Process_IR_SyncSetAuto_Code       (void)
{
   i16_LightLevelRed_AM   = 255; //   RED_MAX_INDEX; 
   i16_LightLevelGreen_AM = 255; // GREEN_MAX_INDEX;
   i16_LightLevelBlue_AM  = 255; //  BLUE_MAX_INDEX;
   i16_LightLevelUv_AM    = 255; //    UV_MAX_INDEX;
   i16_LightLevelWhite_AM = 255; // WHITE_MAX_INDEX;
   //
   Process_Mode_Sequential_Button();
}
//  

void Process_IR_SyncSetIndependent_Code(void)
{
  u8_SyncModeSelector = SYNC_MODE_INDEP;
}
//  
void Process_IR_SyncSetGrouped_Code    (void)
{
  u8_SyncModeSelector = SYNC_MODE_GROUP;
}
*/
//  
/* IR_SYNC_CONTROL
void Process_IR_SetAutoPlay_Code()
{
  if (u8_Mode_Sqnt_Status == MODE_STATUS_STOP )
      u8_Mode_Sqnt_Status  = MODE_STATUS_RUN;
}
//

void Process_IR_SetAutoPaus_Code()
{
  if (u8_Mode_Sqnt_Status == MODE_STATUS_RUN )
      u8_Mode_Sqnt_Status  = MODE_STATUS_STOP;
}
//

void Process_IR_SetAutoStop_Code()
{
  if (u8_Mode_Sqnt_Status != MODE_STATUS_STOP)
  { 
    u8_Mode_Sqnt_Status  = MODE_STATUS_STOP;
    u32_Mode_Sqnt_SM      = SM_MODE_RED_UP;
  }
  //    TurnOffPattern();
  u8_StateMaschine = SM_MODE_TURN_OFF;
}
//

void Process_IR_SingleStrobe_Code( void )
{
  // Эмулируем прием команды от пульта
//  u8_ManualButton = MANUAL_MODE_BUTTON_STROBOSCOPE; 
  u8_Mode_Strobe_SM = SM_MODE_STROB_ON;
  //
  u16_Mode_Strob_SubCounter = 0;
  u16_Mode_Strob_Counter_ms=0;
  //
  //u8_LED3_State = LED_3_NoSync;  
}
*/
//  
//
void ProcessIRDA_Rx(void)
{
  if ( (u16_IR_RX_captureCounter == 72 ) && ( IR_GetCode_V2() == true )    )   // ( IR_GetCode() == true ) 
  { // Код от пульта Remote control code
    if ( ( (u8_LED2_State == LED_2_MASTER) && (b_Grouped)) || (!b_Grouped) )
    u8_LED5_State               = LED_5_IR_COMMAND; // Как минимум- формат правильный
    ProcessIRCode();
    b_Send_ESP32_LED7_Packet    = true;
  } 
  //
  /* // IR_SYNC 
  if ( (u16_IR_RX_captureCounter == 292) && ( IR_SyncRGB_GetCode() == true ) && !b_Grouped )
  { // Синхронизация RGB   RGB sync
    Process_IR_SyncRGB_Code();
  } */
  //
  /* IR_SYNC_CONTROL 
  // ПЕРЕДАЧА ОДНОГО ПАРАМЕТРА.         TRANSFER OF ONE PARAMETER.
  // Задание оттенка белого или УФ      Set white or UV tint
  // Задание скорости                   Speed reference
  if ( (u16_IR_RX_captureCounter == 164)&& ( IR_SyncSetParam_GetCode() == true ) )
  { 
    // Синхронизация белого/УФ или задание скорости. 
    // White/UV sync or set speed.
    Process_IR_SyncSetParam_Code(); 
  } 
  */
  //
  /* IR_SYNC_CONTROL
  // ПЕРЕДАЧА ДИСКРЕТНОЙ КОМАНДЫ.       TRANSMISSION OF A DISCRETE COMMAND.
  // Активация белого или УФ.           White or UV activation.
  // Ручной или автоматический          Manual or automatic
  // Групповой или индивидуальный       Group or individual
  if ( (u16_IR_RX_captureCounter == 100)&& ( IR_SyncSetMode_GetCode() == true ) )
  { 
    //  Управление селектором выбора - белый или УФ
    //  Control selector selector - white or UV

    if ( u8_Type == IR_TX_SET_ACTUAL_WHITE     )       Process_IR_SyncSetActualWhite_Code();
    if ( u8_Type == IR_TX_SET_ACTUAL_UV        )       Process_IR_SyncSetActualUv_Code   ();
    //
    if ( u8_Type == IR_TX_SET_MODE_MANUAL      )       Process_IR_SyncSetManual_Code     ();  
    if ( u8_Type == IR_TX_SET_MODE_AUTO        )       Process_IR_SyncSetAuto_Code       ();  
    //
    if ( u8_Type == IR_TX_SET_SYNC_INDEPENDENT )       Process_IR_SyncSetIndependent_Code();  
    if ( u8_Type == IR_TX_SET_SYNC_GROUPED     )       Process_IR_SyncSetGrouped_Code    ();  
  }  
  */
   /* 
 IR_SYNC_CONTROL
  // 102 103 104 это заплатка на случай сбоя приема хвоста последовательность
  if (  (  (u16_IR_RX_captureCounter == 102) || 
           (u16_IR_RX_captureCounter == 103) ||
           (u16_IR_RX_captureCounter == 104)    )  && 
        ( IR_SyncAutoMode_Check() == true       ) 
      )
  { 
    u16_IR_RX_captureCounter = 102; // Восстанавливаем требуемую длину
    //  Управление Воспроизведением программы
    //  Program Playback Control
   if ( u8_Type ==  IR_TX_SET_AUTO_PLAY     )       Process_IR_SetAutoPlay_Code();
      if ( u8_Type ==  IR_TX_SET_AUTO_PAUS     )       Process_IR_SetAutoPaus_Code();
   if ( u8_Type ==  IR_TX_SET_AUTO_STOP     )       Process_IR_SetAutoStop_Code();  
    //
  if ( u8_Type ==  IR_TX_SET_SINGLE_STROBE )       Process_IR_SingleStrobe_Code();   
    
  }
  else
  {
    asm("nop");
  }*/ 
  //
  u16_IR_RX_captureCounter     =     0 ; 
  b_IR_RX_PacketReceived  = false ;   
  b_IR_RX_PacketProcessed =  true ; 
}
//



/* IR_SYNC_CONTROL 
#define IR_TX_PHASE_IDLE         0
#define IR_TX_PHASE_PREPEARING   1
#define IR_TX_PHASE_TRANSMITTION 2
#define IR_TX_PHASE_WAIT_FOR_ACK 3
uint8_t u8_IR_TX_State = IR_TX_PHASE_IDLE;
*/

// Состояние изменяется при воздействии на устройство. 
// The state changes when the device is impacted.
// В ручном режиме сначала принимаем, разшифровываем, передаем, а затем только выполняем.
// In manual mode, we first receive, decrypt, transmit, and then only execute.
// В автоматическом режиме передаем статут процесса - запуск, стоп и номер шага.
// In automatic mode, we pass the status of the process - start, stop and step number.
/* IR_SYNC_CONTROL 
void   IrTxIdleProcessing(void)
{
  // Ждем прихода команды от WiFi. Если в режиме атономной работы, то сразу выполняем.
  // Wait for a command from WiFi. If in standalone mode, then immediately execute.
  // Если в составе сети, то сначала отправляем в эфир, принимаем и уже в качестве принятого - выполняем.
  // If as part of the network, then first we send it on the air, receive it, and then execute it as received.
  if (  ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_RGB               ) || 
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_WHITE_LIGHT_LEVEL ) || 
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_UV_LIGHT_LEVEL    ) ||
        //
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_ACTUAL_WHITE      ) ||
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_ACTUAL_UV         ) ||
        //  
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_MODE_MANUAL       ) ||
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_MODE_AUTO         ) ||
        //  
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_SYNC_INDEPENDENT  ) ||
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_SYNC_GROUPED      ) ||
        //
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_CHANGE_SPEED      ) ||
        //
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_AUTO_PLAY         ) ||
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_AUTO_PAUS         ) ||
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_AUTO_STOP         ) ||
        ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_SINGLE_STROBE     )      )  
  {
    //  u8_ProcessWiFiCommand = WIFI_COMMAND_IDLE;
    u8_IR_TX_State = IR_TX_PHASE_PREPEARING;
  }  
}
*/

/* IR_SYNC_CONTROL 
//
void   IrTxPrepearingProcessing(void)
{
  // На основе принятой команды по Wifi формируем пакет на передачу
  if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_RGB)  // 1 
  {
    // Готовим на отправку блок управления 
    //    0 - широковещательный,
    //    1- значение яркости RGB
    //    3*2 байта=6 байт яркости
    u16_IR_TX_Red   = u8_desired_R;
    u16_IR_TX_Green = u8_desired_G;
    u16_IR_TX_Blue  = u8_desired_B;
    //
    //    1 байт - контрольная сумма 
    Create_IR_Tx_RGB_Packet(  IR_TX_BROADBAND,
                              IR_TX_RGB_INFO,
                              u16_IR_TX_Red,
                              u16_IR_TX_Green,
                              u16_IR_TX_Blue      );
  }
  //
  if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_WHITE_LIGHT_LEVEL) // 2 
  {
    // Готовим на отправку блок управления 
    u16_IR_TX_White  =u8_desired_W;
    //
    Create_IR_Tx_White_UV_Packet(    IR_TX_BROADBAND,
                                     IR_TX_WHITE_INFO,
                                 u16_IR_TX_White       );
  }
  //
  // IR_SYNC_CONTROL if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_UV_LIGHT_LEVEL) // 3
  // IR_SYNC_CONTROL {
    // Готовим на отправку блок управления 
  // IR_SYNC_CONTROL   u16_IR_TX_Uv  =u8_desired_U;
    //
  // IR_SYNC_CONTROL   Create_IR_Tx_White_UV_Packet(    IR_TX_BROADBAND,
  // IR_SYNC_CONTROL                                    IR_TX_UV_INFO,
  // IR_SYNC_CONTROL                                u16_IR_TX_Uv        );
  // IR_SYNC_CONTROL }
  //
  // IR_SYNC_CONTROL if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_ACTUAL_WHITE ) // 4 
  // IR_SYNC_CONTROL {
  // IR_SYNC_CONTROL   Create_IR_Tx_Selector_Packet(    IR_TX_BROADBAND,
  // IR_SYNC_CONTROL                                    IR_TX_SET_ACTUAL_WHITE  );
  // IR_SYNC_CONTROL }
  //
  // IR_SYNC_CONTROL if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_ACTUAL_UV ) //  5
  // IR_SYNC_CONTROL {
  // IR_SYNC_CONTROL   Create_IR_Tx_Selector_Packet(    IR_TX_BROADBAND,
  // IR_SYNC_CONTROL                                    IR_TX_SET_ACTUAL_UV  );
  // IR_SYNC_CONTROL }
  //
  //
  // IR_SYNC_CONTROL if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_MODE_MANUAL ) //  6
  // IR_SYNC_CONTROL {
  // IR_SYNC_CONTROL   Create_IR_Tx_ControlMode_Packet( IR_TX_BROADBAND,
  // IR_SYNC_CONTROL                                    IR_TX_SET_MODE_MANUAL  );
  // IR_SYNC_CONTROL }
  //
  // IR_SYNC_CONTROL if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_MODE_AUTO ) //  7
  // IR_SYNC_CONTROL {
  // IR_SYNC_CONTROL   Create_IR_Tx_ControlMode_Packet( IR_TX_BROADBAND,
  // IR_SYNC_CONTROL                                    IR_TX_SET_MODE_AUTO  );
  // IR_SYNC_CONTROL }
  //
  //
  // IR_SYNC_CONTROL  if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_SYNC_INDEPENDENT ) //  8
  // IR_SYNC_CONTROL  {
  // IR_SYNC_CONTROL    Create_IR_Tx_SuncMode_Packet(    IR_TX_BROADBAND,
  // IR_SYNC_CONTROL                                     IR_TX_SET_SYNC_INDEPENDENT  );
  // IR_SYNC_CONTROL  }
  //
  // IR_SYNC_CONTROL  if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_SYNC_GROUPED ) // 9 
  // IR_SYNC_CONTROL  {
  // IR_SYNC_CONTROL    Create_IR_Tx_SuncMode_Packet(    IR_TX_BROADBAND,
  // IR_SYNC_CONTROL                                     IR_TX_SET_SYNC_GROUPED  );
  // IR_SYNC_CONTROL  }
  //
  //
  //
  if ( u8_ProcessWiFiCommand == WIFI_COMMAND_SET_CHANGE_SPEED ) // 10
  {
    // Это значения в диапазоне от 0 до 99 
    u16_IR_TX_ChangeSpeed =  u8_SetSpeedValue ;    
    //  
    Create_IR_Tx_ChangeSpeed_Packet( IR_TX_BROADBAND,
                                     IR_TX_SET_CHANGE_SPEED,
                                     u16_IR_TX_ChangeSpeed ); 
  }
  // -------------------------------------------------------------------------- //
        
  if ( u8_ProcessWiFiCommand ==                        WIFI_COMMAND_SET_AUTO_PLAY ) // 11
    Create_IR_Tx_ControlMode_Packet( IR_TX_BROADBAND,  WIFI_COMMAND_SET_AUTO_PLAY );
  //          
  if ( u8_ProcessWiFiCommand ==                        WIFI_COMMAND_SET_AUTO_PAUS ) // 12  
     Create_IR_Tx_ControlMode_Packet( IR_TX_BROADBAND, WIFI_COMMAND_SET_AUTO_PAUS );
  //          
  if ( u8_ProcessWiFiCommand ==                        WIFI_COMMAND_SET_AUTO_STOP ) // 13  
    Create_IR_Tx_ControlMode_Packet( IR_TX_BROADBAND,  WIFI_COMMAND_SET_AUTO_STOP );
  //
  if ( u8_ProcessWiFiCommand ==                        WIFI_COMMAND_SET_SINGLE_STROBE ) // 14  
    Create_IR_Tx_ControlMode_Packet( IR_TX_BROADBAND,  WIFI_COMMAND_SET_SINGLE_STROBE );
 
  // -------------------------------------------------------------------------- //
  //
  u8_ProcessWiFiCommand = WIFI_COMMAND_IDLE;
  //
  u16_IR_TX_PhaseCounter = 0; // Для отсчета фазы отправки
  u16_IR_TX_SendCounter = 0;  // Для отсчета такта в фазе
  // Отправляем при следующем вызове
  u8_IR_TX_State = IR_TX_PHASE_TRANSMITTION;
  // Сразу включаем светодиод
  LL_GPIO_SetOutputPin  (IRDA_TX_GPIO_PORT, IRDA_TX_PIN); 
  //  
}
 */
//
/* IR_SYNC_CONTROL    
void   IrTxTransmittionProcessing(void)
{
  // На основе содержимого массива периодов переключений управляем светодиодом
      u16_IR_Tx_Packet[u16_IR_TX_PhaseCounter]--;
  if (u16_IR_Tx_Packet[u16_IR_TX_PhaseCounter]==0)
  {
    // Переключаем состояние светодиода
    LL_GPIO_TogglePin(IRDA_TX_GPIO_PORT, IRDA_TX_PIN); 
    // Переходим к следующей фазе
      u16_IR_TX_PhaseCounter++;
  }
  // 
  // Если все передали, то переходим в IR_TX_PHASE_WAIT_FOR_ACK
  if (u16_IR_TX_PhaseCounter == u16_Crt_IR_TX_StepCounter) // если пакет полностью отправлен
  {
    // Для дополнительного контоля - выключаем огибающую ИК.
    LL_GPIO_ResetOutputPin(IRDA_TX_GPIO_PORT, IRDA_TX_PIN);
    // Инициируем счетчик ожидания подтверждения со стороны приемника
    u16_IR_TX_WaitCounter = IR_TX_2ms_DUTY_TICS;
    // то ждем подтверждение со стороны приемной части.
    u8_IR_TX_State        = IR_TX_PHASE_WAIT_FOR_ACK;   
  }  
}
//
//   
void   IrTxWaitForAckProcessing(void)
{
  u16_IR_TX_WaitCounter--;
  // По идее, параллельно с отправкой происходит и прием сигнала. 
  // Если в течении 1 мС не пришло подтвердение отправки,  
  // то реинициализируем ожидание изменения статуса для начала передачи.
  // Так-же поступаем если пришло подтверждение
  if (u16_IR_TX_WaitCounter>=3)
    u8_IR_TX_State = IR_TX_PHASE_IDLE;
  
}
//
// 
void   IrTxDefaultProcessing(void)
{
    u8_IR_TX_State = IR_TX_PHASE_IDLE; 
}
*/
//
//    
//
// Вызывается с частотой 76 кГц !!!!!!!!!
  // Сначала реализую передачу данных без поддержки прерывания со стороны пульта,
  // второй очередью  - оценка канала и реализация механизма преемственности доступа к шине со стороны ИК пульта.
  //
  // ИК передатчик работает постоянно и передает данные о состоянии ведущего. Ведущий работает или в ручном или в автоматическом режиме. 
  // В ручном режиме передается пакет
  //    0 - широковещательный,
  //    1- значение яркости
  //    3*2 байта = 6 байта яркости
  //    1 байт - контрольная сумма
  //
  //    В автоматическом режиме имеет смысл передавать только текущий номер таблицы управления. Следует учесть что для каждого устройства имеется  свой уникальный набор максимумов и целесообразно передавать значение в 10 битной кодировке уровня серого для каждого канала. Округляется до 12 бит. Каждый участник сети принимает значение 0..1023 и декодирует его у учетом максимума для канала. 
  //    Структура пакета
  //    0- широковещательный 	             ( 1 байт    )
  //    2- Управление таблицей               ( 1 байт    )
  //    1 - тип таблицы.	             ( 1 байт    )
  //    1 - скорость работы, мс              ( 1 байт    )
  //    2 - индекс таблицы управления  (ТУ). (  2 байта )
  //    Учитывая, что будет передаваться синхроблок длительностью ... мс, 
  //    прямой и инверсный блок данных, то это займет не менее .... мс.
  //
  //    В варианте работы на скоростях изменения около 1 мс синхросигнал всегда будет идти с запаздыванием. 
  //    Следовательно, все узлы работают самостоятельно и получив и декодировав сигнал только корректирую 
  //    свой индекс выполнения и параметры задержки.
  //    
  // Поведение мастера.
  //    Включился, 
  //    получил команду по WiFi, отправил ее в ИК, Получил у самого себя в ИК и выполнил у себя. 
  //    Именно подход, когда сам себе отправил и сам  у себя принял создает эффект максимальной синхронности работы.
  //    
  //
  // Поведение ведомого
  //    Включился, получил команду по ИК, выполнил.
  //    Для варианта выполнения команды в ручном управлении принимаем команду и выполняем однократно.
  //    Если принимаем команду управления таблицей, то автоматически активируем режим работы по таблице в ведомым.
/* IR_SYNC_CONTROL 
void ProcessIRDA_Tx(void)
{
  // ACTUALLY - this is a state machine handler.
  // And you need to do everything sequentially, with a lot of intermediate phases
  // Data preparation phase
  // Data transfer phase
  switch(u8_IR_TX_State)
  {
    case IR_TX_PHASE_IDLE:          IrTxIdleProcessing();         break;
    case IR_TX_PHASE_PREPEARING:    IrTxPrepearingProcessing();   break;
    case IR_TX_PHASE_TRANSMITTION:  IrTxTransmittionProcessing(); break;
    case IR_TX_PHASE_WAIT_FOR_ACK:  IrTxWaitForAckProcessing();   break;
    default:                        IrTxDefaultProcessing();      break;  
  }
}
*/
//


extern uint32_t AutoReloadValue ;
//
// PA9_TIM1_CH2  - IRDA_CARRIER
//__STATIC_INLINE 
void IRDA_CARRIER_ChannelInit(void) // PA9_TIM1_CH2 
{
  //
  // CARRIER PA9 TIM1_CH2
  //
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
 //   LL_AHB2_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  
  /* GPIO TIM2_CH1 configuration */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9,LL_GPIO_SPEED_FREQ_MEDIUM); // LL_GPIO_SPEED_FREQ_HIGH
  LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_0); // LL_GPIO_AF_1
  
  /***********************************************/
  /* Configure the NVIC to handle TIM2 interrupt */
  /***********************************************/
  NVIC_SetPriority(TIM1_CC_IRQn, 12);// 0
  NVIC_EnableIRQ(TIM1_CC_IRQn);

  NVIC_SetPriority( TIM1_TRG_COM_TIM11_IRQn, 0); // TIM11_IRQn
  NVIC_EnableIRQ  ( TIM1_TRG_COM_TIM11_IRQn   ); // TIM11_IRQn
 /* */
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1); 
 // LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1); 
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  
  /* Set the pre-scaler value to have TIM2 counter clock equal to (10) kHz */
  LL_TIM_SetPrescaler(TIM1,  __LL_TIM_CALC_PSC(SystemCoreClock,40000000 ) ); //  10000
  
  /* Enable TIM2_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM1);
  // My
  LL_TIM_OC_SetCompareCH2(TIM1, 250); // 0  No Light
  
  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM2CLK = SystemCoreClock / (APB prescaler & multiplier)               */
  TimOutClock = SystemCoreClock/1;
  // 1080 для 38 кГц. 526 для 76 кГц.
  AutoReloadValue = 1080;//526;//540 ; // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100);
  LL_TIM_SetAutoReload(TIM1, AutoReloadValue); // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM2), 14000)// 100
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH2,LL_TIM_OCMODE_TOGGLE ); // LL_TIM_OCMODE_PWM1  LL_TIM_OCMODE_PWM2
  
  /* Set output channel polarity */
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  //LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  
  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH2(TIM1, ( (LL_TIM_GetAutoReload(TIM1) + 1 ) / 2));
  
  /* Enable TIM2_CCR1 register preload. Read/Write operations access the      */
  /* preload register. TIM2_CCR1 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH2);
  
  /**************************/
  /* TIM2 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC2(TIM1);
  // My
  LL_TIM_OC_SetCompareCH2(TIM1,250 ); //0 No Light
  
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel 1 */
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM1);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM1);
}
//



/**
  * @brief  Initialize LED2.
  * @param  None
  * @retval None
  */
/*
__STATIC_INLINE void LED_Init(void) РУДИМЕНТ
{
  //Enable the LED2 Clock //
  LED2_GPIO_CLK_ENABLE();

  // Configure IO in output push-pull mode to drive external LED2 //
  LL_GPIO_SetPinMode(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);
  // Reset value is LL_GPIO_OUTPUT_PUSHPULL //
  LL_GPIO_SetPinOutputType(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_OUTPUT_PUSHPULL);//++
  // Reset value is LL_GPIO_SPEED_FREQ_LOW LL_GPIO_SPEED_FREQ_LOW//
  LL_GPIO_SetPinSpeed(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_SPEED_FREQ_HIGH);//++
  // Reset value is LL_GPIO_PULL_NO //
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
}
*/

/*
// __STATIC_INLINE 
void IRDA_TX_Init(void)
{
  // Enable the LED2 Clock //
  IRDA_TX_GPIO_CLK_ENABLE();

  // Configure IO in output push-pull mode to drive external LED2 //
  LL_GPIO_SetPinMode(IRDA_TX_GPIO_PORT, IRDA_TX_PIN, LL_GPIO_MODE_OUTPUT);
  // Reset value is LL_GPIO_OUTPUT_PUSHPULL//
  LL_GPIO_SetPinOutputType(IRDA_TX_GPIO_PORT, IRDA_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);//++
  // Reset value is LL_GPIO_SPEED_FREQ_LOW LL_GPIO_SPEED_FREQ_LOW//
  LL_GPIO_SetPinSpeed(IRDA_TX_GPIO_PORT, IRDA_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);//++
  // Reset value is LL_GPIO_PULL_NO //
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
}
//
*/

// __STATIC_INLINE 
/*
void IRDA_Rx_Init(void)
{
  IRDA_RX_GPIO_CLK_ENABLE();
  
  LL_GPIO_SetPinMode(IRDA_RX_GPIO_PORT, IRDA_RX_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(IRDA_RX_GPIO_PORT, IRDA_RX_PIN, LL_GPIO_PULL_NO);
}
*/



/*
// __STATIC_INLINE 
void IRDA_CAR_Init(void)
{
  // Enable the LED2 Clock //
  IRDA_CAR_GPIO_CLK_ENABLE();

  // Configure IO in output push-pull mode to drive external LED2 //
  LL_GPIO_SetPinMode(IRDA_CAR_GPIO_PORT, IRDA_CAR_PIN, LL_GPIO_MODE_OUTPUT);
  // Reset value is LL_GPIO_OUTPUT_PUSHPULL ///
  LL_GPIO_SetPinOutputType(IRDA_CAR_GPIO_PORT, IRDA_CAR_PIN, LL_GPIO_OUTPUT_PUSHPULL);//++
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW LL_GPIO_SPEED_FREQ_LOW//
  LL_GPIO_SetPinSpeed(IRDA_CAR_GPIO_PORT, IRDA_CAR_PIN, LL_GPIO_SPEED_FREQ_HIGH);//++
  // Reset value is LL_GPIO_PULL_NO //
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
}
*/
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
