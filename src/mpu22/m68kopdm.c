#include "m68kmpu.h"
#include "m68kops.h"

/* ======================================================================== */
/* ======================= INSTRUCTION HANDLERS D-M ======================= */
/* ======================================================================== */

static void m68k_op_dbt(void)
{
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbf(void)
{
	uint *d_reg = &DY;
	uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

	*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
	if (res != 0xffff)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_branch_word(m68ki_read_16(REG_PC));
#if 000
//？？？ワカンナイ？？？
		USE_CLKS(10);
#else
//fake???
//		USE_CLKS(4);
#endif
		return;
	}
	REG_PC += 2;
//	USE_CLKS(14); /* これはミステイク（単純コピーミス） */
	USE_CLKS(12); /* 恐らくｍ６８０００の動作からこちらが正しい */
}


static void m68k_op_dbhi(void)
{
	if (CONDITION_NOT_HI)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbls(void)
{
	if (CONDITION_NOT_LS)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbcc(void)
{
	if (CONDITION_NOT_CC)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbcs(void)
{
	if (CONDITION_NOT_CS)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbne(void)
{
	if (CONDITION_NOT_NE)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbeq(void)
{
	if (CONDITION_NOT_EQ)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbvc(void)
{
	if (CONDITION_NOT_VC)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbvs(void)
{
	if (CONDITION_NOT_VS)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbpl(void)
{
	if (CONDITION_NOT_PL)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbmi(void)
{
	if (CONDITION_NOT_MI)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbge(void)
{
	if (CONDITION_NOT_GE)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dblt(void)
{
	if (CONDITION_NOT_LT)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dbgt(void)
{
	if (CONDITION_NOT_GT)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_dble(void)
{
	if (CONDITION_NOT_LE)
	{
		uint *d_reg = &DY;
		uint res = MASK_OUT_ABOVE_16(*d_reg - 1);

		*d_reg = MASK_OUT_BELOW_16(*d_reg) | res;
		if (res != 0xffff)
		{
			//m68ki_add_trace();	   /* auto-disable (see m68kcpu.h) */
			m68ki_branch_word(m68ki_read_16(REG_PC));
			USE_CLKS(10);
			return;
		}
		REG_PC += 2;
		USE_CLKS(14);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_divs_d_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(DY);

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_ai_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_AI));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+4);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+4);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_pi_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_PI_16));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+4);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+4);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_pd_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_PD_16));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+6);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+6);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_di_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_DI));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+8);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+8);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_ix_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_IX));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+10);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+10);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_aw_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_AW));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+8);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+8);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_al_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_AL));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+12);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+12);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_pcdi_16(void)
{
	uint *d_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	int src = MAKE_INT_16(m68ki_read_16(ea));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+8);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+8);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_pcix_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_16(EA_PCIX));

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+10);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+10);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divs_i_16(void)
{
	uint *d_dst = &DX;
	int src = MAKE_INT_16(m68ki_read_imm_16());

	FLAG_C = 0;

	if (src != 0)
	{
		int quotient = MAKE_INT_32(*d_dst) / src;
		int remainder = MAKE_INT_32(*d_dst) % src;

		if (quotient == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(158+4);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(158+4);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_d_16(void)
{
	uint *d_dst = &DX;
	uint src = MASK_OUT_ABOVE_16(DY);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_ai_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AI);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+4);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+4);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_pi_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PI_16);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+4);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+4);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_pd_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PD_16);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+6);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+6);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_di_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_DI);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+8);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+8);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_ix_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_IX);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+10);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+10);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_aw_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AW);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+8);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+8);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_al_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_AL);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+12);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+12);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_pcdi_16(void)
{
	uint *d_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_16(ea);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+8);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+8);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_pcix_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_16(EA_PCIX);

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+10);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+10);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}


static void m68k_op_divu_i_16(void)
{
	uint *d_dst = &DX;
	uint src = m68ki_read_imm_16();

	FLAG_C = 0;

	if (src != 0)
	{
		uint quotient = *d_dst / src;
		uint remainder = *d_dst % src;

		if (MAKE_INT_32(quotient) == MAKE_INT_16(quotient))
		{
			FLAG_Z = quotient;
			FLAG_N = GET_MSB_16(quotient);
			FLAG_V = 0;
			*d_dst = MASK_OUT_ABOVE_32(MASK_OUT_ABOVE_16(quotient) | (remainder << 16));
			USE_CLKS(140+4);
			return;
		}
		FLAG_V = 1;
		USE_CLKS(140+4);
		return;
	}
	m68ki_interrupt(EXCEPTION_ZERO_DIVIDE);
}




