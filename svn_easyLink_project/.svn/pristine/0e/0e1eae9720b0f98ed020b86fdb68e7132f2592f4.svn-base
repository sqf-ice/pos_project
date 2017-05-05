//============================================================================
// Name        : filedownload.c
// Author      :tuxq
// Version     :
// Copyright   : Your copyright notice
// Description : Ansi-style
//============================================================================
#include "..\inc\global.h"

#define LOG_TAG	__FILE__
#ifdef FILEDOWNLOAD_DEBUG		

#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr,...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)


#endif // FILEDOWNLOAD_DEBUG	

static uchar *RetToString(int ReturnCode)
{
	switch(ReturnCode)
	{
	case 0:
		return "Success";
	case FILEDOWNLOAD_ERR_INVALID_PARAM:
		return "Parameter Invalid";
//	case FILEDOWNLOAD_ERR_PARAFILE_FAIL:
//		return "Download Parameter File Failed";
//	case FILEDOWNLOAD_ERR_FIRMFILE_FAIL:
//		return "Download Firmware Failed";
	case FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL:
		return "Download Parameter File Failed";
	case FILEDOWNLOAD_ERR_FILE_OVERSIZE:
		return "File Oversize";
	case FILEDOWNLOAD_ERR_NOT_ALLOWED:
		return "File Not Allowed";
	default:
		return "Unkown Error";
	}
}


//read file info
static void sv_ReadFile(SFileInfo stTaskInfo)
{
	int iRet=0, iFd=0, iFileSize=0, iOffset=0, iReadLen=0;
	unsigned char aucBuff[800+1];

	iFd = PdkFile_Open((char *)stTaskInfo.ucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open=%d, FileName=%s\n", iFd, stTaskInfo.ucFileName);
	if(iFd < 0)
	{
		return;
	}

	iFileSize = PdkFile_GetSize((char *)stTaskInfo.ucFileName);
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
			EL_LOG(LOG_ERROR, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read=%d, iOffset=%d, iReadLen=%d\n", iRet, iOffset, iReadLen);
			PdkFile_Close(iFd);
			return;
		}

		AppStrLog("%s", aucBuff);

		iOffset += iRet;
	}

	PdkFile_Close(iFd);
}

static int DeleteFile(unsigned char *pucFileName)
{
	int iRet=0;

	iRet = PdkFile_Remove((char *)pucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Remove=%d, pucFileName=%s\n", iRet, pucFileName);
	return iRet;
}

uchar GetFileType(const char *pszFileName)
{
	if(strstr(pszFileName, ".ui") != NULL)
	{
		return PARAM_FILE_UI;
	}
	else if(strstr(pszFileName, ".emv") != NULL)
	{
		return PARAM_FILE_EMV;
	}
	else if(strstr(pszFileName, ".clss") != NULL)
	{
		return PARAM_FILE_CLSS;
	}
	else if(strstr(pszFileName, ".bmp") != NULL)
	{
		return PARAM_FILE_BMP;
	}
	else if(strstr(pszFileName, ".lng") != NULL)
	{
		return PARAM_FILE_LNG;
	}
	else if(strstr(pszFileName, ".bin") != NULL)		// modified by ZhangYuan 2016-11-01
	{
		return APP_FILE_M;
	}
	else if(strstr(pszFileName, ".monitor") != NULL)		// modified by ZhangYuan 2016-11-01
	{
		return MONITOR_FILE;
	}
	else if(strstr(pszFileName, ".font") != NULL)
	{
		return FONT_FILE;
	}
	//	return PARA_FILE;
	return FILE_TYPE_UNKNOWN;
}

static int CheckIfUpdateMonitorAllow()
{
	unsigned char aucVerInfo[32+1], ucRet=0x00;

	memset(aucVerInfo, 0, sizeof(aucVerInfo));
	ucRet = ReadVerInfo(aucVerInfo);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "aucVerInfo", aucVerInfo, 8);
	if(aucVerInfo[0] >= 32)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "aucVerInfo[0] >= 32\n");
		return 1;
	}

	return 0;
}

