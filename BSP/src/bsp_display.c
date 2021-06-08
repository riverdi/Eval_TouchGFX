/*
 * bsp_display.c
 *
 *  Created on: Sep 6, 2020
 *      Author: milos
 */
#include "bsp_display.h"
#include "bsp_sdram.h"
#include "ltdc.h"
#include "main.h"
#include "spi.h"

/// Constants -----------------------------------------------------------------

static const uint32_t DISPLAY_RESOLUTION[_DISPLAY_COUNT][2] = {
    //  W    H
    { 0, 0 },        //	Unknown display - generic values.
    { 240, 320 },    //	DISPLAY_RGB_2_8
    { 320, 240 },    //	DISPLAY_RGB_3_5
    { 480, 272 },    //	DISPLAY_RGB_4_3
    { 800, 480 },    //	DISPLAY_RGB_5_0
    { 800, 480 },    //	DISPLAY_RGB_7_0
    { 320, 240 },    // DISPLAY_IPS_3_5
    { 480, 272 },    // DISPLAY_IPS_4_3
    { 800, 480 },    // DISPLAY_IPS_5_0
    { 1024, 600 }    // DISPLAY_IPS_7_0
};

static const RCC_PeriphCLKInitTypeDef CLOCK_INIT[_DISPLAY_COUNT] = {
    // UNKNOWN - Generic values
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_RGB_2_8
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_RGB_3_5
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_RGB_4_3
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_RGB_5_0
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_RGB_7_0
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_IPS_3_5
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_IPS_4_3
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_IPS_5_0
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 96,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivR = RCC_PLLSAIDIVR_4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    },
    //  DISPLAY_IPS_7_0
    {
        .PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_LTDC,
        .PLLSAI.PLLSAIN = 384,
        .PLLSAI.PLLSAIR = 3,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8,
        .PLLSAIDivR = RCC_PLLSAIDIVR_2,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48
    }
};

