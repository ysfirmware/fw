/* ============================================================================ */
/*	Exp15_3_interrupt.c : RS-232C ������ſ��� ���ͷ�Ʈ�� ����� ����	*/
/* ============================================================================ */
/*					programmed by Kyung-Hwan Kim in 2016.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void USART3_IRQHandler(void);			/* USART3 interrupt function */
void TX1_char(U08 data);			/* transmit a character by USART1 */
void TX1_string(U08 *string);			/* transmit a string by USART1 */

/* ----- ���ͷ�Ʈ ó�� ���α׷� ----------------------------------------------- */

volatile unsigned char RXD, count;
volatile unsigned short color;

void USART3_IRQHandler(void)			/* USART3 interrupt function */
{
    if(USART3->ISR & 0x00000020)   		// if RXNE=1, receive a character
    { RXD = USART3->RDR;
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

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp15_3.c    ");

  TFT_string( 1, 4,White,Magenta," RS-232C �������(���ͷ�Ʈ ��� ����) ");
  TFT_string(17, 8,White,Blue," �۽� ");
  TFT_string( 9,11,White,Black,"KEY1~KEY4�� �����ÿ� !");
  TFT_string(17,16,White,Blue," ���� ");
  Line(0,156,319,156,White);
  Line(0,212,319,212,White);

  GPIOB->MODER  = (GPIOB->MODER & 0xFF0FFFFF) | 0x00A00000; // PB11, PB10 = alternate function
  GPIOB->AFR[1] = (GPIOA->AFR[1] & 0xFFFF00FF) | 0x00007700;
  RCC->APB1ENR |= 0x00040000;			// enable USART3 clock
  
  USART3->CR1 = 0x00000000;                     // 1 start bit, 8 data bit, oversampling = 16
  USART3->BRR = 2812;		                // 19200 bps = 54MHz/2812
  USART3->CR2 = 0x00000000;			// asynchronous mode(CLKEN=0), 1 stop bit
  USART3->CR3 = 0x00000000;			// 3 sampling bit
  USART3->CR1 = 0x0000002D;			// enable USART3, transmitter, receiver(RXNEIN=1)
  Delay_ms(1);
  RXD = USART3->RDR;		                // dummy read

  NVIC->ISER[1] = 0x00000080;			// enable (39)USART3 interrupt

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
    }
}

/* ----- ����� ���� �Լ� ----------------------------------------------------- */

void TX1_char(U08 data)				/* transmit a character by USART3 */
{
  while(!(USART3->ISR & 0x00000080));		// wait until TXE = 1
  USART3->TDR = data;
}

void TX1_string(U08 *string)			/* transmit a string by USART3 */
{
  while(*string != '\0')
    { TX1_char(*string);
       string++;
    }
}
