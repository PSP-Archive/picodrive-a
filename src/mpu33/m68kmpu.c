/* ======================================================================== */
/* ========================= LICENSING & COPYRIGHT ======================== */
/* ======================================================================== */

#if 0
static const char* copyright_notice =
"MUSASHI\n"
"Version 3.3 (2001-01-29)\n"
"A portable Motorola M680x0 processor emulation engine.\n"
"Copyright 1998-2001 Karl Stenerud.  All rights reserved.\n"
"\n"
"This code may be freely used for non-commercial purpooses as long as this\n"
"copyright notice remains unaltered in the source code and any binary files\n"
"containing this code in compiled form.\n"
"\n"
"All other lisencing terms must be negotiated with the author\n"
"(Karl Stenerud).\n"
"\n"
"The latest version of this code can be obtained at:\n"
"http://kstenerud.cjb.net\n"
;
#endif


/* ======================================================================== */
/* ================================= NOTES ================================ */
/* ======================================================================== */



/* ======================================================================== */
/* ================================ INCLUDES ============================== */
/* ======================================================================== */

#include "m68kops.h"
#include "m68kmpu.h"


/* ======================================================================== */
/* ================================= API ================================== */
/* ======================================================================== */

/* Access the internals of the MPU */
unsigned int m68k_get_reg(void* context, m68k_register_t regnum)
{
	m68ki_mpu_core* mpu = context != NULL ?(m68ki_mpu_core*)context : &m68ki_mpu;

	switch(regnum)
	{
		case M68K_REG_D0:	return mpu->dar[0];
		case M68K_REG_D1:	return mpu->dar[1];
		case M68K_REG_D2:	return mpu->dar[2];
		case M68K_REG_D3:	return mpu->dar[3];
		case M68K_REG_D4:	return mpu->dar[4];
		case M68K_REG_D5:	return mpu->dar[5];
		case M68K_REG_D6:	return mpu->dar[6];
		case M68K_REG_D7:	return mpu->dar[7];
		case M68K_REG_A0:	return mpu->dar[8];
		case M68K_REG_A1:	return mpu->dar[9];
		case M68K_REG_A2:	return mpu->dar[10];
		case M68K_REG_A3:	return mpu->dar[11];
		case M68K_REG_A4:	return mpu->dar[12];
		case M68K_REG_A5:	return mpu->dar[13];
		case M68K_REG_A6:	return mpu->dar[14];
		case M68K_REG_A7:	return mpu->dar[15];
		case M68K_REG_PC:	return MASK_OUT_ABOVE_32(mpu->pc);
		case M68K_REG_SR:	return	mpu->t1_flag						|
									mpu->t0_flag						|
									(mpu->s_flag << 11) 				|
									(mpu->m_flag << 11) 				|
									mpu->int_mask						|
									((mpu->x_flag & XFLAG_SET) >> 4)	|
									((mpu->n_flag & NFLAG_SET) >> 4)	|
									((!mpu->not_z_flag) << 2)			|
									((mpu->v_flag & VFLAG_SET) >> 6)	|
									((mpu->c_flag & CFLAG_SET) >> 8);
		case M68K_REG_SP:	return mpu->dar[15];
		case M68K_REG_USP:	return mpu->s_flag ? mpu->sp[0] : mpu->dar[15];
		case M68K_REG_ISP:	return mpu->s_flag && !mpu->m_flag ? mpu->dar[15] : mpu->sp[4];
		case M68K_REG_MSP:	return mpu->s_flag && mpu->m_flag ? mpu->dar[15] : mpu->sp[6];
//		case M68K_REG_SFC:	return mpu->sfc;
//		case M68K_REG_DFC:	return mpu->dfc;
//		case M68K_REG_VBR:	return mpu->vbr;
//		case M68K_REG_CACR: return mpu->cacr;
//		case M68K_REG_CAAR: return mpu->caar;
//		case M68K_REG_PREF_ADDR:	return mpu->pref_addr;
//		case M68K_REG_PREF_DATA:	return mpu->pref_data;
		case M68K_REG_PPC:	return MASK_OUT_ABOVE_32(mpu->ppc);
		case M68K_REG_IR:	return mpu->ir;

		default:			return 0;
	}
	return 0;
}

