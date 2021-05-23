/* ======================================================================== */
/* ========================= LICENSING & COPYRIGHT ======================== */
/* ======================================================================== */
#if 00
static const char* copyright_notice =
"MUSASHI\n"
"Version 2.0a (1999-02-21)\n"
"A portable Motorola M680x0 processor emulation engine.\n"
"Copyright 1999 Karl Stenerud.  All rights reserved.\n"
"\n"
"This code may be freely used for non-commercial purpooses as long as this\n"
"copyright notice remains unaltered in the source code and any binary files\n"
"containing this code in compiled form.\n"
"\n"
"Any commercial ventures wishing to use this code must contact the author\n"
"(Karl Stenerud) to negotiate commercial licensing terms.\n"
"\n"
"The latest version of this code can be obtained at:\n"
"http://milliways.scas.bcit.bc.ca/~karl/musashi\n"
;
#endif
/* ======================================================================== */
/* ================================= NOTES ================================ */
/* ======================================================================== */



/* ======================================================================== */
/* ================================ INCLUDES ============================== */
/* ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kmpu.h"
#include "m68kops.h"
#include <time.h>

/* ======================================================================== */
/* =============================== PROTOTYPES ============================= */
/* ======================================================================== */

/* Build the opcode handler table */
static void m68ki_build_opcode_table(void);



/* ======================================================================== */
/* ================================= DATA ================================= */
/* ======================================================================== */

static void 	(*m68ki_instruction_jump_table[0x10000])(void) __attribute__((aligned(64))) ; /* opcode handler jump table */

/* Pointers to speed up address register indirect with index calculation */
static uint* m68k_mpu_dar[2] = {REG_D, REG_A};

#undef EXT_MODE
#ifdef EXT_MODE
int 			m68ki_initial_cycles;			/* MEMO: call executes the initial cycles */
#endif
int 			m68ki_remaining_cycles = 0; 	/* Number of clocks remaining */
//uint			m68k_tracing = 0;

static uint 	m68k_emulation_initialized = 0; /* flag if emulation has been initialized */



/* Pointers to speed up movem instructions */
//uint* m68k_movem_pi_table[16] =
//{
//	 REG_D, REG_D+1, REG_D+2, REG_D+3, REG_D+4, REG_D+5, REG_D+6, REG_D+7,
//	 REG_A, REG_A+1, REG_A+2, REG_A+3, REG_A+4, REG_A+5, REG_A+6, REG_A+7
//};
//uint* m68k_movem_pd_table[16] =
//{
//	 REG_A+7, REG_A+6, REG_A+5, REG_A+4, REG_A+3, REG_A+2, REG_A+1, REG_A,
//	 REG_D+7, REG_D+6, REG_D+5, REG_D+4, REG_D+3, REG_D+2, REG_D+1, REG_D,
//};



/* ======================================================================== */
/* ================================= API ================================== */
/* ======================================================================== */

/* Peek at the internals of the M68K */
#if 00
int m68k_peek_dar(int reg_num)	{ return (reg_num < 0x10) ? REG_D[reg_num&0x0f] : 0; }
#define m68k_peek_dr(aaa)	m68k_peek_dar(aaa)
#define m68k_peek_ar(aaa)	m68k_peek_dar((aaa+8))
//int m68k_peek_dr(int reg_num) { return (reg_num < 8) ? REG_D[reg_num] : 0; }
//int m68k_peek_ar(int reg_num) { return (reg_num < 8) ? REG_A[reg_num] : 0; }
#else
#define m68k_peek_dr(aaa)	REG_D[aaa]
#define m68k_peek_ar(aaa)	REG_A[aaa]
#endif

