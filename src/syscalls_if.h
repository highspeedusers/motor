/********************************************************************************/
/*!
  * @file    common.h 
  * @author  Hibot
  * @version V1.0.0
  * @date    31-August-2012
	@brief          syscalls' Device Dependent Section.

    @section HISTORY
		2010.12.31	V1.00	ReStart Here.
		2011.03.10	V2.00	C++ Ready.

*/
/********************************************************************************/
#ifndef __SYSCALLS_IF_H 
#define __SYSCALLS_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Basis Inclusion (Device Dependent) */
//#include "hw_config.h"

/* printf/scanf relation settings */
/* platform dependent */
#define putch(x)  putch(x)
//#define putch(x) __io_putchar(int ch)
#define getch()   getch()
/* if use scanf echoback, uncomment it */
/* #define ECHOBACK */

/*
Notice: 
		Line Feed     : Dec=10 Hex=0x0A ESC='\n'
		Caridge Return: Dec=13 Hex=0x0D ESC='\r'
*/

#ifdef __cplusplus
}
#endif

#endif /* __SYSCALLS_IF_H  */
