/* ========================================================================== */
/*	        Exp11_3.c : D/A Converter Sine Wave by Interrupt	      */
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

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"  DAC0800 D/A   ");          // display title
  LCD_string(0xC0," 1kHz Sine Wave ");

  TCCR1A = 0x00;                                // CTC mode(4), don't output OC1A
  TCCR1B = 0x0A;                                // 16MHz/8/(1+19) = 100kHz
  TCCR1C = 0x00;
  OCR1A = 19;
  TCNT1 = 0x0000;                               // clear Timer/Counter1

  TIMSK1 = 0x02;                                // enable OC1A interrupt
  TIFR1 = 0x2F;                                 // clear all interrupt flags
  sei();                                        // global interrupt enable

  i = 0;

  while(1);                                     // wait interrupt
}

