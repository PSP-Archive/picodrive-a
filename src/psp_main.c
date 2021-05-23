
#include "shared.h"
//#include "pg.h"

/* include version[START] */
#define VERSION const static char title[]
#include "version.txt"
"m"
#ifdef _mpu22
"22"
#elif _mpu33
"33"
#elif _mpu44
"44"
#elif _mpu55
"55"
#endif
"z"
#ifdef _z01
"01"
#elif _z02
"02"
#elif _z03
"03"
#elif _z04
"04"
#elif _z05
"05"
#elif _z06
"06"
#endif
" 2006-08/01 固定fps modeなくなりました"
"∽∝∵∫∬開発幼稚園";
#undef VERSION
/* include version[END] */

#include "loadrom.h"
#include "main_text.h"

extern unsigned char vdp_reg[0x20];

#if 00
extern void* PSP_Screen_GetTempBuffer( int x, int y );
extern void  PSP_Screen_BitBltGe00(
 unsigned char* src,
 int srcW, int srcH,
// int dstX, int dstY,
 int dstW, int dstH );
#endif

#include "_pg.c"

#if 000
//e氏の「PCE070」を参考にしたが、役に立ってるのか（初心者なんで）ヨクワカラン。
//もしかして使い方間違ってるとか？？（謎）
static void v_sync_e(void)
{
#if 00
	static int pv = 0;
	int 	   cv = sceDisplayGetVcount();
	if(pv==cv) {
		sceDisplayWaitVblankCB();
	}
	pv = sceDisplayGetVcount();
#else
	sceDisplayWaitVblankCB();
#endif
}
#endif

#define CLS_00 pgFillvram(5+(5<<5)+(5<<10))



#define AUDIO_44100	0
#define AUDIO_22050	1
#define AUDIO_11025	2
#define AUDIO_5512	3

#define AUDIO_OFF		4
#define AUDIO_F_5512	5
#define AUDIO_F_11025	6
#define AUDIO_F_22050	7

// need 2^x menus
#undef AAA_MODE
#ifdef AAA_MODE
#define MENU_FPS_MAX	16
#else
#define MENU_FPS_MAX	8
#endif
static const char menu_fps_str[MENU_FPS_MAX][16]={
/*1234567890123456789012*/
 "auto 0(max60)"//	 "skip 0(max60)"	/*0*/
,"auto 1(max60)"//	,"skip 1(max30)"	/*1*/
,"auto 2(max60)"//	,"skip 2(max20)"	/*2*/
,"auto 3(max60)"//	,"skip 3(max15)"	/*3*/
,"auto 4(max60)"//	,"skip 4(max12)"	/*4*/
,"auto 5(max60)"//	,"skip 5(max10)"	/*5*/
,"auto 6(max60)"//	,"skip 6(max 8)"	/*6*/
,"auto 7(max60)"//	,"(AutoFPSmode)"	/*7*/
#ifdef AAA_MODE
,"auto 0(max60)"	/*8*/
,"auto 1(max60)"	/*9*/
,"auto 2(max60)"	/*10*/
,"auto 3(max60)"	/*11*/
,"auto 4(max60)"	/*12*/
,"auto 5(max60)"	/*13*/
,"auto 6(max60)"	/*14*/
,"auto 7(max60)"	/*15*/
#endif
};
#define MENU_AUDIO_MAX	8
static const char menu_audio_str[MENU_AUDIO_MAX][16]={
/*1234567890123456789012*/
 "44.100 [Hz]  "	/*0*/
,"22.050 [Hz]  "	/*1*/
,"11.025 [Hz]  "	/*2*/
," 5.512 [Hz]  "	/*3*/
," OFF         "	/*4*/
," 5.512 [Hz]F "	/*5*/
,"11.025 [Hz]F "	/*6*/
,"22.050 [Hz]F "	/*7*/
};
static const char screen_mode_str[8][16]={
/*1234567890123456789012*/
 "1x         原寸"
,"FIT    縦あわせ"
,"FULL       拡大"
,"MAX_BOTTOM 下  "

,"MAX_DOWN   下側"
,"MAX_MID    真中"
,"MAX_UP   上より"
,"MAX_TOP    上  "
};
static const char on_off_str[2][4]={
 "OFF"
,"ON "
};

#define MENU_SHOW_FPS	0
#define MENU_FPS	1
#define MENU_VSYNC	2
#define MENU_SCREEN 3

#define MENU_AUDIO	4
#define MENU_SUMFIX	5
#define MENU_RAPID	6
#define MENU_RESET	7

// need 2^x menus
#define MENU_MAX	8


#define PAGE_FILER   0
#define PAGE_SETTING 1
#define PAGE_STATE   2
#define PAGE_CHEAT   3

//#define PAGE4MODE
#undef PAGE4MODE
#ifdef PAGE4MODE
#define MAX_PAGE (4)
#else
#define MAX_PAGE (2)
#endif
/*1234567890123456789012*/
static const char setumei[MAX_PAGE][128]={
 "L+R / ×：Back to Game ○：OK / LOAD △：PATH UP [HOME]：QUIT"
,"L+R / ×：Back to Game ←：-- →：++ △：SET TO DEFAULT [HOME]：QUIT"
#ifdef PAGE4MODE
,"L+R / ×：Back to Game [HOME]：QUIT // [STATE MENU]"
,"L+R / ×：Back to Game [HOME]：QUIT // [CHEAT MENU]"
#endif
};

static const char menu_str[MENU_MAX][16]={
/*1234567890123456789012*/
 "SHOW FPS     "
,"SKIP FRAME   "
,"V-SYNC       "
,"SCREEN MODE  "

,"AUDIO RATE   "
,"FILE FIX MODE"
,"RAPID RATE   "
,"[RESET]      "
};
static unsigned char menu_value[MENU_MAX];
static unsigned char menu_value_default[MENU_MAX];

#define PSP_SCREEN_MINI		0
#define PSP_SCREEN_FIT		1
#define PSP_SCREEN_FULL		2
#define PSP_SCREEN_0OVER	3

#define PSP_SCREEN_1OVER	(PSP_SCREEN_0OVER+1)	/*4: OVER+1 */
#define PSP_SCREEN_2OVER	(PSP_SCREEN_0OVER+2)	/*5: OVER+2 */
#define PSP_SCREEN_3OVER	(PSP_SCREEN_0OVER+3)	/*6: OVER+3 */
#define PSP_SCREEN_4OVER	(PSP_SCREEN_0OVER+4)	/*7: OVER+4 */

//prototypes.
//int update_input(void);

#define X_STR_00	 00
#define X_STR_50	 30
#define X_STR_70	 70
#define X_STR_55	(120-28)
#define X_STR_99	(120)
#define Y_STR_FPS	  0
//#define Y_STR_SKIP	 12
#define Y_STR_NUMB	(24)
#define Y_STR_DEBUG (262-12)


#define UNMASK 0
#define FILLED 1

#define MY_OFF 0
#define MY_ON  1

#define AUTO_FPS 7

	// flame skip set to 2 fps.初期値はＦＳＫＩＰ２
	// slow this emulator now, that force 1 at first time.
	// you see easy to the all flames, do use at the Rolling Thunder2,
	// good well in character selecting scene.
	// As flickers can be than flames skipper from zero as good to two it.
	// どうせPSPの液晶は（中間色の）反応速度が悪いので
	// 初期値は、2fps飛ばし。(このエミュまだ遅いんです)
	// 60fpsの確認には、「Rolling Thunder2」のキャラクタ選択画面が最適
	// メガドラのゲームの「ちらつき」を再現する為には、ＦＳＫＩＰ１より
	// ＦＳＫＩＰ２の方が再現度が高い。（が、描画が荒い）

	/* initial value */
		// disabled debug menu at startup. デバッグメニュー表示しない。
#define INIT_SHOW_FPS   MY_OFF
//#define INIT_SHOW_FPS   MY_ON
	// flame skip set to (7) auto fps.初期値はauto fps(7).
