;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp16_1.asm : Digital Clock on Text LCD Module	   []
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

.def	CURSOR	= R8			; cursor position

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

	CALL	LCD_HOME1		; display default clock
	CALL	LCD_STRING
	.db	" 2006/10/01 SUN ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  12:00:00 AM   ",0,0

	LDI	LCD_BUFFER,0x0F		; cursor ON, cursor blink
	CALL	LCD_COMMAND

;------------------------------------------------
;	Check If RTC Data are available
;------------------------------------------------
	CALL	D200MS
	LDS	AL,RTC_FLAG		; RTC_FLAG = "G"(Good) ?
	CPI	AL,'G'
	BREQ	RTC_OK
	LDI	AL,'G'			; if not, initialize RTC
	STS	RTC_FLAG,AL
	LDI	AL,0x06			; initialize year(2006)
	STS	RTC_YEAR,AL
	LDI	AL,0x10			; initialize month(10)
	STS	RTC_MONTH,AL
	LDI	AL,0x01			; initialize date(01)
	STS	RTC_DATE,AL
	LDI	AL,0x01			; initialize week day(SUN)
	STS	RTC_WEEKDAY,AL
	LDI	AL,0x12			; initialize hour(12, AM)
	STS	RTC_HOUR,AL
	LDI	AL,0x00			; initialize minute(00)
	STS	RTC_MINUTE,AL
	LDI	AL,0x00			; initialize second(00)
	STS	RTC_SECOND,AL

	LDI	AL,0b00101111		; initialize Register A
	STS	RTC_A,AL		;  (oscillator on, SQW = 2 Hz)
	LDI	AL,0b00000000		; initialize Register B (12 hour, BCD)
	STS	RTC_B,AL		;  (interrupt disable, SQW disable)

RTC_OK:	JMP	ADJ_HOME

;------------------------------------------------
;	Adjust Year
;------------------------------------------------
ADJ_YEAR:LDI	AL,0x84			; set cursor position
	MOV	CURSOR,AL
ADJ_YEA1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_YEA2
	JMP	ADJ_HOME
ADJ_YEA2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_YEA3
	JMP	ADJ_MONTH
ADJ_YEA3:CPI	AL,0b11010000		; if KEY2, increment
	BRNE	ADJ_YEA4
	LDS	AL,RTC_YEAR
	CALL	INC_YEAR
	STS	RTC_YEAR,AL
	JMP	ADJ_YEA1
ADJ_YEA4:CPI	AL,0b11100000		; if KEY1, decrement
	BRNE	ADJ_YEA1
	LDS	AL,RTC_YEAR
	CALL	DEC_YEAR
	STS	RTC_YEAR,AL
	JMP	ADJ_YEA1

INC_YEAR:CPI	AL,0x99			; if year = 99, year = 00
	BRNE	INC_YEA1
	LDI	AL,0x00
	RET
INC_YEA1:CALL	BCD_INC			; if not, increment year
	RET

DEC_YEAR:CPI	AL,0x00			; if year = 00, year = 99
	BRNE	DEC_YEA1
	LDI	AL,0x99
	RET
DEC_YEA1:CALL	BCD_DEC			; if not, decrement year
	RET

;------------------------------------------------
;	Adjust Month
;------------------------------------------------
ADJ_MONTH:LDI	AL,0x87			; set cursor position
	MOV	CURSOR,AL
ADJ_MON1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_MON2
	JMP	ADJ_YEAR
ADJ_MON2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_MON3
	JMP	ADJ_DATE
ADJ_MON3:CPI	AL,0b11010000		; if KEY2, increment
	BRNE	ADJ_MON4
	LDS	AL,RTC_MONTH
	CALL	INC_MONTH
	STS	RTC_MONTH,AL
	JMP	ADJ_MON1
ADJ_MON4:CPI	AL,0b11100000		; if KEY1, decrement
	BRNE	ADJ_MON1
	LDS	AL,RTC_MONTH
	CALL	DEC_MONTH
	STS	RTC_MONTH,AL
	JMP	ADJ_MON1

