/* ========================================================================== */
/*	           Exp07_2.c : PWM Output by OC1A Pin         	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

void LCD_4d(unsigned int number)                /* display 4-digit decimal number */
{ unsigned int i;
  unsigned char flag;

  flag = 0;
  i = number/1000;                              // 10^3
  if(i == 0) LCD_data(' ');
  else {     LCD_data(i + '0');
             flag = 1;
       }

  number = number % 1000;                       // 10^2
  i = number/100;
  if((i == 0) && (flag == 0))
             LCD_data(' ');
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
{ unsigned char i;
  unsigned int duty;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  LCD_string(0x80,"Period =  1250*2");          // display title
  LCD_string(0xC0,"Width  =  0000*2");

  TCCR1A = 0x82;                                // Phase Correct PWM mode(10)
  TCCR1B = 0x13;
  TCCR1C = 0x00;
  ICR1 = 1250;                                  // f(PWM) = 16MHz/64/2/1250 = 100Hz

  while(1)
    { for(i=0,duty=0; i<6; i++, duty += 250)
        { OCR1A = duty;                         // set duty ratio = duty/1250
          LCD_command(0xCA);                    // display duty
          LCD_4d(duty);
          Beep();
          Delay_ms(5000);
        }
    }
}
