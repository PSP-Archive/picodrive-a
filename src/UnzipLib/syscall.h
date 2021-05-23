/* system call prototype for PSP */

#ifndef _SYSCALL_H_INCLUDED
#define _SYSCALL_H_INCLUDED

#ifndef NULL
#define NULL 0
#endif

#define true 1
#define false 0

#define MAXPATH 512
#define MAXNAME 256
#define MAX_ENTRY 1024

/******************************************************************************/
/* sceDisplay */
/******************************************************************************/
/* sceCtrl */
/******************************************************************************/
/* IoFileMgrForUser */

#define O_RDONLY    0x0001
#define O_WRONLY    0x0002
#define O_RDWR      0x0003
#define O_NBLOCK    0x0010
#define O_APPEND    0x0100
#define O_CREAT     0x0200
#define O_TRUNC     0x0400
#define O_NOWAIT    0x8000

int sceIoOpen(const char* file, int mode, int unknown);
void sceIoClose(int fd);
int sceIoRead(int fd, void *data, int size);
int sceIoWrite(int fd, void *data, int size);
int sceIoLseek(int fd, long long offset, int whence);
int sceIoRemove(const char *file);
int sceIoMkdir(const char *dir, int mode);
int sceIoRmdir(const char *dir);
int sceIoRename(const char *oldname, const char *newname);


enum {
    TYPE_DIR=0x10,
    TYPE_FILE=0x20
};

struct dirent_tm {
	unsigned short unk[2]; //常にゼロ？
	unsigned short year;
	unsigned short mon;
	unsigned short mday;
	unsigned short hour;
	unsigned short min;
	unsigned short sec;
};

struct dirent {
    unsigned long unk0;
    unsigned long type;
    unsigned long size;
	struct dirent_tm ctime; //作成日時
	struct dirent_tm atime; //最終アクセス日時
	struct dirent_tm mtime; //最終更新日時
	unsigned long unk[7]; //常にゼロ？
    char name[0x108];
};

int sceIoDopen(const char *fn);
int sceIoDread(int fd, struct dirent *de);
void sceIoDclose(int fd);

#endif // _SYSCALL_H_INCLUDED
