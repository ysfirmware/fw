/* ========================================================================== */
/*	           Exp07_1.c : PWM Output by OC0A Pin         	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

void LCD_3d(unsigned char number)               /* display 3-digit decimal number */
{ unsigned char i, flag;

  flag = 0;
  i = number/100;                               // 10^2
  if(i == 0) LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  number = number % 100;                        // 10^1
  i = number/10;
  if((i == 0) && (flag == 0))
             LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  i = number % 10;                              // 10^0
  LCD_data(i + '0');
}

int main(void)
{ unsigned char i, duty;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  LCD_string(0x80," Period = 255*2 ");          // display title
  LCD_string(0xC0," Width  = 000*2 ");

  TCCR0A = 0x81;                                // Phase Correct PWM mode(1)
  TCCR0B = 0x03;                                // f(PWM) = 16MHz/64/510 = 490Hz

  while(1)
    { for(i=0,duty=0; i<6; i++, duty += 51)
        { OCR0A = duty;                         // set duty ratio = duty*2/510
          LCD_command(0xCA);                    // display duty
          LCD_3d(duty);
          Beep();
          Delay_ms(5000);
        }
    }
}
