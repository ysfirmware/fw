;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	      Exp14_1.asm : AT24C16 Read/Write using TWI	   []
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
        .db     " AT24C16 EEPROM ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
        .db     "0x0000=0x00/0x00",0,0

	LDI	AL,12			; 400kHz(PWBR=12, TWPS=0)
	STS	TWBR,AL
	LDI	AL,0b00000000
	STS	TWSR,AL

	LDI	BH,0			; initial address
	LDI	BL,0
LOOP:	LDI	LCD_BUFFER,0xC2		; display address
	CALL	LCD_COMMAND
	MOV	AH,BH
	MOV	AL,BL
	CALL	LCD_4HEX
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

LOOP2:	LDI	AL,1			; loop from 0x0000 to 0x07FF
	ADD	BL,AL
	CLR	AL
	ADC	BH,AL
	CPI	BH,0x08
	BRNE	LOOP

	CALL	BEEP
LOOP3:	JMP	LOOP3

;------------------------------------------------
;	AT24C16 Read (BH:BL=address, DL= read data)
;------------------------------------------------
READ:	LDI	AL,0b10100100		; START condition
	STS	TWCR,AL
READ1:	LDS	AL,TWCR			; START complete ?
	SBRS	AL,TWINT
	RJMP	READ1
READ2:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x08
	BRNE	READ2

	MOV	AL,BH			; SLA+W
	LSL	AL
	ANDI	AL,0b00001110
	ORI	AL,0xA0
	STS	TWDR,AL
	LDI	AL,0b10000100
	STS	TWCR,AL
READ3:	LDS	AL,TWCR			; SLA+W complete ?
	SBRS	AL,TWINT
	RJMP	READ3
READ4:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x18
	BRNE	READ4

	STS	TWDR,BL			; word address
	LDI	AL,0b10000100
	STS	TWCR,AL
READ5:	LDS	AL,TWCR			; address complete ?
	SBRS	AL,TWINT
	RJMP	READ5
READ6:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x28
	BRNE	READ6

	LDI	AL,0b10100100		; REPEATED START condition
	STS	TWCR,AL
READ7:	LDS	AL,TWCR			; REPEATED START complete ?
	SBRS	AL,TWINT
	RJMP	READ7
READ8:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x10
	BRNE	READ8

	MOV	AL,BH			; SLA+R
	LSL	AL
	ANDI	AL,0b00001110
	ORI	AL,0b10100001
	STS	TWDR,AL
	LDI	AL,0b10000100
	STS	TWCR,AL
READ9:	LDS	AL,TWCR			; SLA+R complete ?
	SBRS	AL,TWINT
	RJMP	READ9
READ10:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x40
	BRNE	READ10

	LDI	AL,0b10000100		; read data with no acknowledge
	STS	TWCR,AL
READ11:	LDS	AL,TWCR			; address complete ?
	SBRS	AL,TWINT
	RJMP	READ11
READ12:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x58
	BRNE	READ12
	LDS	DL,TWDR			; read data

	LDI	AL,0b10010100		; STOP condition
	STS	TWCR,AL
	RET

;------------------------------------------------
;	AT24C16 Write (BH:BL=address, DL=data)
;------------------------------------------------
WRITE:	LDI	AL,0b10100100		; START condition
	STS	TWCR,AL
WRITE1:	LDS	AL,TWCR			; START complete ?
	SBRS	AL,TWINT
	RJMP	WRITE1
WRITE2:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x08
	BRNE	WRITE2

	MOV	AL,BH			; SLA+W
	LSL	AL
	ANDI	AL,0b00001110
	ORI	AL,0b10100000
	STS	TWDR,AL
	LDI	AL,0b10000100
	STS	TWCR,AL
WRITE3:	LDS	AL,TWCR			; SLA+W complete ?
	SBRS	AL,TWINT
	RJMP	WRITE3
WRITE4:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x18
	BRNE	WRITE4

	STS	TWDR,BL			; word address
	LDI	AL,0b10000100
	STS	TWCR,AL
WRITE5:	LDS	AL,TWCR			; address complete ?
	SBRS	AL,TWINT
	RJMP	WRITE5
WRITE6:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x28
	BRNE	WRITE6

	STS	TWDR,DL			; write data
	LDI	AL,0b10000100
	STS	TWCR,AL
WRITE7:	LDS	AL,TWCR			; data complete ?
	SBRS	AL,TWINT
	RJMP	WRITE7
WRITE8:	LDS	AL,TWSR
	ANDI	AL,0b11111000
	CPI	AL,0x28
	BRNE	WRITE8

	LDI	AL,0b10010100		; STOP condition
	STS	TWCR,AL
	CALL	D5MS			; delay 5 ms for twr time
	RET

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
