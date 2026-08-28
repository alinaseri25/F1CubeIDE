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
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_it.h"
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

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart3_rx;

DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
/* USER CODE BEGIN PV */
uint32_t nextLedTime,checkLinkState,sendRequest;
GPIO_PinState currentState;
uint8_t NetBiosName[100];
bool clientState = false;
err_t err;
HAL_StatusTypeDef uartErr;

extern struct netif gnetif;

uint8_t IPAddr[4],NetMask[4],Gateway[4],GIP[4];

extern uint8_t rxLineBusy;
Packets packet;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_IWDG_Init(void);
static void MX_UART5_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void ReadIP(void);
void sendProcessControl(Packets *packet);
HAL_StatusTypeDef rs485OneSendData(uint8_t *_data, uint32_t _dataLen);
//HAL_StatusTypeDef rs485OneSendData(uint8_t *_json, uint32_t _jsonLen = 0, uint8_t *_payload = nullptr, uint32_t _payloadLen = 0);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart3)
	{
		while(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET);
		HAL_GPIO_WritePin(EN_485_U_GPIO_Port, EN_485_U_Pin, GPIO_PIN_RESET);
		packet.txPacketCompleted();
	}
}

void UartPacket_IdleCallback(UART_HandleTypeDef *huart)
{
	uint8_t *jsonBody = NULL,*payload = NULL;
	uint32_t jsonLen,payloadLen;
	packet.readRxPacket(huart, &jsonBody, &jsonLen, &payload, &payloadLen);
	rxLineBusy = 0;

	if(jsonLen == 0 || jsonBody == NULL)
	{
		return;
	}

	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

	JsonDocument inputDoc;
	JsonDocument outputDoc;

	DeserializationError error = deserializeJson(inputDoc, jsonBody);

	if(!error)
	{
		if(inputDoc["type"].as<int>() == Request)
		{
			if(inputDoc["name"] != "LB")
			{
				return;
			}
			if(inputDoc["req"] == "Time")
			{
				outputDoc["type"] = Response;
				outputDoc["state"] = ok;
				outputDoc["name"] = "LB";
				outputDoc["Time"] = HAL_GetTick()/1000;
				packet.addTxPacket(Low, &outputDoc);
			}
			if(inputDoc["req"] == "TH")
			{
				outputDoc["type"] = Response;
				outputDoc["name"] = "LB";
				outputDoc["state"] = nok;
				packet.addTxPacket(Low, &outputDoc);
			}
		}
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
  MX_LWIP_Init();
  MX_UART5_Init();
  MX_USART3_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	__HAL_IWDG_START(&hiwdg);
	HAL_IWDG_Refresh(&hiwdg);
	currentState = GPIO_PIN_RESET;
	sprintf((char*)NetBiosName,"rs485Server");
	netif_set_hostname(&gnetif,(char*)NetBiosName);
	HAL_IWDG_Refresh(&hiwdg);
	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
	//HAL_UART_Receive_IT(&huart3,(uint8_t *)uartBuffer,300);
	packet.setRxLoopState(true);
	HAL_UART_Receive_DMA(&huart3, packet.getrxBuffer(), packet.getrxBufferSize());
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	nextLedTime = HAL_GetTick() + 500;
	checkLinkState = HAL_GetTick() + 1000;
	sendRequest = HAL_GetTick() + 2000;
  while (1)
  {
	  HAL_IWDG_Refresh(&hiwdg);
	  MX_LWIP_Process();
	  sendProcessControl(&packet);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  if(HAL_GetTick() >= sendRequest)
//	  {
//		  sendRequest = HAL_GetTick() + 10;
//		  JsonDocument outputDoc;
//		  outputDoc["type"] = Request;
//		  outputDoc["req"] = "TH";
//		  packet.addTxPacket(High, &outputDoc);
//		  //rs485OneSendData(packet.readTxPacket()->txBuffer, packet.readTxPacket()->packetLen);
//	  }
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
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
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

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
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
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma1_channel1
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* Configure DMA request hdma_memtomem_dma1_channel1 on DMA1_Channel1 */
  hdma_memtomem_dma1_channel1.Instance = DMA1_Channel1;
  hdma_memtomem_dma1_channel1.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma1_channel1.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_memtomem_dma1_channel1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_memtomem_dma1_channel1.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma1_channel1.Init.Priority = DMA_PRIORITY_MEDIUM;
  if (HAL_DMA_Init(&hdma_memtomem_dma1_channel1) != HAL_OK)
  {
    Error_Handler( );
  }

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, EN_485_U_Pin|LED2_Pin|LED3_Pin|EN_485_D_Pin
                          |LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : EN_485_U_Pin LED2_Pin LED3_Pin EN_485_D_Pin
                           LED4_Pin */
  GPIO_InitStruct.Pin = EN_485_U_Pin|LED2_Pin|LED3_Pin|EN_485_D_Pin
                          |LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED0_Pin LED1_Pin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED5_Pin */
  GPIO_InitStruct.Pin = LED5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED5_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void ReadIP(void)
{
	IPAddr[0] = gnetif.ip_addr.addr >> 0;
	IPAddr[1] = gnetif.ip_addr.addr >> 8;
	IPAddr[2] = gnetif.ip_addr.addr >> 16;
	IPAddr[3] = gnetif.ip_addr.addr >> 24;

	NetMask[0] = gnetif.netmask.addr >> 0;
	NetMask[1] = gnetif.netmask.addr >> 8;
	NetMask[2] = gnetif.netmask.addr >> 16;
	NetMask[3] = gnetif.netmask.addr >> 24;

	Gateway[0] = gnetif.gw.addr >> 0;
	Gateway[1] = gnetif.gw.addr >> 8;
	Gateway[2] = gnetif.gw.addr >> 16;
	Gateway[3] = gnetif.gw.addr >> 24;
}

HAL_StatusTypeDef rs485OneSendData(uint8_t *_data, uint32_t _dataLen)
{
	HAL_StatusTypeDef retvalue = HAL_OK;
	if(_data == NULL || _dataLen == 0)
	{
		return HAL_ERROR;
	}
	HAL_GPIO_WritePin(EN_485_U_GPIO_Port, EN_485_U_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	retvalue = HAL_UART_Transmit_DMA(&huart3,_data,_dataLen);
	if(retvalue != HAL_OK)
	{
		HAL_GPIO_WritePin(EN_485_U_GPIO_Port, EN_485_U_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	}
	return retvalue;
}

void sendProcessControl(Packets *packet)
{
	TXPacket *currentPacket = packet->readTxPacket();
	if(currentPacket != NULL)
	{
		if(HAL_GetTick() <= currentPacket->timeToSendus || currentPacket->state != WaitToSend)
		{
			return;
		}
		if(rxLineBusy == 0 && (HAL_GPIO_ReadPin(EN_485_U_GPIO_Port, EN_485_U_Pin) == GPIO_PIN_RESET))
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
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM7)
  {
	if(HAL_GetTick() >= checkLinkState)
	{
	checkLinkState = HAL_GetTick() + 1000;
//  		JsonDocument doc;
//  		doc["type"] = "time";
//  		doc["time"] = HAL_GetTick() / 1000;
//  		serializeJson(doc, buffer);
//  		uartErr = rs485OneSendData((uint8_t *)buffer, 0);
	if (netif_is_link_up(&gnetif))
	{
		HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
	}

	if (netif_is_up(&gnetif))
	{
		HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
		if(IPAddr[0] == 0)
		{
			ReadIP();
		}
	}
	else
	{
		HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);
	}
	}
	if(HAL_GetTick() >= nextLedTime)
	{
		nextLedTime = HAL_GetTick() + 500;
		//HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, currentState);
		//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, currentState);
		//HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, currentState);
		//HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, currentState);
		//HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, currentState);
		//HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, currentState);
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
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
