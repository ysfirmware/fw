/* ============================================================================ */
/*	Exp01_3_LED.c : 기본 LED 및 버저 출력 프로그램(216MHz SYSCLK 사용)      */
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned int i;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
 
  while(1)
    { for(i = 0; i < 5; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(500);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(500);
        }

      Beep();					// beep
    }
}