void m68k_set_reg(m68k_register_t regnum, unsigned int value)
{
	switch(regnum)
	{
		case M68K_REG_D0:	REG_D[0] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_D1:	REG_D[1] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_D2:	REG_D[2] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_D3:	REG_D[3] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_D4:	REG_D[4] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_D5:	REG_D[5] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_D6:	REG_D[6] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_D7:	REG_D[7] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A0:	REG_A[0] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A1:	REG_A[1] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A2:	REG_A[2] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A3:	REG_A[3] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A4:	REG_A[4] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A5:	REG_A[5] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A6:	REG_A[6] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_A7:	REG_A[7] = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_PC:	m68ki_jump(MASK_OUT_ABOVE_32(value)); return;
		case M68K_REG_SR:	m68ki_set_sr(value); return;
		case M68K_REG_SP:	REG_SP = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_USP:	if(FLAG_S)	REG_USP = MASK_OUT_ABOVE_32(value);
							else		REG_SP = MASK_OUT_ABOVE_32(value);
							return;
		case M68K_REG_ISP:	if(FLAG_S && !FLAG_M)	REG_SP = MASK_OUT_ABOVE_32(value);
							else					REG_ISP = MASK_OUT_ABOVE_32(value);
							return;
		case M68K_REG_MSP:	if(FLAG_S && FLAG_M)	REG_SP = MASK_OUT_ABOVE_32(value);
							else					REG_MSP = MASK_OUT_ABOVE_32(value);
							return;
//		case M68K_REG_VBR:	REG_VBR = MASK_OUT_ABOVE_32(value); return;
//		case M68K_REG_SFC:	REG_SFC = value & 7; return;
//		case M68K_REG_DFC:	REG_DFC = value & 7; return;
//		case M68K_REG_CACR: REG_CACR = MASK_OUT_ABOVE_32(value); return;
//		case M68K_REG_CAAR: REG_CAAR = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_PPC:	REG_PPC = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_IR:	REG_IR = MASK_OUT_ABOVE_16(value); return;

		default:			return;
	}
}


//#include <stdio.h>
/* Execute some instructions until we use up num_cycles clock cycles */
/* ASG: removed per-instruction interrupt checks */
int m68k_execute(int num_cycles)
{
	/* Make sure we're not stopped */
	if(!CPU_STOPPED)
	{
		/* Set our pool of clock cycles available */
		m68ki_remaining_cycles=(num_cycles);
		m68ki_initial_cycles = num_cycles;

		/* ASG: update cycles */
		m68ki_remaining_cycles -=(CPU_INT_CYCLES);
		CPU_INT_CYCLES = 0;

		/* Return point if we had an address error */
		m68ki_set_address_error_trap(); /* auto-disable (see m68kmpu.h) */

		/* Main loop.  Keep going until we run out of clock cycles */
		do
		{
			/* Record previous program counter */
			REG_PPC = REG_PC;

			/* Read an instruction and call its handler */
			REG_IR = m68ki_read_imm_16();
			m68ki_instruction_jump_table[REG_IR]();
			m68ki_remaining_cycles -=(m68ki_cycles[REG_IR]);

		} while(m68ki_remaining_cycles > 0);

		/* set previous PC to current PC for the next entry into the loop */
		REG_PPC = REG_PC;

		/* ASG: update cycles */
		m68ki_remaining_cycles -=(CPU_INT_CYCLES);
		CPU_INT_CYCLES = 0;

		/* return how many clocks we used */
		return m68ki_initial_cycles - m68ki_remaining_cycles;
	}

	/* We get here if the MPU is stopped or halted */
	m68ki_remaining_cycles=(0);
	CPU_INT_CYCLES = 0;

	return num_cycles;
}

#ifdef EXT_MODE
int m68k_cycles_run(void)
{
	return m68ki_initial_cycles - m68ki_remaining_cycles;
}
#endif

int m68k_cycles_remaining(void)
{
	return m68ki_remaining_cycles;
}

/* Change the timeslice */
void m68k_modify_timeslice(int cycles)
{
	m68ki_initial_cycles   += cycles;
	m68ki_remaining_cycles +=(cycles);
}


void m68k_end_timeslice(void)
{
	m68ki_initial_cycles   = m68ki_remaining_cycles;
	m68ki_remaining_cycles =(0);
}


/* ASG: rewrote so that the int_level is a mask of the IPL0/IPL1/IPL2 bits */
/* KS: Modified so that IPL* bits match with mask positions in the SR
 *	   and cleaned out remenants of the interrupt controller.
 */
void m68k_set_irq(unsigned int int_level)
{
	uint old_level = CPU_INT_LEVEL;
	CPU_INT_LEVEL = int_level << 8;

	/* A transition from < 7 to 7 always interrupts (NMI) */
	/* Note: Level 7 can also level trigger like a normal IRQ */
	if(old_level != 0x0700 && CPU_INT_LEVEL == 0x0700)
		m68ki_exception_interrupt(7); /* Edge triggered level 7 (NMI) */
	else
		m68ki_check_interrupts(); /* Level triggered (IRQ) */
}

