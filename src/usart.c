
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "stm32f4xx.h"
#include "usart.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef       USART_InitStructure;
USART_Buf_st            USART_Buf[USART_CH_Num];
USART_TypeDef*          USART_COM;

const USART_Config_st   USART_Info[USART_CH_Num] =  {
                                                        {   // USART3
                                                            USART3,
                                                            GPIOB,
                                                            GPIOB,
                                                            RCC_APB1Periph_USART3,
                                                            RCC_AHB1Periph_GPIOB,
                                                            RCC_AHB1Periph_GPIOB,
                                                            GPIO_Pin_10,
                                                            GPIO_Pin_11,
                                                            GPIO_PinSource10,
                                                            GPIO_PinSource11,
                                                            GPIO_AF_USART3,
                                                            GPIO_AF_USART3,
                                                            USART3_IRQn
                                                        },
                                                        {   // USART6
                                                            USART6,
                                                            GPIOC,
                                                            GPIOG,
                                                            RCC_APB2Periph_USART6,
                                                            RCC_AHB1Periph_GPIOC,
                                                            RCC_AHB1Periph_GPIOG,
                                                            GPIO_Pin_6,
                                                            GPIO_Pin_9,
                                                            GPIO_PinSource6,
                                                            GPIO_PinSource9,
                                                            GPIO_AF_USART6,
                                                            GPIO_AF_USART6,
                                                            USART6_IRQn
                                                        }
                                                    };



/* Private function prototypes -----------------------------------------------*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
void USART_Tx_Buf_Data(USART_CH  USARTn);
void USART_Rx_Buf_Write(USART_CH USARTn);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void USART_Config(USART_CH USARTn, uint32_t BaudRate, uint32_t Buf_Size)
{
    /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_stm32f4xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_stm32f4xx.c file
    */

    NVIC_InitTypeDef NVIC_InitStructure;

    // Initialize Buffer Info
    USART_Buf[USARTn].BufSize           = Buf_Size;

    USART_Buf[USARTn].TxBuf_Read_Index  = 0;
    USART_Buf[USARTn].TxBuf_Write_Index = 0;
    USART_Buf[USARTn].TxBufStoreCount   = 0;
    USART_Buf[USARTn].TxBufReadCount    = 0;
    USART_Buf[USARTn].TxBufInUse        = 0;

    USART_Buf[USARTn].RxBuf_Read_Index  = 0;
    USART_Buf[USARTn].RxBuf_Write_Index = 0;
    USART_Buf[USARTn].RxBufStoreCount   = 0;
    USART_Buf[USARTn].RxBufReadCount    = 0;
    USART_Buf[USARTn].RxBufInUse        = 0;

    USART_Buf[USARTn].TxBuffer          = malloc(USART_Buf[USARTn].BufSize);
    USART_Buf[USARTn].RxBuffer          = malloc(USART_Buf[USARTn].BufSize);

    if(USART_Buf[USARTn].TxBuffer == NULL)
    {
        return;
    }
    if(USART_Buf[USARTn].RxBuffer == NULL)
    {
        return;
    }

    USART_COM = USART_Info[USARTn].Reg;

    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART_Info[USARTn].NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = GET_IRQ_PREENPTION_PRIOROTY(UART_PRIORITY);
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = GET_IRQ_SUB_PRIOROTY(UART_PRIORITY);
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    /* NVIC configuration */
    NVIC_Init(&NVIC_InitStructure);

    /* USARTx configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate            = BaudRate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_COMInit(USARTn, &USART_InitStructure);

    USART_ITConfig(USART_Info[USARTn].Reg, USART_IT_RXNE, ENABLE);
}


/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:
  *     @arg COM1
  *     @arg COM2
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void USART_COMInit(USART_CH USARTn, USART_InitTypeDef* USART_InitStruct)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd((USART_Info[USARTn].TX_PORT_CLK | USART_Info[USARTn].RX_PORT_CLK), ENABLE);

    /* Enable UART clock */
    if(USARTn == USART_CH3)
    {
        RCC_APB1PeriphClockCmd(USART_Info[USARTn].USART_CLK, ENABLE);
    }
    else // USART6
    {
        RCC_APB2PeriphClockCmd(USART_Info[USARTn].USART_CLK, ENABLE);
    }

    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(USART_Info[USARTn].TX_PORT, USART_Info[USARTn].TX_PIN_SOURCE, USART_Info[USARTn].TX_AF);

    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(USART_Info[USARTn].RX_PORT, USART_Info[USARTn].RX_PIN_SOURCE, USART_Info[USARTn].RX_AF);

    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;

    GPIO_InitStructure.GPIO_Pin   = USART_Info[USARTn].TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_Info[USARTn].TX_PORT, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin   = USART_Info[USARTn].RX_PIN;
    GPIO_Init(USART_Info[USARTn].RX_PORT, &GPIO_InitStructure);

    /* USART configuration */
    USART_Init(USART_Info[USARTn].Reg, USART_InitStruct);

    /* Enable USART */
    USART_Cmd(USART_Info[USARTn].Reg, ENABLE);
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART_COM, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART_COM, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

