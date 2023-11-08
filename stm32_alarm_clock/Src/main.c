/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Arrow Keys mapping
 * UP = 'A' = 65
 * DOWN = 'B' = 66
 * RIGHT = 'C' = 67
 * LEFT = 'D' = 68
 * ENTER = 13
 */
#define UP_KEY  65
#define DOWN_KEY 66
#define RIGHT_KEY 67
#define LEFT_KEY  68
#define SEL_KEY  13

#define LONG_CLICK_MIN 20
#define LONG_CLICK_MAX 50
#define LONG_CLICK_COUNT 30

#define DOUBLE_CLICK_MIN 100
#define DOUBLE_CLICK_MAX 200

#define NORMAL_CLICK_MIN 500

enum CLOCK_MODE{
	NORMAL_STATE,
	TIME_SETTING,
	ALARM_TIME_SETTING,
	MUSIC_SELECT
};

enum CLOCK_BUTTON{
	NO_KEY,
	UP,
	DOWN,
	RIGHT,
	LEFT,
	SEL
};

struct clock_state{
	enum CLOCK_MODE mode;
	enum CLOCK_BUTTON button;
	int music_num;
};

struct clock_state current_state;

typedef struct {
  int8_t hours;
  int8_t minutes;
  int8_t seconds;
}TimeTypeDef;

TimeTypeDef ctime;  // current time
TimeTypeDef stime; // setting time
TimeTypeDef atime;  // alarm time

typedef struct {
  int8_t music_num;
  char music_title[16];
}MusicTypeDef;

MusicTypeDef alarm_music[] =
{
  {0,"Three Bears"},
  {1,"Spring Water"},
  {2,"Bicycle"},
  {3,"Home town"},
  {4,"Mom"},

};

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x08104000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x08108000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0810C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x08110000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x08120000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x08140000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x08160000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x08180000) /* Base @ of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x081A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x081C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x081E0000) /* Base @ of Sector 11, 128 Kbytes */

#define MAGIC_NUM 0xdeadbeef

typedef struct {
  uint32_t magic_num;
  TimeTypeDef setting_time;
  TimeTypeDef alarm_time;
  int8_t alarm_music_num;
}NVitemTypeDef;

#define nv_items  ((NVitemTypeDef *) ADDR_FLASH_SECTOR_23)

NVitemTypeDef default_nvitem =
{
  MAGIC_NUM,
  {0,0,0},
  {0,0,0},
  0
};


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void time_display(void);
void mode_analysis(void);
void stime_setting(void);
void atime_setting(void);
void music_select(void);
HAL_StatusTypeDef update_nvitems(void);
/* USER CODE END PFP */

#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

HAL_StatusTypeDef update_nvitems(void)
{
	uint32_t FirstSector,NbOfSectors,SECTORError;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_StatusTypeDef error= HAL_OK;
    uint32_t Address,i;
    uint64_t Data;
    uint8_t *ptr;

	HAL_FLASH_Unlock();
	FirstSector = FLASH_SECTOR_23;
	NbOfSectors = 1;

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;

	error = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
	if(error != HAL_OK)
	{
		return error;
	}

	ptr = (uint8_t*)&default_nvitem;

	for(i=0;i<sizeof(NVitemTypeDef);i++)
	{
		Address = (uint8_t*)nv_items+i;
		Data = *((uint8_t*)ptr+ i);
		error = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,Address,Data);
		if(error != HAL_OK)
		{
			return error;
		}
	}

	HAL_FLASH_Lock();
}
volatile int timer_count,second,minute,hour;
char line[2][18], time_str[16];
uint8_t key_value;
char uart_buf[30];


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM2)
  {
    if((timer_count%100)==0)
    {
      ctime.seconds++;

      if(ctime.seconds>=60)
      {
    	ctime.minutes++;
    	ctime.seconds = 0;
      }
      if(ctime.minutes>=60)
      {
    	ctime.hours++;
        ctime.minutes = 0;
      }
      if(ctime.hours>=24)
      {
    	ctime.hours = 0;
      }
#if 1
      if(current_state.mode==NORMAL_STATE)
      {
		  time_display();
      }
#endif
    }
	  timer_count++;
  }
}

