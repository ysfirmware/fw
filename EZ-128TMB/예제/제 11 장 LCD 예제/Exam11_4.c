/*******************************************************************************
파일명 :   Exam12_4.c
 
동 작  :   LCD 모듈에 문자를 표시하는 기본 프로그램으로 LCD 모듈에 한글로 
           "마이크로마우스" 라고 표시하는 프로그램.

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

void main(void)
{
    unsigned char i;
	
	unsigned char name_a[]={0x00,0x1f,0x11,0x11,0x11,0x11,0x1f,0x00};	/* ㅁ */
	unsigned char name_b[]={0x10,0x10,0x10,0x1c,0x10,0x10,0x10,0x10};	/* ㅏ */
	unsigned char name_c[]={0x00,0x0e,0x11,0x11,0x11,0x11,0x0e,0x00};	/* ㅇ */
	unsigned char name_d[]={0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10};	/* ㅣ */
	unsigned char name_e[]={0x1f,0x01,0x1f,0x01,0x01,0x00,0x00,0x1f};	/* 크 */
	unsigned char name_f[]={0x1f,0x01,0x1f,0x10,0x1f,0x04,0x04,0x1f};	/* 로 */
	unsigned char name_g[]={0x1f,0x04,0x04,0x04,0x04,0x00,0x00,0x00};	/* ㅜ */
	unsigned char name_h[]={0x00,0x04,0x0a,0x11,0x11,0x00,0x00,0x1f};	/* 스 */
	
    init_LCD();

    instruction_out(0x01);   // LCD를 지운다.
	
    string_out(0x80, "HanGul");  // LCD의 첫번째 라인
   	
    while(1)
	  {
		instruction_out(0x40);		/* 위에서 만들어 준 한글폰트를 CGRAM에 저장 */
		for(i=0;i<8;i++)
		{
		char_out(name_a[i]);
		}
		
		instruction_out(0x48);
		for(i=0;i<8;i++)
		{
		char_out(name_b[i]);
		}
		
		instruction_out(0x50);
		for(i=0;i<8;i++)
		{
		char_out(name_c[i]);
		}
		
		instruction_out(0x58);
		for(i=0;i<8;i++)
		{
		char_out(name_d[i]);
		}
		
		instruction_out(0x60);
		for(i=0;i<8;i++)
		{
		char_out(name_e[i]);
		}
		
		instruction_out(0x68);
		for(i=0;i<8;i++)
		{
		char_out(name_f[i]);
		}
		
		instruction_out(0x70);
		for(i=0;i<8;i++)
		{
		char_out(name_g[i]);
		}
		
		instruction_out(0x78);
		for(i=0;i<8;i++)
		{
		char_out(name_h[i]);
		}
		
		instruction_out(0xC0);	// LCD의 자리에 해당하는 주소값에 따라 각각의 문자들을 출력 
		char_out(0x00);
		
		instruction_out(0xC1);
		char_out(0x01);
		
		instruction_out(0xC2);
		char_out(0x02);
		
		instruction_out(0xC3);
		char_out(0x03);
		
		instruction_out(0xC4);
		char_out(0x04);
		
		instruction_out(0xC5);
		char_out(0x05);
		
		instruction_out(0xC6);
		char_out(0x00);
		
		instruction_out(0xC7);
		char_out(0x01);
		
		instruction_out(0xC8);
		char_out(0x02);
		
		instruction_out(0xC9);
		char_out(0x06);
		
		instruction_out(0xCA);
		char_out(0x07);
	  }
}