#if 00
unsigned int m68k_peek_pc(void) { return ADDRESS_68K(REG_PC); }
unsigned int m68k_peek_ppc(void) { return ADDRESS_68K(REG_PPC); }
int m68k_peek_sr(void)			{ return m68ki_get_sr(); }
int m68k_peek_ir(void)			{ return REG_IR; }
int m68k_peek_t1_flag(void) 	{ return FLAG_T1 != 0; }
int m68k_peek_t0_flag(void) 	{ return FLAG_T0 != 0; }
int m68k_peek_s_flag(void)		{ return FLAG_S != 0; }
int m68k_peek_m_flag(void)		{ return FLAG_M != 0; }
int m68k_peek_int_mask(void)	{ return FLAG_INT_MASK; }
int m68k_peek_x_flag(void)		{ return FLAG_X != 0; }
int m68k_peek_n_flag(void)		{ return FLAG_N != 0; }
int m68k_peek_z_flag(void)		{ return FLAG_Z == 0; }
int m68k_peek_v_flag(void)		{ return FLAG_V != 0; }
int m68k_peek_c_flag(void)		{ return FLAG_C != 0; }
int m68k_peek_usp(void) 		{ return FLAG_S ? REG_USP : REG_A[7]; }
int m68k_peek_isp(void) 		{ return FLAG_S && !FLAG_M ? REG_A[7] : REG_ISP; }
int m68k_peek_msp(void) 		{ return FLAG_S && FLAG_M ? REG_A[7] : REG_MSP; }
#endif
#if 00
/* Poke data into the M68K */
void m68k_poke_dr(int reg_num, int value)	{ if(reg_num < 8) REG_D[reg_num] = MASK_OUT_ABOVE_32(value); }
void m68k_poke_ar(int reg_num, int value)	{ if(reg_num < 8) REG_A[reg_num] = MASK_OUT_ABOVE_32(value); }
void m68k_poke_pc(unsigned int value)		{ m68ki_set_pc(ADDRESS_68K(value)); }
void m68k_poke_sr(int value)				{ m68ki_set_sr(MASK_OUT_ABOVE_16(value)); }
void m68k_poke_ir(int value)				{ REG_IR = MASK_OUT_ABOVE_16(value); }
void m68k_poke_t1_flag(int value)			{ FLAG_T1 = (value != 0); }
void m68k_poke_t0_flag(int value)			{ /*if(FLAG_MODE & FLAG_MODE_020_PLUS) FLAG_T0 = (value != 0)*/; }
void m68k_poke_s_flag(int value)			{ m68ki_set_s_flag(value); }
void m68k_poke_m_flag(int value)			{ /*if(FLAG_MODE & FLAG_MODE_020_PLUS) m68ki_set_m_flag(value)*/; }
void m68k_poke_int_mask(int value)			{ FLAG_INT_MASK = value & 7; }
void m68k_poke_x_flag(int value)			{ FLAG_X = (value != 0); }
void m68k_poke_n_flag(int value)			{ FLAG_N = (value != 0); }
void m68k_poke_z_flag(int value)			{ FLAG_Z = (value == 0); }
void m68k_poke_v_flag(int value)			{ FLAG_V = (value != 0); }
void m68k_poke_c_flag(int value)			{ FLAG_C = (value != 0); }
void m68k_poke_usp(int value)
{
	if(FLAG_S)
		REG_USP = MASK_OUT_ABOVE_32(value);
	else
		REG_A[7] = MASK_OUT_ABOVE_32(value);
}
void m68k_poke_isp(int value)
{
	if(FLAG_S && !FLAG_M)
		REG_A[7] = MASK_OUT_ABOVE_32(value);
	else
		REG_ISP = MASK_OUT_ABOVE_32(value);
}
#endif


#if 00
void m68k_poke_msp(int value)
{
/*	if(FLAG_MODE & FLAG_MODE_020_PLUS)
	{
		if(FLAG_S && FLAG_M)
			REG_A[7] = MASK_OUT_ABOVE_32(value);
		else
			REG_MSP = MASK_OUT_ABOVE_32(value);
	}*/
}
void m68k_set_M68K_FLAG_MODE_68000(int dummy)
{
#if 00
	switch(M68K_FLAG_MODE_68000)
	{
		case M68K_FLAG_MODE_68000:
		case M68K_FLAG_MODE_68010:
		case M68K_FLAG_MODE_68020:
			FLAG_MODE = M68K_FLAG_MODE_68000;
			return;
		default:
			FLAG_MODE = M68K_DEFAULT_FLAG_MODE;
	}
#endif
}
#endif

//clock_t t1, t2 ;
//clock_t times[0x10000] ;
//int	  num_t[0x10000] ;

/* Execute some instructions until we use up num_clks clock cycles */
/* ASG: removed per-instruction interrupt checks */
int m68k_execute(int num_clks)
{

	/* Make sure we're not stopped */
	if(!CPU_STOPPED)
	{
#ifdef EXT_MODE
		m68ki_initial_cycles = m68ki_remaining_cycles;
#endif
		/* Set our pool of clock cycles available */
		m68ki_remaining_cycles = num_clks;

		/* ASG: update cycles */
		m68ki_remaining_cycles -= CPU_INT_CYCLES;
		CPU_INT_CYCLES = 0;

		/* Main loop.  Keep going until we run out of clock cycles */
		do
		{
			/* Read an instruction and call its handler */
			REG_IR = m68ki_read_instruction();
			m68ki_instruction_jump_table[REG_IR]();

			/*
			if( REG_IR == 19064 )
			{
				//num_t[ REG_IR ] = 0 ;
			}
			t1 = clock() ;
			m68ki_instruction_jump_table[REG_IR]();
			t2 = clock() ;
			times[ REG_IR ] += t2 - t1 ;
			num_t[ REG_IR ] ++ ;
			//*/
			continue;
		} while(m68ki_remaining_cycles > 0);

		/* set previous PC to current PC for the next entry into the loop */
		REG_PPC = REG_PC;

		/* ASG: update cycles */
		m68ki_remaining_cycles -= CPU_INT_CYCLES;
		CPU_INT_CYCLES = 0;

		/* return how many clocks we used */
		return num_clks - m68ki_remaining_cycles;
	}


	/* We get here if the MPU is stopped */
	m68ki_remaining_cycles = 0;
	return num_clks;
}

