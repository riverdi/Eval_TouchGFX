/*
 * bsp_touch.c
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#include "bsp_touch.h"
#include "bsp_print.h"
#include "i2c.h"

/// Macros -------------------------------------------------------------------

#define RGB_TP_ADDRESS ( 0x38 << 1 )
#define RGB_TP_REG_OFFSET ( 0x03 )
#define RGB_TP_REG_REP ( 6 )
#define RGB_TP_REG_SIZE ( 4 )


#define IPS_TP_ADDRESS ( 0x41 << 1 )
#define IPS_TP_REG_OFFSET ( 0x10 )
#define IPS_TP_REG_REP ( 6 )
#define IPS_TP_REG_SIZE ( 64 )
/// Constants -----------------------------------------------------------------

static const uint16_t CAP_TOUCH_RESOLUTION[_CAP_TOUCH_COUNT][2] = {
    //  W    H
    { 0, 0 },                       //  NO TOUCH PLACETAKER
    { 0, 0 },                       //	CAP_TOUCH_RGB_2_8
    { 896, 640 },                   //	CAP_TOUCH_RGB_3_5
    { 1280, 768 },                  //	CAP_TOUCH_RGB_4_3
    { 800, 480 },                   //	CAP_TOUCH_RGB_5_0
    { 1024, 600 },                 //	CAP_TOUCH_RGB_7_0

    /*
     *  TODO:
     *  Add proper TP resolutions for IPS displays here.
     */

    { 0, 0 },                       //  CAP_TOUCH_IPS_3_5
    { 0, 0 },                       //  CAP_TOUCH_IPS_4_3
    { 0, 0 },                       //  CAP_TOUCH_IPS_5_0
    { 1024, 600 }                        //  CAP_TOUCH_IPS_7_0
};

/// Variables -----------------------------------------------------------------

static cap_touch_t ct;

/// Public Functions ----------------------------------------------------------

int
BSP_CapTouch_Initialize( cap_touch_t type )
{
    ct = type;

    switch ( ct )
    {
    case CAP_TOUCH_RGB_2_8:
    case CAP_TOUCH_RGB_3_5:
    case CAP_TOUCH_RGB_4_3:
    case CAP_TOUCH_RGB_5_0:
    case CAP_TOUCH_RGB_7_0:
    {
        uint8_t tx_test = 0;
        uint8_t tx_buf[2] = { 0x00, 0x00 };

        HAL_GPIO_WritePin( LCD_TPRST_GPIO_Port, LCD_TPRST_Pin, GPIO_PIN_SET );
        HAL_Delay( 100 );

        while ( HAL_OK !=
                HAL_I2C_Master_Transmit( &hi2c1, RGB_TP_ADDRESS, tx_buf, 2, 100 ) )
        {
            if ( ++tx_test > 5 )
            {
                return 1;
            }

            HAL_Delay( 50 );
        }

        break;
    }
    case CAP_TOUCH_IPS_3_5:
    case CAP_TOUCH_IPS_4_3:
    case CAP_TOUCH_IPS_5_0:
    case CAP_TOUCH_IPS_7_0:

        HAL_GPIO_WritePin( LCD_TPRST_GPIO_Port, LCD_TPRST_Pin, GPIO_PIN_SET );
        HAL_Delay( 100 );
        /*
         *  TODO:
         *  Implement initialization sequence for IPS TP controllers.
         */

        break;
    default:

        break;
    }

    return 0;
}

void
BSP_CapTouch_Resolution( uint32_t * w, uint32_t * h )
{
    *w = CAP_TOUCH_RESOLUTION[ct][0];
    *h = CAP_TOUCH_RESOLUTION[ct][1];
}

