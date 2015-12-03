;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp06_2.asm : Timer/Counter0 Interrupt(250Hz)	   []
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
        JMP     RESET                   ; power-on reset entry point

        .org    0x002A
        JMP     TIMER0_COMPA		; interrupt vector OC0A

RESET:	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD
	CALL	BEEP

;------------------------------------------------
;	Initialize Timer/Counter0 CTC Mode
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"     Timer0     ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"250 Hz Interrupt",0,0

        LDI     AL,0b00110000           ; LED2 and 1 on
	OUT	PORTB,AL

        LDI     AL,0b00000010           ; CTC mode(2), don't output OC0A
        OUT     TCCR0A,AL
        LDI     AL,0b00000100		; 16MHz/256/(1+249) = 250Hz
        OUT     TCCR0B,AL
        LDI     AL,249
        OUT     OCR0A,AL
        LDI     AL,0x00                 ; clear Timer/Counter0
        OUT     TCNT0,AL

;------------------------------------------------
;	Initialize OC0A Interrupt
;------------------------------------------------
        LDI     AL,0b00000010           ; enable Timer/Counter0 OC0A interrupt
        STS     TIMSK0,AL
        LDI     AL,0b00000111           ; clear all interrupt flags
        OUT     TIFR0,AL
        SEI                             ; global interrupt enable

	LDI	CL,250			; interrupt counter

LOOP:   RJMP    LOOP                    ; wait interrupt

;====================================================================
;	Interrupt Service Routine of OC0A
;====================================================================
TIMER0_COMPA:
        PUSH    AL                      ; store registers
        PUSH    AH
        IN      AL,SREG
        PUSH    AL

        IN      AL,PORTB                ; toggle LED1
	LDI     AH,0b00010000
	EOR     AL,AH
	OUT	PORTB,AL

	DEC	CL			; 250th interrupt ?
	BRNE	TIMER0_C
	LDI	CL,250			; if yes, toggle LED2
        IN      AL,PORTB
	LDI     AH,0b00100000
	EOR     AL,AH
	OUT	PORTB,AL

TIMER0_C:POP    AL                      ; restore registers
        OUT     SREG,AL
        POP     AH
        POP     AL
        RETI

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
