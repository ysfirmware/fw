#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define SENSOR_PORT		PINF

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
 DDRG  = 0x03;
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

void delay(int n)
{
	volatile int i,j;
	for(i=1;i<n;i++)
	{
    	for(j=1;j<600;j++);
	}
}

//
int main(void)
{
 unsigned char sensor;
// volatile unsigned char stepleft=0, stepright=0;
 
 init_devices();
 
 while(1){
   		sensor = SENSOR_PORT & 0x0F;

		switch(sensor){
			case 0x00:  printf("Nothing\r\n");
						PORTG= 0x01;
						break;
			case 0x08:  printf("LEFTEST\r\n");	   
						PORTG= 0x02;
						break;
			case 0x04:  printf("LEFT\r\n");
						PORTG= 0x03;
						break;
			case 0x03:
			case 0x02:  printf("RIGHT\r\n");
					    PORTG= 0x00;
					   	break;
			case 0x01:  printf("RIGHTEST\r\n");
						PORTG= 0x03;
						break;
			
		}		
 }
 return 0;
}
