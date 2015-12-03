/* ========================================================================== */
/*	          Exp03_3.c : Software Time Delay Application	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

int main(void)
{ unsigned char mode, LED;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80," S/W Time Delay ");		// display title
  LCD_string(0xC0,"Press KEY4-KEY1!");

  mode = 1;					// initial speed mode
  LED = 0x50;					// initial LED data

  while(1)
    { switch(Key_input())			// key input
        { case 0xE0 : mode = 1;
                      LCD_string(0xC0,"    100 [ms]    ");
                      break;
          case 0xD0 : mode = 2;
                      LCD_string(0xC0,"    200 [ms]    ");
                      break;
          case 0xB0 : mode = 3;
                      LCD_string(0xC0,"    300 [ms]    ");
                      break;
          case 0x70 : mode = 4;
                      LCD_string(0xC0,"    400 [ms]    ");
                      break;
          default   : LED ^= 0xF0;		// output LED with complement
                      PORTB = LED;
                      if(mode == 1)      Delay_ms(100);
                      else if(mode == 2) Delay_ms(200);
                      else if(mode == 3) Delay_ms(300);
                      else               Delay_ms(400);
                      break;
        }
    }
}

