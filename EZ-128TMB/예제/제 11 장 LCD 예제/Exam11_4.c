/*******************************************************************************
���ϸ� :   Exam12_4.c
 
�� ��  :   LCD ��⿡ ���ڸ� ǥ���ϴ� �⺻ ���α׷����� LCD ��⿡ �ѱ۷� 
           "����ũ�θ��콺" ��� ǥ���ϴ� ���α׷�.

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
    unsigned char i;
	
	unsigned char name_a[]={0x00,0x1f,0x11,0x11,0x11,0x11,0x1f,0x00};	/* �� */
	unsigned char name_b[]={0x10,0x10,0x10,0x1c,0x10,0x10,0x10,0x10};	/* �� */
	unsigned char name_c[]={0x00,0x0e,0x11,0x11,0x11,0x11,0x0e,0x00};	/* �� */
	unsigned char name_d[]={0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10};	/* �� */
	unsigned char name_e[]={0x1f,0x01,0x1f,0x01,0x01,0x00,0x00,0x1f};	/* ũ */
	unsigned char name_f[]={0x1f,0x01,0x1f,0x10,0x1f,0x04,0x04,0x1f};	/* �� */
	unsigned char name_g[]={0x1f,0x04,0x04,0x04,0x04,0x00,0x00,0x00};	/* �� */
	unsigned char name_h[]={0x00,0x04,0x0a,0x11,0x11,0x00,0x00,0x1f};	/* �� */
	
    init_LCD();

    instruction_out(0x01);   // LCD�� �����.
	
    string_out(0x80, "HanGul");  // LCD�� ù��° ����
   	
    while(1)
	  {
		instruction_out(0x40);		/* ������ ����� �� �ѱ���Ʈ�� CGRAM�� ���� */
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
		
		instruction_out(0xC0);	// LCD�� �ڸ��� �ش��ϴ� �ּҰ��� ���� ������ ���ڵ��� ��� 
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
