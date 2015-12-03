/*******************************************************************************
파일명 :   Exam4_1.c
 
동 작  :   Atmega128의 PD0에 KEY(푸쉬 스위치)가 연결되어 있다. 이 KEY 입력을 받아
           다음의 동작을 제어해 보자. 상승 에지, 누르는 시점에서 동작.

Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)


void main(void)
{
    unsigned char number=0xFE;

	DDRB=0xFF;     
	PORTB=0xFF;   // LED가 연결된 포트

	DDRD&=0xFE;    // PD0 입력, 나머지 출력
	PORTD=0x00;    //외부 풀업 저항 사용
   
	do{      
		while(PIND & 0x01);     // PD0을 누르는 시점에서 동작
		while(!(PIND & 0x01));  // PD0을 떼면
		     
	    PORTB = number;	// 변수에 저장된 LED초기값을 PORTB에 출력 
	    number = (number << 1) | 0x01; 
	      // 변수값을 왼쪽으로 한칸 shift한뒤 0x01값과 비교하여 최하위비트의 LED를 off 
	    if((number & 0xff) == 0xff)	 
		   number = 0xfe;		
	     // 만약 변수의값이 0xff와 비교하여 그 값이 0xFF이면(8개의 비트가 모두 shift되면)
		 // 변수에 LED초기값을 다시 저장     
		     
      }while(1);
}

