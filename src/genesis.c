/*
	Copyright (C)1998-2003  Charles Mac Donald.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "shared.h"
//#define Z80_END_TIMESLICE() z80_end_timeslice() ;
#define Z80_END_TIMESLICE() ;

//nt8 cart_rom[0x500000] __attribute__((aligned(64))) ;	/* Cartridge ROM */
       uint8 cart_rom[MAX_ROM_SIZE] __attribute__((aligned(64))) ;	/* Cartridge ROM */
static uint8 work_ram[0x10000]  __attribute__((aligned(64))) ;	/* 68K work RAM */
static uint8	z80ram[0x2000]	__attribute__((aligned(64))) ;	/* Z80 work RAM */

//////////////////////////////
// vdp.c

static uint8 bg_pattern_cache[0x80000] __attribute__((aligned(64))) ;/* Cached and flipped patterns */
static uint8 color_dirty[0x40];			/* 1= This color is dirty */
static uint8 bg_name_dirty[0x800]; 		/* 1= This pattern is dirty */
static uint16 bg_name_list[0x800]; 		/* List of modified pattern indices */

static uint8 vram[0x10000] __attribute__((aligned(64))) ;	/* Video RAM (64Kx8) */
static uint8 cram[0x80];				/* On-chip color RAM (64x9) */
static uint8 vsram[0x80];				/* On-chip vertical scroll RAM (40x11) */
static uint8 sat[0x400];				/* Internal copy of sprite attribute table */
 uint8 vdp_reg[0x20] __attribute__((aligned(64))) ;	/* Internal VDP registers (23x8) */

static uint16 vdp_status;				/* VDP status flags */
static uint16 addr;						/* Address register */
static uint16 addr_latch;				/* Latched A15, A14 of address */
static uint16 buffer;					/* Read buffer */

//static int VDP17_1f;	//Win_X_Pos;	/* inner one object *//*ras.cpp*/
//static int VDP17_80;					/* inner one object *//*ras.cpp*/
//static int VDP18_1f;	//Win_Y_Pos;	/* inner one object *//*ras.cpp*/
//static int VDP18_80;					/* inner one object *//*ras.cpp*/

static uint8* VDP2_scrollabase;			/* Name table A base address */
static uint8* VDP4_scrollbbase;			/* Name table B base address */
static uint8* VDP3_windowbase;			/* Name table W base address */
static uint8* sprite_base;				/* Sprite attribute table base address */

static int H_Pix;
static uint8* VDP13_hscrollbase;		/* Horizontal scroll table base address */
static uint16 sat_base_mask;			/* Base bits of SAT */
static uint16 sat_addr_mask;			/* Index bits of SAT */
static uint16 bg_list_index;			/* # of modified patterns in list */
static uint16 vdp16_BG_y_row_mask_size;	/* Horizontal scroll mask */

static uint32 y_mask;					/* Name table Y-index bits mask */

static  int counter;					/* Raster counter */
static uint16 frame_end;				/* End-of-frame (IRQ line) */
static uint16 VDP_Current_Line;				/* VDP scan line counter */
//static unsigned short VDP_Current_Line;

static uint32	z80bank;		/* Address of Z80 bank window */

static uint8 is_color_dirty;			/* 1= One or more colors have changed */
static uint8 is_border_dirty;			/* 1= The border color has changed */
static uint8 hint_pending;				/* 0= Line  interrupt is pending */
static uint8 vint_pending;				/* 1= Frame interrupt is pending */

//static int v_update;					/* 1= VC was updated by a ctrl or HV read */
static uint8 dma_fill;					/* 1= DMA fill has been requested */
static uint8 code_reg; 					/* Code register */
static uint8 pending;					/* Pending write flag */
static uint8 playfield_shift;			/* Width of planes A, B (in bits) */

static uint8 vdp16_BG_x_col_mask_size;	/* Vertical scroll mask */
static uint8 border;					/* Border color index */
static uint8 VDP12_1;			/* flag */
static uint8 gen_running;		/* genesis running */

static uint8	z80irq; 		/* /IRQ    to Z80 */
static uint8	z80busreq;		/* /BUSREQ from Z80 */
static uint8	z80reset;		/* /RESET  to Z80 */
static uint8	z80busack;		/* /BUSACK to Z80 */
/****/
//t_input input;
	uint32 input_md_pad0;
//typedef struct
//{
static 	void (*io_port0_func_w)(uint8 data);
static 	uint8 (*io_port0_func_r)(void);
//} port_t;
//static port_t io_port[3];
//static port_t io_port0;

static uint8 th;// = 0;/* pad slesh-hold */
static uint8 object_index_count;/* Sprite line buffer data */

/*odd meter */
static int Cycles_M68K;
/*
	io.c
	I/O controller chip emulation
*/

static uint8 io_reg[0x10];



t_bitmap bitmap;
t_snd snd;
static int sound_tbl[262];


/*--------------------------------------------------------------------------*/
/* Input callbacks															*/
/*--------------------------------------------------------------------------*/
#if 00
/* SEGA mkIII / sega master system. */
static uint8 pad_2b_r(void)
{
	uint8 temp = 0x3F;
	if(input_md_pad0 & INPUT_UP)    temp &= ~0x01;
	if(input_md_pad0 & INPUT_DOWN)  temp &= ~0x02;
	if(input_md_pad0 & INPUT_LEFT)  temp &= ~0x04;
	if(input_md_pad0 & INPUT_RIGHT) temp &= ~0x08;
	if(input_md_pad0 & INPUT_B)	   temp &= ~0x10;
	if(input_md_pad0 & INPUT_C)	   temp &= ~0x20;
	return (temp);
}
#endif

static uint8 device_3b_r(void)
{
uint8 temp;
	temp = 0x33;
	if(input_md_pad0 & INPUT_UP)    temp &= ~0x01;
	if(input_md_pad0 & INPUT_DOWN)  temp &= ~0x02;
	if(th)
	{
//		temp |= 0x0c;	/* temp = 0x3f; */
//		temp |= 0x40;
		temp |= 0x4c;
		if(input_md_pad0 & INPUT_LEFT)  temp &= ~0x04;
		if(input_md_pad0 & INPUT_RIGHT) temp &= ~0x08;

		if(input_md_pad0 & INPUT_B)	   temp &= ~0x10;
		if(input_md_pad0 & INPUT_C)	   temp &= ~0x20;
	}
	else
	{
		if(input_md_pad0 & INPUT_A)	   temp &= ~0x10;
		if(input_md_pad0 & INPUT_START) temp &= ~0x20;
	}
	return (temp);
}

static void device_3b_w(uint8 data)
{
	th = (data & 0x40);
}

/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown functions											*/
/*--------------------------------------------------------------------------*/
#define CUSTUMUSASI


///////////////////////////////////////////////



/*--------------------------------------------------------------------------*/
/* Bus controller chip functions											*/
/*--------------------------------------------------------------------------*/

//int gen_busack_r(void)
//{
//	return (z80busack & 1);
//}
//#define GEN_BUSACK_R (unsigned int)z80busack

#if 00
int vdp_int_ack_callback(int int_level)
{
	switch(int_level)
	{
		case 4:
			hint_pending = 0;
			break;

		case 6:
			vdp_status &= ~0x0080;
			vint_pending = 0;
			break;
	}

	return M68K_INT_ACK_AUTOVECTOR;
}
#endif

//memo:%div

/*
	only update window clip on window change (?)
	fix leftmost window/nta render and window bug
	sprite masking isn't right in sonic/micromachines 2, but
	seems ok in galaxy force 2
*/

// TEST
// BlackAura - Make sure longword alignment is active
#define ALIGN_LONG

#ifdef ALIGN_LONG

/* Or change the names if you depend on these from elsewhere.. */
#undef READ_LONG
#undef WRITE_LONG

//

static __inline__ uint32 READ_LONG(void *address)
{
#if 00
	if ((uint32)address & 3)
	{
//#ifdef LSB_FIRST  /* little endian version */
		return ( *((uint8 *)address  ) +
				(*((uint8 *)address+1)<< 8) +
				(*((uint8 *)address+2)<<16) +
				(*((uint8 *)address+3)<<24) );
//#else			  /* big endian version */
//		return ( *((uint8 *)address+3) +
//				(*((uint8 *)address+2)<<8)  +
//				(*((uint8 *)address+1)<<16) +
//				(*((uint8 *)address)  <<24) );
//#endif	/* LSB_FIRST */
	}
	else
#endif
		return *(uint32 *)address;
}

static __inline__ void WRITE_LONG(void *address, uint32 data)
{
#if 1
//test
//#if 00
#if 1
/* need for align(Thunder Force IV, hold up by demo.etc... ) */
	if ((uint32)address & 3)
	{
//#ifdef LSB_FIRST
			*((uint8 *)address  ) =	 data;
			*((uint8 *)address+1) = (data>> 8);
			*((uint8 *)address+2) = (data>>16);
			*((uint8 *)address+3) = (data>>24);
//#else
//			*((uint8 *)address+3) =  data;
//			*((uint8 *)address+2) = (data>>8);
//			*((uint8 *)address+1) = (data>>16);
//			*((uint8 *)address  )   = (data>>24);
//#endif /* LSB_FIRST */
		return;
	}
	else
#endif
		*(uint32 *)address = data;
#else
/* force write align (slow down??) */
			*((uint8 *)address  ) =	 data;
			*((uint8 *)address+1) = (data>> 8);
			*((uint8 *)address+2) = (data>>16);
			*((uint8 *)address+3) = (data>>24);
#endif
}

#endif	/* ALIGN_LONG */

#ifdef ALIGN_LONG

/* Draw a single 8-pixel column */
#define DRAW_COLUMN(ATTR, LINE) \
	atex = atex_table[(ATTR>>13) & 7]; \
	src = (uint32 *)&bg_pattern_cache[(ATTR & 0x1FFF)<<6 | (LINE)]; \
	WRITE_LONG(dst, READ_LONG(src) | atex); dst++; src++; \
	WRITE_LONG(dst, READ_LONG(src) | atex); dst++; src++; \
	ATTR>>= 16; \
	atex = atex_table[(ATTR>>13) & 7]; \
	src = (uint32 *)&bg_pattern_cache[(ATTR & 0x1FFF)<<6 | (LINE)]; \
	WRITE_LONG(dst, READ_LONG(src) | atex); dst++; src++; \
	WRITE_LONG(dst, READ_LONG(src) | atex); dst++; src++;

#else

/* Draw a single 8-pixel column */
#define DRAW_COLUMN(ATTR, LINE) \
	atex = atex_table[(ATTR>>13) & 7]; \
	src = (uint32 *)&bg_pattern_cache[(ATTR & 0x1FFF)<<6 | (LINE)]; \
	*dst++ = (*src++ | atex); \
	*dst++ = (*src++ | atex); \
	ATTR>>= 16; \
	atex = atex_table[(ATTR>>13) & 7]; \
	src = (uint32 *)&bg_pattern_cache[(ATTR & 0x1FFF)<<6 | (LINE)]; \
	*dst++ = (*src++ | atex); \
	*dst++ = (*src++ | atex);

#endif /* ALIGN_LONG */

/*
	gcc complains about this:
		*lb++ = table[(*lb<<8) |(*src++ | palette)];
	.. claiming the result on lb is undefined.
	So we manually advance lb and use constant offsets into the line buffer.
*/
#if 1
/* 多分ＰＳＰではこっちの方が速い（測定してないよ）*/
#define DRAW_SPRITE_TILE \
	lb[0] = table[(lb[0]<<8) |(src[0] | palette)]; \
	lb[1] = table[(lb[1]<<8) |(src[1] | palette)]; \
	lb[2] = table[(lb[2]<<8) |(src[2] | palette)]; \
	lb[3] = table[(lb[3]<<8) |(src[3] | palette)]; \
	lb[4] = table[(lb[4]<<8) |(src[4] | palette)]; \
	lb[5] = table[(lb[5]<<8) |(src[5] | palette)]; \
	lb[6] = table[(lb[6]<<8) |(src[6] | palette)]; \
	lb[7] = table[(lb[7]<<8) |(src[7] | palette)]

#else
#define DRAW_SPRITE_TILE \
	lb[0] = table[(lb[0]<<8) |(*src++ | palette)]; \
	lb[1] = table[(lb[1]<<8) |(*src++ | palette)]; \
	lb[2] = table[(lb[2]<<8) |(*src++ | palette)]; \
	lb[3] = table[(lb[3]<<8) |(*src++ | palette)]; \
	lb[4] = table[(lb[4]<<8) |(*src++ | palette)]; \
	lb[5] = table[(lb[5]<<8) |(*src++ | palette)]; \
	lb[6] = table[(lb[6]<<8) |(*src++ | palette)]; \
	lb[7] = table[(lb[7]<<8) |(*src++ | palette)]
#endif

/* Pixel creation macros, input is four bits each */

/* 8:8:8 RGB */
//#define MAKE_PIXEL_32(r,g,b) ((r)<<20 | (g)<<12 | (b)<<4)


/* 5:6:5 RGB */
//#define MAKE_PIXEL_16(r,g,b) ((r)<<12 | (g)<<7 | (b)<<1)

/* 3:3:2 RGB */
//#define MAKE_PIXEL_8(r,g,b)  ((r)<< 5 | (g)<<2 | ((b)>>1))


/* Pixel look-up tables and table base address */
static uint8 *lut[5];
static uint8 *lut_base;

/* Attribute expansion table */
static const uint32 atex_table[] = {
	0x00000000, 0x10101010, 0x20202020, 0x30303030,
	0x40404040, 0x50505050, 0x60606060, 0x70707070
};

/* Sprite name look-up table */
static uint8 name_lut[0x400] __attribute__((aligned(64))) ;

/* Line buffers (need align) */
static uint8 tmp_buf[0x400];	/* Temporary buffer */
static uint8  bg_buf[0x400];	/* Merged background buffer */
//atic uint8 nta_buf[0x400];	/* Plane A / Window line buffer */
//atic uint8 ntb_buf[0x400];	/* Plane B line buffer */
static uint8 nab_buf[0x800];	/* [Plane A / Window line buffer] & [Plane B line buffer] */
static uint8 obj_buf[0x400];	/* Object layer line buffer */
#define OFS_BBB 0x400

/* 15-bit pixel remapping data */
static uint16 pixel_15[0x100];
static uint16 pixel_15_lut[3][0x200];


//	VRAMっぽい？？？？→これはラスターバッファじゃろ。
// メガドラ色でレンダーするから、あとでpixel_15_lutで変換する。
// その為直接レンダーせんから、ラスターのバッファってとこじゃないのかな。
static uint8	lut_base_buf[(LUT_MAX * LUT_SIZE) + LUT_SIZE];


/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown routines											*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* Window rendering 														*/
/*--------------------------------------------------------------------------*/
/* Clip data */
/* Clip structure */
typedef struct
{
	uint8 left;
	uint8 right;
	uint8 enable;
	uint8 dummy;
}clip_t;
static clip_t clip[2];

