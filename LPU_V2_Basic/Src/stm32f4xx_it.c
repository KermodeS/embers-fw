/** 
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput/Src/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-February-2017
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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

#include "stm32f4xx_it.h"
#include "stdbool.h"   

#include "LLU_V2_LightControl.h"
#include "LLU_V2_IRDA.h"
#include "LLU_V2_ESP32.h"
#include "LLU_V2_KBD.h"

/** @addtogroup STM32F4xx_LL_Examples
  * @{
  */

/** @addtogroup TIM_PWMOutput
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}
//
extern uint32_t u32_RedChannelWaitFlag   ;
extern uint32_t u32_GreenChannelWaitFlag ;
extern uint32_t u32_BlueChannelWaitFlag  ;
extern uint32_t u32_UvChannelWaitFlag    ;
extern uint32_t u32_WhiteChannelWaitFlag ;
//
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    u32_RedChannelWaitFlag   = 0;
  u32_GreenChannelWaitFlag = 0;
  u32_BlueChannelWaitFlag  = 0;
  u32_UvChannelWaitFlag    = 0;
  u32_WhiteChannelWaitFlag = 0;
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
* @brief  This function handles TIM2 interrupt.
* @param  None
* @retval None
*/
extern  uint32_t TimOutClock ;
uint16_t u16_Stage=0;

//uint32_t u32_Trigger=100;

uint16_t u16_MyCounter = 0;
//
//
//
// ---------------------------------------- // 
extern bool      b_UpdateRedLightLevel  ;
extern bool      b_RedLightLevelUpdated ;
//
extern uint16_t  u16_ActualRedPeriod    ;
extern uint16_t  u16_ActualRedFreq      ;
// ---------------------------------------- // 
//
// ---------------------------------------- //  
extern bool      b_UpdateGreenLightLevel  ;
extern bool      b_GreenLightLevelUpdated ;
//
extern uint16_t  u16_ActualGreenPeriod    ;
extern uint16_t  u16_ActualGreenFreq      ;
// ---------------------------------------- // 
//
// ---------------------------------------- // 
extern bool      b_UpdateBlueLightLevel  ;
extern bool      b_BlueLightLevelUpdated ;
//
extern uint16_t  u16_ActualBluePeriod    ;
extern uint16_t  u16_ActualBlueFreq      ;
// ---------------------------------------- // 
//
// ---------------------------------------- // 
extern bool      b_UpdateUvLightLevel  ;
extern bool      b_UvLightLevelUpdated ;
//
extern uint16_t  u16_ActualUvPeriod    ;
extern uint16_t  u16_ActualUvFreq      ;
// ---------------------------------------- // 
//
// ---------------------------------------- // 
extern bool      b_UpdateWhiteLightLevel  ;
extern bool      b_WhiteLightLevelUpdated ;
//
extern uint16_t  u16_ActualWhitePeriod    ;
extern uint16_t  u16_ActualWhiteFreq      ;
// ---------------------------------------- // 
//
extern uint8_t  u8_StateMaschine     ;
extern uint8_t  u8_Mode_Sqnt_Status  ;
extern uint8_t  u8_Mode_Rnbw_Status  ;
extern uint8_t  u8_ManualButton      ;
extern int32_t i32_GlobalDutyCounter ;
extern int8_t   i8_GlobalDutyIndex   ;
//
//
extern uint32_t u32_Mode_Sqnt_SM      ; 
extern uint32_t u32_Mode_Rnbw_SM      ; 

//extern uint16_t  u16_RedRefference_MP  [256] ;// Индексы шим таблицы 
//extern uint16_t  u16_LightLevel;
extern uint32_t  u32_LightLevelRed   ;
extern uint32_t  u32_LightLevelGreen ;
extern uint32_t  u32_LightLevelBlue  ;
extern uint32_t  u32_LightLevelUv    ;
extern uint32_t  u32_LightLevelWhite ;
//
extern int16_t  i16_LightLevelRed_AM   ; // ~10-20 % from RED_MAX_INDEX
extern int16_t  i16_LightLevelGreen_AM ; // ~10-20 % 
extern int16_t  i16_LightLevelBlue_AM  ; // ~10-20 %
extern int16_t  i16_LightLevelUv_AM    ; // ~10-20 %
extern int16_t  i16_LightLevelWhite_AM ; // ~10-20 %

//
#ifdef LPU_V2_DEVICE  
//
extern 
const uint8_t         
u8_RedLightRepeatFactor_LP  [  RED_LEVEL_MAX_LP ];
//
extern 
const uint8_t         
u8_GreenLightRepeatFactor_LP[ GREEN_LEVEL_MAX_LP ];
//
extern 
const uint8_t         
u8_BlueLightRepeatFactor_LP[ BLUE_LEVEL_MAX_LP ];
//

extern 
const uint8_t         
u8_UvLightRepeatFactor_LP[ UV_LEVEL_MAX_LP ];
//

extern 
const uint8_t         
u8_WhiteLightRepeatFactor_LP[ WHITE_LEVEL_MAX_LP ];
//
#endif    
//
#ifdef MPU_V2_DEVICE     
//
extern 
const uint8_t         
u8_RedLightRepeatFactor_MP  [  RED_LEVEL_MAX_MP ];
//
extern 
const uint8_t         
u8_GreenLightRepeatFactor_MP[ GREEN_LEVEL_MAX_MP ];
//
extern 
const uint8_t         
u8_BlueLightRepeatFactor_MP[ BLUE_LEVEL_MAX_MP ];
//

extern 
const uint8_t         
u8_UvLightRepeatFactor_MP[ UV_LEVEL_MAX_MP ];
//

extern 
const uint8_t         
u8_WhiteLightRepeatFactor_MP[ WHITE_LEVEL_MAX_MP ];
//
#endif     
//
#ifdef HPU_V2_DEVICE      
//
extern 
const uint8_t         
u8_RedLightRepeatFactor_HP  [  RED_LEVEL_MAX_HP ];
//
extern 
const uint8_t         
u8_GreenLightRepeatFactor_HP[ GREEN_LEVEL_MAX_HP ];
//
extern 
const uint8_t         
u8_BlueLightRepeatFactor_HP[ BLUE_LEVEL_MAX_HP ];
//

extern 
const uint8_t         
u8_UvLightRepeatFactor_HP[ UV_LEVEL_MAX_HP ];
//

extern 
const uint8_t         
u8_WhiteLightRepeatFactor_HP[ WHITE_LEVEL_MAX_HP ];
//
#endif    
    
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
extern bool b_TurnOffPatternActive;

