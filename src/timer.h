/**********************************************
/ timer.h
/
/
/*********************************************/

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

void NVIC_Configuration(void);
void Tim_Configuration(void);
uint32_t Sum_Time_ms(void);
uint32_t Get_Time_ms(void);


#ifdef __cplusplus
}
#endif

#endif
