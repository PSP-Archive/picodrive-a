// primitive graphics for Hello World PSP

//#include "_pg.h"

#ifndef NULL
#define NULL  0
#endif

//system call
//void sceDisplayWaitVblankStart();
//void sceDisplaySetMode(long,long,long);
//void sceDisplaySetFrameBuf(char *topaddr,long linesize,long pixelsize,long);

/*Ge*/
//static unsigned int *GEcmd = (unsigned int *)0x441CC000;
//static short *ScreenVertex = (short *)0x441CC100;
//static int gecbid = -1;



//constants

//480*272 = 60*38
#define CMAX_X 60
#define CMAX_Y 38
#define CMAX2_X 30
#define CMAX2_Y 19
#define CMAX4_X 15
#define CMAX4_Y 9


//variables
static unsigned char *pg_vramtop=(unsigned char *)0x04000000;
static long pg_screenmode;
static long pg_showframe;
static long pg_drawframe;


static void* psp_screen_ptr = ((void*)0x04000000) ;

//static void* psp_screen_tmp = ((void*)0x04100000) ;
//static void* PSP_Screen_GetTempBuffer( int x, int y )
//{
//	return psp_screen_tmp + y * (LINESIZE * 2) + x+x ;
//}

static void* PSP_Screen_GetBackBuffer( int x, int y )
{
	return psp_screen_ptr + y * (LINESIZE * 2) + x+x ;
}

static unsigned char *pgGetVramAddr(unsigned long x,unsigned long y)
{
	return pg_vramtop+(pg_drawframe?FRAMESIZE:0)+x*PIXELSIZE*2+y*LINESIZE*2+0x40000000;
//	return pg_vramtop+(pg_drawframe?FRAMESIZE:0)+x*PIXELSIZE*2+y*LINESIZE*2;//+0x40000000;	//•Ï‚í‚ç‚È‚¢‚ç‚µ‚¢
}



/*
void pgPrint(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX_X && y<CMAX_Y) {
		pgPutChar(x*8,y*8,color,0,*str,1,0,1);
		str++;
		x++;
		if (x>=CMAX_X) {
			x=0;
			y++;
		}
	}
}


void pgPrint2(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX2_X && y<CMAX2_Y) {
		pgPutChar(x*16,y*16,color,0,*str,1,0,2);
		str++;
		x++;
		if (x>=CMAX2_X) {
			x=0;
			y++;
		}
	}
}


void pgPrint4(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX4_X && y<CMAX4_Y) {
		pgPutChar(x*32,y*32,color,0,*str,1,0,4);
		str++;
		x++;
		if (x>=CMAX4_X) {
			x=0;
			y++;
		}
	}
}
*/
#if 0
void pgFillvram(unsigned long color)
{
	unsigned long *vr;
	int x,y;
	color+=color<<16;
	vr = (void *)pgGetVramAddr(0,0);
	for(y=0;y<272;y++){
		for(x=0;x<480/2;x++){
			*vr++=color;
		}
		vr+=(LINESIZE-480)/2;
	}
}
#else
static void pgFillvram(unsigned long color)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned long i;

	vptr0=(unsigned char *)pgGetVramAddr(0,0);
	for (i=0; i<FRAMESIZE/2; i++) {
		*(unsigned short *)vptr0=color;
		vptr0+=PIXELSIZE*2;
	}
}

#endif

#if 00
static void pgBitBlt(unsigned long x,unsigned long y,unsigned long w,unsigned long h,unsigned long mag,const unsigned short *d)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	unsigned long xx,yy,mx,my;
	const unsigned short *dd;

	vptr0=pgGetVramAddr(x,y);
	for (yy=0; yy<h; yy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			dd=d;
			for (xx=0; xx<w; xx++) {
				for (mx=0; mx<mag; mx++) {
					*(unsigned short *)vptr=*dd;
					vptr+=PIXELSIZE*2;
				}
				dd++;
			}
			vptr0+=LINESIZE*2;
		}
		d+=w;
	}
}
#endif

/*
void pgPutChar(unsigned long x,unsigned long y,unsigned long color,unsigned long bgcolor,unsigned char ch,char drawfg,char drawbg,char mag)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	const unsigned char *cfont; 	//pointer to font
	unsigned long cx,cy;
	unsigned long b;
	char mx,my;

	if (ch>255) return;
	cfont=font+ch*8;
	vptr0=pgGetVramAddr(x,y);
	for (cy=0; cy<8; cy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			b=0x80;
			for (cx=0; cx<8; cx++) {
				for (mx=0; mx<mag; mx++) {
					if ((*cfont&b)!=0) {
						if (drawfg) *(unsigned short *)vptr=color;
					} else {
						if (drawbg) *(unsigned short *)vptr=bgcolor;
					}
					vptr+=PIXELSIZE*2;
				}
				b=b>>1;
			}
			vptr0+=LINESIZE*2;
		}
		cfont++;
	}
}
*/

