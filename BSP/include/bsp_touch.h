/*
 * bsp_touch.h
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#ifndef INC_BSP_CAP_TOUCH_H_
#define INC_BSP_CAP_TOUCH_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    CAP_TOUCH_UNKNOWN,
    CAP_TOUCH_RGB_2_8,
    CAP_TOUCH_RGB_3_5,
    CAP_TOUCH_RGB_4_3,
    CAP_TOUCH_RGB_5_0,
    CAP_TOUCH_RGB_7_0,
    CAP_TOUCH_IPS_3_5,
    CAP_TOUCH_IPS_4_3,
    CAP_TOUCH_IPS_5_0,
    CAP_TOUCH_IPS_7_0,
    _CAP_TOUCH_COUNT

} cap_touch_t;

int
BSP_CapTouch_Initialize( cap_touch_t type );

void
BSP_CapTouch_Resolution( uint32_t * w, uint32_t * h );

bool
BSP_CapTouch_Detected( void );

int
BSP_CapTouch_Read( uint32_t * x, uint32_t * y );

void
BSP_CapTouch_LoopTest( uint32_t delay );

#endif /* INC_BSP_CAP_TOUCH_H_ */

