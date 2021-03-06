/*******************************************************************************
파일명 :   Exam13_2.c
 
동 작  :   OC1A(PB5)핀을 PWM1A 기능을 이용하여 PWM 값을 LCD 모듈에 출력하고
           DC 모터를 회전시킨다. 
		   여기서 사용하는 PWM 방법은 Phase Correct PWM 10bit를 이용한다.

          모터에 연결된 엔코더에서 회전량을 1초마다 LCD 모듈에 출력한다. 
		  즉, DC 모터가 회전할때의 회전량인 RPS(Revolution Per Second)를 
		  LCD 모듈에 출력한다. 이 엔코더는 PE6(T3)에 접속하여 Time/Counter 1을 
		  카운터로 동작시켜 카운트한다.

           LCD 모듈을 PORTA에 연결하고 문자를 표시하는 기본 프로그램으로 
           이와 동시에 PORTB에 연결된 LED를 좌로 이동하는 프로그램.

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





void pwm1A_out(unsigned int duty)
{
	OCR1A=duty;
} 


void timer3_init(void)  // PE6 -> T3로 사용하여 모터의 엔코더 펄스를 카운트 한다.
{
	TCCR3B=0x07;        // Timer/Counter3를 카운터3으로 사용.(상승에지)
	
	TCNT3=0x0000;   
}


void timer2_init(void)  // 1초를 만들기 위해서 사용한다.
{
	TCCR2 = 0x05;   // 1024분주
	                // f = 16MHz/1024분주 = 15.625KHz , T= 64usec
					// 64usec X 156 = 10msec
					// 255(0xFF) - 156(0x9C) = 99(0x63)

    TCNT2 = 0x63;   // 10msec를 만드는 값 즉, 0.01초
}

unsigned char count=0;
unsigned int temp;

void main( void )
{
    unsigned int number;
     
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

    timer2_init();   // 1초를 만들기 위해서 사용한다.
	timer3_init();   // PE6 -> T3로 사용하여 모터의 엔코더 펄스를 카운트 한다.


    PORTB |= 0x80;   // Motor Enable
    PORTB |= 0x40;   // Motor dir=1(정방향)
	
    TIFR |= 0x40;    // TOV2 클리어
	TIMSK = 0x40;    // Timer/Counter2 Overflow Interrupt Enable 


    string_out(0x84, "DC Motor");
        
    m_delay(20000);  // 2초 대기
    instruction_out(0x01);  //clear LCD
    
    string_out(0x80, "PWM1A/OC1A - RPS");
   
     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능 
   

    while(1)
    {        
       for(number=0x200;number<0x3FF;number++)
       {  
          pwm1A_out(number);  // OC1A 핀(PB5)에 출력
          
          lcd4hex(0xC0, number);
          m_delay(500);
        }
         
       for(number=0x3FF;number>0x200;number--)
       {  
          pwm1A_out(number);  // OC1A 핀(PB5)에 출력
          
          lcd4hex(0xC0, number);
          m_delay(500);
        }
    }
} 

#pragma vector=TIMER2_OVF_vect       // iom128.h를 참고한다. 
__interrupt void Timer2_overflow(void)   
{
    __disable_interrupt();   
     
    TCNT2 = 0x63;          // 0.01초
    
    count++;
    if(count>100)          // 0.01초 * 100회 = 1초
	  { 
	    temp=TCNT3;        // 카운터 3의 값을 읽어서
        
        temp=temp/200;     // 200 슬롯수 

	    lcd_decimal(0xCC, temp);      // RPS 표시, 1초당 회전수
      
	    count=0;
	    TCNT3=0;      // 타이머1 값 클리어
	   }
            
    __enable_interrupt();
}
