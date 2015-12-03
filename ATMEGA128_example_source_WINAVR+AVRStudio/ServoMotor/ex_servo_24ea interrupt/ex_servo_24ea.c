#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define SERVO_PORT1			PORTD
#define SERVO_PORT2			PORTC
#define SERVO_PORT3			PORTB

#define t20ms				130
#define t20mstimer_expired	0
#define ON					1
#define OFF 				0
#define MAX_SERVO			24

volatile int t100ustimer=0, servo[MAX_SERVO], servo_num;

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
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0xFF;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
}


//TIMER0 initialize - prescale:8
// WGM: Normal
// desired value: 1uSec
// actual value:  1.000uSec (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0xFE; //set count
 OCR0  = 0x02;
 TCCR0 = 0x02; //start timer
}

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0xFE; //reload counter value
 t100ustimer++;
 if(t100ustimer > t20ms){
 	t100ustimer = t20mstimer_expired;
 }
 for(servo_num=0;servo_num<8;servo_num++){
	if(servo[servo_num] > t100ustimer){
		SERVO_PORT1 = SERVO_PORT1|(0x01<<servo_num);
 	}else{
		SERVO_PORT1 = SERVO_PORT1&(~(0x01<<servo_num));
 	}
 }
 for(servo_num=0;servo_num<8;servo_num++){
	if(servo[servo_num+8] > t100ustimer){
 		SERVO_PORT2 = SERVO_PORT2|(0x01<<servo_num);
	}else{
		SERVO_PORT2 = SERVO_PORT2&(~(0x01<<servo_num));
	}
 }
 for(servo_num=0;servo_num<8;servo_num++){
	if(servo[servo_num+16] > t100ustimer){
 		SERVO_PORT3 = SERVO_PORT3|(0x01<<servo_num);
	}else{
		SERVO_PORT3 = SERVO_PORT3&(~(0x01<<servo_num));
	}
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
 UCSR0B = 0xF8;
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

void delay(int delaytime){
	int i,j;
	for(i=0;i<delaytime;i++){
		for(j=0;j<100;j++){
		}
	}
}

//
void main(void)
{
 int i;
 init_devices();
 
 for(i=0;i<MAX_SERVO;i++){
 	servo[i]=0;
 } 
 //insert your functional code here...
 printf("\r\n\r\n Testing Servo Motor Ver1.0\r\n");
 
 while(1){
	for(i=0;i<MAX_SERVO;i++){
 	servo[i]=5;}
	delay(30);
	
	for(i=0;i<MAX_SERVO;i++){
 	servo[i]=18;}
	delay(30);
 }
}