INLINE void window_clip(int line)
{
	/* Window size and invert flags */
//	int vp;// = VDP18_1f; //(vdp_reg[18] & 0x1F);
//	int vf = VDP18_80; //(vdp_reg[18]>>7) & 1;

	/* Display size  */
	int sw = (VDP12_1) ? 20 : 16;

	/* Clear clipping data */
	//my_memset0(&clip, sizeof(clip));
	clip[0].left=0;
	clip[0].right=0;
	clip[0].enable=0;
	clip[1].left=0;
	clip[1].right=0;
	clip[1].enable=0;

//	vp=(vdp_reg[18] & 0x1F);
	/* Check if line falls within window range */
//	if(VDP18_80 == (line >= (vp<<3)))
	if(((vdp_reg[18]>>7) & 1) == (line >= (((vdp_reg[18] & 0x1F))<<3)))
	{
		/* Window takes up entire line */
		clip[1].right  = sw;
		clip[1].enable = 1;
	}
	else
	{
		/* Perform horizontal clipping; the results are applied in reverse
		   if the horizontal inversion flag is set */
		int a;
		int w;
	int hp;// = VDP17_1f; //(vdp_reg[17] & 0x1F);
//	int hf = VDP17_80; //(vdp_reg[17]>>7) & 1;
		hp =(vdp_reg[17] & 0x1F);

//		a = VDP17_80;
		a =	((vdp_reg[17]>>7)&1);
		w = a^1;
		if(hp)
		{
			if(hp > sw)
			{
				/* Plane W takes up entire line */
				clip[w].right  = sw;
				clip[w].enable = 1;
			}
			else
			{
				/* Window takes left side, Plane A takes right side */
			//	clip[w].left   = 0;
				clip[w].right  = hp;
				clip[a].left   = hp;
				clip[a].right  = sw;
				clip[0].enable = 1;
				clip[1].enable = 1;
			}
		}
		else
		{
			/* Plane A takes up entire line */
			clip[a].right  = sw;
			clip[a].enable = 1;
		}
	}
}



struct {
	uint16 ypos;
	uint16 xpos;
	uint16 attr;
	uint8 size;
	uint8 index;
}object_info[20];/* Sprite line buffer data */

INLINE void render_obj( uint8 *buf, uint8 *table)
{
	uint16 ypos;
	uint16 attr;
	uint16 xpos;
//	uint8 sizetab[] = {8, 16, 24, 32};
	uint8 size;
	uint8 *src;

	int count;
//	int pixellimit;
	int pixelcount;
	int width;
	int height;
	int v_line;
	int column;
	int sol_flag;


	uint8 *s, *lb;
	uint16 name, index;
	uint8 palette;

	int attr_mask, nt_row;

//	pixellimit = H_Pix;
	pixelcount = 0;
	sol_flag = 0;


	if(object_index_count == 0) return;

	for(count = 0; count < object_index_count; count++)
	{
		size = object_info[count].size & 0x0f;
		xpos = object_info[count].xpos;
		xpos &= 0x1ff;

//		width = sizetab[(size>>2) & 3];
		width = (((size<<1) & 0x18)+0x08);
//		width = (((size+size) & 0x18)+0x08);	// 上と全く同じコードを吐く

/*       --.. .---        add1
 8:0x08: 0000 1000     001 <- 00
16:0x10: 0001 0000     010 <- 01
24:0x18: 0001 1000     011 <- 10
32:0x20: 0010 0000     100 <- 11
*/
		if(xpos != 0) sol_flag = 1;
		else
		if(xpos == 0 && sol_flag) return;

		if(pixelcount > H_Pix) return;
		pixelcount += width;

//	int left ;
//	int right ;
//	left = 0x80;
//	right = 0x80 + (H_Pix);

		if(((xpos + width) >= 0x80) && (xpos < (0x80 + (H_Pix))))
		{
			ypos = object_info[count].ypos;
			ypos &= 0x1ff;

			attr = object_info[count].attr;
			attr_mask = (attr & 0x1800);

		//	height = sizetab[size & 3];
			height = (((size<<(1+2)) & 0x18)+0x08);
			palette = (attr>>9) & 0x70;

			v_line = (VDP_Current_Line - ypos);
			nt_row = (v_line>>3) & 3;
			v_line = (v_line & 7)<<3;

			name = (attr & 0x07FF);
			s = &name_lut[((attr>>3) & 0x300) | (size<<4) | (nt_row<<2)];

			lb = (uint8 *)&buf[0x20 + (xpos - 0x80)];

			width>>= 3;
			for(column = 0; column < width; column += 1, lb+=8)
			{
				index = attr_mask | ((name + s[column]) & 0x07FF);
				src = &bg_pattern_cache[(index<<6) | (v_line)];
				DRAW_SPRITE_TILE;
			}
		}
	}
}

INLINE uint16 get_hscrolla(void)
{
uint16 rrrr;
	switch(vdp_reg[11] & 3)
	{
		default:
//		case 0: /* Full-screen */
			rrrr = *(uint16 *)&VDP13_hscrollbase[0];
			break;

		case 1: /* First 8 lines */
			rrrr = *(uint16 *)&VDP13_hscrollbase[     ((VDP_Current_Line  & 7)<<2)];
			break;

		case 2: /* Every 8 lines */
			rrrr = *(uint16 *)&VDP13_hscrollbase[     ((VDP_Current_Line  & ~7)<<2)];
			break;

		case 3: /* Every line */
			rrrr = *(uint16 *)&VDP13_hscrollbase[     (VDP_Current_Line <<2) ];
			break;
	}
	rrrr &= 0x03ff;
	return rrrr;
}
INLINE uint16 get_hscrollb(void)
{
uint16 rrrr;
	switch(vdp_reg[11] & 3)
	{
		default:
//		case 0: /* Full-screen */
			rrrr = *(uint16 *)&VDP13_hscrollbase[2];
			break;

		case 1: /* First 8 lines */
			rrrr = *(uint16 *)&VDP13_hscrollbase[ 2 + ((VDP_Current_Line  & 7)<<2)];
			break;

		case 2: /* Every 8 lines */
			rrrr = *(uint16 *)&VDP13_hscrollbase[ 2 + ((VDP_Current_Line  & ~7)<<2)];
			break;

		case 3: /* Every line */
			rrrr = *(uint16 *)&VDP13_hscrollbase[ 2 + (VDP_Current_Line <<2) ];
			break;
	}
	rrrr &= 0x03ff;
	return rrrr;
}
/*--------------------------------------------------------------------------*/
/* Background plane rendering												*/
/*--------------------------------------------------------------------------*/
INLINE void render_ntx(int which)//, uint8 *buf)
{
	int column;
	int start, end;
	int index;
	int shift;
	int nametable_row_mask ;
	int v_line;
	uint32 atex, atbuf, *src, *dst;
	uint16  xscroll;
	int y_scroll;
	uint32 *nt;
	int vsr_shift;
	uint32 *vs;
	uint8* table;
	uint8* buf;

	buf=nab_buf;	if(which) buf+=OFS_BBB;

	nametable_row_mask = (vdp16_BG_x_col_mask_size>>1);

	table = (which) ? VDP4_scrollbbase : VDP2_scrollabase;

	xscroll = (which) ? get_hscrollb() : get_hscrolla() ;

	shift = (xscroll & 0x0F);
	index = ((vdp16_BG_x_col_mask_size + 1)>>1) - ((xscroll>>4) & nametable_row_mask);

	if(which)
	{
		start = 0;
		end   = (VDP12_1) ? 20 : 16;
	}
	else
	{
// Looks correct if clip[0].left has 1 subtracted
// Otherwise window has gap between endpoint and where the first normal
// nta column starts

		if(clip[0].enable == 0) return;
		start = clip[0].left;
		end   = clip[0].right;
		index = (index + clip[0].left) & nametable_row_mask;
	}

	vsr_shift = (which) ? 16 : 0;
	vs		  = (uint32 *)&vsram[0];

	y_scroll = (vs[0]>>vsr_shift) & 0xFFFF;
	y_scroll = (VDP_Current_Line + (y_scroll & 0x3FF)) & vdp16_BG_y_row_mask_size;
	v_line	 = (y_scroll & 7)<<3;
	nt		 = (uint32 *)&table[ (((y_scroll>>3)<<playfield_shift) & y_mask)];

	if(shift)
	{
		dst   = (uint32 *)&buf[0x20-(0x10-shift)];
		atbuf = nt[(index-1) & nametable_row_mask];
		DRAW_COLUMN(atbuf, v_line)
	}
	buf = (buf + 0x20 + shift);
	dst = (uint32 *)&buf[start<<4];

	for(column = start; column < end; column += 1, index += 1)
	{
		atbuf = nt[index & nametable_row_mask];
		DRAW_COLUMN(atbuf, v_line)
	}
}
INLINE void render_ntx_vs(int which)//, uint8 *buf)
{
	int column;
	int start, end;
	int index;
	int shift;
	int nametable_row_mask ;
	int v_line;
	uint32 atex, atbuf, *src, *dst;
	uint16 xscroll;
	int y_scroll;
	uint32 *nt;
	int vsr_shift;
	uint32 *vs;
	uint8* table;
	uint8* buf;

	buf=nab_buf;	if(which) buf+=OFS_BBB;

	nametable_row_mask = (vdp16_BG_x_col_mask_size>>1);
	table = (which) ? VDP4_scrollbbase : VDP2_scrollabase;

	xscroll = (which) ? get_hscrollb() : get_hscrolla() ;

	shift	= (xscroll & 0x0F);
	index	= ((vdp16_BG_x_col_mask_size + 1)>>1) - ((xscroll>>4) & nametable_row_mask);

	if(which)
	{
		start = 0;
		end   = (VDP12_1) ? 20 : 16;
	}
	else
	{
		if(clip[0].enable == 0) return;
		start = clip[0].left;
		end   = clip[0].right;
		index = (index + clip[0].left) & nametable_row_mask;
	}

	vsr_shift = (which) ? 16 : 0;
	vs		  = (uint32 *)&vsram[0];
	end 	  = (VDP12_1) ? 20 : 16;

	if(shift)
	{
		dst 	 = (uint32 *)&buf[0x20-(0x10-shift)];
		y_scroll = (VDP_Current_Line & vdp16_BG_y_row_mask_size);
		v_line	 = (y_scroll & 7)<<3;
		nt		 = (uint32 *)&table[ (((y_scroll>>3)<<playfield_shift) & y_mask)];
		atbuf	 = nt[(index-1) & nametable_row_mask];
		DRAW_COLUMN(atbuf, v_line)
	}

	buf = (buf + 0x20 + shift);
	dst = (uint32 *)&buf[start<<4];

	for(column = start; column < end; column += 1, index += 1)
	{
		y_scroll = (vs[column]>>vsr_shift) & 0xFFFF;
		y_scroll = (VDP_Current_Line + (y_scroll & 0x3FF)) & vdp16_BG_y_row_mask_size;
		v_line	 = (y_scroll & 7)<<3;
		nt		 = (uint32 *)&table[ (((y_scroll>>3)<<playfield_shift) & y_mask)];
		atbuf	 = nt[index & nametable_row_mask];
		DRAW_COLUMN(atbuf, v_line)
	}
}
/*--------------------------------------------------------------------------*/
/* Line render function 													*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* Helper functions (cache update, hscroll, window clip)					*/
/*--------------------------------------------------------------------------*/
static void update_bg_pattern_cache(void)
{
	int i;
	uint8 x, y, c;
	uint16 name;

	if(!bg_list_index) return;

	for(i=0; i < bg_list_index; i++)
	{
		name = bg_name_list[i];
		bg_name_list[i] = 0;

		for(y=0; y<8; y++)
		{
			if(bg_name_dirty[name] & (1<<y))
			{
				uint8 *dst = &bg_pattern_cache[name<<6];
				uint32 bp = *(uint32 *)&vram[(name<<5) | (y<<2)];

				for(x = 0; x < 8; x++)
				{
					c = (bp>>((x ^ 3)<<2)) & 0x0F;
					dst[0x00000 | ( y	  <<3) | (x	   )] = (c);
					dst[0x20000 | ( y	  <<3) | (x ^ 7)] = (c);
					dst[0x40000 | ((y ^ 7)<<3) | (x	   )] = (c);
					dst[0x60000 | ((y ^ 7)<<3) | (x ^ 7)] = (c);
				}
			}
		}
		bg_name_dirty[name] = 0;
	}
	bg_list_index = 0;
}




static void _my_memset(void* d,  int s, unsigned int n)
{
//	for (; n>0; n--) *(((char *)d)++)=s;
	if( (unsigned int)d & 3 || n & 3 ){
	char  a;
	char* p;
		a=s;
		p=d;
		while(n--){*p=a; p++;}
	}else{
	unsigned int  a;
	unsigned int* p;
		a=s+(s<<8)+(s<<16)+(s<<24);
		p=d;
		n>>=2;
		while(n--){*p=a; p++;}
	}
}
/*--------------------------------------------------------------------------*/
/* Merge functions															*/
/*--------------------------------------------------------------------------*/
static void mergew(uint8 *srca, uint8 *srcb, uint8 *dst, uint8 *table )
{
	int i;
	for(i=0; i< /*width*/( H_Pix); i++)
	{
		dst[i] = table[(srcb[i]<<8) | (srca[i])];
	}
}
INLINE void render_ntw_nta( void)//uint8 *buf)
{
	int column, v_line, width;
	uint32 *nt, *src, *dst, atex, atbuf;

	width  = (VDP12_1) ? 7 : 6;

	nt	= (uint32 *)&VDP3_windowbase[ ((VDP_Current_Line>>3)<<width)];
	dst = (uint32 *)&nab_buf[0x20 + (clip[1].left<<4)];

	v_line = (VDP_Current_Line & 7)<<3;
	for(column = clip[1].left; column < clip[1].right; column++)
	{
		atbuf = nt[column];
		DRAW_COLUMN(atbuf, v_line)
	}
}
#define BITMAP_VIEWPORT_X 0x20

//extern unsigned char vram_buffer[];
static void render_line(void)
{
	uint8 *lb=tmp_buf;
//	lb = (bitmap.remap) ? tmp_buf
// : &bitmap.data[(VDP_Current_Line * (512*2)/*bitmap.pitch*/)];

	if((vdp_reg[1] & 0x40) == 0x00)
	{
		/* Use the overscan color to clear the screen */
		_my_memset(&lb[BITMAP_VIEWPORT_X], 0x40 | border, H_Pix/*bitmap.viewport.w*/);
	}
	else
	{
		update_bg_pattern_cache();
		window_clip(VDP_Current_Line);

		if(vdp_reg[11] & 4)
		{
			render_ntx_vs(0);//, nta_buf);
			render_ntx_vs(1);//, ntb_buf);
		}
		else
		{
			render_ntx(0);//, nta_buf);
			render_ntx(1);//, ntb_buf);
		}
		render_ntw_nta();

		if(vdp_reg[12] & 8)
		{
			mergew(&nab_buf[0x20], &nab_buf[OFS_BBB+0x20], &bg_buf[0x20], lut[2]);
			my_memset0(&obj_buf[0x20], H_Pix);

			render_obj(     obj_buf, lut[3]);

			mergew(&obj_buf[0x20], &bg_buf[0x20], &lb[0x20], lut[4]);
		}
		else
		{
			mergew(&nab_buf[0x20], &nab_buf[OFS_BBB+0x20], &lb[0x20], lut[0]);
			render_obj(     lb, lut[1]);
		}
	}

	if(vdp_reg[0] & 0x20)
	{
		_my_memset(&lb[BITMAP_VIEWPORT_X], 0x40 | border, 0x08);
	}

	//if(bitmap.remap)
	{
//	remap_16(lb+0x20, (uint16 *)out, pixel_15, H_Pix);
uint8 *src;
uint16 *dst;
	int count;
src=lb+0x20;
//dst=(uint16 *)&bitmap.data[(VDP_Current_Line * (512*2)/*bitmap.pitch*/) + (BITMAP_VIEWPORT_X * 2/*bitmap.granularity*/ )];
//dst=(uint16 *)&vram_buffer[(VDP_Current_Line * (512*2)/*bitmap.pitch*/) + (BITMAP_VIEWPORT_X * 2/*bitmap.granularity*/ )];
//dst=(uint16 *) ((void*)0x04100000) +(VDP_Current_Line * (512*2)/*bitmap.pitch*/) + (BITMAP_VIEWPORT_X * 2/*bitmap.granularity*/ );
dst=(uint16 *) ((void*)0x04100000) +(VDP_Current_Line * (512 ) ) + (BITMAP_VIEWPORT_X   );
		for(count=0; count<H_Pix; count++)
		{
			*dst++ = pixel_15[*src++];
		}
	}
}


