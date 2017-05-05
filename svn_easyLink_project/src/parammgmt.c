/*------------------------------------------------------------
* FileName: parammgmt.c
* Author: liukai
* Date: 2016-08-03
------------------------------------------------------------*/
#include "..\inc\global.h"

#define LOG_TAG	__FILE__
#ifdef PARAMMGMT_DEBUG_		
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else
#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)

#endif // PARAMMGMT_DEBUG_


static unsigned char sg_ucData[4096]={0};	//in D180s no malloc, have to use this.
static int sg_iDataLen=0;

DATA_TERMINFO g_tTermInfo;
DATA_APPPARAM g_tAppParam;
DATA_TRANSPARAM g_tTransParam;
DATA_MATCHLIST g_tMatchList[] = {
	//terminal information
	{CMD_TERM_SN,		g_tTermInfo.ucTermSN,				sizeof(g_tTermInfo.ucTermSN),	0,			DATA_PERMISSION_READ},
	{CMD_TERM_MODEL_CODE,		g_tTermInfo.ucTermModelCode,			sizeof(g_tTermInfo.ucTermModelCode), 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_PRINTER_INFO, 	g_tTermInfo.ucTermPrinterInfo,			sizeof(g_tTermInfo.ucTermPrinterInfo),		DATA_PERMISSION_READ},
	{CMD_TERM_MODEM_EXIST,		g_tTermInfo.ucTermModemExist,		sizeof(g_tTermInfo.ucTermModemExist), 0,	DATA_PERMISSION_READ},
	{CMD_TERM_USB_EXIST,		g_tTermInfo.ucTermUSBExist, 		sizeof(g_tTermInfo.ucTermUSBExist), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_LAN_EXIST,		g_tTermInfo.ucTermLANExist, 		sizeof(g_tTermInfo.ucTermLANExist),  0, 	DATA_PERMISSION_READ},
	{CMD_TERM_GPRS_EXIST,		g_tTermInfo.ucTermGPRSExist,			sizeof(g_tTermInfo.ucTermGPRSExist), 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_CDMA_EXIST,		g_tTermInfo.ucTermCDMAExist,			sizeof(g_tTermInfo.ucTermCDMAExist), 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_WIFI_EXIST,		g_tTermInfo.ucTermWIFIExist,			sizeof(g_tTermInfo.ucTermWIFIExist), 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_RF_EXIST, 	g_tTermInfo.ucTermRFExist,			sizeof(g_tTermInfo.ucTermRFExist), 0,			DATA_PERMISSION_READ},
	{CMD_TERM_IC_EXIST, 	g_tTermInfo.ucTermICExist,			sizeof(g_tTermInfo.ucTermICExist), 0,			DATA_PERMISSION_READ},
	{CMD_TERM_MAG_EXIST,		g_tTermInfo.ucTermMAGExist, 		sizeof(g_tTermInfo.ucTermMAGExist), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_TILT_EXIST,		g_tTermInfo.ucTermTILTExist,			sizeof(g_tTermInfo.ucTermTILTExist), 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_WCDMA_EXIST,		g_tTermInfo.ucTermWCDMAExist,		sizeof(g_tTermInfo.ucTermWCDMAExist), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_TOUCHSCREEN_EXIST,		g_tTermInfo.ucTermTOUCHSCRExist,		sizeof(g_tTermInfo.ucTermTOUCHSCRExist), 0, DATA_PERMISSION_READ},
	{CMD_TERM_COLORSCREEN_EXIST,		g_tTermInfo.ucTermCOLORSCRExist,		sizeof(g_tTermInfo.ucTermCOLORSCRExist), 0, DATA_PERMISSION_READ},
	{CMD_TERM_SCREEN_SIZE,		g_tTermInfo.ucTermScrSize,			sizeof(g_tTermInfo.ucTermScrSize), 0,			DATA_PERMISSION_READ},
	{CMD_TERM_APP_NAME, 	g_tTermInfo.ucTermAppName,			sizeof(g_tTermInfo.ucTermAppName), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_APP_VERSION,		g_tTermInfo.ucTermAppVer,			sizeof(g_tTermInfo.ucTermAppVer), 0,			DATA_PERMISSION_READ},
	{CMD_TERM_APPSO_NAME,		g_tTermInfo.ucTermAppSoName,		sizeof(g_tTermInfo.ucTermAppSoName), 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_APPSO_VERSION,		g_tTermInfo.ucTermAppSoVer, 		sizeof(g_tTermInfo.ucTermAppSoVer), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_PUBSO_NAME,		g_tTermInfo.ucTermPubSoName,		sizeof(g_tTermInfo.ucTermPubSoName), 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_PUBSO_VERSION,		g_tTermInfo.ucTermPubSoVer, 		sizeof(g_tTermInfo.ucTermPubSoVer), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_BATTERY_INFO, 	g_tTermInfo.ucTermBatteryInfo,		sizeof(g_tTermInfo.ucTermBatteryInfo), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_REST_FS_SIZE, 	g_tTermInfo.ucTermRestFSSize,			sizeof(g_tTermInfo.ucTermRestFSSize), 0,		DATA_PERMISSION_READ},
	{CMD_TERM_OS_NAME,		g_tTermInfo.ucTermOSName,			sizeof(g_tTermInfo.ucTermOSName),	 0, 	DATA_PERMISSION_READ},
	{CMD_TERM_OS_VERSION,		g_tTermInfo.ucTermOSVer,				sizeof(g_tTermInfo.ucTermOSVer), 0, 		DATA_PERMISSION_READ},
	{CMD_TERM_PCI_EXIST,		g_tTermInfo.ucTermPCIExist, 		sizeof(g_tTermInfo.ucTermPCIExist),  0, 	DATA_PERMISSION_READ},
	{CMD_TERM_TIME,		g_tTermInfo.ucTermTime, 		sizeof(g_tTermInfo.ucTermTime),  0, 	DATA_PERMISSION_READ_WRITE},
	//application parameters
	{CMD_APP_SLEEP_TIMEOUT, 	g_tAppParam.ucSleepModeTimeout, 	sizeof(g_tAppParam.ucSleepModeTimeout), 0,	DATA_PERMISSION_READ_WRITE},
	{CMD_APP_PIN_ENCRYP_TYPE,		g_tAppParam.ucPINEncryptionType,		sizeof(g_tAppParam.ucPINEncryptionType), 0, DATA_PERMISSION_READ_WRITE},
	{CMD_APP_PIN_ENCRYP_KEY_ID, 	g_tAppParam.ucPINEncryptionKeyIdx,	sizeof(g_tAppParam.ucPINEncryptionKeyIdx), 0,	DATA_PERMISSION_READ_WRITE},
	{CMD_APP_PIN_BLOCK_MODE,		g_tAppParam.ucPINBlockMode, 		sizeof(g_tAppParam.ucPINBlockMode), 0,	DATA_PERMISSION_READ_WRITE},
	{CMD_APP_DATA_ENCRYP_TYPE,		g_tAppParam.ucDataEncryptionType,	sizeof(g_tAppParam.ucDataEncryptionType), 0,	DATA_PERMISSION_READ_WRITE},
	{CMD_APP_DATA_ENCRYP_KEY_ID,		g_tAppParam.ucDataEncryptionKeyIdx, sizeof(g_tAppParam.ucDataEncryptionKeyIdx), 0, DATA_PERMISSION_READ_WRITE},
	{CMD_APP_FALLBACK_ALLOW,		g_tAppParam.ucFallbackAllowFlag,		sizeof(g_tAppParam.ucFallbackAllowFlag), 0, DATA_PERMISSION_READ_WRITE},
	{CMD_APP_PAN_MASK_START_POS,		g_tAppParam.ucPANMaskStartPos,		sizeof(g_tAppParam.ucPANMaskStartPos), 0,	DATA_PERMISSION_READ_WRITE},
	//transaction parameters
	//if add the transaction parameters, please remember to initialize it in ConfigueTransParamInit function.
	{CMD_TXN_CUR_TXN_TYPE,		g_tTransParam.ucCurrentTxnType, 	sizeof(g_tTransParam.ucCurrentTxnType), 0,		DATA_PERMISSION_READ},
	{CMD_TXN_CUR_CLSS_TYPE, 	g_tTransParam.ucCurrentCLSSType,	sizeof(g_tTransParam.ucCurrentCLSSType), 0, DATA_PERMISSION_READ},
	{CMD_TXN_PATH_TYPE, 	g_tTransParam.ucCurrentPathType,	sizeof(g_tTransParam.ucCurrentPathType), 0, DATA_PERMISSION_READ},
	{CMD_TXN_TRACK_ONE_DATA,		g_tTransParam.ucTrackOneData,		sizeof(g_tTransParam.ucTrackOneData), 0,		DATA_PERMISSION_READ},
	{CMD_TXN_TRACK_TWO_DATA,		g_tTransParam.ucTrackTwoData,		sizeof(g_tTransParam.ucTrackTwoData), 0,		DATA_PERMISSION_READ},
	{CMD_TXN_TRACK_THREE_DATA,		g_tTransParam.ucTrackThreeData, 	sizeof(g_tTransParam.ucTrackThreeData), 0,		DATA_PERMISSION_READ},
	{CMD_TXN_EXPIRE_DATE,		g_tTransParam.ucExpDate, 	sizeof(g_tTransParam.ucExpDate), 0,		DATA_PERMISSION_READ},

	{CMD_TXN_ONLINE_RESULT, 	g_tTransParam.ucOnlineResult,		sizeof(g_tTransParam.ucOnlineResult), 0,			DATA_PERMISSION_READ_WRITE},
	{CMD_TXN_RSP_CODE,		g_tTransParam.ucRspCode,			sizeof(g_tTransParam.ucRspCode), 0, 			DATA_PERMISSION_READ_WRITE},
	{CMD_TXN_AUTH_CODE, 	g_tTransParam.ucAuthCode,			sizeof(g_tTransParam.ucAuthCode), 0,				DATA_PERMISSION_READ_WRITE},
	{CMD_TXN_AUTH_DATA, 	g_tTransParam.ucAuthData,			sizeof(g_tTransParam.ucAuthData), 0,				DATA_PERMISSION_READ_WRITE},
	{CMD_TXN_AUTH_DATA_LEN, 	g_tTransParam.ucAuthDataLen,		sizeof(g_tTransParam.ucAuthDataLen), 0, 		DATA_PERMISSION_READ_WRITE},
	{CMD_TXN_ISSUER_SCRIPT, 	g_tTransParam.ucIssuerScript,		sizeof(g_tTransParam.ucIssuerScript), 0,			DATA_PERMISSION_READ_WRITE},
	{CMD_TXN_ISSUER_SCRIPT_LEN, 	g_tTransParam.ucIssuerScriptLen,	sizeof(g_tTransParam.ucIssuerScriptLen), 0, 	DATA_PERMISSION_READ_WRITE},

	{CMD_TXN_ONLINE_PIN_INPUT,		g_tTransParam.ucOnlinePinInput, 	sizeof(g_tTransParam.ucOnlinePinInput), 0,		DATA_PERMISSION_READ},
	{CMD_TXN_PIN_BLOCK, 	g_tTransParam.ucPinBlock,			sizeof(g_tTransParam.ucPinBlock), 0,			DATA_PERMISSION_READ},
	{CMD_TXN_KSN,		g_tTransParam.ucKsn,				sizeof(g_tTransParam.ucKsn), 0, 			DATA_PERMISSION_READ},

	{CMD_TXN_ICS_TYPE,		g_tTransParam.ucICSType,			sizeof(g_tTransParam.ucICSType), 0, 		DATA_PERMISSION_READ_WRITE},
	{CMD_TXN_MASKEDPAN,		g_tTransParam.ucMaskedPAN,			sizeof(g_tTransParam.ucMaskedPAN), 0, 		DATA_PERMISSION_READ},	// Modified by ZhangYuan 2016-12-19, read only
	{CMD_TXN_CARD_PROCESSING_RESULT, g_tTransParam.ucCardProcessResult, sizeof(g_tTransParam.ucCardProcessResult), 0, DATA_PERMISSION_READ}, // Added by ZhangYuan 2017-04-17

	{CMD_TXN_CVM_TYPE, g_tTransParam.ucCurrentCVMType, sizeof(g_tTransParam.ucCurrentCVMType), 0, DATA_PERMISSION_READ},	// Added by ZhangYuan 2016-12-19

	{CMD_TXN_CARD_PROCESSING_RESULT, g_tTransParam.ucCardProcessResult, sizeof(g_tTransParam.ucCardProcessResult), 0, DATA_PERMISSION_READ}, // Added by ZhangYuan 2017-04-17
//	{CMD_TXN_PAN_DATA, g_tTransParam.ucPAN, sizeof(g_tTransParam.ucPAN), 0, DATA_PERMISSION_READ}, // Added by ZhangYuan 2017-04-17

	{"", NULL, 0},
};

