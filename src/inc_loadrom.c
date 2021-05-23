
//#if 00
//#include "./../shared.h"
//
//#include "adler32.c"
//#include "crc32.c"
//#include "infback.c"
//#include "inffast.c"
//#include "inflate.c"
//#include "inftrees.c"
//#include "ioapi.c"
//#include "uncompr.c"
//#include "unzip.c"
//#include "zlibFileMemory.c"
//#include "zlibInterface.c"
//#include "zutil.c"
//#else
#include "shared.h"
//#endif

#include "_clib.c"

#define RET_OK 1
#define RET_NG 0

static int readed_size;

static int sum_check(void)
{
int i,j,k;
	j=0;
	for(i=0x200;i<(readed_size+1-2);i+=2){
		j+=(cart_rom[i  ]<<8);
		j+= cart_rom[i+1];
	}
	j &= 0xffff;
	k  = (cart_rom[0x18e]<<8)+(cart_rom[0x18f]);
	if(menu_value[MENU_SUMFIX]){
		pgScreenFlipV();
		text_print(  20+100,  ( 30+ 80), "�`�F�b�N�T���l�F", COLOR_WHITE,0,0);
		text_counter(20+240 , ( 30+ 80), (j ), 8, COLOR_WHITE);
		text_print(  20+100,  ( 30+100), "�J�[�h���b�W�l�F", COLOR_WHITE,0,0);
		text_counter(20+240 , ( 30+100), (k ), 8, COLOR_WHITE);
		pgScreenFlipV();
	}

	if( (j)==(k) )
		return RET_OK; // 1:OK
	else{
		if(menu_value[MENU_SUMFIX]){
			pgScreenFlipV();
			text_print( 20, ( 30+120), "�����C�����܂����H�i�~�������@���͂��j"		, COLOR_HCYAN,0,0);
			text_print( 20, ( 30+130), "���ӁF�t�@�C���ɂ͏������݂܂���"			, COLOR_WHITE,0,0);
			text_print( 20, ( 30+140), "�uPhantasy Star II�v��uDynamite Heddy�v��uThunder Force IV�v���́A", COLOR_GRAY,0,0);
			text_print( 20, ( 30+150), "�����C�����Ă��܂��ƋN�����܂���B"			, COLOR_GRAY,0,0);
			pgScreenFlipV();

			key_release_chk();	/* �{�^�������܂ő҂� */

			i=1;
			while(i){
				v_sync();
				pad_read();
				if(ps_X && is_X ) i=0;
				if(ps_O && is_O ) i=0;
			}
/* �uPhantasy Star �U�v�́A�����C�����Ă��܂��ƋN�����Ȃ��Ȃ�̂ŕۗ� */
/* �udynamite Heddy�v�́A�����C�����Ă��܂��ƋN�����Ȃ��Ȃ�̂ŕۗ� */
			if(is_O){
				// �`�F�b�N�T�������C��
				cart_rom[0x18e]=((j>>8)&0xff);
				cart_rom[0x18f]=( j    &0xff);

				k  = (cart_rom[0x18e]<<8)+(cart_rom[0x18f]);
				pgScreenFlipV();
				text_print(  20+100,  ( 30+160), "�@�@�@�@�C���l�F", COLOR_RED,0,0);
				text_counter(20+240 , ( 30+160), (k ), 8, COLOR_WHITE);
				pgScreenFlipV();
			}else{
				pgScreenFlipV();
				text_print( 20, ( 30+160), "�C�����܂���"		, COLOR_YELLOW,0,0);
				pgScreenFlipV();
			}
			for(i=0;i<(1*60);i++){	v_sync();	} /* wait 1 sec */
		}/*	if(menu_value[MENU_SUMFIX])*/
		return RET_NG; // 0:NG
	}
}

static int sega_check(void)
{
	if( (cart_rom[0x100]=='S') &&
		(cart_rom[0x101]=='E') &&
		(cart_rom[0x102]=='G') &&
		(cart_rom[0x103]=='A')
		)
		return RET_OK; // 1:OK
	else
		return RET_NG; // 0:NG
}


static int fsize;
 int funcUnzipCallback(int nCallbackId,
	unsigned long ulExtractSize,
	unsigned long ulCurrentPosition,
	const void *pData,
	unsigned long ulDataSize,
	unsigned long ulUserData)
{
	const char *pszFileName;
	const unsigned char *pbData;

	switch(nCallbackId) {
	case UZCB_FIND_FILE:
	pszFileName = (const char *)pData;
		switch( ulUserData ){
		case	0:
		{
			int i;
			char	path[MAXPATH];
			//memcpy( path, (unsigned char*)pszFileName, MAXPATH);
			for(i=0;i<MAXPATH;i++){path[i]=pszFileName[i];}

//			if( !smdchk(path) ){
//			if(0==smdchk(path) ){
			switch( f_extention(path) ){
			case MY_EXT_SMD:
			case MY_EXT_BIN:
		//	if( ulExtractSize < 10000 ){
		//		return	UZCBR_PASS;
		//	}
				if( ulExtractSize < (0x500000+1) ){
					//char	patch[MAXPATH];
					fsize=0;
					return	UZCBR_OK;
				}else{
					return	UZCBR_CANCEL;
				}
			default:
				return	UZCBR_PASS;
			}
		}
			break;
/*
		case	1:
			if( ulExtractSize == 391736 ){
				fsize=0;
				return	UZCBR_OK;
			}else{
				return	UZCBR_PASS;
			}
			break;
*/
		}
		break;
	case UZCB_EXTRACT_PROGRESS:
	pbData = (const unsigned char *)pData;
		switch( ulUserData ){
		case	0:
			{
				//char *p=(void *)ulCurrentPosition;
				_memcpy( (void *)&cart_rom[fsize], (void *)pData, ulDataSize);
				fsize += ulDataSize;
			}
			break;
/*
		case	1:
			{
				char *p=(void *)ulCurrentPosition;
				_memcpy( (void *)&BmpBuffer[fsize], (void *)pData, ulDataSize);
				fsize += ulDataSize;
			}
			break;
*/
		}
		return	UZCBR_OK;
		break;
	default: // unknown...
		break;
	}
	return 1;
}


