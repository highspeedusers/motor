/**
  ******************************************************************************
  * @file    common.h
  * @author  Hibot
  * @version V1.0.0
  * @date    31-August-2012  * @brief   Usart Liblary of Applilation layer
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx.h"


/* Exported types ------------------------------------------------------------*/
typedef enum
{
    USART_CH3,
    USART_CH6,
    USART_CH_Num
} USART_CH;


typedef struct
{
    USART_TypeDef*  Reg;
    GPIO_TypeDef*   TX_PORT;
    GPIO_TypeDef*   RX_PORT;
    uint32_t        USART_CLK;
    uint32_t        TX_PORT_CLK;
    uint32_t        RX_PORT_CLK;
    uint16_t        TX_PIN;
    uint16_t        RX_PIN;
    uint16_t        TX_PIN_SOURCE;
    uint16_t        RX_PIN_SOURCE;
    uint16_t        TX_AF;
    uint16_t        RX_AF;
    uint8_t         NVIC_IRQChannel;
} USART_Config_st;


typedef struct
{
    uint32_t        BufSize;

    uint8_t*        TxBuffer;
    uint16_t        TxBuf_Read_Index;
    uint16_t        TxBuf_Write_Index;
    uint32_t        TxBufStoreCount;
    uint32_t        TxBufReadCount;
    uint8_t         TxBufInUse;

    uint8_t*        RxBuffer;
    uint16_t        RxBuf_Read_Index;
    uint16_t        RxBuf_Write_Index;
    uint32_t        RxBufStoreCount;
    uint32_t        RxBufReadCount;
    uint8_t         RxBufInUse;
} USART_Buf_st;

/* Exported constants --------------------------------------------------------*/

#define USART_BaudRate_9600       9600
#define USART_BaudRate_19200     19200
#define USART_BaudRate_38400     38400
#define USART_BaudRate_57600     57600
#define USART_BaudRate_115200   115200



/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void     USART_Config(USART_CH USARTn, uint32_t BaudRate, uint32_t Buf_Size);
void     USART_COMInit(USART_CH USARTn, USART_InitTypeDef* USART_InitStruct);
uint8_t  STM32_putc(char data);
uint32_t STM32_puts(char *data);
uint32_t USART_Tx_Buf_Write(USART_CH USARTn, char* Data, uint32_t size);
uint32_t USART_GetReceivedDataSize(USART_CH USARTn);
uint32_t USART_GetReceivedData(USART_CH USARTn, char* pBuffer, uint32_t size);
uint32_t USART_DEBUG_PRINT(char* Data);
//void ReceivedData(char str[]);
char get_hold_char(char letter, char mode);
void get_data(char* data, int get);


#ifdef __cplusplus
}
#endif


/************************END OF FILE****************************/
