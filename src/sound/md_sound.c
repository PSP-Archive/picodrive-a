
/*
**
** File: fm.c -- software implementation of Yamaha FM sound generator
**
** Copyright (C) 2001, 2002, 2003 Jarek Burczynski (bujar at mame dot net)
** Copyright (C) 1998 Tatsuyuki Satoh , MultiArcadeMachineEmulator development
**
** Version 1.4 (final beta) based but, cheated it.
**
*/

/*
** History:
**
** 03-08-2003 Jarek Burczynski:
**	- fixed YM2608 initial values (after the reset)
**	- fixed flag and irqmask handling (YM2608)
**	- fixed BUFRDY flag handling (YM2608)
*/

/***************************************************************************

  sn76496.c

  Routines to emulate the Texas Instruments SN76489 / SN76496 programmable
  tone /noise generator. Also known as (or at least compatible with) TMS9919.

  Noise emulation is not accurate due to lack of documentation. The noise
  generator uses a shift register with a XOR-feedback network, but the exact
  layout is unknown. It can be set for either period or white noise; again,
  the details are unknown.

***************************************************************************/

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdarg.h>
//#include <math.h>
#ifndef NULL
#define NULL 0
#endif
//#include "..\shared.h"
#include "md_sound.h"


#ifndef INT32
#define INT32 long
#endif

#ifndef UINT8
#define UINT8 unsigned char
#endif

#ifndef UINT32
#define UINT32 unsigned long
#endif

#ifndef INLINE
#define INLINE __inline
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

/* shared function building option */
#define BUILD_OPN (BUILD_YM2612)

#define PAN_L 0x80
#define PAN_R 0x40

///////////////////

#define MAX76_OUTPUT	0x7fff
//#define AUDIO_CONV(A) (A)

#define STEP76			0x10000
//#define DATATYPE76 unsigned short
//efine DATACONV76(A) ((A) / STEP76)
//#define DATACONV76(A) ((A)>>16)

/* Formulas for noise generator */
/* bit0 = output */

/* noise feedback for white noise mode */
#define FB_WNOISE		0x12000	/* bit15.d(16bits) = bit0(out) ^ bit2 */
//#define FB_WNOISE		0x14000 /* bit15.d(16bits) = bit0(out) ^ bit1 */
//#define FB_WNOISE		0x28000 /* bit16.d(17bits) = bit0(out) ^ bit2 (same to AY-3-8910) */
//#define FB_WNOISE		0x50000 /* bit17.d(18bits) = bit0(out) ^ bit2 */

/* noise feedback for periodic noise mode */
/* it is correct maybe (it was in the Megadrive sound manual) */
//#define FB_PNOISE		0x10000 /* 16bit rorate */
#define FB_PNOISE		0x08000	/* JH 981127 - fixes Do Run Run */

/* noise generator start preset (for periodic noise) */
#define NG_PRESET		0x0f35


//sn_Volume[]
static const int sn_VolTable[16] __attribute__((aligned(64))) ={
	3640,//364032(int)(256*14.22),//(256*15),//(256*14),// 0:ff
	2730,//273024(int)(192*14.22),//(192*15),//(192*14),// 1:c0
	1891,//189216(int)(133*14.22),//(133*15),//(133*14),// 2:85
	1422,//142200(int)(100*14.22),//(100*15),//(100*14),// 3:64
	 995,// 99540(int)( 70*14.22),//( 70*15),//( 70*14),// 4:46
	 739,// 73944(int)( 52*14.22),//( 52*15),//( 52*14),// 5:34
	 526,// 52614(int)( 37*14.22),//( 37*15),//( 37*14),// 6:25
	 384,// 38394(int)( 27*14.22),//( 27*15),//( 27*14),// 7:1b
	 270,// 27018(int)( 19*14.22),//( 19*15),//( 19*14),// 8:13
	 199,// 19908(int)( 14*14.22),//( 14*15),//( 14*14),// 9:0e
	 142,// 14220(int)( 10*14.22),//( 10*15),//( 10*14),//10:0a
	  99,//  9954(int)(  7*14.22),//(  7*15),//(  7*14),//11:07
	  71,//  7110(int)(  5*14.22),//(  5*15),//(  5*14),//12:05
	  57,//  5688(int)(  4*14.22),//(  4*15),//(  4*14),//13:04
	  28,//  2844(int)(  2*14.22),//(  2*15),//(  2*14),//14:02
	   0 //     0(int)(        0) //(     0) //(     0) //15:00
};
/* (  32767)/256/3/3 = 28.44357638/2 */
/* 7FFF =:= 7E00 = 256*42*3  */
/* 42/1.5 -> 28 */


//struct SN76496
//{

//static	int sn_SampleRate;
//static	int sn_VolTable[16];	/* volume table */

static	int sn_Volume[4];		/* volume of voice 0-2 and noise */
static	int sn_Period[4];
static	int sn_Count[4];
static	int sn_Output[4];

static	int sn_Register[8]; 	/* registers */

static	unsigned int sn_UpdateStep;
static	int sn_LastRegister;	/* last register written */
static	unsigned int sn_RNG;	/* noise generator */
static	int sn_NoiseFB; 		/* noise feedback mask */
//};
//static struct SN76496 sn[MAX_76496];

void SN76496Write(/*int chip,*/int data)
{
	//struct SN76496 *R = &sn[0/*chip*/];

	// レジスタ更新毎に音をレンダーすると、精度は最高かもしれないが、非常に重い。
	/* update the output buffer before changing the registers */
	///// commented out by starshine
	//stream_update(sn_Channel,0);

	if (data & 0x80)
	{
		int r = (data & 0x70) >> 4;
	//	int c = r/2;
		int c = (r>>1);	/* 違う音？気のせい？ */

		sn_LastRegister = r;
		sn_Register[r] = (sn_Register[r] & 0x3f0) | (data & 0x0f);
		switch (r)
		{
			case 0: /* tone 0 : frequency */
			case 2: /* tone 1 : frequency */
			case 4: /* tone 2 : frequency */
				sn_Period[c] = sn_UpdateStep * sn_Register[r];
				if (sn_Period[c] == 0) sn_Period[c] = sn_UpdateStep;
				if(r != 4)	break;
				//(r == 4) only
				/* update noise shift frequency */
				if ((sn_Register[6] & 0x03) == 0x03)
					sn_Period[3] = (sn_Period[2]+sn_Period[2]);
				break;
			case 1: /* tone 0 : volume */
			case 3: /* tone 1 : volume */
			case 5: /* tone 2 : volume */
			case 7: /* noise  : volume */
				sn_Volume[c] = sn_VolTable[data & 0x0f];
				break;
			case 6: /* noise  : frequency, mode */
				{
					/* N/512,N/1024,N/2048,Tone #3 output */
					switch(sn_Register[6]&3){
					case 0:	sn_Period[3] = (sn_UpdateStep << (5));		break;
					case 1:	sn_Period[3] = (sn_UpdateStep << (6));		break;
					case 2:	sn_Period[3] = (sn_UpdateStep << (7));		break;
					case 3: sn_Period[3] = (sn_Period[2]+sn_Period[2]);	break;
					}
					/* select noise */
					sn_NoiseFB = (sn_Register[6] & 4) ? FB_WNOISE : FB_PNOISE;
					/* reset noise shifter */
					sn_RNG = NG_PRESET;
					sn_Output[3] = sn_RNG & 1;
				}
				break;
		}
	}
	else
	{
		int r = sn_LastRegister;
	//	int c = r/2;
		int c = (r>>1);	/* 違う音？気のせい？ */

		switch (r)
		{
			case 0: /* tone 0 : frequency */
			case 2: /* tone 1 : frequency */
			case 4: /* tone 2 : frequency */
				sn_Register[r] = (sn_Register[r] & 0x0f) | ((data & 0x3f) << 4);
				sn_Period[c] = sn_UpdateStep * sn_Register[r];
				if (sn_Period[c] == 0) sn_Period[c] = sn_UpdateStep;
				if(r != 4)	break;
				//(r == 4) only
				/* update noise shift frequency */
				if ((sn_Register[6] & 0x03) == 0x03)
					sn_Period[3] = (sn_Period[2]+sn_Period[2]);
				break;
		}
	}
}

///////////////////
#define pL0 0
#define pL1 1
#define pL2 2
#define pL3 3
#define pL4 4
#define pL5 5
#define p_ddd 6
#define p_eee 7

#define pR0 8
#define pR1 9
#define pR2 10
#define pR3 11
#define pR4 12
#define pR5 13

static unsigned char panRL[16] __attribute__((aligned(64))) ;

/* globals */
//#define TYPE_SSG	  0x01	  /* SSG support		  */
//#define TYPE_LFOPAN 0x02	  /* OPN type LFO and PAN */
//#define TYPE_6CH	  0x04	  /* FM 6CH / 3CH		  */
//#define TYPE_DAC	  0x08	  /* YM2612's DAC device  */
//#define TYPE_ADPCM  0x10	  /* two ADPCM units	  */

//#define TYPE_YM2203 (TYPE_SSG)
//#define TYPE_YM2612 (TYPE_DAC |TYPE_LFOPAN |TYPE_6CH)

#define FREQ_SH 		16	/* 16.16 fixed point (frequency calculations) */
#define EG_SH			16	/* 16.16 fixed point (envelope generator timing) */
#define LFO_SH			24	/*	8.24 fixed point (LFO calculations) 	  */
#define TIMER_SH		16	/* 16.16 fixed point (timers calculations)	  */

#define FREQ_MASK		((1<<FREQ_SH)-1)

#define ENV_BITS		10
#define ENV_LEN 		(1<<ENV_BITS)
#define ENV_STEP		(128.0/ENV_LEN)

#define MAX_ATT_INDEX	(ENV_LEN-1) /* 1023 */
#define MIN_ATT_INDEX	(0) 		/* 0 */

#define EG_ATT			4
#define EG_DEC			3
#define EG_SUS			2
#define EG_REL			1
#define EG_OFF			0

#define SIN_BITS		10
#define SIN_LEN 		(1<<SIN_BITS)
#define SIN_MASK		(SIN_LEN-1)

#define TL_RES_LEN		(256) /* 8 bits addressing (real chip) */


/*	TL_TAB_LEN is calculated as:
*	13 - sinus amplitude bits	  (Y axis)
*	2  - sinus sign bit 		  (Y axis)
*	TL_RES_LEN - sinus resolution (X axis)
*/
#define TL_TAB_LEN (13*2*TL_RES_LEN)
//static signed int tl_tab[TL_TAB_LEN];

#define ENV_QUIET		(TL_TAB_LEN>>3)

/* sin waveform table in 'decibel' scale */
//static unsigned int sin_tab[SIN_LEN];

#include "_fm_tables.c"

/* sustain level table (3dB per step) */
/* bit0, bit1, bit2, bit3, bit4, bit5, bit6 */
/* 1,	 2,    4,	 8,    16,	 32,   64	(value)*/
/* 0.75, 1.5,  3,	 6,    12,	 24,   48	(dB)*/

/* 0 - 15: 0, 3, 6, 9,12,15,18,21,24,27,30,33,36,39,42,93 (dB)*/
#define SC(db) (UINT32) ( db * (4.0/ENV_STEP) )
static const UINT32 sl_table[16] __attribute__((aligned(64))) ={
 SC( 0),SC( 1),SC( 2),SC(3 ),SC(4 ),SC(5 ),SC(6 ),SC( 7),
 SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)
};
#undef SC


#define RATE_STEPS (8)


#define O(a) (a*RATE_STEPS)

/*note that there is no O(17) in this table - it's directly in the code */
static const UINT8 eg_rate_select[32+64+32] __attribute__((aligned(64))) ={	/* Envelope Generator rates (32 + 64 rates + 32 RKS) */
/* 32 infinite time rates */
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),

/* rates 00-11 */
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),

O( 4),O( 5),O( 6),O( 7),/* rate 12 */
O( 8),O( 9),O(10),O(11),/* rate 13 */
O(12),O(13),O(14),O(15),/* rate 14 */
O(16),O(16),O(16),O(16),/* rate 15 */

/* 32 dummy rates (same as 15 3) */
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16)

};
#undef O

/*rate	0,	  1,	2,	 3,   4,   5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15*/
/*shift 11,   10,	9,	 8,   7,   6,  5,  4,  3,  2, 1,  0,  0,  0,  0,  0 */
/*mask	2047, 1023, 511, 255, 127, 63, 31, 15, 7,  3, 1,  0,  0,  0,  0,  0 */

#define O(a) (a*1)
static const UINT8 eg_rate_shift[32+64+32] __attribute__((aligned(64))) ={	/* Envelope Generator counter shifts (32 + 64 rates + 32 RKS) */
/* 32 infinite time rates */
O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0),
O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0),
O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0),
O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0),

/* rates 00-11 */
O(11),O(11),O(11),O(11),
O(10),O(10),O(10),O(10),
O( 9),O( 9),O( 9),O( 9),
O( 8),O( 8),O( 8),O( 8),
O( 7),O( 7),O( 7),O( 7),
O( 6),O( 6),O( 6),O( 6),
O( 5),O( 5),O( 5),O( 5),
O( 4),O( 4),O( 4),O( 4),
O( 3),O( 3),O( 3),O( 3),
O( 2),O( 2),O( 2),O( 2),
O( 1),O( 1),O( 1),O( 1),
O( 0),O( 0),O( 0),O( 0),

O( 0),O( 0),O( 0),O( 0),/* rate 12 */
O( 0),O( 0),O( 0),O( 0),/* rate 13 */
O( 0),O( 0),O( 0),O( 0),/* rate 14 */
O( 0),O( 0),O( 0),O( 0),/* rate 15 */

/* 32 dummy rates (same as 15 3) */
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0)

};
#undef O

#if 00
static const UINT8 dt_tab[4 * 32] __attribute__((aligned(64))) ={
/* this is YM2151 and YM2612 phase increment data (in 10.10 fixed point format)*/
/* FD=0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* FD=1 */
	0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
	2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
/* FD=2 */
	1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
	5, 6, 6, 7, 8, 8, 9,10,11,12,13,14,16,16,16,16,
/* FD=3 */
	2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
	8 , 8, 9,10,11,12,13,14,16,17,19,20,22,22,22,22
};
#endif

/* OPN key frequency number -> key code follow table */
/* fnum higher 4bit -> keycode lower 2bit */
static const UINT8 opn_fktable[16] =
{
0,	//0000
0,	//0001
0,	//0010
0,	//0011
0,	//0100
0,	//0101
0,	//0110
1,	//0111 :7
2,	//1000 :8
3,	//1001
3,	//1010
3,	//1011
3,	//1100
3,	//1101
3,	//1110
3};	//1111




