;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp04_2.asm : User-Defined Character Display 	   []
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
	CALL	SET_FONT		; set user font

LOOP:	CALL	BEEP
	CALL	LCD_HOME1		; display logo
	LDI	LCD_BUFFER,0x00
	CALL	LCD_DATA
	CALL	LCD_STRING
	.db	" OK-2561 V1.0 ",0x07,0
	CALL	LCD_HOME2		; display message 1
	CALL	LCD_STRING
	.db	"ATmega2561",0x01,0x02,0x03,0x04,0x05,0x06,0,0
	CALL	D2SEC
	CALL	LCD_HOME2		; display message 2
	CALL	LCD_STRING
	.db	"   2006/10/01   ",0,0
	CALL	D2SEC
	CALL	LCD_HOME2		; display message 3
	CALL	LCD_STRING
	.db	" DUCK-YONG YOON ",0,0
	CALL	D2SEC

	JMP	LOOP

;====================================================================
;	Set User Character Font
;====================================================================
SET_FONT:LDI	LCD_BUFFER,0x40		; set CG RAM address
	CALL	LCD_COMMAND
	LDI	ZL,byte3(FONT << 1)	; start address of font data
	OUT	RAMPZ,ZL
	LDI	ZH,high(FONT << 1)
	LDI	ZL,low(FONT << 1)
	LDI	CL,64
SET_FON1:ELPM	LCD_BUFFER,Z+		; read font data
	CALL	LCD_DATA			; download font data
	DEC	CL
	BRNE	SET_FON1
	RET

FONT:	.db	0b00010000,0b00011000	; character 0
	.db	0b00011100,0b00011110
	.db	0b00011100,0b00011000
	.db	0b00010000,0b00000000
	.db	0b00001111,0b00001001	; character 1
	.db	0b00001001,0b00001001
	.db	0b00001001,0b00001001
	.db	0b00001111,0b00000000
	.db	0b00001000,0b00001000	; character 2
	.db	0b00001000,0b00001110
	.db	0b00001000,0b00001000
	.db	0b00001000,0b00000000
	.db	0b00000001,0b00000010	; character 3
	.db	0b00000100,0b00001000
	.db	0b00000000,0b00000000
	.db	0b00001111,0b00000000
	.db	0b00010000,0b00001000	; character 4
	.db	0b00000100,0b00000010
	.db	0b00000000,0b00000000
	.db	0b00011110,0b00000000
	.db	0b00001111,0b00001000	; character 5
	.db	0b00001000,0b00001111
	.db	0b00001000,0b00001000
	.db	0b00001111,0b00000000
	.db	0b00000100,0b00000100	; character 6
	.db	0b00000100,0b00011100
	.db	0b00000100,0b00000100
	.db	0b00000100,0b00000000
	.db	0b00000001,0b00000011	; character 7
	.db	0b00000111,0b00001111
	.db	0b00000111,0b00000011
	.db	0b00000001,0b00000000

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
