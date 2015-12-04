/* ============================================================================ */
/*		Exp02_3_time_delay(1).c : 시간지연 함수 테스트 프로그램		*/
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
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title
  LCD_string(0xC0,"   Exp02_3.c    ");

  TFT_string(0,8,White,Magenta,"   시간지연 함수  정확도 테스트(TP10)   ");

  while(1)
    { TFT_string(0,16,Cyan,Black,"             H 펄스 = 10 us             ");
      Beep();
      for(i = 0; i < 200000; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_us(10);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_us(10);
        }

      TFT_string(0,16,Cyan,Black,"             H 펄스 = 100 us            ");
      Beep();
      for(i = 0; i < 50000; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_us(100);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_us(10);
        }

      TFT_string(0,16,Cyan,Black,"             H 펄스 = 1 ms              ");
      Beep();
      for(i = 0; i < 5000; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(1);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_us(10);
        }

      TFT_string(0,16,Cyan,Black,"             H 펄스 = 10 ms             ");
      Beep();
      for(i = 0; i < 500; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(10);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_us(10);
        }

      TFT_string(0,16,Cyan,Black,"             H 펄스 = 100 ms            ");
      Beep();
      for(i = 0; i < 50; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(100);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_us(10);
        }
    }
}
