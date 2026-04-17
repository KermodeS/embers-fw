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
#include "LLU_V2_KBD.h"
#include "LLU_V2_ESP32.h"

#include "LLU_V2_LightControl.h"

#include "stdbool.h"   
#include "string.h" 
#include <stdio.h>
#include <stdlib.h>



  
//
void     InitButton_MODE(void)
{
  MODE_BUTTON_GPIO_CLK_ENABLE();
  
  LL_GPIO_SetPinMode(MODE_BUTTON_GPIO_PORT, MODE_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(MODE_BUTTON_GPIO_PORT, MODE_BUTTON_PIN, LL_GPIO_PULL_UP);
}
//
//
void     InitButton_UP(void)
{
  UP_BUTTON_GPIO_CLK_ENABLE();
  
  LL_GPIO_SetPinMode( UP_BUTTON_GPIO_PORT, UP_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull( UP_BUTTON_GPIO_PORT, UP_BUTTON_PIN, LL_GPIO_PULL_UP);
}
//
void     InitButton_DOWN(void)
{
  DOWN_BUTTON_GPIO_CLK_ENABLE();
  
  LL_GPIO_SetPinMode(DOWN_BUTTON_GPIO_PORT, DOWN_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(DOWN_BUTTON_GPIO_PORT, DOWN_BUTTON_PIN, LL_GPIO_PULL_UP);
}
//
//
void     InitButton_PREV(void)
{
  PREV_BUTTON_GPIO_CLK_ENABLE();
  
  LL_GPIO_SetPinMode(PREV_BUTTON_GPIO_PORT, PREV_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(PREV_BUTTON_GPIO_PORT, PREV_BUTTON_PIN, LL_GPIO_PULL_UP);
}
//
void     InitButton_NEXT(void)
{
  NEXT_BUTTON_GPIO_CLK_ENABLE();
  
  LL_GPIO_SetPinMode(NEXT_BUTTON_GPIO_PORT, NEXT_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(NEXT_BUTTON_GPIO_PORT, NEXT_BUTTON_PIN, LL_GPIO_PULL_UP);
}
//


void InitControlButtons(void)
{
  //
  InitButton_MODE();
  //
  InitButton_UP();
  InitButton_DOWN();
  //
  InitButton_PREV();
  InitButton_NEXT();
}
//
    // Надо вызывать с дискретностью  100 раз в секунду и обрабатывать антидребезг
    // Зарезервировано состояний 
    // #define LED_7_DEFAULT      0
    // #define LED_7_KBD_PRESSED  1
    // #define LED_7_KBD_CLICK    2
    // #define LED_7_KBD_DBL      3
    // #define LED_7_KBD_UP       4
    // Двойное нажатие - настраваемое с дисктретностью 0,1-0,3 секунды
  // 
  // Машина состояний кнопки
  // Idle - начальное. Ждем нажатия длительностью более 100мс.
  // обнаруживаем начатие более 100 мс, иначе в Идл.
  // если нажатие более 500мс 
  // то ждем отпускания длительностью более 100мс и активируем LED_7_KBD_CLICK
  // Если длительность нажатия менее 500 мс 
  //    то 
  //       если период отпускания более 500мс - активируем LED_7_KBD_CLICK
  //       иначе, если
  //            произошло еще нажатие, определяем его длительность
  //                    если отпускание при условии  длительность нажатия > 500мс, то активируем LED_7_KBD_PRESSED
  //                     иначе - активируем LED_7_KBD_DBL
  //                    если отпускание при условии  длительность нажатия <= 500мс, то активируем LED_7_KBD_DBL
  // Если нажато более 500мс, то  активируем LED_7_KBD_PRESSED
//
extern bool b_Send_ESP32_LED7_Packet ;
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
bool b_Mode_Click = false;
bool b_Mode_Press = false;
bool b_Mode_DblClk = false;
//
bool b_Prev_Click = false;
bool b_Prev_Press = false;
bool b_Prev_DblClk = false;
//
bool b_Next_Click = false;
bool b_Next_Press = false;
bool b_Next_DblClk = false;
//
bool b_Up_Click = false;
bool b_Up_Press = false;
bool b_Up_DblClk = false;
//
bool b_Down_Click = false;
bool b_Down_Press = false;
bool b_Down_DblClk = false;
//
extern  uint8_t u8_ManualButton ;
extern  uint8_t u8_ManualChannel ;
extern uint8_t u8_StateMaschine ;
extern   bool  b_StopStrobMode ;
//
extern bool b_Grouped ;
//
// MODE_BYTE_IR_CMD
extern bool b_Send_ESP32_IR_CMD_Packet;
//
extern bool b_Send_ESP32_OP_MODE_CMD_Packet ;
//
//
// MODE_BYTE_BRT_CMD
extern bool b_Send_ESP32_BRT_CMD_Packet ;
//
// Это 3 полубайта для задания режима и его параметра
extern uint8_t u8_MODE_State   ; // режим
extern uint8_t u8_MODE_Param_1 ; // старшая половина параметра
extern uint8_t u8_MODE_Param_2 ; // младшая половина параметра
//
// Нужно преобразовать нажатие на клавиши в команды управления ручным режимом.
// Prev Next  - это селектор канала Красный - УФ
// Через  u8_ManualChannel
// Up Down это копия void ProcessLightUp       (void); void ProcessLightDown     (void);
//
void Implement_KBD(void)  // ВНИМАНИЕ - ЭТО Клавиатура на МАКЕТНОЙ ПЛАТЕ
{
  //
  //  Обработка кликов
  // 
  // Клик на центральную кнопку активирует и перебирает автоматические режимы
  if  ( b_Mode_Click )
  {
    b_Mode_Click   = false;
    //
    if (u8_StateMaschine == SM_MODE_MANUAL) 
      Process_Mode_Stroboscope_Button(); // Активация стробоскопа из ручного 
    else
      if ( u8_StateMaschine == SM_MODE_STROBOSCOPE)
        Process_Mode_Sequential_Button ();  // Активация последовательного из строба
        else
          if ( u8_StateMaschine == SM_MODE_SEQUENTIAL)
            Process_Mode_Rainbow_Button();  // Активация радуги из последовательного
            else
              if ( u8_StateMaschine == SM_MODE_RAINBOW)
                Process_Mode_Manual_Button(); // Активация ручного из радуги
    //       
  }
  else //
  if  (b_Prev_Click)
  {
    b_Prev_Click   = false;
 //   if (u8_ManualChannel == MANUAL_MODE_UNDEF) u8_ManualChannel = MANUAL_MODE_WHITE; 
 //   else
      if (u8_ManualChannel == MANUAL_MODE_RED  ) u8_ManualChannel = MANUAL_MODE_WHITE;
      else
        if (u8_ManualChannel == MANUAL_MODE_GREEN  ) u8_ManualChannel = MANUAL_MODE_RED;  
        else
          if (u8_ManualChannel == MANUAL_MODE_BLUE  ) u8_ManualChannel = MANUAL_MODE_GREEN;  
          else
            if (u8_ManualChannel == MANUAL_MODE_UV  ) u8_ManualChannel = MANUAL_MODE_BLUE;  
            else
              if (u8_ManualChannel == MANUAL_MODE_WHITE  ) u8_ManualChannel = MANUAL_MODE_UV;  
    //
    if ( b_Grouped )
    {
      u8_MODE_State   = MODE_BYTE_IR_CMD;
      //
      if (u8_ManualChannel         ==  MANUAL_MODE_RED   )  u8_MODE_Param_1 = IR_CMD_RED ; //     5
      else
        if (u8_ManualChannel       ==  MANUAL_MODE_GREEN )  u8_MODE_Param_1 = IR_CMD_GRN ; //     6 
        else
          if (u8_ManualChannel     ==  MANUAL_MODE_BLUE  )  u8_MODE_Param_1 = IR_CMD_BLU ; //     7
          else
            if (u8_ManualChannel   ==  MANUAL_MODE_UV    )  u8_MODE_Param_1 = IR_CMD_UV  ; //  IR_CMD_F1 #define IR_CMD_F1      12
            else
              if (u8_ManualChannel ==  MANUAL_MODE_WHITE )  u8_MODE_Param_1 = IR_CMD_WHT ; //     8
                else
                                                            u8_MODE_Param_1 = IR_CMD_RED ;  //    5      
      //
      u8_MODE_Param_2 = MODE_PARAM_LOW_NIBBLE_IDLE ;  
      //
      b_Send_ESP32_IR_CMD_Packet = true;
    }
  }
  else //
  if  (b_Next_Click)
  {
    b_Next_Click   = false;
    //
   // if (u8_ManualChannel == MANUAL_MODE_UNDEF) u8_ManualChannel = MANUAL_MODE_RED; 
   // else
      if (u8_ManualChannel == MANUAL_MODE_RED  ) u8_ManualChannel = MANUAL_MODE_GREEN;
      else
        if (u8_ManualChannel == MANUAL_MODE_GREEN  ) u8_ManualChannel = MANUAL_MODE_BLUE;  
        else
          if (u8_ManualChannel == MANUAL_MODE_BLUE  ) u8_ManualChannel = MANUAL_MODE_UV;  
          else
            if (u8_ManualChannel == MANUAL_MODE_UV  ) u8_ManualChannel = MANUAL_MODE_WHITE;  
            else
              if (u8_ManualChannel == MANUAL_MODE_WHITE  ) u8_ManualChannel = MANUAL_MODE_RED;  
    //  
    if ( b_Grouped )
    {
      u8_MODE_State   = MODE_BYTE_IR_CMD;
      //
      if (u8_ManualChannel         ==  MANUAL_MODE_RED   )  u8_MODE_Param_1 = IR_CMD_RED ; //     5
      else
        if (u8_ManualChannel       ==  MANUAL_MODE_GREEN )  u8_MODE_Param_1 = IR_CMD_GRN ; //     6 
        else
          if (u8_ManualChannel     ==  MANUAL_MODE_BLUE  )  u8_MODE_Param_1 = IR_CMD_BLU ; //     7
          else
            if (u8_ManualChannel   ==  MANUAL_MODE_UV    )  u8_MODE_Param_1 = IR_CMD_UV  ; //  IR_CMD_F1 #define IR_CMD_F1      12
            else
              if (u8_ManualChannel ==  MANUAL_MODE_WHITE )  u8_MODE_Param_1 = IR_CMD_WHT ; //     8
                else
                                                            u8_MODE_Param_1 = IR_CMD_RED ;  //    5      
      //
      u8_MODE_Param_2 = MODE_PARAM_LOW_NIBBLE_IDLE ;  
      //
      b_Send_ESP32_IR_CMD_Packet = true;
    }  
  }
  else //
  if  (b_Up_Click)
  {    b_Up_Click      = false; 
    // Этот обработчик нажатия на кнопку на внешнем пульте.
    // Он дейсвтвует сходно с функцией    void ProcessLightUp(void)
    // Если видим флаг  b_Up_Click, то ставим соответсвующее значение переменной u8_ManualButton
    // Затем 
    //  если фиксируется работа в групповом режиме и команда НЕ БЫЛА ПЕРЕХВАЧЕНА ИЗ МОДУЛЯ,
    //  а является результатом работы ик пульта или нажатия на кропку - транслируем ее в окружение.
    // 
    // Реакция в зависимости от состояния
    // В сервисном режиме - изменяем роль. 
    if ( u8_StateMaschine == SM_MODE_SERVICE )
        Process_ChangeRoleCommand();
    else  // В нормальном режиме работы (ручной и остальные) - выполняем увеличение яркости  
       ProcessLightUp() ;
  }
  else //
  if  (b_Down_Click)
  {
    b_Down_Click      = false;
    ProcessLightDown( );
    //
  }
  //
  //  Обработка пресов
  if  (b_Mode_Press)
  {
    b_Mode_Press  = false;
    //
    /* оТТовизм старой версии
    if (  u8_StateMaschine ==   SM_MODE_STROBOSCOPE    ) //  100
    {
      b_StopStrobMode = true;//u8_StateMaschine = SM_MODE_MANUAL  ;
    } */
    //
    // Если находися в ручном режиме, то активируем режим выключения с установкой флага
    // активации сервисного режима. 
    // Из режима Стробоскопа, последовательного и радуги - не активируется. 
    if (u8_StateMaschine == SM_MODE_MANUAL) 
      Process_Mode_SERVICE();
  }
  else //
  if  (b_Prev_Press)
  {
      b_Prev_Press  = false;
  }
  else //
  if  (b_Next_Press)
  {
       b_Next_Press = false;
  }
  else //
  if  (b_Up_Press)
  {
      b_Up_Press  = false;
  }
  else //
  if  (b_Down_Press)
  {
      b_Down_Press  = false;
  }
  //
  // Обработка даблкликов
  if  (b_Mode_DblClk)
  {
    b_Mode_DblClk  = false;
    //
    if ( u8_StateMaschine == SM_MODE_SERVICE )
    {
      // Сохраняем роль в сети 
      // перезагружаем контроллер
       u8_StateMaschine = SM_MODE_SAVE_REBOOT;
    }
  }
  else  //
  if  (b_Prev_DblClk)
  {
      b_Prev_DblClk  = false;
  }
  else 
  if  (b_Next_DblClk)
  {
      b_Next_DblClk  = false;
  }
  else //
  if  (b_Up_DblClk)
  {
      b_Up_DblClk  = false; 
  }
  else //
  if  (b_Down_DblClk)
  {
      b_Down_DblClk  = false;
      //
      if (u8_ManualChannel == MANUAL_MODE_RED  )              ProcessRedButton ();
      else
        if (u8_ManualChannel == MANUAL_MODE_GREEN  )         ProcessGreenButton();
        else
          if (u8_ManualChannel == MANUAL_MODE_BLUE  )         ProcessBlueButton();
          else
            if (u8_ManualChannel == MANUAL_MODE_UV  )           ProcessUvButton();
            else
              if (u8_ManualChannel == MANUAL_MODE_WHITE  )   ProcessWhiteButton();
    
  }
  // ------------------------------ //
}
#define _10ms   1
#define _20ms   2
#define _50ms   5
#define _80ms   8
#define _100ms 10
#define _200ms 20
#define _300ms 30
#define _400ms 40
#define _500ms 50


#define KBD_SM_IDLE_TIME              _20ms
#define KBD_SM_FIRST_DOWN_TIME       _500ms
#define KBD_SM_FIRST_DOWN_USC_TIME    _50ms
#define KBD_SM_FIRST_UP_TIME         _100ms
#define KBD_SM_FIRST_UP_USC_TIME     _200ms
#define KBD_SM_FIRST_UP_DSC_TIME      _20ms
#define KBD_SM_SECOND_DOWN_TIME      _500ms
#define KBD_SM_SECOND_DOWN_USC_TIME  _100ms
#define KBD_SM_SECOND_UP_TIME        _200ms

  #define KBD_SM_IDLE                           0 
  #define KBD_SM_FIRST_DOWN                     1
  #define KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE    2
  #define KBD_SM_FIRST_UP                       3
  #define KBD_SM_SECOND_DOWN                    4
  #define KBD_SM_SECOND_UP                      5 

__STATIC_INLINE void ProcessButton_Mode(void)
{
  //
  static uint32_t u32_KBD_SM_Status  = KBD_SM_IDLE;
  // 
  static int32_t  i32_DownStateCounter = 0; // Счетчик нажатого состояний.
  static int32_t  i32_UpStateCounter   = 0; // Счетчик отпущенного состояния.
  //
  static uint32_t u32_State ;
  //
  u32_State = LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_PORT, MODE_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(UP_BUTTON_GPIO_PORT, UP_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(DOWN_BUTTON_GPIO_PORT, DOWN_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(PREV_BUTTON_GPIO_PORT, PREV_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(NEXT_BUTTON_GPIO_PORT, NEXT_BUTTON_PIN);
  //
  //
  if (u32_State)
  { 
    if (i32_UpStateCounter<100) 
        i32_UpStateCounter++;
    if (i32_DownStateCounter>0) 
      i32_DownStateCounter--; 
  }
  else
  { 
    if (i32_DownStateCounter<100) 
        i32_DownStateCounter++;
    if (i32_UpStateCounter>0    ) 
        i32_UpStateCounter--; 
  }
  //
  switch(u32_KBD_SM_Status)
  {
    case KBD_SM_IDLE:
     if ( i32_DownStateCounter>=KBD_SM_IDLE_TIME ) //_20ms 
     { // фиксируем первое нажатие
       i32_UpStateCounter = 0; // Инициализируем счетчик отпускания
       u32_KBD_SM_Status =  KBD_SM_FIRST_DOWN   ;    
     }
    break;
    //
    case KBD_SM_FIRST_DOWN:
       // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_FIRST_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Mode_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>=KBD_SM_FIRST_DOWN_USC_TIME ) // >50ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_FIRST_UP; // начинаем считать длительность отпускание   
                                  i32_DownStateCounter = 0;
       }  
    break;
    //
    case KBD_SM_FIRST_UP:
      if ( u8_LED7_State == LED_7_KBD_WAIT_FOR_PRESS_OFF ) // Да, ждать  LED_7_KBD_WAIT_FOR_PRESS_OFF, которое будет после отправки
      { // ждем отправки состояния и отпускание после длинного нажатия
        if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        return;//break;
      }
      else
        if (u8_LED7_State == KBD_SM_IDLE )
        { // Отпускание после первого нажатия
          if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_USC_TIME ) // >=200ms
          { // Отпускание в рамках события одиночного нажатия
            i32_DownStateCounter = 0;
            i32_UpStateCounter = 0;
            u32_KBD_SM_Status =  KBD_SM_IDLE ; // Начинаем ждать нового события нажатия от пользователя
            u8_LED7_State = LED_7_KBD_CLICK; // Изменяем состояние светодиодов  
            b_Mode_Click  = true ; 
            b_Send_ESP32_LED7_Packet = true ;
          }
          else
           if ( i32_DownStateCounter>KBD_SM_FIRST_UP_DSC_TIME ) // >20ms 
           { // Произошло повторное нажатие в пределах 0,3 секунду после отпускания
             u32_KBD_SM_Status = KBD_SM_SECOND_DOWN;
             i32_UpStateCounter = 0;
           }
        }       
    break;
    //
    case  KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE:
      // Дали в сервис отправки запрос на отправку статуса LED_7_KBD_PRESSED
      // Отправили состояние LED_7_KBD_PRESSED и перешли в ожидание когда повторно не надо отправлять,
      // а надо дождаться отпусткания
       if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        
     break;
    //    
    //
    case KBD_SM_SECOND_DOWN:
      // Обрабатываем второе нажатие на кнопку
      // Возможно это второе короткое нажатие и генерация события двойного нажатия
      // или эрзац вариант длинного нажатия 
      // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_SECOND_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие. Эрзац вариант
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Mode_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>KBD_SM_SECOND_DOWN_USC_TIME ) // >100ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем считать длительность отпускание   
       }  
    break;
    //
    case KBD_SM_SECOND_UP:
      // Отпускание после длинного нажатия
      if (u8_LED7_State == LED_7_KBD_PRESSED )
      { // ждем отпускание после длинного нажатия
        if ( i32_UpStateCounter>KBD_SM_SECOND_UP_TIME )// >200ms
        { // фиксируем первое отпускание после ЭРЗАЦ-длинного нажатия
          u32_KBD_SM_Status    = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter = 0;
          i32_UpStateCounter   = 0;
        }      
      }
      // Отпускание после ВТОРОГО короткого нажатия
      else
      {
         // Отпускание в рамках события двойного нажатия
          i32_DownStateCounter = 0;
          i32_UpStateCounter = 0;
          u32_KBD_SM_Status =  KBD_SM_IDLE;  // Начинаем ждать нового события нажатия от пользователя
          u8_LED7_State = LED_7_KBD_DBL; // Изменяем состояние светодиодов  
          b_Mode_DblClk = true ;      
          b_Send_ESP32_LED7_Packet = true ;
      }
    break;
    //
    default:
      break;
  }
}


__STATIC_INLINE void ProcessButton_Up(void)
{

  //
 static uint32_t u32_KBD_SM_Status  = KBD_SM_IDLE;
  // 
  static int32_t  i32_DownStateCounter = 0; // Счетчик нажатого состояний.
  static int32_t  i32_UpStateCounter   = 0; // Счетчик отпущенного состояния.
  
  static uint32_t u32_State ;
  //
//u8_State = LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_PORT, MODE_BUTTON_PIN);
  u32_State = LL_GPIO_IsInputPinSet(UP_BUTTON_GPIO_PORT, UP_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(DOWN_BUTTON_GPIO_PORT, DOWN_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(PREV_BUTTON_GPIO_PORT, PREV_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(NEXT_BUTTON_GPIO_PORT, NEXT_BUTTON_PIN);
  //
  //
  if (u32_State)
  { 
    if (i32_UpStateCounter<100) 
        i32_UpStateCounter++;
    if (i32_DownStateCounter>0) 
      i32_DownStateCounter--; 
  }
  else
  { 
    if (i32_DownStateCounter<100) 
        i32_DownStateCounter++;
    if (i32_UpStateCounter>0    ) 
        i32_UpStateCounter--; 
  }
  //
  switch(u32_KBD_SM_Status)
  {
    case KBD_SM_IDLE:
     if ( i32_DownStateCounter>=KBD_SM_IDLE_TIME ) //_20ms 
     { // фиксируем первое нажатие
       i32_UpStateCounter = 0; // Инициализируем счетчик отпускания
       u32_KBD_SM_Status =  KBD_SM_FIRST_DOWN   ;    
     }
    break;
    //
    case KBD_SM_FIRST_DOWN:
       // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_FIRST_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Up_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>=KBD_SM_FIRST_DOWN_USC_TIME ) // >50ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_FIRST_UP; // начинаем считать длительность отпускание   
                                  i32_DownStateCounter = 0;
       }  
    break;
    //
    case KBD_SM_FIRST_UP:
      if ( u8_LED7_State == LED_7_KBD_WAIT_FOR_PRESS_OFF ) // Да, ждать  LED_7_KBD_WAIT_FOR_PRESS_OFF, которое будет после отправки
      { // ждем отправки состояния и отпускание после длинного нажатия
        if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        return;//break;
      }
      else
        if (u8_LED7_State == KBD_SM_IDLE )
        { // Отпускание после первого нажатия
          if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_USC_TIME ) // >=200ms
          { // Отпускание в рамках события одиночного нажатия
            i32_DownStateCounter = 0;
            i32_UpStateCounter = 0;
            u32_KBD_SM_Status =  KBD_SM_IDLE ; // Начинаем ждать нового события нажатия от пользователя
            u8_LED7_State = LED_7_KBD_CLICK; // Изменяем состояние светодиодов  
            b_Up_Click  = true ; 
            b_Send_ESP32_LED7_Packet = true ;
          }
          else
           if ( i32_DownStateCounter>KBD_SM_FIRST_UP_DSC_TIME ) // >20ms 
           { // Произошло повторное нажатие в пределах 0,3 секунду после отпускания
             u32_KBD_SM_Status = KBD_SM_SECOND_DOWN;
             i32_UpStateCounter = 0;
           }
        }       
    break;
    //
    case  KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE:
      // Дали в сервис отправки запрос на отправку статуса LED_7_KBD_PRESSED
      // Отправили состояние LED_7_KBD_PRESSED и перешли в ожидание когда повторно не надо отправлять,
      // а надо дождаться отпусткания
       if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        
     break;
    //    
    //
    case KBD_SM_SECOND_DOWN:
      // Обрабатываем второе нажатие на кнопку
      // Возможно это второе короткое нажатие и генерация события двойного нажатия
      // или эрзац вариант длинного нажатия 
      // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_SECOND_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие. Эрзац вариант
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Up_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>KBD_SM_SECOND_DOWN_USC_TIME ) // >100ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем считать длительность отпускание   
       }  
    break;
    //
    case KBD_SM_SECOND_UP:
      // Отпускание после длинного нажатия
      if (u8_LED7_State == LED_7_KBD_PRESSED )
      { // ждем отпускание после длинного нажатия
        if ( i32_UpStateCounter>KBD_SM_SECOND_UP_TIME )// >200ms
        { // фиксируем первое отпускание после ЭРЗАЦ-длинного нажатия
          u32_KBD_SM_Status    = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter = 0;
          i32_UpStateCounter   = 0;
        }      
      }
      // Отпускание после ВТОРОГО короткого нажатия
      else
      {
         // Отпускание в рамках события двойного нажатия
          i32_DownStateCounter = 0;
          i32_UpStateCounter = 0;
          u32_KBD_SM_Status =  KBD_SM_IDLE;  // Начинаем ждать нового события нажатия от пользователя
          u8_LED7_State = LED_7_KBD_DBL; // Изменяем состояние светодиодов  
          b_Up_DblClk = true ;      
          b_Send_ESP32_LED7_Packet = true ;
      }
    break;
    //
    default:
      break;
  }
}



__STATIC_INLINE void ProcessButton_Down(void)
{

  //
  static uint32_t u32_KBD_SM_Status  = KBD_SM_IDLE;
  // 
  static int32_t  i32_DownStateCounter = 0; // Счетчик нажатого состояний.
  static int32_t  i32_UpStateCounter   = 0; // Счетчик отпущенного состояния.
  //
  static uint32_t u32_State ;
  //
//u8_State = LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_PORT, MODE_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(UP_BUTTON_GPIO_PORT, UP_BUTTON_PIN);
  u32_State = LL_GPIO_IsInputPinSet(DOWN_BUTTON_GPIO_PORT, DOWN_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(PREV_BUTTON_GPIO_PORT, PREV_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(NEXT_BUTTON_GPIO_PORT, NEXT_BUTTON_PIN);
  //
  //
  //
  if (u32_State)
  { 
    if (i32_UpStateCounter<100) 
        i32_UpStateCounter++;
    if (i32_DownStateCounter>0) 
      i32_DownStateCounter--; 
  }
  else
  { 
    if (i32_DownStateCounter<100) 
        i32_DownStateCounter++;
    if (i32_UpStateCounter>0    ) 
        i32_UpStateCounter--; 
  }
  //
  switch(u32_KBD_SM_Status)
  {
    case KBD_SM_IDLE:
     if ( i32_DownStateCounter>=KBD_SM_IDLE_TIME ) //_20ms 
     { // фиксируем первое нажатие
       i32_UpStateCounter = 0; // Инициализируем счетчик отпускания
       u32_KBD_SM_Status =  KBD_SM_FIRST_DOWN   ;    
     }
    break;
    //
    case KBD_SM_FIRST_DOWN:
       // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_FIRST_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Down_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>=KBD_SM_FIRST_DOWN_USC_TIME ) // >50ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_FIRST_UP; // начинаем считать длительность отпускание   
                                  i32_DownStateCounter = 0;
       }  
    break;
    //
    case KBD_SM_FIRST_UP:
      if ( u8_LED7_State == LED_7_KBD_WAIT_FOR_PRESS_OFF ) // Да, ждать  LED_7_KBD_WAIT_FOR_PRESS_OFF, которое будет после отправки
      { // ждем отправки состояния и отпускание после длинного нажатия
        if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        return;//break;
      }
      else
        if (u8_LED7_State == KBD_SM_IDLE )
        { // Отпускание после первого нажатия
          if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_USC_TIME ) // >=200ms
          { // Отпускание в рамках события одиночного нажатия
            i32_DownStateCounter = 0;
            i32_UpStateCounter = 0;
            u32_KBD_SM_Status =  KBD_SM_IDLE ; // Начинаем ждать нового события нажатия от пользователя
            u8_LED7_State = LED_7_KBD_CLICK; // Изменяем состояние светодиодов  
            b_Down_Click  = true ; 
            b_Send_ESP32_LED7_Packet = true ;
          }
          else
           if ( i32_DownStateCounter>KBD_SM_FIRST_UP_DSC_TIME ) // >20ms 
           { // Произошло повторное нажатие в пределах 0,3 секунду после отпускания
             u32_KBD_SM_Status = KBD_SM_SECOND_DOWN;
             i32_UpStateCounter = 0;
           }
        }       
    break;
    //
    case  KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE:
      // Дали в сервис отправки запрос на отправку статуса LED_7_KBD_PRESSED
      // Отправили состояние LED_7_KBD_PRESSED и перешли в ожидание когда повторно не надо отправлять,
      // а надо дождаться отпусткания
       if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        
     break;
    //    
    //
    case KBD_SM_SECOND_DOWN:
      // Обрабатываем второе нажатие на кнопку
      // Возможно это второе короткое нажатие и генерация события двойного нажатия
      // или эрзац вариант длинного нажатия 
      // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_SECOND_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие. Эрзац вариант
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Down_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>KBD_SM_SECOND_DOWN_USC_TIME ) // >100ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем считать длительность отпускание   
       }  
    break;
    //
    case KBD_SM_SECOND_UP:
      // Отпускание после длинного нажатия
      if (u8_LED7_State == LED_7_KBD_PRESSED )
      { // ждем отпускание после длинного нажатия
        if ( i32_UpStateCounter>KBD_SM_SECOND_UP_TIME )// >200ms
        { // фиксируем первое отпускание после ЭРЗАЦ-длинного нажатия
          u32_KBD_SM_Status    = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter = 0;
          i32_UpStateCounter   = 0;
        }      
      }
      // Отпускание после ВТОРОГО короткого нажатия
      else
      {
         // Отпускание в рамках события двойного нажатия
          i32_DownStateCounter = 0;
          i32_UpStateCounter = 0;
          u32_KBD_SM_Status =  KBD_SM_IDLE;  // Начинаем ждать нового события нажатия от пользователя
          u8_LED7_State = LED_7_KBD_DBL; // Изменяем состояние светодиодов  
          b_Down_DblClk = true ;      
          b_Send_ESP32_LED7_Packet = true ;
      }
    break;
    //
    default:
      break;
  }
}
//
//
__STATIC_INLINE void ProcessButton_Next(void)
{
  //
  static uint32_t u32_KBD_SM_Status  = KBD_SM_IDLE;
  // 
  static int32_t  i32_DownStateCounter = 0; // Счетчик нажатого состояний.
  static int32_t  i32_UpStateCounter   = 0; // Счетчик отпущенного состояния.
  //
  static uint32_t u32_State ;
  //
//u8_State = LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_PORT, MODE_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(UP_BUTTON_GPIO_PORT, UP_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(DOWN_BUTTON_GPIO_PORT, DOWN_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(PREV_BUTTON_GPIO_PORT, PREV_BUTTON_PIN);
  u32_State = LL_GPIO_IsInputPinSet(NEXT_BUTTON_GPIO_PORT, NEXT_BUTTON_PIN);
  //
  //
  if (u32_State)
  { 
    if (i32_UpStateCounter<100) 
        i32_UpStateCounter++;
    if (i32_DownStateCounter>0) 
      i32_DownStateCounter--; 
  }
  else
  { 
    if (i32_DownStateCounter<100) 
        i32_DownStateCounter++;
    if (i32_UpStateCounter>0    ) 
        i32_UpStateCounter--; 
  }
  //
  switch(u32_KBD_SM_Status)
  {
    case KBD_SM_IDLE:
     if ( i32_DownStateCounter>=KBD_SM_IDLE_TIME ) //_20ms 
     { // фиксируем первое нажатие
       i32_UpStateCounter = 0; // Инициализируем счетчик отпускания
       u32_KBD_SM_Status =  KBD_SM_FIRST_DOWN   ;    
     }
    break;
    //
    case KBD_SM_FIRST_DOWN:
       // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_FIRST_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Next_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>=KBD_SM_FIRST_DOWN_USC_TIME ) // >50ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_FIRST_UP; // начинаем считать длительность отпускание   
                                  i32_DownStateCounter = 0;
       }  
    break;
    //
    case KBD_SM_FIRST_UP:
      if ( u8_LED7_State == LED_7_KBD_WAIT_FOR_PRESS_OFF ) // Да, ждать  LED_7_KBD_WAIT_FOR_PRESS_OFF, которое будет после отправки
      { // ждем отправки состояния и отпускание после длинного нажатия
        if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        return;//break;
      }
      else
        if (u8_LED7_State == KBD_SM_IDLE )
        { // Отпускание после первого нажатия
          if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_USC_TIME ) // >=200ms
          { // Отпускание в рамках события одиночного нажатия
            i32_DownStateCounter = 0;
            i32_UpStateCounter = 0;
            u32_KBD_SM_Status =  KBD_SM_IDLE ; // Начинаем ждать нового события нажатия от пользователя
            u8_LED7_State = LED_7_KBD_CLICK; // Изменяем состояние светодиодов  
            b_Next_Click  = true ; 
            b_Send_ESP32_LED7_Packet = true ;
          }
          else
           if ( i32_DownStateCounter>KBD_SM_FIRST_UP_DSC_TIME ) // >20ms 
           { // Произошло повторное нажатие в пределах 0,3 секунду после отпускания
             u32_KBD_SM_Status = KBD_SM_SECOND_DOWN;
             i32_UpStateCounter = 0;
           }
        }       
    break;
    //
    case  KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE:
      // Дали в сервис отправки запрос на отправку статуса LED_7_KBD_PRESSED
      // Отправили состояние LED_7_KBD_PRESSED и перешли в ожидание когда повторно не надо отправлять,
      // а надо дождаться отпусткания
       if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        
     break;
    //    
    //
    case KBD_SM_SECOND_DOWN:
      // Обрабатываем второе нажатие на кнопку
      // Возможно это второе короткое нажатие и генерация события двойного нажатия
      // или эрзац вариант длинного нажатия 
      // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_SECOND_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие. Эрзац вариант
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Next_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>KBD_SM_SECOND_DOWN_USC_TIME ) // >100ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем считать длительность отпускание   
       }  
    break;
    //
    case KBD_SM_SECOND_UP:
      // Отпускание после длинного нажатия
      if (u8_LED7_State == LED_7_KBD_PRESSED )
      { // ждем отпускание после длинного нажатия
        if ( i32_UpStateCounter>KBD_SM_SECOND_UP_TIME )// >200ms
        { // фиксируем первое отпускание после ЭРЗАЦ-длинного нажатия
          u32_KBD_SM_Status    = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter = 0;
          i32_UpStateCounter   = 0;
        }      
      }
      // Отпускание после ВТОРОГО короткого нажатия
      else
      {
         // Отпускание в рамках события двойного нажатия
          i32_DownStateCounter = 0;
          i32_UpStateCounter = 0;
          u32_KBD_SM_Status =  KBD_SM_IDLE;  // Начинаем ждать нового события нажатия от пользователя
          u8_LED7_State = LED_7_KBD_DBL; // Изменяем состояние светодиодов  
          b_Next_DblClk = true ;      
          b_Send_ESP32_LED7_Packet = true ;
      }
    break;
    //
    default:
      break;
  }
}


