;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp04_1.asm : Basic LCD Display Program		   []
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
;	Display Messages on LCD
;------------------------------------------------
LOOP:	CALL	BEEP
	CALL	LCD_HOME1		; display logo
	CALL	LCD_STRING
	.db	"  OK-2561 V1.0  ",0,0
	CALL	LCD_HOME2		; display message 1
	CALL	LCD_STRING
	.db	" ATmega2561-16AU",0,0
	CALL	D2SEC
	CALL	LCD_HOME2		; display message 2
	CALL	LCD_STRING
	.db	"   2006/10/01   ",0,0
	CALL	D2SEC

;------------------------------------------------
;	Display and Shift on LCD
;------------------------------------------------
	CALL	BEEP
	CALL	LCD_HOME1		; display long line
	CALL	LCD_STRING
	.db	"0123456789ABCDEF0123456789ABCDEF",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"Go shift left and right 16 times",0,0
	CALL	D1SEC

	LDI	CL,16			; shift left
LEFT:	LDI	LCD_BUFFER,0x18
	CALL	LCD_COMMAND
	CALL	D100MS
	CALL	D200MS
	DEC	CL
	BRNE	LEFT
	CALL	D1SEC

	LDI	CL,16			; shift right
RIGHT:	LDI	LCD_BUFFER,0x1C
	CALL	LCD_COMMAND
	CALL	D100MS
	CALL	D200MS
	DEC	CL
	BRNE	RIGHT
	CALL	D1SEC

	JMP	LOOP

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
