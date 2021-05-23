/* ex: set tabstop=4 noexpandtab: */
/*****************************************************************************
 *
 *	 z80.c
 *	 Portable Z80 emulator V3.5(3.6)
 *
 *	 Copyright (C) 1998,1999,2000 Juergen Buchmueller, all rights reserved.
 *
 *	 - This source code is released as freeware for non-commercial purposes.
 *	 - You are free to use and redistribute this code in modified or
 *	   unmodified form, provided you list me in the credits.
 *	 - If you modify this source code, you must add a notice to each modified
 *	   source file that it has been changed.  If you're a nice person, you
 *	   will clearly mark each change too.  :)
 *	 - If you wish to use this for commercial purposes, please contact me at
 *	   pullmoll@t-online.de
 *	 - The author of this copywritten work reserves the right to change the
 *	   terms of its usage and license at any time, including retroactively
 *	 - This entire notice must remain in the source code.
 *
 *	 Changes in 3.6
 *	 - Got rid of the code that would inexactly emulate a Z80, i.e. removed
 *	   all the #if Z80_EXACT #else branches.
 *	 - Removed leading underscores from local register name shortcuts as
 *	   this violates the C99 standard.
 *	 - Renamed the registers inside the Z80 context to lower case to avoid
 *	   ambiguities (shortcuts would have had the same names as the fields
 *	   of the structure).
 *
 *		http://www.msxnet.org/tech/z80-documented.pdf
 *		http://www.msxnet.org/tech/Z80/z80undoc.txt
 *****************************************************************************/

#include "..\shared.h"
#include "z80.h"

//typedef union {
//	struct { UINT8 l,h,h2,h3; } b;
//	struct { UINT16 l,h; } w;
//	UINT32 d;
//} PAIR;

typedef union {
	struct { UINT8 l,h; } b;
	struct { UINT16 l; } w;
}	PAIR;

//#define CPU_16BIT_PORT			0x4000
//#define CPU_FLAGS_MASK			0xff00
#define CLEAR_LINE				0
#define ASSERT_LINE 			1
//#define REG_PREVIOUSPC			-1
//#define REG_SP_CONTENTS		-2

//#define Z80_INT_REQ	0x01	/* interrupt request mask		*/
//#define Z80_INT_IEO	0x02	/* interrupt disable mask(IEO)	*/
//#define Z80_VECTOR(device,state) (((device)<<8)|(state))

////

enum {
	Z80_PC=1, Z80_SP,
	Z80_A, Z80_B, Z80_C, Z80_D, Z80_E, Z80_H, Z80_L,
	Z80_AF, Z80_BC, Z80_DE, Z80_HL,
	Z80_IX, Z80_IY, Z80_AF2, Z80_BC2, Z80_DE2, Z80_HL2,
	Z80_R, Z80_I, Z80_IM, Z80_IFF1, Z80_IFF2, Z80_HALT,
	Z80_DC0, Z80_DC1, Z80_DC2, Z80_DC3
};

/* cycles counts for taken jr/jp/call and interrupt latency (rst opcodes) */

extern unsigned char z80_readmem16(unsigned short address );
//tern unsigned char z80_readram16(unsigned short address );
extern void z80_writemem16(unsigned short address, unsigned char data );

#define _mes(x)

/** configure Z80 emulated **/

/* i:0: execute main opcodes(only) inside a big switch statement */
#define BIG_SWITCH			0

/* f:1: big flags array for ADD/ADC/SUB/SBC/CP results. (1:size smaller! and performance up.) */
#define BIG_FLAGS_ARRAY 	1

/* H:0: on JP and JR opcodes check for tight loops.(slow down this hacking with my psp.) */
#define BUSY_LOOP_HACKS 	0

/* h:0: check for delay loops counting down BC.(equal or still slow down, on my psp.) */
#define TIME_LOOP_HACKS 	0

/* E:1: Set to 1 for a more exact (but somewhat slower?) Z80 emulation */
#define Z80_EXACT			1

/* I:1: If unless exact I/O port emulate. */
#define STILL_IO			1

/* M:0: Still not emurate NMI. */
#define Z80_EMULATE_NMI 	0

#if 0
パフォーマンスチェックの例：
[bf03]	  off  44  22  11  05  [kHz] s#ifHhEIM#
A31m22z03  75  53  63  67+ 70  681kb s#0100111# (add_table_on)(HandleDD/FD)

A31m22z03  74  53- 62  67  69  705kb s#0100111# (add_table_on)
A31m22z03  74  52+ 62- 66+ 69  710kb s#0101111# (hack2)DAME
A31m22z03  73  52  61  66- 68  720kb s#1100111# (big sw)DAME
A31m22z03  72  51  60  65  67  706kb s#0110111# (hack1)DAME
A31m22z03  73  51+ 61+ 66  68  710kb s#0000111# (none)

A31m33z03  75  53  62  67  69+ 685kb s#0100111# (add_table_on)
A31m33z03  74  52+ 61+ 66  69  685kb s#0100111# (add_table_on)(less JR hack)
#endif
/****************************************************************************/
/* The Z80 registers. HALT is set to 1 when the CPU is halted, the refresh	*/
/* register is calculated as follows: refresh=(Z80.r&127)|(Z80.r2&128)		*/
/****************************************************************************/
typedef struct {
	PAIR	bc2;
	PAIR	de2;
	PAIR	hl2;
	PAIR	af2;
//
	PAIR	bc;
	PAIR	de;
	PAIR	hl;
	PAIR	af;
//
	PAIR	pc;
	PAIR	sp;
	PAIR	ix;
	PAIR	iy;
//
//	int 	icount;
	short	extra_cycles;		/* extra cycles for interrupts */
//
	UINT8	irq_state;			/* irq line state */
	UINT8	halt;
	UINT8	im;
	UINT8	i;
//
	//UINT8	iff1;
	//UINT8	iff2;
	UINT8	iffs;
	//UINT8	nmi_state;			/* nmi line state */
	UINT8	after_EI;// = 0;//after _EI
//
//	int 	extra_cycles;		/* extra cycles for interrupts */
//	UINT16	eax;	/*work*/

//,dummy2;
//

//	PAIR	prvpc;
//	int 	(*irq_callback)(int irqline);
}	Z80_Regs;

#define CF	0x01
#define NF	0x02
#define PF	0x04
#define VF	PF
#define XF	0x08
#define HF	0x10
#define YF	0x20
#define ZF	0x40
#define SF	0x80

#define INT_IRQ 0x01
#define NMI_IRQ 0x02

#define zBC 	Z80.bc.w.l
#define zB		Z80.bc.b.h
#define zC		Z80.bc.b.l

#define zDE 	Z80.de.w.l
#define zD		Z80.de.b.h
#define zE		Z80.de.b.l

#define zHL 	Z80.hl.w.l
#define zH		Z80.hl.b.h
#define zL		Z80.hl.b.l

#define zAF 	Z80.af.w.l
#define zA		Z80.af.b.h
#define zF		Z80.af.b.l

#define zIX 	Z80.ix.w.l
#define zIXh	Z80.ix.b.h
#define zIXl	Z80.ix.b.l

#define zIY 	Z80.iy.w.l
#define zIYh	Z80.iy.b.h
#define zIYl	Z80.iy.b.l

#define zI		Z80.i
#define zIM 	Z80.im

//#define zIFF1	Z80.iff1
//#define zIFF2	Z80.iff2
#define zIFFS	Z80.iffs
#define zHALT	Z80.halt

#define zBC2	Z80.bc2.w.l
#define zDE2	Z80.de2.w.l
#define zHL2	Z80.hl2.w.l
#define zAF2	Z80.af2.w.l

#define zPC 	Z80.pc.w.l
#define zSP 	Z80.sp.w.l

//#define EA		Z80.eax
#define zEI 	Z80.after_EI
//#define z80_ICount	Z80.icount
#define zEXcyc	Z80.extra_cycles

#define IM9 0
#define IM0 1
#define IM1 2
#define IM2 3

#define IFF1_ON  (1)
#define IFF2_ON  (1<<2)
#define IFF1_OFF 0
#define IFF2_OFF 0

static Z80_Regs Z80 __attribute__((aligned(64))) ;
static short z80_ICount /*__attribute__((aligned(64)))*/ ;	/* T-state count	*/
//static int zEXcyc;
static UINT16 EA;
static unsigned char *zBase;//z80ram base

//static const UINT8 *cc[4/*6*/];// = { cc_op, cc_cb, cc_ed, cc_xy, cc_ddcb, cc_ex };

static UINT8 SZ[256] __attribute__((aligned(64))) ; 	/* zero and sign flags */
static UINT8 SZP[256];		/* zero, sign and parity flags */
static UINT8 SZHV_inc[256]; /* zero, sign, half carry and overflow flags INC r8 */
static UINT8 SZHV_dec[256]; /* zero, sign, half carry and overflow flags DEC r8 */
static UINT8 SZ_BIT[256];	/* zero, sign and parity/overflow (=zero) flags for BIT opcode */

#if BIG_FLAGS_ARRAY
static unsigned char SZHVC_add[2*256*256];
static unsigned char SZHVC_sub[2*256*256];
#endif

#undef LARGE_DAA_TABLE
#ifdef LARGE_DAA_TABLE
#include "z80daa.h"
#endif

//#if Z80_EXACT
#if 00
/** the old Z80_EXACT tables **/
/* tmp1 value for ini/inir/outi/otir for [C.1-0][io.1-0] */
static UINT8 irep_tmp1[4][4] = {
	{0,0,1,0},{0,1,0,1},{1,0,1,1},{0,1,1,0}
};

/* tmp1 value for ind/indr/outd/otdr for [C.1-0][io.1-0] */
static UINT8 drep_tmp1[4][4] = {
	{0,1,0,0},{1,0,0,1},{0,0,1,0},{0,1,0,1}
};

/* tmp2 value for all in/out repeated opcodes for B.7-0 */
static UINT8 breg_tmp2[256] = {
	0,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,
	0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,
	1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1
};
#endif

//static const UINT8 cc_op[0x100] = {
//	4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4,
//	8,10, 7, 6, 4, 4, 7, 4,12,11, 7, 6, 4, 4, 7, 4,
//	7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4,
//	7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4,
//	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
//	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
//	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
//	7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
//	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
//	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
//	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
//	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
//	5,10,10,10,10,11, 7,11, 5,10,10, 0,10,17, 7,11,
//	5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11,
//	5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11,
//	5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11};

//static const UINT8 cc_op_b[0x100] = {
//	  0,10-4, 7-4, 6-4,   0,   0, 7-4,	 0,   0,11-4, 7-4, 6-4,   0,   0, 7-4,	 0,
//	8-4,10-4, 7-4, 6-4,   0,   0, 7-4,	 0,12-4,11-4, 7-4, 6-4,   0,   0, 7-4,	 0,
//	7-4,10-4,16-4, 6-4,   0,   0, 7-4,	 0, 7-4,11-4,16-4, 6-4,   0,   0, 7-4,	 0,
//	7-4,10-4,13-4, 6-4,11-4,11-4,10-4,	 0, 7-4,11-4,13-4, 6-4,   0,   0, 7-4,	 0,
//	  0,   0,	0,	 0,   0,   0, 7-4,	 0,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	  0,   0,	0,	 0,   0,   0, 7-4,	 0,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	  0,   0,	0,	 0,   0,   0, 7-4,	 0,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	7-4, 7-4, 7-4, 7-4, 7-4, 7-4,	0, 7-4,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	  0,   0,	0,	 0,   0,   0, 7-4,	 0,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	  0,   0,	0,	 0,   0,   0, 7-4,	 0,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	  0,   0,	0,	 0,   0,   0, 7-4,	 0,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	  0,   0,	0,	 0,   0,   0, 7-4,	 0,   0,   0,	0,	 0,   0,   0, 7-4,	 0,
//	5-4,10-4,10-4,10-4,10-4,11-4, 7-4,11-4, 5-4,10-4,10-4,CB-4,10-4,17-4, 7-4,11-4,
//	5-4,10-4,10-4,11-4,10-4,11-4, 7-4,11-4, 5-4,   0,10-4,11-4,10-4,DD-4, 7-4,11-4,
//	5-4,10-4,10-4,19-4,10-4,11-4, 7-4,11-4, 5-4,   0,10-4,	 0,10-4,ED#4, 7-4,11-4,
//	5-4,10-4,10-4,	 0,10-4,11-4, 7-4,11-4, 5-4, 6-4,10-4,	 0,10-4,DD-4, 7-4,11-4};

//static const UINT8 cc_cb[0x100] = {
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//
//	8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
//	8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
//	8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
//	8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
//
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
//	8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8};

//static const UINT8 cc_xy[0x100] = {
//static const UINT8 cc_dd[0x100] = {
//	 4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
//	 4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
//	 4,14,20,10, 9, 9, 9, 4, 4,15,20,10, 9, 9, 9, 4,
//	 4, 4, 4, 4,23,23,19, 4, 4,15, 4, 4, 4, 4, 4, 4,
//	 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
//	 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
//	 9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9,
////19,19,19,19,19,19, 4,19, 4, 4, 4, 4, 9, 9,19, 4,//fix dd76
//	19,19,19,19,19,19,19,19, 4, 4, 4, 4, 9, 9,19, 4,
//	 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
//	 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
//	 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
//	 4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
//	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4,
//	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
//	 4,14, 4,23, 4,15, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4,
//	 4, 4, 4, 4, 4, 4, 4, 4, 4,10, 4, 4, 4, 4, 4, 4};

//static const UINT8 cc_dd[0x100] = {
//	   0,	0,	 0,   0,   0,	0,	 0,   0,   0,15-4,	 0,   0,   0,	0,	 0,   0,
//	   0,	0,	 0,   0,   0,	0,	 0,   0,   0,15-4,	 0,   0,   0,	0,	 0,   0,
//	   0,14-4,20-4,10-4, 9-4, 9-4, 9-4,   0,   0,15-4,20-4,10-4, 9-4, 9-4, 9-4,   0,
//	   0,	0,	 0,   0,23-4,23-4,19-4,   0,   0,15-4,	 0,   0,   0,	0,	 0,   0,
//	   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,
//	   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,
//	 9-4, 9-4, 9-4, 9-4, 9-4, 9-4,19-4, 9-4, 9-4, 9-4, 9-4, 9-4, 9-4, 9-4,19-4, 9-4,
//	19-4,19-4,19-4,19-4,19-4,19-4,19-4,19-4,   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,//fix dd76,4->19
//	   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,
//	   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,
//	   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,
//	   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,   0,	0,	 0,   0, 9-4, 9-4,19-4,   0,
//	   0,	0,	 0,   0,   0,	0,	 0,   0,   0,	0,	 0,4 -4,   0,	0,	 0,   0,//ddcb
//	   0,	0,	 0,   0,   0,	0,	 0,   0,   0,	0,	 0,   0,   0,	0,	 0,   0,
//	   0,14-4,	 0,23-4,   0,15-4,	 0,   0,   0, 8-4,	 0,   0,   0,	0,	 0,   0,
//	   0,	0,	 0,   0,   0,	0,	 0,   0,   0,10-4,	 0,   0,   0,	0,	 0,   0};




static void take_interrupt(void);
static void z80_burn(short cycles);/*  *NEED* Function prototypes.	*/


//typedef void (*funcptr)(void);
//atic void (*tablename[0x100])(void) = {
//atic const funcptr tablename[0x100] = {
//constにしない方が（微妙に）速い。（が運かも）

#define PROTOTYPES(tablename,prefix) \
	INLINE void prefix##_00(void); INLINE void prefix##_01(void); INLINE void prefix##_02(void); INLINE void prefix##_03(void); \
	INLINE void prefix##_04(void); INLINE void prefix##_05(void); INLINE void prefix##_06(void); INLINE void prefix##_07(void); \
	INLINE void prefix##_08(void); INLINE void prefix##_09(void); INLINE void prefix##_0a(void); INLINE void prefix##_0b(void); \
	INLINE void prefix##_0c(void); INLINE void prefix##_0d(void); INLINE void prefix##_0e(void); INLINE void prefix##_0f(void); \
	INLINE void prefix##_10(void); INLINE void prefix##_11(void); INLINE void prefix##_12(void); INLINE void prefix##_13(void); \
	INLINE void prefix##_14(void); INLINE void prefix##_15(void); INLINE void prefix##_16(void); INLINE void prefix##_17(void); \
	INLINE void prefix##_18(void); INLINE void prefix##_19(void); INLINE void prefix##_1a(void); INLINE void prefix##_1b(void); \
	INLINE void prefix##_1c(void); INLINE void prefix##_1d(void); INLINE void prefix##_1e(void); INLINE void prefix##_1f(void); \
	INLINE void prefix##_20(void); INLINE void prefix##_21(void); INLINE void prefix##_22(void); INLINE void prefix##_23(void); \
	INLINE void prefix##_24(void); INLINE void prefix##_25(void); INLINE void prefix##_26(void); INLINE void prefix##_27(void); \
	INLINE void prefix##_28(void); INLINE void prefix##_29(void); INLINE void prefix##_2a(void); INLINE void prefix##_2b(void); \
	INLINE void prefix##_2c(void); INLINE void prefix##_2d(void); INLINE void prefix##_2e(void); INLINE void prefix##_2f(void); \
	INLINE void prefix##_30(void); INLINE void prefix##_31(void); INLINE void prefix##_32(void); INLINE void prefix##_33(void); \
	INLINE void prefix##_34(void); INLINE void prefix##_35(void); INLINE void prefix##_36(void); INLINE void prefix##_37(void); \
	INLINE void prefix##_38(void); INLINE void prefix##_39(void); INLINE void prefix##_3a(void); INLINE void prefix##_3b(void); \
	INLINE void prefix##_3c(void); INLINE void prefix##_3d(void); INLINE void prefix##_3e(void); INLINE void prefix##_3f(void); \
	INLINE void prefix##_40(void); INLINE void prefix##_41(void); INLINE void prefix##_42(void); INLINE void prefix##_43(void); \
	INLINE void prefix##_44(void); INLINE void prefix##_45(void); INLINE void prefix##_46(void); INLINE void prefix##_47(void); \
	INLINE void prefix##_48(void); INLINE void prefix##_49(void); INLINE void prefix##_4a(void); INLINE void prefix##_4b(void); \
	INLINE void prefix##_4c(void); INLINE void prefix##_4d(void); INLINE void prefix##_4e(void); INLINE void prefix##_4f(void); \
	INLINE void prefix##_50(void); INLINE void prefix##_51(void); INLINE void prefix##_52(void); INLINE void prefix##_53(void); \
	INLINE void prefix##_54(void); INLINE void prefix##_55(void); INLINE void prefix##_56(void); INLINE void prefix##_57(void); \
	INLINE void prefix##_58(void); INLINE void prefix##_59(void); INLINE void prefix##_5a(void); INLINE void prefix##_5b(void); \
	INLINE void prefix##_5c(void); INLINE void prefix##_5d(void); INLINE void prefix##_5e(void); INLINE void prefix##_5f(void); \
	INLINE void prefix##_60(void); INLINE void prefix##_61(void); INLINE void prefix##_62(void); INLINE void prefix##_63(void); \
	INLINE void prefix##_64(void); INLINE void prefix##_65(void); INLINE void prefix##_66(void); INLINE void prefix##_67(void); \
	INLINE void prefix##_68(void); INLINE void prefix##_69(void); INLINE void prefix##_6a(void); INLINE void prefix##_6b(void); \
	INLINE void prefix##_6c(void); INLINE void prefix##_6d(void); INLINE void prefix##_6e(void); INLINE void prefix##_6f(void); \
	INLINE void prefix##_70(void); INLINE void prefix##_71(void); INLINE void prefix##_72(void); INLINE void prefix##_73(void); \
	INLINE void prefix##_74(void); INLINE void prefix##_75(void); INLINE void prefix##_76(void); INLINE void prefix##_77(void); \
	INLINE void prefix##_78(void); INLINE void prefix##_79(void); INLINE void prefix##_7a(void); INLINE void prefix##_7b(void); \
	INLINE void prefix##_7c(void); INLINE void prefix##_7d(void); INLINE void prefix##_7e(void); INLINE void prefix##_7f(void); \
	INLINE void prefix##_80(void); INLINE void prefix##_81(void); INLINE void prefix##_82(void); INLINE void prefix##_83(void); \
	INLINE void prefix##_84(void); INLINE void prefix##_85(void); INLINE void prefix##_86(void); INLINE void prefix##_87(void); \
	INLINE void prefix##_88(void); INLINE void prefix##_89(void); INLINE void prefix##_8a(void); INLINE void prefix##_8b(void); \
	INLINE void prefix##_8c(void); INLINE void prefix##_8d(void); INLINE void prefix##_8e(void); INLINE void prefix##_8f(void); \
	INLINE void prefix##_90(void); INLINE void prefix##_91(void); INLINE void prefix##_92(void); INLINE void prefix##_93(void); \
	INLINE void prefix##_94(void); INLINE void prefix##_95(void); INLINE void prefix##_96(void); INLINE void prefix##_97(void); \
	INLINE void prefix##_98(void); INLINE void prefix##_99(void); INLINE void prefix##_9a(void); INLINE void prefix##_9b(void); \
	INLINE void prefix##_9c(void); INLINE void prefix##_9d(void); INLINE void prefix##_9e(void); INLINE void prefix##_9f(void); \
	INLINE void prefix##_a0(void); INLINE void prefix##_a1(void); INLINE void prefix##_a2(void); INLINE void prefix##_a3(void); \
	INLINE void prefix##_a4(void); INLINE void prefix##_a5(void); INLINE void prefix##_a6(void); INLINE void prefix##_a7(void); \
	INLINE void prefix##_a8(void); INLINE void prefix##_a9(void); INLINE void prefix##_aa(void); INLINE void prefix##_ab(void); \
	INLINE void prefix##_ac(void); INLINE void prefix##_ad(void); INLINE void prefix##_ae(void); INLINE void prefix##_af(void); \
	INLINE void prefix##_b0(void); INLINE void prefix##_b1(void); INLINE void prefix##_b2(void); INLINE void prefix##_b3(void); \
	INLINE void prefix##_b4(void); INLINE void prefix##_b5(void); INLINE void prefix##_b6(void); INLINE void prefix##_b7(void); \
	INLINE void prefix##_b8(void); INLINE void prefix##_b9(void); INLINE void prefix##_ba(void); INLINE void prefix##_bb(void); \
	INLINE void prefix##_bc(void); INLINE void prefix##_bd(void); INLINE void prefix##_be(void); INLINE void prefix##_bf(void); \
	INLINE void prefix##_c0(void); INLINE void prefix##_c1(void); INLINE void prefix##_c2(void); INLINE void prefix##_c3(void); \
	INLINE void prefix##_c4(void); INLINE void prefix##_c5(void); INLINE void prefix##_c6(void); INLINE void prefix##_c7(void); \
	INLINE void prefix##_c8(void); INLINE void prefix##_c9(void); INLINE void prefix##_ca(void); INLINE void prefix##_cb(void); \
	INLINE void prefix##_cc(void); INLINE void prefix##_cd(void); INLINE void prefix##_ce(void); INLINE void prefix##_cf(void); \
	INLINE void prefix##_d0(void); INLINE void prefix##_d1(void); INLINE void prefix##_d2(void); INLINE void prefix##_d3(void); \
	INLINE void prefix##_d4(void); INLINE void prefix##_d5(void); INLINE void prefix##_d6(void); INLINE void prefix##_d7(void); \
	INLINE void prefix##_d8(void); INLINE void prefix##_d9(void); INLINE void prefix##_da(void); INLINE void prefix##_db(void); \
	INLINE void prefix##_dc(void); INLINE void prefix##_dd(void); INLINE void prefix##_de(void); INLINE void prefix##_df(void); \
	INLINE void prefix##_e0(void); INLINE void prefix##_e1(void); INLINE void prefix##_e2(void); INLINE void prefix##_e3(void); \
	INLINE void prefix##_e4(void); INLINE void prefix##_e5(void); INLINE void prefix##_e6(void); INLINE void prefix##_e7(void); \
	INLINE void prefix##_e8(void); INLINE void prefix##_e9(void); INLINE void prefix##_ea(void); INLINE void prefix##_eb(void); \
	INLINE void prefix##_ec(void); INLINE void prefix##_ed(void); INLINE void prefix##_ee(void); INLINE void prefix##_ef(void); \
	INLINE void prefix##_f0(void); INLINE void prefix##_f1(void); INLINE void prefix##_f2(void); INLINE void prefix##_f3(void); \
	INLINE void prefix##_f4(void); INLINE void prefix##_f5(void); INLINE void prefix##_f6(void); INLINE void prefix##_f7(void); \
	INLINE void prefix##_f8(void); INLINE void prefix##_f9(void); INLINE void prefix##_fa(void); INLINE void prefix##_fb(void); \
	INLINE void prefix##_fc(void); INLINE void prefix##_fd(void); INLINE void prefix##_fe(void); INLINE void prefix##_ff(void); \
static void (*tablename[0x100])(void) = {	\
	prefix##_00,prefix##_01,prefix##_02,prefix##_03,prefix##_04,prefix##_05,prefix##_06,prefix##_07, \
	prefix##_08,prefix##_09,prefix##_0a,prefix##_0b,prefix##_0c,prefix##_0d,prefix##_0e,prefix##_0f, \
	prefix##_10,prefix##_11,prefix##_12,prefix##_13,prefix##_14,prefix##_15,prefix##_16,prefix##_17, \
	prefix##_18,prefix##_19,prefix##_1a,prefix##_1b,prefix##_1c,prefix##_1d,prefix##_1e,prefix##_1f, \
	prefix##_20,prefix##_21,prefix##_22,prefix##_23,prefix##_24,prefix##_25,prefix##_26,prefix##_27, \
	prefix##_28,prefix##_29,prefix##_2a,prefix##_2b,prefix##_2c,prefix##_2d,prefix##_2e,prefix##_2f, \
	prefix##_30,prefix##_31,prefix##_32,prefix##_33,prefix##_34,prefix##_35,prefix##_36,prefix##_37, \
	prefix##_38,prefix##_39,prefix##_3a,prefix##_3b,prefix##_3c,prefix##_3d,prefix##_3e,prefix##_3f, \
	prefix##_40,prefix##_41,prefix##_42,prefix##_43,prefix##_44,prefix##_45,prefix##_46,prefix##_47, \
	prefix##_48,prefix##_49,prefix##_4a,prefix##_4b,prefix##_4c,prefix##_4d,prefix##_4e,prefix##_4f, \
	prefix##_50,prefix##_51,prefix##_52,prefix##_53,prefix##_54,prefix##_55,prefix##_56,prefix##_57, \
	prefix##_58,prefix##_59,prefix##_5a,prefix##_5b,prefix##_5c,prefix##_5d,prefix##_5e,prefix##_5f, \
	prefix##_60,prefix##_61,prefix##_62,prefix##_63,prefix##_64,prefix##_65,prefix##_66,prefix##_67, \
	prefix##_68,prefix##_69,prefix##_6a,prefix##_6b,prefix##_6c,prefix##_6d,prefix##_6e,prefix##_6f, \
	prefix##_70,prefix##_71,prefix##_72,prefix##_73,prefix##_74,prefix##_75,prefix##_76,prefix##_77, \
	prefix##_78,prefix##_79,prefix##_7a,prefix##_7b,prefix##_7c,prefix##_7d,prefix##_7e,prefix##_7f, \
	prefix##_80,prefix##_81,prefix##_82,prefix##_83,prefix##_84,prefix##_85,prefix##_86,prefix##_87, \
	prefix##_88,prefix##_89,prefix##_8a,prefix##_8b,prefix##_8c,prefix##_8d,prefix##_8e,prefix##_8f, \
	prefix##_90,prefix##_91,prefix##_92,prefix##_93,prefix##_94,prefix##_95,prefix##_96,prefix##_97, \
	prefix##_98,prefix##_99,prefix##_9a,prefix##_9b,prefix##_9c,prefix##_9d,prefix##_9e,prefix##_9f, \
	prefix##_a0,prefix##_a1,prefix##_a2,prefix##_a3,prefix##_a4,prefix##_a5,prefix##_a6,prefix##_a7, \
	prefix##_a8,prefix##_a9,prefix##_aa,prefix##_ab,prefix##_ac,prefix##_ad,prefix##_ae,prefix##_af, \
	prefix##_b0,prefix##_b1,prefix##_b2,prefix##_b3,prefix##_b4,prefix##_b5,prefix##_b6,prefix##_b7, \
	prefix##_b8,prefix##_b9,prefix##_ba,prefix##_bb,prefix##_bc,prefix##_bd,prefix##_be,prefix##_bf, \
	prefix##_c0,prefix##_c1,prefix##_c2,prefix##_c3,prefix##_c4,prefix##_c5,prefix##_c6,prefix##_c7, \
	prefix##_c8,prefix##_c9,prefix##_ca,prefix##_cb,prefix##_cc,prefix##_cd,prefix##_ce,prefix##_cf, \
	prefix##_d0,prefix##_d1,prefix##_d2,prefix##_d3,prefix##_d4,prefix##_d5,prefix##_d6,prefix##_d7, \
	prefix##_d8,prefix##_d9,prefix##_da,prefix##_db,prefix##_dc,prefix##_dd,prefix##_de,prefix##_df, \
	prefix##_e0,prefix##_e1,prefix##_e2,prefix##_e3,prefix##_e4,prefix##_e5,prefix##_e6,prefix##_e7, \
	prefix##_e8,prefix##_e9,prefix##_ea,prefix##_eb,prefix##_ec,prefix##_ed,prefix##_ee,prefix##_ef, \
	prefix##_f0,prefix##_f1,prefix##_f2,prefix##_f3,prefix##_f4,prefix##_f5,prefix##_f6,prefix##_f7, \
	prefix##_f8,prefix##_f9,prefix##_fa,prefix##_fb,prefix##_fc,prefix##_fd,prefix##_fe,prefix##_ff  \
}

