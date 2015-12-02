/* ============================================================================ */
/*    Exp04_6_key_interrupt(2).c : 키입력에 의한 EXTI 인터럽트(디바운싱 사용)	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void EXTI4_15_IRQHandler(void);			/* EXTI4_15 interrupt function */

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

volatile unsigned int interrupt_flag, KEY1_count, KEY2_count, KEY3_count, KEY4_count;

void EXTI4_15_IRQHandler(void)			/* EXTI4_15 interrupt function */
{
  unsigned char key;

  key = (GPIOB->IDR & 0x00000F00) >> 8;
  if(key == KEY1)      KEY1_count++;
  else if(key == KEY2) KEY2_count++;
  else if(key == KEY3) KEY3_count++;
  else if(key == KEY4) KEY4_count++;

  GPIOA->BSRR = 0x00000800;			// LED on
  Beep();
  GPIOA->BRR = 0x00000800;			// LED off

  while((GPIOB->IDR & 0x00000F00) != 0x00000F00); // debouncing
  Delay_ms(20);

  EXTI->PR = 0x00000F00;			// clear pending bit of EXTI11,10,9,8

  interrupt_flag = 1;
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp04_6.c    ");
  Beep();

  TFT_string(7, 8, White,Magenta, " 키입력 인터럽트 프로그램 ");
  TFT_string(7,12, White,  Black, "    KEY1 입력 = 000000    ");
  TFT_string(7,15, White,  Black, "    KEY2 입력 = 000000    ");
  TFT_string(7,18, White,  Black, "    KEY3 입력 = 000000    ");
  TFT_string(7,21, White,  Black, "    KEY4 입력 = 000000    ");

  GPIOB->MODER &= 0xFF00FFFF;			// PB11,10,9,8 = input mode
  RCC->APB2ENR |= 0x00000001;			// enable SYSCFG
  SYSCFG->EXTICR[2] = 0x00001111;		// EXTI11,10,9,8 = PB11,10,9,8(KEY4,3,2,1)
  EXTI->IMR = 0x00000F00;			// enable EXTI11,10,9,8 interrupt
  EXTI->RTSR = 0x00000000;			// disable EXTI11,10,9,8 rising edge
  EXTI->FTSR = 0x00000F00;			// enable EXTI11,10,9,8 falling edge
  NVIC->ISER[0] = 0x00000080;			// enable (7)EXTI4_15 interrupt

  interrupt_flag = 0;				// initialize variables
  KEY1_count = 0;
  KEY2_count = 0;
  KEY3_count = 0;
  KEY4_count = 0;

  while(1)
    { if(interrupt_flag == 1)			// interrupt ?
        { interrupt_flag = 0;
  	  TFT_color(Green,Black);
	  TFT_xy(23,12);			// display KEY1 count
	  TFT_unsigned_decimal(KEY1_count,1,6);
	  TFT_xy(23,15);			// display KEY2 count
	  TFT_unsigned_decimal(KEY2_count,1,6);
	  TFT_xy(23,18);			// display KEY3 count
	  TFT_unsigned_decimal(KEY3_count,1,6);
	  TFT_xy(23,21);			// display KEY4 count
	  TFT_unsigned_decimal(KEY4_count,1,6);
        }
    }
}