//#define INIT_FPS AUTO_FPS
//#define INIT_FPS 1
#define INIT_FPS 6
	// screen mode set to PSP_SCREEN_FULL .初期値はGe拡大モード
#define INIT_SCREEN PSP_SCREEN_FULL
//#define INIT_AUDIO  AUDIO_11025
#define INIT_AUDIO  AUDIO_22050
/* 業務用高速連射 */
#define INIT_RAPID   1


////////////////////////////////
////////////////////////////////

//int           PSP_Sound_Init           ( int nChannel );
//void          PSP_Sound_Enable         ( short nCh, short nEnable );
///////////////////////////////////////////////////////////////////////////
//#define MAX_PSP_SOUND_BUFFER   (PSP_SOUND_SAMPLE * (2+2) * 2)
// ----------------------------------------

#define PSP_SOUND_SAMPLE       (768)



#define FILE_DIR_LIST_MODE 1
#define FILE_LOADING_MODE  2
// ----------------------------------------
#define MAXDEPTH  16
#define MAXDIRNUM 1024
#define PATHLIST_H 23
// ----------------------------------------
static PSP_FILE_ATTRIBUTE	dlist[MAXDIRNUM];
static int 		dlist_curpos;
static int 		dlist_num;
static int 		dlist_start;
static int 		now_depth;
static int 		cbuf_start[MAXDEPTH];
static int 		cbuf_curpos[MAXDEPTH];

static char		now_path[MAXPATH];
//static char	path_tmp[MAXPATH];
static char		target[MAXPATH];
//static char		path_main[MAXPATH];

//extern	char	target[MAXPATH];

char	path_main[MAXPATH];
// ----------------------------------------
//volatile int bSleep=0;

//int timer_count = 0;
//int old_timer_count = 0;
//int paused = 0;
//int frame_count = 0;

//static unsigned char *keystate;

//int ini_frame=0;
static int menu_mode;

static char menu_redraw;
static char cls_redraw;

static char rom_loaded;
static char inited;	/*menu.c*/

static char menu_page;
static char menu_exit;

static volatile unsigned char bSleep=0;
static volatile unsigned char psp_sound_enable;			/* サウンドON/OFF flug */

// ----------------------------------------
static ctrl_data_t ctl;
// ----------------------------------------
//static	unsigned long is_PAD;
/* triggered based the pad input */
//[is_#] now input key.
//[rs_#] old key.(before key)
//[ps_#] is pressed.(or pushed)[trigged the key]
//[cs_#] count holding key at time.(fps base counted)

static unsigned int is_PAD; // read pad raw data.

static unsigned char is_E,rs_E,ps_E;//,cs_E;	// E: Select//disabled
static unsigned char is_S,rs_S,ps_S;//,cs_S;	// S: Start
static unsigned char is_U,rs_U,ps_U,cs_U;		// U: Up
static unsigned char is_J,rs_J,ps_J;//,cs_J;	// J: Right (see your keyboard)
static unsigned char is_D,rs_D,ps_D,cs_D;		// D: Down
static unsigned char is_F,rs_F,ps_F;//,cs_F;	// F: Left  (see your keyboard)
static unsigned char is_L,rs_L,ps_L;//,cs_L;	// L: L-trigger
static unsigned char is_R,rs_R,ps_R;//,cs_R;	// R: R-trigger
static unsigned char is_A,rs_A,ps_A;//,cs_A;	// A: triangle
static unsigned char is_O,rs_O,ps_O;//,cs_O;	// O: Circle
static unsigned char is_X,rs_X,ps_X;//,cs_X;	// X: Cross
static unsigned char is_Q,rs_Q,ps_Q;//,cs_Q;	// Q: Square
// ----------------------------------------

//static int vram_buffer[512*512/4*2] __attribute__((aligned(64))) ;

//unsigned char vram_buffer[512*512*2] __attribute__((aligned(64))) ;

//#define vram_buffer ((void*)0x04100000);

extern void md_sound_update(unsigned short* p, short len );
//extern char rom_loaded;
//extern char inited;
extern long pg_drawframe;

// ----------------------------------------
static char *mh_strncpy(char *s1,char *s2,int n) {
	char *rt = s1;

	while((*s2!=0) && ((n-1)>0)) {
		*s1 = *s2;
		s1++;
		s2++;
		n--;
	}
	*s1 = 0;

	return rt;
}

static char *mh_strncat(char *s1,char *s2,int n) {
	char *rt = s1;

	while((*s1!=0) && ((n-1)>0)) {
		s1++;
		n--;
	}

	while((*s2!=0) && ((n-1)>0)) {
		*s1 = *s2;
		s1++;
		s2++;
		n--;
	}
	*s1 = 0;

	return rt;
}
static void pad_read(void)
{
static char ccc; //counter
unsigned char ttt; // work
	ccc++; //count up
	ccc&=1; // masked on/off
	//key = Read_Key();
	sceCtrlReadBufferPositive(&ctl,1);

// 0123456789abcdef
// Uuuu--------dddD

	/* pached the analog stick to digitily sense. */
	ttt=ctl.analog[CTRL_ANALOG_Y];
	     if(ttt < 0x10) {         ctl.buttons |= CTRL_UP;    /* UP HIGH */ }
	else if(ttt < 0x40) { if(ccc) ctl.buttons |= CTRL_UP;    /* UP LOW  */ }
	else if(ttt < 0xc0) { ; /* NONE */ }
	else if(ttt < 0xf0) { if(ccc) ctl.buttons |= CTRL_DOWN;  /* DOWN LOW  */ }
	else                {         ctl.buttons |= CTRL_DOWN;  /* DOWN HIGH */ }

	ttt=ctl.analog[CTRL_ANALOG_X];
	     if(ttt < 0x10) {         ctl.buttons |= CTRL_LEFT;   /* LEFT HIGH */ }
	else if(ttt < 0x40) { if(ccc) ctl.buttons |= CTRL_LEFT;   /* LEFT LOW  */ }
	else if(ttt < 0xc0) { ; /* NONE */ }
	else if(ttt < 0xf0) { if(ccc) ctl.buttons |= CTRL_RIGHT;  /* RIGHT LOW  */ }
	else                {         ctl.buttons |= CTRL_RIGHT;  /* RIGHT HIGH */ }

	/* before buttons sense */
	rs_E=is_E;
	rs_S=is_S;
	rs_L=is_L;
	rs_R=is_R;

	rs_U=is_U;
	rs_J=is_J;
	rs_D=is_D;
	rs_F=is_F;

	rs_A=is_A;
	rs_O=is_O;
	rs_X=is_X;
	rs_Q=is_Q;

	/* now buttons sense */
	is_PAD=ctl.buttons;
	is_E=(is_PAD & CTRL_SELECT);
	is_S=(is_PAD & CTRL_START);
	is_L=(is_PAD & CTRL_LTRIGGER)?1:0;
	is_R=(is_PAD & CTRL_RTRIGGER)?1:0;

	is_U=(is_PAD & CTRL_UP);
	is_J=(is_PAD & CTRL_RIGHT);
	is_D=(is_PAD & CTRL_DOWN);
	is_F=(is_PAD & CTRL_LEFT);

	is_A=(is_PAD & CTRL_TRIANGLE)?1:0;
	is_O=(is_PAD & CTRL_CIRCLE)?1:0;
	is_X=(is_PAD & CTRL_CROSS)?1:0;
	is_Q=(is_PAD & CTRL_SQUARE)?1:0;

	/* now pushed or pulled buttons(triggered check) */
	ps_E=(is_E != rs_E);
	ps_S=(is_S != rs_S);
	ps_L=(is_L != rs_L);
	ps_R=(is_R != rs_R);

	ps_U=(is_U != rs_U);
	ps_J=(is_J != rs_J);
	ps_D=(is_D != rs_D);
	ps_F=(is_F != rs_F);

	ps_A=(is_A != rs_A);
	ps_O=(is_O != rs_O);
	ps_X=(is_X != rs_X);
	ps_Q=(is_Q != rs_Q);

	/* key pressed time counter at flame */ /* 要するにオートリピート */
	if(is_U) cs_U++;	if(ps_U) cs_U=0;
	if(is_D) cs_D++;	if(ps_D) cs_D=0;
}
static void key_release_chk(void)	/* ボタン離れるまで待つ */
{
	while(is_PAD){
		v_sync();
		pad_read();
	}
}
// ----------------------------------------
#define MY_EXT_OTHER    0
#define MY_EXT_NONE     1
#define MY_EXT_BIN      2
#define MY_EXT_SMD      3
#define MY_EXT_ZIP      4

