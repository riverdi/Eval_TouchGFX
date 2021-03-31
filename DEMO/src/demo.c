/*
 * demo_initializer.c
 *
 *  Created on: Sep 20, 2020
 *      Author: milos
 */

#include "App_Common.h"
#include "bsp_ribus.h"
#include "fatfs.h"
#include "platform.h"
#include "stm32f4xx_hal.h"
#include <demo.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum fsm
{
    FSM_RIVERDI_START_LOGO_TRANSITION,
    FSM_RIVERDI_START_LOGO,
    FSM_R_SCREEN_TRANSITION,
    FSM_R_SCREEN,
    FSM_G_SCREEN_TRANSITION,
    FSM_G_SCREEN,
    FSM_B_SCREEN_TRANSITION,
    FSM_B_SCREEN,
    FSM_WH_SCREEN_TRANSITION,
    FSM_WH_SCREEN,
    FSM_BL_SCREEN_TRANSITION,
    FSM_BL_SCREEN,
    FSM_IMAGE_FROM_SD_1_TRANSITION,
    FSM_IMAGE_FROM_SD_1,
    FSM_IMAGE_FROM_SD_2_TRANSITION,
    FSM_IMAGE_FROM_SD_2,
    FSM_IMAGE_FROM_SD_3_TRANSITION,
    FSM_IMAGE_FROM_SD_3,
    FSM_DRAWING_TRANSITION,
    FSM_DRAWING,
    // FSM_RIVERDI_END_LOGO_TRANSITION,
    // FSM_RIVERDI_END_LOGO,
    _FSM_STATE_COUNT
};

static const char * TYPE_STRING[_DEV_TYPE_COUNT] = { "RGB - No TP",
    "RGB - Cap TP", "RGB - Res TP", "IPS - No TP", "IPS - Cap TP" };

static const char * SIZE_STRING[_DEV_SIZE_COUNT] = { "2.8", "3.5", "4.3", "5.0",
    "7.0", "10.1"
};

enum fsm current_state;
enum dev_type device_type;
enum dev_size device_size;
uint8_t ribus;

uint32_t d_w;
uint32_t d_h;
uint32_t t_w;
uint32_t t_h;

// -------------------------------------------------------------------

static void
_go_to_next_check( uint32_t period );

static void
_go_to_prev_check( uint32_t period );

static void
_topleft_draw( void );

static void
_bottomright_draw( void );

static void
_finish_draw( void );

// -------------------------------------------------------------------

int
DEMO_DisplaySizeSelection( void )
{
    enum dev_size tmp_size = DEV_7_0;

    //	If no press - selection is skipped.
    if ( !BSP_Button1_is_pressedDebounce( 100 ) )
    {
        BSP_Print( "\r\n[DEMO INIT] Display selection skipped" );
        return 1;
    }

    //	We will be in loop while button is released.
    while ( BSP_Button1_is_pressedDebounce( 10 ) )
    {
        BSP_LED1_on( );
        //	Detect press longer then 100ms.
        if ( BSP_Button2_is_pressedDebounce( 100 ) )
        {
            BSP_LED2_on( );
            while ( BSP_Button2_is_pressedDebounce( 10 ) )
                ;    //	Block until button is released.
            BSP_LED2_off( );
            ++tmp_size;
        }
    }

    BSP_LED1_off( );
    tmp_size %= _DEV_SIZE_COUNT;
    BSP_Print( "\r\n[DEMO INIT] Display size: %s", SIZE_STRING[tmp_size] );
    device_size = tmp_size;

    return 0;
}

void
DEMO_DisplayTypeSelection( void )
{
    enum dev_type tmp_dev = DEV_IPS_NO_TP;

    //	Just block until initial press is detected.
    while ( !BSP_Button1_is_pressedDebounce( 10 ) )
        ;

    //	We will be in loop while button is released.
    while ( BSP_Button1_is_pressedDebounce( 10 ) )
    {
        BSP_LED1_on( );

        //	Detect press longer then 100ms.
        if ( BSP_Button2_is_pressedDebounce( 100 ) )
        {
            BSP_LED2_on( );
            while ( BSP_Button2_is_pressedDebounce( 10 ) )
                ;    //	Block until button is released.
            BSP_LED2_off( );
            ++tmp_dev;
        }
    }

    BSP_LED1_off( );
    tmp_dev %= _DEV_TYPE_COUNT;
    BSP_Print( "\r\n[DEMO INIT] Display type: %s", TYPE_STRING[tmp_dev] );
    device_type = tmp_dev;
}

