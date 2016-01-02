/* ============================================================================ */
/*  Exp13_8_Injected simultaneous mode(triple).c : A/D 컨버터의 불연속변환 모드 */
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
  LCD_string(0xC0,"   Exp13_8.c    ");
  Beep();

  TFT_string(1, 3,White,Magenta,"A/D 컨버터 Regular simultaneous mode ");
  TFT_string(9, 10,White,Black,"ADC0  (+5.0V) = +0.00V");
  TFT_string(9, 16,White,Black,"ADC1  (+3.3V) = +0.00V");
  TFT_string(9, 22,White,Black,"ADC2  ( VR1 ) = +0.00V");
   
  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000700;			// enable ADC1 clock

  ADC1->SMPR1 = 0x00000000;			// sampling time of channel 10~18 = 3 cycle
  ADC1->SMPR2 = 0x00000002;			// sampling time of channel 0 = 28 cycle
  ADC2->SMPR1 = 0x00000000;			// sampling time of channel 10~18 = 3 cycle
  ADC2->SMPR2 = 0x00000010;			// sampling time of channel 1 = 28 cycle
  ADC3->SMPR1 = 0x00000000;			// sampling time of channel 10~18 = 3 cycle
  ADC3->SMPR2 = 0x00000080;			// sampling time of channel 2 = 28 cycle
  
  ADC1->JSQR = 0x00000000;                      // JL[1:0] = 0 : single conversion, cannel 0
  ADC2->JSQR = 0x00008000;                      // JL[1:0] = 0 : single conversion, cannel 1
  ADC3->JSQR = 0x00010000;                      // JL[1:0] = 0 : single conversion, cannel 2
  
  ADC->CCR = 0x00030015;			// ADCCLK = 42MHz/2 = 21MHz
  ADC1->CR1 = 0x00001000;                       // Discontinuous mode on Injected channels
  ADC1->CR2 = 0x00000001;                       // ADC ON
  ADC2->CR1 = 0x00001000;                       // Discontinuous mode on Injected channels
  ADC2->CR2 = 0x00000001;                       // ADC ON  0
  ADC3->CR1 = 0x00001000;                       // Discontinuous mode on Injected channels
  ADC3->CR2 = 0x00000001;                       // ADC ON 
  
  while(1)
    {     
      ADC1->CR2 |= 0x00400000;          //start convertion of regular group
      while(!(ADC1->SR & 0x00000004)||!(ADC2->SR & 0x00000004)||!(ADC3->SR & 0x00000004));	// wait A/D conversion complete      
      
      result = ADC1->JDR1;
      TFT_xy(25,10);			// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096., 1, 2);
      
      result = ADC2->JDR1;
      TFT_xy(25,16);			// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096., 1, 2);

      result = ADC3->JDR1;
      TFT_xy(25,22);			// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096., 1, 2);    
      Delay_ms(250);
    }
}
