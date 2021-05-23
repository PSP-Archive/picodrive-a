
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
		text_print(  20+100,  ( 30+ 80), "チェックサム値：", COLOR_WHITE,0,0);
		text_counter(20+240 , ( 30+ 80), (j ), 8, COLOR_WHITE);
		text_print(  20+100,  ( 30+100), "カードリッジ値：", COLOR_WHITE,0,0);
		text_counter(20+240 , ( 30+100), (k ), 8, COLOR_WHITE);
		pgScreenFlipV();
	}

	if( (j)==(k) )
		return RET_OK; // 1:OK
	else{
		if(menu_value[MENU_SUMFIX]){
			pgScreenFlipV();
			text_print( 20, ( 30+120), "自動修正しますか？（×いいえ　○はい）"		, COLOR_HCYAN,0,0);
			text_print( 20, ( 30+130), "注意：ファイルには書き込みません"			, COLOR_WHITE,0,0);
			text_print( 20, ( 30+140), "「Phantasy Star II」や「Dynamite Heddy」や「Thunder Force IV」等は、", COLOR_GRAY,0,0);
			text_print( 20, ( 30+150), "自動修正してしまうと起動しません。"			, COLOR_GRAY,0,0);
			pgScreenFlipV();

			key_release_chk();	/* ボタン離れるまで待つ */

			i=1;
			while(i){
				v_sync();
				pad_read();
				if(ps_X && is_X ) i=0;
				if(ps_O && is_O ) i=0;
			}
/* 「Phantasy Star Ⅱ」は、自動修正してしまうと起動しなくなるので保留 */
/* 「dynamite Heddy」は、自動修正してしまうと起動しなくなるので保留 */
			if(is_O){
				// チェックサム自動修正
				cart_rom[0x18e]=((j>>8)&0xff);
				cart_rom[0x18f]=( j    &0xff);

				k  = (cart_rom[0x18e]<<8)+(cart_rom[0x18f]);
				pgScreenFlipV();
				text_print(  20+100,  ( 30+160), "　　　　修正値：", COLOR_RED,0,0);
				text_counter(20+240 , ( 30+160), (k ), 8, COLOR_WHITE);
				pgScreenFlipV();
			}else{
				pgScreenFlipV();
				text_print( 20, ( 30+160), "修正しません"		, COLOR_YELLOW,0,0);
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
			// 解凍
			pgScreenFlipV();
			text_print( 20, ( 30+20), "解凍ＯＫ", COLOR_WHITE,0,0);
			pgScreenFlipV();
		}else{
			pgScreenFlipV();
			text_print( 20, ( 30+20), "解凍ＮＧ", COLOR_RED,0,0);
			pgScreenFlipV();
			return	RET_NG; // 0:解凍失敗
		}
	}else{
		fd = sceIoOpen(path_rom,SCE_O_RDONLY,0777);
		if( fd<0 ){
			pgScreenFlipV();
			text_print( 20, ( 30+20), "バイナリＮＧ", COLOR_RED,0,0);
			pgScreenFlipV();
			return RET_NG; // 0:失敗
		}
		readed_size = sceIoRead(fd, (char *)cart_rom, MAX_ROM_SIZE);
		sceIoClose(fd);
			pgScreenFlipV();
			text_print( 20, ( 30+20), "バイナリＯＫ", COLOR_WHITE,0,0);
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
	text_print(  20     , ( 30+40), "読み込み成功", COLOR_WHITE,0,0);
	text_print(  20+100 , ( 30+40), "読み込みバイト：", COLOR_WHITE,0,0);
	text_counter(20+240 , ( 30+40), readed_size, 8, COLOR_WHITE);
	pgScreenFlipV();

	if(sega_check()){
		pgScreenFlipV();
		text_print( 20, ( 30+60), "セガチェック成功", COLOR_WHITE,0,0);
		pgScreenFlipV();
	}else{
		pgScreenFlipV();
		text_print( 20, ( 30+60), "セガチェック失敗", COLOR_RED,0,0);
		pgScreenFlipV();
	}

	if(sum_check()){
		pgScreenFlipV();
		text_print( 20, ( 30+80), "チェックサムＯＫ", COLOR_WHITE,0,0);
		pgScreenFlipV();
	}else{
		pgScreenFlipV();
		text_print( 20, ( 30+80), "チェックサムＮＧ", COLOR_HCYAN,0,0);
		pgScreenFlipV();
	}

#if 00
	{
	int i;
//		for(i=0;i<(2*60);i++){	v_sync();	} /* wait 2 sec */
		for(i=0;i<(1*60);i++){	v_sync();	} /* wait 1 sec */
	}
#endif
	return (RET_OK); //1:成功
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

