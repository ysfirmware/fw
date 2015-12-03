/*******************************************************************************
���ϸ� :   Exam5_2.c
 
�� ��  :   INT0(PD0)�ɿ� ����ġ�� ������ �Ŀ� ����ġ�� ������ not gate�� ���Ͽ�
           ��, ��� �������� ���ͷ�Ʈ�� �߻��ϵ��� ������ �Ŀ�,
		   ���ͷ�Ʈ�� �߻��ϸ� LCD ��⿡ ���ڸ� ī��Ʈ�ϴ� ���α׷��� �ۼ��Ѵ�.
           
Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

unsigned int data=0x00;   //���� ���� ����

#define ENABLE  (PORTA_Bit2=1)
#define DISABLE (PORTA_Bit2=0) 

//#define ENABLE  (PORTA |=  0x04)  // LCD ENable �Ǵ� PORTA_Bit2=1;
//#define DISABLE (PORTA &= 0xFB)   // LCD DIsable �Ǵ� PORTA_Bit2=0;

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
         
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;    // LED off

	DDRD_Bit0=0;   // PD0 �Է� DDRD �������ʹ� �ʱⰪ�� 0���� �����Ǿ� �ֱ� ������
	               // "DDRD_Bit0=0;"�� ���� �����ϴ�. INT0(PD0)��
				   // DDRD&=0xFE;
	        
	init_LCD();    // LCD �ʱ�ȭ
   
    EIMSK|=0x01;   // INT0 ���� ���ͷ�Ʈ �ο��̺�
    EICRA|=0x03;   // INT0 ��� ���� 
    EIFR|=0x01;     // INTF0 Ŭ���� 
   
    instruction_out(0x01);   // LCD clear
	string_out(0x80, "EXTINT0!");
   
     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
        
    while(1)
       lcd_decimal(0xC2, data);   // LCD ��ġ ����   
}           

#pragma vector=INT0_vect              // iom128.h�� �����Ѵ�. 
 __interrupt void INT0_interrrupt(void)   
{
    __disable_interrupt();   
     
    data++;    // data ����
            
    __enable_interrupt();
}
