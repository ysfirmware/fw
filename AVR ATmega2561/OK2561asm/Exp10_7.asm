;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	    Exp10_7.asm : Analog Comparator - VR1 vs 1.1V	   []
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
;	Comparator ADC3 with 1.1V
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"  Analog Comp.  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"   VR1 < 1.1V   ",0,0

	LDI	AL,0b01000011		; +input = 1.1V
	OUT	ACSR,AL
	LDI	AL,0b00000000		; ADEN = 0
	STS	ADCSRA,AL
	LDI	AL,0b01000000		; ACME = 1
	STS	ADCSRB,AL
	LDI	AL,0b00000011		; -input = ADC3
	STS	ADMUX,AL

LOOP:	IN	AL,ACSR
	SBRS	AL,ACO
	RJMP	ADC3HIGH
	LDI	AL,0b00010000		; if ADC3 < 1.1V, LED1 on
	OUT	PORTB,AL
	LDI	LCD_BUFFER,0xC7		;   and display "<"
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,'<'
	CALL	LCD_DATA
	CALL	D100MS
	RJMP	LOOP

ADC3HIGH:LDI	AL,0b10000000		; if ADC3 > 1.1V, LED4 on
	OUT	PORTB,AL
	LDI	LCD_BUFFER,0xC7		;   and display ">"
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,'>'
	CALL	LCD_DATA
	CALL	D100MS
	RJMP	LOOP

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
