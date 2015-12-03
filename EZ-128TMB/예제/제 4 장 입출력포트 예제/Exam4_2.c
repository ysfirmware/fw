/*******************************************************************************
파일명 :   Exam4_2.c
 
동 작  :   ATmega128의 PD0와 PE4에 KEY(푸쉬 스위치)가 연결되어 있다. 
           이들 KEY 입력을 받아 다음의 동작을 제어해 보자.
           이 스위치를 눌렀다가 떼는 시점에서 동작
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)


void PD0_key(void)  // 눌렀다가 뗴면 동작한다.
{
    //while((PIND & 0x01) == 0x00);  // PD0을 누르는 동안
	//while((PIND & 0x01) != 0x00);  // PD0을 떼면
    // 아래와 같은 동작을 한다.

	while(!(PIND & 0x01));  // PD0를 누르는 동안
	while(PIND & 0x01);     // PD0를 떼면
}

void PE4_key(void)  // 눌렀다가 뗴면 동작한다.
{
    while((PINE & 0x10) == 0x00);  // PE4을 누르는 동안
	while((PINE & 0x10) != 0x00);  // PE4을 떼면
    // 아래와 같은 동작을 한다.

	//while(!(PINE & 0x10));  // PE4를 누르는 동안
	//while(PINE & 0x10);     // PE4를 떼면
}

void main(void)
{
    unsigned char number=0xFE;

	DDRB=0xFF;
	PORTB=0xFF;    // PORTB_Bit3=1;   // CodeVisionAVR에서 비트 단위 프로그램하기 PORTB.3=1;
                   // PORTB=1<<3;
        
	DDRD &=0xFE;   // PD0 입력(INT0) // DDRD |= (0<<DDD4) | (0<<DDD0); 처럼 사용하도 된다.
	PORTD=0x00;    // 외부 풀업저항 사용

	DDRE &=0xEF;   // PE4 입력으로사용(INT4)
	PORTE=0x00;    // 외부 풀업저항 사용


    PD0_key();     // PD0	스위치를 누를때까지 대기
	               // 눌렀다가 뗴면 동작한다.
	
	do{  
	     PE4_key();   // PE4 스위치를 누를때까지
		              // 눌렀다가 떼는 시점에서 동작
  
	     PORTB = number;  // 변수에 저장된 LED초기값을 포트 B에 출력 
	     number = (number << 1) | 0x01; 
	      // 변수값을 왼쪽으로 한칸 shift한뒤 0x01값과 비교하여 최하위비트의 LED를 off 
	     if((number & 0xFF) == 0xFF)	 
		   number = 0xFE;		
	     // 만약 변수의값이 0xff와 비교하여 그 값이 0xFF이면(8개의 비트가 모두 shift되면)
		 // 변수에 LED초기값을 다시 저장 	   		
	  }while(1);
}

