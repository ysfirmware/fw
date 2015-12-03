/*******************************************************************************
파일명 :   Exam8_4.c

동 작  :   ADC1(PF1)핀에 CDS 센서를 연결한 후에, 그 결과 값을 포트 B에 
           연결된 LED에 출력하는 프로그램으로 AD 변환 완료 인터럽트를 이용한다.
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

unsigned char result=0xFF;

void m_delay(unsigned int m) // msec 딜레이 루틴
{
	unsigned int i, j; 
	                            
	for(i=0;i<m;i++)
	   for(j=0;j<215;j++);  //16MHz : 1msec              
}

void main( void )
{
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;    // LED off

    DDRF |= 0xFD;  // ADC1(PF1) 입력
	               // 또는 DDRF_Bit1=0;으로 설정하여도 된다.
   
    ADMUX=0x21;    // (00 1 00001) 외부 AREF, 좌 정렬,  채널 1
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 분주
                      
    ADCSR |= 0x08; // ADC Interrupt enable(개별 인터럽트 설정)

	ADCSR |= 0x40; //ADC start conversion
    

     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능
        
    while(1)  //대기
	  PORTB=~result;   // LED에 반전 출력
}         

#pragma vector=ADC_vect              // iom128.h를 참고한다. 
__interrupt void ADC_conversion_done(void)   
{
    __disable_interrupt();   
     
    result=ADCL;  // 좌측 정렬 후에 하위값 먼저 읽고
    result=ADCH;  // 상위값을 읽는다. 
                
    __enable_interrupt();
}

