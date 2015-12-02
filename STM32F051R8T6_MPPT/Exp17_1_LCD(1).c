/* ============================================================================ */
/*	Exp17_1_LCD(1).c : 텍스트 LCD에 표시한 디지털 시계 프로그그램(1)	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void DS3234_initialize(void);			/* initialize DS3234 RTC */
unsigned char DS3234_read(U16 address);		/* read DS3234 */
void DS3234_write(U16 address, U08 value);	/* write DS3234 */
void Display_time(void);			/* display DS3234 time */

/* ----- 메인 프로그램 -------------------------------------------------------- */

unsigned char year, month, date, weekday, hour, minute, second;
unsigned char beep_flag = 0;

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  DS3234_initialize();                          // initialize DS3234
  Beep();

  LCD_string(0x80,"2013/09/01 (SUN)");		// display initial screen
  LCD_string(0xC0,"  12:00:00  AM  ");

  while(1)
    { Display_time();				// display time
      Delay_ms(50);
    }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

void DS3234_initialize(void)			/* initialize DS3234 RTC */
{
  GPIOB->MODER &= 0xFFFFF033;			// PB1(-RTC_CS) = output
  GPIOB->MODER |= 0x00000A84;			// PB5(TS_MOSI),PB4(TS_MISO),PB3(TS_CLK) = alternate function
  GPIOB->BSRR = 0x00000002;			// -RTC_CS = 1
  GPIOB->AFR[0] &= 0xFF000FFF;			// PB5 = SPI1_MOSI, PB4 = SPI1_MISO, PB3 = SPI1_CLK

  RCC->AHBENR |= 0x00040000;			// enable clock of port B
  RCC->APB2ENR |= 0x00001000;			// enable SPI1 clock

  SPI1->CR1 = 0x0367;				// master mode, 48MHz/32 = 1.5MHz, CPOL = CPHA = 1
  SPI1->CR2 = 0x0F00;				// 16-bit data, disable SS output

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
      DS3234_write(0x05, 0x09);			// month = 0x09
      DS3234_write(0x06, 0x13);			// year = 0x2013
      DS3234_write(0x0E, 0x00);			// control register
      DS3234_write(0x0F, 0x00);			// control and status register
    }
}

unsigned char DS3234_read(U16 address)		/* read DS3234 */
{
  unsigned short word;

  GPIOB->BRR = 0x00000002;			// -RTS_CS = 0
  SPI1->DR = address << 8;
  while(!(SPI1->SR & 0x0001));
  word = SPI1->DR;
  GPIOB->BSRR = 0x00000002;			// -RTS_CS = 1

  return word & 0x00FF;
}

void DS3234_write(U16 address, U08 value)	/* write DS3234 */
{
  unsigned short word = 0;

  GPIOB->BRR = 0x00000002;			// -RTS_CS = 0
  SPI1->DR = ((0x80 + address) << 8) + value;
  while(!(SPI1->SR & 0x0001));
  word |= SPI1->DR;
  GPIOB->BSRR = 0x00000002;			// -RTS_CS = 1
}

void Display_time(void)				/* display DS3234 time */
{
  LCD_command(0x80);				// display year
  LCD_hexadecimal(0x20,2);
  year = DS3234_read(0x06);
  LCD_hexadecimal(year,2);
  LCD_command(0x85);				// display month
  month = DS3234_read(0x05);
  LCD_hexadecimal(month,2);
  LCD_command(0x88);				// display date
  date = DS3234_read(0x04);
  LCD_hexadecimal(date,2);
  weekday = DS3234_read(0x03);			// display weekday
  if(weekday == 0x01)      LCD_string(0x8C,"SUN");
  else if(weekday == 0x02) LCD_string(0x8C,"MON");
  else if(weekday == 0x03) LCD_string(0x8C,"TUE");
  else if(weekday == 0x04) LCD_string(0x8C,"WED");
  else if(weekday == 0x05) LCD_string(0x8C,"THU");
  else if(weekday == 0x06) LCD_string(0x8C,"FRI");
  else if(weekday == 0x07) LCD_string(0x8C,"SAT");

  LCD_command(0xC2);				// display hour
  hour = DS3234_read(0x02);
  LCD_hexadecimal(hour & 0x1F,2);
  if((hour & 0x20) == 0x00) LCD_string(0xCC,"AM");
  else                      LCD_string(0xCC,"PM");
  LCD_command(0xC5);				// display minute
  minute = DS3234_read(0x01);
  LCD_hexadecimal(minute,2);
  LCD_command(0xC8);				// display second
  second = DS3234_read(0x00);
  LCD_hexadecimal(second & 0x7F,2);

  if((minute == 0x00) && (second == 0x00) && (beep_flag == 0))
    { beep_flag = 1;
      Beep();
    }
  else if(second != 0x00)
    beep_flag = 0;
}
