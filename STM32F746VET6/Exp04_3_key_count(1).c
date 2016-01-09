/* ============================================================================ */
/*		Exp04_3_key_count(1).c : 키입력 카운트(디바운싱 미사용)		*/
/* ============================================================================ */
/*					programmed by Seung-Yong Lee in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned char key;
  unsigned int KEY1_count, KEY2_count, KEY3_count, KEY4_count;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp04_3.c    ");
  Beep();

  TFT_string(8, 8, White,Magenta, " 키입력 카운트 프로그램 ");
  TFT_string(7,12, White,  Black, "    KEY1 입력 = 000000    ");
  TFT_string(7,15, White,  Black, "    KEY2 입력 = 000000    ");
  TFT_string(7,18, White,  Black, "    KEY3 입력 = 000000    ");
  TFT_string(7,21, White,  Black, "    KEY4 입력 = 000000    ");

  KEY1_count = 0;				// initialize variables
  KEY2_count = 0;
  KEY3_count = 0;
  KEY4_count = 0;

  while(1)
    { key = GPIOC->IDR & 0x0000000F;		// key input
      switch(key)
        { case KEY1 : KEY1_count++;		// increment KEY1 count
	              TFT_xy(23,12);		// display KEY1 count
  	              TFT_color(Green,Black);
	              TFT_unsigned_decimal(KEY1_count,1,6);
                      break;
          case KEY2 : KEY2_count++;		// increment KEY2 count
	              TFT_xy(23,15);		// display KEY2 count
  	              TFT_color(Green,Black);
	              TFT_unsigned_decimal(KEY2_count,1,6);
                      break;
          case KEY3 : KEY3_count++;		// increment KEY3 count
	              TFT_xy(23,18);		// display KEY3 count
  	              TFT_color(Green,Black);
	              TFT_unsigned_decimal(KEY3_count,1,6);
                      break;
          case KEY4 : KEY4_count++;		// increment KEY4 count
	              TFT_xy(23,21);		// display KEY4 count
  	              TFT_color(Green,Black);
	              TFT_unsigned_decimal(KEY4_count,1,6);
                      break;
          default:    break;
        }
    }
}
