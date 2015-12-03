#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

void delay(int n);


const char segment_data[10] = {63, 6,91,79,102,109,125,39,127,103};

unsigned char display_num[4]={0,0,0,0};         // Seven segment 4자리 숫자 출력 버퍼


void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xff;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x0f;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x03;
}

//TIMER0 initialize - prescale:64
// WGM: Normal
// desired value: 1mSec
// actual value:  1.000mSec (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0x06; //set count
 OCR0  = 0xFA;
 TCCR0 = 0x04; //start timer
}

unsigned char digit_num=0;

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0x06; //reload counter value
    digit_num++;
    digit_num = digit_num%4;
	PORTC = 0x0f;
	PORTA = segment_data[display_num[digit_num]];
	PORTC = ~(0x01 << digit_num);

}



//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
 timer0_init();
 
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x41; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void delay(int n)
{
	volatile int i,j;
	for(i=1;i<n;i++)
	{
    	for(j=1;j<600;j++);
	}
}


//
int main(void)
{
 
 int i=0;
 init_devices();
 //insert your functional code here...
 
	while(1){
        i++;
        display_num[0] = (i%10000)/1000;
        display_num[1] = (i%1000)/100;
        display_num[2] = (i%100)/10;
        display_num[3] = (i%10);
        delay(1000);
	}


return 0;
}
