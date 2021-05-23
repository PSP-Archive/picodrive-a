#ifndef M68K_MPU__HEADER
#define M68K_MPU__HEADER


#include "m68k.h"
#include <limits.h>

/* ======================================================================== */
/* ==================== ARCHITECTURE-DEPENDANT DEFINES ==================== */
/* ======================================================================== */

/* Check if we have certain storage sizes */

#if UCHAR_MAX == 0xff
#define M68K_HAS_8_BIT_SIZE  1
#else
#define M68K_HAS_8_BIT_SIZE  0
#endif /* UCHAR_MAX == 0xff */

#if USHRT_MAX == 0xffff
#define M68K_HAS_16_BIT_SIZE 1
#else
#define M68K_HAS_16_BIT_SIZE 0
#endif /* USHRT_MAX == 0xffff */

#if ULONG_MAX == 0xffffffff
#define M68K_HAS_32_BIT_SIZE 1
#else
#define M68K_HAS_32_BIT_SIZE 0
#endif /* ULONG_MAX == 0xffffffff */

#if UINT_MAX > 0xffffffff
#define M68K_OVER_32_BIT	 1
#else
#define M68K_OVER_32_BIT	 0
#endif /* UINT_MAX > 0xffffffff */

/* Data types used in this emulation core */
#undef int8
#undef int16
#undef int32
#undef uint
#undef uint8
#undef uint16
#undef uint

#define int8   signed char			/* ASG: changed from char to signed char */
#define uint8  unsigned char
#define int16  short
#define uint16 unsigned short
#define int32  long

/* int and unsigned int must be at least 32 bits wide */
#define uint   unsigned int


/* Allow for architectures that don't have 8-bit sizes */
#if M68K_HAS_8_BIT_SIZE
#define MAKE_INT_8(A) (int8)((A)&0xff)
#else
#undef	int8
#define int8   int
#undef	uint8
#define uint8  unsigned int
INLINE int MAKE_INT_8(int value)
{
   /* Will this work for 1's complement machines? */
   return (value & 0x80) ? value | ~0xff : value & 0xff;
}
#endif /* M68K_HAS_8_BIT_SIZE */


/* Allow for architectures that don't have 16-bit sizes */
#if M68K_HAS_16_BIT_SIZE
#define MAKE_INT_16(A) (int16)((A)&0xffff)
#else
#undef	int16
#define int16  int
#undef	uint16
#define uint16 unsigned int
INLINE int MAKE_INT_16(int value)
{
   /* Will this work for 1's complement machines? */
   return (value & 0x8000) ? value | ~0xffff : value & 0xffff;
}
#endif /* M68K_HAS_16_BIT_SIZE */


/* Allow for architectures that don't have 32-bit sizes */
#if M68K_HAS_32_BIT_SIZE
#if M68K_OVER_32_BIT
#define MAKE_INT_32(A) (int32)((A)&0xffffffff)
#else
#define MAKE_INT_32(A) (int32)(A)
#endif /* M68K_OVER_32_BIT */
#else
#undef	int32
#define int32  int
INLINE int MAKE_INT_32(int value)
{
   /* Will this work for 1's complement machines? */
   return (value & 0x80000000) ? value | ~0xffffffff : value & 0xffffffff;
}
#endif /* M68K_HAS_32_BIT_SIZE */



/* ======================================================================== */
/* ============================ GENERAL DEFINES =========================== */
/* ======================================================================== */

/* Exception Vectors handled by emulation */
#define EXCEPTION_ILLEGAL_INSTRUCTION	   4
#define EXCEPTION_ZERO_DIVIDE			   5
#define EXCEPTION_CHK					   6
#define EXCEPTION_TRAPV 				   7
#define EXCEPTION_PRIVILEGE_VIOLATION	   8
#define EXCEPTION_TRACE 				   9
#define EXCEPTION_1010					  10
#define EXCEPTION_1111					  11
#define EXCEPTION_FORMAT_ERROR			  14
#define EXCEPTION_UNINITIALIZED_INTERRUPT 15
#define EXCEPTION_SPURIOUS_INTERRUPT	  24
#define EXCEPTION_INTERRUPT_AUTOVECTOR	  24
#define EXCEPTION_TRAP_BASE 			  32

/* Function codes set by MPU during data/address bus activity */
#define FUNCTION_CODE_USER_DATA 		 1
#define FUNCTION_CODE_USER_PROGRAM		 2
#define FUNCTION_CODE_SUPERVISOR_DATA	 5
#define FUNCTION_CODE_SUPERVISOR_PROGRAM 6
#define FUNCTION_CODE_REG_SP_BASEACE	 7

/* MPU modes for deciding what to emulate */
#define FLAG_MODE_000  M68K_FLAG_MODE_68000
#define FLAG_MODE_010  M68K_FLAG_MODE_68010
#define FLAG_MODE_020  M68K_FLAG_MODE_68020

#define FLAG_MODE_ALL		(FLAG_MODE_000 | FLAG_MODE_010 | FLAG_MODE_020)
#define FLAG_MODE_010_PLUS	(FLAG_MODE_010 | FLAG_MODE_020)
#define FLAG_MODE_010_LESS	(FLAG_MODE_000 | FLAG_MODE_010)
#define FLAG_MODE_020_PLUS	FLAG_MODE_020
#define FLAG_MODE_020_LESS	(FLAG_MODE_000 | FLAG_MODE_010 | FLAG_MODE_020)


/* ======================================================================== */
/* ================================ MACROS ================================ */
/* ======================================================================== */