#define op_40 op_00 /* LD	B,B*/
#define op_49 op_00 /* LD	C,C*/
#define op_52 op_00 /* LD	D,D*/
#define op_5b op_00 /* LD	E,E*/
#define op_64 op_00 /* LD	H,H*/
#define op_6d op_00 /* LD	L,L*/
#define op_7f op_00 /* LD	A,A*/


PROTOTYPES(Z80op,op);
PROTOTYPES(Z80cb,cb);

INLINE void dd_09(void);
INLINE void dd_19(void);
INLINE void dd_21(void); INLINE void dd_22(void); INLINE void dd_23(void);
INLINE void dd_24(void); INLINE void dd_25(void); INLINE void dd_26(void);

INLINE void dd_29(void); INLINE void dd_2a(void); INLINE void dd_2b(void);
INLINE void dd_2c(void); INLINE void dd_2d(void); INLINE void dd_2e(void);

INLINE void dd_34(void); INLINE void dd_35(void); INLINE void dd_36(void);

INLINE void dd_39(void);

INLINE void dd_44(void); INLINE void dd_45(void); INLINE void dd_46(void);
INLINE void dd_4c(void); INLINE void dd_4d(void); INLINE void dd_4e(void);
INLINE void dd_54(void); INLINE void dd_55(void); INLINE void dd_56(void);
INLINE void dd_5c(void); INLINE void dd_5d(void); INLINE void dd_5e(void);

INLINE void dd_60(void); INLINE void dd_61(void); INLINE void dd_62(void); INLINE void dd_63(void);
INLINE void dd_64(void); INLINE void dd_65(void); INLINE void dd_66(void); INLINE void dd_67(void);
INLINE void dd_68(void); INLINE void dd_69(void); INLINE void dd_6a(void); INLINE void dd_6b(void);
INLINE void dd_6c(void); INLINE void dd_6d(void); INLINE void dd_6e(void); INLINE void dd_6f(void);
INLINE void dd_70(void); INLINE void dd_71(void); INLINE void dd_72(void); INLINE void dd_73(void);
INLINE void dd_74(void); INLINE void dd_75(void); INLINE void dd_76(void); INLINE void dd_77(void);

INLINE void dd_7c(void); INLINE void dd_7d(void); INLINE void dd_7e(void);
INLINE void dd_84(void); INLINE void dd_85(void); INLINE void dd_86(void);
INLINE void dd_8c(void); INLINE void dd_8d(void); INLINE void dd_8e(void);
INLINE void dd_94(void); INLINE void dd_95(void); INLINE void dd_96(void);
INLINE void dd_9c(void); INLINE void dd_9d(void); INLINE void dd_9e(void);
INLINE void dd_a4(void); INLINE void dd_a5(void); INLINE void dd_a6(void);
INLINE void dd_ac(void); INLINE void dd_ad(void); INLINE void dd_ae(void);
INLINE void dd_b4(void); INLINE void dd_b5(void); INLINE void dd_b6(void);
INLINE void dd_bc(void); INLINE void dd_bd(void); INLINE void dd_be(void);

//
INLINE void dd_cb(void);
INLINE void dd_e1(void); INLINE void dd_e3(void);
INLINE void dd_e5(void);
INLINE void dd_e9(void);
INLINE void dd_f9(void);

static void (*Z80dd[0x100])(void) = {
	op_00,op_01,op_02,op_03,op_04,op_05,op_06,op_07,op_08,dd_09,op_0a,op_0b,op_0c,op_0d,op_0e,op_0f,
	op_10,op_11,op_12,op_13,op_14,op_15,op_16,op_17,op_18,dd_19,op_1a,op_1b,op_1c,op_1d,op_1e,op_1f,
	op_20,dd_21,dd_22,dd_23,dd_24,dd_25,dd_26,op_27,op_28,dd_29,dd_2a,dd_2b,dd_2c,dd_2d,dd_2e,op_2f,
	op_30,op_31,op_32,op_33,dd_34,dd_35,dd_36,op_37,op_38,dd_39,op_3a,op_3b,op_3c,op_3d,op_3e,op_3f,
	op_40,op_41,op_42,op_43,dd_44,dd_45,dd_46,op_47,op_48,op_49,op_4a,op_4b,dd_4c,dd_4d,dd_4e,op_4f,
	op_50,op_51,op_52,op_53,dd_54,dd_55,dd_56,op_57,op_58,op_59,op_5a,op_5b,dd_5c,dd_5d,dd_5e,op_5f,
	dd_60,dd_61,dd_62,dd_63,dd_64,dd_65,dd_66,dd_67,dd_68,dd_69,dd_6a,dd_6b,dd_6c,dd_6d,dd_6e,dd_6f,
	dd_70,dd_71,dd_72,dd_73,dd_74,dd_75,dd_76,dd_77,op_78,op_79,op_7a,op_7b,dd_7c,dd_7d,dd_7e,op_7f,
	op_80,op_81,op_82,op_83,dd_84,dd_85,dd_86,op_87,op_88,op_89,op_8a,op_8b,dd_8c,dd_8d,dd_8e,op_8f,
	op_90,op_91,op_92,op_93,dd_94,dd_95,dd_96,op_97,op_98,op_99,op_9a,op_9b,dd_9c,dd_9d,dd_9e,op_9f,
	op_a0,op_a1,op_a2,op_a3,dd_a4,dd_a5,dd_a6,op_a7,op_a8,op_a9,op_aa,op_ab,dd_ac,dd_ad,dd_ae,op_af,
	op_b0,op_b1,op_b2,op_b3,dd_b4,dd_b5,dd_b6,op_b7,op_b8,op_b9,op_ba,op_bb,dd_bc,dd_bd,dd_be,op_bf,
	op_c0,op_c1,op_c2,op_c3,op_c4,op_c5,op_c6,op_c7,op_c8,op_c9,op_ca,dd_cb,op_cc,op_cd,op_ce,op_cf,
	op_d0,op_d1,op_d2,op_d3,op_d4,op_d5,op_d6,op_d7,op_d8,op_d9,op_da,op_db,op_dc,op_dd,op_de,op_df,
	op_e0,dd_e1,op_e2,dd_e3,op_e4,dd_e5,op_e6,op_e7,op_e8,dd_e9,op_ea,op_eb,op_ec,op_ed,op_ee,op_ef,
	op_f0,op_f1,op_f2,op_f3,op_f4,op_f5,op_f6,op_f7,op_f8,dd_f9,op_fa,op_fb,op_fc,op_fd,op_fe,op_ff
};


//PROTOTYPES(Z80dd,dd); //HandleDD//(zIX)
//PROTOTYPES(Z80ed,ed); //HandleED
//PROTOTYPES(Z80fd,fd); //HandleFD//(zIY)




#define ddcb_40 ddcb_46
#define ddcb_41 ddcb_46
#define ddcb_42 ddcb_46
#define ddcb_43 ddcb_46
#define ddcb_44 ddcb_46
#define ddcb_45 ddcb_46
#define ddcb_47 ddcb_46

#define ddcb_48 ddcb_4e
#define ddcb_49 ddcb_4e
#define ddcb_4a ddcb_4e
#define ddcb_4b ddcb_4e
#define ddcb_4c ddcb_4e
#define ddcb_4d ddcb_4e
#define ddcb_4f ddcb_4e

#define ddcb_50 ddcb_56
#define ddcb_51 ddcb_56
#define ddcb_52 ddcb_56
#define ddcb_53 ddcb_56
#define ddcb_54 ddcb_56
#define ddcb_55 ddcb_56
#define ddcb_57 ddcb_56

#define ddcb_58 ddcb_5e
#define ddcb_59 ddcb_5e
#define ddcb_5a ddcb_5e
#define ddcb_5b ddcb_5e
#define ddcb_5c ddcb_5e
#define ddcb_5d ddcb_5e
#define ddcb_5f ddcb_5e

#define ddcb_60 ddcb_66
#define ddcb_61 ddcb_66
#define ddcb_62 ddcb_66
#define ddcb_63 ddcb_66
#define ddcb_64 ddcb_66
#define ddcb_65 ddcb_66
#define ddcb_67 ddcb_66

#define ddcb_68 ddcb_6e
#define ddcb_69 ddcb_6e
#define ddcb_6a ddcb_6e
#define ddcb_6b ddcb_6e
#define ddcb_6c ddcb_6e
#define ddcb_6d ddcb_6e
#define ddcb_6f ddcb_6e

#define ddcb_70 ddcb_76
#define ddcb_71 ddcb_76
#define ddcb_72 ddcb_76
#define ddcb_73 ddcb_76
#define ddcb_74 ddcb_76
#define ddcb_75 ddcb_76
#define ddcb_77 ddcb_76

#define ddcb_78 ddcb_7e
#define ddcb_79 ddcb_7e
#define ddcb_7a ddcb_7e
#define ddcb_7b ddcb_7e
#define ddcb_7c ddcb_7e
#define ddcb_7d ddcb_7e
#define ddcb_7f ddcb_7e



PROTOTYPES(Z80ddcb,ddcb);




#if 1
//OPTによってはイル。
/****************************************************************************/
/* Burn an odd amount of cycles, that is instructions taking something		*/
/* different from 4 T-states per opcode (and R increment)					*/
/****************************************************************************/
static __inline__ void BURNODD(int cycles, int opcodes, int cyclesum)
{
	if( cycles > 0 )
	{
		//zR += (cycles / cyclesum) * opcodes;
		z80_ICount -= (cycles / cyclesum) * cyclesum;
	}
}
#endif

/***************************************************************
 * define an opcode function
 ***************************************************************/
#define OP(prefix,opcode)  INLINE void prefix##_##opcode(void)

/***************************************************************
 * adjust cycle count by n T-states
 ***************************************************************/
#define CC(prefix,opcode) z80_ICount -= cc_##prefix[opcode]


#if 000

MEMO::
#define EXEC_INLINE(prefix,opcode)
{
	unsigned op = opcode;
	CC(prefix,op);
	switch(op)
	{
	case 0x00:prefix##_##00();break;
	case 0x01:prefix##_##01();break;
	case 0x02:prefix##_##02();break;
...
	case 0xfe:prefix##_##fe();break;
	case 0xff:prefix##_##ff();break;
	}
}
#endif


/***************************************************************
 * Enter HALT state; write 1 to fake port on first execution
 ***************************************************************/
#define ENTER_HALT {											\
	zPC--;														\
	zHALT = 1;													\
	if( !zEI )											\
		z80_burn( z80_ICount ); 								\
}

/***************************************************************
 * Leave HALT state; write 0 to fake port
 ***************************************************************/
#define LEAVE_HALT {											\
	if( zHALT ) 												\
	{															\
		zHALT = 0;												\
		zPC++;													\
	}															\
}

/***************************************************************
 * Input a byte from given I/O port
 * Output a byte to given I/O port
 ***************************************************************/
#define IN(port) 0xff;
#define OUT(port,value) ;

/***************************************************************
 * Read a byte from given memory location
 ***************************************************************/
#define RM(addr) (UINT8)z80_readmem16(addr)

/***************************************************************
 * Read a word from given memory location
 ***************************************************************/
static __inline__ void RM16( UINT16 addr, PAIR *r )
{
	r->b.l = RM((addr  )       );
	r->b.h = RM((addr+1)&0xffff);
}

/***************************************************************
 * Write a byte to given memory location
 ***************************************************************/
#define WM(addr,value) z80_writemem16(addr,value)

/***************************************************************
 * Write a word to given memory location
 ***************************************************************/
static __inline__ void WM16( UINT16 addr, PAIR *r )
{
	WM((addr  )       ,r->b.l);
	WM((addr+1)&0xffff,r->b.h);
}

/***************************************************************
 * ROP() is identical to RM() except it is used for
 * reading opcodes. In case of system with memory mapped I/O,
 * this function can be used to greatly speed up emulation
 ***************************************************************/
//static __inline__ UINT8 ROP(void)
//{
//	unsigned pc = zPCD;
//	zPC++;
//	return z80_readmem16(pc);
//}
#define ROP ARG
/****************************************************************
 * ARG() is identical to ROP() except it is used
 * for reading opcode arguments. This difference can be used to
 * support systems that use different encoding mechanisms for
 * opcodes and opcode arguments
 ***************************************************************/
static __inline__ UINT8 ARG(void)
{
	unsigned short pc = zPC;
	zPC++;
//	return z80_readmem16(pc);
	return (*(zBase+pc));
//	return z80_readmem16(zPC++);
}

static __inline__ UINT32 ARG16(void)
{
	unsigned short pc = zPC;
	zPC += 2;
//	return z80_readmem16(pc) | (z80_readmem16((pc+1)&0xffff) << 8);
	return (*(zBase+pc)) | (((unsigned short)(*(zBase+pc+1))) << 8);
}

/***************************************************************
 * Calculate the effective address EA of an opcode using
 * IX+offset resp. IY+offset addressing.
 ***************************************************************/
#define IX8ARG (UINT16)(zIX+(INT8)ARG())

/***************************************************************
 * POP
 ***************************************************************/
#define POP(DR) { RM16( zSP, &Z80.DR ); zSP += 2; }

/***************************************************************
 * PUSH
 ***************************************************************/
#define PUSH(SR) { zSP -= 2; WM16( zSP, &Z80.SR ); }

#define XLK_NOP  4/*cc_op[0x00]*/
#define XLK_18_ 12/*cc_op[0x18]*/
#define XLK_31_ 10/*cc_op[0x31]*/
#define XLK_c3_ 10/*cc_op[0xc3]*/
/***************************************************************
 * JP
 ***************************************************************/
#if BUSY_LOOP_HACKS
#define JP {									\
	unsigned short oldpc = zPC-1;				\
	zPC = ARG16();								\
	/* speed up busy loop */					\
	if( zPC == oldpc )							\
	{											\
		if( !zEI )								\
			BURNODD( z80_ICount, 1, XLK_c3_ );	\
	}											\
	else										\
	{											\
		UINT8 op = z80_readmem16(zPC); 			\
		if( zPC == (oldpc-1) )					\
		{										\
			/* NOP - JP $-1 or EI - JP $-1 */	\
			if( op == 0x00 || op == 0xfb )		\
			{									\
				if( !zEI )						\
					BURNODD( z80_ICount-XLK_NOP, 2, XLK_NOP+XLK_c3_); \
			}									\
		}										\
		else									\
		/* LD SP,#xxxx - JP $-3 (Galaga) */ 	\
		if( zPC == (oldpc-3) && op == 0x31 )	\
		{										\
			if( !zEI )							\
				BURNODD( z80_ICount-XLK_31_, 2, XLK_31_+XLK_c3_); \
		}										\
	}											\
}
#else
#define JP {		zPC = ARG16();		}
#endif






/***************************************************************
 * RETI
 ***************************************************************/
#define RETI	{												\
	POP(pc);													\
/* according to http://www.msxnet.org/tech/Z80/z80undoc.txt */	\
/* according to http://www.msxnet.org/tech/z80-documented.pdf */\
/*	zIFF1 = zIFF2;	*/											\
}

/***************************************************************
 * LD	R,A
 ***************************************************************/
#define LD_R_A {												\
	/*zR = zA;*/												\
	/*zR2 = zA & 0x80;*/			/* keep bit 7 of R */		\
}

/***************************************************************
 * LD	A,R
 ***************************************************************/
#define LD_A_R {												\
	zA = (zA+7)&0xff;/*(zR & 0x7f) | zR2;*/ 					\
	zF = (zF & CF) | SZ[zA] | (zIFFS&IFF2_ON)/*( zIFF2 << 2 )*/;					\
}


/***************************************************************
 * INC	r8
 ***************************************************************/
static __inline__ UINT8 INC(UINT8 value)
{
	UINT8 res = value + 1;
	zF = (zF & CF) | SZHV_inc[res];
	return (UINT8)res;
}

/***************************************************************
 * DEC	r8
 ***************************************************************/
static __inline__ UINT8 DEC(UINT8 value)
{
	UINT8 res = value - 1;
	zF = (zF & CF) | SZHV_dec[res];
	return res;
}

/***************************************************************
 * RLCA
 ***************************************************************/
#if Z80_EXACT
#define RLCA													\
	zA = (zA << 1) | (zA >> 7); 								\
	zF = (zF & (SF | ZF | PF)) | (zA & (YF | XF | CF))
#else
#define RLCA													\
	zA = (zA << 1) | (zA >> 7); 								\
	zF = (zF & (SF | ZF | YF | XF | PF)) | (zA & CF)
#endif

/***************************************************************
 * RRCA
 ***************************************************************/
#if Z80_EXACT
#define RRCA													\
	zF = (zF & (SF | ZF | PF)) | (zA & CF); 					\
	zA = (zA >> 1) | (zA << 7); 								\
	zF |= (zA & (YF | XF) )
#else
#define RRCA													\
	zF = (zF & (SF | ZF | YF | XF | PF)) | (zA & CF);			\
	zA = (zA >> 1) | (zA << 7)
#endif

/***************************************************************
 * RLA
 ***************************************************************/
#if Z80_EXACT
#define RLA {													\
	UINT8 res = (zA << 1) | (zF & CF);							\
	UINT8 c = (zA & 0x80) ? CF : 0; 							\
	zF = (zF & (SF | ZF | PF)) | c | (res & (YF | XF)); 		\
	zA = res;													\
}
#else
#define RLA {													\
	UINT8 res = (zA << 1) | (zF & CF);							\
	UINT8 c = (zA & 0x80) ? CF : 0; 							\
	zF = (zF & (SF | ZF | YF | XF | PF)) | c;					\
	zA = res;													\
}
#endif

/***************************************************************
 * RRA
 ***************************************************************/
#if Z80_EXACT
#define RRA {													\
	UINT8 res = (zA >> 1) | (zF << 7);							\
	UINT8 c = (zA & 0x01) ? CF : 0; 							\
	zF = (zF & (SF | ZF | PF)) | c | (res & (YF | XF)); 		\
	zA = res;													\
}
#else
#define RRA {													\
	UINT8 res = (zA >> 1) | (zF << 7);							\
	UINT8 c = (zA & 0x01) ? CF : 0; 							\
	zF = (zF & (SF | ZF | YF | XF | PF)) | c;					\
	zA = res;													\
}
#endif

/***************************************************************
 * RRD
 ***************************************************************/
#define RRD {													\
	UINT8 n = RM(zHL);											\
	WM( zHL, (n >> 4) | (zA << 4) );							\
	zA = (zA & 0xf0) | (n & 0x0f);								\
	zF = (zF & CF) | SZP[zA];									\
}

/***************************************************************
 * RLD
 ***************************************************************/
#define RLD {													\
	UINT8 n = RM(zHL);											\
	WM( zHL, (n << 4) | (zA & 0x0f) );							\
	zA = (zA & 0xf0) | (n >> 4);								\
	zF = (zF & CF) | SZP[zA];									\
}

/***************************************************************
 * ADD	A,n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define ADD(value)												\
{																\
	UINT32  ah = zAF & 0xff00;									\
	UINT32 res = (UINT8)((ah >> 8) + value);					\
	zF = SZHVC_add[ah | res];									\
	zA = res;													\
}
#else
#define ADD(value)												\
{																\
	unsigned val = value;										\
	unsigned res = zA + val;									\
	zF = SZ[(UINT8)res] | ((res >> 8) & CF) |					\
		((zA ^ res ^ val) & HF) |								\
		(((val ^ zA ^ 0x80) & (val ^ res) & 0x80) >> 5);		\
	zA = (UINT8)res;											\
}
#endif

/***************************************************************
 * ADC	A,n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define ADC(value)												\
{																\
	UINT32 ah = (zAF & 0xff00);									\
	UINT32  c = (zAF & 1);										\
	UINT32 res = (UINT8)((ah >> 8) + value + c);				\
	zF = SZHVC_add[(c << 16) | ah | res];						\
	zA = res;													\
}
#else
#define ADC(value)												\
{																\
	unsigned val = value;										\
	unsigned res = zA + val + (zF & CF);						\
	zF = SZ[res & 0xff] | ((res >> 8) & CF) |					\
		((zA ^ res ^ val) & HF) |								\
		(((val ^ zA ^ 0x80) & (val ^ res) & 0x80) >> 5);		\
	zA = res;													\
}
#endif

/***************************************************************
 * SUB	n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define SUB(value)												\
{																\
	UINT32 ah = (zAF & 0xff00); 								\
	UINT32 res = (UINT8)((ah >> 8) - value);					\
	zF = SZHVC_sub[ah | res];									\
	zA = res;													\
}
#else
#define SUB(value)												\
{																\
	unsigned val = value;										\
	unsigned res = zA - val;									\
	zF = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((zA ^ res ^ val) & HF) |								\
		(((val ^ zA) & (zA ^ res) & 0x80) >> 5);				\
	zA = res;													\
}
#endif

/***************************************************************
 * SBC	A,n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define SBC(value)												\
{																\
	UINT32 ah = (zAF & 0xff00), c = (zAF & 1);					\
	UINT32 res = (UINT8)((ah >> 8) - value - c);				\
	zF = SZHVC_sub[(c<<16) | ah | res]; 						\
	zA = res;													\
}
#else
#define SBC(value)												\
{																\
	unsigned val = value;										\
	unsigned res = zA - val - (zF & CF);						\
	zF = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((zA ^ res ^ val) & HF) |								\
		(((val ^ zA) & (zA ^ res) & 0x80) >> 5);				\
	zA = res;													\
}
#endif

/***************************************************************
 * NEG
 ***************************************************************/
#define NEG {													\
	UINT8 value = zA;											\
	zA = 0; 													\
	SUB(value); 												\
}

/***************************************************************
 * DAA
 ***************************************************************/
#ifdef LARGE_DAA_TABLE
#define DAA {													\
	int idx = zA;												\
	if( zF & CF ) idx |= 0x100; 								\
	if( zF & HF ) idx |= 0x200; 								\
	if( zF & NF ) idx |= 0x400; 								\
	zAF = DAATable[idx];										\
}
#else
#define DAA {													\
	UINT8 cf, nf, hf, lo, hi, diff; 							\
	cf = zF & CF;												\
	nf = zF & NF;												\
	hf = zF & HF;												\
	lo = (zA & 0x0f);	/* mod 15 */							\
	hi = (zA>>4);		/* div 16 */							\
																\
	if(cf)	{	diff = ((lo <= 9) && (!hf)) ? 0x60 : 0x66;	}	\
	else														\
	{															\
		if(lo >= 10){	diff = (hi <= 8) ? 0x06 : 0x66; }		\
		else													\
		{														\
			if(hi >= 10){	diff = hf ? 0x66 : 0x60;	}		\
			else		{	diff = hf ? 0x06 : 0x00;	}		\
		}														\
	}															\
	if(nf) zA -= diff;											\
	else zA += diff;											\
																\
	zF = SZP[zA] | (zF & NF);									\
	if(cf || ((lo <= 9) ? (hi >= 10) : (hi >= 9))) zF |= CF;	\
	if(nf ? (hf && (lo <= 5)) : (lo >= 10)) zF |= HF;			\
}
#endif

/***************************************************************
 * AND	n
 ***************************************************************/
#define AND(value)												\
	zA &= value;												\
	zF = SZP[zA] | HF

/***************************************************************
 * OR	n
 ***************************************************************/
#define OR(value)												\
	zA |= value;												\
	zF = SZP[zA]

/***************************************************************
 * XOR	n
 ***************************************************************/
#define XOR(value)												\
	zA ^= value;												\
	zF = SZP[zA]

/***************************************************************
 * CP	n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define CP(value)												\
{																\
	unsigned val = value;										\
	UINT32 ah = (zAF & 0xff00); 								\
	UINT32 res = (UINT8)((ah >> 8) - val);						\
	zF = (SZHVC_sub[ah | res] & ~(YF | XF)) | (val & (YF | XF));\
}
#else
#define CP(value)												\
{																\
	unsigned val = value;										\
	unsigned res = zA - val;									\
	zF = (SZ[res & 0xff] & (SF | ZF)) | 						\
		(val & (YF | XF)) | ((res >> 8) & CF) | NF |			\
		((zA ^ res ^ val) & HF) |								\
		((((val ^ zA) & (zA ^ res)) >> 5) & VF);				\
}
#endif



/***************************************************************
 * EX	DE,HL
 ***************************************************************/
#define EX_DE_HL {									\
	unsigned short tmp; 							\
	tmp = zDE; zDE = zHL; zHL = tmp;				\
}

/***************************************************************
 * EXX
 ***************************************************************/
#define EXX {										\
	unsigned short tmp; 							\
	tmp = zBC; zBC = zBC2; zBC2 = tmp;				\
	tmp = zDE; zDE = zDE2; zDE2 = tmp;				\
	tmp = zHL; zHL = zHL2; zHL2 = tmp;				\
}

/***************************************************************
 * EX	(SP),r16
 ***************************************************************/
#define EXSP(DR)									\
{													\
	PAIR tmp = { { 0, 0/*, 0, 0*/ } };					\
	RM16( zSP, &tmp );							\
	WM16( zSP, &Z80.DR );							\
	Z80.DR = tmp;									\
}







/***************************************************************
 * RLC	r8
 ***************************************************************/
static __inline__ UINT8 RLC(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (res >> 7)) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * RRC	r8
 ***************************************************************/