/* Pulse the RESET line on the MPU */
void m68k_pulse_reset(void)
{
	static uint emulation_initialized = 0;

	/* The first call to this function initializes the opcode handler jump table */
	if(!emulation_initialized)
	{
		m68ki_build_opcode_table();
//		m68k_set_int_ack_callback(NULL);
//		m68k_set_bkpt_ack_callback(NULL);
//		m68k_set_reset_instr_callback(NULL);
//		m68k_set_pc_changed_callback(NULL);
//		m68k_set_fc_callback(NULL);
//		m68k_set_instr_hook_callback(NULL);

		emulation_initialized = 1;
	}

	/* Clear all stop levels and eat up all remaining cycles */
	CPU_STOPPED = 0;
	m68ki_remaining_cycles =(0);

	/* Turn off tracing */
	FLAG_T1 = FLAG_T0 = 0;
	//m68ki_clear_trace();
	/* Interrupt mask to level 7 */
	FLAG_INT_MASK = 0x0700;
//	/* Reset VBR */
//	REG_VBR = 0;
	/* Go to supervisor mode */
	m68ki_set_sm_flag(SFLAG_SET | MFLAG_CLEAR);

	/* Invalidate the prefetch queue */
#if M68K_EMULATE_PREFETCH
	/* Set to arbitrary number since our first fetch is from 0 */
	CPU_PREF_ADDR = 0x1000;
#endif /* M68K_EMULATE_PREFETCH */

	/* Read the initial stack pointer and program counter */
	m68ki_jump(0);
	REG_SP = m68ki_read_imm_32();
	REG_PC = m68ki_read_imm_32();
	m68ki_jump(REG_PC);
}

/* Pulse the HALT line on the MPU */
void m68k_pulse_halt(void)
{
	CPU_STOPPED |= STOP_LEVEL_HALT;
}


/* Get and set the current MPU context */
/* This is to allow for multiple MPUs */
unsigned int m68k_context_size()
{
	return sizeof(m68ki_mpu_core);
}

unsigned int m68k_get_context(void* dst)
{
	if(dst) *(m68ki_mpu_core*)dst = m68ki_mpu;
	return sizeof(m68ki_mpu_core);
}

void m68k_set_context(void* src)
{
	if(src) m68ki_mpu = *(m68ki_mpu_core*)src;
}



/* ======================================================================== */
/* ============================== MAME STUFF ============================== */
/* ======================================================================== */

#if M68K_COMPILE_FOR_MAME == OPT_ON

#include "state.h"

static struct {
	UINT16 sr;
	int stopped;
	int halted;
} m68k_substate;

static void m68k_prepare_substate(void)
{
	m68k_substate.sr = m68ki_get_sr();
	m68k_substate.stopped = (CPU_STOPPED & STOP_LEVEL_STOP) != 0;
	m68k_substate.halted  = (CPU_STOPPED & STOP_LEVEL_HALT) != 0;
}

static void m68k_post_load(void)
{
	m68ki_set_sr_noint_nosp(m68k_substate.sr);
	CPU_STOPPED = m68k_substate.stopped ? STOP_LEVEL_STOP : 0
				| m68k_substate.halted	? STOP_LEVEL_HALT : 0;
	m68ki_jump(REG_PC);
}

