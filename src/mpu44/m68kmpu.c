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

//extern void m68040_fpu_op0(void);
//extern void m68040_fpu_op1(void);

#include "m68kops.h"
#include "m68kmpu.h"
//#include "m68kfpu.c"



/* ======================================================================== */
/* =============================== CALLBACKS ============================== */
/* ======================================================================== */

/* Default callbacks used if the callback hasn't been set yet, or if the
 * callback is set to NULL
 */

/* Interrupt acknowledge */
//static int default_int_ack_callback_data;
//static int default_int_ack_callback(int int_level)
//{
//	default_int_ack_callback_data = int_level;
//	MPU_INT_LEVEL = 0;
//	return M68K_INT_ACK_AUTOVECTOR;
//}

/* Breakpoint acknowledge */
//static unsigned int default_bkpt_ack_callback_data;
//static void default_bkpt_ack_callback(unsigned int data)
//{
//	default_bkpt_ack_callback_data = data;
//}

/* Called when a reset instruction is executed */
//static void default_reset_instr_callback(void)
//{
//}

/* Called when a cmpi.l #v, dn instruction is executed */
//static void default_cmpild_instr_callback(unsigned int val, int reg)
//{
//}

/* Called when a rte instruction is executed */
//static void default_rte_instr_callback(void)
//{
//}

/* Called when the program counter changed by a large value */
//static unsigned int default_pc_changed_callback_data;
//static void default_pc_changed_callback(unsigned int new_pc)
//{
//	default_pc_changed_callback_data = new_pc;
//}

/* Called every time there's bus activity (read/write to/from memory */
//static unsigned int default_set_fc_callback_data;
//static void default_set_fc_callback(unsigned int new_fc)
//{
//	default_set_fc_callback_data = new_fc;
//}

/* Called every instruction cycle prior to execution */
//static void default_instr_hook_callback(void)
//{
//}



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
									(mpu->s_flag << 11)					|
									(mpu->m_flag << 11)					|
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
//		case M68K_REG_CACR:	return mpu->cacr;
//		case M68K_REG_CAAR:	return mpu->caar;
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
		case M68K_REG_USP:	if(FLAG_S)
								REG_USP = MASK_OUT_ABOVE_32(value);
							else
								REG_SP = MASK_OUT_ABOVE_32(value);
							return;
		case M68K_REG_ISP:	if(FLAG_S && !FLAG_M)
								REG_SP = MASK_OUT_ABOVE_32(value);
							else
								REG_ISP = MASK_OUT_ABOVE_32(value);
							return;
		case M68K_REG_MSP:	if(FLAG_S && FLAG_M)
								REG_SP = MASK_OUT_ABOVE_32(value);
							else
								REG_MSP = MASK_OUT_ABOVE_32(value);
							return;
//		case M68K_REG_VBR:	REG_VBR = MASK_OUT_ABOVE_32(value); return;
//		case M68K_REG_SFC:	REG_SFC = value & 7; return;
//		case M68K_REG_DFC:	REG_DFC = value & 7; return;
//		case M68K_REG_CACR:	REG_CACR = MASK_OUT_ABOVE_32(value); return;
//		case M68K_REG_CAAR:	REG_CAAR = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_PPC:	REG_PPC = MASK_OUT_ABOVE_32(value); return;
		case M68K_REG_IR:	REG_IR = MASK_OUT_ABOVE_16(value); return;
//		case M68K_REG_PREF_ADDR:	MPU_PREF_ADDR = MASK_OUT_ABOVE_32(value); return;

		default:			return;
	}
}

/* Set the callbacks */
//void m68k_set_int_ack_callback(int  (*callback)(int int_level))
//{
//	CALLBACK_INT_ACK = callback ? callback : default_int_ack_callback;
//}
//
//void m68k_set_bkpt_ack_callback(void  (*callback)(unsigned int data))
//{
//	CALLBACK_BKPT_ACK = callback ? callback : default_bkpt_ack_callback;
//}
//
//void m68k_set_reset_instr_callback(void  (*callback)(void))
//{
//	CALLBACK_RESET_INSTR = callback ? callback : default_reset_instr_callback;
//}
//
//void m68k_set_cmpild_instr_callback(void  (*callback)(unsigned int, int))
//{
//	CALLBACK_CMPILD_INSTR = callback ? callback : default_cmpild_instr_callback;
//}
//
//void m68k_set_rte_instr_callback(void  (*callback)(void))
//{
//	CALLBACK_RTE_INSTR = callback ? callback : default_rte_instr_callback;
//}
//
//void m68k_set_pc_changed_callback(void  (*callback)(unsigned int new_pc))
//{
//	CALLBACK_PC_CHANGED = callback ? callback : default_pc_changed_callback;
//}
//
//void m68k_set_fc_callback(void  (*callback)(unsigned int new_fc))
//{
//	CALLBACK_SET_FC = callback ? callback : default_set_fc_callback;
//}
//
//void m68k_set_instr_hook_callback(void  (*callback)(void))
//{
//	CALLBACK_INSTR_HOOK = callback ? callback : default_instr_hook_callback;
//}

