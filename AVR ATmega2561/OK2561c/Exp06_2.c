/* ========================================================================== */
/*	           Exp06_2.c : Timer/Counter0 Interrupt(250Hz)	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK2561.h"

volatile unsigned char interrupt_count;         // interrupt counter

ISR(TIMER0_COMPA_vect)				/* OC0A interrupt function */
{
  PORTB = PORTB ^ 0x10;                         // toggle LED1
  interrupt_count--;
  if(interrupt_count == 0)                      // 250th interrupt ?
    { interrupt_count = 250;                    // if yes, toggle LED2
      PORTB = PORTB ^ 0x20;
    }
}

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"     Timer0     ");          // display title
  LCD_string(0xC0,"250 Hz Interrupt");

  PORTB = 0x30;                                 // LED2 and 1 on

  TCCR0A = 0x02;                                // CTC mode(2), don't output OC0A
  TCCR0B = 0x04;                                // 16MHz/256/(1+249) = 250Hz
  OCR0A = 249;
  TCNT0 = 0x00;                                 // clear Timer/Counter0

  TIMSK0 = 0x02;                                // enable OC0A interrupt
  TIFR0 = 0x07;                                 // clear all interrupt flags
  sei();                                        // global interrupt enable

  interrupt_count = 250;                        // interrupt counter

  while(1);                                     // wait interrupt
}

