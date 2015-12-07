/****************************************Copyright (c)****************************************************
**                                      
**                                 ��      ����shan-yan.taobao.com
**                                 ��������Ⱥ��15935762
**								   Q        Q: 895838470
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The FPU application function
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
#include "arm_math.h"


#define TEST_LENGTH_SAMPLES 2048 
 
//FFT Bin�����������
extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES]; 
static float32_t testOutput[TEST_LENGTH_SAMPLES/2]; 
 
uint32_t fftSize=1024; 
uint32_t ifftFlag=0; 
uint32_t doBitReverse=1;  
uint32_t refIndex=213,testIndex=0;/* Reference index at which max energy of bin ocuurs */ 


/*************************************************************************************
  * �������ƣ�main()
  * ����    ��void
  * ����ֵ  ��void
  * ����    �����������main����
  *************************************************************************************/
int main(void)
{ 
	arm_status status; 
	arm_cfft_radix4_instance_f32 S; 
	float32_t maxValue; 

  	SystemInit();			               //��ʼ��ϵͳʱ�ӣ�����ʱ��Ϊ168Mhz
  	LED_GPIO_Config();					   //��ʼ��LED��GPIO����
  	SysTick_Init();                        //ϵͳ���ĳ�ʼ��  
  	USART1_Conf();                         //����1��ʼ��
  	LCD_Init();                            //LCD��ʼ��
	LCD_Clear(BLUE);
	LCD_SetColors(RED,BLUE);
  	printf("\r\n��ӭʹ��SY-STM32F407 V2�����壡\r\n");
  	printf("\r\n        ɽ�ҿƼ���\r\n");
  	printf("\r\n            -----רҵ��ֵ��������\r\n");
  	delay_nms(200);                          //��ʱ
  	printf("\r\n ����һ��FPU��������ʵ�� \r\n");
	LCD_DrawString(40,10,"SY-STM32F407 V2");
	LCD_DrawString(55,40,"ARM FPU DEMO");

	LCD_SetColors(GREEN,BLUE);
	status = ARM_MATH_SUCCESS; 	 //��ʼ��CFFT/CIFFT��Ԫ 
	status = arm_cfft_radix4_init_f32(&S,fftSize,ifftFlag, doBitReverse); //ͨ��CFFT/CIFFT��Ԫ��������
	arm_cfft_radix4_f32(&S, testInput_f32_10khz); //ͨ�����ӵķ������㵥Ԫ��������,�Լ���ÿ��bin�ķ���
	arm_cmplx_mag_f32(testInput_f32_10khz, testOutput,fftSize);  //�������ֵ������һ�µ�BINֵ 
	arm_max_f32(testOutput, fftSize, &maxValue, &testIndex); 	 
	if(testIndex !=  refIndex)status = ARM_MATH_TEST_FAILURE; 
	while(status != ARM_MATH_SUCCESS)LCD_DrawString(40,100,"ARM math failed"); 
 	LCD_DrawString(40,100,"ARM math sucess");

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

