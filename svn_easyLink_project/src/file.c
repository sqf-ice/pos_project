/*
* ============================================================================
* COPYRIGHT
*              Pax CORPORATION PROPRIETARY INFORMATION
*   This software is supplied under the terms of a license agreement or
*   nondisclosure agreement with Pax Corporation and may not be copied
*   or disclosed except in accordance with the terms in that agreement.
*      Copyright (C) 2016 Pax Corporation. All rights reserved.
* Module Date:2016/09/05     
* Module Auth:huangwp     
* Description:file module

*      
* ============================================================================
*/

#include "..\inc\global.h"


#define LOG_TAG	__FILE__
#ifdef FILE_DEBUG		
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else
#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)

#endif // FILE_DEBUG



//the blk=1001 use to save the exist file info


//before create a new file, please fill this table
FILE_TABLE_T g_stFileTable[] = {
	{FILE_INFO_NAME, FILE_TYPE_SMALL},
	{"test", FILE_TYPE_LARGE},	
	{"testone", FILE_TYPE_MID},	
	{"testtwo", FILE_TYPE_SMALL},	
	{"testthree", FILE_TYPE_LARGE},
	{"getsize", FILE_TYPE_LARGE},
	{COMM_TYPE_FILE, FILE_TYPE_SMALL},
	{"emv_param", FILE_TYPE_LARGE},
};

#if 1
//save the file info to blk 1-3
static int SaveFileInfo(void)
{
	uint iWriteLen = 0;
	int iRet = 0;
	int i = 0;
	int iBlkCnt = 0;
	//FILE_INFO_T stFileInfo[MAX_FILE_CNT];
	int iFd = FILE_INFO_START_BLK;
	uint uiLen = 0;//the total len to write
	uint iLen = 0; //the len have writed

	iBlkCnt = sizeof(g_stFileInfo)/BLK_LEN;
	uiLen = sizeof(g_stFileInfo);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iBlkCnt= %d, uiLen = %d\n", iBlkCnt,uiLen);

	for(i = 0; i <= iBlkCnt; i++)
	{		
		if(i == iBlkCnt)//write the last data to last blk
		{
			iRet = TableFsWrite(iFd, 0 ,uiLen%BLK_LEN, (uchar*)g_stFileInfo + iLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite= %d, iFd = %d\n", iRet,iFd);			
			if(iRet == 0)
			{
				iLen = iLen + uiLen%BLK_LEN;
			}
			else
			{
				EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite= %d, iFd = %d\n", iRet,iFd);
				return FILE_ERR_WRITE_FAIL;
			}
		}
		else
		{
			iRet = TableFsWrite(iFd , 0 ,BLK_LEN, (uchar*)g_stFileInfo + iLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite= %d, iFd + iBlk = %d\n", iRet,iFd);			
			if(iRet == 0)
			{
				iLen = iLen + BLK_LEN;
			}
			else
			{
				EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite= %d, iFd + iBlk = %d\n", iRet,iFd);
				return FILE_ERR_WRITE_FAIL;
			}
			uiLen = uiLen - BLK_LEN;	//remaining len to write

		}
		iFd++;
	}

#ifdef FILE_DEBUG
	for(i  = 0; i < sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]); i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].ucFileName = %s\n", i, g_stFileInfo[i].ucFileName);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iStartBlk = %d\n", i, g_stFileInfo[i].iStartBlk);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iUseFlag = %d\n", i, g_stFileInfo[i].iUseFlag);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iOpenFlag = %d\n", i, g_stFileInfo[i].iOpenFlag);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iLen = %d\n", i, g_stFileInfo[i].iLen);
	}
#endif

	return 0;

}
#endif

static int PDK_ReadFileInfo(void)
{
	int iRet = 0;
	uint iReadLen = 0;
	int iFd = FILE_INFO_START_BLK;
	int i = 0;
	uint uiLen = 0;
	int iBlkCnt = 0;
	uint iLen = 0;


	uiLen = sizeof(g_stFileInfo);

	iBlkCnt = sizeof(g_stFileInfo)/BLK_LEN;
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iBlkCnt= %d, sizeof(g_stFileInfo)= %d\n", iBlkCnt, sizeof(g_stFileInfo));	
	memset(g_stFileInfo, 0, sizeof(g_stFileInfo));

	for(i = 0; i <= iBlkCnt; i++)
	{		
		if(i == iBlkCnt)
		{
			iRet = TableFsRead(iFd, 0 ,uiLen%BLK_LEN, (uchar*)g_stFileInfo + iLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TableFsRead= %d, iFd= %d\n", iRet, iFd);			
			if(iRet >= 0)
			{
				//iLen = iLen + uiLen%BLK_LEN;
				iLen = iLen + iRet;
			}
			else
			{
				EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsRead= %d, iFd= %d\n", iRet, iFd);
				return FILE_ERR_READ_FAIL; 
			}
		}
		else
		{
			iRet = TableFsRead(iFd, 0 ,BLK_LEN, (uchar*)g_stFileInfo + iLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TableFsRead= %d, iFd= %d\n", iRet, iFd);			
			if(iRet >= 0)
			{
				iLen = iLen + iRet;
			}
			else
			{
				EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsRead= %d, iFd= %d\n", iRet, iFd);
				return FILE_ERR_READ_FAIL; 
			}
			uiLen = uiLen - BLK_LEN;
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiLen - BLK_LEN + iOffset = %d\n", uiLen);
		}
		iFd++;
	}

	for(i  = 0; i < sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]); i++)
	{
		g_stFileInfo[i].iOpenFlag = 0;
	}

#ifdef FILE_DEBUG
	for(i  = 0; i < sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]); i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].ucFileName = %s\n", i, g_stFileInfo[i].ucFileName);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iStartBlk = %d\n", i, g_stFileInfo[i].iStartBlk);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iUseFlag = %d\n", i, g_stFileInfo[i].iUseFlag);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iOpenFlag = %d\n", i, g_stFileInfo[i].iOpenFlag);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iLen = %d\n", i, g_stFileInfo[i].iLen);
	}
#endif

	return 0;

}

