#include <avr/io.h>
#include <avr/interrupt.h>

#define MOTOR_PORT		PORTC
#define MOTOR_PORT_DDR	DDRC

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xff;
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

void delay(int n)
{
	volatile int i,j;
	for(i=1;i<n;i++)
	{
    	for(j=1;j<600;j++);
	}
}

int main(void)
{
 init_devices();
 
MOTOR_PORT_DDR = 0xff;
    
	while(1){
		MOTOR_PORT = 0x33;
		delay(4);
		MOTOR_PORT = 0x66;
		delay(4);
		MOTOR_PORT = 0xcc;
		delay(4);
		MOTOR_PORT = 0x99;
		delay(4);		
	}
return 0;
}

