/*******************************************************************************
파일명 :   Exam13_3.c
 
동 작  :   ADC0(PF0)에 연결된 가변저항을 조절해서 AD 변환결과 값이 200H보다 크면 
           DC 모터를 정방향으로 속도를 점점 증가하고, 200H보다 작으면 DC 모터를 
    	   역방향으로 속도를 점점 증가하는 프로그램으로,       
           OC1A(PB5)핀을 PWM1A 기능을 이용하여 PWM 값을 LCD 모듈에 출력하고
           DC 모터를 회전시킨다. 
		   여기서 사용하는 PWM 방법은 Phase Correct PWM 10bit를 이용한다.

           LCD 모듈을 PORTA에 연결하고 문자를 표시하는 프로그램이다.

           <LCD 회로 연결>
		   
           PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128의 PORTA에 연결된 LCD 모듈은 4비트 방식.

           <모터 회로 연결>

            PB7       PB6       PB5(OC1A)  <-  atmega128
             |         |          |
            enable    dir        PWM       <-  L298 DC Motor Driver
            출력      출력       출력 
           
		    <가변저항 연결>

			ADC0(PORTF.0) --> 가변저항 10K
                   
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

void lcd4hex(unsigned char b, unsigned int ch)
{
	unsigned char dch;
        
    instruction_out(b);
        
	dch = (ch>>8)&0xff;
	lcd2hex(dch);
	dch = ch&0xff;
	lcd2hex(ch);	
}



void dc_motor(unsigned char dir, unsigned int speed)
{	
    if(dir==1)           // 정방향
		PORTB |= 0x40;   // 모터 방향 DIR(정방향)
     else
        PORTB &= 0xBF;   // 모터 방향 DIR(역방향)

    OCR1A=speed;
}


void ADC_init(void) // ADC 초기화
{   
	DDRF |= 0xFE;  // ADC0(PF0) 입력
	               // 또는 DDRF_Bit0=0;으로 설정하여도 된다.
   
    ADMUX=0x00;    // (00 0 00000) 외부 AREF, 우측 정렬,  채널 0
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 분주
                      
    ADCSR |= 0x08; // ADC Interrupt enable(개별 인터럽트 설정)

	ADCSR |= 0x40; //ADC start conversion
    
}

unsigned int ad_result=0;

void main( void )
{
    DDRB |= 0xE0;    // 111* **** -> 출출출* ****
    PORTB=0xFF;
    
    init_LCD();      // LCD 초기화 함수 실행
    
	TCCR1A |= 0x83;  // (10** **11)
    TCCR1B |= 0x02;  // (***0 0010) Phase correct PWM 10bit, Non-inverting, 8 prescaler
                     // f=16MHz/(2*N*0x3FF), N=8 prescaler factor  
                     // f= 997Hz, T=1.003msec
				     // PWM의 주기는 1.003msec가 된다.
    TCCR1C=0x00;     // FOC1A=0
    
    TCNT1=0x0000;    // T/C1 value=0
    OCR1A=0x000;

    ADC_init();      // ADC 초기화   

    PORTB |= 0x80;   // Motor Enable
               
    string_out(0x84, "DC Motor");
        
    m_delay(20000);  // 1초 대기
    instruction_out(0x01);  //clear LCD
        
    instruction_out(0x84); // LCD 위치 지정
	char_out('H');
    instruction_out(0x8F); // LCD 위치 지정
	char_out('H');

    __enable_interrupt();  // Global interrupt enable
                           // 또는 _SEI(); 가능 
   

    while(1)
	{
		lcd4hex(0x80, ad_result);   // LCD 위치 지정
  	 		
        if(ad_result>0x200)
			{
			  dc_motor(1, ad_result);  //정회전
			              
	          lcd4hex(0x8B, ad_result);  // LCD 위치 지정              
              string_out(0xC5, "CW  UP"); // LCD의 두번째 라인
			}
		 else
			{
			  dc_motor(0, 0x3FF-ad_result);  //역회전
		               
	          lcd4hex(0x8B, 0x3FF-ad_result);  // LCD 위치 지정               
              string_out(0xC5, "CCW UP");  // LCD의 두번째 라인
			}
	}		
} 

#pragma vector=ADC_vect              // iom128.h 참고한다.
__interrupt void ADC_conversion_done(void)   
{
    __disable_interrupt();   
     
    ad_result=ADC;  // ADC 변환 결과 값을 읽어서 ad_result에 대입한다.         
    
    __enable_interrupt();
}
