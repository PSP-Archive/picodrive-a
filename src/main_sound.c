
#include "shared.h"


/* */
#define SND1_MAXSLOT	3
//#define SND_BUFSIZE 	256
//#define SND_BUFSIZE 	512
#define SND_BUFSIZE 	735

//wavout_wavinfo_t wavout_snd1_wavinfo[SND1_MAXSLOT];
//int wavout_snd1_playing[SND1_MAXSLOT];

int wavout_snd1_c[SND1_MAXSLOT];	/* the sound samples pointer */


//void wavoutStopPlay1()



int wavoutInit()
{
//	wavoutStopPlay1();
//	pgaSetChannelCallback(1,wavout_snd1_callback);
//	{
//		int i;
//		for (i=0; i<SND1_MAXSLOT; i++) wavout_1secInits(i);
//	}
	return 0;
}


//void audio_update(void)


static unsigned short* md_sound_ptr ;

struct sndinfo sndi;


// static FMSAMPLE audio_bufferRL[ SND_BUFSIZE*2  ];
int may_want_to_get_sound(struct sndinfo *sndi)
{
//	int i;
//	int len = sndi->len;
//	int in_dac, cur_dac = 0, acc_dac = len;
//	short* pl ;
	//int *dac = dac_data[dac_play & 0x07] - 1;

//{

//	int i;
	int len_fm;
#if 00
	int len_sn;
#endif
//	int16 acc;
//	FMSAMPLE *Buf[2];
	FMSAMPLE *bufferRL;

//	Buf[0] = snd.fm.buffer[0] ;//+ snd.fm.lastStage;
//	Buf[1] = snd.fm.buffer[1] ;//+ snd.fm.lastStage;
//	bufferRL = audio_bufferRL;	/*safety*/
	bufferRL = (FMSAMPLE*) sndi->r;	/*safety*/
	len_fm=snd.buffer_size - snd.snd_lastStage;
//	YM2612UpdateOne(/*0,*/  Buf, len_fm);
	YM2612_76496UpdateOne(/*0,*/  bufferRL, len_fm);

#if 00
//	Buf[0] = snd.psg.buffer ;//+ snd.psg.lastStage;
	len_sn=snd.buffer_size - snd.psg.lastStage;
//	SN76496Update(0, Buf[0], len_sn);
	SN76496Update(/*0,*/ bufferRL, len_sn);
#endif

#if 000
/* ƒƒ‚ */
//	pl = (FMSAMPLE*) sndi->r;	/*safety*/
//	for(i = 0; i < snd.buffer_size; i++)
	for(i = 0; i < len_fm; i++)
//	for(i = 0; i < len; i++)
//	{
//	for(i = 0; i < (len ); )
	{
int mysn;

#if 00
//mysn = snd.psg.buffer[i];		if( (++wavout_snd1_c[0]) >= (SND_BUFSIZE*60) )wavout_snd1_c[0] = 0;
/* fcpsoundbuff[0][wavout_snd1_c[0]] =*/mysn= snd.psg.buffer[i];		if( (++wavout_snd1_c[0]) >= (SND_BUFSIZE*60) )wavout_snd1_c[0] = 0;

*pl++ = (snd.fm.buffer[0][i]+mysn);	if( (++wavout_snd1_c[1]) >= (SND_BUFSIZE*60) )wavout_snd1_c[1] = 0;
*pl++ = (snd.fm.buffer[1][i]+mysn);	if( (++wavout_snd1_c[2]) >= (SND_BUFSIZE*60) )wavout_snd1_c[2] = 0;
#else
//mysn = snd.psg.buffer[i];		if( (++wavout_snd1_c[0]) >= (SND_BUFSIZE*60) )wavout_snd1_c[0] = 0;
/* fcpsoundbuff[0][wavout_snd1_c[0]] =*//*mysn= snd.psg.buffer[i];*/		if( (++wavout_snd1_c[0]) >= (SND_BUFSIZE*60) )wavout_snd1_c[0] = 0;

/* *pl++ = (*bufferRL++);*/	if( (++wavout_snd1_c[1]) >= (SND_BUFSIZE*60) )wavout_snd1_c[1] = 0;
/* *pl++ = (*bufferRL++);*/	if( (++wavout_snd1_c[2]) >= (SND_BUFSIZE*60) )wavout_snd1_c[2] = 0;

#endif
	}
//for debug
//	text_counter( 70, 100, acc, 3, -1);
#endif



wavout_snd1_c[0]+=len_fm;	if( (wavout_snd1_c[0]) >= (SND_BUFSIZE*60) )wavout_snd1_c[0] = 0;
wavout_snd1_c[1]+=len_fm;	if( (wavout_snd1_c[1]) >= (SND_BUFSIZE*60) )wavout_snd1_c[1] = 0;
wavout_snd1_c[2]+=len_fm;	if( (wavout_snd1_c[2]) >= (SND_BUFSIZE*60) )wavout_snd1_c[2] = 0;

	return 0;
}


void md_sound_update(unsigned short* p, short len )
{
	if( p )
	{
		md_sound_ptr = p   ;
		sndi.r   = p       ;
		sndi.len = len     ;

		may_want_to_get_sound( &sndi ) ;
	}
}



