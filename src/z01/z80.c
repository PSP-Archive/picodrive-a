/* Single-Z80 32 Bit emulator [Modified] */
/* Copyright 1996-2000 Neil Bradley, All rights reserved
 *
 * License agreement:
 *
 * (MZ80 Refers to both the assembly code emitted by makeZ80.c and makeZ80.c itself)
 *
 * MZ80 May be distributed in unmodified form to any medium.
 *
 * MZ80 May not be sold, or sold as a part of a commercial package without
 * the express written permission of Neil Bradley (neil@synthcom.com). This
 * includes shareware.
 *
 * Modified versions of MZ80 may not be publicly redistributed without author
 * approval (neil@synthcom.com). This includes distributing via a publicly
 * accessible LAN. You may make your own source modifications and distribute
 * MZ80 in source or object form, but if you make modifications to MZ80
 * then it should be noted in the top as a comment in makeZ80.c.
 *
 * MZ80 Licensing for commercial applications is available. Please email
 * neil@synthcom.com for details.
 *
 * Synthcom Systems, Inc, and Neil Bradley will not be held responsible for
 * any damage done by the use of MZ80. It is purely "as-is".
 *
 * If you use MZ80 in a freeware application, credit in the following text:
 *
 * "Multi-Z80 CPU emulator by Neil Bradley (neil@synthcom.com)"
 *
 * must accompany the freeware application within the application itself or
 * in the documentation.
 *
 * Legal stuff aside:
 *
 * If you find problems with MZ80, please email the author so they can get
 * resolved. If you find a bug and fix it, please also email the author so
 * that those bug fixes can be propogated to the installed base of MZ80
 * users. If you find performance improvements or problems with MZ80, please
 * email the author with your changes/suggestions and they will be rolled in
 * with subsequent releases of MZ80.
 *
 * The whole idea of this emulator is to have the fastest available 32 bit
 * Multi-Z80 emulator for the PC, giving maximum performance.
 */
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "z80.h"

#undef STILL_IO

#undef LARGE_DAA_TABLE
//#define LARGE_DAA_TABLE
#ifdef LARGE_DAA_TABLE
#include "z80daa.h"
#endif


/* Precomputed flag tables */
const static unsigned char SZHV_inc[0x100] __attribute__((aligned(64))) =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x94,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x90,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x50
};
const static unsigned char SZHV_dec[0x100] =
{
	0x92,0x42,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x12,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x12,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x12,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x12,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x12,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x12,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x12,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x16,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,
	0x92,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,
	0x92,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,
	0x92,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,
	0x92,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,
	0x92,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,
	0x92,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,
	0x92,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82
};
const static unsigned char SZP[0x100] =
{
	0x44,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84
};
//const static unsigned char bPostANDFlags[0x100] =
//{
//	0x54,0x10,0x10,0x14,0x10,0x14,0x14,0x10,0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,
//	0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,0x14,0x10,0x10,0x14,0x10,0x14,0x14,0x10,
//	0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,0x14,0x10,0x10,0x14,0x10,0x14,0x14,0x10,
//	0x14,0x10,0x10,0x14,0x10,0x14,0x14,0x10,0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,
//	0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,0x14,0x10,0x10,0x14,0x10,0x14,0x14,0x10,
//	0x14,0x10,0x10,0x14,0x10,0x14,0x14,0x10,0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,
//	0x14,0x10,0x10,0x14,0x10,0x14,0x14,0x10,0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,
//	0x10,0x14,0x14,0x10,0x14,0x10,0x10,0x14,0x14,0x10,0x10,0x14,0x10,0x14,0x14,0x10,
//	0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94,0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,
//	0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94,
//	0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94,
//	0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94,0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,
//	0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94,
//	0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94,0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,
//	0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94,0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,
//	0x94,0x90,0x90,0x94,0x90,0x94,0x94,0x90,0x90,0x94,0x94,0x90,0x94,0x90,0x90,0x94
//};







// Flag definitions


#define CF	0x01
#define NF	0x02
#define PF	0x04
#define VF	PF
#define XF	0x08
#define HF	0x10
#define YF	0x20
#define ZF	0x40
#define SF	0x80

#define IFF1	0x01
#define IFF2	0x02

#define zBC 	Z80.bc.w.l
#define zB		Z80.bc.b.l
#define zC		Z80.bc.b.h

#define zDE 	Z80.de.w.l
#define zD		Z80.de.b.l
#define zE		Z80.de.b.h

#define zHL 	Z80.hl.w.l
#define zH		Z80.hl.b.l
#define zL		Z80.hl.b.h

#define zAF 	Z80.af.w.l
#define zA		Z80.af.b.l
#define zF		Z80.af.b.h

#define zIX 	Z80.ix.w.l
#define zIXh	Z80.ix.b.l
#define zIXl	Z80.ix.b.h

#define zIY 	Z80.iy.w.l
#define zIYh	Z80.iy.b.l
#define zIYl	Z80.iy.b.h

#define zI		Z80.i
//#define zR	Z80.r

#define zIM 	Z80.im
#define zIFF1	Z80.iff1
#define zIFF2	Z80.iff2
#define zHALT	Z80.halt

#define zBC2	Z80.bc2.w.l
#define zDE2	Z80.de2.w.l
#define zHL2	Z80.hl2.w.l
#define zAF2	Z80.af2.w.l

#define zPC 	Z80.pc
#define zSP 	Z80.sp

#define zBase	Z80.Base

#define IM9 0
#define IM0 1
#define IM1 2
#define IM2 3

#define m_opcode	Z80.opcode
#define z80_IntLine Z80.InterruptLine

#define NEG \
	zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |\
		pbSubSbcTable[((UINT32) 0 << 8) | zA];\
	zA = 0 - zA


//UINT32 z80intAddr;
//UINT32 z80pc;
/* Modular global variables go here*/
//extern unsigned char n z80_read(unsigned int address );
//extern unsigned char z80_readmem16(unsigned int address );
//extern void z80_writemem16(unsigned int address,unsigned char data );
extern unsigned char z80_readmem16(unsigned short address );
extern void z80_writemem16(unsigned short address, unsigned char data );

#define RM(addr) (UINT8)z80_readmem16(addr)
#define WM(addr,value) z80_writemem16(addr,value)

typedef union
{
	struct { unsigned short l; } w;
	struct { unsigned char h,l; } b;
} PAIR80;

#define pbPC Z80.z_pbPC
typedef struct /*mz80context*/
{
	unsigned char *Base;
	unsigned char *z_pbPC;		/* Program counter normalized */

	PAIR80 ix;
	PAIR80 iy;

	PAIR80 bc;
	PAIR80 de;
	PAIR80 hl;
	PAIR80 af;

	unsigned short pc;	/* UINT32 */
	unsigned short sp;	/* UINT32 */

	UINT8 opcode;
	UINT8 i;
	UINT8 iff1;//	UINT32 z80inInterrupt;
	UINT8 iff2;//	UINT32 z80inInterrupt;

	UINT8 InterruptLine;	/* interrupt line  */
	UINT8 halt; /* halt flag [0 or 1] */
	UINT8 im;	/* im mode [0,1,2]	*/
	UINT8 dummy;


	PAIR80 bc2;
	PAIR80 de2;
	PAIR80 hl2;
	PAIR80 af2;

//	PAIR80 fxy; PAIR80 fxy2;

/////
//	unsigned short r;	/* uchar */

//	unsigned char intAddr;	//0x38;
//	unsigned char nmiAddr;	//0x66;

//	UINT32 z80clockticks;	//????
//	UINT32 z80interruptState;	/* New! */

//	struct MemoryReadByte *z80MemRead_none; 	// 廃止
//	struct MemoryWriteByte *z80MemWrite_none;	// 廃止
//	struct z80PortRead *z80IoRead;				// 廃止
//	struct z80PortWrite *z80IoWrite;			// 廃止
//	UINT32 z80rCounter; 						// 廃止
//	unsigned char z80intPending;				// 廃止
} Z80_Regs;

static Z80_Regs Z80 __attribute__((aligned(64))) ; /* CPU Context */
//static unsigned char *pb SP;	/* Stack pointer normalized */
//static struct MemoryReadByte *psMemRead ; /* Read memory structure */
//static struct MemoryWriteByte *psMemWrite ; /* Write memory structure */
//static struct z80PortRead *psIoRead; /* Read I/O structure */
//static struct z80PortWrite *psIoWrite; /* Write memory structure */
//static INT32 z80_ICount; /* Used as a countdown */
/* これ（z80_ICount）も何故かショートに出来ない。（謎） */
//static signed long z80_ICount  ; /* Used as a countdown */
static unsigned short z80_ICount	 ; /* Used as a countdown */
#if 00
/* こっちにすると、何故か種々の異常な状態がつ！ */
/* ＯｕｔＲｕｎ音楽めちゃくちゃ。ＭＷ４音でない等。ナゼ？？*/
//static unsigned short dwReturnCode;		/* Return code from exec() (from UINT32) */
static unsigned short dwOriginalCycles; /* How many cycles did we start with? (from UINT32) */
//static unsigned short dwElapsedTicks; /* How many ticks did we elapse? (from UINT32) */
//volatile INT32 sdwAddr;		/* Temporary address storage */
volatile unsigned short dwAddr; 	/* Temporary stack address (from UINT32) */
//static unsigned char *pbAddAdcTable;	/* Pointer to add/adc flag table */
//static unsigned char *pbSubSbcTable;	/* Pointer to sub/sbc flag table */
//volatile unsigned short dwTemp;		/* Temporary value (from UINT32) */
//volatile unsigned char   bTemp;	/* Temporary value */
//static unsigned char bTemp2;		/* Temporary value */
#else
//static  UINT32 dwReturnCode;		/* Return code from exec() (from UINT32) */
static	UINT32 dwOriginalCycles;	/* How many cycles did we start with? (from UINT32) */
//static  UINT32 dwElapsedTicks;	/* How many ticks did we elapse? (from UINT32) */
//volatile INT32 sdwAddr;		/* Temporary address storage */
volatile  UINT32 dwAddr;		/* Temporary stack address (from UINT32) */
//static unsigned char *pbAddAdcTable;	/* Pointer to add/adc flag table */
//static unsigned char *pbSubSbcTable;	/* Pointer to sub/sbc flag table */
//volatile	UINT32 dwTemp;		/* Temporary value (from UINT32) */
//volatile unsigned char   bTemp;	/* Temporary value */
//static unsigned char bTemp2;		/* Temporary value */
#endif

//New!!!
// TODO:  combine into a single int...

//efine IFF_OFF 0x00
#define IFF_ON	0x04
/* m_iff ON was force 0x04 because that Z80 .  */


#define ADJ_FUZZY_PC  zPC = (UINT32) pbPC - (UINT32) zBase



#define Z80RETURN z80Return();
static void z80Return(void){
	zPC = (
			((UINT16)(zBase[zSP  ])   )+	/* Pop LSB */
			((UINT16)(zBase[zSP+1])<<8) );	/* Pop MSB */
	pbPC = (zBase + zPC);	/* Point PC to our return address */
	zSP += 2;  /* Pop the word off */
}
#define Z80CALL z80Call()
void z80Call(void){
	ADJ_FUZZY_PC;
	zSP-=1;
	zBase[zSP]= (unsigned char)((zPC>>8)&0xff); /* MSB */
	zSP-=1; /* Back our stack up */
	zBase[zSP]= (unsigned char)((zPC   )&0xff); /* LSB */
}

#define Ret0_TRUE \
	Z80RETURN \
	z80_ICount -= 6 /*	dwElapsedTicks += 6;*/

static void Ret0(unsigned int fff);




