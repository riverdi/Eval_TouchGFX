/*
 * bsp_flash.h
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#ifndef INC_BSP_FLASH_H_
#define INC_BSP_FLASH_H_

uint16_t
BSP_Flash_Get_Manufacturer_ID( void );

void
BSP_Flash_Sector_Erase( uint32_t sector );

void
BSP_Flash_Chip_Erase( void );

void
BSP_Flash_Read_Data_Q( unsigned long address, unsigned char * data,
                       unsigned int length );

void
BSP_Flash_Write_Data_Q( unsigned long address, unsigned char * data,
                        unsigned int length );

void
BSP_Flash_SingleTest( void );

uint8_t BSP_QSPI_EnableMemoryMappedMode(void);
#endif /* INC_BSP_FLASH_H_ */
