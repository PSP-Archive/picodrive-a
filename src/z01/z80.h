#ifndef _Z80_H_
#define _Z80_H_

//#include "cpuintrf.h"
//#include "osd_cpu.h"

typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;

typedef signed char 		INT8;
typedef signed short		INT16;
typedef signed int			INT32;

#ifndef NULL
#define NULL   (0)
#endif


extern void z80_reset(void);

extern void z80_execute(UINT32);
extern void z80_set_irq_line0_off(void);
extern void z80_set_irq_line0_on(void);//now force int(0)

extern void z80_init(void);

extern void z80_debt_clk(int cycles);//borrow z80 clock on after payment out.


//tern UINT32 z80_execute(UINT32);
//extern UINT32 mz80GetContextSize(void);
////extern UINT32 mz80GetElapsedTicks(UINT32);	// îpé~
//extern void mz80ReleaseTimeslice(void);
//extern void mz80GetContext(void *);
//extern void mz80SetContext(void *);
//extern void mz80ClearPendingInterrupt(void);
////extern UINT32 z80_set_irq_line0_on(unsigned char dwIntVector_LowAddr);
//extern UINT32 mz80nmi(void);
//extern UINT32 z80intAddr;
//extern UINT32 z80nmiAddr;

// Debugger useful routines

//extern unsigned char mz80SetRegisterValue(void *, UINT32, UINT32);
//extern UINT32 mz80GetRegisterValue(void *, UINT32);
//extern UINT32 mz80GetRegisterTextValue(void *, UINT32, unsigned char *);
//extern unsigned char *mz80GetRegisterName(UINT32);

// Memory/IO read/write commands

//#ifndef VALUE_BYTE
//#define VALUE_BYTE	0
//#endif
//
//#ifndef VALUE_WORD
//#define VALUE_WORD	1
//#endif
//
//#ifndef VALUE_DWORD
//#define VALUE_DWORD 2
//#endif

//#ifndef VALUE_IO
//#define	VALUE_IO	3
//#endif

//extern void mz80WriteValue(unsigned char bWhat, UINT32 dwAddr, UINT32 dwData);
//extern UINT32 mz80ReadValue(unsigned char bWhat, UINT32 dwAddr);



//#define NO_INST2_BRK {pbPC-=2;NO_INST();break;}


#endif	// _Z80_H_
