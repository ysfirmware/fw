;
;   Timer overflow program with ATmega128
;

.include "m128def.inc"



; Interrupt vectors

.ORG	$0000
		RJMP 	RESET

.ORG	OVF0addr
		RJMP 	TIMER_OV


.def TEMP = R16
.def TIME = R17
.def medium = R18
.def coarse = R19


RESET: 
		LDI		TEMP,	LOW(RAMEND)
		OUT		SPL,	TEMP
		LDI		TEMP,	HIGH(RAMEND)
		OUT		SPH,	TEMP


		LDI		TIME, 	$80
		OUT		TCNT0,	TIME

		LDI		TEMP,	(1<<TOIE0)		; Timer0 overflow interrupt enable
		OUT		TIMSK,	TEMP

		LDI		TEMP,	$05             ; Clock prescler = CLK/1024
		OUT		TCCR0,	TEMP


		SEI
 
        LDI 	temp,	0xff
        OUT 	DDRB, temp

LOOP: 
        OUT 	PORTB, 	TEMP
      
	   
      
	    RJMP LOOP




TIMER_OV: 
        CLI

		OUT		TCNT0,	TIME
        
		DEC 	TEMP 

		SEI
        RETI

