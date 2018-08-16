/**********************************************
/ timer.c
/
/ refered to following blog
/ http://roboneco.hateblo.jp/entry/20130911/1378900563
/ http://hima-tubusi.blogspot.jp/2012/04/stbeeminil.html
/
/*********************************************/

#include "timer.h"
#include "stm32f4xx.h"


//#define TIM_Period 65535
//#define TIM_Prescaler 1000000
//#define TIM_ClockDivision 0

void NVIC_Configuration()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	// enable the TIM7 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1 );
}

void Tim_Configuration()
{
	// supply clock to TIM7
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 16800 - 1;  // make 1ms ((CPU clock / Prescalar) / TIM_Period = 1ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 10 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);

	TIM_Cmd(TIM7, ENABLE);
}

void TIM7_IRQHandler()
{
	Sum_Time_ms();
}

uint32_t Sum_Time_ms()
{
	static uint32_t time_ms = 0;
	time_ms++;
	return time_ms;
}

uint32_t Get_Time_ms()
{
	int time;
	time = Sum_Time_ms() - 1;
	return time; // have to minus 1 bacause
}

//void Systick_Handler()
//{
//	Sum_Time_ms();
//}
