/* ============================================================================ */
/*		Exp09_1_SysTick.c : SysTick 타이머 인터럽트			*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void SysTick_Handler(void);			/* SysTick interrupt function */

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  GPIOA->ODR ^= 0x00000800;			// toggle LED
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
  LCD_string(0xC0,"   Exp09_1.c    ");
  Beep();

  TFT_string( 1, 4, White,Magenta, " SysTick 타이머 인터럽트 이용 LED 점멸 ");
  TFT_string(10,10, Green,  Black, "100ms ON / 100ms OFF");
  TFT_string( 7,18, White,  Black, "KEY1 - 100ms 주기 인터럽트");
  TFT_string( 7,20, White,  Black, "KEY2 - 200ms 주기 인터럽트");
  TFT_string( 7,22, White,  Black, "KEY3 - 400ms 주기 인터럽트");
  TFT_string( 7,24, White,  Black, "KEY4 - 800ms 주기 인터럽트");

  SysTick->LOAD  = 599999;			// 6MHz/(599999+1) = 10Hz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 48MHz/8 = 6MHz, SysTick enable and interrupt request

  while(1)
    { key = Key_input();			// key input
      switch(key)
        { case KEY1 : SysTick->LOAD  = 599999;	// 6MHz/(599999+1) = 10Hz
                      TFT_string(10,10, Green,  Black, "100ms ON / 100ms OFF");
                      break;
          case KEY2 : SysTick->LOAD  = 1199999;	// 6MHz/(1199999+1) = 5Hz
                      TFT_string(10,10, Green,  Black, "200ms ON / 200ms OFF");
                      break;
          case KEY3 : SysTick->LOAD  = 2399999;	// 6MHz/(2399999+1) = 2.5Hz
                      TFT_string(10,10, Green,  Black, "400ms ON / 400ms OFF");
                      break;
          case KEY4 : SysTick->LOAD  = 4799999;	// 6MHz/(4799999+1) = 1.25Hz
                      TFT_string(10,10, Green,  Black, "800ms ON / 800ms OFF");
                      break;
        }
    }
}
