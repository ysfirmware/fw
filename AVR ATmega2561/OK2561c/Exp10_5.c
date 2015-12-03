/* ========================================================================== */
/*	       Exp10_5.c : A/D Converter ADC1 Input for LM35DZ	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <math.h>
#include "OK2561.h"

void LCD_4hex(unsigned int number)              /* display 4-digit hex number */
{ unsigned int i;

  i = number >> 12;                             // 16^3
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = (number >> 8) & 0x000F;                   // 16^2
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = (number >> 4) & 0x000F;                   // 16^1
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = number & 0x000F;                          // 16^0
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');
}

void LCD_2d1(float number)		        /* floating-point number xx.x */
{ unsigned int i, j;

  j = (int)(number*10. + 0.5);
  i = j / 100;					// 10^1
  if(i == 0) LCD_data(' ');
  else       LCD_data(i + '0');

  j = j % 100;					// 10^0
  i = j / 10;
  LCD_data(i + '0');
  LCD_data('.');

  i = j % 10;					// 10^-1
  LCD_data(i + '0');
}

int main(void)
{ unsigned char i;
  int sum;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"  ADC1(LM35DZ)  ");          // display title
  LCD_string(0xC0," 0000H = 00.0");
  LCD_data(0xDF);
  LCD_data('C');
  LCD_data(' ');

  ADCSRA = 0x87;                                // ADC enable, 125kHz
  ADCSRB = 0x00;
  ADMUX = 0xF0;                                 // diffrential ADC1 vs ADC0(GND), gain 1
  Delay_us(150);				// (use left adjust and Vref=2.56V)

  while(1)
    { sum = 0;
      for(i=0; i<16; i++)                       // read ADC by 16 times
        { ADCSRA = 0xD7;                        // start conversion and clear ADIF
          while((ADCSRA & 0x10) != 0x10);
          sum += (signed int)ADCW >> 6;		// convert 10 bit to 16 bit 2's complement
          Delay_ms(1);				// and add A/D result 16 times
        }

      sum = sum/16;                             // divide sum by 16
      sum = fabs(sum);				// calculate absolute value

      LCD_command(0xC1);                        // display in HEX
      LCD_4hex(sum);
      LCD_command(0xC9);                        // display in temperature(XX.X)
      LCD_2d1(sum*256./512.);                   // Temp = sum*256/512

      Delay_ms(1000);                           // delay 1 sec
    }
}

