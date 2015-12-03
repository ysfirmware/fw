/* ========================================================================== */
/*	       Exp20_6.c : printf() Function to Graphic LCD Module            */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <stdio.h>
#include "OK2561.h"
#include "GLCD2561.h"

static int GLCD_putchar(char c, FILE *stream);
static FILE device = FDEV_SETUP_STREAM(GLCD_putchar, NULL, _FDEV_SETUP_WRITE);

static int GLCD_putchar(char c, FILE *stream)	/* print a character to GLCD */
{
  if((c < 0x20) || (c > 0x7E))			// check from 0x20 to 0x7E
    return 0;

  GLCD_English(c);
  return 0;
}

int main(void)
{ unsigned char i = 1;
  double x = 0.001;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  GLCD_clear();                                 // initialize GLCD screen
  cursor_flag = 0;                              // cursor off

  LCD_string(0x80," printf to GLCD ");          // display title
  LCD_string(0xC0,"Press KEY4-KEY1!");

  stdout = &device;				// set graphic LCD to stdout
  
  while(1)
    { switch(Key_input())                       // key input
        { case 0xE0 : PORTB = 0x10;             // KEY1 ?
                      GLCD_string(0,0,"  KEY1 is OK !  ");
                      GLCD_xy(2,3);
                      printf("i = %03d",i);
                      GLCD_xy(3,3);
                      printf("x = %5.3f",x);
                      i++;
                      x += 0.001;
                      break;
          case 0xD0 : PORTB = 0x20;             // KEY2 ?
                      GLCD_string(0,0,"  KEY2 is OK !  ");
                      GLCD_xy(2,3);
                      printf("i = %03d",i);
                      GLCD_xy(3,3);
                      printf("x = %5.3f",x);
		      i++;
                      x += 0.001;
                      break;
          case 0xB0 : PORTB = 0x40;             // KEY3 ?
                      GLCD_string(0,0,"  KEY3 is OK !  ");
                      GLCD_xy(2,3);
                      printf("i = %03d",i);
                      GLCD_xy(3,3);
                      printf("x = %5.3f",x);
		      i++;
                      x += 0.001;
                      break;
          case 0x70 : PORTB = 0x80;             // KEY4 ?
                      GLCD_string(0,0,"  KEY4 is OK !  ");
                      GLCD_xy(2,3);
                      printf("i = %03d",i);
                      GLCD_xy(3,3);
                      printf("x = %5.3f",x);
		      i++;
                      x += 0.001;
                      break;
          default:    break;
        }
    }
}
