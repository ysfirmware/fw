/* ============================================================================ */
/*		Exp13_4_continuous.c : A/D 컨버터의 연속변환 모드 사용		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned short ADC0_result, ADC1_result, ADC2_result, ADC3_result;
  unsigned short ADC16_result, ADC17_result, ADC18_result;
  float temperature;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_4.c    ");
  Beep();

  TFT_string(6, 3,White,Magenta," A/D 컨버터의 연속변환 모드 ");
  TFT_string(9, 7,White,Black,"ADC0  (+5.0V) = +0.00V");
  TFT_string(9,10,White,Black,"ADC1  (+3.3V) = +0.00V");
  TFT_string(9,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(9,16,White,Black,"ADC3 (LM35DZ) = +00.0 C");
  TFT_xy(30,16);
  TFT_English(0x7F);
  TFT_string(9,19,White,Black,"ADC16 (Temp.) = +00.0 C");
  TFT_xy(30,19);
  TFT_English(0x7F);
  TFT_string(9,22,White,Black,"ADC17 (+1.2V) = +0.00V");
  TFT_string(9,25,White,Black,"ADC18 (+VBAT) = +0.00V");

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3
  RCC->APB2ENR |= 0x00000200;			// enable ADC clock
  RCC->CFGR |= 0x00004000;			// ADC clock = PCLK/4 = 48MHz/4 = 12MHz
  RCC->CFGR3 |= 0x00000100;

  ADC1->CR = 0x00000001;			// enable ADC
  ADC->CCR = 0x01C00000;			// VBATEN = TSEN = VREFEN = 1
  ADC1->SMPR = 0x00000005;			// sampling time = 55.5 cycle
  ADC1->CFGR1 = 0x00002000;			// 12-bit resolution, continuous conversion mode
  ADC1->CFGR2 = 0x80000000;			// PCLK/4 = 12MHz
  ADC1->CHSELR = 0x0007000F;			// channel 0, 1, 2, 3, 16, 17, 18

  while(1)
    { ADC1->CR = 0x00000005;			// start conversion by software
      while(!(ADC1->ISR & 0x00000004));		// channel 0 (+5.0V) : wait for EOC
      ADC0_result = ADC1->DR;
      while(!(ADC1->ISR & 0x00000004));		// channel 1 (+3.3V) : wait for EOC
      ADC1_result = ADC1->DR;
      while(!(ADC1->ISR & 0x00000004));		// channel 2 (VR1) : wait for EOC
      ADC2_result = ADC1->DR;
      while(!(ADC1->ISR & 0x00000004));		// channel 3 (LM35DZ) : wait for EOC
      ADC3_result = ADC1->DR;
      while(!(ADC1->ISR & 0x00000004));		// channel 16 : wait for EOC
      ADC16_result = ADC1->DR;
      while(!(ADC1->ISR & 0x00000004));		// channel 17 : wait for EOC
      ADC17_result = ADC1->DR;
      while((ADC1->ISR & 0x0000000C) != 0x0000000C); // channel 18 : wait for EOSEQ and EOC
      ADC18_result = ADC1->DR;
      ADC1->ISR &= 0x0000000A;			// clear EOSEQ, EOSMP
      ADC1->CR = 0x00000011;			// stop conversion by software

      TFT_xy(25,7);				// display channel 0 (+5.0V)
      TFT_color(Cyan,Black);
      TFT_signed_float((float)ADC0_result*2.*3.3/4096., 1, 2);

      TFT_xy(25,10);				// display channel 1 (+3.3V)
      TFT_color(Cyan,Black);
      TFT_signed_float((float)ADC1_result*2.*3.3/4096., 1, 2);

      TFT_xy(25,13);				// display channel 2 (VR1)
      TFT_color(Cyan,Black);
      TFT_signed_float((float)ADC2_result*3.3/4096., 1, 2);

      TFT_xy(25,16);				// display channel 3 (LM35DZ)
      TFT_color(Magenta,Black);
      TFT_signed_float((float)ADC3_result*330./4096., 2, 1);

      TFT_xy(25,19);				// display channel 16
      TFT_color(Green,Black);
      temperature = (1.43 - (float)ADC16_result*3.3/4096.) / 0.0043 + 25.;
      TFT_signed_float(temperature, 2, 1);

      TFT_xy(25,22);				// display channel 17
      TFT_color(Green,Black);
      TFT_signed_float((float)ADC17_result*3.3/4096., 1, 2);

      TFT_xy(25,25);				// display channel 18
      TFT_color(Green,Black);
      TFT_signed_float((float)ADC18_result*2.*3.3/4096., 1, 2);

      Delay_ms(250);
    }
}
