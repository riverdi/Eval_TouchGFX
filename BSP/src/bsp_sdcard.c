/*
 * bsp_sdcard.c
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#include "bsp_sdcard.h"
#include "bsp_print.h"
#include "fatfs.h"
#include "fatfs_platform.h"
#include <string.h>

static void
_Print_Result( FRESULT result );

int
BSP_SD_Card_Init( void )
{
    FRESULT res;

    //	Enable to format the SD Card for the first time.
#if 0

	BYTE work[_MAX_SS];

	BSP_Print( "\r\nFATFS mkfs" );
	if ( FR_OK != ( res = f_mkfs((TCHAR const*)SDPath, FM_FAT, 0, work, sizeof(work)) ) )
	{
		_Print_Result( res );
		return 1;
	}
	BSP_Print( " - OK" );

#endif

    if ( FR_OK != ( res = f_mount( &SDFatFS, ( TCHAR const * )SDPath, 0 ) ) )
    {
        _Print_Result( res );
        return 1;
    }

    return 0;
}

int
BSP_SD_Card_Write_File( char * filename, char * data, unsigned int len )
{
    FIL fil;
    UINT written;

    if ( FR_OK != f_open( &fil, filename, FA_CREATE_ALWAYS | FA_WRITE ) )
    {
        return 1;
    }

    if ( FR_OK != f_write( &fil, data, len, &written ) )
    {
        f_close( &fil );
        return 1;
    }

    f_close( &fil );

    return 0;
}

int
BSP_SD_Card_Read_File( char * filename, char * data, unsigned int len )
{
    FIL fil;
    UINT read;

    if ( FR_OK != f_open( &fil, filename, FA_READ ) )
    {
        return 1;
    }

    if ( FR_OK != f_read( &fil, data, len, &read ) )
    {
        f_close( &fil );
        return 1;
    }

    f_close( &fil );

    return 0;
}

void
BSP_SD_Card_SingleTest( void )
{
    BSP_Print( "\r\n\tSD Card Initialization" );
    if ( BSP_SD_Card_Init( ) )
    {
        BSP_Print( " - Error during initialization" );
    }
    else
    {
        char temp[10];
        char data[10] = "0xFFFFFFFF";

        if ( BSP_SD_Card_Write_File( "0:/test.txt", data, strlen( data ) ) )
        {
            BSP_Print( " - Error during write" );
        }

        if ( BSP_SD_Card_Read_File( "test.txt", temp, strlen( data ) ) )
        {
            BSP_Print( " - Error during read" );
        }

        if ( strcmp( temp, data ) )
        {
            BSP_Print( " - Error comparing files" );
        }

        BSP_Print( " - OK" );
    }
}

static void
_Print_Result( FRESULT result )
{
    BSP_Print( " - Result: " );

    switch ( result )
    {
    case FR_OK: BSP_Print( "Succeeded" ); break;
    case FR_DISK_ERR:
        BSP_Print( "A hard error occurred in the low level disk I/O layer" );
        break;
    case FR_INT_ERR: BSP_Print( "Assertion failed" ); break;
    case FR_NOT_READY: BSP_Print( "The physical drive cannot work" ); break;
    case FR_NO_FILE: BSP_Print( "Could not find the file" ); break;
    case FR_NO_PATH: BSP_Print( "Could not find the path" ); break;
    case FR_INVALID_NAME: BSP_Print( "The path name format is invalid" ); break;
    case FR_DENIED:
        BSP_Print( "Access denied due to prohibited access or directory full" );
        break;
    case FR_EXIST: BSP_Print( "Access denied due to prohibited access" ); break;
    case FR_INVALID_OBJECT:
        BSP_Print( "The file/directory object is invalid" );
        break;
    case FR_WRITE_PROTECTED:
        BSP_Print( "The physical drive is write protected" );
        break;
    case FR_INVALID_DRIVE:
        BSP_Print( "The logical drive number is invalid" );
        break;
    case FR_NOT_ENABLED: BSP_Print( "The volume has no work area" ); break;
    case FR_NO_FILESYSTEM: BSP_Print( "There is no valid FAT volume" ); break;
    case FR_MKFS_ABORTED:
        BSP_Print( "The f_mkfs() aborted due to any problem" );
        break;
    case FR_TIMEOUT:
        BSP_Print( "Could not get a grant to access the volume within defined "
                   "period" );
        break;
    case FR_LOCKED:
        BSP_Print(
            "The operation is rejected according to the file sharing policy" );
        break;
    case FR_NOT_ENOUGH_CORE:
        BSP_Print( "LFN working buffer could not be allocated" );
        break;
    case FR_TOO_MANY_OPEN_FILES:
        BSP_Print( "Number of open files > _FS_LOCK" );
        break;
    case FR_INVALID_PARAMETER: BSP_Print( "Given parameter is invalid" ); break;
    default: BSP_Print( "Unknown" ); break;
    }
}
