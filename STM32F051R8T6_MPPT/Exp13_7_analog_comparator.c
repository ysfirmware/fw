/* ============================================================================ */
/*	Exp13_7_analog_comparator.c : 아날로그 비교기 응용 프로그램		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp13_7.c    ");
  Beep();

  TFT_string(5,10,White,Magenta," 아날로그 비교기 COMP2의 응용 ");
  TFT_string(5,18,White,Black,"(+)=ADC1=3.3V/2   (-)=ADC2=VR1");

  GPIOA->MODER |= 0x0000003C;			// use ADC1, ADC2
  GPIOA->MODER = ((GPIOA->MODER & 0xFCFFFFFF) | 0x02000000); // PA12 = COMP2_OUT
  GPIOA->AFR[1] = 0x00070000;
  RCC->APB2ENR |= 0x00000001;			// enable SYSCFG and COMP clock

  COMP->CSR = 0x38E10001;			// high hysteresis, invert output COMP2_OUT,
						// (+) = ADC1, (-) = ADC2, enable COMP2 and COMP1

  while(1)
    { if((COMP->CSR & 0x40000000) == 0x40000000)// if ADC1 < ADC2, LED = on and buzzer on
        { TFT_string(21,18, Magenta,Black, "<");
          GPIOA->BSRR = 0x00000800;
        }
      else					// if ADC1 > ADC2, LED = off and buzzer off
        { TFT_string(21,18, Magenta,Black, ">");
          GPIOA->BRR = 0x00000800;
        }
    }
}
