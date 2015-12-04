/* ============================================================================ */
/*	Exp03_1_shift.c : 텍스트 LCD 기본 출력 및 시프트 동작 프로그램		*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned int i;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();                             // initialize text LCD module

  while(1)
    { Beep();
      LCD_string(0x80," OK-STM746 V1.0 ");      // display logo
      LCD_string(0xC0," STM32F746VET6  ");      // display message 1
      Delay_ms(2000);
      LCD_string(0xC0,"   2015/11/01   ");      // display message 2
      Delay_ms(2000);

      Beep();
      LCD_string(0x80,"0123456789ABCDEF0123456789ABCDEF"); // display long line
      LCD_string(0xC0,"Go shift left and right 16 times");
      Delay_ms(1000);

      for(i = 0; i < 16; i++)			// shift left
        { LCD_command(0x18);
          Delay_ms(300);
        }
      Delay_ms(1000);

      for(i = 0; i < 16; i++)			// shift right
        { LCD_command(0x1C);
          Delay_ms(300);
        }
      Delay_ms(1000);
    }
}

