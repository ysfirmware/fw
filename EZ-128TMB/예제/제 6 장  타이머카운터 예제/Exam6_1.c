/*******************************************************************************
���ϸ� :   Exam6_1.c
 
�� ��  :   8bit Timer/counter0�� Overflow flag bit(TOV0)�� �˻��Ͽ� ���� �ð����� 
           (64usec X 256 = 16.384msec) ��Ʈ B�� ����� LED�� �ϳ��� �̵��ϴ� 
		   ���α׷�.
   
           (16MHz)/(1024 prescaler) = 15.625KHz
		   
            T = 1/(15.625KHz) = 64usec 
           
Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

unsigned char data=0xFE;

void main(void)
{
         
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;    // LED off
	
	TCCR0=0x07;    // Normal mode(Timer/counte0 overflow), 1024 prescaler
    TCNT0=0x00;    // Timer/counte0 value(ī��Ʈ ��)
	TIMSK=0x01;    // TOIE0(Timer/counter0 oveflow interrupt enable)
	               // ���� ���ͷ�Ʈ �ο��̺�
				   // �Ǵ� TIMSK |= (1<<TOIE0); �� ����Ͽ��� �ȴ�.    
    TIFR|=0x01;     // TOV0 Timer/counter0 oveflow flag Ŭ���� 

    __disable_interrupt();   // Global ���ͷ�Ʈ �Ҵ�

    while(1)
	  {    
	    while(!(TIFR&0x01)); // TOV0 Timer/counter0 oveflow flag�� "1"�� �� ������
		                     // ��ٸ���.
		TIFR|=0x01;     // TOV0 Timer/counter0 oveflow flag Ŭ����
        TCNT0=0x00;    // Timer/counte0 value(ī��Ʈ ��)
		
		PORTB=data;
		data--;
	   };
}