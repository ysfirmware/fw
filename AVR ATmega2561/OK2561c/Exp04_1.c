/* ========================================================================== */
/*	             Exp04_1.c : Basic LCD Display Program	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

int main(void)
{ unsigned char i;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  while(1)
    { Beep();
      LCD_string(0x80,"  OK-2561 V1.0  ");      // display logo
      LCD_string(0xC0," ATmega2561-16AU");      // display message 1
      Delay_ms(2000);
      LCD_string(0xC0,"   2006/10/01   ");      // display message 2
      Delay_ms(2000);

      Beep();
      LCD_string(0x80,"0123456789ABCDEF0123456789ABCDEF"); // display long line
      LCD_string(0xC0,"Go shift left and right 16 times");
      Delay_ms(1000);

      for(i=0; i<16; i++)                       // shift left
        { LCD_command(0x18);
          Delay_ms(300);
        }
      Delay_ms(1000);

      for(i=0; i<16; i++)                       // shift right
        { LCD_command(0x1C);
          Delay_ms(300);
        }
      Delay_ms(1000);
    }
}

