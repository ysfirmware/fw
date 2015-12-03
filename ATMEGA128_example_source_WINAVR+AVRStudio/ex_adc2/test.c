#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

static int ADdata[8];

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
 DDRE  = 0xff;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x01;
 DDRG  = 0xff;
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
 adc_init();
 fdevopen(Putchar,0);  

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
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

void LED_delay(int n)
{
	volatile int i,j;
	for(i=1;i<n;i++)
	{
    	for(j=1;j<600;j++);
	}
}

void Adc_Start( void )
{
 ADMUX = 0x00;
 ADCSRA = 0xc7;  //AD enable, ADSC:1, free runnung:0,ADIF,ADIE:0
}

void Adc_Stop( void )
{
 ADMUX = 0x00;
 ADCSRA = 0x07; // Adc disable
}

unsigned int Adc_Data( unsigned char mux )
{
 unsigned int value = 0x0000;
 ADCSRA &=0x3f;  // Adc_disable, ADSC:0
 ADMUX |= mux;
 ADCSRA |= 0xC7; // Adc_Enable
 delay(5);
 
 value = (ADCL)&0x00ff;
 value |=  (ADCH << 8);
 value &= 0x03ff; 
 //delay(25);
 return value; 
}

int main(void)
{
	
	int i=0;
	unsigned int right,left;
	
	init_devices();
	//insert your functional code here...
	printf(" \n\n\rADC test program...\n\r");
	PORTG = 0x03;
	delay(1000);
	PORTG = 0x00;
  
	PORTA = 0x04;
	Adc_Start();
	delay(25);
	
	
	/*while(1){	
			ADdata[i] = Adc_Data(i);
			printf("  ",PINA);
			printf("C[%d]:%d ", i,ADdata[i]);
			Adc_Stop();
			i++;
			
			if(i>7){ 
				i=0;
				printf("\r\nADC Data");
				delay(100);
			}
			PORTG ++;
	}*/

	//IRS TEST

	while(1)
	{
			DDRF=0x0f;
			PORTF&=~0x02;
			PORTF|=0x01;
	 				
	    	left = Adc_Data(7);       //
			delay(10);
			
			PORTF&=~0x01;
			PORTF|=0x02;
	 					
	    	right = Adc_Data(6);
    		printf("left : %d    right : %d \r", left,right);
			delay(10);
	}

	return 0;
}
