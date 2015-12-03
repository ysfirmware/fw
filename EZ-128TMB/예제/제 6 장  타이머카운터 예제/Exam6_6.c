/*******************************************************************************
파일명 :   Exam6_6.c
 
동 작  :   OC0(PB4)핀을 PWM0 기능을 이용하여 PWM 폭을 LED(PORTB.4)에 나타내고,
           PWM0 출력 값을 LCD 모듈에 출력하는 프로그램으로, LCD 모듈을 PORTA에 
		   연결하고 문자를 표시하는 프로그램이다.
          
		   <LCD 회로 연결>
		   
           PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128의 PORTA에 연결된 LCD 모듈은 4비트 방식.

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


void lcd2hex(unsigned char ch)
{
	unsigned char ddata=0x00, dch;

    dch = (ch>>4)&0x0f;
	if( dch <= 9 ) ddata = '0'+dch; 
	else ddata = 'A'+(dch-10); 
	char_out(ddata);

	dch = ch&0x0f;
	if( dch <= 9 ) ddata = '0'+dch; 
	else ddata = 'A'+(dch-10); 
	char_out(ddata);
}


void pwm0_out(unsigned char duty)
{
	OCR0=duty;
} 


void main( void )
{
    unsigned char number;
     
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;
   
    DDRB_Bit4 = 1; // PORTB.4를 OC0로 설정한다.

    init_LCD();    //LCD 초기화 함수 실행
    
    TCCR0=0x6A;    // (0110 1010) Fast PWM, Non-inverting, 8 prescaler
                   // f=16MHz/(N*256), N=8 prescaler factor  
                   // f= 7.8125KHz, T=128usec
				   // PWM의 주기는 128usec가 된다.
    
    TCNT0=0x00;    // T/C value=0
	OCR0=0x00;    
    
    m_delay(10);  // 10msec 대기
    instruction_out(0x01);  //clear LCD
    
    string_out(0x80,"PWM0/OC0");
	instruction_out(0xC5);   // LCD 위치 지정
	char_out('H');
   
    while(1)
    { 
       for(number=0;number<256;number++)
       {  
          pwm0_out(number);  // OC0 핀(PB4)에 출력
          
          instruction_out(0xC3); 
          lcd2hex(number);
          m_delay(100);
        }
    }
} 

