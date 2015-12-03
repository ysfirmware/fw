/* ========================================================================== */
/*	       Exp04_4.c : Floating Point Number Display Program	      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

void LCD_1d1(float number)		        /* floating-point number x.x */
{ unsigned char i, j;

  j = (int)(number*10. + 0.5);
  i = j / 10;					// 10^0
  LCD_data(i + '0');
  LCD_data('.');
  i = j % 10;					// 10^-1
  LCD_data(i + '0');
}

int main(void)
{ unsigned char i;
  unsigned int sum;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80," FLOAT  DISPLAY ");		// display title
  LCD_string(0xC0,"  VR1 = 0.0[V]  ");

  ADCSRA = 0x85;				// ADC enable, 500kHz
  ADCSRB = 0x00;
  ADMUX = 0x43;					// select ADC3 with AVCC
  Delay_us(150);

  while(1)
    { sum = 0;					// clear sum
      for(i = 1; i <= 16; i++)			// read ADC by 16 times
        { ADCSRA = 0xD7;			// start conversion and clear ADIF
          while((ADCSRA & 0x10) != 0X10);
          sum += ADCW;				// read A/D result 16 times
          Delay_ms(1);
        }
      sum = sum >> 4;				// divide sum by 16

      LCD_command(0xC8);			// display VR1 in voltage(X.X)
      LCD_1d1(sum*5./1024.);			// Volt = sum*5/1024

      Delay_ms(200);
    }
}

