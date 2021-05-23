#include "m68kmpu.h"
#include "m68kops.h"

/* ======================================================================== */
/* ======================= INSTRUCTION HANDLERS N-Z ======================= */
/* ======================================================================== */

static void m68k_op_nbcd_d(void)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = 1;
		FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(6);
}


static void m68k_op_nbcd_ai(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+4);
}


static void m68k_op_nbcd_pi(void)
{
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+4);
}


static void m68k_op_nbcd_pi7(void)
{
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+4);
}


static void m68k_op_nbcd_pd(void)
{
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+6);
}


static void m68k_op_nbcd_pd7(void)
{
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+6);
}


static void m68k_op_nbcd_di(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+8);
}


static void m68k_op_nbcd_ix(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+10);
}


static void m68k_op_nbcd_aw(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+8);
}


static void m68k_op_nbcd_al(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(0x9a - dst - (FLAG_X != 0));

	if (res != 0x9a)
	{
		if ((res & 0x0f) == 0xa)
			res = (res & 0xf0) + 0x10;

		m68ki_write_8(ea, res);

		if (res != 0)
			FLAG_Z = 1;
		FLAG_C = FLAG_X = 1;
	}
	else
		FLAG_C = FLAG_X = 0;
	USE_CLKS(8+12);
}


static void m68k_op_neg_d_8(void)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(-dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(4);
}


static void m68k_op_neg_ai_8(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+4);
}


static void m68k_op_neg_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+4);
}


static void m68k_op_neg_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+4);
}


static void m68k_op_neg_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+6);
}


static void m68k_op_neg_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+6);
}


static void m68k_op_neg_di_8(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+8);
}


static void m68k_op_neg_ix_8(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+10);
}


static void m68k_op_neg_aw_8(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+8);
}


static void m68k_op_neg_al_8(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+12);
}


static void m68k_op_neg_d_16(void)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(-dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(4);
}


static void m68k_op_neg_ai_16(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+4);
}


static void m68k_op_neg_pi_16(void)
{
	uint ea  = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+4);
}


static void m68k_op_neg_pd_16(void)
{
	uint ea  = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+6);
}


static void m68k_op_neg_di_16(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+8);
}


static void m68k_op_neg_ix_16(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+10);
}


static void m68k_op_neg_aw_16(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+8);
}


static void m68k_op_neg_al_16(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(8+12);
}


static void m68k_op_neg_d_32(void)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(-dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(6);
}


static void m68k_op_neg_ai_32(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(12+8);
}


static void m68k_op_neg_pi_32(void)
{
	uint ea  = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(12+8);
}


static void m68k_op_neg_pd_32(void)
{
	uint ea  = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(12+10);
}


static void m68k_op_neg_di_32(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(12+12);
}


static void m68k_op_neg_ix_32(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(12+14);
}


static void m68k_op_neg_aw_32(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(12+12);
}


static void m68k_op_neg_al_32(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = res != 0;
	USE_CLKS(12+16);
}


static void m68k_op_negx_d_8(void)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(4);
}


static void m68k_op_negx_ai_8(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+4);
}


static void m68k_op_negx_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+4);
}


static void m68k_op_negx_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+4);
}


static void m68k_op_negx_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+6);
}


static void m68k_op_negx_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+6);
}


static void m68k_op_negx_di_8(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+8);
}


static void m68k_op_negx_ix_8(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+10);
}


static void m68k_op_negx_aw_8(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+8);
}


static void m68k_op_negx_al_8(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(-dst - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_8(dst & res);
	FLAG_C = FLAG_X = GET_MSB_8(dst | res);
	USE_CLKS(8+12);
}


static void m68k_op_negx_d_16(void)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(4);
}


static void m68k_op_negx_ai_16(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(8+4);
}


static void m68k_op_negx_pi_16(void)
{
	uint ea  = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(8+4);
}


static void m68k_op_negx_pd_16(void)
{
	uint ea  = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(8+6);
}


static void m68k_op_negx_di_16(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(8+8);
}


static void m68k_op_negx_ix_16(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(8+10);
}


static void m68k_op_negx_aw_16(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(8+8);
}