//
// ЗЕЛЕНЫЙ GREEN
void TIM3_IRQHandler(void)
{
  static uint16_t u16_LightLevel=GRN_OFFSET;
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  //
  static uint32_t u32_SubCounter   = 0; // 
  static uint32_t u32_SubCounterLevel   = 1; //&((uint32_t) ((u8_RedLightRepeatFactor_MP))); //   
  //
  
  if(LL_TIM_IsActiveFlag_CC3(TIM3) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC3(TIM3);
    //
    if (b_TurnOffPatternActive == true)
    {
      u16_LightLevel    = GRN_OFFSET ;
    }
    else
    if ( b_GreenLightLevelUpdated == true )
    {
      if (
            ( (u8_StateMaschine ==  SM_MODE_SEQUENTIAL ) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN) )
            ||
            ( (u8_StateMaschine ==  SM_MODE_RAINBOW    ) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN) )    
         ) 
      {
          if  (  
                (  (u32_Mode_Sqnt_SM == SM_MODE_GREEN_UP) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN) )  
                ||  
                (  (u32_Mode_Rnbw_SM == SM_MODE_GREEN_UP) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN) )
              )
          {        //
                #ifdef  LPU_V2_DEVICE      
                UpdateGreenLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                 
                #endif    
                #ifdef MPU_V2_DEVICE   // 20..1919      // Индексы шим таблицы  0.255 
                UpdateGreenLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                #endif            
                #ifdef HPU_V2_DEVICE   
                UpdateGreenLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                          
                #endif
                //
                u32_SubCounter++;
                if ( u32_SubCounter>= u32_SubCounterLevel)
                {
                  u32_SubCounter = 0;
                  u16_LightLevel ++ ; // u32_LightLevelRed++; 
                  //
                  u32_SubCounterLevel = u8_GreenLightRepeatFactor_LP[u16_LightLevel-GRN_OFFSET]; 
                  //
                }
                //
                if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
                {
                  // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                  // без синхро
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel>=i16_LightLevelGreen_AM-2) 
                    {
                      u32_Mode_Sqnt_SM    = SM_MODE_GREEN_DOWN;                       
                    } 
                  }
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel>=i16_LightLevelGreen_AM-2)
                    {
                      u32_Mode_Sqnt_SM    = SM_MODE_GREEN_DOWN;          
                      u8_Mode_Sqnt_Status = MODE_STATUS_STOP;                    
                    } 
                  }                                
                }
                else
                if( u8_StateMaschine ==  SM_MODE_RAINBOW )
                {
                  // КОД СТРОГО ДЛЯ РАДУГИ
                  // без синхро
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel>=i16_LightLevelGreen_AM-2)  // Радуга. Макс желтый.                               
                    {
                       u32_Mode_Rnbw_SM    = SM_MODE_RED_DOWN;  // Макс желтый, получаем зеленый                         
                    } 
                  }
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel>=i16_LightLevelGreen_AM-2)
                    {
                       u32_Mode_Rnbw_SM    = SM_MODE_RED_DOWN;   // Макс красный, получаем зеленый   
                        u8_Mode_Rnbw_Status = MODE_STATUS_STOP;                    
                    } 
                  }                                
                }
          }       
          else
            if (
                  ( ( u32_Mode_Sqnt_SM == SM_MODE_GREEN_DOWN ) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN) )
                  || 
                  ( ( u32_Mode_Rnbw_SM == SM_MODE_GREEN_DOWN ) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN) )
                )    
            {    //
                  #ifdef  LPU_V2_DEVICE          
                  UpdateGreenLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                   
                  #endif
                  #ifdef MPU_V2_DEVICE   // 20 -1919                             0- 255
                                  //   u16_LightLevel = u16_RedRefference_MP [u32_LightLevelRed]; 
                  UpdateGreenLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                  #endif
                  #ifdef HPU_V2_DEVICE   
                  UpdateGreenLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                           
                  #endif
                  //
                  u32_SubCounter++;
                  if ( u32_SubCounter>= u32_SubCounterLevel)
                  {
                     u32_SubCounter = 0;
                     u16_LightLevel -- ; //u32_LightLevelRed--; 
                     //
                     u32_SubCounterLevel = u8_GreenLightRepeatFactor_LP[u16_LightLevel-GRN_OFFSET]; 
                     //                 
                  }
                  //
                  if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
                  {
                    // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                    if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                    {
                      if (u16_LightLevel==(GRN_OFFSET-1))  
                      { // Загашен зеленый. Разжигаем синий
                        u32_Mode_Sqnt_SM = SM_MODE_BLUE_UP; 
                     // u8_Mode_Sqnt_Status = MODE_STATUS_STOP;
                      }
                    }
                    //
                    // Работа с синхронизацией
                    if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                    {
                      if (u16_LightLevel==(GRN_OFFSET-1))  //  == 0    u32_LightLevelRed
                      { // Загашен зеленый. Разжигаем синий
                        u32_Mode_Sqnt_SM = SM_MODE_BLUE_UP; 
                        u8_Mode_Sqnt_Status = MODE_STATUS_STOP;
                      }
                    }                                  
                  }
                  else
                  if( u8_StateMaschine ==  SM_MODE_RAINBOW )
                  {
                    // КОД СТРОГО ДЛЯ РАДУГИ
                    if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                    {
                      if (u16_LightLevel==(GRN_OFFSET-1))  
                      { // Загашен зеленый. Горит только синий. Гасим синий
                        u32_Mode_Rnbw_SM = SM_MODE_BLUE_DOWN; 
                     // u8_Mode_Sqnt_Status = MODE_STATUS_STOP;
                      }
                    }
                    //
                    // Работа с синхронизацией
                    if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                    {
                      if (u16_LightLevel==(GRN_OFFSET-1))  //  == 0    u32_LightLevelRed
                      { // Загашен зеленый. Горит только синий. Гасим синий
                        u32_Mode_Rnbw_SM = SM_MODE_BLUE_DOWN; 
                        u8_Mode_Rnbw_Status = MODE_STATUS_STOP;
                      }
                    }   
                  } 
                  //     
        }
      }  
      
    }
    //    
    //
    // А это фрагмент для обновления яркости свечения. 
    if ( b_UpdateGreenLightLevel)
    {
      b_UpdateGreenLightLevel  = false;
      //
      u32_Period = u16_ActualGreenPeriod;
      u32_Freq   = u16_ActualGreenFreq  ;
      //
      LL_TIM_OC_SetCompareCH3(TIM3, u32_Period );
      LL_TIM_SetAutoReload   (TIM3, u32_Freq   );
      //
      b_GreenLightLevelUpdated = true;
    }
    Timer3CaptureCompare_Callback();
  }
  //  
}
//
//
//  БЕЛЫЙ WHITE
void TIM4_IRQHandler(void)
{
  static uint16_t u16_LightLevel=WHT_OFFSET;
  //
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  static uint32_t u32_SubCounter   = 0; // 
  static uint32_t u32_SubCounterLevel   = 1; //&((uint32_t) ((u8_RedLightRepeatFactor_MP))); //   
  //
  if(LL_TIM_IsActiveFlag_CC2(TIM4) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM4);
    //
    if (b_TurnOffPatternActive == true)
      u16_LightLevel = WHT_OFFSET;
    else
    if (b_WhiteLightLevelUpdated == true)
    {
      if (
           ( (u8_StateMaschine ==  SM_MODE_SEQUENTIAL ) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN ) )
           ||
           ( (u8_StateMaschine ==  SM_MODE_RAINBOW    ) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN ) ) 
          )
      {
          if ( 
                ( ( u32_Mode_Sqnt_SM == SM_MODE_WHITE_UP)  && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN ) )
                ||    
                ( ( u32_Mode_Rnbw_SM == SM_MODE_WHITE_UP)  && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN ) )
              )
          {        //
                #ifdef  LPU_V2_DEVICE      
                UpdateWhiteLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                  
                #endif    
                #ifdef MPU_V2_DEVICE   // 20..1919      // Индексы шим таблицы  0.255 
                UpdateWhiteLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                #endif            
                #ifdef HPU_V2_DEVICE   
                UpdateWhiteLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                            
                #endif
               //
                u32_SubCounter++;
                if ( u32_SubCounter>= u32_SubCounterLevel)
                {
                  u32_SubCounter = 0;
                  u16_LightLevel ++ ; // u32_LightLevelRed++; 
                  
                  u32_SubCounterLevel = u8_WhiteLightRepeatFactor_LP[u16_LightLevel-WHT_OFFSET];   
                }
                //
                if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
                {
                  // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                  //
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel>=i16_LightLevelWhite_AM-2) 
                      u32_Mode_Sqnt_SM = SM_MODE_WHITE_DOWN;  
                  }
                  //
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel>=i16_LightLevelWhite_AM-2) 
                    {
                      u32_Mode_Sqnt_SM    = SM_MODE_WHITE_DOWN;       
                      u8_Mode_Sqnt_Status = MODE_STATUS_STOP;  
                    }
                  }                                
                }
                else
                if( u8_StateMaschine ==  SM_MODE_RAINBOW )
                {
                  // КОД СТРОГО ДЛЯ РАДУГИ
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel>=i16_LightLevelWhite_AM-2) 
                      u32_Mode_Rnbw_SM = SM_MODE_WHITE_DOWN;   
                  }
                  //
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel>=i16_LightLevelWhite_AM-2) 
                    {
                      u32_Mode_Rnbw_SM    = SM_MODE_WHITE_DOWN;       
                      u8_Mode_Rnbw_Status = MODE_STATUS_STOP;  
                    }
                  }                                

                }

          }       
          else
            if ( 
                  ( (u32_Mode_Sqnt_SM == SM_MODE_WHITE_DOWN) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN ) ) 
                  || 
                  ( (u32_Mode_Rnbw_SM == SM_MODE_WHITE_DOWN) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN ) ) 
                )  
            {    //
                  #ifdef  LPU_V2_DEVICE          
                  UpdateWhiteLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                
                  #endif
                  #ifdef MPU_V2_DEVICE   // 20 -1919                             0- 255
                                  //   u16_LightLevel = u16_RedRefference_MP [u32_LightLevelRed]; 
                  UpdateWhiteLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                  #endif
                  #ifdef HPU_V2_DEVICE   
                  UpdateWhiteLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                       
                  #endif
                //
              //LL_mDelay(u8_Mode_A_StepDelay_ms); 
                               u32_SubCounter++;
                if ( u32_SubCounter>= u32_SubCounterLevel)
                {
                   u32_SubCounter = 0;
                   u16_LightLevel -- ; //u32_LightLevelRed--; 
                   //
                   u32_SubCounterLevel = u8_WhiteLightRepeatFactor_LP[u16_LightLevel-WHT_OFFSET]; 
                   //                
                } 
                //
                if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
                {
                  // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel==(WHT_OFFSET-1))  
                     u32_Mode_Sqnt_SM = SM_MODE_RED_UP;
                    //
                  }
                  //
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel==(WHT_OFFSET-1))  
                    {
                      u32_Mode_Sqnt_SM = SM_MODE_RED_UP;    
                      u8_Mode_Sqnt_Status = MODE_STATUS_STOP;
                    }
                  }                                
                }
                else
                if( u8_StateMaschine ==  SM_MODE_RAINBOW )
                {
                  // КОД СТРОГО ДЛЯ РАДУГИ
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel==(WHT_OFFSET-1))   
                     u32_Mode_Rnbw_SM = SM_MODE_RED_UP;
                    //
                  }
                  //
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel==(WHT_OFFSET-1))  
                    {
                      u32_Mode_Rnbw_SM = SM_MODE_RED_UP;    
                      u8_Mode_Rnbw_Status = MODE_STATUS_STOP;
                    }
                  }                                
                }
            } 
      }      
    }   
    //
    if ( b_UpdateWhiteLightLevel)
    {
      b_UpdateWhiteLightLevel  = false;
      //
      u32_Period = u16_ActualWhitePeriod;
      u32_Freq   = u16_ActualWhiteFreq  ;
      //
      LL_TIM_OC_SetCompareCH2(TIM4, u32_Period );
      LL_TIM_SetAutoReload   (TIM4, u32_Freq   );
      //
      b_WhiteLightLevelUpdated = true;
    }
    Timer4CaptureCompare_Callback();
  }
  //  
}
//
//
// Blue
void TIM2_IRQHandler(void)
{
  static uint16_t u16_LightLevel=BLU_OFFSET;
  //
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  static uint32_t u32_SubCounter   = 0; // 
  static uint32_t u32_SubCounterLevel   = 1; //&((uint32_t) ((u8_RedLightRepeatFactor_MP))); //   
  //
  //
  if(LL_TIM_IsActiveFlag_CC2(TIM2) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM2);
    //
    if (b_TurnOffPatternActive == true)
      u16_LightLevel = BLU_OFFSET;
    else
    if (b_BlueLightLevelUpdated == true)
    if (  
          ( ( u8_StateMaschine ==  SM_MODE_SEQUENTIAL ) && (u8_Mode_Sqnt_Status == MODE_STATUS_RUN) )
          ||
          ( ( u8_StateMaschine ==  SM_MODE_RAINBOW    ) && (u8_Mode_Rnbw_Status == MODE_STATUS_RUN) )
        )
    {
        if ( 
              ( (u32_Mode_Sqnt_SM == SM_MODE_BLUE_UP)  && (u8_Mode_Sqnt_Status == MODE_STATUS_RUN) )   
              || 
              ( (u32_Mode_Rnbw_SM == SM_MODE_BLUE_UP)  && (u8_Mode_Rnbw_Status == MODE_STATUS_RUN) )
            ) 
        {        //
              #ifdef  LPU_V2_DEVICE      
              UpdateBlueLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                  
              #endif    
              #ifdef MPU_V2_DEVICE   // 20..1919      // Индексы шим таблицы  0.255 
              UpdateBlueLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
              #endif            
              #ifdef HPU_V2_DEVICE   
              UpdateBlueLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                           
              #endif
             //
              u32_SubCounter++;
              if ( u32_SubCounter>= u32_SubCounterLevel)
              {
                u32_SubCounter = 0;
                u16_LightLevel ++ ; // u32_LightLevelRed++; 
                //
                u32_SubCounterLevel = u8_BlueLightRepeatFactor_LP[u16_LightLevel-BLU_OFFSET];   
                //
              }
              //
              // И для последовательного  и для радуги поведение единообразно. 
              // изменяются только сходные по функционалу переменные.
              if( u8_StateMaschine ==  SM_MODE_SEQUENTIAL  ) 
              {
                // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                {
                  if (u16_LightLevel>=i16_LightLevelBlue_AM-2) 
                  {
                    u32_Mode_Sqnt_SM = SM_MODE_BLUE_DOWN;  
                  }
                }
                //
                // Работа с синхронизацией
                if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                {
                  if (u16_LightLevel>=i16_LightLevelBlue_AM-2) 
                  {
                    u32_Mode_Sqnt_SM = SM_MODE_BLUE_DOWN; 
                    u8_Mode_Sqnt_Status = MODE_STATUS_STOP;  
                  }
                }                                
              }
              else
              if( u8_StateMaschine ==  SM_MODE_RAINBOW )
              {
                // КОД СТРОГО ДЛЯ РАДУГИ
                if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                {
                  if (u16_LightLevel>=i16_LightLevelBlue_AM-2) 
                  {
                    u32_Mode_Rnbw_SM = SM_MODE_GREEN_DOWN; 
                  }
                }
                //
                // Работа с синхронизацией
                if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                {
                  if (u16_LightLevel>=i16_LightLevelBlue_AM-2) 
                  {
                    u32_Mode_Rnbw_SM = SM_MODE_GREEN_DOWN; 
                    u8_Mode_Rnbw_Status = MODE_STATUS_STOP;  
                  }
                } 
              }

        }       
        else
          if ( 
                (  (u32_Mode_Sqnt_SM == SM_MODE_BLUE_DOWN) && (u8_Mode_Sqnt_Status == MODE_STATUS_RUN) ) 
                ||  
                (  (u32_Mode_Rnbw_SM == SM_MODE_BLUE_DOWN) && (u8_Mode_Rnbw_Status == MODE_STATUS_RUN) )
              )   
          {    //
                #ifdef  LPU_V2_DEVICE          
                UpdateBlueLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                  
                #endif
                #ifdef MPU_V2_DEVICE   // 20 -1919                             0- 255
                                //   u16_LightLevel = u16_RedRefference_MP [u32_LightLevelRed]; 
                UpdateBlueLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                #endif
                #ifdef HPU_V2_DEVICE   
                UpdateBlueLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                          
                #endif
              //
            //LL_mDelay(u8_Mode_A_StepDelay_ms); 
                   u32_SubCounter++;
              if ( u32_SubCounter>= u32_SubCounterLevel)
              {
                 u32_SubCounter = 0;
                 u16_LightLevel -- ; //u32_LightLevelRed--; 
                 //
                 u32_SubCounterLevel = u8_BlueLightRepeatFactor_LP[u16_LightLevel-BLU_OFFSET]; 
                 //
              } 
              //
              if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
              {
                // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                // И для последовательного  и для радуги поведение единообразно. 
                // изменяются только сходные по функционалу переменные. 
                if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                {
                  if (u16_LightLevel==(BLU_OFFSET-1))  
                  {
                    u32_Mode_Sqnt_SM = SM_MODE_UV_UP;    
                  }
                } 
                //
                // Работа с синхронизацией
                if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                {
                  if (u16_LightLevel==(BLU_OFFSET-1))  
                  {
                    u32_Mode_Sqnt_SM = SM_MODE_UV_UP;    
                    u8_Mode_Sqnt_Status = MODE_STATUS_STOP;
                  }
                }                 
              }
              else
              if( u8_StateMaschine ==  SM_MODE_RAINBOW )
              {
                // КОД СТРОГО ДЛЯ РАДУГИ
                if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                {
                  if (u16_LightLevel==(BLU_OFFSET-1))  
                  {
                    u32_Mode_Rnbw_SM = SM_MODE_UV_UP;    
                  }
                } 
                //
                // Работа с синхронизацией
                if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                {
                  if (u16_LightLevel==(BLU_OFFSET-1))  
                  {
                    u32_Mode_Rnbw_SM = SM_MODE_UV_UP;    
                    u8_Mode_Rnbw_Status = MODE_STATUS_STOP;
                  }
                }
              }
              
          } 
      
    }
    //   
    //
    if ( b_UpdateBlueLightLevel)
    {
      b_UpdateBlueLightLevel  = false;
      //
      u32_Period = u16_ActualBluePeriod;
      u32_Freq   = u16_ActualBlueFreq  ;
      //
      LL_TIM_OC_SetCompareCH2(TIM2, u32_Period );
      LL_TIM_SetAutoReload   (TIM2, u32_Freq   );
      //
      b_BlueLightLevelUpdated = true;
    }
    Timer2CaptureCompare_Callback();
  }
  // 
}
//
//
//
// PB9_TIM11_CH1  - RED
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
   static uint16_t u16_LightLevel=RED_OFFSET;
  //
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  static uint32_t u32_SubCounter   = 0; // 
  static uint32_t u32_SubCounterLevel   = 1; //&((uint32_t) ((u8_RedLightRepeatFactor_MP))); //   
  //
  if(LL_TIM_IsActiveFlag_CC1(TIM11) == 1) // Красный таймер
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC1(TIM11);
    //
    //
    if (b_TurnOffPatternActive == true)
      u16_LightLevel = RED_OFFSET;
    else
    if (b_RedLightLevelUpdated == true)
    {
      //
      if (
            (  (u8_StateMaschine ==  SM_MODE_SEQUENTIAL) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN )  )
            ||
            (  (u8_StateMaschine ==  SM_MODE_RAINBOW   ) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN )  )  
          )       
      {
          if ( 
                ( ( u32_Mode_Sqnt_SM  == SM_MODE_RED_UP ) && (u8_StateMaschine ==  SM_MODE_SEQUENTIAL))
                || 
                ( ( u32_Mode_Rnbw_SM  == SM_MODE_RED_UP ) && (u8_StateMaschine ==  SM_MODE_RAINBOW   ) )
              )   
          {     
                //                     RED_OFFSET..2400
                #ifdef  LPU_V2_DEVICE      
                UpdateRedLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                 
                #endif    
                #ifdef MPU_V2_DEVICE   // 20..1919      // Индексы шим таблицы  0.255 
                UpdateRedLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                #endif            
                #ifdef HPU_V2_DEVICE   
                UpdateRedLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                          
                #endif
                //
                // Независимо от наличия/отсутствия синхронизации
                u32_SubCounter++;
                if ( u32_SubCounter>= u32_SubCounterLevel)
                {
                  u32_SubCounter = 0;
                  u16_LightLevel ++ ; // u32_LightLevelRed++; 
                  //    
                  u32_SubCounterLevel = u8_RedLightRepeatFactor_LP[u16_LightLevel-RED_OFFSET];
                  //
                }  
                //
                if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
                {
                  // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                  // работа без синхронизации
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel>=i16_LightLevelRed_AM-2) 
                     u32_Mode_Sqnt_SM = SM_MODE_RED_DOWN;  
                  }
                  //
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel>=i16_LightLevelRed_AM-2) 
                    {
                      u32_Mode_Sqnt_SM    = SM_MODE_RED_DOWN; // Красный в максимуме. Надо тушить.
                      u8_Mode_Sqnt_Status = MODE_STATUS_STOP; 
                    }
                  } 
                }
                else
                  if( u8_StateMaschine ==  SM_MODE_RAINBOW )
                  {
                    // КОД СТРОГО ДЛЯ РАДУГИ
                    // работа без синхронизации
                    if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                    {
                      if (u16_LightLevel>=i16_LightLevelRed_AM-2) 
                      {
                      //  u16_LightLevel =0; // Частный случай
                       u32_Mode_Rnbw_SM = SM_MODE_GREEN_UP;  // Красный в максимуме. Активируем зеленый.
                      }
                    }
                    //
                    // Работа с синхронизацией
                    if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                    {
                      if (u16_LightLevel>=i16_LightLevelRed_AM-2) 
                      {
                       //  u16_LightLevel =0; // Частный случай
                        u32_Mode_Rnbw_SM = SM_MODE_GREEN_UP; // Красный в максимуме. Активируем зеленый.
                        u8_Mode_Rnbw_Status = MODE_STATUS_STOP; 
                      }
                    } 
                  }

          }       
          else
            if ( 
                 ( (u32_Mode_Sqnt_SM  == SM_MODE_RED_DOWN) && ( u8_StateMaschine ==  SM_MODE_SEQUENTIAL ) ) 
                 || 
                 ( (u32_Mode_Rnbw_SM  == SM_MODE_RED_DOWN) && ( u8_StateMaschine ==  SM_MODE_RAINBOW    ) )  
                )    
            {    //
                  #ifdef  LPU_V2_DEVICE          
                  UpdateRedLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                
                  #endif
                  #ifdef MPU_V2_DEVICE   // 20 -1919                             0- 255
                                  //   u16_LightLevel = u16_RedRefference_MP [u32_LightLevelRed]; 
                  UpdateRedLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                  #endif
                  #ifdef HPU_V2_DEVICE   
                  UpdateRedLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                         
                  #endif
                  //
                  //
                  u32_SubCounter++;
                  //
                  if ( u32_SubCounter>= u32_SubCounterLevel)
                  {
                    u32_SubCounter = 0;
                    u16_LightLevel -- ; 
                    //
                    u32_SubCounterLevel = u8_RedLightRepeatFactor_LP[u16_LightLevel-RED_OFFSET];
                    //
                  }                  
                  //
                  if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
                  {
                    // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                    if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                    {
                      if (u16_LightLevel==(RED_OFFSET-1))  // 
                      {
                        // u16_LightLevel =0; // Частный случай
                         u32_Mode_Sqnt_SM = SM_MODE_GREEN_UP;                          
                      }

                    }
                    //
                    // Работа с синхронизацией
                    if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                    {
                      if (u16_LightLevel==(RED_OFFSET-1))  //  
                      {
                         //u16_LightLevel =0; // Частный случай
                        u32_Mode_Sqnt_SM = SM_MODE_GREEN_UP; // Потушили красный и надо зажигать зеленый
                        u8_Mode_Sqnt_Status = MODE_STATUS_STOP;
                      }                   
                    } 
                  }
                  else
                  if( u8_StateMaschine ==  SM_MODE_RAINBOW )
                  {
                    // КОД СТРОГО ДЛЯ РАДУГИ
                    if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                    {
                      if (u16_LightLevel==(RED_OFFSET-1))  // 
                      {
                         //u16_LightLevel =0; // Частный случай
                        u32_Mode_Rnbw_SM = SM_MODE_BLUE_UP; // Потушили красный и надо зажигать СИНИЙ.
                      }
                         
                    }
                    //
                    // Работа с синхронизацией
                    if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                    {
                      if (u16_LightLevel==(RED_OFFSET-1))  //  
                      {
                         //u16_LightLevel =0; // Частный случай
                        u32_Mode_Rnbw_SM = SM_MODE_BLUE_UP; // Потушили красный и надо зажигать СИНИЙ.
                        u8_Mode_Rnbw_Status = MODE_STATUS_STOP;
                      }                   
                    }
                  }
            } 
      }
      //      
    }
    //
    // код для режима RAINBOW. Надо добавить.
    //    
    // Оценка внешнего флага и применение нового значения частоты и скважности.
    // Флаг устанавливается в void SetRedLevel(uint16_t u16_LightIndex)
    if ( b_UpdateRedLightLevel)
    {
      b_UpdateRedLightLevel  = false;
      //
      u32_Period = u16_ActualRedPeriod;
      u32_Freq   = u16_ActualRedFreq  ;
      //
      LL_TIM_OC_SetCompareCH1(TIM11, u32_Period );
      LL_TIM_SetAutoReload   (TIM11, u32_Freq   );
      //
      b_RedLightLevelUpdated = true;
    }  
    //  
    Timer11CaptureCompare_Callback();
    //
  }
  
  /* ++++ ????    */
  // Это вторая часть обработчика прерывания совмещенного таймера.
  if(LL_TIM_IsActiveFlag_CC2(TIM1) == 1) // ТИМ 1 таймер
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM1);
    //
    Timer1CaptureCompare_Callback();
  }
  
}
//
/*
uint32_t u32_IrdaRX_ActualState = 1;
uint32_t u32_IrdaRX_PrevState   = 1;
//
//
   
//
extern uint16_t u16_IR_RX_captureCounter;
extern uint16_t u16_IR_RX_capture [1024];
extern uint16_t u16_captureActual;
//
extern bool b_IR_RX_PacketReceived ; //== false) && (
extern bool b_IR_RX_PacketProcessed; // true 
//
//
extern int32_t i32_GlobalDutyCounter   ; 
//
extern uint8_t u8_RedLightDelayDutyCounter ;  // int16_t i16 
extern uint8_t u8_GreenLightDelayDutyCounter ; 
extern uint8_t u8_BlueLightDelayDutyCounter  ; 
extern uint8_t u8_UvLightDelayDutyCounter    ; 
extern uint8_t u8_WhiteLightDelayDutyCounter ; 
//
extern uint8_t u8_ProcessWiFiCommand ;

//
extern uint16_t u16_Mode_Strob_Counter_ms ;
extern uint16_t u16_Mode_Strob_SubCounter ;
//
extern uint8_t u8_Mode_Strob_Status ;
extern uint8_t u8_Mode_Strobe_SM    ;  
*/
//