static void m68k_op_eor_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY ^= MASK_OUT_ABOVE_8(DX));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_eor_ai_8(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_eor_pi_8(void)
{
	uint ea = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_eor_pi7_8(void)
{
	uint ea = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_eor_pd_8(void)
{
	uint ea = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_eor_pd7_8(void)
{
	uint ea = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_eor_di_8(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_eor_ix_8(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_eor_aw_8(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_eor_al_8(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_8(DX ^ m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_eor_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY ^= MASK_OUT_ABOVE_16(DX));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_eor_ai_16(void)
{
	uint ea = EA_AI;
	uint res = MASK_OUT_ABOVE_16(DX ^ m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_eor_pi_16(void)
{
	uint ea = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(DX ^ m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_eor_pd_16(void)
{
	uint ea = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(DX ^ m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_eor_di_16(void)
{
	uint ea = EA_DI;
	uint res = MASK_OUT_ABOVE_16(DX ^ m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_eor_ix_16(void)
{
	uint ea = EA_IX;
	uint res = MASK_OUT_ABOVE_16(DX ^ m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_eor_aw_16(void)
{
	uint ea = EA_AW;
	uint res = MASK_OUT_ABOVE_16(DX ^ m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_eor_al_16(void)
{
	uint ea = EA_AL;
	uint res = MASK_OUT_ABOVE_16(DX ^ m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_eor_d_32(void)
{
	uint res = DY ^= DX;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_eor_ai_32(void)
{
	uint ea = EA_AI;
	uint res = DX ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_eor_pi_32(void)
{
	uint ea = EA_PI_32;
	uint res = DX ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_eor_pd_32(void)
{
	uint ea = EA_PD_32;
	uint res = DX ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_eor_di_32(void)
{
	uint ea = EA_DI;
	uint res = DX ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_eor_ix_32(void)
{
	uint ea = EA_IX;
	uint res = DX ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+14);
}


static void m68k_op_eor_aw_32(void)
{
	uint ea = EA_AW;
	uint res = DX ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_eor_al_32(void)
{
	uint ea = EA_AL;
	uint res = DX ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+16);
}


static void m68k_op_eori_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY ^= m68ki_read_imm_8());

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_eori_ai_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AI;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_eori_pi_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PI_8;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_eori_pi7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PI7_8;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_eori_pd_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PD_8;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_eori_pd7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_PD7_8;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_eori_di_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_DI;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_eori_ix_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_IX;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_eori_aw_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AW;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_eori_al_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea = EA_AL;
	uint res = tmp ^ m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_eori_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY ^= m68ki_read_imm_16());

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_eori_ai_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint res = tmp ^ m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_eori_pi_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_PI_16;
	uint res = tmp ^ m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_eori_pd_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_PD_16;
	uint res = tmp ^ m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_eori_di_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint res = tmp ^ m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_eori_ix_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint res = tmp ^ m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_eori_aw_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint res = tmp ^ m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_eori_al_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint res = tmp ^ m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_eori_d_32(void)
{
	uint res = DY ^= m68ki_read_imm_32();

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(16);
}


static void m68k_op_eori_ai_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AI;
	uint res = tmp ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+8);
}


static void m68k_op_eori_pi_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_PI_32;
	uint res = tmp ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+8);
}


static void m68k_op_eori_pd_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_PD_32;
	uint res = tmp ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+10);
}


static void m68k_op_eori_di_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_DI;
	uint res = tmp ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+12);
}


static void m68k_op_eori_ix_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_IX;
	uint res = tmp ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+14);
}


static void m68k_op_eori_aw_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AW;
	uint res = tmp ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+12);
}


static void m68k_op_eori_al_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea = EA_AL;
	uint res = tmp ^ m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+16);
}


static void m68k_op_eori_to_ccr(void)
{
	m68ki_set_ccr(m68ki_get_ccr() ^ m68ki_read_imm_16());
	USE_CLKS(20);
}


static void m68k_op_eori_to_sr(void)
{
	uint eor_val = m68ki_read_imm_16();

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(m68ki_get_sr() ^ eor_val);
		USE_CLKS(20);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_exg_dd(void)
{
	uint *reg_a = &DX;
	uint *reg_b = &DY;
	uint tmp = *reg_a;

	*reg_a = *reg_b;
	*reg_b = tmp;

	USE_CLKS(6);
}


static void m68k_op_exg_aa(void)
{
	uint *reg_a = &AX;
	uint *reg_b = &AY;
	uint tmp = *reg_a;

	*reg_a = *reg_b;
	*reg_b = tmp;

	USE_CLKS(6);
}


static void m68k_op_exg_da(void)
{
	uint *reg_a = &DX;
	uint *reg_b = &AY;
	uint tmp = *reg_a;

	*reg_a = *reg_b;
	*reg_b = tmp;

	USE_CLKS(6);
}


static void m68k_op_ext_16(void)
{
	uint *d_dst = &DY;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | MASK_OUT_ABOVE_8(*d_dst) | (GET_MSB_8(*d_dst) ? 0xff00 : 0);

	FLAG_N = GET_MSB_16(*d_dst);
	FLAG_Z = MASK_OUT_ABOVE_16(*d_dst);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_ext_32(void)
{
	uint *d_dst = &DY;

	*d_dst = MASK_OUT_ABOVE_16(*d_dst) | (GET_MSB_16(*d_dst) ? 0xffff0000 : 0);

	FLAG_N = GET_MSB_32(*d_dst);
	FLAG_Z = *d_dst;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}



static void m68k_op_illegal(void)
{
	m68ki_exception(EXCEPTION_ILLEGAL_INSTRUCTION);
//	M68K_DO_LOG((M68K_LOG, "%s at %08x: illegal instruction %04x (%s)\n",
//				 m68k_op_mpu_names[FLAG_MODE], ADDRESS_68K(REG_PPC), REG_IR,
//				 m68k_op_disassemble_quick(ADDRESS_68K(REG_PPC))));
#ifdef M68K_LOG
	/* I use this to get the proper offset when disassembling the offending instruction */
	m68k_op_pc_offset = 2;
#endif
}


static void m68k_op_jmp_ai(void)
{
	m68ki_branch_long(EA_AI);
	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(0+8);
}


static void m68k_op_jmp_di(void)
{
	m68ki_branch_long(EA_DI);
	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(0+10);
}


static void m68k_op_jmp_ix(void)
{
	m68ki_branch_long(EA_IX);
	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(0+14);
}


static void m68k_op_jmp_aw(void)
{
	m68ki_branch_long(EA_AW);
	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(0+10);
}


static void m68k_op_jmp_al(void)
{
	m68ki_branch_long(EA_AL);
	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(0+12);
}


static void m68k_op_jmp_pcdi(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	m68ki_branch_long(ea);
	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(0+10);
}


static void m68k_op_jmp_pcix(void)
{
	m68ki_branch_long(EA_PCIX);
	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	USE_CLKS(0+14);
}


static void m68k_op_jsr_ai(void)
{
	uint ea = EA_AI;

	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(REG_PC);
	m68ki_branch_long(ea);
	USE_CLKS(0+16);
}


static void m68k_op_jsr_di(void)
{
	uint ea = EA_DI;

	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(REG_PC);
	m68ki_branch_long(ea);
	USE_CLKS(0+18);
}


static void m68k_op_jsr_ix(void)
{
	uint ea = EA_IX;

	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(REG_PC);
	m68ki_branch_long(ea);
	USE_CLKS(0+22);
}


static void m68k_op_jsr_aw(void)
{
	uint ea = EA_AW;

	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(REG_PC);
	m68ki_branch_long(ea);
	USE_CLKS(0+18);
}


static void m68k_op_jsr_al(void)
{
	uint ea = EA_AL;

	m68ki_push_32(REG_PC);
	m68ki_branch_long(ea);
	USE_CLKS(0+20);
}


static void m68k_op_jsr_32_pcdi(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));

	m68ki_push_32(REG_PC);
	m68ki_branch_long(ea);
//	USE_CLKS(0+18);
	USE_CLKS(0+20);
}


static void m68k_op_jsr_pcix(void)
{
	uint ea = EA_PCIX;

	//m68ki_add_trace();			   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(REG_PC);
	m68ki_branch_long(ea);
	USE_CLKS(0+22);
}


static void m68k_op_lea_ai(void)
{
	AX = EA_AI;
	USE_CLKS(0+4);
}


static void m68k_op_lea_di(void)
{
	AX = EA_DI;
	USE_CLKS(0+8);
}


static void m68k_op_lea_ix(void)
{
	AX = EA_IX;
	USE_CLKS(0+12);
}


static void m68k_op_lea_aw(void)
{
	AX = EA_AW;
	USE_CLKS(0+8);
}


static void m68k_op_lea_al(void)
{
	AX = EA_AL;
	USE_CLKS(0+12);
}


static void m68k_op_lea_pcdi(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	AX = ea;
	USE_CLKS(0+8);
}


static void m68k_op_lea_pcix(void)
{
	AX = EA_PCIX;
	USE_CLKS(0+12);
}


static void m68k_op_link_16_a7(void)
{
	REG_A[7] -= 4;
	m68ki_write_32(REG_A[7], REG_A[7]);
	REG_A[7] = MASK_OUT_ABOVE_32(REG_A[7] + MAKE_INT_16(m68ki_read_imm_16()));
	USE_CLKS(16);
}


static void m68k_op_link_16(void)
{
	uint *a_dst = &AY;

	m68ki_push_32(*a_dst);
	*a_dst = REG_A[7];
	REG_A[7] = MASK_OUT_ABOVE_32(REG_A[7] + MAKE_INT_16(m68ki_read_imm_16()));
	USE_CLKS(16);
}



static void m68k_op_lsr_s_8(void)
{
	uint *d_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = src >> shift;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_X = FLAG_C = shift > 8 ? 0 : (src >> (shift - 1)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift+shift) + 6);
}


static void m68k_op_lsr_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = src >> shift;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_X = FLAG_C = (src >> (shift - 1)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift+shift) + 6);
}


static void m68k_op_lsr_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = *d_dst;
	uint res = src >> shift;

	*d_dst = res;

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_X = FLAG_C = (src >> (shift - 1)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift+shift) + 8);
}


static void m68k_op_lsr_r_8(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = src >> shift;

	USE_CLKS((shift+shift) + 6);
	if (shift != 0)
	{
		if (shift <= 8)
		{
			*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;
			FLAG_X = FLAG_C = (src >> (shift - 1)) & 1;
			FLAG_N = 0;
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		*d_dst &= 0xffffff00;
		FLAG_X = FLAG_C = 0;
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_lsr_r_16(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = src >> shift;

	USE_CLKS((shift+shift) + 6);
	if (shift != 0)
	{
		if (shift <= 16)
		{
			*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;
			FLAG_X = FLAG_C = (src >> (shift - 1)) & 1;
			FLAG_N = 0;
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		*d_dst &= 0xffff0000;
		FLAG_X = FLAG_C = 0;
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_lsr_r_32(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = *d_dst;
	uint res = src >> shift;

	USE_CLKS((shift+shift) + 8);
	if (shift != 0)
	{
		if (shift < 32)
		{
			*d_dst = res;
			FLAG_X = FLAG_C = (src >> (shift - 1)) & 1;
			FLAG_N = 0;
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		*d_dst = 0;
		FLAG_X = FLAG_C = (shift == 32 ? GET_MSB_32(src) : 0);
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_lsr_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_lsr_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_lsr_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_lsr_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_lsr_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_lsr_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_lsr_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	m68ki_write_16(ea, res);

	FLAG_N = 0;
	FLAG_Z = res;
	FLAG_C = FLAG_X = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_lsl_s_8(void)
{
	uint *d_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = shift > 8 ? 0 : (src >> (8 - shift)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift+shift) + 6);
}


static void m68k_op_lsl_s_16(void)
{
	uint *d_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = (src >> (16 - shift)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift+shift) + 6);
}


static void m68k_op_lsl_s_32(void)
{
	uint *d_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = *d_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = (src >> (32 - shift)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift+shift) + 8);
}


static void m68k_op_lsl_r_8(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*d_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);

	USE_CLKS((shift+shift) + 6);
	if (shift != 0)
	{
		if (shift <= 8)
		{
			*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;
			FLAG_X = FLAG_C = (src >> (8 - shift)) & 1;
			FLAG_N = GET_MSB_8(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		*d_dst &= 0xffffff00;
		FLAG_X = FLAG_C = 0;
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_lsl_r_16(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*d_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);

	USE_CLKS((shift+shift) + 6);
	if (shift != 0)
	{
		if (shift <= 16)
		{
			*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;
			FLAG_X = FLAG_C = (src >> (16 - shift)) & 1;
			FLAG_N = GET_MSB_16(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		*d_dst &= 0xffff0000;
		FLAG_X = FLAG_C = 0;
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_lsl_r_32(void)
{
	uint *d_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = *d_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);

	USE_CLKS((shift+shift) + 8);
	if (shift != 0)
	{
		if (shift < 32)
		{
			*d_dst = res;
			FLAG_X = FLAG_C = (src >> (32 - shift)) & 1;
			FLAG_N = GET_MSB_32(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		*d_dst = 0;
		FLAG_X = FLAG_C = (shift == 32 ? src & 1 : 0);
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_lsl_ea_ai(void)
{
	uint ea = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_lsl_ea_pi(void)
{
	uint ea = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_lsl_ea_pd(void)
{
	uint ea = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_lsl_ea_di(void)
{
	uint ea = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_lsl_ea_ix(void)
{
	uint ea = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_lsl_ea_aw(void)
{
	uint ea = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_lsl_ea_al(void)
{
	uint ea = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_dd_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_move_dd_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_move_dd_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_move_dd_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_move_dd_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+6);
}


static void m68k_op_move_dd_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+6);
}


static void m68k_op_move_dd_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_move_dd_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}


static void m68k_op_move_dd_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_move_dd_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_8(*d_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_move_ai_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_ai_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_ai_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_ai_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_ai_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_ai_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_ai_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_ai_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_ai_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_ai_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_ai_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_ai_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_ai_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi7_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pi_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pi7_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi7_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi7_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi7_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pi7_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pi7_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi7_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pi7_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi7_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_pi7_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi7_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pi7_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = (REG_A[7] += 2) - 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pi_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pi_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pi_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_pi_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pi_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = AX++;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd7_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pd_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pd7_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd7_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd7_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd7_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pd7_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pd7_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd7_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pd7_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd7_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_pd7_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd7_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pd7_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = REG_A[7] -= 2;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pd_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pd_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pd_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_pd_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pd_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = --AX;

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_di_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_di_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_di_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_di_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_di_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+6);
}


static void m68k_op_move_di_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+6);
}


static void m68k_op_move_di_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_di_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_di_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_di_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_di_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_di_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_di_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_ix_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
//	uint ea = EA_AX_IX_8();
	uint ea = m68ki_get_ea_ix_dst();

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14);
}


static void m68k_op_move_ix_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+4);
}


static void m68k_op_move_ix_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+4);
}


static void m68k_op_move_ix_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+4);
}


static void m68k_op_move_ix_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+6);
}


static void m68k_op_move_ix_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+6);
}


static void m68k_op_move_ix_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+8);
}


static void m68k_op_move_ix_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+10);
}


static void m68k_op_move_ix_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+8);
}


static void m68k_op_move_ix_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+12);
}


