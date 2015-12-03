
#include <avr/io.h>
#include <avr/pgmspace.h>
//#include <stdio.h>
#include "FONTK52.h"
#include "GLCDK52.h"


//ICC-AVR application builder : 2006-06-13 오후 10:18:26

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xFF;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xFF;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 XMCRA = 0x00; //external memory
 port_init();
 
 
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources

}

//
int main(void)
{
 unsigned char i;
 init_devices();
 //insert your functional code here...
 
  //for(i=0;i<100;i++)
  //delay(60000);
  lcd_init();				/* initialize GLCD */
  lcd_clear();

  //lcd_xy(0,0);
  //lcd_string(0,0,"안녕하세요!!");	
  //write_data(CS_ALL,0xff);
  
  while(1)  
    { 	  
	  lcd_string(0,0,"  뉴티씨(NEWTC) ");	
	  lcd_string(1,0," www.newtc.co.kr");
      lcd_string(2,0,"한글 English !@#");
      lcd_string(3,0,"UG-13B04-SEAT5-E");
	  delay_ms(1200);
	  
      lcd_string(0,0,"GLCD128X64  V1.0");	
      lcd_string(1,0,"128x64 그래픽LCD");
      lcd_string(2,0," 8x16도트 ASCII ");
      lcd_string(3,0,"16x16");
	  delay_ms(1200);
    }	
}