//
uint32_t u32_IrdaRX_ActualState = 1;
uint32_t u32_IrdaRX_PrevState   = 1;
//
//
   
//
extern uint16_t u16_IR_RX_captureCounter;
extern uint16_t u16_IR_RX_capture [1024];
extern uint16_t u16_captureActual;
//
extern bool b_IR_RX_PacketReceived ; //== false) && (
extern bool b_IR_RX_PacketProcessed; // true 
//
//
extern int32_t i32_GlobalDutyCounter   ; 
//
extern uint32_t u32_RedLightDelayDutyCounter ;  // int16_t i16 
extern uint32_t u32_GreenLightDelayDutyCounter ; 
extern uint32_t u32_BlueLightDelayDutyCounter  ; 
extern uint32_t u32_UvLightDelayDutyCounter    ; 
extern uint32_t u32_WhiteLightDelayDutyCounter ; 
//
extern uint8_t u8_ProcessWiFiCommand ;
extern bool     b_Mode_Sqnt_CallBack ;
extern __IO uint8_t ubDmaTransferStatus ;
extern uint16_t u16_StartADC ;
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
extern bool b_Send_ESP32_LED1_Packet   ;
extern bool b_Send_ESP32_LED2_Packet   ;
extern bool b_Send_ESP32_LED3_Packet   ;
extern bool b_Send_ESP32_LED4_Packet   ;
extern bool b_Send_ESP32_LED5_Packet   ;
extern bool b_Send_ESP32_LED6_Packet   ;
extern bool b_Send_ESP32_LED7_Packet   ;
extern bool b_Send_ESP32_LED8_Packet   ;
//
extern uint32_t u32_SyncLostTimer_mks      ; 
//
extern uint8_t u8_Mode_Strob_Status;
extern uint8_t u8_Mode_Strobe_SM;
extern   bool  b_StopStrobMode ;
extern uint8_t u8_StateMaschine ;
//
extern uint32_t SysTimerData[10];
//
extern bool b_Grouped ;
//
//
extern uint8_t u8_ManualChannel;
//
//
void ProcessStrobeMode(void );
void ProcessStrobeMode(void )
{
  static uint32_t u32_WaitCounter=0;
  static uint32_t u32_WaitFactor = 570;//0.015/(1/38000)=570;
  
  //  
  if  ( ( u8_Mode_Strob_Status == MODE_STATUS_RUN ) &&( u8_StateMaschine == SM_MODE_STROBOSCOPE ) ) 
  {
    switch (u8_Mode_Strobe_SM)
    {
       case SM_MODE_STROB_ON:
        if ( u8_ManualChannel == MANUAL_MODE_RED   )   SetRedLevel   ( i16_LightLevelRed_AM   ); else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN )   SetGreenLevel ( i16_LightLevelGreen_AM ); else 
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  )   SetBlueLevel  ( i16_LightLevelBlue_AM  ); else
        if ( u8_ManualChannel == MANUAL_MODE_UV    )   SetUvLevel    ( i16_LightLevelUv_AM    ); else       
        if ( u8_ManualChannel == MANUAL_MODE_WHITE )   SetWhiteLevel ( i16_LightLevelWhite_AM );         
        //   
        u8_Mode_Strobe_SM = SM_MODE_STROB_WAIT_OFF;
        u32_WaitCounter = 0 ;
        break;
      //
    case   SM_MODE_STROB_WAIT_OFF:
        u32_WaitCounter ++ ;
        if (u32_WaitCounter > u32_WaitFactor ) 
        {
          u8_Mode_Strobe_SM = SM_MODE_STROB_OFF ;
        }
         break;      
          
      case SM_MODE_STROB_OFF: 
        if ( u8_ManualChannel == MANUAL_MODE_RED   )   SetRedLevel   ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_GREEN )   SetGreenLevel ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_BLUE  )   SetBlueLevel  ( 0 );  else
        if ( u8_ManualChannel == MANUAL_MODE_UV    )   SetUvLevel    ( 0 );  else       
        if ( u8_ManualChannel == MANUAL_MODE_WHITE )   SetWhiteLevel ( 0 );    
        //
        u8_Mode_Strobe_SM =  SM_MODE_STROB_ALL_DOWN ; 
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
        u8_Mode_Strobe_SM = SM_MODE_STROB_IDLE;
        // 
        break; 
      //  
      case SM_MODE_STROB_IDLE:
             
       break;   
      //  
      default:   
        u8_Mode_Strobe_SM = SM_MODE_STROB_IDLE; 
        break;//+++ 
      //
    }
  } 
}

