#ifndef _DEMO_INITIALIZER_
#define _DEMO_INITIALIZER_

#include "bsp_adc_touch.h"
#include "bsp_button.h"
#include "bsp_display.h"
#include "bsp_flash.h"
#include "bsp_led.h"
#include "bsp_print.h"
#include "bsp_ribus.h"
#include "bsp_sdcard.h"
#include "bsp_sdram.h"
#include "bsp_touch.h"

enum dev_type
{
    DEV_RGB_NO_TP,
    DEV_RGB_CAPACITIVE_TP,
    DEV_RGB_RESISTIVE_TP,
    DEV_IPS_NO_TP,
    DEV_IPS_CAPACITIVE_TP,
    _DEV_TYPE_COUNT
};

enum dev_size
{
    DEV_2_8,
    DEV_3_5,
    DEV_4_3,
    DEV_5_0,
    DEV_7_0,
    DEV_10_1,
    _DEV_SIZE_COUNT
};

extern enum dev_type device_type;
extern uint32_t d_w;
extern uint32_t d_h;
extern uint32_t t_w;
extern uint32_t t_h;

int
DEMO_DisplaySizeSelection( void );

void
DEMO_DisplayTypeSelection( void );

int
DEMO_RibusDrawEscape( void );

int
DEMO_RibusDisplayQuery( void );

void
DEMO_RestoreConfiguratiion( void );

void
DEMO_StoreConfiguratiion( void );

void
DEMO_Initialize( void );

void
DEMO_Run( void );

int
DEMO_DisplayBitmapFromSd( uint8_t idx );

int
DEMO_DisplayBitmapFromSd_Ribus( uint8_t idx );

void
DEMO_CapacitiveTouchDraw( void );

void
DEMO_ResistiveTouchDraw( void );

void
DEMO_RibusTouchDraw( void );

#endif
