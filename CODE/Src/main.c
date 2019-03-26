
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "WS28.h"
#include "usercmd.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
                         
uint8_t ReceiveBuff[BUFFERSIZE];		//接收缓冲区
uint8_t recv_end_flag = 0,Rx_len;		//接收完成中断，接收到字符长度
uint8_t User_Run_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void WriteFlash(uint8_t addrnum,uint32_t data);
uint32_t ReadFlash(uint8_t addrnum);
void uartdamget(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */


uint32_t tim16,lasttim16;
uint8_t Color_Cnt;
uint8_t Work_Mode = Mode_Sleep;
uint8_t Sleep_State = Sleep_End;
uint32_t color_temp = 0;
uint32_t sys_color[3] = {RED,GREEN,BLUE};

//uint32_t WriteFlashData = 0x55555555;
//uint32_t ReadFlashData[4] ;
//uint32_t FlashAddr[3] = {0x08003400,0x08003800,0x08003C00};//0x08003000

#define FLASH_USER_START_PAGE 	 0x08003C00
#define FLASH_USER_SIZE					 5


#define Sleep_Min   						 10	//minutes
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

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
	MX_SPI1_Init();
	Sw28_Init(&hsw28,PIXELS_LEN);
	MX_TIM16_Init();
	MX_USART1_UART_Init();
	
	//colorWipe(&hsw28,BLACK,10);
	/* Initialize interrupts */
	MX_NVIC_Init();
	/* USER CODE BEGIN 2 */
	//Sw28_Init(&hsw28,PIXELS_LEN);
	HAL_TIM_Base_Start_IT(&htim16);

	/*        使能串口1 IDLE中断        */
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	User_CMD_Init();
	uint8_t userdata_flag = 0;
	for(uint8_t i=0;i<3;i++)
	{
		sys_color[i] = ReadFlash(i);
		if(sys_color[i] == 0xFFFFFFFF)
		{
			sys_color[i] = RED;
			userdata_flag = 1;
		}
		else
		{
			userdata_flag = 0;
		}
	}
	if(userdata_flag == 1)
	{
		Show_Firstdown_inf();
	}
	
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
	/* USER CODE END WHILE */
		uartdamget();
	/* USER CODE BEGIN 3 */
		if(Work_Mode == Mode_Bright)
		{
			colorWipe(&hsw28,sys_color[Color_Cnt-1],10);
		}
		else if(Work_Mode == Mode_Debug)
		{
			colorWipe(&hsw28,color_temp,10);
		}
		else if(Work_Mode == Mode_Breathe)
		{
			if(lasttim16 != tim16)
			{
				lasttim16 = tim16;
				Breathe_Show();		
			}
		}
		else
		{
			colorWipe(&hsw28,BLACK,10);
		}
//		if(Sleep_State==Sleep_Begin)
//		{
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_SET);
//		}
//		else
//		{
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
//		}
	}
	/* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI0_1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
  /* TIM16_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM16_IRQn);
}

/* USER CODE BEGIN 4 */
void WriteFlash(uint8_t addrnum,uint32_t data)
{
	uint8_t i;
  uint32_t FlashDataTemp[FLASH_USER_SIZE]={0x00000000}; 
	for(i=0;i<FLASH_USER_SIZE;i++)
		FlashDataTemp[i] = ReadFlash(i);
	FlashDataTemp[addrnum] = data;
	
	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef f;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = FLASH_USER_START_PAGE;
	f.NbPages = 1;

	uint32_t PageError = 0;

	HAL_FLASHEx_Erase(&f, &PageError);
	
	for(i=0;i<FLASH_USER_SIZE;i++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_USER_START_PAGE+4*i, FlashDataTemp[i]);

	HAL_FLASH_Lock();
}

uint32_t ReadFlash(uint8_t addrnum)
{
	uint32_t temp = *(__IO uint32_t*)(FLASH_USER_START_PAGE+4*addrnum);
	return temp;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_0)
  {
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0));
		if(Work_Mode==Mode_Bright)
		{
			Sleep_State=Sleep_Begin;
			Work_Mode = Mode_Breathe;
			Beathe_Init(&bre28,sys_color[Color_Cnt-1]);
			tim16 = 0;
		}
		else if(Work_Mode == Mode_Breathe)
		{	
			Sleep_State=Sleep_End;
			Work_Mode = Mode_Bright;
		}
  }  
	if(GPIO_Pin == GPIO_PIN_1)
  {
		if(Work_Mode != Mode_Breathe)
		{
			while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1));
			if(Color_Cnt<3)
			{
				Work_Mode = Mode_Bright;
				Color_Cnt++;
			}
			else
			{
				Work_Mode = Mode_Sleep;
				Color_Cnt = 0;
				tim16 = 0;
				Sleep_State=Sleep_End;
			}
		}
  } 
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//10ms
	if(htim == &htim16)
	{
		if(Sleep_State==Sleep_Begin)
		{	
			if(tim16<Sleep_Min*100*60)
			{	
				tim16++;
			}
			else
			{
				if(bre28.Status!=WAIT)
					tim16++;
				else
				{
					tim16=0;
					Sleep_State=Sleep_End;
					Work_Mode = Mode_Sleep;		
				}
			}		
		}
		
	} 
}

void uartdamget(void)
{
	if(recv_end_flag ==1)
	{
		if(User_Run_flag == 0)
		{
			User_Run_flag =1;
		}
		else
		{
			CMD_String_deal(ReceiveBuff);
		}
		/*        打印接收到的数据长度 */
		//printf("\nrx_len=%d\r\n",Rx_len);
		
		//Sw_Light_Color(color_temp);
//		for(int i=0;i<Rx_len;i++)
//		{
//		/*        打印接收到的数据 */
//			printf("%c",ReceiveBuff[i]);
//		}
//		printf("\r\n");   
//		printf("%#x\r\n",color_temp);
		
		/*        清空接收缓存区        */
		for(int i = 0; i < Rx_len ; i++) 
				ReceiveBuff[i]=0;
		/*        接收数据长度清零        */
		Rx_len=0;
		recv_end_flag=0;
	}
	/*        开启下一次接收        */
	HAL_UART_Receive_DMA(&huart1,(uint8_t*)ReceiveBuff,BUFFERSIZE);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