/*There are 4 different LFO AM depths available, they are:
  0 dB, 1.4 dB, 5.9 dB, 11.8 dB
  Here is how it is generated (in EG steps):

  11.8 dB = 0, 2, 4, 6, 8, 10,12,14,16...126,126,124,122,120,118,....4,2,0
   5.9 dB = 0, 1, 2, 3, 4, 5, 6, 7, 8....63, 63, 62, 61, 60, 59,.....2,1,0
   1.4 dB = 0, 0, 0, 0, 1, 1, 1, 1, 2,...15, 15, 15, 15, 14, 14,.....0,0,0

  (1.4 dB is loosing precision as you can see)

  It's implemented as generator from 0..126 with step 2 then a shift
  right N times, where N is:
	8 for 0 dB
	3 for 1.4 dB
	1 for 5.9 dB
	0 for 11.8 dB
*/
//static const UINT8 lfo_ams_depth_shift[4] = {8, 3, 1, 0};



/*There are 8 different LFO PM depths available, they are:
  0, 3.4, 6.7, 10, 14, 20, 40, 80 (cents)

  Modulation level at each depth depends on F-NUMBER bits: 4,5,6,7,8,9,10
  (bits 8,9,10 = FNUM MSB from OCT/FNUM register)

  Here we store only first quarter (positive one) of full waveform.
  Full table (lfo_pm_table) containing all 128 waveforms is build
  at run (init) time.

  One value in table below represents 4 (four) basic LFO steps
  (1 PM step = 4 AM steps).

  For example:
   at LFO SPEED=0 (which is 108 samples per basic LFO step)
   one value from "lfo_pm_output" table lasts for 432 consecutive
   samples (4*108=432) and one full LFO waveform cycle lasts for 13824
   samples (32*432=13824; 32 because we store only a quarter of whole
			waveform in the table below)
*/
static const UINT8 lfo_pm_output[7*8][8] __attribute__((aligned(64))) ={ /* 7 bits meaningful (of F-NUMBER), 8 LFO output levels per one depth (out of 32), 8 LFO depths */
/* FNUM BIT 4: 000 0001xxxx */
/* DEPTH 0 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 1 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 2 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 3 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 4 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 5 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 6 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 7 */ {0,	0,	 0,   0,   1,	1,	 1,   1},

/* FNUM BIT 5: 000 0010xxxx */
/* DEPTH 0 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 1 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 2 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 3 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 4 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 5 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 6 */ {0,	0,	 0,   0,   1,	1,	 1,   1},
/* DEPTH 7 */ {0,	0,	 1,   1,   2,	2,	 2,   3},

/* FNUM BIT 6: 000 0100xxxx */
/* DEPTH 0 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 1 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 2 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 3 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 4 */ {0,	0,	 0,   0,   0,	0,	 0,   1},
/* DEPTH 5 */ {0,	0,	 0,   0,   1,	1,	 1,   1},
/* DEPTH 6 */ {0,	0,	 1,   1,   2,	2,	 2,   3},
/* DEPTH 7 */ {0,	0,	 2,   3,   4,	4,	 5,   6},

/* FNUM BIT 7: 000 1000xxxx */
/* DEPTH 0 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 1 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 2 */ {0,	0,	 0,   0,   0,	0,	 1,   1},
/* DEPTH 3 */ {0,	0,	 0,   0,   1,	1,	 1,   1},
/* DEPTH 4 */ {0,	0,	 0,   1,   1,	1,	 1,   2},
/* DEPTH 5 */ {0,	0,	 1,   1,   2,	2,	 2,   3},
/* DEPTH 6 */ {0,	0,	 2,   3,   4,	4,	 5,   6},
/* DEPTH 7 */ {0,	0,	 4,   6,   8,	8, 0xa, 0xc},

/* FNUM BIT 8: 001 0000xxxx */
/* DEPTH 0 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 1 */ {0,	0,	 0,   0,   1,	1,	 1,   1},
/* DEPTH 2 */ {0,	0,	 0,   1,   1,	1,	 2,   2},
/* DEPTH 3 */ {0,	0,	 1,   1,   2,	2,	 3,   3},
/* DEPTH 4 */ {0,	0,	 1,   2,   2,	2,	 3,   4},
/* DEPTH 5 */ {0,	0,	 2,   3,   4,	4,	 5,   6},
/* DEPTH 6 */ {0,	0,	 4,   6,   8,	8, 0xa, 0xc},
/* DEPTH 7 */ {0,	0,	 8, 0xc,0x10,0x10,0x14,0x18},

/* FNUM BIT 9: 010 0000xxxx */
/* DEPTH 0 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 1 */ {0,	0,	 0,   0,   2,	2,	 2,   2},
/* DEPTH 2 */ {0,	0,	 0,   2,   2,	2,	 4,   4},
/* DEPTH 3 */ {0,	0,	 2,   2,   4,	4,	 6,   6},
/* DEPTH 4 */ {0,	0,	 2,   4,   4,	4,	 6,   8},
/* DEPTH 5 */ {0,	0,	 4,   6,   8,	8, 0xa, 0xc},
/* DEPTH 6 */ {0,	0,	 8, 0xc,0x10,0x10,0x14,0x18},
/* DEPTH 7 */ {0,	0,0x10,0x18,0x20,0x20,0x28,0x30},

/* FNUM BIT10: 100 0000xxxx */
/* DEPTH 0 */ {0,	0,	 0,   0,   0,	0,	 0,   0},
/* DEPTH 1 */ {0,	0,	 0,   0,   4,	4,	 4,   4},
/* DEPTH 2 */ {0,	0,	 0,   4,   4,	4,	 8,   8},
/* DEPTH 3 */ {0,	0,	 4,   4,   8,	8, 0xc, 0xc},
/* DEPTH 4 */ {0,	0,	 4,   8,   8,	8, 0xc,0x10},
/* DEPTH 5 */ {0,	0,	 8, 0xc,0x10,0x10,0x14,0x18},
/* DEPTH 6 */ {0,	0,0x10,0x18,0x20,0x20,0x28,0x30},
/* DEPTH 7 */ {0,	0,0x20,0x30,0x40,0x40,0x50,0x60},

};

/* all 128 LFO PM waveforms */
static INT32 lfo_pm_table[128*8*32] __attribute__((aligned(64))) ; /* 128 combinations of 7 bits meaningful (of F-NUMBER), 8 LFO depths, 32 LFO output levels per one depth */





/* register number to channel number , slot offset */
//#define OPN_CHAN(N) (N&3)
//#define OPN_SLOT(N) ((N>>2)&3)

/* slot number */
#define SLOT1 0
#define SLOT2 2
#define SLOT3 1
#define SLOT4 3

/* bit0 = Right enable , bit1 = Left enable */
#define OUTD_RIGHT	1
#define OUTD_LEFT	2
#define OUTD_CENTER 3

/* struct describing a single operator (SLOT) */
typedef struct
{
	INT32	*DT;		/* detune		   :dt_tab[DT] */
	UINT32	ar; 		/* attack rate	*/
	UINT32	d1r;		/* decay rate	*/
	UINT32	d2r;		/* sustain rate */
	UINT32	rr; 		/* release rate */
	UINT32	mul;		/* multiple 	   :ML_TABLE[ML] */
	UINT8	KSR;		/* key scale rate  :3-KSR */
	UINT8	ksr;		/* key scale rate  :kcode>>(3-KSR) */
	UINT8	state;		/* EG: phase type */
	UINT8	dummy1;

	/* Phase Generator */
	UINT32	phase;		/* phase counter */
	UINT32	Incr;		/* phase step */

	/* Envelope Generator */
	UINT32	tl; 		/* total level: TL << 3 */
	INT32	volume; 	/* envelope counter */
	UINT32	sl; 		/* sustain level:sl_table[SL] */
	UINT32	vol_out;	/* current output from EG circuit (without AM from LFO) */

	UINT8	eg_sh_ar;	/*	(attack state) */
	UINT8	eg_sel_ar;	/*	(attack state) */
	UINT8	eg_sh_d1r;	/*	(decay state) */
	UINT8	eg_sel_d1r; /*	(decay state) */

	UINT8	eg_sh_d2r;	/*	(sustain state) */
	UINT8	eg_sel_d2r; /*	(sustain state) */
	UINT8	eg_sh_rr;	/*	(release state) */
	UINT8	eg_sel_rr;	/*	(release state) */

	UINT32	eg_sh_ar_val  ;
	UINT32	eg_sh_d1r_val ;
	UINT32	eg_sh_d2r_val ;
	UINT32	eg_sh_rr_val  ;

	UINT32	key;		/* 0=last key was KEY OFF, 1=KEY ON */

	/* LFO */
	UINT32	AMmask; 	/* AM enable flag */

//	UINT8	ssg;		/* SSG-EG waveform */
//	UINT8	ssgn;		/* SSG-EG negated output */
} FM_SLOT;

typedef struct
{
	FM_SLOT SLOT[4] __attribute__((aligned(64))) ;	/* four SLOTs (operators) */

	UINT8	ALGO;		/* algorithm */
	UINT8	FB; 		/* feedback shift */
	INT32	op1_out[2]; /* op1 output for feedback */

	INT32	*connect1;	/* SLOT1 output pointer */
	INT32	*connect3;	/* SLOT3 output pointer */
	INT32	*connect2;	/* SLOT2 output pointer */
	INT32	*connect4;	/* SLOT4 output pointer */

	INT32	*mem_connect;/* where to put the delayed sample (MEM) */
	INT32	mem_value;	/* delayed sample (MEM) value */

	INT32	pms;		/* channel PMS */
//	UINT8	ams;		/* channel AMS */
	UINT32	ams_val ;

	UINT32	fc; 		/* fnum,blk:adjusted to sample rate */
	UINT8	kcode;		/* key code:						*/
	UINT32	block_fnum; /* current blk/fnum value for this slot (can be different between slots of one channel in 3slot mode) */
} FM_CH;


typedef struct
{
	void *	param;		/* this chip parameter	*/
	int 	clock;		/* master clock  (Hz)	*/
	int 	rate;		/* sampling rate (Hz)	*/

	UINT32	mode;		/* mode  CSM / 3SLOT	*/

	int 	TA; 		/* timer a				*/
	int 	TAC;		/* timer a counter		*/
	int 	TBC;		/* timer b counter		*/
	/* local time tables */
//	INT32	dt_tab[8][32];/* DeTune table		*/

	UINT8	address;	/* address register 	*/
	UINT8	irq;		/* interrupt level		*/
	UINT8	irqmask;	/* irq mask 			*/
	UINT8	status; 	/* status flag			*/

	/* Extention Timer and IRQ handler */
//	FM_TIMERHANDLER Timer_Handler;
//	FM_IRQHANDLER	IRQ_Handler;

	UINT8	fn_h;		/* freq latch			*/
	UINT8	TB; 		/* timer b				*/

//	UINT8	prescaler_sel_dummy;/* prescaler selector */
//	UINT8	dummy2;

//	double	freqbase;	/* frequency base		*/
//	double	TimerBase;	/* Timer base time		*/
#if FM_BUSY_FLAG_SUPPORT
a	double	BusyExpire; /* ExpireTime of Busy clear */
#endif

//	const struct ssg_callbacks *SSG;
} FM_ST;

/****************************/
/* OPN unit 				*/
/****************************/

/* OPN 3slot struct */
typedef struct
{
	UINT32	fc[3] __attribute__((aligned(64))) ;			/* fnum3,blk3: calculated */
	UINT32	block_fnum[3];	/* current fnum value for this slot (can be different between slots of one channel in 3slot mode) */
	UINT8	fn_h;			/* freq3 latch */
	UINT8	kcode[3];		/* key code */
} FM_3SLOT;


	/* there are 2048 FNUMs that can be generated using FNUM/BLK registers
		but LFO works with one more bit of a precision so we really need 4096 elements */
static		UINT32	F2612_fn_table[4096] __attribute__((aligned(64))) ;	/* fnumber->increment counter */

static /*unsigned int*/INT32 F2612_dt_tab[8][32] __attribute__((aligned(64))) ;	// Detune table




//typedef struct
//{
//} FM_OPN;

/********************************/
/*		YM2612 local section	*/
/********************************/
/* here's the virtual YM2612 */
//typedef struct
//{
#ifdef _STATE_H
//static		UINT8		F2612_REGS[512];			/* registers			*/
#endif

//} YM2612;


/* current chip state */
static INT32	out_fm[6] __attribute__((aligned(64))) ;		/*8-> 6(0to5) outputs of working channels */
static UINT32	LFO_AM; 		/* runtime LFO calculations helper */
static INT32	LFO_PM; 		/* runtime LFO calculations helper */
static INT32	m2,c1,c2;		/* Phase Modulation input for operators 2,3,4 */
static INT32	mem;			/* one sample delay memory */

//atic void 	*cur_chip = 0;	/* pointer of current chip struct */
//static FM_ST	*State; 		/* basic status */
//static FM_CH	*cch[6];		/*8-> 6(0to5) pointer of FM channels */

	//FM_OPN		OPN;				/* OPN state			*/
static		FM_CH		F2612_CH[6];	/* channel state		*/
static		UINT8		F2612_addr_A1;	/* address line A1		*/

int   is_init_YM2612 = 0 ;

/* OPN/A/B common state */
static		UINT32	LFO_INC_TAB[8];	/* LFO FREQ table */

static		UINT32	F2612_eg_cnt;			/* global envelope generator counter */
static		UINT32	F2612_eg_timer; 		/* global envelope generator counter works at frequency = chipclock/64/3 */
static		UINT32	F2612_eg_timer_add; 	/* step of eg_timer */
static		UINT32	F2612_eg_timer_overflow;/* envelope generator timer overlfows every 3 samples (on real chip) */

	/* LFO */
static		UINT32	F2612_lfo_cnt;
static		UINT32	F2612_lfo_inc;

	/* dac output (YM2612) */
static		int 		F2612_dacen;
static		INT32		F2612_dacout;

//static	UINT8	F2612_type; 			/* chip type */
static		FM_ST	F2612_ST;				/* general state */
static		FM_3SLOT F2612_SL3; 			/* 3 slot mode state */
//static	FM_CH	*F2612_P_CH;			/* pointer of CH */
//static	unsigned int F2612_pan[6*2];	/* fm channels output masks (0xffffffff = enable) */


/*------------------****************------------------***************----------------*/
#ifdef USE_YM2612DAC
#undef USE_YM2612DAC
#endif

