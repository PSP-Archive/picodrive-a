#include "m68kmpu.h"
#include "m68kops.h"

/* ======================================================================== */
/* ======================= INSTRUCTION HANDLERS A-C ======================= */
/* ======================================================================== */

static void m68k_op_1010(void)
{
	m68ki_exception(EXCEPTION_1010);
//	M68K_DO_LOG_EMU((M68K_LOG, "%s at %08x: called 1010 instruction %04x (%s)\n",
//					 m68k_op_mpu_names[FLAG_MODE], ADDRESS_68K(REG_PC - 2), REG_IR,
//					 m68k_op_disassemble_quick(ADDRESS_68K(REG_PC - 2))));
}


static void m68k_op_1111(void)
{
	m68ki_exception(EXCEPTION_1111);
//	M68K_DO_LOG_EMU((M68K_LOG, "%s at %08x: called 1111 instruction %04x (%s)\n",
//					 m68k_op_mpu_names[FLAG_MODE], ADDRESS_68K(REG_PC - 2), REG_IR,
//					 m68k_op_disassemble_quick(ADDRESS_68K(REG_PC - 2))));
}

#if 00
static void m68k_op_abcd_rr(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	FLAG_V = ~res; /* Undefined V behavior */

	if(res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	FLAG_V &= res; /* Undefined V behavior part II */
	FLAG_N = NFLAG_8(res); /* Undefined N behavior */

	res = MASK_OUT_ABOVE_8(res);
	//if (res != 0) FLAG_Z = 1;
	FLAG_Z |= res;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	USE_CLKS(6);
}


static void m68k_op_abcd_mm_ax7(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	FLAG_V = ~res; /* Undefined V behavior */

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	FLAG_V &= res; /* Undefined V behavior part II */
	FLAG_N = NFLAG_8(res); /* Undefined N behavior */

	res = MASK_OUT_ABOVE_8(res);
//	if (res != 0)	FLAG_Z = 1;
	FLAG_Z |= res;

	m68ki_write_8(ea, res);

	USE_CLKS(18);
}


static void m68k_op_abcd_mm_ay7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	FLAG_V = ~res; /* Undefined V behavior */

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	FLAG_V &= res; /* Undefined V behavior part II */
	FLAG_N = NFLAG_8(res); /* Undefined N behavior */

	res = MASK_OUT_ABOVE_8(res);
//	if (res != 0)	FLAG_Z = 1;
	FLAG_Z |= res;

	m68ki_write_8(ea, res);

	USE_CLKS(18);
}


static void m68k_op_abcd_mm_axy7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	FLAG_V = ~res; /* Undefined V behavior */

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	FLAG_V &= res; /* Undefined V behavior part II */
	FLAG_N = NFLAG_8(res); /* Undefined N behavior */

	res = MASK_OUT_ABOVE_8(res);
//	if (res != 0)	FLAG_Z = 1;
	FLAG_Z |= res;

	m68ki_write_8(ea, res);

	USE_CLKS(18);
}


static void m68k_op_abcd_mm(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	FLAG_V = ~res; /* Undefined V behavior */

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	FLAG_V &= res; /* Undefined V behavior part II */
	FLAG_N = NFLAG_8(res); /* Undefined N behavior */

	res = MASK_OUT_ABOVE_8(res);
//	if (res != 0)	FLAG_Z = 1;
	FLAG_Z |= res;

	m68ki_write_8(ea, res);

	USE_CLKS(18);
}
#endif
//////////////////////
static void m68k_op_abcd_rr(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | MASK_OUT_ABOVE_8(res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(6);
}


static void m68k_op_abcd_mm_ax7(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}


static void m68k_op_abcd_mm_ay7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}


static void m68k_op_abcd_mm_axy7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}


static void m68k_op_abcd_mm(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = LOW_NIBBLE(src) + LOW_NIBBLE(dst) + (FLAG_X != 0);

	if (res > 9) res += 6;
	res += HIGH_NIBBLE(src) + HIGH_NIBBLE(dst);
	if ((FLAG_X = FLAG_C = res > 0x99) != 0)
		res -= 0xa0;

	FLAG_N = GET_MSB_8(res); /* officially undefined */

	m68ki_write_8(ea, res);

	if (MASK_OUT_ABOVE_8(res) != 0)
		FLAG_Z = 1;
	USE_CLKS(18);
}
//////////////////////
static void m68k_op_add_er_d_8(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_add_er_ai_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_AI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_add_er_pi_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PI_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_add_er_pi7_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PI7_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_add_er_pd_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PD_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_add_er_pd7_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PD7_8);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_add_er_di_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_DI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_add_er_ix_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_IX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_add_er_aw_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_AW);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_add_er_al_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_AL);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+12);
}


static void m68k_op_add_er_pcdi_8(void)
{
	uint* r_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_8(ea);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_add_er_pcix_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_8(EA_PCIX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_add_er_i_8(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_imm_8();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_add_er_d_16(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_add_er_a_16(void)
{
	uint* r_dst = &DX;
	uint src = AY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_add_er_ai_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_AI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_add_er_pi_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_PI_16);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_add_er_pd_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_PD_16);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_add_er_di_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_DI);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_add_er_ix_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_IX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_add_er_aw_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_AW);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_add_er_al_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_AL);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+12);
}


static void m68k_op_add_er_pcdi_16(void)
{
	uint* r_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_16(ea);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_add_er_pcix_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_16(EA_PCIX);
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_add_er_i_16(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_imm_16();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_add_er_d_32(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_add_er_a_32(void)
{
	uint* r_dst = &DX;
	uint src = AY;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_add_er_ai_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_AI);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_add_er_pi_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_add_er_pd_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_add_er_di_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_DI);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_add_er_ix_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_IX);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_add_er_aw_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_AW);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_add_er_al_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_AL);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+16);
}


static void m68k_op_add_er_pcdi_32(void)
{
	uint* r_dst = &DX;
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_add_er_pcix_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_add_er_i_32(void)
{
	uint* r_dst = &DX;
	uint src = m68ki_read_imm_32();
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_add_re_ai_8(void)
{
	uint ea  = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_add_re_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_add_re_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_add_re_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_add_re_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_add_re_di_8(void)
{
	uint ea  = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_add_re_ix_8(void)
{
	uint ea  = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_add_re_aw_8(void)
{
	uint ea  = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_add_re_al_8(void)
{
	uint ea  = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_add_re_ai_16(void)
{
	uint ea  = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_add_re_pi_16(void)
{
	uint ea  = EA_PI_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_add_re_pd_16(void)
{
	uint ea  = EA_PD_16;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_add_re_di_16(void)
{
	uint ea  = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_add_re_ix_16(void)
{
	uint ea  = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_add_re_aw_16(void)
{
	uint ea  = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_add_re_al_16(void)
{
	uint ea  = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_add_re_ai_32(void)
{
	uint ea  = EA_AI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_add_re_pi_32(void)
{
	uint ea  = EA_PI_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_add_re_pd_32(void)
{
	uint ea  = EA_PD_32;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_add_re_di_32(void)
{
	uint ea  = EA_DI;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_add_re_ix_32(void)
{
	uint ea  = EA_IX;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+14);
}


static void m68k_op_add_re_aw_32(void)
{
	uint ea  = EA_AW;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_add_re_al_32(void)
{
	uint ea  = EA_AL;
	uint src = DX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+16);
}


static void m68k_op_adda_d_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(DY));
	USE_CLKS(8);
}


static void m68k_op_adda_a_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(AY));
	USE_CLKS(8);
}


static void m68k_op_adda_ai_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_AI)));
	USE_CLKS(8+4);
}


static void m68k_op_adda_pi_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_PI_16)));
	USE_CLKS(8+4);
}


static void m68k_op_adda_pd_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_PD_16)));
	USE_CLKS(8+6);
}


static void m68k_op_adda_di_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_DI)));
	USE_CLKS(8+8);
}


