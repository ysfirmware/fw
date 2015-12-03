;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp19_2.asm : Integer Number Display Program	   []
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

	CALL	GLCD_CLEAR		; clear graphic LCD screen
	CLR	AL			; cursor off
	STS	CURSOR_FLAG,AL

;------------------------------------------------
;	Display Title on Text LCD Module
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"Graphic LCD Test",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Display Number ",0,0

;------------------------------------------------
;	8-Bit Integer Display
;------------------------------------------------
INTEGER:CALL	BEEP
	CALL	GLCD_HOME0		; display title
	CALL	GLCD_STRING
	.db	" ** ®•¸aÎa¯¡ ** ",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"16»¥®= 0x00    ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"10»¥®= 000     ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	" 2»¥®= 00000000",0,0

	LDI	CL,255			; from 1 to 255
	LDI	BL,1
INTEG1:	LDI	AL,1			; hexadecimal number
	STS	Xcharacter,AL
	LDI	AL,10
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MOV	LCD_BUFFER,BL
	CALL	GLCD_2HEX
	LDI	AL,2			; decimal number
	STS	Xcharacter,AL
	LDI	AL,8
	STS	Ycharacter,AL
	CALL	GLCD_XY
	CLR	AH
	MOV	AL,BL
	CALL	GLCD_3DEC
	LDI	AL,3			; binary number
	STS	Xcharacter,AL
	LDI	AL,8
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MOV	LCD_BUFFER,BL
	CALL	GLCD_8BIN
	CALL	D500MS
	INC	BL
	DEC	CL
	BRNE	INTEG1
	CALL	D1SEC

;------------------------------------------------
;	Multiplication Table Display(1)
;------------------------------------------------
	CALL	BEEP
	CALL	GLCD_HOME0		; display title
	CALL	GLCD_STRING
	.db	" **  ŠŠ”e  ** ",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"                ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"   0 x 0 = 00   ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"                ",0,0

	LDI	BH,2			; BH = from 2 to 9
MULT1:	LDI	BL,1			; BL = from 1 to 9
MULT2:	LDI	AL,2			; display multiplicand
	STS	Xcharacter,AL
	LDI	AL,3
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MOV	LCD_BUFFER,BH
	ORI	LCD_BUFFER,'0'
	CALL	GLCD_ENGLISH
	LDI	AL,2			; display multiplier
	STS	Xcharacter,AL
	LDI	AL,7
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MOV	LCD_BUFFER,BL
	ORI	LCD_BUFFER,'0'
	CALL	GLCD_ENGLISH
	LDI	AL,2			; display multiplication
	STS	Xcharacter,AL
	LDI	AL,11
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MUL	BH,BL
	MOV	AL,R0
	CALL	GLCD_2DEC
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
	CALL	BEEP
	CALL	GLCD_HOME0		; display title
	CALL	GLCD_STRING
	.db	"** 2¸aŸ¡ ‰³­Q **",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"                ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	" 00 x 00 = 0000 ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"                ",0,0

	LDI	BH,10			; BH = from 10 to 90
MULT11:	LDI	BL,10			; BL = from 10 to 90
MULT12:	LDI	AL,2			; display multiplicand
	STS	Xcharacter,AL
	LDI	AL,1
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MOV	AL,BH
	CALL	GLCD_2DEC
	LDI	AL,2			; display multiplier
	STS	Xcharacter,AL
	LDI	AL,6
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MOV	AL,BL
	CALL	GLCD_2DEC
	LDI	AL,2			; display multiplication
	STS	Xcharacter,AL
	LDI	AL,11
	STS	Ycharacter,AL
	CALL	GLCD_XY
	MUL	BH,BL
	MOV	AH,R1
	MOV	AL,R0
	CALL	GLCD_4DEC
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
.include "GLCD2561.INC"			; OK-2561 GLCD subroutine file
