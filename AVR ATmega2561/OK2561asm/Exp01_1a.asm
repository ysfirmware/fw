;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp01_1a.asm : Basic Assembly Program (1)	   []
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
;	Initialize ATmega2561 MCU
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

LOOP:	LDI	AL,0b01010000		; LED1,3 on
	OUT	PORTB,AL
	CALL	D500MS
	LDI	AL,0b10100000		; LED2,4 on
	OUT	PORTB,AL
	CALL	D500MS
	JMP	LOOP

;------------------------------------------------
;	Include User Subroutine File
;------------------------------------------------
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
