/* ========================================================================== */
/*	    Exp08_2.c : Digital Clock on LCD Module and 7-Segment LED         */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK2561.h"

volatile unsigned char second, minute, hour, ampm;

void LCD_2digit(unsigned char number)           /* display 2-digit decimal number */
{
  LCD_data(number/10 + '0');                    // 10^1
  LCD_data(number%10 + '0');                    // 10^0
}

ISR(TIMER1_COMPA_vect)				/* OC1A interrupt function */
{
  DIG_SELECT = 0x00;                            // clear 7-segment LED

  second++;					// increment second
  if(second == 60)                              // if second = 60, second = 0
    { second = 0;
      minute++;                                 // increment minute
      if(minute == 60)                          // if minute = 60, minute = 0
        { minute = 0;
          hour++;                               // increment hour
          if(hour == 13)                        // if hour = 13, hour = 1
            hour = 1;
          if(hour == 12)                        // if hour = 12, adjust AM/PM
            { if(ampm == 'A') ampm = 'P';
              else            ampm = 'A';
            }
        }
     }

  LCD_command(0xC2);				// display hour
  LCD_2digit(hour);
  LCD_command(0xC5);				// display minute
  LCD_2digit(minute);
  LCD_command(0xC8);				// display second
  LCD_2digit(second);
  LCD_command(0xCB);				// display AM/PM
  LCD_data(ampm);
}

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
  else if(number == '-') return 0x02;
  else if(number == '.') return 0x01;
  else return 0x00;
}

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80," Digital Clock  ");          // display title
  LCD_string(0xC0,"  12:00:00 AM   ");

  hour = 12;					// initial time
  minute = 0;
  second = 0;
  ampm = 'A';

  TCCR1A = 0x00;                                // CTC mode(4), don't output OC1A
  TCCR1B = 0x0C;                                // 16MHz/256/(1+62499) = 1Hz
  TCCR1C = 0x00;
  OCR1A = 62499;

  TIMSK1 = 0x02;                                // enable OC1A interrupt
  TIFR1 = 0x2F;                                 // clear all interrupt flags
  sei();                                        // global interrupt enable

  while(1)
    { LCD_DATABUS = Bin2LED(hour/10);           // display hour on 7-segment LED
      DIG_SELECT = 0x80;
      Delay_ms(1);
      LCD_DATABUS = Bin2LED(hour%10);
      DIG_SELECT = 0x40;
      Delay_ms(1);
      LCD_DATABUS = Bin2LED('-');               // display "-" on 7-segment LED
      DIG_SELECT = 0x20;
      Delay_ms(1);
      LCD_DATABUS = Bin2LED(minute/10);         // display minute on 7-segment LED
      DIG_SELECT = 0x10;
      Delay_ms(1);
      LCD_DATABUS = Bin2LED(minute%10);
      DIG_SELECT = 0x08;
      Delay_ms(1);
      LCD_DATABUS = Bin2LED('-');               // display "-" on 7-segment LED
      DIG_SELECT = 0x04;
      Delay_ms(1);
      LCD_DATABUS = Bin2LED(second/10);         // display second on 7-segment LED
      DIG_SELECT = 0x02;
      Delay_ms(1);
      LCD_DATABUS = Bin2LED(second%10);
      DIG_SELECT = 0x01;
      Delay_ms(1);
    }
}