/**
  * @brief  Send one byte data to USART.
  * @param  None
  * @retval None
  */
uint8_t STM32_putc(char data)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART_COM, (uint8_t) data);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART_COM, USART_FLAG_TC) == RESET)
    {}

    return data;
}


/**
  * @brief  Send multi-byte data to USART.
  * @param  None
  * @retval None
  */
uint32_t STM32_puts(char *data)
{
    char *c = data;
    uint32_t num = 0;

    while(*c != '\0')
    {
        STM32_putc(*c);
        c++;
        num++;
    }

    return num;
}


/**
  * @brief  Set Tx data to Tx buffer.
  * @param  Data transmission data pointer.
  * @param  size size of Data.
  * @retval size of set data.
  */
uint32_t USART_Tx_Buf_Write(USART_CH USARTn, char* Data, uint32_t size)
{
    uint32_t set_len;
    uint32_t empty_size;
    uint32_t i;

    set_len = strlen(Data);

    if((size > 0) && (size < set_len))
    {
        set_len = size;
    }

    empty_size = USART_Buf[USARTn].BufSize - (USART_Buf[USARTn].TxBufStoreCount - USART_Buf[USARTn].TxBufReadCount);

    if(empty_size < set_len)
    {
        set_len = empty_size;
    }

    for(i=0; i<set_len; i++)
    {
        USART_Buf[USARTn].TxBuffer[USART_Buf[USARTn].TxBuf_Write_Index] = Data[i];
        USART_Buf[USARTn].TxBuf_Write_Index++;
        USART_Buf[USARTn].TxBufInUse = 1;
        USART_Buf[USARTn].TxBufStoreCount++;
        USART_Buf[USARTn].TxBufInUse = 0;

        if(USART_Buf[USARTn].TxBuf_Write_Index >= USART_Buf[USARTn].BufSize)
        {
            USART_Buf[USARTn].TxBuf_Write_Index = 0;
        }
    }
    USART_ITConfig(USART_Info[USARTn].Reg, USART_IT_TXE, ENABLE);
    return i;
}


/**
  * @brief  Write Rx data to Buffer
  * @param  None
  * @retval None
  */
void USART_Tx_Buf_Data(USART_CH  USARTn)
{
    if(USART_Buf[USARTn].TxBufStoreCount == USART_Buf[USARTn].TxBufReadCount)
    {
        USART_ITConfig(USART_Info[USARTn].Reg, USART_IT_TXE, DISABLE);
    }
    else
    {
        uint16_t data;

        data = (uint16_t) USART_Buf[USARTn].TxBuffer[USART_Buf[USARTn].TxBuf_Read_Index];
        USART_SendData(USART_Info[USARTn].Reg, data);
        USART_Buf[USARTn].TxBuf_Read_Index++;
        USART_Buf[USARTn].TxBufReadCount++;

        if(USART_Buf[USARTn].TxBuf_Read_Index >= USART_Buf[USARTn].BufSize)
        {
            USART_Buf[USARTn].TxBuf_Read_Index = 0;
        }
    }

    if((USART_Buf[USARTn].TxBufReadCount  >= USART_Buf[USARTn].BufSize) &&
       (USART_Buf[USARTn].TxBufStoreCount >= USART_Buf[USARTn].BufSize) &&
       (USART_Buf[USARTn].TxBufInUse      == 0)                   )
    {
        USART_Buf[USARTn].TxBufReadCount  -= USART_Buf[USARTn].BufSize;
        USART_Buf[USARTn].TxBufStoreCount -= USART_Buf[USARTn].BufSize;
    }
}


/**
  * @brief  Get recieve data size.
  * @param  None
  * @retval size of recieve data.
  */
uint32_t USART_GetReceivedDataSize(USART_CH USARTn)
{
    return (USART_Buf[USARTn].RxBufStoreCount - USART_Buf[USARTn].RxBufReadCount);
}


/**
  * @brief  Get recieve data size.
  * @param  The number of data to get.
  *         0     : All the data stored in the buffer
  *         other :
  * @retval size of data.
  */
