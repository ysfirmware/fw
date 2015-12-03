;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp06_3.asm : Timer/Counter1 and 3 Interrupt	   []
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

        .org    0x0022
        JMP     TIMER1_COMPA		; interrupt vector OC1A

        .org    0x0040
        JMP     TIMER3_COMPA		; interrupt vector OC3A

RESET:	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD
	CALL	BEEP

;------------------------------------------------
;	Initialize Timer/Counter1 CTC Mode
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" Timer1 = 00000 ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Timer3 = 00000 ",0,0

        LDI     AL,0b01010000           ; LED3 and 1 on
	OUT	PORTB,AL

        LDI     AL,0b00000000           ; CTC mode(4), don't output OC1A
        STS     TCCR1A,AL
        LDI     AL,0b00001100           ; 16MHz/256/(1+62499) = 1Hz
        STS     TCCR1B,AL
        LDI     AL,0b00000000
        STS     TCCR1C,AL
        LDI     AL,high(62499)
        STS     OCR1AH,AL
        LDI     AL,low(62499)
        STS     OCR1AL,AL
        LDI     AL,0x00                 ; clear Timer/Counter1
        STS     TCNT1H,AL
        STS     TCNT1L,AL

	CLR	R27			; clear Timer/Counter1 interrupt counter
	CLR	R26

;------------------------------------------------
;	Initialize Timer/Counter3 CTC Mode
;------------------------------------------------
        LDI     AL,0b00000000           ; CTC mode(4), don't output OC3A
        STS     TCCR3A,AL
        LDI     AL,0b00001101           ; 16MHz/1024/(1+3124) = 5Hz
        STS     TCCR3B,AL
        LDI     AL,0b00000000
        STS     TCCR3C,AL
        LDI     AL,high(3124)
        STS     OCR3AH,AL
        LDI     AL,low(3124)
        STS     OCR3AL,AL
        LDI     AL,0x00                 ; clear Timer/Counter3
        STS     TCNT3H,AL
        STS     TCNT3L,AL

	CLR	R29			; clear Timer/Counter3 interrupt counter
	CLR	R28

;------------------------------------------------
;	Initialize OC1A and OC3A Interrupt
;------------------------------------------------
        LDI     AL,0b00000010           ; enable Timer/Counter1 OC1A interrupt
        STS     TIMSK1,AL
        LDI     AL,0b00101111           ; clear all interrupt flags
        STS     TIFR1,AL
        LDI     AL,0b00000010           ; enable Timer/Counter3 OC3A interrupt
        STS     TIMSK3,AL
        LDI     AL,0b00101111           ; clear all interrupt flags
        STS     TIFR3,AL
        SEI                             ; global interrupt enable

LOOP:   LDI	LCD_BUFFER,0x8A		; display OC1A interrupt count
	CALL	LCD_COMMAND
	MOV	AH,R27
	MOV	AL,R26
	CALL	LCD_5DEC
	LDI	LCD_BUFFER,0xCA		; display OC3A interrupt count
	CALL	LCD_COMMAND
	MOV	AH,R29
	MOV	AL,R28
	CALL	LCD_5DEC
	RJMP    LOOP			; loop and wait interrupt

;====================================================================
;	Interrupt Service Routine of OC1A
;====================================================================
TIMER1_COMPA:
        PUSH    AL                      ; store registers
        PUSH    AH
        IN      AL,SREG
        PUSH    AL

        IN      AL,PORTB                ; toggle LED1
	LDI     AH,0b00010000
	EOR     AL,AH
	OUT	PORTB,AL

	ADIW	R26,1			; increment interrupt counter

        POP     AL                      ; restore registers
        OUT     SREG,AL
        POP     AH
        POP     AL
        RETI

;====================================================================
;	Interrupt Service Routine of OC3A
;====================================================================
TIMER3_COMPA:
        PUSH    AL                      ; store registers
        PUSH    AH
        IN      AL,SREG
        PUSH    AL

        IN      AL,PORTB                ; toggle LED3
	LDI     AH,0b01000000
	EOR     AL,AH
	OUT	PORTB,AL

	ADIW	R28,1			; increment interrupt counter

        POP     AL                      ; restore registers
        OUT     SREG,AL
        POP     AH
        POP     AL
        RETI

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
