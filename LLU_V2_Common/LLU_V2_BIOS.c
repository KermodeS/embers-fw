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
#include "LLU_V2_BIOS.h"
#include "LLU_V2_KBD.h"
#include "LLU_V2_ESP32.h"

#include "LLU_V2_LightControl.h"

#include "stdbool.h"   
#include "string.h" 
#include <stdio.h>
#include <stdlib.h>





uint32_t u32_BIOS[128]=
{
  //
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  //
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  //
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  //
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 0x42696F73, 
  //
};
//
// Структура биоса
// Для надежности хранение - каждый элемент дублируется инвертированной копией. 
// Массив хранения содержит элементы с шагов кратным 2, то есть 0, 2 4 и так далее до 63
//
// БЛОК аппаратных характеристик - 8 элементов
// Тип устройства: не определен, LPU_V2, MPU_V2, HPU_V2.
// Версия аппаратной части чч мм гг час. 
// Версия прошивки         чч мм гг час.  
// Версия биоса            чч мм гг час. 
// Резерв 4 - 8
// 
// Блок системных настроек - 8 элементов.
// Режим радиомодуля - не опр, мастер, эхослейв, слейв.
// Резерв 2 - 8.
//
// Блок сетевых настроек. (4+4)*4= 32 элемента длиной 4 байта.
//  Мастер/слейв режим. ВАР 1 
// SSID - 16 символов. 4 элемента.
// pass - 16 символов. 4 элемента. 
// Блок сетевых настроек.  Мастер/слейв режим. ВАР 2
// SSID - 16 символов. 4 элемента.
// pass - 16 символов. 4 элемента.
// Блок сетевых настроек.  Мастер/слейв режим. ВАР 3
// SSID - 16 символов. 4 элемента.
// pass - 16 символов. 4 элемента. 
// Блок сетевых настроек.  Мастер/слейв режим. ВАР 4
// SSID - 16 символов. 4 элемента.
// pass - 16 символов. 4 элемента. 
//
// Блок резервных настроек - 8 элементов
// резерв 1-8.
//
//  Аппаратные настройки
#define _1_DEVICE_TYPE_POSITION (  0 )
#define _1_HW_VER_POSITION      (  2 )
#define _1_MCU_FW_VER_POSITION  (  4 )
#define _1_ESP_FW_VER_POSITION  (  6 )
#define _1_BIOS_VER_POSITION    (  8 )
#define _1_RESERVE_6_POSITION   ( 10 )
#define _1_RESERVE_7_POSITION   ( 12 )
#define _1_RESERVE_8_POSITION   ( 14 )
//
// Системные настройки
#define _2_NET_ROLE_POSITION    ( 16 +  0 )
#define _2_RESERVE_2_POSITION   ( 16 +  2 )
#define _2_RESERVE_3_POSITION   ( 16 +  4 )
#define _2_RESERVE_4_POSITION   ( 16 +  6 )
#define _2_RESERVE_5_POSITION   ( 16 +  8 )
#define _2_RESERVE_6_POSITION   ( 16 + 10 )
#define _2_RESERVE_7_POSITION   ( 16 + 12 )
#define _2_RESERVE_8_POSITION   ( 16 + 14 )
//
// Блок сетевых настроек
#define _3_SSID_A_POSITION      ( 16 + 16 +  0 )
#define _3_PASS_A_POSITION      ( 16 + 16 +  8 )
//
#define _3_SSID_B_POSITION      ( 16 + 16 + 16 )
#define _3_PASS_B_POSITION      ( 16 + 16 + 24 )
//
#define _3_SSID_C_POSITION      ( 16 + 16 + 32 )
#define _3_PASS_C_POSITION      ( 16 + 16 + 40 )
//
#define _3_SSID_D_POSITION      ( 16 + 16 + 48 )
#define _3_PASS_D_POSITION      ( 16 + 16 + 56 )
//
//
#define _4_RESERVE_1_POSITION   ( 16 + 16 + 64 +  0 )
#define _4_RESERVE_2_POSITION   ( 16 + 16 + 64 +  2 )
#define _4_RESERVE_3_POSITION   ( 16 + 16 + 64 +  4 )
#define _4_RESERVE_4_POSITION   ( 16 + 16 + 64 +  6 )
#define _4_RESERVE_5_POSITION   ( 16 + 16 + 64 +  8 )
#define _4_RESERVE_6_POSITION   ( 16 + 16 + 64 + 10 )
#define _4_RESERVE_7_POSITION   ( 16 + 16 + 64 + 12 )
#define _4_RESERVE_8_POSITION   ( 16 + 16 + 64 + 14 )
//
//
#define _5_RESERVE_1_POSITION   ( 16 + 16 + 64 + 16 +  0 )
#define _5_RESERVE_2_POSITION   ( 16 + 16 + 64 + 16 +  2 )
#define _5_RESERVE_3_POSITION   ( 16 + 16 + 64 + 16 +  4 )
#define _5_RESERVE_4_POSITION   ( 16 + 16 + 64 + 16 +  6 )
#define _5_RESERVE_5_POSITION   ( 16 + 16 + 64 + 16 +  8 )
#define _5_RESERVE_6_POSITION   ( 16 + 16 + 64 + 16 + 10 )
#define _5_RESERVE_7_POSITION   ( 16 + 16 + 64 + 16 + 12 )
#define _5_CRC32_POSITION       ( 16 + 16 + 64 + 16 + 14 )
//
//
// 
//
uint8_t *p_ArrayAdr;
uint32_t *p_u32_ArrayAdr;
//
//
uint32_t   u32_Adr ; // 0x08000B3C4
uint32_t   u32_Data; 
//
static void BIOS_Error_Handler(void);
static void BIOS_Error_Handler(void)
{

}