#define PARAM_SENSITIVE_TAG_NUM_MAX    10
unsigned int gl_Param_SensitiveTagList[PARAM_SENSITIVE_TAG_NUM_MAX] = {0x56, 0x57, 0x5A, CMD_TXN_TRACK_ONE_DATA, CMD_TXN_TRACK_TWO_DATA, CMD_TXN_TRACK_THREE_DATA, 0x00};

int SetConfigurationData(const unsigned char *pucTag, const unsigned char *pucData, int iDataLen);
int GetConfigurationData(const unsigned char *pucTag, unsigned char *pucDataOut, int *piDataLenOut);
static int ValidateTLVData(const unsigned char * paucTLVDataList, unsigned int uiTLVDataListLen, unsigned char * paucWrongTag, unsigned int * punTagLen);
static int ValidateAndExecuteConfigurationData(const unsigned char *pucTag, const unsigned char *pucData, int iDataLen);
static int GetSpecifyConfigurationData(const unsigned char *pucTag, int iTagLen, unsigned char *punDataOut, int *piDataLen);
//static int SaveClssPreProcParam(const unsigned char *pucTag, int iTagLen, const unsigned char *pucData, int iDataLen);
int SetData(const unsigned char *pucData, unsigned char *pucSuccessTagListOut, unsigned int *puiSuccessTagListLenOut);
int GetData(const unsigned char *pucData, unsigned char *pucTLVListOut, unsigned int *puiTLVListLenOut);
int LoadTLVDataFromFSToMem(void);
int GetAllTLVDataFromMem(unsigned char *pucDataOut, int *piDataLen);

// Added by ZhangYuan 2017-04-18
static int IsSensitiveTag(unsigned short usTag)
{
	int iTmp = 0;

	while(iTmp < PARAM_SENSITIVE_TAG_NUM_MAX && 0x00 != gl_Param_SensitiveTagList[iTmp])
	{
		if(usTag == gl_Param_SensitiveTagList[iTmp])
		{
			return 1;
		}

		iTmp ++;
	}

	return 0;
}

