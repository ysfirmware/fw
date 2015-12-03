#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

void tx0Char(char message);

// UART0 을 이용한 출력
void tx0Char(char message)
{
	while (((UCSR0A>>UDRE0)&0x01) == 0) ;  // UDRE, data register empty        
    UDR0 = message;
}


static int Putchar(char c, FILE *stream)
{
	tx0Char(c);
    return 0;
      
}

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0xff;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
}


//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9615 (0.2%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x67; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x18;
}


//ADC initialize
// Conversion time: 104uS
void adc_init(void)
{
 ADCSRA = 0x00; //disable adc
 ADMUX  = 0x00; //select adc input 0
 ACSR   = 0x80;
 ADCSRA = 0x87;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
 uart0_init();
 fdevopen(Putchar,0);  
 adc_init();

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}


// 입력으로 들어오는 채널의 ADC를 스타트 시킨다.
void startConvertion(unsigned char ch)
{
	ADCSRA = ADCSRA & 0x3f;
	ADMUX = 0x60 | (ch & 0x0f);
	ADCSRA = ADCSRA | 0xc0;
}

// startConvertion() 후에 수행되며 컨터팅 된 값을 리턴한다.  
unsigned char readConvertData(void)
{
	volatile unsigned char temp;
	while((ADCSRA & 0x10)==0);
	ADCSRA = ADCSRA | 0x10; 
	temp = ADCL;
	temp = ADCH;
	ADCSRA = ADCSRA | 0x10; 
	return temp; 
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
	volatile unsigned char i, temp,right,left;
	
	init_devices();
	//insert your functional code here..
	printf(" \n\n\rADC test program...\n\r");
	PORTG = 0x01;
	delay(500);
	PORTG = 0x02;
	
	//IRS test
	
  /* 	PORTF |= 0x08;
	while(1)
	{
			DDRF=0x0f;
			PORTF&=~0x02;
			PORTF|=0x01;
	 		startConvertion(7);			// Left sensor Converting
	    	left = readConvertData();
			delay(10);
			
			PORTF&=~0x01;
			PORTF|=0x02;
	 		startConvertion(6);			// Left sensor Converting
	    	right = readConvertData();
    		printf("left : %d    right : %d \r", left,right);
			delay(10);
	}*/
	while(1){
        for(i=0; i<8; i++){
    		startConvertion(i);			// Left sensor Converting
	    	temp = readConvertData();
    		printf("ch %d  :  %d\n\r", i, temp);
        }
    	PORTG ++;
        delay(600);
		

	}
	return 0;
}
