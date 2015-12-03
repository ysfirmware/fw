/*******************************************************************************
파일명 :   Exam6_9.c
 
동 작  :   Atmega128의 ICP1/PD4에 KEY(푸쉬 스위치)가 연결되어 있다. 이 KEY 입력을 받아
           Timer/counter1의 Input Capture1으로 동작하게끔 설정하고 하강 에지에서 
		   입력받아 저장된 시간값(ICR1)을 포트 A에 연결된 LCD Module에 16진수 
		   값으로 출력하는 프로그램으로, Timer/Counter1 Input Capture Interrupt를
		   이용하여 인터럽트로 처리한다.
      
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

unsigned int cap_time=0;

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


void lcd2hex(unsigned char ch)
{
	unsigned char data=0x00, dch;

    dch = (ch>>4)&0x0f;
	if( dch <= 9 ) data = '0'+dch; 
	else data = 'A'+(dch-10); 
	char_out(data);

	dch = ch&0x0f;
	if( dch <= 9 ) data = '0'+dch; 
	else data = 'A'+(dch-10); 
	char_out(data);
}


void lcd4hex(unsigned char b, unsigned int ch)
{
	unsigned char dch;
        
    instruction_out(b);
        
	dch = (ch>>8)&0xff;
	lcd2hex(dch);
	dch = ch&0xff;
	lcd2hex(ch);	
}


void main(void)
{       
	DDRD=0xEF;   // 또는 DDRD_Bit4=0; 으로 사용하여도 된다.
	PORTD=0x00;

	TCCR1A=0x00;
	TCCR1B=0x85;  // 1024 Prescaler, ICP1(PD4)의 하강에지에서 입력
	              // Input Capture Noise Cancler 실행
    TCCR1C=0x00;
	TCNT1=0x0000;  // 초기값을 클리어한다.
    
	TIMSK=0x20;  // TICIE1(Timer/Counter1 Input Capture Interrupt Enable) 설정 
	             // 개별 인터럽트 설정
    TIFR|=0x20;  // ICF1(Timer/Counter1 Input Capture Flag) 클리어

    init_LCD();

    instruction_out(0x01);   // LCD를 지운다.
	string_out(0x80, "  ICP1  ");  // LCD의 첫번째 라인   
    instruction_out(0xC6);  //LCD 위치 지정
	char_out('H');
    __enable_interrupt();    // Global interrupt enable
                             // 또는 _SEI(); 가능
    while(1)
	   lcd4hex(0xC2, cap_time);  // LCD에 출력한다.
}

#pragma vector=TIMER1_CAPT_vect              // iom128.h를 참고한다. 
 __interrupt void TIMER1_CAPT_vect_interrrupt(void)   
{
    __disable_interrupt();   
      
    cap_time=ICR1;         // 캡쳐된 시간값(ICR1)을 cap_time에 저장한다.
            
    __enable_interrupt();
}
