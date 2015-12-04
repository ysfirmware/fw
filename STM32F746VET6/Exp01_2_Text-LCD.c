/* ============================================================================ */
/*	Exp01_2_Text-LCD.c : �⺻ �ؽ�Ʈ LCD ��� ���α׷�(16MHz HSI ���)      */
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"

void Delay_us(unsigned int time_us);		/* time delay for us in 16MHz */
void Delay_ms(unsigned int time_ms);		/* time delay for ms in 16MHz */
void Beep(void);				/* beep for 50 ms */
void Initialize_LCD(void);			/* initialize text LCD module */
void LCD_command(unsigned char command);	/* write a command(instruction) to text LCD */
void LCD_data(unsigned char data);		/* display a character on text LCD */
void LCD_string(unsigned char command, unsigned char *string);	/* display a string on LCD */

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  unsigned int i;

  RCC->AHB1ENR = 0x0000001C;			// port C, D, E clock enable

  GPIOC->MODER &= 0x3FFFFFFF;			// set PC15(buzzer) to output
  GPIOC->MODER |= 0x40000000;

  GPIOD->MODER = 0x55555555;			// set port D to output

  GPIOE->MODER &= 0x3FFFF3F3;			// set PE15(LED), PE5(LCD_E), PE1(TFT_RS) to output
  GPIOE->MODER |= 0x40000404;

  GPIOE->ODR = 0x0000004F;			// TFT_RS = 1, LCD_E = 0

  Delay_ms(50);                                 // wait for system stabilization
  Initialize_LCD();				// initialize text LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title

  while(1)
    { LCD_string(0xC0," 200ms interval ");
      Beep();
      for(i = 0; i < 12; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(200);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(200);
        }

      LCD_string(0xC0," 400ms interval ");
      Beep();
      for(i = 0; i < 6; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(400);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(400);
        }

      LCD_string(0xC0," 600ms interval ");
      Beep();
      for(i = 0; i < 4; i++)
        { GPIOE->BSRR = 0x00008000;		// LED on
          Delay_ms(600);
	  GPIOE->BSRR = 0x80000000;		// LED off
	  Delay_ms(600);
        }
    }
}

/* ----- ����� �Լ� ---------------------------------------------------------- */

void SystemInit(void)				/* dummy system function */
{
  asm volatile("NOP");
}

void Delay_us(unsigned int time_us)		/* time delay for us in 16MHz */
{
  register unsigned int i;

  for(i = 0; i < time_us; i++)			// 3 cycle
    { asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");
      asm volatile("NOP");			// 13 cycle
    }
}

void Delay_ms(unsigned int time_ms)		/* time delay for ms in 16MHz */
{
  register unsigned int i;

  for(i = 0; i < time_ms; i++)
    Delay_us(1000);
}

void Beep(void)					/* beep for 50 ms */
{ 
  GPIOC->BSRR = 0x00008000;   			// buzzer on
  Delay_ms(50);					// delay 50 ms
  GPIOC->BSRR = 0x80000000;			// buzzer off
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
  GPIOE->BSRR = 0x00220000;			// E = 0, Rs = 0
  GPIOD->ODR = command;				// output command
  Delay_us(1);
  GPIOE->BSRR = 0x00000020;			// E = 1
  Delay_us(1);
  GPIOE->BSRR = 0x00200000;			// E = 0
  Delay_us(50);
}

void LCD_data(unsigned char data)		/* display a character on text LCD */
{
  GPIOE->BSRR = 0x00200002;			// E = 0, Rs = 1
  GPIOD->ODR = data;				// output data
  Delay_us(1);
  GPIOE->BSRR = 0x00000020;			// E = 1
  Delay_us(1);
  GPIOE->BSRR = 0x00200000;			// E = 0
  Delay_us(50);
}

void LCD_string(unsigned char command, unsigned char *string)	/* display a string on LCD */
{
  LCD_command(command);				// start position of string
  while(*string != '\0')			// display string
    { LCD_data(*string);
       string++;
    }
}
