;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp19_3.asm : Digital Clock on Graphic LCD		   []
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
	.db	" Digital Clock  ",0,0

;------------------------------------------------
;	Display Digital Clock
;------------------------------------------------
	CALL	GLCD_HOME0		; display title
	CALL	GLCD_STRING
	.db	"2006‘e 10¶© 01·©",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	" µ¡¸å  12:00:00 ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"    (·©¶a·©)    ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"Ñe¸‹¡µ¥  00.0•¡",0,0

LOOP:	LDI	CL,10			; repeat 10 times
LOOP1:	LDI	AL,0			; display year
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	CALL	GLCD_XY
	LDI	LCD_BUFFER,0x20
	CALL	GLCD_2HEX
	LDS	LCD_BUFFER,RTC_YEAR
	CALL	GLCD_2HEX
	LDI	AL,0			; display month
	STS	Xcharacter,AL
	LDI	AL,7
	STS	Ycharacter,AL
	LDS	LCD_BUFFER,RTC_MONTH
	CALL	GLCD_2HEX
	LDI	AL,0			; display date
	STS	Xcharacter,AL
	LDI	AL,12
	STS	Ycharacter,AL
	LDS	LCD_BUFFER,RTC_DATE
	CALL	GLCD_2HEX

	LDI	AL,1			; display AM/PM
	STS	Xcharacter,AL
	LDI	AL,3
	STS	Ycharacter,AL
	LDS	LCD_BUFFER,RTC_HOUR
	PUSH	LCD_BUFFER
	CPI	LCD_BUFFER,0x80
	BRSH	PM
	CALL	GLCD_STRING
	.db	"¸å",0,0
	RJMP	DSP_HOUR
PM:	CALL	GLCD_STRING
	.db	"Ò",0,0
DSP_HOUR:LDI	AL,1			; display hour
	STS	Xcharacter,AL
	LDI	AL,7
	STS	Ycharacter,AL
	POP	LCD_BUFFER
	ANDI	LCD_BUFFER,0b01111111
	CALL	GLCD_2HEX
	LDI	AL,1			; display minute
	STS	Xcharacter,AL
	LDI	AL,10
	STS	Ycharacter,AL
	LDS	LCD_BUFFER,RTC_MINUTE
	CALL	GLCD_2HEX
	LDI	AL,1			; display second
	STS	Xcharacter,AL
	LDI	AL,13
	STS	Ycharacter,AL
	LDS	LCD_BUFFER,RTC_SECOND
	CALL	GLCD_2HEX

	LDI	AL,2			; display weekday
	STS	Xcharacter,AL
	LDI	AL,5
	STS	Ycharacter,AL
	LDS	AL,RTC_WEEKDAY
	CPI	AL,1
	BRNE	CK_MON
	CALL	GLCD_STRING
	.db	"·©",0,0
	RJMP	LOOP2
CK_MON:	CPI	AL,2
	BRNE	CK_TUE
	CALL	GLCD_STRING
	.db	"¶©",0,0
	RJMP	LOOP2
CK_TUE:	CPI	AL,3
	BRNE	CK_WED
	CALL	GLCD_STRING
	.db	"ÑÁ",0,0
	RJMP	LOOP2
CK_WED:	CPI	AL,4
	BRNE	CK_THU
	CALL	GLCD_STRING
	.db	"®",0,0
	RJMP	LOOP2
CK_THU:	CPI	AL,5
	BRNE	CK_FRI
	CALL	GLCD_STRING
	.db	"¡¢",0,0
	RJMP	LOOP2
CK_FRI:	CPI	AL,6
	BRNE	CK_SAT
	CALL	GLCD_STRING
	.db	"‹q",0,0
	RJMP	LOOP2
CK_SAT:	CALL	GLCD_STRING
	.db	"É¡",0,0

LOOP2:	CALL	D100MS			; delay for 100 ms
	DEC	CL
	BREQ	DSP_TEMP
	JMP	LOOP1

;------------------------------------------------
;	Display Temperature
;------------------------------------------------
DSP_TEMP:LDI	AL,0b10000101		; ADC enable, 500 kHz
	STS	ADCSRA,AL
	LDI	AL,0b00000000
	STS	ADCSRB,AL
	LDI	AL,0b11000001		; select ADC1 with 2.56V
	STS	ADMUX,AL		; (single ended)
	CALL	D50US
	CALL	D100US

	LDI	CL,16			; read ADC by 16 times
	CLR	BH			; clear sum
	CLR	BL
LOOP3:	LDI	AL,0b11010101		; start conversion and clear ADIF
	STS	ADCSRA,AL
	NOP
LOOP4:	LDS	AL,ADCSRA		; A/D conversion complete ?
	SBRS	AL,ADIF
	JMP	LOOP4
	LDS	AL,ADCL			; ADCL must be read first, then ADCH
	LDS	AH,ADCH
	ADD	BL,AL			; add A/D result 16 times
	ADC	BH,AH
	CALL	D1MS
	DEC	CL
	BRNE	LOOP3
	LSR	BH			; divide sum by 16
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL

	LDI	AL,3			; display in temperature(XX.X)
	STS	Xcharacter,AL
	LDI	AL,10
	STS	Ycharacter,AL
	CALL	GLCD_XY
	LDI	AH,high(2560)		; Temp = ADC1*256/1024
	LDI	AL,low(2560)
	CALL	MUL16
	LDI	DH,high(1024)
	LDI	DL,low(1024)
	CALL	DIV16
	LDI	BL,100			; display XX.X
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	GLCD_ENGLISH
	MOV	AL,AH
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	GLCD_ENGLISH
	LDI	LCD_BUFFER,'.'
	CALL	GLCD_ENGLISH
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	GLCD_ENGLISH

	JMP	LOOP

;------------------------------------------------
;	Include User Subroutine File
;------------------------------------------------
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
.include "GLCD2561.INC"			; OK-2561 GLCD subroutine file
