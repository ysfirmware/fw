#include <avr/io.h>
#include <avr/interrupt.h>
#include "c:/WinAVR-20100110/avr/include/avr/iom128.h"
#include <stdio.h>

#define SERVO_PORT      PORTB
#define SERVO_PORT_DIR  DDRB
//#define PULSE_GAP       2496
#define PULSE_GAP       2700

#define max				140
#define min				40

#define MAX_SERVO_COUNT 8

volatile unsigned int servo_val;
volatile unsigned int pulse_=0;


static int Putchar(char c, FILE *stream);
void tx0Char(char message);


static int Putchar(char c, FILE *stream)
{
	tx0Char(c);
	return 0;
      
}

// UART0 을 이용한 출력
void tx0Char(char message)
{
	while (((UCSR0A>>UDRE0)&0x01) == 0) ;  // UDRE, data register empty        
    UDR0 = message;
}


int Getchar(void)
{
    while ((UCSR0A & 0x80) == 0);
    return UDR0;
}


void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0xff;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
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
 TCNT0 = 0xF8; //reload counter value
 //TIMER1 has overflowed
 
 pulse_++;

 if(pulse_<=servo_val) SERVO_PORT|=(0x01);
 else					 	SERVO_PORT&=~(0x01);
 
 if(pulse_>=PULSE_GAP) 
 {
  pulse_=0;
 }
 
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
 timer0_init();
 uart0_init();
 fdevopen(Putchar,0);

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x01; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void delay(int n)
{
	volatile int i,j;
	for(i=1;i<n;i++)
	{
    	for(j=1;j<100;j++);
	}
}

void main(void)
{
 //servo_val[i]  max 290  min 70    if servo_val[i]=0  power off
	unsigned int i, defValue=min;
	char ch;
    init_devices();
	printf("\n\r");
	
	

	DDRF = 0x03;
    

    //printf("\n\rServo Motor test....\n\r");
    delay(100);
    SERVO_PORT_DIR = 0xff;
	
	while(1){

		 ch=Getchar();
		 printf("Value:%d\n\r",defValue);
		 if((ch == 'a') || (ch == 'A')){
			defValue++;
		 }
		 
		 if((ch == 'q') || (ch == 'Q')){
			defValue--;
		 }		 
		 
		 if(defValue <=min) defValue = min;
		 if(defValue >=max) defValue = max;
		 servo_val = defValue; 
	
		 delay(5);
    }
	
}
