;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]    Exp17_1.asm : ASCII(English)-Only Display on Graphic LCD	   []
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
;	Graphic LCD Module : Hyundai LCD, HG12605NY-LY, 128x64 dot
;====================================================================
;------------------------------------------------
;	Graphic LCD Module Address
;------------------------------------------------
;.equ	LCD_DATABUS	0x4000		; GLCD data
;.equ	LCD_CONTROL	0x4100		; GLCD control signal
					;     GND = R/-W    (5, read/write)
					;      D4 = D/-I    (4, data/instruction)
					;      D5 = E       (6, enable)
					;      D6 = CS1     (15, chip select 1)
					;      D7 = CS2     (16, chip select 2)
.dseg
	.org	0x0200
;------------------------------------------------
;	Variables for Graphic LCD Module
;------------------------------------------------
Xcharacter:	.byte	1		; X character(0-7) of graphic LCD module
Ycharacter:	.byte	1		; Y character(0-19) of graphic LCD module
CURSOR_FLAG:	.byte	1		; cursor on/off flag(0=off, 1=on)
Xcursor:	.byte	1		; X cursor position of graphic LCD module
Ycursor:	.byte	1		; Y cursor position of graphic LCD module

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

	CALL	CLEAR_SCREEN		; clear graphic LCD screen
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
	.db	" ASCII(English) ",0,0

;------------------------------------------------
;	Display English ASCII
;------------------------------------------------
LOOP:	CALL	GLCD_HOME0		; display screen 1
	CALL	GLCD_STRING
	.db	"  OK-2561 Kit V1.0  ",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"     2006/10/01     ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"                    ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"     Designed by    ",0,0
	CALL	GLCD_HOME4
	CALL	GLCD_STRING
	.db	"   Duck-Yong Yoon.  ",0,0
	CALL	GLCD_HOME5
	CALL	GLCD_STRING
	.db	"                    ",0,0
	CALL	GLCD_HOME6
	CALL	GLCD_STRING
	.db	"      Made by       ",0,0
	CALL	GLCD_HOME7
	CALL	GLCD_STRING
	.db	" Ohm Publishing Co. ",0,0
	CALL	D5SEC

	CALL	GLCD_HOME0		; display screen 2
	CALL	GLCD_STRING
	.db	"     Hyundai LCD    ",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"    HG12605NY-LY    ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"                    ",0,0
	CALL	GLCD_HOME3
	CALL	GLCD_STRING
	.db	"    Yellow/Green    ",0,0
	CALL	GLCD_HOME4
	CALL	GLCD_STRING
	.db	"   LED Backlight    ",0,0
	CALL	GLCD_HOME5
	CALL	GLCD_STRING
	.db	"                    ",0,0
	CALL	GLCD_HOME6
	CALL	GLCD_STRING
	.db	"128 x 64 Graphic LCD",0,0
	CALL	GLCD_HOME7
	CALL	GLCD_STRING
	.db	" 6x8 Box, 5x7 ASCII ",0,0
	CALL	D5SEC

	CALL	GLCD_HOME0		; display screen 3
	CALL	GLCD_STRING
	.db	"====================",0,0
	CALL	GLCD_HOME1
	CALL	GLCD_STRING
	.db	"  ASCII Characters  ",0,0
	CALL	GLCD_HOME2
	CALL	GLCD_STRING
	.db	"====================",0,0
	CALL	GLCD_HOME7
	CALL	GLCD_STRING
	.db	"                    ",0,0

	CALL	GLCD_HOME3		; from 0x20 to 0x7E
	LDI	CL,95
	LDI	DL,0x20
LOOP1:	MOV	LCD_BUFFER,DL
	CALL	GLCD_CHARACTER
	INC	DL
	DEC	CL
	BRNE	LOOP1
	CALL	D5SEC
	JMP	LOOP

;====================================================================
;	Graphic LCD Display Subroutine
;====================================================================
;------------------------------------------------
;	Clear Graphic LCD Screen
;------------------------------------------------
CLEAR_SCREEN:
	LDI	AL,0b00000000		; clear all control signals
	STS	LCD_CONTROL,AL
	LDI	AL,0b11000000		; CS1,CS2 display ON
	LDI	LCD_BUFFER,0b00111111
	CALL	GLCD_COMMAND
	LDI	AL,0b11000000		; CS1,CS2 display start line
	LDI	LCD_BUFFER,0b11000000
	CALL	GLCD_COMMAND

	LDI	BL,0b10111000		; X start address
	LDI	CH,8
