/* ============================================================================ */
/*    Exp17_5_vlarge.c : DS3234를 이용한 매우 큰 글씨 디지털 시계 프로그그램	*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include "OK-STM746_large.h"
#include "OK-STM746_xlarge.h"

/* ---------------------------------------------------------------------------- */
/*		사용자 정의 함수 선언						*/
/* ---------------------------------------------------------------------------- */

void DS3234_initialize(void);			/* initialize DS3234 RTC */
unsigned char DS3234_read(U16 address);		/* read DS3234 */
void DS3234_write(U16 address, U08 value);	/* write DS3234 */
void Display_time(void);			/* display current time */
void Time_increment(void);			/* increment time */
void Time_decrement(void);			/* decrement time */
void Cursor_left(void);				/* go cursor left */
void Cursor_right(void);			/* go cursor right */

/* ---------------------------------------------------------------------------- */
/*		상수 및 변수 선언						*/
/* ---------------------------------------------------------------------------- */

unsigned char year, month, date, weekday, hour, minute, second, second_old;
unsigned char blink_count = 0, hour_flag = 0;

/* ---------------------------------------------------------------------------- */
/*		메인 프로그램							*/
/* ---------------------------------------------------------------------------- */

int main(void)
{
  unsigned int key = no_key;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  DS3234_initialize();				// initialize DS3234 RTC

  LCD_string(0x80," OK-STM746 V1.0 ");          // display title
  LCD_string(0xC0,"   Exp17_5.c    ");
  Beep();

  TFT_string( 0,1,Green,  Black,"****************************************");
  TFT_string( 0,3,Magenta,Black,"       OK-STM746 키트 디지털 시계       ");
  TFT_string( 0,5,Green,  Black,"****************************************");

  TFT_string_large(6,8,Cyan,Black,"2015/11/01 (일)");
  TFT_string_large(2,13,Cyan,Black,"A");
  TFT_string_large(2,16,Cyan,Black,"M");
  Rectangle(12,96, 307,157, Cyan);
  Rectangle(13,97, 306,156, Cyan);
  TFT_xy(5,13);
  TFT_color(White,Black);
  TFT_hexadecimal_vlarge(0,2);
  TFT_English_vlarge(':');
  TFT_hexadecimal_vlarge(0,2);
  TFT_English_vlarge(':');
  TFT_hexadecimal_vlarge(0,2);

  TFT_cursor(Magenta);				// display cursor
  Xcursor = 39;
  Ycursor = 17;

  TFT_string(3,25,White,Blue,"커서");		// display key function
  TFT_string(3,27,White,Blue," 좌 ");
  TFT_string(13,25,White,Blue,"커서");
  TFT_string(13,27,White,Blue," 우 ");
  TFT_string(23,25,White,Magenta," 값 ");
  TFT_string(23,27,White,Magenta,"증가");
  TFT_string(33,25,White,Magenta," 값 ");
  TFT_string(33,27,White,Magenta,"감소");

  Rectangle(18,194, 61,237, Magenta);
  Rectangle(19,195, 60,236, Magenta);
  Rectangle(98,194, 141,237, Magenta);
  Rectangle(99,195, 140,236, Magenta);
  Rectangle(178,194, 221,237, Magenta);
  Rectangle(179,195, 220,236, Magenta);
  Rectangle(258,194, 301,237, Magenta);
  Rectangle(259,195, 300,236, Magenta);

  while(1)
    { second = DS3234_read(0x00);		// new second ?
      if(second != second_old)			// if yes, read time
        { second_old = second;
	  minute = DS3234_read(0x01);
          hour = DS3234_read(0x02) & 0x3F;
          weekday = DS3234_read(0x03);
          date = DS3234_read(0x04);
          month = DS3234_read(0x05);
          year = DS3234_read(0x06);

          blink_count = 0;			// start colon blink count
        }

      if((minute == 0x00) && (second == 0x00) && (hour_flag == 0)) // 매시 정각 경보
        { hour_flag = 1;
	  Beep();
	}
      else if(second >= 0x01)
        hour_flag = 0;

      key = Key_input();			// key input
      switch(key)
        { case KEY1 : Cursor_left();		// if KEY1, go cursor left
                      break;
          case KEY2 : Cursor_right();		// if KEY2, go cursor right
                      break;
          case KEY3 : Time_increment();		// if KEY3, increment time
                      break;
          case KEY4 : Time_decrement();		// if KEY4, decrement time
                      break;
        }

      Display_time();				// display current time
      if(blink_count < 50)			// display colon for 0.5 sec
	{ TFT_color(Cyan,Black);
	  TFT_xy(13,13);
	  TFT_English_vlarge(':');
          TFT_xy(25,13);
	  TFT_English_vlarge(':');
        }
      else
	{ TFT_color(Cyan,Black);
          TFT_xy(13,13);
          TFT_English_vlarge(' ');
          TFT_xy(25,13);
          TFT_English_vlarge(' ');
        }

      Delay_ms(8);				// delay count for blink time
      blink_count++;
    }
}

