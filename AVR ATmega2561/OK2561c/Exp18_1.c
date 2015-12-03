/* ========================================================================== */
/*	       Exp18_1.c : Function Display on Graphic LCD Module             */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include "OK2561.h"

/* -------------------------------------------------------------------------- */
/*   Graphic LCD Module : Hyundai LCD,HG12605NY-LY,128x64 dot,LED backlight   */
/* -------------------------------------------------------------------------- */
/* LCD_DATABUS(0x4000) : D0-D7 = DB0-DB7 (7-14, data bus)		      */
/* LCD_CONTROL(0x4100) :   GND = R/-W    (5, read/write)		      */
/*                          D4 = D/-I    (4, data/instruction)		      */
/*                          D5 = E       (6, enable)			      */
/*                          D6 = CS1     (15, chip select 1)		      */
/*                          D7 = CS2     (16, chip select 2)		      */

unsigned char screen[8][128];		        // screen buffer

void GLCD_command(uint8_t signal,uint8_t command)       /* write a command */
{ LCD_CONTROL = signal & 0xC0;			// D/-I = 0
  asm volatile(" PUSH  R0 ");                   // delay for 500 ns
  asm volatile(" POP   R0 ");
  asm volatile(" PUSH  R0 ");
  asm volatile(" POP   R0 ");
  LCD_DATABUS = command;                        // write command
  LCD_CONTROL = (signal & 0xC0)|0x20;		// E = 1
  asm volatile(" PUSH  R0 ");                   // delay for 500 ns
  asm volatile(" POP   R0 ");
  asm volatile(" PUSH  R0 ");
  asm volatile(" POP   R0 ");
  LCD_CONTROL = signal & 0xC0;                  // E = 0
  asm volatile(" NOP      ");
  LCD_CONTROL = 0x00;				// clear all control signals
  Delay_us(10);                                 // wait for GLCD busy
}

void GLCD_data(uint8_t signal,uint8_t character)        /* write a data */
{ LCD_CONTROL = (signal & 0xC0)|0x10;		// D/-I = 1
  asm volatile(" PUSH  R0 ");                   // delay for 500 ns
  asm volatile(" POP   R0 ");
  asm volatile(" PUSH  R0 ");
  asm volatile(" POP   R0 ");
  LCD_DATABUS = character;                      // write data
  LCD_CONTROL = (signal & 0xC0)|0x30;		// E = 1
  asm volatile(" PUSH  R0 ");                   // delay for 500 ns
  asm volatile(" POP   R0 ");
  asm volatile(" PUSH  R0 ");
  asm volatile(" POP   R0 ");
  LCD_CONTROL = (signal & 0xC0)|0x10;           // E = 0
  asm volatile(" NOP      ");
  LCD_CONTROL = 0x00;				// clear all control signals
  Delay_us(10);                                 // wait GLCD busy
}

void Axis_xy(uint8_t x,uint8_t y)		/* set x, y axis position */
{ GLCD_command(0xC0,0xB8+x);			// X address(0 - 7)

  if(y <= 63)					// if y <= 63, CS1 Y address
    GLCD_command(0x40,0x40 + y);
  else						// if y >= 64, CS2 Y address
    GLCD_command(0x80,0x40 + y - 64);
}

void Draw(void)					/* draw screen with buffer data */
{ unsigned char x, y;

  for(x = 0; x <= 7; x++)
    { Axis_xy(x, 0);				// draw CS1 area
      for(y = 0; y <= 63; y++)
        GLCD_data(0x40, screen[x][y]);
      Axis_xy(x, 64);				// draw CS2 area
      for(y = 64; y <= 127; y++)
        GLCD_data(0x80, screen[x][y]);
    }
}

void Clear_screen(void)				/* clear buffer and GLCD screen */
{ unsigned char x, y;

  LCD_CONTROL = 0x00;				// clear all control signals
  GLCD_command(0xC0,0x3F);			// CS1,CS2 display ON
  GLCD_command(0xC0,0xC0);			// CS1,CS2 display position

  for(x = 0; x <= 7; x++)			// clear screen buffer
    for(y = 0; y <= 127; y++)
      screen[x][y] = 0;

  Draw();					// clear screen
}

void Dot(uint8_t xx,uint8_t y)			/* draw a dot on GLCD */
{ unsigned char x, i;

  if((xx > 63) || (y > 127)) return;

  x = xx / 8;					// calculate x address
  i = xx % 8;
  if(i == 0)      i = 0x01;
  else if(i == 1) i = 0x02;
  else if(i == 2) i = 0x04;
  else if(i == 3) i = 0x08;
  else if(i == 4) i = 0x10;
  else if(i == 5) i = 0x20;
  else if(i == 6) i = 0x40;
  else            i = 0x80;
  screen[x][y] |= i;				// OR old data with new data

  Axis_xy(x, y);				// draw dot on GLCD screen
  if(y <= 63)
    GLCD_data(0x40, screen[x][y]);
  else
    GLCD_data(0x80, screen[x][y]);
}

void Line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)	/* draw a straight line */
{ int x, y;

  if(y1 != y2)					// if y1 != y2, y is variable
    { if(y1 < y2)				//              x is function
        for(y = y1; y <= y2; y++)
          { x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
            Dot(x,y);
          }
      else
        for(y = y1; y >= y2; y--)
          { x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
            Dot(x,y);
          }
    }
  else if(x1 != x2)				// if x1 != x2, x is variable
    { if(x1 < x2)				//              y is function
        for(x = x1; x <= x2; x++)
          { y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
            Dot(x,y);
          }
      else
        for(x = x1; x >= x2; x--)
          { y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
            Dot(x,y);
          }
    }
  else						// if x1 == x2 and y1 == y2,
    Dot(x1,y1);					//             it is a dot
}

void Rectangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2) /* draw a rectangle */
{ Line(x1,y1,x1,y2);				// horizontal line
  Line(x2,y1,x2,y2);
  Line(x1,y1,x2,y1);				// vertical line
  Line(x1,y2,x2,y2);
}

void Circle(uint8_t x1,uint8_t y1,uint8_t r)	/* draw a circle */
{ int x, y;
  float s;

  for(y = y1 - r*3/4; y <= y1 + r*3/4; y++)	// draw with y variable
    { s = sqrt(r*r - (y-y1)*(y-y1)) + 0.5;
      x = x1 + (uint8_t)s;
      Dot(x,y);
      x = x1 - (uint8_t)s;
      Dot(x,y);
    }

  for(x = x1 - r*3/4; x <= x1 + r*3/4; x++)	// draw with x variable
    { s = sqrt(r*r - (x-x1)*(x-x1)) + 0.5;
      y = y1 + (uint8_t)s;
      Dot(x,y);
      y = y1 - (uint8_t)s;
      Dot(x,y);
    }
}

void Sine(int8_t peak)				/* draw a sine curve */
{ unsigned char x, y;

  if(peak > 30) return;

  for(y = 0; y <= 120; y++)
    { x = 32 - (char)(sin((float)y * 3. * M_PI / 180.) * peak + 0.5);
      Dot(x,y+3);
    }
}

void Cosine(int8_t peak)			/* draw a cosine curve */
{ unsigned char x, y;

  if(peak > 30) return;

  for(y = 0; y <= 120; y++)
    { x = 32 - (char)(cos((float)y * 3. * M_PI / 180.) * peak + 0.5);
      Dot(x,y+3);
    }
}

int main(void)
{ unsigned char i, x, y;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  Clear_screen();				// initialize GLCD screen

  LCD_string(0x80,"Graphic LCD Test");          // display title on text LCD

  while(1)
    { LCD_string(0xC0,"   (1) Lines    ");	// graphic show 1
      Rectangle(0,0,63,127);
      Delay_ms(300);
      for(i = 0,x = 7,y = 15; i <= 7; i++,x += 8,y += 16)
        { Line(x,0,0,y);
          Line(0,127-y,x,127);
          Delay_ms(300);
        }
      for(i = 0,x = 7,y = 15; i <= 7; i++,x += 8,y += 16)
        { Line(63,y,x,127);
          Line(x,0,63,127-y);
          Delay_ms(300);
        }
      Delay_ms(3000);
      Clear_screen();
    
      LCD_string(0xC0," (2) Rectangles ");	// graphic show 2
      for(i = 0,x = 0,y = 0; i <= 7; i++,x += 4,y += 8)
        { Rectangle(x,y,63 - x,127 - y);
          Delay_ms(300);
        }
      Delay_ms(1000);
      for(i = 0; i <= 63; i++)			// (scroll up)
        { GLCD_command(0xC0,0xC0 + i);
          Delay_ms(100);
        }
      GLCD_command(0xC0,0xC0);
      Delay_ms(3000);
      Clear_screen();

      for(i = 0,x = 0,y = 0; i <= 7; i++,x += 4,y += 4) // (two)
        { Rectangle(x,y,63-x,63-y);
          Rectangle(x,y+64,63-x,127-y);
          Delay_ms(300);
        }
      Delay_ms(3000);
      Clear_screen();

      LCD_string(0xC0,"  (3) Circles   ");	// graphic show 3
      Rectangle(0,0,63,127);
      Delay_ms(300);
      for(i = 7; i <= 63; i += 8)
        { Circle(31,63,i);
          Delay_ms(300);
        }
      Delay_ms(1000);
      for(i = 63; i != 0; i--)			// (scroll down)
        { GLCD_command(0xC0,0xC0 + i);
          Delay_ms(100);
        }
      GLCD_command(0xC0,0xC0);
      Delay_ms(3000);
      Clear_screen();

      for(i = 3; i <= 31; i += 4)		// (two)
        { Circle(31,31,i);
          Circle(31,95,i);
          Delay_ms(300);
        }
      Delay_ms(3000);
      Clear_screen();

      LCD_string(0xC0," (4) Sine Curve ");	// graphic show 4
      Line(32,0,32,127);			// (X axis)
      Line(29,124,32,127);
      Line(35,124,32,127);
      Line(31,33,33,33);
      Line(31,63,33,63);
      Line(31,93,33,93);
      Line(31,123,33,123);
      Line(0,3,63,3);				// (Y axis)
      Line(3,0,0,3);
      Line(3,6,0,3);
      for(i = 5; i <= 30; i += 5)
        { Sine(i);
          Delay_ms(300);
        }
      Delay_ms(3000);
      Clear_screen();

      LCD_string(0xC0,"(5) Cosine Curve");	// graphic show 5
      Line(32,0,32,127);			// (X axis)
      Line(29,124,32,127);
      Line(35,124,32,127);
      Line(31,33,33,33);
      Line(31,63,33,63);
      Line(31,93,33,93);
      Line(31,123,33,123);
      Line(0,3,63,3);				// (Y axis)
      Line(3,0,0,3);
      Line(3,6,0,3);
      for(i = 5; i <= 30; i += 5)
        { Cosine(i);
          Delay_ms(300);
        }
      Delay_ms(3000);
      Clear_screen();
    }
}
