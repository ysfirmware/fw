;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	   System Diagnostic Program for "OK-2561" Kit V1.0	   []
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
	JMP	RESET			; power-on reset entry point

	.org	0x0032
	JMP	USART0_RXC		; USART0 Rx Complete

RESET:	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD

	CALL	CLEAR_SCREEN		; clear graphic LCD screen
	CLR	AL			; cursor off
	STS	CURSOR_FLAG,AL

;------------------------------------------------
;	Boot and Display Logo
;------------------------------------------------
	CALL	LCD_HOME1		; display logo 1 on text LCD
	CALL	LCD_STRING
	.db	"  OK-2561 V1.0  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" ATmega2561-16AU",0,0

	CALL	BUZZER_ON		; buzzer on
	LDI	AL,0b00010000		; LED1 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b00100000		; LED2 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b01000000		; LED3 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b10000000		; LED4 on
	OUT	PORTB,AL
	CALL	D200MS
	LDI	AL,0b00000000		; all LED off
	OUT	PORTB,AL
	CALL	BUZZER_OFF		; buzzer off
	CALL	D2SEC

	CALL	LCD_HOME2		; display logo 2
	CALL	LCD_STRING
	.db	"D.Y.Yoon in 2006",0,0
	CALL	D2SEC

;------------------------------------------------
;	TEST 1 : Test Graphic LCD Module
;------------------------------------------------
TEST1:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	"Graphic LCD TEST",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"ASCII characters",0,0
	CALL	BEEP

	CALL	GLCD_HOME0		; display screen 1
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
TEST1A:	MOV	LCD_BUFFER,DL
	CALL	GLCD_CHARACTER
	INC	DL
	DEC	CL
	BRNE	TEST1A
	CALL	D5SEC
	CALL	CLEAR_SCREEN		; clear graphic LCD screen

;------------------------------------------------
;	TEST 2 : Test Key Input
;------------------------------------------------
TEST2:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	" KEY INPUT TEST ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Press KEY1-4 ! ",0,0
	CALL	BEEP

	LDI	CL,150			; loop = 150 times
READ_KEY:CALL	KEY_CHECK		; input key
	CPI	AL,0b11100000		; if KEY1, display KEY1 message
	BRNE	CK_KEY2
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"   KEY1 is OK.  ",0,0
	JMP	TEST2X

CK_KEY2:CPI	AL,0b11010000		; if KEY2, display KEY2 message
	BRNE	CK_KEY3
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"   KEY2 is OK.  ",0,0
	JMP	TEST2X

CK_KEY3:CPI	AL,0b10110000		; if KEY3, display KEY3 message
	BRNE	CK_KEY4
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"   KEY3 is OK.  ",0,0
	JMP	TEST2X

CK_KEY4:CPI	AL,0b01110000		; if KEY4, display KEY4 message
	BRNE	TEST2X
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"   KEY4 is OK.  ",0,0

TEST2X:	COM	AL			; display LED
	ANDI	AL,0b11110000
	OUT	PORTB,AL
	CALL	D50MS
	DEC	CL
	BREQ	TEST2Y
	RJMP	READ_KEY

TEST2Y: LDI	AL,0b00000000		; clear LED
	OUT	PORTB,AL

;------------------------------------------------
;	TEST 3 : Speaker Sound
;------------------------------------------------
TEST3:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	"  SPEAKER TEST  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Do Re Mi Fa ...",0,0
	CALL	D1SEC

	LDI	AL,4			; tempo = 4
	CALL	SET_TEMPO
	CALL	MIC
	CALL	NOTE4
	CALL	MID
	CALL	NOTE4
	CALL	MIE
	CALL	NOTE4
	CALL	MIF
	CALL	NOTE4
	CALL	MIG
	CALL	NOTE4
	CALL	MIA
	CALL	NOTE4
	CALL	MIB
	CALL	NOTE4
	CALL	HIC
	CALL	NOTE2
	CALL	REST4

	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" Do Si Ra Sol...",0,0
	CALL	HIC
	CALL	NOTE4
	CALL	MIB
	CALL	NOTE4
	CALL	MIA
	CALL	NOTE4
	CALL	MIG
	CALL	NOTE4
	CALL	MIF
	CALL	NOTE4
	CALL	MIE
	CALL	NOTE4
	CALL	MID
	CALL	NOTE4
	CALL	MIC
	CALL	NOTE2
	CALL	REST4

