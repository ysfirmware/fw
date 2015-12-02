/* ============================================================================ */
/*	Exp06_3_transparent.c : TFT-LCD ���� �۾� �� �ܰ���ü ��� ���α׷�	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");          // display title
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
    }
}
