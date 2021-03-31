/*
 * bsp_adc_touch.c
 *
 *  Created on: Sep 13, 2020
 *      Author: milos
 */

#include "bsp_adc_touch.h"
#include "bsp_print.h"
#include "main.h"
#include "spi.h"

//	Comment out to disable debug messages
//#define DBG(x, ...) 	BSP_Print( ( x ), ##__VA_ARGS__ )

//	More samples mean better precision + slower readout
#define READ_CALIB_SAMPLES ( 32 )

#ifndef DBG
#define DBG( x, ... )
#endif

static rotation_t rot;
static uint16_t w;
static uint16_t h;

static uint16_t z_thres;
static uint16_t cal_z_off;
static uint16_t cal_x_off;
static uint16_t cal_y_off;
static uint16_t cal_x_range;
static uint16_t cal_y_range;

//	Functions to provide info to user during calibration sequence.

static draw_point_fp top_left_drawer;
static draw_point_fp bottom_right_drawer;
static draw_point_fp finish_drawer;

static uint16_t
_TSC2046_Comm( uint8_t cmd )
{
    uint8_t tx_buf[3];
    uint8_t rx_buf[3];
    uint16_t ret = 0;

    tx_buf[0] = cmd;

    HAL_GPIO_WritePin( TP_SPI_CS_GPIO_Port, TP_SPI_CS_Pin, GPIO_PIN_RESET );
    HAL_SPI_TransmitReceive( &hspi1, tx_buf, rx_buf, 3, 100 );
    HAL_GPIO_WritePin( TP_SPI_CS_GPIO_Port, TP_SPI_CS_Pin, GPIO_PIN_SET );

    ret = rx_buf[1] & 0x7F;
    ret <<= 5;
    ret |= rx_buf[2] >> 3;

    return ret;
}

static inline uint16_t
_TSC2046_GetRaw_X( void )
{
    return _TSC2046_Comm( 0xD1 );
}

static inline uint16_t
_TSC2046_GetRaw_Y( void )
{
    return _TSC2046_Comm( 0x91 );
}

static inline uint16_t
_TSC2046_GetRaw_Z( void )
{
    uint16_t z1 = _TSC2046_Comm( 0xB1 );
    uint16_t z2 = ( 4095 - _TSC2046_Comm( 0xC1 ) );

    /*
        TODO:
        Improve calculation here. We are now based only on z1.
     */
    //BSP_Print( "\r\n\t%d - %d", z1, z2 );

    return z1 > z2 ? z1 : z2;
}

static inline void
_TSC2046_PowerDown( void )
{
    _TSC2046_Comm( 0xD0 );
}

static bool
_TSC2046_IsTouched( void )
{
    uint16_t z = _TSC2046_GetRaw_Z( ) - cal_z_off;

    /*
        TODO:
        Improve calculation here.
        We are now based only on z1.
        In relation to get raw z.
     */

    if ( ( z > z_thres ) && ( z < 4096 ) )
    {
        return true;
    }

    return false;
}

static void
_TSC2046_GetRaw_Data( uint16_t * x, uint16_t * y )
{
    switch ( rot )
    {
    case ROT0:
        *x = 4095 - _TSC2046_GetRaw_X( );
        *y = 4095 - _TSC2046_GetRaw_Y( );
        break;
    case ROT90:
        *x = 4095 - _TSC2046_GetRaw_Y( );
        *y = _TSC2046_GetRaw_X( );
        break;
    case ROT180:
        *x = _TSC2046_GetRaw_X( );
        *y = _TSC2046_GetRaw_Y( );
        break;
    case ROT270:
        *x = _TSC2046_GetRaw_Y( );
        *y = 4095 - _TSC2046_GetRaw_X( );
        break;
    default: return;
    }
}

void
BSP_AdcTouch_Initialize( uint16_t width, uint16_t height, rotation_t rotate,
                         uint16_t z_threshold )
{
    uint32_t tmp_z = 0;

    rot = rotate;
    w = width;
    h = height;
    z_thres = z_threshold;

    BSP_Print( "\r\nADC Touch - W: %d, H: %d, R: %d", w, h, rot );

    for ( uint8_t i = 0; i < READ_CALIB_SAMPLES; i++ )
    {
        tmp_z += _TSC2046_GetRaw_Z( );
        ( void )_TSC2046_GetRaw_X( );
        ( void )_TSC2046_GetRaw_Y( );
        HAL_Delay( 10 );
    }

    cal_z_off = tmp_z / READ_CALIB_SAMPLES;
    DBG( "\r\n Z Offset :%d", cal_z_off );

    cal_x_off = 0;
    cal_y_off = 0;
    cal_x_range = 0;
    cal_y_range = 0;
}

