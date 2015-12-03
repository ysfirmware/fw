/* ========================================================================== */
/*	             Exp02_1.c : Basic C Program (1)	                      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>

void Delay_us(unsigned char time_us)            /* time delay for us */
{ register unsigned char i;

  for(i = 0; i < time_us; i++)      	        // 4 cycle +
    { asm volatile(" PUSH  R0 ");               // 2 cycle +
      asm volatile(" POP   R0 ");       	// 2 cycle +
      asm volatile(" PUSH  R0 ");       	// 2 cycle +
      asm volatile(" POP   R0 ");       	// 2 cycle +
      asm volatile(" PUSH  R0 ");       	// 2 cycle +
      asm volatile(" POP   R0 ");	        // 2 cycle = 16 cycle = 1 us for 16MHz
    }
}

void Delay_ms(unsigned int time_ms)       	/* time delay for ms */
{ register unsigned int i;

  for(i = 0; i < time_ms; i++)
    { Delay_us(250);
      Delay_us(250);
      Delay_us(250);
      Delay_us(250);
    }
}

int main(void)
{
  DDRB = 0xFF;					// initialize PORTB = output

  while(1)
    { PORTB = 0x50;				// LED1, 3 on
      Delay_ms(500);
      PORTB = 0xA0;				// LED2, 4 on
      Delay_ms(500);
    }
}