INC_MONTH:CPI	AL,0x12			; if month = 12, month = 01
	BRNE	INC_MON1
	LDI	AL,0x01
	RET
INC_MON1:CALL	BCD_INC			; if not, increment month
	RET

DEC_MONTH:CPI	AL,0x01			; if month = 01, month = 12
	BRNE	DEC_MON1
	LDI	AL,0x12
	RET
DEC_MON1:CALL	BCD_DEC			; if not, decrement month
	RET

;------------------------------------------------
;	Adjust Date
;------------------------------------------------
ADJ_DATE:LDI	AL,0x8A			; set cursor position
	MOV	CURSOR,AL
ADJ_DAT1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_DAT2
	JMP	ADJ_MONTH
ADJ_DAT2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_DAT3
	JMP	ADJ_WEEK
ADJ_DAT3:CPI	AL,0b11010000		; if KEY2, increment
	BRNE	ADJ_DAT4
	LDS	AL,RTC_DATE
	CALL	INC_DATE
	STS	RTC_DATE,AL
	JMP	ADJ_DAT1
ADJ_DAT4:CPI	AL,0b11100000		; if KEY1, decrement
	BRNE	ADJ_DAT1
	LDS	AL,RTC_DATE
	CALL	DEC_DATE
	STS	RTC_DATE,AL
	JMP	ADJ_DAT1

INC_DATE:CPI	AL,0x31			; if date = 31, date = 01
	BRNE	INC_DATE1
	LDI	AL,0x01
	RET
INC_DATE1:CALL	BCD_INC			; if not, increment date
	RET

DEC_DATE:CPI	AL,0x01			; if date = 01, date = 31
	BRNE	DEC_DATE1
	LDI	AL,0x31
	RET
DEC_DATE1:CALL	BCD_DEC			; if not, decrement date
	RET

;------------------------------------------------
;	Adjust Weekday
;------------------------------------------------
ADJ_WEEK:LDI	AL,0x8E			; set cursor position
	MOV	CURSOR,AL
ADJ_WEE1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_WEE2
	JMP	ADJ_DATE
ADJ_WEE2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_WEE3
	JMP	ADJ_HOUR
ADJ_WEE3:CPI	AL,0b11010000		; if KEY2, increment
	BRNE	ADJ_WEE4
	LDS	AL,RTC_WEEKDAY
	INC	AL
	CPI	AL,8
	BRNE	ADJ_WEE3A
	LDI	AL,1
ADJ_WEE3A:STS	RTC_WEEKDAY,AL
	JMP	ADJ_WEE1
ADJ_WEE4:CPI	AL,0b11100000		; if KEY1, decrement
	BRNE	ADJ_WEE1
	LDS	AL,RTC_WEEKDAY
	DEC	AL
	CPI	AL,0
	BRNE	ADJ_WEE4A
	LDI	AL,7
ADJ_WEE4A:STS	RTC_WEEKDAY,AL
	JMP	ADJ_WEE1

;------------------------------------------------
;	Adjust Hour
;------------------------------------------------
ADJ_HOUR:LDI	AL,0xC3			; set cursor position
	MOV	CURSOR,AL
ADJ_HOU1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_HOU2
	JMP	ADJ_WEEK
ADJ_HOU2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_HOU3
	JMP	ADJ_MINUTE
ADJ_HOU3:CPI	AL,0b11010000		; if KEY2, increment
	BRNE	ADJ_HOU4
	LDS	AL,RTC_HOUR
	CALL	INC_HOUR
	STS	RTC_HOUR,AL
	JMP	ADJ_HOU1
ADJ_HOU4:CPI	AL,0b11100000		; if KEY1, decrement
	BRNE	ADJ_HOU1
	LDS	AL,RTC_HOUR
	CALL	DEC_HOUR
	STS	RTC_HOUR,AL
	JMP	ADJ_HOU1

INC_HOUR:CPI	AL,0x12			; if hour = 12, hour = 01
	BRNE	INC_HOUR1
	LDI	AL,0x01
	RET