static __inline__ UINT8 RRC(UINT8 value)
{
	unsigned res = value;
	unsigned c = ((res & 0x01) ? CF : 0);
	res = ((res >> 1) | (res << 7)) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * RL	r8
 ***************************************************************/
static __inline__ UINT8 RL(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (zF & CF)) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * RR	r8
 ***************************************************************/
static __inline__ UINT8 RR(UINT8 value)
{
	unsigned res = value;
	unsigned c = ((res & 0x01) ? CF : 0);
	res = ((res >> 1) | (zF << 7)) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLA	r8
 ***************************************************************/
static __inline__ UINT8 SLA(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = (res << 1) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRA	r8
 ***************************************************************/
static __inline__ UINT8 SRA(UINT8 value)
{
	unsigned res = value;
	unsigned c = ((res & 0x01) ? CF : 0);
	res = ((res >> 1) | (res & 0x80)) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLL	r8
 ***************************************************************/
static __inline__ UINT8 SLL(UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | 0x01) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRL	r8
 ***************************************************************/
static __inline__ UINT8 SRL(UINT8 value)
{
	unsigned res = value;
	unsigned c = ((res & 0x01) ? CF : 0);
	res = (res >> 1) & 0xff;
	zF = SZP[res] | c;
	return res;
}

/***************************************************************
 * BIT	bit,r8
 ***************************************************************/
#define BIT(bit,reg)											\
	zF = (zF & CF) | HF | SZ_BIT[reg & (1<<bit)]

//#define BIT3	BIT;	z80_ICount += 3

/***************************************************************
 * BIT	bit,(IX/Y+o)
 ***************************************************************/
#if Z80_EXACT
#define BIT_XY3(bit,reg)										\
	zF = (zF & CF) | HF | (SZ_BIT[reg & (1<<bit)] & ~(YF|XF)) | ((EA>>8) & (YF|XF));\
	z80_ICount += 3
#else
#define BIT_XY3 BIT;	z80_ICount += 3
#endif

/***************************************************************
 * RES	bit,r8
 * SET	bit,r8
 ***************************************************************/
#define RES(bit,value) ((value) & ((~(1<<bit))&0xff))
#define SET(bit,value) ((value) | (  (1<<bit) &0xff))


/***************************************************************
 * LDI
 ***************************************************************/
#if Z80_EXACT
#define LDI {													\
	UINT8 io = RM(zHL); 										\
	WM( zDE, io );												\
	zF &= SF | ZF | CF; 										\
	if( (zA + io) & 0x02 ) zF |= YF; /* bit 1 -> flag 5 */		\
	if( (zA + io) & 0x08 ) zF |= XF; /* bit 3 -> flag 3 */		\
	zHL++; zDE++; zBC--;										\
	if( zBC ) zF |= VF; 										\
}
#else
#define LDI {													\
	WM( zDE, RM(zHL) ); 										\
	zF &= SF | ZF | YF | XF | CF;								\
	zHL++; zDE++; zBC--;										\
	if( zBC ) zF |= VF; 										\
}
#endif

/***************************************************************
 * CPI
 ***************************************************************/
#if Z80_EXACT
#define CPI {													\
	UINT8 val = RM(zHL);										\
	UINT8 res = zA - val;										\
	zHL++; zBC--;												\
	zF = (zF & CF) | (SZ[res]&~(YF|XF)) | ((zA^val^res)&HF) | NF; \
	if( zF & HF ) res -= 1; 									\
	if( res & 0x02 ) zF |= YF; /* bit 1 -> flag 5 */			\
	if( res & 0x08 ) zF |= XF; /* bit 3 -> flag 3 */			\
	if( zBC ) zF |= VF; 										\
}
#else
#define CPI {													\
	UINT8 val = RM(zHL);										\
	UINT8 res = zA - val;										\
	zHL++; zBC--;												\
	zF = (zF & CF) | SZ[res] | ((zA ^ val ^ res) & HF) | NF;	\
	if( zBC ) zF |= VF; 										\
}
#endif

/***************************************************************
 * INI
 ***************************************************************/
#if Z80_EXACT
#if 00
#define INI {													\
	UINT8 io = IN(zBC); 										\
	zB--;														\
	WM( zHL, io );												\
	zHL++;														\
	zF = SZ[zB];												\
	if( io & SF ) zF |= NF; 									\
	if( ( ( (zC + 1) & 0xff) + io) & 0x100 ) zF |= HF | CF; 	\
	if( (irep_tmp1[zC & 3][io & 3] ^							\
		 breg_tmp2[zB] ^										\
		 (zC >> 2) ^											\
		 (io >> 2)) & 1 )										\
		zF |= PF;												\
}
#endif
#define INI {													\
	unsigned t; 												\
	UINT8 io = IN(zBC); 										\
	zB--;														\
	WM( zHL, io );												\
	zHL++;														\
	zF = SZ[zB];												\
	t = (unsigned)((zC + 1) & 0xff) + (unsigned)io; 			\
	if( io & SF ) zF |= NF; 									\
	if( t & 0x100 ) zF |= HF | CF;								\
	zF |= SZP[(UINT8)(t & 0x07) ^ zB] & PF; 					\
}
#else
#define INI {													\
	zB--;														\
	WM( zHL, IN(zBC) ); 										\
	zHL++;														\
	zF = (zB) ? NF : NF | ZF;									\
}
#endif

#if STILL_IO
#undef	INI
#define INI ;
#endif

/***************************************************************
 * OUTI
 ***************************************************************/
#if Z80_EXACT
#if 00
#define OUTI {													\
	UINT8 io = RM(zHL); 										\
	zB--;														\
	OUT( zBC, io ); 											\
	zHL++;														\
	zF = SZ[zB];												\
	if( io & SF ) zF |= NF; 									\
	if( ( ( (zC + 1) & 0xff) + io) & 0x100 ) zF |= HF | CF; 	\
	if( (irep_tmp1[zC & 3][io & 3] ^							\
		 breg_tmp2[zB] ^										\
		 (zC >> 2) ^											\
		 (io >> 2)) & 1 )										\
		zF |= PF;												\
}
#endif
#define OUTI {													\
	unsigned t; 												\
	UINT8 io = RM(zHL); 										\
	zB--;														\
	OUT( zBC, io ); 											\
	zHL++;														\
	zF = SZ[zB];												\
	t = (unsigned)zL + (unsigned)io;							\
	if( io & SF ) zF |= NF; 									\
	if( t & 0x100 ) zF |= HF | CF;								\
	zF |= SZP[(UINT8)(t & 0x07) ^ zB] & PF; 					\
}
#else
#define OUTI {													\
	zB--;														\
	OUT( zBC, RM(zHL) );										\
	zHL++;														\
	zF = (zB) ? NF : NF | ZF;									\
}
#endif

#if STILL_IO
#undef	OUTI
#define OUTI ;
#endif

/***************************************************************
 * LDD
 ***************************************************************/
#if Z80_EXACT
#define LDD {													\
	UINT8 io = RM(zHL); 										\
	WM( zDE, io );												\
	zF &= SF | ZF | CF; 										\
	if( (zA + io) & 0x02 ) zF |= YF; /* bit 1 -> flag 5 */		\
	if( (zA + io) & 0x08 ) zF |= XF; /* bit 3 -> flag 3 */		\
	zHL--; zDE--; zBC--;										\
	if( zBC ) zF |= VF; 										\
}
#else
#define LDD {													\
	WM( zDE, RM(zHL) ); 										\
	zF &= SF | ZF | YF | XF | CF;								\
	zHL--; zDE--; zBC--;										\
	if( zBC ) zF |= VF; 										\
}
#endif

/***************************************************************
 * CPD
 ***************************************************************/
#if Z80_EXACT
#define CPD {													\
	UINT8 val = RM(zHL);										\
	UINT8 res = zA - val;										\
	zHL--; zBC--;												\
	zF = (zF & CF) | (SZ[res]&~(YF|XF)) | ((zA^val^res)&HF) | NF; \
	if( zF & HF ) res -= 1; 									\
	if( res & 0x02 ) zF |= YF; /* bit 1 -> flag 5 */			\
	if( res & 0x08 ) zF |= XF; /* bit 3 -> flag 3 */			\
	if( zBC ) zF |= VF; 										\
}
#else
#define CPD {													\
	UINT8 val = RM(zHL);										\
	UINT8 res = zA - val;										\
	zHL--; zBC--;												\
	zF = (zF & CF) | SZ[res] | ((zA ^ val ^ res) & HF) | NF;	\
	if( zBC ) zF |= VF; 										\
}
#endif

/***************************************************************
 * IND
 ***************************************************************/
#if Z80_EXACT
#if 00
#define IND {													\
	UINT8 io = IN(zBC); 										\
	zB--;														\
	WM( zHL, io );												\
	zHL--;														\
	zF = SZ[zB];												\
	if( io & SF ) zF |= NF; 									\
	if( ( ( (zC - 1) & 0xff) + io) & 0x100 ) zF |= HF | CF; 	\
	if( (drep_tmp1[zC & 3][io & 3] ^							\
		 breg_tmp2[zB] ^										\
		 (zC >> 2) ^											\
		 (io >> 2)) & 1 )										\
		zF |= PF;												\
}
#endif
#define IND {													\
	unsigned t; 												\
	UINT8 io = IN(zBC); 										\
	zB--;														\
	WM( zHL, io );												\
	zHL--;														\
	zF = SZ[zB];												\
	t = ((unsigned)(zC - 1) & 0xff) + (unsigned)io; 			\
	if( io & SF ) zF |= NF; 									\
	if( t & 0x100 ) zF |= HF | CF;								\
	zF |= SZP[(UINT8)(t & 0x07) ^ zB] & PF; 					\
}
#else
#define IND {													\
	zB--;														\
	WM( zHL, IN(zBC) ); 										\
	zHL--;														\
	zF = (zB) ? NF : NF | ZF;									\
}
#endif

#if STILL_IO
#undef	IND
#define IND ;
#endif

/***************************************************************
 * OUTD
 ***************************************************************/
#if Z80_EXACT
#if 00
#define OUTD {													\
	UINT8 io = RM(zHL); 										\
	zB--;														\
	OUT( zBC, io ); 											\
	zHL--;														\
	zF = SZ[zB];												\
	if( io & SF ) zF |= NF; 									\
	if( ( ( (zC - 1) & 0xff) + io) & 0x100 ) zF |= HF | CF; 	\
	if( (drep_tmp1[zC & 3][io & 3] ^							\
		 breg_tmp2[zB] ^										\
		 (zC >> 2) ^											\
		 (io >> 2)) & 1 )										\
		zF |= PF;												\
}
#endif
#define OUTD {													\
	unsigned t; 												\
	UINT8 io = RM(zHL); 										\
	zB--;														\
	OUT( zBC, io ); 											\
	zHL--;														\
	zF = SZ[zB];												\
	t = (unsigned)zL + (unsigned)io;							\
	if( io & SF ) zF |= NF; 									\
	if( t & 0x100 ) zF |= HF | CF;								\
	zF |= SZP[(UINT8)(t & 0x07) ^ zB] & PF; 					\
}
#else
#define OUTD {													\
	zB--;														\
	OUT( zBC, RM(zHL) );										\
	zHL--;														\
	zF = (zB) ? NF : NF | ZF;									\
}
#endif

#if STILL_IO
#undef	OUTD
#define OUTD ;
#endif

/***************************************************************
 * LDIR
 ***************************************************************/
#define LDIR													\
	LDI;														\
	if( zBC )													\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xb0);*/											\
	}

/***************************************************************
 * CPIR
 ***************************************************************/
#define CPIR													\
	CPI;														\
	if( zBC && !(zF & ZF) ) 									\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xb1);*/											\
	}

/***************************************************************
 * INIR
 ***************************************************************/
#define INIR													\
	INI;														\
	if( zB )													\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xb2);*/											\
	}

#if STILL_IO
#undef	INIR
#define INIR ;
#endif

/***************************************************************
 * OTIR
 ***************************************************************/
#define OTIR													\
	OUTI;														\
	if( zB )													\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xb3);*/											\
	}

#if STILL_IO
#undef	OTIR
#define OTIR ;
#endif

/***************************************************************
 * LDDR
 ***************************************************************/
#define LDDR													\
	LDD;														\
	if( zBC )													\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xb8);*/											\
	}

/***************************************************************
 * CPDR
 ***************************************************************/
#define CPDR													\
	CPD;														\
	if( zBC && !(zF & ZF) ) 									\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xb9);*/											\
	}

/***************************************************************
 * INDR
 ***************************************************************/
#define INDR													\
	IND;														\
	if( zB )													\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xba);*/											\
	}

#if STILL_IO
#undef	INDR
#define INDR ;
#endif

/***************************************************************
 * OTDR
 ***************************************************************/
#define OTDR													\
	OUTD;														\
	if( zB )													\
	{															\
		zPC -= 2;												\
		z80_ICount-=5;/*CC(ex,0xbb);*/											\
	}

#if STILL_IO
#undef	OTDR
#define OTDR ;
#endif


/**********************************************************
 * opcodes with CB prefix
 * rotate, shift and bit operations
 **********************************************************/
OP(cb,00) { zB = RLC(zB);										} /* RLC	B		*/
OP(cb,01) { zC = RLC(zC);										} /* RLC	C		*/
OP(cb,02) { zD = RLC(zD);										} /* RLC	D		*/
OP(cb,03) { zE = RLC(zE);										} /* RLC	E		*/
OP(cb,04) { zH = RLC(zH);										} /* RLC	H		*/
OP(cb,05) { zL = RLC(zL);										} /* RLC	L		*/
OP(cb,06) { WM( zHL, RLC(RM(zHL)) );							} /* RLC	(HL)	*/
OP(cb,07) { zA = RLC(zA);										} /* RLC	A		*/

OP(cb,08) { zB = RRC(zB);										} /* RRC	B		*/
OP(cb,09) { zC = RRC(zC);										} /* RRC	C		*/
OP(cb,0a) { zD = RRC(zD);										} /* RRC	D		*/
OP(cb,0b) { zE = RRC(zE);										} /* RRC	E		*/
OP(cb,0c) { zH = RRC(zH);										} /* RRC	H		*/
OP(cb,0d) { zL = RRC(zL);										} /* RRC	L		*/
OP(cb,0e) { WM( zHL, RRC(RM(zHL)) );							} /* RRC	(HL)	*/
OP(cb,0f) { zA = RRC(zA);										} /* RRC	A		*/

OP(cb,10) { zB = RL(zB);										} /* RL 	B		*/
OP(cb,11) { zC = RL(zC);										} /* RL 	C		*/
OP(cb,12) { zD = RL(zD);										} /* RL 	D		*/
OP(cb,13) { zE = RL(zE);										} /* RL 	E		*/
OP(cb,14) { zH = RL(zH);										} /* RL 	H		*/
OP(cb,15) { zL = RL(zL);										} /* RL 	L		*/
OP(cb,16) { WM( zHL, RL(RM(zHL)) ); 							} /* RL 	(HL)	*/
OP(cb,17) { zA = RL(zA);										} /* RL 	A		*/

OP(cb,18) { zB = RR(zB);										} /* RR 	B		*/
OP(cb,19) { zC = RR(zC);										} /* RR 	C		*/
OP(cb,1a) { zD = RR(zD);										} /* RR 	D		*/
OP(cb,1b) { zE = RR(zE);										} /* RR 	E		*/
OP(cb,1c) { zH = RR(zH);										} /* RR 	H		*/
OP(cb,1d) { zL = RR(zL);										} /* RR 	L		*/
OP(cb,1e) { WM( zHL, RR(RM(zHL)) ); 							} /* RR 	(HL)	*/
OP(cb,1f) { zA = RR(zA);										} /* RR 	A		*/

OP(cb,20) { zB = SLA(zB);										} /* SLA	B		*/
OP(cb,21) { zC = SLA(zC);										} /* SLA	C		*/
OP(cb,22) { zD = SLA(zD);										} /* SLA	D		*/
OP(cb,23) { zE = SLA(zE);										} /* SLA	E		*/
OP(cb,24) { zH = SLA(zH);										} /* SLA	H		*/
OP(cb,25) { zL = SLA(zL);										} /* SLA	L		*/
OP(cb,26) { WM( zHL, SLA(RM(zHL)) );							} /* SLA	(HL)	*/
OP(cb,27) { zA = SLA(zA);										} /* SLA	A		*/

OP(cb,28) { zB = SRA(zB);										} /* SRA	B		*/
OP(cb,29) { zC = SRA(zC);										} /* SRA	C		*/
OP(cb,2a) { zD = SRA(zD);										} /* SRA	D		*/
OP(cb,2b) { zE = SRA(zE);										} /* SRA	E		*/
OP(cb,2c) { zH = SRA(zH);										} /* SRA	H		*/
OP(cb,2d) { zL = SRA(zL);										} /* SRA	L		*/
OP(cb,2e) { WM( zHL, SRA(RM(zHL)) );							} /* SRA	(HL)	*/
OP(cb,2f) { zA = SRA(zA);										} /* SRA	A		*/

OP(cb,30) { zB = SLL(zB);										} /* SLL	B		*/
OP(cb,31) { zC = SLL(zC);										} /* SLL	C		*/
OP(cb,32) { zD = SLL(zD);										} /* SLL	D		*/
OP(cb,33) { zE = SLL(zE);										} /* SLL	E		*/
OP(cb,34) { zH = SLL(zH);										} /* SLL	H		*/
OP(cb,35) { zL = SLL(zL);										} /* SLL	L		*/
OP(cb,36) { WM( zHL, SLL(RM(zHL)) );							} /* SLL	(HL)	*/
OP(cb,37) { zA = SLL(zA);										} /* SLL	A		*/

OP(cb,38) { zB = SRL(zB);										} /* SRL	B		*/
OP(cb,39) { zC = SRL(zC);										} /* SRL	C		*/
OP(cb,3a) { zD = SRL(zD);										} /* SRL	D		*/
OP(cb,3b) { zE = SRL(zE);										} /* SRL	E		*/
OP(cb,3c) { zH = SRL(zH);										} /* SRL	H		*/
OP(cb,3d) { zL = SRL(zL);										} /* SRL	L		*/
OP(cb,3e) { WM( zHL, SRL(RM(zHL)) );							} /* SRL	(HL)	*/
OP(cb,3f) { zA = SRL(zA);										} /* SRL	A		*/

OP(cb,40) { BIT(0,zB);											} /* BIT	0,B 	*/
OP(cb,41) { BIT(0,zC);											} /* BIT	0,C 	*/
OP(cb,42) { BIT(0,zD);											} /* BIT	0,D 	*/
OP(cb,43) { BIT(0,zE);											} /* BIT	0,E 	*/
OP(cb,44) { BIT(0,zH);											} /* BIT	0,H 	*/
OP(cb,45) { BIT(0,zL);											} /* BIT	0,L 	*/
OP(cb,46) { BIT(0,RM(zHL)); 	z80_ICount += 3;				} /* BIT	0,(HL)	*/
OP(cb,47) { BIT(0,zA);											} /* BIT	0,A 	*/

OP(cb,48) { BIT(1,zB);											} /* BIT	1,B 	*/
OP(cb,49) { BIT(1,zC);											} /* BIT	1,C 	*/
OP(cb,4a) { BIT(1,zD);											} /* BIT	1,D 	*/
OP(cb,4b) { BIT(1,zE);											} /* BIT	1,E 	*/
OP(cb,4c) { BIT(1,zH);											} /* BIT	1,H 	*/
OP(cb,4d) { BIT(1,zL);											} /* BIT	1,L 	*/
OP(cb,4e) { BIT(1,RM(zHL)); 	z80_ICount += 3;				} /* BIT	1,(HL)	*/
OP(cb,4f) { BIT(1,zA);											} /* BIT	1,A 	*/

OP(cb,50) { BIT(2,zB);											} /* BIT	2,B 	*/
OP(cb,51) { BIT(2,zC);											} /* BIT	2,C 	*/
OP(cb,52) { BIT(2,zD);											} /* BIT	2,D 	*/
OP(cb,53) { BIT(2,zE);											} /* BIT	2,E 	*/
OP(cb,54) { BIT(2,zH);											} /* BIT	2,H 	*/
OP(cb,55) { BIT(2,zL);											} /* BIT	2,L 	*/
OP(cb,56) { BIT(2,RM(zHL)); 	z80_ICount += 3;				} /* BIT	2,(HL)	*/
OP(cb,57) { BIT(2,zA);											} /* BIT	2,A 	*/

OP(cb,58) { BIT(3,zB);											} /* BIT	3,B 	*/
OP(cb,59) { BIT(3,zC);											} /* BIT	3,C 	*/
OP(cb,5a) { BIT(3,zD);											} /* BIT	3,D 	*/
OP(cb,5b) { BIT(3,zE);											} /* BIT	3,E 	*/
OP(cb,5c) { BIT(3,zH);											} /* BIT	3,H 	*/
OP(cb,5d) { BIT(3,zL);											} /* BIT	3,L 	*/
OP(cb,5e) { BIT(3,RM(zHL)); 	z80_ICount += 3;				} /* BIT	3,(HL)	*/
OP(cb,5f) { BIT(3,zA);											} /* BIT	3,A 	*/

OP(cb,60) { BIT(4,zB);											} /* BIT	4,B 	*/
OP(cb,61) { BIT(4,zC);											} /* BIT	4,C 	*/
OP(cb,62) { BIT(4,zD);											} /* BIT	4,D 	*/
OP(cb,63) { BIT(4,zE);											} /* BIT	4,E 	*/
OP(cb,64) { BIT(4,zH);											} /* BIT	4,H 	*/
OP(cb,65) { BIT(4,zL);											} /* BIT	4,L 	*/
OP(cb,66) { BIT(4,RM(zHL)); 	z80_ICount += 3;				} /* BIT	4,(HL)	*/
OP(cb,67) { BIT(4,zA);											} /* BIT	4,A 	*/

OP(cb,68) { BIT(5,zB);											} /* BIT	5,B 	*/
OP(cb,69) { BIT(5,zC);											} /* BIT	5,C 	*/
OP(cb,6a) { BIT(5,zD);											} /* BIT	5,D 	*/
OP(cb,6b) { BIT(5,zE);											} /* BIT	5,E 	*/
OP(cb,6c) { BIT(5,zH);											} /* BIT	5,H 	*/
OP(cb,6d) { BIT(5,zL);											} /* BIT	5,L 	*/
OP(cb,6e) { BIT(5,RM(zHL)); 	z80_ICount += 3;				} /* BIT	5,(HL)	*/
OP(cb,6f) { BIT(5,zA);											} /* BIT	5,A 	*/

OP(cb,70) { BIT(6,zB);											} /* BIT	6,B 	*/
OP(cb,71) { BIT(6,zC);											} /* BIT	6,C 	*/
OP(cb,72) { BIT(6,zD);											} /* BIT	6,D 	*/
OP(cb,73) { BIT(6,zE);											} /* BIT	6,E 	*/
OP(cb,74) { BIT(6,zH);											} /* BIT	6,H 	*/
OP(cb,75) { BIT(6,zL);											} /* BIT	6,L 	*/
OP(cb,76) { BIT(6,RM(zHL)); 	z80_ICount += 3;				} /* BIT	6,(HL)	*/
OP(cb,77) { BIT(6,zA);											} /* BIT	6,A 	*/

OP(cb,78) { BIT(7,zB);											} /* BIT	7,B 	*/
OP(cb,79) { BIT(7,zC);											} /* BIT	7,C 	*/
OP(cb,7a) { BIT(7,zD);											} /* BIT	7,D 	*/
OP(cb,7b) { BIT(7,zE);											} /* BIT	7,E 	*/
OP(cb,7c) { BIT(7,zH);											} /* BIT	7,H 	*/
OP(cb,7d) { BIT(7,zL);											} /* BIT	7,L 	*/
OP(cb,7e) { BIT(7,RM(zHL)); 	z80_ICount += 3;				} /* BIT	7,(HL)	*/
OP(cb,7f) { BIT(7,zA);											} /* BIT	7,A 	*/

OP(cb,80) { zB = RES(0,zB); 									} /* RES	0,B 	*/
OP(cb,81) { zC = RES(0,zC); 									} /* RES	0,C 	*/
OP(cb,82) { zD = RES(0,zD); 									} /* RES	0,D 	*/
OP(cb,83) { zE = RES(0,zE); 									} /* RES	0,E 	*/
OP(cb,84) { zH = RES(0,zH); 									} /* RES	0,H 	*/
OP(cb,85) { zL = RES(0,zL); 									} /* RES	0,L 	*/
OP(cb,86) { WM( zHL, RES(0,RM(zHL)) );							} /* RES	0,(HL)	*/
OP(cb,87) { zA = RES(0,zA); 									} /* RES	0,A 	*/

OP(cb,88) { zB = RES(1,zB); 									} /* RES	1,B 	*/
OP(cb,89) { zC = RES(1,zC); 									} /* RES	1,C 	*/
OP(cb,8a) { zD = RES(1,zD); 									} /* RES	1,D 	*/
OP(cb,8b) { zE = RES(1,zE); 									} /* RES	1,E 	*/
OP(cb,8c) { zH = RES(1,zH); 									} /* RES	1,H 	*/
OP(cb,8d) { zL = RES(1,zL); 									} /* RES	1,L 	*/
OP(cb,8e) { WM( zHL, RES(1,RM(zHL)) );							} /* RES	1,(HL)	*/
OP(cb,8f) { zA = RES(1,zA); 									} /* RES	1,A 	*/

OP(cb,90) { zB = RES(2,zB); 									} /* RES	2,B 	*/
OP(cb,91) { zC = RES(2,zC); 									} /* RES	2,C 	*/
OP(cb,92) { zD = RES(2,zD); 									} /* RES	2,D 	*/
OP(cb,93) { zE = RES(2,zE); 									} /* RES	2,E 	*/
OP(cb,94) { zH = RES(2,zH); 									} /* RES	2,H 	*/
OP(cb,95) { zL = RES(2,zL); 									} /* RES	2,L 	*/
OP(cb,96) { WM( zHL, RES(2,RM(zHL)) );							} /* RES	2,(HL)	*/
OP(cb,97) { zA = RES(2,zA); 									} /* RES	2,A 	*/

OP(cb,98) { zB = RES(3,zB); 									} /* RES	3,B 	*/
OP(cb,99) { zC = RES(3,zC); 									} /* RES	3,C 	*/
OP(cb,9a) { zD = RES(3,zD); 									} /* RES	3,D 	*/
OP(cb,9b) { zE = RES(3,zE); 									} /* RES	3,E 	*/
OP(cb,9c) { zH = RES(3,zH); 									} /* RES	3,H 	*/
OP(cb,9d) { zL = RES(3,zL); 									} /* RES	3,L 	*/
OP(cb,9e) { WM( zHL, RES(3,RM(zHL)) );							} /* RES	3,(HL)	*/
OP(cb,9f) { zA = RES(3,zA); 									} /* RES	3,A 	*/

OP(cb,a0) { zB = RES(4,zB); 									} /* RES	4,B 	*/
OP(cb,a1) { zC = RES(4,zC); 									} /* RES	4,C 	*/
OP(cb,a2) { zD = RES(4,zD); 									} /* RES	4,D 	*/
OP(cb,a3) { zE = RES(4,zE); 									} /* RES	4,E 	*/
OP(cb,a4) { zH = RES(4,zH); 									} /* RES	4,H 	*/
OP(cb,a5) { zL = RES(4,zL); 									} /* RES	4,L 	*/
OP(cb,a6) { WM( zHL, RES(4,RM(zHL)) );							} /* RES	4,(HL)	*/
OP(cb,a7) { zA = RES(4,zA); 									} /* RES	4,A 	*/

OP(cb,a8) { zB = RES(5,zB); 									} /* RES	5,B 	*/
OP(cb,a9) { zC = RES(5,zC); 									} /* RES	5,C 	*/
OP(cb,aa) { zD = RES(5,zD); 									} /* RES	5,D 	*/
OP(cb,ab) { zE = RES(5,zE); 									} /* RES	5,E 	*/
OP(cb,ac) { zH = RES(5,zH); 									} /* RES	5,H 	*/
OP(cb,ad) { zL = RES(5,zL); 									} /* RES	5,L 	*/
OP(cb,ae) { WM( zHL, RES(5,RM(zHL)) );							} /* RES	5,(HL)	*/
OP(cb,af) { zA = RES(5,zA); 									} /* RES	5,A 	*/

OP(cb,b0) { zB = RES(6,zB); 									} /* RES	6,B 	*/
OP(cb,b1) { zC = RES(6,zC); 									} /* RES	6,C 	*/
OP(cb,b2) { zD = RES(6,zD); 									} /* RES	6,D 	*/
OP(cb,b3) { zE = RES(6,zE); 									} /* RES	6,E 	*/
OP(cb,b4) { zH = RES(6,zH); 									} /* RES	6,H 	*/
OP(cb,b5) { zL = RES(6,zL); 									} /* RES	6,L 	*/
OP(cb,b6) { WM( zHL, RES(6,RM(zHL)) );							} /* RES	6,(HL)	*/
OP(cb,b7) { zA = RES(6,zA); 									} /* RES	6,A 	*/