/* ファイル名の拡張子を調べる */
static int f_extention(char *p)
{
	while(*p){
		if( '.'== *p )break;	/* '.'か$00が来るまで調べる。 */
		p++;	/*次の文字へ進む*/
	}
	if( !*p )return MY_EXT_NONE;	/* 1: '.'がなかったらおしまい */
	/* （ピリオドまでやったから、続きをチェック） */
	while(*p){	/* 一文字ずつチェック */
		if( ( '.'==p[0])&&(!p[4]) ){	/* '.???'+$00（ピリオドと終わりの組み合わせかな？） */
		char c1,c2,c3;
/* こんなんじゃなくてもいいよね。多分。 */
#define MY_TOUPPER(ccc) ((0x40<ccc)?((ccc)|0x20):(ccc))
/* 少々限定的なtoupper()（例えば、$00は$20になっちゃう） */
//efine MY_TOUPPER(ccc) ((ccc)|0x20)
			c1=MY_TOUPPER(p[1]);
			c2=MY_TOUPPER(p[2]);
			c3=MY_TOUPPER(p[3]);
#undef MY_TOUPPER
			if(	( 'z'==c1 ) && ( 'i'==c2 ) && ( 'p'==c3 ) )return MY_EXT_ZIP;	/* '.zip'+$00ならおしまい。 */
			if(	( 's'==c1 ) && ( 'm'==c2 ) && ( 'd'==c3 ) )return MY_EXT_SMD;	/* '.smd'+$00ならおしまい。 */
			if(	( 'b'==c1 ) && ( 'i'==c2 ) && ( 'n'==c3 ) )return MY_EXT_BIN;	/* '.bin'+$00ならおしまい。 */
		}
		p++;	/*次の文字へ進む*/
	}
	return MY_EXT_OTHER;	/* 0: 駄目 */
}
static void	menu_file_Get_DirList(void) {
	int ret,fd;

	// Directory read
	fd = sceIoDopen(now_path);
	dlist_num = 0;
	ret = 1;
	if( now_depth>0 ){
		_strcpy(dlist[dlist_num].name,"..");
		dlist[dlist_num].type = TYPE_DIR;
		dlist_num++;
	}
	while((ret>0) && (dlist_num<MAXDIRNUM)) {
		ret = sceIoDread(fd, &dlist[dlist_num]);
		if (dlist[dlist_num].name[0] == '.') continue;
		if (0==f_extention(dlist[dlist_num].name)) continue;
		if (ret>0) dlist_num++;
	}
	sceIoDclose(fd);

	// ディレクトリリストエラーチェック
	if (dlist_start  >= dlist_num) { dlist_start  = dlist_num-1; }
	if (dlist_start  <	0)		   { dlist_start  = 0;			 }
	if (dlist_curpos >= dlist_num) { dlist_curpos = dlist_num-1; }
	if (dlist_curpos <	0)		   { dlist_curpos = 0;			 }
}


static void page_filer(void)
{
	int i;
	int col;
	char file_ctrl_flag;
	char path_move_mode;

	path_move_mode=0;
	file_ctrl_flag= 0;

	// 現在のファイルパス位置、表示
	//mh_strncpy(path_tmp,now_path,40);
	//text_print(0,0,path_tmp,COLOR_WHITE,0,1);
	//text_print(0,0,now_path,COLOR_WHITE,0,1);
	text_print(0,10,now_path,COLOR_WHITE,0,1);

	// ディレクトリリスト
	i = dlist_start;
	while (i<(dlist_start+PATHLIST_H)) {
		if (i<dlist_num) {
			col = COLOR_WHITE;
			if(dlist[i].type & TYPE_DIR) {	col = COLOR_YELLOW;	}
			if(i==dlist_curpos){			col = COLOR_RED;}

		//	mh_strncpy(path_tmp,dlist[i].name,40);
		//	text_print(32,((i-dlist_start)+2)*10,path_tmp,col,0,1);
			text_print(32,((i-dlist_start)+2)*10,dlist[i].name,col,0,1);
		}
		i++;
	}
	v_sync();pgScreenFlip();

//		if( control_bef_ctl&CTRL_TRIANGLE )return 0;

//[menu_file_Control]//

#define REPEAT_TIME 10
	if ( ((ps_U ||(REPEAT_TIME<cs_U)) && is_U) ) {
		if (dlist_curpos > 0) {
			dlist_curpos--;
			if (dlist_curpos < dlist_start) { dlist_start = dlist_curpos; }
		}
	}
	else if ( ((ps_D ||(REPEAT_TIME<cs_D)) && is_D) ) {
		if (dlist_curpos < (dlist_num-1)) {
			dlist_curpos++;
			if (dlist_curpos >= (dlist_start+PATHLIST_H)) { dlist_start++; }
		}
	}
#undef REPEAT_TIME

	if ( ps_F && is_F ) {
		dlist_curpos -= PATHLIST_H;
		if (dlist_curpos <	0)			{ dlist_curpos = 0; 		  }
		if (dlist_curpos < dlist_start) { dlist_start = dlist_curpos; }
	}
	else if ( ps_J && is_J ) {
		dlist_curpos += PATHLIST_H;
		if (dlist_curpos >= dlist_num) { dlist_curpos = dlist_num-1; }
		while (dlist_curpos >= (dlist_start+PATHLIST_H)) { dlist_start++; }
	}
	//-----
	if ( ps_A && is_A ) {
		path_move_mode=1;
	}
	else if ( ps_O && is_O ) {
		if( dlist[dlist_curpos].name[0]=='.' ){
			path_move_mode=1;
			//break;
			//goto pathmove;
		}
		else if (dlist[dlist_curpos].type & TYPE_DIR) {
			if (now_depth<MAXDEPTH) {
				// path移動
				mh_strncat(now_path,dlist[dlist_curpos].name,MAXPATH);
				mh_strncat(now_path,"/",MAXPATH);
				cbuf_start[now_depth]  = dlist_start;
				cbuf_curpos[now_depth] = dlist_curpos;
				dlist_start  = 0;
				dlist_curpos = 0;
				now_depth++;
				file_ctrl_flag= FILE_DIR_LIST_MODE;
			}
		} else {
			file_ctrl_flag= FILE_LOADING_MODE;
		}
	}

	//-----
	if(path_move_mode){
	//	pathmove:
		if (now_depth > 0) {
			// path移動
			for(i=0;i<MAXPATH;i++) {
				if (now_path[i]==0) break;
			}
			i--;
			while(i>4) {
				if (now_path[i-1]=='/') {
					now_path[i]=0;
					break;
				}
				i--;
			}
			now_depth--;
			dlist_start  = cbuf_start[now_depth];
			dlist_curpos = cbuf_curpos[now_depth];
			file_ctrl_flag= FILE_DIR_LIST_MODE;
		}
	}
	//-----
//---------------


		switch(file_ctrl_flag) {
		case FILE_DIR_LIST_MODE:
			menu_file_Get_DirList();
			break;
		case FILE_LOADING_MODE:
			mh_strncpy(target,now_path,MAXPATH);
			mh_strncat(target,dlist[dlist_curpos].name,MAXPATH);

			CLS_00;
			//text_print( 240-((_strlen(target)*5)>>1), 130, target, COLOR_WHITE,0,0);
			text_print( 20,  30, target, COLOR_WHITE,0,0);
			v_sync();pgScreenFlip();

			if( !load_rom(target) ){
				rom_loaded=0;
				//Error_mes("ROMないでぇ");
				return	;//0;
			}
			rom_loaded=1;
			inited=0;	/*強制初期化*/
			menu_exit=MY_OFF;
			return ;//1;
			break;
//		case 3:
//			return 0;
		}
}
static void menu_line( int x, int y,int num,int color,int values){
	text_print(  x-X_STR_55, y, (char *)&menu_str[num][0]	, COLOR_HCYAN,COLOR_BLACK,FILLED);
	switch(num){

	case MENU_SHOW_FPS:
	case MENU_VSYNC:
	case MENU_SUMFIX:
		text_print(  x , y,
			 (char *)&on_off_str[values][0]
			, color,COLOR_BLACK,FILLED);
		break;
	case MENU_FPS:
		text_print(  x , y,
			 (char *)&menu_fps_str[values][0]
			, color,COLOR_BLACK,FILLED);
		break;
	case MENU_SCREEN:
		text_print(  x , y,
			 (char *)&screen_mode_str[values][0]
			, color,COLOR_BLACK,FILLED);
		break;
	case MENU_AUDIO:
		text_print(  x , y,
			(char *)&menu_audio_str[values][0]
			, color,COLOR_BLACK,FILLED);
		break;
	case MENU_RESET:
		break;
	default:
		text_counter(x+X_STR_99-X_STR_55, y, values/*ini_frame*/, 4, color);
	}
}