;------------------------------------------------
;	TEST 4 : Test A/D Converter
;------------------------------------------------
TEST4:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	"A/D(LM35DZ) TEST",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"ADC2=000H=00.0",0xDF,"C",0,0
	CALL	BEEP

	LDI	AL,0b10000101		; ADC enable, 500kHz
	STS	ADCSRA,AL
	LDI	AL,0b00000000
	STS	ADCSRB,AL
	LDI	AL,0b01000010		; select ADC2 with AVCC
	STS	ADMUX,AL		; (single ended)
	CALL	D50US
	CALL	D100US

	LDI	CH,20			; display 10 sec
TEST4A1:LDI	CL,16			; read ADC by 16 times
	CLR	BH			; clear sum
	CLR	BL
TEST4A2:LDI	AL,0b11010101		; start conversion and clear ADIF
	STS	ADCSRA,AL
	NOP
TEST4A3:LDS	AL,ADCSRA		; conversion complete ?
	SBRS	AL,ADIF
	JMP	TEST4A3
	LDS	AL,ADCL			; ADCL must be read first, then ADCH
	LDS	AH,ADCH
	ADD	BL,AL			; add A/D result 16 times
	ADC	BH,AH
	CALL	D1MS
	DEC	CL
	BRNE	TEST4A2
	LSR	BH			; divide sum by 16
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LDI	LCD_BUFFER,0xC4		; display in HEX
	CALL	LCD_COMMAND
	MOV	AH,BH
	MOV	AL,BL
	CALL	LCD_4HEX
	LDI	LCD_BUFFER,0xC4
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,'='
	CALL	LCD_DATA
	LDI	LCD_BUFFER,0xCA		; display in temperature(XX.X)
	CALL	LCD_COMMAND
	LDI	AH,high(1000)		; Temp = ADC2*100/1024
	LDI	AL,low(1000)
	CALL	MUL16
	LDI	DH,high(1024)
	LDI	DL,low(1024)
	CALL	DIV16
	LDI	BL,100
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	MOV	AL,AH
	CLR	AH
	LDI	BL,10
	CALL	DIV8
	MOV	LCD_BUFFER,AL
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	LDI	LCD_BUFFER,'.'
	CALL	LCD_DATA
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	CALL	D500MS
	DEC	CH
	BREQ	TEST4B
	JMP	TEST4A1

TEST4B:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	" A/D(VR1)  TEST ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"ADC3=000H=0.0[V]",0,0
	CALL	BEEP

	LDI	AL,0b01000011		; select ADC3 with AVCC
	STS	ADMUX,AL		; (single ended)
	CALL	D50US
	CALL	D100US

	LDI	CH,20			; display 10 sec
TEST4B1:LDI	CL,16			; read ADC by 16 times
	CLR	BH			; clear sum
	CLR	BL
TEST4B2:LDI	AL,0b11010101		; start conversion and clear ADIF
	STS	ADCSRA,AL
	NOP