/*--------------------------------------------------------------------------*/
/* Color update functions													*/
/*--------------------------------------------------------------------------*/
#if 00
static void color_update_15(int index, uint16 data)
{
	if(vdp_reg[12] & 8)
	{
		pixel_15[0x00 | index] = pixel_15_lut[0][data];
		pixel_15[0x40 | index] = pixel_15_lut[1][data];
		pixel_15[0x80 | index] = pixel_15_lut[2][data];
	}
	else
	{
		uint16 temp = pixel_15_lut[1][data];
		pixel_15[0x00 | index] = temp;
		pixel_15[0x40 | index] = temp;
		pixel_15[0x80 | index] = temp;
	}
}
#else
#define color_update_15(index,ddddd)\
{\
uint16 temp_data=ddddd;\
uint16 temp = pixel_15_lut[1][temp_data];\
	if(vdp_reg[12] & 8)\
	{\
		pixel_15[0x00 | index] = pixel_15_lut[0][temp_data];\
		pixel_15[0x40 | index] = temp;\
		pixel_15[0x80 | index] = pixel_15_lut[2][temp_data];\
	}else{\
		pixel_15[0x00 | index] = temp;\
		pixel_15[0x40 | index] = temp;\
		pixel_15[0x80 | index] = temp;\
	}\
}

#endif

/*--------------------------------------------------------------------------*/
/* Object render functions													*/
/*--------------------------------------------------------------------------*/

void parse_sprite_base(int p_line)
{
//	static uint8 sizetab[] = {8, 16, 24, 32};
	uint8 *p, *q, link = 0;
	uint16 ypos;

	int count;
	int height;

	int limit = (VDP12_1) ? 20 : 16;
	int total = (VDP12_1) ? 80 : 64;

	object_index_count = 0;

	for(count = 0; count < total; count++)
	{
		q = &sat[link<<3];
		p = &sprite_base[ (link<<3)];

		ypos = *(uint16 *)&q[0];
		ypos &= 0x1FF;

	//	height = sizetab[q[3] & 3];
		height = (((q[3]<<(1+2)) & 0x18)+0x08);

		if((p_line >= ypos) && (p_line < (ypos + height)))
		{
			object_info[object_index_count].ypos = *(uint16 *)&q[0];
			object_info[object_index_count].xpos = *(uint16 *)&p[6];

			// using xpos from internal sprite_base stops sprite x
			// scrolling in bloodlin.bin,
			// but this seems to go against the test prog
//			object_info[object_index_count].xpos = *(uint16 *)&q[6];
			object_info[object_index_count].attr = *(uint16 *)&p[4];
			object_info[object_index_count].size = q[3];
			object_info[object_index_count].index = count;

			object_index_count += 1;

			if(object_index_count == limit)
			{
				if(vint_pending == 0)
					vdp_status |= 0x40;
				return;
			}
		}

		link = q[2] & 0x7F;
		if(link == 0) break;
	}
}



//////////////////////////////
// memvdp.c
/*
	memvdp.c --
	Memory handlers for when the VDP reads the V-bus during DMA.
*/




//////////////////////////////
// vdp.c


/*
	The vdp_reg[] array is updated at the *end* of this function, so the new
	register data can be compared with the previous data.
*/

static void vdp_reg_w(uint8 r, uint8 d)
{
	switch(r)
	{
		case 0x00: /* CTRL #1 */
			if(hint_pending)
			{
				if(d & 0x10)
				{
					hint_pending = 0;
					m68k_set_irq(4);
				}
				else
				{
					/* Cancel pending level 4 interrupt */
					m68k_set_irq(0);
				}
			}
			break;

		case 0x01: /* CTRL #2 */
			if(vint_pending)
			{
				if(d & 0x20)
				{
					vdp_status &= ~0x0080;
					vint_pending = 0;
					m68k_set_irq(6);
				}
				else
				{
					/* Cancel pending level 6 interrupt */
					m68k_set_irq(0);
				}
			}

			/* Change the frame timing */
			frame_end = (d & 8) ? 0xF0 : 0xE0;

			/* Check if the viewport height has actually been changed */
			if((vdp_reg[1] & 8) != (d & 8))
			{
				/* Update the height of the viewport */
			/*	bitmap.viewport.oh = bitmap.viewport.h;*/
			//	bitmap.viewport.h = (d & 8) ? 240 : 224;
				bitmap./*viewport.*/changed = 1;
			}
			break;

		case 0x02: /* NTAB */ /* scrollabase */
			VDP2_scrollabase = vram + ((d<<10) & 0xE000);
			break;

		case 0x03: /* NTWB */ /* windowbase */
			if(VDP12_1){	VDP3_windowbase = vram + ( (d<<10) & 0xF000);	}
			else{			VDP3_windowbase = vram + ( (d<<10) & 0xF800);	}
			break;

		case 0x04: /* NTBB */ /* scrollbbase */
			VDP4_scrollbbase = vram + ((d<<13) & 0xE000);
			break;

		case 0x05: /* SATB */ /* spritebase */
			sat_base_mask = (VDP12_1) ? 0xFC00 : 0xFE00;
			sat_addr_mask = (VDP12_1) ? 0x03FF : 0x01FF;
			sprite_base   =  vram + ((d<<9) & sat_base_mask);
			break;

		case 0x07: /* bgcolor */
			d &= 0x3F;
			/* Check if the border color has actually changed */
			if(border != d)
			{
				/* Mark the border color as modified */
				is_border_dirty = 1;
				border = d;
				//if(border){
					color_update_15(0x00, *(uint16 *)&cram[(border+border)]);
					color_update_15(0x40, *(uint16 *)&cram[(border+border)]);
					color_update_15(0x80, *(uint16 *)&cram[(border+border)]);
				//}
			}
			break;

/*12*/	case 0x0C:
{
		int old_VDP12;
			old_VDP12=vdp_reg[12];

			/* Check if the viewport width has actually been changed */
			if((old_VDP12 & 0x81) != (d & 0x81))
			{

				//CRam_Flag=1;
				if((0==(d & 0x81)) ){//.HCell_32
		// the vertical mode, force 32 cell.
				VDP12_1=0;
		//			H_Cell=32;
		//			H_Win_Mul=5;
					H_Pix=256;
		//			H_Pix_Begin=32;	// This emulator force 0;
			//		Win_Addr=vram+((VDP_Reg.Pat_Win_Adr<<10)&(0x3e<<10));
					//.HCell_32_ok
//					Spr_Addr=vram+((VDP_Reg.Spr_Att_Adr<<9)&(0x7f<<9));
				//	goto WinH;

				}else{
				VDP12_1=1;
		//			H_Cell=40;
		//			H_Win_Mul=6;
					H_Pix=320;
		//			H_Pix_Begin=0;	// This emulator force 0;
			//		Win_Addr=vram+((VDP_Reg.Pat_Win_Adr<<10)&(0x3c<<10));
					//.HCell_40_ok
//					Spr_Addr=vram+((VDP_Reg.Spr_Att_Adr<<9)&(0x7e<<9));
				//	goto WinH;
				}

				/* Update the width of the viewport */
			/*	bitmap.viewport.ow = bitmap.viewport.w; */
//				bitmap.viewport.w  = H_Pix;
				bitmap./*viewport.*/changed = 1;
			}

			/* See if the S/TE mode bit has changed */
			if((old_VDP12 & 8) != (d & 8))
			{
				int i;
				vdp_reg[12] = d;

				/* Update colors */
				for(i = /*1*/1/*0*/; i < 0x40; i++)
				{
					color_update_15(i, *(uint16 *)&cram[i+i]);
				}
				//color_update_15(0x00, *(uint16 *)&cram[border+border]);
				//color_update_15(0x40, *(uint16 *)&cram[border+border]);
				//color_update_15(0x80, *(uint16 *)&cram[border+border]);

				/* Flush palette */
				is_color_dirty = is_border_dirty = 1;
				_my_memset(color_dirty, 1, 0x40);
			}

			/* Check interlace mode 2 setting */
			//im2_flag = ((d & 0x06) == 0x06) ? 1 : 0;

			/* The following register updates check this value */
			vdp_reg[12] = d;

			/* Update display-dependant registers */
			vdp_reg_w(0x03, vdp_reg[0x03]);
			vdp_reg_w(0x05, vdp_reg[0x05]);
}			break;

/*13*/		case 0x0D: /* HSCB */ /* hscrollbase */
			VDP13_hscrollbase = vram+((d<<10) & 0xFC00);
			break;

/* --rr --ss
     |    00  6  0x1f 0x1fc0
     |    01  7  0x3f 0x1fc0
     |    10  0  0x1f 0x1f80
     |    11  8  0x7f 0x1f00
     00          0x0ff
     01          0x1ff
     10          0x2ff
     11          0x3ff
*/
/*16*/	case 0x10: /* Playfield size */
			{
			/* Tables that define the playfield layout */
//			const static uint8  shift_table[]    = {6, 7, 0, 8};
			const static uint8  shift_table[]    = {6, 7, 7, 8};
//			const static uint8  col_mask_table[] = {0x1F, 0x3F, 0x1F, 0x7F};
			const static uint8  col_mask_table[] = {0x1F, 0x3F, 0x3F, 0x7F};
			const static uint32 y_mask_table[]   = {0x1FC0, 0x1FC0, 0x1F80, 0x1F00};
#if 1
			const static uint16 row_mask_table[] = {0x0FF, 0x1FF, 0x2FF, 0x3FF};
#endif
				playfield_shift				= shift_table[(d & 3)];
				vdp16_BG_x_col_mask_size	= col_mask_table[(d & 3)];
				y_mask						= y_mask_table[(d & 3)];

#if 1
				vdp16_BG_y_row_mask_size	= row_mask_table[(d>>4) & 3];
#else
//	switch( (VDP_Reg.Scr_Size >> 2) & 0x0f )
	switch( (d                >> 2) & 0x0f )
	{
	default:										 //[Gens]
/*	case  0:*/ vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31 0x0ff;//=:= ((32-1)<<8);
//	case  1:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31
//	case  2:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;//  0
//	case  3:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31

	case 13:
	case  4: //vdp16_BG_y_row_mask_size = 0x1ff; break; //  63; break;// 63
	case  5:   vdp16_BG_y_row_mask_size = 0x1ff; break; //  63; break;// 63
//	case  6:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;//  0
//	case  7:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31

//	case  8:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31
//	case  9:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31
//	case 10:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;//  0
//	case 11:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31

	case 12:   vdp16_BG_y_row_mask_size = 0x3ff; break; // 127; break;//127
  //case 13:   vdp16_BG_y_row_mask_size = 0x1ff; break; //  63; break;// 63
//	case 14:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;//  0
//	case 15:   vdp16_BG_y_row_mask_size = 0x0ff; break; //  31; break;// 31
	}
#endif
			}


			break;
//			case 17:
//				VDP17_1f=(d & 0x1F);
//				VDP17_80=(d>>7) & 1;
//			break;
//			case 18:
//				VDP18_1f=(d & 0x1F);
//				VDP18_80=(d>>7) & 1;
//			break;

	}

	/* Write new register value */
	vdp_reg[r] = d;
}


static uint16 vdp_hvc_r(void)
{
	int cycles;
//	uint8 *hctab;
//	int vc;
	int hc;
#if 0
	cycles = m68k_cycles_run();
#else
	cycles=Cycles_M68K;
#endif

//	hctab = (VDP12_1) ? cycle2hc40 : cycle2hc32;
//	hc = hctab[(cycles % 487)];

//#ifdef _USE_VC28_TBL
//	vc = vc28[VDP_Current_Line];
//#else
/* _PSP */
//	vc=VDP_Current_Line;
//	if(0xDA<vc) vc -= 6;/* V counter values for NTSC 192-line display */
//	if(0xEA<vc) vc -= 6;/* V counter values for NTSC 224-line display */
//	vc=(VDP_Current_Line&0xff);/* V counter values for NTSC 240-line display */
//#endif

//	return (((VDP_Current_Line&0xff)<<8) | hc);
#if 00
/* MAME SEGA C2 ver */
			int xpos = cpu_gethorzbeampos();
			int ypos = cpu_getscanline();

			/* adjust for the weird counting rules */
			if (xpos > 0xe9) xpos -= (342 - 0x100);
			if (ypos > 0xea) ypos -= (262 - 0x100);

			/* kludge for ichidant: it sets the H counter to 160, then */
			/* expects that a read from here will return 159 */
			if (ypos > 0) ypos -= 2;
			return (ypos << 8) | xpos;
#endif
/*
■ てきとー（良くない加減）だけどさ、要するに大体こういう事だろ。
テーブル全部、検討したけど、別に測定してどうこうって奴じゃなくって、
多分こうなってるに違いないの一案だよ。あれらは。
あれらのテーブルは要するに、単純な割り算テーブルだ。
■まだワカンナイんだったら、これで十分。
■変数「VDP_Current_Line」は。現在「今の描画ラスター番号」を使用してる。

 */
hc=((cycles+cycles+cycles+cycles+cycles)>>1)&0xff;
	return (((VDP_Current_Line&0xff)<<8) | hc);
}



/* Pack and unpack CRAM data */
#define PACK_CRAM(d)	((((d)&0xE00)>>9)|(((d)&0x0E0)>>2)|(((d)&0x00E)<<5))
#define UNPACK_CRAM(d)	((((d)&0x1C0)>>5)|(((d)&0x038)<<2)|(((d)&0x007)<<9))

/* Mark a pattern as dirty */
#define MARK_BG_DIRTY								\
{													\
	int name = (addr>>5) & 0x7FF; 					\
	if(bg_name_dirty[name] == 0)					\
	{												\
		bg_name_list[bg_list_index] = name; 		\
		bg_list_index += 1; 						\
	}												\
	bg_name_dirty[name] |= (1<<((addr>>2) & 0x07)); \
}

static void vdp_data_w(uint16 data)
{
	/* Clear pending flag */
	pending = 0;

	switch(code_reg & 0x0F)
	{
		case 0x01: /* VRAM */
			/* Byte-swap data if A0 is set */
			if(addr & 1) data = (data>>8) | (data<<8);

			/* Copy SAT data to the internal SAT */
			if((addr & sat_base_mask) == (sprite_base -vram) )
			{
				*(uint16 *)&sat[addr & sat_addr_mask] = data;
			}

			/* Only write unique data to VRAM */
			if(data != *(uint16 *)&vram[addr & 0xFFFE])
			{
				/* Write data to VRAM */
				*(uint16 *)&vram[addr & 0xFFFE] = data;

				/* Update the pattern cache */
				MARK_BG_DIRTY;
			}
			break;

		case 0x03: /* CRAM */
			{
				uint16 *p = (uint16 *)&cram[(addr & 0x7E)];
				data &= 0x0EEE;
				if(data != *p)
				{
					int index = (addr>>1) & 0x3F;
					*p = PACK_CRAM(data);

					if((index & 0x0F) != 0x00)
					{
						color_dirty[index] = is_color_dirty = 1;
					}

					if(index == border)
					{
						is_border_dirty = 1;
						color_update_15(0x00, *p);
						color_update_15(0x40, *p);
						color_update_15(0x80, *p);
					}

					if((index&0x3f)!=0)color_update_15(index, *p);
				}
			}
			break;

		case 0x05: /* VSRAM */
			*(uint16 *)&vsram[(addr & 0x7E)] = data;
			break;
	}

	/* Bump address register */
	addr += vdp_reg[15];

	if(dma_fill)
	{
		int length = (vdp_reg[20]<<8 | vdp_reg[19]) & 0xFFFF;
		if(!length) length = 0x10000;

		do {
			vram[(addr & 0xFFFF)] = (data>>8) & 0xFF;
			MARK_BG_DIRTY;
			addr += vdp_reg[15];
		} while(--length);
		dma_fill = 0;
	}
}



