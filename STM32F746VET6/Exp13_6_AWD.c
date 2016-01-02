/* ============================================================================ */
/*   Exp13_6_calibration_AWD.c : A/D 컨버터의 캘리브레이션 및 아날로그 워치독	*/
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include "OK-STM746_large.h"
#include "OK-STM746_xlarge.h"

int main(void)
{
  unsigned char i, ADC2_count, ADC3_count, ADC2_start_flag, ADC3_start_flag;
  unsigned short result, ADC2_buffer[16], ADC3_buffer[16];

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  
  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_6.c    ");
  Beep();

  TFT_string(3, 5,White,Magenta," A/D 컨버터의 AWD ");

  TFT_string(9,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(9,18,White,Black,"ADC3 (LM35DZ) = +00.0 C");
  TFT_xy(30,18);
  TFT_English(0x7F);
  TFT_string(4,24,White,Black,"LT=0x04D9=1.0V    HT=0x0C1F=2.5V");
  TFT_string(19,24,Magenta,Black,"//");

  ADC2_count = 0;				// initialize variables
  ADC3_count = 0;
  ADC2_start_flag = 0;
  ADC3_start_flag = 0;

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock

  ADC1->SMPR1 = 0x07FC0000;			// sampling time of channel 16~18 = 480 cycle
  ADC1->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  ADC->CCR = 0x00800000;			// enable channel 16~17, ADCCLK = 42MHz/2 = 21MHz
  ADC->CCR = 0x00830000;	                // TSVREFE = 1, VBATE = 0, 54Mhz / 8 = 6.75Mhz 
  ADC1->CR1 = 0x00800800;			// regular channel watchdog eanable,12-bit resolution, single discontinuous mode
  ADC1->CR2 = 0x00000001;			// right alignment, ADON = 1
  
  ADC1->HTR = 0x00000C1F;                       //2.5V
  ADC1->LTR = 0x000004D9;                       //1V

  while(1)
    { ADC1->SR &= 0xFFFFFFFE;                   // clear AWD
      ADC1->SQR3 = 0x00000002;		        // ********** channel 2 (VR1)
      ADC1->CR2 |= 0x40000000;			// start conversion by software
      while(!(ADC1->SR & 0x00000002));		// wait for end of conversion
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

      if(ADC1->SR & 0x00000001)
        TFT_string(1,26,Magenta,Black,"ADC2 is out of analog watchdog window.");
      else
        TFT_string(1,26,Green,Black,"  ADC2 is in analog watchdog window.  ");

      ADC1->SQR3 = 0x00000003;		        // ********** channel 3 (LM35DZ)
      ADC1->CR2 |= 0x40000000;			// start conversion by software
      while(!(ADC1->SR & 0x00000002));		// wait for end of conversion
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
      Delay_ms(10);
    }
}
