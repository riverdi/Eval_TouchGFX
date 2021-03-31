/*
 * bsp_ribus.h
 *
 *  Created on: Aug 30, 2020
 *      Author: milos
 */

#ifndef INC_BSP_RIBUS_H_
#define INC_BSP_RIBUS_H_

#include "App_Common.h"

typedef enum
{
    EVE_DISPLAY_UNKNOWN,
    EVE_DISPLAY_2_8,
    EVE_DISPLAY_3_5,
    EVE_DISPLAY_4_3,
    EVE_DISPLAY_5_0,
    EVE_DISPLAY_7_0,
    _EVE_DISPLAY_COUNT

} eve_display_size_t;

int
RiBUS_Query( eve_display_size_t display, int tp );

void
BSP_Ribus_DisplayInitialize( eve_display_size_t size, uint8_t cal );

void
BSP_Ribus_DisplayResolution( eve_display_size_t size, uint32_t * w,
                             uint32_t * h );

void
BSP_Ribus_SingleTest( void );

#endif /* INC_BSP_RIBUS_H_ */
