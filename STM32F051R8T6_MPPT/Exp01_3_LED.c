/* ============================================================================ */
/*	Exp01_3_LED.c : 기본 LED 및 버저 출력 프로그램(48MHz SYSCLK 사용)       */
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned int i;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
 
  while(1)
    { for(i = 0; i < 5; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(500);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(500);
        }
      Beep();					// beep
    }
}
