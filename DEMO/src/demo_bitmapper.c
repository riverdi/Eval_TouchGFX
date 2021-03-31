/*
 * demo_bitmapper.c
 *
 *  Created on: Sep 27, 2020
 *      Author: milos
 */

#include "App_Common.h"
#include "Gpu.h"
#include "demo.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>

/// Types ---------------------------------------------------------------------

#pragma pack( push, 1 )
struct file_meta
{
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixelArrayOffset;
};

struct bitmap_meta
{
    uint32_t dibHeaderSize;
    int32_t imageWidth;
    int32_t imageHeight;
    uint16_t numColorPlanes;
    uint16_t bitsPerPixel;
    uint32_t compressionMethod;
    uint32_t totalImageSize;
    int32_t horizontalResolution;
    int32_t verticalResolution;
    uint32_t numColorsInPalette;
    uint32_t numImportantColors;
};
#pragma pack( pop )

/// Declarations---------------------------------------------------------------

static int
_compose_bitmap_filename( uint8_t file_index, char * fname );

static int
_compose_bitmap_filename_ribus( uint8_t file_index, char * fname );

static int
_display_bitmap( char * fname );

static int
_display_bitmap_ribus( char * fname );

static int
_verify_file_meta( struct file_meta * desc );

static int
_verify_bmp_meta( struct bitmap_meta * desc );

/// Exported ------------------------------------------------------------------

int
DEMO_DisplayBitmapFromSd( uint8_t idx )
{
    char filename[256] = {0};

    if ( _compose_bitmap_filename( idx, filename ) )
    {
        BSP_Print( "\r\n[DEMO RUN] Filename compose failed" );
        return 1;
    }

    if ( _display_bitmap( filename ) )
    {
        BSP_Print( "\r\n[DEMO RUN] Display image failed" );
        return 1;
    }

    return 0;
}

int
DEMO_DisplayBitmapFromSd_Ribus( uint8_t idx )
{
    char filename[256] = {0};

    if ( _compose_bitmap_filename_ribus( idx, filename ) )
    {
        BSP_Print( "\r\n[DEMO RUN] Filename compose failed" );
        return 1;
    }

    if ( _display_bitmap_ribus( filename ) )
    {
        BSP_Print( "\r\n[DEMO RUN] Display image failed" );
        return 1;
    }

    return 0;
}

/// Privates ------------------------------------------------------------------

static int
_compose_bitmap_filename( uint8_t file_index, char * fname )
{
    char tmp[16];

    //	Default file system.
    strcpy( fname, "0:/" );

    //	Default folder location depending on screen resolution.
    snprintf( tmp, sizeof( tmp ), "%lu", d_w );
    strcat( fname, tmp );
    strcat( fname, "x" );
    snprintf( tmp, sizeof( tmp ), "%lu", d_h );
    strcat( fname, tmp );
    strcat( fname, "/" );

    //	Filename composed of index and default extension.
    snprintf( tmp, sizeof( tmp ), "%d", file_index );
    strcat( fname, tmp );
    strcat( fname, ".bmp" );
    BSP_Print( "\r\n[DEMO RUN] Filename %s: ", fname );

    return 0;
}

static int
_compose_bitmap_filename_ribus( uint8_t file_index, char * fname )
{
    char tmp[16];

#if 0
	//	Default file system.
	strcpy( fname, "0:/" );

	//	Default folder location depending on screen resolution.
	snprintf( tmp, sizeof(tmp), "%lu", d_w );
	strcat( fname, tmp );
	strcat( fname, "x" );
	snprintf( tmp, sizeof(tmp), "%lu", d_h );
	strcat( fname, tmp );
	strcat( fname, "/" );
#endif

    strcpy( fname, "0:/1024x600/" );

    //	Filename composed of index and default extension.
    snprintf( tmp, sizeof( tmp ), "%d", file_index );
    strcat( fname, tmp );
    strcat( fname, ".bmp" );
    BSP_Print( "\r\n[DEMO RUN] Filename %s: ", fname );

    return 0;
}

static int
_display_bitmap( char * fname )
{
	FIL file;
	UINT read;
	struct file_meta file_desc;
	struct bitmap_meta bmp_desc;

	if ( FR_OK != f_open( &file, fname, FA_READ ) )
	{
		BSP_Print( "\r\n[DEMO RUN] Can't open file" );
		return 1;
	}

	//	Read and verify file meta.
	if ( FR_OK != f_read( &file, &file_desc, sizeof( struct file_meta ), &read ) )
	{
		BSP_Print( "\r\n[DEMO RUN] Can't read file metadata" );
		f_close( &file );
		return 1;
	}

	if (( read != sizeof( struct file_meta ) ) || ( _verify_file_meta( &file_desc ) ) )
	{
		BSP_Print( "\r\n[DEMO RUN] File metadata verification failed" );
		f_close( &file );
		return 1;
	}

	//	Read bitmap meta.
	if ( FR_OK != f_read( &file, &bmp_desc, sizeof( struct bitmap_meta ), &read ) )
	{
		BSP_Print( "\r\n[DEMO RUN] Can't read bitmap metadata" );
		f_close( &file );
		return 1;
	}

	if (( read != sizeof( struct bitmap_meta ) ) || ( _verify_bmp_meta( &bmp_desc ) ) )
	{
		BSP_Print( "\r\n[DEMO RUN] Bitmap metadata verification failed" );
		f_close( &file );
		return 1;
	}

	//	Read bitmap data - row by row, starting from the bottom.
	for ( int32_t rc = bmp_desc.imageHeight; rc > 0; --rc )
	{
		uint8_t row_buffer[3840];		//	Bytes per pixel * max row width => 3 * 1280 = 3840

		//	Load row from file to buffer.
		if ( FR_OK != f_read( &file, row_buffer, ( bmp_desc.imageWidth * 3 ), &read ) )
		{
			BSP_Print( "\r\n[DEMO RUN] Bitmap data read failed" );
			f_close( &file );
			return 1;
		}

		//	Display row from buffer - left to right.
		for ( int32_t cc = 0; cc < bmp_desc.imageWidth; ++cc )
		{
			uint32_t pixel = 0;

			/*
			 * Convert bmp format to frame buffer format.
			 *  3 bytes per pixel to 4 bytes per pixel.
			 */
			pixel |= row_buffer[cc * 3 + 2];
			pixel <<= 8;
			pixel |= row_buffer[cc * 3 + 1];
			pixel <<= 8;
			pixel |= row_buffer[cc * 3];

			BSP_Display_Pixel( cc, rc, pixel );
		}
	}

	f_close( &file );
	return 0;
}

