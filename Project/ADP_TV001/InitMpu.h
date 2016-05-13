/*********************** (C) COPYRIGHT 2013 Hanyoung Nux ***********************
* File Name          : InitMpuF1.h
* Author             : Frank Yang (ysd375@hynux.com)
* Version            : V1.0.0
* Date               : 08/20/2013
* Description        : Header for InitMpuF1 program
********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INITMPUF1_H
#define __INITMPUF1_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay_ms(vu32 nTime);
void SysTick_Init(void);

void Init_Mpu(void);

#endif