/* Bit Isolation Macros */
#define BIT_0(A)  ((A) & 0x00000001)
#define BIT_1(A)  ((A) & 0x00000002)
#define BIT_2(A)  ((A) & 0x00000004)
#define BIT_3(A)  ((A) & 0x00000008)
#define BIT_4(A)  ((A) & 0x00000010)
#define BIT_5(A)  ((A) & 0x00000020)
#define BIT_6(A)  ((A) & 0x00000040)
#define BIT_7(A)  ((A) & 0x00000080)
#define BIT_8(A)  ((A) & 0x00000100)
#define BIT_9(A)  ((A) & 0x00000200)
#define BIT_A(A)  ((A) & 0x00000400)
#define BIT_B(A)  ((A) & 0x00000800)
#define BIT_C(A)  ((A) & 0x00001000)
#define BIT_D(A)  ((A) & 0x00002000)
#define BIT_E(A)  ((A) & 0x00004000)
#define BIT_F(A)  ((A) & 0x00008000)
#define BIT_10(A) ((A) & 0x00010000)
#define BIT_11(A) ((A) & 0x00020000)
#define BIT_12(A) ((A) & 0x00040000)
#define BIT_13(A) ((A) & 0x00080000)
#define BIT_14(A) ((A) & 0x00100000)
#define BIT_15(A) ((A) & 0x00200000)
#define BIT_16(A) ((A) & 0x00400000)
#define BIT_17(A) ((A) & 0x00800000)
#define BIT_18(A) ((A) & 0x01000000)
#define BIT_19(A) ((A) & 0x02000000)
#define BIT_1A(A) ((A) & 0x04000000)
#define BIT_1B(A) ((A) & 0x08000000)
#define BIT_1C(A) ((A) & 0x10000000)
#define BIT_1D(A) ((A) & 0x20000000)
#define BIT_1E(A) ((A) & 0x40000000)
#define BIT_1F(A) ((A) & 0x80000000)

/* Get the most significant bit for specific sizes */
#define GET_MSB_8(A)  ((A) & 0x80)
#define GET_MSB_9(A)  ((A) & 0x100)
#define GET_MSB_16(A) ((A) & 0x8000)
#define GET_MSB_17(A) ((A) & 0x10000)
#define GET_MSB_32(A) ((A) & 0x80000000)

/* Isolate nibbles */
#define LOW_NIBBLE(A) ((A) & 0x0f)
#define HIGH_NIBBLE(A) ((A) & 0xf0)

/* These are used to isolate 8, 16, and 32 bit sizes */
#define MASK_OUT_ABOVE_8(A)  ((A) & 0xff)
#define MASK_OUT_ABOVE_16(A) ((A) & 0xffff)
#define MASK_OUT_BELOW_8(A)  ((A) & ~0xff)
#define MASK_OUT_BELOW_16(A) ((A) & ~0xffff)

/* No need for useless masking if we're 32-bit */
//#if M68K_OVER_32_BIT
//#define MASK_OUT_ABOVE_32(A) ((A) & 0xffffffff)
//#define MASK_OUT_BELOW_32(A) ((A) & ~0xffffffff)
//#else
#define MASK_OUT_ABOVE_32(A) (A)
#define MASK_OUT_BELOW_32(A) 0
//#endif /* M68K_OVER_32_BIT */


/* Simulate address lines of 68k family */
#define ADDRESS_68K(A) ((A)&0x00ffffff) //(FLAG_MODE & FLAG_MODE_020_PLUS ? A : (A)&0xffffff)
//#define ADDRESS_68K(A) (FLAG_MODE & FLAG_MODE_020_PLUS ? A : (A)&0xffffff)


/* Instruction extension word information for indexed addressing modes */
#define EXT_INDEX_LONG(A)		  BIT_B(A)
#define EXT_INDEX_AR(A) 		  BIT_F(A)
#define EXT_INDEX_REGISTER(A)	  (((A)>>12)&7)
#define EXT_INDEX_SCALE(A)		  (((A)>>9)&3)
#define EXT_BRIEF_FORMAT(A) 	  !BIT_8(A)
#define EXT_IX_SUPPRESSED(A)	  BIT_6(A)
#define EXT_BR_SUPPRESSED(A)	  BIT_7(A)
#define EXT_BD_PRESENT(A)		  BIT_5(A)
#define EXT_BD_LONG(A)			  BIT_4(A)
#define EXT_NO_MEMORY_INDIRECT(A) !((A)&7)
#define EXT_OD_PRESENT(A)		  BIT_1(A)
#define EXT_OD_LONG(A)			  BIT_0(A)
#define EXT_POSTINDEX(A)		  BIT_2(A)


/* Shift & Rotate Macros.
 * 32-bit shifts defined in architecture-dependant section.
 */

#define LSL(A, C) ((A) << (C))
#define LSR(A, C) ((A) >> (C))

/* Some > 32-bit optimizations */
#if M68K_OVER_32_BIT
/* Shift left and right */
#define LSR_32(A, C) ((A) >> (C))
#define LSL_32(A, C) ((A) << (C))
#else
/* We have to do this because the morons at ANSI decided that shifts
 * by >= data size are undefined.
 */
#define LSR_32(A, C) ((C) < 32 ? (A) >> (C) : 0)
#define LSL_32(A, C) ((C) < 32 ? (A) << (C) : 0)
#endif /* M68K_OVER_32_BIT */

#define ROL_8(A, C) 	 MASK_OUT_ABOVE_8(LSL(A, C) | LSR(A, 8-(C)))
#define ROL_9(A, C) 					  LSL(A, C) | LSR(A, 9-(C))
#define ROL_16(A, C)	MASK_OUT_ABOVE_16(LSL(A, C) | LSR(A, 16-(C)))
#define ROL_17(A, C)					  LSL(A, C) | LSR(A, 17-(C))
#define ROL_32(A, C) MASK_OUT_ABOVE_32(LSL_32(A, C) | LSR_32(A, 32-(C)))
#define ROL_33(A, C)				  (LSL_32(A, C) | LSR_32(A, 33-(C)))

#define ROR_8(A, C) 	 MASK_OUT_ABOVE_8(LSR(A, C) | LSL(A, 8-(C)))
#define ROR_9(A, C) 					  LSR(A, C) | LSL(A, 9-(C))
#define ROR_16(A, C)	MASK_OUT_ABOVE_16(LSR(A, C) | LSL(A, 16-(C)))
#define ROR_17(A, C)					  LSR(A, C) | LSL(A, 17-(C))
#define ROR_32(A, C) MASK_OUT_ABOVE_32(LSR_32(A, C) | LSL_32(A, 32-(C)))
#define ROR_33(A, C)				  (LSR_32(A, C) | LSL_32(A, 33-(C)))


/* Access the MPU registers */
//#define FLAG_MODE 	m68ki_mpu.mode
#define REG_D			m68ki_mpu.dar
#define REG_A			(m68ki_mpu.dar+8)
#define REG_PPC 		m68ki_mpu.ppc
#define REG_PC			m68ki_mpu.pc
#define REG_SP_BASE 	m68ki_mpu.sp
#define REG_USP 		m68ki_mpu.sp[0]
#define REG_ISP 		m68ki_mpu.sp[1]
#define REG_MSP 		m68ki_mpu.sp[3]

#define REG_IR			m68ki_mpu.ir

