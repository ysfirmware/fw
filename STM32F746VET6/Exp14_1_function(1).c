/* ============================================================================ */
/*		Exp14_1_function(1).c : D/A 컨버터의 수학함수 파형 출력		*/
/* ============================================================================ */
/*					programmed by Ho-Jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned short x, y, i, j;
  float a;
  unsigned short DAC_data[200];

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp14_1.c    ");

  TFT_string(3,5,White,Magenta," D/A 컨버터의 수학함수 파형 출력 ");

  GPIOA->MODER |= 0x00000300;			// PA4 = DAC_OUT1
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock

  DAC->CR = 0x00010001;				// DAC1/2 enable, output buffer and trigger disable

  while(1)
    { TFT_string(11,15,White,Black,"TP3(DAC1_OUT) 단자");
      TFT_string( 4,18,Green,Black,"약 500Hz의 삼각파 출력 (0~+3.3V)");
      Beep();
      a = 4095./100.;				// calculate data of triangular wave
      for(x = 0; x <= 100; x++)
        { y = (unsigned short)(a * x);
          DAC_data[x] = y;
        }
      i = 1;
      for(x = 101; x < 200; x++)
        { DAC_data[x] = DAC_data[100-i];
          i++;
        }
   
      for(i = 0; i < 2500; i++)			// display for 5 sec
        { for(j = 0; j < 200; j++)
          { DAC->DHR12R1 = DAC_data[j];		// data for channel 1
            Delay_us(10);
          }
        }

      TFT_string(11,15,White,Black,"TP3(DAC1_OUT) 단자");
      TFT_string( 4,18,Green,Black,"약 250Hz의 정현파 출력 (0~+3.3V)");
      Beep();

      for(x = 0; x < 200; x++)			// calculate data of sinusoidal wave
        DAC_data[x] = (signed short)(2047. * sin(x * 1.8 * 3.141592 / 180.)) + 2047;

      for(i = 0; i < 1250; i++)			// about 5 sec
        { for(j = 0; j < 200; j++)
            { DAC->DHR12R1 = DAC_data[j];	// output data
              Delay_us(20);
            }
        }
    }
}