static const LTDC_InitTypeDef DISPLAY_INIT[_DISPLAY_COUNT] = {
    //	UNKNOWN - Generic values
    { .HSPolarity = 0,
      .VSPolarity = 0,
      .DEPolarity = 0,
      .PCPolarity = 0,
      .HorizontalSync = 0,
      .VerticalSync = 0,
      .AccumulatedHBP = 0,
      .AccumulatedVBP = 0,
      .AccumulatedActiveW = 0,
      .AccumulatedActiveH = 0,
      .TotalWidth = 0,
      .TotalHeigh = 0,
      .Backcolor.Blue = 0,
      .Backcolor.Green = 0,
      .Backcolor.Red = 0 },

    //	DISPLAY_RGB_2_8
    { .HSPolarity = LTDC_HSPOLARITY_AL,
      .VSPolarity = LTDC_VSPOLARITY_AL,
      .DEPolarity = LTDC_DEPOLARITY_AL,
      .PCPolarity = LTDC_PCPOLARITY_IPC,
      .HorizontalSync = 11,
      .VerticalSync = 2,
      .AccumulatedHBP = 19,
      .AccumulatedVBP = 4,
      .AccumulatedActiveW = 259,
      .AccumulatedActiveH = 324,
      .TotalWidth = 267,
      .TotalHeigh = 332,
      .Backcolor.Blue = 0,
      .Backcolor.Green = 0,
      .Backcolor.Red = 0 },

    //	DISPLAY_RGB_3_5
    { .HSPolarity = LTDC_HSPOLARITY_AL,
      .VSPolarity = LTDC_VSPOLARITY_AL,
      .DEPolarity = LTDC_DEPOLARITY_AL,
      .PCPolarity = LTDC_PCPOLARITY_IPC,
      .HorizontalSync = 19,
      .VerticalSync = 4,
      .AccumulatedHBP = 69,
      .AccumulatedVBP = 12,
      .AccumulatedActiveW = 389,
      .AccumulatedActiveH = 252,
      .TotalWidth = 407,
      .TotalHeigh = 262,
      .Backcolor.Blue = 0,
      .Backcolor.Green = 0,
      .Backcolor.Red = 0 },

    //	DISPLAY_RGB_4_3
    { .HSPolarity = LTDC_HSPOLARITY_AL,
      .VSPolarity = LTDC_VSPOLARITY_AL,
      .DEPolarity = LTDC_DEPOLARITY_AL,
      .PCPolarity = LTDC_PCPOLARITY_IPC,
      .HorizontalSync = 19,
      .VerticalSync = 9,
      .AccumulatedHBP = 42,
      .AccumulatedVBP = 11,
      .AccumulatedActiveW = 522,
      .AccumulatedActiveH = 283,
      .TotalWidth = 547,
      .TotalHeigh = 291,
      .Backcolor.Blue = 0,
      .Backcolor.Green = 0,
      .Backcolor.Red = 0 },

    //	DISPLAY_RGB_5_0
    { .HSPolarity = LTDC_HSPOLARITY_AL,
      .VSPolarity = LTDC_VSPOLARITY_AL,
      .DEPolarity = LTDC_DEPOLARITY_AL,
      .PCPolarity = LTDC_PCPOLARITY_IPC,
      .HorizontalSync = 19,
      .VerticalSync = 9,
      .AccumulatedHBP = 45,
      .AccumulatedVBP = 22,
      .AccumulatedActiveW = 845,
      .AccumulatedActiveH = 502,
      .TotalWidth = 1055,
      .TotalHeigh = 524,
      .Backcolor.Blue = 0,
      .Backcolor.Green = 0,
      .Backcolor.Red = 0 },

    //	DISPLAY_RGB_7_0
    { .HSPolarity = LTDC_HSPOLARITY_AL,
      .VSPolarity = LTDC_VSPOLARITY_AL,
      .DEPolarity = LTDC_DEPOLARITY_AL,
      .PCPolarity = LTDC_PCPOLARITY_IPC,
      .HorizontalSync = 19,
      .VerticalSync = 9,
      .AccumulatedHBP = 45,
      .AccumulatedVBP = 22,
      .AccumulatedActiveW = 845,
      .AccumulatedActiveH = 502,
      .TotalWidth = 1055,
      .TotalHeigh = 524,
      .Backcolor.Blue = 0,
      .Backcolor.Green = 0,
      .Backcolor.Red = 0 },

      /*
       *  TODO:
       *  Fix display parameters here.
       */

      //  DISPLAY_IPS_3_5
      { .HSPolarity = LTDC_HSPOLARITY_AL,
        .VSPolarity = LTDC_VSPOLARITY_AL,
        .DEPolarity = LTDC_DEPOLARITY_AL,
        .PCPolarity = LTDC_PCPOLARITY_IPC,
        .HorizontalSync = 19,
        .VerticalSync = 4,
        .AccumulatedHBP = 69,
        .AccumulatedVBP = 12,
        .AccumulatedActiveW = 389,
        .AccumulatedActiveH = 252,
        .TotalWidth = 407,
        .TotalHeigh = 262,
        .Backcolor.Blue = 0,
        .Backcolor.Green = 0,
        .Backcolor.Red = 0 },

      //  DISPLAY_IPS_4_3
      { .HSPolarity = LTDC_HSPOLARITY_AL,
        .VSPolarity = LTDC_VSPOLARITY_AL,
        .DEPolarity = LTDC_DEPOLARITY_AL,
        .PCPolarity = LTDC_PCPOLARITY_IPC,
        .HorizontalSync = 19,
        .VerticalSync = 9,
        .AccumulatedHBP = 42,
        .AccumulatedVBP = 11,
        .AccumulatedActiveW = 522,
        .AccumulatedActiveH = 283,
        .TotalWidth = 547,
        .TotalHeigh = 291,
        .Backcolor.Blue = 0,
        .Backcolor.Green = 0,
        .Backcolor.Red = 0 },

      //  DISPLAY_IPS_5_0
      { .HSPolarity = LTDC_HSPOLARITY_AL,
        .VSPolarity = LTDC_VSPOLARITY_AL,
        .DEPolarity = LTDC_DEPOLARITY_AL,
        .PCPolarity = LTDC_PCPOLARITY_IPC,
        .HorizontalSync = 19,
        .VerticalSync = 9,
        .AccumulatedHBP = 45,
        .AccumulatedVBP = 22,
        .AccumulatedActiveW = 845,
        .AccumulatedActiveH = 502,
        .TotalWidth = 1055,
        .TotalHeigh = 524,
        .Backcolor.Blue = 0,
        .Backcolor.Green = 0,
        .Backcolor.Red = 0 },

      //  DISPLAY_IPS_7_0
      { .HSPolarity = LTDC_HSPOLARITY_AL,
        .VSPolarity = LTDC_VSPOLARITY_AL,
        .DEPolarity = LTDC_DEPOLARITY_AL,
        .PCPolarity = LTDC_PCPOLARITY_IPC,
        .HorizontalSync = 2,
        .VerticalSync = 2,
        .AccumulatedHBP = 162,
        .AccumulatedVBP = 25,
        .AccumulatedActiveW = 1186,
        .AccumulatedActiveH = 625,
        .TotalWidth = 1346,
        .TotalHeigh = 637,
        .Backcolor.Blue = 0,
        .Backcolor.Green = 0,
        .Backcolor.Red = 0 },
};


