/*******************************************************************************
���ϸ� :   Exam6_9.c
 
�� ��  :   Atmega128�� ICP1/PD4�� KEY(Ǫ�� ����ġ)�� ����Ǿ� �ִ�. �� KEY �Է��� �޾�
           Timer/counter1�� Input Capture1���� �����ϰԲ� �����ϰ� �ϰ� �������� 
		   �Է¹޾� ����� �ð���(ICR1)�� ��Ʈ A�� ����� LCD Module�� 16���� 
		   ������ ����ϴ� ���α׷�����, Timer/Counter1 Input Capture Interrupt��
		   �̿��Ͽ� ���ͷ�Ʈ�� ó���Ѵ�.
      
	       PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128�� PORTA�� ����� LCD ����� 4��Ʈ ����� ����Ѵ�.

Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

#define ENABLE  (PORTA_Bit2=1)
#define DISABLE (PORTA_Bit2=0) 

//#define ENABLE  (PORTA |=  0x04)  // LCD ENable �Ǵ� PORTA_Bit2=1;
//#define DISABLE (PORTA &= 0xFB)   // LCD DIsable �Ǵ� PORTA_Bit2=0;

unsigned int cap_time=0;

void init_PORTA(void)
{
	DDRA = 0xFF;    // PORTA ���
	PORTA=0xFF;     // �ʱ� ��
}

/*void wait_busy_flag(void)  
  {                         
     unsigned char busy_flag; 
     
     PORTA|=0x06;    //PA2(EN)=1  PA1(R/-W)=1  PA0(RS)=0
     DDRA &=0x7F;    //PORTA7 �Է� 
          
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

  
void m_delay(unsigned int m) //������ ��ƾ
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

void instruction_out(unsigned char b)//��ɾ� ���� �Լ�
{
	PORTA=b&0xF0;                    //���� 4BIT ��� 

	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	PORTA=(b<<4)&0xF0;               //���� 4BIT ���
	
	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	//��ɾ� ������ �Ϸ�ɶ����� ����
	m_delay(2);
}


void char_out(unsigned char b)  //LCD�� �ѹ��� ��� �Լ�
{
   
	PORTA=(b&0xF0)|0x01;        //���� 4BIT ���
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE 
	
	PORTA=((b<<4)&0xF0)|0x01;   //���� 4BIT ���
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE

   //��ɾ� ������ �Ϸ�ɶ����� ����
    m_delay(2);
}


void string_out(unsigned char b, unsigned char *str) //���ڿ� ��� �Լ�
{
	unsigned int i=0;
	
	instruction_out(b);   //LCD ��ġ ����
	
	do{
	    char_out(str[i]);
	   }while(str[++i]!='\0');   // NULL ���ڸ� ���� �� ����
}


void init_LCD(void)                //LCD�ʱ�ȭ ��ƾ
{   
	init_PORTA();    // LCD�� ������ ��Ʈ �ʱ�ȭ

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
	DDRD=0xEF;   // �Ǵ� DDRD_Bit4=0; ���� ����Ͽ��� �ȴ�.
	PORTD=0x00;

	TCCR1A=0x00;
	TCCR1B=0x85;  // 1024 Prescaler, ICP1(PD4)�� �ϰ��������� �Է�
	              // Input Capture Noise Cancler ����
    TCCR1C=0x00;
	TCNT1=0x0000;  // �ʱⰪ�� Ŭ�����Ѵ�.
    
	TIMSK=0x20;  // TICIE1(Timer/Counter1 Input Capture Interrupt Enable) ���� 
	             // ���� ���ͷ�Ʈ ����
    TIFR|=0x20;  // ICF1(Timer/Counter1 Input Capture Flag) Ŭ����

    init_LCD();

    instruction_out(0x01);   // LCD�� �����.
	string_out(0x80, "  ICP1  ");  // LCD�� ù��° ����   
    instruction_out(0xC6);  //LCD ��ġ ����
	char_out('H');
    __enable_interrupt();    // Global interrupt enable
                             // �Ǵ� _SEI(); ����
    while(1)
	   lcd4hex(0xC2, cap_time);  // LCD�� ����Ѵ�.
}

#pragma vector=TIMER1_CAPT_vect              // iom128.h�� �����Ѵ�. 
 __interrupt void TIMER1_CAPT_vect_interrrupt(void)   
{
    __disable_interrupt();   
      
    cap_time=ICR1;         // ĸ�ĵ� �ð���(ICR1)�� cap_time�� �����Ѵ�.
            
    __enable_interrupt();
}
