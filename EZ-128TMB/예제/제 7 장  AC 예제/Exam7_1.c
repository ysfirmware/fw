/***************************************************************************
파일명 :   Exam7_1.c

동 작  :   + 핀인 AIN0와 - 핀인 AIN1의 전압을 비교해서 AIN0의 전압이 AIN1의 
          전압보다 높으면 ACSR(Analog Comparator Control & Status Register)의 
		  비트 3인 ACIE(Analog Comparator Interrupt Enable)를 "1"로 설정하고 
		  전역 인터럽트가 설정되어 있으면 개별 인터럽트가 발생된다.

                      
Device :  Atmega128-16AI
Clock  :  16MHz
Compiler: IAR AVR C Compiler V2.28A
***************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

#define ENABLE  (PORTA_Bit2=1)
#define DISABLE (PORTA_Bit2=0) 

//#define ENABLE  (PORTA |=  0x04)  // LCD ENable 또는 PORTA_Bit2=1;
//#define DISABLE (PORTA &= 0xFB)   // LCD DIsable 또는 PORTA_Bit2=0;

void init_PORTA(void)
{
	DDRA = 0xFF;    // PORTA 출력
	PORTA=0xFF;     // 초기 값
}

/*void wait_busy_flag(void)  
  {                         
     unsigned char busy_flag; 
     
     PORTA|=0x06;    //PA2(EN)=1  PA1(R/-W)=1  PA0(RS)=0
     DDRA &=0x7F;    //PORTA7 입력 
          
     do{
	     busy_flag = PINA; 
	    }while( busy_flag & 0x80); 

     DDRA|=0x80; 
     PORTA=0xFF;	 
 }*/  
 
void wait_busy_flag(void)
{
	unsigned int i;
	
	for(i=0;i<10000;i++);
} 

  
void m_delay(unsigned int m) //딜레이 루틴
{
	unsigned int i, j;

	for(i=0;i<m;i++)
	   for(j=0;j<2650;j++);  //16MHz : 1msec 
}

void ddelay(unsigned int p)
{
	unsigned int i;

	for(i=0;i<p;i++);
}

void instruction_out(unsigned char b)//명령어 쓰기 함수
{
	PORTA=b&0xF0;                    //상위 4BIT 출력 

	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	PORTA=(b<<4)&0xF0;               //하위 4BIT 출력
	
	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	//명령어 수행이 완료될때까지 지연
	m_delay(2);

}


void char_out(unsigned char b)  //LCD에 한문자 출력 함수
{
   
	PORTA=(b&0xF0)|0x01;        //상위 4BIT 출력
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE 
	
	PORTA=((b<<4)&0xF0)|0x01;   //하위 4BIT 출력
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE

   //명령어 수행이 완료될때까지 지연
    m_delay(2);
}


void string_out(unsigned char b, unsigned char *str) //문자열 출력 함수
{
	unsigned int i=0;
	
	instruction_out(b);   //LCD 위치 지정
	
	do{
	    char_out(str[i]);
	   }while(str[++i]!='\0');   // NULL 문자를 만날 때 까지
}


void init_LCD(void)                //LCD초기화 루틴
{   
	init_PORTA();    // LCD를 연결한 포트 초기화

  	m_delay(15);                         
	            
	ddelay(600);			  
	instruction_out(0x28);   //LCD FUNCTION SET(16X2 LINE, 4 BIT, 5X8 DOT)            
    m_delay(2); 
    instruction_out(0x28);   //LCD FUNCTION SET(16X2 LINE, 4 BIT, 5X8 DOT)    
	m_delay(2); 
	instruction_out(0x0C);   //LCD DISPLAY ON, CURSOR OFF, BLINK OFF 
	instruction_out(0x06);   //LCD ENTRY MODE SET
	instruction_out(0x02);   //RETURN HOME
	instruction_out(0x01);   //LCD CLEAR
	instruction_out(0x01);   //LCD CLEAR 
}


unsigned char led=0x00;

void main(void)
{
	DDRE &= 0xF3;  // PE.3, PE.2 입력
	   
	init_LCD();    // 포트 A에 연결된 LCD를 초기화 한다.

	
	ACSR &= 0x7F;    // Analog Comparator Enable
	ACSR |= 0x08;    // ACIE, Toggle mode
		
    m_delay(100); 

	string_out(0x80, "Compare!");

	 __enable_interrupt();

	while(1)
	{
	  if(led==0x00)
	    string_out(0xC2, "Low ");
	  else
	    string_out(0xC2, "High");
	}
}

#pragma vector=ANA_COMP_vect              // iom128.h 참고한다.
__interrupt void analog_comparator(void)   
{
	__disable_interrupt(); 
	
	led^=0xff;   // 입터럽트가 발생할 때 마다 토글한다.

	__enable_interrupt();
 }