/* ========================================================================== */
/*	         Exp20_3.c : Watchdog Timer - Interrupt Mode                  */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "OK2561.h"

ISR(WDT_vect)					/* WDT interrupt function */
{
  Beep();
}

int main(void)
{ unsigned char i;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  LCD_string(0x80,"  OK-2561 V1.0  ");          // display booting by reset
  LCD_string(0xC0," Reset and Boot ");
  PORTE = 0x04;                                 // buzzer on
  PORTB = 0x10;                                 // LED1 on
  Delay_ms(200);
  PORTB = 0x20;                                 // LED2 on
  Delay_ms(200);
  PORTB = 0x40;                                 // LED3 on
  Delay_ms(200);
  PORTB = 0x80;                                 // LED4 on
  Delay_ms(200);
  PORTB = 0x00;                                 // all LED off
  PORTE = 0x00;                                 // buzzer off
  Delay_ms(1000);

  LCD_string(0x80,"Press KEY4/KEY1!");          // display message
  LCD_string(0xC0,"(KEY3/2=ON/OFF) ");
  PORTB = 0x20;                                 // LED2 on
  cli();

  while(1)
    { switch(Key_input())                       // key input
        { case 0xE0 : LCD_string(0x80,"   Blink LED1   ");      // KEY1
                      LCD_string(0xC0,"     100 ms     ");
                      for(i=0; i<20; i++)
                        { wdt_reset();
                          PORTB ^= 0x10;
                          Delay_ms(100);
                        }
                      LCD_string(0x80,"Press KEY4/KEY1!");
                      LCD_string(0xC0,"(KEY3/2=ON/OFF) ");
                      break;
          case 0x70 : LCD_string(0x80,"   Blink LED4   ");      // KEY4
                      LCD_string(0xC0,"     400 ms     ");
                      for(i=0; i<10; i++)
                        { wdt_reset();
                          PORTB ^= 0x80;
                          Delay_ms(400);
                        }
                      LCD_string(0x80,"Press KEY4/KEY1!");
                      LCD_string(0xC0,"(KEY3/2=ON/OFF) ");
                      break;
          case 0xD0 : WDTCSR = (1<<WDCE) | (1<<WDE);	// KEY2
                      WDTCSR = (0<<WDCE) | (0<<WDE);	//  (WDT off)
                      PORTB = 0x20;
                      cli();
                      break;
          case 0xB0 : WDTCSR = (1<<WDCE) | (1<<WDE);	// KEY3
                      WDTCSR = (1<<WDIE) | (1<<WDP2);	//  (WDT on, interrupt mode)
                      PORTB = 0x40;			//  (period = 0.25 sec)
                      sei();
                      break;
          default:    wdt_reset();
                      break;
        }
    }
}
