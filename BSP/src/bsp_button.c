/*
 * bsp_button.c
 *
 *  Created on: Aug 25, 2020
 *      Author: milos
 */

#include "bsp_button.h"
#include "bsp_print.h"
#include "gpio.h"

bool
BSP_Button1_is_pressed( void )
{
    return ( bool )!HAL_GPIO_ReadPin( USR_BTN_1_GPIO_Port, USR_BTN_1_Pin );
}

bool
BSP_Button1_is_pressedDebounce( uint32_t debounce )
{
    if ( !HAL_GPIO_ReadPin( USR_BTN_1_GPIO_Port, USR_BTN_1_Pin ) )
    {
        HAL_Delay( debounce );
        return ( bool )!HAL_GPIO_ReadPin( USR_BTN_1_GPIO_Port, USR_BTN_1_Pin );
    }

    return false;
}

bool
BSP_Button2_is_pressed( void )
{
    return ( bool )!HAL_GPIO_ReadPin( USR_BTN_2_GPIO_Port, USR_BTN_2_Pin );
}

bool
BSP_Button2_is_pressedDebounce( uint32_t debounce )
{
    if ( !HAL_GPIO_ReadPin( USR_BTN_2_GPIO_Port, USR_BTN_2_Pin ) )
    {
        HAL_Delay( debounce );
        return ( bool )!HAL_GPIO_ReadPin( USR_BTN_2_GPIO_Port, USR_BTN_2_Pin );
    }

    return false;
}

void
BSP_Button_LoopTest( void )
{
    BSP_Print( "\r\n\tButton Check" );

    if ( BSP_Button1_is_pressed( ) )
    {
        BSP_Print( " - BTN 1 press" );
    }

    if ( BSP_Button2_is_pressed( ) )
    {
        BSP_Print( " - BTN 2 press" );
    }
}