//
// СЕЙЧАС ЭТО 38 кГц
// От этого таймера еще зависят кнопки.
//
void TIM1_CC_IRQHandler(void)
{
 //
  if(LL_TIM_IsActiveFlag_CC2(TIM1) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM1);
    //
    //
    ProcessControlButtons();
    //
    //
    ProcessWiFiSerial(); 
    Process_ESP32_LEDS();
    //
    ProcessStrobeMode();
    //
    //
    // Контроль пропадания синхронизации
    if ( ( (u8_LED2_State == LED_2_MASTER  ) || ( u8_LED2_State == LED_2_ECHO_SLAVE)  || (u8_LED2_State == LED_2_SLAVE ) )  &&
         ( (u8_LED3_State == LED_3_SentSync) || (u8_LED3_State == LED_3_RecieveSync)  || (u8_LED3_State == LED_3_NoSync)         )
       )
    { 
       if  (u32_SyncLostTimer_mks >= SYNC_LOST_TIME_MKS )
       {
           u8_LED3_State            = LED_3_NoSync;
           b_Grouped = false;
           u32_SyncLostTimer_mks=0;
           
           if (b_StopStrobMode )
           {
             b_StopStrobMode = false;
             u8_LED3_State            = LED_3_DEFAULT;
             b_Grouped = false;
             //
             // u8_Mode_Strob_Status = MODE_STATUS_RUN;
              u8_Mode_Strobe_SM = SM_MODE_STROB_IDLE; 
              u8_StateMaschine = SM_MODE_MANUAL  ;
           }
           else
           {
              u8_Mode_Strob_Status = MODE_STATUS_RUN;
              u8_Mode_Strobe_SM = SM_MODE_STROB_ON; // Запускам машину состояний стробоскопа, которая перешлва в IDLE  
           }
           //
           b_Send_ESP32_LED3_Packet = true;
           //
       }    
       else
        u32_SyncLostTimer_mks+= 13; // 1/ 76000 = 13,158 мкс.  
    }
    //
    if ( i32_GlobalDutyCounter < (2147483648-2) ) i32_GlobalDutyCounter   ++;  
    //
    // Р°РІС‚РѕР·Р°РїСѓСЃРє СЂРµР¶РёРјР° A
    
    // #ifdef LOW_POWER_DEVICE 
    
    if ( b_Mode_Sqnt_CallBack == true )
    {
      b_Mode_Sqnt_CallBack = false;
      u8_ProcessWiFiCommand = WIFI_COMMAND_SET_AUTO_PLAY;
    }
   // #endif
    
   // #ifdef MID_POWER_DEVICE    
    if ( b_Mode_Sqnt_CallBack == true )
    {
      b_Mode_Sqnt_CallBack = false;
      u8_ProcessWiFiCommand = WIFI_COMMAND_SET_AUTO_PLAY;
    }
    // #endif
    /// ; 
  }
  //
  Timer1CaptureCompare_Callback();  
}
//

