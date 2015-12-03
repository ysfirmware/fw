/* ========================================================================== */
/*	             Exp04_3.c : Integer Number Display Program	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

void LCD_2d(unsigned char number)               /* display 2-digit decimal number */
{ unsigned char i;

  i = number/10;                                // 10^1
  if(i == 0) LCD_data(' ');
  else       LCD_data(i + '0');

  i = number % 10;                              // 10^0
  LCD_data(i + '0');
}

void LCD_3d(unsigned int number)                /* display 3-digit decimal number */
{ unsigned int i;
  unsigned char flag;

  flag = 0;
  i = number/100;                               // 10^2
  if(i == 0) LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  number = number % 100;                        // 10^1
  i = number/10;
  if((i == 0) && (flag == 0))
             LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  i = number % 10;                              // 10^0
  LCD_data(i + '0');
}

void LCD_4d(unsigned int number)                /* display 4-digit decimal number */
{ unsigned int i;
  unsigned char flag;

  flag = 0;
  i = number/1000;                              // 10^3
  if(i == 0) LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  number = number % 1000;                       // 10^2
  i = number/100;
  if((i == 0) && (flag == 0))
             LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  number = number % 100;                        // 10^1
  i = number/10;
  if((i == 0) && (flag == 0))
             LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  i = number % 10;                              // 10^0
  LCD_data(i + '0');
}

void LCD_2hex(unsigned char number)             /* display 2-digit hex number */
{ unsigned char i;

  i = (number >> 4) & 0x0F;                     // 16^1
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = number & 0x0F;                            // 16^0
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');
}

void LCD_8bin(unsigned char number)             /* display 8-bit binary number */
{
  LCD_data(((number >> 7) & 0x01) + '0');       // 2^7
  LCD_data(((number >> 6) & 0x01) + '0');       // 2^6
  LCD_data(((number >> 5) & 0x01) + '0');       // 2^5
  LCD_data(((number >> 4) & 0x01) + '0');       // 2^4
  LCD_data(((number >> 3) & 0x01) + '0');       // 2^3
  LCD_data(((number >> 2) & 0x01) + '0');       // 2^2
  LCD_data(((number >> 1) & 0x01) + '0');       // 2^1
  LCD_data((number & 0x01) + '0');              // 2^0
}

int main(void)
{ unsigned char i, j;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  while(1)
    { LCD_string(0x80," INTEGER = 000  ");      // display 8-bit integer
      LCD_string(0xC0,"= 00H = 00000000");
      Beep();

      for(i=1; i>0; i++)                        // integer number from 1 to 255
        { LCD_command(0x8B);                    // decimal number
          LCD_3d((unsigned int)i);
          LCD_command(0xC2);                    // hexadecimal number
          LCD_2hex(i);
          LCD_command(0xC8);                    // binary number
          LCD_8bin(i);
          Delay_ms(1000);
        }

      LCD_string(0x80," MULTI TABLE(1) ");      // display multiplication table(1)
      LCD_string(0xC0,"   0 x 0 = 00   ");
      Beep();

      for(i=2; i<=9; i++)
        for(j=1; j<=9; j++)
          { LCD_command(0xC3); LCD_data(i+'0'); // display multiplicand
            LCD_command(0xC7); LCD_data(j+'0'); // display multiplier
            LCD_command(0xCB); LCD_2d(i*j);     // display multiplication
            Delay_ms(1000);
          }

      LCD_string(0x80," MULTI TABLE(2) ");      // display multiplication table(2)
      LCD_string(0xC0," 00 x 00 = 0000 ");
      Beep();

      for(i=10; i<=90; i += 10)
        for(j=10; j<=90; j += 10)
          { LCD_command(0xC1); LCD_2d(i);       // display multiplicand
            LCD_command(0xC6); LCD_2d(j);       // display multiplier
            LCD_command(0xCB); LCD_4d(i*j);     // display multiplication
            Delay_ms(1000);
          }
    }
}

