/* ============================================================================ */
/*	Exp02_2_register.c : 몇가지 시스템 레지스터 및 I/O 레지스터 값의 확인	*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned int x;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Beep();					// beep

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title
  LCD_string(0xC0,"   Exp02_2.c    ");

  TFT_string(0, 1, White,Magenta,"  시스템 제어 블록(SCB)의  레지스터 값  ");
  TFT_string(0, 4, White,  Black," CPUID = 0x00000000    CCR = 0x00000000 ");
  TFT_string(0, 6, White,  Black," CPACR = 0x00000000  MVFR0 = 0x00000000 ");
  TFT_string(0, 9, White,   Blue,"   플래시 메모리 제어기의 레지스터 값   ");
  TFT_string(5,12,Yellow,  Black,"    FLASH_ACR = 0x00000000    ");
  TFT_string(5,14,Yellow,  Black,"  FLASH_OPTCR = 0x00000000    ");
  TFT_string(5,16,Yellow,  Black," FLASH_OPTCR1 = 0x00000000    ");
  TFT_string(0,19, White,   Blue,"          I/O 제어 레지스터 값          ");
  TFT_string(5,22,Yellow,  Black,"      PWR_CR1 = 0x00000000    ");
  TFT_string(5,24,Yellow,  Black,"     PWR_CSR1 = 0x00000000    ");
  TFT_string(5,26,Yellow,  Black,"      RCC_CSR = 0x00000000    ");
  TFT_string(5,28,Yellow,  Black,"    GPIOC_IDR = 0x00000000    ");

  while(1)
    {
      x = SCB->CPUID;				// SCB_CPUID
      TFT_xy(11,4);
      TFT_color(Cyan,Black);
      TFT_hexadecimal(x,8);
      x = SCB->CCR;				// SCB_CCR
      TFT_xy(31,4);
      TFT_hexadecimal(x,8);
      x = SCB->CPACR;				// SCB_CPACR
      TFT_xy(11,6);
      TFT_hexadecimal(x,8);
      x = SCB->MVFR0;				// SCB_MVFR0
      TFT_xy(31,6);
      TFT_hexadecimal(x,8);

      x = FLASH->ACR;				// FLASH_ACR
      TFT_xy(23,12);
      TFT_hexadecimal(x,8);
      x = FLASH->OPTCR;				// FLASH_OPTCR
      TFT_xy(23,14);
      TFT_hexadecimal(x,8);
      x = FLASH->OPTCR1;			// FLASH_OPTCR1
      TFT_xy(23,16);
      TFT_hexadecimal(x,8);

      x = PWR->CR1;				// PWR_CR1
      TFT_xy(23,22);
      TFT_hexadecimal(x,8);
      x = PWR->CSR1;				// PWR_CSR1
      TFT_xy(23,24);
      TFT_hexadecimal(x,8);
      x = RCC->CSR;				// RCC_CSR
      TFT_xy(23,26);
      TFT_hexadecimal(x,8);
      x = GPIOC->IDR;				// GPIOC_IDR
      TFT_xy(23,28);
      TFT_hexadecimal(x,8);

      Delay_ms(200);
    }
}