/* selected 'FM.C core DAC' or 'other emulator DAC'. eg DG EN was externed patch.  */
//#define USE_YM2612DAC
#ifdef USE_YM2612DAC
const static int dac2612[256] __attribute__((aligned(64))) ={
	/* it was fuzzy ( serious table was unkown now ) */
-127,-127,-126,-125,-125,-124,-123,-123,-122,-121,-121,-120,-120,-119,-119,-118,
-118,-117,-117,-117,-116,-116,-115,-115,-114,-114,-113,-113,-113,-112,-112,-111,
-111,-111,-110,-110,-109,-109,-109,-108,-108,-107,-107,-107,-106,-106,-105,-105,
-104,-104,-103,-103,-102,-102,-101,-101,-100,-100, -99, -99, -98, -98, -97, -97,
 -96, -95, -95, -94, -93, -93, -92, -91, -91, -90, -89, -89, -88, -87, -87, -86,
 -85, -85, -84, -83, -83, -82, -81, -81, -80, -79, -79, -78, -77, -76, -75, -74,
 -72, -71, -70, -69, -68, -67, -66, -65, -64, -63, -62, -61, -60, -59, -58, -57,
 -55, -54, -53, -52, -50, -48, -46, -44, -42, -38, -32, -30, -25, -20, -10,   0,

   0,  10,	20,  25,  30,  32,	38,  42,  44,  46,	48,  50,  52,  53,	54,  55,
  57,  58,	59,  60,  61,  62,	63,  64,  65,  66,	67,  68,  69,  70,	71,  72,
  74,  75,	76,  77,  78,  79,	79,  80,  81,  81,	82,  83,  83,  84,	85,  85,
  86,  87,	87,  88,  89,  89,	90,  91,  91,  92,	93,  93,  94,  95,	95,  96,

  97,  97,	98,  98,  99,  99, 100, 100, 101, 101, 102, 102, 103, 103, 104, 104,
 105, 105, 106, 106, 107, 107, 107, 108, 108, 109, 109, 109, 110, 110, 111, 111,
 111, 112, 112, 113, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 117, 118,
 118, 119, 119, 120, 120, 121, 121, 122, 123, 123, 124, 125, 125, 126, 127, 127
};
#endif



/* status set and IRQ handling */
INLINE void FM_STATUS_SET(FM_ST *ST,int flag)
{
	/* set status flag */
	ST->status |= flag;
	if ( !(ST->irq) && (ST->status & ST->irqmask) )
	{
		ST->irq = 1;
		/* callback user interrupt handler (IRQ is OFF to ON) */
		//if(ST->IRQ_Handler) (ST->IRQ_Handler)(ST->param,1);
	}
}

/* status reset and IRQ handling */
INLINE void FM_STATUS_RESET(FM_ST *ST,int flag)
{
	/* reset status flag */
	ST->status &=~flag;
	if ( (ST->irq) && !(ST->status & ST->irqmask) )
	{
		ST->irq = 0;
		/* callback user interrupt handler (IRQ is ON to OFF) */
		//if(ST->IRQ_Handler) (ST->IRQ_Handler)(ST->param,0);
	}
}

/* IRQ mask set */
INLINE void FM_IRQMASK_SET(FM_ST *ST,int flag)
{
	ST->irqmask = flag;
	/* IRQ handling check */
	FM_STATUS_SET(ST,0);
	FM_STATUS_RESET(ST,0);
}

/* OPN Mode Register Write */
INLINE void set_timers( FM_ST *ST, void *n, int v )
{
	/* b7 = CSM MODE */
	/* b6 = 3 slot mode */
	/* b5 = reset b */
	/* b4 = reset a */
	/* b3 = timer enable b */
	/* b2 = timer enable a */
	/* b1 = load b */
	/* b0 = load a */
	ST->mode = v;

	if( v & 0x20 )		FM_STATUS_RESET(ST,0x02);	/* reset Timer b flag */
	if( v & 0x10 )		FM_STATUS_RESET(ST,0x01);	/* reset Timer a flag */
	/* load b */
	if( v & 0x02 )
	{
		if( ST->TBC == 0 )
		{
			ST->TBC = ( 256-ST->TB)<<4;
			/* External timer handler */
//			if (ST->Timer_Handler) (ST->Timer_Handler)(n,1,ST->TBC,ST->TimerBase);
		}
	}
	else
	{	/* stop timer b */
		if( ST->TBC != 0 )
		{
			ST->TBC = 0;
//			if (ST->Timer_Handler) (ST->Timer_Handler)(n,1,0,ST->TimerBase);
		}
	}
	/* load a */
	if( v & 0x01 )
	{
		if( ST->TAC == 0 )
		{
			ST->TAC = (1024-ST->TA);
			/* External timer handler */
//			if (ST->Timer_Handler) (ST->Timer_Handler)(n,0,ST->TAC,ST->TimerBase);
		}
	}
	else
	{	/* stop timer a */
		if( ST->TAC != 0 )
		{
			ST->TAC = 0;
//			if (ST->Timer_Handler) (ST->Timer_Handler)(n,0,0,ST->TimerBase);
		}
	}
}


/* Timer A Overflow */
INLINE void TimerAOver(FM_ST *ST)
{
	/* set status (if enabled) */
	if(ST->mode & 0x04) FM_STATUS_SET(ST,0x01);
	/* clear or reload the counter */
	ST->TAC = (1024-ST->TA);
//	if (ST->Timer_Handler) (ST->Timer_Handler)(ST->param,0,ST->TAC,ST->TimerBase);
}
/* Timer B Overflow */
INLINE void TimerBOver(FM_ST *ST)
{
	/* set status (if enabled) */
	if(ST->mode & 0x08) FM_STATUS_SET(ST,0x02);
	/* clear or reload the counter */
	ST->TBC = ( 256-ST->TB)<<4;
//	if (ST->Timer_Handler) (ST->Timer_Handler)(ST->param,1,ST->TBC,ST->TimerBase);
}

#define FM_INTERNAL_TIMER 1
#if FM_INTERNAL_TIMER

// NTSC genesis master xtal(53.693175[MHz]) if cases the divide 7.5
/*
XTAL/7	 : 7670453
DG EN100  : 7520000 (mistake)
XTAL/7.5 : 7159090 (==68000 clock==YM clock, but timers div 2 ? )
*/
/* 7520/4096 = 1.8359375  */
/* ----- internal timer mode , update timer */
/* 44100x4096 = 180633600  */
/* 44100/60   = 735  */
/* 4850/4096  = 1.18408203125 */
/* (53693175/ 7.5)=68K	 7.159090*4096=29323.63264 */
/* (53693175/15  )=Z80	 3.579545*4096=14661.81632 */

/* PSP_DG EN:4850 ???  */

//  clock:7520000 was (7520000/44100 )/(6*24);//F2612_ST.freqbase = 1.18417737465356512975560594608214;
//  clock:7159090 was (7159090/44100 )/(6*24);//F2612_ST.freqbase = 1.12734473418997228521038044847569;

//  clock:7159090 was  4617.60403124212648022171831695641

//#define FREQ_QQQ_BASE 4850
//#define FREQ_QQQ_BASE (29323)
//#define FREQ_QQQ_BASE (14661)
#define FREQ_QQQ_BASE (4618)

/* ---------- calculate timer A ---------- */
	#define INTERNAL_TIMER_A(ST,CSM_CH) 				\
	{													\
		if( ST->TAC /*&& (ST->Timer_Handler==0)*/ )		\
			if( (ST->TAC -= FREQ_QQQ_BASE /*(int)(ST->freqbase*4096)*/ ) <= 0 ) \
			{											\
				TimerAOver( ST );						\
				/* CSM mode total level latch and auto key on */	\
				if( ST->mode & 0x80 )					\
					CSMKeyControll( CSM_CH );			\
			}											\
	}
/* ---------- calculate timer B ---------- */
	#define INTERNAL_TIMER_B(ST,step)						\
	{														\
		if( ST->TBC /*&& (ST->Timer_Handler==0)*/ ) 			\
			if( (ST->TBC -= (FREQ_QQQ_BASE*step)/*(int)(ST->freqbase*4096*step)*/ ) <= 0 )	\
				TimerBOver( ST );							\
	}
#else /* FM_INTERNAL_TIMER */
/* external timer mode */
#define INTERNAL_TIMER_A(ST,CSM_CH)
#define INTERNAL_TIMER_B(ST,step)
#endif /* FM_INTERNAL_TIMER */



#if FM_BUSY_FLAG_SUPPORT
INLINE UINT8 FM_STATUS_FLAG(FM_ST *ST)
{
	if( ST->BusyExpire )
	{
		if( (ST->BusyExpire - FM_GET_TIME_NOW()) > 0)
			return ST->status | 0x80;	/* with busy */
		/* expire */
		ST->BusyExpire = 0;
	}
	return ST->status;
}
INLINE void FM_BUSY_SET(FM_ST *ST,int busyclock )
{
	ST->BusyExpire = FM_GET_TIME_NOW() + (ST->TimerBase * busyclock);
}
#define FM_BUSY_CLEAR(ST) ((ST)->BusyExpire = 0)
#else
#define FM_STATUS_FLAG(ST) ((ST)->status)
#define FM_BUSY_SET(ST,bclock) {}
#define FM_BUSY_CLEAR(ST) {}
#endif




INLINE void FM_KEYON(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( !SLOT->key )
	{
		SLOT->key	= 1;
		SLOT->phase = 0;		/* restart Phase Generator */
		SLOT->state = EG_ATT;	/* phase -> Attack */
	}
}

INLINE void FM_KEYOFF(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( SLOT->key )
	{
		SLOT->key = 0;
		if (SLOT->state>EG_REL)
			SLOT->state = EG_REL;/* phase -> Release */
	}
}

/* set algorithm connection */
static void setup_connection( /*unsigned char*/int ch )
{
	FM_CH *CH		= &F2612_CH[ch];

	INT32 *carrier	= &out_fm[ch];

	INT32 **om1 	= &CH->connect1;
	INT32 **om2 	= &CH->connect3;
	INT32 **oc1 	= &CH->connect2;

	INT32 **memc	= &CH->mem_connect;

	switch( CH->ALGO ){
	case 0:
		/* M1---C1---MEM---M2---C2---OUT */
		*om1 = &c1;
		*oc1 = &mem;	//(m2)feedback
		*om2 = &c2;
		*memc= &m2;
		break;
	case 1:
		/* M1------+-MEM---M2---C2---OUT */
		/*		C1-+					 */
		*om1 = &mem;	//(m2)feedback
		*oc1 = &mem;	//(m2)feedback
		*om2 = &c2;
		*memc= &m2;
		break;
	case 2:
		/* M1-----------------+-C2---OUT */
		/*		C1---MEM---M2-+ 		 */
		*om1 = &c2;
		*oc1 = &mem;	//(m2)feedback
		*om2 = &c2;
		*memc= &m2;
		break;
	case 3:
		/* M1---C1---MEM------+-C2---OUT */
		/*				   M2-+ 		 */
		*om1 = &c1;
		*oc1 = &mem;	//(c2)feedback
		*om2 = &c2;
		*memc= &c2;
		break;
	case 4:
		/* M1---C1-+-OUT */
		/* M2---C2-+	 */
		/* MEM: not used */
		*om1 = &c1;
		*oc1 = carrier;
		*om2 = &c2;
//		*memc= &mem;	/* store it anywhere where it will not be used */
		break;
	case 5:
		/*	  +----C1----+	   */
		/* M1-+-MEM---M2-+-OUT */
		/*	  +----C2----+	   */
		*om1 = 0;	/* special case(0==special mark) */
		*oc1 = carrier;
		*om2 = carrier;
		*memc= &m2;
		break;
	case 6:
		/* M1---C1-+	 */
		/*		M2-+-OUT */
		/*		C2-+	 */
		/* MEM: not used */
		*om1 = &c1;
		*oc1 = carrier;
		*om2 = carrier;
//		*memc= &mem;	/* not used */
		break;
	case 7:
		/* M1-+ 	*/
		/* C1-+-OUT */
		/* M2-+ 	*/
		/* C2-+ 	*/
		/* MEM: not used*/
		*om1 = carrier;
		*oc1 = carrier;
		*om2 = carrier;
//		*memc= &mem;	/* not used */
		break;
	}
	CH->connect4 = carrier;
}


#if 0

INLINE signed int op_calc(UINT32 phase, unsigned int env, signed int pm)
{
UINT32 p;
	p = (env<<3) + sin_tab[ ( ((signed int)((phase & ~FREQ_MASK) + (pm<<15))) >> FREQ_SH ) & SIN_MASK ];
	if (p >= TL_TAB_LEN)		return 0;
	return tl_tab[p];
}
INLINE signed int op_calc1(UINT32 phase, unsigned int env, signed int pm)
{
UINT32 p;
	p = (env<<3) + sin_tab[ ( ((signed int)((phase & ~FREQ_MASK) + pm )) >> FREQ_SH ) & SIN_MASK ];
	if (p >= TL_TAB_LEN)		return 0;
	return tl_tab[p];
}
INLINE signed int op_calc10(UINT32 phase, unsigned int env)
{
UINT32 p;
	p = (env<<3) + sin_tab[ ( ((signed int)((phase & ~FREQ_MASK)      )) >> FREQ_SH ) & SIN_MASK ];
	if (p >= TL_TAB_LEN)		return 0;
	return tl_tab[p];
}

#else //0

INLINE signed int op_calc(UINT32 phase, unsigned int env, signed int pm)
{
	phase >>= FREQ_SH	   ;
	pm	  >>= FREQ_SH - 15 ;
	phase += pm ;
	phase &= SIN_MASK ;
	env = (env<<3) + sin_tab[ phase ];
	if (env >= TL_TAB_LEN){ return 0; }
	return tl_tab[env];
}



#endif //0


static const UINT8 eg_inc[19*8/*RATE_STEPS*/] __attribute__((aligned(64))) ={

/*cycle:0 1  2 3  4 5  6 7*/

/* 0 */ 0,1, 0,1, 0,1, 0,1, /* rates 00..11 0 (increment by 0 or 1) */
/* 1 */ 0,1, 0,1, 1,1, 0,1, /* rates 00..11 1 */
/* 2 */ 0,1, 1,1, 0,1, 1,1, /* rates 00..11 2 */
/* 3 */ 0,1, 1,1, 1,1, 1,1, /* rates 00..11 3 */

/* 4 */ 1,1, 1,1, 1,1, 1,1, /* rate 12 0 (increment by 1) */
/* 5 */ 1,1, 1,2, 1,1, 1,2, /* rate 12 1 */
/* 6 */ 1,2, 1,2, 1,2, 1,2, /* rate 12 2 */
/* 7 */ 1,2, 2,2, 1,2, 2,2, /* rate 12 3 */

/* 8 */ 2,2, 2,2, 2,2, 2,2, /* rate 13 0 (increment by 2) */
/* 9 */ 2,2, 2,4, 2,2, 2,4, /* rate 13 1 */
/*10 */ 2,4, 2,4, 2,4, 2,4, /* rate 13 2 */
/*11 */ 2,4, 4,4, 2,4, 4,4, /* rate 13 3 */

/*12 */ 4,4, 4,4, 4,4, 4,4, /* rate 14 0 (increment by 4) */
/*13 */ 4,4, 4,8, 4,4, 4,8, /* rate 14 1 */
/*14 */ 4,8, 4,8, 4,8, 4,8, /* rate 14 2 */
/*15 */ 4,8, 8,8, 4,8, 8,8, /* rate 14 3 */

/*16 */ 8,8, 8,8, 8,8, 8,8, /* rates 15 0, 15 1, 15 2, 15 3 (increment by 8) */
/*17 */ 16,16,16,16,16,16,16,16, /* rates 15 2, 15 3 for attack */
/*18 */ 0,0, 0,0, 0,0, 0,0, /* infinity rates for attack and decay(s) */
};