// FOR UV control
void TIM1_UP_TIM10_IRQHandler(void)
{
  static uint16_t u16_LightLevel=UV_OFFSET;
  //
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  //
  static uint32_t u32_SubCounter   = 0; // 
  static uint32_t u32_SubCounterLevel   = 1; //&((uint32_t) ((u8_RedLightRepeatFactor_MP))); //   
  //
  if(LL_TIM_IsActiveFlag_CC1(TIM10) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC1(TIM10);
    //
    // код для режима SEQUENTIAL/RAINBOW
    //
    if (b_TurnOffPatternActive == true)
      u16_LightLevel = UV_OFFSET;
    else
    if (b_UvLightLevelUpdated == true)
    if (  
          (  (u8_StateMaschine ==  SM_MODE_SEQUENTIAL )  && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN )    )
          ||
          (  (u8_StateMaschine ==  SM_MODE_RAINBOW    )  && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN )    )        
       )
    {
        if ( 
              ( (u32_Mode_Sqnt_SM == SM_MODE_UV_UP) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN )  ) 
              || 
              ( (u32_Mode_Rnbw_SM == SM_MODE_UV_UP) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN )  ) 
            )   
        {        //
              #ifdef  LPU_V2_DEVICE      
              UpdateUvLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                   
              #endif    
              #ifdef MPU_V2_DEVICE   // 20..1919      // Индексы шим таблицы  0.255 
              UpdateUvLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
              #endif            
              #ifdef HPU_V2_DEVICE   
              UpdateUvLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                            
              #endif
             //
              u32_SubCounter++;
              if ( u32_SubCounter>= u32_SubCounterLevel)
              {
                u32_SubCounter = 0;
                u16_LightLevel ++ ; // u32_LightLevelRed++; 
                //
                u32_SubCounterLevel = u8_UvLightRepeatFactor_LP[u16_LightLevel-UV_OFFSET];   
                //

              }
              //
              // для ультрафиолета  последовательный и радуга работают одинаково.
              if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
              {
                  // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА
                  // без синхро
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel>=i16_LightLevelUv_AM-2) 
                      u32_Mode_Sqnt_SM = SM_MODE_UV_DOWN;    
                  }
                  //
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel>=i16_LightLevelUv_AM-2) 
                    {
                      u32_Mode_Sqnt_SM = SM_MODE_UV_DOWN; 
                      u8_Mode_Sqnt_Status = MODE_STATUS_STOP;  
                    }  
                  }                                
              }
              else
              if( u8_StateMaschine ==  SM_MODE_RAINBOW )
              {
                  // КОД СТРОГО ДЛЯ РАДУГИ
                  // без синхро
                  if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                  {
                    if (u16_LightLevel>=i16_LightLevelUv_AM-2) 
                      u32_Mode_Rnbw_SM = SM_MODE_UV_DOWN;    
                  }
                  //
                  // Работа с синхронизацией
                  if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                  {
                    if (u16_LightLevel>=i16_LightLevelUv_AM-2) 
                    {
                      u32_Mode_Rnbw_SM = SM_MODE_UV_DOWN; 
                      u8_Mode_Rnbw_Status = MODE_STATUS_STOP;  
                    }  
                  }  
              }
              //

        }       
        else
          if ( 
               ( (u32_Mode_Sqnt_SM == SM_MODE_UV_DOWN) && ( u8_Mode_Sqnt_Status == MODE_STATUS_RUN ) )
               || 
               ( (u32_Mode_Rnbw_SM == SM_MODE_UV_DOWN) && ( u8_Mode_Rnbw_Status == MODE_STATUS_RUN ) )
              )   
          {    //
                #ifdef  LPU_V2_DEVICE          
                UpdateUvLevelFast_LP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                 
                #endif
                #ifdef MPU_V2_DEVICE   // 20 -1919                             0- 255
                                //   u16_LightLevel = u16_RedRefference_MP [u32_LightLevelRed]; 
                UpdateUvLevelFast_MP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                     
                #endif
                #ifdef HPU_V2_DEVICE   
                UpdateUvLevelFast_HP (u16_LightLevel); // 0 .. xxx_LEVEL_MAX_xP-1                        
                #endif
              //

              u32_SubCounter++;
              if ( u32_SubCounter>= u32_SubCounterLevel)
              {
                 u32_SubCounter = 0;
                 u16_LightLevel -- ; //u32_LightLevelRed--; 
                 //
                 u32_SubCounterLevel = u8_UvLightRepeatFactor_LP[u16_LightLevel-UV_OFFSET]; 
                 //                 
              } 
              //
              // для ультрафиолета  последовательный и радуга работают одинаково.
              if(  u8_StateMaschine ==  SM_MODE_SEQUENTIAL )
              {
                // КОД СТРОГО ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО РЕЖИМА       
                if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                {
                  if (u16_LightLevel==(UV_OFFSET-1))  //  == 0    u32_LightLevelRed
                   u32_Mode_Sqnt_SM = SM_MODE_WHITE_UP;
                }
                //
                // Работа с синхронизацией
                if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                {
                  if (u16_LightLevel==(UV_OFFSET-1))  
                  {
                    u32_Mode_Sqnt_SM = SM_MODE_WHITE_UP;    
                    u8_Mode_Sqnt_Status = MODE_STATUS_STOP;
                  }
                }                
              }
              else
              if( u8_StateMaschine ==  SM_MODE_RAINBOW )
              {
                // КОД СТРОГО ДЛЯ РАДУГИ
                if ( ( u8_LED3_State  == LED_3_NoSync) || ( u8_LED3_State == LED_3_DEFAULT) )
                {
                  if (u16_LightLevel==(UV_OFFSET-1))  //  == 0    u32_LightLevelRed
                   u32_Mode_Rnbw_SM = SM_MODE_WHITE_UP;
                }
                //
                // Работа с синхронизацией
                if ( ( u8_LED3_State  == LED_3_SentSync) || ( u8_LED3_State == LED_3_RecieveSync) )
                {
                  if (u16_LightLevel==(UV_OFFSET-1))  
                  {
                    u32_Mode_Rnbw_SM = SM_MODE_WHITE_UP;    
                    u8_Mode_Rnbw_Status = MODE_STATUS_STOP;
                  }
                }
              }           
          } 
    }
    //
    //
    if ( b_UpdateUvLightLevel)
    {
      b_UpdateUvLightLevel  = false;
      //
      u32_Period = u16_ActualUvPeriod;
      u32_Freq   = u16_ActualUvFreq  ;
      //
      LL_TIM_OC_SetCompareCH1(TIM10, u32_Period );
      LL_TIM_SetAutoReload   (TIM10, u32_Freq   );
      //
      b_UvLightLevelUpdated = true;
    }
   
  }
  // 
   Timer10CaptureCompare_Callback();
}
//
//
void ADC_IRQHandler(void)
{
  /* Check whether ADC group regular overrun caused the ADC interruption */
  if(LL_ADC_IsActiveFlag_OVR(ADC1) != 0)
  {
    /* Clear flag ADC group regular overrun */
    LL_ADC_ClearFlag_OVR(ADC1);
    
    /* Call interruption treatment function */
    AdcGrpRegularOverrunError_Callback();
  }
  /* Check whether ADC group regular end of sequence conversions caused       */
  /* the ADC interruption.                                                    */
  /* Note: On this STM32 serie, ADC group regular end of conversion           */
  /*       must be selected among end of unitary conversion                   */
  /*       or end of sequence conversions.                                    */
  /*       Refer to function "LL_ADC_REG_SetFlagEndOfConversion()".           */
  else /* if(LL_ADC_IsActiveFlag_EOCS(ADC1) != 0) */
  {
    /* Clear flag ADC group regular end of sequence conversions */
    LL_ADC_ClearFlag_EOCS(ADC1);
    
    /* Call interruption treatment function */
    AdcGrpRegularSequenceConvComplete_Callback();
  }
}

