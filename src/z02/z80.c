
/* z80stb.c
 * Zilog Z80 processor emulator in C. Modified to be API compliant with Neil
 * Bradley's MZ80 (for ported projects)
 * Ported to 'C' by Jeff Mitchell; original copyright follows:
 *
 * Abstract:
 *   Internal declarations for Zilog Z80 emulator.
 *
 * Revisions:
 *   18-Apr-97 EAM Created
 *   ??-???-97 EAM Released in MageX 0.5
 *   26-Jul-97 EAM Fixed Time Pilot slowdown bug and
 *      Gyruss hiscore bugs (opcode 0xC3)
 *
 * Original copyright (C) 1997 Edward Massey
 */

// Dave
#define FALSE 0
#define TRUE 1

#include "z80.h"

#define CF	0x01
#define NF	0x02
#define PF	0x04
#define VF	PF
#define XF	0x08
#define HF	0x10
#define YF	0x20
#define ZF	0x40
#define SF	0x80


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

#define zPendingInterrupt Z80.pi

typedef union
{
	struct { unsigned short l; } w;
	struct { unsigned char h,l; } b;
} PAIR80;

typedef struct /*mz80context*/
{
	PAIR80 bc;
	PAIR80 de;
	PAIR80 hl;
	PAIR80 af;
//
	unsigned char *Base;
//
	UINT16 pc;
	UINT16 sp;
	PAIR80 ix;
	PAIR80 iy;
//
	PAIR80 bc2;
	PAIR80 de2;
	PAIR80 hl2;
	PAIR80 af2;
/////
//	UINT8 *z80Base;
//	struct MemoryReadByte *z80MemRead;
//	struct MemoryWriteByte *z80MemWrite;
//	struct z80PortRead *z80IoRead;
//	struct z80PortWrite *z80IoWrite;
	//UINT32 z80clockticks;
	UINT8 im;//IM
	UINT8 halt;
	UINT8 iff1;//iff1
	UINT8 iff2;//iff2

	UINT8 i;
	UINT8 pi;

//	UINT8 r;

//	UINT16 z80nmiAddr;
//	UINT16 z80intAddr;
//} ;//RETRO_PACKED ;
//typedef struct mz80context CONTEXTMZ80;
//	UINT8 dummy1;
//	UINT8 dummy2;
//

} Z80_Regs;

/* accessors */
#define LOWORD(x)       ( x & 0xFFFF )
#define HIWORD(x)       ( x >> 16 )
#define LOBYTE(x)       ( x & 0xFF )
#define HIBYTE(x)       ( x >> 8 )
#define MAKEWORD(lo,hi) ( lo | ( hi << 8 ) )

/* execz80.h */


/* execz80i.h */
#define BIG_FLAGS_ARRAY

//#define ISF_PENDING   0x00000008
//#define ISF_MODE_MASK 0x00000003
//#define ISF_HALT      0x00000004
//#define ISF_IFF1      0x00000010
//#define ISF_IFF2      0x00000020
//#define ISF_IREG_MASK 0x00000F00

//#define _ASSERT assert

//UINT32 m_cRef; // reference count

//static CONTEXTMZ80 cpu ; /* CPU Context */

#define RM(aaa)		z80_readmem16(aaa)
#define WM(aaa,ddd)	z80_writemem16(aaa,ddd)

//extern unsigned char RM(unsigned int address );
//extern void WM(unsigned int address, unsigned char data );
extern unsigned char z80_readmem16(unsigned short address );
extern void z80_writemem16(unsigned short address, unsigned char data );



//static int zPC;
//static int zSP;

//UINT8 zR; // consider:  should be int for performance?
//unsigned char zI; // consider:  combine into

// TODO:  combine into a single int...
//static unsigned char zHALT;
//static unsigned char zPendingInterrupt = FALSE;
//static UINT8 zIFF1, zIFF2;
//static UINT8 zIM;

//UINT16 zAF2;
//static UINT16 zBC2;
//static UINT16 zDE2;
//static UINT16 zHL2;
//UINT16 z80intAddr = 0x38;
//UINT16 z80nmiAddr = 0x66;

static Z80_Regs Z80 __attribute__((aligned(64))) ; /* CPU Context */
static int z80_ICount;// = 0;


static UINT8 SZ[256] __attribute__((aligned(64))) ;
static UINT8 SZP[256];

static UINT8 SZHV_inc[256];
static UINT8 SZHV_dec[256];

//static UINT8 rgfBit[256][8];
static UINT8 SZ_BIT[256];	/* zero, sign and parity/overflow (=zero) flags for BIT opcode */

static UINT8 rgfAdd[256][256];
static UINT8 rgfSub[256][256];
static UINT8 rgfAddc[256][256][2];
static UINT8 rgfSubc[256][256][2];

static UINT32 dwElapsedTicks = 0;

#undef LARGE_DAA_TABLE
//#define LARGE_DAA_TABLE
#ifdef LARGE_DAA_TABLE
#include "z80daa.h"
#endif

#ifndef NULL
#define NULL 0
#endif
//static struct MemoryReadByte *z80MemoryRead = NULL;
//static struct MemoryWriteByte *z80MemoryWrite = NULL;
//static struct z80PortRead *z80IoRead = NULL;
//static struct z80PortWrite *z80IoWrite = NULL;
//UINT16 z80pc = 0;


/* ************************************
 * Memory Handlers (guesses)
 * ************************************
 */
#define ImmedByte() RM(zPC++)

static /* static */ UINT16 ARG16( void ) {
	unsigned short w;
	w  =  (unsigned short)(ImmedByte());
	w += ((unsigned short)(ImmedByte())<<8);
	return w;	// attn: little endian specific
}

static /* static */ void MemWriteWord ( UINT16 wAddr, UINT16 wVal ) {
  	WM( wAddr++,   ( wVal & 0xff) );
  	WM( wAddr,     ( wVal >>8) );
}

static UINT16 MemReadWord ( UINT16 wAddr ) {
  	UINT8 op1;
  	UINT8 op2;

	op1 = RM(wAddr++);
	op2 = RM(wAddr);
  	return (   ( op1 | (op2<<8) ) );
}

#define GetPC() ( (UINT16)(zPC) )
#define SetPC(aaa) zPC=(UINT16)(aaa)
//static void SetPC ( UINT16 wAddr ) {
//zPC=wAddr;
//}


static void AdjustPC ( signed char cb ) {
	zPC += (unsigned short)(cb);
}

static void Push(UINT16 wArg) {
	WM(--zSP ,  (wArg >> 8));
	WM(--zSP , LOBYTE(wArg));
}

static UINT16 Pop() {
	unsigned short wAddr = 0;
	wAddr  =  RM(zSP++ );
	wAddr |= (RM(zSP++ ) << 8);

	return(wAddr);
}

//static UINT16 GetSP() {
//	return (UINT16)(zSP );
//}
//static void SetSP(UINT16 wAddr) {
//	zSP=wAddr;
//}
#define GetSP() ( (UINT16)(zSP) )
#define SetSP(aaa) zSP=(UINT16)(aaa)

///* static */ UINT8 mz80GetMemory ( UINT16 addr ) {
//	struct MemoryReadByte *mr = z80MemoryRead;
//
//	z80pc = GetPC(); /* for some of the platforms */
//
//	while ( mr -> lowAddr != 0xffffffff ) {
//
//		if(addr >= mr->lowAddr && addr <= mr->highAddr) {
//			z80pc = GetPC();
//			return ( mr -> memoryCall ( addr, mr ) );
//		}
//		mr++;
//	}
//	return ( m_rgbMemory [ addr ] );
//}

///* static */ void mz80PutMemory( unsigned short int addr, unsigned char byte ) {
//	struct MemoryWriteByte *mr = z80MemoryWrite;
//
//	while ( mr->lowAddr != 0xffffffff ) {
//		if(addr >= mr->lowAddr && addr <= mr->highAddr) {
//			z80pc = GetPC();
//			mr->memoryCall(addr, byte, mr);
//			return;
//		}
//		mr++;
//	}
//	m_rgbMemory [ addr ] = byte;
//
//}

static UINT8 In(UINT8 bPort) {
//	UINT8 bVal;
//	UINT8 bVal = m_rgpfnPortRead[bPort](bPort);
//	struct z80PortRead *mr = z80IoRead;

//	bVal=0xff;
	//z80pc = GetPC(); /* for some of the platforms */

//	while ( mr->lowIoAddr != 0xffff ) {
//		if( bPort >= mr->lowIoAddr && bPort <= mr->highIoAddr) {
//			bVal = mr->IOCall ( bPort, mr );
//			break;
//		}
//	mr++;
//	}

	zF = (zF & CF) | SZP[/*bVal*/0xff];//0x84;???
	return /*bVal*/0xff;
}

#if 00
static UINT8 InRaw(UINT8 bPort) {
	UINT8 bVal;

	struct z80PortRead *mr = z80IoRead;

	//z80pc = GetPC(); /* for some of the platforms */

	while ( mr->lowIoAddr != 0xffff ) {
		if( bPort >= mr->lowIoAddr && bPort <= mr->highIoAddr) {
			bVal = mr->IOCall ( bPort, mr );
			break;
		}
		mr++;
	}

	// zF = (zF & CF) | SZP[bVal];
	return bVal;
}
#endif

static void Out(UINT8 bPort, UINT8 bVal) {
//	m_rgpfnPortWrite[bPort](bPort, bVal);
	//struct z80PortWrite *mr = z80IoWrite;

	//z80pc = GetPC(); /* for some of the platforms */

//	while ( mr->lowIoAddr != 0xffff ) {
//		if(bPort >= mr->lowIoAddr && bPort <= mr->highIoAddr) {
//			mr->IOCall ( bPort, bVal, mr );
//		return;
//		}
//	mr++;
//	}

}



/* ************************************
 * Retrocade guts stuff (again) (these guys are defined after macros)
 * ************************************
 */
#if 00
unsigned long int mz80GetContextSize(void) {
  return ( sizeof ( CONTEXTMZ80 ) );
}

/* set memory base, etc. */
void mz80GetContext ( void *receiver ) {
	CONTEXTMZ80 *c = receiver;

	c -> z80af.w.l = zAF;	c -> z80af2.w.l = zAF2;
	c -> z80bc.w.l = zBC;	c -> z80bc2.w.l = zBC2;
	c -> z80de.w.l = zDE;	c -> z80de2.w.l = zDE2;
	c -> z80hl.w.l = zHL;	c -> z80hl2.w.l = zHL2;

	c -> z80ix.w.l = zIX;
	c -> z80iy.w.l = zIY;
	c -> z80sp = GetSP();
	//c -> z80pc = GetPC();
//	c -> z80i = zI;
//	c -> z80r = zR;
//	c -> z80intAddr = z80intAddr;
//	c -> z80nmiAddr = z80nmiAddr;
//	c -> iff1 = zIFF1;
//	c -> iff2 = zIFF2;
//	c -> im = zIM;
//	c -> halt = zHALT;

//fprintf ( stderr, "GetContext\n" );
}

void mz80SetContext( void *sender ) {
	CONTEXTMZ80 *c = sender;

	zAF = c -> z80af.w.l;	zAF2 = c -> z80af2.w.l;
	zBC = c -> z80bc.w.l;	zBC2 = c -> z80bc2.w.l;
	zDE = c -> z80de.w.l;	zDE2 = c -> z80de2.w.l;
	zHL = c -> z80hl.w.l;	zHL2 = c -> z80hl2.w.l;

	zIX = c -> z80ix.w.l;
	zIY = c -> z80iy.w.l;

	zI = c -> z80i;
//	zR = c -> z80r;
//	z80intAddr = c -> z80intAddr;
//	z80nmiAddr = c -> z80nmiAddr;

//	zIFF1 = c->zIFF1;
//	zIFF2 = c->zIFF2;
//	zIM  = c->im;
//	zHALT = c->halt;

	//SetPC ( c -> z80pc );
	SetSP ( c -> z80sp );

//	fprintf ( stderr, "SetContext\n" );
}
#endif
/* ************************************
 * Z80 Please-inline-my-ass opcodes
 * ************************************
 */

#define Ret0_TRUE SetPC(Pop());

//static int Ret0(int f)
//{
//	if(f)
//	{
//		Ret0_TRUE
//		return 6;
//	}
//	else
//	{
//		return 0;
//	}
//}

//static int Ret1(int f)
//{
//	if(f)
//	{
//		return 0;
//	}
//	else
//	{
//		Ret0_TRUE
//		return 6;
//	}
//}

static UINT16 Adc_2 (UINT16 wArg1, UINT16 wArg2)
{
#ifdef BIG_FLAGS_ARRAY
	int res  = wArg1 + wArg2 + (zF & CF);
	zF = (((wArg1^res^wArg2)&0x1000)>>8)|((res>>16)&1)|((res&0x8000)>>8)|((res&65535)?0:ZF)|(((wArg2^wArg1^0x8000)&(wArg2^res)&0x8000)>>13);
	return res;
#else
	int res  = wArg1 + wArg2 + (zF & CF);
	zF = (((wArg1^res^wArg2)&0x1000)>>8)|((res>>16)&1)|((res&0x8000)>>8)|((res&65535)?0:ZF)|(((wArg2^wArg1^0x8000)&(wArg2^res)&0x8000)>>13);
	return res;
#endif
}

static UINT16 Sbc_2 (UINT16 wArg1, UINT16 wArg2)
{
#ifdef BIG_FLAGS_ARRAY
	int res = wArg1 - wArg2 - (zF & CF);

	zF  = (((wArg1^res^wArg2)&0x1000)>>8)|((res>>16)&1)|((res&0x8000)>>8)|
		((res&65535)?0:ZF)|(((wArg1^wArg2)&(wArg1^res)&0x8000)>>13)|NF;

	return res;
#else
	int res = wArg1 - wArg2 - (zF & CF);

	zF  = (((wArg1^res^wArg2)&0x1000)>>8)|((res>>16)&1)|((res&0x8000)>>8)|
		((res&65535)?0:ZF)|(((wArg1^wArg2)&(wArg1^res)&0x8000)>>13)|NF;

	return res;
#endif
}

static UINT16 Add_2 (UINT16 wArg1, UINT16 wArg2)
{
#ifdef BIG_FLAGS_ARRAY
	int res  = wArg1 + wArg2;
	zF = (zF&(SF|ZF|VF))|(((wArg1^res^wArg2)&0x1000)>>8)|((res>>16)&1);
	return res;
#else
	int res  = wArg1 + wArg2;
	zF = (zF&(SF|ZF|VF))|(((wArg1^res^wArg2)&0x1000)>>8)|((res>>16)&1);
	return res;
#endif
}

