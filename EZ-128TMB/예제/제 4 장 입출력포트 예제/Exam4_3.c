/*******************************************************************************
파일명 :   Exam4_3.c
 
동 작  :    ATmega128의 PORTB에 연결된 LED가 PB0(LSB)에서 PB7(MSB) 방향으로
           한 칸씩 이동하는 프로그램을 작성해 보자.

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
	   for(j=0;j<5;j++);  //16MHz : 1msec 2650 

	__enable_interrupt();
}


void main(void)
{       
    unsigned char number=0xFF;
    
	DDRB=0xFF;    // 출력으로 설정  
	PORTB=0xFF;   // LED가 연결된 포트
    
	do{
	    PORTB = number;	// 변수에 저장된 LED초기값을 PORTB에 출력 
	    number = (number << 1) | 0x01; 
	      // 변수값을 왼쪽으로 한칸 shift한뒤 0x01값과 비교하여 최하위비트의 LED를 off 
	    if((number & 0xff) == 0xff)	 
		   number = 0xfe;		
	     // 만약 변수의값이 0xff와 비교하여 그 값이 0xff이면(8개의 비트가 모두 shift되면)
		 // 변수에 LED초기값을 다시 저장 
	    m_delay(2);		// 일정시간 지연 
	}while(1);		// 무한루프 
} 
         