;====================================================================
;	User Register Definitions in OK-2561 V1.0 Training Kit
;====================================================================
.def	RAMPZ_BUFFER	= r11		; can't use immediate addressing
.def	KEY_FLAG	= r12
.def	ZERO_FLAG	= r13
.def	TXD_BUFFER	= r14
.def	RXD_BUFFER	= r15

.def	AL		= r16		; can use immediate addressing
.def	AH		= r17
.def	BL		= r18
.def	BH		= r19
.def	CL		= r20
.def	CH		= r21
.def	DL		= r22
.def	DH		= r23

.def	LCD_BUFFER	= r24
.def	DELAY_COUNT	= r25

;====================================================================
;	External I/O Address in OK-2561 V1.0 Training Kit
;====================================================================
;------------------------------------------------
;	LCD, GLCD, and 7-Segment LED
;------------------------------------------------
.equ	LCD_DATABUS	= 0x4000	; LCD/GLCD data
					; 7-segment LED pattern
.equ	LCD_CONTROL	= 0x4100	; LCD/GLCD control signal
.equ	DIG_SELECT	= 0x4200	; 7-segment LED digit select
					; 8-bit parallel data

;------------------------------------------------
;	DS12C887 RTC
;------------------------------------------------
.equ	RTC_SECOND	= 0x4300	; second   (00-59, BCD)
.equ	RTC_SEC_ALM	= 0x4301	; second alarm  
.equ	RTC_MINUTE	= 0x4302	; minute   (00-59, BCD)
.equ	RTC_MIN_ALM	= 0x4303	; minute alarm
.equ	RTC_HOUR	= 0x4304	; hour     (AM = 01-12, PM = 81-92, BCD)
.equ	RTC_HOUR_ALM	= 0x4305	; hour alarm
.equ	RTC_WEEKDAY	= 0x4306	; week day (SUN = 1, SAT = 7)
.equ	RTC_DATE	= 0x4307	; date     (01-31, BCD)
.equ	RTC_MONTH	= 0x4308	; month    (01-12, BCD)
.equ	RTC_YEAR	= 0x4309	; year     (00-99, BCD)
.equ	RTC_A		= 0x430A	; Reg. A : b7 = UIP flag(read only)
					;	   b6-4 = 010   oscillator on
					;	   b3-0 = 0000  no SQW frequency
					;		  0011  8192 Hz
					;		  1111  2 Hz
.equ	RTC_B		= 0x430B	; Reg. B : b7 = set UIP transfer
					;	   b6 = PIE(periodic interrupt enable)
					;	   b5 = AIE(alarm interrupt enable)
					;	   b4 = UIE(update ended interrupt enable)
					;	   b3 = square wave enable(SQW)
					;	   b2 = data mode(0=BCD, 1=binary)
					;	   b1 = 12/24 bit control(0=12, 1=24)
					;	   b0 = daylight saving enable
.equ	RTC_C		= 0x430C	; Reg. C : b7 = interrupt request flag
					;	   b6 = PF(periodic interrupt flag)
					;	   b5 = AF(alarm interrupt flag)
					;	   b4 = UF(update ended interrupt flag)
					;	   b3-0 = not used
.equ	RTC_D		= 0x430D	; Reg. D : b7 = VRT(valid RAM and time, read only)
					;	   b6-0 = not used
.equ	RTC_FLAG	= 0x430F	; battery backup and time OK("G") flag

;********************************************************************
;*	Common Subroutines for OK-2561 V1.0 Training Kit	    *
;********************************************************************
;====================================================================
;	Initialize ATmega2561 MCU and OK-2561 Kit
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

	CLR	AL			; initialize system variables
	MOV	ZERO_FLAG,AL
	MOV	KEY_FLAG,AL
	RET

;====================================================================
;	Text LCD Display
;====================================================================
;------------------------------------------------
;	Initialize LCD Module
;------------------------------------------------
INIT_LCD:LDI	LCD_BUFFER,0x38		; function set(8 bit, 2 line, 5x7 dot)
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
#if(defined(__ATmega2560__) || defined(__ATmega2561__))
	PUSH	RAMPZ_BUFFER
#endif
	IN	ZL,GPIOR2		; restore RAMPZ:ZH:ZL
	OUT	RAMPZ,ZL
	IN	ZH,GPIOR1
	IN	ZL,GPIOR0
	RET

