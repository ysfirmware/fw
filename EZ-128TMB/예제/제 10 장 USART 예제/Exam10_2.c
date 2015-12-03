/*******************************************************************************
���ϸ� :   Exam10_2.c
 
�� ��  :  IBM PC�� Ű���忡�� �ϳ��� ASCII CODE ���� �о �� ���� ��Ʈ B�� 
          ����� LED�� ���� ����ϰ�, ����Ϳ� �ٽ� ����ϸ� �׸��� LCD ��⿡
		  �� ���� ����ϴ� ���α׷��� �ۼ��Ѵ�. 
		  ���α׷� ������ ���� ������� �ۼ��Ͽ���.

        *  PA7  PA6  PA5  PA4  PA3  PA2  PA1  PA0  <--- PORTA
	        |    |    |    |    |    |    |    |
	        D7   D6   D5   D4   X    EN  R/-W  RS  <--- LCD Module
           
           ATMEGA128�� PORTA�� ����� LCD ����� 4��Ʈ ����� ����Ѵ�.

        *  PE0 -> RXD
		   PE1 -> TXD
           
           16MHz���� 9600 bps


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

void init_SERIAL(void)   // �ø��� �Լ� �ʱ�ȭ
{

     DDRE=0xFE;     // PE1(TXD) ���(1), PE0(RXD) �Է�(0)

     UCSR0A=0x00;
     UCSR0B=0x18;   // Receive enable, Transmitte enable
     UCSR0C=0x06;   // �񵿱� ���, No parity bit, 1 stop bit   

     UBRR0H=0x00;
     UBRR0L=0x67;   // 16MHz���� 9600 bps  (10���� 103)
}

// �ø��� ��Ʈ�κ��� �ѹ��ڸ� �����Ѵ�.  
char getch(void)
{
    unsigned char data;

	while(!(UCSR0A & 0x80));
    data=UDR0;

	UCSR0A |= 0x80;        // Ŭ���� RXC0
	
	return(data);
}

// �ø��� ��Ʈ�� �ѹ��ڸ� �۽��Ѵ�. 
void putch(unsigned char data)
{
	while(!(UCSR0A & 0x20));
	UDR0=data;
    
	UCSR0A |= 0x20;         // Ŭ���� UDRE0
}

void main(void)
{   
    unsigned char data;	

	DDRB=0xFF;     // PORTB ������� ����
	PORTB=0xFF;    // PORTB_Bit3=1;   
	               // CodeVisionAVR���� ��Ʈ ���� ���α׷��ϱ� PORTB.3=1;
                   // PORTB=1<<3;
    
	init_SERIAL();  // LCD �ʱ�ȭ
    init_LCD();          

    instruction_out(0x01);     // LCD clear	
    string_out(0x81, "USART0");
	instruction_out(0xC5);    //LCD ��ġ ����
	char_out('H');     
            
       
	while(1)
	    {
		  data=getch();  // ������ �����͸� data ������ �����Ѵ�.
		  putch(data);   // ������ �����͸� IBM PC�� ����Ѵ�.
 
		  PORTB=~data;   // LED�� ���� ����Ѵ�.
		  instruction_out(0xC3);  // LCD ��ġ ����
	      lcd2hex(data);	      // LCD�� ����Ѵ�.    
         } 
}