/*******************************************************************************
파일명 :   Exam10_1.c
 
동 작  :  0x00(0)~0xFF(255)에 해당하는 ASCII CODE 값을 포트 B에 연결된 LED로 
          출력하고, 포트 A에 연결된 LCD 모듈에 그에 해당하는 HEX 값으로 표시하며,
		  그리고 IBM PC로 ASCII CODE 값을 모니터에 표시하는 프로그램이다.
		  반복 동작을 하므로 PORTD.0 핀의 스위치를 눌러아먄 동작을 하며, 이 떄
		  보율은 9600BPS로 설정하였다. 프로그램 동작은 폴링 방법으로 작성하였다.

        *  PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128의 PORTA에 연결된 LCD 모듈은 4비트 방식을 사용한다.

        *  PE0 -> RXD
		   PE1 -> TXD
           
           16MHz에서 9600 bps


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

void init_SERIAL(void)   // 시리얼 함수 초기화
{

     DDRE=0xFE;     // PE1(TXD) 출력(1), PE0(RXD) 입력(0)

     UCSR0A=0x00;
     UCSR0B=0x18;   // Receive enable, Transmitte enable
     UCSR0C=0x06;   // 비동기 방식, No parity bit, 1 stop bit   

     UBRR0H=0x00;
     UBRR0L=0x67;   // 16MHz에서 9600 bps  (10진수 103)
}

// 시리얼 포트로 한문자를 송신한다. 
void putch(unsigned char data)
{
	while(!(UCSR0A & 0x20));
	UDR0=data;
    
	UCSR0A |= 0x20;         // 클리어 UDRE0
}

void PD0_key(void)  // 눌렀다가 뗴면 동작한다.
{
    //while((PIND & 0x01) == 0x00);  // PD0을 누르는 동안
	//while((PIND & 0x01) != 0x00);  // PD0을 떼면
    // 아래와 같은 동작을 한다.

	while(!(PIND & 0x01));  // PD0를 누르는 동안
	while(PIND & 0x01);     // PD0를 떼면
}

void main(void)
{   
    unsigned char i;	

	DDRB=0xFF;     // PORTB 출력으로 지정
	PORTB=0xFF;    // PORTB_Bit3=1;   // CodeVisionAVR에서 비트 단위 프로그램하기 PORTB.3=1;
                   // PORTB=1<<3;
    
	init_SERIAL();  //LCD 초기화
    init_LCD();
    
           

    instruction_out(0x01);     // LCD clear	
    string_out(0x81, "USART0");
	instruction_out(0xC5);    //LCD 위치 지정
	char_out('H');
      
    PD0_key(); // PD.0 KEY를 누를때까지 대기  
         
       
	while(1)
	    {
	      for(i=0;i<256;i++)
	         {
			   PORTB=~i;  // PORTB에 연결된 LED로 출력한다.
	           putch(i);  // IBM PC로 출력한다.

               instruction_out(0xC3);  //LCD 위치 지정
	           lcd2hex(i);

               m_delay(100);  
             }
         }
}