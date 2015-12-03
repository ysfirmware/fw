;
;   ATmega128 PORTB LED test program
;

.include "m128def.inc"       ; Include ATmega128 definition file

.def Data = R16
.def Min = R17
.def Middle = R18
.def Max = R19
            
		.cseg
		.org 0x0000

	     RJMP RESET

RESET: 
           LDI 		Data, 0xFF
   	   OUT 	DDRB, Data
    
LOOP:  
	   OUT 	PORTB, Data
       	   DEC		Data
	   
	   
;DELAY:     LDI       Max, 40     ; 딜레이 구문은 시뮬레이션때문에
;DELAY1:    LDI       Middle, 40   ; 주석처리하였다. 
;DELAY2:    LDI       Min, 30
;DELAY3:    DEC      Min
;	     BRNE     DELAY3
;	     DEC      Middle
;	     BRNE     DELAY2
;	     DEC      Max
;	     BRNE     DELAY1
			   
	    RJMP LOOP


