/* ================================================================================== */
/* Exp14_8_simultaneous triangle generator.c : D/A 컨버터를 이용한 동시 삼각파 발생기 */
/* ================================================================================== */
/*					programmed by Ho-Jun Lee in 2015.             */

#include "stm32f746xx.h"
#include "OK-STM746.h"

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Initialize_touch_screen();			// initialize touch screen

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp14_6.c    ");

  TFT_string(3,5,White,Magenta," D/A 컨버터의 수학함수 파형 출력 ");
  TFT_string(7,12,White,Black,"J1(DAC1_OUT) J2(DAC2_OUT) 단자");
  TFT_string( 7,18,Green,Black," 듀얼 삼각파 발생기 (0~+3.3V)");
  Beep();

  GPIOA->MODER |= 0x00000300;			// PA4 = DAC_OUT1
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock
  DAC->CR = 0x0FBD0FBD;				// DAC enable, output buffer and trigger disable
  
  while(1)
  {
    DAC->SWTRIGR |= 0x00000003; 
    DAC->DOR1 = (unsigned short)(DAC->DHR12RD & 0x00000FFF);
    DAC->DOR2 = (unsigned short)((DAC->DHR12RD & 0x0FFF0000) >> 16);
  }
}