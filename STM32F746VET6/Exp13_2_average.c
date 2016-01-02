/* ============================================================================ */
/*	Exp13_2_average.c : A/D 컨버터의 단일변환 모드 사용(평균 처리)		*/
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned char i;
  unsigned short result;
  float x;  
  
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");	        // display title
  LCD_string(0xC0,"   Exp13_2.c    ");
  Beep();

  TFT_string(6, 3,White,Magenta," A/D 컨버터의 단일변환 모드 ");
  TFT_string(8, 7,White,Black,"ADC0  (+5.0V) = +0.00V");
  TFT_string(8,10,White,Black,"ADC1  (+3.3V) = +0.00V");
  TFT_string(8,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(8,16,White,Black,"ADC3 (LM35DZ) = +00.0 C");
  TFT_string(8,19,White,Black,"ADC18 (Temp.) = +00.0 C");
  TFT_string(8,22,White,Black,"ADC17 (1.21V) = +0.00V");
  TFT_xy(29,16);
  TFT_English(0x7F);
  TFT_xy(29,19);
  TFT_English(0x7F);

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock

  ADC1->SMPR1 = 0x07FC0000;			// sampling time of channel 16~18 = 480 cycle
  ADC1->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  ADC->CCR = 0x00800000;			// enable channel 16~17, ADCCLK = 42MHz/2 = 21MHz
  ADC->CCR = 0x00830000;	                // TSVREFE = 1, VBATE = 0, 54MHz / 8 = 6.75Mhz 
  ADC1->CR1 = 0x00000800;			// 12-bit resolution, single discontinuous mode
  ADC1->CR2 = 0x00000001;			// right alignment, ADON = 1

  while(1)
    {ADC1->SQR3 = 0x00000000;			// channel 0 (+5.0V)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,7);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096.,1,2);

      ADC1->SQR3 = 0x00000001;			// channel 1 (+3.3V)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,10);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096.,1,2);

      ADC1->SQR3 = 0x00000002;			// channel 2 (VR1)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,13);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096.,1,2);

      ADC1->SQR3 = 0x00000003;			// channel 3 (LM35DZ)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,16);				// display temperature
      TFT_color(Magenta,Black);
      TFT_signed_float((float)result*330./4096.,2,1);

      ADC1->SQR3 = 0x00000012;			// channel 18 (temperature sensor)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      x = (float)result*3.3/4096.;		// calculate temperature
      x = (x-0.76)/0.0025 + 25.;
      TFT_xy(24,19);				// display voltage
      TFT_color(Green,Black);
      TFT_signed_float(x,2,1);

      ADC1->SQR3 = 0x00000011;			// channel 17 (VREFINT = 1.21V)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,22);				// display voltage
      TFT_color(Green,Black);
      TFT_signed_float((float)result*3.3/4096.,1,2);
    }
}
