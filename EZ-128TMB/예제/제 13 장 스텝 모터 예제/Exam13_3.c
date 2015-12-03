/***************************************************************************
파일명 :   Exam14_3.c
 
동 작  :   스텝 모터를 구동하는 기본적인 프로그램으로 
          * 외부인터럽트 INT0(PD.0) 키를 누르면 스텝 모터를 감속,
          * 외부인터럽트 INT1(PD.1) 키를 누르면 스텝 모터를 가속
		  하는 프로그램을 작성한다.


            <스텝 모터 회로 연결>

           PE6       PE5     PE4(OC3B)   /RESET   : Atmega128 
            |         |         |          |
           DIR      ENable    Pusle     /RESET    : L297
           출력      출력     출력       출력  
           
                   
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

unsigned int speed=2500;

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
	   for(j=0;j<215;j++);  //16MHz : 1msec               
}


void u20_delay(unsigned int u) //딜레이 루틴  16MHz : 20usec
{
    unsigned int i, j; 
	                            
	 
	for(i=0;i<u;i++)
	   for(j=0;j<4;j++);      //16MHz : 20usec  
	       
}        


void instruction_out(unsigned char b)//명령어 쓰기 함수
{
	PORTA=b&0xF0;                    //상위 4BIT 출력 

	ENABLE;                          //ENABLE 
	u20_delay(10);
	DISABLE;                         //DISABLE
	
	PORTA=(b&0x0F)<<4;               //하위 4BIT 출력
	
	ENABLE;                          //ENABLE
	u20_delay(10);
	DISABLE;                         //DISABLE
	
	wait_busy_flag();                //명령어 수행이 완료될때까지 지연
}


void char_out(unsigned char b)  //LCD에 한문자 출력 함수
{
   
	PORTA=(b&0xF0)|0x01;        //상위 4BIT 출력
	
	ENABLE;                     //ENABLE
	u20_delay(10);
	DISABLE;                    //DISABLE 
	
	PORTA=((b&0x0F)<<4)|0x01;   //하위 4BIT 출력
	
	ENABLE;                     //ENABLE
	u20_delay(10);
	DISABLE;                    //DISABLE

	wait_busy_flag();           //명령어 수행이 완료될때까지 지연
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

  	m_delay(20);                          
	              
	instruction_out(0x28);   //LCD FUNCTION SET(16X2 LINE, 4 BIT, 5X8 DOT)            
	wait_busy_flag();        //명령어 수행이 완료 될때까지의 딜레이
	instruction_out(0x0C);   //LCD DISPLAY ON, CURSOR OFF, BLINK OFF 
	instruction_out(0x06);   //LCD ENTRY MODE SET
	instruction_out(0x02);   //RETURN HOME
	instruction_out(0x01);   //LCD CLEAR
	instruction_out(0x01);   //LCD CLEAR 
}


void lcd_decimal(unsigned int hex)
{
	unsigned int h;
	
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

void delay(unsigned int k)
{
    unsigned int i;

	for(i=0;i<k;i++); 
}


void main(void)
{
	DDRB=0xFF;    // 포트 B 출력
	PORTB=0xFF;   // LED off
    
    DDRE=0xFE;    // PE7  PE6  PE5  PE4(0C3B) PE3  PE2  PE1(TXD)  PE0(RXD)
	              //  1    1    1      1       1     1     1         0 
                  //  NC  출   출      출      NC    NC    출       입
			      //      DIR   EN   PULSE 

    PORTE=0x00;   // 초기값

	init_LCD();    // LCD 초기화


    EIMSK|=(1<<INT1)|(1<<INT0);	//EIMSK|=0x02|0x01;  // INT1, INT0 개별 인터럽트 인에이블
    EICRA|=0x08|0x02;   // INT1, INT0 폴링(하강) 에지 
    EIFR=0x02|0x01;     // INTF1, INTF0 클리어 
	
    PORTE |= 0x20;   // L297 enable
	PORTE |= 0x40;   // (Dir=1) ---> CW 
                     // (Dir=0) ---> CCW 

    string_out(0x80, "Step Motor SPEED");
    instruction_out(0xC6);  // LCD 위치
	lcd_decimal(speed);

    __enable_interrupt();    // Global interrupt enable
                             // 또는 _SEI(); 가능

    while(1)
	 {      
		    PORTE |= 0x10;    // 주기 1/(1.6msec) = 625Hz
			delay(130);   // Limit : 130 -> 0.1msec
            PORTE &= 0xEF;
			delay(speed);  // Limit : 2000 -> 1.5msec		 
     }
}               

#pragma vector=INT0_vect              // Refer iom128.h !!!!!
__interrupt void INT0_interrrupt(void)   
{
    __disable_interrupt();   
     
    speed=speed+2;       //펄스폭이 넓어지므로 감속

	instruction_out(0xC6);  // LCD 위치
	lcd_decimal(speed);
            
    __enable_interrupt();
}

#pragma vector=INT1_vect              // Refer iom128.h !!!!!
__interrupt void INT1_interrrupt(void)   
{
    __disable_interrupt();   
     
    speed=speed-2;      //펄스폭이 좁아지므로 가속

    instruction_out(0xC6); // LCD 위치
	lcd_decimal(speed);
               
    __enable_interrupt();
}