int
DEMO_RibusDisplayQuery( void )
{
    return ( ribus = RiBUS_Query( device_size, device_type ) );
}

int
DEMO_RibusDrawEscape( void )
{
    if ( BSP_Button1_is_pressedDebounce( 1 ) )
    {
        current_state -= 3;
        current_state %= _FSM_STATE_COUNT;
        return 1;
    }

    if ( BSP_Button2_is_pressedDebounce( 1 ) )
    {
        ++current_state;
        current_state %= _FSM_STATE_COUNT;
        return 1;
    }

    return 0;
}

void
DEMO_RestoreConfiguratiion( void )
{
    uint32_t var;
    uint8_t tmp[12];

    BSP_Flash_Read_Data_Q( 0x00000000, tmp, 12 );

    var = tmp[0];
    var <<= 8;
    var |= tmp[1];
    var <<= 8;
    var |= tmp[2];
    var <<= 8;
    var |= tmp[3];
    device_size = var;

    var = tmp[4];
    var <<= 8;
    var |= tmp[5];
    var <<= 8;
    var |= tmp[6];
    var <<= 8;
    var |= tmp[7];
    device_type = var;

    var = tmp[8];
    var <<= 8;
    var |= tmp[9];
    var <<= 8;
    var |= tmp[10];
    var <<= 8;
    var |= tmp[11];
    ribus = var;
    ribus = 0;
    device_type = DEV_IPS_CAPACITIVE_TP;
   // device_type =DEV_RGB_NO_TP;
    device_size = DEV_7_0;
    BSP_Print( "\r\n[DEMO INIT] Display Size: %s", SIZE_STRING[device_size] );
    BSP_Print( "\r\n[DEMO INIT] Display Type: %s", TYPE_STRING[device_type] );
}

void
DEMO_StoreConfiguratiion( void )
{
    uint32_t var;
    uint8_t tmp[12];

    var = device_size;
    tmp[0] = var >> 24;
    tmp[1] = ( var & 0x00FF0000 ) >> 16;
    tmp[2] = ( var & 0x0000FF00 ) >> 8;
    tmp[3] = var & 0x000000FF;

    var = device_type;
    tmp[4] = var >> 24;
    tmp[5] = ( var & 0x00FF0000 ) >> 16;
    tmp[6] = ( var & 0x0000FF00 ) >> 8;
    tmp[7] = var & 0x000000FF;

    var = ribus;
    tmp[8] = var >> 24;
    tmp[9] = ( var & 0x00FF0000 ) >> 16;
    tmp[10] = ( var & 0x0000FF00 ) >> 8;
    tmp[11] = var & 0x000000FF;

    BSP_Flash_Sector_Erase( 0 );
    BSP_Flash_Write_Data_Q( 0x00000000, tmp, 12 );
}