static const uint32_t DISPLAY_MAIN_COLORS[5] = {
    //		R			G			B			BL			WH
    0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 0x00FFFFFF
};

/// Variables -----------------------------------------------------------------

static display_t DISPLAY = DISPLAY_IPS_7_0;
static LTDC_LayerCfgTypeDef layerCfg;
static RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

/// Private Declaration -------------------------------------------------------

static void
SETUP_2_8( void );

/// Public Implementation -----------------------------------------------------

void
BSP_Display_Initialize( display_t type )
{
    DISPLAY = type;

    PeriphClkInitStruct.PeriphClockSelection = CLOCK_INIT[DISPLAY].PeriphClockSelection;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = CLOCK_INIT[DISPLAY].PLLSAI.PLLSAIN;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = CLOCK_INIT[DISPLAY].PLLSAI.PLLSAIR;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = CLOCK_INIT[DISPLAY].PLLSAI.PLLSAIP;
    PeriphClkInitStruct.PLLSAIDivR = CLOCK_INIT[DISPLAY].PLLSAIDivR;
    PeriphClkInitStruct.Clk48ClockSelection = CLOCK_INIT[DISPLAY].Clk48ClockSelection;
    PeriphClkInitStruct.SdioClockSelection = CLOCK_INIT[DISPLAY].SdioClockSelection;

    if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK )
    {
        Error_Handler( );
    }

    hltdc.Instance = LTDC;
    hltdc.Init.HSPolarity = DISPLAY_INIT[DISPLAY].HSPolarity;
    hltdc.Init.VSPolarity = DISPLAY_INIT[DISPLAY].VSPolarity;
    hltdc.Init.DEPolarity = DISPLAY_INIT[DISPLAY].DEPolarity;
    hltdc.Init.PCPolarity = DISPLAY_INIT[DISPLAY].PCPolarity;
    hltdc.Init.HorizontalSync = DISPLAY_INIT[DISPLAY].HorizontalSync;
    hltdc.Init.VerticalSync = DISPLAY_INIT[DISPLAY].VerticalSync;
    hltdc.Init.AccumulatedHBP = DISPLAY_INIT[DISPLAY].AccumulatedHBP;
    hltdc.Init.AccumulatedVBP = DISPLAY_INIT[DISPLAY].AccumulatedVBP;
    hltdc.Init.AccumulatedActiveW = DISPLAY_INIT[DISPLAY].AccumulatedActiveW;
    hltdc.Init.AccumulatedActiveH = DISPLAY_INIT[DISPLAY].AccumulatedActiveH;
    hltdc.Init.TotalWidth = DISPLAY_INIT[DISPLAY].TotalWidth;
    hltdc.Init.TotalHeigh = DISPLAY_INIT[DISPLAY].TotalHeigh;
    hltdc.Init.Backcolor.Blue = DISPLAY_INIT[DISPLAY].Backcolor.Blue;
    hltdc.Init.Backcolor.Green = DISPLAY_INIT[DISPLAY].Backcolor.Green;
    hltdc.Init.Backcolor.Red = DISPLAY_INIT[DISPLAY].Backcolor.Red;

    if ( HAL_LTDC_Init( &hltdc ) != HAL_OK )
    {
        Error_Handler( );
    }

    layerCfg.WindowX0 = 0;
    layerCfg.WindowX1 = DISPLAY_RESOLUTION[DISPLAY][0];
    layerCfg.WindowY0 = 0;
    layerCfg.WindowY1 = DISPLAY_RESOLUTION[DISPLAY][1];
    layerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    layerCfg.Alpha = 255;
    layerCfg.Alpha0 = 0;
    layerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    layerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
    layerCfg.FBStartAdress = 0xc0000000;
    layerCfg.ImageWidth = DISPLAY_RESOLUTION[DISPLAY][0];
    layerCfg.ImageHeight = DISPLAY_RESOLUTION[DISPLAY][1];
    layerCfg.Backcolor.Blue = 0;
    layerCfg.Backcolor.Green = 0;
    layerCfg.Backcolor.Red = 0;

    if ( HAL_LTDC_ConfigLayer( &hltdc, &layerCfg, 0 ) != HAL_OK )
    {
        Error_Handler( );
    }
}

