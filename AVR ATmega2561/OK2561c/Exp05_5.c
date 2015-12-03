/* ========================================================================== */
/*		Exp05_5.c : INT7 Interrupt Program - Level Trigger	      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK2561.h"

ISR(INT7_vect)					/* INT7 interrupt function */
{ unsigned char i,LED;

  for(i=1, LED=0x10; i<=5; i++)			// shift from LED1 to LED4
    { PORTB = LED;
      Delay_ms(200);
      LED <<= 1;
    }
}

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80," INT7 Interrupt ");		// display title
  LCD_string(0xC0,"(Level Trigger) ");

  EICRB = 0x00;                                 // INT7 = level trigger
  EIMSK = 0x80;					// enable INT7
  EIFR = 0xFF;                                  // clear interrupt flag
  sei();                                        // global interrupt enable

  while(1);                                     // wait interrupt
}

