/*
	sound.c
	YM2612 and SN76489 emulation
*/

#include "..\shared.h"

/* for debug macro */
 #define ENABLE_SOUNDS

/* 1:use osakana timers. 0:other. */
//#define USE_OSAKANA 1
#define USE_OSAKANA 0


/* YM2612 data */
static uint8 fm_reg[2][0x100];	/* Register arrays (2x256) */
static uint8 fm_latch[2];		/* Register latches */
static uint8 fm_status;			/* Read-only status flags */



/**************/
#if USE_OSAKANA
/* YM2612 data(osakana timers ) */
static int fm_timera_tab[0x400];	/* Precalculated timer A values */
static int fm_timerb_tab[0x100];	/* Precalculated timer B values */
static t_timer timer[2];			/* Timers A and B */
#else
static int  ras_fm_ticker[4];
#endif

void fm_update_timers(void)
{
#if USE_OSAKANA
	int i;

	/* Process YM2612 timers */
	for(i = 0; i < 2; i += 1)
	{
		/* Is the timer running? */
		if(timer[i].running)
		{
			/* Each scanline takes up roughly 64 microseconds */
			timer[i].count += 64;
			/* Check if the counter overflowed */
			if(timer[i].count > timer[i].base)
			{
				timer[i].count   = 0;	/* Reload counter */
				timer[i].running = 0;	/* Disable timer */
				/* Set overflow flag (if flag setting is enabled) */
				if(timer[i].enable)
				{
					fm_status |= (1 << i);
				}
			}
		}
	}
#else
//	int amax,bmax, i;
	int i;

	// Our raster lasts 63.61323 microseconds
//	for (i=0;i<4;i++) ras_fm_ticker[i]+=64;
	ras_fm_ticker[0]+=64;
	ras_fm_ticker[1]+=64;
	ras_fm_ticker[2]+=64;
	ras_fm_ticker[3]+=64;

//	if (sound_is_okay) ret=YM2612Read(0,a&3);

	// period of timer a is
	// 18 * (1024 - ((fm_reg[0][0x24]<<2)+(fm_reg[0][0x25])&3))) microsecs

	/* calculate the FM timer A maximum. */
	i=18 * (1024 -  ((fm_reg[0][0x24]<<2)+(fm_reg[0][0x25]&3)) );
	if (i<=0) i=1;
	if (ras_fm_ticker[0]>=i)
	{
		fm_status|=1; ras_fm_ticker[0]-=i;
	}

	/* calculate the FM timer B maximum. */
	i=288*(256-fm_reg[0][0x26]);
	if (i<=0) i=1;
	if (ras_fm_ticker[1]>=i)
	{
		fm_status|=2; ras_fm_ticker[1]-=i;
	}
//	return 0;
#endif
}

/**************/

/* Initialize the YM2612 and SN76489 emulation */
void sound_init(void)
{
#if USE_OSAKANA
/*psp_dgen100*///#define VCLK   (7520000L)

	/* Timers run at half the YM2612 input clock */
//	float clock = ((53.693175 / 7  ) / 2);
	float clock = ((53.693175 / 7.5) / 2);
//	float clock = (((7.520000)     ) / 2);
	int i;

	/* Make FM Timer A table */
	for(i = 0; i < 1024; i += 1)
	{
		/* Formula is "time(us) = 72 * (1024 - A) / clock" */
		fm_timera_tab[i] = ((int)(float)(72 * (1024 - i)) / (clock));
	}

	/* Make FM Timer B table */
	for(i = 0; i < 256; i += 1)
	{
		/* Formula is "time(us) = 1152 * (256 - B) / clock" */
		fm_timerb_tab[i] = (int)(float)(1152 * (256 - i)) / clock;
	}
#endif
}

void sound_reset(void)
{
	if(snd.enabled)
	{
		YM2612ResetChip(/*0*/);
	}
}

void fm_write(int address, int data)
{
	int a0 = ( address       & 1);
	int a1 = ((address >> 1) & 1);

//return;/////////////////////////////////////////////////////////////////////////

	if(a0)
	{
		/* Register data */
		fm_reg[a1][fm_latch[a1]] = data;

#if 000
/*???*/
		/* Timer control only in set A */
		if(a1 == 0)
#endif
		switch(fm_latch[a1])
		{
#if USE_OSAKANA
		case 0x24: /* FM Timer A (LSB) */
			timer[0].index = (timer[0].index & 0x0003) | (data << 2);
			timer[0].index &= 0x03FF;
			timer[0].base = fm_timera_tab[timer[0].index];
			break;

		case 0x25: /* FM Timer A (MSB) */
			timer[0].index = (timer[0].index & 0x03FC) | (data & 3);
			timer[0].index &= 0x03FF;
			timer[0].base = fm_timera_tab[timer[0].index];
			break;

		case 0x26: /* FM Timer B */
			timer[1].index = data;
			timer[1].base = timer[1].count = fm_timerb_tab[timer[1].index];
			break;
#else
/* I used to FM core DAC. see 'fm.c' */
//		case 0x2a : /*dac_submit(v);*/ return  ;	/* YM2612 DAC out */
//		case 0x2b : /*dac_enable(v);*/ return  ;	/* YM2612 DAC Sel */
#endif
			case 0x27: /* FM Timer Control */
#if USE_OSAKANA
				/* LOAD */
				timer[0].running = (data >> 0) & 1;	if(timer[0].running) timer[0].count = 0;
				timer[1].running = (data >> 1) & 1;	if(timer[1].running) timer[1].count = 0;

				/* ENABLE */
				timer[0].enable = (data >> 2) & 1;
				timer[1].enable = (data >> 3) & 1;
#endif
				/* RESET */
				if(data & 0x10) fm_status &= ~1;
				if(data & 0x20) fm_status &= ~2;
				break;
#if USE_OSAKANA
#else
			default:	if((fm_latch[a1]&0xfc)==0x24) { return  ; }
#endif
		}
	}
	else
	{
		/* Register latch */
		fm_latch[a1] = data;
	}

	/* ym2612 レジスタ更新 */
	if(snd.enabled)
	{
		YM2612Write(/*0,*/ address & 3, data);
	}
}


int fm_read(int address)
{
#if USE_OSAKANA
	return (fm_status);
#else
	if ((address&1) )
	{
		if(snd.enabled) YM2612Read(/*0,*/ /*address&3*/);
	}
	else
		return fm_status;
	return 0;
#endif
}


void psg_write(int data)
{
	/* sn76496 レジスタ更新 */
	if(snd.enabled)
	{
		SN76496Write(/*0,*/ data);
	}
}

/*後始末*/
#undef USE_OSAKANA
#ifdef ENABLE_SOUNDS
#undef ENABLE_SOUNDS
#endif