bool
BSP_CapTouch_Detected( void )
{
    switch ( ct )
    {
    case CAP_TOUCH_RGB_2_8:
    case CAP_TOUCH_RGB_3_5:
    case CAP_TOUCH_RGB_4_3:
    case CAP_TOUCH_RGB_5_0:
    case CAP_TOUCH_RGB_7_0:

        if ( !HAL_GPIO_ReadPin( LCD_TPINT_GPIO_Port, LCD_TPINT_Pin ) )
        {
            return true;
        }

        break;
    case CAP_TOUCH_IPS_3_5:
    case CAP_TOUCH_IPS_4_3:
    case CAP_TOUCH_IPS_5_0:
    case CAP_TOUCH_IPS_7_0:
        if ( !HAL_GPIO_ReadPin( LCD_TPINT_GPIO_Port, LCD_TPINT_Pin ) )
        {
            return true;
        }
        /*
         *  TODO:
         *  Implement check touch sequence for IPS TP controllers.
         */

        break;
    default:

        break;
    }

    return false;
}

int
BSP_CapTouch_Read( uint32_t * x, uint32_t * y )
{
    switch ( ct )
    {
    case CAP_TOUCH_RGB_2_8:
    case CAP_TOUCH_RGB_3_5:
    case CAP_TOUCH_RGB_4_3:
    case CAP_TOUCH_RGB_5_0:
    case CAP_TOUCH_RGB_7_0:
    {
        uint8_t tx_buf;
        uint8_t rx_buf[RGB_TP_REG_SIZE];

        tx_buf = RGB_TP_REG_OFFSET;

        if ( HAL_OK !=
             HAL_I2C_Master_Transmit( &hi2c1, RGB_TP_ADDRESS, &tx_buf, 1,
                                      1000 ) )
        {
            return 1;
        }

        if ( HAL_OK !=
             HAL_I2C_Master_Receive( &hi2c1, RGB_TP_ADDRESS, rx_buf,
                                     RGB_TP_REG_SIZE, 1000 ) )
        {
            return 2;
        }

        *x = 0;
        *x = rx_buf[0] & 0x0F;
        *x <<= 8;
        *x |= rx_buf[1];
        *y = 0;
        *y = rx_buf[2] & 0x0F;
        *y <<= 8;
        *y |= rx_buf[3];

        break;
    }
    case CAP_TOUCH_IPS_3_5:
    case CAP_TOUCH_IPS_4_3:
    case CAP_TOUCH_IPS_5_0:
    case CAP_TOUCH_IPS_7_0:
    {

    	        uint8_t rx_buf[IPS_TP_REG_SIZE] = {0};
    	       // uint8_t tx_buf[4];
    	       // tx_buf[0] = IPS_TP_REG_OFFSET;

    	        if ( HAL_OK !=  HAL_I2C_Mem_Read(&hi2c1, IPS_TP_ADDRESS, 0x10, 0, rx_buf, 64, 100))
    	        	{
    	        		return 1;
    	        	}
    	        if (rx_buf[0] == 0x48)
    	        {
        	        *x = 0;
        	        *x = rx_buf[3] & 0x0F;
        	        *x <<= 8;
        	        *x |= rx_buf[2];
        	        *y = 0;
        	        *y = rx_buf[5] & 0x0F;
        	        *y <<= 8;
        	        *y |= rx_buf[4];
    	        }
    	        else
    	        {
    	        	return 2;
    	        }


        /*
         *  TODO:
         *  Implement check touch sequence for IPS TP controllers.
         */

        break;
    }
    default:

        break;
    }

    return 0;
}

void
BSP_CapTouch_LoopTest( uint32_t delay )
{
    unsigned long start = HAL_GetTick( );

    while ( ( start + delay ) > HAL_GetTick( ) )
    // for ( ;; )	///	Block when testing.
    {
        if ( BSP_CapTouch_Detected( ) )
        {
            uint32_t X;
            uint32_t Y;

            if ( !BSP_CapTouch_Read( &X, &Y ) )
            {
                BSP_Print( "\r\n\tTouch ( %lu, %lu )", X, Y );
            }
        }
    }
}
