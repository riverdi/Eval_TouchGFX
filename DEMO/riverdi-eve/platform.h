/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 *
 * ---
 *
 * Adopted for STM32CubeIDE :
 * - Milos Vidojevic <mvidojevic82@gmail.com>
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stdarg.h>
#include <string.h>
#include "Gpu_Hal.h"

/*****************************************************************************/

#define TRUE		    (1)
#define FALSE		    (0)

typedef char		    bool_t;
typedef char		    char8_t;
typedef unsigned char	uchar8_t;
typedef signed char	    schar8_t;
typedef float		    float_t;

/* EVE inclusions */

//#include "Gpu.h"
//#include "CoPro_Cmds.h"
//#include "Hal_Utils.h"

/*****************************************************************************/

typedef enum
{
    GPIO_CS,
    GPIO_PD,
    GPIO_INT,
    GPIO_CLK,
    GPIO_SDO

} gpio_name;

typedef enum
{
    GPIO_LOW,
    GPIO_HIGH

} gpio_val;

/*****************************************************************************/

bool_t
platform_init( Gpu_HalInit_t * );

void
platform_sleep_ms( uint32_t );

bool_t
platform_spi_init( Gpu_Hal_Context_t * );

void
platform_spi_deinit( Gpu_Hal_Context_t * );

uint8_t
platform_spi_send_recv_byte( Gpu_Hal_Context_t *, uint8_t, uint32_t );

uint16_t
platform_spi_send_data( Gpu_Hal_Context_t *, uint8_t *, uint16_t, uint32_t );

void
platform_spi_recv_data( Gpu_Hal_Context_t *, uint8_t *, uint16_t, uint32_t );

bool_t
platform_gpio_init( Gpu_Hal_Context_t *, gpio_name );

bool_t
platform_gpio_value( Gpu_Hal_Context_t *, gpio_name, gpio_val );

/*****************************************************************************/

#endif /*_PLATFORM_H_*/