static uchar *RetToString(int ReturnCode)
{
	switch(ReturnCode)
	{
	case EL_PARAM_RET_ERR_DATA:
		return "Error Data";
	case EL_PARAM_RET_INVALID_PARAM:
		return "Invalid Parameter";
	case EL_PARAM_RET_PARTIAL_FAILED:
		return "Partial Failed";
	case EL_PARAM_RET_ALL_FAILED:
		return "All Failed";
	case EL_PARAM_RET_SIZE_SMALL:
		return "Size Small";
	case EL_PARAM_RET_API_ORDER_ERR:
		return "Api Order Error";

	case EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED:
		return "Encrypt sensitive data failed";

	case CLSS_API_ORDER_ERR:
		return "APIs are called in wrong order";
	
	default:
		return "Unknown Error";
	}
}



int SetConfigurationData(const unsigned char *pucTag, const unsigned char *pucData, int iDataLen)
{
	int i=0, iRet=0;
	int iTagLen=CONFIGURATION_TAG_LENGTH;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucTag: %02x %02x, iDataLen=%d\n", pucTag[0], pucTag[1], iDataLen);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucData: ", pucData, iDataLen);

	for ( i=0; g_tMatchList[i].pIndicator != NULL; i++ ) 
	{
		if ( 0 == memcmp(g_tMatchList[i].ucTag, pucTag, iTagLen) ) 
		{
			if ( (unsigned int)iDataLen > g_tMatchList[i].uiBufLen - 1 ) 
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iDataLen > g_tMatchList[i].uiBufLen - 1, iDataLen=%d, g_tMatchList[i].uiBufLen - 1 =%d\n", iDataLen, g_tMatchList[i].uiBufLen - 1 );
				return EL_PARAM_RET_INVALID_PARAM;
			}

			iRet = ValidateAndExecuteConfigurationData(pucTag, pucData, iDataLen);
			if ( iRet != EMV_OK ) 
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ValidateAndExecuteConfigurationData=%d\n", iRet);
				return iRet;
			}

			memset(g_tMatchList[i].pIndicator, 0, g_tMatchList[i].uiBufLen);
			memcpy(g_tMatchList[i].pIndicator, pucData, iDataLen);
			g_tMatchList[i].uiDataLen = iDataLen;
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_tMatchList[i].ucTag: %02x %02x, g_tMatchList[i].uiDataLen = %d, g_tMatchList[i].pIndicator = %02x\n", g_tMatchList[i].ucTag[0], g_tMatchList[i].ucTag[1], g_tMatchList[i].uiDataLen, g_tMatchList[i].pIndicator);
			EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_tMatchList[i].pIndicator: ", g_tMatchList[i].pIndicator, g_tMatchList[i].uiDataLen);
			break;
		}
	}

	//not found the tag
	if ( g_tMatchList[i].pIndicator == NULL ) 
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_tMatchList[i].pIndicator == NULL\n");
		return EL_PARAM_RET_INVALID_PARAM;
	}

	return EMV_OK;
}

int GetConfigurationData(const unsigned char *pucTag, unsigned char *pucDataOut, int *piDataLenOut)
{
	int i=0, iRet=-1;
	int iTagLen=CONFIGURATION_TAG_LENGTH;

	for ( i=0; g_tMatchList[i].pIndicator != NULL; i++ ) {
		if ( 0 == memcmp(g_tMatchList[i].ucTag, pucTag, iTagLen) ) {

			iRet = GetSpecifyConfigurationData(pucTag, iTagLen, g_tMatchList[i].pIndicator, &g_tMatchList[i].uiDataLen);
			if ( iRet != EMV_OK ) {
				return EL_PARAM_RET_INVALID_PARAM;
			}

			memcpy(pucDataOut, g_tMatchList[i].pIndicator, g_tMatchList[i].uiDataLen);
			*piDataLenOut = g_tMatchList[i].uiDataLen;
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "piDataLenOut = %d, pucDataOut = %s\n", *piDataLenOut, pucDataOut);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_tMatchList[i].uiDataLen = %d, g_tMatchList[i].pIndicator = %s\n", g_tMatchList[i].uiDataLen, g_tMatchList[i].pIndicator);
			break;
		}
	}

	//not found the tag
	if ( g_tMatchList[i].pIndicator == NULL ) {
		return EL_PARAM_RET_INVALID_PARAM;
	}

	return EMV_OK;
}

static int ValidateTLVData(const unsigned char * paucTLVDataList, unsigned int uiTLVDataListLen,
						   unsigned char * paucWrongTag, unsigned int * punTagLen)
{
	int iRet=0;
	unsigned char aucWrongTagList[256+1]={0};

	if ( paucTLVDataList == NULL || paucWrongTag == NULL || punTagLen == NULL ) {
		return EL_PARAM_RET_INVALID_PARAM;
	}

	*punTagLen = 0;
#ifndef _WIN32
	iRet = EMV_II_CheckTagListValid(paucTLVDataList, uiTLVDataListLen, aucWrongTagList, punTagLen);
#endif
	return iRet;
}

static int ValidateAndExecuteConfigurationData(const unsigned char *pucTag, const unsigned char *pucData, int iDataLen)
{
	int iRet=-1, iTagLen=CONFIGURATION_TAG_LENGTH;
	unsigned long ulTmp=0;

	if ( pucTag == NULL ) 
	{
		return EL_PARAM_RET_INVALID_PARAM;
	}

	if ( 0 == memcmp(pucTag, CMD_APP_SLEEP_TIMEOUT, iTagLen) ) 
	{
		PubBcd2Long((unsigned char*)pucData, 2, &ulTmp);
		if ( (ulTmp < 0) || (ulTmp > 3000) ) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	} 
	else if ( 0 == memcmp(pucTag, CMD_APP_PIN_ENCRYP_TYPE, iTagLen) ) 
	{
		if ( pucData[0] < 0x00 ||  pucData[0] > 0x02) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	} 
	else if ( 0 == memcmp(pucTag, CMD_APP_PIN_BLOCK_MODE, iTagLen) ) 
	{
		if ( pucData[0] < 0x00 ||  pucData[0] > 0x03 ) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	} 
	else if ( 0 == memcmp(pucTag, CMD_APP_DATA_ENCRYP_TYPE, iTagLen) ) 
	{
		if ( pucData[0] < 0x00 ||  pucData[0] > 0x02 ) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	} 
	else if ( 0 == memcmp(pucTag, CMD_APP_FALLBACK_ALLOW, iTagLen) ) 
	{
		if ( pucData[0] < 0x00 ||  pucData[0] > 0x01 ) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	} 
	else if ( 0 == memcmp(pucTag, CMD_APP_PAN_MASK_START_POS, iTagLen) ) 
	{
		if ( pucData[0] < 0x00 ||  pucData[0] > 0x06 ) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	} 
	else if ( 0 == memcmp(pucTag, CMD_TXN_ONLINE_RESULT, iTagLen) ) 
	{
		if ( pucData[0] < 0x00 ||  pucData[0] > 0x02 ) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	} 
	else if ( 0 == memcmp(pucTag, CMD_TERM_TIME, iTagLen) ) 
	{
		if ( 0 != SetTime((unsigned char*)pucData) ) 
		{
			return EL_PARAM_RET_INVALID_PARAM;
		}
	}
	else 
	{	//no limit for here.
		return EMV_OK;
	}

	return EMV_OK;
}

