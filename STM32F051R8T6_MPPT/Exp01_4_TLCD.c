/* ============================================================================ */
/*	Exp01_4_TLCD.c : 기본 텍스트 LCD 출력 프로그램(48MHz SYSCLK 사용)       */
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned int i;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title

  while(1)
    { LCD_string(0xC0," 200ms interval ");
      Beep();
      for(i = 0; i < 12; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(200);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(200);
        }

      LCD_string(0xC0," 400ms interval ");
      Beep();
      for(i = 0; i < 6; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(400);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(400);
        }

      LCD_string(0xC0," 600ms interval ");
      Beep();
      for(i = 0; i < 4; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(600);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(600);
        }
    }
}

