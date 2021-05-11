/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define b_trip_Pin GPIO_PIN_2
#define b_trip_GPIO_Port GPIOE
#define b_gal_Pin GPIO_PIN_3
#define b_gal_GPIO_Port GPIOE
#define b_test_Pin GPIO_PIN_4
#define b_test_GPIO_Port GPIOE
#define a_trip_Pin GPIO_PIN_5
#define a_trip_GPIO_Port GPIOE
#define trip_out_Pin GPIO_PIN_6
#define trip_out_GPIO_Port GPIOE
#define adhe_1_Pin GPIO_PIN_0
#define adhe_1_GPIO_Port GPIOC
#define adhe_2_Pin GPIO_PIN_1
#define adhe_2_GPIO_Port GPIOC
#define adhe_1e_Pin GPIO_PIN_2
#define adhe_1e_GPIO_Port GPIOC
#define adhe_2e_Pin GPIO_PIN_3
#define adhe_2e_GPIO_Port GPIOC
#define io_1_Pin GPIO_PIN_0
#define io_1_GPIO_Port GPIOA
#define temp_ch1_Pin GPIO_PIN_1
#define temp_ch1_GPIO_Port GPIOA
#define temp_ch2_Pin GPIO_PIN_2
#define temp_ch2_GPIO_Port GPIOA
#define ledeth_Pin GPIO_PIN_3
#define ledeth_GPIO_Port GPIOA
#define spi1_cs_Pin GPIO_PIN_4
#define spi1_cs_GPIO_Port GPIOA
#define cp1ad_Pin GPIO_PIN_4
#define cp1ad_GPIO_Port GPIOC
#define cp2ad_Pin GPIO_PIN_5
#define cp2ad_GPIO_Port GPIOC
#define pwm1_Pin GPIO_PIN_0
#define pwm1_GPIO_Port GPIOB
#define pwm2_Pin GPIO_PIN_1
#define pwm2_GPIO_Port GPIOB
#define voi_data_Pin GPIO_PIN_7
#define voi_data_GPIO_Port GPIOE
#define voi_cs_Pin GPIO_PIN_8
#define voi_cs_GPIO_Port GPIOE
#define voi_clk_Pin GPIO_PIN_9
#define voi_clk_GPIO_Port GPIOE
#define kl1_Pin GPIO_PIN_10
#define kl1_GPIO_Port GPIOE
#define kn1_Pin GPIO_PIN_11
#define kn1_GPIO_Port GPIOE
#define kl2_Pin GPIO_PIN_12
#define kl2_GPIO_Port GPIOE
#define kn2_Pin GPIO_PIN_13
#define kn2_GPIO_Port GPIOE
#define rey2_Pin GPIO_PIN_14
#define rey2_GPIO_Port GPIOE
#define rey1_Pin GPIO_PIN_15
#define rey1_GPIO_Port GPIOE
#define rey4_Pin GPIO_PIN_10
#define rey4_GPIO_Port GPIOB
#define rey3_Pin GPIO_PIN_11
#define rey3_GPIO_Port GPIOB
#define e2cs_Pin GPIO_PIN_12
#define e2cs_GPIO_Port GPIOB
#define e2sck_Pin GPIO_PIN_13
#define e2sck_GPIO_Port GPIOB
#define e2miso_Pin GPIO_PIN_14
#define e2miso_GPIO_Port GPIOB
#define e2mosi_Pin GPIO_PIN_15
#define e2mosi_GPIO_Port GPIOB
#define ledR1_Pin GPIO_PIN_10
#define ledR1_GPIO_Port GPIOD
#define ledR2_Pin GPIO_PIN_11
#define ledR2_GPIO_Port GPIOD
#define ledG1_Pin GPIO_PIN_12
#define ledG1_GPIO_Port GPIOD
#define ledG2_Pin GPIO_PIN_13
#define ledG2_GPIO_Port GPIOD
#define ledB1_Pin GPIO_PIN_14
#define ledB1_GPIO_Port GPIOD
#define ledB2_Pin GPIO_PIN_15
#define ledB2_GPIO_Port GPIOD
#define pf2_Pin GPIO_PIN_8
#define pf2_GPIO_Port GPIOC
#define pf1_Pin GPIO_PIN_9
#define pf1_GPIO_Port GPIOC
#define io_2_Pin GPIO_PIN_8
#define io_2_GPIO_Port GPIOA
#define rey5_Pin GPIO_PIN_9
#define rey5_GPIO_Port GPIOA
#define rey6_Pin GPIO_PIN_10
#define rey6_GPIO_Port GPIOA
#define ledcpu_Pin GPIO_PIN_15
#define ledcpu_GPIO_Port GPIOA
#define blewk_Pin GPIO_PIN_0
#define blewk_GPIO_Port GPIOD
#define bleint_Pin GPIO_PIN_1
#define bleint_GPIO_Port GPIOD
#define set1_Pin GPIO_PIN_3
#define set1_GPIO_Port GPIOD
#define set2_Pin GPIO_PIN_4
#define set2_GPIO_Port GPIOD
#define con2_485_Pin GPIO_PIN_7
#define con2_485_GPIO_Port GPIOD
#define set3_Pin GPIO_PIN_3
#define set3_GPIO_Port GPIOB
#define set4_Pin GPIO_PIN_4
#define set4_GPIO_Port GPIOB
#define con1_485_Pin GPIO_PIN_5
#define con1_485_GPIO_Port GPIOB
#define pe_dete_Pin GPIO_PIN_0
#define pe_dete_GPIO_Port GPIOE
#define esb_Pin GPIO_PIN_1
#define esb_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
