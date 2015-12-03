/* ========================================================================== */
/*	    Exp11_2a.c : D/A Converter Sine Wave Output by Function	      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <math.h>
#include "OK2561.h"

int main(void)
{ unsigned char i;
  float x;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"  DAC0800 D/A   ");          // display title
  LCD_string(0xC0,"   Sine Wave    ");

  while(1)
    { for(i=0; i<100; i++)                      // output D/A
        { x = (float)360.*i/100.;
          PORTB = 0x80 + 0x7F*sin(M_PI*x/180.);
        }
    }
}

