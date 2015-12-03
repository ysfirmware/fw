#include <avr/io.h>
#include <avr/interrupt.h>

#define SERVO_PORTA PORTA
#define SERVO_PORTB PORTB
#define SERVO_PORTC PORTC

unsigned int ServoA[10]={0,}, ServoB[10]={0,}, ServoC[10]={0,};

volatile unsigned int count=0;
 
void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xFF;
 PORTB = 0x00;
 DDRB  = 0xFF;
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
// desired value: 8uSec
// actual value:  8.000uSec (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0xFC; //set count
 OCR0  = 0x04;
 TCCR0 = 0x03; //start timer
}

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0xFE; //reload counter value

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
 //MIN=15, MAX=60 
 ServoA[0]= 15;
 ServoA[1]= 22;
 ServoA[2]= 29;
 ServoA[3]= 36;
 ServoA[4]= 44;
 ServoA[5]= 51;
 ServoA[6]= 54;
 ServoA[7]= 60;
 delay(1000);
 ServoB[0]= 15;
 ServoB[1]= 22;
 ServoB[2]= 29;
 ServoB[3]= 36;
 ServoB[4]= 44;
 ServoB[5]= 51;
 ServoB[6]= 54;
 ServoB[7]= 60;
 delay(1000);
 ServoC[0]= 15;
 ServoC[1]= 22;
 ServoC[2]= 29;
 ServoC[3]= 36;
 ServoC[4]= 44;
 ServoC[5]= 51;
 ServoC[6]= 54;
 ServoC[7]= 60;
 delay(1000);


 
 while(1){
	if(count>1000){
		cli();
		SERVO_PORTA = 0xFF;
		SERVO_PORTB = 0xFF;
		SERVO_PORTC = 0xFF;
		count = 0;
		sei();
		}
		cli();
	for(i=0; i<8; i++){
			
  		if(ServoA[i] < count)	SERVO_PORTA &= bitreset[i];
					
  		if(ServoB[i] < count)	SERVO_PORTB &= bitreset[i];
					
		if(ServoC[i] < count)	SERVO_PORTC &= bitreset[i];
			
		}
		sei();
				
}
}

