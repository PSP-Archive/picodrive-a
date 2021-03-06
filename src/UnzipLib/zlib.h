/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.2.2, October 3rd, 2004

  Copyright (C) 1995-2004 Jean-loup Gailly and Mark Adler

*/

#ifndef ZLIB_H
#define ZLIB_H

#include "zconf.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef voidpf (*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidpf opaque, voidpf address));

struct internal_state;

typedef struct z_stream_s {
	Bytef	 *next_in;	/* next input byte */
	Bytef	 *next_out; /* next output byte should be put there */
	uInt	 avail_in;	/* number of bytes available at next_in */
	uInt	 avail_out; /* remaining free space at next_out */
	uLong	 total_in;	/* total nb of input bytes read so far */
	uLong	 total_out; /* total nb of bytes output so far */

//	char	 *msg;		/* last error message, NULL if no error */
//	int		*msg;//"infback.c","inffast.c","inflate.c"?̂݁B
	int		err; //no_err=0; err=1;

	struct internal_state FAR *state; /* not visible by applications */

	alloc_func zalloc;	/* used to allocate the internal state */
	free_func  zfree;	/* used to free the internal state */
	voidpf	   opaque;	/* private data object passed to zalloc and zfree */

	int 	data_type;	/* best guess about the data type: ascii or binary */
	uLong	adler;		/* adler32 value of the uncompressed data */
	uLong	reserved;	/* reserved for future use */
} z_stream;

typedef z_stream FAR *z_streamp;

/* constants */

#define Z_NO_FLUSH		0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_SYNC_FLUSH	2
#define Z_FULL_FLUSH	3
#define Z_FINISH		4
#define Z_BLOCK 		5
/* Allowed flush values; see deflate() and inflate() below for details */

#define Z_OK			0
#define Z_STREAM_END	1
#define Z_NEED_DICT 	2
#define Z_ERRNO 	   (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_NO_COMPRESSION		 0
#define Z_BEST_SPEED			 1
#define Z_BEST_COMPRESSION		 9
#define Z_DEFAULT_COMPRESSION  (-1)
/* compression levels */

#define Z_FILTERED			  1
#define Z_HUFFMAN_ONLY		  2
#define Z_RLE				  3
#define Z_DEFAULT_STRATEGY	  0
/* compression strategy; see deflateInit2() below for details */

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
/* Possible values of the data_type field (though see inflate()) */

#define Z_DEFLATED	 8
/* The deflate compression method (the only one supported in this version) */

#define Z_NULL	0  /* for initializing zalloc, zfree, opaque */

//#define zlib_version zlibVersion()
/* for compatibility with versions < 1.0.2 */

						/* basic functions */


ZEXTERN int ZEXPORT deflate OF((z_streamp strm, int flush));

ZEXTERN int ZEXPORT deflateEnd OF((z_streamp strm));
/*
	 All dynamically allocated data structures for this stream are freed.
   This function discards any unprocessed input and does not flush any
   pending output.

	 deflateEnd returns Z_OK if success, Z_STREAM_ERROR if the
   stream state was inconsistent, Z_DATA_ERROR if the stream was freed
   prematurely (some input or output was discarded). In the error case,
   msg may be set but then points to a static string (which must not be
   deallocated).
*/




ZEXTERN int ZEXPORT inflate OF((z_streamp strm, int flush));

ZEXTERN int ZEXPORT inflateEnd OF((z_streamp strm));

ZEXTERN int ZEXPORT deflateSetDictionary OF((z_streamp strm,
											 const Bytef *dictionary,
											 uInt  dictLength));

ZEXTERN int ZEXPORT deflateCopy OF((z_streamp dest,
									z_streamp source));

ZEXTERN int ZEXPORT deflateReset OF((z_streamp strm));

ZEXTERN int ZEXPORT deflateParams OF((z_streamp strm,
									  int level,
									  int strategy));

ZEXTERN uLong ZEXPORT deflateBound OF((z_streamp strm,
									   uLong sourceLen));

ZEXTERN int ZEXPORT deflatePrime OF((z_streamp strm,
									 int bits,
									 int value));

ZEXTERN int ZEXPORT inflateSetDictionary OF((z_streamp strm,
											 const Bytef *dictionary,
											 uInt  dictLength));

ZEXTERN int ZEXPORT inflateSync OF((z_streamp strm));

ZEXTERN int ZEXPORT inflateCopy OF((z_streamp dest,
									z_streamp source));

ZEXTERN int ZEXPORT inflateReset OF((z_streamp strm));


typedef unsigned (*in_func) OF((void FAR *, unsigned char FAR * FAR *));
typedef int (*out_func) OF((void FAR *, unsigned char FAR *, unsigned));

ZEXTERN int ZEXPORT inflateBack OF((z_stream FAR *strm,
									in_func in, void FAR *in_desc,
									out_func out, void FAR *out_desc));

ZEXTERN int ZEXPORT inflateBackEnd OF((z_stream FAR *strm));

ZEXTERN uLong ZEXPORT zlibCompileFlags OF((void));

ZEXTERN int ZEXPORT compress OF((Bytef *dest,	uLongf *destLen,
								 const Bytef *source, uLong sourceLen));