//
__STATIC_INLINE void ProcessButton_Prev(void)
{
  //
  static uint32_t u32_KBD_SM_Status  = KBD_SM_IDLE;
  // 
  static int32_t  i32_DownStateCounter = 0; // Счетчик нажатого состояний.
  static int32_t  i32_UpStateCounter   = 0; // Счетчик отпущенного состояния.
  //
  static uint32_t u32_State ;
  //
//u8_State = LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_PORT, MODE_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(UP_BUTTON_GPIO_PORT, UP_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(DOWN_BUTTON_GPIO_PORT, DOWN_BUTTON_PIN);
  u32_State = LL_GPIO_IsInputPinSet(PREV_BUTTON_GPIO_PORT, PREV_BUTTON_PIN);
//u8_State = LL_GPIO_IsInputPinSet(NEXT_BUTTON_GPIO_PORT, NEXT_BUTTON_PIN);
  //
   //
  if (u32_State)
  { 
    if (i32_UpStateCounter<100) 
        i32_UpStateCounter++;
    if (i32_DownStateCounter>0) 
      i32_DownStateCounter--; 
  }
  else
  { 
    if (i32_DownStateCounter<100) 
        i32_DownStateCounter++;
    if (i32_UpStateCounter>0    ) 
        i32_UpStateCounter--; 
  }
  //
  switch(u32_KBD_SM_Status)
  {
    case KBD_SM_IDLE:
     if ( i32_DownStateCounter>=KBD_SM_IDLE_TIME ) //_20ms 
     { // фиксируем первое нажатие
       i32_UpStateCounter = 0; // Инициализируем счетчик отпускания
       u32_KBD_SM_Status =  KBD_SM_FIRST_DOWN   ;    
     }
    break;
    //
    case KBD_SM_FIRST_DOWN:
       // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_FIRST_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Prev_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>=KBD_SM_FIRST_DOWN_USC_TIME ) // >50ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_FIRST_UP; // начинаем считать длительность отпускание   
                                  i32_DownStateCounter = 0;
       }  
    break;
    //
    case KBD_SM_FIRST_UP:
      if ( u8_LED7_State == LED_7_KBD_WAIT_FOR_PRESS_OFF ) // Да, ждать  LED_7_KBD_WAIT_FOR_PRESS_OFF, которое будет после отправки
      { // ждем отправки состояния и отпускание после длинного нажатия
        if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        return;//break;
      }
      else
        if (u8_LED7_State == KBD_SM_IDLE )
        { // Отпускание после первого нажатия
          if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_USC_TIME ) // >=200ms
          { // Отпускание в рамках события одиночного нажатия
            i32_DownStateCounter = 0;
            i32_UpStateCounter = 0;
            u32_KBD_SM_Status =  KBD_SM_IDLE ; // Начинаем ждать нового события нажатия от пользователя
            u8_LED7_State = LED_7_KBD_CLICK; // Изменяем состояние светодиодов  
            b_Prev_Click  = true ; 
            b_Send_ESP32_LED7_Packet = true ;
          }
          else
           if ( i32_DownStateCounter>KBD_SM_FIRST_UP_DSC_TIME ) // >20ms 
           { // Произошло повторное нажатие в пределах 0,3 секунду после отпускания
             u32_KBD_SM_Status = KBD_SM_SECOND_DOWN;
             i32_UpStateCounter = 0;
           }
        }       
    break;
    //
    case  KBD_SM_FIRST_DOWN_WAIT_FOR_RELEASE:
      // Дали в сервис отправки запрос на отправку статуса LED_7_KBD_PRESSED
      // Отправили состояние LED_7_KBD_PRESSED и перешли в ожидание когда повторно не надо отправлять,
      // а надо дождаться отпусткания
       if ( i32_UpStateCounter>=KBD_SM_FIRST_UP_TIME ) // >=200ms
        { // фиксируем первое отпускание после длинного нажатия
          u32_KBD_SM_Status         = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter     = 0;
          u8_LED7_State            = LED_7_KBD_UP ; // LED_7_KBD_UP работает как LED_7_DEFAULT ; 
          b_Send_ESP32_LED7_Packet = true ;
        }    
        
     break;
    //    
    //
    case KBD_SM_SECOND_DOWN:
      // Обрабатываем второе нажатие на кнопку
      // Возможно это второе короткое нажатие и генерация события двойного нажатия
      // или эрзац вариант длинного нажатия 
      // Находимся в состоянии нажатия
       if ( (i32_DownStateCounter>KBD_SM_SECOND_DOWN_TIME) && (u8_LED7_State != LED_7_KBD_PRESSED) ) // >500ms
       { // фиксируем длинное нажатие. Эрзац вариант
          i32_UpStateCounter = 0; // Начинаем ждать отпускание
          u8_LED7_State     = LED_7_KBD_PRESSED; // Изменяем состояние светодиодов
          b_Prev_Press  = true ; 
          b_Send_ESP32_LED7_Packet = true ;
                                   u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
       } 
       if ( i32_UpStateCounter>KBD_SM_SECOND_DOWN_USC_TIME ) // >100ms
       { // фиксируем отпускание. 
         // По умолчание - нажатие было коротким, так как в обработчике длинного нажатия 
         // выполняется переход в u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем ждать отпускание
                                  u32_KBD_SM_Status  = KBD_SM_SECOND_UP; // начинаем считать длительность отпускание   
       }  
    break;
    //
    case KBD_SM_SECOND_UP:
      // Отпускание после длинного нажатия
      if (u8_LED7_State == LED_7_KBD_PRESSED )
      { // ждем отпускание после длинного нажатия
        if ( i32_UpStateCounter>KBD_SM_SECOND_UP_TIME )// >200ms
        { // фиксируем первое отпускание после ЭРЗАЦ-длинного нажатия
          u32_KBD_SM_Status    = KBD_SM_IDLE;   // Начинаем ждать нового события нажатия от пользователя
          i32_DownStateCounter = 0;
          i32_UpStateCounter   = 0;
        }      
      }
      // Отпускание после ВТОРОГО короткого нажатия
      else
      {
         // Отпускание в рамках события двойного нажатия
          i32_DownStateCounter = 0;
          i32_UpStateCounter = 0;
          u32_KBD_SM_Status =  KBD_SM_IDLE;  // Начинаем ждать нового события нажатия от пользователя
          u8_LED7_State = LED_7_KBD_DBL; // Изменяем состояние светодиодов  
          b_Prev_DblClk = true ;      
          b_Send_ESP32_LED7_Packet = true ;
      }
    break;
    //
    default:
      break;
  }
}

