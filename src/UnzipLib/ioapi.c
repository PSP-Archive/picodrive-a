/* ioapi.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   Version 1.01, May 8th, 2004

   Copyright (C) 1998-2004 Gilles Vollant
*/

//�폜#include <stdio.h>
//�폜#include <stdlib.h>
//�폜#include <string.h>

//�ǉ�
#include "zlibType.h"
//�ǉ�
#include "zlibFileMemory.h"

#include "zlib.h"
#include "ioapi.h"




#include "zlibFileMemory.h"
#include "zlibType.h"
#include "zconf.h"
#include "syscall.h"

void* ___memcpy(void *buf1, const void *buf2, int n)
{
	while(n-->0)
		((unsigned char*)buf1)[n] = ((unsigned char*)buf2)[n];
	return buf1;
}



//FILE ___zlibfb;
#define MAX_PATH		  260
#define AllocBuffSize	  65536

//unsigned long long ___zlibTmpBuffSize;

//unsigned char *___zlibTmpBuff;	//��Ɨp�B����zip�������ɓW�J����
//unsigned int	___zlibInZipBuffPointer;		//��L���������ǂ��܂Ŏg������
//unsigned char ___zlibInZipFileName[MAX_PATH]; //��Ɨp�B����zip�������ɓW�J����
//unsigned int	___zlibInZipSize;			//����zip�̑傫��[�o�C�g]

typedef struct {
	int nFileId;			// file id
	unsigned long ulPtr;	// file pointer
}zlibInFile;
static unsigned char ___zlibAllocBuff[AllocBuffSize];	//�������̊m��
static unsigned int  ___zlibAllocBuffPointer;	//��L���������ǂ��܂Ŏg������

static zlibInFile ___zlibInFile = {0, 0};


void ___free( void *memblock )
{
	return;
}

void *___calloc( size_t num, size_t size )
{
	return NULL;
}

void *___malloc( size_t size )
{
	___zlibAllocBuffPointer += size;

	if(___zlibAllocBuffPointer > AllocBuffSize)
	{
		// �������s��
		return NULL;
	}

	return &___zlibAllocBuff[___zlibAllocBuffPointer - size];
}









static voidpf ZOPEN (const char* filename ){
	FILE* file = NULL;

	if ((filename!=NULL) ){
	//	file = ___fopen(filename );
	int fb1;
		fb1 = sceIoOpen(filename,O_RDONLY,0777);
		if(fb1 < 0)
		{
			file=NULL;
		//	break;
		//	return NULL;
		}else{
		___zlibInFile.nFileId = fb1;
		___zlibInFile.ulPtr = 0;
		___zlibAllocBuffPointer = 0;
	//	return ((FILE *)&___zlibInFile);
	//	return ((void *)&___zlibInFile);
	//	file=((FILE *)&___zlibInFile);
		file=((void *)&___zlibInFile);
		}
	}
	return file;
}




static long ___ftell( FILE *stream )
{
	return ___zlibInFile.ulPtr;
}
static long ZTELL (
   voidpf stream
){
	long ret;
	ret = ___ftell((FILE *)stream);
	return ret;
}


/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */

#ifndef SEEK_CUR
#define SEEK_CUR	1
#endif

#ifndef SEEK_END
#define SEEK_END	2
#endif

#ifndef SEEK_SET
#define SEEK_SET	0
#endif

static int ___fseek( FILE *stream, long offset, int origin )
{
	unsigned long ulPtr;
	ulPtr = sceIoLseek(___zlibInFile.nFileId, offset, origin);
	___zlibInFile.ulPtr = ulPtr;
	return ulPtr;
}
static long ZSEEK (
   voidpf stream,
   uLong offset,
   int origin
){
	int fseek_origin=0;
	long ret;


	switch (origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR:	fseek_origin = SEEK_CUR;	break;
	case ZLIB_FILEFUNC_SEEK_END:	fseek_origin = SEEK_END;	break;
	case ZLIB_FILEFUNC_SEEK_SET:	fseek_origin = SEEK_SET;	break;
	default: return -1;
	}
	ret = 0;
	___fseek((FILE *)stream, offset, fseek_origin);
	return ret;
}

static int ___fclose( FILE *stream )
{
	sceIoClose(___zlibInFile.nFileId);
	return 0;
}
static int ZCLOSE (
   voidpf stream
){
	int ret;
	ret = ___fclose((FILE *)stream);
	return ret;
}

//static int ___ferror( FILE *stream )
//{
//	return 0;
//}

static size_t ___fread( void *buffer, size_t size, size_t count, FILE *stream )
{
//	int ret = 0;
	int readSize = size * count;

	readSize = sceIoRead(___zlibInFile.nFileId, buffer, readSize);
	___zlibInFile.ulPtr += readSize;

	return readSize;
}
static uLong ZREAD (
   voidpf stream,
   void* buf,
   uLong size
){
	uLong ret;
	ret = (uLong)___fread(buf, 1, (size_t)size, (FILE *)stream);
	return ret;
}