;------------------------------------------------
;	Display 4 Digit HEX Data(AH:AL)
;------------------------------------------------
LCD_4HEX:MOV	LCD_BUFFER,AH		; high byte
	RCALL	LCD_2HEX
	MOV	LCD_BUFFER,AL		; low byte
	RCALL	LCD_2HEX
	RET

;------------------------------------------------
;	Display 2 Digit HEX Data(LCD_BUFFER)
;------------------------------------------------
LCD_2HEX:PUSH	LCD_BUFFER
	LSR	LCD_BUFFER		; upper digit
	LSR	LCD_BUFFER
	LSR	LCD_BUFFER
	LSR	LCD_BUFFER
	CPI	LCD_BUFFER,0x0A
	BRSH	LCD_2HEX1
	ORI	LCD_BUFFER,'0'
	RJMP	LCD_2HEX2
LCD_2HEX1:SUBI	LCD_BUFFER,0x0A
	PUSH	AL
	LDI	AL,'A'
	ADD	LCD_BUFFER,AL
	POP	AL
LCD_2HEX2:CALL	LCD_DATA
	POP	LCD_BUFFER		; lower digit
	ANDI	LCD_BUFFER,0x0F
	CPI	LCD_BUFFER,0x0A
	BRSH	LCD_2HEX3
	ORI	LCD_BUFFER,'0'
	RJMP	LCD_2HEX4
LCD_2HEX3:SUBI	LCD_BUFFER,0x0A
	PUSH	AL
	LDI	AL,'A'
	ADD	LCD_BUFFER,AL
	POP	AL
LCD_2HEX4:CALL	LCD_DATA
	RET

;------------------------------------------------
;	Display 5/4/3-Digit Decimal Number(AH:AL)
;	Display     2-Digit Decimal Number(AL)
;------------------------------------------------
LCD_5DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
	CLR	BH
	CLR	BL
	LDI	DH,high(10000)		; display 10^4 digit
	LDI	DL,low(10000)
	RCALL	DIV16
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CPI	LCD_BUFFER,'0'
	BRNE	LCD_5DEC1
	LDI	LCD_BUFFER,' '
	RJMP	LCD_5DEC2
LCD_5DEC1:CLR	ZERO_FLAG
	COM	ZERO_FLAG
LCD_5DEC2:RCALL	LCD_DATA
	MOVW	AH:AL,BH:BL
	RJMP	LCD_4DEC1

LCD_4DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
LCD_4DEC1:CLR	BH
	CLR	BL
	LDI	DH,high(1000)		; display 10^3 digit
	LDI	DL,low(1000)
	RCALL	DIV16
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CLR	AH
	CP	ZERO_FLAG,AH
	BRNE	LCD_4DEC3
	CPI	LCD_BUFFER,'0'
	BRNE	LCD_4DEC2
	LDI	LCD_BUFFER,' '
	RJMP	LCD_4DEC3
LCD_4DEC2:CLR	ZERO_FLAG
	COM	ZERO_FLAG
LCD_4DEC3:RCALL	LCD_DATA
	MOVW	AH:AL,BH:BL
	RJMP	LCD_3DEC1

LCD_3DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
LCD_3DEC1:LDI	BL,100			; display 10^2 digit
	RCALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CLR	BH
	CP	ZERO_FLAG,BH
	BRNE	LCD_3DEC3
	CPI	LCD_BUFFER,'0'
	BRNE	LCD_3DEC2
	LDI	LCD_BUFFER,' '
	RJMP	LCD_3DEC3
LCD_3DEC2:CLR	ZERO_FLAG
	COM	ZERO_FLAG
LCD_3DEC3:RCALL	LCD_DATA
	MOV	AL,AH
	RJMP	LCD_2DEC1

LCD_2DEC:PUSH	DH			; push registers
	PUSH	DL
	PUSH	BH
	PUSH	BL
	CLR	ZERO_FLAG		; clear zero zone flag
LCD_2DEC1:CLR	AH
	LDI	BL,10			; display 10^1 digit
	RCALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CLR	BH
	CP	ZERO_FLAG,BH
	BRNE	LCD_2DEC2
	CPI	LCD_BUFFER,'0'
	BRNE	LCD_2DEC2
	LDI	LCD_BUFFER,' '
