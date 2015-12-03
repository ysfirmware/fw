/*******************************************************************************
���ϸ� :   Exam6_6.c
 
�� ��  :   OC0(PB4)���� PWM0 ����� �̿��Ͽ� PWM ���� LED(PORTB.4)�� ��Ÿ����,
           PWM0 ��� ���� LCD ��⿡ ����ϴ� ���α׷�����, LCD ����� PORTA�� 
		   �����ϰ� ���ڸ� ǥ���ϴ� ���α׷��̴�.
          
		   <LCD ȸ�� ����>
		   
           PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128�� PORTA�� ����� LCD ����� 4��Ʈ ���.

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


void pwm0_out(unsigned char duty)
{
	OCR0=duty;
} 


void main( void )
{
    unsigned char number;
     
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;
   
    DDRB_Bit4 = 1; // PORTB.4�� OC0�� �����Ѵ�.

    init_LCD();    //LCD �ʱ�ȭ �Լ� ����
    
    TCCR0=0x6A;    // (0110 1010) Fast PWM, Non-inverting, 8 prescaler
                   // f=16MHz/(N*256), N=8 prescaler factor  
                   // f= 7.8125KHz, T=128usec
				   // PWM�� �ֱ�� 128usec�� �ȴ�.
    
    TCNT0=0x00;    // T/C value=0
	OCR0=0x00;    
    
    m_delay(10);  // 10msec ���
    instruction_out(0x01);  //clear LCD
    
    string_out(0x80,"PWM0/OC0");
	instruction_out(0xC5);   // LCD ��ġ ����
	char_out('H');
   
    while(1)
    { 
       for(number=0;number<256;number++)
       {  
          pwm0_out(number);  // OC0 ��(PB4)�� ���
          
          instruction_out(0xC3); 
          lcd2hex(number);
          m_delay(100);
        }
    }
} 