static void m68k_op_move_ix_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+8);
}


static void m68k_op_move_ix_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+10);
}


static void m68k_op_move_ix_i_8(void)
{
	uint res = m68ki_read_imm_8();

	m68ki_write_8(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+4);
}


static void m68k_op_move_aw_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_aw_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_aw_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_aw_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_aw_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+6);
}


static void m68k_op_move_aw_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+6);
}


static void m68k_op_move_aw_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_aw_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_aw_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_aw_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_aw_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_aw_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_aw_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_al_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16);
}


static void m68k_op_move_al_ai_8(void)
{
	uint res = m68ki_read_8(EA_AI);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+4);
}


static void m68k_op_move_al_pi_8(void)
{
	uint res = m68ki_read_8(EA_PI_8);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+4);
}


static void m68k_op_move_al_pi7_8(void)
{
	uint res = m68ki_read_8(EA_PI7_8);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+4);
}


static void m68k_op_move_al_pd_8(void)
{
	uint res = m68ki_read_8(EA_PD_8);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+6);
}


static void m68k_op_move_al_pd7_8(void)
{
	uint res = m68ki_read_8(EA_PD7_8);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+6);
}


static void m68k_op_move_al_di_8(void)
{
	uint res = m68ki_read_8(EA_DI);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_al_ix_8(void)
{
	uint res = m68ki_read_8(EA_IX);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+10);
}