void
DEMO_Initialize( void )
{
    if ( BSP_SD_Card_Init( ) )
    {
        for ( ;; )
            ;
    }
    device_type = DEV_IPS_CAPACITIVE_TP;
      // device_type =DEV_RGB_NO_TP;
       device_size = DEV_7_0;
    if ( ribus )
    {
        switch ( device_size )
        {
        case DEV_2_8:

            BSP_Ribus_DisplayInitialize( EVE_DISPLAY_2_8, 1 );
            BSP_Ribus_DisplayResolution( EVE_DISPLAY_2_8, &d_w, &d_h );

            break;
        case DEV_3_5:

            BSP_Ribus_DisplayInitialize( EVE_DISPLAY_3_5, 1 );
            BSP_Ribus_DisplayResolution( EVE_DISPLAY_3_5, &d_w, &d_h );

            break;
        case DEV_4_3:

            BSP_Ribus_DisplayInitialize( EVE_DISPLAY_4_3, 1 );
            BSP_Ribus_DisplayResolution( EVE_DISPLAY_4_3, &d_w, &d_h );

            break;
        case DEV_5_0:

            BSP_Ribus_DisplayInitialize( EVE_DISPLAY_5_0, 1 );
            BSP_Ribus_DisplayResolution( EVE_DISPLAY_5_0, &d_w, &d_h );

            break;
        case DEV_7_0:

            BSP_Ribus_DisplayInitialize( EVE_DISPLAY_7_0, 0 );
            BSP_Ribus_DisplayResolution( EVE_DISPLAY_7_0, &d_w, &d_h );

            break;
        case DEV_10_1:
        case _DEV_SIZE_COUNT:
        default: break;
        }
    }
    else
    {
        switch ( device_type )
        {
        case DEV_RGB_CAPACITIVE_TP:

            switch ( device_size )
            {
            case DEV_2_8:

                BSP_Display_Initialize( DISPLAY_RGB_2_8 );
                BSP_CapTouch_Initialize( CAP_TOUCH_RGB_2_8 );

                break;
            case DEV_3_5:

                BSP_Display_Initialize( DISPLAY_RGB_3_5 );
                BSP_CapTouch_Initialize( CAP_TOUCH_RGB_3_5 );

                break;
            case DEV_4_3:

                BSP_Display_Initialize( DISPLAY_RGB_4_3 );
                BSP_CapTouch_Initialize( CAP_TOUCH_RGB_4_3 );

                break;
            case DEV_5_0:

                BSP_Display_Initialize( DISPLAY_RGB_5_0 );
                BSP_CapTouch_Initialize( CAP_TOUCH_RGB_5_0 );

                break;
            case DEV_7_0:

                BSP_Display_Initialize( DISPLAY_RGB_7_0 );
                BSP_CapTouch_Initialize( CAP_TOUCH_RGB_7_0 );

                break;
            case DEV_10_1:
            case _DEV_SIZE_COUNT:
            default: break;
            }

            BSP_Display_Resolution( &d_w, &d_h );
            BSP_CapTouch_Resolution( &t_w, &t_h );

            break;
        case DEV_RGB_RESISTIVE_TP:

            switch ( device_size )
            {
            case DEV_2_8:

                BSP_Display_Initialize( DISPLAY_RGB_2_8 );

                break;
            case DEV_3_5:

                BSP_Display_Initialize( DISPLAY_RGB_3_5 );

                break;
            case DEV_4_3:

                BSP_Display_Initialize( DISPLAY_RGB_4_3 );

                break;
            case DEV_5_0:

                BSP_Display_Initialize( DISPLAY_RGB_5_0 );

                break;
            case DEV_7_0:

               BSP_Display_Initialize( DISPLAY_RGB_7_0 );

                break;
            case DEV_10_1:
            case _DEV_SIZE_COUNT:
            default: break;
            }

            BSP_Display_Resolution( &d_w, &d_h );
            BSP_AdcTouch_Initialize( d_w, d_h, ROT0, 300 );
            BSP_AdcTouch_Calibrate( _topleft_draw, _bottomright_draw,
                                    _finish_draw );

            break;
        case DEV_IPS_CAPACITIVE_TP:

            switch ( device_size )
            {
            case DEV_3_5:

                BSP_Display_Initialize( DISPLAY_IPS_3_5 );
                BSP_CapTouch_Initialize( CAP_TOUCH_IPS_3_5 );

                break;
            case DEV_4_3:

                BSP_Display_Initialize( DISPLAY_IPS_4_3 );
                BSP_CapTouch_Initialize( CAP_TOUCH_IPS_4_3 );

                break;
            case DEV_5_0:

                BSP_Display_Initialize( DISPLAY_IPS_5_0 );
                BSP_CapTouch_Initialize( CAP_TOUCH_IPS_5_0 );

                break;
            case DEV_7_0:

                BSP_Display_Initialize( DISPLAY_IPS_7_0 );
                BSP_CapTouch_Initialize( CAP_TOUCH_IPS_7_0 );

                break;
            case DEV_2_8:
            case DEV_10_1:
            case _DEV_SIZE_COUNT:
            default: break;
            }

            BSP_Display_Resolution( &d_w, &d_h );
            BSP_CapTouch_Resolution( &t_w, &t_h );

            break;
        case DEV_IPS_NO_TP:

            switch ( device_size )
            {
            case DEV_3_5:

                BSP_Display_Initialize( DISPLAY_IPS_3_5 );

                break;
            case DEV_4_3:

                BSP_Display_Initialize( DISPLAY_IPS_4_3 );

                break;
            case DEV_5_0:

                BSP_Display_Initialize( DISPLAY_IPS_5_0 );

                break;
            case DEV_7_0:

                BSP_Display_Initialize( DISPLAY_IPS_7_0 );

                break;
            case DEV_2_8:
            case DEV_10_1:
            case _DEV_SIZE_COUNT:
            default: break;
            }

            BSP_Display_Resolution( &d_w, &d_h );

            break;
        case DEV_RGB_NO_TP:

            switch ( device_size )
            {
            case DEV_2_8:

                BSP_Display_Initialize( DISPLAY_RGB_2_8 );

                break;
            case DEV_3_5:

                BSP_Display_Initialize( DISPLAY_RGB_3_5 );

                break;
            case DEV_4_3:

                BSP_Display_Initialize( DISPLAY_RGB_4_3 );

                break;
            case DEV_5_0:

                BSP_Display_Initialize( DISPLAY_RGB_5_0 );

                break;
            case DEV_7_0:

               BSP_Display_Initialize( DISPLAY_RGB_7_0 );

                break;
            case DEV_10_1:
            case _DEV_SIZE_COUNT:
            default: break;
            }

            BSP_Display_Resolution( &d_w, &d_h );

            break;
        case _DEV_TYPE_COUNT:
        default: break;
        }
    }

    BSP_Display_Enable( );
}

