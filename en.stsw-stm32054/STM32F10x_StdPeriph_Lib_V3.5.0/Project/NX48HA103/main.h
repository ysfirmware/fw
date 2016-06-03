/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "heater.h"

/* Exported types ------------------------------------------------------------*/
#define TRUE		1
#define FALSE		0
#define	LED1_SET(i)		GPIO_WriteBit(GPIOF, GPIO_Pin_3, (i)?Bit_SET:Bit_RESET)
#define LED2_SET(i)		GPIO_WriteBit(GPIOF, GPIO_Pin_1, (i)?Bit_SET:Bit_RESET)

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//extern void Motion_main();
//extern void Comm_main();
//extern void Heater_main();


void TimingDelay_Decrement(void);
static void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
