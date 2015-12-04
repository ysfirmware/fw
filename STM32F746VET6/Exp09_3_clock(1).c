/* ============================================================================ */
/*	Exp09_3_clock(1).c : SysTick 타이머를 이용한 디지털 시계(1)		*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void SysTick_Handler(void);			/* SysTick interrupt function */

volatile unsigned char ampm = 'A', hour = 12, minute = 0, second = 0, count100Hz = 0;

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  count100Hz++;                                 //100Hz = 0.01sec -> 100count
    if(count100Hz == 100)                       //0.01sec x 100 = 1sec
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

  if(ampm == 'A') TFT_string(13,14,White,Black,"오전"); // display AM/PM
  else            TFT_string(13,14,White,Black,"오후");

  TFT_color(Green,Black);
  TFT_xy(18,14);				// display hour
  TFT_unsigned_decimal(hour,1,2);
  TFT_xy(21,14);				// display minute
  TFT_unsigned_decimal(minute,1,2);
  TFT_xy(24,14);				// display second
  TFT_unsigned_decimal(second,1,2);

  if((minute == 0x00) && (second == 0x00) && (count100Hz == 0x00))        // beep on every hour
    Beep();
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title
  LCD_string(0xC0,"   Exp09_3.c    ");
  Beep();

  Rectangle(0,0, 319,239, Green);		// display clock title
  Rectangle(1,1, 318,238, Green);
  TFT_string( 1, 4,White,Magenta," SysTick 인터럽트에 의한  디지털 시계 ");
  TFT_string(13,14,White,  Black,"오전 12:00:00");

  SysTick->LOAD  = 269999;			// 27MHz/(269999+1) = 100Hz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 216MHz/8 = 27MHz, SysTick enable and interrupt request

  while(1);
}
