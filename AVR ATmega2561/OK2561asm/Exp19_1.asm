;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	     Exp19_1.asm : English/Korean Language Display	   []
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
	.db	" English/Korean ",0,0

;------------------------------------------------
;	Display ASCII and Korean Language
;------------------------------------------------
LOOP:	CALL	GLCD_HOME0		; display screen 1
	CALL	GLCD_STRING
	.db	"OK-2561ŰƮ V1.0",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"   2006/10/01   ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	" ���� : ������  ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	" �Ǹ� : Ohm��   ",0,0
	CALL	D5SEC

	CALL	GLCD_HOME0		; display screen 2
	CALL	GLCD_STRING
	.db	"    ���� LCD    ",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"  HG12605NY-LY  ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"                ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"128x64 �׷���LCD",0,0
	CALL	D5SEC

	CALL	GLCD_HOME0		; display screen 3
	CALL	GLCD_STRING
	.db	" ����/�ѱ� ǥ�� ",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"                ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	" 8x16��Ʈ ASCII ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"16x16��Ʈ ������",0,0
	CALL	D5SEC

	CALL	GLCD_HOME0		; display screen 4
	CALL	GLCD_STRING
	.db	"   * ����� *   ",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"����ũ�����μ���",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"   * ������ *   ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"����ũ����Ʈ�ѷ�",0,0
	CALL	D5SEC

	LDI	DL,0x00			; from 0x00 to 0x7F
	LDI	CL,2
LOOP1:	LDI	CH,64
	CALL	GLCD_HOME0
LOOP2:	MOV	LCD_BUFFER,DL
	CALL	GLCD_ENGLISH
	INC	DL
	DEC	CH
	BRNE	LOOP2
	CALL	D5SEC
	DEC	CL
	BRNE	LOOP1
	JMP	LOOP

;------------------------------------------------
;	Include User Subroutine File
;------------------------------------------------
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
.include "GLCD2561.INC"			; OK-2561 GLCD subroutine file
