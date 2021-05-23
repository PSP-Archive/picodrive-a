
#ifndef _GENESIS_H_
#define _GENESIS_H_

#include "shared.h"
/* Global variables */
//tern uint8 cart_rom[0x500000] __attribute__((aligned(64))) ;
extern uint8 cart_rom[MAX_ROM_SIZE] __attribute__((aligned(64))) ;
//extern uint8 work_ram[0x10000] __attribute__((aligned(64))) ;
//extern uint8 z80ram[0x2000] __attribute__((aligned(64))) ;
//extern uint8 z80busreq;
//extern uint8 z80busack;
//extern uint8 z80reset;
//extern uint8 z80irq;
//extern uint32 z80bank;
//extern uint8 gen_running;

/* Function prototypes */
void gen_init(void);
void gen_reset(void);
void gen_shutdown(void);
int gen_busack_r(void);
void gen_busreq_w(int state);
void gen_reset_w(int state);
void gen_bank_w(int state);
//static void bswap(uint8 *mem, int length);
//static int z80_irq_callback(int param);
void m68k_irq_ack_callback(int int_level);

#endif /* _GENESIS_H_ */
#ifndef _IO_H_
#define _IO_H_

/*

void io_reset(void);
void io_set_version(int export, int pal, int fdd, int n);


*/



typedef struct
{
    uint8 state;
} t_input_3b;

/* Global variables */
//extern port_t io_port[3];
//extern uint8 io_reg[0x10];

/* Function prototypes */
//extern void gen_io_w(int offset, int value); /* vdp.c */
//extern int gen_io_r(int offset); /* vdp.c */

//extern uint8 pad_2b_r(void);
//extern uint8 device_3b_r(void);
//extern void device_3b_w(uint8 data);
//extern void io_reset(void);

#endif /* _IO_H_ */


#ifndef _RENDER_H_
#define _RENDER_H_


/* Look-up pixel table information */
#define LUT_MAX         (5)
#define LUT_SIZE        (0x10000)



/* Function prototypes */

//int make_lut_bg(int bx, int ax);
//int make_lut_obj(int bx, int sx);
//int make_lut_bg_ste(int bx, int ax);
//int make_lut_obj_ste(int bx, int sx);
//int make_lut_bgobj_ste(int bx, int sx);

//int render_init(void);

//void render_reset(void);
//void render_shutdown(void);
//void render_line(int line);

//void render_obj(int line, uint8 *buf, uint8 *table);
//void render_obj_im2(int line, uint8 *buf, uint8 *table);
//void render_ntw(int line, uint8 *buf);
//void render_ntw_im2(int line, uint8 *buf);
//void render_ntx(int which, int line, uint8 *buf);
//void render_ntx_im2(int which, int line, uint8 *buf);
//void render_ntx_vs(int which, int line, uint8 *buf);
///void update_bg_pattern_cache(void);

//static void get_hscroll(int line, uint16 *scrolla, uint16 *scrollb);
//void window_clip(int line);

//void remap_8(uint8 *src, uint8 *dst, uint8 *table, int length);
//void remap_16(uint8 *src, uint16 *dst, uint16 *table, int length);
//void remap_32(uint8 *src, uint32 *dst, uint32 *table, int length);

///void merge(uint8 *srca, uint8 *srcb, uint8 *dst, uint8 *table, int width);

//void color_update_8(int index, uint16 data);
//void color_update_15(int index, uint16 data);
//void color_update_16(int index, uint16 data);
//void color_update_32(int index, uint16 data);

///void make_name_lut(void);
///void parse_satb(int line);

#endif /* _RENDER_H_ */


#ifndef _MEMVDP_H_
#define _MEMVDP_H_

/* Function prototypes */
//unsigned int vdp_dma_r(unsigned int address);

#endif /* _MEMVDP_H_ */

#ifndef _VDP_H_
#define _VDP_H_

/* Global variables */


// uint8 vdp_reg[0x20];


//extern uint16 ntab;
//extern uint16 ntbb;
//extern uint16 ntwb;
//extern uint16 satb;
//extern uint16 hscb;
//extern uint8 is_color_dirty;
//extern uint8 color_dirty[0x40];
//extern uint8 border;

//extern uint8 is_border_dirty;
//extern uint8 bg_name_dirty[0x800];
//extern uint16 bg_name_list[0x800];
//extern uint16 bg_list_index;
//extern uint8 bg_pattern_cache[0x80000];
//extern uint8 playfield_shift;
//extern uint8 vdp16_BG_x_col_mask_size;
//extern uint16 vdp16_BG_y_row_mask_size;
//extern uint32 y_mask;

//extern int vint_pending;
//extern int hint_pending;

///extern int scanline;

// extern int counter;
// extern int im2_flag;
// extern int frame_end;
// int v_counter;
//extern int v_update;

//extern void (*color_update)(int index, uint16 data);


/* Function prototypes */
//void vdp_init(void);
//void vdp_reset(void);
//void vdp_shutdown(void);

