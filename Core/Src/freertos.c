/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
/* Definitions for fansTask */
osThreadId_t fansTaskHandle;
const osThreadAttr_t fansTask_attributes = {
  .name = "fansTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for debugTask */
osThreadId_t debugTaskHandle;
const osThreadAttr_t debugTask_attributes = {
  .name = "debugTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for pedalsTask */
osThreadId_t pedalsTaskHandle;
const osThreadAttr_t pedalsTask_attributes = {
  .name = "pedalsTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for gearTask */
osThreadId_t gearTaskHandle;
const osThreadAttr_t gearTask_attributes = {
  .name = "gearTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for telemetryTask */
osThreadId_t telemetryTaskHandle;
const osThreadAttr_t telemetryTask_attributes = {
  .name = "telemetryTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void fans_thread(void *argument);
void debug_thread(void *argument);
void pedals_thread(void *argument);
void gear_thread(void *argument);
void telemetry_thread(void *argument);

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
  /* creation of fansTask */
  fansTaskHandle = osThreadNew(fans_thread, NULL, &fansTask_attributes);

  /* creation of debugTask */
  debugTaskHandle = osThreadNew(debug_thread, NULL, &debugTask_attributes);

  /* creation of pedalsTask */
  pedalsTaskHandle = osThreadNew(pedals_thread, NULL, &pedalsTask_attributes);

  /* creation of gearTask */
  gearTaskHandle = osThreadNew(gear_thread, NULL, &gearTask_attributes);

  /* creation of telemetryTask */
  telemetryTaskHandle = osThreadNew(telemetry_thread, NULL, &telemetryTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_fans_thread */
/**
  * @brief  Function implementing the fansTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_fans_thread */
__weak void fans_thread(void *argument)
{
  /* USER CODE BEGIN fans_thread */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END fans_thread */
}

/* USER CODE BEGIN Header_debug_thread */
/**
* @brief Function implementing the debugTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_debug_thread */
__weak void debug_thread(void *argument)
{
  /* USER CODE BEGIN debug_thread */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END debug_thread */
}

/* USER CODE BEGIN Header_pedals_thread */
/**
* @brief Function implementing the pedalsTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pedals_thread */
__weak void pedals_thread(void *argument)
{
  /* USER CODE BEGIN pedals_thread */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END pedals_thread */
}

/* USER CODE BEGIN Header_gear_thread */
/**
* @brief Function implementing the gearTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_gear_thread */
__weak void gear_thread(void *argument)
{
  /* USER CODE BEGIN gear_thread */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END gear_thread */
}

/* USER CODE BEGIN Header_telemetry_thread */
/**
* @brief Function implementing the telemetryTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_telemetry_thread */
__weak void telemetry_thread(void *argument)
{
  /* USER CODE BEGIN telemetry_thread */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END telemetry_thread */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

