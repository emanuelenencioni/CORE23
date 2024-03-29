/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GEAR_PROX_DOWN_Pin GPIO_PIN_2
#define GEAR_PROX_DOWN_GPIO_Port GPIOE
#define GEAR_PROX_UP_Pin GPIO_PIN_3
#define GEAR_PROX_UP_GPIO_Port GPIOE
#define DIO_AUX1_Pin GPIO_PIN_4
#define DIO_AUX1_GPIO_Port GPIOE
#define DIO_AUX2_Pin GPIO_PIN_13
#define DIO_AUX2_GPIO_Port GPIOC
#define SHUTDOWN_SENSE_Pin GPIO_PIN_14
#define SHUTDOWN_SENSE_GPIO_Port GPIOC
#define CUTOFF_Pin GPIO_PIN_15
#define CUTOFF_GPIO_Port GPIOC
#define ADC_AUX2_Pin GPIO_PIN_0
#define ADC_AUX2_GPIO_Port GPIOC
#define DESMO2_Pin GPIO_PIN_1
#define DESMO2_GPIO_Port GPIOC
#define VUVG_UP_Pin GPIO_PIN_2
#define VUVG_UP_GPIO_Port GPIOC
#define VUVG_DOWN_Pin GPIO_PIN_3
#define VUVG_DOWN_GPIO_Port GPIOC
#define DESMO1_Pin GPIO_PIN_0
#define DESMO1_GPIO_Port GPIOA
#define CLUTCH_OIL_Pin GPIO_PIN_1
#define CLUTCH_OIL_GPIO_Port GPIOA
#define GEAR_UP_AIR_Pin GPIO_PIN_2
#define GEAR_UP_AIR_GPIO_Port GPIOA
#define VPPM_SENSE_Pin GPIO_PIN_3
#define VPPM_SENSE_GPIO_Port GPIOA
#define VPPM_OUT_Pin GPIO_PIN_4
#define VPPM_OUT_GPIO_Port GPIOA
#define DAC_AUX_Pin GPIO_PIN_5
#define DAC_AUX_GPIO_Port GPIOA
#define EBS_AIR_2_Pin GPIO_PIN_6
#define EBS_AIR_2_GPIO_Port GPIOA
#define EBS_AIR_1_Pin GPIO_PIN_7
#define EBS_AIR_1_GPIO_Port GPIOA
#define APPS_1_Pin GPIO_PIN_4
#define APPS_1_GPIO_Port GPIOC
#define ADC_AUX1_Pin GPIO_PIN_5
#define ADC_AUX1_GPIO_Port GPIOC
#define APPS_2_Pin GPIO_PIN_0
#define APPS_2_GPIO_Port GPIOB
#define BPPS_Pin GPIO_PIN_1
#define BPPS_GPIO_Port GPIOB
#define RES_IN1_Pin GPIO_PIN_2
#define RES_IN1_GPIO_Port GPIOB
#define RES_IN2_Pin GPIO_PIN_7
#define RES_IN2_GPIO_Port GPIOE
#define RES_STATUS_Pin GPIO_PIN_8
#define RES_STATUS_GPIO_Port GPIOE
#define ASMS_STATUS_Pin GPIO_PIN_9
#define ASMS_STATUS_GPIO_Port GPIOE
#define BRAKE_LIGHT_Pin GPIO_PIN_11
#define BRAKE_LIGHT_GPIO_Port GPIOE
#define QM_TRIGGER_Pin GPIO_PIN_13
#define QM_TRIGGER_GPIO_Port GPIOE
#define SHUTDOWN_CMD_Pin GPIO_PIN_14
#define SHUTDOWN_CMD_GPIO_Port GPIOE
#define RADIATOR_Pin GPIO_PIN_10
#define RADIATOR_GPIO_Port GPIOB
#define INTERCOOLER_Pin GPIO_PIN_11
#define INTERCOOLER_GPIO_Port GPIOB
#define MOT_SEL0_Pin GPIO_PIN_12
#define MOT_SEL0_GPIO_Port GPIOB
#define MOT_INA_Pin GPIO_PIN_13
#define MOT_INA_GPIO_Port GPIOB
#define MOT_PWM_Pin GPIO_PIN_14
#define MOT_PWM_GPIO_Port GPIOB
#define MOT_MS_Pin GPIO_PIN_15
#define MOT_MS_GPIO_Port GPIOB
#define MOT_MS_EN_Pin GPIO_PIN_8
#define MOT_MS_EN_GPIO_Port GPIOD
#define MOT_INB_Pin GPIO_PIN_9
#define MOT_INB_GPIO_Port GPIOD
#define MOT_SEL1_Pin GPIO_PIN_10
#define MOT_SEL1_GPIO_Port GPIOD
#define EBS_LED_Pin GPIO_PIN_6
#define EBS_LED_GPIO_Port GPIOC
#define NEUTRAL_LED_Pin GPIO_PIN_7
#define NEUTRAL_LED_GPIO_Port GPIOC
#define NEUTRAL_LED_EXTI_IRQn EXTI9_5_IRQn
#define ASSI_LED_B_Pin GPIO_PIN_8
#define ASSI_LED_B_GPIO_Port GPIOC
#define ASSI_LED_Y_Pin GPIO_PIN_9
#define ASSI_LED_Y_GPIO_Port GPIOC
#define ASSI_SIREN_Pin GPIO_PIN_8
#define ASSI_SIREN_GPIO_Port GPIOA
#define LORA_D0_Pin GPIO_PIN_15
#define LORA_D0_GPIO_Port GPIOA
#define LORA_RST_Pin GPIO_PIN_0
#define LORA_RST_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_3
#define LED_B_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_4
#define LED_R_GPIO_Port GPIOB
#define BTN_1_Pin GPIO_PIN_0
#define BTN_1_GPIO_Port GPIOE
#define BTN_1_EXTI_IRQn EXTI0_IRQn
#define BTN_2_Pin GPIO_PIN_1
#define BTN_2_GPIO_Port GPIOE
#define BTN_2_EXTI_IRQn EXTI1_IRQn

/* USER CODE BEGIN Private defines */
#define MIN_RPM_ENG_ON 800
#define AS_INIT_BRAKE_PRESSURE 15
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