#ifdef EXT_MODE
int m68k_cycles_run(void)
{
	return m68ki_initial_cycles - m68ki_remaining_cycles;
}
#endif

/* ASG: rewrote so that the int_line is a mask of the IPL0/IPL1/IPL2 bits */
//void m68k_set_irq(unsigned int int_level)
void m68k_set_irq(unsigned int int_line)
//void m68k_assert_irq(int int_line)
{
	uint old_level = CPU_INT_LEVEL;
	CPU_INT_LEVEL = (int_line&7) << 8;

	/* A transition from < 7 to 7 always interrupts (NMI) */
	/* Note: Level 7 can also level trigger like a normal IRQ */
	if(old_level != 0x0700 && CPU_INT_LEVEL == 0x0700)
		m68ki_exception_interrupt(7); /* Edge triggered level 7 (NMI) */
	else
		m68ki_check_interrupts(); /* Level triggered (IRQ) */

#if 00
	/* OR in the bits of the interrupt */
	int old_state = CPU_INT_STATE;
	CPU_INT_STATE = 0;	/* ASG: remove me to do proper mask setting */
	CPU_INT_STATE |= int_line & 7;

	/* if it's NMI, we're edge triggered */
	if (CPU_INT_STATE == 7)
	{
		if (old_state != 7)
			m68ki_exception_interrupt(1 << 7);
	}

	/* other interrupts just reflect the current state */
	else
		m68ki_check_interrupts();
//}
///* ASG: rewrote so that the int_line is a mask of the IPL0/IPL1/IPL2 bits */
//void m68k_clear_irq(int int_line)
//{
	/* AND in the bits of the interrupt */
	CPU_INT_STATE &= ~int_line & 7;
	CPU_INT_STATE = 0;	/* ASG: remove me to do proper mask setting */

	/* check for interrupts again */
	m68ki_check_interrupts();
#endif
}


/* Reset the M68K */
void m68k_pulse_reset(void)//;// *param)
{
//	CPU_HALTED = 0;
	CPU_STOPPED = 0;

///	CPU_INT_STATE = 0;	/* ASG: changed from CPU_INTS_PENDING */


	FLAG_T1 = FLAG_T0 = 0;
	m68ki_clear_trace();
	FLAG_S = 1;
	FLAG_M = 0;

	//CPU_INT_LEVEL=0;//???
	/* Interrupt mask to level 7 */
/// FLAG_INT_MASK = 7;
	FLAG_INT_MASK = 0x0700;
/* ASG: removed a bunch of this initialization....
	FLAG_X = FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 1;
	REG_USP = 0;
	REG_MSP = 0;
	REG_IR = 0;
	REG_D[0] = REG_D[1] = REG_D[2] = REG_D[3] =
	REG_D[4] = REG_D[5] = REG_D[6] = REG_D[7] =
	REG_A[0] = REG_A[1] = REG_A[2] = REG_A[3] =
	REG_A[4] = REG_A[5] = REG_A[6] = REG_A[7] = 0;*/
//	 FLAG_VBR = 0;
/*	FLAG_SFC = 0;
	REG_DFC = 0;*/
	REG_A[7] = /*REG_ISP =*/ m68ki_read_32(0);
	m68ki_set_pc(m68ki_read_32(4));
	m68ki_remaining_cycles = 0;

//	 if (FLAG_MODE == 0) FLAG_MODE = M68K_DEFAULT_FLAG_MODE;	/* KW 990319 */
	/* The first call to this function initializes the opcode handler jump table */
	if(m68k_emulation_initialized)
		return;
	else
	{
		m68ki_build_opcode_table();
/*		if(FLAG_MODE == 0)
		FLAG_MODE = M68K_DEFAULT_FLAG_MODE;*/

		m68k_emulation_initialized = 1;
	}
}


/* Get and set the current MPU context */
/* This is to allow for multiple CPUs */

unsigned int m68k_get_context(void* dst)
{
	if(dst) *(m68ki_mpu_core*)dst = m68ki_mpu;
	return sizeof(m68ki_mpu_core);
}

void m68k_set_context(void* src)
{
	if(src) m68ki_mpu = *(m68ki_mpu_core*)src;
}



/* Check if the instruction is a valid one */
int m68k_is_valid_instruction(int instruction, int dummy)
{
	if(m68ki_instruction_jump_table[MASK_OUT_ABOVE_16(instruction)] == m68k_op_illegal)
		return 0;
	return 1;
}

#include"m68kopac.c"
#include"m68kopdm.c"
#include"m68kops.c"
#include"m68kopnz.c"

/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */
