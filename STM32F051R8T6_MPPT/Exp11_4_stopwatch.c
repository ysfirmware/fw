/* ============================================================================ */
/*		Exp11_4_stopwatch.c : 타이머6을 이용한 전자 스톱워치		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void TIM6_DAC_IRQHandler(void);			/* TIM6 interrupt function */
void Clear_time(void);				/* clear time */
void Display_key(U08 key);			/* display key input */

volatile unsigned int second, minute, hour;
volatile unsigned char run_flag;

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void TIM6_DAC_IRQHandler(void)			/* TIM6 interrupt function */
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

  TFT_color(Magenta,Black);			// display time
  TFT_xy(12,10);
  TFT_unsigned_decimal(hour,1,2);
  TFT_xy(17,10);
  TFT_unsigned_decimal(minute,1,2);
  TFT_xy(22,10);
  TFT_unsigned_decimal(second/100,1,2);
  TFT_xy(25,10);
  TFT_unsigned_decimal(second%100,1,2);

  TIM6->SR = 0x0000;				// clear pending bit of TIM6 interrupt
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned char key;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp11_4.c    ");
  Beep();

  TFT_string( 4, 4, White,Magenta, " 마이크로컨트롤러 전자 스톱워치 ");
  TFT_string(10,18, White,Black, "KEY1 - 스타트(start)");
  TFT_string(10,20, White,Black, "KEY2 -");
  TFT_string(10,22, White,Black, "KEY3 - 리셋(reset)");
  TFT_string(10,24, White,Black, "KEY4 - 스톱(stop)");
  Clear_time();					/* clear time */

  RCC->APB1ENR |= 0x00000010;			// enable TIM6 clock
  TIM6->PSC = 4799;				// 48MHz/(4799+1) = 10kHz
  TIM6->ARR = 99;				// 10kHz / (99+1) = 100Hz
  TIM6->CNT = 0;				// clear counter
  TIM6->DIER = 0x0000;				// disable update interrupt
  TIM6->CR1 = 0x0005;				// overflow interrupt, enable TIM6

  NVIC->ISER[0] |= 0x00020000;			// enable (17)TIM6 interrupt

  run_flag = 0;					// initial status

  while(1)
    { key = Key_input();			// key input
      switch(key)
        { case KEY1 : if(run_flag == 1) break;	// if run_flag=1, ignore KEY1
		      run_flag =  1;
                      Display_key(key);
                      TIM6->DIER = 0x0001;	// enable update interrupt
                      break;
          case KEY3 : if(run_flag == 1) break;	// if run_flag=1, ignore KEY3
	              TIM6->DIER = 0x0000;	// disable update interrupt
                      Clear_time();
                      Display_key(key);
                      break;
          case KEY4 : if(run_flag == 0) break;	// if run_flag=0, ignore KEY4
                      run_flag = 0;
	              TIM6->DIER = 0x0000;	// disable update interrupt
                      Display_key(key);
                      break;
          default:    break;
        }
    }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

void Clear_time(void)				/* clear time */
{
  second = 0;                                   // clear time
  minute = 0;
  hour = 0;

  TFT_string(12,10, Magenta,Black, "00 : 00 : 00.00"); // clear display
  TFT_string(15,10, Green,  Black, ":");
  TFT_string(20,10, Green,  Black, ":");
  TFT_string(24,10, Green,  Black, ".");
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
