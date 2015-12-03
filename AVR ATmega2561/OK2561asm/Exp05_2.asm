;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp05_2.asm : Key Input - Debouncing		   []
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
	.db	"Press KEY4-KEY1!",0,0

READ_KEY:CALL	KEY_IN			; input key
	CPI	AL,0b11100000		; KEY1 input ?
	BRNE	CK_KEY2
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  KEY1 is OK !  ",0,0
	JMP	READ_KEY

CK_KEY2:CPI	AL,0b11010000		; KEY2 input ?
	BRNE	CK_KEY3
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  KEY2 is OK !  ",0,0
	JMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; KEY3 input ?
	BRNE	CK_KEY4
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  KEY3 is OK !  ",0,0
	JMP	READ_KEY

CK_KEY4:CPI	AL,0b01110000		; KEY4 input ?
	BRNE	READ_KEY
	LDI	AH,0b11110000
	EOR	AL,AH
	OUT	PORTB,AL
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  KEY4 is OK !  ",0,0
	JMP	READ_KEY

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