static void m68k_op_move_al_aw_8(void)
{
	uint res = m68ki_read_8(EA_AW);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_al_al_8(void)
{
	uint res = m68ki_read_8(EA_AL);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_al_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_8(ea);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_al_pcix_8(void)
{
	uint res = m68ki_read_8(EA_PCIX);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+10);
}


static void m68k_op_move_al_i_8(void)
{
	uint res = m68ki_read_imm_8();
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_8(ea_dst, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+4);
}


static void m68k_op_move_dd_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_move_dd_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_move_dd_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_move_dd_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_move_dd_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+6);
}


static void m68k_op_move_dd_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_move_dd_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}


static void m68k_op_move_dd_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_move_dd_i_16(void)
{
	uint res = m68ki_read_imm_16();
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_move_ai_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_ai_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_ai_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_ai_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_ai_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_ai_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_ai_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_ai_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_ai_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_ai_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_ai_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_ai_i_16(void)
{
	uint res = m68ki_read_imm_16();
	uint ea_dst = AX;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pi_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pi_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pi_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pi_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pi_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_pi_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pi_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pi_i_16(void)
{
	uint res = m68ki_read_imm_16();
	uint ea_dst = (AX += 2) - 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pd_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8);
}


static void m68k_op_move_pd_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_pd_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+6);
}


static void m68k_op_move_pd_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pd_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+12);
}


static void m68k_op_move_pd_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+8);
}


static void m68k_op_move_pd_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+10);
}


static void m68k_op_move_pd_i_16(void)
{
	uint res = m68ki_read_imm_16();
	uint ea_dst = AX -= 2;

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(8+4);
}


static void m68k_op_move_di_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_di_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_di_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_di_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_di_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+6);
}


static void m68k_op_move_di_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_di_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_di_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_di_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_di_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_di_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_di_i_16(void)
{
	uint res = m68ki_read_imm_16();
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_ix_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14);
}


static void m68k_op_move_ix_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14);
}


static void m68k_op_move_ix_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+4);
}


static void m68k_op_move_ix_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+4);
}