static int GetSpecifyConfigurationData(const unsigned char *pucTag, int iTagLen, unsigned char *punDataOut, int *piDataLen)
{
	unsigned char ucTemp[128]={0};
	int iLen=0;

	if ( pucTag == NULL ) {
		return EL_PARAM_RET_INVALID_PARAM;
	}

	//get battery info.
	if ( 0 == memcmp(pucTag, CMD_TERM_BATTERY_INFO, CONFIGURATION_TAG_LENGTH) ) {
		ucTemp[0] = BatteryCheck();
		iLen = 1;
	} else if ( 0 == memcmp(pucTag, CMD_TERM_TIME, CONFIGURATION_TAG_LENGTH) ) {
		GetTime(ucTemp);
		iLen = 7;		//len = 7, BCD: YYMMDDhhmmssWW
	} else {
		return EMV_OK;
	}

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucTemp: ", ucTemp, iLen);
	if ( iLen != 0 ) {
		memcpy(punDataOut, ucTemp, iLen);
		*piDataLen = iLen;
	}

	return EMV_OK;
}

static int SaveIssuerTags(unsigned short usTag, const unsigned char *paucData, int iDataLen)
{
	int iRet=0, iAuthDataLen=0, iIssuerScriptLen=0;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usTag = %x\n", usTag);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "paucData: ", paucData, iDataLen);

	switch( usTag )
	{
	case 0x91:
		memcpy(g_tTransParam.ucAuthData, paucData, MIN(iDataLen, 16));
		iAuthDataLen = MIN(iDataLen, 16);
		g_tTransParam.ucAuthDataLen[0] = iAuthDataLen / 256;
		g_tTransParam.ucAuthDataLen[1] = iAuthDataLen % 256;
		break;

	case 0x71:
	case 0x72:
		iIssuerScriptLen = g_tTransParam.ucIssuerScriptLen[0] * 256 + g_tTransParam.ucIssuerScriptLen[1];
		g_tTransParam.ucIssuerScript[iIssuerScriptLen++] = (uchar)usTag;
		if( iDataLen > 127 )
		{
			g_tTransParam.ucIssuerScript[iIssuerScriptLen++] = 0x81;
		}
		g_tTransParam.ucIssuerScript[iIssuerScriptLen++] = (uchar)iDataLen;
		memcpy(&g_tTransParam.ucIssuerScript[iIssuerScriptLen], paucData, iDataLen);
		iIssuerScriptLen += iDataLen;
		g_tTransParam.ucIssuerScriptLen[0] = iIssuerScriptLen / 256;
		g_tTransParam.ucIssuerScriptLen[1] = iIssuerScriptLen % 256;
		return EMV_OK;

	default:
		break;
	}

	return -1;;
}

int LoadTLVDataFromFSToMem(void)
{
	int iRet=0, iFd=0;
	unsigned int uiTLVDataLen=0;

	memset(sg_ucData, 0, sizeof(sg_ucData));
	sg_iDataLen = 0;

	iFd = PdkFile_Open(TRANS_PARAM_FILE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open = %d\n", iFd);
	if ( iFd < 0 ) 
	{
		return EL_PARAM_RET_ALL_FAILED;
	}

	iRet = PdkFile_Read(iFd, (unsigned char*)&uiTLVDataLen, sizeof(uiTLVDataLen)); 
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read = %d, uiTLVDataLen = %d\n", iRet, uiTLVDataLen);
	if ( (iRet != sizeof(uiTLVDataLen)) || (!uiTLVDataLen) ) 
	{	//if uiTLVDataLen = 0, no data
		PdkFile_Close(iFd);
		PdkFile_Remove(TRANS_PARAM_FILE);		//added by kevinliu 20161025
		return EL_PARAM_RET_ALL_FAILED;
	}

	//the following code could work on monitor and prolin, not monitor mini.-------------------------------
	/*	
	if ( sg_pucData ) {
	free(sg_pucData);
	sg_pucData = NULL;
	sg_iDataLen = 0;
	}
	sg_pucData = (unsigned char*)malloc(uiTLVDataLen);
	if ( !sg_pucData ) {
	return EL_PARAM_RET_ALL_FAILED;
	}
	*/
	//------------------------------------------------------------------------------------------


	//	iRet = PdkFile_Read(iFd, sg_pucData, uiTLVDataLen);
	iRet = PdkFile_Read(iFd, sg_ucData, uiTLVDataLen);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read = %d\n", iRet);
	if ( iRet != uiTLVDataLen ) 
	{
		//the following code could work on monitor and prolin, not monitor mini.-------------------------------
		/*
		free(sg_pucData);
		sg_pucData = NULL;
		sg_iDataLen = 0;
		*/
		//----------------------------------------------------------------------------------------
		PdkFile_Close(iFd);
		PdkFile_Remove(TRANS_PARAM_FILE);		//added by kevinliu 20161025
		return EL_PARAM_RET_ALL_FAILED;
	}

	sg_iDataLen = uiTLVDataLen;
	PdkFile_Close(iFd);
	PdkFile_Remove(TRANS_PARAM_FILE);

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sg_ucData: ", sg_ucData, sg_iDataLen);

	return EMV_OK;
}

int GetAllTLVDataFromMem(unsigned char *pucDataOut, int *piDataLen)
{
	int iRet=0;

	//	if ( (!sg_pucData) || (!sg_iDataLen) ) {
	if ( (!sg_ucData) || (!sg_iDataLen) ) 
	{
		return EL_PARAM_RET_INVALID_PARAM;
	}

	//the following code could work on monitor and prolin, not monitor mini.-------------------------------
	/*
	memcpy(pucDataOut, sg_pucData, sg_iDataLen);
	*piDataLen = sg_iDataLen;

	free(sg_pucData);
	sg_pucData = NULL;
	sg_iDataLen = 0;
	*/
	//-----------------------------------------------------------------------------------------

	memcpy(pucDataOut, sg_ucData, sg_iDataLen);
	*piDataLen = sg_iDataLen;

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucDataOut: ", pucDataOut, *piDataLen);

	return EMV_OK;
}


int SetData(const unsigned char *pucData, unsigned char *pucFailedTagListOut, unsigned int *puiFailedTagListLenOut)
{
	int iRet=-1, iPos=0, iTagLen=0, iDataLen=0, iLengthLen=0, iLengthSubsequentByte=0, iCount=0, iOutTagLen=0;
	unsigned char ucConfigurationTag[2+1]= {0};
	int iTagCount=0, iSuccessCount=0, iFailCount=0;
	int i=0, iFd=0;
	unsigned char ucDataType=0;
	const unsigned char *pucTlvList=NULL;
	unsigned int uiTLVListLen=0, uiPrevTLVListLen=0, uiCurTLVListLen=0, uiFailedTagLen=0;
	unsigned char ucFailedTag[3+1]={0};
	unsigned short usTag=0;
	int iRet1=0, iRet2=0, iRet3=0;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Begin ========== \n");
	if ( (pucData == NULL) || (pucFailedTagListOut == NULL) || (puiFailedTagListLenOut == NULL) ) 
	{
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_INVALID_PARAM, EL_PARAM_RET_INVALID_PARAM, __LINE__);
		return EL_PARAM_RET_INVALID_PARAM;
	}

	//parser income data.
	ucDataType = pucData[0];
	uiTLVListLen = pucData[1] * 256 + pucData[2];
	pucTlvList = pucData + 3;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucDataType = %02x, uiTLVListLen = %d\n", ucDataType, uiTLVListLen);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucTlvList: ", pucTlvList, uiTLVListLen);

	//added by kevinliu 20161027
	if ( uiTLVListLen == 0 ) 
	{
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_INVALID_PARAM, EL_PARAM_RET_INVALID_PARAM, __LINE__);
		return EL_PARAM_RET_INVALID_PARAM;
	}	

	//data type = EMV TLV data.
	if ( ucDataType == PARAMMGMT_DATATYPE_EMV ) 
	{
		while(((unsigned int)iPos < uiTLVListLen) ) 
		{
			//calculate tag length, refer to EMV tag Field structure
			if ( 0x1F == (pucTlvList[iPos] & 0x1F) ) 
			{				
				if ( (unsigned int)(iPos + 1) >= uiTLVListLen ) // Added by ZhangYuan 2016-10-18 check length
				{
					iTagLen = 1;
					memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
					iOutTagLen += iTagLen;
					pucFailedTagListOut[0] = iOutTagLen /256;
					pucFailedTagListOut[1] = iOutTagLen % 256;
					iFailCount ++;
					break;
				}

				if ( 0x80 == (pucTlvList[iPos + 1] & 0x80) ) 
				{
					iTagLen = 3;
					usTag = 0;
				} 
				else 
				{
					iTagLen = 2;
					usTag = pucTlvList[iPos] * 256 + pucTlvList[iPos + 1];
				}
			} 
			else 
			{
				iTagLen = 1;
				usTag = pucTlvList[iPos];
			}

			iTagCount++;
			//check length
			if ( (unsigned int)(iPos + iTagLen) >= uiTLVListLen ) 
			{
				memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
				iOutTagLen += iTagLen;
				pucFailedTagListOut[0] = iOutTagLen /256;
				pucFailedTagListOut[1] = iOutTagLen % 256;
				iFailCount ++;
				break;
			}

			//calculate data length, refer to EMV tag Field structure
			iCount = 0;
			iDataLen = 0;
			iLengthSubsequentByte = 0;
			if ( 0x80 == (pucTlvList[iPos + iTagLen] & 0x80) ) 
			{
				iLengthSubsequentByte = pucTlvList[iPos + iTagLen] & 0x7F;				
				if ( (unsigned int)(iPos + iTagLen + iLengthSubsequentByte) >= uiTLVListLen ) // Added by ZhangYuan 2016-10-18 check length
				{
					memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList[iPos + iTagLen], 1);				
					iOutTagLen += 1;
					pucFailedTagListOut[0] = iOutTagLen /256;
					pucFailedTagListOut[1] = iOutTagLen % 256;
					iFailCount ++;
					break;
				}

				for ( i = iLengthSubsequentByte; i > 0; i-- ) 
				{
					if ( iCount == 0 ) 
					{
						iDataLen += pucTlvList[iTagLen + i];
					} 
					else 
					{
						iDataLen += pucTlvList[iTagLen + i] * iCount * 256;
					}
					iCount++;
				}
			} 
			else 
			{
				iDataLen = pucTlvList[iPos + iTagLen];
			}
			iLengthLen = 1 + iLengthSubsequentByte;
			//check length
			if ( (unsigned int)(iPos + iTagLen + iLengthLen + iDataLen) > uiTLVListLen ) 
			{
				memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
				iOutTagLen += iTagLen;
				pucFailedTagListOut[0] = iOutTagLen /256;
				pucFailedTagListOut[1] = iOutTagLen % 256;
				iFailCount ++;
				break;
			}

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iTagLen = %d\n", iTagLen);
			EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Tag: ", pucTlvList+iPos, iTagLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iLengthLen = %d, iDataLen = %d\n", iLengthLen, iDataLen);
			EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Value: ", pucTlvList+iPos+iTagLen+iLengthLen, iDataLen);

			//For saving memory, on D180s, the EMV kernel won't save the tag 71 and 72. should save it in application.
