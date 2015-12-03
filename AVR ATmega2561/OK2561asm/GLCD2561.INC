;====================================================================
;	Graphic LCD Display Subroutines for OK-2561 V1.0 Kit
;	(Hyundai LCD : HG12605NY-LY, 128x64 dot, LED backlight)
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
	.org	0x1F80
;------------------------------------------------
;	Variables for Graphic LCD Module
;------------------------------------------------
KOREAN_BUFFER:	.byte	32		; 32 byte Korean font buffer

K_CODE:		.byte	2		; Korean code for graphic LCD module
CHO_5BIT:	.byte	1		; chosung 5 bit of Korean code
JOONG_5BIT:	.byte	1		; joongsung 5 bit of Korean code
JONG_5BIT:	.byte	1		; jongsung 5 bit of Korean code
CHO_BUL:	.byte	1		; chosung bul number of Korean font
JOONG_BUL:	.byte	1		; joongsung bul number of Korean font
JONG_BUL:	.byte	1		; jongsung bul number of Korean font
JONG_FLAG:	.byte	1		; jongsung flag(0=don't have, 1=have)
Xcharacter:	.byte	1		; X charcater(0-3) of graphic LCD
Ycharacter:	.byte	1		; Y character(0-15) of graphic LCD
CURSOR_FLAG:	.byte	1		; cursor on/off flag(0=off, 1=on)
Xcursor:	.byte	1		; X cursor position of graphic LCD module
Ycursor:	.byte	1		; Y cursor position of graphic LCD module

.cseg
;------------------------------------------------
;	Clear Graphic LCD Screen
;------------------------------------------------
GLCD_CLEAR:
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
GLCD_C1:LDI	BH,0b01000000		; Y start address
	LDI	AL,0b11000000		; CS1,CS2 X address
	MOV	LCD_BUFFER,BL
	CALL	GLCD_COMMAND
	LDI	AL,0b11000000		; CS1,CS2 Y address
	MOV	LCD_BUFFER,BH
	CALL	GLCD_COMMAND
	LDI	CL,64
GLCD_C2:LDI	AL,0b11000000		; clear CS1 and CS2
	LDI	LCD_BUFFER,0x00
	CALL	GLCD_DATA
	DEC	CL
	BRNE	GLCD_C2
	INC	BL
	DEC	CH
	BRNE	GLCD_C1
	RET

;------------------------------------------------
;	Set Graphic LCD Display Position
;------------------------------------------------
GLCD_HOME0:LDI	AL,0
	STS	Xcharacter,AL
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

GLCD_XY:PUSH	AH
	LDS	LCD_BUFFER,Xcharacter	; Xcharacter (0 - 3)
	LSL	LCD_BUFFER		;   (upper row)
	LDI	AL,0b10111000
	ADD	LCD_BUFFER,AL
	LDI	AL,0b11000000
	CALL	GLCD_COMMAND
	RJMP	GLCD_XY2
GLCD_XY1:PUSH	AH
	LDS	LCD_BUFFER,Xcharacter	; Xcharacter + 1
	LSL	LCD_BUFFER		;   (lower row)
	LDI	AL,0b10111001
	ADD	LCD_BUFFER,AL
	LDI	AL,0b11000000
	CALL	GLCD_COMMAND
GLCD_XY2:LDS	AL,Ycharacter		; Ycharacter (0 - 15)
	CPI	AL,8
	BRSH	GLCD_XY3
	PUSH	R0
	PUSH	R1
	LDI	AH,8			; if Ycharacter =< 7, CS1 Ycharacter
	MUL	AL,AH
	LDI	LCD_BUFFER,0b01000000
	ADD	LCD_BUFFER,R0
	LDI	AL,0b01000000
	CALL	GLCD_COMMAND
	POP	R1
	POP	R0
	POP	AH
	RET
GLCD_XY3:PUSH	R0
	PUSH	R1
	SUBI	AL,8			; if Ycharacter >= 8, CS2 Ycharacter
	LDI	AH,8
	MUL	AL,AH
	LDI	LCD_BUFFER,0b01000000
	ADD	LCD_BUFFER,R0
	LDI	AL,0b10000000
	CALL	GLCD_COMMAND
	POP	R1
	POP	R0
	POP	AH
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
;	Display a String
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
GLCD_ST1:ELPM	LCD_BUFFER,Z+		; get a character
	CPI	LCD_BUFFER,0
	BREQ	GLCD_ST3
	SBRC	LCD_BUFFER,7
	RJMP	GLCD_ST2
	RCALL	GLCD_ENGLISH		; display 8x16 English character
	RJMP	GLCD_ST1
GLCD_ST2:STS	K_CODE,LCD_BUFFER	; display 16x16 Korean character
	ELPM	LCD_BUFFER,Z+
	STS	K_CODE+1,LCD_BUFFER
	RCALL	GLCD_KOREAN
	RJMP	GLCD_ST1
GLCD_ST3:SBRC	ZL,0
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
;	Display a 8x16 Dot English(ASCII) Character (LCD_BUFFER)
;====================================================================
GLCD_ENGLISH:
	PUSH	R0			; store registers
	PUSH	R1
	PUSH	AL
	PUSH	AH
	PUSH	BL
	PUSH	BH
	PUSH	ZL
	PUSH	ZH

	SBRC	LCD_BUFFER,7		; if ASCII >= 0x80, null character
	CLR	LCD_BUFFER

	LDI	AH,byte3(E_FONT << 1)	; start address of character font
	LDI	ZH,high(E_FONT << 1)
	LDI	ZL,low(E_FONT << 1)
	LDI	AL,16
	MUL	AL,LCD_BUFFER
	ADD	ZL,R0
	ADC	ZH,R1
	CLR	AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	CALL	GLCD_XY			; upper row of 16-byte font
	LDI	BL,8
	LDS	AL,Ycharacter		; CS1 or CS2 ?
	CPI	AL,8
	BRSH	GLCD_ENG1
	LDI	AL,0b01000000
	RJMP	GLCD_ENG2
GLCD_ENG1:LDI	AL,0b10000000
GLCD_ENG2:ELPM	LCD_BUFFER,Z+
	CALL	GLCD_DATA
	DEC	BL
	BRNE	GLCD_ENG2

	CALL	GLCD_XY1		; lower row of 16-byte font
	LDI	BL,8
	LDS	AL,Ycharacter		; CS1 or CS2 ?
	CPI	AL,8
	BRSH	GLCD_ENG3
	LDI	AL,0b01000000
	RJMP	GLCD_ENG4
GLCD_ENG3:LDI	AL,0b10000000
GLCD_ENG4:ELPM	LCD_BUFFER,Z+
	LDS	AH,CURSOR_FLAG		; cursor on ?
	CPI	AH,0
	BREQ	GLCD_ENG5
	LDS	AH,Xcharacter		; Xcharacter = Xcursor ?
	LDS	BH,Xcursor
	CP	AH,BH
	BRNE	GLCD_ENG5
	LDS	AH,Ycharacter		; Ycharacter = Ycursor ?
	LDS	BH,Ycursor
	CP	AH,BH
	BRNE	GLCD_ENG5
	ORI	LCD_BUFFER,0b10000000	; if yes, display cursor
GLCD_ENG5:CALL	GLCD_DATA
	DEC	BL
	BRNE	GLCD_ENG4

	LDS	AL,Ycharacter		; go next character position
	INC	AL
	CPI	AL,16
	BRNE	GLCD_ENG6
	LDS	AL,Xcharacter
	INC	AL
	STS	Xcharacter,AL
	CLR	AL
GLCD_ENG6:STS	Ycharacter,AL

	POP	ZH			; restore registers
	POP	ZL
	POP	BH
	POP	BL
	POP	AH
	POP	AL
	POP	R1
	POP	R0
	RET

;====================================================================
;	Display a 16x16 Dot Korean Character (K_CODE)
;====================================================================
GLCD_KOREAN:
	PUSH	R0			; store registers
	PUSH	R1
	PUSH	AL
	PUSH	AH
	PUSH	BL
	PUSH	BH
	PUSH	YL
	PUSH	YH
	PUSH	ZL
	PUSH	ZH

	LDS	AL,Ycharacter		; if can not display, go next line
	CPI	AL,15
	BRNE	GLCD_KOR0
	LDI	LCD_BUFFER,' '
	CALL	GLCD_ENGLISH
GLCD_KOR0:NOP

;------------------------------------------------
;	Get chosung, joongsung, jongsung 5 bit
;------------------------------------------------
	LDS	AL,K_CODE		; chosung 5 bit
	LSR	AL
	LSR	AL
	ANDI	AL,0b00011111
	LDI	AH,byte3(TBL_CHO << 1)
	LDI	ZH,high(TBL_CHO << 1)
	LDI	ZL,low(TBL_CHO << 1)
	ADD	ZL,AL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z
	STS	CHO_5BIT,AL
	LDS	AH,K_CODE		; joongsung 5 bit
	LDS	AL,K_CODE+1
	LSL	AL
	ROL	AH
	LSL	AL
	ROL	AH
	LSL	AL
	ROL	AH
	MOV	AL,AH
	ANDI	AL,0b00011111
	LDI	AH,byte3(TBL_JOONG << 1)
	LDI	ZH,high(TBL_JOONG << 1)
	LDI	ZL,low(TBL_JOONG << 1)
	ADD	ZL,AL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z
	STS	JOONG_5BIT,AL
	LDS	AL,K_CODE+1		; jongsung 5 bit
	ANDI	AL,0b00011111
	LDI	AH,byte3(TBL_JONG << 1)
	LDI	ZH,high(TBL_JONG << 1)
	LDI	ZL,low(TBL_JONG << 1)
	ADD	ZL,AL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z
	STS	JONG_5BIT,AL

;------------------------------------------------
;	Get chosung, joongsung, jongsung bul number
;------------------------------------------------
	LDS	AL,JONG_5BIT		; have jongsung ?
	CPI	AL,0
	BRNE	GLCD_BUL4
	CLR	AH			; don't have jongsung
	STS	JONG_FLAG,AH
	LDI	AH,byte3(BUL_CHO1 << 1)	; chosung bul
	LDI	ZH,high(BUL_CHO1 << 1)
	LDI	ZL,low(BUL_CHO1 << 1)
	LDS	AL,JOONG_5BIT
	ADD	ZL,AL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z
	STS	CHO_BUL,AL
	LDS	AL,CHO_5BIT		; joongsung bul
	CPI	AL,1
	BRNE	GLCD_BUL1
	CLR	AH
	STS	JOONG_BUL,AH
	RJMP	GLCD_BUL3
GLCD_BUL1:LDS	AL,CHO_5BIT
	CPI	AL,16
	BRNE	GLCD_BUL2
	CLR	AH
	STS	JOONG_BUL,AH
	RJMP	GLCD_BUL3
GLCD_BUL2:LDI	AH,1
	STS	JOONG_BUL,AH
GLCD_BUL3:RJMP	GLCD_BUF

GLCD_BUL4:LDI	AH,1			; have jongsung
	STS	JONG_FLAG,AH
	LDI	AH,byte3(BUL_CHO2 << 1)	; chosung bul
	LDI	ZH,high(BUL_CHO2 << 1)
	LDI	ZL,low(BUL_CHO2 << 1)
	LDS	AL,JOONG_5BIT
	ADD	ZL,AL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z
	STS	CHO_BUL,AL
	LDS	AL,CHO_5BIT		; joongsung bul
	CPI	AL,1
	BRNE	GLCD_BUL5
	LDI	AH,2
	STS	JOONG_BUL,AH
	RJMP	GLCD_BUL7
GLCD_BUL5:LDS	AL,CHO_5BIT
	CPI	AL,16
	BRNE	GLCD_BUL6
	LDI	AH,2
	STS	JOONG_BUL,AH
	RJMP	GLCD_BUL7
GLCD_BUL6:LDI	AH,3
	STS	JOONG_BUL,AH
GLCD_BUL7:LDI	AH,byte3(BUL_JONG << 1)	; jongsung bul
	LDI	ZH,high(BUL_JONG << 1)
	LDI	ZL,low(BUL_JONG << 1)
	LDS	AL,JOONG_5BIT
	ADD	ZL,AL
	CLR	AL
	ADC	ZH,AL
	ADC	AH,AL
	OUT	RAMPZ,AH
	ELPM	AL,Z
	STS	JONG_BUL,AL

;------------------------------------------------
;	Make Buffer for Korean Character
;------------------------------------------------
GLCD_BUF:CLR	AH			; chosung offset
	LDS	AL,CHO_BUL		;  (bul * 20 * 32)
	LDI	BH,high(640)
	LDI	BL,low(640)
	CALL	MUL16
	LDS	BL,CHO_5BIT
	LDI	BH,32
	MUL	BL,BH
	LDI	BH,byte3(CHO_FONT0 << 1)
	LDI	ZH,high(CHO_FONT0 << 1)
	LDI	ZL,low(CHO_FONT0 << 1)
	ADD	ZL,R0
	ADC	ZH,R1
	CLR	BL
	ADC	BH,BL
	ADD	ZL,AL
	ADC	ZH,AH
	ADC	BH,BL
	OUT	RAMPZ,BH
	LDI	YH,high(KOREAN_BUFFER)	; copy chosung to Korean buffer
	LDI	YL,low(KOREAN_BUFFER)
	LDI	BL,32
GLCD_BUF1:ELPM	AL,Z+
	ST	Y+,AL
	DEC	BL
	BRNE	GLCD_BUF1

	CLR	AH			; joongsung offset
	LDS	AL,JOONG_BUL		;  (bul * 22 * 32)
	LDI	BH,high(704)
	LDI	BL,low(704)
	CALL	MUL16
	LDS	BL,JOONG_5BIT
	LDI	BH,32
	MUL	BL,BH
	LDI	BH,byte3(JOONG_FONT0 << 1)
	LDI	ZH,high(JOONG_FONT0 << 1)
	LDI	ZL,low(JOONG_FONT0 << 1)
	ADD	ZL,R0
	ADC	ZH,R1
	CLR	BL
	ADC	BH,BL
	ADD	ZL,AL
	ADC	ZH,AH
	ADC	BH,BL
	OUT	RAMPZ,BH
	LDI	YH,high(KOREAN_BUFFER)	; OR joongsung to Korean buffer
	LDI	YL,low(KOREAN_BUFFER)
	LDI	BL,32
GLCD_BUF2:ELPM	AL,Z+
	LD	AH,Y
	OR	AL,AH
	ST	Y+,AL
	DEC	BL
	BRNE	GLCD_BUF2

	LDS	AL,JONG_FLAG		; have jongsung ?
	CPI	AL,1
	BRNE	GLCD_DSP
	CLR	AH			; jongsung offset
	LDS	AL,JONG_BUL		;  (bul * 28 * 32)
	LDI	BH,high(896)
	LDI	BL,low(896)
	CALL	MUL16
	LDS	BL,JONG_5BIT
	LDI	BH,32
	MUL	BL,BH
	LDI	BH,byte3(JONG_FONT0 << 1)
	LDI	ZH,high(JONG_FONT0 << 1)
	LDI	ZL,low(JONG_FONT0 << 1)
	ADD	ZL,R0
	ADC	ZH,R1
	CLR	BL
	ADC	BH,BL
	ADD	ZL,AL
	ADC	ZH,AH
	ADC	BH,BL
	OUT	RAMPZ,BH
	LDI	YH,high(KOREAN_BUFFER)	; OR jongsung to Korean buffer
	LDI	YL,low(KOREAN_BUFFER)
	LDI	BL,32
GLCD_BUF3:ELPM	AL,Z+
	LD	AH,Y
	OR	AL,AH
	ST	Y+,AL
	DEC	BL
	BRNE	GLCD_BUF3

;------------------------------------------------
;	Display Korean Character
;------------------------------------------------
GLCD_DSP:CALL	GLCD_XY			; upper left row of 32-byte font
	LDI	YH,high(KOREAN_BUFFER)	; start address of character font
	LDI	YL,low(KOREAN_BUFFER)
	LDI	BL,8
	LDS	AL,Ycharacter		; CS1 or CS2 ?
	CPI	AL,8
	BRSH	GLCD_DSP1
	LDI	AL,0b01000000
	RJMP	GLCD_DSP2
GLCD_DSP1:LDI	AL,0b10000000
GLCD_DSP2:LD	LCD_BUFFER,Y+
	CALL	GLCD_DATA
	DEC	BL
	BRNE	GLCD_DSP2

	CALL	GLCD_XY1		; lower left row of 32-byte font
	LDI	YH,high(KOREAN_BUFFER)	; start address of character font
	LDI	YL,low(KOREAN_BUFFER)
	ADIW	YH:YL,16
	LDI	BL,8
	LDS	AL,Ycharacter		; CS1 or CS2 ?
	CPI	AL,8
	BRSH	GLCD_DSP3
	LDI	AL,0b01000000
	RJMP	GLCD_DSP4
GLCD_DSP3:LDI	AL,0b10000000
GLCD_DSP4:LD	LCD_BUFFER,Y+
	LDS	AH,CURSOR_FLAG		; cursor on ?
	CPI	AH,0
	BREQ	GLCD_DSP5
	LDS	AH,Xcursor		; Xcharacter = Xcursor ?
	LDS	BH,Xcharacter
	CP	AH,BH
	BRNE	GLCD_DSP5
	LDS	AH,Ycursor		; Ycharacter = Ycursor ?
	LDS	BH,Ycharacter
	CP	AH,BH
	BRNE	GLCD_DSP5
	ORI	LCD_BUFFER,0b10000000	; if yes, display cursor
GLCD_DSP5:CALL	GLCD_DATA
	DEC	BL
	BRNE	GLCD_DSP4

	LDS	AL,Ycharacter		; go next half character position
	INC	AL
	STS	Ycharacter,AL

	CALL	GLCD_XY			; upper right row of 32-byte font
	LDI	YH,high(KOREAN_BUFFER)	; start address of character font
	LDI	YL,low(KOREAN_BUFFER)
	ADIW	YH:YL,8
	LDI	BL,8
	LDS	AL,Ycharacter		; CS1 or CS2 ?
	CPI	AL,8
	BRSH	GLCD_DSP6
	LDI	AL,0b01000000
	RJMP	GLCD_DSP7
GLCD_DSP6:LDI	AL,0b10000000
GLCD_DSP7:LD	LCD_BUFFER,Y+
	CALL	GLCD_DATA
	DEC	BL
	BRNE	GLCD_DSP7

	CALL	GLCD_XY1		; lower right row of 32-byte font
	LDI	YH,high(KOREAN_BUFFER)	; start address of character font
	LDI	YL,low(KOREAN_BUFFER)
	ADIW	YH:YL,24
	LDI	BL,8
	LDS	AL,Ycharacter		; CS1 or CS2 ?
	CPI	AL,8
	BRSH	GLCD_DSP8
	LDI	AL,0b01000000
	RJMP	GLCD_DSP9
GLCD_DSP8:LDI	AL,0b10000000
GLCD_DSP9:LD	LCD_BUFFER,Y+
	LDS	AH,CURSOR_FLAG		; cursor on ?
	CPI	AH,0
	BREQ	GLCD_DSP0
	LDS	AH,Xcursor		; Xcharacter = Xcursor ?
	LDS	BH,Xcharacter
	CP	AH,BH
	BRNE	GLCD_DSP0
	LDS	AH,Ycursor		; Ycharacter = Ycursor ?
	INC	AH
	LDS	BH,Ycharacter
	CP	AH,BH
	BRNE	GLCD_DSP0
	ORI	LCD_BUFFER,0b10000000	; if yes, display cursor
GLCD_DSP0:CALL	GLCD_DATA
	DEC	BL
	BRNE	GLCD_DSP9

	LDS	AL,Ycharacter		; go next character position
	INC	AL
	CPI	AL,16
	BRNE	GLCD_KOR1
	LDS	AL,Xcharacter
	INC	AL
	STS	Xcharacter,AL
	CLR	AL
GLCD_KOR1:STS	Ycharacter,AL

	POP	ZH			; restore registers
	POP	ZL
	POP	YH
	POP	YL
	POP	BH
	POP	BL
	POP	AH
	POP	AL
	POP	R1
	POP	R0
	RET

;------------------------------------------------
;	Display 4 Digit HEX Data(AH:AL)
;------------------------------------------------
GLCD_4HEX:MOV	LCD_BUFFER,AH		; high byte
	RCALL	GLCD_2HEX
	MOV	LCD_BUFFER,AL		; low byte
	RCALL	GLCD_2HEX
	RET

;------------------------------------------------
;	Display 2 Digit HEX Data(LCD_BUFFER)
;------------------------------------------------
GLCD_2HEX:PUSH	LCD_BUFFER
	LSR	LCD_BUFFER		; upper digit
	LSR	LCD_BUFFER
	LSR	LCD_BUFFER
	LSR	LCD_BUFFER
	CPI	LCD_BUFFER,0x0A
	BRSH	GLCD_2H1
	ORI	LCD_BUFFER,'0'
	RJMP	GLCD_2H2
GLCD_2H1:SUBI	LCD_BUFFER,0x0A
	PUSH	AL
	LDI	AL,'A'
	ADD	LCD_BUFFER,AL
	POP	AL
GLCD_2H2:CALL	GLCD_ENGLISH
	POP	LCD_BUFFER		; lower digit
	ANDI	LCD_BUFFER,0x0F
	CPI	LCD_BUFFER,0x0A
	BRSH	GLCD_2H3
	ORI	LCD_BUFFER,'0'
	RJMP	GLCD_2H4
GLCD_2H3:SUBI	LCD_BUFFER,0x0A
	PUSH	AL
	LDI	AL,'A'
	ADD	LCD_BUFFER,AL
	POP	AL
GLCD_2H4:CALL	GLCD_ENGLISH
	RET

;------------------------------------------------
;	Display 5/4/3-Digit Decimal Number(AH:AL)
;	Display     2-Digit Decimal Number(AL)
;------------------------------------------------
GLCD_5DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
	CLR	BH
	CLR	BL
	LDI	DH,high(10000)		; display 10^4 digit
	LDI	DL,low(10000)
	CALL	DIV16
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CPI	LCD_BUFFER,'0'
	BRNE	GLCD_5D1
	LDI	LCD_BUFFER,' '
	RJMP	GLCD_5D2
GLCD_5D1:CLR	ZERO_FLAG
	COM	ZERO_FLAG
GLCD_5D2:CALL	GLCD_ENGLISH
	MOVW	AH:AL,BH:BL
	RJMP	GLCD_4D1

GLCD_4DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
GLCD_4D1:CLR	BH
	CLR	BL
	LDI	DH,high(1000)		; display 10^3 digit
	LDI	DL,low(1000)
	CALL	DIV16
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CLR	AH
	CP	ZERO_FLAG,AH
	BRNE	GLCD_4D3
	CPI	LCD_BUFFER,'0'
	BRNE	GLCD_4D2
	LDI	LCD_BUFFER,' '
	RJMP	GLCD_4D3
GLCD_4D2:CLR	ZERO_FLAG
	COM	ZERO_FLAG
GLCD_4D3:CALL	GLCD_ENGLISH
	MOVW	AH:AL,BH:BL
	JMP	GLCD_3D1

GLCD_3DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
GLCD_3D1:LDI	BL,100			; display 10^2 digit
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CLR	BH
	CP	ZERO_FLAG,BH
	BRNE	GLCD_3D3
	CPI	LCD_BUFFER,'0'
	BRNE	GLCD_3D2
	LDI	LCD_BUFFER,' '
	RJMP	GLCD_3D3
GLCD_3D2:CLR	ZERO_FLAG
	COM	ZERO_FLAG
GLCD_3D3:CALL	GLCD_ENGLISH
	MOV	AL,AH
	JMP	GLCD_2D1

GLCD_2DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
GLCD_2D1:CLR	AH
	LDI	BL,10			; display 10^1 digit
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CLR	BH
	CP	ZERO_FLAG,BH
	BRNE	GLCD_2D2
	CPI	LCD_BUFFER,'0'
	BRNE	GLCD_2D2
	LDI	LCD_BUFFER,' '
GLCD_2D2:CALL	GLCD_ENGLISH
	MOV	LCD_BUFFER,AH		; display 10^0 digit
	ORI	LCD_BUFFER,'0'
	CALL	GLCD_ENGLISH
	POP	BL
	POP	BH
	POP	DL
	POP	DH
	RET

;------------------------------------------------
;	Display 8-Bit Binary Data(LCD_BUFFER)
;------------------------------------------------
GLCD_8BIN:PUSH	AH
	PUSH	AL
	LDI	AH,8
	MOV	AL,LCD_BUFFER
GLCD_8B1:ROL	AL
	MOV	LCD_BUFFER,AL
	ROL	LCD_BUFFER
	ANDI	LCD_BUFFER,0b00000001
	ORI	LCD_BUFFER,'0'
	CALL	GLCD_ENGLISH
	DEC	AH
	BRNE	GLCD_8B1
	POP	AL
	POP	AH
	RET

.nolist
;====================================================================
;	8x16 Dot English(ASCII) Character Font Data
;====================================================================
E_FONT:	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; 00H
	.db $F8,$04,$14,$04,$04,$14,$04,$F8,$07,$08,$09,$0B,$0B,$09,$08,$07 ; 01H
	.db $F8,$FC,$6C,$7C,$7C,$6C,$FC,$F8,$07,$0F,$0F,$0C,$0C,$0F,$0F,$07 ; 02H
	.db $00,$F0,$F8,$F8,$F0,$F8,$F8,$F0,$00,$00,$01,$03,$07,$03,$01,$00 ; 03H
	.db $00,$40,$E0,$F0,$F8,$F0,$E0,$40,$00,$00,$00,$01,$03,$01,$00,$00 ; 04H
	.db $E0,$E0,$F8,$1C,$1C,$F8,$E0,$E0,$00,$00,$04,$07,$07,$04,$00,$00 ; 05H
	.db $60,$F0,$F8,$FC,$FC,$F8,$F0,$60,$00,$00,$04,$07,$07,$04,$00,$00 ; 06H
	.db $00,$00,$60,$F0,$F0,$60,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; 07H
	.db $00,$FF,$1F,$0F,$0F,$0F,$1F,$FF,$00,$FF,$FF,$FE,$FE,$FE,$FF,$FF ; 08H
	.db $00,$00,$E0,$10,$10,$10,$E0,$00,$00,$00,$00,$01,$01,$01,$00,$00 ; 09H
	.db $00,$FF,$1F,$EF,$EF,$EF,$1F,$FF,$00,$FF,$FF,$FE,$FE,$FE,$FF,$FF ; 0AH
	.db $00,$00,$90,$48,$7C,$48,$90,$00,$00,$03,$04,$08,$08,$08,$04,$03 ; 0BH
	.db $00,$30,$48,$84,$84,$84,$48,$30,$00,$02,$02,$02,$0F,$02,$02,$02 ; 0CH
	.db $00,$00,$00,$FC,$04,$08,$70,$00,$00,$06,$0F,$07,$00,$00,$00,$00 ; 0DH
	.db $00,$00,$00,$FC,$14,$14,$14,$FC,$00,$0C,$1E,$0F,$00,$06,$0F,$07 ; 0EH
	.db $50,$50,$E0,$BC,$BC,$E0,$50,$50,$01,$01,$00,$07,$07,$00,$01,$01 ; 0FH
	.db $00,$00,$FC,$F8,$F0,$E0,$40,$00,$00,$00,$07,$03,$01,$00,$00,$00 ; 10H
	.db $00,$00,$40,$E0,$F0,$F8,$FC,$00,$00,$00,$00,$00,$01,$03,$07,$00 ; 11H
	.db $00,$10,$18,$FC,$FC,$18,$10,$00,$00,$01,$03,$07,$07,$03,$01,$00 ; 12H
	.db $00,$00,$FC,$FC,$00,$00,$FC,$FC,$00,$00,$04,$04,$00,$00,$04,$04 ; 13H
	.db $38,$7C,$44,$FC,$FC,$04,$FC,$FC,$00,$00,$00,$07,$07,$00,$07,$07 ; 14H
	.db $00,$C4,$EE,$3A,$12,$32,$E6,$C4,$00,$08,$19,$13,$12,$17,$1D,$08 ; 15H
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$0E,$0E,$0E,$0E,$0E,$0E,$00 ; 16H
	.db $00,$10,$18,$FC,$FC,$18,$10,$00,$08,$09,$0B,$0F,$0F,$0B,$09,$08 ; 17H
	.db $00,$10,$18,$FC,$FC,$18,$10,$00,$00,$00,$00,$07,$07,$00,$00,$00 ; 18H
	.db $00,$00,$00,$FC,$FC,$00,$00,$00,$00,$01,$03,$07,$07,$03,$01,$00 ; 19H
	.db $00,$40,$40,$40,$50,$F0,$E0,$40,$00,$00,$00,$00,$01,$01,$00,$00 ; 1AH
	.db $00,$40,$E0,$F0,$50,$40,$40,$40,$00,$00,$00,$01,$01,$00,$00,$00 ; 1BH
	.db $F8,$00,$00,$00,$00,$00,$00,$00,$03,$02,$02,$02,$02,$02,$02,$02 ; 1CH
	.db $40,$E0,$50,$40,$40,$50,$E0,$40,$00,$00,$01,$00,$00,$01,$00,$00 ; 1DH
	.db $00,$00,$00,$C0,$F0,$C0,$00,$00,$00,$04,$07,$07,$07,$07,$07,$04 ; 1EH
	.db $00,$10,$70,$F0,$F0,$F0,$70,$10,$00,$00,$00,$01,$07,$01,$00,$00 ; 1FH
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; space
	.db $00,$00,$38,$FC,$FC,$38,$00,$00,$00,$00,$00,$0D,$0D,$00,$00,$00 ; !
	.db $00,$0E,$1E,$00,$00,$1E,$0E,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; "
	.db $20,$F8,$F8,$20,$F8,$F8,$20,$00,$02,$0F,$0F,$02,$0F,$0F,$02,$00 ; #
	.db $38,$7C,$44,$47,$47,$CC,$98,$00,$06,$0C,$08,$38,$38,$0F,$07,$00 ; $
	.db $30,$30,$00,$80,$C0,$60,$30,$00,$0C,$06,$03,$01,$00,$0C,$0C,$00 ; %
	.db $80,$D8,$7C,$E4,$BC,$D8,$40,$00,$07,$0F,$08,$08,$07,$0F,$08,$00 ; &
	.db $00,$10,$1E,$0E,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; '
	.db $00,$00,$F0,$F8,$0C,$04,$00,$00,$00,$00,$03,$07,$0C,$08,$00,$00 ; (
	.db $00,$00,$04,$0C,$F8,$F0,$00,$00,$00,$00,$08,$0C,$07,$03,$00,$00 ; )
	.db $80,$A0,$E0,$C0,$C0,$E0,$A0,$80,$00,$02,$03,$01,$01,$03,$02,$00 ; *
	.db $00,$80,$80,$E0,$E0,$80,$80,$00,$00,$00,$00,$03,$03,$00,$00,$00 ; +
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$10,$1E,$0E,$00,$00,$00 ; ,
	.db $80,$80,$80,$80,$80,$80,$80,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; -
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$0C,$0C,$00,$00,$00 ; .
	.db $00,$00,$00,$80,$C0,$60,$30,$00,$0C,$06,$03,$01,$00,$00,$00,$00 ; /
	.db $F0,$F8,$0C,$C4,$0C,$F8,$F0,$00,$03,$07,$0C,$08,$0C,$07,$03,$00 ; 0
	.db $00,$10,$18,$FC,$FC,$00,$00,$00,$00,$08,$08,$0F,$0F,$08,$08,$00 ; 1
	.db $08,$0C,$84,$C4,$64,$3C,$18,$00,$0E,$0F,$09,$08,$08,$0C,$0C,$00 ; 2
	.db $08,$0C,$44,$44,$44,$FC,$B8,$00,$04,$0C,$08,$08,$08,$0F,$07,$00 ; 3
	.db $C0,$E0,$B0,$98,$FC,$FC,$80,$00,$00,$00,$00,$08,$0F,$0F,$08,$00 ; 4
	.db $7C,$7C,$44,$44,$44,$C4,$84,$00,$04,$0C,$08,$08,$08,$0F,$07,$00 ; 5
	.db $F0,$F8,$4C,$44,$44,$C0,$80,$00,$07,$0F,$08,$08,$08,$0F,$07,$00 ; 6
	.db $0C,$0C,$04,$84,$C4,$7C,$3C,$00,$00,$00,$0F,$0F,$00,$00,$00,$00 ; 7
	.db $B8,$FC,$44,$44,$44,$FC,$B8,$00,$07,$0F,$08,$08,$08,$0F,$07,$00 ; 8
	.db $38,$7C,$44,$44,$44,$FC,$F8,$00,$00,$08,$08,$08,$0C,$07,$03,$00 ; 9
	.db $00,$00,$00,$30,$30,$00,$00,$00,$00,$00,$00,$06,$06,$00,$00,$00 ; :
	.db $00,$00,$00,$30,$30,$00,$00,$00,$00,$00,$08,$0E,$06,$00,$00,$00 ; ;
	.db $00,$80,$C0,$60,$30,$18,$08,$00,$00,$00,$01,$03,$06,$0C,$08,$00 ; <
	.db $00,$20,$20,$20,$20,$20,$20,$00,$00,$01,$01,$01,$01,$01,$01,$00 ; =
	.db $00,$08,$18,$30,$60,$C0,$80,$00,$00,$08,$0C,$06,$03,$01,$00,$00 ; >
	.db $18,$1C,$04,$C4,$E4,$3C,$18,$00,$00,$00,$00,$0D,$0D,$00,$00,$00 ; ?
	.db $F0,$F8,$08,$C8,$C8,$F8,$F0,$00,$07,$0F,$08,$0B,$0B,$0B,$01,$00 ; @
	.db $E0,$F0,$98,$8C,$98,$F0,$E0,$00,$0F,$0F,$00,$00,$00,$0F,$0F,$00 ; A
	.db $04,$FC,$FC,$44,$44,$FC,$B8,$00,$08,$0F,$0F,$08,$08,$0F,$07,$00 ; B
	.db $F0,$F8,$0C,$04,$04,$0C,$18,$00,$03,$07,$0C,$08,$08,$0C,$06,$00 ; C
	.db $04,$FC,$FC,$04,$0C,$F8,$F0,$00,$08,$0F,$0F,$08,$0C,$07,$03,$00 ; D
	.db $04,$FC,$FC,$44,$E4,$0C,$1C,$00,$08,$0F,$0F,$08,$08,$0C,$0E,$00 ; E
	.db $04,$FC,$FC,$44,$E4,$0C,$1C,$00,$08,$0F,$0F,$08,$00,$00,$00,$00 ; F
	.db $F0,$F8,$0C,$84,$84,$8C,$98,$00,$03,$07,$0C,$08,$08,$07,$0F,$00 ; G
	.db $FC,$FC,$40,$40,$40,$FC,$FC,$00,$0F,$0F,$00,$00,$00,$0F,$0F,$00 ; H
	.db $00,$00,$04,$FC,$FC,$04,$00,$00,$00,$00,$08,$0F,$0F,$08,$00,$00 ; I
	.db $00,$00,$00,$04,$FC,$FC,$04,$00,$07,$0F,$08,$08,$0F,$07,$00,$00 ; J
	.db $04,$FC,$FC,$C0,$E0,$3C,$1C,$00,$08,$0F,$0F,$00,$01,$0F,$0E,$00 ; K
	.db $04,$FC,$FC,$04,$00,$00,$00,$00,$08,$0F,$0F,$08,$08,$0C,$0E,$00 ; L
	.db $FC,$FC,$38,$70,$38,$FC,$FC,$00,$0F,$0F,$00,$00,$00,$0F,$0F,$00 ; M
	.db $FC,$FC,$38,$70,$E0,$FC,$FC,$00,$0F,$0F,$00,$00,$00,$0F,$0F,$00 ; N
	.db $F8,$FC,$04,$04,$04,$FC,$F8,$00,$07,$0F,$08,$08,$08,$0F,$07,$00 ; O
	.db $04,$FC,$FC,$44,$44,$7C,$38,$00,$08,$0F,$0F,$08,$00,$00,$00,$00 ; P
	.db $F8,$FC,$04,$04,$04,$FC,$F8,$00,$07,$0F,$08,$0E,$3C,$3F,$27,$00 ; Q
	.db $04,$FC,$FC,$44,$C4,$FC,$38,$00,$08,$0F,$0F,$00,$00,$0F,$0F,$00 ; R
	.db $18,$3C,$64,$44,$C4,$9C,$18,$00,$06,$0E,$08,$08,$08,$0F,$07,$00 ; S
	.db $00,$1C,$0C,$FC,$FC,$0C,$1C,$00,$00,$00,$08,$0F,$0F,$08,$00,$00 ; T
	.db $FC,$FC,$00,$00,$00,$FC,$FC,$00,$07,$0F,$08,$08,$08,$0F,$07,$00 ; U
	.db $FC,$FC,$00,$00,$00,$FC,$FC,$00,$01,$03,$06,$0C,$06,$03,$01,$00 ; V
	.db $FC,$FC,$00,$C0,$00,$FC,$FC,$00,$07,$0F,$0E,$03,$0E,$0F,$07,$00 ; W
	.db $0C,$3C,$F0,$E0,$F0,$3C,$0C,$00,$0C,$0F,$03,$01,$03,$0F,$0C,$00 ; X
	.db $00,$3C,$7C,$C0,$C0,$7C,$3C,$00,$00,$00,$08,$0F,$0F,$08,$00,$00 ; Y
	.db $1C,$0C,$84,$C4,$64,$3C,$1C,$00,$0E,$0F,$09,$08,$08,$0C,$0E,$00 ; Z
	.db $00,$00,$FC,$FC,$04,$04,$00,$00,$00,$00,$0F,$0F,$08,$08,$00,$00 ; [
	.db $38,$70,$E0,$C0,$80,$00,$00,$00,$00,$00,$00,$01,$03,$07,$0E,$00 ; \
	.db $00,$00,$04,$04,$FC,$FC,$00,$00,$00,$00,$08,$08,$0F,$0F,$00,$00 ; ]
	.db $08,$0C,$06,$03,$06,$0C,$08,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; ^
	.db $00,$00,$00,$00,$00,$00,$00,$00,$20,$20,$20,$20,$20,$20,$20,$20 ; _
	.db $00,$00,$03,$07,$04,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; `
	.db $00,$A0,$A0,$A0,$E0,$C0,$00,$00,$07,$0F,$08,$08,$07,$0F,$08,$00 ; a
	.db $04,$FC,$FC,$20,$60,$C0,$80,$00,$00,$0F,$0F,$08,$08,$0F,$07,$00 ; b
	.db $C0,$E0,$20,$20,$20,$60,$40,$00,$07,$0F,$08,$08,$08,$0C,$04,$00 ; c
	.db $80,$C0,$60,$24,$FC,$FC,$00,$00,$07,$0F,$08,$08,$07,$0F,$08,$00 ; d
	.db $C0,$E0,$A0,$A0,$A0,$E0,$C0,$00,$07,$0F,$08,$08,$08,$0C,$04,$00 ; e
	.db $40,$F8,$FC,$44,$0C,$18,$00,$00,$08,$0F,$0F,$08,$00,$00,$00,$00 ; f
	.db $C0,$E0,$20,$20,$C0,$E0,$20,$00,$27,$6F,$48,$48,$7F,$3F,$00,$00 ; g
	.db $04,$FC,$FC,$40,$20,$E0,$C0,$00,$08,$0F,$0F,$00,$00,$0F,$0F,$00 ; h
	.db $00,$00,$20,$EC,$EC,$00,$00,$00,$00,$00,$08,$0F,$0F,$08,$00,$00 ; i
	.db $00,$00,$00,$00,$20,$EC,$EC,$00,$00,$30,$70,$40,$40,$7F,$3F,$00 ; j
	.db $04,$FC,$FC,$80,$C0,$60,$20,$00,$08,$0F,$0F,$01,$03,$0E,$0C,$00 ; k
	.db $00,$00,$04,$FC,$FC,$00,$00,$00,$00,$00,$08,$0F,$0F,$08,$00,$00 ; l
	.db $E0,$E0,$60,$C0,$60,$E0,$C0,$00,$0F,$0F,$00,$07,$00,$0F,$0F,$00 ; m
	.db $20,$E0,$C0,$20,$20,$E0,$C0,$00,$00,$0F,$0F,$00,$00,$0F,$0F,$00 ; n
	.db $C0,$E0,$20,$20,$20,$E0,$C0,$00,$07,$0F,$08,$08,$08,$0F,$07,$00 ; o
	.db $20,$E0,$C0,$20,$20,$E0,$C0,$00,$40,$7F,$7F,$48,$08,$0F,$07,$00 ; p
	.db $C0,$E0,$20,$20,$C0,$E0,$20,$00,$07,$0F,$08,$48,$7F,$7F,$40,$00 ; q
	.db $20,$E0,$C0,$60,$20,$E0,$C0,$00,$08,$0F,$0F,$08,$00,$00,$00,$00 ; r
	.db $40,$E0,$A0,$20,$20,$60,$40,$00,$04,$0C,$09,$09,$0B,$0E,$04,$00 ; s
	.db $20,$20,$F8,$FC,$20,$20,$00,$00,$00,$00,$07,$0F,$08,$0C,$04,$00 ; t
	.db $E0,$E0,$00,$00,$E0,$E0,$00,$00,$07,$0F,$08,$08,$07,$0F,$08,$00 ; u
	.db $00,$E0,$E0,$00,$00,$E0,$E0,$00,$00,$03,$07,$0C,$0C,$07,$03,$00 ; v
	.db $E0,$E0,$00,$80,$00,$E0,$E0,$00,$07,$0F,$0C,$07,$0C,$0F,$07,$00 ; w
	.db $20,$60,$C0,$80,$C0,$60,$20,$00,$08,$0C,$07,$03,$07,$0C,$08,$00 ; x
	.db $E0,$E0,$00,$00,$00,$E0,$E0,$00,$47,$4F,$48,$48,$68,$3F,$1F,$00 ; y
	.db $60,$60,$20,$A0,$E0,$60,$20,$00,$0C,$0E,$0B,$09,$08,$0C,$0C,$00 ; z
	.db $00,$40,$40,$F8,$BC,$04,$04,$00,$00,$00,$00,$07,$0F,$08,$08,$00 ; {
	.db $00,$00,$00,$BC,$BC,$00,$00,$00,$00,$00,$00,$0F,$0F,$00,$00,$00 ; |
	.db $00,$04,$04,$BC,$F8,$40,$40,$00,$00,$08,$08,$0F,$07,$00,$00,$00 ; }
	.db $80,$C0,$40,$C0,$80,$C0,$40,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; ~
	.db $80,$C0,$60,$30,$60,$C0,$80,$00,$07,$07,$04,$04,$04,$07,$07,$00 ; 7FH

;====================================================================
;	Font Processing Table for Korean Character
;====================================================================
TBL_CHO:  .db 0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,0
TBL_JOONG:.db 0,0,0,1,2,3,4,5,0,0,6,7,8,9,10,11,0,0,12,13,14,15,16,17,0,0,18,19,20,21
TBL_JONG: .db 0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0,17,18,19,20,21,22,23,24,25,26,27

BUL_CHO1: .db 0,0,0,0,0,0,0,0,0,1,3,3,3,1,2,4,4,4,2,1,3,0
BUL_CHO2: .db 0,5,5,5,5,5,5,5,5,6,7,7,7,6,6,7,7,7,6,6,7,5
BUL_JONG: .db 0,0,2,0,2,1,2,1,2,3,0,2,1,3,3,1,2,1,3,3,1,1

;====================================================================
;	16x16 Dot Korean Character Font Data
;====================================================================
CHO_FONT0:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$08,$08,$08,$88,$C8,$78,$38,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$02,$02,$01,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$C8,$F8,$38,$C8,$F8,$38,$00,$00,$00,$00,$00,$00,$00
	.db $00,$02,$01,$01,$02,$01,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$F8,$F8,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$03,$02,$02,$02,$02,$02,$02,$00,$00,$00,$00,$00,$00
	.db $00,$08,$F8,$F8,$08,$08,$08,$08,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$03,$02,$02,$02,$02,$02,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$F8,$F8,$08,$F8,$F8,$08,$08,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$03,$02,$01,$03,$02,$02,$02,$00,$00,$00,$00,$00,$00
	.db $00,$00,$C8,$C8,$48,$48,$48,$78,$78,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$03,$02,$02,$02,$02,$02,$02,$00,$00,$00,$00,$00,$00
	.db $00,$08,$F8,$F8,$08,$08,$08,$F8,$F8,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$03,$03,$02,$02,$02,$03,$03,$00,$00,$00,$00,$00,$00,$00
	.db $00,$10,$F0,$F0,$40,$40,$48,$F8,$F8,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$03,$03,$02,$02,$02,$03,$03,$00,$00,$00,$00,$00,$00,$00
	.db $10,$F0,$F0,$40,$F0,$F0,$48,$F8,$F8,$00,$00,$00,$00,$00,$00,$00
	.db $00,$03,$03,$02,$03,$03,$02,$03,$03,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$C4,$7C,$FC,$80,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$02,$02,$01,$01,$00,$00,$01,$03,$02,$00,$00,$00,$00,$00,$00
	.db $00,$00,$F8,$78,$80,$80,$78,$F8,$00,$00,$00,$00,$00,$00,$00,$00
	.db $02,$03,$01,$00,$03,$03,$00,$01,$03,$02,$00,$00,$00,$00,$00,$00
	.db $00,$E0,$F0,$18,$08,$08,$18,$F0,$E0,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$03,$02,$02,$03,$01,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$08,$88,$C8,$78,$38,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$02,$02,$01,$01,$00,$01,$03,$02,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$88,$C8,$F8,$B8,$C8,$F8,$38,$00,$00,$00,$00,$00,$00,$00
	.db $02,$03,$01,$02,$03,$01,$00,$01,$03,$02,$00,$00,$00,$00,$00,$00
	.db $00,$10,$10,$14,$94,$D4,$F4,$30,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$02,$02,$01,$01,$00,$01,$03,$02,$00,$00,$00,$00,$00,$00,$00
	.db $00,$28,$28,$28,$28,$A8,$E8,$78,$38,$00,$00,$00,$00,$00,$00,$00
	.db $00,$02,$02,$02,$01,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$F8,$F8,$48,$48,$48,$48,$48,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$03,$02,$02,$02,$02,$02,$02,$00,$00,$00,$00,$00,$00
	.db $08,$08,$F8,$F8,$08,$08,$F8,$F8,$08,$08,$00,$00,$00,$00,$00,$00
	.db $02,$02,$03,$03,$02,$02,$03,$03,$02,$02,$00,$00,$00,$00,$00,$00
	.db $00,$08,$C8,$E8,$2C,$2C,$2C,$E8,$C8,$08,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$03,$02,$02,$02,$03,$01,$00,$00,$00,$00,$00,$00,$00
CHO_FONT1:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$04,$04,$04,$04,$FC,$7C,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$FC,$7C,$04,$04,$04,$FC,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$40,$40,$40,$40,$40,$40,$40,$40,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$44,$44,$44,$44,$44,$44,$44,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$44,$44,$44,$3C,$7C,$44,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$54,$54,$54,$54,$54,$5C,$5C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$44,$44,$44,$44,$44,$7C,$7C,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$50,$50,$50,$50,$54,$7C,$7C,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$50,$50,$7C,$7C,$50,$50,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$80,$80,$40,$60,$34,$1C,$1C,$10,$20,$60,$C0,$80,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$80,$C0,$60,$3C,$1C,$B0,$C0,$60,$3C,$3C,$60,$C0,$80,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$38,$7C,$44,$44,$44,$44,$44,$7C,$38,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$80,$84,$44,$44,$24,$24,$14,$3C,$2C,$44,$C0,$80,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$44,$64,$34,$1C,$1C,$64,$64,$1C,$1C,$34,$64,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$80,$88,$48,$48,$6A,$3A,$1A,$3A,$28,$48,$C8,$80,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$24,$24,$24,$24,$24,$24,$24,$24,$FC,$FC,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$01,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$54,$54,$54,$54,$54,$54,$54,$54,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$44,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$34,$7C,$4E,$4E,$4E,$4E,$7C,$34,$04,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
CHO_FONT2:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$04,$04,$04,$04,$FC,$7C,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$7C,$3C,$04,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$40,$40,$40,$40,$40,$40,$40,$40,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$44,$44,$44,$44,$44,$44,$44,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$44,$44,$44,$3C,$7C,$44,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$54,$54,$54,$54,$54,$5C,$5C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$44,$44,$44,$44,$44,$7C,$7C,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$50,$50,$50,$50,$54,$7C,$7C,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$50,$50,$7C,$7C,$50,$50,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$40,$40,$20,$30,$1A,$0E,$0E,$08,$10,$30,$60,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$60,$30,$1E,$0E,$18,$60,$18,$0E,$1E,$30,$60,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$1C,$3E,$22,$22,$22,$22,$3E,$1C,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$40,$42,$22,$22,$12,$12,$0A,$1E,$16,$22,$60,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$44,$64,$34,$1C,$1C,$64,$64,$1C,$1C,$34,$64,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$40,$44,$24,$24,$36,$1E,$0E,$1E,$14,$24,$64,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$24,$24,$24,$24,$24,$24,$24,$24,$FC,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$54,$54,$54,$54,$54,$54,$54,$54,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$44,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$34,$7C,$4E,$4E,$4E,$4E,$7C,$34,$04,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
CHO_FONT3:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$04,$04,$04,$04,$FC,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$04,$FC,$7C,$04,$04,$FC,$7C,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$40,$40,$40,$40,$40,$40,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$44,$44,$44,$44,$44,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$44,$44,$3C,$7C,$44,$44,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$34,$74,$54,$54,$54,$5C,$5C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$44,$44,$44,$7C,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$50,$50,$54,$7C,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$50,$7C,$7C,$50,$7C,$7C,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$80,$40,$60,$34,$1C,$1C,$20,$60,$C0,$80,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$80,$C0,$7C,$3C,$E0,$E0,$3C,$7C,$C0,$80,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$38,$7C,$44,$44,$44,$7C,$38,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$44,$44,$24,$34,$1C,$2C,$64,$40,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$64,$34,$3C,$6C,$34,$3C,$6C,$44,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$88,$88,$4C,$6C,$3C,$58,$C8,$80,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$14,$14,$14,$14,$14,$14,$FC,$7C,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$54,$54,$54,$54,$54,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$34,$7C,$4E,$4E,$4E,$7C,$34,$04,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
CHO_FONT4:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$04,$04,$04,$04,$FC,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$04,$7C,$3C,$04,$04,$7C,$3C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$40,$40,$40,$40,$40,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$44,$44,$44,$44,$44,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$44,$44,$3C,$7C,$44,$44,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$34,$74,$54,$54,$54,$5C,$5C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$44,$44,$44,$7C,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$50,$50,$54,$7C,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$50,$7C,$7C,$50,$7C,$7C,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$20,$30,$1A,$0E,$0E,$10,$30,$60,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$60,$3C,$1C,$70,$70,$1C,$3C,$60,$40,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$38,$7C,$44,$44,$44,$7C,$38,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$44,$24,$24,$14,$1C,$2C,$64,$40,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$44,$64,$34,$3C,$6C,$74,$3C,$6C,$44,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$44,$44,$24,$26,$16,$1E,$2C,$64,$40,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$14,$14,$14,$14,$14,$FC,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$54,$54,$54,$54,$54,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$34,$7C,$4E,$4E,$4E,$7C,$34,$04,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
CHO_FONT5:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$04,$84,$C4,$64,$3C,$1C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$84,$E4,$7C,$9C,$E4,$7C,$1C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$00,$00,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$FC,$80,$80,$80,$80,$80,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$FC,$84,$84,$84,$84,$80,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$FC,$84,$7C,$FC,$84,$84,$80,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$E4,$E4,$24,$24,$24,$3C,$3C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$01,$01,$01,$01,$01,$01,$01,$00,$00,$00,$00,$00,$00
	.db $00,$04,$FC,$FC,$84,$84,$84,$FC,$FC,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$FC,$FC,$90,$90,$94,$FC,$FC,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $04,$FC,$FC,$90,$FC,$FC,$90,$FC,$FC,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$80,$E4,$3C,$7C,$C0,$80,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$00,$00,$00,$00,$00,$01,$01,$00,$00,$00,$00,$00,$00
	.db $00,$80,$C0,$7C,$1C,$E0,$1C,$7C,$C0,$80,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$78,$FC,$84,$84,$84,$FC,$78,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$84,$84,$44,$64,$34,$5C,$CC,$80,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$84,$C4,$64,$FC,$DC,$64,$FC,$9C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$01,$01,$00,$00,$00,$01,$01,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$8C,$CC,$6C,$F8,$98,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$00,$00,$00,$00,$01,$01,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$14,$14,$94,$D4,$74,$3C,$1C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$01,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$FC,$FC,$24,$24,$24,$24,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$01,$01,$01,$01,$01,$01,$00,$00,$00,$00,$00,$00,$00
	.db $00,$84,$FC,$FC,$84,$84,$FC,$FC,$84,$84,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$74,$FC,$8E,$8E,$FC,$74,$04,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
CHO_FONT6:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$04,$04,$04,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$04,$04,$7C,$3C,$04,$04,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$1C,$3C,$20,$20,$20,$20,$20,$20,$20,$20,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$1C,$3C,$24,$24,$24,$24,$24,$24,$24,$24,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$1C,$3C,$24,$24,$24,$1C,$3C,$24,$24,$24,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$54,$54,$54,$54,$54,$5C,$5C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$3C,$3C,$24,$24,$24,$24,$24,$3C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$50,$50,$50,$50,$50,$54,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$50,$50,$7C,$7C,$50,$50,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$40,$40,$20,$30,$1A,$0E,$0E,$08,$10,$30,$60,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$60,$30,$1E,$0E,$58,$70,$30,$1E,$1E,$30,$60,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$38,$7C,$44,$44,$44,$44,$7C,$38,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$44,$44,$44,$24,$24,$14,$3C,$2C,$44,$40,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$44,$64,$34,$1C,$1C,$64,$64,$1C,$1C,$34,$64,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$44,$44,$24,$34,$1E,$0E,$1E,$34,$24,$44,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$14,$14,$14,$14,$14,$14,$14,$14,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$54,$54,$54,$54,$54,$54,$54,$54,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$44,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$34,$7C,$4E,$4E,$4E,$4E,$7C,$34,$04,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
CHO_FONT7:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$04,$04,$04,$04,$7C,$3C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$04,$7C,$3C,$04,$04,$7C,$3C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$40,$40,$40,$40,$40,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$44,$44,$44,$44,$44,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$44,$44,$3C,$7C,$44,$44,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$34,$74,$54,$54,$54,$5C,$5C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$44,$44,$44,$7C,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$50,$50,$54,$7C,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$7C,$7C,$50,$7C,$7C,$50,$7C,$7C,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$20,$30,$1A,$0E,$0E,$10,$30,$60,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$60,$3C,$1C,$70,$70,$1C,$3C,$60,$40,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$38,$7C,$44,$44,$44,$7C,$38,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$44,$24,$24,$14,$1C,$2C,$64,$40,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$44,$64,$34,$3C,$6C,$34,$3C,$6C,$44,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$44,$44,$24,$26,$16,$1E,$2C,$64,$40,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$14,$14,$14,$14,$14,$FC,$7C,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$3C,$7C,$54,$54,$54,$54,$54,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$04,$34,$7C,$4E,$4E,$4E,$7C,$34,$04,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
JOONG_FONT0:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$40,$40,$40
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$42,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$20,$20,$20
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$01,$01,$01
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$22,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$01,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$40,$40,$42,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$40,$44,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$A0,$A0,$A2,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$A0,$A0,$A4,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$08,$08,$08,$08,$0F,$0F,$08,$08,$08,$08,$08,$08,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$80,$80,$80
	.db $00,$04,$04,$04,$07,$07,$04,$04,$04,$04,$00,$7F,$3F,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$42,$FE,$FE,$00
	.db $00,$04,$04,$07,$07,$04,$04,$04,$04,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$04,$04,$04,$07,$07,$04,$04,$04,$04,$04,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$08,$0F,$0F,$08,$08,$09,$0F,$0F,$08,$08,$08,$08,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$7F,$3F,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$3F,$1F,$01,$01,$05,$05,$04,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$01,$01,$01,$3F,$1F,$01,$05,$05,$04,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$3F,$1F,$01,$01,$01,$01,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$7F,$3F,$01,$01,$7F,$3F,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$04,$04,$04,$04,$04,$04,$04,$02,$02,$02,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00
JOONG_FONT1:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$40,$40,$40
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$42,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$20,$20,$20
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$01,$01,$01
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$22,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$01,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$40,$40,$42,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$44,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$A0,$A0,$A2,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$A4,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$08,$08,$08,$08,$0F,$0F,$08,$08,$08,$08,$08,$08,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$80,$80,$80
	.db $00,$04,$04,$04,$07,$07,$04,$04,$04,$04,$00,$7F,$3F,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$42,$FE,$FE,$00
	.db $00,$04,$04,$04,$07,$07,$04,$04,$04,$00,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$04,$04,$04,$07,$07,$04,$04,$04,$04,$04,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$08,$08,$0F,$0F,$08,$08,$0F,$0F,$08,$08,$08,$08,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$7F,$3F,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$3F,$1F,$01,$01,$05,$05,$04,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$01,$01,$01,$1F,$0F,$01,$05,$05,$04,$3F,$1F,$00,$7F,$3F,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$3F,$1F,$01,$01,$01,$01,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$7F,$3F,$01,$01,$7F,$3F,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$04,$04,$04,$04,$04,$04,$04,$02,$02,$02,$00,$7F,$3F,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$00,$00
JOONG_FONT2:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$20,$20,$20
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$22,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$50,$50,$50
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$52,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$20,$20,$20,$22,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$20,$24,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$50,$50,$52,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$50,$54,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$C0,$C0,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$C0,$C0,$00,$00,$00,$00,$02,$FE,$FE,$20,$20,$20
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$03,$01,$00,$00,$00
	.db $00,$00,$00,$C0,$C0,$00,$00,$00,$00,$04,$FC,$FC,$22,$FE,$FE,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$00,$01,$00,$00,$03,$01,$00
	.db $00,$00,$00,$00,$C0,$C0,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$C0,$C0,$00,$00,$00,$C0,$C0,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00
	.db $00,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$80,$00
	.db $00,$00,$00,$00,$00,$00,$00,$07,$03,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$40,$40,$40,$C0,$C0,$40,$40,$40,$40,$04,$FC,$FC,$00,$00
	.db $00,$00,$00,$00,$00,$07,$03,$00,$00,$01,$01,$01,$07,$03,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$40,$44,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$01,$01,$01,$07,$03,$01,$01,$01,$00,$07,$03,$00,$07,$03,$00
	.db $00,$00,$80,$80,$80,$80,$80,$80,$80,$80,$80,$00,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$03,$03,$00,$00,$00,$00,$00,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$07,$07,$01,$01,$07,$07,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00
JOONG_FONT3:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$20,$20,$20
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$22,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$50,$50,$50
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$04,$FC,$FC,$52,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$20,$20,$22,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$24,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$50,$50,$52,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$54,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$C0,$C0,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$C0,$C0,$00,$00,$00,$00,$02,$FE,$FE,$20,$20,$20
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$03,$01,$00,$00,$00
	.db $00,$00,$00,$00,$C0,$C0,$00,$00,$00,$04,$FC,$FC,$22,$FE,$FE,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$00,$01,$00,$00,$03,$01,$00
	.db $00,$00,$00,$00,$C0,$C0,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$00,$C0,$C0,$00,$00,$C0,$C0,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$07,$07,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$80,$80,$80,$80,$80,$80,$80,$80,$80,$00,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$03,$03,$00,$00,$00,$00,$01,$01,$03,$03,$00
	.db $00,$80,$80,$80,$80,$80,$80,$80,$A0,$24,$FC,$FC,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$03,$01,$00,$00,$00,$00,$03,$01,$00,$03,$01,$00
	.db $00,$00,$80,$80,$80,$80,$80,$80,$80,$80,$80,$00,$02,$FE,$FE,$00
	.db $00,$00,$00,$00,$00,$03,$03,$00,$00,$00,$00,$00,$00,$03,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$07,$07,$01,$01,$07,$07,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$03,$01,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$02,$FE,$FE,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$03,$01,$00,$00
JONG_FONT0:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$04,$04,$04,$04,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$7C,$3C,$04,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$7C,$3C,$40,$60,$3C,$1C,$70,$60,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$3C,$7C,$40,$40,$40,$40,$40,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$3C,$7C,$40,$40,$44,$64,$3C,$3C,$64,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$3C,$7C,$40,$40,$28,$78,$5C,$5C,$78,$28,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$3C,$7C,$44,$44,$44,$44,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$34,$74,$54,$54,$54,$54,$54,$5C,$5C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$44,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$50,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$60,$3C,$3C,$60,$40,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$54,$54,$54,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$18,$44,$7C,$7C,$44,$7C,$7C,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$28,$78,$5C,$5C,$78,$28,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$7C,$7C,$44,$44,$44,$44,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$7C,$7C,$50,$50,$50,$54,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$7C,$7C,$50,$7C,$7C,$40,$60,$3C,$3C,$60,$60,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$40,$40,$20,$30,$1C,$0C,$10,$30,$60,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$40,$60,$30,$1C,$5C,$70,$30,$1C,$1C,$30,$60,$40,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$38,$7C,$44,$44,$44,$44,$7C,$38,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$44,$44,$24,$34,$1C,$1C,$34,$24,$44,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$48,$48,$28,$38,$1C,$1C,$38,$28,$48,$48,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$14,$14,$14,$14,$14,$14,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$3C,$7C,$54,$54,$54,$54,$54,$54,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$44,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$08,$28,$78,$5C,$5C,$5C,$5C,$78,$28,$08,$00,$00
JONG_FONT1:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$04,$04,$04,$04,$04,$04,$7C,$3C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$04,$04,$7C,$3C,$04,$04,$04,$7C,$3C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$04,$04,$7C,$3C,$40,$60,$3C,$1C,$70,$60,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$3C,$7C,$40,$40,$40,$40,$40,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$3C,$7C,$40,$40,$44,$64,$3C,$3C,$64,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$3C,$7C,$40,$40,$28,$78,$5C,$5C,$78,$28,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$3C,$7C,$44,$44,$44,$44,$44,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$34,$74,$54,$54,$54,$54,$54,$5C,$5C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$04,$04,$7C,$3C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$44,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$50,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$60,$3C,$3C,$60,$40,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$54,$54,$54,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$18,$44,$7C,$7C,$44,$7C,$7C,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$28,$78,$5C,$5C,$78,$28,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$7C,$7C,$44,$44,$44,$44,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$7C,$7C,$50,$50,$50,$54,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$7C,$7C,$50,$7C,$7C,$40,$60,$3C,$3C,$60,$60,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$40,$40,$20,$30,$1C,$0C,$10,$30,$60,$40,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$40,$60,$30,$1C,$5C,$70,$30,$1C,$1C,$30,$60,$40
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$38,$7C,$44,$44,$44,$44,$7C,$38,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$44,$44,$24,$34,$1C,$1C,$34,$24,$44,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$48,$48,$28,$38,$1C,$1C,$38,$28,$48,$48,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$14,$14,$14,$14,$14,$14,$7C,$3C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$3C,$7C,$54,$54,$54,$54,$54,$54,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$44,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$08,$28,$78,$5C,$5C,$5C,$5C,$78,$28,$08,$00
JONG_FONT2:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$04,$04,$04,$04,$04,$04,$04,$7C,$3C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$04,$04,$04,$7C,$3C,$04,$04,$04,$7C,$3C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$04,$04,$04,$04,$7C,$3C,$40,$60,$3C,$1C,$70,$60
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$3C,$7C,$40,$40,$40,$40,$40,$40,$40,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$3C,$7C,$40,$40,$00,$44,$64,$3C,$3C,$64,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$3C,$7C,$40,$40,$40,$28,$78,$5C,$5C,$78,$28,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$3C,$7C,$44,$44,$44,$44,$44,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$34,$74,$54,$54,$54,$54,$54,$5C,$5C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$04,$04,$7C,$3C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$44,$7C,$7C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$50,$7C,$7C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$60,$3C,$3C,$60,$40
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$54,$54,$54,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$18,$44,$7C,$7C,$44,$7C,$7C,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$28,$78,$5C,$5C,$78,$28,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$7C,$7C,$44,$44,$44,$44,$44,$7C,$7C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$04,$7C,$7C,$50,$50,$50,$50,$54,$7C,$7C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$7C,$7C,$50,$7C,$7C,$40,$60,$3C,$3C,$60,$60,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$40,$40,$20,$30,$1C,$0C,$10,$30,$60,$40,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$40,$60,$30,$1C,$5C,$70,$30,$1C,$1C,$30,$60,$40
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$38,$7C,$44,$44,$44,$44,$7C,$38,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$44,$44,$24,$34,$1C,$1C,$34,$24,$44,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$48,$48,$28,$38,$1C,$1C,$38,$28,$48,$48,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$14,$14,$14,$14,$14,$14,$14,$7C,$3C,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$04,$3C,$7C,$54,$54,$54,$54,$54,$54,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$44,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$08,$28,$78,$5C,$5C,$5C,$5C,$78,$28,$08,$00
JONG_FONT3:
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$04,$04,$04,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$04,$04,$7C,$3C,$00,$04,$04,$04,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$04,$04,$7C,$3C,$40,$60,$3C,$1C,$70,$60,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$40,$40,$40,$40,$40,$40,$40,$40,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$40,$40,$00,$44,$64,$3C,$3C,$64,$44,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$3C,$7C,$40,$40,$40,$28,$78,$5C,$5C,$78,$28,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$08,$38,$78,$48,$48,$48,$48,$48,$48,$48,$48,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$54,$54,$54,$54,$54,$5C,$5C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$04,$04,$04,$7C,$3C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$44,$44,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$50,$7C,$7C,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$40,$60,$3C,$3C,$60,$40,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$5C,$00,$7C,$7C,$54,$54,$54,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$34,$74,$54,$5C,$18,$44,$7C,$7C,$44,$7C,$7C,$44,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$34,$74,$54,$5C,$5C,$00,$28,$78,$5C,$5C,$78,$28,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$7C,$7C,$44,$44,$44,$44,$44,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$04,$7C,$7C,$50,$50,$50,$50,$50,$54,$7C,$7C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$7C,$7C,$50,$7C,$7C,$40,$60,$3C,$3C,$60,$60,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$40,$40,$20,$30,$1C,$0C,$10,$30,$60,$40,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$40,$60,$30,$1C,$1C,$30,$60,$60,$30,$1C,$1C,$30,$60,$40,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$00,$38,$7C,$44,$44,$44,$44,$7C,$38,$00,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$44,$44,$24,$34,$1C,$1C,$34,$24,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$48,$48,$28,$38,$1C,$1C,$38,$28,$48,$48,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$14,$14,$14,$14,$14,$14,$14,$14,$7C,$3C,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$04,$3C,$7C,$54,$54,$54,$54,$54,$54,$54,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$44,$44,$7C,$7C,$44,$44,$7C,$7C,$44,$44,$00,$00,$00
	.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.db $00,$00,$00,$08,$28,$78,$5C,$5C,$5C,$5C,$78,$28,$08,$00,$00,$00
.list

