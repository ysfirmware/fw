;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	  Exp15_3.asm : Display Temperature on 7-Segment LED	   []
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
;	Display Temperature on Text LCD Module
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"  TEMPERATURE   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" 0000H = 00.0",0xDF,"C ",0,0

	LDI	AL,0b10000101		; ADC enable, 500 kHz
	STS	ADCSRA,AL
	LDI	AL,0b00000000
	STS	ADCSRB,AL
	LDI	AL,0b11000001		; select ADC1 with 2.56V
	STS	ADMUX,AL		; (single ended)
	CALL	D50US
	CALL	D100US

LOOP:	LDI	CL,16			; read ADC by 16 times
	CLR	BH			; clear sum
	CLR	BL
LOOP1:	LDI	AL,0b11010101		; start conversion and clear ADIF
	STS	ADCSRA,AL
	NOP
LOOP2:	LDS	AL,ADCSRA		; A/D conversion complete ?
	SBRS	AL,ADIF
	JMP	LOOP2
	LDS	AL,ADCL			; ADCL must be read first, then ADCH
	LDS	AH,ADCH
	ADD	BL,AL			; add A/D result 16 times
	ADC	BH,AH
	CALL	D20US
	DEC	CL
	BRNE	LOOP1
	LSR	BH			; divide sum by 16
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL

	LDI	LCD_BUFFER,0xC1		; display in HEX
	CALL	LCD_COMMAND
	MOV	AH,BH
	MOV	AL,BL
	CALL	LCD_4HEX
	LDI	LCD_BUFFER,0xC9		; display in temperature(XX.X)
	CALL	LCD_COMMAND
	LDI	AH,high(2560)		; Temp = ADC1*256/1024
	LDI	AL,low(2560)
	CALL	MUL16
	LDI	DH,high(1024)
	LDI	DL,low(1024)
	CALL	DIV16
	LDI	BL,100
	CALL	DIV8
	MOV	R5,AL			; for 7-segment LED
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	MOV	AL,AH
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	MOV	R6,AL			; for 7-segment LED
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,'.'
	CALL	LCD_DATA
	MOV	R7,AH			; for 7-segment LED
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA

;------------------------------------------------
;	Display Temperature on 7-Segment LED
;------------------------------------------------
	LDI	CL,200			; display for 1 sec
LOOP3:	MOV	AL,R5			; ?X.X 'C
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b01000000
	STS	DIG_SELECT,AL
	CALL	D1MS
	MOV	AL,R6			; X?.X 'C
	CALL	BIN2LED
	ORI	AL,0b00000001
	STS	LCD_DATABUS,AL
	LDI	AL,0b00100000
	STS	DIG_SELECT,AL
	CALL	D1MS
	MOV	AL,R7			; XX.? 'C
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00010000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b01000000		; '
	STS	LCD_DATABUS,AL
	LDI	AL,0b00001000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0x0C			; C
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	LDI	AL,0b00000100
	STS	DIG_SELECT,AL
	CALL	D1MS
	DEC	CL
	BRNE	LOOP3

	LDI	AL,0b00000000		; clear 7-segment LED
	STS	DIG_SELECT,AL
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
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
