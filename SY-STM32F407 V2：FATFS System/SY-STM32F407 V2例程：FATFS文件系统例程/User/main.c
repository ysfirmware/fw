/****************************************Copyright (c)****************************************************
**                                      
**                                 ��      ����shan-yan.taobao.com
**                                 ��������Ⱥ��15935762
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
/* ------------------------------------------������ͷ�ļ�-----------------------------------------------*/
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
  * �������ƣ�main()
  * ����    ��void
  * ����ֵ  ��void
  * ����    �����������main����
  *************************************************************************************/
int main(void)
{ 
	SD_Error state;
	FATFS fs;           
	FRESULT res;                 
	DIR dirs;
  	FILINFO finfo;

  	SystemInit();			                      //��ʼ��ϵͳʱ�ӣ�����ʱ��Ϊ168Mhz
  	LED_GPIO_Conf();							  //��ʼ��LED��GPIO����
  	SysTick_Init();								  //ϵͳ���ĳ�ʼ��
  	USART1_Conf();								  //����1��ʼ��
  	LCD_Init();									  //LCD��ʼ��
	state=SD_Init();							  //TF����ʼ��
  	delay_nms(100);
  	printf("\r\n��ӭʹ��SY-STM32F407 V2�����壡\r\n");
 	printf("\r\n        ɽ�ҿƼ���\r\n");
  	printf("\r\n            -----רҵ��ֵ��������\r\n");
	printf("����һ��FATFS�ļ�ϵͳʵ�飡\r\n");
   	delay_nms(50);								  //��ʱ
   	LCD_Clear(BLUE);							  //��������ɫ��
	LCD_SetColors(RED,BLUE);					  //����ǰ��ɫ����ɫ��������ɫ����ɫ��
	LCD_DrawString(40,10,"SY-STM32F407 V2");	  //����ַ�
   	LCD_DrawString(40,40,"FATFS Test Demo");	  //����ַ�
	LCD_SetColors(GREEN,BLUE);					  //����ǰ��ɫ����ɫ��������ɫ����ɫ��
   	if(state==SD_OK)
	{
		LCD_DrawString(0,60,"SDCard Init Success");
		printf("SD����ʼ���ɹ���\r\n");
	}
	else
	{
		LCD_DrawString(0,60,"SDCard Init Error");
		printf("SD����ʼ��ʧ�ܣ�\r\n");
	}
	LCD_DrawString(10,85,"Capacity:     M");
	LCD_DrawNum(160,85,SDCardInfo.CardCapacity>>20,4);
	printf("SD����Ϊ��%dM\r\n",SDCardInfo.CardCapacity>>20);
	res=f_mount(0, &fs);
  	res=f_opendir(&dirs, "0:/");//�򿪸�Ŀ¼
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

