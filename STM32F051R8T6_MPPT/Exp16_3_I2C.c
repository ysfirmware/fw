/* ============================================================================ */
/*	Exp16_3_I2C.c : I2C ������ſ� ���� AT24C16 EEPROM�� �׼���		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void Initialize_I2C2(void);			/* initialize I2C2 */
void AT24C16_write(U16 address,U08 byte);	/* write a byte to AT24C16 */
unsigned char AT24C16_read(U16 address);	/* read a byte from AT24C16 */

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  unsigned char byte, error;
  unsigned short address;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Initialize_I2C2();				// initialize I2C2

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp16_3.c    ");
  Beep();

  TFT_string( 3, 5,White,Magenta, "I2C�� ����� AT24C16 EEPROM �׼���");
  TFT_string(11,10,White,Black, "��巹�� = 0x0000");
  TFT_string( 7,14,White,Black, "�޸� ���� ������ = 0x00");
  TFT_string( 7,17,White,Black, "�޸� �б� ������ = 0x00");

  error = 0;
  for(address = 0x0000; address <= 0x07FF; address++)
    { TFT_color(Cyan,Black);
      TFT_xy(22,10);
      TFT_0x_hexadecimal(address,4);

      TFT_xy(28,14);				// test 0x55
      TFT_0x_hexadecimal(0x55,2);

      AT24C16_write(address,0x55);
      byte = AT24C16_read(address);

      TFT_xy(28,17);
      TFT_0x_hexadecimal(byte,2);
      Delay_ms(200);

      if(byte != 0x55)				// if error, beep 3 times
        { error = 1;
	  Beep_3times();
          Delay_ms(2000);
        }

      TFT_xy(28,14);				// test 0xAA
      TFT_0x_hexadecimal(0xAA,2);

      AT24C16_write(address,0xAA);
      byte = AT24C16_read(address);

      TFT_xy(28,17);
      TFT_0x_hexadecimal(byte,2);
      Delay_ms(200);

      if(byte != 0xAA)				// if error, beep 3 times
        { error = 1;
	  Beep_3times();
          Delay_ms(2000);
        }
    }

  Beep();
  if(error == 0) TFT_string(9,22,Green,Black,"AT24C16 �б�/���� OK !");
  else           TFT_string(8,22,Red,Black,"AT24C16 �б�/���� ���� !");

  while(1);
}

/* ----- ����� ���� �Լ� ----------------------------------------------------- */

void Initialize_I2C2(void)			/* initialize I2C2 */
{
  GPIOB->MODER &= 0xFF0FFFFF;			// PB11(I2C2_SDA),PB10(I2C2_SCL) = alternate function
  GPIOB->MODER |= 0x00A00000;
  GPIOB->AFR[1] &= 0xFFFF00FF;			// PB11 = I2C2_SDA, PB10 = I2C2_SCL
  GPIOB->AFR[1] |= 0x00001100;

  RCC->APB1ENR |= 0x00400000;			// enable I2C2 clock(= PCLK = 48MHz)

  I2C2->CR1 = 0x00000000;			// PE = 0
  I2C2->TIMINGR = 0x50400000;			// PRESC=5, SCLDEL=4, SDADEL=3, SCLH=9, SCLL=9
  I2C2->CR1 = 0x00000001;			// PE = 1
}

void AT24C16_write(U16 address,U08 byte)	/* write a byte to AT24C16 */
{

  I2C2->CR2 = 0x000200A0 + ((address >> 7) & 0x000E); // write operation, NBYTES=2, SADD=0xA0
  I2C2->CR2 |= 0x00002000;			// START condition
  while(!(I2C2->ISR & 0x00000002));		// TXIS = 1 ?

  I2C2->TXDR = address & 0x00FF;		// write address
  while(!(I2C2->ISR & 0x00000002));		// TXIS = 1 ?

  I2C2->TXDR = byte;				// write data
  while(!(I2C2->ISR & 0x00000040));		// TC = 1 ?
  I2C2->CR2 |= 0x00004000;			// STOP condition
  Delay_ms(5);                                  // delay 5 ms for twr time
}

unsigned char AT24C16_read(U16 address)		/* read a byte from AT24C16 */
{ unsigned char byte;

  I2C2->CR2 = 0x000100A0 + ((address >> 7) & 0x000E); // write operation, NBYTES=1, SADD=0xA0
  I2C2->CR2 |= 0x00002000;			// START condition
  while(!(I2C2->ISR & 0x00000002));		// TXIS = 1 ?

  I2C2->TXDR = address & 0x00FF;		// write address
  while(!(I2C2->ISR & 0x00000040));		// TC = 1 ?

  I2C2->CR2 = 0x000104A0 + ((address >> 7) & 0x000E); // read operation, NBYTES=1, SADD=0xA0
  I2C2->CR2 |= 0x00002000;			// START condition
  while(!(I2C2->ISR & 0x00000004));		// RXNE = 1 ?
  byte = I2C2->RXDR & 0x000000FF;		// read data
  while(!(I2C2->ISR & 0x00000040));		// TC = 1 ?

  I2C2->CR2 |= 0x00004000;			// STOP condition
  Delay_us(10);

  return byte;                                  // return data
}
