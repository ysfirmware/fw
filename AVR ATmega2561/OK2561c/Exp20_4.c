/* ========================================================================== */
/*	            Exp20_4.c : printf() Function to USART0	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <stdio.h>
#include "OK2561.h"

static int USART0_putchar(char c, FILE *stream);
static FILE device = FDEV_SETUP_STREAM(USART0_putchar, NULL, _FDEV_SETUP_WRITE);

void USART0_initialize(void)		        /* initialize USART0 */
{
  UBRR0 = 51;                                   // 19200 bps
  UCSR0A = 0x00;                                // asynchronous normal mode
  UCSR0B = 0x18;                                // Rx/Tx enable, 8 data
  UCSR0C = 0x06;                                // no parity, 1 stop, 8 data
}

static int USART0_putchar(char c, FILE *stream)	/* print a character to USART0 */
{
  if(c == '\n')                                 // process CR(carriage return)
    { loop_until_bit_is_set(UCSR0A,UDRE0);
      UDR0 = 0x0D;
      loop_until_bit_is_set(UCSR0A,UDRE0);
      UDR0 = 0x0A;
    }
  else
    { loop_until_bit_is_set(UCSR0A,UDRE0);	// Tx ready ?
      UDR0 = c;					// if yes, print
    }
  return 0;
}

int main(void)
{ unsigned char i = 1;
  double x = 0.001;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  USART0_initialize();                          // initialize USART0

  LCD_string(0x80,"printf to USART0");          // display title
  LCD_string(0xC0,"Press KEY4-KEY1!");

  stdout = &device;				// set USART0 to stdout

  while(1)
    { switch(Key_input())                       // key input
        { case 0xE0 : PORTB = 0x10;             // KEY1 ?
                      printf("KEY1 is OK !  i=%3d  x=%5.3f\n",i++,x);
                      x += 0.001;
                      break;
          case 0xD0 : PORTB = 0x20;             // KEY2 ?
                      printf("KEY2 is OK !  i=%3d  x=%5.3f\n",i++,x);
                      x += 0.001;
                      break;
          case 0xB0 : PORTB = 0x40;             // KEY3 ?
                      printf("KEY3 is OK !  i=%3d  x=%5.3f\n",i++,x);
                      x += 0.001;
                      break;
          case 0x70 : PORTB = 0x80;             // KEY4 ?
                      printf("KEY4 is OK !  i=%3d  x=%5.3f\n",i++,x);
                      x += 0.001;
                      break;
          default:    break;
        }
    }
}
