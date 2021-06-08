/**
  ******************************************************************************
  * File Name          : STM32TouchController.cpp
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* USER CODE BEGIN STM32TouchController */

#include "STM32TouchController.hpp"
#include "main.h"
#include "i2c.h"

volatile uint8_t TouchINT = 0;

void STM32TouchController::init()
{

	HAL_GPIO_WritePin( LCD_TPRST_GPIO_Port, LCD_TPRST_Pin, GPIO_PIN_SET );
	HAL_Delay(10);
	HAL_GPIO_WritePin( LCD_TPRST_GPIO_Port, LCD_TPRST_Pin, GPIO_PIN_RESET );
	HAL_Delay(10);
	HAL_GPIO_WritePin( LCD_TPRST_GPIO_Port, LCD_TPRST_Pin, GPIO_PIN_SET );
	HAL_Delay(1000);

	  /* EXTI interrupt init*/
	  /* EXTI interrupt init*/
	  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 7, 0);
	  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    /**
     * Initialize touch controller and driver
     *
     */
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{


    if (TouchINT)




   // if ( !HAL_GPIO_ReadPin( LCD_TPINT_GPIO_Port, LCD_TPINT_Pin ) )
    {
       int32_t X;
       int32_t Y;

   	TouchINT = 0;
        uint8_t rx_buf[64] = {0};
        uint8_t tx_buf[4];
        tx_buf[0] = 1;

        if ( HAL_OK !=  HAL_I2C_Mem_Read(&hi2c1, ( 0x41 << 1 ), 0x10, 0, rx_buf, 64, 100))
        	{
        		return false;
        	}
        if (rx_buf[0] == 0x48)
        {
        	X = 0;
        	X = rx_buf[3] & 0x0F;
        	X <<= 8;
        	X |= rx_buf[2];
        	Y = 0;
        	Y = rx_buf[5] & 0x0F;
        	Y <<= 8;
        	Y |= rx_buf[4];
        	*(int32_t*)&x = X;
        	*(int32_t*)&y = Y;
        }
        else
        {
        	return false;
        }


        	return true;

    }

    /**
     * By default sampleTouch returns false,
     * return true if a touch has been detected, otherwise false.
     *
     * Coordinates are passed to the caller by reference by x and y.
     *
     * This function is called by the TouchGFX framework.
     * By default sampleTouch is called every tick, this can be adjusted by HAL::setTouchSampleRate(int8_t);
     *
     */
    return false;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  if (GPIO_Pin == LCD_TPINT_Pin)
  {
	  TouchINT = 1;

  }
}

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
