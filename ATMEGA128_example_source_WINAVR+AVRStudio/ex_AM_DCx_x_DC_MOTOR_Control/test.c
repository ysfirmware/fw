#include <avr/io.h>
#include <avr/interrupt.h>

#define MOTOR_PORT 		PORTC
#define PWM1_ON			(MOTOR_PORT|=0x01)
#define PWM1_OFF		(MOTOR_PORT&=0xFE)
#define DIR1_ON			(MOTOR_PORT|=0x02)
#define DIR1_OFF 		(MOTOR_PORT&=0xFD)
#define ENABLE1_OFF		(MOTOR_PORT|=0x04)
#define ENABLE1_ON		(MOTOR_PORT&=0xFB)
#define BREAK1_ON		(MOTOR_PORT|=0x08)
#define BREAK1_OFF 		(MOTOR_PORT&=0xF7)

#define PWM2_ON			(MOTOR_PORT|=0x10)
#define PWM2_OFF		(MOTOR_PORT&=~0x10)
#define DIR2_ON			(MOTOR_PORT|=0x20)
#define DIR2_OFF 		(MOTOR_PORT&=~0x20)
#define ENABLE2_OFF		(MOTOR_PORT|=0x40)
#define ENABLE2_ON		(MOTOR_PORT&=~0x40)
#define BREAK2_ON		(MOTOR_PORT|=0x80)
#define BREAK2_OFF 		(MOTOR_PORT&=~0x80)



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
 DDRG  = 0xff;
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
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void delay(int delaytime){
	 int i,j;
	 for(i=0;i<1000;i++){
	 	for(j=0;j<delaytime;j++){
		}
	 }
}

int main(void)
{
 init_devices();
 
 ENABLE1_ON;
 PWM1_ON;
 BREAK1_OFF;
 ENABLE2_ON;
 PWM2_ON;
 BREAK2_OFF; 
 
 while(1){
 	 DIR1_ON;		
	 DIR2_ON;	
	 PORTG = 0x01;	
	 delay(2000);
   	 DIR1_OFF;
	 DIR2_OFF;	 
	 PORTG = 0x02;
	 delay(2000);
  }
  return 0;
}