static void m68k_op_negx_al_16(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(-dst - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_16(dst & res);
	FLAG_C = FLAG_X = GET_MSB_16(dst | res);
	USE_CLKS(8+12);
}


static void m68k_op_negx_d_32(void)
{
	uint* r_dst = &DY;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(6);
}


static void m68k_op_negx_ai_32(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(12+8);
}


static void m68k_op_negx_pi_32(void)
{
	uint ea  = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(12+8);
}


static void m68k_op_negx_pd_32(void)
{
	uint ea  = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(12+10);
}


static void m68k_op_negx_di_32(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(12+12);
}


static void m68k_op_negx_ix_32(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(12+14);
}


static void m68k_op_negx_aw_32(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(12+12);
}


static void m68k_op_negx_al_32(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(-dst - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = GET_MSB_32(dst & res);
	FLAG_C = FLAG_X = GET_MSB_32(dst | res);
	USE_CLKS(12+16);
}


static void m68k_op_nop(void)
{
	USE_CLKS(4);
}


static void m68k_op_not_d_8(void)
{
	uint* r_dst = &DY;
	uint res = MASK_OUT_ABOVE_8(~*r_dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_not_ai_8(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_not_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_not_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_not_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_not_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_not_di_8(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_not_ix_8(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_not_aw_8(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_not_al_8(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_8(~m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_not_d_16(void)
{
	uint* r_dst = &DY;
	uint res = MASK_OUT_ABOVE_16(~*r_dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_not_ai_16(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_not_pi_16(void)
{
	uint ea  = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_not_pd_16(void)
{
	uint ea  = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_not_di_16(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_not_ix_16(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_not_aw_16(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_not_al_16(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_16(~m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_not_d_32(void)
{
	uint* r_dst = &DY;
	uint res = *r_dst = MASK_OUT_ABOVE_32(~*r_dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6);
}


static void m68k_op_not_ai_32(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_not_pi_32(void)
{
	uint ea  = EA_PI_32;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_not_pd_32(void)
{
	uint ea  = EA_PD_32;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_not_di_32(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_not_ix_32(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
//	USE_CLKS(12+14);
	USE_CLKS(12+6); /*26->18?*/
}


static void m68k_op_not_aw_32(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_not_al_32(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_32(~m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+16);
}


static void m68k_op_or_er_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= MASK_OUT_ABOVE_8(DY)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_or_er_ai_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_AI)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_or_er_pi_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PI_8)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_or_er_pi7_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PI7_8)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_or_er_pd_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PD_8)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+6);
}


static void m68k_op_or_er_pd7_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PD7_8)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+6);
}


static void m68k_op_or_er_di_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_DI)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_or_er_ix_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_IX)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_or_er_aw_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_AW)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_or_er_al_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_AL)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+12);
}


static void m68k_op_or_er_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(ea)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_or_er_pcix_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_8(EA_PCIX)));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_or_er_i_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DX |= m68ki_read_imm_8()));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_or_er_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= MASK_OUT_ABOVE_16(DY)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_or_er_ai_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_AI)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_or_er_pi_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_PI_16)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_or_er_pd_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_PD_16)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+6);
}


static void m68k_op_or_er_di_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_DI)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_or_er_ix_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_IX)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_or_er_aw_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_AW)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_or_er_al_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_AL)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+12);
}


static void m68k_op_or_er_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(ea)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_or_er_pcix_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_16(EA_PCIX)));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_or_er_i_16(void)
{
	uint res = MASK_OUT_ABOVE_16((DX |= m68ki_read_imm_16()));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_or_er_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= DY));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_or_er_ai_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_AI)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+8);
}


static void m68k_op_or_er_pi_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_PI_32)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+8);
}


static void m68k_op_or_er_pd_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_PD_32)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+10);
}


static void m68k_op_or_er_di_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_DI)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+12);
}


static void m68k_op_or_er_ix_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_IX)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+14);
}


static void m68k_op_or_er_aw_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_AW)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+12);
}


static void m68k_op_or_er_al_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_AL)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+16);
}


static void m68k_op_or_er_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(ea)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+12);
}


static void m68k_op_or_er_pcix_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_32(EA_PCIX)));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+14);
}


static void m68k_op_or_er_i_32(void)
{
	uint res = MASK_OUT_ABOVE_32((DX |= m68ki_read_imm_32()));

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+10);
}


