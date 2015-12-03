#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define SERVO_PORT      PORTC
#define SERVO_PORT_DIR  DDRC
#define PULSE_GAP       2000

#define max				135
#define min				35

#define MAX_SERVO_COUNT 8

unsigned int servo_val[8];

static int Putchar(char c, FILE *stream);
void tx0Char(char message);
void tx1Char(char message);



static int Putchar(char c, FILE *stream)
{
	tx0Char(c);
    tx1Char(c);
	return 0;
      
}

// UART0 을 이용한 출력
void tx0Char(char message)
{
	while (((UCSR0A>>UDRE0)&0x01) == 0) ;  // UDRE, data register empty        
    UDR0 = message;
}

// UART1 을 이용한 출력
void tx1Char(char message)
{
	while (((UCSR1A>>UDRE1)&0x01) == 0) ;  // UDRE, data register empty        
    UDR1 = message;
}
// scanf 함수 사용시 추가할 것.
int Getchar(void)
{
    while ((UCSR0A & 0x80) == 0);
    return UDR0;
}


void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x03;
 PORTB = 0x00;
 DDRB  = 0xFF;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x03;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0xFF;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
}


/////////////////////////////////////////////////////////////////////////



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
 static unsigned int pulse_=0;
 static unsigned char fs=0;
 TCNT0 = 0xF8; //reload counter value
 //TIMER1 has overflowed
 
 pulse_++;
 fs++;
  fs=fs%MAX_SERVO_COUNT;
  if(pulse_>=PULSE_GAP) 
 {
  SERVO_PORT=0xff;
  pulse_=0;
 }

 if(pulse_<=servo_val[fs]) SERVO_PORT|=(0x01<<fs);
 else					   SERVO_PORT&=~(0x01<<fs);
 
}



/////////////////////////////////////////////////////////////////////////////
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
 UCSR0B = 0x08;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 char i;
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
}

void delay(int n)
{
	volatile int i,j;
	for(i=0;i<n;i++)
	{
    	for(j=0;j<1000;j++);
	}
}


void main(void)
{

    init_devices();
	SERVO_PORT_DIR=0xff;
    printf("\n\rServo Motor test....\n\r");

	
	while(1)
	{
	 servo_val[0]=min;
	 servo_val[1]=min;
	 servo_val[2]=min;
	 servo_val[3]=min;
	 servo_val[4]=min;
	 servo_val[5]=min;
	 servo_val[6]=min;
	 servo_val[7]=min;
	 delay(1000);
	 servo_val[0]=max;
	 servo_val[1]=max;
	 servo_val[2]=max;
	 servo_val[3]=max;
	 servo_val[4]=max;
	 servo_val[5]=max;
	 servo_val[6]=max;
	 servo_val[7]=max;
	 delay(1000);
	}

	
  
}


