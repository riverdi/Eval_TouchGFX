/*
 * bsp_ribus.c
 *
 *  Created on: Aug 30, 2020
 *      Author: milos
 */

#include "bsp_ribus.h"
#include "App_Common.h"
#include "Gpu.h"
#include "bsp_print.h"
#include "demo.h"
#include "platform.h"

static const Gpu_Display_Config_t EVE_DISPLAY_CONFIG[_EVE_DISPLAY_COUNT] = {
    //	UNKNOWN - Generic values
    { .module = EVE_UNKNOWN,
      .ft80x_enable = 0,
      .ft81x_enable = 0,
      .bt81x_enable = 0,
      .DispWidth = 0,
      .DispHeight = 0,
      .DispHCycle = 0,
      .DispHOffset = 0,
      .DispHSync0 = 0,
      .DispHSync1 = 0,
      .DispVCycle = 0,
      .DispVOffset = 0,
      .DispVSync0 = 0,
      .DispVSync1 = 0,
      .DispPCLK = 0,
      .DispSwizzle = 0,
      .DispPCLKPol = 0,
      .DispCSpread = 0,
      .DispDither = 0 },
    //	DISPLAY_2_8
    { .module = EVE_UNKNOWN,
      .ft81x_enable = 0,
      .ft81x_enable = 0,
      .bt81x_enable = 0,
      .DispWidth = 240,
      .DispHeight = 320,
      .DispHCycle = 270,
      .DispHOffset = 20,
      .DispHSync0 = 0,
      .DispHSync1 = 10,
      .DispVCycle = 326,
      .DispVOffset = 2,
      .DispVSync0 = 0,
      .DispVSync1 = 2,
      .DispPCLK = 5,
      .DispSwizzle = 4,
      .DispPCLKPol = 0,
      .DispCSpread = 0,
      .DispDither = 0 },
    //	DISPLAY_3_5
    { .module = EVE_3,
      .ft80x_enable = 0,
      .ft81x_enable = 1,
      .bt81x_enable = 1,
      .DispWidth = 320,
      .DispHeight = 240,
      .DispHCycle = 408,
      .DispHOffset = 70,
      .DispHSync0 = 0,
      .DispHSync1 = 10,
      .DispVCycle = 263,
      .DispVOffset = 13,
      .DispVSync0 = 0,
      .DispVSync1 = 2,
      .DispPCLK = 6,
      .DispSwizzle = 2,
      .DispPCLKPol = 1,
      .DispCSpread = 0,
      .DispDither = 1 },
    //	DISPLAY_4_3
    { .module = EVE_3,
      .ft80x_enable = 0,
      .ft81x_enable = 1,
      .bt81x_enable = 1,
      .DispWidth = 480,
      .DispHeight = 272,
      .DispHCycle = 548,
      .DispHOffset = 43,
      .DispHSync0 = 0,
      .DispHSync1 = 41,
      .DispVCycle = 292,
      .DispVOffset = 12,
      .DispVSync0 = 0,
      .DispVSync1 = 10,
      .DispPCLK = 5,
      .DispSwizzle = 0,
      .DispPCLKPol = 1,
      .DispCSpread = 0,
      .DispDither = 1 },
    //	DISPLAY_5_0
    { .module = EVE_2,
      .ft80x_enable = 0,
      .ft81x_enable = 1,
      .bt81x_enable = 0,
      .DispWidth = 800,
      .DispHeight = 480,
      .DispHCycle = 1056,
      .DispHOffset = 46,
      .DispHSync0 = 0,
      .DispHSync1 = 10,
      .DispVCycle = 525,
      .DispVOffset = 23,
      .DispVSync0 = 0,
      .DispVSync1 = 10,
      .DispPCLK = 2,
      .DispSwizzle = 0,
      .DispPCLKPol = 0,
      .DispCSpread = 0,
      .DispDither = 1 },
    //	DISPLAY_7_0
    { .module = EVE_2,
      .ft80x_enable = 0,
      .ft81x_enable = 1,
      .bt81x_enable = 0,
      .DispWidth = 800,
      .DispHeight = 480,
      .DispHCycle = 1056,
      .DispHOffset = 46,
      .DispHSync0 = 0,
      .DispHSync1 = 10,
      .DispVCycle = 525,
      .DispVOffset = 23,
      .DispVSync0 = 0,
      .DispVSync1 = 10,
      .DispPCLK = 2,
      .DispSwizzle = 0,
      .DispPCLKPol = 0,
      .DispCSpread = 0,
      .DispDither = 1 }
};

