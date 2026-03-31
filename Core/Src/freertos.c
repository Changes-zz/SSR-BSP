/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* Definitions for TaskJ4310 */
osThreadId_t TaskJ4310Handle;
const osThreadAttr_t TaskJ4310_attributes = {
  .name = "TaskJ4310",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task3508 */
osThreadId_t Task3508Handle;
const osThreadAttr_t Task3508_attributes = {
  .name = "Task3508",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void J4310_Task(void *argument);
extern void DJI_M3508_Task(void *argument);
/* USER CODE END FunctionPrototypes */

void StartTaskJ4310(void *argument);
void StartTask3508(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TaskJ4310 */
  TaskJ4310Handle = osThreadNew(StartTaskJ4310, NULL, &TaskJ4310_attributes);

  /* creation of Task3508 */
  Task3508Handle = osThreadNew(StartTask3508, NULL, &Task3508_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskJ4310 */
/**
  * @brief  Function implementing the TaskJ4310 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskJ4310 */
void StartTaskJ4310(void *argument)
{
  /* USER CODE BEGIN StartTaskJ4310 */
  /* 现在把任务循环移入到 API 实现中，直接调用一次作为任务入口 */
  J4310_Task(argument);
  for(;;)
  {
      osDelay(1);
  }
  /* USER CODE END StartTaskJ4310 */
}

/* USER CODE BEGIN Header_StartTask3508 */
/**
* @brief Function implementing the Task3508 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask3508 */
void StartTask3508(void *argument)
{
  /* USER CODE BEGIN StartTask3508 */
  DJI_M3508_Task(argument);
  for(;;)
  {
      osDelay(1);
  }
  /* USER CODE END StartTask3508 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

