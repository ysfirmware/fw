;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	      Exp13_1.asm : AT93C56 Read/Write using SPI	   []
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
;	Write/Read Test
;------------------------------------------------
        CALL    LCD_HOME1               ; display title on LCD
	CALL	LCD_STRING
        .db     " AT93C56 EEPROM ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
        .db     "0x00 = 0x00/0x00",0,0

	LDI	AL,0b00000111		; MISO=input, etc.=output
	OUT	DDRB,AL
	LDI	AL,0b00000000		; CS = 0
	OUT	PORTB,AL
	LDI	AL,0b01010001		; SPI mode 0, 1MHz
	OUT	SPCR,AL
	LDI	AL,0b00000000		; SPI2X=0
	OUT	SPSR,AL

	CALL	EWEN			; AT93C56 write enable

	LDI	BL,0			; initial address
LOOP:	LDI	LCD_BUFFER,0xC2		; display address
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,BL
	CALL	LCD_2HEX
	LDI	LCD_BUFFER,0xC9		; display write data
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,0x55
	CALL	LCD_2HEX
	LDI	DL,0x55			; write 0x55
	CALL	WRITE
	CALL	READ			; read
	LDI	LCD_BUFFER,0xCE		; display read data
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,DL
	CALL	LCD_2HEX
	CALL	D500MS
	CPI	DL,0x55			; if error, beep
	BREQ	LOOP1
	CALL	BEEP
	CALL	D2SEC

LOOP1:	LDI	LCD_BUFFER,0xC9		; display write data
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,0xAA
	CALL	LCD_2HEX
	LDI	DL,0xAA			; write 0xAA
	CALL	WRITE
	CALL	READ			; read
	LDI	LCD_BUFFER,0xCE		; display read data
	CALL	LCD_COMMAND
	MOV	LCD_BUFFER,DL
	CALL	LCD_2HEX
	CALL	D500MS
	CPI	DL,0xAA			; if error, beep
	BREQ	LOOP2
	CALL	BEEP
	CALL	D2SEC

LOOP2:	INC	BL			; loop from 0x00 to 0xFF
	BRNE	LOOP

	CALL	EWDS			; AT93C56 write disable
	CALL	BEEP
LOOP3:	JMP	LOOP3

;------------------------------------------------
;	AT93C56 Write Enable
;------------------------------------------------
EWEN:	LDI	AL,0b00000001		; CS = 1
	OUT	PORTB,AL
	LDI	AL,0b00001001		; 0000 1001
	OUT	SPDR,AL
EWEN1:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	EWEN1
	LDI	AL,0b10000000		; 1000 0000
	OUT	SPDR,AL
EWEN2:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	EWEN2
	LDI	AL,0b00000000		; CS = 0
	OUT	PORTB,AL
	RET

;------------------------------------------------
;	AT93C56 Write Disable
;------------------------------------------------
EWDS:	LDI	AL,0b00000001		; CS = 1
	OUT	PORTB,AL
	LDI	AL,0b00001000		; 0000 1000
	OUT	SPDR,AL
EWDS1:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	EWDS1
	LDI	AL,0b00000000		; 0000 0000
	OUT	SPDR,AL
EWDS2:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	EWDS2
	LDI	AL,0b00000000		; CS = 0
	OUT	PORTB,AL
	RET

;------------------------------------------------
;	AT93C56 Read (BL=address, DL= read data)
;------------------------------------------------
READ:	LDI	AL,0b00000001		; CS = 1
	OUT	PORTB,AL
	LDI	AL,0b00001100		; 0000 1100
	OUT	SPDR,AL
READ1:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	READ1
	OUT	SPDR,BL			; A7 - A0
READ2:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	READ2
	LDI	AL,0b00000000		; dummy 0000 0000
	OUT	SPDR,AL
READ3:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	READ3
	IN	DH,SPDR			; DH = upper byte
	LDI	AL,0b00000000		; dummy 0000 0000
	OUT	SPDR,AL
READ4:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	READ4
	IN	DL,SPDR			; DL = lower byte
	LDI	AL,0b00000000		; CS = 0
	OUT	PORTB,AL
	LSL	DL			; get data and return
	ROL	DH
	MOV	DL,DH
	RET

;------------------------------------------------
;	AT93C56 Write (BL=address, DL=data)
;------------------------------------------------
WRITE:	LDI	AL,0b00000001		; CS = 1
	OUT	PORTB,AL
	LDI	AL,0b00001010		; 0000 1010
	OUT	SPDR,AL
WRITE1:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	WRITE1
	OUT	SPDR,BL			; A7 - A0
WRITE2:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	WRITE2
	OUT	SPDR,DL			; D7 - D0
WRITE3:	IN	AL,SPSR			; transmit complete ?
	SBRS	AL,SPIF
	RJMP	WRITE3
	LDI	AL,0b00000000		; CS = 0
	OUT	PORTB,AL

	CALL	D5MS			; delay 5 ms for twp time
	RET

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
