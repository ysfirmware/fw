/*******************************************************************************
파일명 :   Exam8_1.c

동 작  :   ADC0(PF0)핀에 가변저항 10K를 연결한 후에, 그 결과 값을 포트 B에 
           연결된 LED에 출력하는 프로그램으로 폴링(Polling) 방법으로 작성한다.
           ADC Free Runing Select 비트를 "1"로 설정한 경우와
		   ADC Free Runing Select 비트를 "0"으로 설정한 경우를 비교하여 보자.

Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

void m_delay(unsigned int m) //딜레이 루틴
{
	unsigned int i, j; 
	                       
    __disable_interrupt();	
	  
	for(i=0;i<m;i++)
	   for(j=0;j<2650;j++);  //16MHz : 1msec  

	__enable_interrupt();
}


void main( void )
{
    unsigned char result;     
		 
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;    // LED off

    DDRF &= 0xFE;  // ADC0(PF0) 입력
	               // 또는 DDRF_Bit0=0;으로 설정하여도 된다.
   
    ADMUX=0x20;    // (00 1 00000) 외부 AREF, 좌 정렬,  채널 0
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 분주
                      
    ADCSR |= 0x40; //ADC start conversion

    do{
         m_delay(1);   // 1msec 대기
         
         while((ADCSR & 0x10)==0);   //변환이 완료될 때까지 대기
		 							 // 즉, ADIF == 1 ?
                                     //while(!(ADCSR & 0x10)); 와 동일
         ADCSR |= 0x10;              // ADIF를 "1"로 클리어한다.

         result=ADCL;  // 좌측 정렬 후에 하위값 먼저 읽고
         result=ADCH;  // 상위값을 읽는다.
                  
         PORTB=~result;   // LED에 반전 출력

	     m_delay(1);   // 1msec 대기
       }while(1);    
}           