OP(cb,b8) { zB = RES(7,zB); 									} /* RES	7,B 	*/
OP(cb,b9) { zC = RES(7,zC); 									} /* RES	7,C 	*/
OP(cb,ba) { zD = RES(7,zD); 									} /* RES	7,D 	*/
OP(cb,bb) { zE = RES(7,zE); 									} /* RES	7,E 	*/
OP(cb,bc) { zH = RES(7,zH); 									} /* RES	7,H 	*/
OP(cb,bd) { zL = RES(7,zL); 									} /* RES	7,L 	*/
OP(cb,be) { WM( zHL, RES(7,RM(zHL)) );							} /* RES	7,(HL)	*/
OP(cb,bf) { zA = RES(7,zA); 									} /* RES	7,A 	*/

OP(cb,c0) { zB = SET(0,zB); 									} /* SET	0,B 	*/
OP(cb,c1) { zC = SET(0,zC); 									} /* SET	0,C 	*/
OP(cb,c2) { zD = SET(0,zD); 									} /* SET	0,D 	*/
OP(cb,c3) { zE = SET(0,zE); 									} /* SET	0,E 	*/
OP(cb,c4) { zH = SET(0,zH); 									} /* SET	0,H 	*/
OP(cb,c5) { zL = SET(0,zL); 									} /* SET	0,L 	*/
OP(cb,c6) { WM( zHL, SET(0,RM(zHL)) );							} /* SET	0,(HL)	*/
OP(cb,c7) { zA = SET(0,zA); 									} /* SET	0,A 	*/

OP(cb,c8) { zB = SET(1,zB); 									} /* SET	1,B 	*/
OP(cb,c9) { zC = SET(1,zC); 									} /* SET	1,C 	*/
OP(cb,ca) { zD = SET(1,zD); 									} /* SET	1,D 	*/
OP(cb,cb) { zE = SET(1,zE); 									} /* SET	1,E 	*/
OP(cb,cc) { zH = SET(1,zH); 									} /* SET	1,H 	*/
OP(cb,cd) { zL = SET(1,zL); 									} /* SET	1,L 	*/
OP(cb,ce) { WM( zHL, SET(1,RM(zHL)) );							} /* SET	1,(HL)	*/
OP(cb,cf) { zA = SET(1,zA); 									} /* SET	1,A 	*/

OP(cb,d0) { zB = SET(2,zB); 									} /* SET	2,B 	*/
OP(cb,d1) { zC = SET(2,zC); 									} /* SET	2,C 	*/
OP(cb,d2) { zD = SET(2,zD); 									} /* SET	2,D 	*/
OP(cb,d3) { zE = SET(2,zE); 									} /* SET	2,E 	*/
OP(cb,d4) { zH = SET(2,zH); 									} /* SET	2,H 	*/
OP(cb,d5) { zL = SET(2,zL); 									} /* SET	2,L 	*/
OP(cb,d6) { WM( zHL, SET(2,RM(zHL)) );							} /* SET	2,(HL)	*/
OP(cb,d7) { zA = SET(2,zA); 									} /* SET	2,A 	*/

OP(cb,d8) { zB = SET(3,zB); 									} /* SET	3,B 	*/
OP(cb,d9) { zC = SET(3,zC); 									} /* SET	3,C 	*/
OP(cb,da) { zD = SET(3,zD); 									} /* SET	3,D 	*/
OP(cb,db) { zE = SET(3,zE); 									} /* SET	3,E 	*/
OP(cb,dc) { zH = SET(3,zH); 									} /* SET	3,H 	*/
OP(cb,dd) { zL = SET(3,zL); 									} /* SET	3,L 	*/
OP(cb,de) { WM( zHL, SET(3,RM(zHL)) );							} /* SET	3,(HL)	*/
OP(cb,df) { zA = SET(3,zA); 									} /* SET	3,A 	*/

OP(cb,e0) { zB = SET(4,zB); 									} /* SET	4,B 	*/
OP(cb,e1) { zC = SET(4,zC); 									} /* SET	4,C 	*/
OP(cb,e2) { zD = SET(4,zD); 									} /* SET	4,D 	*/
OP(cb,e3) { zE = SET(4,zE); 									} /* SET	4,E 	*/
OP(cb,e4) { zH = SET(4,zH); 									} /* SET	4,H 	*/
OP(cb,e5) { zL = SET(4,zL); 									} /* SET	4,L 	*/
OP(cb,e6) { WM( zHL, SET(4,RM(zHL)) );							} /* SET	4,(HL)	*/
OP(cb,e7) { zA = SET(4,zA); 									} /* SET	4,A 	*/

OP(cb,e8) { zB = SET(5,zB); 									} /* SET	5,B 	*/
OP(cb,e9) { zC = SET(5,zC); 									} /* SET	5,C 	*/
OP(cb,ea) { zD = SET(5,zD); 									} /* SET	5,D 	*/
OP(cb,eb) { zE = SET(5,zE); 									} /* SET	5,E 	*/
OP(cb,ec) { zH = SET(5,zH); 									} /* SET	5,H 	*/
OP(cb,ed) { zL = SET(5,zL); 									} /* SET	5,L 	*/
OP(cb,ee) { WM( zHL, SET(5,RM(zHL)) );							} /* SET	5,(HL)	*/
OP(cb,ef) { zA = SET(5,zA); 									} /* SET	5,A 	*/

OP(cb,f0) { zB = SET(6,zB); 									} /* SET	6,B 	*/
OP(cb,f1) { zC = SET(6,zC); 									} /* SET	6,C 	*/
OP(cb,f2) { zD = SET(6,zD); 									} /* SET	6,D 	*/
OP(cb,f3) { zE = SET(6,zE); 									} /* SET	6,E 	*/
OP(cb,f4) { zH = SET(6,zH); 									} /* SET	6,H 	*/
OP(cb,f5) { zL = SET(6,zL); 									} /* SET	6,L 	*/
OP(cb,f6) { WM( zHL, SET(6,RM(zHL)) );							} /* SET	6,(HL)	*/
OP(cb,f7) { zA = SET(6,zA); 									} /* SET	6,A 	*/

OP(cb,f8) { zB = SET(7,zB); 									} /* SET	7,B 	*/
OP(cb,f9) { zC = SET(7,zC); 									} /* SET	7,C 	*/
OP(cb,fa) { zD = SET(7,zD); 									} /* SET	7,D 	*/
OP(cb,fb) { zE = SET(7,zE); 									} /* SET	7,E 	*/
OP(cb,fc) { zH = SET(7,zH); 									} /* SET	7,H 	*/
OP(cb,fd) { zL = SET(7,zL); 									} /* SET	7,L 	*/
OP(cb,fe) { WM( zHL, SET(7,RM(zHL)) );							} /* SET	7,(HL)	*/
OP(cb,ff) { zA = SET(7,zA); 									} /* SET	7,A 	*/


/**********************************************************
* opcodes with DD/FD CB prefix
* rotate, shift and bit operations with (IX+o)
**********************************************************/
//static void HandleDDCB (){}
OP(ddcb,00) { zB = RLC( RM(EA) ); WM( EA,zB );					} /* RLC	B=(XY+o)	*/
OP(ddcb,01) { zC = RLC( RM(EA) ); WM( EA,zC );					} /* RLC	C=(XY+o)	*/
OP(ddcb,02) { zD = RLC( RM(EA) ); WM( EA,zD );					} /* RLC	D=(XY+o)	*/
OP(ddcb,03) { zE = RLC( RM(EA) ); WM( EA,zE );					} /* RLC	E=(XY+o)	*/
OP(ddcb,04) { zH = RLC( RM(EA) ); WM( EA,zH );					} /* RLC	H=(XY+o)	*/
OP(ddcb,05) { zL = RLC( RM(EA) ); WM( EA,zL );					} /* RLC	L=(XY+o)	*/
OP(ddcb,06) { WM( EA, RLC( RM(EA) ) );							} /* RLC	  (XY+o)	*/
OP(ddcb,07) { zA = RLC( RM(EA) ); WM( EA,zA );					} /* RLC	A=(XY+o)	*/

OP(ddcb,08) { zB = RRC( RM(EA) ); WM( EA,zB );					} /* RRC	B=(XY+o)	*/
OP(ddcb,09) { zC = RRC( RM(EA) ); WM( EA,zC );					} /* RRC	C=(XY+o)	*/
OP(ddcb,0a) { zD = RRC( RM(EA) ); WM( EA,zD );					} /* RRC	D=(XY+o)	*/
OP(ddcb,0b) { zE = RRC( RM(EA) ); WM( EA,zE );					} /* RRC	E=(XY+o)	*/
OP(ddcb,0c) { zH = RRC( RM(EA) ); WM( EA,zH );					} /* RRC	H=(XY+o)	*/
OP(ddcb,0d) { zL = RRC( RM(EA) ); WM( EA,zL );					} /* RRC	L=(XY+o)	*/
OP(ddcb,0e) { WM( EA,RRC( RM(EA) ) );							} /* RRC	  (XY+o)	*/
OP(ddcb,0f) { zA = RRC( RM(EA) ); WM( EA,zA );					} /* RRC	A=(XY+o)	*/

OP(ddcb,10) { zB = RL( RM(EA) ); WM( EA,zB );					} /* RL 	B=(XY+o)	*/
OP(ddcb,11) { zC = RL( RM(EA) ); WM( EA,zC );					} /* RL 	C=(XY+o)	*/
OP(ddcb,12) { zD = RL( RM(EA) ); WM( EA,zD );					} /* RL 	D=(XY+o)	*/
OP(ddcb,13) { zE = RL( RM(EA) ); WM( EA,zE );					} /* RL 	E=(XY+o)	*/
OP(ddcb,14) { zH = RL( RM(EA) ); WM( EA,zH );					} /* RL 	H=(XY+o)	*/
OP(ddcb,15) { zL = RL( RM(EA) ); WM( EA,zL );					} /* RL 	L=(XY+o)	*/
OP(ddcb,16) { WM( EA,RL( RM(EA) ) );							} /* RL 	  (XY+o)	*/
OP(ddcb,17) { zA = RL( RM(EA) ); WM( EA,zA );					} /* RL 	A=(XY+o)	*/

OP(ddcb,18) { zB = RR( RM(EA) ); WM( EA,zB );					} /* RR 	B=(XY+o)	*/
OP(ddcb,19) { zC = RR( RM(EA) ); WM( EA,zC );					} /* RR 	C=(XY+o)	*/
OP(ddcb,1a) { zD = RR( RM(EA) ); WM( EA,zD );					} /* RR 	D=(XY+o)	*/
OP(ddcb,1b) { zE = RR( RM(EA) ); WM( EA,zE );					} /* RR 	E=(XY+o)	*/
OP(ddcb,1c) { zH = RR( RM(EA) ); WM( EA,zH );					} /* RR 	H=(XY+o)	*/
OP(ddcb,1d) { zL = RR( RM(EA) ); WM( EA,zL );					} /* RR 	L=(XY+o)	*/
OP(ddcb,1e) { WM( EA,RR( RM(EA) ) );							} /* RR 	  (XY+o)	*/
OP(ddcb,1f) { zA = RR( RM(EA) ); WM( EA,zA );					} /* RR 	A=(XY+o)	*/

OP(ddcb,20) { zB = SLA( RM(EA) ); WM( EA,zB );					} /* SLA	B=(XY+o)	*/
OP(ddcb,21) { zC = SLA( RM(EA) ); WM( EA,zC );					} /* SLA	C=(XY+o)	*/
OP(ddcb,22) { zD = SLA( RM(EA) ); WM( EA,zD );					} /* SLA	D=(XY+o)	*/
OP(ddcb,23) { zE = SLA( RM(EA) ); WM( EA,zE );					} /* SLA	E=(XY+o)	*/
OP(ddcb,24) { zH = SLA( RM(EA) ); WM( EA,zH );					} /* SLA	H=(XY+o)	*/
OP(ddcb,25) { zL = SLA( RM(EA) ); WM( EA,zL );					} /* SLA	L=(XY+o)	*/
OP(ddcb,26) { WM( EA,SLA( RM(EA) ) );							} /* SLA	  (XY+o)	*/
OP(ddcb,27) { zA = SLA( RM(EA) ); WM( EA,zA );					} /* SLA	A=(XY+o)	*/

OP(ddcb,28) { zB = SRA( RM(EA) ); WM( EA,zB );					} /* SRA	B=(XY+o)	*/
OP(ddcb,29) { zC = SRA( RM(EA) ); WM( EA,zC );					} /* SRA	C=(XY+o)	*/
OP(ddcb,2a) { zD = SRA( RM(EA) ); WM( EA,zD );					} /* SRA	D=(XY+o)	*/
OP(ddcb,2b) { zE = SRA( RM(EA) ); WM( EA,zE );					} /* SRA	E=(XY+o)	*/
OP(ddcb,2c) { zH = SRA( RM(EA) ); WM( EA,zH );					} /* SRA	H=(XY+o)	*/
OP(ddcb,2d) { zL = SRA( RM(EA) ); WM( EA,zL );					} /* SRA	L=(XY+o)	*/
OP(ddcb,2e) { WM( EA,SRA( RM(EA) ) );							} /* SRA	  (XY+o)	*/
OP(ddcb,2f) { zA = SRA( RM(EA) ); WM( EA,zA );					} /* SRA	A=(XY+o)	*/

OP(ddcb,30) { zB = SLL( RM(EA) ); WM( EA,zB );					} /* SLL	B=(XY+o)	*/
OP(ddcb,31) { zC = SLL( RM(EA) ); WM( EA,zC );					} /* SLL	C=(XY+o)	*/
OP(ddcb,32) { zD = SLL( RM(EA) ); WM( EA,zD );					} /* SLL	D=(XY+o)	*/
OP(ddcb,33) { zE = SLL( RM(EA) ); WM( EA,zE );					} /* SLL	E=(XY+o)	*/
OP(ddcb,34) { zH = SLL( RM(EA) ); WM( EA,zH );					} /* SLL	H=(XY+o)	*/
OP(ddcb,35) { zL = SLL( RM(EA) ); WM( EA,zL );					} /* SLL	L=(XY+o)	*/
OP(ddcb,36) { WM( EA,SLL( RM(EA) ) );							} /* SLL	  (XY+o)	*/
OP(ddcb,37) { zA = SLL( RM(EA) ); WM( EA,zA );					} /* SLL	A=(XY+o)	*/

OP(ddcb,38) { zB = SRL( RM(EA) ); WM( EA,zB );					} /* SRL	B=(XY+o)	*/
OP(ddcb,39) { zC = SRL( RM(EA) ); WM( EA,zC );					} /* SRL	C=(XY+o)	*/
OP(ddcb,3a) { zD = SRL( RM(EA) ); WM( EA,zD );					} /* SRL	D=(XY+o)	*/
OP(ddcb,3b) { zE = SRL( RM(EA) ); WM( EA,zE );					} /* SRL	E=(XY+o)	*/
OP(ddcb,3c) { zH = SRL( RM(EA) ); WM( EA,zH );					} /* SRL	H=(XY+o)	*/
OP(ddcb,3d) { zL = SRL( RM(EA) ); WM( EA,zL );					} /* SRL	L=(XY+o)	*/
OP(ddcb,3e) { WM( EA,SRL( RM(EA) ) );							} /* SRL	  (XY+o)	*/
OP(ddcb,3f) { zA = SRL( RM(EA) ); WM( EA,zA );					} /* SRL	A=(XY+o)	*/

//OP(ddcb,40) { ddcb_46();										} /* BIT	0,B=(XY+o)	*/
//OP(ddcb,41) { ddcb_46();										} /* BIT	0,C=(XY+o)	*/
//OP(ddcb,42) { ddcb_46();										} /* BIT	0,D=(XY+o)	*/
//OP(ddcb,43) { ddcb_46();										} /* BIT	0,E=(XY+o)	*/
//OP(ddcb,44) { ddcb_46();										} /* BIT	0,H=(XY+o)	*/
//OP(ddcb,45) { ddcb_46();										} /* BIT	0,L=(XY+o)	*/
OP(ddcb,46) { BIT_XY3(0,RM(EA));								} /* BIT	0,	(XY+o)	*/
//OP(ddcb,47) { ddcb_46();										} /* BIT	0,A=(XY+o)	*/

//OP(ddcb,48) { ddcb_4e();										} /* BIT	1,B=(XY+o)	*/
//OP(ddcb,49) { ddcb_4e();										} /* BIT	1,C=(XY+o)	*/
//OP(ddcb,4a) { ddcb_4e();										} /* BIT	1,D=(XY+o)	*/
//OP(ddcb,4b) { ddcb_4e();										} /* BIT	1,E=(XY+o)	*/
//OP(ddcb,4c) { ddcb_4e();										} /* BIT	1,H=(XY+o)	*/
//OP(ddcb,4d) { ddcb_4e();										} /* BIT	1,L=(XY+o)	*/
OP(ddcb,4e) { BIT_XY3(1,RM(EA));								} /* BIT	1,	(XY+o)	*/
//OP(ddcb,4f) { ddcb_4e();										} /* BIT	1,A=(XY+o)	*/

//OP(ddcb,50) { ddcb_56();										} /* BIT	2,B=(XY+o)	*/
//OP(ddcb,51) { ddcb_56();										} /* BIT	2,C=(XY+o)	*/
//OP(ddcb,52) { ddcb_56();										} /* BIT	2,D=(XY+o)	*/
//OP(ddcb,53) { ddcb_56();										} /* BIT	2,E=(XY+o)	*/
//OP(ddcb,54) { ddcb_56();										} /* BIT	2,H=(XY+o)	*/
//OP(ddcb,55) { ddcb_56();										} /* BIT	2,L=(XY+o)	*/
OP(ddcb,56) { BIT_XY3(2,RM(EA));								} /* BIT	2,	(XY+o)	*/
//OP(ddcb,57) { ddcb_56();										} /* BIT	2,A=(XY+o)	*/

//OP(ddcb,58) { ddcb_5e();										} /* BIT	3,B=(XY+o)	*/
//OP(ddcb,59) { ddcb_5e();										} /* BIT	3,C=(XY+o)	*/
//OP(ddcb,5a) { ddcb_5e();										} /* BIT	3,D=(XY+o)	*/
//OP(ddcb,5b) { ddcb_5e();										} /* BIT	3,E=(XY+o)	*/
//OP(ddcb,5c) { ddcb_5e();										} /* BIT	3,H=(XY+o)	*/
//OP(ddcb,5d) { ddcb_5e();										} /* BIT	3,L=(XY+o)	*/
OP(ddcb,5e) { BIT_XY3(3,RM(EA));								} /* BIT	3,	(XY+o)	*/
//OP(ddcb,5f) { ddcb_5e();										} /* BIT	3,A=(XY+o)	*/

//OP(ddcb,60) { ddcb_66();										} /* BIT	4,B=(XY+o)	*/
//OP(ddcb,61) { ddcb_66();										} /* BIT	4,C=(XY+o)	*/
//OP(ddcb,62) { ddcb_66();										} /* BIT	4,D=(XY+o)	*/
//OP(ddcb,63) { ddcb_66();										} /* BIT	4,E=(XY+o)	*/
//OP(ddcb,64) { ddcb_66();										} /* BIT	4,H=(XY+o)	*/
//OP(ddcb,65) { ddcb_66();										} /* BIT	4,L=(XY+o)	*/
OP(ddcb,66) { BIT_XY3(4,RM(EA));								} /* BIT	4,	(XY+o)	*/
//OP(ddcb,67) { ddcb_66();										} /* BIT	4,A=(XY+o)	*/

//OP(ddcb,68) { ddcb_6e();										} /* BIT	5,B=(XY+o)	*/
//OP(ddcb,69) { ddcb_6e();										} /* BIT	5,C=(XY+o)	*/
//OP(ddcb,6a) { ddcb_6e();										} /* BIT	5,D=(XY+o)	*/
//OP(ddcb,6b) { ddcb_6e();										} /* BIT	5,E=(XY+o)	*/
//OP(ddcb,6c) { ddcb_6e();										} /* BIT	5,H=(XY+o)	*/
//OP(ddcb,6d) { ddcb_6e();										} /* BIT	5,L=(XY+o)	*/
OP(ddcb,6e) { BIT_XY3(5,RM(EA));								} /* BIT	5,	(XY+o)	*/
//OP(ddcb,6f) { ddcb_6e();										} /* BIT	5,A=(XY+o)	*/

//OP(ddcb,70) { ddcb_76();										} /* BIT	6,B=(XY+o)	*/
//OP(ddcb,71) { ddcb_76();										} /* BIT	6,C=(XY+o)	*/
//OP(ddcb,72) { ddcb_76();										} /* BIT	6,D=(XY+o)	*/
//OP(ddcb,73) { ddcb_76();										} /* BIT	6,E=(XY+o)	*/
//OP(ddcb,74) { ddcb_76();										} /* BIT	6,H=(XY+o)	*/
//OP(ddcb,75) { ddcb_76();										} /* BIT	6,L=(XY+o)	*/
OP(ddcb,76) { BIT_XY3(6,RM(EA));								} /* BIT	6,	(XY+o)	*/
//OP(ddcb,77) { ddcb_76();										} /* BIT	6,A=(XY+o)	*/

//OP(ddcb,78) { ddcb_7e();										} /* BIT	7,B=(XY+o)	*/
//OP(ddcb,79) { ddcb_7e();										} /* BIT	7,C=(XY+o)	*/
//OP(ddcb,7a) { ddcb_7e();										} /* BIT	7,D=(XY+o)	*/
//OP(ddcb,7b) { ddcb_7e();										} /* BIT	7,E=(XY+o)	*/
//OP(ddcb,7c) { ddcb_7e();										} /* BIT	7,H=(XY+o)	*/
//OP(ddcb,7d) { ddcb_7e();										} /* BIT	7,L=(XY+o)	*/
OP(ddcb,7e) { BIT_XY3(7,RM(EA));								} /* BIT	7,	(XY+o)	*/
//OP(ddcb,7f) { ddcb_7e();										} /* BIT	7,A=(XY+o)	*/

OP(ddcb,80) { zB = RES(0, RM(EA) ); WM( EA,zB );				} /* RES	0,B=(XY+o)	*/
OP(ddcb,81) { zC = RES(0, RM(EA) ); WM( EA,zC );				} /* RES	0,C=(XY+o)	*/
OP(ddcb,82) { zD = RES(0, RM(EA) ); WM( EA,zD );				} /* RES	0,D=(XY+o)	*/
OP(ddcb,83) { zE = RES(0, RM(EA) ); WM( EA,zE );				} /* RES	0,E=(XY+o)	*/
OP(ddcb,84) { zH = RES(0, RM(EA) ); WM( EA,zH );				} /* RES	0,H=(XY+o)	*/
OP(ddcb,85) { zL = RES(0, RM(EA) ); WM( EA,zL );				} /* RES	0,L=(XY+o)	*/
OP(ddcb,86) { WM( EA, RES(0,RM(EA)) );							} /* RES	0,	(XY+o)	*/
OP(ddcb,87) { zA = RES(0, RM(EA) ); WM( EA,zA );				} /* RES	0,A=(XY+o)	*/

OP(ddcb,88) { zB = RES(1, RM(EA) ); WM( EA,zB );				} /* RES	1,B=(XY+o)	*/
OP(ddcb,89) { zC = RES(1, RM(EA) ); WM( EA,zC );				} /* RES	1,C=(XY+o)	*/
OP(ddcb,8a) { zD = RES(1, RM(EA) ); WM( EA,zD );				} /* RES	1,D=(XY+o)	*/
OP(ddcb,8b) { zE = RES(1, RM(EA) ); WM( EA,zE );				} /* RES	1,E=(XY+o)	*/
OP(ddcb,8c) { zH = RES(1, RM(EA) ); WM( EA,zH );				} /* RES	1,H=(XY+o)	*/
OP(ddcb,8d) { zL = RES(1, RM(EA) ); WM( EA,zL );				} /* RES	1,L=(XY+o)	*/
OP(ddcb,8e) { WM( EA, RES(1,RM(EA)) );							} /* RES	1,	(XY+o)	*/
OP(ddcb,8f) { zA = RES(1, RM(EA) ); WM( EA,zA );				} /* RES	1,A=(XY+o)	*/

OP(ddcb,90) { zB = RES(2, RM(EA) ); WM( EA,zB );				} /* RES	2,B=(XY+o)	*/
OP(ddcb,91) { zC = RES(2, RM(EA) ); WM( EA,zC );				} /* RES	2,C=(XY+o)	*/
OP(ddcb,92) { zD = RES(2, RM(EA) ); WM( EA,zD );				} /* RES	2,D=(XY+o)	*/
OP(ddcb,93) { zE = RES(2, RM(EA) ); WM( EA,zE );				} /* RES	2,E=(XY+o)	*/
OP(ddcb,94) { zH = RES(2, RM(EA) ); WM( EA,zH );				} /* RES	2,H=(XY+o)	*/
OP(ddcb,95) { zL = RES(2, RM(EA) ); WM( EA,zL );				} /* RES	2,L=(XY+o)	*/
OP(ddcb,96) { WM( EA, RES(2,RM(EA)) );							} /* RES	2,	(XY+o)	*/
OP(ddcb,97) { zA = RES(2, RM(EA) ); WM( EA,zA );				} /* RES	2,A=(XY+o)	*/

OP(ddcb,98) { zB = RES(3, RM(EA) ); WM( EA,zB );				} /* RES	3,B=(XY+o)	*/
OP(ddcb,99) { zC = RES(3, RM(EA) ); WM( EA,zC );				} /* RES	3,C=(XY+o)	*/
OP(ddcb,9a) { zD = RES(3, RM(EA) ); WM( EA,zD );				} /* RES	3,D=(XY+o)	*/
OP(ddcb,9b) { zE = RES(3, RM(EA) ); WM( EA,zE );				} /* RES	3,E=(XY+o)	*/
OP(ddcb,9c) { zH = RES(3, RM(EA) ); WM( EA,zH );				} /* RES	3,H=(XY+o)	*/
OP(ddcb,9d) { zL = RES(3, RM(EA) ); WM( EA,zL );				} /* RES	3,L=(XY+o)	*/
OP(ddcb,9e) { WM( EA, RES(3,RM(EA)) );							} /* RES	3,	(XY+o)	*/
OP(ddcb,9f) { zA = RES(3, RM(EA) ); WM( EA,zA );				} /* RES	3,A=(XY+o)	*/

OP(ddcb,a0) { zB = RES(4, RM(EA) ); WM( EA,zB );				} /* RES	4,B=(XY+o)	*/
OP(ddcb,a1) { zC = RES(4, RM(EA) ); WM( EA,zC );				} /* RES	4,C=(XY+o)	*/
OP(ddcb,a2) { zD = RES(4, RM(EA) ); WM( EA,zD );				} /* RES	4,D=(XY+o)	*/
OP(ddcb,a3) { zE = RES(4, RM(EA) ); WM( EA,zE );				} /* RES	4,E=(XY+o)	*/
OP(ddcb,a4) { zH = RES(4, RM(EA) ); WM( EA,zH );				} /* RES	4,H=(XY+o)	*/
OP(ddcb,a5) { zL = RES(4, RM(EA) ); WM( EA,zL );				} /* RES	4,L=(XY+o)	*/
OP(ddcb,a6) { WM( EA, RES(4,RM(EA)) );							} /* RES	4,	(XY+o)	*/
OP(ddcb,a7) { zA = RES(4, RM(EA) ); WM( EA,zA );				} /* RES	4,A=(XY+o)	*/

OP(ddcb,a8) { zB = RES(5, RM(EA) ); WM( EA,zB );				} /* RES	5,B=(XY+o)	*/
OP(ddcb,a9) { zC = RES(5, RM(EA) ); WM( EA,zC );				} /* RES	5,C=(XY+o)	*/
OP(ddcb,aa) { zD = RES(5, RM(EA) ); WM( EA,zD );				} /* RES	5,D=(XY+o)	*/
OP(ddcb,ab) { zE = RES(5, RM(EA) ); WM( EA,zE );				} /* RES	5,E=(XY+o)	*/
OP(ddcb,ac) { zH = RES(5, RM(EA) ); WM( EA,zH );				} /* RES	5,H=(XY+o)	*/
OP(ddcb,ad) { zL = RES(5, RM(EA) ); WM( EA,zL );				} /* RES	5,L=(XY+o)	*/
OP(ddcb,ae) { WM( EA, RES(5,RM(EA)) );							} /* RES	5,	(XY+o)	*/
OP(ddcb,af) { zA = RES(5, RM(EA) ); WM( EA,zA );				} /* RES	5,A=(XY+o)	*/

