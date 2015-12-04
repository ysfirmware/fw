/* ============================================================================ */
/*	Interrupt_Systick.c : OK-STM32F4 키트 인터럽트 테스트 프로그램		*/
/* ============================================================================ */
/*			    Designed and programmed by Duck-Yong Yoon in 2012.  */

#include "stm32f4xx.h"
#include "OK-STM32F4.h"

volatile unsigned int systick_count = 0, systick_flag = 0;

void SysTick_Handler(void)
{
  systick_count++;
  if(systick_count >= 1000)			// 1 sec ?
    { systick_count = 0;
      systick_flag++;
      if(systick_flag >= 4) systick_flag = 0;
      Beep();
    }
}

int main(void)
{
  Initialize_MCU();				// initialize STM32F407VGT6 MCU
  Delay_ms(200);
  Initialize_TFT_LCD();				// initialize TFT-LCD

  TFT_string(2,10, White,Magenta, " 인터럽트 테스트 프로그램 ");
  TFT_string(2,13, White,  Black, "        (SysTick)         ");

  SysTick->LOAD  = 20999;                       // SysTick Reload Register = N-1 for 1ms (21MHz/21000 = 1kHz)
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 168MHz/8 = 21MHz, interrupt request, counter enable 

  while(1)
    { if(systick_flag == 0)
        { TFT_string(7,30, Cyan,Black, "비프 / LED1 점등");
          GPIOE->BSRRL = 0x0100;		// LED1 on 
	  GPIOE->BSRRH = 0x0800;		// LED4 off 
        }
      else if(systick_flag == 1)
        { TFT_string(7,30, Green,Black, "비프 / LED2 점등"); 
          GPIOE->BSRRL = 0x0200;		// LED2 on 
	  GPIOE->BSRRH = 0x0100;		// LED1 off 
        }
      else if(systick_flag == 2)
        { TFT_string(7,30, Red,Black, "비프 / LED3 점등");
          GPIOE->BSRRL = 0x0400;		// LED3 on 
	  GPIOE->BSRRH = 0x0200;		// LED2 off 
        }
      else if(systick_flag == 3) 
        { TFT_string(7,30, White,Black, "비프 / LED4 점등");
          GPIOE->BSRRL = 0x0800;		// LED4 on   
	  GPIOE->BSRRH = 0x0400;		// LED3 off 
        }
    }
}
