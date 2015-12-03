//ICC-AVR application builder : 2007-03-22 ¿ÀÈÄ 7:32:10
// Target : M128
// Crystal: 16.000Mhz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

void delay(int n);



void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x01;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x03;
}


ISR(INT0_vect)
{
    PORTG|=0x01;
	delay(1000);
 //external interupt on INT0
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
 EICRA = 0x02; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x01;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}


void delay(int n)
{
	volatile int i,j;
	for(i=1;i<n;i++)
	{
    	for(j=1;j<900;j++);
	}
}

void main ( void )
{
    init_devices( );

	
	while( 1 )
	{
		PORTG&=~0x01;
	}
}