OP(ddcb,b0) { zB = RES(6, RM(EA) ); WM( EA,zB );				} /* RES	6,B=(XY+o)	*/
OP(ddcb,b1) { zC = RES(6, RM(EA) ); WM( EA,zC );				} /* RES	6,C=(XY+o)	*/
OP(ddcb,b2) { zD = RES(6, RM(EA) ); WM( EA,zD );				} /* RES	6,D=(XY+o)	*/
OP(ddcb,b3) { zE = RES(6, RM(EA) ); WM( EA,zE );				} /* RES	6,E=(XY+o)	*/
OP(ddcb,b4) { zH = RES(6, RM(EA) ); WM( EA,zH );				} /* RES	6,H=(XY+o)	*/
OP(ddcb,b5) { zL = RES(6, RM(EA) ); WM( EA,zL );				} /* RES	6,L=(XY+o)	*/
OP(ddcb,b6) { WM( EA, RES(6,RM(EA)) );							} /* RES	6,	(XY+o)	*/
OP(ddcb,b7) { zA = RES(6, RM(EA) ); WM( EA,zA );				} /* RES	6,A=(XY+o)	*/

OP(ddcb,b8) { zB = RES(7, RM(EA) ); WM( EA,zB );				} /* RES	7,B=(XY+o)	*/
OP(ddcb,b9) { zC = RES(7, RM(EA) ); WM( EA,zC );				} /* RES	7,C=(XY+o)	*/
OP(ddcb,ba) { zD = RES(7, RM(EA) ); WM( EA,zD );				} /* RES	7,D=(XY+o)	*/
OP(ddcb,bb) { zE = RES(7, RM(EA) ); WM( EA,zE );				} /* RES	7,E=(XY+o)	*/
OP(ddcb,bc) { zH = RES(7, RM(EA) ); WM( EA,zH );				} /* RES	7,H=(XY+o)	*/
OP(ddcb,bd) { zL = RES(7, RM(EA) ); WM( EA,zL );				} /* RES	7,L=(XY+o)	*/
OP(ddcb,be) { WM( EA, RES(7,RM(EA)) );							} /* RES	7,	(XY+o)	*/
OP(ddcb,bf) { zA = RES(7, RM(EA) ); WM( EA,zA );				} /* RES	7,A=(XY+o)	*/

OP(ddcb,c0) { zB = SET(0, RM(EA) ); WM( EA,zB );				} /* SET	0,B=(XY+o)	*/
OP(ddcb,c1) { zC = SET(0, RM(EA) ); WM( EA,zC );				} /* SET	0,C=(XY+o)	*/
OP(ddcb,c2) { zD = SET(0, RM(EA) ); WM( EA,zD );				} /* SET	0,D=(XY+o)	*/
OP(ddcb,c3) { zE = SET(0, RM(EA) ); WM( EA,zE );				} /* SET	0,E=(XY+o)	*/
OP(ddcb,c4) { zH = SET(0, RM(EA) ); WM( EA,zH );				} /* SET	0,H=(XY+o)	*/
OP(ddcb,c5) { zL = SET(0, RM(EA) ); WM( EA,zL );				} /* SET	0,L=(XY+o)	*/
OP(ddcb,c6) { WM( EA, SET(0,RM(EA)) );							} /* SET	0,	(XY+o)	*/
OP(ddcb,c7) { zA = SET(0, RM(EA) ); WM( EA,zA );				} /* SET	0,A=(XY+o)	*/

OP(ddcb,c8) { zB = SET(1, RM(EA) ); WM( EA,zB );				} /* SET	1,B=(XY+o)	*/
OP(ddcb,c9) { zC = SET(1, RM(EA) ); WM( EA,zC );				} /* SET	1,C=(XY+o)	*/
OP(ddcb,ca) { zD = SET(1, RM(EA) ); WM( EA,zD );				} /* SET	1,D=(XY+o)	*/
OP(ddcb,cb) { zE = SET(1, RM(EA) ); WM( EA,zE );				} /* SET	1,E=(XY+o)	*/
OP(ddcb,cc) { zH = SET(1, RM(EA) ); WM( EA,zH );				} /* SET	1,H=(XY+o)	*/
OP(ddcb,cd) { zL = SET(1, RM(EA) ); WM( EA,zL );				} /* SET	1,L=(XY+o)	*/
OP(ddcb,ce) { WM( EA, SET(1,RM(EA)) );							} /* SET	1,	(XY+o)	*/
OP(ddcb,cf) { zA = SET(1, RM(EA) ); WM( EA,zA );				} /* SET	1,A=(XY+o)	*/

OP(ddcb,d0) { zB = SET(2, RM(EA) ); WM( EA,zB );				} /* SET	2,B=(XY+o)	*/
OP(ddcb,d1) { zC = SET(2, RM(EA) ); WM( EA,zC );				} /* SET	2,C=(XY+o)	*/
OP(ddcb,d2) { zD = SET(2, RM(EA) ); WM( EA,zD );				} /* SET	2,D=(XY+o)	*/
OP(ddcb,d3) { zE = SET(2, RM(EA) ); WM( EA,zE );				} /* SET	2,E=(XY+o)	*/
OP(ddcb,d4) { zH = SET(2, RM(EA) ); WM( EA,zH );				} /* SET	2,H=(XY+o)	*/
OP(ddcb,d5) { zL = SET(2, RM(EA) ); WM( EA,zL );				} /* SET	2,L=(XY+o)	*/
OP(ddcb,d6) { WM( EA, SET(2,RM(EA)) );							} /* SET	2,	(XY+o)	*/
OP(ddcb,d7) { zA = SET(2, RM(EA) ); WM( EA,zA );				} /* SET	2,A=(XY+o)	*/

OP(ddcb,d8) { zB = SET(3, RM(EA) ); WM( EA,zB );				} /* SET	3,B=(XY+o)	*/
OP(ddcb,d9) { zC = SET(3, RM(EA) ); WM( EA,zC );				} /* SET	3,C=(XY+o)	*/
OP(ddcb,da) { zD = SET(3, RM(EA) ); WM( EA,zD );				} /* SET	3,D=(XY+o)	*/
OP(ddcb,db) { zE = SET(3, RM(EA) ); WM( EA,zE );				} /* SET	3,E=(XY+o)	*/
OP(ddcb,dc) { zH = SET(3, RM(EA) ); WM( EA,zH );				} /* SET	3,H=(XY+o)	*/
OP(ddcb,dd) { zL = SET(3, RM(EA) ); WM( EA,zL );				} /* SET	3,L=(XY+o)	*/
OP(ddcb,de) { WM( EA, SET(3,RM(EA)) );							} /* SET	3,	(XY+o)	*/
OP(ddcb,df) { zA = SET(3, RM(EA) ); WM( EA,zA );				} /* SET	3,A=(XY+o)	*/

OP(ddcb,e0) { zB = SET(4, RM(EA) ); WM( EA,zB );				} /* SET	4,B=(XY+o)	*/
OP(ddcb,e1) { zC = SET(4, RM(EA) ); WM( EA,zC );				} /* SET	4,C=(XY+o)	*/
OP(ddcb,e2) { zD = SET(4, RM(EA) ); WM( EA,zD );				} /* SET	4,D=(XY+o)	*/
OP(ddcb,e3) { zE = SET(4, RM(EA) ); WM( EA,zE );				} /* SET	4,E=(XY+o)	*/
OP(ddcb,e4) { zH = SET(4, RM(EA) ); WM( EA,zH );				} /* SET	4,H=(XY+o)	*/
OP(ddcb,e5) { zL = SET(4, RM(EA) ); WM( EA,zL );				} /* SET	4,L=(XY+o)	*/
OP(ddcb,e6) { WM( EA, SET(4,RM(EA)) );							} /* SET	4,	(XY+o)	*/
OP(ddcb,e7) { zA = SET(4, RM(EA) ); WM( EA,zA );				} /* SET	4,A=(XY+o)	*/

OP(ddcb,e8) { zB = SET(5, RM(EA) ); WM( EA,zB );				} /* SET	5,B=(XY+o)	*/
OP(ddcb,e9) { zC = SET(5, RM(EA) ); WM( EA,zC );				} /* SET	5,C=(XY+o)	*/
OP(ddcb,ea) { zD = SET(5, RM(EA) ); WM( EA,zD );				} /* SET	5,D=(XY+o)	*/
OP(ddcb,eb) { zE = SET(5, RM(EA) ); WM( EA,zE );				} /* SET	5,E=(XY+o)	*/
OP(ddcb,ec) { zH = SET(5, RM(EA) ); WM( EA,zH );				} /* SET	5,H=(XY+o)	*/
OP(ddcb,ed) { zL = SET(5, RM(EA) ); WM( EA,zL );				} /* SET	5,L=(XY+o)	*/
OP(ddcb,ee) { WM( EA, SET(5,RM(EA)) );							} /* SET	5,	(XY+o)	*/
OP(ddcb,ef) { zA = SET(5, RM(EA) ); WM( EA,zA );				} /* SET	5,A=(XY+o)	*/

OP(ddcb,f0) { zB = SET(6, RM(EA) ); WM( EA,zB );				} /* SET	6,B=(XY+o)	*/
OP(ddcb,f1) { zC = SET(6, RM(EA) ); WM( EA,zC );				} /* SET	6,C=(XY+o)	*/
OP(ddcb,f2) { zD = SET(6, RM(EA) ); WM( EA,zD );				} /* SET	6,D=(XY+o)	*/
OP(ddcb,f3) { zE = SET(6, RM(EA) ); WM( EA,zE );				} /* SET	6,E=(XY+o)	*/
OP(ddcb,f4) { zH = SET(6, RM(EA) ); WM( EA,zH );				} /* SET	6,H=(XY+o)	*/
OP(ddcb,f5) { zL = SET(6, RM(EA) ); WM( EA,zL );				} /* SET	6,L=(XY+o)	*/
OP(ddcb,f6) { WM( EA, SET(6,RM(EA)) );							} /* SET	6,	(XY+o)	*/
OP(ddcb,f7) { zA = SET(6, RM(EA) ); WM( EA,zA );				} /* SET	6,A=(XY+o)	*/

OP(ddcb,f8) { zB = SET(7, RM(EA) ); WM( EA,zB );				} /* SET	7,B=(XY+o)	*/
OP(ddcb,f9) { zC = SET(7, RM(EA) ); WM( EA,zC );				} /* SET	7,C=(XY+o)	*/
OP(ddcb,fa) { zD = SET(7, RM(EA) ); WM( EA,zD );				} /* SET	7,D=(XY+o)	*/
OP(ddcb,fb) { zE = SET(7, RM(EA) ); WM( EA,zE );				} /* SET	7,E=(XY+o)	*/
OP(ddcb,fc) { zH = SET(7, RM(EA) ); WM( EA,zH );				} /* SET	7,H=(XY+o)	*/
OP(ddcb,fd) { zL = SET(7, RM(EA) ); WM( EA,zL );				} /* SET	7,L=(XY+o)	*/
OP(ddcb,fe) { WM( EA, SET(7,RM(EA)) );							} /* SET	7,	(XY+o)	*/
OP(ddcb,ff) { zA = SET(7, RM(EA) ); WM( EA,zA );				} /* SET	7,A=(XY+o)	*/

/**********************************************************
 * IX register related opcodes (DD prefix)
 **********************************************************/
/**********************************************************
 * IY register related opcodes (FD prefix)
 **********************************************************/

/////
/***************************************************************
 * ADD16
 ***************************************************************/
static void ADD16IX(UINT16 my_SR)
{
	UINT32 res;
	UINT16 rrr;
	res = (UINT32)zIX + (UINT32)my_SR;
	rrr = (UINT16)res;
	zF = (zF & (SF | ZF | VF)) |
		(((zIX ^ rrr ^ my_SR) >> 8) & HF) |
		((res&(0x00010000) )?1:0) |
		((rrr >> 8) & (YF | XF));
	zIX=rrr;
/*z80_ICount -= (15-4-4);*/\
/*z80_ICount -= (11-4);*/\
  z80_ICount -= ((7));\
}
OP(dd,09) {  /*z80_ICount -= (15-4-4);*/ ADD16IX(zBC);						} /* ADD  IX,BC 	*/
OP(dd,19) {  /*z80_ICount -= (15-4-4);*/ ADD16IX(zDE);						} /* ADD  IX,DE 	*/
OP(dd,29) {  /*z80_ICount -= (15-4-4);*/ ADD16IX(zIX);						} /* ADD  IX,IX 	*/
OP(dd,39) {  /*z80_ICount -= (15-4-4);*/ ADD16IX(zSP);						} /* ADD  IX,SP 	*/


//OP(dd,00) { op_00();								} /* DB   DD		*/
//OP(dd,01) { op_01();								} /* DB   DD		*/
//OP(dd,02) { op_02();								} /* DB   DD		*/
//OP(dd,03) { op_03();								} /* DB   DD		*/
//OP(dd,04) { op_04();								} /* DB   DD		*/
//OP(dd,05) { op_05();								} /* DB   DD		*/
//OP(dd,06) { op_06();								} /* DB   DD		*/
//OP(dd,07) { op_07();								} /* DB   DD		*/

//OP(dd,08) { op_08();								} /* DB   DD		*/
//OP(dd,0a) { op_0a();								} /* DB   DD		*/
//OP(dd,0b) { op_0b();								} /* DB   DD		*/
//OP(dd,0c) { op_0c();								} /* DB   DD		*/
//OP(dd,0d) { op_0d();								} /* DB   DD		*/
//OP(dd,0e) { op_0e();								} /* DB   DD		*/
//OP(dd,0f) { op_0f();								} /* DB   DD		*/

//OP(dd,10) { op_10();								} /* DB   DD		*/
//OP(dd,11) { op_11();								} /* DB   DD		*/
//OP(dd,12) { op_12();								} /* DB   DD		*/
//OP(dd,13) { op_13();								} /* DB   DD		*/
//OP(dd,14) { op_14();								} /* DB   DD		*/
//OP(dd,15) { op_15();								} /* DB   DD		*/
//OP(dd,16) { op_16();								} /* DB   DD		*/
//OP(dd,17) { op_17();								} /* DB   DD		*/

//OP(dd,18) { op_18();								} /* DB   DD		*/
//OP(dd,1a) { op_1a();								} /* DB   DD		*/
//OP(dd,1b) { op_1b();								} /* DB   DD		*/
//OP(dd,1c) { op_1c();								} /* DB   DD		*/
//OP(dd,1d) { op_1d();								} /* DB   DD		*/
//OP(dd,1e) { op_1e();								} /* DB   DD		*/
//OP(dd,1f) { op_1f();								} /* DB   DD		*/

//OP(dd,20) { op_20();								} /* DB   DD		*/
OP(dd,21) { z80_ICount -= (14-4-4); zIX = ARG16(); 					} /* LD   IX,w		*/
OP(dd,22) { z80_ICount -= (20-4-4); EA = ARG16(); WM16( EA, &Z80.ix ); } /* LD   (w),IX	*/
OP(dd,23) { z80_ICount -= (10-4-4); zIX++;								} /* INC  IX		*/
OP(dd,24) { z80_ICount -= (9-4-4);	zIXh = INC(zIXh);					} /* INC  IXh		*/
OP(dd,25) { z80_ICount -= (9-4-4);	zIXh = DEC(zIXh);					} /* DEC  IXh		*/
OP(dd,26) { z80_ICount -= (9-4-4);	zIXh = ARG();						} /* LD   IXh,n 	*/
//OP(dd,27) { op_27();								} /* DB   DD		*/

//OP(dd,28) { op_28();								} /* DB   DD		*/
OP(dd,2a) { z80_ICount -= (20-4-4); EA = ARG16(); RM16( EA, &Z80.ix );	} /* LD   IX,(w)	*/
OP(dd,2b) { z80_ICount -= (10-4-4); zIX--;								} /* DEC  IX		*/
OP(dd,2c) { z80_ICount -= (9-4-4); 	zIXl = INC(zIXl);					} /* INC  IXl		*/
OP(dd,2d) { z80_ICount -= (9-4-4); 	zIXl = DEC(zIXl);					} /* DEC  IXl		*/
OP(dd,2e) { z80_ICount -= (9-4-4); 	zIXl = ARG();						} /* LD   IXl,n 	*/
//OP(dd,2f) { op_2f();								} /* DB   DD		*/

//OP(dd,30) { op_30();								} /* DB   DD		*/
//OP(dd,31) { op_31();								} /* DB   DD		*/
//OP(dd,32) { op_32();								} /* DB   DD		*/
//OP(dd,33) { op_33();								} /* DB   DD		*/
OP(dd,34) { z80_ICount -= (23-4-4); EA = IX8ARG; WM( EA, INC(RM(EA)) ); } /* INC  (IX+o)	*/
OP(dd,35) { z80_ICount -= (23-4-4); EA = IX8ARG; WM( EA, DEC(RM(EA)) ); } /* DEC  (IX+o)	*/
OP(dd,36) { z80_ICount -= (19-4-4); EA = IX8ARG; WM( EA, ARG() );		} /* LD   (IX+o),n	*/
//OP(dd,37) { op_37();								} /* DB   DD		*/

//OP(dd,38) { op_38();								} /* DB   DD		*/
//OP(dd,3a) { op_3a();								} /* DB   DD		*/
//OP(dd,3b) { op_3b();								} /* DB   DD		*/
//OP(dd,3c) { op_3c();								} /* DB   DD		*/
//OP(dd,3d) { op_3d();								} /* DB   DD		*/
//OP(dd,3e) { op_3e();								} /* DB   DD		*/
//OP(dd,3f) { op_3f();								} /* DB   DD		*/

/////
//OP(dd,40) { op_40();								} /* DB   DD		*/
//OP(dd,48) { op_48();								} /* DB   DD		*/
//OP(dd,41) { op_41();								} /* DB   DD		*/
//OP(dd,49) { op_49();								} /* DB   DD		*/
//OP(dd,42) { op_42();								} /* DB   DD		*/
//OP(dd,4a) { op_4a();								} /* DB   DD		*/
//OP(dd,43) { op_43();								} /* DB   DD		*/
//OP(dd,4b) { op_4b();								} /* DB   DD		*/

//OP(dd,50) { op_50();								} /* DB   DD		*/
//OP(dd,51) { op_51();								} /* DB   DD		*/
//OP(dd,52) { op_52();								} /* DB   DD		*/
//OP(dd,53) { op_53();								} /* DB   DD		*/

//OP(dd,58) { op_58();								} /* DB   DD		*/
//OP(dd,59) { op_59();								} /* DB   DD		*/
//OP(dd,5a) { op_5a();								} /* DB   DD		*/
//OP(dd,5b) { op_5b();								} /* DB   DD		*/

//OP(dd,47) { op_47();								} /* DB   DD		*/
//OP(dd,4f) { op_4f();								} /* DB   DD		*/
//OP(dd,57) { op_57();								} /* DB   DD		*/
//OP(dd,5f) { op_5f();								} /* DB   DD		*/

OP(dd,44) { z80_ICount -= (9-4-4);	zB = zIXh;							} /* LD   B,IXh 	*/
OP(dd,4c) { z80_ICount -= (9-4-4);	zC = zIXh;							} /* LD   C,IXh 	*/
OP(dd,54) { z80_ICount -= (9-4-4);	zD = zIXh;							} /* LD   D,IXh 	*/
OP(dd,5c) { z80_ICount -= (9-4-4);	zE = zIXh;							} /* LD   E,IXh 	*/

OP(dd,45) { z80_ICount -= (9-4-4);	zB = zIXl;							} /* LD   B,IXl 	*/
OP(dd,4d) { z80_ICount -= (9-4-4);	zC = zIXl;							} /* LD   C,IXl 	*/
OP(dd,55) { z80_ICount -= (9-4-4);	zD = zIXl;							} /* LD   D,IXl 	*/
OP(dd,5d) { z80_ICount -= (9-4-4);	zE = zIXl;							} /* LD   E,IXl 	*/

OP(dd,46) { z80_ICount -= (19-4-4); zB = RM(IX8ARG);						} /* LD   B,(IX+o)	*/
OP(dd,4e) { z80_ICount -= (19-4-4); zC = RM(IX8ARG);						} /* LD   C,(IX+o)	*/
OP(dd,56) { z80_ICount -= (19-4-4); zD = RM(IX8ARG);						} /* LD   D,(IX+o)	*/
OP(dd,5e) { z80_ICount -= (19-4-4); zE = RM(IX8ARG);						} /* LD   E,(IX+o)	*/


OP(dd,60) { z80_ICount -= (9-4-4); 	zIXh = zB;							} /* LD   IXh,B 	*/
OP(dd,61) { z80_ICount -= (9-4-4); 	zIXh = zC;							} /* LD   IXh,C 	*/
OP(dd,62) { z80_ICount -= (9-4-4); 	zIXh = zD;							} /* LD   IXh,D 	*/
OP(dd,63) { z80_ICount -= (9-4-4); 	zIXh = zE;							} /* LD   IXh,E 	*/
OP(dd,64) { z80_ICount -= (9-4-4);										} /* LD   IXh,IXh	*/
OP(dd,65) { z80_ICount -= (9-4-4); 	zIXh = zIXl; 						} /* LD   IXh,IXl	*/
OP(dd,66) { z80_ICount -= (19-4-4); zH = RM(IX8ARG);						} /* LD   H,(IX+o)	*/
OP(dd,67) { z80_ICount -= (9-4-4); 	zIXh = zA;							} /* LD   IXh,A 	*/

OP(dd,68) { z80_ICount -= (9-4-4); 	zIXl = zB;							} /* LD   IXl,B 	*/
OP(dd,69) { z80_ICount -= (9-4-4); 	zIXl = zC;							} /* LD   IXl,C 	*/
OP(dd,6a) { z80_ICount -= (9-4-4); 	zIXl = zD;							} /* LD   IXl,D 	*/
OP(dd,6b) { z80_ICount -= (9-4-4); 	zIXl = zE;							} /* LD   IXl,E 	*/
OP(dd,6c) { z80_ICount -= (9-4-4); 	zIXl = zIXh; 						} /* LD   IXl,IXh	*/
OP(dd,6d) { z80_ICount -= (9-4-4);										} /* LD   IXl,IXl	*/
OP(dd,6e) { z80_ICount -= (19-4-4); zL = RM(IX8ARG);						} /* LD   L,(IX+o)	*/
OP(dd,6f) { z80_ICount -= (9-4-4); 	zIXl = zA;							} /* LD   IXl,A 	*/

OP(dd,70) { z80_ICount -= (19-4-4); WM( IX8ARG, zB );						} /* LD   (IX+o),B	*/
OP(dd,71) { z80_ICount -= (19-4-4); WM( IX8ARG, zC );						} /* LD   (IX+o),C	*/
OP(dd,72) { z80_ICount -= (19-4-4); WM( IX8ARG, zD );						} /* LD   (IX+o),D	*/
OP(dd,73) { z80_ICount -= (19-4-4); WM( IX8ARG, zE );						} /* LD   (IX+o),E	*/
OP(dd,74) { z80_ICount -= (19-4-4); WM( IX8ARG, zH );						} /* LD   (IX+o),H	*/
OP(dd,75) { z80_ICount -= (19-4-4); WM( IX8ARG, zL );						} /* LD   (IX+o),L	*/
//OP(dd,76) { op_76();														} /* DB   DD		*/ //dd76???
OP(dd,76) { z80_ICount -= (19-4-4); 										} /* LD   (IX+o),(IX+o) */ //dd76???
OP(dd,77) { z80_ICount -= (19-4-4); WM( IX8ARG, zA );						} /* LD   (IX+o),A	*/

//OP(dd,78) { op_78();								} /* DB   DD		*/
//OP(dd,79) { op_79();								} /* DB   DD		*/
//OP(dd,7a) { op_7a();								} /* DB   DD		*/
//OP(dd,7b) { op_7b();								} /* DB   DD		*/
OP(dd,7c) { z80_ICount -= (9-4-4); 	zA = zIXh;								} /* LD   A,IXh 	*/
OP(dd,7d) { z80_ICount -= (9-4-4); 	zA = zIXl;								} /* LD   A,IXl 	*/
OP(dd,7e) { z80_ICount -= (19-4-4); zA = RM(IX8ARG);							} /* LD   A,(IX+o)	*/
//OP(dd,7f) { op_7f();								} /* DB   DD		*/

/////

//OP(dd,80) { op_80();								} /* DB   DD		*/
//OP(dd,81) { op_81();								} /* DB   DD		*/
//OP(dd,82) { op_82();								} /* DB   DD		*/
//OP(dd,83) { op_83();								} /* DB   DD		*/
OP(dd,84) { z80_ICount -= (9-4-4);	ADD(zIXh);								} /* ADD  A,IXh 	*/
OP(dd,85) { z80_ICount -= (9-4-4);	ADD(zIXl);								} /* ADD  A,IXl 	*/
OP(dd,86) { z80_ICount -= (19-4-4); ADD(RM(IX8ARG));							} /* ADD  A,(IX+o)	*/
//OP(dd,87) { op_87();								} /* DB   DD		*/

//OP(dd,88) { op_88();								} /* DB   DD		*/
//OP(dd,89) { op_89();								} /* DB   DD		*/
//OP(dd,8a) { op_8a();								} /* DB   DD		*/
//OP(dd,8b) { op_8b();								} /* DB   DD		*/
OP(dd,8c) { z80_ICount -= (9-4-4);	ADC(zIXh);								} /* ADC  A,IXh 	*/
OP(dd,8d) { z80_ICount -= (9-4-4);	ADC(zIXl);								} /* ADC  A,IXl 	*/
OP(dd,8e) { z80_ICount -= (19-4-4); ADC(RM(IX8ARG));							} /* ADC  A,(IX+o)	*/
//OP(dd,8f) { op_8f();								} /* DB   DD		*/

//OP(dd,90) { op_90();								} /* DB   DD		*/
//OP(dd,91) { op_91();								} /* DB   DD		*/
//OP(dd,92) { op_92();								} /* DB   DD		*/
//OP(dd,93) { op_93();								} /* DB   DD		*/
OP(dd,94) { z80_ICount -= (9-4-4);	SUB(zIXh);								} /* SUB  IXh		*/
OP(dd,95) { z80_ICount -= (9-4-4);	SUB(zIXl);								} /* SUB  IXl		*/
OP(dd,96) { z80_ICount -= (19-4-4); SUB(RM(IX8ARG));							} /* SUB  (IX+o)	*/
//OP(dd,97) { op_97();								} /* DB   DD		*/

//OP(dd,98) { op_98();								} /* DB   DD		*/
//OP(dd,99) { op_99();								} /* DB   DD		*/
//OP(dd,9a) { op_9a();								} /* DB   DD		*/
//OP(dd,9b) { op_9b();								} /* DB   DD		*/
OP(dd,9c) { z80_ICount -= (9-4-4);	SBC(zIXh);								} /* SBC  A,IXh 	*/
OP(dd,9d) { z80_ICount -= (9-4-4);	SBC(zIXl);								} /* SBC  A,IXl 	*/
OP(dd,9e) { z80_ICount -= (19-4-4); SBC(RM(IX8ARG));							} /* SBC  A,(IX+o)	*/
//OP(dd,9f) { op_9f();								} /* DB   DD		*/

//OP(dd,a0) { op_a0();								} /* DB   DD		*/
//OP(dd,a1) { op_a1();								} /* DB   DD		*/
//OP(dd,a2) { op_a2();								} /* DB   DD		*/
//OP(dd,a3) { op_a3();								} /* DB   DD		*/
OP(dd,a4) { z80_ICount -= (9-4-4);	AND(zIXh);								} /* AND  IXh		*/
OP(dd,a5) { z80_ICount -= (9-4-4);	AND(zIXl);								} /* AND  IXl		*/
OP(dd,a6) { z80_ICount -= (19-4-4); AND(RM(IX8ARG));							} /* AND  (IX+o)	*/
//OP(dd,a7) { op_a7();								} /* DB   DD		*/

