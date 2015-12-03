#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define MOTOR_PORT		PORTC       // 스테핑 모터 연결 포트
#define MOTOR_PORT_DDR	DDRC        // 스테핑 모터 연결 포트의 DDR REG

#define MOTOR_ENABLE	(MOTOR_PORT = MOTOR_PORT|0x80)
#define MOTOR_DISABLE	(MOTOR_PORT = MOTOR_PORT&0x7f)
#define MOTOR_STEP_M0	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x00)
#define MOTOR_STEP_M1	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x10)
#define MOTOR_STEP_M2	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x20)
#define MOTOR_STEP_M3	(MOTOR_PORT = (MOTOR_PORT&0xcf)|0x30)
#define MOTOR_LEFT_CLK	(MOTOR_PORT = MOTOR_PORT^0x01)
#define MOTOR_RIGHT_CLK	(MOTOR_PORT = MOTOR_PORT^0x04)
#define MOTOR_LEFT_CW	(MOTOR_PORT = MOTOR_PORT&0xfd)
#define MOTOR_LEFT_CCW	(MOTOR_PORT = MOTOR_PORT|0x02)
#define MOTOR_RIGHT_CW	(MOTOR_PORT = MOTOR_PORT&0xf7)
#define MOTOR_RIGHT_CCW	(MOTOR_PORT = MOTOR_PORT|0x08)

volatile unsigned int ti;

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xff;
 PORTD = 0x00;
 DDRD  = 0xff;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
}

//TIMER0 initialize - prescale:8
// WGM: Normal
// desired value: 10KHz
// actual value: 10.000KHz (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0x83; //set count
 OCR0  = 0x7D;
 TCCR0 = 0x03; //start timer
}


ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0x38; //reload counter value
 MOTOR_LEFT_CLK;
 MOTOR_RIGHT_CLK;
if(++ti>=10000)
{
 ti=0;
 PORTG^=0x03;
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
	TIMSK = 0x01; //timer interrupt sources
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


int main(void)
{
 //volatile unsigned char step=0;
 init_devices();
 
 MOTOR_PORT_DDR = 0xff;

	MOTOR_ENABLE;
	MOTOR_STEP_M0;
    MOTOR_LEFT_CCW;
    MOTOR_RIGHT_CW;
	PORTG=0x01;
	
	 while(1);
				return 0;
}
