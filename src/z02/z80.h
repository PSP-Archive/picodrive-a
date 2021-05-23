/* Multi-Z80 32 Bit emulator */

/* Copyright 1999, Neil Bradley, All rights reserved
 *
 * License agreement:
 *
 * The mZ80 emulator may be distributed in unmodified form to any medium.
 *
 * mZ80 May not be sold, or sold as a part of a commercial package without
 * the express written permission of Neil Bradley (neil@synthcom.com). This
 * includes shareware.
 *
 * Modified versions of mZ80 may not be publicly redistributed without author
 * approval (neil@synthcom.com). This includes distributing via a publicly
 * accessible LAN. You may make your own source modifications and distribute
 * mZ80 in object only form.
 *
 * mZ80 Licensing for commercial applications is available. Please email
 * neil@synthcom.com for details.
 *
 * Synthcom Systems, Inc, and Neil Bradley will not be held responsible for
 * any damage done by the use of mZ80. It is purely "as-is".
 *
 * If you use mZ80 in a freeware application, credit in the following text:
 *
 * "Multi-Z80 CPU emulator by Neil Bradley (neil@synthcom.com)"
 *
 * must accompany the freeware application within the application itself or
 * in the documentation.
 *
 * Legal stuff aside:
 *
 * If you find problems with mZ80, please email the author so they can get
 * resolved. If you find a bug and fix it, please also email the author so
 * that those bug fixes can be propogated to the installed base of mZ80
 * users. If you find performance improvements or problems with mZ80, please
 * email the author with your changes/suggestions and they will be rolled in
 * with subsequent releases of mZ80.
 *
 * The whole idea of this emulator is to have the fastest available 32 bit
 * Multi-z80 emulator for the PC, giving maximum performance.
 */

/* General z80 based defines */

//#include "rccore/types.h"

/* extra declarations for z80stb.c to make life easier.
 */






#ifndef	_MZ80_H_
#define	_MZ80_H_

typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;

typedef signed char 		INT8;
typedef signed short		INT16;
typedef signed int			INT32;

#ifndef UINT32
#define UINT32  unsigned long
#endif

#ifndef UINT16
#define UINT16  unsigned short
#endif

#ifndef UINT8
#define UINT8   unsigned char
#endif

#ifdef __cplusplus
extern "C" {
#endif



//struct z80TrapRec
//{
//	UINT16 trapAddr;
//	UINT8  skipCnt;
//	UINT8  origIns;
//};




extern void z80_reset(void);

extern void z80_execute(UINT32);
extern void z80_set_irq_line0_off(void);
extern void z80_set_irq_line0_on(void);//now force int(0)

extern void z80_init(void);

extern void z80_debt_clk(int cycles);//borrow z80 clock on after payment out.




//extern UINT8 *mz80Base;
//extern UINT32 mz80GetContextSize(void);
//extern UINT32 mz80GetElapsedTicks(UINT32);
//extern void mz80ReleaseTimeslice();
//extern void mz80GetContext(void *);
//extern void mz80SetContext(void *);
//extern UINT32 z80_set_irq_line0_on(UINT32);
//extern void mz80nmi(void);
//extern UINT16 z80intAddr;
//extern UINT16 z80nmiAddr;
//extern UINT16 z80pc;

#ifdef __cplusplus
};
#endif

#endif	// _MZ80_H_









#ifndef h_z80stb_h
#define h_z80stb_h

/* RC hack */
//#define DWORD           UINT32
//#define WORD            UINT16
//#define BYTE            UINT8
//#define LPBYTE          UINT8*
//#define ULONG           unsigned long
//#define BOOL            UINT8



//typedef struct {
//  UINT16 AF,BC,DE,HL,IX,IY,PC,SP;
//  UINT16 AF2,BC2,DE2,HL2;
//  unsigned IFF1,IFF2,HALT,IM,I,R,R2;
//} EXECZ80_CONTEXT;

// typedef UINT8 (*EXECZ80_MEMREAD_PROC)(UINT16);
// typedef void (*EXECZ80_MEMWRITE_PROC)(UINT16, UINT8);
// typedef UINT8 (*EXECZ80_PORTREAD_PROC)(UINT8);
// typedef void (*EXECZ80_PORTWRITE_PROC)(UINT8, UINT8);

// #define Z80_END_TABLE     ((EXECZ80_MEMREAD_PROC) 0xFFFFFFFF)
// #define Z80_IGNORE_READ   ((EXECZ80_MEMREAD_PROC) 0xFFFFFFFE)
// #define Z80_IGNORE_WRITE  ((EXECZ80_MEMWRITE_PROC)0xFFFFFFFE)
// #define Z80_ILLEGAL_READ  ((EXECZ80_MEMREAD_PROC) 0xFFFFFFFD)
// #define Z80_ILLEGAL_WRITE ((EXECZ80_MEMWRITE_PROC)0xFFFFFFFD)

// typedef struct {
//   UINT32 dwAddrMin;
//   UINT32 dwAddrMax;
//   UINT32 dwFlags;
//   EXECZ80_MEMREAD_PROC pfnMemRead;
//   EXECZ80_MEMWRITE_PROC pfnMemWrite;
// } EXECZ80_MEM_DESCRIPTOR;

// typedef struct {
//   UINT32 dwFlags;
//   UINT8* rgbMemory;
//   const UINT8 *rgbInstructions;
//   EXECZ80_MEMREAD_PROC pfnMemRead;
//   EXECZ80_MEMWRITE_PROC pfnMemWrite;
//   EXECZ80_PORTREAD_PROC pfnPortRead;
//   EXECZ80_PORTWRITE_PROC pfnPortWrite;
//   const EXECZ80_MEM_DESCRIPTOR *pmd;
// } EXECZ80_CREATE;


/* int Create ( EXECZ80_CREATE &cs, IExecZ80 *&pExec ); */
/* int CExecZ80 ( EXECZ80_CREATE &cs ); */
/* int ~CExecZ80(); */

//int Irq(UINT8 bVal);
//int Nmi();
//int Exec(int cCyclesArg);



#endif

