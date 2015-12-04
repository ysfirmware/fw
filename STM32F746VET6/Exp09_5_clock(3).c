/* ============================================================================ */
/*	Exp09_5_clock(3).c : SysTick Ÿ�̸Ӹ� �̿��� ������ �ð�(3)		*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void SysTick_Handler(void);			/* SysTick interrupt function */
void Display_time(void);			/* display current time */
void Time_increment(void);			/* increment time */
void Time_decrement(void);			/* decrement time */
void Cursor_left(void);				/* go cursor left */
void Cursor_right(void);			/* go cursor right */
unsigned char Icon_input(void);			/* input touch screen icon */

volatile unsigned char ampm = 'A', hour = 12, minute = 0, second = 0, count100Hz = 0;

/* ----- ���ͷ�Ʈ ó�� ���α׷� ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  count100Hz++;
    if(count100Hz == 100)
    { count100Hz = 0;
      second++;					// increment second
      if(second == 60)                          // if second = 60, second = 0
      { second = 0;
        minute++;                               // increment minute
        if(minute == 60)                        // if minute = 60, minute = 0
        { minute = 0;
          hour++;                               // increment hour
          if(hour == 13)                        // if hour = 13, hour = 1
            hour = 1;
          if(hour == 12)                        // if hour = 12, adjust AM/PM
          { if(ampm == 'A') ampm = 'P';
            else            ampm = 'A';
          }
        }
      }
    }

  if((minute == 0x00) && (second == 0x00) && (count100Hz == 0x00))	// beep on every hour
    Beep();
}

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  unsigned char key;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Initialize_touch_screen();			// initialize touch screen

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title
  LCD_string(0xC0,"   Exp09_5.c    ");
  Beep();

  Rectangle(0,0, 319,239, Green);		// display clock title
  Rectangle(1,1, 318,238, Green);
  TFT_string( 1, 4,White,Magenta," SysTick ���ͷ�Ʈ�� ����  ������ �ð� ");
  TFT_string(13,14,White,  Black,"���� 12:00:00");
  TFT_string( 2,25,White,Blue,"KEY1");		// display icon
  TFT_string(12,25,White,Blue,"KEY2");
  TFT_string(24,25,White,Blue,"KEY3");
  TFT_string(34,25,White,Blue,"KEY4");
  TFT_string( 2,27,Red, Black,"����");
  TFT_string(12,27,Red, Black,"����");
  TFT_string(24,27,Red, Black,"����");
  TFT_string(34,27,Red, Black,"����");
  Rectangle( 10,196,  53,234, Cyan);
  Rectangle( 90,196, 133,234, Cyan);
  Rectangle(186,196, 229,234, Cyan);
  Rectangle(266,196, 309,234, Cyan);

  TFT_cursor(Red);				// display cursor
  Xcursor = 38;
  Ycursor = 14;

  SysTick->LOAD  = 269999;			// 27MHz/(269999+1) = 100Hz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 216MHz/8 = 27MHz, SysTick enable and interrupt request

  while(1)
    { key = Key_input();			// key input
      if(key == no_key)				// if no key input, read touch screen icon
        key = Icon_input();

      switch(key)
        { case KEY1 : Cursor_left();		// if KEY1, go cursor left
                      break;
          case KEY2 : Cursor_right();		// if KEY2, go cursor right
                      break;
          case KEY3 : Time_increment();		// if KEY3, increment time
                      break;
          case KEY4 : Time_decrement();		// if KEY4, decrement time
                      break;
	  default   : Display_time();		// display time
	              break;
        }
    }
}

/* ----- ����� ���� �Լ� ----------------------------------------------------- */

void Display_time(void)				/* display current time */
{
  if(ampm == 'A') TFT_string(13,14,White,Black,"����"); // display AM/PM
  else            TFT_string(13,14,White,Black,"����");

  TFT_color(Green,Black);
  TFT_xy(18,14);				// display hour
  TFT_unsigned_decimal(hour,1,2);
  TFT_xy(21,14);				// display minute
  TFT_unsigned_decimal(minute,1,2);
  TFT_xy(24,14);				// display second
  TFT_unsigned_decimal(second,1,2);

  TFT_xy(38,14);				// display home
  TFT_English(' ');
}