static void m68k_op_adda_ix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_IX)));
	USE_CLKS(8+10);
}


static void m68k_op_adda_aw_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_AW)));
	USE_CLKS(8+8);
}


static void m68k_op_adda_al_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_AL)));
	USE_CLKS(8+12);
}


static void m68k_op_adda_pcdi_16(void)
{
	uint *a_dst = &AX;

	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(ea)));
	USE_CLKS(8+8);
}


static void m68k_op_adda_pcix_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_16(EA_PCIX)));
	USE_CLKS(8+10);
}


static void m68k_op_adda_i_16(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + MAKE_INT_16(m68ki_read_imm_16()));
	USE_CLKS(8+4);
}


static void m68k_op_adda_d_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + DY);
	USE_CLKS(8);
}


static void m68k_op_adda_a_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + AY);
	USE_CLKS(8);
}


static void m68k_op_adda_ai_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_AI));
	USE_CLKS(6+8);
}


static void m68k_op_adda_pi_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_PI_32));
	USE_CLKS(6+8);
}


static void m68k_op_adda_pd_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_PD_32));
	USE_CLKS(6+10);
}


static void m68k_op_adda_di_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_DI));
	USE_CLKS(6+12);
}


static void m68k_op_adda_ix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_IX));
	USE_CLKS(6+14);
}


static void m68k_op_adda_aw_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_AW));
	USE_CLKS(6+12);
}


static void m68k_op_adda_al_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_AL));
	USE_CLKS(6+16);
}


static void m68k_op_adda_pcdi_32(void)
{
	uint *a_dst = &AX;

	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(ea));
	USE_CLKS(6+12);
}


static void m68k_op_adda_pcix_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_32(EA_PCIX));
	USE_CLKS(6+14);
}


static void m68k_op_adda_i_32(void)
{
	uint *a_dst = &AX;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + m68ki_read_imm_32());
	USE_CLKS(6+10);
}


static void m68k_op_addi_d_8(void)
{
	uint* r_dst = &DY;
	uint src = m68ki_read_imm_8();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_addi_ai_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_addi_pi_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_addi_pi7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_addi_pd_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+6);
}


static void m68k_op_addi_pd7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+6);
}


static void m68k_op_addi_di_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_addi_ix_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_addi_aw_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_addi_al_8(void)
{
	uint src = m68ki_read_imm_8();
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_addi_d_16(void)
{
	uint* r_dst = &DY;
	uint src = m68ki_read_imm_16();
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_addi_ai_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_addi_pi_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+4);
}


static void m68k_op_addi_pd_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+6);
}


static void m68k_op_addi_di_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_addi_ix_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_addi_aw_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_addi_al_16(void)
{
	uint src = m68ki_read_imm_16();
	uint ea  = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_addi_d_32(void)
{
	uint* r_dst = &DY;
	uint src = m68ki_read_imm_32();
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(16);
}


static void m68k_op_addi_ai_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+8);
}


static void m68k_op_addi_pi_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+8);
}


static void m68k_op_addi_pd_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+10);
}


static void m68k_op_addi_di_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+12);
}


static void m68k_op_addi_ix_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+14);
}


static void m68k_op_addi_aw_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+12);
}


static void m68k_op_addi_al_32(void)
{
	uint src = m68ki_read_imm_32();
	uint ea  = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(20+16);
}


static void m68k_op_addq_d_8(void)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst);

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_addq_ai_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_addq_pi_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_addq_pi7_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_addq_pd_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_addq_pd7_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD7_8;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_addq_di_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_DI;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_addq_ix_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_IX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_addq_aw_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AW;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_addq_al_8(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AL;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_addq_d_16(void)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst);

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_addq_a_16(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + (((REG_IR >> 9) - 1) & 7) + 1);
	USE_CLKS(4);
}


static void m68k_op_addq_ai_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_addq_pi_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_addq_pd_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD_16;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_addq_di_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_DI;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_addq_ix_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_IX;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_addq_aw_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AW;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_addq_al_16(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AL;
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(8+12);
}


static void m68k_op_addq_d_32(void)
{
	uint* r_dst = &DY;
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_addq_a_32(void)
{
	uint *a_dst = &AY;

	*a_dst = MASK_OUT_ABOVE_32(*a_dst + (((REG_IR >> 9) - 1) & 7) + 1);
	USE_CLKS(8);
}


static void m68k_op_addq_ai_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_addq_pi_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PI_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_addq_pd_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_PD_32;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_addq_di_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_DI;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_addq_ix_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_IX;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+14);
}


static void m68k_op_addq_aw_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AW;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_addq_al_32(void)
{
	uint src = (((REG_IR >> 9) - 1) & 7) + 1;
	uint ea  = EA_AL;
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(12+16);
}


static void m68k_op_addx_rr_8(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_8(src + dst + (FLAG_X != 0));

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_addx_rr_16(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = MASK_OUT_ABOVE_16(src + dst + (FLAG_X != 0));

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_addx_rr_32(void)
{
	uint* r_dst = &DX;
	uint src = DY;
	uint dst = *r_dst;
	uint res = *r_dst = MASK_OUT_ABOVE_32(src + dst + (FLAG_X != 0));

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_addx_mm_8_ax7(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_addx_mm_8_ay7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_addx_mm_8_axy7(void)
{
	uint src = m68ki_read_8(REG_A[7] -= 2);
	uint ea  = REG_A[7] -= 2;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_addx_mm_8(void)
{
	uint src = m68ki_read_8(--AY);
	uint ea  = --AX;
	uint dst = m68ki_read_8(ea);
	uint res = MASK_OUT_ABOVE_8(src + dst + (FLAG_X != 0));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_8(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_8(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_addx_mm_16(void)
{
	uint src = m68ki_read_16(AY -= 2);
	uint ea  = (AX -= 2);
	uint dst = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src + dst + (FLAG_X != 0));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_16(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_16(src, dst, res);
	USE_CLKS(18);
}


static void m68k_op_addx_mm_32(void)
{
	uint src = m68ki_read_32(AY -= 4);
	uint ea  = (AX -= 4);
	uint dst = m68ki_read_32(ea);
	uint res = MASK_OUT_ABOVE_32(src + dst + (FLAG_X != 0));

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	if (res != 0)
		FLAG_Z = 1;
	FLAG_V = VFLAG_ADD_32(src, dst, res);
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);
	USE_CLKS(30);
}


static void m68k_op_and_er_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (DY | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_and_er_ai_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_AI) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_and_er_pi_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PI_8) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_and_er_pi7_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PI7_8) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_and_er_pd_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PD_8) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+6);
}


static void m68k_op_and_er_pd7_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PD7_8) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+6);
}