uint32_t USART_GetReceivedData(USART_CH USARTn, char* pBuffer, uint32_t size)
{
    uint32_t dataSize   = 0;
    uint32_t dataCount  = 0;

    if(size == 0)
    {
        dataSize = USART_Buf[USARTn].RxBufStoreCount - USART_Buf[USARTn].RxBufReadCount;
    }
    else
    {
        if(size < (USART_Buf[USARTn].RxBufStoreCount - USART_Buf[USARTn].RxBufReadCount))
        {
            dataSize = size;
        }
        else
        {
            dataSize = USART_Buf[USARTn].RxBufStoreCount - USART_Buf[USARTn].RxBufReadCount;
        }
    }

    for(dataCount=0; dataCount<dataSize; dataCount++)
    {
        *pBuffer = USART_Buf[USARTn].RxBuffer[USART_Buf[USARTn].RxBuf_Read_Index];
        USART_Buf[USARTn].RxBuf_Read_Index++;
        USART_Buf[USARTn].RxBufInUse = 1;
        USART_Buf[USARTn].RxBufReadCount++;
        USART_Buf[USARTn].RxBufInUse = 0;
        pBuffer++;

        if(USART_Buf[USARTn].RxBuf_Read_Index == USART_Buf[USARTn].BufSize)
        {
            USART_Buf[USARTn].RxBuf_Read_Index = 0;
        }
    }
    *pBuffer = '\0';

    return dataCount;
}


/**
  * @brief  Write Rx data to Buffer
  * @param  None
  * @retval None
  */
void USART_Rx_Buf_Write(USART_CH USARTn)
{
    uint32_t    RxBufStoreSize = USART_Buf[USARTn].RxBufStoreCount - USART_Buf[USARTn].RxBufReadCount;

    if( RxBufStoreSize < USART_Buf[USARTn].BufSize )
    {
        USART_Buf[USARTn].RxBuffer[USART_Buf[USARTn].RxBuf_Write_Index] = (USART_ReceiveData(USART3) & 0x7F);
        USART_Buf[USARTn].RxBuf_Write_Index++;
        USART_Buf[USARTn].RxBufStoreCount++;

        if( USART_Buf[USARTn].RxBuf_Write_Index == USART_Buf[USARTn].BufSize )
        {
            USART_Buf[USARTn].RxBuf_Write_Index  = 0;
        }

        if((USART_Buf[USARTn].RxBufStoreCount >= USART_Buf[USARTn].BufSize) &&
           (USART_Buf[USARTn].RxBufReadCount  >= USART_Buf[USARTn].BufSize) &&
           (USART_Buf[USARTn].RxBufInUse == 0)                      )
        {
            USART_Buf[USARTn].RxBufStoreCount -= USART_Buf[USARTn].BufSize;
            USART_Buf[USARTn].RxBufReadCount  -= USART_Buf[USARTn].BufSize;
        }
    }
}


uint32_t USART_DEBUG_PRINT(char* Data)
{
#ifdef DEBUG_SERIAL_CH
    return USART_Tx_Buf_Write(DEBUG_SERIAL_CH, Data, strlen(Data));
#endif /* DEBUG_SERIAL_CH */
}


/**
  * @brief  This function handles USARTx global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
    	USART_Rx_Buf_Write(USART_CH3);  // 受信した文字をバッファに保存する
//    	char str[64];
//    	char test_r = '\r\n';
//    	char test_u = USART_ReceiveData(USART3);
//    	sprintf(str," %d,  %d\r\n", test_r, test_u);
//    	STM32_puts(str);
    	if(USART_ReceiveData(USART3) == '\n')
    	        {
            // 改行コードを受信した時点でそれまでバッファに保存されていたデータを他の場所に保存しておき，必要なタイミングでそのデータを読む．
    	        	char data[64];
    	        	USART_GetReceivedData(USART_CH3, data, 0);
    	        	get_data(data, 0);

    	        }





    }

    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {
        USART_Tx_Buf_Data(USART_CH3);
    }
}


/**
  * @brief  This function handles USARTx global interrupt request.
  * @param  None
  * @retval None
  */
void USART6_IRQHandler(void)
{
    if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
    {
        USART_Rx_Buf_Write(USART_CH6);
    }

    if(USART_GetITStatus(USART6, USART_IT_TXE) != RESET)
    {
        USART_Tx_Buf_Data(USART_CH6);
    }
}

//char ReceivedData(char letter)
//{
//	static
//	return letter;
//}

char get_hold_char(char letter, char mode)
{
	static char command;
	char temp = '0';
    char str[128];
    char x;
    x = mode;

//    sprintf(str,"%c %c\r\n>",letter, mode);
//    STM32_puts(str);
	if(x == '0')
	{
		command = letter;
	}
	else if(x == '1')
	{
		temp = command;
		command = 'i';
	}
	return temp;
}

void get_data(char* data, int get)
{
	char temp[64] = {'\0'};
	static char hold_data[64] = {'\0'};

	if(get == 0)
	{
//		temp = data;
//		hold_data = temp;
		sprintf(hold_data, "%s\r\n", data);
	}
	if(get == 1)
	{
		sprintf(temp, "%s\r\n", hold_data);
		sprintf(data, "%s", temp);
//		STM32_puts(temp);
	}
}


/**** END OF FILE ****/
