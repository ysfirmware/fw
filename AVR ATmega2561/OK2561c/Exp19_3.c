/* ========================================================================== */
/*              Exp19_3.c : Digital Clock on Graphic LCD Module               */
/* ========================================================================== */
/*                        Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"
#include "GLCD2561.h"

void GLCD_2hex(unsigned char number)            /* display 2-digit hex number */
{ unsigned char i;

  i = (number >> 4) & 0x0F;                     // 16^1
  if(i <= 9) GLCD_English(i + '0');
  else       GLCD_English(i - 10 + 'A');

  i = number & 0x0F;                            // 16^0
  if(i <= 9) GLCD_English(i + '0');
  else       GLCD_English(i - 10 + 'A');
}

void GLCD_2d1(float number)		        /* floating-point number xx.x */
{ unsigned int i, j;

  j = (int)(number*10. + 0.5);
  i = j / 100;					// 10^1
  if(i == 0) GLCD_English(' ');
  else       GLCD_English(i + '0');

  j = j % 100;					// 10^0
  i = j / 10;
  GLCD_English(i + '0');
  GLCD_English('.');

  i = j % 10;					// 10^-1
  GLCD_English(i + '0');
}

int main(void)
{ unsigned char i, x;
  unsigned int sum;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  GLCD_clear();                                 // initialize GLCD screen
  cursor_flag = 0;                              // cursor off

  LCD_string(0x80,"Graphic LCD Test");          // display title on text LCD
  LCD_string(0xC0," English/Korean ");
  Beep();

  GLCD_string(0,0,"2006년 10월 01일");          // display title
  GLCD_string(1,0," 오전  12:00:00 ");
  GLCD_string(2,0,"    (일요일)    ");
  GLCD_string(3,0,"현재기온  00.0도");

  while(1)
    { for(i=0; i<10; i++)
        { GLCD_xy(0,0);                         // display year
          GLCD_2hex(0x20);
          GLCD_2hex(RTC_YEAR);
          GLCD_xy(0,7);                         // display month
          GLCD_2hex(RTC_MONTH);
          GLCD_xy(0,12);                        // display date
          GLCD_2hex(RTC_DATE);
          x = RTC_HOUR;                         // display AM/PM
          if(x<0x80) GLCD_string(1,3,"전");
          else       GLCD_string(1,3,"후");
          GLCD_xy(1,7);                         // display hour
          GLCD_2hex(x & 0x7F);
          GLCD_xy(1,10);                        // display minute
          GLCD_2hex(RTC_MINUTE);
          GLCD_xy(1,13);                        // display second
          GLCD_2hex(RTC_SECOND);
          x = RTC_WEEKDAY;                      // display weekday
          if(x == 1)      GLCD_string(2,5,"일");
          else if(x == 2) GLCD_string(2,5,"월");
          else if(x == 3) GLCD_string(2,5,"화");
          else if(x == 4) GLCD_string(2,5,"수");
          else if(x == 5) GLCD_string(2,5,"목");
          else if(x == 6) GLCD_string(2,5,"금");
          else            GLCD_string(2,5,"토");
          Delay_ms(100);
        }

      ADCSRA = 0x85;                            // ADC enable, 500kHz
      ADCSRB = 0x00;
      ADMUX = 0xC1;                             // select ADC1 with 2.56V
      Delay_us(150);

      sum = 0;
      for(i=0; i<16; i++)                       // read ADC by 16 times
        { ADCSRA = 0xD5;                        // start conversion and clear ADIF
          while((ADCSRA & 0x10) != 0x10);
          sum += ADCW;                          // add A/D result 16 times
          Delay_ms(1);
        }

      sum = sum >> 4;                           // divide sum by 16

      GLCD_xy(3,10);                            // display temperature(XX.X)
      GLCD_2d1((float)sum*256./1024.);
    }
}