/**
  * @brief  This function handles DMA2 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* Check whether DMA transfer complete caused the DMA interruption */
  if(LL_DMA_IsActiveFlag_TC0(DMA2) == 1)
  {
    /*  Clear Stream  transfer complete flag*/
    LL_DMA_ClearFlag_TC0(DMA2);
    /* Call interruption treatment function */
    AdcDmaTransferComplete_Callback();
  }
  
  /* Check whether DMA transfer error caused the DMA interruption */
  if(LL_DMA_IsActiveFlag_TE0(DMA2) == 1)
  {
    /* Clear flag DMA transfer error */
    LL_DMA_ClearFlag_TE0(DMA2);
    
    /* Call interruption treatment function */
    AdcDmaTransferError_Callback();
  }
}




// ==================== I2C_Начало кода ============================================== //

extern __IO uint8_t  ubMasterRequestDirection;
extern __IO uint8_t  ubMasterXferDirection;
extern __IO uint8_t  ubMasterNbDataToReceive;
// РУДИМЕНТ СЛЕЙВА.extern __IO uint8_t  ubSlaveNbDataToTransmit;

extern uint8_t u8_SlaveAddress ;
//
extern uint8_t u8_TxPacketDataLength ;
extern uint8_t u8_RxPacketDataLength ;


