#include <avr/io.H>

#define relay_p PORTC

#define relay1(n)  	 n? (relay_p|=0x01) : (relay_p&=~0x01) //ch1
#define relay2(n)  	 n? (relay_p|=0x02) : (relay_p&=~0x02) //ch2
#define relay3(n)  	 n? (relay_p|=0x04) : (relay_p&=~0x04) //ch3
#define relay4(n)  	 n? (relay_p|=0x08) : (relay_p&=~0x08) //ch4

// Relay powersupply module Control




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

void init_devices(void)
{
 //stop errant interrupts until set up
 XDIV  = 0x00; //xtal divider
 XMCRA = 0x00; //external memory
 port_init();
 
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 //all peripherals are now initialized
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
   for(i=0;i<time_ms;i++) delay_us(800);
}

//
void main(void)
{
   init_devices();
    
/*
	while(1)
	{
		relay2(1);
	}
  */ 
   while(1)
   {
	relay1(1);
	delay_ms(2000);
	relay1(0);
	relay2(1);	
	delay_ms(2000);
	relay2(0);
	relay3(1);	
	delay_ms(2000);
	relay3(0);
	relay4(1);
	delay_ms(2000);
	relay4(0);
   }
}

