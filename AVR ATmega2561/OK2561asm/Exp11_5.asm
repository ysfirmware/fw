;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	    Exp11_5.asm : Variable D/A Voltage by Key Input	   []
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
;	Key Input and Variable Output
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"   D/A Output   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"0x80 = + 0.0 [V]",0,0

	LDI	DL,0x80			; initial D/A output value
	OUT	PORTB,DL

READ_KEY:CALL	KEY_IN			; input key
	CPI	AL,0b11100000		; if KEY1, decrement by 0x01
	BRNE	CK_KEY2
	DEC	DL
	CALL	DAC_OUT
	JMP	READ_KEY

CK_KEY2:CPI	AL,0b11010000		; if KEY2, increment by 0x01
	BRNE	CK_KEY3
	INC	DL
	CALL	DAC_OUT
	JMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; if KEY3, decrement by 0x10
	BRNE	CK_KEY4
	SUBI	DL,0x10
	CALL	DAC_OUT
	JMP	READ_KEY

CK_KEY4:CPI	AL,0b01110000		; if KEY4, increment by 0x10
	BRNE	READ_KEY
	LDI	AL,0x10
	ADD	DL,AL
	CALL	DAC_OUT
	JMP	READ_KEY

;------------------------------------------------
;	Output D/A and Display Voltage
;------------------------------------------------
DAC_OUT:OUT	PORTB,DL		; output D/A
	LDI	LCD_BUFFER,0xC2		; display in HEX
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,DL
	CALL	LCD_2HEX
	LDI	LCD_BUFFER,0xC7		; display sign of voltage
	CALL	LCD_COMMAND		;   and calculate absolute value
	CPI	DL,0x80
	BRSH	POSITIVE
	LDI	LCD_BUFFER,'-'
	CALL	LCD_DATA
	LDI	AL,0x80
	SUB	AL,DL
	RJMP	VALUE
POSITIVE:LDI	LCD_BUFFER,'+'
	CALL	LCD_DATA
	MOV	AL,DL
	SUBI	AL,0x80
VALUE:	LDI	BL,100			; voltage = (absolute/128)*10
	MUL	AL,BL
	MOV	AH,R1
	MOV	AL,R0
	LDI	BL,128
	CALL	DIV8
	CLR	AH			; display XX.X
	LDI	BL,100
	CALL	DIV8
	ORI	AL,'0'
	CPI	AL,'0'
	BRNE	VALUE1
	LDI	AL,' '
VALUE1:	MOV	LCD_BUFFER,AL
	CALL	LCD_DATA
	MOV	AL,AH
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,'.'
	CALL	LCD_DATA
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	RET

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
