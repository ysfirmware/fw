/***************************************************************************
���ϸ� :   Exam14_3.c
 
�� ��  :   ���� ���͸� �����ϴ� �⺻���� ���α׷����� 
          * �ܺ����ͷ�Ʈ INT0(PD.0) Ű�� ������ ���� ���͸� ����,
          * �ܺ����ͷ�Ʈ INT1(PD.1) Ű�� ������ ���� ���͸� ����
		  �ϴ� ���α׷��� �ۼ��Ѵ�.


            <���� ���� ȸ�� ����>

           PE6       PE5     PE4(OC3B)   /RESET   : Atmega128 
            |         |         |          |
           DIR      ENable    Pusle     /RESET    : L297
           ���      ���     ���       ���  
           
                   
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

unsigned int speed=2500;

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
	   for(j=0;j<215;j++);  //16MHz : 1msec               
}


void u20_delay(unsigned int u) //������ ��ƾ  16MHz : 20usec
{
    unsigned int i, j; 
	                            
	 
	for(i=0;i<u;i++)
	   for(j=0;j<4;j++);      //16MHz : 20usec  
	       
}        


void instruction_out(unsigned char b)//��ɾ� ���� �Լ�
{
	PORTA=b&0xF0;                    //���� 4BIT ��� 

	ENABLE;                          //ENABLE 
	u20_delay(10);
	DISABLE;                         //DISABLE
	
	PORTA=(b&0x0F)<<4;               //���� 4BIT ���
	
	ENABLE;                          //ENABLE
	u20_delay(10);
	DISABLE;                         //DISABLE
	
	wait_busy_flag();                //��ɾ� ������ �Ϸ�ɶ����� ����
}


void char_out(unsigned char b)  //LCD�� �ѹ��� ��� �Լ�
{
   
	PORTA=(b&0xF0)|0x01;        //���� 4BIT ���
	
	ENABLE;                     //ENABLE
	u20_delay(10);
	DISABLE;                    //DISABLE 
	
	PORTA=((b&0x0F)<<4)|0x01;   //���� 4BIT ���
	
	ENABLE;                     //ENABLE
	u20_delay(10);
	DISABLE;                    //DISABLE

	wait_busy_flag();           //��ɾ� ������ �Ϸ�ɶ����� ����
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

  	m_delay(20);                          
	              
	instruction_out(0x28);   //LCD FUNCTION SET(16X2 LINE, 4 BIT, 5X8 DOT)            
	wait_busy_flag();        //��ɾ� ������ �Ϸ� �ɶ������� ������
	instruction_out(0x0C);   //LCD DISPLAY ON, CURSOR OFF, BLINK OFF 
	instruction_out(0x06);   //LCD ENTRY MODE SET
	instruction_out(0x02);   //RETURN HOME
	instruction_out(0x01);   //LCD CLEAR
	instruction_out(0x01);   //LCD CLEAR 
}


void lcd_decimal(unsigned int hex)
{
	unsigned int h;
	
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

void delay(unsigned int k)
{
    unsigned int i;

	for(i=0;i<k;i++); 
}


void main(void)
{
	DDRB=0xFF;    // ��Ʈ B ���
	PORTB=0xFF;   // LED off
    
    DDRE=0xFE;    // PE7  PE6  PE5  PE4(0C3B) PE3  PE2  PE1(TXD)  PE0(RXD)
	              //  1    1    1      1       1     1     1         0 
                  //  NC  ��   ��      ��      NC    NC    ��       ��
			      //      DIR   EN   PULSE 

    PORTE=0x00;   // �ʱⰪ

	init_LCD();    // LCD �ʱ�ȭ


    EIMSK|=(1<<INT1)|(1<<INT0);	//EIMSK|=0x02|0x01;  // INT1, INT0 ���� ���ͷ�Ʈ �ο��̺�
    EICRA|=0x08|0x02;   // INT1, INT0 ����(�ϰ�) ���� 
    EIFR=0x02|0x01;     // INTF1, INTF0 Ŭ���� 
	
    PORTE |= 0x20;   // L297 enable
	PORTE |= 0x40;   // (Dir=1) ---> CW 
                     // (Dir=0) ---> CCW 

    string_out(0x80, "Step Motor SPEED");
    instruction_out(0xC6);  // LCD ��ġ
	lcd_decimal(speed);

    __enable_interrupt();    // Global interrupt enable
                             // �Ǵ� _SEI(); ����

    while(1)
	 {      
		    PORTE |= 0x10;    // �ֱ� 1/(1.6msec) = 625Hz
			delay(130);   // Limit : 130 -> 0.1msec
            PORTE &= 0xEF;
			delay(speed);  // Limit : 2000 -> 1.5msec		 
     }
}               

#pragma vector=INT0_vect              // Refer iom128.h !!!!!
__interrupt void INT0_interrrupt(void)   
{
    __disable_interrupt();   
     
    speed=speed+2;       //�޽����� �о����Ƿ� ����

	instruction_out(0xC6);  // LCD ��ġ
	lcd_decimal(speed);
            
    __enable_interrupt();
}

#pragma vector=INT1_vect              // Refer iom128.h !!!!!
__interrupt void INT1_interrrupt(void)   
{
    __disable_interrupt();   
     
    speed=speed-2;      //�޽����� �������Ƿ� ����

    instruction_out(0xC6); // LCD ��ġ
	lcd_decimal(speed);
               
    __enable_interrupt();
}