#ifndef VCLK
#define VCLK   (7159090)
#endif


//#define SUPPORT_AUTO_FPS_MODE
#undef SUPPORT_AUTO_FPS_MODE

#ifdef SUPPORT_AUTO_FPS_MODE

static unsigned int	syorioti;


#endif

/* menuも段々破綻してきたな */
static int now_sound_samples;
static void audio_rate(void){
	switch(menu_value[MENU_AUDIO ]){

	case AUDIO_44100:	YM2612Init(44100);now_sound_samples=PSP_SOUND_SAMPLE;		break;
	case AUDIO_22050:	YM2612Init(22050);now_sound_samples=PSP_SOUND_SAMPLE/2;	break;
	case AUDIO_11025:	YM2612Init(11025);now_sound_samples=PSP_SOUND_SAMPLE/4;	break;
	case AUDIO_5512:	YM2612Init( 5512);now_sound_samples=PSP_SOUND_SAMPLE/8;	break;

	case AUDIO_OFF:		break;
	case AUDIO_F_5512:	YM2612Init( 5512);now_sound_samples=PSP_SOUND_SAMPLE/8;	break;
	case AUDIO_F_11025:	YM2612Init(11025);now_sound_samples=PSP_SOUND_SAMPLE/4;	break;
	case AUDIO_F_22050:	YM2612Init(22050);now_sound_samples=PSP_SOUND_SAMPLE/2;	break;
	}
}
//static void adj_pre_auto(void);
static void fix_menu_value(void)
{
	switch(menu_mode){
	case MENU_SHOW_FPS:	menu_value[MENU_SHOW_FPS] &=1;	break; /* toggled */
	case MENU_FPS:		menu_value[MENU_FPS   ] &=(MENU_FPS_MAX-1);	break; /* 0 to 15 */
	case MENU_VSYNC:	menu_value[MENU_VSYNC ] &=1;	break; /* toggled */
	case MENU_SCREEN:	menu_value[MENU_SCREEN] &=7;	break; /* 8 screen mode */

	case MENU_AUDIO:	menu_value[MENU_AUDIO ] &=7; audio_rate();	break; /* 0 to 7 */
	case MENU_SUMFIX:	menu_value[MENU_SUMFIX] &=1;	break; /* toggled */
	case MENU_RAPID:	menu_value[MENU_RAPID ] &=7;	break; /* 0 to 7 */
	case MENU_RESET:	menu_value[MENU_RESET ]  =0; system_reset();	break; /* immediate reset.動かしたら、速攻リセット。 */
	//MEMO:	case MENU_RAPID:	if( menu_value[MENU_RAPID ] >5 ) menu_value[MENU_RAPID ]=0;	break;
	}
#if 00
//	adj_pre_auto();
//static void adj_pre_auto(void)
{
#ifdef SUPPORT_AUTO_FPS_MODE
	sceCtrlReadBufferPositive(&ctl,1);	//need it???
	syorioti   =ctl.frame-(TM_1FRAME*3);
#endif
}
#endif
}
static void page_setting(void)
{
	int yyy;
	int ccc;
	int num;
	char aaa;
	yyy=48;

	/*書く*/
	text_print(  20, 24, "config       現在値"				, COLOR_HCYAN,COLOR_BLACK,FILLED);
	text_print( 240, 24, "default      ロード後の再設定値"	, COLOR_HCYAN,COLOR_BLACK,FILLED);
	for(num=0;num<MENU_MAX;num++)
	{
		yyy+=12;
		ccc=COLOR_WHITE;
		if(num==menu_mode) ccc=COLOR_RED;
		menu_line( 20+X_STR_55, yyy,num,ccc,menu_value[num]);
		menu_line(240+X_STR_55, yyy,num,ccc,menu_value_default[num]);
	}
	v_sync();pgScreenFlip();
	if      ( ps_U && is_U ) {		menu_mode--;	}
	else if ( ps_D && is_D ) {		menu_mode++;	}
	menu_mode &= (MENU_MAX-1);

	aaa=0;
	if      ( ps_F && is_F ) {	menu_value[menu_mode]--;	aaa=1;	}
	else if ( ps_J && is_J ) {	menu_value[menu_mode]++;	aaa=1;	}

	if ( ps_A && is_A ) {
		menu_value_default[menu_mode]=menu_value[menu_mode];
		aaa=1;
	}
	if(aaa) fix_menu_value();

}
#ifdef PAGE4MODE
static void page_state(void){
	text_print(      20+X_STR_00, 20, "state page"	, COLOR_HCYAN,COLOR_BLACK,FILLED);
	v_sync();pgScreenFlip();
}
static void page_cheat(void){
	text_print(      20+X_STR_00, 20, "cheat page"	, COLOR_HCYAN,COLOR_BLACK,FILLED);
	v_sync();pgScreenFlip();
}
#endif
//extern unsigned long Read_Key(void);
int 	menu_system(void)
{
		CLS_00;		//画面消す。
		pgScreenFlip();
		CLS_00;		//画面消す。

	key_release_chk();	//キー離されるまで待つ。
	menu_exit=MY_ON;
	while(menu_exit){
		v_sync();
		pad_read();	/* pad button read */

		if ( 2==(is_L + is_R) ){	/* L+Rキーでも戻れる */
			menu_exit=MY_OFF;
		}
		else if ( ps_X && is_X ){	/* Ｘキーでも戻れる */
			menu_exit=MY_OFF;
		}
		else if( ps_L && is_L ){
			menu_page--;
			menu_page&=(MAX_PAGE-1);
		}
		else if( ps_R && is_R ){
			menu_page++;
			menu_page&=(MAX_PAGE-1);
		}
		/* ＶＥＲ表示＆タイトル */
		CLS_00;		//画面消す。
		text_print(0, 0,(char *)title,COLOR_CYAN,0,1);

		/* キー操作、１行説明表示 */
		text_print(0,262, (char *)&setumei[(int)menu_page][0] ,COLOR_WHITE,COLOR_BLACK,FILLED);

		switch(menu_page)
		{
		case PAGE_FILER:	page_filer();	break;
		case PAGE_SETTING:	page_setting();	break;
#ifdef PAGE4MODE
		case PAGE_STATE:	page_state();	break;
		case PAGE_CHEAT:	page_cheat();	break;
#endif
		}
	}
	key_release_chk();	//キー離されるまで待つ。
	CLS_00;				//画面消す。
	return rom_loaded;//0;
}



