/*******************************************************************************
���ϸ� :   Exam5_3.c
 
�� ��  :   INT4(PE4)�ɿ� ����ġ�� ������ �Ŀ� ����ġ�� ������ not gate�� ���Ͽ�
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

	DDRE_Bit4=0;   // PE4 �Է� DDRE4 ��Ʈ�� �ʱⰪ�� 0���� �����Ǿ� �ֱ� ������
	               // "DDRE_Bit4=0;"�� ���� �����ϴ�. INT4(PE4)��
				   // DDRE&=0xEF;

    EIMSK|=0x10;   // INT4 ���� ���ͷ�Ʈ �ο��̺�
    EICRB|=0x03;   // INT4 ��� ���� 
    EIFR|=0x10;     // INTF4 Ŭ���� 
   
     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
        
    while(1);    
}           

#pragma vector=INT4_vect              // iom128.h�� �����Ѵ�. 
__interrupt void INT4_interrrupt(void)   
{
    __disable_interrupt();   
     
    PORTB=data;
	
	data--;
            
    __enable_interrupt();
}

