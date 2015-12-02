/* ============================================================================ */
/*	Exp15_2_polling.c : RS-232C 직렬통신에서 폴링을 사용한 송수신		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

unsigned char RX1_char(void);			/* receive a character by USART1 */
void TX1_char(U08 data);			/* transmit a character by USART1 */
void TX1_string(U08 *string);			/* transmit a string by USART1 */

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned char RXD, count;
  unsigned short color;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp15_2.c    ");

  TFT_string( 2, 4,White,Magenta," RS-232C 직렬통신(폴링 방식 송수신) ");
  TFT_string(17, 8,White,Blue," 송신 ");
  TFT_string( 9,11,White,Black,"KEY1~KEY4를 누르시오 !");
  TFT_string(17,16,White,Blue," 수신 ");
  Line(0,156,319,156,White);
  Line(0,212,319,212,White);

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

  count = 0;					// initialize received data display
  color = Cyan;
  TFT_xy(0,20);
  TFT_color(color,Black);

  while(1)
    { switch(Key_input())                       // key input
        { case KEY1 : TX1_string("  KEY1 is OK !  ");
                      TX1_char(0x0D); TX1_char(0x0A);
                      break;
          case KEY2 : TX1_string("  KEY2 is OK !  ");
                      TX1_char(0x0D); TX1_char(0x0A);
                      break;
          case KEY3 : TX1_string("  KEY3 is OK !  ");
                      TX1_char(0x0D); TX1_char(0x0A);
                      break;
          case KEY4 : TX1_string("  KEY4 is OK !  ");
                      TX1_char(0x0D); TX1_char(0x0A);
                      break;
          default:    break;
        }

      RXD = RX1_char();				// receive data if any
      if((RXD >= 0x20) && (RXD <= 0x7F))
        { TFT_English(RXD);			// display a character
          count++;				// 120 character OK ?
          if(count >= 120)			// if yes, go home
            { count = 0;
	      TFT_xy(0,20);
              color ^= 0xFFFF;
              TFT_color(color,Black);
            }
        }
    }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

unsigned char RX1_char(void)			/* receive a character by USART1 */
{
  if(USART1->ISR & 0x00000020)   		// if RXNE=1, return with a received character
    return USART1->RDR;
  else						// if RXNE=0, return with 0x00
    return 0x00;
}

void TX1_char(U08 data)				/* transmit a character by USART1 */
{
  while(!(USART1->ISR & 0x00000080));		// wait until TXE = 1
  USART1->TDR = data;
}

void TX1_string(U08 *string)			/* transmit a string by USART1 */
{
  while(*string != '\0')
    { TX1_char(*string);
       string++;
    }
}
