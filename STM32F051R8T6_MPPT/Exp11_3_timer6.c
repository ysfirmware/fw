/* ============================================================================ */
/*		Exp11_3_timer6.c : 타이머6을 사용한 가변 주기 인터럽트		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void TIM6_DAC_IRQHandler(void);			/* TIM6 interrupt function */

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void TIM6_DAC_IRQHandler(void)			/* TIM6 interrupt function */
{
  GPIOA->ODR ^= 0x00000800;			// toggle LED

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
  LCD_string(0xC0,"   Exp11_3.c    ");
  Beep();

  TFT_string( 2, 4, White,Magenta, " 타이머6 인터럽트를 이용한 LED 점멸 ");
  TFT_string(10,10, Green,  Black, "100ms ON / 100ms OFF");
  TFT_string( 7,18, White,  Black, "KEY1 - 100ms 주기 인터럽트");
  TFT_string( 7,20, White,  Black, "KEY2 - 200ms 주기 인터럽트");
  TFT_string( 7,22, White,  Black, "KEY3 - 400ms 주기 인터럽트");
  TFT_string( 7,24, White,  Black, "KEY4 - 800ms 주기 인터럽트");

  RCC->APB1ENR |= 0x00000010;			// enable TIM6 clock
  TIM6->PSC = 4799;				// 48MHz/(4799+1) = 10kHz
  TIM6->ARR = 999;				// 10kHz / (999+1) = 10Hz
  TIM6->CNT = 0;				// clear counter
  TIM6->DIER = 0x0001;				// enable update interrupt
  TIM6->CR1 = 0x0085;				// use buffer of TIM6_ARR, enable TIM6

  NVIC->ISER[0] |= 0x00020000;			// enable (17)TIM6 interrupt

  while(1)
    { key = Key_input();			// key input
      switch(key)
        { case KEY1 : TIM6->ARR  = 999;		// 10kHz/(999+1) = 10Hz
                      TFT_string(10,10, Green,  Black, "100ms ON / 100ms OFF");
                      break;
          case KEY2 : TIM6->ARR  = 1999;	// 10kHz/(1999+1) = 5Hz
                      TFT_string(10,10, Green,  Black, "200ms ON / 200ms OFF");
                      break;
          case KEY3 : TIM6->ARR  = 3999;	// 10kHz/(3999+1) = 2.5Hz
                      TFT_string(10,10, Green,  Black, "400ms ON / 400ms OFF");
                      break;
          case KEY4 : TIM6->ARR  = 7999;	// 10kHz/(7999+1) = 1.25Hz
                      TFT_string(10,10, Green,  Black, "800ms ON / 800ms OFF");
                      break;
        }
    }
}
