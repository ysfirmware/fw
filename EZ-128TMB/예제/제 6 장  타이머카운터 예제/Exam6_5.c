/*******************************************************************************
���ϸ� :   Exam6_5.c
 
�� ��  :   OC0(PB4)���� CTC ����� �̿��Ͽ� �ֱ������� LED�� 
           ��� ���� ����Ѵ�. �׷��� ���ͷ�Ʈ�� �̿��Ͽ� �ֱ⸦ �����ϴ�.
 
           ���ļ� = (16MHz)/(1024 Prescaler) = 15625 Hz
		   T = 1/15625 = 0.000064 sec = 64 usec
           
Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

unsigned char value=1;


void OC0_CTC(unsigned char duty)
{
	OCR0=duty;
} 

void main( void )
{       
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;
   
    DDRB_Bit4 = 1; // PORTB.4�� OC0�� �����Ѵ�.

    TCCR0=0x1F;    // (0001 1111) CTC ���, ���, 1024 prescaler
    TCNT0=0x00;    // Ÿ�̸�0 �ʱⰪ
    
    OC0_CTC(0x50); // OCR0�� TCNT0�� ������ OC0 �� ��� 
                   // �� ����� �ֱ�� 0x00~0x30������ �ȴ�. 

    TIFR |= 0x02;  // OUTPUT COMPARE FLAG 0 Ŭ����
    TIMSK=0x02;    // OUTPUT COMPARE 0 interrupt MASK
    
     __enable_interrupt();    // Global interrupt enable

    while(1);    
} 


#pragma vector=TIMER0_COMP_vect         // iom128.h�� �����Ѵ�.
__interrupt void TIMER0_COMP(void)   
{
    __disable_interrupt();   
     
    TCNT0=0x00; 
        
    if(value == 1)
      {
         OC0_CTC(0xE0);  // Ton = 0xE0(224) X 64usec = 14.336msec
	     value=0;
       }	      
     else
	  {
	     OC0_CTC(0x50); // Toff = 0x50(80) X 64usec = 5.120msec              
         value=1;
      }
             
    __enable_interrupt();
}
    
