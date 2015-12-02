/* ============================================================================ */
/*	Exp13_2_average.c : A/D 컨버터의 단일변환 모드 사용(평균 처리)		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned char i;
  unsigned short result;
  float temperature;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_2.c    ");
  Beep();

  TFT_string(6, 3,White,Magenta," A/D 컨버터의 단일변환 모드 ");
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
  ADC1->CFGR1 = 0x00000000;			// 12-bit resolution, single conversion mode
  ADC1->CFGR2 = 0x80000000;			// PCLK/4 = 12MHz

  while(1)
    { ADC1->CHSELR = 0x00000001;		// channel 0 (+5.0V)
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR |= 0x00000004;		// start conversion by software
          while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(25,7);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096., 1, 2);

      ADC1->CHSELR = 0x00000002;		// channel 1 (+3.3V)
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR |= 0x00000004;		// start conversion by software
          while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(25,10);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096., 1, 2);

      ADC1->CHSELR = 0x00000004;		// channel 2 (VR1)
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR |= 0x00000004;		// start conversion by software
          while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(25,13);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096., 1, 2);

      ADC1->CHSELR = 0x00000008;		// channel 3 (LM35DZ)
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR |= 0x00000004;		// start conversion by software
          while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(25,16);				// display temperature
      TFT_color(Magenta,Black);
      TFT_signed_float((float)result*330./4096., 2, 1);

      ADC1->CHSELR = 0x00010000;		// channel 16 (Temperature sensor)
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR |= 0x00000004;		// start conversion by software
          while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(25,19);				// display temperature
      TFT_color(Green,Black);
      temperature = (1.43 - (float)result*3.3/4096.) / 0.0043 + 25.;
      TFT_signed_float(temperature, 2, 1);

      ADC1->CHSELR = 0x00020000;		// channel 17 (VREFINT)
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR |= 0x00000004;		// start conversion by software
          while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(25,22);				// display voltage
      TFT_color(Green,Black);
      TFT_signed_float((float)result*3.3/4096., 1, 2);

      ADC1->CHSELR = 0x00040000;		// channel 18 (VBAT/2)
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR |= 0x00000004;		// start conversion by software
          while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(25,25);				// display VBAT voltage
      TFT_color(Green,Black);
      TFT_signed_float((float)result*2.*3.3/4096., 1, 2);

      Delay_ms(250);
    }
}