#define FLAG_T1 		m68ki_mpu.t1_flag
#define FLAG_T0 		m68ki_mpu.t0_flag
#define FLAG_S			m68ki_mpu.s_flag
#define FLAG_M			m68ki_mpu.m_flag
#define FLAG_X			m68ki_mpu.x_flag
#define FLAG_N			m68ki_mpu.n_flag
#define FLAG_Z			m68ki_mpu.not_z_flag
#define FLAG_V			m68ki_mpu.v_flag
#define FLAG_C			m68ki_mpu.c_flag
#define FLAG_INT_MASK	m68ki_mpu.int_mask

#define CPU_INT_LEVEL	m68ki_mpu.int_level /* ASG: changed from CPU_INTS_PENDING */
#define CPU_INT_CYCLES	m68ki_mpu.int_cycles /* ASG */

//#define CPU_INT_STATE   m68ki_mpu.int_state /* ASG: changed from CPU_INTS_PENDING */
#define CPU_STOPPED 	m68ki_mpu.stopped
//#define CPU_HALTED		m68ki_mpu.halted


#define 	CYC_BCC_NOTAKE_B   -2
#define 	CYC_BCC_NOTAKE_W	2
#define 	CYC_DBCC_F_NOEXP   -2
#define 	CYC_DBCC_F_EXP		2
#define 	CYC_SCC_R_FALSE 	2
#define 	CYC_MOVEM_W 		2
#define 	CYC_MOVEM_L 		3
#define 	CYC_SHIFT			1
#define 	CYC_RESET		  132



/*
 * The general instruction format follows this pattern:
 * .... XXX. .... .YYY
 * where XXX is register X and YYY is register Y
 */
/* Data Register Isolation */
#define DX (REG_D[(REG_IR >> 9) & 7])
#define DY (REG_D[REG_IR & 7])
/* Address Register Isolation */
#define AX (REG_A[(REG_IR >> 9) & 7])
#define AY (REG_A[REG_IR & 7])


/* Effective Address Calculations */
#define EA_AI	 AY 								   /* address register indirect */
#define EA_PI_8  (AY++) 							   /* postincrement (size = byte) */
#define EA_PI7_8 ((REG_A[7]+=2)-2)					   /* postincrement (size = byte & AR = 7) */
#define EA_PI_16 ((AY+=2)-2)						   /* postincrement (size = word) */
#define EA_PI_32 ((AY+=4)-4)						   /* postincrement (size = long) */
#define EA_PD_8  (--AY) 							   /* predecrement (size = byte) */
#define EA_PD7_8 (REG_A[7]-=2)						   /* predecrement (size = byte & AR = 7) */
#define EA_PD_16 (AY-=2)							   /* predecrement (size = word) */
#define EA_PD_32 (AY-=4)							   /* predecrement (size = long) */
#define EA_DI	 (AY+MAKE_INT_16(m68ki_read_imm_16())) /* displacement */
#define EA_IX	 m68ki_get_ea_ix_AY()					  /* indirect + index */
#define EA_AW	 MAKE_INT_16(m68ki_read_imm_16())	   /* absolute word */
#define EA_AL	 m68ki_read_imm_32()				   /* absolute long */
#define EA_PCIX  m68ki_get_ea_pcix()				   /* pc indirect + index */


/* Add and Subtract Flag Calculation Macros */
#define VFLAG_ADD_8(S, D, R) GET_MSB_8((S & D & ~R) | (~S & ~D & R))
#define VFLAG_ADD_16(S, D, R) GET_MSB_16((S & D & ~R) | (~S & ~D & R))
#define VFLAG_ADD_32(S, D, R) GET_MSB_32((S & D & ~R) | (~S & ~D & R))

#define CFLAG_ADD_8(S, D, R) GET_MSB_8((S & D) | (~R & D) | (S & ~R))
#define CFLAG_ADD_16(S, D, R) GET_MSB_16((S & D) | (~R & D) | (S & ~R))
#define CFLAG_ADD_32(S, D, R) GET_MSB_32((S & D) | (~R & D) | (S & ~R))


#define VFLAG_SUB_8(S, D, R) GET_MSB_8((~S & D & ~R) | (S & ~D & R))
#define VFLAG_SUB_16(S, D, R) GET_MSB_16((~S & D & ~R) | (S & ~D & R))
#define VFLAG_SUB_32(S, D, R) GET_MSB_32((~S & D & ~R) | (S & ~D & R))

#define CFLAG_SUB_8(S, D, R) GET_MSB_8((S & ~D) | (R & ~D) | (S & R))
#define CFLAG_SUB_16(S, D, R) GET_MSB_16((S & ~D) | (R & ~D) | (S & R))
#define CFLAG_SUB_32(S, D, R) GET_MSB_32((S & ~D) | (R & ~D) | (S & R))




#define NFLAG_8(A) (A)
#define NFLAG_16(A) ((A)>>8)
#define NFLAG_32(A) ((A)>>24)
//#define NFLAG_64(A) ((A)>>56)

#define ZFLAG_8(A) MASK_OUT_ABOVE_8(A)
#define ZFLAG_16(A) MASK_OUT_ABOVE_16(A)
#define ZFLAG_32(A) MASK_OUT_ABOVE_32(A)









/* Conditions */
#define CONDITION_HI	 (FLAG_C == 0 && FLAG_Z != 0)
#define CONDITION_NOT_HI (FLAG_C != 0 || FLAG_Z == 0)
#define CONDITION_LS	 (FLAG_C != 0 || FLAG_Z == 0)
#define CONDITION_NOT_LS (FLAG_C == 0 && FLAG_Z != 0)
#define CONDITION_CC	 (FLAG_C == 0)
#define CONDITION_NOT_CC (FLAG_C != 0)
#define CONDITION_CS	 (FLAG_C != 0)
#define CONDITION_NOT_CS (FLAG_C == 0)
#define CONDITION_NE	 (FLAG_Z != 0)
#define CONDITION_NOT_NE (FLAG_Z == 0)
#define CONDITION_EQ	 (FLAG_Z == 0)
#define CONDITION_NOT_EQ (FLAG_Z != 0)
#define CONDITION_VC	 (FLAG_V == 0)
#define CONDITION_NOT_VC (FLAG_V != 0)
#define CONDITION_VS	 (FLAG_V != 0)
#define CONDITION_NOT_VS (FLAG_V == 0)
#define CONDITION_PL	 (FLAG_N == 0)
#define CONDITION_NOT_PL (FLAG_N != 0)
#define CONDITION_MI	 (FLAG_N != 0)
#define CONDITION_NOT_MI (FLAG_N == 0)
#define CONDITION_GE	 ((FLAG_N == 0) == (FLAG_V == 0))
#define CONDITION_NOT_GE ((FLAG_N == 0) != (FLAG_V == 0))
#define CONDITION_LT	 ((FLAG_N == 0) != (FLAG_V == 0))
#define CONDITION_NOT_LT ((FLAG_N == 0) == (FLAG_V == 0))
#define CONDITION_GT	 (FLAG_Z != 0 && (FLAG_N == 0) == (FLAG_V == 0))
#define CONDITION_NOT_GT (FLAG_Z == 0 || (FLAG_N == 0) != (FLAG_V == 0))
#define CONDITION_LE	 (FLAG_Z == 0 || (FLAG_N == 0) != (FLAG_V == 0))
#define CONDITION_NOT_LE (FLAG_Z != 0 && (FLAG_N == 0) == (FLAG_V == 0))


