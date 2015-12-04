/* ============================================================================ */
/*	Exp06_3_transparent.c : TFT-LCD ���� �۾� �� �ܰ���ü ��� ���α׷�	*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include "OK-STM746_large.h"

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");          // display title
  LCD_string(0xC0,"   Exp06_3.c    ");

  while(1)
    { TFT_color_screen(White);
      TFT_string(4, 2, White,Magenta," ���� �۾� �� �ܰ���ü ��� �� ");
      TFT_string(7, 7, White,Blue,   " (1) �ܰ����� ���� ��� ");
      TFT_string(3,10, Yellow,Transparent,"�ѱ�, English, ABCDEF, 0123456789");
      TFT_string(6,13, Yellow,Transparent,"�۾��� �� ������ �ʽ��ϴ�.");

      TFT_string(7,18, White,Blue,   " (2) �ܰ����� ���� ��� ");
      TFT_outline(Blue);
      TFT_string(3,22, Yellow,Transparent,"�ѱ�, English, ABCDEF, 0123456789");
      TFT_string(7,25, Yellow,Transparent,"���� �۾��� �� ���Դϴ�.");
      TFT_outline(Transparent);
      Beep();
      Delay_ms(5000);

      TFT_color_screen(Yellow);
      TFT_string(4, 2, White,Magenta," ���� �۾� �� �ܰ���ü ��� �� ");
      TFT_string(7, 7, White,Blue,   " (1) �ܰ����� ���� ��� ");
      TFT_string(3,10, White,Transparent,"�ѱ�, English, ABCDEF, 0123456789");
      TFT_string(6,13, White,Transparent,"�۾��� �� ������ �ʽ��ϴ�.");

      TFT_string(7,18, White,Blue,   " (2) �ܰ����� ���� ��� ");
      TFT_outline(Blue);
      TFT_string(3,22, White,Transparent,"�ѱ�, English, ABCDEF, 0123456789");
      TFT_string(7,25, White,Transparent,"���� �۾��� �� ���Դϴ�.");
      TFT_outline(Transparent);
      Beep();
      Delay_ms(5000);

      TFT_color_screen(White);
      TFT_string(4, 2, White,Magenta," ���� �۾� �� �ܰ���ü ��� �� ");
      TFT_string(7, 7, White,Blue,   " (1) �ܰ����� ���� ��� ");
      TFT_string_large(3,10, Yellow,Transparent,"�ѱ�, English, 01234");
      TFT_string_large(0,13, Yellow,Transparent,"�۾��� �� ������ �ʽ��ϴ�.");

      TFT_string(7,18, White,Blue,   " (2) �ܰ����� ���� ��� ");
      TFT_outline(Blue);
      TFT_string_large(3,22, Yellow,Transparent,"�ѱ�, English, 01234");
      TFT_string_large(1,25, Yellow,Transparent,"���� �۾��� �� ���Դϴ�.");
      TFT_outline(Transparent);
      Beep();
      Delay_ms(5000);

      TFT_color_screen(Yellow);
      TFT_string(4, 2, White,Magenta," ���� �۾� �� �ܰ���ü ��� �� ");
      TFT_string(7, 7, White,Blue,   " (1) �ܰ����� ���� ��� ");
      TFT_string_large(3,10, White,Transparent,"�ѱ�, English, 01234");
      TFT_string_large(0,13, White,Transparent,"�۾��� �� ������ �ʽ��ϴ�.");

      TFT_string(7,18, White,Blue,   " (2) �ܰ����� ���� ��� ");
      TFT_outline(Blue);
      TFT_string_large(3,22, White,Transparent,"�ѱ�, English, 01234");
      TFT_string_large(1,25, White,Transparent,"���� �۾��� �� ���Դϴ�.");
      TFT_outline(Transparent);
      Beep();
      Delay_ms(5000);
    }
}
