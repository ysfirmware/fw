/*******************************************************************************
���ϸ� :   Exam6_2.c
 
�� ��  :   8bit Timer/counter0�� Overflow interrupt�� �̿��Ͽ� ���� �ð����� 
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
   
     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
        
    while(1)
		 PORTB=data;
}           

#pragma vector=TIMER0_OVF_vect              // iom128.h�� �����Ѵ�. 
__interrupt void TIMER0_OVF_interrrupt(void)   
{
    __disable_interrupt();   
    
	TCNT0=0x01;    // Timer/counte0 value(ī��Ʈ ��) 

   	data--;
            
    __enable_interrupt();
}