void time_display(void)
{
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;

  memset(line,0,sizeof(line));

  if(current_state.mode == NORMAL_STATE)
  {
	  sprintf(line[0],"Korea Polytech  \r\n");
	  hours = ctime.hours;
	  minutes = ctime.minutes;
	  seconds = ctime.seconds;
  }
  else if(current_state.mode == TIME_SETTING)
  {
	  sprintf(line[0],"Time Setting   \r\n");
	  hours = stime.hours;
	  minutes = stime.minutes;
	  seconds = stime.seconds;
  }
  else if(current_state.mode == ALARM_TIME_SETTING)
  {
	  sprintf(line[0],"Alarm Setting   \r\n");
	  hours = atime.hours;
	  minutes = atime.minutes;
	  seconds = atime.seconds;
  }


  if(hours>=12)
  {
    strncpy(line[1],"PM ",3);
    sprintf(time_str,"%02d:%02d:%02d",hours-12,minutes,seconds);
  }
  else
  {
    strncpy(line[1],"AM ",3);
    sprintf(time_str,"%02d:%02d:%02d",hours,minutes,seconds);
  }
  strcat(line[1],time_str);

  if(current_state.mode == ALARM_TIME_SETTING)
	  strcat(line[1]," AL \r\n");
  else
	  strcat(line[1],"\r\n");

  HAL_UART_Transmit_IT(&huart3,(uint8_t *)line,sizeof(line));

}

uint32_t last_time,current_time,time_interval, last_time_interval;
uint32_t long_key_count;
/* Arrow Keys mapping
 * UP = 'A' = 65
 * DOWN = 'B' = 66
 * RIGHT = 'C' = 67
 * LEFT = 'D' = 68
 * ENTER = 13
 */
/*
 * Double click : time_interval = 100~200ms
 * Long Key : time_interval = 20~50ms in 30 consecutive times
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if (huart->Instance == USART3)
  {

	current_time = HAL_GetTick();
    time_interval = current_time - last_time;
    last_time = current_time;

    switch(current_state.mode)
    {
      case NORMAL_STATE:
    	  switch(key_value)
    	  {
    	  	  case SEL_KEY:
        		  current_state.button = SEL;
        		  mode_analysis();
    	  		  break;
    	  	  default:  // Arrow keys is Not valid in NORMAL state
    	  		  break;
    	  }
    	  break;
      case TIME_SETTING:
    	  stime_setting();
    	  break;
      case ALARM_TIME_SETTING:
    	  atime_setting();
    	  break;
      case MUSIC_SELECT:
    	  music_select();
    	  break;
      default:
    	  break;
    }
    last_time_interval = time_interval;

	/* Receive one byte in interrupt mode */
	HAL_UART_Receive_IT(&huart3, &key_value, 1);
  }
}

void music_select(void)
{
  int pos, count;

  pos = current_state.music_num;
  count = sizeof(alarm_music)/sizeof(alarm_music[0]);  //total music count

  switch(key_value)
  {
    case UP_KEY:
    	pos++;
    	if(pos==count)
    	{
          pos =0;
    	}
    	break;
    case DOWN_KEY:
    	pos--;
    	if(pos < 0)
    	{
          pos =count-1;
    	}
    	break;
      case SEL_KEY:
	  case RIGHT_KEY:
		  default_nvitem.alarm_music_num = pos;
    	  update_nvitems();
		  current_state.mode = NORMAL_STATE;
		  break;

  }
  current_state.music_num =  pos;

  memset(line,0,sizeof(line));
  if(pos <= count-2)
  {
    sprintf(line[0],"%s <\r\n",alarm_music[pos].music_title);  // "<" mark is selected music title
    sprintf(line[1],"%s \r\n",alarm_music[pos+1].music_title);
  }
  else
  {
    sprintf(line[0],"%s <\r\n",alarm_music[pos].music_title);
    sprintf(line[1],"%s \r\n",alarm_music[0].music_title);
  }

  HAL_UART_Transmit_IT(&huart3,(uint8_t *)line,sizeof(line));

}