static void Add_1 (UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
	int res = zA + bArg;
	zF = rgfAdd[zA][bArg];
	zA = res;
#else
	int res = zA + bArg;
	zF=SZ[res&255]|((res&256)>>8)| ((zA^res^bArg)&HF)| (((bArg^zA^0x80)&(bArg^res)&0x80)>>5);
	zA=res;
#endif
}

static void Adc_1 (UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
	int res  = zA + bArg + (zF & CF);
	zF = rgfAddc[zA][bArg][zF & CF];
	zA = res;
#else
	int res  = zA + bArg + (zF & CF);
	zF = SZ[res & 255]|((res & 256)>>8)|((zA^res^bArg)&HF)|(((bArg^zA^0x80)&(bArg^res)&0x80)>>5);
	zA = res;
#endif
}

static void Sub_1 (UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
	int res  = zA - bArg;
	zF = rgfSub[zA][bArg];
	zA = res;
#else
	int res  = zA - bArg;
	zF = SZ[res&255]|((res&256)>>8)|NF|((zA^res^bArg)&HF)|(((bArg^zA)&(zA^res)&0x80)>>5);
	zA = res;
#endif
}

static void Sbc_1 (UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
	int res = zA - bArg - (zF & CF);
	zF = rgfSubc[zA][bArg][zF & CF];
	zA = res;
#else
	int res = zA - bArg - (zF & CF);
	zF=SZ[res&255]|((res&256)>>8)|NF|((zA^res^bArg)&HF)|(((bArg^zA)&(bArg^res)&0x80)>>5);
	zA=res;
#endif
}

static void And(UINT8 bArg)
{
	zF = SZP[zA &= bArg] | HF;
}

static void Or(UINT8 bArg)
{
	zF = SZP[zA |= bArg];
}

static void Xor(UINT8 bArg)
{
	zF = SZP[zA ^= bArg];
}

static void Cp(UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
//	int res = zA - bArg;
	zF = rgfSub[zA][bArg];
#else
	int res = zA - bArg;
	zF = SZ[res&255]|((res&256)>>8)|NF|((zA^res^bArg)&HF)|(((bArg^zA)&(bArg^res)&0x80)>>5);
#endif
}

static UINT8 myInc(UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
	bArg++;
	zF = (zF & CF) | SZHV_inc[bArg];
	return bArg;
#else
	bArg++;
	zF = (zF&CF)|SZ[bArg]|((bArg==0x80)?VF:0)|((bArg&0x0F)?0:HF);
	return bArg;
#endif
}

static UINT8 myDec(UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
	zF = (zF & CF) | SZHV_dec[bArg--];
	return bArg;
#else
	zF=(zF&CF)|NF|
		((bArg==0x80)?VF:0)|((bArg&0x0F)?0:HF);

	zF|=SZ[--bArg];

	return bArg;
#endif
}

// shouldn't even be members...

static void BIT(int nBit, UINT8 bArg)
{
#ifdef BIG_FLAGS_ARRAY
//	zF = (zF & CF) | rgfBit[bArg][nBit];
	zF = (zF & CF) | HF | SZ_BIT[bArg & (1<<nBit)];
#else
	zF = (zF & CF) | HF | ((bArg&(1<< nBit))? ((nBit==7)?SF:0):ZF);
#endif
}

#define BIT3	z80_ICount += 3;BIT


#if 1
#define BIT_XY3	z80_ICount += 3;BIT
#else
static void BIT_XY3(int nBit, UINT8 bArg)
{
	z80_ICount += 3;
#ifdef BIG_FLAGS_ARRAY
//	zF = (zF & CF) | rgfBit[bArg][nBit];
	zF = (zF & CF) | HF | SZ_BIT[bArg & (1<<nBit)];
//	zF = (zF & CF) | HF | (SZ_BIT[reg & (1<<bit)] & ~(YF|XF)) | ((EA>>8) & (YF|XF))

#else
	zF = (zF & CF) | HF | ((bArg&(1<< nBit))? ((nBit==7)?SF:0):ZF);
#endif
}
#endif
static UINT8 Rlc(UINT8 bArg)
{
	int res = bArg >> 7;
	bArg   = (bArg << 1) | res;
	zF = SZP[bArg] | res;
	return bArg;
}

static UINT8 Rrc(UINT8 bArg)
{
	int res = bArg & 1;
	bArg = (bArg >> 1) | (res << 7);
	zF = SZP[bArg]|res;
	return bArg;
}

static UINT8 Rl(UINT8 bArg)
{
	int res=bArg>>7;
	bArg=(bArg<<1)|(zF&1);
	zF=SZP[bArg]|res;
	return bArg;
}

static UINT8 Rr(UINT8 bArg)
{
	int res = bArg&1;
	bArg=(bArg>>1)|(zF<<7);
	zF=SZP[bArg]|res;
	return bArg;
}



/* ************************************
 * Z80 Utilities
 * ************************************
 */

//UINT16 swap_variable;
#define swap(b1,b2) swap_variable = b1; b1 = b2; b2 = swap_variable;
/* inline void swap ( UINT16 &b1, UINT16 &b2 ) {
 * UINT16 b = b1;
 * b1 = b2;
 * b2 = b;
 * }
 */



static void Rla() {
  int i  = zF&CF;
  zF = (zF&0xEC)|((zA&0x80)>>7);
  zA = (zA<<1)|i;
}

static void Rra() {
  int i = zF&CF;
  zF=(zF&0xEC)|(zA&0x01);
  zA=(zA>>1)|(i<<7);
}




/* ************************************
 * Flow of Control
 * ************************************
 */

static int Jr0(int f) {
	if(f)
	{
		AdjustPC((signed char)ImmedByte());
		return 5;
	}
	else
	{
		AdjustPC(1);
		return 0;
	}
}

static int Jr1(int f) {
	if(f)
	{
		AdjustPC(1);
		return 0;
	}
	else
	{
		AdjustPC((signed char)ImmedByte());
		return 5;
	}
}

static int Call0(int f) {
//	char string[150];

	if(f)
	{
		UINT16 wAddr = ARG16();
		Push(GetPC());
		SetPC(wAddr);
		return 7;
	}
	else
	{
		AdjustPC(2);
		return 0;
	}
}

static int Call1(int f) {
	if(f)
	{
		AdjustPC(2);
		return 0;
	}
	else
	{
		UINT16 wAddr = ARG16();
		Push(GetPC());
		SetPC(wAddr);
		return 7;
	}
}

static int Jp0(int f) {
	if(f)
	{
		SetPC(ARG16());
		return 0; // ????????????????
	}
	else
	{
		AdjustPC(2);
		return 0;
	}
}

static int Jp1(int f) {
	if(f)
	{
		AdjustPC(2);
		return 0;
	}
	else
	{
		SetPC(ARG16());
		return 0; // ????????????????
	}
}

static void Rst(UINT16 wAddr) {
	Push(GetPC());
	SetPC(wAddr);
}


static UINT8 Sla(UINT8 bArg)
{
	UINT8 c = bArg>>7;
	bArg<<=1;
	zF=SZP[bArg]|c;
	return bArg;
}

static UINT8 Sra(UINT8 bArg)
{
	UINT8 c = bArg&1;
	bArg = (bArg>>1)|(bArg&0x80);
	zF = SZP[bArg]|c;
	return bArg;
}

static UINT8 Sll(UINT8 bArg)
{
	UINT8 c = bArg>>7;
	bArg=(bArg<<1)|1;
	zF=SZP[bArg]|c;
	return bArg;
}

static UINT8 Srl(UINT8 bArg)
{
	UINT8 c = bArg&1;
	bArg>>=1;
	zF=SZP[bArg]|c;
	return bArg;
}
/* ************************************
 * CB prefixed instructions
 * ************************************
 */
//static __inline__ UINT8 Set(int nBit, UINT8 bArg)
//{
//	return (bArg | (1 << nBit));
//}
//static __inline__ UINT8 Res(int nBit, UINT8 bArg)
//{
//	return (bArg & ~(1 << nBit));
//}
static void HandleCB(void) {//aready4clk//ddddd
	const UINT8 bOpcode = ImmedByte();

	if(0x06==(bOpcode&0x07)){
	z80_ICount -= (15-4);
		volatile UINT8 RM16HL=RM(zHL);
		switch (bOpcode)
		{
			case 0x06:	WM(zHL, Rlc(RM16HL));	break;	// rlc_xhl
			case 0x0E:	WM(zHL, Rrc(RM16HL));	break;	// rrc_xhl
			case 0x16:	WM(zHL, Rl(RM16HL));	break;	// rl_xhl
			case 0x1E:	WM(zHL, Rr(RM16HL));	break;	// rr_xhl
			case 0x26:	WM(zHL, Sla(RM16HL));	break;	// sla_xhl
			case 0x2E:	WM(zHL, Sra(RM16HL));	break;	// sra_xhl
			case 0x36:	WM(zHL, Sll(RM16HL));	break;	// sll_xhl
			case 0x3E:	WM(zHL, Srl(RM16HL));	break;	// srl_xhl
			case 0x46:	BIT3(0, RM16HL);		break;	//	return 12; // bit_0_xhl
			case 0x4E:	BIT3(1, RM16HL);		break;	//	return 12; // bit_1_xhl
			case 0x56:	BIT3(2, RM16HL);		break;	//	return 12; // bit_2_xhl
			case 0x5E:	BIT3(3, RM16HL);		break;	//	return 12; // bit_3_xhl
			case 0x66:	BIT3(4, RM16HL);		break;	//	return 12; // bit_4_xhl
			case 0x6E:	BIT3(5, RM16HL);		break;	//	return 12; // bit_5_xhl
			case 0x76:	BIT3(6, RM16HL);		break;	//	return 12; // bit_6_xhl
			case 0x7E:	BIT3(7, RM16HL);		break;	//	return 12; // bit_7_xhl
			case 0x86:	WM(zHL, (RM16HL & ((~(1<<0))&0xff)));	break;	// res_0_xhl Res()
			case 0x8E:	WM(zHL, (RM16HL & ((~(1<<1))&0xff)));	break;	// res_1_xhl
			case 0x96:	WM(zHL, (RM16HL & ((~(1<<2))&0xff)));	break;	// res_2_xhl
			case 0x9E:	WM(zHL, (RM16HL & ((~(1<<3))&0xff)));	break;	// res_3_xhl
			case 0xA6:	WM(zHL, (RM16HL & ((~(1<<4))&0xff)));	break;	// res_4_xhl
			case 0xAE:	WM(zHL, (RM16HL & ((~(1<<5))&0xff)));	break;	// res_5_xhl
			case 0xB6:	WM(zHL, (RM16HL & ((~(1<<6))&0xff)));	break;	// res_6_xhl
			case 0xBE:	WM(zHL, (RM16HL & ((~(1<<7))&0xff)));	break;	// res_7_xhl
			case 0xC6:	WM(zHL, (RM16HL |	 (1<<0) 	  ));	break;	// set_0_xhl Set()
			case 0xCE:	WM(zHL, (RM16HL |	 (1<<1) 	  ));	break;	// set_1_xhl
			case 0xD6:	WM(zHL, (RM16HL |	 (1<<2) 	  ));	break;	// set_2_xhl
			case 0xDE:	WM(zHL, (RM16HL |	 (1<<3) 	  ));	break;	// set_3_xhl
			case 0xE6:	WM(zHL, (RM16HL |	 (1<<4) 	  ));	break;	// set_4_xhl
			case 0xEE:	WM(zHL, (RM16HL |	 (1<<5) 	  ));	break;	// set_5_xhl
			case 0xF6:	WM(zHL, (RM16HL |	 (1<<6) 	  ));	break;	// set_6_xhl
			case 0xFE:	WM(zHL, (RM16HL |	 (1<<7) 	  ));	break;	// set_7_xhl
		}
	//	return 15;
	}else{
	z80_ICount -= (8-4);
		switch(bOpcode&0xc0)
		{
		case 0x00:	/* 0x00-0x3f */
			switch(bOpcode)
			{
			case 0x00:	zB = Rlc(zB);	break;	// rlc_b
			case 0x01:	zC = Rlc(zC);	break;	// rlc_c
			case 0x02:	zD = Rlc(zD);	break;	// rlc_d
			case 0x03:	zE = Rlc(zE);	break;	// rlc_e
			case 0x04:	zH = Rlc(zH);	break;	// rlc_h
			case 0x05:	zL = Rlc(zL);	break;	// rlc_l
			case 0x07:	zA = Rlc(zA);	break;	// rlc_a
			case 0x08:	zB = Rrc(zB);	break;	// rrc_b
			case 0x09:	zC = Rrc(zC);	break;	// rrc_c
			case 0x0A:	zD = Rrc(zD);	break;	// rrc_d
			case 0x0B:	zE = Rrc(zE);	break;	// rrc_e
			case 0x0C:	zH = Rrc(zH);	break;	// rrc_h
			case 0x0D:	zL = Rrc(zL);	break;	// rrc_l
			case 0x0F:	zA = Rrc(zA);	break;	// rrc_a
			case 0x10:	zB = Rl(zB);	break;	// rl_b
			case 0x11:	zC = Rl(zC);	break;	// rl_c
			case 0x12:	zD = Rl(zD);	break;	// rl_d
			case 0x13:	zE = Rl(zE);	break;	// rl_e
			case 0x14:	zH = Rl(zH);	break;	// rl_h
			case 0x15:	zL = Rl(zL);	break;	// rl_l
			case 0x17:	zA = Rl(zA);	break;	// rl_a
			case 0x18:	zB = Rr(zB);	break;	// rr_b
			case 0x19:	zC = Rr(zC);	break;	// rr_c
			case 0x1A:	zD = Rr(zD);	break;	// rr_d
			case 0x1B:	zE = Rr(zE);	break;	// rr_e
			case 0x1C:	zH = Rr(zH);	break;	// rr_h
			case 0x1D:	zL = Rr(zL);	break;	// rr_l
			case 0x1F:	zA = Rr(zA);	break;	// rr_a

			case 0x20:	zB = Sla(zB);	break;	// sla_b
			case 0x21:	zC = Sla(zC);	break;	// sla_c
			case 0x22:	zD = Sla(zD);	break;	// sla_d
			case 0x23:	zE = Sla(zE);	break;	// sla_e
			case 0x24:	zH = Sla(zH);	break;	// sla_h
			case 0x25:	zL = Sla(zL);	break;	// sla_l
			case 0x27:	zA = Sla(zA);	break;	// sla_a

			case 0x28:	zB = Sra(zB);	break;	// sra_b
			case 0x29:	zC = Sra(zC);	break;	// sra_c
			case 0x2A:	zD = Sra(zD);	break;	// sra_d
			case 0x2B:	zE = Sra(zE);	break;	// sra_e
			case 0x2C:	zH = Sra(zH);	break;	// sra_h
			case 0x2D:	zL = Sra(zL);	break;	// sra_l
			case 0x2F:	zA = Sra(zA);	break;	// sra_a

			case 0x30:	zB = Sll(zB);	break;	// sll_b
			case 0x31:	zC = Sll(zC);	break;	// sll_c
			case 0x32:	zD = Sll(zD);	break;	// sll_d
			case 0x33:	zE = Sll(zE);	break;	// sll_e
			case 0x34:	zH = Sll(zH);	break;	// sll_h
			case 0x35:	zL = Sll(zL);	break;	// sll_l
			case 0x37:	zA = Sll(zA);	break;	// sll_a

			case 0x38:	zB = Srl(zB);	break;	// srl_b
			case 0x39:	zC = Srl(zC);	break;	// srl_c
			case 0x3A:	zD = Srl(zD);	break;	// srl_d
			case 0x3B:	zE = Srl(zE);	break;	// srl_e
			case 0x3C:	zH = Srl(zH);	break;	// srl_h
			case 0x3D:	zL = Srl(zL);	break;	// srl_l
			case 0x3F:	zA = Srl(zA);	break;	// srl_a
			}
			break;
		case 0x40:	/* 0x40-0x7f */
			{/* 0011 1000 */
			unsigned char bit_pattarn;
//			bit_pattarn =(1<<((bOpcode&0x38)>>3));
			bit_pattarn =(   ((bOpcode&0x38)>>3));
				switch(bOpcode&0x07){
			case 0x00:	BIT(bit_pattarn, zB);	break;	// bit_#_b
			case 0x01:	BIT(bit_pattarn, zC);	break;	// bit_#_c
			case 0x02:	BIT(bit_pattarn, zD);	break;	// bit_#_d
			case 0x03:	BIT(bit_pattarn, zE);	break;	// bit_#_e
			case 0x04:	BIT(bit_pattarn, zH);	break;	// bit_#_h
			case 0x05:	BIT(bit_pattarn, zL);	break;	// bit_#_l
			case 0x07:	BIT(bit_pattarn, zA);	break;	// bit_#_a
				}
			}
			break;
		case 0x80:	/* 0x80-0xbf */
			{
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((bOpcode&0x38)>>3));
			bit_pattarn = (~bit_pattarn);
				switch(bOpcode&0x07){
			case 0x00:	zB &= bit_pattarn;	break;	// res_#_b Res()
			case 0x01:	zC &= bit_pattarn;	break;	// res_#_c
			case 0x02:	zD &= bit_pattarn;	break;	// res_#_d
			case 0x03:	zE &= bit_pattarn;	break;	// res_#_e
			case 0x04:	zH &= bit_pattarn;	break;	// res_#_h
			case 0x05:	zL &= bit_pattarn;	break;	// res_#_l
			case 0x07:	zA &= bit_pattarn;	break;	// res_#_a
				}
			}
			break;
		case 0xc0:	/* 0xc0-0xff */
			{
			unsigned char bit_pattarn;
			bit_pattarn =(1<<((bOpcode&0x38)>>3));
				switch(bOpcode&0x07){
			case 0x00:	zB |= bit_pattarn;	break;	// set_#_b Set()
			case 0x01:	zC |= bit_pattarn;	break;	// set_#_c
			case 0x02:	zD |= bit_pattarn;	break;	// set_#_d
			case 0x03:	zE |= bit_pattarn;	break;	// set_#_e
			case 0x04:	zH |= bit_pattarn;	break;	// set_#_h
			case 0x05:	zL |= bit_pattarn;	break;	// set_#_l
			case 0x07:	zA |= bit_pattarn;	break;	// set_#_a
				}
			}
			break;
		}
	//	return 8;
	}//ends of else//

	//	default:
	//		//_ASSERT(FALSE);