#if 00
void vdp_ctrl_w(uint16 data);
uint16 vdp_ctrl_r(void);
void vdp_data_w(uint16 data);
uint16 vdp_data_r(void);
void vdp_reg_w(uint8 r, uint8 d);
uint16 vdp_hvc_r(void);
void dma_copy(void);
void dma_vbus(void);
void vdp_test_w(uint16 value);
#endif


#endif /* _VDP_H_ */
//////////////////////////////
// membnk.h

#ifndef _MEMBNK_H_
#define _MEMBNK_H_

/* Function prototypes */
void z80_write_banked_memory(unsigned int address, unsigned int data);
int z80_read_banked_memory(unsigned int address);

void z80bank_vdp_w(int address, int data);
int z80bank_vdp_r(int address);

void z80bank_unused_w(int address, int data);
int z80bank_unused_r(int address);
void z80bank_lockup_w(int address, int data);
int z80bank_lockup_r(int address);

#endif /* _MEMBNK_H_ */

//////////////////////////////
// memz80.h

#ifndef _MEMZ80_H_
#define _MEMZ80_H_

/* Function prototypes */
unsigned char z80_readmem16(unsigned short address);
void z80_writemem16(unsigned short address, unsigned char data);


//unsigned int z80_readport16(unsigned int port);
//void z80_writeport16(unsigned int port, unsigned int data);
void z80_unused_w(int address, int data);
int z80_unused_r(int address);
void z80_lockup_w(int address, int data);
int z80_lockup_r(int address);
int z80_vdp_r(int address);
void z80_vdp_w(int address, int data);

#endif /* _MEMZ80_H_ */

//////////////////////////////
// mem68k.h

#ifndef _MEM68K_H_
#define _MEM68K_H_

/* Function prototypes */
//unsigned int m68k_read_bus_8(unsigned int address);
//unsigned int m68k_read_bus_16(unsigned int address);
void m68k_unused_w(unsigned int address, unsigned int value);

void m68k_lockup_w_8(unsigned int address, unsigned int value);
void m68k_lockup_w_16(unsigned int address, unsigned int value);
unsigned int m68k_lockup_r_8(unsigned int address);
unsigned int m68k_lockup_r_16(unsigned int address);

#endif /* _MEM68K_H_ */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/* Input devices */
#define MAX_DEVICES 		(8) 	/* Unsure of maximum */
#define DEVICE_2BUTTON		(0) 	/* 2-button gamepad */
#define DEVICE_3BUTTON		(1) 	/* 3-button gamepad */
#define DEVICE_6BUTTON		(2) 	/* 6-button gamepad */

/* Input bitmasks */
#define INPUT_MODE		(0x00000800)
#define INPUT_Z 		(0x00000400)
#define INPUT_Y 		(0x00000200)
#define INPUT_X 		(0x00000100)
#define INPUT_START 	(0x00000080)
#define INPUT_C 		(0x00000040)
#define INPUT_B 		(0x00000020)
#define INPUT_A 		(0x00000010)
#define INPUT_LEFT		(0x00000008)
#define INPUT_RIGHT 	(0x00000004)
#define INPUT_DOWN		(0x00000002)
#define INPUT_UP		(0x00000001)

typedef struct
{
	//uint8 *data;		/* Bitmap data */
	//int width;		/*(512) Bitmap width (32+512+32) */
	//int height; 		/*(512) Bitmap height (256) */
//	int depth;			/*15 Color depth (8 bits) */
	//int pitch;		/*(512*2) Width of bitmap in bytes */
//	int granularity;	/*2 Size of each pixel in bytes */
//	int remap;			/* 1= Translate pixel data */
//	struct {
		//int x;			/*0x20 X offset of viewport within bitmap */
		//int y;			/*0x20 Y offset of viewport within bitmap */
		//int w;			/* Width of viewport */
		//int h;			/* Height of viewport */
//		int ow; 		/* Previous width of viewport */
//		int oh; 		/* Previous height of viewport */
		unsigned char changed;	/* 1= Viewport width or height have changed */
//	}viewport;
}t_bitmap;

//typedef struct
//{
//	uint8 dev[MAX_DEVICES]; 	/* Can be any of the DEVICE_* values */
//	uint32 pad[MAX_DEVICES];	/* Can be any of the INPUT_* bitmasks */
//	uint32 system;				/* Can be any of the SYSTEM_* bitmasks */
//}t_input;

typedef struct
{
	int sample_rate;	/* Sample rate (8000-44100) */
	int enabled;		/* 1= sound emulation is enabled */
	int buffer_size;	/* Size of sound buffer (in bytes) */
//	int16 *buffer[2];	/* Signed 16-bit stereo sound data */
	int snd_curStage;
	int snd_lastStage;
}t_snd;

/* Global variables */
extern t_bitmap bitmap;
//extern t_input input;
extern t_snd snd;

/* Function prototypes */
void system_init(void);
/*int*/void md_run_frame(unsigned char skip /*,struct sndinfo *sndi*/);
void system_reset(void);
void system_shutdown(void);
int audio_init(int rate);
void audio_update(void);

#endif /* _SYSTEM_H_ */


