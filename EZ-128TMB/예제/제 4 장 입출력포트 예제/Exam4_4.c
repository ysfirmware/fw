/*******************************************************************************
파일명 :   Exam4_4.c
 
동 작  :  ATmega128의 PORTB에 연결된 LED가 PB0에서 PB3, PB4에서 PB7가 각각 4비트씩
          모였다가 퍼지는 프로그램을 작성해 보자.

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


void main(void)
{ 

    unsigned int h, j, m;   // 지역변수선언 
 	h = 0xFE;
	j = 0x7F;		       // 각각의 변수에 LED초기값을 저장 

	DDRB=0xFF;    // 출력으로 설정     
	PORTB=0xFF;   // LED가 연결된 포트
   
	
	do{
     	m = h & j;		    // h와 j변수에 저장된 값을 and비교하여 m변수에 저장
	    PORTB = m;		    // 위에서 비교하여 저장한 변수m값을 PORTB에 출력 
	    m_delay(100);		// 일정시간 지연 

		if((h==0x7f)||(j==0xfe))	// 조건문 (만약 변수 h와 j가 각각의 조건값과 같다면) 
	  	 { 
		   h = 0xfe; 
		   j = 0x7f;		// 변수들의 값을 초기화 
		 } 
		else				// 위의 조건이 아니면 
		  {
		   h = ((h << 1) | 0x01); 
		   j = ((j >> 1) | 0x80);	
		  }                     //각각의 변수들의 값을 좌 또는 우로 shift하여 
								// 최상위 또는 최하위비트값을 or비교에 의해 LED off 
	   }while(1);		// 무한루프 
}              