//	return 8;//			return 0;
}


/* ************************************
 * ED prefixed opcodes
 * ************************************
 */

static void HandleED(void) {//aready4clk//ddddd
	const UINT8 bOpcode = ImmedByte();

	switch (bOpcode)
	{
//		case 0x00:	/*z80_ICount -= 0;*/	break; // nop
//...
//		case 0x3F:	/*z80_ICount -= 0;*/	break; // nop

		case 0x40:	z80_ICount -= (12-4);	zB = In(zC);		break; //ed: in_b_c
		case 0x48:	z80_ICount -= (12-4);	zC = In(zC);		break; //ed: in_c_c
		case 0x50:	z80_ICount -= (12-4);	zD = In(zC);		break; //ed: in_d_c
		case 0x58:	z80_ICount -= (12-4);	zE = In(zC);		break; //ed: in_e_c
		case 0x60:	z80_ICount -= (12-4);	zH = In(zC);		break; //ed: in_h_c
		case 0x68:	z80_ICount -= (12-4);	zL = In(zC);		break; //ed: in_l_c
		case 0x70:	z80_ICount -= (12-4);	In(zC);				break; //ed: in_0_c
		case 0x78:	z80_ICount -= (12-4);	zA = In(zC);		break; //ed: in_a_c
/////
		case 0x41:	z80_ICount -= (12-4);	Out(zBC, zB);		break; //ed: out_c_b
		case 0x49:	z80_ICount -= (12-4);	Out(zBC, zC);		break; //ed: out_c_c
		case 0x51:	z80_ICount -= (12-4);	Out(zBC, zD);		break; //ed: out_c_d
		case 0x59:	z80_ICount -= (12-4);	Out(zBC, zE);		break; //ed: out_c_e
		case 0x61:	z80_ICount -= (12-4);	Out(zBC, zH);		break; //ed: out_c_h
		case 0x69:	z80_ICount -= (12-4);	Out(zBC, zL);		break; //ed: out_c_l
		case 0x71:	z80_ICount -= (12-4);	Out(zBC, 0);		break; //ed: out_c_0
		case 0x79:	z80_ICount -= (12-4);	Out(zBC, zA);		break; //ed: out_c_a
/////
		case 0x42:	z80_ICount -= (15-4);	zHL = Sbc_2(zHL, zBC);		break; //ed: sbc_hl_bc
		case 0x4A:	z80_ICount -= (15-4);	zHL = Adc_2(zHL, zBC);		break; //ed: adc_hl_bc
		case 0x52:	z80_ICount -= (15-4);	zHL = Sbc_2(zHL, zDE);		break; //ed: sbc_hl_de
		case 0x5A:	z80_ICount -= (15-4);	zHL = Adc_2(zHL, zDE);		break; //ed: adc_hl_de
		case 0x62:	z80_ICount -= (15-4);	zHL = Sbc_2(zHL, zHL);		break; //ed: sbc_hl_hl
		case 0x6A:	z80_ICount -= (15-4);	zHL = Adc_2(zHL, zHL);		break; //ed: adc_hl_hl
		case 0x72:	z80_ICount -= (15-4);	zHL = Sbc_2(zHL, GetSP());	break; //ed: sbc_hl_sp
		case 0x7A:	z80_ICount -= (15-4);	zHL = Adc_2(zHL, GetSP());	break; //ed: adc_hl_sp
/////
		case 0x43:	z80_ICount -= (20-4);	MemWriteWord(ARG16(), zBC);		break; //ed: ld_xword_bc
		case 0x4B:	z80_ICount -= (20-4);	zBC = MemReadWord(ARG16());		break; //ed: ld_bc_xword
		case 0x53:	z80_ICount -= (20-4);	MemWriteWord(ARG16(), zDE);		break; //ed: ld_xword_de
		case 0x5B:	z80_ICount -= (20-4);	zDE = MemReadWord(ARG16());		break; //ed: ld_de_xword
		case 0x63:	z80_ICount -= (20-4);	MemWriteWord(ARG16(), zHL);		break; //ed: ld_xword_hl
		case 0x6B:	z80_ICount -= (20-4);	zHL = MemReadWord(ARG16());		break; //ed: ld_hl_xword
		case 0x73:	z80_ICount -= (20-4);	MemWriteWord(ARG16(), GetSP());	break; //ed: ld_xword_sp
		case 0x7B:	z80_ICount -= (20-4);	SetSP(MemReadWord(ARG16()));	break; //ed: ld_sp_xword
/////
		case 0x44: //ed: neg
		case 0x4C: //ed: neg
		case 0x54: //ed: neg
		case 0x5C: //ed: neg
		case 0x64: //ed: neg
		case 0x6C: //ed: neg
		case 0x74: //ed: neg
		case 0x7C: //ed: neg
			z80_ICount -= (8-4);
			{
			UINT8 b = zA;
				zA = 0;
				Sub_1(b);
			}
			break;
/////
		case 0x45://zIFF1 = zIFF2;	z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: retn
		case 0x55://zIFF1 = zIFF2;	z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: retn
		case 0x65://zIFF1 = zIFF2;	z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: retn
		case 0x75://zIFF1 = zIFF2;	z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: retn
			z80_ICount -= (8-4);
			if( zIFF1 == 0 && zIFF2 == 1 ){
				zIFF1 =1;
//				if( Z80.irq_state != CLEAR_LINE )
//				{
//					take_interrupt();
//				}
			}else{
				zIFF1 = zIFF2;
				z80_ICount -= 6;Ret0_TRUE;
			}
			break;
//
		case 0x4D://z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: reti
		case 0x5D://z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: reti
		case 0x6D://z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: reti
		case 0x7D:	z80_ICount -= (8+6-4);	/*	z80_ICount -= 6;*/Ret0_TRUE;	break; //ed: reti
/////
		case 0x46://z80_ICount -= (8-4);	zIM = 0;	break; //ed: im_0
		case 0x66://z80_ICount -= (8-4);	zIM = 0;	break; //ed: im_0
		case 0x4E://z80_ICount -= (8-4);	zIM = 0;	break; //ed: im_0
		case 0x6E:	z80_ICount -= (8-4);	zIM = 0;	break; //ed: im_0
		case 0x56://z80_ICount -= (8-4);	zIM = 1;	break; //ed: im_1
		case 0x76:	z80_ICount -= (8-4);	zIM = 1;	break; //ed: im_1
		case 0x5E://z80_ICount -= (8-4);	zIM = 2;	break; //ed: im_2
		case 0x7E:	z80_ICount -= (8-4);	zIM = 2;	break; //ed: im_2
/////
		case 0x47: //ed: ld_i_a
			z80_ICount -= (9-4);
			zI = zA;
			break;
		case 0x4F: //ed: ld_r_a
			z80_ICount -= (9-4);
//			zR = zA;
//			zR2 = zA;
			break;
		case 0x57: //ed: ld_a_i
			z80_ICount -= (9-4);
			zA = zI;
			zF = (zF & CF)| SZ[zI] | (zIFF2 << 2);
			break;
		case 0x5F: //ed: ld_a_r
			z80_ICount -= (9-4);
//			zA = (zR & 0x7F); /////////////////////////////////////// | (zR2 & 0x80);
			zA = (zA+7)&0xff;
			zF = (zF & CF) | SZ[zA] | (zIFF2 << 2);
			break;
		case 0x67: //ed: rrd
			z80_ICount -= (18-4);
		{
			UINT8 i = RM(zHL);
			WM(zHL, (i>>4) | (zA << 4));
			zA = (zA & 0xF0) | (i&0x0F);
			zF =(zF&CF)|SZP[zA];
			break;
		}
		case 0x6F: //ed: rld
			z80_ICount -= (18-4);
		{
			UINT8 i;
			i=RM(zHL);
			WM(zHL,(i<<4)|(zA&0x0F));
			zA=(zA&0xF0)|(i>>4);
			zF=(zF&CF)|SZP[zA];
			break;
		}
		case 0x77: //ed: nop
			z80_ICount -= (8-4);
			break;
//		case 0x7F:	/*z80_ICount -= 0;*/	break; //ed: nop





////////////////////
#if 1
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
#define LDI {\
			WM(zDE, RM(zHL));\
			zHL++; zDE++; zBC--;\
			zF = (zF & 0xE9) | (zBC ? VF : 0);\
		}
#endif
#if 1
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
//		case 0x80:	/*z80_ICount -= 0;*/	break; //ed: nop
//...
//		case 0x9F:	/*z80_ICount -= 0;*/	break; //ed: nop

		case 0xA0: //ed: ldi
			z80_ICount -= (16-4);
			LDI
			break;

		case 0xA1: //ed: cpi
			z80_ICount -= (16-4);
#if 00
		{
			UINT8 i,j;
			i=RM(zHL);
			j=zA-i;
			zHL++;
			zBC--;
			zF=(zF&CF)|SZ[j]|
			((zA^i^j)&HF)|(zBC? VF:0)|NF;
		}
#endif
			CPI
			break;

		case 0xA2: //ed: ini
			z80_ICount -= (16-4);
		{
			zB--;
			WM(zHL,In(zBC));
			zHL++;
			zF=(zB)? NF:(NF|ZF);
		}
			break;

		case 0xA3: //ed: outi
			z80_ICount -= (16-4);
		{
			zB--;
			Out(zBC, RM(zHL));
			zHL++;
			zF=(zB)? NF:(ZF|NF);
		}
			break;

//		case 0xA4: //ed: nop
//		case 0xA5: //ed: nop
//		case 0xA6: //ed: nop
//		case 0xA7: //ed: nop
//			z80_ICount -= (8-4);
//			break;

		case 0xA8: //ed: ldd
			z80_ICount -= (16-4);
		{
			WM(zDE,RM(zHL));
			zDE--;
			zHL--;
			zBC--;
			zF=(zF&0xE9)|(zBC? VF:0);
		}
			break;

		case 0xA9: //ed: cpd
			z80_ICount -= (16-4);
		{
			UINT8 i,j;
			i=RM(zHL);
			j=zA-i;
			zHL--;
			zBC--;
			zF=(zF&CF)|SZ[j]|
			((zA^i^j)&HF)|(zBC? VF:0)|NF;
		}
			break;

		case 0xAA: //ed: ind
			z80_ICount -= (16-4);
		{
			zB--;
			WM(zHL,In(zBC));
			zHL--;
			zF=(zB)? NF:(NF|ZF);
		}
			break;

		case 0xAB: //ed: outd
			z80_ICount -= (16-4);
		{
			zB--;
			Out(zBC,RM(zHL));
			zHL--;
			zF=(zB)? NF:(ZF|NF);
		}
			break;

//		case 0xAC: //ed: nop
//		case 0xAD: //ed: nop
//		case 0xAE: //ed: nop
//		case 0xAF: //ed: nop
//			z80_ICount -= (8-4);
//			break;

		case 0xB0: //ed: ldir
			z80_ICount -= (16-4);
		{
/*
			if(m_rgfMemDesc[zHL] & MF_READ_DIRECT &&
				m_rgfMemDesc[zDE] & MF_WRITE_DIRECT)
			{
				do
				{
					m_rgbMemory[zDE++] = m_rgbMemory[zHL++];
					z80_ICount -= 21;
				}
				while (--zBC); //  && z80_ICount>0);

				zF=(zF&0xE9)|(zBC? VF:0);

				if(zBC)
	//				m_regPC -= 2;
					AdjustPC(-2);
				else
				//	z80_ICount += 5;
					z80_ICount -= 5;

			}
			else
*/
			{
#if 00
				//do
				{
					WM(zDE, RM(zHL));
					zHL++; zDE++; zBC--;
				//	z80_ICount -= 21;
				}
				//while (zBC); //  && z80_ICount>0);
				zF=(zF&0xE9)|(zBC? VF:0);
#endif
				LDI
				if(zBC){
	//				m_regPC -= 2;
					AdjustPC(-2);
					z80_ICount -= 5;
				}
			}

		}
			break;

		case 0xB1: //ed: cpir
			z80_ICount -= (16-4);
		{
#if 00
			UINT8 i,j;
			//do
			{
				i=RM(zHL);
				j=zA-i;
				zHL++;
				zBC--;
			//	z80_ICount-=21;
			}
			//while (zBC && j && z80_ICount>0);
			zF=(zF&CF)|SZ[j]|
			((zA^i^j)&HF)|(zBC? VF:0)|NF;
#endif
		CPI
		//	if(zBC && j){
			if( zBC && !(zF & ZF) ){
				AdjustPC(-2); // m_regPC-=2;
				z80_ICount-=5;
			}
		}
			break;

		case 0xB2: //ed: inir
			z80_ICount -= (16-4);
		{
			//do
			{
				zB--;
				WM(zHL,In(zBC));
				zHL++;
			//	z80_ICount-=21;
			}
			//while (zB && z80_ICount>0);
			zF=(zB)? NF:(NF|ZF);
			if(zB){
				AdjustPC(-2); // m_regPC-=2;
				z80_ICount-=5;
			}
		}
			break;

		case 0xB3: //ed: otir
			z80_ICount -= (16-4);
		{
			//do
			{
				zB--;
				Out(zBC,RM(zHL));
				zHL++;
			//	z80_ICount-=21;
			}
			//while (zB && z80_ICount>0);
			zF=(zB)? NF:(ZF|NF);
			if(zB){
				AdjustPC(-2); // m_regPC-=2;
				z80_ICount-=5;
			}
		}
			break;

//		case 0xB4: //ed: nop
//		case 0xB5: //ed: nop
//		case 0xB6: //ed: nop
//		case 0xB7: //ed: nop
//			z80_ICount -= (8-4);
//			break;

		case 0xB8: //ed: lddr
			z80_ICount -= (16-4);
		{
			//do
			{
				WM(zDE,RM(zHL));
				zHL--;
				zDE--;
				zBC--;
			//	z80_ICount-=21;
			}
			//while (zBC);
			zF=(zF&0xE9)|(zBC? VF:0);
			if(zBC){
				AdjustPC(-2); // m_regPC-=2;
				z80_ICount-=5;
			}
		}
			break;

		case 0xB9: //ed: cpdr
			z80_ICount -= (16-4);
		{
			UINT8 i,j;
			//do
			{
				i=RM(zHL);
				j=zA-i;
				zHL--;
				zBC--;
			//	z80_ICount-=21;
			}
			//while (zBC && j && z80_ICount>0);
			zF=(zF&CF)|SZ[j]|
			((zA^i^j)&HF)|(zBC? VF:0)|NF;
		//	if(zBC && j){
			if( zBC && !(zF & ZF) ){
				AdjustPC(-2); // m_regPC-=2;
				z80_ICount-=5;
			}
		}
			break;

		case 0xBA: //ed: indr
			z80_ICount -= (16-4);
		{
			//do
			{
				zB--;
				WM(zHL,In(zBC));
				zHL--;
			//	z80_ICount-=21;
			}
			//while (zB && z80_ICount>0);
			zF=(zB)? NF:(NF|ZF);
			if(zB){
				AdjustPC(-2); // m_regPC-=2;
				z80_ICount-=5;
			}
		}
			break;

		case 0xBB: //ed: otdr
			z80_ICount -= (16-4);
		{
			//do
			{
				zB--;
				Out(zBC,RM(zHL));
				zHL--;
			//	z80_ICount-=21;
			}
			//while (zB && z80_ICount>0);
			zF=(zB)? NF:(ZF|NF);
			if(zB){
				AdjustPC(-2); // m_regPC-=2;
				z80_ICount-=5;
			}
		}
			break;

//		case 0xBC: //ed: nop
//		case 0xBD: //ed: nop
//		case 0xBE: //ed: nop
//		case 0xBF: //ed: nop
//...
//		case 0xF8: //ed: nop
//		case 0xF9: //ed: nop
//		case 0xFA: //ed: nop
//		case 0xFB: //ed: nop
//		case 0xFC: //ed: nop
//		case 0xFD: //ed: nop
//		case 0xFE: //ed: patch
//		case 0xFF: //ed: nop
//			/*z80_ICount -= 0;*/
//			break;

		default:
//			//_ASSERT(FALSE);
			z80_ICount -= (8-4);
			break;
	}

