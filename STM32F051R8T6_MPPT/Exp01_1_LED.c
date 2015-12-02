/* ============================================================================ */
/*	Exp01_1_LED.c : 기본 LED 및 버저 출력 프로그램(8MHz HSI 사용)           */
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"

void Delay_us(unsigned int time_us);		/* time delay for us in 8MHz */
void Delay_ms(unsigned int time_ms);		/* time delay for ms in 8MHz */
void Beep(void);				/* beep for 50 ms */

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned int i;

  RCC->AHBENR = 0x00020000;			// IOPAEN = 1

  GPIOA->MODER &= 0x3C000000;			// reserve PA14(SWCLK) and PA13(SWDIO)
  GPIOA->MODER |= 0x01400000;			// set PA12(BUZZER) and PA11(LED) to output

  while(1)
    { for(i = 0; i < 5; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(500);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(500);
        }
      Beep();					// beep
    }
}

/* ----- 사용자 함수 ---------------------------------------------------------- */

void SystemInit(void)				/* dummy system function */
{
  asm volatile("NOP");
}

void Delay_us(unsigned int time_us)		/* time delay for us in 8MHz */
{
  register unsigned int i;

  for(i = 0; i < time_us; i++)			// 3 cycle
    { asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");				// 4 cycle
    }
}

void Delay_ms(unsigned int time_ms)		/* time delay for ms in 8MHz */
{
  register unsigned int i;

  for(i = 0; i < time_ms; i++)
    Delay_us(1000);
}

void Beep(void)					/* beep for 50 ms */
{ 
  GPIOA->BSRR = 0x00001000; 		  	// buzzer on
  Delay_ms(50);					// delay 50 ms
  GPIOA->BRR = 0x00001000;			// buzzer off
}
