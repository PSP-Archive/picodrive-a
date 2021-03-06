
#include "zlibType.h"
#include "zlibFileMemory.h"
#include "lib_main.h"

#include "zlib.h"
#include "unzip.h"





#include "unzip.c"


#define IsShiftJIS(x) ((unsigned char)((x ^ 0x20) - 0xA1) <= 0x3B)

int (*___zlib_pfuncCallback)(int nCallbackId, unsigned long ulExtractSize,
	unsigned long ulCurrentPosition, const void *pData,
	unsigned long ulDataSize, unsigned long ulUserData) = NULL;



void Unzip_setCallback(int (*pfuncCallback)(int nCallbackId, unsigned long ulExtractSize,
	unsigned long ulCurrentPosition, const void *pData,
	unsigned long ulDataSize, unsigned long ulUserData))
{
	___zlib_pfuncCallback = pfuncCallback;
}

static size_t z__strlen(const char *s)
{
	int ret;

	for(ret=0; s[ret]; ret++)
		;

	return ret;
}
int Unzip_execExtract(const char *pszTargetFile, unsigned long ulUserData)
{
unzFile hUnzip;
int nRet;
	nRet = UZEXR_FATALERROR;
	if(!___zlib_pfuncCallback) return UZEXR_INVALIDCALLBACK;

	hUnzip = unzOpen(pszTargetFile);
	if(!hUnzip)	return UZEXR_INVALIDFILE;

	do
	{
	char szConFilename[512];
	unz_file_info fileInfo;
	int nLen;
//	int i;

		if(
			unzGetCurrentFileInfo(
				hUnzip, &fileInfo, szConFilename,
				sizeof szConFilename ) != UNZ_OK) {
			nRet = UZEXR_INVALIDFILE;
			break;
		}

		nLen = z__strlen(szConFilename);
		// ディレクトリの場合
		if(nLen >= 2 && !IsShiftJIS(szConFilename[nLen - 2]) && szConFilename[nLen - 1] == '\\') {
			continue;
		}

		// ファイルの場合、まずコールバックで展開するか聞く
		nRet = ___zlib_pfuncCallback(UZCB_FIND_FILE, fileInfo.uncompressed_size,
			0, szConFilename, 0, ulUserData);
		if(nRet == UZCBR_PASS) {
			nRet = unzGoToNextFile(hUnzip);
			if(nRet == UNZ_END_OF_LIST_OF_FILE) {
				nRet = UZEXR_OK;
				break;
			}
			continue;
		}
		else if(nRet == UZCBR_CANCEL) {
			nRet = UZEXR_CANCEL;
			break;
		}

/* たぶん要らない
		for (i = 0; i < nLen; ++i)
		if(szConFilename[i] == '/')
		szConFilename[i] = '\\';	//Windowsなので、\マークがディレクトリの区切りとなる
*/

		// ファイルの場合
		if(unzOpenCurrentFile(hUnzip) != UNZ_OK) {
			nRet = UZEXR_INVALIDFILE;
			break;
		}

		// データの展開
		{
			unsigned char szBuffer[8192];
			unsigned long dwSizeRead;
			unsigned long dwBufPosition = 0;
			while ((dwSizeRead = unzReadCurrentFile(hUnzip, szBuffer, sizeof(szBuffer))) > 0)
			{
				nRet = ___zlib_pfuncCallback(UZCB_EXTRACT_PROGRESS, fileInfo.uncompressed_size,
					dwBufPosition, szBuffer, dwSizeRead, ulUserData);
				dwBufPosition += dwSizeRead;
				if(nRet == UZCBR_PASS) {
					break;
				}
				else if(nRet == UZCBR_CANCEL) {
					unzCloseCurrentFile(hUnzip);
					unzClose(hUnzip);
					return UZEXR_CANCEL;
				}
			}
		}
		unzCloseCurrentFile(hUnzip);
		nRet = unzGoToNextFile(hUnzip);
		if(nRet == UNZ_END_OF_LIST_OF_FILE) {
			nRet = UZEXR_OK;
			break;
		}
	} while (1);
	unzClose(hUnzip);
	return nRet;	//解凍するものが無いので、解凍しなかった。
}
