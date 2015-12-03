#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

#define digits 4 

      char jari [digits];
	  const char a[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};
	  void aaa(void);
void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xff;
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

int i=0,loop;
char ff=1;

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0x0f; //reload counter value
 
 	   
	 aaa();
	   
	   loop++;
	   
	   if (loop <0)
	   {
	   	  aaa();
		  loop=0;
		  }
		   
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

void delay(int cnt)
{
 int i,j;
 	 for (i=0; i<cnt; i++)
	 	 for(j=0; j<1000; j++);
}		

void aaa(void)
{
 static int a=0;
 
 		switch ( a )
		{
		 	   case 0:
		 	    	PORTC=0xfe;
			  		PORTA=jari[0];
			  		break;
			  
			   case 1:
		 	   		PORTC=0xfd;
			   		PORTA=jari[1];
			   		break;
			  
			   case 2:
		 	     	PORTC=0xfb;
			 	  	PORTA=jari[2];
			 	    break;
			  
		 	  case 3:
		  	   	   PORTC=0xF7;
			       PORTA=jari[3];
			 	   break;	
			  }
			  
			  a++;

			  if (a>3)
   			  a=0;
 }

//
int main(void)
{
 
 init_devices();

 jari[0] = a[1];  //1번 FND
 		jari[1] = a[3];  //2번 FND
			   jari[2] = a[5];  //3번 FND
			   		  jari[3] = a[7];  //4번 FND
 
	while (1)
		{
			}


return 0;
}