static ushort GetFileType(const char *pszFileName)
{
	int iSize = 0;
	int i = 0;
	ushort iIndex = 0;

	iSize = sizeof(g_stFileTable)/sizeof(g_stFileTable[0]);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetBlk blk size = %d\n", iSize );
	for(i = 0; i < iSize; i++)
	{
		if(strcmp(g_stFileTable[i].ucFileName, pszFileName) == 0)
		{
			return g_stFileTable[i].iFileType;
		}
	}
	if(strstr(pszFileName, ".ui") != NULL)
	{
		return FILE_TYPE_MID;
	}
	else if(strstr(pszFileName, ".emv") != NULL)
	{
		return FILE_TYPE_LARGE;
	}
	else if(strstr(pszFileName, ".clss") != NULL)
	{
		return FILE_TYPE_LARGE;
	}
	else if(strstr(pszFileName, ".bmp") != NULL)
	{
		return FILE_TYPE_SMALL;
	}
	else if(strstr(pszFileName, ".lng") != NULL)
	{
		return FILE_TYPE_SMALL;
	}

	//default return mid size file
	return FILE_TYPE_MID;
}

static int GetBlk(const char *pszFileName)
{
	int iSize = 0;
	int i = 0;
	ushort iFileType = 0;
	int iStartBlk = 0;
	ushort iNum = 0;
	ushort iFileSize = 0;
	ushort iStart = 0;

	if(NULL == pszFileName)
	{
		return FILE_ERR_INVALID_PARAM;
	}
	iSize = sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetBlk blk size = %d\n", iSize );

	for(i = 0; i < iSize; i++)
	{
		if(strcmp(g_stFileInfo[i].ucFileName, pszFileName) == 0 && g_stFileInfo[i].iOpenFlag == 0)
		{
			g_stFileInfo[i].iCurrent = 0;
			g_stFileInfo[i].iOpenFlag = 1;
			return g_stFileInfo[i].iStartBlk;
		}
		else if(strcmp(g_stFileInfo[i].ucFileName, pszFileName) == 0 && g_stFileInfo[i].iOpenFlag == 1)
		{
			return FILE_ERR_OPENED;
		}
	}

	// find a empty place to save a new fd create a new fd
	iFileType = GetFileType(pszFileName);
	switch(iFileType)
	{
	case FILE_TYPE_SMALL:
		{
			//small file info save in g_stFileInfo[0] to g_stFileInfo[29]
			i = 0;
			iStart = i;
			iStartBlk = SAVE_FILE_STAART_BLK;
			iNum = MAX_FILE_CNT - LARGE_SIZE_FILE_CNT - MID_SIZE_FILE_CNT;
			iFileSize = SAMLL_FILE_SIZE;
			break;
		}
	case FILE_TYPE_MID:
		{
			i = 30;
			iStart = i;
			iStartBlk = SAVE_FILE_STAART_BLK + SAMLL_FILE_BLK_NUM; 
			iNum = MAX_FILE_CNT - LARGE_SIZE_FILE_CNT;
			iFileSize = MID_FILE_SIZE;
			break;
		}
	case FILE_TYPE_LARGE:
		{
			i = 50;
			iStart = i;
			iStartBlk = SAVE_FILE_STAART_BLK + SAMLL_FILE_BLK_NUM + MID_FILE_BLK_NUM;
			iNum = MAX_FILE_CNT;
			iFileSize = LARGE_FILE_SIZE;
			break;
		}
	default:
		{
			i = 30;
			iStart = i;
			iStartBlk = SAVE_FILE_STAART_BLK + SAMLL_FILE_BLK_NUM; 
			iNum = LARGE_SIZE_FILE_CNT;
			iFileSize = MID_FILE_SIZE;
			break;
		}

	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iFileType = %d\n iSize= %d\n  iNum = %d\n", iFileType, iSize, iNum);
	for(i; i < iNum; i++)
	{
		if(g_stFileInfo[i].iUseFlag == 0)
		{
			strcpy(g_stFileInfo[i].ucFileName, pszFileName);
			g_stFileInfo[i].iUseFlag = 1;
			g_stFileInfo[i].iStartBlk = iStartBlk + (i-iStart)*iFileSize;
			g_stFileInfo[i].iCurrent = 0;
			g_stFileInfo[i].iOpenFlag = 1;

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[i].iStartBlk = %d = %d+%d*%d\n", g_stFileInfo[i].iStartBlk, iStartBlk, i, iFileSize);

			return g_stFileInfo[i].iStartBlk;
		}

	}
	return FILE_ERR_OPEN_FAIL;

}

static int GetBlkIndex(int BlkFd)
{
	int iSize = 0;
	int i = 0;
	iSize = sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iSize= %d\n", iSize);

	for(i = 0; i < iSize; i++)
	{
		if(g_stFileInfo[i].iStartBlk == BlkFd)
		{
			return i;
		}
	}
	return -1;
}

#if 0
//the blk use to save the file info
static int SaveFileInfo(int iFd)
{
	int iIndex = 0;
	int iOffset = 0;
	uint uiLen = 0;    //the len to write
	uint iLen = 0 ;   //the len have writed
	int iSingleSize = 0;
	int iTotalSize = 0;
	int iStartPosition = 0;
	int iBlkIndex = 0;
	int iRet = 0;
	int iBlkCnt = 0;
	int i = 0;

	iSingleSize = sizeof(g_stFileInfo[0]);
#ifdef FILE_DEBUG
	ComDebugTx("SaveFileInfo sizeof(g_stFileInfo) = %d\n", sizeof(g_stFileInfo));
	ComDebugTx("SaveFileInfo sizeof(g_stFileInfo[0] = %d\n", sizeof(g_stFileInfo[0]));
	ComDebugTx("SaveFileInfo iSingleSize  = %d\n", iSingleSize);
#endif
	iIndex = GetBlkIndex(iFd);
#ifdef FILE_DEBUG
	ComDebugTx("SaveFileInfo GetBlkIndex iIndex = %d\n", iIndex);
#endif
	if(iIndex < 0)
	{
		return iIndex;
	}

	if(g_RemoveFlag)
	{
		g_stFileInfo[iIndex].iStartBlk = 0;
		g_RemoveFlag = 0;
	}

	iStartPosition = iSingleSize * iIndex;
#ifdef FILE_DEBUG
	ComDebugTx("SaveFileInfo iStartPosition= %d\n", iStartPosition);
#endif
	iBlkIndex = iStartPosition / BLK_LEN;
	iOffset = iStartPosition % BLK_LEN;
#ifdef FILE_DEBUG
	ComDebugTx("SaveFileInfo iBlkIndex= %d  iOffset= %d\n ", iBlkIndex, iOffset);
#endif

	if(iOffset + iSingleSize <= BLK_LEN)
	{
		iRet = TableFsWrite(iBlkIndex + FILE_INFO_START_BLK, iOffset, iSingleSize, (uchar*)(&g_stFileInfo[iIndex]));
#ifdef FILE_DEBUG
		ComDebugTx("SaveFileInfo TableFsWrite iRet = %d\n", iRet);
#endif
		if(iRet < 0)
		{
			return iRet;
		}
		return iSingleSize;
	}
	else if(iSingleSize + iOffset > BLK_LEN)
	{
		iBlkCnt = (iSingleSize + iOffset)/BLK_LEN;
#ifdef FILE_DEBUG
		ComDebugTx("SaveFileInfo iBlkIndex > BLK_LEN iBlkCnt = %d\n", iBlkCnt);
#endif
		for(i = 0; i <= iBlkCnt; i++)
		{		
			if(i == iBlkCnt)//write the last data to last blk
			{
				iRet = TableFsWrite(iBlkIndex + FILE_INFO_START_BLK, iOffset ,iSingleSize%BLK_LEN,  (uchar*)(&g_stFileInfo[iIndex]) + iLen);
#ifdef FILE_DEBUG
				ComDebugTx("SaveFileInfo TableFsWrite the last time iRet= %d\n", iRet );
#endif
				if(iRet == 0)
				{
					iLen = iLen + uiLen%BLK_LEN;
				}
				else
				{
					return FILE_ERR_WRITE_FAIL;
				}
			}
			else
			{
				//ComDebugTx("PdkFile_Write TableFsWrite before the last time iFd + iBlk = %d\n", iFd + iBlk );
				iRet = TableFsWrite(iBlkIndex + iBlkIndex, iOffset ,BLK_LEN - iOffset,  (uchar*)(&g_stFileInfo[iIndex]) + iLen);
#ifdef FILE_DEBUG
				ComDebugTx("SaveFileInfo TableFsWrite before the last time iRet = %d\n", iRet );
#endif
				if(iRet == 0)
				{
					iLen = iLen + BLK_LEN - iOffset;
				}
				else
				{
					return FILE_ERR_WRITE_FAIL;
				}
				iSingleSize = iSingleSize - BLK_LEN + iOffset;	//remaining len to write
				iOffset = 0;

			}
			iBlkIndex++;
		}
		return iLen;

	}

	return 0;
}
#endif

int PdkFile_Open (const char *pszFileName)
{

	return GetBlk(pszFileName);
}

int PdkFile_Exist (const char * pszFileName)
{
	int iSize = 0;
	int i = 0;
	iSize = sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]);

	for(i = 0; i < iSize; i++)
	{
		if(strcmp(g_stFileInfo[i].ucFileName, pszFileName) == 0)
		{
			return PDK_RET_OK;
		}
	}
	return FILE_ERR_NOT_EXIST;;
}