void
BSP_Display_SetBackgroung( uint8_t r, uint8_t g, uint8_t b )
{
    uint32_t color;

    color = r;
    color <<= 8;
    color |= g;
    color <<= 8;
    color |= b;

    BSP_Display_PaintBox( 0, 0, DISPLAY_RESOLUTION[DISPLAY][0], DISPLAY_RESOLUTION[DISPLAY][1], color );
}

uint32_t
BSP_Display_GetWidth( display_t type )
{
    return DISPLAY_RESOLUTION[type][0];
}

uint32_t
BSP_Display_GetHeight( display_t type )
{
    return DISPLAY_RESOLUTION[type][1];
}

void
BSP_Display_Resolution( uint32_t * w, uint32_t * h )
{
    *w = DISPLAY_RESOLUTION[DISPLAY][0];
    *h = DISPLAY_RESOLUTION[DISPLAY][1];
}

void
BSP_Display_Enable( void )
{
    switch ( DISPLAY )
    {
    case DISPLAY_RGB_2_8:

        HAL_GPIO_WritePin( GPIOB, GPIO_PIN_7,
                           GPIO_PIN_SET );    //    Backlight switch
        HAL_GPIO_WritePin( LCD_GPIO3_GPIO_Port, LCD_GPIO3_Pin,
                           GPIO_PIN_SET );    //    ON/OFF
        HAL_GPIO_WritePin( LCD_GPIO3_GPIO_Port, LCD_GPIO3_Pin,
                           GPIO_PIN_RESET );    //  ON/OFF
        HAL_GPIO_WritePin( LCD_GPIO3_GPIO_Port, LCD_GPIO3_Pin,
                           GPIO_PIN_SET );    //    ON/OFF
        SETUP_2_8( );

        break;
    case DISPLAY_RGB_3_5:
    case DISPLAY_RGB_4_3:
    case DISPLAY_RGB_5_0:
    case DISPLAY_RGB_7_0:

        HAL_GPIO_WritePin( GPIOB,GPIO_PIN_7,
                           GPIO_PIN_SET );    //    Backlight switch
        HAL_GPIO_WritePin( LCD_GPIO3_GPIO_Port, LCD_GPIO3_Pin,
                           GPIO_PIN_SET );    //    ON/OFF
        break;
    case DISPLAY_IPS_3_5:
    case DISPLAY_IPS_4_3:
    case DISPLAY_IPS_5_0:
    case DISPLAY_IPS_7_0:
        HAL_GPIO_WritePin( GPIOB, GPIO_PIN_7,
                           GPIO_PIN_SET );    //    Backlight switch
        /*
         *  TODO:
         *  Add proper IPS displays enable sequence here.
         */

        break;
    default: break;
    }
}

void
BSP_Display_PaintBox( uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey,
                          uint32_t color )
{
    uint32_t x;
    uint32_t y;

    for ( y = sy; y < ey; ++y )
    {
        for ( x = sx; x < ex; ++x )
        {
            *( uint32_t * )( 0xc0000000 + ( ( (y * DISPLAY_RESOLUTION[DISPLAY][0]) + x ) * 4 ) ) = color;
        }
    }
}

void
BSP_Display_Pixel( uint32_t x, uint32_t y, uint32_t color )
{
    *( uint32_t * )( 0xc0000000 + ( ( (y * DISPLAY_RESOLUTION[DISPLAY][0]) + x ) * 4 ) ) = color;
}


