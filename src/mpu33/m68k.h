#ifndef M68K__HEADER
#define M68K__HEADER

/* ======================================================================== */
/* ========================= LICENSING & COPYRIGHT ======================== */
/* ======================================================================== */
/*
 *                                  MUSASHI
 *                                Version 3.3
 *
 * A portable Motorola M680x0 processor emulation engine.
 * Copyright 1998-2001 Karl Stenerud.  All rights reserved.
 *
 * This code may be freely used for non-commercial purposes as long as this
 * copyright notice remains unaltered in the source code and any binary files
 * containing this code in compiled form.
 *
 * All other lisencing terms must be negotiated with the author
 * (Karl Stenerud).
 *
 * The latest version of this code can be obtained at:
 * http://kstenerud.cjb.net
 */

/* ======================================================================== */
/* ============================= CONFIGURATION ============================ */
/* ======================================================================== */

/* Import the configuration for this build */
#include "m68kconf.h"


/* ======================================================================== */
/* ============================ GENERAL DEFINES =========================== */
/* ======================================================================== */

/* There are 7 levels of interrupt to the 68K.
 * A transition from < 7 to 7 will cause a non-maskable interrupt (NMI).
 */
#define M68K_IRQ_NONE 0
#define M68K_IRQ_1    1
#define M68K_IRQ_2    2
#define M68K_IRQ_3    3
#define M68K_IRQ_4    4
#define M68K_IRQ_5    5
#define M68K_IRQ_6    6
#define M68K_IRQ_7    7


/* Special interrupt acknowledge values.
 * Use these as special returns from the interrupt acknowledge callback
 * (specified later in this header).
 */

/* Causes an interrupt autovector (0x18 + interrupt level) to be taken.
 * This happens in a real 68K if VPA or AVEC is asserted during an interrupt
 * acknowledge cycle instead of DTACK.
 */
#define M68K_INT_ACK_AUTOVECTOR    0xffffffff

/* Causes the spurious interrupt vector (0x18) to be taken
 * This happens in a real 68K if BERR is asserted during the interrupt
 * acknowledge cycle (i.e. no devices responded to the acknowledge).
 */
#define M68K_INT_ACK_SPURIOUS      0xfffffffe

/* Registers used by m68k_get_reg() and m68k_set_reg() */
typedef enum
{
	/* Real registers */
	M68K_REG_D0,		/* Data registers */
	M68K_REG_D1,
	M68K_REG_D2,
	M68K_REG_D3,
	M68K_REG_D4,
	M68K_REG_D5,
	M68K_REG_D6,
	M68K_REG_D7,
	M68K_REG_A0,		/* Address registers */
	M68K_REG_A1,
	M68K_REG_A2,
	M68K_REG_A3,
	M68K_REG_A4,
	M68K_REG_A5,
	M68K_REG_A6,
	M68K_REG_A7,
	M68K_REG_PC,		/* Program Counter */
	M68K_REG_SR,		/* Status Register */
	M68K_REG_SP,		/* The current Stack Pointer (located in A7) */
	M68K_REG_USP,		/* User Stack Pointer */
	M68K_REG_ISP,		/* Interrupt Stack Pointer */
	M68K_REG_MSP,		/* Master Stack Pointer */
//	M68K_REG_SFC,		/* Source Function Code */
//	M68K_REG_DFC,		/* Destination Function Code */
//	M68K_REG_VBR,		/* Vector Base Register */
//	M68K_REG_CACR,		/* Cache Control Register */
//	M68K_REG_CAAR,		/* Cache Address Register */

	/* Assumed registers */
	/* These are cheat registers which emulate the 1-longword prefetch
	 * present in the 68000 and 68010.
	 */
//	M68K_REG_PREF_ADDR,	/* Last prefetch address */
//	M68K_REG_PREF_DATA,	/* Last prefetch data */

	/* Convenience registers */
	M68K_REG_PPC,		/* Previous value in the program counter */
	M68K_REG_IR/*,*/		/* Instruction register */
//	M68K_REG_CPU _TYPE	/* Type of MPU being run */
} m68k_register_t;

/* ======================================================================== */
/* ====================== FUNCTIONS CALLED BY THE MPU ===================== */
/* ======================================================================== */

/* You will have to implement these functions */

/* read/write functions called by the MPU to access memory.
 * while values used are 32 bits, only the appropriate number
 * of bits are relevant (i.e. in write_memory_8, only the lower 8 bits
 * of value should be written to memory).
 *
 * NOTE: I have separated the immediate and PC-relative memory fetches
 *       from the other memory fetches because some systems require
 *       differentiation between PROGRAM and DATA fetches (usually
 *       for security setups such as encryption).
 *       This separation can either be achieved by setting
 *       M68K_SEPARATE_READS in m68kconf.h and defining
 *       the read functions, or by setting M68K_EMULATE_FC and
 *       making a function code callback function.
 *       Using the callback offers better emulation coverage
 *       because you can also monitor whether the MPU is in SYSTEM or
 *       USER mode, but it is also slower.
 */

/* Read from anywhere */
extern unsigned char   m68k_read_memory_8(unsigned int address);
extern unsigned short  m68k_read_memory_16(unsigned int address);
//unsigned int  m68k_read_memory_32(unsigned int address);

/* Read data immediately following the PC */
//unsigned int  m68k_read_immediate_16(unsigned int address);
//unsigned int  m68k_read_immediate_32(unsigned int address);

/* Read data relative to the PC */
//unsigned int  m68k_read_pcrelative_8(unsigned int address);
//unsigned int  m68k_read_pcrelative_16(unsigned int address);
//unsigned int  m68k_read_pcrelative_32(unsigned int address);

