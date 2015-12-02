/* ============================================================================ */
/*		Exp04_1_bouncing.c : 키입력 값 표시(디바운싱 미사용)		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned int key;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp04_1.c    ");
  Beep();

  TFT_string(14,9,White,Magenta," 키입력 값 ");
  TFT_string(7,15,White,  Black,"GPIOB = 0b0000000000000000");

  while(1)
    { key = GPIOB->IDR & 0x00000F00;		// read GPIOB
      TFT_xy(17,15);				// display value
      TFT_color(Cyan,Black);
      TFT_binary(key,16);
    }
}