void
BSP_Display_LoopTest( void )
{
    static uint32_t loop;

    BSP_Display_PaintBox( 0, 0, DISPLAY_RESOLUTION[DISPLAY][0], DISPLAY_RESOLUTION[DISPLAY][1], DISPLAY_MAIN_COLORS[++loop % 5] );
    BSP_Display_PaintBox( 10, 10, 100, 100,
                              DISPLAY_MAIN_COLORS[( 1 + loop ) % 5] );
}

// -------------------------------------------------------------------- 2.8 RGB

#define COMMAND ( 0 )
#define DATA ( 1 )

//	MOSI + GPIO 2 bit-banding + CS

#define CS( x ) HAL_GPIO_WritePin( R_CS_GPIO_Port, R_CS_Pin, ( x ) )

//#define CLK(x)	HAL_GPIO_WritePin( LCD_GPIO2_GPIO_Port, LCD_GPIO2_Pin, (x) )
#define CLK( x ) HAL_GPIO_WritePin( RIB_SCK_GPIO_Port, RIB_SCK_Pin, ( x ) )

#define SDO( x ) HAL_GPIO_WritePin( RIB_MISO_GPIO_Port, RIB_MISO_Pin, ( x ) )
//#define SDO(x)	HAL_GPIO_WritePin( RIB_MOSI_GPIO_Port, RIB_MOSI_Pin, (x) )
//#define SDO(x)	HAL_GPIO_WritePin( LCD_GPIO2_GPIO_Port, LCD_GPIO2_Pin, (x) )

static void
ILI_CS_Send( uint8_t data )
{
    unsigned char m = 0x80;

    for ( int i = 0; i < 8; i++ )
    {
        CLK( GPIO_PIN_RESET );

        if ( data & m )
        {
            SDO( GPIO_PIN_SET );
        }
        else
        {
            SDO( GPIO_PIN_RESET );
        }

        CLK( GPIO_PIN_SET );

        m >>= 1;

        CLK( GPIO_PIN_RESET );
    }
}

static void
CS_ILI( uint8_t mode )
{
#define REG_GPIO 3154068UL

    switch ( mode )
    {
    case 0:

        CS( GPIO_PIN_RESET );
        ILI_CS_Send( ( ( REG_GPIO >> 16 ) & 0xBF ) | 0x80 );
        ILI_CS_Send( ( REG_GPIO & 0xFF00 ) >> 8 );
        ILI_CS_Send( ( REG_GPIO & 0xFF ) );
        ILI_CS_Send( 0x00 );
        CS( GPIO_PIN_SET );

        break;
    case 1:

        CS( GPIO_PIN_RESET );
        ILI_CS_Send( ( ( REG_GPIO >> 16 ) & 0xBF ) | 0x80 );
        ILI_CS_Send( ( REG_GPIO & 0xFF00 ) >> 8 );
        ILI_CS_Send( ( REG_GPIO & 0xFF ) );
        ILI_CS_Send( 0x83 );
        CS( GPIO_PIN_SET );
        SDO( GPIO_PIN_RESET );
        CLK( GPIO_PIN_RESET );

        break;
    }
}

static void
ILI_Send( uint8_t type, uint8_t data )
{
    unsigned char m = 0x80;

    if ( type == COMMAND )
    {
        CLK( GPIO_PIN_RESET );
        SDO( GPIO_PIN_RESET );
        CLK( GPIO_PIN_SET );
    }
    else if ( type == DATA )
    {
        CLK( GPIO_PIN_RESET );
        SDO( GPIO_PIN_SET );
        CLK( GPIO_PIN_SET );
    }

    for ( int i = 0; i < 8; i++ )
    {
        CLK( GPIO_PIN_RESET );

        if ( data & m )
        {
            SDO( GPIO_PIN_SET );
        }
        else
        {
            SDO( GPIO_PIN_RESET );
        }

        CLK( GPIO_PIN_SET );

        m >>= 1;
    }

    HAL_Delay( 1 );
}