/* Use up clock cycles.
 * NOTE: clock cycles used in here are 99.9% correct for a 68000, not for the
 * higher processors.
 */
#define USE_CLKS(A) m68ki_remaining_cycles -= (A)


/* Push/pull data to/from the stack */
#define m68ki_push_16(A) m68ki_write_16(REG_A[7]-=2, A)
#define m68ki_push_32(A) m68ki_write_32(REG_A[7]-=4, A)
#define m68ki_pull_16()  m68ki_read_16((REG_A[7]+=2) - 2)
#define m68ki_pull_32()  m68ki_read_32((REG_A[7]+=4) - 4)


/* branch byte and word are for branches, while long is for jumps.
 * So far it's been safe to not call set_pc() for branch word.
 */
#define m68ki_branch_byte(A) REG_PC += MAKE_INT_8(A)
#define m68ki_branch_word(A) REG_PC += MAKE_INT_16(A)
#define m68ki_branch_long(A) m68ki_set_pc(A)


/* Get the condition code register */
#define m68ki_get_ccr() (((FLAG_X != 0) 	<< 4) | \
						 ((FLAG_N != 0) 	<< 3) | \
						 ((FLAG_Z == 0) 	<< 2) | \
						 ((FLAG_V != 0) 	<< 1) | \
						  (FLAG_C != 0))

/* Get the status register */
#define m68ki_get_sr() (((FLAG_T1 != 0) 	<< 15) | \
						((FLAG_T0 != 0) 	<< 14) | \
						((FLAG_S != 0)		<< 13) | \
						((FLAG_M != 0)		<< 12) | \
						 (FLAG_INT_MASK 	/*<<  8*/) | \
						  m68ki_get_ccr())


/* ======================================================================== */
/* ========================= CONFIGURATION DEFINES ======================== */
/* ======================================================================== */

/* Act on values in m68kconf.h */
#if M68K_INT_ACK
#define m68ki_int_ack(A) CPU_INT_ACK_CALLBACK(A)
#else
/* Default action is to used autovector mode, which is most common */
#define m68ki_int_ack(A) M68K_INT_ACK_AUTOVECTOR
#endif /* M68K_INT_ACK */

#if M68K_BKPT_ACK
#define m68ki_bkpt_ack(A) CPU_BKPT_ACK_CALLBACK(A)
#else
#define m68ki_bkpt_ack(A)
#endif /* M68K_BKPT_ACK */

#if M68K_OUTPUT_RESET
#define m68ki_output_reset() CPU_RESET_INSTR_CALLBACK()
#else
#define m68ki_output_reset()
#endif /* M68K_OUTPUT_RESET */

#if M68K_PC_CHANGED
#define m68ki_pc_changed(A) REG_PC_CHANGED_CALLBACK(A)
#else
#define m68ki_pc_changed(A)
#endif /* M68K_PC_CHANGED */

#if M68K_SET_FC
#define m68ki_set_fc(A) FLAG_SET_FC_CALLBACK(A)
#else
#define m68ki_set_fc(A)
#endif /* M68K_SET_FC */

#if M68K_INSTR_HOOK
#define m68ki_instr_hook() CPU_INSTR_HOOK_CALLBACK()
#else
#define m68ki_instr_hook()
#endif /* M68K_INSTR_HOOK */

#if M68K_TRACE
/* Initiates trace checking before each instruction (t1) */
#define m68ki_set_trace() m68k_tracing = FLAG_T1
/* adds t0 to trace checking if we encounter change of flow */
#define m68ki_add_trace() m68k_tracing |= FLAG_T0
/* Clear all tracing */
#define m68ki_clear_trace() m68k_tracing = 0
/* Cause a trace exception if we are tracing */
#define m68ki_exception_if_trace() if(m68k_tracing) m68ki_exception(EXCEPTION_TRACE)
#else
#define m68ki_set_trace()
#define m68ki_add_trace()
#define m68ki_clear_trace()
#define m68ki_exception_if_trace()
#endif /* M68K_TRACE */


#ifdef M68K_LOG

extern char* m68k_disassemble_quick(uint pc);
extern uint  m68k_pc_offset;
extern char* m68k_mpu_names[];

#define M68K_DO_LOG(A) if(M68K_LOG) fprintf A
#if M68K_LOG_EMULATED_INSTRUCTIONS
#define M68K_DO_LOG_EMU(A) if(M68K_LOG) fprintf A
#else
#define M68K_DO_LOG_EMU(A)
#endif

#else
#define M68K_DO_LOG(A)
#define M68K_DO_LOG_EMU(A)

#endif


/* ======================================================================== */
/* =============================== PROTOTYPES ============================= */
/* ======================================================================== */

