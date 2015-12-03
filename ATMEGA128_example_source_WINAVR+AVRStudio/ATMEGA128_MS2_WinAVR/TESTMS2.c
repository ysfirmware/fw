// S/W Environment : AVR Studio + WINAVR Compiler
// Target : M128
// Crystal: 16Mhz
//
// Made by New Technology Company(NEWTC Co., Ltd.) in Korea.
// DAEWOO RYU   
// Email : davidryu@newtc.co.kr
// 82-2-704-4774(TEL), 82-2-704-4733(FAX)
// http://www.NewTC.co.kr
// example : MS2 Stepping Motor Control

#include <avr/io.h>
#include <avr/interrupt.h>
#include "c:/WinAVR/avr/include/avr/iom128.h"
#include <stdio.h>


#define MOTOR_PORT		PORTC       // 스테핑 모터 연결 포트
#define MOTOR_PORT_DDR	DDRC        // 스테핑 모터 연결 포트의 DDR REG

#define MOTOR_ENABLE	(MOTOR_PORT = MOTOR_PORT|0x80)
#define MOTOR_DISABLE	(MOTOR_PORT = MOTOR_PORT&0x7f)
#define MOTOR_STEP_M0	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x00)
#define MOTOR_STEP_M1	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x10)
#define MOTOR_STEP_M2	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x20)
#define MOTOR_STEP_M3	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x30)
#define MOTOR_LEFT_CLK	(MOTOR_PORT = MOTOR_PORT^0x01)
#define MOTOR_RIGHT_CLK	(MOTOR_PORT = MOTOR_PORT^0x04)
#define MOTOR_LEFT_CW	(MOTOR_PORT = MOTOR_PORT&0xfd)
#define MOTOR_LEFT_CCW	(MOTOR_PORT = MOTOR_PORT|0x02)
#define MOTOR_RIGHT_CW	(MOTOR_PORT = MOTOR_PORT&0xf7)
#define MOTOR_RIGHT_CCW	(MOTOR_PORT = MOTOR_PORT|0x08)

void tx0Char(char message);
void tx1Char(char message);




//int putchar(char c);
//int getchar(void);
void port_init(void);
void uart0_init(void);
void uart1_init(void);
void init_devices(void);
void delay(int n);
static int Putchar(char c, FILE *stream);

static int Putchar(char c, FILE *stream)
{
	tx0Char(c);
    //tx1Char(c);
	return 0;
      
}

/*
// printf 함수에서 사용할 putchar 에 UART0와 UART1으로
// 데이터를 보내도록 하였다.
int putchar(char c)
{
    tx0Char(c);
    tx1Char(c);
	return c;
}*/

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
 DDRA  = 0x83;
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

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XDIV  = 0x00; //xtal divider
 XMCRA = 0x00; //external memory
 port_init();
 uart0_init();              // UART 0 초기화
 //uart1_init();              // UART 1 초기화
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


// 시간 지연 함수
void delay(int n)
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
	
	//insert your functional code here...
	printf("\n\r\ntesting...\n\r");
	printf("MS2 스테핑 모터 드라이버 프로그램...\n\r");
	  MOTOR_PORT_DDR = 0xff;

	MOTOR_ENABLE;
	MOTOR_STEP_M3;
    MOTOR_LEFT_CCW;
    MOTOR_RIGHT_CCW;
	
	/* 딜레이로 마이크로 스텝 드라이버 구동 */ 
	while(1){
    	PORTG++;
		MOTOR_LEFT_CLK;
		MOTOR_RIGHT_CLK;
		delay(2);
		MOTOR_LEFT_CLK;
		MOTOR_RIGHT_CLK;
		delay(2);
	}
	return 0;
}