int PdkFile_Read (int iFd, uchar *psDataOut, uint uiLen)
{
	int iRet = 0;
	int i = 0;
	ushort iBlkCnt = 0;
	uint iLen = 0;
	int iIndex = 0;
	uint iCurrent = 0;
	ushort iBlk = 0;
	uint iOffset = 0;
	uint iToatalLen = 0;

	iToatalLen = uiLen;

	if(uiLen < 0)
	{
		return FILE_ERR_INVALID_PARAM;
	}

	iIndex = GetBlkIndex(iFd);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetBlkIndex= %d\n", iIndex);
	if(iIndex < 0)
	{
		return FILE_ERR_OTHER;
	}
	if(g_stFileInfo[iIndex].iOpenFlag == 0)
	{
		return FILE_ERR_NOT_OPENED;
	}

	//the postion is past the file tail
	if(uiLen > g_stFileInfo[iIndex].iLen - g_stFileInfo[iIndex].iCurrent)
	{
		//return FILE_ERR_READ_FAIL;
		uiLen = g_stFileInfo[iIndex].iLen - g_stFileInfo[iIndex].iCurrent;
	}

	iCurrent = g_stFileInfo[iIndex].iCurrent;

	iBlk = iCurrent/BLK_LEN;
	iOffset = iCurrent%BLK_LEN;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iIndex = %d, iCurrent = %d, iOffset = %d, iBlk = %d, iFd = %d, uiLen = %d\n", iIndex, iCurrent, iOffset, iBlk, iFd, uiLen);

	if((uiLen + iOffset) <= BLK_LEN)
	{
		iRet = TableFsRead(iFd + iBlk , iOffset ,uiLen, psDataOut);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read <= BLK_LEN, TableFsRead = %d, len = %d\n", iRet, strlen(psDataOut));			
		if(iRet >= 0)
		{
			g_stFileInfo[iIndex].iCurrent = iRet + g_stFileInfo[iIndex].iCurrent;
			return iRet;
		}
		else
		{
			EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsRead[line:%d] iRet = %d\n", __LINE__,iRet);
			return FILE_ERR_READ_FAIL;
		}
	}

	else if((uiLen + iOffset) > BLK_LEN)
	{
		iBlkCnt = (uiLen + iOffset)/BLK_LEN;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read > BLK_LEN, iBlkCnt = %d\n", iBlkCnt);
		for(i = 0; i <= iBlkCnt; i++)
		{		
			if(i == iBlkCnt)
			{
				iRet = TableFsRead(iFd + iBlk, iOffset ,uiLen%BLK_LEN, psDataOut + iLen);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iRet = %d, iFd + iBlk = %d\n", iRet, iFd + iBlk );				
				if(iRet >= 0)
				{
					//iLen = iLen + uiLen%BLK_LEN;
					iLen = iLen + iRet;
				}
				else
				{
					//return FILE_ERR_READ_FAIL; 
					return iLen;
				}
				//iLen = iLen + iRet;
				//uiLen = uiLen - uiLen%BLK_LEN;
			}
			else
			{
				iRet = TableFsRead(iFd + iBlk, iOffset ,BLK_LEN - iOffset, psDataOut + iLen);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iRet = %d, iFd + iBlk = %d\n", iRet, iFd + iBlk);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "psDataOut = %s\n", psDataOut);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "len = %d, psDataOut len = %d\n", iToatalLen - strlen(psDataOut), strlen(psDataOut));
				if(iRet >= 0)
				{
					//iLen = iLen + BLK_LEN - iOffset;
					iLen = iLen + iRet;
				}
				else
				{
					//return FILE_ERR_READ_FAIL;
					if(i == 0)
					{
						EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsRead[line:%d] iRet = %d\n", __LINE__,iRet);
						return FILE_ERR_READ_FAIL;
					}
					else
					{
						return iLen;
					}
				}

				//iLen = iLen + iRet;
				uiLen = uiLen - BLK_LEN + iOffset;
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiLen - BLK_LEN + iOffset = %d\n", uiLen);				
				iOffset = 0;

			}
			iFd++;
		}
		g_stFileInfo[iIndex].iCurrent = g_stFileInfo[iIndex].iCurrent + iLen;
		return iLen;
	}
	return FILE_ERR_OTHER;
}