static void m68k_op_move_ix_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+6);
}


static void m68k_op_move_ix_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+8);
}


static void m68k_op_move_ix_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+10);
}


static void m68k_op_move_ix_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+8);
}


static void m68k_op_move_ix_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+12);
}


static void m68k_op_move_ix_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+8);
}


static void m68k_op_move_ix_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+10);
}


static void m68k_op_move_ix_i_16(void)
{
	uint res = m68ki_read_imm_16();

	m68ki_write_16(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(14+4);
}


static void m68k_op_move_aw_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_aw_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_aw_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_aw_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_aw_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+6);
}


static void m68k_op_move_aw_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_aw_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_aw_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_aw_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_aw_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_aw_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_aw_i_16(void)
{
	uint res = m68ki_read_imm_16();
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+4);
}


static void m68k_op_move_al_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16);
}


static void m68k_op_move_al_a_16(void)
{
	uint res = MASK_OUT_ABOVE_16(AY);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16);
}


static void m68k_op_move_al_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+4);
}


static void m68k_op_move_al_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+4);
}


static void m68k_op_move_al_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+6);
}


static void m68k_op_move_al_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_al_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+10);
}


static void m68k_op_move_al_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_al_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_al_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_al_pcix_16(void)
{
	uint res = m68ki_read_16(EA_PCIX);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+10);
}


static void m68k_op_move_al_i_16(void)
{
	uint res = m68ki_read_imm_16();
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_16(ea_dst, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+4);
}


static void m68k_op_move_dd_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_move_dd_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_move_dd_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_dd_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_move_dd_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}


static void m68k_op_move_dd_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+14);
}


static void m68k_op_move_dd_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}


static void m68k_op_move_dd_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+16);
}


static void m68k_op_move_dd_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}


static void m68k_op_move_dd_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+14);
}


static void m68k_op_move_dd_i_32(void)
{
	uint res = m68ki_read_imm_32();
	uint *d_dst = &DX;

	*d_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_move_ai_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_ai_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_ai_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_ai_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_ai_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_ai_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_ai_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+14);
}


static void m68k_op_move_ai_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_ai_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+16);
}


static void m68k_op_move_ai_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_ai_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+14);
}


static void m68k_op_move_ai_i_32(void)
{
	uint res = m68ki_read_imm_32();
	uint ea_dst = AX;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_pi_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_pi_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_pi_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_pi_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_pi_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_pi_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_pi_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+14);
}


static void m68k_op_move_pi_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_pi_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+16);
}


static void m68k_op_move_pi_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_pi_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+14);
}


static void m68k_op_move_pi_i_32(void)
{
	uint res = m68ki_read_imm_32();
	uint ea_dst = (AX += 4) - 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_pd_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_pd_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12);
}


static void m68k_op_move_pd_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_pd_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_pd_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+10);
}


static void m68k_op_move_pd_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_pd_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+14);
}


static void m68k_op_move_pd_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_pd_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+16);
}


static void m68k_op_move_pd_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+12);
}


static void m68k_op_move_pd_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+14);
}


static void m68k_op_move_pd_i_32(void)
{
	uint res = m68ki_read_imm_32();
	uint ea_dst = AX -= 4;

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(12+8);
}


static void m68k_op_move_di_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16);
}


static void m68k_op_move_di_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16);
}


static void m68k_op_move_di_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_di_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_di_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+10);
}


static void m68k_op_move_di_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_di_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+14);
}


static void m68k_op_move_di_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_di_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+16);
}


static void m68k_op_move_di_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_di_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+14);
}


static void m68k_op_move_di_i_32(void)
{
	uint res = m68ki_read_imm_32();
	uint ea_dst = AX + MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_ix_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18);
}


static void m68k_op_move_ix_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18);
}


static void m68k_op_move_ix_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+8);
}


static void m68k_op_move_ix_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+8);
}


static void m68k_op_move_ix_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+10);
}


static void m68k_op_move_ix_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+12);
}


static void m68k_op_move_ix_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+14);
}


static void m68k_op_move_ix_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+12);
}


static void m68k_op_move_ix_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+16);
}


static void m68k_op_move_ix_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+12);
}


static void m68k_op_move_ix_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+14);
}


static void m68k_op_move_ix_i_32(void)
{
	uint res = m68ki_read_imm_32();

	m68ki_write_32(m68ki_get_ea_ix_dst(), res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(18+8);
}


static void m68k_op_move_aw_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16);
}


static void m68k_op_move_aw_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16);
}


static void m68k_op_move_aw_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_aw_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_aw_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+10);
}


static void m68k_op_move_aw_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_aw_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+14);
}


static void m68k_op_move_aw_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_aw_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+16);
}


static void m68k_op_move_aw_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+12);
}


static void m68k_op_move_aw_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+14);
}


static void m68k_op_move_aw_i_32(void)
{
	uint res = m68ki_read_imm_32();
	uint ea_dst = MAKE_INT_16(m68ki_read_imm_16());

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(16+8);
}


static void m68k_op_move_al_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20);
}


static void m68k_op_move_al_a_32(void)
{
	uint res = MASK_OUT_ABOVE_32(AY);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20);
}


static void m68k_op_move_al_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+8);
}


static void m68k_op_move_al_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+8);
}


static void m68k_op_move_al_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+10);
}


static void m68k_op_move_al_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+12);
}


static void m68k_op_move_al_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+14);
}


static void m68k_op_move_al_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+12);
}


static void m68k_op_move_al_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+16);
}


static void m68k_op_move_al_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_32(ea);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+12);
}


static void m68k_op_move_al_pcix_32(void)
{
	uint res = m68ki_read_32(EA_PCIX);
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+14);
}