//
void ProcessControlButtons (void)
{
   static uint32_t  u32_CallCounter=0;
  u32_CallCounter++;
  //
  if (u32_CallCounter == (760/2+1)) // >=
  {
    // Это 0,01 секунды или 760 вызовов. Частота вызовов 76000
    // На частоте прерываний ИК таймера величиной 38 кГц 0,01 секунды или 760/2 вызовов.
 // u32_CallCounter=0;
    //
    ProcessButton_Mode();
    //
 //  ProcessButton_Up();
 //  ProcessButton_Down();
    //
 //  ProcessButton_Next();
 //  ProcessButton_Prev();   
    //
  }
  // else  
  if (u32_CallCounter ==  (760/2+2)) // >=
  {
    // Это 0,01 секунды или 760 вызовов. Частота вызовов 76000
    // На частоте прерываний ИК таймера величиной 38 кГц 0,01 секунды или 760/2 вызовов.
 // u32_CallCounter=0;
    //
  //    ProcessButton_Mode();
    //
 ProcessButton_Up();
 //  ProcessButton_Down();
    //
 //  ProcessButton_Next();
 //  ProcessButton_Prev();      
    //
  }
  //
  // else  
  if (u32_CallCounter == (760/2+3)) // >=
  {
    // Это 0,01 секунды или 760 вызовов. Частота вызовов 76000
    // На частоте прерываний ИК таймера величиной 38 кГц 0,01 секунды или 760/2 вызовов.
 // u32_CallCounter=0;
    //
  //   ProcessButton_Mode();
    //
 //  ProcessButton_Up();
  ProcessButton_Down();
    //
 //  ProcessButton_Next();
 //  ProcessButton_Prev();    
    //
  }
  //
  // else  
  if (u32_CallCounter == (760/2+4)) // >=
  {
    // Это 0,01 секунды или 760 вызовов. Частота вызовов 76000
    // На частоте прерываний ИК таймера величиной 38 кГц 0,01 секунды или 760/2 вызовов.
 // u32_CallCounter=0;
    //
  //   ProcessButton_Mode();
    //
 //  ProcessButton_Up();
 //  ProcessButton_Down();
    //
  ProcessButton_Next();
 //  ProcessButton_Prev();   
    //
  }
  //
  // else  
  if (u32_CallCounter == (760/2+5)) // >=
  {
    // Это 0,01 секунды или 760 вызовов. Частота вызовов 76000
    // На частоте прерываний ИК таймера величиной 38 кГц 0,01 секунды или 760/2 вызовов.
 //   u32_CallCounter=0;
    //
   //  ProcessButton_Mode();
    //
 //  ProcessButton_Up();
 //  ProcessButton_Down();
    //
 //  ProcessButton_Next();
  ProcessButton_Prev();    
    //
  }
  // else  
  if (u32_CallCounter == (760/2+6)) // >=
  {
    // Это 0,01 секунды или 760 вызовов. Частота вызовов 76000
    // На частоте прерываний ИК таймера величиной 38 кГц 0,01 секунды или 760/2 вызовов.
    u32_CallCounter=0;
    //
   //  ProcessButton_Mode();
    //
 //  ProcessButton_Up();
 //  ProcessButton_Down();
    //
 //  ProcessButton_Next();
 // ProcessButton_Prev();    
    //
    Implement_KBD();
  }
  //
  //      
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