int PdkFile_Write (int iFd, const uchar *psDataIn, uint uiLen)
{
	int iIndex = 0;
	ushort iBlk = 0;
	int iOffset = 0;
	uint iCurrent = 0;
	int iRet = 0;
	ushort iBlkCnt = 0;
	uint iLen = 0;   //the  len have writed
	int i = 0;
	uint iTotalLen = 0;
	uint iMaxSize = 0;
	ushort iFileType = 0;

	iTotalLen = uiLen;

	iIndex = GetBlkIndex(iFd);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetBlkIndex = %d\n", iIndex);
	if(iIndex < 0)
	{
		EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "GetBlkIndex = %d\n", iIndex);
		return FILE_ERR_OTHER;
	}

	iFileType = GetFileType(g_stFileInfo[iIndex].ucFileName);
	switch(iFileType)
	{
	case FILE_TYPE_SMALL:
		{
			iMaxSize = SAMLL_FILE_SIZE * BLK_LEN; //10K
			break;

		}
	case FILE_TYPE_MID:
		{
			iMaxSize = MID_FILE_SIZE * BLK_LEN; //30K
			break;
		}
	case FILE_TYPE_LARGE:
		{
			iMaxSize = LARGE_FILE_SIZE * BLK_LEN; //100K
			break;
		}
	default:
		{
			return FILE_ERR_OTHER;
		}
	}
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iFileType = %d\n  iMaxSize = %d\n", iFileType, iMaxSize);

	if(g_stFileInfo[iIndex].iOpenFlag == 0)
	{
		return FILE_ERR_NOT_OPENED;
	}
	if(uiLen > (uint)(iMaxSize - g_stFileInfo[iIndex].iCurrent))
	{
		return FILE_ERR_MEM_OVERFLOW;
	}
	if(uiLen < 0)
	{
		return FILE_ERR_INVALID_PARAM;
	}

	iCurrent = g_stFileInfo[iIndex].iCurrent;
	iBlk = iCurrent/BLK_LEN;
	iOffset = iCurrent%BLK_LEN;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iCurrent = %d, iBlk = %d, iOffset = %d, iFd = %d, uiLen = %d, g_stFileInfo[iIndex].iLen = %d\n", iCurrent, iBlk, iOffset, iFd, uiLen, g_stFileInfo[iIndex].iLen);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "psDataIn = %s\n", psDataIn );

	if((uiLen + iOffset) <= BLK_LEN)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite <= BLK_LEN, iFd + iBlk = %d\n", iFd + iBlk );

		iRet = TableFsWrite(iFd + iBlk , iOffset , uiLen, (uchar*)psDataIn);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite  <= BLK_LEN, iRet= %d, iFd + iBlk = %d\n", iRet, iFd + iBlk);		
		if(iRet == 0)
		{
			if(!g_stFileInfo[iIndex].iLen)//the file have no data
			{
				g_stFileInfo[iIndex].iLen = uiLen;
				//g_stFileInfo[iIndex].iCurrent = uiLen;
			}
			else if(iCurrent + uiLen > g_stFileInfo[iIndex].iLen)//when write the file hava data,and the len pass old file len
			{
				g_stFileInfo[iIndex].iLen = iCurrent + uiLen;
			}
			g_stFileInfo[iIndex].iCurrent = g_stFileInfo[iIndex].iCurrent + uiLen;
			SaveFileInfo();
			return uiLen;
		}
		else
		{
			EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite = %d\n", iRet);	
			return FILE_ERR_WRITE_FAIL;
		}
	}
	else if((uiLen + iOffset) > BLK_LEN)
	{
		iBlkCnt = (uiLen + iOffset)/BLK_LEN;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write > BLK_LEN, iBlkCnt = %d\n", iBlkCnt);	

		for(i = 0; i <= iBlkCnt; i++)
		{		
			if(i == iBlkCnt)//write the last data to last blk
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iFd + iBlk = %d\n", iFd + iBlk );				
				iRet = TableFsWrite(iFd + iBlk, iOffset ,uiLen%BLK_LEN, (uchar*)psDataIn + iLen);
				if(iRet == 0)
				{
					iLen = iLen + uiLen%BLK_LEN;
				}
				else
				{
					EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite = %d\n", iRet);	
					return FILE_ERR_WRITE_FAIL;
				}
			}
			else
			{
				iRet = TableFsWrite(iFd + iBlk, iOffset ,BLK_LEN - iOffset, (uchar*)psDataIn + iLen);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite = %d, iFd + iBlk = %d\n", iRet, iFd + iBlk );				
				if(iRet == 0)
				{
					iLen = iLen + BLK_LEN - iOffset;
				}
				else
				{
					EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsWrite = %d\n", iRet);
					return FILE_ERR_WRITE_FAIL;
				}
				uiLen = uiLen - BLK_LEN + iOffset;	//remaining len to write
				iOffset = 0;

			}
			iFd++;
		}
		g_stFileInfo[iIndex].iCurrent = iLen + g_stFileInfo[iIndex].iCurrent;
		if(g_stFileInfo[iIndex].iLen  <= iOffset + iTotalLen)
		{
			g_stFileInfo[iIndex].iLen = iLen;
		}
		else if(g_stFileInfo[iIndex].iCurrent >= g_stFileInfo[iIndex].iLen)
		{
			g_stFileInfo[iIndex].iLen = g_stFileInfo[iIndex].iCurrent;
		}
		SaveFileInfo();
		return iLen;
	}	
	return FILE_ERR_OTHER;
}

int PdkFile_Close (int iFd)
{
	int iIndex = 0;

	iIndex = GetBlkIndex(iFd);
	if(iIndex == -1)
	{
		return FILE_ERR_INVALID_PARAM;
	}
	if(g_stFileInfo[iIndex].iOpenFlag == 0)
	{
		return FILE_ERR_NOT_OPENED;
	}
	else
	{
		g_stFileInfo[iIndex].iOpenFlag = 0;
		return PDK_RET_OK;
	}
}