void
DEMO_Run( void )
{
    uint32_t heartbeat = HAL_GetTick( ) % 1000;

    if ( ( heartbeat > 475 ) && ( heartbeat < 500 ) )
    {
        BSP_LED1_on( );
    }
    else
    {
        BSP_LED1_off( );
    }

    if ( ( heartbeat > 650 ) && ( heartbeat < 750 ) )
    {
        BSP_LED2_on( );
    }
    else
    {
        BSP_LED2_off( );
    }
    current_state = FSM_IMAGE_FROM_SD_1_TRANSITION;
    switch ( current_state )
    {
    case FSM_RIVERDI_START_LOGO:
    case FSM_R_SCREEN:
    case FSM_G_SCREEN:
    case FSM_B_SCREEN:
    case FSM_WH_SCREEN:
    case FSM_BL_SCREEN:
    case FSM_IMAGE_FROM_SD_1:
    case FSM_IMAGE_FROM_SD_2:
    case FSM_IMAGE_FROM_SD_3:
        //	case FSM_RIVERDI_END_LOGO:

        /*
         * Fixed image is presented during this states - only
         * necessary to check the request to switch the state.
         */
        _go_to_next_check( 2 );
        _go_to_prev_check( 2 );

        break;
    case FSM_RIVERDI_START_LOGO_TRANSITION:

        BSP_Print( "\r\n[DEMO RUN] Go To Riverdi Start Logo" );
        current_state = FSM_RIVERDI_START_LOGO;

        if ( ribus )
        {
            if ( DEMO_DisplayBitmapFromSd_Ribus( 0 ) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
                current_state = FSM_R_SCREEN_TRANSITION;
            }
        }
        else
        {
           // if ( DEMO_DisplayBitmapFromSd( 0 ) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load logo" );
                current_state = FSM_R_SCREEN_TRANSITION;
            }
        }

        break;
    case FSM_R_SCREEN_TRANSITION:

        if ( ribus )
        {
            App_Set_Background( &host, 255, 0, 0 );
        }
        else
        {
            BSP_Display_SetBackgroung( 255, 0, 0 );
        }

        BSP_Print( "\r\n[DEMO RUN] Go To R Screen" );
        current_state = FSM_R_SCREEN;
        break;
    case FSM_G_SCREEN_TRANSITION:

        if ( ribus )
        {
            App_Set_Background( &host, 0, 255, 0 );
        }
        else
        {
            BSP_Display_SetBackgroung( 0, 255, 0 );
        }
        BSP_Print( "\r\n[DEMO RUN] Go To G Screen" );
        current_state = FSM_G_SCREEN;
        break;
    case FSM_B_SCREEN_TRANSITION:

        if ( ribus )
        {
            App_Set_Background( &host, 0, 0, 255 );
        }
        else
        {
            BSP_Display_SetBackgroung( 0, 0, 255 );
        }
        BSP_Print( "\r\n[DEMO RUN] Go To B Screen" );
        current_state = FSM_B_SCREEN;
        break;
    case FSM_WH_SCREEN_TRANSITION:

        if ( ribus )
        {
            App_Set_Background( &host, 255, 255, 255 );
        }
        else
        {
            BSP_Display_SetBackgroung( 255, 255, 255 );
        }
        BSP_Print( "\r\n[DEMO RUN] Go To WH Screen" );
        current_state = FSM_WH_SCREEN;
        break;
    case FSM_BL_SCREEN_TRANSITION:

        if ( ribus )
        {
            App_Set_Background( &host, 0, 0, 0 );
        }
        else
        {
            BSP_Display_SetBackgroung( 0, 0, 0 );
        }
        BSP_Print( "\r\n[DEMO RUN] Go To BL Screen" );
        current_state = FSM_BL_SCREEN;
        break;
    case FSM_IMAGE_FROM_SD_1_TRANSITION:

        if ( ribus )
        {
            if ( DEMO_DisplayBitmapFromSd_Ribus( 1) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
                current_state = FSM_IMAGE_FROM_SD_2_TRANSITION;
            }
        }
        else
        {
            if ( DEMO_DisplayBitmapFromSd( 1 ) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
                current_state = FSM_IMAGE_FROM_SD_2_TRANSITION;
            }
        }

        BSP_Print( "\r\n[DEMO RUN] Go To Image 1" );
        current_state = FSM_IMAGE_FROM_SD_1;
        break;
    case FSM_IMAGE_FROM_SD_2_TRANSITION:

        if ( ribus )
        {
            if ( DEMO_DisplayBitmapFromSd_Ribus( 2 ) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
                current_state = FSM_IMAGE_FROM_SD_2_TRANSITION;
            }
        }
        else
        {
            //if ( DEMO_DisplayBitmapFromSd( 2 ) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
                current_state = FSM_IMAGE_FROM_SD_3_TRANSITION;
            }
        }
        BSP_Print( "\r\n[DEMO RUN] Go To Image 2" );
        current_state = FSM_IMAGE_FROM_SD_2;
        break;
    case FSM_IMAGE_FROM_SD_3_TRANSITION:

        if ( ribus )
        {
            if ( DEMO_DisplayBitmapFromSd_Ribus( 3 ) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
                current_state = FSM_IMAGE_FROM_SD_2_TRANSITION;
            }
        }
        else
        {
           // if ( DEMO_DisplayBitmapFromSd( 3 ) )
            {
                BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
                current_state = FSM_DRAWING_TRANSITION;
            }
        }
        BSP_Print( "\r\n[DEMO RUN] Go To Image 3" );
        current_state = FSM_IMAGE_FROM_SD_3;
        break;
    case FSM_DRAWING_TRANSITION:

        //	Skip Drawing in case of no TP for obvious reason.
        if ( ( DEV_RGB_NO_TP == device_type ) ||
             ( DEV_IPS_NO_TP == device_type ) )
        {
            current_state = FSM_RIVERDI_START_LOGO_TRANSITION;
        }
        else
        {
            if ( ribus )
            {
            }
            else
            {
                BSP_Display_SetBackgroung( 255, 255, 255 );
                HAL_Delay( 1000 );
            }

            BSP_Print( "\r\n[DEMO RUN] Go To Drawing" );
            current_state = FSM_DRAWING;
        }

        break;
    case FSM_DRAWING:

        if ( ribus )
        {
            DEMO_RibusTouchDraw( );
            current_state = FSM_RIVERDI_START_LOGO_TRANSITION;

            switch ( device_size )
            {
            case DEV_2_8:

                BSP_Ribus_DisplayInitialize( EVE_DISPLAY_2_8, 1 );
                BSP_Ribus_DisplayResolution( EVE_DISPLAY_2_8, &d_w, &d_h );

                break;
            case DEV_3_5:

                BSP_Ribus_DisplayInitialize( EVE_DISPLAY_3_5, 1 );
                BSP_Ribus_DisplayResolution( EVE_DISPLAY_3_5, &d_w, &d_h );

                break;
            case DEV_4_3:

                BSP_Ribus_DisplayInitialize( EVE_DISPLAY_4_3, 1 );
                BSP_Ribus_DisplayResolution( EVE_DISPLAY_4_3, &d_w, &d_h );

                break;
            case DEV_5_0:

                BSP_Ribus_DisplayInitialize( EVE_DISPLAY_5_0, 1 );
                BSP_Ribus_DisplayResolution( EVE_DISPLAY_5_0, &d_w, &d_h );

                break;
            case DEV_7_0:

                BSP_Ribus_DisplayInitialize( EVE_DISPLAY_7_0, 1 );
                BSP_Ribus_DisplayResolution( EVE_DISPLAY_7_0, &d_w, &d_h );

                break;
            case DEV_10_1:
            case _DEV_SIZE_COUNT:
            default: break;
            }
        }
        else
        {

            switch ( device_type )
            {
            case DEV_RGB_CAPACITIVE_TP:
            case DEV_IPS_CAPACITIVE_TP:

                DEMO_CapacitiveTouchDraw( );

                break;
            case DEV_RGB_RESISTIVE_TP:

                DEMO_ResistiveTouchDraw( );

                break;
            case DEV_RGB_NO_TP:
            case DEV_IPS_NO_TP:
            case _DEV_TYPE_COUNT:
            default:

                break;
            }

            _go_to_next_check( 1 );
            _go_to_prev_check( 1 );
        }

        break;
