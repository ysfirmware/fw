/* ========================================================================== */
/*	        Exp11_4.c : Variable D/A Frequency by Key Input      	      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "OK2561.h"

unsigned char i;
prog_uchar sin_table[100] = {			// sine wave data table
    0x80,0x88,0x90,0x98,0xA0,0xA7,0xAF,0xB6,0xBD,0xC4,
    0xCB,0xD1,0xD7,0xDD,0xE2,0xE7,0xEB,0xEF,0xF3,0xF6,
    0xF9,0xFB,0xFD,0xFE,0xFF,0xFF,0xFF,0xFE,0xFD,0xFB,
    0xF9,0xF6,0xF3,0xEF,0xEB,0xE7,0xE2,0xDD,0xD7,0xD1,
    0xCB,0xC4,0xBD,0xB6,0xAF,0xA7,0xA0,0x98,0x90,0x88,
    0x80,0x78,0x70,0x68,0x60,0x59,0x51,0x4A,0x43,0x3C,
    0x35,0x2F,0x29,0x23,0x1E,0x19,0x15,0x11,0x0D,0x0A,
    0x07,0x05,0x03,0x02,0x01,0x01,0x01,0x02,0x03,0x05,
    0x07,0x0A,0x0D,0x11,0x15,0x19,0x1E,0x23,0x29,0x2F,
    0x35,0x3C,0x43,0x4A,0x51,0x59,0x60,0x68,0x70,0x78 };

ISR(TIMER1_COMPA_vect)				/* OC1A interrupt function */
{
  PORTB = pgm_read_byte(&sin_table[i]);         // output data to D/A
  i++;
  if(i == 100)
    i = 0;
}

void LCD_3dec(unsigned char number)		/* display 3-digit decimal number */
{ unsigned char i;

  if(number >= 100) LCD_data(number/100 + '0'); // 10^2
  else              LCD_data(' ');
  i = number%100;
  LCD_data(i/10 + '0');                         // 10^1
  LCD_data(i%10 + '0');                         // 10^0
}

void Set_timer1(unsigned char number)           /* set Timer1 OCR1A */
{
  OCR1A = number - 1;				// set OCR1A
  LCD_command(0xCA);                            // display div_ratio
  LCD_3dec(number);
}

int main(void)
{ unsigned char div_ratio;

  MCU_initialize();                             // initialize MCU
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"   D/A Output   ");          // display title
  LCD_string(0xC0,"16MHz/800/100 Hz");

  TCCR1A = 0x00;                                // CTC mode(4), don't output OC1A
  TCCR1B = 0x0A;                                // 16MHz/8/(1+99) = 20kHz
  TCCR1C = 0x00;
  OCR1A = 99;
  TCNT1 = 0x0000;                               // clear Timer/Counter1

  TIMSK1 = 0x02;                                // enable OC1A interrupt
  TIFR1 = 0x2F;                                 // clear all interrupt flags
  sei();                                        // global interrupt enable

  i = 0;
  div_ratio = 100;                              // initial division ratio

  while(1)
    { switch(Key_input())                       // key input
        { case 0xE0 : if(div_ratio != 20)       // if KEY1, decrement by 1
                        div_ratio--;
                      Set_timer1(div_ratio);
                      break;
          case 0xD0 : if(div_ratio != 200)      // if KEY2, increment by 1
                        div_ratio++;
                      Set_timer1(div_ratio);
                      break;
          case 0xB0 : div_ratio -= 10;          // if KEY3, decrement by 10
                      if(div_ratio < 20)
                        div_ratio = 20;
                      Set_timer1(div_ratio);
                      break;
          case 0x70 : div_ratio += 10;          // if KEY4, increment by 10
                      if(div_ratio > 200)
                        div_ratio = 200;
                      Set_timer1(div_ratio);
                      break;
          default:    break;
        }
    }
}