ZEXTERN int ZEXPORT compress2 OF((Bytef *dest,	 uLongf *destLen,
								  const Bytef *source, uLong sourceLen,
								  int level));

ZEXTERN uLong ZEXPORT compressBound OF((uLong sourceLen));

ZEXTERN int ZEXPORT uncompress OF((Bytef *dest,   uLongf *destLen,
								   const Bytef *source, uLong sourceLen));


typedef voidp gzFile;

ZEXTERN gzFile ZEXPORT gzopen  OF((const char *path, const char *mode));

ZEXTERN gzFile ZEXPORT gzdopen	OF((int fd, const char *mode));

ZEXTERN int ZEXPORT gzsetparams OF((gzFile file, int level, int strategy));

ZEXTERN int ZEXPORT    gzread  OF((gzFile file, voidp buf, unsigned len));

ZEXTERN int ZEXPORT    gzwrite OF((gzFile file,
								   voidpc buf, unsigned len));

ZEXTERN int ZEXPORTVA	gzprintf OF((gzFile file, const char *format, ...));

ZEXTERN int ZEXPORT gzputs OF((gzFile file, const char *s));

ZEXTERN char * ZEXPORT gzgets OF((gzFile file, char *buf, int len));

ZEXTERN int ZEXPORT    gzputc OF((gzFile file, int c));

ZEXTERN int ZEXPORT    gzgetc OF((gzFile file));

ZEXTERN int ZEXPORT    gzungetc OF((int c, gzFile file));

ZEXTERN int ZEXPORT    gzflush OF((gzFile file, int flush));

ZEXTERN z_off_t ZEXPORT    gzseek OF((gzFile file,
									  z_off_t offset, int whence));

ZEXTERN int ZEXPORT    gzrewind OF((gzFile file));

ZEXTERN z_off_t ZEXPORT    gztell OF((gzFile file));

ZEXTERN int ZEXPORT gzeof OF((gzFile file));

ZEXTERN int ZEXPORT    gzclose OF((gzFile file));

ZEXTERN const char * ZEXPORT gzerror OF((gzFile file, int *errnum));

ZEXTERN void ZEXPORT gzclearerr OF((gzFile file));

ZEXTERN uLong ZEXPORT adler32 OF((uLong adler, const Bytef *buf, uInt len));


ZEXTERN uLong ZEXPORT crc32   OF((uLong crc, const Bytef *buf, uInt len));
/*
	 Update a running crc with the bytes buf[0..len-1] and return the updated
   crc. If buf is NULL, this function returns the required initial value
   for the crc. Pre- and post-conditioning (one's complement) is performed
   within this function so it shouldn't be done by the application.
   Usage example:

	 uLong crc = crc32(0L, Z_NULL, 0);

	 while (read_buffer(buffer, length) != EOF) {
	   crc = crc32(crc, buffer, length);
	 }
	 if (crc != original_crc) error();
*/


						/* various hacks, don't look :) */

/* deflateInit and inflateInit are macros to allow checking the zlib version
 * and the compiler's view of z_stream:
 */
ZEXTERN int ZEXPORT deflateInit_ OF((z_streamp strm,
 int level, /*const char *version,*/ int stream_size));
ZEXTERN int ZEXPORT inflateInit_ OF((z_streamp strm,
 /*const char *version,*/ int stream_size));
ZEXTERN int ZEXPORT deflateInit2_ OF((z_streamp strm,
 int level, int  method, int windowBits, int memLevel,
 int strategy, /*const char *version,*/ int stream_size));
ZEXTERN int ZEXPORT inflateInit2_ OF((z_streamp strm,
 int windowBits, /*const char *version,*/ int stream_size));
ZEXTERN int ZEXPORT inflateBackInit_ OF((z_stream FAR *strm,
 int windowBits, unsigned char FAR *window, /*const char *version,*/ int stream_size));
#define deflateInit(strm, level) \
		deflateInit_((strm), (level),		/*ZLIB_VERSION,*/ sizeof(z_stream))
#define inflateInit(strm) \
		inflateInit_((strm),				/*ZLIB_VERSION,*/ sizeof(z_stream))
#define deflateInit2(strm, level, method, windowBits, memLevel, strategy) \
		deflateInit2_((strm),(level),(method),(windowBits),(memLevel),\
					  (strategy),			/*ZLIB_VERSION,*/ sizeof(z_stream))
#define inflateInit2(strm, windowBits) \
		inflateInit2_((strm), (windowBits), /*ZLIB_VERSION,*/ sizeof(z_stream))
#define inflateBackInit(strm, windowBits, window) \
		inflateBackInit_((strm), (windowBits), (window), \
		/*ZLIB_VERSION,*/ sizeof(z_stream))


#if !defined(ZUTIL_H) && !defined(NO_DUMMY_DECL)
	struct internal_state {int dummy;}; /* hack for buggy compilers */
#endif

ZEXTERN const char	 * ZEXPORT zError			OF((int));
ZEXTERN int 		   ZEXPORT inflateSyncPoint OF((z_streamp z));
ZEXTERN const uLongf * ZEXPORT get_crc_table	OF((void));

#ifdef __cplusplus
}
#endif

#endif /* ZLIB_H */