#ifdef _D180S_
			iRet = SaveIssuerTags(usTag, pucTlvList+iPos+iTagLen+iLengthLen, iDataLen);
			if ( iRet == EMV_OK ) {
//				memcpy(pucSuccessTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);
//				iOutTagLen += iTagLen;
//				pucSuccessTagListOut[0] = iOutTagLen /256;
//				pucSuccessTagListOut[1] = iOutTagLen % 256;
				iSuccessCount++;

				iPos += iTagLen + iLengthLen + iDataLen;
				continue;
			}
#endif

			iRet = ValidateTLVData(pucTlvList + iPos, iTagLen + iLengthLen + iDataLen, ucFailedTag, &uiFailedTagLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ValidateTLVData = %d\n", iRet);
			if ( (iRet == EMV_OK) && (uiFailedTagLen == 0) ) 
			{
				//try to set data here.
#ifndef _WIN32
				EMVSetTLVData(usTag, pucTlvList+iPos+iTagLen+iLengthLen, iDataLen);
				Clss_SetTLVData_Wave(usTag, pucTlvList+iPos+iTagLen+iLengthLen, iDataLen);
				Clss_SetTLVDataList_MC(pucTlvList + iPos, iTagLen + iLengthLen + iDataLen);
#endif
				//if one of these three return CLSS_API_ORDER_ERR, save the data for future use.
				//TODO, remember delete this file when the transaction finished.
				iFd = PdkFile_Open(TRANS_PARAM_FILE);
				if ( iFd < 0 ) 
				{
					ComPoseInfoMsg(RetToString, EL_PARAM_RET_ALL_FAILED, EL_PARAM_RET_ALL_FAILED, __LINE__);
					return EL_PARAM_RET_ALL_FAILED;
				}

				iRet = PdkFile_GetSize (TRANS_PARAM_FILE);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_GetSize = %d\n", iRet);
				if ( iRet > sizeof(uiTLVListLen) ) 
				{
					iRet = PdkFile_Read(iFd, (unsigned char*)&uiPrevTLVListLen, sizeof(uiPrevTLVListLen)); 
					if ( iRet != sizeof(uiPrevTLVListLen) ) {
						// Added by ZhangYuan 2016-10-17 Close file ========
						PdkFile_Close(iFd);
						// Added end ================================
						ComPoseInfoMsg(RetToString, EL_PARAM_RET_ALL_FAILED, EL_PARAM_RET_ALL_FAILED, __LINE__);
						return EL_PARAM_RET_ALL_FAILED;
					}
				}

				PdkFile_Seek(iFd, 0, SEEK_SET);
				uiCurTLVListLen = uiPrevTLVListLen + (iTagLen + iLengthLen + iDataLen);
				iRet = PdkFile_Write(iFd, (unsigned char*)&uiCurTLVListLen, sizeof(uiCurTLVListLen)); 
				if ( iRet != sizeof(uiCurTLVListLen) ) 
				{
					// Added by ZhangYuan 2016-10-17 Close file ===
					PdkFile_Close(iFd);
					// Added end ============================
					ComPoseInfoMsg(RetToString, EL_PARAM_RET_ALL_FAILED, EL_PARAM_RET_ALL_FAILED, __LINE__);
					return EL_PARAM_RET_ALL_FAILED;
				}

				PdkFile_Seek(iFd, sizeof(uiTLVListLen) + uiPrevTLVListLen, SEEK_SET);
				iRet = PdkFile_Write(iFd, pucTlvList + iPos, iTagLen + iLengthLen + iDataLen); 
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write = %d\n", iRet);
				EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Data: ", pucTlvList+iPos, iTagLen + iLengthLen + iDataLen);
				if ( iRet != iTagLen + iLengthLen + iDataLen ) 
				{
					// Added by ZhangYuan 2016-10-17 Close file ========
					PdkFile_Close(iFd);
					// Added end ================================
					ComPoseInfoMsg(RetToString, EL_PARAM_RET_ALL_FAILED, EL_PARAM_RET_ALL_FAILED, __LINE__);
					return EL_PARAM_RET_ALL_FAILED;
				}

				PdkFile_Close(iFd);


//				memcpy(pucSuccessTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);
//				iOutTagLen += iTagLen;
//				pucSuccessTagListOut[0] = iOutTagLen /256;
//				pucSuccessTagListOut[1] = iOutTagLen % 256;
				iSuccessCount++;
				//special process for clss preprocess  parameter, if read from file will slow down the clss speed.s
				//				SaveClssPreProcParam(pucTlvList+iPos, iTagLen, pucTlvList+iPos+iTagLen+iLengthLen, iDataLen);
			}
			else 
			{			
				iFailCount++;

				memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
				iOutTagLen += iTagLen;
				pucFailedTagListOut[0] = iOutTagLen /256;
				pucFailedTagListOut[1] = iOutTagLen % 256;				
			}

			iPos += iTagLen + iLengthLen + iDataLen;
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iPos=%d\n", iPos);
		}		
	} 
	else if ( ucDataType == PARAMMGMT_DATATYPE_CONFIG ) 
	{
		//		while ( pucTlvList[iPos] && ((unsigned int)iPos <= uiTLVListLen) ) {
		while((unsigned int)iPos < uiTLVListLen)	// modified by ZhangYuan 2016-10-18
		{
			iTagLen = CONFIGURATION_TAG_LENGTH;
			// Added by ZhangYuan 2016-10-18 check length
			if ( (unsigned int)(iPos + iTagLen) >= uiTLVListLen ) 
			{
				iTagLen = 1;
				memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
				iOutTagLen += iTagLen;
				pucFailedTagListOut[0] = iOutTagLen /256;
				pucFailedTagListOut[1] = iOutTagLen % 256;
				iFailCount ++;
				
				break;
			}
			// Added end
			iTagCount++;
			memcpy(ucConfigurationTag, pucTlvList + iPos, iTagLen);

			//calculate data length, refer to EMV tag Field structure
			iCount = 0;
			iDataLen = 0;
			iLengthSubsequentByte = 0;
			if ( 0x80 == (pucTlvList[iPos + iTagLen] & 0x80) ) 
			{
				iLengthSubsequentByte = pucTlvList[iPos + iTagLen] & 0x7F;
				// Added by ZhangYuan 2016-10-18 check length
				if ( (unsigned int)(iPos + iTagLen + iLengthSubsequentByte) >= uiTLVListLen ) 
				{
					memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
					iOutTagLen += iTagLen;
					pucFailedTagListOut[0] = iOutTagLen /256;
					pucFailedTagListOut[1] = iOutTagLen % 256;
					iFailCount ++;
					break;
				}
				// Added end
				for ( i = iLengthSubsequentByte; i > 0; i-- ) 
				{
					if ( iCount == 0 ) 
					{
						iDataLen += pucTlvList[iTagLen + i];
					} 
					else 
					{
						iDataLen += pucTlvList[iTagLen + i] * iCount * 256;
					}
					iCount++;
				}
			} 
			else 
			{
				iDataLen = pucTlvList[iPos + iTagLen];
			}

			iLengthLen = 1 + iLengthSubsequentByte;
			//check length
			if ( (unsigned int)(iPos + iTagLen + iLengthLen + iDataLen) > uiTLVListLen ) 
			{
				iFailCount++;

				memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
				iOutTagLen += iTagLen;
				pucFailedTagListOut[0] = iOutTagLen /256;
				pucFailedTagListOut[1] = iOutTagLen % 256;
								
				break;
			}

			EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "tag: ", pucTlvList + iPos, iTagLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iDataLen = %d\n", iDataLen);
			EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "data: ", pucTlvList + iPos + iTagLen + iLengthLen, iDataLen);

			for ( i=0; g_tMatchList[i].pIndicator != NULL; i++ ) 
			{
				if ( 0 == memcmp(g_tMatchList[i].ucTag, pucTlvList + iPos, iTagLen) ) 
				{
					if ( (unsigned int)iDataLen > g_tMatchList[i].uiBufLen - 1 ) 
					{
						iFailCount++;
						memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
						iOutTagLen += iTagLen;
						pucFailedTagListOut[0] = iOutTagLen /256;
						pucFailedTagListOut[1] = iOutTagLen % 256;
						iFailCount ++;
						break;
					}

					//permission control
					if ( 0 == (DATA_PERMISSION_WRITE & g_tMatchList[i].ucPermission) ) 
					{
						EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "NO DATA_PERMISSION_WRITE \n");
						iFailCount++;
						memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);				
						iOutTagLen += iTagLen;
						pucFailedTagListOut[0] = iOutTagLen /256;
						pucFailedTagListOut[1] = iOutTagLen % 256;
						iFailCount ++;
						break;
					}

					iRet = ValidateAndExecuteConfigurationData(pucTlvList + iPos, pucTlvList + iPos + iTagLen + iLengthLen, iDataLen);
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ValidateAndExecuteConfigurationData=%d \n", iRet);
					if ( iRet != EMV_OK ) 
					{ 
						iFailCount++;
						memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);
						iOutTagLen += iTagLen;
						pucFailedTagListOut[0] = iOutTagLen /256;
						pucFailedTagListOut[1] = iOutTagLen % 256;
						iFailCount ++;
						break;
					}

					memset(g_tMatchList[i].pIndicator, 0, g_tMatchList[i].uiBufLen);
					memcpy(g_tMatchList[i].pIndicator, pucTlvList + iPos + iTagLen + iLengthLen, iDataLen);
					g_tMatchList[i].uiDataLen = iDataLen;
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TAG: %02x %02x, LENGT: %d, value: %s\n", g_tMatchList[i].ucTag[0], g_tMatchList[i].ucTag[1], g_tMatchList[i].uiDataLen, g_tMatchList[i].pIndicator);
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Value: ", g_tMatchList[i].pIndicator, iDataLen);