static void m68k_op_and_er_di_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_DI) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_and_er_ix_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_IX) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_and_er_aw_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_AW) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_and_er_al_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_AL) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+12);
}


static void m68k_op_and_er_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(ea) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_and_er_pcix_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_8(EA_PCIX) | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_and_er_i_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DX &= (m68ki_read_imm_8() | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_and_er_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (DY | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4);
}


static void m68k_op_and_er_ai_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_AI) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_and_er_pi_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_PI_16) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_and_er_pd_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_PD_16) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+6);
}


static void m68k_op_and_er_di_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_DI) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_and_er_ix_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_IX) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_and_er_aw_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_AW) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_and_er_al_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_AL) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+12);
}


static void m68k_op_and_er_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(ea) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+8);
}


static void m68k_op_and_er_pcix_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_16(EA_PCIX) | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+10);
}


static void m68k_op_and_er_i_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DX &= (m68ki_read_imm_16() | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(4+4);
}


static void m68k_op_and_er_d_32(void)
{
	uint res = DX &= DY;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_and_er_ai_32(void)
{
	uint res = DX &= m68ki_read_32(EA_AI);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+8);
}


static void m68k_op_and_er_pi_32(void)
{
	uint res = DX &= m68ki_read_32(EA_PI_32);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+8);
}


static void m68k_op_and_er_pd_32(void)
{
	uint res = DX &= m68ki_read_32(EA_PD_32);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+10);
}


static void m68k_op_and_er_di_32(void)
{
	uint res = DX &= m68ki_read_32(EA_DI);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+12);
}


static void m68k_op_and_er_ix_32(void)
{
	uint res = DX &= m68ki_read_32(EA_IX);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+14);
}


static void m68k_op_and_er_aw_32(void)
{
	uint res = DX &= m68ki_read_32(EA_AW);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+12);
}


static void m68k_op_and_er_al_32(void)
{
	uint res = DX &= m68ki_read_32(EA_AL);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+16);
}


static void m68k_op_and_er_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint res = DX &= m68ki_read_32(ea);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+12);
}


static void m68k_op_and_er_pcix_32(void)
{
	uint res = DX &= m68ki_read_32(EA_PCIX);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+14);
}


static void m68k_op_and_er_i_32(void)
{
	uint res = DX &= m68ki_read_imm_32();

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(6+10);
}


static void m68k_op_and_re_ai_8(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_and_re_pi_8(void)
{
	uint ea  = EA_PI_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_and_re_pi7_8(void)
{
	uint ea  = EA_PI7_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_and_re_pd_8(void)
{
	uint ea  = EA_PD_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_and_re_pd7_8(void)
{
	uint ea  = EA_PD7_8;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_and_re_di_8(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_and_re_ix_8(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_and_re_aw_8(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_and_re_al_8(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_8(DX & m68ki_read_8(ea));

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_and_re_ai_16(void)
{
	uint ea  = EA_AI;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_and_re_pi_16(void)
{
	uint ea  = EA_PI_16;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+4);
}


static void m68k_op_and_re_pd_16(void)
{
	uint ea  = EA_PD_16;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+6);
}


static void m68k_op_and_re_di_16(void)
{
	uint ea  = EA_DI;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_and_re_ix_16(void)
{
	uint ea  = EA_IX;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+10);
}


static void m68k_op_and_re_aw_16(void)
{
	uint ea  = EA_AW;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+8);
}


static void m68k_op_and_re_al_16(void)
{
	uint ea  = EA_AL;
	uint res = MASK_OUT_ABOVE_16(DX & m68ki_read_16(ea));

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8+12);
}


static void m68k_op_and_re_ai_32(void)
{
	uint ea  = EA_AI;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_and_re_pi_32(void)
{
	uint ea  = EA_PI_32;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_and_re_pd_32(void)
{
	uint ea  = EA_PD_32;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_and_re_di_32(void)
{
	uint ea  = EA_DI;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_and_re_ix_32(void)
{
	uint ea  = EA_IX;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+14);
}


static void m68k_op_and_re_aw_32(void)
{
	uint ea  = EA_AW;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_and_re_al_32(void)
{
	uint ea  = EA_AL;
	uint res = DX & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+16);
}


static void m68k_op_andi_d_8(void)
{
	uint res = MASK_OUT_ABOVE_8(DY &= (m68ki_read_imm_8() | 0xffffff00));

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_andi_ai_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_AI;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_andi_pi_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PI_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_andi_pi7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PI7_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_andi_pd_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PD_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_andi_pd7_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_PD7_8;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_andi_di_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_DI;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_andi_ix_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_IX;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_andi_aw_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_AW;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_andi_al_8(void)
{
	uint tmp = m68ki_read_imm_8();
	uint ea  = EA_AL;
	uint res = tmp & m68ki_read_8(ea);

	m68ki_write_8(ea, res);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_andi_d_16(void)
{
	uint res = MASK_OUT_ABOVE_16(DY &= (m68ki_read_imm_16() | 0xffff0000));

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(8);
}


static void m68k_op_andi_ai_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_AI;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_andi_pi_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_PI_16;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+4);
}


static void m68k_op_andi_pd_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_PD_16;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+6);
}


static void m68k_op_andi_di_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_DI;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_andi_ix_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_IX;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+10);
}


static void m68k_op_andi_aw_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_AW;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+8);
}


static void m68k_op_andi_al_16(void)
{
	uint tmp = m68ki_read_imm_16();
	uint ea  = EA_AL;
	uint res = tmp & m68ki_read_16(ea);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(12+12);
}


static void m68k_op_andi_d_32(void)
{
	uint res = DY &= (m68ki_read_imm_32());

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(14);
}


static void m68k_op_andi_ai_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_AI;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+8);
}


static void m68k_op_andi_pi_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_PI_32;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+8);
}


static void m68k_op_andi_pd_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_PD_32;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+10);
}


static void m68k_op_andi_di_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_DI;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+12);
}


static void m68k_op_andi_ix_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_IX;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+14);
}


static void m68k_op_andi_aw_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_AW;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+12);
}


static void m68k_op_andi_al_32(void)
{
	uint tmp = m68ki_read_imm_32();
	uint ea  = EA_AL;
	uint res = tmp & m68ki_read_32(ea);

	m68ki_write_32(ea, res);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_C = FLAG_V = 0;
	USE_CLKS(20+16);
}