/* ---------------------------------------------------------------------------- */
/*		사용자 정의 함수						*/
/* ---------------------------------------------------------------------------- */

void DS3234_initialize(void)			/* initialize DS3234 RTC */
{
  GPIOB->MODER &= 0x03FFFFFF;			// alternate function
  GPIOB->MODER |= 0xA8000000;
  GPIOB->AFR[1] &= 0x000FFFFF;			// PB15 = SPI2_MOSI, PB14 = SPI2_MISO, PB13 = SPI2_SCK
  GPIOB->AFR[1] |= 0x55500000;

  GPIOE->MODER &= 0xFFFFCFFF;			// PE6 = -RTC_CS = 1
  GPIOE->MODER |= 0x00001000;
  GPIOE->BSRR = 0x00000040;

  RCC->AHB1ENR |= 0x00000012;			// enable clock of port B, E
  RCC->APB1ENR |= 0x00004000;			// enable SPI2 clock

  SPI2->CR1 = 0x0B67;				// master mode, 54MHz/32 = 1.6875MHz (Max 4MHz)
  SPI2->CR2 = 0x0F00;				// 16-bit data, disable SS output, CPOL = CPHA = 1

  second =  DS3234_read(0x00);			// if not second, initialize
  minute =  DS3234_read(0x01);			// if not minute, initialize
  hour =  DS3234_read(0x02);			// if not 12 hour system, initialize
  weekday =  DS3234_read(0x03);			// if not weekday, initialize
  date =  DS3234_read(0x04);			// if not date, initialize
  month =  DS3234_read(0x05);			// if not month, initialize
  year =  DS3234_read(0x06);			// if not year, initialize

  if((second > 0x59) || (minute > 0x59) || (hour < 0x40) || (weekday > 0x07) || (date > 0x31) || (month > 0x12) || (year > 0x99))
    { DS3234_write(0x00, 0x00);			// second = 0x00
      DS3234_write(0x01, 0x00);			// minute = 0x00
      DS3234_write(0x02, 0x12 + 0x40);		// hour = 0x12 AM
      DS3234_write(0x03, 0x01);			// weekday = 0x01(Sunday)
      DS3234_write(0x04, 0x01);			// date = 0x01
      DS3234_write(0x05, 0x11);			// month = 0x11
      DS3234_write(0x06, 0x15);			// year = 0x2015
      DS3234_write(0x0E, 0x00);			// control register
      DS3234_write(0x0F, 0x00);			// control register
    }
}

unsigned char DS3234_read(U16 address)		/* read DS3234 */
{
  unsigned short word;

  GPIOE->BSRR = 0x00400000;			// -RTS_CS = 0
  SPI2->DR = address << 8;
  while((SPI2->SR & 0x0003) != 0x0003);
  word = SPI2->DR;
  GPIOE->BSRR = 0x00000040;			// -RTS_CS = 1

  return word & 0x00FF;
}

void DS3234_write(U16 address, U08 value)	/* write DS3234 */
{
  unsigned short word = 0;

  GPIOE->BSRR = 0x00400000;			// -RTS_CS = 0
  SPI2->DR = ((0x80 + address) << 8) + value;
  while((SPI2->SR & 0x0003) != 0x0003);
  word |= SPI2->DR;
  GPIOE->BSRR = 0x00000040;			// -RTS_CS = 1
}