static int gen_io_r(int offset)
{
	switch(offset)
	{
		case 0x00: /* Version */
		{
		uint8 temp;
//	uint8 pal_mode;
/*
 apC- vvvv

a:
p:pal mode

*/
			switch(READgBYTE(cart_rom, 0x0001F0))
			{
				case '1':	case 'J':	temp = 0x00;	break;
				case '2':	case 'U':	temp = 0x80;	break;
				case '4':	case 'E':	temp = 0xC0;	break;
				case '8':				temp = 0x00;	break;
							case 'A':	temp = 0xC0;	break;
							case 'B':	temp = 0x60;	break;//C0->60
//							case '4':	temp = 0x80;	break;
				default:	temp = 0x80;	break;
			}
#define HAS_SCD 0x20/* No Sega CD unit attached */
#define GEN_VER 0x00/* Version 0 hardware */
			return (temp | /*(pal_mode<<6) |*/ HAS_SCD | GEN_VER);
		}
			break;

		case 0x01: /* io Port A Data */
			if(io_port0_func_r) return ((io_reg[offset] & 0x80) | io_port0_func_r());
			return (io_reg[offset] | ((~io_reg[offset+3]) & 0x7F));
#if 00
		case 0x02: /* io Port B Data */
			if(io_port[1].data_r) return ((io_reg[offset] & 0x80) | io_port[1].data_r());
			return (io_reg[offset] | ((~io_reg[offset+3]) & 0x7F));

		case 0x03: /* io Port C Data */
			if(io_port[2].data_r) return ((io_reg[offset] & 0x80) | io_port[2].data_r());
			return (io_reg[offset] | ((~io_reg[offset+3]) & 0x7F));
#endif
	}
	return (io_reg[offset]);
}
/*static*/INLINE unsigned int vdp_dma_r(unsigned int address)
{
	switch( address & (7<<21) )
	{
		case (0<<21): /* Cartridge ROM */
		case (1<<21):
			return READ_WORD(cart_rom, address);

		case (2<<21): /* Unused */
		case (3<<21):
			return 0xFF00;

		case (4<<21): /* Work RAM */
		case (6<<21):
		case (7<<21):
			return READ_WORD(work_ram, address & 0xFFFF);

		case (5<<21): /* Z80 area and I/O chip */
			/* Z80 area always returns $FFFF */
			if(address <= 0xA0FFFF)
			{
				/* Return $FFFF only when the Z80 isn't hogging the Z-bus.
				   (e.g. Z80 isn't reset and 68000 has the bus) */
				return( (z80busack == 0)
					? 0xFFFF
					: READ_WORD(work_ram, address & 0xFFFF));
			}
			else

			/* The I/O chip and work RAM try to drive the data bus which
			   results in both values being combined in random ways when read.
			   We return the I/O chip values which seem to have precedence, */
			if(address <= 0xA1001F)
			{
				uint8 temp = gen_io_r((address>>1) & 0x0F);
				return (temp<<8 | temp);
			}
			else
			{
				/* All remaining locations access work RAM */
				return READ_WORD(work_ram, address & 0xFFFF);
			}
	}

//	return -1;
	return 0xffff;
}
static void dma_vbus(void)
{
	uint32 base, source = ((vdp_reg[23] & 0x7F)<<17 | vdp_reg[22]<<9 | vdp_reg[21]<<1) & 0xFFFFFE;
	uint32 length = (vdp_reg[20]<<8 | vdp_reg[19]) & 0xFFFF;

	if(!length) length = 0x10000;
	base = source;

	do {
		uint16 temp = vdp_dma_r(source);
		source += 2;
		source = ((base & 0xFE0000) | (source & 0x1FFFF));
		vdp_data_w(temp);
	} while (--length);

	vdp_reg[19] = (length>>0) & 0xFF;
	vdp_reg[20] = (length>>8) & 0xFF;

	vdp_reg[21] = (source>>1) & 0xFF;
	vdp_reg[22] = (source>>9) & 0xFF;
	vdp_reg[23] = (vdp_reg[23] & 0x80) | ((source>>17) & 0x7F);
}



/*
	VRAM to VRAM copy
	Read byte from VRAM (source), write to VRAM (addr),
	bump source and add r15 to addr.

	- see how source addr is affected
	  (can it make high source byte inc?)
*/
static void dma_copy(void)
{
	int length = (vdp_reg[20]<<8 | vdp_reg[19]) & 0xFFFF;
	int source = (vdp_reg[22]<<8 | vdp_reg[21]) & 0xFFFF;
	if(!length) length = 0x10000;

	do {
		uint8 temp = vram[source];
		vram[addr] = temp;
		MARK_BG_DIRTY;
		source = (source + 1) & 0xFFFF;
		addr = (addr + vdp_reg[15]) & 0xFFFF;
	} while (--length);

	vdp_reg[19] = (length   ) & 0xFF;
	vdp_reg[20] = (length>>8) & 0xFF;
}

//void vdp_test_w(uint16 value){}
/*--------------------------------------------------------------------------*/
/* Memory access functions													*/
/*--------------------------------------------------------------------------*/

static void vdp_ctrl_w(uint16 data)
{
	if(pending == 0)
	{
		if((data & 0xC000) == 0x8000)
		{
			uint8 r = (data>>8) & 0x1F;
			uint8 d = (data     ) & 0xFF;
			vdp_reg_w(r, d);
		}
		else
		{
			pending = 1;
		}

		addr = ((addr_latch & 0xC000) | (data & 0x3FFF)) & 0xFFFF;
		code_reg = ((code_reg & 0x3C) | ((data>>14) & 0x03)) & 0x3F;
	}
	else
	{
		/* Clear pending flag */
		pending = 0;

		/* Update address and code registers */
		addr = ((addr & 0x3FFF) | ((data & 3 )<<14)) & 0xFFFF;
		code_reg = ((code_reg &   0x03) | ((data>>2) & 0x3C)) & 0x3F;

		/* Save address bits A15 and A14 */
		addr_latch = (addr & 0xC000);

		if((code_reg & 0x20) && (vdp_reg[1] & 0x10))
		{
			switch(vdp_reg[23] & 0xC0)
			{
				case 0x00: /* V bus to VDP DMA */
				case 0x40: /* V bus to VDP DMA */
					dma_vbus();
					break;

				case 0x80: /* VRAM fill */
					dma_fill = 1;
					break;

				case 0xC0: /* VRAM copy */
					dma_copy();
					break;
			}
		}
	}
}

static uint16 vdp_ctrl_r(void)
{
#if 0
	int cycles = m68k_cycles_run();
	uint16 temp = vdp_status;

	/* Clear pending flag */
	pending = 0;

	/* VBlank flag is set when the screen is disabled */
	if((vdp_reg[1] & 0x40) == 0x00)
	{
		temp |= 0x0008;
	}

	/* Clear collision flag on reads */
	vdp_status &= ~0x0020;

	/* Set HBlank flag based on H counter */
	if(VDP12_1)
	{
		int hc = cycle2hc40[(cycles % 487)];
		if((hc >= 0xB6) && (hc <= 0xFF)) temp |= 0x0004;
	}
	else
	{
		int hc = cycle2hc32[(cycles % 487)];
		if((hc >= 0x93) && (hc <= 0xFF)) temp |= 0x0004;
	}

	/* Flag FIFO as empty */
	temp |= 0x0200;

	/* Clear unused bits */
	temp &= 0x03FF;

	return (temp);
#else


//	uint16 temp = (0x4e71 & 0xFC00);
//	pending = 0;
//	vdp_status &= ~0x0020;			// clear sprite hit flag on reads
//	vdp_status |= 0x0200;			// fifo empty
//	vdp_status ^= 0x0004;			/* hack (red zone) */
//	temp |= (vdp_status & 0x03BF);	// clear spr over
//	return (temp);

/* Gens2.12 vdp_io.asm */
unsigned short i;
unsigned short temp;
		i=vdp_status;
		temp=i;
		i ^= 0xFF00;	/* xor */
		i &= 0xFF9F;
		if( (i & 0x0008)  ) {
			i &= 0xFF1F;
		}
//In_VBlank:
		vdp_status=i;
//		test byte [VDP_Reg.Set_2], 0x40;	//VDP[1]
#if 00
		temp=vdp_status;//vdp_status=temp;
#else
/* ??? bug??? */
		vdp_status=temp;
#endif
//		jz short .Display_OFF
		if(  0x40==(vdp_reg[1]  &0xff)){/*Display_OFF:*/ temp|=8; }
//		if(  0x40==(VDP_Reg.Set2&0xff)){/*Display_OFF:*/ temp|=8; }
//

	return (temp);
#endif
}




static uint16 vdp_data_r(void)
{
	uint16 temp = 0;

	/* Clear pending flag */
	pending = 0;

	switch(code_reg & 0x0F)
	{
		case 0x00: /* VRAM */
			temp = *(uint16 *)&vram[(addr & 0xFFFE)];
			break;

		case 0x08: /* CRAM */
			temp = *(uint16 *)&cram[(addr & 0x7E)];
			temp = UNPACK_CRAM(temp);
			break;

		case 0x04: /* VSRAM */
			temp = *(uint16 *)&vsram[(addr & 0x7E)];
			break;
	}

	/* Bump address register */
	addr += vdp_reg[15];

	/* return data */
	return (temp);
}



////////////////////
/// z80_membnk.c

/*
	membnk.c --
	Memory handlers Z80 access to the banked V-bus address space.
*/


/*
	Handlers for access to unused addresses and those which make the
	machine lock up.
*/
//void z80bank_unused_w(int address, int data)
//{
//	error("Z80 bank unused write %06X = %02X (%04X)\n", address, data, z80_get_reg(Z80_PC));
//}

#ifdef MY_00FF
#undef MY_00FF
#endif

//#define MY_00FF
#ifdef MY_00FF
int z80bank_unused_r(int address)
{
//	error("Z80 bank unused read %06X (%04X)\n", address, z80_get_reg(Z80_PC));
	return (address & 1) ? 0x00 : 0xFF;	// original
//	return 0xFF;	// I think force 0xff.
}
#endif

void z80bank_lockup_w(int address, int data)
{
//	error("Z80 bank lockup write %06X = %02X (%04X)\n", address, data, z80_get_reg(Z80_PC));
	gen_running = 0;
	Z80_END_TIMESLICE();
}

int z80bank_lockup_r(int address)
{
//	error("Z80 bank lockup read %06X (%04X)\n", address, z80_get_reg(Z80_PC));
	gen_running = 0;
	Z80_END_TIMESLICE();
	return 0xFF;
}



void gen_z80reset_w(int state)
{
	z80reset = (state & 1);
	z80busack = 1 ^ (z80busreq & z80reset);

	if(z80reset == 0)
	{
		if(snd.enabled)
		{
			YM2612ResetChip(/*0*/);
		}

		z80_reset(/*0*/);

		z80irq = 0;
//		z80_set_irq_callback(z80_irq_callback);
	}
}
/*--------------------------------------------------------------------------*/
/* I/O chip functions														*/
/*--------------------------------------------------------------------------*/
static void gen_io_w(int offset, int value)
{
	switch(offset)
	{
		case 0x01: /* io Port A Data */
			value = ((value & 0x80) | (value & io_reg[offset+3]));
			io_reg[offset] = value;
			if(io_port0_func_w) io_port0_func_w(value);
			return;
#if 00
		case 0x02: /* io Port B Data */
			value = ((value & 0x80) | (value & io_reg[offset+3]));
			io_reg[offset] = value;
			if(io_port[1].data_w) io_port[1].data_w(value);
			return;

		case 0x03: /* io Port C Data */
			value = ((value & 0x80) | (value & io_reg[offset+3]));
			io_reg[offset] = value;
			if(io_port[2].data_w) io_port[2].data_w(value);
			return;
#endif
		case 0x04: /* io Port A Ctrl */
//		case 0x05: /* io Port B Ctrl */
//		case 0x06: /* io Port C Ctrl */
			io_reg[offset] = value & 0xFF;
			break;

#if 00
		case 0x07: /* io Port A TxData */
//		case 0x0A: /* io Port B TxData */
//		case 0x0D: /* io Port C TxData */
			io_reg[offset] = value;
			break;

		case 0x09: /* io Port A S-Ctrl */
//		case 0x0C: /* io Port B S-Ctrl */
//		case 0x0F: /* io Port C S-Ctrl */
			io_reg[offset] = (value & 0xF8);
			break;
#endif
	}
	return;
}

void gen_busreq_w(int state)
{
	z80busreq = (state & 1);
	z80busack = 1 ^ (z80busreq & z80reset);

	if(z80busreq == 0 && z80reset == 1)
	{
		z80_execute(32);
		z80_debt_clk(32);
	}
}
void z80_write_banked_memory(unsigned int address, unsigned int data)
{
//	switch((address>>21) & 7)
	switch((address) & (7<<21))
	{
	//	case (0<<21): /* Cartridge ROM */
	//	case (1<<21):
	//		z80bank_unused_w(address, data);
	//		return;

	//	case (2<<21): /* Unused */
	//	case (3<<21):
	//		z80bank_unused_w(address, data);
	//		return;

		case (4<<21): /* Unused (lockup) */
			z80bank_lockup_w(address, data);
			return;

		case (5<<21): /* Z80, I/O chip, etc. */
			if(address <= 0xA0FFFF)
			{
				z80bank_lockup_w(address, data);
				return;
			}
			else
			{
			//	switch((address>>8) & 0xFF)
				switch((address  ) & (0xFF<<8))
				{
					case (0x00<<8): /* I/O chip */
						if(address <= 0xA1001F)
							gen_io_w((address>>1) & 0x0F, data);
				//		else
				//			z80bank_unused_w(address, data);
						return;

				//	case (0x10<<8): /* DRAM refresh */
				//		z80bank_unused_w(address, data);
				//		return;

					case (0x11<<8): /* /BUSREQ */
						if(address & 1)
							;
				//			z80bank_unused_w(address, data);
						else
							gen_busreq_w(data & 1);
						return;

					case (0x12<<8): /* /RESET (w) */
						if(address & 1)
							;
				//			z80bank_unused_w(address, data);
						else
							gen_z80reset_w(data & 1);
						return;

				//	case (0x13<<8): /* /TIME region */
				//		z80bank_unused_w(address, data);
				//		return;

				//	case (0x20<<8): /* ? */
				//		z80bank_unused_w(address, data);
				//		return;

				//	case (0x30<<8): /* ? */
				//		z80bank_unused_w(address, data);
				//		return;

					default: /* Invalid */
						z80bank_lockup_w(address, data);
						return;
				}
			}
			return;

		case (6<<21): /* VDP */
			z80bank_vdp_w(address, data);
			return;

		case (7<<21): /* Work RAM */
			WRITEgBYTE(work_ram, address & 0xFFFF, data);
			return;
	}
}


