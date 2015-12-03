/* ========================================================================== */
/*	             Exp09_1.c : Music Play Program	                      */
/* ========================================================================== */
/*			  Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"

#define	VLOA	9008			// define note, octave 2
#define	VLOAX	8580
#define	VLOB	8098
#define	LOC	7644			// define note, octave 3
#define	LOCX	7214
#define	LOD	6810
#define	LODX	6427
#define	LOE	6066
#define	LOF	5726
#define	LOFX	5404
#define	LOG	5101
#define	LOGX	4815
#define	LOA	4544
#define	LOAX	4289
#define	LOB	4049
#define	MIC	3821			// define note, octave 4
#define	MICX	3607
#define	MID	3404
#define	MIDX	3213
#define	MIE	3033
#define	MIF	2862
#define	MIFX	2702
#define	MIG	2550
#define	MIGX	2407
#define	MIA	2272
#define	MIAX	2144
#define	MIB	2024
#define	HIC	1910			// define note, octave 5
#define	HICX	1803
#define	HID	1702
#define	HIDX	1606
#define	HIE	1516
#define	HIF	1431
#define	HIFX	1350
#define	HIG	1275
#define	HIGX	1203
#define	HIA	1135
#define	HIAX	1072
#define	HIB	1011
#define	VHIC	955			// define note, octave 6
#define	VHICX	901
#define	VHID	850
#define	VHIDX	803
#define	VHIE	757

#define	NOTE32	1*3			// define note length
#define	NOTE16	2*3
#define	NOTE16D	3*3
#define	NOTE16T	2*2
#define	NOTE8	4*3
#define	NOTE8D	6*3
#define	NOTE8T	4*2
#define	NOTE4	8*3
#define	NOTE4D	12*3
#define	NOTE4T	8*2
#define	NOTE2	16*3
#define	NOTE2D	24*3
#define	NOTE1	32*3

#define	REST32	1*3			// define rest length
#define	REST16	2*3
#define	REST16D	3*3
#define	REST8	4*3
#define	REST8D	6*3
#define	REST4	8*3
#define	REST4D	12*3
#define	REST2	16*3
#define	REST2D	24*3
#define	REST1	32*3

unsigned char tempo;

void Set_tempo(unsigned char number)            /* set tempo */
{
  tempo = number;                               // set tempo
  TCCR3A = 0x40;                                // CTC mode(12), use OC3A
  TCCR3B = 0x18;                                // speaker off
  TCCR3C = 0x00;
}

void Play_note(unsigned int sound, unsigned int note) /* play note */
{
  ICR3 = sound;					// set ICR3
  TCNT3 = 0x0000;                               // clear Timer/Counter3
  TCCR3B = 0x1A;                                // start CTC mode(prescaler = 8)

  Delay_ms(note*tempo*7);                       // note play time

  TCCR3B = 0x18;                                // speaker off
}

void Play_rest(unsigned int rest)               /* play rest */
{
  Delay_ms(rest*tempo*7);                       // rest play time
}