static void m68k_op_or_re_ai_8(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_or_re_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_or_re_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_or_re_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_or_re_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_or_re_di_8(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_or_re_ix_8(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_or_re_aw_8(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_or_re_al_8(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_8(DX | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_or_re_ai_16(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_or_re_pi_16(void)
{
	uint ea  = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_or_re_pd_16(void)
{
	uint ea  = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_or_re_di_16(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_or_re_ix_16(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_or_re_aw_16(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_or_re_al_16(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_16(DX | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_or_re_ai_32(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_or_re_pi_32(void)
{
	uint ea  = EA_PI_32;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_or_re_pd_32(void)
{
	uint ea  = EA_PD_32;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_or_re_di_32(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_or_re_ix_32(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+14);
}


static void m68k_op_or_re_aw_32(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_or_re_al_32(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_32(DX | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+16);
}


static void m68k_op_ori_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8((DY |= m68ki_read_imm_8()));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_ori_ai_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_ori_pi_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_ori_pi7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_ori_pd_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_ori_pd7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_ori_di_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_ori_ix_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_ori_aw_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_ori_al_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_8(tmp | m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_ori_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY |= m68ki_read_imm_16());

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_ori_ai_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_ori_pi_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_ori_pd_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_ori_di_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_ori_ix_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_ori_aw_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_ori_al_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_16(tmp | m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_ori_d_32(void)
{
	uint res = MASK_OUT_ABOVE_32(DY |= m68ki_read_imm_32());

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(16);
}


static void m68k_op_ori_ai_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+8);
}


static void m68k_op_ori_pi_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_PI_32;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+8);
}


static void m68k_op_ori_pd_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_PD_32;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+10);
}


static void m68k_op_ori_di_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+12);
}


static void m68k_op_ori_ix_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+14);
}


static void m68k_op_ori_aw_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+12);
}


static void m68k_op_ori_al_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_32(tmp | m68ki_read_32(ea));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+16);
}


static void m68k_op_ori_to_ccr(void)
{
	m68ki_set_ccr(m68ki_get_ccr() | m68ki_read_imm_16());
	USE_CLKS(20);
}


static void m68k_op_ori_to_sr(void)
{
	uint or_val = m68ki_read_imm_16();

	if (FLAG_S)
	{
		m68ki_set_sr(m68ki_get_sr() | or_val);
		USE_CLKS(20);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}



static void m68k_op_pea_ai(void)
{
	uint ea  = EA_AI;

	m68ki_push_32(ea);
	USE_CLKS(0+12);
}


static void m68k_op_pea_di(void)
{
	uint ea  = EA_DI;

	m68ki_push_32(ea);
	USE_CLKS(0+16);
}


static void m68k_op_pea_ix(void)
{
	uint ea  = EA_IX;

	m68ki_push_32(ea);
	USE_CLKS(0+20);
}


static void m68k_op_pea_aw(void)
{
	uint ea  = EA_AW;

	m68ki_push_32(ea);
	USE_CLKS(0+16);
}


static void m68k_op_pea_al(void)
{
	uint ea  = EA_AL;

	m68ki_push_32(ea);
	USE_CLKS(0+20);
}


static void m68k_op_pea_pcdi(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));

	m68ki_push_32(ea);
	USE_CLKS(0+16);
}


static void m68k_op_pea_pcix(void)
{
	uint ea  = EA_PCIX;

	m68ki_push_32(ea);
	USE_CLKS(0+20);
}


static void m68k_op_rst(void)
{
	if (FLAG_S)
	{
		m68ki_output_reset();		   /* auto-disable (see m68kcpu.h) */
		USE_CLKS(132);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_ror_s_8(void)
{
	uint* r_dst = &DY;
	uint orig_shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROR_8(src, shift);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = (src >> ((shift - 1) & 7)) & 1;
	FLAG_V = 0;
	USE_CLKS((orig_shift << 1) + 6);
}


static void m68k_op_ror_s_16(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = ROR_16(src, shift);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = (src >> (shift - 1)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_ror_s_32(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = ROR_32(src, shift);

	*r_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = (src >> (shift - 1)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 8);
}


static void m68k_op_ror_r_8(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = ROR_8(src, shift);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
		FLAG_C = (src >> ((shift - 1) & 7)) & 1;
		FLAG_N = GET_MSB_8(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_ror_r_16(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 15;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = ROR_16(src, shift);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		FLAG_C = (src >> ((shift - 1) & 15)) & 1;
		FLAG_N = GET_MSB_16(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_ror_r_32(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 31;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = ROR_32(src, shift);

	USE_CLKS((orig_shift << 1) + 8);
	if (orig_shift != 0)
	{
		*r_dst = res;
		FLAG_C = (src >> ((shift - 1) & 31)) & 1;
		FLAG_N = GET_MSB_32(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_ror_ea_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_ror_ea_pi(void)
{
	uint ea  = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_ror_ea_pd(void)
{
	uint ea  = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_ror_ea_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_ror_ea_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_ror_ea_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_ror_ea_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = ROR_16(src, 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = src & 1;
	FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_rol_s_8(void)
{
	uint* r_dst = &DY;
	uint orig_shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = MASK_OUT_ABOVE_8(ROL_8(src, shift));

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = (src >> (8 - orig_shift)) & 1;
	FLAG_V = 0;
	USE_CLKS((orig_shift << 1) + 6);
}


static void m68k_op_rol_s_16(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, shift));

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = (src >> (16 - shift)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_rol_s_32(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = MASK_OUT_ABOVE_32(ROL_32(src, shift));

	*r_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = (src >> (32 - shift)) & 1;
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 8);
}


static void m68k_op_rol_r_8(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 7;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = MASK_OUT_ABOVE_8(ROL_8(src, shift));

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		if (shift != 0)
		{
			*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
			FLAG_C = (src >> (8 - shift)) & 1;
			FLAG_N = GET_MSB_8(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}
		FLAG_C = src & 1;
		FLAG_N = GET_MSB_8(src);
		FLAG_Z = src;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_rol_r_16(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 15;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, shift));

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		if (shift != 0)
		{
			*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
			FLAG_C = (src >> (16 - shift)) & 1;
			FLAG_N = GET_MSB_16(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}
		FLAG_C = src & 1;
		FLAG_N = GET_MSB_16(src);
		FLAG_Z = src;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_rol_r_32(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift & 31;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = MASK_OUT_ABOVE_32(ROL_32(src, shift));

	USE_CLKS((orig_shift << 1) + 8);
	if (orig_shift != 0)
	{
		*r_dst = res;

		FLAG_C = (src >> (32 - shift)) & 1;
		FLAG_N = GET_MSB_32(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_rol_ea_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_rol_ea_pi(void)
{
	uint ea  = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_rol_ea_pd(void)
{
	uint ea  = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_rol_ea_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_rol_ea_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_rol_ea_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_rol_ea_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(ROL_16(src, 1));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = GET_MSB_16(src);
	FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_roxr_s_8(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint tmp = ROR_9(src | ((FLAG_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_9(tmp);
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_roxr_s_16(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), shift);
	uint res = MASK_OUT_ABOVE_16(tmp);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_roxr_s_32(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = MASK_OUT_ABOVE_32((ROR_33(src, shift) & ~(1 << (32 - shift))) | ((FLAG_X != 0) << (32 - shift)));
	uint new_x_flag = src & (1 << (shift - 1));

	*r_dst = res;

	FLAG_C = FLAG_X = new_x_flag;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;

	USE_CLKS((shift << 1) + 8);
}


static void m68k_op_roxr_r_8(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 9;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint tmp = ROR_9(src | ((FLAG_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
		FLAG_C = FLAG_X = GET_MSB_9(tmp);
		FLAG_N = GET_MSB_8(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_roxr_r_16(void)
{
uint* r_dst;
uint orig_shift;

	r_dst = &DY;
	orig_shift = DX & 0x3f;

	USE_CLKS((orig_shift << 1) + 6);
	if(orig_shift != 0)
	{
	uint shift;
	uint src;
	uint tmp;
	uint res;

#if 1
		shift = orig_shift % 17;
#else
		shift = orig_shift;
	/*	while(17<shift){	shift-=17;	};*/
		if(17<shift){shift-=17;
		if(17<shift){shift-=17;
		if(17<shift){shift-=17; }}};
#endif
	src = MASK_OUT_ABOVE_16(*r_dst);
	tmp = ROR_17(src | ((FLAG_X != 0) << 16), shift);
	res = MASK_OUT_ABOVE_16(tmp);

		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		FLAG_C = FLAG_X = GET_MSB_17(tmp);
		FLAG_N = GET_MSB_16(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = 0;//GET_MSB_16(res);
	FLAG_Z = 1;//res;
	FLAG_V = 0;
}


static void m68k_op_roxr_r_32(void)
{
	uint* r_dst;
	uint orig_shift;
	uint shift;
	uint src;
	uint res;
	uint new_x_flag;
	r_dst = &DY;
	orig_shift = DX & 0x3f;
#if 00
	shift = orig_shift % 33;
#else
	shift = orig_shift; if(33<shift){	shift-=33;	};
#endif
	src = MASK_OUT_ABOVE_32(*r_dst);
	res = MASK_OUT_ABOVE_32((ROR_33(src, shift) & ~(1 << (32 - shift))) | ((FLAG_X != 0) << (32 - shift)));
	new_x_flag = src & (1 << (shift - 1));

	USE_CLKS((orig_shift << 1) + 8);
	if (shift != 0)
	{
		*r_dst = res;
		FLAG_X = new_x_flag;
	}
	else
		res = src;
	FLAG_C = FLAG_X;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_roxr_ea_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_roxr_ea_pi(void)
{
	uint ea  = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_roxr_ea_pd(void)
{
	uint ea  = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_roxr_ea_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_roxr_ea_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_roxr_ea_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_roxr_ea_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_16(ea);
	uint tmp = ROR_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_roxl_s_8(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint tmp = ROL_9(src | ((FLAG_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_9(tmp);
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_roxl_s_16(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), shift);
	uint res = MASK_OUT_ABOVE_16(tmp);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_roxl_s_32(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = MASK_OUT_ABOVE_32((ROL_33(src, shift) & ~(1 << (shift - 1))) | ((FLAG_X != 0) << (shift - 1)));
	uint new_x_flag = src & (1 << (32 - shift));

	*r_dst = res;

	FLAG_C = FLAG_X = new_x_flag;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;

	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_roxl_r_8(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 9;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint tmp = ROL_9(src | ((FLAG_X != 0) << 8), shift);
	uint res = MASK_OUT_ABOVE_8(tmp);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
		FLAG_C = FLAG_X = GET_MSB_9(tmp);
		FLAG_N = GET_MSB_8(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_roxl_r_16(void)
{
	uint* r_dst = &DY;
	uint orig_shift = DX & 0x3f;
	uint shift = orig_shift % 17;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), shift);
	uint res = MASK_OUT_ABOVE_16(tmp);

	USE_CLKS((orig_shift << 1) + 6);
	if (orig_shift != 0)
	{
		*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
		FLAG_C = FLAG_X = GET_MSB_17(tmp);
		FLAG_N = GET_MSB_16(res);
		FLAG_Z = res;
		FLAG_V = 0;
		return;
	}

	FLAG_C = FLAG_X;
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_roxl_r_32(void)
{
	uint* r_dst;
	uint orig_shift;
	uint shift;
	uint src;
	uint res;
	uint new_x_flag;

	r_dst = &DY;
	orig_shift = DX & 0x3f;
#if 00
	shift = orig_shift % 33;
#else
	shift = orig_shift; if(33<shift){	shift-=33;	};
#endif
	src = MASK_OUT_ABOVE_32(*r_dst);
	res = MASK_OUT_ABOVE_32((ROL_33(src, shift) & ~(1 << (shift - 1))) | ((FLAG_X != 0) << (shift - 1)));
	new_x_flag = src & (1 << (32 - shift));

	USE_CLKS((orig_shift << 1) + 8);
	if (shift != 0)
	{
		*r_dst = res;
		FLAG_X = new_x_flag;
	}
	else
		res = src;
	FLAG_C = FLAG_X;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_roxl_ea_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_roxl_ea_pi(void)
{
	uint ea  = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_roxl_ea_pd(void)
{
	uint ea  = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_roxl_ea_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_roxl_ea_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_roxl_ea_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_roxl_ea_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_16(ea);
	uint tmp = ROL_17(src | ((FLAG_X != 0) << 16), 1);
	uint res = MASK_OUT_ABOVE_16(tmp);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_X = GET_MSB_17(tmp);
	FLAG_V = 0;
	USE_CLKS(8+12);
}



static void m68k_op_rte(void)
{
	uint new_sr;
	uint new_pc;
	uint format_word;

	if (FLAG_S)
	{
		new_sr = m68ki_pull_16();
		new_pc = m68ki_pull_32();
		m68ki_branch_long(new_pc);
		m68ki_set_sr(new_sr);
		if (!(0/*FLAG_MODE & FLAG_MODE_010_PLUS*/))
		{
			USE_CLKS(20);
			return;
		}
		format_word = (m68ki_pull_16() >> 12) & 0xf;
		/* I'm ignoring code 8 (bus error and address error) */
		if (format_word != 0)
			/* Generate a new program counter from the format error vector */
			m68ki_set_pc(m68ki_read_32((EXCEPTION_FORMAT_ERROR<<2)/*+FLAG_VBR*/));
		USE_CLKS(20);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}



static void m68k_op_rtr(void)
{
	m68ki_set_ccr(m68ki_pull_16());
	m68ki_branch_long(m68ki_pull_32());
	USE_CLKS(20);
}


static void m68k_op_rts(void)
{
	m68ki_branch_long(m68ki_pull_32());
	USE_CLKS(16);
}


static void m68k_op_sbcd_rr(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (FLAG_X != 0);

	if (res > 9) res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res += 0xa0;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | MASK_OUT_ABOVE_8(res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(6);
}


static void m68k_op_sbcd_mm_ax7(void)
{
	uint src = m68ki_read_8(--(AY));
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (FLAG_X != 0);

	if (res > 9) res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res += 0xa0;

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}


static void m68k_op_sbcd_mm_ay7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (FLAG_X != 0);

	if (res > 9) res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res += 0xa0;

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}


static void m68k_op_sbcd_mm_axy7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (FLAG_X != 0);

	if (res > 9) res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res += 0xa0;

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}


static void m68k_op_sbcd_mm(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - (FLAG_X != 0);

	if (res > 9) res -= 6;
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res += 0xa0;

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}


static void m68k_op_st_d(void)
{
	DY |= 0xff;
	USE_CLKS(6);
}


static void m68k_op_st_ai(void)
{
	m68ki_write_8(EA_AI, 0xff);
	USE_CLKS(8+4);
}


static void m68k_op_st_pi(void)
{
	m68ki_write_8(EA_PI_8, 0xff);
	USE_CLKS(8+4);
}


static void m68k_op_st_pi7(void)
{
	m68ki_write_8(EA_PI7_8, 0xff);
	USE_CLKS(8+4);
}


static void m68k_op_st_pd(void)
{
	m68ki_write_8(EA_PD_8, 0xff);
	USE_CLKS(8+6);
}


static void m68k_op_st_pd7(void)
{
	m68ki_write_8(EA_PD7_8, 0xff);
	USE_CLKS(8+6);
}


static void m68k_op_st_di(void)
{
	m68ki_write_8(EA_DI, 0xff);
	USE_CLKS(8+8);
}


static void m68k_op_st_ix(void)
{
	m68ki_write_8(EA_IX, 0xff);
	USE_CLKS(8+10);
}


static void m68k_op_st_aw(void)
{
	m68ki_write_8(EA_AW, 0xff);
	USE_CLKS(8+8);
}


static void m68k_op_st_al(void)
{
	m68ki_write_8(EA_AL, 0xff);
	USE_CLKS(8+12);
}


static void m68k_op_sf_d(void)
{
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_sf_ai(void)
{
	m68ki_write_8(EA_AI, 0);
	USE_CLKS(8+4);
}


static void m68k_op_sf_pi(void)
{
	m68ki_write_8(EA_PI_8, 0);
	USE_CLKS(8+4);
}


static void m68k_op_sf_pi7(void)
{
	m68ki_write_8(EA_PI7_8, 0);
	USE_CLKS(8+4);
}


static void m68k_op_sf_pd(void)
{
	m68ki_write_8(EA_PD_8, 0);
	USE_CLKS(8+6);
}


static void m68k_op_sf_pd7(void)
{
	m68ki_write_8(EA_PD7_8, 0);
	USE_CLKS(8+6);
}


static void m68k_op_sf_di(void)
{
	m68ki_write_8(EA_DI, 0);
	USE_CLKS(8+8);
}


static void m68k_op_sf_ix(void)
{
	m68ki_write_8(EA_IX, 0);
	USE_CLKS(8+10);
}


static void m68k_op_sf_aw(void)
{
	m68ki_write_8(EA_AW, 0);
	USE_CLKS(8+8);
}


static void m68k_op_sf_al(void)
{
	m68ki_write_8(EA_AL, 0);
	USE_CLKS(8+12);
}


static void m68k_op_shi_d(void)
{
	if (CONDITION_HI)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_shi_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_shi_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_shi_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_shi_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_shi_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_shi_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_shi_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_shi_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_shi_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_HI ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_sls_d(void)
{
	if (CONDITION_LS)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_sls_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sls_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sls_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sls_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sls_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sls_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sls_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_sls_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sls_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_LS ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_scc_d(void)
{
	if (CONDITION_CC)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_scc_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_scc_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_scc_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_scc_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_scc_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_scc_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_scc_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_scc_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_scc_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_CC ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_scs_d(void)
{
	if (CONDITION_CS)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_scs_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_scs_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_scs_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_scs_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_scs_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_scs_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_scs_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_scs_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_scs_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_CS ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_sne_d(void)
{
	if (CONDITION_NE)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_sne_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sne_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sne_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sne_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sne_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sne_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sne_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_sne_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sne_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_NE ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_seq_d(void)
{
	if (CONDITION_EQ)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_seq_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_seq_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_seq_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_seq_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_seq_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_seq_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_seq_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_seq_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_seq_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_EQ ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_svc_d(void)
{
	if (CONDITION_VC)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_svc_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_svc_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_svc_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_svc_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_svc_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_svc_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_svc_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_svc_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_svc_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_VC ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_svs_d(void)
{
	if (CONDITION_VS)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_svs_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_svs_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_svs_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_svs_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_svs_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_svs_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_svs_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_svs_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_svs_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_VS ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_spl_d(void)
{
	if (CONDITION_PL)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_spl_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_spl_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_spl_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_spl_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_spl_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_spl_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_spl_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_spl_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_spl_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_PL ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_smi_d(void)
{
	if (CONDITION_MI)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_smi_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_smi_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_smi_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_smi_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_smi_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_smi_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_smi_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_smi_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_smi_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_MI ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_sge_d(void)
{
	if (CONDITION_GE)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_sge_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sge_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sge_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sge_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sge_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sge_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sge_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_sge_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sge_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_GE ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_slt_d(void)
{
	if (CONDITION_LT)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_slt_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_slt_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_slt_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_slt_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_slt_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_slt_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_slt_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_slt_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_slt_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_LT ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_sgt_d(void)
{
	if (CONDITION_GT)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_sgt_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sgt_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sgt_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sgt_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sgt_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sgt_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sgt_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_sgt_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sgt_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_GT ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_sle_d(void)
{
	if (CONDITION_LE)
	{
		DY |= 0xff;
		USE_CLKS(6);
		return;
	}
	DY &= 0xffffff00;
	USE_CLKS(4);
}


static void m68k_op_sle_ai(void)
{
	m68ki_write_8(EA_AI, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sle_pi(void)
{
	m68ki_write_8(EA_PI_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sle_pi7(void)
{
	m68ki_write_8(EA_PI7_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+4);
}


static void m68k_op_sle_pd(void)
{
	m68ki_write_8(EA_PD_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sle_pd7(void)
{
	m68ki_write_8(EA_PD7_8, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+6);
}


static void m68k_op_sle_di(void)
{
	m68ki_write_8(EA_DI, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sle_ix(void)
{
	m68ki_write_8(EA_IX, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+10);
}


static void m68k_op_sle_aw(void)
{
	m68ki_write_8(EA_AW, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+8);
}


static void m68k_op_sle_al(void)
{
	m68ki_write_8(EA_AL, CONDITION_LE ? 0xff : 0);
	USE_CLKS(8+12);
}


static void m68k_op_stop(void)
{
	uint new_sr = m68ki_read_imm_16();

	if (FLAG_S)
	{
		CPU_STOPPED = 1;
		m68ki_set_sr(new_sr);
		m68ki_remaining_cycles = 0;
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_sub_er_d_8(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_sub_er_ai_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_AI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_sub_er_pi_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PI_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_sub_er_pi7_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PI7_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_sub_er_pd_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PD_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_sub_er_pd7_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PD7_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_sub_er_di_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_DI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_sub_er_ix_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_IX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_sub_er_aw_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_AW);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_sub_er_al_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_AL);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+12);
}


static void m68k_op_sub_er_pcdi_8(void)
{
	uint* r_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_8(ea);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_sub_er_pcix_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PCIX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_sub_er_i_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_imm_8();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_sub_er_d_16(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_sub_er_a_16(void)
{
	uint* r_dst = &DX;
	uint src = AY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_sub_er_ai_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_AI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_sub_er_pi_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_PI_16);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_sub_er_pd_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_PD_16);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_sub_er_di_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_DI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_sub_er_ix_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_IX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_sub_er_aw_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_AW);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_sub_er_al_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_AL);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+12);
}


static void m68k_op_sub_er_pcdi_16(void)
{
	uint* r_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_16(ea);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_sub_er_pcix_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_PCIX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_sub_er_i_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_imm_16();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_sub_er_d_32(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_sub_er_a_32(void)
{
	uint* r_dst = &DX;
	uint src = AY;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_sub_er_ai_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_AI);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_sub_er_pi_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_sub_er_pd_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_sub_er_di_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_DI);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_sub_er_ix_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_IX);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_sub_er_aw_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_AW);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_sub_er_al_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_AL);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+16);
}


static void m68k_op_sub_er_pcdi_32(void)
{
	uint* r_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_sub_er_pcix_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_sub_er_i_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_imm_32();
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_sub_re_ai_8(void)
{
	uint ea  = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_sub_re_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_sub_re_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_sub_re_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_sub_re_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_sub_re_di_8(void)
{
	uint ea  = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_sub_re_ix_8(void)
{
	uint ea  = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_sub_re_aw_8(void)
{
	uint ea  = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_sub_re_al_8(void)
{
	uint ea  = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_sub_re_ai_16(void)
{
	uint ea  = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_sub_re_pi_16(void)
{
	uint ea  = EA_PI_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_sub_re_pd_16(void)
{
	uint ea  = EA_PD_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_sub_re_di_16(void)
{
	uint ea  = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_sub_re_ix_16(void)
{
	uint ea  = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_sub_re_aw_16(void)
{
	uint ea  = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_sub_re_al_16(void)
{
	uint ea  = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_sub_re_ai_32(void)
{
	uint ea  = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_sub_re_pi_32(void)
{
	uint ea  = EA_PI_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_sub_re_pd_32(void)
{
	uint ea  = EA_PD_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_sub_re_di_32(void)
{
	uint ea  = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_sub_re_ix_32(void)
{
	uint ea  = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+14);
}


static void m68k_op_sub_re_aw_32(void)
{
	uint ea  = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_sub_re_al_32(void)
{
	uint ea  = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+16);
}


static void m68k_op_suba_d_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(DY));
	USE_CLKS(8);
}


static void m68k_op_suba_a_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(AY));
	USE_CLKS(8);
}


static void m68k_op_suba_ai_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_AI)));
	USE_CLKS(8+4);
}


static void m68k_op_suba_pi_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_PI_16)));
	USE_CLKS(8+4);
}


static void m68k_op_suba_pd_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_PD_16)));
	USE_CLKS(8+6);
}


static void m68k_op_suba_di_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_DI)));
	USE_CLKS(8+8);
}


static void m68k_op_suba_ix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_IX)));
	USE_CLKS(8+10);
}


static void m68k_op_suba_aw_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_AW)));
	USE_CLKS(8+8);
}


static void m68k_op_suba_al_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_AL)));
	USE_CLKS(8+12);
}


static void m68k_op_suba_pcdi_16(void)
{
	uint *a_dst = &AX;

	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(ea)));
	USE_CLKS(8+8);
}


static void m68k_op_suba_pcix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_16(EA_PCIX)));
	USE_CLKS(8+10);
}


static void m68k_op_suba_i_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - MAKE_INT_16(m68ki_read_imm_16()));
	USE_CLKS(8+4);
}


static void m68k_op_suba_d_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - DY);
	USE_CLKS(8);
}


static void m68k_op_suba_a_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - AY);
	USE_CLKS(8);
}


static void m68k_op_suba_ai_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_AI));
	USE_CLKS(6+8);
}


static void m68k_op_suba_pi_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_PI_32));
	USE_CLKS(6+8);
}


static void m68k_op_suba_pd_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_PD_32));
	USE_CLKS(6+10);
}


static void m68k_op_suba_di_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_DI));
	USE_CLKS(6+12);
}


static void m68k_op_suba_ix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_IX));
	USE_CLKS(6+14);
}


static void m68k_op_suba_aw_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_AW));
	USE_CLKS(6+12);
}


static void m68k_op_suba_al_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_AL));
	USE_CLKS(6+16);
}


static void m68k_op_suba_pcdi_32(void)
{
	uint *a_dst = &AX;

	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(ea));
	USE_CLKS(6+12);
}


static void m68k_op_suba_pcix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_32(EA_PCIX));
	USE_CLKS(6+14);
}


static void m68k_op_suba_i_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - m68ki_read_imm_32());
	USE_CLKS(6+10);
}


static void m68k_op_subi_d_8(void)
{
	uint* r_dst = &DY;
	uint src = m68ki_read_imm_8();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_subi_ai_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_subi_pi_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_subi_pi7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_subi_pd_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+6);
}


static void m68k_op_subi_pd7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+6);
}


static void m68k_op_subi_di_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_subi_ix_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_subi_aw_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_subi_al_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_subi_d_16(void)
{
	uint* r_dst = &DY;
	uint src = m68ki_read_imm_16();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_subi_ai_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_subi_pi_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_subi_pd_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+6);
}


static void m68k_op_subi_di_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_subi_ix_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_subi_aw_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_subi_al_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_subi_d_32(void)
{
	uint* r_dst = &DY;
	uint src = m68ki_read_imm_32();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	*r_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(16);
}


static void m68k_op_subi_ai_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+8);
}


