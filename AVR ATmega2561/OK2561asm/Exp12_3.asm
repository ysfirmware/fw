;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]   Exp12_3.asm : RS-232C Communication using USART0 Interrupt   []
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
        JMP     RESET                   ; power-on reset entry point

        .org    0x0032
        JMP	USART0_RX		; USART0 Rx complete

RESET:	LDI	AH,high(RAMEND)		; initialize SP
	LDI	AL,low(RAMEND)
	OUT	SPH,AH
	OUT	SPL,AL

	CALL	INIT_MCU		; initialize MCU and kit
	CALL	D50MS			; wait for system stabilization
	CALL	INIT_LCD		; initialize text LCD
	CALL	BEEP

;------------------------------------------------
;	Initialize USART0 (19200 bps, 8 data, no parity, 1 stop)
;------------------------------------------------
	LDI	AH,high(51)		; 19200 bps
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

;------------------------------------------------
;	Key Scan and Transmit a String
;------------------------------------------------
        CALL    LCD_HOME1               ; display title on LCD
	CALL	LCD_STRING
        .db     "RS-232C (USART0)",0,0
	CALL	LCD_HOME2
	CALL	LCD_STRING
        .db     "                ",0,0

	LDI	LCD_BUFFER,0x0F		; cursor ON
	CALL	LCD_COMMAND
	LDI	LCD_BUFFER,0xC0
	CALL	LCD_COMMAND
	LDI	CL,0			; cursor position

READ_KEY:CALL	KEY_IN			; key input
	CPI	AL,0b11100000		; KEY1 ?
	BRNE	CK_KEY2
	COM	AL			; turn on LED1
	ANDI	AL,0b11110000
	OUT	PORTB,AL
	CALL	TX0_STRING
        .db     "  KEY1 is OK !  ",0x0D,0x0A,0,0
	JMP	READ_KEY

CK_KEY2:CPI	AL,0b11010000		; KEY2 ?
	BRNE	CK_KEY3
	COM	AL			; turn on LED2
	ANDI	AL,0b11110000
	OUT	PORTB,AL
	CALL	TX0_STRING
        .db     "  KEY2 is OK !  ",0x0D,0x0A,0,0
	JMP	READ_KEY

CK_KEY3:CPI	AL,0b10110000		; KEY3 ?
	BRNE	CK_KEY4
	COM	AL			; turn on LED3
	ANDI	AL,0b11110000
	OUT	PORTB,AL
	CALL	TX0_STRING
        .db     "  KEY3 is OK !  ",0x0D,0x0A,0,0
	JMP	READ_KEY

CK_KEY4:CPI	AL,0b01110000		; KEY4 ?
	BRNE	READ_KEY
	COM	AL			; turn on LED4
	ANDI	AL,0b11110000
	OUT	PORTB,AL
	CALL	TX0_STRING
        .db     "  KEY4 is OK !  ",0x0D,0x0A,0,0
	JMP	READ_KEY

;====================================================================
;	RS-232C Subroutines using USART0
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
;	Transmit a Character(TXD_BUFFER) by USART0
;------------------------------------------------
TX0_CHAR:PUSH	AL

TX0_CHAR1:LDS	AL,UCSR0A		; data register empty ?
	SBRS	AL,UDRE0
	JMP	TX0_CHAR1
	STS	UDR0,TXD_BUFFER

	POP	AL
	RET

;====================================================================
;	Interrupt Service Routine of USART0
;====================================================================
USART0_RX:PUSH	AL                      ; store registers
        IN      AL,SREG
        PUSH    AL

	LDS	LCD_BUFFER,UDR0		; if yes, display a character
	CALL	LCD_DATA
	INC	CL			; 16 character OK ?
	CPI	CL,16
	BRNE	USART0_RX1
	LDI	CL,0			; if yes, go to home
	LDI	LCD_BUFFER,0xC0
	CALL	LCD_COMMAND
	CALL	BEEP
USART0_RX1:
        POP     AL                      ; restore registers
        OUT     SREG,AL
        POP     AL
        RETI

;====================================================================
;	Include User Subroutine File
;====================================================================
.include "OK2561.INC"			; OK-2561 I/O and subroutine file
