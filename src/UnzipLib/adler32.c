/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-2003 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#define ZLIB_INTERNAL
#include "zlib.h"

#define BASE 65521UL	/* largest prime smaller than 65536 */
#define N5552MAX 5552
/* N5552MAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */


#ifdef NO_DIVIDE
	#define MODED(a) \
	do { \
		if (a >= (BASE << 16)) a -= (BASE << 16); \
		if (a >= (BASE << 15)) a -= (BASE << 15); \
		if (a >= (BASE << 14)) a -= (BASE << 14); \
		if (a >= (BASE << 13)) a -= (BASE << 13); \
		if (a >= (BASE << 12)) a -= (BASE << 12); \
		if (a >= (BASE << 11)) a -= (BASE << 11); \
		if (a >= (BASE << 10)) a -= (BASE << 10); \
		if (a >= (BASE <<  9)) a -= (BASE <<  9); \
		if (a >= (BASE <<  8)) a -= (BASE <<  8); \
		if (a >= (BASE <<  7)) a -= (BASE <<  7); \
		if (a >= (BASE <<  6)) a -= (BASE <<  6); \
		if (a >= (BASE <<  5)) a -= (BASE <<  5); \
		if (a >= (BASE <<  4)) a -= (BASE <<  4); \
		if (a >= (BASE <<  3)) a -= (BASE <<  3); \
		if (a >= (BASE <<  2)) a -= (BASE <<  2); \
		if (a >= (BASE <<  1)) a -= (BASE <<  1); \
		if (a >= BASE) a -= BASE; \
	} while (0)
#else
	#define MODED(a) a %= BASE
#endif

/* ========================================================================= */
uLong ZEXPORT adler32(adler, buf, len)
	uLong adler;
	const Bytef *buf;
	uInt len;
{
	unsigned long s1 = adler & 0xffff;
	unsigned long s2 = (adler >> 16) & 0xffff;
	int k;

	if (buf == Z_NULL) return 1L;

	while (len > 0) {
		k = len < N5552MAX ? (int)len : N5552MAX;
		len -= k;
		while (k >= 16) {
			s1 += buf[ 0]; s2 += s1;
			s1 += buf[ 1]; s2 += s1;
			s1 += buf[ 2]; s2 += s1;
			s1 += buf[ 3]; s2 += s1;
			s1 += buf[ 4]; s2 += s1;
			s1 += buf[ 5]; s2 += s1;
			s1 += buf[ 6]; s2 += s1;
			s1 += buf[ 7]; s2 += s1;
			s1 += buf[ 8]; s2 += s1;
			s1 += buf[ 9]; s2 += s1;
			s1 += buf[10]; s2 += s1;
			s1 += buf[11]; s2 += s1;
			s1 += buf[12]; s2 += s1;
			s1 += buf[13]; s2 += s1;
			s1 += buf[14]; s2 += s1;
			s1 += buf[15]; s2 += s1;
			buf += 16;
			k -= 16;
		}
		if (k != 0) do {
			s1 += *buf++;
			s2 += s1;
		} while (--k);
		MODED(s1);
		MODED(s2);
	}
	return (s2 << 16) | s1;
}
