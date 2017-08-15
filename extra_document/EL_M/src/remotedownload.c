/*------------------------------------------------------------
* FileName: remotedownload.c
* Author:	ZhangYuan
* Date:		2016-08-30
------------------------------------------------------------*/

#include "..\inc\global.h"
#include "..\inc\remotedownload.h"
#include "..\inc\file.h"


#define LOG_TAG	__FILE__
#ifdef MPOS_PROTIMS_DEBUG		
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else
#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)

#endif // MPOS_PROTIMS_DEBUG



#define EASYLINK_FILE

#define MPOS_PROTIMS_TASK_LIST_FILE	"TaskList.tmp"

TASK_TABLE		g_stTaskTable[TMS_TASK_NUM_MAX] = {{0}};
static uchar	sg_ucTaskFlag = 0x00;	// 0x00: task list not download, 0x01: task list downloaded
extern uchar GetFileType(const char *pszFileName);

int GetTermSN(unsigned char *pucRspDataOut, int *piLenOut)
{
	unsigned char aucSn[32+1];
	int iSnLen=0;

	memset(aucSn, 0, sizeof(aucSn));
	ReadSN(aucSn);

	iSnLen = strlen((char *)aucSn);
	pucRspDataOut[0] = iSnLen;
	memcpy(pucRspDataOut+1, aucSn, iSnLen);
	*piLenOut = iSnLen + 1;

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TermSN: ", pucRspDataOut, *piLenOut);

	return MPOS_STATUS_OK;
}

int GetTermExtSN(unsigned char *pucRspDataOut, int *piLenOut)
{
	unsigned char aucExtSn[32+1];
	int iExtSnLen=0;

	memset(aucExtSn, 0, sizeof(aucExtSn));
	EXReadSN(aucExtSn);

	iExtSnLen = strlen((char *)aucExtSn);
	pucRspDataOut[0] = iExtSnLen;
	memcpy(pucRspDataOut+1, aucExtSn, iExtSnLen);
	*piLenOut = iExtSnLen+1;

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetTermExtSN: ", pucRspDataOut, *piLenOut);

	return MPOS_STATUS_OK;
}

int GetTermVerInfo(unsigned char *pucRspDataOut, int *piLenOut)
{
	unsigned char aucVerInfo[32+1], ucRet=0x00;

	memset(aucVerInfo, 0, sizeof(aucVerInfo));
	ucRet = ReadVerInfo(pucRspDataOut);
	*piLenOut = 8;

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetTermVerInfo: ", pucRspDataOut, *piLenOut);

	return MPOS_STATUS_OK;
}

int GetTerminalInfo(unsigned char *pucRspDataOut, int *piLenOut)
{
	unsigned char aucTermInfo[64+1];

	memset(aucTermInfo, 0, sizeof(aucTermInfo));
	*piLenOut = GetTermInfo(aucTermInfo);

	memcpy(pucRspDataOut+1, aucTermInfo, *piLenOut);

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetTerminalInfo: ", pucRspDataOut, *piLenOut);

	return MPOS_STATUS_OK;
}

static void sv_CheckIfParserNeeded(unsigned char *pucFileName)
{
	int iRet=0;
	unsigned char ucFileType=0x00;

	ucFileType = GetFileType((char *)pucFileName);
	switch(ucFileType)
	{
	case PARAM_FILE_EMV:
		iRet = ParseEmvFile(pucFileName);
		if(iRet < 0)
		{
			ScrCls();
			ScrPrint(0,3,CFONT,"EMV FILE ERROR");
			DelayMs(300);
		}
		break;

	case PARAM_FILE_CLSS:
		iRet = ParseClssFile(pucFileName);
		if(iRet < 0)
		{
			ScrCls();
			ScrPrint(0,3,CFONT,"CLSS FILE ERROR");
			DelayMs(300);
		}
		break;

	default:
		break;
	}
}

static void sv_CheckIfRebootNeeded()
{
	int iRet=0, iFd=0, i=0, iOffset=0;
	unsigned short	usTaskNum=0, usDownloadedTaskNum=0;
	unsigned char	ucRebootFlag=0x00;
	TASK_TABLE stTaskInfo;

	iFd = PdkFile_Open(MPOS_PROTIMS_TASK_LIST_FILE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open = %d\n", iFd);
	if(iFd < 0)
	{
		return;
	}

	iRet = PdkFile_Read(iFd, (unsigned char *)&usTaskNum, 2);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usTaskNum=%d, iOffset=%d\n", iRet, usTaskNum, iOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return ;
	}
	iOffset += 2;

	iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iOffset=%d\n", iRet, iOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return;
	}
	iRet = PdkFile_Read(iFd, (unsigned char *)&usDownloadedTaskNum, 2);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usDownloadedTaskNum=%d, iOffset=%d\n", iRet, usDownloadedTaskNum, iOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return;
	}
	iOffset += 2;

	iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iOffset=%d\n", iRet, iOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return ;
	}

	for(i=0; i<usDownloadedTaskNum; i++)
	{
		memset(&stTaskInfo, 0, sizeof(TASK_TABLE));
		iRet = PdkFile_Read(iFd, (unsigned char *)&stTaskInfo, sizeof(TASK_TABLE));
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, stTaskInfo.ucTaskType=%02x, stTaskInfo.ucDone=%02x, iOffset=%d\n", iRet, stTaskInfo.ucTaskType, stTaskInfo.ucDone, iOffset);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			return ;
		}
		if (stTaskInfo.ucDone == PROTIMS_UPDATED_STATUS)			
		{
			ucRebootFlag = ucRebootFlag | 0x0F;
		}

		if(stTaskInfo.ucTaskType ==  PROTIMS_DOWNLOAD_MONITOR
			|| stTaskInfo.ucTaskType ==  PROTIMS_DOWNLOAD_APPLICATION)
		{
			ucRebootFlag = ucRebootFlag | 0xF0;
		}

		iOffset += sizeof(TASK_TABLE);
		iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iOffset=%d\n", iRet, iOffset);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			return ;
		}
	}

	PdkFile_Close(iFd);

	if(ucRebootFlag == 0xFF)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "reboot...\n");
		g_ucRebootFlag = 0x01;
	}
}