LCD_2DEC2:RCALL	LCD_DATA
	MOV	LCD_BUFFER,AH		; display 10^0 digit
	ORI	LCD_BUFFER,'0'
	RCALL	LCD_DATA
	POP	BL
	POP	BH
	POP	DL
	POP	DH
	RET

;------------------------------------------------
;	Display 8-Bit Binary Data(LCD_BUFFER)
;------------------------------------------------
LCD_8BIN:PUSH	AH
	PUSH	AL
	LDI	AH,8
	MOV	AL,LCD_BUFFER
LCD_8B1:MOV	LCD_BUFFER,AL
	ROL	LCD_BUFFER
	MOV	AL,LCD_BUFFER
	ROL	LCD_BUFFER
	ANDI	LCD_BUFFER,0b00000001
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	DEC	AH
	BRNE	LCD_8B1
	POP	AL
	POP	AH
	RET

;====================================================================
;	Input Key Switch with Beep & Debounce(AL=key value)
;====================================================================
KEY_IN:	RCALL	KEY_CHECK		; read 1st
	MOV	AH,AL
	CALL	D1MS
	CALL	KEY_CHECK		; read 2nd
	CPSE	AH,AL
	RJMP	KEY_IN
	CPI	AL,0b11110000		; if no key, check key off
	BRNE	KEY_IN2
	CLR	AH
	CP	KEY_FLAG,AH
	BREQ	KEY_IN1
	CALL	D20MS
	CLR	KEY_FLAG
KEY_IN1:RET
KEY_IN2:CLR	AH			; if key input, check continuous key
	CP	KEY_FLAG,AH
	BREQ	KEY_IN3
	LDI	AL,0b11110000		; if continuous, treat as no key input
	RET
KEY_IN3:COM	KEY_FLAG		; if new key, beep & delay for debounce
	CALL	BEEP
	RET

KEY_CHECK:IN	AL,PINF			; read key input
	ANDI	AL,0b11110000
	RET

;====================================================================
;	Buzzer Sound
;====================================================================
;------------------------------------------------
;	Buzzer ON or OFF
;------------------------------------------------
BUZZER_ON:
	SBI	PORTE,2
	RET

BUZZER_OFF:
	CBI	PORTE,2
	RET

;------------------------------------------------
;	Sound Beep
;------------------------------------------------
BEEP:	RCALL	BUZZER_ON
	RCALL	D50MS
	RCALL	BUZZER_OFF
	RET

;------------------------------------------------
;	Sound Error
;------------------------------------------------
ERROR:	RCALL	BEEP			; beep 2 times
	RCALL	D100MS
	RCALL	BEEP
	RET

;====================================================================
;	Time Delay (for 16MHz clock)
;====================================================================
D1US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,1
	RJMP	BASE1US

D2US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,2
	RJMP	BASE1US

D5US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,5
	RJMP	BASE1US

D10US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,10
	RJMP	BASE1US

D20US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,20
	RJMP	BASE1US

D50US:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,50
	RJMP	BASE1US

D100US: PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,100
	RJMP	BASE1US

D200US: PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,200
	RJMP	BASE1US

D500US: RCALL	D100US
	RCALL	D200US
	RCALL	D200US
	RET

D1MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,1
	RJMP	BASE1MS

D2MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,2
	RJMP	BASE1MS

D5MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,5
	RJMP	BASE1MS

D10MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,10
	RJMP	BASE1MS

D20MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,20
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

D2SEC:	RCALL	D1SEC
	RCALL	D1SEC
	RET

D5SEC:	RCALL	D1SEC
	RCALL	D2SEC
	RCALL	D2SEC
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

;====================================================================
;	16-Bit Arithmetic
;====================================================================
;------------------------------------------------
;	16-Bit Addition (AH:AL = AH:AL + BH:BL)
;------------------------------------------------
ADD16:	ADD	AL,BL			; add low bytes
	ADC	AH,BH			; add high bytes
	RET

;------------------------------------------------
;	16-Bit Subtraction (AH:AL = AH:AL - BH:BL)
;------------------------------------------------
SUB16:	SUB	AL,BL			; subtract low bytes
	SBC	AH,BH			; subtract high bytes
	RET

