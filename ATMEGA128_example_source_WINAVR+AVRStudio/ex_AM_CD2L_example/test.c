#include <avr/io.h>
#include <avr/interrupt.h> 

#define MOTORPORT1 PORTC

#define ON		   1
#define OFF		   0
#define CW		   1
#define CCW		   0

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

void M1A(int onoff){
 if(onoff==ON)
 	MOTORPORT1 = MOTORPORT1|0x01;
 else
 	MOTORPORT1 = MOTORPORT1&0xFE;  
}

void M1A_(int onoff){
 if(onoff==ON)
 	MOTORPORT1 = MOTORPORT1|0x02;
 else
 	MOTORPORT1 = MOTORPORT1&0xFD;  
}

void M1B(int onoff){
 if(onoff==ON)
 	MOTORPORT1 = MOTORPORT1|0x04;
 else
 	MOTORPORT1 = MOTORPORT1&0xFB;  
}

void M1B_(int onoff){
 if(onoff==ON)
 	MOTORPORT1 = MOTORPORT1|0x08;
 else
 	MOTORPORT1 = MOTORPORT1&0xF7;  
}


void Motor1(int CWCCW){
 if(CWCCW==CW){
 	M1A(ON);
	M1A_(OFF);
 }else{
 	M1A(OFF);
	M1A_(ON);
 }
}

void Motor2(int CWCCW){
 if(CWCCW==CW){
 	M1B(ON);
	M1B_(OFF);
 }else{
 	M1B(OFF);
	M1B_(ON);
 }
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
 
 while(1){
 	Motor1(0);
	Motor2(0);
	delay(1000);
 	Motor1(1);
	Motor2(1);
	delay(1000); 
 }
 return 0;
}

