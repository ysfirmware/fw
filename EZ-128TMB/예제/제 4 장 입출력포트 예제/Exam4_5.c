/*******************************************************************************
���ϸ� :   Exam4_5.c
 
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
    
    
    unsigned char L_shift, R_shift, i;

    DDRB=0xFF;    // ������� ����  
	PORTB=0xFF;   // LED�� ����� ��Ʈ LED OFF
   
	
	while(1)                           //���� ����
     {
      L_shift = 0x01;                  // L_shift�� 00000001B�� ����                 
      R_shift = 0x80;                  // R_shfit�� 10000000B�� ����

      for(i=0 ; i<8 ; i++)             // ������ 8�� ���� 
        {         
          PORTB = L_shift;             // ��Ʈ1�� L_shfit�� ����    
          L_shift = L_shift << 1;      // L_shfit���� 1ĭ shfit left��Ŵ
          m_delay(100);                // �ð�����
        }

      for(i=0 ; i<8 ; i++)             // ������ 8�� ���� 
        {          
          PORTB = R_shift;             // ��ƮB�� R_shfit�� ����   
          R_shift = R_shift >> 1;      // R_shfit���� 1ĭ shfit right��Ŵ 
          m_delay(100);                // �ð�����
        }
    }
}