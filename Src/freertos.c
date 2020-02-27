/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "can.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId modbusTaskHandle;
osThreadId printTaskHandle;
osThreadId canTaskHandle;
osMessageQId uartbufQueueHandle;
osSemaphoreId canSemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartModbusTask(void const * argument);
void StartPrintTask(void const * argument);
void StartCanTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of canSem */
  osSemaphoreDef(canSem);
  canSemHandle = osSemaphoreCreate(osSemaphore(canSem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of uartbufQueue */
  osMessageQDef(uartbufQueue, 16, uint16_t);
  uartbufQueueHandle = osMessageCreate(osMessageQ(uartbufQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of modbusTask */
  osThreadDef(modbusTask, StartModbusTask, osPriorityHigh, 0, 128);
  modbusTaskHandle = osThreadCreate(osThread(modbusTask), NULL);

  /* definition and creation of printTask */
  osThreadDef(printTask, StartPrintTask, osPriorityNormal, 0, 128);
  printTaskHandle = osThreadCreate(osThread(printTask), NULL);

  /* definition and creation of canTask */
  osThreadDef(canTask, StartCanTask, osPriorityRealtime, 0, 128);
  canTaskHandle = osThreadCreate(osThread(canTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartModbusTask */
/**
* @brief Function implementing the modbusTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartModbusTask */
void StartModbusTask(void const * argument)
{
  /* USER CODE BEGIN StartModbusTask */
  /* Infinite loop */
  for(;;)
  {
		(void)eMBPoll();
    osDelay(1);
  }
  /* USER CODE END StartModbusTask */
}

/* USER CODE BEGIN Header_StartPrintTask */
/**
* @brief Function implementing the printTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPrintTask */
void StartPrintTask(void const * argument)
{
  /* USER CODE BEGIN StartPrintTask */
  /* Infinite loop */
  for(;;)
  {
		if(Bool_RecvComplete)
		{
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
			Bool_RecvComplete = False;
			HAL_UART_Transmit(&huart1,Str_RecvBuf,U16_RecvLength,1000);
			while(HAL_UART_Receive_IT(&huart1,RxBuf,1) != HAL_OK)
			{   
				HAL_UART_Transmit(&huart1, (uint8_t *)&"Uart1 Error\r\n",13,1000);    
				HAL_Delay(100);
			} 
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
			U16_RecvLength = 0;	
			osSemaphoreRelease(canSemHandle);
		}
    osDelay(10);
  }
  /* USER CODE END StartPrintTask */
}

/* USER CODE BEGIN Header_StartCanTask */
/**
* @brief Function implementing the canTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCanTask */
__weak void StartCanTask(void const * argument)
{
  /* USER CODE BEGIN StartCanTask */
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(canSemHandle,osWaitForever);
		Can_TxMsg();
		//osSemaphoreCreate(canSemHandle, 1);
//		
//		HAL_I2C_Master_Transmit(&hi2c1,0xA0,(uint8_t*)&("asdfgh"),4,10);

    osDelay(100);
  }
  /* USER CODE END StartCanTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