///////////////////////////////////////////////////////////////////////////
/// PSP Sound
///////////////////////////////////////////////////////////////////////////
//#define MAX_PSP_SOUND_CHANNEL  (1)
//efine MAX_PSP_VOLUME         (0x8000)
//efine MAX_PSP_SOUND_BANK     (6)
//#define PSP_SOUND_SAMPLE_W     (PSP_SOUND_SAMPLE/2)

//static volatile int   psp_sound_terminate = 0;	/* サウンド有効かどうか、判断するフラグ */
static unsigned long  psp_sound_handle;			/* 登録するサウンドスレッドのハンドル */
static unsigned long  psp_sound_thid;			/* 登録するサウンドスレッドのＩＤ */
static unsigned short psp_sound_playbuf[2][ PSP_SOUND_SAMPLE * 2 ] __attribute__((aligned(64))) ;

static inline void memset4x( unsigned long* dest, unsigned long set, short ct4x )
{
	for(; ct4x>0; ct4x-- ){ *dest++ = set; }
}

// （コールバックされる）サウンドスレッド。
static int PSP_Sound_Thread(/*SceSize*/unsigned int args, void *argp )
{
//	int nChannel = *((int*)(argp));
	short outbank = 0;

	while ( 1/*psp_sound_terminate == 0*/ )
	{
//#define FORCE_SOUND_ON
#ifndef FORCE_SOUND_ON

		outbank ^= 1;
		if( psp_sound_enable )
		{
#endif
			md_sound_update( psp_sound_playbuf[outbank], now_sound_samples );

			//sceAudioOutputPannedBlocking( psp_sound_handle , MAX_PSP_VOLUME, MAX_PSP_VOLUME, (char *)psp_sound_playbuf[outbank] );
			sceAudioOutputPannedBlocking( psp_sound_handle ,
/* psp_sound_vol(R?) */ 0xFFFF,
/* psp_sound_vol(L?) */ 0xFFFF,
 (char *)psp_sound_playbuf[outbank] );

//psp_sound_vol = 0x8000;
//psp_sound_vol = 0xFFFF;
//	case VOL_08 : psp_sound_vol = 0x8000;
//	case VOL_10 : psp_sound_vol = 0xA000;
//	case VOL_12 : psp_sound_vol = 0xC000;
//	case VOL_14 : psp_sound_vol = 0xE000;
//	case VOL_16 : psp_sound_vol = 0xFFFF;

#ifndef FORCE_SOUND_ON
		}
		else
		{
			memset4x( (unsigned long*)psp_sound_playbuf[outbank], 0, now_sound_samples );
			sceAudioOutputPannedBlocking( psp_sound_handle , 0, 0, (char *)psp_sound_playbuf[outbank] );
		}
#endif
	}

	sceKernelExitThread(0);
	return 0;
}

void PSP_Sound_Init(void)
{
	my_memset0( psp_sound_playbuf,  sizeof(psp_sound_playbuf) );
	/*
	for( n=0; n < MAX_PSP_SOUND_BANK; n++ )
	{
		memset( psp_sound_buffer[0][n], 0, MAX_PSP_SOUND_BUFFER * sizeof(short) );
	}
	*/
	//psp_sound_terminate = 0;

//	psp_sound_handle = -1; /* initial value was unregitered/failed:-1. 未登録／失敗は-1とする。（初期値） */
	psp_sound_thid   = -1; /* initial value was unregitered/failed:-1. 未登録／失敗は-1とする。（初期値） */
	psp_sound_enable = 0 ;

//	psp_sound_handle_w = sceAudio_3(-1, PSP_SOUND_SAMPLE_W, 0);

	psp_sound_handle = sceAudioChReserve(-1, PSP_SOUND_SAMPLE, 0);

	if( psp_sound_handle >= 0 )
	{
	// Register SCE sound thread. サウンドスレッドを登録する。
	const char thread_name[] = "pgasnd0";	// register the thread name. 登録する名前
		psp_sound_thid =
			sceKernelCreateThread(
			thread_name,
			&PSP_Sound_Thread,
			0x12,0x10000,0,NULL );

		if( psp_sound_thid >= 0 )
		{
		int dummy_n;
			dummy_n=0;
			sceKernelStartThread( psp_sound_thid , sizeof(dummy_n), &dummy_n );
		}
	}
}

//void PSP_Sound_Enable( short nCh, short nEnable )
//{
//	psp_sound_enable[ nCh ] = nEnable;
//}

#if 00
static void PSP_Sound_Term(void)
{
	//psp_sound_terminate = 1;

	if( psp_sound_thid >= 0 )
	{
		sceKernelWaitThreadEnd(psp_sound_thid ,NULL);
		sceKernelDeleteThread (psp_sound_thid );

		psp_sound_thid = -1;
	}

	if( psp_sound_handle >= 0 )
	{
		sceAudioChRelease( psp_sound_handle );
	}
}
#endif

///////////////////////////////////////////////////////////////////////////


////////////////////////////////
////////////////////////////////
#define PSP222CLOCK scePowerSetClockFrequency(222,222,111)
#define PSP333CLOCK scePowerSetClockFrequency(333,333,166)

static void PSP_Power_CheckSleep(void)
{
	if( bSleep )
	{
		psp_sound_enable =0;//PSP_Sound_Enable( 0);
		while( bSleep )
		{
			sceDisplayWaitVblankStart();
		}
		if(AUDIO_OFF==menu_value[MENU_AUDIO])
			psp_sound_enable = 0;/* sound OFF */
		else
			psp_sound_enable = 1;/* sound ON */

	//	psp_sound_enable =1;//emc->sound_on;//PSP_Sound_Enable( emc->sound_on);
//		return 1;
	}
//	return 0;
}

#if 00
// セーブ処理
static void saves()
{
	PSP222CLOCK;

	sceKernelPowerLock(0);
	/* ここでファイルをアクセス */
	sceKernelPowerUnlock(0);
}
#endif

// ホームボタン終了時にコールバック
int exit_callback(void)
{
	bSleep=1;
// Cleanup the games resources etc (if required)
	PSP222CLOCK;//saves();
// Exit game
	sceKernelExitGame();
	return 0;
}

// 電源スイッチ操作時や不定期にコールバック。
// この関数がまだ実行中でもサスペンド・スタンバイに入る可能性がある。
void power_callback(int unknown, int pwrflags)
{
	// Combine pwrflags and the above defined masks
	int cbid;
	if(pwrflags & POWER_CB_POWER)
	{
		if( !bSleep )
		{
			bSleep=1;

			// ファイルアクセス中にサスペンド・スタンバイされて
			// 0byteのセーブファイルができてしまうことがあるので、
			// 書き込み中はサスペンド・スタンバイを無効化。
			sceKernelPowerLock(0);
			PSP222CLOCK;//saves();
			sceKernelPowerUnlock(0);
		}
	}
	else if(pwrflags & POWER_CB_BATLOW)
	{
		//if( EmuSysCfg_Get()->option_flags & OPTION_LOWBATT )
		{
			if( !bSleep )
			{
				bSleep=1;

				sceKernelPowerLock(0);
				PSP222CLOCK;//saves();
				sceKernelPowerUnlock(0);

			// 強制サスペンド。
			// バッテリが約10%を切りパワーランプが点滅を始めると、
			// 動作が極端に遅くなりフリーズしたりセーブできなくなったりする。
			// 市販ゲームでは0%まで使えてるようなのが謎。
				scePowerRequestSuspend();
			}
		}
	}
	else if(pwrflags & POWER_CB_RESCOMP)
	{
		bSleep=0;
		PSP333CLOCK;
	}
	/*
	else if( pwrflags & POWER_CB_BATTPOWER )
	{
		if( EmuSysCfg_Get()->option_flags & OPTION_SHOW_BATT )
		{
		}
	}
	*/

	// コールバック関数の再登録
	// （一度呼ばれたら再登録しておかないと次にコールバックされない）
	cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
	scePowerRegisterCallback(0, cbid);
}


// （コールバックされる）ポーリング用スレッド
static int CallbackThread(/*SceSize*/unsigned int args, void *argp)
{
	int cbid;

	// コールバック関数の登録
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
	scePowerRegisterCallback(0, cbid);

	// ポーリング
	sceKernelSleepThreadCB();
	return 0;
}