TEST4B3:LDS	AL,ADCSRA		; conversion complete ?
	SBRS	AL,ADIF
	JMP	TEST4B3
	LDS	AL,ADCL			; ADCL must be read first, then ADCH
	LDS	AH,ADCH
	ADD	BL,AL			; add A/D result 16 times
	ADC	BH,AH
	CALL	D1MS
	DEC	CL
	BRNE	TEST4B2
	LSR	BH			; divide sum by 16
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LSR	BH
	ROR	BL
	LDI	LCD_BUFFER,0xC4		; display in HEX
	CALL	LCD_COMMAND
	MOV	AH,BH
	MOV	AL,BL
	CALL	LCD_4HEX
	LDI	LCD_BUFFER,0xC4
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,'='
	CALL	LCD_DATA
	LDI	LCD_BUFFER,0xCA		; display in voltage(X.X)
	CALL	LCD_COMMAND
	LDI	AH,high(50)		; Volt = ADC3*5/1024
	LDI	AL,low(50)
	CALL	MUL16
	LDI	DH,high(1024)
	LDI	DL,low(1024)
	CALL	DIV16
	LDI	BL,10			; calculate X.X
	CALL	DIV8
	ORI	AL,'0'
	MOV	LCD_BUFFER,AL
	CALL	LCD_DATA
	LDI	LCD_BUFFER,'.'
	CALL	LCD_DATA
	MOV	LCD_BUFFER,AH
	ORI	LCD_BUFFER,'0'
	CALL	LCD_DATA
	CALL	D500MS
	DEC	CH
	BREQ	TEST5
	JMP	TEST4B1

;------------------------------------------------
;	TEST 5 : Test D/A Converter
;------------------------------------------------
TEST5:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	"DAC0800 D/A TEST",0,0

TEST5A:	CALL	LCD_HOME2		; triangular wave
	CALL	LCD_STRING
	.db	"Triangular Wave ",0,0
	CALL	BEEP

	LDI	CH,10			; output for 10 sec(about 200 Hz)
TEST5A1:LDI	CL,200
TEST5A2:LDI	ZL,byte3(TRI_TABLE << 1); start address of triangular table
	OUT	RAMPZ,ZL
	LDI	ZH,high(TRI_TABLE << 1)
	LDI	ZL,low(TRI_TABLE << 1)
	LDI	BL,100
TEST5A3:ELPM	AL,Z+			; read data
	OUT	PORTB,AL		; output D/A
	CALL	D50US
	DEC	BL
	BRNE	TEST5A3
	DEC	CL
	BRNE	TEST5A2
	DEC	CH
	BRNE	TEST5A1

TEST5B:	CALL	LCD_HOME2		; sine wave
	CALL	LCD_STRING
	.db	"   Sine Wave    ",0,0
	CALL	BEEP

	LDI	CH,10			; output for 10 sec(about 200 Hz)
TEST5B1:LDI	CL,200
TEST5B2:LDI	ZL,byte3(SIN_TABLE << 1); start address of sine table
	OUT	RAMPZ,ZL
	LDI	ZH,high(SIN_TABLE << 1)
	LDI	ZL,low(SIN_TABLE << 1)
	LDI	BL,100
TEST5B3:ELPM	AL,Z+			; read data
	OUT	PORTB,AL		; output D/A
	CALL	D50US
	DEC	BL
	BRNE	TEST5B3
	DEC	CL
	BRNE	TEST5B2
	DEC	CH
	BRNE	TEST5B1

	LDI	AL,0x00			; output 0x00 to D/A
	OUT	PORTB,AL

	JMP	TEST6

TRI_TABLE:				; triangular wave data table
	.db	$80,$84,$88,$8C,$90,$94,$98,$9C,$A0,$A4
	.db	$A8,$AC,$B0,$B4,$B8,$BC,$C0,$C4,$C8,$CC
	.db	$D0,$D4,$D8,$DC,$E0,$E4,$E0,$DC,$D8,$D4
	.db	$D0,$CC,$C8,$C4,$C0,$BC,$B8,$B4,$B0,$AC
	.db	$A8,$A4,$A0,$9C,$98,$94,$90,$8C,$88,$84
	.db	$80,$7C,$78,$74,$70,$6C,$68,$64,$60,$5C
	.db	$58,$54,$50,$4C,$48,$44,$40,$3C,$38,$34
	.db	$30,$2C,$28,$24,$20,$1C,$20,$24,$28,$2C
	.db	$30,$34,$38,$3C,$40,$44,$48,$4C,$50,$54
	.db	$58,$5C,$60,$64,$68,$6C,$70,$74,$78,$7C

