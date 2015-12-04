/* ============================================================================ */
/*	Exp05_3_numeric(2).c : TFT-LCD ��ġ ������ ��� ���α׷�(2)		*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned int x, y;
  float a, b;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");          // display title
  LCD_string(0xC0,"   Exp05_3.c    ");

  while(1)
    { TFT_clear_screen();			// ����
      TFT_string(6,0,White, Magenta," Multiplication Table(����) ");

      for(x = 2; x <= 9; x++)
        { for(y = 1; y <= 9; y++)
	    { TFT_color(Green, Black);		// ���ڸ� ������
	      TFT_xy( 5,3*y); TFT_English(x + '0');
	      TFT_xy( 7,3*y); TFT_English('x');
              TFT_xy( 9,3*y); TFT_English(y + '0');
              TFT_xy(11,3*y); TFT_English('=');
              TFT_xy(13,3*y); TFT_unsigned_decimal(x*y,0,2);

              TFT_color(Yellow, Black);		// 10�ڸ� ������
	      TFT_xy(21,3*y); TFT_unsigned_decimal(x*10,0,2);
	      TFT_xy(24,3*y); TFT_English('x');
              TFT_xy(26,3*y); TFT_unsigned_decimal(y*10,0,2);
              TFT_xy(29,3*y); TFT_English('=');
              TFT_xy(31,3*y); TFT_unsigned_decimal((x*10)*(y*10),0,4);
            }
          Beep();
          Delay_ms(3000);
        }

      TFT_clear_screen();			// �Ǽ�
      TFT_string(6,0,White, Magenta," Multiplication Table(�Ǽ�) ");

      for(x = 1,a = 1.1; x <= 9; x++, a += 1.1)
        { for(y = 1, b = 1.1; y <= 9; y++, b += 1.1)
	    { TFT_color(Green, Black);		// �Ǽ� ������
	      TFT_xy(11,3*y); TFT_unsigned_float(a,1,1);
	      TFT_xy(15,3*y); TFT_English('x');
              TFT_xy(17,3*y); TFT_unsigned_float(b,1,1);
              TFT_xy(21,3*y); TFT_English('=');
              TFT_xy(23,3*y); TFT_unsigned_float(a*b,2,2);
            }
          Beep();
          Delay_ms(3000);
        }
    }
}
