#include "../inc/global.h"


#define LOG_TAG	__FILE__
#ifdef UI_DEBUG_		
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else
#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)

#endif // UI_DEBUG_	


// ===== Added by ZhangYuan 2016-11-15 ==============
//#define UI_BMP_LOGO_DEBUG_	

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef short 				SHORT;
typedef long 				LONG;


typedef struct BMP_FILE_HEADER
{
	WORD	usFileType;
	DWORD	ulngFileSize;
	WORD	usRsv1;
	WORD	usRsv2;
	DWORD	ulngFileOffBits;
}BMP_FILE_HEADER;

typedef struct BMP_INFO_HEADER
{
	DWORD	ulngSize;
	LONG	lngWidth;
	LONG	lngHeight;
	WORD	usPlanes;
	WORD	usBitCount;
	DWORD	ulngCompression;
	DWORD	ulngSizeImg;
	LONG	lngXPelsPerMeter;
	LONG	lngYPelsPerMeter;
	DWORD	ulngClrUsed;
	DWORD	ulngClrImportant;
}BMP_INFO_HEADER;

int BmpConvert(unsigned char *pucFileName, unsigned char *pucConvBmp, unsigned int uiConvBmpSize)
{
	unsigned char	aucBmpBuff[1024+1]={0}, aucConvBuf[1024+1]={0}, ucFlag=0x00, ucByte=0x00;
	int				iRet=0, iFd=0, iFileSize=0, iOffset=0, iFileLen=0, iBmpPixWidth=0, iBmpPixBitCount=0, iLogoOffset=0, iWidth=0;
	BMP_FILE_HEADER stBmpFileHeader;
	BMP_INFO_HEADER	stBmpInfoHeader;

	unsigned long	lngDataSizePerLine=0;
	int				iLcdLines=0, i=0, j=0, k=0;

	iRet = PdkFile_Exist(pucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Exist=%d, pucFileName=%s\n", iRet, pucFileName);
	if(iRet != PDK_RET_OK)
	{
		return -1;
	}

	iFd = PdkFile_Open(pucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open=%d, pucFileName=%s\n", iFd, pucFileName);
	if(iFd < 0)
	{
		return -2;
	}

	memset(aucBmpBuff, 0, sizeof(aucBmpBuff));
	//memset(aucBmpBuff, 0, sizeof(stBmpFileHeader));
	iRet = PdkFile_Read(iFd, aucBmpBuff, sizeof(BMP_FILE_HEADER));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, BMP_FILE_HEADER\n", iRet);
	stBmpFileHeader.ulngFileSize =  ((int)aucBmpBuff [5]) * 16777216 +
		((int)aucBmpBuff [4]) * 65536 +
		((int)aucBmpBuff [3]) * 256  +
		(int)aucBmpBuff [2];
	stBmpFileHeader.ulngFileOffBits = ((int)aucBmpBuff [13]) * 16777216 +
		((int)aucBmpBuff [12]) * 65536 +
		((int)aucBmpBuff [11]) * 256  +
		(int)aucBmpBuff [10];

	memset(aucBmpBuff, 0, sizeof(aucBmpBuff));
	iRet = PdkFile_Read(iFd, aucBmpBuff+sizeof(BMP_FILE_HEADER), sizeof(BMP_INFO_HEADER));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, BMP_INFO_HEADER\n", iRet);
	memset(&stBmpInfoHeader, 0, sizeof(stBmpInfoHeader));
	stBmpInfoHeader.ulngSize = ((int)aucBmpBuff [17]) * 16777216 +
		((int)aucBmpBuff [16]) * 65536 +
		((int)aucBmpBuff [15]) * 256  +
		(int)aucBmpBuff [14];

	stBmpInfoHeader.lngWidth = ((int)aucBmpBuff [21]) * 16777216 +
		((int)aucBmpBuff [20]) * 65536 +
		((int)aucBmpBuff [19]) * 256  +
		(int)aucBmpBuff [18];

	stBmpInfoHeader.lngHeight = ((int)aucBmpBuff [25]) * 16777216 +
		((int)aucBmpBuff [24]) * 65536 +
		((int)aucBmpBuff [23]) * 256  +
		(int)aucBmpBuff [22];

	stBmpInfoHeader.usPlanes = ((int)aucBmpBuff [27]) * 256  + (int)aucBmpBuff [26];
	stBmpInfoHeader.usBitCount = ((int)aucBmpBuff [29]) * 256  + (int)aucBmpBuff [28];

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stBmpFileHeader.ulngFileSize=%d, stBmpFileHeader.ulngFileOffBits=%d\n", stBmpFileHeader.ulngFileSize, stBmpFileHeader.ulngFileOffBits);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stBmpInfoHeader.ulngSize=%d, stBmpInfoHeader.lngWidth=%d, stBmpInfoHeader.lngHeight=%d\n", stBmpInfoHeader.ulngSize, stBmpInfoHeader.lngWidth, stBmpInfoHeader.lngHeight);

	if(stBmpInfoHeader.usBitCount != 1)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stBmpInfoHeader.usBitCount != 1\n");
		PdkFile_Close(iFd);
		return -3;
	}

	if(stBmpInfoHeader.lngWidth > 500)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stBmpInfoHeader.lngWidth > 500\n");
		PdkFile_Close(iFd);
		return -3;
	}
	iWidth = stBmpInfoHeader.lngWidth;

	memset(aucConvBuf, 0, sizeof(aucConvBuf));
	iRet = PdkFile_Read(iFd, aucConvBuf, 8);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d\n", iRet);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "aucBuf: ", aucConvBuf, 8);
	if ((aucConvBuf[0] == 0xFF)
		&&(aucConvBuf[1] == 0xFF)
		&&(aucConvBuf[2] == 0xFF))
	{
		ucFlag = 0x01;
	}
	else if((aucConvBuf[0] == 0x00)
		&&(aucConvBuf[1] == 0x00)
		&&(aucConvBuf[2] == 0x00))
	{
		ucFlag = 0x00;
	}

	ucByte = stBmpInfoHeader.lngWidth%8;
	if(ucByte == 0)
	{
		ucByte = 0x00;
	}
	else
	{
		ucByte = (uchar)(2^(stBmpInfoHeader.lngWidth/8));
	}

	lngDataSizePerLine= (stBmpInfoHeader.lngWidth * stBmpInfoHeader.usBitCount + 31)/8; //一个扫描行所占的字节数
	lngDataSizePerLine= lngDataSizePerLine/4*4; 			// 字节数必须是4的倍数

	iLcdLines = stBmpInfoHeader.lngHeight/8;
	if (stBmpInfoHeader.lngHeight != (iLcdLines*8))
	{
		iLcdLines ++;
	}
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iLcdLines * 8 * stBmpInfoHeader.lngWidth=%d\n", iLcdLines * stBmpInfoHeader.lngWidth);	
	memset(aucConvBuf, 0xff, iLcdLines * stBmpInfoHeader.lngWidth);
	for(i=stBmpInfoHeader.lngHeight - 1, j=0; i>=0; i--,j++)
	{
		iRet = PdkFile_Seek(iFd, stBmpFileHeader.ulngFileOffBits + i*lngDataSizePerLine, SEEK_SET);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d\n", iRet);	
			PdkFile_Close(iFd);
			return -3;
		}

		memset(aucBmpBuff, 0, sizeof(aucBmpBuff));
		iRet = PdkFile_Read(iFd, aucBmpBuff, lngDataSizePerLine);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, lngDataSizePerLine=%d, i=%d, j=%d\n", iRet, lngDataSizePerLine, i, j);
		if(ucFlag == 0x01)
		{
			for (k = 0; k < lngDataSizePerLine; k++)
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "j*lngDataSizePerLine + k=%d\n", (j*lngDataSizePerLine + k));
				if (k < stBmpInfoHeader.lngWidth/8)
				{						
					aucConvBuf[j*lngDataSizePerLine + k] = ~(aucBmpBuff[k]);
				}
				else if(k = stBmpInfoHeader.lngWidth/8)
				{						
					aucConvBuf[j*lngDataSizePerLine+k] = ~( aucBmpBuff[k] | 0xFF - ucByte);
				}
				else
				{
					aucConvBuf[j*lngDataSizePerLine+k] = 0x00;						
				}
			}
		}
		else
		{
			memcpy(aucConvBuf+j*lngDataSizePerLine, aucBmpBuff, lngDataSizePerLine);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "j*lngDataSizePerLine=%d, lngDataSizePerLine=%d\n", j*lngDataSizePerLine, lngDataSizePerLine);
		}
	}
	PdkFile_Close(iFd);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ConvBmp size: %d, stBmpInfoHeader.lngWidth=%ld, iWidth=%d\n", j*lngDataSizePerLine, stBmpInfoHeader.lngWidth, iWidth);
	//	iWidth = stBmpInfoHeader.lngWidth;
	memset(pucConvBmp, 0x00, uiConvBmpSize);
	pucConvBmp[iLogoOffset] = iLcdLines;
	iLogoOffset += 1;
	for(i=0; i<iLcdLines; i++)
	{
		pucConvBmp[iLogoOffset] = iWidth/256;
		pucConvBmp[iLogoOffset+1] = iWidth%256;

		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucConvBmp[%d]=%02x, pucConvBmp[%d]=%02x, iLogoOffset=%d\n", iLogoOffset, pucConvBmp[iLogoOffset], iLogoOffset+1, pucConvBmp[iLogoOffset+1], iLogoOffset);
		iLogoOffset += 2;

		for(j=0, k=0; j<(int)lngDataSizePerLine; j++)
		{
			int m=0, n=0;		
			for(m=7; m>=0; m--)
			{
				unsigned char ucLogo=0x00;
				ucLogo = 0x00;
				for(n=0; n<8; n++)
				{
					unsigned char ucTemp=0x00;
					ucTemp= *(aucConvBuf+i*lngDataSizePerLine*8+lngDataSizePerLine*n+j);
					ucTemp = (ucTemp>>m)&0x01;
					if (ucTemp == 0x00)
						ucLogo |= 0x01 << n;
				}

				k++;

				pucConvBmp[iLogoOffset] = ucLogo;
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucConvBmp[%d]=%02x, iLogoOffset=%d\n", iLogoOffset, pucConvBmp[iLogoOffset], iLogoOffset);
				iLogoOffset += 1;		
				if (k == (int)iWidth)
				{
					break;
				}
			}
			if (k == (int)iWidth)
				break;
		}
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Convert successfully, Len=%d\n", iLogoOffset);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Logo: ", pucConvBmp, iLogoOffset);

	return 0;
}



// ===== Added end 2016-11-15==============================


