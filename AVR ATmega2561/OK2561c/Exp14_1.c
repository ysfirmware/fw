/* ========================================================================== */
/*	          Exp14_1.c : AT24C16 Read/Write using TWI	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

void LCD_4hex(unsigned int number)              /* display 4-digit hex number */
{ unsigned int i;

  i = number >> 12;                             // 16^3
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = (number >> 8) & 0x000F;                   // 16^2
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = (number >> 4) & 0x000F;                   // 16^1
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = number & 0x000F;                          // 16^0
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');
}

void LCD_2hex(unsigned char number)             /* display 2-digit hex number */
{ unsigned char i;

  i = (number >> 4) & 0x0F;                     // 16^1
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');

  i = number & 0x0F;                            // 16^0
  if(i <= 9) LCD_data(i + '0');
  else       LCD_data(i - 10 + 'A');
}

unsigned char AT24C16_READ(unsigned int address) /* read a byte */
{ unsigned char byte;

  TWCR = 0xA4;                                  // START condition
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x08)); // START complete ?

  TWDR = 0xA0 + ((address >> 7) & 0x000E);      // SLA+W
  TWCR = 0x84;
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x18)); // SLA+W complete ?

  TWDR = (address & 0x00FF);                    // word address
  TWCR = 0x84;
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x28)); // address complete ?

  TWCR = 0xA4;                                  // REPEATED START condition
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x10)); // START complete ?

  TWDR = 0xA0 + ((address >> 7) & 0x000E) + 0x01; // SLA+R
  TWCR = 0x84;
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x40)); // SLA+R complete ?

  TWCR = 0x84;                                  // read data with no acknowledge
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x58)); // data complete ?
  byte = TWDR;                                  // read data

  TWCR = 0x94;   		                // STOP condition

  return byte;                                  // return data
}

void AT24C16_WRITE(unsigned int address,unsigned char byte) /* write a byte */
{
  TWCR = 0xA4;                                  // START condition
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x08)); // START complete ?

  TWDR = 0xA0 + ((address >> 7) & 0x000E);      // SLA+W
  TWCR = 0x84;
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x18)); // SLA+W complete ?

  TWDR = (address & 0x00FF);                    // word address
  TWCR = 0x84;
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x28)); // address complete ?

  TWDR = byte;                                  // write data
  TWCR = 0x84;
  while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x28)); // data complete ?

  TWCR = 0x94;   		                // STOP condition

  Delay_ms(5);                                  // delay 5 ms for twr time
}

int main(void)
{ unsigned char byte;
  unsigned int address;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80," AT24C16 EEPROM ");          // display title
  LCD_string(0xC0,"0x0000=0x00/0x00");

  TWBR = 12;                                    // 400kHz(TWBR=12, TWPS=0)
  TWSR = 0x00;

  for(address=0x0000; address<=0x07FF; address++)
    { LCD_command(0xC2);                        // display address
      LCD_4hex(address);

      LCD_command(0xC9);                        // display write data
      LCD_2hex(0x55);
      AT24C16_WRITE(address,0x55);              // write 0x55
      byte = AT24C16_READ(address);
      LCD_command(0xCE);                        // display read data
      LCD_2hex(byte);
      Delay_ms(500);
      if(byte != 0x55)                          // if error, beep
        { Beep();
          Delay_ms(2000);
        }

      LCD_command(0xC9);                        // display write data
      LCD_2hex(0xAA);
      AT24C16_WRITE(address,0xAA);              // write 0xAA
      byte = AT24C16_READ(address);
      LCD_command(0xCE);                        // display read data
      LCD_2hex(byte);
      Delay_ms(500);
      if(byte != 0xAA)                          // if error, beep
        { Beep();
          Delay_ms(2000);
        }
    }

  Beep();
  while(1);
}
