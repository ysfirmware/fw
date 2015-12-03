/*******************************************************************************
파일명 :   Exam6_4.c
 
동 작  :   OC0(PB4)핀을 CTC 기능을 이용하여 주기적으로 LED에 
           토글 값을 출력하며, 여기서 주기는 50%가 된다.

           주파수 = (16MHz)/(1024 Prescaler) = 15625 Hz
		   T = 1/15625 = 0.000064 sec = 64 usec

Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)


void OC0_CTC_out(unsigned char duty)
{
	OCR0=duty;
} 


void main( void )
{       
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;
   
    DDRB_Bit4 = 1; // PORTB.4를 OC0의 출력로 설정한다.

    TCCR0=0x1F;    // (0001 1111) CTC 모드, 토글, 1024 prescaler
    TCNT0=0x00;    // 타이머0 초기값
    
    OC0_CTC_out(0x80); // OCR0와 TCNT0가 같으면 토글 
                       // 즉 토글의 주기는 0x00~0x8E0까지가 된다. 
					   // 0x8C0(128) X 64usec = 8192 usec = 8.192 msec
					   // 하나의 펄스의 주기는 8.192 + 8.192 = 16.384 msec 이다.
                       // 16.384 msec마다 LED가 On/Off 된다.

    while(1);    
} 

