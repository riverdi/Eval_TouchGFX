/*
 * bsp_adc_touch.h
 *
 *  Created on: Sep 13, 2020
 *      Author: milos
 */

#ifndef INC_BSP_ADC_TOUCH_H_
#define INC_BSP_ADC_TOUCH_H_

#include <stdbool.h>
#include <stdint.h>

typedef void ( *draw_point_fp )( void );

typedef enum
{
    ROT0,
    ROT90,
    ROT180,
    ROT270

} rotation_t;

void
BSP_AdcTouch_Initialize( uint16_t width, uint16_t height, rotation_t rotate,
                         uint16_t z_thres );

void
BSP_AdcTouch_Calibrate( draw_point_fp tl, draw_point_fp br, draw_point_fp fin );

bool
BSP_AdcTouch_Point( uint16_t * x, uint16_t * y );

void
BSP_AdcTouch_LoopTest( uint32_t delay );

#endif /* INC_BSP_ADC_TOUCH_H_ */
