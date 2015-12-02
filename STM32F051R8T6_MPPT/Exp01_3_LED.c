/* ============================================================================ */
/*	Exp01_3_LED.c : �⺻ LED �� ���� ��� ���α׷�(48MHz SYSCLK ���)       */
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