/* Sets up the callback thread and returns its thread id */
static void SetupCallbacks(void)
{
	int thid;// = 0;
//	v_sync();
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
	  sceKernelStartThread(thid, 0, 0);
	}
//	return thid;
}



static void	xinit(void)
{
	rom_loaded=0;
	inited=0;
	//pgiInit();
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(1);

	pgInit();	/*	PSP_Screen_Init  ( );*/
	SetupCallbacks();	//Power init
	pgScreenFrame(2,0);
	Unzip_setCallback(funcUnzipCallback);

	now_sound_samples=PSP_SOUND_SAMPLE;
	PSP_Sound_Init();
	wavoutInit();
//	PSP_Sound_Enable(0,1);


////////menu_init();
//	ini_read();
//	mh_strncpy( now_path, ini_data.path_rom, MAXPATH);
	mh_strncpy( now_path, path_main, MAXPATH);

	{
	char *m;
		m = now_path;
		now_depth=0;
		while(1){
			if(!*m)break;
			else
			if(*m++=='/'){
				now_depth++;
			}
		}
		if( now_depth )now_depth--;
	}
	menu_file_Get_DirList();
	dlist_start  = 0;
	dlist_curpos = 0;
	menu_page=PAGE_FILER;


	menu_value_default[MENU_SHOW_FPS]=INIT_SHOW_FPS;
	menu_value_default[MENU_FPS   ]=INIT_FPS;
	menu_value_default[MENU_SCREEN]=INIT_SCREEN;
	menu_value_default[MENU_AUDIO ]=INIT_AUDIO;
	menu_value_default[MENU_RAPID ]=INIT_RAPID;
}





#ifdef SUPPORT_AUTO_FPS_MODE

static unsigned char calc_auto_vsync_skips(void) /* 自動フレームスキップ */
{
unsigned char m;
//#define ADD40 40
#define ADD40 60
	{
	static int f=0;
	static int t=0;
//	int c;
	//	if( ctl.frame+(TM_1FRAME*30) < syorioti ){ //カウンタ折り返し	4294967295
	//	}

		m=1;
//	m=0;m++;
		t++;
		if( 60==t ){syorioti += ADD40; t=0;}
		syorioti += TM_1FRAME;

		if( ctl.frame < syorioti ){ //合格
			if( !f ){
				while( ctl.frame < syorioti ){
					sceCtrlReadBufferPositive(&ctl,1);
				}
			}else{
				f=0;
			}
		}else{
			f=1;
			while( ctl.frame >= syorioti ){
				m++;
				t++;
				if( 60==t ){syorioti += ADD40; t=0;}
				syorioti += TM_1FRAME;
			}
		}
	}
	switch(m){
	case 0:
	case 1:	m=1;break;
	case 2:	m=2;break;
	case 3:	m=3;break;
	case 4:	m=4;break;
	case 5:	m=5;break;
	default:
	case 6:	m=6;break;
	}
//	if(m>7) m=7;
	return m;
}
#undef ADD40
#endif



static void   cls_screenset(void)
{
	CLS_00;	/* cls */// debugscreenset();
	if( cls_redraw)  cls_redraw--; /* because fliped(2screen) */
}




static void system_reset_value(void){
	 cls_redraw =2; /* cls    flag */ /* because fliped(2screen) */
	menu_redraw =2; /* redraw flag */ /* because fliped(2screen) */

	menu_mode=MENU_SCREEN;	/* start at screen changes. 初期状態は、画面切り替えメニュー */

	menu_value[MENU_SHOW_FPS]	=menu_value_default[MENU_SHOW_FPS];
	menu_value[MENU_FPS   ]		=menu_value_default[MENU_FPS   ];
	menu_value[MENU_SCREEN]		=menu_value_default[MENU_SCREEN];
	menu_value[MENU_AUDIO ]		=menu_value_default[MENU_AUDIO ];
	menu_value[MENU_RAPID ]		=menu_value_default[MENU_RAPID ];
	audio_rate();

	system_reset();
}


extern uint32 input_md_pad0;
int update_input(void)
{
static signed char rapid_ct;	/* need static */
unsigned char flag_aaa;
/*
	int running = 1;

	keystate = SDL_GetKeyState(NULL);

	memset(&input, 0, sizeof(t_input));
	if(keystate[SDLK_UP])	  input_md_pad0 |= INPUT_UP;
	else
	if(keystate[SDLK_DOWN])   input_md_pad0 |= INPUT_DOWN;
	if(keystate[SDLK_LEFT])   input_md_pad0 |= INPUT_LEFT;
	else
	if(keystate[SDLK_RIGHT])  input_md_pad0 |= INPUT_RIGHT;

	if(keystate[SDLK_a])	  input_md_pad0 |= INPUT_A;
	if(keystate[SDLK_s])	  input_md_pad0 |= INPUT_B;
	if(keystate[SDLK_d])	  input_md_pad0 |= INPUT_C;
	if(keystate[SDLK_f])	  input_md_pad0 |= INPUT_START;
	if(keystate[SDLK_z])	  input_md_pad0 |= INPUT_X;
	if(keystate[SDLK_x])	  input_md_pad0 |= INPUT_Y;
	if(keystate[SDLK_c])	  input_md_pad0 |= INPUT_Z;
	if(keystate[SDLK_v])	  input_md_pad0 |= INPUT_MODE;

	if(keystate[SDLK_TAB])	  system_reset();

	if(keystate[SDLK_ESCAPE]) running = 0;
	return (running);
*/

	pad_read();
	/* 特殊キーのチェック */
	if(PSP_SCREEN_MINI==menu_value[MENU_SCREEN]){	//1xモードは操作が特殊。
	/* only changes at screen normal. */
	/* 原寸画面の場合の特殊操作（デバッグ用ミニメニュー） */
		if( ps_E && is_E )
		{
			/* menu mode changed */
			menu_mode++;
			menu_mode &= (MENU_MAX-1);
			 cls_redraw =2; /* cls    flag */ /* because fliped(2screen) */
			menu_redraw =2; /* redraw flag */ /* because fliped(2screen) */
		}
		else if( ps_S && is_S )	/* menu value changed -- */
		{
			menu_value[menu_mode]--;
			fix_menu_value();
			 cls_redraw =2; /* cls    flag */ /* because fliped(2screen) */
			menu_redraw =2; /* redraw flag */ /* because fliped(2screen) */
		}
	}else{
	//1xモード以外の操作。
		switch(is_PAD){
		case CTRL_SELECT:
		case CTRL_START:
			return	1;	/* メニューに戻る */
		}
	}
	switch(is_PAD){
	case (CTRL_TRIANGLE|CTRL_LTRIGGER|CTRL_RTRIGGER):
		/* △（メガドラのスタート）＋Ｌ＋Ｒで、リセット。 */
		system_reset_value();
		break;
	case (CTRL_LTRIGGER|CTRL_RTRIGGER):	/* Ｌ＋Ｒ */
		return	1;	/* メニューに戻る */
	}
	//my_memset0(&input,  sizeof(t_input));
	input_md_pad0=0;
//	if( pad_data&CTRL_LTRIGGER )	{}
//	if( pad_data&CTRL_START )		input_md_pad0 |= INPUT_START;

	if( is_U )	input_md_pad0 |= INPUT_UP;
	if( is_J )	input_md_pad0 |= INPUT_RIGHT;
	if( is_D )	input_md_pad0 |= INPUT_DOWN;
	if( is_F )	input_md_pad0 |= INPUT_LEFT;

	if( is_A )	input_md_pad0 |= INPUT_START;

	/* ＬかＲキーで連射。 */
	flag_aaa=0;	/* 連射未発生 */
	if(is_L|is_R){
		/* シンクロ連射装置 */
		if(menu_value[MENU_RAPID]){/* Auto Rapid mode? */
			if( rapid_ct<0 ){ /* 連射発生なら */
				rapid_ct=(menu_value[MENU_RAPID]);	/* 設定値をカウンタセット */
				rapid_ct++;
				flag_aaa=1;	/* 連射発生 */
			}
			rapid_ct--;
		}else{
			/* 連射装置、判定対策用。連射 */
			if(((ctl.frame)&1)==1){	flag_aaa=1;	}	/* 不定期に、連射発生 */
		}
	}
	if(flag_aaa){	/* 連射発生時（連射はトリガーOFF） */
#if 00
		if( is_O )	input_md_pad0 &= (~INPUT_C);
		if( is_X )	input_md_pad0 &= (~INPUT_B);
		if( is_Q )	input_md_pad0 &= (~INPUT_A);
#else
					input_md_pad0 &= (~(INPUT_A|INPUT_B|INPUT_C|INPUT_START));
#endif
	}else{		/* 通常時 */
		if( is_O )	input_md_pad0 |= INPUT_C;
		if( is_X )	input_md_pad0 |= INPUT_B;
		if( is_Q )	input_md_pad0 |= INPUT_A;
	}

	return	0;
}

