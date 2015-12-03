/*******************************************************************************
파일명 :   Exam8_3.c

동 작  :   ADC0(PF0)핀에 가변저항 10K를 연결한 후에, 그 결과 값을 포트 A에 
           연결된 LCD Module에 16진수 값으로 출력하는 프로그램으로 AD 변환 완료 
		   인터럽트를 이용한다.
           
Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)


unsigned int result=0x0000;

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

void main( void )
{
    DDRB=0xFF;     // 포트 B 출력 
    PORTB=0xFF;    // LED off

    DDRF |= 0xFE;  // ADC0(PF0) 입력
	               // 또는 DDRF_Bit0=0;으로 설정하여도 된다.
   
    ADMUX=0x00;    // (00 0 00000) 외부 AREF, 우측 정렬,  채널 0
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 분주
                      
    ADCSR |= 0x08; // ADC Interrupt enable(개별 인터럽트 설정)

	ADCSR |= 0x40; //ADC start conversion
    
    init_LCD();
	string_out(0x80, "ADC0->VR");
	instruction_out(0xC5);  //LCD 위치 지정
	char_out('H');

     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능
        
    while(1)  //대기
	    lcd4hex(0xC1, result);  // LCD 위치 지정과 2 바이트 헥사값 출력
}         

#pragma vector=ADC_vect              // iom128.h를 참고한다. 
__interrupt void ADC_conversion_done(void)   
{
    __disable_interrupt();        
      
    result=ADC;            // ADC 변환 결과 값(ADC)를 읽어 result에 저장한다.
	
    __enable_interrupt();
}

