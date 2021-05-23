#define DOS 1

#ifndef _SHARED_H_
#define _SHARED_H_

#define LSB_FIRST

#include "_clib.h"
#include <stdio.h>
//#include <math.h>
//#include <zlib.h>

//efine MAX_ROM_SIZE (0x00500000) // cart_romのサイズ。 [5MB]以上のROMって無いよな。多分。
#define MAX_ROM_SIZE (0x00600000) // cart_romのサイズ。 Gensが上限[6MB]なんで真似しよ。


#include "types.h"
#include "macros.h"

#ifdef _mpu22
#include "mpu22/m68k.h"
#elif _mpu33
#include "mpu33/m68k.h"
#elif _mpu44
#include "mpu44/m68k.h"
#endif

//#include "mpu/m68k.h"
#ifdef _z01
#include "z01/z80.h"
#elif _z02
#include "z02/z80.h"
#elif _z03
#include "z03/z80.h"
#elif _z04
#include "z04/z80.h"
#elif _z05
#include "z05/z80.h"
#endif

  #include "main_sound.h"
#include "genesis.h"
//#include "render.h"
//#include "memvdp.h"
//#include "vdp.h"
//#include "membnk.h"
//#include "memz80.h"
//#include "mem68k.h"

//#include "system.h"
//#include "unzip.h"
///#include "fileio.h"
#if 00
#include "./load/loadrom.h"
#else
//#include "loadrom.h"
#endif
//#include "io.h"
#include "sound/sound.h"
#include "sound/md_sound.h"
//#include "sound/fm.h"
//#include "sound/sn76496.h"
//#include "osd.h"

#include "_pg.h"
//#include "main_text.h"
//#include "main_sound.h"
//#include "main_sub.h"
#include "psp_main.h"
///#include "zlibInterface.h"

/* TM_1FRAME: 1 flame the virtical timer counts.==16666 */
#define TM_1FRAME  (16666)  // 1 frame time

#define v_sync	sceDisplayWaitVblankStart


#define RGB2COL(rrr,ggg,bbb)  ((((bbb>>3) & 0x1F)<<10)+(((ggg>>3) & 0x1F)<<5)+(((rrr>>3) & 0x1F)<<0)+0x8000)

#define COLOR_BLACK RGB2COL(  0,  0,  0)
#define COLOR_GRAY	RGB2COL( 10, 10, 10)
#define COLOR_HCYAN	RGB2COL(155,155,155)

#define COLOR_RED    RGB2COL(255,  0,  0)
#define COLOR_CYAN   RGB2COL( 55,255,255)
#define COLOR_YELLOW RGB2COL(255,255,  0)
#define COLOR_WHITE  RGB2COL(255,255,255)

#endif /* _SHARED_H_ */

