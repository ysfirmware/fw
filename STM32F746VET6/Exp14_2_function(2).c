/* ============================================================================ */
/*    Exp14_2_function(2).c : D/A 컨버터를 사용한 정확한 주파수의 삼각파 출력	*/
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void SysTick_Handler(void);			/* SysTick interrupt function */

volatile unsigned short data_count = 0, DAC_data[200];

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  DAC->DHR12R1 = DAC_data[data_count];		// output D/A data

  data_count++;
  if(data_count >= 200) data_count = 0;
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned short i, x;
  

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp14_2.c    ");

  TFT_string(3,5,White,Magenta," D/A 컨버터의 수학함수 파형 출력 ");
  TFT_string(11,15,White,Black,"TP3(DAC1_OUT) 단자");
  TFT_string( 4,18,Green,Black,"약 500Hz의 삼각파 출력 (0~+3.3V)");
  Beep();

  for(x = 0; x <= 100; x++)			// calculate data of triangular wave
    DAC_data[x] = (unsigned short)((4095./100.) * (float)x);
  i = 1;
  for(x = 101; x < 200; x++)
    { DAC_data[x] = DAC_data[100-i];
      i++;
    }

  GPIOA->MODER |= 0x00000300;			// PA4 = DAC_OUT1
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock

  DAC->CR = 0x00000003;				// DAC enable, output buffer and trigger disable
  
  SysTick->LOAD  = 269;			        // 27MHz/(1349999+1) = 20Hz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 216MHz/8 = 27MHz, SysTick enable and interrupt disable

  while(1);
}
