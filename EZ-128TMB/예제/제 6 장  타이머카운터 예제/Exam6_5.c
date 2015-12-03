/*******************************************************************************
파일명 :   Exam6_5.c
 
동 작  :   OC0(PB4)핀을 CTC 기능을 이용하여 주기적으로 LED에 
           토글 값을 출력한다. 그러나 인터럽트를 이용하여 주기를 조절하는.
 
           주파수 = (16MHz)/(1024 Prescaler) = 15625 Hz
		   T = 1/15625 = 0.000064 sec = 64 usec
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

unsigned char value=1;


void OC0_CTC(unsigned char duty)
{
	OCR0=duty;
} 

void main( void )
{       
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;
   
    DDRB_Bit4 = 1; // PORTB.4를 OC0로 설정한다.

    TCCR0=0x1F;    // (0001 1111) CTC 모드, 토글, 1024 prescaler
    TCNT0=0x00;    // 타이머0 초기값
    
    OC0_CTC(0x50); // OCR0와 TCNT0가 같으면 OC0 핀 토글 
                   // 즉 토글의 주기는 0x00~0x30까지가 된다. 

    TIFR |= 0x02;  // OUTPUT COMPARE FLAG 0 클리어
    TIMSK=0x02;    // OUTPUT COMPARE 0 interrupt MASK
    
     __enable_interrupt();    // Global interrupt enable

    while(1);    
} 


#pragma vector=TIMER0_COMP_vect         // iom128.h를 참고한다.
__interrupt void TIMER0_COMP(void)   
{
    __disable_interrupt();   
     
    TCNT0=0x00; 
        
    if(value == 1)
      {
         OC0_CTC(0xE0);  // Ton = 0xE0(224) X 64usec = 14.336msec
	     value=0;
       }	      
     else
	  {
	     OC0_CTC(0x50); // Toff = 0x50(80) X 64usec = 5.120msec              
         value=1;
      }
             
    __enable_interrupt();
}
    