void stime_setting(void)
{
static	int position = 0;

	if(position==0)
	{
		switch(key_value)
		{
		  case SEL_KEY:
		  case RIGHT_KEY:
			  position =1;
			  break;
		  case UP_KEY:
		  case DOWN_KEY:
			  if(stime.hours>=12)
				  stime.hours -=12;
			  else
				  stime.hours +=12;
			  break;
		  default:
			  break;
		}
	}
	else if(position==1)
	{
		switch(key_value)
		{
		  case SEL_KEY:
		  case RIGHT_KEY:
			  position =2;
			  break;
		  case LEFT_KEY:
			  position =0;
			  break;
		  case UP_KEY:
			  stime.hours++;
			  if(stime.hours>=12)
				  stime.hours = 0;
			  break;
		  case DOWN_KEY:
			  stime.hours--;
			  if(stime.hours<0)
				  stime.hours = 11;
			  break;
		  default:
			  break;
		}

	}
	else if(position==2)
	{
		switch(key_value)
		{
		  case SEL_KEY:
		  case RIGHT_KEY:
			  position =3;
			  break;
		  case LEFT_KEY:
			  position =1;
			  break;
		  case UP_KEY:
			  stime.minutes++;
			  if(stime.minutes>=60)
				  stime.minutes = 0;
			  break;
		  case DOWN_KEY:
			  stime.minutes--;
			  if(stime.minutes<0)
				  stime.minutes = 59;
			  break;
		  default:
			  break;
		}
	}
	else if(position==3)
	{
		switch(key_value)
		{
		  case SEL_KEY:
		  case RIGHT_KEY:
			  position = 0;
	    	  ctime.hours = stime.hours;
	    	  ctime.minutes = stime.minutes;
	    	  ctime.seconds = stime.seconds;

	    	  default_nvitem.setting_time.hours = stime.hours;
	    	  default_nvitem.setting_time.minutes = stime.minutes;
	    	  default_nvitem.setting_time.seconds = stime.seconds;
	    	  update_nvitems();
			  current_state.mode = NORMAL_STATE;
			  break;
		  case LEFT_KEY:
			  position =2;
			  break;
		  case UP_KEY:
			  stime.seconds++;
			  if(stime.seconds>=60)
				  stime.seconds = 0;
			  break;
		  case DOWN_KEY:
			  stime.seconds--;
			  if(stime.seconds<0)
				  stime.seconds = 59;
			  break;
		  default:
			  break;
		}
	}
//	printf("<%d,%d,%d,%d,%d>\r\n",key_value,position,stime.hours,stime.minutes,stime.seconds);
	time_display();
}