/////////////////
static unsigned long tm_ed     = 0;
static unsigned long tm_pd     = 0;
static unsigned long tm_60     = 0;

static unsigned long tm_st     = 0;
static          long tm_res    = 0;

static unsigned char skip_flg  = 0;//フレームスキップフラグ。0:not_skip, 1:skip, ... this frame.
static unsigned char skip_cfg  = 0;//フレームスキップ設定値。(configurated value.)
static unsigned char skip_tmp  = 0;//フレームスキップ一時値。(work, tempolary value.)
static unsigned char fps       = 0;

       unsigned char fps_last;
//static unsigned char fps_last;
/////////////////

static void ShowFps2(int mode);
void xmain(int argc, char *argv)
{
	{
		char *p;
		_strcpy(path_main, argv);
		p = _strrchr(path_main, '/');
		p++;	*p = 0;
	}

	xinit();

	while( 1 ){//「初期化→実行」のループ。
		psp_sound_enable = 0;//PSP_Sound_Enable( 0);
		PSP222CLOCK;
		menu_system();	/* the Filer & setting */

		if(!inited)
		{
			my_memset0(&bitmap,  sizeof(t_bitmap));
			//bitmap.width  = 512;
			//bitmap.height = 512;
		//	bitmap.depth  = 15;
		//	bitmap.granularity = 2;
		//	bitmap.pitch  = (bitmap.width * 2 /*bitmap.granularity*/ );
		//	bitmap.pitch  = (512 * 2 /*bitmap.granularity*/ );
			//bitmap.data   = (unsigned char *)vram_buffer;//bmp->pixels;
		//	bitmap.viewport.w = 256;
		//	bitmap.viewport.h = 224;
			//BITMAP_VIEWPORT_X = 0x20;
			//bitmap.viewport.y = 0x00;
//			bitmap.remap = 1;

			system_init();			/* initialize genesis(mega drive) system */
			system_reset_value();	//リセット
			inited=1;
		}

		if(AUDIO_OFF==menu_value[MENU_AUDIO])
			psp_sound_enable = 0;/* sound OFF */
		else
			psp_sound_enable = 1;/* sound ON */
		PSP333CLOCK;

#ifdef SUPPORT_AUTO_FPS_MODE
		sceCtrlReadBufferPositive(&ctl,1);	//need it???
		syorioti   =ctl.frame-(TM_1FRAME*3);
#endif

/////////////////
	fps      = 0;
	fps_last = 0;
	skip_flg = 0;
	skip_cfg = menu_value[MENU_FPS	];//emc->skip_count;
	tm_ed    = sceKernelLibcClock();
	tm_res   = 0;
/////////////////
	sceKernelDcacheWritebackAll();
/////////////////

		//wavoutStartPlay1();

		{//「実行」のスコープ。
		int running = 1;
			while(running){//「実行」のループ。
//				unsigned char i;

/////////////////
		fps++;
		tm_st = tm_ed;

		if( (tm_ed - tm_pd) > TM_1FRAME )
		{
			tm_pd = tm_ed;
#if 000
			sceCtrlReadBufferPositive( &psp_pad_data, 1 );//	PSP_Ctrlpad_Update();

			switch(/*PSP_Ctrlpad_GetButtons()*/(psp_pad_data.buttons)
					&(PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER|PSP_CTRL_START|PSP_CTRL_SELECT))/*調べないキーはマスク */
			{
			case (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER): /* L+R同時押し */
			case PSP_CTRL_START:/* 設定メニューキー、スタートかセレクトでメニュー */
			case PSP_CTRL_SELECT:
//				resize_image_320to80( PSP_Screen_GetBackBuffer( 80, 16+8 ), &state_temp_image[0][0] );
				resize_image_320to80( PSP_Screen_GetBackBuffer_offs( 80, 16+8 ), &state_temp_image[0][0] );
				goto _MAIN_MENU;//ret = 2;

				break;
			default:
				md_setpaddata( &psp_pad_data/*PSP_Ctrlpad_GetPadData()*/, emc );
				break;
			}
#endif
				//sceCtrlRead(&ctl,1);
				if( update_input() ) break;
		}
		if( (tm_ed - tm_60) > (TM_1FRAME*60) )
		{
			fps_last = fps;
			fps      = 0;
			tm_60    = tm_ed;
		}
/////////////////


				//frame_count++;
#if 00
				if(AUTO_FPS==menu_value[MENU_FPS	]){
					i=calc_auto_vsync_skips();
				}else{
					i=menu_value[MENU_FPS	]+1; /* 1: force no flame skips */
				}

			//	if( i>10 )i=10;

//				if( ini_frame ){
//					i=ini_frame;
//				}

				while( i-- ){
					if(!md_run_frame(i/*, NULL*/))system_reset();
				}
#else
		if( (skip_flg == 0) /*|| (skip_cfg == 0)*/ )
		{	//通常時
			//if(!md_run_frame(0/*, NULL*/))system_reset();
			md_run_frame(0);

/* もし、VSYNC 付けるとしたら、この辺かな。 */

				//今本当は４段ネスト。
{/* start of score [A].字下げが、めんどいし、ここからスコープ */
unsigned int *v;
//	v = (void *)vram_buffer;
	v = ((void*)0x04100000);
	v += (16/*+4*/);	/* ????? at 4 for Dangerrous seed with engine bug */
/* ここのオフセット、良くワカンナイ。？？？ */
					/* genesis 0x20 obj render offset.(vdp.c)  */

//MEMO:x v += ((pg_drawframe)?(0):(512*256/2)); /* the harf of bitmap buffer */
//MEMO:  v += ((pg_drawframe)?(512*256/2):(0)); /* the harf of bitmap buffer */

	switch(menu_value[MENU_SCREEN]){
	default:
	case PSP_SCREEN_MINI: /* NORMAL.原寸サイズ */
			/* render */
			{
				int x,y;
				int mx,my;
				unsigned int *vr;
				mx=(480-320)/2;
				my=(272-224)/2;
				vr = (void *)pgGetVramAddr(mx,my);
			/*	v += 16;*/
//				v += (0*512/2);
				for(y=0;y<224;y++){
					for(x=0;x<(320/2);x++){
						*vr++=*v++;
					}
					v  += ((     512-320)>>1);
					vr += ((LINESIZE-320)>>1);
				}
			}
		break;
	case PSP_SCREEN_FIT: /* FIT with aspect rate.縦横比率を考えて拡大 */
#define ASPECT_RATE_WITH_WIDE (PSP_SCREEN_WIDTH*224/272)
				if( vdp_reg[12]&1 )	PSP_Screen_BitBltGe00((unsigned char*)v,320, 224,	(ASPECT_RATE_WITH_WIDE), PSP_SCREEN_HEIGHT );
				else				PSP_Screen_BitBltGe00((unsigned char*)v,256, 224,	(ASPECT_RATE_WITH_WIDE), PSP_SCREEN_HEIGHT );
#undef ASPECT_RATE_WITH_WIDE
		break;
	case PSP_SCREEN_FULL: /* FULL without aspects.縦横比率は無視して拡大 */
				if( vdp_reg[12]&1 )	PSP_Screen_BitBltGe00((unsigned char*)v,320, 224,	PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT );
				else				PSP_Screen_BitBltGe00((unsigned char*)v,256, 224,	PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT );
		break;
/* OVER with aspect rate.縦横比率を考えて拡大。但し上や下がはみ出す。 */
#define ASPECT_RATE_WITH_HEIGHT (224*320/480)
//MEMO:#define DIFF_LASTERS            (224-ASPECT_RATE_WITH_HEIGHT)
//MEMO:まだ上手い分割案を考えてない
//MEMO: all_v += ((PSP_LINESIZE*(224-ASPECT_RATE_WITH_HEIGHT)/2  )  );
//MEMO: 75=DIFF_LASTERS     （取り敢えずこの辺が妥当かな）
//MEMO: 75=19+19+19+18              p5 at div4
//MEMO: 75=13+12+13+12+13+12        p7 at div6
//MEMO: 75= 9+10+ 9+10+ 9+ 9+10+ 9  p9 at div8
	case PSP_SCREEN_0OVER: /* OVER 3 (bottom)下側 */
				v += ((LINESIZE*(19) )/2 );
		/* not break; */
	case PSP_SCREEN_1OVER: /* OVER 4 (near_bottom)下より */
				v += ((LINESIZE*(19) )/2 );
		/* not break; */
	case PSP_SCREEN_2OVER: /* OVER 5 (middle)真中 */
				v += ((LINESIZE*(19) )/2 );
		/* not break; */
	case PSP_SCREEN_3OVER: /* OVER 6 (near_top)上の方 */
				v += ((LINESIZE*(18) )/2 );
		/* not break; */
	case PSP_SCREEN_4OVER: /* OVER 7 (top)上 */
				if( vdp_reg[12]&1 )	PSP_Screen_BitBltGe00((unsigned char*)v,320, ASPECT_RATE_WITH_HEIGHT,	PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT );
				else			PSP_Screen_BitBltGe00((unsigned char*)v,256, ASPECT_RATE_WITH_HEIGHT,	PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT );
#undef ASPECT_RATE_WITH_HEIGHT
		break;
	}
}/*ends of score [A].スコープお終い */
				//今本当は４段ネスト。

				PSP_Power_CheckSleep();

				if(PSP_SCREEN_MINI==menu_value[MENU_SCREEN]){ /* NORMAL mode only */
					if( pg_drawframe )	{	/*v_sync();*/pgScreenFlip();}
					else				pgScreenFlip();
					if( cls_redraw)
					{
						cls_screenset();
					}
					if(menu_value[MENU_SHOW_FPS]){
						if(menu_redraw) {
//							debug_screenset();
//static void debug_screenset(void)
//{
//	//pgFillvram(RGB2COL(10,10,10));	/* cls */
//	text_print( X_STR_00, Y_STR_SKIP,  "FRAME SKIPS"	, COLOR_HCYAN,COLOR_BLACK,FILLED);
//	text_print( X_STR_00, Y_STR_NUMB,  "MENU num"		, COLOR_HCYAN,COLOR_BLACK,FILLED);
//}
	/* infomation mini menu */
	menu_line(20+X_STR_55, Y_STR_DEBUG,menu_mode,COLOR_WHITE,menu_value[menu_mode]);
	if(menu_redraw) menu_redraw--; /* because fliped(2screen) */
						}
					}
				}else{	/* Ge mode.拡大モード時はFlipを使わない */
					sceDisplaySetFrameBuf((unsigned char *)0x04000000/*+FRAMESIZE*/,LINESIZE,PIXELSIZE,0);
				}
				//	pgScreenFlip();

			//	v_sync_e();
				/* calc fps, and view menu.ＦＰＳの計算とメニュー表示 */
				{
				//	static unsigned int f=0;
					static unsigned char tt=0;
				//	unsigned int c;

					tt++;
					tt &= 0x0f;
				//	c=ctl.frame-f;
					if(0==tt){
						if(menu_value[MENU_SHOW_FPS]){
							//fps_last=(1000000/(c));
							if(PSP_SCREEN_MINI==menu_value[MENU_SCREEN]){
								text_print( X_STR_00, Y_STR_FPS,   "   FPS"			, COLOR_HCYAN,COLOR_BLACK,FILLED);
								ShowFps2(1);
							}else{ShowFps2(0);}
						//	text_counter( 40+X_STR_00, Y_STR_FPS, 1000000/(a/60), 3, -1);
//(?)						text_counter( X_STR_00, Y_STR_SKIP, syorioti_c, 2, -1);
						}
					}
				//	if( ini_frame ){
				//	}else{
				//		text_print( X_STR_50, Y_STR_NUMB, "auto", RGB2COL(255,255,255),RGB2COL(0,0,0),-1);
				//	}
				//	f=ctl.frame;
				}/*ends of scope */


		}else	//フレームスキップ時
		{
			skip_flg = 0;
		//	if(!md_run_frame(1/*, NULL*/))system_reset();
			md_run_frame(1);
		}
#endif
/////////////////
		/* タイマー */
		tm_ed  = sceKernelLibcClock();
//static unsigned char calc_skip( void)
//		tm_res += ((TM_1FRAME*60) / 60/*emc->debug_mode*/) - (long)tm_frame;
		tm_res +=  (TM_1FRAME) - (long)(tm_ed - tm_st);

		if( tm_res < 0 )
		{
			if( skip_tmp ==0  ){
				skip_tmp = skip_cfg;
				tm_res = 0;
			}else{
				skip_tmp--;
				skip_flg = 1;
				if(skip_cfg == 0)skip_flg = 0;
			}
		}
		else if( tm_res > TM_1FRAME*2 )
		{
			tm_res = TM_1FRAME*2;
		}
/////////////////


			}/*ends of while(running) */	//「実行」のループ。
		}/*ends of scope */	//「実行」のスコープ。
		//system_shutdown();	/* メガドラをお終いにする。 */
	}/*ends of while(1) */	//「初期化→実行」のループ。

	/* ここに来ることは考えていない。 */
	/* with safty at bug.万が一きたら、クロックを下げて無限ループ */

//	pgErrorHalt("over exec, system error.");
	PSP222CLOCK;
	while (1) { 	v_sync();	}/*ＰＳＰの無限ループはＶＳＹＮＣとか取らないと駄目（ハングしちゃう）*/
	//pgExit(0);
//	return 0;
}
#undef CLS_00

