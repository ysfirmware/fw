;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp01_2.asm : Basic Assembly Program (2)	   []
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
;	Definitions in OK-2561 V1.0 Training Kit
;====================================================================
;------------------------------------------------
;	User Register
;------------------------------------------------
.def	RAMPZ_BUFFER	= R11		; can't use immediate addressing

.def	AL		= R16		; can use immediate addressing
.def	AH		= R17
.def	BL		= R18
.def	BH		= R19
.def	CL		= R20
.def	CH		= R21
.def	DL		= R22
.def	DH		= R23

.def	LCD_BUFFER	= R24
.def	DELAY_COUNT	= R25

;------------------------------------------------
;	Address of LCD
;------------------------------------------------
.equ	LCD_DATABUS	= 0x4000	; LCD data output address
.equ	LCD_CONTROL	= 0x4100	; LCD control signal output address
.equ	DIG_SELECT	= 0x4200	; 7-segment LED digit select output address

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
;	Display LCD
;------------------------------------------------
	CALL	LCD_HOME1		; display logo
	CALL	LCD_STRING
	.db	"  OK-2561 V1.0  ",0,0
LOOP:	CALL	LCD_HOME2		; display message 1
	CALL	LCD_STRING
	.db	" ATmega2561-16AU",0,0
	CALL	D1SEC
	CALL	LCD_HOME2		; display message 2
	CALL	LCD_STRING
	.db	"   2006/10/01   ",0,0
	CALL	D1SEC
	JMP	LOOP

;====================================================================
;	Initialize ATmega2561 MCU and System Variables
;====================================================================
INIT_MCU:LDI	AL,0b11000001		; 0x2200-0x7FFF = 1 wait
	STS	XMCRA,AL		; 0x8000-0xFFFF = 0 wait
	LDI	AL,0b10000000		; enable bus keeper
	STS	XMCRB,AL		; use PC0-PC7 as high byte of address bus

	LDI	AL,0xFF			; PORTB = output (D/A, LED1-4, matrix LED)
	OUT	DDRB,AL
	LDI	AL,0x00
	OUT	PORTB,AL
	LDI	AL,0xFF			; PORTD = output (matrix LED)
	OUT	DDRD,AL
	LDI	AL,0x00
	OUT	PORTD,AL
	LDI	AL,0x0C			; PORTE = output for PE3, PE2
	OUT	DDRE,AL
	LDI	AL,0x00
	OUT	PORTE,AL
	LDI	AL,0x00			; PORTF = input (KEY1-4)
	STS	DDRF,AL
	LDI	AL,0x00
	STS	PORTF,AL

	LDI	AL,0b00000000		; clear LCD/GLCD control signal
	STS	LCD_CONTROL,AL

	LDI	AL,0b00000000		; clear 7-segment LED
	STS	DIG_SELECT,AL
	RET

;====================================================================
;	Text LCD Display
;====================================================================
;------------------------------------------------
;	Initialize LCD Module
;------------------------------------------------
INIT_LCD:LDI	AL,0b00000011		; E = 1, Rs = 1 (dummy write)
	STS	LCD_CONTROL,AL
	CALL	D1US
	LDI	AL,0b00000010		; E = 0, Rs = 1
	STS	LCD_CONTROL,AL
	CALL	D2MS
	LDI	LCD_BUFFER,0x38		; function set(8 bit, 2 line, 5x7 dot)
	RCALL	LCD_COMMAND
	LDI	LCD_BUFFER,0x0C		; display control(display ON, cursor OFF)
	RCALL	LCD_COMMAND
	LDI	LCD_BUFFER,0x06		; entry mode set(increment, not shift)
	RCALL	LCD_COMMAND
	LDI	LCD_BUFFER,0x01		; clear display
	RCALL	LCD_COMMAND
	CALL	D2MS
	RET

;------------------------------------------------
;	Set LCD Cursor Position
;------------------------------------------------
LCD_HOME1:LDI	LCD_BUFFER,0x80		; go home 1 (line 1, column 1)
	RJMP	LCD_COMMAND
LCD_HOME2:LDI	LCD_BUFFER,0xC0		; go home 2 (line 2, column 1)
	RJMP	LCD_COMMAND