typedef struct
{
//	 uint mode; 		/* MPU Operation Mode: 68000, 68010, or 68020 */
	uint dar[16];		/* Data and Address Registers */
	uint ppc;			/* Previous program counter */
	uint pc;			/* Program Counter */
	uint sp[4]; 		/* User, Interrupt, and Master Stack Pointers */
//	 uint vbr;			/* Vector Base Register (68010+) */
//	 uint sfc;			/* Source Function Code Register (m68010+) */
//	 uint dfc;			/* Destination Function Code Register (m68010+) */
	uint ir;			/* Instruction Register */
	uint t1_flag;		/* Trace 1 */
	uint t0_flag;		/* Trace 0 */
	uint s_flag;		/* Supervisor */
	uint m_flag;		/* Master/Interrupt state */
	uint x_flag;		/* Extend */
	uint n_flag;		/* Negative */
	uint not_z_flag;	/* Zero, inverted for speedups */
	uint v_flag;		/* Overflow */
	uint c_flag;		/* Carry */
	uint int_mask;		/* I0-I2 */

	uint int_level;    /* State of interrupt pins IPL0-IPL2 -- ASG: changed from ints_pending */
/// uint int_state; 	/* Current interrupt state -- ASG: changed from ints_pending */
	uint stopped;		/* Stopped state */
//	uint halted;		/* Halted state */
	uint int_cycles;	/* ASG: extra cycles from generated interrupts */


} m68ki_mpu_core;



extern int			 m68ki_remaining_cycles;
extern uint 		 m68k_tracing;

//extern m68ki_mpu_core m68ki_mpu;
/* The MPU core */
static m68ki_mpu_core m68ki_mpu __attribute__((aligned(64))) ;// = {0};

static uint*		 m68k_mpu_dar[] __attribute__((aligned(64))) ;
//extern uint*		   m68k_movem_pi_table[];
//extern uint*		   m68k_movem_pd_table[];

///static uint8 		m68k_int_masks[];
//static uint8		 m68ki_shift_8_table[];
//static uint16		 m68ki_shift_16_table[];
//static uint 		 m68ki_shift_32_table[];
//static uint8		 m68ki_exception_cycle_table[];
/* Used when checking for pending interrupts */
///static uint8 m68k_int_masks[] __attribute__((aligned(64))) =
/// {0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x80};

/* Used by shift & rotate instructions */
static uint8 m68ki_shift_8_table[65] __attribute__((aligned(64))) =
{
 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff
};
static uint16 m68ki_shift_16_table[65] __attribute__((aligned(64))) =
{
 0x0000, 0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00,
 0xff00, 0xff80, 0xffc0, 0xffe0, 0xfff0, 0xfff8, 0xfffc, 0xfffe,
 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
 0xffff
};
static uint m68ki_shift_32_table[65] __attribute__((aligned(64))) =
{
 0x00000000, 0x80000000, 0xc0000000, 0xe0000000, 0xf0000000, 0xf8000000, 0xfc000000, 0xfe000000,
 0xff000000, 0xff800000, 0xffc00000, 0xffe00000, 0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000,
 0xffff0000, 0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00,
 0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0, 0xfffffff8, 0xfffffffc, 0xfffffffe,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 0xffffffff
};


/* Number of clock cycles to use for exception processing.
 * I used 4 for any vectors that are undocumented for processing times.
 */
static const uint8 m68ki_exception_cycle_table[48/*256*/] __attribute__((aligned(64))) =
{
    4, /*40  0: Reset - Initial Stack Pointer    (should never be called)    */
    4, /*40  1: Reset - Initial Program Counter  (should never be called)    */
   50, /*  2: Bus Error                                (unused in emulation) */
   50, /*  3: Address Error                            (unused in emulation) */
   34, /*  4: Illegal Instruction                                            */
   38, /*  5: Divide by Zero -- ASG: changed from 42                         */
   40, /*  6: CHK -- ASG: chanaged from 44                                   */
   34, /*  7: TRAPV                                                          */
   34, /*  8: Privilege Violation                                            */
   34, /*  9: Trace                                                          */
    4, /* 10: 1010                                                           */
    4, /* 11: 1111                                                           */
    4, /* 12: RESERVED                                                       */
    4, /* 13: Coprocessor Protocol Violation           (unused in emulation) */
    4, /* 14: Format Error                             (unused in emulation) */
   44, /* 15: Uninitialized Interrupt                                        */
    4, /* 16: RESERVED                                                       */
    4, /* 17: RESERVED                                                       */
    4, /* 18: RESERVED                                                       */
    4, /* 19: RESERVED                                                       */
    4, /* 20: RESERVED                                                       */
    4, /* 21: RESERVED                                                       */
    4, /* 22: RESERVED                                                       */
    4, /* 23: RESERVED                                                       */
   44, /* 24: Spurious Interrupt                                             */
   44, /* 25: Level 1 Interrupt Autovector                                   */
   44, /* 26: Level 2 Interrupt Autovector                                   */
   44, /* 27: Level 3 Interrupt Autovector                                   */
   44, /* 28: Level 4 Interrupt Autovector                                   */
   44, /* 29: Level 5 Interrupt Autovector                                   */
   44, /* 30: Level 6 Interrupt Autovector                                   */
   44, /* 31: Level 7 Interrupt Autovector                                   */
   34, /* 32: TRAP #0 -- ASG: chanaged from 38                               */
   34, /* 33: TRAP #1                                                        */
   34, /* 34: TRAP #2                                                        */
   34, /* 35: TRAP #3                                                        */
   34, /* 36: TRAP #4                                                        */
   34, /* 37: TRAP #5                                                        */
   34, /* 38: TRAP #6                                                        */
   34, /* 39: TRAP #7                                                        */
   34, /* 40: TRAP #8                                                        */
   34, /* 41: TRAP #9                                                        */
   34, /* 42: TRAP #10                                                       */
   34, /* 43: TRAP #11                                                       */
   34, /* 44: TRAP #12                                                       */
   34, /* 45: TRAP #13                                                       */
   34, /* 46: TRAP #14                                                       */
   34, /* 47: TRAP #15                                                       */
#if 00
    4, /* 48: FP Branch or Set on Unknown Condition    (unused in emulation) */
    4, /* 49: FP Inexact Result                        (unused in emulation) */
    4, /* 50: FP Divide by Zero                        (unused in emulation) */
    4, /* 51: FP Underflow                             (unused in emulation) */
    4, /* 52: FP Operand Error                         (unused in emulation) */
    4, /* 53: FP Overflow                              (unused in emulation) */
    4, /* 54: FP Signaling NAN                         (unused in emulation) */
    4, /* 55: FP Unimplemented Data Type               (unused in emulation) */
    4, /* 56: MMU Configuration Error                  (unused in emulation) */
    4, /* 57: MMU Illegal Operation Error              (unused in emulation) */
    4, /* 58: MMU Access Level Violation Error         (unused in emulation) */
    4, /* 59: RESERVED                                                       */
    4, /* 60: RESERVED                                                       */
    4, /* 61: RESERVED                                                       */
    4, /* 62: RESERVED                                                       */
    4, /* 63: RESERVED                                                       */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /*  64- 79: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /*  80- 95: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /*  96-111: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 112-127: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 128-143: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 144-159: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 160-175: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 176-191: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 192-207: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 208-223: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 224-239: User Defined */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 240-255: User Defined */
#endif
};



