/*
 * bsp_led.c
 *
 *  Created on: Aug 25, 2020
 *      Author: milos
 */

/*
 * bsp_led.h
 *
 *  Created on: Aug 25, 2020
 *      Author: milos
 */

#include "bsp_led.h"
#include "bsp_print.h"
#include "gpio.h"

void
BSP_LED1_on( void )
{
    HAL_GPIO_WritePin( USR_LED_1_GPIO_Port, USR_LED_1_Pin, GPIO_PIN_SET );
}

void
BSP_LED1_off( void )
{
    HAL_GPIO_WritePin( USR_LED_1_GPIO_Port, USR_LED_1_Pin, GPIO_PIN_RESET );
}

void
BSP_LED1_toggle( void )
{
    HAL_GPIO_TogglePin( USR_LED_1_GPIO_Port, USR_LED_1_Pin );
}

void
BSP_LED2_on( void )
{
    HAL_GPIO_WritePin( USR_LED_2_GPIO_Port, USR_LED_2_Pin, GPIO_PIN_SET );
}

void
BSP_LED2_off( void )
{
    HAL_GPIO_WritePin( USR_LED_2_GPIO_Port, USR_LED_2_Pin, GPIO_PIN_RESET );
}

void
BSP_LED2_toggle( void )
{
    HAL_GPIO_TogglePin( USR_LED_2_GPIO_Port, USR_LED_2_Pin );
}

void
BSP_LED_SingleTest( void )
{
    BSP_Print( "\r\nLED Test - OK" );
    BSP_LED1_on( );
    BSP_LED2_off( );
}

void
BSP_LED_LoopTest( void )
{
    BSP_Print( "\r\n\tToggle LED" );
    BSP_LED1_toggle( );
    BSP_LED2_toggle( );
}
