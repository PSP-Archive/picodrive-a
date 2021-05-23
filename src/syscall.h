/* system call prototype for PSP */

#ifndef _MY_SYSCALL_H_
#define _MY_SYSCALL_H_

/* 完全に標準ライブラリと判明したものをこっちに移す用
（素直にＲＩＮ１３２の奴をコピーしたら、何がなんだか、判らなくなっちゃう） */


/******************************************************************************/
/* sceSuspendForUser */
int sceKernelPowerLock(int locktype);
int sceKernelPowerUnlock(int locktype);

/******************************************************************************/
/* scePower */
#define POWER_CB_POWER		0x80000000
#define POWER_CB_HOLDON 	0x40000000
#define POWER_CB_STANDBY	0x00080000
#define POWER_CB_RESCOMP	0x00040000
#define POWER_CB_RESUME 	0x00020000
#define POWER_CB_SUSPEND	0x00010000
#define POWER_CB_EXT		0x00001000
#define POWER_CB_BATLOW 	0x00000100
#define POWER_CB_BATTERY	0x00000080
#define POWER_CB_BATTPOWER	0x0000007F

long scePowerSetClockFrequency(long,long,long);
void scePowerRegisterCallback(int zero, int cbid);
//void PowerSetCallback(int zero, int cbid);


void scePowerRequestSuspend(void);

/******************************************************************************/
/* sceDmac */
/******************************************************************************/
/* sceDisplay */

void sceDisplayWaitVblankCB(void);

void sceDisplayWaitVblankStart();
void sceDisplaySetMode(long,long,long);
void sceDisplaySetFrameBuf(unsigned char *topaddr,long linesize,long pixelsize,long);

int sceDisplayGetVcount(void);

/******************************************************************************/
/* sceCtrl */
/* Index for the two analog directions */
#define CTRL_ANALOG_X	0
#define CTRL_ANALOG_Y	1

/* Returned control data the buttons */
/* the Button bit masks. */
#define CTRL_SELECT 	0x0001
//efine CTRL_SEL2		0x0002
//efine CTRL_SEL3		0x0004
#define CTRL_START		0x0008
#define CTRL_UP 		0x0010
#define CTRL_RIGHT		0x0020
#define CTRL_DOWN		0x0040
#define CTRL_LEFT		0x0080
#define CTRL_LTRIGGER	0x0100
#define CTRL_RTRIGGER	0x0200
//efine CTRL_L2TRIGGER	0x0400
//efine CTRL_R2TRIGGER	0x0800
#define CTRL_TRIANGLE	0x1000
#define CTRL_CIRCLE 	0x2000
#define CTRL_CROSS		0x4000
#define CTRL_SQUARE 	0x8000

typedef struct _ctrl_data
{
	unsigned long frame;			// 時間(tick)
	unsigned long buttons;
	unsigned char analog[4];		// [0]:X [1]:Y
	unsigned long unused;
} ctrl_data_t;

//
//void  sceCtrlInit(int unknown);
//void  sceCtrlSetAnalogMode(int on);
//void  sceCtrlRead(ctrl_data_t* paddata, int unknown);

void  sceCtrlSetSamplingCycle(int unknown);
void  sceCtrlSetSamplingMode(int on);
void  sceCtrlReadBufferPositive(ctrl_data_t* paddata, int unknown);

/******************************************************************************/
/* IoFileMgrForUser */

//efine OF_RDONLY		0x0001
#define SCE_O_RDONLY	0x0001
#define SCE_O_WRONLY	0x0002
#define SCE_O_RDWR		0x0003
#define SCE_O_NBLOCK	0x0010
#define SCE_O_APPEND	0x0100
#define SCE_O_CREAT 	0x0200
#define SCE_O_TRUNC 	0x0400
#define SCE_O_EXCL      0x0800
#define SCE_O_NOWAIT	0x8000

extern int	sceIoOpen(	const char* file, int mode, int unknown);
extern void sceIoClose( int fd);
extern int	sceIoRead(	int fd, void *data, int size);
extern int	sceIoWrite( int fd, void *data, int size);
extern int	sceIoLseek( int fd, int offset, int whence);

extern int	sceIoRemove(const char *file);
extern int	sceIoMkdir(const char *dir, int mode);