static void pgScreenFrame(long mode,long frame)
{
	pg_screenmode=mode;
	frame=(frame?1:0);
	pg_showframe=frame;
	if (mode==0) {
		//screen off
		pg_drawframe=frame;
		sceDisplaySetFrameBuf(0,0,0,1);
	} else if (mode==1) {
		//show/draw same
		pg_drawframe=frame;
		sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	} else if (mode==2) {
		//show/draw different
		pg_drawframe=(frame?0:1);
		sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	}
}


static void pgScreenFlip()
{
	pg_showframe=(pg_showframe?0:1);
	pg_drawframe=(pg_drawframe?0:1);
	sceDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,0);
}

static void pgScreenFlipV()
{
	v_sync();
	pgScreenFlip();
}

/******************************************************************************/
//#define MAXVOLUME 0x8000
/******************************************************************************/

#if 00
void pgErrorHalt(const char *str)
{
/*
	pgScreenFrame(1,0);
	pgcLocate(0,0);
	pgcColor(0xffff,0x0000);
	pgcDraw(1,1);
	pgcSetmag(1);
	pgcPuts(str);
*/
	scePowerSetClockFrequency(222,222,111);
	while (1) { 	v_sync();	}
}
#endif

//void pgiInit()
//{
//	sceCtrlSetSamplingCycle(0);
//	sceCtrlSetSamplingMode(1);
//}




//static unsigned int GEcmd[64];
static unsigned int GEcmd[32] __attribute__((aligned(64))) ;
//////////////////////////////////////////////////////////////////////////////////////////////////
/// Ge
/******************************************************************************/
/* copy from uo_Snes9x for PSP */
/******************************************************************************/
static unsigned int GeInit[] =
{
	0x01000000, 0x02000000,
	0x10000000, 0x12000000, 0x13000000, 0x15000000, 0x16000000, 0x17000000,
	0x18000000, 0x19000000, 0x1A000000, 0x1B000000, 0x1C000000, 0x1D000000,
	0x1E000000, 0x1F000000,
	0x20000000, 0x21000000, 0x22000000, 0x23000000, 0x24000000, 0x25000000,
	0x26000000, 0x27000000, 0x28000000, 0x2A000000, 0x2B000000, 0x2C000000,
	0x2D000000, 0x2E000000, 0x2F000000,
	0x30000000, 0x31000000, 0x32000000, 0x33000000, 0x36000000, 0x37000000,
	0x38000000, 0x3A000000, 0x3B000000, 0x3C000000, 0x3D000000, 0x3E000000,
	0x3F000000,
	0x40000000, 0x41000000, 0x42000000, 0x43000000, 0x44000000, 0x45000000,
	0x46000000, 0x47000000, 0x48000000, 0x49000000, 0x4A000000, 0x4B000000,
	0x4C000000, 0x4D000000,
	0x50000000, 0x51000000, 0x53000000, 0x54000000, 0x55000000, 0x56000000,
	0x57000000, 0x58000000, 0x5B000000, 0x5C000000, 0x5D000000, 0x5E000000,
	0x5F000000,
	0x60000000, 0x61000000, 0x62000000, 0x63000000, 0x64000000, 0x65000000,
	0x66000000, 0x67000000, 0x68000000, 0x69000000, 0x6A000000, 0x6B000000,
	0x6C000000, 0x6D000000, 0x6E000000, 0x6F000000,
	0x70000000, 0x71000000, 0x72000000, 0x73000000, 0x74000000, 0x75000000,
	0x76000000, 0x77000000, 0x78000000, 0x79000000, 0x7A000000, 0x7B000000,
	0x7C000000, 0x7D000000, 0x7E000000, 0x7F000000,
	0x80000000, 0x81000000, 0x82000000, 0x83000000, 0x84000000, 0x85000000,
	0x86000000, 0x87000000, 0x88000000, 0x89000000, 0x8A000000, 0x8B000000,
	0x8C000000, 0x8D000000, 0x8E000000, 0x8F000000,
	0x90000000, 0x91000000, 0x92000000, 0x93000000, 0x94000000, 0x95000000,
	0x96000000, 0x97000000, 0x98000000, 0x99000000, 0x9A000000, 0x9B000000,
	0x9C000000, 0x9D000000, 0x9E000000, 0x9F000000,
	0xA0000000, 0xA1000000, 0xA2000000, 0xA3000000, 0xA4000000, 0xA5000000,
	0xA6000000, 0xA7000000, 0xA8000000, 0xA9000000, 0xAA000000, 0xAB000000,
	0xAC000000, 0xAD000000, 0xAE000000, 0xAF000000,
	0xB0000000, 0xB1000000, 0xB2000000, 0xB3000000, 0xB4000000, 0xB5000000,
	0xB8000000, 0xB9000000, 0xBA000000, 0xBB000000, 0xBC000000, 0xBD000000,
	0xBE000000, 0xBF000000,
	0xC0000000, 0xC1000000, 0xC2000000, 0xC3000000, 0xC4000000, 0xC5000000,
	0xC6000000, 0xC7000000, 0xC8000000, 0xC9000000, 0xCA000000, 0xCB000000,
	0xCC000000, 0xCD000000, 0xCE000000, 0xCF000000,
	0xD0000000, 0xD2000000, 0xD3000000, 0xD4000000, 0xD5000000, 0xD6000000,
	0xD7000000, 0xD8000000, 0xD9000000, 0xDA000000, 0xDB000000, 0xDC000000,
	0xDD000000, 0xDE000000, 0xDF000000,
	0xE0000000, 0xE1000000, 0xE2000000, 0xE3000000, 0xE4000000, 0xE5000000,
	0xE6000000, 0xE7000000, 0xE8000000, 0xE9000000, 0xEB000000, 0xEC000000,
	0xEE000000,
	0xF0000000, 0xF1000000, 0xF2000000, 0xF3000000, 0xF4000000, 0xF5000000,
	0xF6000000, 0xF7000000, 0xF8000000, 0xF9000000,
	0x0F000000, 0x0C000000
};

