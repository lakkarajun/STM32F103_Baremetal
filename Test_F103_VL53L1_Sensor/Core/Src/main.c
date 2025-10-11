/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @Author         : Nagaraju Lakkaraju
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "main.h"
#include <string.h>
#include <stdio.h>
#include "vl53l1_api.h"
#include "vl53l1_platform_user_data.h"

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

void USART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        USART1_SendChar(ptr[i]);
    }
    return len;
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  printf("%s(): STM32F103 Bluepill with VL53L1x Sensor Test \r\n", __func__);
  VL53L1_Dev_t dev;
  VL53L1_DEV Dev = &dev;
  Dev->i2c_slave_address = 0x52; // 0x29 << 1
  VL53L1_Error status;
  VL53L1_RangingMeasurementData_t result;

  char buf[64];
  uint8_t isDataReady = 0;

  /* Initialize sensor */
  status = VL53L1_DataInit(Dev);
  if (status) {
	  printf("%s(): DataInit failed\r\n", __func__);
	  while(1);
  }
  printf("%s(): DataInit: Done\r\n", __func__);

  status = VL53L1_StaticInit(Dev);
  if (status) {
	  printf("%s(): StaticInit failed\r\n", __func__);
	  while(1);
  }
  printf("%s(): StaticInit: Done\r\n", __func__);

  status = VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_SHORT);
  status = VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 20000); // 20ms
  status = VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, 25); // >20ms + 4ms
  status = VL53L1_StartMeasurement(Dev);
  if (status) {
	  printf("%s(): Start Measurement failed\r\n", __func__);
	  while(1);
  }
  printf("%s(): Start Measurement: Done\r\n", __func__);

  while (1) {
	  VL53L1_GetMeasurementDataReady(Dev, &isDataReady);
	  if (isDataReady) {
		  status = VL53L1_GetRangingMeasurementData(Dev, &result);
	      if (status == VL53L1_ERROR_NONE) {
	    	  snprintf(buf, sizeof(buf), "Status: %d, Distance: %d mm, Signal: %.2ld MCPS\r\n",
	    			  result.RangeStatus, result.RangeMilliMeter, result.SignalRateRtnMegaCps);
	          printf("%s", buf);
	      } else {
	    	  snprintf(buf, sizeof(buf), "GetData failed: %d\r\n", status);
	    	  printf("%s", buf);
	      }

	      VL53L1_ClearInterruptAndStartMeasurement(Dev);
	  }

	  HAL_Delay(500); // Adjust based on timing
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
