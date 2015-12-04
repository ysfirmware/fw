/* ============================================================================ */
/*	Exp02_7_performance.c : STM32F407VET6�� ����ӵ� �׽�Ʈ ���α׷�	*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include <arm_math.h>
#include "stm32f746xx.h"
#include "OK-STM746.h"

#define PI_rad	3.14159265358979F

int main(void)
{
  signed int i;
  unsigned int count;
  float x, y, z, answer;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD modul

  LCD_string(0x80," OK-STM746 V1.0 ");          // display title
  LCD_string(0xC0,"   Exp02_7.c    ");
  Beep();

  SysTick->LOAD  = 0x00FFFFFF;			// maximum value
  SysTick->CTRL  = 0x00000005;			// SysTick enable and clock source = 216MHz
  SysTick->VAL   = 0;				// clear SysTick Counter

  while(1)
    { TFT_clear_screen();
      TFT_string(3, 2,White,Magenta,"  STM32F746VET6��  ����ӵ� ����  ");
      TFT_string(3, 5,Cyan,   Black,"         (�Ǽ� ��� �ð�)         ");
      TFT_string(3, 8,Yellow, Black,"    x=000.000, y=00.00, z=0.0     ");
      TFT_string(3,13,White,  Black,"x * y * z * 9.  = 0000 = 00.000 us");
      TFT_string(3,15,White,  Black,"x / y / z / 9.  = 0000 = 00.000 us");
      TFT_string(3,19,White,  Black,"sqrt(x)         = 0000 = 00.000 us");
      TFT_string(3,21,White,  Black,"log(x)          = 0000 = 00.000 us");
      TFT_string(3,23,White,  Black,"exp(x)          = 0000 = 00.000 us");
      TFT_color(Cyan,Black);
      TFT_xy(24,8);
      TFT_English(0x7F);
      Rectangle(10,96, 309,208, White);
      Line(10,144, 309,144, White);

      for(i = 1; i <= 9; i++)
        { x = 111.111 * (float)i;
          y = 11.11 * (float)i;
          z = 1.1 * (float)i;

          TFT_color(Yellow,Black);
          TFT_xy(9,8);
          TFT_unsigned_float(x,3,3);
          TFT_xy(20,8);
          TFT_unsigned_float(y,2,2);
          TFT_xy(29,8);
          TFT_unsigned_float(z,1,1);

	  SysTick->VAL = 0;			// x * y * z * 9.
	  answer = x * y * z * 9.;
          count = SysTick->VAL;
          if(count == 0) count = 0x00FFFFFF;
	  count = 0x00FFFFFF + 1 - count;
          TFT_color(Green,Black);
          TFT_xy(21,13);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,13);
          TFT_unsigned_float((float)count/216.,2,3);

	  SysTick->VAL = 0;			// x / y / z / 9.
	  answer = x / y / z / 9.;
          count = SysTick->VAL;
          if(count == 0) count = 0x00FFFFFF;
	  count = 0x00FFFFFF + 1 - count;
          TFT_xy(21,15);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,15);
          TFT_unsigned_float((float)count/216.,2,3);

	  SysTick->VAL = 0;			// sqrt(x)
	  answer = sqrt(x);
          count = SysTick->VAL;
          count = 0x00FFFFFF + 1 - count;
          TFT_color(Cyan,Black);
          TFT_xy(21,19);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,19);
          TFT_unsigned_float((float)count/216.,2,3);

	  SysTick->VAL = 0;			// log(x)
	  answer = log(x);
          count = SysTick->VAL;
          count = 0x00FFFFFF + 1 - count;
          TFT_xy(21,21);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,21);
          TFT_unsigned_float((float)count/216.,2,3);

	  SysTick->VAL = 0;			// exp(x)
	  answer = exp(x);
          count = SysTick->VAL;
          count = 0x00FFFFFF + 1 - count;
          TFT_xy(21,23);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,23);
          TFT_unsigned_float((float)count/216.,2,3);

          answer = answer * 2.;			// dummy use of answer variable
          Delay_ms(1000);
        }
      Beep();

      TFT_clear_screen();
      TFT_string(3, 2,White,Magenta,"  STM32F746VET6�� ����ӵ�  ����  ");
      TFT_string(3, 5,Cyan,   Black,"       (�ﰢ�Լ� ��� �ð�)       ");
      TFT_string(3, 8,Yellow, Black,"              x = +90             ");
      TFT_string(3,13,White,  Black,"sin(x)          = 0000 = 00.000 us");
      TFT_string(3,15,White,  Black,"arm_sin_f32(x)  = 0000 = 00.000 us");
      TFT_string(3,19,White,  Black,"cos(x)          = 0000 = 00.000 us");
      TFT_string(3,21,White,  Black,"arm_cos_f32(x)  = 0000 = 00.000 us");
      TFT_color(Yellow,Black);
      TFT_xy(24,8);
      TFT_English(0x7F);
      Rectangle(10,96, 309,192, White);
      Line(10,144, 309,144, White);

      for(i = -90; i <= 90; i += 10)
        { x = (float)i * PI_rad / 180.;

	  TFT_color(Yellow,Black);
          TFT_xy(21,8);
          TFT_signed_decimal(i,1,2);

	  SysTick->VAL = 0;			// sin()
	  y = sin(x);
          count = SysTick->VAL;
          count = 0x00FFFFFF + 1 - count;
          TFT_color(Green,Black);
          TFT_xy(21,13);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,13);
          TFT_unsigned_float((float)count/216.,2,3);

	  SysTick->VAL = 0;			// arm_sin_f32()
	  y = arm_sin_f32(x);
          count = SysTick->VAL;
          count = 0x00FFFFFF + 1 - count;
          TFT_xy(21,15);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,15);
          TFT_unsigned_float((float)count/216.,2,3);

	  SysTick->VAL = 0;			// cos()
	  y = cos(x);
          count = SysTick->VAL;
          count = 0x00FFFFFF + 1 - count;
          TFT_color(Cyan,Black);
          TFT_xy(21,19);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,19);
          TFT_unsigned_float((float)count/216.,2,3);

	  SysTick->VAL = 0;			// arm_cos_f32()
	  y = arm_cos_f32(x);
          count = SysTick->VAL;
          count = 0x00FFFFFF + 1 - count;
          TFT_xy(21,21);
          TFT_unsigned_decimal(count,0,4);
          TFT_xy(28,21);
          TFT_unsigned_float((float)count/216.,2,3);

          y = y * 2.;				// dummy use of y variable
          Delay_ms(1000);
        }
      Beep();
    }
}