static void m68k_op_andi_to_ccr(void)
{
	m68ki_set_ccr(m68ki_get_ccr() & m68ki_read_imm_16());
	USE_CLKS(20);
}


static void m68k_op_andi_to_sr(void)
{
	uint and_val = m68ki_read_imm_16();

	if (FLAG_S)
	{

		m68ki_set_sr(m68ki_get_sr() & and_val);
		USE_CLKS(20);
		return;
	}
	m68ki_exception(EXCEPTION_PRIVILEGE_VIOLATION);
}


static void m68k_op_asr_s_8(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = src >> shift;

	if (GET_MSB_8(src)) res |= m68ki_shift_8_table[shift];

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_X = FLAG_C = shift > 7 ? FLAG_N : (src >> (shift - 1)) & 1;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_asr_s_16(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = src >> shift;

	if (GET_MSB_16(src))	res |= m68ki_shift_16_table[shift];

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_X = FLAG_C = (src >> (shift - 1)) & 1;
	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_asr_s_32(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = src >> shift;

	if (GET_MSB_32(src))	res |= m68ki_shift_32_table[shift];

	*r_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_X = FLAG_C = (src >> (shift - 1)) & 1;
	USE_CLKS((shift << 1) + 8);
}


static void m68k_op_asr_r_8(void)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = src >> shift;

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 8)
		{
			if (GET_MSB_8(src)) 	res |= m68ki_shift_8_table[shift];

			*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

			FLAG_C = FLAG_X = (src >> (shift - 1)) & 1;
			FLAG_N = GET_MSB_8(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		if (GET_MSB_8(src))
		{
			*r_dst |= 0xff;
			FLAG_C = FLAG_X = 1;
			FLAG_N = 1;
			FLAG_Z = 1;
			FLAG_V = 0;
			return;
		}

		*r_dst &= 0xffffff00;
		FLAG_C = FLAG_X = 0;
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


static void m68k_op_asr_r_16(void)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = src >> shift;

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 16)
		{
			if (GET_MSB_16(src))	res |= m68ki_shift_16_table[shift];

			*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

			FLAG_C = FLAG_X = (src >> (shift - 1)) & 1;
			FLAG_N = GET_MSB_16(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		if (GET_MSB_16(src))
		{
			*r_dst |= 0xffff;
			FLAG_C = FLAG_X = 1;
			FLAG_N = 1;
			FLAG_Z = 1;
			FLAG_V = 0;
			return;
		}

		*r_dst &= 0xffff0000;
		FLAG_C = FLAG_X = 0;
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


static void m68k_op_asr_r_32(void)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_32(*r_dst);
	uint res = src >> shift;

	USE_CLKS((shift << 1) + 8);
	if (shift != 0)
	{
		if (shift < 32)
		{
			if (GET_MSB_32(src))	res |= m68ki_shift_32_table[shift];

			*r_dst = res;

			FLAG_C = FLAG_X = (src >> (shift - 1)) & 1;
			FLAG_N = GET_MSB_32(res);
			FLAG_Z = res;
			FLAG_V = 0;
			return;
		}

		if (GET_MSB_32(src))
		{
			*r_dst = 0xffffffff;
			FLAG_C = FLAG_X = 1;
			FLAG_N = 1;
			FLAG_Z = 1;
			FLAG_V = 0;
			return;
		}

		*r_dst = 0;
		FLAG_C = FLAG_X = 0;
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


static void m68k_op_asr_ea_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_C = FLAG_X = src & 1;
	USE_CLKS(8+4);
}


static void m68k_op_asr_ea_pi(void)
{
	uint ea  = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_C = FLAG_X = src & 1;
	USE_CLKS(8+4);
}


static void m68k_op_asr_ea_pd(void)
{
	uint ea  = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_C = FLAG_X = src & 1;
	USE_CLKS(8+6);
}


static void m68k_op_asr_ea_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_C = FLAG_X = src & 1;
	USE_CLKS(8+8);
}


static void m68k_op_asr_ea_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_C = FLAG_X = src & 1;
	USE_CLKS(8+10);
}


static void m68k_op_asr_ea_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_C = FLAG_X = src & 1;
	USE_CLKS(8+8);
}


static void m68k_op_asr_ea_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = src >> 1;

	if (GET_MSB_16(src))
		res |= 0x8000;

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
	FLAG_C = FLAG_X = src & 1;
	USE_CLKS(8+12);
}


static void m68k_op_asl_s_8(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);
int iii;

	*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;

	FLAG_X = FLAG_C = (src >> (8 - shift)) & 1;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;

	iii = m68ki_shift_8_table[shift + 1];
	src &= iii;
	FLAG_V = !(src == 0 || ((src == iii) && (shift < 8) ));

	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_asl_s_16(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);
	int iii;

	*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = (src >> (16 - shift)) & 1;

	iii=m68ki_shift_16_table[shift + 1];
	src &= iii;
	FLAG_V = !(src == 0 || src == iii);

	USE_CLKS((shift << 1) + 6);
}


static void m68k_op_asl_s_32(void)
{
	uint* r_dst = &DY;
	uint shift = (((REG_IR >> 9) - 1) & 7) + 1;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);
int iii;
	*r_dst = res;

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = (src >> (32 - shift)) & 1;

	iii = m68ki_shift_32_table[shift + 1];
	src &= iii;
	FLAG_V = !(src == 0 || src == iii);

	USE_CLKS((shift << 1) + 8);
}


static void m68k_op_asl_r_8(void)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_8(*r_dst);
	uint res = MASK_OUT_ABOVE_8(src << shift);

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 8)
		{
		int iii;
			*r_dst = MASK_OUT_BELOW_8(*r_dst) | res;
			FLAG_X = FLAG_C = (src >> (8 - shift)) & 1;
			FLAG_N = GET_MSB_8(res);
			FLAG_Z = res;

			iii = m68ki_shift_8_table[shift + 1];
			src &= iii;
			FLAG_V = !(src == 0 || src == iii);
			return;
		}

		*r_dst &= 0xffffff00;
		FLAG_X = FLAG_C = (shift == 8 ? src & 1 : 0);
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = !(src == 0);
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_asl_r_16(void)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = MASK_OUT_ABOVE_16(*r_dst);
	uint res = MASK_OUT_ABOVE_16(src << shift);

	USE_CLKS((shift << 1) + 6);
	if (shift != 0)
	{
		if (shift < 16)
		{
		int iii;
			*r_dst = MASK_OUT_BELOW_16(*r_dst) | res;
			FLAG_X = FLAG_C = (src >> (16 - shift)) & 1;
			FLAG_N = GET_MSB_16(res);
			FLAG_Z = res;

			iii =m68ki_shift_16_table[shift + 1];
			src &= iii;
			FLAG_V = !(src == 0 || src == iii);
			return;
		}

		*r_dst &= 0xffff0000;
		FLAG_X = FLAG_C = (shift == 16 ? src & 1 : 0);
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = !(src == 0);
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = 0;
}