static int sn_UpdateTaskInfo(TASK_TABLE stTaskInfo)
{
	int iRet=0, iFd=0, i=0, iOffset=0, iFileSize=0;
	unsigned short usTaskNum=0, usDownloadedTaskNum=0; //, usTotalTaskNum=0;
	TASK_TABLE stTaskInfoSave;

	iFd = PdkFile_Open(MPOS_PROTIMS_TASK_LIST_FILE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open = %d, FileName: %s\n", iFd, stTaskInfo.aucFileName);
	if(iFd < 0)
	{
		return 0;
	}

	iFileSize = PdkFile_GetSize(MPOS_PROTIMS_TASK_LIST_FILE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_GetSize = %d\n", iFileSize);
	if( iFileSize < 0)
	{
		return 0;
	}

	iRet = PdkFile_Read(iFd, (unsigned char *)&usTaskNum, 2);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read = %d, usTaskNum=%d, iOffset=%d\n", iRet, usTaskNum, iOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return 0;
	}
	iOffset += 2;

	iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return 0;
	}
	iRet = PdkFile_Read(iFd, (unsigned char *)&usDownloadedTaskNum, 2);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read = %d, usDownloadedTaskNum=%d, iOffset=%d\n", iRet, usDownloadedTaskNum, iOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return 0;
	}
	iOffset += 2;

	iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return 0;
	}

	for(i=0; i<usDownloadedTaskNum && iOffset < iFileSize; i++)
	{
		memset(&stTaskInfoSave, 0, sizeof(TASK_TABLE));
		iRet = PdkFile_Read(iFd, (unsigned char *)&stTaskInfoSave, sizeof(TASK_TABLE));
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read = %d, stTaskInfoSave.FileName=%s, iOffset=%d\n", iRet, stTaskInfoSave.aucFileName, iOffset);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			return 0;
		}
		if (stTaskInfoSave.ucTaskNo == stTaskInfo.ucTaskNo )
		{
			iRet = PdkFile_Write(iFd, (unsigned char *)&stTaskInfo, sizeof(TASK_TABLE));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write = %d, stTaskInfo.FileName=%s, iOffset=%d\n", iRet, stTaskInfo.aucFileName, iOffset);
			PdkFile_Close(iFd);
			return 1;
		}

		iOffset += sizeof(TASK_TABLE);
		iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek = %d, iOffset=%d\n", iRet, iOffset);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			return 0;
		}
	}

	iRet = PdkFile_Close(iFd);

	return 0;
}