int PdkFile_Seek (int iFd, int iOffSet, uint uiFromWhere)
{
	uint iCurrent = 0;
	int iIndex = 0;
	ushort iFileType = 0;
	uint iMaxSize = 0;

	iIndex = GetBlkIndex(iFd);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetBlkIndex = %d\n", iIndex);
	if(iIndex < 0)
	{
		EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "GetBlkIndex = %d\n", iIndex);
		return FILE_ERR_OTHER;
	}


	if(((iOffSet < 0) && (uiFromWhere == SEEK_SET)) || ((iOffSet > 0) && (uiFromWhere == SEEK_END)) || (g_stFileInfo[iIndex].iLen < (uint)abs(iOffSet)))
	{
		return FILE_ERR_INVALID_PARAM;
	}

	iFileType = GetFileType(g_stFileInfo[iIndex].ucFileName);
	switch(iFileType)
	{
	case FILE_TYPE_SMALL:
		{
			iMaxSize = SAMLL_FILE_SIZE * BLK_LEN; //10K
			break;

		}
	case FILE_TYPE_MID:
		{
			iMaxSize = MID_FILE_SIZE * BLK_LEN; //30K
			break;
		}
	case FILE_TYPE_LARGE:
		{
			iMaxSize = LARGE_FILE_SIZE * BLK_LEN; //100K
			break;
		}
	default:
		{
			return FILE_ERR_OTHER;
		}
	}
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iFileType = %d\n  iMaxSize = %d\n", iFileType, iMaxSize);

	if(uiFromWhere == SEEK_CUR)
	{
		if(iOffSet >= 0)
		{
			iCurrent = g_stFileInfo[iIndex].iCurrent + iOffSet;	
			if(iCurrent > g_stFileInfo[iIndex].iLen)
			{
				return FILE_ERR_INVALID_PARAM;
			}	
		}
		else
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iOffSet < 0 g_stFileInfo[iIndex].iCurrent =  %d, abs(iOffSet) =  %d\n", g_stFileInfo[iIndex].iCurrent, abs(iOffSet));
			if(g_stFileInfo[iIndex].iCurrent <= (uint)abs(iOffSet))
			{
				return FILE_ERR_INVALID_PARAM;
			}
			iCurrent = g_stFileInfo[iIndex].iCurrent - abs(iOffSet);
		}
	}
	else if(uiFromWhere == SEEK_SET)
	{
		iCurrent = 0 + iOffSet;
	}
	else if(uiFromWhere == SEEK_END)
	{
		iCurrent = g_stFileInfo[iIndex].iLen - abs(iOffSet);
	}

	if(iCurrent > iMaxSize)
	{
		return FILE_ERR_END_OVERFLOW;
	}
	g_stFileInfo[iIndex].iCurrent = iCurrent;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "exit, g_stFileInfo[iIndex].iCurrent =  %d\n", g_stFileInfo[iIndex].iCurrent);

	return iCurrent;
}

int PdkFile_GetSize (const char * pszFileName)
{
	int iSize = 0;
	int i = 0;
	iSize = sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]);

	if(NULL == pszFileName)
	{
		return FILE_ERR_INVALID_PARAM;
	}
	for(i = 0; i < iSize; i++)
	{
		if(strcmp(g_stFileInfo[i].ucFileName, pszFileName) == 0) 
		{
			return g_stFileInfo[i].iLen;
		}
	}
	return FILE_ERR_NOT_EXIST;

}

/*****************************************
int PdkFile_Truncate(int iFd, uint uiLen)
{
int iRet = 0;
int iIndex = 0;
uint iRestLen = 0;
int iBlk = 0;
int iBlkCnt = 0;
int i = 0;

iIndex = GetBlkIndex (iFd);

if(iIndex < 0)
{
return FILE_ERR_OTHER;
}
if(!g_stFileInfo[iIndex].iOpenFlag)
{
return FILE_ERR_NOT_OPENED;
}
if(g_stFileInfo[iIndex].iLen < uiLen)
{
return FILE_ERR_END_OVERFLOW;
}

iBlk = uiLen/BLK_LEN;
//g_stFileInfo[iIndex].iCurrent = (int)uiLen;
iRestLen = g_stFileInfo[iIndex].iLen - uiLen;

if(iRestLen <= BLK_LEN)
{
iRet = TableFsErase (iFd + iBlk);
if(iRet < 0)
{
return FILE_ERR_OTHER;
}
}
else if(iRestLen > BLK_LEN)
{
iBlkCnt = iRestLen/BLK_LEN;
for(i = 0; i <= iBlkCnt; i++)
{		
iRet = TableFsErase(iFd + iBlk);
if(iRet < 0)
{
return FILE_ERR_OTHER;
}
iFd++;
}
}
g_stFileInfo[iIndex].iLen = uiLen;
return PDK_RET_OK;
}
***********************************************/

int PdkFile_Truncate(int iFd, uint uiLen)
{
	int iRet = 0;
	int iIndex = 0;
	uint iRestLen = 0;
	ushort iBlk = 0;
	ushort iBlkCnt = 0;
	int i = 0;

	iIndex = GetBlkIndex (iFd);

	if(iIndex < 0)
	{
		return FILE_ERR_OTHER;
	}
	if(!g_stFileInfo[iIndex].iOpenFlag)
	{
		return FILE_ERR_NOT_OPENED;
	}
	if(g_stFileInfo[iIndex].iLen < uiLen)
	{
		return FILE_ERR_END_OVERFLOW;
	}

	g_stFileInfo[iIndex].iLen = uiLen;
	SaveFileInfo();
	return PDK_RET_OK;
}


int PdkFile_Remove (const char * pszFileName)
{
	int iSize = 0;
	int i = 0;
	int iStartBlk = 0;
	int iFd = 0;
	int iRet = 0;
	ushort iIndex = 0;
	ushort iBlkNum = 0;

	iSize = sizeof(g_stFileInfo)/sizeof(g_stFileInfo[0]);

	if(NULL == pszFileName)
	{
		return FILE_ERR_INVALID_PARAM;
	}
	for(i = 0; i < iSize; i++)
	{
		if(strcmp(g_stFileInfo[i].ucFileName, pszFileName) == 0) 
		{
			iStartBlk = g_stFileInfo[i].iStartBlk;
			iIndex = i;
			break;
		}
		if(i == iSize - 1)
		{
			return FILE_ERR_NOT_EXIST;
		}
	}
	iBlkNum = g_stFileInfo[i].iLen/BLK_LEN;
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stFileInfo[%d].iLen =  %d, g_stFileInfo[%d].iStartBlk =  %d, iBlkNum =  %d\n", iIndex, g_stFileInfo[iIndex].iLen,iIndex, g_stFileInfo[iIndex].iStartBlk, iBlkNum);

	iFd = iStartBlk;
	for(i = 0; i < iBlkNum; i++)
	{
		iRet = TableFsErase(iStartBlk);
		iStartBlk++;
		if(iRet < 0)
		{
			return FILE_ERR_OTHER;
		}
	}

	memset(g_stFileInfo[iIndex].ucFileName, 0, FILE_NAME_LEN);
	g_stFileInfo[iIndex].iStartBlk = 0;//fd
	g_stFileInfo[iIndex].iUseFlag = 0;//use to flag if this array[i] is used
	g_stFileInfo[iIndex].iOpenFlag = 0;
	g_stFileInfo[iIndex].iCurrent = 0;//seek
	g_stFileInfo[iIndex].iLen = 0;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sizeof(g_stFileInfo) =  %d\n", sizeof(g_stFileInfo[0]));
	//memset(&g_stFileInfo[iIndex], 0, sizeof(g_stFileInfo[0]));
	SaveFileInfo();

	return PDK_RET_OK;

}