static void m68k_op_asl_r_32(void)
{
	uint* r_dst = &DY;
	uint shift = DX & 0x3f;
	uint src = *r_dst;
	uint res = MASK_OUT_ABOVE_32(src << shift);

	USE_CLKS((shift << 1) + 8);
	if (shift != 0)
	{
		if (shift < 32)
		{
		int iii;
			*r_dst = res;
			FLAG_X = FLAG_C = (src >> (32 - shift)) & 1;
			FLAG_N = GET_MSB_32(res);
			FLAG_Z = res;

			iii = m68ki_shift_32_table[shift + 1];
			src &= iii;
			FLAG_V = !(src == 0 || src == iii);
			return;
		}

		*r_dst = 0;
		FLAG_X = FLAG_C = (shift == 32 ? src & 1 : 0);
		FLAG_N = 0;
		FLAG_Z = 0;
		FLAG_V = !(src == 0);
		return;
	}

	FLAG_C = 0;
	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = 0;
}


static void m68k_op_asl_ea_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	src &= 0xc000;
	FLAG_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+4);
}


static void m68k_op_asl_ea_pi(void)
{
	uint ea  = EA_PI_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	src &= 0xc000;
	FLAG_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+4);
}


static void m68k_op_asl_ea_pd(void)
{
	uint ea  = EA_PD_16;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	src &= 0xc000;
	FLAG_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+6);
}


static void m68k_op_asl_ea_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	src &= 0xc000;
	FLAG_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+8);
}


static void m68k_op_asl_ea_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	src &= 0xc000;
	FLAG_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+10);
}


static void m68k_op_asl_ea_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	src &= 0xc000;
	FLAG_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+8);
}


static void m68k_op_asl_ea_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_16(ea);
	uint res = MASK_OUT_ABOVE_16(src << 1);

	m68ki_write_16(ea, res);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_X = FLAG_C = GET_MSB_16(src);
	src &= 0xc000;
	FLAG_V = !(src == 0 || src == 0xc000);
	USE_CLKS(8+12);
}


static void m68k_op_bhi_8(void)
{
	if (CONDITION_HI)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bhi_16(void)
{
	if (CONDITION_HI)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}


static void m68k_op_bls_8(void)
{
	if (CONDITION_LS)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bls_16(void)
{
	if (CONDITION_LS)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bcc_8(void)
{
	if (CONDITION_CC)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bcc_16(void)
{
	if (CONDITION_CC)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bcs_8(void)
{
	if (CONDITION_CS)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bcs_16(void)
{
	if (CONDITION_CS)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bne_8(void)
{
	if (CONDITION_NE)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bne_16(void)
{
	if (CONDITION_NE)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_beq_8(void)
{
	if (CONDITION_EQ)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_beq_16(void)
{
	if (CONDITION_EQ)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bvc_8(void)
{
	if (CONDITION_VC)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bvc_16(void)
{
	if (CONDITION_VC)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bvs_8(void)
{
	if (CONDITION_VS)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bvs_16(void)
{
	if (CONDITION_VS)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bpl_8(void)
{
	if (CONDITION_PL)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bpl_16(void)
{
	if (CONDITION_PL)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bmi_8(void)
{
	if (CONDITION_MI)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bmi_16(void)
{
	if (CONDITION_MI)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bge_8(void)
{
	if (CONDITION_GE)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bge_16(void)
{
	if (CONDITION_GE)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_blt_8(void)
{
	if (CONDITION_LT)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_blt_16(void)
{
	if (CONDITION_LT)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_bgt_8(void)
{
	if (CONDITION_GT)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_bgt_16(void)
{
	if (CONDITION_GT)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}

static void m68k_op_ble_8(void)
{
	if (CONDITION_LE)
	{
		m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
		USE_CLKS(10);
		return;
	}
	USE_CLKS(8);
}

static void m68k_op_ble_16(void)
{
	if (CONDITION_LE)
	{
		m68ki_branch_word(m68ki_read_16(REG_PC));
		USE_CLKS(10);
		return;
	}
	REG_PC += 2;
	USE_CLKS(12);
}




static void m68k_op_bchg_r_d(void)
{
	uint* r_dst = &DY;
	uint mask = 1 << (DX & 0x1f);

	FLAG_Z = *r_dst & mask;
	*r_dst ^= mask;
	USE_CLKS(8);
}


static void m68k_op_bchg_r_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+4);
}


static void m68k_op_bchg_r_pi(void)
{
	uint ea  = EA_PI_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+4);
}


static void m68k_op_bchg_r_pi7(void)
{
	uint ea  = EA_PI7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+4);
}


static void m68k_op_bchg_r_pd(void)
{
	uint ea  = EA_PD_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+6);
}


static void m68k_op_bchg_r_pd7(void)
{
	uint ea  = EA_PD7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+6);
}


static void m68k_op_bchg_r_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+8);
}


static void m68k_op_bchg_r_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+10);
}


static void m68k_op_bchg_r_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+8);
}


static void m68k_op_bchg_r_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(8+12);
}


static void m68k_op_bchg_s_d(void)
{
	uint* r_dst = &DY;
	uint mask = 1 << (m68ki_read_imm_8() & 0x1f);

	FLAG_Z = *r_dst & mask;
	*r_dst ^= mask;
	USE_CLKS(12);
}


static void m68k_op_bchg_s_ai(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AI;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+4);
}


static void m68k_op_bchg_s_pi(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PI_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+4);
}


static void m68k_op_bchg_s_pi7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PI7_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+4);
}


static void m68k_op_bchg_s_pd(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PD_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+6);
}


static void m68k_op_bchg_s_pd7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PD7_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+6);
}


static void m68k_op_bchg_s_di(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_DI;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+8);
}


static void m68k_op_bchg_s_ix(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_IX;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+10);
}


static void m68k_op_bchg_s_aw(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AW;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+8);
}


static void m68k_op_bchg_s_al(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AL;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src ^ mask);
	USE_CLKS(12+12);
}


static void m68k_op_bclr_r_d(void)
{
	uint* r_dst = &DY;
	uint mask = 1 << (DX & 0x1f);

	FLAG_Z = *r_dst & mask;
	*r_dst &= ~mask;
	USE_CLKS(10);
}


static void m68k_op_bclr_r_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+4);
}


static void m68k_op_bclr_r_pi(void)
{
	uint ea  = EA_PI_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+4);
}


static void m68k_op_bclr_r_pi7(void)
{
	uint ea  = EA_PI7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+4);
}


static void m68k_op_bclr_r_pd(void)
{
	uint ea  = EA_PD_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+6);
}


static void m68k_op_bclr_r_pd7(void)
{
	uint ea  = EA_PD7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+6);
}


static void m68k_op_bclr_r_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+8);
}


static void m68k_op_bclr_r_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+10);
}


static void m68k_op_bclr_r_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+8);
}


static void m68k_op_bclr_r_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(8+12);
}


