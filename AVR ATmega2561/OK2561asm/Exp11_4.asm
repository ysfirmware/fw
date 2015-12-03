;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	 Exp11_4.asm : Variable D/A Frequency by Key Input	   []
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
        JMP     RESET                   ; power-on reset entry point

        .org    0x0022
        JMP     TIMER1_COMPA		; interrupt vector OC1A

RESET:	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD
	CALL	BEEP

;------------------------------------------------
;	Initialize OC1A Interrupt
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"   D/A Output   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"16MHz/800/100 Hz",0,0

        LDI     AL,0b00000000           ; CTC mode(4), don't output OC1A
        STS     TCCR1A,AL
        LDI     AL,0b00001010           ; interrupt = 16MHz/8/(1+99) = 20kHz
        STS     TCCR1B,AL
        LDI     AL,0b00000000
        STS     TCCR1C,AL
        LDI     AL,high(99)
        STS     OCR1AH,AL
        LDI     AL,low(99)
        STS     OCR1AL,AL
        LDI     AL,0x00                 ; clear Timer/Counter1
        STS     TCNT1H,AL
        STS     TCNT1L,AL
        LDI     AL,0b00000010           ; enable Timer/Counter1 OC1A interrupt
        STS     TIMSK1,AL
        LDI     AL,0b00101111           ; clear all interrupt flags
        STS     TIFR1,AL
        SEI                             ; global interrupt enable

;------------------------------------------------
;	Key Input and Variable Output
;------------------------------------------------
	LDI	DL,0			; offset of data address

	LDI	BL,100			; initial division ratio
READ_KEY:CALL	KEY_IN			; input key
	CPI	AL,0b11100000		; if KEY1, decrement by 1
	BRNE	CK_KEY2
	DEC	BL
	JMP	SET_FREQ

CK_KEY2:CPI	AL,0b11010000		; if KEY2, increment by 1
	BRNE	CK_KEY3
	LDI	AH,0b11110000
	INC	BL
	JMP	SET_FREQ

CK_KEY3:CPI	AL,0b10110000		; if KEY3, decrement by 10
	BRNE	CK_KEY4
	SUBI	BL,10
	JMP	SET_FREQ

CK_KEY4:CPI	AL,0b01110000		; if KEY4, increment by 10
	BRNE	READ_KEY
	LDI	AL,10
	ADD	BL,AL

SET_FREQ:CPI	BL,20			; lower limit = 20
	BRSH	SET_FR1
	LDI	BL,20
	RJMP	SET_FR2
SET_FR1:CPI	BL,200			; upper limit = 200
	BRLO	SET_FR2
	LDI	BL,200
SET_FR2:CLR	AL			; modify OCR1A
        STS     OCR1AH,AL
	MOV	AL,BL
	DEC	AL
        STS     OCR1AL,AL
	LDI	LCD_BUFFER,0xCA		; display frequency
	CALL	LCD_COMMAND
	CLR	AH
	MOV	AL,BL
	CALL	LCD_3DEC
	JMP	READ_KEY

;====================================================================
;	Interrupt Service Routine of OC1A
;====================================================================
TIMER1_COMPA:
        PUSH    AH                      ; store registers
	PUSH	AL
        IN      AL,SREG
        PUSH    AL

	LDI	AH,byte3(SIN_TABLE << 1); calculate data address 
	LDI	ZH,high(SIN_TABLE << 1)
	LDI	ZL,low(SIN_TABLE << 1)
	ADD	ZL,DL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z+			; read data
	OUT	PORTB,AL		; output D/A

	INC	DL			; calculate next offset
	CPI	DL,100
	BRNE	TIMER1_C1
	CLR	DL

TIMER1_C1:
        POP     AL                      ; restore registers
        OUT     SREG,AL
        POP     AL
	POP	AH
        RETI

SIN_TABLE:				; sine wave data table
	.db	0x80,0x88,0x90,0x98,0xA0,0xA7,0xAF,0xB6,0xBD,0xC4
	.db	0xCB,0xD1,0xD7,0xDD,0xE2,0xE7,0xEB,0xEF,0xF3,0xF6
	.db	0xF9,0xFB,0xFD,0xFE,0xFF,0xFF,0xFF,0xFE,0xFD,0xFB
	.db	0xF9,0xF6,0xF3,0xEF,0xEB,0xE7,0xE2,0xDD,0xD7,0xD1
	.db	0xCB,0xC4,0xBD,0xB6,0xAF,0xA7,0xA0,0x98,0x90,0x88
	.db	0x80,0x78,0x70,0x68,0x60,0x59,0x51,0x4A,0x43,0x3C
	.db	0x35,0x2F,0x29,0x23,0x1E,0x19,0x15,0x11,0x0D,0x0A
	.db	0x07,0x05,0x03,0x02,0x01,0x01,0x01,0x02,0x03,0x05
	.db	0x07,0x0A,0x0D,0x11,0x15,0x19,0x1E,0x23,0x29,0x2F
	.db	0x35,0x3C,0x43,0x4A,0x51,0x59,0x60,0x68,0x70,0x78

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
