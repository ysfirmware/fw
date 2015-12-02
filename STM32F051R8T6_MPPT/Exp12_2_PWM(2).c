/* ============================================================================ */
/*		Exp12_2_PWM(2).c : 2Ã¤³Î PWM Ãâ·Â ÇÁ·Î±×·¥			*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned char key;
  unsigned int PWM1;				// pulse width of TIM15_CH1
  unsigned int PWM2;				// pulse width of TIM15_CH2

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp12_2.c    ");
  Beep();

  TFT_string(5, 5, White,Magenta, " Å¸ÀÌ¸Ó TIM15ÀÇ PWM Ãâ·Â ¸ðµå ");
  TFT_string(6, 9, Cyan,   Black, "TIM15_CH1 : ÁÖ±â   = 1000 us");
  TFT_string(6,11, Cyan,   Black, "  (OC1)     ÆÞ½ºÆø =  200 us");
  TFT_string(6,13, Cyan,   Black, "            µàÆ¼ºñ =   20 %");
  TFT_string(6,16, Cyan,   Black, "TIM15_CH2 : ÁÖ±â   = 1000 us");
  TFT_string(6,18, Cyan,   Black, "  (OC2)     ÆÞ½ºÆø =  500 us");
  TFT_string(6,20, Cyan,   Black, "            µàÆ¼ºñ =   50 %");
  TFT_string(4,25, White,  Black, "KEY1~KEY4·Î ÆÞ½ºÆøÀ» °¡º¯ÇÏ½Ã¿À.");
  TFT_string( 1,28,White,Red," CH1( ) ");
  TFT_xy( 6,28); TFT_English(0x18);
  TFT_string(11,28,White,Red," CH1( ) ");
  TFT_xy(16,28); TFT_English(0x19);
  TFT_string(21,28,White,Blue," CH2( ) ");
  TFT_xy(26,28); TFT_English(0x18);
  TFT_string(31,28,White,Blue," CH2( ) ");
  TFT_xy(36,28); TFT_English(0x19);

  PWM1 = 199;					// initialize PWM1 = 199 (duty ratio 20%)
  PWM2 = 499;					// initialize PWM2 = 499 (duty ratio 50%)

  GPIOB->MODER &= 0x0FFFFFFF;			// PB15/PB14 = alternate function mode
  GPIOB->MODER |= 0xA0000000;
  GPIOB->AFR[1] |= 0x11000000;			// PB15 = TIM15_CH2, PB14 = TIM15_CH1

  RCC->APB2ENR |= 0x00010000;			// enable TIM15 clock
  TIM15->PSC = 47;				// 48MHz/(47+1) = 1MHz
  TIM15->ARR = 999;				// 1MHz/(999+1) = 1kHz
  TIM15->CCR1 = PWM1;
  TIM15->CCR2 = PWM2;
  TIM15->CNT = 0;				// clear counter
  TIM15->CCMR1 = 0x6C6C;			// OC2M=OC1M=110(PWM mode 1), CC2S=CC1S=00(output)
  TIM15->CCER = 0x0011;				// CC2E = CC1E = 1(enable OC2/OC1 output)
  TIM15->BDTR = 0x8000;				// MOE = 1
  TIM15->CR1 = 0x0005;				// enable TIM15

  while(1)
    { key = Key_input();			// key input
      if((key == KEY1) && (PWM1 != 899))	// if KEY1, increment CH1 duty ratio by 10%
        { PWM1 += 100;
          TIM15->CCR1  = PWM1;
          TFT_color(Yellow,Black);
          TFT_xy(27,11);
	  TFT_unsigned_decimal(PWM1+1,0,4);
          TFT_xy(28,13);
          TFT_unsigned_decimal((PWM1+1)/10,0,3);
        }
      else if((key == KEY2) && (PWM1 != 99))	// if KEY2, decrement CH1 duty ratio by 10%
        { PWM1 -= 100;
          TIM15->CCR1  = PWM1;
          TFT_color(Yellow,Black);
          TFT_xy(27,11);
	  TFT_unsigned_decimal(PWM1+1,0,4);
          TFT_xy(28,13);
          TFT_unsigned_decimal((PWM1+1)/10,0,3);
	}
      else if((key == KEY3) && (PWM2 != 899))	// if KEY3, increment CH2 duty ratio by 10%
        { PWM2 += 100;
          TIM15->CCR2  = PWM2;
          TFT_color(Yellow,Black);
          TFT_xy(27,18);
	  TFT_unsigned_decimal(PWM2+1,0,4);
          TFT_xy(28,20);
          TFT_unsigned_decimal((PWM2+1)/10,0,3);
        }
      else if((key == KEY4) && (PWM2 != 99))	// if KEY4, decrement CH2 duty ratio by 10%
        { PWM2 -= 100;
          TIM15->CCR2  = PWM2;
          TFT_color(Yellow,Black);
          TFT_xy(27,18);
	  TFT_unsigned_decimal(PWM2+1,0,4);
          TFT_xy(28,20);
          TFT_unsigned_decimal((PWM2+1)/10,0,3);
	}
    }
}
