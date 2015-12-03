/***************************************************************************
���ϸ� :   Exam7_1.c

�� ��  :   + ���� AIN0�� - ���� AIN1�� ������ ���ؼ� AIN0�� ������ AIN1�� 
          ���к��� ������ ACSR(Analog Comparator Control & Status Register)�� 
		  ��Ʈ 3�� ACIE(Analog Comparator Interrupt Enable)�� "1"�� �����ϰ� 
		  ���� ���ͷ�Ʈ�� �����Ǿ� ������ ���� ���ͷ�Ʈ�� �߻��ȴ�.

                      
Device :  Atmega128-16AI
Clock  :  16MHz
Compiler: IAR AVR C Compiler V2.28A
***************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

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


unsigned char led=0x00;

void main(void)
{
	DDRE &= 0xF3;  // PE.3, PE.2 �Է�
	   
	init_LCD();    // ��Ʈ A�� ����� LCD�� �ʱ�ȭ �Ѵ�.

	
	ACSR &= 0x7F;    // Analog Comparator Enable
	ACSR |= 0x08;    // ACIE, Toggle mode
		
    m_delay(100); 

	string_out(0x80, "Compare!");

	 __enable_interrupt();

	while(1)
	{
	  if(led==0x00)
	    string_out(0xC2, "Low ");
	  else
	    string_out(0xC2, "High");
	}
}

#pragma vector=ANA_COMP_vect              // iom128.h �����Ѵ�.
__interrupt void analog_comparator(void)   
{
	__disable_interrupt(); 
	
	led^=0xff;   // ���ͷ�Ʈ�� �߻��� �� ���� ����Ѵ�.

	__enable_interrupt();
 }