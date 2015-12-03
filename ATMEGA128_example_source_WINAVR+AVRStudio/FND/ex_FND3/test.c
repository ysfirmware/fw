#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

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


//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
  
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x41; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void delay(int times)
{
int	 i,j;
	 for (i=0; i<times; i++) {
	 	 for (j=0;j<i;j++);
	 }
}

void SS_val(int PC, int PA, int D_time)
{
 		  PORTC = ~PC;
		  PORTA = PA;
		  delay(D_time);

}

//
int main(void)
{
int	 D_time=1;
int	 SS_data[10]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 
	 			  0x6d, 0x7d, 0x07, 0x7f, 0x6f};
int	 SS_PORTC[5]={0, 0x01, 0x02, 0x04, 0x08};
/*-------------------------------------------------------------------*/
int	 what=3232;
int where[4]={0,0,0,0};
	 
	 where[3] = what/1000;
	 where[2] = (what - where[3]*1000)/100;
	 where[1] = (what - where[3]*1000 - where[2]*100)/10;
	 where[0] = (what - where[3]*1000 - where[2]*100 - where[1]*10);
/*------------------------------------------------------------------*/


 init_devices();
 //insert your functional code here...
 while(1) {

 		  			SS_val(SS_PORTC[1], SS_data[where[3]], D_time);
 		  			SS_val(SS_PORTC[2], SS_data[where[2]], D_time);
					SS_val(SS_PORTC[3], SS_data[where[1]], D_time);
 		  			SS_val(SS_PORTC[4], SS_data[where[0]], D_time);
		  		
	}


return 0;
}
