/*
  File: fm.h -- header file for software emuration for FM sound genelator

*/
#ifndef _H_FM_FM_
#define _H_FM_FM_

////// starshine - one chip 4 me!
#define HAS_YM2612 1
#define BUILD_YM2612 (HAS_YM2612)       // build YM2612 emulator

/* stereo mixing / separate */
//#define FM_STEREO_MIX
/* output bit size 8 or 16 */
#define FM_SAMPLE_BITS 16

#define YM2203_NUMBUF 1

#ifdef FM_STEREO_MIX
  #define YM2151_NUMBUF 1
  #define YM2608_NUMBUF 1
  #define YM2612_NUMBUF 1
  #define YM2610_NUMBUF  1
#else
  #define YM2151_NUMBUF 2    /* FM L+R */
  #define YM2608_NUMBUF 2    /* FM L+R+ADPCM+RYTHM */
  #define YM2612_NUMBUF 2    /* FM L+R */
  #define YM2610_NUMBUF 2
#endif

#if (FM_SAMPLE_BITS==16)
typedef unsigned short FMSAMPLE;
typedef unsigned long FMSAMPLE_MIX;
#endif
#if (FM_SAMPLE_BITS==8)
typedef unsigned char  FMSAMPLE;
typedef unsigned short FMSAMPLE_MIX;
#endif

/* For YM2151/YM2608/YM2612 option */

typedef void (*FM_TIMERHANDLER)(void *param,int c,int cnt,double stepTime);
typedef void (*FM_IRQHANDLER)(void *param,int irq);
/* FM_TIMERHANDLER : Stop or Start timer         */
/* int n          = chip number                  */
/* int c          = Channel 0=TimerA,1=TimerB    */
/* int count      = timer count (0=stop)         */
/* doube stepTime = step time of one count (sec.)*/

/* FM_IRQHHANDLER : IRQ level changing sense     */
/* int n       = chip number                     */
/* int irq     = IRQ level 0=OFF,1=ON            */

#ifdef BUILD_OPN
/* -------------------- YM2203/YM2608 Interface -------------------- */
/*
** 'n' : YM2203 chip number 'n'
** 'r' : register
** 'v' : value
*/
//void OPNWriteReg(int n, int r, int v);

/*
** read status  YM2203 chip number 'n'
*/
unsigned char OPNReadStatus(int n);

#endif /* BUILD_OPN */




#if BUILD_YM2612

void YM2612Init(int rate_44100Hz );
//void * YM2612Init(int index, int baseclock, int rate,
//               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2612Shutdown(void *chip);
void YM2612ResetChip(void);
//void YM2612UpdateOne( FMSAMPLE **buffer, int length, int flags );
//void YM2612UpdateOne( FMSAMPLE **buffer, int length /*,int flags*/ );
void YM2612_76496UpdateOne( FMSAMPLE *bufferRL, int length /*,int flags*/ );

int YM2612Write(int a,unsigned char v);
unsigned char YM2612Read(void);

//int YM2612TimerOver(void *chip, int c );
//void YM2612Postload(void *chip);

int YM2612GetState( unsigned char* buff ) ;
int YM2612SetState( unsigned char* buff, int len ) ;
int YM2612SetRegs ( signed short* regs ) ;

#endif /* BUILD_YM2612 */


#endif /* _H_FM_FM_ */

#ifndef SN76496_H
#define SN76496_H

#define MAX_76496 1

//struct SN76496interface
//{
//    int num;
//	int baseclock[MAX_76496];
//	int volume[MAX_76496];
//};

/* Function prototypes */
void SN76496Write(/*int chip,*/int data);
//void SN76496Update(int chip,signed short int *buffer,int length);
///void SN76496Update( FMSAMPLE *bufferRL,int length);
//void SN76496_set_clock(int chip,int clock);
void SN76496_set_gain(int chip,int gain);
void SN76496_init(void);// int clock,int volume,int sample_rate);
//int SN76496_sh_start(int clock, int volume, int rate);

#endif

