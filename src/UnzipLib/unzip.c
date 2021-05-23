
/*	unzip.c -- IO for uncompress .zip files using zlib
	Version 1.01d, September 22th, 2004
	Copyright (C) 1998-2004 Gilles Vollant
	- http://www.winimage.com/zLibDll
	Read unzip.h for more info
*/

/* Decryption code comes from crypt.c by Info-ZIP but has been greatly reduced in terms of
compatibility with older software. The following is from the original crypt.c. Code
woven in by Terry Thorsen 1/2003.
*/
/*
	Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

	See the accompanying file LICENSE, version 2000-Apr-09 or later
	(the contents of which are also included in zip.h) for terms of use.
	If, for some reason, all these files are missing, the Info-ZIP license
	also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
	This encryption code is a direct transcription of the algorithm from
	Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
	file (appnote.txt) is distributed with the PKZIP program (even in the
	version without encryption capabilities).
 */

//追加
#include "zlibType.h"
//追加
#include "zlibFileMemory.h"

#include "zlib.h"
#include "unzip.h"

//#include "zlibFileMemory.c"
#include "ioapi.c"

#ifdef NO_ERRNO_H
	extern int errno;
#else
//削除#include <errno.h>
#endif

/* compile with -Dlocal if your debugger can't find static symbols */

#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif

#ifndef UNZ_MAXFILENAMEINZIP
#define UNZ_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
#define ALLOC(size) (___malloc(size))
#endif
#ifndef TRYFREE
#define TRYFREE(p) {if(p) ___free(p);}
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)


/* unz_file_info_interntal contain internal info about a file in zipfile*/
typedef struct unz_file_info_internal_s
{
	uLong offset_curfile;	/* relative offset of local header 4 bytes */
} unz_file_info_internal;


/* file_in_zip_read_info_s contain internal information about a file in zipfile,
	when reading and decompress it */
typedef struct
{
	char  *read_buffer; 			/* internal buffer for compressed data */
	z_stream stream;				/* zLib stream structure for inflate */

	uLong pos_in_zipfile;			/* position in byte on the zipfile, for fseek*/
	uLong stream_initialised;		/* flag set if stream structure is initialised*/

	uLong offset_local_extrafield;	/* offset of the local extra field */
	uInt  size_local_extrafield;	/* size of the local extra field */
	uLong pos_local_extrafield;		/* position in the local extra field in read*/

	uLong crc32;					/* crc32 of all data uncompressed */
	uLong crc32_wait;				/* crc32 we must obtain after decompress all */
	uLong rest_read_compressed; 	/* number of byte to be decompressed */
	uLong rest_read_uncompressed;	/*number of byte to be obtained after decomp*/
//	zlib_filefunc_def z_filefunc;
	voidpf filestream;				/* io structore of the zipfile */
	uLong compression_method;		/* compression method (0==store) */
	uLong byte_before_the_zipfile;	/* byte before the zipfile, (>0 for sfx)*/
	int   raw;
} file_in_zip_read_info_s;


/* unz_s contain internal information about the zipfile
*/
typedef struct
{
//	zlib_filefunc_def z_filefunc;
	voidpf filestream;				/* io structore of the zipfile */
	unz_global_info gi; 			/* public global information */
	uLong byte_before_the_zipfile;	/* byte before the zipfile, (>0 for sfx)*/
	uLong num_file; 				/* number of the current file in the zipfile*/
	uLong pos_in_central_dir;		/* pos of the current file in the central dir*/
	uLong current_file_ok;			/* flag about the usability of the current file*/
	uLong central_pos;				/* position of the beginning of the central dir*/

	uLong size_central_dir; 		/* size of the central directory  */
	uLong offset_central_dir;		/* offset of start of central directory with
									   respect to the starting disk number */

	unz_file_info cur_file_info;	/* public info about the current file in zip*/
	unz_file_info_internal cur_file_info_internal; /* private info about it*/
	file_in_zip_read_info_s* pfile_in_zip_read; /* structure about the current
													file if we are decompressing it */
//	int encrypted;
#define NOUNCRYPT

} unz_s;