#if 0
	case FSM_RIVERDI_END_LOGO_TRANSITION:

		if ( ribus )
		{
			if ( DEMO_DisplayBitmapFromSd_Ribus( 0 ) )
			{
				BSP_Print( "\r\n[DEMO RUN] Failed to load image" );
				current_state = FSM_IMAGE_FROM_SD_2_TRANSITION;
			}
		}
		else
		{
			if ( DEMO_DisplayBitmapFromSd( 0 ) )
			{
				BSP_Print( "\r\n[DEMO RUN] Failed to load logo" );
				current_state = FSM_R_SCREEN_TRANSITION;
			}
		}

		BSP_Print( "\r\n[DEMO RUN] Go To Riverdi End Logo" );
		current_state = FSM_RIVERDI_END_LOGO;
		break;
#endif
    case _FSM_STATE_COUNT:
    default:

        BSP_Print( "\r\n[DEMO RUN] Go To Riverdi Start Logo" );
        current_state = FSM_RIVERDI_START_LOGO_TRANSITION;
        break;
    }
}

static void
_go_to_next_check( uint32_t period )
{
    if ( BSP_Button2_is_pressedDebounce( period ) )
    {
        ++current_state;
        current_state %= _FSM_STATE_COUNT;
    }
}

static void
_go_to_prev_check( uint32_t period )
{
    //	Go back to previous is same for all screens.
    if ( BSP_Button1_is_pressedDebounce( period ) )
    {
        current_state -= 3;
        current_state %= _FSM_STATE_COUNT;
    }
}

static void
_topleft_draw( void )
{
    BSP_Display_SetBackgroung( 255, 255, 255 );
    BSP_Display_PaintBox( 2, 2, 24, 6, 0x00FF0000 );
    BSP_Display_PaintBox( 2, 2, 6, 24, 0x00FF0000 );
}

static void
_bottomright_draw( void )
{
    BSP_Display_PaintBox( 2, 2, 24, 6, 0x00FFFFFF );
    BSP_Display_PaintBox( 2, 2, 6, 24, 0x00FFFFFF );
    BSP_Display_PaintBox( d_w - 24, d_h - 6, d_w - 2, d_h - 2, 0x00FF0000 );
    BSP_Display_PaintBox( d_w - 6, d_h - 24, d_w - 2, d_h - 2, 0x00FF0000 );
}

static void
_finish_draw( void )
{
    BSP_Display_PaintBox( d_w - 24, d_h - 6, d_w - 2, d_h - 2, 0x00FFFFFF );
    BSP_Display_PaintBox( d_w - 6, d_h - 24, d_w - 2, d_h - 2, 0x00FFFFFF );
}
