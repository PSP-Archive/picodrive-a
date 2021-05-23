
// memo:%div/

//#include "font3333.c"
extern const unsigned short zenkaku_font10[];

#include "main_text.h"
#include "_pg.h"


#define HANKAKU10_OFFS 0x20

#define bc(aa,bb,cc,dd,ee,ff,gg,hh) (\
(aa?0x01:0x00)+\
(bb?0x02:0x00)+\
(cc?0x04:0x00)+\
(dd?0x08:0x00)+\
(ee?0x10:0x00)+\
(ff?0x20:0x00)+\
(gg?0x40:0x00)+\
(hh?0x80:0x00)\
)

const static unsigned char hankaku_font5x10[]={ /* x:y == 5x10 */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/*0x20 ' ' */
	0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x00,0x00,	/*0x21 '!' */
	0x00,0x0a,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/*0x22 '"' */
	0x00,0x0a,0x0a,0x1f,0x0a,0x1f,0x0a,0x0a,0x00,0x00,	/*0x23 '#' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,1,1,1,0,0),
bc(0,1,0,1,0,0,0,0),
bc(0,0,1,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,1,0,0,0),
bc(0,0,0,1,0,1,0,0),
bc(0,0,0,1,0,1,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x04,0x1e,0x05,0x0e,0x14,0x0f,0x04,0x00,0x00,	/*0x24 '$' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,0,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x00,0x16,0x16,0x08,0x04,0x1a,0x1a,0x00,0x00,	/*0x25 '%' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,1,0,1,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,0,1,1,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x04,0x0a,0x02,0x14,0x12,0x0a,0x14,0x00,0x00,	/*0x26 '&' */
	0x00,0x04,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,	/*0x27 ''' */

bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,1,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x10,0x08,0x08,0x04,0x04,0x04,0x08,0x08,0x10,0x00,	/*0x28 '(' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x02,0x04,0x04,0x08,0x08,0x08,0x04,0x04,0x02,0x00,	/*0x29 ')' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x00,0x0a,0x04,0x1f,0x04,0x0a,0x00,0x00,0x00,	/*0x2a '*' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x00,0x04,0x04,0x1f,0x04,0x04,0x00,0x00,0x00,	/*0x2b '+' */
	0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x02,0x00,	/*0x2c ',' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x00,0x00,	/*0x2d '-' */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,	/*0x2e '.' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,1,0,0),
bc(0,0,0,0,0,1,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x10,0x10,0x08,0x08,0x04,0x04,0x02,0x02,0x00,	/*0x2f '/' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,0,1,1,1,0,0,0),//00001100
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00011010
bc(0,1,0,0,0,1,0,0),//00010110
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00001100
bc(0,0,1,1,1,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x0c,0x12,0x1a,0x16,0x12,0x12,0x0c,0x00,0x00,	/*0x30 '0' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,0,0,1,0,0,0,0),//00000100
bc(0,0,1,1,0,0,0,0),//00000110
bc(0,0,0,1,0,0,0,0),//00000100
bc(0,0,0,1,0,0,0,0),//00000100
bc(0,0,0,1,0,0,0,0),//00000100
bc(0,0,0,1,0,0,0,0),//00000100
bc(0,0,0,1,0,0,0,0),//00001110
bc(0,0,0,1,0,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x04,0x06,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,	/*0x31 '1' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,1,1,1,1,0,0,0),//00001100
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00011010
bc(0,0,0,0,1,0,0,0),//00010110
bc(0,0,0,1,0,0,0,0),//00010010
bc(0,0,1,0,0,0,0,0),//00010010
bc(0,1,0,0,0,0,0,0),//00001100
bc(0,1,1,1,1,1,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x0c,0x12,0x10,0x08,0x04,0x02,0x1e,0x00,0x00,	/*0x32 '2' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,1,1,1,1,0,0,0),//00001100
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00011010
bc(0,0,0,1,1,0,0,0),//00010110
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00001100
bc(0,1,1,1,1,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x0c,0x12,0x10,0x08,0x10,0x12,0x0c,0x00,0x00,	/*0x33 '3' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,0,0,0,1,0,0,0),//00001100
bc(0,0,0,1,1,0,0,0),//00010010
bc(0,0,1,0,1,0,0,0),//00011010
bc(0,0,1,0,1,0,0,0),//00010110
bc(0,1,0,0,1,0,0,0),//00010010
bc(0,1,1,1,1,1,0,0),//00010010
bc(0,0,0,0,1,0,0,0),//00001100
bc(0,0,0,0,1,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x08,0x0c,0x0a,0x0a,0x1e,0x08,0x08,0x00,0x00,	/*0x34 '4' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,1,1,1,1,1,0,0),//00001100
bc(0,1,0,0,0,0,0,0),//00010010
bc(0,1,0,0,0,0,0,0),//00011010
bc(0,1,1,1,1,0,0,0),//00010110
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00001100
bc(0,1,1,1,1,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x1e,0x02,0x0e,0x10,0x10,0x12,0x0c,0x00,0x00,	/*0x35 '5' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,0,1,1,1,1,0,0),//00001100
bc(0,1,0,0,0,0,0,0),//00010010
bc(0,1,0,0,0,0,0,0),//00011010
bc(0,1,1,1,1,0,0,0),//00010110
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00001100
bc(0,0,1,1,1,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x0c,0x12,0x02,0x0e,0x12,0x12,0x0c,0x00,0x00,	/*0x36 '6' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,1,1,1,1,1,0,0),//00001100
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,1,0,0,0),//00011010
bc(0,0,0,0,1,0,0,0),//00010110
bc(0,0,0,1,0,0,0,0),//00010010
bc(0,0,0,1,0,0,0,0),//00010010
bc(0,0,0,1,0,0,0,0),//00001100
bc(0,0,0,1,0,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x1e,0x12,0x08,0x08,0x04,0x04,0x04,0x00,0x00,	/*0x37 '7' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,0,1,1,1,0,0,0),//00001100
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00011010
bc(0,0,1,1,1,0,0,0),//00010110
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00001100
bc(0,0,1,1,1,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x0c,0x12,0x12,0x0c,0x12,0x12,0x0c,0x00,0x00,	/*0x38 '8' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,0,1,1,1,0,0,0),//00001100
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00011010
bc(0,0,1,1,1,1,0,0),//00010110
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00010010
bc(0,0,0,0,0,1,0,0),//00001100
bc(0,1,1,1,1,0,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x0c,0x12,0x12,0x1c,0x10,0x12,0x0c,0x00,0x00,	/*0x39 '9' */

	0x00,0x00,0x0c,0x0c,0x00,0x00,0x0c,0x0c,0x00,0x00,	/*0x3a ':' */
	0x00,0x00,0x0c,0x0c,0x00,0x00,0x0c,0x08,0x04,0x00,	/*0x3b ';' */
	0x00,0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x00,0x00,	/*0x3c '<' */
	0x00,0x00,0x00,0x1e,0x00,0x1e,0x00,0x00,0x00,0x00,	/*0x3d '=' */
	0x00,0x02,0x04,0x08,0x10,0x08,0x04,0x02,0x00,0x00,	/*0x3e '>' */
	0x00,0x0c,0x12,0x10,0x08,0x08,0x00,0x08,0x00,0x00,	/*0x3f '?' */

	0x00,0x0c,0x12,0x1a,0x16,0x0a,0x02,0x0c,0x00,0x00,	/*0x40 '@' */

bc(0,0,0,0,0,0,0,0),//00000000
bc(0,0,0,1,0,0,0,0),//00001100
bc(0,0,0,1,0,0,0,0),//00010010
bc(0,0,1,0,1,0,0,0),//00010010
bc(0,0,1,0,1,0,0,0),//00011110
bc(0,0,1,1,1,0,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00010010
bc(0,1,0,0,0,1,0,0),//00000000
bc(0,0,0,0,0,0,0,0),//00000000
//	0x00,0x0c,0x12,0x12,0x1e,0x12,0x12,0x12,0x00,0x00,	/*0x41 'A' */

bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,0,0,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0e,0x12,0x12,0x0e,0x12,0x12,0x0e,0x00,0x00,	/*0x42 'B' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,1,1,1,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,1,1,1,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0c,0x12,0x02,0x02,0x02,0x12,0x0c,0x00,0x00,	/*0x43 'C' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0e,0x12,0x12,0x12,0x12,0x12,0x0e,0x00,0x00,	/*0x44 'D' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x1e,0x02,0x02,0x0e,0x02,0x02,0x1e,0x00,0x00,	/*0x45 'E' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x1e,0x02,0x02,0x0e,0x02,0x02,0x02,0x00,0x00,	/*0x46 'F' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,1,1,1,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,1,1,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,1,1,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0c,0x12,0x02,0x1a,0x12,0x12,0x1c,0x00,0x00,	/*0x47 'G' */

bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x12,0x12,0x1e,0x12,0x12,0x12,0x00,0x00,	/*0x48 'H' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,1,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,1,1,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0e,0x04,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,	/*0x49 'I' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,1,1,1,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,1,1,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x18,0x10,0x10,0x10,0x12,0x12,0x0c,0x00,0x00,	/*0x4a 'J' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,1,0,1,0,0,0,0),
bc(0,1,1,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,1,0,0,0,0,0),
bc(0,1,0,1,0,0,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x12,0x0a,0x06,0x0a,0x12,0x12,0x00,0x00,	/*0x4b 'K' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x1e,0x00,0x00,	/*0x4c 'L' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,1,0,1,1,0,0),
bc(0,1,1,0,1,1,0,0),
bc(0,1,1,0,1,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x1e,0x1e,0x12,0x12,0x12,0x12,0x00,0x00,	/*0x4d 'M' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,1,0,0,1,0,0),
bc(0,1,1,0,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,0,1,1,0,0),
bc(0,1,0,0,1,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x12,0x16,0x1a,0x12,0x12,0x12,0x00,0x00,	/*0x4e 'N' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,1,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,1,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0c,0x12,0x12,0x12,0x12,0x12,0x0c,0x00,0x00,	/*0x4f 'O' */

bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0e,0x12,0x12,0x0e,0x02,0x02,0x02,0x00,0x00,	/*0x50 'P' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,1,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,0,1,1,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0c,0x12,0x12,0x12,0x16,0x1a,0x0c,0x10,0x00,	/*0x51 'Q' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,1,0,1,0,0,0,0),
bc(0,1,0,0,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0e,0x12,0x12,0x0e,0x06,0x0a,0x12,0x00,0x00,	/*0x52 'R' */
bc(0,0,0,0,0,0,0,0),
bc(0,0,1,1,1,1,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,0,0,1,0,0),
bc(0,0,0,0,0,1,0,0),
bc(0,1,1,1,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x0c,0x12,0x02,0x0c,0x10,0x12,0x0c,0x00,0x00,	/*0x53 'S' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x1f,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,	/*0x54 'T' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,1,1,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x0c,0x00,0x00,	/*0x55 'U' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x12,0x12,0x12,0x1e,0x0c,0x0c,0x00,0x00,	/*0x56 'V' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,0,1,0,1,0,0),
bc(0,1,1,0,1,1,0,0),
bc(0,1,1,0,1,1,0,0),
bc(0,1,1,0,1,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x12,0x12,0x12,0x1e,0x1e,0x12,0x00,0x00,	/*0x57 'W' */

bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x12,0x12,0x12,0x0c,0x12,0x12,0x12,0x00,0x00,	/*0x58 'X' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x11,0x11,0x0a,0x04,0x04,0x04,0x04,0x00,0x00,	/*0x59 'Y' */
bc(0,0,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,0,0,0,1,0,0),
bc(0,0,0,0,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,1,0,0,0,0,0,0),
bc(0,1,1,1,1,1,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x1e,0x10,0x08,0x04,0x04,0x02,0x1e,0x00,0x00,	/*0x5a 'Z' */
	0x1c,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x1c,0x00,	/*0x5b '[' */
	0x00,0x11,0x0a,0x04,0x1f,0x04,0x1f,0x04,0x00,0x00,	/*0x5c '/'(yen) */
	0x0e,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x0e,0x00,	/*0x5d ']' */
	0x04,0x0a,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/*0x5e '^' */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,	/*0x5f '_' */

	0x00,0x04,0x04,0x08,0x00,0x00,0x00,0x00,0x00,0x00,	/*0x60 '`' */
	0x00,0x00,0x00,0x16,0x19,0x11,0x11,0x19,0x16,0x00,	/*0x61 'a' */
	0x00,0x01,0x01,0x0f,0x11,0x11,0x11,0x11,0x0f,0x00,	/*0x62 'b' */
	0x00,0x00,0x00,0x1e,0x01,0x01,0x01,0x01,0x1e,0x00,	/*0x63 'c' */
	0x00,0x10,0x10,0x1e,0x11,0x11,0x11,0x11,0x1e,0x00,	/*0x64 'd' */
	0x00,0x00,0x00,0x0e,0x11,0x1f,0x01,0x01,0x1e,0x00,	/*0x65 'e' */
	0x00,0x18,0x04,0x1e,0x04,0x04,0x04,0x04,0x04,0x00,	/*0x66 'f' */
	0x00,0x00,0x00,0x1e,0x11,0x11,0x1e,0x10,0x08,0x06,	/*0x67 'g' */

	0x00,0x01,0x01,0x0f,0x11,0x11,0x11,0x11,0x11,0x00,	/*0x68 'h' */
	0x00,0x04,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00,	/*0x69 'i' */
	0x00,0x10,0x00,0x10,0x10,0x10,0x10,0x10,0x08,0x04,	/*0x6a 'j' */
	0x00,0x01,0x01,0x11,0x09,0x05,0x07,0x09,0x11,0x00,	/*0x6b 'k' */
	0x00,0x06,0x04,0x04,0x04,0x04,0x04,0x04,0x0e,0x00,	/*0x6c 'l' */
	0x00,0x00,0x00,0x0b,0x15,0x15,0x15,0x15,0x15,0x00,	/*0x6d 'm' */
	0x00,0x00,0x00,0x0f,0x11,0x11,0x11,0x11,0x11,0x00,	/*0x6e 'n' */
	0x00,0x00,0x00,0x0e,0x11,0x11,0x11,0x11,0x0e,0x00,	/*0x6f 'o' */

	0x00,0x00,0x00,0x0f,0x11,0x11,0x0f,0x01,0x01,0x01,	/*0x70 'p' */
	0x00,0x00,0x00,0x1e,0x11,0x11,0x1e,0x10,0x10,0x10,	/*0x71 'q' */
	0x00,0x00,0x00,0x1a,0x06,0x02,0x02,0x02,0x02,0x00,	/*0x72 'r' */
	0x00,0x00,0x00,0x1c,0x02,0x0c,0x10,0x10,0x0e,0x00,	/*0x73 's' */
	0x00,0x04,0x04,0x1e,0x04,0x04,0x04,0x04,0x18,0x00,	/*0x74 't' */
	0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x11,0x1e,0x00,	/*0x75 'u' */
	0x00,0x00,0x00,0x11,0x11,0x11,0x0a,0x0a,0x04,0x00,	/*0x76 'v' */
	0x00,0x00,0x00,0x11,0x15,0x15,0x15,0x15,0x0a,0x00,	/*0x77 'w' */

	0x00,0x00,0x00,0x11,0x0a,0x04,0x0a,0x0a,0x11,0x00,	/*0x78 'x' */

bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
bc(0,1,0,0,0,1,0,0),
bc(0,0,1,0,1,0,0,0),
bc(0,0,0,1,1,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,0,1,0,0,0,0),
bc(0,0,1,0,0,0,0,0),
bc(0,0,0,0,0,0,0,0),
//	0x00,0x00,0x00,0x11,0x14,0x18,0x10,0x08,0x08,0x04,	/*0x79 'y' */
	0x00,0x00,0x00,0x1e,0x10,0x08,0x04,0x02,0x1e,0x00,	/*0x7a 'z' */
	0x18,0x04,0x04,0x04,0x02,0x04,0x04,0x04,0x04,0x18,	/*0x7b '{' */
	0x04,0x04,0x04,0x04,0x00,0x04,0x04,0x04,0x04,0x04,	/*0x7c '|' */
	0x06,0x08,0x08,0x08,0x10,0x08,0x08,0x08,0x08,0x06,	/*0x7d '}' */
	0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/*0x7e '~' */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/*0x7f ' ' */

};
#undef bc



//unsigned short rgb2col(unsigned char r,unsigned char g,unsigned char b)
//{
//	return ((((b>>3) & 0x1F)<<10)+(((g>>3) & 0x1F)<<5)+(((r>>3) & 0x1F)<<0)+0x8000);
//}

// ---------------------------------------------------------------------------------




static void ShowFps2(int mode)
{
	unsigned short *vr=0;
	unsigned char  *fnt1;
	unsigned char  *fnt2;
	unsigned char  pt;
	unsigned char ch2;
	unsigned char val;
	int y1;
	int k;

//	const int show_x[] = { 0, 0, 480-5*5,     0, 480-5*5 };
//	const int show_y[] = { 0, 0,       0, 272-9, 272-9   };

//switch( 1 ){
//case 0:	vr=( (unsigned short *)PSP_Screen_GetBackBuffer_f(  0,   0 ) );	break;
//case 1:	vr=( (unsigned short *)PSP_Screen_GetBackBuffer_f(  0,   0 ) );	break;
//case 2:	vr=( (unsigned short *)PSP_Screen_GetBackBuffer_f(464,   0 ) );	break;
//case 3:	vr=( (unsigned short *)PSP_Screen_GetBackBuffer_f(  0, 262 ) );	break;
//case 4:	vr=( (unsigned short *)PSP_Screen_GetBackBuffer_f(464, 262 ) );	break;
//}
	vr=( (unsigned short *) 0x04000000 );

	k=0;	/* auto delayed shadow */
	val=fps_last;
//// input value of decimal, cliped output 00 to 99.
	if(val>99) val=99;	/*MAX*/
	ch2='0'-HANKAKU10_OFFS; while(val>9){val-=10;	ch2++;};
	fnt1 = (unsigned char *)&hankaku_font5x10[ch2*10+1];

	ch2='0'-HANKAKU10_OFFS; while(val>0){val--;		ch2++;};
	fnt2 = (unsigned char *)&hankaku_font5x10[ch2*10+1];

	// draw
//	vr = (unsigned short *)PSP_Screen_GetBackBuffer_f(x,y);
	vr+=14;	/* 10==5+5 */
//	vr_temp=vr;
	if(mode){ // bg black mode
		for(y1=0;y1<(10-2);y1++)
		{
			vr += LINESIZE-14;	/* 10==5+5 */
			pt = *fnt1++;
			     {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x02){	*vr = 0xffff;			}
			else  {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x04){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x08){	*vr = 0xffff;			}
			else  {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x10){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x20){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			     {		*vr = (unsigned short)0;	}	vr++;
			//vr += PSP_LINESIZE-5;
			pt = *fnt2++;
			     {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x02){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x04){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x08){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x10){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			if(pt & 0x20){	*vr = 0xffff;			}
			else {		*vr = (unsigned short)0;	}	vr++;
			     {		*vr = (unsigned short)0;	}	vr++;
		}
	}else{ // bg clear mode
		for(y1=0;y1<(10-2);y1++)
		{
			vr += LINESIZE-14;	/* 10==5+5 */
			pt = *fnt1++;
			vr++;
			if(pt & 0x02){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x04){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x08){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x10){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x20){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			     if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			//vr += PSP_LINESIZE-5;
			pt = *fnt2++;
			vr++;
			if(pt & 0x02){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x04){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x08){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x10){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			if(pt & 0x20){	*vr = 0xffff;				k=1;	}
			else if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
			     if(k){		*vr = (unsigned short)0;	k=0;	}	vr++;
		}
	}
}

static void Draw_Char_Hankaku(int x,int y,unsigned char ch,int col,int backcol,int fill) {
	unsigned short *vr;
	unsigned char  *fnt;
	unsigned char  pt;
	int y1;

	ch = ((ch-HANKAKU10_OFFS)&0x7f);	/* ascii(0x00-0x7f) */

	// mapping
//	if (ch<0x20) {		ch  = 0;	}
//	else if (ch<0x80) {	ch -= 0x20;	}
//	else if (ch<0xa0) {	ch  = 0;	}
//	else {				ch -= 0x40;	}

	fnt = (unsigned char *)&hankaku_font5x10[ch*10];

	// draw
	vr = (unsigned short *)pgGetVramAddr(x,y);
	for(y1=0;y1<10;y1++) {
		pt = *fnt++;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		vr += LINESIZE-7;
	}
}

inline unsigned short num2elisa(unsigned short c) {
		 if (c >=4418) {	return c + (0xda1 - 4418);	}
	else if (c >=1410) {	return c + (0x20c - 1410);	}
	else if (c >= 690) {	return 0x6b;	}
	else if (c >= 658) {	return c + (0x1ec - 658);	}
	else if (c >= 612) {	return c + (0x1cb - 612);	}
	else if (c >= 564) {	return c + (0x1aa - 564);	}
	else if (c >= 502) {	return c + (0x192 - 502);	}
	else if (c >= 470) {	return c + (0x17a - 470);	}
	else if (c >= 376) {	return c + (0x124 - 376);	}
	else if (c >= 282) {	return c + (0xd1 - 282);	}
	else if (c >= 252) {	return c + (0xb7 - 252);	}
	else if (c >= 220) {	return c + (0x9d - 220);	}
	else if (c >= 203) {	return c + (0x93 - 203);	}
	else if (c >= 187) {	return 0x92;	}
	else if (c >= 175) {	return c + (0x8a - 203);	}
	else if (c >= 153) {	return c + (0x7b - 153);	}
	else if (c >= 135) {	return c + (0x74 - 135);	}
	else if (c >= 119) {	return c + (0x6c - 119);	}
	else 			   {	return c;	}
}
static void Draw_Char_Zenkaku(int x,int y,unsigned char u,unsigned char d,int col,int backcol,int fill) {
	unsigned short *vr;
	unsigned short *fnt;
	unsigned short pt;
	int y1;

	// mapping
	if (d > 0x7F) d--;
	if (u > 0x9F) u-=0x40;
	d -= 0x40; u -= 0x81;
	fnt = (unsigned short *)&zenkaku_font10[num2elisa(u*0xbc+d)*10];

	// draw
	vr = (unsigned short *)pgGetVramAddr(x,y);
	for(y1=0;y1<10;y1++) {
		pt = *fnt++;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		if (pt & 1) { *vr = col; } else { if (fill) *vr = backcol; } vr++; pt >>= 1;
		vr += LINESIZE-10;
	}
}

static void text_print(int x,int y,/*unsigned*/ char *str,int col,int backcol,int fill) {
	unsigned char ch = 0,bef = 0;

	while(*str != 0) {
		ch = *str++;
		if (bef!=0) {
			Draw_Char_Zenkaku(x,y,bef,ch,col,backcol,fill);
			x+=10;
			bef=0;
		} else {
			if (((ch>=0x80) && (ch<0xa0)) || (ch>=0xe0)) {
				bef = ch;
			} else {
				Draw_Char_Hankaku(x,y,ch,col,backcol,fill);
				x+=(5+2);
			}
		}
	}
}

// ---------------------------------------------------------------------------------
#if 1
static void	text_counter(int x,int y,unsigned int c,int k,int col)
{
	char m[2];
	m[1]=0;
	do{
		m[0]='0'+(c%10);
		text_print(x-=(5+2),y,m,col,0,1);
		c/=10;
		if(!(--k))c=0;
	}while(c);
	while(k--){
		text_print(x-=(5+2),y," ",col,0,1);
	}
}
#endif
#if 00
void	text_counterh(int x,int y,unsigned int c,int k,int col)
{
	char m[2];
	static char m2[]="0123456789ABCDEF";
	m[1]=0;
	while(k--){
		m[0]=m2[(c&15)];
		text_print(x-=(5+2),y,m,col,0,1);
		c/=16;
	}
	while(k--){
		text_print(x-=(5+2),y," ",col,0,1);
	}
}
#endif