/**
  * Brief   This function handles I2C3 (Master) interrupt request.
  * Param   None
  * Retval  None
  */
void I2C3_EV_IRQHandler(void)
{
  /* Check SB flag value in ISR register */
  if(LL_I2C_IsActiveFlag_SB(I2C3))
  {
     /* Send Slave address with a 7-Bit SLAVE_OWN_ADDRESS for a ubMasterRequestDirection request */
  // LL_I2C_TransmitData8(I2C3, SLAVE_OWN_ADDRESS | ubMasterRequestDirection);
     LL_I2C_TransmitData8(I2C3, (u8_SlaveAddress<<1) | ubMasterRequestDirection);
     
  }
  /* Check ADDR flag value in ISR register */
  else if(LL_I2C_IsActiveFlag_ADDR(I2C3))
  {
    /* Verify the transfer direction */
    if(LL_I2C_GetTransferDirection(I2C3) == LL_I2C_DIRECTION_READ)
    {
      ubMasterXferDirection = LL_I2C_DIRECTION_READ;

      if(ubMasterNbDataToReceive == 1)
      {
        /* Prepare the generation of a Non ACKnowledge condition after next received byte */
        LL_I2C_AcknowledgeNextData(I2C3, LL_I2C_NACK);

        /* Enable DMA transmission requests */
        LL_I2C_EnableDMAReq_RX(I2C3);
      }
      else if(ubMasterNbDataToReceive == 2)
      {
        /* Prepare the generation of a Non ACKnowledge condition after next received byte */
        LL_I2C_AcknowledgeNextData(I2C3, LL_I2C_NACK);

        /* Enable Pos */
        LL_I2C_EnableBitPOS(I2C3);
      }
      else
      {
        /* Enable Last DMA bit */
        LL_I2C_EnableLastDMA(I2C3);

        /* Enable DMA transmission requests */
        LL_I2C_EnableDMAReq_RX(I2C3);
      }
    }
    else
    {
      ubMasterXferDirection = LL_I2C_DIRECTION_WRITE;

      /* Enable DMA transmission requests */
      LL_I2C_EnableDMAReq_TX(I2C3);
    }

    /* Clear ADDR flag value in ISR register */
    LL_I2C_ClearFlag_ADDR(I2C3);
  }
}

/**
  * Brief   This function handles I2C3 (Master) error interrupt request.
  * Param   None
  * Retval  None
  */
void I2C3_ER_IRQHandler(void)
{
  /* Call Error function */
  Error_Callback();
}

/**
  * @brief  This function handles DMA1_Stream5 interrupt request.
  * @param  None
  * @retval None
  */
// Что делают ?
//  Master Transmit cn6
void DMA1_Stream5_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC5(DMA1))
  {
    LL_DMA_ClearFlag_TC5(DMA1);

    Transfer_Complete_Callback();
  }
  else if(LL_DMA_IsActiveFlag_TE5(DMA1))
  {
    Transfer_Error_Callback();
  }
}

/**
  * @brief  This function handles DMA1_Stream2 interrupt request.
  * @param  None
  * @retval None
  */
// Master Receive cn3
void DMA1_Stream2_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC2(DMA1))
  {
    LL_DMA_ClearFlag_TC2(DMA1);
    Transfer_Complete_Callback();
  }
  else if(LL_DMA_IsActiveFlag_TE2(DMA1))
  {
    Transfer_Error_Callback();
  }
}
// ==================== I2C Конец  кода ============================================== //




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