//已经下载过，函数返回1；没有下载过，函数返回0；
static int sn_CompareTaskInfo(int iFd, TASK_TABLE stTaskInfo)
{
	int iRet=0,/* iFd=0,*/ i=0, iOffset=0, iFileSize=0;
	unsigned short usTaskNum=0, usDownloadedTaskNum=0; //, usTotalTaskNum=0;
	TASK_TABLE stTaskInfoSave;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stTaskInfo.FileName=%s\n", stTaskInfo.aucFileName);

	/*iFd = PdkFile_Open(MPOS_PROTIMS_TASK_LIST_FILE);
	#ifdef MPOS_PROTIMS_DEBUG
	AppLog("[sn_CompareTaskInfo] PdkFile_Open=%d\n", iFd);
	#endif
	if(iFd < 0)
	{
	return 0;
	}*/

	iFileSize = PdkFile_GetSize(MPOS_PROTIMS_TASK_LIST_FILE);
	if( iFileSize < 0)
	{
		return 0;
	}

	iRet = PdkFile_Read(iFd, (unsigned char *)&usTaskNum, 2);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usTaskNum=%d, iOffset=%d\n", iRet, usTaskNum, iOffset);
	if(iRet < 0)
	{
		//iRet = PdkFile_Close(iFd);
		return 0;
	}
	iOffset += 2;

	iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iOffset=%d\n", iRet, iOffset);
	if(iRet < 0)
	{
		//iRet = PdkFile_Close(iFd);
		return 0;
	}
	iRet = PdkFile_Read(iFd, (unsigned char *)&usDownloadedTaskNum, 2);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usDownloadedTaskNum=%d, iOffset=%d\n", iRet, usDownloadedTaskNum, iOffset);
	if(iRet < 0)
	{
		//iRet = PdkFile_Close(iFd);
		return 0;
	}
	iOffset += 2;

	iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iOffset=%d\n", iRet, iOffset);
	if(iRet < 0)
	{
		//iRet = PdkFile_Close(iFd);
		return 0;
	}

	for(i=0; i<usDownloadedTaskNum && iOffset < iFileSize; i++)
	{
		memset(&stTaskInfoSave, 0, sizeof(TASK_TABLE));
		iRet = PdkFile_Read(iFd, (unsigned char *)&stTaskInfoSave, sizeof(TASK_TABLE));
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, stTaskInfoSave.FileName=%s, iOffset=%d\n", iRet, stTaskInfoSave.aucFileName, iOffset);
		if(iRet < 0)
		{
			return 0;
		}
		if (stTaskInfoSave.ucTaskType == stTaskInfo.ucTaskType 
			&& (memcmp(stTaskInfoSave.aucFileName, stTaskInfo.aucFileName, sizeof(stTaskInfo.aucFileName)) == 0 || 
			memcmp(stTaskInfoSave.aucAppName, stTaskInfo.aucAppName, sizeof(stTaskInfo.aucAppName)) == 0 ) )
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Same Task\n");
			//iRet = PdkFile_Close(iFd);
			return 1;
		}

		iOffset += sizeof(TASK_TABLE);
		iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iOffset=%d\n", iRet, iOffset);
		if(iRet < 0)
		{
			//iRet = PdkFile_Close(iFd);
			return 0;
		}
	}

	//iRet = PdkFile_Close(iFd);

	return 0;
}

