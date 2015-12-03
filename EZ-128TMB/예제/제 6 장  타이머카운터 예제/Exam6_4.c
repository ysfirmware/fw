/*******************************************************************************
���ϸ� :   Exam6_4.c
 
�� ��  :   OC0(PB4)���� CTC ����� �̿��Ͽ� �ֱ������� LED�� 
           ��� ���� ����ϸ�, ���⼭ �ֱ�� 50%�� �ȴ�.

           ���ļ� = (16MHz)/(1024 Prescaler) = 15625 Hz
		   T = 1/15625 = 0.000064 sec = 64 usec

Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)


void OC0_CTC_out(unsigned char duty)
{
	OCR0=duty;
} 


void main( void )
{       
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;
   
    DDRB_Bit4 = 1; // PORTB.4�� OC0�� ��·� �����Ѵ�.

    TCCR0=0x1F;    // (0001 1111) CTC ���, ���, 1024 prescaler
    TCNT0=0x00;    // Ÿ�̸�0 �ʱⰪ
    
    OC0_CTC_out(0x80); // OCR0�� TCNT0�� ������ ��� 
                       // �� ����� �ֱ�� 0x00~0x8E0������ �ȴ�. 
					   // 0x8C0(128) X 64usec = 8192 usec = 8.192 msec
					   // �ϳ��� �޽��� �ֱ�� 8.192 + 8.192 = 16.384 msec �̴�.
                       // 16.384 msec���� LED�� On/Off �ȴ�.

    while(1);    
} 