int z80_read_banked_memory(unsigned int address)
{
//	switch((address>>21) & 7)
	switch((address) & (7<<21))
	{
		case (0<<21): /* Cartridge ROM */
		case (1<<21):
			return READgBYTE(cart_rom, address);
#ifdef MY_00FF
		case (2<<21): /* Unused */
		case (3<<21):
			return z80bank_unused_r(address);
#endif
		case (4<<21): /* Unused (lockup) */
			return z80bank_lockup_r(address);

		case (5<<21): /* Z80, I/O chip, etc.*/
			if(address <= 0xA0FFFF)
			{
				return z80bank_lockup_r(address);
			}
			else
			{
			//	switch((address>>8) & 0xFF)
				switch((address  ) & (0xFF<<8))
				{
#ifdef MY_00FF
					case (0x00<<8): /* I/O chip */
						if(address <= 0xA1001F)
							return gen_io_r((address>>1) & 0x0F);
						else
							return z80bank_unused_r(address);
						break;

					case (0x10<<8): /* Unused */
						return z80bank_unused_r(address);

					case (0x11<<8): /* /BUSACK from Z80 */
						/* The Z80 can't read this bit (it would be halted
						   when the bit was zero) so we always return '1'. */
						return 0xFF;

					case (0x12<<8): return z80bank_unused_r(address);	/* Unused */
					case (0x13<<8): return z80bank_unused_r(address);	/* /TIME region */
					case (0x20<<8): return z80bank_unused_r(address);	/* Unused */
					case (0x30<<8): return z80bank_unused_r(address);	/* Unused */
#else
/*USED IT*/
					case (0x00<<8): /* I/O chip */
						if(address <= 0xA1001F)
							return gen_io_r((address>>1) & 0x0F);
					case (0x10<<8): /* Unused */
					case (0x11<<8): /* /BUSACK from Z80 */
					case (0x12<<8): /* Unused */
					case (0x13<<8): /* /TIME region */
					case (0x20<<8): /* Unused */
					case (0x30<<8): /* Unused */
						break;	//return 0xFF;
#endif
					default:		return z80bank_lockup_r(address);	/* Lockup */
				}
			}
			break;

		case (6<<21): /* VDP */
			return z80bank_vdp_r(address);

	//	case (7<<21): /* Work RAM - can't be read on some Genesis models (!) */
	//		return 0xFF;
	}

//	return (-1);
	return 0xff;
}


void z80bank_vdp_w(int address, int data)
{
	if((address & 0xE700E0) == 0xC0000)
	{
		switch(address & 0x1F)
		{
			case 0x00: /* Data port */
			case 0x01:
			case 0x02:
			case 0x03:
				vdp_data_w(data<<8 | data);
				return;

			case 0x04: /* Control port */
			case 0x05:
			case 0x06:
			case 0x07:
				vdp_ctrl_w(data<<8 | data);
				return;

			case 0x08: /* Lockup (HVC) */
			case 0x09:
			case 0x0A:
			case 0x0B:
			case 0x0C:
			case 0x0D:
			case 0x0E:
			case 0x0F:
				z80bank_lockup_w(address, data);
				return;

		//	case 0x10: /* Unused */
		//	case 0x12:
		//	case 0x14:
		//	case 0x16:
		//		z80bank_unused_w(address, data);
		//		return;

			case 0x11: /* PSG */
			case 0x13:
			case 0x15:
			case 0x17:
				psg_write(data);
				return;

		//	case 0x18: /* Unused */
		//	case 0x19:
		//	case 0x1A:
		//	case 0x1B:
		//		z80bank_unused_w(address, data);
		//		return;

			case 0x1C: /* Test register */
			case 0x1D:
			case 0x1E:
			case 0x1F:
//??????		//vdp_test_w(data<<8 | data);
				return;
		}
	}
	else
	{
		/* Invalid VDP address */
		z80bank_lockup_w(address, data);
		return;
	}
}


int z80bank_vdp_r(int address)
{
	if((address & 0xE700E0) == 0xC0000)
	{
		switch(address & 0x1F)
		{
			case 0x00: /* Data */
			case 0x02:
				return (vdp_data_r()>>8) & 0xFF;

			case 0x01: /* Data */
			case 0x03:
				return vdp_data_r() & 0xFF;

			case 0x04: /* Control */
			case 0x06:
				return (0xFC | (vdp_ctrl_r()>>8)) & 0xFF;

			case 0x05: /* Control */
			case 0x07:
				return vdp_ctrl_r() & 0xFF;

			case 0x08: /* HVC */
			case 0x0A:
			case 0x0C:
			case 0x0E:
				return (vdp_hvc_r()>>8) & 0xFF;

			case 0x09: /* HVC */
			case 0x0B:
			case 0x0D:
			case 0x0F:
				return vdp_hvc_r() & 0xFF;

			case 0x10: /* Lockup */
			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
			case 0x15:
			case 0x16:
			case 0x17:
				return z80bank_lockup_r(address);
#ifdef MY_00FF
			case 0x18: /* Unused */
			case 0x19:
			case 0x1A:
			case 0x1B:
			case 0x1C:
			case 0x1D:
			case 0x1E:
			case 0x1F:
				return (z80bank_unused_r(address) | 0xFF);
#endif
		}
	}
	else
	{
		/* Invalid VDP address */
		return z80bank_lockup_r(address);
	}

	return 0xFF;
}





/*
	Handlers for access to unused addresses and those which make the
	machine lock up.
*/
//void z80_unused_w(int address, int data)
//{
//	error("Z80 unused write %04X = %02X (%04X)\n", address, data, z80_get_reg(Z80_PC));
//}

//int z80_unused_r(int address)
//{
//	error("Z80 unused read %04X (%04X)\n", address, z80_get_reg(Z80_PC));
//	return 0xFF;
//}
#if 00
void z80_lockup_w(int address, int data)
{
	error("Z80 lockup write %04X = %02X (%04X)\n", address, data, z80_get_reg(Z80_PC));
	gen_running = 0;
	Z80_END_TIMESLICE();
}

int z80_lockup_r(int address)
{
	error("Z80 lockup read %04X (%04X)\n", address, z80_get_reg(Z80_PC));
	gen_running = 0;
	Z80_END_TIMESLICE();
	return 0xFF;
}
#endif
//////////////////////////////
// memz80.c
/*
	memz80.c --
	Memory handlers for Z80 memory and port access, and the Z80 to
	VDP interface.
*/

#define LOG_PORT 0		/* 1= Log Z80 I/O port accesses */


unsigned char/*int*/ z80_readmem16(unsigned short address)
{
	switch(address & 0x0e000) /* 0x0e000 */
	{
		case 0x0000: /* Work RAM */
		case 0x2000:
			return z80ram[address & 0x1FFF];

		case 0x4000: /* YM2612 */
			return fm_read(address & 3);

		case 0x6000: /* VDP */
			if((address & 0xFF00) == 0x7F00)
				return z80_vdp_r(address);
			return 0xFF;

		default: /* V-bus bank */
			return z80_read_banked_memory(z80bank | (address & 0x7FFF));
	}

	return 0xFF;
}

void gen_bank_w(int state)
{
	z80bank = ((z80bank>>1) | ((state & 1)<<23)) & 0xFF8000;
}

void z80_writemem16(unsigned short address, unsigned /*int*/char data )
{
//	switch((address>>13) & 7)
	switch( address & (7<<13) )
	{
		case (0<<13): /* Work RAM */
		case (1<<13):
			z80ram[address & 0x1FFF] = data;
			return;

		case (2<<13): /* YM2612 */
			fm_write(address & 3, data);
			return;

		case (3<<13): /* Bank register and VDP */
			switch(address & 0xFF00)
			{
				case 0x6000:
					gen_bank_w(data & 1);
					return;

				case 0x7F00:
					z80_vdp_w(address, data);
					return;

			//	default:
			//		z80_unused_w(address, data);
			//		return;
			}
			return;

		default: /* V-bus bank */
			z80_write_banked_memory(z80bank | (address & 0x7FFF), data);
			return;
	}
}


int z80_vdp_r(int address)
{
	switch(address & 0xFF)
	{
		case 0x00: /* VDP data port */
		case 0x02:
			return (vdp_data_r()>>8) & 0xFF;

		case 0x01: /* VDP data port */
		case 0x03:
			return (vdp_data_r() & 0xFF);

		case 0x04: /* VDP control port */
		case 0x06:
			return (0xFF | ((vdp_ctrl_r()>>8) & 3));

		case 0x05: /* VDP control port */
		case 0x07:
			return (vdp_ctrl_r() & 0xFF);

		case 0x08: /* HV counter */
		case 0x0A:
		case 0x0C:
		case 0x0E:
			return (vdp_hvc_r()>>8) & 0xFF;

		case 0x09: /* HV counter */
		case 0x0B:
		case 0x0D:
		case 0x0F:
			return (vdp_hvc_r() & 0xFF);

//		case 0x10: /* Unused (PSG) */
//		case 0x11:
//		case 0x12:
//		case 0x13:
//		case 0x14:
//		case 0x15:
//		case 0x16:
//		case 0x17:
//			return z80_lockup_r(address);

//		case 0x18: /* Unused */
//		case 0x19:
//		case 0x1A:
//		case 0x1B:
//			return z80_unused_r(address);

//		case 0x1C: /* Unused (test register) */
//		case 0x1D:
//		case 0x1E:
//		case 0x1F:
//			return z80_unused_r(address);

//		default: /* Invalid VDP addresses */
//			return z80_lockup_r(address);
	}

	return 0xFF;
}


void z80_vdp_w(int address, int data)
{
	switch(address & 0xFF)
	{
		case 0x00: /* VDP data port */
		case 0x01:
		case 0x02:
		case 0x03:
			vdp_data_w(data<<8 | data);
			return;

		case 0x04: /* VDP control port */
		case 0x05:
		case 0x06:
		case 0x07:
			vdp_ctrl_w(data<<8 | data);
			return;

//		case 0x08: /* Unused (HV counter) */
//		case 0x09:
//		case 0x0A:
//		case 0x0B:
//		case 0x0C:
//		case 0x0D:
//		case 0x0E:
//		case 0x0F:
//			z80_lockup_w(address, data);
//			return;

		case 0x11: /* PSG */
		case 0x13:
		case 0x15:
		case 0x17:
			psg_write(data);
			return;

//		case 0x10: /* Unused */
//		case 0x12:
//		case 0x14:
//		case 0x16:
//			z80_unused_w(address, data);

//		case 0x18: /* Unused */
//		case 0x19:
//		case 0x1A:
//		case 0x1B:
//			z80_unused_w(address, data);
//			return;

//		case 0x1C: /* Test register */
//		case 0x1D:
//		case 0x1E:
//		case 0x1F:
//			vdp_test_w(data<<8 | data);
//			return;

//		default: /* Invalid VDP addresses */
//			z80_lockup_w(address, data);
//			return;
	}
}


/*
	Port handlers. Ports are unused when not in Mark III compatability mode.

	Games that access ports anyway:
	- Thunder Force IV reads port $BF in it's interrupt handler.
*/

//unsigned int z80_readport16(unsigned int port)
//{
//#if LOG_PORT
//	error("Z80 read port %04X (%04X)\n", port, z80_get_reg(Z80_PC));
//#endif
//	return 0xFF;
//}

//void z80_writeport16(unsigned int port, unsigned int data)
//{
//#if LOG_PORT
//	error("Z80 write %02X to port %04X (%04X)\n", data, port, z80_get_reg(Z80_PC));
//#endif
//}




//////////////////////////////
// mem68k.c


/*INLINE*/static unsigned int m68k_read_bus_16(unsigned int address)
{
	uint16 temp = 0x4e71;

	if(address >= 0xC00000)
	{
		return (temp);
	}
	else
	{
		return (temp & 0xFF00);
	}
}

/*INLINE*/static unsigned int m68k_read_bus_8(unsigned int address)
{
	uint16 temp = m68k_read_bus_16(address);
	return ((address & 1) ? (temp & 0xFF) : (temp>>8));
}

void m68k_unused_w(unsigned int address, unsigned int value)
{
//error("Unused %08X = %08X (%08X)\n", address, value, Turbo68KReadPC());
}

//void m68k_unused_8_w(unsigned int address, unsigned int value)
//{
//error("Unused %08X = %02X (%08X)\n", address, value, Turbo68KReadPC());
//}

//void m68k_unused_16_w(unsigned int address, unsigned int value)
//{
//error("Unused %08X = %04X (%08X)\n", address, value, Turbo68KReadPC());
//}

/*
	Functions to handle memory accesses which cause the Genesis to halt
	either temporarily (press RESET button to restart) or unrecoverably
	(cycle power to restart).
*/

void m68k_lockup_w_8(unsigned int address, unsigned int value)
{
//	error("Lockup %08X = %02X (%08X)\n", address, value, m68k_get_reg(NULL, M68K_REG_PC));
	gen_running = 0;
#ifdef MUSA33ONLY
	//m68k_end_timeslice();
#endif
}

/* uncommented to HANG-UP emulate. */
//#define NO_HAVE_ERROR
#ifdef NO_HAVE_ERROR

void m68k_lockup_w_16(unsigned int address, unsigned int value)
{
//	error("Lockup %08X = %04X (%08X)\n", address, value, m68k_get_reg(NULL, M68K_REG_PC));
	gen_running = 0;
#ifdef MUSA33ONLY
	//m68k_end_timeslice();
#endif
}

unsigned int m68k_lockup_r_8(unsigned int address)
{
//	error("Lockup %08X.b (%08X)\n", address, m68k_get_reg(NULL, M68K_REG_PC));
	gen_running = 0;
#ifdef MUSA33ONLY
	//m68k_end_timeslice();
#endif
//	return -1;
	return 0xff;	//??
}

unsigned int m68k_lockup_r_16(unsigned int address)
{
//	error("Lockup %08X.w (%08X)\n", address, m68k_get_reg(NULL, M68K_REG_PC));
	gen_running = 0;
#ifdef MUSA33ONLY
	//m68k_end_timeslice();
#endif
//	return -1;
	return 0xffff;	//??
}

#endif	// ends of NO_HAVE_ERROR


/* NEW type was shered handler by Z80 memory from 8 bit between 16 bit. */
//#define Z80_MEMTYPE_NEW
/* NEW type *NOT* need it. it type perfomance down this emulator. */

#ifdef Z80_MEMTYPE_NEW
extern unsigned int cpu_readmem16(unsigned int address);/* real Z80 memory */

/* z80 memory (only 8 bit) */
static unsigned int z80_8read(unsigned int address){
	if(z80busack == 1)
	{
		/* Z80 controls Z bus */
		return (m68k_read_bus_8(address));
	}
	else
	{
#if 00
//	cpu_readmem16(address);			// 68k     be able to used the z80-banking sw?
	cpu_readmem16(address&0x7fff);	// 68k not be able to used the z80-banking sw?
#else
		/* Read data from Z bus */
		switch(address & 0x6000)
		{
			case 0x0000: /* RAM */
			case 0x2000:
				return (z80ram[(address & 0x1FFF)]);

			case 0x4000: /* YM2612 */
				return (fm_read(address & 3));

#ifdef NO_HAVE_ERROR
			case 0x6000: /* Unused */
				switch(address & 0xFF00)
				{
					case 0x7F00: /* VDP */
						m68k_lockup_r_8(address);
					default: /* Unused */
						return (0xFF);
				}
				break;
#endif
		}
		/* z80bank memory sw? */
#endif
	}
	return (0xFF);
}
#endif
/*--------------------------------------------------------------------------*/
/* 68000 memory handlers													*/
/*--------------------------------------------------------------------------*/

