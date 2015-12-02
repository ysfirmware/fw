/* ============================================================================ */
/*	Exp02_2_register.c : 몇가지 시스템 레지스터 및 I/O 레지스터 값의 확인	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned int x;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Beep();					// beep

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp02_2.c    ");

  TFT_string(10, 1, White,Magenta," 시스템 레지스터 값 ");
  TFT_string( 5, 4, White,  Black,"    SCB_CPUID = 0x00000000    ");
  TFT_string( 5, 7, White,  Black," SysTick_CTRL = 0x00000000    ");
  TFT_string( 9,11, White,   Blue," I/O 제어 레지스터 값 ");
  TFT_string( 5,14,Yellow,  Black,"    FLASH_ACR = 0x00000000    ");
  TFT_string( 5,17,Yellow,  Black,"      PWR_CSR = 0x00000000    ");
  TFT_string( 5,20,Yellow,  Black,"      RCC_CSR = 0x00000000    ");
  TFT_string( 5,23,Yellow,  Black,"     EXTI_IMR = 0x00000000    ");
  TFT_string( 5,26,Yellow,  Black,"    GPIOB_IDR =     0x0000    ");

  while(1)
    { x = SCB->CPUID;				// SCB_CPUID
      TFT_xy(23,4);
      TFT_color(Cyan,Black);
      TFT_hexadecimal(x,8);

      x = SysTick->CTRL;			// SysTick_CTRL
      TFT_xy(23,7);
      TFT_hexadecimal(x,8);

      x = FLASH->ACR;				// FLASH_ACR
      TFT_xy(23,14);
      TFT_hexadecimal(x,8);

      x = PWR->CSR;				// PWR_CSR
      TFT_xy(23,17);
      TFT_hexadecimal(x,8);

      x = RCC->CSR;				// RCC_CSR
      TFT_xy(23,20);
      TFT_hexadecimal(x,8);

      x = EXTI->IMR;				// EXTI_IMR
      TFT_xy(23,23);
      TFT_hexadecimal(x,8);

      x = GPIOB->IDR;				// GPIOB_IDR
      TFT_xy(27,26);
      TFT_hexadecimal(x,4);

      Delay_ms(200);
    }
}
