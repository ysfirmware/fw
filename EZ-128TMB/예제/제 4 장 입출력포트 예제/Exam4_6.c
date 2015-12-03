/*******************************************************************************
파일명 :   Exam4_6.c
 
동 작  :  ATmega128의 PORTB에 연결된 LED가 PB0(LSB)에서 PB7(MSB) 방향으로, 다시
          PB7(MSB)에서 PB0(LSB) 방향으로 한 칸씩 이동하는 프로그램을 작성해 보자.


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
    unsigned char a, b, i;

    DDRB=0xFF;    // 출력으로 설정  
	PORTB=0xFF;   // LED가 연결된 포트

	while(1)
        {
          a=0xfe;
          b=0x7f;
          for(i=1;i<5;i++)
             {
              PORTB=a&b;
              m_delay(100);
              a=((a<<1)|0x01);
              b=((b>>1)|0x80);
             }
  
          a=0xf7;
          b=0xef;
          for(i=1;i<5;i++)
             {
              PORTB=a&b;   
              m_delay(100); 
              a=((a>>1)|0x80);
              b=((b<<1)|0x01);
             }
         }
}