
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LLU_V2_LIGHT_CONTROL_H
#define __LLU_V2_LIGHT_CONTROL_H



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

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

// Общий код для LPU_V2 MPU_V2 HPU_V2 
//   
// Инициализируем каналы путем настройки таймеров.
void     LLU_V2_RedChannelInit  (void); 
void     LLU_V2_GreenChannelInit(void);
void     LLU_V2_BlueChannelInit (void); 
void     LLU_V2_UvChannelInit   (void); 
void     LLU_V2_WhiteChannelInit(void); 
//
//
void ProcessMainStateMaschine(void);
//
//
void TurnOnPattern    (void);
void TurnOffPattern    (void);

void ProcessPowerUp   (void);
void ProcessPowerDown (void);
//
void ProcessLightUp       (void);
void ProcessLightDown     (void);
//
void ProcessRedButton     (void);
void ProcessGreenButton   (void);
void ProcessBlueButton    (void);
void ProcessUvButton      (void);
void ProcessWhiteButton   (void);
//
//
//
// ЭТО ОБРАБОТЧИКИ ДЛЯ КНОПОК. Нажали на кнопку - ВЫЗВАЛИ эти обработчики.
// S - Верхняя
void Process_Mode_Stroboscope_Button(void); 
// S - нижняя
void Process_Mode_Sequential_Button (void);
// F - самая нижняя
void Process_Mode_Rainbow_Button (void);    
// Сиреневая
void Process_Mode_Manual_Button (void);     
//
//
void Process_Mode_SERVICE      ( void );     
void Process_ChangeRoleCommand ( void ); 
//
//
//
void Process_Mode_Manual     (void) ; // Заглушка. Для поддержания восприятия.
void Process_Mode_Stroboscope(void) ; 
//
//__STATIC_INLINE 
void Process_Mode_Sequential(void) ;
void Process_Mode_Rainbow    (void) ; 
//
//
void SetRedLevel(uint16_t u16_LightIndex);
void SetGreenLevel(uint16_t u16_LightIndex);
void SetBlueLevel(uint16_t u16_LightIndex);
void SetUvLevel(uint16_t u16_LightIndex);
void SetWhiteLevel(uint16_t u16_LightIndex);


// __STATIC_INLINE void     UserButton_Init(void);
__STATIC_INLINE void     BOARD_EN_Init(void);

 void Delay_uS   (uint16_t u16_Delay);

// ПЕРВЫЙ ВАРИАНТ. С ПУЛЬТОМ, без платы управления
/* 
// Режимы работы - ручной и автоматический
// В ручном режиме можно произвольно настраивать цвет и оттенок путем задания интенсивности 
// по каждому каналу 
// В автоматическом режиме доступно три функции - плавно по цвета, плавно по радуге, прерывисто.
#define SM_MODE_STAND_BY                        0
// Включение
#define SM_MODE_TURN_ON                         1
// Выключение
#define SM_MODE_TURN_OFF                        2
//
#define SM_MODE_MANUAL                         10
//
//
//
// Активация режима затенения
#define SM_MODE_DIM_ALL                 11
// Все погашено
#define SM_MODE_ALL_DIMMED              12
//
// Это общий обработчик реакции на кнопку
#define SM_MODE_MANUAL_CHANGE_STATE 13
//
// изменение состояние в ручном режиме
#define SM_MODE_MANUAL_UNDEF      10
#define SM_MODE_MANUAL_RED        11
#define SM_MODE_MANUAL_GREEN_ON   12
#define SM_MODE_MANUAL_BLUE_ON    13
#define SM_MODE_MANUAL_UV_ON      14
#define SM_MODE_MANUAL_WHITE_ON   15



//
// Для обработки сигнала от кнопки в ручном режиме
// Начальное или неопределенное состояние
#define MANUAL_MODE_BUTTON_UNDEF        0
//
#define MANUAL_MODE_BUTTON_RED          1
#define MANUAL_MODE_BUTTON_GREEN        2
#define MANUAL_MODE_BUTTON_BLUE         3
#define MANUAL_MODE_BUTTON_UV           4
#define MANUAL_MODE_BUTTON_WHITE        5
//
#define MANUAL_MODE_BUTTON_LIGHT_UP     6
#define MANUAL_MODE_BUTTON_LIGHT_DOWN   7
//
#define MANUAL_MODE_BUTTON_MODE_A       8
#define MANUAL_MODE_BUTTON_MODE_B       9
#define MANUAL_MODE_BUTTON_MODE_C      10
#define MANUAL_MODE_BUTTON_STROBOSCOPE 11
//
//
//
#define SM_MODE_FUNCTION_A                      20
#define SM_MODE_FUNCTION_B                      30
#define SM_MODE_FUNCTION_C                      40
//
#define SM_MODE_STROBOSCOPE                     50
//

// В ручном режиме можно управлять одним из пяти каналов
// Это что-бы определять на что воздействовать при нажатии на Ярче/тускнее и т.п.
#define MANUAL_MODE_UNDEF 0
#define MANUAL_MODE_RED   1
#define MANUAL_MODE_GREEN 2
#define MANUAL_MODE_BLUE  3
#define MANUAL_MODE_UV    4
#define MANUAL_MODE_WHITE 5

// Для отслеживания типа автоматического режима
#define FUNCTION_MODE_UNDEF 0
#define FUNCTION_MODE_A     1
#define FUNCTION_MODE_B     2
#define FUNCTION_MODE_C     3
//
//
// ТОЛЬКО 2 СОСТОЯНИЯ. Третье не добавлять.
#define MODE_STATUS_STOP 0
#define MODE_STATUS_RUN  1
//
//
// Для идентификации канала влияния при приеме сообщения SetLightLevel]
#define SELECTOR_ACTUAL_WHITE 0
#define SELECTOR_ACTUAL_UV    1
//
// режим синхронизации устройстав
#define SYNC_MODE_INDEP 0
#define SYNC_MODE_GROUP 1
//
//
// Общая задержка для работы в автоматическом режиме
#define UPDATE_DELAY_MIN_MS       1
#define UPDATE_DELAY_DEFALT_MS   50
#define UPDATE_DELAY_MAX_MS    100
//
//
*/
// 
// ВТОРОЙ ВАРИАНТ. С ПЛАТОЙ УПРАВЛЕНИЯ
// Включили питание
#define SM_MODE_POWER_UP  0
//
 // Включение
