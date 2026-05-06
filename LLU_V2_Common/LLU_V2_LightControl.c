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
//
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "LLU_V2_BIOS.h"
#include "LLU_V2_ESP32.h"
//#include "LLU_V2_IRDA.h"
#include "LLU_V2_LightControl.h"
//
#include "stdbool.h"   
#include "string.h" 
#include <stdio.h>
#include <stdlib.h>
//
extern uint8_t u8_ProcessWiFiCommand ;
//
extern uint8_t u8_DeviceRole;
//
//
#ifdef LPU_V2_DEVICE 
//
extern uint16_t  u16_RedLightFreqPeriod_LP        [ RED_LEVEL_MAX_LP ];
extern uint16_t  u16_GreenLightFreqPeriod_LP      [ GREEN_LEVEL_MAX_LP ];
extern uint16_t  u16_BlueLightFreqPeriod_LP       [ BLUE_LEVEL_MAX_LP ];
extern uint16_t  u16_UvLightFreqPeriod_LP         [ UV_LEVEL_MAX_LP ];
extern uint16_t  u16_WhiteLightFreqPeriod_LP      [ WHITE_LEVEL_MAX_LP ];
//
extern uint16_t  u16_RedRefference_LP  [256] ;// Индексы шим таблицы 
extern uint16_t  u16_GreenRefference_LP[256] ;// Индексы шим таблицы 
extern uint16_t  u16_BlueRefference_LP [256] ;// Индексы шим таблицы 
extern uint16_t  u16_UvRefference_LP   [256] ;// Индексы шим таблицы 
extern uint16_t  u16_WhiteRefference_LP[256] ;// Индексы шим таблицы 
//
#endif
//
//
#ifdef MPU_V2_DEVICE 
//
extern uint16_t  u16_RedLightFreqPeriod_MP        [ RED_LEVEL_MAX_MP ];
extern uint16_t  u16_GreenLightFreqPeriod_MP      [ GREEN_LEVEL_MAX_MP ];
extern uint16_t  u16_BlueLightFreqPeriod_MP       [ BLUE_LEVEL_MAX_MP ];
extern uint16_t  u16_UvLightFreqPeriod_MP         [ UV_LEVEL_MAX_MP ];
extern uint16_t  u16_WhiteLightFreqPeriod_MP      [ WHITE_LEVEL_MAX_MP ];
//
extern uint16_t  u16_RedRefference_MP  [256] ;// Индексы шим таблицы 
extern uint16_t  u16_GreenRefference_MP[256] ;// Индексы шим таблицы 
extern uint16_t  u16_BlueRefference_MP [256] ;// Индексы шим таблицы 
extern uint16_t  u16_UvRefference_MP   [256] ;// Индексы шим таблицы 
extern uint16_t  u16_WhiteRefference_MP[256] ;// Индексы шим таблицы 
//

//
#endif
//
//
#ifdef  HPU_V2_DEVICE
//
extern uint16_t  u16_RedLightFreqPeriod_HP        [ RED_LEVEL_MAX_HP ];
extern uint16_t  u16_GreenLightFreqPeriod_HP      [ GREEN_LEVEL_MAX_HP ];
extern uint16_t  u16_BlueLightFreqPeriod_HP       [ BLUE_LEVEL_MAX_HP ];
extern uint16_t  u16_UvLightFreqPeriod_HP         [ UV_LEVEL_MAX_HP ];
extern uint16_t  u16_WhiteLightFreqPeriod_HP      [ WHITE_LEVEL_MAX_HP ];
//
extern uint16_t  u16_RedRefference_HP  [256] ;// Индексы шим таблицы 
extern uint16_t  u16_GreenRefference_HP[256] ;// Индексы шим таблицы 
extern uint16_t  u16_BlueRefference_HP [256] ;// Индексы шим таблицы 
extern uint16_t  u16_UvRefference_HP   [256] ;// Индексы шим таблицы 
extern uint16_t  u16_WhiteRefference_HP[256] ;// Индексы шим таблицы 
//
#endif
//


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
//
// Это индексы таблицы управления. От 0 до 1920, к примеру.
uint16_t   RED_MAX_INDEX = 0;
uint16_t GREEN_MAX_INDEX = 0;
uint16_t  BLUE_MAX_INDEX = 0;
uint16_t    UV_MAX_INDEX = 0;
uint16_t WHITE_MAX_INDEX = 0;
//
//
/* Measured duty cycle */
__IO uint32_t uw_TIM1_MeasuredDutyCycle  = 0; // PA9_TIM1_CH2  - IRDA_CARRIER
__IO uint32_t uw_TIM11_MeasuredDutyCycle = 0; // PB9_TIM11_CH1 - RED
__IO uint32_t uw_TIM3_MeasuredDutyCycle  = 0; // PB0 TIM3_CH3  - GREEN
__IO uint32_t uw_TIM2_MeasuredDutyCycle  = 0; // PA1 TIM2_CH2  - BLUE_PWM 
__IO uint32_t uw_TIM10_MeasuredDutyCycle = 0; // PB8_TIM10_CH1 - UV
__IO uint32_t uw_TIM4_MeasuredDutyCycle  = 0; // PB7_TIM4_CH2  - WHT
//
//

/* TIMx Clock */
uint32_t TimOutClock = 1;

/* Private function prototypes -----------------------------------------------*/
//
//uint32_t u32_ActualNetRole = ESP32_NET_ROLE_DEFAULT  ;
extern uint32_t u32_ActualNetRole   ;
//
//
void Delay_uS(uint16_t u16_Delay)
{
   while (u16_Delay--)
   {
//+++     asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
     asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");     
   }
}
//
// =============================================================================
// PERCEPTUAL GAMMA CORRECTION — Manual mode only (Stage 2)
// =============================================================================
// Applied exclusively inside DirectLightCtrl() — the manual brightness path.
// Animation ISR paths are NOT affected; they continue using the original
// repeat-factor tables and PWM lookup tables unchanged.
//
// Each channel has its own gamma table (101 entries, fixed-point x10000).
// R/G/B/W use gamma 2.2 (standard perceptual curve for visible LEDs).
// UV uses gamma 1.8 (more linear lux-vs-current response than visible LEDs).
//
// To retune a channel after hardware measurement, replace only its table.
// Regenerate with: round((i/100)^gamma * 10000) for i in 0..100.
// No floating point, no math.h, safe to call from any context.
//
// gamma=2.2  (RED)
const uint16_t u16_GammaTable_RED[101] = {
        0,     0,     2,     4,     8,    14,    21,    29,    39,    50,
       63,    78,    94,   112,   132,   154,   177,   203,   230,   259,
      290,   323,   358,   394,   433,   474,   516,   561,   608,   657,
      707,   760,   815,   872,   932,   993,  1056,  1122,  1190,  1260,
     1332,  1406,  1483,  1562,  1643,  1726,  1812,  1899,  1989,  2082,
     2176,  2273,  2373,  2474,  2578,  2684,  2793,  2904,  3017,  3132,
     3250,  3371,  3494,  3619,  3746,  3876,  4009,  4143,  4281,  4420,
     4563,  4707,  4854,  5004,  5156,  5310,  5468,  5627,  5789,  5954,
     6121,  6290,  6462,  6637,  6814,  6994,  7176,  7361,  7549,  7739,
     7931,  8126,  8324,  8524,  8727,  8933,  9141,  9352,  9565,  9781,
    10000,
};
// gamma=2.2  (GREEN)
const uint16_t u16_GammaTable_GREEN[101] = {
        0,     0,     2,     4,     8,    14,    21,    29,    39,    50,
       63,    78,    94,   112,   132,   154,   177,   203,   230,   259,
      290,   323,   358,   394,   433,   474,   516,   561,   608,   657,
      707,   760,   815,   872,   932,   993,  1056,  1122,  1190,  1260,
     1332,  1406,  1483,  1562,  1643,  1726,  1812,  1899,  1989,  2082,
     2176,  2273,  2373,  2474,  2578,  2684,  2793,  2904,  3017,  3132,
     3250,  3371,  3494,  3619,  3746,  3876,  4009,  4143,  4281,  4420,
     4563,  4707,  4854,  5004,  5156,  5310,  5468,  5627,  5789,  5954,
     6121,  6290,  6462,  6637,  6814,  6994,  7176,  7361,  7549,  7739,
     7931,  8126,  8324,  8524,  8727,  8933,  9141,  9352,  9565,  9781,
    10000,
};
// gamma=2.2  (BLUE)
const uint16_t u16_GammaTable_BLUE[101] = {
        0,     0,     2,     4,     8,    14,    21,    29,    39,    50,
       63,    78,    94,   112,   132,   154,   177,   203,   230,   259,
      290,   323,   358,   394,   433,   474,   516,   561,   608,   657,
      707,   760,   815,   872,   932,   993,  1056,  1122,  1190,  1260,
     1332,  1406,  1483,  1562,  1643,  1726,  1812,  1899,  1989,  2082,
     2176,  2273,  2373,  2474,  2578,  2684,  2793,  2904,  3017,  3132,
     3250,  3371,  3494,  3619,  3746,  3876,  4009,  4143,  4281,  4420,
     4563,  4707,  4854,  5004,  5156,  5310,  5468,  5627,  5789,  5954,
     6121,  6290,  6462,  6637,  6814,  6994,  7176,  7361,  7549,  7739,
     7931,  8126,  8324,  8524,  8727,  8933,  9141,  9352,  9565,  9781,
    10000,
};
// gamma=1.8  (UV — more linear lux-vs-current response than visible LEDs)
const uint16_t u16_GammaTable_UV[101] = {
        0,     3,     9,    18,    30,    46,    63,    83,   106,   131,
      158,   188,   220,   254,   290,   329,   369,   412,   457,   503,
      552,   603,   655,   710,   766,   825,   885,   947,  1011,  1077,
     1145,  1215,  1286,  1359,  1434,  1511,  1590,  1670,  1752,  1836,
     1922,  2009,  2098,  2189,  2281,  2376,  2472,  2569,  2668,  2769,
     2872,  2976,  3082,  3189,  3298,  3409,  3522,  3636,  3751,  3868,
     3987,  4108,  4230,  4353,  4478,  4605,  4733,  4863,  4995,  5128,
     5262,  5398,  5536,  5675,  5816,  5958,  6102,  6247,  6394,  6542,
     6692,  6843,  6996,  7151,  7306,  7464,  7622,  7783,  7945,  8108,
     8272,  8439,  8606,  8775,  8946,  9118,  9292,  9466,  9643,  9821,
    10000,
};
// gamma=2.2  (WHITE)
const uint16_t u16_GammaTable_WHITE[101] = {
        0,     0,     2,     4,     8,    14,    21,    29,    39,    50,
       63,    78,    94,   112,   132,   154,   177,   203,   230,   259,
      290,   323,   358,   394,   433,   474,   516,   561,   608,   657,
      707,   760,   815,   872,   932,   993,  1056,  1122,  1190,  1260,
     1332,  1406,  1483,  1562,  1643,  1726,  1812,  1899,  1989,  2082,
     2176,  2273,  2373,  2474,  2578,  2684,  2793,  2904,  3017,  3132,
     3250,  3371,  3494,  3619,  3746,  3876,  4009,  4143,  4281,  4420,
     4563,  4707,  4854,  5004,  5156,  5310,  5468,  5627,  5789,  5954,
     6121,  6290,  6462,  6637,  6814,  6994,  7176,  7361,  7549,  7739,
     7931,  8126,  8324,  8524,  8727,  8933,  9141,  9352,  9565,  9781,
    10000,
};

// GammaCorrectIndex — converts a linear PWM index to a perceptually uniform one.
// u16_Index:   raw index in [u16_Offset .. u16_Max-1]
// u16_Max:     channel table size (e.g. RED_MAX_INDEX)
// u16_Offset:  dead-zone offset (e.g. RED_OFFSET)
// pu16_Table:  pointer to the channel's 101-entry gamma table
// Returns:     gamma-corrected index in the same [u16_Offset .. u16_Max-1] range.
uint16_t GammaCorrectIndex(uint16_t u16_Index, uint16_t u16_Max,
                                   uint16_t u16_Offset, const uint16_t* pu16_Table)
{
    uint32_t u32_Range, u32_Pct, u32_Corrected;
    if (u16_Index <= u16_Offset) return u16_Offset;
    if (u16_Index >= u16_Max)    return (uint16_t)(u16_Max - 1u);
    u32_Range = (uint32_t)(u16_Max - u16_Offset);
    u32_Pct   = ((uint32_t)(u16_Index - u16_Offset) * 100u) / u32_Range;
    if (u32_Pct > 100u) u32_Pct = 100u;
    {
        uint32_t u32_frac = ((uint32_t)(u16_Index - u16_Offset) * 100u) % u32_Range;
        uint32_t u32_lo   = pu16_Table[u32_Pct];
        uint32_t u32_hi   = (u32_Pct < 100u) ? pu16_Table[u32_Pct + 1u] : 10000u;
        u32_Corrected     = u32_lo + ((u32_hi - u32_lo) * u32_frac) / u32_Range;
    }
    u32_Corrected = (uint32_t)u16_Offset + (u32_Corrected * u32_Range) / 10000u;
    if (u32_Corrected >= (uint32_t)u16_Max)   u32_Corrected = (uint32_t)(u16_Max - 1u);
    if (u32_Corrected < (uint32_t)u16_Offset) u32_Corrected = (uint32_t)u16_Offset;
    return (uint16_t)u32_Corrected;
}
// =============================================================================
//
//
// На данный момент - просто счетчик количества вызовов таймера 76 кГц
int32_t i32_GlobalDutyCounter   = 0;
//
// На данный момент - только обнуляется при нажатии кнопки SEQUENTIAL или STROB.
int8_t  i8_GlobalDutyIndex      = 0;
//
// НЕ ИСПОЛЬЗУЕТСЯ
int32_t i32_GlobalDutyStorage[256];
//      
//
/*
uint32_t u32_RedLightDelayDutyCounter   = 0; //  int16_t
uint32_t u32_GreenLightDelayDutyCounter = 0; 
uint32_t u32_BlueLightDelayDutyCounter  = 0; 
uint32_t u32_UvLightDelayDutyCounter    = 0; 
uint32_t u32_WhiteLightDelayDutyCounter = 0; */
//
//
// Оценивается в впрерывании, ставистя в SetRedLevel();
bool b_UpdateRedLightLevel  = false;
bool b_RedLightLevelUpdated = true;
//
uint16_t  u16_ActualRedPeriod =     0; // No light
uint16_t  u16_ActualRedFreq   = 65535; // Max Period, i.e about 308 Hz
// ----------------------------------------------- //     
//
//
// ----------------------------------------------- //
//       
//
//
bool b_UpdateGreenLightLevel  = false;
bool b_GreenLightLevelUpdated = true;
//
uint16_t  u16_ActualGreenPeriod =     0; // No light
uint16_t  u16_ActualGreenFreq   = 65535; // Max Period, i.e about 308 Hz
// ----------------------------------------------- //     
//
//
// ----------------------------------------------- //
//
//
bool b_UpdateBlueLightLevel  = false;
bool b_BlueLightLevelUpdated = true;
//
uint16_t  u16_ActualBluePeriod =     0; // No light
uint16_t  u16_ActualBlueFreq   = 65535; // Max Period, i.e about 300 Hz
// ----------------------------------------------- //     
//
//
bool b_UpdateUvLightLevel  = false;
bool b_UvLightLevelUpdated = true;
//
uint16_t  u16_ActualUvPeriod =     0; // No light
uint16_t  u16_ActualUvFreq   = 65535; // Max Period, i.e about 300 Hz
// ----------------------------------------------- //     
//
//
// ----------------------------------------------- //
//
//
bool b_UpdateWhiteLightLevel  = false;
bool b_WhiteLightLevelUpdated = true;
//
uint16_t  u16_ActualWhitePeriod =     0; // No light
uint16_t  u16_ActualWhiteFreq   = 65535; // Max Period, i.e about 300 Hz
// ----------------------------------------------- //     
//
//
//
uint32_t u32_Delay_ms = 5;// 2000;
// фактор ускорения
uint32_t _SF  = 50; 
//
//
//
// ??????????? uint8_t u8_SM_MODE_MANUAL_ChangeReason = SM_MODE_MANUAL_UNDEF;
//
uint8_t u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF ;      //MANUAL_MODE_BUTTON_UNDEF;
//
uint8_t u8_StateMaschine = SM_MODE_TURN_ON;//SM_MODE_STAND_BY;//SM_MODE_POWER_UP
//
uint8_t u8_ManualChannel = MANUAL_MODE_RED; //MANUAL_MODE_UNDEF;
// В прошивке модуля функцию данной переменной выполняет  u32_ActualChannel
//
uint8_t u8_FunctionMode = FUNCTION_MODE_UNDEF;
//
//

uint8_t   u8_Mode_Strob_Status         = MODE_STATUS_STOP; //MODE_STATUS_RUN;
bool       b_Mode_Strob_CallBack       = false;
uint16_t u16_Mode_Strob_StepDelay_A_ms = 15;//20;
uint16_t u16_Mode_Strob_StepDelay_B_ms = 10; // 50;
uint16_t u16_Mode_Strob_StepDelay_C_ms = 10; //500;    //1000;
bool   b_StopStrobMode                 = false;
//
uint8_t u8_Mode_Sqnt_Status       = MODE_STATUS_STOP;
bool     b_Mode_Sqnt_CallBack     = false;
uint8_t u8_Mode_Sqnt_StepDelay_ms = 20;
//uint8_t u8_Mode_Sqnt_Request      = SEQ_MODE_REQUEST_IDLE;
extern uint32_t u32_Mode_Sqnt_SM      ; 
extern uint32_t u32_Mode_Rnbw_SM      ; 
//
//extern bool b_Send_ESP32_SeqMode_Packet  ;
extern bool b_Send_ESP32_Rsrv1_Packet    ;
extern bool b_Send_ESP32_Rsrv2_Packet    ;
//
uint8_t u8_Mode_Rnbw_Status=MODE_STATUS_STOP;
bool     b_Mode_Rnbw_CallBack     = false;
uint8_t u8_Mode_Rnbw_StepDelay_ms = 10;
//
uint8_t u8_Mode_Manl_Status=MODE_STATUS_STOP;
bool     b_Mode_Manl_CallBack     = false;
uint8_t u8_Mode_Manl_StepDelay_ms = 10;
//
uint16_t u16_Mode_Strob_Counter_ms =0;
uint16_t u16_Mode_Strob_SubCounter =0;
//
uint32_t u32_SyncLostTimer_mks = 0; 
// Если значение более 3`000`000 мkс - синхроницациии нет
// Передаем на светодиод LED3 состояние LED_3_NoSync

//
uint32_t u32_RedChannelWaitFlag = 0;
uint32_t u32_GreenChannelWaitFlag = 0;
uint32_t u32_BlueChannelWaitFlag = 0;
uint32_t u32_UvChannelWaitFlag = 0;
uint32_t u32_WhiteChannelWaitFlag = 0;

//
// Глобальные переменные для контроля яркости
// Это значения в диапазоне от 20 до _AM.
uint32_t  u32_LightLevelRed   = 0;
uint32_t  u32_LightLevelGreen = 0;
uint32_t  u32_LightLevelBlue  = 0;
uint32_t  u32_LightLevelUv    = 0;
uint32_t  u32_LightLevelWhite = 0;
//

// Значения рабочих максимумов в процессе работы. 
// Задаются на старте минимальным значениями. Меняются в процессе работы.
// Actual Max. 
// Фактически - это значения в диапазоне от 0 до RED(GREEN и так далее)_MAX_INDEX
// То есть от нуля до (1920-1)-1.
//                           (2120     - 20        )/10 = 2100/10= 210
int16_t i16_R_10perc ;//= ( (  RED_MAX_INDEX - RED_OFFSET)/10);
int16_t i16_G_10perc ;//= ( (GREEN_MAX_INDEX - GRN_OFFSET)/10);
int16_t i16_B_10perc ;//= ( ( BLUE_MAX_INDEX - BLU_OFFSET)/10);
int16_t i16_U_10perc ;//= ( (   UV_MAX_INDEX -  UV_OFFSET)/10);
int16_t i16_W_10perc ;//= ( (WHITE_MAX_INDEX - WHT_OFFSET)/10);
//