static void m68k_op_move_al_i_32(void)
{
	uint res = m68ki_read_imm_32();
	uint ea_dst = m68ki_read_imm_32();

	m68ki_write_32(ea_dst, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(20+8);
}


static void m68k_op_movea_d_16(void)
{
	AX = MAKE_INT_16(DY);
	USE_CLKS(4);
}


static void m68k_op_movea_a_16(void)
{
	AX = MAKE_INT_16(AY);
	USE_CLKS(4);
}


static void m68k_op_movea_ai_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_AI));
	USE_CLKS(4+4);
}


static void m68k_op_movea_pi_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_PI_16));
	USE_CLKS(4+4);
}


static void m68k_op_movea_pd_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_PD_16));
	USE_CLKS(4+6);
}


static void m68k_op_movea_di_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_DI));
	USE_CLKS(4+8);
}


static void m68k_op_movea_ix_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_IX));
	USE_CLKS(4+10);
}


static void m68k_op_movea_aw_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_AW));
	USE_CLKS(4+8);
}


static void m68k_op_movea_al_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_AL));
	USE_CLKS(4+12);
}


static void m68k_op_movea_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	AX = MAKE_INT_16(m68ki_read_16(ea));
	USE_CLKS(4+8);
}


static void m68k_op_movea_pcix_16(void)
{
	AX = MAKE_INT_16(m68ki_read_16(EA_PCIX));
	USE_CLKS(4+10);
}


static void m68k_op_movea_i_16(void)
{
	AX = MAKE_INT_16(m68ki_read_imm_16());
	USE_CLKS(4+4);
}


static void m68k_op_movea_d_32(void)
{
	AX = MASK_OUT_ABOVE_32(DY);
	USE_CLKS(4);
}


static void m68k_op_movea_a_32(void)
{
	AX = MASK_OUT_ABOVE_32(AY);
	USE_CLKS(4);
}


static void m68k_op_movea_ai_32(void)
{
	AX = m68ki_read_32(EA_AI);
	USE_CLKS(4+8);
}


static void m68k_op_movea_pi_32(void)
{
	AX = m68ki_read_32(EA_PI_32);
	USE_CLKS(4+8);
}


static void m68k_op_movea_pd_32(void)
{
	AX = m68ki_read_32(EA_PD_32);
	USE_CLKS(4+10);
}


static void m68k_op_movea_di_32(void)
{
	AX = m68ki_read_32(EA_DI);
	USE_CLKS(4+12);
}


static void m68k_op_movea_ix_32(void)
{
	AX = m68ki_read_32(EA_IX);
	USE_CLKS(4+14);
}


static void m68k_op_movea_aw_32(void)
{
	AX = m68ki_read_32(EA_AW);
	USE_CLKS(4+12);
}


static void m68k_op_movea_al_32(void)
{
	AX = m68ki_read_32(EA_AL);
	USE_CLKS(4+16);
}


static void m68k_op_movea_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	AX = m68ki_read_32(ea);
	USE_CLKS(4+12);
}


static void m68k_op_movea_pcix_32(void)
{
	AX = m68ki_read_32(EA_PCIX);
	USE_CLKS(4+14);
}


static void m68k_op_movea_i_32(void)
{
	AX = m68ki_read_imm_32();
	USE_CLKS(4+8);
}




static void m68k_op_move_to_ccr_d(void)
{
	m68ki_set_ccr(DY);
	USE_CLKS(12);
}


static void m68k_op_move_to_ccr_ai(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_AI));
	USE_CLKS(12+4);
}


static void m68k_op_move_to_ccr_pi(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_PI_16));
	USE_CLKS(12+4);
}


static void m68k_op_move_to_ccr_pd(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_PD_16));
	USE_CLKS(12+6);
}


static void m68k_op_move_to_ccr_di(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_DI));
	USE_CLKS(12+8);
}


static void m68k_op_move_to_ccr_ix(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_IX));
	USE_CLKS(12+10);
}


static void m68k_op_move_to_ccr_aw(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_AW));
	USE_CLKS(12+8);
}


static void m68k_op_move_to_ccr_al(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_AL));
	USE_CLKS(12+12);
}


static void m68k_op_move_to_ccr_pcdi(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	m68ki_set_ccr(m68ki_read_16(ea));
	USE_CLKS(12+8);
}


static void m68k_op_move_to_ccr_pcix(void)
{
	m68ki_set_ccr(m68ki_read_16(EA_PCIX));
	USE_CLKS(12+10);
}


static void m68k_op_move_to_ccr_i(void)
{
	m68ki_set_ccr(m68ki_read_imm_16());
	USE_CLKS(12+4);
}


