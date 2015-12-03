//
//  Atmega128의 PORTB에 연결된 LED를 TIMER0 OVERFLOW를 이용하여 이동하는 프로그램 
//

#include<ina90.h>
#include<iom128.h>

unsigned char temp=0;


void main(void)
{
        
    DDRB = 0xFF;     // PORTB is all OUTPORT
    PORTB = 0xFF;    // LED OFF
    
    TCCR0=0x07;      // system clock/1024
    TCNT0=0x00;      // TiMER0 start at 0x00
    
    TIMSK=0x01;      // TiMER0 overflow interrupt MASK
    TIFR=0x00;
    
    __enable_interrupt();    // Global interrupt enable
    
     
    
    while(1)
    {
      PORTB = temp;
     }
}           

#pragma vector=TIMER0_OVF_vect              // Refer iom128.h !!!!!
__interrupt void T0_overflow(void)   
{
    __disable_interrupt();   
     
    TCNT0=0x00; 
        
    temp++;
            
    __enable_interrupt();
}
