;********************************************************************
;*	Music Play Subroutines for OK-2561 V1.0 Training Kit	    *
;********************************************************************
;------------------------------------------------
;	Set Tempo and Initialize Timer/Couner3
;------------------------------------------------
SET_TEMPO:
	MOV	DH,AL			; store tempo(from 1 to 7)
	LDI	AL,0b01000000		; set CTC mode(12)
	STS	TCCR3A,AL
	LDI	AL,0b00011000		; speaker off
	STS	TCCR3B,AL
	LDI	AL,0b00000000
	STS	TCCR3C,AL
	RET

;====================================================================
;	Subroutine to Make Note
;====================================================================
;------------------------------------------------
;	Note of Octave 2 (Very Low Octave)
;------------------------------------------------
VLOA:	LDI	AH,high(9008)		; note A
	LDI	AL,low(9008)
	CALL	SET_TIMER3
	RET
VLOAX:	LDI	AH,high(8580)		; note A#
	LDI	AL,low(8580)
	CALL	SET_TIMER3
	RET
VLOB:	LDI	AH,high(8098)		; note B
	LDI	AL,low(8098)
	CALL	SET_TIMER3
	RET

;------------------------------------------------
;	Note of Octave 3 (Low Octave)
;------------------------------------------------
LOC:	LDI	AH,high(7644)		; note C
	LDI	AL,low(7644)
	CALL	SET_TIMER3
	RET
LOCX:	LDI	AH,high(7214)		; note C#
	LDI	AL,low(7214)
	CALL	SET_TIMER3
	RET
LOD:	LDI	AH,high(6810)		; note D
	LDI	AL,low(6810)
	CALL	SET_TIMER3
	RET
LODX:	LDI	AH,high(6427)		; note D#
	LDI	AL,low(6427)
	CALL	SET_TIMER3
	RET
LOE:	LDI	AH,high(6066)		; note E
	LDI	AL,low(6066)
	CALL	SET_TIMER3
	RET
LOF:	LDI	AH,high(5726)		; note F
	LDI	AL,low(5726)
	CALL	SET_TIMER3
	RET
LOFX:	LDI	AH,high(5404)		; note F#
	LDI	AL,low(5404)
	CALL	SET_TIMER3
	RET
LOG:	LDI	AH,high(5101)		; note G
	LDI	AL,low(5101)
	CALL	SET_TIMER3
	RET
LOGX:	LDI	AH,high(4815)		; note G#
	LDI	AL,low(4815)
	CALL	SET_TIMER3
	RET
LOA:	LDI	AH,high(4544)		; note A
	LDI	AL,low(4544)
	CALL	SET_TIMER3
	RET
LOAX:	LDI	AH,high(4289)		; note A#
	LDI	AL,low(4289)
	CALL	SET_TIMER3
	RET
LOB:	LDI	AH,high(4049)		; note B
	LDI	AL,low(4049)
	CALL	SET_TIMER3
	RET

;------------------------------------------------
;	Note of Octave 4 (Middle Octave)
;------------------------------------------------
MIC:	LDI	AH,high(3821)		; note C
	LDI	AL,low(3821)
	CALL	SET_TIMER3
	RET
MICX:	LDI	AH,high(3607)		; note C#
	LDI	AL,low(3607)
	CALL	SET_TIMER3
	RET
MID:	LDI	AH,high(3404)		; note D
	LDI	AL,low(3404)
	CALL	SET_TIMER3
	RET
MIDX:	LDI	AH,high(3213)		; note D#
	LDI	AL,low(3213)
	CALL	SET_TIMER3
	RET
MIE:	LDI	AH,high(3033)		; note E
	LDI	AL,low(3033)
	CALL	SET_TIMER3
	RET
MIF:	LDI	AH,high(2862)		; note F
	LDI	AL,low(2862)
	CALL	SET_TIMER3
	RET
MIFX:	LDI	AH,high(2702)		; note F#
	LDI	AL,low(2702)
	CALL	SET_TIMER3
	RET
MIG:	LDI	AH,high(2550)		; note G
	LDI	AL,low(2550)
	CALL	SET_TIMER3
	RET
MIGX:	LDI	AH,high(2407)		; note G#
	LDI	AL,low(2407)
	CALL	SET_TIMER3
	RET
MIA:	LDI	AH,high(2272)		; note A
	LDI	AL,low(2272)
	CALL	SET_TIMER3
	RET
MIAX:	LDI	AH,high(2144)		; note A#
	LDI	AL,low(2144)
	CALL	SET_TIMER3
	RET
MIB:	LDI	AH,high(2024)		; note B
	LDI	AL,low(2024)
	CALL	SET_TIMER3
	RET

