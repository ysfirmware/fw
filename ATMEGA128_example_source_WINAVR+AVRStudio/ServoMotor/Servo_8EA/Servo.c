#include <avr/io.h>
#include <avr/interrupt.h>

#define SERVO_PORT PORTC

unsigned int Servo[8]={0,};
volatile unsigned int count;

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
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
// desired value: 4uSec
// actual value:  4.000uSec (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0xFE; //set count
 OCR0  = 0x02;
 TCCR0 = 0x03; //start timer
}

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0xFF; //reload counter value
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
 //MIN=80, MAX=315
 Servo[0]= 80;
 Servo[1]= 110;
 Servo[2]= 145;
 Servo[3]= 175;
 Servo[4]= 210;
 Servo[5]= 240;
 Servo[6]= 275;
 Servo[7]= 315;
 delay(1000);

 while(1){
 
	if(count>=4999){
 		SERVO_PORT = 0xFF;
		count = 0;
	}
	cli();
	for(i=0; i<8; i++){
		if(Servo[i] < count){SERVO_PORT &= bitreset[i];}
		}
		sei();
				
	}
 		 
}