static void PSP_Screen_InitGe(void)
{
	int qid;
	sceKernelDcacheWritebackAll();
	qid = sceGeListEnQueue(&GeInit[0], 0, -1, 0);
	sceGeListSync(qid, 0);


	// Draw Area
	GEcmd[ 0] = 0x15000000UL | (  0 << 10) | 0;
	GEcmd[ 1] = 0x16000000UL | (271 << 10) | 479;
	// Tex Enable
	GEcmd[ 2] = 0x1E000000UL | 1;
	// Viewport
	GEcmd[ 3] = 0x42000000UL | (((int)((float)(  480)) >> 8) & 0x00FFFFFF);
	GEcmd[ 4] = 0x43000000UL | (((int)((float)( -272)) >> 8) & 0x00FFFFFF);
	GEcmd[ 5] = 0x44000000UL | (((int)((float)(50000)) >> 8) & 0x00FFFFFF);
	GEcmd[ 6] = 0x45000000UL | (((int)((float)( 2048)) >> 8) & 0x00FFFFFF);
	GEcmd[ 7] = 0x46000000UL | (((int)((float)( 2048)) >> 8) & 0x00FFFFFF);
	GEcmd[ 8] = 0x47000000UL | (((int)((float)(60000)) >> 8) & 0x00FFFFFF);
	GEcmd[ 9] = 0x4C000000UL | (1024 << 4);
	GEcmd[10] = 0x4D000000UL | (1024 << 4);
	// Model Color
	GEcmd[11] = 0x54000000UL;
	GEcmd[12] = 0x55000000UL | 0xFFFFFF;
	GEcmd[13] = 0x56000000UL | 0xFFFFFF;
	GEcmd[14] = 0x57000000UL | 0xFFFFFF;
	GEcmd[15] = 0x58000000UL | 0xFF;
	// Depth Buffer
	GEcmd[16] = 0x9E000000UL | 0x88000;
	GEcmd[17] = 0x9F000000UL | (0x44 << 16) | 512;
	// Tex
	GEcmd[18] = 0xC2000000UL | (0 << 16) | (0 << 8) | 0;
	GEcmd[19] = 0xC3000000UL | 1;
	GEcmd[20] = 0xC6000000UL | (1 <<  8) | 1;
	GEcmd[21] = 0xC7000000UL | (1 <<  8) | 1;
	GEcmd[22] = 0xC9000000UL | (0 << 16) | (0 << 8) | 0;
//	GEcmd[22] = 0xC9000000UL | (0 << 16) | (0 << 8) | 3;//[RIN]
	// Pixel Format
	GEcmd[23] = 0xD2000000UL | 1;
	// Scissor
	GEcmd[24] = 0xD4000000UL | (  0 << 10) |   0;
	GEcmd[25] = 0xD5000000UL | (271 << 10) | 479;
	// Depth
	GEcmd[26] = 0xD6000000UL | 10000;
	GEcmd[27] = 0xD7000000UL | 50000;
	// List End
	GEcmd[28] = 0x0F000000UL;
	GEcmd[29] = 0x0C000000UL;
	GEcmd[30] = 0;
	sceKernelDcacheWritebackAll();
	qid = sceGeListEnQueue(&GEcmd[0], &GEcmd[30], -1, 0);
	sceGeListSync(qid, 0);

/**---force init Ge ----**/

	// Tex Flush
	GEcmd[ 5] = 0xCB000000UL;
	// Set Vertex
	GEcmd[ 6] = 0x12000000UL | (1 << 23) | (0 << 11) | (0 << 9) | (2 << 7) | (0 << 5) | (0 << 2) | 2;
	GEcmd[ 7] = 0x10000000UL;
	GEcmd[ 8] = 0x02000000UL;
	// Draw Vertex
	GEcmd[11] = 0x04000000UL | (6 << 16) | 2;
	// List End
	GEcmd[12] = 0x0F000000UL;
	GEcmd[13] = 0x0C000000UL;
	GEcmd[14] = 0;

}
/******************************************************************************/
/* copy from uo_Snes9x for PSP */
/******************************************************************************/

