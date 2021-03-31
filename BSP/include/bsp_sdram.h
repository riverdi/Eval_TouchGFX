/*
 * bsp_sram.h
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#ifndef INC_BSP_SDRAM_H_
#define INC_BSP_SDRAM_H_

/*
 * 	SDRAM data sheet: http://www.issi.com/WW/pdf/42-45S16400J.pdf
 *
 * 	IS42S16400J SDRAM SIZE: 67108864 bits = 8388608 bytes = 0x800000 bytes
 * 	SDRAM refresh counter (90MHz SDRAM clock)
 */
#define RAMSIZE_BYTES ( ( uint32_t )0x800000 )
#define RAMSIZE_DWORDS ( ( uint32_t )( RAMSIZE_BYTES / sizeof( uint32_t ) ) )
#define REFRESH_COUNT ( ( uint32_t )0x056A )
#define SDRAM_BANK_ADDR ( ( uint32_t )0xC0000000 )

void
BSP_SDRAM_Configuration( void );

void
BSP_SDRAM_Initialization_Sequence( void );

void
BSP_SDRAM_Write_Dword( unsigned long address, unsigned long data );

unsigned long
BSP_SDRAM_Read_Dword( unsigned long address );

void
BSP_SDRAM_Initliazation( void );

void
BSP_SDRAM_SingleTest( void );

#endif /* INC_BSP_SDRAM_H_ */