#define SM_MODE_TURN_ON                         1
// Выключение
#define SM_MODE_TURN_OFF                        2
//
//
// Перезагрузка с предшествующей записью BIOS в ПЗУ.
#define SM_MODE_SAVE_REBOOT                     5
// 
// Определение корректности прошивки
// Драйвера линий, АЦП. таймеры, ИК, Модуль
#define SM_MODE_FW_CHECK   10
// 
// Инициализация аппаратной части 
#define SM_MODE_FW_INIT    20
// 
// Начало работы прошивки
#define SM_MODE_FW_READY   30
// 
// Устройство готово к внешним нажатиям и командам
// для варианта двух MPU_V2 - Сразу перевожу в ручной режим 
#define SM_MODE_STAND_BY   40
//
// Ручной. Общий вызов
#define SM_MODE_MANUAL     50
// Управление в ручном режиме, через ИК ПУЛЬТ или ПУ
// изменение состояние в ручном режиме
#define SM_MODE_MANUAL_UNDEF   50
#define SM_MODE_MANUAL_RED     51
#define SM_MODE_MANUAL_GREEN   52
#define SM_MODE_MANUAL_BLUE    53
#define SM_MODE_MANUAL_UV      54
#define SM_MODE_MANUAL_WHITE   55
//
// Сервисный режим. 
// Прим. Далее по коду - приведены состояния данного режима.  
#define SM_MODE_SERVICE        80
//                         
// Стробоскоп
#define SM_MODE_STROBOSCOPE 100
//
// Последовательно, цвет за цветом 
#define SM_MODE_SEQUENTIAL  150
// 
// Радуга 
#define SM_MODE_RAINBOW     200


 
 
 // Для обработки сигнала от кнопки в ручном режиме
