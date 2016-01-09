/* ============================================================================ */
/*   Exp04_5_key_interrupt(1).c : 키입력에 의한 EXTI 인터럽트(디바운싱 미사용)	*/
/* ============================================================================ */
/*				        programmed by Seung-Yong Lee in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"


void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

volatile unsigned int interrupt_flag, KEY1_count, KEY2_count, KEY3_count, KEY4_count;


void EXTI0_IRQHandler(void)
{
  KEY1_count++;
  
  EXTI->PR = 0x00000001;
  
  GPIOE->BSRR = 0x00008000;		// LED on
  Beep();
  GPIOE->BSRR = 0x80000000;		// LED off
  
  interrupt_flag = 1;
}

void EXTI1_IRQHandler(void)
{
  KEY2_count++;
  
  EXTI->PR = 0x00000002;
  
  GPIOE->BSRR = 0x00008000;		// LED on
  Beep();
  GPIOE->BSRR = 0x80000000;		// LED off  
  
  interrupt_flag = 1;
}


void EXTI2_IRQHandler(void)
{
  KEY3_count++;
  
  EXTI->PR = 0x00000004;
  
  GPIOE->BSRR = 0x00008000;		// LED on
  Beep();
  GPIOE->BSRR = 0x80000000;		// LED off 

  interrupt_flag = 1;
}


void EXTI3_IRQHandler(void)
{
  KEY4_count++;

  EXTI->PR = 0x00000008;
  
  GPIOE->BSRR = 0x00008000;		// LED on
  Beep();
  GPIOE->BSRR = 0x80000000;		// LED off
  
  interrupt_flag = 1;

}


/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp04_5.c    ");
  Beep();

  TFT_string(7, 8, White,Magenta, " 키입력 인터럽트 프로그램 ");
  TFT_string(7,12, White,  Black, "    KEY1 입력 = 000000    ");
  TFT_string(7,15, White,  Black, "    KEY2 입력 = 000000    ");
  TFT_string(7,18, White,  Black, "    KEY3 입력 = 000000    ");
  TFT_string(7,21, White,  Black, "    KEY4 입력 = 000000    ");

  GPIOC->MODER &= 0xFFFFFF00;			// PC3,2,1,0 = input mode
  RCC->AHB1ENR |= 0x00000004;			// enable SYSCFG
  SYSCFG->EXTICR[0] = 0x00002222;               // EXTI3,2,1,0 = PC3,2,1,0(KEY4,3,2,1)
  
  EXTI->IMR = 0x0000000F;			// enable EXTI3,2,1,0 interrupt
  EXTI->RTSR = 0x00000000;			// disable EXTI3,2,1,0 rising edge
  EXTI->FTSR = 0x0000000F;			// enable EXTI3,2,1,0 falling edge
  
  //NVIC_EnableIRQ(EXTI0_IRQn);
  //NVIC_EnableIRQ(EXTI1_IRQn);
  //NVIC_EnableIRQ(EXTI2_IRQn);
  //NVIC_EnableIRQ(EXTI3_IRQn);
  NVIC->ISER[0] = 0x000003C0;
  
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
