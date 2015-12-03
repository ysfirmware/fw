/*******************************************************************************
���ϸ� :   Exam13_3.c
 
�� ��  :   ADC0(PF0)�� ����� ���������� �����ؼ� AD ��ȯ��� ���� 200H���� ũ�� 
           DC ���͸� ���������� �ӵ��� ���� �����ϰ�, 200H���� ������ DC ���͸� 
    	   ���������� �ӵ��� ���� �����ϴ� ���α׷�����,       
           OC1A(PB5)���� PWM1A ����� �̿��Ͽ� PWM ���� LCD ��⿡ ����ϰ�
           DC ���͸� ȸ����Ų��. 
		   ���⼭ ����ϴ� PWM ����� Phase Correct PWM 10bit�� �̿��Ѵ�.

           LCD ����� PORTA�� �����ϰ� ���ڸ� ǥ���ϴ� ���α׷��̴�.

           <LCD ȸ�� ����>
		   
           PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128�� PORTA�� ����� LCD ����� 4��Ʈ ���.

           <���� ȸ�� ����>

            PB7       PB6       PB5(OC1A)  <-  atmega128
             |         |          |
            enable    dir        PWM       <-  L298 DC Motor Driver
            ���      ���       ��� 
           
		    <�������� ����>

			ADC0(PORTF.0) --> �������� 10K
                   
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
    if(dir==1)           // ������
		PORTB |= 0x40;   // ���� ���� DIR(������)
     else
        PORTB &= 0xBF;   // ���� ���� DIR(������)

    OCR1A=speed;
}


void ADC_init(void) // ADC �ʱ�ȭ
{   
	DDRF |= 0xFE;  // ADC0(PF0) �Է�
	               // �Ǵ� DDRF_Bit0=0;���� �����Ͽ��� �ȴ�.
   
    ADMUX=0x00;    // (00 0 00000) �ܺ� AREF, ���� ����,  ä�� 0
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 ����
                      
    ADCSR |= 0x08; // ADC Interrupt enable(���� ���ͷ�Ʈ ����)

	ADCSR |= 0x40; //ADC start conversion
    
}

unsigned int ad_result=0;

void main( void )
{
    DDRB |= 0xE0;    // 111* **** -> ������* ****
    PORTB=0xFF;
    
    init_LCD();      // LCD �ʱ�ȭ �Լ� ����
    
	TCCR1A |= 0x83;  // (10** **11)
    TCCR1B |= 0x02;  // (***0 0010) Phase correct PWM 10bit, Non-inverting, 8 prescaler
                     // f=16MHz/(2*N*0x3FF), N=8 prescaler factor  
                     // f= 997Hz, T=1.003msec
				     // PWM�� �ֱ�� 1.003msec�� �ȴ�.
    TCCR1C=0x00;     // FOC1A=0
    
    TCNT1=0x0000;    // T/C1 value=0
    OCR1A=0x000;

    ADC_init();      // ADC �ʱ�ȭ   

    PORTB |= 0x80;   // Motor Enable
               
    string_out(0x84, "DC Motor");
        
    m_delay(20000);  // 1�� ���
    instruction_out(0x01);  //clear LCD
        
    instruction_out(0x84); // LCD ��ġ ����
	char_out('H');
    instruction_out(0x8F); // LCD ��ġ ����
	char_out('H');

    __enable_interrupt();  // Global interrupt enable
                           // �Ǵ� _SEI(); ���� 
   

    while(1)
	{
		lcd4hex(0x80, ad_result);   // LCD ��ġ ����
  	 		
        if(ad_result>0x200)
			{
			  dc_motor(1, ad_result);  //��ȸ��
			              
	          lcd4hex(0x8B, ad_result);  // LCD ��ġ ����              
              string_out(0xC5, "CW  UP"); // LCD�� �ι�° ����
			}
		 else
			{
			  dc_motor(0, 0x3FF-ad_result);  //��ȸ��
		               
	          lcd4hex(0x8B, 0x3FF-ad_result);  // LCD ��ġ ����               
              string_out(0xC5, "CCW UP");  // LCD�� �ι�° ����
			}
	}		
} 

#pragma vector=ADC_vect              // iom128.h �����Ѵ�.
__interrupt void ADC_conversion_done(void)   
{
    __disable_interrupt();   
     
    ad_result=ADC;  // ADC ��ȯ ��� ���� �о ad_result�� �����Ѵ�.         
    
    __enable_interrupt();
}