// Начальное или неопределенное состояние
#define MANUAL_MODE_BUTTON_UNDEF        0
//
#define MANUAL_MODE_BUTTON_RED          1
#define MANUAL_MODE_BUTTON_GREEN        2
#define MANUAL_MODE_BUTTON_BLUE         3
#define MANUAL_MODE_BUTTON_UV           4
#define MANUAL_MODE_BUTTON_WHITE        5
//
#define MANUAL_MODE_BUTTON_LIGHT_UP     6
#define MANUAL_MODE_BUTTON_LIGHT_DOWN   7
//
//
// 
// Стробоскоп  
#define MANUAL_MODE_BUTTON_STROBOSCOPE   8
// Последовательный
#define MANUAL_MODE_BUTTON_SEQUENTIAL    9 
// Радуга
#define MANUAL_MODE_BUTTON_RAINBOW      10 
//
// Ручной
#define MANUAL_MODE_BUTTON_MANUAL       11 
//
//
// В ручном режиме можно управлять одним из пяти каналов
// Это что-бы определять на что воздействовать при нажатии на Ярче/тускнее и т.п.
//#define MANUAL_MODE_UNDEF 0
#define MANUAL_MODE_RED   0
#define MANUAL_MODE_GREEN 1
#define MANUAL_MODE_BLUE  2
#define MANUAL_MODE_UV    3
#define MANUAL_MODE_WHITE 4
// По аналогии с макросами селекции канала управления в ручном режиме,
// формирую макросы селекции канала в режиме управления яркостью через 
// команды прямого управления
#define _ACTUAL_RED MANUAL_MODE_RED
#define _ACTUAL_GRN MANUAL_MODE_GREEN
#define _ACTUAL_BLU MANUAL_MODE_BLUE
#define _ACTUAL_UV  MANUAL_MODE_UV
#define _ACTUAL_WHT MANUAL_MODE_WHITE
//
// Для отслеживания типа автоматического режима
#define FUNCTION_MODE_UNDEF 0
#define FUNCTION_MODE_STRB  1
#define FUNCTION_MODE_SEQN  2
#define FUNCTION_MODE_RNBW  3
//
 // ТОЛЬКО 2 СОСТОЯНИЯ. Третье не добавлять.
#define MODE_STATUS_STOP 0
#define MODE_STATUS_RUN  1
 //
// Типы запросов режима SEQENTIAL
#define SEQ_MODE_REQUEST_IDLE           0
#define SEQ_MODE_MASTER_READY_FOR_START 1
#define SEQ_MODE_MASTER_START_IT        2
#define SEQ_MODE_SEQ_IS_ACTIVE          3
#define SEQ_MODE_SEQ_IS_ENDED           4
 //
 // Для идентификации канала влияния при приеме сообщения SetLightLevel]
#define SELECTOR_ACTUAL_WHITE 0
#define SELECTOR_ACTUAL_UV    1
//
// режим синхронизации устройстав
#define SYNC_MODE_INDEP 0
#define SYNC_MODE_GROUP 1
//
//
// Общая задержка для работы в автоматическом режиме
#define UPDATE_DELAY_MIN_MS       1
#define UPDATE_DELAY_DEFALT_MS   50
#define UPDATE_DELAY_MAX_MS     100
//
//
                                                  // IRQ Handler treatment.//
//void UserButton_Callback(void); 