void Display_time(void)				/* display current time */
{
  TFT_color(Yellow,Black);			// display color
  TFT_xy(6,8);					// display year
  TFT_hexadecimal_large(0x20,2);
  TFT_hexadecimal_large(year,2);
  TFT_xy(11,8);		  			// display month
  TFT_hexadecimal_large(month,2);
  TFT_xy(14,8);					// display date
  TFT_hexadecimal_large(date,2);
  if(weekday == 0x01)      TFT_string_large(18,8, Yellow,Black, "일"); // display weekday
  else if(weekday == 0x02) TFT_string_large(18,8, Yellow,Black, "월");
  else if(weekday == 0x03) TFT_string_large(18,8, Yellow,Black, "화");
  else if(weekday == 0x04) TFT_string_large(18,8, Yellow,Black, "수");
  else if(weekday == 0x05) TFT_string_large(18,8, Yellow,Black, "목");
  else if(weekday == 0x06) TFT_string_large(18,8, Yellow,Black, "금");
  else if(weekday == 0x07) TFT_string_large(18,8, Yellow,Black, "토");

  if((hour & 0x20) == 0x00) TFT_string_large(2,13,Cyan,Black,"A"); // display AM/PM
  else                      TFT_string_large(2,13,Cyan,Black,"P");

  TFT_color(White,Black);			// display color
  TFT_xy(5,13);					// display hour
  TFT_hexadecimal_vlarge(hour & 0x1F,2);
  TFT_xy(17,13);				// display minute
  TFT_hexadecimal_vlarge(minute,2);
  TFT_xy(29,13);				// display second
  TFT_hexadecimal_vlarge(second,2);

  TFT_string(39,17, White,Black, " ");		// display home
}

void Cursor_left(void)			        /* go cursor left */
{
  if     ((Xcursor == 39) && (Ycursor == 17)) { Xcursor = 33; Ycursor = 13; }
  else if((Xcursor == 33) && (Ycursor == 13)) { Xcursor = 29; Ycursor = 13; }
  else if((Xcursor == 29) && (Ycursor == 13)) { Xcursor = 21; Ycursor = 13; }
  else if((Xcursor == 21) && (Ycursor == 13)) { Xcursor = 17; Ycursor = 13; }
  else if((Xcursor == 17) && (Ycursor == 13)) { Xcursor = 9;  Ycursor = 13; }
  else if((Xcursor == 9)  && (Ycursor == 13)) { Xcursor = 5;  Ycursor = 13; }
  else if((Xcursor == 5)  && (Ycursor == 13)) { Xcursor = 2;  Ycursor = 13; }
  else if((Xcursor == 2)  && (Ycursor == 13)) { Xcursor = 18; Ycursor = 8; }
  else if((Xcursor == 18) && (Ycursor == 8))  { Xcursor = 15; Ycursor = 8; }
  else if((Xcursor == 15) && (Ycursor == 8))  { Xcursor = 14; Ycursor = 8; }
  else if((Xcursor == 14) && (Ycursor == 8))  { Xcursor = 12; Ycursor = 8; }
  else if((Xcursor == 12) && (Ycursor == 8))  { Xcursor = 11; Ycursor = 8; }
  else if((Xcursor == 11) && (Ycursor == 8))  { Xcursor = 9;  Ycursor = 8; }
  else if((Xcursor == 9)  && (Ycursor == 8))  { Xcursor = 8;  Ycursor = 8; }
  else if((Xcursor == 8)  && (Ycursor == 8))  { Xcursor = 39; Ycursor = 17; }
}

