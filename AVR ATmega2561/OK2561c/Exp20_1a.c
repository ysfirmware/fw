/* ========================================================================== */
/*	          Exp20_1a.c : Read/Write ATmega2561 EEPROM (2)	              */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include <avr/eeprom.h>
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

int main(void)
{ unsigned char byte;
  unsigned int address;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  LCD_string(0x80," ATmega2561 EEP ");          // display title
  LCD_string(0xC0,"0x0000=0x00/0x00");

  EECR = 0x00;					// can erase and write EEPROM

  for(address=0x0000; address<=0x0FFF; address++)
    { LCD_command(0xC2);                        // display address
      LCD_4hex(address);

      LCD_command(0xC9);                        // display write data
      LCD_2hex(0x55);
      while(!eeprom_is_ready());                // if EEPROM is ready,
      eeprom_write_byte((uint8_t *)address,0x55);  //  write 0x55
      while(!eeprom_is_ready());                // if EEPROM is ready,
      byte = eeprom_read_byte((uint8_t *)address); //  read
      LCD_command(0xCE);                        // display read data
      LCD_2hex(byte);
      Delay_ms(500);
      if(byte != 0x55)                          // if error, beep
        { Beep();
          Delay_ms(2000);
        }

      LCD_command(0xC9);                        // display write data
      LCD_2hex(0xAA);
      while(!eeprom_is_ready());                // if EEPROM is ready,
      eeprom_write_byte((uint8_t *)address,0xAA);  //  write 0xAA
      while(!eeprom_is_ready());                // if EEPROM is ready,
      byte = eeprom_read_byte((uint8_t *)address); //  read
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