INLINE void advance_eg_channel(/*FM_OPN *OPN,*/ FM_SLOT *SLOT)
{
//	unsigned int out;
	unsigned int i;
	i = 4; /* four operators per channel */
	do
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			if ( !(F2612_eg_cnt & ((1<<SLOT->eg_sh_ar)-1) ) )
			{
				SLOT->volume +=
					(~SLOT->volume *
						(eg_inc[SLOT->eg_sel_ar + ((F2612_eg_cnt>>SLOT->eg_sh_ar)&7)])
					) >>4;
				if (SLOT->volume <= MIN_ATT_INDEX)
				{
					SLOT->volume = MIN_ATT_INDEX;
					SLOT->state = EG_DEC;
				}
			}
			break;
		case EG_DEC:	/* decay phase */
			if ( !(F2612_eg_cnt & ((1<<SLOT->eg_sh_d1r)-1) ) )
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d1r + ((F2612_eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if ( SLOT->volume >= SLOT->sl )
					SLOT->state = EG_SUS;
			}
			break;
		case EG_SUS:	/* sustain phase */
			if ( !(F2612_eg_cnt & ((1<<SLOT->eg_sh_d2r)-1) ) )
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((F2612_eg_cnt>>SLOT->eg_sh_d2r)&7)];
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;
		case EG_REL:	/* release phase */
			if ( !(F2612_eg_cnt & ((1<<SLOT->eg_sh_rr)-1) ) )
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((F2612_eg_cnt>>SLOT->eg_sh_rr)&7)];
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					SLOT->state = EG_OFF;
				}
			}
			break;
		}
		SLOT->vol_out = SLOT->tl + ((UINT32)SLOT->volume);
		SLOT++;
		i--;
	}while (i);
}


INLINE void advance_eg_channel_slot(/*FM_OPN *OPN,*/ FM_SLOT *SLOT )
{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
//			if ( !(F2612_eg_cnt & ((1<<SLOT->eg_sh_ar)-1) ) )
			if ( !(F2612_eg_cnt & SLOT->eg_sh_ar_val) )
			{
				SLOT->volume +=
					(~SLOT->volume *
						(eg_inc[SLOT->eg_sel_ar + ((F2612_eg_cnt>>SLOT->eg_sh_ar)&7)])
					) >>4;
				if (SLOT->volume <= MIN_ATT_INDEX)
				{
					SLOT->volume = MIN_ATT_INDEX;
					SLOT->state = EG_DEC;
				}
			}
			break;
		case EG_DEC:	/* decay phase */
			if ( !(F2612_eg_cnt & SLOT->eg_sh_d1r_val) ) //((1<<SLOT->eg_sh_d1r)-1) ) )
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d1r + ((F2612_eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if ( SLOT->volume >= SLOT->sl )
					SLOT->state = EG_SUS;
			}
			break;
		case EG_SUS:	/* sustain phase */
			if ( !(F2612_eg_cnt & SLOT->eg_sh_d2r_val) ) //((1<<SLOT->eg_sh_d2r)-1) ) )
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((F2612_eg_cnt>>SLOT->eg_sh_d2r)&7)];
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;
		case EG_REL:	/* release phase */
			if ( !(F2612_eg_cnt & SLOT->eg_sh_rr_val) ) //((1<<SLOT->eg_sh_rr)-1) ) )
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((F2612_eg_cnt>>SLOT->eg_sh_rr)&7)];
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					SLOT->state = EG_OFF;
				}
			}
			break;
		}
		SLOT->vol_out = SLOT->tl + ((UINT32)SLOT->volume);
}



#if 0
#define volume_calc(OP) ((OP)->vol_out + (AM & (OP)->AMmask))
INLINE void chan_calc(/*FM_OPN *OPN,*/ FM_CH *CH)
{
unsigned int eg_out;
UINT32 AM = LFO_AM >> CH->ams;

	m2 = c1 = c2 = mem = 0;

	*CH->mem_connect = CH->mem_value;	/* restore delayed sample (MEM) value to m2 or c2 */

	eg_out = volume_calc(&CH->SLOT[SLOT1]);
	{
		INT32 out = CH->op1_out[0] + CH->op1_out[1];
		CH->op1_out[0] = CH->op1_out[1];

		if( !CH->connect1 ){
			/* algorithm 5	*/
			mem = c1 = c2 = CH->op1_out[0];
		}else{
			/* other algorithms */
			*CH->connect1 += CH->op1_out[0];
		}

		CH->op1_out[1] = 0;
		if( eg_out < ENV_QUIET )	/* SLOT 1 */
		{
			if (!CH->FB)
				out=0;

			CH->op1_out[1] = op_calc1(CH->SLOT[SLOT1].phase, eg_out, (out<<CH->FB) );
		}
	}

	eg_out = volume_calc(&CH->SLOT[SLOT3]);
	if( eg_out < ENV_QUIET )		/* SLOT 3 */
		*CH->connect3 += op_calc(CH->SLOT[SLOT3].phase, eg_out, m2);

	eg_out = volume_calc(&CH->SLOT[SLOT2]);
	if( eg_out < ENV_QUIET )		/* SLOT 2 */
		*CH->connect2 += op_calc(CH->SLOT[SLOT2].phase, eg_out, c1);

	eg_out = volume_calc(&CH->SLOT[SLOT4]);
	if( eg_out < ENV_QUIET )		/* SLOT 4 */
		*CH->connect4 += op_calc(CH->SLOT[SLOT4].phase, eg_out, c2);


	/* store current MEM */
	CH->mem_value = mem;

	/* update phase counters AFTER output calculations */
	if(CH->pms)
	{


	/* add support for 3 slot mode */


		UINT32 block_fnum = CH->block_fnum;

		UINT32 fnum_lfo   = ((block_fnum & 0x7f0) >> 4) * 32 * 8;
		INT32  lfo_fn_table_index_offset = lfo_pm_table[ fnum_lfo + CH->pms + LFO_PM ];

		if (lfo_fn_table_index_offset)	/* LFO phase modulation active */
		{
			UINT8  blk;
			UINT32 fn;
			int kc,fc;

			block_fnum = block_fnum*2 + lfo_fn_table_index_offset;

			blk = (block_fnum&0x7000) >> 12;
			fn	= block_fnum & 0xfff;

			/* keyscale code */
			kc = (blk<<2) | opn_fktable[fn >> 8];
			/* phase increment counter */
			fc = F2612_fn_table[fn]>>(7-blk);

			CH->SLOT[SLOT1].phase += ((fc+CH->SLOT[SLOT1].DT[kc])*CH->SLOT[SLOT1].mul) >> 1;
			CH->SLOT[SLOT2].phase += ((fc+CH->SLOT[SLOT2].DT[kc])*CH->SLOT[SLOT2].mul) >> 1;
			CH->SLOT[SLOT3].phase += ((fc+CH->SLOT[SLOT3].DT[kc])*CH->SLOT[SLOT3].mul) >> 1;
			CH->SLOT[SLOT4].phase += ((fc+CH->SLOT[SLOT4].DT[kc])*CH->SLOT[SLOT4].mul) >> 1;
		}
		else	/* LFO phase modulation  = zero */
		{
			CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
			CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
			CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
			CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
		}
	}
	else	/* no LFO phase modulation */
	{
		CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
		CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
		CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
		CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
	}
}

#else
/* _PSP use */
#define volume_calc(ams,OP) ((OP)->vol_out + (ams & (OP)->AMmask))
static int F2612_ecount;




INLINE signed int op_calc1(UINT32 phase, unsigned int env, signed int pm)
{
	phase += pm ;
	phase >>= FREQ_SH  ;
	phase &= SIN_MASK ;
	env = (env<<3) + sin_tab[ phase ];
	if (env >= TL_TAB_LEN){ return 0; }
	return tl_tab[env];
}
INLINE signed int op_calc10(UINT32 phase, unsigned int env)
{
	phase >>= FREQ_SH ;
	phase &= SIN_MASK ;
	env = (env<<3) + sin_tab[ phase ];
	if (env >= TL_TAB_LEN){ return 0; }
	return tl_tab[env];
}
INLINE void chan_calc(/*FM_OPN *OPN,*/ FM_CH *CH)
{
unsigned int eg_out;	 /*envelope output*/
int i;
	for(i=0;i<F2612_ecount;i++){
		advance_eg_channel_slot( &CH->SLOT[SLOT1]);
		advance_eg_channel_slot( &CH->SLOT[SLOT2]);
		advance_eg_channel_slot( &CH->SLOT[SLOT3]);
		advance_eg_channel_slot( &CH->SLOT[SLOT4]);
	}

	/* Phase Generator */
	m2 = c1 = c2 = mem = 0;

	*CH->mem_connect = CH->mem_value;	/* restore delayed sample (MEM) value to m2 or c2 */

	/* Envelope Generator */
	eg_out = volume_calc(CH->ams_val, &CH->SLOT[SLOT1]);
	/* Connection */
	{
		if( eg_out < ENV_QUIET )	/* SLOT 1 */
		{
			if ( CH->FB )
			{
			signed int pm;
				pm = ( ( CH->op1_out[0] + CH->op1_out[1] )  << CH->FB);
				CH->op1_out[0] = CH->op1_out[1];

				if( !CH->connect1 ){
					/* algorithm 5	*/
					mem = c1 = c2 = CH->op1_out[0]; //m2 to mem
				}else{
					/* other algorithms */
					*CH->connect1 += CH->op1_out[0];
				}

				//CH->op1_out[1] = 0;
				CH->op1_out[1] = op_calc1(CH->SLOT[SLOT1].phase, eg_out, pm );
			}
			else
			{
				CH->op1_out[0] = CH->op1_out[1];

				if( !CH->connect1 ){
					/* algorithm 5	*/
					mem = c1 = c2 = CH->op1_out[0];
				}else{
					/* other algorithms */
					*CH->connect1 += CH->op1_out[0];
				}

				//CH->op1_out[1] = 0;
				CH->op1_out[1] = op_calc10(CH->SLOT[SLOT1].phase, eg_out );
			}
		}
		else
		{
			CH->op1_out[0] = CH->op1_out[1];

			if( !CH->connect1 )
			{
				/* algorithm 5	*/
				mem = c1 = c2 = CH->op1_out[0];
			}else{
				/* other algorithms */
				*CH->connect1 += CH->op1_out[0];
			}

			CH->op1_out[1] = 0;
		}
	}

#if 0
	eg_out = volume_calc(CH->ams_val, &CH->SLOT[SLOT3]);
	if( eg_out < ENV_QUIET )		/* SLOT 3 */
		*CH->connect3 += op_calc(CH->SLOT[SLOT3].phase, eg_out, m2);

	eg_out = volume_calc(CH->ams_val, &CH->SLOT[SLOT2]);
	if( eg_out < ENV_QUIET )		/* SLOT 2 */
		*CH->connect2 += op_calc(CH->SLOT[SLOT2].phase, eg_out, c1);

	eg_out = volume_calc(CH->ams_val, &CH->SLOT[SLOT4]);
	if( eg_out < ENV_QUIET )		/* SLOT 4 */
		*CH->connect4 += op_calc(CH->SLOT[SLOT4].phase, eg_out, c2);
#else

/* 何故かelse節の方が速い。投機実行の為？？？ */
	eg_out = CH->SLOT[SLOT3].vol_out;
	if( !(CH->SLOT[SLOT3].AMmask) ){;}else{eg_out += CH->ams_val;}
	if( eg_out >= ENV_QUIET ){;}else{*CH->connect3 += op_calc(CH->SLOT[SLOT3].phase, eg_out, m2);}	/* SLOT 3 */

	eg_out = CH->SLOT[SLOT2].vol_out;
	if( !(CH->SLOT[SLOT2].AMmask) ){;}else{eg_out += CH->ams_val;}
	if( eg_out >= ENV_QUIET ){;}else{*CH->connect2 += op_calc(CH->SLOT[SLOT2].phase, eg_out, c1);}	/* SLOT 2 */

	eg_out = CH->SLOT[SLOT4].vol_out;
	if( !(CH->SLOT[SLOT4].AMmask) ){;}else{eg_out += CH->ams_val;}
	if( eg_out >= ENV_QUIET ){;}else{*CH->connect4 += op_calc(CH->SLOT[SLOT4].phase, eg_out, c2);}	/* SLOT 4 */

#endif //0


	/* store current MEM */
	CH->mem_value = mem;

	/* update phase counters AFTER output calculations */
	if(CH->pms)
	{
	/* add support for 3 slot mode */
	UINT32 block_fnum = CH->block_fnum;
	UINT32 fnum_lfo   = ((block_fnum & 0x7f0) >> 4) * 32 * 8;
	INT32  lfo_fn_table_index_offset = lfo_pm_table[ fnum_lfo + CH->pms + LFO_PM ];
		if (lfo_fn_table_index_offset)	/* LFO phase modulation active */
		{
			UINT8  blk;
			UINT32 fn;
			int kc,fc;

			block_fnum += block_fnum;
			block_fnum += lfo_fn_table_index_offset;

			blk = (block_fnum&0x7000) >> 12;
			fn	= block_fnum & 0xfff;

			/* keyscale code */
			kc = (blk<<2) | opn_fktable[fn >> 8];
			/* phase increment counter */
			fc = F2612_fn_table[fn]>>(7-blk);

			CH->SLOT[SLOT1].phase += ((fc+CH->SLOT[SLOT1].DT[kc])*CH->SLOT[SLOT1].mul) >> 1;
			CH->SLOT[SLOT2].phase += ((fc+CH->SLOT[SLOT2].DT[kc])*CH->SLOT[SLOT2].mul) >> 1;
			CH->SLOT[SLOT3].phase += ((fc+CH->SLOT[SLOT3].DT[kc])*CH->SLOT[SLOT3].mul) >> 1;
			CH->SLOT[SLOT4].phase += ((fc+CH->SLOT[SLOT4].DT[kc])*CH->SLOT[SLOT4].mul) >> 1;
		}
		else	/* LFO phase modulation  = zero */
		{
			CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
			CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
			CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
			CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
		}
	}
	else	/* no LFO phase modulation */
	{
		CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
		CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
		CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
		CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
	}
}

#endif // 0


