;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]                                                                []
;[]	   Exp20_2.asm : Watchdog Timer - System Reset Mode	   []
;[]                                                                []
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

	LDI	AL,0b00000000		; WDRF = 0
	OUT	MCUSR,AL
	LDI	AL,(1<<WDCE) | (1<<WDE)	; disable Watchdog Timer
	STS	WDTCSR,AL
	LDI	AL,(0<<WDCE) | (0<<WDE)
	STS	WDTCSR,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD

;------------------------------------------------
;	Booting Display
;------------------------------------------------
	CALL	LCD_HOME1		; display booting by reset
	CALL	LCD_STRING
	.db	"  OK-2561 V1.0  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Reset and Boot ",0,0

	CALL	BUZZER_ON		; buzzer on
	LDI	AL,0b00010000		; LED1 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b00100000		; LED2 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b01000000		; LED3 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b10000000		; LED4 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b00000000		; all LED off
	OUT	PORTB,AL
	CALL	BUZZER_OFF		; buzzer off
	CALL	D1SEC

;------------------------------------------------
;	Key Input and Test WDT
;------------------------------------------------
	CALL	DSP_WDT			; display message
	LDI	AL,0b00100000		; LED2 = on
	OUT	PORTB,AL

READ_KEY:CALL	KEY_IN			; input key
	CPI	AL,0b11100000		; if KEY1, blink LED1 by 20 times
	BRNE	CK_KEY4			;    with 0.1sec period
	CALL	LCD_HOME1
	CALL	LCD_STRING
	.db	"   Blink LED1   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"     100 ms     ",0,0
	LDI	CL,20
LED1:	WDR				;  (reset Watchdog Timer)
	IN	AL,PORTB
	LDI	AH,0b00010000
	EOR	AL,AH
	OUT	PORTB,AL
	CALL	D100MS
	DEC	CL
	BRNE	LED1
	CALL	DSP_WDT
	JMP	READ_KEY

CK_KEY4:CPI	AL,0b01110000		; if KEY4, blink LED4 by 10 times
	BRNE	CK_KEY2			;   with 0.4sec period 
	CALL	LCD_HOME1
	CALL	LCD_STRING
	.db	"   Blink LED4   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"     400 ms     ",0,0
	LDI	CL,10
LED4:	WDR				;  (reset Watchdog Timer)
	IN	AL,PORTB
	LDI	AH,0b10000000
	EOR	AL,AH
	OUT	PORTB,AL
	CALL	D200MS
	CALL	D200MS
	DEC	CL
	BRNE	LED4
	CALL	DSP_WDT
	JMP	READ_KEY

CK_KEY2:CPI	AL,0b11010000		; if KEY2, disable Watchdog Timer
	BRNE	CK_KEY3
	LDI	AL,(1<<WDCE) | (1<<WDE)
	STS	WDTCSR,AL
	LDI	AL,(0<<WDCE) | (0<<WDE)
	STS	WDTCSR,AL
	LDI	AL,0b00100000		; LED2 = on
	OUT	PORTB,AL
	JMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; if KEY3, enable Watchdog Timer
	BRNE	NO_KEY
	LDI	AL,(1<<WDCE) | (1<<WDE)
	STS	WDTCSR,AL		;  (system reset mode, 0.25 sec)
	LDI	AL,(1<<WDE) | (1<<WDP2)
	STS	WDTCSR,AL
	LDI	AL,0b01000000		; LED3 = on
	OUT	PORTB,AL
	JMP	READ_KEY

NO_KEY:	WDR				; reset Watchdog Timer
	JMP	READ_KEY

;------------------------------------------------
;	Display Subroutine
;------------------------------------------------
DSP_WDT:CALL	LCD_HOME1
	CALL	LCD_STRING
	.db	"Press KEY4/KEY1!",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"(KEY3/2=ON/OFF) ",0,0
	RET

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