#define BIOS_START_ADDR   0x0803F000
//ADDR_FLASH_SECTOR_5
//ADDR_FLASH_SECTOR_2   /* Start @ of user Flash area */
#define BIOS_END_ADDR    0x0803F3FF
//ADDR_FLASH_SECTOR_5  +  GetSectorSize(ADDR_FLASH_SECTOR_5) -1 /* End @ of user Flash area : sector start address + sector size -1 */

#define DATA_32          ((uint32_t)0x42696F73)      
//((uint32_t)0x12345678)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
uint32_t SectorError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0, u32_BIOS_Counter = 0;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
   
/* Private function prototypes -----------------------------------------------*/
//static
void SystemClock_Config(void);
// static void Error_Handler(void);
static uint32_t GetSector(uint32_t Address);
//static
 uint32_t GetSectorSize(uint32_t Sector);

uint32_t u32_ActualNetRole = ESP32_NET_ROLE_DEFAULT  ;

static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
   else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_5))*/
  {
    sector = FLASH_SECTOR_5;
  }

  return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
//static
uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}

//

//
void RetriveBiosData(void)
{
                                                         // 0x08000B3C4
                                 p_u32_ArrayAdr = (uint32_t*)BIOS_START_ADDR;
  u32_ActualNetRole =(uint32_t)  p_u32_ArrayAdr[_1_DEVICE_TYPE_POSITION] ;
  //
}
//
//  
void ReadBios (void)
{
  u32_BIOS_Counter = 0;
  // 
  u32_Adr = BIOS_START_ADDR; // 0x08000B3C4
  p_u32_ArrayAdr = (uint32_t*)u32_Adr;
  u32_Data =(uint32_t)  p_u32_ArrayAdr[u32_BIOS_Counter] ;
  //
  while (u32_BIOS_Counter < 128)
  {
    //data32 = *(__IO uint32_t*)Address;
    u32_Data =(uint32_t)  p_u32_ArrayAdr[u32_BIOS_Counter] ;    
    //
    u32_BIOS [ u32_BIOS_Counter ] = u32_Data;
    //
    u32_BIOS_Counter ++; //Address = Address + 4;
    //
  } 
  //
  return;
}
//
//
//
void WrightBios (void)
{
    
  /* Unlock the Flash to enable the flash control register access *************/ 
  HAL_FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by BIOS_START_ADDR and BIOS_END_ADDR) ***********/

  /* Get the 1st sector to erase */
  FirstSector = GetSector(BIOS_START_ADDR);
  /* Get the number of sector to erase from 1st sector*/
  NbOfSectors = GetSector(BIOS_END_ADDR) - FirstSector + 1;

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  { 
    /* 
      Error occurred while sector erase. 
      User can add here some code to deal with this error. 
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
    */
    /*
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
    */
    BIOS_Error_Handler();
  }

  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
     you have to make sure that these data are rewritten before they are accessed during code
     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
     DCRST and ICRST bits in the FLASH_CR register. */
  __HAL_FLASH_DATA_CACHE_DISABLE();
  __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

  __HAL_FLASH_DATA_CACHE_RESET();
  __HAL_FLASH_INSTRUCTION_CACHE_RESET();

  __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
  __HAL_FLASH_DATA_CACHE_ENABLE();

  /* Program the user Flash area word by word
    (area defined by BIOS_START_ADDR and BIOS_END_ADDR) ***********/

  Address = BIOS_START_ADDR;
  u32_BIOS_Counter = 0;

  while (Address < BIOS_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, u32_BIOS[u32_BIOS_Counter++]) == HAL_OK) // DATA_32
    {
      Address = Address + 4;
    }
    else
    { 
      /* Error occurred while writing data in Flash memory. 
         User can add here some code to deal with this error */
      /*
        FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
      */
      BIOS_Error_Handler();
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock(); 

  /* Check if the programmed data is OK 
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = BIOS_START_ADDR;
  MemoryProgramStatus = 0x0;
  
  while (Address < BIOS_END_ADDR)
  {
    data32 = *(__IO uint32_t*)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;  
    }

    Address = Address + 4;
  }  

  /*Check if there is an issue to program data*/
  if (MemoryProgramStatus == 0)
  {
    /* No error detected. Switch on LED4*/
   //;; BSP_LED_On(LED4);
  }
  else
  {
    /* Error detected. Switch on LED5*/
    BIOS_Error_Handler();
  }
  
}
//
#define _INVERT ^0xFFFFFFFF

