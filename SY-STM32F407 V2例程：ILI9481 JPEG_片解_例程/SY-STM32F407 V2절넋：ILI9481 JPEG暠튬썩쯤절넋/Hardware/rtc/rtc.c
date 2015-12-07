#include "rtc.h"

void RTC_Config(void)
{
  RTC_InitTypeDef RTC_InitStructure;
  /*ʹ�ܵ�Դʱ��*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
/*ʹ�ܺ�����ʱ��*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  /*����RTC�󱸺󱸼Ĵ�����дȨ��*/
  PWR_BackupAccessCmd(ENABLE);
  /*��λ����*/
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  /* ʹ���ⲿ����ʱ��*/
  RCC_LSEConfig(RCC_LSE_ON);
  /*�ȴ�����ʱ��׼������*/  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /*ѡ���ⲿ����ʱ��(LSE)��ΪRTCʱ��Դ*/
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /*ʹ��RTC*/
  RCC_RTCCLKCmd(ENABLE);
  /*�ȴ�RTC APB �Ĵ���ͬ��*/
  RTC_WaitForSynchro();
  /*����RTC��Ƶֵ*/
  RTC_InitStructure.RTC_AsynchPrediv = 0x1F;                          //�����첽��Ƶֵ
  RTC_InitStructure.RTC_SynchPrediv  = 0x3FF;                         //����ͬ����Ƶֵ
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;             //����ʱ���ʽΪ24Сʱ��
  RTC_Init(&RTC_InitStructure);                                        //��ʼ��RTC
/* ʹ�ܵ͵�ѹʱʹ�ú󱸵��*/
  PWR_BackupRegulatorCmd(ENABLE);
  /* �ȴ�����͵�ѹ�ȶ������� */
  while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
  {
  }
   /*��󱸼Ĵ���0д��0x5566���ڱ���Ѿ����ù�RTC*/
   RTC_WriteBackupRegister(RTC_BKP_DR0, 0X5566);	
}
void RTC_AlarmConfig(void)
{
  RTC_AlarmTypeDef RTC_AlarmStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 
  /*�����ж����ȼ����� */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;                   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;            //������ռ���ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                   //���ô����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                      //ʹ�������ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);                                      //��ʼ���ж�����
 
  /*���������ж�����*/
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
	/*��������A*/
  RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
  
  /* Set AlarmA subseconds and enable SubSec Alarm : generate 8 interripts per Second */
  RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);
  /* ʹ������A�ж�*/
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
