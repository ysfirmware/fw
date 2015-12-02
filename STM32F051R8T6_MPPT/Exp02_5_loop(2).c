/* ============================================================================ */
/*		Exp02_5_loop(2).c : 무한루프 처리 프로그램(2)			*/
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
  LCD_string(0xC0,"   Exp02_5.c    ");

  while(1)					// ***** 무한 루프의 시작 *****
    { for(i = 0; i < 5; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(500);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(500);
        }
      Beep();					// beep
    }						// ***** 무한 루프의 끝 *****
}
