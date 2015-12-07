/****************************************Copyright (c)****************************************************
**                                      
**                                 淘      宝：shan-yan.taobao.com
**                                 技术交流群：15935762
**								   Q        Q: 895838470
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The JPEG application function
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
#include "adc.h"


extern unsigned  char JPGBUFF[]; 


/*************************************************************************************
  * 函数名称：main()
  * 参数    ：void
  * 返回值  ：void
  * 描述    ：程序主入口main函数
  *************************************************************************************/
int main(void)
{ 
  	SystemInit();			               //初始化系统时钟，设置时钟为168Mhz
  	LED_GPIO_Config();					   //初始化LED的GPIO配置
  	SysTick_Init();                        //系统节拍初始化  
  	USART1_Conf();                         //串口1初始化
  	LCD_Init();                            //LCD初始化
  	printf("\r\n欢迎使用SY-STM32F407 V2开发板！\r\n");
  	printf("\r\n        山岩科技！\r\n");
  	printf("\r\n            -----专业，值得信赖！\r\n");
  	delay_nms(50);                          //延时
  	printf("\r\n 这是一个JPG解码例程 \r\n");

	LoadJpegFile(JPGBUFF);

	while(1)
	{
  		LED1(On);
		delay_nms(300);
		LED1(Off);
		delay_nms(300);		
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

