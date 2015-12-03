;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	  Exp11_1.asm : D/A Converter Triangular Wave Output	   []
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
;	D/A Output
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"  DAC0800 D/A   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"Triangular Wave ",0,0

LOOP:	LDI	ZL,byte3(TRI_TABLE << 1); start address of triangular table
	OUT	RAMPZ,ZL
	LDI	ZH,high(TRI_TABLE << 1)
	LDI	ZL,low(TRI_TABLE << 1)
	LDI	CL,100
LOOP1:	ELPM	AL,Z+			; read data
	OUT	PORTB,AL		; output D/A
	CALL	D10US
	DEC	CL
	BRNE	LOOP1
	JMP	LOOP

TRI_TABLE:				; triangular wave data table
	.db	0x80,0x84,0x88,0x8C,0x90,0x94,0x98,0x9C,0xA0,0xA4
	.db	0xA8,0xAC,0xB0,0xB4,0xB8,0xBC,0xC0,0xC4,0xC8,0xCC
	.db	0xD0,0xD4,0xD8,0xDC,0xE0,0xE4,0xE0,0xDC,0xD8,0xD4
	.db	0xD0,0xCC,0xC8,0xC4,0xC0,0xBC,0xB8,0xB4,0xB0,0xAC
	.db	0xA8,0xA4,0xA0,0x9C,0x98,0x94,0x90,0x8C,0x88,0x84
	.db	0x80,0x7C,0x78,0x74,0x70,0x6C,0x68,0x64,0x60,0x5C
	.db	0x58,0x54,0x50,0x4C,0x48,0x44,0x40,0x3C,0x38,0x34
	.db	0x30,0x2C,0x28,0x24,0x20,0x1C,0x20,0x24,0x28,0x2C
	.db	0x30,0x34,0x38,0x3C,0x40,0x44,0x48,0x4C,0x50,0x54
	.db	0x58,0x5C,0x60,0x64,0x68,0x6C,0x70,0x74,0x78,0x7C

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
