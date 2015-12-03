/* ========================================================================== */
/*	             Exp08_3.c : Digital Clock with Time Adjust	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK2561.h"

volatile unsigned char cursor, second, minute, hour, ampm;

void LCD_2digit(unsigned char number)           /* display 2-digit decimal number */
{
  LCD_data(number/10 + '0');                    // 10^1
  LCD_data(number%10 + '0');                    // 10^0
}

ISR(TIMER1_COMPA_vect)				/* OC1A interrupt function */
{
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
  LCD_command(cursor);				// go cursor position
}

void Cursor_left(void)			        /* go cursor left */
{
  if(cursor != 0xC3) cursor -= 3;
  else               cursor = 0xCF;
  LCD_command(cursor);
}

void Cursor_right(void)             		/* go cursor right */
{
  if(cursor != 0xCF) cursor += 3;
  else               cursor = 0xC3;
  LCD_command(cursor);
}

void Increment(void)	               		/* increment time */
{ switch(cursor)
    { case 0xC3 : if(hour != 12) hour++;	// in case of hour
                  else           hour = 1;
                  LCD_command(0xC2); LCD_2digit(hour);
                  LCD_command(cursor);
                  break;
      case 0xC6 : if(minute != 59) minute++;	// in case of minute
                  else             minute = 0;
                  LCD_command(0xC5); LCD_2digit(minute);
                  LCD_command(cursor);
                  break;
      case 0xC9 : if(second != 59) second++;	// in case of second
                  else             second = 0;
                  LCD_command(0xC8); LCD_2digit(second);
                  LCD_command(cursor);
                  break;
      case 0xCC : if(ampm == 'A') ampm = 'P';	// in case of AM/PM
                  else            ampm = 'A';
                  LCD_command(0xCB); LCD_data(ampm);
                  LCD_command(cursor);
                  break;
      default   : break;
    }
}

void Decrement(void)	               		/* decrement time */
{ switch(cursor)
    { case 0xC3 : if(hour != 1) hour--;		// in case of hour
                  else          hour = 12;
                  LCD_command(0xC2); LCD_2digit(hour);
                  LCD_command(cursor);
                  break;
      case 0xC6 : if(minute != 0) minute--;	// in case of minute
                  else            minute = 59;
                  LCD_command(0xC5); LCD_2digit(minute);
                  LCD_command(cursor);
                  break;
      case 0xC9 : if(second != 0) second--;	// in case of second
                  else            second = 59;
                  LCD_command(0xC8); LCD_2digit(second);
                  LCD_command(cursor);
                  break;
      case 0xCC : if(ampm == 'A') ampm = 'P';	// in case of AM/PM
                  else            ampm = 'A';
                  LCD_command(0xCB); LCD_data(ampm);
                  LCD_command(cursor);
                  break;
      default   : break;
    }
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

  cursor = 0xCF;				// initial cursor position
  LCD_command(0x0F);				// cursor ON

  TCCR1A = 0x00;                                // CTC mode(4), don't output OC1A
  TCCR1B = 0x0C;                                // 16MHz/256/(1+62499) = 1Hz
  TCCR1C = 0x00;
  OCR1A = 62499;

  TIMSK1 = 0x02;                                // enable OC1A interrupt
  TIFR1 = 0x2F;                                 // clear all interrupt flags
  sei();                                        // global interrupt enable

  while(1)
    { switch(Key_input())			// key input
        { case 0x70 : Cursor_left();		// if KEY4, go cursor left
                      break;
          case 0xB0 : Cursor_right();		// if KEY3, go cursor right
                      break;
          case 0xD0 : Increment();		// if KEY2, increment time
                      break;
          case 0xE0 : Decrement();		// if KEY1, decrement time
                      break;
          default   : break;
        }
    }
}