//					memcpy(pucSuccessTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);
//					iOutTagLen += iTagLen;
//					pucSuccessTagListOut[0] = iOutTagLen /256;
//					pucSuccessTagListOut[1] = iOutTagLen % 256;
					iSuccessCount++;

					break;
				}
			}

			//not found the tag
			if ( g_tMatchList[i].pIndicator == NULL ) 
			{
				iFailCount++;
				memcpy(pucFailedTagListOut + 2 + iOutTagLen, pucTlvList + iPos, iTagLen);
				iOutTagLen += iTagLen;
				pucFailedTagListOut[0] = iOutTagLen /256;
				pucFailedTagListOut[1] = iOutTagLen % 256;
				break;
			}

			iPos = iPos + iTagLen + iLengthLen + iDataLen;

		}
	} 
	else 
	{
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_INVALID_PARAM, EL_PARAM_RET_INVALID_PARAM, __LINE__);   
		return EL_PARAM_RET_INVALID_PARAM;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iOutTagLen = %d\n", iOutTagLen);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "FailedTagList", pucFailedTagListOut+2, iOutTagLen);

//	*puiSuccessTagListLenOut = iOutTagLen + 2;
	if ( (iSuccessCount == 0) || (iFailCount == 0) ) 
	{
		//all failed
		if ( iSuccessCount == 0 || iTagCount == iFailCount)  
		{
			ComPoseInfoMsg(RetToString, EL_PARAM_RET_ALL_FAILED, EL_PARAM_RET_ALL_FAILED, __LINE__);                                                                                                                                                                 ComPoseInfoMsg(RetToString, EL_PARAM_RET_ALL_FAILED, iRet, __LINE__);
			return EL_PARAM_RET_ALL_FAILED;
		}
		//all success
		if ( iFailCount == 0 ) 
		{
			return EMV_OK;
		}
	} 
	else 
	{
		pucFailedTagListOut[0] = iOutTagLen /256;
		pucFailedTagListOut[1] = iOutTagLen % 256;
		*puiFailedTagListLenOut = iOutTagLen + 2;
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_PARTIAL_FAILED, EL_PARAM_RET_PARTIAL_FAILED, __LINE__);
		return EL_PARAM_RET_PARTIAL_FAILED;
	}

	return EMV_OK;
}