static uint8 block[0x4000] __attribute__((aligned(64))) ;
static void deinterleave_block(uint8 *src)
{
	int i;
	_memcpy(block, src, 0x4000);
	for(i=0; i<0x2000; i++)
	{
		src[i+i  ] = block[0x2000+i];
		src[i+i+1] = block[       i];
	}
}

static int load_rom(char *path_rom)
{
	int fd;
	int offset = 0;
	uint8 header[0x200];

	my_memset0(cart_rom, MAX_ROM_SIZE);

//	if( zipchk(path_rom) ){
	if( MY_EXT_ZIP==f_extention(path_rom) ){
		if( Unzip_execExtract(path_rom, 0) == UZEXR_OK ){
			readed_size = fsize;
			// ��
			pgScreenFlipV();
			text_print( 20, ( 30+20), "�𓀂n�j", COLOR_WHITE,0,0);
			pgScreenFlipV();
		}else{
			pgScreenFlipV();
			text_print( 20, ( 30+20), "�𓀂m�f", COLOR_RED,0,0);
			pgScreenFlipV();
			return	RET_NG; // 0:�𓀎��s
		}
	}else{
		fd = sceIoOpen(path_rom,SCE_O_RDONLY,0777);
		if( fd<0 ){
			pgScreenFlipV();
			text_print( 20, ( 30+20), "�o�C�i���m�f", COLOR_RED,0,0);
			pgScreenFlipV();
			return RET_NG; // 0:���s
		}
		readed_size = sceIoRead(fd, (char *)cart_rom, MAX_ROM_SIZE);
		sceIoClose(fd);
			pgScreenFlipV();
			text_print( 20, ( 30+20), "�o�C�i���n�j", COLOR_WHITE,0,0);
			pgScreenFlipV();
	}


//	if((readed_size / 512) & 1){
	if((readed_size >>9 ) & 1){
		int i;
		uint8 *ptr=cart_rom;

		readed_size   -= 512;
		offset += 512;

		_memcpy(header, ptr, 512);

	//	for(i=0; i<(readed_size/0x4000); i+=1)
	//	{
	//		deinterleave_block(ptr + offset + (i * 0x4000));
	//	}
		for(i=0; i<readed_size; i+=0x4000)
		{
			deinterleave_block(ptr+offset+i);
		}
		_memcpy(cart_rom, ptr + offset, readed_size);
	}

	pgScreenFlipV();
	text_print(  20     , ( 30+40), "�ǂݍ��ݐ���", COLOR_WHITE,0,0);
	text_print(  20+100 , ( 30+40), "�ǂݍ��݃o�C�g�F", COLOR_WHITE,0,0);
	text_counter(20+240 , ( 30+40), readed_size, 8, COLOR_WHITE);
	pgScreenFlipV();

	if(sega_check()){
		pgScreenFlipV();
		text_print( 20, ( 30+60), "�Z�K�`�F�b�N����", COLOR_WHITE,0,0);
		pgScreenFlipV();
	}else{
		pgScreenFlipV();
		text_print( 20, ( 30+60), "�Z�K�`�F�b�N���s", COLOR_RED,0,0);
		pgScreenFlipV();
	}

	if(sum_check()){
		pgScreenFlipV();
		text_print( 20, ( 30+80), "�`�F�b�N�T���n�j", COLOR_WHITE,0,0);
		pgScreenFlipV();
	}else{
		pgScreenFlipV();
		text_print( 20, ( 30+80), "�`�F�b�N�T���m�f", COLOR_HCYAN,0,0);
		pgScreenFlipV();
	}

#if 00
	{
	int i;
//		for(i=0;i<(2*60);i++){	v_sync();	} /* wait 2 sec */
		for(i=0;i<(1*60);i++){	v_sync();	} /* wait 1 sec */
	}
#endif
	return (RET_OK); //1:����
}
/*
int load_rom(char *path_rom)
{
	int readed_size, offset = 0;
	uint8 header[0x200];
	uint8 *ptr;

	ptr = load_archive(path_rom, &readed_size);
	if(!ptr) return (0);

	if((readed_size / 512) & 1)
	{
		int i;

		readed_size -= 512;
		offset += 512;

		memcpy(header, ptr, 512);

		for(i = 0; i < (readed_size / 0x4000); i += 1)
		{
			deinterleave_block(ptr + offset + (i * 0x4000));
		}
	}

	_memset(cart_rom, 0, MAX_ROM_SIZE);
	if(readed_size > MAX_ROM_SIZE) readed_size = MAX_ROM_SIZE;
	_memcpy(cart_rom, ptr + offset, readed_size);

	// Free allocated file data
	free(ptr);

	return (1);
}
*/
#undef RET_OK
#undef RET_NG