static void m68k_op_bclr_s_d(void)
{
	uint* r_dst = &DY;
	uint mask = 1 << (m68ki_read_imm_8() & 0x1f);

	FLAG_Z = *r_dst & mask;
	*r_dst &= ~mask;
	USE_CLKS(14);
}


static void m68k_op_bclr_s_ai(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AI;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+4);
}


static void m68k_op_bclr_s_pi(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PI_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+4);
}


static void m68k_op_bclr_s_pi7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PI7_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+4);
}


static void m68k_op_bclr_s_pd(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PD_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+6);
}


static void m68k_op_bclr_s_pd7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PD7_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+6);
}


static void m68k_op_bclr_s_di(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_DI;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+8);
}


static void m68k_op_bclr_s_ix(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_IX;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+10);
}


static void m68k_op_bclr_s_aw(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AW;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+8);
}


static void m68k_op_bclr_s_al(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AL;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src & ~mask);
	USE_CLKS(12+12);
}


static void m68k_op_bra_8(void)
{
	m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
	USE_CLKS(10);
}

static void m68k_op_bra_16(void)
{
	m68ki_branch_word(m68ki_read_16(REG_PC));
	USE_CLKS(10);
}



static void m68k_op_bset_r_d(void)
{
	uint* r_dst = &DY;
	uint mask = 1 << (DX & 0x1f);

	FLAG_Z = *r_dst & mask;
	*r_dst |= mask;
	USE_CLKS(8);
}


static void m68k_op_bset_r_ai(void)
{
	uint ea  = EA_AI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+4);
}


static void m68k_op_bset_r_pi(void)
{
	uint ea  = EA_PI_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+4);
}


static void m68k_op_bset_r_pi7(void)
{
	uint ea  = EA_PI7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+4);
}


static void m68k_op_bset_r_pd(void)
{
	uint ea  = EA_PD_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+6);
}


static void m68k_op_bset_r_pd7(void)
{
	uint ea  = EA_PD7_8;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+6);
}


static void m68k_op_bset_r_di(void)
{
	uint ea  = EA_DI;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+8);
}


static void m68k_op_bset_r_ix(void)
{
	uint ea  = EA_IX;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+10);
}


static void m68k_op_bset_r_aw(void)
{
	uint ea  = EA_AW;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+8);
}


static void m68k_op_bset_r_al(void)
{
	uint ea  = EA_AL;
	uint src = m68ki_read_8(ea);
	uint mask = 1 << (DX & 7);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(8+12);
}


static void m68k_op_bset_s_d(void)
{
	uint* r_dst = &DY;
	uint mask = 1 << (m68ki_read_imm_8() & 0x1f);

	FLAG_Z = *r_dst & mask;
	*r_dst |= mask;
	USE_CLKS(12);
}


static void m68k_op_bset_s_ai(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AI;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+4);
}


static void m68k_op_bset_s_pi(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PI_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+4);
}


static void m68k_op_bset_s_pi7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PI7_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+4);
}


static void m68k_op_bset_s_pd(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PD_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+6);
}


static void m68k_op_bset_s_pd7(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_PD7_8;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+6);
}


static void m68k_op_bset_s_di(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_DI;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+8);
}


static void m68k_op_bset_s_ix(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_IX;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+10);
}


static void m68k_op_bset_s_aw(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AW;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+8);
}


static void m68k_op_bset_s_al(void)
{
	uint mask = 1 << (m68ki_read_imm_8() & 7);
	uint ea  = EA_AL;
	uint src = m68ki_read_8(ea);

	FLAG_Z = src & mask;
	m68ki_write_8(ea, src | mask);
	USE_CLKS(12+12);
}


static void m68k_op_bsr_8(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(REG_PC);
	m68ki_branch_byte(MASK_OUT_ABOVE_8(REG_IR));
	USE_CLKS(18);
}


static void m68k_op_bsr_16(void)
{
	m68ki_add_trace();				   /* auto-disable (see m68kcpu.h) */
	m68ki_push_32(REG_PC + 2);
	m68ki_branch_word(m68ki_read_16(REG_PC));
	USE_CLKS(18);
}



static void m68k_op_btst_r_d(void)
{
	FLAG_Z = DY & (1 << (DX & 0x1f));
	USE_CLKS(6);
}


static void m68k_op_btst_r_ai(void)
{
	FLAG_Z = m68ki_read_8(EA_AI) & (1 << (DX & 7));
	USE_CLKS(4+4);
}


static void m68k_op_btst_r_pi(void)
{
	FLAG_Z = m68ki_read_8(EA_PI_8) & (1 << (DX & 7));
	USE_CLKS(4+4);
}


static void m68k_op_btst_r_pi7(void)
{
	FLAG_Z = m68ki_read_8(EA_PI7_8) & (1 << (DX & 7));
	USE_CLKS(4+4);
}


static void m68k_op_btst_r_pd(void)
{
	FLAG_Z = m68ki_read_8(EA_PD_8) & (1 << (DX & 7));
	USE_CLKS(4+6);
}


static void m68k_op_btst_r_pd7(void)
{
	FLAG_Z = m68ki_read_8(EA_PD7_8) & (1 << (DX & 7));
	USE_CLKS(4+6);
}


static void m68k_op_btst_r_di(void)
{
	FLAG_Z = m68ki_read_8(EA_DI) & (1 << (DX & 7));
	USE_CLKS(4+8);
}


static void m68k_op_btst_r_ix(void)
{
	FLAG_Z = m68ki_read_8(EA_IX) & (1 << (DX & 7));
	USE_CLKS(4+10);
}


static void m68k_op_btst_r_aw(void)
{
	FLAG_Z = m68ki_read_8(EA_AW) & (1 << (DX & 7));
	USE_CLKS(4+8);
}


static void m68k_op_btst_r_al(void)
{
	FLAG_Z = m68ki_read_8(EA_AL) & (1 << (DX & 7));
	USE_CLKS(4+12);
}


static void m68k_op_btst_r_pcdi(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	FLAG_Z = m68ki_read_8(ea) & (1 << (DX & 7));
	USE_CLKS(4+8);
}


static void m68k_op_btst_r_pcix(void)
{
	FLAG_Z = m68ki_read_8(EA_PCIX) & (1 << (DX & 7));
	USE_CLKS(4+10);
}


static void m68k_op_btst_r_i(void)
{
	FLAG_Z = m68ki_read_imm_8() & (1 << (DX & 7));
	USE_CLKS(4+4);
}


static void m68k_op_btst_s_d(void)
{
	FLAG_Z = DY & (1 << (m68ki_read_imm_8() & 0x1f));
	USE_CLKS(10);
}


static void m68k_op_btst_s_ai(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_AI) & (1 << bit);
	USE_CLKS(8+4);
}


static void m68k_op_btst_s_pi(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_PI_8) & (1 << bit);
	USE_CLKS(8+4);
}


static void m68k_op_btst_s_pi7(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_PI7_8) & (1 << bit);
	USE_CLKS(8+4);
}