/* Read data from anywhere */
INLINE uint m68ki_read_8  (uint address);
INLINE uint m68ki_read_16 (uint address);
INLINE uint m68ki_read_32 (uint address);

/* Write to memory */
INLINE void m68ki_write_8 (uint address, uint value);
INLINE void m68ki_write_16(uint address, uint value);
INLINE void m68ki_write_32(uint address, uint value);

/* Read data immediately after the program counter */
INLINE uint m68ki_read_imm_8(void);
INLINE uint m68ki_read_imm_16(void);
INLINE uint m68ki_read_imm_32(void);

/* Reads the next word after the program counter */
INLINE uint m68ki_read_instruction(void);

///* Read data with specific function code */
//INLINE uint m68ki_read_8_fc  (uint address, uint fc);
//INLINE uint m68ki_read_16_fc (uint address, uint fc);
//INLINE uint m68ki_read_32_fc (uint address, uint fc);
//
///* Write data with specific function code */
//INLINE void m68ki_write_8_fc (uint address, uint fc, uint value);
//INLINE void m68ki_write_16_fc(uint address, uint fc, uint value);
//INLINE void m68ki_write_32_fc(uint address, uint fc, uint value);

INLINE uint m68ki_get_ea_ix_AY(void);		/* Get ea for address register indirect + index */
INLINE uint m68ki_get_ea_pcix(void);		/* Get ea for program counter indirect + index */
INLINE uint m68ki_get_ea_ix_dst(void);		/* Get ea ix for destination of move instruction */

INLINE void m68ki_set_s_flag(int value);					/* Set the S flag */
//INLINE void m68ki_set_m_flag(int value);					/* Set the M flag */
INLINE void m68ki_set_sm_flag(int s_value, int m_value);	/* Set the S and M flags */
INLINE void m68ki_set_ccr(uint value);						/* set the condition code register */
INLINE void m68ki_set_sr(uint value);						/* set the status register */
//INLINE void m68ki_set_sr_no_int(uint value);				/* ASG: set the status register, but don't check interrupts */
INLINE void m68ki_set_pc(uint address); 					/* set the program counter */
INLINE void m68ki_exception_interrupt(uint int_level);	 /* service a pending interrupt -- ASG: added parameter */
INLINE void m68ki_exception(uint vector);					/* process an exception */
INLINE void m68ki_interrupt(uint vector);					/* process an interrupt */
INLINE void m68ki_check_interrupts(void);					/* ASG: check for interrupts */

/* ======================================================================== */
/* =========================== UTILITY FUNCTIONS ========================== */
/* ======================================================================== */

/* Set the function code and read memory from anywhere. */
INLINE uint m68ki_read_8(uint address)
{
   m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
   return m68k_read_memory_8(ADDRESS_68K(address));
}
INLINE uint m68ki_read_16(uint address)
{
   m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
   return m68k_read_memory_16(ADDRESS_68K(address));
}
INLINE uint m68ki_read_32(uint address)
{
	m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
//	return m68k_read_memory_32(ADDRESS_68K(address));
	return((m68k_read_memory_16(ADDRESS_68K(address))<<16) | (m68k_read_memory_16(ADDRESS_68K(address)+2)));
}

/* Set the function code and read memory immediately following the PC. */
INLINE uint m68ki_read_imm_8(void)
{
   m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
   REG_PC += 2;
// return m68k_read_immediate_8(ADDRESS_68K(REG_PC-1));
   return m68k_read_memory_8(ADDRESS_68K(REG_PC-1));
}
INLINE uint m68ki_read_imm_16(void)
{
   m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
   REG_PC += 2;
// return m68k_read_immediate_16(ADDRESS_68K(REG_PC-2));
   return m68k_read_memory_16(ADDRESS_68K(REG_PC-2));
}
INLINE uint m68ki_read_imm_32(void)
{
   m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
   REG_PC += 4;
//	return m68k_read_immediate_32(ADDRESS_68K(REG_PC-4));
	return((m68k_read_memory_16(ADDRESS_68K(REG_PC-4))<<16) | (m68k_read_memory_16(ADDRESS_68K(REG_PC-4)+2)));
}

/* Set the function code and write memory to anywhere. */
INLINE void m68ki_write_8(uint address, uint value)
{
	m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
	m68k_write_memory_8(ADDRESS_68K(address), value);
}
INLINE void m68ki_write_16(uint address, uint value)
{
	m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
	m68k_write_memory_16(ADDRESS_68K(address), value);
}
INLINE void m68ki_write_32(uint address, uint value)
{
	m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_DATA : FUNCTION_CODE_USER_DATA);
//	m68k_write_memory_32(ADDRESS_68K(address), value);
	m68k_write_memory_16(ADDRESS_68K(address)  ,(value>>16) );
	m68k_write_memory_16(ADDRESS_68K(address)+2,value&0xffff);
}



/* Set the function code and read an instruction immediately following the PC. */
INLINE uint m68ki_read_instruction(void)
{
	m68ki_set_fc(FLAG_S ? FUNCTION_CODE_SUPERVISOR_PROGRAM : FUNCTION_CODE_USER_PROGRAM);
	REG_PC += 2;
//	return m68k_ read_instruction(ADDRESS_68K(REG_PC-2));
//	return m68k_read_immediate_16(ADDRESS_68K(REG_PC-2));
	return m68k_read_memory_16(ADDRESS_68K(REG_PC-2));
}

#if 000
/* Read/Write data with a specific function code (used by MOVES) */
INLINE uint m68ki_read_8_fc(uint address, uint fc)
{
   m68ki_set_fc(fc&7);
   return m68k_read_memory_8(ADDRESS_68K(address));
}
INLINE uint m68ki_read_16_fc(uint address, uint fc)
{
   m68ki_set_fc(fc&7);
   return m68k_read_memory_16(ADDRESS_68K(address));
}
INLINE uint m68ki_read_32_fc(uint address, uint fc)
{
   m68ki_set_fc(fc&7);
   return m68k_read_memory_32(ADDRESS_68K(address));
}

