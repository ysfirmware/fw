/* ============================================================================ */
/*	Interrupt_Exti0.c : OK-STM32F4 ŰƮ ���ͷ�Ʈ �׽�Ʈ ���α׷�		*/
/* ============================================================================ */
/*			    Designed and programmed by Duck-Yong Yoon in 2012.  */

#include "stm32f4xx.h"
#include "OK-STM32F4.h"

volatile unsigned int interrupt_count = 0;

void EXTI0_IRQHandler(void)
{
   interrupt_count++;
   if(interrupt_count >= 4) interrupt_count = 0;
   Beep();

   EXTI->PR = 0x00000001;			// clear pending bit of EXTI0
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

  TFT_string(2,10, White,Magenta, " ���ͷ�Ʈ �׽�Ʈ ���α׷� ");
  TFT_string(2,13, White,  Black, "         (EXTI0)          ");

  EXTI->IMR = 0x00000001;			// enable EXTI0 interrupt
  Interrupt_enable(EXTI0_IRQn);			// EXTI0 = 6

  while(1)
    { EXTI->SWIER = 0x00000001;
      Delay_ms(1);				// delay for software interrupt

      if(interrupt_count == 0)
        { TFT_string(7,30, Cyan,Black, "���� / LED1 ����");
          GPIOE->BSRRL = 0x0100;		// LED1 on 
	  GPIOE->BSRRH = 0x0800;		// LED4 off 
        }
      else if(interrupt_count == 1)
        { TFT_string(7,30, Green,Black, "���� / LED2 ����"); 
          GPIOE->BSRRL = 0x0200;		// LED2 on 
	  GPIOE->BSRRH = 0x0100;		// LED1 off 
        }
      else if(interrupt_count == 2)
        { TFT_string(7,30, Red,Black, "���� / LED3 ����");
          GPIOE->BSRRL = 0x0400;		// LED3 on 
	  GPIOE->BSRRH = 0x0200;		// LED2 off 
        }
      else if(interrupt_count == 3) 
        { TFT_string(7,30, White,Black, "���� / LED4 ����");
          GPIOE->BSRRL = 0x0800;		// LED4 on   
	  GPIOE->BSRRH = 0x0400;		// LED3 off 
        }

      Delay_ms(999);				// delay for 1 sec
  }
}