static void m68k_op_move_fr_sr_d(void)
{
//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		DY = MASK_OUT_BELOW_16(DY) | m68ki_get_sr();
		USE_CLKS(6);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_sr_ai(void)
{
	uint ea = EA_AI;

//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		m68ki_write_16(ea, m68ki_get_sr());
		USE_CLKS(8+4);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_sr_pi(void)
{
	uint ea = EA_PI_16;

//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		m68ki_write_16(ea, m68ki_get_sr());
		USE_CLKS(8+4);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_sr_pd(void)
{
	uint ea = EA_PD_16;

//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		m68ki_write_16(ea, m68ki_get_sr());
		USE_CLKS(8+6);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_sr_di(void)
{
	uint ea = EA_DI;

//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		m68ki_write_16(ea, m68ki_get_sr());
		USE_CLKS(8+8);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_sr_ix(void)
{
	uint ea = EA_IX;

//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		m68ki_write_16(ea, m68ki_get_sr());
		USE_CLKS(8+10);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_sr_aw(void)
{
	uint ea = EA_AW;

//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		m68ki_write_16(ea, m68ki_get_sr());
		USE_CLKS(8+8);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_sr_al(void)
{
	uint ea = EA_AL;

//	if ((FLAG_MODE & FLAG_MODE_000) || FLAG_S)	/* NS990408 */
//	{
		m68ki_write_16(ea, m68ki_get_sr());
		USE_CLKS(8+12);
		return;
//	}
//	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_d(void)
{
	if (FLAG_S)
	{
		m68ki_set_sr(DY);
		USE_CLKS(12);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_ai(void)
{
	uint new_sr = m68ki_read_16(EA_AI);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+4);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_pi(void)
{
	uint new_sr = m68ki_read_16(EA_PI_16);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+4);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_pd(void)
{
	uint new_sr = m68ki_read_16(EA_PD_16);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+6);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_di(void)
{
	uint new_sr = m68ki_read_16(EA_DI);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+8);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_ix(void)
{
	uint new_sr = m68ki_read_16(EA_IX);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+10);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_aw(void)
{
	uint new_sr = m68ki_read_16(EA_AW);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+8);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_al(void)
{
	uint new_sr = m68ki_read_16(EA_AL);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+12);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_pcdi(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint new_sr = m68ki_read_16(ea);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+8);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_pcix(void)
{
	uint new_sr = m68ki_read_16(EA_PCIX);

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+10);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_sr_i(void)
{
	uint new_sr = m68ki_read_imm_16();

	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		m68ki_set_sr(new_sr);
		USE_CLKS(12+4);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_fr_usp(void)
{
	if (FLAG_S)
	{
		AY = REG_USP;
		USE_CLKS(4);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_move_to_usp(void)
{
	if (FLAG_S)
	{
		//m68ki_add_trace();		   /* auto-disable (see m68kcpu.h) */
		REG_USP = AY;
		USE_CLKS(4);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


#if 00
/* AfterBurner II 起動しない。puls-man death? */
//static unsigned short rrr_tbl[16] __attribute__((aligned(64))) ={
//0x0001,0x0002,0x0004,0x0008,
//0x0010,0x0020,0x0040,0x0080,
//0x0100,0x0200,0x0400,0x0800,
//0x1000,0x2000,0x4000,0x8000 };


static void m68k_op_movem_pd_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++){
//		if(register_list & rrr_ tbl[i])
		if(register_list & (1 << i))
		{
			ea -= 2;
		//	m68ki_write_16(ea, *(m68k_op_movem_pd_table[i]));
		//	m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[15-i]));
		//	m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_D[15-i])); /* REG_DA[]==REG_D[] */
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_D[15-i])); /* REG_DA[]==REG_D[] */
			count++;
		}
	}
	AY = ea;
	USE_CLKS((count << CYC_MOVEM_W) + 8);
}


static void m68k_op_movem_pd_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++){
		if(register_list & (1 << i))
		{
			ea -= 4;
		//	m68ki_write_32(ea, *(m68k_op_movem_pd_table[i]));
			m68ki_write_32(ea, (REG_D[15-i])); /* REG_DA[]==REG_D[] */
			count++;
		}
	}
	AY = ea;
	USE_CLKS((count << CYC_MOVEM_L) + 8);
}


static void m68k_op_movem_pi_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++){
		if(register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
		//	REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	}
	AY = ea;
	USE_CLKS((count << 2) + 12);
}


static void m68k_op_movem_pi_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++){
		if(register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
		//	REG_DA[i] = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	}
	AY = ea;
	USE_CLKS((count << CYC_MOVEM_L) + 12);
}

static void movem16w_re(uint ea_z){
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = ea_z;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_16(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_16(ea, REG_D[i]);
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2));
}

//static void movem16w_re(uint ea){
//	uint register_list = m68ki_read_imm_16();
//	uint i = 0;
//	for (; i < 16; i++){
//		if(register_list & (1 << i))
//		{
//			m68ki_write_16(ea, REG_D[i]);	ea += 2;
//			USE_CLKS(1<<2);
//		}
//	}
//}
static void m68k_op_movem_re_ai_16(void){	movem16w_re(EA_AI); USE_CLKS(4+ 4);}
static void m68k_op_movem_re_di_16(void){	movem16w_re(EA_DI); USE_CLKS(4+ 8);}
static void m68k_op_movem_re_ix_16(void){	movem16w_re(EA_IX); USE_CLKS(4+10);}
static void m68k_op_movem_re_aw_16(void){	movem16w_re(EA_AW); USE_CLKS(4+ 8);}
static void m68k_op_movem_re_al_16(void){	movem16w_re(EA_AL); USE_CLKS(4+12);}





#else
/* old type */


static void m68k_op_movem_pd_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
			ea -= 2;
		//	m68ki_write_16(ea, *(m68k_op_movem_pd_table[i]));
		//	m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_DA[15-i]));
		//	m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_D[15-i])); /* REG_DA[]==REG_D[] */
			m68ki_write_16(ea, MASK_OUT_ABOVE_16(REG_D[15-i])); /* REG_DA[]==REG_D[] */
			count++;
		}
	AY = ea;
	USE_CLKS((count << CYC_MOVEM_W) + 8);
}


static void m68k_op_movem_pd_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
			ea -= 4;
		//	m68ki_write_32(ea, *(m68k_op_movem_pd_table[i]));
			m68ki_write_32(ea, (REG_D[15-i])); /* REG_DA[]==REG_D[] */
			count++;
		}
	AY = ea;
	USE_CLKS((count << CYC_MOVEM_L) + 8);
}


static void m68k_op_movem_pi_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
		//	REG_DA[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	AY = ea;
	USE_CLKS((count << 2) + 12);
}


static void m68k_op_movem_pi_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = AY;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
		//	REG_DA[i] = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	AY = ea;
	USE_CLKS((count << CYC_MOVEM_L) + 12);
}


static void m68k_op_movem_re_ai_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_16(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_16(ea, REG_D[i]);
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 4+4);
}


static void m68k_op_movem_re_di_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_16(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_16(ea, REG_D[i]);
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 4+8);
}


static void m68k_op_movem_re_ix_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_16(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_16(ea, REG_D[i]);
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 4+10);
}


static void m68k_op_movem_re_aw_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_16(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_16(ea, REG_D[i]);
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 4+8);
}


static void m68k_op_movem_re_al_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_16(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_16(ea, REG_D[i]);
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 4+12);
}

/* old type */
#endif



