;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	    Exp10_1.asm : A/D Converter ADC3 Input for VR1	   []
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
;	Read VR1 by ADC3
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"   ADC3 (VR1)   ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" 0000H = 0.0[V] ",0,0
	CALL	BEEP

	LDI	AL,0b10000101		; ADC enable, 500kHz
	STS	ADCSRA,AL
	LDI	AL,0b00000000
	STS	ADCSRB,AL
	LDI	AL,0b01000011		; select ADC3 with AVCC
	STS	ADMUX,AL		; (single ended)
	CALL	D50US
	CALL	D100US

LOOP:	LDI	CL,16			; read ADC by 16 times
	CLR	BH			; clear sum
	CLR	BL
LOOP1:	LDI	AL,0b11010101		; start conversion and clear ADIF
	STS	ADCSRA,AL
	NOP
LOOP2:	LDS	AL,ADCSRA		; A/D conversion complete ?
	SBRS	AL,ADIF
	JMP	LOOP2
	LDS	AL,ADCL			; ADCL must be read first, then ADCH
	LDS	AH,ADCH
	ADD	BL,AL			; add A/D result 16 times
	ADC	BH,AH
	CALL	D1MS
	DEC	CL
	BRNE	LOOP1
	LSR	BH			; divide sum by 16
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL

	LDI	LCD_BUFFER,0xC1		; display in HEX
	CALL	LCD_COMMAND
	MOV	AH,BH
	MOV	AL,BL
	CALL	LCD_4HEX
	LDI	LCD_BUFFER,0xC9		; display in voltage(X.X)
	CALL	LCD_COMMAND
	LDI	AH,high(50)		; Volt = ADC3*5/1024
	LDI	AL,low(50)
	CALL	MUL16
	LDI	DH,high(1024)
	LDI	DL,low(1024)
	CALL	DIV16
	LDI	BL,10			; display X.X
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,'.'
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
