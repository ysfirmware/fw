/*******************************************************************************
���ϸ� :   Exam8_1.c

�� ��  :   ADC0(PF0)�ɿ� �������� 10K�� ������ �Ŀ�, �� ��� ���� ��Ʈ B�� 
           ����� LED�� ����ϴ� ���α׷����� ����(Polling) ������� �ۼ��Ѵ�.
           ADC Free Runing Select ��Ʈ�� "1"�� ������ ����
		   ADC Free Runing Select ��Ʈ�� "0"���� ������ ��츦 ���Ͽ� ����.

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


void main( void )
{
    unsigned char result;     
		 
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;    // LED off

    DDRF &= 0xFE;  // ADC0(PF0) �Է�
	               // �Ǵ� DDRF_Bit0=0;���� �����Ͽ��� �ȴ�.
   
    ADMUX=0x20;    // (00 1 00000) �ܺ� AREF, �� ����,  ä�� 0
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 ����
                      
    ADCSR |= 0x40; //ADC start conversion

    do{
         m_delay(1);   // 1msec ���
         
         while((ADCSR & 0x10)==0);   //��ȯ�� �Ϸ�� ������ ���
		 							 // ��, ADIF == 1 ?
                                     //while(!(ADCSR & 0x10)); �� ����
         ADCSR |= 0x10;              // ADIF�� "1"�� Ŭ�����Ѵ�.

         result=ADCL;  // ���� ���� �Ŀ� ������ ���� �а�
         result=ADCH;  // �������� �д´�.
                  
         PORTB=~result;   // LED�� ���� ���

	     m_delay(1);   // 1msec ���
       }while(1);    
}           