//static void NO_INST(void)
//{
////	pbPC -= dwCount; /* Invalid instruction - back up */
//	ADJ_FUZZY_PC;/*?*/
////	dwReturnCode = (UINT32) pbPC - (UINT32) zBase;
//	dwOriginalCycles -= z80_ICount;
//	z80_ICount = 0;
//}
static void Rlc_c8(unsigned char *vvv){
#if 00
unsigned char bTemp2;
	zF &= ~(ZF | SF | HF | VF | NF | CF);
	bTemp2	= (*vvv >> 7);
	*vvv = (*vvv << 1) | bTemp2;
	zF |= bTemp2 | SZP[*vvv];
#else
//マチガッテルかも
	*vvv = (*vvv << 1) | (*vvv >> 7);
	zF = (zF & (SF | ZF | PF)) | (*vvv & (YF | XF | CF));
#endif
}
static void Rrc_c8(unsigned char *vvv){
#if 00
	zF &= ~(ZF | SF | HF | VF | NF | CF);
	zF |= (*vvv & CF);
	*vvv = (*vvv >> 1) | (*vvv << 7);
	zF |= SZP[*vvv];
#else
//マチガッテルかも
	zF = (zF & (SF | ZF | PF)) | (*vvv & (YF | XF | CF));
	*vvv = (*vvv >> 1) | (*vvv << 7);
#endif
}
static void Rl_c8(unsigned char *vvv){
#if 00
unsigned char bTemp2;
	bTemp2 = zF & CF;
	zF &= ~(ZF | SF | HF | VF | NF | CF);
	zF |= (*vvv >> 7);
	*vvv = (*vvv << 1) | bTemp2;
	zF |= SZP[*vvv];
#else
//マチガッテルかも
unsigned char res = (*vvv << 1) | (zF & CF);
unsigned char c = (*vvv & 0x80) ? CF : 0;
	zF = (zF & (SF | ZF | PF)) | c | (res & (YF | XF));
	*vvv = res;
#endif
}
static void Rr_c8(unsigned char *vvv){ /*???*/
#if 00
unsigned char bTemp2;
	bTemp2 = (zF & CF) << 7;
	zF &= ~(ZF | SF | HF | VF | NF | CF);
	zF |= (*vvv & CF);
	*vvv = (*vvv >> 1) | bTemp2;	/**!!(regB)___*/
	zF |= SZP[*vvv];
#else
//マチガッテルかも
unsigned char res = (*vvv >> 1) | (zF << 7);
unsigned char c = (*vvv & 0x01) ? CF : 0;
	zF = (zF & (SF | ZF | PF)) | c | (res & (YF | XF));
	*vvv = res;
#endif
}
static void Sla_c8(unsigned char *vvv){
#if 00
	zF &= ~(ZF | SF | HF | VF | NF | CF);
	zF |= (*vvv >> 7);
	*vvv = (*vvv << 1);
	zF |= SZP[*vvv];
#else
	unsigned char c = ((*vvv & 0x80) ? CF : 0);
	*vvv = (*vvv << 1) & 0xff;
	zF = SZP[*vvv] | c;
#endif
}
static void Sra_c8(unsigned char *vvv){
#if 00
	zF &= ~(ZF | SF | HF | VF | NF | CF);
	zF |= (*vvv & CF);
	*vvv = (*vvv >> 1) | (*vvv & 0x80);
	zF |= SZP[*vvv];
#else
	unsigned char c = ((*vvv & 0x01) ? CF : 0);
	*vvv = ((*vvv >> 1) | (*vvv & 0x80)) & 0xff;
	zF = SZP[*vvv] | c;
#endif
}
//#define Sll_c8 Sla_c8
//static void Sll_c8(unsigned char *vvv){
//	zF &= ~(ZF | SF | HF | VF | NF | CF);
//	zF |= (*vvv >> 7);
//	*vvv = (*vvv << 1);
//	zF |= SZP[*vvv];
//}
static void Srl_c8(unsigned char *vvv){
#if 00
	zF &= ~(ZF | SF | HF | VF | NF | CF);
	zF |= (*vvv & CF);
	*vvv = (*vvv >> 1);
	zF |= SZP[*vvv];
#else
	unsigned char c = ((*vvv & 0x01) ? CF : 0);
	*vvv = (*vvv >> 1) & 0xff;
	zF = SZP[*vvv] | c;
#endif
}
static void bit_c8(unsigned char mask){
	zF &= ~(HF | NF | ZF);
	zF |= (HF);
	if(!(mask))
	{
		zF |= ZF;
	}
}
static void bit_xhl(unsigned char mask){
	zF &= ~(HF | NF | ZF);
	zF |= (HF);
	if(!(RM(zHL)&mask))
	{
		zF |= ZF;
	}
	//z80_ICount -= 12;
	z80_ICount += (15-12+4);
}
static void HandleCB(void)
{
	m_opcode =*pbPC;
	pbPC++;
	ADJ_FUZZY_PC;

	if(0x06==(m_opcode&0x07)){
		/*HL*/
		z80_ICount -= (15-4); /* */
		switch(m_opcode&0xc0)
		{
		case 0x00:	/* 0x00-0x3f */
			switch(m_opcode)
			{
		case 0x06:
{unsigned char bTemp1;bTemp1=RM(zHL);Rlc_c8(&bTemp1);WM( zHL, bTemp1);}
					break; //CB: rlc_xhl
		case 0x0e:
{unsigned char bTemp1;bTemp1=RM(zHL);Rrc_c8(&bTemp1);WM( zHL, bTemp1);}
					break; //CB: rrc_xhl
		case 0x16:
{unsigned char bTemp1;bTemp1=RM(zHL);Rl_c8(&bTemp1);WM( zHL, bTemp1);}
					break; //CB: rl_xhl
		case 0x1e:
{unsigned char bTemp1;bTemp1=RM(zHL);Rr_c8(&bTemp1);WM( zHL, bTemp1);}
					break; //CB: rr_xhl
		case 0x36:
		case 0x26:
{unsigned char bTemp1;bTemp1=RM(zHL);Sla_c8(&bTemp1);WM( zHL, bTemp1);}
					break; //CB: sla_xhl
		case 0x2e:
{unsigned char bTemp1;bTemp1=RM(zHL);Sra_c8(&bTemp1);WM( zHL, bTemp1);}
					break; //CB: sra_xhl
// Sll_c8 == Sla_c8
//		case 0x36: WM( zHL, Sll_c8(RM(zHL)) );
//					break; //CB: sll_xhl
		case 0x3e:
{unsigned char bTemp1;bTemp1=RM(zHL);Srl_c8(&bTemp1);WM( zHL, bTemp1);}
					break; //CB: srl_xhl
			}
			break;
		case 0x40:	/* 0x40-0x7f */ //0x46:0x4e:0x56:0x5e:0x66:0x6e:0x76:0x7e:
			{
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((m_opcode&0x38)>>3));
			bit_xhl(bit_pattarn); //CB: bit_#_xhl
			}
			break;
		case 0x80:	/* 0x80-0xbf */ //0x86:0x8e:0x96:0x9e:0xa6:0xae:0xb6:0xbe:
			{
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((m_opcode&0x38)>>3));
			bit_pattarn = (~bit_pattarn);
			WM(zHL, ((RM(zHL))&bit_pattarn) );
			}
			break;
		case 0xc0:	/* 0xc0-0xff */ //	0xc6:0xce:0xd6:0xde:0xe6:0xee:0xf6:0xfe:
			{/* 0011 1000 */
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((m_opcode&0x38)>>3));
			WM(zHL, ((RM(zHL))|bit_pattarn) );
			}
			break;
		}
	}else{
		/*a,b,c,d,e,h,l*/
		z80_ICount -= (8-4);
		switch(m_opcode&0xc0)
		{
		case 0x00:	/* 0x00-0x3f */
			switch(m_opcode)
			{
			case 0x00:	Rlc_c8(&zB);	break; //CB: rlc_b
			case 0x01:	Rlc_c8(&zC);	break; //CB: rlc_c
			case 0x02:	Rlc_c8(&zD);	break; //CB: rlc_d
			case 0x03:	Rlc_c8(&zE);	break; //CB: rlc_e
			case 0x04:	Rlc_c8(&zH);	break; //CB: rlc_h
			case 0x05:	Rlc_c8(&zL);	break; //CB: rlc_l
			case 0x07:	Rlc_c8(&zA);	break; //CB: rlc_a

			case 0x08:	Rrc_c8(&zB);	break; //CB: rrc_b
			case 0x09:	Rrc_c8(&zC);	break; //CB: rrc_c
			case 0x0a:	Rrc_c8(&zD);	break; //CB: rrc_d
			case 0x0b:	Rrc_c8(&zE);	break; //CB: rrc_e
			case 0x0c:	Rrc_c8(&zH);	break; //CB: rrc_h
			case 0x0d:	Rrc_c8(&zL);	break; //CB: rrc_l
			case 0x0f:	Rrc_c8(&zA);	break; //CB: rrc_a

			case 0x10:	Rl_c8(&zB); break; //CB: rl_b
			case 0x11:	Rl_c8(&zC); break; //CB: rl_c
			case 0x12:	Rl_c8(&zD); break; //CB: rl_d
			case 0x13:	Rl_c8(&zE); break; //CB: rl_e
			case 0x14:	Rl_c8(&zH); break; //CB: rl_h
			case 0x15:	Rl_c8(&zL); break; //CB: rl_l
			case 0x17:	Rl_c8(&zA); break; //CB: rl_a

			case 0x18:	Rr_c8(&zB); break; //CB: rr_b
			case 0x19:	Rr_c8(&zC); break; //CB: rr_c
			case 0x1a:	Rr_c8(&zD); break; //CB: rr_d
			case 0x1b:	Rr_c8(&zE); break; //CB: rr_e
			case 0x1c:	Rr_c8(&zH); break; //CB: rr_h
			case 0x1d:	Rr_c8(&zL); break; //CB: rr_l
			case 0x1f:	Rr_c8(&zA); break; //CB: rr_a

			// Sll_c8 == Sla_c8
case 0x20:	case 0x30://Sll_c8(&zB);	break; //CB: sll_b
						Sla_c8(&zB);	break; //CB: sla_b
case 0x21:	case 0x31://Sll_c8(&zC);	break; //CB: sll_c
						Sla_c8(&zC);	break; //CB: sla_c
case 0x22:	case 0x32://Sll_c8(&zD);	break; //CB: sll_d
						Sla_c8(&zD);	break; //CB: sla_d
case 0x23:	case 0x33://Sll_c8(&zE);	break; //CB: sll_e
						Sla_c8(&zE);	break; //CB: sla_e
case 0x24:	case 0x34://Sll_c8(&zH);	break; //CB: sll_h
						Sla_c8(&zH);	break; //CB: sla_h
case 0x25:	case 0x35://Sll_c8(&zL);	break; //CB: sll_l
						Sla_c8(&zL);	break; //CB: sla_l
case 0x27:	case 0x37://Sll_c8(&zA);	break; //CB: sll_a
						Sla_c8(&zA);	break; //CB: sla_a

			case 0x28:	Sra_c8(&zB);	break; //CB: sra_b
			case 0x29:	Sra_c8(&zC);	break; //CB: sra_c
			case 0x2a:	Sra_c8(&zD);	break; //CB: sra_d
			case 0x2b:	Sra_c8(&zE);	break; //CB: sra_e
			case 0x2c:	Sra_c8(&zH);	break; //CB: sra_h
			case 0x2d:	Sra_c8(&zL);	break; //CB: sra_l
			case 0x2f:	Sra_c8(&zA);	break; //CB: sra_a

			//Sll_c8 == Sla_c8
//			case 0x30:	Sll_c8(&zB);	break; //CB: sll_b
//			case 0x31:	Sll_c8(&zC);	break; //CB: sll_c
//			case 0x32:	Sll_c8(&zD);	break; //CB: sll_d
//			case 0x33:	Sll_c8(&zE);	break; //CB: sll_e
//			case 0x34:	Sll_c8(&zH);	break; //CB: sll_h
//			case 0x35:	Sll_c8(&zL);	break; //CB: sll_l
//			case 0x37:	Sll_c8(&zA);	break; //CB: sll_a

			case 0x38:	Srl_c8(&zB);	break; //CB: srl_b
			case 0x39:	Srl_c8(&zC);	break; //CB: srl_c
			case 0x3a:	Srl_c8(&zD);	break; //CB: srl_d
			case 0x3b:	Srl_c8(&zE);	break; //CB: srl_e
			case 0x3c:	Srl_c8(&zH);	break; //CB: srl_h
			case 0x3d:	Srl_c8(&zL);	break; //CB: srl_l
			case 0x3f:	Srl_c8(&zA);	break; //CB: srl_a
			}
			break;
		case 0x40:	/* 0x40-0x7f */
			{/* 0011 1000 */
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((m_opcode&0x38)>>3));
				switch(m_opcode&0x07){
					case 0x00: bit_c8((zB & bit_pattarn));	break; //CB: bit_#_b
					case 0x01: bit_c8((zC & bit_pattarn));	break; //CB: bit_#_c
					case 0x02: bit_c8((zD & bit_pattarn));	break; //CB: bit_#_d
					case 0x03: bit_c8((zE & bit_pattarn));	break; //CB: bit_#_e
					case 0x04: bit_c8((zH & bit_pattarn));	break; //CB: bit_#_h
					case 0x05: bit_c8((zL & bit_pattarn));	break; //CB: bit_#_l
					case 0x07: bit_c8((zA & bit_pattarn));	break; //CB: bit_#_a
				}
			}
			break;
		case 0x80:	/* 0x80-0xbf */
			{
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((m_opcode&0x38)>>3));
			bit_pattarn = (~bit_pattarn);
				switch(m_opcode&0x07){
					case 0x00: zB &= bit_pattarn;	break; //CB: res_#_b
					case 0x01: zC &= bit_pattarn;	break; //CB: res_#_c
					case 0x02: zD &= bit_pattarn;	break; //CB: res_#_d
					case 0x03: zE &= bit_pattarn;	break; //CB: res_#_e
					case 0x04: zH &= bit_pattarn;	break; //CB: res_#_h
					case 0x05: zL &= bit_pattarn;	break; //CB: res_#_l
					case 0x07: zA &= bit_pattarn;	break; //CB: res_#_a
				}
			}
			break;
		case 0xc0:	/* 0xc0-0xff */
			{
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((m_opcode&0x38)>>3));
				switch(m_opcode&0x07){
					case 0x00: zB |= bit_pattarn;	break; //CB: set_#_b
					case 0x01: zC |= bit_pattarn;	break; //CB: set_#_c
					case 0x02: zD |= bit_pattarn;	break; //CB: set_#_d
					case 0x03: zE |= bit_pattarn;	break; //CB: set_#_e
					case 0x04: zH |= bit_pattarn;	break; //CB: set_#_h
					case 0x05: zL |= bit_pattarn;	break; //CB: set_#_l
					case 0x07: zA |= bit_pattarn;	break; //CB: set_#_a
				}
			}
			break;
		}
	}/* ends of if(HL) */
}
static unsigned char pbSubSbcTable[ 256*256*2 ] __attribute__((aligned(64))) ;
static void HandleED(void)
{
	m_opcode =*pbPC;
	pbPC++;
	ADJ_FUZZY_PC;
	switch((m_opcode&0xc0))
	{
//	case 0x00:<NOP>
//		break;
	case 0x40:
		switch((m_opcode&0x07))
		{
		case 0x00:/* 0x#0 0x#8 */
			switch(m_opcode)
			{/* [force] Unclaimed I/O read to get '0xff'! */
			case 0x40:zB = 0xff;break;//ED: in_b_c
			case 0x48:zC = 0xff;break;//ED: in_c_c
			case 0x50:zD = 0xff;break;//ED: in_d_c
			case 0x58:zE = 0xff;break;//ED: in_e_c
			case 0x60:zH = 0xff;break;//ED: in_h_c
			case 0x68:zL = 0xff;break;//ED: in_l_c
			case 0x70:/*zL = 0xff;*/break;//ED: in_0_c
			case 0x78:zA = 0xff;break;//ED: in_a_c
//				dwAddr = zC;
//				psIoRead = zIoRead; /* Beginning of our handler */
//				{
//					ADJ_FUZZY_PC;
//					zA = psIoRead->IOCall(dwAddr, psIoRead);
//				}
			}
				zF &= ~(SF | ZF | HF | VF | NF);
				zF |= 0x84;//SZP[z#];
				z80_ICount -= (12-4);
			break;
		case 0x01:/* 0x#1 0x#9 *//* port output */
		//	case 0x41: //ED: out_c_b
		//	case 0x49: //ED: out_c_c
		//	case 0x51: //ED: out_c_d
		//	case 0x59: //ED: out_c_e
		//	case 0x61: //ED: out_c_h
		//	case 0x69: //ED: out_c_l
		//	case 0x71: //ED: out_c_0
		//	case 0x79: //ED: out_c_a
//				dwAddr = zC;
//				psIoWrite = zIoWrite; /* Beginning of our handler */
//				{
//					ADJ_FUZZY_PC;
//					psIoWrite->IOCall(dwAddr, z#, psIoWrite);
//				}
				z80_ICount -= (12-4);
			break;
		case 0x02:/* 0x#2 0x#a */
			z80_ICount -= (15-4);
			switch(m_opcode)
			{
			case 0x42: //ED: sbc_hl_bc
				{unsigned long dwTemp5;
				dwTemp5 = zHL - zBC - (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zBC) >> 8) & HF);
				zF |= ((((zBC ^ zHL) & (zBC ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			case 0x4a: //ED: adc_hl_bc
				{unsigned long dwTemp5;
				dwTemp5 = zHL + zBC + (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zBC) >> 8) & HF);
				zF |= ((((zBC ^ zHL ^ 0x8000) & (zBC ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			case 0x52: //ED: sbc_hl_de
				{unsigned long dwTemp5;
				dwTemp5 = zHL - zDE - (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zDE) >> 8) & HF);
				zF |= ((((zDE ^ zHL) & (zDE ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			case 0x5a: //ED: adc_hl_de
				{unsigned long dwTemp5;
				dwTemp5 = zHL + zDE + (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zDE) >> 8) & HF);
				zF |= ((((zDE ^ zHL ^ 0x8000) & (zDE ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			case 0x62: //ED: sbc_hl_hl
				{unsigned long dwTemp5;
				dwTemp5 = zHL - zHL - (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zHL) >> 8) & HF);
				zF |= ((((zHL ^ zHL) & (zHL ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			case 0x6a: //ED: adc_hl_hl
				{unsigned long dwTemp5;
				dwTemp5 = zHL + zHL + (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zHL) >> 8) & HF);
				zF |= ((((zHL ^ zHL ^ 0x8000) & (zHL ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			case 0x72: //ED: sbc_hl_sp
				{unsigned long dwTemp5;
				dwTemp5 = zHL - zSP - (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zSP) >> 8) & HF);
				zF |= ((((zSP ^ zHL) & (zSP ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			case 0x7a: //ED: adc_hl_sp
				{unsigned long dwTemp5;
				dwTemp5 = zHL + zSP + (zF & CF);
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= ((dwTemp5 >> 8) & SF);
				if(0 == (dwTemp5 & 0xffff))
				{
					zF |= ZF;
				}
				zF |= (((zHL ^ dwTemp5 ^ zSP) >> 8) & HF);
				zF |= ((((zSP ^ zHL ^ 0x8000) & (zSP ^ dwTemp5)) >> 13) & VF);
				zF |= ((dwTemp5 >> 16) & CF);
				zHL = dwTemp5 & 0xffff;
				}
				break;
			}
			break;
		case 0x03:/* 0x#3 0x#b */
			z80_ICount -= (20-4);
			switch(m_opcode)
			{
//efine ImmedWord() (*pbPC | ( pbPC[1] << 8)) &0x00ffff
#define ImmedWord() (*pbPC | ( pbPC[1] << 8))

			case 0x43: //ED: ld_xword_bc
				{unsigned short dwTemp4;
				dwTemp4=ImmedWord();
				WM(dwTemp4	, (zBC & 0xff) );
				WM(dwTemp4+1, (zBC >> 8) );
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			case 0x4b: //ED: ld_bc_xword
				{unsigned short dwTemp3;
				dwTemp3=ImmedWord();
				zBC  = RM(dwTemp3 );
				zBC |= ((UINT16)RM(dwTemp3+1 ) << 8);
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			case 0x53: //ED: ld_xword_de
				{unsigned short dwTemp4;
				dwTemp4=ImmedWord();
				WM(dwTemp4	, (zDE & 0xff) );
				WM(dwTemp4+1, (zDE >> 8) );
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			case 0x5b: //ED: ld_de_xword
				{unsigned short dwTemp3;
				dwTemp3=ImmedWord();
				zDE = RM(dwTemp3 );
				zDE |= ((UINT16)RM(dwTemp3+1 ) << 8);
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			case 0x63: //ED: ld_xword_hl
				{unsigned short dwTemp4;
				dwTemp4=ImmedWord();
				WM(dwTemp4	, (zHL & 0xff) );
				WM(dwTemp4+1, (zHL >> 8) );
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			case 0x6b: //ED: ld_hl_xword // 適当？？？
				{unsigned short dwTemp3;
				dwTemp3=ImmedWord();
				zHL = RM(dwTemp3 );
				zHL |= ((UINT16)RM(dwTemp3+1 ) << 8);
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			case 0x73: //ED: ld_xword_sp
				{unsigned short dwTemp4;
				dwTemp4=ImmedWord();
				WM(dwTemp4	, (zSP & 0xff) );
				WM(dwTemp4+1, (zSP >> 8) );
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			case 0x7b: //ED: ld_sp_xword
				{unsigned short dwTemp3;
				dwTemp3=ImmedWord();
				zSP = RM(dwTemp3 );
				zSP |= ((UINT16)RM(dwTemp3+1 ) << 8);
				}pbPC+=2;
				ADJ_FUZZY_PC;
				break;
			}
			break;
		case 0x04:/* 0x#4 0x#c */
//			switch(m_opcode)
//			{
//			case 0x44: //ED: neg
//			case 0x4c: //ED: neg
//			case 0x54: //ED: neg
//			case 0x5c: //ED: neg
//			case 0x64: //ED: neg
//			case 0x6c: //ED: neg
//			case 0x74: //ED: neg
//			case 0x7c: //ED: neg
				NEG;
				z80_ICount -= (8-4);
//				break;
//			}
			break;
		case 0x05:/* 0x#5 0x#d */
			z80_ICount -= (14-4);	//14==(8+6)
			if(m_opcode&0x08){//ED: reti (return IRQ:Return for normal interrupt.)
#if 1
				Z80RETURN;
#else
//				Ret0(1);
#endif
			}else{ //ED: retn (return NMI:Return for non-maskable interrupt.)
				zIFF1 = zIFF2;//
#if 1
		//		zIFXX &= ~(IFF1);	/* Keep IFF2 around */
		//		zIFXX |= ((zIFXX >> 1) & IFF1); /* IFF2->IFF1 */
				Z80RETURN;
			//	ADJ_FUZZY_PC;
//				Ret0(1);	//	Ret0_TRUE;
#else
			//	zIFF1 = zIFF2;//手抜き
				Ret0(1);	//	Ret0_TRUE;
#endif
			}
		//	case 0x45: //ED: retn
		//	case 0x55: //ED: retn
		//	case 0x65: //ED: retn
		//	case 0x75: //ED: retn

		//	case 0x4d: //ED: reti
		//	case 0x5d: //ED: reti
		//	case 0x6d: //ED: reti
		//	case 0x7d: //ED: reti
			break;
		case 0x06:/* 0x#6 0x#e */
#if 00
			switch(m_opcode)// [---1 1---]
			{
			case 0x46: //ED: im_9(00)// [0100 0110]
			case 0x66: //ED: im_9(00)// [0110 0110]
				zIM = IM9;
				z80_ICount -= (13-4);//8;//13??
				break;
			case 0x4e: //ED: im_0(01)// [0100 1110]
			case 0x6e: //ED: im_0(01)// [0110 1110]
				zIM = IM0;
				z80_ICount -= (13-4);//8;//13??
				break;
			case 0x56: //ED: im_1(02)// [0101 0110]
			case 0x76: //ED: im_1(02)// [0111 0110]
				zIM = IM1;
		//		z_int_addr = 0x38;//??
				z80_ICount -= (13-4);//8;
				break;
			case 0x5e: //ED: im_2(03)// [0101 1110]
			case 0x7e: //ED: im_2(03)// [0111 1110]
				zIM = IM2;
				z80_ICount -= (19-4);//8;
				break;
			}
#endif
			zIM = ((m_opcode>>3)&0x03);
		//	if(IM2==zIM){z80_ICount -= 6;}
		//	z80_ICount -= (13-4);

			z80_ICount -= (8-4);//???
			break;
		case 0x07:/* 0x#7 0x#f */
			switch(m_opcode)
			{
			case 0x47: //ED: ld_i_a
				zI = zA;
				z80_ICount -= (9-4);
				break;
			case 0x4f: //ED: ld_r_a
				/*zR = zA;*/
				z80_ICount -= (9-4);
				break;
			case 0x57: //ED: ld_a_i
				zF &= ~(HF | VF | NF | ZF | SF);
			//	zF |= ((zIFXX & IFF2) << 1);
			//	zF |= ((zIFF2) << 2);
			//	zF |= ((zIFF2)?(0x04):(0x00));
				zF |=  (zIFF2)	;
				zA = zI;
				zF |= SZP[zA];
				z80_ICount -= (9-4);
				break;
			case 0x5f: //ED: ld_a_r
#if 00
				zF &= ~(HF | SF | ZF | VF | NF);
				zF |= SZP[zR];
			//	zF = (zF & ~(VF)) | ((zIFXX & IFF2) << 1);
			//	zF = (zF & ~(VF)) | ((zIFF2) << 2);
			//	zF = (zF & ~(VF)) | ((zIFF2)?(0x04):(0x00));
				zF = (zF & ~(VF)) |  (zIFF2)  ;
				zA = zR;
#define bTempAAA ((zR + (zB + z80_ICount-4+1 + zH)) ^ zA)
				zR = (zR & 0x80) | (bTempAAA & 0x7f);
#undef bTempAAA
#else
/*fuzzy it!!*/
	zA = (zA+7)&0xff;
	zF = (zF & CF) | SZP[zA] |	(zIFF2)  ;
#endif
				z80_ICount -= (9-4);
				break;
			case 0x67: //ED: rrd
				{unsigned char bTemp1;
				unsigned char bTemp2;
				bTemp1 = RM(zHL );
				bTemp2 = (zA & 0x0f) << 4;
				zA = (zA & 0xf0) | (bTemp1 & 0x0f);
				bTemp1 = (bTemp1 >> 4) | bTemp2;
				WM(zHL, bTemp1 );
				}
				zF &= ~(SF | ZF | HF | VF | NF);
				zF |= SZP[zA];
				z80_ICount -= (18-4);
				break;
			case 0x6f: //ED: rld
				{unsigned char bTemp1;
				{unsigned char bTemp2;
				bTemp1 = RM(zHL );
				bTemp2 = (zA & 0x0f);
				zA = (zA & 0xf0) | (bTemp1 >> 4);
				bTemp1 = (bTemp1 << 4) | bTemp2;
				}
				WM(zHL, bTemp1 );
				}
				zF &= ~(SF | ZF | HF | VF | NF);
				zF |= SZP[zA];
				z80_ICount -= (18-4);
				break;
			case 0x77: //ED: (rrl)
//			//INST2NO_BRK //ED:  nop//ワカランつ（多分ここはＮＯＰじゃないよ）
//				z80_ICount -= (18-4);
//				break;
			case 0x7f: //ED: (rlr)
			//INST2NO_BRK //ED:  nop//ワカランつ（多分ここはＮＯＰじゃないよ）
				z80_ICount -= (18-4);
				break;
			}
			break;
		}
		break;
////////////////////////////////////////
	case 0x80:
		if((m_opcode&0xe0)==0x80){ /* [###- ----] == [100- ----]; 0x80 or 0x90 */
			z80_ICount -= (8-4);//??
		}else{
			switch((m_opcode&0x07))
			{
			case 0x00:/* 0x#0 0x#8 */
				switch(m_opcode)
				{
				case 0xa0: //ED: ldi
					WM(zDE, RM(zHL ) );
					zHL++;	zHL &= 0xffff;
					zDE++;	zDE &= 0xffff;
					zBC--;	zBC &= 0xffff;
					zF &= ~(HF | NF | VF);
					if(zBC)
					{
						zF |= VF;
					}
					z80_ICount -= (16-4);
					break;
				case 0xa8: //ED: ldd
					WM(zDE, RM(zHL ) );
					zHL--;	zHL &= 0xffff;
					zDE--;	zDE &= 0xffff;
					zBC--;	zBC &= 0xffff;
					zF &= ~(HF | NF | VF);
					if(zBC)
					{
						zF |= VF;
					}
					z80_ICount -= (16-4);
					break;
/* ぷよぷよ１が音でないのは多分こいつのせいだだだだだっだあああ。 */
/* やっぱＬＤＩＲはループしなきゃ！！（が、都合により保留...） */
				case 0xb0: //ED: ldir
					//while ((z80_ICount > 0) && (zBC))
					//do
					{
						WM(zDE, RM(zHL) );
						zHL++;	zHL &= 0xffff;
						zDE++;	zDE &= 0xffff;
						zBC--;	zBC &= 0xffff;
						z80_ICount -= 21;
					}//while ((0==(z80_ICount & 0xff00)) && (zBC)) ;
//差し当たりループ排除a（仮対応）
			//		}while ((z80_ICount > 0) && (zBC)) ;
//					}while (--zBC ) ;
					zF &= ~(HF | NF | VF);
					if(zBC)
					{
						zF |= VF;
						pbPC -= 2;	/* Back up so we hit this instruction again */
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					ADJ_FUZZY_PC;/*?*/
					break;
				case 0xb8: //ED: lddr
					//while ((z80_ICount > 0) && (zBC))
					//do
					{
						WM(zDE, RM(zHL) );
						zHL--;	zHL &= 0xffff;
						zDE--;	zDE &= 0xffff;
						zBC--;	zBC &= 0xffff;
						z80_ICount -= 21;
	//				}while ((z80_ICount > 0) && (zBC)) ;
	//				}while (  (zBC)) ;
					}//while ((0==(z80_ICount & 0xff00)) && (zBC)) ;
//差し当たりループ排除b（仮対応）
					zF &= ~(HF | NF | VF);
					if(zBC)
					{
						zF |= VF;
						pbPC -= 2;	/* Back up so we hit this instruction again */
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					ADJ_FUZZY_PC;
					break;
				}
			break;
			case 0x01:/* 0x#1 0x#9 */
				switch(m_opcode)
				{
				case 0xa1: //ED: cpi
					{unsigned char bTemp1;
					bTemp1 = RM(zHL );
					zHL++;	zHL &= 0xffff;
					zBC--;	zBC &= 0xffff;
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (pbSubSbcTable[((UINT32) zA << 8) | bTemp1] & (SF | NF | ZF));
					}
					if(zBC)
					{
						zF |= VF;
					}
					z80_ICount -= (16-4);
					break;
				case 0xa9: //ED: cpd
					{unsigned char bTemp1;
					bTemp1 = RM(zHL );
					zHL--;	zHL &= 0xffff;
					zBC--;	zBC &= 0xffff;
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (pbSubSbcTable[((UINT32) zA << 8) | bTemp1] & (SF | NF | ZF));
					}
					if(zBC)
					{
						zF |= VF;
					}
					z80_ICount -= (16-4);
					break;
/* ぷよぷよ１が音でないのは多分こいつのせいだだだだだっだあああ。 */
/* やっぱＣＰＩＲはループしなきゃ！！（が、都合により保留...） */
				case 0xb1: //ED: cpir
					//while ((z80_ICount >= 0) && (zBC))
					{
					unsigned char i;
					unsigned char j;
					//do
					{
						i = RM(zHL );
						j = zA-i;
						zHL++;	zHL &= 0xffff;
						zBC--;	zBC &= 0xffff;
						z80_ICount -= 21;
					}//while ((0==(z80_ICount & 0xff00)) && (zBC)) ;
//差し当たりループ排除a（仮対応）
				//	}while ((z80_ICount >= 0) && (zBC)) ;
//					}while ((0==(z80_ICount & 0xff00)) && j && (zBC)) ;
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (pbSubSbcTable[((UINT32) zA << 8) | i] & (SF | NF | ZF));
					if(zBC && j)
					{
						zF |= VF;
						pbPC -= 2;	/* Back up so we hit this instruction again */
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					}
					ADJ_FUZZY_PC;
					break;
				case 0xb9: //ED: cpdr
					{unsigned char bTemp1;
					//while ((z80_ICount >= 0) && (zBC))
					//do
					{
						bTemp1 = RM(zHL);
						zHL--;	zHL &= 0xffff;
						zBC--;	zBC &= 0xffff;
						z80_ICount -= 21;
						if(zA == bTemp1)
						{
							break;
						}
					}//while ((0==(z80_ICount & 0xff00)) && (zBC) ) ;
	//				}while ((z80_ICount > 0) && (zBC)) ;
	//				}while ((0==(z80_ICount & 0xff00)) && (zB)) ;
//差し当たりループ排除b（仮対応）
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (pbSubSbcTable[((UINT32) zA << 8) | bTemp1] & (SF | NF | ZF));
					}
					if(zBC)
					{
						zF |= VF;
						pbPC -= 2;	/* Back up so we hit this instruction again */
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					ADJ_FUZZY_PC;
					break;
				}
			break;
#if 1//IO_000000
			default:/*dummy I/O */
					z80_ICount -= (16-4);
					break;
#else
			case 0x02:/* 0x#2 0x#a */
				switch(m_opcode)
				{
#ifdef STILL_IO
				case 0xa2: //ED: ini
					{
				//	psIoRead = zIoRead; /* Beginning of our handler */
				//	{
				//		ADJ_FUZZY_PC;
				//		bTemp = psIoRead->IOCall(zB, psIoRead);
				//	}
				//	bTemp = 0xff; /* [] Unclaimed I/O read */
				//	WM(zHL, bTemp );
					WM(zHL, 0xff  );
					zHL++;	zHL &= 0xffff;
					z80_ICount -= 16;
					zB--;
					}
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (/*SZP[0xff bTemp]*/0x84 & (SF | NF | HF | VF));//0x84;//
					if(zB)
					{
						zF |= VF;
						pbPC -= 2;
					}
					ADJ_FUZZY_PC;
					z80_ICount -= (16-4);
					break;
#endif
#ifdef STILL_IO
				case 0xaa: //ED: ind
					{
				//	psIoRead = zIoRead; /* Beginning of our handler */
				//	{
				//		ADJ_FUZZY_PC;
				//		bTemp = psIoRead->IOCall(zB, psIoRead);
				//	}
				//	bTemp = 0xff; /* [] Unclaimed I/O read */
					WM(zHL, /*bTemp*/0xff );
					zHL--;	//zHL &= 0xffff;
					zB--;
					}
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (/*SZP[0xff bTemp]*/0x84 & (SF | NF | HF | VF));
					if(zB)
					{
						zF |= VF;
						pbPC -= 2;
					}
					ADJ_FUZZY_PC;
					z80_ICount -= (16-4);
					break;
#endif
#ifdef STILL_IO
				case 0xb2: //ED: inir
					//while ((z80_ICount > 0) && (zB))
					do
					{
					//	psIoRead = zIoRead; /* Beginning of our handler */
					//	{
					//		ADJ_FUZZY_PC;
					//		bTemp = psIoRead->IOCall(zB, psIoRead);
					//	}
				//		bTemp = 0xff; /* [] Unclaimed I/O read */
						WM(zHL, /*bTemp*/0xff );
						zHL++;	zHL &= 0xffff;
						//z80_ICount -= 16;
						z80_ICount -= 21;
						zB--;
				//	}while ((z80_ICount > 0) && (zB)) ;
					}while ((0==(z80_ICount & 0xff00)) && (zB)) ;
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (/*SZP[0xff bTemp]*/0x84 & (SF | NF | HF | VF));
					if(zB)
					{
						pbPC -= 2;	/* Back up so we hit this instruction again */
						zF |= VF;
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					ADJ_FUZZY_PC;
					break;
#endif
#ifdef STILL_IO
				case 0xba: //ED: indr
					//while ((z80_ICount > 0) && (zB))
					do
					{
		//				psIoRead = zIoRead; /* Beginning of our handler */
		//				{
		//					ADJ_FUZZY_PC;
		//					bTemp = psIoRead->IOCall(zB, psIoRead);
		//				}
					//	bTemp = 0xff; /* [] Unclaimed I/O read */
						WM(zHL, 0xff/*bTemp*/ );
						zHL--;	//zHL &= 0xffff;
						//z80_ICount -= 16;
						z80_ICount -= 21;
						zB--;
	//				}while ((z80_ICount > 0) && (zB)) ;
					}while ((0==(z80_ICount & 0xff00)) && (zB)) ;
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (/*SZP[0xff bTemp]*/0x84 & (SF | NF | HF | VF));//0x84;//
					if(zB)
					{
						pbPC -= 2;	/* Back up so we hit this instruction again */
						zF |= VF;
						pbPC -= 2;
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					ADJ_FUZZY_PC;
					break;
#endif
				}
			break;
			case 0x03:/* 0x#3 0x#b */
				switch(m_opcode)
				{
#ifdef STILL_IO
				case 0xa3: //ED: outi
					{
					bTemp = RM(zHL );
				//	psIoWrite = zIoWrite; /* Beginning of our handler */
				//	{
				//		ADJ_FUZZY_PC;
				//		psIoWrite->IOCall(zBC, bTemp, psIoWrite);
				//	}
					zHL++;	zHL &= 0xffff;
					zB--;
					}
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (SZP[bTemp] & (SF | NF | HF | VF));
					if(zB)
					{
						zF |= VF;
					}
					z80_ICount -= (16-4);
					break;
#endif
#ifdef STILL_IO
				case 0xab: //ED: outd
					{unsigned char bTemp1;
					{
					bTemp1 = RM(zHL );
				//	psIoWrite = zIoWrite; /* Beginning of our handler */
				//	{
				//		ADJ_FUZZY_PC;
				//		psIoWrite->IOCall(zBC, bTemp, psIoWrite);
				//	}
					zHL--;	//zHL &= 0xffff;
					zB--;
					}
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (SZP[bTemp1] & (SF | NF | HF | VF));
					}
					if(zB)
					{
						zF |= VF;
					}
					z80_ICount -= (16-4);
				break;
#endif
#ifdef STILL_IO
				case 0xb3: //ED: otir
					//while ((z80_ICount > 0) && (zB))
					do
					{
						bTemp = RM(zHL );
				//		psIoWrite = zIoWrite; /* Beginning of our handler */
				//		{
				//			ADJ_FUZZY_PC;
				//			psIoWrite->IOCall(zBC, bTemp, psIoWrite);
				//		}
						zHL++;	zHL &= 0xffff;
						//z80_ICount -= 16;
						z80_ICount -= 21;
						zB--;
	//				}while ((z80_ICount > 0) && (zB)) ;
					}while ((0==(z80_ICount & 0xff00)) && (zB)) ;
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (SZP[bTemp] & (SF | NF | HF | VF));
					if(zB)
					{
						pbPC -= 2;	/* Back up so we hit this instruction again */
						zF |= VF;
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					ADJ_FUZZY_PC;
					break;
#endif
#ifdef STILL_IO
				case 0xbb: //ED: otdr
					//while ((z80_ICount > 0) && (zB))
					do
					{
						bTemp = RM(zHL );
		//				psIoWrite = zIoWrite; /* Beginning of our handler */
		//				{
		//					ADJ_FUZZY_PC;
		//					psIoWrite->IOCall(zBC, bTemp, psIoWrite);
		//				}
						zHL--;	//zHL &= 0xffff;
						zB--;
						//z80_ICount -= 16;
						z80_ICount -= 21;
	//				}while ((z80_ICount > 0) && (zB)) ;
					}while ((0==(z80_ICount & 0xff00)) && (zB)) ;
					zF &= ~(ZF | SF | HF | VF | NF);
					zF |= (SZP[bTemp] & (SF | NF | HF | VF));
					if(zB)
					{
						pbPC -= 2;	/* Back up so we hit this instruction again */
						zF |= VF;
						z80_ICount -= 5;
					}
					z80_ICount -= (16-4);
					ADJ_FUZZY_PC;
					break;
#endif
				}
			break;
			case 0x04:/* 0x#4 0x#c */
			case 0x05:/* 0x#5 0x#d */
			case 0x06:/* 0x#6 0x#e */
			case 0x07:/* 0x#7 0x#f */
				z80_ICount -= (8-4);//??
			break;
#endif//IO_000000
			}
		}
		break;
//	case 0xc0://<NOP>
//		break;
	default://<NOP>
		z80_ICount -= (8-4); //適当でも少しかかる筈
	//	pbPC-=2;NO_INST();//死ぬのは止めよう。
		break;
	}
}
//////////
//			case 0xa4:INST2NO_BRK //ED: nop
//			case 0xa5:INST2NO_BRK //ED: nop
//			case 0xa6:INST2NO_BRK //ED: nop
//			case 0xa7:INST2NO_BRK //ED: nop
//		case 0xac:INST2NO_BRK //ED: nop
//		case 0xad:INST2NO_BRK //ED: nop
//		case 0xae:INST2NO_BRK //ED: nop
//		case 0xaf:INST2NO_BRK //ED: nop
//			case 0xb4:INST2NO_BRK //ED: nop
//			case 0xb5:INST2NO_BRK //ED: nop
//			case 0xb6:INST2NO_BRK //ED: nop
//			case 0xb7:INST2NO_BRK //ED: nop
//		case 0xbc:INST2NO_BRK //ED: nop
//		case 0xbd:INST2NO_BRK //ED: nop
//		case 0xbe:INST2NO_BRK //ED: nop
//		case 0xbf:INST2NO_BRK //ED: nop
//////////
//////////
//		case 0x00:INST2NO_BRK //ED: nop
//		...
//		case 0x3f:INST2NO_BRK //ED: nop
//////////
//		case 0x80:INST2NO_BRK //ED: nop
//		...
//		case 0x9f:INST2NO_BRK
//////////
//		case 0xc0:INST2NO_BRK //ED: nop
//		...
//		case 0xfd:INST2NO_BRK //ED: nop
//		case 0xfe:INST2NO_BRK //ED: patch
//		case 0xff:INST2NO_BRK //ED: nop
//////////
//////////


/////////////////
// FDCB / DDCB
//static void HandleDDCB(void/*UINT32 dwWhich*/) //IX
//static void HandleFDCB(void/*UINT32 dwWhich*/) //IY
static void HandleIXIY(void/*UINT32 dwWhich*/)
{
unsigned char bTemp1;

//	if(dwWhich){	dwAddr = (UINT32) ((INT32) zIY + ((INT32) *pbPC++)) & 0xffff;	}
//	else{			dwAddr = (UINT32) ((INT32) zIX + ((INT32) *pbPC++)) & 0xffff;	}
#if 00
				dwAddr = (UINT32) ((INT32) zIX + ((INT32) *pbPC++)) & 0xffff;
#else
				m_opcode =*pbPC;
				pbPC++;
		//		ADJ_FUZZY_PC;
				if(m_opcode&0x80){	dwAddr = 0xff00;}	/* sign extended */
				else{				dwAddr = 0x0000;}
				dwAddr += m_opcode;
				dwAddr += zIX;
				dwAddr &= 0x00ffff;
#endif

	bTemp1 = RM(dwAddr );

	m_opcode =*pbPC;
	pbPC++;
	ADJ_FUZZY_PC;

	if(0x40==(m_opcode & 0xc0)) /* 0x40-0x7f */
	{
//	const unsigned char my_tbl[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
		z80_ICount -= (20-4);//(4+16 ?)
		zF = (zF & ~(ZF | NF)) | HF;
	//	if(!(bTemp1 & (1<<((m_opcode>>3)&7)) )) 		{	zF |= ZF;	}
	//	if(!(bTemp1 & (my_tbl[((m_opcode>>3)&7)]) ))	{	zF |= ZF;	}
		if(!((bTemp1>>( ((m_opcode>>3)&7) ) )&1))	{	zF |= ZF;	}
#if 00
		switch(m_opcode & 0x78)
		case 0x40: // bit_0_xix / bit_0_xiy
		// 0x41: 0x42: 0x43: 0x44: 0x45: 0x46: 0x47:
				if(!(bTemp1 & 0x01))	{	zF |= ZF;	}	break;
		case 0x48: // bit_1_xix / bit_1_xiy
		// 0x49: 0x4a: 0x4b: 0x4c: 0x4d: 0x4e: 0x4f:
				if(!(bTemp1 & 0x02))	{	zF |= ZF;	}	break;
		case 0x50: // bit_2_xix / bit_2_xiy
		// 0x51: 0x52: 0x53: 0x54: 0x55: 0x56: 0x57:
				if(!(bTemp1 & 0x04))	{	zF |= ZF;	}	break;
		case 0x58: // bit_3_xix / bit_3_xiy
// 0x59: 0x5a: 0x5b: 0x5c: 0x5d: 0x5e: 0x5f:
				if(!(bTemp1 & 0x08))	{	zF |= ZF;	}	break;
		case 0x60: // bit_4_xix / bit_4_xiy
// 0x61: 0x62: 0x63: 0x64: 0x65: 0x66: 0x67:
				if(!(bTemp1 & 0x10))	{	zF |= ZF;	}	break;
		case 0x68: // bit_5_xix / bit_5_xiy
// 0x69: 0x6a: 0x6b: 0x6c: 0x6d: 0x6e: 0x6f:
				if(!(bTemp1 & 0x20))	{	zF |= ZF;	}	break;
		case 0x70: // bit_6_xix / bit_6_xiy
// 0x71: 0x72: 0x73: 0x74: 0x75: 0x76: 0x77:
				if(!(bTemp1 & 0x40))	{	zF |= ZF;	}	break;
		case 0x78: // bit_7_xix / bit_7_xiy
// 0x79: 0x7a: 0x7b: 0x7c: 0x7d: 0x7e: 0x7f:
				if(!(bTemp1 & 0x80))	{	zF |= ZF;	}	break;
#endif
	}else if(0x06==(m_opcode & 0x07)){ /* 0x#6 or 0x#e */
		z80_ICount -= (23-4);//(4+16+3 ?)
	//	ADJ_FUZZY_PC;
	switch (m_opcode & 0xf8)
	{
		case (0x06 & 0xf8): //DDCB/FDCB: rlc_xix / rlc_xiy
#if 1
		{
		unsigned char bTemp2;
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				bTemp2 = (bTemp1 >> 7);
				bTemp1 = (bTemp1 << 1) | bTemp2;
				zF |= bTemp2 | SZP[bTemp1];
		}
#else
		Rlc_c8(bTemp1);
#endif
			break;

		case (0x0e & 0xf8): //DDCB/FDCB: rrc_xiy
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				zF |= (bTemp1 & CF);
				bTemp1 = (bTemp1 >> 1) | (bTemp1 << 7);
				zF |= SZP[bTemp1];
			break;

		case (0x16 & 0xf8): //DDCB/FDCB: rl_xiy
		{
		unsigned char bTemp2;
				bTemp2 = zF & CF;
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				zF |= (bTemp1 >> 7);
				bTemp1 = (bTemp1 << 1) | bTemp2;
				zF |= SZP[bTemp1];
		}
			break;

		case (0x1e & 0xf8): //DDCB/FDCB: rr_xiy
		{
		unsigned char bTemp2;
				bTemp2 = (zF & CF) << 7;
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				zF |= (bTemp1 & CF);
				bTemp1 = (bTemp1 >> 1) | bTemp2;
				zF |= SZP[bTemp1];
		}
			break;

		case (0x26 & 0xf8): //DDCB/FDCB: sla_xiy
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				zF |= (bTemp1 >> 7);
				bTemp1 = (bTemp1 << 1);
				zF |= SZP[bTemp1];
			break;

		case (0x2e & 0xf8): //DDCB/FDCB: sra_xiy
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				zF |= (bTemp1 & CF);
				bTemp1 = (bTemp1 >> 1) | (bTemp1 & 0x80);
				zF |= SZP[bTemp1];
			break;

		case (0x36 & 0xf8): //DDCB/FDCB: sll_xiy（よくワカンナイ多分）
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				zF |= (bTemp1 & CF);
				bTemp1 = (bTemp1 << 1) ;
				zF |= SZP[bTemp1];
			break;

		case (0x3e & 0xf8): //DDCB/FDCB: srl_xiy
				zF &= ~(ZF | SF | HF | VF | NF | CF);
				zF |= (bTemp1 & CF);
				bTemp1 = (bTemp1 >> 1);
				zF |= SZP[bTemp1];
			break;

		case (0x86 & 0xf8): bTemp1 &= 0xfe; break; //DDCB/FDCB: res_0_xiy//??????
		case (0x8e & 0xf8): bTemp1 &= 0xfd; break; //DDCB/FDCB: res_1_xiy//??????
		case (0x96 & 0xf8): bTemp1 &= 0xfb; break; //DDCB/FDCB: res_2_xiy//??????
		case (0x9e & 0xf8): bTemp1 &= 0xf7; break; //DDCB/FDCB: res_3_xiy
		case (0xa6 & 0xf8): bTemp1 &= 0xef; break; //DDCB/FDCB: res_4_xiy
		case (0xae & 0xf8): bTemp1 &= 0xdf; break; //DDCB/FDCB: res_5_xiy
		case (0xb6 & 0xf8): bTemp1 &= 0xbf; break; //DDCB/FDCB: res_6_xiy
		case (0xbe & 0xf8): bTemp1 &= 0x7f; break; //DDCB/FDCB: res_7_xiy

		case (0xc6 & 0xf8): bTemp1 |= 0x01; break; //DDCB/FDCB: set_0_xiy
		case (0xce & 0xf8): bTemp1 |= 0x02; break; //DDCB/FDCB: set_1_xiy
		case (0xd6 & 0xf8): bTemp1 |= 0x04; break; //DDCB/FDCB: set_2_xiy
		case (0xde & 0xf8): bTemp1 |= 0x08; break; //DDCB/FDCB: set_3_xiy
		case (0xe6 & 0xf8): bTemp1 |= 0x10; break; //DDCB/FDCB: set_4_xiy
		case (0xee & 0xf8): bTemp1 |= 0x20; break; //DDCB/FDCB: set_5_xiy
		case (0xf6 & 0xf8): bTemp1 |= 0x40; break; //DDCB/FDCB: set_6_xiy
		case (0xfe & 0xf8): bTemp1 |= 0x80; break; //DDCB/FDCB: set_7_xiy
	}
	//		dwAddr &= 0x00ffff; /* my_masked */
	WM(dwAddr, bTemp1 );

	}else{
	//		z80_ICount -= 9; //適当でも少しかかる筈
	//		z80_ICount -= 8; //解からんが少し変えた。(4[pre-fhech]+4[nop?])仮に最速でも８クロック？？
			// DDxxでも、FDxxでもプリフェチが４クロックなら。ここに来るなら、当然４クロックは消費。(gens)
			z80_ICount -= (4-4+16); //もしかして。（多分こっち、「ぷよぷよ１」の勝った後の動作、少し良くなるタブン）
	//	pbPC-=4;NO_INST();//死ぬの禁止
		ADJ_FUZZY_PC;
	}
}

#define IndirectIX() (unsigned short)(zIX +  ((INT8) *pbPC++) )

static unsigned char pbAddAdcTable[ 256*256*2 ] __attribute__((aligned(64))) ;
//static void HandleFD(void)
static void HandleDD(void)
{
// DDxx命令は最低でも４クロックかかるらしい。(gens)
// FDxx命令は最低でも４クロックかかるらしい。(gens)
	m_opcode =*pbPC;
	pbPC++;
	ADJ_FUZZY_PC;
	switch (m_opcode)
	{
//		case 0x00:INST2NO_BRK //DD: nop
//		case 0x01:INST2NO_BRK
//		case 0x02:INST2NO_BRK
//		case 0x03:INST2NO_BRK
//		case 0x04:INST2NO_BRK
//		case 0x05:INST2NO_BRK
//		case 0x06:INST2NO_BRK
//		case 0x07:INST2NO_BRK
//		case 0x08:INST2NO_BRK
		case 0x09: //DD: add_ix_bc
			z80_ICount -= (15-4);//4+11 ??
			zF &= ~(CF | NF | HF);
			{unsigned long dwTemp5;
			dwTemp5 = zIX + zBC;
			zF |= ((dwTemp5 >> 16) & CF) | (((zIX ^ dwTemp5 ^ zBC) >> 8) & HF);
			zIX = dwTemp5 & 0xffff;
			}
			break;

//		case 0x0a:INST2NO_BRK
//		case 0x0b:INST2NO_BRK
//		case 0x0c:INST2NO_BRK
//		case 0x0d:INST2NO_BRK
//		case 0x0e:INST2NO_BRK
//		case 0x0f:INST2NO_BRK
//		case 0x10:INST2NO_BRK
//		case 0x11:INST2NO_BRK
//		case 0x12:INST2NO_BRK
//		case 0x13:INST2NO_BRK
//		case 0x14:INST2NO_BRK
//		case 0x15:INST2NO_BRK
//		case 0x16:INST2NO_BRK
//		case 0x17:INST2NO_BRK
//		case 0x18:INST2NO_BRK
		case 0x19: //DD: add_ix_de
			z80_ICount -= (15-4);//4+11 ??
			zF &= ~(CF | NF | HF);
			{unsigned long dwTemp5;
			dwTemp5 = zIX + zDE;
			zF |= ((dwTemp5 >> 16) & CF) | (((zIX ^ dwTemp5 ^ zDE) >> 8) & HF);
			zIX = dwTemp5 & 0xffff;
			}
			break;

//		case 0x1a:INST2NO_BRK
//		case 0x1b:INST2NO_BRK
//		case 0x1c:INST2NO_BRK
//		case 0x1d:INST2NO_BRK
//		case 0x1e:INST2NO_BRK
//		case 0x1f:INST2NO_BRK
//		case 0x20:INST2NO_BRK

		case 0x21: //DD: ld_ix_word
			zIX = ImmedWord();
			pbPC+=2;
			ADJ_FUZZY_PC;
			z80_ICount -= (14-4);//4+10 ??
			break;

		case 0x22: //DD: ld_xword_ix
			{unsigned short dwTemp4;
			dwTemp4=ImmedWord();
			WM(dwTemp4	, (zIX & 0xff) );
			WM(dwTemp4+1, (zIX >> 8) );
			}pbPC+=2;
			ADJ_FUZZY_PC;
			z80_ICount -= (20-4);//4+16 ??
			break;

		case 0x23:	zIX++;				z80_ICount -= (10-4);	break; //DD: inc_ix//4+6 ??

#define zF_INC(aaa) zF = (zF & CF) | SZHV_inc[aaa++]
#define zF_DEC(aaa) zF = (zF & CF) | SZHV_dec[aaa--]

		case 0x24:	zF_INC(zIXh);		z80_ICount -= (9-4);	break; //DD: inc_ixh//4+5 ??
		case 0x25:	zF_DEC(zIXh);		z80_ICount -= (9-4);	break; //DD: dec_ixh//4+5 ??

		case 0x26: //DD: ld_ixh_byte
			zIXh = *pbPC++;
			ADJ_FUZZY_PC;
			z80_ICount -= (9-4);//4+5 ??
			break;

//		case 0x27:INST2NO_BRK //DD: nop
//		case 0x28:INST2NO_BRK //DD: nop

		case 0x29: //DD: add_ix_ix
			zF &= ~(CF | NF | HF);
			{unsigned long dwTemp5;
			dwTemp5 = zIX + zIX;
			zF |= ((dwTemp5 >> 16) & CF) | (((zIX ^ dwTemp5 ^ zHL) >> 8) & HF);
			zIX = dwTemp5 & 0xffff;
			}
			z80_ICount -= (15-4);//4+11 ??
			break;

		case 0x2a: //DD: ld_ix_xword
			{unsigned short dwTemp3;
			dwTemp3=ImmedWord();
			zIX = RM(dwTemp3 );
			zIX |= ((UINT16)RM(dwTemp3+1 ) << 8);
			}pbPC+=2;
			ADJ_FUZZY_PC;
			z80_ICount -= (20-4);//4+16 ??
			break;

		case 0x2b:	zIX--;			z80_ICount -= (10-4);	break; //DD: dec_ix//4+6 ??
		case 0x2c:	zF_INC(zIXl);	z80_ICount -= (9-4);	break; //DD: inc_ixl//4+5 ??
		case 0x2d:	zF_DEC(zIXl);	z80_ICount -= (9-4);	break; //DD: dec_ixl//4+5 ??

		case 0x2e: //DD: ld_ixl_byte
			zIXl = *pbPC++;
			ADJ_FUZZY_PC;
			z80_ICount -= (9-4);
			break;

//		case 0x2f:INST2NO_BRK //DD: nop
//		case 0x30:INST2NO_BRK //DD: nop
//		case 0x31:INST2NO_BRK //DD: nop
//		case 0x32:INST2NO_BRK //DD: nop
//		case 0x33:INST2NO_BRK //DD: nop

		case 0x34: //DD: inc_xix
				dwAddr = IndirectIX();
				dwAddr &= 0x00ffff; /* my_masked */
			{unsigned char bTemp1;
				bTemp1 = RM(dwAddr );
				zF_INC(bTemp1);
				WM(dwAddr, bTemp1 );
				}
				ADJ_FUZZY_PC;//
			z80_ICount -= (23-4);
			break;

		case 0x35: //DD: dec_xix
				dwAddr = IndirectIX();
				dwAddr &= 0x00ffff; /* my_masked */
			{unsigned char bTemp1;
				bTemp1 = RM(dwAddr );
				zF_DEC(bTemp1);
				WM(dwAddr, bTemp1 );
				}
				ADJ_FUZZY_PC;//
			z80_ICount -= (23-4);
			break;
		case 0x36: //DD: ld_xix_byte
				dwAddr = IndirectIX();
				dwAddr &= 0x00ffff; /* my_masked */

				WM(dwAddr, *pbPC++ );
				ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x37:INST2NO_BRK //DD: nop
//		case 0x38:INST2NO_BRK //DD: nop

		case 0x39: //DD: add_ix_sp
			zF &= ~(CF | NF | HF);
			{unsigned long dwTemp5;
			dwTemp5 = zIX + zSP;
			zF |= ((dwTemp5 >> 16) & CF) | (((zIX ^ dwTemp5 ^ zSP) >> 8) & HF);
			zIX = dwTemp5 & 0xffff;
			}
			z80_ICount -= (15-4);
			break;

//		case 0x3a:INST2NO_BRK
//		case 0x3b:INST2NO_BRK
//		case 0x3c:INST2NO_BRK
//		case 0x3d:INST2NO_BRK
//		case 0x3e:INST2NO_BRK
//		case 0x3f:INST2NO_BRK
//		case 0x40:INST2NO_BRK
//		case 0x41:INST2NO_BRK
//		case 0x42:INST2NO_BRK
//		case 0x43:INST2NO_BRK
		case 0x44:	zB = zIXh;	z80_ICount -= (9-4);	break; //DD: ld_b_ixh
		case 0x45:	zB = zIXl;	z80_ICount -= (9-4);	break; //DD: ld_b_ixl

		case 0x46: //DD: ld_b_xix
			zB = RM(IndirectIX() );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x47:INST2NO_BRK
//		case 0x48:INST2NO_BRK
//		case 0x49:INST2NO_BRK
//		case 0x4a:INST2NO_BRK
//		case 0x4b:INST2NO_BRK
		case 0x4c:	zC = zIXh;	z80_ICount -= (9-4);	break; //DD: ld_c_ixh
		case 0x4d:	zC = zIXl;	z80_ICount -= (9-4);	break; //DD: ld_c_ixl

		case 0x4e: //DD: ld_c_xix
			zC = RM(IndirectIX() );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x4f:INST2NO_BRK
//		case 0x50:INST2NO_BRK
//		case 0x51:INST2NO_BRK
//		case 0x52:INST2NO_BRK
//		case 0x53:INST2NO_BRK
		case 0x54:	zD = zIXh;	z80_ICount -= (9-4);	break; //DD: ld_d_ixh
		case 0x55:	zD = zIXl;	z80_ICount -= (9-4);	break; //DD: ld_d_ixl

		case 0x56:
			zD = RM(IndirectIX() );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x57:INST2NO_BRK
//		case 0x58:INST2NO_BRK
//		case 0x59:INST2NO_BRK
//		case 0x5a:INST2NO_BRK
//		case 0x5b:INST2NO_BRK
		case 0x5c:	zE = zIXh;	z80_ICount -= (9-4);	break;
		case 0x5d:	zE = zIXl;	z80_ICount -= (9-4);	break;

		case 0x5e:
			zE = RM(IndirectIX() );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x5f:INST2NO_BRK //DD: nop

		case 0x60:	zIXh = zB;	z80_ICount -= (9-4);	break; //DD: ld_ixh_b
		case 0x61:	zIXh = zC;	z80_ICount -= (9-4);	break; //DD: ld_ixh_c
		case 0x62:	zIXh = zD;	z80_ICount -= (9-4);	break; //DD: ld_ixh_d
		case 0x63:	zIXh = zE;	z80_ICount -= (9-4);	break; //DD: ld_ixh_e
		case 0x64:	zIXh = zH;	z80_ICount -= (9-4);	break; //DD: ld_ixh_h
		case 0x65:	zIXh = zL;	z80_ICount -= (9-4);	break; //DD: ld_ixh_l

		case 0x66: //DD: ld_h_xix
			zH = RM(IndirectIX() );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x67:	zIXh = zA;	z80_ICount -= (9-4);	break; //DD: ld_ixh_a
		case 0x68:	zIXl = zB;	z80_ICount -= (9-4);	break; //DD: ld_ixl_b
		case 0x69:	zIXl = zC;	z80_ICount -= (9-4);	break; //DD: ld_ixl_c
		case 0x6a:	zIXl = zD;	z80_ICount -= (9-4);	break; //DD: ld_ixl_d
		case 0x6b:	zIXl = zE;	z80_ICount -= (9-4);	break; //DD: ld_ixl_e
		case 0x6c:	zIXl = zH;	z80_ICount -= (9-4);	break; //DD: ld_ixl_h
		case 0x6d:	zIXl = zL;	z80_ICount -= (9-4);	break; //DD: ld_ixl_l

		case 0x6e: //DD: ld_l_xix
			zL = RM(IndirectIX() );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x6f:	zIXl = zA;	z80_ICount -= (9-4);	break; //DD: ld_ixl_a

		case 0x70: //DD: ld_xix_b
			WM(IndirectIX(), zB );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x71: //DD: ld_xix_c
			WM(IndirectIX(), zC );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x72: //DD: ld_xix_d
			WM(IndirectIX(), zD );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x73: //DD: ld_xix_e
			WM(IndirectIX(), zE );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x74: //DD: ld_xix_h
			WM(IndirectIX(), zH );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x75: //DD: ld_xix_l
			WM(IndirectIX(), zL );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

		case 0x76: //DD: ld_xix_xix (nop but, eat 19 cycles???)
			z80_ICount -= (19-4);
			break;

		case 0x77: //DD: ld_xix_a
			WM(IndirectIX(), zA );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x78:INST2NO_BRK
//		case 0x79:INST2NO_BRK
//		case 0x7a:INST2NO_BRK
//		case 0x7b:INST2NO_BRK

		case 0x7c:	zA = zIXh;	z80_ICount -= (9-4);	break; //DD: ld_a_ixh
		case 0x7d:	zA = zIXl;	z80_ICount -= (9-4);	break; //DD: ld_a_ixl

		case 0x7e: //DD: ld_a_xix
			zA = RM(IndirectIX() );
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x7f:INST2NO_BRK
//		case 0x80:INST2NO_BRK
//		case 0x81:INST2NO_BRK
//		case 0x82:INST2NO_BRK
//		case 0x83:INST2NO_BRK

		case 0x84: //DD: add_a_ixh//???適当？？
	//	unsigned char bTemp2;
	//			bTemp2 = zA + zIXh;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zIXh];
				zA += zIXh;/*(+)*/
			z80_ICount -= (9-4);
			break;

		case 0x85: //DD: add_a_ixl//???適当？？
	//	unsigned char bTemp2;
	//			bTemp2 = zA + zIXl;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zIXl];
				zA += zIXl;/*(+)*/
			z80_ICount -= (9-4);
			break;

		case 0x86: //DD: add_a_xix
			{unsigned char bTemp1;
				bTemp1 = RM(IndirectIX() );
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | bTemp1];
				zA += bTemp1;
			}
				ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x87:INST2NO_BRK
//		case 0x88:INST2NO_BRK
//		case 0x89:INST2NO_BRK
//		case 0x8a:INST2NO_BRK
//		case 0x8b:INST2NO_BRK
		case 0x8c: //DD: adc_a_ixh//???適当？？
		{
	//	unsigned char bTemp2;
		unsigned char bTemp2;
	//			bTemp2 = zA + zIXh + (zF & CF);
				bTemp2 =			zIXh + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zIXh | (((UINT32) zF & CF) << 16)];
				zA += bTemp2;/*(+)*/
		}
			z80_ICount -= (9-4);
			break;

		case 0x8d: //DD: adc_a_ixl//???適当？？
		{
	//	unsigned char bTemp2;
		unsigned char bTemp2;
	//			bTemp2 = zA + zIXl + (zF & CF);
				bTemp2 =			zIXl + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zIXl | (((UINT32) zF & CF) << 16)];
				zA += bTemp2;/*(+)*/
		}
			z80_ICount -= (9-4);
			break;

		case 0x8e: //DD: adc_a_xix
		{unsigned char bTemp1;
		unsigned char bTemp2;
				bTemp1 = RM(IndirectIX() );
				bTemp2 = (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | bTemp1 | (((UINT32) zF & CF) << 16)];
				zA += bTemp1 + bTemp2;
		}
				ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x8f:INST2NO_BRK
//		case 0x90:INST2NO_BRK
//		case 0x91:INST2NO_BRK
//		case 0x92:INST2NO_BRK
//		case 0x93:INST2NO_BRK

		case 0x94: //DD: sub_ixh//???適当？？
	//	unsigned char bTemp2;
	//			bTemp2 = zA - zIXh;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zIXh];
				zA -= zIXh;/*(+)*/
			z80_ICount -= (9-4);
		break;

		case 0x95: //DD: sub_ixl//???適当？？
	//	unsigned char bTemp2;
	//			bTemp2 = zA - zIXl;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zIXl];
				zA -= zIXl;/*(+)*/
			z80_ICount -= (9-4);
		break;

		case 0x96: //DD: sub_xix
			{unsigned char bTemp1;
				bTemp1 = RM(IndirectIX()  );
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | bTemp1];
				zA -= bTemp1;
				}
			ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;

//		case 0x97:INST2NO_BRK
//		case 0x98:INST2NO_BRK
//		case 0x99:INST2NO_BRK
//		case 0x9a:INST2NO_BRK
//		case 0x9b:INST2NO_BRK

		case 0x9c: //DD: sbc_a_ixh//???適当？？
		{
	//	unsigned char bTemp2;
		unsigned char bTemp2;
	//			bTemp2 = zA - zIXh - (zF & CF);
				bTemp2 =			zIXh + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zIXh | (((UINT32) zF & CF) << 16)];
				zA -= bTemp2;/*(+)*/
		}
			z80_ICount -= (9-4);
			break;

		case 0x9d: //DD: sbc_a_ixl//???適当？？
		{
	//	unsigned char bTemp2;
		unsigned char bTemp2;
	//			bTemp2 = zA - zIXl - (zF & CF);
				bTemp2 =			zIXl + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zIXl | (((UINT32) zF & CF) << 16)];
				zA -= bTemp2;/*(+)*/
		}
			z80_ICount -= (9-4);
			break;

		case 0x9e: //DD: sbc_a_xix
		{unsigned char bTemp1;
		unsigned char bTemp2;
				bTemp1 = RM(IndirectIX()  );
				bTemp2 = zA;
				zA = zA - bTemp1 - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) bTemp2 << 8) | bTemp1 | (((UINT32) zF & CF) << 16)];
		}
				ADJ_FUZZY_PC;//
			z80_ICount -= (19-4);
			break;
//		case 0x9f:INST2NO_BRK //DD:
//		case 0xa0:INST2NO_BRK //DD:
//		case 0xa1:INST2NO_BRK //DD:
//		case 0xa2:INST2NO_BRK //DD:
//		case 0xa3:INST2NO_BRK //DD:
		case 0xa4: //DD: and_ixh//???適当？？
				zA &= zIXh;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
			z80_ICount -= (9-4);
			break;

		case 0xa5: //DD: and_ixl//???適当？？
				zA &= zIXl;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
			z80_ICount -= (9-4);
			break;

		case 0xa6: //DD: and_xix
				zA &= RM(IndirectIX());
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				ADJ_FUZZY_PC;
			z80_ICount -= (19-4);
			break;

//		case 0xa7:INST2NO_BRK //DD:
//		case 0xa8:INST2NO_BRK //DD:
//		case 0xa9:INST2NO_BRK //DD:
//		case 0xaa:INST2NO_BRK //DD:
//		case 0xab:INST2NO_BRK //DD:
		case 0xac: //DD: xor_ixh//???適当？？
				zA ^= zIXh;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
			z80_ICount -= (9-4);
			break;

		case 0xad: //DD: xor_ixl//???適当？？
				zA ^= zIXl;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
			z80_ICount -= (9-4);
			break;

		case 0xae: //DD: xor_xix
				zA ^= RM(IndirectIX());
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				ADJ_FUZZY_PC;
			z80_ICount -= (19-4);
			break;

//		case 0xaf:INST2NO_BRK //DD:
//		case 0xb0:INST2NO_BRK //DD:
//		case 0xb1:INST2NO_BRK //DD:
//		case 0xb2:INST2NO_BRK //DD:
//		case 0xb3:INST2NO_BRK //DD:
		case 0xb4: //DD: or_ixh//???適当？？
				zA |= zIXh;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
			z80_ICount -= (9-4);
			break;

		case 0xb5: //DD: or_ixl//???適当？？
				zA |= zIXl;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
			z80_ICount -= (9-4);
			break;

		case 0xb6: //DD: or_xix
				zA |= RM(IndirectIX());
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				ADJ_FUZZY_PC;
			z80_ICount -= (19-4);
			break;

//		case 0xb7:INST2NO_BRK //DD:
//		case 0xb8:INST2NO_BRK //DD:
//		case 0xb9:INST2NO_BRK //DD:
//		case 0xba:INST2NO_BRK //DD:
//		case 0xbb:INST2NO_BRK //DD:
		case 0xbc: //DD: cp_ixh//???適当？？
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
			z80_ICount -= (9-4);
			break;

		case 0xbd: //DD: cp_ixl//???適当？？
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
			z80_ICount -= (9-4);
			break;

		case 0xbe: //DD: cp_xix
				{unsigned char bTemp1;
				bTemp1 = RM(IndirectIX()  );
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | bTemp1];
				}
				ADJ_FUZZY_PC;
			z80_ICount -= (19-4);
			break;

//		case 0xbf:INST2NO_BRK //DD:
//		case 0xc0:INST2NO_BRK //DD:
//		case 0xc1:INST2NO_BRK //DD:
//		case 0xc2:INST2NO_BRK //DD:
//		case 0xc3:INST2NO_BRK //DD:
//		case 0xc4:INST2NO_BRK //DD:
//		case 0xc5:INST2NO_BRK //DD:
//		case 0xc6:INST2NO_BRK //DD:
//		case 0xc7:INST2NO_BRK //DD:
//		case 0xc8:INST2NO_BRK //DD:
//		case 0xc9:INST2NO_BRK //DD:
//		case 0xca:INST2NO_BRK //DD:
		case 0xcb: //DD: dd_cb

				HandleIXIY();
			//	HandleDDCB();	//IX
			//	HandleFDCB();	//IY
			break;

//		case 0xcc:INST2NO_BRK //DD:
//		case 0xcd:INST2NO_BRK //DD:
//		case 0xce:INST2NO_BRK //DD:
//		case 0xcf:INST2NO_BRK //DD:
//		case 0xd0:INST2NO_BRK //DD:
//		case 0xd1:INST2NO_BRK
//		case 0xd2:INST2NO_BRK
//		case 0xd3:INST2NO_BRK
//		case 0xd4:INST2NO_BRK
//		case 0xd5:INST2NO_BRK
//		case 0xd6:INST2NO_BRK
//		case 0xd7:INST2NO_BRK
//		case 0xd8:INST2NO_BRK
//		case 0xd9:INST2NO_BRK
//		case 0xda:INST2NO_BRK
//		case 0xdb:INST2NO_BRK
//		case 0xdc:INST2NO_BRK
//		case 0xdd:INST2NO_BRK
//		case 0xde:INST2NO_BRK
//		case 0xdf:INST2NO_BRK
//		case 0xe0:INST2NO_BRK
		case 0xe1: //DD: pop_ix
			//	ADJ_FUZZY_PC;
			//	zSP &= 0x00ffff; /* my_masked */
				zIX = RM(zSP );
				zIX |= ((UINT16)RM(zSP+1 ) << 8);
				zSP += 2;
			z80_ICount -= (14-4);
			break;

//		case 0xe2:INST2NO_BRK
//		case 0xe2: //FD: nop(eat ????)
//			z80_ICount -= (9-4); //適当でも少しかかる筈
//			break;

		case 0xe3: //DD: ex_xsp_ix
			//	ADJ_FUZZY_PC;
			//	zSP &= 0x00ffff; /* my_masked */
				dwAddr = RM(zSP );
				dwAddr |= ((UINT16)RM(zSP+1 ) << 8);

				WM(zSP	, (zIX & 0xff) );
				WM(zSP+1, (zIX >> 8) );
				zIX = dwAddr;
			z80_ICount -= (23-4);
			break;

//		case 0xe4:INST2NO_BRK
		case 0xe5: //DD: push_ix
				zSP -= 2;
			//	zSP &= 0x00ffff; /* my_masked */
			//	ADJ_FUZZY_PC;
				WM(zSP	, (zIX & 0xff) );
				WM(zSP+1, (zIX >> 8) );
			z80_ICount -= (15-4);
			break;

//		case 0xe6:INST2NO_BRK
//		case 0xe7:INST2NO_BRK
//		case 0xe8:INST2NO_BRK
		case 0xe9: //DD: jp_ix
				pbPC = zBase + zIX;
				ADJ_FUZZY_PC;/*?*/
			z80_ICount -= (8-4);
			break;

//		case 0xea:INST2NO_BRK
//		case 0xeb:INST2NO_BRK
//		case 0xec:INST2NO_BRK
//		case 0xed:INST2NO_BRK
//		case 0xee:INST2NO_BRK
//		case 0xef:INST2NO_BRK
//		case 0xf0:INST2NO_BRK
//		case 0xf1:INST2NO_BRK
//		case 0xf2:INST2NO_BRK
//		case 0xf3:INST2NO_BRK
//		case 0xf4:INST2NO_BRK
//		case 0xf5:INST2NO_BRK
//		case 0xf6:INST2NO_BRK
//		case 0xf7:INST2NO_BRK
//		case 0xf8:INST2NO_BRK
		case 0xf9: //DD: ld_sp_ix
				zSP = zIX;
			z80_ICount -= (10-4);
			break;

//		case 0xfa:INST2NO_BRK
//		case 0xfb:INST2NO_BRK
//		case 0xfc:INST2NO_BRK
//		case 0xfd:INST2NO_BRK
//		case 0xfe:INST2NO_BRK
//		case 0xff:INST2NO_BRK

		default:
			z80_ICount -= (4+4-4); //これで正しい筈('DD:'prefixで４クロック+NOPで４クロック)
		//pbPC-=2;NO_INST();// 死ぬの禁止
			break;

	}
}
///////////////////////


static void Rst(unsigned char vvv) {
	Z80CALL;
	pbPC = zBase + vvv; /* Normalize the address */
	ADJ_FUZZY_PC;
}

static void Ret0(unsigned int fff){
	if(fff)
	{
		Ret0_TRUE;
	}
}
/* Main execution entry point */
// /*UINT32*/unssigned short
//void z80_execute(UINT32 dwCycles)
//#include "8080tbl.c"



static void OP80_00(void){ // nop
	/* Intentionally not doing anything - NOP! */
	//z80_ICount -= 4;
}
static void OP80_08(void){ // ex_af_af //「ＡＦ」裏レジスタと交換
	{
	unsigned short iii;
		iii = zAF;	zAF = zAF2; zAF2 = iii;
	}
	//z80_ICount -= 4;
}
static void OP80_10(void){ // djnz
	{int sdwAddr;
	sdwAddr = (INT8) *pbPC; /* Get LSB first */
	pbPC++;
	zB--;
	if(zB)
	{
		pbPC +=sdwAddr;
		z80_ICount -= 5;	//+5	//dwElapsedTicks += 5;	/* 5 More for jump taken */
	}}
	z80_ICount -= (8-4);//5+3
	ADJ_FUZZY_PC;
}
static void OP80_18(void){ // jr
	{int sdwAddr;
	sdwAddr = (INT8) *pbPC; /* Get LSB first */
	pbPC++;
	pbPC +=sdwAddr;
	}
	z80_ICount -= (12-4);// 4+3+5
	ADJ_FUZZY_PC;/*?*/
}
static void OP80_20(void){ // jr_nz
	{int sdwAddr;
	sdwAddr = (INT8) *pbPC++;	/* Get LSB first */
	if(!(zF & ZF))
	{
		pbPC += sdwAddr;
		z80_ICount -= 5;//+5
	}}
	z80_ICount -= (7-4);//4+3
	ADJ_FUZZY_PC;/*?*/
}
static void OP80_28(void){ // jr_z
	{int sdwAddr;
	sdwAddr = (INT8) *pbPC++;	/* Get LSB first */
	if(zF & ZF)
	{
		pbPC += sdwAddr;
		z80_ICount -= 5;
	}}
	z80_ICount -= (7-4);
	ADJ_FUZZY_PC;/*?*/
}
static void OP80_30(void){ // jr_nc
	{int sdwAddr;
	sdwAddr = (INT8) *pbPC++;	/* Get LSB first */
	if(!(zF & CF))
	{
		pbPC += sdwAddr;
		z80_ICount -= 5;
	}}
	z80_ICount -= (7-4);
	ADJ_FUZZY_PC;/*?*/
}
static void OP80_38(void){ // jr_c
	{int sdwAddr;
	sdwAddr = (INT8) *pbPC++;	/* Get LSB first */
	if(zF & CF)
	{
		pbPC += sdwAddr;
		z80_ICount -= 5;
	}}
	z80_ICount -= (7-4);
	ADJ_FUZZY_PC;/*?*/
}


static void OP80_01(void){ // ld_bc_word
	zBC = ImmedWord();
	pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}
static void OP80_11(void){ // ld_de_word
	zDE = ImmedWord();
	pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}
static void OP80_21(void){ // ld_hl_word
	zHL = ImmedWord();
	pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}
static void OP80_31(void){ // ld_sp_word
	zSP = ImmedWord();
	pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}


static void OP80_02(void){ // ld_xbc_a
	WM(zBC, zA );
	z80_ICount -= (7-4);
}

static void OP80_03(void){ // inc_bc
	zBC++;
	zBC &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_04(void){ // inc_b
	zF_INC(zB);
	//z80_ICount -= 4;
}

static void OP80_05(void){ // dec_b
	zF_DEC(zB);
	//z80_ICount -= 4;
}

static void OP80_06(void){ // ld_b_byte
	zB = *pbPC++; /* Get immediate byte into register */
	ADJ_FUZZY_PC;
	z80_ICount -= (7-4);
}

static void OP80_07(void){ // RLCA
	zA = (zA << 1) | (zA >> 7);
	zF = (zF & (SF | ZF | PF)) | (zA & (YF | XF | CF));
	//z80_ICount -= 4;
}





static void OP80_09(void){ // add_hl_bc
	zF &= ~(CF | NF | HF);
	{unsigned long dwTemp5;
	dwTemp5 = zHL + zBC;
	zF |= ((dwTemp5 >> 16) & CF) | (((zHL ^ dwTemp5 ^ zBC) >> 8) & HF);
	zHL = dwTemp5 & 0xffff;
	}
	z80_ICount -= (11-4);
}
static void OP80_19(void){ // add_hl_de
	zF &= ~(CF | NF | HF);
	{unsigned long dwTemp5;
	dwTemp5 = zHL + zDE;
	zF |= ((dwTemp5 >> 16) & CF) | (((zHL ^ dwTemp5 ^ zDE) >> 8) & HF);
	zHL = dwTemp5 & 0xffff;
	}
	z80_ICount -= (11-4);
}
static void OP80_29(void){ // add_hl_hl
	zF &= ~(CF | NF | HF);
	{unsigned long dwTemp5;
	dwTemp5 = zHL + zHL;
	zF |= ((dwTemp5 >> 16) & CF) | (((zHL ^ dwTemp5 ^ zHL) >> 8) & HF);
	zHL = dwTemp5 & 0xffff;
	}
	z80_ICount -= (11-4);
}
static void OP80_39(void){ // add_hl_sp
	zF &= ~(CF | NF | HF);
	{unsigned long dwTemp5;
	dwTemp5 = zHL + zSP;
	zF |= ((dwTemp5 >> 16) & CF) | (((zHL ^ dwTemp5 ^ zSP) >> 8) & HF);
	zHL = dwTemp5 & 0xffff;
	}
	z80_ICount -= (11-4);
}


static void OP80_0a(void){ // ld_a_xbc
	zA = RM(zBC );
	z80_ICount -= (7-4);
}

static void OP80_0b(void){ // dec_bc
	zBC--;	zBC &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_0c(void){ // inc_c
	zF_INC(zC);
	//z80_ICount -= 4;
}

static void OP80_0d(void){ // dec_c
	zF_DEC(zC);
	//z80_ICount -= 4;
}

static void OP80_0e(void){ // ld_c_byte
	zC = *pbPC++; /* Get immediate byte into register */
	ADJ_FUZZY_PC;
	z80_ICount -= (7-4);
}

static void OP80_0f(void) // RRCA
{
	zF = (zF & (SF | ZF | PF)) | (zA & CF);
	zA = (zA >> 1) | (zA << 7);
	zF |= (zA & (YF | XF) );
	//z80_ICount -= 4;
}





static void OP80_12(void){ // ld_xde_a
	WM(zDE, zA );
	z80_ICount -= (7-4);
}

static void OP80_13(void){ // inc_de
	zDE++;	zDE &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_14(void){ // inc_d
	zF_INC(zD);
	//z80_ICount -= 4;
}

static void OP80_15(void){ // dec_d
	zF_DEC(zD);
	//z80_ICount -= 4;
}

static void OP80_16(void){ // ld_d_byte
	zD = *pbPC++; /* Get immediate byte into register */
	ADJ_FUZZY_PC;/*?*/
	z80_ICount -= (7-4);
}

static void OP80_17(void){ // rla
	{unsigned char bTemp1;
	bTemp1 = zA >> 7;
	zA = (zA << 1) | (zF & CF);
	zF = (zF & ~(CF | NF | HF)) | bTemp1;
	}
	//z80_ICount -= 4;
}





static void OP80_1a(void){ // ld_a_xde
	zA = RM(zDE );
	z80_ICount -= (7-4);
}

static void OP80_1b(void){ // dec_de
	zDE--;
	zDE &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_1c(void){ // inc_e
	zF_INC(zE);
	//z80_ICount -= 4;
}

static void OP80_1d(void){ // dec_e
	zF_DEC(zE);
	//z80_ICount -= 4;
}

static void OP80_1e(void){ // ld_e_byte
	zE = *pbPC++; /* Get immediate byte into register */
	ADJ_FUZZY_PC;/*?*/
	z80_ICount -= (7-4);
}

static void OP80_1f(void){ // rra
	{unsigned char bTemp1;
	bTemp1 = (zF & CF) << 7;
	zF = (zF & ~(CF | NF | HF)) | (zA & CF);
	zA = ((zA >> 1) | bTemp1);
	}
	//z80_ICount -= 4;
}





static void OP80_22(void){ // ld_xword_hl
	{unsigned short dwTemp4;
	dwTemp4=ImmedWord();
	WM(dwTemp4	, (zHL & 0xff) );
	WM(dwTemp4+1, (zHL >> 8) );
	}pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (16-4);
}

static void OP80_23(void){ // inc_hl
	zHL++;
	zHL &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_24(void){ // inc_h
	zF_INC(zH);
	//z80_ICount -= 4;
}

static void OP80_25(void){ // dec_h
	zF_DEC(zH);
	//z80_ICount -= 4;
}

static void OP80_26(void){ // ld_h_byte
	zH = *pbPC++; /* Get immediate byte into register */
	ADJ_FUZZY_PC;/*?*/
	z80_ICount -= (7-4);
}

static void OP80_27(void){ // DAA
#if 00
dwAddr = (zA)|
((		((zF & HF) >> (4-1) )/*2?*/
	 |	((zF & NF) << 1)/*4?*/
	 |	(zF & CF)/*1?*/
	) << 8);

	zF &= ~(CF | VF | HF | ZF | SF);
	zF |= b2DAA_Table[dwAddr][0];/*addr culcutation by system(high word)*/
	zA	= b2DAA_Table[dwAddr][1];/*addr culcutation by system(low word)*/
#endif

#ifdef LARGE_DAA_TABLE
	unsigned short aaa = zA;
	if( zF & CF ) aaa |= 0x100;
	if( zF & HF ) aaa |= 0x200;
	if( zF & NF ) aaa |= 0x400;
	zAF = DAA_Table[aaa];
#else
	unsigned char cf, nf, hf, lo, hi, diff;
	cf = zF & CF;
	nf = zF & NF;
	hf = zF & HF;
	lo = (zA & 0x0f);	/* mod 15 */
	hi = (zA>>4);		/* div 16 */
	if(cf)	{	diff = (lo <= 9 && !hf) ? 0x60 : 0x66;	}
	else
	{
		if(lo >= 10){	diff = (hi <= 8) ? 0x06 : 0x66; }
		else
		{
			if(hi >= 10){	diff = hf ? 0x66 : 0x60;	}
			else		{	diff = hf ? 0x06 : 0x00;	}
		}
	}
	if(nf) zA -= diff;
	else zA += diff;
	zF = SZP[zA] | (zF & NF);
	if(cf || (lo <= 9 ? hi >= 10 : hi >= 9)) zF |= CF;
	if(nf ? hf && lo <= 5 : lo >= 10)	zF |= HF;
#endif
	//z80_ICount -= 4;
}





static void OP80_2a(void){ // ld_hl_xword
	{unsigned short dwTemp3;
	dwTemp3=ImmedWord();
	zHL = RM(dwTemp3 );
	zHL |= ((UINT16)RM(dwTemp3+1 ) << 8);
	}pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (16-4);
}

static void OP80_2b(void){ // dec_hl
	zHL--;
	zHL &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_2c(void){ // inc_l
	zF_INC(zL);
	//z80_ICount -= 4;
}

static void OP80_2d(void){ // dec_l
	zF_DEC(zL);
	//z80_ICount -= 4;
}

static void OP80_2e(void){ // ld_l_byte
	zL = *pbPC++; /* Get immediate byte into register */
	ADJ_FUZZY_PC;/*?*/
	z80_ICount -= (7-4);
}

static void OP80_2f(void){ // cpl
	zA ^= 0xff;
	zF |= (NF | HF);
	//z80_ICount -= 4;
}





static void OP80_32(void){ // ld_xbyte_a
	WM(ImmedWord(), zA );
	pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (13-4);
}

static void OP80_33(void){ // inc_sp
	zSP++;
	zSP &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_34(void){ // inc_xhl
	{unsigned char bTemp1;
	bTemp1 = RM(zHL );
	zF_INC(bTemp1);
	WM(zHL, bTemp1 );
	}
	z80_ICount -= (11-4);
}

static void OP80_35(void){ // dec_xhl
	{unsigned char bTemp1;
	bTemp1 = RM(zHL );
	zF_DEC(bTemp1);
	WM(zHL, bTemp1 );
	}
	z80_ICount -= (11-4);
}

static void OP80_36(void){ // ld_xhl_byte
	WM(zHL, *pbPC++ );
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}

static void OP80_37(void){ // scf
	zF &= ~(HF | NF);
	zF |= CF;
	//z80_ICount -= 4;
}





static void OP80_3a(void){ // ld_a_xbyte
	zA = RM(ImmedWord());
	pbPC+=2;
	ADJ_FUZZY_PC;
	z80_ICount -= (13-4);
}

static void OP80_3b(void){ // dec_sp
	zSP--;
	zSP &= 0xffff;
	z80_ICount -= (6-4);
}

static void OP80_3c(void){	zF_INC(zA); /*z80_ICount -= 4;*/	} // inc_a
static void OP80_3d(void){	zF_DEC(zA); /*z80_ICount -= 4;*/	} // dec_a

static void OP80_3e(void){ // ld_a_byte
	zA = *pbPC++; /* Get immediate byte into register */
	ADJ_FUZZY_PC;
	z80_ICount -= (7-4);
}

static void OP80_3f(void){ // ccf
zF=((zF&(SF|ZF|PF|CF))|((zF&CF)<<4)|(zA&(YF|XF)))^CF;
//#if 1
//		//? 	bTemp1 = (zF & CF) << 4;
//				zF &= ~(HF | NF);
//				zF ^= CF;
//#else
///*???*/
//				zF =((zF & 0xED)|((zF & 1)<<4))^1;
//#endif
	//z80_ICount -= 4;
}


//////////////////////////


#define OP80_40 OP80_00/*zB = zB;*/ // ld_b_b

static void OP80_41(void){	zB = zC;	/*z80_ICount -= 4;*/}/*e*/ // ld_b_c
static void OP80_42(void){	zB = zD;	/*z80_ICount -= 4;*/}/*e*/ // ld_b_d
static void OP80_43(void){	zB = zE;	/*z80_ICount -= 4;*/}/*e*/ // ld_b_e
static void OP80_44(void){	zB = zH;	/*z80_ICount -= 4;*/}/*e*/ // ld_b_h
static void OP80_45(void){	zB = zL;	/*z80_ICount -= 4;*/}/*e*/ // ld_b_l

static void OP80_46(void){	zB = RM(zHL );	z80_ICount -= (7-4);}/*e*/ // ld_b_xhl

static void OP80_47(void){	zB = zA;	/*z80_ICount -= 4;*/}/*e*/ // ld_b_a
static void OP80_48(void){	zC = zB;	/*z80_ICount -= 4;*/}/*e*/ // ld_c_b
//tatic void OP80_49(void){ zC = zC;	/*z80_ICount -= 4;*/}/*e*/ // ld_c_c
#define OP80_49 OP80_00/*zC = zC;*/ // ld_c_c
static void OP80_4a(void){	zC = zD;	/*z80_ICount -= 4;*/}/*e*/ // ld_c_d
static void OP80_4b(void){	zC = zE;	/*z80_ICount -= 4;*/}/*e*/ // ld_c_e
static void OP80_4c(void){	zC = zH;	/*z80_ICount -= 4;*/}/*e*/ // ld_c_h
static void OP80_4d(void){	zC = zL;	/*z80_ICount -= 4;*/}/*e*/ // ld_c_l

static void OP80_4e(void){	zC = RM(zHL );	z80_ICount -= (7-4);}/*e*/ // ld_c_xhl

static void OP80_4f(void){	zC = zA;	/*z80_ICount -= 4;*/}/*e*/ // ld_c_a
static void OP80_50(void){	zD = zB;	/*z80_ICount -= 4;*/}/*e*/ // ld_d_b
static void OP80_51(void){	zD = zC;	/*z80_ICount -= 4;*/}/*e*/ // ld_d_c
//tatic void OP80_52(void){ zD = zD;	/*z80_ICount -= 4;*/}/*e*/ // ld_d_d
#define OP80_52 OP80_00/*zD = zD;*/ // ld_d_d
static void OP80_53(void){	zD = zE;	/*z80_ICount -= 4;*/}/*e*/ // ld_d_e
static void OP80_54(void){	zD = zH;	/*z80_ICount -= 4;*/}/*e*/ // ld_d_h
static void OP80_55(void){	zD = zL;	/*z80_ICount -= 4;*/}/*e*/ // ld_d_l

static void OP80_56(void){	zD = RM(zHL );	z80_ICount -= (7-4);}/*e*/ // ld_d_xhl

static void OP80_57(void){	zD = zA;	/*z80_ICount -= 4;*/}/*e*/ // ld_d_a
static void OP80_58(void){	zE = zB;	/*z80_ICount -= 4;*/}/*e*/ // ld_e_b
static void OP80_59(void){	zE = zC;	/*z80_ICount -= 4;*/}/*e*/ // ld_e_c
static void OP80_5a(void){	zE = zD;	/*z80_ICount -= 4;*/}/*e*/ // ld_e_d
//tatic void OP80_5b(void){ zE = zE;	/*z80_ICount -= 4;*/}/*e*/ // ld_e_e
#define OP80_5b OP80_00/*zE = zE;*/ // ld_e_e
static void OP80_5c(void){	zE = zH;	/*z80_ICount -= 4;*/}/*e*/ // ld_e_h
static void OP80_5d(void){	zE = zL;	/*z80_ICount -= 4;*/}/*e*/ // ld_e_l

static void OP80_5e(void){	zE = RM(zHL );	z80_ICount -= (7-4);}/*e*/ // ld_e_xhl

static void OP80_5f(void){	zE = zA;	/*z80_ICount -= 4;*/}/*e*/ // ld_e_a
static void OP80_60(void){	zH = zB;	/*z80_ICount -= 4;*/}/*e*/ // ld_h_b
static void OP80_61(void){	zH = zC;	/*z80_ICount -= 4;*/}/*e*/ // ld_h_c
static void OP80_62(void){	zH = zD;	/*z80_ICount -= 4;*/}/*e*/ // ld_h_d
static void OP80_63(void){	zH = zE;	/*z80_ICount -= 4;*/}/*e*/ // ld_h_e
//tatic void OP80_64(void){ zH = zH;	/*z80_ICount -= 4;*/}/*e*/ // ld_h_h
#define OP80_64 OP80_00/*zH = zH;*/ // ld_h_h
static void OP80_65(void){	zH = zL;	/*z80_ICount -= 4;*/}/*e*/ // ld_h_l

static void OP80_66(void){	zH = RM(zHL );	z80_ICount -= (7-4);}/*e*/ // ld_h_xhl

static void OP80_67(void){	zH = zA;	/*z80_ICount -= 4;*/}/*e*/ // ld_h_a
static void OP80_68(void){	zL = zB;	/*z80_ICount -= 4;*/}/*e*/ // ld_l_b
static void OP80_69(void){	zL = zC;	/*z80_ICount -= 4;*/}/*e*/ // ld_l_c
static void OP80_6a(void){	zL = zD;	/*z80_ICount -= 4;*/}/*e*/ // ld_l_d
static void OP80_6b(void){	zL = zE;	/*z80_ICount -= 4;*/}/*e*/ // ld_l_e
static void OP80_6c(void){	zL = zH;	/*z80_ICount -= 4;*/}/*e*/ // ld_l_h
//tatic void OP80_6d(void){ zL = zL;	/*z80_ICount -= 4;*/}/*e*/ // ld_l_l
#define OP80_6d OP80_00/*zL = zL;*/ // ld_l_l

static void OP80_6e(void){	zL = RM(zHL );	z80_ICount -= (7-4);}/*e*/ // ld_l_xhl

static void OP80_6f(void){	zL = zA;	/*z80_ICount -= 4;*/}/*e*/ // ld_l_a

static void OP80_70(void){	WM(zHL, zB );	z80_ICount -= (7-4);}/*e*/ // ld_xhl_b
static void OP80_71(void){	WM(zHL, zC );	z80_ICount -= (7-4);}/*e*/ // ld_xhl_c
static void OP80_72(void){	WM(zHL, zD );	z80_ICount -= (7-4);}/*e*/ // ld_xhl_d
static void OP80_73(void){	WM(zHL, zE );	z80_ICount -= (7-4);}/*e*/ // ld_xhl_e
static void OP80_74(void){	WM(zHL, zH );	z80_ICount -= (7-4);}/*e*/ // ld_xhl_h
static void OP80_75(void){	WM(zHL, zL );	z80_ICount -= (7-4);}/*e*/ // ld_xhl_l

static void OP80_76(void){ // halt
	//z80_ICount -= 4;
	zHALT = 1;
	//dwElapsedTicks += z80_ICount;
	z80_ICount = 0;
}/*e*/

static void OP80_77(void){	WM(zHL, zA );	z80_ICount -= (7-4);}/*e*/ // ld_xhl_a

static void OP80_78(void){	zA = zB;	/*z80_ICount -= 4;*/}/*e*/ // ld_a_b
static void OP80_79(void){	zA = zC;	/*z80_ICount -= 4;*/}/*e*/ // ld_a_c
static void OP80_7a(void){	zA = zD;	/*z80_ICount -= 4;*/}/*e*/ // ld_a_d
static void OP80_7b(void){	zA = zE;	/*z80_ICount -= 4;*/}/*e*/ // ld_a_e
static void OP80_7c(void){	zA = zH;	/*z80_ICount -= 4;*/}/*e*/ // ld_a_h
static void OP80_7d(void){	zA = zL;	/*z80_ICount -= 4;*/}/*e*/ // ld_a_l

static void OP80_7e(void){	zA = RM(zHL );	z80_ICount -= (7-4);}/*e*/ // ld_a_xhl

//tatic void OP80_7f(void){//zA = zA;	z80_ICount -= 4;}/*e*/ // ld_a_a
#define OP80_7f OP80_00/*zA = zA;*/ // ld_a_a

static void OP80_80(void){ // add_a_b
		//	unsigned char bTemp2;
		//		bTemp2 = zA + zB;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zB];
		//		zA = bTemp2;
				zA += zB;
				//z80_ICount -= 4;
}/*e*/

static void OP80_81(void){ // add_a_c
		//	unsigned char bTemp2;
		//		bTemp2 = zA + zC;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zC];
		//		zA = bTemp2;
				zA += zC;
				//z80_ICount -= 4;
}/*e*/

static void OP80_82(void){ // add_a_d
		//	unsigned char bTemp2;
		//		bTemp2 = zA + zD;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zD];
		//		zA = bTemp2;
				zA += zD;
				//z80_ICount -= 4;
}/*e*/

static void OP80_83(void){ // add_a_e
		//	unsigned char bTemp2;
		//		bTemp2 = zA + zE;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zE];
		//		zA = bTemp2;
				zA += zE;
				//z80_ICount -= 4;
}/*e*/

static void OP80_84(void){ // add_a_h
		//	unsigned char bTemp2;
		//		bTemp2 = zA + zH;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zH];
		//		zA = bTemp2;
				zA += zH;
				//z80_ICount -= 4;
}/*e*/

static void OP80_85(void){ // add_a_l
		//	unsigned char bTemp2;
		//		bTemp2 = zA + zL;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zL];
		//		zA = bTemp2;
				zA += zL;
				//z80_ICount -= 4;
}/*e*/

static void OP80_86(void){ // add_a_xhl
		//	unsigned char bTemp2;
				{unsigned char bTemp1;
				bTemp1 = RM(zHL );
			//	bTemp2 = zA + bTemp1;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | bTemp1];
			//	zA = bTemp2;
				zA += bTemp1;
				}
				z80_ICount -= (7-4);
}/*e*/

static void OP80_87(void){ // add_a_a
		//	unsigned char bTemp2;
		//		bTemp2 = zA + zA;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zA];
		//		zA = bTemp2;
				zA += zA;
				//z80_ICount -= 4;
}/*e*/

static void OP80_88(void){ // adc_a_b
			{
			unsigned char bTemp2;
			//	bTemp2 = zA + zB + (zF & CF);
				bTemp2 =			zB + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zB | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_89(void){ // adc_a_c
			{
			unsigned char bTemp2;
			//	bTemp2 = zA + zC + (zF & CF);
				bTemp2 =			zC + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zC | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_8a(void){ // adc_a_d
			{
			unsigned char bTemp2;
			//	bTemp2 = zA + zD + (zF & CF);
				bTemp2 =			zD + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zD | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_8b(void){ // adc_a_e
			{
			unsigned char bTemp2;
			//	bTemp2 = zA + zE + (zF & CF);
				bTemp2 =			zE + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zE | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_8c(void){ // adc_a_h
			{
			unsigned char bTemp2;
			//	bTemp2 = zA + zH + (zF & CF);
				bTemp2 =			zH + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zH | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_8d(void){ // adc_a_l
			{
			unsigned char bTemp2;
			//	bTemp2 = zA + zL + (zF & CF);
				bTemp2 =			zL + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zL | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_8e(void){ // adc_a_xhl
		//	unsigned char bTemp2;
				{unsigned char bTemp1;
				bTemp1 = RM(zHL );
			//	bTemp2 = zA + bTemp1 + (zF & CF);
				bTemp1 +=					(zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | bTemp1 | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += bTemp1;
				}
				z80_ICount -= (7-4);
}/*e*/
static void OP80_8f(void){ // adc_a_a
			{
			unsigned char bTemp2;
			//	bTemp2 = zA + zA + (zF & CF);
				bTemp2 =					   (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | zA | (((UINT32) zF & CF) << 16)];
			//	zA = bTemp2;
				zA += zA;
				zA += bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_90(void){ // sub_b
		//	unsigned char bTemp2;
		//		bTemp2 = zA - zB;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zB];
		//		zA = bTemp2;
				zA -= zB;
				//z80_ICount -= 4;
}/*e*/

static void OP80_91(void){ // sub_c
		//	unsigned char bTemp2;
		//		bTemp2 = zA - zC;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zC];
		//		zA = bTemp2;
				zA -= zC;
				//z80_ICount -= 4;
}/*e*/

static void OP80_92(void){ // sub_d
		//	unsigned char bTemp2;
		//		bTemp2 = zA - zD;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zD];
		//		zA = bTemp2;
				zA -= zD;
				//z80_ICount -= 4;
}/*e*/

static void OP80_93(void){ // sub_e
		//	unsigned char bTemp2;
		//		bTemp2 = zA - zE;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zE];
		//		zA = bTemp2;
				zA -= zE;
				//z80_ICount -= 4;
}/*e*/

static void OP80_94(void){ // sub_h
		//	unsigned char bTemp2;
		//		bTemp2 = zA - zH;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zH];
		//		zA = bTemp2;
				zA -= zH;
				//z80_ICount -= 4;
}/*e*/

static void OP80_95(void){ // sub_l
		//	unsigned char bTemp2;
		//		bTemp2 = zA - zL;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zL];
		//		zA = bTemp2;
				zA -= zL;
				//z80_ICount -= 4;
}/*e*/

static void OP80_96(void){ // sub_xhl
		//	unsigned char bTemp2;
				{unsigned char bTemp1;
				bTemp1 = RM(zHL );
		//		bTemp2 = zA - bTemp1;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | bTemp1];
		//		zA = bTemp2;
				zA -= bTemp1;
				}
				z80_ICount -= (7-4);
}/*e*/

static void OP80_97(void){ // sub_a //????
		//	unsigned char bTemp2;
		//		bTemp2 = zA - zA;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zA];
				zA = 0;//bTemp2; //????
				//z80_ICount -= 4;
}/*e*/

static void OP80_98(void){ // sbc_a_b
			{
			unsigned char bTemp2;
				bTemp2 = zA - zB - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zB | (((UINT32) zF & CF) << 16)];
				zA = bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_99(void){ // sbc_a_c
			{
			unsigned char bTemp2;
				bTemp2 = zA - zC - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zC | (((UINT32) zF & CF) << 16)];
				zA = bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_9a(void){ // sbc_a_d
			{
			unsigned char bTemp2;
				bTemp2 = zA - zD - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zD | (((UINT32) zF & CF) << 16)];
				zA = bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_9b(void){ // sbc_a_e
			{
			unsigned char bTemp2;
				bTemp2 = zA - zE - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zE | (((UINT32) zF & CF) << 16)];
				zA = bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_9c(void){ // sbc_a_h
			{
			unsigned char bTemp2;
				bTemp2 = zA - zH - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zH | (((UINT32) zF & CF) << 16)];
				zA = bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_9d(void){ // sbc_a_l
			{
			unsigned char bTemp2;
				bTemp2 = zA - zL - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zL | (((UINT32) zF & CF) << 16)];
				zA = bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_9e(void){ // sbc_a_xhl
			{
			unsigned char bTemp2;
				{unsigned char bTemp1;
				bTemp1 = RM(zHL );
				bTemp2 = zA - bTemp1 - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | bTemp1 | (((UINT32) zF & CF) << 16)];
				}
				zA = bTemp2;
			}
				z80_ICount -= (7-4);
}/*e*/
static void OP80_9f(void){ // sbc_a_a
			{
			unsigned char bTemp2;
				bTemp2 = zA - zA - (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zA | (((UINT32) zF & CF) << 16)];
				zA = bTemp2;
			}
				//z80_ICount -= 4;
}/*e*/
static void OP80_a0(void){ // and_b
				zA &= zB;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a1(void){ // and_c
				zA &= zC;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a2(void){ // and_d
				zA &= zD;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a3(void){ // and_e
				zA &= zE;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a4(void){ // and_h
				zA &= zH;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a5(void){ // and_l
				zA &= zL;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a6(void){ // and_xhl
				zA &= RM(zHL );
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				z80_ICount -= (7-4);
}/*e*/

static void OP80_a7(void){ // and_a
				zA &= zA;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a8(void){ // xor_b
				zA ^= zB;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_a9(void){ // xor_c
				zA ^= zC;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_aa(void){ // xor_d
				zA ^= zD;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_ab(void){ // xor_e
				zA ^= zE;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_ac(void){ // xor_h
				zA ^= zH;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_ad(void){ // xor_l
				zA ^= zL;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_ae(void){ // xor_xhl
				zA ^= RM(zHL );
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				z80_ICount -= (7-4);
}/*e*/

static void OP80_af(void){ // xor_a
			//	zA ^= zA;
				zA	= 0;	// えーん、基本の「き」だよ。これ知らないの、もぐりダヨ。
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/
				// 知らないのなら、一度もＺ８０のプログラム組んだことないだろ。

static void OP80_b0(void){ // or_b
				zA |= zB;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b1(void){ // or_c
				zA |= zC;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b2(void){ // or_d
				zA |= zD;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b3(void){ // or_e
				zA |= zE;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b4(void){ // or_h
				zA |= zH;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b5(void){ // or_l
				zA |= zL;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b6(void){ // or_xhl
				zA |= RM(zHL );
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				z80_ICount -= (7-4);
}/*e*/

static void OP80_b7(void){ // or_a
				zA |= zA;
				zF &= ~(CF | NF | VF | HF | ZF | SF);
				zF |= SZP[zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b8(void){ // cp_b
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zB];
				//z80_ICount -= 4;
}/*e*/

static void OP80_b9(void){ // cp_c
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zC];
				//z80_ICount -= 4;
}/*e*/

static void OP80_ba(void){ // cp_d
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zD];
				//z80_ICount -= 4;
}/*e*/

static void OP80_bb(void){ // cp_e
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zE];
				//z80_ICount -= 4;
}/*e*/

static void OP80_bc(void){ // cp_h
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zH];
				//z80_ICount -= 4;
}/*e*/

static void OP80_bd(void){ // cp_l
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zL];
				//z80_ICount -= 4;
}/*e*/

static void OP80_be(void){ // cp_xhl
				{unsigned char bTemp1;
				bTemp1 = RM(zHL );
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | bTemp1];
				}
				z80_ICount -= (7-4);
}/*e*/

static void OP80_bf(void){ // cp_a
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | zA];
				//z80_ICount -= 4;
}/*e*/

static void OP80_c0(void){ // ret_nz
				if(!(zF & ZF))
				{
					Z80RETURN
					z80_ICount -= 6;	//dwElapsedTicks += 6;
				}
				z80_ICount -= (5-4);
}/*e*/

static void OP80_c1(void){ // pop_bc
			//	zSP &= 0x00ffff; /* my_masked */
			//	ADJ_FUZZY_PC;
				zBC = RM(zSP );
				zBC |= ((UINT16)RM(zSP+1 ) << 8);
				zSP += 2;
				z80_ICount -= (10-4);
}/*e*/

#if 00
static void OP80_c2(void){ // jp_nz
				{unsigned short dwTemp4;
				dwTemp4=ImmedWord();
				pbPC+=2;
				if(!(zF & ZF))
				{
					pbPC = zBase + dwTemp4; /* Normalize the address */
				}
				}
				ADJ_FUZZY_PC;/*?*/
				z80_ICount -= (10-4);
}/*e*/
#endif

#if 00
#define IMMW \
				dwAddr = *pbPC++;	/* Get LSB first */\
				dwAddr |= ( *pbPC++ << 8); /* no cast(UINT32) */ /* Get MSB last */
#else
#define IMMW \
		dwAddr=ImmedWord();\
		pbPC+=2;
#endif
#define JMMW dwAddr=ImmedWord();

static void OP80_c2(void){ // jp_nz
				if(!(zF & ZF))
				{
					JMMW
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}else{pbPC+=2;}
				ADJ_FUZZY_PC;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_c3(void){ // jp
				JMMW
				pbPC = zBase + dwAddr;	/* Normalize the address */
				ADJ_FUZZY_PC;/*?*/
				z80_ICount -= (10-4);
}/*e*/

static void OP80_c4(void){ // call_nz
				IMMW
				if(!(zF & ZF))
				{
					Z80CALL;
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}
				ADJ_FUZZY_PC;/*?*/
				z80_ICount -= (10-4);
}/*e*/

static void OP80_c5(void){ // push_bc
				zSP -= 2;
			//	zSP &= 0x00ffff; /* my_masked */
			//	ADJ_FUZZY_PC;
				WM(zSP	, (zBC & 0xff) );
				WM(zSP+1, (zBC >> 8) );
				z80_ICount -= (11-4);
}/*e*/

static void OP80_c6(void){ // add_a_byte
				{unsigned char bTemp1;
				bTemp1 = *pbPC++;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | bTemp1];
				zA += bTemp1;
				}
				ADJ_FUZZY_PC;
				z80_ICount -= (7-4);
}/*e*/

static void OP80_c7(void){ // rst_00
				Rst(0x00);
				z80_ICount -= (11-4);
}/*e*/

static void OP80_c8(void){ // ret_z
				Ret0(zF & ZF);
				z80_ICount -= (5-4);
}/*e*/

static void OP80_c9(void){ // ret
				Z80RETURN
				z80_ICount -= (4-4+6);
			//	z80_ICount -= 4;
			//	Ret0(1);
}/*e*/

static void OP80_ca(void){ // jp_z
				if(zF & ZF)
				{
					JMMW
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}else{pbPC+=2;}
				ADJ_FUZZY_PC;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_cb(void){ // cb
				HandleCB();/*?????????*/
}/*e*/

static void OP80_cc(void){ // call_z
				IMMW
				if(zF & ZF)
				{
					Z80CALL;
					pbPC = zBase + dwAddr;	/* Normalize the address */
					ADJ_FUZZY_PC;/*?*/
				}
				z80_ICount -= (10-4);
}/*e*/

static void OP80_cd(void){ // call
				IMMW
				Z80CALL;
				pbPC = zBase + dwAddr;	/* Normalize the address */
				ADJ_FUZZY_PC;/*?*/
				z80_ICount -= (17-4);
}/*e*/

static void OP80_ce(void){ // adc_a_byte
				{unsigned char bTemp1;
				bTemp1 = (*pbPC++) + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbAddAdcTable[((UINT32) zA << 8) | bTemp1 | (((UINT32) zF & CF) << 16)];
				zA += bTemp1;
				}
				ADJ_FUZZY_PC;
				z80_ICount -= (7-4);
}/*e*/

static void OP80_cf(void){ // rst_08
				Rst(0x08);
				z80_ICount -= (11-4);
}/*e*/

static void OP80_d0(void){ // ret_nc
				if(!(zF & CF))
				{
					Z80RETURN
					z80_ICount -= 6;	//dwElapsedTicks += 6;
				}
					z80_ICount -= (5-4);
}/*e*/

static void OP80_d1(void){ // pop_de
			//	zSP &= 0x00ffff; /* my_masked */
			//	ADJ_FUZZY_PC;
				zDE = RM(zSP );
				zDE |= ((UINT16)RM(zSP+1 ) << 8);
				zSP += 2;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_d2(void){ // jp_nc
				if(!(zF & CF))
				{
					JMMW
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}else{pbPC+=2;}
				ADJ_FUZZY_PC;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_d3(void){ // out_byte_a
#if 1
				pbPC++;
#else
				dwTemp = *pbPC++;
				psIoWrite = zIoWrite; /* Beginning of our handler */
				{
					ADJ_FUZZY_PC;
					psIoWrite->IOCall(dwTemp, zA, psIoWrite);
				}
#endif
				ADJ_FUZZY_PC;
				z80_ICount -= (11-4);
}/*e*/

static void OP80_d4(void){ // call_nc
				IMMW
				if(!(zF & CF))
				{
					Z80CALL;
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}
				ADJ_FUZZY_PC;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_d5(void){ // push_de
				zSP -= 2;
			//	zSP &= 0x00ffff; /* my_masked */
			//	ADJ_FUZZY_PC;
				WM(zSP, (zDE & 0xff) );
				WM(zSP+1, (zDE >> 8) );
				z80_ICount -= (11-4);
}/*e*/

static void OP80_d6(void){ // sub_byte
				{unsigned char bTemp1;
				bTemp1 = *pbPC++;
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | bTemp1];
				zA -= bTemp1;
				}
				ADJ_FUZZY_PC;
				z80_ICount -= (7-4);
}/*e*/

static void OP80_d7(void){ // rst_10
				Rst(0x10);
				z80_ICount -= (11-4);
}/*e*/

static void OP80_d8(void){ // ret_c
				Ret0(zF & CF);
				z80_ICount -= (5-4);
}/*e*/

static void OP80_d9(void){ // exx
			{
			unsigned short iii;
				iii = zBC;	zBC = zBC2; zBC2 = iii;
				iii = zDE;	zDE = zDE2; zDE2 = iii;
				iii = zHL;	zHL = zHL2; zHL2 = iii;
			}
				//z80_ICount -= 4;
}/*e*/

static void OP80_da(void){ // jp_c
				if(zF & CF)
				{
					JMMW
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}else{pbPC+=2;}
				ADJ_FUZZY_PC;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_db(void){ // in_a_byte
//				dwTemp = *pbPC++;
//				psIoRead = zIoRead; /* Beginning of our handler */
//				{
//					ADJ_FUZZY_PC;
//					zA = psIoRead->IOCall(dwTemp, psIoRead);
//				}
				zA = 0xff; /* [force] Unclaimed I/O read */
				z80_ICount -= (11-4);
}/*e*/

static void OP80_dc(void){ // call_c
				IMMW
				if(zF & CF)
				{
					Z80CALL;
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}
					ADJ_FUZZY_PC;/*?*/
				z80_ICount -= (10-4);
}/*e*/


static void OP80_dd(void){ // dd
	HandleDD();/*???????????*/
}


static void OP80_de(void){ // sbc_a_byte
				{unsigned char bTemp1;
				bTemp1 = (*pbPC++) + (zF & CF);
				zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
								pbSubSbcTable[((UINT32) zA << 8) | bTemp1 | (((UINT32) zF & CF) << 16)];
				zA = zA - bTemp1;
				}
				ADJ_FUZZY_PC;
				z80_ICount -= (7-4);
}/*e*/

static void OP80_df(void){ // rst_18
				Rst(0x18);
				z80_ICount -= (11-4);
}/*e*/

static void OP80_e0(void){ // ret_po
				if(!(zF & VF))
				{
					Z80RETURN
					z80_ICount -= 6;	//dwElapsedTicks += 6;
				}
					z80_ICount -= (5-4);
}/*e*/

static void OP80_e1(void){ // pop_hl
			//	zSP &= 0x00ffff; /* my_masked */
			//	ADJ_FUZZY_PC;
				zHL = RM(zSP );
				zHL |= ((UINT16)RM(zSP+1 ) << 8);
				zSP += 2;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_e2(void){ // jp_po
				if(!(zF & VF))
				{
					JMMW
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}else{pbPC+=2;}
				ADJ_FUZZY_PC;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_e3(void){ // ex_xsp_hl
			//	zSP &= 0x00ffff; /* my_masked */
			//	ADJ_FUZZY_PC;
				dwAddr = RM(zSP );
				dwAddr |= ((UINT16)RM(zSP+1 ) << 8);

				WM(zSP, (zHL & 0xff) );
				WM(zSP+1, (zHL >> 8) );
				zHL = dwAddr;
				z80_ICount -= (19-4);
}/*e*/

static void OP80_e4(void){ // call_po
				IMMW
				if(!(zF & VF))
				{
					Z80CALL;
					pbPC = zBase + dwAddr;	/* Normalize the address */
				}
				ADJ_FUZZY_PC;
				z80_ICount -= (10-4);
}/*e*/

static void OP80_e5(void){ // push_hl
	zSP -= 2;
//	zSP &= 0x00ffff; /* my_masked */
	ADJ_FUZZY_PC;
	WM(zSP, (zHL & 0xff) );
	WM(zSP+1, (zHL >> 8) );
	z80_ICount -= (11-4);
}/*e*/

static void OP80_e6(void){ // and_byte
	zA &= *pbPC++;
	zF &= ~(CF | NF | VF | HF | ZF | SF);
	zF |= (SZP[zA] | HF);//bPostANDFlags[zA];
	ADJ_FUZZY_PC;
	z80_ICount -= (7-4);
}/*e*/

static void OP80_e7(void){ // rst_20
	Rst(0x20);
	z80_ICount -= (11-4);
}/*e*/

static void OP80_e8(void){ // ret_pe
	Ret0(zF & VF);
	z80_ICount -= (5-4);
}/*e*/

static void OP80_e9(void){ // jp_hl
	pbPC = zBase + zHL;
	ADJ_FUZZY_PC;/*?*/
	//z80_ICount -= 4;
}/*e*/

static void OP80_ea(void){ // jp_pe
	if(zF & VF)
	{
		JMMW
		pbPC = zBase + dwAddr;	/* Normalize the address */
	}else{pbPC+=2;}
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}/*e*/

static void OP80_eb(void){ // ex_de_hl
	unsigned short iii;
	iii = zDE;	zDE = zHL;	zHL = iii;
	//z80_ICount -= 4;
}/*e*/

static void OP80_ec(void){ // call_pe
	IMMW
	if(zF & VF)
	{
				Z80CALL;
		pbPC = zBase + dwAddr;	/* Normalize the address */
	}
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}/*e*/

static void OP80_ed(void){ // ed
	HandleED();/*????????*/
}/*e*/

static void OP80_ee(void){ // xor_byte
	zA ^= *pbPC++;
	zF &= ~(CF | NF | VF | HF | ZF | SF);
	zF |= SZP[zA];
	ADJ_FUZZY_PC;
	z80_ICount -= (7-4);
}/*e*/

static void OP80_ef(void){ // rst_28
	Rst(0x28);
	z80_ICount -= (11-4);
}/*e*/

static void OP80_f0(void){ // ret_p
	if(!(zF & SF))
	{
					Z80RETURN
		z80_ICount -= 6;	//dwElapsedTicks += 6;
	}
		z80_ICount -= (5-4);
}/*e*/

static void OP80_f1(void){ // pop_af
	//	zSP &= 0x00ffff; /* my_masked */
	//	ADJ_FUZZY_PC;
		zAF = RM(zSP );
		zAF |= ((UINT16)RM(zSP+1 ) << 8);
		zSP += 2;
	z80_ICount -= (10-4);
}/*e*/

static void OP80_f2(void){ // jp_p
	if(!(zF & SF))
	{
		JMMW
		pbPC = zBase + dwAddr;	/* Normalize the address */
	}else{pbPC+=2;}
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}/*e*/

static void OP80_f3(void){ // di //Z80I_DI
//	zIFXX &= (~IFF1);

	zIFF1 = 0;
	zIFF2 = 0;
	//z80_ICount -= 4;//ok(gens)
}/*e*/

static void OP80_f4(void){ // call_p
	IMMW
	if(!(zF & SF))
	{
				Z80CALL;
		pbPC = zBase + dwAddr;	/* Normalize the address */
	}
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}/*e*/

static void OP80_f5(void){ // push_af
	zSP -= 2;
//	zSP &= 0x00ffff; /* my_masked */
//	ADJ_FUZZY_PC;
	WM(zSP	, (zAF & 0xff) );
	WM(zSP+1, (zAF >> 8) );
	z80_ICount -= (11-4);
}/*e*/

static void OP80_f6(void){ // or_byte
	zA |= *pbPC++;
	ADJ_FUZZY_PC;
	zF &= ~(CF | NF | VF | HF | ZF | SF);
	zF |= SZP[zA];
	z80_ICount -= (7-4);
}/*e*/

static void OP80_f7(void){ // rst_30
	Rst(0x30);
	z80_ICount -= (11-4);
}/*e*/

static void OP80_f8(void){ // ret_m
	Ret0(zF & SF);
	z80_ICount -= (5-4);
}/*e*/

static void OP80_f9(void){ // ld_sp_hl
	zSP = zHL;
	z80_ICount -= (6-4);
}/*e*/

static void OP80_fa(void){ // jp_m
	if(zF & SF)
	{
		JMMW
		pbPC = zBase + dwAddr;	/* Normalize the address */
	}else{pbPC+=2;}
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}/*e*/

static void OP80_fb(void){ // ei
//	zIFXX |= IFF1;

	zIFF2 = IFF_ON;
	if(0==zIFF1){	zIFF1 = IFF_ON;}

	//z80_ICount -= 4;//ok(gens)
}/*e*/

static void OP80_fc(void){ // call_m
	IMMW
	if(zF & SF)
	{
				Z80CALL;
		pbPC = zBase + dwAddr;	/* Normalize the address */
	}
	ADJ_FUZZY_PC;
	z80_ICount -= (10-4);
}/*e*/

static void OP80_fd(void){ // fd
//	HandleFD();/**???????**/
				{unsigned short mytemp;
				mytemp= zIX;
					zIX=zIY;
					HandleDD();
					zIY=zIX;
				zIX=mytemp;
				}
}

static void OP80_fe(void){ // cp_byte
	zF = (zF & ~(CF | NF | VF | HF | ZF | SF)) |
					pbSubSbcTable[((UINT32) zA << 8) | (*pbPC++)];
	ADJ_FUZZY_PC;
	z80_ICount -= (7-4);
}/*e*/

static void OP80_ff(void){ // rst_38
	Rst(0x38);
	z80_ICount -= (11-4);
}/*e*/
//		}


const static void* OP8080[256]={
&OP80_00,&OP80_01,&OP80_02,&OP80_03,&OP80_04,&OP80_05,&OP80_06,&OP80_07,
&OP80_08,&OP80_09,&OP80_0a,&OP80_0b,&OP80_0c,&OP80_0d,&OP80_0e,&OP80_0f,
&OP80_10,&OP80_11,&OP80_12,&OP80_13,&OP80_14,&OP80_15,&OP80_16,&OP80_17,
&OP80_18,&OP80_19,&OP80_1a,&OP80_1b,&OP80_1c,&OP80_1d,&OP80_1e,&OP80_1f,
&OP80_20,&OP80_21,&OP80_22,&OP80_23,&OP80_24,&OP80_25,&OP80_26,&OP80_27,
&OP80_28,&OP80_29,&OP80_2a,&OP80_2b,&OP80_2c,&OP80_2d,&OP80_2e,&OP80_2f,
&OP80_30,&OP80_31,&OP80_32,&OP80_33,&OP80_34,&OP80_35,&OP80_36,&OP80_37,
&OP80_38,&OP80_39,&OP80_3a,&OP80_3b,&OP80_3c,&OP80_3d,&OP80_3e,&OP80_3f,
&OP80_40,&OP80_41,&OP80_42,&OP80_43,&OP80_44,&OP80_45,&OP80_46,&OP80_47,
&OP80_48,&OP80_49,&OP80_4a,&OP80_4b,&OP80_4c,&OP80_4d,&OP80_4e,&OP80_4f,
&OP80_50,&OP80_51,&OP80_52,&OP80_53,&OP80_54,&OP80_55,&OP80_56,&OP80_57,
&OP80_58,&OP80_59,&OP80_5a,&OP80_5b,&OP80_5c,&OP80_5d,&OP80_5e,&OP80_5f,
&OP80_60,&OP80_61,&OP80_62,&OP80_63,&OP80_64,&OP80_65,&OP80_66,&OP80_67,
&OP80_68,&OP80_69,&OP80_6a,&OP80_6b,&OP80_6c,&OP80_6d,&OP80_6e,&OP80_6f,
&OP80_70,&OP80_71,&OP80_72,&OP80_73,&OP80_74,&OP80_75,&OP80_76,&OP80_77,
&OP80_78,&OP80_79,&OP80_7a,&OP80_7b,&OP80_7c,&OP80_7d,&OP80_7e,&OP80_7f,
&OP80_80,&OP80_81,&OP80_82,&OP80_83,&OP80_84,&OP80_85,&OP80_86,&OP80_87,
&OP80_88,&OP80_89,&OP80_8a,&OP80_8b,&OP80_8c,&OP80_8d,&OP80_8e,&OP80_8f,
&OP80_90,&OP80_91,&OP80_92,&OP80_93,&OP80_94,&OP80_95,&OP80_96,&OP80_97,
&OP80_98,&OP80_99,&OP80_9a,&OP80_9b,&OP80_9c,&OP80_9d,&OP80_9e,&OP80_9f,
&OP80_a0,&OP80_a1,&OP80_a2,&OP80_a3,&OP80_a4,&OP80_a5,&OP80_a6,&OP80_a7,
&OP80_a8,&OP80_a9,&OP80_aa,&OP80_ab,&OP80_ac,&OP80_ad,&OP80_ae,&OP80_af,
&OP80_b0,&OP80_b1,&OP80_b2,&OP80_b3,&OP80_b4,&OP80_b5,&OP80_b6,&OP80_b7,
&OP80_b8,&OP80_b9,&OP80_ba,&OP80_bb,&OP80_bc,&OP80_bd,&OP80_be,&OP80_bf,
&OP80_c0,&OP80_c1,&OP80_c2,&OP80_c3,&OP80_c4,&OP80_c5,&OP80_c6,&OP80_c7,
&OP80_c8,&OP80_c9,&OP80_ca,&OP80_cb,&OP80_cc,&OP80_cd,&OP80_ce,&OP80_cf,
&OP80_d0,&OP80_d1,&OP80_d2,&OP80_d3,&OP80_d4,&OP80_d5,&OP80_d6,&OP80_d7,
&OP80_d8,&OP80_d9,&OP80_da,&OP80_db,&OP80_dc,&OP80_dd,&OP80_de,&OP80_df,
&OP80_e0,&OP80_e1,&OP80_e2,&OP80_e3,&OP80_e4,&OP80_e5,&OP80_e6,&OP80_e7,
&OP80_e8,&OP80_e9,&OP80_ea,&OP80_eb,&OP80_ec,&OP80_ed,&OP80_ee,&OP80_ef,
&OP80_f0,&OP80_f1,&OP80_f2,&OP80_f3,&OP80_f4,&OP80_f5,&OP80_f6,&OP80_f7,
&OP80_f8,&OP80_f9,&OP80_fa,&OP80_fb,&OP80_fc,&OP80_fd,&OP80_fe,&OP80_ff,
};



/****************************************************************************
 * Execute 'cycles' T-states. Return number of T-states really executed
 ****************************************************************************/
 void z80_debt_clk(int cycles){
//	Z80.extra_cycles+=cycles;
}
void z80_execute(UINT32 dwCycles)
{
	unsigned char bOpcode;
//	dwReturnCode = 0x8000/*0000*/; /* Assume it'll work */
	z80_ICount = dwCycles;
	dwOriginalCycles = dwCycles;
	if(zHALT)
	{
		//dwElapsedTicks += dwCycles;
		return;//(0x8000/*0000*/);
	}
#if 1
//	if( zPC < 0x2000 ) // genesis & megadrive ??
	if( zPC < 0x4000 )
	{
		pbPC = zBase + zPC;
		ADJ_FUZZY_PC;/*?*/
	}
	else
	{
		//dwElapsedTicks = 0 ;
		return;// dwReturnCode ;
	}
#endif

	//zR=(zPC)&0x07f; /* refrash ram cycle. */

	z80_ICount &= 0x00ff;

	//while (z80_ICount > 0)
	do {
		/* for debug
		static sp = 0 ;
		if( sp != zSP )
		{
			sp = zSP ;
		}
		*/
		z80_ICount -= 4;	//強制的に４サイクル引く
		//必ずしも合理的なやり方ではないが、この方式に変更。

		bOpcode = *pbPC++;
		ADJ_FUZZY_PC;
	{
	void (*jump_ptr)(void);
		jump_ptr=(void *)OP8080[bOpcode];
		jump_ptr();
	}

	}while (0==(z80_ICount & 0xff00) );
	//dwElapsedTicks += (dwOriginalCycles - z80_ICount);
	ADJ_FUZZY_PC;
	return;//(dwReturnCode); /* Indicate success */
}
#if 00
/* Get mz80's context */
static void z80s_context_copy(void *dest,void *src )
{
int i ;
	for(i=0; i<(sizeof(Z80_Regs)); i++)
	{
		((char*)dest)[i]=((char*)src)[i];
	}
}
void mz80GetContext(void *pData)
{
	z80s_context_copy(pData, &Z80 );
}
/* Set mz80's context */
void mz80SetContext(void *pData)
{
	z80s_context_copy(&Z80, pData );
}
/* Get mz80's context size */
UINT32 mz80GetContextSize(void)
{
	return(sizeof(Z80_Regs));
}
#endif
/* This will return the elapsed ticks */
//UINT32 mz80GetElapsedTicks(UINT32 dwClear)
//{
//	UINT32 dwTemp = dwElapsedTicks;
//	if(dwClear)
//	{
//		dwElapsedTicks = 0;
//	}
//	return(dwTemp);
//}
/* Releases mz80 from its current timeslice */
//void mz80ReleaseTimeslice(void)
//{
//	dwOriginalCycles -= z80_ICount;
//	z80_ICount = 0;
//}
/* This routine is mz80's reset handler */
extern unsigned char* z80_get_mem_ptr(void);
void z80_reset(void)
{
		zBase		=z80_get_mem_ptr();
#if 00
	zBC2 = 0;
	zDE2 = 0;
	zHL2 = 0;
	zAF2 = 0;
	zBC  = 0;
	zDE  = 0;
	zHL  = 0;
	zAF  = 0;
//	zR	 = 0;
	zI	 = 0;
#else
/*もしかしてこう？*/
	zBC2 = 0xffff;
	zDE2 = 0xffff;
	zHL2 = 0xffff;
	zAF2 = 0xffff;
	zBC  = 0xffff;
	zDE  = 0xffff;
	zHL  = 0xffff;
	zAF  = 0xffff;
//	zR	 = 0xffff;
	zI	 = 0xff;
#endif

	zHALT = 0;

	zIM   = IM9;	//どこにも「資料」は無いが、タブン内部的には（回路の都合で）こーゆーふーになってるだろう。
//多分「本物」は
//IM0 0
//IM1 1
//IM2 2
//IM0 3（起動時、動作はIM0と同じ）

	zIX = 0xffff; /* Yes, this is intentional */
	zIY = 0xffff; /* Yes, this is intentional */
	zF	 = ZF;
	zIFF1 = 0;
	zIFF2 = 0;
//	zIFXX = 0 ;

//	z_int_addr = 0x38;
//	z_nmi_addr = 0x66;

	zPC = 0;
	zSP = 0;
}
/* 現在無効 */
#ifdef NEW_Z80_INT
#undef NEW_Z80_INT
#endif

#ifdef NEW_Z80_INT
/* NMI Handler _do_NMI: */
//UINT32 mz80nmi(void)
static void mz80nmi(void)
{
	zIFF1 = 0;

	z80_IntLine &= (~0x80);
//	if(zHALT)
//	{
//
		zHALT = 0;
//	}

	Rst(0x66/*z_nmi_addr*/); /* Our NMI */

	z80_ICount -= 13;//??? clock unknown
//	return(0);
}
#endif

/* Interrupt handler */
void z80_set_irq_line0_off(void)
{
//	Z80.irq_state = CLEAR_LINE;
}
//void z80_set_irq_line0_on(void)
//{
//	Z80.irq_state = ASSERT_LINE;
//	take_interrupt();
//}
//UINT32 z80_set_irq_line0_on(UINT32 dwIntVector_LowAddr)
//UINT32 z80_set_irq_line0_on(unsigned char dwIntVector_LowAddr)
void z80_set_irq_line0_on(void)
{
/* 割り込みチェックでちゅー */
#ifdef NEW_Z80_INT
/* ああそうか、ふつーの割り込み(IRQ)でも「割り込み線」実装せんと、ここで帰っちゃう。だめぽ。 */
	if(0==z80_IntLine) return;					// 割り込み線チェックでちゅー。
	if(0x80==(z80_IntLine&0x80)) mz80nmi(); 	// 絶対、割り込みの神様でちゅー。ははーつ。
#endif
	if(zIFF1 == 0)			// is IRQ disabled? 	// で、（ふつーの）割り込みあるかにゃ？
		return /*0xffffffff*/;	// Interrupt not taken! // 割り込み無いからおしまい。かーえろ。
//	if(0 == (zIFXX & IFF1))
//		return(0xffffffff);


/* 割り込み発生 _do_INT: */
//	zIFXX &= ~(IFF1 | IFF2);
	zIFF1 = 0;

#ifdef NEW_Z80_INT
	z80_IntLine &= 0x80;
#endif
//	if(zHALT)
//	{
//
		zHALT = 0;
//	}
#if 00
	switch(zIM)
	{
	default:
	case IM9:
	case IM0:
	//	Rst(0x38/*z_int_addr*/);//?
	//	z80_ICount -= 13;
	//	break;
	case IM1:
		Rst(0x38/*z_int_addr*/);
	//	zPC = z_int_addr;
		z80_ICount -= 13;
		break;

	case IM2:
		Z80CALL;
		zPC = ((unsigned short)(zI<<8) ) | ( ( ((unsigned short)dwIntVector_LowAddr) & 0x00ff) );
		zPC = ((unsigned short)(zBase[zPC+1]<<8) ) | (zBase[zPC]);

		pbPC = zBase + zPC; /* Normalize the address */
		ADJ_FUZZY_PC;
		z80_ICount -= 19;
		break;
	}
	return(0);
#endif
	if(!(IM2==zIM))/* IM0 or IM1 */
	{
		Rst(0x38/*z_int_addr*/);
	//	zPC = z_int_addr;
		z80_ICount -= 13;
	}else{/* IM2 only */
				Z80CALL;
		zPC = ((unsigned short)(zI<<8) ) /*| ( ( ((unsigned short)dwIntVector_LowAddr) & 0x00ff) )*/;
		zPC = ((unsigned short)(zBase[zPC+1]<<8) ) | (zBase[zPC]);

		pbPC = zBase + zPC; /* Normalize the address */
		ADJ_FUZZY_PC;
		z80_ICount -= 19;
	}
	return/*(0)*/;
}
/* Initialize  */
void z80_init(void)
{
	UINT32 dwLoop;
	unsigned char *pbTempPtr;
	unsigned char *pbTempPtr2;
	unsigned char bNewAdd;
	unsigned char bNewSub;
	unsigned char bFlag;
	unsigned char bLow;
	unsigned char bHigh;
	unsigned char bCarry;
		pbTempPtr  = pbAddAdcTable;
		pbTempPtr2 = pbSubSbcTable;
		for (dwLoop = 0; dwLoop < (256*256*2); dwLoop++)
		{
			bLow = dwLoop & 0xff;
			bHigh = (dwLoop >> 8) & 0xff;
			bCarry = (dwLoop >> 16);
///
			bFlag = 0;
			bNewAdd = bHigh + bLow + bCarry;
			if(0 == bNewAdd)
			{
				bFlag |= ZF;
			}
			else
			{
				bFlag = bNewAdd & 0x80; /* Sign flag */
			}
			if(((UINT32) bLow + (UINT32) bHigh + (UINT32) bCarry) >= 0x100)
			{
				bFlag |= CF;
			}
			if( ((bLow ^ bHigh ^ 0x80) & (bLow ^ (bNewAdd & 0x80))) & 0x80)
			{
				bFlag |= VF;
			}
			if(((bLow & 0x0f) + (bHigh & 0x0f) + bCarry) >= 0x10)
			{
				bFlag |= HF;
			}
			*pbTempPtr++ = bFlag;	/* Store our new flag */
///
			// Now do subtract - Zero
			bFlag = NF;
			bNewSub = bHigh - bLow - bCarry;
			if(0 == bNewSub)
			{
				bFlag |= ZF;
			}
			else
			{
				bFlag |= bNewSub & 0x80; /* Sign flag */
			}
			if( ((INT32) bHigh - (INT32) bLow - (INT32) bCarry) < 0)
			{
				bFlag |= CF;
			}
			if( ((INT32) (bHigh & 0xf) - (INT32) (bLow & 0x0f) - (INT32) bCarry) < 0)
			{
				bFlag |= HF;
			}
			if( ((bLow ^ bHigh) & (bHigh ^ bNewSub) & 0x80) )
			{
				bFlag |= VF;
			}
			*pbTempPtr2++ = bFlag;	/* Store our sub flag */
///
		}
}

/* undef でプリプロの負担を軽くする。 */
#undef zF
#undef zA
#undef zAF
#undef zAF2

#undef zC
#undef zB
#undef zBC
#undef zBC2

#undef zE
#undef zD
#undef zDE
#undef zDE2

#undef zL
#undef zH
#undef zHL
#undef zHL2


#undef zIXh
#undef zIXl
#undef zIX

#undef zIYh
#undef zIYl
#undef zIY

#undef zI
#undef zR

#undef zIM
#undef zHALT

#undef NEG

#undef Ret0_TRUE

#undef IFF_ON

#undef Sll_c8

