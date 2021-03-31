/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 *
 * ---
 *
 * Adopted for STM32CubeIDE :
 * - Milos Vidojevic <mvidojevic82@gmail.com>
 *
 */

#include "platform.h"
#include "stm32f4xx.h"
#include "main.h"
#include "gpio.h"
#include "spi.h"

bool_t
platform_init( Gpu_HalInit_t * halinit )
{
    ( void )halinit;
    return TRUE;
}

void
platform_sleep_ms( uint32_t ms )
{
    HAL_Delay( ms );
}

bool_t
platform_spi_init( Gpu_Hal_Context_t * phost )
{
    ( void )phost;
    return TRUE;
}

void
platform_spi_deinit( Gpu_Hal_Context_t * phost )
{
    ( void )phost;
	return;
}

uint8_t
platform_spi_send_recv_byte( Gpu_Hal_Context_t * phost, uint8_t data, uint32_t opt )
{
    uint8_t answer;

    ( void )phost;
    HAL_SPI_TransmitReceive( &hspi2, &data, &answer, 1, HAL_MAX_DELAY );

    return answer;
}

uint16_t
platform_spi_send_data( Gpu_Hal_Context_t * phost, uint8_t * data, uint16_t size, uint32_t opt )
{
    ( void )phost;
    HAL_SPI_Transmit( &hspi2, data, size, HAL_MAX_DELAY );

    return size;
}

void
platform_spi_recv_data( Gpu_Hal_Context_t * phost, uint8_t * data, uint16_t size, uint32_t opt )
{
    ( void )phost;
    HAL_SPI_Receive( &hspi2, data, size, HAL_MAX_DELAY );
}

bool_t
platform_gpio_init( Gpu_Hal_Context_t * phost, gpio_name ngpio )
{
    ( void )phost;
    ( void )ngpio;
    return TRUE;
}

bool_t
platform_gpio_value( Gpu_Hal_Context_t * phost, gpio_name ngpio, gpio_val vgpio )
{
	switch ( ngpio )
	{
	case GPIO_CS:

		HAL_GPIO_WritePin( R_CS_GPIO_Port, R_CS_Pin, vgpio );

		break;
	case GPIO_PD:

		HAL_GPIO_WritePin( R_RST_GPIO_Port, R_RST_Pin, vgpio );

		break;
    case GPIO_INT:

	    //HAL_GPIO_WritePin( RIB_INT_GPIO_Port, RIB_INT_Pin, vgpio );

		break;
    case GPIO_CLK:

        //HAL_GPIO_WritePin( GPIOB, GPIO_PIN_13, vgpio );

        break;
    case GPIO_SDO:

        //HAL_GPIO_WritePin( GPIOB, GPIO_PIN_15, vgpio );

        break;
	default:
        return FALSE;
	}

    return TRUE;
}