static int
_display_bitmap_ribus( char * fname )
{
    //	Download data to the RAM

    FIL file;
    UINT read;
    struct file_meta file_desc;
    struct bitmap_meta bmp_desc;

    if ( FR_OK != f_open( &file, fname, FA_READ ) )
    {
        BSP_Print( "\r\n[DEMO RUN] Can't open file" );
        return 1;
    }

    //	Read and verify file meta.
    if ( FR_OK !=
         f_read( &file, &file_desc, sizeof( struct file_meta ), &read ) )
    {
        BSP_Print( "\r\n[DEMO RUN] Can't read file metadata" );
        f_close( &file );
        return 1;
    }

#if 0
	if (( read != sizeof( struct file_meta ) ) || ( _verify_file_meta( &file_desc ) ) )
	{
		BSP_Print( "\r\n[DEMO RUN] File metadata verification failed" );
		f_close( &file );
		return 1;
	}
#endif

    //	Read bitmap meta.
    if ( FR_OK !=
         f_read( &file, &bmp_desc, sizeof( struct bitmap_meta ), &read ) )
    {
        BSP_Print( "\r\n[DEMO RUN] Can't read bitmap metadata" );
        f_close( &file );
        return 1;
    }

#if 0
	if (( read != sizeof( struct bitmap_meta ) ) || ( _verify_bmp_meta( &bmp_desc ) ) )
	{
		BSP_Print( "\r\n[DEMO RUN] Bitmap metadata verification failed" );
		f_close( &file );
		return 1;
	}
#endif

    //	Read bitmap data - row by row, starting from the bottom.
    for ( int32_t rc = bmp_desc.imageHeight; rc > 0; --rc )
    {
        uint8_t row_buffer[2400];    //	Bytes per pixel * max row width => 3 *
                                     //800 = 2400
        uint8_t rgb_buffer[1600];    //	Bytes per pixel * max row width => 2 *
                                     //800 = 1600

        //	Load row from file to buffer.
        if ( FR_OK !=
             f_read( &file, row_buffer, ( bmp_desc.imageWidth * 3 ), &read ) )
        {
            BSP_Print( "\r\n[DEMO RUN] Bitmap data read failed" );
            f_close( &file );
            return 1;
        }

        //	Display row from buffer - left to right.
        for ( int32_t cc = 0; cc < bmp_desc.imageWidth; ++cc )
        {
            /*
             * Convert bmp format to frame buffer format.
             *  3 bytes per pixel to 2 bytes per pixel RGB565.
             */
            rgb_buffer[cc * 2 + 1] = ( row_buffer[cc * 3 + 2] & 0xF8 );
            rgb_buffer[cc * 2 + 1] |= ( row_buffer[cc * 3 + 1] >> 5 );
            rgb_buffer[cc * 2] = ( row_buffer[cc * 3 + 1] & 0xE0 );
            rgb_buffer[cc * 2] |= ( row_buffer[cc * 3] >> 3 );
        }

        Gpu_Hal_WrMem( &host, RAM_G + ( rc * bmp_desc.imageWidth * 2 ),
                       rgb_buffer, bmp_desc.imageWidth * 2 );
    }

    App_DisplayImage( &host, bmp_desc.imageWidth, bmp_desc.imageHeight, d_w,
                      d_h );
    f_close( &file );

    return 0;
}

static int
_verify_file_meta( struct file_meta * desc )
{
    //	Signature of the .bmp file according to documentation.
    if ( desc->signature != 0x4D42 )
    {
        return 1;
    }

    return 0;
}

static int
_verify_bmp_meta( struct bitmap_meta * desc )
{
    //	Width and height must match the display resolution.
    if ( desc->imageHeight != d_h )
    {
        BSP_Print( "\r\n[DEMO RUN] Wrong image height: %d", desc->imageHeight );
        return 1;
    }
    if ( desc->imageWidth != d_w )
    {
        BSP_Print( "\r\n[DEMO RUN] Wrong image width: %d", desc->imageWidth );
        return 1;
    }

    return 0;
}

/// -------------------------------------------------------------- End of file