void Cursor_right(void)				/* go cursor right */
{
  if     ((Xcursor == 39) && (Ycursor == 17)) { Xcursor = 8;  Ycursor = 8; }
  else if((Xcursor == 8)  && (Ycursor == 8))  { Xcursor = 9;  Ycursor = 8; }
  else if((Xcursor == 9)  && (Ycursor == 8))  { Xcursor = 11; Ycursor = 8; }
  else if((Xcursor == 11) && (Ycursor == 8))  { Xcursor = 12; Ycursor = 8; }
  else if((Xcursor == 12) && (Ycursor == 8))  { Xcursor = 14; Ycursor = 8; }
  else if((Xcursor == 14) && (Ycursor == 8))  { Xcursor = 15; Ycursor = 8; }
  else if((Xcursor == 15) && (Ycursor == 8))  { Xcursor = 18; Ycursor = 8; }
  else if((Xcursor == 18) && (Ycursor == 8))  { Xcursor = 2;  Ycursor = 13; }
  else if((Xcursor == 2)  && (Ycursor == 13)) { Xcursor = 5;  Ycursor = 13; }
  else if((Xcursor == 5)  && (Ycursor == 13)) { Xcursor = 9;  Ycursor = 13; }
  else if((Xcursor == 9)  && (Ycursor == 13)) { Xcursor = 17; Ycursor = 13; }
  else if((Xcursor == 17) && (Ycursor == 13)) { Xcursor = 21; Ycursor = 13; }
  else if((Xcursor == 21) && (Ycursor == 13)) { Xcursor = 29; Ycursor = 13; }
  else if((Xcursor == 29) && (Ycursor == 13)) { Xcursor = 33; Ycursor = 13; }
  else if((Xcursor == 33) && (Ycursor == 13)) { Xcursor = 39; Ycursor = 17; }
}

void Time_increment(void)			/* time increment */
{
  if((Xcursor == 8) && (Ycursor == 8))		// in case of year
    { year = DS3234_read(0x06);
      if((year & 0xF0) < 0x90) year += 0x10;
      else                     year &= 0x0F;
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 9) && (Ycursor == 8))	// in case of year
    { year = DS3234_read(0x06);
      if((year & 0x0F) < 0x09) year += 0x01;
      else                     year &= 0xF0;
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 11) && (Ycursor == 8))	// in case of month
    { month = DS3234_read(0x05);
      month ^= 0x10;
      if(month == 0x00) month += 0x01;
      if(month > 0x12)  month = 0x12;
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 12) && (Ycursor == 8))	// in case of month
    { month = DS3234_read(0x05);
      if((month & 0x0F) < 0x09)
        { month += 0x01;
	  if(month > 0x12) month &= 0xF0;
	}
      else
        { month &= 0xF0;
	  if(month == 0x00) month = 0x01;
        }
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 14) && (Ycursor == 8))	// in case of date
    { date = DS3234_read(0x04);
      if((date & 0xF0) < 0x30)
        { date += 0x10;
	  if(date > 0x31) date = 0x31;
        }
      else
        { date &= 0x0F;
	  if(date == 0x00) date = 0x01;
        }
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 15) && (Ycursor == 8))	// in case of date
    { date = DS3234_read(0x04);
      if((date & 0x0F) < 0x09)
        { date += 0x01;
	  if(date > 0x31) date &= 0xF0;
	}
      else
        { date &= 0xF0;
	  if(date == 0x00) date = 0x01;
        }
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 18) && (Ycursor == 8))	// in case of weekday
    { weekday = DS3234_read(0x03);
      if(weekday < 0x07) weekday += 0x01;
      else               weekday = 0x01;
      DS3234_write(0x03,weekday);
    }
  else if((Xcursor == 2) && (Ycursor == 13))	// in case of AM/PM
    { hour = DS3234_read(0x02);
      hour ^= 0x20;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 5) && (Ycursor == 13))	// in case of hour
    { hour = DS3234_read(0x02);
      hour ^= 0x10;
      if((hour & 0x1F) == 0x00) hour += 0x01;
      if((hour & 0x1F) > 0x12)  hour = (hour & 0x60) + 0x12;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 9) && (Ycursor == 13))	// in case of hour
    { hour = DS3234_read(0x02);
      if((hour & 0x0F) < 0x09)
        { hour += 0x01;
	  if((hour & 0x1F) > 0x12) hour &= 0xF0;
	}
      else
        { hour &= 0xF0;
	  if((hour & 0x1F) == 0x00) hour += 0x01;
        }
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 17) && (Ycursor == 13))	// in case of minute
    { minute = DS3234_read(0x01);
      if((minute & 0xF0) < 0x50) minute += 0x10;
      else                       minute &= 0x0F;
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 21) && (Ycursor == 13))	// in case of minute
    { minute = DS3234_read(0x01);
      if((minute & 0x0F) < 0x09) minute += 0x01;
      else                       minute &= 0xF0;
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 29) && (Ycursor == 13))	// in case of second
    { second = DS3234_read(0x00);
      if((second & 0xF0) < 0x50) second += 0x10;
      else                       second &= 0x0F;
      DS3234_write(0x00,second);
    }
  else if((Xcursor == 33) && (Ycursor == 13))	// in case of second
    { second = DS3234_read(0x00);
      if((second & 0x0F) < 0x09) second += 0x01;
      else                       second &= 0xF0;
      DS3234_write(0x00,second);
    }
}