void SysFileInit(void)
{
	int iRet;

	iRet = TableFsInit();							//初始化表文件系统
	if (iRet==TBL_RET_NOT_FMT)
	{
		EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "TableFsInit==TBL_RET_NOT_FMT\n");
		iRet = TableFsFormat(DEFAULT_TBL_BLK_LEN);	//格式化1024字节的块(将文件系统格式化为每1024字节大小的为1块的格式)
		// TableFsErase(0);	
		return;//清空第0块数据
	}
	iRet = PDK_ReadFileInfo();
	if (iRet < 0)
	{
		return;
	}
}

#if 0
int Test_Write_not_exist()
{
	int iFd = 0;
	int iRet = 0;
	uint iWriteLen = 0;
	uint iReadLen = 0;
	uchar BufOut[2000] = {0};
	uchar BufIn[2000] = {0};
	int i = 0;

	for(i = 0 ; i <= 1028/4; i++)
	{
		memcpy(BufIn +i*4,"1234", 4);
	}
	for(i = 0; i < 256/4; i++)
	{
		memcpy(BufIn + 1028 + 4*i,"5678",4);
	}	

	ComDebugTx("Test_Write_not_exist memcpy BufIn len = %d\n", strlen(BufIn));
	ComDebugTx("Test_Write_not_exist memcpy BufIn = %s\n", BufIn);

	iFd = PdkFile_Open ("test");
	ComDebugTx("Test_Write_not_exist PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("Test_Write_not_exist PdkFile_Open fail\n");
		return iFd;
	}

	iWriteLen = PdkFile_Write (iFd, "huang wu ping", 20);
	ComDebugTx("Test_Write_not_exist first call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Write_not_exist first call PdkFile_Write fail\n");
	}

	iRet = PdkFile_Seek (iFd, 0, SEEK_SET);
	ComDebugTx("Test_Write_not_exist first call PdkFile_Seek iRet = %d\n", iRet);

	memset(BufOut, 0, sizeof(BufOut));
	iReadLen = PdkFile_Read(iFd, BufOut, 20);
	ComDebugTx("Test_Write_not_exist first call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Write_not_exist first  call PdkFile_Read BufOut = %s\n", BufOut);

	iRet = PdkFile_Seek (iFd, 0, SEEK_SET);
	ComDebugTx("Test_Write_not_exist second call PdkFile_Seek iRet = %d\n", iRet);

	iWriteLen = PdkFile_Write (iFd, BufIn, sizeof(BufIn));
	ComDebugTx("Test_Write_not_exist  second call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Write_not_exist second call PdkFile_Write fail\n");
	}

	iRet = PdkFile_Seek (iFd, 0, SEEK_SET);
	ComDebugTx("Test_Write_not_exist second call PdkFile_Seek iRet = %d\n", iRet);
	memset(BufOut, 0, sizeof(BufOut));
	iReadLen = PdkFile_Read(iFd, BufOut, sizeof(BufOut));
	ComDebugTx("Test_Write_not_exist second call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Write_not_exist second call PdkFile_Read BufOut = %s\n", BufOut);

	iRet = PdkFile_Seek (iFd, -10, SEEK_CUR);
	ComDebugTx("Test_Write_not_exist third call PdkFile_Seek iRet = %d\n", iRet);
	memset(BufOut, 0, sizeof(BufOut));
	iReadLen = PdkFile_Read(iFd, BufOut, 10);
	ComDebugTx("Test_Write_not_exist third call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Write_not_exist third call PdkFile_Read BufOut = %s\n", BufOut);

	iRet = PdkFile_Seek (iFd, -1499, SEEK_CUR);
	ComDebugTx("Test_Write_not_exist fourth call PdkFile_Seek iRet = %d\n", iRet);
	memset(BufOut, 0, sizeof(BufOut));
	iReadLen = PdkFile_Read(iFd, BufOut, 1499);
	ComDebugTx("Test_Write_not_exist fourth call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Write_not_exist fourth call PdkFile_Read BufOut = %s\n", BufOut);

	iRet = PdkFile_Seek (iFd, -1000, SEEK_CUR);
	ComDebugTx("Test_Write_not_exist fifth call PdkFile_Seek iRet = %d\n", iRet);
	memset(BufOut, 0, sizeof(BufOut));
	iReadLen = PdkFile_Write(iFd, "just test", 9);
	ComDebugTx("Test_Write_not_exist fifth call PdkFile_Write iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	iReadLen = PdkFile_Read(iFd, BufOut, 991);
	ComDebugTx("Test_Write_not_exist fifth call PdkFile_Read BufOut = %s\n", BufOut);
	ComDebugTx("Test_Write_not_exist fifth call PdkFile_Read iReadLen = %d\n", iReadLen);

	iRet = PdkFile_Close (iFd);
	ComDebugTx("Test_Write_not_exist PdkFile_Close iRet = %d\n", iRet);

	PDK_ReadFileInfo();

	return 0;

}

