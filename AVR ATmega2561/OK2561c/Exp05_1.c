/* ========================================================================== */
/*	             Exp05_1.c : Key Input - No Debouncing                    */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

void LCD_8bin(unsigned char number)		/* display 8-bit binary number */
{
  LCD_data(((number >> 7) & 0x01) + '0');       // 2^7
  LCD_data(((number >> 6) & 0x01) + '0');       // 2^6
  LCD_data(((number >> 5) & 0x01) + '0');       // 2^5
  LCD_data(((number >> 4) & 0x01) + '0');       // 2^4
  LCD_data(((number >> 3) & 0x01) + '0');       // 2^3
  LCD_data(((number >> 2) & 0x01) + '0');       // 2^2
  LCD_data(((number >> 1) & 0x01) + '0');       // 2^1
  LCD_data((number & 0x01) + '0');              // 2^0
}

int main(void)
{ unsigned char key;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"   KEY INPUT    ");          // display title
  LCD_string(0xC0,"PORTF=0b11110000");

  while(1)
    { LCD_command(0xC8);			// cursor position
      key = PINF & 0xF0;                        // read key input
      LCD_8bin(key);				// display binary data
      PORTB = key ^ 0xF0;			// display LED
    }
}
