;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	 Exp04_4.asm : Floating Point Number Display Program	   []
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
;	Display VR1 Voltage X.X[V]
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" FLOAT  DISPLAY ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  VR1 = 0.0[V]  ",0,0

	LDI	AL,0b10000101		; ADC enable, 500kHz
	STS	ADCSRA,AL
	LDI	AL,0b00000000
	STS	ADCSRB,AL
	LDI	AL,0b01000011		; select ADC3 with AVCC
	STS	ADMUX,AL
	CALL	D50US
	CALL	D100US

READ_AD:LDI	CL,16			; read ADC by 16 times
	CLR	BH			; clear sum
	CLR	BL
READ_A1:LDI	AL,0b11010101		; start conversion and clear ADIF
	STS	ADCSRA,AL
	NOP
READ_A2:LDS	AL,ADCSRA		; conversion complete ?
	SBRS	AL,ADIF
	JMP	READ_A2
	LDS	AL,ADCL			; ADCL must be read first, then ADCH
	LDS	AH,ADCH
	ADD	BL,AL			; add data 16 times
	ADC	BH,AH
	CALL	D1MS
	DEC	CL
	BRNE	READ_A1
	LSR	BH			; divide sum by 16
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL

	LDI	LCD_BUFFER,0xC8		; display VR1 in voltage(X.X)
	CALL	LCD_COMMAND
	LDI	AH,high(50)		; Volt = ADC3*5/1024
	LDI	AL,low(50)
	CALL	MUL16
	LDI	DH,high(1024)
	LDI	DL,low(1024)
	CALL	DIV16
	LDI	BL,10			; calculate X.X
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,'.'
	CALL	LCD_DATA
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	CALL	D200MS

	JMP	READ_AD

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