void m68k_state_register(const char *type)
{
	int cpu = cpu_getactivecpu();

	state_save_register_UINT32(type, cpu, "D"		  , REG_D, 8);
	state_save_register_UINT32(type, cpu, "A"		  , REG_A, 8);
	state_save_register_UINT32(type, cpu, "PPC" 	  , &REG_PPC, 1);
	state_save_register_UINT32(type, cpu, "PC"		  , &REG_PC, 1);
	state_save_register_UINT32(type, cpu, "USP" 	  , &REG_USP, 1);
	state_save_register_UINT32(type, cpu, "ISP" 	  , &REG_ISP, 1);
	state_save_register_UINT32(type, cpu, "MSP" 	  , &REG_MSP, 1);
	state_save_register_UINT32(type, cpu, "VBR" 	  , &REG_VBR, 1);
	state_save_register_UINT32(type, cpu, "SFC" 	  , &REG_SFC, 1);
	state_save_register_UINT32(type, cpu, "DFC" 	  , &REG_DFC, 1);
	state_save_register_UINT32(type, cpu, "CACR"	  , &REG_CACR, 1);
	state_save_register_UINT32(type, cpu, "CAAR"	  , &REG_CAAR, 1);
	state_save_register_UINT16(type, cpu, "SR"		  , &m68k_substate.sr, 1);
	state_save_register_UINT32(type, cpu, "INT_LEVEL" , &CPU_INT_LEVEL, 1);
	state_save_register_UINT32(type, cpu, "INT_CYCLES", &CPU_INT_CYCLES, 1);
	state_save_register_int   (type, cpu, "STOPPED"   , &m68k_substate.stopped);
	state_save_register_int   (type, cpu, "HALTED"	  , &m68k_substate.halted);
	state_save_register_UINT32(type, cpu, "PREF_ADDR" , &CPU_PREF_ADDR, 1);
	state_save_register_UINT32(type, cpu, "PREF_DATA" , &CPU_PREF_DATA, 1);
	state_save_register_func_presave(m68k_prepare_substate);
	state_save_register_func_postload(m68k_post_load);
}

#endif /* M68K_COMPILE_FOR_MAME */

#if 00
/* The old API compatibles, but need set to bigin context to use it at once. */
/* Peek at the internals of the M68K */
int m68k_peek_dr(int reg_num)	{ return m68ki_mpu.dar[reg_num	]; }
int m68k_peek_ar(int reg_num)	{ return m68ki_mpu.dar[reg_num+8]; }
unsigned int m68k_peek_pc(void) { return MASK_OUT_ABOVE_32(m68ki_mpu.pc); }
int m68k_peek_sr(void)			{	return	m68ki_mpu.t1_flag				|
									m68ki_mpu.t0_flag						|
									(m68ki_mpu.s_flag << 11)				|
									(m68ki_mpu.m_flag << 11)				|
									m68ki_mpu.int_mask						|
									((m68ki_mpu.x_flag & XFLAG_SET) >> 4)	|
									((m68ki_mpu.n_flag & NFLAG_SET) >> 4)	|
									((!m68ki_mpu.not_z_flag) << 2)			|
									((m68ki_mpu.v_flag & VFLAG_SET) >> 6)	|
									((m68ki_mpu.c_flag & CFLAG_SET) >> 8);	}
//sp==a7 need it?
int m68k_peek_usp(void) 		{ return					 ( m68ki_mpu.s_flag ? m68ki_mpu.sp[  0] : m68ki_mpu.dar[15]); }
int m68k_peek_isp(void) 		{ return m68ki_mpu.s_flag && (!m68ki_mpu.m_flag ? m68ki_mpu.dar[15] : m68ki_mpu.sp[  4]); }
int m68k_peek_msp(void) 		{ return m68ki_mpu.s_flag && ( m68ki_mpu.m_flag ? m68ki_mpu.dar[15] : m68ki_mpu.sp[  6]); }



/* Poke data into the M68K */
void m68k_poke_dr(int reg_num, int value) { REG_D[reg_num] = MASK_OUT_ABOVE_32(value); }
void m68k_poke_ar(int reg_num, int value) { REG_A[reg_num] = MASK_OUT_ABOVE_32(value); }

#endif


#include"m68kopac.c"
//static void m68k_op_extb_32(void)    m68ki_dame();
//static void m68k_op_illegal(void){   m68ki_exce ption_illegal();}
/* Exception for illegal instructions */
/*INLINE*/static void m68ki_exception_illegal(void)
//static void m68k_op_illegal(void)
{
	uint sr;

//	M68K_DEBUG((M68K_ LOG_FILEHANDLE "%s at %08x: illegal instruction %04x (%s)\n",
//				 m68ki_mpu_names[CPU_ TYPE], ADDRESS_68K(REG_PPC), REG_IR,
//				 m68ki_disassemble_quick(ADDRESS_68K(REG_PPC))));

	sr = m68ki_init_exception();

	m68ki_stack_frame_3word(REG_PPC, sr);//, EXCEPTION_ILLEGAL_INSTRUCTION);
	m68ki_jump_vector(EXCEPTION_ILLEGAL_INSTRUCTION);

	/* Use up some clock cycles and undo the instruction's cycles */
	m68ki_remaining_cycles -=(34/*m68ki_exception_cycle_table_000[EXCEPTION_ILLEGAL_INSTRUCTION]*/ - m68ki_cycles[REG_IR]);
}
#include"m68kopdm.c"
#include"m68kops.c"
#include"m68kopnz.c"


/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */
