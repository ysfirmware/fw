/* ============================================================================ */
/*		Exp13_1_single.c : A/D 컨버터의 단일변환 모드 사용		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned short result;
  unsigned char key;
  unsigned int PWM;				// pulse width

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_1.c    ");
  Beep();

  TFT_string(9,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(9,16,White,Black,"ADC3 (LM35DZ) = +00.0 C");

  PWM=2399;
  
  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3
  RCC->APB2ENR |= 0x00000200;			// enable ADC clock
  RCC->CFGR |= 0x00004000;			// ADC clock = PCLK/4 = 48MHz/4 = 12MHz
  RCC->CFGR3 |= 0x00000100;

  ADC1->CR = 0x00000001;			// enable ADC
  ADC->CCR = 0x01C00000;			// VBATEN = TSEN = VREFEN = 1
  ADC1->SMPR = 0x00000005;			// sampling time = 55.5 cycle
  ADC1->CFGR1 = 0x00000000;			// 12-bit resolution, single conversion mode
  ADC1->CFGR2 = 0x80000000;			// PCLK/4 = 12MHz

  GPIOA->MODER &= 0xFF3FFFFF;			// PA11 = alternate function mode
  GPIOA->MODER |= 0x00800000;
  GPIOA->AFR[1] |= 0x00002000;			// PA11 = TIM1_CH4

  RCC->APB2ENR |= 0x00000800;			// enable TIM1 clock
  TIM1->PSC = 0;				// 48MHz/(47999+1) = 1kHz
  TIM1->ARR = 4799;				// 1kHz / (999+1) = 1Hz
  TIM1->CCR4 = PWM;
  TIM1->CNT = 0;				// clear counter
  TIM1->CCMR2 = 0x6C00;				// OC4M = 110(PWM mode 1), CC4S = 00(output)
  TIM1->CCER = 0x1000;				// CC4E = 1(enable OC4 output)
  TIM1->BDTR = 0x8000;				// MOE = 1
  TIM1->CR1 = 0x0005;				// edge-aligned, up-counter, enable TIM1
  
  while(1)
    { 
      ADC1->CHSELR = 0x00000004;		// channel 2 (VR1)
      ADC1->CR |= 0x00000004;			// start conversion by software
      while(!(ADC1->ISR & 0x00000004));		// wait for end of conversion
      result = ADC1->DR;
      TFT_xy(25,13);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096., 1, 2);

      ADC1->CHSELR = 0x00000008;		// channel 3 (LM35DZ)
      ADC1->CR |= 0x00000004;			// start conversion by software
      while(!(ADC1->ISR & 0x00000004));		// wait for end of conversion
      result = ADC1->DR;
      TFT_xy(25,16);				// display temperature
      TFT_color(Magenta,Black);
      TFT_signed_float((float)result*330./4096., 2, 1);
      
      key = Key_input();			// key input
      if((key == KEY3) && (PWM != 899))		// if KEY3, increment duty ratio by 10%
        { PWM += 100;
          TIM1->CCR4  = PWM;
          TFT_color(Yellow,Black);
          TFT_xy(26,13);
	  TFT_unsigned_decimal(PWM+1,0,4);
          TFT_xy(27,16);
	  TFT_unsigned_decimal((PWM+1)/10,0,3);
        }
      else if((key == KEY4) && (PWM != 99))	// if KEY4, decrement duty ratio by 10%
        { PWM -= 100;
          TIM1->CCR4  = PWM;
          TFT_color(Yellow,Black);
          TFT_xy(26,13);
	  TFT_unsigned_decimal(PWM+1,0,4);
          TFT_xy(27,16);
	  TFT_unsigned_decimal((PWM+1)/10,0,3);
	}
    }
}
