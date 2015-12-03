;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		  Exp07_1.asm : PWM Output by OC0A Pin		   []
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
;	PWM Control by Timer/Counter0
;------------------------------------------------
	CALL	LCD_HOME1		; display title
	CALL	LCD_STRING
	.db	" Period = 255*2 ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Width  = 000*2 ",0,0

        LDI     AL,0b10000001           ; Phase Correct PWM mode(1), use OC0A
        OUT     TCCR0A,AL
        LDI     AL,0b00000011		; f(PWM) = 16MHz/64/510 = 490Hz
        OUT     TCCR0B,AL

LOOP1:	LDI	CL,6			; loop counter
	LDI	DL,0			; initial OCR0A value
LOOP2:	OUT     OCR0A,DL		; set duty ratio = DL*2/510
	LDI	LCD_BUFFER,0xCA		; display duty
	CALL	LCD_COMMAND
	CLR	AH
	MOV	AL,DL
	CALL	LCD_3DEC
	CALL	BEEP
        CALL    D5SEC
	LDI	AL,51			; increment duty ratio by 20%
	ADD	DL,AL			;  ( DL = DL + 51 )
	DEC	CL
	BRNE	LOOP2			; end loop ?
	JMP	LOOP1

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O subroutine file