//OP(dd,a8) { op_a8();								} /* DB   DD		*/
//OP(dd,a9) { op_a9();								} /* DB   DD		*/
//OP(dd,aa) { op_aa();								} /* DB   DD		*/
//OP(dd,ab) { op_ab();								} /* DB   DD		*/
OP(dd,ac) { z80_ICount -= (9-4-4);	XOR(zIXh);								} /* XOR  IXh		*/
OP(dd,ad) { z80_ICount -= (9-4-4);	XOR(zIXl);								} /* XOR  IXl		*/
OP(dd,ae) { z80_ICount -= (19-4-4); XOR(RM(IX8ARG));							} /* XOR  (IX+o)	*/
//OP(dd,af) { op_af();								} /* DB   DD		*/

//OP(dd,b0) { op_b0();								} /* DB   DD		*/
//OP(dd,b1) { op_b1();								} /* DB   DD		*/
//OP(dd,b2) { op_b2();								} /* DB   DD		*/
//OP(dd,b3) { op_b3();								} /* DB   DD		*/
OP(dd,b4) { z80_ICount -= (9-4-4);	OR(zIXh);								} /* OR   IXh		*/
OP(dd,b5) { z80_ICount -= (9-4-4);	OR(zIXl);								} /* OR   IXl		*/
OP(dd,b6) { z80_ICount -= (19-4-4); OR(RM(IX8ARG)); 						} /* OR   (IX+o)	*/
//OP(dd,b7) { op_b7();								} /* DB   DD		*/

//OP(dd,b8) { op_b8();								} /* DB   DD		*/
//OP(dd,b9) { op_b9();								} /* DB   DD		*/
//OP(dd,ba) { op_ba();								} /* DB   DD		*/
//OP(dd,bb) { op_bb();								} /* DB   DD		*/
OP(dd,bc) { z80_ICount -= (9-4-4);	CP(zIXh);								} /* CP   IXh		*/
OP(dd,bd) { z80_ICount -= (9-4-4);	CP(zIXl);								} /* CP   IXl		*/
OP(dd,be) { z80_ICount -= (19-4-4); CP(RM(IX8ARG)); 						} /* CP   (IX+o)	*/
//OP(dd,bf) { op_bf();								} /* DB   DD		*/


////

//OP(dd,c0) { op_c0();								} /* DB   DD		*/
//OP(dd,c1) { op_c1();								} /* DB   DD		*/
//OP(dd,c2) { op_c2();								} /* DB   DD		*/
//OP(dd,c3) { op_c3();								} /* DB   DD		*/
//OP(dd,c4) { op_c4();								} /* DB   DD		*/
//OP(dd,c5) { op_c5();								} /* DB   DD		*/
//OP(dd,c6) { op_c6();								} /* DB   DD		*/
//OP(dd,c7) { op_c7();								} /* DB   DD		*/

//OP(dd,c8) { op_c8();								} /* DB   DD		*/
//OP(dd,c9) { op_c9();								} /* DB   DD		*/
//OP(dd,ca) { op_ca();								} /* DB   DD		*/
OP(dd,cb) {
   EA = IX8ARG;
//EXEC(ddcb,ARG());
{
	unsigned op = ARG();
//	CC(ddcb,op);
	z80_ICount -= (23-4-4);
	(*Z80ddcb[op])();
}


} /* **   DD CB xx	*/
//OP(dd,cc) { op_cc();								} /* DB   DD		*/
//OP(dd,cd) { op_cd();								} /* DB   DD		*/
//OP(dd,ce) { op_ce();								} /* DB   DD		*/
//OP(dd,cf) { op_cf();								} /* DB   DD		*/

//OP(dd,d0) { op_d0();								} /* DB   DD		*/
//OP(dd,d1) { op_d1();								} /* DB   DD		*/
//OP(dd,d2) { op_d2();								} /* DB   DD		*/
//OP(dd,d3) { op_d3();								} /* DB   DD		*/
//OP(dd,d4) { op_d4();								} /* DB   DD		*/
//OP(dd,d5) { op_d5();								} /* DB   DD		*/
//OP(dd,d6) { op_d6();								} /* DB   DD		*/
//OP(dd,d7) { op_d7();								} /* DB   DD		*/

//OP(dd,d8) { op_d8();								} /* DB   DD		*/
//OP(dd,d9) { op_d9();								} /* DB   DD		*/
//OP(dd,da) { op_da();								} /* DB   DD		*/
//OP(dd,db) { op_db();								} /* DB   DD		*/
//OP(dd,dc) { op_dc();								} /* DB   DD		*/
//OP(dd,dd) { op_dd();								} /* DB   DD		*/
//OP(dd,de) { op_de();								} /* DB   DD		*/
//OP(dd,df) { op_df();								} /* DB   DD		*/

//OP(dd,e0) { op_e0();								} /* DB   DD		*/
OP(dd,e1) { z80_ICount -= (14-4-4);  POP(ix);									} /* POP  IX		*/
//OP(dd,e2) { op_e2();								} /* DB   DD		*/
OP(dd,e3) { z80_ICount -= (23-4-4);  EXSP(ix);								} /* EX   (SP),IX	*/
//OP(dd,e4) { op_e4();								} /* DB   DD		*/
OP(dd,e5) { z80_ICount -= (15-4-4);  PUSH( ix );								} /* PUSH IX		*/
//OP(dd,e6) { op_e6();								} /* DB   DD		*/
//OP(dd,e7) { op_e7();								} /* DB   DD		*/

//OP(dd,e8) { op_e8();								} /* DB   DD		*/
OP(dd,e9) { z80_ICount -= (8-4-4);	zPC = zIX;								} /* JP   (IX)		*/
//OP(dd,ea) { op_ea();								} /* DB   DD		*/
//OP(dd,eb) { op_eb();								} /* DB   DD		*/
//OP(dd,ec) { op_ec();								} /* DB   DD		*/
//OP(dd,ed) { op_ed();								} /* DB   DD		*/
//OP(dd,ee) { op_ee();								} /* DB   DD		*/
//OP(dd,ef) { op_ef();								} /* DB   DD		*/

//OP(dd,f0) { op_f0();								} /* DB   DD		*/
//OP(dd,f1) { op_f1();								} /* DB   DD		*/
//OP(dd,f2) { op_f2();								} /* DB   DD		*/
//OP(dd,f3) { op_f3();								} /* DB   DD		*/
//OP(dd,f4) { op_f4();								} /* DB   DD		*/
//OP(dd,f5) { op_f5();								} /* DB   DD		*/
//OP(dd,f6) { op_f6();								} /* DB   DD		*/
//OP(dd,f7) { op_f7();								} /* DB   DD		*/

//OP(dd,f8) { op_f8();								} /* DB   DD		*/
OP(dd,f9) { z80_ICount -= (10-4-4);  zSP = zIX; 							} /* LD   SP,IX 	*/
//OP(dd,fa) { op_fa();								} /* DB   DD		*/
//OP(dd,fb) { op_fb();								} /* DB   DD		*/
//OP(dd,fc) { op_fc();								} /* DB   DD		*/
//OP(dd,fd) { op_fd();								} /* DB   DD		*/
//OP(dd,fe) { op_fe();								} /* DB   DD		*/
//OP(dd,ff) { op_ff();								} /* DB   DD		*/





/**********************************************************
 * special opcodes (ED prefix)
 **********************************************************/
#if 00
static const UINT8 cc_ed[0x100] = {
//4#	//12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9,
//5#	//12,12,15,20, 8, 8, 8, 9,12,12,15,20, 8, 8, 8, 9,
//6#	//12,12,15,20, 8, 8, 8,18,12,12,15,20, 8, 8, 8,18,
//7#	//12,12,15,20, 8, 8, 8, 8,12,12,15,20, 8, 8, 8, 8,

//4#	12,12,15,20, 8,14, 8, 9,
//		12,12,15,20, 8,14, 8, 9,
//5#	12,12,15,20, 8,14, 8, 9,
//		12,12,15,20, 8,14, 8, 9,
//6#	12,12,15,20, 8,14, 8,18,
//		12,12,15,20, 8,14, 8,18,
//7#	12,12,15,20, 8,14, 8, 8,
//		12,12,15,20, 8,14, 8, 8,

#endif

/***************************************************************
 * RETN
 ***************************************************************/
#define RETN	{							\
	POP(pc);								\
	/*if( zIFF1 == 0 && zIFF2 == 1 )*/			\
	if( zIFFS == IFF2_ON )			\
	{										\
	/*	zIFF1 = 1;	*/						\
		zIFFS = (IFF1_ON|IFF2_ON);			\
		if( Z80.irq_state != CLEAR_LINE )	\
		{									\
			take_interrupt();				\
		}									\
	}										\
	else{									\
	/*	zIFF1 = zIFF2;	*/					\
		zIFFS |= ((zIFFS>>2)&1);		\
	}										\
}
/***************************************************************
 * SBC	r16,r16
 ***************************************************************/
#define SBC16(Reg)												\
{																\
	UINT32 zREGd =(UINT32)Z80.Reg.w.l;\
	UINT32 zHLd =(UINT32)zHL;\
	UINT32 res = zHLd - zREGd - (zF & CF);					\
	zF = (((zHLd ^ res ^ zREGd) >> 8) & HF) | NF |			\
		((res >> 16) & CF) |									\
		((res >> 8) & (SF | YF | XF)) | 						\
		((res & 0xffff) ? 0 : ZF) | 							\
		(((zREGd ^ zHLd) & (zHLd ^ res) &0x8000) >> 13);	\
	zHL = (UINT16)res;											\
}
/***************************************************************
 * ADC	r16,r16
 ***************************************************************/
#define ADC16(Reg)												\
{																\
	UINT32 zREGd =(UINT32)Z80.Reg.w.l;\
	UINT32 zHLd =(UINT32)zHL;\
	UINT32 res = zHLd + zREGd + (zF & CF);					\
	zF = (((zHLd ^ res ^ zREGd) >> 8) & HF) |				\
		((res >> 16) & CF) |									\
		((res >> 8) & (SF | YF | XF)) | 						\
		((res & 0xffff) ? 0 : ZF) | 							\
		(((zREGd ^ zHLd ^ 0x8000) & (zREGd ^ res) & 0x8000) >> 13); \
	zHL = (UINT16)res;											\
}
static void HandleED(void){
	unsigned op;
	op=ROP();
//	CC(ed,op);
	switch(op)
	{
#if STILL_IO
	case 0x40: //{ /*zB = IN(zBC); zF = (zF & CF) | SZP[zB]; */ 	}	z80_ICount -= (12-4);break; /* IN	B,(C)	*/
	case 0x48: //{ /*zC = IN(zBC); zF = (zF & CF) | SZP[zC]; */ 	}	z80_ICount -= (12-4);break; /* IN	C,(C)	*/
	case 0x50: //{ /*zD = IN(zBC); zF = (zF & CF) | SZP[zD]; */ 	}	z80_ICount -= (12-4);break; /* IN	D,(C)	*/
	case 0x58: //{ /*zE = IN(zBC); zF = (zF & CF) | SZP[zE]; */ 	}	z80_ICount -= (12-4);break; /* IN	E,(C)	*/
	case 0x60: //{ /*zH = IN(zBC); zF = (zF & CF) | SZP[zH]; */ 	}	z80_ICount -= (12-4);break; /* IN	H,(C)	*/
	case 0x68: //{ /*zL = IN(zBC); zF = (zF & CF) | SZP[zL]; */ 	}	z80_ICount -= (12-4);break; /* IN	L,(C)	*/
	case 0x70: //{ /*UINT8 res = IN(zBC); zF = (zF & CF) | SZP[res]; */}	z80_ICount -= (12-4);break; /* IN	 0,(C)	*/
	case 0x78: { /*zA = IN(zBC); zF = (zF & CF) | SZP[zA]; */		}	z80_ICount -= (12-4);break; /* IN	A,(C)	*/
#else
	case 0x40: { zB = IN(zBC); zF = (zF & CF) | 0x84/*SZP[0xff zB]*/;}	z80_ICount -= (12-4);break; /* IN	 B,(C)	*/
	case 0x48: { zC = IN(zBC); zF = (zF & CF) | 0x84/*SZP[0xff zC]*/;}	z80_ICount -= (12-4);break; /* IN	 C,(C)	*/
	case 0x50: { zD = IN(zBC); zF = (zF & CF) | 0x84/*SZP[0xff zD]*/;}	z80_ICount -= (12-4);break; /* IN	 D,(C)	*/
	case 0x58: { zE = IN(zBC); zF = (zF & CF) | 0x84/*SZP[0xff zE]*/;}	z80_ICount -= (12-4);break; /* IN	 E,(C)	*/
	case 0x60: { zH = IN(zBC); zF = (zF & CF) | 0x84/*SZP[0xff zH]*/;}	z80_ICount -= (12-4);break; /* IN	 H,(C)	*/
	case 0x68: { zL = IN(zBC); zF = (zF & CF) | 0x84/*SZP[0xff zL];*/}	z80_ICount -= (12-4);break; /* IN	 L,(C)	*/
	case 0x70:{/*UINT8 res=IN(zBC);*/zF=(zF&CF)|0x84/*SZP[0xff res];*/} z80_ICount -= (12-4);break; /* IN	0,(C)	*/
	case 0x78: { zA = IN(zBC); zF = (zF & CF) | 0x84/*SZP[0xff zA]*/;}	z80_ICount -= (12-4);break; /* IN	 A,(C)	*/
#endif

	case 0x41: { OUT(zBC, zB);							}	z80_ICount -= (12-4);break; /* OUT	(C),B	*/
	case 0x49: { OUT(zBC, zC);							}	z80_ICount -= (12-4);break; /* OUT	(C),C	*/
	case 0x51: { OUT(zBC, zD);							}	z80_ICount -= (12-4);break; /* OUT	(C),D	*/
	case 0x59: { OUT(zBC, zE);							}	z80_ICount -= (12-4);break; /* OUT	(C),E	*/
	case 0x61: { OUT(zBC, zH);							}	z80_ICount -= (12-4);break; /* OUT	(C),H	*/
	case 0x69: { OUT(zBC, zL);							}	z80_ICount -= (12-4);break; /* OUT	(C),L	*/
	case 0x71: { OUT(zBC, 0);							}	z80_ICount -= (12-4);break; /* OUT	(C),0	*/
	case 0x79: { OUT(zBC, zA);							}	z80_ICount -= (12-4);break; /* OUT	(C),A	*/

	case 0x42: { SBC16( bc );							}	z80_ICount -= (15-4);break; /* SBC	HL,BC	*/
	case 0x52: { SBC16( de );							}	z80_ICount -= (15-4);break; /* SBC	HL,DE	*/
	case 0x62: { SBC16( hl );							}	z80_ICount -= (15-4);break; /* SBC	HL,HL	*/
	case 0x72: { SBC16( sp );							}	z80_ICount -= (15-4);break; /* SBC	HL,SP	*/

	case 0x4a: { ADC16( bc );							}	z80_ICount -= (15-4);break; /* ADC	HL,BC	*/
	case 0x5a: { ADC16( de );							}	z80_ICount -= (15-4);break; /* ADC	HL,DE	*/
	case 0x6a: { ADC16( hl );							}	z80_ICount -= (15-4);break; /* ADC	HL,HL	*/
	case 0x7a: { ADC16( sp );							}	z80_ICount -= (15-4);break; /* ADC	HL,SP	*/

	case 0x43: { EA = ARG16(); WM16( EA, &Z80.bc ); 	}	z80_ICount -= (20-4);break; /* LD	(w),BC	*/
	case 0x53: { EA = ARG16(); WM16( EA, &Z80.de ); 	}	z80_ICount -= (20-4);break; /* LD	(w),DE	*/
	case 0x63: { EA = ARG16(); WM16( EA, &Z80.hl ); 	}	z80_ICount -= (20-4);break; /* LD	(w),HL	*/
	case 0x73: { EA = ARG16(); WM16( EA, &Z80.sp ); 	}	z80_ICount -= (20-4);break; /* LD	(w),SP	*/

	case 0x4b: { EA = ARG16(); RM16( EA, &Z80.bc ); 	}	z80_ICount -= (20-4);break; /* LD	BC,(w)	*/
	case 0x5b: { EA = ARG16(); RM16( EA, &Z80.de ); 	}	z80_ICount -= (20-4);break; /* LD	DE,(w)	*/
	case 0x6b: { EA = ARG16(); RM16( EA, &Z80.hl ); 	}	z80_ICount -= (20-4);break; /* LD	HL,(w)	*/
	case 0x7b: { EA = ARG16(); RM16( EA, &Z80.sp ); 	}	z80_ICount -= (20-4);break; /* LD	SP,(w)	*/

	case 0x44: //{ NEG; 								}	z80_ICount -= (8-4);break; /* NEG			*/
	case 0x4c: //{ NEG; 								}	z80_ICount -= (8-4);break; /* NEG			*/
	case 0x54: //{ NEG; 								}	z80_ICount -= (8-4);break; /* NEG			*/
	case 0x5c: //{ NEG; 								}	z80_ICount -= (8-4);break; /* NEG			*/
	case 0x64: //{ NEG; 								}	z80_ICount -= (8-4);break; /* NEG			*/
	case 0x6c: //{ NEG; 								}	z80_ICount -= (8-4);break; /* NEG			*/
	case 0x74: //{ NEG; 								}	z80_ICount -= (8-4);break; /* NEG			*/
	case 0x7c: { NEG;									}	z80_ICount -= (8-4);break; /* NEG			*/

	case 0x45: //{ RETN;								}	z80_ICount -= (14-4);break; /* RETN;		*/
	case 0x55: //{ RETN;								}	z80_ICount -= (14-4);break; /* RETN;		*/
	case 0x65: //{ RETN;								}	z80_ICount -= (14-4);break; /* RETN;		*/
	case 0x75: { RETN;									}	z80_ICount -= (14-4);break; /* RETN;		*/
	case 0x4d: //{ RETI;								}	z80_ICount -= (14-4);break; /* RETI 		*/
	case 0x5d: //{ RETI;								}	z80_ICount -= (14-4);break; /* RETI 		*/
	case 0x6d: //{ RETI;								}	z80_ICount -= (14-4);break; /* RETI 		*/
	case 0x7d: { RETI;									}	z80_ICount -= (14-4);break; /* RETI 		*/

	case 0x46: //{ zIM = 0; 							}	z80_ICount -= (8-4);break; /* IM	0		*/
	case 0x66: //{ zIM = 0; 							}	z80_ICount -= (8-4);break; /* IM	0		*/
	case 0x4e: //{ zIM = 0; 							}	z80_ICount -= (8-4);break; /* IM	0		*/
	case 0x6e: //{ zIM = 0; 							}	z80_ICount -= (8-4);break; /* IM	0		*/
	case 0x56: //{ zIM = 1; 							}	z80_ICount -= (8-4);break; /* IM	1		*/
	case 0x76: //{ zIM = 1; 							}	z80_ICount -= (8-4);break; /* IM	1		*/
	case 0x5e: //{ zIM = 2; 							}	z80_ICount -= (8-4);break; /* IM	2		*/
	case 0x7e: //{ zIM = 2; 							}	z80_ICount -= (8-4);break; /* IM	2		*/
					zIM = ((op>>3)&0x03);					z80_ICount -= (8-4);break;
	case 0x47: { zI = zA;											}	z80_ICount -= (9-4);break; /* LD	I,A 	*/
	case 0x4f: { LD_R_A;											}	z80_ICount -= (9-4);break; /* LD	R,A 	*/
	case 0x57: { zA = zI; zF = (zF & CF) | SZ[zA] | (zIFFS&IFF2_ON)/*( zIFF2 << 2 )*/; }	z80_ICount -= (9-4);break; /* LD	A,I 	*/
	case 0x5f: { LD_A_R;											}	z80_ICount -= (9-4);break; /* LD	A,R 	*/
	case 0x67: { RRD;												}	z80_ICount -= (18-4);break; /* RRD	(HL)		*/
	case 0x6f: { RLD;												}	z80_ICount -= (18-4);break; /* RLD	(HL)		*/
//	case 0x77: { ;													}	z80_ICount -= (9-4);break; /* DB	ED,77	*/
//	case 0x7f: { ;													}	z80_ICount -= (9-4);break; /* DB	ED,7F	*/




//a#	16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,
//b#	16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,

	case 0xa0: { LDI;	}	z80_ICount -= (16-4);break; /* LDI	*/
	case 0xa1: { CPI;	}	z80_ICount -= (16-4);break; /* CPI	*/

	case 0xa8: { LDD;	}	z80_ICount -= (16-4);break; /* LDD	*/
	case 0xa9: { CPD;	}	z80_ICount -= (16-4);break; /* CPD	*/

	case 0xb0: { LDIR;	}	z80_ICount -= (16-4);break; /* LDIR */
	case 0xb1: { CPIR;	}	z80_ICount -= (16-4);break; /* CPIR */

	case 0xb8: { LDDR;	}	z80_ICount -= (16-4);break; /* LDDR */
	case 0xb9: { CPDR;	}	z80_ICount -= (16-4);break; /* CPDR */

#if 00
/*簡略化*/
	case 0xa2:// { INI; 	}	z80_ICount -= (16-4);break; /* INI	*/
	case 0xa3:// { OUTI;	}	z80_ICount -= (16-4);break; /* OUTI */
	case 0xaa:// { IND; 	}	z80_ICount -= (16-4);break; /* IND	*/
	case 0xab:// { OUTD;	}	z80_ICount -= (16-4);break; /* OUTD */
	case 0xb2:// { INIR;	}	z80_ICount -= (16-4);break; /* INIR */
	case 0xb3:// { OTIR;	}	z80_ICount -= (16-4);break; /* OTIR */
	case 0xba:// { INDR;	}	z80_ICount -= (16-4);break; /* INDR */
	case 0xbb:// { OTDR;	}	z80_ICount -= (16-4);break; /* OTDR */
				z80_ICount -= (16-4);break;
#endif
	default:
		z80_ICount -= (8-4);
		break;
	}

}

#if TIME_LOOP_HACKS

#define CHECK_BC_LOOP											\
if( zBC > 1 && (zPC < 0xfffc) ) {								\
	UINT8 op1 = z80_readmem16(zPC);								\
	UINT8 op2 = z80_readmem16(zPC+1);								\
	if( (op1==0x78 && op2==0xb1) || (op1==0x79 && op2==0xb0) )	\
	{															\
		UINT8 op3 = z80_readmem16(zPC+2);							\
		UINT8 op4 = z80_readmem16(zPC+3);							\
		if( op3==0x20 && op4==0xfb )							\
		{														\
			int cnt =											\
				4/*cc_op[0x78]*/ +									\
				4/*cc_op[0xb1]*/ +									\
				7/*cc_op[0x20]*/ +									\
				5/*cc_ex[0x20]*/;								\
			while( zBC > 0 && z80_ICount > cnt )				\
			{													\
				BURNODD( cnt, 4, cnt ); 						\
				zBC--;											\
			}													\
		}														\
		else													\
		if( op3 == 0xc2 )										\
		{														\
			UINT8 ad1 = z80_readmem16(zPC+3);					\
			UINT8 ad2 = z80_readmem16(zPC+4);					\
			if( (ad1 + 256 * ad2) == (zPC - 1) )				\
			{													\
				int cnt =										\
					4/*cc_op[0x78]*/ +								\
					4/*cc_op[0xb1]*/ +								\
					10/*cc_op[0xc2]*/ + 							\
					0/*cc_ex[0xc2]*/;							\
				while( zBC > 0 && z80_ICount > cnt )			\
				{												\
					BURNODD( cnt, 4, cnt ); 					\
					zBC--;										\
				}												\
			}													\
		}														\
	}															\
}

#define CHECK_DE_LOOP											\
if( zDE > 1 && (zPC < 0xfffc) ) {								\
	UINT8 op1 = z80_readmem16(zPC);								\
	UINT8 op2 = z80_readmem16(zPC+1);								\
	if( (op1==0x7a && op2==0xb3) || (op1==0x7b && op2==0xb2) )	\
	{															\
		UINT8 op3 = z80_readmem16(zPC+2);							\
		UINT8 op4 = z80_readmem16(zPC+3);							\
		if( op3==0x20 && op4==0xfb )							\
		{														\
			int cnt =											\
				4/*cc_op[0x7a]*/ +									\
				4/*cc_op[0xb3]*/ +									\
				7/*cc_op[0x20]*/ +									\
				5/*cc_ex[0x20]*/;								\
			while( zDE > 0 && z80_ICount > cnt )				\
			{													\
				BURNODD( cnt, 4, cnt ); 						\
				zDE--;											\
			}													\
		}														\
		else													\
		if( op3==0xc2 ) 										\
		{														\
			UINT8 ad1 = z80_readmem16(zPC+3);					\
			UINT8 ad2 = z80_readmem16(zPC+4);					\
			if( (ad1 + 256 * ad2) == (zPC - 1) )				\
			{													\
				int cnt =										\
					4/*cc_op[0x7a]*/ +								\
					4/*cc_op[0xb3]*/ +								\
					10/*cc_op[0xc2]*/ + 							\
					0/*cc_ex[0xc2]*/;							\
				while( zDE > 0 && z80_ICount > cnt )			\
				{												\
					BURNODD( cnt, 4, cnt ); 					\
					zDE--;										\
				}												\
			}													\
		}														\
	}															\
}

#define CHECK_HL_LOOP											\
if( zHL > 1 && (zPC < 0xfffc) ) {								\
	UINT8 op1 = z80_readmem16(zPC);								\
	UINT8 op2 = z80_readmem16(zPC+1);							\
	if( (op1==0x7c && op2==0xb5) || (op1==0x7d && op2==0xb4) )	\
	{															\
		UINT8 op3 = z80_readmem16(zPC+2);						\
		UINT8 op4 = z80_readmem16(zPC+3);						\
		if( op3==0x20 && op4==0xfb )							\
		{														\
			int cnt =											\
				4/*cc_op[0x7c]*/ +						\
				cc_op[0xb5] +						\
				7/*cc_op[0x20]*/ +						\
				5/*cc_ex[0x20]*/;						\
			while( zHL > 0 && z80_ICount > cnt )				\
			{													\
				BURNODD( cnt, 4, cnt ); 						\
				zHL--;											\
			}													\
		}														\
		else													\
		if( op3==0xc2 ) 										\
		{														\
			UINT8 ad1 = z80_readmem16(zPC+3);				\
			UINT8 ad2 = z80_readmem16(zPC+4);				\
			if( (ad1 + 256 * ad2) == (zPC - 1) )				\
			{													\
				int cnt =										\
					4/*cc_op[0x7c]*/ +					\
					cc_op[0xb5] +					\
					cc_op[0xc2] +					\
					cc_ex[0xc2];					\
				while( zHL > 0 && z80_ICount > cnt )			\
				{												\
					BURNODD( cnt, 4, cnt ); 					\
					zHL--;										\
				}												\
			}													\
		}														\
	}															\
}

#else

#define CHECK_BC_LOOP
#define CHECK_DE_LOOP
#define CHECK_HL_LOOP

#endif


#if 00
/* extra cycles if jr/jp/call taken and 'interrupt latency' on rst 0-7 */
static const UINT8 cc_ex[0x100] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* DJNZ */
	5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, /* JR NZ/JR Z */
	5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, /* JR NC/JR C */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, /* LDIR/CPIR/INIR/OTIR LDDR/CPDR/INDR/OTDR */
	6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
	6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
	6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
	6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2};
#endif
/**********************************************************
 * main opcodes
 **********************************************************/
/***************************************************************
 * JR (???? この「ＪＲ」は、よくワカンナイでっす。謎)
 ***************************************************************/
