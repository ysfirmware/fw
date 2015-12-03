;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]								   []
;[]		Exp01_1.asm : Basic Assembly Program (1)	   []
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

;------------------------------------------------
;	Initialize ATmega2561 MCU
;------------------------------------------------
.cseg
	.org	0x0000
	LDI	R17,high(RAMEND)	; initialize SP
	LDI	R16,low(RAMEND)
	OUT	SPH,R17
	OUT	SPL,R16

;------------------------------------------------
;	Blink LED1/3 and LED2/4
;------------------------------------------------
	LDI	R16,0xFF		; initialize PORTB = output
	OUT	DDRB,R16

LOOP:	LDI	R16,0b01010000		; LED1,3 on
	OUT	PORTB,R16
	CALL	D500MS
	LDI	R16,0b10100000		; LED2,4 on
	OUT	PORTB,R16
	CALL	D500MS
	JMP	LOOP

;------------------------------------------------
;	Delay Subroutine
;------------------------------------------------
D500MS: RCALL	D100MS			; delay 500ms
	RCALL	D200MS
	RCALL	D200MS
	RET

D100MS: LDI	R18,100
	RJMP	BASE1MS
D200MS: LDI	R18,200
BASE1MS:RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	RCALL	D200US			; 200 us
	DEC	R18
	BRNE	BASE1MS			; (total = 1 ms)
	RET

D200US: LDI	R19,200			; delay 200us
BASE1US:NOP				; 1
	PUSH	R19			; 2
	POP	R19			; 2
	PUSH	R19			; 2
	POP	R19			; 2
	PUSH	R19			; 2
	POP	R19			; 2
	DEC	R19			; 1
	BRNE	BASE1US			; 2 (total 16 cycles = 1 us)
	RET
