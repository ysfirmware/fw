/* ============================================================================ */
/*		Exp04_2_debouncing.c : 키입력(디바운싱 사용)			*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned char key;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp04_2.c    ");
  Beep();

  TFT_string(6, 9,White,Magenta," 키 KEY1~KEY4를 누르시오 ! ");

  while(1)
    { key = Key_input();                       // key input
      switch(key)
        { case KEY1 : GPIOA->BSRR = 0x00000800;
                      TFT_string(14,15, Cyan,Black, "KEY1 is OK !");
		      Delay_ms(100);
		      GPIOA->BRR = 0x00000800;
                      break;
          case KEY2 : GPIOA->BSRR = 0x00000800;
                      TFT_string(14,15, Cyan,Black, "KEY2 is OK !");
		      Delay_ms(100);
		      GPIOA->BRR = 0x00000800;
                      break;
          case KEY3 : GPIOA->BSRR = 0x00000800;
                      TFT_string(14,15, Cyan,Black, "KEY3 is OK !");
		      Delay_ms(100);
		      GPIOA->BRR = 0x00000800;
                      break;
          case KEY4 : GPIOA->BSRR = 0x00000800;
                      TFT_string(14,15, Cyan,Black, "KEY4 is OK !");
		      Delay_ms(100);
		      GPIOA->BRR = 0x00000800;
                      break;
          default:    break;
        }
    }
}
