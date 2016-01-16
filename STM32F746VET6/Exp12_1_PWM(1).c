/* ============================================================================ */
/*		Exp12_1_PWM(1).c : 1채널 PWM 출력 프로그램			*/
/* ============================================================================ */
/*					   programmed by Dong-Ki Kim in 2016.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned char key;
  unsigned int PWM;				// pulse width

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp12_1.c    ");
  Beep();

  TFT_string(5, 5, White,Magenta, " 타이머 TIM1의 PWM 출력 모드 ");
  TFT_string(6,10, Cyan,   Black, "TIM1_CH4 : 주기   = 1000 ms");
  TFT_string(6,13, Cyan,   Black, " (OC4)     펄스폭 =  500 ms");
  TFT_string(6,16, Cyan,   Black, "           듀티비 =   50 %");
  TFT_string(4,24, White,  Black, "KEY3/KEY4로 듀티비를 가변하시오.");

  PWM = 4999;					// initialize PWM = 4999 (duty ratio 50%)

  GPIOE->MODER &= 0xCFFFFFFF;			// PE14 = alternate function mode
  GPIOE->MODER |= 0x20000000;
  GPIOE->AFR[1] |= 0x01000000;			// PE14 = TIM1_CH4

  RCC->APB2ENR |= 0x00000001;			// enable TIM1 clock
  RCC->DCKCFGR1 |= 0x01000000;                  // TIM1CLK = HCLK
  TIM1->PSC = 21599;				// 216MHz/(21599+1) = 10kHz
  TIM1->ARR = 9999;				// 1kHz / (9999+1) = 1Hz
  TIM1->CCR4 = PWM;
  TIM1->CNT = 0;				// clear counter
  TIM1->CCMR2 = 0x00006C00;			// OC4M = 110(PWM mode 1), CC4S = 00(output)
  TIM1->CCER = 0x00001000;			// CC4E = 1(enable OC4 output)
  TIM1->BDTR = 0x00008000;			// MOE = 1
  TIM1->CR1 = 0x0005;				// edge-aligned, up-counter, enable TIM1

  while(1)
    { key = Key_input();			// key input
      if((key == KEY3) && (PWM != 8999))	// if KEY3, increment duty ratio by 10%
        { PWM += 1000;
          TIM1->CCR4 = PWM;
          TFT_color(Yellow,Black);
          TFT_xy(26,13);
	  TFT_unsigned_decimal((PWM+1)/10,0,4);
          TFT_xy(27,16);
	  TFT_unsigned_decimal((PWM+1)/100,0,3);
        }
      else if((key == KEY4) && (PWM != 999))	// if KEY4, decrement duty ratio by 10%
        { PWM -= 1000;
          TIM1->CCR4 = PWM;
          TFT_color(Yellow,Black);
          TFT_xy(26,13);
	  TFT_unsigned_decimal((PWM+1)/10,0,4);
          TFT_xy(27,16);
	  TFT_unsigned_decimal((PWM+1)/100,0,3);
	}
    }
}
