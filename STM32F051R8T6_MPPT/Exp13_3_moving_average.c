/* ============================================================================ */
/*	Exp13_3_moving_average.c : A/D 컨버터의 단일변환 모드 사용(이동평균)	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned char i, ADC2_count, ADC3_count, ADC2_start_flag, ADC3_start_flag;
  unsigned short result, ADC2_buffer[16], ADC3_buffer[16];

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_3.c    ");
  Beep();

  TFT_string(5, 5,White,Magenta," A/D 변환 결과의 이동평균 처리 ");
  TFT_string(9,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(9,18,White,Black,"ADC3 (LM35DZ) = +00.0 C");
  TFT_xy(30,18);
  TFT_English(0x7F);

  ADC2_count = 0;				// initialize variables
  ADC3_count = 0;
  ADC2_start_flag = 0;
  ADC3_start_flag = 0;

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3
  RCC->APB2ENR |= 0x00000200;			// enable ADC clock
  RCC->CFGR |= 0x00004000;			// ADC clock = PCLK/4 = 48MHz/4 = 12MHz
  RCC->CFGR3 |= 0x00000100;

  ADC1->CR = 0x00000001;			// enable ADC
  ADC1->SMPR = 0x00000005;			// sampling time = 55.5 cycle
  ADC1->CFGR1 = 0x00000000;			// 12-bit resolution, single conversion mode
  ADC1->CFGR2 = 0x80000000;			// PCLK/4 = 12MHz

  while(1)
    { ADC1->CHSELR = 0x00000004;		// ********** channel 2 (VR1)
      ADC1->CR |= 0x00000004;			// start conversion by software
      while(!(ADC1->ISR & 0x00000004));		// wait for end of conversion
      result = ADC1->DR;

      if(ADC2_start_flag == 1)			// if not start new, store result
        { ADC2_buffer[ADC2_count] = result;
          ADC2_count++;
          ADC2_count = ADC2_count % 16;
        }
      else
        { for(i = 0; i < 16; i++)		// if start new, initialize buffer 
            ADC2_buffer[i] = result;
          ADC2_start_flag = 1;
        }

      result = 0;
      for(i = 0; i < 16; i++)			// calculate moving average of buffer
        result += ADC2_buffer[i];
      result = result >> 4;
      TFT_xy(25,13);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096., 1, 2);

      ADC1->CHSELR = 0x00000008;		// ********** channel 3 (LM35DZ)
      ADC1->CR |= 0x00000004;			// start conversion by software
      while(!(ADC1->ISR & 0x00000004));		// wait for end of conversion
      result = ADC1->DR;

      if(ADC3_start_flag == 1)			// if not start new, store result
        { ADC3_buffer[ADC3_count] = result;
          ADC3_count++;
          ADC3_count = ADC3_count % 16;
        }
      else
        { for(i = 0; i < 16; i++)		// if start new, initialize buffer 
            ADC3_buffer[i] = result;
          ADC3_start_flag = 1;
        }

      result = 0;
      for(i = 0; i < 16; i++)			// calculate moving average of buffer
        result += ADC3_buffer[i];
      result = result >> 4;
      TFT_xy(25,18);				// display temperature
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*330./4096., 2, 1);

      Delay_ms(250);
    }
}
