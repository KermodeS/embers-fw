
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LLU_V2_KBD_H
#define __LLU_V2_KBD_H

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


//
void InitControlButtons    ( void );
void ProcessControlButtons ( void );
//
__STATIC_INLINE void ProcessButton_Mode  ( void );
//
__STATIC_INLINE void ProcessButton_Up    ( void );
__STATIC_INLINE void ProcessButton_Down  ( void );
//
__STATIC_INLINE void ProcessButton_Next  ( void );
__STATIC_INLINE void ProcessButton_Prev  ( void );
//
//


//
void InitButton_MODE(void);
//
void InitButton_UP(void);
void InitButton_DOWN(void);
//
void InitButton_PREV(void);
void InitButton_NEXT(void);
//
void Implement_KBD(void);
//
//
#endif /* __LLU_V2_KBD_H */