/* ============================================================================ */
/*	Exp09_2_stopwatch.c : SysTick Ÿ�̸Ӹ� �̿��� ���� �����ġ		*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void SysTick_Handler(void);			/* SysTick interrupt function */
void Clear_time(void);				/* clear time */
void Display_key(U08 key);			/* display key input */

volatile unsigned int second, minute, hour;
volatile unsigned char run_flag;

/* ----- ���ͷ�Ʈ ó�� ���α׷� ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  second++;                                     // increment 1/100 second
  if(second == 6000)                            // second = 60 ?
    { second = 0;                               // if yes, second = 0 and minute++
      minute++;
      if(minute == 60)                          // minute = 60 ?
        { minute = 0;                           // if yes, minute = 0 and hour++
          hour++;
          if(hour == 100)                       // hour = 100 ?
            hour = 0;                           // if yes, hour = 0
        }
    }

  TFT_color(White,Black);			// display time
  TFT_xy(12,10);
  TFT_unsigned_decimal(hour,1,2);
  TFT_xy(17,10);
  TFT_unsigned_decimal(minute,1,2);
  TFT_xy(22,10);
  TFT_unsigned_decimal(second/100,1,2);
  TFT_xy(25,10);
  TFT_unsigned_decimal(second%100,1,2);
}

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  unsigned char key;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title
  LCD_string(0xC0,"   Exp09_2.c    ");
  Beep();

  TFT_string( 4, 4, White,Magenta, " ����ũ����Ʈ�ѷ� ���� �����ġ ");
  TFT_string(10,18, White,Black, "KEY1 - ��ŸƮ(start)");
  TFT_string(10,20, White,Black, "KEY2 -");
  TFT_string(10,22, White,Black, "KEY3 - ����(reset)");
  TFT_string(10,24, White,Black, "KEY4 - ����(stop)");
  Clear_time();					/* clear time */

  SysTick->LOAD  = 269999;			// 27MHz/(269999+1) = 100Hz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000001;			// 217MHz/8 = 27MHz, SysTick enable and interrupt disable

  run_flag = 0;					// initial status

  while(1)
    { key = Key_input();			// key input
      switch(key)
        { case KEY1 : if(run_flag == 1) break;	// if run_flag=1, ignore KEY1
		      run_flag =  1;
                      Display_key(key);
                      SysTick->CTRL  = 0x00000003;// interrupt enable
                      break;
          case KEY3 : if(run_flag == 1) break;	// if run_flag=1, ignore KEY3
	              SysTick->CTRL  = 0x00000001;// interrupt disable
                      Clear_time();
                      Display_key(key);
                      break;
          case KEY4 : if(run_flag == 0) break;	// if run_flag=0, ignore KEY4
                      run_flag = 0;
	              SysTick->CTRL  = 0x00000001;// interrupt disable
                      Display_key(key);
                      break;
          default:    break;
        }
    }
}

/* ----- ����� ���� �Լ� ----------------------------------------------------- */

void Clear_time(void)				/* clear time */
{
  second = 0;                                   // clear time
  minute = 0;
  hour = 0;

  TFT_string(12,10, White,Black, "00 : 00 : 00.00"); // clear display
  TFT_string(15,10, Cyan,Black, ":");
  TFT_string(20,10, Cyan,Black, ":");
  TFT_string(24,10, Cyan,Black, ".");
}

void Display_key(U08 key)			/* display key input */
{
  TFT_string(10,18, White,Black, "KEY1");
  TFT_string(10,22, White,Black, "KEY3");
  TFT_string(10,24, White,Black, "KEY4");

  if(key == KEY1)      TFT_string(10,18, Magenta,Black, "KEY1");
  else if(key == KEY3) TFT_string(10,22, Magenta,Black, "KEY3");
  else if(key == KEY4) TFT_string(10,24, Magenta,Black, "KEY4");
}
