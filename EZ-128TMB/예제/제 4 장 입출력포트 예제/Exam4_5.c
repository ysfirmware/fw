/*******************************************************************************
파일명 :   Exam4_5.c
 
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
    
    
    unsigned char L_shift, R_shift, i;

    DDRB=0xFF;    // 출력으로 설정  
	PORTB=0xFF;   // LED가 연결된 포트 LED OFF
   
	
	while(1)                           //무한 루프
     {
      L_shift = 0x01;                  // L_shift에 00000001B를 대입                 
      R_shift = 0x80;                  // R_shfit에 10000000B를 대입

      for(i=0 ; i<8 ; i++)             // 루프를 8번 수행 
        {         
          PORTB = L_shift;             // 포트1에 L_shfit값 대입    
          L_shift = L_shift << 1;      // L_shfit값을 1칸 shfit left시킴
          m_delay(100);                // 시간지연
        }

      for(i=0 ; i<8 ; i++)             // 루프를 8번 수행 
        {          
          PORTB = R_shift;             // 포트B에 R_shfit값 대입   
          R_shift = R_shift >> 1;      // R_shfit값을 1칸 shfit right시킴 
          m_delay(100);                // 시간지연
        }
    }
}