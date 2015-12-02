/* ============================================================================ */
/*		Exp02_1_MCO.c : MCO 출력 프로그램			       */
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

  LCD_string(0x80,"OK-STM32F0  V1.0");          // display title
  LCD_string(0xC0,"   Exp02_1.c    ");

  TFT_string( 0,8,White,Magenta,"    MCO 단자(TP2) 주파수 출력 테스트    ");

  GPIOA->MODER &= 0xFFFCFFFF;			// PA8 = MCO(alternate function)
  GPIOA->MODER |= 0x00020000;
  GPIOA->AFR[1] &= 0xFFFFFFF0;

  RCC->CSR |= 0x00000001;			// LSION = 1
  RCC->CR2 |= 0x00000001;			// HSI14ON = 1

  while(1)
    { TFT_string( 0,16,Cyan,Black,"            MCO = LSI(40kHz)            ");
      TFT_string(22,16, Red,Black,"40kHz");
      Beep();
      RCC->CFGR &= 0xF8FFFFFF;			// MCO[2:0] = 010 = LSI
      RCC->CFGR |= 0x02000000;
      Delay_ms(5000);

      TFT_string( 0,16,Cyan,Black,"            MCO = HSI(8MHz)             ");
      TFT_string(22,16, Red,Black,"8MHz");
      Beep();
      RCC->CFGR &= 0xF8FFFFFF;			// MCO[2:0] = 101 = HSI
      RCC->CFGR |= 0x05000000;
      Delay_ms(5000);

      TFT_string( 0,16,Cyan,Black,"           MCO = HSI14(14MHz)           ");
      TFT_string(23,16, Red,Black,"14MHz");
      Beep();
      RCC->CFGR &= 0xF8FFFFFF;			// MCO[2:0] = 001 = HSI14
      RCC->CFGR |= 0x01000000;
      Delay_ms(5000);

      TFT_string( 0,16,Cyan,Black,"            MCO = HSE(16MHz)            ");
      TFT_string(22,16, Red,Black,"16MHz");
      Beep();
      RCC->CFGR &= 0xF8FFFFFF;			// MCO[2:0] = 110 = HSE
      RCC->CFGR |= 0x06000000;
      Delay_ms(5000);

      TFT_string( 0,16,Cyan,Black,"           MCO = PLL/2(24MHz)           ");
      TFT_string(23,16, Red,Black,"24MHz");
      Beep();
      RCC->CFGR &= 0xF8FFFFFF;			// MCO[2:0] = 111 = PLL/2
      RCC->CFGR |= 0x07000000;
      Delay_ms(5000);
    }
}
