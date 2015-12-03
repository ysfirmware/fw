/*******************************************************************************
���ϸ� :   Exam4_3.c
 
�� ��  :    ATmega128�� PORTB�� ����� LED�� PB0(LSB)���� PB7(MSB) ��������
           �� ĭ�� �̵��ϴ� ���α׷��� �ۼ��� ����.

Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

void m_delay(unsigned int m) //������ ��ƾ
{
	unsigned int i, j; 
	                   
    
    __disable_interrupt();	
	  
	for(i=0;i<m;i++)
	   for(j=0;j<5;j++);  //16MHz : 1msec 2650 

	__enable_interrupt();
}


void main(void)
{       
    unsigned char number=0xFF;
    
	DDRB=0xFF;    // ������� ����  
	PORTB=0xFF;   // LED�� ����� ��Ʈ
    
	do{
	    PORTB = number;	// ������ ����� LED�ʱⰪ�� PORTB�� ��� 
	    number = (number << 1) | 0x01; 
	      // �������� �������� ��ĭ shift�ѵ� 0x01���� ���Ͽ� ��������Ʈ�� LED�� off 
	    if((number & 0xff) == 0xff)	 
		   number = 0xfe;		
	     // ���� �����ǰ��� 0xff�� ���Ͽ� �� ���� 0xff�̸�(8���� ��Ʈ�� ��� shift�Ǹ�)
		 // ������ LED�ʱⰪ�� �ٽ� ���� 
	    m_delay(2);		// �����ð� ���� 
	}while(1);		// ���ѷ��� 
} 
         