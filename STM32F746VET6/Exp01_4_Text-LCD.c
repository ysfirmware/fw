/* ============================================================================ */
/*    Exp01_4_Text-LCD.c : 기본 텍스트 LCD 출력 프로그램(216MHz SYSCLK 사용)    */
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned int i;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title

  LCD_data('A');

  while(1)
    {
      LCD_string(0xC0," 200ms interval ");
      Beep();
      for(i = 0; i < 12; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(200);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(200);
        }

      LCD_string(0xC0," 400ms interval ");
      Beep();
      for(i = 0; i < 6; i++)
      for(i = 0; i < 6; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(400);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(400);
        }

      LCD_string(0xC0," 600ms interval ");
      Beep();
      for(i = 0; i < 4; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(600);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(600);
        }
    }
}

