/****************************************Copyright (c)****************************************************
**                                      
**                                 淘      宝：shan-yan.taobao.com
**                                 技术交流群：15935762
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
/* ------------------------------------------包含的头文件-----------------------------------------------*/
#include "stm32f4xx.h"
#include "delay.h"
#include "led.h"


/*************************************************************************************
  * 函数名称：main()
  * 参数    ：void
  * 返回值  ：void
  * 描述    ：程序主入口main函数
  *************************************************************************************/
int main(void)
{
  	SystemInit();			                                   //初始化系统时钟，设置时钟为168Mhz
  	LED_GPIO_Conf();										   //初始化LED的GPIO配置
  	while (1)
  	{
  		LED0(On);					                           //打开LED0
		delay_nms(1000);									   //延时约1s
		LED0(Off);											   //关闭LED0
		delay_nms(1000);									   //延时约1s
    	LED1(On);											   //打开LED1
		delay_nms(1000);									   //延时约1s
		LED1(Off);											   //关闭LED1
		delay_nms(1000);									   //延时约1s
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

