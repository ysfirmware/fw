/*******************************************************************************
파일명 :   Exam12_5.c
 
동 작  :   LCD 모듈에 문자를 표시하는 기본 프로그램으로 LCD 모듈에 하트 모양
           과 문자의 이동을 표시하는 프로그램.

           PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	       D7   D6   D5   D4    X    EN  R/-W  RS  <--- LCD Module
           
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

void heart_cgram(void)
{
   unsigned int i;
   unsigned char heart[]={0x00,0x0a,0x15,0x11,0x11,0x0a,0x04,0x00};  // 하트 모양
                       
   instruction_out(0x40);
   for(i=0; i<8; i++)
   {
     char_out(heart[i]);
   }
}

void main(void)
{
    int ch, i, j;
    unsigned char sudo[]={"Micro!!!"};	
   
	init_LCD();

    instruction_out(0x01);   // LCD를 지운다.
	heart_cgram();

    while(1)
    {
     for(ch=0,i=8;ch<86,i>0;ch++,--i)
     {
      for(j=0;j<i;j++)
      {
        instruction_out(0x87-j);   char_out(sudo[ch]);
        instruction_out(0x87-j+1); char_out(' ');
        instruction_out(0xC0+j);   char_out(sudo[i-1]);
        instruction_out(0xC0+j-1); char_out(' ');
        m_delay(100);
      }
     }
 
    for(i=0;i<10;i++)   //10회 반복
    {
      for(j=0;j<8;j++)
      {
       instruction_out(0x80+j);   char_out(0x00);
       instruction_out(0x80+j-1); char_out(' ');
       instruction_out(0xC7-j);   char_out(0x00);
       instruction_out(0xC7-j+1); char_out(' ');
       m_delay(100);
      }
     }
  
    for(i=0;i<10;i++)    //10회 반복
    {
      for(j=0;j<8;j++)
      {
       instruction_out(0x87-j);   char_out(0x00);
       instruction_out(0x87-j+1); char_out(' ');
       instruction_out(0xC0+j);   char_out(0x00);
       instruction_out(0xC0+j-1); char_out(' ');
       m_delay(100);
      }
    }
  }
}
