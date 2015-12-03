/*******************************************************************************
���ϸ� :   Exam4_1.c
 
�� ��  :   Atmega128�� PD0�� KEY(Ǫ�� ����ġ)�� ����Ǿ� �ִ�. �� KEY �Է��� �޾�
           ������ ������ ������ ����. ��� ����, ������ �������� ����.

Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)


void main(void)
{
    unsigned char number=0xFE;

	DDRB=0xFF;     
	PORTB=0xFF;   // LED�� ����� ��Ʈ

	DDRD&=0xFE;    // PD0 �Է�, ������ ���
	PORTD=0x00;    //�ܺ� Ǯ�� ���� ���
   
	do{      
		while(PIND & 0x01);     // PD0�� ������ �������� ����
		while(!(PIND & 0x01));  // PD0�� ����
		     
	    PORTB = number;	// ������ ����� LED�ʱⰪ�� PORTB�� ��� 
	    number = (number << 1) | 0x01; 
	      // �������� �������� ��ĭ shift�ѵ� 0x01���� ���Ͽ� ��������Ʈ�� LED�� off 
	    if((number & 0xff) == 0xff)	 
		   number = 0xfe;		
	     // ���� �����ǰ��� 0xff�� ���Ͽ� �� ���� 0xFF�̸�(8���� ��Ʈ�� ��� shift�Ǹ�)
		 // ������ LED�ʱⰪ�� �ٽ� ����     
		     
      }while(1);
}