/* update phase increment and envelope generator */
INLINE void refresh_fc_eg_slot(FM_SLOT *SLOT , int fc , int kc )
{
	int ksr;

	/* (frequency) phase increment counter */
	SLOT->Incr = ((fc+SLOT->DT[kc])*SLOT->mul) >> 1;

	ksr = kc >> SLOT->KSR;
	if( SLOT->ksr != ksr )
	{
		SLOT->ksr = ksr;
		/* calculate envelope generator rates */
		if ((SLOT->ar + SLOT->ksr) < 32+62)
		{
			SLOT->eg_sh_ar	= eg_rate_shift [SLOT->ar  + SLOT->ksr ];
			SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + SLOT->ksr ];
		}
		else
		{
			SLOT->eg_sh_ar	= 0;
			SLOT->eg_sel_ar = 17*RATE_STEPS;
		}
		SLOT->eg_sh_ar_val	= ((1<<SLOT->eg_sh_ar)-1) ;

		SLOT->eg_sh_d1r 	= eg_rate_shift [SLOT->d1r + SLOT->ksr];
		SLOT->eg_sh_d1r_val = ((1<<SLOT->eg_sh_d1r)-1) ;
		SLOT->eg_sel_d1r	= eg_rate_select[SLOT->d1r + SLOT->ksr];

		SLOT->eg_sh_d2r 	= eg_rate_shift [SLOT->d2r + SLOT->ksr];
		SLOT->eg_sh_d2r_val = ((1<<SLOT->eg_sh_d2r)-1) ;
		SLOT->eg_sel_d2r	= eg_rate_select[SLOT->d2r + SLOT->ksr];

		SLOT->eg_sh_rr		= eg_rate_shift [SLOT->rr  + SLOT->ksr];
		SLOT->eg_sh_rr_val	= ((1<<SLOT->eg_sh_rr )-1) ;
		SLOT->eg_sel_rr 	= eg_rate_select[SLOT->rr  + SLOT->ksr];
	}
}

/* update phase increment counters */
INLINE void refresh_fc_eg_chan(FM_CH *CH )
{
	if( CH->SLOT[SLOT1].Incr==-1){
		int fc = CH->fc;
		int kc = CH->kcode;
		refresh_fc_eg_slot(&CH->SLOT[SLOT1] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT2] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT3] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT4] , fc , kc );
	}
}

static short dt_table[] =
{
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x004B,0x004B,0x004B,0x004B,
0x004B,0x004B,0x004B,0x004B,0x0097,0x0097,0x0097,0x0097,
0x0097,0x00E3,0x00E3,0x00E3,0x012F,0x012F,0x012F,0x017A,
0x017A,0x01C6,0x01C6,0x0212,0x025E,0x025E,0x025E,0x025E,
0x004B,0x004B,0x004B,0x004B,0x0097,0x0097,0x0097,0x0097,
0x0097,0x00E3,0x00E3,0x00E3,0x012F,0x012F,0x012F,0x017A,
0x017A,0x01C6,0x01C6,0x0212,0x025E,0x025E,0x02AA,0x02F5,
0x0341,0x038D,0x03D9,0x0425,0x04BC,0x04BC,0x04BC,0x04BC,
0x0097,0x0097,0x0097,0x0097,0x0097,0x00E3,0x00E3,0x00E3,
0x012F,0x012F,0x012F,0x017A,0x017A,0x01C6,0x01C6,0x0212,
0x025E,0x025E,0x02AA,0x02F5,0x0341,0x038D,0x03D9,0x0425,
0x04BC,0x0508,0x059F,0x05EB,0x0683,0x0683,0x0683,0x0683
} ;


static void reset_channels( FM_ST *ST , FM_CH *CH , int num )
{
	int c;
	int s;

	ST->mode   = 0; /* normal mode */
	ST->TA	   = 0;
	ST->TAC    = 0;
	ST->TB	   = 0;
	ST->TBC    = 0;

	panRL[pL0]=PAN_L;
	panRL[pL1]=PAN_L;
	panRL[pL2]=PAN_L;
	panRL[pL3]=PAN_L;
	panRL[pL4]=PAN_L;
	panRL[pL5]=PAN_L;

	panRL[pR0]=PAN_R;
	panRL[pR1]=PAN_R;
	panRL[pR2]=PAN_R;
	panRL[pR3]=PAN_R;
	panRL[pR4]=PAN_R;
	panRL[pR5]=PAN_R;

	for( c=0; c<num; c++ )
	{
		CH[c].ALGO = 0 ;
		setup_connection( c ) ;

		CH[c].fc = 0;
		for(s = 0 ; s < 4 ; s++ )
		{
			CH[c].SLOT[s].DT = F2612_dt_tab[0] ;
			CH[c].SLOT[s].state= EG_OFF;
			CH[c].SLOT[s].volume = MAX_ATT_INDEX;
			CH[c].SLOT[s].vol_out= MAX_ATT_INDEX;
		}
	}
}



/* CSM Key Controll */
INLINE void CSMKeyControll(FM_CH *CH)
{
	/* this is wrong, atm */

	/* all key on */
	FM_KEYON(CH,SLOT1);
	FM_KEYON(CH,SLOT2);
	FM_KEYON(CH,SLOT3);
	FM_KEYON(CH,SLOT4);
}

#ifdef _STATE_H
/* FM channel save , internal state only */
static void FMsave_state_channel(const char *name,int num,FM_CH *CH,int num_ch)
{
	int slot , ch;
	char state_name[20];
	const char slot_array[4] = { 1 , 3 , 2 , 4 };

	for(ch=0;ch<num_ch;ch++,CH++)
	{
		/* channel */
		sprintf(state_name,"%s.CH%d",name,ch);
		state_save_register_INT32(state_name, num, "feedback" , CH->op1_out , 2);
		state_save_register_UINT32(state_name, num, "phasestep"   , &CH->fc , 1);
		/* slots */
		for(slot=0;slot<4;slot++)
		{
			FM_SLOT *SLOT = &CH->SLOT[slot];

			sprintf(state_name,"%s.CH%d.SLOT%d",name,ch,slot_array[slot]);
			state_save_register_UINT32(state_name, num, "phasecount" , &SLOT->phase, 1);
			state_save_register_UINT8 (state_name, num, "state" 	 , &SLOT->state, 1);
			state_save_register_INT32 (state_name, num, "volume"	 , &SLOT->volume, 1);
		}
	}
}

static void FMsave_state_st(const char *state_name,int num,FM_ST *ST)
{
#if FM_BUSY_FLAG_SUPPORT
	state_save_register_double(state_name, num, "BusyExpire", &ST->BusyExpire , 1);
#endif
	state_save_register_UINT8 (state_name, num, "address"	, &ST->address , 1);
	state_save_register_UINT8 (state_name, num, "IRQ"		, &ST->irq	   , 1);
	state_save_register_UINT8 (state_name, num, "IRQ MASK"	, &ST->irqmask , 1);
	state_save_register_UINT8 (state_name, num, "status"	, &ST->status  , 1);
	state_save_register_UINT32(state_name, num, "mode"		, &ST->mode    , 1);
//	state_save_register_UINT8 (state_name, num, "prescaler" , &ST->prescaler_sel_dummy , 1);
	state_save_register_UINT8 (state_name, num, "freq latch", &ST->fn_h , 1);
	state_save_register_int   (state_name, num, "TIMER A"	, &ST->TA	);
	state_save_register_int   (state_name, num, "TIMER Acnt", &ST->TAC	);
	state_save_register_UINT8 (state_name, num, "TIMER B"	, &ST->TB	, 1);
	state_save_register_int   (state_name, num, "TIMER Bcnt", &ST->TBC	);
}
#endif /* _STATE_H */

#if BUILD_OPN




/* write a OPN mode register 0x20-0x2f */
static void OPNWriteMode(/*FM_OPN *OPN,*/ int r, int v)
{
	UINT8 c;

	switch(r){
//	case 0x21:	/* Test */
//		break;
	case 0x22:	/* LFO FREQ (YM2608/YM2610/YM2610B/YM2612) */
		//if( /*F2612_type*/TYPE_YM2612 & TYPE_LFOPAN )
		//{
			if (v&0x08) /* LFO enabled ? */
			{
				F2612_lfo_inc = LFO_INC_TAB[v&7];
			}
			else
			{
				F2612_lfo_inc = 0;
			}
		//}
		break;
	case 0x24:	F2612_ST.TA = (F2612_ST.TA & 0x03)|(((int)v)<<2);	break;	/* timer A High 8*/
	case 0x25:	F2612_ST.TA = (F2612_ST.TA & 0x3fc)|(v&3);			break;	/* timer A Low 2*/
	case 0x26:	F2612_ST.TB = v;									break;	/* timer B */
	case 0x27:	set_timers( &(F2612_ST),F2612_ST.param,v );			break;	/* mode, timer control */
	case 0x28:	/* key on / off */
		c = v & 0x03;
		if(3==c) break;
		if(v&0x04) c+=3;

		/*scopes*/
		{
		FM_CH *CH;
			CH = &F2612_CH[c];
			if(v&0x10) FM_KEYON(CH,SLOT1); else FM_KEYOFF(CH,SLOT1);
			if(v&0x20) FM_KEYON(CH,SLOT2); else FM_KEYOFF(CH,SLOT2);
			if(v&0x40) FM_KEYON(CH,SLOT3); else FM_KEYOFF(CH,SLOT3);
			if(v&0x80) FM_KEYON(CH,SLOT4); else FM_KEYOFF(CH,SLOT4);
		}
		break;
	}
}

