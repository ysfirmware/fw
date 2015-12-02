/* ============================================================================ */
/*		Exp15_1_loopback.c : RS-232C 직렬통신 루프백 테스트		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  unsigned char i, test_flag, RXD;
  unsigned char string[24] = { "OK-STM32F0 Loopback Test" };

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp15_1.c    ");

  TFT_string(4,4,White,Magenta," RS-232C 직렬통신 루프백 테스트 ");

  TFT_string(1, 9,White,Black,"CN5 컨넥터에 RS-232C 케이블을 접속하고");
  TFT_string(6,12,White,Black,"2번핀과 3번핀을 연결하시오 !");
  TFT_string(4,18,Cyan, Black,"송신 : ");
  TFT_string(4,21,Cyan, Black,"수신 : ");
  Delay_ms(1000);

  GPIOA->MODER  = (GPIOA->MODER & 0xFFC3FFFF) | 0x00280000; // PA10, PA9 = alternate function
  GPIOA->AFR[1] = (GPIOA->AFR[1] & 0xFFFFF00F) | 0x00000110;
  RCC->APB2ENR |= 0x00004000;			// enable USART1 clock
  RCC->CFGR3 &= 0xFFFFFFFC;			// USART1 clock = PCLK

  USART1->CR1 = 0x00000000;			// 1 start bit, 8 data bit, oversampling = 16
  USART1->BRR = 2500;				// 19200 bps = 48MHz/19200
  USART1->CR2 = 0x00000000;			// LSB first, 1 stop bit
  USART1->CR3 = 0x00000000;			// 3 sampling bit
  USART1->CR1 = 0x0000000D;			// enable USART1, transmitter, receiver
  Delay_ms(1);
  RXD = USART1->RDR;				// dummy read

  while(1)
    { test_flag = 0;
      for(i = 0; i < 24; i++)
        { USART1->TDR = string[i];		// transmit test data
          TFT_xy(11+i,18);			// display transmitted test data
          TFT_color(Green,Black);
          TFT_English(string[i]);
          Delay_ms(5);

          if(USART1->ISR & 0x00000020) RXD = USART1->RDR; // receive test data
          else                         RXD = ' ';
          TFT_xy(11+i,21);			// display transmitted test data
          TFT_color(Magenta,Black);
          TFT_English(RXD);

          if(RXD == string[i])			// data OK ?
            test_flag++;

          Beep();
          Delay_ms(300);
        }

      if(test_flag == 24)
        { TFT_string(15,25,  Green,Black,"통신 OK !");
          Beep();
        }
      else
        { TFT_string(14,25,Magenta,Black,"통신 에러 !");
          Beep_3times();
        }
      Delay_ms(2000);

      TFT_string(14,25,Magenta,Black,"           "); // clear message
      TFT_string(11,18,Magenta,Black,"                        ");
      TFT_string(11,21,Magenta,Black,"                        ");
    }
}
