/*
 * bsp_display.h
 *
 *  Created on: Sep 6, 2020
 *      Author: milos
 */

#ifndef INC_BSP_DISPLAY_H_
#define INC_BSP_DISPLAY_H_

#include <stdint.h>

typedef enum
{
    DISPLAY_UNKNOWN,
    DISPLAY_RGB_2_8,
    DISPLAY_RGB_3_5,
    DISPLAY_RGB_4_3,
    DISPLAY_RGB_5_0,
    DISPLAY_RGB_7_0,
    DISPLAY_IPS_3_5,
    DISPLAY_IPS_4_3,
    DISPLAY_IPS_5_0,
    DISPLAY_IPS_7_0,
    _DISPLAY_COUNT

} display_t;

void
BSP_Display_Initialize( display_t type );

uint32_t
BSP_Display_GetWidth( display_t type );

uint32_t
BSP_Display_GetHeight( display_t type );

void
BSP_Display_Resolution( uint32_t * w, uint32_t * h );

void
BSP_Display_Enable( void );

void
BSP_Display_SetBackgroung( uint8_t r, uint8_t g, uint8_t b );

void
BSP_Display_PaintBox( uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey,
                          uint32_t color );

void
BSP_Display_Pixel( uint32_t x, uint32_t y, uint32_t color );

void
BSP_Display_LoopTest( void );

#endif /* INC_BSP_DISPLAY_H_ */