static void m68k_op_subi_pi_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+8);
}


static void m68k_op_subi_pd_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+10);
}


static void m68k_op_subi_di_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+12);
}


static void m68k_op_subi_ix_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+14);
}


static void m68k_op_subi_aw_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+12);
}


static void m68k_op_subi_al_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(20+16);
}


static void m68k_op_subq_d_8(void)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_subq_ai_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_subq_pi_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_subq_pi7_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_subq_pd_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_subq_pd7_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_subq_di_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_subq_ix_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_subq_aw_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_subq_al_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_subq_d_16(void)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_subq_a_16(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - ((((REG_IR >> 9) - 1) & 7) + 1));
	USE_CLKS(8);
}


static void m68k_op_subq_ai_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_subq_pi_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_subq_pd_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_subq_di_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_subq_ix_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_subq_aw_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_subq_al_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_subq_d_32(void)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	*r_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_subq_a_32(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst - ((((REG_IR >> 9) - 1) & 7) + 1));
	USE_CLKS(8);
}


static void m68k_op_subq_ai_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_subq_pi_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_subq_pd_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_subq_di_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_subq_ix_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+14);
}


static void m68k_op_subq_aw_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_subq_al_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+16);
}


static void m68k_op_subx_rr_8(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(dst - src - (FLAG_X != 0));

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_subx_rr_16(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(dst - src - (FLAG_X != 0));

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_subx_rr_32(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_32(dst - src - (FLAG_X != 0));

	*r_dst = res;

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_subx_mm_8_ax7(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_subx_mm_8_ay7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_subx_mm_8_axy7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_subx_mm_8(void)
{
	uint src = m68ki_read_8(--(AY));
	uint ea  = --(AX);
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(dst - src - (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_8(src, dst, res);
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_subx_mm_16(void)
{
	uint src = m68ki_read_16(AY -= 2);
	uint ea  = (AX -= 2);
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(dst - src - (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_16(src, dst, res);
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_subx_mm_32(void)
{
	uint src = m68ki_read_32(AY -= 4);
	uint ea  = (AX -= 4);
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(dst - src - (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	USE_CLKS(30);
}


static void m68k_op_swap(void)
{
	uint* r_dst = &DY;
#if 00
//	*r_dst ^= (*r_dst >> 16) & 0x0000ffff;
//	*r_dst ^= (*r_dst << 16) & 0xffff0000;
//	*r_dst ^= (*r_dst >> 16) & 0x0000ffff;
#endif
	*r_dst = ((*r_dst>>16) | (MASK_OUT_ABOVE_32(*r_dst<<16)));

	FLAG_N = GET_MSB_32(*r_dst);
	FLAG_Z = *r_dst;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_tas_d(void)
{
	uint* r_dst = &DY;

	FLAG_Z = MASK_OUT_ABOVE_8(*r_dst);
	FLAG_N = GET_MSB_8(*r_dst);
	FLAG_V = FLAG_C = 0;
	*r_dst |= 0x80;
	USE_CLKS(4);
}


static void m68k_op_tas_ai(void)
{
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+4);
}


static void m68k_op_tas_pi(void)
{
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+4);
}


static void m68k_op_tas_pi7(void)
{
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+4);
}


static void m68k_op_tas_pd(void)
{
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+6);
}


static void m68k_op_tas_pd7(void)
{
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+6);
}


static void m68k_op_tas_di(void)
{
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+8);
}


static void m68k_op_tas_ix(void)
{
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+10);
}


static void m68k_op_tas_aw(void)
{
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+8);
}


static void m68k_op_tas_al(void)
{
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);

	FLAG_Z = dst;
	FLAG_N = GET_MSB_8(dst);
	FLAG_V = FLAG_C = 0;
//#if GENESIS_HACKS
	/* Write-back prevented in this system */
//#else
	//m68ki_write_8(ea, dst | 0x80);
//#endif
	USE_CLKS(14+12);
}


static void m68k_op_trap(void)
{
	m68ki_interrupt(EXCEPTION_TRAP_BASE + (REG_IR & 0xf));	/* HJB 990403 */
}


static void m68k_op_trapv(void)
{
	if (!FLAG_V)
	{
		USE_CLKS(4);
		return;
	}
	m68ki_interrupt(EXCEPTION_TRAPV);  /* HJB 990403 */
}


static void m68k_op_tst_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_tst_ai_8(void)
{
	uint ea  = EA_AI;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_tst_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_tst_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_tst_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+6);
}


static void m68k_op_tst_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+6);
}


static void m68k_op_tst_di_8(void)
{
	uint ea  = EA_DI;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_tst_ix_8(void)
{
	uint ea  = EA_IX;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_tst_aw_8(void)
{
	uint ea  = EA_AW;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_tst_al_8(void)
{
	uint ea  = EA_AL;
	uint res = m68ki_read_8(ea);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}




static void m68k_op_tst_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}

static void m68k_op_tst_ai_16(void)
{
	uint res = m68ki_read_16(EA_AI);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_tst_pi_16(void)
{
	uint res = m68ki_read_16(EA_PI_16);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+4);
}


static void m68k_op_tst_pd_16(void)
{
	uint res = m68ki_read_16(EA_PD_16);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+6);
}


static void m68k_op_tst_di_16(void)
{
	uint res = m68ki_read_16(EA_DI);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_tst_ix_16(void)
{
	uint res = m68ki_read_16(EA_IX);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_tst_aw_16(void)
{
	uint res = m68ki_read_16(EA_AW);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_tst_al_16(void)
{
	uint res = m68ki_read_16(EA_AL);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}



static void m68k_op_tst_d_32(void)
{
	uint res = DY;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4);
}


static void m68k_op_tst_ai_32(void)
{
	uint res = m68ki_read_32(EA_AI);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_tst_pi_32(void)
{
	uint res = m68ki_read_32(EA_PI_32);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+8);
}


static void m68k_op_tst_pd_32(void)
{
	uint res = m68ki_read_32(EA_PD_32);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+10);
}


static void m68k_op_tst_di_32(void)
{
	uint res = m68ki_read_32(EA_DI);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}


static void m68k_op_tst_ix_32(void)
{
	uint res = m68ki_read_32(EA_IX);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+14);
}


static void m68k_op_tst_aw_32(void)
{
	uint res = m68ki_read_32(EA_AW);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+12);
}


static void m68k_op_tst_al_32(void)
{
	uint res = m68ki_read_32(EA_AL);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = FLAG_C = 0;
	USE_CLKS(4+16);
}




static void m68k_op_unlk_a7(void)
{
	REG_A[7] = m68ki_read_32(REG_A[7]);
	USE_CLKS(12);
}


static void m68k_op_unlk(void)
{
	uint *a_dst = &AY;

	REG_A[7] = *a_dst;
	*a_dst = m68ki_pull_32();
	USE_CLKS(12);
}