int GetData(const unsigned char *pucData, unsigned char *pucTLVListOut, unsigned int *puiTLVListLenOut)
{
	int iRet=-1, iPos=0, iTagLen=0, iDataLen=0, iLengthLen=0, iOutTLVListLen=0;
	unsigned short usTag=0;
	unsigned char ucData[MAX_TLVDATA_LEN]={0}, ucTag[3+1]={0}, ucLen[3+1]={0}, ucConfigurationTag[5+1]= {0};
	int iTagCount=0, iSuccessCount=0, iFailCount=0;
	int i=0;
	unsigned char ucDataType=0;
	const unsigned char *pucTagList=NULL;
	unsigned int uiTagListLen=0;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Begin ----------------\n");
	if ( (pucData == NULL) || (pucTLVListOut == NULL) || (puiTLVListLenOut == NULL) ) 
	{
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_INVALID_PARAM, EL_PARAM_RET_INVALID_PARAM, __LINE__);
		return EL_PARAM_RET_INVALID_PARAM;
	}

	//parser income data.
	ucDataType = pucData[0];
	uiTagListLen = pucData[1] * 256 + pucData[2];
	pucTagList = pucData + 3;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucDataType = %02x, uiTagListLen=%d\n", ucDataType, uiTagListLen);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucTagList: ", pucTagList, uiTagListLen);

	//added by kevinliu 20161027
	if ( uiTagListLen == 0 ) 
	{
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_INVALID_PARAM, EL_PARAM_RET_INVALID_PARAM, __LINE__);
		return EL_PARAM_RET_INVALID_PARAM;
	}


	if ( ucDataType == PARAMMGMT_DATATYPE_EMV ) 
	{
		//		while( pucTagList[iPos] && ((unsigned int)iPos <= uiTagListLen) ) {
		while((unsigned int)iPos < uiTagListLen) // Modified by ZhangYuan 2016-10-18
		{
			//judge tag length
			if ( 0x1F == (pucTagList[iPos] & 0x1F) ) 
			{
				// Added by ZhangYuan 2016-10-18 check length
				//modified by Kevin
				if ( (unsigned int)(iPos + 1) > uiTagListLen ) 
				{
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "(iPos + 1) > uiTagListLen, iPos+1=%d, uiTagListLen=%d\n", iPos+1, uiTagListLen);

					iTagLen = 1;
					memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
					memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, "\x00", 1);
					iOutTLVListLen += iTagLen + iLengthLen + iDataLen;

					pucTLVListOut[0] = iOutTLVListLen /256;
					pucTLVListOut[1] = iOutTLVListLen % 256;
					iFailCount ++;
					
					break;
				}
				// Added end

				if ( 0x80 == (pucTagList[iPos + 1] & 0x80) ) 
				{
					iTagLen = 3;
					usTag = 0;
				} 
				else 
				{
					iTagLen = 2;
					usTag = pucTagList[iPos] * 256 + pucTagList[iPos + 1];
				}
			}
			else 
			{
				iTagLen = 1;
				usTag = pucTagList[iPos];
			}

			iTagCount++;
			if ( (unsigned int)(iPos + iTagLen) > uiTagListLen ) 
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "(iPos + iTagLen) > uiTagListLen, iPos+iTagLen=%d, uiTagListLen=%d\n", iPos+iTagLen, uiTagListLen);
				
				memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
				memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, "\x00", 1);
				iOutTLVListLen += iTagLen + iLengthLen + iDataLen;

				pucTLVListOut[0] = iOutTLVListLen /256;
				pucTLVListOut[1] = iOutTLVListLen % 256;
				
				iFailCount++;
				break;
			}

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "tag len = %d\n", iTagLen);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "tag: ", pucTagList + iPos, iTagLen);

#ifndef _WIN32
			if (g_tTransParam.ucCurrentTxnType[0] == TRANS_TYPE_EMV_CONTACT ) 
			{
				iRet = EMVGetTLVData(usTag, ucData, &iDataLen);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVGetTLVData = %d\n", iRet);
			} 
			else if (g_tTransParam.ucCurrentTxnType[0] == TRANS_TYPE_CONTACTLESS && g_tTransParam.ucCurrentCLSSType[0] == CLSS_KERNEL_VISA) 
			{
				iRet = Clss_GetTLVData_Wave(usTag, ucData, &iDataLen);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Clss_GetTLVData_Wave = %d\n", iRet);
			} 
			else if (g_tTransParam.ucCurrentTxnType[0] == TRANS_TYPE_CONTACTLESS && g_tTransParam.ucCurrentCLSSType[0] == CLSS_KERNEL_MC)
			{
				memset(ucTag, 0, sizeof(ucTag));
				memcpy(ucTag, pucTagList + iPos, iTagLen);
				iRet = Clss_GetTLVDataList_MC(ucTag, iTagLen, sizeof(ucData), ucData, &iDataLen);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Clss_GetTLVDataList_MC = %d\n", iRet);
			} 
			else 
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "CLSS_KERNEL_ELSE\n");
				return EL_PARAM_RET_INVALID_PARAM;
			}

			EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "VALUE: ", ucData, iDataLen);

