/* ============================================================================ */
/*	Exp13_5_discontinuous.c : A/D 컨버터의 불연속scan 모드 사용		*/
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void TIM1_UP_TIM10_IRQHandler(void);	        /* TIM1 interrupt function (200Hz) */

volatile unsigned int interrupt_count;		// interrupt count
volatile unsigned char interrupt_flag, beep_flag; // flag by interrupt
volatile unsigned short result, result1, result2, result3;

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void TIM1_UP_TIM10_IRQHandler(void)	        /* TIM1 interrupt function (200Hz) */
{
  TIM1->SR = 0x0000;				// clear pending bit of TIM1 interrupt

  GPIOE->ODR ^= 0x00008000;			// toggle LED
}

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_5.c    ");
  Beep();

  TFT_string(5, 3,White,Magenta," A/D 컨버터의 SCAN 모드 ");
  TFT_string(9, 7,White,Black,"ADC0  (+5.0V) = +0.00V");
  TFT_string(9,10,White,Black,"ADC1  (+3.3V) = +0.00V");
  TFT_string(9,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(9,16,White,Black,"ADC3 (LM35DZ) = +00.0 C");
  TFT_xy(30,16);
  TFT_English(0x7F);
  
  RCC->APB2ENR |= 0x00000001;			// enable TIM1 clock
  TIM1->PSC = 1079;				// 108MHz/(1079+1) = 100kHz
  TIM1->ARR = 4999;				// 100kHz / (4999+1) = 200Hz
  TIM1->CNT = 0;				// clear counter
  TIM1->DIER = 0x0001;				// enable update interrupt
  TIM1->CR1 = 0x0005;				// overflow interrupt, enable TIM1
  TIM1->CR2 = 0x00200000;

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock

  ADC1->SMPR1 = 0x07FC0000;			// sampling time of channel 16~18 = 480 cycle
  ADC1->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  ADC->CCR  = 0x00800000;			// enable channel 16~17
  ADC->CCR |= 0x00030000;	                // TSVREFE = 1, VBATE = 0, 5MHz / 8 = 6.75Mhz 
  ADC1->CR1 = 0x00000100;                       // 12-bit resolution, scan mode
  ADC1->CR2 = 0x00280001;			// right alignment, ADON = 1
  
  ADC1->JSQR = 0x00318820;
  
  NVIC->ISER[0] |= 0x02000000;			// enable (25)TIM1 interrupt
  
  while(1)
    { 
      while(!(ADC1->SR & 0x00000004));	        // wait A/D conversion complete
      result = ADC1->JDR1;
      TFT_xy(25,7);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096.,1,2);
      
      result1 = ADC1->JDR2;
      TFT_xy(25,10);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result1*2.*3.3/4096.,1,2);
      
      result2 = ADC1->JDR3;
      TFT_xy(25,13);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result2*3.3/4096.,1,2);
      
      result3 = ADC1->JDR4;
      TFT_xy(25,16);				// display temperature
      TFT_color(Magenta,Black);
      TFT_signed_float((float)result3*330./4096.,2,1);
    }
}
