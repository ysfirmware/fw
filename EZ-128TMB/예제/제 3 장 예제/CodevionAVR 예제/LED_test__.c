//
//  ATmega128의 PORTB에 연결된 LED를 테스트하는 프로그램
//
//  16MHz
 
 
#include <mega128.h>


void delay(unsigned int i)
{
    while(--i);  
    while(--i);
    while(--i);
    while(--i);
}

void main(void)
{
    unsigned int number = 0xFE;
    
    DDRB = 0xFF;
    PORTB = 0xFF;
    
    do{
        PORTB = number;
        number = (number << 1) | 0x01;
        
        if((number & 0xFF) == 0xFF)
           number = 0xFE;
        delay(60000);
        
       }while(1);
}           
    