// Данные значения ре-инициализируются перед фактическим использование в TurnOnPattern()
int16_t  i16_LightLevelRed_AM   = 100 ; 
int16_t  i16_LightLevelGreen_AM = 100 ; 


int16_t  i16_LightLevelBlue_AM  = 100 ; 
int16_t  i16_LightLevelUv_AM    = 100 ; 
int16_t  i16_LightLevelWhite_AM = 100 ; 
//

// 
// Значения цветом, пришедшие из Web интрефейса
uint8_t u8_desired_R ;  char c_desired_R[4]; uint16_t   u16_LightLevelRed_Desired;  
uint8_t u8_desired_G ;  char c_desired_G[4]; uint16_t u16_LightLevelGreen_Desired;  
uint8_t u8_desired_B ;  char c_desired_B[4]; uint16_t  u16_LightLevelBlue_Desired;  
bool b_UpdateRGB =false;
//
uint8_t u8_desired_W ; float f_desired_W ;    char c_desired_W[5]; uint16_t  u16_LightLevelWhite_Desired;  
bool b_UpdateW =false;
//
uint8_t u8_desired_U ; float f_desired_U ; char c_desired_U[5]; uint16_t  u16_LightLevelUv_Desired;  
bool b_UpdateU =false;
//
//SetSpeed 018
uint8_t u8_SetSpeedValue ;  char c_SetSpeedValue[4];// uint16_t  u16_SetSpeedValue;  
bool b_UpdateSetSpeed =false;
uint16_t AutoModeActualSpeed = 50; // выражено в %
//
//
// Для идентификации канала влияния при приеме сообщения SetLightLevel]
uint8_t u8_Actual_Wht_Uv_Selector = SELECTOR_ACTUAL_WHITE;
//
// режим синхронизации устройстав
uint8_t u8_SyncModeSelector = SYNC_MODE_INDEP;
//
// Общая задержка для работы в автоматическом режиме
uint8_t u8_UpdateSet = UPDATE_DELAY_DEFALT_MS;
//
// Роль в цепи синхронизации
uint8_t u8_SyncRole = SYNC_ROLE_SLAVE; //SYNC_ROLE_MASTER  ;
//
//

