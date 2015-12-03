// S/W Environment : AVR Studio + WINAVR Compiler
// Target : M128
// Crystal: 16Mhz
//
// Made by New Technology Company(N.T.C) in Korea.
// DAEWOO RYU   
// Email : davidryu@newtc.co.kr
// 82-2-704-4774(TEL), 82-2-704-4733(FAX)
// http://www.NTC.co.kr
// example : UART Communication

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h> <--삽입 

#include "c:/WinAVR/avr/include/avr/iom128.h"
#include <stdio.h>


void tx0Char(char message);
void tx1Char(char message);


//int putchar(char c);
//int getchar(void);
void port_init(void);
void uart0_init(void);
void uart1_init(void);
void init_devices(void);
void delays(int n);
static int Putchar(char c, FILE *stream);

volatile unsigned int howmuch;

SIGNAL(SIG_OVERFLOW0){	
	TCNT0 = 0x38; 
	howmuch++;
	if(howmuch >= 10000)
	{
		howmuch=0;
		if((PORTA&0x01)==0x01){
			PORTA = 0x00;
		}else{
			PORTA = 0x01;
		}
	}
}


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


void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x02;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0xff;
 PORTG = 0x00;
 DDRG  = 0x03;
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

//UART1 initialize
// desired baud rate:9600
// actual baud rate:9615 (0.2%)
// char size: 8 bit
// parity: Disabled
void uart1_init(void)
{
 UCSR1B = 0x00; //disable while setting baud rate
 UCSR1A = 0x00;
 UCSR1C = 0x06;
// UBRR1L = 0x2F; //set baud rate lo 7.3728 MHz
// UBRR1L = 0x47; //set baud rate lo 11.0592 Mhz
 UBRR1L = 0x67; //set baud rate lo 16Mhz
 UBRR1H = 0x00; //set baud rate hi
 UCSR1B = 0x18;
}


//TIMER0 initialize - prescale:8
// WGM: Normal
// desired value: 10000Hz
// actual value: 10000.000Hz (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0x38; //set count
 OCR0  = 0xC8;
 TCCR0 = 0x02; //start timer
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XDIV  = 0x00; //xtal divider
 XMCRA = 0x00; //external memory
 port_init();
 timer0_init();
 //uart0_init();              // UART 0 초기화
 //uart1_init();              // UART 1 초기화
 //fdevopen(Putchar,0);   

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x01; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}


// 시간 지연 함수
void delays(int n)
{
	volatile int i,j;
	for(i=1;i<n;i++)
	{
    	for(j=1;j<600;j++);
	}
}



int main (void)
{
	int i=0;
	init_devices();
	howmuch = 0;
	//insert your functional code here...
	//printf("\n\r\ntesting...\n\r");
	//printf("printf 테스트 프로그램...\n\r");
	PORTA = 0x01;
	while(1){

		//printf("테스트 i : %d\n\r",i);
		//i++;
		//PORTA = i;
        //PORTG = i;          // LED 출력
        delays(100);

	}
	return 0;
}