static void  PSP_Screen_BitBltGe00(
 unsigned char* src,
 int srcW, int srcH,
// int dstX, int dstY,
 int dstW, int dstH )
{
	static int qid = -1 ;
//	unsigned int GEcmd[16] ;
	static short ScreenVertex[10] __attribute__((aligned(64))) =  {
		//0  1	2  3  4    5	6	 7	  8  9
		  0, 0, 0, 0, 0, 256, 224, 480, 272, 0
		};

	if( qid != -1){ sceGeListSync(qid, 0); }

//	ScreenVertex[2] = 0;//dstX ; //static init on force 0
//	ScreenVertex[3] = 0;//dstY ; //static init on force 0
	ScreenVertex[5] = srcW ;
	ScreenVertex[6] = srcH ;
	ScreenVertex[7] = dstW ; // + dstX
	ScreenVertex[8] = dstH ; // + dstY

	// Set Draw Buffer
	GEcmd[ 0] = 0x9C000000UL | ( (unsigned long)PSP_Screen_GetBackBuffer(0,0) & 0x00FFFFFF);
	GEcmd[ 1] = 0x9D000000UL | (((unsigned long)PSP_Screen_GetBackBuffer(0,0) & 0xFF000000) >> 8) | 512;

	// Set Tex Buffer
	GEcmd[ 2] = 0xA0000000UL | ( (unsigned long)src & 0x00FFFFFF);
	GEcmd[ 3] = 0xA8000000UL | (((unsigned long)src & 0xFF000000) >> 8) | 512;

	// Set Tex Buffer
	if (srcW > 256)
	{
		if (srcH > 256){	GEcmd[ 4] = 0xB8000000UL | (9 << 8) | 9;}
		else{				GEcmd[ 4] = 0xB8000000UL | (8 << 8) | 9;}
	}else{
		if (srcH > 256){	GEcmd[ 4] = 0xB8000000UL | (9 << 8) | 8;}
		else{				GEcmd[ 4] = 0xB8000000UL | (8 << 8) | 8;}
	}
	// Set Vertex
	GEcmd[ 9] = 0x10000000UL | (((unsigned long)((void*)&ScreenVertex) & 0xFF000000) >> 8);
	GEcmd[10] = 0x01000000UL | ( (unsigned long)((void*)&ScreenVertex) & 0x00FFFFFF);
	sceKernelDcacheWritebackAll();
	qid = sceGeListEnQueue(&GEcmd[0], &GEcmd[14], -1, NULL);

	sceGeListSync(qid, 1);
}


/////////////////////////////////////////////////////////
/// psp_screen.c
//////////////////////////////////////////////////////////////////////////


#if 00
void pgInit()
{
	sceDisplaySetMode(0,SCREEN_WIDTH,SCREEN_HEIGHT);
	pgScreenFrame(0,0);

//	pgGeInit();[RIN]
	PSP_Screen_InitGe();
}
#else
//int PSP_Screen_Init( void )
static void pgInit()
{
	PSP_Screen_InitGe() ;

	sceDisplaySetMode( 1, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT );
	sceDisplaySetFrameBuf(0,0,0,1);

	my_memset0( psp_screen_ptr, FRAMESIZE * 2 ) ;

	sceDisplaySetFrameBuf( psp_screen_ptr, LINESIZE,1,1 ) ;

//	return 0 ;
}
#endif
