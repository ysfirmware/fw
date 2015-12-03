;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp03_2.asm : Software Time Delay Application	   []
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
	.db	" S/W Time Delay ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"Press KEY4-KEY1!",0,0

	LDI	CH,1			; initial speed mode
	LDI	CL,0b01010000		; initial LED data

READ_KEY:CALL	KEY_IN			; input key
	CPI	AL,0b11100000		; if KEY1, speed mode 1
	BRNE	CK_KEY2
	LDI	CH,1
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"     100[ms]    ",0,0
	JMP	LED_DISPLAY

CK_KEY2:CPI	AL,0b11010000		; if KEY2, speed mode 2
	BRNE	CK_KEY3
	LDI	CH,2
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"     200[ms]    ",0,0
	JMP	LED_DISPLAY

CK_KEY3:CPI	AL,0b10110000		; if KEY3, speed mode 3
	BRNE	CK_KEY4
	LDI	CH,3
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"     300[ms]    ",0,0
	JMP	LED_DISPLAY

CK_KEY4:CPI	AL,0b01110000		; if KEY4, speed mode 4
	BRNE	LED_DISPLAY
	LDI	CH,4
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"     400[ms]    ",0,0

LED_DISPLAY:
	OUT	PORTB,CL		; display LED
	LDI	AL,0b11110000		; complement LED data
	EOR	CL,AL
	CPI	CH,1			; time delay
	BRNE	LED_D1
	CALL	D100MS
	JMP	READ_KEY
LED_D1:	CPI	CH,2
	BRNE	LED_D2
	CALL	D200MS
	JMP	READ_KEY
LED_D2:	CPI	CH,3
	BRNE	LED_D3
	CALL	D100MS
	CALL	D200MS
	JMP	READ_KEY
LED_D3:	CALL	D200MS
	CALL	D200MS
	JMP	READ_KEY

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