static void m68k_op_btst_s_pd(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_PD_8) & (1 << bit);
	USE_CLKS(8+6);
}


static void m68k_op_btst_s_pd7(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_PD7_8) & (1 << bit);
	USE_CLKS(8+6);
}


static void m68k_op_btst_s_di(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_DI) & (1 << bit);
	USE_CLKS(8+8);
}


static void m68k_op_btst_s_ix(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_IX) & (1 << bit);
	USE_CLKS(8+10);
}


static void m68k_op_btst_s_aw(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_AW) & (1 << bit);
	USE_CLKS(8+8);
}


static void m68k_op_btst_s_al(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_AL) & (1 << bit);
	USE_CLKS(8+12);
}


static void m68k_op_btst_s_pcdi(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	FLAG_Z = m68ki_read_8(ea) & (1 << bit);
	USE_CLKS(8+8);
}


static void m68k_op_btst_s_pcix(void)
{
	uint bit = m68ki_read_imm_8() & 7;

	FLAG_Z = m68ki_read_8(EA_PCIX) & (1 << bit);
	USE_CLKS(8+10);
}





static void m68k_op_chk_d_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(DY);

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_ai_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_AI));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+4);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_pi_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_PI_16));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+4);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_pd_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_PD_16));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+6);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_di_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_DI));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+8);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_ix_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_IX));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+10);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_aw_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_AW));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+8);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_al_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_AL));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+12);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_pcdi_16(void)
{
	int src = MAKE_INT_16(DX);
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	int bound = MAKE_INT_16(m68ki_read_16(ea));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+8);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_pcix_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_16(EA_PCIX));

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+10);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}


static void m68k_op_chk_i_16(void)
{
	int src = MAKE_INT_16(DX);
	int bound = MAKE_INT_16(m68ki_read_imm_16());

	FLAG_Z = ZFLAG_16(src); /* Undocumented */
	FLAG_V = 0; 			/* Undocumented */
	FLAG_C = 0; 			/* Undocumented */

	if (src >= 0 && src <= bound)
	{
		USE_CLKS(10+4);
		return;
	}
	FLAG_N = src < 0;
	m68ki_interrupt(EXCEPTION_CHK);
}






static void m68k_op_clr_d_8(void)
{
	DY &= 0xffffff00;

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(4);
}


static void m68k_op_clr_ai_8(void)
{
	m68ki_write_8(EA_AI, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+4);
}


static void m68k_op_clr_pi_8(void)
{
	m68ki_write_8(EA_PI_8, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+4);
}


static void m68k_op_clr_pi7_8(void)
{
	m68ki_write_8(EA_PI7_8, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+4);
}


static void m68k_op_clr_pd_8(void)
{
	m68ki_write_8(EA_PD_8, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+6);
}


static void m68k_op_clr_pd7_8(void)
{
	m68ki_write_8(EA_PD7_8, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+6);
}


static void m68k_op_clr_di_8(void)
{
	m68ki_write_8(EA_DI, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+8);
}


static void m68k_op_clr_ix_8(void)
{
	m68ki_write_8(EA_IX, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+10);
}


static void m68k_op_clr_aw_8(void)
{
	m68ki_write_8(EA_AW, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+8);
}


static void m68k_op_clr_al_8(void)
{
	m68ki_write_8(EA_AL, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+12);
}


static void m68k_op_clr_d_16(void)
{
	DY &= 0xffff0000;

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(4);
}


static void m68k_op_clr_ai_16(void)
{
	m68ki_write_16(EA_AI, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+4);
}


static void m68k_op_clr_pi_16(void)
{
	m68ki_write_16(EA_PI_16, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+4);
}


static void m68k_op_clr_pd_16(void)
{
	m68ki_write_16(EA_PD_16, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+6);
}


static void m68k_op_clr_di_16(void)
{
	m68ki_write_16(EA_DI, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+8);
}


static void m68k_op_clr_ix_16(void)
{
	m68ki_write_16(EA_IX, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+10);
}


static void m68k_op_clr_aw_16(void)
{
	m68ki_write_16(EA_AW, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+8);
}