INLINE void m68ki_write_8_fc(uint address, uint fc, uint value)
{
   m68ki_set_fc(fc&7);
   m68k_write_memory_8(ADDRESS_68K(address), value);
}
INLINE void m68ki_write_16_fc(uint address, uint fc, uint value)
{
   m68ki_set_fc(fc&7);
   m68k_write_memory_16(ADDRESS_68K(address), value);
}
INLINE void m68ki_write_32_fc(uint address, uint fc, uint value)
{
   m68ki_set_fc(fc&7);
   m68k_write_memory_32(ADDRESS_68K(address), value);
}
#endif

/* Decode address register indirect with index */
INLINE uint m68ki_get_ea_ix_AY(void)
{
   uint extension = m68ki_read_imm_16();
   uint Xn;
   uint base = AY;
//	 uint outer = 0;
   Xn = m68k_mpu_dar[EXT_INDEX_AR(extension)!=0][EXT_INDEX_REGISTER(extension)];

   /* Sign-extend the index value if needed */
   if(!EXT_INDEX_LONG(extension))
	  Xn = MAKE_INT_16(Xn);

   /* If we're running 010 or less, there's no scale or full extension word mode */
//	 if(FLAG_MODE & FLAG_MODE_010_LESS)
	  return base + Xn + MAKE_INT_8(extension);


}

/* Decode address register indirect with index for MOVE destination */
INLINE uint m68ki_get_ea_ix_dst(void)
{
   uint extension = m68ki_read_imm_16();
   uint Xn;
   uint base = AX; /* This is the only thing different from m68ki_get_ea_ix_AY() */
//	 uint outer = 0;
   Xn = m68k_mpu_dar[EXT_INDEX_AR(extension)!=0][EXT_INDEX_REGISTER(extension)];

   /* Sign-extend the index value if needed */
   if(!EXT_INDEX_LONG(extension))
	  Xn = MAKE_INT_16(Xn);

   /* If we're running 010 or less, there's no scale or full extension word mode */
//	 if(FLAG_MODE & FLAG_MODE_010_LESS)
	  return base + Xn + MAKE_INT_8(extension);


}

/* Decode program counter indirect with index */
INLINE uint m68ki_get_ea_pcix(void)
{
   uint base = (REG_PC+=2) - 2;
   uint extension = m68ki_read_16(base);
   uint Xn = m68k_mpu_dar[EXT_INDEX_AR(extension)!=0][EXT_INDEX_REGISTER(extension)];
//	 uint outer = 0;

   /* Sign-extend the index value if needed */
   if(!EXT_INDEX_LONG(extension))
	  Xn = MAKE_INT_16(Xn);

   /* If we're running 010 or less, there's no scale or full extension word mode */
//	 if(FLAG_MODE & FLAG_MODE_010_LESS)
	  return base + Xn + MAKE_INT_8(extension);


}


/* Set the S flag and change the active stack pointer. */
INLINE void m68ki_set_s_flag(int value)
{
   /* ASG: Only do the rest if we're changing */
   value = (value != 0);
   if (FLAG_S != value)
   {
	  /* Backup the old stack pointer */
	  REG_SP_BASE[FLAG_S | (FLAG_M & (FLAG_S<<1))] = REG_A[7];
	  /* Set the S flag */
	  FLAG_S = value;
	  /* Set the new stack pointer */
	  REG_A[7] = REG_SP_BASE[FLAG_S | (FLAG_M & (FLAG_S<<1))];
   }
}

#if 00
/* Set the M flag and change the active stack pointer. */
INLINE void m68ki_set_m_flag(int value)
{
   /* ASG: Only do the rest if we're changing */
   value = (value != 0 /*&& FLAG_MODE & FLAG_MODE_020_PLUS*/)<<1;
   if (FLAG_M != value)
   {
	  /* Backup the old stack pointer */
	  REG_SP_BASE[FLAG_S | (FLAG_M & (FLAG_S<<1))] = REG_A[7];
	  /* Set the M flag */
	  FLAG_M = value;
	  /* Set the new stack pointer */
	  REG_A[7] = REG_SP_BASE[FLAG_S | (FLAG_M & (FLAG_S<<1))];
   }
}
#endif

/* Set the S and M flags and change the active stack pointer. */
INLINE void m68ki_set_sm_flag(int s_value, int m_value)
{
   /* ASG: Only do the rest if we're changing */
   s_value = (s_value != 0);
   m_value = (m_value != 0 /*&& FLAG_MODE & FLAG_MODE_020_PLUS*/)<<1;
   if (FLAG_S != s_value || FLAG_M != m_value)
   {
	  /* Backup the old stack pointer */
	  REG_SP_BASE[FLAG_S | (FLAG_M & (FLAG_S<<1))] = REG_A[7];
	  /* Set the S and M flags */
	  FLAG_S = s_value != 0;
	  FLAG_M = (m_value != 0 /*&& FLAG_MODE & FLAG_MODE_020_PLUS*/)<<1;
	  /* Set the new stack pointer */
	  REG_A[7] = REG_SP_BASE[FLAG_S | (FLAG_M & (FLAG_S<<1))];
   }
}


/* Set the condition code register */
INLINE void m68ki_set_ccr(uint value)
{
   FLAG_X = BIT_4(value);
   FLAG_N = BIT_3(value);
   FLAG_Z = !BIT_2(value);
   FLAG_V = BIT_1(value);
   FLAG_C = BIT_0(value);
}

/* Set the status register */
INLINE void m68ki_set_sr(uint value)
{
   /* ASG: detect changes to the INT_MASK */
/// int old_mask = FLAG_INT_MASK;

   /* Mask out the "unimplemented" bits */
   value &= 0xa71f; /* 68000: T1 -- S  -- -- I2 I1 I0 -- -- -- X  N  Z	V  C  */

   /* Now set the status register */
   FLAG_T1 = BIT_F(value);
   FLAG_T0 = BIT_E(value);
//	FLAG_INT_MASK = (value >> 8) & 7;
	FLAG_INT_MASK = value & 0x0700;
   FLAG_X = BIT_4(value);
   FLAG_N = BIT_3(value);
   FLAG_Z = !BIT_2(value);
   FLAG_V = BIT_1(value);
   FLAG_C = BIT_0(value);
   m68ki_set_sm_flag(BIT_D(value), BIT_C(value));

   /* ASG: detect changes to the INT_MASK */
///   if (FLAG_INT_MASK != old_mask)
	  m68ki_check_interrupts();
}

