/****************************************Copyright (c)****************************************************
**                                      
**                                 淘      宝：shan-yan.taobao.com
**                                 技术交流群：15935762
**								   Q        Q: 895838470
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The FATFS application function
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
#include "sdcard.h"
#include "ff.h"


uint8_t tx_buf[512];
uint8_t rx_buf[512];
/*************************************************************************************
  * 函数名称：main()
  * 参数    ：void
  * 返回值  ：void
  * 描述    ：程序主入口main函数
  *************************************************************************************/
int main(void)
{ 
	SD_Error state;
	FATFS fs;           
	FRESULT res;                 
	DIR dirs;
  	FILINFO finfo;

  	SystemInit();			                      //初始化系统时钟，设置时钟为168Mhz
  	LED_GPIO_Conf();							  //初始化LED的GPIO配置
  	SysTick_Init();								  //系统节拍初始化
  	USART1_Conf();								  //串口1初始化
  	LCD_Init();									  //LCD初始化
	state=SD_Init();							  //TF卡初始化
  	delay_nms(100);
  	printf("\r\n欢迎使用SY-STM32F407 V2开发板！\r\n");
 	printf("\r\n        山岩科技！\r\n");
  	printf("\r\n            -----专业，值得信赖！\r\n");
	printf("这是一个FATFS文件系统实验！\r\n");
   	delay_nms(50);								  //延时
   	LCD_Clear(BLUE);							  //清屏（蓝色）
	LCD_SetColors(RED,BLUE);					  //设置前景色（红色），背景色（蓝色）
	LCD_DrawString(40,10,"SY-STM32F407 V2");	  //输出字符
   	LCD_DrawString(40,40,"FATFS Test Demo");	  //输出字符
	LCD_SetColors(GREEN,BLUE);					  //设置前景色（绿色），背景色（蓝色）
   	if(state==SD_OK)
	{
		LCD_DrawString(0,60,"SDCard Init Success");
		printf("SD卡初始化成功！\r\n");
	}
	else
	{
		LCD_DrawString(0,60,"SDCard Init Error");
		printf("SD卡初始化失败！\r\n");
	}
	LCD_DrawString(10,85,"Capacity:     M");
	LCD_DrawNum(160,85,SDCardInfo.CardCapacity>>20,4);
	printf("SD容量为：%dM\r\n",SDCardInfo.CardCapacity>>20);
	res=f_mount(0, &fs);
  	res=f_opendir(&dirs, "0:/");//打开根目录
  	res=f_readdir(&dirs, &finfo);
  	while(res!= FR_OK)LCD_DrawString(20,110,"Fatfs failed!");
	LCD_DrawString(20,110,"File name:");
  	LCD_DrawString(190,110,(u8 *)finfo.fname);
	f_readdir(&dirs, &finfo);
	LCD_DrawString(190,140,(u8 *)finfo.fname);
	f_readdir(&dirs, &finfo);
	LCD_DrawString(190,170,(u8 *)finfo.fname);
	f_readdir(&dirs, &finfo);
	LCD_DrawString(190,200,(u8 *)finfo.fname);
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