;------------------------------------------------
;	Note of Octave 5 (High Octave)
;------------------------------------------------
HIC:	LDI	AH,high(1910)		; note C
	LDI	AL,low(1910)
	CALL	SET_TIMER3
	RET
HICX:	LDI	AH,high(1803)		; note C#
	LDI	AL,low(1803)
	CALL	SET_TIMER3
	RET
HID:	LDI	AH,high(1702)		; note D
	LDI	AL,low(1702)
	CALL	SET_TIMER3
	RET
HIDX:	LDI	AH,high(1606)		; note D#
	LDI	AL,low(1606)
	CALL	SET_TIMER3
	RET
HIE:	LDI	AH,high(1516)		; note E
	LDI	AL,low(1516)
	CALL	SET_TIMER3
	RET
HIF:	LDI	AH,high(1431)		; note F
	LDI	AL,low(1431)
	CALL	SET_TIMER3
	RET
HIFX:	LDI	AH,high(1350)		; note F#
	LDI	AL,low(1350)
	CALL	SET_TIMER3
	RET
HIG:	LDI	AH,high(1275)		; note G
	LDI	AL,low(1275)
	CALL	SET_TIMER3
	RET
HIGX:	LDI	AH,high(1203)		; note G#
	LDI	AL,low(1203)
	CALL	SET_TIMER3
	RET
HIA:	LDI	AH,high(1135)		; note A
	LDI	AL,low(1135)
	CALL	SET_TIMER3
	RET
HIAX:	LDI	AH,high(1072)		; note A#
	LDI	AL,low(1072)
	CALL	SET_TIMER3
	RET
HIB:	LDI	AH,high(1011)		; note B
	LDI	AL,low(1011)
	CALL	SET_TIMER3
	RET

;------------------------------------------------
;	Note of Octave 6 (Very High Octave)
;------------------------------------------------
VHIC:	LDI	AH,high(955)		; note C
	LDI	AL,low(955)
	CALL	SET_TIMER3
	RET
VHICX:	LDI	AH,high(901)		; note C#
	LDI	AL,low(901)
	CALL	SET_TIMER3
	RET
VHID:	LDI	AH,high(850)		; note D
	LDI	AL,low(850)
	CALL	SET_TIMER3
	RET
VHIDX:	LDI	AH,high(803)		; note D#
	LDI	AL,low(803)
	CALL	SET_TIMER3
	RET
VHIE:	LDI	AH,high(757)		; note E
	LDI	AL,low(757)
	CALL	SET_TIMER3
	RET

;------------------------------------------------
;	Set Timer/Counter3
;------------------------------------------------
SET_TIMER3:
	STS	ICR3H,AH		; set ICR3
	STS	ICR3L,AL
	LDI	AL,0			; clear TCNT3
	STS	TCNT3H,AL
	STS	TCNT3L,AL
	LDI	AL,0b00011010		; start CTC mode(prescaler = 8)
	STS	TCCR3B,AL
	RET

; ===================================================================
;	Subroutine for Length of Note and Rest
; ===================================================================
NOTE32:
REST32:	LDI	CH,1*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE16:
REST16:	LDI	CH,2*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE16D:
REST16D:LDI	CH,3*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE16T:LDI	CH,2*2			; delay for note(triple)
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE8:
REST8:	LDI	CH,4*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE8D:
REST8D:	LDI	CH,6*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE8T: LDI	CH,4*2			; delay for note(triple)
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE4:
REST4:	LDI	CH,8*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE4D:
REST4D:	LDI	CH,12*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE4T: LDI	CH,8*2			; delay for note(triple)
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE2:
REST2:	LDI	CH,16*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE2D:
REST2D:	LDI	CH,24*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET
NOTE1:
REST1:	LDI	CH,32*3			; delay for note or rest
	CALL	PLAY
	CALL	SPEAKER_OFF		; speaker off
	RET

;------------------------------------------------
;	Note & Rest Play Time with Base Time 21 ms
;------------------------------------------------
PLAY:	MOV	CL,DH			; play time = CH * tempo * 7 ms
BASETIME:CALL	D7MS
	DEC	CL
	BRNE	BASETIME
	DEC	CH
	BRNE	PLAY
	RET

;------------------------------------------------
;	Delay for 7 ms
;------------------------------------------------
D7MS:	PUSH	DELAY_COUNT
	LDI	DELAY_COUNT,7
	JMP	BASE1MS

;------------------------------------------------
;	Speaker Off
;------------------------------------------------
SPEAKER_OFF:
	LDI	AL,0b00011000		; speaker off
	STS	TCCR3B,AL
	CALL	D1MS			; delay for 1 ms
	RET

