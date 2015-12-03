;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp06_4.asm : Stop Watch by Timer/Counter1		   []
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
;	Variables for Stop Watch
;------------------------------------------------
;	R7  = run flag(1=run state, 0=stop state)
;	R26 = second low byte
;	R27 = second high byte
;	R28 = minute
;	R29 = hour

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
;	Stop Watch
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" AVR STOP WATCH ",0,0
CLEAR_TIME:
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  00:00:00.00   ",0,0

	CLR	R26			; clear time
	CLR	R27
	CLR	R28
	CLR	R29
	CLR	R7			; clear run flag
	CALL	SET_TIMER1		; set Timer/Counter1 and interrupt

READ_KEY:CALL	KEY_IN			; key input
	CPI	AL,0b11100000		; if KEY1, start
	BRNE	CK_KEY3
	SBRC	R7,0			; if run flag = 1, ignore KEY1
	JMP	READ_KEY
	LDI	AL,0b00010000
	OUT	PORTB,AL
        LDI     AL,0x00
        STS     TCNT1H,AL
        STS     TCNT1L,AL
	SEI
	INC	R7			; run flag = 1
	RJMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; if KEY3, reset
	BRNE	CK_KEY4
	SBRC	R7,0			; if run flag = 1, ignore KEY3
	JMP	READ_KEY
	CLI
	LDI	AL,0b01000000
	OUT	PORTB,AL
	RJMP	CLEAR_TIME

CK_KEY4:CPI	AL,0b01110000		; if KEY4, stop
	BRNE	READ_KEY
	SBRS	R7,0			; if run flag = 0, ignore KEY4
	JMP	READ_KEY
	CLI
	LDI	AL,0b10000000
	OUT	PORTB,AL
	DEC	R7			; run flag = 0
	RJMP	READ_KEY

;====================================================================
;	Subroutine
;====================================================================
;------------------------------------------------
;	Display Time
;------------------------------------------------
DSP_TIME:LDI	LCD_BUFFER,0xC2		; display hour
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,R29
	CALL	LCD_2DIGIT

	LDI	LCD_BUFFER,0xC5		; display minute
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,R28
	CALL	LCD_2DIGIT

	MOV	AH,R27			; second*100/100
	MOV	AL,R26
	LDI	BL,100
	CALL	DIV8			; AL=quotient, AH=remainder

	LDI	LCD_BUFFER,0xC8		; display second
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,AL
	CALL	LCD_2DIGIT
	LDI	LCD_BUFFER,0xCB		; display 1/100 second
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,AH
	CALL	LCD_2DIGIT
	RET

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

;------------------------------------------------
;	Initialize Timer/Counter1 and OC1A Interrupt
;------------------------------------------------
SET_TIMER1:
        LDI     AL,0b00000000           ; CTC mode(4), don't output OC1A
        STS     TCCR1A,AL
        LDI     AL,0b00001100           ; 16MHz/256/(1+624) = 100Hz
        STS     TCCR1B,AL
        LDI     AL,0b00000000
        STS     TCCR1C,AL
        LDI     AL,high(624)
        STS     OCR1AH,AL
        LDI     AL,low(624)
        STS     OCR1AL,AL
        LDI     AL,0x00                 ; clear Timer/Counter1
        STS     TCNT1H,AL
        STS     TCNT1L,AL

        LDI     AL,0b00000010           ; enable Timer/Counter1 OC1A interrupt
        STS     TIMSK1,AL
        LDI     AL,0b00101111           ; clear all interrupt flags
        STS     TIFR1,AL
        RET

;====================================================================
;	Interrupt Service Routine of OC1A
;====================================================================
TIMER1_COMPA:
        PUSH    AL                      ; store registers
        IN      AL,SREG
        PUSH    AL

        ADIW    R26,1                   ; increment 1/100 second
	CPI	R27,high(6000)		; second = 60 ?
	BRNE	TIMER1_RET
	CPI	R26,low(6000)
	BRNE	TIMER1_RET
	CLR	R27			; if yes, second = 0
	CLR	R26
	INC	R28			;    and increment minute
	CPI	R28,60			; minute = 60 ?
	BRNE	TIMER1_RET
	CLR	R28			; if yes, minute = 0
	INC	R29			;    and increment hour
	CPI	R29,100			; hour = 100 ?
	BRNE	TIMER1_RET
	CLR	R29			; if yes, hour = 0

TIMER1_RET:
	CALL	DSP_TIME		; display time
        POP     AL                      ; restore registers
        OUT     SREG,AL
        POP     AL
        RETI

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