/* ===========================================================================
	 Read a byte from a gz_stream; update next_in and avail_in. Return EOF
   for end of file.
   IN assertion: the stream s has been sucessfully opened for reading.
*/

static int unzlocal_getByte(

	voidpf filestream,
	int *pi
){
	unsigned char c;
	int err = (int)ZREAD( filestream,&c,1);
	if(err==1)
	{
		*pi = (int)c;
		return UNZ_OK;
	}
	else
	{
		//if(ZERROR( filestream))
		//	return UNZ_ERRNO;
		//else
			return UNZ_EOF;
	}
}


/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets
*/
static int unzlocal_getShort(

	voidpf filestream,
	uLong *pX
)
{
	uLong x ;
	int i=0;
	int err;

	err = unzlocal_getByte(  filestream,&i);
	x = (uLong)i;

	if(err==UNZ_OK)
		err = unzlocal_getByte(  filestream,&i);
	x += ((uLong)i)<<8;

	if(err==UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}


static int unzlocal_getLong(

	voidpf filestream,
	uLong *pX
)
{
	uLong x ;
	int i=0;
	int err;

	err = unzlocal_getByte(  filestream,&i);
	x = (uLong)i;

	if(err==UNZ_OK)
		err = unzlocal_getByte(  filestream,&i);
	x += ((uLong)i)<<8;

	if(err==UNZ_OK)
		err = unzlocal_getByte(  filestream,&i);
	x += ((uLong)i)<<16;

	if(err==UNZ_OK)
		err = unzlocal_getByte(  filestream,&i);
	x += ((uLong)i)<<24;

	if(err==UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}


#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif





/*
   Translate date/time from Dos format to tm_unz (readable more easilty)
*/
/*local*/
//static void unzlocal_DosDateToTmuDate(
//	uLong ulDosDate,
//	tm_unz* ptm
//){
//	uLong uDate;
//	uDate = (uLong)(ulDosDate>>16);
//	ptm->tm_mday = (uInt)(uDate&0x1f);
//	ptm->tm_mon  = (uInt)((((uDate)&0x1E0)/0x20)-1);
//	ptm->tm_year = (uInt)(((uDate&0x0FE00)/0x0200)+1980);
//
//	ptm->tm_hour = (uInt) ((ulDosDate &0xF800)/0x800);
//	ptm->tm_min  = (uInt) ((ulDosDate&0x7E0)/0x20);
//	ptm->tm_sec  = (uInt) (2*(ulDosDate&0x1f));
//}
/*
  Get Info about the current file in the zipfile, with internal only info
*/
static int unzlocal_GetCurrentFileInfoInternal(
	unzFile file,
	unz_file_info *pfile_info,
	unz_file_info_internal *pfile_info_internal,
	char *szFileName,
	uLong fileNameBufferSize
)
//	  char *szComment;
//	  uLong commentBufferSize;
{
	unz_s* s;
	unz_file_info file_info;
	unz_file_info_internal file_info_internal;
	int err=UNZ_OK;
	uLong uMagic;
	long lSeek=0;

	if(file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	if(ZSEEK(  s->filestream,
			  s->pos_in_central_dir+s->byte_before_the_zipfile,
			  ZLIB_FILEFUNC_SEEK_SET)!=0)
		err=UNZ_ERRNO;


	/* we check the magic */
	if(err==UNZ_OK){
		if(unzlocal_getLong(   s->filestream,&uMagic) != UNZ_OK)
			err=UNZ_ERRNO;
		else if(uMagic!=0x02014b50)
			err=UNZ_BADZIPFILE;
	}

	if(unzlocal_getShort(  s->filestream,&file_info.version)					!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.version_needed)			!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.flag)						!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.compression_method)		!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getLong(   s->filestream,&file_info.dosDate)					!= UNZ_OK) err=UNZ_ERRNO;

//	unzlocal_DosDateToTmuDate(file_info.dosDate,&file_info.tmu_date);

	if(unzlocal_getLong(   s->filestream,&file_info.crc)						!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getLong(   s->filestream,&file_info.compressed_size)			!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getLong(   s->filestream,&file_info.uncompressed_size)		!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.size_filename)			!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.size_file_extra)			!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.size_file_comment)		!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.disk_num_start)			!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getShort(  s->filestream,&file_info.internal_fa)				!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getLong(   s->filestream,&file_info.external_fa)				!= UNZ_OK) err=UNZ_ERRNO;
	if(unzlocal_getLong(   s->filestream,&file_info_internal.offset_curfile)	!= UNZ_OK) err=UNZ_ERRNO;

	lSeek+=file_info.size_filename;
	if((err==UNZ_OK) && (szFileName!=NULL))
	{
		uLong uSizeRead ;
		if(file_info.size_filename<fileNameBufferSize)
		{
			*(szFileName+file_info.size_filename)='\0';
			uSizeRead = file_info.size_filename;
		}
		else
			uSizeRead = fileNameBufferSize;

		if((file_info.size_filename>0) && (fileNameBufferSize>0))
			if(ZREAD(  s->filestream,szFileName,uSizeRead)!=uSizeRead)
				err=UNZ_ERRNO;
		lSeek -= uSizeRead;
	}
	lSeek+=file_info.size_file_extra;
	lSeek+=file_info.size_file_comment;

	if((err==UNZ_OK) && (pfile_info!=NULL))
		*pfile_info=file_info;

	if((err==UNZ_OK) && (pfile_info_internal!=NULL))
		*pfile_info_internal=file_info_internal;

	return err;
}

