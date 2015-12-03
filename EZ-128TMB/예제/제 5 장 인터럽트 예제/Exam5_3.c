/*******************************************************************************
파일명 :   Exam5_3.c
 
동 작  :   INT4(PE4)핀에 스위치를 연결한 후에 스위치를 누르면 not gate를 통하여
           즉, 상승 에지에서 인터럽트를 발생하도록 설정한 후에,
		   인터럽트가 발생하면 포트 B에 연결된 LED를 하나씩 이동하는
		   프로그램을 작성한다.
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

unsigned int data=0xFE;

void main(void)
{
         
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;    // LED off

	DDRE_Bit4=0;   // PE4 입력 DDRE4 비트는 초기값이 0으로 설정되어 있기 때문에
	               // "DDRE_Bit4=0;"는 생략 가능하다. INT4(PE4)핀
				   // DDRE&=0xEF;

    EIMSK|=0x10;   // INT4 개별 인터럽트 인에이블
    EICRB|=0x03;   // INT4 상승 에지 
    EIFR|=0x10;     // INTF4 클리어 
   
     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능
        
    while(1);    
}           

#pragma vector=INT4_vect              // iom128.h를 참고한다. 
__interrupt void INT4_interrrupt(void)   
{
    __disable_interrupt();   
     
    PORTB=data;
	
	data--;
            
    __enable_interrupt();
}

