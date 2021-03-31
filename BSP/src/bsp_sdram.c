/*
 * bsp_sram.c
 *
 *  Created on: Aug 26, 2020
 *      Author: milos
 */

#include "bsp_sdram.h"
#include "bsp_print.h"
#include "fmc.h"

/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_8 */
#define SDRAM_MEMORY_WIDTH FMC_SDRAM_MEM_BUS_WIDTH_16

/* #define SDCLOCK_PERIOD                   FMC_SDRAM_CLOCK_PERIOD_2 */
#define SDCLOCK_PERIOD FMC_SDRAM_CLOCK_PERIOD_3

#define SDRAM_TIMEOUT ( ( uint32_t )0xFFFF )

#define SDRAM_MODEREG_BURST_LENGTH_1 ( ( uint16_t )0x0000 )
#define SDRAM_MODEREG_BURST_LENGTH_2 ( ( uint16_t )0x0001 )
#define SDRAM_MODEREG_BURST_LENGTH_4 ( ( uint16_t )0x0002 )
#define SDRAM_MODEREG_BURST_LENGTH_8 ( ( uint16_t )0x0004 )
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ( ( uint16_t )0x0000 )
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ( ( uint16_t )0x0008 )
#define SDRAM_MODEREG_CAS_LATENCY_2 ( ( uint16_t )0x0020 )
#define SDRAM_MODEREG_CAS_LATENCY_3 ( ( uint16_t )0x0030 )
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ( ( uint16_t )0x0000 )
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ( ( uint16_t )0x0000 )
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ( ( uint16_t )0x0200 )

void
BSP_SDRAM_Configuration( void )
{
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;

    /* SDRAM device configuration */
    hsdram1.Instance = FMC_SDRAM_DEVICE;

    /* Timing configuration for 90 MHz of SDRAM clock frequency (180MHz/2) */
    /* TMRD: 2 Clock cycles */
    SDRAM_Timing.LoadToActiveDelay = 2;
    /* TXSR: min=70ns (6x11.90ns) */
    SDRAM_Timing.ExitSelfRefreshDelay = 7;
    /* TRAS: min=42ns (4x11.90ns) max=120k (ns) */
    SDRAM_Timing.SelfRefreshTime = 4;
    /* TRC:  min=63 (6x11.90ns) */
    SDRAM_Timing.RowCycleDelay = 7;
    /* TWR:  2 Clock cycles */
    SDRAM_Timing.WriteRecoveryTime = 2;
    /* TRP:  15ns => 2x11.90ns */
    SDRAM_Timing.RPDelay = 2;
    /* TRCD: 15ns => 2x11.90ns */
    SDRAM_Timing.RCDDelay = 2;

    hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
    hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
    hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
    hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
    hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
    hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram1.Init.SDClockPeriod = SDCLOCK_PERIOD;
    hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;

    /* Initialize the SDRAM controller */
    if ( HAL_SDRAM_Init( &hsdram1, &SDRAM_Timing ) != HAL_OK )
    {
        /* Initialization Error */
        Error_Handler( );
    }
}

/**
 * @brief  Perform the SDRAM exernal memory inialization sequence
 * @param  hsdram: SDRAM handle
 * @param  Command: Pointer to SDRAM command structure
 * @retval None
 */
void
BSP_SDRAM_Initialization_Sequence( void )
{
    __IO unsigned long tmpmrd = 0;
    FMC_SDRAM_CommandTypeDef cmd;
    FMC_SDRAM_CommandTypeDef * Command = &cmd;

    /* Step 3:  Configure a clock configuration enable command */
    Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber = 1;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand( &hsdram1, Command, 0x1000 );

    /* Step 4: Insert 100 ms delay */
    HAL_Delay( 100 );

    /* Step 5: Configure a PALL (precharge all) command */
    Command->CommandMode = FMC_SDRAM_CMD_PALL;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber = 1;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand( &hsdram1, Command, 0x1000 );

    /* Step 6 : Configure a Auto-Refresh command */
    Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber = 4;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand( &hsdram1, Command, 0x1000 );

    /* Step 7: Program the external memory mode register */
    tmpmrd = ( uint32_t )SDRAM_MODEREG_BURST_LENGTH_2 |
        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL | SDRAM_MODEREG_CAS_LATENCY_3 |
        SDRAM_MODEREG_OPERATING_MODE_STANDARD |
        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber = 1;
    Command->ModeRegisterDefinition = tmpmrd;

    /* Send the command */
    HAL_SDRAM_SendCommand( &hsdram1, Command, 0x1000 );

    /* Step 8: Set the refresh rate counter */
    /* (15.62 us x Freq) - 20 */
    /* Set the device refresh counter */
    HAL_SDRAM_ProgramRefreshRate( &hsdram1, REFRESH_COUNT );
}

void
BSP_SDRAM_Write_Dword( unsigned long address, unsigned long data )
{
    *( __IO unsigned long * )( SDRAM_BANK_ADDR + address ) = data;
}

unsigned long
BSP_SDRAM_Read_Dword( unsigned long address )
{
    return *( __IO unsigned long * )( SDRAM_BANK_ADDR + address );
}

void
BSP_SDRAM_Initliazation( void )
{
    BSP_Print( "\r\n\tSDRAM Initialization" );
  //  BSP_SDRAM_Configuration( );
    BSP_SDRAM_Initialization_Sequence( );


	uint32_t *ptr=(uint32_t*)0xC0000000;
	for(uint32_t i=0;i<0x400000/4;++i) {
		//*ptr=0xFFFFFFFF; //white
		*ptr=0x0;  //black
		++ptr;
	}

}

void
BSP_SDRAM_SingleTest( void )
{
    BSP_Print( "\r\n\tSDRAM Test" );

    for ( unsigned long i = 0; i < RAMSIZE_DWORDS; ++i )
    {
        BSP_SDRAM_Write_Dword( i * 4, 0xFFFFFFFF );
    }

    for ( unsigned long i = 0; i < RAMSIZE_DWORDS; ++i )
    {
        if ( 0xFFFFFFFF != BSP_SDRAM_Read_Dword( i * 4 ) )
        {
            BSP_Print( " - SDRAD error" );
            return;
        }
    }

    BSP_Print( " - OK" );
}