SIN_TABLE:				; sine wave data table
	.db	$80,$88,$90,$98,$A0,$A7,$AF,$B6,$BD,$C4
	.db	$CB,$D1,$D7,$DD,$E2,$E7,$EB,$EF,$F3,$F6
	.db	$F9,$FB,$FD,$FE,$FF,$FF,$FF,$FE,$FD,$FB
	.db	$F9,$F6,$F3,$EF,$EB,$E7,$E2,$DD,$D7,$D1
	.db	$CB,$C4,$BD,$B6,$AF,$A7,$A0,$98,$90,$88
	.db	$80,$78,$70,$68,$60,$59,$51,$4A,$43,$3C
	.db	$35,$2F,$29,$23,$1E,$19,$15,$11,$0D,$0A
	.db	$07,$05,$03,$02,$01,$01,$01,$02,$03,$05
	.db	$07,$0A,$0D,$11,$15,$19,$1E,$23,$29,$2F
	.db	$35,$3C,$43,$4A,$51,$59,$60,$68,$70,$78

;------------------------------------------------
;	TEST 6 : Test RS-232C by USART0
;------------------------------------------------
TEST6:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	"  RS-232C TEST  ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"USART0 testing..",0,0
	CALL	BEEP
	CALL	D2SEC

	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"USART0  not OK !",0,0

	LDI	AH,high(51)		; 19200 baud
	LDI	AL,low(51)
	STS	UBRR0H,AH
	STS	UBRR0L,AL
	LDI	AL,0b00000000		; asynchronous normal mode
	STS	UCSR0A,AL
	LDI	AL,0b10011000		; receiver/transmitter enable, 8 data
	STS	UCSR0B,AL		; Rx interrupt enable
	LDI	AL,0b00000110		; no parity, 1 stop, 8 data
	STS	UCSR0C,AL
	NOP
	LDS	AL,UDR0			; dummy read
	SEI				; global interrupt enable

	LDI	LCD_BUFFER,0xC0		; cursor home 2
	CALL	LCD_COMMAND
	CALL	TX0_STRING		; transmit test message
        .db     "USART0 test OK !",0,0
	CALL	D2SEC			; wait for receiving and displaying data
	CLI				; global interrupt disable

;------------------------------------------------
;	TEST 7 : Test 7-Segment LED
;------------------------------------------------
TEST7:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	" 7-SEG LED TEST ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"  same numbers  ",0,0
	CALL	BEEP

	LDI	AL,0b11111111		; select all digits
	STS	DIG_SELECT,AL
	LDI	AL,0b11111100		; 0
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b01100000		; 1
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b11011010		; 2
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b11110010		; 3
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b01100110		; 4
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b10110110		; 5
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b10111110		; 6
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b11100100		; 7
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b11111110		; 8
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b11110110		; 9
	STS	LCD_DATABUS,AL
	CALL	D500MS
	LDI	AL,0b00000001		; .
	STS	LCD_DATABUS,AL
	CALL	D1SEC

	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"different number",0,0

	LDI	CH,5			; display 4 sec = 5*100*8 ms
TEST7A:	LDI	CL,100
TEST7B:	LDI	AL,0b01100000		; digit8 = 1
	STS	LCD_DATABUS,AL
	LDI	AL,0b10000000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b11011010		; digit7 = 2
	STS	LCD_DATABUS,AL
	LDI	AL,0b01000000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b11110010		; digit6 = 3
	STS	LCD_DATABUS,AL
	LDI	AL,0b00100000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b01100110		; digit5 = 4
	STS	LCD_DATABUS,AL
	LDI	AL,0b00010000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b10110110		; digit4 = 5
	STS	LCD_DATABUS,AL
	LDI	AL,0b00001000
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b10111110		; digit3 = 6
	STS	LCD_DATABUS,AL
	LDI	AL,0b00000100
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b11100100		; digit2 = 7
	STS	LCD_DATABUS,AL
	LDI	AL,0b00000010
	STS	DIG_SELECT,AL
	CALL	D1MS
	LDI	AL,0b11111110		; digit1 = 8
	STS	LCD_DATABUS,AL
	LDI	AL,0b00000001
	STS	DIG_SELECT,AL
	CALL	D1MS
	DEC	CL
	BREQ	TEST7C
	JMP	TEST7B
