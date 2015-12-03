;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]     Exp10_6.asm : A/D Converter ADC3 vs ADC2 Bipolar Input     []
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
;	Read Bipolar ADC3 with ADC2
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"  ADC3 vs ADC2  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"    +0.00[V]    ",0,0
	CALL	BEEP

	LDI	AL,0b10000111		; ADC enable, 125 kHz
	STS	ADCSRA,AL
	LDI	AL,0b00000000
	STS	ADCSRB,AL
	LDI	AL,0b01111011		; diffrential ADC3 vs ADC2, gain 1
	STS	ADMUX,AL		; (use left adjust and Vref=AVCC)
	CALL	D50US
	CALL	D100US

LOOP:	LDI	CL,16			; read ADC by 16 times
	CLR	BH			; clear sum
	CLR	BL
LOOP1:	LDI	AL,0b11010111		; start conversion and clear ADIF
	STS	ADCSRA,AL
	NOP
LOOP2:	LDS	AL,ADCSRA		; A/D conversion complete ?
	SBRS	AL,ADIF
	JMP	LOOP2
	LDS	AL,ADCL			; ADCL must be read first, then ADCH
	LDS	AH,ADCH
	ASR	AH			; convert 10 bit to 16 bit 2's complement
	ROR	AL			;   (shift right 6 times)
	ASR	AH
	ROR	AL
	ASR	AH
	ROR	AL
	ASR	AH
	ROR	AL
	ASR	AH
	ROR	AL
	ASR	AH
	ROR	AL
	ADD	BL,AL
	ADC	BH,AH
	CALL	D1MS
	DEC	CL
	BRNE	LOOP1
	ASR	BH			; divide sum by 16
	ROR	BL
	ASR	BH
	ROR	BL
	ASR	BH
	ROR	BL
	ASR	BH
	ROR	BL

	LDI	LCD_BUFFER,0xC4		; cursor position
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,'+'		; display +/- sign of voltage
	SBRC	BH,7
	LDI	LCD_BUFFER,'-'
	CALL	LCD_DATA
	SBRS	BH,7			; if negative, calculate absolute value
	JMP	LOOP3
	CLR	AL
	SUB	AL,BL
	CLR	AH
	SBC	AH,BH
	MOV	BH,AH
	MOV	BL,AL

LOOP3:	LDI	AH,high(500)		; voltage = value*5/512
	LDI	AL,low(500)
	CALL	MUL16
	LDI	DH,high(512)
	LDI	DL,low(512)
	CALL	DIV16
	LDI	BL,100			; display X.XX
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,'.'
	CALL	LCD_DATA
	MOV	AL,AH
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	CALL	D200MS			; delay 200 ms
	JMP	LOOP

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