//	return ;//(z80_ICountArg - z80_ICount);
}







/////////////////
// FDCB / DDCB prefixed opcodes
//static int HandleDDCB() {
//static int HandleFDCB() {
static void HandleIXIY(void){//aready 4clk//ddddd
 UINT16 wAddr;
unsigned char bTemp1;


	wAddr =(zIX + (signed char)ImmedByte());
	bTemp1 = RM(wAddr);

	//z80_ICount -= 4;//aready 4clk//ddddd

	z80_ICount -= (23-4);//(4+16+3 ?)
	{
	const UINT8 m_opcode= ImmedByte();

	if(0x40==(m_opcode & 0xc0)) /* 0x40-0x7f */
	{
//		z80_ICount -= (20+3-4);//(4+16 ?)
		BIT_XY3( ((m_opcode>>3)&7), bTemp1);
#if 00
		case 0x40: // bit_0_xix / bit_0_xiy
		// 0x41: 0x42: 0x43: 0x44: 0x45: 0x46: 0x47:
			BIT_XY3(0, RM(wAddr));	break;
		case 0x48: // bit_1_xix / bit_1_xiy
		// 0x49: 0x4a: 0x4b: 0x4c: 0x4d: 0x4e: 0x4f:
			BIT_XY3(1, RM(wAddr));	break;
		case 0x50: // bit_2_xix / bit_2_xiy
		// 0x51: 0x52: 0x53: 0x54: 0x55: 0x56: 0x57:
			BIT_XY3(2, RM(wAddr));	break;
		case 0x58: // bit_3_xix / bit_3_xiy
// 0x59: 0x5a: 0x5b: 0x5c: 0x5d: 0x5e: 0x5f:
			BIT_XY3(3, RM(wAddr));	break;
		case 0x60: // bit_4_xix / bit_4_xiy
// 0x61: 0x62: 0x63: 0x64: 0x65: 0x66: 0x67:
			BIT_XY3(4, RM(wAddr));	break;
		case 0x68: // bit_5_xix / bit_5_xiy
// 0x69: 0x6a: 0x6b: 0x6c: 0x6d: 0x6e: 0x6f:
			BIT_XY3(5, RM(wAddr));	break;
		case 0x70: // bit_6_xix / bit_6_xiy
// 0x71: 0x72: 0x73: 0x74: 0x75: 0x76: 0x77:
			BIT_XY3(6, RM(wAddr));	break;
		case 0x78: // bit_7_xix / bit_7_xiy
// 0x79: 0x7a: 0x7b: 0x7c: 0x7d: 0x7e: 0x7f:
			BIT_XY3(7, RM(wAddr));	break;
#endif

	}else if(0x06==(m_opcode & 0x07)){ /* 0x#6 or 0x#e */
//		z80_ICount -= (23-4);//(4+16+3 ?)
	switch (m_opcode & 0xf8)
	{
		case (0x06 & 0xf8):	bTemp1=Rlc(bTemp1);	break; //DDCB/FDCB: rlc_xix / rlc_xiy
		case (0x0e & 0xf8):	bTemp1=Rrc(bTemp1);	break; //DDCB/FDCB: rrc_xiy
		case (0x16 & 0xf8):	bTemp1=Rl(bTemp1);	break; //DDCB/FDCB: rl_xiy
		case (0x1e & 0xf8):	bTemp1=Rr(bTemp1);	break; //DDCB/FDCB: rr_xiy
		case (0x26 & 0xf8):	bTemp1=Sla(bTemp1);	break; //DDCB/FDCB: sla_xiy
		case (0x2e & 0xf8):	bTemp1=Sra(bTemp1);	break; //DDCB/FDCB: sra_xiy
		case (0x36 & 0xf8):	bTemp1=Sll(bTemp1);	break; //DDCB/FDCB: sll_xiy
		case (0x3e & 0xf8):	bTemp1=Srl(bTemp1);	break; //DDCB/FDCB: srl_xiy

		case (0x86 & 0xf8):	bTemp1 &= 0xfe;	break; //DDCB/FDCB: res_0_xiy//??????
		case (0x8e & 0xf8):	bTemp1 &= 0xfd;	break; //DDCB/FDCB: res_1_xiy//??????
		case (0x96 & 0xf8):	bTemp1 &= 0xfb;	break; //DDCB/FDCB: res_2_xiy//??????
		case (0x9e & 0xf8):	bTemp1 &= 0xf7;	break; //DDCB/FDCB: res_3_xiy
		case (0xa6 & 0xf8):	bTemp1 &= 0xef;	break; //DDCB/FDCB: res_4_xiy
		case (0xae & 0xf8):	bTemp1 &= 0xdf;	break; //DDCB/FDCB: res_5_xiy
		case (0xb6 & 0xf8):	bTemp1 &= 0xbf;	break; //DDCB/FDCB: res_6_xiy
		case (0xbe & 0xf8):	bTemp1 &= 0x7f;	break; //DDCB/FDCB: res_7_xiy

		case (0xc6 & 0xf8):	bTemp1 |= 0x01;	break; //DDCB/FDCB: set_0_xiy
		case (0xce & 0xf8):	bTemp1 |= 0x02;	break; //DDCB/FDCB: set_1_xiy
		case (0xd6 & 0xf8):	bTemp1 |= 0x04;	break; //DDCB/FDCB: set_2_xiy
		case (0xde & 0xf8):	bTemp1 |= 0x08;	break; //DDCB/FDCB: set_3_xiy
		case (0xe6 & 0xf8):	bTemp1 |= 0x10;	break; //DDCB/FDCB: set_4_xiy
		case (0xee & 0xf8):	bTemp1 |= 0x20;	break; //DDCB/FDCB: set_5_xiy
		case (0xf6 & 0xf8):	bTemp1 |= 0x40;	break; //DDCB/FDCB: set_6_xiy
		case (0xfe & 0xf8):	bTemp1 |= 0x80;	break; //DDCB/FDCB: set_7_xiy
	}
	WM(wAddr, bTemp1 );
	}else{
	//		z80_ICount -= 9; //適当でも少しかかる筈
	//		z80_ICount -= 8; //解からんが少し変えた。(4[pre-fhech]+4[nop?])仮に最速でも８クロック？？
			// DDxxでも、FDxxでもプリフェチが４クロックなら。ここに来るなら、当然４クロックは消費。(gens)
//			z80_ICount -= (4-4+16); //もしかして。（多分こっち、「ぷよぷよ１」の勝った後の動作、少し良くなるタブン）
	z80_ICount +=(23-4 -16);
	}
	}
}


/* ************************************
 * DD prefixed opcodes
 * ************************************
 */
static   UINT16 IndirectIX() {
	return zIX + (signed char)ImmedByte();
}
/* ************************************
 * FD prefixed opcodes
 * ************************************
 */
//static UINT16 IndirectIY() {
//	return zIY + (signed char)ImmedByte();
//}
//#define illegal_1()  SetPC(GetPC() - 1);
#define illegal_1()  ;
#define illegal_2()  ;

