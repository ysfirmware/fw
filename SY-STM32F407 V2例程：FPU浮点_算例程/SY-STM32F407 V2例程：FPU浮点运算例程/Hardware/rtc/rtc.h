#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx.h"

void RTC_Config(void);
void RTC_SetFullTime (uint8_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t minutes,uint8_t seconds,uint8_t weekday);
#endif