int main(void)
{
  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module

  LCD_string(0x80," * MUSIC PLAY * ");          // display title

  while(1)
    { LCD_string(0xC0,"PongdangPongdang");	// display music title
      Set_tempo(4);				// set tempo

      Play_note(MIC,NOTE8);	             	// line 1
      Play_note(MID,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIC,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIG,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIC,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIC,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIG,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIA,NOTE4);      		// line 2
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIA,NOTE4);
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MIC,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIG,NOTE4);
      Play_note(MIA,NOTE8D);	            	// line 3
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_rest(REST8);
      Play_note(MIG,NOTE8D);
      Play_note(MIG,NOTE16);
      Play_note(MIE,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MIC,NOTE4D);
      Play_rest(REST8);
      Play_note(MID,NOTE8D);	            	// line 4
      Play_note(MIE,NOTE16);
      Play_note(MIC,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIG,NOTE4);
      Play_note(MID,NOTE8);
      Play_note(MIE,NOTE16);
      Play_note(MIF,NOTE16);
      Play_note(MIE,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MIC,NOTE4D);
      Play_rest(REST8);
      Delay_ms(1000);

      LCD_string(0xC0,"     Age 18     ");	// display music title
      Set_tempo(3);				// set tempo

      Play_note(MIG,NOTE4D);	            	// line 1
      Play_note(MIG,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIC,NOTE4);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIC,NOTE8);
      Play_note(MIB,NOTE8);
      Play_note(MIA,NOTE2);
      Play_note(MIG,NOTE4D);	            	// line 2
      Play_note(MIG,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIC,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MID,NOTE2);
      Play_note(MIG,NOTE4D);	            	// line 3
      Play_note(MIG,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIC,NOTE4);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIC,NOTE8);
      Play_note(MIB,NOTE8);
      Play_note(MIA,NOTE2);
      Play_note(MIG,NOTE8);	             	// line 4
      Play_note(HIE,NOTE4);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8D);
      Play_note(HIC,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE2);
      Play_note(MIG,NOTE4D);	            	// line 5
      Play_note(HIE,NOTE8);
      Play_note(HIC,NOTE4);
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIC,NOTE2D);
      Play_rest(REST8);
      Play_note(MIG,NOTE4D);	            	// line 6
      Play_note(HIG,NOTE8);
      Play_note(HIE,NOTE4);
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(HIC,NOTE2D);
      Play_note(MIG,NOTE4D);	            	// line 7
      Play_note(MIG,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIC,NOTE4);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIC,NOTE8);
      Play_note(MIB,NOTE8);
      Play_note(MIA,NOTE2);
      Play_note(MIG,NOTE8);	             	// line 8
      Play_note(HIE,NOTE4);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8D);
      Play_note(HIC,NOTE16);
      Play_note(MIG,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIC,NOTE2);
      Delay_ms(1000);

      LCD_string(0xC0,"Triangle Rotary ");	// display music title
      Set_tempo(5);				// set tempo

      Play_note(MIC,NOTE4);	             	// prelude
      Play_note(MIC,NOTE16);
      Play_note(MID,NOTE16);
      Play_note(MIE,NOTE16);
      Play_note(MIG,NOTE16);
      Play_note(HIE,NOTE4D);
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE4);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE16);
      Play_note(MIE,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE2);
      Play_note(HIC,NOTE8);
      Play_note(MIA,NOTE16);
      Play_note(HIC,NOTE16);
      Play_note(MIA,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIE,NOTE4);
      Play_note(MIG,NOTE16);
      Play_note(MIE,NOTE16);
      Play_note(MID,NOTE16);
      Play_note(MIE,NOTE16);
      Play_note(MIC,NOTE2D);
      Play_rest(REST8);	                	// line 1
      Play_note(MIC,NOTE8);
      Play_note(MIC,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MIE,NOTE4);
      Play_note(MIG,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIG,NOTE1);
      Play_note(MIE,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HIE,NOTE4);
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE2D);
      Play_rest(REST8);
      Play_note(HIE,NOTE4D);	            	// line 2
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE4);
      Play_note(MIA,NOTE16);
      Play_note(HIC,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(MIG,NOTE16);
      Play_rest(REST8);
      Play_note(MIE,NOTE16);
      Play_note(MIG,NOTE16);
      Play_note(MIA,NOTE16);
      Play_note(HIC,NOTE16);
      Play_note(MIA,NOTE8);
      Play_note(MIG,NOTE2);
      Play_rest(REST8);
      Play_note(LOA,NOTE8);
      Play_note(LOA,NOTE8);
      Play_note(MIC,NOTE8);
      Play_note(MID,NOTE4);
      Play_note(MIE,NOTE16);
      Play_note(MID,NOTE16);
      Play_note(MIC,NOTE16);
      Play_note(LOA,NOTE16);
      Play_note(MIC,NOTE2D);
      Play_rest(REST8);
      Play_note(MID,NOTE4D);	            	// line 3
      Play_note(MIE,NOTE8);
      Play_note(MID,NOTE4);
      Play_note(MIC,NOTE8);
      Play_note(MID,NOTE8);
      Play_rest(REST8);
      Play_note(MIE,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(HIC,NOTE2);
      Play_rest(REST8);
      Play_note(MIG,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(HIE,NOTE4);
      Play_note(HID,NOTE16T);
      Play_note(HIE,NOTE16T);
      Play_note(HID,NOTE16T);
      Play_note(HIC,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIE,NOTE2D);
      Play_rest(REST8);
      Play_note(HIF,NOTE4D);	            	// line 4
      Play_note(HIE,NOTE8);
      Play_note(HID,NOTE4);
      Play_note(HIC,NOTE4);
      Play_rest(REST8);
      Play_note(MIA,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE4);
      Play_note(MIA,NOTE8);
      Play_note(MIG,NOTE8);
      Play_rest(REST8);
      Play_note(MIE,NOTE16);
      Play_note(MIE,NOTE16);
      Play_note(MID,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIG,NOTE4);
      Play_note(MIE,NOTE16T);
      Play_note(MIF,NOTE16T);
      Play_note(MIE,NOTE16T);
      Play_note(MID,NOTE8);
      Play_note(MIC,NOTE2D);
      Delay_ms(1000);

      LCD_string(0xC0,"  Chil-Gab San  ");	// display music title
      Set_tempo(4);				// set tempo

      Play_note(HIG,NOTE16);	            	// prelude line 1
      Play_note(HIAX,NOTE16);
      Play_note(HIG,NOTE2);
      Play_note(HIG,NOTE16);
      Play_note(HIF,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIF,NOTE16);
      Play_note(HIG,NOTE16);
      Play_note(HIAX,NOTE16);
      Play_note(HIG,NOTE2D);
      Play_note(HID,NOTE16);
      Play_note(HIF,NOTE16);
      Play_note(HID,NOTE2);
      Play_note(HID,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIC,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIG,NOTE16);
      Play_note(HIF,NOTE16);
      Play_note(HID,NOTE2D);
      Play_rest(REST32);
      Play_note(MID,NOTE4D);	            	// prelude line 2
      Play_note(MID,NOTE8);
      Play_note(MIF,NOTE8);
      Play_note(MID,NOTE16);
      Play_note(MIF,NOTE16);
      Play_note(MIG,NOTE2);
      Play_note(MIAX,NOTE4);
      Play_note(MIG,NOTE2);
      Play_note(HIG,NOTE16);
      Play_note(HIF,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIF,NOTE16);
      Play_note(HIG,NOTE16);
      Play_note(HIAX,NOTE16);
      Play_note(HIG,NOTE2);
      Play_rest(REST8);
      Play_note(HID,NOTE4);	             	// line 1
      Play_note(HID,NOTE2);
      Play_note(HID,NOTE16);
      Play_note(HIF,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(HIC,NOTE16);
      Play_note(HID,NOTE2);
      Play_note(HIC,NOTE8);
      Play_note(MIAX,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(MIAX,NOTE8T);
      Play_note(MIA,NOTE8T);
      Play_note(MIG,NOTE8T);
      Play_note(MID,NOTE2);
      Play_rest(REST8);
      Play_note(MID,NOTE8);	             	// line 2
      Play_note(MIF,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIAX,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE2);
      Play_note(HIC,NOTE8T);
      Play_note(MIAX,NOTE8T);
      Play_note(HIC,NOTE8T);
      Play_note(HID,NOTE1);
      Play_rest(REST4);
      Play_note(HID,NOTE4);	             	// line 3
      Play_note(HID,NOTE4);
      Play_note(HIF,NOTE4);
      Play_note(HIG,NOTE2);
      Play_note(HIF,NOTE4);
      Play_note(HID,NOTE4);
      Play_note(HIF,NOTE4);
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE2);
      Play_rest(REST8);
      Play_note(HIC,NOTE8);	             	// line 4
      Play_note(MIAX,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(MIAX,NOTE8T);
      Play_note(MIA,NOTE8T);
      Play_note(MIG,NOTE8T);
      Play_note(MID,NOTE2);
      Play_note(MIAX,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIG,NOTE1);
      Play_rest(REST4);
      Play_note(MIA,NOTE4D);	            	// line 5
      Play_note(MIAX,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(MIAX,NOTE8);
      Play_note(MIA,NOTE4D);
      Play_note(HID,NOTE8);
      Play_note(HID,NOTE4);
      Play_note(MIG,NOTE4);
      Play_note(MIG,NOTE4);
      Play_note(MIG,NOTE8);
      Play_note(MIF,NOTE8);
      Play_note(MIG,NOTE2);
      Play_rest(REST4);
      Play_note(MIG,NOTE4);	             	// line 6
      Play_note(MIAX,NOTE4);
      Play_note(HIC,NOTE4);
      Play_note(HID,NOTE4D);
      Play_note(HIF,NOTE8);
      Play_note(HIG,NOTE8);
      Play_note(HIF,NOTE8);
      Play_note(HID,NOTE1);
      Play_rest(REST4);
      Play_note(HIG,NOTE4D);	            	// line 7
      Play_note(HIF,NOTE8);
      Play_note(HIG,NOTE8);
      Play_note(HIF,NOTE8);
      Play_note(HID,NOTE2D);
      Play_note(HIC,NOTE8);
      Play_note(MIAX,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(MIAX,NOTE8T);
      Play_note(MIA,NOTE8T);
      Play_note(MIG,NOTE8T);
      Play_note(MID,NOTE2);
      Play_rest(REST8);
      Play_note(MID,NOTE8);	             	// line 8
      Play_note(MIF,NOTE8);
      Play_note(MIG,NOTE8);
      Play_note(MIAX,NOTE8);
      Play_note(HIC,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(HIC,NOTE2);
      Play_note(MIAX,NOTE8T);
      Play_note(HIC,NOTE8T);
      Play_note(MIAX,NOTE8T);
      Play_note(MIG,NOTE1);
      Play_rest(REST1);
      Delay_ms(1000);

      LCD_string(0xC0,"   Empty Sky    ");	// display music title
      Set_tempo(4);				// set tempo

      Play_note(MIA,NOTE8);	             	// line 1
      Play_note(MIB,NOTE8);
      Play_note(MIFX,NOTE4D);
      Play_note(MIE,NOTE16);
      Play_note(MID,NOTE16);
      Play_note(MIFX,NOTE2D);
      Play_note(MIE,NOTE8);
      Play_note(MIFX,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(LOB,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(LOA,NOTE2D);
      Play_note(MID,NOTE8);	             	// line 2
      Play_note(LOA,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(MIE,NOTE8);
      Play_note(MIFX,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIB,NOTE2);
      Play_note(MIFX,NOTE8);
      Play_note(MIA,NOTE1);
      Play_rest(REST4);
      Play_note(MID,NOTE8);	             	// line 3
      Play_note(MID,NOTE8);
      Play_note(HID,NOTE4D);
      Play_note(HID,NOTE8);
      Play_note(HID,NOTE2D);
      Play_note(MIB,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(MIB,NOTE4D);
      Play_note(MIA,NOTE8);
      Play_note(MIFX,NOTE2D);
      Play_note(MIE,NOTE8);	             	// line 4
      Play_note(MIE,NOTE8);
      Play_note(MIE,NOTE4D);
      Play_note(MIB,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIFX,NOTE8);
      Play_note(MIFX,NOTE4D);
      Play_note(MIE,NOTE8);
      Play_note(MID,NOTE1);
      Play_rest(REST4);
      Play_note(HID,NOTE8);	             	// line 5
      Play_note(HIE,NOTE8);
      Play_note(HIE,NOTE4D);
      Play_note(HIE,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(HIE,NOTE8);
      Play_note(HIFX,NOTE2);
      Play_note(HIE,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(MIB,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(HIE,NOTE8);
      Play_note(HIFX,NOTE8);
      Play_note(HIE,NOTE2D);
      Play_note(HIFX,NOTE8);	            	// line 6
      Play_note(HIFX,NOTE8);
      Play_note(HIFX,NOTE4D);
      Play_note(HIE,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(HIE,NOTE16);
      Play_note(HID,NOTE16);
      Play_note(MIB,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIFX,NOTE4);
      Play_note(MIB,NOTE8);
      Play_note(HIE,NOTE8);
      Play_note(HID,NOTE4);
      Play_note(MIB,NOTE8);
      Play_note(HID,NOTE8);
      Play_note(MIA,NOTE2D);
      Play_note(LOA,NOTE8);	             	// line 7
      Play_note(LOB,NOTE8);
      Play_note(MID,NOTE4D);
      Play_note(MIE,NOTE8);
      Play_note(MIFX,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIA,NOTE2);
      Play_note(MID,NOTE8);
      Play_note(MID,NOTE8);
      Play_note(HID,NOTE4D);
      Play_note(HID,NOTE8);
      Play_note(MIB,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIFX,NOTE2);
      Play_note(MIA,NOTE8);	             	// line 8
      Play_note(HIFX,NOTE8);
      Play_note(HIFX,NOTE4);
      Play_note(HIFX,NOTE4);
      Play_note(HIE,NOTE4D);
      Play_note(HIFX,NOTE8);
      Play_note(MIA,NOTE8);
      Play_note(MIB,NOTE8);
      Play_note(HID,NOTE1);
      Play_rest(REST1);
      Delay_ms(1000);
    }
}

