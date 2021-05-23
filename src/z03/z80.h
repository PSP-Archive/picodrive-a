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

/******************************************************************************
 * Union of UINT8, UINT16 and UINT32 in native endianess of the target
 * This is used to access bytes and words in a machine independent manner.
 * The upper bytes h2 and h3 normally contain zero (16 bit CPU cores)
 * thus PAIR.d can be used to pass arguments to the memory system
 * which expects 'int' really.
 ******************************************************************************/

extern void z80_reset(void);
extern void z80_end_timeslice(void);
extern int  z80_execute(short cycles);
extern void z80_set_irq_line(int irqline, int state);
extern void z80_set_irq_line0_off(void);
extern void z80_set_irq_line0_on(void);

extern void z80_init(void);

extern void z80_debt_clk(short cycles);//borrow z80 clock on after payment out.

#endif	//_Z80_H_