TEST7C:	DEC	CH
	BREQ	TEST7D
	JMP	TEST7A
TEST7D:	LDI	AL,0b00000000		; clear 7-segment LED
	STS	DIG_SELECT,AL

;------------------------------------------------
;	TEST 8 : Test 62256 External SRAM
;------------------------------------------------
TEST8:	CALL	LCD_HOME1		; display test
	CALL	LCD_STRING
	.db	"62256 SRAM  TEST",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	" 0000H test OK! ",0,0
	CALL	BEEP

	LDI	XH,high(0x8000)		; SRAM start address
	LDI	XL,low(0x8000)

TEST8A:	LDI	LCD_BUFFER,0xC1		; display test address
	CALL	LCD_COMMAND
	MOV	AH,XH
	MOV	AL,XL
	CALL	LCD_4HEX
	LDI	AH,0x55			; test data 0x55
	ST	X,AH
	NOP
	LD	AL,X
	CP	AL,AH
	BRNE	RAM_ERR
	LDI	AH,0xAA			; test data 0xAA
	ST	X,AH
	NOP
	LD	AL,X+
	CP	AL,AH
	BRNE	RAM_ERR
	MOV	AL,XL
	OR	AL,XH
	CPI	AL,0x00
	BRNE	TEST8A
	CALL	D1SEC
	JMP	TEST_END

RAM_ERR:LDI	LCD_BUFFER,0xC6		; display error message
	CALL	LCD_COMMAND
	CALL	LCD_STRING
	.db	"  00H/00H ",0,0
	LDI	LCD_BUFFER,0xC8
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,AH
	CALL	LCD_2HEX
	LDI	LCD_BUFFER,0xCC
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,AL
	CALL	LCD_2HEX
	CALL	ERROR			; error beep
	CALL	D5SEC

TEST_END:CALL	LCD_HOME1		; display test end message
	CALL	LCD_STRING
	.db	"  All test end. ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
	.db	"                ",0,0
	CALL	D2SEC

	JMP	TEST1

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

;====================================================================
;	USART0 Serial Communication Subroutine
;====================================================================
;------------------------------------------------
;	Transmit String by USART0
;------------------------------------------------
TX0_STRING:OUT	GPIOR0,ZL		; store RAMPZ:ZH:ZL
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
TX0_STR1:ELPM	TXD_BUFFER,Z+		; read a character. end string ?
	MOV	AL,TXD_BUFFER
	CPI	AL,0
	BREQ	TX0_STR2		; if yes, return
	CALL	TX0_CHAR		; if no, transmit a character
	RJMP	TX0_STR1
TX0_STR2:SBRC	ZL,0			; if odd address, skip 1 address
	ELPM	TXD_BUFFER,Z+
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
;	Transmit a Character(TX0_BUFFER) by USART0
;------------------------------------------------
TX0_CHAR:PUSH	AL

TX0_CHAR1:LDS	AL,UCSR0A		; data register empty ?
	SBRS	AL,UDRE0
	JMP	TX0_CHAR1
	STS	UDR0,TXD_BUFFER

	POP	AL
	RET

;------------------------------------------------
;	Interrupt Service Routine of USART0
;------------------------------------------------
USART0_RXC:
	PUSH	AL                      ; store registers
        IN      AL,SREG

	LDS	LCD_BUFFER,UDR0		; if yes, display a character
	CALL	LCD_DATA

        OUT     SREG,AL			; restore registers
        POP     AL
        RETI

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
.include "MUSIC2561.INC"		; music subroutine file