int Test_Write_exist()
{
	int iFd = 0;
	int iRet = 0;
	uint iWriteLen = 0;
	uint iReadLen = 0;
	uchar BufOut[2000] = {0};

	iFd = PdkFile_Open ("test");
	ComDebugTx("Test_Write_exist PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("Test_Write_exist PdkFile_Open fail\n");
		return iFd;
	}

	iReadLen = PdkFile_Read(iFd, BufOut, sizeof(BufOut));
	ComDebugTx("Test_Write_exist first call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Write_exist first call PdkFile_Read BufOut = %s\n", BufOut);

	//iRet = PdkFile_Seek (iFd, -15, SEEK_CUR);
	//ComDebugTx("Test_Write_exist fist call PdkFile_Seek iRet = %d\n", iRet);
	iWriteLen = PdkFile_Write (iFd, "d180s test file", 15);
	ComDebugTx("Test_Write_exist  fist call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Write_exist first call PdkFile_Write fail\n");
	}

	iRet = PdkFile_Seek (iFd, -15, SEEK_CUR);
	ComDebugTx("Test_Write_exist second call PdkFile_Seek iRet = %d\n", iRet);
	memset(BufOut, 0, sizeof(BufOut));
	iReadLen = PdkFile_Read(iFd, BufOut, 15);
	ComDebugTx("Test_Write_exist second call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Write_exist second call PdkFile_Read BufOut = %s\n", BufOut);

	iRet = PdkFile_Seek (iFd, 2, SEEK_SET);
	if(iRet < 0)
	{
		ComDebugTx("Test_Write_exist PdkFile_Seek iRet = %d\n", iRet);
	}

	memcpy(BufOut, 0, sizeof(BufOut));
	iReadLen = PdkFile_Read(iFd, BufOut, sizeof(BufOut) - 2);
	ComDebugTx("Test_Write_exist third call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Write_exist third call PdkFile_Read BufOut = %s\n", BufOut);
	iRet = PdkFile_Close (iFd);
	ComDebugTx("Test_Write_exist PdkFile_Close iRet = %d\n", iRet);
	PDK_ReadFileInfo();

	return 0;

}

int Test_Exist()
{
	int iFd = 0;
	int iRet = 0;

	iFd = PdkFile_Open ("test_exist");
	ComDebugTx("Test_Exist PdkFile_Open iFd = %d\n", iFd);
	if(iFd < 0)
	{
		ComDebugTx("Test_Write_exist PdkFile_Open fail\n");
		return iFd;
	}
	PdkFile_Close (iFd);

	iRet = PdkFile_Exist ("test_exist");
	ComDebugTx("Test_Exist test_exist iRet = %d\n", iRet);

	iRet = PdkFile_Exist ("test");
	ComDebugTx("Test_Exist test iRet = %d\n", iRet);

	iRet = PdkFile_Exist ("test_not_exist");
	ComDebugTx("Test_Exist test_not_exist iRet = %d\n", iRet);
	PDK_ReadFileInfo();

	return 0;
}

int Test_GetSize()
{
	int iFd = 0;
	int iRet = 0;
	uint iWriteLen = 0;
	uint Len = 0;

	char ucTestSize[] ="Test_GetSize_size";

	iFd = PdkFile_Open ("Test_GetSize");
	ComDebugTx("Test_Exist PdkFile_Open iFd = %d\n", iFd);
	if(iFd < 0)
	{
		ComDebugTx("Test_Write_exist PdkFile_Open fail\n");
		return iFd;
	}

	iWriteLen = PdkFile_Write (iFd, ucTestSize, strlen(ucTestSize));
	ComDebugTx("Test_GetSize Test_GetSize len = %d\n", strlen(ucTestSize));
	PdkFile_Close (iFd);

	Len = PdkFile_GetSize("test");
	ComDebugTx("Test_GetSize Test_GetSize test len = %d\n", Len);

	Len = PdkFile_GetSize("test_exist");
	ComDebugTx("Test_GetSize Test_GetSize test_exist len = %d\n", Len);

	Len = PdkFile_GetSize("test_exist_not Exist");
	ComDebugTx("Test_GetSize Test_GetSize test_exist_not  len = %d\n", Len);

	return 0;
}


int Test_Trunk()
{
	int iFd = 0;
	int iRet = 0;
	uchar ucBufOut[2000] = {0};
	uint iReadLen = 0;

	iFd = PdkFile_Open ("test");
	ComDebugTx("Test_Trunk PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("Test_Trunk PdkFile_Open fail\n");
		return iFd;
	}

	iRet = PdkFile_Truncate(iFd, 1200);
	ComDebugTx("Test_Trunk PdkFile_Truncate first iRet = %d\n", iRet );

	memset(ucBufOut, 0, sizeof(ucBufOut));
	iReadLen = PdkFile_Read(iFd, ucBufOut, 1200);
	ComDebugTx("Test_Trunk first call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Trunk first call PdkFile_Read ucBufOut = %s\n", ucBufOut);

	iRet = PdkFile_Truncate(iFd, 6);
	ComDebugTx("Test_Trunk PdkFile_Truncate second iRet = %d\n", iRet );

	iRet = PdkFile_Seek (iFd, 0, SEEK_SET);
	ComDebugTx("Test_Trunk second call PdkFile_Seek iRet = %d\n", iRet);
	memset(ucBufOut, 0, sizeof(ucBufOut));
	iReadLen = PdkFile_Read(iFd, ucBufOut, 6);
	ComDebugTx("Test_Trunk second call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Trunk second call PdkFile_Read BufOut = %s\n", ucBufOut);

	iRet = PdkFile_Seek (iFd, 0, SEEK_SET);
	ComDebugTx("Test_Trunk third call PdkFile_Seek iRet = %d\n", iRet);
	memset(ucBufOut, 0, sizeof(ucBufOut));
	iReadLen = PdkFile_Read(iFd, ucBufOut, 10);
	ComDebugTx("Test_Trunk third call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Trunk third call PdkFile_Read BufOut = %s\n", ucBufOut);

	return iRet;
}

int Test_Remove()
{
	int iRet = 0;
	int iFd = 0;
	iRet = PdkFile_Remove ("1234");

	ComDebugTx("Test_Remove first PdkFile_Remove iRet = %d\n", iRet);

	iRet = PdkFile_Remove ("test");

	ComDebugTx("Test_Remove second PdkFile_Remove iRet = %d\n", iRet);


	iRet = PdkFile_Exist ("test");
	ComDebugTx("Test_Remove PdkFile_Exist test iRet = %d\n", iRet);

	iFd = PdkFile_Open ("Test_Remove");
	ComDebugTx("Test_Write_exist PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("Test_Write_exist PdkFile_Open fail\n");
		return iFd;
	}
	iRet = PdkFile_Close (iFd);

	iRet = PdkFile_Exist ("Test_Remove");
	ComDebugTx("Test_Remove PdkFile_Exist Test_Remove iRet = %d\n", iRet);
	PDK_ReadFileInfo();
	return 0;

}

