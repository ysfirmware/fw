;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp04_3.asm : Integer Number Display Program	   []
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

;------------------------------------------------
;	8-Bit Integer Display
;------------------------------------------------
INTEGER:CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" INTEGER = 000  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"= 00H = 00000000",0,0
	CALL	BEEP

	LDI	CL,255			; from 1 to 255
	LDI	BL,1
INTEG1:	LDI	LCD_BUFFER,0x8B		; decimal number
	CALL	LCD_COMMAND
	CLR	AH
	MOV	AL,BL
	CALL	LCD_3DEC
	LDI	LCD_BUFFER,0xC2		; hexdecimal number
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,BL
	CALL	LCD_2HEX
	LDI	LCD_BUFFER,0xC8		; binary number
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,BL
	CALL	LCD_8BIN
	CALL	D1SEC
	INC	BL
	DEC	CL
	BRNE	INTEG1

;------------------------------------------------
;	Multiplication Table Display(1)
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" MULTI TABLE(1) ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"   0 x 0 = 00   ",0,0
	CALL	BEEP

	LDI	BH,2			; BH = from 2 to 9
MULT1:	LDI	BL,1			; BL = from 1 to 9
MULT2:	LDI	LCD_BUFFER,0xC3		; display multiplicand
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,BH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,0xC7		; display multiplier
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,BL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,0xCB		; display multiplication
	CALL	LCD_COMMAND
	MUL	BH,BL
	MOV	AL,R0
	CALL	LCD_2DEC
	CALL	D1SEC
	INC	BL			; BL = BL + 1
	CPI	BL,10			; end(9) ?
	BRNE	MULT2
	INC	BH			; BH = BH + 1
	CPI	BH,10			; end(9) ?
	BRNE	MULT1

;------------------------------------------------
;	Multiplication Table Display(2)
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" MULTI TABLE(2) ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" 00 x 00 = 0000 ",0,0
	CALL	BEEP

	LDI	BH,10			; BH = from 10 to 90
MULT11:	LDI	BL,10			; BL = from 10 to 90
MULT12:	LDI	LCD_BUFFER,0xC1		; display multiplicand
	CALL	LCD_COMMAND
	MOV	AL,BH
	CALL	LCD_2DEC
	LDI	LCD_BUFFER,0xC6		; display multiplier
	CALL	LCD_COMMAND
	MOV	AL,BL
	CALL	LCD_2DEC
	LDI	LCD_BUFFER,0xCB		; display multiplication
	CALL	LCD_COMMAND
	MUL	BH,BL
	MOV	AH,R1
	MOV	AL,R0
	CALL	LCD_4DEC
	CALL	D1SEC
	LDI	AL,10			; BL = BL + 10
	ADD	BL,AL
	CPI	BL,100			; end(90) ?
	BRNE	MULT12
	LDI	AL,10			; BH = BH + 10
	ADD	BH,AL
	CPI	BH,100			; end(90) ?
	BRNE	MULT11

	JMP	INTEGER

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
