#ifndef _Z80_H_
#define _Z80_H_



typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;

typedef signed char 		INT8;
typedef signed short		INT16;
typedef signed int			INT32;


#define CLEAR_LINE				0
#define ASSERT_LINE 			1






extern void z80_reset(void);
//int param);
extern int  z80_execute(short cycles);
extern void z80_set_irq_line(int irqline, int state);
extern void z80_set_irq_line0_off(void);
extern void z80_set_irq_line0_on(void);

extern void z80_init(void);

extern void z80_debt_clk(short cycles);//borrow z80 clock on after payment out.

//void z80_exit(void);
//extern void z80_enable(int enable);

//#ifdef SAVE_STATE
//void state_save_z80(u8 *state_buf);
//void state_load_z80(u8 *state_buf);
//#endif

#endif	//_Z80_H_