//////////////////////////
/*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/
/*lib_main.c local */
static int ZEXPORT unzGoToFirstFile(
	unzFile file
){
	int err=UNZ_OK;
	unz_s* s;

	if(file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	s->pos_in_central_dir=s->offset_central_dir;
	s->num_file=0;

	err=unzlocal_GetCurrentFileInfoInternal(
file,
&s->cur_file_info,
&s->cur_file_info_internal,
NULL,0 );

	s->current_file_ok = (err == UNZ_OK);
	return err;
}

/*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/
/*lib_main.c*/
static int ZEXPORT unzGoToNextFile(
	unzFile file
){
	unz_s* s;
	int err;

	if(file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	if(!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;
	if(s->gi.number_entry != 0xffff)	/* 2^16 files overflow hack */
	  if(s->num_file+1==s->gi.number_entry)
		return UNZ_END_OF_LIST_OF_FILE;

	s->pos_in_central_dir += (
		SIZECENTRALDIRITEM +
		s->cur_file_info.size_filename +
		s->cur_file_info.size_file_extra +
		s->cur_file_info.size_file_comment) ;

	s->num_file++;
	err = unzlocal_GetCurrentFileInfoInternal(
file,&s->cur_file_info,
&s->cur_file_info_internal,
NULL,0 );
	s->current_file_ok = (err == UNZ_OK);
	return err;
}
/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem.
*/
/*lib_main.c*/
static int ZEXPORT unzGetCurrentFileInfo(
	unzFile file,
	unz_file_info *pfile_info,
	char *szFileName,
	uLong fileNameBufferSize
)
{
	return unzlocal_GetCurrentFileInfoInternal(
file,
pfile_info,
NULL,
szFileName,
fileNameBufferSize
);
}
///////////////////////////



/*
  Locate the Central directory of a zipfile (at the end, just before
	the global comment)
*/
/*local:unzOpen()*/
static uLong unzlocal_SearchCentralDir(

	voidpf filestream
)
{
	unsigned char buf[BUFREADCOMMENT+4];
	uLong uSizeFile;
	uLong uBackRead;
	uLong uMaxBack=0xffff; /* maximum size of global comment */
	uLong uPosFound=0;

	if(ZSEEK(  filestream,0,ZLIB_FILEFUNC_SEEK_END) != 0)
		return 0;


	uSizeFile = ZTELL(  filestream);

	if(uMaxBack>uSizeFile)
		uMaxBack = uSizeFile;

//	  buf = (unsigned char*)ALLOC(BUFREADCOMMENT+4);
//	  if(buf==NULL)
//		  return 0;

	uBackRead = 4;
	while (uBackRead<uMaxBack)
	{
		uLong uReadSize,uReadPos;
		if(uBackRead+BUFREADCOMMENT>uMaxBack)
			uBackRead = uMaxBack;
		else
			uBackRead+=BUFREADCOMMENT;
		uReadPos = uSizeFile-uBackRead;

		uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ?
					 (BUFREADCOMMENT+4) : (uSizeFile-uReadPos);

		if(ZSEEK(  filestream,uReadPos,ZLIB_FILEFUNC_SEEK_SET)!=0)
			break;

		if(ZREAD(  filestream,buf,uReadSize) !=uReadSize )
			break;

		{
		int i;
		for(i=(int)uReadSize-3; (i--)>0;)
			if(
				((*(buf+i  ))=='P'/*0x50*/) &&
				((*(buf+i+1))=='K'/*0x4b*/) &&
				((*(buf+i+2))==0x05) &&
				((*(buf+i+3))==0x06)
			){
				uPosFound = uReadPos+i;
				break;
			}
		}

		if(uPosFound!=0)
			break;
	}
//	  TRYFREE(buf);
	return uPosFound;
}

/*
  Open a Zip file. path contain the full pathname (by example,
	 on a Windows NT computer "c:\\test\\zlib114.zip" or on an Unix computer
	 "zlib/zlib114.zip".
	 If the zipfile cannot be opened (file doesn't exist or in not valid), the
	   return value is NULL.
	 Else, the return value is a unzFile Handle, usable with other function
	   of this unzip package.
*/
/*lib_main()*/
static unzFile ZEXPORT unzOpen(const char *path)
//	zlib_filefunc_def* pzlib_filefunc_def;
{
	unz_s us;
	unz_s *s;
	uLong central_pos,uL;

/* number of the current dist, used for spaning ZIP, unsupported, always 0*/
	uLong number_disk;
/* number the the disk with central dir, used for spaning ZIP, unsupported, always 0*/
	uLong number_disk_with_CD;
/* total number of entries in the central dir (same than number_entry on nospan) */
	uLong number_entry_CD;

	int err=UNZ_OK;

//	if(pzlib_filefunc_def==NULL)
	//	fill_fopen_filefunc(&us.z_filefunc);
//	else
//		us.z_filefunc = *pzlib_filefunc_def;

	us.filestream= ZOPEN(path );
	if(us.filestream==NULL)
		return NULL;

	central_pos = unzlocal_SearchCentralDir(  us.filestream);
	if(central_pos==0)
		err=UNZ_ERRNO;

	if(ZSEEK(
			us.filestream,
			central_pos,ZLIB_FILEFUNC_SEEK_SET)!=0)
		err=UNZ_ERRNO;

	/* the signature, already checked */
	if(unzlocal_getLong(    us.filestream,&uL					)!=UNZ_OK)	err=UNZ_ERRNO;
	/* number of this disk */
	if(unzlocal_getShort(   us.filestream,&number_disk			)!=UNZ_OK)	err=UNZ_ERRNO;
	/* number of the disk with the start of the central directory */
	if(unzlocal_getShort(   us.filestream,&number_disk_with_CD	)!=UNZ_OK)	err=UNZ_ERRNO;
	/* total number of entries in the central dir on this disk */
	if(unzlocal_getShort(   us.filestream,&us.gi.number_entry	)!=UNZ_OK)	err=UNZ_ERRNO;
	/* total number of entries in the central dir */
	if(unzlocal_getShort(   us.filestream,&number_entry_CD		)!=UNZ_OK)	err=UNZ_ERRNO;

	if((number_entry_CD!=us.gi.number_entry) ||
		(number_disk_with_CD!=0) ||
		(number_disk!=0))
		err=UNZ_BADZIPFILE;

	/* size of the central directory */
	if(unzlocal_getLong(   us.filestream,&us.size_central_dir	)!=UNZ_OK)	err=UNZ_ERRNO;
	/* offset of start of central directory with respect to the starting disk number */
	if(unzlocal_getLong(   us.filestream,&us.offset_central_dir)!=UNZ_OK)	err=UNZ_ERRNO;
	/* zipfile comment length */
	if(unzlocal_getShort(   us.filestream,&us.gi.size_comment	)!=UNZ_OK)	err=UNZ_ERRNO;

	if((central_pos<us.offset_central_dir+us.size_central_dir) &&
		(err==UNZ_OK))
		err=UNZ_BADZIPFILE;
	if(err!=UNZ_OK)
	{
		ZCLOSE(   us.filestream);
		return NULL;
	}

	us.byte_before_the_zipfile = central_pos - (us.offset_central_dir+us.size_central_dir);
	us.central_pos = central_pos;
	us.pfile_in_zip_read = NULL;
//	us.encrypted = 0;


	s=(unz_s*)ALLOC(sizeof(unz_s));
	*s=us;
	unzGoToFirstFile((unzFile)s);
	return (unzFile)s;
}


/////////////
/*
  Close the file in zip opened with unzipOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/
/*lib_main local:unzClose() local:unzOpenCurrentFile() */
static int ZEXPORT unzCloseCurrentFile(unzFile file)
{
	int err=UNZ_OK;

	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if(file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	pfile_in_zip_read_info=s->pfile_in_zip_read;

	if(pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;


	if((pfile_in_zip_read_info->rest_read_uncompressed == 0) &&
		(!pfile_in_zip_read_info->raw))
	{
		if(pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
			err=UNZ_CRCERROR;
	}


	TRYFREE(pfile_in_zip_read_info->read_buffer);
	pfile_in_zip_read_info->read_buffer = NULL;
	if(pfile_in_zip_read_info->stream_initialised)
		inflateEnd(&pfile_in_zip_read_info->stream);

	pfile_in_zip_read_info->stream_initialised = 0;
	TRYFREE(pfile_in_zip_read_info);

	s->pfile_in_zip_read=NULL;

	return err;
}
/*
  Close a ZipFile opened with unzipOpen.
  If there is files inside the .Zip opened with unzipOpenCurrentFile (see later),
	these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
  return UNZ_OK if there is no problem. */
/*lib_main.c*/
static int ZEXPORT unzClose(unzFile file)
{
	unz_s* s;
	if(file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;

	if(s->pfile_in_zip_read!=NULL)
		unzCloseCurrentFile(file);

	ZCLOSE(   s->filestream);
	TRYFREE(s);
	return UNZ_OK;
}

//////////////

/*
  Read the local header of the current zipfile
  Check the coherency of the local header and info in the end of central
		directory about this file
  store in *piSizeVar the size of extra info in local header
		(filename and size of extra field data)
*/
/*local:unzOpenCurrentFile()*/
static int unzlocal_CheckCurrentFileCoherencyHeader(
	unz_s* s,
	uInt* piSizeVar,
	uLong *poffset_local_extrafield,
	uInt  *psize_local_extrafield
){
	uLong uMagic,uData,uFlags;
	uLong size_filename;
	uLong size_extra_field;
	int err=UNZ_OK;

	*piSizeVar = 0;
	*poffset_local_extrafield = 0;
	*psize_local_extrafield = 0;

	if(ZSEEK(   s->filestream,s->cur_file_info_internal.offset_curfile +
								s->byte_before_the_zipfile,ZLIB_FILEFUNC_SEEK_SET)!=0)
		return UNZ_ERRNO;


	if(err==UNZ_OK){
		if(unzlocal_getLong(   s->filestream,&uMagic) != UNZ_OK)
			err=UNZ_ERRNO;
		else if(uMagic!=0x04034b50)
			err=UNZ_BADZIPFILE;
	}

	if(unzlocal_getShort(   s->filestream,&uData) != UNZ_OK)
		err=UNZ_ERRNO;
/*
	else if((err==UNZ_OK) && (uData!=s->cur_file_info.wVersion))
		err=UNZ_BADZIPFILE;
*/
	if(unzlocal_getShort(   s->filestream,&uFlags) != UNZ_OK)		err=UNZ_ERRNO;
	if(unzlocal_getShort(   s->filestream,&uData) != UNZ_OK)		err=UNZ_ERRNO;
	else if((err==UNZ_OK) && (uData!=s->cur_file_info.compression_method))
		err=UNZ_BADZIPFILE;

	if((err==UNZ_OK) && (s->cur_file_info.compression_method!=0) &&
						 (s->cur_file_info.compression_method!=Z_DEFLATED))
		err=UNZ_BADZIPFILE;

	if(unzlocal_getLong(   s->filestream,&uData) != UNZ_OK) /* date/time */
		err=UNZ_ERRNO;

	if(unzlocal_getLong(   s->filestream,&uData) != UNZ_OK) /* crc */
		err=UNZ_ERRNO;
	else if((err==UNZ_OK) && (uData!=s->cur_file_info.crc) &&
							  ((uFlags & 8)==0))
		err=UNZ_BADZIPFILE;

	if(unzlocal_getLong(   s->filestream,&uData) != UNZ_OK) /* size compr */
		err=UNZ_ERRNO;
	else if((err==UNZ_OK) && (uData!=s->cur_file_info.compressed_size) &&
							  ((uFlags & 8)==0))
		err=UNZ_BADZIPFILE;

	if(unzlocal_getLong(   s->filestream,&uData) != UNZ_OK) /* size uncompr */
		err=UNZ_ERRNO;
	else if((err==UNZ_OK) && (uData!=s->cur_file_info.uncompressed_size) &&
							  ((uFlags & 8)==0))
		err=UNZ_BADZIPFILE;


	if(unzlocal_getShort(   s->filestream,&size_filename) != UNZ_OK)
		err=UNZ_ERRNO;
	else if((err==UNZ_OK) && (size_filename!=s->cur_file_info.size_filename))
		err=UNZ_BADZIPFILE;

	*piSizeVar += (uInt)size_filename;

	if(unzlocal_getShort(   s->filestream,&size_extra_field) != UNZ_OK)
		err=UNZ_ERRNO;
	*poffset_local_extrafield= s->cur_file_info_internal.offset_curfile +
									SIZEZIPLOCALHEADER + size_filename;
	*psize_local_extrafield = (uInt)size_extra_field;

	*piSizeVar += (uInt)size_extra_field;

	return err;
}

/*
  Open for reading data the current file in the zipfile.
  If there is no error and the file is opened, the return value is UNZ_OK.
*/
/*lib_main.c*/
static int ZEXPORT unzOpenCurrentFile(unzFile file)
{
	int err=UNZ_OK;
	uInt iSizeVar;
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	uLong offset_local_extrafield;	/* offset of the local extra field */
	uInt  size_local_extrafield;	/* size of the local extra field */

	if(file==NULL)
		return UNZ_PARAMERROR;

	s=(unz_s*)file;
	if(!s->current_file_ok)
		return UNZ_PARAMERROR;

	if(s->pfile_in_zip_read != NULL)
		unzCloseCurrentFile(file);

	if(unzlocal_CheckCurrentFileCoherencyHeader(s,&iSizeVar,
				&offset_local_extrafield,&size_local_extrafield)!=UNZ_OK)
		return UNZ_BADZIPFILE;

	pfile_in_zip_read_info = (file_in_zip_read_info_s*)
										ALLOC(sizeof(file_in_zip_read_info_s));
	if(pfile_in_zip_read_info==NULL)
		return UNZ_INTERNALERROR;

	pfile_in_zip_read_info->read_buffer=(char*)ALLOC(UNZ_BUFSIZE);
	pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
	pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
	pfile_in_zip_read_info->pos_local_extrafield=0;
	pfile_in_zip_read_info->raw=0/*raw*/;

	if(pfile_in_zip_read_info->read_buffer==NULL)
	{
		TRYFREE(pfile_in_zip_read_info);
		return UNZ_INTERNALERROR;
	}

	pfile_in_zip_read_info->stream_initialised=0;

	if((s->cur_file_info.compression_method!=0) &&
		(s->cur_file_info.compression_method!=Z_DEFLATED))
		err=UNZ_BADZIPFILE;

	pfile_in_zip_read_info->crc32_wait=s->cur_file_info.crc;
	pfile_in_zip_read_info->crc32=0;
	pfile_in_zip_read_info->compression_method =
			s->cur_file_info.compression_method;
	pfile_in_zip_read_info->filestream=s->filestream;

	pfile_in_zip_read_info->byte_before_the_zipfile=s->byte_before_the_zipfile;

	pfile_in_zip_read_info->stream.total_out = 0;

	if((s->cur_file_info.compression_method==Z_DEFLATED) &&
		(!0/*raw*/))
	{
	  pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
	  pfile_in_zip_read_info->stream.zfree = (free_func)0;
	  pfile_in_zip_read_info->stream.opaque = (voidpf)0;
	  pfile_in_zip_read_info->stream.next_in = (voidpf)0;
	  pfile_in_zip_read_info->stream.avail_in = 0;

	  err=inflateInit2(&pfile_in_zip_read_info->stream, -MAX_WBITS);

	  if(err == Z_OK)
		pfile_in_zip_read_info->stream_initialised=1;
	  else
		return err;
		/* windowBits is passed < 0 to tell that there is no zlib header.
		 * Note that in this case inflate *requires* an extra "dummy" byte
		 * after the compressed stream in order to complete decompression and
		 * return Z_STREAM_END.
		 * In unzip, i don't wait absolutely Z_STREAM_END because I known the
		 * size of both compressed and uncompressed data
		 */
	}
	pfile_in_zip_read_info->rest_read_compressed =
			s->cur_file_info.compressed_size ;
	pfile_in_zip_read_info->rest_read_uncompressed =
			s->cur_file_info.uncompressed_size ;


	pfile_in_zip_read_info->pos_in_zipfile =
			s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
			  iSizeVar;

	pfile_in_zip_read_info->stream.avail_in = (uInt)0;

	s->pfile_in_zip_read = pfile_in_zip_read_info;


	return UNZ_OK;
}


/*
  Read bytes from the current file (opened by unzOpenCurrentFile)
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
  return <0 with error code if there is an error
    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/
/*lib_main.c*/
static int ZEXPORT unzReadCurrentFile(
	unzFile file,
	voidp buf,
	unsigned len
){
	int err=UNZ_OK;
	uInt iRead = 0;
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if(file==NULL)
		return UNZ_PARAMERROR;
	s=(unz_s*)file;
	pfile_in_zip_read_info=s->pfile_in_zip_read;

	if(pfile_in_zip_read_info==NULL)
		return UNZ_PARAMERROR;


	if((pfile_in_zip_read_info->read_buffer == NULL))
		return UNZ_END_OF_LIST_OF_FILE;
	if(len==0)
		return 0;

	pfile_in_zip_read_info->stream.next_out = (Bytef*)buf;

	pfile_in_zip_read_info->stream.avail_out = (uInt)len;

	if((len>pfile_in_zip_read_info->rest_read_uncompressed) &&
		(!(pfile_in_zip_read_info->raw)))
		pfile_in_zip_read_info->stream.avail_out =
			(uInt)pfile_in_zip_read_info->rest_read_uncompressed;

	if((len>pfile_in_zip_read_info->rest_read_compressed+
		   pfile_in_zip_read_info->stream.avail_in) &&
		 (pfile_in_zip_read_info->raw))
		pfile_in_zip_read_info->stream.avail_out =
			(uInt)pfile_in_zip_read_info->rest_read_compressed+
			pfile_in_zip_read_info->stream.avail_in;

	while (pfile_in_zip_read_info->stream.avail_out>0)
	{
		if((pfile_in_zip_read_info->stream.avail_in==0) &&
			(pfile_in_zip_read_info->rest_read_compressed>0))
		{
			uInt uReadThis = UNZ_BUFSIZE;
			if(pfile_in_zip_read_info->rest_read_compressed<uReadThis)
				uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
			if(uReadThis == 0)
				return UNZ_EOF;
			if(ZSEEK(
					  pfile_in_zip_read_info->filestream,
					  pfile_in_zip_read_info->pos_in_zipfile +
						 pfile_in_zip_read_info->byte_before_the_zipfile,
						 ZLIB_FILEFUNC_SEEK_SET)!=0)
				return UNZ_ERRNO;
			if(ZREAD(
					  pfile_in_zip_read_info->filestream,
					  pfile_in_zip_read_info->read_buffer,
					  uReadThis)!=uReadThis)
				return UNZ_ERRNO;


			pfile_in_zip_read_info->pos_in_zipfile += uReadThis;

			pfile_in_zip_read_info->rest_read_compressed-=uReadThis;

			pfile_in_zip_read_info->stream.next_in =
				(Bytef*)pfile_in_zip_read_info->read_buffer;
			pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
		}

		if((pfile_in_zip_read_info->compression_method==0) || (pfile_in_zip_read_info->raw))
		{
		uInt uDoCopy,i ;
			if((pfile_in_zip_read_info->stream.avail_in == 0) &&
				(pfile_in_zip_read_info->rest_read_compressed == 0))
				return (iRead==0) ? UNZ_EOF : iRead;

			if(pfile_in_zip_read_info->stream.avail_out <
							pfile_in_zip_read_info->stream.avail_in)
				uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
			else
				uDoCopy = pfile_in_zip_read_info->stream.avail_in ;

			for (i=0;i<uDoCopy;i++){
				*(pfile_in_zip_read_info->stream.next_out+i) =
				*(pfile_in_zip_read_info->stream.next_in +i);
			}
			pfile_in_zip_read_info->crc32 = crc32(
				pfile_in_zip_read_info->crc32,
				pfile_in_zip_read_info->stream.next_out,
				uDoCopy);
			pfile_in_zip_read_info->rest_read_uncompressed-=uDoCopy;
			pfile_in_zip_read_info->stream.avail_in  -= uDoCopy;
			pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
			pfile_in_zip_read_info->stream.next_out  += uDoCopy;
			pfile_in_zip_read_info->stream.next_in   += uDoCopy;
			pfile_in_zip_read_info->stream.total_out += uDoCopy;
			iRead += uDoCopy;
		}
		else
		{
			uLong uTotalOutBefore,uTotalOutAfter;
			const Bytef *bufBefore;
			uLong uOutThis;
			int flush=Z_SYNC_FLUSH;

			uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
			bufBefore = pfile_in_zip_read_info->stream.next_out;

			/*
			if((pfile_in_zip_read_info->rest_read_uncompressed ==
					 pfile_in_zip_read_info->stream.avail_out) &&
				(pfile_in_zip_read_info->rest_read_compressed == 0))
				flush = Z_FINISH;
			*/
			err=inflate(&pfile_in_zip_read_info->stream,flush);

//			if((err>=0) && (pfile_in_zip_read_info->stream.msg!=NULL))
//ここのチェック外しちゃ駄目かもしれん。
//			if((err>=0) )
			if((err>=0) && (pfile_in_zip_read_info->stream.err != 0))
			  err = Z_DATA_ERROR;

			uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
			uOutThis = uTotalOutAfter-uTotalOutBefore;

			pfile_in_zip_read_info->crc32 =
				crc32(pfile_in_zip_read_info->crc32,bufBefore,
						(uInt)(uOutThis));

			pfile_in_zip_read_info->rest_read_uncompressed -=
				uOutThis;

			iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

			if(err==Z_STREAM_END)
				return (iRead==0) ? UNZ_EOF : iRead;
			if(err!=Z_OK)
				break;
		}
	}

	if(err==Z_OK)
		return iRead;
	return err;
}