#endif	//#ifndef _WIN32

			if ( iRet == EMV_OK ) 
			{											
				if(IsSensitiveTag(usTag) == 1 && g_tAppParam.ucDataEncryptionType[0] != 0x00) // Added by ZhangYuan, if sensitive data, check if  it needs to be encrypted
				{
					unsigned int 	iTmpLen=0;
					unsigned char 	aucBufTmp[512] = {0};
					
					memset(aucBufTmp, 0x00, sizeof(aucBufTmp));	
					iRet = PedEncryptData(ucData, iDataLen, aucBufTmp, &iTmpLen);
					if(iRet != EL_RET_OK)
					{
						ComPoseInfoMsg(RetToString, EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED, iRet, __LINE__);
						return EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED;
					}

					iDataLen = iTmpLen;
					memset(ucData, 0, sizeof(ucData));
					memcpy(ucData, aucBufTmp, iTmpLen);
				}
				
				//calculate length field length
				if ( iDataLen > 0x7F ) 
				{
					iLengthLen = iDataLen / 0xFF + 1 + 1;
					ucLen[0] = 0x80 | (iDataLen / 0xFF);
					for ( i=1; i < iLengthLen; i++ ) 
					{
						if ( i == iLengthLen - 1 ) 
						{
							ucLen[i] = iDataLen & 0xFF;
						} 
						else 
						{
							ucLen[i] = (iDataLen >> (iLengthLen - 1 - i)) * 8;
						}
					}
				} 
				else 
				{
					iLengthLen = 1;
					ucLen[0] = iDataLen;
				}

				memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
				memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, ucLen, iLengthLen);
				memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen + iLengthLen, ucData, iDataLen);
				iOutTLVListLen += iTagLen + iLengthLen + iDataLen;

				pucTLVListOut[0] = iOutTLVListLen /256;
				pucTLVListOut[1] = iOutTLVListLen % 256;

				iSuccessCount++;
			} 
			else 
			{ 
				if ( iRet == CLSS_API_ORDER_ERR ) 
				{
					ComPoseInfoMsg(RetToString, CLSS_API_ORDER_ERR, CLSS_API_ORDER_ERR, __LINE__);
					return EL_PARAM_RET_API_ORDER_ERR;
				}
				iFailCount++;
				
				memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
				memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, "\x00", 1);
				iOutTLVListLen += iTagLen + iLengthLen + iDataLen;

				pucTLVListOut[0] = iOutTLVListLen /256;
				pucTLVListOut[1] = iOutTLVListLen % 256;
			}
			iPos += iTagLen;
		}
	} 
	else if ( ucDataType == PARAMMGMT_DATATYPE_CONFIG ) 
	{

		//		while ( pucTagList[iPos] && ((unsigned int)iPos <= uiTagListLen) ) {
		while((unsigned int)iPos < uiTagListLen) // Modified by ZhangYuan 2016-10-18
		{ 
			iTagLen = CONFIGURATION_TAG_LENGTH;
			memcpy(ucConfigurationTag, pucTagList + iPos, iTagLen);

			iTagCount++;
			for ( i=0; g_tMatchList[i].pIndicator != NULL; i++ ) 
			{
				if ( 0 == memcmp(g_tMatchList[i].ucTag, pucTagList + iPos, iTagLen) ) 
				{		
					//permission control
					if ( 0 == (DATA_PERMISSION_READ & g_tMatchList[i].ucPermission) ) 
					{
						EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "NO DATA_PERMISSION_READ \n");

						memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
						memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, "\x00", 1);
						iOutTLVListLen += iTagLen + 1;

						pucTLVListOut[0] = iOutTLVListLen /256;
						pucTLVListOut[1] = iOutTLVListLen % 256;
						
						iFailCount++;
						break;
					}

					iRet = GetSpecifyConfigurationData(pucTagList + iPos, iTagLen, g_tMatchList[i].pIndicator, &g_tMatchList[i].uiDataLen);
					if ( iRet != EMV_OK ) 
					{
						memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
						memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, "\x00", 1);
						iOutTLVListLen += iTagLen + 1;

						pucTLVListOut[0] = iOutTLVListLen /256;
						pucTLVListOut[1] = iOutTLVListLen % 256;
						
						iFailCount++;
						break;
					}					

					memcpy(ucData, g_tMatchList[i].pIndicator, g_tMatchList[i].uiDataLen);
					iDataLen = g_tMatchList[i].uiDataLen;

					usTag = g_tMatchList[i].ucTag[0] * 256 + g_tMatchList[i].ucTag[1];
					if(IsSensitiveTag(usTag) == 1 && g_tAppParam.ucDataEncryptionType[0] != 0x00) // Added by ZhangYuan, if sensitive data, check if  it needs to be encrypted
					{
						unsigned int 	iTmpLen=0;
						unsigned char 	aucBufTmp[512] = {0};
						
						memset(aucBufTmp, 0x00, sizeof(aucBufTmp));	
						iRet = PedEncryptData(ucData, iDataLen, aucBufTmp, &iTmpLen);
						if(iRet != EL_RET_OK)
						{
							ComPoseInfoMsg(RetToString, EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED, iRet, __LINE__);
							return EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED;
						}

						iDataLen = iTmpLen;
						memset(ucData, 0, sizeof(ucData));
						memcpy(ucData, aucBufTmp, iTmpLen);
					}

					//calculate length field length
					if ( iDataLen > 0x7F ) 
					{
						if ( (iDataLen % 0xFF) == 0 ) 
						{
							iLengthLen = iDataLen / 0xFF + 1;
						} 
						else 
						{
							iLengthLen = (iDataLen / 0xFF + 1) + 1;
						}
						ucLen[0] = 0x80 | (iLengthLen - 1);
						for ( i=1; i < iLengthLen; i++ ) 
						{
							if ( i == iLengthLen - 1 ) 
							{
								ucLen[i] = iDataLen & 0xFF;
							} 
							else 
							{
								ucLen[i] = (iDataLen >> (iLengthLen - 1 - i)) * 8;
							}
						}
					} 
					else 
					{
						iLengthLen = 1;
						ucLen[0] = iDataLen;
					}

					memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
					memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, ucLen, iLengthLen);
					memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen + iLengthLen, ucData, iDataLen);
					iOutTLVListLen += iTagLen + iLengthLen + iDataLen;

					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iTagLen: %d, iLengthLen: %d, iDataLen: %d\n", iTagLen, iLengthLen, iDataLen);
					EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TAG", pucTagList + iPos, iTagLen);					
					EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucLen", ucLen, iLengthLen);
					EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Value", ucData, iDataLen);
					
					pucTLVListOut[0] = iOutTLVListLen /256;
					pucTLVListOut[1] = iOutTLVListLen % 256;

					iSuccessCount++;

					break;
				}
			}

			//not found the tag
			if ( g_tMatchList[i].pIndicator == NULL ) 
			{
				memcpy(pucTLVListOut + 2 + iOutTLVListLen, pucTagList + iPos, iTagLen);
				memcpy(pucTLVListOut + 2 + iOutTLVListLen + iTagLen, "\x00", 1);
				iOutTLVListLen += iTagLen + 1;

				pucTLVListOut[0] = iOutTLVListLen /256;
				pucTLVListOut[1] = iOutTLVListLen % 256;

				iFailCount++;
				break;
			}

			iPos += iTagLen;

		}
	}
	else 
	{
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_INVALID_PARAM, EL_PARAM_RET_INVALID_PARAM, __LINE__);
		return EL_PARAM_RET_INVALID_PARAM;
	}

	*puiTLVListLenOut = iOutTLVListLen + 2;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "puiTLVListLenOut = %d\n", *puiTLVListLenOut);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucTLVListOut: ", pucTLVListOut + 2, *puiTLVListLenOut);

	if ( (iSuccessCount == 0) || (iFailCount == 0) ) 
	{
		//all failed
		if ( iSuccessCount == 0 || iTagCount == iFailCount) 
		{
			ComPoseInfoMsg(RetToString, EL_PARAM_RET_ALL_FAILED, EL_PARAM_RET_ALL_FAILED, __LINE__);
			return EL_PARAM_RET_ALL_FAILED;
		}
		//all success
		if ( iFailCount == 0 ) 
		{
			return EMV_OK;
		}
	} 
	else 
	{
		ComPoseInfoMsg(RetToString, EL_PARAM_RET_PARTIAL_FAILED, EL_PARAM_RET_PARTIAL_FAILED, __LINE__);
		return EL_PARAM_RET_PARTIAL_FAILED;
	}

	return EMV_OK;
}