CLEAR_S1:LDI	BH,0b01000000		; Y start address
	LDI	AL,0b11000000		; CS1,CS2 X address
	MOV	LCD_BUFFER,BL
	CALL	GLCD_COMMAND
	LDI	AL,0b11000000		; CS1,CS2 Y address
	MOV	LCD_BUFFER,BH
	CALL	GLCD_COMMAND
	LDI	CL,64
CLEAR_S2:LDI	AL,0b11000000		; clear CS1 and CS2
	LDI	LCD_BUFFER,0x00
	CALL	GLCD_DATA
	DEC	CL
	BRNE	CLEAR_S2
	INC	BL
	DEC	CH
	BRNE	CLEAR_S1
	RET

;------------------------------------------------
;	Set Graphic LCD Display Position
;------------------------------------------------
GLCD_HOME0:LDI	AL,0
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	RJMP	GLCD_XY
GLCD_HOME1:LDI	AL,1
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	RJMP	GLCD_XY
GLCD_HOME2:LDI	AL,2
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	RJMP	GLCD_XY
GLCD_HOME3:LDI	AL,3
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	RJMP	GLCD_XY
GLCD_HOME4:LDI	AL,4
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	RJMP	GLCD_XY
GLCD_HOME5:LDI	AL,5
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	RJMP	GLCD_XY
GLCD_HOME6:LDI	AL,6
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL
	RJMP	GLCD_XY
GLCD_HOME7:LDI	AL,7
	STS	Xcharacter,AL
	LDI	AL,0
	STS	Ycharacter,AL

GLCD_XY:LDS	AL,Xcharacter		; Xcharacter (0-7)
	LDI	LCD_BUFFER,0b10111000
	ADD	LCD_BUFFER,AL
	LDI	AL,0b11000000
	CALL	GLCD_COMMAND
	LDS	AL,Ycharacter		; Ycharacter (0-19)
	CPI	AL,10
	BRSH	GLCD_XY1
	PUSH	R0
	PUSH	R1
	LDI	AH,6			; if Ycharacter =< 9, CS1 Ycharacter
	MUL	AL,AH
	LDI	LCD_BUFFER,0b01000000
	ADD	LCD_BUFFER,R0
	LDI	AL,4
	ADD	LCD_BUFFER,AL
	LDI	AL,0b01000000
	CALL	GLCD_COMMAND
	POP	R1
	POP	R0
	RET
GLCD_XY1:PUSH	R0
	PUSH	R1
	SUBI	AL,10			; if Ycharacter >= 10, CS2 Ycharacter
	LDI	AH,6
	MUL	AL,AH
	LDI	LCD_BUFFER,0b01000000
	ADD	LCD_BUFFER,R0
	LDI	AL,0b10000000
	CALL	GLCD_COMMAND
	POP	R1
	POP	R0
	RET

;------------------------------------------------
;	Write a Command (AL, LCD_BUFFER)
;------------------------------------------------
GLCD_COMMAND:
	PUSH	AL
	ANDI	AL,0b11000000		; clear all signals except CS1, CS2
	STS	LCD_CONTROL,AL		; D/-I = 0
	PUSH	R0			; delay for 500 ns
	POP	R0
	PUSH	R0
	POP	R0
	STS	LCD_DATABUS,LCD_BUFFER	; write command
	ORI	AL,0b00100000		; E = 1
	STS	LCD_CONTROL,AL
	PUSH	R0			; delay for 500 ns
	POP	R0
	PUSH	R0
	POP	R0
	ANDI	AL,0b11011111		; E = 0
	STS	LCD_CONTROL,AL
	LDI	AL,0b00000000		; clear all control signals
	STS	LCD_CONTROL,AL
	CALL	D10US			; wait for GLCD busy
	POP	AL
	RET

;------------------------------------------------
;	Write a Data (AL, LCD_BUFFER)
;------------------------------------------------
GLCD_DATA:PUSH	AL
	ANDI	AL,0b11000000		; clear all signals except CS1, CS2
	ORI	AL,0b00010000		; D/-I = 1
	STS	LCD_CONTROL,AL
	PUSH	R0			; delay for 500 ns
	POP	R0
	PUSH	R0
	POP	R0
	STS	LCD_DATABUS,LCD_BUFFER	; write data
	ORI	AL,0b00100000		; E = 1
	STS	LCD_CONTROL,AL
	PUSH	R0			; delay for 500 ns
	POP	R0
	PUSH	R0
	POP	R0
	ANDI	AL,0b11011111		; E = 0
	STS	LCD_CONTROL,AL
	LDI	AL,0b00000000		; clear all control signals
	STS	LCD_CONTROL,AL
	CALL	D10US			; wait for GLCD busy
	POP	AL
	RET