static void HandleDD(void) { /* (HandleFD) *///aready 4 clk//ddddd
	UINT16 wAddr;
	UINT16 wTemp;

	const UINT8 bOpcode = ImmedByte();
//	zR++;
	switch (bOpcode)
	{
//		case 0x00: // nop
//		case 0x01: // nop
//		case 0x02: // nop
//		case 0x03: // nop
//		case 0x04: // nop
//		case 0x05: // nop
//		case 0x06: // nop
//		case 0x07: // nop
//		case 0x08: // nop
		case 0x09:	zIX = Add_2(zIX, zBC);		z80_ICount -=  (15-4);break; // add_ix_bc
//		case 0x0A: // nop
//		case 0x0B: // nop
//		case 0x0C: // nop
//		case 0x0D: // nop
//		case 0x0E: // nop
//		case 0x0F: // nop
//		case 0x10: // nop
//		case 0x11: // nop
//		case 0x12: // nop
//		case 0x13: // nop
//		case 0x14: // nop
//		case 0x15: // nop
//		case 0x16: // nop
//		case 0x17: // nop
//		case 0x18: // nop
		case 0x19:	zIX = Add_2(zIX, zDE);		z80_ICount -=  (15-4);break; // add_ix_de

//		case 0x1A: // nop
//		case 0x1B: // nop
//		case 0x1C: // nop
//		case 0x1D: // nop
//		case 0x1E: // nop
//		case 0x1F: // nop
//		case 0x20: // nop
		case 0x21:	zIX = ARG16();				z80_ICount -= (14-4);break; // ld_ix_word
		case 0x22:	MemWriteWord(ARG16(), zIX);	z80_ICount -= (20-4);break; // ld_xword_ix
		case 0x23:	zIX++;							z80_ICount -= (10-4);break; // inc_ix
		case 0x24:	zIXh = myInc(zIXh);				z80_ICount -= (9-4);break; // inc_ixh
		case 0x25:	zIXh = myDec(zIXh);				z80_ICount -= (9-4);break; // dec_ixh
		case 0x26:	zIXh = ImmedByte();				z80_ICount -= (9-4);break; // ld_ixh_byte
//		case 0x27: // nop
//		case 0x28: // nop
		case 0x29:	zIX = Add_2(zIX, zIX);			z80_ICount -= (15-4);break; // add_ix_ix
		case 0x2A:	zIX = MemReadWord(ARG16());	z80_ICount -= (20-4);break; // ld_ix_xword
		case 0x2B:	zIX--;							z80_ICount -= (10-4);break; // dec_ix
		case 0x2C:	zIXl = myInc(zIXl);				z80_ICount -= (9-4);break; // inc_ixl
		case 0x2D:	zIXl = myDec(zIXl);				z80_ICount -= (9-4);break; // dec_ixl
		case 0x2E:	zIXl = ImmedByte();				z80_ICount -= (9-4);break; // ld_ixl_byte
//		case 0x2F: // nop

//		case 0x30: // nop
//		case 0x31: // nop
//		case 0x32: // nop
//		case 0x33: // nop
		case 0x34:
			wAddr = IndirectIX();
			WM(wAddr, myInc(RM(wAddr)));
			z80_ICount -= (23-4);break; // inc_xix

		case 0x35:
			wAddr = IndirectIX();
			WM(wAddr, myDec(RM(wAddr)));
			z80_ICount -= (23-4);break; // dec_xix

		case 0x36:
			wAddr = IndirectIX();
			WM(wAddr, ImmedByte());
			z80_ICount -= (19-4);break; // ld_xix_byte

//		case 0x37: // nop
//		case 0x38: // nop

		case 0x39:
			zIX = Add_2(zIX, GetSP());
			z80_ICount -= (15-4);break; // add_ix_sp

//		case 0x3A: // nop
//		case 0x3B: // nop
//		case 0x3C: // nop
//		case 0x3D: // nop
//		case 0x3E: // nop
//		case 0x3F: // nop

//		case 0x40: // nop
//		case 0x41: // nop
//		case 0x42: // nop
//		case 0x43: // nop
		case 0x44:	zB = zIXh;							z80_ICount -=  (9-4);break; // ld_b_ixh
		case 0x45:	zB = zIXl;							z80_ICount -=  (9-4);break; // ld_b_ixl
		case 0x46:	zB = RM(IndirectIX());	z80_ICount -= (19-4);break; // ld_b_xix
//		case 0x47: // nop
//		case 0x48: // nop
//		case 0x49: // nop
//		case 0x4A: // nop
//		case 0x4B: // nop
		case 0x4C:	zC = zIXh;							z80_ICount -=  (9-4);break; // ld_c_ixh
		case 0x4D:	zC = zIXl;							z80_ICount -=  (9-4);break; // ld_c_ixl
		case 0x4E:	zC = RM(IndirectIX());	z80_ICount -= (19-4);break; // ld_c_xix
//		case 0x4F: // nop

//		case 0x50: // nop
//		case 0x51: // nop
//		case 0x52: // nop
//		case 0x53: // nop
		case 0x54:	zD = zIXh;							z80_ICount -=  (9-4);break; // ld_d_ixh
		case 0x55:	zD = zIXl;							z80_ICount -=  (9-4);break; // ld_d_ixl
		case 0x56:	zD = RM(IndirectIX());	z80_ICount -= (19-4);break; // ld_d_xix

//		case 0x57: // nop
//		case 0x58: // nop
//		case 0x59: // nop
//		case 0x5A: // nop
//		case 0x5B: // nop
		case 0x5C:	zE = zIXh;							z80_ICount -=  (9-4);break; // ld_e_ixh
		case 0x5D:	zE = zIXl;							z80_ICount -=  (9-4);break; // ld_e_ixl
		case 0x5E:	zE = RM(IndirectIX());	z80_ICount -= (19-4);break; // ld_e_xix
//		case 0x5F: // nop

		case 0x60:	zIXh = zB;							z80_ICount -=  (9-4);break; // ld_ixh_b
		case 0x61:	zIXh = zC;							z80_ICount -=  (9-4);break; // ld_ixh_c
		case 0x62:	zIXh = zD;							z80_ICount -=  (9-4);break; // ld_ixh_d
		case 0x63:	zIXh = zE;							z80_ICount -=  (9-4);break; // ld_ixh_e
		case 0x64:	zIXh = zH;							z80_ICount -=  (9-4);break; // ld_ixh_h
		case 0x65:	zIXh = zL;							z80_ICount -=  (9-4);break; // ld_ixh_l
		case 0x66:	zH = RM(IndirectIX());	z80_ICount -= (19-4);break; // ld_h_xix
		case 0x67:	zIXh = zA;							z80_ICount -=  (9-4);break; // ld_ixh_a
		case 0x68:	zIXl = zB;							z80_ICount -=  (9-4);break; // ld_ixl_b
		case 0x69:	zIXl = zC;							z80_ICount -=  (9-4);break; // ld_ixl_c
		case 0x6A:	zIXl = zD;							z80_ICount -=  (9-4);break; // ld_ixl_d
		case 0x6B:	zIXl = zE;							z80_ICount -=  (9-4);break; // ld_ixl_e
		case 0x6C:	zIXl = zH;							z80_ICount -=  (9-4);break; // ld_ixl_h
		case 0x6D:	zIXl = zL;							z80_ICount -=  (9-4);break; // ld_ixl_l
		case 0x6E:	zL = RM(IndirectIX());	z80_ICount -= (19-4);break; // ld_l_xix
		case 0x6F:	zIXl = zA;							z80_ICount -=  (9-4);break; // ld_ixl_a

		case 0x70:	WM(IndirectIX(), zB);	z80_ICount -= (19-4);break; // ld_xix_b
		case 0x71:	WM(IndirectIX(), zC);	z80_ICount -= (19-4);break; // ld_xix_c
		case 0x72:	WM(IndirectIX(), zD);	z80_ICount -= (19-4);break; // ld_xix_d
		case 0x73:	WM(IndirectIX(), zE);	z80_ICount -= (19-4);break; // ld_xix_e
		case 0x74:	WM(IndirectIX(), zH);	z80_ICount -= (19-4);break; // ld_xix_h
		case 0x75:	WM(IndirectIX(), zL);	z80_ICount -= (19-4);break; // ld_xix_l
		case 0x76:										z80_ICount -= (19-4);break; // ld_0_xix // nop
		case 0x77:	WM(IndirectIX(), zA);	z80_ICount -= (19-4);break; // ld_xix_a
//		case 0x78: // nop
//		case 0x79: // nop
//		case 0x7A: // nop
//		case 0x7B: // nop
		case 0x7C:	zA = zIXh;							z80_ICount -=  (9-4);break; // ld_a_ixh
		case 0x7D:	zA = zIXl;							z80_ICount -=  (9-4);break; // ld_a_ixl
		case 0x7E:	zA = RM(IndirectIX());	z80_ICount -= (19-4);break; // ld_a_xix
//		case 0x7F: // nop
//		case 0x80: // nop
//		case 0x81: // nop
//		case 0x82: // nop
//		case 0x83: // nop
		case 0x84:	Add_1(zIXh);						z80_ICount -=  (9-4);break; // add_a_ixh
		case 0x85:	Add_1(zIXl);						z80_ICount -=  (9-4);break; // add_a_ixl
		case 0x86:	Add_1(RM(IndirectIX()));	z80_ICount -= (19-4);break; // add_a_xix

//		case 0x87: // nop
//		case 0x88: // nop
//		case 0x89: // nop
//		case 0x8A: // nop
//		case 0x8B: // nop
		case 0x8C:	Adc_1(zIXh);						z80_ICount -=  (9-4);break; // adc_a_ixh
		case 0x8D:	Adc_1(zIXl);						z80_ICount -=  (9-4);break; // adc_a_ixl
		case 0x8E:	Adc_1(RM(IndirectIX()));	z80_ICount -= (19-4);break; // adc_a_xix

//		case 0x8F: // nop
//		case 0x90: // nop
//		case 0x91: // nop
//		case 0x92: // nop
//		case 0x93: // nop

		case 0x94:	Sub_1(zIXh);						z80_ICount -=  (9-4);break; // sub_ixh
		case 0x95:	Sub_1(zIXl);						z80_ICount -=  (9-4);break; // sub_ixl
		case 0x96:	Sub_1(RM(IndirectIX()));	z80_ICount -= (19-4);break; // sub_xix

//		case 0x97: // nop
//		case 0x98: // nop
//		case 0x99: // nop
//		case 0x9A: // nop
//		case 0x9B: // nop

		case 0x9C:	Sbc_1(zIXh);						z80_ICount -=  (9-4);break; // sbc_a_ixh
		case 0x9D:	Sbc_1(zIXl);						z80_ICount -=  (9-4);break; // sbc_a_ixl
		case 0x9E:	Sbc_1(RM(IndirectIX()));	z80_ICount -= (19-4);break; // sbc_a_xix

//		case 0x9F: // nop
//		case 0xA0: // nop
//		case 0xA1: // nop
//		case 0xA2: // nop
//		case 0xA3: // nop

		case 0xA4:	And(zIXh);							z80_ICount -=  (9-4);break; // and_ixh
		case 0xA5:	And(zIXl);							z80_ICount -=  (9-4);break; // and_ixl
		case 0xA6:	And(RM(IndirectIX()));	z80_ICount -= (19-4);break; // and_xix

//		case 0xA7: // nop
//		case 0xA8: // nop
//		case 0xA9: // nop
//		case 0xAA: // nop
//		case 0xAB: // nop

		case 0xAC:	Xor(zIXh);							z80_ICount -=  (9-4);break; // xor_ixh
		case 0xAD:	Xor(zIXl);							z80_ICount -=  (9-4);break; // xor_ixl
		case 0xAE:	Xor(RM(IndirectIX()));	z80_ICount -= (19-4);break; // xor_xix

//		case 0xAF: // nop
//		case 0xB0: // nop
//		case 0xB1: // nop
//		case 0xB2: // nop
//		case 0xB3: // nop

		case 0xB4:	Or(zIXh);							z80_ICount -=  (9-4);break; // or_ixh
		case 0xB5:	Or(zIXl);							z80_ICount -=  (9-4);break; // or_ixl
		case 0xB6:	Or(RM(IndirectIX()));	z80_ICount -= (19-4);break; // or_xix

//		case 0xB7: // nop
//		case 0xB8: // nop
//		case 0xB9: // nop
//		case 0xBA: // nop
//		case 0xBB: // nop

		case 0xBC:	Cp(zIXh);							z80_ICount -=  (9-4);break; // cp_ixh
		case 0xBD:	Cp(zIXl);							z80_ICount -=  (9-4);break; // cp_ixl
		case 0xBE:	Cp(RM(IndirectIX()));	z80_ICount -= (19-4);break; // cp_xix

//		case 0xBF: // nop
//		case 0xC0: // nop
//		case 0xC1: // nop
//		case 0xC2: // nop
//		case 0xC3: // nop
//		case 0xC4: // nop
//		case 0xC5: // nop
//		case 0xC6: // nop
//		case 0xC7: // nop
//		case 0xC8: // nop
//		case 0xC9: // nop
//		case 0xCA: // nop

		case 0xCB: // dd_cb
		//	z80_ICount -= HandleDDCB();
		//	wAddr = zIX + (signed char)ImmedByte();
		//	HandleIXIY(zIX + (signed char)ImmedByte());
			//z80_ICount += 4;//ddddd
			HandleIXIY();
		//	z80_ICount -= 0;
			break;

//		case 0xCC: // nop
//		case 0xCD: // nop
//		case 0xCE: // nop
//		case 0xCF: // nop
//		case 0xD0: // nop
//		case 0xD1: // nop
//		case 0xD2: // nop
//		case 0xD3: // nop
//		case 0xD4: // nop
//		case 0xD5: // nop
//		case 0xD6: // nop
//		case 0xD7: // nop
//		case 0xD8: // nop
//		case 0xD9: // nop
//		case 0xDA: // nop
//		case 0xDB: // nop
//		case 0xDC: // nop
//		case 0xDD: // nop
//		case 0xDE: // nop
//		case 0xDF: // nop
//		case 0xE0: // nop

		case 0xE1:	zIX = Pop();			z80_ICount -= (14-4);break; // pop_ix
//		case 0xE2: // nop
		case 0xE3:
			wTemp = MemReadWord(GetSP());
			MemWriteWord(GetSP(), zIX);
			zIX = wTemp;
			z80_ICount -= (23-4);break; // ex_xsp_ix
//		case 0xE4: // nop
		case 0xE5:	Push(zIX);			z80_ICount -= (15-4);break; // push_ix
//		case 0xE6: // nop
//		case 0xE7: // nop
//		case 0xE8: // nop
		case 0xE9:	SetPC(zIX);			z80_ICount -= (8-4);break; // jp_ix

//		case 0xEA:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xEB:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xEC:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xED:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xEE:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xEF:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF0:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF1:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF2:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF3:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF4:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF5:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF6:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF7:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xF8:	illegal_1();	z80_ICount -= 0; // nop
		case 0xF9:	SetSP(zIX);		z80_ICount -= (10-4); break;// ld_sp_ix
//		case 0xFA:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xFB:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xFC:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xFD: 	illegal_1();	z80_ICount -= 0; // nop
//		case 0xFE:	illegal_1();	z80_ICount -= 0; // nop
//		case 0xFF:	illegal_1();	z80_ICount -= 0; // nop
	//	default:
	//	z80_ICount -= 8;
	//		return 0;
	}
	z80_ICount -= (8-4);
}


#if 00
UINT32 mz80GetElapsedTicks(UINT32 dwClearIt)
{
	UINT32 dwTempVar = 0;
//	char string[150];

//	sprintf(string, "Time=%ld\r\n", dwElapsedTicks);
//	HostPutString(string);

	dwTempVar = dwElapsedTicks;
	if(dwClearIt)
		dwElapsedTicks = 0;
	return(dwTempVar);
}

