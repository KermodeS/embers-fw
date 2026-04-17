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
// для АЦП
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_dma.h"
//
// Для ребута
#include "stm32f4xx_ll_wwdg.h"
//
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

// Это  прошивка для LLU_V2_HPU 
#define UNDEF_DEVICE      0 
#define LOW_POWER_DEVICE  1
#define MID_POWER_DEVICE  2
#define HIGH_POWER_DEVICE 3
//
//
#undef LPU_V2_DEVICE
#undef MPU_V2_DEVICE
//
#define  HPU_V2_DEVICE  
// 
//
// Тут смысл такой.
// В первом комплекте есть 3 пары приборов - два LPU, два MPU, дла HPU.
// MPU c внешней черной антенной назначен мастером. У него нормально работает 
// ИК приемник. второй МПУ настроен как эхослейв.
// В смежных проектах для ЛПУ и ХПУ будет использоватся _Oleg B _Sean
//
#define HPU_V2_SEAN_UNIT 1
//  #define HPU_V2_OLEG_UNIT 2
//
//
#ifdef HPU_V2_SEAN_UNIT
  #define RED_OFFSET 10
  #define GRN_OFFSET 10
  #define BLU_OFFSET 10
  #define UV_OFFSET  20
  #define WHT_OFFSET 10
#endif
//
#ifdef HPU_V2_OLEG_UNIT
  #define RED_OFFSET 10
  #define GRN_OFFSET 10
  #define BLU_OFFSET 10
  #define UV_OFFSET  20
  #define WHT_OFFSET 10
#endif
//
//
#ifdef HPU_V2_SEAN_UNIT
  #define   RED_LEVEL_MAX_HP (1100 + RED_OFFSET )
  #define GREEN_LEVEL_MAX_HP (1500 + GRN_OFFSET )
  #define  BLUE_LEVEL_MAX_HP (1800 + BLU_OFFSET )
  #define    UV_LEVEL_MAX_HP (2600 +  UV_OFFSET )
  #define WHITE_LEVEL_MAX_HP (2100 + WHT_OFFSET )
#endif
//
//
#ifdef HPU_V2_OLEG_UNIT
  #define   RED_LEVEL_MAX_HP (1100 + RED_OFFSET )
  #define GREEN_LEVEL_MAX_HP (1500 + GRN_OFFSET )
  #define  BLUE_LEVEL_MAX_HP (1800 + BLU_OFFSET )
  #define    UV_LEVEL_MAX_HP (2600 +  UV_OFFSET )
  #define WHITE_LEVEL_MAX_HP (2100 + WHT_OFFSET )
#endif
//
//
// Процентный сдвиг при ручном управлении
#define PRCNT_SHIFT  10
//
//
void UpdateRedLevelFast_HP (uint16_t u16_LightIndex);
void UpdateGreenLevelFast_HP (uint16_t u16_LightIndex);
void UpdateWhiteLevelFast_HP (uint16_t u16_LightIndex);
void UpdateUvLevelFast_HP (uint16_t u16_LightIndex);
void UpdateBlueLevelFast_HP (uint16_t u16_LightIndex);
//
// ---------------------------------------------------- // 
//void UserButton_Callback(void); // ПЕРЕНЕСТИ В ADC.h
void AdcDmaTransferComplete_Callback(void);
void AdcDmaTransferError_Callback(void);
void AdcGrpRegularSequenceConvComplete_Callback(void);
void AdcGrpRegularOverrunError_Callback(void);
void GetADC_Data(void);
//
#define INTERNAL_TEMPSENSOR_AVGSLOPE   ((int32_t) 2500)        /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_V25        ((int32_t)  760)        /* Internal temperature sensor, parameter V25 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_V25_TEMP   ((int32_t)   25)
#define INTERNAL_TEMPSENSOR_V25_VREF   ((int32_t) 3300)
//
// ---------------------------------------------------- //
//
//
// ==================== I2C_Начало кода ============================================== //

#include "stdio.h"
#include "string.h"
/////////  #include "stm32f4xx_ll_bus.h"

/////////  #include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
/////////  #include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_cortex.h"
/////////  #include "stm32f4xx_ll_gpio.h"
/////////  #include "stm32f4xx_ll_exti.h"
/////////  #include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_pwr.h"

//    
#define  KBD_ADDRESS                      ( 0x20 ) 
#define  LED_ADDRESS                      ( 0x20 ) 
//     
#define IRDA_ADDRESS                      ( 0x50 ) 
//    
//                                          0x5A 
                                                      /* This value is a left shift of a real 7 bits of a slave address
                                                        value which can find in a Datasheet as example: b0101101
                                                        mean in uint8_t equivalent at 0x2D and this value can be
                                                        seen in the OAR1 register in bits ADD[1:7] */

/**
  * @brief Master Transfer Request Direction
  */
#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01
//
void Transfer_Complete_Callback(void);
void Transfer_Error_Callback(void);
void Error_Callback(void);
// ==================== I2C Конец  кода ============================================== //


//
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