INC_HOUR1:CPI	AL,0x92			; if hour = 92, hour = 81
	BRNE	INC_HOUR2
	LDI	AL,0x81
	RET
INC_HOUR2:CALL	BCD_INC			; if not, increment hour
	RET

DEC_HOUR:CPI	AL,0x01			; if hour = 01, hour = 12
	BRNE	DEC_HOUR1
	LDI	AL,0x12
	RET
DEC_HOUR1:CPI	AL,0x81			; if hour = 81, hour = 92
	BRNE	DEC_HOUR2
	LDI	AL,0x92
	RET
DEC_HOUR2:CALL	BCD_DEC			; if not, decrement hour
	RET

;------------------------------------------------
;	Adjust Minute
;------------------------------------------------
ADJ_MINUTE:LDI	AL,0xC6			; set cursor position
	MOV	CURSOR,AL
ADJ_MIN1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_MIN2
	JMP	ADJ_HOUR
ADJ_MIN2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_MIN3
	JMP	ADJ_SECOND
ADJ_MIN3:CPI	AL,0b11010000		; if KEY2, increment
	BRNE	ADJ_MIN4
	LDS	AL,RTC_MINUTE
	CALL	INC_MIN
	STS	RTC_MINUTE,AL
	JMP	ADJ_MIN1
ADJ_MIN4:CPI	AL,0b11100000		; if KEY1, decrement
	BRNE	ADJ_MIN1
	LDS	AL,RTC_MINUTE
	CALL	DEC_MIN
	STS	RTC_MINUTE,AL
	JMP	ADJ_MIN1

INC_MIN:CPI	AL,0x59			; if minute = 59, minute = 00
	BRNE	INC_MIN1
	LDI	AL,0x00
	RET
INC_MIN1:CALL	BCD_INC			; if not, increment minute
	RET

DEC_MIN:CPI	AL,0x00			; if minute = 00, minute = 59
	BRNE	DEC_MIN1
	LDI	AL,0x59
	RET
DEC_MIN1:CALL	BCD_DEC			; if not, decrement minute
	RET

;------------------------------------------------
;	Adjust Second
;------------------------------------------------
ADJ_SECOND:LDI	AL,0xC9			; set cursor position
	MOV	CURSOR,AL
ADJ_SEC1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_SEC2
	JMP	ADJ_MINUTE
ADJ_SEC2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_SEC3
	JMP	ADJ_AMPM
ADJ_SEC3:CPI	AL,0b11010000		; if KEY2, increment
	BRNE	ADJ_SEC4
	LDS	AL,RTC_SECOND
	CALL	INC_SEC
	STS	RTC_SECOND,AL
	JMP	ADJ_SEC1
ADJ_SEC4:CPI	AL,0b11100000		; if KEY1, decrement
	BRNE	ADJ_SEC1
	LDS	AL,RTC_SECOND
	CALL	DEC_SEC
	STS	RTC_SECOND,AL
	JMP	ADJ_SEC1

INC_SEC:CPI	AL,0x59			; if second = 59, second = 00
	BRNE	INC_SEC1
	LDI	AL,0x00
	RET
INC_SEC1:CALL	BCD_INC			; if not, increment second
	RET

DEC_SEC:CPI	AL,0x00			; if second = 00, second = 59
	BRNE	DEC_SEC1
	LDI	AL,0x59
	RET
DEC_SEC1:CALL	BCD_DEC			; if not, decrement second
	RET

;------------------------------------------------
;	Adjust AM/PM
;------------------------------------------------
ADJ_AMPM:LDI	AL,0xCC			; set cursor position
	MOV	CURSOR,AL
ADJ_AP1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_AP2
	JMP	ADJ_SECOND
ADJ_AP2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_AP3
	JMP	ADJ_HOME
ADJ_AP3:CPI	AL,0b11010000		; if KEY1 or KEY2, toggle
	BRNE	ADJ_AP4
	JMP	ADJ_AP5
ADJ_AP4:CPI	AL,0b11100000
	BRNE	ADJ_AP1