#if 1
//うーんとタブンはっくって感じ。
#define JR()													\
{																\
	unsigned short oldpc = zPC-1;								\
	INT8 arg = (INT8)ARG(); /* ARG() also increments PC */		\
	zPC += arg; 			/* so don't do PC += ARG() */		\
	/* speed up busy loop */									\
	if( zPC == oldpc )											\
	{															\
		if( !zEI )												\
			BURNODD( z80_ICount, 1, XLK_18_ );					\
	}															\
	else														\
	{															\
	/*	UINT8 op = z80_readmem16(zPC); 			*/				\
		UINT8 op = (*(zBase+zPC)); 							\
		if( zPC == (oldpc-1) )									\
		{														\
			/* NOP - JR $-1 or EI - JR $-1 */					\
			if( op == 0x00 || op == 0xfb )						\
			{													\
				if( !zEI )										\
				   BURNODD( z80_ICount-XLK_NOP, 2, XLK_NOP+XLK_18_); \
			}													\
		}														\
		else													\
		/* LD SP,#xxxx - JR $-3 */								\
		if( zPC == (oldpc-3) && op == 0x31 )					\
		{														\
			if( !zEI )											\
			   BURNODD( z80_ICount-XLK_31_, 2, XLK_31_+XLK_18_); \
		}														\
	}															\
}
#else
//で、簡略化してみた？？解からんが本来こう？
#define JR()													\
{																\
	INT8 arg = (INT8)ARG(); /* ARG() also increments PC */		\
	zPC += arg; 			/* so don't do PC += ARG() */		\
}
#endif
/***************************************************************
 * JR_COND
 ***************************************************************/

#define JR_COND(cond/*,opcode*/)	\
	if( cond )						\
	{								\
		z80_ICount -= ((5));	/*	CC(ex,opcode);*/\
		{							\
	/*		INT8 arg = (INT8)z80_readmem16(zPC);*/\
			INT8 arg = (INT8)(*(zBase+zPC));\
			zPC += arg; 			\
		}							\
	}								\
	zPC++;

static void op_00(void) { ; 														} /* NOP			*/
static void op_08(void) {	unsigned short iii; iii = zAF; zAF = zAF2; zAF2 = iii;	} /* EX   AF,AF'	*/
static void op_10(void) { z80_ICount -= (8-4);	zB--; JR_COND( zB  /*, 0x10*/ );	} /* DJNZ o 		*/
static void op_18(void) { z80_ICount -= (12-4); JR();								} /* JR   o 		*/
static void op_20(void) { z80_ICount -= (7-4);	JR_COND(!(zF & ZF)/*, 0x20*/ ); } /* JR   NZ,o		*/
static void op_28(void) { z80_ICount -= (7-4);	JR_COND( (zF & ZF)/*, 0x28*/ ); } /* JR   Z,o		*/
static void op_30(void) { z80_ICount -= (7-4);	JR_COND(!(zF & CF)/*, 0x30*/ ); } /* JR   NC,o		*/
static void op_38(void) { z80_ICount -= (7-4);	JR_COND( (zF & CF)/*, 0x38*/ ); } /* JR   C,o		*/

static void op_01(void) { z80_ICount -= (10-4); zBC = ARG16();				} /* LD   BC,w		*/
static void op_11(void) { z80_ICount -= (10-4); zDE = ARG16();				} /* LD   DE,w		*/
static void op_21(void) { z80_ICount -= (10-4); zHL = ARG16();				} /* LD   HL,w		*/
static void op_31(void) { z80_ICount -= (10-4); zSP = ARG16();				} /* LD   SP,w		*/

static void ADD16HL(UINT16 my_SR)
{
	UINT32 res;
	UINT16 rrr;
	res = (UINT32)zHL + (UINT32)my_SR;
	rrr = (UINT16)res;
	zF = (zF & (SF | ZF | VF)) |
		(((zHL ^ rrr ^ my_SR) >> 8) & HF) |
		((res&(0x00010000) )?1:0) |
		((rrr >> 8) & (YF | XF));
	zHL=rrr;
/*z80_ICount -= (15-4-4);*/
/*z80_ICount -= (11-4);*/
  z80_ICount -= ((7));
}
static void op_09(void) { /*z80_ICount -= (11-4);*/ ADD16HL(zBC);				} /* ADD  HL,BC 	*/
static void op_19(void) { /*z80_ICount -= (11-4);*/ ADD16HL(zDE);				} /* ADD  HL,DE 	*/
static void op_29(void) { /*z80_ICount -= (11-4);*/ ADD16HL(zHL);				} /* ADD  HL,HL 	*/
static void op_39(void) { /*z80_ICount -= (11-4);*/ ADD16HL(zSP);				} /* ADD  HL,SP 	*/

static void op_02(void) { z80_ICount -= (7-4);	WM( zBC, zA );				} /* LD   (BC),A	*/
static void op_0a(void) { z80_ICount -= (7-4);	zA = RM(zBC);				} /* LD   A,(BC)	*/
static void op_12(void) { z80_ICount -= (7-4);	WM( zDE, zA );				} /* LD   (DE),A	*/
static void op_1a(void) { z80_ICount -= (7-4);	zA = RM(zDE);										} /* LD   A,(DE)	*/
static void op_22(void) { z80_ICount -= (16-4); EA = ARG16(); WM16( EA, &Z80.hl );					} /* LD   (w),HL	*/
static void op_2a(void) { z80_ICount -= (16-4); EA = ARG16(); RM16( EA, &Z80.hl );					} /* LD   HL,(w)	*/
static void op_32(void) { z80_ICount -= (13-4); EA = ARG16(); WM( EA, zA ); 						} /* LD   (w),A 	*/
static void op_3a(void) { z80_ICount -= (13-4); EA = ARG16(); zA = RM( EA );						} /* LD   A,(w) 	*/

static void op_03(void) { z80_ICount -= (6-4);	zBC++;						} /* INC  BC		*/
static void op_0b(void) { z80_ICount -= (6-4);	zBC--; CHECK_BC_LOOP;		} /* DEC  BC		*/
static void op_13(void) { z80_ICount -= (6-4);	zDE++;						} /* INC  DE		*/
static void op_1b(void) { z80_ICount -= (6-4);	zDE--; CHECK_DE_LOOP;		} /* DEC  DE		*/
static void op_23(void) { z80_ICount -= (6-4);	zHL++;						} /* INC  HL		*/
static void op_2b(void) { z80_ICount -= (6-4);	zHL--; CHECK_HL_LOOP;		} /* DEC  HL		*/
static void op_33(void) { z80_ICount -= (6-4);	zSP++;						} /* INC  SP		*/
static void op_3b(void) { z80_ICount -= (6-4);	zSP--;						} /* DEC  SP		*/

static unsigned short RM3zHL(void){ z80_ICount -= 3;return RM(zHL); }

static void op_04(void) {						zB = INC(zB);				} /* INC  B 		*/
static void op_0c(void) {						zC = INC(zC);				} /* INC  C 		*/
static void op_14(void) {						zD = INC(zD);				} /* INC  D 		*/
static void op_1c(void) {						zE = INC(zE);				} /* INC  E 		*/
static void op_24(void) {						zH = INC(zH);				} /* INC  H 		*/
static void op_2c(void) {						zL = INC(zL);				} /* INC  L 		*/
static void op_34(void) { z80_ICount -= (11-4-3);	WM( zHL, INC(RM3zHL()) );	} /* INC  (HL)		*/
static void op_3c(void) {						zA = INC(zA);				} /* INC  A 		*/

static void op_05(void) {						zB = DEC(zB);				} /* DEC  B 		*/
static void op_0d(void) {						zC = DEC(zC);				} /* DEC  C 		*/
static void op_15(void) {						zD = DEC(zD);				} /* DEC  D 		*/
static void op_1d(void) {						zE = DEC(zE);				} /* DEC  E 		*/
static void op_25(void) {						zH = DEC(zH);				} /* DEC  H 		*/
static void op_2d(void) {						zL = DEC(zL);				} /* DEC  L 		*/
static void op_35(void) { z80_ICount -= (11-4-3);	WM( zHL, DEC(RM3zHL()) );	} /* DEC  (HL)		*/
static void op_3d(void) {						zA = DEC(zA);				} /* DEC  A 		*/

static void op_06(void) { z80_ICount -= (7-4);	zB = ARG(); 				} /* LD   B,n		*/
static void op_0e(void) { z80_ICount -= (7-4);	zC = ARG(); 				} /* LD   C,n		*/
static void op_16(void) { z80_ICount -= (7-4);	zD = ARG(); 				} /* LD   D,n		*/
static void op_1e(void) { z80_ICount -= (7-4);	zE = ARG(); 				} /* LD   E,n		*/
static void op_26(void) { z80_ICount -= (7-4);	zH = ARG(); 				} /* LD   H,n		*/
static void op_2e(void) { z80_ICount -= (7-4);	zL = ARG(); 				} /* LD   L,n		*/
static void op_36(void) { z80_ICount -= (10-4); WM( zHL, ARG() );			} /* LD   (HL),n	*/
static void op_3e(void) { z80_ICount -= (7-4);	zA = ARG(); 				} /* LD   A,n		*/

static void op_07(void) {						RLCA;						} /* RLCA			*/
static void op_0f(void) {						RRCA;						} /* RRCA			*/
static void op_17(void) {						RLA;						} /* RLA			*/
static void op_1f(void) {						RRA;												} /* RRA			*/
static void op_27(void) {						DAA;												} /* DAA			*/
static void op_2f(void) {						zA^=0xff; zF=(zF&(SF|ZF|PF|CF))|HF|NF|(zA&(YF|XF)); } /* CPL			*/
static void op_37(void) {						zF = (zF & (SF|ZF|PF)) | CF | (zA & (YF|XF));		} /* SCF			*/
static void op_3f(void) {						zF=((zF&(SF|ZF|PF|CF))|((zF&CF)<<4)|(zA&(YF|XF)))^CF;} /* CCF			*/
//static void op_3f(void) { zF = ((zF & ~(HF|NF)) | ((zF & CF)<<4)) ^ CF;	} /* CCF			*/

////////

//static void op_40(void) { 													} /* LD   B,B		*/
static void op_41(void) { zB = zC;											} /* LD   B,C		*/
static void op_42(void) { zB = zD;											} /* LD   B,D		*/
static void op_43(void) { zB = zE;											} /* LD   B,E		*/
static void op_44(void) { zB = zH;											} /* LD   B,H		*/
static void op_45(void) { zB = zL;											} /* LD   B,L		*/
static void op_46(void) { /*z80_ICount -= (7-4-3);*/	zB = RM3zHL();			} /* LD   B,(HL)	*/
static void op_47(void) { zB = zA;											} /* LD   B,A		*/

static void op_48(void) { zC = zB;											} /* LD   C,B		*/
//static void op_49(void) { 													} /* LD   C,C		*/
static void op_4a(void) { zC = zD;											} /* LD   C,D		*/
static void op_4b(void) { zC = zE;											} /* LD   C,E		*/
static void op_4c(void) { zC = zH;											} /* LD   C,H		*/
static void op_4d(void) { zC = zL;											} /* LD   C,L		*/
static void op_4e(void) { /*z80_ICount -= (7-4-3);*/	zC = RM3zHL();			} /* LD   C,(HL)	*/
static void op_4f(void) { zC = zA;											} /* LD   C,A		*/

static void op_50(void) { zD = zB;											} /* LD   D,B		*/
static void op_51(void) { zD = zC;											} /* LD   D,C		*/
//static void op_52(void) { 													} /* LD   D,D		*/
static void op_53(void) { zD = zE;											} /* LD   D,E		*/
static void op_54(void) { zD = zH;											} /* LD   D,H		*/
static void op_55(void) { zD = zL;											} /* LD   D,L		*/
static void op_56(void) { /*z80_ICount -= (7-4-3);*/	zD = RM3zHL();			} /* LD   D,(HL)	*/
static void op_57(void) { zD = zA;											} /* LD   D,A		*/

static void op_58(void) { zE = zB;											} /* LD   E,B		*/
static void op_59(void) { zE = zC;											} /* LD   E,C		*/
static void op_5a(void) { zE = zD;											} /* LD   E,D		*/
//static void op_5b(void) { 													} /* LD   E,E		*/
static void op_5c(void) { zE = zH;											} /* LD   E,H		*/
static void op_5d(void) { zE = zL;											} /* LD   E,L		*/
static void op_5e(void) { /*z80_ICount -= (7-4-3);*/	zE = RM3zHL();			} /* LD   E,(HL)	*/
static void op_5f(void) { zE = zA;											} /* LD   E,A		*/

static void op_60(void) { zH = zB;											} /* LD   H,B		*/
static void op_61(void) { zH = zC;											} /* LD   H,C		*/
static void op_62(void) { zH = zD;											} /* LD   H,D		*/
static void op_63(void) { zH = zE;											} /* LD   H,E		*/
//static void op_64(void) { 													} /* LD   H,H		*/
static void op_65(void) { zH = zL;											} /* LD   H,L		*/
static void op_66(void) { /*z80_ICount -= (7-4-3);*/	zH = RM3zHL();			} /* LD   H,(HL)	*/
static void op_67(void) { zH = zA;											} /* LD   H,A		*/

static void op_68(void) { zL = zB;											} /* LD   L,B		*/
static void op_69(void) { zL = zC;											} /* LD   L,C		*/
static void op_6a(void) { zL = zD;											} /* LD   L,D		*/
static void op_6b(void) { zL = zE;											} /* LD   L,E		*/
static void op_6c(void) { zL = zH;											} /* LD   L,H		*/
//static void op_6d(void) { 													} /* LD   L,L		*/
static void op_6e(void) { /*z80_ICount -= (7-4-3);*/	zL = RM3zHL();			} /* LD   L,(HL)	*/
static void op_6f(void) { zL = zA;											} /* LD   L,A		*/

static void op_70(void) { z80_ICount -= (7-4);	WM( zHL, zB );				} /* LD   (HL),B	*/
static void op_71(void) { z80_ICount -= (7-4);	WM( zHL, zC );				} /* LD   (HL),C	*/
static void op_72(void) { z80_ICount -= (7-4);	WM( zHL, zD );				} /* LD   (HL),D	*/
static void op_73(void) { z80_ICount -= (7-4);	WM( zHL, zE );				} /* LD   (HL),E	*/
static void op_74(void) { z80_ICount -= (7-4);	WM( zHL, zH );				} /* LD   (HL),H	*/
static void op_75(void) { z80_ICount -= (7-4);	WM( zHL, zL );				} /* LD   (HL),L	*/
static void op_76(void) {						ENTER_HALT; 				} /* HALT			*/
static void op_77(void) { z80_ICount -= (7-4);	WM( zHL, zA );				} /* LD   (HL),A	*/

static void op_78(void) { zA = zB;											} /* LD   A,B		*/
static void op_79(void) { zA = zC;											} /* LD   A,C		*/
static void op_7a(void) { zA = zD;											} /* LD   A,D		*/
static void op_7b(void) { zA = zE;											} /* LD   A,E		*/
static void op_7c(void) { zA = zH;											} /* LD   A,H		*/
static void op_7d(void) { zA = zL;											} /* LD   A,L		*/
static void op_7e(void) { /*z80_ICount -= (7-4-3);*/	zA = RM3zHL();			} /* LD   A,(HL)	*/
//static void op_7f(void) { 													} /* LD   A,A		*/

static void op_80(void) { ADD(zB);											} /* ADD  A,B		*/
static void op_81(void) { ADD(zC);											} /* ADD  A,C		*/
static void op_82(void) { ADD(zD);											} /* ADD  A,D		*/
static void op_83(void) { ADD(zE);											} /* ADD  A,E		*/
static void op_84(void) { ADD(zH);											} /* ADD  A,H		*/
static void op_85(void) { ADD(zL);											} /* ADD  A,L		*/
static void op_86(void) { /*z80_ICount -= (7-4-3);*/	ADD(RM3zHL());			} /* ADD  A,(HL)	*/
static void op_87(void) { ADD(zA);											} /* ADD  A,A		*/

static void op_88(void) { ADC(zB);											} /* ADC  A,B		*/
static void op_89(void) { ADC(zC);											} /* ADC  A,C		*/
static void op_8a(void) { ADC(zD);											} /* ADC  A,D		*/
static void op_8b(void) { ADC(zE);											} /* ADC  A,E		*/
static void op_8c(void) { ADC(zH);											} /* ADC  A,H		*/
static void op_8d(void) { ADC(zL);											} /* ADC  A,L		*/
static void op_8e(void) { /*z80_ICount -= (7-4-3);*/	ADC(RM3zHL());			} /* ADC  A,(HL)	*/
static void op_8f(void) { ADC(zA);											} /* ADC  A,A		*/

static void op_90(void) { SUB(zB);											} /* SUB  B 		*/
static void op_91(void) { SUB(zC);											} /* SUB  C 		*/
static void op_92(void) { SUB(zD);											} /* SUB  D 		*/
static void op_93(void) { SUB(zE);											} /* SUB  E 		*/
static void op_94(void) { SUB(zH);											} /* SUB  H 		*/
static void op_95(void) { SUB(zL);											} /* SUB  L 		*/
static void op_96(void) { /*z80_ICount -= (7-4-3);*/	SUB(RM3zHL());			} /* SUB  (HL)		*/
static void op_97(void) { SUB(zA);											} /* SUB  A 		*/

static void op_98(void) { SBC(zB);											} /* SBC  A,B		*/
static void op_99(void) { SBC(zC);											} /* SBC  A,C		*/
static void op_9a(void) { SBC(zD);											} /* SBC  A,D		*/
static void op_9b(void) { SBC(zE);											} /* SBC  A,E		*/
static void op_9c(void) { SBC(zH);											} /* SBC  A,H		*/
static void op_9d(void) { SBC(zL);											} /* SBC  A,L		*/
static void op_9e(void) { /*z80_ICount -= (7-4-3);*/	SBC(RM3zHL());			} /* SBC  A,(HL)	*/
static void op_9f(void) { SBC(zA);											} /* SBC  A,A		*/

static void op_a0(void) { AND(zB);											} /* AND  B 		*/
static void op_a1(void) { AND(zC);											} /* AND  C 		*/
static void op_a2(void) { AND(zD);											} /* AND  D 		*/
static void op_a3(void) { AND(zE);											} /* AND  E 		*/
static void op_a4(void) { AND(zH);											} /* AND  H 		*/
static void op_a5(void) { AND(zL);											} /* AND  L 		*/
static void op_a6(void) { /*z80_ICount -= (7-4-3);*/	AND(RM3zHL());			} /* AND  (HL)		*/
static void op_a7(void) { AND(zA);											} /* AND  A 		*/

static void op_a8(void) { XOR(zB);											} /* XOR  B 		*/
static void op_a9(void) { XOR(zC);											} /* XOR  C 		*/
static void op_aa(void) { XOR(zD);											} /* XOR  D 		*/
static void op_ab(void) { XOR(zE);											} /* XOR  E 		*/
static void op_ac(void) { XOR(zH);											} /* XOR  H 		*/
static void op_ad(void) { XOR(zL);											} /* XOR  L 		*/
static void op_ae(void) { /*z80_ICount -= (7-4-3);*/	XOR(RM3zHL());			} /* XOR  (HL)		*/
static void op_af(void) { /*XOR(zA);*/zA=0; zF =SZP[0] /*SZP[zA 00]*/;		} /* XOR  A 		*/

static void op_b0(void) { OR(zB);											} /* OR   B 		*/
static void op_b1(void) { OR(zC);											} /* OR   C 		*/
static void op_b2(void) { OR(zD);											} /* OR   D 		*/
static void op_b3(void) { OR(zE);											} /* OR   E 		*/
static void op_b4(void) { OR(zH);											} /* OR   H 		*/
static void op_b5(void) { OR(zL);											} /* OR   L 		*/
static void op_b6(void) { /*z80_ICount -= (7-4-3);*/	OR(RM3zHL());			} /* OR   (HL)		*/
static void op_b7(void) { OR(zA);											} /* OR   A 		*/

static void op_b8(void) { CP(zB);											} /* CP   B 		*/
static void op_b9(void) { CP(zC);											} /* CP   C 		*/
static void op_ba(void) { CP(zD);											} /* CP   D 		*/
static void op_bb(void) { CP(zE);											} /* CP   E 		*/
static void op_bc(void) { CP(zH);											} /* CP   H 		*/
static void op_bd(void) { CP(zL);											} /* CP   L 		*/
static void op_be(void) { /*z80_ICount -= (7-4-3);*/	CP(RM3zHL());		} /* CP   (HL)		*/
static void op_bf(void) { CP(zA);											} /* CP   A 		*/

////////
static void RET_COND(unsigned char cond/*,opcode*/){
	z80_ICount -= (5-4);
	if( cond )
	{
		POP(pc);
		z80_ICount -= ((6));/*CC(ex,opcode);*/
	}
}
static void op_c0(void) { /*z80_ICount -= (5-4);*/	RET_COND( !(zF & ZF)/*, 0xc0*/ );	} /* RET  NZ		*/
static void op_d0(void) { /*z80_ICount -= (5-4);*/	RET_COND( !(zF & CF)/*, 0xd0*/ );	} /* RET  NC		*/
static void op_e0(void) { /*z80_ICount -= (5-4);*/	RET_COND( !(zF & PF)/*, 0xe0*/ );	} /* RET  PO		*/
static void op_f0(void) { /*z80_ICount -= (5-4);*/	RET_COND( !(zF & SF)/*, 0xf0*/ );	} /* RET  P 		*/

static void op_c8(void) { /*z80_ICount -= (5-4);*/	RET_COND(  (zF & ZF)/*, 0xc8*/ );	} /* RET  Z 		*/
static void op_d8(void) { /*z80_ICount -= (5-4);*/	RET_COND(  (zF & CF)/*, 0xd8*/ );	} /* RET  C 		*/
static void op_e8(void) { /*z80_ICount -= (5-4);*/	RET_COND(  (zF & PF)/*, 0xe8*/ );	} /* RET  PE		*/
static void op_f8(void) { /*z80_ICount -= (5-4);*/	RET_COND(  (zF & SF)/*, 0xf8*/ );	} /* RET  M 		*/

static void op_c1(void) { z80_ICount -= (10-4); POP(bc);						} /* POP  BC		*/
static void op_c9(void) { z80_ICount -= (10-4); POP(pc);						} /* RET			*/
static void op_d1(void) { z80_ICount -= (10-4); POP(de);						} /* POP  DE		*/
static void op_d9(void) {						EXX;							} /* EXX			*/
static void op_e1(void) { z80_ICount -= (10-4); POP(hl);						} /* POP  HL		*/
static void op_e9(void) {						zPC = zHL;						} /* JP   (HL)		*/
static void op_f1(void) { z80_ICount -= (10-4); POP(af);						} /* POP  AF		*/
static void op_f9(void) { z80_ICount -= (6-4);	zSP = zHL;						} /* LD   SP,HL 	*/

static void JP_COND(unsigned char cond){
	z80_ICount -= (10-4);
//	if( cond )	{	zPC = ARG16();		}
//	else		{	zPC += 2;			}
//	if( cond )	{	zPC = (z80_readmem16(zPC) | (z80_readmem16((zPC+1)&0xffff) << 8));		}
	if( cond )	{	zPC = ((*(zBase+zPC)) | ( ((unsigned short)(*(zBase+zPC+1))) << 8));		}
	else		{	zPC += 2;			}
}
static void op_c2(void) { /*z80_ICount -= (10-4);*/ JP_COND(!(zF & ZF) ); 		} /* JP   NZ,a		*/
static void op_d2(void) { /*z80_ICount -= (10-4);*/ JP_COND(!(zF & CF) ); 		} /* JP   NC,a		*/
static void op_e2(void) { /*z80_ICount -= (10-4);*/ JP_COND(!(zF & PF) ); 		} /* JP   PO,a		*/
static void op_f2(void) { /*z80_ICount -= (10-4);*/ JP_COND(!(zF & SF) ); 		} /* JP   P,a		*/
static void op_ca(void) { /*z80_ICount -= (10-4);*/ JP_COND( (zF & ZF) ); 		} /* JP   Z,a		*/
static void op_da(void) { /*z80_ICount -= (10-4);*/ JP_COND( (zF & CF) ); 		} /* JP   C,a		*/
static void op_ea(void) { /*z80_ICount -= (10-4);*/ JP_COND( (zF & PF) ); 		} /* JP   PE,a		*/
static void op_fa(void) { /*z80_ICount -= (10-4);*/ JP_COND( (zF & SF) ); 		} /* JP   M,a		*/

static void op_c3(void) { z80_ICount -= (10-4); JP; 							} /* JP   a 		*/
static void op_cb(void) {
//EXEC(cb,ROP());
//#define EXEC(cb,ROP())
{
	unsigned op = ROP();
//	CC(cb,op);
	if(0x06==(op&0x07)){z80_ICount -= (15-4);}
	else{z80_ICount -= (8-4);}
	(*Z80cb[op])();
}

} /* **** CB xx 	*/
static void op_d3(void) { z80_ICount -= (11-4); /*unsigned n =*/ ARG() /*|(zA << 8)*/; OUT( n, zA );} /* OUT  (n),A  */
static void op_db(void) { z80_ICount -= (11-4); /*unsigned n =*/ ARG() /*|(zA << 8)*/; zA =IN(n);	} /* IN   A,(n) 	*/
static void op_e3(void) { z80_ICount -= (19-4); EXSP(hl);											} /* EX   HL,(SP)	*/
static void op_eb(void) {						EX_DE_HL;											} /* EX   DE,HL 	*/
static void op_f3(void) {					/*	zIFF1 = 0;	zIFF2 = 0;	*/zIFFS=0;					} /* DI 			*/
/***************************************************************
 * EI
 ***************************************************************/
	/* If interrupts were disabled, execute one more
	 * instruction and check the IRQ line.
	 * If not, simply set interrupt flip-flop 2
	 */
static void mono_exec(void)
{
	unsigned op = ROP();
/*	CC(prefix,op);*/
	z80_ICount -= 4;
	(*Z80op[op])();
}
static void op_fb(void) {
//	if( zIFF1 == 0 )
	if( (zIFFS &IFF1_ON)==0 )
	{
	//	zIFF1 = 1;
	//	zIFF2 = 1;
		zIFFS = (IFF1_ON|IFF2_ON);

//		while( z80_readmem16(zPC) == 0xfb ) /* more EIs? */
		while( (*(zBase+zPC)) == 0xfb ) /* more EIs? */
		{
		/*	CC(op,0xfb);*/
			z80_ICount -= 4;
			zPC++;

		}
		if( Z80.irq_state != CLEAR_LINE )
		{
			zEI = 1;	/* avoid cycle skip hacks */
		/*	EXEC(op,ROP()); */
			mono_exec();
			zEI = 0;
			take_interrupt();
		} else {
			/*{EXEC(op,ROP());}*/
		}
	} else {
	//	zIFF2 = 1;
		zIFFS |= IFF2_ON;
	}
} /* EI 			*/


/***************************************************************
 * CALL
 ***************************************************************/
#define CALL()					\
	EA = ARG16();				\
	PUSH( pc ); 				\
	zPC = EA;

static void CALL_COND(unsigned char cond/*,opcode*/){
	if( cond )
	{
		CALL();
		z80_ICount -= ((7));/*CC(ex,opcode);*/
	}
	else
	{
		zPC+=2;
	}
	z80_ICount -= (10-4);
}
static void op_c4(void) { /*z80_ICount -= (10-4);*/ CALL_COND(!(zF & ZF)/*, 0xc4*/ );	} /* CALL NZ,a		*/
static void op_d4(void) { /*z80_ICount -= (10-4);*/ CALL_COND(!(zF & CF)/*, 0xd4*/ );	} /* CALL NC,a		*/
static void op_e4(void) { /*z80_ICount -= (10-4);*/ CALL_COND(!(zF & PF)/*, 0xe4*/ );	} /* CALL PO,a		*/
static void op_f4(void) { /*z80_ICount -= (10-4);*/ CALL_COND(!(zF & SF)/*, 0xf4*/ );	} /* CALL P,a		*/
static void op_cc(void) { /*z80_ICount -= (10-4);*/ CALL_COND( (zF & ZF)/*, 0xcc*/ );	} /* CALL Z,a		*/
static void op_dc(void) { /*z80_ICount -= (10-4);*/ CALL_COND( (zF & CF)/*, 0xdc*/ );	} /* CALL C,a		*/
static void op_ec(void) { /*z80_ICount -= (10-4);*/ CALL_COND( (zF & PF)/*, 0xec*/ );	} /* CALL PE,a		*/
static void op_fc(void) { /*z80_ICount -= (10-4);*/ CALL_COND( (zF & SF)/*, 0xfc*/ );	} /* CALL M,a		*/

