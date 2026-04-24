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
uint16_t   RED_MAX_INDEX = 0 ;
uint16_t GREEN_MAX_INDEX = 0 ;
uint16_t  BLUE_MAX_INDEX = 0 ;
uint16_t    UV_MAX_INDEX = 0 ;
uint16_t WHITE_MAX_INDEX = 0 ;
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
static const uint16_t u16_GammaTable_RED[101] = {
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
static const uint16_t u16_GammaTable_GREEN[101] = {
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
static const uint16_t u16_GammaTable_BLUE[101] = {
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
static const uint16_t u16_GammaTable_UV[101] = {
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
static const uint16_t u16_GammaTable_WHITE[101] = {
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
static uint16_t GammaCorrectIndex(uint16_t u16_Index, uint16_t u16_Max,
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
  //
  b_TurnOffPatternActive = true;
  //
  u32_Mode_Sqnt_SM  = SM_MODE_RED_UP ;  
  u32_Mode_Rnbw_SM  = SM_MODE_RED_UP ; 
  //
  //      
  //  гасим  Red
  u32_Delay_ms = 1;
  while  (u32_LightLevelRed || u32_LightLevelGreen||u32_LightLevelBlue||u32_LightLevelUv||u32_LightLevelWhite)
  {
    if( u32_LightLevelRed  >11 ) u32_LightLevelRed   -= 10; else if (u32_LightLevelRed)  u32_LightLevelRed   -- ;
    if( u32_LightLevelGreen>11 ) u32_LightLevelGreen -= 10; else if (u32_LightLevelGreen)   u32_LightLevelGreen -- ;
    if( u32_LightLevelBlue >11 ) u32_LightLevelBlue  -= 10; else if (u32_LightLevelBlue)   u32_LightLevelBlue  -- ;
    if( u32_LightLevelUv   >11 ) u32_LightLevelUv    -= 10; else if (u32_LightLevelUv)  u32_LightLevelUv    -- ;
    if( u32_LightLevelWhite>11 ) u32_LightLevelWhite -= 10; else if (u32_LightLevelWhite)  u32_LightLevelWhite -- ;
    //
    SetRedLevel   ( u32_LightLevelRed   );
    SetGreenLevel ( u32_LightLevelGreen );
    SetBlueLevel  ( u32_LightLevelBlue  );
    SetUvLevel    ( u32_LightLevelUv    );
    SetWhiteLevel ( u32_LightLevelWhite );
    //  
    LL_mDelay(u32_Delay_ms); 
    //     
  }
  //
  b_TurnOffPatternActive = false;
  //
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
void  DirectLightCtrl( int32_t i32_Channel, int32_t i32_Perc)
{
  static uint32_t u32_Delay_ms =1;
  // Для выбранного канала свечения, в зависимости от заданного значения, 
  // выполняем плавное увеличение или уменьшение яркости.
  if ( i32_Channel == _ACTUAL_RED )
  { // 
    i16_LightLevelRed_AM = RED_OFFSET+i16_R_10perc* i32_Perc / PRCNT_SHIFT;
    if ( i16_LightLevelRed_AM > RED_MAX_INDEX )  i16_LightLevelRed_AM = RED_MAX_INDEX-2 ;
    if ( i16_LightLevelRed_AM <             0 )  i16_LightLevelRed_AM =               0 ;
    //
    if (i16_LightLevelRed_AM > u32_LightLevelRed)
    {// Увеличение яркости
      for (                                 ;  u32_LightLevelRed<i16_LightLevelRed_AM; u32_LightLevelRed ++ ) 
      {      SetRedLevel (GammaCorrectIndex((uint16_t)u32_LightLevelRed, RED_MAX_INDEX, RED_OFFSET, u16_GammaTable_RED));  LL_mDelay(u32_Delay_ms);   }
    }
    else
    {// Уменьшение яркости
      for (                                 ;  u32_LightLevelRed>i16_LightLevelRed_AM; u32_LightLevelRed -- )  
      {      SetRedLevel (GammaCorrectIndex((uint16_t)u32_LightLevelRed, RED_MAX_INDEX, RED_OFFSET, u16_GammaTable_RED)); LL_mDelay(u32_Delay_ms);   }
    }
  } 
  else
  if ( i32_Channel == _ACTUAL_GRN )
  { // 
    i16_LightLevelGreen_AM = i16_G_10perc* i32_Perc / PRCNT_SHIFT;
    if ( i16_LightLevelGreen_AM > GREEN_MAX_INDEX )  i16_LightLevelGreen_AM = GREEN_MAX_INDEX-2 ;
    if ( i16_LightLevelGreen_AM <             0 )  i16_LightLevelGreen_AM =               0 ;
    //
    if (i16_LightLevelGreen_AM > u32_LightLevelGreen)
    {// Увеличение яркости
      for (                                 ;  u32_LightLevelGreen<i16_LightLevelGreen_AM; u32_LightLevelGreen ++ ) 
      {      SetGreenLevel (GammaCorrectIndex((uint16_t)u32_LightLevelGreen, GREEN_MAX_INDEX, GRN_OFFSET, u16_GammaTable_GREEN));  LL_mDelay(u32_Delay_ms);   }
    }
    else
    {// Уменьшение яркости
      for (                                 ;  u32_LightLevelGreen>i16_LightLevelGreen_AM; u32_LightLevelGreen -- )  
      {      SetGreenLevel (GammaCorrectIndex((uint16_t)u32_LightLevelGreen, GREEN_MAX_INDEX, GRN_OFFSET, u16_GammaTable_GREEN)); LL_mDelay(u32_Delay_ms);   }
    }
  } 
  else
  if ( i32_Channel == MANUAL_MODE_BLUE )
  { // 
    i16_LightLevelBlue_AM = i16_R_10perc* i32_Perc / PRCNT_SHIFT;
    if ( i16_LightLevelBlue_AM > RED_MAX_INDEX )  i16_LightLevelBlue_AM = BLUE_MAX_INDEX-2 ;
    if ( i16_LightLevelBlue_AM <             0 )  i16_LightLevelBlue_AM =               0 ;
    //
    if (i16_LightLevelBlue_AM > u32_LightLevelBlue)
    {// Увеличение яркости
      for (                                 ;  u32_LightLevelBlue<i16_LightLevelBlue_AM; u32_LightLevelBlue ++ ) 
      {      SetBlueLevel (GammaCorrectIndex((uint16_t)u32_LightLevelBlue, BLUE_MAX_INDEX, BLU_OFFSET, u16_GammaTable_BLUE));  LL_mDelay(u32_Delay_ms);   }
    }
    else
    {// Уменьшение яркости
      for (                                 ;  u32_LightLevelBlue>i16_LightLevelBlue_AM; u32_LightLevelBlue -- )  
      {      SetBlueLevel (GammaCorrectIndex((uint16_t)u32_LightLevelBlue, BLUE_MAX_INDEX, BLU_OFFSET, u16_GammaTable_BLUE)); LL_mDelay(u32_Delay_ms);   }
    }
  } 
  else
  if ( i32_Channel == MANUAL_MODE_UV )
  { // 
    i16_LightLevelUv_AM = i16_R_10perc* i32_Perc / PRCNT_SHIFT;
    if ( i16_LightLevelUv_AM > RED_MAX_INDEX )  i16_LightLevelUv_AM = UV_MAX_INDEX-2 ;
    if ( i16_LightLevelUv_AM <             0 )  i16_LightLevelUv_AM =               0 ;
    //
    if (i16_LightLevelUv_AM > u32_LightLevelUv)
    {// Увеличение яркости
      for (                                 ;  u32_LightLevelUv<i16_LightLevelUv_AM; u32_LightLevelUv ++ ) 
      {      SetUvLevel (GammaCorrectIndex((uint16_t)u32_LightLevelUv, UV_MAX_INDEX, UV_OFFSET, u16_GammaTable_UV));  LL_mDelay(u32_Delay_ms);   }
    }
    else
    {// Уменьшение яркости
      for (                                 ;  u32_LightLevelUv>i16_LightLevelUv_AM; u32_LightLevelUv -- )  
      {      SetUvLevel (GammaCorrectIndex((uint16_t)u32_LightLevelUv, UV_MAX_INDEX, UV_OFFSET, u16_GammaTable_UV)); LL_mDelay(u32_Delay_ms);   }
    }
  } 
  else
  if ( i32_Channel == MANUAL_MODE_WHITE )
  { // 
    i16_LightLevelWhite_AM = i16_R_10perc* i32_Perc / PRCNT_SHIFT;
    if ( i16_LightLevelWhite_AM > RED_MAX_INDEX )  i16_LightLevelWhite_AM = WHITE_MAX_INDEX-2 ;
    if ( i16_LightLevelWhite_AM <             0 )  i16_LightLevelWhite_AM =               0 ;
    //
    if (i16_LightLevelWhite_AM > u32_LightLevelWhite)
    {// Увеличение яркости
      for (                                 ;  u32_LightLevelWhite<i16_LightLevelWhite_AM; u32_LightLevelWhite ++ ) 
      {      SetWhiteLevel (GammaCorrectIndex((uint16_t)u32_LightLevelWhite, WHITE_MAX_INDEX, WHT_OFFSET, u16_GammaTable_WHITE));  LL_mDelay(u32_Delay_ms);   }
    }
    else
    {// Уменьшение яркости
      for (                                 ;  u32_LightLevelWhite>i16_LightLevelWhite_AM; u32_LightLevelWhite -- )  
      {      SetWhiteLevel (GammaCorrectIndex((uint16_t)u32_LightLevelWhite, WHITE_MAX_INDEX, WHT_OFFSET, u16_GammaTable_WHITE)); LL_mDelay(u32_Delay_ms);   }
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
        if ( u8_ManualChannel == MANUAL_MODE_RED   )   SetRedLevel   ( i16_LightLevelRed_AM   ); else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN )   SetGreenLevel ( i16_LightLevelGreen_AM ); else 
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  )   SetBlueLevel  ( i16_LightLevelBlue_AM  ); else
        if ( u8_ManualChannel == MANUAL_MODE_UV    )   SetUvLevel    ( i16_LightLevelUv_AM    ); else       
        if ( u8_ManualChannel == MANUAL_MODE_WHITE )   SetWhiteLevel ( i16_LightLevelWhite_AM );         
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
        if ( u8_ManualChannel == MANUAL_MODE_RED   )   SetRedLevel   ( i16_LightLevelRed_AM   ); else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN )   SetGreenLevel ( i16_LightLevelGreen_AM ); else 
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  )   SetBlueLevel  ( i16_LightLevelBlue_AM  ); else
        if ( u8_ManualChannel == MANUAL_MODE_UV    )   SetUvLevel    ( i16_LightLevelUv_AM    ); else       
        if ( u8_ManualChannel == MANUAL_MODE_WHITE )   SetWhiteLevel ( i16_LightLevelWhite_AM );         
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
  // Восстанавливаем амплитудные значения каждого канала
  i16_LightLevelRed_AM   =  i32_ActualPerc_AM [_ACTUAL_RED]/ PRCNT_SHIFT * i16_R_10perc  ;     
  i16_LightLevelGreen_AM =  i32_ActualPerc_AM [_ACTUAL_GRN]/ PRCNT_SHIFT * i16_G_10perc  ; 
  i16_LightLevelBlue_AM  =  i32_ActualPerc_AM [_ACTUAL_BLU]/ PRCNT_SHIFT * i16_B_10perc  ; 
  i16_LightLevelUv_AM    =  i32_ActualPerc_AM [_ACTUAL_UV ]/ PRCNT_SHIFT * i16_U_10perc  ; 
  i16_LightLevelWhite_AM =  i32_ActualPerc_AM [_ACTUAL_WHT]/ PRCNT_SHIFT * i16_W_10perc  ;  
  // 
  // Корректируем максимум для случая 100%
  if ( i32_ActualPerc_AM [_ACTUAL_RED] == 100 ) i16_LightLevelRed_AM    -= 2;
  if ( i32_ActualPerc_AM [_ACTUAL_GRN] == 100 ) i16_LightLevelGreen_AM  -= 2;
  if ( i32_ActualPerc_AM [_ACTUAL_BLU] == 100 ) i16_LightLevelBlue_AM   -= 2;
  if ( i32_ActualPerc_AM [_ACTUAL_UV ] == 100 ) i16_LightLevelUv_AM     -= 2;
  if ( i32_ActualPerc_AM [_ACTUAL_WHT] == 100 ) i16_LightLevelWhite_AM  -= 2;
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
  // Восстанавливаем амплитудные значения каждого канала в 100%
  
  i32_ActualPerc_AM [_ACTUAL_RED] = 100 ;     
  i32_ActualPerc_AM [_ACTUAL_GRN] = 100 ; 
  i32_ActualPerc_AM [_ACTUAL_BLU] = 100 ; 
  i32_ActualPerc_AM [_ACTUAL_UV ] = 100 ; 
  i32_ActualPerc_AM [_ACTUAL_WHT] = 100 ; 
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
                      
      }
      //
      //  ДАННОЕ ДЕЙСТВИЕ БЛОКИРУЕТ УПРАВЛЕНИЕ СТРОБОСКОПОМ В САМОМ ОБРАБОТЧИКЕ СТРОБОСКОПА
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
    }
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_DOWN )
    {   u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
        i16_LightLevelRed_AM   -= i16_R_10perc; if (i16_LightLevelRed_AM   < 1) i16_LightLevelRed_AM   = 1;
        i16_LightLevelGreen_AM -= i16_G_10perc; if (i16_LightLevelGreen_AM < 1) i16_LightLevelGreen_AM = 1;
        i16_LightLevelBlue_AM  -= i16_B_10perc; if (i16_LightLevelBlue_AM  < 1) i16_LightLevelBlue_AM  = 1;
        i16_LightLevelUv_AM    -= i16_U_10perc; if (i16_LightLevelUv_AM    < 1) i16_LightLevelUv_AM    = 1;
        i16_LightLevelWhite_AM -= i16_W_10perc; if (i16_LightLevelWhite_AM < 1) i16_LightLevelWhite_AM = 1;
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
    // Коррекция канала свечения и его яркости
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
    }
    // Уменьшение яркости строба
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
      // 
    }
    //   
    // Изменение канала строба
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
    }
    if( u8_ManualButton == MANUAL_MODE_BUTTON_LIGHT_DOWN )
    {   u8_ManualButton = MANUAL_MODE_BUTTON_UNDEF;
        i16_LightLevelRed_AM   -= i16_R_10perc; if (i16_LightLevelRed_AM   < 1) i16_LightLevelRed_AM   = 1;
        i16_LightLevelGreen_AM -= i16_G_10perc; if (i16_LightLevelGreen_AM < 1) i16_LightLevelGreen_AM = 1;
        i16_LightLevelBlue_AM  -= i16_B_10perc; if (i16_LightLevelBlue_AM  < 1) i16_LightLevelBlue_AM  = 1;
        i16_LightLevelUv_AM    -= i16_U_10perc; if (i16_LightLevelUv_AM    < 1) i16_LightLevelUv_AM    = 1;
        i16_LightLevelWhite_AM -= i16_W_10perc; if (i16_LightLevelWhite_AM < 1) i16_LightLevelWhite_AM = 1;
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