ADJ_AP5:LDS	AL,RTC_HOUR
	SBRS	AL,7
	JMP	ADJ_AP6
	ANDI	AL,0b01111111		; if PM, PM -> AM
	STS	RTC_HOUR,AL
	JMP	ADJ_AP1
ADJ_AP6:ORI	AL,0b10000000		; if AM, AM -> PM
	STS	RTC_HOUR,AL
	JMP	ADJ_AP1

;------------------------------------------------
;	Adjust Home Position
;------------------------------------------------
ADJ_HOME:LDI	AL,0xCF			; set cursor position
	MOV	CURSOR,AL
ADJ_HOM1:CALL	DSP_RTC
	CALL	KEY_IN			; input key KEY1-4
	CPI	AL,0b01110000		; if KEY4, go left
	BRNE	ADJ_HOM2
	JMP	ADJ_AMPM
ADJ_HOM2:CPI	AL,0b10110000		; if KEY3, go right
	BRNE	ADJ_HOM1
	JMP	ADJ_YEAR

;------------------------------------------------
;	BCD Increment(AL)
;------------------------------------------------
BCD_INC:PUSH	R0			; store registers
	PUSH	R1
	PUSH	AH
	PUSH	BL

	PUSH	AL			; convert BCD to binary
	LSR	AL
	LSR	AL
	LSR	AL
	LSR	AL
	LDI	AH,10
	MUL	AL,AH
	POP	AL
	ANDI	AL,0b00001111
	ADD	AL,R0
	INC	AL			; increment
	CLR	AH			; convert binary to BCD
	LDI	BL,10
	CALL	DIV8
	LSL	AL
	LSL	AL
	LSL	AL
	LSL	AL
	ADD	AL,AH

	POP	BL			; restore registers
	POP	AH
	POP	R1
	POP	R0
	RET

;------------------------------------------------
;	BCD Decrement(AL)
;------------------------------------------------
BCD_DEC:PUSH	R0			; store registers
	PUSH	R1
	PUSH	AH
	PUSH	BL

	PUSH	AL			; convert BCD to binary
	LSR	AL
	LSR	AL
	LSR	AL
	LSR	AL
	LDI	AH,10
	MUL	AL,AH
	POP	AL
	ANDI	AL,0b00001111
	ADD	AL,R0
	DEC	AL			; decrement
	CLR	AH			; convert binary to BCD
	LDI	BL,10
	CALL	DIV8
	LSL	AL
	LSL	AL
	LSL	AL
	LSL	AL
	ADD	AL,AH

	POP	BL			; restore registers
	POP	AH
	POP	R1
	POP	R0
	RET

;====================================================================
;	RTC Subroutine
;====================================================================
;------------------------------------------------
;	Display Clock from RTC
;------------------------------------------------
DSP_RTC:LDI	LCD_BUFFER,0x0C		; cursor OFF
	CALL	LCD_COMMAND

	LDS	BH,RTC_MINUTE		; AH = minute
	LDS	BL,RTC_SECOND		; AL = second

	CALL	DSP_YEAR		; display year/month/date
	CALL	DSP_MONTH
	CALL	DSP_DATE
	CALL	DSP_WEEK
	CALL	DSP_HOUR		; display hour/minute/second
	CALL	DSP_MINUTE
	CALL	DSP_SECOND

	MOV	LCD_BUFFER,CURSOR	; go back cursor position
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,0x0F		; cursor ON, cursor blink
	CALL	LCD_COMMAND
	CALL	D1MS			; time delay for cursor

	OR	BL,BH			; minute = 00 and second = 00 ?
	CPI	BL,0x00
	BREQ	DSP_RTC1
	RET

DSP_RTC1:CALL	BEEP			; beep for each 00 second
	CALL	D1SEC
	RET

;------------------------------------------------
;	Display Year on LCD
;------------------------------------------------
DSP_YEAR:LDI	LCD_BUFFER,0x81		; set LCD cursor position
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,0x20		; display 20XX on LCD
	CALL	LCD_2HEX
	LDS	LCD_BUFFER,RTC_YEAR	; display year from RTC
	CALL	LCD_2HEX
	RET

