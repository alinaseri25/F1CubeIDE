/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <iostream>
#include "stm32f1xx_it.h"
#include "DHT22.hpp"
#include "jsonpacket.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */
uint16_t lastPos = 0;
char uartBuffer[300];
uint32_t nextLedTime,readSensor;
HAL_StatusTypeDef uartErr;
DHT22 sensor(dht22_GPIO_Port,dht22_Pin);
Temp_Hum tempHum;
Packets packet;
extern uint8_t rxLineBusy;
JsonDocument inputDoc;
JsonDocument outputDoc;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_IWDG_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void sendProcessControl(Packets *packet);
HAL_StatusTypeDef rs485OneSendData(uint8_t *_data, uint32_t _dataLen);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
		HAL_GPIO_WritePin(rs485En_GPIO_Port, rs485En_Pin, GPIO_PIN_RESET);
		packet.txPacketCompleted();
	}
}

void UartPacket_IdleCallback(UART_HandleTypeDef *huart)
{
	uint8_t *jsonBody = NULL,*payload = NULL;
	uint32_t jsonLen,payloadLen;
	packet.readRxPacket(huart, &jsonBody, &jsonLen, &payload, &payloadLen);
	rxLineBusy = 0;
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);

	if(jsonLen == 0)
	{
		return;
	}

	DeserializationError error = deserializeJson(inputDoc, jsonBody);

	if(!error)
	{
		if(inputDoc["type"].as<int>() == Request)
		{
			if(inputDoc["name"] != "MB")
			{
				return;
			}
			if(inputDoc["req"] == "Time")
			{
				outputDoc["type"] = Response;
				outputDoc["state"] = ok;
				outputDoc["name"] = "MB";
				outputDoc["Time"] = HAL_GetTick()/1000;
				packet.addTxPacket(Low, &outputDoc);
				return;
			}
			else if(inputDoc["req"] == "TH")
			{
				outputDoc["type"] = Response;
				outputDoc["state"] = ok;
				outputDoc["name"] = "MB";
				outputDoc["temp"] = tempHum.Temp;
				outputDoc["hum"] = tempHum.Hum;
				packet.addTxPacket(Medium, &outputDoc);
				return;
			}

			//uint8_t buffer[200];
			//uint16_t len = serializeJson(outputDoc, buffer);


			//rs485OneSendData(packet.readTxPacket()->txBuffer, packet.readTxPacket()->packetLen);
		}
	}
	else
	{
		outputDoc["type"] = Response;
		outputDoc["state"] = nok;
		outputDoc["name"] = "MB";
		outputDoc["temp"] = tempHum.Temp;
		outputDoc["hum"] = tempHum.Hum;
		packet.addTxPacket(Medium, &outputDoc);
	}

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_IWDG_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  __HAL_IWDG_START(&hiwdg);
  HAL_IWDG_Refresh(&hiwdg);
  packet.setRxLoopState(true);
  HAL_UART_Receive_DMA(&huart1, packet.getrxBuffer(), packet.getrxBufferSize());
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  nextLedTime = HAL_GetTick() + 500;
  readSensor = HAL_GetTick() + 2000;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //std::string outputStr,who{"Ali Naseri"};
  //int age{36};
  //double pi{std::numbers::pi};
  //outputStr = std::format("Hello {}, your age is {:05}, pi number is : {: 05.5}",who,age,pi);
  while (1)
  {
	  HAL_IWDG_Refresh(&hiwdg);
	  sendProcessControl(&packet);
	  if(HAL_GetTick() >= readSensor)
	  {
		  tempHum = sensor.DHT_Read();
		  readSensor = HAL_GetTick() + 2000;
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 2500;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, dht22_Pin|LED1_Pin|rs485En_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED0_Pin */
  GPIO_InitStruct.Pin = LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : dht22_Pin */
  GPIO_InitStruct.Pin = dht22_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(dht22_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin rs485En_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|rs485En_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void sendProcessControl(Packets *packet)
{
	TXPacket *currentPacket = packet->readTxPacket();
	if(currentPacket != NULL)
	{
		if(HAL_GetTick() <= currentPacket->timeToSendus || currentPacket->state != WaitToSend)
		{
			return;
		}
		if(rxLineBusy == 0 && (HAL_GPIO_ReadPin(rs485En_GPIO_Port, rs485En_Pin) == GPIO_PIN_RESET))
		{
			currentPacket->state = WaitToSent;
			if(rs485OneSendData(currentPacket->txBuffer, currentPacket->packetLen) != HAL_OK)
			{
				currentPacket->state = WaitToSend;
				packet->packetResetTimer(currentPacket);
			}
		}
		else
		{
			packet->packetResetTimer(currentPacket);
		}
	}
}

HAL_StatusTypeDef rs485OneSendData(uint8_t *_data, uint32_t _dataLen)
{
	if(_data == NULL || _dataLen == 0)
	{
		return HAL_ERROR;
	}
	HAL_GPIO_WritePin(rs485En_GPIO_Port, rs485En_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	return HAL_UART_Transmit_DMA(&huart1,_data,_dataLen);
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM4)
  {
	  if(HAL_GetTick() >= nextLedTime)
	  {
		  nextLedTime = HAL_GetTick() + 500;
		  //HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
//		  sprintf(buffer,"Time",HAL_GetTick()/1000);
//		  uartErr = rs485OneSendData((uint8_t *)buffer, 0);
	  }
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
