/* ============================================================================ */
/*	Exp07_5_realtime.c : VR1 전압을 실시간으로 A/D 변환하여 측정한 파형	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void SysTick_Handler(void);			/* SysTick interrupt with 20ms interval */
void Draw_axis(void);				/* draw x-axis and y-axis */
void TFT_English_pixel(U16 Xpixel,U16 Ypixel, U08 code); /* write a English ASCII character */

unsigned short x0, x, y0, y;
volatile unsigned short ADC_result, ADC_count, end_flag;
volatile unsigned char ADC_buffer[291];

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt with 20ms interval */
{
  ADC1->CHSELR = 0x00000004;			// channel 2 (VR1)
  Delay_us(10);
  ADC1->CR |= 0x00000004;			// start conversion by software
  while(!(ADC1->ISR & 0x00000004));		// wait for end of conversion

  x = ADC_count;
  ADC_buffer[x] = (unsigned short)((float)ADC1->DR*3.3*50./4095. + 0.5);

  if(ADC_count != 0)				// draw graph
    { x0 = ADC_count - 1;
      x = ADC_count;
      y0 = ADC_buffer[x0];
      y = ADC_buffer[x];
      Line(20+x0,215-y0, 20+x,215-y, Magenta);
    }

  if(ADC_count >= 290)				// screen end ?
    { end_flag = 1;
      ADC_count = 0;
    }
  else
    ADC_count++;
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned short i;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp07_5.c    ");
  Beep();

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3
  RCC->APB2ENR |= 0x00000200;			// enable ADC clock
  RCC->CFGR |= 0x00004000;			// ADC clock = PCLK/4 = 48MHz/4 = 12MHz
  RCC->CFGR3 |= 0x00000100;

  ADC1->CR = 0x00000001;			// enable ADC
  ADC1->SMPR = 0x00000005;			// sampling time = 55.5 cycle
  ADC1->CFGR1 = 0x00000000;			// 12-bit resolution, single conversion mode
  ADC1->CFGR2 = 0x80000000;			// PCLK/4 = 12MHz

  SysTick->LOAD  = 119999;			// N-1 for 1sec (6MHz/120000 = 50Hz)
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 48MHz/8 = 6MHz, SysTick enable and interrupt request

  while(1)
    { Draw_axis();				// draw x-axis and y-axis
      end_flag = 0;
      ADC_count = 0;

      while(end_flag == 0);			// wait screen graph end

      SysTick->CTRL  = 0x0000000;		// SysTick disable
      for(i = 1; i <= 290; i++)			// clear old graph
        { x0 = i - 1;
          x = i;
          y0 = ADC_buffer[x0];
          y = ADC_buffer[x];
          Line(20+x0,215-y0, 20+x,215-y, Black);
        }
      Beep();
      SysTick->CTRL  = 0x0000003;		// SysTick enable
    }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

void Draw_axis(void)				/* draw x-axis and y-axis */
{
  TFT_string(15,0, White,Blue, " VR1 전압 ");	// display title

  Line(15,215, 310,215, White);			// draw x-axis
  Line(305,210, 310,215, White);
  Line(305,220, 310,215, White);
  Line(70,212, 70,218, White);			// draw x-axis scale
  Line(120,212, 120,218, White);
  Line(170,212, 170,218, White);
  Line(220,212, 220,218, White);
  Line(270,212, 270,218, White);
  TFT_color(Cyan,Black);			// write time scale
  TFT_English_pixel(16,222, '0');
  TFT_English_pixel(66,222, '1');
  TFT_English_pixel(116,222, '2');
  TFT_English_pixel(166,222, '3');
  TFT_English_pixel(216,222, '4');
  TFT_English_pixel(266,222, '5');
  TFT_color(Magenta,Black);
  TFT_English_pixel(288,222, 's');
  TFT_English_pixel(296,222, 'e');
  TFT_English_pixel(304,222, 'c');

  Line(20,220, 20,5, White);			// draw y-axis
  Line(15,10, 20,5, White);
  Line(25,10, 20,5, White);
  Line(17,165, 23,165, White);			// draw y-axis scale
  Line(17,115, 23,115, White);
  Line(17,65, 23,65, White);
  Line(17,15, 23,15, White);
  TFT_color(Cyan,Black);			// write voltage scale
  TFT_English_pixel(5,208, '0');
  TFT_English_pixel(5,158, '1');
  TFT_English_pixel(5,108, '2');
  TFT_English_pixel(5,58, '3');
  TFT_color(Magenta,Black);
  TFT_English_pixel(5,8, 'V');
}

void TFT_English_pixel(U16 Xpixel,U16 Ypixel, U08 code)	/* write a English ASCII character */
{
  unsigned char data, x, y;
  unsigned int pixel[8][16];

  for(x = 0; x < 8; x++)			// read English ASCII font
    { data = E_font[code][x];
      for(y = 0; y < 8; y++)
        { if(data & 0x01) pixel[x][y] = foreground;
	  else            pixel[x][y] = background;
	  data = data >> 1;
	}
    }

  for(x = 0; x < 8; x++)
    { data = E_font[code][x+8];
      for(y = 0; y < 8; y++)
	{ if(data & 0x01) pixel[x][y+8] = foreground;
	  else            pixel[x][y+8] = background;
	  data = data >> 1;
	}
    }

  for(y = 0; y < 16; y++)			// write character
    for(x = 0; x < 8; x++)
      TFT_pixel(Xpixel + x, Ypixel + y, pixel[x][y]);
}