unsigned char m68k_read_memory_8(unsigned int address)
{
//	switch((address>>21) & 7)
	switch( address & (7<<21) )
	{
		case (0<<21): /*[0][1][2][3] ROM */
		case (1<<21):
			return READgBYTE(cart_rom, address);

		case (7<<21): /*[e][f] RAM */
			return READgBYTE(work_ram, address & 0xFFFF);

		case (5<<21): /*[a][b] Z80 & I/O */
			if(address <= 0xA0FFFF) /* for Z80 */
			{
#ifdef Z80_MEMTYPE_NEW
				return(z80_8read(address&0xffff));
#else
				if(z80busack == 1)
				{
					/* Z80 controls Z bus */
					return (m68k_read_bus_8(address));
				}
				else
				{
					/* Read data from Z bus */
					switch(address & 0x6000)
					{
						case 0x0000: /* RAM */
						case 0x2000:
							return (z80ram[(address & 0x1FFF)]);

						case 0x4000: /* YM2612 */
							return (fm_read(address & 3));
#ifdef NO_HAVE_ERROR
						case 0x6000: /* Unused */
							switch(address & 0xFF00)
							{
								case 0x7F00: /* VDP */
									m68k_lockup_r_8(address);

								default: /* Unused */
									return (0xFF);
							}
							break;
#endif
					}
				}
#endif
			}
			else /* for I/O */
			{
			//	switch((address>>8) & 0xFF)
				switch( address & (0xFF<<8) )
				{
					case (0x00<<8): /* I/O CHIP */
						if(address <= 0xA1001F)
						{
							return (gen_io_r((address>>1) & 0x0F));
						}
						else
						{
							return (m68k_read_bus_8(address));
						}
						break;

					case (0x10<<8): /* MEMORY MODE */
						return (m68k_read_bus_8(address));

					case (0x11<<8): /* BUSACK */
						if((address & 1) == 0)
						{
							return ((unsigned int)z80busack/*GEN_BUSACK_R*/ | (m68k_read_bus_8(address) & 0xFE));
						}
						else
						return (m68k_read_bus_8(address));

					case (0x12<<8): /* RESET */
						return (m68k_read_bus_8(address));

					case (0x13<<8): /* TIME */
						return (m68k_read_bus_8(address));

					case (0x20<<8): /* UNKNOWN */
						return (m68k_read_bus_8(address));

					case (0x30<<8): /* UNKNOWN */
						return (m68k_read_bus_8(address));
#ifdef NO_HAVE_ERROR
					default: /* Unused */
						return (m68k_lockup_r_8(address));
#endif
				}
			}
			break;

		case (6<<21): /*[c][d] VDP */
			if((address & 0xE700E0) == 0xC00000)
			{
				switch(address & 0x1F)
				{
					case 0x00: /* DATA */
					case 0x02:
						return (vdp_data_r()>>8);

					case 0x01: /* DATA */
					case 0x03:
						return (vdp_data_r() & 0xFF);

					case 0x04: /* CTRL */
					case 0x06:
						return ((m68k_read_bus_8(address) & 0xFC) | (vdp_ctrl_r()>>8));

					case 0x05: /* CTRL */
					case 0x07:
						return (vdp_ctrl_r() & 0xFF);

					case 0x08: /* HVC */
					case 0x0A:
					case 0x0C:
					case 0x0E:
						return (vdp_hvc_r()>>8);

					case 0x09: /* HVC */
					case 0x0B:
					case 0x0D:
					case 0x0F:
						return (vdp_hvc_r() & 0xFF);
#ifdef NO_HAVE_ERROR
					case 0x10: /* PSG */
					case 0x11:
					case 0x12:
					case 0x13:
					case 0x14:
					case 0x15:
					case 0x16:
					case 0x17:
						return (m68k_lockup_r_8(address));
#endif
					case 0x18: /* Unused */
					case 0x19:
					case 0x1A:
					case 0x1B:
					case 0x1C:
					case 0x1D:
					case 0x1E:
					case 0x1F:
						return (m68k_read_bus_8(address));
				}
			}
#ifdef NO_HAVE_ERROR
			else
			{
				/* Unused */
				return (m68k_lockup_r_8(address));
			}
#endif
			break;

		case (2<<21): /*[4][5][6][7] Unused */
		case (3<<21):
			return (m68k_read_bus_8(address));
#ifdef NO_HAVE_ERROR
		case (4<<21): /*[8][9] Unused */
			return (m68k_lockup_r_8(address));
#endif
	}

//	return -1;
	return 0xff;	//??
}



unsigned short m68k_read_memory_16(unsigned int address)
{
//	switch((address>>21) & 7)
	switch( address & (7<<21) )
	{
		case (0<<21): /*[0][1][2][3] ROM */
		case (1<<21):
			return READ_WORD(cart_rom, address);

		case (7<<21): /* RAM */
			return READ_WORD(work_ram, address & 0xFFFF);

		case (5<<21): /* Z80 & I/O */
			if(address <= 0xA0FFFF)/* Z80 */
			{
#ifdef Z80_MEMTYPE_NEW
				return ((z80_8read(address&0xffff)<<8 | z80_8read((address+1)&0xffff) ));
#else
				if(z80busack == 1)
				{
					return (m68k_read_bus_16(address));
				}
				else
				{
					uint8 temp;

					switch(address & 0x6000)
					{
						case 0x0000: /* RAM */
						case 0x2000:
							temp = z80ram[address & 0x1FFF];
							return (temp<<8 | temp);

						case 0x4000: /* YM2612 */
							temp = fm_read(address & 3);
							return (temp<<8 | temp);

#ifdef NO_HAVE_ERROR
						case 0x6000:
							switch(address & 0xFF00)
							{
								case 0x7F00: /* VDP */
									m68k_lockup_r_16(address);

								default: /* Unused */
									return (0xFFFF);
							}
							break;
#endif
					}
				}
#endif
			}
			else/* I/O */
			{
				if(address <= 0xA1001F)
				{
					uint8 temp = gen_io_r((address>>1) & 0x0F);
					return (temp<<8 | temp);
				}
				else
				{
				//	switch((address>>8) & 0xFF)
					switch((address  ) & (0xFF<<8))
					{
						case (0x10<<8): /* MEMORY MODE */
							return (m68k_read_bus_16(address));

						case (0x11<<8): /* BUSACK */
							return ((m68k_read_bus_16(address) & 0xFEFF) | (/*GEN_BUSACK_R*/(unsigned int)z80busack<<8));

						case (0x12<<8): /* RESET */
							return (m68k_read_bus_16(address));

						case (0x13<<8): /* TIME */
							return (m68k_read_bus_16(address));

						case (0x20<<8): /* UNKNOWN */
							return (m68k_read_bus_16(address));

						case (0x30<<8): /* UNKNOWN */
							return (m68k_read_bus_16(address));
#ifdef NO_HAVE_ERROR
						default: /* Unused */
							return (m68k_lockup_r_16(address));
#endif
					}
				}
			}
			break;

		case (6<<21): /*[c][d] VDP */
			if((address & 0xE700E0) == 0xC00000)
			{
				switch(address & 0x1F)
				{
					case 0x00: /* DATA */
					case 0x02:
						return (vdp_data_r());

					case 0x04: /* CTRL */
					case 0x06:
						return (vdp_ctrl_r() | (m68k_read_bus_16(address) & 0xFC00));

					case 0x08: /* HVC */
					case 0x0A:
					case 0x0C:
					case 0x0E:
						return (vdp_hvc_r());
#ifdef NO_HAVE_ERROR
					case 0x10: /* PSG(read) */
					case 0x12:
					case 0x14:
					case 0x16:
						return (m68k_lockup_r_16(address));
#endif
					case 0x18: /* Unused */
					case 0x1A:
					case 0x1C:
					case 0x1E:
						return (m68k_read_bus_16(address));
				}
			}
#ifdef NO_HAVE_ERROR
			else
			{
				return (m68k_lockup_r_16(address));
			}
#endif
			break;

		case (2<<21):
		case (3<<21):
			return (m68k_read_bus_16(address));
#ifdef NO_HAVE_ERROR
		case (4<<21):
			return (m68k_lockup_r_16(address));
#endif
	}

	return (0xA5A5);
}


//unsigned int m68k_read_memory_32(unsigned int address)
//{
//	/* Split into 2 reads */
//	return (m68k_read_memory_16(address + 0)<<16 | m68k_read_memory_16(address + 2));
//}


void m68k_write_memory_8(unsigned int address, unsigned char value)
{
//	switch((address>>21) & 7)
	switch( address & (7<<21) )
	{
		case (7<<21):
			WRITEgBYTE(work_ram, address & 0xFFFF, value);
			return;

		case (6<<21):
			if((address & 0xE700E0) == 0xC00000)
			{
				switch(address & 0x1F)
				{
					case 0x00: /* DATA */
					case 0x01:
					case 0x02:
					case 0x03:
						vdp_data_w(((unsigned short)(value))<<8 | value);
						return;

					case 0x04: /* CTRL */
					case 0x05:
					case 0x06:
					case 0x07:
						vdp_ctrl_w(((unsigned short)(value))<<8 | value);
						return;

					case 0x08: /* HVC */
					case 0x09:
					case 0x0A:
					case 0x0B:
					case 0x0C:
					case 0x0D:
					case 0x0E:
					case 0x0F:
						m68k_lockup_w_8(address, value);
						return;

				//	case 0x10: /* PSG(write) */
				//	case 0x12:
				//	case 0x14:
				//	case 0x16:
				//		//m68k_unused_8_w(address, value);
				//		return;

					case 0x11: /* PSG(write) */
					case 0x13:
					case 0x15:
					case 0x17:
						psg_write(value);
						return;

				//	case 0x18: /* Unused */
				//	case 0x19:
				//	case 0x1A:
				//	case 0x1B:
				//	case 0x1C:
				//	case 0x1D:
				//	case 0x1E:
				//	case 0x1F:
				//		m68k_unused_8_w(address, value);
				//		return;
				}
			}
			else
			{
				m68k_lockup_w_8(address, value);
				return;
			}

		case (5<<21):
			if(address <= 0xA0FFFF)
			{
				if(z80busack == 1)
				{
				//	m68k_unused_8_w(address, value);
					return;
				}
				else
				{
					switch(address & 0x6000)
					{
						case 0x0000:
						case 0x2000:
							z80ram[(address & 0x1FFF)] = value;
							return;

						case 0x4000:
							fm_write(address & 3, value);
							return;

						case 0x6000:
							switch(address & 0xFF00)
							{
								case 0x6000: /* BANK */
									gen_bank_w(value & 1);
									return;

								case 0x7F00: /* VDP */
									m68k_lockup_w_8(address, value);
									return;

							//	default: /* Unused */
							//	//	m68k_unused_8_w(address, value);
							//		return;
							}
							break;
					}
				}
			}
			else
			{
				if(address <= 0xA1001F)
				{
					/* I/O chip only gets /LWR */
					if(address & 1)
						gen_io_w((address>>1) & 0x0F, value);
					return;
				}
				else
				{
					/* Bus control chip registers */
				//	switch((address>>8) & 0xFF)
					switch((address ) & (0xFF<<8))
					{
						case (0x10<<8): /* MEMORY MODE */
						//	m68k_unused_8_w(address, value);
							return;

						case (0x11<<8): /* BUSREQ */
							if((address & 1) == 0)
							{
								gen_busreq_w(value & 1);
							}
						//	else
						//	{
						//	//	m68k_unused_8_w(address, value);
						//	}
							return;

						case (0x12<<8): /* RESET */
							gen_z80reset_w(value & 1);
							return;

					//	case (0x13<<8): /* TIME */
					//	//	m68k_unused_8_w(address, value);
					//		return;

					//	case (0x20<<8): /* UNKNOWN */
					//		m68k_unused_8_w(address, value);
					//		return;

					//	case (0x30<<8): /* UNKNOWN */
					//		m68k_unused_8_w(address, value);
					//		return;

					//	default: /* Unused */
					//		m68k_lockup_w_8(address, value);	// original
					//		return;
					}
				}
			}
			break;


	//	case (0<<21): /* ROM */
	//	case (1<<21): /* ROM */
	//		m68k_unused_8_w(address, value);
	//		return;

	//	case (2<<21): /* Unused */
	//	case (3<<21):
	//		m68k_unused_8_w(address, value);
	//		return;

	//	case (4<<21): /* Unused */
	//		m68k_lockup_w_8(address, value);	// original
	//		return;
	}
	return;
}




void m68k_write_memory_16(unsigned int address, unsigned short value)
{
//	switch((address>>21) & 7)
	switch( address & (7<<21) )
	{
	//	case (0<<21): /* ROM */
	//	case (1<<21): /* ROM */
	//	//	m68k_unused_16_w(address, value);
	//		return;

	//	case (2<<21): /* Unused */
	//	case (3<<21):
	//	//	m68k_unused_16_w(address, value);
	//		return;

	//	case (4<<21): /* Unused */
	//	//	m68k_unused_16_w(address, value);
	//		m68k_lockup_w_16(address, value);	// original
	//		return;

		case (5<<21): /* Z80 area, I/O chip, miscellaneous. */
			if(address <= 0xA0FFFF)
			{
				/* Writes are ignored when the Z80 hogs the Z-bus */
				if(z80busack == 1) {
				//	m68k_unused_8_w(address, value);
					return;
				}

				/* Write into Z80 address space */
				switch(address & 0x6000)
				{
					case 0x0000: /* Work RAM */
					case 0x2000: /* Work RAM */
						z80ram[(address & 0x1FFF)] = (value>>8) & 0xFF;
						return;

					case 0x4000: /* YM2612 */
						fm_write(address & 3, (value>>8) & 0xFF);
						return;

					case 0x6000: /* Bank register and VDP */
						switch(address & 0x7F00)
						{
							case 0x6000: /* Bank register */
								gen_bank_w((value>>8) & 1);
								return;
#ifdef NO_HAVE_ERROR
							 case 0x7F00: /* VDP registers */
								m68k_lockup_w_16(address, value);
								return;
#endif
						//	 default: /* Unused */
						//	//	m68k_unused_8_w(address, value);
						//		return;
						}
						break;
				}
			}
			else
			{
				/* I/O chip */
				if(address <= 0xA1001F)
				{
					gen_io_w((address>>1) & 0x0F, value & 0x00FF);
					return;
				}
				else
				{
					/* Bus control chip registers */
				//	switch((address>>8) & 0xFF)
					switch((address ) & (0xFF<<8))
					{
					//	case (0x10<<8): /* MEMORY MODE */
					//	//	m68k_unused_16_w(address, value);
					//		return;

						case (0x11<<8): /* BUSREQ */
							gen_busreq_w((value>>8) & 1);
							return;

						case (0x12<<8): /* RESET */
							gen_z80reset_w((value>>8) & 1);
							return;

					//	case (0x13<<8): /* TIME */
					//	//	m68k_unused_16_w(address, value);
					//		return;

					//	case (0x20<<8): /* UNKNOWN */
					//	//	m68k_unused_16_w(address, value);
					//		return;

					//	case (0x30<<8): /* UNKNOWN */
					//	//	m68k_unused_16_w(address, value);
					//		return;

					//	default: /* Unused */
					//	//	m68k_unused_16_w(address, value);
					//	//	m68k_lockup_w_16(address, value);	// original
					//		return;
					}
				}
			}
			break;

		case (6<<21): /* VDP */
			if((address & 0xE700E0) == 0xC00000)
			{
				switch(address & 0x1C)
				{
					case 0x00: /* DATA */
						vdp_data_w(value);
						return;

					case 0x04: /* CTRL */
						vdp_ctrl_w(value);
						return;
#ifdef NO_HAVE_ERROR
					case 0x08: /* HV counter */
					case 0x0C: /* HV counter */
						m68k_lockup_w_16(address, value);
						return;
#endif
					case 0x10: /* PSG */
					case 0x14: /* PSG */
						psg_write(value & 0xFF);
						return;

				//	case 0x18: /* Unused */
				//	case 0x1C: /* Unused */
				//		m68k_unused_8_w(address, value);
				//		return;
				}
			}
		//	else
		//	{
		//		/* Invalid address */
		//		m68k_lockup_w_16(address, value);
		//	}
			break;

		case (7<<21): /* Work RAM */
			WRITE_WORD(work_ram, address & 0xFFFF, value);
			return;
	}
	return;
}


