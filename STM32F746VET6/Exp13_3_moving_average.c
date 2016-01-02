/* ============================================================================ */
/*	Exp13_3_moving_average.c : A/D 컨버터의 단일변환 모드 사용(이동평균)	*/
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned char i, ADC1_ch2_count, ADC1_ch3_count, ADC1_ch2_start_flag, ADC1_ch3_start_flag;
  unsigned short result, ADC1_ch2_buffer[16], ADC1_ch3_buffer[16];

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_3.c    ");
  Beep();

  TFT_string(5, 5,White,Magenta," A/D 변환 결과의 이동평균 처리 ");
  TFT_string(9,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(9,18,White,Black,"ADC3 (LM35DZ) = +00.0 C");
  TFT_xy(30,18);
  TFT_English(0x7F);

  ADC1_ch2_count = 0;				// initialize variables
  ADC1_ch3_count = 0;
  ADC1_ch2_start_flag = 0;
  ADC1_ch3_start_flag = 0;

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock

  ADC1->SMPR1 = 0x07FC0000;			// sampling time of channel 16~18 = 480 cycle
  ADC1->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  ADC->CCR = 0x00800000;			// enable channel 16~18
  ADC->CCR = 0x00830000;	                // TSVREFE = 1, VBATE = 0, 54MHz / 8 = 6.75Mhz 
  ADC1->CR1 = 0x00000800;			// 12-bit resolution, single discontinuous mode
  ADC1->CR2 = 0x00000001;			// right alignment, ADON = 1
  
  while(1)
    { ADC1->SQR3 = 0x00000002;			// channel 2 (VR1)
      Delay_us(1);
      ADC1->CR2 |= 0x40000000;		        // start conversion by software
      while(!(ADC1->SR & 0x00000002));	        // wait A/D conversion complete
      result = ADC1->DR;

      if(ADC1_ch2_start_flag == 1)		// if not start new, store result
        { ADC1_ch2_buffer[ADC1_ch2_count] = result;
          ADC1_ch2_count++;
          ADC1_ch2_count = ADC1_ch2_count % 16;
        }
      else
        { for(i = 0; i < 16; i++)		// if start new, initialize buffer 
            ADC1_ch2_buffer[i] = result;
          ADC1_ch2_start_flag = 1;
        }
      
      result = 0;
      for(i = 0; i < 16; i++)			// calculate moving average of buffer
        result += ADC1_ch2_buffer[i];
      result = result >> 4;
      TFT_xy(25,13);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096., 1, 2);
      
      ADC1->SQR3 = 0x00000003;			// channel 3 (LM35DZ)
      Delay_us(1);
      ADC1->CR2 |= 0x40000000;		        // start conversion by software
      while(!(ADC1->SR & 0x00000002));	        // wait A/D conversion complete
      result = ADC1->DR;

      if(ADC1_ch3_start_flag == 1)		// if not start new, store result
        { ADC1_ch3_buffer[ADC1_ch3_count] = result;
          ADC1_ch3_count++;
          ADC1_ch3_count = ADC1_ch3_count % 16;
        }
      else
        { for(i = 0; i < 16; i++)		// if start new, initialize buffer 
            ADC1_ch3_buffer[i] = result;
          ADC1_ch3_start_flag = 1;
        }

      result = 0;
      for(i = 0; i < 16; i++)			// calculate moving average of buffer
        result += ADC1_ch3_buffer[i];
      result = result >> 4;
      TFT_xy(25,18);				// display temperature
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*330./4096., 2, 1);

      Delay_ms(10);
    }
}