//#include <stdio.h>
/* Set the MPU type. */
//void m68k_set_mpu_type(unsigned int mpu_type)
//{
//	switch(mpu_type)
//	{
//		case M68K_MPU_TYPE_68000:
//			MPU_TYPE         = MPU_TYPE_000;
//			MPU_ADDRESS_MASK = 0x00ffffff;
//			MPU_SR_MASK      = 0xa71f; /* T1 -- S  -- -- I2 I1 I0 -- -- -- X  N  Z  V  C  */
//			m68ki_cycles  = m68ki_cycles[0];
//			CYC_EXCEPTION    = m68ki_exception_cycle_table[0];
//
//			return;
//	}
//}

/* Execute some instructions until we use up num_cycles clock cycles */
/* ASG: removed per-instruction interrupt checks */
int m68k_execute(int num_cycles)
{
	/* Make sure we're not stopped */
	if(!MPU_STOPPED)
	{
		/* Set our pool of clock cycles available */
		SET_CYCLES(num_cycles);
		m68ki_initial_cycles = num_cycles;

		/* ASG: update cycles */
		USE_CYCLES(MPU_INT_CYCLES);
		MPU_INT_CYCLES = 0;

		/* Return point if we had an address error */
		//m68ki_set_address_error_trap(); /* auto-disable (see m68kmpu.h) */

		/* Main loop.  Keep going until we run out of clock cycles */
		do
		{
			/* Set tracing accodring to T1. (T0 is done inside instruction) */
			//m68ki_trace_t1(); /* auto-disable (see m68kmpu.h) */

			/* Set the address space for reads */
			//m68ki_use_data_space(); /* auto-disable (see m68kmpu.h) */

			/* Call external hook to peek at MPU */
			//m68ki_instr_hook(); /* auto-disable (see m68kmpu.h) */

			/* Record previous program counter */
			REG_PPC = REG_PC;

			/* Read an instruction and call its handler */
			REG_IR = m68ki_read_imm_16();
			m68ki_instruction_jump_table[REG_IR]();
//#if 1
/*68kmake_v3_m44+l1*/
			USE_CYCLES(m68ki_cycles[REG_IR]);
//#endif
			/* Trace m68k_exception, if necessary */
			//m68ki_exception_if_trace(); /* auto-disable (see m68kmpu.h) */
		} while(GET_CYCLES() > 0);

		/* set previous PC to current PC for the next entry into the loop */
		REG_PPC = REG_PC;

		/* ASG: update cycles */
		USE_CYCLES(MPU_INT_CYCLES);
		MPU_INT_CYCLES = 0;

		/* return how many clocks we used */
		return m68ki_initial_cycles - GET_CYCLES();
	}

	/* We get here if the MPU is stopped or halted */
	SET_CYCLES(0);
	MPU_INT_CYCLES = 0;

	return num_cycles;
}

#ifdef EXT_MODE
int m68k_cycles_run(void)
{
	return m68ki_initial_cycles - GET_CYCLES();
}
#endif

int m68k_cycles_remaining(void)
{
	return GET_CYCLES();
}

/* Change the timeslice */
void m68k_modify_timeslice(int cycles)
{
	m68ki_initial_cycles += cycles;
	ADD_CYCLES(cycles);
}


void m68k_end_timeslice(void)
{
	m68ki_initial_cycles = GET_CYCLES();
	SET_CYCLES(0);
}


/* ASG: rewrote so that the int_level is a mask of the IPL0/IPL1/IPL2 bits */
/* KS: Modified so that IPL* bits match with mask positions in the SR
 *     and cleaned out remenants of the interrupt controller.
 */
