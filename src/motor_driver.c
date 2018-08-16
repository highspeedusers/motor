/*******************************************************
/ motor_driver.c
/ 
/ PB0 and PB4 are used as PWM output pins.
/ PB10 and PB11 are used as PWM output pins.
/
/ initialize TIM as pwm mode
   |
   |-- select pins used as pwm pins
   |-- determine prescaler
   |-- must supply clocks to timer circuit and gpio port
     |-- 1. supply clocks.
     |-- 2. initialize gpio port
     |-- 3. initialize timer








/******************************************************/
#include "stm32f4xx.h"
#include "motor_driver.h"


void Steering_Motor_Driver_Init()
{
// we need pins for two PWM
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	TIM_OCInitTypeDef TIM_OCInitStructure;
 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	RCC_ClocksTypeDef RCC_Clocks;
 	RCC_GetClocksFreq(&RCC_Clocks);
	int PrescalerValue = 0;
 	PrescalerValue = (uint16_t) ((RCC_Clocks.SYSCLK_Frequency  / 42000000 ) + 0.5) - 1;

 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // PB enable
 	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;  // we set for pin0 and pin1
 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;  // select gpio mode as AF
 	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  // frequency of gpio clock  
 	GPIO_Init(GPIOB,&GPIO_InitStructure);
 	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);  // AF config
 	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);  // AF config


	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 666 - 1;
 	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

 	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  	/* TIM Interrupts enable */
  	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
  	/* TIM3 enable counter */
  	TIM_Cmd(TIM3, ENABLE);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=333;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC3Init(TIM3,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);


	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=333;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);

}

void Motor_Driver_Init()
{
	Steering_Motor_Driver_Init();
}