;------------------------------------------------
;	Display a 6x8 Dot ASCII Character(LCD_BUFFER)
;------------------------------------------------
GLCD_CHARACTER:
	PUSH	R0			; store registers
	PUSH	R1
	PUSH	AL
	PUSH	AH
	PUSH	BL
	PUSH	BH
	PUSH	ZL
	PUSH	ZH

	PUSH	LCD_BUFFER
	CALL	GLCD_XY			; go display position
	POP	LCD_BUFFER

	CPI	LCD_BUFFER,0x7F		; if ASCII >= 0x7F, space character
	BRSH	GLCD_CH1
	CPI	LCD_BUFFER,0x20		; if ASCII < 0x20, space character
	BRSH	GLCD_CH2
GLCD_CH1:LDI	LCD_BUFFER,0x20
GLCD_CH2:LDI	AH,byte3(FONT << 1)	; start address of character font
	LDI	ZH,high(FONT << 1)
	LDI	ZL,low(FONT << 1)
	SUBI	LCD_BUFFER,0x20		; from 0x20 to 0x7E
	LDI	AL,6
	MUL	AL,LCD_BUFFER
	ADD	ZL,R0
	ADC	ZH,R1
	CLR	AL
	ADC	AH,AL
	OUT	RAMPZ,AH

	LDI	BL,5			; display 5-byte font
	LDS	AL,Ycharacter		; CS1 or CS2 ?
	CPI	AL,10
	BRSH	GLCD_CH3
	LDI	AL,0b01000000
	RJMP	GLCD_CH4
GLCD_CH3:LDI	AL,0b10000000
GLCD_CH4:ELPM	LCD_BUFFER,Z+
	LDS	AH,CURSOR_FLAG		; cursor on ?
	CPI	AH,0
	BREQ	GLCD_CH5
	LDS	AH,Xcharacter		; Xcharacter = Xcursor ?
	LDS	BH,Xcursor
	CP	AH,BH
	BRNE	GLCD_CH5
	LDS	AH,Ycharacter		; Ycharacter = Ycursor ?
	LDS	BH,Ycursor
	CP	AH,BH
	BRNE	GLCD_CH5
	ORI	LCD_BUFFER,0b10000000	; if yes, display cursor
GLCD_CH5:CALL	GLCD_DATA
	DEC	BL
	BRNE	GLCD_CH4
	LDI	LCD_BUFFER,0b00000000	; last byte 0x00
	CALL	GLCD_DATA

	LDS	AL,Ycharacter		; go next character position
	INC	AL
	CPI	AL,20
	BRNE	GLCD_CH6
	LDS	AL,Xcharacter
	INC	AL
	STS	Xcharacter,AL
	CLR	AL
GLCD_CH6:STS	Ycharacter,AL

	POP	ZH			; restore registers
	POP	ZL
	POP	BH
	POP	BL
	POP	AH
	POP	AL
	POP	R1
	POP	R0
	RET

;------------------------------------------------
;	Display a String on Graphic LCD
;------------------------------------------------
GLCD_STRING:
	OUT	GPIOR0,ZL		; store RAMPZ:ZH:ZL
	OUT	GPIOR1,ZH
	IN	ZL,RAMPZ
	OUT	GPIOR2,ZL
#if(defined(__ATmega2560__) || defined(__ATmega2561__))
	POP	RAMPZ_BUFFER		; get start address of string(3-byte PC)
#else
	CLR	RAMPZ_BUFFER		; get start address of string(2-byte PC)
#endif
	POP	ZH
	POP	ZL
	LSL	ZL			; convert into byte address
	ROL	ZH
	ROL	RAMPZ_BUFFER
	OUT	RAMPZ,RAMPZ_BUFFER
GLCD_ST1:ELPM	LCD_BUFFER,Z+		; read a character. end string ?
	CPI	LCD_BUFFER,0
	BREQ	GLCD_ST2		; if yes, return
	CALL	GLCD_CHARACTER		; if no, display a character
	RJMP	GLCD_ST1
GLCD_ST2:SBRC	ZL,0			; if odd address, skip 1 address
	ELPM	LCD_BUFFER,Z+
	IN	RAMPZ_BUFFER,RAMPZ	; convert into word address
	LSR	RAMPZ_BUFFER
	ROR	ZH
	ROR	ZL
	PUSH	ZL			; store new return address
	PUSH	ZH