int RemoteDownload_TaskInfo(const unsigned char *pucReqData, unsigned char *pucRspData, int *piRspLen)
{
	int iRet=MPOS_STATUS_OK, iRetValue=0, i=0, iFd=0, iFileOffset=0;
	unsigned short	usTaskNum=0, usTaskNumInFile=0, usCurTaskNum=0, usDownloadedTaskNum=0, usLen=0;
	unsigned int	uiOffset=0;

	*piRspLen = 0;

	if(pucReqData == NULL)
	{
		return MPOS_STATUS_NOTASKLIST;
	}

	usTaskNum = pucReqData[0] * 256 + pucReqData[1];	// 获得要下载的所有任务数，一个任务对应一个文件 
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usTaskNum=%d\n", usTaskNum);
	if (usTaskNum > TMS_TASK_NUM_MAX )//10)
	{
		return MPOS_STATUS_TASK_NUM_OVERFLOW; //下载的任务数过多
	}

	uiOffset += 2;

	iFd = PdkFile_Open(MPOS_PROTIMS_TASK_LIST_FILE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open=%d, FileName=%s\n", iFd, MPOS_PROTIMS_TASK_LIST_FILE);
	if(iFd < 0)
	{
		return MPOS_STATUS_SYS_ERR; 
	}
	/*iRet = PdkFile_Seek(iFd, 0, SEEK_SET);
	#ifdef MPOS_PROTIMS_DEBUG
	AppLog("[RemoteDownload_TaskInfo] PdkFile_Seek=%d\n", iRet);
	#endif
	if(iRet < 0)
	{
	iRet = PdkFile_Close(iFd);
	return MPOS_STATUS_SYS_ERR; 
	}*/
	iRet = PdkFile_Read(iFd, (unsigned char *)&usTaskNumInFile, sizeof(usTaskNumInFile));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usTaskNumInFile=%d\n", iRet, usTaskNumInFile);
	/*if(iRet < 0)
	{
	iRet = PdkFile_Close(iFd);
	return MPOS_STATUS_SYS_ERR; 
	}*/

	if(usTaskNumInFile != usTaskNum)	// new downloading...
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usTaskNumInFile=%d, usTaskNum=%d\n", usTaskNumInFile, usTaskNum);
		PdkFile_Truncate(iFd, 0);
		PdkFile_Seek(iFd, 0, SEEK_SET);
		PdkFile_Write(iFd, "\x00\x00\x00\x00", 4);
	}

	iRet = PdkFile_Seek(iFd, 0, SEEK_SET);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d\n", iRet);
	if(iRet < 0)
	{
		iRet = PdkFile_Close(iFd);
		return MPOS_STATUS_SYS_ERR; 
	}
	iRet = PdkFile_Write(iFd, (unsigned char *)&usTaskNum, sizeof(usTaskNum));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write=%d, usTaskNum=%d\n", iRet, usTaskNum);
	if(iRet < 0)
	{
		iRet = PdkFile_Close(iFd);
		return MPOS_STATUS_SYS_ERR; 
	}

	/*iRet = PdkFile_Seek(iFd, 2, SEEK_SET);
	if(iRet < 0)
	{
	iRet = PdkFile_Close(iFd);
	return MPOS_STATUS_SYS_ERR; 
	}*/
	iRet = PdkFile_Read(iFd, (unsigned char *)&usDownloadedTaskNum, sizeof(usDownloadedTaskNum));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write=%d, usDownloadedTaskNum=%d\n", iRet, usDownloadedTaskNum);
	if(iRet < 0)
	{
		iRet = PdkFile_Close(iFd);
		return MPOS_STATUS_SYS_ERR; 
	}

	usCurTaskNum = pucReqData[uiOffset] * 256 + pucReqData[uiOffset + 1];      //当前数据包所包含的任务
	uiOffset += 2;
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usCurTaskNum=%d, uiOffset=%d\n", usCurTaskNum, uiOffset);
	iFileOffset += 4;
	for(i=0; i<MIN(usCurTaskNum, TMS_TASK_NUM_MAX); i++)	// parse task table
	{
		g_stTaskTable[i].ucTaskType = pucReqData[uiOffset];	// Task Type
		uiOffset += 1;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stTaskTable[%d].ucTaskType=%02x, uiOffset=%d\n", i, g_stTaskTable[i].ucTaskType, uiOffset);

		g_stTaskTable[i].ucTaskNo = pucReqData[uiOffset];	// Task No.
		uiOffset += 1;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stTaskTable[%d].ucTaskNo=%02x, uiOffset=%d\n", i, g_stTaskTable[i].ucTaskNo, uiOffset);

		usLen = pucReqData[uiOffset] * 256 + pucReqData[uiOffset + 1];		// Length of file name
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usLen=%d, [%02x %02x]\n", usLen, pucReqData[uiOffset], pucReqData[uiOffset+1]);
		uiOffset += 2;

		memcpy(g_stTaskTable[i].aucFileName, pucReqData + uiOffset, usLen);	// File name
		uiOffset += usLen;	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stTaskTable[%d].aucFileName=%s, uiOffset=%d\n", i, g_stTaskTable[i].aucFileName, uiOffset);

		usLen = pucReqData[uiOffset] * 256 + pucReqData[uiOffset+1];		// Length of App name
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usLen=%d, [%02x %02x]\n", usLen, pucReqData[uiOffset], pucReqData[uiOffset+1]);
		uiOffset += 2;

		memcpy(g_stTaskTable[i].aucAppName, pucReqData + uiOffset, usLen);	// App Name
		uiOffset += usLen;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stTaskTable[%d].aucAppName=%s, uiOffset=%d\n", i, g_stTaskTable[i].aucAppName, uiOffset);

		usLen = pucReqData[uiOffset] * 256 + pucReqData[uiOffset+1];		// Length of version
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usLen=%d, [%02x %02x]\n", usLen, pucReqData[uiOffset], pucReqData[uiOffset+1]);
		uiOffset += 2;

		memcpy(g_stTaskTable[i].aucVersion, pucReqData + uiOffset, usLen);	// Version
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usLen=%d, [%02x %02x]\n", usLen, pucReqData[uiOffset], pucReqData[uiOffset+1]);
		uiOffset += usLen;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stTaskTable[%d].aucVersion=%s, uiOffset=%d\n", i, g_stTaskTable[i].aucVersion, uiOffset);

		g_stTaskTable[i].ucForceUpdate = pucReqData[uiOffset];					
		uiOffset += 1;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stTaskTable[%d].ucForceUpdate=%02x, uiOffset=%d\n", i, g_stTaskTable[i].ucForceUpdate, uiOffset);

		iRetValue = sn_CompareTaskInfo(iFd, g_stTaskTable[i]);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sn_CompareTaskInfo=%d\n", iRetValue);
		if(iRetValue == 1)
		{
			g_stTaskTable[i].ucDone = PROTIMS_UPDATED_STATUS;
			continue;
		}

		iRet = PdkFile_Seek(iFd, iFileOffset, SEEK_SET);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iFileOffset=%d\n", iRet, iFileOffset);
		if(iRet < 0)
		{
			iRet = PdkFile_Close(iFd);
			return MPOS_STATUS_SYS_ERR; 
		}
		iRet = PdkFile_Write(iFd, (unsigned char *)&g_stTaskTable[i], sizeof(TASK_TABLE));
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write=%d, g_stTaskTable[%d], FileName=%s\n", iRet, i, g_stTaskTable[i].aucFileName);
		if(iRet < 0)
		{
			iRet = PdkFile_Close(iFd);
			return MPOS_STATUS_SYS_ERR; 
		}

		iFileOffset += sizeof(TASK_TABLE);
	}
	usDownloadedTaskNum += MIN(usCurTaskNum, TMS_TASK_NUM_MAX);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usDownloadedTaskNum=%d\n", usDownloadedTaskNum);
	iRet = PdkFile_Seek(iFd, 2, SEEK_SET);
	if(iRet < 0)
	{
		iRet = PdkFile_Close(iFd);
		return MPOS_STATUS_SYS_ERR; 
	}
	iRet = PdkFile_Write(iFd, (unsigned char *)&usDownloadedTaskNum, sizeof(usDownloadedTaskNum));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write=%d, usDownloadedTaskNum=%d\n", iRet, usDownloadedTaskNum);
	if(iRet < 0)
	{
		iRet = PdkFile_Close(iFd);
		return MPOS_STATUS_SYS_ERR; 
	}

	PdkFile_Close(iFd);
	sg_ucTaskFlag = 0x01;

	return MPOS_STATUS_OK;
}

