/* ========================================================================== */
/*	    Exp15_1.c : Display Same Characters on 7-Segment LED              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

unsigned char Bin2LED(unsigned char number)     /* convert binary to LED pattern */
{
  if(number == 0)       return 0xFC;
  else if(number == 1)  return 0x60;
  else if(number == 2)  return 0xDA;
  else if(number == 3)  return 0xF2;
  else if(number == 4)  return 0x66;
  else if(number == 5)  return 0xB6;
  else if(number == 6)  return 0xBE;
  else if(number == 7)  return 0xE4;
  else if(number == 8)  return 0xFE;
  else if(number == 9)  return 0xF6;
  else if(number == 10) return 0xEE;
  else if(number == 11) return 0x3E;
  else if(number == 12) return 0x9C;
  else if(number == 13) return 0x7A;
  else if(number == 14) return 0x9E;
  else if(number == 15) return 0x8E;
  else if(number == 'H') return 0x6E;
  else if(number == '-') return 0x02;
  else if(number == '.') return 0x01;
  else if(number == 0x27) return 0x40;
  else return 0x00;
}

int main(void)
{ unsigned char i;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  LCD_string(0x80," 7-SEGMENT LED  ");          // display title
  LCD_string(0xC0,"                ");

  while(1)
    { Beep();
      DIG_SELECT = 0xFF;                        // select all digits

      for(i=0; i<= 0x0F; i++)                   // display from 0 to F
        { LCD_DATABUS = Bin2LED(i);
          Delay_ms(1000);
        }
      LCD_DATABUS = Bin2LED('H');               // display "H"
      Delay_ms(1000);
      LCD_DATABUS = Bin2LED('-');               // display "-"
      Delay_ms(1000);
      LCD_DATABUS = Bin2LED('.');               // display "."
      Delay_ms(1000);
      LCD_DATABUS = Bin2LED(0x27);              // display "'"
      Delay_ms(1000);

      DIG_SELECT = 0xFF;
      for(i=1; i<=2; i++)                       // display random pattern
        { LCD_DATABUS = 0x04; Delay_ms(100);
          LCD_DATABUS = 0x80; Delay_ms(100);
          LCD_DATABUS = 0x40; Delay_ms(100);
          LCD_DATABUS = 0x02; Delay_ms(100);
          LCD_DATABUS = 0x08; Delay_ms(100);
          LCD_DATABUS = 0x10; Delay_ms(100);
          LCD_DATABUS = 0x20; Delay_ms(100);
          LCD_DATABUS = 0x02; Delay_ms(100);
        }
    }
}

