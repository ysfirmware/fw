/* ============================================================================ */
/* Exp13_7_Regular simultaneous mode(triple).c : A/D 컨버터의 불연속변환 모드 	*/
/* ============================================================================ */
/*					programmed by Ho-jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned short result;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_7.c    ");
  Beep();

  TFT_string(1, 3,White,Magenta,"A/D 컨버터 Regular simultaneous mode ");
  TFT_string(9, 10,White,Black,"ADC0  (+5.0V) = +0.00V");
  TFT_string(9, 16,White,Black,"ADC1  (+3.3V) = +0.00V");
  TFT_string(9, 22,White,Black,"ADC2  ( VR1 ) = +0.00V");
   
  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000700;			// enable ADC1 clock

  ADC1->SMPR1 = 0x00000000;			// single conversionsampling time of channel 16~17 = 0 cycle
  ADC1->SMPR2 = 0x00000002;			// sampling time of channel 1 = 28 cycle
  ADC2->SMPR1 = 0x00000000;			// sampling time of channel 0 = 28 cycle
  ADC2->SMPR2 = 0x00000010;			// sampling time of channel 2 = 28 cycle
  ADC3->SMPR1 = 0x00000000;			// sampling time of channel 0 = 28 cycle
  ADC3->SMPR2 = 0x00000080;			// sampling time of channel 3 = 28 cycle
  
  ADC->CCR = 0x00030016;			// enable channel 16~17, ADCCLK = 42MHz/2 = 21MHz
  ADC1->CR1 = 0x00000800;                       // Discontinuous mode on regular channels
  ADC1->CR2 = 0x00000001;                       // ADC ON
  ADC2->CR1 = 0x00000800;                       // Discontinuous mode on regular channels
  ADC2->CR2 = 0x00000001;                       // ADC ON  
  ADC3->CR1 = 0x00000800;                       // Discontinuous mode on regular channels
  ADC3->CR2 = 0x00000001;                       // ADC ON 
  ADC1->SQR1 = 0x00000000;                      // L[3:0] = 0 : single conversion
  ADC2->SQR1 = 0x00000000;                      // L[3:0] = 0 : single conversion
  ADC3->SQR1 = 0x00000000;                      // L[3:0] = 0 : single conversion
  ADC1->SQR3 = 0x00000000;                      // cannel 0
  ADC2->SQR3 = 0x00000001;                      // cannel 1
  ADC3->SQR3 = 0x00000002;                      // cannel 2
  
  while(1)
    {     
      ADC1->CR2 |= 0x40000000;          //start convertion of regular group
      while(!(ADC1->SR & 0x00000002)||!(ADC2->SR & 0x00000002)||!(ADC3->SR & 0x00000002));	// wait A/D conversion complete      
      
      result = ADC1->DR;
      TFT_xy(25,10);			// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096., 1, 2);
      
      result = ADC2->DR;
      TFT_xy(25,16);			// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096., 1, 2);

      result = ADC3->DR;
      TFT_xy(25,22);			// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096., 1, 2);      
      Delay_ms(250);
    }
}
