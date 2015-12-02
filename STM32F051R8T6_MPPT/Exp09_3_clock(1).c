/* ============================================================================ */
/*	Exp09_3_clock(1).c : SysTick Ÿ�̸Ӹ� �̿��� ������ �ð�(1)		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void SysTick_Handler(void);			/* SysTick interrupt function */

volatile unsigned char ampm = 'A', hour = 12, minute = 0, second = 0;

/* ----- ���ͷ�Ʈ ó�� ���α׷� ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  second++;					// increment second
  if(second == 60)                              // if second = 60, second = 0
    { second = 0;
      minute++;                                 // increment minute
      if(minute == 60)                          // if minute = 60, minute = 0
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

  if(ampm == 'A') TFT_string(13,14,White,Black,"����"); // display AM/PM
  else            TFT_string(13,14,White,Black,"����");

  TFT_color(Green,Black);
  TFT_xy(18,14);				// display hour
  TFT_unsigned_decimal(hour,1,2);
  TFT_xy(21,14);				// display minute
  TFT_unsigned_decimal(minute,1,2);
  TFT_xy(24,14);				// display second
  TFT_unsigned_decimal(second,1,2);

  if((minute == 0x00) && (second == 0x00))	// beep on every hour
    Beep();
}

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp09_3.c    ");
  Beep();

  Rectangle(0,0, 319,239, Green);		// display clock title
  Rectangle(1,1, 318,238, Green);
  TFT_string( 1, 4,White,Magenta," SysTick ���ͷ�Ʈ�� ����  ������ �ð� ");
  TFT_string(13,14,White,  Black,"���� 12:00:00");

  SysTick->LOAD  = 5999999;			// 6MHz/(5999999+1) = 1Hz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 48MHz/8 = 6MHz, SysTick enable and interrupt request

  while(1);
}
