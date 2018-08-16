/**
  ******************************************************************************
  * @file    sample_project/TIM1_RCC_Delay/inc/led.h
  * @author  Hibot
  * @version V1.0.0
  * @date    31-August-2012
  * @brief   Led Liblary of Applilation layer
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Led_H
#define __Led_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
   
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;

#define LEDn                             4

#define LED1_PIN                         GPIO_Pin_0
#define LED1_GPIO_PORT                   GPIOE
#define LED1_GPIO_CLK                    RCC_AHB1Periph_GPIOE  
  
#define LED2_PIN                         GPIO_Pin_1
#define LED2_GPIO_PORT                   GPIOE
#define LED2_GPIO_CLK                    RCC_AHB1Periph_GPIOE  
  
#define LED3_PIN                         GPIO_Pin_3
#define LED3_GPIO_PORT                   GPIOE
#define LED3_GPIO_CLK                    RCC_AHB1Periph_GPIOE  
  
#define LED4_PIN                         GPIO_Pin_4
#define LED4_GPIO_PORT                   GPIOE
#define LED4_GPIO_CLK                    RCC_AHB1Periph_GPIOE

void LED_Init(Led_TypeDef Led);
void LED_On(Led_TypeDef Led);
void LED_Off(Led_TypeDef Led);
void LED_Toggle(Led_TypeDef Led);

#ifdef __cplusplus
}
#endif

#endif /* __APPLIB_Led_H */