/* [OPNWriteReg][_][_slot] write a OPN register (0x30-0xff) */
static void OPNWriteReg(/*FM_OPN *OPN,*/ int r, int v)
{
FM_CH *NOW_CH;
FM_SLOT *SLOT;
UINT8 c;
	c = (r&3);//OPN_CHAN
	/* 0x30 - 0xff */
	if(c == 3) return;		/* 0xX3,0xX7,0xXB,0xXF */
	if(r >= 0x100) c+=3;	/* the (F2612_type & TYPE_6CH) access. */

	NOW_CH = &F2612_CH[c];

	SLOT = &NOW_CH->SLOT[((r>>2)&3)];//OPN_SLOT((r>>2)&3)
	switch( r & 0xf0 ) {
	case 0x30:	/* DET , MUL */
		//set_det_mul(&F2612_ST,CH,SLOT,v);
		/* set detune & multiple */
		//INLINE void set_det_mul(FM_ST *ST,FM_CH *CH,FM_SLOT *SLOT,int v)
		SLOT->DT  = F2612_dt_tab[(v>>4)&7];
		v &= 0x0f;
		SLOT->mul = (v)? (v+v) : 1;
		NOW_CH->SLOT[SLOT1].Incr=-1;
		break;
	case 0x40:	/* TL */
		//set_tl(CH,SLOT,v);
		/* set total level */
		//INLINE void set_tl(FM_CH *CH,FM_SLOT *SLOT , int v)
		SLOT->tl = (v&0x7f)<<(ENV_BITS-7); /* 7bit TL */
		break;
	case 0x50:	/* KS, AR */
		//set_ar_ksr(CH,SLOT,v);
		/* set attack rate & key scale	*/
		//INLINE void set_ar_ksr(FM_CH *CH,FM_SLOT *SLOT,int v)
		{
		UINT8 old_KSR = SLOT->KSR;

			SLOT->ar = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

			SLOT->KSR = 3-(v>>6);
			if (SLOT->KSR != old_KSR)
			{
				NOW_CH->SLOT[SLOT1].Incr=-1;
			}
			else
			{
				/* refresh Attack rate */
				if ((SLOT->ar + SLOT->ksr) < 32+62)
				{
					SLOT->eg_sh_ar	= eg_rate_shift [SLOT->ar  + SLOT->ksr ];
					SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + SLOT->ksr ];
				}
				else
				{
					SLOT->eg_sh_ar	= 0;
					SLOT->eg_sel_ar = 17*RATE_STEPS;
				}
				SLOT->eg_sh_ar_val = ((1<<SLOT->eg_sh_ar)-1) ;
			}
		}
		break;
	case 0x60:	/* bit7 = AM ENABLE, DR */
		//set_dr(SLOT,v);
		/* set decay rate */
		//INLINE void set_dr(FM_SLOT *SLOT,int v)
			SLOT->d1r			= (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;
			SLOT->eg_sh_d1r 	= eg_rate_shift [SLOT->d1r + SLOT->ksr];
			SLOT->eg_sel_d1r	= eg_rate_select[SLOT->d1r + SLOT->ksr];
			SLOT->eg_sh_d1r_val = ((1<<SLOT->eg_sh_d1r)-1) ;
		//if(F2612_type & TYPE_LFOPAN) /* YM2608/2610/2610B/2612 */
		//{
			SLOT->AMmask = (v&0x80) ? ~0 : 0;
		//}
		break;
	case 0x70:	/*	   SR */
		//set_sr(SLOT,v);
		/* set sustain rate */
		//INLINE void set_sr(FM_SLOT *SLOT,int v)
			SLOT->d2r			= (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;
			SLOT->eg_sh_d2r 	= eg_rate_shift [SLOT->d2r + SLOT->ksr];
			SLOT->eg_sel_d2r	= eg_rate_select[SLOT->d2r + SLOT->ksr];
			SLOT->eg_sh_d2r_val = ((1<<SLOT->eg_sh_d2r)-1) ;
		break;
	case 0x80:	/* SL, RR */
		//set_sl_rr(SLOT,v);
		/* set release rate */
		//INLINE void set_sl_rr(FM_SLOT *SLOT,int v)
			SLOT->sl = sl_table[ v>>4 ];
			SLOT->rr  = 34 + ((v&0x0f)<<2);
			SLOT->eg_sh_rr	= eg_rate_shift [SLOT->rr  + SLOT->ksr];
			SLOT->eg_sel_rr = eg_rate_select[SLOT->rr  + SLOT->ksr];
			SLOT->eg_sh_rr_val	= ((1<<SLOT->eg_sh_rr )-1) ;
		break;
//	case 0x90:	/* SSG-EG */
//		SLOT->ssg  =  v&0x0f;
//		SLOT->ssgn = (v&0x04)>>1; /* bit 1 in ssgn = attack */
//		break;
	case 0xa0:
		switch( (r&(3<<2)) ){//OPN_SLOT((r>>2)&3)
		case (0<<2): 	/* 0xa0-0xa2 : FNUM1 */
			{
				UINT32 fn = (((UINT32)( (F2612_ST.fn_h)&7))<<8) + v;
				UINT8 blk = F2612_ST.fn_h>>3;
				/* keyscale code */
				NOW_CH->kcode = (blk<<2) | opn_fktable[fn >> 7];
				/* phase increment counter */
				NOW_CH->fc = F2612_fn_table[fn*2]>>(7-blk);

				/* store fnum in clear form for LFO PM calculations */
				NOW_CH->block_fnum = (blk<<11) | fn;

				NOW_CH->SLOT[SLOT1].Incr=-1;
			}
			break;
		case (1<<2): 	/* 0xa4-0xa6 : FNUM2,BLK */
			F2612_ST.fn_h = v&0x3f;
			break;
		case (2<<2): 	/* 0xa8-0xaa : 3CH FNUM1 */
			if(r < 0x100)
			{
				UINT32 fn = (((UINT32)(F2612_SL3.fn_h&7))<<8) + v;
				UINT8 blk = F2612_SL3.fn_h>>3;
				/* keyscale code */
				F2612_SL3.kcode[c]= (blk<<2) | opn_fktable[fn >> 7];
				/* phase increment counter */
				F2612_SL3.fc[c] = F2612_fn_table[fn*2]>>(7-blk);
				F2612_SL3.block_fnum[c] = fn;
				F2612_CH[2].SLOT[SLOT1].Incr=-1;
			}
			break;
		case (3<<2): 	/* 0xac-0xae : 3CH FNUM2,BLK */
			if(r < 0x100)
				F2612_SL3.fn_h = v&0x3f;
			break;
		}
		break;

	case 0xb0:
		switch( (r&(3<<2)) ){//OPN_SLOT((r>>2)&3)
		case (0<<2): 	/* 0xb0-0xb2 : FB,ALGO */
			{
				int feedback = (v>>3)&7;
				NOW_CH->ALGO = v&7;
				NOW_CH->FB	 = feedback ? feedback+6 : 0;
				setup_connection( c ) ;
			}
			break;
		case (1<<2): 	/* 0xb4-0xb6 : L , R , AMS , PMS (YM2612/YM2610B/YM2610/YM2608) */
			//if( F2612_type & TYPE_LFOPAN)
			//{
				/* b0-2 PMS */
				NOW_CH->pms = ((v&7)<<5); /* CH->pms = PM depth * 32 (index in lfo_pm_table) */

				/* b4-5 AMS */
				//CH->ams = lfo_ams_depth_shift[(v>>4) & 0x03];
			//	NOW_CH->ams_val = LFO_AM >> lfo_ams_depth_shift[(v>>4) & 0x03];
				switch((v>>4) & 0x03)
				{
				case 0:	NOW_CH->ams_val = LFO_AM >> 8;	break;
				case 1:	NOW_CH->ams_val = LFO_AM >> 3;	break;
				case 2:	NOW_CH->ams_val = LFO_AM >> 1;	break;
				case 3:	NOW_CH->ams_val = LFO_AM     ;	break;
				}

				/* PAN :  b7 = L, b6 = R */
				//F2612_pan[ c*2   ] = (v & 0x80) ? ~0 : 0;
				//F2612_pan[ c*2+1 ] = (v & 0x40) ? ~0 : 0;

				/* PAN :	b7 = L, b6 = R */
//				F2612_PAN[c] = (v & (PAN_L|PAN_R));
				switch(c){ /* YM2612 pan swapped? */
				case 0: panRL[pL0]=(v)&PAN_L;	panRL[pR0]=(v)&PAN_R;	break;
				case 1: panRL[pL1]=(v)&PAN_L;	panRL[pR1]=(v)&PAN_R;	break;
				case 2: panRL[pL2]=(v)&PAN_L;	panRL[pR2]=(v)&PAN_R;	break;
				case 3: panRL[pL3]=(v)&PAN_L;	panRL[pR3]=(v)&PAN_R;	break;
				case 4: panRL[pL4]=(v)&PAN_L;	panRL[pR4]=(v)&PAN_R;	break;
				case 5: panRL[pL5]=(v)&PAN_L;	panRL[pR5]=(v)&PAN_R;	break;
				}

			//}
			break;
		}
		break;
	}
}

#endif /* BUILD_OPN */




#if BUILD_YM2612





#define ADVANCE_PAN \
			int lt,rt;\
			lt = out76496; \
			if( panRL[pL0] ){ lt += out_fm[0]; }\
			if( panRL[pL1] ){ lt += out_fm[1]; }\
			if( panRL[pL2] ){ lt += out_fm[2]; }\
			if( panRL[pL3] ){ lt += out_fm[3]; }\
			if( panRL[pL4] ){ lt += out_fm[4]; }\
			if( panRL[pL5] ){ lt += out_fm[5]; }\
		/*	lt += bufR[0] ;*/ \
\
			rt = out76496; \
			if( panRL[pR0] ){ rt += out_fm[0]; }\
			if( panRL[pR1] ){ rt += out_fm[1]; }\
			if( panRL[pR2] ){ rt += out_fm[2]; }\
			if( panRL[pR3] ){ rt += out_fm[3]; }\
			if( panRL[pR4] ){ rt += out_fm[4]; }\
			if( panRL[pR5] ){ rt += out_fm[5]; }\
		/*	rt += bufR[1] ;*/ \


static int STEP765;//=(STEP76<<(0));
static int sn76_MAXOUT=0;
/* Generate samples for one of the YM2612s */
void YM2612_76496UpdateOne( FMSAMPLE *bufferRL, int length /*,int flags*/)
{
//int panRL[p_ddd]=0;
//int panRL[p_eee];

	int i,j /*,num*/;
//	UINT32 cnt;
//	FMSAMPLE  *bufL,*bufR;
	//INT32 dacout	= g_YM2612.dacout;
//	static int lt_old=0, rt_old=0 ;

	/* set bufer */
//	bufL = buffer[0];
//	bufR = buffer[1];

	unsigned int out76496;
	int vol76[4];
	int left76;

	FM_CH *NOW_CH2;
	NOW_CH2 = &F2612_CH[2];




////////////
//STEP765=(STEP76*panRL[p_eee]);
//STEP765=(STEP76<<panRL[p_ddd]);
/* ７６を５まかす、から７６５。くだらん。 */


#if 1
/*-------------*/
//	int i;
//struct SN76496 *R = &sn[0/*chip*/];

//	j=(panRL[p_eee]*length*STEP76);
	j=(length*(STEP765));	/* need full 'length' */

	/* If the volume is 0, increase the counter */
	for (i = 0;i < 4;i++)
	{
		/* note that I do count += length, NOT count = length + 1. You might think */
		/* it's the same since the volume is 0, but doing the latter could cause */
		/* interferencies when the program is rapidly modulating the volume. */
		if (sn_Volume[i] == 0)
		{
			if (sn_Count[i] <= (j) )
				sn_Count[i] += (j);
		}
	}
/*-------------*/
#endif

/* これ以降の'length'は意味が変わるので注意 */
//	length /= 8 ;
//	length >>= 3 ;
	length >>= panRL[p_ddd] ;

	/* refresh PG and EG */
	refresh_fc_eg_chan( &F2612_CH[0] );
	refresh_fc_eg_chan( &F2612_CH[1] );
//	if( (/*F2612_ST.*/State->mode & 0xc0) )
	if( (F2612_ST.mode & 0xc0) )
	{
		/* 3SLOT MODE */
		if( NOW_CH2->SLOT[SLOT1].Incr==-1)
		{
			refresh_fc_eg_slot(&NOW_CH2->SLOT[SLOT1] , F2612_SL3.fc[1] , F2612_SL3.kcode[1] );
			refresh_fc_eg_slot(&NOW_CH2->SLOT[SLOT2] , F2612_SL3.fc[2] , F2612_SL3.kcode[2] );
			refresh_fc_eg_slot(&NOW_CH2->SLOT[SLOT3] , F2612_SL3.fc[0] , F2612_SL3.kcode[0] );
			refresh_fc_eg_slot(&NOW_CH2->SLOT[SLOT4] , NOW_CH2->fc	, NOW_CH2->kcode );
		}
	}else refresh_fc_eg_chan( NOW_CH2 );
	refresh_fc_eg_chan( &F2612_CH[3] );
	refresh_fc_eg_chan( &F2612_CH[4] );
	refresh_fc_eg_chan( &F2612_CH[5] );

	/* buffering */


	for(j=0; j < length ; j++)
	{

//out76496=0;
#if 1
/*-------------*/
		/* vol[] keeps track of how long each square wave stays */
		/* in the 1 position during the sample period. */
		vol76[0] = vol76[1] = vol76[2] = vol76[3] = 0;

		for (i = 0;i < 3;i++)
		{
		int wpi;
//		int wpj;
			wpi = sn_Period[i];
//			wpj >>=panRL[p_ddd];	// 仕方ないからこうしよう。

			if (sn_Output[i]) vol76[i] += sn_Count[i];
			sn_Count[i] -= STEP765;
			/* Period[i] is the half period of the square wave. Here, in each */
			/* loop I add Period[i] twice, so that at the end of the loop the */
			/* square wave is in the same status (0 or 1) it was at the start. */
			/* vol76[i] is also incremented by Period[i], since the wave has been 1 */
			/* exactly half of the time, regardless of the initial position. */
			/* If we exit the loop in the middle, Output[i] has to be inverted */
			/* and vol76[i] incremented only if the exit status of the square */
			/* wave is 1. */
			while (sn_Count[i] <= 0)
			{
				sn_Count[i] += wpi;
				if (sn_Count[i] > 0)
				{
					sn_Output[i] ^= 1;
					if (sn_Output[i]) vol76[i] += wpi;//(wpj);
					break;
				}
				sn_Count[i] += wpi;
				vol76[i] += wpi;//(wpj);
			}
			if (sn_Output[i]) vol76[i] -= (sn_Count[i]);
		}

		// ノイズに関しては発想の転換が必要。
		// 仕方ないからこうしよう。
		left76 = STEP76; // STEP765;	// どっちでもうまく音は出ないし、後者は（重い！）
		// なぜなら、誤魔化さない事は、レンダー精度を落としてる限り、本質的に不可能だからだ。
		// （本物の76のクロック以下になった場合は誤魔化すしかありえない）
		// ４４１００なら、誤魔化さなくて済むんだけどな。
		//
		do
		{
			int nextevent76;

			if (sn_Count[3] < left76) nextevent76 = sn_Count[3];
			else nextevent76 = left76;

			if (sn_Output[3]) vol76[3] += sn_Count[3];
#if 00
			sn_Count[3] -= nextevent76;
#else
			sn_Count[3] -= (nextevent76<<panRL[p_ddd]);	// 仕方ないからこうしよう。
#endif
			if (sn_Count[3] <= 0)
			{
				if (sn_RNG & 1) sn_RNG ^= sn_NoiseFB;
				sn_RNG >>= 1;
				sn_Output[3] = sn_RNG & 1;
				sn_Count[3] += sn_Period[3];
				if (sn_Output[3]) vol76[3] += sn_Period[3];
			}
			if (sn_Output[3]) vol76[3] -= sn_Count[3];

			left76 -= nextevent76;
		} while (left76 > 0);

		out76496 =
			  vol76[3] * (sn_Volume[3])
			+ vol76[2] * (sn_Volume[2])
			+ vol76[1] * (sn_Volume[1])
			+ vol76[0] * (sn_Volume[0]);

#if 0
		if( 	out76496 > MAX76_OUTPUT * STEP76){
				out76496 = MAX76_OUTPUT * STEP76;
		}else{
		//		out76496 =(out76496/STEP76);
				out76496 =(out76496 >>(16 ));
		}
//		*(buffer++) = out76496 / STEP76;

/* gain init:  add,(vol table /2) */
		if ( out76496 > MAX76_OUTPUT * STEP765 )
		{		out76496= (( (MAX76_OUTPUT )*3)>>3);	}
		else{	out76496= (( (out76496>>16 )*3)>>3);	}
//  約１／３に減衰
#endif
//*((unsigned short*)bufferRL++) +=  out76496;
//*((unsigned short*)bufferRL++) +=  out76496;
#if 1
	// MAX76_OUTPUT==0x7fff==32767,   12288==256*16*3   12288/32767 == 0.375011444（これは参考値、結果的にDaveさんと同じミキシング精度になる）
	// MAX76_OUTPUT==0x7fff==32767,   11520==256*15*3   11520/32767 == 0.351573229（現在はこう。これが最適？？）
	// （仕様変更により、sn_Volume[]テーブル値が違う、既に約１／３（３５％）になってる）
	//	if ( out76496 > (MAX76_OUTPUT/3) * (STEP76) /*5*/ )
	//	if ( out76496 > (10922) * (STEP76) /*5*/ ) /* １／３の参考値 */
	//	if ( out76496 > (11520) * (STEP76) /*5*/ ) /* 間違いだー */
		if ( out76496 > (10920) * (STEP76) /*5*/ ) /* 現在はこうしないと駄目 */
		{
			out76496 =(sn76_MAXOUT); /* 最大ゲインは予め計算してある。 gain init:  add,(vol table /2) */
		}else{
			out76496 =(out76496>>(16  )); /* ミキシング済みだからなんの問題もないんよ */
		}
//Daveさんの場合。（３３％よりも再現度（ミキシング精度）が高いので不思議に思っていた。）
//out76496= (( (out76496  )*3)>>3);//  約１／３（３８％）に減衰


#if 00
/* あまりにも聞きづらいので、調整。（誤魔化しの根拠は一応ある） */
		switch( panRL[p_ddd] )
		{
			default :
			case 0/*44100*/ :               break ; /*  1（ここだけ誤魔化し方が違う） */
			case 1/*22050*/ :               break ; /*  0 */
			case 2/*11025*/ : out76496>>=1; break ; /* -1 */
			case 3/* 5512*/ : out76496>>=2; break ; /* -2 */
		}
//dac2612[];
#endif
#endif



/*-------------*/
#endif


		//advance_lfo(/* &g_YM2612.OPN */);
/* advance LFO to next sample */
//INLINE void advance_lfo(void/*FM_OPN *OPN*/)
{
UINT8 pos;
UINT8 prev_pos;
	if(F2612_lfo_inc)	/* LFO enabled ? */
	{
#if 00
		/*original*/
		prev_pos = F2612_lfo_cnt>>LFO_SH & 0x7f;
		F2612_lfo_cnt += F2612_lfo_inc;
		pos = (F2612_lfo_cnt >> LFO_SH) & 0x7f;
		/* update AM when LFO output changes */
		/*if (prev_pos != pos)*/
		/* actually I can't optimize is this way without rewritting chan_calc()
		to use chip->lfo_am instead of global lfo_am */
		/* triangle */
		/* AM: 0 to 126 step +2, 126 to 0 step -2 */
		if (pos<64) LFO_AM =        (pos& 0x3f) * 2;
		else		LFO_AM = 126 - ((pos& 0x3f) * 2);
		/* PM works with 4 times slower clock */
		prev_pos >>= 2;
		pos      >>= 2;
#else
		prev_pos = F2612_lfo_cnt>>LFO_SH & 0x7f;
		F2612_lfo_cnt += F2612_lfo_inc;
		pos = (F2612_lfo_cnt >> (LFO_SH-1)) & 0xfe;

		LFO_AM = (pos& 0x7e);
	//	if (pos<128) {;}else LFO_AM = 252 - (LFO_AM);
		if (pos>128) LFO_AM = 252 - (LFO_AM);
		/* PM works with 4 times slower clock */
		prev_pos >>= 2;
		pos      >>= 3;
#endif
		/* update PM when LFO output changes */
		/*if (prev_pos != pos)*/ /* can't use global lfo_pm for this optimization, must be chip->lfo_pm instead*/
		LFO_PM = pos;
	}
	else
	{
		LFO_AM = 0;
		LFO_PM = 0;
	}
}


		/* clear outputs */
		out_fm[0] = 0;
		out_fm[1] = 0;
		out_fm[2] = 0;
		out_fm[3] = 0;
		out_fm[4] = 0;
		out_fm[5] = 0;

		/* advance envelope generator */
		F2612_ecount=0;
		F2612_eg_timer += F2612_eg_timer_add;
#if 1
		while (F2612_eg_timer >= F2612_eg_timer_overflow)
		{
			F2612_eg_timer -= F2612_eg_timer_overflow;
			F2612_eg_cnt++;

			F2612_ecount++;
		}
#endif

		/* calculate FM */
		chan_calc( &F2612_CH[0] );
		chan_calc( &F2612_CH[1] );
		chan_calc(	NOW_CH2 );
		chan_calc( &F2612_CH[3] );
		chan_calc( &F2612_CH[4] );

#if 00
		if( F2612_dacen )	*(&F2612_CH[5].connect4) += F2612_dacout;
		else				chan_calc( &F2612_CH[5] );
#else
		if( F2612_dacen )	out_fm[5]= (F2612_dacout);
		else				chan_calc( &F2612_CH[5] );
#endif


		{
			ADVANCE_PAN
//			rt |= (lt << 16) ;
//			//*((unsigned long*)bufferRL) = ((lt & 0x0000FFFF) << 16) | (rt & 0x0000FFFF) ;
//			*((unsigned long*)bufferRL) = ((rt & 0x0000FFFF) << 16) | (lt & 0x0000FFFF) ;
//			bufferRL += 2 ;

switch(panRL[p_ddd]){
case 3:
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;	/*no break;*/
case 2:
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;	/*no break;*/
case 1:
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;	/*no break;*/
case 0:
*((unsigned short*)bufferRL++) = rt;	*((unsigned short*)bufferRL++) = lt;	/*no break;*/
}

			/* buffering for osakana pga mixer. */
//			bufL[i] = lt;
//			bufR[i] = rt;
//*((unsigned short*)bufferRL++) =  rt;
//*((unsigned short*)bufferRL++) =  lt;
		}

		/* timer A control */
		//INTERNAL_TIMER_A( &F2612_ST , NOW_CH2 )
		if( F2612_ST.TAC ) //&&  (F2612_ST.Timer_Handler==0) )
		{
			if( (F2612_ST.TAC -= FREQ_QQQ_BASE*panRL[p_eee] /*(int)(ST->freqbase*4096)*/ ) <= 0 )
			{
				TimerAOver( &F2612_ST );
				/* CSM mode total level latch and auto key on */
				if( F2612_ST.mode & 0x80 )
					CSMKeyControll( NOW_CH2 );
			}
		}
	}

//	INTERNAL_TIMER_B(&F2612_ST,length)
	if( F2612_ST.TBC ) //&& (F2612_ST.Timer_Handler==0) )
	{
		if( (F2612_ST.TBC -= (FREQ_QQQ_BASE*panRL[p_eee]*length)/*(int)(ST->freqbase*4096*step)*/ ) <= 0 )
			TimerBOver( &F2612_ST );
	}
}


////////////////


#if 00
/*
void SN76496_set_gain(int chip,int gain)
{
	struct SN76496 *R = &sn[chip];
	int i;
	double out;


	gain &= 0xff;

	// increase max output basing on gain (0.2 dB per step) //
	out = MAX76_OUTPUT / 3;
	while (gain-- > 0)
		out *= 1.023292992;	// = (10 ^ (0.2/20)) //

	// build volume table (2dB per step) //
	for (i = 0;i < 15;i++)
	{
		// limit volume to avoid clipping //
		if (out > MAX76_OUTPUT / 3) sn_VolTable[i] = MAX76_OUTPUT / 3;
		else sn_VolTable[i] = out;

		out /= 1.258925412;	// = 10 ^ (2/20) = 2dB //
	}
	sn_VolTable[15] = 0;
}
*/
void SN76496_set_gain(int chip,int gain)
{
//	struct SN76496 *R = &sn[0/*chip*/];
	int i;
//	double out;
	unsigned int out;
	gain &= 0xff;

	// increase max output basing on gain (0.2 dB per step) //
	out = (MAX76_OUTPUT*2) / 3;
	while (gain-- > 0)
//		out *= 1.023292992;	// = (10 ^ (0.2/20)) //
		out *= 2;//3292992;	// = (10 ^ (0.2/20)) //

	// build volume table (2dB per step) //
	for (i = 0;i < 15;i++)
	{
		// limit volume to avoid clipping //
		if (out/2 > MAX76_OUTPUT / 3) sn_VolTable[i] = MAX76_OUTPUT / 3;
		else sn_VolTable[i] = out/2;

//		out /= 1.258925412;	// = 10 ^ (2/20) = 2dB //
		out /= 2;//8925412;	// = 10 ^ (2/20) = 2dB //
//Error_count(sn_VolTable[i]);
	}
	sn_VolTable[15] = 0;
}
#endif

static void SN76496_reset(void)
{
	sn_Volume[0] = 0;
	sn_Volume[1] = 0;
	sn_Volume[2] = 0;
	sn_Volume[3] = 0;

	sn_LastRegister = 0;
	sn_Register[0] = 0;
	sn_Register[1] = 0x0f;	/* volume = 0 */
	sn_Register[2] = 0;
	sn_Register[3] = 0x0f;	/* volume = 0 */
	sn_Register[4] = 0;
	sn_Register[5] = 0x0f;	/* volume = 0 */
	sn_Register[6] = 0;
	sn_Register[7] = 0x0f;	/* volume = 0 */

	sn_Output[0] = 0;
	sn_Output[1] = 0;
	sn_Output[2] = 0;
	sn_Output[3] = 0;
	sn_Period[0] = sn_Count[0] = sn_UpdateStep;
	sn_Period[1] = sn_Count[1] = sn_UpdateStep;
	sn_Period[2] = sn_Count[2] = sn_UpdateStep;
	sn_Period[3] = sn_Count[3] = sn_UpdateStep;

	sn_RNG = NG_PRESET;
	sn_Output[3] = sn_RNG & 1;
}


//int SN76496_sh_start(int clock, int volume, int rate)
//{
//    SN76496_init(/*0,*/ clock, volume & 0xff, rate);
//	SN76496_set_gain(0, (volume >> 8) & 0xff);
//}
//
void SN76496_init(void)//
/*int chip,*/
{
//int clock;
//int volume;
//int sample_rate;
// clock=3579545;
//  volume=100;
//  sample_rate=44100;
//	int i;
//	struct SN76496 *R = &sn[0/*chip*/];

//	sn_SampleRate = 44100;//sample_rate;
//	SN76496_set_clock(/*chip 0,*/clock);
//static void SN76496_set_clock(/*int chip,*/int clock)
//{
//	struct SN76496 *R = &sn[0/*chip*/];


	/* the base clock for the tone generators is the chip clock divided by 16; */
	/* for the noise generator, it is clock / 256. */
	/* Here we calculate the number of steps which happen during one sample */
	/* at the given sample rate. No. of events = sample rate / (clock/16). */
	/* STEP76 is a multiplier used to turn the fraction into a fixed point */
	/* number. */
//	sn_UpdateStep = ((double)STEP76 * sn_SampleRate * 16) / clock;
//	sn_UpdateStep = ( (unsigned int)STEP76 * 44100/*(unsigned int)sn_SampleRate*/ ) / (/*clock*/3579545/16);
	sn_UpdateStep = ( (unsigned int)STEP76 * 44100  ) / (3579545/16);

//	Error_count(sn_UpdateStep);
//}

	SN76496_reset();
//	return 0;
}





//////////////////////////
#ifdef _STATE_H
#if 000000
void YM2612Postload(void *chip)
{
	if (chip)
	{
		YM2612 *F2612 = (YM2612 *)chip;
		int r;

		/* DAC data & port */
		F2612_dacout = ((int)F2612->REGS[0x2a] - 0x80) << 0;	/* level unknown */
		F2612_dacen  = F2612->REGS[0x2d] & 0x80;
		/* OPN registers */
		/* DT / MULTI , TL , KS / AR , AMON / DR , SR , SL / RR , SSG-EG */
		for(r=0x30;r<0x9e;r++)
			if((r&3) != 3)
			{
				OPNWriteReg(/*&F2612->OPN,*/ r,F2612->REGS[r]);
				OPNWriteReg(/*&F2612->OPN,*/ r|0x100,F2612->REGS[r|0x100]);
			}
		/* FB / CONNECT , L / R / AMS / PMS */
		for(r=0xb0;r<0xb6;r++)
			if((r&3) != 3)
			{
				OPNWriteReg(/*&F2612->OPN,*/ r,F2612->REGS[r]);
				OPNWriteReg(/*&F2612->OPN,*/ r|0x100,F2612->REGS[r|0x100]);
			}
		/* channels */
		/*FM_channel_postload(F2612->CH,6);*/
	}
	cur_chip = NULL;
}
#endif

static void YM2612_save_state(YM2612 *F2612, int index)
{
	const char statename[] = "YM2612";

	state_save_register_UINT8 (statename, index, "regs"   , F2612->REGS   , 512);
	FMsave_state_st(statename,index,&F2612_ST);
	FMsave_state_channel(statename,index,F2612->CH,6);
	/* 3slots */
	state_save_register_UINT32 (statename, index, "slot3fc" , F2612_SL3.fc ,   3);
	state_save_register_UINT8  (statename, index, "slot3fh" , &F2612_SL3.fn_h, 1);
	state_save_register_UINT8  (statename, index, "slot3kc" , F2612_SL3.kcode, 3);
	/* address register1 */
	state_save_register_UINT8 (statename, index, "addr_A1" , &F2612->addr_A1, 1);
}
#endif /* _STATE_H */






/* reset one of chip */
/* called by mem.cpp & md.cpp */
void YM2612ResetChip(void)
{
	int i;
	//YM2612 *F2612 = &g_YM2612 ;
	//FM_OPN *OPN	= &F2612->OPN;

//	int x , c ;
//	char buff[128];
//	FILE* pp;

	if( !is_init_YM2612 ){ return ; }

//	OPNSetPres();///*OPN,*/ 6*24, 6*24, 0);
/* prescaler set (and make time tables) */
//static void OPNSetPres(void)///*FM_OPN *OPN,*/ int pres=6*24 , int TimerPres=6*24, int SSGpres)

// NTSC genesis master xtal(53.693175[MHz]) if cases the divide 7.5
/*
XTAL/7	 : 7670453
DG EN100  : 7520000 (mistake)
XTAL/7.5 : 7159090 (==68000 clock==YM clock, but timers div 2 ? )
*/
	/* frequency base */
//	F2612_ST.freqbase = (F2612_ST.rate) ? ((double)F2612_ST.clock / F2612_ST.rate) / pres : 0;
//  clock:7520000 was (7520000/44100 )/(6*24);//F2612_ST.freqbase = 1.18417737465356512975560594608214;
//  clock:7159090 was (7159090/44100 )/(6*24);//F2612_ST.freqbase = 1.12734473418997228521038044847569;

#if 0
	F2612_ST.rate = (double)F2612_ST.clock / pres;
	F2612_ST.freqbase = 1.0;
#endif
/* 77606 ?????????? */
/* DGEN100type: 77606.2484252960443436633912824389 == (0x00010000 * 1.18417737465356512975560594608214); */
/* SEGADRIVE  : 73881.6644998740236835474930713026 == (0x00010000 * 1.12734473418997228521038044847569 ); */
	F2612_eg_timer_add	= 73882 * (panRL[p_eee]) ; //(1<<EG_SH)	*  F2612_ST.freqbase;
	F2612_eg_timer_overflow = ( 3 ) * (1<<EG_SH);


	/* Timer base time */
//	F2612_ST.TimerBase = 1.0/((double)F2612_ST.clock / (double)TimerPres);

	/* SSG part  prescaler set */
//	if( SSGpres ) (*F2612_ST.SSG->set_clock)( F2612_ST.param, F2612_ST.clock * 2 / SSGpres );

	/* make time tables */
//	init_timetables( &F2612_ST, dt_tab );
/* initialize time tables */
//static void init_timetables( FM_ST *ST , const UINT8 *dttable )

	{
	//double rate;
	int j;
	int c;
		c = 0;
		/* DeTune table */
		for (i = 0;i <= 3;i++){
			for (j = 0;j <= 31;j++){
				//rate = ((double)dttable[i*32 + j]) * SIN_LEN	* ST->freqbase	* (1<<FREQ_SH) / ((double)(1<<20));
				F2612_dt_tab[i  ][j] =  dt_table[c] * (panRL[p_eee]) ; //(INT32) rate;
				F2612_dt_tab[i+4][j] = -F2612_dt_tab[i][j];
				c++ ;
//				logerror("FM.C: DT [%2i %2i] = %8x  \n", i, j, ST->dt_tab[i][j] );
			}
		}
	}


	/* there are 2048 FNUMs that can be generated using FNUM/BLK registers
		but LFO works with one more bit of a precision so we really need 4096 elements */
	/* calculate fnumber -> increment counter table */
	for(i=0; i<4096; i++)
	{
		/* freq table for octave 7 */
		/* OPN phase increment counter = 20bit */
		F2612_fn_table[i] = fn_tables[i] * (panRL[p_eee]) ;//(UINT32)( (double)i * 32 * F2612_ST.freqbase * (1<<(FREQ_SH-10)) ); /* -10 because chip works with 10.10 fixed point, while we use 16.16 */
//		logerror("FM.C: fn_table[%4i] = %08x (dec=%8i)\n", i, F2612_fn_table[i]>>6,F2612_fn_table[i]>>6 );
	}

#if 0
pp = fopen( "_fn_table.c", "wb" ) ;
if( pp )
{
	x = 0 ; c = 0;
	for(i = 0; i < 4096; i++)
	{
		sprintf( buff, "0x%08X,", F2612_fn_table[i] ) ;
		x++ ;
		if( x >= 8 ){ strcat( buff, "\r\n" ) ; x = 0; }
		fwrite( buff, 1, strlen(buff), pp ) ;
	}
	fclose( pp ) ;
}
#endif //

	/* LFO freq. table */
//	int lfo_freq_table[] =
//	{
//		0x0002CE93,0x0003EFDF,0x0004450B,0x0004864D,
//		0x0004E3B6,0x0006E3C7,0x0025E4C7,0x003CA13F
//	};
/* 8 LFO speed parameters */
/* each value represents number of samples that one LFO level will last for */
//static const UINT32 lfo_samples_per_step[8] = {108, 77, 71, 67, 62, 44, 8, 5};

	/* Amplitude modulation: 64 output levels (triangle waveform); 1 level lasts for one of "lfo_samples_per_step" samples */
	/* Phase modulation: one entry from lfo_pm_output lasts for one of 4 * "lfo_samples_per_step" samples  */
	LFO_INC_TAB[0] = (0x0002CE93) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;
	LFO_INC_TAB[1] = (0x0003EFDF) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;
	LFO_INC_TAB[2] = (0x0004450B) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;
	LFO_INC_TAB[3] = (0x0004864D) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;
	LFO_INC_TAB[4] = (0x0004E3B6) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;
	LFO_INC_TAB[5] = (0x0006E3C7) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;
	LFO_INC_TAB[6] = (0x0025E4C7) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;
	LFO_INC_TAB[7] = (0x003CA13F) * (panRL[p_eee]); //(1.0 / lfo_samples_per_step[i]) * (1<<LFO_SH) * F2612_ST.freqbase;

//	logerror("FM.C: lfo_freq[%i] = %08x (dec=%8i)\n", i, LFO_INC_TAB[i],LFO_INC_TAB[i] );



#if 0
pp = fopen( "_lfo_freq_table.c", "wb" ) ;
if( pp )
{
	x = 0 ; c = 0;
	for(i = 0; i < 8; i++)
	{
		sprintf( buff, "0x%08X,", LFO_INC_TAB[i] ) ;
		x++ ;
		if( x >= 8 ){ strcat( buff, "\r\n" ) ; x = 0; }
		fwrite( buff, 1, strlen(buff), pp ) ;
	}
	fclose( pp ) ;
}
#endif //

//}


	/* DAC mode clear */
	F2612_dacen = 0;

	/* status clear */
	F2612_eg_timer = 0;
	F2612_eg_cnt   = 0;

	FM_IRQMASK_SET(&F2612_ST,0x03);
	FM_BUSY_CLEAR(&F2612_ST);
	OPNWriteMode(/*OPN,*/0x27,0x30); /* mode 0 , timer reset */

	FM_STATUS_RESET(&F2612_ST, 0xff);

	reset_channels( &F2612_ST , &F2612_CH[0] , 6 );
	for(i = 0xb6 ; i >= 0xb4 ; i-- )
	{
		OPNWriteReg(/*OPN,*/i	   ,0xc0);
		OPNWriteReg(/*OPN,*/i|0x100,0xc0);
	}
	for(i = 0xb2 ; i >= 0x30 ; i-- )
	{
		OPNWriteReg(/*OPN,*/i	   ,0);
		OPNWriteReg(/*OPN,*/i|0x100,0);
	}
	for(i = 0x26 ; i >= 0x20 ; i-- ) OPNWriteReg(/*OPN,*/i,0);

}



/* initialize generic tables */
/*static int*/INLINE void init_tables(void)
{
signed int i;

#if 0
signed int i,x;
signed int n;
double o,m;
	char buff[128];
	FILE* pp;

	for (x=0; x<TL_RES_LEN; x++)
	{
		m = (1<<16) / pow(2, (x+1) * (ENV_STEP/4.0) / 8.0);
		m = floor(m);

		/* we never reach (1<<16) here due to the (x+1) */
		/* result fits within 16 bits at maximum */

		n = (int)m; 	/* 16 bits here */
		n >>= 4;		/* 12 bits here */
		if (n&1)		/* round to nearest */
			n = (n>>1)+1;
		else
			n = n>>1;
						/* 11 bits here (rounded) */
		n <<= 2;		/* 13 bits here (as in real chip) */
		tl_tab[ x*2 + 0 ] = n;
		tl_tab[ x*2 + 1 ] = -tl_tab[ x*2 + 0 ];

		for (i=1; i<13; i++)
		{
			tl_tab[ x*2+0 + i*2*TL_RES_LEN ] =	tl_tab[ x*2+0 ]>>i;
			tl_tab[ x*2+1 + i*2*TL_RES_LEN ] = -tl_tab[ x*2+0 + i*2*TL_RES_LEN ];
		}
	#if 0
			logerror("tl %04i", x);
			for (i=0; i<13; i++)
				logerror(", [%02i] %4x", i*2, tl_tab[ x*2 /*+1*/ + i*2*TL_RES_LEN ]);
			logerror("\n");
		}
	#endif
	}
	/*logerror("FM.C: TL_TAB_LEN = %i elements (%i bytes)\n",TL_TAB_LEN, (int)sizeof(tl_tab));*/


pp = fopen( "_tl_table.c", "wb" ) ;
if( pp )
{
	x=0;
	for( i=0; i < TL_TAB_LEN ; i++ )
	{
		sprintf( buff, "%5d,", tl_tab[i] ) ;
		x++ ;
		if( x >= 8 ){ strcat( buff, "\r\n" ) ; x = 0; }
		fwrite( buff, 1, strlen(buff), pp ) ;
	}

	fclose( pp ) ;
}

	for (i=0; i<SIN_LEN; i++)
	{
		/* non-standard sinus */
		m = sin( ((i*2)+1) * PI / SIN_LEN ); /* checked against the real chip */

		/* we never reach zero here due to ((i*2)+1) */
		if (m>0.0)	o = 8*log( 1.0/m)/log(2);	/* convert to 'decibels' */
		else		o = 8*log(-1.0/m)/log(2);	/* convert to 'decibels' */

		o = o / (ENV_STEP/4);

		n = (int)(2.0*o);
		if (n&1)	n = (n>>1)+1;	/* round to nearest */
		else		n = (n>>1)  ;

		sin_tab[ i ] = n*2 + (m>=0.0? 0: 1 );
		/*logerror("FM.C: sin [%4i]= %4i (tl_tab value=%5i)\n", i, sin_tab[i],tl_tab[sin_tab[i]]);*/
	}

	/*logerror("FM.C: ENV_QUIET= %08x\n",ENV_QUIET );*/

pp = fopen( "_sin_table.c", "wb" ) ;
if( pp )
{
	x=0;
	for( i=0; i < SIN_LEN ; i++ )
	{
		sprintf( buff, "%5d,", sin_tab[i] ) ;
		x++;
		if( x >= 8 ){ strcat( buff, "\r\n" ) ; x = 0; }
		fwrite( buff, 1, strlen(buff), pp ) ;
	}
	fclose( pp ) ;
}

#endif //

	/* build LFO PM modulation table */
	for(i = 0; i < 8; i++) /* 8 PM depths */
	{
		UINT8 fnum;
		for (fnum=0; fnum<128; fnum++) /* 7 bits meaningful of F-NUMBER */
		{
			UINT8 value;
			UINT8 step;
			UINT32 offset_depth = i;
			UINT32 offset_fnum_bit;
			UINT32 bit_tmp;

			for (step=0; step<8; step++)
			{
				value = 0;
				for (bit_tmp=0; bit_tmp<7; bit_tmp++) /* 7 bits */
				{
					if (fnum & (1<<bit_tmp)) /* only if bit "bit_tmp" is set */
					{
						offset_fnum_bit = bit_tmp * 8;
						value += lfo_pm_output[offset_fnum_bit + offset_depth][step];
					}
				}
				lfo_pm_table[(fnum*32*8) + (i*32) + step   + 0] = value;
				lfo_pm_table[(fnum*32*8) + (i*32) +(step^7)+ 8] = value;
				lfo_pm_table[(fnum*32*8) + (i*32) + step   +16] = -value;
				lfo_pm_table[(fnum*32*8) + (i*32) +(step^7)+24] = -value;
			}
#if 0
			logerror("LFO depth=%1x FNUM=%04x (<<4=%4x): ", i, fnum, fnum<<4);
			for (step=0; step<16; step++) /* dump only positive part of waveforms */
				logerror("%02x ", lfo_pm_table[(fnum*32*8) + (i*32) + step] );
			logerror("\n");
#endif
		}
	}
//	return 1;
}
/* initialize YM2612 emulator(s) */
void YM2612Init(int rate_44100Hz )
//	YM2612Init(1, VCLK, /*rate*/44100, NULL, NULL);
//void * YM2612Init(int dummy_index, int clock, int rate,
//			   FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler)
{
	//YM2612 *F2612;
	//cur_chip = NULL;	/* hiro-shi!! */
	//F2612 = &g_YM2612 ;
	/* allocate extend state space */
	//if( (F2612 = (YM2612 *)malloc(sizeof(YM2612)))==NULL)
	//	return NULL;
	/* clear */
	//memset(F2612,0,sizeof(YM2612));
	/* allocate total level table (128kb space) */
//	if( !init_tables() )
//	{
//#ifndef _PSP10
//		free( F2612 );
//#endif //_PSP10
//		return NULL;
//	}

	init_tables();

	F2612_ST.rate  = rate_44100Hz;//rate;

/*この辺はシステム(init)に入れてしまいたい*/
	switch( F2612_ST.rate )
	{
//	default:
	case 44100: panRL[p_ddd]=0; /*F2612_fff=0;*/ STEP765=(STEP76<<(0)); sn76_MAXOUT=(MAX76_OUTPUT/3)    ; break;
	case 22050: panRL[p_ddd]=1; /*F2612_fff=0;*/ STEP765=(STEP76<<(1)); sn76_MAXOUT=(MAX76_OUTPUT/3)    ; break;
	case 11025: panRL[p_ddd]=2; /*F2612_fff=1;*/ STEP765=(STEP76<<(2)); sn76_MAXOUT=(MAX76_OUTPUT/3)>>2/*>>2*/ ; break;
 /* ７６のノイズ周波数帯域が変わって聞きづらい為、仕方なく補正 ううんやっぱ１１０２５が一番厳しい */
	case  5512: panRL[p_ddd]=3; /*F2612_fff=1;*/ STEP765=(STEP76<<(3)); sn76_MAXOUT=(MAX76_OUTPUT/3)>>1/*>>2*/ ; break;
 /* ７６のノイズ周波数帯域が変わって聞きづらい為、仕方なく補正 ５５１２は荒いからなんとかなっちゃうみたい。 */
	}
	panRL[p_eee]=(1<<panRL[p_ddd]);/*1 2 4 8 (44100/F2612_ST.rate) */
// 1==(44100/44100)
// 2==(44100/22050)
// 4==(44100/11025)
// 8==(44100/ 5512)

	F2612_ST.param = NULL ;
//	F2612_type = TYPE_YM2612;
//	F2612_P_CH = F2612_CH;
	F2612_ST.clock = 7159090;//clock;

	/* F2612->OPN.ST.irq = 0; */
	/* F2612->OPN.ST.status = 0; */
	/* Extend handler */
//	F2612_ST.Timer_Handler = NULL;//TimerHandler;
//	F2612_ST.IRQ_Handler   = NULL;//IRQHandler;

	is_init_YM2612 = 1 ;
	YM2612ResetChip();


#ifdef _STATE_H
	YM2612_save_state(F2612, index);
#endif
//	return 0;
}

















