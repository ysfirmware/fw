/*******************************************************************************
파일명 :   Exam6_7.c
 
동 작  :   8bit Timer/counter2의 Overflow interrupt를 이용하여 일정 시간마다 
           (1 sec) 포트 A에 연결된 LCD에 카운트하는 이동하는 프로그램.
   
           (16MHz)/(1024 presclaer) = 15.625KHz
		   
            T = 1/(15.625KHz) = 64usec(펄스 하나를 셈하는 시간)
            64usec(펄스 하나를 셈하는 시간) x 157 = 10048usec := 10msec
            10msec x 100회 = 1000sec = 1sec
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)

unsigned char count=0, data=0;

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
	
    init_LCD();

	TCCR2=0x05;    // Normal mode(Timer/counte2 overflow), 1024 prescaler
    TCNT2=0xFF-157;// 10msec를 얻기위한 Timer/counte0 value(카운트 값)
	TIMSK=0x40;    // TOIE2(Timer/counter0 oveflow interrupt enable)
	               // 개별 인터럽트 인에이블
				   // 또는 TIMSK |= (1<<TOIE2); 를 사용하여도 된다.
    TIFR=0x40;     // TOV2(Timer/counter2 oveflow flag) 클리어 
   
    instruction_out(0x01);   // LCD clear
	string_out(0x80, "T/C2 OVF");	
   
     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능
        
    while(1)
       lcd_decimal(0xC2, data);   // LCD 위치 지정   
}            

#pragma vector=TIMER2_OVF_vect              // iom128.h를 참고한다. 
__interrupt void TIMER2_OVF_interrrupt(void)   
{
    __disable_interrupt();   
    	
	TCNT2=0xFF-157;// 10msec를 얻기위한 Timer/counte0 value(카운트 값)
    count++;
	if(count==100)    // 1초를 위한 카운트 횟수
	{   
		data++;    // data 증가
		count=0;

	    PORTB=~data;
	}		
    __enable_interrupt();
}