int GetTaskInfoFromFile(unsigned char ucTaskNo, TASK_TABLE *pstTaskInfo)
{
	int iRet=0, iFd=0, i=0, iIdx=0, iOffset=0;
	unsigned short	usTaskNum=0, usDownloadedTaskNum=0;

	// load task list info from file
	iFd = PdkFile_Open(MPOS_PROTIMS_TASK_LIST_FILE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open=%d, FileName=%s\n", iFd, MPOS_PROTIMS_TASK_LIST_FILE);
	if(iFd < 0)
	{
		return -1;
	}

	iRet = PdkFile_Read(iFd, (unsigned char *)&usTaskNum, sizeof(usTaskNum));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usTaskNum=%d, iOffset=%d\n", iRet, usTaskNum, iOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return -1;
	}	

	iOffset += 4;
	for(i=0; i<=usTaskNum/TMS_TASK_NUM_MAX; i++)
	{
		iRet = PdkFile_Seek(iFd, iOffset, SEEK_SET);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, iOffset=%d\n", iRet, iOffset);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			return -1;
		}

		memset(&g_stTaskTable, 0, sizeof(g_stTaskTable));
		iRet = PdkFile_Read(iFd, (unsigned char *)&g_stTaskTable, sizeof(g_stTaskTable));
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, g_stTaskTable\n", iRet);
		if(iFd < 0)
		{
			PdkFile_Close(iFd);
			return -1;
		}

		for(iIdx=0; iIdx<TMS_TASK_NUM_MAX; iIdx++)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_stTaskTable[%d].ucTaskNo=%02x, ucTaskNo=%02x\n", iIdx, g_stTaskTable[iIdx].ucTaskNo, ucTaskNo);
			if(g_stTaskTable[iIdx].ucTaskNo == ucTaskNo)
			{
				PdkFile_Close(iFd);
				memcpy(pstTaskInfo, &g_stTaskTable[iIdx], sizeof(TASK_TABLE));
				return iIdx;
			}
		}

		iOffset += sizeof(g_stTaskTable); 
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iOffset=%d\n", iOffset);
	}

	PdkFile_Close(iFd);
	return -1;
}

// update firmware, app, font 
/*
[input] unsigned char ucObject: 0x00-Firmware & APP, 0x01-FONT
[input] unsigned char *pucData: data buffer
[input] unsigned int uiDataLen: length of data buffer
*/
static int sn_UpdateFirmware(const unsigned char ucTaskType, const REMOTE_LOAD_STEP eRemoteLoadStep, unsigned int uiTotalSize, const unsigned char *pucData, unsigned int uiDataLen)
{
	int iRet=0;
	unsigned char ucObject = 0x00;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucTaskType=%02x, eRemoteLoadStep=%d, uiDataLen=%d, uiTotalSize=%d\n", ucTaskType, eRemoteLoadStep, uiDataLen, uiTotalSize);
	switch(ucTaskType)
	{
	case PROTIMS_DOWNLOAD_APPLICATION:
		ucObject = 0x00;
		break;

	case PROTIMS_DOWNLOAD_FONT: 
		ucObject = 0x01;
		break;

	case PROTIMS_DOWNLOAD_MONITOR:		// modified by ZhangYuan, if download monitor by RemoteLoad, the object should be 0x02
		ucObject = 0x02;
		break;

	default:
		return -1;
	}

	if(eRemoteLoadStep == REMOTE_LOAD_BEGIN)
	{
		iRet = RemoteLoad(ucObject, REMOTE_LOAD_BEGIN, NULL, uiTotalSize);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Begin, RemoteLoad=%d, ucObject=%02x\n", iRet, ucObject);
		if(iRet != 0)
		{
			return iRet;
		}
	}

	iRet = RemoteLoad(ucObject, REMOTE_LOAD_WRITE, (unsigned char *)pucData, uiDataLen);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Write, RemoteLoad=%d, ucObject=%02x\n", iRet, ucObject);
	if(iRet != 0)
	{
		return iRet;
	}

	if(eRemoteLoadStep == REMOTE_LOAD_END)
	{
		iRet = RemoteLoad(ucObject, REMOTE_LOAD_END, NULL, 0);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "End, RemoteLoad=%d, ucObject=%02x\n", iRet, ucObject);
		if(iRet != 0)
		{
			return iRet;
		}
	}

	return 0;
}

