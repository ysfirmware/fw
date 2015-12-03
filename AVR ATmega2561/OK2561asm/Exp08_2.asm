;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]   Exp08_2.asm : Digital Clock on LCD module and 7-Segment LED  []
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

;------------------------------------------------
;	Display Time on 7-Segment LED
;------------------------------------------------
LOOP:	MOV	AL,DL			; display hour
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b10000000
	STS	DIG_SELECT,AL
	CALL	D1MS
	MOV	AL,AH
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b01000000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,'-'			; if no, display "-"
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00100000
	STS	DIG_SELECT,AL
	CALL	D1MS
	MOV	AL,CH			; display minute
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00010000
	STS	DIG_SELECT,AL
	CALL	D1MS
	MOV	AL,AH
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00001000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,'-'			; if no, display "-"
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00000100
	STS	DIG_SELECT,AL
	CALL	D1MS
	MOV	AL,CL			; display second
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00000010
	STS	DIG_SELECT,AL
	CALL	D1MS
	MOV	AL,AH
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00000001
	STS	DIG_SELECT,AL
	CALL	D1MS
	JMP	LOOP

;------------------------------------------------
;	Convert Binary to 7-segment LED Pattern
;------------------------------------------------
; Input  : AL = 8-bit binary(from 0x00 to 0x0F), "H", "-", ".", "'"
; Output : AL = 7-segment LED pattern data

BIN2LED:PUSH	AH
	PUSH	ZH
	PUSH	ZL
	CPI	AL,'H'			; if AL="H", AL=0x10
	BRNE	BIN2L1
	LDI	AL,0x10
	RJMP	BIN2L4
BIN2L1:	CPI	AL,'-'			; if AL="-", AL=0x11
	BRNE	BIN2L2
	LDI	AL,0x11
	RJMP	BIN2L4
BIN2L2:	CPI	AL,'.'			; if AL=".", AL=0x12
	BRNE	BIN2L3
	LDI	AL,0x12
	RJMP	BIN2L4
BIN2L3:	CPI	AL,0x27			; if AL="'", AL=0x13
	BRNE	BIN2L4
	LDI	AL,0x13
BIN2L4:	LDI	AH,byte3(LED_TABLE << 1); RAMPZ:Z = statrt address of LED data
	LDI	ZH,high(LED_TABLE << 1)
	LDI	ZL,low(LED_TABLE << 1)
	ADD	ZL,AL			; RAMPZ:Z = RAMPZ:Z + AL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z			; read LED data from RAMPZ:Z address
	POP	ZL
	POP	ZH
	POP	AH
	RET

LED_TABLE:				; 7-segment LED data table
	.db	0b11111100,0b01100000	; 0,1
	.db	0b11011010,0b11110010	; 2,3
	.db	0b01100110,0b10110110	; 4,5
	.db	0b10111110,0b11100100	; 6,7
	.db	0b11111110,0b11110110	; 8,9
	.db	0b11101110,0b00111110	; A,b
	.db	0b10011100,0b01111010	; C,d
	.db	0b10011110,0b10001110	; E,F
	.db	0b01101110,0b00000010	; H,-
	.db	0b00000001,0b01000000	; .,'

;====================================================================
;	Interrupt Service Routine of OC1A
;====================================================================
TIMER1_COMPA:
	PUSH	AL			; store registers
	PUSH	AH
        IN      AL,SREG
        PUSH    AL

	LDI	AL,0b00000000		; clear 7-segment LED
	STS	DIG_SELECT,AL

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
	POP	AH
	POP	AL
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
