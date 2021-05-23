#ifndef M68K__HEADER
#define M68K__HEADER

/* ======================================================================== */
/* ========================= LICENSING & COPYRIGHT ======================== */
/* ======================================================================== */
/*
 *                                  MUSASHI
 *                                Version 2.0a
 *
 * A portable Motorola M680x0 processor emulation engine.
 * Copyright 1999 Karl Stenerud.  All rights reserved.
 *
 * This code may be freely used for non-commercial purpooses as long as this
 * copyright notice remains unaltered in the source code and any binary files
 * containing this code in compiled form.
 *
 * Any commercial ventures wishing to use this code must contact the author
 * (Karl Stenerud) to negotiate commercial licensing terms.
 *
 * The latest version of this code can be obtained at:
 * http://milliways.scas.bcit.bc.ca/~karl/musashi
 */

/* ======================================================================== */
/* ============================= CONFIGURATION ============================ */
/* ======================================================================== */

/* Import the configuration for this build */
#include "m68kconf.h"

/* ======================================================================== */
/* ============================ GENERAL DEFINES =========================== */
/* ======================================================================== */

/* There are 7 levels of interrupt to the 68000.  Level 7 cannot me masked */
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


/* MPU types for use in m68k_set_mpu_type() */
//#define M68K_CPU_MODE_68000 1
//#define M68K_CPU_MODE_68010 2
//#define M68K_CPU_MODE_68020 4

/* ======================================================================== */
/* ====================== FUNCTIONS CALLED BY THE MPU ===================== */
/* ======================================================================== */

/* You will have to implement these functions */

/* read/write functions called by the MPU to access memory.
 * while values used are 32 bits, only the appropriate number
 * of bits are relevant (i.e. in write_memory_8, only the lower 8 bits
 * of value should be written to memory).
 * address will be a 24-bit value.
 */

/* Read from anywhere */
unsigned char   m68k_read_memory_8( unsigned int address);
unsigned short  m68k_read_memory_16(unsigned int address);
//unsigned int  m68k_read_memory_32(unsigned int address);

/* Read data immediately following the PC */
//unsigned int  m68k_read_immediate_8( unsigned int address);  //need it
//unsigned int  m68k_read_immediate_16(unsigned int address);
//unsigned int  m68k_read_immediate_32(unsigned int address);

/* Write to anywhere */
void m68k_write_memory_8( unsigned int address, unsigned char  value);
void m68k_write_memory_16(unsigned int address, unsigned short value);
//void m68k_write_memory_32(unsigned int address, unsigned int value);

/* ======================================================================== */
/* ====================== FUNCTIONS TO ACCESS THE MPU ===================== */
/* ======================================================================== */

/* Use this function to set the MPU type ypu want to emulate.
 * Currently supported types are: M68K_CPU_MODE_68000, M68K_CPU_MODE_68010,
 * and M68K_CPU_MODE_68020.
 */
void m68k_set_mpu_mode(int cpu_mode);

/* Reset the MPU as if you asserted RESET */
/* You *MUST* call this at least once to initialize the emulation */
void m68k_pulse_reset(void);//void *param);

/* execute num_clks worth of instructions.  returns number of clks used */
int m68k_execute(int num_clks);

/* The following 2 functions simulate an interrupt controller attached to the
 * MPU since the 68000 needs an interrupt controller attached to work
 * properly.  Valid interrupt lines are 1, 2, 3, 4, 5, 6, and 7 (7 is a non-
 * maskable interrupt)
 */
//void m68k_assert_irq(int int_line);
//void m68k_clear_irq(int int_line);
void m68k_set_irq(unsigned int int_level);

/* Halt the MPU as if you asserted the HALT pin */
void m68k_pulse_halt(void);

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

/* context switching to allow multiple MPUs */
unsigned m68k_get_context(void* dst);
void m68k_set_context(void* dst);

/* check if an instruction is valid for the specified MPU mode */
int m68k_is_valid_instruction(int instruction, int cpu_mode);



/* ======================================================================== */
/* ============================= CONFIGURATION ============================ */
/* ======================================================================== */

/* Import the configuration for this build */
#include "m68kconf.h"



//extern unsigned char/*int*/ m68ki_cycles/*[]*/[0x10000];

/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */

#endif /* M68K__HEADER */
