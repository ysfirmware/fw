/* ============================================================================ */
/*		Exp12_1_PWM(1).c : 1채널 PWM 출력 프로그램			*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned char key;
  unsigned int PWM;				// pulse width

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp12_1.c    ");
  Beep();

  TFT_string(5, 5, White,Magenta, " 타이머 TIM1의 PWM 출력 모드 ");
  TFT_string(6,10, Cyan,   Black, "TIM1_CH4 : 주기   = 1000 ms");
  TFT_string(6,13, Cyan,   Black, " (OC4)     펄스폭 =  500 ms");
  TFT_string(6,16, Cyan,   Black, "           듀티비 =   50 %");
  TFT_string(4,24, White,  Black, "KEY3/KEY4로 듀티비를 가변하시오.");

  PWM = 2399;					// initialize PWM = 499 (duty ratio 50%)

  GPIOA->MODER &= 0xFF3FFFFF;			// PA11 = alternate function mode
  GPIOA->MODER |= 0x00800000;
  GPIOA->AFR[1] |= 0x00002000;			// PA11 = TIM1_CH4

  RCC->APB2ENR |= 0x00000800;			// enable TIM1 clock
  TIM1->PSC = 0;				// 48MHz/(47999+1) = 1kHz
  TIM1->ARR = 4799;				// 1kHz / (999+1) = 1Hz
  TIM1->CCR4 = PWM;
  TIM1->CNT = 0;				// clear counter
  TIM1->CCMR2 = 0x6C00;				// OC4M = 110(PWM mode 1), CC4S = 00(output)
  TIM1->CCER = 0x1000;				// CC4E = 1(enable OC4 output)
  TIM1->BDTR = 0x8000;				// MOE = 1
  TIM1->CR1 = 0x0005;				// edge-aligned, up-counter, enable TIM1

  while(1)
    { 
  
  
  
    }
}