//void m68k_write_memory_32(unsigned int address, unsigned int value)
//{
//	/* Split into 2 writes */
//	m68k_write_memory_16(address  ,(value>>16) & 0xFFFF);
//	m68k_write_memory_16(address+2, value      & 0xFFFF);
//}


/*  487*262*60=7655640  */
/*  487=  */
// NTSC genesis master xtal(53.693175[MHz]) if cases the divide 7.5
// 455 // 455.4128498728==(((53693175/7.5)/60)/0x106) // m68000
// 228 // 227.7064249363==(((53693175/15 )/60)/0x106) // z80
#define M68_455 455
#define Z80_228 228
//do_skip 0:draw frame, 1:un-draw frame.
/*int*/void md_run_frame(unsigned char do_skip/*,struct sndinfo *sndi*/)
{
//	if(gen_running == 0)	return 0;
	Cycles_M68K =0;
	/* Clear V-Blank flag */
	vdp_status &= (~0x0008);

	/* Point to start of sound buffer */
	snd.snd_lastStage = snd.snd_curStage  = 0;

	/* Render a line of the display */
	if(!do_skip)
	{
		/* Parse sprites for line 0 (done on line 261) */
		parse_sprite_base(0x80);
	}

//表示画面
///////////
// 262[laster]==0x106
	for(VDP_Current_Line=0; VDP_Current_Line<frame_end; VDP_Current_Line++)
	{
		/* VDP_Current_Line Used by HV counter */

		/* Run 68000 emulation */
		Cycles_M68K +=M68_455;
		m68k_execute(M68_455);

		/* Run Z80 emulation (if enabled) */
		if(z80reset == 1 && z80busreq == 0)
		{
			z80_execute(Z80_228);
		}

		/* If a Z80 interrupt is still pending after a scanline, cancel it */
		if(z80irq == 1)
		{
			z80irq = 0;
			z80_set_irq_line0_off();
		}

		/* Render a line of the display */
		if(!do_skip)
		{
			/*if(VDP_Current_Line <  frame_end   )*/ render_line();
			if(VDP_Current_Line <  frame_end-1 )     parse_sprite_base(0x81 + VDP_Current_Line);
		}

		/* Do H interrupt processing */
	//	if(VDP_Current_Line <= frame_end)
	//	{
			counter--;
			if(counter == -1)
			{
				counter = vdp_reg[10];
				hint_pending = 1;
				if(vdp_reg[0] & 0x10)
				{
					hint_pending = 0;
					m68k_set_irq(4);
				}
			}
	//	}
	//	else
	//	{
	//		counter = vdp_reg[10];
	//	}

		/* Do end of frame processing */
		//

		fm_update_timers();

		snd.snd_curStage = sound_tbl[VDP_Current_Line];
	}

//ＧＥＮＳ2.14はXTAL/7.0  (7/7==1)
//でもこれはXTAL/7.5  (7/7.5=0.933333...)
//つまり約93%にすると。128x0.9333...=119.4666...

//終わりラスター画面
///////////(VDP_Current_Line == frame_end)
	//for(VDP_Current_Line = 0; VDP_Current_Line < 262; VDP_Current_Line++)
	//{
//#define TEST_128 (100)/*hold-up? turbo outrun*/
//#define TEST_128 (119)/*hold-up? turbo outrun*/
  #define TEST_128 (128)/*buggy turbo outrun*/
//#define TEST_128 (132)/*buggy turbo outrun*/
//#define TEST_128 (140)/*buggy turbo outrun*/
//#define TEST_128 (150)/*hold-up turbo outrun*/
#define TEST_256 (TEST_128+TEST_128)

		/* Run 68000 emulation */
		Cycles_M68K +=(M68_455-(TEST_256) );
		m68k_execute(M68_455-(TEST_256) );

		/* Run Z80 emulation (if enabled) */
		if(z80reset == 1 && z80busreq == 0)
		{
			z80_execute(Z80_228-(TEST_128));
		}

		/* If a Z80 interrupt is still pending after a scanline, cancel it */
		if(z80irq == 1)
		{
			z80irq = 0;
			z80_set_irq_line0_off();
		}

		/* Render a line of the display */
	//	if(!do_skip)
	//	{
	//		if(VDP_Current_Line <  frame_end   ) render_line();
	//		if(VDP_Current_Line <  frame_end-1 ) parse_sprite_base(0x81 + VDP_Current_Line);
	//	}

		/* Do H interrupt processing */
	//	if(VDP_Current_Line <= frame_end)
		{
			counter--;
			if(counter == -1)
			{
				counter = vdp_reg[10];
				hint_pending = 1;
				if(vdp_reg[0] & 0x10)
				{
					hint_pending = 0;
					m68k_set_irq(4);
				}
			}
		}
	//	else
	//	{
	//		counter = vdp_reg[10];
	//	}

		/* Do end of frame processing */
	//	if(VDP_Current_Line == frame_end)
		{
			vdp_status |= 0x0088;
			vint_pending = 1;

			/* Give enough time to read the interrupt pending flag before
			   the interrupt actually occurs. */
			Cycles_M68K +=(TEST_256/*16*/);
			m68k_execute(TEST_256/*16*/);

			if(vdp_reg[1] & 0x20)
			{
				vdp_status &= ~0x0080;
				vint_pending = 0;
				m68k_set_irq(6);
			}

			if(z80reset == 1 && z80busreq == 0)
			{
				z80_set_irq_line0_on();
				z80irq = 1;
				z80_execute(TEST_128);
			}
		}
		fm_update_timers();

		snd.snd_curStage = sound_tbl[VDP_Current_Line];
	//}
	VDP_Current_Line++;
#undef TEST_128
#undef TEST_256

//非表示画面
///////////
	for( ; VDP_Current_Line<262; VDP_Current_Line++)
	{

		/* Run 68000 emulation */
		Cycles_M68K +=M68_455;
		m68k_execute(M68_455);
		/* Run Z80 emulation (if enabled) */
		if(z80reset == 1 && z80busreq == 0)
		{
			z80_execute(Z80_228);
		}

		/* If a Z80 interrupt is still pending after a scanline, cancel it */
		if(z80irq == 1)
		{
			z80irq = 0;
			z80_set_irq_line0_off();
		}

		/* Render a line of the display */
	//	if(!do_skip)
	//	{
	//		if(VDP_Current_Line <  frame_end   ) render_line();
	//		if(VDP_Current_Line <  frame_end-1 ) parse_sprite_base(0x81 + VDP_Current_Line);
	//	}

		/* Do H interrupt processing */
	//	if(VDP_Current_Line <= frame_end)
	//	{
	//	}
	//	else
	//	{
			counter = vdp_reg[10];
	//	}

		/* Do end of frame processing */
		////

		fm_update_timers();

		snd.snd_curStage = sound_tbl[VDP_Current_Line];
	}
///////////
#if 00
	if(snd.enabled)
	{
		// Fill the sound buffers
		if(sndi) may_want_to_get_sound(sndi);	// per flame
		//audio_update();
	}
#endif
//	return gen_running;
}








///the externals called function.

void z80_set_irq0(void)
{
	z80irq = 0;
}

//static int z80_irq_callback(int param)
//{
//	z80irq = 0;
//	z80_set_irq_line0_off();
//	return 0xFF;
//}


//////////////////////////////////////////
//////////////////////////////////////////
/*--------------------------------------------------------------------------*/
/* Look-up table functions													*/
/*--------------------------------------------------------------------------*/

/* Input (bx):	d5-d0=color, d6=priority, d7=unused */
/* Input (ax):	d5-d0=color, d6=priority, d7=unused */
/* Output:		d5-d0=color, d6=priority, d7=unused */
static int makeinit_lut_bg(int bx, int ax)
{
	int bf, bp, b;
	int af, ap, a;
	int x = 0;
	int c;

	bf = (bx & 0x7F);
	bp = (bx>>6) & 1;
	b  = (bx & 0x0F);

	af = (ax & 0x7F);
	ap = (ax>>6) & 1;
	a  = (ax & 0x0F);

	c = (ap ? (a ? af : (b ? bf : x)) : \
		(bp ? (b ? bf : (a ? af : x)) : \
		(	  (a ? af : (b ? bf : x)) )));

	/* Strip palette bits from transparent pixels */
	if((c & 0x0F) == 0x00) c = (c & 0xC0);

	return (c);
}
/* Input (bx):	d5-d0=color, d6=priority, d7=sprite pixel marker */
/* Input (sx):	d5-d0=color, d6=priority, d7=unused */
/* Output:		d5-d0=color, d6=zero, d7=sprite pixel marker */
static int makeinit_lut_obj(int bx, int sx)
{
	int bf, bp, bs, b;
	int sf, sp, s;
	int c;

	bf = (bx & 0x3F);
	bs = (bx>>7) & 1;
	bp = (bx>>6) & 1;
	b  = (bx & 0x0F);

	sf = (sx & 0x3F);
	sp = (sx>>6) & 1;
	s  = (sx & 0x0F);

	if(s == 0) return bx;

	if(bs)
	{
		c = bf;
	}
	else
	{
		c = (sp ? (s ? sf : bf)  : \
			(bp ? (b ? bf : (s ? sf : bf)) : \
				  (s ? sf : bf) ));
	}

	/* Strip palette bits from transparent pixels */
	if((c & 0x0F) == 0x00) c = (c & 0xC0);

	return (c | 0x80);
}
/* Input (bx):	d5-d0=color, d6=priority, d7=unused */
/* Input (sx):	d5-d0=color, d6=priority, d7=unused */
/* Output:		d5-d0=color, d6=priority, d7=intensity select (half/normal) */
static int makeinit_lut_bg_ste(int bx, int ax)
{
	int bf, bp, b;
	int af, ap, a;
	int gi;
	int x = 0;
	int c;

	bf = (bx & 0x7F);
	bp = (bx>>6) & 1;
	b  = (bx & 0x0F);

	af = (ax & 0x7F);
	ap = (ax>>6) & 1;
	a  = (ax & 0x0F);

	gi = (ap | bp) ? 0x80 : 0x00;

	c = (ap ? (a ? af : (b ? bf : x )) : \
		(bp ? (b ? bf : (a ? af : x )) : \
		(	  (a ? af : (b ? bf : x )) )));

	c |= gi;

	/* Strip palette bits from transparent pixels */
	if((c & 0x0F) == 0x00) c = (c & 0xC0);

	return (c);
}
/* Input (bx):	d5-d0=color, d6=priority, d7=sprite pixel marker */
/* Input (sx):	d5-d0=color, d6=priority, d7=unused */
/* Output:		d5-d0=color, d6=priority, d7=sprite pixel marker */
static int makeinit_lut_obj_ste(int bx, int sx)
{
	int bf, bs;
	int sf;
	int c;

	bf = (bx & 0x7F);
	bs = (bx>>7) & 1;
	sf = (sx & 0x7F);

	if((sx & 0x0F) == 0) return bx;

	c = (bs) ? bf : sf;

	/* Strip palette bits from transparent pixels */
	if((c & 0x0F) == 0x00) c = (c & 0xC0);

	return (c | 0x80);
}
/* Input (bx):	d5-d0=color, d6=priority, d7=intensity (half/normal) */
/* Input (sx):	d5-d0=color, d6=priority, d7=sprite marker */
/* Output:		d5-d0=color, d6=intensity (half/normal), d7=(double/invalid) */
static int makeinit_lut_bgobj_ste(int bx, int sx)
{
	int c;

	int bf = (bx & 0x3F);
	int bp = (bx>>6) & 1;
	int bi = (bx & 0x80) ? 0x40 : 0x00;
	int b  = (bx & 0x0F);

	int sf = (sx & 0x3F);
	int sp = (sx>>6) & 1;
	int si = (sx & 0x40);
	int s  = (sx & 0x0F);

	if(bi & 0x40) si |= 0x40;

	if(sp)
	{
		if(s)
		{
			if((sf & 0x3E) == 0x3E)
			{
				if(sf & 1)
				{		c = (bf | 0x00);		}
				else{	c = (bx & 0x80) ? (bf | 0x80) : (bf | 0x40);	}
			}
			else
			{
				if(sf == 0x0E || sf == 0x1E || sf == 0x2E)
				{		c = (sf | 0x40);		}
				else{	c = (sf | si);			}
			}
		}
		else
		{
			c = (bf | bi);
		}
	}
	else
	{
		if(bp)
		{
			if(b)
			{					c = (bf | bi);		}
			else
			{
				if(s)
				{
					if((sf & 0x3E) == 0x3E)
					{
						if(sf & 1)
						{		c = (bf | 0x00);						}
						else{	c = (bx & 0x80) ? (bf | 0x80) : (bf | 0x40);	}
					}
					else
					{
						if(sf == 0x0E || sf == 0x1E || sf == 0x2E)
						{		c = (sf | 0x40);				}
						else{	c = (sf | si);					}
					}
				}
				else
				{				c = (bf | bi);		}
			}
		}
		else
		{
			if(s)
			{
				if((sf & 0x3E) == 0x3E)
				{
					if(sf & 1)
					{			c = (bf | 0x00);					}
					else{		c = (bx & 0x80) ? (bf | 0x80) : (bf | 0x40);	}
				}
				else
				{
					if(sf == 0x0E || sf == 0x1E || sf == 0x2E)
					{			c = (sf | 0x40);				}
					else{		c = (sf | si);					}
				}
			}
			else
			{					c = (bf | bi);		}
		}
	}
	if((c & 0x0f) == 0x00) c = (c & 0xC0);
	return (c);
}
/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown functions											*/
/*--------------------------------------------------------------------------*/
static void render_init(void)
{
	int bx, ax, i;

	/* Allocate and align pixel look-up tables */
	lut_base = &lut_base_buf[0];
//	lut_base = malloc((LUT_MAX * LUT_SIZE) + LUT_SIZE);

	lut[0] = (uint8 *)(((uint32)lut_base + LUT_SIZE) & ~(LUT_SIZE - 1));
	for(i=1; i<LUT_MAX; i++)
	{
		lut[i] = lut[0] + (i * LUT_SIZE);
	}

	/* Make pixel look-up table data */
	for(bx = 0; bx < 0x100; bx++){
		for(ax = 0; ax < 0x100; ax++){
			uint16 index = (bx<<8) | (ax);
			lut[0][index] = makeinit_lut_bg(bx, ax);
			lut[1][index] = makeinit_lut_obj(bx, ax);
			lut[2][index] = makeinit_lut_bg_ste(bx, ax);
			lut[3][index] = makeinit_lut_obj_ste(bx, ax);
			lut[4][index] = makeinit_lut_bgobj_ste(bx, ax);
		}
	}

	/* パレットテーブルを予め作成しとく。 */
	/* Make pixel data tables */
	for(i = 0; i < 0x200; i++)
	{
	int r, g, b;
#if 00
		r = (i>>6)&7;	/* red   3bit. 赤にする予定の色、３ビット分 */
		g = (i>>3)&7;	/* green 3bit. 緑にする予定の色、３ビット分 */
		b = (i   )&7;	/* blue  3bit. 青にする予定の色、３ビット分 */
/* 5:5:5 RGB */
#define MAKE_PIXEL_15(r,g,b) ((r)<<1 | (g)<<6 | (b)<<11)
		pixel_15_lut[0][i] = MAKE_PIXEL_15(r,g,b);				/* shadow「影（シャドウ）」機能用、少し暗くする */
		pixel_15_lut[1][i] = MAKE_PIXEL_15(r<<1,g<<1,b<<1);		/* normal普通の色 */
		pixel_15_lut[2][i] = MAKE_PIXEL_15(r|16,g|16,b|16);		/* high-light「ハイライト」機能用、少し明るくする。 */
#undef MAKE_PIXEL_15
#else
// 註記：メガドラＶＤＰの特性＋外部インターフェースの特性両方を考慮しなければならない。
	const int mega_gamma[8][3]={
/* shadow, normal, highlight */
{(1*4)+(3)-1,(1*4)+(3)-1,(1*4)+(3)-1},  //  6   6   6 //  5//(1*2)+(4)-1, //  5// パレット０は特殊
//1*2)+(3)-1,(1*4)+(3)-1,(1*4)+(12)-1// //  - [ 6][15]//この行はメモ
{(2*2)+(3)-1,(2*4)+(3)-1,(2*4)+(10)-1}, //  6  10  17 //  7//(2*2)+(4)-1, //  7
{(3*2)+(2)-1,(3*4)+(2)-1,(3*4)+( 9)-1}, //  7  13  20 //  8//(3*2)+(3)-1, //  8
{(4*2)+(2)-1,(4*4)+(2)-1,(4*4)+( 7)-1}, //  9  17  22 // 10//(4*2)+(3)-1, // 10
{(5*2)+(1)-1,(5*4)+(1)-1,(5*4)+( 5)-1}, // 10  20  24 // 11//(5*2)+(2)-1, // 11
{(6*2)+(1)-1,(6*4)+(1)-1,(6*4)+( 3)-1}, // 12  24  26 // 12//(6*2)+(1)-1, // 12
{(7*2)+(0)-1,(7*4)+(0)-1,(7*4)+( 2)-1}, // 13  27  29 // 13//(7*2)+(1)-1, // 13
{(8*2)+ 0 -1,(8*4)+ 0 -1,(8*4)+  0 -1}  //[15][31][31]// 15//(8*2)+ 0 -1  // 15
};
/* 5:5:5 RGB */
#define MAKE_PIXEL_31(r,g,b) ((r)   | (g)<<5 | (b)<<10)
		r = mega_gamma[(i>>6)&7][0];	/* red   3bit. 赤にする予定の色、３ビット分 */
		g = mega_gamma[(i>>3)&7][0];	/* green 3bit. 緑にする予定の色、３ビット分 */
		b = mega_gamma[(i   )&7][0];	/* blue  3bit. 青にする予定の色、３ビット分 */
		pixel_15_lut[0][i] = MAKE_PIXEL_31(r,g,b);				/* shadow「影（シャドウ）」機能用、少し暗くする */
		r = mega_gamma[(i>>6)&7][1];	/* red   3bit. 赤にする予定の色、３ビット分 */
		g = mega_gamma[(i>>3)&7][1];	/* green 3bit. 緑にする予定の色、３ビット分 */
		b = mega_gamma[(i   )&7][1];	/* blue  3bit. 青にする予定の色、３ビット分 */
//		pixel_15_lut[1][i] = MAKE_PIXEL_31(r<<1,g<<1,b<<1);		/* normal普通の色 */
		pixel_15_lut[1][i] = MAKE_PIXEL_31(r,g,b);				/* shadow「影（シャドウ）」機能用、少し暗くする */
		r = mega_gamma[(i>>6)&7][2];	/* red   3bit. 赤にする予定の色、３ビット分 */
		g = mega_gamma[(i>>3)&7][2];	/* green 3bit. 緑にする予定の色、３ビット分 */
		b = mega_gamma[(i   )&7][2];	/* blue  3bit. 青にする予定の色、３ビット分 */
//		pixel_15_lut[2][i] = MAKE_PIXEL_31(r|16,g|16,b|16);		/* high-light「ハイライト」機能用、少し明るくする。 */
		pixel_15_lut[2][i] = MAKE_PIXEL_31(r,g,b);				/* shadow「影（シャドウ）」機能用、少し暗くする */
	//	/* 但し、パレット０は特別 */
	//	if(((i&7)|((i>>3)&7)|((i>>6)&7))==0){
	//		pixel_15_lut[0][0] =
	//		pixel_15_lut[2][0] = pixel_15_lut[1][0];
	//	}
#endif
 	}

	/* Make sprite name look-up table */
//	make_name_lut();
//static void make_name_lut(void)
{
	int col, row;
	int vcol, vrow;
	int width, height;
	int flipx, flipy;
	int i, name;

	my_memset0(name_lut, sizeof(name_lut));

	for(i = 0; i < 0x400; i += 1)
	{
		vcol = col = i & 3;
		vrow = row = (i>>2) & 3;
		height = (i>>4) & 3;
		width  = (i>>6) & 3;
		flipx  = (i>>8) & 1;
		flipy  = (i>>9) & 1;

		if(flipx)	vcol = (width - col);
		if(flipy)	vrow = (height - row);

		name = vrow + (vcol * (height + 1));

		if((row > height) || col > width)
			name = -1;

		name_lut[i] = name;
	}
}
//	return (1);
}




