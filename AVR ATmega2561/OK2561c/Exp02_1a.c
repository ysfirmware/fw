/* ========================================================================== */
/*	             Exp02_1a.c : Basic C Program (1)	                      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit

  while(1)
    { PORTB = 0x50;				// LED1, 3 on
      Delay_ms(500);
      PORTB = 0xA0;				// LED2, 4 on
      Delay_ms(500);
    }
}