void SetRedLevel(uint16_t u16_LightIndex)
{
  //
  // Glow channel works in three modes of the PWM period.
  // 65535 - for the most accurate indication at the beginning of the range
  // 4 kHz period - some later  after the first range
  // 16 kHz period - main range.
  //
  // luminescence index specifies the period passed as out-load to the timer
  // And in parallel, the timer frequency is updated.
  // So, we are working with two pairs of values:
  //  - timer period, (0-65536, i.e. uint16_t)
  //  - timer frequency (0-65536, i.e. uint16_t)
  if ( b_RedLightLevelUpdated )
  {
    b_RedLightLevelUpdated = false;
    //
#ifdef LPU_V2_DEVICE
    {
      u16_ActualRedPeriod = u16_RedLightFreqPeriod_LP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_RedLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualRedFreq   = 65535  ; 
      //
      if ( (u16_RedLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualRedFreq   = 10499  ; 
      //
      if ( (u16_RedLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualRedFreq   = 2624  ;       
    }
#endif    
    //
#ifdef MPU_V2_DEVICE
    {
      u16_ActualRedPeriod = u16_RedLightFreqPeriod_MP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_RedLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualRedFreq   = 65535  ; 
      //
      if ( (u16_RedLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualRedFreq   = 10499  ; 
      //
      if ( (u16_RedLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualRedFreq   = 2624  ;       
    }
#endif 
       //
#ifdef HPU_V2_DEVICE
    {
      u16_ActualRedPeriod = u16_RedLightFreqPeriod_HP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_RedLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualRedFreq   = 65535  ; 
      //
      if ( (u16_RedLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualRedFreq   = 10499  ; 
      //
      if ( (u16_RedLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualRedFreq   = 2624  ;       
    }
#endif    
    //
    b_UpdateRedLightLevel = true;
  }
}
//
//

void SetGreenLevel(uint16_t u16_LightIndex)
{
  //
  //u32_LightLevelGreen = u16_LightIndex;
  //
  // Glow channel works in three modes of the PWM period.
  // 65535 - for the most accurate indication at the beginning of the range
  // 4 kHz period - some later  after the start
  // 16 kHz period - main range.
  //
  // luminescence index specifies the period passed as out-load to the timer
  // And in parallel, the timer frequency is updated.
  // So, we are working with two pairs of values:
  //  - timer period, (0-65536, i.e. uint16_t)
  //  - timer frequency (0-65536, i.e. uint16_t)
  if ( b_GreenLightLevelUpdated )
  {
    b_GreenLightLevelUpdated = false;
    //
#ifdef LPU_V2_DEVICE   
    if(u8_DeviceRole == LOW_POWER_DEVICE)
    {
      u16_ActualGreenPeriod = u16_GreenLightFreqPeriod_LP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_GreenLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualGreenFreq   = 65535  ; 
      //
      if ( (u16_GreenLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualGreenFreq   = 10499  ; 
      //
      if ( (u16_GreenLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualGreenFreq   = 2624  ;       
    }
#endif
    //
#ifdef MPU_V2_DEVICE
    if(u8_DeviceRole == MID_POWER_DEVICE)
    {
      u16_ActualGreenPeriod = u16_GreenLightFreqPeriod_MP[u16_LightIndex]& 0x3FFF ; 
      //
      if ( (u16_GreenLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualGreenFreq   = 65535  ; 
      //
      if ( (u16_GreenLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualGreenFreq   = 10499  ; 
      //
      if ( (u16_GreenLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualGreenFreq   = 2624  ;       
    }
#endif
    //
#ifdef HPU_V2_DEVICE
    if(u8_DeviceRole == HIGH_POWER_DEVICE)
    {
      u16_ActualGreenPeriod = u16_GreenLightFreqPeriod_HP[u16_LightIndex]& 0x3FFF ; 
      //
      if ( (u16_GreenLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualGreenFreq   = 65535  ; 
      //
      if ( (u16_GreenLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualGreenFreq   = 10499  ; 
      //
      if ( (u16_GreenLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualGreenFreq   = 2624  ;       
    }
#endif
    // 
    //
    b_UpdateGreenLightLevel = true;
  }
}
//
//
void SetBlueLevel(uint16_t u16_LightIndex)
{
  //
  //u16_LightLevelBlue = u16_LightIndex;
  //
  // Glow channel works in three modes of the PWM period.
  // 65535 - for the most accurate indication at the beginning of the range
  // 4 kHz period - some later  after the start
  // 16 kHz period - main range.
  //
  // luminescence index specifies the period passed as out-load to the timer
  // And in parallel, the timer frequency is updated.
  // So, we are working with two pairs of values:
  //  - timer period, (0-65536, i.e. uint16_t)
  //  - timer frequency (0-65536, i.e. uint16_t)
  if ( b_BlueLightLevelUpdated )
  {
    b_BlueLightLevelUpdated = false;
    // 
#ifdef LPU_V2_DEVICE  
    if(u8_DeviceRole == LOW_POWER_DEVICE)
    {
      u16_ActualBluePeriod = u16_BlueLightFreqPeriod_LP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_BlueLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualBlueFreq   = 65535  ; 
      //
      if ( (u16_BlueLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualBlueFreq   = 10499  ; 
      //
      if ( (u16_BlueLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualBlueFreq   = 2624  ;       
    }
#endif    
    //
#ifdef MPU_V2_DEVICE    
    if(u8_DeviceRole == MID_POWER_DEVICE)
    {
      u16_ActualBluePeriod = u16_BlueLightFreqPeriod_MP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_BlueLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualBlueFreq   = 65535  ; 
      //
      if ( (u16_BlueLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualBlueFreq   = 10499  ; 
      //
      if ( (u16_BlueLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualBlueFreq   = 2624  ;       
    }
#endif    
    //
 #ifdef HPU_V2_DEVICE   
    if(u8_DeviceRole == HIGH_POWER_DEVICE)
    {
      u16_ActualBluePeriod = u16_BlueLightFreqPeriod_HP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_BlueLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualBlueFreq   = 65535  ; 
      //
      if ( (u16_BlueLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualBlueFreq   = 10499  ; 
      //
      if ( (u16_BlueLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualBlueFreq   = 2624  ;       
    }
#endif    
    //  
    //
    b_UpdateBlueLightLevel = true;
  }
}
//
//
void SetUvLevel(uint16_t u16_LightIndex)
{
  //
  //u32_LightLevelUv = u16_LightIndex;
  //
  // Glow channel works in three modes of the PWM period.
  // 65535 - for the most accurate indication at the beginning of the range
  // 4 kHz period - some later after the start
  // 16 kHz period - main range.
  //
  // luminescence index specifies the period passed as out-load to the timer
  // And in parallel, the timer frequency is updated.
  // So, we are working with two pairs of values:
  //  - timer period, (0-65536, i.e. uint16_t)
  //  - timer frequency (0-65536, i.e. uint16_t)
  if ( b_UvLightLevelUpdated )
  {
    b_UvLightLevelUpdated = false;
    //
#ifdef LPU_V2_DEVICE  
    if(u8_DeviceRole == LOW_POWER_DEVICE)
    {
      u16_ActualUvPeriod = u16_UvLightFreqPeriod_LP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_UvLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualUvFreq   = 65535  ; 
      //
      if ( (u16_UvLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualUvFreq   = 10499  ; 
      //
      if ( (u16_UvLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualUvFreq   = 2624  ;       
    }
#endif
    //
#ifdef MPU_V2_DEVICE      
    if(u8_DeviceRole == MID_POWER_DEVICE)
    {
      u16_ActualUvPeriod = u16_UvLightFreqPeriod_MP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_UvLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualUvFreq   = 65535  ; 
      //
      if ( (u16_UvLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualUvFreq   = 10499  ; 
      //
      if ( (u16_UvLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualUvFreq   = 2624  ;       
    }
#endif    
    //
#ifdef HPU_V2_DEVICE    
    if(u8_DeviceRole == HIGH_POWER_DEVICE)
    {
      u16_ActualUvPeriod = u16_UvLightFreqPeriod_HP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_UvLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualUvFreq   = 65535  ; 
      //
      if ( (u16_UvLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualUvFreq   = 10499  ; 
      //
      if ( (u16_UvLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualUvFreq   = 2624  ;       
    }
 #endif   
    b_UpdateUvLightLevel = true;
  }
}
//
//
void SetWhiteLevel(uint16_t u16_LightIndex)
{
  //
  //u32_LightLevelWhite = u16_LightIndex;
  //
  // Glow channel works in three modes of the PWM period.
  // 65535 - for the most accurate indication at the beginning of the range
  // 4 kHz period - some later after the start
  // 16 kHz period - main range.
  //
  // luminescence index specifies the period passed as out-load to the timer
  // And in parallel, the timer frequency is updated.
  // So, we are working with two pairs of values:
  //  - timer period, (0-65536, i.e. uint16_t)
  //  - timer frequency (0-65536, i.e. uint16_t)
  if ( b_WhiteLightLevelUpdated )
  {
    b_WhiteLightLevelUpdated = false;
    //
#ifdef LPU_V2_DEVICE  
    if(u8_DeviceRole == LOW_POWER_DEVICE)
    {
      u16_ActualWhitePeriod = u16_WhiteLightFreqPeriod_LP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_WhiteLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualWhiteFreq   = 65535  ; 
      //
      if ( (u16_WhiteLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualWhiteFreq   = 10499  ; 
      //
      if ( (u16_WhiteLightFreqPeriod_LP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualWhiteFreq   = 2624  ;       
    }
#endif    
    //
#ifdef MPU_V2_DEVICE     
    if(u8_DeviceRole == MID_POWER_DEVICE)
    {
      u16_ActualWhitePeriod = u16_WhiteLightFreqPeriod_MP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_WhiteLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualWhiteFreq   = 65535  ; 
      //
      if ( (u16_WhiteLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualWhiteFreq   = 10499  ; 
      //
      if ( (u16_WhiteLightFreqPeriod_MP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualWhiteFreq   = 2624  ;       
    }
#endif     
    //
#ifdef HPU_V2_DEVICE      
    if(u8_DeviceRole == HIGH_POWER_DEVICE)
    {
      u16_ActualWhitePeriod = u16_WhiteLightFreqPeriod_HP[u16_LightIndex] & 0x3FFF ; 
      //
      if ( (u16_WhiteLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_65535)
        u16_ActualWhiteFreq   = 65535  ; 
      //
      if ( (u16_WhiteLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_10499)
        u16_ActualWhiteFreq   = 10499  ; 
      //
      if ( (u16_WhiteLightFreqPeriod_HP[u16_LightIndex]&0xC000) == LIGHT_LEVEL_PERIOD_2624)
        u16_ActualWhiteFreq   = 2624  ;       
    }
#endif    
    //
    b_UpdateWhiteLightLevel = true;
  }
}
//
/*
extern int16_t i_R_10perc ; // = ( (  RED_MAX_INDEX - RED_OFFSET)/10);
extern int16_t i_G_10perc ; // = ( (GREEN_MAX_INDEX - GRN_OFFSET)/10);
extern int16_t i_B_10perc ; // = ( ( BLUE_MAX_INDEX - BLU_OFFSET)/10);
extern int16_t i_U_10perc ; // = ( (   UV_MAX_INDEX -  UV_OFFSET)/10);
extern int16_t i_W_10perc ; // = ( (WHITE_MAX_INDEX - WHT_OFFSET)/10);
*/
//
//
void TurnOnPattern(void)
{
  //
  i16_LightLevelRed_AM   = i16_R_10perc*3 ; // 100; // ~10-20 %
  i16_LightLevelGreen_AM = i16_G_10perc*3 ; // 100; // ~10-20 %
  i16_LightLevelBlue_AM  = i16_B_10perc*3 ; // 100; // ~10-20 %
  i16_LightLevelUv_AM    = i16_U_10perc*3 ; // 100; // ~10-20 %
  i16_LightLevelWhite_AM = i16_W_10perc*3 ; // 100; // ~10-20 %
  //
  // разгоняем и гасим  Red
  u32_Delay_ms = 1;
  for (           u32_LightLevelRed = 0;  u32_LightLevelRed < i16_LightLevelRed_AM; u32_LightLevelRed +=10 ) 
  {  SetRedLevel (u32_LightLevelRed);  LL_mDelay(u32_Delay_ms);   }   LL_mDelay(5); 
  //
 // u32_Delay_ms = 1;
  for (                      ;  u32_LightLevelRed>0; u32_LightLevelRed -=10 )  
  { SetRedLevel (u32_LightLevelRed); LL_mDelay(u32_Delay_ms);   }  LL_mDelay(5); 
  //
  // 
  // разгоняем и гасим  Green
//  u32_Delay_ms = 2;
  for (             u32_LightLevelGreen = 0;  u32_LightLevelGreen<i16_LightLevelGreen_AM; u32_LightLevelGreen +=10 ) 
  {  SetGreenLevel (u32_LightLevelGreen);  LL_mDelay(u32_Delay_ms);   }   LL_mDelay(5); 
  //
 // u32_Delay_ms = 1;
  for (                                    ;  u32_LightLevelGreen>0; u32_LightLevelGreen -=10 ) 
  { SetGreenLevel (u32_LightLevelGreen); LL_mDelay(u32_Delay_ms);   }  LL_mDelay(5); 
  //
  // разгоняем и гасим  Blue
 // u32_Delay_ms = 1;
  for (            u32_LightLevelBlue = 0;  u32_LightLevelBlue<i16_LightLevelBlue_AM; u32_LightLevelBlue +=10 ) 
  {  SetBlueLevel (u32_LightLevelBlue);  LL_mDelay(u32_Delay_ms);   }   LL_mDelay(5); 
  //
 // u32_Delay_ms = 1;
  for (                               ;  u32_LightLevelBlue>0; u32_LightLevelBlue -=10 ) 
  { SetBlueLevel (u32_LightLevelBlue); LL_mDelay(u32_Delay_ms);   }  LL_mDelay(5); 
  //
  // разгоняем и гасим   UV  
 // u32_Delay_ms = 1;
  for (          u32_LightLevelUv = 0;  u32_LightLevelUv<i16_LightLevelUv_AM; u32_LightLevelUv +=10 ) 
  {  SetUvLevel (u32_LightLevelUv);  LL_mDelay(u32_Delay_ms);   }   LL_mDelay(5); 
  //
//  u32_Delay_ms = 1;
  for (                              ;  u32_LightLevelUv>0; u32_LightLevelUv -=10 ) 
  { SetUvLevel (u32_LightLevelUv); LL_mDelay(u32_Delay_ms);   }  LL_mDelay(5);
  //
  // разгоняем и гасим   White 
 // u32_Delay_ms = 1;
  for (             u32_LightLevelWhite = 0;  u32_LightLevelWhite<i16_LightLevelWhite_AM; u32_LightLevelWhite +=10 ) 
  {  SetWhiteLevel (u32_LightLevelWhite);  LL_mDelay(u32_Delay_ms);   }   LL_mDelay(5); 
  //
 // u32_Delay_ms = 1;
  for (                                    ;  u32_LightLevelWhite>0; u32_LightLevelWhite -=10 ) 
  { SetWhiteLevel (u32_LightLevelWhite); LL_mDelay(u32_Delay_ms);   }  LL_mDelay(5); 
  // 
  // обнуляем яркость всех каналов
  i16_LightLevelRed_AM   = 0; // RED_OFFSET ; // 100; // ~10-20 %
  i16_LightLevelGreen_AM = 0; // GRN_OFFSET ; // 100; // ~10-20 %
  i16_LightLevelBlue_AM  = 0; // BLU_OFFSET ; // 100; // ~10-20 %
  i16_LightLevelUv_AM    = 0; // UV_OFFSET ; // 100; // ~10-20 %
  i16_LightLevelWhite_AM = 0; // WHT_OFFSET ; // 100; // ~10-20 %
  //
  
}//
//
extern bool b_TurnOffPatternActive;
extern bool b_Grouped ;
//
// MODE_BYTE_IR_CMD
extern bool b_Send_ESP32_IR_CMD_Packet;
// MODE_BYTE_BRT_CMD
extern bool b_Send_ESP32_BRT_CMD_Packet ;
//
// Для изменения режима
extern bool b_Send_ESP32_OP_MODE_CMD_Packet ;
//
extern bool b_Send_ESP32_LED7_Packet ;
//
// Это 3 полубайта для задания режима и его параметра
extern uint8_t u8_MODE_State   ; // режим
extern uint8_t u8_MODE_Param_1 ; // старшая половина параметра
extern uint8_t u8_MODE_Param_2 ; // младшая половина параметра
//
extern bool b_FromWiFi ;
extern bool b_BRT_Update ;
extern bool b_Restore_AM  ;
//
void TurnOffPattern(void)
{
  /* Instant off — zero all channels immediately.
   * Previously used a gradual ramp-down (1ms per step) which blocked the main
   * loop for up to ~300ms, causing mode button presses to be missed (T7).
   * Side effect: no fade-out animation on mode switch. Acceptable trade-off. */
  b_TurnOffPatternActive = true;
  //
  u32_Mode_Sqnt_SM  = SM_MODE_RED_UP;
  u32_Mode_Rnbw_SM  = SM_MODE_RED_UP;
  //
  u32_LightLevelRed   = 0;
  u32_LightLevelGreen = 0;
  u32_LightLevelBlue  = 0;
  u32_LightLevelUv    = 0;
  u32_LightLevelWhite = 0;
  //
  b_RedLightLevelUpdated   = true; SetRedLevel  (0);
  b_GreenLightLevelUpdated = true; SetGreenLevel(0);
  b_BlueLightLevelUpdated  = true; SetBlueLevel (0);
  b_UvLightLevelUpdated    = true; SetUvLevel   (0);
  b_WhiteLightLevelUpdated = true; SetWhiteLevel(0);
  //
  b_TurnOffPatternActive = false;
}
//
void ProcessPowerUp(void)
{
  if (u8_StateMaschine == SM_MODE_STAND_BY )
  { // Переход из выключенного состояний во включенное
    u8_StateMaschine = SM_MODE_MANUAL;//SM_MODE_TURN_ON;
    //
    if ( b_Grouped )
    {
      u8_MODE_State   = MODE_BYTE_IR_CMD;
      //
      u8_MODE_Param_1 = IR_CMD_ON ; //  #define IR_CMD_ON      4
      u8_MODE_Param_2 = MODE_PARAM_HIGH_NIBBLE_IDLE ; 
      //
      b_Send_ESP32_IR_CMD_Packet = true;
    }
  }
}
//
void ProcessPowerDown(void)
{
  // Плавно, в параллельном режиме, выключаем все каналы свечения
  // Переходим в ожидание
  u8_StateMaschine = SM_MODE_STAND_BY;//SM_MODE_TURN_OFF;
  //
  if ( b_Grouped  && (b_FromWiFi == false) )
  {
    u8_MODE_State   = MODE_BYTE_IR_CMD;
    //
    u8_MODE_Param_1 = IR_CMD_OFF ; // #define IR_CMD_OFF     3
    u8_MODE_Param_2 = MODE_PARAM_HIGH_NIBBLE_IDLE ;  // младшая половина параметра
    //
    b_Send_ESP32_IR_CMD_Packet = true;
  }
  //
  if ( b_FromWiFi ==  true )
  {
       b_FromWiFi  = false ;    
       //
       u8_LED7_State = LED_7_KBD_CLICK;
       b_Send_ESP32_LED7_Packet = true;
  }
  //
}
//
// эта команда передается шерез поток команд управления яркостью, а не ИК поток.
void ProcessLightUp(void)
{
   u8_ManualButton = MANUAL_MODE_BUTTON_LIGHT_UP;
}
//
// эта команда передается шерез поток команд управления яркостью, а не ИК поток.
void ProcessLightDown(void)
{
  u8_ManualButton = MANUAL_MODE_BUTTON_LIGHT_DOWN;
}

//
bool    b_DirectLightControl = false;
uint32_t u32_ActualChannel = _ACTUAL_RED;

//
// Это массив значений процента яркости от 0 до 100
// массив задается при приеме сообщения BRT_X_YYY или при прямом нажатии на Up Down
int32_t i32_ActualPerc[5]  = { 0, 0, 0, 0, 0};
//
// Когда происходит выключение канала, то формируется BRT_X_000, что в итоге приводит к тому,
// что в массив i32_ActualPerc будет записан 0 и последующее нажатие на кнопку канала не приведет к его активации
// Для хранения яркости до выключенного состояний использую массив хранения ActualMax
int32_t i32_ActualPerc_AM[5]  = { 0, 0, 0, 0, 0};
//
/* Forward declaration — defined later in animation engine section */
static uint16_t PerceptualToRaw(uint16_t u16_Perc, uint16_t u16_MaxIdx,
                                 uint8_t u8_IsUV, uint16_t u16_Zone3Start);

/* Pre-computed raw indices for strobe interrupt */
uint16_t u16_StrobeRaw_RED   = 0u;
uint16_t u16_StrobeRaw_GREEN = 0u;
uint16_t u16_StrobeRaw_BLUE  = 0u;
uint16_t u16_StrobeRaw_UV    = 0u;
uint16_t u16_StrobeRaw_WHITE = 0u;

/* Update strobe raw indices from current GlobalBrightMax.
 * Call whenever u16_GlobalBrightMax changes. */
void UpdateStrobeRawIndices(void);
void UpdateStrobeRawIndices(void)
{
    u16_StrobeRaw_RED   = PerceptualToRaw(u16_GlobalBrightMax, RED_MAX_INDEX,   0u, 1230u);
    u16_StrobeRaw_GREEN = PerceptualToRaw(u16_GlobalBrightMax, GREEN_MAX_INDEX, 0u, 1449u);
    u16_StrobeRaw_BLUE  = PerceptualToRaw(u16_GlobalBrightMax, BLUE_MAX_INDEX,  0u, 1449u);
    u16_StrobeRaw_UV    = PerceptualToRaw(u16_GlobalBrightMax, UV_MAX_INDEX,    1u, 1449u);
    u16_StrobeRaw_WHITE = PerceptualToRaw(u16_GlobalBrightMax, WHITE_MAX_INDEX, 0u, 1449u);
}

void  DirectLightCtrl( int32_t i32_Channel, int32_t i32_Perc)
{
  /* Smoothly ramp the selected channel to the target brightness level.
   * i32_Perc is 0-100 (Manual percent scale). Converted to 0-1000 for
   * PerceptualToRaw() so Manual uses the same gamma curve as the Fade
   * engine — ensuring matched lux output at equivalent brightness steps.
   * Each ramp step maps the raw index back to perceptual scale for output.
   * IWDG kicked every step — ramp can take up to ~3s at full range. */
  static uint32_t u32_Delay_ms = 1;
  uint16_t u16_Perc1000 = (uint16_t)((i32_Perc < 0 ? 0 : i32_Perc > 100 ? 100 : i32_Perc) * 10);

  if ( i32_Channel == _ACTUAL_RED )
  {
    i16_LightLevelRed_AM = (int16_t)PerceptualToRaw(u16_Perc1000, RED_MAX_INDEX, 0u, 1230u);
    if (i16_LightLevelRed_AM > RED_MAX_INDEX-2)   i16_LightLevelRed_AM = RED_MAX_INDEX-2;
    if (i16_LightLevelRed_AM < 0)                 i16_LightLevelRed_AM = 0;
    if ((uint32_t)i16_LightLevelRed_AM > u32_LightLevelRed)
    { // Ramp up
      for (; u32_LightLevelRed < (uint32_t)i16_LightLevelRed_AM; u32_LightLevelRed++)
        { b_RedLightLevelUpdated=true; UpdateRedLevelFast_MP((uint16_t)u32_LightLevelRed); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
    else
    { // Ramp down
      for (; u32_LightLevelRed > (uint32_t)i16_LightLevelRed_AM; u32_LightLevelRed--)
        { b_RedLightLevelUpdated=true; UpdateRedLevelFast_MP((uint16_t)u32_LightLevelRed); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
  }
  else
  if ( i32_Channel == _ACTUAL_GRN )
  {
    i16_LightLevelGreen_AM = (int16_t)PerceptualToRaw(u16_Perc1000, GREEN_MAX_INDEX, 0u, 1449u);
    if (i16_LightLevelGreen_AM > GREEN_MAX_INDEX-2) i16_LightLevelGreen_AM = GREEN_MAX_INDEX-2;
    if (i16_LightLevelGreen_AM < 0)                 i16_LightLevelGreen_AM = 0;
    if ((uint32_t)i16_LightLevelGreen_AM > u32_LightLevelGreen)
    { // Ramp up
      for (; u32_LightLevelGreen < (uint32_t)i16_LightLevelGreen_AM; u32_LightLevelGreen++)
        { b_GreenLightLevelUpdated=true; UpdateGreenLevelFast_MP((uint16_t)u32_LightLevelGreen); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
    else
    { // Ramp down
      for (; u32_LightLevelGreen > (uint32_t)i16_LightLevelGreen_AM; u32_LightLevelGreen--)
        { b_GreenLightLevelUpdated=true; UpdateGreenLevelFast_MP((uint16_t)u32_LightLevelGreen); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
  }
  else
  if ( i32_Channel == MANUAL_MODE_BLUE )
  {
    i16_LightLevelBlue_AM = (int16_t)PerceptualToRaw(u16_Perc1000, BLUE_MAX_INDEX, 0u, 1449u);
    if (i16_LightLevelBlue_AM > BLUE_MAX_INDEX-2)   i16_LightLevelBlue_AM = BLUE_MAX_INDEX-2;
    if (i16_LightLevelBlue_AM < 0)                  i16_LightLevelBlue_AM = 0;
    if ((uint32_t)i16_LightLevelBlue_AM > u32_LightLevelBlue)
    { // Ramp up
      for (; u32_LightLevelBlue < (uint32_t)i16_LightLevelBlue_AM; u32_LightLevelBlue++)
        { b_BlueLightLevelUpdated=true; UpdateBlueLevelFast_MP((uint16_t)u32_LightLevelBlue); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
    else
    { // Ramp down
      for (; u32_LightLevelBlue > (uint32_t)i16_LightLevelBlue_AM; u32_LightLevelBlue--)
        { b_BlueLightLevelUpdated=true; UpdateBlueLevelFast_MP((uint16_t)u32_LightLevelBlue); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
  }
  else
  if ( i32_Channel == MANUAL_MODE_UV )
  {
    /* UV uses a different gamma curve (gamma 1.8 vs 2.2) — u8_IsUV=1 */
    i16_LightLevelUv_AM = (int16_t)PerceptualToRaw(u16_Perc1000, UV_MAX_INDEX, 1u, 1449u);
    if (i16_LightLevelUv_AM > UV_MAX_INDEX-2)       i16_LightLevelUv_AM = UV_MAX_INDEX-2;
    if (i16_LightLevelUv_AM < 0)                    i16_LightLevelUv_AM = 0;
    if ((uint32_t)i16_LightLevelUv_AM > u32_LightLevelUv)
    { // Ramp up
      for (; u32_LightLevelUv < (uint32_t)i16_LightLevelUv_AM; u32_LightLevelUv++)
        { b_UvLightLevelUpdated=true; UpdateUvLevelFast_MP((uint16_t)u32_LightLevelUv); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
    else
    { // Ramp down
      for (; u32_LightLevelUv > (uint32_t)i16_LightLevelUv_AM; u32_LightLevelUv--)
        { b_UvLightLevelUpdated=true; UpdateUvLevelFast_MP((uint16_t)u32_LightLevelUv); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
  }
  else
  if ( i32_Channel == MANUAL_MODE_WHITE )
  {
    /* White channel: hardware-disabled (overvoltage fault DD1/VT1).
     * Code retained for completeness — will not drive hardware. */
    i16_LightLevelWhite_AM = (int16_t)PerceptualToRaw(u16_Perc1000, WHITE_MAX_INDEX, 0u, 1449u);
    if (i16_LightLevelWhite_AM > WHITE_MAX_INDEX-2) i16_LightLevelWhite_AM = WHITE_MAX_INDEX-2;
    if (i16_LightLevelWhite_AM < 0)                 i16_LightLevelWhite_AM = 0;
    if ((uint32_t)i16_LightLevelWhite_AM > u32_LightLevelWhite)
    { // Ramp up
      for (; u32_LightLevelWhite < (uint32_t)i16_LightLevelWhite_AM; u32_LightLevelWhite++)
        { SetWhiteLevel((uint16_t)u32_LightLevelWhite); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
    else
    { // Ramp down
      for (; u32_LightLevelWhite > (uint32_t)i16_LightLevelWhite_AM; u32_LightLevelWhite--)
        { SetWhiteLevel((uint16_t)u32_LightLevelWhite); LL_mDelay(u32_Delay_ms); IWDG->KR = 0xAAAA; }
    }
  }
}
//
//
  
//
void ProcessRedButton     (void)
{ 
  u8_ManualButton = MANUAL_MODE_BUTTON_RED;
}
//
void ProcessGreenButton   (void)
{
  u8_ManualButton = MANUAL_MODE_BUTTON_GREEN;
}
//
void ProcessBlueButton    (void)
{
  u8_ManualButton = MANUAL_MODE_BUTTON_BLUE;
}
//
void ProcessUvButton      (void)
{
   u8_ManualButton = MANUAL_MODE_BUTTON_UV;
}
//
void ProcessWhiteButton   (void)
{
  u8_ManualButton = MANUAL_MODE_BUTTON_WHITE;
}
//
// 
// на ик пульте это S - Верхняя
void Process_Mode_Stroboscope_Button(void)
{
 // рано u8_ManualButton = MANUAL_MODE_BUTTON_STROBOSCOPE; 

  //
  if ( b_Grouped && (b_FromWiFi == false) )
  {
    u8_MODE_State   = MODE_BYTE_IR_CMD;
    //
    u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ; // 
    u8_MODE_Param_2 = IR_CMD_STRB ;  // #define IR_CMD_S1      16
    //                                  #define IR_CMD_STRB  IR_CMD_S1 
    //
    b_Send_ESP32_IR_CMD_Packet = true;
  }
  if ( b_Grouped  == false)
    u8_ManualButton = MANUAL_MODE_BUTTON_STROBOSCOPE; 
  //
  if ( b_FromWiFi == true )
       b_FromWiFi  = false ; 
}
//
// на ик пульте это  S - нижняя
void Process_Mode_Sequential_Button (void)
{
  // рано u8_ManualButton = MANUAL_MODE_BUTTON_SEQUENTIAL;
  //
  if ( b_Grouped && (b_FromWiFi == false) )
  {
    u8_MODE_State   = MODE_BYTE_IR_CMD;
    //
    u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ; //
    u8_MODE_Param_2 = IR_CMD_SEQ ;  // #define IR_CMD_GRN3    18
    //
    b_Send_ESP32_IR_CMD_Packet = true;
  }
  if ( b_Grouped  == false)
    u8_ManualButton = MANUAL_MODE_BUTTON_SEQUENTIAL;
  //
  if ( b_FromWiFi == true )
       b_FromWiFi  = false ;
}
//
// на ик пульте это  F - самая нижняя
void Process_Mode_Rainbow_Button (void)
{
  // рано u8_ManualButton = MANUAL_MODE_BUTTON_RAINBOW;
  //
  if ( b_Grouped && (b_FromWiFi == false))
  {
    u8_MODE_State   = MODE_BYTE_IR_CMD;
    //
    u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ; // 
    u8_MODE_Param_2 = IR_CMD_RNB ;  // #define IR_CMD_GRN4    22  
    //
    b_Send_ESP32_IR_CMD_Packet = true;
  } 
  //
  if ( b_Grouped  == false)
    u8_ManualButton = MANUAL_MODE_BUTTON_RAINBOW;  
  //
  if ( b_FromWiFi == true )
       b_FromWiFi  = false ;
}
//
// на ик пульте это  Сиреневая
void Process_Mode_Manual_Button (void)
{
  // рано u8_ManualButton = MANUAL_MODE_BUTTON_MANUAL;
  //
  if ( b_Grouped && (b_FromWiFi == false))
  {
    u8_MODE_State   = MODE_BYTE_IR_CMD;
    //
    u8_MODE_Param_1 = MODE_PARAM_LOW_NIBBLE_IDLE  ; // 
    u8_MODE_Param_2 = IR_CMD_MNL ;  // #define IR_CMD_MNL     IR_CMD_PRPL3
    //                                  #define IR_CMD_F2      24
    //
    b_Send_ESP32_IR_CMD_Packet = true;
  } 
  //
  if ( b_Grouped  == false)
    u8_ManualButton = MANUAL_MODE_BUTTON_MANUAL;  
  //
  if ( b_FromWiFi == true )
       b_FromWiFi  = false ;
}
//-
// В качестве напоминания как работает передача команд с пультика или ИК-пульта...
// Нажимаем кнопку на пультике, она перехватывается обработчкиком. 
// Обработчик формирует щироковещательный пакет и отправляет его в сеть.
// В сети он передается и принимается всеми. И будучи принятым из сети - имплементируется.
// В случае   активации сервисного режима (путем ПРЕССА на МОД) - нужно игнорировать команды 
// от ИК пульта и от сети.  ХОТЯ, в будушем, не исключена возможность дистанционного запуска
// сервисного режима.
// В данном случае - напрямую активируем режим сервисного управления и подрежим задания 
// роли коммуникационного модуля.
void Process_Mode_SERVICE (void)
{
  // быстро, за 0,5 секунду на каждый, зажигаем, гасим каждый канал
  TurnOffPattern();
  //
  // Нужно отправить в модуль сообщение.
  // модуль должен принять сообщение и отправить его в виде консольного сообщения в контроллер.
  // Контроллер принимает сообщение и меняет режим - переводит машину в сервисный, 
  // то есть   u8_StateMaschine = SM_MODE_SERVICE ;
  //
  // Независимо от роли и нахождения в сети устройства или нет  - в модуль отправляется команда
  //
  u8_MODE_State   = MODE_BYTE_OP_MOD_CMD ; //MODE_BYTE_IR_CMD;
  //
  u8_MODE_Param_1 = OP_MODE_CMD_SERVICE ; // MODE_PARAM_LOW_NIBBLE_IDLE  ; 
  u8_MODE_Param_2 = u32_ActualNetRole;  
  //
  b_Send_ESP32_OP_MODE_CMD_Packet = true;
  //
  // Будет сформирован пакет, отправлен в модуль и обработан в сервисе static void rx_task().
  // В потоке приемника он будет принят и изменится режим работы модуля, с регулар на сервис.
  // При этом - изменится режим индикации. Начнут мигать зеленые светодиоды.
  // Далее, нажимая UpClick меняем роль устройства (Роль контроллера+роль модуля), что отражается
  // на свечении синих светодиодов. 
}

void Process_ChangeRoleCommand ( void )
{
  // Вызывается после UpClick в сервисном режиме - управление ролью.
  // Возможны варианты:
  // #define ESP32_NET_ROLE_MASTER     1 
  // #define ESP32_NET_ROLE_ECHOSLAVE  2
  // #define ESP32_NET_ROLE_SLAVE      3 
  // #define ESP32_NET_ROLE_UNDEF      4   
  // Инкрементируем состояние роли
  u32_ActualNetRole ++ ; 
  //
  if (u32_ActualNetRole > ESP32_NET_ROLE_SLAVE)
    u32_ActualNetRole = ESP32_NET_ROLE_DEFAULT; // ESP32_NET_ROLE_MASTER
  //      
  u8_MODE_State   = MODE_BYTE_OP_MOD_CMD ; //MODE_BYTE_IR_CMD;
  //
  u8_MODE_Param_1 = OP_MODE_CMD_SERVICE ; // MODE_PARAM_LOW_NIBBLE_IDLE  ; 
  u8_MODE_Param_2 = u32_ActualNetRole;  
  //
  b_Send_ESP32_OP_MODE_CMD_Packet = true;
  
}
//
//
uint32_t u32_Mode_Sqnt_SM            =      SM_MODE_RED_UP;  
uint32_t u32_Mode_Rnbw_SM            =      SM_MODE_RED_UP;  

//uint8_t u8_Mode_A_Activity      = PROCESS_MODE_A_PLAY;
//  
// uint16_t u16_LightLevel=20; // 0 

void Process_Mode_Sequential(void)
{  
  //
  if (b_UpdateSetSpeed)
  {
    b_UpdateSetSpeed = false;
                             AutoModeActualSpeed = u8_SetSpeedValue;
    u8_Mode_Sqnt_StepDelay_ms = AutoModeActualSpeed;
  }
  //
  //
  if ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN )
  {
    // Перенесено в прерывания
  }
  else
    if ( u8_Mode_Sqnt_Status == MODE_STATUS_STOP )
     LL_mDelay(1) ;
}
  
  
void Process_Mode_Rainbow(void)
{
  // Фактически - болванка. 
  // Вся работа идет в прерываниях.
}
//  
//
//
uint8_t u8_Mode_Strobe_SM = SM_MODE_STROB_ON;  
// 
void Process_Mode_Stroboscope(void)
{
  return ;
  //  
  if ( u8_Mode_Strob_Status == MODE_STATUS_RUN )
  {
    //+++ switch (u8_Mode_Strobe_SM)
    if (u8_Mode_Strobe_SM == SM_MODE_STROB_ON)
    {
      //
      //+++ case SM_MODE_STROB_ON:
        // Dead code — Process_Mode_Stroboscope() returns immediately at entry.
        // Active strobe output runs in stm32f4xx_it.c using u16_StrobeRaw_*.
        // Updated for consistency: use pre-computed perceptual raw indices.
        if ( u8_ManualChannel == MANUAL_MODE_RED   )   SetRedLevel   ( u16_StrobeRaw_RED   ); else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN )   SetGreenLevel ( u16_StrobeRaw_GREEN ); else
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  )   SetBlueLevel  ( u16_StrobeRaw_BLUE  ); else
        if ( u8_ManualChannel == MANUAL_MODE_UV    )   SetUvLevel    ( u16_StrobeRaw_UV    ); else
        if ( u8_ManualChannel == MANUAL_MODE_WHITE )   SetWhiteLevel ( u16_StrobeRaw_WHITE );
        //
        LL_mDelay(u16_Mode_Strob_StepDelay_A_ms); //+++   u8_Mode_Strobe_SM = SM_MODE_STROB_OFF;   
        //
       //+++  break;
      //
      //+++ case SM_MODE_STROB_OFF:
        if ( u8_ManualChannel == MANUAL_MODE_RED   )   SetRedLevel   ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN )   SetGreenLevel ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  )   SetBlueLevel  ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_UV    )   SetUvLevel    ( 0 );  else       
        if ( u8_ManualChannel == MANUAL_MODE_WHITE )   SetWhiteLevel ( 0 );    
        //
        LL_mDelay(u16_Mode_Strob_StepDelay_B_ms);  
        //+++ u8_Mode_Strobe_SM = SM_MODE_STROB_ALL_DOWN; 
        //
       //+++  break;        
      //
      //+++ case SM_MODE_STROB_ALL_DOWN:
        SetRedLevel   (0); 
        SetGreenLevel (0);  
        SetBlueLevel  (0); 
        SetUvLevel    (0);  
        SetWhiteLevel (0); 
        //
        LL_mDelay(u16_Mode_Strob_StepDelay_C_ms);  
        //
    //+++     if (u8_SyncRole == SYNC_ROLE_MASTER)
    //+++        u8_ProcessWiFiCommand = WIFI_COMMAND_SET_SINGLE_STROBE; //u8_Mode_Strobe_SM = SM_MODE_STROB_ON;
        //

        u8_Mode_Strobe_SM = SM_MODE_STROB_IDLE;
        // 
        //+++ break;  
      //  
      //+++ case SM_MODE_STROB_IDLE:
      //+++   LL_mDelay(1);        
      //+++   break;  
      //  
      //+++ default:         u8_Mode_Strobe_SM = SM_MODE_STROB_IDLE;  break;
      //
    }
  }
}
  /*  
  if ( u8_Mode_Strob_Status == MODE_STATUS_RUN )
  {
    switch (u8_Mode_Strobe_SM)
    {
      //
      case SM_MODE_STROB_ON:
        // Use PerceptualToRaw() + Update*LevelFast_MP() to match Fade/Manual path.
        // Force flag before each call to guarantee hardware update lands.
        if ( u8_ManualChannel == MANUAL_MODE_RED   ) { b_RedLightLevelUpdated=true;
            UpdateRedLevelFast_MP  (PerceptualToRaw(u16_GlobalBrightMax, RED_MAX_INDEX,   0u, 1230u)); } else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN ) { b_GreenLightLevelUpdated=true;
            UpdateGreenLevelFast_MP(PerceptualToRaw(u16_GlobalBrightMax, GREEN_MAX_INDEX, 0u, 1449u)); } else
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  ) { b_BlueLightLevelUpdated=true;
            UpdateBlueLevelFast_MP (PerceptualToRaw(u16_GlobalBrightMax, BLUE_MAX_INDEX,  0u, 1449u)); } else
        if ( u8_ManualChannel == MANUAL_MODE_UV    ) { b_UvLightLevelUpdated=true;
            UpdateUvLevelFast_MP   (PerceptualToRaw(u16_GlobalBrightMax, UV_MAX_INDEX,    1u, 1449u)); } else
        if ( u8_ManualChannel == MANUAL_MODE_WHITE ) { b_WhiteLightLevelUpdated=true;
            UpdateWhiteLevelFast_MP(PerceptualToRaw(u16_GlobalBrightMax, WHITE_MAX_INDEX, 0u, 1449u)); }
        //
        LL_mDelay(u16_Mode_Strob_StepDelay_A_ms);   u8_Mode_Strobe_SM = SM_MODE_STROB_OFF;   
        //
        break;
      //
      case SM_MODE_STROB_OFF:
        if ( u8_ManualChannel == MANUAL_MODE_RED   )   SetRedLevel   ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN )   SetGreenLevel ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  )   SetBlueLevel  ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_UV    )   SetUvLevel    ( 0 );  else       
        if ( u8_ManualChannel == MANUAL_MODE_WHITE )   SetWhiteLevel ( 0 );    
        //
        LL_mDelay(u16_Mode_Strob_StepDelay_B_ms);  
        u8_Mode_Strobe_SM = SM_MODE_STROB_ALL_DOWN; 
        //
        break;        
      //
      case SM_MODE_STROB_ALL_DOWN:
        SetRedLevel   (0); 
        SetGreenLevel (0);  
        SetBlueLevel  (0); 
        SetUvLevel    (0);  
        SetWhiteLevel (0); 
        //
        LL_mDelay(u16_Mode_Strob_StepDelay_C_ms);  
        //
        if (u8_SyncRole == SYNC_ROLE_MASTER)
           u8_ProcessWiFiCommand = WIFI_COMMAND_SET_SINGLE_STROBE; //u8_Mode_Strobe_SM = SM_MODE_STROB_ON;
        //

        u8_Mode_Strobe_SM = SM_MODE_STROB_IDLE;
        // 
        break;  
      //  
      case SM_MODE_STROB_IDLE:
        LL_mDelay(1);        
        break;  
      //  
      default:         u8_Mode_Strobe_SM = SM_MODE_STROB_IDLE;  break;
      //
    }
  }
  */
//
//
/*
extern uint16_t   RED_MAX_INDEX  ;
extern uint16_t GREEN_MAX_INDEX  ;
extern uint16_t  BLUE_MAX_INDEX  ;
extern uint16_t    UV_MAX_INDEX  ;
extern uint16_t WHITE_MAX_INDEX  ;*/
//
//                           (2120     - 20        )/10 = 2100/10= 210
int16_t i16_R_10perc ;//= ( (  RED_MAX_INDEX - RED_OFFSET)/10);
int16_t i16_G_10perc ;//= ( (GREEN_MAX_INDEX - GRN_OFFSET)/10);
int16_t i16_B_10perc ;//= ( ( BLUE_MAX_INDEX - BLU_OFFSET)/10);
int16_t i16_U_10perc ;//= ( (   UV_MAX_INDEX -  UV_OFFSET)/10);
int16_t i16_W_10perc ;//= ( (WHITE_MAX_INDEX - WHT_OFFSET)/10);
//  
//
void RestoreActualMax (void);
void RestoreActualMax (void)
{
  /* Restore AM hardware indices using PerceptualToRaw() to match the Fade engine
   * gamma curve. Replaces old linear formula (i16_G_10perc * percent / PRCNT_SHIFT)
   * which produced different brightness than PerceptualToRaw at equivalent settings.
   * i32_ActualPerc_AM[] is 0-100; multiply by 10 for PerceptualToRaw 0-1000 scale. */
  i16_LightLevelRed_AM   = (int16_t)PerceptualToRaw((uint16_t)(i32_ActualPerc_AM[_ACTUAL_RED] * 10), RED_MAX_INDEX,   0u, 1230u);
  i16_LightLevelGreen_AM = (int16_t)PerceptualToRaw((uint16_t)(i32_ActualPerc_AM[_ACTUAL_GRN] * 10), GREEN_MAX_INDEX, 0u, 1449u);
  i16_LightLevelBlue_AM  = (int16_t)PerceptualToRaw((uint16_t)(i32_ActualPerc_AM[_ACTUAL_BLU] * 10), BLUE_MAX_INDEX,  0u, 1449u);
  i16_LightLevelUv_AM    = (int16_t)PerceptualToRaw((uint16_t)(i32_ActualPerc_AM[_ACTUAL_UV ] * 10), UV_MAX_INDEX,    1u, 1449u);
  i16_LightLevelWhite_AM = (int16_t)PerceptualToRaw((uint16_t)(i32_ActualPerc_AM[_ACTUAL_WHT] * 10), WHITE_MAX_INDEX, 0u, 1449u);
  //
  u32_LightLevelRed   = 0;
  u32_LightLevelGreen = 0;
  u32_LightLevelBlue  = 0;
  u32_LightLevelUv    = 0;
  u32_LightLevelWhite = 0;
}
//
void SetActualMax (void);
void SetActualMax (void)
{
    // Set amplitude values scaled to global brightness level
  int32_t i32_Perc = (int32_t)u16_GlobalBrightMax / 10; // 0-100
  if (i32_Perc < 10) i32_Perc = 10; // floor at 10%
  //
  i32_ActualPerc_AM [_ACTUAL_RED] = i32_Perc ;     
  i32_ActualPerc_AM [_ACTUAL_GRN] = i32_Perc ; 
  i32_ActualPerc_AM [_ACTUAL_BLU] = i32_Perc ; 
  i32_ActualPerc_AM [_ACTUAL_UV ] = i32_Perc ; 
  i32_ActualPerc_AM [_ACTUAL_WHT] = (i32_Perc * 40) / 100; // White at 40% of global
 
  // 
  RestoreActualMax ();
}
//
extern bool    b_FS_ModeDefined  ;
extern uint8_t u8_FS_Mode        ;

extern bool    b_FS_ParamDefined[4] ;
extern uint8_t u8_FS_R_Perc ;
extern uint8_t u8_FS_G_Perc ;
extern uint8_t u8_FS_B_Perc ;
extern uint8_t u8_FS_U_Perc ;
extern uint8_t u8_FS_W_Perc ;
//
// Для стробоскопа
extern uint8_t u8_FS_Strobe_ActCh ;

extern bool    b_FS_Implemented  ;
//
void ProcessMainStateMaschine(void)
{
  static int32_t i32_BrtPercTemp=0;
  //
  // Выполняем коррекцию режима для только-что включенного узла
  // если работает как подчиненный В группе
  if ( (                                 b_Grouped                    )                &&
       ( ( u8_LED2_State == LED_2_ECHO_SLAVE ) || ( u8_LED2_State == LED_2_SLAVE )   )     )
  if (b_FS_Implemented == false)// если Начальная коррекция не выполенне
  {
    //
    if ( (u8_FS_Mode == FS_MODE_MANUAL   )&& ( b_FS_ParamDefined[FS_PARAM_MANUAL]==true) )// Если мастер в ручном режиме
    {
      u8_FS_Mode = FS_MODE_IDLE;
      b_FS_ParamDefined[FS_PARAM_MANUAL] = false;
      b_FS_Implemented = true;// Начальная коррекция выполнена
      //
      i32_ActualPerc_AM[MANUAL_MODE_RED  ]= i32_ActualPerc[MANUAL_MODE_RED  ] = u8_FS_R_Perc ;
      i32_ActualPerc_AM[MANUAL_MODE_GREEN]= i32_ActualPerc[MANUAL_MODE_GREEN] = u8_FS_G_Perc ;
      i32_ActualPerc_AM[MANUAL_MODE_BLUE ]= i32_ActualPerc[MANUAL_MODE_BLUE ] = u8_FS_B_Perc ;
      i32_ActualPerc_AM[MANUAL_MODE_UV   ]= i32_ActualPerc[MANUAL_MODE_UV   ] = u8_FS_U_Perc ;
      i32_ActualPerc_AM[MANUAL_MODE_WHITE]= i32_ActualPerc[MANUAL_MODE_WHITE] = u8_FS_W_Perc ;  
      //
      DirectLightCtrl(_ACTUAL_RED, u8_FS_R_Perc);
      DirectLightCtrl(_ACTUAL_GRN, u8_FS_G_Perc);
      DirectLightCtrl(_ACTUAL_BLU, u8_FS_B_Perc);
      DirectLightCtrl(_ACTUAL_UV,  u8_FS_U_Perc);
      DirectLightCtrl(_ACTUAL_WHT, u8_FS_W_Perc);
      //
       u8_StateMaschine = SM_MODE_MANUAL ; //u8_ManualButton = MANUAL_MODE_BUTTON_MANUAL ; // 
    }
    if ( (u8_FS_Mode == FS_MODE_STROBE   )&& ( b_FS_ParamDefined[FS_PARAM_STROBE]==true) )// Если мастер в стробоскопе
    {
      u8_FS_Mode = FS_MODE_IDLE;
      b_FS_ParamDefined[FS_PARAM_MANUAL] = false;
      b_FS_Implemented = true;// Начальная коррекция выполнена
      //
      //
      u8_ManualChannel = u8_FS_Strobe_ActCh ;
      //
      b_Restore_AM = true;      
      u8_ManualButton = MANUAL_MODE_BUTTON_STROBOSCOPE ; //  u8_StateMaschine = SM_MODE_STROBOSCOPE ;  

    }
    //
    if ( u8_FS_Mode == FS_MODE_SEQUENTIAL    )// Если мастер в последовательном
    {
      u8_FS_Mode = FS_MODE_IDLE;
      b_FS_ParamDefined[FS_PARAM_MANUAL] = false;
      b_FS_Implemented = true;// Начальная коррекция выполнена
      //
      u8_ManualButton = MANUAL_MODE_BUTTON_SEQUENTIAL;//u8_StateMaschine = SM_MODE_SEQUENTIAL ;  
    }
    //
    if ( u8_FS_Mode == FS_MODE_RAINBOW   )// Если мастер в радуге
    {
      u8_FS_Mode = FS_MODE_IDLE;
      b_FS_ParamDefined[FS_PARAM_MANUAL] = false;
      b_FS_Implemented = true;// Начальная коррекция выполнена
      //
      u32_Mode_Rnbw_SM = SM_MODE_RED_UP;  
      b_Restore_AM = true;
      u8_ManualButton = MANUAL_MODE_BUTTON_RAINBOW; //u8_StateMaschine = SM_MODE_RAINBOW ;
    }
  }
 /**/
  // Перехватываем запрос активации заданного режима (ручной и три автоматических)
  if ( (u8_ManualButton == MANUAL_MODE_BUTTON_STROBOSCOPE ) || 
       (u8_ManualButton == MANUAL_MODE_BUTTON_SEQUENTIAL  ) || 
       (u8_ManualButton == MANUAL_MODE_BUTTON_RAINBOW     ) ||
       (u8_ManualButton == MANUAL_MODE_BUTTON_MANUAL      )    )
  {
      // Если активирован хотя-бы один из ручных режимов (А ОН ВКЛЮЧЕН, т.к. это обработчик ручного режима)  - отключаем и гасим все.
      TurnOffPattern();
      //
      u32_LightLevelRed   = 0;
      u32_LightLevelGreen = 0;
      u32_LightLevelBlue  = 0;
      u32_LightLevelUv    = 0;
      u32_LightLevelWhite = 0;
      //  
  // ПОНИЖЕ
  // Отключаем светодиод синхронизации при смене режима
  //   u8_LED3_State = LED_3_NoSync;  
      //
      // Останавливаем остальные режима
      u8_Mode_Strob_Status = MODE_STATUS_STOP;
      u8_Mode_Sqnt_Status  = MODE_STATUS_STOP;
      u8_Mode_Rnbw_Status  = MODE_STATUS_STOP;
      u8_Mode_Manl_Status  = MODE_STATUS_STOP;
      //
      // Меняем машину состояний
      if ( u8_ManualButton == MANUAL_MODE_BUTTON_STROBOSCOPE )
      {               u8_StateMaschine = SM_MODE_STROBOSCOPE; 
                      //
                      u32_Mode_Sqnt_SM  = SM_MODE_RED_UP;
                      /* Sync global brightness from Manual before SetActualMax() runs */
                      u16_GlobalBrightMax = (uint16_t)(i32_ActualPerc[u8_ManualChannel] * 10u);
                      if (u16_GlobalBrightMax > 1000u) u16_GlobalBrightMax = 1000u;
                      if (u16_GlobalBrightMax < 100u)  u16_GlobalBrightMax = 100u;
                      b_Restore_AM = true;
                      //
                      u8_Mode_Strob_Status= MODE_STATUS_RUN;  
      }    
      //
      if ( u8_ManualButton == MANUAL_MODE_BUTTON_SEQUENTIAL      ) 
      {        
        // Нажимаем кнопку вызова последовательного режима, с пульта.
        // Мастер определяет в каком режиме он находится - в паре с ведомым или автономно.
        // ели нет связи - производится автостарт. 
        //
        u8_Mode_Strob_Status = MODE_STATUS_STOP ;
        //
        u8_StateMaschine     = SM_MODE_SEQUENTIAL; 
        SysTick->VAL = 8400000; // 100 мс.
        /* Sync global brightness from Manual and set AM indices immediately */
        u16_GlobalBrightMax = (uint16_t)(i32_ActualPerc[u8_ManualChannel] * 10u);
        if (u16_GlobalBrightMax > 1000u) u16_GlobalBrightMax = 1000u;
        if (u16_GlobalBrightMax < 100u)  u16_GlobalBrightMax = 100u;
        SetActualMax();
        //
        if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
        {
          u8_Mode_Sqnt_Status  = MODE_STATUS_RUN       ; // начинаем выполнение
          //u8_Mode_Sqnt_Request = SEQ_MODE_REQUEST_IDLE ; // Запрос не отправляем
        }
        //
        if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
        {
          u8_Mode_Sqnt_Status  = MODE_STATUS_STOP ;                // НЕ начинаем выполнение
          // Для синхростарта была реализована архитектура, при которой каналы включаются и выключаются заранее
          // заданными сообщениями:
          // RS_R_ON RS_R_OFF,
          // RS_G_ON RS_G_OFF,
          // RS_B_ON RS_B_OFF,
          // RS_U_ON RS_U_OFF,
          // RS_W_ON RS_W_OFF,
        }                        
      }
      //
      // Отключаем светодиод синхронизации при смене режима
      u8_LED3_State = LED_3_NoSync;  
      b_Grouped = false;
      //
      //
      if ( u8_ManualButton == MANUAL_MODE_BUTTON_RAINBOW      )
      {   // Работает по аналогии с обработчиком кнопки последовательного режима
          //
          // Нажимаем кнопку вызова Rainbow режима, с пульта.
          // Мастер определяет в каком режиме он находится - в паре с ведомым или автономно.
          // ели нет связи - производится автостарт. 
          //
          u8_Mode_Strob_Status = MODE_STATUS_STOP ;
          //
          u8_StateMaschine     = SM_MODE_RAINBOW; 
          SysTick->VAL = 8400000; // 100 мс.
          /* Sync global brightness from Manual and set AM indices immediately */
          u16_GlobalBrightMax = (uint16_t)(i32_ActualPerc[u8_ManualChannel] * 10u);
          if (u16_GlobalBrightMax > 1000u) u16_GlobalBrightMax = 1000u;
          if (u16_GlobalBrightMax < 100u)  u16_GlobalBrightMax = 100u;
          SetActualMax();
          //
          if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
          {
            u8_Mode_Rnbw_Status  = MODE_STATUS_RUN       ; // начинаем выполнение
            //u8_Mode_Sqnt_Request = SEQ_MODE_REQUEST_IDLE ; // Запрос не отправляем
          }
          //
          if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
          {
            u8_Mode_Rnbw_Status  = MODE_STATUS_STOP ;                // НЕ начинаем выполнение
            // Для синхростарта была реализована архитектура, при которой каналы включаются и выключаются заранее
            // заданными сообщениями:
            // RS_R_ON RS_R_OFF,
            // RS_G_ON RS_G_OFF,
            // RS_B_ON RS_B_OFF,
            // RS_U_ON RS_U_OFF,
            // RS_W_ON RS_W_OFF,   
          }                       
      }
      //
      if ( u8_ManualButton == MANUAL_MODE_BUTTON_MANUAL      )
      {              u8_StateMaschine = SM_MODE_MANUAL; 
           u8_Mode_Manl_Status= MODE_STATUS_RUN;  
           //
           u8_Mode_Strob_Status = MODE_STATUS_STOP ;
           //
           /* Restore active channel brightness on return to Manual.
            * i32_ActualPerc[] is the reliable source — updated on every Up/Down
            * press in Manual mode. i32_ActualPerc_AM[] is NOT used here as it
            * is only updated on channel button press, not brightness changes.
            * Zero hardware counters first — animation modes drive Set*Level()
            * directly without updating u32_LightLevel*, leaving them stale.
            * Without zeroing, DirectLightCtrl() ramps from wrong position.
            * White omitted: hardware-disabled (overvoltage fault DD1/VT1).   */
           u32_LightLevelRed   = 0;
           u32_LightLevelGreen = 0;
           u32_LightLevelBlue  = 0;
           u32_LightLevelUv    = 0;
           u32_LightLevelWhite = 0;
           /* Fast restore on mode transition — jump directly to target level.
            * DirectLightCtrl() ramps over ~3s which blocks button processing (T7).
            * On mode entry, instant restore is acceptable. Up/Down still uses
            * the full smooth ramp via DirectLightCtrl(). */
           { uint16_t u16_Target = PerceptualToRaw(
                 (uint16_t)(i32_ActualPerc[u8_ManualChannel] * 10),
                 (u8_ManualChannel == MANUAL_MODE_RED  ) ? RED_MAX_INDEX   :
                 (u8_ManualChannel == MANUAL_MODE_GREEN) ? GREEN_MAX_INDEX :
                 (u8_ManualChannel == MANUAL_MODE_BLUE ) ? BLUE_MAX_INDEX  : UV_MAX_INDEX,
                 (u8_ManualChannel == MANUAL_MODE_UV   ) ? 1u : 0u,
                 (u8_ManualChannel == MANUAL_MODE_RED  ) ? 1230u : 1449u);
             if (u8_ManualChannel == MANUAL_MODE_RED  ) { u32_LightLevelRed   = u16_Target; b_RedLightLevelUpdated  =true; UpdateRedLevelFast_MP  (u16_Target); }
             if (u8_ManualChannel == MANUAL_MODE_GREEN) { u32_LightLevelGreen = u16_Target; b_GreenLightLevelUpdated=true; UpdateGreenLevelFast_MP(u16_Target); }
             if (u8_ManualChannel == MANUAL_MODE_BLUE ) { u32_LightLevelBlue  = u16_Target; b_BlueLightLevelUpdated =true; UpdateBlueLevelFast_MP (u16_Target); }
             if (u8_ManualChannel == MANUAL_MODE_UV   ) { u32_LightLevelUv    = u16_Target; b_UvLightLevelUpdated   =true; UpdateUvLevelFast_MP   (u16_Target); }
             /* Sync AM index so subsequent Up/Down ramps start from correct position */
             i16_LightLevelRed_AM   = (u8_ManualChannel == MANUAL_MODE_RED  ) ? (int16_t)u16_Target : i16_LightLevelRed_AM;
             i16_LightLevelGreen_AM = (u8_ManualChannel == MANUAL_MODE_GREEN) ? (int16_t)u16_Target : i16_LightLevelGreen_AM;
             i16_LightLevelBlue_AM  = (u8_ManualChannel == MANUAL_MODE_BLUE ) ? (int16_t)u16_Target : i16_LightLevelBlue_AM;
             i16_LightLevelUv_AM    = (u8_ManualChannel == MANUAL_MODE_UV   ) ? (int16_t)u16_Target : i16_LightLevelUv_AM;
           }
      }
      //
      //  This flag blocks strobe handler from processing its own button while mode-switching
      u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
      //
  }
  //
  //
  // РЕЖИМ 3. ПООЧЕРЕДНОЕ ПЛАВНОЕ.
  if (  u8_StateMaschine ==  SM_MODE_SEQUENTIAL     ) // 150
  {
    if ( b_Restore_AM )
    {
      b_Restore_AM = false ;
      //
      SetActualMax (); //RestoreActualMax();     
    }
    //
    if ( u8_ManualButton == MANUAL_MODE_BUTTON_SEQUENTIAL )
    {
      u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
      // Toggle Mode status
      u8_Mode_Sqnt_Status++; u8_Mode_Sqnt_Status&=0x01;
      //
      i8_GlobalDutyIndex    = 0;
      i32_GlobalDutyCounter = 0;
    }
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_UP )
    {   u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
        i16_LightLevelRed_AM   += i16_R_10perc; if (i16_LightLevelRed_AM   > RED_MAX_INDEX)   i16_LightLevelRed_AM   = RED_MAX_INDEX-2;
        i16_LightLevelGreen_AM += i16_G_10perc; if (i16_LightLevelGreen_AM > GREEN_MAX_INDEX) i16_LightLevelGreen_AM = GREEN_MAX_INDEX-2;
        i16_LightLevelBlue_AM  += i16_B_10perc; if (i16_LightLevelBlue_AM  > BLUE_MAX_INDEX)  i16_LightLevelBlue_AM  = BLUE_MAX_INDEX-2;
        i16_LightLevelUv_AM    += i16_U_10perc; if (i16_LightLevelUv_AM    > UV_MAX_INDEX)    i16_LightLevelUv_AM    = UV_MAX_INDEX-2;
        i16_LightLevelWhite_AM += i16_W_10perc; if (i16_LightLevelWhite_AM > WHITE_MAX_INDEX) i16_LightLevelWhite_AM = WHITE_MAX_INDEX-2;
        if (u16_GlobalBrightMax <= 950u) u16_GlobalBrightMax += 50u; else u16_GlobalBrightMax = 1000u;
      /* Sync i32_ActualPerc[] so Manual mode inherits animation brightness */
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      { int32_t i32_P = (int32_t)u16_GlobalBrightMax / 10;
        if (i32_P > 100) i32_P = 100; if (i32_P < 0) i32_P = 0;
        i32_ActualPerc[MANUAL_MODE_RED  ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_GREEN] = i32_P;
        i32_ActualPerc[MANUAL_MODE_BLUE ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_UV   ] = i32_P; }
    }
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_DOWN )
    {   u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
        i16_LightLevelRed_AM   -= i16_R_10perc; if (i16_LightLevelRed_AM   < 1) i16_LightLevelRed_AM   = 1;
        i16_LightLevelGreen_AM -= i16_G_10perc; if (i16_LightLevelGreen_AM < 1) i16_LightLevelGreen_AM = 1;
        i16_LightLevelBlue_AM  -= i16_B_10perc; if (i16_LightLevelBlue_AM  < 1) i16_LightLevelBlue_AM  = 1;
        i16_LightLevelUv_AM    -= i16_U_10perc; if (i16_LightLevelUv_AM    < 1) i16_LightLevelUv_AM    = 1;
        i16_LightLevelWhite_AM -= i16_W_10perc; if (i16_LightLevelWhite_AM < 1) i16_LightLevelWhite_AM = 1;
        if (u16_GlobalBrightMax > 200u) u16_GlobalBrightMax -= 50u; else u16_GlobalBrightMax = 200u;
      /* Sync i32_ActualPerc[] so Manual mode inherits animation brightness */
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      { int32_t i32_P = (int32_t)u16_GlobalBrightMax / 10;
        if (i32_P > 100) i32_P = 100; if (i32_P < 0) i32_P = 0;
        i32_ActualPerc[MANUAL_MODE_RED  ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_GREEN] = i32_P;
        i32_ActualPerc[MANUAL_MODE_BLUE ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_UV   ] = i32_P; }
    }

      /* Recover any fade channels stuck at zero after rapid Down presses */
      { uint8_t fi;
        for (fi=0u; fi<5u; fi++) {
            if (g_Fade[fi].i32_PercX10 > (int32_t)(u16_GlobalBrightMax * 10u))
                g_Fade[fi].i32_PercX10 = (int32_t)(u16_GlobalBrightMax * 10u);
            if (g_Fade[fi].i32_PercX10 <= 0 && g_Fade[fi].u8_Dir == 0u) {
                g_Fade[fi].i32_PercX10 = 0;
                g_Fade[fi].u8_Dir      = 1u;
                g_Fade[fi].u32_SubStepAccum = 0u;
            }
        }
      }
    u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF; /* clear any unhandled button */
    Process_Mode_Sequential();
    return;
  } 
  else
  //
  if (u8_StateMaschine == SM_MODE_TURN_ON)
  {
    // быстро, за 0,5 секунду на каждый, зажигаем, гасим каждый канал
    TurnOnPattern();
    // переходим в режим ручной
    u8_StateMaschine = SM_MODE_MANUAL;

  }
  else
  if (u8_StateMaschine == SM_MODE_TURN_OFF)
  {
    // быстро, за 0,5 секунду на каждый, зажигаем, гасим каждый канал
    TurnOffPattern();
    // переходим в режим ручной
    u8_StateMaschine = SM_MODE_STAND_BY;
  }
    else
  // РЕЖИМ 2. СТОБОСКОП.
  if (  u8_StateMaschine ==   SM_MODE_STROBOSCOPE    ) //  100
  {
     if ( b_Restore_AM )
    {
      b_Restore_AM = false ;
      //
      SetActualMax (); //RestoreActualMax(); 
    }
    //
    if ( (u8_Mode_Strob_Status == MODE_STATUS_RUN)&& (u16_Mode_Strob_Counter_ms>=STROBE_REINIT_TIME_MS ) )
    {
      // Обнуляем таймер ожидания
      u16_Mode_Strob_SubCounter = 0;
      u16_Mode_Strob_Counter_ms=0;
      // вручную перезапускаем процесс
      u8_Mode_Strobe_SM = SM_MODE_STROB_ON; 
    }
    //
    // Adjust active channel and brightness level
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_UP )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
      //
      if (u8_ManualChannel == MANUAL_MODE_RED)
      {  i16_LightLevelRed_AM+=i16_R_10perc;   if (i16_LightLevelRed_AM>RED_MAX_INDEX)      i16_LightLevelRed_AM=RED_MAX_INDEX-2;     }
      //
      if (u8_ManualChannel == MANUAL_MODE_GREEN)
      {  i16_LightLevelGreen_AM+=i16_G_10perc; if (i16_LightLevelGreen_AM>GREEN_MAX_INDEX)  i16_LightLevelGreen_AM=GREEN_MAX_INDEX-2; }
      //
      if (u8_ManualChannel == MANUAL_MODE_BLUE)
      {  i16_LightLevelBlue_AM+=i16_B_10perc; if (i16_LightLevelBlue_AM>BLUE_MAX_INDEX)     i16_LightLevelBlue_AM=BLUE_MAX_INDEX-2;   }
      //
      if (u8_ManualChannel == MANUAL_MODE_UV)
      {  i16_LightLevelUv_AM+=i16_U_10perc; if (i16_LightLevelUv_AM>UV_MAX_INDEX)           i16_LightLevelUv_AM=UV_MAX_INDEX-2;       }
      //
      if (u8_ManualChannel == MANUAL_MODE_WHITE)
      {  i16_LightLevelWhite_AM+=i16_W_10perc; if (i16_LightLevelWhite_AM>WHITE_MAX_INDEX)  i16_LightLevelWhite_AM=WHITE_MAX_INDEX-2; }
      if (u16_GlobalBrightMax <= 950u) u16_GlobalBrightMax += 50u; else u16_GlobalBrightMax = 1000u;
      /* Sync i32_ActualPerc[] so Manual mode inherits animation brightness */
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      { int32_t i32_P = (int32_t)u16_GlobalBrightMax / 10;
        if (i32_P > 100) i32_P = 100; if (i32_P < 0) i32_P = 0;
        i32_ActualPerc[MANUAL_MODE_RED  ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_GREEN] = i32_P;
        i32_ActualPerc[MANUAL_MODE_BLUE ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_UV   ] = i32_P; }
    }
    // Decrease strobe brightness
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_DOWN )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
      //
      i16_LightLevelRed_AM-=i16_R_10perc;     if (i16_LightLevelRed_AM<(int16_t)1)       i16_LightLevelRed_AM=1;
      //
      i16_LightLevelGreen_AM-=i16_G_10perc;   if (i16_LightLevelGreen_AM<(int16_t)1)     i16_LightLevelGreen_AM=1;
      //
      i16_LightLevelBlue_AM-=i16_B_10perc;    if (i16_LightLevelBlue_AM<(int16_t)1)      i16_LightLevelBlue_AM=1;
      //
      i16_LightLevelUv_AM-=i16_U_10perc;      if (i16_LightLevelUv_AM<(int16_t)1)        i16_LightLevelUv_AM=1;
      //
      i16_LightLevelWhite_AM-=i16_W_10perc;   if (i16_LightLevelWhite_AM<(int16_t)1)     i16_LightLevelWhite_AM=1;
      if (u16_GlobalBrightMax > 200u) u16_GlobalBrightMax -= 50u; else u16_GlobalBrightMax = 200u;
      /* Sync i32_ActualPerc[] so Manual mode inherits animation brightness */
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      { int32_t i32_P = (int32_t)u16_GlobalBrightMax / 10;
        if (i32_P > 100) i32_P = 100; if (i32_P < 0) i32_P = 0;
        i32_ActualPerc[MANUAL_MODE_RED  ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_GREEN] = i32_P;
        i32_ActualPerc[MANUAL_MODE_BLUE ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_UV   ] = i32_P; }
      /* Clamp all fade channels to new ceiling and recover any stuck-dark channels.
       * Rapid Down presses can force i32_PercX10 to zero with Dir=down, leaving
       * channels permanently silenced. Reset Dir to UP to rejoin the animation. */
      { uint8_t fi;
        for (fi=0u; fi<5u; fi++) {
            if (g_Fade[fi].i32_PercX10 > (int32_t)(u16_GlobalBrightMax * 10u))
                g_Fade[fi].i32_PercX10 = (int32_t)(u16_GlobalBrightMax * 10u);
            if (g_Fade[fi].i32_PercX10 <= 0 && g_Fade[fi].u8_Dir == 0u) {
                g_Fade[fi].i32_PercX10 = 0;
                g_Fade[fi].u8_Dir      = 1u;
            }
        }
      }
    }
    //
    // Change strobe/animation channel
    if (u8_ManualButton == MANUAL_MODE_BUTTON_RED )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;   u8_ManualChannel = MANUAL_MODE_RED ;     }  
    //
    if (u8_ManualButton == MANUAL_MODE_BUTTON_GREEN )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;   u8_ManualChannel = MANUAL_MODE_GREEN ;   } 
    //     
    if (u8_ManualButton == MANUAL_MODE_BUTTON_BLUE )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;   u8_ManualChannel = MANUAL_MODE_BLUE ;    }
    //     
    if (u8_ManualButton == MANUAL_MODE_BUTTON_UV )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;   u8_ManualChannel = MANUAL_MODE_UV ;      } 
    //    
    if (u8_ManualButton == MANUAL_MODE_BUTTON_WHITE )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;   u8_ManualChannel = MANUAL_MODE_WHITE ;   }  
    //
    //
    // Управлние самим стробоскопом. Включение и выключение через пульт
    if ( u8_ManualButton == MANUAL_MODE_BUTTON_STROBOSCOPE )
    {
      u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
      // Toggle Mode status
      // Фактически u8_Mode_Strob_Status переводим в состояние  MODE_STATUS_STOP
      u8_Mode_Strob_Status++; u8_Mode_Strob_Status&=0x01;
      //
      u8_Mode_Strobe_SM = SM_MODE_STROB_ON;
      //
      i8_GlobalDutyIndex    = 0;
      i32_GlobalDutyCounter = 0;
    }
    Process_Mode_Stroboscope();
    //return;
  }
  //
  else
 /* перенесено вверх 
   // РЕЖИМ 3. ПООЧЕРЕДНОЕ ПЛАВНОЕ.
  else*/
  // РЕЖИМ 4. РАДУГА.
  if (  u8_StateMaschine ==  SM_MODE_RAINBOW     )
  {
    if ( b_Restore_AM )
    {
      b_Restore_AM = false ;
      SetActualMax ();
    }
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_UP )
    {   u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
        i16_LightLevelRed_AM   += i16_R_10perc; if (i16_LightLevelRed_AM   > RED_MAX_INDEX)   i16_LightLevelRed_AM   = RED_MAX_INDEX-2;
        i16_LightLevelGreen_AM += i16_G_10perc; if (i16_LightLevelGreen_AM > GREEN_MAX_INDEX) i16_LightLevelGreen_AM = GREEN_MAX_INDEX-2;
        i16_LightLevelBlue_AM  += i16_B_10perc; if (i16_LightLevelBlue_AM  > BLUE_MAX_INDEX)  i16_LightLevelBlue_AM  = BLUE_MAX_INDEX-2;
        i16_LightLevelUv_AM    += i16_U_10perc; if (i16_LightLevelUv_AM    > UV_MAX_INDEX)    i16_LightLevelUv_AM    = UV_MAX_INDEX-2;
        i16_LightLevelWhite_AM += i16_W_10perc; if (i16_LightLevelWhite_AM > WHITE_MAX_INDEX) i16_LightLevelWhite_AM = WHITE_MAX_INDEX-2;
        if (u16_GlobalBrightMax <= 950u) u16_GlobalBrightMax += 50u; else u16_GlobalBrightMax = 1000u;
      /* Sync i32_ActualPerc[] so Manual mode inherits animation brightness */
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      { int32_t i32_P = (int32_t)u16_GlobalBrightMax / 10;
        if (i32_P > 100) i32_P = 100; if (i32_P < 0) i32_P = 0;
        i32_ActualPerc[MANUAL_MODE_RED  ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_GREEN] = i32_P;
        i32_ActualPerc[MANUAL_MODE_BLUE ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_UV   ] = i32_P; }
    }
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_DOWN )
    {   u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
        i16_LightLevelRed_AM   -= i16_R_10perc; if (i16_LightLevelRed_AM   < 1) i16_LightLevelRed_AM   = 1;
        i16_LightLevelGreen_AM -= i16_G_10perc; if (i16_LightLevelGreen_AM < 1) i16_LightLevelGreen_AM = 1;
        i16_LightLevelBlue_AM  -= i16_B_10perc; if (i16_LightLevelBlue_AM  < 1) i16_LightLevelBlue_AM  = 1;
        i16_LightLevelUv_AM    -= i16_U_10perc; if (i16_LightLevelUv_AM    < 1) i16_LightLevelUv_AM    = 1;
        i16_LightLevelWhite_AM -= i16_W_10perc; if (i16_LightLevelWhite_AM < 1) i16_LightLevelWhite_AM = 1;
        if (u16_GlobalBrightMax > 200u) u16_GlobalBrightMax -= 50u; else u16_GlobalBrightMax = 200u;
      /* Sync i32_ActualPerc[] so Manual mode inherits animation brightness */
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      { int32_t i32_P = (int32_t)u16_GlobalBrightMax / 10;
        if (i32_P > 100) i32_P = 100; if (i32_P < 0) i32_P = 0;
        i32_ActualPerc[MANUAL_MODE_RED  ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_GREEN] = i32_P;
        i32_ActualPerc[MANUAL_MODE_BLUE ] = i32_P;
        i32_ActualPerc[MANUAL_MODE_UV   ] = i32_P; }
    }

      /* Recover any fade channels stuck at zero after rapid Down presses */
      { uint8_t fi;
        for (fi=0u; fi<5u; fi++) {
            if (g_Fade[fi].i32_PercX10 > (int32_t)(u16_GlobalBrightMax * 10u))
                g_Fade[fi].i32_PercX10 = (int32_t)(u16_GlobalBrightMax * 10u);
            if (g_Fade[fi].i32_PercX10 <= 0 && g_Fade[fi].u8_Dir == 0u) {
                g_Fade[fi].i32_PercX10 = 0;
                g_Fade[fi].u8_Dir      = 1u;
                g_Fade[fi].u32_SubStepAccum = 0u;
            }
        }
      }
    u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF; /* clear any unhandled button */
  }    
  //
  // ВТОРОЙ ВАРИАНТ, 
  // с поддержкой радиосинхронизации.
  // Управление через клавиатуру и через пульт.
  // РЕЖИМ 1. РУЧНОЕ УПРАВЛЕНИЕ
  //
  //                                      1          2          3          4          5   
  static uint8_t u8_Param_1[5]= { BRT_CMD_R, BRT_CMD_G, BRT_CMD_B, BRT_CMD_U, BRT_CMD_W };  
  //
  if (  u8_StateMaschine == SM_MODE_MANUAL      ) 
  {
     if ( b_Restore_AM )
    {
      b_Restore_AM = false ;
      //
      RestoreActualMax(); 
    }
    // Если перехватили что-то из консоли
    if (b_BRT_Update == true)
    {
      b_BRT_Update = false;
      //
      DirectLightCtrl(u32_ActualChannel, i32_ActualPerc[u32_ActualChannel]);
    }
    //
    // Обработчики сигналов от ИК пульта ИЛИ от кнопок
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_UP )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
      //
      u32_ActualChannel = u8_ManualChannel;
      i32_ActualPerc[u32_ActualChannel] += PRCNT_SHIFT;
      if (i32_ActualPerc[u32_ActualChannel] > 100) 
          i32_ActualPerc[u32_ActualChannel] = 100;
      /* Sync global brightness so animation modes inherit Manual level.
       * Set directly from i32_ActualPerc to keep in lockstep. */
      u16_GlobalBrightMax = (uint16_t)(i32_ActualPerc[u32_ActualChannel] * 10u);
      if (u16_GlobalBrightMax > 1000u) u16_GlobalBrightMax = 1000u;
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      //
      if ( b_Grouped  ) 
      {
        u8_MODE_State   = MODE_BYTE_BRT_CMD;
        //
        u8_MODE_Param_1 =    u8_Param_1 [u32_ActualChannel]   ; // 1 .. 5       
        u8_MODE_Param_2 = i32_ActualPerc[u32_ActualChannel]/10; // 0, 1, ... 10. Что соотв 0, 10%, .. 100%. 
        //
        b_Send_ESP32_BRT_CMD_Packet = true;
      }
      else
        DirectLightCtrl(u32_ActualChannel, i32_ActualPerc[u32_ActualChannel]);
    }
    //
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_DOWN )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
      //
      u32_ActualChannel = u8_ManualChannel;
      i32_ActualPerc[u32_ActualChannel] -= PRCNT_SHIFT;
      if (i32_ActualPerc[u32_ActualChannel] < 0) 
          i32_ActualPerc[u32_ActualChannel] = 0;
      /* Sync global brightness so animation modes inherit Manual level.
       * Set directly from i32_ActualPerc to keep in lockstep. */
      u16_GlobalBrightMax = (uint16_t)(i32_ActualPerc[u32_ActualChannel] * 10u);
      /* Allow zero in Manual (LED fully off) but floor animations at 200 when active */
      if (u16_GlobalBrightMax > 0u && u16_GlobalBrightMax < 200u) u16_GlobalBrightMax = 200u;
      { uint8_t fi; for (fi=0u; fi<5u; fi++) g_Fade[fi].u16_BrightnessMax = u16_GlobalBrightMax; } UpdateStrobeRawIndices();
      //
      if ( b_Grouped  ) 
      {
        u8_MODE_State   = MODE_BYTE_BRT_CMD;
        //
        u8_MODE_Param_1 =    u8_Param_1 [u32_ActualChannel]   ; // 1 .. 5       
        u8_MODE_Param_2 = i32_ActualPerc[u32_ActualChannel]/10; // 0, 1, ... 10. Что соотв 0, 10%, .. 100%. 
        //
        b_Send_ESP32_BRT_CMD_Packet = true;
      }
      else
        DirectLightCtrl(u32_ActualChannel, i32_ActualPerc[u32_ActualChannel]);
    }
    //   
    if (u8_ManualButton == MANUAL_MODE_BUTTON_RED ) // РУЧНОЙ РЕЖИМ. Красная кнопка.
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
      // Если это повторное нажатие при уже включенном режиме и яркости более единицы, то выключает
      if( ( u8_StateMaschine == SM_MODE_MANUAL     ) )
      {
        // В ручном режиме, при нажатии на кнопку КРАСНЫЙ мы должны отправить или команду BRT_R_000 или BRT_R_xxx
        // Действующее значение яркости находится в переменной i16_LightLevelRed_AM
        // Заданное значение процента яркостиu i32_ActualPerc[32_ActualChannel] 
        // Если при нажатии на кнопку канала яркость не нулевая - касим канал
        // Если канал загашен, а активный процент не нелевой - отправляем текущий процент яркости
        u8_ManualChannel = MANUAL_MODE_RED   ; 
        u32_ActualChannel = u8_ManualChannel;
        //
        if ( i16_LightLevelRed_AM >= i16_R_10perc )  // Если при нажатии на кномку яркость была не нулевая
        {
          i32_ActualPerc_AM[u32_ActualChannel] = i32_ActualPerc[u32_ActualChannel];
          i32_BrtPercTemp = 0; // гасим яркость канала
        }      
        else
          i32_BrtPercTemp = i32_ActualPerc_AM[u32_ActualChannel]; // Восстанавливаем значение яркости после отключения.
        //
        if ( b_Grouped  ) 
        {
          u8_MODE_State   = MODE_BYTE_BRT_CMD;
          //
          u8_MODE_Param_1 =    u8_Param_1 [u32_ActualChannel]   ; // 1 .. 5       
          u8_MODE_Param_2 = i32_BrtPercTemp / 10 ; // 0, 1, ... 10. Что соотв 0, 10%, .. 100%. 
          //
          b_Send_ESP32_BRT_CMD_Packet = true;
        }
        else
          DirectLightCtrl(u32_ActualChannel, i32_BrtPercTemp); // i32_ActualPerc[u32_ActualChannel]
        //
      }  
    } //  if (u8_ManualButton == MANUAL_MODE_BUTTON_RED )
    //   
    if (u8_ManualButton == MANUAL_MODE_BUTTON_GREEN ) // РУЧНОЙ РЕЖИМ. ЗЕЛЕНА кнопочка.
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
      if( ( u8_StateMaschine == SM_MODE_MANUAL     ) )
      {
        u8_ManualChannel = MANUAL_MODE_GREEN ; 
        u32_ActualChannel = u8_ManualChannel;
        //
        if ( i16_LightLevelGreen_AM >= i16_G_10perc )  // Если при нажатии на кномку яркость была не нулевая
        {
          i32_ActualPerc_AM[u32_ActualChannel] = i32_ActualPerc[u32_ActualChannel];
          i32_BrtPercTemp = 0; // гасим яркость канала
        }      
        else
          i32_BrtPercTemp = i32_ActualPerc_AM[u32_ActualChannel];// Восстанавливаем значение яркости после отключения.
        //
        if ( b_Grouped  ) 
        {
          u8_MODE_State   = MODE_BYTE_BRT_CMD;
          //
          u8_MODE_Param_1 =    u8_Param_1 [u32_ActualChannel]   ; // 1 .. 5       
          u8_MODE_Param_2 = i32_BrtPercTemp / 10 ; // 0, 1, ... 10. Что соотв 0, 10%, .. 100%. 
          //
          b_Send_ESP32_BRT_CMD_Packet = true;
        }
        else
          DirectLightCtrl(u32_ActualChannel, i32_BrtPercTemp); // i32_ActualPerc[u32_ActualChannel]
        //
      }  
    } //  if (u8_ManualButton == MANUAL_MODE_BUTTON_Green )
    //      
    if (u8_ManualButton == MANUAL_MODE_BUTTON_BLUE ) // РУЧНОЙ РЕЖИМ. синяя кнопка.
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
       if( ( u8_StateMaschine == SM_MODE_MANUAL     ) )
      {
        // В ручном режиме, при нажатии на кнопку КРАСНЫЙ мы должны отправить или команду BRT_R_000 или BRT_R_xxx
        // Действующее значение яркости находится в переменной i16_LightLevelRed_AM
        // Заданное значение процента яркостиu i32_ActualPerc[32_ActualChannel] 
        // Если при нажатии на кнопку канала яркость не нулевая - касим канал
        // Если канал загашен, а активный процент не нелевой - отправляем текущий процент яркости
        u8_ManualChannel = MANUAL_MODE_BLUE ; 
        u32_ActualChannel = u8_ManualChannel;
        //
        if ( i16_LightLevelBlue_AM >= i16_B_10perc )  // Если при нажатии на кномку яркость была не нулевая
        {
          i32_ActualPerc_AM[u32_ActualChannel] = i32_ActualPerc[u32_ActualChannel];
          i32_BrtPercTemp = 0; // гасим яркость канала
        }      
        else
          i32_BrtPercTemp = i32_ActualPerc_AM[u32_ActualChannel];// Восстанавливаем значение яркости после отключения.
        //
        if ( b_Grouped  ) 
        {
          u8_MODE_State   = MODE_BYTE_BRT_CMD;
          //
          u8_MODE_Param_1 =    u8_Param_1 [u32_ActualChannel]   ; // 1 .. 5       
          u8_MODE_Param_2 = i32_BrtPercTemp / 10 ; // 0, 1, ... 10. Что соотв 0, 10%, .. 100%. 
          //
          b_Send_ESP32_BRT_CMD_Packet = true;
        }
        else
          DirectLightCtrl(u32_ActualChannel, i32_BrtPercTemp); // i32_ActualPerc[u32_ActualChannel]
        //
      }  
    } //  if (u8_ManualButton == MANUAL_MODE_BUTTON_Blue )
    // 
    //      
    if (u8_ManualButton == MANUAL_MODE_BUTTON_UV ) // РУЧНОЙ РЕЖИМ. УФ кнопка.
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
      if( ( u8_StateMaschine == SM_MODE_MANUAL     ) )
      {
        // В ручном режиме, при нажатии на кнопку КРАСНЫЙ мы должны отправить или команду BRT_R_000 или BRT_R_xxx
        // Действующее значение яркости находится в переменной i16_LightLevelRed_AM
        // Заданное значение процента яркостиu i32_ActualPerc[32_ActualChannel] 
        // Если при нажатии на кнопку канала яркость не нулевая - касим канал
        // Если канал загашен, а активный процент не нелевой - отправляем текущий процент яркости
        u8_ManualChannel = MANUAL_MODE_UV ; 
        u32_ActualChannel = u8_ManualChannel;
        //
        if ( i16_LightLevelUv_AM >= i16_U_10perc )  // Если при нажатии на кномку яркость была не нулевая
        {
          i32_ActualPerc_AM[u32_ActualChannel] = i32_ActualPerc[u32_ActualChannel];
          i32_BrtPercTemp = 0; // гасим яркость канала
        }      
        else
          i32_BrtPercTemp = i32_ActualPerc_AM[u32_ActualChannel];// /10;
        //
        if ( b_Grouped  ) 
        {
          u8_MODE_State   = MODE_BYTE_BRT_CMD;
          //
          u8_MODE_Param_1 =    u8_Param_1 [u32_ActualChannel]   ; // 1 .. 5       
          u8_MODE_Param_2 = i32_BrtPercTemp / 10 ;  // 0, 1, ... 10. Что соотв 0, 10%, .. 100%. 
          //
          b_Send_ESP32_BRT_CMD_Packet = true;
        }
        else
          DirectLightCtrl(u32_ActualChannel, i32_BrtPercTemp); // i32_ActualPerc[u32_ActualChannel]
        //
      }        
    } //  if (u8_ManualButton == MANUAL_MODE_BUTTON_Uv )
    // 
    //      
    if (u8_ManualButton == MANUAL_MODE_BUTTON_WHITE )  // РУЧНОЙ РЕЖИМ. БЕЛАЯ кнопка.
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
       if( ( u8_StateMaschine == SM_MODE_MANUAL     ) )
      {
        // В ручном режиме, при нажатии на кнопку КРАСНЫЙ мы должны отправить или команду BRT_R_000 или BRT_R_xxx
        // Действующее значение яркости находится в переменной i16_LightLevelRed_AM
        // Заданное значение процента яркостиu i32_ActualPerc[32_ActualChannel] 
        // Если при нажатии на кнопку канала яркость не нулевая - касим канал
        // Если канал загашен, а активный процент не нелевой - отправляем текущий процент яркости
        u8_ManualChannel = MANUAL_MODE_WHITE ; 
        u32_ActualChannel = u8_ManualChannel;
        //
        if ( i16_LightLevelWhite_AM >= i16_W_10perc )  // Если при нажатии на кномку яркость была не нулевая
        {
          i32_ActualPerc_AM[u32_ActualChannel] = i32_ActualPerc[u32_ActualChannel];
          i32_BrtPercTemp = 0; // гасим яркость канала
        }      
        else
          i32_BrtPercTemp = i32_ActualPerc_AM[u32_ActualChannel];// /10;
        //
        if ( b_Grouped  ) 
        {
          u8_MODE_State   = MODE_BYTE_BRT_CMD;
          //
          u8_MODE_Param_1 =    u8_Param_1 [u32_ActualChannel]   ; // 1 .. 5       
          u8_MODE_Param_2 = i32_BrtPercTemp / 10 ;  // 0, 1, ... 10. Что соотв 0, 10%, .. 100%. 
          //
          b_Send_ESP32_BRT_CMD_Packet = true;
        }
        else
          DirectLightCtrl(u32_ActualChannel, i32_BrtPercTemp); // i32_ActualPerc[u32_ActualChannel]
        //
      }   
    } //  if (u8_ManualButton == MANUAL_MODE_BUTTON_White )
    //
    // В конце обработчика ручного режима, проверяем флаг наличия перехвата из консоли модуля
    // если была принята известная команда, то формируем обратный пакет в модуль с индикацией нажатия на кнопку.
    if ( b_FromWiFi ==  true )
    {
       b_FromWiFi  = false ;    
       //
       u8_LED7_State = LED_7_KBD_CLICK;
       b_Send_ESP32_LED7_Packet = true;
    }
  } //  if (  u8_StateMaschine == SM_MODE_MANUAL      ) 
  //
  //
  // Внешнее управление
  if ( b_DirectLightControl == true)
  {
    b_DirectLightControl = false;
    //   
    DirectLightCtrl(u32_ActualChannel, i32_ActualPerc[u32_ActualChannel]);
  }
  //
  //
  // Сервисный режим
  if ( u8_StateMaschine == SM_MODE_SERVICE )
  {
    // В сервисном режиме отслеживаем нажатие на Up и меняем роль устройства в иерархии.
    // Выход из сервисного режима производится путем DoubleClick на Mode c последующей записью 
    // страницы виоса в ПЗУ.
    // В варианте BE Все станции обладают одним и тем-же SSID И пароль. В биосе хранится только роль.
    // 
    // Меняем режим модуля.
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_UP )
    {   u8_ManualButton  = MANUAL_MODE_BUTTON_UNDEF;
       // Определяем текущий режим модуля - мастер, эхослейв, слейв.
       // Изменяем на 1 уровень, отправляе в модуль.
       // Модуль принимает сообщение и изменяет индикацию 
    }
  }
  //
  if ( u8_StateMaschine == SM_MODE_SAVE_REBOOT )
  {
    // Должно быть все выключено, но для дополнительной надежности допослнительно выключаем
    // быстро, за 0,5 секунду на каждый, зажигаем, гасим каждый канал
    TurnOffPattern();
    // 
    // Текущий режим модуля Сохраняем в ПЗУ DIOS.
    SaveBios();
    //
    // перезагружаем.
    SysReboot();
    // 
  }  
}
//



// для чего?

uint32_t AutoReloadValue ;
/**
  * @brief  Configures the timer to generate a PWM signal on the OC1 output.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
//__STATIC_INLINE 
void  LLU_V2_RedChannelInit(void)
{
  // Enable the peripheral clock of GPIOs //
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  // 
  // GPIO TIM2_CH1 configuration //
  LL_GPIO_SetPinMode   (GPIOB, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE );
  LL_GPIO_SetPinPull   (GPIOB, LL_GPIO_PIN_9, LL_GPIO_PULL_DOWN      );
  LL_GPIO_SetPinSpeed  (GPIOB, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_9, LL_GPIO_AF_3           ); //LL_GPIO_SetAFPin_0_7 LL_GPIO_AF_2
  //
  // Configure the NVIC to handle TIM11 interrupt 
  NVIC_SetPriority( TIM1_TRG_COM_TIM11_IRQn, 2); //   0  TIM11_IRQn
  NVIC_EnableIRQ  ( TIM1_TRG_COM_TIM11_IRQn   ); //    TIM11_IRQn
  //
  LL_APB2_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM11 );  // LL_APB1_GRP1_PERIPH_TIM11
  
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  
  /* Set the pre-scaler value to have TIM2 counter clock equal to 40 MHz */
  LL_TIM_SetPrescaler(TIM11,  __LL_TIM_CALC_PSC(SystemCoreClock,40000000 ) ); //  10000
  
  /* Enable TIM2_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM11);
  
  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM2CLK = SystemCoreClock / (APB prescaler & multiplier)               */
  TimOutClock = SystemCoreClock/1;
  AutoReloadValue = 65535 ; // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100);
  LL_TIM_SetAutoReload(TIM11, AutoReloadValue); //650 14000
//LL_TIM_SetAutoReload(TIM11, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100));   //  1000 100
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM11, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
  
  /* Set output channel polarity */
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  //LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  
  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH1(TIM11, ( (LL_TIM_GetAutoReload(TIM11) + 1 ) / 2));
  
  /* Enable TIM2_CCR1 register preload. Read/Write operations access the      */
  /* preload register. TIM2_CCR1 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM11, LL_TIM_CHANNEL_CH1);
  // My
  LL_TIM_OC_SetCompareCH1(TIM11, 0); // No Light
  
  /**************************/
  /* TIM2 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC1(TIM11);

  /* Start output signal generation */
  // Enable output channel 1 //
  LL_TIM_CC_EnableChannel(TIM11, LL_TIM_CHANNEL_CH1);
  // Enable counter //
  LL_TIM_EnableCounter(TIM11);
  // Force update generation 
  LL_TIM_GenerateEvent_UPDATE(TIM11);
}

//__STATIC_INLINE 
void  LLU_V2_GreenChannelInit(void)
{
  //
  // GREEN_PWM PB0 TIM3_CH3
  //
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  
  /* GPIO TIM2_CH1 configuration */
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_0, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_0, LL_GPIO_AF_2); // LL_GPIO_AF_1
  
  /***********************************************/
  /* Configure the NVIC to handle TIM2 interrupt */
  /***********************************************/
  NVIC_SetPriority(TIM3_IRQn, 3);// 0 
  NVIC_EnableIRQ(TIM3_IRQn);
  
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); 
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  
  /* Set the pre-scaler value to have TIM2 counter clock equal to (10) kHz */
  LL_TIM_SetPrescaler(TIM3,  __LL_TIM_CALC_PSC(SystemCoreClock,40000000 ) ); //  10000
  
  /* Enable TIM2_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM3);
  
  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM2CLK = SystemCoreClock / (APB prescaler & multiplier)               */
  TimOutClock = SystemCoreClock/1;
  AutoReloadValue = 65535 ; // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100);
  LL_TIM_SetAutoReload(TIM3, AutoReloadValue); //650 14000
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM1);
  
  /* Set output channel polarity */
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  //LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  
  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH3(TIM3, ( (LL_TIM_GetAutoReload(TIM3) + 1 ) / 2));
  
  /* Enable TIM2_CCR1 register preload. Read/Write operations access the      */
  /* preload register. TIM2_CCR1 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH3);
  // My
  LL_TIM_OC_SetCompareCH3(TIM3, 0); // No Light

  
  /**************************/
  /* TIM2 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC3(TIM3);
  
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel 1 */
  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH3);
  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM3);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM3);
  //
}
//
//
//__STATIC_INLINE 
void  LLU_V2_BlueChannelInit(void)
{
  //
  // BLUE_PWM PA1 TIM2_CH2
  //
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  
  /* GPIO TIM2_CH1 configuration */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_1, LL_GPIO_AF_1); // LL_GPIO_AF_1
  
  /***********************************************/
  /* Configure the NVIC to handle TIM2 interrupt */
  /***********************************************/
  NVIC_SetPriority(TIM2_IRQn, 4); // 0
  NVIC_EnableIRQ(TIM2_IRQn);
  
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2); 
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  
  /* Set the pre-scaler value to have TIM2 counter clock equal to (10) kHz */
  LL_TIM_SetPrescaler(TIM2,  __LL_TIM_CALC_PSC(SystemCoreClock,40000000 ) ); //  10000
  
  /* Enable TIM2_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM2);
  // My
  LL_TIM_OC_SetCompareCH2(TIM2, 0); // No Light
  
  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM2CLK = SystemCoreClock / (APB prescaler & multiplier)               */
  TimOutClock = SystemCoreClock/1;
  AutoReloadValue = 65535 ; // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100);
  LL_TIM_SetAutoReload(TIM2, AutoReloadValue); // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM2), 14000)// 100
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
  
  /* Set output channel polarity */
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  //LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  
  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH2(TIM2, ( (LL_TIM_GetAutoReload(TIM2) + 1 ) / 2));
  
  /* Enable TIM2_CCR1 register preload. Read/Write operations access the      */
  /* preload register. TIM2_CCR1 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH2);
  
  /**************************/
  /* TIM2 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC2(TIM2);
  // My
  LL_TIM_OC_SetCompareCH2(TIM2, 0); // No Light
  
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel 1 */
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);
  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM2);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM2);
}


//__STATIC_INLINE 
void  LLU_V2_UvChannelInit(void)
{
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  
  
  /* GPIO TIM2_CH1 configuration */
  LL_GPIO_SetPinMode   (GPIOB, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE );
  LL_GPIO_SetPinPull   (GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_DOWN      );
  LL_GPIO_SetPinSpeed  (GPIOB, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_8, LL_GPIO_AF_3           ); //LL_GPIO_SetAFPin_0_7 LL_GPIO_AF_2
  
  /***********************************************/
  /* Configure the NVIC to handle TIM2 interrupt */
  /***********************************************/
  NVIC_SetPriority( TIM1_UP_TIM10_IRQn, 5); // 0  TIM10_IRQn
  NVIC_EnableIRQ  ( TIM1_UP_TIM10_IRQn   ); // TIM10_IRQn
  
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
//  LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM1 ); 
//  LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM9 ); 
//  LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM10 ); 
  //
 // LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM11 ); 
  LL_APB2_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM10 );  // LL_APB1_GRP1_PERIPH_TIM11
  
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  
  /* Set the pre-scaler value to have TIM2 counter clock equal to (10) kHz */
  LL_TIM_SetPrescaler(TIM10,  __LL_TIM_CALC_PSC(SystemCoreClock,40000000 ) ); //  10000
  
  /* Enable TIM2_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM10);
  
  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM2CLK = SystemCoreClock / (APB prescaler & multiplier)               */
  TimOutClock = SystemCoreClock/1;
  AutoReloadValue = 65535 ; // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100);
  LL_TIM_SetAutoReload(TIM10, AutoReloadValue); //650 14000
//LL_TIM_SetAutoReload(TIM11, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100));   //  1000 100
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM10, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
  
  /* Set output channel polarity */
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  //LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  
  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH1(TIM10, ( (LL_TIM_GetAutoReload(TIM10) + 1 ) / 2));
  
  /* Enable TIM2_CCR1 register preload. Read/Write operations access the      */
  /* preload register. TIM2_CCR1 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM10, LL_TIM_CHANNEL_CH1);
  // My
  LL_TIM_OC_SetCompareCH1(TIM10, 0); // No Light
  
  /**************************/
  /* TIM2 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC1(TIM10);
  
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel 1 */
  LL_TIM_CC_EnableChannel(TIM10, LL_TIM_CHANNEL_CH1);
  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM10);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM10);
}
//

//__STATIC_INLINE 
void  LLU_V2_WhiteChannelInit(void) // TIM4_CN2 PB7
{
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  
  
  /* GPIO TIM2_CH1 configuration */
  LL_GPIO_SetPinMode   (GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE );
  LL_GPIO_SetPinPull   (GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN      );
  LL_GPIO_SetPinSpeed  (GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_0_7 (GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_2           ); // LL_GPIO_AF_3
  
  /***********************************************/
  /* Configure the NVIC to handle TIM2 interrupt */
  /***********************************************/
  NVIC_SetPriority( TIM4_IRQn, 6); // 0  TIM10_IRQn
  NVIC_EnableIRQ  ( TIM4_IRQn   ); // TIM10_IRQn
  
  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
//  LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM1 ); 
//  LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM9 ); 
//  LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM10 ); 
  //
 // LL_APB1_GRP1_EnableClock ( LL_APB2_GRP1_PERIPH_TIM11 ); 
  LL_APB1_GRP1_EnableClock ( LL_APB1_GRP1_PERIPH_TIM4 );  // LL_APB1_GRP1_PERIPH_TIM11
  
  
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  
  /* Set the pre-scaler value to have TIM2 counter clock equal to (10) kHz */
  LL_TIM_SetPrescaler(TIM4,  __LL_TIM_CALC_PSC(SystemCoreClock,40000000 ) ); //  10000
  
  /* Enable TIM2_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM4);
  
  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM2CLK = SystemCoreClock / (APB prescaler & multiplier)               */
  TimOutClock = SystemCoreClock/1;
  AutoReloadValue = 65535 ; // __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100);
  LL_TIM_SetAutoReload(TIM4, AutoReloadValue); //650 14000
//LL_TIM_SetAutoReload(TIM11, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100));   //  1000 100
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM4, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
  
  /* Set output channel polarity */
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  //LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  
  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH1(TIM4, ( (LL_TIM_GetAutoReload(TIM4) + 1 ) / 2));
  
  /* Enable TIM2_CCR1 register preload. Read/Write operations access the      */
  /* preload register. TIM2_CCR1 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM4, LL_TIM_CHANNEL_CH2);
  // My
  LL_TIM_OC_SetCompareCH1(TIM4, 0); // No Light
  
  /**************************/
  /* TIM2 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC2(TIM4);
  
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel 1 */
  LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH2);
  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM4);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM4);
}
//


//
/*
__STATIC_INLINE void BOARD_EN_Init(void) // PA0
{
  // Enable the LED2 Clock //
  BOARD_EN_GPIO_CLK_ENABLE();

  // Configure IO in output push-pull mode to drive external LED2 //
  LL_GPIO_SetPinMode(BOARD_EN_GPIO_PORT, BOARD_EN_PIN, LL_GPIO_MODE_OUTPUT);
  // Reset value is LL_GPIO_OUTPUT_PUSHPULL //
  LL_GPIO_SetPinOutputType(BOARD_EN_GPIO_PORT, BOARD_EN_PIN, LL_GPIO_OUTPUT_PUSHPULL);//++
  // Reset value is LL_GPIO_SPEED_FREQ_LOW LL_GPIO_SPEED_FREQ_LOW//
  LL_GPIO_SetPinSpeed(BOARD_EN_GPIO_PORT, BOARD_EN_PIN, LL_GPIO_SPEED_FREQ_HIGH);//++
  // Reset value is LL_GPIO_PULL_NO //
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
  LL_mDelay(1); 
  LL_GPIO_SetOutputPin(BOARD_EN_GPIO_PORT, BOARD_EN_PIN); 
  LL_mDelay(1);   
}
*/
//


/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  User button interrupt processing
  * @note   When the user key button is pressed the PWM duty cycle is updated. 
  * @param  None
  * @retval None
  */
/*
void UserButton_Callback(void)
{

}
*/

/**
  * @brief  Timer capture/compare interrupt processing
  * @param  None
  * @retval None
  */
// Red
void Timer11CaptureCompare_Callback(void)
{
  uw_TIM11_MeasuredDutyCycle = (LL_TIM_GetCounter(TIM11) * 100) / ( LL_TIM_GetAutoReload(TIM11) + 1 );
}
// Green
void Timer3CaptureCompare_Callback(void)
{
  uw_TIM3_MeasuredDutyCycle = (LL_TIM_GetCounter(TIM3) * 100) / ( LL_TIM_GetAutoReload(TIM3) + 1 );
}
// Blue
void Timer2CaptureCompare_Callback(void)
{
  uw_TIM2_MeasuredDutyCycle = (LL_TIM_GetCounter(TIM2) * 100) / ( LL_TIM_GetAutoReload(TIM2) + 1 );
}
// Uv
void Timer10CaptureCompare_Callback(void)
{
  uw_TIM10_MeasuredDutyCycle = (LL_TIM_GetCounter(TIM10) * 100) / ( LL_TIM_GetAutoReload(TIM10) + 1 );
}
// White
void Timer4CaptureCompare_Callback(void)
{
  uw_TIM4_MeasuredDutyCycle = (LL_TIM_GetCounter(TIM4) * 100) / ( LL_TIM_GetAutoReload(TIM4) + 1 );
}

// IR Carrier 76 kHz.
void Timer1CaptureCompare_Callback(void)
{
  uw_TIM1_MeasuredDutyCycle = (LL_TIM_GetCounter(TIM1) * 100) / ( LL_TIM_GetAutoReload(TIM1) + 1 );
}



// =============================================================================
// ANIMATION STATE MACHINE — uwTick-driven fade engine (Stage 3)
// Replaces ISR-based animation stepping for Sequential and Rainbow modes.
// =============================================================================
//
// Inverse gamma tables: perceptual index 0-1000 -> linear fraction 0-1000.
// Apply at output: rawIndex = offset + (fraction * (max-offset)) / 1000.
// R/G/B/W: inverse of gamma 2.2.   UV: inverse of gamma 1.8.
// Generated: round(pow(i/1000.0, 1.0/gamma)*1000) for i in 0..1000.
//
static const uint16_t u16_InvGamma_RGBW[1001] = {
        0,    43,    59,    71,    81,    90,    98,   105,   111,   118,
      123,   129,   134,   139,   144,   148,   153,   157,   161,   165,
      169,   173,   176,   180,   184,   187,   190,   194,   197,   200,
      203,   206,   209,   212,   215,   218,   221,   223,   226,   229,
      232,   234,   237,   239,   242,   244,   247,   249,   252,   254,
      256,   259,   261,   263,   265,   268,   270,   272,   274,   276,
      278,   280,   283,   285,   287,   289,   291,   293,   295,   297,
      299,   301,   302,   304,   306,   308,   310,   312,   314,   315,
      317,   319,   321,   323,   324,   326,   328,   330,   331,   333,
      335,   336,   338,   340,   341,   343,   345,   346,   348,   350,
      351,   353,   354,   356,   357,   359,   361,   362,   364,   365,
      367,   368,   370,   371,   373,   374,   376,   377,   379,   380,
      381,   383,   384,   386,   387,   389,   390,   391,   393,   394,
      396,   397,   398,   400,   401,   402,   404,   405,   406,   408,
      409,   410,   412,   413,   414,   416,   417,   418,   420,   421,
      422,   423,   425,   426,   427,   429,   430,   431,   432,   434,
      435,   436,   437,   438,   440,   441,   442,   443,   444,   446,
      447,   448,   449,   450,   452,   453,   454,   455,   456,   457,
      459,   460,   461,   462,   463,   464,   466,   467,   468,   469,
      470,   471,   472,   473,   475,   476,   477,   478,   479,   480,
      481,   482,   483,   484,   486,   487,   488,   489,   490,   491,
      492,   493,   494,   495,   496,   497,   498,   499,   500,   501,
      502,   503,   505,   506,   507,   508,   509,   510,   511,   512,
      513,   514,   515,   516,   517,   518,   519,   520,   521,   522,
      523,   524,   525,   526,   527,   528,   529,   530,   531,   532,
      533,   533,   534,   535,   536,   537,   538,   539,   540,   541,
      542,   543,   544,   545,   546,   547,   548,   549,   550,   551,
      551,   552,   553,   554,   555,   556,   557,   558,   559,   560,
      561,   562,   562,   563,   564,   565,   566,   567,   568,   569,
      570,   571,   571,   572,   573,   574,   575,   576,   577,   578,
      579,   579,   580,   581,   582,   583,   584,   585,   585,   586,
      587,   588,   589,   590,   591,   592,   592,   593,   594,   595,
      596,   597,   597,   598,   599,   600,   601,   602,   602,   603,
      604,   605,   606,   607,   607,   608,   609,   610,   611,   612,
      612,   613,   614,   615,   616,   616,   617,   618,   619,   620,
      621,   621,   622,   623,   624,   625,   625,   626,   627,   628,
      629,   629,   630,   631,   632,   632,   633,   634,   635,   636,
      636,   637,   638,   639,   640,   640,   641,   642,   643,   643,
      644,   645,   646,   646,   647,   648,   649,   650,   650,   651,
      652,   653,   653,   654,   655,   656,   656,   657,   658,   659,
      659,   660,   661,   662,   662,   663,   664,   665,   665,   666,
      667,   668,   668,   669,   670,   670,   671,   672,   673,   673,
      674,   675,   676,   676,   677,   678,   678,   679,   680,   681,
      681,   682,   683,   684,   684,   685,   686,   686,   687,   688,
      689,   689,   690,   691,   691,   692,   693,   694,   694,   695,
      696,   696,   697,   698,   698,   699,   700,   701,   701,   702,
      703,   703,   704,   705,   705,   706,   707,   707,   708,   709,
      710,   710,   711,   712,   712,   713,   714,   714,   715,   716,
      716,   717,   718,   718,   719,   720,   720,   721,   722,   722,
      723,   724,   724,   725,   726,   726,   727,   728,   728,   729,
      730,   730,   731,   732,   732,   733,   734,   734,   735,   736,
      736,   737,   738,   738,   739,   740,   740,   741,   742,   742,
      743,   744,   744,   745,   745,   746,   747,   747,   748,   749,
      749,   750,   751,   751,   752,   753,   753,   754,   754,   755,
      756,   756,   757,   758,   758,   759,   760,   760,   761,   761,
      762,   763,   763,   764,   765,   765,   766,   766,   767,   768,
      768,   769,   770,   770,   771,   771,   772,   773,   773,   774,
      775,   775,   776,   776,   777,   778,   778,   779,   779,   780,
      781,   781,   782,   783,   783,   784,   784,   785,   786,   786,
      787,   787,   788,   789,   789,   790,   790,   791,   792,   792,
      793,   793,   794,   795,   795,   796,   796,   797,   798,   798,
      799,   799,   800,   801,   801,   802,   802,   803,   804,   804,
      805,   805,   806,   806,   807,   808,   808,   809,   809,   810,
      811,   811,   812,   812,   813,   813,   814,   815,   815,   816,
      816,   817,   818,   818,   819,   819,   820,   820,   821,   822,
      822,   823,   823,   824,   824,   825,   826,   826,   827,   827,
      828,   828,   829,   830,   830,   831,   831,   832,   832,   833,
      834,   834,   835,   835,   836,   836,   837,   838,   838,   839,
      839,   840,   840,   841,   841,   842,   843,   843,   844,   844,
      845,   845,   846,   846,   847,   848,   848,   849,   849,   850,
      850,   851,   851,   852,   853,   853,   854,   854,   855,   855,
      856,   856,   857,   857,   858,   859,   859,   860,   860,   861,
      861,   862,   862,   863,   863,   864,   865,   865,   866,   866,
      867,   867,   868,   868,   869,   869,   870,   870,   871,   872,
      872,   873,   873,   874,   874,   875,   875,   876,   876,   877,
      877,   878,   878,   879,   880,   880,   881,   881,   882,   882,
      883,   883,   884,   884,   885,   885,   886,   886,   887,   887,
      888,   889,   889,   890,   890,   891,   891,   892,   892,   893,
      893,   894,   894,   895,   895,   896,   896,   897,   897,   898,
      898,   899,   899,   900,   900,   901,   901,   902,   903,   903,
      904,   904,   905,   905,   906,   906,   907,   907,   908,   908,
      909,   909,   910,   910,   911,   911,   912,   912,   913,   913,
      914,   914,   915,   915,   916,   916,   917,   917,   918,   918,
      919,   919,   920,   920,   921,   921,   922,   922,   923,   923,
      924,   924,   925,   925,   926,   926,   927,   927,   928,   928,
      929,   929,   930,   930,   931,   931,   932,   932,   933,   933,
      934,   934,   935,   935,   936,   936,   937,   937,   938,   938,
      939,   939,   940,   940,   941,   941,   942,   942,   943,   943,
      944,   944,   945,   945,   945,   946,   946,   947,   947,   948,
      948,   949,   949,   950,   950,   951,   951,   952,   952,   953,
      953,   954,   954,   955,   955,   956,   956,   957,   957,   958,
      958,   959,   959,   959,   960,   960,   961,   961,   962,   962,
      963,   963,   964,   964,   965,   965,   966,   966,   967,   967,
      968,   968,   968,   969,   969,   970,   970,   971,   971,   972,
      972,   973,   973,   974,   974,   975,   975,   976,   976,   976,
      977,   977,   978,   978,   979,   979,   980,   980,   981,   981,
      982,   982,   983,   983,   983,   984,   984,   985,   985,   986,
      986,   987,   987,   988,   988,   989,   989,   989,   990,   990,
      991,   991,   992,   992,   993,   993,   994,   994,   995,   995,
      995,   996,   996,   997,   997,   998,   998,   999,   999,  1000,
     1000
};

static const uint16_t u16_InvGamma_UV[1001] = {
        0,    22,    32,    40,    47,    53,    58,    64,    68,    73,
       77,    82,    86,    90,    93,    97,   101,   104,   107,   111,
      114,   117,   120,   123,   126,   129,   132,   134,   137,   140,
      143,   145,   148,   150,   153,   155,   158,   160,   163,   165,
      167,   170,   172,   174,   176,   179,   181,   183,   185,   187,
      189,   191,   193,   196,   198,   200,   202,   204,   206,   208,
      210,   211,   213,   215,   217,   219,   221,   223,   225,   226,
      228,   230,   232,   234,   235,   237,   239,   241,   242,   244,
      246,   248,   249,   251,   253,   254,   256,   258,   259,   261,
      262,   264,   266,   267,   269,   270,   272,   274,   275,   277,
      278,   280,   281,   283,   284,   286,   287,   289,   290,   292,
      293,   295,   296,   298,   299,   301,   302,   304,   305,   306,
      308,   309,   311,   312,   314,   315,   316,   318,   319,   321,
      322,   323,   325,   326,   327,   329,   330,   331,   333,   334,
      335,   337,   338,   339,   341,   342,   343,   345,   346,   347,
      349,   350,   351,   352,   354,   355,   356,   358,   359,   360,
      361,   363,   364,   365,   366,   368,   369,   370,   371,   372,
      374,   375,   376,   377,   379,   380,   381,   382,   383,   385,
      386,   387,   388,   389,   390,   392,   393,   394,   395,   396,
      397,   399,   400,   401,   402,   403,   404,   406,   407,   408,
      409,   410,   411,   412,   413,   415,   416,   417,   418,   419,
      420,   421,   422,   424,   425,   426,   427,   428,   429,   430,
      431,   432,   433,   434,   436,   437,   438,   439,   440,   441,
      442,   443,   444,   445,   446,   447,   448,   449,   450,   452,
      453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
      463,   464,   465,   466,   467,   468,   469,   470,   471,   472,
      473,   474,   475,   476,   477,   478,   479,   480,   481,   482,
      483,   484,   485,   486,   487,   488,   489,   490,   491,   492,
      493,   494,   495,   496,   497,   498,   499,   500,   501,   502,
      503,   504,   505,   506,   507,   508,   508,   509,   510,   511,
      512,   513,   514,   515,   516,   517,   518,   519,   520,   521,
      522,   523,   524,   525,   525,   526,   527,   528,   529,   530,
      531,   532,   533,   534,   535,   536,   536,   537,   538,   539,
      540,   541,   542,   543,   544,   545,   546,   546,   547,   548,
      549,   550,   551,   552,   553,   554,   555,   555,   556,   557,
      558,   559,   560,   561,   562,   563,   563,   564,   565,   566,
      567,   568,   569,   570,   570,   571,   572,   573,   574,   575,
      576,   576,   577,   578,   579,   580,   581,   582,   582,   583,
      584,   585,   586,   587,   588,   588,   589,   590,   591,   592,
      593,   594,   594,   595,   596,   597,   598,   599,   599,   600,
      601,   602,   603,   604,   604,   605,   606,   607,   608,   609,
      609,   610,   611,   612,   613,   613,   614,   615,   616,   617,
      618,   618,   619,   620,   621,   622,   622,   623,   624,   625,
      626,   627,   627,   628,   629,   630,   631,   631,   632,   633,
      634,   635,   635,   636,   637,   638,   639,   639,   640,   641,
      642,   643,   643,   644,   645,   646,   646,   647,   648,   649,
      650,   650,   651,   652,   653,   654,   654,   655,   656,   657,
      657,   658,   659,   660,   661,   661,   662,   663,   664,   664,
      665,   666,   667,   667,   668,   669,   670,   671,   671,   672,
      673,   674,   674,   675,   676,   677,   677,   678,   679,   680,
      680,   681,   682,   683,   683,   684,   685,   686,   686,   687,
      688,   689,   689,   690,   691,   692,   692,   693,   694,   695,
      695,   696,   697,   698,   698,   699,   700,   701,   701,   702,
      703,   704,   704,   705,   706,   706,   707,   708,   709,   709,
      710,   711,   712,   712,   713,   714,   714,   715,   716,   717,
      717,   718,   719,   720,   720,   721,   722,   722,   723,   724,
      725,   725,   726,   727,   727,   728,   729,   730,   730,   731,
      732,   732,   733,   734,   735,   735,   736,   737,   737,   738,
      739,   740,   740,   741,   742,   742,   743,   744,   745,   745,
      746,   747,   747,   748,   749,   749,   750,   751,   752,   752,
      753,   754,   754,   755,   756,   756,   757,   758,   758,   759,
      760,   761,   761,   762,   763,   763,   764,   765,   765,   766,
      767,   767,   768,   769,   770,   770,   771,   772,   772,   773,
      774,   774,   775,   776,   776,   777,   778,   778,   779,   780,
      780,   781,   782,   782,   783,   784,   784,   785,   786,   786,
      787,   788,   789,   789,   790,   791,   791,   792,   793,   793,
      794,   795,   795,   796,   797,   797,   798,   799,   799,   800,
      801,   801,   802,   803,   803,   804,   804,   805,   806,   806,
      807,   808,   808,   809,   810,   810,   811,   812,   812,   813,
      814,   814,   815,   816,   816,   817,   818,   818,   819,   820,
      820,   821,   822,   822,   823,   823,   824,   825,   825,   826,
      827,   827,   828,   829,   829,   830,   831,   831,   832,   833,
      833,   834,   834,   835,   836,   836,   837,   838,   838,   839,
      840,   840,   841,   842,   842,   843,   843,   844,   845,   845,
      846,   847,   847,   848,   848,   849,   850,   850,   851,   852,
      852,   853,   854,   854,   855,   855,   856,   857,   857,   858,
      859,   859,   860,   860,   861,   862,   862,   863,   864,   864,
      865,   865,   866,   867,   867,   868,   869,   869,   870,   870,
      871,   872,   872,   873,   874,   874,   875,   875,   876,   877,
      877,   878,   878,   879,   880,   880,   881,   882,   882,   883,
      883,   884,   885,   885,   886,   886,   887,   888,   888,   889,
      890,   890,   891,   891,   892,   893,   893,   894,   894,   895,
      896,   896,   897,   897,   898,   899,   899,   900,   900,   901,
      902,   902,   903,   903,   904,   905,   905,   906,   906,   907,
      908,   908,   909,   909,   910,   911,   911,   912,   912,   913,
      914,   914,   915,   915,   916,   917,   917,   918,   918,   919,
      920,   920,   921,   921,   922,   923,   923,   924,   924,   925,
      926,   926,   927,   927,   928,   929,   929,   930,   930,   931,
      931,   932,   933,   933,   934,   934,   935,   936,   936,   937,
      937,   938,   938,   939,   940,   940,   941,   941,   942,   943,
      943,   944,   944,   945,   945,   946,   947,   947,   948,   948,
      949,   950,   950,   951,   951,   952,   952,   953,   954,   954,
      955,   955,   956,   956,   957,   958,   958,   959,   959,   960,
      960,   961,   962,   962,   963,   963,   964,   964,   965,   966,
      966,   967,   967,   968,   968,   969,   970,   970,   971,   971,
      972,   972,   973,   974,   974,   975,   975,   976,   976,   977,
      978,   978,   979,   979,   980,   980,   981,   982,   982,   983,
      983,   984,   984,   985,   985,   986,   987,   987,   988,   988,
      989,   989,   990,   991,   991,   992,   992,   993,   993,   994,
      994,   995,   996,   996,   997,   997,   998,   998,   999,   999,
     1000
};

//
// g_Fade[5] — per-channel fade parameter blocks (0=R 1=G 2=B 3=UV 4=W)
FadeChannel_t g_Fade[5];
/* Debug: captured at Fade peak for Green channel */
volatile uint16_t dbg_fade_brightnessmax = 0;
volatile int32_t  dbg_fade_percx10_peak  = 0;
volatile uint16_t dbg_fade_raw_out       = 0;
volatile uint16_t dbg_globbrightmax      = 0;
uint16_t u16_GlobalBrightMax = 1000u;  // 0-1000, global animation brightness ceiling

// Default animation parameters
#define ANIM_DEFAULT_DURATION_MS  4000u
#define ANIM_DEFAULT_MAX          1000u

//
// Externs needed by the animation engine
//
extern uint32_t u32_ST_captureActual;
//
// Forward declarations for UpdateXxxLevelFast_MP (defined in main.c)
#ifdef MPU_V2_DEVICE
void UpdateRedLevelFast_MP  (uint16_t);
void UpdateGreenLevelFast_MP(uint16_t);
void UpdateBlueLevelFast_MP (uint16_t);
void UpdateUvLevelFast_MP   (uint16_t);
void UpdateWhiteLevelFast_MP(uint16_t);
#endif
//
// PerceptualToRaw — maps perceptual 0-1000 to raw PWM table index [offset..max-1].
// u8_IsUV: 1 = use UV gamma-1.8 table, 0 = use RGBW gamma-2.2 table.

#define PERC_ZERO_THRESH 8u

static uint16_t PerceptualToRaw(uint16_t u16_Perc,
                                 uint16_t u16_MaxIdx,
                                 uint8_t  u8_IsUV,
                                 uint16_t u16_Zone3Start)
{
    uint32_t u32_Frac, u32_Range, u32_Raw;
    if (u16_Perc < PERC_ZERO_THRESH) return 0u;
    if (u16_Perc >= 1000u)           return (uint16_t)(u16_MaxIdx - 1u);
    u32_Frac  = u8_IsUV ? (uint32_t)u16_InvGamma_UV[u16_Perc]
                        : (uint32_t)u16_InvGamma_RGBW[u16_Perc];
    // Map perceptual 0-1000 across the full raw range
    u32_Range = (uint32_t)(u16_MaxIdx - 20u);
    u32_Raw   = 20u + (u32_Frac * u32_Range) / 1000u;
    if (u32_Raw >= (uint32_t)u16_MaxIdx) u32_Raw = (uint32_t)(u16_MaxIdx - 1u);
    return (uint16_t)u32_Raw;
}

// OutputChannel — compute dithered raw index and call UpdateXxxLevelFast_MP.
// u16_Perc:  perceptual level 0-1000.
// u8_Frac10: sub-step residue 0-9 (i32_PercX10 % 10); used for dither decision.

static void OutputChannel(uint8_t u8_Ch, uint16_t u16_Perc, uint8_t u8_Frac10)
{
    uint16_t u16_MaxIdx = 0;
    uint16_t u16_Zone3Start = 0;
    uint8_t  u8_IsUV = 0u;
    uint16_t u16_RawLo, u16_Out;

#ifdef MPU_V2_DEVICE
    switch (u8_Ch) {
        case FADE_CH_RED:
            u16_MaxIdx = RED_MAX_INDEX;   u16_Zone3Start = 1230u; break;
        case FADE_CH_GREEN:
            u16_MaxIdx = GREEN_MAX_INDEX; u16_Zone3Start = 1449u; break;
        case FADE_CH_BLUE:
            u16_MaxIdx = BLUE_MAX_INDEX;  u16_Zone3Start = 1449u; break;
        case FADE_CH_UV:
            u16_MaxIdx = UV_MAX_INDEX;    u16_Zone3Start = 1449u;
            u8_IsUV = 1u;                 break;
        case FADE_CH_WHITE:
            u16_MaxIdx = WHITE_MAX_INDEX; u16_Zone3Start = 1449u; break;
        default: return;
    }
    if (u16_MaxIdx == 0u) return;

    u16_RawLo = PerceptualToRaw(u16_Perc, u16_MaxIdx, u8_IsUV, u16_Zone3Start);

    if ((u8_Frac10 >= 5u) && (u16_RawLo < (uint16_t)(u16_MaxIdx - 1u)))
    {
        u16_Out = (g_Fade[u8_Ch].u8_DitherPhase & 1u)
                  ? (uint16_t)(u16_RawLo + 1u) : u16_RawLo;
    }
    else
    {
        u16_Out = u16_RawLo;
    }
    g_Fade[u8_Ch].u8_DitherPhase++;

    /* Brightness ceiling is already encoded in u16_BrightnessMax which feeds
     * PerceptualToRaw() via StepChannel(). No second scaling needed here.
     * Removed: u16_Out * u16_GlobalBrightMax / 1000 caused double-dimming. */

    switch (u8_Ch) {
        case FADE_CH_RED:   UpdateRedLevelFast_MP  (u16_Out); break;
        case FADE_CH_GREEN: UpdateGreenLevelFast_MP(u16_Out); break;
        case FADE_CH_BLUE:  UpdateBlueLevelFast_MP (u16_Out); break;
        case FADE_CH_UV:    UpdateUvLevelFast_MP   (u16_Out); break;
        case FADE_CH_WHITE: UpdateWhiteLevelFast_MP(u16_Out); break;
        default: break;
    }
#else
    (void)u8_Ch; (void)u16_Perc; (void)u8_Frac10;
#endif
}

// SilenceChannel — drive a channel to its floor (BrightnessMin) immediately.
static void SilenceChannel(uint8_t u8_Ch)
{
    FadeChannel_t *p = &g_Fade[u8_Ch];
    p->i32_PercX10      = 0;
    p->u8_Dir           = 1u;
    p->u32_SubStepAccum = 0u; // Clear residue on silence so next activation starts clean
#ifdef MPU_V2_DEVICE
    switch (u8_Ch) {
        case FADE_CH_RED:   SetRedLevel  (0u); break;
        case FADE_CH_GREEN: SetGreenLevel(0u); break;
        case FADE_CH_BLUE:  SetBlueLevel (0u); break;
        case FADE_CH_UV:    SetUvLevel   (0u); break;
        case FADE_CH_WHITE: SetWhiteLevel(0u); break;
        default: break;
    }
#endif
}

// StartChannelUp — set direction to UP and record current tick.
static void StartChannelUp(uint8_t u8_Ch, uint32_t u32_NowMs)
{
    g_Fade[u8_Ch].u8_Dir          = 1u;
    g_Fade[u8_Ch].u32_LastTickMs  = u32_NowMs;
    g_Fade[u8_Ch].u32_SubStepAccum = 0u; // Clear residue so prior brightness ceiling does not bleed into next phase
}

// StepChannel — advance one channel by elapsed ms.
// Returns: 0 = still running, 1 = just reached BrightnessMin, 2 = just reached BrightnessMax.
static uint8_t StepChannel(uint8_t u8_Ch, uint32_t u32_NowMs)
{
    FadeChannel_t *p = &g_Fade[u8_Ch];
    int32_t  i32_MaxX10   = (int32_t)p->u16_BrightnessMax * 10;
    int32_t  i32_MinX10   = (int32_t)p->u16_BrightnessMin * 10;
    uint32_t u32_Elapsed;
    int32_t  i32_Step;
    uint8_t  u8_Event = 0u;

    if (u32_NowMs == p->u32_LastTickMs) return 0u;
    u32_Elapsed       = u32_NowMs - p->u32_LastTickMs;
    p->u32_LastTickMs = u32_NowMs;

    if (p->u32_FadeDurationMs == 0u) p->u32_FadeDurationMs = ANIM_DEFAULT_DURATION_MS;
   // If BrightnessMax was reduced below current level, clamp and reverse
    if (p->i32_PercX10 > i32_MaxX10) {
        p->i32_PercX10 = i32_MaxX10;
        p->u8_Dir = 0u;
    }
    // Constant-duration step using fractional accumulator to avoid integer truncation.
    // At low brightness (small MaxX10), naive MaxX10*elapsed/duration truncates to 0.
    // Accumulate x1000 scaled units; advance i32_PercX10 only when accumulator >= 1000.
    // Example: BrightnessMax=200, elapsed=2ms, duration=12000ms:
    //   accum += 2000*2*1000/12000 = 333 per tick
    //   after 3 ticks: accum=999 → step=0; 4th tick: accum=1332 → step=1, accum=332
    //   2000 units at this rate takes ~6000ms = same proportion as 10000 in 12000ms.
    { uint32_t u32_StepX1000 = (i32_MaxX10 > 0 && p->u32_FadeDurationMs > 0u)
          ? ((uint32_t)i32_MaxX10 * u32_Elapsed * 1000u) / p->u32_FadeDurationMs
          : 1000u;
      p->u32_SubStepAccum += u32_StepX1000;
      i32_Step = (int32_t)(p->u32_SubStepAccum / 1000u);
      p->u32_SubStepAccum %= 1000u;
    }
    if (i32_Step == 0) {
        OutputChannel(u8_Ch, (uint16_t)(p->i32_PercX10 / 10), (uint8_t)(p->i32_PercX10 % 10));
        return 0u;
    }

    if (p->u8_Dir) {
        p->i32_PercX10 += i32_Step;
        if (p->i32_PercX10 >= i32_MaxX10) {
            p->i32_PercX10 = i32_MaxX10;
            u8_Event = 2u; // reached max
            /* Debug capture at peak */
            dbg_fade_brightnessmax = p->u16_BrightnessMax;
            dbg_fade_percx10_peak  = p->i32_PercX10;
            dbg_globbrightmax      = u16_GlobalBrightMax;
        }
    } else {
        p->i32_PercX10 -= i32_Step;
        if (p->i32_PercX10 <= i32_MinX10) {
            p->i32_PercX10 = i32_MinX10;
            u8_Event = 1u; // reached min
        }
    }

    OutputChannel(u8_Ch,
                  (uint16_t)(p->i32_PercX10 / 10),
                  (uint8_t) (p->i32_PercX10 % 10));
    return u8_Event;
}

// Sequential channel order: R -> G -> B -> UV -> W -> R -> ...
static const uint8_t c_SeqOrder[5] = {
    FADE_CH_RED, FADE_CH_GREEN, FADE_CH_BLUE, FADE_CH_UV, FADE_CH_WHITE
};

// Animation state (persistent across calls)
static uint8_t  u8_SeqActiveIdx = 0u; // index into c_SeqOrder
static uint8_t  u8_SeqPhase     = 0u; // 0 = fading up, 1 = fading down
static uint8_t  u8_RnbwFading   = FADE_CH_RED;
static uint8_t  u8_RnbwDecaying = 0xFFu; // 0xFF = none
static uint8_t  u8_RnbwStarted  = 0u;
static uint8_t  u8_SeqStarted   = 0u;

void Animation_Init(void)
{
    uint8_t i;
    for (i = 0u; i < 5u; i++) {
        g_Fade[i].u16_BrightnessMax  = u16_GlobalBrightMax;
        g_Fade[i].u16_BrightnessMin  = 0u;
        g_Fade[i].u32_LastTickMs     = 0u;
        g_Fade[i].i32_PercX10        = 0;
        g_Fade[i].u8_Dir             = 1u;
        g_Fade[i].u8_DitherPhase     = 0u;
        g_Fade[i].u32_SubStepAccum   = 0u;
    }
    // Per-channel durations proportional to table size for even visual speed
    g_Fade[FADE_CH_RED  ].u32_FadeDurationMs = 12000u; // equalised: all channels same duration
    g_Fade[FADE_CH_GREEN].u32_FadeDurationMs = 12000u; // 2900 entries
    g_Fade[FADE_CH_BLUE ].u32_FadeDurationMs = 12000u; // 2900 entries
    g_Fade[FADE_CH_UV   ].u32_FadeDurationMs = 12000u; // equalised
    g_Fade[FADE_CH_WHITE].u32_FadeDurationMs = 12000u; // 2900 entries
    //
    u8_SeqActiveIdx = 0u;
    u8_SeqPhase     = 0u;
    u8_RnbwFading   = FADE_CH_RED;
    u8_RnbwDecaying = 0xFFu;
    u8_RnbwStarted  = 0u;
    u8_SeqStarted   = 0u;
    UpdateStrobeRawIndices(); /* Init strobe raw indices from startup GlobalBrightMax */
}

void Animation_Update(void)
{
    uint32_t u32_Now = u32_ST_captureActual;
    uint8_t  u8_Event, u8_Next, i;

    // Sync brightness ceiling from manual AM variables (raw index -> 0-1000 perceptual)
    // This makes the UP/DOWN buttons control animation brightness ceiling.
   // Apply global brightness ceiling to all channels uniformly.
    // u16_GlobalBrightMax (0-1000) is the single source of truth — set by
    // Up/Down in any mode. Do not hardcode 1000 here.
    {
        uint8_t ch;
        for (ch = 0u; ch < 5u; ch++) {
            g_Fade[ch].u16_BrightnessMax = u16_GlobalBrightMax;
        }
    } 
    // White is physically brighter — cap at 60% of global max to balance
    if (u8_StateMaschine == SM_MODE_SEQUENTIAL)
    {
        if (u8_Mode_Sqnt_Status != MODE_STATUS_RUN) {
            // On stop, reset sequential state so next RUN starts cleanly
            u8_SeqStarted = 0u;
            return;
        }

        // First entry into RUN: silence all channels, start first channel up
        if (!u8_SeqStarted) {
            for (i = 0u; i < 5u; i++) SilenceChannel(i);
            u8_SeqActiveIdx = 0u;
            u8_SeqPhase     = 0u;
            StartChannelUp(c_SeqOrder[0u], u32_Now);
            u8_SeqStarted = 1u;
        }

        u8_Event = StepChannel(c_SeqOrder[u8_SeqActiveIdx], u32_Now);

        if (u8_SeqPhase == 0u) {
            // Fading up — wait for MAX event
            if (u8_Event == 2u) {
                // Reached max: start fading down
                g_Fade[c_SeqOrder[u8_SeqActiveIdx]].u8_Dir = 0u;
                u8_SeqPhase = 1u;
            }
            // Update legacy SM variable (UP state = channel*2)
            u32_Mode_Sqnt_SM = (uint32_t)(c_SeqOrder[u8_SeqActiveIdx]) * 2u;
        } else {
            // Fading down — wait for MIN event (level == 0)
            if (u8_Event == 1u) {
                // Reached min: silence and advance to next channel
                SilenceChannel(c_SeqOrder[u8_SeqActiveIdx]);
                u8_SeqActiveIdx = (uint8_t)((u8_SeqActiveIdx + 1u) % 5u);
                u8_SeqPhase = 0u;
                StartChannelUp(c_SeqOrder[u8_SeqActiveIdx], u32_Now);
            }
            // Update legacy SM variable (DOWN state = channel*2+1)
            u32_Mode_Sqnt_SM = (uint32_t)(c_SeqOrder[u8_SeqActiveIdx]) * 2u + 1u;
        }
    }
    else if (u8_StateMaschine == SM_MODE_RAINBOW)
    {
        if (u8_Mode_Rnbw_Status != MODE_STATUS_RUN) {
            u8_RnbwStarted = 0u;
            return;
        }

        // First entry: silence all, start Red up
        if (!u8_RnbwStarted) {
            for (i = 0u; i < 5u; i++) SilenceChannel(i);
            u8_RnbwFading   = FADE_CH_RED;
            u8_RnbwDecaying = 0xFFu;
            StartChannelUp(FADE_CH_RED, u32_Now);
            u8_RnbwStarted = 1u;
        }

        // Step the channel currently fading up
        u8_Event = StepChannel(u8_RnbwFading, u32_Now);

        // Also step the decaying channel (if one is active)
        if (u8_RnbwDecaying != 0xFFu) {
            StepChannel(u8_RnbwDecaying, u32_Now);
        }

        // When the rising channel hits its max, start the next channel up
        // and put the current channel into decay
        if (u8_Event == 2u) {
            u8_Next = (uint8_t)((u8_RnbwFading + 1u) % 5u);
            // Silence any fully-decayed channel (it should be at zero)
            if (u8_RnbwDecaying != 0xFFu) {
                SilenceChannel(u8_RnbwDecaying);
            }
            // Current fading channel becomes the decaying channel
            u8_RnbwDecaying = u8_RnbwFading;
            g_Fade[u8_RnbwDecaying].u8_Dir = 0u; // fade down
            // Start next channel rising
            u8_RnbwFading = u8_Next;
            StartChannelUp(u8_Next, u32_Now);
        }

        // Update legacy SM variable
        u32_Mode_Rnbw_SM = (uint32_t)(u8_RnbwFading) * 2u;
    }
    else
    {
        // Not in animation mode: reset state for clean re-entry
        u8_SeqStarted  = 0u;
        u8_RnbwStarted = 0u;
    }
}
// =============================================================================
// End of Animation State Machine (Stage 3)
// =============================================================================


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
