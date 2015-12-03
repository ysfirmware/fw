/*******************************************************************************
파일명 :   Exam6_1.c
 
동 작  :   8bit Timer/counter0의 Overflow flag bit(TOV0)를 검사하여 일정 시간마다 
           (64usec X 256 = 16.384msec) 포트 B에 연결된 LED를 하나씩 이동하는 
		   프로그램.
   
           (16MHz)/(1024 prescaler) = 15.625KHz
		   
            T = 1/(15.625KHz) = 64usec 
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

unsigned char data=0xFE;

void main(void)
{
         
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;    // LED off
	
	TCCR0=0x07;    // Normal mode(Timer/counte0 overflow), 1024 prescaler
    TCNT0=0x00;    // Timer/counte0 value(카운트 값)
	TIMSK=0x01;    // TOIE0(Timer/counter0 oveflow interrupt enable)
	               // 개별 인터럽트 인에이블
				   // 또는 TIMSK |= (1<<TOIE0); 를 사용하여도 된다.    
    TIFR|=0x01;     // TOV0 Timer/counter0 oveflow flag 클리어 

    __disable_interrupt();   // Global 인터럽트 불능

    while(1)
	  {    
	    while(!(TIFR&0x01)); // TOV0 Timer/counter0 oveflow flag가 "1"이 될 때까지
		                     // 기다린다.
		TIFR|=0x01;     // TOV0 Timer/counter0 oveflow flag 클리어
        TCNT0=0x00;    // Timer/counte0 value(카운트 값)
		
		PORTB=data;
		data--;
	   };
}