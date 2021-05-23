
#ifndef _MY_PG_H_
#define _MY_PG_H_

// primitive graphics for Hello World PSP


////////////////////////////////////////////////////////
#define PSP_SCREEN_WIDTH  (480)
#define PSP_SCREEN_HEIGHT (272)
#define	LINESIZE	  (512)
#define PIXELSIZE     (1)
#define	FRAMESIZE	  (0x00044000)
////////////////////////////////////////////////////////
#define PSP_RGB(r,g,b) ((((b>>3) & 0x1F)<<10)|(((g>>3) & 0x1F)<<5)|(((r>>3) & 0x1F)<<0))
////////////////////////////////////////////////////////

//#define SCREEN_WIDTH  480
//#define SCREEN_HEIGHT 272

//#define PIXELSIZE	1			//in short
//#define LINESIZE	512 		//in short
//#define FRAMESIZE	0x44000 	//in byte



#include"syscall.h"


//void SetExitCallback(int cbid);
//void KernelPollCallbacks(void);


#endif /* _MY_PG_H_ */