void m68k_set_irq(unsigned int int_level)
{
	uint old_level = MPU_INT_LEVEL;
	MPU_INT_LEVEL = int_level << 8;

	/* A transition from < 7 to 7 always interrupts (NMI) */
	/* Note: Level 7 can also level trigger like a normal IRQ */
	if(old_level != 0x0700 && MPU_INT_LEVEL == 0x0700)
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
		emulation_initialized = 1;
	}

	/* Clear all stop levels and eat up all remaining cycles */
	MPU_STOPPED = 0;
	SET_CYCLES(0);

//	MPU_RUN_MODE = RUN_MODE_BERR_AERR_RESET;

	/* Turn off tracing */
	FLAG_T1 = FLAG_T0 = 0;
	//m68ki_clear_trace();
	/* Interrupt mask to level 7 */
	FLAG_INT_MASK = 0x0700;
	/* Reset VBR */
	//REG_VBR = 0;
	/* Go to supervisor mode */
	m68ki_set_sm_flag(SFLAG_SET | MFLAG_CLEAR);

	/* Invalidate the prefetch queue */
#if M68K_EMULATE_PREFETCH
	/* Set to arbitrary number since our first fetch is from 0 */
	MPU_PREF_ADDR = 0x1000;
#endif /* M68K_EMULATE_PREFETCH */

	/* Read the initial stack pointer and program counter */
	m68ki_jump(0);
	REG_SP = m68ki_read_imm_32();
	REG_PC = m68ki_read_imm_32();
	m68ki_jump(REG_PC);

//	MPU_RUN_MODE = RUN_MODE_NORMAL;
}

/* Pulse the HALT line on the MPU */
void m68k_pulse_halt(void)
{
	MPU_STOPPED |= STOP_LEVEL_HALT;
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
	UINT8 stopped;
	UINT8 halted;
} m68k_substate;

static void m68k_prepare_substate(void)
{
	m68k_substate.sr = m68ki_get_sr();
	m68k_substate.stopped = (MPU_STOPPED & STOP_LEVEL_STOP) != 0;
	m68k_substate.halted  = (MPU_STOPPED & STOP_LEVEL_HALT) != 0;
}

static void m68k_post_load(void)
{
	m68ki_set_sr_noint_nosp(m68k_substate.sr);
	MPU_STOPPED = m68k_substate.stopped ? STOP_LEVEL_STOP : 0
		        | m68k_substate.halted  ? STOP_LEVEL_HALT : 0;
	m68ki_jump(REG_PC);
}

void m68k_state_register(const char *type, int index)
{
	state_save_register_item_array(type, index, REG_D);
	state_save_register_item_array(type, index, REG_A);
	state_save_register_item(type, index, REG_PPC);
	state_save_register_item(type, index, REG_PC);
	state_save_register_item(type, index, REG_USP);
	state_save_register_item(type, index, REG_ISP);
	state_save_register_item(type, index, REG_MSP);
	state_save_register_item(type, index, REG_VBR);
	state_save_register_item(type, index, REG_SFC);
	state_save_register_item(type, index, REG_DFC);
	state_save_register_item(type, index, REG_CACR);
	state_save_register_item(type, index, REG_CAAR);
	state_save_register_item(type, index, m68k_substate.sr);
	state_save_register_item(type, index, MPU_INT_LEVEL);
	state_save_register_item(type, index, MPU_INT_CYCLES);
	state_save_register_item(type, index, m68k_substate.stopped);
	state_save_register_item(type, index, m68k_substate.halted);
	state_save_register_item(type, index, MPU_PREF_ADDR);
	state_save_register_item(type, index, MPU_PREF_DATA);
	state_save_register_func_presave(m68k_prepare_substate);
	state_save_register_func_postload(m68k_post_load);
}

#endif /* M68K_COMPILE_FOR_MAME */



#include"m68kopac.c"
/* Exception for illegal instructions */
static void m68ki_exception_illegal(void)
{
	uint sr;

	sr = m68ki_init_exception();

//	m68ki_stack_frame_0000(REG_PPC, sr, EXCEPTION_ILLEGAL_INSTRUCTION);
	m68ki_stack_frame_3word(REG_PPC, sr);
	m68ki_jump_vector(EXCEPTION_ILLEGAL_INSTRUCTION);

	/* Use up some clock cycles and undo the instruction's cycles */
	USE_CYCLES(34/*CYC_EXCEPTION[EXCEPTION_ILLEGAL_INSTRUCTION]*/ - m68ki_cycles[REG_IR]);
}
#include"m68kopdm.c"
#include"m68kops.c"
#include"m68kopnz.c"


/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */
