
typedef struct {
	unsigned long channels;
	unsigned long samplerate;
	unsigned long samplecount;
	unsigned long datalength;
	char *wavdata;
	unsigned long rateratio;		// samplerate / 44100 * 0x10000
	unsigned long playptr;
	unsigned long playptr_frac;
	int playloop;
} wavout_wavinfo_t;

int wavoutInit();
void wavoutStopPlay1();
void wavoutStartPlay1();

//void audio_update(void);

// New struct, happily encapsulates all the sound info
struct sndinfo { unsigned short *l, *r; int len; };

extern struct sndinfo sndi;

extern int may_want_to_get_sound(struct sndinfo *sndi);
