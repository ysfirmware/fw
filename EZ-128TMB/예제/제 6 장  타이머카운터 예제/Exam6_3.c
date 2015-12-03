/*******************************************************************************
���ϸ� :   Exam6_3.c
 
�� ��  :   8bit Timer/counter0�� Overflow interrupt�� �̿��Ͽ� ���� �ð����� 
           (1 sec) ��Ʈ B�� ����� LED�� �ϳ��� �̵��ϴ� ���α׷�.
   
           (16MHz)/(1024 presclaer) = 15.625KHz
		   
            T = 1/(15.625KHz) = 64usec(�޽� �ϳ��� ���ϴ� �ð�)
            64usec(�޽� �ϳ��� ���ϴ� �ð�) x 157 = 10048usec := 10msec
            10msec x 100ȸ = 1000msec = 1sec
           
Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

unsigned char count=0, data=0xFE;

void main(void)
{
         
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;    // LED off
	
	TCCR0=0x07;    // Normal mode(Timer/counte0 overflow), 1024 prescaler
    TCNT0=0xFF-157;// 10msec�� ������� Timer/counte0 value(ī��Ʈ ��)
	TIMSK=0x01;    // TOIE0(Timer/counter0 oveflow interrupt enable)
	               // ���� ���ͷ�Ʈ �ο��̺�
				   // �Ǵ� TIMSK |= (1<<TOIE0); �� ����Ͽ��� �ȴ�.
  //  EICRA|=0x08;   // INT1 ����(�ϰ�) ���� 
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
    	
	TCNT0=0xFF-157;// 10msec�� ������� Timer/counte0 value(ī��Ʈ ��)
    count++;
	if(count==100)    // 1�ʸ� ���� ī��Ʈ Ƚ��
	{
		data--;
        count=0;
	}		
    __enable_interrupt();
}
