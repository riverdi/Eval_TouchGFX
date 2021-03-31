/*
 * bsp_flash.c
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#include "bsp_print.h"
#include "quadspi.h"
#include <stdlib.h>
#include <string.h>

#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0x60 //C7

#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define QUAD_INOUT_4BYTE_FAST_READ_CMD       0xEC

#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32
#define EXT_QUAD_IN_FAST_PROG_CMD            0x12

static void
_Write_enable( void );
static unsigned char
_Flash_Busy( void );
static void
_Quad_Enable( void );

void
BSP_Flash_Get_Manufacturer_ID( void )
{
    QSPI_CommandTypeDef cmd;
    unsigned char data[2];

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = READ_ID_CMD;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = 0x00000000UL;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DummyCycles = 0;
    cmd.NbData = 2;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    HAL_QSPI_Command( &hqspi, &cmd, 1000 );
    HAL_QSPI_Receive( &hqspi, data, 1000 );

    BSP_Print( "\r\n\tManufacturer ID: %xh %xh", data[0], data[1] );
}



uint8_t BSP_QSPI_EnableMemoryMappedMode(void)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  /* Configure the command for the read instruction */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 6;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (HAL_QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

void
BSP_Flash_Sector_Erase( uint32_t sector )
{
    QSPI_CommandTypeDef cmd;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = SECTOR_ERASE_CMD;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = sector;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_NONE;
    cmd.DummyCycles = 0;
    cmd.NbData = 0;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    _Quad_Enable( );
    _Write_enable( );
    HAL_QSPI_Command( &hqspi, &cmd, 1000 );

    BSP_Print( "\r\n\tSector erasing - " );
    while ( _Flash_Busy( ) )
        ;
    BSP_Print( "Done" );
}

void
BSP_Flash_Chip_Erase( void )
{
    QSPI_CommandTypeDef cmd;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CHIP_ERASE_CMD;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = 0x00000000UL;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_NONE;
    cmd.DummyCycles = 0;
    cmd.NbData = 0;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    HAL_QSPI_Command( &hqspi, &cmd, 1000 );

    BSP_Print( "\r\n\tChip erasing - " );
    while ( _Flash_Busy( ) )
        ;
    BSP_Print( "Done" );
}

void
BSP_Flash_Read_Data_Q( unsigned long address, unsigned char * data,
                       unsigned int length )
{
    QSPI_CommandTypeDef cmd;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = QUAD_OUT_FAST_READ_CMD;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = ( address & 0x007FFFFFUL );
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.DummyCycles = 8;
    cmd.NbData = length;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    _Quad_Enable( );
    HAL_QSPI_Command( &hqspi, &cmd, 1000 );
    HAL_QSPI_Receive( &hqspi, data, 1000 );

    BSP_Print( "\r\n\tData Read (%x) -", address );
    for ( unsigned int i = 0; i < length; ++i )
    {
        BSP_Print( " %xh", data[i] );
    }
}

void
BSP_Flash_Write_Data_Q( unsigned long address, unsigned char * data,
                        unsigned int length )
{
    QSPI_CommandTypeDef cmd;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = QUAD_INPUT_PAGE_PROG_CMD;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = ( address & 0x007FFFFFUL );
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.DummyCycles = 0;
    cmd.NbData = length;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    _Quad_Enable( );
    _Write_enable( );
    HAL_QSPI_Command( &hqspi, &cmd, 1000 );
    HAL_QSPI_Transmit( &hqspi, data, 1000 );

    BSP_Print( "\r\n\tData Write (%x) -", address );
    for ( unsigned int i = 0; i < length; ++i )
    {
        BSP_Print( " %xh", data[i] );
    }
    BSP_Print( "\r\n\t\tFlash Writing - " );
    while ( _Flash_Busy( ) )
        ;
    BSP_Print( "Done" );
}

void
BSP_Flash_SingleTest( void )
{
    char custom_string_wr[10] = "Hello";
    char custom_string_rd[10] = "";
    unsigned long address = rand( ) & 0x007FFFFF;

    BSP_Print( "\t\tFlash Test\r\n" );
    BSP_Flash_Get_Manufacturer_ID( );
    BSP_Flash_Chip_Erase( );

    BSP_Flash_Write_Data_Q( address, ( unsigned char * )custom_string_wr,
                            strlen( custom_string_wr ) );
    BSP_Flash_Read_Data_Q( address, ( unsigned char * )custom_string_rd,
                           strlen( custom_string_wr ) );

    if ( strcmp( custom_string_wr, custom_string_rd ) )
    {
        BSP_Print( "\r\n\tFlash Test - FAILED" );
    }
    else
    {
        BSP_Print( "\r\n\tFlash Test - OK" );
    }
}

static unsigned char
_Flash_Busy( void )
{
    unsigned char status_reg;
    QSPI_CommandTypeDef cmd;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = 0x05;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = 0x00000000UL;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DummyCycles = 0;
    cmd.NbData = 1;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    HAL_QSPI_Command( &hqspi, &cmd, 1000 );
    HAL_QSPI_Receive( &hqspi, &status_reg, 1000 );

    BSP_Print( "\r\n\t\tStatus Register - %xh", status_reg );
    return ( status_reg & 0x01 );
}

static void
_Quad_Enable( void )
{
    unsigned char status_reg;
    QSPI_CommandTypeDef cmd;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = 0x35;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = 0x00000000UL;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DummyCycles = 0;
    cmd.NbData = 1;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    HAL_QSPI_Command( &hqspi, &cmd, 1000 );
    HAL_QSPI_Receive( &hqspi, &status_reg, 1000 );

    cmd.Instruction = 0x31;
    status_reg |= 0x02;

    HAL_QSPI_Command( &hqspi, &cmd, 1000 );
    HAL_QSPI_Transmit( &hqspi, &status_reg, 1000 );
    BSP_Print( "\r\n\t\tQuad Enabled" );
}

static void
_Write_enable( void )
{
    QSPI_CommandTypeDef cmd;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = 0x06;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = 0x00000000UL;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_NONE;
    cmd.DummyCycles = 0;
    cmd.NbData = 0;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    HAL_QSPI_Command( &hqspi, &cmd, 1000 );
    BSP_Print( "\r\n\t\tWrite Enabled" );
}
