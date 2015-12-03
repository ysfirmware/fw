;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	        Exp03_5.asm : Infinite Loop(2) - Repeat		   []
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

;====================================================================
;	Main Program
;====================================================================
.cseg
	.org	0x0000
	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD
	CALL	BEEP

;------------------------------------------------
;	Key Input and Display
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" Infinite Loop  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"     Repeat     ",0,0

LOOPX:	LDI	CL,4			; shift from LED1 to LED4
	LDI	AL,0b00010000		; initial LED data

LOOP:	OUT	PORTB,AL		; output LED data
	CALL	D500MS
	LSL	AL			; shift left
	DEC	CL
	BRNE	LOOP

	RJMP	LOOPX			; infinite loop

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