#define YM2612UpdateReq(param)

/* YM2612 write */
/* n = number  */
/* a = address */
/* v = value   */
int YM2612Write(int a, UINT8 v)
{
	//YM2612 *F2612 = &g_YM2612 ;
	int addr;

	v &= 0xff;	/* adjust to 8 bit bus */

//	switch( a&3){
	switch( a ){
	case 0: /* address port 0 */
		F2612_ST.address = v;
		F2612_addr_A1 = 0;
		break;

	case 1: /* data port 0	  */
		if (F2612_addr_A1 != 0)
			break;	/* verified on real YM2608 */

		addr = F2612_ST.address;
#ifdef _STATE_H
		F2612->REGS[addr] = v;
#endif
		switch( addr & 0xf0 )
		{
		case 0x20:	/* 0x20-0x2f Mode */
			switch( addr )
			{
			case 0x2a:	/* DAC data (YM2612) */
				YM2612UpdateReq(F2612_ST.param);
#if 00
				F2612_dacout = ((int)v - 0x80) << 8;	/* level unknown */
#else
				/* v == unsigned 8bit PCM DAC */
//				F2612_dacout = (dac2612[v&0xff]<<(6  ));
//				F2612_dacout = (dac2612[v&0xff]<<(3-panRL[p_ddd]));
F2612_dacout=0;/*DAC disable TEST*/
#endif
				break;
			case 0x2b:	/* DAC Sel	(YM2612) */
				/* b7 = dac enable */
				F2612_dacen = v & 0x80;
				//cur_chip = NULL;
				break;

			default:	/* OPN section */
				YM2612UpdateReq(F2612_ST.param);
				/* write register */
				OPNWriteMode(/*&(F2612->OPN),*/addr,v);
			}
			break;
		default:	/* 0x30-0xff OPN section */
			YM2612UpdateReq(F2612_ST.param);
			/* write register */
			OPNWriteReg(/*&(F2612->OPN),*/ addr,v);
		}
		break;

	case 2: /* address port 1 */
		F2612_ST.address = v;
		F2612_addr_A1 = 1;
		break;

	case 3: /* data port 1	  */
		if (F2612_addr_A1 != 1)
			break;	/* verified on real YM2608 */

		addr = F2612_ST.address;
#ifdef _STATE_H
		F2612->REGS[addr | 0x100] = v;
#endif
		YM2612UpdateReq(F2612_ST.param);
		OPNWriteReg(/*&(F2612->OPN),*/ addr | 0x100,v);
		break;
	}
	return F2612_ST.irq;
}

