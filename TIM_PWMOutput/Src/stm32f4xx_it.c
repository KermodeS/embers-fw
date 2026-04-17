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

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
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
extern bool      b_UpdateRedLightLevel  ;
extern bool      b_RedLightLevelUpdated ;
//
extern uint16_t  u16_ActualRedPeriod    ;
extern uint16_t  u16_ActualRedFreq      ;
//
//

extern bool      b_UpdateGreenLightLevel  ;
extern bool      b_GreenLightLevelUpdated ;
//
extern uint16_t  u16_ActualGreenPeriod    ;
extern uint16_t  u16_ActualGreenFreq      ;
//
//
//
extern bool      b_UpdateBlueLightLevel  ;
extern bool      b_BlueLightLevelUpdated ;
//
extern uint16_t  u16_ActualBluePeriod    ;
extern uint16_t  u16_ActualBlueFreq      ;
//
//
//
extern bool      b_UpdateUvLightLevel  ;
extern bool      b_UvLightLevelUpdated ;
//
extern uint16_t  u16_ActualUvPeriod    ;
extern uint16_t  u16_ActualUvFreq      ;
//
//
//
extern bool      b_UpdateWhiteLightLevel  ;
extern bool      b_WhiteLightLevelUpdated ;
//
extern uint16_t  u16_ActualWhitePeriod    ;
extern uint16_t  u16_ActualWhiteFreq      ;
//
//