static void op_c5(void) { z80_ICount -= (11-4); PUSH( bc ); 					} /* PUSH BC		*/
static void op_cd(void) { z80_ICount -= (17-4); CALL(); 						} /* CALL a 		*/
static void op_d5(void) { z80_ICount -= (11-4); PUSH( de ); 					} /* PUSH DE		*/

static void op_dd(void) {
//	EXEC(dd,ROP());
//#define EXEC(dd,ROP())
{
	unsigned op = ROP();
//	CC(dd,op);
	z80_ICount -= (((4)));	/*4+4 cycles eated */
	(*Z80dd[op])();
}

} /* **** DD xx 	*/
static void op_e5(void) { z80_ICount -= (11-4); PUSH( hl ); 		} /* PUSH HL		*/
static void op_ed(void) { HandleED();								} /* **** ED xx 	*/
static void op_f5(void) { z80_ICount -= (11-4); PUSH( af ); 		} /* PUSH AF		*/
//static void op_fd(void) {   EXEC(fd,ROP());						} /* **** FD xx 	*/
static void op_fd(void) {
	{unsigned short mytemp;
		mytemp= zIX;
			zIX=zIY;
		//	EXEC(dd,ROP());
			op_dd();
			zIY=zIX;
		zIX=mytemp;
	}
} /* **** FD xx 	*/


static void op_c6(void) { z80_ICount -= (7-4);	ADD(ARG()); 	} /* ADD  A,n		*/
static void op_ce(void) { z80_ICount -= (7-4);	ADC(ARG()); 	} /* ADC  A,n		*/
static void op_d6(void) { z80_ICount -= (7-4);	SUB(ARG()); 	} /* SUB  n 		*/
static void op_de(void) { z80_ICount -= (7-4);	SBC(ARG()); 	} /* SBC  A,n		*/
static void op_e6(void) { z80_ICount -= (7-4);	AND(ARG()); 	} /* AND  n 		*/
static void op_ee(void) { z80_ICount -= (7-4);	XOR(ARG()); 	} /* XOR  n 		*/
static void op_f6(void) { z80_ICount -= (7-4);	OR(ARG());		} /* OR   n 		*/
static void op_fe(void) { z80_ICount -= (7-4);	CP(ARG());		} /* CP   n 		*/

/***************************************************************
 * RST
 ***************************************************************/
static void RST(unsigned char addr){
	z80_ICount -= (11-4);
	PUSH( pc );
	zPC = (unsigned short)addr;
}
static void op_c7(void) { /*z80_ICount -= (11-4);*/ RST(0x00);		} /* RST  0 		*/
static void op_cf(void) { /*z80_ICount -= (11-4);*/ RST(0x08);		} /* RST  1 		*/
static void op_d7(void) { /*z80_ICount -= (11-4);*/ RST(0x10);		} /* RST  2 		*/
static void op_df(void) { /*z80_ICount -= (11-4);*/ RST(0x18);		} /* RST  3 		*/
static void op_e7(void) { /*z80_ICount -= (11-4);*/ RST(0x20);		} /* RST  4 		*/
static void op_ef(void) { /*z80_ICount -= (11-4);*/ RST(0x28);		} /* RST  5 		*/
static void op_f7(void) { /*z80_ICount -= (11-4);*/ RST(0x30);		} /* RST  6 		*/
static void op_ff(void) { /*z80_ICount -= (11-4);*/ RST(0x38);		} /* RST  7 		*/



extern void z80_set_irq0(void);
static void take_interrupt(void)
{
//	if(  zIFF1 )
	if( (zIFFS&IFF1_ON) )
	{
//		int irq_vector;

		/* Check if processor was halted */
		LEAVE_HALT;

		/* Clear both interrupt flip flops */
	//	zIFF1 = 0;
	//	zIFF2 = 0;
		zIFFS = 0;

		/* Call back the cpu interface to retrieve the vector */
	//	irq_vector = (*Z80.irq_callback)(0);
		z80_set_irq0();
	//	z80_set_irq_line(0, CLEAR_LINE);
		z80_set_irq_line0_off();
//		irq_vector = 0xff;

		/* Interrupt mode 2. Call [Z80.i:databyte] */
		if( zIM == IM2 )
		{
//			irq_vector = (irq_vector & 0xff) | (zI << 8);
			PUSH( pc );
//			RM16( irq_vector, &Z80.pc );
			RM16(  ((zI<<8)|0x00ff) , &Z80.pc );
			/* CALL opcode timing */
			zEXcyc += 17;//(17)//cc_op[0xcd];
		}
		else/* others */
		/* Interrupt mode 1. RST 38h */
		{
			PUSH( pc );
			zPC = 0x0038;
			/* RST $38 + 'interrupt latency' cycles */
			zEXcyc += 13;//(11+2)//cc_op[0xff] + cc_ex[0xff];//???
		}


	}
}

/****************************************************************************
 * IRQ callback
 ****************************************************************************/
//static int z80_irq_callback(int irqline)
//{
//	return 0xff;
//}
/****************************************************************************
 * Set IRQ vector callback
 ****************************************************************************/
//void z80_set_irq_callback(int (*callback)(int))
//{
//	Z80.irq_callback = callback;
//}
/****************************************************************************
 * Processor initialization
 ****************************************************************************/
extern unsigned char* z80_get_mem_ptr(void);
/*static*/ void z80_init(void)
{
int i, p;

#if BIG_FLAGS_ARRAY
	{
	int oldval, newval, val;
	UINT8 *padd, *padc, *psub, *psbc;
		///* allocate big flag arrays once */
		padd = &SZHVC_add[	0*256];
		padc = &SZHVC_add[256*256];
		psub = &SZHVC_sub[	0*256];
		psbc = &SZHVC_sub[256*256];
		for (oldval = 0; oldval < 256; oldval++)
		{
			for (newval = 0; newval < 256; newval++)
			{
				/* add or adc w/o carry set */
				val = newval - oldval;
				*padd = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
				*padd |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
				if( (newval & 0x0f) < (oldval & 0x0f) ) *padd |= HF;
				if( newval < oldval ) *padd |= CF;
				if( (val^oldval^0x80) & (val^newval) & 0x80 ) *padd |= VF;
				padd++;

				/* adc with carry set */
				val = newval - oldval - 1;
				*padc = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
				*padc |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
				if( (newval & 0x0f) <= (oldval & 0x0f) ) *padc |= HF;
				if( newval <= oldval ) *padc |= CF;
				if( (val^oldval^0x80) & (val^newval) & 0x80 ) *padc |= VF;
				padc++;

				/* cp, sub or sbc w/o carry set */
				val = oldval - newval;
				*psub = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
				*psub |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
				if( (newval & 0x0f) > (oldval & 0x0f) ) *psub |= HF;
				if( newval > oldval ) *psub |= CF;
				if( (val^oldval) & (oldval^newval) & 0x80 ) *psub |= VF;
				psub++;

				/* sbc with carry set */
				val = oldval - newval - 1;
				*psbc = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
				*psbc |= (newval & (YF | XF));	/* undocumented flag bits 5+3 */
				if( (newval & 0x0f) >= (oldval & 0x0f) ) *psbc |= HF;
				if( newval >= oldval ) *psbc |= CF;
				if( (val^oldval) & (oldval^newval) & 0x80 ) *psbc |= VF;
				psbc++;
			}
		}
	}
#endif
	for (i = 0; i < 256; i++)
	{
		p = 0;
		if( i&0x01 ) p++;
		if( i&0x02 ) p++;
		if( i&0x04 ) p++;
		if( i&0x08 ) p++;
		if( i&0x10 ) p++;
		if( i&0x20 ) p++;
		if( i&0x40 ) p++;
		if( i&0x80 ) p++;
		SZ[i] = i ? i & SF : ZF;
		SZ[i] |= (i & (YF | XF));		/* undocumented flag bits 5+3 */
		SZ_BIT[i] = i ? i & SF : ZF | PF;
		SZ_BIT[i] |= (i & (YF | XF));	/* undocumented flag bits 5+3 */
		SZP[i] = SZ[i] | ((p & 1) ? 0 : PF);
		SZHV_inc[i] = SZ[i];
		if( i == 0x80 ) SZHV_inc[i] |= VF;
		if( (i & 0x0f) == 0x00 ) SZHV_inc[i] |= HF;
		SZHV_dec[i] = SZ[i] | NF;
		if( i == 0x7f ) SZHV_dec[i] |= VF;
		if( (i & 0x0f) == 0x0f ) SZHV_dec[i] |= HF;
	}
#if 00
	state_save_register_item("z80", index, Z80.prvpc.w.l);
	state_save_register_item("z80", index, Z80.pc.w.l);
	state_save_register_item("z80", index, Z80.sp.w.l);
	state_save_register_item("z80", index, Z80.af.w.l);
	state_save_register_item("z80", index, Z80.bc.w.l);
	state_save_register_item("z80", index, Z80.de.w.l);
	state_save_register_item("z80", index, Z80.hl.w.l);
	state_save_register_item("z80", index, Z80.ix.w.l);
	state_save_register_item("z80", index, Z80.iy.w.l);
	state_save_register_item("z80", index, Z80.af2.w.l);
	state_save_register_item("z80", index, Z80.bc2.w.l);
	state_save_register_item("z80", index, Z80.de2.w.l);
	state_save_register_item("z80", index, Z80.hl2.w.l);
	state_save_register_item("z80", index, Z80.r);
	state_save_register_item("z80", index, Z80.r2);
	state_save_register_item("z80", index, Z80.iff1);
	state_save_register_item("z80", index, Z80.iff2);
	state_save_register_item("z80", index, Z80.halt);
	state_save_register_item("z80", index, Z80.im);
	state_save_register_item("z80", index, Z80.i);
	state_save_register_item("z80", index, Z80.nmi_state);
	state_save_register_item("z80", index, Z80.irq_state);
#endif

//タブン
//	Z80.irq_callback = 0;//irqcallback;

//quick base
		zBase		=z80_get_mem_ptr();
}

/****************************************************************************
 * Do a reset
 ****************************************************************************/
/*static*/ void z80_reset(void)
{
	/* Reset registers to their initial values */
	my_memset0(&Z80, sizeof(Z80));
#if 00
	zBC2 = 0;
	zDE2 = 0;
	zHL2 = 0;
	zAF2 = 0;
	zBC  = 0;
	zDE  = 0;
	zHL  = 0;
	zAF  = 0;
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
	zI	 = 0xff;
#endif

//	zIM   = IM9;	//
//IM0 0
//IM1 1
//IM2 2
//IM0 3（起動時、動作はIM0と同じ）

	zIX = 0xffff;	/* IX are FFFF after a reset! */
	zIY = 0xffff;	/* IY are FFFF after a reset! */
	zF = ZF;		/* Zero flag is set */

	zPC = 0x0000;
	zSP = 0x0000;
	//zI = 0;

//	Z80.nmi_state = CLEAR_LINE;
	Z80.irq_state = CLEAR_LINE;

	z80_ICount = 0;
	zEXcyc = 0;
	zEI=0;
}

//void z80_end_timeslice(void)
//{
//	z80_ICount = 0;
//}

/****************************************************************************
 * Execute 'cycles' T-states. Return number of T-states really executed
 ****************************************************************************/
 void z80_debt_clk(short cycles){
	zEXcyc+=cycles;
}
/*static*/ int z80_execute(short cycles)
{
	z80_ICount = cycles - zEXcyc;
	zEXcyc = 0;

	do
	{
//		EXEC_INLINE(op,ROP());
//#define EXEC(prefix,opcode)
{
	unsigned op = ROP();
//	CC(prefix,op);
	z80_ICount -= 4;
	(*Z80op[op])();
}

	} while( z80_ICount > 0 );

	z80_ICount -= zEXcyc;
	zEXcyc = 0;

	return cycles - z80_ICount;
}

/****************************************************************************
 * Burn 'cycles' T-states. Adjust R register for the lost time
 ****************************************************************************/
static void z80_burn(short cycles)
{
	if( cycles > 0 )
	{
//		/* NOP takes 4 cycles per instruction */
//		int n = (cycles + 3) / 4;
//		z80_ICount -= 4 * n;
		z80_ICount -= ((cycles+3)&(~3));
	}
}
#if 00
/****************************************************************************
 * Get all registers in given buffer
 ****************************************************************************/
//atic unsigned int z80_get_context (void *dst)
static void z80_get_context (void *dst)
{
	if( dst )
		*(Z80_Regs*)dst = Z80;
//	return sizeof(Z80_Regs);
}

/****************************************************************************
 * Set all registers to given values
 ****************************************************************************/
static void z80_set_context (void *src)
{
	if( src )
		Z80 = *(Z80_Regs*)src;
}
#endif

/****************************************************************************
 * Set IRQ line state
 ****************************************************************************/
/*static*/ void z80_set_irq_line(int irqline, int state)
{
#if Z80_EMULATE_NMI
	//#define INPUT_LINE_NMI		IRQ_LINE_NMI
	//てきとー
	#define INPUT_LINE_NMI 0x66
	if(irqline == INPUT_LINE_NMI)
	{
		if( Z80.nmi_state == state ) return;

		_mes(("Z80 set_irq_line (NMI) %d\n", state));
		Z80.nmi_state = state;
		if( state == CLEAR_LINE ) return;

		_mes(("Z80 take NMI\n"));
		LEAVE_HALT; 		/* Check if processor was halted */

	//	zIFF1 = 0;
		zIFFS &= 0xfe;
		PUSH( pc );
		zPC = 0x0066;
		zEXcyc += 11;
	}
	else
#endif
	{
		_mes(("Z80 set_irq_line %d\n", state));

		/* update the IRQ state */
		Z80.irq_state = state;

		/* if there's something pending, take it */
		if( state != CLEAR_LINE )
			take_interrupt();
	}
}
void z80_set_irq_line0_off(void)
{
	Z80.irq_state = CLEAR_LINE;
}
void z80_set_irq_line0_on(void)
{
	Z80.irq_state = ASSERT_LINE;
	take_interrupt();
}

/****************************************************************************
 * Set IRQ vector callback
 ****************************************************************************/
//void z80_set_irq_callback(int (*callback)(int))
//{
//	Z80.irq_callback = callback;
//}

//void mz80GetContext(void *pData)
//{
//	z80s_context_copy(pData, &cpu );
//}
/* Set mz80's context */
//void mz80SetContext(void *pData)
//{
//	z80s_context_copy(&cpu, pData );
//}
#if 00
/**************************************************************************
 * Generic set_info
 **************************************************************************/
static void z80_set_info(UINT32 state, union cpuinfo *info)
{
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_INPUT_STATE + INPUT_LINE_NMI:	set_irq_line(INPUT_LINE_NMI, info->i);	break;
		case CPUINFO_INT_INPUT_STATE + 0:				set_irq_line(0, info->i);				break;
		case CPUINFO_INT_PC:					PC = info->i;				break;
		case CPUINFO_INT_REGISTER + Z80_PC: Z80.pc.w.l = info->i;			break;
		case CPUINFO_INT_SP:				zSP = info->i;					break;
		case CPUINFO_INT_REGISTER + Z80_SP: Z80.sp.w.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_A:	Z80.af.b.h = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_B:	Z80.bc.b.h = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_C:	Z80.bc.b.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_D:	Z80.de.b.h = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_E:	Z80.de.b.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_H:	Z80.hl.b.h = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_L:	Z80.hl.b.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_AF: Z80.af.w.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_BC: Z80.bc.w.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_DE: Z80.de.w.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_HL: Z80.hl.w.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_IX: Z80.ix.w.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_IY: Z80.iy.w.l = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_R:	Z80.r = info->i;Z80.r2 = info->i & 0x80;	break;
		case CPUINFO_INT_REGISTER + Z80_I:		Z80.i = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_AF2:	Z80.af2.w.l = info->i;		break;
		case CPUINFO_INT_REGISTER + Z80_BC2:	Z80.bc2.w.l = info->i;		break;
		case CPUINFO_INT_REGISTER + Z80_DE2:	Z80.de2.w.l = info->i;		break;
		case CPUINFO_INT_REGISTER + Z80_HL2:	Z80.hl2.w.l = info->i;		break;
		case CPUINFO_INT_REGISTER + Z80_zIM:	Z80.im = info->i;			break;
		case CPUINFO_INT_REGISTER + Z80_zIFF1:	Z80.iff1 = info->i; 		break;
		case CPUINFO_INT_REGISTER + Z80_zIFF2:	Z80.iff2 = info->i; 		break;
		case CPUINFO_INT_REGISTER + Z80_HALT:	Z80.halt = info->i; 		break;
	}
}
/**************************************************************************
 * Generic get_info
 **************************************************************************/
void z80_get_info(UINT32 state, union cpuinfo *info)
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:			info->i = sizeof(Z80);	break;
		case CPUINFO_INT_INPUT_LINES:			info->i = 1;			break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:	info->i = 0xff; 		break;
		case CPUINFO_INT_ENDIANNESS:			info->i = CPU_IS_LE;	break;
		case CPUINFO_INT_CLOCK_DIVIDER: 		info->i = 1;			break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES: info->i = 1;			break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES: info->i = 4;			break;
		case CPUINFO_INT_MIN_CYCLES:			info->i = 1;			break;
		case CPUINFO_INT_MAX_CYCLES:			info->i = 16;			break;

		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 8;	break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 16;	break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_PROGRAM: info->i = 0;	break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;	break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;	break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_DATA:	info->i = 0;	break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 8;	break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 16;	break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_IO:		info->i = 0;	break;

		case CPUINFO_INT_INPUT_STATE + INPUT_LINE_NMI:	info->i = Z80.nmi_state;	break;
		case CPUINFO_INT_INPUT_STATE + 0:				info->i = Z80.irq_state;	break;
		case CPUINFO_INT_PREVIOUSPC:					info->i = Z80.prvpc.w.l;	break;
		case CPUINFO_INT_PC:					info->i = zPCD; 			break;
		case CPUINFO_INT_REGISTER + Z80_PC: 	info->i = Z80.pc.w.l;		break;
		case CPUINFO_INT_SP:					info->i = zSPD; 			break;
		case CPUINFO_INT_REGISTER + Z80_SP: 	info->i = Z80.sp.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_A:		info->i = Z80.af.b.h;		break;
		case CPUINFO_INT_REGISTER + Z80_B:		info->i = Z80.bc.b.h;		break;
		case CPUINFO_INT_REGISTER + Z80_C:		info->i = Z80.bc.b.l;		break;
		case CPUINFO_INT_REGISTER + Z80_D:		info->i = Z80.de.b.h;		break;
		case CPUINFO_INT_REGISTER + Z80_E:		info->i = Z80.de.b.l;		break;
		case CPUINFO_INT_REGISTER + Z80_H:		info->i = Z80.hl.b.h;		break;
		case CPUINFO_INT_REGISTER + Z80_L:		info->i = Z80.hl.b.l;		break;
		case CPUINFO_INT_REGISTER + Z80_AF: 	info->i = Z80.af.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_BC: 	info->i = Z80.bc.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_DE: 	info->i = Z80.de.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_HL: 	info->i = Z80.hl.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_IX: 	info->i = Z80.ix.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_IY: 	info->i = Z80.iy.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_R:		info->i = (Z80.r & 0x7f) | (Z80.r2 & 0x80); break;
		case CPUINFO_INT_REGISTER + Z80_I:		info->i = Z80.i;			break;
		case CPUINFO_INT_REGISTER + Z80_AF2:	info->i = Z80.af2.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_BC2:	info->i = Z80.bc2.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_DE2:	info->i = Z80.de2.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_HL2:	info->i = Z80.hl2.w.l;		break;
		case CPUINFO_INT_REGISTER + Z80_zIM:	info->i = Z80.im;			break;
		case CPUINFO_INT_REGISTER + Z80_zIFF1:	info->i = Z80.iff1; 		break;
		case CPUINFO_INT_REGISTER + Z80_zIFF2:	info->i = Z80.iff2; 		break;
		case CPUINFO_INT_REGISTER + Z80_HALT:	info->i = Z80.halt; 		break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:		info->setinfo = z80_set_info;				break;
		case CPUINFO_PTR_GET_CONTEXT:	info->getcontext = z80_get_context; 		break;
		case CPUINFO_PTR_SET_CONTEXT:	info->setcontext = z80_set_context; 		break;
		case CPUINFO_PTR_INIT:			info->init = z80_init;						break;
		case CPUINFO_PTR_RESET: 		info->reset = z80_reset;					break;
		case CPUINFO_PTR_EXIT:			info->exit = z80_exit;						break;
		case CPUINFO_PTR_EXECUTE:		info->execute = z80_execute;				break;
		case CPUINFO_PTR_BURN:			info->burn = NULL;							break;
#ifdef MAME_DEBUG
		case CPUINFO_PTR_DISASSEMBLE_NEW:	info->disassemble_new = z80_dasm;		break;
#endif
		case CPUINFO_PTR_INSTRUCTION_COUNTER:	info->icount = &z80_ICount; 		break;
		case CPUINFO_PTR_REGISTER_LAYOUT:		info->p = z80_reg_layout;			break;
		case CPUINFO_PTR_WINDOW_LAYOUT: 		info->p = z80_win_layout;			break;
	}
}
#endif
#if 0
/****************************************************************************
 * Save CPU state
 ****************************************************************************/
void z80_state_save(void *file)
{
	int cpu = cpu_getactivecpu();
	state_save_UINT16(file, "z80", cpu, "AF", &Z80.af.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "BC", &Z80.bc.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "DE", &Z80.de.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "HL", &Z80.hl.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "IX", &Z80.ix.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "IY", &Z80.iy.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "PC", &Z80.pc.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "SP", &Z80.sp.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "af2", &Z80.af2.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "bc2", &Z80.bc2.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "de2", &Z80.de2.w.l, 1);
	state_save_UINT16(file, "z80", cpu, "hl2", &Z80.hl2.w.l, 1);
	state_save_UINT8(file, "z80", cpu, "R", &Z80.r, 1);
	state_save_UINT8(file, "z80", cpu, "R2", &Z80.r2, 1);
	state_save_UINT8(file, "z80", cpu, "IFF1", &Z80.iff1, 1);
	state_save_UINT8(file, "z80", cpu, "IFF2", &Z80.iff2, 1);
	state_save_UINT8(file, "z80", cpu, "HALT", &Z80.halt, 1);
	state_save_UINT8(file, "z80", cpu, "IM", &Z80.im, 1);
	state_save_UINT8(file, "z80", cpu, "I", &Z80.i, 1);

	state_save_UINT8(file, "z80", cpu, "nmi_state", &Z80.nmi_state, 1);
	state_save_UINT8(file, "z80", cpu, "irq_state", &Z80.irq_state, 1);
	/* daisy chain needs to be saved by z80ctc.c somehow */
}

/****************************************************************************
 * Load CPU state
 ****************************************************************************/
void z80_state_load(void *file)
{
	int cpu = cpu_getactivecpu();
	state_load_UINT16(file, "z80", cpu, "AF", &Z80.af.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "BC", &Z80.bc.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "DE", &Z80.de.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "HL", &Z80.hl.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "IX", &Z80.ix.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "IY", &Z80.iy.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "PC", &Z80.pc.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "SP", &Z80.sp.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "af2", &Z80.af2.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "bc2", &Z80.bc2.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "de2", &Z80.de2.w.l, 1);
	state_load_UINT16(file, "z80", cpu, "hl2", &Z80.hl2.w.l, 1);
	state_load_UINT8(file, "z80", cpu, "R", &Z80.r, 1);
	state_load_UINT8(file, "z80", cpu, "R2", &Z80.r2, 1);
	state_load_UINT8(file, "z80", cpu, "IFF1", &Z80.iff1, 1);
	state_load_UINT8(file, "z80", cpu, "IFF2", &Z80.iff2, 1);
	state_load_UINT8(file, "z80", cpu, "HALT", &Z80.halt, 1);
	state_load_UINT8(file, "z80", cpu, "IM", &Z80.im, 1);
	state_load_UINT8(file, "z80", cpu, "I", &Z80.i, 1);

	state_load_UINT8(file, "z80", cpu, "nmi_state", &Z80.nmi_state, 1);
	state_load_UINT8(file, "z80", cpu, "irq_state", &Z80.irq_state, 1);
	/* daisy chain needs to be restored by z80ctc.c somehow */
}
unsigned z80_dasm( char *buffer, unsigned pc )
{
#ifdef MAME_DEBUG
	return DasmZ80( buffer, pc );
#else
	sprintf( buffer, "$%02X", z80_readmem16(pc) );
	return 1;
#endif
}
#endif

#if 00
void state_save_z80(u8 *state_buf)
{
	u8 *in = state_buf;
	u16 r	= Z80.r;
	u16 iff = Z80.iff1 & (Z80.iff2 << 1);
	u8 nmi_state = Z80.nmi_state != CLEAR_LINE;
	u8 irq_state = Z80.irq_state != CLEAR_LINE;
	u8 irq_vector = 0xff;

	state_save_word(&Z80.bc.w.l, 1);
	state_save_word(&Z80.de.w.l, 1);
	state_save_word(&Z80.hl.w.l, 1);
	state_save_word(&Z80.af.w.l, 1);
	state_save_word(&Z80.ix.w.l, 1);
	state_save_word(&Z80.iy.w.l, 1);
	state_save_word(&Z80.sp.w.l, 1);
	state_save_long(&Z80.pc.d, 1);
	state_save_word(&Z80.bc2.w.l, 1);
	state_save_word(&Z80.de2.w.l, 1);
	state_save_word(&Z80.hl2.w.l, 1);
	state_save_word(&Z80.af2.w.l, 1);
	state_save_word(&r, 1);
	state_save_word(&iff, 1);
	state_save_byte(&Z80.i, 1);
	state_save_byte(&Z80.im, 1);
	state_save_byte(&irq_vector, 1);
	state_save_byte(&irq_state, 1);
	state_save_byte(&nmi_state, 1);
	state_save_byte(&Z80.after_EI, 1);
	state_save_byte(&Z80.halt, 1);
	state_save_long(&z80_ICount, 1);
	state_save_long(&zEXcyc, 1);
}
void state_load_z80(u8 *state_buf)
{
	u8 *out = state_buf;
	u16 r, iff;
	u8 nmi_state, irq_state, irq_vector;

	state_load_word(&Z80.bc.w.l, 1);
	state_load_word(&Z80.de.w.l, 1);
	state_load_word(&Z80.hl.w.l, 1);
	state_load_word(&Z80.af.w.l, 1);
	state_load_word(&Z80.ix.w.l, 1);
	state_load_word(&Z80.iy.w.l, 1);
	state_load_word(&Z80.sp.w.l, 1);
	state_load_long(&Z80.pc.d, 1);
	state_load_word(&Z80.bc2.w.l, 1);
	state_load_word(&Z80.de2.w.l, 1);
	state_load_word(&Z80.hl2.w.l, 1);
	state_load_word(&Z80.af2.w.l, 1);
	state_load_word(&r, 1);
	state_load_word(&iff, 1);
	state_load_byte(&Z80.i, 1);
	state_load_byte(&Z80.im, 1);
	state_load_byte(&irq_vector, 1);
	state_load_byte(&irq_state, 1);
	state_load_byte(&nmi_state, 1);
	state_load_byte(&Z80.after_EI, 1);
	state_load_byte(&Z80.halt, 1);
	state_load_long(&z80_ICount, 1);
	state_load_long(&zEXcyc, 1);

	Z80.r = r & 0xff;
	Z80.r2 = r & 0x80;
	Z80.iff1 = iff & 1;
	Z80.iff2 = iff >> 1;
	Z80.nmi_state = nmi_state ? PULSE_LINE : CLEAR_LINE;
	Z80.irq_state = irq_state ? ASSERT_LINE : CLEAR_LINE;
}
#endif

