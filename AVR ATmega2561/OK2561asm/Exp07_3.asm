;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	Exp07_3.asm : Variable-Frequency, Variable-Duty PWM	   []
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
;	Initialize Phase and Frequency Correct PWM
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"Period =  1250*2",0,0	; vary from 125 to 1250
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"Width  =   125*2",0,0	; vary from 0 to 1250

        LDI     AL,0b00100001           ; Phase and Frequency Correct PWM mode(9)
        STS     TCCR1A,AL		;   and use OC1B
        LDI     AL,0b00010011
        STS     TCCR1B,AL
        LDI     AL,0b00000000
        STS     TCCR1C,AL

        LDI     CH,high(1250)		; set default PWM frequency(period)
        LDI     CL,low(1250)
        STS     OCR1AH,CH		; f(PWM) = 16MHz/64/2/1250 = 100Hz
        STS     OCR1AL,CL

        LDI     DH,high(125)		; set default PWM duty ratio
        LDI     DL,low(125)
        STS     OCR1BH,DH		; D(PWM) = 125/1250 = 10%
        STS     OCR1BL,DL

;------------------------------------------------
;	PWM Control by Key Input
;------------------------------------------------
READ_KEY:CALL	KEY_IN			; key input
	CPI	AL,0b11100000		; if KEY1, decrement duty ratio
	BRNE	CK_KEY2			;   ( DH:DL = DH:DL - 125 )
	CPI	DH,0
	BRNE	CK_KEY1A
	CPI	DL,0
	BREQ	READ_KEY
CK_KEY1A:SUBI	DL,125
	CLR	AL
	SBC	DH,AL
	RJMP	CK_KEY2B

CK_KEY2:CPI	AL,0b11010000		; if KEY2, increment duty ratio
	BRNE	CK_KEY3			;   ( DH:DL = DH:DL + 125 )
	CP	DH,CH
	BRNE	CK_KEY2A
	CP	DL,CL
	BREQ	READ_KEY
CK_KEY2A:LDI	AL,125
	ADD	DL,AL
	CLR	AL
	ADC	DH,AL
CK_KEY2B:STS	OCR1BH,DH		; output duty ratio
        STS     OCR1BL,DL
	LDI	LCD_BUFFER,0xCA		; display duty ratio
	CALL	LCD_COMMAND
	MOV	AH,DH
	MOV	AL,DL
	CALL	LCD_4DEC
	RJMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; if KEY3, decrement period
	BRNE	CK_KEY4			;   ( CH:CL = CH:CL - 125 )
	CPI	CH,0
	BRNE	CK_KEY3A
	CPI	CL,125
	BREQ	READ_KEY
CK_KEY3A:CP	CH,DH
	BRNE	CK_KEY3B
	CP	CL,DL
	BREQ	READ_KEY
CK_KEY3B:SUBI	CL,125
	CLR	AL
	SBC	CH,AL
	RJMP	CK_KEY4B

CK_KEY4:CPI	AL,0b01110000		; if KEY4, increment period
	BRNE	READ_KEY		;   ( CH:CL = CH:CL + 125 )
	CPI	CH,high(1250)
	BRNE	CK_KEY4A
	CPI	CL,low(1250)
	BREQ	READ_KEY
CK_KEY4A:LDI	AL,125
	ADD	CL,AL
	CLR	AL
	ADC	CH,AL
CK_KEY4B:STS	OCR1AH,CH		; output frequency
        STS     OCR1AL,CL
	LDI	LCD_BUFFER,0x8A		; display frequency
	CALL	LCD_COMMAND
	MOV	AH,CH
	MOV	AL,CL
	CALL	LCD_4DEC
	RJMP	READ_KEY

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
