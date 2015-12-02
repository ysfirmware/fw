/* ============================================================================ */
/*   Exp13_6_calibration_AWD.c : A/D �������� Ķ���극�̼� �� �Ƴ��α� ��ġ��	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned char i, ADC2_count, ADC3_count, ADC2_start_flag, ADC3_start_flag;
  unsigned short calibration, result, ADC2_buffer[16], ADC3_buffer[16];

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_6.c    ");
  Beep();

  TFT_string(3, 5,White,Magenta," A/D �������� Ķ���극�̼� �� AWD ");
  TFT_string(7, 8,Yellow,Black,"(Ķ���극�̼� �� = 0x0000)");
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
  RCC->APB2ENR |= 0x00000200;			// enable ADC clock
  RCC->CFGR |= 0x00004000;			// ADC clock = PCLK/4 = 48MHz/4 = 12MHz
  RCC->CFGR3 |= 0x00000100;

  ADC1->CR = 0x80000000;			// start calibration
  while(ADC1->CR & 0x80000000);			// wait for end of calibration
  calibration = ADC1->DR;			// read and display calibration value
  TFT_xy(26,8);
  TFT_color(Yellow,Black);
  TFT_0x_hexadecimal(calibration, 4);

  ADC1->CR = 0x00000001;			// enable ADC
  ADC1->SMPR = 0x00000005;			// sampling time = 55.5 cycle
  ADC1->CFGR1 = 0x08800000;			// 12-bit resolution, AWDEN=1, AWDCH=2
  ADC1->CFGR2 = 0x80000000;			// PCLK/4 = 12MHz
  ADC1->TR = 0x0C1F04D9;                        // HT=0x0C1F, LT=04D9 for analog watchdog

  while(1)
    { ADC1->ISR |= 0x00000080;                  // clear AWD

      ADC1->CHSELR = 0x00000004;		// ********** channel 2 (VR1)
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

      TFT_xy(25,15);				// display voltage using calibration value
      TFT_color(Yellow,Black);
      if(result >= calibration) result -= calibration;
      TFT_signed_float((float)result*3.3/4096., 1, 2);

      if(ADC1->ISR & 0x00000080)
        TFT_string(1,26,Magenta,Black,"ADC2 is out of analog watchdog window.");
      else
        TFT_string(1,26,Green,Black,"  ADC2 is in analog watchdog window.  ");

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

      TFT_xy(25,20);				// display temperature using calibration value
      TFT_color(Yellow,Black);
      if(result >= calibration) result -= calibration;
      TFT_signed_float((float)result*330./4096., 2, 1);

      Delay_ms(250);
    }
}