//
//
void TIM3_IRQHandler(void)
{
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  if(LL_TIM_IsActiveFlag_CC3(TIM3) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC3(TIM3);
    //
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

//
//
void TIM4_IRQHandler(void)
{
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  if(LL_TIM_IsActiveFlag_CC2(TIM4) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM4);
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


/*
  //
  // Check whether CC1 interrupt is pending /
  if(LL_TIM_IsActiveFlag_CC4(TIM3) == 1)
  {
    // Clear the update interrupt flag
    LL_TIM_ClearFlag_CC4(TIM3);
   
    // My
    static uint32_t u32_Period   = 0 ;
    static uint32_t   b_CountDir = 1;

    
  LL_TIM_OC_SetCompareCH4(TIM3, u32_Period);
    if (b_CountDir)
    {
      u32_Period++; if(u32_Period>=8000 ) b_CountDir=0;
    }
    else
    {
      u32_Period--;  if(u32_Period<=   0 ) b_CountDir=1;
    }

  }
  //

*/



    /*   
    // My
    static uint32_t u32_Period   = 0 ;
    static uint32_t   b_CountDir = 1;
    //
    LL_TIM_OC_SetCompareCH3(TIM3, u32_Period);
    //
    
    //

    if (b_CountDir)
    {
      u32_Period++;
      u16_MyCounter++;
      //
      if(u32_Period>=40000 )
      {
        u16_Stage =0;
        b_CountDir=0;
      }
     
      if ( (u32_Period == 250) && (u16_Stage == 0) )
      {
        u16_Stage = 1;
        u32_Period = 49;
        
        LL_TIM_SetAutoReload(TIM3, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 4000)); 
      } 
      if ( (u32_Period == 450) && (u16_Stage == 1) )
      {
        u16_Stage = 2;
        u32_Period = 121;
      //  LL_TIM_SetAutoReload(TIM3, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 6000)); 
        LL_TIM_SetAutoReload(TIM3, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 16000)); 
      } 
    }
    else
    {
      u32_Period--;  
      if(u32_Period<=   0 ) 
      { 
        u16_MyCounter = 0;
        TimOutClock = SystemCoreClock/1;
        LL_TIM_SetAutoReload(TIM3, 65535 ); //__LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM3), 100)
        b_CountDir=1;
      }
      
    }
    // TIM2 capture/compare interrupt processing(function defined in main.c) //
    //TimerCaptureCompare_Callback();
  }
  */



/**
  * @brief  This function handles external line 13 interrupt request.
  * @param  None
  * @retval None
  */
void USER_BUTTON_IRQHANDLER(void)
{
  /* Manage Flags */
  if(LL_EXTI_IsActiveFlag_0_31(USER_BUTTON_EXTI_LINE) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(USER_BUTTON_EXTI_LINE);

    /* User button interrupt processing(function defined in main.c) */
    UserButton_Callback(); 
  }
}
//
//
void TIM2_IRQHandler(void)
{
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  if(LL_TIM_IsActiveFlag_CC2(TIM2) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM2);
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
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  if(LL_TIM_IsActiveFlag_CC1(TIM11) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC1(TIM11);
    //
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
   
  }
  
  if(LL_TIM_IsActiveFlag_CC2(TIM1) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM1);
    //
   
   
  }
  
  // 
   Timer1CaptureCompare_Callback();
 // Timer11CaptureCompare_Callback();
  
}
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
extern uint8_t u8_RedLightDelayDutyCounter ;  // int16_t i16 
extern uint8_t u8_GreenLightDelayDutyCounter ; 
extern uint8_t u8_BlueLightDelayDutyCounter  ; 
extern uint8_t u8_UvLightDelayDutyCounter    ; 
extern uint8_t u8_WhiteLightDelayDutyCounter ; 
//
extern uint8_t u8_ProcessWiFiCommand ;
extern bool     b_Mode_A_CallBack ;
//

// this is a universal 76 kHz (actually - somewhere around 78 kHz) timer for processing IR and serial port
// It is this option that allows you to clearly catch fronties and their duration
// and process the last port in parallel.
void TIM1_CC_IRQHandler(void)
{
  //
 // static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
 // static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  if(LL_TIM_IsActiveFlag_CC2(TIM1) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC2(TIM1);
    //
    //  LL_TIM_OC_SetCompareCH1(TIM1, 800 );
    //  LL_TIM_SetAutoReload   (TIM1, 2000   );
    //
    LL_GPIO_TogglePin(IRDA_CAR_GPIO_PORT, IRDA_CAR_PIN); 
    //
    //
    u32_IrdaRX_ActualState = LL_GPIO_IsInputPinSet(IRDA_RX_GPIO_PORT, IRDA_RX_PIN);
    if ( u32_IrdaRX_ActualState == u32_IrdaRX_PrevState)
    {
       u16_captureActual++;             // Increased duration for comparison
       if (u16_captureActual> 5000)     // control duration for comparison
       {
              u16_captureActual =  5000;
         if (u16_IR_RX_captureCounter)
         { // If there is data after the pause, then process it
          
          if ( b_IR_RX_PacketProcessed == true)
          {
            b_IR_RX_PacketProcessed = true;
            ProcessIRDA_Rx(); // Determine what we got. We influence the state machine.
            u16_IR_RX_captureCounter     =     0 ; 
            b_IR_RX_PacketProcessed =  true ;
          }   
         }
       }
    }
    else
    { // Словили фронт (рост или падение)
      u32_IrdaRX_PrevState = u32_IrdaRX_ActualState;
      u16_IR_RX_capture[u16_IR_RX_captureCounter] = u16_captureActual; // Записали его длительность
      u16_captureActual = 0; // Обнулили длительность для след сравнения
      u16_IR_RX_captureCounter++; // Увеличили индекс записи
      if ( u16_IR_RX_captureCounter >= 1024 )  // Проконтролировали индекс записа
        u16_IR_RX_captureCounter = 0;
    } 
    //
    ProcessWiFiSerial();
    //  
    ProcessIRDA_Tx();   
    //
    if (   u8_RedLightDelayDutyCounter   <  250 )   u8_RedLightDelayDutyCounter   ++; // 7600
    if (   u8_GreenLightDelayDutyCounter <  250 )   u8_GreenLightDelayDutyCounter  ++; // 7600
    if (   u8_BlueLightDelayDutyCounter  <  250 )   u8_BlueLightDelayDutyCounter   ++; // 7600
    if (   u8_UvLightDelayDutyCounter    <  250 )   u8_UvLightDelayDutyCounter     ++; // 7600 
    if (   u8_WhiteLightDelayDutyCounter <  250 )   u8_WhiteLightDelayDutyCounter  ++; // 7600  
    //
    if ( i32_GlobalDutyCounter < (2147483648-2) ) i32_GlobalDutyCounter   ++;  
    //
    // автозапуск режима A
    
   // #ifdef LOW_POWER_DEVICE 
    
    if ( b_Mode_A_CallBack == true )
    {
      b_Mode_A_CallBack = false;
      u8_ProcessWiFiCommand = WIFI_COMMAND_SET_AUTO_PLAY;
    }
   // #endif
    
   // #ifdef MID_POWER_DEVICE    
    if ( b_Mode_A_CallBack == true )
    {
      b_Mode_A_CallBack = false;
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
  //
  static uint32_t u32_Period =     1; // u16_ActualGreenPeriod;
  static uint32_t u32_Freq   = 65535; // u16_ActualGreenFreq  ;
  //
  if(LL_TIM_IsActiveFlag_CC1(TIM10) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC1(TIM10);
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


/*

  // Check whether CC1 interrupt is pending 
  if(LL_TIM_IsActiveFlag_CC2(TIM2) == 1)
  {
    // Clear the update interrupt flag 
    LL_TIM_ClearFlag_CC2(TIM2);
   
    // My
    static uint32_t u32_Period   = 0 ;
    static uint32_t   b_CountDir = 1;

    LL_TIM_OC_SetCompareCH2(TIM2, u32_Period);
    if (b_CountDir)
    {
      u32_Period++; if(u32_Period>=40000 ) b_CountDir=0;
    }
    else
    {
      u32_Period--;  if(u32_Period<=   0 ) b_CountDir=1;
    }
    // TIM2 capture/compare interrupt processing(function defined in main.c) //
  //  TimerCaptureCompare_Callback();
  }
  //
  if(LL_TIM_IsActiveFlag_CC1(TIM2) == 1)
  {
    // Clear the update interrupt flag//
    LL_TIM_ClearFlag_CC1(TIM2);
   
    // My
    static uint32_t u32_Period   = 0 ;
    static uint32_t   b_CountDir = 1;

    
    LL_TIM_OC_SetCompareCH1(TIM2, u32_Period);
    if (b_CountDir)
    {
      u32_Period++; if(u32_Period>=40000 ) b_CountDir=0;
    }
    else
    {
      u32_Period--;  if(u32_Period<=   0 ) b_CountDir=1;
    }
    
    // TIM2 capture/compare interrupt processing(function defined in main.c) 
   TimerCaptureCompare_Callback();
  }
  //  TimerCaptureCompare_Callback();


*/


/*

*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
