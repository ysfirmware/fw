/* ============================================================================ */
/*	Interrupt_Exti12.c : OK-STM32F4 키트 인터럽트 테스트 프로그램		*/
/* ============================================================================ */
/*			    Designed and programmed by Duck-Yong Yoon in 2012.  */

#include "stm32f4xx.h"
#include "OK-STM32F4.h"

volatile unsigned int interrupt_count = 0;

void EXTI15_10_IRQHandler(void)
{
   interrupt_count++;
   if(interrupt_count >= 4) interrupt_count = 0;
   Beep();

   while((GPIOE->IDR & 0x00001000) == 0);	// debouncing of KEY1
   Delay_ms(20);

   EXTI->PR = 0x00001000;			// clear pending bit of EXTI12
}

void Interrupt_enable(U08 position)		/* enable interrupt */
{
  SCB->VTOR = 0x08000000;			// set vector table from 0x08000000
  NVIC->ISER[position / 32] = 1 << (position % 32); // enable interrupt
}

void Interrupt_disable(U08 position)		/* disable interrupt */
{
  NVIC->ICER[position / 32] = 1 << (position % 32); // disable interrupt
}

int main(void)
{
  Initialize_MCU();				// initialize STM32F407VGT6 MCU
  Delay_ms(200);
  Initialize_TFT_LCD();				// initialize TFT-LCD

  TFT_string(2,10, White,Magenta, " 인터럽트 테스트 프로그램 ");
  TFT_string(2,13, White,  Black, "      (EXTI12 = KEY1)     ");

  SYSCFG->EXTICR[3] = 0x00000004;		// EXTI12 = PE12(KEY1)
  EXTI->IMR = 0x00001000;			// enable EXTI12 interrupt
  EXTI->FTSR = 0x00001000;			// EXTI12 falling edge
  Interrupt_enable(EXTI15_10_IRQn);		// EXTI12 = 40

  while(1)
    { if(interrupt_count == 0)
        { TFT_string(7,30, Cyan,Black, "비프 / LED1 점등");
          GPIOE->BSRRL = 0x0100;		// LED1 on 
	  GPIOE->BSRRH = 0x0800;		// LED4 off 
        }
      else if(interrupt_count == 1)
        { TFT_string(7,30, Green,Black, "비프 / LED2 점등"); 
          GPIOE->BSRRL = 0x0200;		// LED2 on 
	  GPIOE->BSRRH = 0x0100;		// LED1 off 
        }
      else if(interrupt_count == 2)
        { TFT_string(7,30, Red,Black, "비프 / LED3 점등");
          GPIOE->BSRRL = 0x0400;		// LED3 on 
	  GPIOE->BSRRH = 0x0200;		// LED2 off 
        }
      else if(interrupt_count == 3) 
        { TFT_string(7,30, White,Black, "비프 / LED4 점등");
          GPIOE->BSRRL = 0x0800;		// LED4 on   
	  GPIOE->BSRRH = 0x0400;		// LED3 off 
        }

      Delay_ms(1);				// delay for repeated interrupt
    }
}