static int sn_UpdateParamFile(const TASK_TABLE stTaskInfo, const unsigned int uiFileOffset, const unsigned char *pucData, unsigned int uiDataLen)
{
	int iRet=0, iFd=0;

	if(uiFileOffset == 0 && PdkFile_Exist((char *)stTaskInfo.aucFileName) == PDK_RET_OK)	// overwrite
	{
		PdkFile_Remove((char *)stTaskInfo.aucFileName);
	}

	iFd = PdkFile_Open((char *)stTaskInfo.aucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open=%d, aucFileName=%s\n", iFd, stTaskInfo.aucFileName);
	if(iFd < 0)
	{
		return iFd;
	}

	iRet = PdkFile_Seek(iFd, uiFileOffset, SEEK_SET);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Seek=%d, uiFileOffset=%d\n", iRet, uiFileOffset);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return iRet;
	}

	iRet = PdkFile_Write(iFd, pucData, uiDataLen);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write=%d, uiDataLen=%d\n", iRet, uiDataLen);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return iRet;
	}

	iRet = PdkFile_Close(iFd);
	if(iRet < 0)
	{
		return iRet;
	}

	return 0;
}

static int sn_DeleteParamFile(unsigned char *pucFileName)
{
	int iRet=0;

	iRet = PdkFile_Remove((char *)pucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Remove=%d, pucFileName=%s\n", iRet, pucFileName);
	return iRet;
}

#ifdef MPOS_PROTIMS_DEBUG
static void sv_ReadFile(TASK_TABLE stTaskInfo)
{
	int iRet=0, iFd=0, iFileSize=0, iOffset=0, iReadLen=0;
	unsigned char aucBuff[800+1];

	iFd = PdkFile_Open((char *)stTaskInfo.aucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open=%d, FileName=%s\n", iFd, stTaskInfo.aucFileName);
	if(iFd < 0)
	{
		return;
	}

	iFileSize = PdkFile_GetSize((char *)stTaskInfo.aucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_GetSize=%d, iFileSize=%d, iReadLen=%d\n", iRet, iFileSize);
	while(iOffset < iFileSize)
	{
		if(iFileSize - iOffset > 800)
		{
			iReadLen = 800;
		}
		else
		{
			iReadLen = iFileSize - iOffset;
		}

		memset(aucBuff, 0, sizeof(aucBuff));
		iRet = PdkFile_Read(iFd, aucBuff, iReadLen);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, iOffset=%d, iReadLen=%d\n", iRet, iOffset, iReadLen);
			PdkFile_Close(iFd);
			return;
		}

		AppStrLog("%s", aucBuff);

		iOffset += iRet;
	}

	PdkFile_Close(iFd);
}
#endif

int RemoteDownload_SaveData(const unsigned char *pucReqData, unsigned char *pucRspData, int *piRspLen)
{
	int iRet=0, iFd=0, iOffset=0, iFileOffset=0, iFileSize=0, iCurSize=0;
	TASK_TABLE stTaskInfo;
	REMOTE_LOAD_STEP eRemoteLoadStep = REMOTE_LOAD_BEGIN;

	*piRspLen = 0; 

	if(sg_ucTaskFlag == 0x00)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sg_ucTaskFlag=%02x\n", sg_ucTaskFlag);
		return MPOS_STATUS_FORGET_SET_TASK;
	}

	memset(&stTaskInfo, 0, sizeof(TASK_TABLE));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "[SaveData] ", (unsigned char *)pucReqData, 13);
	iRet = GetTaskInfoFromFile(pucReqData[0], &stTaskInfo);	// pucReqData[0] is FileNo
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetTaskInfoFromFile=%d, ucTaskNo=%02x, ucTaskType=%02x\n", iRet, pucReqData[0], stTaskInfo.ucTaskType);
	if(iRet < 0)
	{
		return MPOS_STATUS_FILENO_NOT_SAME;		// 没有匹配到相同的TaskNo
	}
	iOffset += 1;

	iFileSize = (pucReqData[iOffset] << 24) + (pucReqData[iOffset+1] << 16) +  (pucReqData[iOffset+2] << 8) + pucReqData[iOffset+3];
	iOffset += 4;

	iFileOffset = (pucReqData[iOffset] << 24) + (pucReqData[iOffset+1] << 16) +  (pucReqData[iOffset+2] << 8) + pucReqData[iOffset+3];
	iOffset += 4;

	iCurSize = (pucReqData[iOffset] << 24) + (pucReqData[iOffset+1] << 16) +  (pucReqData[iOffset+2] << 8) + pucReqData[iOffset+3];
	iOffset += 4;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iFileSize=%d, iFileOffset=%d, iCurSize=%d\n", iFileSize, iFileOffset, iCurSize);
	if(iFileOffset == 0)
	{
		eRemoteLoadStep = REMOTE_LOAD_BEGIN;
	}
	else if(iFileSize == (iFileOffset + iCurSize))
	{
		eRemoteLoadStep = REMOTE_LOAD_END;
	}
	else
	{
		eRemoteLoadStep = REMOTE_LOAD_WRITE;
	}
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "eRemoteLoadStep=%d\n", eRemoteLoadStep);

	//	ScrPrint(0,2,CFONT,"%s:", stTaskInfo.aucFileName);
	PubDispString(stTaskInfo.aucFileName, 2|DISP_LINE_CENTER);
	ScrPrint(0,4,CFONT,"  Completed:%d%", (iFileOffset * 100)/iFileSize);


	switch(stTaskInfo.ucTaskType)
	{
	case PROTIMS_CREATE_FILE_SYSTEM: 
	case PROTIMS_WNET_FIRMWARE_UPDATE:
	case PROTIMS_DOWNLOAD_DLL:   
	case PROTIMS_DELLETE_DLL:           
	case PROTIMS_DOWNLOAD_USPUK:			 		
	case PROTIMS_DOWNLOAD_UAPUK:
	case PROTIMS_DELETE_APPLICATION:
	case PROTIMS_DELETE_ALL_APPLICATION:  
	case PROTIMS_DOWNLOAD_PUBFILE:		 		
	case PROTIMS_DELETE_PUBFILE:
		iRet = MPOS_STATUS_CMD_NOTSPT;
		break;

	case PROTIMS_DOWNLOAD_MONITOR:   
	case PROTIMS_DOWNLOAD_APPLICATION: 
	case PROTIMS_DOWNLOAD_FONT: 
		iRet = sn_UpdateFirmware(stTaskInfo.ucTaskType, eRemoteLoadStep, iFileSize, pucReqData + iOffset, iCurSize);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sn_UpdateFirmware=%d, iOffset=%d\n", iRet, iOffset);
		if(iRet != 0)
		{
			return MPOS_STATUS_FIRMWARE_SAVE_ERR;
		}	
		break;

	case PROTIMS_DOWNLOAD_PARA_FILE:
		iRet = sn_UpdateParamFile(stTaskInfo, iFileOffset, pucReqData + iOffset, iCurSize);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sn_UpdateParamFile=%d, FileName=%s, iFileOffset=%d, iCurSize=%d\n", iRet, stTaskInfo.aucFileName, iFileOffset, iCurSize);
		if(iRet != 0)
		{
			return MPOS_STATUS_PARA_SAVE_ERR;
		}
		break;

	case PROTIMS_DELETE_PARA_FILE:     
		iRet = sn_DeleteParamFile(stTaskInfo.aucFileName);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sn_DeleteParamFile=%d, FileName=%s\n", iRet, stTaskInfo.aucFileName);
		if(iRet != 0)
		{
			return MPOS_STATUS_PARA_SAVE_ERR;
		}
		break;

	default:
		return MPOS_STATUS_CMD_NOTSPT;		
	}

	if(eRemoteLoadStep == REMOTE_LOAD_END)
	{
		stTaskInfo.ucDone = PROTIMS_UPDATED_STATUS;
		iRet = sn_UpdateTaskInfo(stTaskInfo);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sn_UpdateTaskInfo=%d, FileName=%s\n", iRet, stTaskInfo.aucFileName);
		//sv_ReadFile(stTaskInfo);

		sv_CheckIfRebootNeeded();
		sv_CheckIfParserNeeded(stTaskInfo.aucFileName);	// Added by ZhangYuan 2016-11-08

		PubDispString(stTaskInfo.aucFileName, 2|DISP_LINE_CENTER);
		ScrPrint(0,4,CFONT,"  Completed:100%");
		Beep();
		DelayMs(150);
		ScrCls();
		g_IdeUi = 0x00;
	}		

	return iRet;
}

