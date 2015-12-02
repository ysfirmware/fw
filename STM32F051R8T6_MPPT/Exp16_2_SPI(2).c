/* ============================================================================ */
/*	Exp16_2_SPI(2).c : 8비트 SPI 전송 포맷을 사용한 터치 스크린 입력	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void Initialize_SPI1_8bit(void);		/* initialize touch screen using 8-bit SPI1 */
unsigned short Read_ADS7846_8bit(U08 command);	/* read X, Y value from ADS7846 using 8-bit SPI1 */
void Input_touch_screen(void);			/* touch screen input of X, Y value from ADS7846 */

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Initialize_SPI1_8bit();			// initialize touch screen using 8-bit SPI1

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp16_2.c    ");
  Beep();

  Rectangle(0,0, 319,239, Green);		// display title
  Rectangle(1,1, 318,238, Green);

  TFT_string( 1,5,White,Magenta, "8-비트 SPI 통신에 의한 터치스크린 입력");

  TFT_string( 1, 1, Cyan,   Black, "(0,0)");
  TFT_string(32, 1, Cyan,   Black, "(319,0)");
  TFT_string( 1,27, Cyan,   Black, "(0,239)");
  TFT_string(30,27, Cyan,   Black, "(319,239)");

  TFT_string( 9, 9, Cyan,   Black, " 원시 데이터");
  TFT_string(21, 9, Magenta,Black, " x = 0000 ");
  TFT_string(21,11, Magenta,Black, " y = 0000 ");
  TFT_string( 9,15, Cyan,   Black, " 픽셀 데이터");
  TFT_string(21,15, Magenta,Black, " x =  000 ");
  TFT_string(21,17, Magenta,Black, " y =  000 ");
  TFT_string( 8,22, White,   Blue, " 비프음 ");
  TFT_string(20,22, White,    Red, " LED ON/OFF ");
  Rectangle(61,173, 130,194, White);
  Rectangle(157,173, 258,194, White);

  while(1)
    { Input_touch_screen();			// input touch screen

      TFT_color(Cyan,Black);
      TFT_xy(26,9);				// display 12-bit original X data
      TFT_unsigned_decimal(x_12bit,0,4);
      TFT_xy(26,11);				// display 12-bit original Y data
      TFT_unsigned_decimal(y_12bit,0,4);
      TFT_xy(27,15);				// display pixel X data
      TFT_unsigned_decimal(x_touch,0,3);
      TFT_xy(27,17);				// display pixel Y data
      TFT_unsigned_decimal(y_touch,0,3);

      if((x_touch >= 61) && (x_touch <= 130) && (y_touch >= 173) && (y_touch <= 194))
        { Beep();
          Delay_ms(200);
        }
      else if((x_touch >= 157) && (x_touch <= 258) && (y_touch >= 173) && (y_touch <= 194))
        { Beep();
          GPIOA->ODR ^= 0x00000800;
 	  Delay_ms(200);
        }
    }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

void Initialize_SPI1_8bit(void)		/* initialize touch screen using 8-bit SPI1 */
{
  GPIOA->MODER &= 0x3FFFFFFF;			// PA15(-TS_INT) = input
  GPIOB->MODER &= 0xFFFFF03C;			// PB0(-TS_CS) = output
  GPIOB->MODER |= 0x00000A81;			// PB5(TS_MOSI),PB4(TS_MISO),PB3(TS_CLK) = alternate function
  GPIOB->BSRR = 0x00000001;			// -TS_CS = 1
  GPIOB->AFR[0] &= 0xFF000FFF;			// PB5 = SPI1_MOSI, PB4 = SPI1_MISO, PB3 = SPI1_CLK

  RCC->AHBENR |= 0x00060000;			// enable clock of port A,B
  RCC->APB2ENR |= 0x00001000;			// enable SPI1 clock

  SPI1->CR1 = 0x0364;				// master mode, 48MHz/32 = 1.5MHz, CPOL = CPHA = 0
  SPI1->CR2 = 0x1700;				// 8-bit data, disable SS output

  Read_ADS7846_8bit(ADS7846_CMD_X);		// dummy input for -TS_INT signal
}

unsigned short Read_ADS7846_8bit(U08 command)	/* read X, Y value from ADS7846 using 8-bit SPI1 */
{
  unsigned int address;
  unsigned short axis;

  address = (uint32_t)SPI1 + 0x0C;

  GPIOB->BRR = 0x00000001;			// -TS_CS = 0
  *(__IO uint8_t *)address = command;
  while(!(SPI1->SR & 0x0001));
  axis = SPI1->DR;
  Delay_us(1);
  *(__IO uint8_t *)address = 0x00;
  while(!(SPI1->SR & 0x0001));
  axis = SPI1->DR & 0x00FF;
  axis <<= 8;
  Delay_us(1);
  *(__IO uint8_t *)address = 0x00;
  while(!(SPI1->SR & 0x0001));
  axis += (SPI1->DR & 0x00FF);
  GPIOB->BSRR = 0x00000001;			// -TS_CS = 1

  axis >>= 3;
  return axis;
}

void Input_touch_screen(void)			/* touch screen input of X, Y value from ADS7846 */
{
  unsigned char i;
  unsigned short temp;

  x_12bit = 0;					// initial value
  y_12bit = 0;

  for(i = 0; i < 16; i++)			// read X, Y value by 16 times if -PENIRQ enable
    { if((GPIOA->IDR & 0x00008000) == 0)	// -TS_INT = 0 ?
        x_12bit += Read_ADS7846_8bit(ADS7846_CMD_X);	// if yes, measure X position
      else					// if not, return with 0
        { x_12bit = 0;
	  y_12bit = 0;
	  break;
	}
      Delay_us(5);

      if((GPIOA->IDR & 0x00008000) == 0)	// -TS_INT = 0 ?
        y_12bit += Read_ADS7846_8bit(ADS7846_CMD_Y);	// if yes, measure Y position
      else					// if not, return with 0
        { x_12bit = 0;
	  y_12bit = 0;
	  break;
	}
      Delay_us(5);
    }

  x_12bit >>= 4;				// calculate average for 16 times
  y_12bit >>= 4;

  if((x_12bit == 0) && (y_12bit == 0))
    { x_touch = 0;
      y_touch = 0;
      return;
    }

  if(x_12bit <= x_touch_min)			// convert to pixel x-axis value
    x_touch = 0;
  else if(x_12bit >= x_touch_max)
    x_touch = 239;
  else
    x_touch = (unsigned int)((float)(x_12bit - x_touch_min) * 239./(float)(x_touch_max - x_touch_min));

  if(y_12bit <= y_touch_min)			// convert to pixel y-axis value
    y_touch = 319;
  else if(y_12bit >= y_touch_max)
    y_touch = 0;
  else
    y_touch = 319 - (unsigned int)((float)(y_12bit - y_touch_min) * 319./(float)(y_touch_max - y_touch_min));

  if(ScreenMode == 'L')				// adjust axis value for landscape mode
    { temp = x_12bit;				// exchange x_12bit and y_12bit
      x_12bit = y_12bit;
      y_12bit = temp;
      temp = x_touch;				// exchange x_touch and y_touch
      x_touch = y_touch;
      y_touch = 239 - temp;
    }
}
