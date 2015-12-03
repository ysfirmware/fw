;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	Exp05_4a.asm : Key Input Count - Debouncing(Use Macro)	   []
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

;------------------------------------------------
;	Macro Definition
;------------------------------------------------
.macro	DSP_KEY
	LDI	AH,0b11110000		; display LED
	EOR	AL,AH
	OUT	PORTB,AL
	INC	@0			; display key count
	LDI	AL,100
	CP	@0,AL
	BRNE	COUNT_OK
	CLR	@0
COUNT_OK:LDI	LCD_BUFFER,@1
	CALL	LCD_COMMAND
	MOV	AL,@0
	CALL	LCD_2DEC
.endm

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
	.db	"KEY1=00  KEY2=00",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"KEY3=00  KEY4=00",0,0

	CLR	R1			; initialize key count
	CLR	R2
	CLR	R3
	CLR	R4

READ_KEY:CALL	KEY_IN			; key input with debouncing
	CPI	AL,0b11100000		; KEY1 ?
	BRNE	CK_KEY2
	DSP_KEY	R1,0x85			; display KEY1
	JMP	READ_KEY

CK_KEY2:CPI	AL,0b11010000		; KEY2 ?
	BRNE	CK_KEY3
	DSP_KEY	R2,0x8E			; display KEY2
	JMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; KEY3 ?
	BRNE	CK_KEY4
	DSP_KEY	R3,0xC5			; display KEY3
	JMP	READ_KEY

CK_KEY4:CPI	AL,0b01110000		; KEY4 ?
	BRNE	READ_KEY
	DSP_KEY	R4,0xCE			; display KEY4
	JMP	READ_KEY

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
