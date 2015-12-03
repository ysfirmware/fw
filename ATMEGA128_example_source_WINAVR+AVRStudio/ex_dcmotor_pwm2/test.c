#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

// DC MOTOR Control
#define MOTOR_PORT 		PORTC
#define PWM_PORT		0x00
#define PWM1_ON			(MOTOR_PORT|=0x01)
#define PWM1_OFF		(MOTOR_PORT&=0xFE)
#define DIR1_ON			(MOTOR_PORT|=0x02)
#define DIR1_OFF 		(MOTOR_PORT&=0xFD)
#define ENABLE1_OFF		(MOTOR_PORT|=0x04)
#define ENABLE1_ON		(MOTOR_PORT&=0xFB)
#define BREAK1_ON		(MOTOR_PORT|=0x08)
#define BREAK1_OFF 		(MOTOR_PORT&=0xF7)

#define PWM2_ON			(MOTOR_PORT|=0x10)
#define PWM2_OFF		(MOTOR_PORT&=0xEF)
#define DIR2_ON			(MOTOR_PORT|=0x20)
#define DIR2_OFF 		(MOTOR_PORT&=0xDF)
#define ENABLE2_OFF		(MOTOR_PORT|=0x40)
#define ENABLE2_ON		(MOTOR_PORT&=0xBF)
#define BREAK2_ON		(MOTOR_PORT|=0x80)
#define BREAK2_OFF 		(MOTOR_PORT&=0x7F)


#define LEFT			1
#define RIGHT			2
#define STOP			3

// SPEED Control
#define MIN_SPEED 1
#define MAX_SPEED 1700
#define SPEED_UP  1

// SWITCH Control
#define SW_PORT	  	  	PINA
#define SW_PORT_DDR		DDRA
#define PRESSED			1
#define OPEN			0

volatile unsigned char num=PWM_PORT;
volatile unsigned char motor_state=LEFT, motor2_state=LEFT;
volatile unsigned char T20ms=0, T2_20ms=0;
volatile unsigned int DCMOTOR[8], sw_state[8]={0,0,0,0,0,0,0,0};
volatile unsigned int Tcount=0,T2_count=0;
volatile unsigned int motor_speed,motor2_speed;

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

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0x06; //reload counter value
 T20ms++;
 if(T20ms >= 20)
 {
	T20ms = 0;
	Tcount = 0;
 }
 
 T2_20ms++;
 if(T2_20ms >= 20)
 {
	T2_20ms = 0;
	T2_count = 0;
 }
}

//TIMER2 initialize - prescale:64
// WGM: Normal
// desired value: 100uSec
// actual value: 100.000uSec (0.0%)
void timer2_init(void)
{
 TCCR2 = 0x00; //stop
 TCNT2 = 0xEC; //setup
 OCR2  = 0x14;
 TCCR2 = 0x02; //start
}

ISR(TIMER2_OVF_vect)
{
 TCNT2 = 0xE7; //reload counter value
 Tcount++;
 T2_count++;
 
 if(DCMOTOR[0]>Tcount)
    MOTOR_PORT |= (0x01<<num);
 else	
 	MOTOR_PORT &= (~(0x01<<num));	
	
 if(DCMOTOR[4]>Tcount)
    MOTOR_PORT |= (0x01<<4);
 else	
 	MOTOR_PORT &= (~(0x01<<4));	
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
 timer2_init();

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
	 unsigned int i, j;
	 for(i=0;i<cnt;i++){
		for(j=0;j<1000;j++)
		{
		}
	}
}

void motor_init(void){
	 ENABLE1_ON;
 	 PWM1_ON;
 	 BREAK1_OFF; 
	 DIR1_ON;
 	 
	 ENABLE2_ON;
 	 PWM2_ON;
 	 BREAK2_OFF; 
	 DIR2_ON;
}

//
int main(void)
{
 
 init_devices();
 //insert your functional code here...
 motor_init();
 SW_PORT_DDR = 0x00;
 DCMOTOR[0] =  motor_speed = 800;
 DCMOTOR[4] = motor2_speed = 100;   
 
 while(1){ 		
	
	switch(SW_PORT&0xff){
		case 0x01 :  motor_speed+=SPEED_UP;
			 	  	 break;
		case 0x02 :  motor_speed-=SPEED_UP;
			 	  	 break;
		case 0x04 :  sw_state[2] = PRESSED;			 	  	 		 	  	 	
			 	  	 break; 
		case 0x08 :  sw_state[3] = PRESSED;			 	  	
			 	  	 break;
		
		case 0x10 :  motor2_speed+=SPEED_UP;
			 	  	 break;
		case 0x20 :  motor2_speed-=SPEED_UP;
			 	  	 break;
		case 0x40 :  sw_state[4] = PRESSED;			 	  	 		 	  	 	
			 	  	 break; 
		case 0x80 :  sw_state[5] = PRESSED;			 	  	
			 	  	 break;
					 			 
		case 0x00 : if(sw_state[2] == PRESSED){
			 	  		if(motor_state == STOP){
					 		motor_state = LEFT;
					 	}else{
			 	  	 		motor_state = STOP;
					 	}
						sw_state[2] = OPEN;
					}
					if(sw_state[3] == PRESSED){
			 	  		if(motor_state == STOP){
					 		motor_state = RIGHT;
					 	}else{
			 	  	 		motor_state = STOP;
					 	}
						sw_state[3] = OPEN;
					}
					
					if(sw_state[4] == PRESSED){
			 	  		if(motor2_state == STOP){
					 		motor2_state = LEFT;
					 	}else{
			 	  	 		motor2_state = STOP;
					 	}
						sw_state[4] = OPEN;
					}
					if(sw_state[5] == PRESSED){
			 	  		if(motor2_state == STOP){
					 		motor2_state = RIGHT;
					 	}else{
			 	  	 		motor2_state = STOP;
					 	}
						sw_state[5] = OPEN;
					}
					break;
		
		default : break;
	}
	
	if(motor_speed >= MAX_SPEED){
		motor_speed = MAX_SPEED;  
	}
	if(motor_speed <= MIN_SPEED){
		motor_speed = MIN_SPEED;  
	}
	if(motor2_speed >= MAX_SPEED){
		motor2_speed = MAX_SPEED;  
	}
	if(motor2_speed <= MIN_SPEED){
		motor2_speed = MIN_SPEED;  
	}

	switch(motor_state){
		case LEFT  : BREAK1_OFF;
			 	   	 DIR1_ON;
			 	   	 break; 
		case RIGHT : BREAK1_OFF;
			 	   	 DIR1_OFF;
			 	   	 break;
		case STOP  : BREAK1_ON;
			 	   	 break;
		default : break;
	}
	
	switch(motor2_state){
		case LEFT  : BREAK2_OFF;
			 	   	 DIR2_ON;
			 	   	 break; 
		case RIGHT : BREAK2_OFF;
			 	   	 DIR2_OFF;
			 	   	 break;
		case STOP  : BREAK2_ON;
			 	   	 break;
		default : break;
	}	
		
	DCMOTOR[0] = motor_speed;
	DCMOTOR[4] = motor2_speed;
	delay(5);
	

}
return 0;
}
