#ifndef ___ZLIBFILEMEMORY
#define ___ZLIBFILEMEMORY

#include "zlibType.h"
#include "zlibFileMemory.h"

extern void* ___memcpy(void *buf1, const void *buf2, int n);

extern void ___free( void *memblock );
extern void *___calloc( size_t num, size_t size );
extern void *___malloc( size_t size );

#if 00
extern FILE *___fopen( const char *filename, const char *mode );
extern size_t ___fwrite( const void*buffer, size_t size, size_t count, FILE *stream );
extern long ___ftell( FILE *stream );
extern int ___fclose( FILE *stream );
extern int ___ferror( FILE *stream );
extern size_t ___fread( void *buffer, size_t size, size_t count, FILE *stream );
extern int ___fseek( FILE *stream, long offset, int origin );
#endif


#endif //___ZLIBFILEMEMORY

