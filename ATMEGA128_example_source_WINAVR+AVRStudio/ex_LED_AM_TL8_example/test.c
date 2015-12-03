#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>


void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xFF;
 PORTB = 0x00;
 DDRB  = 0xFF;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xFF;
 PORTD = 0x00;
 DDRD  = 0xFF;
 PORTE = 0x00;
 DDRE  = 0xFF;
 PORTF = 0x00;
 DDRF  = 0xFF;
 PORTG = 0x00;
 DDRG  = 0xFF;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
  
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void delay(int cnt){
	 int i, j;
	 for(i=0;i<=700;i++){
	 	for(j=0;j<=cnt;j++){
		}
	}
}
int main(void)
{
    int i;
    init_devices();

    
 while(1){
		
		  for(i=0; i<8; i++)
		  {

//		      PORTA=(1<<i); 
			  PORTA = 0XFF;
			  PORTB=(1<<i);
			  PORTC=(1<<i);
			  PORTD=(1<<i);
			  PORTE=(1<<i);
			  PORTF=(1<<i);
			  PORTG=(1<<i);

		  	  delay(150);

		  } 
		  
	}
	return 0;
}

