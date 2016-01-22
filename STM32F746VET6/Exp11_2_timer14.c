/* ============================================================================ */
/*		Exp11_2_timer14.c : Ÿ�̸�14�� ����� �ֱ����� ���ͷ�Ʈ		*/
/* ============================================================================ */
/*					   programmed by Dong-Ki Kim in 2016.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void TIM8_TRG_COM_TIM14_IRQHandler(void);	/* TIM14 interrupt function (2Hz) */

volatile unsigned int interrupt_count;		// interrupt count
volatile unsigned char interrupt_flag, beep_flag; // flag by interrupt

/* ----- ���ͷ�Ʈ ó�� ���α׷� ----------------------------------------------- */

void TIM8_TRG_COM_TIM14_IRQHandler(void)	/* TIM14 interrupt function (2Hz) */
{
  TIM14->SR = 0x0000;				// clear pending bit of TIM14 interrupt
    
  interrupt_flag = 1;				// interrupt ocurred

  interrupt_count++;				// increment interrupt count
  if((interrupt_count % 10) == 0)		// beep in 5-second interval
    beep_flag = 1;

  GPIOE->ODR ^= 0x00008000;			// toggle LED
}

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp11_2.c    ");
  Beep();

  TFT_string(11,10,White,Magenta," Ÿ�̸�14 ���ͷ�Ʈ ");
  TFT_string(18,16,Green,Black,  "00000");

  interrupt_count = 0;				// initialize interrupt variables
  interrupt_flag = 0;
  beep_flag = 0;

  RCC->APB1ENR |= 0x00000100;			// enable TIM14 clock
  RCC->DCKCFGR1 |= 0x01000000;                  // TIM14CLK = HCLK
  TIM14->PSC = 2159;				// 216MHz/(2159+1) = 100kHz
  TIM14->ARR = 49999;				// 100kHz / (49999+1) = 2Hz
  TIM14->CNT = 0;				// clear counter
  TIM14->DIER = 0x0001;				// enable update interrupt
  TIM14->CR1 = 0x0005;				// overflow interrupt, enable TIM14

  NVIC->ISER[1] |= 0x00002000;			// enable (45)TIM14 interrupt

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