void Time_decrement(void)			/* time decrement */
{
  if((Xcursor == 8) && (Ycursor == 8))		// in case of year
    { year = DS3234_read(0x06);
      if((year & 0xF0) >= 0x10) year -= 0x10;
      else                      year += 0x90;
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 9) && (Ycursor == 8))	// in case of year
    { year = DS3234_read(0x06);
      if((year & 0x0F) >= 0x01) year -= 0x01;
      else                      year += 0x09;
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 11) && (Ycursor == 8))	// in case of month
    { month = DS3234_read(0x05);
      month ^= 0x10;
      if(month == 0x00) month += 0x01;
      if(month > 0x12)  month = 0x12;
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 12) && (Ycursor == 8))	// in case of month
    { month = DS3234_read(0x05);
      if((month & 0x0F) >= 0x01)
        { month -= 0x01;
          if(month == 0x00) month = 0x09;
	}
      else
        { month += 0x09;
	  if(month > 0x12) month = 0x12;
        }
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 14) && (Ycursor == 8))	// in case of date
    { date = DS3234_read(0x04);
      if((date & 0xF0) >= 0x10)
        { date -= 0x10;
          if(date == 0x00) date = 0x01;
	}
      else
        { date += 0x30;
	  if(date > 0x31) date = 0x31;
        }
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 15) && (Ycursor == 8))	// in case of date
    { date = DS3234_read(0x04);
      if((date & 0x0F) >= 0x01)
        { date -= 0x01;
	  if(date == 0x00) date = 0x09;
	}
      else
        { date += 0x09;
	  if(date > 0x31) date = 0x31;
        }
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 18) && (Ycursor == 8))	// in case of weekday
    { weekday = DS3234_read(0x03);
      if(weekday > 0x01) weekday -= 0x01;
      else               weekday = 0x07;
      DS3234_write(0x03,weekday);
    }
  else if((Xcursor == 2) && (Ycursor == 13))	// in case of AM/PM
    { hour = DS3234_read(0x02);
      hour ^= 0x20;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 5) && (Ycursor == 13))	// in case of hour
    { hour = DS3234_read(0x02);
      hour ^= 0x10;
      if((hour & 0x1F) == 0x00) hour += 0x01;
      if((hour & 0x1F) > 0x12)  hour = (hour & 0x60) + 0x12;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 9) && (Ycursor == 13))	// in case of hour
    { hour = DS3234_read(0x02);
      if((hour & 0x0F) >= 0x01)
        { hour -= 0x01;
	  if((hour & 0x1F) == 0x00) hour = (hour & 0xE0) + 0x09;
	}
      else
        { hour += 0x09;
	  if((hour & 0x1F) > 0x12) hour = (hour & 0xE0) + 0x12;
        }
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 17) && (Ycursor == 13))	// in case of minute
    { minute = DS3234_read(0x01);
      if((minute & 0xF0) >= 0x10) minute -= 0x10;
      else                        minute += 0x50;
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 21) && (Ycursor == 13))	// in case of minute
    { minute = DS3234_read(0x01);
      if((minute & 0x0F) >= 0x01) minute -= 0x01;
      else                        minute += 0x09;
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 29) && (Ycursor == 13))	// in case of second
    { second = DS3234_read(0x00);
      if((second & 0xF0) >= 0x10) second -= 0x10;
      else                        second += 0x50;
      DS3234_write(0x00,second);
    }
  else if((Xcursor == 33) && (Ycursor == 13))	// in case of second
    { second = DS3234_read(0x00);
      if((second & 0x0F) >= 0x01) second -= 0x01;
      else                        second += 0x09;
      DS3234_write(0x00,second);
    }
}
