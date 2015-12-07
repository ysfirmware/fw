#include "rtc.h"

void RTC_Config(void)
{
  RTC_InitTypeDef RTC_InitStructure;
  /*使能电源时钟*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
/*使能后备区域时钟*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  /*允许RTC后备后备寄存器读写权限*/
  PWR_BackupAccessCmd(ENABLE);
  /*复位后备域*/
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  /* 使能外部低速时钟*/
  RCC_LSEConfig(RCC_LSE_ON);
  /*等待低速时钟准备就绪*/  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /*选择外部低速时钟(LSE)作为RTC时钟源*/
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /*使能RTC*/
  RCC_RTCCLKCmd(ENABLE);
  /*等待RTC APB 寄存器同步*/
  RTC_WaitForSynchro();
  /*配置RTC分频值*/
  RTC_InitStructure.RTC_AsynchPrediv = 0x1F;                          //设置异步分频值
  RTC_InitStructure.RTC_SynchPrediv  = 0x3FF;                         //设置同步分频值
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;             //设置时间格式为24小时制
  RTC_Init(&RTC_InitStructure);                                        //初始化RTC
/* 使能低电压时使用后备电池*/
  PWR_BackupRegulatorCmd(ENABLE);
  /* 等待后备域低电压稳定器就绪 */
  while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
  {
  }
   /*向后备寄存器0写入0x5566用于标记已经配置过RTC*/
   RTC_WriteBackupRegister(RTC_BKP_DR0, 0X5566);	
}
void RTC_AlarmConfig(void)
{
  RTC_AlarmTypeDef RTC_AlarmStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 
  /*闹铃中断优先级配置 */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;                   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;            //配置先占优先级为0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                   //配置从优先级为0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                      //使能闹铃中断通道
  NVIC_Init(&NVIC_InitStructure);                                      //初始化中断配置
 
  /*配置闹铃中断类型*/
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
	/*设置闹铃A*/
  RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
  
  /* Set AlarmA subseconds and enable SubSec Alarm : generate 8 interripts per Second */
  RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);
  /* 使能闹铃A中断*/
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
}
void RTC_SetFullTime (uint8_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t minutes,uint8_t seconds,uint8_t weekday)
{
	RTC_DateTypeDef RTC_DateStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	PWR_BackupAccessCmd(ENABLE);
	RTC_DateStructure.RTC_WeekDay=weekday; 
  RTC_DateStructure.RTC_Month=month;  
  RTC_DateStructure.RTC_Date=date;    
  RTC_DateStructure.RTC_Year=year;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
	if(hour>12)
		{
   RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
   RTC_TimeStructure.RTC_Hours   = hour-0x12;
		}
		else
		{
	  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = hour;
		}
  RTC_TimeStructure.RTC_Minutes = minutes;
  RTC_TimeStructure.RTC_Seconds = seconds; 
		
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
}
