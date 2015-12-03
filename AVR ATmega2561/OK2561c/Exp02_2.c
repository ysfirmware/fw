/* ========================================================================== */
/*	             Exp02_2.c : Basic C Program (2)	                      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>

#define LCD_DATABUS  (*(volatile unsigned char *)0x4000) // LCD/GLCD data
#define LCD_CONTROL  (*(volatile unsigned char *)0x4100) // LCD/GLCD control signal
#define DIG_SELECT   (*(volatile unsigned char *)0x4200) // 7-segment LED digit select

void MCU_initialize(void)			/* initialize ATmege2561 MCU & OK-2561 kit */
{ 
  XMCRA = 0xC1;					// 0x2200-0x7FFF=1 wait, 0x8000-0xFFFF=0 wait
  XMCRB = 0x80;					// enable bus keeper, use PC0-PC7 as address

  DDRB = 0xFF;					// PORTB = output (D/A, LED1-4, matrix LED)
  PORTB = 0x00;
  DDRD = 0xFF;					// PORTD = output (matrix LED)
  PORTD = 0x00;
  DDRE = 0x0C;					// PORTE = output for PE3, PE2
  PORTE = 0x00;
  DDRF = 0x00;					// PORTF = input (KEY1-4)
  PORTF = 0x00;

  LCD_CONTROL = 0x00;				// clear LCD/GLCD control signal
  DIG_SELECT = 0x00;				// clear 7-segment LED
}

void Delay_us(unsigned char time_us)		/* time delay for us */
{ register unsigned char i;

  for(i = 0; i < time_us; i++)			// 4 cycle +
    { asm volatile(" PUSH  R0 ");		// 2 cycle +
      asm volatile(" POP   R0 ");		// 2 cycle +
      asm volatile(" PUSH  R0 ");		// 2 cycle +
      asm volatile(" POP   R0 ");		// 2 cycle +
      asm volatile(" PUSH  R0 ");		// 2 cycle +
      asm volatile(" POP   R0 ");		// 2 cycle = 16 cycle = 1 us for 16MHz
    }
}

void Delay_ms(unsigned int time_ms)		/* time delay for ms */
{ register unsigned int i;

  for(i = 0; i < time_ms; i++)
    { Delay_us(250);
      Delay_us(250);
      Delay_us(250);
      Delay_us(250);
    }
}

void LCD_command(unsigned char command)		/* write a command(instruction) to text LCD */
{
  LCD_CONTROL = 0x00;				// E = 0, Rs = 0
  LCD_DATABUS = command;			// output command
  LCD_CONTROL = 0x01;				// E = 1
  asm volatile(" PUSH  R0 ");			// delay for about 250 ns
  asm volatile(" POP   R0 ");
  LCD_CONTROL = 0x00;				// E = 0
  Delay_us(50);
}

void LCD_data(unsigned char data)		/* display a character on text LCD */
{
  LCD_CONTROL = 0x02;				// E = 0, Rs = 1
  LCD_DATABUS = data;				// output data
  LCD_CONTROL = 0x03;				// E = 1
  asm volatile(" PUSH  R0 ");			// delay for about 250 ns
  asm volatile(" POP   R0 ");
  LCD_CONTROL = 0x02;				// E = 0
  Delay_us(50);
}

void LCD_string(unsigned char command, char *string) /* display a string on LCD */
{
  LCD_command(command);				// start position of string
  while(*string != '\0')			// display string
    { LCD_data(*string);
       string++;
    }
}

void LCD_initialize(void)			/* initialize text LCD module */
{
  LCD_command(0x38);				// function set(8 bit, 2 line, 5x7 dot)
  LCD_command(0x0C);				// display control(display ON, cursor OFF)
  LCD_command(0x06);				// entry mode set(increment, not shift)
  LCD_command(0x01);				// clear display
  Delay_ms(2);
}

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  LCD_string(0x80,"  OK-2561 V1.0  ");          // display logo

  while(1)
    { LCD_string(0xC0," ATmega2561-16AU");      // display message 1
      Delay_ms(1000);
      LCD_string(0xC0,"   2006/10/01   ");      // display message 2
      Delay_ms(1000);
    }
}

