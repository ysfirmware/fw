/* ============================================================================ */
/*		Exp13_4_continuous.c : A/D 컨버터의 연속변환 모드 사용		*/
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"


int main(void)
{
  unsigned short ADC0_result=0., ADC1_result=0., ADC2_result=0.;
  
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_4.c    ");
  Beep();

  TFT_string(6, 3,White,Magenta," A/D 컨버터의 연속변환 모드 ");
  TFT_string(9,10,White,Black,"ADC0  (+5.0V) = +0.00V");
  TFT_string(9,15,White,Black,"ADC1  (+3.3V) = +0.00V");
  TFT_string(9,20,White,Black,"ADC2  ( VR1 ) = +0.00V");

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000700;			// enable ADC1~3 clock
  
  ADC1->SMPR1 = 0x00000000;			// sampling time of channel 16~18 = 3 cycle
  ADC1->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  ADC2->SMPR1 = 0x00000000;			// sampling time of channel 16~18 = 3 cycle
  ADC2->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  ADC3->SMPR1 = 0x00000000;			// sampling time of channel 16~18 = 3 cycle
  ADC3->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  
  ADC->CCR  = 0x00800000;			// enable channel 16~17, ADCCLK = 42MHz/2 = 21MHz
  ADC->CCR |= 0x00030000; 	                // TSVREFE = 1, VBATE = 0, 54Mhz / 8 = 6.75MHz 
  ADC1->CR1 = 0x00000000;			// ADC1, 12-bit resolution
  ADC1->CR2 = 0x00000003;			// right alignment, continuous mode, ADON = 1
  ADC2->CR1 = 0x00000000;			// ADC2, 12-bit resolution
  ADC2->CR2 = 0x00000003;			// right alignment, continuous mode, ADON = 1
  ADC3->CR1 = 0x00000000;			// ADC3, 12-bit resolution
  ADC3->CR2 = 0x00000003;			// right alignment, continuous mode, ADON = 1
  
  ADC1->SQR1 = 0x00000000;
  ADC1->SQR3 = 0x00000000;                      // channel 0 (+5.0V)
  ADC2->SQR1 = 0x00000000;
  ADC2->SQR3 = 0x00000001;                      // channel 1 (+3.3V)
  ADC3->SQR1 = 0x00000000;
  ADC3->SQR3 = 0x00000002;                      // channel 2 (VR1)
  
  ADC1->CR2 |= 0x40000000;			// start conversion by software
  ADC2->CR2 |= 0x40000000;			// start conversion by software
  ADC3->CR2 |= 0x40000000;			// start conversion by software
  
  while(1)
    { 
      while(!(ADC1->SR & 0x00000002));	     	// channel 0 (+5.0V) : wait for EOC
      ADC0_result = ADC1->DR;
      while(!(ADC2->SR & 0x00000002));		// channel 1 (+3.3V) : wait for EOC
      ADC1_result = ADC2->DR;
      while(!(ADC3->SR & 0x00000002));		// channel 2 (VR1) : wait for EOC
      ADC2_result = ADC3->DR;
      
      TFT_xy(25,10);				// display channel 0 (+5.0V)
      TFT_color(Cyan,Black);
      TFT_signed_float((float)ADC0_result*2.*3.3/4096., 1, 2);

      TFT_xy(25,15);				// display channel 1 (+3.3V)
      TFT_color(Cyan,Black);
      TFT_signed_float((float)ADC1_result*2.*3.3/4096., 1, 2);

      TFT_xy(25,20);				// display channel 2 (VR1)
      TFT_color(Cyan,Black);
      TFT_signed_float((float)ADC2_result*3.3/4096., 1, 2);

      Delay_ms(10);
    }
}