#if(defined(__ATmega2560__) || defined(__ATmega2561__))
	PUSH	RAMPZ_BUFFER
#endif
	IN	ZL,GPIOR2		; restore RAMPZ:ZH:ZL
	OUT	RAMPZ,ZL
	IN	ZH,GPIOR1
	IN	ZL,GPIOR0
	RET

;====================================================================
;	5x7 ASCII Character Font Data (6x8 Character Box)
;====================================================================
FONT:	.db	0x00,0x00,0x00,0x00,0x00,0	; 0x20 space
	.db	0x00,0x00,0x4F,0x00,0x00,0	; 0x21 !
	.db	0x00,0x07,0x00,0x07,0x00,0	; 0x22 "
	.db	0x14,0x7F,0x14,0x7F,0x14,0	; 0x23 #
	.db	0x24,0x2A,0x7F,0x2A,0x12,0	; 0x24 $
	.db	0x23,0x13,0x08,0x64,0x62,0	; 0x25 %
	.db	0x36,0x49,0x55,0x22,0x50,0	; 0x26 &
	.db	0x00,0x05,0x03,0x00,0x00,0	; 0x27 '
	.db	0x00,0x1C,0x22,0x41,0x00,0	; 0x28 (
	.db	0x00,0x41,0x22,0x1C,0x00,0	; 0x29 )
	.db	0x14,0x08,0x3E,0x08,0x14,0	; 0x2A *
	.db	0x08,0x08,0x3E,0x08,0x08,0	; 0x2B +
	.db	0x00,0x50,0x30,0x00,0x00,0	; 0x2C ,
	.db	0x08,0x08,0x08,0x08,0x08,0	; 0x2D -
	.db	0x00,0x60,0x60,0x00,0x00,0	; 0x2E .
	.db	0x20,0x10,0x08,0x04,0x02,0	; 0x2F /
	.db	0x3E,0x51,0x49,0x45,0x3E,0	; 0x30 0
	.db	0x00,0x42,0x7F,0x40,0x00,0	; 0x31 1
	.db	0x42,0x61,0x51,0x49,0x46,0	; 0x32 2
	.db	0x21,0x41,0x45,0x4B,0x31,0	; 0x33 3
	.db	0x18,0x14,0x12,0x7F,0x10,0	; 0x34 4
	.db	0x27,0x45,0x45,0x45,0x39,0	; 0x35 5
	.db	0x3C,0x4A,0x49,0x49,0x30,0	; 0x36 6
	.db	0x01,0x71,0x09,0x05,0x03,0	; 0x37 7
	.db	0x36,0x49,0x49,0x49,0x36,0	; 0x38 8
	.db	0x06,0x49,0x49,0x29,0x1E,0	; 0x39 9
	.db	0x00,0x36,0x36,0x00,0x00,0	; 0x3A :
	.db	0x00,0x56,0x36,0x00,0x00,0	; 0x3B ;
	.db	0x08,0x14,0x22,0x41,0x00,0	; 0x3C <
	.db	0x14,0x14,0x14,0x14,0x14,0	; 0x3D =
	.db	0x00,0x41,0x22,0x14,0x08,0	; 0x3E >
	.db	0x02,0x01,0x51,0x09,0x06,0	; 0x3F ?
	.db	0x32,0x49,0x79,0x41,0x3E,0	; 0x40 @
	.db	0x7E,0x11,0x11,0x11,0x7E,0	; 0x41 A
	.db	0x7F,0x49,0x49,0x49,0x36,0	; 0x42 B
	.db	0x3E,0x41,0x41,0x41,0x22,0	; 0x43 C
	.db	0x7F,0x41,0x41,0x22,0x1C,0	; 0x44 D
	.db	0x7F,0x49,0x49,0x49,0x41,0	; 0x45 E
	.db	0x7F,0x09,0x09,0x09,0x01,0	; 0x46 F
	.db	0x3E,0x41,0x49,0x49,0x7A,0	; 0x47 G
	.db	0x7F,0x08,0x08,0x08,0x7F,0	; 0x48 H
	.db	0x00,0x41,0x7F,0x41,0x00,0	; 0x49 I
	.db	0x20,0x40,0x41,0x3F,0x01,0	; 0x4A J
	.db	0x7F,0x08,0x14,0x22,0x41,0	; 0x4B K
	.db	0x7F,0x40,0x40,0x40,0x40,0	; 0x4C L
	.db	0x7F,0x02,0x0C,0x02,0x7F,0	; 0x4D M
	.db	0x7F,0x04,0x08,0x10,0x7F,0	; 0x4E N
	.db	0x3E,0x41,0x41,0x41,0x3E,0	; 0x4F O
	.db	0x7F,0x09,0x09,0x09,0x06,0	; 0x50 P
	.db	0x3E,0x41,0x51,0x21,0x5E,0	; 0x51 Q
	.db	0x7F,0x09,0x19,0x29,0x46,0	; 0x52 R
	.db	0x26,0x49,0x49,0x49,0x32,0	; 0x53 S
	.db	0x01,0x01,0x7F,0x01,0x01,0	; 0x54 T
	.db	0x3F,0x40,0x40,0x40,0x3F,0	; 0x55 U
	.db	0x1F,0x20,0x40,0x20,0x1F,0	; 0x56 V
	.db	0x3F,0x40,0x38,0x40,0x3F,0	; 0x57 W
	.db	0x63,0x14,0x08,0x14,0x63,0	; 0x58 X
	.db	0x07,0x08,0x70,0x08,0x07,0	; 0x59 Y
	.db	0x61,0x51,0x49,0x45,0x43,0	; 0x5A Z
	.db	0x00,0x7F,0x41,0x41,0x00,0	; 0x5B [
	.db	0x02,0x04,0x08,0x10,0x20,0	; 0x5C \
	.db	0x00,0x41,0x41,0x7F,0x00,0	; 0x5D ]
	.db	0x04,0x02,0x01,0x02,0x04,0	; 0x5E ^
	.db	0x40,0x40,0x40,0x40,0x40,0	; 0x5F _
	.db	0x00,0x01,0x02,0x04,0x00,0	; 0x60 `
	.db	0x20,0x54,0x54,0x54,0x78,0	; 0x61 a
	.db	0x7F,0x48,0x44,0x44,0x38,0	; 0x62 b
	.db	0x38,0x44,0x44,0x44,0x20,0	; 0x63 c
	.db	0x38,0x44,0x44,0x48,0x7F,0	; 0x64 d
	.db	0x38,0x54,0x54,0x54,0x18,0	; 0x65 e
	.db	0x08,0x7E,0x09,0x01,0x02,0	; 0x66 f
	.db	0x0C,0x52,0x52,0x52,0x3E,0	; 0x67 g
	.db	0x7F,0x08,0x04,0x04,0x78,0	; 0x68 h
	.db	0x00,0x04,0x7D,0x00,0x00,0	; 0x69 i
	.db	0x20,0x40,0x44,0x3D,0x00,0	; 0x6A j
	.db	0x7F,0x10,0x28,0x44,0x00,0	; 0x6B k
	.db	0x00,0x41,0x7F,0x40,0x00,0	; 0x6C l
	.db	0x7C,0x04,0x18,0x04,0x7C,0	; 0x6D m
	.db	0x7C,0x08,0x04,0x04,0x78,0	; 0x6E n
	.db	0x38,0x44,0x44,0x44,0x38,0	; 0x6F o
	.db	0x7C,0x14,0x14,0x14,0x08,0	; 0x70 p
	.db	0x08,0x14,0x14,0x18,0x7C,0	; 0x71 q
	.db	0x7C,0x08,0x04,0x04,0x08,0	; 0x72 r
	.db	0x48,0x54,0x54,0x54,0x20,0	; 0x73 s
	.db	0x04,0x3F,0x44,0x40,0x20,0	; 0x74 t
	.db	0x3C,0x40,0x40,0x20,0x7C,0	; 0x75 u
	.db	0x1C,0x20,0x40,0x20,0x1C,0	; 0x76 v
	.db	0x3C,0x40,0x30,0x40,0x3C,0	; 0x77 w
	.db	0x44,0x28,0x10,0x28,0x44,0	; 0x78 x
	.db	0x0C,0x50,0x50,0x50,0x3C,0	; 0x79 y
	.db	0x44,0x64,0x54,0x4C,0x44,0	; 0x7A z
	.db	0x00,0x08,0x36,0x41,0x00,0	; 0x7B {
	.db	0x00,0x00,0x77,0x00,0x00,0	; 0x7C |
	.db	0x00,0x41,0x36,0x08,0x00,0	; 0x7D }
	.db	0x08,0x04,0x08,0x10,0x08,0	; 0x7E ~

;------------------------------------------------
;	Include User Subroutine File
;------------------------------------------------
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
