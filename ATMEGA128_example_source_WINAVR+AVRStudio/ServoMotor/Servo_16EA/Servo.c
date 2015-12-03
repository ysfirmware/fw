#include <avr/io.h>
#include <avr/interrupt.h>

#define SERVO_PORTA PORTA
#define SERVO_PORTC PORTC

unsigned int ServoA[8]={0,};
unsigned int ServoC[8]={0,};

volatile unsigned int count=0;

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
 DDRG  = 0x03;
}

//TIMER0 initialize - prescale:32
// WGM: Normal
// desired value: 10uSec
// actual value: 10.000uSec (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0xFB; //set count
 OCR0  = 0x05;
 TCCR0 = 0x03; //start timer
}

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0xEC; //reload counter value
 count++;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
 timer0_init();

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x01; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void delay(unsigned int cnt)
{
 unsigned int i;
 for(i=0; i<cnt; i++) ;
 }
//
void main(void)
{
 unsigned char i;
 unsigned char bitreset[8]={0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
 init_devices();
 delay(1000);
 //MIN=70, MAX=230 
 ServoA[0]= 12;
 ServoA[1]= 15;
 ServoA[2]= 20;
 ServoA[3]= 25;
 ServoA[4]= 30;
 ServoA[5]= 35;
 ServoA[6]= 40;
 ServoA[7]= 45;
 delay(1000);
 ServoC[0]= 12;
 ServoC[1]= 15;
 ServoC[2]= 20;
 ServoC[3]= 25;
 ServoC[4]= 30;
 ServoC[5]= 35;
 ServoC[6]= 40;
 ServoC[7]= 45;
 delay(1000);
 
 while(1){
  		 	if(count>1500){
 				SERVO_PORTA = 0xFF;
				SERVO_PORTC = 0xFF;
				count = 0;
				}
			cli();
			for(i=0; i<8; i++)
			{
		  		if(ServoA[i] < count){SERVO_PORTA &= bitreset[i];}
		  		if(ServoC[i] < count){SERVO_PORTC &= bitreset[i];}
			}
			sei();
			delay(10);
 		  
}
}