enum {
	TYPE_DIR=0x10,
	TYPE_FILE=0x20
};

typedef struct _PSP_FILE_DATE
{
	unsigned short unk[2];	//常にゼロ？
	unsigned short year;
	unsigned short mon;
	unsigned short mday;
	unsigned short hour;
	unsigned short min;
	unsigned short sec;
}PSP_FILE_DATE ;

typedef struct _PSP_FILE_ATTRIBUTE
{
	unsigned long unk0;		/* mode */
	unsigned long type;
	unsigned long size;
	PSP_FILE_DATE ctime;	//作成日時
	PSP_FILE_DATE atime;	//最終アクセス日時
	PSP_FILE_DATE mtime;	//最終更新日時
	unsigned long unk[7];	//常にゼロ？
    char          name[0x108];
//	unsigned char dmy[128];
}PSP_FILE_ATTRIBUTE  ;

//typedef struct dirent {
//	unsigned long mode;
//	unsigned long type;
//	unsigned long size;
//	unsigned long unk[19];
//	char name[0x108];
//	unsigned char dmy[128];
//} dirent_t;


//int  sceIoRmdir(const char *dir);
//int  sceIoRename(const char *oldname, const char *newname);
//int sceIoDevctl(const char *name int cmd, void *arg, size_t arglen, void *buf, size_t *buflen);
int  sceIoDopen(const char *fn);
//int  sceIoDread(int fd, dirent_t *de);
int  sceIoDread(int fd, PSP_FILE_ATTRIBUTE *de);
void sceIoDclose(int fd);

//int sceIoChdir(const char *path);

/******************************************************************************/
/* StdioForUser */


/******************************************************************************/
/* LoadExecForUser */
void sceKernelExitGame(void);
int sceKernelRegisterExitCallback(int);

/******************************************************************************/
/* ThreadManForUser */
typedef int (*pg_threadfunc_t)(unsigned int args, void *argp); /* 多分 */
//t sceKernelCreateThread(const char *name, void *func, int a,int b,int c,int d);
int sceKernelCreateThread(const char *name, pg_threadfunc_t func, unsigned long priority, unsigned long stacksize, unsigned long attrib, void *unk);

int sceKernelSleepThreadCB(void);
int sceKernelCreateCallback(const char *name, void *func, void *arg);
//int sceKernelCreateCallbacks(const char *name, void *func);


int sceKernelStartThread(int hthread, int arg0, void *arg1);
void sceKernelExitThread(int ret);
int sceKernelWaitThreadEnd(int hthread, void *unk);
int sceKernelDeleteThread(int hthread);


/******************************************************************************/
/* UtilsForUser */
unsigned long sceKernelLibcClock(void);

void sceKernelDcacheWritebackAll(void);

/******************************************************************************/
/* sceGe_user */
typedef struct _GeCB {
	void (*signal_func)(int id, void *arg);
	void *signal_arg;
	void (*finish_func)(int id, void *arg);
	void *finish_arg;
} GeCB;

void *sceGeEdramGetAddr(void);
int sceGeEdramSetAddrTranslation(int unknown);
int sceGeListEnQueue(const void *start, const void *stall, int cbid, const void *unknown);
int sceGeListEnQueueHead(const void *start, const void *stall, int cbid, const void *unknown);
int sceGeListSync(int id, int unknown);
int sceGeDrawSync(int unknown);
int sceGeListUpdateStallAddr(int id, const void *p);
int sceGeSetCallback(GeCB *callback);
int sceGeUnsetCallback(int id);

/******************************************************************************/
/* sceAudio */
int sceAudioOutputBlocking(int unknown);
int sceAudioOutputPanned(int handle, int volL, int volR, char* buf);
int sceAudioOutputPannedBlocking(int handle, int volL, int volR, char* buf);
int sceAudioChReserve(int unknown0, int samples, int unknown1);
void sceAudioChRelease(int handle);
int sceAudioGetChannelRestLen(int unknown, int unknown1);
int sceAudioSetChannelDataLen(int unknown, int unknown1);
int sceAudioChangeChannelConfig(int unknown);
int sceAudioChangeChannelVolume(int unknown);

#endif  // _MY_SYSCALL_H_
