;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp09_2.asm : Telephone Bell Sound		   []
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

;------------------------------------------------
;	Telephone Bell Sound by Timer/Counter3
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"   * SOUND *    ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Telephone Bell ",0,0

        LDI     AL,0b01000000           ; set CTC mode(12), use OC3A
        STS     TCCR3A,AL
        LDI     AL,0b00011010           ; f(OC3A) = 16MHz/8/2/(1+N)
        STS     TCCR3B,AL
        LDI     AL,0b00000000
        STS     TCCR3C,AL
	CLR     AL			; clear counter/timer3
        STS     TCNT3H,AL
        STS     TCNT3L,AL

LOOP1:	LDI	CL,20
LOOP2:  LDI     AL,0b00011010           ; speaker on
        STS     TCCR3B,AL
LOOP2A:	LDS	AL,TCNT3L		; protect from glitch
	LDS	AL,TCNT3H
	CPI	AL,high(3124)
	BRSH	LOOP2A
	LDI     AL,high(3124)		; 320 Hz
        STS     ICR3H,AL
        LDI     AL,low(3124)
        STS     ICR3L,AL
	CALL	D20MS			; delay for 25 ms
	CALL	D5MS
LOOP2B: LDS	AL,TCNT3L		; protect from glitch
	LDS	AL,TCNT3H
	CPI	AL,high(2082)
	BRSH	LOOP2B
	LDI     AL,high(2082)		; 480 Hz
        STS     ICR3H,AL
        LDI     AL,low(2082)
        STS     ICR3L,AL
	CALL	D20MS			; delay for 25 ms
	CALL	D5MS
	DEC	CL
	BRNE	LOOP2
        LDI     AL,0b00011000           ; speaker off
        STS     TCCR3B,AL
	CALL	D1SEC			; delay for 1 sec
	RJMP	LOOP1

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