;------------------------------------------------
;	Write a Command to LCD(LCD_BUFFER)
;------------------------------------------------
LCD_COMMAND:
	PUSH	AL
	LDI	AL,0b00000000		; E = 0, Rs = 0
	STS	LCD_CONTROL,AL
	STS	LCD_DATABUS,LCD_BUFFER	; output command
	LDI	AL,0b00000001		; E = 1
	STS	LCD_CONTROL,AL
	PUSH	R0			; delay 4 cycles(about 250 ns)
	POP	R0
	LDI	AL,0b00000000		; E = 0
	STS	LCD_CONTROL,AL
	CALL	D50US
	POP	AL
	RET

;------------------------------------------------
;	Display a Character on LCD(LCD_BUFFER)
;------------------------------------------------
LCD_DATA:PUSH	AL
	LDI	AL,0b00000010		; E = 0, Rs = 1
	STS	LCD_CONTROL,AL
	STS	LCD_DATABUS,LCD_BUFFER	; output data
	LDI	AL,0b00000011		; E = 1
	STS	LCD_CONTROL,AL
	PUSH	R0			; delay 4 cycles(about 250 ns)
	POP	R0
	LDI	AL,0b00000010		; E = 0
	STS	LCD_CONTROL,AL
	CALL	D50US
	POP	AL
	RET

;------------------------------------------------
;	Display String on LCD
;------------------------------------------------
LCD_STRING:OUT	GPIOR0,ZL		; store RAMPZ:ZH:ZL
	OUT	GPIOR1,ZH
	IN	ZL,RAMPZ
	OUT	GPIOR2,ZL
	POP	RAMPZ_BUFFER		; get start address of string
	POP	ZH
	POP	ZL
	LSL	ZL			; convert into byte address
	ROL	ZH
	ROL	RAMPZ_BUFFER
	OUT	RAMPZ,RAMPZ_BUFFER
LCD_STR1:ELPM	LCD_BUFFER,Z+		; read a character. end string ?
	CPI	LCD_BUFFER,0
	BREQ	LCD_STR2		; if yes, return
	CALL	LCD_DATA		; if no, display a character
	RJMP	LCD_STR1
LCD_STR2:SBRC	ZL,0			; if odd address, skip 1 address
	ELPM	LCD_BUFFER,Z+
	IN	RAMPZ_BUFFER,RAMPZ	; convert into word address
	LSR	RAMPZ_BUFFER
	ROR	ZH
	ROR	ZL
	PUSH	ZL			; store new return address
	PUSH	ZH
	PUSH	RAMPZ_BUFFER
	IN	ZL,GPIOR2		; restore RAMPZ:ZH:ZL
	OUT	RAMPZ,ZL
	IN	ZH,GPIOR1
	IN	ZL,GPIOR0
	RET

;====================================================================
;	Time Delay (for 16MHz clock)
;====================================================================
D1US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,1
	RJMP	BASE1US

D50US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,50
	RJMP	BASE1US

D200US: PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,200
	RJMP	BASE1US

D2MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,2
	RJMP	BASE1MS

D50MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,50
	RJMP	BASE1MS

D100MS: PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,100
	RJMP	BASE1MS

D200MS: PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,200
	RJMP	BASE1MS

D500MS: RCALL	D100MS
	RCALL	D200MS
	RCALL	D200MS
	RET

D1SEC:	RCALL	D500MS
	RCALL	D500MS
	RET

;------------------------------------------------
;	Basic 1 us Delay
;------------------------------------------------
BASE1US:NOP				; 1
	PUSH	DELAY_COUNT		; 2
	POP	DELAY_COUNT		; 2
	PUSH	DELAY_COUNT		; 2
	POP	DELAY_COUNT		; 2
	PUSH	DELAY_COUNT		; 2
	POP	DELAY_COUNT		; 2
	DEC	DELAY_COUNT		; 1
	BRNE	BASE1US			; 2 (total 16 cycles = 1 us)
	POP	DELAY_COUNT
	RET

;------------------------------------------------
;	Basic 1 ms Delay
;------------------------------------------------
BASE1MS:RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	DEC	DELAY_COUNT
	BRNE	BASE1MS			; (total = 1 ms)
	POP	DELAY_COUNT
	RET
