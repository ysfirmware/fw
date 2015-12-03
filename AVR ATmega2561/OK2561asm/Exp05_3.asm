;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp05_3.asm : Key Input Count - No Debouncing	   []
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
	.db	"KEY1=00  KEY2=00",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"KEY3=00  KEY4=00",0,0

	CLR	R1			; initialize key count
	CLR	R2
	CLR	R3
	CLR	R4

READ_KEY:IN	AL,PINF			; key input without debouncing
	ANDI	AL,0b11110000
	CPI	AL,0b11100000		; KEY1 ?
	BRNE	CK_KEY2
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	INC	R1			; display KEY1 count
	LDI	AL,100
	CP	R1,AL
	BRNE	CK_KEY11
	CLR	R1
CK_KEY11:LDI	LCD_BUFFER,0x85
	CALL	LCD_COMMAND
	MOV	AL,R1
	CALL	LCD_2DEC
	JMP	READ_KEY

CK_KEY2:CPI	AL,0b11010000		; KEY2 ?
	BRNE	CK_KEY3
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	INC	R2			; display KEY2 count
	LDI	AL,100
	CP	R2,AL
	BRNE	CK_KEY21
	CLR	R2
CK_KEY21:LDI	LCD_BUFFER,0x8E
	CALL	LCD_COMMAND
	MOV	AL,R2
	CALL	LCD_2DEC
	JMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; KEY3 ?
	BRNE	CK_KEY4
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	INC	R3			; display KEY3 count
	LDI	AL,100
	CP	R3,AL
	BRNE	CK_KEY31
	CLR	R3
CK_KEY31:LDI	LCD_BUFFER,0xC5
	CALL	LCD_COMMAND
	MOV	AL,R3
	CALL	LCD_2DEC
	JMP	READ_KEY

CK_KEY4:CPI	AL,0b01110000		; KEY4 ?
	BRNE	READ_KEY
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	INC	R4			; display KEY4 count
	LDI	AL,100
	CP	R4,AL
	BRNE	CK_KEY41
	CLR	R4
CK_KEY41:LDI	LCD_BUFFER,0xCE
	CALL	LCD_COMMAND
	MOV	AL,R4
	CALL	LCD_2DEC
	JMP	READ_KEY

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
