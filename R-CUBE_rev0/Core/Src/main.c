/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "printf.h"
#include "stm32hpmlib.h"
#include "stm32_tm1637.h"
#include "shtc1.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//TEST_HPM        TEST_TMP1637          TEST_I2C_BME680
#define TEST_I2C_SHTC1
#define TEST_HPM
#define TEST_TM1637

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint32_t time_is_runing;
int pm2,pm10,resultUartSet,resultStartMeasure,resultRead,resultStopMeasure;
int32_t temperature, humidity;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  
  /* USER CODE BEGIN 2 */

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  
  

  #ifdef TEST_HPM
  hpmSetup();
  #endif

  #ifdef TEST_TM1637
  volatile uint32_t counter=0;
  tm1637Init();
  tm1637SetBrightness(3);
  tm1637DisplayDecimal(1234, 1);
  #endif

  #ifdef TEST_I2C_SHTC1
    HAL_GPIO_WritePin(EN_1V8_GPIO_Port,EN_1V8_Pin,GPIO_PIN_SET);
    sensirion_i2c_init();

    #endif
    int32_t measurementValue[4];
  while (1)
  {
    
    /*HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
    HAL_Delay(250);
    HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin);
    HAL_Delay(250);*/
    #ifdef TEST_I2C_SHTC1
    
    
    //printf_("Communication is working %d\n", time_is_runing);
   

	  
	          /* Measure temperature and relative humidity and store into variables
	           * temperature, humidity (each output multiplied by 1000).
	           */
	          int8_t ret = shtc1_measure_blocking_read(&temperature, &humidity);
	          if (ret == STATUS_OK) {
	              printf_("measured temperature: %0.1f degreeCelsius, "
	                     "measured humidity: %0.1f percentRH\n\n",
	                     temperature / 1000.0f, humidity / 1000.0f);
	          } else {
	              printf_("error reading measurement\n");
	          }

	          sensirion_sleep_usec(1000000);

    #endif
    
    
    

    #ifdef TEST_HPM
    resultRead = hpmReadResults(&pm2,&pm10);  
    if( resultRead == 0)
      //printf_("Counter %d result set uart: %d result set measure %d result read %d stop measure %d\n",time_is_runing, resultUartSet, resultStartMeasure, resultRead,resultStopMeasure);
    printf_("PM2.5: %d PM10: %d \n\n",pm2,pm10);
    //}

    #endif

    measurementValue[0]=pm2;
    measurementValue[1]=pm10;
    measurementValue[2]=temperature/100;
    measurementValue[3]=humidity/100;


    #ifdef TEST_TM1637
    
    

    (counter == 0) || (counter == 1) ? tm1637DisplayDecimal(measurementValue[counter], 0) : tm1637DisplayDecimal(measurementValue[counter], 1);

    HAL_Delay(500);
    

    if( (time_is_runing%4) == 0 ){
      
	    counter++;
      
      
    }
    
    if(counter == 4) counter=0;
    
    time_is_runing++;
    

    #endif
    

    
    
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
