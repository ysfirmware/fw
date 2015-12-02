/* ============================================================================ */
/*	Exp06_7_printf.c : �ؽ�Ʈ LCD �� TFT-LCD�� ��� ����ϴ� printf()	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include <stdio.h>
#include "stm32f0xx.h"
#include "OK-STM32F0.h"

unsigned int device = 'L';			// device for printf()
                                                // 'L'=text LCD, 'T' = TFT-LCD
unsigned int high_code = 0;			// high byte of 16-bit Korean code

int putchar(int character)			/* output a character to stream */
{
  if(device == 'L')				// text LCD
    LCD_data(character);
  else if(device == 'T')			// TFT-LCD
    { if(high_code >= 0x80)
        { TFT_Korean(KS_code_conversion((high_code << 8) + character));
          high_code = 0;
        }
      else if(character >= 0x80)
        high_code = character;
      else
        TFT_English(character);
    }

  return character;
}

int main(void)
{
  unsigned int x;
  float a;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  while(1)
    { LCD_string(0x80,"    printf()    ");	// display title on text LCD
      LCD_string(0xC0," 00 x 00 = 0000 ");
      TFT_clear_screen();			// clear TFT-LCD screen
      TFT_string(9,8,White, Magenta," printf() �Լ��� ��� ");

      for(x = 1; x <= 9; x++)
        { device = 'L';				// printf()�� ����� text LCD ���
	  LCD_command(0xC1);
          printf("%2d x %2d = %4d", x*11, x*11, (x*11)*(x*11));

          device = 'T';				// printf()�� ����� TFT-LCD ���
	  TFT_color(Cyan, Black);
	  TFT_xy(7,18);
          printf("���� ���� : %2d x %2d = %4d", x*11, x*11, (x*11)*(x*11));

          Beep();
          Delay_ms(2000);
        }

      LCD_string(0x80,"    printf()    ");	// display title on text LCD
      LCD_string(0xC0," 0.0x0.0 = 00.00");
      TFT_clear_screen();			// clear TFT-LCD screen
      TFT_string(9,8,White, Magenta," printf() �Լ��� ��� ");

      for(x = 1, a = 1.1; x <= 9; x++, a += 1.1)
        { device = 'L';				// printf()�� ����� text LCD ���
	  LCD_command(0xC1);
          printf("%3.1fx%3.1f = %5.2f", a, a, a*a);

          device = 'T';				// printf()�� ����� TFT-LCD ���
	  TFT_color(Green, Black);
	  TFT_xy(6,18);
          printf("�Ǽ� ���� : %3.1f x %3.1f = %5.2f", a, a, a*a);

          Beep();
          Delay_ms(2000);
        }
    }
}
