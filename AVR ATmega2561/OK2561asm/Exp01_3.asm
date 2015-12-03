;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp01_3.asm : Integer Number Display Program	   []
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
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"Display(1) = 000",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"Display(2) = 000",0,0

LOOP:	CALL	BEEP
	LDI	AL,1			; from 1 to 255
	MOV	R7,AL
LOOP1:	LDI	LCD_BUFFER,0x8D		; Display(1)
	CALL	LCD_COMMAND
	CLR	AH
	MOV	AL,R7
	CALL	LCD_3DEC
	LDI	LCD_BUFFER,0xCD		; Display(2)
	CALL	LCD_COMMAND
	CLR	AH
	MOV	AL,R7
	LDI	BL,100
	CALL	DIV8			; (division operation AH:AL/BL)
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	MOV	AL,AH
	CLR	AH
	LDI	BL,10
	CALL	DIV8			; (division operation AH:AL/BL)
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	CALL	D1SEC
	INC	R7			; increment loop counter
	BRNE	LOOP1			; continue if not end

	JMP	LOOP			; start again

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