// MODE PC10
#define MODE_BUTTON_PIN                   LL_GPIO_PIN_10
#define MODE_BUTTON_GPIO_PORT             GPIOC
#define MODE_BUTTON_GPIO_CLK_ENABLE()     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)  
//
// UP PD2
#define UP_BUTTON_PIN                     LL_GPIO_PIN_2
#define UP_BUTTON_GPIO_PORT               GPIOD
#define UP_BUTTON_GPIO_CLK_ENABLE()       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD)  
//
// DOWN PB5
#define DOWN_BUTTON_PIN                   LL_GPIO_PIN_5
#define DOWN_BUTTON_GPIO_PORT             GPIOB
#define DOWN_BUTTON_GPIO_CLK_ENABLE()     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)  
//
// PREV PC11
#define PREV_BUTTON_PIN                   LL_GPIO_PIN_11
#define PREV_BUTTON_GPIO_PORT             GPIOC
#define PREV_BUTTON_GPIO_CLK_ENABLE()     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC) 
//
// NEXT PC12
#define NEXT_BUTTON_PIN                   LL_GPIO_PIN_12
#define NEXT_BUTTON_GPIO_PORT             GPIOC
#define NEXT_BUTTON_GPIO_CLK_ENABLE()     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC) 
//
//
// TIMx interrupt processing //
void Timer1CaptureCompare_Callback(void); // Carrier
void Timer11CaptureCompare_Callback(void); // RedTimer
void Timer3CaptureCompare_Callback(void) ; // GreenTimer
void Timer2CaptureCompare_Callback(void) ; // BlueTimer
void Timer10CaptureCompare_Callback(void); // UvTimer
void Timer4CaptureCompare_Callback(void) ; // WhtTimer
//
//
// Макросы нужные для кодирования и декодирования элементов массива,
// каждый из которых содерчит данные о периоде и скважности.
#define LIGHT_LEVEL_UNKNOWN       0x0000
#define LIGHT_LEVEL_PERIOD_65535  0x4000
#define LIGHT_LEVEL_PERIOD_10499  0x8000
#define LIGHT_LEVEL_PERIOD_2624   0xC000
//
//
// Дублирующее объявление
void TurnOnPattern    (void);
void TurnOffPattern    (void);
//
void ProcessPowerUp   (void);
void ProcessPowerDown (void);
//
void ProcessLightUp       (void);
void ProcessLightDown     (void);
//
void  DirectLightCtrl( int32_t i32_Channel, int32_t i32_Perc); 
//
void ProcessRedButton     (void);
void ProcessGreenButton   (void);
void ProcessBlueButton    (void);
void ProcessUvButton      (void);
void ProcessWhiteButton   (void);
//
//
// Режим поочередного зажигания каналов
// Работаем на 5 каналов 
// Возможные состояния машины
// Изначально считал что будет только последовательный режим.
// Поэтому в названии SM_MODE_A
// Заменил на SM_MODE_SEQ
// Но если учесть что один и тот же код применим и для последовательного и 
// для радуги, то можно использовать совмещенный макрос
// МАКРОСТЫ И ДЛЯ ПОСЛЕДОВАТЕЛЬНОГО И ДЛЯ РАДУГИ
#define SM_MODE_RED_UP      0
#define SM_MODE_RED_DOWN    1
#define SM_MODE_GREEN_UP    2
#define SM_MODE_GREEN_DOWN  3
#define SM_MODE_BLUE_UP     4
#define SM_MODE_BLUE_DOWN   5
#define SM_MODE_UV_UP       6
#define SM_MODE_UV_DOWN     7
#define SM_MODE_WHITE_UP    8
#define SM_MODE_WHITE_DOWN  9
// Специально, что-бы в режиме определения статуса узла из сообщений консоли
//и начала выполенения последовательного и радуги, с целью блокирования паразитного
// включения красного (переменная инициализируется значением по умолчанияю), точнее
// uint32_t u32_Mode_Sqnt_SM            =      SM_MODE_RED_UP;  
// uint32_t u32_Mode_Rnbw_SM            =      SM_MODE_RED_UP; 
// ... пришлось ввести дополнительное состояние
#define SM_MODE_IDLE       10


//
//#define SM_MODE_A_STOP       10
//
// Режим воспроизведения
//#define PROCESS_MODE_A_PLAY 0
//#define PROCESS_MODE_A_PAUS 1
//
//
#define SYNC_ROLE_UNDEF     0
#define SYNC_ROLE_MASTER    1
#define SYNC_ROLE_SLAVE     2
//
// РЕЖИМ Стробоскоп
// Работаем на 5 каналов 
// Возможные состояния машины
#define SM_MODE_STROB_ON         0
#define SM_MODE_STROB_WAIT_OFF   5
#define SM_MODE_STROB_OFF       10
#define SM_MODE_STROB_ALL_DOWN  20 
#define SM_MODE_STROB_IDLE      30 
//
#define STROBE_REINIT_TIME_MS 2000
//
// Для обслуживания процесса инизиализации ведомого  из потока от мастера
// в момент старта.
#define FS_MODE_IDLE       0
#define FS_MODE_MANUAL     1
#define FS_MODE_STROBE     2
#define FS_MODE_SEQUENTIAL 3
#define FS_MODE_RAINBOW    4 
//
#define FS_PARAM_MANUAL     0
#define FS_PARAM_STROBE     1
#define FS_PARAM_SEQUENTIAL 2
#define FS_PARAM_RAINBOW    3
//
//
// Состояния сервисного режима
#define SERVICE_MODE_IDLE            0
#define SERVICE_MODE_ROLE_SELECTION  1 
//
// Коммуникационный модуль может работать в нескольких режимах, главным образом - в режиме по умолчанию 
// или в одном из принудительно заданных режимов.
#define ESP32_NET_ROLE_DEFAULT    0
#define ESP32_NET_ROLE_MASTER     1 
#define ESP32_NET_ROLE_ECHOSLAVE  2
#define ESP32_NET_ROLE_SLAVE      3 
#define ESP32_NET_ROLE_UNDEF      4 
//
//
#endif /* __LLU_V2_LIGHT_CONTROL_H */