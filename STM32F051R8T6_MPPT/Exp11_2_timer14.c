/* ============================================================================ */
/*		Exp11_2_timer14.c : 타이머14를 사용한 주기적인 인터럽트		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void TIM14_IRQHandler(void);			/* TIM14 interrupt function (2Hz) */

volatile unsigned int interrupt_count;		// interrupt count
volatile unsigned char interrupt_flag, beep_flag; // flag by interrupt

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void TIM14_IRQHandler(void)			/* TIM14 interrupt function (2Hz) */
{
  interrupt_flag = 1;				// interrupt ocurred

  interrupt_count++;				// increment interrupt count
  if((interrupt_count % 10) == 0)		// beep in 5-second interval
    beep_flag = 1;

  GPIOA->ODR ^= 0x00000800;			// toggle LED

  TIM14->SR = 0x0000;				// clear pending bit of TIM14 interrupt
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp11_2.c    ");
  Beep();

  TFT_string(11,10,White,Magenta," 타이머14 인터럽트 ");
  TFT_string(18,16,Green,Black,  "00000");

  interrupt_count = 0;				// initialize interrupt variables
  interrupt_flag = 0;
  beep_flag = 0;

  RCC->APB1ENR |= 0x00000100;			// enable TIM14 clock
  TIM14->PSC = 479;				// 48MHz/(479+1) = 100kHz
  TIM14->ARR = 49999;				// 100kHz / (49999+1) = 2Hz
  TIM14->CNT = 0;				// clear counter
  TIM14->DIER = 0x0001;				// enable update interrupt
  TIM14->CR1 = 0x0005;				// overflow interrupt, enable TIM14

  NVIC->ISER[0] |= 0x00080000;			// enable (19)TIM14 interrupt

  while(1)
    { if(interrupt_flag == 1)
        { interrupt_flag = 0;
	  TFT_xy(18,16);			// display interrupt count
          TFT_color(Green,Black);
          TFT_unsigned_decimal(interrupt_count,1,5);

          if(beep_flag == 1)			// if 5 seconds, beep
            { beep_flag = 0;
              Beep();
	    }
        }
    }
}
