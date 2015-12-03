/* ========================================================================== */
/*	           Exp05_4.c : Key Input Count - Debouncing	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

void LCD_2d(unsigned char number)		/* display 2-digit decimal number */
{ unsigned char i;

  i = number/10;				/* 10^1 */
  if(i == 0) LCD_data(' ');
  else       LCD_data(i + '0');

  i = number % 10;				/* 10^0 */
  LCD_data(i + '0');
}

int main(void)
{ unsigned char key;
  unsigned char key1=0,key2=0,key3=0,key4=0;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80,"KEY1=00  KEY2=00");          // display title
  LCD_string(0xC0,"KEY3=00  KEY4=00");

  while(1)
    { key = Key_input();			// key input with debouncing
      switch(key)
        { case 0xE0 : PORTB = key ^ 0xF0;	// display LED
                      key1++;			// display KEY1 count
                      if(key1 > 99) key1 = 0;
                      LCD_command(0x85);
                      LCD_2d(key1);
                      break;
          case 0xD0 : PORTB = key ^ 0xF0;	// display LED
                      key2++;			// display KEY2 count
                      if(key2 > 99) key2 = 0;
                      LCD_command(0x8E);
                      LCD_2d(key2);
                      break;
          case 0xB0 : PORTB = key ^ 0xF0;	// display LED
                      key3++;			// display KEY3 count
                      if(key3 > 99) key3 = 0;
                      LCD_command(0xC5);
                      LCD_2d(key3);
                      break;
          case 0x70 : PORTB = key ^ 0xF0;	// display LED
                      key4++;			// display KEY4 count
                      if(key4 > 99) key4 = 0;
                      LCD_command(0xCE);
                      LCD_2d(key4);
                      break;
        }
    }
}