UINT8 YM2612Read(void){ return F2612_ST.status;}

#if 00000
int YM2612TimerOver(void *chip,int c)
{
	//YM2612 *F2612 = chip;

	if( c )
	{	/* Timer B */
		TimerBOver( &(F2612_ST) );
	}
	else
	{	/* Timer A */
		YM2612UpdateReq(F2612_ST.param);
		/* timer update */
		TimerAOver( &(F2612_ST) );
		/* CSM mode key,TL controll */
		if( F2612_ST.mode & 0x80 )
		{	/* CSM mode total level latch and auto key on */
			CSMKeyControll( &(F2612_CH[2]) );
		}
	}
	return F2612_ST.irq;
}
#endif











#endif /* BUILD_YM2612 */


/* About 2612DAC:

YM2612 ch#6 DAC was not liner., I think it is ...
+
 Amplify                                    :
    A                                       :
127 +                   __------~~ /        :
    |             _ --~~         /          :
    |         _-~              /            :
    |       /~               /              :
    |     /2612DAC         /                :
    |    /               /                  :
    |   /              /                    :
 63 +   /            /                      :
    |   /          /  linear dac            :
    |  /         /       (of couse logscalable at voltage) :
    |  /       /                            :
    | /      /                              :
    | /    /                                :
    | /  /                                  :
-   || /                                    :
    |/                                      :
  --+----------------+-------------|-> data :
   0|                63           127       :
  0x80      - <--                 0xff --> +
(minus as same it., but zero(0x80 or 0x7f) cross point at linear.)
 */
