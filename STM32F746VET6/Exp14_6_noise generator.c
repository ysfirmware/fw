/* ============================================================================ */
/*	Exp14_6_noise generator.c : D/A �����͸� �̿��� ������ �߻���	        */
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Initialize_touch_screen();			// initialize touch screen

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp14_6.c    ");

  TFT_string(3,5,White,Magenta," D/A �������� �����Լ� ���� ��� ");
  TFT_string(11,15,White,Black," J1(DAC1_OUT) ����");
  TFT_string( 7,18,Green,Black," ������ �߻��� (0~+3.3V)");
  Beep();

  GPIOA->MODER |= 0x00000300;			// PA4 = DAC_OUT1
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock
  DAC->CR = 0x00000B7D;				// DAC enable, output buffer and trigger disable
  
  while(1)
  {
    DAC->SWTRIGR |= 0x00000001; 
    DAC->DOR1 = DAC->DHR12R1;
  }
}