/*******************************************************************************
���ϸ� :   Exam8_4.c

�� ��  :   ADC1(PF1)�ɿ� CDS ������ ������ �Ŀ�, �� ��� ���� ��Ʈ B�� 
           ����� LED�� ����ϴ� ���α׷����� AD ��ȯ �Ϸ� ���ͷ�Ʈ�� �̿��Ѵ�.
           
Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

unsigned char result=0xFF;

void m_delay(unsigned int m) // msec ������ ��ƾ
{
	unsigned int i, j; 
	                            
	for(i=0;i<m;i++)
	   for(j=0;j<215;j++);  //16MHz : 1msec              
}

void main( void )
{
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;    // LED off

    DDRF |= 0xFD;  // ADC1(PF1) �Է�
	               // �Ǵ� DDRF_Bit1=0;���� �����Ͽ��� �ȴ�.
   
    ADMUX=0x21;    // (00 1 00001) �ܺ� AREF, �� ����,  ä�� 1
    ADCSR=0xA7;    // (1010 0 111) ADC enable, ADC Free Runing Select, 128 ����
                      
    ADCSR |= 0x08; // ADC Interrupt enable(���� ���ͷ�Ʈ ����)

	ADCSR |= 0x40; //ADC start conversion
    

     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
        
    while(1)  //���
	  PORTB=~result;   // LED�� ���� ���
}         

#pragma vector=ADC_vect              // iom128.h�� �����Ѵ�. 
__interrupt void ADC_conversion_done(void)   
{
    __disable_interrupt();   
     
    result=ADCL;  // ���� ���� �Ŀ� ������ ���� �а�
    result=ADCH;  // �������� �д´�. 
                
    __enable_interrupt();
}

