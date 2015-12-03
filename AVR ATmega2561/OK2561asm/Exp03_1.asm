;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		    Exp03_1.asm : Bit Access			   []
;[]								   []
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;
;		Designed and programmed by Duck-Yong Yoon in 2006.
;
;------------------------------------------------
;	Include Header File
;------------------------------------------------
.nolist
.include "m2561def.inc"			; ATmega2561 definition file
.list

;------------------------------------------------
;	Initialize MCU
;------------------------------------------------
.cseg
	.org	0x0000
	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

;------------------------------------------------
;	Blink LED1/3 and LED2/4
;------------------------------------------------
	LDI	AL,0xFF			; initialize PORTB = output
	OUT	DDRB,AL

LOOP:	CBI	PORTB,PB7		; LED1,3 on
	SBI	PORTB,PB6
	CBI	PORTB,PB5
	SBI	PORTB,PB4
	CALL	D500MS
	LDI	AL,(1<<PB7)|(1<<PB5)	; LED2,4 on
	OUT	PORTB,AL
	CALL	D500MS
	JMP	LOOP

;------------------------------------------------
;	Include User Subroutine File
;------------------------------------------------
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
