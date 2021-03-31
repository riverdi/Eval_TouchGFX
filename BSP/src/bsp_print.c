/*
 * printer.c
 *
 *  Created on: Aug 25, 2020
 *      Author: milosv
 */

#include "usart.h"
#include <bsp_print.h>
#include <stdio.h>
#include <string.h>

void
BSP_Print( const char * format, ... )
{
    va_list arg;
    char str[128] ={0};

    va_start( arg, format );
    vsprintf( str, format, arg );
    va_end( arg );
    HAL_UART_Transmit( &huart6, ( unsigned char * )str, strlen( str ), 1000 );
}