#undef PAGE_FILER
#undef PAGE_SETTING

#undef FILE_DIR_LIST_MODE
#undef FILE_LOADING_MODE

#undef MY_OFF
#undef MY_ON

/* プリプロの負担を軽くする為、後始末。 */


#undef PSP_SCREEN_MINI
#undef PSP_SCREEN_FIT
#undef PSP_SCREEN_FULL
#undef PSP_SCREEN_0OVER

#undef PSP_SCREEN_1OVER
#undef PSP_SCREEN_2OVER
#undef PSP_SCREEN_3OVER
#undef PSP_SCREEN_4OVER

#undef X_STR_00
#undef X_STR_50
#undef X_STR_70
#undef Y_STR_FPS
//#undef Y_STR_SKIP
//#undef Y_STR_NUMB
#undef Y_STR_DEBUG

#undef UNMASK
#undef FILLED
////////////////////


#include "inc_main_text.c"
#include "inc_loadrom.c"



////////////////////
// 現状手抜き実装の為、ＭＥＮＵをＬＯＡＤＲＯＭで使う。
// いつになったらクリーンナップ時間が取れるやら。
#undef MENU_SHOW_FPS
#undef MENU_FPS
#undef MENU_VSYNC
#undef MENU_SCREEN

#undef MENU_AUDIO
#undef MENU_SUMFIX
#undef MENU_RAPID
#undef MENU_RESET

#undef MENU_MAX