//void vdp_init(void){}

static void vdp_reset(void)
{
	my_memset0(sat, 	sizeof(sat));
	my_memset0(vram,	sizeof(vram));
	my_memset0(cram,	sizeof(cram));
	my_memset0(vsram,	sizeof(vsram));
	my_memset0(vdp_reg, sizeof(vdp_reg));

	addr = 0;
	addr_latch = 0;
	code_reg = pending = buffer = 0;
	vdp_status = 0;
	VDP2_scrollabase  = vram/*+0*/;
	VDP4_scrollbbase  = vram/*+0*/;
	VDP3_windowbase   = vram/*+0*/;
	sprite_base       = vram/*+0*/;
	VDP13_hscrollbase = vram/*+0*/;
	sat_base_mask = 0xFE00;
	sat_addr_mask = 0x01FF;

	/* Mark all colors as dirty to force a palette update */
	is_color_dirty = 1;
	_my_memset(color_dirty, 1, 0x40);
	border = 0x00;
	is_border_dirty = 1;

	my_memset0(bg_name_dirty, sizeof(bg_name_dirty));
	my_memset0(bg_name_list, sizeof(bg_name_list));
	bg_list_index = 0;
	my_memset0(bg_pattern_cache, sizeof(bg_pattern_cache));

	playfield_shift = 6;
	vdp16_BG_x_col_mask_size = 0x1F;
//	vdp16_BG_y_row_mask_size = 0x0FF;
	vdp16_BG_y_row_mask_size = 0x0ff;//=:= ((32-1)<<8);
	y_mask = 0x1FC0;

	hint_pending = 0;
	vint_pending = 0;
	counter = 0;
	frame_end = 0xE0;
	VDP_Current_Line = 0;
//	v_update  = 0;

	/* Initialize viewport */
	//BITMAP_VIEWPORT_X = 0x20;
	//bitmap.viewport.y = 0x20;
	H_Pix=256;//bitmap.viewport.w = 256;
	//bitmap.viewport.h = 224;
//	bitmap.viewport.ow = 256;
//	bitmap.viewport.oh = 224;
	bitmap./*viewport.*/changed = 1;

	VDP12_1=0;
	H_Pix=256;
}


void system_reset(void)
{
//	gen_reset();
//static void gen_reset(void)
//{
	/* Clear RAM */
	my_memset0(work_ram, sizeof(work_ram));
	my_memset0(  z80ram, sizeof(  z80ram));

	gen_running = 1;
	z80reset	= 0;	/* Z80 is reset */
	z80busreq	= 0;	/* Z80 has control of the Z bus */
	z80busack	= 1;	/* Z80 is busy using the Z bus */
	z80bank		= 0;	/* Assume default bank is 000000-007FFF */
	z80irq		= 0;	/* No interrupts occuring */

//	io_reset();
//static void io_reset(void)
//{
	/* I/O register default settings */
	uint8 io_def[0x10] =
	{
		0xA0,
		0x7F, 0x7F, 0x7F,
		0x00, 0x00, 0x00,
		0xFF, 0x00, 0x00,
		0xFF, 0x00, 0x00,
		0xFB, 0x00, 0x00,
	};

	/* Initialize I/O registers */
	//memcpy(io_reg, io_def, 0x10);
	{int i;
	for(i=0;i<0x10;i++){io_reg[i]=io_def[i];}
	}

	/*
		io Port A : 3B pad
		io Port B : Unused
		io Port C : Unused
	*/
	io_port0_func_w = device_3b_w;
	io_port0_func_r = device_3b_r;
//	io_port[1].data_w = NULL;
//	io_port[1].data_r = NULL;
//	io_port[2].data_w = NULL;
//	io_port[2].data_r = NULL;
//}


	/* Reset the 68000 emulator */
	m68k_pulse_reset();
	//error("PC:%08X\tSP:%08X\n", m68k_get_reg(NULL,M68K_REG_PC), m68k_get_reg(NULL,M68K_REG_SP));
	z80_reset(/*0*/);

//	z80irq = 0;
//	z80_set_irq_callback(z80_irq_callback);
//}

	vdp_reset();
//	render_reset();
//static void render_reset(void)
//{
	my_memset0(&clip, sizeof(clip));

	my_memset0(bg_buf,  sizeof(bg_buf));
	my_memset0(tmp_buf, sizeof(tmp_buf));
//	my_memset0(nta_buf, sizeof(nta_buf));
//	my_memset0(ntb_buf, sizeof(ntb_buf));
	my_memset0(nab_buf, sizeof(nab_buf));
	my_memset0(obj_buf, sizeof(obj_buf));


//	_my_memset(&pixel_15, 0, sizeof(pixel_15));
//	_my_memset(&pixel_15, 12, sizeof(pixel_15));/*test*/

	{int i=0;
		for(i=0;i<(0x100);i++){
			pixel_15[i]=MAKE_PIXEL_31(6,6,6);
		}
	}
//}
	if(snd.enabled)
	{
		YM2612ResetChip(/*0*/);
	}
}

/* for sz11 interface. */
 void *z80_get_mem_ptr(void){
return z80ram;
}
void system_init(void)//メガドラシステム初期化（一回のみ）
{
//	gen_init();
//void gen_init(void)
//{
	sound_init();
	my_memset0(&snd,   sizeof(snd));

//	bswap(cart_rom, MAX_ROM_SIZE);
//static void bswap(uint8 *mem, int length)
{
	int i;
	for(i=0; i<MAX_ROM_SIZE; i+=2)
	{
	uint8 temp;
		temp = cart_rom[i  ];
			   cart_rom[i  ] = cart_rom[i+1];
							   cart_rom[i+1] = temp;
	}
}
#ifndef CUSTUMUSASI
	m68k_set_cpu_type(M68K_CPU_TYPE_68000);
#endif
//	m68k_pulse_reset();
//	error("PC:%08X\tSP:%08X\n", m68k_get_reg(NULL,M68K_REG_PC), m68k_get_reg(NULL,M68K_REG_SP));

	z80_init();//Z80フラグテーブル初期化。
//	gen_running = 1;
//}


//	vdp_init();
	render_init();
//	s_audio_init(/* 44100 */);	/* 15360=(256*60) ??? */
//static int s_audio_init(void)
{
//int rate;
/*
XTAL/7   : 7670453
DGEN100  : 7520000
XTAL/7.5 : 7159090 (==68000 clock)
*/
/* 68000 and YM2612 clock *//* div 7.5? */
/*osakana*/
//#define VCLK   (53693175 / 7  )
/*psp_dgen100*/
//#define VCLK   (7520000L)
//#define VCLK   (7159090)

//rate = 44100;

	/* Z80 and SN76489 clock */

	/* Clear the sound data context */
	my_memset0(&snd, sizeof(t_snd));


//	/* Make sure the requested sample rate is valid */
//	if(!rate || ((rate < 8000) | (rate > 44100)))
//	{
//		Error_mes("sound rate setting error");
//		return (0);
//	}

	/* Calculate the sound buffer size */
	snd.sample_rate =  /*rate*/44100;
	snd.buffer_size = (/*rate*/44100 / 60);

	/* Allocate sound buffers */


	/* Initialize sound chip emulation */
	SN76496_init();//3579545, 100, /*rate*/44100);
//	YM2612Init(1, VCLK, /*rate*/44100, NULL, NULL);
//	YM2612Init(44100);//あとでsystem_reset_value()でやるから要らない。
	/* Set audio enable flag */
	snd.enabled = 1;

	/* Make sound table */
	{int i;
		for (i=0; i<262; i++)
		{
			float p = snd.buffer_size * i;
			p = p / 262;
			sound_tbl[i] = p;
		}
	}
//	return (1);
}

}









#undef MAKE_PIXEL_31


//void gen_shutdown(void)
//{
//	int i;

	/* Print 68K and Z80 state */
	//error("\n");
	//error("\nPC:%08X\tSP:%08X\tSR:%04X\n", m68k_get_reg(NULL,M68K_REG_PC), m68k_get_reg(NULL,M68K_REG_SP), m68k_get_reg(NULL, M68K_REG_SR));
//	for(i=0;i<8;i++)
//	{
//		error("D%d:%08X\tA%d:%08X\n",
//			i, m68k_get_reg(NULL, M68K_REG_D0+i),
//			i, m68k_get_reg(NULL, M68K_REG_A0+i));
//	}

//	error("\n");
//	error("PC:%04X\tSP:%04X\n", z80_get_reg(Z80_PC), z80_get_reg(Z80_SP));
//	error("AF:%04X\tAF:%04X\n", z80_get_reg(Z80_AF), z80_get_reg(Z80_AF2));
//	error("BC:%04X\tBC:%04X\n", z80_get_reg(Z80_BC), z80_get_reg(Z80_BC2));
//	error("DE:%04X\tDE:%04X\n", z80_get_reg(Z80_DE), z80_get_reg(Z80_DE2));
//	error("HL:%04X\tHL:%04X\n", z80_get_reg(Z80_HL), z80_get_reg(Z80_HL2));
//	error("IX:%04X\tIY:%04X\n", z80_get_reg(Z80_IX), z80_get_reg(Z80_IY));
//	error("\n");
//}
//void vdp_shutdown(void){}
//static void render_shutdown(void)
//{
//	  if(lut_base) free(lut_base);
//}
//void system_shutdown(void)
//{
//	gen_shutdown();
//	vdp_shutdown();
//	render_shutdown();
//}
