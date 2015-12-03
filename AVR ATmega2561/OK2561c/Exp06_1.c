/* ========================================================================== */
/*	             Exp06_1.c : Timer/Counter1 Interrupt(1Hz)	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK2561.h"

ISR(TIMER1_COMPA_vect)				/* OC1A interrupt function */
{
  PORTB = PORTB ^ 0x10;                         // toggle LED1
}

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"     Timer1     ");          // display title
  LCD_string(0xC0," 1 Hz Interrupt ");

  PORTB = 0x10;                                 // LED1 on

  TCCR1A = 0x00;                                // CTC mode(4), don't output OC1A
  TCCR1B = 0x0C;                                // 16MHz/256/(1+62499) = 1Hz
  TCCR1C = 0x00;
  OCR1A = 62499;
  TCNT1 = 0x0000;                               // clear Timer/Counter1

  TIMSK1 = 0x02;                                // enable OC1A interrupt
  TIFR1 = 0x2F;                                 // clear all interrupt flags
  sei();                                        // global interrupt enable

  while(1);                                     // wait interrupt
}
