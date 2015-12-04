/* ============================================================================ */
/*		Exp02_1_clock.c : 클록 주파수 확인 프로그램		        */
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");          // display title
  LCD_string(0xC0,"   Exp02_1.c    ");
  Beep();

  TFT_string(0,5,White,Magenta,"   MCO1/MCO2 단자 주파수 출력 테스트   ");

  GPIOA->MODER &= 0xFFFCFFFF;			// PA8 = MCO1(PA8 = 180MHz high speed)
  GPIOA->MODER |= 0x00020000;
  GPIOA->AFR[1] &= 0xFFFFFFF0;
  GPIOA->OSPEEDR |= 0x00030000;

  GPIOC->MODER &= 0xFFF3FFFF;			// PC9 = MCO2(PC9 = 180MHz high speed)
  GPIOC->MODER |= 0x00080000;
  GPIOC->AFR[1] &= 0xFFFFFF0F;
  GPIOC->OSPEEDR |= 0x000C0000;

  while(1)
    { TFT_string(0,16,Cyan,Black,"     MCO1(TP8) = HSI      =  16MHz     ");
      TFT_string(0,19,Cyan,Black,"     MCO2(TP9) = SYSCLK/2 = 108MHz     ");
      RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSI/1
      RCC->CFGR |= 0x20000000;			// MCO2 = SYSCLK/2
      Delay_ms(5000);
      Beep();

      TFT_string(0,16,Cyan,Black,"     MCO1(TP8) = HSI/2    =   8MHz     ");
      TFT_string(0,19,Cyan,Black,"     MCO2(TP9) = SYSCLK/4 =  54MHz     ");
      RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSI/2
      RCC->CFGR |= 0x34000000;			// MCO2 = SYSCLK/4
      Delay_ms(5000);
      Beep();

      TFT_string(0,16,Cyan,Black,"     MCO1(TP8) = HSE      =  16MHz     ");
      TFT_string(0,19,Cyan,Black,"     MCO2(TP9) = PLL/2    = 108MHz     ");
      RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSE/1
      RCC->CFGR |= 0xE0400000;			// MCO2 = PLL/2
      Delay_ms(5000);
      Beep();

      TFT_string(0,16,Cyan,Black,"     MCO1(TP8) = HSE/2    =   8MHz     ");
      TFT_string(0,19,Cyan,Black,"     MCO2(TP9) = PLL/4    =  54MHz     ");
      RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSE/2
      RCC->CFGR |= 0xF4400000;			// MCO2 = PLL/4
      Delay_ms(5000);
      Beep();
    }
}
