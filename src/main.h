/**
  ******************************************************************************
  * @file    main.h
  * @author  Hibot
  * @version V1.0.0
  * @date    31-August-2012
  * @brief   Header for main.c module
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#define SYSTEMTICK_PERIOD_MS 10

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>

//@ #include <stdio.h>

/******************/
/* Use function */
/******************/
#define USE_UART3
//#define USE_UART6
//@ #define USE_TCP
#define USE_UDP
//#define USE_CAN




/* Uncomment SERIAL_DEBUG to enables retarget of printf to  serial port (COM1 on STM32 TITECH M4board)
   for debug purpose */
#define SERIAL_DEBUG
#define DEBUG_SERIAL_CH     USART_CH3

//#define USE_LCD        /* enable LCD  */
//#define USE_DHCP       /* enable DHCP, if disabled static address is used */

/* MII and RMII mode selection, for TITECH M4L Board(MB786) RevB ***********/
#define RMII_MODE  // User have to provide the 50 MHz clock by soldering a 50 MHz
                     // oscillator (ref SM7745HEV-50.0M or equivalent) on the U3
                     // footprint located under CN3 and also removing jumper on JP5.
                     // This oscillator is not provided with the board.
                     // For more details, please refer to TITECH M4
                     // board User manual (UM1461).
//#define MII_MODE

/* Uncomment the define below to clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef 	MII_MODE
 #define PHY_CLOCK_MCO
#endif

/* TITECH M4 jumpers setting
    +==========================================================================================+
    +  Jumper |       MII mode configuration            |      RMII mode configuration         +
    +==========================================================================================+
    +  JP5    | 2-3 provide 25MHz clock by MCO(PA8)     |  Not fitted                          +
    +         | 1-2 provide 25MHz clock by ext. Crystal |                                      +
    + -----------------------------------------------------------------------------------------+
    +  JP6    |          2-3                            |  1-2                                 +
    + -----------------------------------------------------------------------------------------+
    +  JP8    |          Open                           |  Close                               +
    +==========================================================================================+
  */


/* TCP port */
#define TCP_PORT    7

/* UDP port */
#define UDP_LOCAL_PORT      10003   /* define the UDP local connection port  */
#define UDP_REMOTE_PORT     10004   /* define the UDP remote connection port */

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0           2
#define MAC_ADDR1           0
#define MAC_ADDR2           0
#define MAC_ADDR3           0
#define MAC_ADDR4           0
#define MAC_ADDR5           0

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0            10
#define IP_ADDR1            0
#define IP_ADDR2            1
#define IP_ADDR3            220

/*NETMASK*/
#define NETMASK_ADDR0       255
#define NETMASK_ADDR1       255
#define NETMASK_ADDR2       255
#define NETMASK_ADDR3       0

/*Gateway Address*/
#define GW_ADDR0            192
#define GW_ADDR1            168
#define GW_ADDR2            0
#define GW_ADDR3            1

/* Destination IP ADDRESS */
#define DEST_IP_ADDR0       10
#define DEST_IP_ADDR1       0
#define DEST_IP_ADDR2       1
#define DEST_IP_ADDR3       103



#define MAKE_IRQ_PRIORITY( pre, sub )   (uint8_t)( (((pre) & 0x0f) << 4) | (sub & 0x0f) )
#define GET_IRQ_PREENPTION_PRIOROTY(p)  (uint8_t)((p) >> 4)
#define GET_IRQ_SUB_PRIOROTY(p)         (uint8_t)((p) & 0x07)

#define I2C1_PRIORITY   MAKE_IRQ_PRIORITY( 3, 0 )
#define UART_PRIORITY   MAKE_IRQ_PRIORITY( 2, 0 )
#define AD_PRIORITY     MAKE_IRQ_PRIORITY( 3, 0 )
#define TIM_PRIORITY    MAKE_IRQ_PRIORITY( 3, 0 )
#define TIM2_PRIORITY   MAKE_IRQ_PRIORITY( 3, 0 )
#define TIM3_PRIORITY   MAKE_IRQ_PRIORITY( 3, 0 )
#define CAN_PRIORITY    MAKE_IRQ_PRIORITY( 3, 0 )
#define CAN1_PRIORITY   MAKE_IRQ_PRIORITY( 3, 0 )
#define CAN2_PRIORITY   MAKE_IRQ_PRIORITY( 3, 0 )
#define ETHER_PRIORITY  MAKE_IRQ_PRIORITY( 3, 0 )

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Delay(uint32_t nCount);
void Time_Update(void);
void TIM_Config(void);
void LED_Config(void);
void ADC1_CH0_Config(void);

#endif /* __MAIN_H */

/************************END OF FILE****/