static void
SETUP_2_8( void )
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    HAL_SPI_MspDeInit( &hspi2 );

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = RIB_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( RIB_MOSI_GPIO_Port, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = RIB_MISO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( RIB_MISO_GPIO_Port, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = RIB_SCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( RIB_SCK_GPIO_Port, &GPIO_InitStruct );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0x01 );
    HAL_Delay( 5 );
    CS_ILI( 1 );

    /* display off */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0x28 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xcf );
    ILI_Send( DATA, 0x00 );
    ILI_Send( DATA, 0x81 );
    ILI_Send( DATA, 0x30 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xed );
    ILI_Send( DATA, 0x64 );
    ILI_Send( DATA, 0x03 );
    ILI_Send( DATA, 0x12 );
    ILI_Send( DATA, 0x81 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xe8 );
    ILI_Send( DATA, 0x85 );
    ILI_Send( DATA, 0x01 );
    ILI_Send( DATA, 0x79 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xcb );
    ILI_Send( DATA, 0x39 );
    ILI_Send( DATA, 0x2c );
    ILI_Send( DATA, 0x00 );
    ILI_Send( DATA, 0x34 );
    ILI_Send( DATA, 0x02 );
    CS_ILI( 1 );

    /* interface control */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xF6 );
    ILI_Send( DATA, 0x01 );
    ILI_Send( DATA, 0x00 );
    ILI_Send( DATA, 0x06 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xf7 );
    ILI_Send( DATA, 0x20 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xea );
    ILI_Send( DATA, 0x06 );
    ILI_Send( DATA, 0x00 );
    CS_ILI( 1 );

    /* power control */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xc0 );
    ILI_Send( DATA, 0x26 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xc1 );
    ILI_Send( DATA, 0x11 );
    CS_ILI( 1 );

    /* VCOM */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xc5 );
    ILI_Send( DATA, 0x35 );
    ILI_Send( DATA, 0x3E );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xc7 );
    ILI_Send( DATA, 0xBE );
    CS_ILI( 1 );

    /* memory access control */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0x36 );
    ILI_Send( DATA, 0x40 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0x3a );
    ILI_Send( DATA, 0x60 );
    CS_ILI( 1 );

    /* frame rate */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xb0 );
    ILI_Send( DATA, 0xC0 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xb1 );
    ILI_Send( DATA, 0x00 );
    ILI_Send( DATA, 0x1B );
    CS_ILI( 1 );

    /* gamma */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xf2 );
    ILI_Send( DATA, 0x02 );
    CS_ILI( 1 );

    CS_ILI( 0 );
    ILI_Send( COMMAND, 0x26 );
    ILI_Send( DATA, 0x01 );
    CS_ILI( 1 );

    /* p-gamma correction */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xE0 );
    ILI_Send( DATA, 0x1f );
    ILI_Send( DATA, 0x1a );
    ILI_Send( DATA, 0x18 );
    ILI_Send( DATA, 0x0a );
    ILI_Send( DATA, 0x0f );
    ILI_Send( DATA, 0x06 );
    ILI_Send( DATA, 0x45 );
    ILI_Send( DATA, 0x87 );
    ILI_Send( DATA, 0x32 );
    ILI_Send( DATA, 0x0a );
    ILI_Send( DATA, 0x07 );
    ILI_Send( DATA, 0x02 );
    ILI_Send( DATA, 0x07 );
    ILI_Send( DATA, 0x05 );
    ILI_Send( DATA, 0x00 );
    CS_ILI( 1 );

    /* n-gamma correction */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0xE1 );
    ILI_Send( DATA, 0x00 );
    ILI_Send( DATA, 0x25 );
    ILI_Send( DATA, 0x27 );
    ILI_Send( DATA, 0x05 );
    ILI_Send( DATA, 0x10 );
    ILI_Send( DATA, 0x09 );
    ILI_Send( DATA, 0x3a );
    ILI_Send( DATA, 0x78 );
    ILI_Send( DATA, 0x4d );
    ILI_Send( DATA, 0x05 );
    ILI_Send( DATA, 0x18 );
    ILI_Send( DATA, 0x0d );
    ILI_Send( DATA, 0x38 );
    ILI_Send( DATA, 0x3a );
    ILI_Send( DATA, 0x1f );
    CS_ILI( 1 );

    /* sleep out */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0x11 );
    HAL_Delay( 10 );
    CS_ILI( 1 );

    /* display on */
    CS_ILI( 0 );
    ILI_Send( COMMAND, 0x29 );
    HAL_Delay( 10 );
    CS_ILI( 1 );

    HAL_SPI_MspInit( &hspi2 );
}
