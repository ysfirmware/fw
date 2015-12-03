#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include "c:/WinAVR-20100110/avr/include/avr/iom128.h"



// UART0 을 이용한 출력
void tx0Char(char message)
{
	while (((UCSR0A>>UDRE0)&0x01) == 0) ;  // UDRE, data register empty        
    UDR0 = message;
}

// UART1 을 이용한 출력
void tx1Char(char message)
{
	while (((UCSR1A>>UDRE1)&0x01) == 0) ;  // UDRE, data register empty        
    UDR1 = message;
}

static int Putchar(char c, FILE *stream)
{
	tx0Char(c);
    tx1Char(c);
	return 0;
      
}




void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xff;
 PORTD = 0x00;
 DDRD  = 0x00;
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

//UART1 initialize
// desired baud rate:9600
// actual baud rate:9615 (0.2%)
// char size: 8 bit
// parity: Disabled
void uart1_init(void)
{
 UCSR1B = 0x00; //disable while setting baud rate
 UCSR1A = 0x00;
 UCSR1C = 0x06;
 UBRR1L = 0x67; //set baud rate lo 16Mhz
 UBRR1H = 0x00; //set baud rate hi
 UCSR1B = 0x18;
}


//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
 uart0_init();              // UART 0 초기화
 uart1_init();              // UART 1 초기화
 fdevopen(Putchar,0);
 
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 //all peripherals are now initialized
}

void EEP_W(int ad,char val)
{
 eeprom_write_byte(ad,val);
}

int EEP_R(int ad)
{
 int k;
 k=eeprom_read_byte(ad);
 return k;
}

void delay_us(int time_us)
{
   register int i;
   for(i=0; i<time_us; i++){   // 4 cycle +
      asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle +
      asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle +
     /* asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle   = 16 cycle = 1us for 16MHz*/
   }
}

void delay_ms(int time_ms)
{
   register int i;
   for(i=0;i<time_ms;i++) delay_us(1000);
}

void main(void){
  unsigned char eeppeint;
	 init_devices();
	 
	    EEP_W(0,1);
		EEP_W(1,2);
		EEP_W(2,4);
		EEP_W(3,8);
		EEP_W(4,16);
	
	 while(1){
		eeppeint=EEP_R(0);
		printf("\n\r%d",eeppeint);
		delay_ms(500);

		eeppeint=EEP_R(1);
		printf("\n\r%d",eeppeint);
		delay_ms(500);

		eeppeint=EEP_R(2);
		printf("\n\r%d",eeppeint);
		delay_ms(500);

		eeppeint=EEP_R(3);
		printf("\n\r%d",eeppeint);
		delay_ms(500);

		eeppeint=EEP_R(4);
		printf("\n\r%d",eeppeint);
		delay_ms(500);		
	 }
}
