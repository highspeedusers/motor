/**
  ******************************************************************************
  * @file    common.h 
  * @author  Hibot
  * @version V1.0.0
  * @date    31-August-2012
  * @brief   Common Header
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_common_H
#define __STM32_common_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/** 
  * @brief  STM32 Common Status
  */ 
typedef enum
{
  SC_OK = 0,
  SC_FAILURE, 
  SC_TIMEOUT,
  SC_PARAM_ERROR,
  SC_NOT_OPERATIONAL, 
  SC_END_OF_ERROR_CODE
}SC_Status;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* __STM32_commmon_H */

/**** END OF FILE ****/
