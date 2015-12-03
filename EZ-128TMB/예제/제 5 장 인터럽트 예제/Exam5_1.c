/*******************************************************************************
���ϸ� :   Exam5_1.c
 
�� ��  :   INT0(PD0)�ɿ� ����ġ�� ������ �Ŀ� ����ġ�� ������ not gate�� ���Ͽ� 
           ��, ��� �������� ���ͷ�Ʈ�� �߻��ϵ��� ������ �Ŀ�,
		   ���ͷ�Ʈ�� �߻��ϸ� ��Ʈ B�� ����� LED�� �ϳ��� �̵��ϴ�
		   ���α׷��� �ۼ��Ѵ�.
           
Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)

unsigned int data=0xFE;

void main(void)
{
         
    DDRB=0xFF;     // ��Ʈ B ��� 
    PORTB=0xFF;    // LED off

	DDRD_Bit0=0;   // PD0 �Է�
   
    EIMSK|=0x01;   // INT0 ���� ���ͷ�Ʈ �ο��̺�
    EICRA|=0x03;   // INT0 ��� ���� 
    EIFR|=0x01;     // INTF0 Ŭ���� 
   
     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
        
    while(1);
    
}           

#pragma vector=INT0_vect              // iom128.h�� �����Ѵ�. 
 __interrupt void INT0_interrrupt(void)   
{
    __disable_interrupt();   
     
    PORTB=data;
	
	data--;
            
    __enable_interrupt();
}

