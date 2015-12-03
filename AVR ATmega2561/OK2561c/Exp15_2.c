/* ========================================================================== */
/*	    Exp15_2.c : Display Different Characters on 7-Segment LED         */
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
{ unsigned char i, j, k, digit;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  LCD_string(0x80," 7-SEGMENT LED  ");          // display title
  LCD_string(0xC0,"                ");

  while(1)
    { Beep();

      for(i=0; i<250; i++)                      // display for 2 sec
        for(j=0,digit=0x80; j<=7; j++)          // display from 0 to 7
          { LCD_DATABUS = Bin2LED(j);
            DIG_SELECT = digit;
            digit = digit>>1;
            Delay_ms(1);
          }

      for(i=0; i<250; i++)                      // display for 2 sec
        for(j=8,digit=0x80; j<=15; j++)         // display from 8 to F
          { LCD_DATABUS = Bin2LED(j);
            DIG_SELECT = digit;
            Delay_ms(1);
            digit = digit>>1;
          }

      for(k=0; k<=8; k++)                       // display shift left
        for(i=0; i<125; i++)                    // display for 1 sec
          for(j=0,digit=0x80; j<=7; j++)
            { LCD_DATABUS = Bin2LED(k+j) + 0x01;
              DIG_SELECT = digit;
              Delay_ms(1);
              digit = digit>>1;
            }

      DIG_SELECT = 0x00;                        // clear 7-segment LED
    }
}
