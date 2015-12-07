/****************************************Copyright (c)****************************************************
**                                      
**                                 ��      ����shan-yan.taobao.com
**                                 ��������Ⱥ��15935762
**								   Q        Q: 895838470
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The LED application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              nongxiaoming
** Created date:            2011-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             shanyan
** Modified date:           2012-10-19
** Version:                 v2.0
** Descriptions:            
**
*********************************************************************************************************/
/* ------------------------------------------������ͷ�ļ�-----------------------------------------------*/
#include "stm32f4xx.h"
#include "delay.h"
#include "led.h"


/*************************************************************************************
  * �������ƣ�main()
  * ����    ��void
  * ����ֵ  ��void
  * ����    �����������main����
  *************************************************************************************/
int main(void)
{
  	SystemInit();			                                   //��ʼ��ϵͳʱ�ӣ�����ʱ��Ϊ168Mhz
  	LED_GPIO_Conf();										   //��ʼ��LED��GPIO����
  	while (1)
  	{
  		LED0(On);					                           //��LED0
		delay_nms(1000);									   //��ʱԼ1s
		LED0(Off);											   //�ر�LED0
		delay_nms(1000);									   //��ʱԼ1s
    	LED1(On);											   //��LED1
		delay_nms(1000);									   //��ʱԼ1s
		LED1(Off);											   //�ر�LED1
		delay_nms(1000);									   //��ʱԼ1s
  	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