static int UpdateFirmware(const uchar ucTaskType, const REMOTE_LOAD_STEP eDownLoadStep, SFileInfo *sFileInfo)
{
	int iRet=0;
	uchar ucObject = 0x00;

	switch(ucTaskType)
	{
	case APP_FILE_M:
		if(sFileInfo->uiFileSize > 400*1024)
		{
			ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_FILE_OVERSIZE, FILEDOWNLOAD_ERR_FILE_OVERSIZE, __LINE__);
			return FILEDOWNLOAD_ERR_FILE_OVERSIZE;
		}
		ucObject = 0x00;
		break;

	case FONT_FILE: 
		if(sFileInfo->uiFileSize > 2048*1024)
		{
			ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_FILE_OVERSIZE, FILEDOWNLOAD_ERR_FILE_OVERSIZE, __LINE__);
			return FILEDOWNLOAD_ERR_FILE_OVERSIZE;
		}
		ucObject = 0x01;
		break;

	case MONITOR_FILE:		// modified by ZhangYuan, if download monitor by RemoteLoad, the object should be 0x02
		if(!CheckIfUpdateMonitorAllow())
		{
			ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_NOT_ALLOWED, FILEDOWNLOAD_ERR_NOT_ALLOWED, __LINE__);
			return FILEDOWNLOAD_ERR_NOT_ALLOWED;
		}
		if(sFileInfo->uiFileSize > 400*1024)
		{
			return FILEDOWNLOAD_ERR_FILE_OVERSIZE;
		}
		ucObject = 0x02;
		break;

	default:
		//			break;
		return FILEDOWNLOAD_ERR_INVALID_PARAM;	// modified by ZhangYuan 2016-11-01, if task type err, return parameter invalid
	}
	if(eDownLoadStep == REMOTE_LOAD_BEGIN){
		iRet = RemoteLoad(ucObject, REMOTE_LOAD_BEGIN, NULL, sFileInfo->uiFileSize);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "REMOTE_LOAD_BEGIN, RemoteLoad=%d, ucObject=%02x\n",iRet, ucObject);
		if(iRet != 0){
			ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL, iRet, __LINE__);
			return FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL;
		}
	}
	iRet = RemoteLoad(ucObject, REMOTE_LOAD_WRITE, sFileInfo->ucFileContent, sFileInfo->uiCurFileSize);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "REMOTE_LOAD_WRITE, RemoteLoad=%d, ucObject=%02x\n",iRet, ucObject);
	if(iRet != 0){
		ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL, iRet, __LINE__);
		return FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL;
	}

	if(eDownLoadStep == REMOTE_LOAD_END){
		iRet =RemoteLoad(ucObject, REMOTE_LOAD_END, NULL, 0);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "REMOTE_LOAD_END, RemoteLoad=%d, ucObject=%02x\n",iRet, ucObject);
		if(iRet != 0){
			ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL, iRet, __LINE__);
			return FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL;
		}

		g_ucRebootFlag = 0x01;	// Added by ZhangYuan 2016-10-27 reboot if download app/monitor/font success
	}
	return 0;
}

static int UpdateParamFile(const SFileInfo sFileInfo){
	int iFd = 0, iRet = 0;
	uint uiCurFileSize = 0;

	if(sFileInfo.uiFileSize > MAX_FILE_SIZE)
	{
		return FILEDOWNLOAD_ERR_FILE_OVERSIZE;
	}

	iRet = PdkFile_Exist((char*)sFileInfo.ucFileName);
	if(iRet == PDK_RET_OK && sFileInfo.uiFileOffset == 0){
		PdkFile_Remove((char*)sFileInfo.ucFileName);
	}

	//if file not exist,create one;if exist,open it
	iFd = PdkFile_Open ((char*)sFileInfo.ucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open = %d\n",iFd);
	if(iFd < 0){
		return iFd;
	}
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiCurFileSize = %d, sFileInfo.uiCurFileSize = %d, sFileInfo.uiFileSize = %d, sFileInfo.ucFileName = %s\n", uiCurFileSize, sFileInfo.uiCurFileSize, sFileInfo.uiFileSize, sFileInfo.ucFileName);
	iRet = PdkFile_Seek (iFd, sFileInfo.uiFileOffset, SEEK_SET);//relocate file
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		return iRet;
	}
	iRet = PdkFile_Write (iFd, sFileInfo.ucFileContent, sFileInfo.uiCurFileSize);//byte
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write iRet = %d\n",iRet);
	if(iRet < 0){
		PdkFile_Close(iFd);
		return iRet;
	}

	PdkFile_Close(iFd);

	return 0;
}

