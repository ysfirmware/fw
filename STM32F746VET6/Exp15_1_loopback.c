/* ============================================================================ */
/*		Exp15_1_loopback.c : RS-232C 직렬통신 루프백 테스트		*/
/* ============================================================================ */
/*					programmed by Kyung-Hwan Kim in 2016.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  unsigned char i, test_flag, RXD;
  unsigned char string[24] = { "OK-STM32F7 Loopback Test" };

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp15_1.c    ");

  TFT_string(4,4,White,Magenta," RS-232C 직렬통신 루프백 테스트 ");

  TFT_string(1, 9,White,Black,"CN5 컨넥터에 RS-232C 케이블을 접속하고");
  TFT_string(6,12,White,Black,"2번핀과 3번핀을 연결하시오 !");
  TFT_string(4,18,Cyan, Black,"송신 : ");
  TFT_string(4,21,Cyan, Black,"수신 : ");
  Delay_ms(1000);

  GPIOB->MODER  = (GPIOB->MODER & 0xFF0FFFFF) | 0x00A00000; // PB11, PB10 = alternate function
  GPIOB->AFR[1] = (GPIOA->AFR[1] & 0xFFFF00FF) | 0x00007700;
  RCC->APB1ENR |= 0x00040000;			// enable USART3 clock
  
  USART3->CR1 = 0x00000000;                     // 1 start bit, 8 data bit, oversampling = 16
  USART3->BRR = 2812;		                // 19200 bps = 54MHz/2812
  USART3->CR2 = 0x00000000;			// asynchronous mode(CLKEN=0), 1 stop bit
  USART3->CR3 = 0x00000000;			// 3 sampling bit
  USART3->CR1 = 0x0000000D;			// enable USART3, transmitter, receiver
  Delay_ms(1);
  RXD = USART3->RDR;		                // dummy read

  while(1)
    { test_flag = 0;
      for(i = 0; i < 24; i++)
        { USART3->TDR = string[i];		// transmit test data
          TFT_xy(11+i,18);			// display transmitted test data
          TFT_color(Green,Black);
          TFT_English(string[i]);
          Delay_ms(5);

          if(USART3->ISR & 0x00000020) RXD = USART3->RDR; // receive test data
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
