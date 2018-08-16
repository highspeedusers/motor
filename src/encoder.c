/********************************************************
/ encoder.c
/ refered to following blog and modified (2016/6/12)
/ http://aoimouse.blogspot.jp/2015/05/stm32f4.html
/
/*******************************************************/
#include <stdio.h>
#include "stm32f4xx.h"
#include "encoder.h"

int32_t Get_Encoder_Count_Of_Steering_Motor()
{
	return TIM5->CNT;
}

int32_t Get_Encoder_Count_Of_Drive_Motor()
{
	return TIM2->CNT;
}


void Clear_Encoder_Count_Of_Steering_Motor()
{
	TIM5->CNT = 0;
}

void Clear_Encoder_Count_Of_Drive_Motor()
{
	TIM2->CNT = 0;
}

void Steering_Motor_Encoder_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
    // Encoder Interface Mode
    GPIO_PinAFConfig(GPIOH , GPIO_PinSource10 , GPIO_AF_TIM5);
    GPIO_PinAFConfig(GPIOH , GPIO_PinSource11 , GPIO_AF_TIM5);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOH, &GPIO_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    /* TIM5 encoder mode enable*/
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12
        ,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
    TIM_Cmd(TIM5, ENABLE);
    Clear_Encoder_Count_Of_Steering_Motor();
}

void Drive_Motor_Encoder_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
    // Encoder Interface Mode
    GPIO_PinAFConfig(GPIOA , GPIO_PinSource15 , GPIO_AF_TIM2);
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource3 , GPIO_AF_TIM2);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* TIM3 encoder mode enable*/
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12
        ,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
    TIM_Cmd(TIM2, ENABLE);
    Clear_Encoder_Count_Of_Drive_Motor();
}

void Encoder_Init()
{
	Steering_Motor_Encoder_Init();
	Drive_Motor_Encoder_Init();
}