int  DownloadFile(const uchar *pucRequest)
{
	int iRet = 0;
	SFileInfo sFileInfo;
	uchar		ucFileType = 0x00;
	uint uiReqOffset = 0, uiLength = 0;
	REMOTE_LOAD_STEP eDownLoadStep = REMOTE_LOAD_BEGIN;

	memset(&sFileInfo, 0, sizeof(SFileInfo));

	uiLength = (uint)pucRequest[0]<<8 | pucRequest[1];
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiLength = %d\n",uiLength);
	if(uiLength > FILE_NAME_MAX_LEN)	// Added by ZhangYuan 2016-11-01,  parameter validation check
	{
		ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_INVALID_PARAM, FILEDOWNLOAD_ERR_INVALID_PARAM, __LINE__);
		return FILEDOWNLOAD_ERR_INVALID_PARAM;
	}

	uiReqOffset += 2;
	memcpy(sFileInfo.ucFileName, pucRequest + uiReqOffset, uiLength);
	uiReqOffset +=  uiLength;
	sFileInfo.uiFileSize = (uint)pucRequest[uiReqOffset]<<24 | (uint)pucRequest[uiReqOffset+1]<<16 | (uint)pucRequest[uiReqOffset+2]<<8 |(uint)pucRequest[uiReqOffset+3];
	uiReqOffset += sizeof(uint);
	sFileInfo.uiFileOffset = (uint)pucRequest[uiReqOffset]<<24 | (uint)pucRequest[uiReqOffset+1]<<16 | (uint)pucRequest[uiReqOffset+2]<<8 |(uint)pucRequest[uiReqOffset+3];
	uiReqOffset += sizeof(uint);
	sFileInfo.uiCurFileSize = (uint)pucRequest[uiReqOffset]<<24 | (uint)pucRequest[uiReqOffset+1]<<16 | (uint)pucRequest[uiReqOffset+2]<<8 |(uint)pucRequest[uiReqOffset+3];
	uiReqOffset += sizeof(uint);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiFileSize = %d, uiFileOffset = %d, uiCurFileSize = %d\n",sFileInfo.uiFileSize, sFileInfo.uiFileOffset, sFileInfo.uiCurFileSize);
	if(sFileInfo.uiFileOffset > sFileInfo.uiFileSize || sFileInfo.uiCurFileSize > BLK_LENGTH_MAX)	// Added by ZhangYuan 2016-11-01, parameter validation check
	{
		ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_INVALID_PARAM, FILEDOWNLOAD_ERR_INVALID_PARAM, __LINE__);
		return FILEDOWNLOAD_ERR_INVALID_PARAM;
	}

	memcpy(sFileInfo.ucFileContent, pucRequest + uiReqOffset, sFileInfo.uiCurFileSize);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiReqOffset = %d\n",uiReqOffset);
	g_IdeUi = 0x01;	// Added by ZhangYuan 2016-10-27

	//download step
	if(sFileInfo.uiFileOffset == 0 )//&& sFileInfo.uiCurFileSize != 0) // Modified by ZhangYuan 2016-11-01
	{
		eDownLoadStep = REMOTE_LOAD_BEGIN;

		ScrCls();	// Added by ZhangYuan 2016-11-01, clear screen if start to download file
	}
	else if(sFileInfo.uiFileSize == (sFileInfo.uiFileOffset + sFileInfo.uiCurFileSize))
	{
		eDownLoadStep = REMOTE_LOAD_END;

		g_IdeUi = 0x00;	// Added by ZhangYuan 2016-10-27		
	}
	else 
	{
		eDownLoadStep = REMOTE_LOAD_WRITE;
	}

	// === Added by ZhangYuan 2016-10-27 
	//	PubDispString(sFileInfo.ucFileName, 2|DISP_LINE_CENTER);
	ScrPrint(0,3,CFONT," Completed:%d%", (sFileInfo.uiFileOffset * 100)/sFileInfo.uiFileSize);
	// === Added end

	//get file type
	ucFileType = GetFileType((char*)sFileInfo.ucFileName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetFileType usFileType = %02x, %s\n",ucFileType, sFileInfo.ucFileName);
	switch(ucFileType){
	case PARAM_FILE_EMV:
	case PARAM_FILE_CLSS:
	case PARAM_FILE_UI:
	case PARAM_FILE_LNG:
	case PARAM_FILE_BMP:
		if(ucFileType == PARAM_FILE_UI)	// Added by ZhangYuan 2016-11-18, if ui file, use fixed name
		{
			memset(sFileInfo.ucFileName, 0, sizeof(sFileInfo.ucFileName));
			strcpy(sFileInfo.ucFileName, UI_FILE_NAME);
		}
		iRet = UpdateParamFile(sFileInfo);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "UpdateParamFile iRet = %d\n",iRet);
		if(iRet == 0){
			//			sv_ReadFile(sFileInfo);
			//			return 0;
			break;
		}
		else{
			ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL, iRet, __LINE__);
			return FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL;
		}
		break;

	case MONITOR_FILE:
	case FONT_FILE:
	case APP_FILE_M:
		iRet = UpdateFirmware(ucFileType, eDownLoadStep, &sFileInfo);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "UpdateFirmware=%d, eDownLoadStep = %d\n", iRet, eDownLoadStep);
		if(iRet == 0){
			break;
		}
		else{			
			return iRet;
		}
		break;

	default:
		ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_INVALID_PARAM, FILEDOWNLOAD_ERR_INVALID_PARAM, __LINE__);
		return FILEDOWNLOAD_ERR_INVALID_PARAM;
		//		break;
	}

	// === Added by ZhangYuan 2016-10-27 
	if(sFileInfo.uiFileOffset + sFileInfo.uiCurFileSize == sFileInfo.uiFileSize)
	{	

		//		PubDispString(sFileInfo.ucFileName, 2|DISP_LINE_CENTER);
		ScrPrint(0,1,CFONT," Completed:100%");
		ScrPrint(0,3,CFONT,"Wait, Parsing...");
		Beep();

		//add by huangwp 2016-11-28
		if(ucFileType == PARAM_FILE_UI)
		{
			g_UiFileExist = 1;
		}

		if(ucFileType == PARAM_FILE_EMV)	// Added by ZhangYuan 2016-11-08, parse parameter file
		{
			iRet = ParseEmvFile(sFileInfo.ucFileName);
			if(iRet < 0)
			{
				ScrCls();
				ScrPrint(0,3,CFONT,"EMV FILE ERROR");
				DelayMs(300);

				ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_PARSE_ERR, iRet, __LINE__);
				return FILEDOWNLOAD_ERR_PARSE_ERR;
			}
		}
		else if(ucFileType == PARAM_FILE_CLSS)
		{
			iRet = ParseClssFile(sFileInfo.ucFileName);

			if(iRet < 0)
			{
				ScrCls();
				ScrPrint(0,3,CFONT,"CLSS FILE ERROR");
				DelayMs(300);
				ComPoseInfoMsg(RetToString, FILEDOWNLOAD_ERR_PARSE_ERR, iRet, __LINE__);
				return FILEDOWNLOAD_ERR_PARSE_ERR;
			}
		}

		ScrCls();
		g_IdeUi = 0x00;
	}
	// === Added end

	ComPoseInfoMsg(RetToString, 0, 0, __LINE__);
	return 0;
}