void
BSP_AdcTouch_Calibrate( draw_point_fp tl, draw_point_fp br, draw_point_fp fin )
{
    uint8_t i = 0;
    uint32_t TL_X = 0;
    uint32_t TL_Y = 0;
    uint32_t BR_X = 0;
    uint32_t BR_Y = 0;

    top_left_drawer = tl;
    bottom_right_drawer = br;
    finish_drawer = fin;

    //	Get Top-Left corner.
    top_left_drawer( );
    for ( ;; )
    {
        if ( _TSC2046_IsTouched( ) )
        {
            uint16_t tmp_x;
            uint16_t tmp_y;

            if ( ++i > READ_CALIB_SAMPLES )
            {
                break;
            }

            _TSC2046_GetRaw_Data( &tmp_x, &tmp_y );
            TL_X += tmp_x;
            TL_Y += tmp_y;
        }
        HAL_Delay( 10 );
    }

    bottom_right_drawer( );
    HAL_Delay( 2000 );
    i = 0;

    //	Get Bottom-Right corner.
    for ( ;; )
    {
        if ( _TSC2046_IsTouched( ) )
        {
            uint16_t tmp_x;
            uint16_t tmp_y;

            if ( ++i > READ_CALIB_SAMPLES )
            {
                break;
            }

            _TSC2046_GetRaw_Data( &tmp_x, &tmp_y );
            BR_X += tmp_x;
            BR_Y += tmp_y;
        }
        HAL_Delay( 10 );
    }

    finish_drawer( );

    cal_x_off = TL_X / READ_CALIB_SAMPLES;
    cal_y_off = TL_Y / READ_CALIB_SAMPLES;
    cal_x_range = ( BR_X / READ_CALIB_SAMPLES ) - cal_x_off;
    cal_y_range = ( BR_Y / READ_CALIB_SAMPLES ) - cal_y_off;

    DBG( "\r\n\t X off :%d", cal_x_off );
    DBG( "\r\n\t Y off :%d", cal_y_off );
    DBG( "\r\n\t X range :%d", cal_x_range );
    DBG( "\r\n\t Y range :%d", cal_y_range );
    HAL_Delay( 2000 );
}

bool
BSP_AdcTouch_Point( uint16_t * x, uint16_t * y )
{
    uint16_t tmp_x = 0;
    uint16_t tmp_y = 0;
    uint32_t sum_x = 0;
    uint32_t sum_y = 0;
    int32_t calc_x = 0;
    int32_t calc_y = 0;

    //	Is screen not pressed.
    if ( !_TSC2046_IsTouched( ) )
    {
        return false;
    }

    //	Read touch data.
    for ( uint8_t i = 0; i < READ_CALIB_SAMPLES; ++i )
    {
        _TSC2046_GetRaw_Data( &tmp_x, &tmp_y );
        sum_x += tmp_x;
        sum_y += tmp_y;
    }

    //	Is screen not pressed.
    if ( !_TSC2046_IsTouched( ) )
    {
        return false;
    }

    tmp_x = sum_x / READ_CALIB_SAMPLES;
    tmp_y = sum_y / READ_CALIB_SAMPLES;
    //DBG( "\r\n\t X :%d | Y: %d", tmp_x, tmp_y );

    // 	[WIDTH/HEIGHT] : [ADC range] = [?] : [( ADC read - ADCoff )]
    calc_x = ( w * ( tmp_x - cal_x_off ) ) / cal_x_range;
    calc_y = ( h * ( tmp_y - cal_y_off ) ) / cal_y_range;

    //	Cut off values out of the bound.
    if ( calc_x < 0 )
    {
        calc_x = 0;
    }
    if ( calc_x > w )
    {
        calc_x = w;
    }
    if ( calc_y < 0 )
    {
        calc_y = 0;
    }
    if ( calc_y > h )
    {
        calc_y = h;
    }

    *x = calc_x;
    *y = calc_y;

    return true;
}

void
BSP_AdcTouch_LoopTest( uint32_t delay )
{
    // unsigned long start = HAL_GetTick( );
    // while ( ( start + delay ) > HAL_GetTick( ) )
    for ( ;; )    ///	Block when testing.
    {
        uint16_t X, Y;

        if ( BSP_AdcTouch_Point( &X, &Y ) )
        {
            BSP_Print( "\r\n\t%d\t%d", X, Y );
        }
    }
}
