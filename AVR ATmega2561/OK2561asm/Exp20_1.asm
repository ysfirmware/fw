;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]	      Exp20_1.asm : Read/Write ATmega2561 EEPROM	   []
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
        .db     " ATmega2561 EEP ",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
        .db     "0x0000=0x00/0x00",0,0

	LDI	AL,0b00000000		; can erase and write EEPROM
	OUT	EECR,AL			;  (EEPM1=EEPM0=0)

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

LOOP2:	LDI	AL,1			; loop from 0x0000 to 0x0FFF
	ADD	BL,AL
	CLR	AL
	ADC	BH,AL
	CPI	BH,0x10
	BRNE	LOOP

	CALL	BEEP
LOOP3:	JMP	LOOP3

;------------------------------------------------
;	Read EEPROM (BH:BL=address, DL= read data)
;------------------------------------------------
READ:	SBIC	EECR,EEPE		; if EEPE=1, wait
	RJMP	READ
	OUT	EEARH,BH		; if EEPE=0, read
	OUT	EEARL,BL
	SBI	EECR,EERE
	IN	DL,EEDR
	RET

;------------------------------------------------
;	Write EEPROM (BH:BL=address, DL=data)
;------------------------------------------------
WRITE:	SBIC	EECR,EEPE		; if EEPE=1, wait
	RJMP	WRITE
WRITE1:	LDS	AL,SPMCSR		; if SPMEM=1, wait
	SBRC	AL,SPMEN
	RJMP	WRITE1
	OUT	EEARH,BH		; if not, write
	OUT	EEARL,BL
	OUT	EEDR,DL
	SBI	EECR,EEMPE
	SBI	EECR,EEPE
	RET

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
