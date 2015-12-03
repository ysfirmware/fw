/* ========================================================================== */
/*	             Exp03_1.c : Bit Access	                              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit

  while(1)
    { PORTB = (1<<6) | (1<<4);                  // LED1, 3 on
      Delay_ms(500);
      PORTB = _BV(7) | _BV(5);                  // LED2, 4 on
      Delay_ms(500);
    }
}

