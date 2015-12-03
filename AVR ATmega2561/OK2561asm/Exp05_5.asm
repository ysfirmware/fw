;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	 Exp05_5.asm : INT7 Interrupt Program - Level Trigger	   []
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
;	Initialize INT7
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" INT7 Interrupt ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"(Level Trigger) ",0,0

        LDI     AL,0b00000000           ; INT7 = level trigger
        STS	EICRB,AL
	LDI	AL,0b10000000		; enable INT7
	OUT	EIMSK,AL
	LDI	AL,0b11111111		; clear all interrupt flags
	OUT	EIFR,AL
	SEI				; global interrupt enable

LOOP:   RJMP    LOOP                    ; wait interrupt

;====================================================================
;	Interrupt Service Routine of INT7
;====================================================================
ISR_INT7:PUSH	AL			; store registers
        PUSH    CL
        IN      AL,SREG
        PUSH    AL

	LDI	AL,0b00010000		; shift from LED1 to LED4
	LDI	CL,5
ISR_INT7A:OUT	PORTB,AL
	CALL	D200MS
	LSL	AL
	DEC	CL
	BRNE	ISR_INT7A

	POP	AL                      ; restore registers
        OUT     SREG,AL
        POP     CL
        POP     AL
        RETI

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