static void m68k_op_movem_re_ai_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_32(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_32(ea, REG_D[i]);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 4+4);
}


static void m68k_op_movem_re_di_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_32(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_32(ea, REG_D[i]);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 4+8);
}


static void m68k_op_movem_re_ix_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_32(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_32(ea, REG_D[i]);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 4+10);
}


static void m68k_op_movem_re_aw_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_32(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_32(ea, REG_D[i]);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 4+8);
}


static void m68k_op_movem_re_al_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	m68ki_write_32(ea, *(m68k_op_movem_pi_table[i]));
			m68ki_write_32(ea, REG_D[i]);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 4+12);
}


static void m68k_op_movem_er_ai_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 8+4);
}


static void m68k_op_movem_er_di_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 8+8);
}


static void m68k_op_movem_er_ix_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 8+10);
}


static void m68k_op_movem_er_aw_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 8+8);
}


static void m68k_op_movem_er_al_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 8+12);
}


static void m68k_op_movem_er_pcdi_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 8+8);
}


static void m68k_op_movem_er_pcix_16(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_PCIX;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			REG_D[i] = MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_16(ea)));
			ea += 2;
			count++;
		}
	USE_CLKS((count << 2) + 8+10);
}


static void m68k_op_movem_er_ai_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 8+4);
}


static void m68k_op_movem_er_di_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_DI;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 8+8);
}


static void m68k_op_movem_er_ix_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_IX;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 8+10);
}


static void m68k_op_movem_er_aw_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AW;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 8+8);
}


static void m68k_op_movem_er_al_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_AL;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 8+12);
}


static void m68k_op_movem_er_pcdi_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 8+8);
}


static void m68k_op_movem_er_pcix_32(void)
{
	uint i = 0;
	uint register_list = m68ki_read_imm_16();
	uint ea = EA_PCIX;
	uint count = 0;

	for (; i < 16; i++)
		if (register_list & (1 << i))
		{
		//	*(m68k_op_movem_pi_table[i]) = m68ki_read_32(ea);
			REG_D[i] = m68ki_read_32(ea);
			ea += 4;
			count++;
		}
	USE_CLKS((count << CYC_MOVEM_L) + 8+10);
}





static void m68k_op_movep_re_16(void)
{
	uint ea = AY + MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_imm_16()));
	uint src = DX;

	m68ki_write_8(ea, MASK_OUT_ABOVE_8(src >> 8));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src));
	USE_CLKS(16);
}


static void m68k_op_movep_re_32(void)
{
	uint ea = AY + MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_imm_16()));
	uint src = DX;

	m68ki_write_8(ea, MASK_OUT_ABOVE_8(src >> 24));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src >> 16));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src >> 8));
	m68ki_write_8(ea += 2, MASK_OUT_ABOVE_8(src));
	USE_CLKS(24);
}


static void m68k_op_movep_er_16(void)
{
	uint ea = AY + MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_imm_16()));
	uint *d_dst = &DX;

	*d_dst = MASK_OUT_BELOW_16(*d_dst) | ((m68ki_read_8(ea) << 8) + m68ki_read_8(ea + 2));
	USE_CLKS(16);
}


static void m68k_op_movep_er_32(void)
{
	uint ea = AY + MAKE_INT_16(MASK_OUT_ABOVE_16(m68ki_read_imm_16()));

	DX = (m68ki_read_8(ea) << 24) + (m68ki_read_8(ea + 2) << 16)
		+ (m68ki_read_8(ea + 4) << 8) + m68ki_read_8(ea + 6);
	USE_CLKS(24);
}

static void m68k_op_moveq(void)
{
	uint res = DX = MAKE_INT_8(MASK_OUT_ABOVE_8(REG_IR));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_muls_d_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(DY) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54);
}


static void m68k_op_muls_ai_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_AI)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+4);
}


static void m68k_op_muls_pi_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_PI_16)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+4);
}


static void m68k_op_muls_pd_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_PD_16)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+6);
}


static void m68k_op_muls_di_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_DI)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+8);
}


static void m68k_op_muls_ix_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_IX)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+10);
}


static void m68k_op_muls_aw_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_AW)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+8);
}


static void m68k_op_muls_al_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_AL)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+12);
}


static void m68k_op_muls_pcdi_16(void)
{
	uint *d_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MAKE_INT_16(m68ki_read_16(ea)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+8);
}


static void m68k_op_muls_pcix_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_16(EA_PCIX)) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+10);
}


static void m68k_op_muls_i_16(void)
{
	uint *d_dst = &DX;
	uint res = MAKE_INT_16(m68ki_read_imm_16()) * MAKE_INT_16(MASK_OUT_ABOVE_16(*d_dst));

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+4);
}


static void m68k_op_mulu_d_16(void)
{
	uint *d_dst = &DX;
	uint res = MASK_OUT_ABOVE_16(DY) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54);
}


static void m68k_op_mulu_ai_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_AI) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+4);
}


static void m68k_op_mulu_pi_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_PI_16) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+4);
}


static void m68k_op_mulu_pd_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_PD_16) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+6);
}


static void m68k_op_mulu_di_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_DI) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+8);
}


static void m68k_op_mulu_ix_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_IX) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+10);
}


static void m68k_op_mulu_aw_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_AW) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+8);
}


static void m68k_op_mulu_al_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_AL) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+12);
}


static void m68k_op_mulu_pcdi_16(void)
{
	uint *d_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = m68ki_read_16(ea) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+8);
}


static void m68k_op_mulu_pcix_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_16(EA_PCIX) * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+10);
}


static void m68k_op_mulu_i_16(void)
{
	uint *d_dst = &DX;
	uint res = m68ki_read_imm_16() * MASK_OUT_ABOVE_16(*d_dst);

	*d_dst = res;

	FLAG_Z = res;
	FLAG_N = GET_MSB_32(res);
	FLAG_V = FLAG_C = 0;
	USE_CLKS(54+4);
}


