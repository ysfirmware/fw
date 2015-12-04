/* ============================================================================ */
/*		Exp02_5_loop(1).c : 무한루프 처리 프로그램(1)			*/
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
  LCD_string(0xC0,"   Exp02_5.c    ");

  for(i = 0; i < 5; i++)			// ***** 작업 수행 *****
    { GPIOE->BSRR = 0x00008000;			// LED on
      Delay_ms(500);
      GPIOE->BSRR = 0x80000000;			// LED off
      Delay_ms(500);
    }						// ***** 작업 종료 *****
  Beep();					// beep

  while(1);					// ***** 무한 루프 *****
}