int RemoteDownload_TaskAsk(const unsigned char *pucReqData, unsigned char *pucRspData, int *piRspLen, unsigned char *pucSendRspFlag)
{
	int iRet=0, iFd=0, iFileOffset=0, iRspOffset=0, i=0, iSend=0;
	unsigned short usTaskNum=0, usDownloadedTask=0, usCurTaskNum=0, usTmp=0;
	TASK_TABLE stTaskInfo;

	*piRspLen = 0;

	iFd = PdkFile_Open(MPOS_PROTIMS_TASK_LIST_FILE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open=%d, FileName=%s\n", iFd, MPOS_PROTIMS_TASK_LIST_FILE);
	if(iFd < 0)
	{
		return MPOS_STATUS_SYS_ERR;
	}

	/*iFileOffset = 0;
	iRet = PdkFile_Seek(iFd, iFileOffset, SEEK_SET);
	#ifdef MPOS_PROTIMS_DEBUG
	AppLog("[RemoteDownload_TaskAsk] PdkFile_Seek=%d, iFileOffset=%d\n", iRet, iFileOffset);
	#endif
	if(iRet < 0)
	{
	PdkFile_Close(iFd);
	return MPOS_STATUS_NOTASKLIST;
	}*/

	iRet = PdkFile_Read(iFd, (unsigned char *)&usTaskNum, sizeof(usTaskNum));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usTaskNum=%d\n", iRet, usTaskNum);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return MPOS_STATUS_NOTASKLIST;
	}
	pucRspData[iRspOffset] = (usTaskNum >> 8)  & 0xff; 
	pucRspData[iRspOffset+1] = usTaskNum  & 0xff; 
	iRspOffset += 2;

	/*iFileOffset += 2;
	iRet = PdkFile_Seek(iFd, iFileOffset, SEEK_SET);
	#ifdef MPOS_PROTIMS_DEBUG
	AppLog("[RemoteDownload_TaskAsk] PdkFile_Seek=%d, iFileOffset=%d\n", iRet, iFileOffset);
	#endif
	if(iRet < 0)
	{
	PdkFile_Close(iFd);
	return MPOS_STATUS_NOTASKLIST;
	}*/
	iRet = PdkFile_Read(iFd, (unsigned char *)&usDownloadedTask, sizeof(usDownloadedTask));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, usDownloadedTask=%d\n", iRet, usDownloadedTask);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);		
		return MPOS_STATUS_NOTASKLIST;
	}
	iFileOffset += 2;

	iRspOffset += 2;
	for(i=0; i<=usDownloadedTask/TMS_TASK_NUM_MAX; i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "i=%d\n", i);
		while(iSend < usDownloadedTask)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iSend=%d, usDownloadedTask=%d\n", iSend, usDownloadedTask);
			/*iRet = PdkFile_Seek(iFd, iFileOffset, SEEK_SET);
			#ifdef MPOS_PROTIMS_DEBUG
			AppLog("[RemoteDownload_TaskAsk] PdkFile_Seek=%d, iFileOffset=%d\n", iRet, iFileOffset);
			#endif
			if(iRet < 0)
			{
			PdkFile_Close(iFd);
			return MPOS_STATUS_SYS_ERR;
			}*/
			memset(&stTaskInfo, 0, sizeof(TASK_TABLE));
			iRet = PdkFile_Read(iFd, (unsigned char *)&stTaskInfo, sizeof(TASK_TABLE));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d\n", iRet);
			/*if(iRet < 0)
			{
			PdkFile_Close(iFd);
			return MPOS_STATUS_SYS_ERR;
			}*/

			if(stTaskInfo.ucDone == PROTIMS_UPDATED_STATUS)	// only need to send the finished task info
			{
				pucRspData[iRspOffset] = stTaskInfo.ucTaskType;	// Task Type
				pucRspData[iRspOffset+1] = stTaskInfo.ucTaskNo;	// Task No.
				iRspOffset += 2;

				usTmp = strlen((char *)stTaskInfo.aucFileName);	// Length of File Name
				pucRspData[iRspOffset] = (usTmp >> 8)  & 0xff; 
				pucRspData[iRspOffset+1] = usTmp  & 0xff; 
				iRspOffset += 2;
				memcpy(pucRspData + iRspOffset, stTaskInfo.aucFileName, usTmp);	// File Name
				iRspOffset += usTmp;

				usTmp = strlen((char *)stTaskInfo.aucAppName);	// Length of APP Name
				pucRspData[iRspOffset] = (usTmp >> 8)  & 0xff; 
				pucRspData[iRspOffset+1] = usTmp  & 0xff; 
				iRspOffset += 2;
				memcpy(pucRspData + iRspOffset, stTaskInfo.aucAppName, usTmp);	// APP Name
				iRspOffset += usTmp;

				usTmp = strlen((char *)stTaskInfo.aucVersion);	// Length of Version
				pucRspData[iRspOffset] = (usTmp >> 8)  & 0xff; 
				pucRspData[iRspOffset+1] = usTmp  & 0xff; 
				iRspOffset += 2;
				memcpy(pucRspData + iRspOffset, stTaskInfo.aucVersion, usTmp);	// Version
				iRspOffset += usTmp;
			}		

			iFileOffset += sizeof(TASK_TABLE);

			iSend ++;
			usCurTaskNum ++;
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iSend=%d, usCurTaskNum=%d\n", iSend, usCurTaskNum);
			if(usCurTaskNum > TMS_TASK_NUM_MAX)
			{
				break;
			}
		}

		iRspOffset = 2;
		pucRspData[iRspOffset] = (usCurTaskNum >> 8)  & 0xff; 
		pucRspData[iRspOffset+1] = usCurTaskNum  & 0xff;
		iRspOffset += 2;
		usCurTaskNum = 0;

		// If all the file downloaded failed, then return no task list
		if ( 4 == iRspOffset )
		{
			PdkFile_Close(iFd);		
			*piRspLen = 0;
			iRspOffset = 0;
			iRet = MPOS_STATUS_NOTASKLIST;
		}
		else
		{
			iRet = MPOS_STATUS_OK;
		}

		*pucSendRspFlag = 0x01;

		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iRspOffset=%d, usCurTaskNum=%d\n", iRspOffset, usCurTaskNum);
		iRet = iSendResponse(pucRspData, iRspOffset, iRet, EL_CMD_REMOTEDOWNLOAD_TASKASK);		
	}

	// If all the file downloaded failed, then return no task list
	if ( 4 == iRspOffset )
	{
		PdkFile_Close(iFd);		
		*piRspLen = 0;
		return MPOS_STATUS_NOTASKLIST;
	}

	*piRspLen = iFileOffset;

	PdkFile_Close(iFd);		

	return iRet;
}







