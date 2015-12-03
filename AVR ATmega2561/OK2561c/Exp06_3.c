/* ========================================================================== */
/*	            Exp06_3.c : Timer/Counter1 and 3 Interrupt                */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK2561.h"

volatile unsigned int OC1A_count, OC3A_count;   // interrupt counter

void LCD_5d(unsigned int number)                /* display 5-digit decimal number */
{ unsigned int i;
  unsigned char flag;

  flag = 0;
  i = number/10000;                             // 10^4
  if(i == 0) LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  number = number % 10000;                      // 10^3
  i = number/1000;
  if((i == 0) && (flag == 0))
             LCD_data(' ');
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

ISR(TIMER1_COMPA_vect)				/* OC1A interrupt function */
{
  PORTB = PORTB ^ 0x10;                         // toggle LED1
  OC1A_count++;                                 // increment OC1A interrupt count
}

ISR(TIMER3_COMPA_vect)				/* OC3A interrupt function */
{
  PORTB = PORTB ^ 0x40;                         // toggle LED3
  OC3A_count++;                                 // increment OC3A interrupt count
}

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80," Timer1 = 00000 ");          // display title
  LCD_string(0xC0," Timer3 = 00000 ");

  PORTB = 0x50;                                 // LED3 and 1 on

  TCCR1A = 0x00;                                // CTC mode(4), don't output OC1A
  TCCR1B = 0x0C;                                // 16MHz/256/(1+62499) = 1Hz
  TCCR1C = 0x00;
  OCR1A = 62499;
  TCNT1 = 0x0000;                               // clear Timer/Counter1

  TCCR3A = 0x00;                                // CTC mode(4), don't output OC3A
  TCCR3B = 0x0C;                                // 16MHz/256/(1+12499) = 5Hz
  TCCR3C = 0x00;
  OCR3A = 12499;
  TCNT3 = 0x0000;                               // clear Timer/Counter3

  TIMSK1 = 0x02;                                // enable OC1A interrupt
  TIFR1 = 0x2F;                                 // clear all interrupt flags
  TIMSK3 = 0x02;                                // enable OC3A interrupt
  TIFR3 = 0x2F;                                 // clear all interrupt flags
  sei();                                        // global interrupt enable

  OC1A_count = 0;                               // clear OC1A interrupt counter
  OC3A_count = 0;                               // clear OC3A interrupt counter

  while(1)                                      // wait interrupt
    { LCD_command(0x8A);                        // display OC1A interrupt count
      LCD_5d(OC1A_count);
      LCD_command(0xCA);                        // display OC3A interrupt count
      LCD_5d(OC3A_count);
    }
}