/* Memory access for the disassembler */
//unsigned int m68k_read_disassembler_8  (unsigned int address);
//unsigned int m68k_read_disassembler_16 (unsigned int address);
//unsigned int m68k_read_disassembler_32 (unsigned int address);

/* Write to anywhere */
extern void m68k_write_memory_8( unsigned int address, unsigned char  value);
extern void m68k_write_memory_16(unsigned int address, unsigned short value);
//void m68k_write_memory_32(unsigned int address, unsigned int value);

/* Special call to simulate undocumented 68k behavior when move.l with a
 * predecrement destination mode is executed.
 * To simulate real 68k behavior, first write the high word to
 * [address+2], and then write the low word to [address].
 *
 * Enable this functionality with M68K_SIMULATE_PD_WRITES in m68kconf.h.
 */
//void m68k_write_memory_32_pd(unsigned int address, unsigned int value);


/* ======================================================================== */
/* ====================== FUNCTIONS TO ACCESS THE MPU ===================== */
/* ======================================================================== */

//void m68k_set_mpu _type(unsigned int mpu _type);

/* Do whatever initialisations the core requires.  Should be called
 * at least once at init time.
 */
void m68k_init(void);

/* Pulse the RESET pin on the MPU.
 * You *MUST* reset the MPU at least once to initialize the emulation
 * Note: If you didn't call m68k_set_mpu _type() before resetting
 *       the MPU for the first time, the MPU will be set to
 *       M68K_MPU _TYPE_68000.
 */
void m68k_pulse_reset(void);

/* execute num_cycles worth of instructions.  returns number of cycles used */
int m68k_execute(int num_cycles);

/* These functions let you read/write/modify the number of cycles left to run
 * while m68k_execute() is running.
 * These are useful if the 68k accesses a memory-mapped port on another device
 * that requires immediate processing by another MPU.
 */
int m68k_cycles_run(void);              /* Number of cycles run so far */
int m68k_cycles_remaining(void);        /* Number of cycles left */
void m68k_modify_timeslice(int cycles); /* Modify cycles left */
void m68k_end_timeslice(void);          /* End timeslice now */

/* Set the IPL0-IPL2 pins on the MPU (IRQ).
 * A transition from < 7 to 7 will cause a non-maskable interrupt (NMI).
 * Setting IRQ to 0 will clear an interrupt request.
 */
void m68k_set_irq(unsigned int int_level);


/* Halt the MPU as if you pulsed the HALT pin. */
void m68k_pulse_halt(void);


/* Context switching to allow multiple MPUs */

/* Get the size of the mpu context in bytes */
unsigned int m68k_context_size(void);

/* Get a mpu context */
unsigned int m68k_get_context(void* dst);

/* set the current mpu context */
void m68k_set_context(void* dst);

/* Register the MPU state information */
void m68k_state_register(const char *type);


/* Peek at the internals of a MPU context.  This can either be a context
 * retrieved using m68k_get_context() or the currently running context.
 * If context is NULL, the currently running MPU context will be used.
 */
unsigned int m68k_get_reg(void* context, m68k_register_t reg);

/* Poke values into the internals of the currently running MPU context */
void m68k_set_reg(m68k_register_t reg, unsigned int value);

/* Check if an instruction is valid for the specified MPU type */
//unsigned int m68k_is_valid_instruction(unsigned int instruction, unsigned int cpu _type);

/* Disassemble 1 instruction using the epecified MPU type at pc.  Stores
 * disassembly in str_buff and returns the size of the instruction in bytes.
 */
//unsigned int m68k_disassemble(char* str_buff, unsigned int pc, unsigned int cpu _type);


/* ======================================================================== */
/* ============================== MAME STUFF ============================== */
/* ======================================================================== */

#if M68K_COMPILE_FOR_MAME == OPT_ON
#include "m68kmame.h"
#endif /* M68K_COMPILE_FOR_MAME */



/*----the compatibles old type------*/
/* look at the internals of the MPU */
int m68k_peek_dr(int reg_num);
int m68k_peek_ar(int reg_num);
unsigned int m68k_peek_pc(void);
unsigned int m68k_peek_ppc(void);
int m68k_peek_sr(void);
int m68k_peek_ir(void);
int m68k_peek_t1_flag(void);
int m68k_peek_t0_flag(void);
int m68k_peek_s_flag(void);
int m68k_peek_m_flag(void);
int m68k_peek_int_mask(void);
int m68k_peek_x_flag(void);
int m68k_peek_n_flag(void);
int m68k_peek_z_flag(void);
int m68k_peek_v_flag(void);
int m68k_peek_c_flag(void);
int m68k_peek_usp(void);
int m68k_peek_isp(void);
int m68k_peek_msp(void);

/* poke values into the internals of the MPU */
void m68k_poke_dr(int reg_num, int value);
void m68k_poke_ar(int reg_num, int value);
void m68k_poke_pc(unsigned int value);
void m68k_poke_sr(int value);
void m68k_poke_ir(int value);
void m68k_poke_t1_flag(int value);
void m68k_poke_t0_flag(int value);
void m68k_poke_s_flag(int value);
void m68k_poke_m_flag(int value);
void m68k_poke_int_mask(int value);
void m68k_poke_x_flag(int value);
void m68k_poke_n_flag(int value);
void m68k_poke_z_flag(int value);
void m68k_poke_v_flag(int value);
void m68k_poke_c_flag(int value);
void m68k_poke_usp(int value);
void m68k_poke_isp(int value);
void m68k_poke_msp(int value);



//static unsigned char/*int*/ m68ki_cycles/*[]*/[0x10000] __attribute__((aligned(64))) ;

/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */

#endif /* M68K__HEADER */