;------------------------------------------------
;	Display Month on LCD
;------------------------------------------------
DSP_MONTH:LDI	LCD_BUFFER,0x86		; set LCD cursor position
	CALL	LCD_COMMAND
	LDS	LCD_BUFFER,RTC_MONTH	; display month from RTC
	CALL	LCD_2HEX
	RET

;------------------------------------------------
;	Display Date on LCD
;------------------------------------------------
DSP_DATE:LDI	LCD_BUFFER,0x89		; set LCD cursor position
	CALL	LCD_COMMAND
	LDS	LCD_BUFFER,RTC_DATE	; display date from RTC
	CALL	LCD_2HEX
	RET

;------------------------------------------------
;	Display Weekday on LCD
;------------------------------------------------
DSP_WEEK:LDI	LCD_BUFFER,0x8C		; set LCD cursor position
	CALL	LCD_COMMAND
	LDS	LCD_BUFFER,RTC_WEEKDAY	; display weekday from RTC
	CPI	LCD_BUFFER,1
	BRNE	DSP_WEEK1
	CALL	LCD_STRING
	.db	"SUN ",0,0
	RET
DSP_WEEK1:CPI	LCD_BUFFER,2
	BRNE	DSP_WEEK2
	CALL	LCD_STRING
	.db	"MON ",0,0
	RET
DSP_WEEK2:CPI	LCD_BUFFER,3
	BRNE	DSP_WEEK3
	CALL	LCD_STRING
	.db	"TUE ",0,0
	RET
DSP_WEEK3:CPI	LCD_BUFFER,4
	BRNE	DSP_WEEK4
	CALL	LCD_STRING
	.db	"WED ",0,0
	RET
DSP_WEEK4:CPI	LCD_BUFFER,5
	BRNE	DSP_WEEK5
	CALL	LCD_STRING
	.db	"THU ",0,0
	RET
DSP_WEEK5:CPI	LCD_BUFFER,6
	BRNE	DSP_WEEK6
	CALL	LCD_STRING
	.db	"FRI ",0,0
	RET
DSP_WEEK6:CALL	LCD_STRING
	.db	"SAT ",0,0
	RET

;------------------------------------------------
;	Display Hour on LCD
;------------------------------------------------
DSP_HOUR:LDI	LCD_BUFFER,0xC2		; set LCD cursor position
	CALL	LCD_COMMAND
	LDS	LCD_BUFFER,RTC_HOUR	; display hour from RTC
	ANDI	LCD_BUFFER,0b01111111
	CALL	LCD_2HEX
	LDI	LCD_BUFFER,0xCB		; set LCD cursor position
	CALL	LCD_COMMAND
	LDS	LCD_BUFFER,RTC_HOUR	; read hour from RTC
	CPI	LCD_BUFFER,0x80		; AM or PM ?
	BRSH	DSP_HOUR1
	LDI	LCD_BUFFER,'A'
	JMP	DSP_HOUR2
DSP_HOUR1:LDI	LCD_BUFFER,'P'
DSP_HOUR2:CALL	LCD_DATA		; display 'A' or 'B' on LCD
	RET

;------------------------------------------------
;	Display Minute on LCD
;------------------------------------------------
DSP_MINUTE:LDI	LCD_BUFFER,0xC5		; set LCD cursor position
	CALL	LCD_COMMAND
	LDS	LCD_BUFFER,RTC_MINUTE	; display minute from RTC
	CALL	LCD_2HEX
	RET

;------------------------------------------------
;	Display Second on LCD
;------------------------------------------------
DSP_SECOND:LDI	LCD_BUFFER,0xC8		; set LCD cursor position
	CALL	LCD_COMMAND
	LDS	LCD_BUFFER,RTC_SECOND	; display second from RTC
	CALL	LCD_2HEX
	RET

;------------------------------------------------
;	Include User Subroutine File
;------------------------------------------------
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
