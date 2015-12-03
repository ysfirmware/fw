/*******************************************************************************
파일명 :   Exam5_2.c
 
동 작  :   INT0(PD0)핀에 스위치를 연결한 후에 스위치를 누르면 not gate를 통하여
           즉, 상승 에지에서 인터럽트를 발생하도록 설정한 후에,
		   인터럽트가 발생하면 LCD 모듈에 숫자를 카운트하는 프로그램을 작성한다.
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

unsigned int data=0x00;   //전역 변수 설정

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

void lcd_decimal(unsigned char b, unsigned int hex)
{
	unsigned int h;
	
	instruction_out(b);
	
	h = (hex / 1000);
	
	if(h != 0)
		char_out(h + 0x30);
 	 else
	   	char_out(0x30);
		
	h = ((hex % 1000) / 100);
	
	if(h != 0)
		char_out(h + 0x30);
	  else
		char_out(0x30);
		
	h = (((hex % 1000) % 100) /10);
	
	if(h != 0)
		char_out(h + 0x30);
	  else
		char_out(0x30);
		
	h= (((hex % 1000) % 100) % 10);
	
	if(h != 0)
		char_out(h + 0x30);
	  else
		char_out(0x30);
}


void main(void)
{
         
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;    // LED off

	DDRD_Bit0=0;   // PD0 입력 DDRD 레지스터는 초기값이 0으로 설정되어 있기 때문에
	               // "DDRD_Bit0=0;"는 생략 가능하다. INT0(PD0)핀
				   // DDRD&=0xFE;
	        
	init_LCD();    // LCD 초기화
   
    EIMSK|=0x01;   // INT0 개별 인터럽트 인에이블
    EICRA|=0x03;   // INT0 상승 에지 
    EIFR|=0x01;     // INTF0 클리어 
   
    instruction_out(0x01);   // LCD clear
	string_out(0x80, "EXTINT0!");
   
     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능
        
    while(1)
       lcd_decimal(0xC2, data);   // LCD 위치 지정   
}           

#pragma vector=INT0_vect              // iom128.h를 참고한다. 
 __interrupt void INT0_interrrupt(void)   
{
    __disable_interrupt();   
     
    data++;    // data 증가
            
    __enable_interrupt();
}
