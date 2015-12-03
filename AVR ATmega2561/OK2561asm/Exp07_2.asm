;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp07_2.asm : PWM Output by OC1A Pin		   []
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
;	PWM Control by Timer/Counter1
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	"Period =  1250*2",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"Width  =  0000*2",0,0

        LDI     AL,0b10000010           ; Phase Correct PWM mode(10), use OC1A
        STS     TCCR1A,AL
        LDI     AL,0b00010011
        STS     TCCR1B,AL
        LDI     AL,0b00000000
        STS     TCCR1C,AL
        LDI     AL,high(1250)		; f(PWM) = 16MHz/64/2/1250 = 100Hz
        STS     ICR1H,AL
        LDI     AL,low(1250)
        STS     ICR1L,AL

LOOP1:	LDI	CL,6			; loop counter
	LDI	DH,0			; initial OCR1A value
	LDI	DL,0
LOOP2:	STS     OCR1AH,DH		; set duty ratio = DH:DL/1250
	STS	OCR1AL,DL
	LDI	LCD_BUFFER,0xCA		; display duty
	CALL	LCD_COMMAND
	MOV	AH,DH
	MOV	AL,DL
	CALL	LCD_4DEC
	CALL	BEEP
        CALL    D5SEC
	LDI	AL,250			; increment duty ratio by 20%
	ADD	DL,AL			;  ( DH:DL = DH:DL + 250 )
	CLR	AL
	ADC	DH,AL
	DEC	CL
	BRNE	LOOP2			; end loop ?
	JMP	LOOP1

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
