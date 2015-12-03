/*******************************************************************************
���ϸ� :   Exam4_6.c
 
�� ��  :  ATmega128�� PORTB�� ����� LED�� PB0(LSB)���� PB7(MSB) ��������, �ٽ�
          PB7(MSB)���� PB0(LSB) �������� �� ĭ�� �̵��ϴ� ���α׷��� �ۼ��� ����.


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
    unsigned char a, b, i;

    DDRB=0xFF;    // ������� ����  
	PORTB=0xFF;   // LED�� ����� ��Ʈ

	while(1)
        {
          a=0xfe;
          b=0x7f;
          for(i=1;i<5;i++)
             {
              PORTB=a&b;
              m_delay(100);
              a=((a<<1)|0x01);
              b=((b>>1)|0x80);
             }
  
          a=0xf7;
          b=0xef;
          for(i=1;i<5;i++)
             {
              PORTB=a&b;   
              m_delay(100); 
              a=((a>>1)|0x80);
              b=((b<<1)|0x01);
             }
         }
}