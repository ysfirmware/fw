/* ============================================================================ */
/*	Exp01_2_TLCD.c : 기본 텍스트 LCD 출력 프로그램(8MHz HSI 사용)           */
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"

void Delay_us(unsigned int time_us);		/* time delay for us in 8MHz */
void Delay_ms(unsigned int time_ms);		/* time delay for ms in 8MHz */
void Beep(void);				/* beep for 50 ms */
void Initialize_LCD(void);			/* initialize text LCD module */
void LCD_command(unsigned char command);	/* write a command(instruction) to text LCD */
void LCD_data(unsigned char data);		/* display a character on text LCD */
void LCD_string(unsigned char command, char *string);	/* display a string on LCD */

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned int i;

  RCC->AHBENR = 0x005A0000;			// IOPFEN = IOPDEN = IOPCEN = IOPAEN = 1

  GPIOA->MODER &= 0x3C000000;			// reserve PA14(SWCLK) and PA13(SWDIO)
  GPIOA->MODER |= 0x01400000;			// set PA12(BUZZER) and PA11(LED) to output

  GPIOC->MODER = 0x55555555;			// set PC15-PC0 to output

  GPIOD->MODER = 0x00000010;			// set PD2(LCD_E) to output
  GPIOD->ODR = 0x00000000;			// LCD_E = 0

  GPIOF->MODER = 0x00005500;			// set PF7-PF4 to output
  GPIOF->ODR = 0x000000D0;			// -TFT_RESET = -TFT_CS = -TFT_WR = 1

  Delay_ms(50);                                 // wait for system stabilization
  Initialize_LCD();				// initialize text LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title

  while(1)
    { LCD_string(0xC0," 200ms interval ");
      Beep();
      for(i = 0; i < 12; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(200);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(200);
        }

      LCD_string(0xC0," 400ms interval ");
      Beep();
      for(i = 0; i < 6; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(400);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(400);
        }

      LCD_string(0xC0," 600ms interval ");
      Beep();
      for(i = 0; i < 4; i++)
        { GPIOA->BSRR = 0x00000800;		// LED on
          Delay_ms(600);
	  GPIOA->BRR = 0x00000800;		// LED off
	  Delay_ms(600);
        }
    }
}

/* ----- 사용자 함수 ---------------------------------------------------------- */

void SystemInit(void)				/* dummy system function */
{
  asm volatile("NOP");
}

void Delay_us(unsigned int time_us)		/* time delay for us in 8MHz */
{
  register unsigned int i;

  for(i = 0; i < time_us; i++)			// 3 cycle
    { asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");				// 4 cycle
    }
}

void Delay_ms(unsigned int time_ms)		/* time delay for ms in 8MHz */
{
  register unsigned int i;

  for(i = 0; i < time_ms; i++)
    Delay_us(1000);
}

void Beep(void)					/* beep for 50 ms */
{ 
  GPIOA->BSRR = 0x00001000;			// buzzer on
  Delay_ms(50);					// delay 50 ms
  GPIOA->BRR = 0x00001000;			// buzzer off
}

void Initialize_LCD(void)			/* initialize text LCD module */
{
  LCD_command(0x38);				// function set(8 bit, 2 line, 5x7 dot)
  LCD_command(0x0C);				// display control(display ON, cursor OFF)
  LCD_command(0x06);				// entry mode set(increment, not shift)
  LCD_command(0x01);				// clear display
  Delay_ms(2);
}

void LCD_command(unsigned char command)		/* write a command(instruction) to text LCD */
{
  GPIOD->BRR = 0x00000004;			// E = 0
  GPIOF->BRR = 0x00000020;			// Rs = 0
  GPIOC->ODR = command;				// output command
  GPIOD->BSRR = 0x00000004;			// E = 1
  asm("NOP");
  asm("NOP");
  GPIOD->BRR = 0x00000004;			// E = 0
  Delay_us(50);
}

void LCD_data(unsigned char data)		/* display a character on text LCD */
{
  GPIOD->BRR = 0x00000004;			// E = 0
  GPIOF->BSRR = 0x00000020;			// Rs = 1
  GPIOC->ODR = data;				// output data
  GPIOD->BSRR = 0x00000004;			// E = 1
  asm("NOP");
  asm("NOP");
  GPIOD->BRR = 0x00000004;			// E = 0
  Delay_us(50);
}

void LCD_string(unsigned char command, char *string)	/* display a string on LCD */
{
  LCD_command(command);				// start position of string
  while(*string != '\0')			// display string
    { LCD_data(*string);
       string++;
    }
}
