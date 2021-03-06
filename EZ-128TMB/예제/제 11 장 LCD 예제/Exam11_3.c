/*******************************************************************************
파일명 :   Exam12_3.c
 
동 작  :   LCD 모듈에 문자를 표시하는 기본 프로그램으로 LCD 모듈에 시간을 
           표시하는 프로그램.

           PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128의 PORTA에 연결된 LCD 모듈은 4비트 방식을 사용한다.


Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

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

void main(void)
{
    unsigned int a, hour, min, sec;

    init_LCD();

    instruction_out(0x01);   // LCD를 지운다.
	
    string_out(0x80, "CLOCK");  // LCD의 첫번째 라인
   		
    instruction_out(0xC2);             //LCD에 문자 출력 위치 지정
    char_out(':');                     //문자 출력 
    instruction_out(0xC5);             //LCD에 문자 출력 위치 지정
    char_out(':');                     //문자 출력 

                   
   while(1)                           //무한 루프
    {
    for(a=0;a<2;a++)                  //AM,PM 변환루프
      {
     for(hour=0;hour<12;hour++)       //시간 변환 루프       
        {
       for(min=0;min<60;min++)        //분 변환 루프
         {
         for(sec=0;sec<60;sec++)      //초 변환 루프
           {
           if(a==0)              
             string_out(0x86, "AM");  //a가 0일 경우 AM을 출력              
           else                
             string_out(0x86, "PM");  //a가 1일 경우 PM을 출력
              
           instruction_out(0xC0); char_out('0'+hour/10);  //시간의 10^1 출력   
           instruction_out(0xC1); char_out('0'+hour%10);  //시간의 10^0 출력
           instruction_out(0xC3); char_out('0'+min/10);   //분의 10^1 출력
           instruction_out(0xC4); char_out('0'+min%10);   //분의 10^0 출력
           instruction_out(0xC6); char_out('0'+sec/10);   //초의 10^1 출력
           instruction_out(0xC7); char_out('0'+sec%10);   //초의 10^0 출력
                           
           m_delay(1000);     
            }
          }
        }
      }
   }
} 




    