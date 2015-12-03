/***************************************************************************

파일명 :   Exam14_1.c
 
동 작  :   스텝 모터를 구동하는 기본적인 프로그램으로 PD0을 누를때마다
           스텝 모터가 1회전하는 프로그램을 작성한다.


           <스텝 모터 회로 연결>

           PE6       PE5     PE4(OC3B)   /RESET   : Atmega128 
            |         |         |          |
           DIR      ENable    Pusle     /RESET    : L297
           출력      출력     출력       출력  
           
           <참고>
		   * 3msec의 펄스   --> 333Hz
           * 1.6msec의 펄스 --> 625Hz 

Device    : Atmega128-16AI
Clock     : 16MHz
개발 환경 : IAR EWAVR 2.28A
*******************************************************************************/

#include<iom128.h>  // ATMEGA128 IO REGISTER 정의
#include<ina90.h>   // AVR 패밀리를 위한 내장함수 정의(inavr.h, comp_a90.h)


void delay(unsigned int k)
{
    unsigned int i;

	for(i=0;i<k;i++); 
}

void PD0_key(void)
{
    //while(PIND & 0x01) != 0x00);  // PD0을 누르는 동안
	//while(((PIND & 0x01) == 0x00);  // PD0을 떼면
    // 아래와 같은 동작을 한다.

	//while(PIND & 0x01);    // PD0를 누르는 동안
	//while(!(PIND & 0x01));     // PD0를 떼면

	while(!PIND_Bit0);   // PD0를 누르는 동안
	while(PIND_Bit0);   // PD0를 떼면
}


void main(void)
{
        
    unsigned char i;

	DDRB=0xFF;    // 포트 B 출력
	PORTB=0xFF;   // LED off
    
    DDRE=0xFE;    // PE7  PE6  PE5  PE4(0C3B) PE3  PE2  PE1(TXD)  PE0(RXD)
	              //  1    1    1      1       1     1     1         0 
                  //  NC  출   출      출      NC    NC    출       입
			      //      DIR   EN   PULSE 

    PORTE=0x00;   // 초기값
	
    PORTE |= 0x20;   // L297 enable
	PORTE |= 0x40;   // (Dir=1) ---> CW 
                     // (Dir=0) ---> CCW 
    while(1)
	 {
		//PD0_key();   // PD.0을 누를때까지 대기

        for(i=0;i<200;i++)    // 1.8'x 200펄스 = 360' 1회전
		 {	
		    PORTE |= 0x10;    // 주기 1/(1.6msec) = 625Hz
			delay(130);       // Limit : 130 -> 0.1msec
            PORTE &= 0xEF;
			delay(2000);      // Limit : 2000 -> 1.5msec
		 }
     }
}               

