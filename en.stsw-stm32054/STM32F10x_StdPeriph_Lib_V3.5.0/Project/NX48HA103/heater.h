#ifndef __HEATER
#define __HEATER


/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/

#include "main.h"


/* Exported types ------------------------------------------------------------*/
#define HEATER_ON			1
#define HEATER_OFF		0

#define HEATER_A	GPIO_Pin_5
#define HEATER_B	GPIO_Pin_6
#define HEATER_C	GPIO_Pin_7
#define HEATER_D	GPIO_Pin_8

void Heater_Cont(u16 nCh, u8 nStat);
void Heater_main();
void Delay_ms(__IO uint32_t nTime);

#endif //__HEATER

