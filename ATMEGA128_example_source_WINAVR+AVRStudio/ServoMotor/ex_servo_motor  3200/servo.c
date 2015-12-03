#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define SERVO_PORT      PORTB
#define SERVO_PORT_DIR  DDRB
#define PULSE_GAP 3700


 unsigned int servo_val;
 volatile unsigned int pulse=0;

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
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0x01;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x03;
}


/////////////////////////////////////////////////////////////////////////


//TIMER1 initialize - prescale:8
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1uSec
// actual value:  1.000uSec (0.0%)
void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFF; //setup
 TCNT1L = 0xFE;
 OCR1AH = 0x00;
 OCR1AL = 0x02;
 OCR1BH = 0x00;
 OCR1BL = 0x02;
 OCR1CH = 0x00;
 OCR1CL = 0x02;
 ICR1H  = 0x00;
 ICR1L  = 0x02;
 TCCR1A = 0x00;
 TCCR1B = 0x02; //start Timer
}

ISR(TIMER1_OVF_vect)
{
 //TIMER1 has overflowed
 //PORTE^=1;
 PORTB^=1;
 if(PORTB&0x01)
 {
 TCNT1=0xFA89-servo_val;
 }
 else
 {
 TCNT1=0x63c0+servo_val;
 }
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
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
 timer1_init();
 uart0_init();
 fdevopen(Putchar,0);

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x05; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 
 sei(); //re-enable interrupts
}

void delay(int n)
{
	volatile int i,j;

	for(i=0;i<n;i++)
	{
    	for(j=1;j<100;j++);
	}
}

void main(void)
{
	int val=1;

    init_devices();
	
	printf("\n\rServo Motor test....\n\r");
    
	
    while(1)            
	{
	 if(PING&0x04)
	 {
	  val++;
	 }
	 else if(PING&0x08)
	 {
	  val--;
	 }
	 if(val>=3500) val=3500;
	 if(val<=0) val=0;
	 servo_val=val;
	 
	 delay(5);
	 }
}


