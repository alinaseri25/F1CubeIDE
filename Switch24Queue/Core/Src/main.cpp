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
#include "74HC595.h"
#include "KeyBoard.h"
#include "dwt_stm32_delay.h"
#include <stdbool.h>
#include "SAVEKEY.h"
#include "LnkList.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	ResetSwitch								= 0x00,
	ResetFactorySwitch						,
	SaveSwitch								,
	SetSwitchPin							,
	SetScenarioPin
}CommandHeader;

typedef struct
{
	CommandHeader							Header;
	uint8_t									SwitchNumber;
	uint8_t									ID;
	KeyNum									KeyNumber;
	HeaderType								KeyType;
	uint32_t								DingDongTime;
	uint8_t									CheckSum;
}CommandSwitch;

typedef enum
{
	ScenarioON								= 0x00,
	ScenarioOFF								,
	ScenarioONOFF
}ScenarioStates;

typedef struct
{
	uint8_t									ID;
	KeyNum									KeyNumber;
	ScenarioStates							State;
}OnScenario;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SwitchLen 24
#define ScenarioLen 10
#define WriteToFlashTime 30000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

IWDG_HandleTypeDef hiwdg;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */
extern uint8_t rxLineBusy;

uint32_t NextWriteToFlash;
RowsCols Rows[5],Cols[5];
MKeyBoard Keyboard;

