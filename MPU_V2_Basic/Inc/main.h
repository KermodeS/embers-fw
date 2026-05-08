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
#define ENABLE_I2C_IRDA 0

/* =========================================================================
 * BUILD CONFIGURATION FLAGS
 * Set DISABLE_I2C_MASTER to skip STM8 co-processor I2C init.
 * Enable when STM8 is absent or unresponsive (address 0x50).
 * ========================================================================= */
#define DISABLE_I2C_MASTER

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
// For ADC
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_dma.h"
//
// For watchdog reset / reboot
#include "stm32f4xx_ll_wwdg.h"
#include "stm32f4xx_ll_iwdg.h"    // Independent watchdog (IWDG)
//
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

// This firmware is for LLU_V2_MPU 
#define UNDEF_DEVICE      0 
#define LOW_POWER_DEVICE  1
#define MID_POWER_DEVICE  2
#define HIGH_POWER_DEVICE 3
//
//
#undef LPU_V2_DEVICE
#undef HPU_V2_DEVICE
//
#define MPU_V2_DEVICE
//
//
#define RED_OFFSET 20
#define GRN_OFFSET 20
#define BLU_OFFSET 20
#define UV_OFFSET  20
#define WHT_OFFSET 20
//
#define   RED_LEVEL_MAX_MP (2100 + RED_OFFSET )
#define GREEN_LEVEL_MAX_MP (2900 + GRN_OFFSET )
#define  BLUE_LEVEL_MAX_MP (2900 + BLU_OFFSET )
#define    UV_LEVEL_MAX_MP (2800 +  UV_OFFSET )
#define WHITE_LEVEL_MAX_MP (2700 + WHT_OFFSET )
//
// Percentage step for manual control
#define PRCNT_SHIFT  10
//
//
void UpdateRedLevelFast_MP (uint16_t u16_LightIndex);
void UpdateGreenLevelFast_MP (uint16_t u16_LightIndex);
void UpdateWhiteLevelFast_MP (uint16_t u16_LightIndex);
void UpdateUvLevelFast_MP (uint16_t u16_LightIndex);
void UpdateBlueLevelFast_MP (uint16_t u16_LightIndex);
//
// ---------------------------------------------------- // 
//void UserButton_Callback(void); // MOVE TO ADC.h
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
// ==================== I2C  BEGIN ============================================================ //

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
// ==================== I2C  END ============================================================== //


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
