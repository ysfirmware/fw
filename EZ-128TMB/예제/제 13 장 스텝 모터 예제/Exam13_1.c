/***************************************************************************

���ϸ� :   Exam14_1.c
 
�� ��  :   ���� ���͸� �����ϴ� �⺻���� ���α׷����� PD0�� ����������
           ���� ���Ͱ� 1ȸ���ϴ� ���α׷��� �ۼ��Ѵ�.


           <���� ���� ȸ�� ����>

           PE6       PE5     PE4(OC3B)   /RESET   : Atmega128 
            |         |         |          |
           DIR      ENable    Pusle     /RESET    : L297
           ���      ���     ���       ���  
           
           <����>
		   * 3msec�� �޽�   --> 333Hz
           * 1.6msec�� �޽� --> 625Hz 

Device    : Atmega128-16AI
Clock     : 16MHz
���� ȯ�� : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER ����
#include<ina90.h>   // AVR �йи��� ���� �����Լ� ����(inavr.h, comp_a90.h)


void delay(unsigned int k)
{
    unsigned int i;

	for(i=0;i<k;i++); 
}

void PD0_key(void)
{
    //while(PIND & 0x01) != 0x00);  // PD0�� ������ ����
	//while(((PIND & 0x01) == 0x00);  // PD0�� ����
    // �Ʒ��� ���� ������ �Ѵ�.

	//while(PIND & 0x01);    // PD0�� ������ ����
	//while(!(PIND & 0x01));     // PD0�� ����

	while(!PIND_Bit0);   // PD0�� ������ ����
	while(PIND_Bit0);   // PD0�� ����
}


void main(void)
{
        
    unsigned char i;

	DDRB=0xFF;    // ��Ʈ B ���
	PORTB=0xFF;   // LED off
    
    DDRE=0xFE;    // PE7  PE6  PE5  PE4(0C3B) PE3  PE2  PE1(TXD)  PE0(RXD)
	              //  1    1    1      1       1     1     1         0 
                  //  NC  ��   ��      ��      NC    NC    ��       ��
			      //      DIR   EN   PULSE 

    PORTE=0x00;   // �ʱⰪ
	
    PORTE |= 0x20;   // L297 enable
	PORTE |= 0x40;   // (Dir=1) ---> CW 
                     // (Dir=0) ---> CCW 
    while(1)
	 {
		//PD0_key();   // PD.0�� ���������� ���

        for(i=0;i<200;i++)    // 1.8'x 200�޽� = 360' 1ȸ��
		 {	
		    PORTE |= 0x10;    // �ֱ� 1/(1.6msec) = 625Hz
			delay(130);       // Limit : 130 -> 0.1msec
            PORTE &= 0xEF;
			delay(2000);      // Limit : 2000 -> 1.5msec
		 }
     }
}               

