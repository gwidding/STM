/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"
#include "dac.h"
#include "eth.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
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

/* USER CODE BEGIN PV */
RTC_DateTypeDef sDate;
RTC_TimeTypeDef sTime;
char showTime[30] = {0};
char showDate[30] = {0};
char ampm[2][3] = {"AM", "PM"};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart3, (uint8_t *)ptr, len, 500);
	return len;
}

int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 1000);
	return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay(uint16_t time) {
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim2)) < time);
}

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

float Temperature = 0;
float Humidity = 0;
uint8_t Presence = 0;

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

#define DHT11_PORT GPIOC
#define DHT11_PIN GPIO_PIN_0

void DHT11_Start (void)
{
	Set_Pin_Output(DHT11_PORT, DHT11_PIN);  // set the pin as output
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	delay (18000);   // wait for 18ms
	Set_Pin_Input(GPIOC, DHT11_Pin);    // set as input
}
uint8_t DHT11_Check_Response (void) {
	uint8_t Response = 0;
	delay(40);
	if (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))) {
		delay(80);
		if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
		else Response = -1; // 255
	}
	while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));

	return Response;
}

uint8_t DHT11_Read(void) {
	uint8_t i, j;
	for (j = 0; j < 8; j++) {
		while (!(HAL_GPIO_ReadPin(GPIOC, DHT11_PIN)));
		delay(20);
		if (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))) // if the pin is low
		{
			i &= ~(1 << (7 - j));   // write 0
		} else
			i |= (1 << (7 - j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));
	}
	return i;
}

#define LCD_ADDR (0x27 << 1)

void Display_Temp (float Temp) {
	char str[20] = {0};
	sprintf(str, "TEMP:- %.2f C", Temp);
	LCD_SendCommand(LCD_ADDR, 0b10000000);
	LCD_SendString(LCD_ADDR, str);
}

void Display_Rh (float Rh) {
	char str[20] = {0};
	sprintf(str, "RH:- %.2f %%", Rh);
	LCD_SendCommand(LCD_ADDR, 0b11000000);
	LCD_SendString(LCD_ADDR, str);
}
void get_time(void) {
//	RTC_DateTypeDef sDate;
//	RTC_TimeTypeDef sTime;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	sprintf((char *)showTime, "%s %02d : %02d : %02d", ampm[sTime.TimeFormat], sTime.Hours, sTime.Minutes, sTime.Seconds);
	sprintf((char *)showDate, "%04d-%02d-%02d", 2000 + sDate.Year, sDate.Month, sDate.Date);
}

//void set_time(uint8_t hh, uint8_t mm, uint8_t ss) {
//	RTC_TimeTypeDef sTime;
//
//	sTime.Hours = hh; // set hours
//	sTime.Minutes = mm; // set minutes
//	sTime.Seconds = ss; // set seconds
//
//	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
//}

void set_Date(uint8_t ww, uint8_t mm, uint8_t dd, uint8_t yy) {
	RTC_DateTypeDef sDate;

	sDate.WeekDay = ww; // date RTC_WEEKDAY_THURSDAY
	sDate.Month = mm; // month RTC_MONTH_FEBRUARY
	sDate.Date = dd; // date
	sDate.Year = yy; // year

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}
extern void I2C_Scan(void);
void init();
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
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_RTC_Init();
  MX_TIM2_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
//  set_time(11, 59, 50);
  set_Date(RTC_WEEKDAY_MONDAY, 10, 27, 23);
//	init();


	I2C_Scan();
	LCD_Init(LCD_ADDR);
	int setmode = 0;

	uint32_t current_tick_1;
	uint32_t current_tick_2;
	uint32_t old_tick_1;
	uint32_t old_tick_2;

	void setModeCheck(int n) {
		if (n == 1) {
			HAL_GPIO_WritePin(GPIOB, LD1_Pin, 1);
			HAL_GPIO_WritePin(GPIOB, LD2_Pin, 0);
			HAL_GPIO_WritePin(GPIOB, LD3_Pin, 0);
		}
		else if (n == 2) {
			HAL_GPIO_WritePin(GPIOB, LD1_Pin, 0);
			HAL_GPIO_WritePin(GPIOB, LD2_Pin, 1);
			HAL_GPIO_WritePin(GPIOB, LD3_Pin, 0);
		}
		else if (n == 3) {
			HAL_GPIO_WritePin(GPIOB, LD1_Pin, 0);
			HAL_GPIO_WritePin(GPIOB, LD2_Pin, 0);
			HAL_GPIO_WritePin(GPIOB, LD3_Pin, 1);
		}
		else {
			HAL_GPIO_WritePin(GPIOB, LD3_Pin, 0);
		}
	}
	HAL_TIM_Base_Start(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		Display_Temp(Temperature);
		Display_Rh(Humidity);

		/**************** DHT 11 *********************/
		DHT11_Start();
		Presence = DHT11_Check_Response();
		Rh_byte1 = DHT11_Read();
		Rh_byte2 = DHT11_Read();
		Temp_byte1 = DHT11_Read();
		Temp_byte2 = DHT11_Read();
		SUM = DHT11_Read();

		TEMP = Temp_byte1;
		RH = Rh_byte1;

		Temperature = (float) TEMP;
		Humidity = (float) RH;

		HAL_Delay(1000);

	setModeCheck(setmode);
	get_time();
	// set address to 0x00
	LCD_SendCommand(LCD_ADDR, 0b10000000);
	LCD_SendString(LCD_ADDR, showDate);

	// set address to 0x40
	LCD_SendCommand(LCD_ADDR, 0b11000000);
	LCD_SendString(LCD_ADDR, showTime);


	if (HAL_GPIO_ReadPin(GPIOF, setBtn_Pin) == 0) {
		current_tick_1 = HAL_GetTick();
	}
	if (HAL_GPIO_ReadPin(GPIOF, upBtn_Pin) == 0) {
		current_tick_2 = HAL_GetTick();
	}

	if (HAL_GPIO_ReadPin(GPIOF, setBtn_Pin) == 0 && (current_tick_1 - old_tick_1 > 300) ) {
		  setmode++;
		  if (setmode == 4) setmode = 0;
		  old_tick_1 = current_tick_1;
	}

	if (setmode == 0) {
		HAL_Delay(1000);
	}
	else if (HAL_GPIO_ReadPin(GPIOF, upBtn_Pin) == 0 && (current_tick_2 - old_tick_2 > 300)) {
		if (setmode == 1) sTime.Hours = (sTime.Hours + 1) % 13;
		if (setmode == 2) sTime.Minutes = (sTime.Minutes + 1) % 60;
		if (setmode == 3) sTime.Seconds = (sTime.Seconds + 1) % 60;

		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		old_tick_2 = current_tick_2;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* RTC_Alarm_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
