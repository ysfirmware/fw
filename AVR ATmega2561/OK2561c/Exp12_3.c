/* ========================================================================== */
/*	    Exp12_3.c : RS-232C Communication using USART0 Interrupt	      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK2561.h"

unsigned char cursor;

ISR(USART0_RX_vect)				/* UART0 interrupt function */
{
  LCD_data(UDR0);                               // display a character
  cursor++;                                     // 16 character OK ?
  if(cursor == 16)
    { LCD_command(0xC0);                        // if yes, go home
      cursor = 0;
      Beep();
    }
}

void TX0_char(unsigned char data)		/* transmit a character by USART0 */
{
  while((UCSR0A & 0x20) == 0x00);		// data register empty ?
  UDR0 = data;
}

void TX0_string(char *string)			/* transmit a string by USART0 */
{
  while(*string != '\0')
    { TX0_char(*string);
       string++;
    }
}

int main(void)
{ unsigned char RXD;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  UBRR0 = 51;                                   // 19200 bps
  UCSR0A = 0x00;                                // asynchronous normal mode
  UCSR0B = 0x98;                                // interrupt enable, 8 data
  UCSR0C = 0x06;                                // no parity, 1 stop, 8 data
  RXD = UDR0;                                   // dummy read
  sei();                                        // global interrupt enable

  LCD_string(0x80,"RS-232C (USART0)");          // display title
  LCD_string(0xC0,"                ");

  LCD_command(0x0F);                            // cursor ON
  LCD_command(0xC0);                            // initial cursor position
  cursor = 0;

  while(1)
    { switch(Key_input())                       // key input
        { case 0xE0 : PORTB = 0x10;             // KEY1 ?
                      TX0_string("  KEY1 is OK !  ");
                      TX0_char(0x0D); TX0_char(0x0A);
                      break;
          case 0xD0 : PORTB = 0x20;             // KEY2 ?
                      TX0_string("  KEY2 is OK !  ");
                      TX0_char(0x0D); TX0_char(0x0A);
                      break;
          case 0xB0 : PORTB = 0x40;             // KEY3 ?
                      TX0_string("  KEY3 is OK !  ");
                      TX0_char(0x0D); TX0_char(0x0A);
                      break;
          case 0x70 : PORTB = 0x80;             // KEY4 ?
                      TX0_string("  KEY4 is OK !  ");
                      TX0_char(0x0D); TX0_char(0x0A);
                      break;
          default:    break;
        }
    }
}
