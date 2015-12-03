/*******************************************************************************
파일명 :   Exam6_3.c
 
동 작  :   8bit Timer/counter0의 Overflow interrupt를 이용하여 일정 시간마다 
           (1 sec) 포트 B에 연결된 LED를 하나씩 이동하는 프로그램.
   
           (16MHz)/(1024 presclaer) = 15.625KHz
		   
            T = 1/(15.625KHz) = 64usec(펄스 하나를 셈하는 시간)
            64usec(펄스 하나를 셈하는 시간) x 157 = 10048usec := 10msec
            10msec x 100회 = 1000msec = 1sec
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

unsigned char count=0, data=0xFE;

void main(void)
{
         
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;    // LED off
	
	TCCR0=0x07;    // Normal mode(Timer/counte0 overflow), 1024 prescaler
    TCNT0=0xFF-157;// 10msec를 얻기위한 Timer/counte0 value(카운트 값)
	TIMSK=0x01;    // TOIE0(Timer/counter0 oveflow interrupt enable)
	               // 개별 인터럽트 인에이블
				   // 또는 TIMSK |= (1<<TOIE0); 를 사용하여도 된다.
  //  EICRA|=0x08;   // INT1 폴링(하강) 에지 
    TIFR|=0x01;     // TOV0 Timer/counter0 oveflow flag 클리어 
   
     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능
        
    while(1)
		 PORTB=data;
}           

#pragma vector=TIMER0_OVF_vect              // iom128.h를 참고한다. 
__interrupt void TIMER0_OVF_interrrupt(void)   
{
    __disable_interrupt();   
    	
	TCNT0=0xFF-157;// 10msec를 얻기위한 Timer/counte0 value(카운트 값)
    count++;
	if(count==100)    // 1초를 위한 카운트 횟수
	{
		data--;
        count=0;
	}		
    __enable_interrupt();
}