void mz80ReleaseTimeslice()
{
	z80_ICount = 0;
}
#endif


/* ************************************
 * Opcodes
 * ************************************
 */

/*---------------------------------------------------------------------------

	Exec

	Executes the specified number of t-states.  Returns the actual number of
	t-states executed.  If a halt instruction is encountered, the execution
	loop exits.  If the processor is currently in a halted state this
	function immediately returns 0.

---------------------------------------------------------------------------*/
/****************************************************************************
 * Execute 'cycles' T-states. Return number of T-states really executed
 ****************************************************************************/
 void z80_debt_clk(int cycles){
//	Z80.extra_cycles+=cycles;
}
//unsigned long z80_execute( unsigned long z80_ICountArg ) {
void z80_execute( unsigned long z80_ICountArg ) {
	UINT32 origElapsedTicks=dwElapsedTicks;

//	if(zHALT)
//		return 0;

	//
	//  Save t-state count for return value calculation
	//

	z80_ICount = z80_ICountArg;

#ifndef TRACE_Z80
	while (z80_ICount > 0)
#endif
	{
//#ifdef _DEBUG
#if 000
		static int cins;
		//if((cins % 1000) == 0 && GetAsyncKeyState('X') < 0) _ASSERT(FALSE);
		//_ASSERT(cins != _instrap);
		if(cins >= _rgcins[0] && cins <= _rgcins[0] + _rgcins[1] * _rgcins[2] && ((cins - _rgcins[0]) % _rgcins[2]) == 0)
		{
			if(cins == _rgcins[0])
			{
				TRACE("instr    PC   AF   BC   DE   HL   IX   IY   SP   cycl dasm        \n");
				TRACE("-------- ---- ---- ---- ---- ---- ---- ---- ---- ---- ------------\n");
			}
			TCHAR szBuf[128];
			Z80_Dasm(&m_rgbMemory[GetPC()], szBuf, GetPC());
			TRACE("%08X %04X %04X %04X %04X %04X %04X %04X %04X %04X %s\n",
				cins, GetPC(), zAF, zBC, zDE, zHL, zIX, zIY, GetSP(), z80_ICount, szBuf);
		}
		cins++;
#endif
		const UINT8 bOpcode = ImmedByte();

//#ifdef _DEBUG
//		m_rgcOpcode[0][bOpcode]++;
//#endif

//		zR++;

#if 0
		/* debug crap for RC */
		{
		  char szBuf [ 128 ];
		  Z80_Dasm(&m_rgbMemory[GetPC()], szBuf, GetPC());

		  fprintf ( stderr,
			    "%04X %04X %04X %04X %04X %04X %04X %04X %04X %s\n",
			    GetPC(), zAF, zBC, zDE, zHL,
			    zIX, zIY, GetSP(), z80_ICount, szBuf );
		}

		// fprintf ( stderr, "%d\n", bOpcode );
#endif
		//z80pc = GetPC(); /* for some of the platforms */

		dwElapsedTicks = origElapsedTicks + (z80_ICountArg - z80_ICount);

		z80_ICount -= 4;/*強制４clk*/
		switch (bOpcode)
		{
			case 0x00: // nop
				//z80_ICount -= (4-4);
				break;

			case 0x01: // ld_bc_word
				z80_ICount -= (10-4);
				zBC = ARG16();
				break;

			case 0x02: // ld_xbc_a
				z80_ICount -= (7-4);
				WM(zBC, zA);
				break;

			case 0x03: // inc_bc
				z80_ICount -= (6-4);
				zBC++;
				break;

			case 0x04: // inc_b
				//z80_ICount -= (4-4);
				zB = myInc(zB);
				break;

			case 0x05: // dec_b
				//z80_ICount -= (4-4);
				zB = myDec(zB);
				break;

			case 0x06: // ld_b_byte
				z80_ICount -= (7-4);
				zB = ImmedByte();
				break;

			case 0x07: // RLCA
				//z80_ICount -= (4-4);
//				Rlca();
//static void Rlca() {
	zA = (zA << 1) | ((zA & 0x80) >> 7);
//	zF = (zF & 0xEC) | (zA & CF);
	zF = (zF & ( SF | ZF | VF)) | (zA & ( XF | YF | CF));
//}

				break;

			case 0x08: // ex_af_af
				//z80_ICount -= (4-4);
{
unsigned short iii;
	iii = zAF;	zAF = zAF2;	zAF2 = iii;
}
//				swap(zAF, zAF2);
				break;

			case 0x09: // add_hl_bc
				z80_ICount -= (11-4);
				zHL = Add_2(zHL, zBC);
				break;

			case 0x0A: // ld_a_xbc
				z80_ICount -= (7-4);
				zA = RM(zBC);
				break;

			case 0x0B: // dec_bc
				z80_ICount -= (6-4);
				zBC--;
				break;

			case 0x0C: // inc_c
				//z80_ICount -= (4-4);
				zC = myInc(zC);
				break;

			case 0x0D: // dec_c
				//z80_ICount -= (4-4);
				zC = myDec(zC);
				break;

			case 0x0E: // ld_c_byte
				z80_ICount -= (7-4);
				zC = ImmedByte();
				break;

			case 0x0F: // RRCA
				//z80_ICount -= (4-4);
//				Rrca();
//static void Rrca()
{
unsigned char aaa;
aaa = zA;
//	zF=(zF&0xEC)|(zA&0x01);
	zA=(aaa>>1)|(aaa<<7);
zF = (zF & ( SF | ZF | VF)) | (zA & ( XF | YF)) | (aaa & CF);
}
				break;

			case 0x10: // djnz
				z80_ICount -= (8-4);
				z80_ICount -= Jr0(--zB);
				break;

			case 0x11: // ld_de_word
				z80_ICount -= (10-4);
				zDE = ARG16();
				break;

			case 0x12: // ld_xde_a
				z80_ICount -= (7-4);
				WM(zDE, zA);
				break;

			case 0x13: // inc_de
				z80_ICount -= (6-4);
				zDE++;
				break;

			case 0x14: // inc_d
				//z80_ICount -= (4-4);
				zD = myInc(zD);
				break;

			case 0x15: // dec_d
				//z80_ICount -= (4-4);
				zD = myDec(zD);
				break;

			case 0x16: // ld_d_byte
				z80_ICount -= (7-4);
				zD = ImmedByte();
				break;

			case 0x17: // rla
				//z80_ICount -= (4-4);
				Rla();
				break;

			case 0x18: // jr
			{
				z80_ICount += 4;//ddddd
#if 0
				z80_ICount -= Jr0(TRUE);
#else
				// speedup technique adopted from Nicola's code in MAME to speedup
				// Galaga (the sound CPU spins with "loop: jr loop" waiting for
				// an interrupt).  This technique is generic enough that is shouldn't
				// adversely effect other applications.

				signed char offset = (signed char)ImmedByte();
				z80_ICount -= (7-4);
				z80_ICount -= 5;

				AdjustPC(offset);

				if(offset == -2)
				{
//					TRACE(__FILE__ ":jr (infinite loop) at %04X\n", GetPC());
				  goto z80ExecBottom;
				  /* return (z80_ICountArg - z80_ICount); */
				}
#endif
				break;
			}

			case 0x19:	z80_ICount -= (11-4);	zHL = Add_2(zHL, zDE);		break; // add_hl_de
			case 0x1A:	z80_ICount -= (7-4);	zA = RM(zDE);	break; // ld_a_xde
			case 0x1B:	z80_ICount -= (6-4);	zDE--;						break; // dec_de
			case 0x1C:	/*z80_ICount -= (4-4);*/	zE = myInc(zE);				break; // inc_e
			case 0x1D:	/*z80_ICount -= (4-4);*/	zE = myDec(zE);				break; // dec_e
			case 0x1E:	z80_ICount -= (7-4);	zE = ImmedByte();			break; // ld_e_byte
			case 0x1F:	/*z80_ICount -= (4-4);*/	Rra();						break; // rra
			case 0x20:	z80_ICount -= (7-4);
						z80_ICount -= Jr1(zF & ZF);					break; // jr_nz
			case 0x21:	z80_ICount -= (10-4);	zHL = ARG16();			break; // ld_hl_word
			case 0x22:	z80_ICount -= (16-4);	MemWriteWord(ARG16(), zHL);		break; // ld_xword_hl
			case 0x23:	z80_ICount -= (6-4);	zHL++;				break; // inc_hl
			case 0x24:	/*z80_ICount -= (4-4);*/	zH = myInc(zH);		break; // inc_h
			case 0x25:	/*z80_ICount -= (4-4);*/	zH = myDec(zH);		break; // dec_h
			case 0x26:	z80_ICount -= (7-4);	zH = ImmedByte();	break; // ld_h_byte

			case 0x27: // daa
			{
#ifdef LARGE_DAA_TABLE
				int aaa = zA;
				if(zF&CF) aaa|=256;
				if(zF&HF) aaa|=512;
				if(zF&NF) aaa|=1024;
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
		if(lo >= 10){	diff = (hi <= 8) ? 0x06 : 0x66;	}
		else
		{
			if(hi >= 10){	diff = hf ? 0x66 : 0x60;	}
			else		{	diff = hf ? 0x06 : 0x00;	}
		}
	}
	if(nf) zA -= diff;
	else zA += diff;
	zF = SZP[zA] | (zF & NF);
	if (cf || (lo <= 9 ? hi >= 10 : hi >= 9)) zF |= CF;
	if (nf ? hf && lo <= 5 : lo >= 10)	zF |= HF;
#endif
				/*z80_ICount -= (4-4);*/
				break;
			}

			case 0x28:
				z80_ICount -= (7-4);
				z80_ICount -= Jr0(zF & ZF);
				break; // jr_z

			case 0x29:
				z80_ICount -= (11-4);
				zHL = Add_2(zHL, zHL);
				break; // add_hl_hl

			case 0x2A:
				z80_ICount -= (16-4);
				zHL = MemReadWord(ARG16());
				break; // ld_hl_xword

			case 0x2B:
				z80_ICount -= (6-4);
				zHL--;
				break; // dec_hl

			case 0x2C:
				/*z80_ICount -= (4-4);*/
				zL = myInc(zL);
				break; // inc_l

			case 0x2D:
				/*z80_ICount -= (4-4);*/
				zL = myDec(zL);
				break; // dec_l

			case 0x2E:
				z80_ICount -= (7-4);
				zL = ImmedByte();
				break; // ld_l_byte

			case 0x2F: // CPL
				/*z80_ICount -= (4-4);*/
				zA ^= 0xFF;
			//	zF |= (HF | NF);
				zF = (zF & (SF | ZF | VF | CF)) | HF |  NF | (zA & ( XF | YF));
				break;

			case 0x30: // jr_nc
				z80_ICount -= (7-4);
				z80_ICount -= Jr1(zF & CF);
				break;

			case 0x31: // ld_sp_word
				z80_ICount -= (10-4);
				SetSP(ARG16());
				break;

			case 0x32:	z80_ICount -= (13-4);	WM(ARG16(), zA);	break; // ld_xbyte_a
			case 0x33:	z80_ICount -= (6-4);	SetSP(GetSP() + 1);					break; // inc_sp
			case 0x34:	z80_ICount -= (11-4);	WM(zHL, myInc(RM(zHL)));		break; // inc_xhl
			case 0x35:	z80_ICount -= (11-4);	WM(zHL, myDec(RM(zHL)));		break; // dec_xhl
			case 0x36:	z80_ICount -= (10-4);	WM(zHL, ImmedByte());	break; // ld_xhl_byte

			case 0x37: // SCF
				/*z80_ICount -= (4-4);*/
			//	zF = (zF & 0xEC) | CF;
				zF = (zF & ( SF | ZF | VF)) | (zA & ( XF | YF)) | CF;
				break;

			case 0x38:	z80_ICount -= (7-4);	z80_ICount -= Jr0(zF & CF);	break; // jr_c
			case 0x39:	z80_ICount -= (11-4);	zHL = Add_2(zHL, GetSP());		break; // add_hl_sp
			case 0x3A:	z80_ICount -= (13-4);	zA = RM(ARG16());	break; // ld_a_xbyte
			case 0x3B:	z80_ICount -= (6-4);	SetSP(GetSP() - 1);	break; // dec_sp
			case 0x3C:	/*z80_ICount -= (4-4);*/	zA = myInc(zA);		break; // inc_a
			case 0x3D:	/*z80_ICount -= (4-4);*/	zA = myDec(zA);		break; // dec_a
			case 0x3E:	z80_ICount -= (7-4);	zA = ImmedByte();	break; // ld_a_byte
			case 0x3F:
				/*z80_ICount -= (4-4);*/
			//	zF =((zF & 0xED)|((zF & 1)<<4))^1;
				zF = ((zF & ( SF | ZF | VF | CF)) | ((zF & CF) << 4) | (zA & ( XF | YF))) ^ CF;
				break; // CCF
			case 0x40:	/*z80_ICount -= (4-4);*//*	zB = zB;*/	break; // ld_b_b
			case 0x41:	/*z80_ICount -= (4-4);*/	zB = zC;	break; // ld_b_c
			case 0x42:	/*z80_ICount -= (4-4);*/	zB = zD;	break; // ld_b_d
			case 0x43:	/*z80_ICount -= (4-4);*/	zB = zE;	break; // ld_b_e
			case 0x44:	/*z80_ICount -= (4-4);*/	zB = zH;	break; // ld_b_h
			case 0x45:	/*z80_ICount -= (4-4);*/	zB = zL;	break; // ld_b_l
			case 0x46:	z80_ICount -= (7-4);		zB = RM(zHL);	break; // ld_b_xhl
			case 0x47:	/*z80_ICount -= (4-4);*/	zB = zA;	break; // ld_b_a
			case 0x48:	/*z80_ICount -= (4-4);*/	zC = zB;	break; // ld_c_b
			case 0x49:	/*z80_ICount -= (4-4);*//*	zC = zC;*/	break; // ld_c_c
			case 0x4A:	/*z80_ICount -= (4-4);*/	zC = zD;	break; // ld_c_d
			case 0x4B:	/*z80_ICount -= (4-4);*/	zC = zE;	break; // ld_c_e
			case 0x4C:	/*z80_ICount -= (4-4);*/	zC = zH;	break; // ld_c_h
			case 0x4D:	/*z80_ICount -= (4-4);*/	zC = zL;	break; // ld_c_l
			case 0x4E:	z80_ICount -= (7-4);		zC = RM(zHL);	break; // ld_c_xhl
			case 0x4F:	/*z80_ICount -= (4-4);*/	zC = zA;	break; // ld_c_a
			case 0x50:	/*z80_ICount -= (4-4);*/	zD = zB;	break; // ld_d_b
			case 0x51:	/*z80_ICount -= (4-4);*/	zD = zC;	break; // ld_d_c
			case 0x52:	/*z80_ICount -= (4-4);*//*	zD = zD;*/	break; // ld_d_d
			case 0x53:	/*z80_ICount -= (4-4);*/	zD = zE;	break; // ld_d_e
			case 0x54:	/*z80_ICount -= (4-4);*/	zD = zH;	break; // ld_d_h
			case 0x55:	/*z80_ICount -= (4-4);*/	zD = zL;	break; // ld_d_l
			case 0x56:	z80_ICount -= (7-4);		zD = RM(zHL);	break; // ld_d_xhl
			case 0x57:	/*z80_ICount -= (4-4);*/	zD = zA;	break; // ld_d_a
			case 0x58:	/*z80_ICount -= (4-4);*/	zE = zB;	break; // ld_e_b
			case 0x59:	/*z80_ICount -= (4-4);*/	zE = zC;	break; // ld_e_c
			case 0x5A:	/*z80_ICount -= (4-4);*/	zE = zD;	break; // ld_e_d
			case 0x5B:	/*z80_ICount -= (4-4);*//*	zE = zE;*/	break; // ld_e_e
			case 0x5C:	/*z80_ICount -= (4-4);*/	zE = zH;	break; // ld_e_h
			case 0x5D:	/*z80_ICount -= (4-4);*/	zE = zL;	break; // ld_e_l
			case 0x5E:	z80_ICount -= (7-4);		zE = RM(zHL);	break; // ld_e_xhl
			case 0x5F:	/*z80_ICount -= (4-4);*/	zE = zA;	break; // ld_e_a
			case 0x60:	/*z80_ICount -= (4-4);*/	zH = zB;	break; // ld_h_b
			case 0x61:	/*z80_ICount -= (4-4);*/	zH = zC;	break; // ld_h_c
			case 0x62:	/*z80_ICount -= (4-4);*/	zH = zD;	break; // ld_h_d
			case 0x63:	/*z80_ICount -= (4-4);*/	zH = zE;	break; // ld_h_e
			case 0x64:	/*z80_ICount -= (4-4);*//*	zH = zH;*/	break; // ld_h_h
			case 0x65:	/*z80_ICount -= (4-4);*/	zH = zL;	break; // ld_h_l
			case 0x66:	z80_ICount -= (7-4);		zH = RM(zHL);	break; // ld_h_xhl
			case 0x67:	/*z80_ICount -= (4-4);*/	zH = zA;	break; // ld_h_a
			case 0x68:	/*z80_ICount -= (4-4);*/	zL = zB;	break; // ld_l_b
			case 0x69:	/*z80_ICount -= (4-4);*/	zL = zC;	break; // ld_l_c
			case 0x6A:	/*z80_ICount -= (4-4);*/	zL = zD;	break; // ld_l_d
			case 0x6B:	/*z80_ICount -= (4-4);*/	zL = zE;	break; // ld_l_e
			case 0x6C:	/*z80_ICount -= (4-4);*/	zL = zH;	break; // ld_l_h
			case 0x6D:	/*z80_ICount -= (4-4);*//*	zL = zL;*/	break; // ld_l_l
			case 0x6E:	z80_ICount -= (7-4);		zL = RM(zHL);	break; // ld_l_xhl
			case 0x6F:	/*z80_ICount -= (4-4);*/	zL = zA;	break; // ld_l_a
			case 0x70:	z80_ICount -= (7-4);	WM(zHL, zB);	break; // ld_xhl_b
			case 0x71:	z80_ICount -= (7-4);	WM(zHL, zC);	break; // ld_xhl_c
			case 0x72:	z80_ICount -= (7-4);	WM(zHL, zD);	break; // ld_xhl_d
			case 0x73:	z80_ICount -= (7-4);	WM(zHL, zE);	break; // ld_xhl_e
			case 0x74:	z80_ICount -= (7-4);	WM(zHL, zH);	break; // ld_xhl_h
			case 0x75:	z80_ICount -= (7-4);	WM(zHL, zL);	break; // ld_xhl_l
			case 0x76: // halt
				/*z80_ICount -= (4-4);*/
			//	SetPC(GetPC() - 1);
				zHALT = TRUE;
				goto z80ExecBottom;
				/* return (z80_ICountArg - z80_ICount); */

			case 0x77:	z80_ICount -= (7-4);	WM(zHL, zA);	break; // ld_xhl_a
			case 0x78:	/*z80_ICount -= (4-4);*/	zA = zB;	break; // ld_a_b
			case 0x79:	/*z80_ICount -= (4-4);*/	zA = zC;	break; // ld_a_c
			case 0x7A:	/*z80_ICount -= (4-4);*/	zA = zD;	break; // ld_a_d
			case 0x7B:	/*z80_ICount -= (4-4);*/	zA = zE;	break; // ld_a_e
			case 0x7C:	/*z80_ICount -= (4-4);*/	zA = zH;	break; // ld_a_h
			case 0x7D:	/*z80_ICount -= (4-4);*/	zA = zL;	break; // ld_a_l
			case 0x7E:	z80_ICount -= (7-4);	zA = RM(zHL);	break; // ld_a_xhl
			case 0x7F:	/*z80_ICount -= (4-4);*//*	zA = zA;*/	break; // ld_a_a
			case 0x80:	/*z80_ICount -= (4-4);*/	Add_1(zB);	break; // add_a_b
			case 0x81:	/*z80_ICount -= (4-4);*/	Add_1(zC);	break; // add_a_c
			case 0x82:	/*z80_ICount -= (4-4);*/	Add_1(zD);	break; // add_a_d
			case 0x83:	/*z80_ICount -= (4-4);*/	Add_1(zE);	break; // add_a_e
			case 0x84:	/*z80_ICount -= (4-4);*/	Add_1(zH);	break; // add_a_h
			case 0x85:	/*z80_ICount -= (4-4);*/	Add_1(zL);	break; // add_a_l
			case 0x86:	z80_ICount -= (7-4);	Add_1(RM(zHL));	break; // add_a_xhl
			case 0x87:	/*z80_ICount -= (4-4);*/	Add_1(zA);	break; // add_a_a
			case 0x88:	/*z80_ICount -= (4-4);*/	Adc_1(zB);	break; // adc_a_b
			case 0x89:	/*z80_ICount -= (4-4);*/	Adc_1(zC);	break; // adc_a_c
			case 0x8A:	/*z80_ICount -= (4-4);*/	Adc_1(zD);	break; // adc_a_d
			case 0x8B:	/*z80_ICount -= (4-4);*/	Adc_1(zE);	break; // adc_a_e
			case 0x8C:	/*z80_ICount -= (4-4);*/	Adc_1(zH);	break; // adc_a_h
			case 0x8D:	/*z80_ICount -= (4-4);*/	Adc_1(zL);	break; // adc_a_l
			case 0x8E:	z80_ICount -= (7-4);	Adc_1(RM(zHL));	break; // adc_a_xhl
			case 0x8F:	/*z80_ICount -= (4-4);*/	Adc_1(zA);	break; // adc_a_a
			case 0x90:	/*z80_ICount -= (4-4);*/	Sub_1(zB);	break; // sub_b
			case 0x91:	/*z80_ICount -= (4-4);*/	Sub_1(zC);	break; // sub_c
			case 0x92:	/*z80_ICount -= (4-4);*/	Sub_1(zD);	break; // sub_d
			case 0x93:	/*z80_ICount -= (4-4);*/	Sub_1(zE);	break; // sub_e
			case 0x94:	/*z80_ICount -= (4-4);*/	Sub_1(zH);	break; // sub_h
			case 0x95:	/*z80_ICount -= (4-4);*/	Sub_1(zL);	break; // sub_l
			case 0x96:	z80_ICount -= (7-4);	Sub_1(RM(zHL));	break; // sub_xhl
			case 0x97:	/*z80_ICount -= (4-4);*/	Sub_1(zA);	break; // sub_a
			case 0x98:	/*z80_ICount -= (4-4);*/	Sbc_1(zB);	break; // sbc_a_b
			case 0x99:	/*z80_ICount -= (4-4);*/	Sbc_1(zC);	break; // sbc_a_c
			case 0x9A:	/*z80_ICount -= (4-4);*/	Sbc_1(zD);	break; // sbc_a_d
			case 0x9B:	/*z80_ICount -= (4-4);*/	Sbc_1(zE);	break; // sbc_a_e
			case 0x9C:	/*z80_ICount -= (4-4);*/	Sbc_1(zH);	break; // sbc_a_h
			case 0x9D:	/*z80_ICount -= (4-4);*/	Sbc_1(zL);	break; // sbc_a_l
			case 0x9E:	z80_ICount -= (7-4);	Sbc_1(RM(zHL));	break; // sbc_a_xhl
			case 0x9F:	/*z80_ICount -= (4-4);*/	Sbc_1(zA);	break; // sbc_a_a
			case 0xA0:	/*z80_ICount -= (4-4);*/	And(zB);	break; // and_b
			case 0xA1:	/*z80_ICount -= (4-4);*/	And(zC);	break; // and_c
			case 0xA2:	/*z80_ICount -= (4-4);*/	And(zD);	break; // and_d
			case 0xA3:	/*z80_ICount -= (4-4);*/	And(zE);	break; // and_e
			case 0xA4:	/*z80_ICount -= (4-4);*/	And(zH);	break; // and_h
			case 0xA5:	/*z80_ICount -= (4-4);*/	And(zL);	break; // and_l
			case 0xA6:	z80_ICount -= (7-4);	And(RM(zHL));	break; // and_xhl
			case 0xA7:	/*z80_ICount -= (4-4);*/	And(zA);	break; // and_a
			case 0xA8:	/*z80_ICount -= (4-4);*/	Xor(zB);	break; // xor_b
			case 0xA9:	/*z80_ICount -= (4-4);*/	Xor(zC);	break; // xor_c
			case 0xAA:	/*z80_ICount -= (4-4);*/	Xor(zD);	break; // xor_d
			case 0xAB:	/*z80_ICount -= (4-4);*/	Xor(zE);	break; // xor_e
			case 0xAC:	/*z80_ICount -= (4-4);*/	Xor(zH);	break; // xor_h
			case 0xAD:	/*z80_ICount -= (4-4);*/	Xor(zL);	break; // xor_l
			case 0xAE:	z80_ICount -= (7-4);	Xor(RM(zHL));	break; // xor_xhl
			case 0xAF:	/*z80_ICount -= (4-4);*/	Xor(zA);	break; // xor_a
			case 0xB0:	/*z80_ICount -= (4-4);*/	Or(zB);		break; // or_b
			case 0xB1:	/*z80_ICount -= (4-4);*/	Or(zC);		break; // or_c
			case 0xB2:	/*z80_ICount -= (4-4);*/	Or(zD);		break; // or_d
			case 0xB3:	/*z80_ICount -= (4-4);*/	Or(zE);		break; // or_e
			case 0xB4:	/*z80_ICount -= (4-4);*/	Or(zH);		break; // or_h
			case 0xB5:	/*z80_ICount -= (4-4);*/	Or(zL);		break; // or_l
			case 0xB6:	z80_ICount -= (7-4);		Or(RM(zHL));		break; // or_xhl
			case 0xB7:	/*z80_ICount -= (4-4);*/	Or(zA);		break; // or_a
			case 0xB8:	/*z80_ICount -= (4-4);*/	Cp(zB);		break; // cp_b
			case 0xB9:	/*z80_ICount -= (4-4);*/	Cp(zC);		break; // cp_c
			case 0xBA:	/*z80_ICount -= (4-4);*/	Cp(zD);		break; // cp_d
			case 0xBB:	/*z80_ICount -= (4-4);*/	Cp(zE);		break; // cp_e
			case 0xBC:	/*z80_ICount -= (4-4);*/	Cp(zH);		break; // cp_h
			case 0xBD:	/*z80_ICount -= (4-4);*/	Cp(zL);		break; // cp_l
			case 0xBE:	z80_ICount -= (7-4);	Cp(RM(zHL));		break; // cp_xhl
			case 0xBF:	/*z80_ICount -= (4-4);*/	Cp(zA);		break; // cp_a
			case 0xC0: // ret_nz
				z80_ICount -= (5-4);
				if/*Ret0*/(!(zF & ZF)){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xC1: // pop_bc
				z80_ICount -= (10-4);
				zBC = Pop();
				break;

			case 0xC2: // jp_nz
				z80_ICount -= (10-4);
				z80_ICount -= Jp1(zF & ZF);
				break;

			case 0xC3: // jp
				z80_ICount -= (10-4);
#if 1
				Jp0(TRUE);
#else
				{
					// speedup hack for Galaga adopted from MAME...

					UINT16 w = ARG16();
					UINT16 wPc = GetPC();

					SetPC(w);

					// removed since Galaga never hit it (maybe 1942 will), and it
					// caused the slowdown in Time Pilot and Gyruss hiscore bug
					// it isn't correct anyway since "0085 jp 0088" causes is to return (Gyruss)
					// shouldn't it be if(w == wPC - 3)???
					// jul-26-97 eam

/*
					if(w == wPc)
					{
	//					TRACE(__FILE__ ":jp (infinite loop) at %04X\n", GetPC());
						return (z80_ICountArg - z80_ICount);
					}
					else */
					if(w == wPc - 6) // attn: very Galaga specific!!!! should be done in MageX by patching ROMs with HALTs (once I figure out how to fool the ROM check at bootup)
					{
						if(*m_rgbOpcode == 0x31)
						{
		//					TRACE(__FILE__ ":jp (ld sp,#xxxx) at %04X\n", GetPC());
						  goto z80ExecBottom;
						  /* return (z80_ICountArg - z80_ICount); */
						}
					}
				}
#endif
				break;

			case 0xC4: // call_nz
				z80_ICount -= (10-4);
				z80_ICount -= Call1(zF & ZF);
				break;

			case 0xC5: // push_bc
				z80_ICount -= (11-4);
				Push(zBC);
				break;

			case 0xC6: // add_a_byte
				z80_ICount -= (7-4);
				Add_1(ImmedByte());
				break;

			case 0xC7: // rst_00
				z80_ICount -= (11-4);
				Rst(0x00);
				break;

			case 0xC8: // ret_z
				z80_ICount -= (5-4);
				if/*Ret0*/(zF & ZF){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xC9: // ret
				z80_ICount -= (4+6-4);
			/*	z80_ICount -= 6;*/Ret0_TRUE;
				break;

			case 0xCA: // jp_z
				z80_ICount -= (10-4);
				z80_ICount -= Jp0(zF & ZF);
				break;

			case 0xCB: // cb
				//z80_ICount += 4;//ddddd
				HandleCB();
				break;

			case 0xCC: // call_z
				z80_ICount -= (10-4);
				z80_ICount -= Call0(zF & ZF);
				break;

			case 0xCD: // call
				z80_ICount -= (10-4);
				z80_ICount -= Call0(TRUE);
				break;

			case 0xCE: // adc_a_byte
				z80_ICount -= (7-4);
				Adc_1(ImmedByte());
				break;

			case 0xCF: // rst_08
				z80_ICount -= (11-4);
				Rst(0x08);
				break;

			case 0xD0: // ret_nc
				z80_ICount -= (5-4);
				if/*Ret0*/(!(zF & CF)){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xD1: // pop_de
				z80_ICount -= (10-4);
				zDE = Pop();
				break;

			case 0xD2: // jp_nc
				z80_ICount -= (10-4);
				z80_ICount -= Jp1(zF & CF);
				break;

			case 0xD3: // out_byte_a
				z80_ICount -= (11-4);
				Out(ImmedByte(), zA);
				break;

			case 0xD4: // call_nc
				z80_ICount -= (10-4);
				z80_ICount -= Call1(zF & CF);
				break;

			case 0xD5: // push_de
				z80_ICount -= (11-4);
				Push(zDE);
				break;

			case 0xD6: // sub_byte
			{
				z80_ICount -= (7-4);
				Sub_1(ImmedByte());
				break;
			}

			case 0xD7: // rst_10
				z80_ICount -= (11-4);
				Rst(0x10);
				break;

			case 0xD8: // ret_c
				z80_ICount -= (5-4);
				if/*Ret0*/(zF & CF){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xD9: // exx
				/*z80_ICount -= (4-4);*/
			//	Exx();
//void Exx() {
			{
			unsigned short iii;
				iii = zBC;	zBC = zBC2;	zBC2 = iii;
				iii = zDE;	zDE = zDE2;	zDE2 = iii;
				iii = zHL;	zHL = zHL2;	zHL2 = iii;
			}
//	swap(zBC, zBC2);
//	swap(zDE, zDE2);
//	swap(zHL, zHL2);
//}

				break;

			case 0xDA: // jp_c
				z80_ICount -= (10-4);
				z80_ICount -= Jp0(zF & CF);
				break;

			case 0xDB: // in_a_byte
				z80_ICount -= (11-4);
			{
				//UINT8 bPort =
					ImmedByte();
				zA = 0xff;//InRaw( bPort );
				break;
			}

			case 0xDC: // call_c
				z80_ICount -= (10-4);
				z80_ICount -= Call0(zF & CF);
				break;

			case 0xDD: // dd
				//z80_ICount -=
				//z80_ICount += 4;//ddddd
				HandleDD();
				break;

			case 0xDE: // sbc_a_byte
				z80_ICount -= (7-4);
				Sbc_1(ImmedByte());
				break;

			case 0xDF: // rst_18
				z80_ICount -= (11-4);
				Rst(0x18);
				break;

			case 0xE0: // ret_po
				z80_ICount -= (5-4);
				if/*Ret0*/(!(zF & VF)){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xE1: // pop_hl
				z80_ICount -= (10-4);
				zHL = Pop();
				break;

			case 0xE2: // jp_po
				z80_ICount -= (10-4);
				z80_ICount -= Jp1(zF & VF);
				break;

			case 0xE3: // ex_xsp_hl
				z80_ICount -= (19-4);
			{
				int i = MemReadWord(GetSP());
				MemWriteWord(GetSP(), zHL);
				zHL = i;
				break;
			}

			case 0xE4: // call_po
				z80_ICount -= (10-4);
				z80_ICount -= Call1(zF & VF);
				break;

			case 0xE5: // push_hl
				z80_ICount -= (11-4);
				Push(zHL);
				break;

			case 0xE6: // and_byte
				z80_ICount -= (7-4);
				And(ImmedByte());
				break;

			case 0xE7: // rst_20
				z80_ICount -= (11-4);
				Rst(0x20);
				break;

			case 0xE8: // ret_pe
				z80_ICount -= (5-4);
				if/*Ret0*/(zF & VF){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xE9: // jp_hl
				/*z80_ICount -= (4-4);*/
				SetPC(zHL);
				break;

			case 0xEA: // jp_pe
				z80_ICount -= (10-4);
				z80_ICount -= Jp0(zF & VF);
				break;

			case 0xEB: // ex_de_hl
			{
			unsigned short iii;
				iii = zDE;	zDE = zHL;	zHL = iii;
			}
//				swap(zDE, zHL);
				/*z80_ICount -= (4-4);*/
				break;

			case 0xEC: // call_pe
				z80_ICount -= (10-4);
				z80_ICount -= Call0(zF & VF);
				break;

			case 0xED: // ed
				//z80_ICount += 4;//ddddd
				//z80_ICount -=
				HandleED();//z80_ICount);
				break;

			case 0xEE: // xor_byte
				z80_ICount -= (7-4);
				Xor(ImmedByte());
				break;

			case 0xEF: // rst_28
				z80_ICount -= (11-4);
				Rst(0x28);
				break;

			case 0xF0: // ret_p
				z80_ICount -= (5-4);
				if/*Ret0*/(!(zF & SF)){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xF1: // pop_af
				z80_ICount -= (10-4);
				zAF = Pop();
				break;

			case 0xF2: // jp_p
				z80_ICount -= (10-4);
				z80_ICount -= Jp1(zF & SF);
				break;

			case 0xF3: // di
				/*z80_ICount -= (4-4);*/
//				Di();
//void Di() {
				/* fprintf ( stderr, "Disable Interrupt\n" ); */
	zIFF1 = 0;
	zIFF2 = 0;
//}
				break;

			case 0xF4: // call_p
				z80_ICount -= (10-4);
				z80_ICount -= Call1(zF & SF);
				break;

			case 0xF5: // push_af
				z80_ICount -= (11-4);
				Push(zAF);
				break;

			case 0xF6: // or_byte
				z80_ICount -= (7-4);
				Or(ImmedByte());
				break;

			case 0xF7: // rst_30
				z80_ICount -= (11-4);
				Rst(0x30);
				break;

			case 0xF8: // ret_m
				z80_ICount -= (5-4);
				if/*Ret0*/(zF & SF){
					z80_ICount -= 6;
					Ret0_TRUE;
				}
				break;

			case 0xF9: // ld_sp_hl
				z80_ICount -= (6-4);
				SetSP(zHL);
				break;

			case 0xFA: // jp_m
				z80_ICount -= (10-4);
				z80_ICount -= Jp0(zF & SF);
				break;

			case 0xFB: // ei
				/*z80_ICount -= (4-4);*/
//				z80_ICount -= Ei();
//int Ei() {
	/* fprintf ( stderr, "Enable Interrupt\n" ); */
	zIFF2 = 1;
	if(zIFF1 != 0)	{;}
	else{ zIFF1 = 1;}
//}

//				if(zPendingInterrupt)
				{
//					return (z80_ICountArg - z80_ICount);
				}
				break;

			case 0xFC: // call_m
				z80_ICount -= (10-4);
				z80_ICount -= Call0(zF & SF);
				break;

			case 0xFD: // fd
			//	z80_ICount -=
			//	HandleFD();
				{unsigned short mytemp;
				mytemp= zIX;
					zIX=zIY;
					//z80_ICount += 4;//ddddd
					HandleDD();
					zIY=zIX;
				zIX=mytemp;
				}
				break;

			case 0xFE: // cp_byte
				z80_ICount -= (7-4);
				Cp(ImmedByte());
				break;

			case 0xFF: // rst_38
				z80_ICount -= (11-4);
				Rst(0x38);
				break;

//			default:
//				//assert(FALSE);
//				break;
		}
	}

z80ExecBottom:
	//z80pc = GetPC(); /* for some of the platforms */
	dwElapsedTicks = origElapsedTicks + (z80_ICountArg - z80_ICount);
	return /*( 0x80000000 )*/;
}


/* ************************************
 * Z80 Good-bits follow
 * ************************************
 */

void InitTables (void) {
	int i;
	UINT32 dw;

	for (i=0; i<256; i++)
	{
		int p=0;
		UINT8 bArg;
//		int j;

		if(i&1)   p++;
		if(i&2)   p++;
		if(i&4)   p++;
		if(i&8)   p++;
		if(i&16)  p++;
		if(i&32)  p++;
		if(i&64)  p++;
		if(i&128) p++;

//		SZ[i] = 0;
//		if(i==0)	SZ[i]|=ZF;	// SZ[i] |= i ? 0 : ZF;
//		if(i&0x80)	SZ[i]|=SF;	// SZ[i] |= i & SF;

		SZ[i] = i ? i & SF : ZF;
		SZ[i] |= (i & (YF | XF));		/* undocumented flag bits 5+3 */
		SZ_BIT[i] = i ? i & SF : ZF | PF;
		SZ_BIT[i] |= (i & (YF | XF));	/* undocumented flag bits 5+3 */

		//		PTable[i]  = (p&1) ? 0:VF;
		SZP[i]= SZ[i] | ((p&1) ? 0:VF);

		SZHV_inc[i] = SZ[i] | ((i == 0x80) ? VF : 0) | ((i & 0x0F) ? 0 : HF);

		//		SZHV_dec[i] = NF | ((i == 0x80) ? VF : 0) | ((i & 0x0F) ? 0 : HF) | SZ[(UINT8)i - 1];

		bArg = i;
		SZHV_dec[i] = NF|((bArg==0x80)?VF:0)|((bArg&0x0F)?0:HF);
		SZHV_dec[i]|=SZ[--bArg];

//		for( j=0; j<8; j++)	rgfBit[i][j] = HF | ((i & (1 << j))? ((j == 7) ? SF : 0) : ZF);
		}

		//	Calculate flag tables
		for ( dw = 0; dw <= 256 * 256; dw++)
		{
			UINT8 bLo = LOBYTE(LOWORD(dw));
			UINT8 bHi =	(LOWORD(dw))>>8;

			const UINT8 regA = bLo;
			const UINT8 bArg = bHi;

			int nAcc;
			UINT8 regF;
			int c;

			//	ADD
			nAcc = regA + bArg;
			regF = 0;

			regF |= SZ[nAcc&255];
			regF |= ((nAcc&256)>>8);
			regF |= ((regA^nAcc^bArg)&HF);
			regF |= (((bArg^regA^0x80)&(bArg^nAcc)&0x80)>>5);

			rgfAdd[regA][bArg] = regF;

			//	SUB
			nAcc  = regA - bArg;
			regF = SZ[nAcc&255]|((nAcc&256)>>8)|NF|((regA^nAcc^bArg)&HF)|(((bArg^regA)&(regA^nAcc)&0x80)>>5);

			rgfSub[regA][bArg] = regF;
/////
			for ( c = 0; c <= 1; c++)
			{
			//	ADC
			nAcc = regA + bArg + c;
			regF = 0;

			regF |= SZ[nAcc&255];
			regF |= ((nAcc&256)>>8);
			regF |= ((regA^nAcc^bArg)&HF);
			regF |= (((bArg^regA^0x80)&(bArg^nAcc)&0x80)>>5);

			rgfAddc[regA][bArg][c] = regF;

			//	SBC
			nAcc = regA - bArg - c;
			regF = SZ[nAcc&255]|((nAcc&256)>>8)|NF|((regA^nAcc^bArg)&HF)|(((bArg^regA)&(bArg^nAcc)&0x80)>>5);

			rgfSubc[regA][bArg][c] = regF;
		}
	}
}

/* reset registers, interrupts, memory read/write memory handlers,
 * port read and writing handlers, etc.
 */
extern void *z80_get_mem_ptr(void);
void z80_reset(void) {

	Z80.Base		=z80_get_mem_ptr();
	InitTables();//オカシイ

	zBC2 = 0xffff;
	zDE2 = 0xffff;
	zHL2 = 0xffff;
	zAF2 = 0xffff;
	zBC = 0xffff;
	zDE = 0xffff;
	zHL = 0xffff;
	zAF = 0xffff;
//	zR = 0;//rand();
	zI = 0xff;

	zIX = 0xffff;		// Yes, this intentional, and the way a Z80 comes
	zIY = 0xffff;		// up in power-on state!
	zF = ZF;			/* Zero flag is set */

	zHALT = FALSE;
	zIM = 0;
	zIFF1 = 0;
	zIFF2 = 0;
//	z80intAddr = 0x38;
//	z80nmiAddr = 0x66;

	SetPC(0x0000);
	SetSP(0x0000);

	zPendingInterrupt = FALSE;

	z80_ICount = 0;

//fprintf ( stderr, "Reset\n" );
}


//// Z80 IRQ follows.
void z80_set_irq_line0_off(void)
{
//	Z80.irq_state = CLEAR_LINE;
}
//UINT32 z80_set_irq_line0_on ( unsigned char bVal ) {
void z80_set_irq_line0_on( void ) {
	if(zIFF1 == 0)			// is IRQ disabled?
		return /*0xffffffff*/;		// Interrupt not taken!

	zPendingInterrupt = FALSE;

	zIFF1 = 0;

	if(zHALT)
	{
		SetPC(GetPC() + 1);
		zHALT = FALSE;
	}

	if(0 == zIM || 1 == zIM)
	{
		Rst(0x38/*z80intAddr*/);
		return/*(0)*/;		// We took the interrupt
	}

	Push(GetPC());
	SetPC(MemReadWord(MAKEWORD(0/*bVal*/, zI)));

	return/*(0)*/; // fix-me
}
#if 00
//// Z80 NMI follows.
void mz80nmi( void ) {
	zIFF1 = 0;

	if(zHALT)
	{
		SetPC(GetPC() + 1);
		zHALT = FALSE;
	}

	Rst(0x66/*z80nmiAddr*/);

	return/*(0)*/;
}
#endif
/* Initialize MZ80 for action */
// ワカンナイからこれでいい？？
void z80_init(void)
{
//	InitTables();
	z80_reset();//オカシイ
}
