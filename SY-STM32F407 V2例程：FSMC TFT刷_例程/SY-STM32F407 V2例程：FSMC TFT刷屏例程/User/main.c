/****************************************Copyright (c)****************************************************
**                                      
**                                 淘      宝：shan-yan.taobao.com
**                                 技术交流群：15935762
**								   Q        Q: 895838470
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The FSMC application function
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
#include "usart.h"
#include "lcd.h"


/*************************************************************************************
  * 函数名称：main()
  * 参数    ：void
  * 返回值  ：void
  * 描述    ：程序主入口main函数
  *************************************************************************************/
int main(void)
{ 
	u16 i=0;

  	SystemInit();			                                   //初始化系统时钟，设置时钟为168Mhz
 	LED_GPIO_Conf();										   //初始化LED的GPIO配置
  	SysTick_Init();
  	USART1_Conf();
  	LCD_Init();
  	delay_nms(100);
 	printf("\r\n欢迎使用SY-STM32F407 V2开发板！\r\n");
  	printf("\r\n        山岩科技！\r\n");
 	printf("\r\n            -----专业，值得信赖！\r\n");
 
  	delay_nms(500);
   	LCD_Clear(0xf800);
   	delay_nms(500);
   	LCD_Clear(0x001f);
   	delay_nms(500);
  	while (1)
  	{	
  		LED0(On);					                           //打开LED0
		delay_nms(100);									       //延时约0.5s
		LED0(Off);											   //关闭LED0
		delay_nms(100);									       //延时约0.5s
    	LED1(On);											   //打开LED1
		delay_nms(100);									       //延时约0.5s
		LED1(Off);										       //关闭LED1
		delay_nms(100);									       //延时约0.5s
		i=rand()%65535+1;
		LCD_Clear(i);
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