bool TestBios (void)
{
  ReadBios();
  
  if ( u32_BIOS[ _1_DEVICE_TYPE_POSITION     ] !=
       (u32_BIOS[ _1_DEVICE_TYPE_POSITION + 1 ]_INVERT) ) 
    return false;
  //
  return true;
}
//
//
void RestoreBios(void)
{
  // реинициируем массив значением по умолчанию
  //
  u32_BIOS[ _1_DEVICE_TYPE_POSITION     ] = ESP32_NET_ROLE_DEFAULT ;
  u32_BIOS[ _1_DEVICE_TYPE_POSITION + 1 ] = ESP32_NET_ROLE_DEFAULT _INVERT ;
  //
  WrightBios();
  //
}

void SaveBios(void)
{
   //
  u32_BIOS[ _1_DEVICE_TYPE_POSITION     ] = u32_ActualNetRole ;
  u32_BIOS[ _1_DEVICE_TYPE_POSITION + 1 ] = u32_ActualNetRole _INVERT ;
  //
  WrightBios();
  // 
}

void SysReboot(void)
{
  // void Check_WWDG_Reset(void)
  if (LL_RCC_IsActiveFlag_WWDGRST())
  {
    /* clear WWDG reset flag */
    LL_RCC_ClearResetFlags();
  }
  //
  // void Configure_WWDG(void)
  /* Enable the peripheral clock of DBG register (uncomment for debug purpose) */
  /*LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_WWDG_STOP); */
  
  /* Enable the peripheral clock WWDG */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);

  /* Configure WWDG */
  /* (1) set prescaler to have a rollover each about ~2s */
  /* (2) set window value to same value (~2s) as downcounter in order to ba able to refresh the WWDG almost immediately */
  /* (3) Refresh WWDG before activate it */
  /* (4) Activate WWDG */
  LL_WWDG_SetPrescaler(WWDG, LL_WWDG_PRESCALER_8); /* (1) */
  LL_WWDG_SetWindow (WWDG, 0xE);     // 0x7E                /* (2) */
  LL_WWDG_SetCounter(WWDG, 0XE);   // 0X7E               /* (3) */
  LL_WWDG_Enable(WWDG);                            /* (4) */
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
