;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]  Exp15_2.asm : Display Different Characters on 7-Segment LEDs  []
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

	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" 7-SEGMENT LED  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"                ",0,0

;------------------------------------------------
;	Display from 0 to F
;------------------------------------------------
LOOP:	CALL	BEEP

	LDI	CL,250			; display for 2 sec
LOOP1:	LDI	CH,8
	LDI	DL,0x00			; display from 0 to 7
	LDI	DH,0b10000000		; digit select data
LOOP2:	MOV	AL,DL
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	MOV	AL,DH
	STS	DIG_SELECT,AL
	CALL	D1MS
	INC	DL
	LSR	DH
	DEC	CH
	BRNE	LOOP2
	DEC	CL
	BRNE	LOOP1

	LDI	CL,250			; display for 2 sec
LOOP3:	LDI	CH,8
	LDI	DL,0x08			; display from 8 to F
	LDI	DH,0b10000000		; digit select data
LOOP4:	MOV	AL,DL
	CALL	BIN2LED
	STS	LCD_DATABUS,AL
	MOV	AL,DH
	STS	DIG_SELECT,AL
	CALL	D1MS
	INC	DL
	LSR	DH
	DEC	CH
	BRNE	LOOP4
	DEC	CL
	BRNE	LOOP3

;------------------------------------------------
;	Display from 0 to F by Left Shift
;------------------------------------------------
	LDI	BL,0x00
LOOP5:	LDI	CL,125			; display for 1 sec
LOOP6:	LDI	CH,8
	MOV	DL,BL			; display from BL value
	LDI	DH,0b10000000		; digit select data
LOOP7:	MOV	AL,DL
	CALL	BIN2LED
	ORI	AL,0b00000001
	STS	LCD_DATABUS,AL
	MOV	AL,DH
	STS	DIG_SELECT,AL
	CALL	D1MS
	INC	DL
	LSR	DH
	DEC	CH
	BRNE	LOOP7
	DEC	CL
	BRNE	LOOP6
	INC	BL
	CPI	BL,0x09
	BRNE	LOOP5

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
