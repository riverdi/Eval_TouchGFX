/*
 * bsp_sdcard.h
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#ifndef INC_BSP_SDCARD_H_
#define INC_BSP_SDCARD_H_

int
BSP_SD_Card_Init( void );

int
BSP_SD_Card_Write_File( char * filename, char * data, unsigned int len );

int
BSP_SD_Card_Read_File( char * filename, char * data, unsigned int len );

void
BSP_SD_Card_SingleTest( void );

#endif /* INC_BSP_SDCARD_H_ */