void Cursor_left(void)				/* go cursor left */
{
  if     ((Xcursor == 38) && (Ycursor == 14)) { Xcursor = 25; Ycursor = 14; }
  else if((Xcursor == 25) && (Ycursor == 14)) { Xcursor = 22; Ycursor = 14; }
  else if((Xcursor == 22) && (Ycursor == 14)) { Xcursor = 19; Ycursor = 14; }
  else if((Xcursor == 19) && (Ycursor == 14)) { Xcursor = 15; Ycursor = 14; }
  else if((Xcursor == 15) && (Ycursor == 14)) { Xcursor = 38; Ycursor = 14; }
}

void Cursor_right(void)				/* go cursor right */
{
  if     ((Xcursor == 38) && (Ycursor == 14)) { Xcursor = 15; Ycursor = 14; }
  else if((Xcursor == 15) && (Ycursor == 14)) { Xcursor = 19; Ycursor = 14; }
  else if((Xcursor == 19) && (Ycursor == 14)) { Xcursor = 22; Ycursor = 14; }
  else if((Xcursor == 22) && (Ycursor == 14)) { Xcursor = 25; Ycursor = 14; }
  else if((Xcursor == 25) && (Ycursor == 14)) { Xcursor = 38; Ycursor = 14; }
}

void Time_increment(void)			/* increment time */
{
  if((Xcursor == 15) && (Ycursor == 14))	// in case of AM/PM
    { if(ampm == 'A') ampm = 'P';
      else            ampm = 'A';
    }
  else if((Xcursor == 19) && (Ycursor == 14))	// in case of hour
    { if(hour == 12) hour = 1;
      else           hour++;
    }
  else if((Xcursor == 22) && (Ycursor == 14))	// in case of minute
    { if(minute == 59) minute = 0;
      else             minute++;
    }
  else if((Xcursor == 25) && (Ycursor == 14))	// in case of second
    { if(second == 59) second = 0;
      else             second++;
    }
}

void Time_decrement(void)			/* decrement time */
{
  if((Xcursor == 15) && (Ycursor == 14))	// in case of AM/PM
    { if(ampm == 'A') ampm = 'P';
      else            ampm = 'A';
    }
  else if((Xcursor == 19) && (Ycursor == 14))	// in case of hour
    { if(hour == 1) hour = 12;
      else          hour--;
    }
  else if((Xcursor == 22) && (Ycursor == 14))	// in case of minute
    { if(minute == 0) minute = 59;
      else            minute--;
    }
  else if((Xcursor == 25) && (Ycursor == 14))	// in case of second
    { if(second == 0) second = 59;
      else            second--;
    }
}

unsigned char icon_flag = 0;

unsigned char Icon_input(void)			/* input touch screen icon */
{
  unsigned char icon;

  Touch_screen_input();				// input touch screen

  if((icon_flag == 0) && (x_touch >= 10) && (x_touch <= 53) && (y_touch >= 196) && (y_touch <= 234))
    { icon = KEY1;
      icon_flag = 1;
      Rectangle( 10,196,  53,234, Magenta);
      Beep();
    }
  else if((icon_flag == 0) && (x_touch >= 90) && (x_touch <= 133) && (y_touch >= 196) && (y_touch <= 234))
    { icon = KEY2;
      icon_flag = 1;
      Rectangle( 90,196, 133,234, Magenta);
      Beep();
    }
  else if((icon_flag == 0) && (x_touch >= 186) && (x_touch <= 229) && (y_touch >= 196) && (y_touch <= 234))
    { icon = KEY3;
      icon_flag = 1;
      Rectangle(186,196, 229,234, Magenta);
      Beep();
    }
  else if((icon_flag == 0) && (x_touch >= 266) && (x_touch <= 309) && (y_touch >= 196) && (y_touch <= 234))
    { icon = KEY4;
      icon_flag = 1;
      Rectangle(266,196, 309,234, Magenta);
      Beep();
    }
  else if((icon_flag == 1) && (x_touch == 0) && (y_touch == 0))
    { icon = no_key;
      icon_flag = 0;
      Rectangle( 10,196,  53,234, Cyan);
      Rectangle( 90,196, 133,234, Cyan);
      Rectangle(186,196, 229,234, Cyan);
      Rectangle(266,196, 309,234, Cyan);
      Delay_ms(50);
    }
  else
    icon = no_key;

  return icon;
}
