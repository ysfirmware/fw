/*******************************************************************************
���ϸ� :   Exam5_4.c
 
�� ��  :   INT0(PD0)�ɿ� ����ġ�� ������ �Ŀ� ����ġ�� ������ not gate�� ���Ͽ�,
           INT4(PE4)�ɿ� ����ġ�� ������ �Ŀ� ����ġ�� ������ not gate�� ���Ͽ�
           ��, ��� �������� ���ͷ�Ʈ�� �߻��ϵ��� ������ �Ŀ�,
		   INT0 ���ͷ�Ʈ�� �߻��ϸ� ��ƮB�� ����� LED�� �·� �̵��ϰ�,
           INT4 ���ͷ�Ʈ�� �߻��ϸ� ��ƮB�� ����� LED�� ��� �̵��ϴ�
		   ���α׷��� �ۼ��Ѵ�.
           
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

unsigned int data=0xFE;

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


void main(void)
{
         
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0x00;    // LED ON

	DDRE=0xFF;     
    PORTE=0x00;    

	DDRD_Bit0=0;   // PD0 �Է� DDRD �������ʹ� �ʱⰪ�� 0���� �����Ǿ� �ֱ� ������
	               // "DDRD_Bit0=0;"�� ���� �����ϴ�. INT0(PD0)��
				   // DDRD&=0xFE;
	DDRE_Bit4=0;   // PE4 �Է� DDRE4 ��Ʈ�� �ʱⰪ�� 0���� �����Ǿ� �ֱ� ������
	               // "DDRE_Bit4=0;"�� ���� �����ϴ�. INT4(PE4)��
				   // DDRE&=0xEF;
				   
	init_LCD();    // LCD �ʱ�ȭ   
   

	EIMSK|=0x11;   // INT1, INT4 ���� ���ͷ�Ʈ �ο��̺�
    EICRA|=0x03;   // INT0 ��� ���� 
    EICRB|=0x03;   // INT4 ��� ����

    EIFR|=0x11;     // INTF4, INTF0 Ŭ���� 
   
     m_delay(1000);  //1�� ���


    instruction_out(0x01);   // LCD clear
	string_out(0x80, " INT4_0 ");  // LCD ��ġ ����
    string_out(0xC0, "<-L, R->");  // LCD ��ġ ����

     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
        
    while(1);  // ���ͷ�Ʈ ���        
}           

#pragma vector=INT0_vect              // iom128.h�� �����Ѵ�. 
__interrupt void INT0_interrrupt(void)   
{
    __disable_interrupt();   
     
    PORTB=data;
	data++;    // data ����

    __enable_interrupt();
}

#pragma vector=INT4_vect              // iom128.h�� �����Ѵ�. 
__interrupt void INT4_interrrupt(void)   
{
    __disable_interrupt();   
     
    PORTB=data;
	data--;   // data ����
	
    __enable_interrupt();
}
