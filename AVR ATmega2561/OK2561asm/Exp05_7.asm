;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]   Exp05_7.asm : INT7 Interrupt Count Program - No Debouncing   []
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

        .org    0x0010
        JMP     ISR_INT7		; interrupt vector INT7

RESET:	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD
	CALL	BEEP

;------------------------------------------------
;	Initialize INT7 and Do Main Routine
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"   INT7 Count   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"      000       ",0,0

        LDI     AL,0b10000000           ; INT7 = falling edge trigger
        STS     EICRB,AL
	LDI	AL,0b10000000		; enable INT7
	OUT	EIMSK,AL
	LDI	AL,0b11111111		; clear all interrupt flags
	OUT	EIFR,AL
	SEI				; global interrupt enable

	LDI	DL,0			; initial interrupt count

LOOP1:	LDI	AL,0b00010000		; shift from LED1 to LED4
	LDI	CL,4
LOOP2:	OUT	PORTB,AL
	CALL	D200MS
	LSL	AL
	DEC	CL
	BRNE	LOOP2
	RJMP	LOOP1

;====================================================================
;	Interrupt Service Routine of INT7
;====================================================================
ISR_INT7:PUSH	AL			; store registers
        PUSH    AH
        IN      AL,SREG
        PUSH    AL

	INC	DL			; increment interrupt count

	LDI	LCD_BUFFER,0xC6		; display interrupt count
	CALL	LCD_COMMAND
	CLR	AH
	MOV	AL,DL
	CALL	LCD_3DEC

	POP	AL                      ; restore registers
        OUT     SREG,AL
        POP     AH
        POP     AL
        RETI

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
