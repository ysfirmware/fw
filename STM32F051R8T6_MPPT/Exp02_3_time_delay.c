/* ============================================================================ */
/*		Exp02_3_time_delay.c : 시간지연 루틴 테스트 프로그램		*/
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
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp02_3.c    ");

  TFT_string( 0,8,White,Magenta,"   시간지연 루틴  정확도 테스트 (TP1)   ");

  while(1)
    { TFT_string( 0,16,Cyan,Black,"             H 펄스 = 10 us             ");
      Beep();
      for(i = 0; i < 200000; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_us(10);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_us(10);
        }

      TFT_string( 0,16,Cyan,Black,"             H 펄스 = 100 us            ");
      Beep();
      for(i = 0; i < 50000; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_us(100);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_us(10);
        }

      TFT_string( 0,16,Cyan,Black,"             H 펄스 = 1 ms              ");
      Beep();
      for(i = 0; i < 5000; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(1);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_us(10);
        }

      TFT_string( 0,16,Cyan,Black,"             H 펄스 = 10 ms             ");
      Beep();
      for(i = 0; i < 500; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(10);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_us(10);
        }

      TFT_string( 0,16,Cyan,Black,"             H 펄스 = 100 ms            ");
      Beep();
      for(i = 0; i < 50; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(100);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_us(10);
        }
    }
}
