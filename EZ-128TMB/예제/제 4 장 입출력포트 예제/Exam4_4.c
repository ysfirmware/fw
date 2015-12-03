/*******************************************************************************
���ϸ� :   Exam4_4.c
 
�� ��  :  ATmega128�� PORTB�� ����� LED�� PB0���� PB3, PB4���� PB7�� ���� 4��Ʈ��
          �𿴴ٰ� ������ ���α׷��� �ۼ��� ����.

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
	   for(j=0;j<2650;j++);  //16MHz : 1msec  

	__enable_interrupt();
}


void main(void)
{ 

    unsigned int h, j, m;   // ������������ 
 	h = 0xFE;
	j = 0x7F;		       // ������ ������ LED�ʱⰪ�� ���� 

	DDRB=0xFF;    // ������� ����     
	PORTB=0xFF;   // LED�� ����� ��Ʈ
   
	
	do{
     	m = h & j;		    // h�� j������ ����� ���� and���Ͽ� m������ ����
	    PORTB = m;		    // ������ ���Ͽ� ������ ����m���� PORTB�� ��� 
	    m_delay(100);		// �����ð� ���� 

		if((h==0x7f)||(j==0xfe))	// ���ǹ� (���� ���� h�� j�� ������ ���ǰ��� ���ٸ�) 
	  	 { 
		   h = 0xfe; 
		   j = 0x7f;		// �������� ���� �ʱ�ȭ 
		 } 
		else				// ���� ������ �ƴϸ� 
		  {
		   h = ((h << 1) | 0x01); 
		   j = ((j >> 1) | 0x80);	
		  }                     //������ �������� ���� �� �Ǵ� ��� shift�Ͽ� 
								// �ֻ��� �Ǵ� ��������Ʈ���� or�񱳿� ���� LED off 
	   }while(1);		// ���ѷ��� 
}              