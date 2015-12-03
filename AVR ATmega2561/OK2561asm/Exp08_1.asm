;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	    Exp08_1.asm : Digital Clock on Text LCD Module	   []
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

RESET:	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD
	CALL	BEEP

;------------------------------------------------
;	Initialize Digital Clock
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" Digital Clock  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  12:00:00 AM   ",0,0

        LDI     AL,0b00000000           ; CTC mode(4), don't output OC1A
        STS     TCCR1A,AL
        LDI     AL,0b00001100           ; interrupt = 16MHz/256/(1+62499) = 1Hz
        STS     TCCR1B,AL
        LDI     AL,0b00000000
        STS     TCCR1C,AL
        LDI     AL,high(62499)
        STS     OCR1AH,AL
        LDI     AL,low(62499)
        STS     OCR1AL,AL

        LDI     AL,0b00000010           ; enable Timer/Counter1 OC1A interrupt
        STS     TIMSK1,AL
        LDI     AL,0b00101111           ; clear all interrupt flags
        STS     TIFR1,AL
        SEI                             ; global interrupt enable

	LDI	DH,'A'			; DH = AMPM = 'A'
	LDI	DL,12			; DL = hour = 12
	LDI	CH,0			; CH = minute = 00
	LDI	CL,0			; CL = second = 00

LOOP:   RJMP    LOOP                    ; wait interrupt

;====================================================================
;	Interrupt Service Routine of OC1A
;====================================================================
TIMER1_COMPA:
        IN      AL,SREG			; store registers
        PUSH    AL

	INC	CL			; increment second
	CPI	CL,60			; second = 60 ?
	BRNE	TIMER1_3
	CLR	CL			; if yes, second = 00
	INC	CH			;   and increment minute
	CPI	CH,60			; minute = 60 ?
	BRNE	TIMER1_3
	CLR	CH			; if yes, minute = 00
	INC	DL			;   and increment hour
	CPI	DL,13			; hour = 13 ?
	BRNE	TIMER1_1
	LDI	DL,1			; if yes, hour = 01
	RJMP	TIMER1_3
TIMER1_1:CPI	DL,12			; HOUR = 12 ?
	BRNE	TIMER1_3
	CPI	DH,'A'			; if yes and AMPM = 'A', AMPM = 'P'
	BRNE	TIMER1_2
	LDI	DH,'P'
	RJMP	TIMER1_3
TIMER1_2:LDI	DH,'A'			; if yes and AMPM = 'P', AMPM = 'A'

TIMER1_3:LDI	LCD_BUFFER,0xC2		; display hour
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,DL
	CALL	LCD_2DIGIT
	LDI	LCD_BUFFER,0xC5		; display minute
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,CH
	CALL	LCD_2DIGIT
	LDI	LCD_BUFFER,0xC8		; display second
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,CL
	CALL	LCD_2DIGIT
	LDI	LCD_BUFFER,0xCB		; display AM/PM
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,DH
	CALL	LCD_DATA

        POP     AL                      ; restore registers
        OUT     SREG,AL
        RETI

;------------------------------------------------
;	Display 2-digit Decimal Number
;------------------------------------------------
LCD_2DIGIT:
	PUSH	AH
	PUSH	BL
	MOV	AL,LCD_BUFFER		; AH:AL / BL
	CLR	AH
	LDI	BL,10
	CALL	DIV8			; AL = quotient, AH = remainder

	MOV	LCD_BUFFER,AL		; display 10^1
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	MOV	LCD_BUFFER,AH		; display 10^0
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	POP	BL
	POP	AH
	RET

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