#if 00
/* Set the status register */
INLINE void m68ki_set_sr_no_int(uint value)
{
   /* Mask out the "unimplemented" bits */
   value &= 	0xa71f; /* 68000: T1 -- S  -- -- I2 I1 I0 -- -- -- X  N  Z	V  C  */

   /* Now set the status register */
   FLAG_T1 = BIT_F(value);
   FLAG_T0 = BIT_E(value);
   FLAG_INT_MASK = (value >> 8) & 7;
   FLAG_X = BIT_4(value);
   FLAG_N = BIT_3(value);
   FLAG_Z = !BIT_2(value);
   FLAG_V = BIT_1(value);
   FLAG_C = BIT_0(value);
   m68ki_set_sm_flag(BIT_D(value), BIT_C(value));
}
#endif

/* I set the PC this way to let host programs be nicer.
 * This is mainly for programs running from separate ram banks.
 * If the host program knows where the PC is, it can offer faster
 * ram access times for data to be retrieved immediately following
 * the PC.
 */
INLINE void m68ki_set_pc(uint address)
{
   /* Set the program counter */
   REG_PC = address;
   /* Inform the host program */
/* MAME */
//	 change_pc24(ADDRESS_68K(address));
/*
   m68ki_pc_changed(ADDRESS_68K(address));
*/
}


/* Process an exception */
INLINE void m68ki_exception(uint vector)
{
	/* Save the old status register */
	uint old_sr = m68ki_get_sr();

	/* Use up some clock cycles */
	if(vector<48)	USE_CLKS(m68ki_exception_cycle_table[vector]);
	else			USE_CLKS(4);

	/* Turn off stopped state and trace flag, clear pending traces */
	CPU_STOPPED = 0;
	FLAG_T1 = FLAG_T0 = 0;
	m68ki_clear_trace();
	/* Enter supervisor mode */
	m68ki_set_s_flag(1);
	/* Push a stack frame */
//	 if(FLAG_MODE & FLAG_MODE_010_PLUS)
//		m68ki_push_16(vector<<2); /* This is format 0 */
	m68ki_push_32(REG_PPC); /* save previous PC, ie. PC that contains an offending instruction */
	m68ki_push_16(old_sr);
	/* Generate a new program counter from the vector */
	m68ki_set_pc(m68ki_read_32((vector<<2)/*+FLAG_VBR*/));
}


/* Process an interrupt (or trap) */
INLINE void m68ki_interrupt(uint vector)
{
	/* Save the old status register */
	uint old_sr = m68ki_get_sr();

	/* Use up some clock cycles */
	/* ASG: just keep them pending */
/* USE_CLKS(m68ki_exception_cycle_table[vector]);*/
	if(vector<48)	CPU_INT_CYCLES += m68ki_exception_cycle_table[vector];
	else			CPU_INT_CYCLES += 4;

	/* Turn off stopped state and trace flag, clear pending traces */
	CPU_STOPPED = 0;
	FLAG_T1 = FLAG_T0 = 0;
	m68ki_clear_trace();
	/* Enter supervisor mode */
	m68ki_set_s_flag(1);
	/* Push a stack frame */
//	 if(FLAG_MODE & FLAG_MODE_010_PLUS)
//		m68ki_push_16(vector<<2); /* This is format 0 */
	m68ki_push_32(REG_PC);
	m68ki_push_16(old_sr);
	/* Generate a new program counter from the vector */
	m68ki_set_pc(m68ki_read_32((vector<<2)/*+FLAG_VBR*/));
}

//extern int vdp_int_ack_callback(int int_level);
//extern void vdp_status_int6(void);
/* Service an interrupt request */
INLINE void m68ki_exception_interrupt(uint int_level)	/* ASG: added parameter here */
{
	uint vector;
//	  uint int_level = 7;

	/* Start at level 7 and then go down */
///  for(;!(pending_mask & (1<<int_level));int_level--)	/* ASG: changed to use parameter instead of CPU_INTS_PENDING */
/// 	 ;
#if 00
/* ＩＮＴ割り込みコールバック可能版(picodrive) */
	/* Get the exception vector */
//	vector = m68ki_int_ack(int_level);
//	vector = vdp_int_ack_callback(int_level);
	vector = M68K_INT_ACK_AUTOVECTOR;
/*本当はコールバック入れた方がいいかも(メガドラは、int4==hint, int6==vint)*/

	/* hook vectoers */
	switch(vector){
	case M68K_INT_ACK_AUTOVECTOR:
		/* Use the autovectors.  This is the most commonly used implementation */
		vector = EXCEPTION_INTERRUPT_AUTOVECTOR+int_level;
		break;
	case M68K_INT_ACK_SPURIOUS:
		/* Called if no devices respond to the interrupt acknowledge */
		vector = EXCEPTION_SPURIOUS_INTERRUPT;
		break;
	}
#else
/* 簡略化版(segadrive) */
		vector = EXCEPTION_INTERRUPT_AUTOVECTOR+int_level;
	//if(6==int_level) vdp_status_int6();
#endif
	/* ignored */
	if(vector > 0xff)
	{
		 /* Everything else is ignored */
		 return;
	}

#if 1
	if(vector<2)
	{
	//	case 0x00: case 0x01:
	  /* vectors 0 and 1 are ignored since they are for reset only */
		 return;
	}
#endif
//							case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
//	  case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
// ...
//	  case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
//	  case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
		 /* The external peripheral has provided the interrupt vector to take */
//		 break;

	/* If vector is uninitialized, call the uninitialized interrupt vector */
	if(m68ki_read_32(vector<<2) == 0)
		vector = EXCEPTION_UNINITIALIZED_INTERRUPT;

	/* Generate an interupt */
	m68ki_interrupt(vector);

	/* Set the interrupt mask to the level of the one being serviced */
//	FLAG_INT_MASK = int_level;
	FLAG_INT_MASK = int_level<<8;

	/* Automatically clear IRQ if we are not using an acknowledge scheme */
	CPU_INT_LEVEL = 0;
}


/* ASG: Check for interrupts */
INLINE void m68ki_check_interrupts(void)
{
   //uint pending_mask = 1 << CPU_INT_STATE;
   //if (pending_mask & m68k_int_masks[FLAG_INT_MASK])
   //	m68ki_exception_interrupt(pending_mask);
	if(CPU_INT_LEVEL > FLAG_INT_MASK)
		m68ki_exception_interrupt(CPU_INT_LEVEL>>8);
}


#endif /* M68K_MPU__HEADER */