static void m68k_op_clr_al_16(void)
{
	m68ki_write_16(EA_AL, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(8+12);
}


static void m68k_op_clr_d_32(void)
{
	DY = 0;

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(6);
}


static void m68k_op_clr_ai_32(void)
{
	m68ki_write_32(EA_AI, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(12+8);
}


static void m68k_op_clr_pi_32(void)
{
	m68ki_write_32(EA_PI_32, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(12+8);
}


static void m68k_op_clr_pd_32(void)
{
	m68ki_write_32(EA_PD_32, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(12+10);
}


static void m68k_op_clr_di_32(void)
{
	m68ki_write_32(EA_DI, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(12+12);
}


static void m68k_op_clr_ix_32(void)
{
	m68ki_write_32(EA_IX, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(12+14);
}


static void m68k_op_clr_aw_32(void)
{
	m68ki_write_32(EA_AW, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(12+12);
}


static void m68k_op_clr_al_32(void)
{
	m68ki_write_32(EA_AL, 0);

	FLAG_N = FLAG_V = FLAG_C = 0;
	FLAG_Z = 0;
	USE_CLKS(12+16);
}


static void m68k_op_cmp_d_8(void)
{
	uint src = DY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_cmp_ai_8(void)
{
	uint src = m68ki_read_8(EA_AI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_cmp_pi_8(void)
{
	uint src = m68ki_read_8(EA_PI_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_cmp_pi7_8(void)
{
	uint src = m68ki_read_8(EA_PI7_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_cmp_pd_8(void)
{
	uint src = m68ki_read_8(EA_PD_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_cmp_pd7_8(void)
{
	uint src = m68ki_read_8(EA_PD7_8);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_cmp_di_8(void)
{
	uint src = m68ki_read_8(EA_DI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_cmp_ix_8(void)
{
	uint src = m68ki_read_8(EA_IX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_cmp_aw_8(void)
{
	uint src = m68ki_read_8(EA_AW);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_cmp_al_8(void)
{
	uint src = m68ki_read_8(EA_AL);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+12);
}


static void m68k_op_cmp_pcdi_8(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_8(ea);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_cmp_pcix_8(void)
{
	uint src = m68ki_read_8(EA_PCIX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_cmp_i_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_cmp_d_16(void)
{
	uint src = DY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_cmp_a_16(void)
{
	uint src = AY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4);
}


static void m68k_op_cmp_ai_16(void)
{
	uint src = m68ki_read_16(EA_AI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_cmp_pi_16(void)
{
	uint src = m68ki_read_16(EA_PI_16);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_cmp_pd_16(void)
{
	uint src = m68ki_read_16(EA_PD_16);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+6);
}


static void m68k_op_cmp_di_16(void)
{
	uint src = m68ki_read_16(EA_DI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_cmp_ix_16(void)
{
	uint src = m68ki_read_16(EA_IX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_cmp_aw_16(void)
{
	uint src = m68ki_read_16(EA_AW);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_cmp_al_16(void)
{
	uint src = m68ki_read_16(EA_AL);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+12);
}


static void m68k_op_cmp_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_16(ea);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+8);
}


static void m68k_op_cmp_pcix_16(void)
{
	uint src = m68ki_read_16(EA_PCIX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+10);
}


static void m68k_op_cmp_i_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(4+4);
}


static void m68k_op_cmp_d_32(void)
{
	uint src = DY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


static void m68k_op_cmp_a_32(void)
{
	uint src = AY;
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


static void m68k_op_cmp_ai_32(void)
{
	uint src = m68ki_read_32(EA_AI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmp_pi_32(void)
{
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmp_pd_32(void)
{
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_cmp_di_32(void)
{
	uint src = m68ki_read_32(EA_DI);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_cmp_ix_32(void)
{
	uint src = m68ki_read_32(EA_IX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_cmp_aw_32(void)
{
	uint src = m68ki_read_32(EA_AW);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_cmp_al_32(void)
{
	uint src = m68ki_read_32(EA_AL);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+16);
}


static void m68k_op_cmp_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_cmp_pcix_32(void)
{
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_cmp_i_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = DX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmpa_d_16(void)
{
	uint src = MAKE_INT_16(DY);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


static void m68k_op_cmpa_a_16(void)
{
	uint src = MAKE_INT_16(AY);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


static void m68k_op_cmpa_ai_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_AI));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+4);
}


static void m68k_op_cmpa_pi_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_PI_16));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+4);
}


static void m68k_op_cmpa_pd_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_PD_16));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+6);
}


static void m68k_op_cmpa_di_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_DI));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmpa_ix_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_IX));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_cmpa_aw_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_AW));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmpa_al_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_AL));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_cmpa_pcdi_16(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = MAKE_INT_16(m68ki_read_16(ea));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmpa_pcix_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_16(EA_PCIX));
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_cmpa_i_16(void)
{
	uint src = MAKE_INT_16(m68ki_read_imm_16());
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+4);
}


static void m68k_op_cmpa_d_32(void)
{
	uint src = DY;
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


static void m68k_op_cmpa_a_32(void)
{
	uint src = AY;
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6);
}


static void m68k_op_cmpa_ai_32(void)
{
	uint src = m68ki_read_32(EA_AI);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmpa_pi_32(void)
{
	uint src = m68ki_read_32(EA_PI_32);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmpa_pd_32(void)
{
	uint src = m68ki_read_32(EA_PD_32);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+10);
}


static void m68k_op_cmpa_di_32(void)
{
	uint src = m68ki_read_32(EA_DI);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_cmpa_ix_32(void)
{
	uint src = m68ki_read_32(EA_IX);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_cmpa_aw_32(void)
{
	uint src = m68ki_read_32(EA_AW);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_cmpa_al_32(void)
{
	uint src = m68ki_read_32(EA_AL);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+16);
}


static void m68k_op_cmpa_pcdi_32(void)
{
	uint old_pc = (REG_PC+=2) - 2;
	uint ea  = old_pc + MAKE_INT_16(m68ki_read_16(old_pc));
	uint src = m68ki_read_32(ea);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+12);
}


static void m68k_op_cmpa_pcix_32(void)
{
	uint src = m68ki_read_32(EA_PCIX);
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+14);
}


static void m68k_op_cmpa_i_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = AX;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(6+8);
}


static void m68k_op_cmpi_d_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = DY;
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_cmpi_ai_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_AI);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_cmpi_pi_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PI_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_cmpi_pi7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PI7_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_cmpi_pd_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PD_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_cmpi_pd7_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_PD7_8);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_cmpi_di_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_DI);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_cmpi_ix_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_IX);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_cmpi_aw_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_AW);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_cmpi_al_8(void)
{
	uint src = m68ki_read_imm_8();
	uint dst = m68ki_read_8(EA_AL);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(8+12);
}



static void m68k_op_cmpi_d_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = DY;
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8);
}


static void m68k_op_cmpi_ai_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_AI);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_cmpi_pi_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_PI_16);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+4);
}


static void m68k_op_cmpi_pd_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_PD_16);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+6);
}


static void m68k_op_cmpi_di_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_DI);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_cmpi_ix_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_IX);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+10);
}


static void m68k_op_cmpi_aw_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_AW);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+8);
}


static void m68k_op_cmpi_al_16(void)
{
	uint src = m68ki_read_imm_16();
	uint dst = m68ki_read_16(EA_AL);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(8+12);
}



static void m68k_op_cmpi_d_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = DY;
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(14);
}


static void m68k_op_cmpi_ai_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_AI);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_cmpi_pi_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_PI_32);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+8);
}


static void m68k_op_cmpi_pd_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_PD_32);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+10);
}


static void m68k_op_cmpi_di_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_DI);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_cmpi_ix_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_IX);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+14);
}


static void m68k_op_cmpi_aw_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_AW);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+12);
}


static void m68k_op_cmpi_al_32(void)
{
	uint src = m68ki_read_imm_32();
	uint dst = m68ki_read_32(EA_AL);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(12+16);
}



static void m68k_op_cmpm_8_ax7(void)
{
	uint src = m68ki_read_8(AY++);
	uint dst = m68ki_read_8((REG_A[7] += 2) - 2);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


static void m68k_op_cmpm_8_ay7(void)
{
	uint src = m68ki_read_8((REG_A[7] += 2) - 2);
	uint dst = m68ki_read_8(AX++);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


static void m68k_op_cmpm_8_axy7(void)
{
	uint src = m68ki_read_8((REG_A[7] += 2) - 2);
	uint dst = m68ki_read_8((REG_A[7] += 2) - 2);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


static void m68k_op_cmpm_8(void)
{
	uint src = m68ki_read_8(AY++);
	uint dst = m68ki_read_8(AX++);
	uint res = MASK_OUT_ABOVE_8(dst - src);

	FLAG_N = GET_MSB_8(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_8(src, dst, res);
	FLAG_C = CFLAG_SUB_8(src, dst, res);
	USE_CLKS(12);
}


static void m68k_op_cmpm_16(void)
{
	uint src = m68ki_read_16((AY += 2) - 2);
	uint dst = m68ki_read_16((AX += 2) - 2);
	uint res = MASK_OUT_ABOVE_16(dst - src);

	FLAG_N = GET_MSB_16(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_16(src, dst, res);
	FLAG_C = CFLAG_SUB_16(src, dst, res);
	USE_CLKS(12);
}


static void m68k_op_cmpm_32(void)
{
	uint src = m68ki_read_32((AY += 4) - 4);
	uint dst = m68ki_read_32((AX += 4) - 4);
	uint res = MASK_OUT_ABOVE_32(dst - src);

	FLAG_N = GET_MSB_32(res);
	FLAG_Z = res;
	FLAG_V = VFLAG_SUB_32(src, dst, res);
	FLAG_C = CFLAG_SUB_32(src, dst, res);
	USE_CLKS(20);
}


