;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp05_1.asm : Key Input - No Debouncing		   []
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
;	Key Input and Display
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"   KEY INPUT    ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"PORTF=0b11110000",0,0

READ_KEY:LDI	LCD_BUFFER,0xC8		; cursor position
	CALL	LCD_COMMAND
	IN	AL,PINF			; read key input
	ANDI	AL,0b11110000
	MOV	LCD_BUFFER,AL		; display binary data
	CALL	LCD_8BIN
	LDI	AH,0b11110000		; display LED
	EOR	AL,AH
	OUT	PORTB,AL
	JMP	READ_KEY

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
