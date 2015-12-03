/*******************************************************************************
���ϸ� :   Exam8_3.c

�� ��  :   ADC0(PF0)�ɿ� �������� 10K�� ������ �Ŀ�, �� ��� ���� ��Ʈ A�� 
           ����� LCD Module�� 16���� ������ ����ϴ� ���α׷����� AD ��ȯ �Ϸ� 
		   ���ͷ�Ʈ�� �̿��Ѵ�.
           
Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)


unsigned int result=0x0000;

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
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;    // LED off

    DDRF |= 0xFE;  // ADC0(PF0) �Է�
	               // �Ǵ� DDRF_Bit0=0;���� �����Ͽ��� �ȴ�.
   
    ADMUX=0x00;    // (00 0 00000) �ܺ� AREF, ���� ����,  ä�� 0
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 ����
                      
    ADCSR |= 0x08; // ADC Interrupt enable(���� ���ͷ�Ʈ ����)

	ADCSR |= 0x40; //ADC start conversion
    
    init_LCD();
	string_out(0x80, "ADC0->VR");
	instruction_out(0xC5);  //LCD ��ġ ����
	char_out('H');

     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
        
    while(1)  //���
	    lcd4hex(0xC1, result);  // LCD ��ġ ������ 2 ����Ʈ ��簪 ���
}         

#pragma vector=ADC_vect              // iom128.h�� �����Ѵ�. 
__interrupt void ADC_conversion_done(void)   
{
    __disable_interrupt();        
      
    result=ADC;            // ADC ��ȯ ��� ��(ADC)�� �о� result�� �����Ѵ�.
	
    __enable_interrupt();
}