;------------------------------------------------
;	16-Bit * 16-Bit = 32-Bit Unsigned Multiplication
;	(BH:BL:AH:AL = AH:AL * BH:BL)
;------------------------------------------------
MUL16:	PUSH	R0
	PUSH	R1
	PUSH	R2
	PUSH	CL
	PUSH	CH
	PUSH	DL
	PUSH	DH
	CLR	R2
	MUL	AH,BH			; AH * BH
	MOVW	DH:DL,R1:R0
	MUL	AL,BL			; AL * BL
	MOVW	CH:CL,R1:R0
	MUL	AH,BL			; AH * BL
	ADD	CH,R0
	ADC	DL,R1
	ADC	DH,R2
	MUL	BH,AL			; BH * AL
	ADD	CH,R0
	ADC	DL,R1
	ADC	DH,R2
	MOVW	AH:AL,CH:CL		; BH:BL:AH:AL <-- DH:DL:CH:CL
	MOVW	BH:BL,DH:DL
	POP	DH
	POP	DL
	POP	CH
	POP	CL
	POP	R2
	POP	R1
	POP	R0
	RET

;------------------------------------------------
;	32-Bit / 16-Bit = 16-Bit Unsigned Division
;	(BH:BL:AH:AL / DH:DL --> Q = AH:AL, R = BH:BL)
;------------------------------------------------
DIV16:	PUSH	R0			; quotient
	PUSH	R1
	PUSH	R2			; remainder
	PUSH	R3
	PUSH	R4
	PUSH	R5
	PUSH	CL			; bit counter
	PUSH	CH			; zero value register

	CLR	R2			; clear remainder
	CLR	R3
	CLR	R4
	CLR	R5
	CLR	CH			; clear zero value register

	LDI	CL,32			; loop counter
DIV16A:	LSL	AL			; shift dividend by 1 bit to left
	ROL	AH
	ROL	BL
	ROL	BH
	ROL	R2
	ROL	R3
	ROL	R4
	ROL	R5
	CP	R5,CH			; remainder >= divisor ?
	BRNE	DIV16B
	CP	R4,CH
	BRNE	DIV16B
	CP	R3,DH
	BRLO	DIV16C
	BRNE	DIV16B
	CP	R2,DL
	BRLO	DIV16C
DIV16B:	SUB	R2,DL			; if yes, remainder - divisor
	SBC	R3,DH
	SBC	R4,CH
	SBC	R5,CH
	SEC				;   and shift 1 into quotient
	RJMP	DIV16D
DIV16C:	CLC				; if no, shift 0 into quotient
DIV16D:	ROL	R0			; shift quotient by 1 bit to left
	ROL	R1
	DEC	CL
	BRNE	DIV16A

	MOV	BH,R3			; make remainder
	MOV	BL,R2
	MOV	AH,R1			; make quotient
	MOV	AL,R0

	POP	CH
	POP	CL
	POP	R5
	POP	R4
	POP	R3
	POP	R2
	POP	R1
	POP	R0
	RET

;------------------------------------------------
;	16-Bit / 8-Bit = 8-Bit Unsigned Division
;	(AH:AL / BL --> Q = AL, R = AH)
;------------------------------------------------
DIV8:	PUSH	R0			; quotient
	PUSH	R1			; remainder
	PUSH	R2
	PUSH	CL			; bit counter
	PUSH	CH			; zero value register

	CLR	R1			; clear remainder
	CLR	R2
	CLR	CH			; clear zero value register

	LDI	CL,16			; loop counter
DIV8A:	LSL	AL			; shift dividend by 1 bit to left
	ROL	AH
	ROL	R1
	ROL	R2
	CP	R2,CH			; remainder >= divisor ?
	BRNE	DIV8B
	CP	R1,BL
	BRLO	DIV8C
DIV8B:	SUB	R1,BL			; if yes, remainder - divisor
	SBC	R2,CH			;   and shift 1 into quotient
	SEC
	RJMP	DIV8D
DIV8C:	CLC				; if no, shift 0 into quotient
DIV8D:	ROL	R0			; shift quotient by 1 bit to left
	DEC	CL
	BRNE	DIV8A

	MOV	AH,R1			; make remainder
	MOV	AL,R0			; make quotient

	POP	CH
	POP	CL
	POP	R2
	POP	R1
	POP	R0
	RET