void atime_setting(void)
{
static	int position = 0;

	if(position==0)
	{
		switch(key_value)
		{
//		  case SEL_KEY:
		  case RIGHT_KEY:
			  position =1;
			  break;
		  case UP_KEY:
		  case DOWN_KEY:
			  if(atime.hours>=12)
				  atime.hours -=12;
			  else
				  atime.hours +=12;
			  break;
		  default:
			  break;
		}
	}
	else if(position==1)
	{
		switch(key_value)
		{
//		  case SEL_KEY:
		  case RIGHT_KEY:
			  position =2;
			  break;
		  case LEFT_KEY:
			  position =0;
			  break;
		  case UP_KEY:
			  atime.hours++;
			  if(atime.hours>=12)
				  atime.hours = 0;
			  break;
		  case DOWN_KEY:
			  atime.hours--;
			  if(atime.hours<0)
				  atime.hours = 11;
			  break;
		  default:
			  break;
		}

	}
	else if(position==2)
	{
		switch(key_value)
		{
//		  case SEL_KEY:
		  case RIGHT_KEY:
			  position =3;
			  break;
		  case LEFT_KEY:
			  position =1;
			  break;
		  case UP_KEY:
			  atime.minutes++;
			  if(atime.minutes>=60)
				  atime.minutes = 0;
			  break;
		  case DOWN_KEY:
			  atime.minutes--;
			  if(atime.minutes<0)
				  atime.minutes = 59;
			  break;
		  default:
			  break;
		}
	}
	else if(position==3)
	{
		switch(key_value)
		{
//		  case SEL_KEY:
		  case RIGHT_KEY:
			  position = 0;

			  default_nvitem.alarm_time.hours = atime.hours;
	    	  default_nvitem.alarm_time.minutes = atime.minutes;
	    	  default_nvitem.alarm_time.seconds = atime.seconds;
	    	  update_nvitems();

			  current_state.mode = NORMAL_STATE;
			  break;
		  case LEFT_KEY:
			  position =2;
			  break;
		  case UP_KEY:
			  atime.seconds++;
			  if(atime.seconds>=60)
				  atime.seconds = 0;
			  break;
		  case DOWN_KEY:
			  atime.seconds--;
			  if(atime.seconds<0)
				  atime.seconds = 59;
			  break;
		  default:
			  break;
		}
	}
	printf("[%d,%d,%d,%d,%d]\r\n",key_value,position,atime.hours,atime.minutes,atime.seconds);
	time_display();
}
void mode_analysis(void)
{

    if(time_interval>=LONG_CLICK_MIN && time_interval <=LONG_CLICK_MAX)
    {
    	long_key_count++;
    }
    else if(time_interval>=DOUBLE_CLICK_MIN && time_interval <=DOUBLE_CLICK_MAX)
    {
      if(current_state.mode == NORMAL_STATE)
      {
    	  current_state.mode = MUSIC_SELECT;
          printf("MUSIC_SELECT %ld\r\n",time_interval);
      }
      long_key_count = 0;
    }
    else if(time_interval>= NORMAL_CLICK_MIN)
    {
    	if(current_state.mode == NORMAL_STATE)
    	{
    	  current_state.mode = TIME_SETTING;
    	  stime.hours = ctime.hours;
    	  stime.minutes = ctime.minutes;
    	  stime.seconds = ctime.seconds;
    	  printf("TIME_SETTING  %ld\r\n",time_interval);
    	  time_display();
    	}
      long_key_count = 0;
    }

    if(long_key_count>=LONG_CLICK_COUNT)
    {
      if(current_state.mode == NORMAL_STATE)
      {
        current_state.mode  = ALARM_TIME_SETTING;
        printf("ALARM_TIME_SETTING  %ld\r\n",time_interval);
      }
      long_key_count = 0;
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
  MX_RTC_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_Base_Start_IT(&htim2);

  HAL_UART_Receive_IT(&huart3,&key_value,1);

  current_state.mode = NORMAL_STATE;
  current_state.button = NO_KEY;
  current_state.music_num = 0;

  if(nv_items->magic_num == MAGIC_NUM) //
  {
    memcpy(&default_nvitem,nv_items,sizeof(NVitemTypeDef));
    ctime.hours = default_nvitem.setting_time.hours;
	ctime.minutes = default_nvitem.setting_time.minutes;
	ctime.seconds = default_nvitem.setting_time.seconds;
  }
  else
  {
	update_nvitems();
  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 900;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_MDC_Pin RMII_RXD0_Pin RMII_RXD1_Pin */
  GPIO_InitStruct.Pin = RMII_MDC_Pin|RMII_RXD0_Pin|RMII_RXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_REF_CLK_Pin RMII_MDIO_Pin RMII_CRS_DV_Pin */
  GPIO_InitStruct.Pin = RMII_REF_CLK_Pin|RMII_MDIO_Pin|RMII_CRS_DV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : RMII_TXD1_Pin */
  GPIO_InitStruct.Pin = RMII_TXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD1_Pin */
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_TX_EN_Pin RMII_TXD0_Pin */
  GPIO_InitStruct.Pin = RMII_TX_EN_Pin|RMII_TXD0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