int
RiBUS_Query( eve_display_size_t display, int tp )
{
    Gpu_HalInit_t halinit;
    uint8_t chipid;
    uint8_t loop = 0;

    Gpu_Hal_Init( &halinit );
    Gpu_Hal_Open( phost );

    Gpu_Hal_Powercycle( phost, TRUE );

    /*
        FT81x will be in SPI Single channel after POR. If we are here with
        FT4222 in multi channel, then an explicit switch to single channel is
        essential.
    */

    //  All currently used chips have FT81 enabled.
    //if ( phost->display_config.ft81x_enable )
    //{
        Gpu_Hal_SetSPI( phost, GPU_SPI_SINGLE_CHANNEL, GPU_SPI_ONEDUMMY );
    //}

    //  Right now we can compare only with screen size
    //if ( phost->display_config.module == EVE_2 )

    if ( ( display == EVE_DISPLAY_5_0 ) || ( display == EVE_DISPLAY_7_0 ) )
    {
        Gpu_HostCommand( phost, GPU_INTERNAL_OSC );
    }
    else
    {
        Gpu_HostCommand( phost, GPU_EXTERNAL_OSC );
    }

    Gpu_HostCommand( phost, GPU_PLL_48M );
    Gpu_HostCommand( phost, GPU_ACTIVE_M );
    Gpu_Hal_Sleep( 200 );

    //  Read Register ID to check if chip ID series is correct.

    do
    {
        Gpu_Hal_Sleep( 100 );
        chipid = Gpu_Hal_Rd8( phost, REG_ID );
        BSP_Print( "\r\nChip ID: %x", chipid );

        if ( ++loop > 20 )
        {
            return 0;
        }

    } while ( chipid != 0x7C );

    return 1;
}

void
BSP_Ribus_DisplayInitialize( eve_display_size_t size, uint8_t cal )
{
    BSP_Print( "\r\n\t\tModue: %d",
               host.display_config.module = EVE_DISPLAY_CONFIG[size].module );
    BSP_Print( "\r\n\t\tFT80x enable: %d",
               host.display_config.ft80x_enable =
                   EVE_DISPLAY_CONFIG[size].ft80x_enable );
    BSP_Print( "\r\n\t\tFT81x enable: %d",
               host.display_config.ft81x_enable =
                   EVE_DISPLAY_CONFIG[size].ft81x_enable );
    BSP_Print( "\r\n\t\tBT81x enable: %d",
               host.display_config.bt81x_enable =
                   EVE_DISPLAY_CONFIG[size].bt81x_enable );
    BSP_Print( "\r\n\t\tDisplay Width: %d",
               host.display_config.DispWidth =
                   EVE_DISPLAY_CONFIG[size].DispWidth );
    BSP_Print( "\r\n\t\tDisplay Hegiht: %d",
               host.display_config.DispHeight =
                   EVE_DISPLAY_CONFIG[size].DispHeight );
    BSP_Print( "\r\n\t\tH Cycle: %d",
               host.display_config.DispHCycle =
                   EVE_DISPLAY_CONFIG[size].DispHCycle );
    BSP_Print( "\r\n\t\tH Offset: %d",
               host.display_config.DispHOffset =
                   EVE_DISPLAY_CONFIG[size].DispHOffset );
    BSP_Print( "\r\n\t\tH Sync 0: %d",
               host.display_config.DispHSync0 =
                   EVE_DISPLAY_CONFIG[size].DispHSync0 );
    BSP_Print( "\r\n\t\tH Sync 1: %d",
               host.display_config.DispHSync1 =
                   EVE_DISPLAY_CONFIG[size].DispHSync1 );
    BSP_Print( "\r\n\t\tV Cycle: %d",
               host.display_config.DispVCycle =
                   EVE_DISPLAY_CONFIG[size].DispVCycle );
    BSP_Print( "\r\n\t\tV Offset: %d",
               host.display_config.DispVOffset =
                   EVE_DISPLAY_CONFIG[size].DispVOffset );
    BSP_Print( "\r\n\t\tV Sync 0: %d",
               host.display_config.DispVSync0 =
                   EVE_DISPLAY_CONFIG[size].DispVSync0 );
    BSP_Print( "\r\n\t\tV Sync 1: %d",
               host.display_config.DispVSync1 =
                   EVE_DISPLAY_CONFIG[size].DispVSync1 );
    BSP_Print( "\r\n\t\tCLK: %d",
               host.display_config.DispPCLK =
                   EVE_DISPLAY_CONFIG[size].DispPCLK );
    BSP_Print( "\r\n\t\tSwizzle: %d",
               host.display_config.DispSwizzle =
                   EVE_DISPLAY_CONFIG[size].DispSwizzle );
    BSP_Print( "\r\n\t\tPCLK pol: %d",
               host.display_config.DispPCLKPol =
                   EVE_DISPLAY_CONFIG[size].DispPCLKPol );
    BSP_Print( "\r\n\t\tC Spread: %d",
               host.display_config.DispCSpread =
                   EVE_DISPLAY_CONFIG[size].DispCSpread );
    BSP_Print( "\r\n\t\tDither: %d",
               host.display_config.DispDither =
                   EVE_DISPLAY_CONFIG[size].DispDither );

    App_Common_Init( &host );

    if ( cal )
    {
        if ( ( device_type != DEV_RGB_NO_TP ) &&
             ( device_type != DEV_IPS_NO_TP ) )

        {
            App_Calibrate_Screen( &host );
        }
    }
}

void
BSP_Ribus_DisplayResolution( eve_display_size_t size, uint32_t * w,
                             uint32_t * h )
{
    *w = EVE_DISPLAY_CONFIG[size].DispWidth;
    *h = EVE_DISPLAY_CONFIG[size].DispHeight;
}

void
BSP_Ribus_SingleTest( void )
{
    BSP_Print( "\r\n\tRiBUS Test" );
    App_Show_Logo( &host );
    BSP_Print( " - OK" );
}