int Test_Write_Again(void)
{
	int iFd = 0;
	int iWriteLen = 0;

	iFd = PdkFile_Open ("testtwo");
	ComDebugTx("Test_Write_Again testtwo PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("Test_Write_Again testtwo PdkFile_Open fail\n");
		return iFd;
	}

	iWriteLen = PdkFile_Write (iFd, "huang wu pingawe1 12e21", 40);
	ComDebugTx("Test_Write_Again first call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Write_Again first call PdkFile_Write fail\n");
	}
	PdkFile_Close (iFd);

	iFd = PdkFile_Open ("testthree");
	ComDebugTx("Test_Write_Again PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("Test_Write_Again PdkFile_Open fail\n");
		return iFd;
	}

	iWriteLen = PdkFile_Write (iFd, "12412qweda sdwawrfardQEfS", 1241);
	ComDebugTx("Test_Write_Again second call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Write_Again second call PdkFile_Write fail\n");
	}
	PdkFile_Close (iFd);
	return 0;
}
int Test_Read(void)
{
	int iFd = 0;
	int iReadLen = 0;
	char BufOut[2000] = {0};

	iFd = PdkFile_Open ("testthree");
	ComDebugTx("Test_Read PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("Test_Read PdkFile_Open fail\n");
		return iFd;
	}

	iReadLen = PdkFile_Read(iFd, BufOut, sizeof(BufOut));
	ComDebugTx("Test_Read second call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Read second call PdkFile_Read BufOut = %s\n", BufOut);
	PdkFile_Close(iFd);
	PDK_ReadFileInfo();
}

int Test_Bug()
{
	uchar BufOut[2000] = {0};
	uchar BufIn[2000] = {0};
	int i = 0;
	int iFd = 0;
	int iWriteLen = 0;
	int GetSize = 0;
	int iRet = 0;
	int iReadLen = 0;

	iFd = PdkFile_Open ("getsize");
	ComDebugTx("getsize PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("getsize PdkFile_Open fail\n");
		return iFd;
	}

	for(i = 0 ; i <= 620/4; i++)
	{
		memcpy(BufIn +i*4,"1234", 4);
	}

	iWriteLen = PdkFile_Write (iFd, BufIn, 620);
	ComDebugTx("Test_Bug first call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Bug first call PdkFile_Write fail\n");
	}
	GetSize = PdkFile_GetSize("getsize");
	ComDebugTx("Test_Bug PdkFile_GetSize first GetSize = %d\n", GetSize);

	memset(BufIn,0,620);
	for(i = 0; i < 620/4; i++)
	{
		memcpy(BufIn+ 4*i,"5678",4);
	}	


	iWriteLen = PdkFile_Write (iFd, BufIn, 620);
	ComDebugTx("Test_Bug second call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Bug second call PdkFile_Write fail\n");
	}
	GetSize = PdkFile_GetSize("getsize");
	ComDebugTx("Test_Bug PdkFile_GetSize second GetSize = %d\n", GetSize);

	iWriteLen = PdkFile_Write (iFd, BufIn, 620);
	ComDebugTx("Test_Bug third call PdkFile_Write iWriteLen = %d\n", iWriteLen);
	if(iWriteLen < 0)
	{
		ComDebugTx("Test_Bug thitd call PdkFile_Write fail\n");
	}
	GetSize = PdkFile_GetSize("getsize");
	ComDebugTx("Test_Bug PdkFile_GetSize third GetSize = %d\n", GetSize);

	iRet = PdkFile_Close (iFd);
	ComDebugTx("Test_Bug first PdkFile_Close iRet = %d\n", iRet);

	iFd = PdkFile_Open ("getsize");
	ComDebugTx("getsize second PdkFile_Open iFd = %d\n", iFd );
	if(iFd < 0)
	{
		ComDebugTx("getsize second PdkFile_Open fail\n");
		return iFd;
	}

	iReadLen = PdkFile_Read(iFd, BufOut, sizeof(BufOut));
	ComDebugTx("Test_Bug second call PdkFile_Read iReadLen = %d\n", iReadLen);
	if(iReadLen < 0)
	{
		return iReadLen;
	}
	ComDebugTx("Test_Bug second call PdkFile_Read BufOut = %s\n", BufOut);
	iRet = PdkFile_Close (iFd);
	ComDebugTx("Test_Bug seconde PdkFile_Close iRet = %d\n", iRet);

	return 0;
}
int Test_File(void)
{
	int iRet = 0;
	int Flag = 1;
	ST_TABLE_FS_INFO pstTblFsInfo;
	int i = 0;


	SysFileInit();
	//iRet = TableFsEraseAll ();
	//ComDebugTx("Test_File TableFsEraseAll iRet = %d\n", iRet);
	memset(&pstTblFsInfo, 0, sizeof(pstTblFsInfo));
	iRet = TableFsGetInfo(&pstTblFsInfo);
	ComDebugTx("Test_File TableFsGetInfo pstTblFsInfo.usTbFsVer = %d\n", pstTblFsInfo.usTbFsVer);
	ComDebugTx("Test_File TableFsGetInfo pstTblFsInfo.usBlkLen = %d\n", pstTblFsInfo.usBlkLen);
	ComDebugTx("Test_File TableFsGetInfo pstTblFsInfo.usMaxIdx = %d\n", pstTblFsInfo.usMaxIdx);

	/*
	for(i = 0; i <= pstTblFsInfo.usMaxIdx; i++)
	{
	iRet = TableFsErase (i);
	if(iRet < 0)
	{
	ComDebugTx("Test_File TableFsErase[%d] iRet = %d\n", i, iRet);
	}
	}
	*/

	while(Flag)
	{
		ComDebugTx("Test_File ==================================================================================\n");
		//PDK_ReadFileInfo();
		iRet = Test_Write_not_exist();
		ComDebugTx("Test_File Test_Write_not_exist iRet = %d\n", iRet);

		//PDK_ReadFileInfo();

		iRet = Test_Write_exist();
		ComDebugTx("Test_File Test_Write_exist iRet = %d\n", iRet);

		Test_Exist();

		//Test_GetSize();

		//Test_Trunk();

		Test_Remove();
		//ComDebugTx("Test_File //////////////////////////////////////////////////////////////////////////////////\n");
		//PDK_ReadFileInfo();
		Test_Write_Again();
		//PDK_ReadFileInfo();
		Test_Read();
		//Test_Bug();
		//PDK_ReadFileInfo();

		Flag= 0;

		if(getkey() == KEYENTER)
		{
			Flag = 1;
		}
	}



	return 0;

}
#endif
