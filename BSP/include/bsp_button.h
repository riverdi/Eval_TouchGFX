/*
 * bsp_button.h
 *
 *  Created on: Aug 25, 2020
 *      Author: milos
 */

#ifndef INC_BSP_BUTTON_H_
#define INC_BSP_BUTTON_H_

#include <stdbool.h>
#include <stdint.h>

bool
BSP_Button1_is_pressed( void );

bool
BSP_Button1_is_pressedDebounce( uint32_t debounce );

bool
BSP_Button2_is_pressed( void );

bool
BSP_Button2_is_pressedDebounce( uint32_t debounce );

void
BSP_Button_LoopTest( void );

#endif /* INC_BSP_BUTTON_H_ */