switchState Pins_Test[SwitchLen];
uint32_t DingDongs[SwitchLen];
OnScenario Pins_Scenario[SwitchLen][ScenarioLen];
uint32_t KeyPress,LastKeyPress,SendStateTime,SendReportSenTime,LEDTime;
SerialPort Port;
SaveKeyProtocol Protocol(&huart1,DIR_GPIO_Port,DIR_Pin,&Port);
bool SendState = false,SendReportSen = false,skeepData = false,senkeys[10];
uint32_t TimeCounter;
LinkedList *LnkLst = new LinkedList();
uint32_t NextSend;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */
void ResetFactory(void);
void DoRelay(switchState *Pins);
void ReportAllKeys(uint8_t ID,HeaderType type);
HAL_StatusTypeDef WriteToFlash(uint8_t _state);
void ReadFromFlash(bool _SaveScenario);
bool changeCurtainState(KeyNum _Key,KeyState _State,uint8_t _ID);
void DataRecieved(SerialPort	*_Port);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UartPacket_IdleCallback(UART_HandleTypeDef *huart)
{
	rxLineBusy = 0;
	Protocol.rxCompleted();
	Protocol.Process();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		Protocol.LineFreed();
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
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  	HAL_IWDG_Refresh(&hiwdg);
  	Protocol.LineFreed();

	ReadFromFlash(true);
	DoRelay(Pins_Test);
	HAL_IWDG_Refresh(&hiwdg);
	Rows[4].GPIO = Row5_GPIO_Port;
	Rows[4].GPIO_Pin = Row5_Pin;
	Rows[3].GPIO = Row4_GPIO_Port;
	Rows[3].GPIO_Pin = Row4_Pin;
	Rows[2].GPIO = Row3_GPIO_Port;
	Rows[2].GPIO_Pin = Row3_Pin;
	Rows[1].GPIO = Row2_GPIO_Port;
	Rows[1].GPIO_Pin = Row2_Pin;
	Rows[0].GPIO = Row1_GPIO_Port;
	Rows[0].GPIO_Pin = Row1_Pin;

	Cols[4].GPIO = Col5_GPIO_Port;
	Cols[4].GPIO_Pin = Col5_Pin;
	Cols[3].GPIO = Col4_GPIO_Port;
	Cols[3].GPIO_Pin = Col4_Pin;
	Cols[2].GPIO = Col3_GPIO_Port;
	Cols[2].GPIO_Pin = Col3_Pin;
	Cols[1].GPIO = Col2_GPIO_Port;
	Cols[1].GPIO_Pin = Col2_Pin;
	Cols[0].GPIO = Col1_GPIO_Port;
	Cols[0].GPIO_Pin = Col1_Pin;

	Keyboard.SetRows(Cols,5);
	Keyboard.SetCols(Rows,5);

	NextSend = HAL_GetTick() + 15;
	SendState = true;
	SendStateTime = HAL_GetTick() + 200;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Row1_Pin|Row2_Pin|Out_13_Pin|Out_5_Pin
                          |Out_20_Pin|Out_19_Pin|Out_18_Pin|Out_6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Row3_Pin|Row4_Pin|Row5_Pin|NET_LED_Pin
                          |Out_17_Pin|Out_16_Pin|Out_15_Pin|Out_14_Pin
                          |Out_7_Pin|Out_8_Pin|Out_9_Pin|Out_10_Pin
                          |Out_11_Pin|Out_12_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Out_4_Pin|Out_3_Pin|Out_2_Pin|Out_1_Pin
                          |STATE_LED_Pin|Out_24_Pin|Out_23_Pin|Out_22_Pin
                          |Out_21_Pin|DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Col1_Pin Col2_Pin Col3_Pin Col4_Pin
                           Col5_Pin */
  GPIO_InitStruct.Pin = Col1_Pin|Col2_Pin|Col3_Pin|Col4_Pin
                          |Col5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Row1_Pin Row2_Pin Out_13_Pin Out_5_Pin
                           Out_20_Pin Out_19_Pin Out_18_Pin Out_6_Pin */
  GPIO_InitStruct.Pin = Row1_Pin|Row2_Pin|Out_13_Pin|Out_5_Pin
                          |Out_20_Pin|Out_19_Pin|Out_18_Pin|Out_6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Row3_Pin Row4_Pin Row5_Pin NET_LED_Pin
                           Out_17_Pin Out_16_Pin Out_15_Pin Out_14_Pin
                           Out_7_Pin Out_8_Pin Out_9_Pin Out_10_Pin
                           Out_11_Pin Out_12_Pin */
  GPIO_InitStruct.Pin = Row3_Pin|Row4_Pin|Row5_Pin|NET_LED_Pin
                          |Out_17_Pin|Out_16_Pin|Out_15_Pin|Out_14_Pin
                          |Out_7_Pin|Out_8_Pin|Out_9_Pin|Out_10_Pin
                          |Out_11_Pin|Out_12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Out_4_Pin Out_3_Pin Out_2_Pin Out_1_Pin
                           STATE_LED_Pin Out_24_Pin Out_23_Pin Out_22_Pin
                           Out_21_Pin DIR_Pin */
  GPIO_InitStruct.Pin = Out_4_Pin|Out_3_Pin|Out_2_Pin|Out_1_Pin
                          |STATE_LED_Pin|Out_24_Pin|Out_23_Pin|Out_22_Pin
                          |Out_21_Pin|DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void ResetFactory(void)
{
	for(uint8_t i = 0 ; i < SwitchLen ; i++)
	{
		Pins_Test[i].ID = (i / 4) + 1;
		Pins_Test[i].KeyNumber = (KeyNum)(Key01 + (i % 4));
		Pins_Test[i].PinState = GPIO_PIN_RESET;
		Pins_Test[i].KeyType = Key;
		Pins_Test[i].virtualKey = true;
		Pins_Test[i].DingDongTime = 0;
		for(uint8_t i2 = 0 ; i2 < ScenarioLen ; i2++)
		{
			Pins_Scenario[i][i2].ID = 0;
			Pins_Scenario[i][i2].KeyNumber = (KeyNum)0;
			Pins_Scenario[i][i2].State = (ScenarioStates)0;
		}
	}

	if(WriteToFlash(1) == HAL_ERROR)
	{
		ResetFactory();
	}
}

void DoRelay(switchState *Pins)
{
	(Pins[0].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_1_GPIO_Port,Out_1_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_1_GPIO_Port,Out_1_Pin,GPIO_PIN_SET);
	(Pins[1].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_2_GPIO_Port,Out_2_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_2_GPIO_Port,Out_2_Pin,GPIO_PIN_SET);
	(Pins[2].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_3_GPIO_Port,Out_3_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_3_GPIO_Port,Out_3_Pin,GPIO_PIN_SET);
	(Pins[3].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_4_GPIO_Port,Out_4_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_4_GPIO_Port,Out_4_Pin,GPIO_PIN_SET);
	(Pins[4].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_5_GPIO_Port,Out_5_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_5_GPIO_Port,Out_5_Pin,GPIO_PIN_SET);
	(Pins[5].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_6_GPIO_Port,Out_6_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_6_GPIO_Port,Out_6_Pin,GPIO_PIN_SET);
	(Pins[6].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_7_GPIO_Port,Out_7_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_7_GPIO_Port,Out_7_Pin,GPIO_PIN_SET);
	(Pins[7].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_8_GPIO_Port,Out_8_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_8_GPIO_Port,Out_8_Pin,GPIO_PIN_SET);
	(Pins[8].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_9_GPIO_Port,Out_9_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_9_GPIO_Port,Out_9_Pin,GPIO_PIN_SET);
	(Pins[9].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_10_GPIO_Port,Out_10_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_10_GPIO_Port,Out_10_Pin,GPIO_PIN_SET);
	(Pins[10].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_11_GPIO_Port,Out_11_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_11_GPIO_Port,Out_11_Pin,GPIO_PIN_SET);
	(Pins[11].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_12_GPIO_Port,Out_12_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_12_GPIO_Port,Out_12_Pin,GPIO_PIN_SET);
	(Pins[12].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_13_GPIO_Port,Out_13_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_13_GPIO_Port,Out_13_Pin,GPIO_PIN_SET);
	(Pins[13].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_14_GPIO_Port,Out_14_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_14_GPIO_Port,Out_14_Pin,GPIO_PIN_SET);
	(Pins[14].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_15_GPIO_Port,Out_15_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_15_GPIO_Port,Out_15_Pin,GPIO_PIN_SET);
	(Pins[15].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_16_GPIO_Port,Out_16_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_16_GPIO_Port,Out_16_Pin,GPIO_PIN_SET);
	(Pins[16].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_17_GPIO_Port,Out_17_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_17_GPIO_Port,Out_17_Pin,GPIO_PIN_SET);
	(Pins[17].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_18_GPIO_Port,Out_18_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_18_GPIO_Port,Out_18_Pin,GPIO_PIN_SET);
	(Pins[18].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_19_GPIO_Port,Out_19_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_19_GPIO_Port,Out_19_Pin,GPIO_PIN_SET);
	(Pins[19].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_20_GPIO_Port,Out_20_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_20_GPIO_Port,Out_20_Pin,GPIO_PIN_SET);
	(Pins[20].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_21_GPIO_Port,Out_21_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_21_GPIO_Port,Out_21_Pin,GPIO_PIN_SET);
	(Pins[21].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_22_GPIO_Port,Out_22_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_22_GPIO_Port,Out_22_Pin,GPIO_PIN_SET);
	(Pins[22].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_23_GPIO_Port,Out_23_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_23_GPIO_Port,Out_23_Pin,GPIO_PIN_SET);
	(Pins[23].PinState == GPIO_PIN_RESET)?HAL_GPIO_WritePin(Out_24_GPIO_Port,Out_24_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(Out_24_GPIO_Port,Out_24_Pin,GPIO_PIN_SET);
}

void ReportAllKeys(uint8_t ID,HeaderType type)
{
	SendState = true;
	HAL_IWDG_Refresh(&hiwdg);
	if(ID == 0 && type == 0x00)
	{
		for(uint8_t _Count = 0 ; _Count < SwitchLen ; _Count++)
		{
			HAL_IWDG_Refresh(&hiwdg);
			if(Pins_Test[_Count].KeyType == Key && Pins_Test[_Count].virtualKey == true)
			{
				//Protocol.SendCommand(Pins_Test[_Count].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ReqOff:ReqOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
				LnkLst->SetCommand(Key,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
			}
			else if(Pins_Test[_Count].KeyType == Curtain && Pins_Test[_Count].virtualKey == true)
			{
				LnkLst->SetCommand(Curtain,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
			}
		}
	}
	else
	{
		for(uint8_t _Count = 0 ; _Count < SwitchLen ; _Count++)
		{
			HAL_IWDG_Refresh(&hiwdg);
			if(Pins_Test[_Count].ID == ID && Pins_Test[_Count].KeyType == type)
			{
				if(Pins_Test[_Count].KeyType == Key && Pins_Test[_Count].virtualKey == true)
				{
					//Protocol.SendCommand(Pins_Test[_Count].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ReqOff:ReqOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
					LnkLst->SetCommand(Key,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
				}
				else if(Pins_Test[_Count].KeyType == Curtain && Pins_Test[_Count].virtualKey == true)
				{
					LnkLst->SetCommand(Curtain,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
				}
			}
		}
	}
	SendState = false;
}

HAL_StatusTypeDef WriteToFlash(uint8_t _state)
{
	uint16_t *RWData;
	uint32_t FlashAddress;
	FLASH_EraseInitTypeDef s_eraseinit;
	uint32_t page_error = 0;

	s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.PageAddress = FLASH_BASE + 0xF800;
  s_eraseinit.NbPages     = 1;

	HAL_IWDG_Refresh(&hiwdg);
	//__disable_irq();
	HAL_FLASH_Unlock();

	if(_state < 1 || _state > 2)
	{
		HAL_FLASH_Lock();
		__enable_irq();
		return HAL_ERROR;
	}

  if (HAL_FLASHEx_Erase(&s_eraseinit, &page_error) == HAL_OK)
	{
		RWData = (uint16_t *)&Pins_Test;
		FlashAddress = FLASH_BASE + 0xF800;
		if(*(__IO uint16_t *)(FlashAddress) != 0xFFFF)
		{
			HAL_FLASH_Lock();
			__enable_irq();
			return HAL_ERROR;
		}
		for(uint32_t _Count = 0 ; _Count < (SwitchLen * sizeof(switchState)) ; _Count+=2,FlashAddress+=2,RWData++)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FlashAddress,*RWData);
			if(*RWData != *(__IO uint16_t *)(FlashAddress))
			{
				HAL_FLASH_Lock();
				__enable_irq();
				return HAL_ERROR;
			}
		}
		if(_state == 2)
		{
			HAL_FLASH_Lock();
			HAL_FLASH_Unlock();
			s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
			s_eraseinit.PageAddress = FLASH_BASE + 0xF400;
			s_eraseinit.NbPages     = 1;
			FlashAddress = FLASH_BASE + 0xF400;
			RWData = (uint16_t *)&Pins_Scenario;
			if (HAL_FLASHEx_Erase(&s_eraseinit, &page_error) == HAL_OK)
			{
				if(*(__IO uint16_t *)(FlashAddress) != 0xFFFF)
				{
					HAL_FLASH_Lock();
					__enable_irq();
					return HAL_ERROR;
				}
				for(uint32_t _Count = 0 ; _Count < (SwitchLen * ScenarioLen * sizeof(OnScenario)) ; _Count+=2,FlashAddress+=2,RWData++)
				{
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,FlashAddress,*RWData);
					if(*RWData != *(__IO uint16_t *)(FlashAddress))
					{
						HAL_FLASH_Lock();
						__enable_irq();
						return HAL_ERROR;
					}
				}
			}
			else
			{
				HAL_FLASH_Lock();
				__enable_irq();
				return HAL_ERROR;
			}
		}
		HAL_FLASH_Lock();
		__enable_irq();
		return HAL_OK;
	}

	HAL_FLASH_Lock();
	__enable_irq();
	return HAL_ERROR;
}

void ReadFromFlash(bool _SaveScenario)
{
	uint16_t *RWData = (uint16_t *)Pins_Test;
	uint32_t FlashAddress;

	FlashAddress = FLASH_BASE + 0xF800 + (0 * sizeof(switchState));
	*RWData = *(__IO uint16_t *)(FlashAddress);

//	if(*RWData == 0xFFFF)
//	{
//		ResetFactory();
//		return;
//	}

	FlashAddress = FLASH_BASE + 0xF800;
	for(uint32_t _Count = 0 ; _Count < (SwitchLen * sizeof(switchState))  ; _Count+=2,RWData++,FlashAddress+=2)
	{
			*RWData = *(__IO uint16_t *)(FlashAddress);
	}
	DoRelay(Pins_Test);
	RWData = (uint16_t *)Pins_Scenario;
	FlashAddress = FLASH_BASE + 0xF400;
	if(_SaveScenario == true)
	{
		for(uint32_t _Count = 0 ; _Count < (SwitchLen * ScenarioLen * sizeof(OnScenario))  ; _Count+=2,RWData++,FlashAddress+=2)
		{
				*RWData = *(__IO uint16_t *)(FlashAddress);
		}
	}
}

bool changeCurtainState(KeyNum _Key,KeyState _State,uint8_t _ID)
{
	bool retvalue = true;
	if(_State == ReqOn || _State == ResOn)
	{
		KeyNum open,close,stop;
		if(_Key == Open1 || _Key == Close1)
		{
			open = Open1;
			stop = Stop1;
			close = Close1;
		}
		else if(_Key >= Open2 && _Key <= Close2)
		{
			open = Open2;
			stop = Stop2;
			close = Close2;
		}
		else if(_Key >= Open3 && _Key <= Close3)
		{
			open = Open3;
			stop = Stop3;
			close = Close3;
		}
		for(uint8_t i = 0 ; i < SwitchLen ; i++)
		{
			if((Pins_Test[i].KeyNumber == open || Pins_Test[i].KeyNumber == stop || Pins_Test[i].KeyNumber == close) && Pins_Test[i].ID == _ID)
			{
				(Pins_Test[i].KeyNumber == _Key)?(Pins_Test[i].PinState = GPIO_PIN_SET):(Pins_Test[i].PinState = GPIO_PIN_RESET);
				if(Pins_Test[i].virtualKey == true)
				{
					//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
					LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
				}
			}
		}
	}
	else if(_State == ReqOff || _State == ResOff)
	{
		for(uint8_t i = 0 ; i < SwitchLen ; i++)
		{
			if((Pins_Test[i].KeyNumber == _Key) && Pins_Test[i].ID == _ID)
			{
				Pins_Test[i].PinState = GPIO_PIN_RESET;
				if(Pins_Test[i].virtualKey == true)
				{
					//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
					LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
				}
			}
		}
	}
	return retvalue;
}

void DataRecieved(SerialPort	*_Port)
{
	ThermostatHeader *ThermostateData;
	KeyHeader	*KeyHeaderString;
	ScenarioHeader *ScenarioHeaderString;
	CommandSwitch	*Cmdsw;
	//GetState *State;
	switch(_Port->Len)
	{
		case sizeof(CommandSwitch):
			Cmdsw = (CommandSwitch *)_Port->Buffer;
			if(Cmdsw->Header == ResetFactorySwitch && Cmdsw->ID == 254 && Cmdsw->CheckSum == (254 + ResetFactorySwitch) && Cmdsw->KeyNumber == 0 && Cmdsw->KeyType == 0 && Cmdsw->SwitchNumber == 0 && Cmdsw->DingDongTime == 0)
			{
				ResetFactory();
			}
			else if(Cmdsw->Header == ResetSwitch && Cmdsw->ID == 254 && Cmdsw->CheckSum == (254 + ResetSwitch) && Cmdsw->KeyNumber == 0 && Cmdsw->KeyType == 0 && Cmdsw->SwitchNumber == 0 && Cmdsw->DingDongTime == 0)
			{
				HAL_NVIC_SystemReset();
			}
			else if(Cmdsw->Header == SetScenarioPin)
			{
				Pins_Scenario[Cmdsw->SwitchNumber][Cmdsw->KeyType - 1].ID = Cmdsw->ID;
				Pins_Scenario[Cmdsw->SwitchNumber][Cmdsw->KeyType - 1].KeyNumber = Cmdsw->KeyNumber;
				Pins_Scenario[Cmdsw->SwitchNumber][Cmdsw->KeyType - 1].State = (ScenarioStates)Cmdsw->CheckSum;
			}
			else if(Cmdsw->Header == SetSwitchPin)
			{
				Pins_Test[Cmdsw->SwitchNumber].ID = Cmdsw->ID;
				Pins_Test[Cmdsw->SwitchNumber].KeyNumber = Cmdsw->KeyNumber;
				Pins_Test[Cmdsw->SwitchNumber].KeyType = Cmdsw->KeyType;
				Pins_Test[Cmdsw->SwitchNumber].DingDongTime = Cmdsw->DingDongTime;
				if((Cmdsw->CheckSum & 0x01) == 0x01)
				{
					Pins_Test[Cmdsw->SwitchNumber].virtualKey = true;
				}
				else
				{
					Pins_Test[Cmdsw->SwitchNumber].virtualKey = false;
				}
			}
			else if(Cmdsw->Header == SaveSwitch && Cmdsw->ID == 254 && Cmdsw->CheckSum == (uint8_t)(254 + SaveSwitch) && Cmdsw->KeyNumber == 0 && Cmdsw->KeyType == 0 && Cmdsw->SwitchNumber == 0)
			{
				if(WriteToFlash(2) == HAL_ERROR)
				{
					WriteToFlash(2);
				}
			}
		break;
		case 4:
			//State = (GetState *)_Port->Buffer;
			/*if(Port.Buffer[0] == 0x11 && Port.Buffer[1] == 0x45)
			{
				SendState = true;
				SendStateTime = HAL_GetTick() + 20;
			}*/
			//ReportAllKeys(State->ID,State->header);
		break;
		case sizeof(KeyHeader):
			KeyHeaderString = (KeyHeader *)_Port->Buffer;
			if(KeyHeaderString->checksum==(uint8_t)(KeyHeaderString->header + KeyHeaderString->ID + KeyHeaderString->KeySelect + KeyHeaderString->state))
			{
				if(KeyHeaderString->header == Curtain && (KeyHeaderString->KeySelect >= Open1 && KeyHeaderString->KeySelect <= Close3))
				{
					changeCurtainState(KeyHeaderString->KeySelect,KeyHeaderString->state,KeyHeaderString->ID);
					DoRelay(Pins_Test);
					//WriteToFlash();
					NextWriteToFlash = HAL_GetTick() + WriteToFlashTime;
				}
				//-------------------------------------------------------------------------------Keys Control
				if (KeyHeaderString->header == Key && KeyHeaderString->KeySelect >= Key01)
				{
					for(uint8_t i=0 ; i < SwitchLen ; i++)
					{
						if(KeyHeaderString->ID == Pins_Test[i].ID && KeyHeaderString->KeySelect == Pins_Test[i].KeyNumber && Pins_Test[i].KeyType == KeyHeaderString->header)
						{
							if(KeyHeaderString->state == ResOn || KeyHeaderString->state == ReqOn)
							{
								Pins_Test[i].PinState = GPIO_PIN_SET;
								if(Pins_Test[i].PinState == GPIO_PIN_SET && Pins_Test[i].DingDongTime != 0)
								{
									DingDongs[i] = Pins_Test[i].DingDongTime + HAL_GetTick();
								}
								if(Pins_Test[i].virtualKey == true)
								{
									//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
									LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
								}
							}
							else
							{
								Pins_Test[i].PinState = GPIO_PIN_RESET;
								if(Pins_Test[i].virtualKey == true)
								{
									//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
									LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
								}
							}
						}
					}
					DoRelay(Pins_Test);
					//WriteToFlash();
					NextWriteToFlash = HAL_GetTick() + WriteToFlashTime;
				}
				else if ((KeyHeaderString->header == Key || KeyHeaderString->header == Curtain) && KeyHeaderString->KeySelect <= Key01)
				{
					for(uint8_t i=0 ; i < SwitchLen ; i++)
					{
						for(uint8_t j=0 ; j < ScenarioLen ; j++)
						{
							if(Pins_Scenario[i][j].ID == KeyHeaderString->ID && Pins_Scenario[i][j].KeyNumber == KeyHeaderString->KeySelect && Pins_Test[i].KeyType == Key)
							{
								if(KeyHeaderString->state == ResOn || KeyHeaderString->state == ReqOn)
								{
									if(Pins_Scenario[i][j].State == ScenarioON || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										Pins_Test[i].PinState = GPIO_PIN_SET;
										if(Pins_Test[i].virtualKey == true)
										{
											//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
											LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
										}
									}
								}
								else
								{
									if(Pins_Scenario[i][j].State == ScenarioOFF || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										Pins_Test[i].PinState = GPIO_PIN_RESET;
										if(Pins_Test[i].virtualKey == true)
										{
											//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
											LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
										}
									}
								}
							}
							else if(Pins_Scenario[i][j].ID == KeyHeaderString->ID && Pins_Scenario[i][j].KeyNumber == KeyHeaderString->KeySelect  && Pins_Test[i].KeyType == Curtain)
							{
								if(KeyHeaderString->state == ResOn || KeyHeaderString->state == ReqOn)
								{
									if(Pins_Scenario[i][j].State == ScenarioON || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										changeCurtainState(Pins_Test[i].KeyNumber,KeyHeaderString->state,Pins_Test[i].ID);
									}
								}
								else
								{
									if(Pins_Scenario[i][j].State == ScenarioOFF || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										changeCurtainState(Pins_Test[i].KeyNumber,KeyHeaderString->state,Pins_Test[i].ID);
									}
								}
							}
						}
					}
					DoRelay(Pins_Test);
					//WriteToFlash();
					NextWriteToFlash = HAL_GetTick() + WriteToFlashTime;
				}

			}
		break;
		case sizeof(ScenarioHeader):
			ScenarioHeaderString = (ScenarioHeader *)_Port->Buffer;
			if(ScenarioHeaderString->checksum==(uint8_t)(ScenarioHeaderString->header + ScenarioHeaderString->state + ScenarioHeaderString->ID + ScenarioHeaderString->KeySelect + ScenarioHeaderString->Fixed0 + ScenarioHeaderString->ID_Replay + ScenarioHeaderString->KeySelect_Replay))
			{
				if(ScenarioHeaderString->header == Key)
				{
					for(uint8_t i=0 ; i < SwitchLen ; i++)
					{
						for(uint8_t j=0 ; j < ScenarioLen ; j++)
						{
							if(Pins_Scenario[i][j].ID == ScenarioHeaderString->ID && Pins_Scenario[i][j].KeyNumber == ScenarioHeaderString->KeySelect  && Pins_Test[i].KeyType == Key)
							{
								if(ScenarioHeaderString->state == ResOn || ScenarioHeaderString->state == ReqOn)
								{
									if(Pins_Scenario[i][j].State == ScenarioON || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										Pins_Test[i].PinState = GPIO_PIN_SET;
										if(Pins_Test[i].virtualKey == true)
										{
											//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
											LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
										}
									}
								}
								else
								{
									if(Pins_Scenario[i][j].State == ScenarioOFF || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										Pins_Test[i].PinState = GPIO_PIN_RESET;
										if(Pins_Test[i].virtualKey == true)
										{
											//Protocol.SendCommand(Pins_Test[i].KeyType,(Pins_Test[_Count].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[_Count].ID,Pins_Test[_Count].KeyNumber);
											LnkLst->SetCommand(Pins_Test[i].KeyType,(Pins_Test[i].PinState == GPIO_PIN_RESET)?ResOff:ResOn,Pins_Test[i].ID,Pins_Test[i].KeyNumber);
										}
									}
								}
							}
							else if(Pins_Scenario[i][j].ID == ScenarioHeaderString->ID && Pins_Scenario[i][j].KeyNumber == ScenarioHeaderString->KeySelect  && Pins_Test[i].KeyType == Curtain)
							{
								if(ScenarioHeaderString->state == ResOn || ScenarioHeaderString->state == ReqOn)
								{
									if(Pins_Scenario[i][j].State == ScenarioON || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										changeCurtainState(Pins_Scenario[i][j].KeyNumber,ScenarioHeaderString->state,Pins_Scenario[i][j].ID);
									}
								}
								else
								{
									if(Pins_Scenario[i][j].State == ScenarioOFF || Pins_Scenario[i][j].State == ScenarioONOFF)
									{
										changeCurtainState(Pins_Scenario[i][j].KeyNumber,ScenarioHeaderString->state,Pins_Scenario[i][j].ID);
									}
								}
							}
						}
					}
					DoRelay(Pins_Test);
					//WriteToFlash();
					NextWriteToFlash = HAL_GetTick() + WriteToFlashTime;
				}
			}
		break;
		case sizeof(ThermostatHeader):
			ThermostateData = (ThermostatHeader *)_Port->Buffer;
			if(ThermostateData->header == Thermostat )
			{
				for(uint8_t i = 0 ; i < SwitchLen ; i ++)
				{
					if(ThermostateData->ID == Pins_Test[i].ID && Pins_Test[i].KeyNumber == Valve1)
					{
						Pins_Test[i].PinState = (GPIO_PinState)(ThermostateData->FanSpeed_ValveState & 0x0F);
					}
				}
				ThermostateData->FanSpeed_ValveState = ThermostateData->FanSpeed_ValveState >> 4;
				if(ThermostateData->Fanstate != Auto)
				{
					ThermostateData->FanSpeed_ValveState = ThermostateData->Fanstate;
				}
				ThermostateData->FanSpeed_ValveState += Valve1;
				for(uint8_t i = 0 ; i < SwitchLen ; i ++)
				{
					if(ThermostateData->ID == Pins_Test[i].ID && Pins_Test[i].KeyNumber != Valve1)
					{
						Pins_Test[i].PinState = GPIO_PIN_RESET;
					}
					if(ThermostateData->ID == Pins_Test[i].ID && Pins_Test[i].KeyNumber  == ThermostateData->FanSpeed_ValveState)
					{
						Pins_Test[i].PinState = GPIO_PIN_SET;
					}
				}
				DoRelay(Pins_Test);
				//WriteToFlash();
				NextWriteToFlash = HAL_GetTick() + WriteToFlashTime;
			}
		break;
		default:
		break;
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
