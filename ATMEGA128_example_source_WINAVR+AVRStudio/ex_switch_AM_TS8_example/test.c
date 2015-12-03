#include <avr/io.h>
#include <avr/interrupt.h>
#include "c:/WinAVR-20100110/avr/include/avr/iom128.h"
#include <stdio.h>

void tx0Char(char message);

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0xff;
}

// UART0 을 이용한 출력
void tx0Char(char message)
{
	while (((UCSR0A>>UDRE0)&0x01) == 0) ;  // UDRE, data register empty        
    UDR0 = message;
}


static int Putchar(char c, FILE *stream)
{
	tx0Char(c);
    return 0;
      
}

//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9615 (0.2%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x67; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x18;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
 uart0_init();
 fdevopen(Putchar,0);  
 
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void delay(int delaytime){
	 int i,j;
	 for(i=0;i<1000;i++){
	 	for(j=0;j<delaytime;j++){
		}
	 }
}

//
int main(void)
{
 volatile unsigned char keyin;
 init_devices();

 while(1){
 		  if(PINA != 0x00){
		      keyin = PINA;
		  }
		  else keyin=0;
 		  switch(keyin){
		  	case 1 : PORTG = 0x00; 
				   	 printf("1");
					 delay(300);
					 break;
			case 2 : PORTG = 0x01;
				   	 printf("2");
					 delay(300);
					 break;
		  	case 4 : PORTG = 0x02; 
				   	 printf("3");
					 delay(300);
					 break;
		  	case 8 : PORTG = 0x03;
				   	 printf("4");
					 delay(300);
					 break;
		  	case 16 : PORTG = 0x00; 
				   	 printf("5");
					 delay(300);
					 break;
		  	case 32 : PORTG = 0x01;
				   	 printf("6");
					 delay(300);
					 break;
		  	case 64: PORTG = 0x02;
				   	 printf("7");
					 delay(300);
					 break;
		  	case 128 : PORTG = 0x03;
				   	 printf("8");
					 delay(300);
					 break;
		  				
		  } 
 }
 return 0;
}
