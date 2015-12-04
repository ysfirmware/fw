/* ============================================================================ */
/*	Exp01_1_LED.c : �⺻ LED �� ���� ��� ���α׷�(16MHz HSI ���)          */
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"

void Delay_us(unsigned int time_us);		/* time delay for us in 16MHz */
void Delay_ms(unsigned int time_ms);		/* time delay for ms in 16MHz */
void Beep(void);				/* beep for 50 ms */

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  unsigned int i;

  RCC->AHB1ENR = 0x00000014;			// port C and E clock enable

  GPIOC->MODER &= 0x3FFFFFFF;			// set PC15(buzzer) to output
  GPIOC->MODER |= 0x40000000;

  GPIOE->MODER &= 0x3FFFFFFF;			// set PE15(LED) to output
  GPIOE->MODER |= 0x40000000;

  while(1)
    { for(i = 0; i < 5; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(500);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(500);
        }

      Beep();					// beep
    }
}

/* ----- ����� �Լ� ---------------------------------------------------------- */

void SystemInit(void)				/* dummy system function */
{
  asm volatile("NOP");
}

void Delay_us(unsigned int time_us)		/* time delay for us in 16MHz */
{
  register unsigned int i;

  for(i = 0; i < time_us; i++)			// 3 cycle
    { asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");			// 13 cycle
    }
}

void Delay_ms(unsigned int time_ms)		/* time delay for ms in 16MHz */
{
  register unsigned int i;

  for(i = 0; i < time_ms; i++)
    Delay_us(1000);
}

void Beep(void)					/* beep for 50 ms */
{ 
  GPIOC->BSRR = 0x00008000;   			// buzzer on
  Delay_ms(50);					// delay 50 ms
  GPIOC->BSRR = 0x80000000;			// buzzer off
}
