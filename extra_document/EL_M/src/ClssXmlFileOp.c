/*
 * ============================================================================
 * COPYRIGHT
 *              Pax CORPORATION PROPRIETARY INFORMATION
 *   This software is supplied under the terms of a license agreement or
 *   nondisclosure agreement with Pax Corporation and may not be copied
 *   or disclosed except in accordance with the terms in that agreement.
 *      Copyright (C) 2016 Pax Corporation. All rights reserved.
 * Module Date: 2016/10/10    
 * Module Auth: ZhangYuan 
 * Description:   EMV contactless xml file operarte module

 *      
 * ============================================================================
 */
#include "..\inc\global.h"

#define   LOG_TAG   "CLSS_PARSE"
#ifdef CLSS_PARSE_DEBUG		

#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr,...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine,pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)


#endif // CLSS_PARSE_DEBUG


//#define  TAG   "CLSS_PARSE"
//#define CLSS_TEST

static int giAidIndex = 0;
static int giPayPassConfIndex = 0;


static int giPayWaveAidIndex = 0;
static int giPayWaveInterWareIndex = 0;
static int giPayWavePrgIdIndex = 0;
static int giPayWaveTermCapIndex = 0;
static int giClssOhterConfIndex = 0;
static int giClssExpressPayAidIndex = 0;
static int giClssExpressPayConfigIndex = 0;

static int giCurrentOffset = 0;

static int giParsePos = 0;

extern uchar RecvBuf[2048];

static NODE_RECORD_T gstNoeRecord[] = 
{
		{"PAYPASS_AID",0,0},
		{"PAYPASS_PAYPASSCONFIGURATION",0,0},
		{"PAYWAVE_AID",0,0},
		{"PAYWAVE_INTER_WARE",0,0},
		{"PAYWAVE_PROGRAMID",0,0},
		{"PAYWAVE_TERM_CAP",0,0},
		{CLSS_EXPRESSPAY_AID_NODE,0,0},
		{CLSS_EXPRESSPAY_CONFIG_NODE,0,0},
		{"CLSS_OHTER_CONF",0,0},
};

#define XML_NODE_NAME_MAX_LEN 32
unsigned char aucExpressPayAidNode[3][XML_NODE_NAME_MAX_LEN] = {
	"EXPRESSPAYPARAM",
	"AIDLIST",
	"AID",
};

unsigned char aucExpressPayConfigNode[][XML_NODE_NAME_MAX_LEN] = {
	"EXPRESSPAYCONFIGURATION",
};

// Added end

static int ClssGetRecordIndex(const uchar *pszNodeName, int iSize)
{
	int i = 0;

	for(i = 0; i < iSize; i++)
	{
		if(strcmp(pszNodeName, gstNoeRecord[i].ucListNodeName) == 0)
		{
			return i;
		}
	}
	return CLSS_XML_FILE_OP_ERR_RECORD_NOT_FOUNTD;
}

static int ClssGetRecordData(NODE_RECORD_T *pststRecord, int iLen)
{
	int iFd = 0;
	int iFileSize = 0;
	int iHavaReadLen = 0;
	int iRet = 0;
	int i = 0;

	iRet = PdkFile_Exist(CLSS_XML_FILE_HAVE_PARSED);
	if(iRet < 0 )
	{
		return iRet;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	while(iHavaReadLen < iLen)
	{
		memset(pststRecord, 0, iLen);
		iRet = PdkFile_Read(iFd, (uchar*)pststRecord, iLen);
		iHavaReadLen += iRet;
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			return iRet;
		}
	}
//	for(i = 0 ; i < sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]); i++ )
//	{
//		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d] = %d, iStartPos[%d] = %d\n",i,pststRecord[i].count,i,pststRecord[i].iStartPos);
//		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d] = %s\n",pststRecord[i].ucListNodeName);
//	}
	PdkFile_Close(iFd);
	return 0;
}


int ClssGetNodeNum(const uchar *pszName)
{
	int iIndex = 0;
	int iRet = 0;
	
	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};
	
	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex(pszName, sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
		return iIndex;
	}

	return stRecord[iIndex].count;

}

int GetClssNodeData(void *pvClssNodeData, int iSize, int iAidIndex, uchar *pszRecordName)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex(pszRecordName, sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));

	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"Index = %d\n",iIndex);
		return iIndex;
	}

	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iAidIndex > stRecord[iIndex].count)
	{
		return CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iAidIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
		
	iRet = PdkFile_Read(iFd, (uchar*)pvClssNodeData, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	PdkFile_Close(iFd);

	return 0;
}


int GetPayPassAidData(PAYPASS_AID_NODE_T *pstPayPassAidNode, int iSize, int iAidIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYPASS_AID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
		return iIndex;
	}


//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iAidIndex > stRecord[iIndex].count)
	{
		return CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iAidIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstPayPassAidNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

#ifdef CLSS_TEST
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stAidNode.SelFlag = %d\n",pstPayPassAidNode->ucSelFlag);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.AID = ", pstPayPassAidNode->aucAID, sizeof(pstPayPassAidNode->aucAID));
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stAidNode.IfUseLocalName = %d\n",pstPayPassAidNode->ucIfUseLocalName);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stAidNode.AppName = %s\n",pstPayPassAidNode->aucAppName);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.Version = ", pstPayPassAidNode->aucVersion, sizeof(pstPayPassAidNode->aucVersion));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.aucMagAppVersion = ", pstPayPassAidNode->aucMagAppVersion, sizeof(pstPayPassAidNode->aucMagAppVersion));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.TACDenial = ", pstPayPassAidNode->aucTACDenial, sizeof(pstPayPassAidNode->aucTACDenial));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.TACOnline = ", pstPayPassAidNode->aucTACOnline, sizeof(pstPayPassAidNode->aucTACOnline));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.TACDefault = ", pstPayPassAidNode->aucTACDefault, sizeof(pstPayPassAidNode->aucTACDefault));
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stAidNode.ulClssFloorLimit = %d\n",pstPayPassAidNode->ulClssFloorLimit);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.aucTermRisk = ", pstPayPassAidNode->aucTermRisk, sizeof(pstPayPassAidNode->aucTermRisk));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.aucTxnLmt_NoOnDevice = ", pstPayPassAidNode->aucTxnLmt_NoOnDevice, sizeof(pstPayPassAidNode->aucTxnLmt_NoOnDevice));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.aucTxnLmt_OnDevice = ", pstPayPassAidNode->aucTxnLmt_OnDevice, sizeof(pstPayPassAidNode->aucTxnLmt_OnDevice));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassAidData---- stAidNode.aucRDCVMLmt = ", pstPayPassAidNode->aucRDCVMLmt, sizeof(pstPayPassAidNode->aucRDCVMLmt));
#endif

	PdkFile_Close(iFd);

	return 0;
}

#ifdef CLSS_TEST
int TestGetPayPassAid()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	PAYPASS_AID_NODE_T stPayPassAidNode = {0};

	iNum = ClssGetNodeNum(CLSS_PAYPASS_AID_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stPayPassAidNode, 0, sizeof(stPayPassAidNode));
		iRet = GetPayPassAidData(&stPayPassAidNode, sizeof(stPayPassAidNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif

int GetPayPassConfData(PAYPASS_CONF_T *pstPayPassConfNode, int iSize, int iConfIndex) 
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYPASS_PAYPASSCONFIGURATION", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));

	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
	if(iIndex < 0)
	{
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d, stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iConfIndex > stRecord[iIndex].count)
	{
		return CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iConfIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
			
	iRet = PdkFile_Read(iFd, (uchar*)pstPayPassConfNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

#ifdef CLSS_TEST
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucKernelConf = ", &(pstPayPassConfNode->ucKernelConf), sizeof(pstPayPassConfNode->ucKernelConf));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucTornLeftTime = ", &(pstPayPassConfNode->ucTornLeftTime), sizeof(pstPayPassConfNode->ucTornLeftTime));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucMaximumTornNumber = ", &(pstPayPassConfNode->ucMaximumTornNumber), sizeof(pstPayPassConfNode->ucMaximumTornNumber));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucMagneticCVM = ", &(pstPayPassConfNode->ucMagneticCVM), sizeof(pstPayPassConfNode->ucMagneticCVM));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucMageticNoCVM = ", &(pstPayPassConfNode->ucMageticNoCVM), sizeof(pstPayPassConfNode->ucMageticNoCVM));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucMobileSupport = ", &(pstPayPassConfNode->ucMobileSupport), sizeof(pstPayPassConfNode->ucMobileSupport));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucCardDataInput = ", &(pstPayPassConfNode->ucCardDataInput), sizeof(pstPayPassConfNode->ucCardDataInput));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucCVMCapability_CVMRequired = ", &(pstPayPassConfNode->ucCVMCapability_CVMRequired), sizeof(pstPayPassConfNode->ucCVMCapability_CVMRequired));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucCVMCapability_NoCVMRequired = ", &(pstPayPassConfNode->ucCVMCapability_NoCVMRequired), sizeof(pstPayPassConfNode->ucCVMCapability_NoCVMRequired));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucTerminalType = ", &(pstPayPassConfNode->ucTerminalType), sizeof(pstPayPassConfNode->ucTerminalType));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucAccoutType = ", &(pstPayPassConfNode->ucAccoutType), sizeof(pstPayPassConfNode->ucAccoutType));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.aucAdditionalTerminalCapability = ", pstPayPassConfNode->aucAdditionalTerminalCapability, sizeof(pstPayPassConfNode->aucAdditionalTerminalCapability));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucKernelID = ", &(pstPayPassConfNode->ucKernelID), sizeof(pstPayPassConfNode->ucKernelID));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayPassConfData ---- pstConfNode.ucSecurityCapability = ", &(pstPayPassConfNode->ucSecurityCapability), sizeof(pstPayPassConfNode->ucSecurityCapability));
#endif

	PdkFile_Close(iFd);

	return 0;
}

#ifdef CLSS_TEST
int TestGetPayPassConf()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	PAYPASS_CONF_T stPayPassConfNode = {0};

	iNum = ClssGetNodeNum(CLSS_PAYPASS_CONF_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stPayPassConfNode, 0, sizeof(stPayPassConfNode));
		iRet = GetPayPassConfData(&stPayPassConfNode, sizeof(stPayPassConfNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif

int GetPayWaveAidData(PAYWAVE_AID_NODE_T *pstPayWaveAidNode, int iSize, int iPayWaveAidIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYWAVE_AID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,GetRecordData stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iPayWaveAidIndex > stRecord[iIndex].count)
	{
		return CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iPayWaveAidIndex*iSize;
	
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstPayWaveAidNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

#ifdef CLSS_TEST
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stAidNode.ucSelFlag = %d\n",pstPayWaveAidNode->ucSelFlag);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveAidData ---- stAidNode.aucAID = ", pstPayWaveAidNode->aucAID, sizeof(pstPayWaveAidNode->aucAID));
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stAidNode.ucIfUseLocalName = %d\n",pstPayWaveAidNode->ucIfUseLocalName);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stAidNode.aucAppName = %s\n",pstPayWaveAidNode->aucAppName);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveAidData ---- stAidNode.aucAppVersion = ", pstPayWaveAidNode->aucAppVersion, sizeof(pstPayWaveAidNode->aucAppVersion));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveAidData ---- stAidNode.ucCrypto17Flg = ", &(pstPayWaveAidNode->ucCrypto17Flg), sizeof(pstPayWaveAidNode->ucCrypto17Flg));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveAidData ---- stAidNode.ucZeroAmtNoAllowed =", &(pstPayWaveAidNode->ucZeroAmtNoAllowed), sizeof(pstPayWaveAidNode->ucZeroAmtNoAllowed));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveAidData ---- stAidNode.ucStatusCheckFlg = ", &(pstPayWaveAidNode->ucStatusCheckFlg), sizeof(pstPayWaveAidNode->ucStatusCheckFlg));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveAidData ---- stAidNode.aucReaderTTQ = ", pstPayWaveAidNode->aucReaderTTQ, sizeof(pstPayWaveAidNode->aucReaderTTQ));

	for(i = 0; i < MAX_TRANSTYPE_NUM; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- i = %d \n",i);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveAidData ParseInterWare---- stAidNode.ucTransType = ", &(pstPayWaveAidNode->stFlmtByTransType[i].ucTransType), sizeof(pstPayWaveAidNode->stFlmtByTransType[i].ucTransType));
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.ucTermFLmtFlg = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ucTermFLmtFlg);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.ucRdClssTxnLmtFlg = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ucRdClssTxnLmtFlg);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.ucRdCVMLmtFlg = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ucRdCVMLmtFlg);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.ucRdClssFLmtFlg = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ucRdClssFLmtFlg);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.aucTermFLmt = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ulTermFLmt);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.aucRdClssTxnLmt = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ulcRdClssTxnLmt);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.aucRdCVMLmt = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ulRdCVMLmt);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ParseInterWare---- stAidNode.aucRdClssFLmt = %d\n",pstPayWaveAidNode->stFlmtByTransType[i].ulRdClssFLmt);
	}
#endif

	PdkFile_Close(iFd);

	return 0;
}

#ifdef CLSS_TEST
int TestGetPayWaveAid()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	PAYWAVE_AID_NODE_T stPayWaviAidNode = {0};

	iNum = ClssGetNodeNum(CLSS_PAYWAVE_AID_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stPayWaviAidNode, 0, sizeof(stPayWaviAidNode));
		iRet = GetPayWaveAidData(&stPayWaviAidNode, sizeof(stPayWaviAidNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif


int GetPayWaveProgramIdData(PAYWAVE_PROGREAM_ID_NODE_T *pstPayWavePrgIdNode, int iSize, int iPrgIdIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYWAVE_PROGRAMID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));

	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
		return iIndex;
	}
//
//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iPrgIdIndex > stRecord[iIndex].count)
	{
		return CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);

	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iPrgIdIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstPayWavePrgIdNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

#ifdef CLSS_TEST
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveProgramIdData ---- pstPayWavePrgIdNode.aucProgramId = ", pstPayWavePrgIdNode->aucProgramId, sizeof(pstPayWavePrgIdNode->aucProgramId));
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucPrgramIdLen = %d\n",pstPayWavePrgIdNode->ucPrgramIdLen);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ulTerminalFloorLimit = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ulTermFLmt);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ulContactlessCVMLimit = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ulRdCVMLmt);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ulContactlessTransactionLimit = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ulRdClssTxnLmt);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ulContactlessFloorLimit = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ulRdClssFLmt);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucTerminalFloorLimitSupported = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ucTermFLmtFlg);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucContactlessTransactionLimitSupported = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ucRdClssTxnLmtFlg);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucCVMLimitSupported = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ucRdCVMLmtFlg);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucContactlessFloorLimitSupported = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ucRdClssFLmtFlg);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucCryptogramVersion17Supported = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ucCrypto17Flg);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucZeroAmountNoAllowed = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ucZeroAmtNoAllowed);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstPayWavePrgIdNode.ucStatusCheckSupported = %d\n",pstPayWavePrgIdNode->tPreProcDRL.ucStatusCheckFlg);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveProgramIdData ---- pstPayWavePrgIdNode.aucReaderTTQ = ", pstPayWavePrgIdNode->tPreProcDRL.aucReaderTTQ, sizeof(pstPayWavePrgIdNode->tPreProcDRL.aucReaderTTQ));
#endif

	PdkFile_Close(iFd);

	return 0;
}

#ifdef CLSS_TEST
int TestGetPayWavePrgId()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	PAYWAVE_PROGREAM_ID_NODE_T stPayWavePrgIdNode = {0};

	iNum = ClssGetNodeNum(CLSS_PAYWAVE_PROGRAM_ID_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stPayWavePrgIdNode, 0, sizeof(stPayWavePrgIdNode));
		iRet = GetPayWaveProgramIdData(&stPayWavePrgIdNode, sizeof(stPayWavePrgIdNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif

int GetPayWaveTermCapData(PAYWAVE_TERM_CAPABILITY_NODE_T *pstPayWaveTermCapNode, int iSize, int iTermCapIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYWAVE_TERM_CAP", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iTermCapIndex > stRecord[iIndex].count)
	{
		return CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iTermCapIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstPayWaveTermCapNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

#ifdef CLSS_TEST
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveTermCapData ---- pstPayWaveTermCapNode.ucTermType = ", &(pstPayWaveTermCapNode->ucTermType), sizeof(pstPayWaveTermCapNode->ucTermType));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveTermCapData ---- pstPayWaveTermCapNode.ucCardDataInput = ", &(pstPayWaveTermCapNode->ucCardDataInput), sizeof(pstPayWaveTermCapNode->ucCardDataInput));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveTermCapData ---- pstPayWaveTermCapNode.ucCVMSupport = ", &(pstPayWaveTermCapNode->ucCVMSupport), sizeof(pstPayWaveTermCapNode->ucCVMSupport));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveTermCapData ---- pstPayWaveTermCapNode.ucOffDataAuth = ", &(pstPayWaveTermCapNode->ucOffDataAuth), sizeof(pstPayWaveTermCapNode->ucOffDataAuth));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveTermCapData ---- pstPayWaveTermCapNode.ucTransTypeCapa = ", &(pstPayWaveTermCapNode->ucTransTypeCapa), sizeof(pstPayWaveTermCapNode->ucTransTypeCapa));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveTermCapData ---- pstPayWaveTermCapNode.ucTermInput = ", &(pstPayWaveTermCapNode->ucTermInput), sizeof(pstPayWaveTermCapNode->ucTermInput));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetPayWaveTermCapData ---- pstPayWaveTermCapNode.ucTermOutput = ", &(pstPayWaveTermCapNode->ucTermOutput), sizeof(pstPayWaveTermCapNode->ucTermOutput));
#endif

	PdkFile_Close(iFd);

	return 0;
}

#ifdef CLSS_TEST
int TestGetPayWaveTermCap()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	PAYWAVE_TERM_CAPABILITY_NODE_T stPayWaveTermCapNode = {0};

	iNum  = ClssGetNodeNum(CLSS_PAYWAVE_TERM_CAP_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stPayWaveTermCapNode, 0, sizeof(stPayWaveTermCapNode));
		iRet = GetPayWaveTermCapData(&stPayWaveTermCapNode, sizeof(stPayWaveTermCapNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif


int GetClssOtherCofData(CLSS_OTHER_CONF_NODE_T *pstOtherConfNode, int iSize, int iOtherCofIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[CLSS_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = ClssGetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("CLSS_OHTER_CONF", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iOtherCofIndex > stRecord[iIndex].count)
	{
		return CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(CLSS_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(CLSS_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iOtherCofIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstOtherConfNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

#ifdef CLSS_TEST
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetClssOtherCofData ---- pstOtherConfNode.aucCountryCode = ", pstOtherConfNode->aucCountryCode, sizeof(pstOtherConfNode->aucCountryCode));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetClssOtherCofData ---- pstOtherConfNode.aucCurrencyCode = ", pstOtherConfNode->aucCurrencyCode, sizeof(pstOtherConfNode->aucCurrencyCode));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetClssOtherCofData ---- pstOtherConfNode.aucRefCurrencyCode = ", pstOtherConfNode->aucRefCurrencyCode, sizeof(pstOtherConfNode->aucRefCurrencyCode));
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstOtherConfNode->pstOtherConfNode = %d\n", pstOtherConfNode->ucCurrencyExponent);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstOtherConfNode->ucRefCurrencyExponent = %d\n", pstOtherConfNode->ucRefCurrencyExponent);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetClssOtherCofData ---- pstOtherConfNode.aucRefCurrenceConVerRate = ", pstOtherConfNode->aucRefCurrenceConVerRate, sizeof(pstOtherConfNode->aucRefCurrenceConVerRate));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetClssOtherCofData ---- pstOtherConfNode.ucTransTypeCapa = ", pstOtherConfNode->aucMerchantCateCode, sizeof(pstOtherConfNode->aucMerchantCateCode));
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstOtherConfNode->aucMerchantId = %s\n", pstOtherConfNode->aucMerchantId);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstOtherConfNode->aucTerminalId = %s\n", pstOtherConfNode->aucTerminalId);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstOtherConfNode->aucMerChantName = %s\n", pstOtherConfNode->aucMerChantName);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"pstOtherConfNode->aucMerChantLocalAddr = %s\n", pstOtherConfNode->aucMerChantLocalAddr);
#endif

	PdkFile_Close(iFd);

	return 0;
}

#ifdef CLSS_TEST
int TestGetOherConf()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	CLSS_OTHER_CONF_NODE_T stClssOtherConfNode = {0};

	iNum = ClssGetNodeNum(CLSS_OTHER_CONF_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stClssOtherConfNode, 0, sizeof(stClssOtherConfNode));
		iRet = GetClssOtherCofData(&stClssOtherConfNode, sizeof(stClssOtherConfNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif

int GetExpressPayAidData(EXPRESSPAY_AID_NODE_T *pstExpressPayAidNode, int iIdx)
{
	int iRet = 0;
	
	iRet = GetClssNodeData(pstExpressPayAidNode, sizeof(EXPRESSPAY_AID_NODE_T), iIdx, CLSS_EXPRESSPAY_AID_NODE);

	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"GetClssNodeData iRet = %d\n",iRet);
#ifdef CLSS_TEST
	if(iRet == 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ExpressPay, ucSelFlag=%02x\n",pstExpressPayAidNode->ucSelFlag);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucAID: ", pstExpressPayAidNode->aucAID, sizeof(pstExpressPayAidNode->aucAID));
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ExpressPay, ucAidLen=%02x\n",pstExpressPayAidNode->ucAidLen);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ExpressPay, ucIfUseLocalName=%02x\n",pstExpressPayAidNode->ucIfUseLocalName);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ExpressPay, aucAppName=%s\n",pstExpressPayAidNode->aucAppName);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucVersion: ", pstExpressPayAidNode->aucVersion, sizeof(pstExpressPayAidNode->aucVersion));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucTACDenial: ", pstExpressPayAidNode->aucTACDenial, sizeof(pstExpressPayAidNode->aucTACDenial));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucTACOnline: ", pstExpressPayAidNode->aucTACOnline, sizeof(pstExpressPayAidNode->aucTACOnline));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucTACDefault: ", pstExpressPayAidNode->aucTACDefault, sizeof(pstExpressPayAidNode->aucTACDefault));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucDDOL: ", pstExpressPayAidNode->aucDDOL, sizeof(pstExpressPayAidNode->aucDDOL));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucTDOL: ", pstExpressPayAidNode->aucTDOL, sizeof(pstExpressPayAidNode->aucTDOL));
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ExpressPay, aucTerminalCapability=%02x\n",pstExpressPayAidNode->aucTerminalCapability);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucClssCVMLimit: ", pstExpressPayAidNode->aucClssCVMLimit, sizeof(pstExpressPayAidNode->aucClssCVMLimit));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucClssTransLimit: ", pstExpressPayAidNode->aucClssTransLimit, sizeof(pstExpressPayAidNode->aucClssTransLimit));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayAidData] ExpressPay, aucClssFloorLimit: ", pstExpressPayAidNode->aucClssFloorLimit, sizeof(pstExpressPayAidNode->aucClssFloorLimit));

	}
#endif

	return iRet;
}

int GetExpressPayConfigData(EXPRESSPAY_CONFIG_NODE_T *pstExpressPayConfigNode, int iIdx)
{
	int iRet = 0;
	
	iRet = GetClssNodeData(pstExpressPayConfigNode, sizeof(EXPRESSPAY_CONFIG_NODE_T), iIdx, CLSS_EXPRESSPAY_CONFIG_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"GetClssNodeData iRet = %d\n",iRet);

#ifdef CLSS_TEST
	if(iRet == 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ExpressPay, ucTmSupOptTrans=%02x\n", pstExpressPayConfigNode->ucTmSupOptTrans);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayConfigData] ExpressPay, aucUNRange: ", pstExpressPayConfigNode->aucUNRange, sizeof(pstExpressPayConfigNode->aucUNRange));
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[GetExpressPayConfigData] ExpressPay, aucTmTransCapa: ", pstExpressPayConfigNode->aucTmTransCapa, sizeof(pstExpressPayConfigNode->aucTmTransCapa));
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"ExpressPay, ucDelayAuthFlag=%02x\n", pstExpressPayConfigNode->ucDelayAuthFlag);
	}
#endif

	return iRet;
}



#ifdef CLSS_TEST
int TestGetExpressPayAid()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	EXPRESSPAY_AID_NODE_T stExpressPayAidNode = {0};

	iNum = ClssGetNodeNum(CLSS_EXPRESSPAY_AID_NODE);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);

	for(i = 0; i < iNum; i++)
	{
		memset(&stExpressPayAidNode, 0, sizeof(stExpressPayAidNode));
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		iRet = GetExpressPayAidData(&stExpressPayAidNode, i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d, i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}

int TestGetExpressPayConfig()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	EXPRESSPAY_CONFIG_NODE_T stExpressPayConfigNode = {0};

	iNum = ClssGetNodeNum(CLSS_EXPRESSPAY_CONFIG_NODE);
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stExpressPayConfigNode, 0, sizeof(stExpressPayConfigNode));
		iRet = GetExpressPayConfigData(&stExpressPayConfigNode, i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d, i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}

#endif


static int SaveNodeStruct(const uchar *pszNodeName, int iClssType, void *data, int iLen)
{
	int iOffset = 0;
	int iBaseOffset = 0;
	//NODE_RECORD_T stRecord[CLSS_PRASED_NODE_CNT] = {0};
	int iRet = 0;
	int iSaveLen = 0;
	int i = 0;

	if((strcmp(pszNodeName, "<AID>") == 0) && (iClssType ==CLSS_PAYPASS))
	{
		PAYPASS_AID_NODE_T *stAidNode = NULL;
		stAidNode = (PAYPASS_AID_NODE_T *)data;

		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," giAidIndex = %d, giCurrentOffset = %d\n",giAidIndex,giCurrentOffset);

		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)stAidNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giAidIndex == 0)
		{
			iRet = ClssGetRecordIndex("PAYPASS_AID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));

			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giAidIndex++;
	}
	else if((strcmp(pszNodeName, "<PAYPASSCONFIGURATION>") == 0) && (iClssType ==CLSS_PAYPASS))
	{
		PAYPASS_CONF_T *pstConfNode = NULL;
		pstConfNode = (PAYPASS_CONF_T *)data;

		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," giPayPassConfIndex = %d, giCurrentOffset = %d\n",giPayPassConfIndex,giCurrentOffset);

		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)pstConfNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giPayPassConfIndex == 0)
		{
			iRet = ClssGetRecordIndex("PAYPASS_PAYPASSCONFIGURATION", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giPayPassConfIndex++;
	}
	else if((strcmp(pszNodeName, "<AID>") == 0) && (iClssType ==CLSS_PAYWAVE))
	{
		PAYWAVE_AID_NODE_T *stAidNode = NULL;
		stAidNode = (PAYWAVE_AID_NODE_T *)data;

		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," giPayWaveAidIndex = %d, giPayWaveInterWareIndex = %d, giCurrentOffset = %d\n",giPayWaveAidIndex,giPayWaveInterWareIndex, giCurrentOffset);
		
		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)stAidNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giPayWaveAidIndex == 0)
		{
			iRet = ClssGetRecordIndex("PAYWAVE_AID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giPayWaveAidIndex++;
	}
	else if((strcmp(pszNodeName, "<PROGRAMID>") == 0) && (iClssType ==CLSS_PAYWAVE))
	{
		PAYWAVE_PROGREAM_ID_NODE_T *pstPrgIdNode = NULL;
		pstPrgIdNode = (PAYWAVE_PROGREAM_ID_NODE_T *)data;

		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," giPayWavePrgIdIndex = %d, giCurrentOffset = %d\n",giPayWavePrgIdIndex,giCurrentOffset);

		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)pstPrgIdNode, iLen, giCurrentOffset);

		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giPayWavePrgIdIndex == 0)
		{
			iRet = ClssGetRecordIndex("PAYWAVE_PROGRAMID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giPayWavePrgIdIndex++;
	}
	else if((strcmp(pszNodeName, "<TERMINALCAPABILITIES>") == 0) && (iClssType ==CLSS_PAYWAVE))
	{
		PAYWAVE_TERM_CAPABILITY_NODE_T *pstTermCapNode = NULL;
		pstTermCapNode = (PAYWAVE_TERM_CAPABILITY_NODE_T *)data;

		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," giPayWaveTermCapIndex = %d, giCurrentOffset = %d\n",giPayWaveTermCapIndex,giCurrentOffset);
		
		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)pstTermCapNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giPayWaveTermCapIndex == 0)
		{
			iRet = ClssGetRecordIndex("PAYWAVE_TERM_CAP", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giPayWaveTermCapIndex++;
	}
	else if((strcmp(pszNodeName, "<AID>") == 0) && (iClssType == CLSS_EXPRESSPAY)) // Added by ZhangYuan 2017-03-27
	{
		EXPRESSPAY_AID_NODE_T *pstExpressPayAidNode = NULL;
		pstExpressPayAidNode = (EXPRESSPAY_AID_NODE_T *)data;

		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)pstExpressPayAidNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giClssExpressPayAidIndex == 0)
		{
			iRet = ClssGetRecordIndex(CLSS_EXPRESSPAY_AID_NODE, sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giClssExpressPayAidIndex++;
	}
	else if((strcmp(pszNodeName, "<EXPRESSPAYCONFIGURATION>") == 0) && (iClssType == CLSS_EXPRESSPAY)) // Added by ZhangYuan 2017-03-27
	{
		EXPRESSPAY_CONFIG_NODE_T *pstExpressPayConfigNode = NULL;
		pstExpressPayConfigNode = (EXPRESSPAY_CONFIG_NODE_T *)data;

		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)pstExpressPayConfigNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giClssExpressPayConfigIndex == 0)
		{
			iRet = ClssGetRecordIndex(CLSS_EXPRESSPAY_CONFIG_NODE, sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giClssExpressPayConfigIndex++;
	}
	else if((strcmp(pszNodeName, "<OTHERCONFIGURATION>") == 0) && (iClssType ==CLSS_COMMON))
	{
		CLSS_OTHER_CONF_NODE_T *pstOtherConfNode = NULL;
		pstOtherConfNode = (CLSS_OTHER_CONF_NODE_T *)data;

		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," giClssOhterConfIndex = %d, giCurrentOffset = %d\n",giClssOhterConfIndex,giCurrentOffset);

		iSaveLen = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)pstOtherConfNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
	
		if(giClssOhterConfIndex == 0)
		{
			iRet = ClssGetRecordIndex("CLSS_OHTER_CONF", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				return iRet;
			}
			gstNoeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giClssOhterConfIndex++;
	}
	return giCurrentOffset;
}


//begin to parse aid
static int PaypassGetAidChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</AID>") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<PartialAIDSelection") != NULL)
	{
		return PAYPASS_PARTIALAIDSELECTION;
	}
	if(strstr(pszXmlOneLine, "<ApplicationID") != NULL)
	{
		return PAYPASS_APPLICATIONID;
	}
	if(strstr(pszXmlOneLine, "<IfUseLocalAIDName") != NULL)
	{
		return PAYPASS_LOACALAIDNAME;
	}
	if(strstr(pszXmlOneLine, "<LocalAIDName") != NULL)
	{
		return PAYPASS_LOACALAIDNAME;
	}
	if(strstr(pszXmlOneLine, "<TerminalAIDVersion") != NULL)
	{
		return PAYPASS_TERMINALAIDVERSION;
	}
	if(strstr(pszXmlOneLine, "<MagneticApplicationVersionNumber") != NULL)
	{
		return PAYPASS_MAG_APP_VERSION_NUM;
	}
	if(strstr(pszXmlOneLine, "<TACDenial") != NULL)
	{
		return PAYPASS_TACDENIAL;
	}
	if(strstr(pszXmlOneLine, "<TACOnline") != NULL)
	{
		return PAYPASS_TACONLINE;
	}
	if(strstr(pszXmlOneLine, "<TACDefault") != NULL)
	{
		return PAYPASS_TACDEFAULT;
	}
	if(strstr(pszXmlOneLine, "<TerminalRisk") != NULL)
	{
		return PAYPASS_TERMINAL_RISK;
	}
	if(strstr(pszXmlOneLine, "<ContactlessCVMLimit") != NULL)
	{
		return PAYPASS_CLSS_CVM_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimit_NoOnDevice") != NULL)
	{
		return PAYPASS_CLSS_TRANS_LIMIT_NO_ON_DEVICE;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimit_OnDevice") != NULL)
	{
		return PAYPASS_CLSS_TRANS_LIMIT_ON_DEVICE;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimit") != NULL)
	{
		return PAYPASS_CLSS_FLOOR_LIMIT;
	}
	
	return 0;

}


static int ParsePayPassAidChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	PAYPASS_AID_NODE_T *stAidNode = NULL;
	stAidNode = (PAYPASS_AID_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = PaypassGetAidChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case PAYPASS_PARTIALAIDSELECTION:
			iRet = GetUcharData(pszXmlOneLine, "PartialAIDSelection", &(stAidNode->ucSelFlag));
			break;
		case PAYPASS_APPLICATIONID:
			iRet = GetBcdData(pszXmlOneLine, "ApplicationID", stAidNode->aucAID);
			stAidNode->ucAidLen = iRet;
			break;
		case PAYPASS_IFUSELOCALNAME:
			iRet = GetUcharData(pszXmlOneLine, "IfUseLocalAIDName",&(stAidNode->ucIfUseLocalName));
			break;
		case PAYPASS_LOACALAIDNAME:
			iRet = GetStringData(pszXmlOneLine, "LocalAIDName", stAidNode->aucAppName);
			break;
		case PAYPASS_TERMINALAIDVERSION:
			iRet = GetBcdData(pszXmlOneLine, "TerminalAIDVersion", stAidNode->aucVersion);
			break;
		case PAYPASS_MAG_APP_VERSION_NUM:
			iRet = GetBcdData(pszXmlOneLine, "MagneticApplicationVersionNumber", stAidNode->aucMagAppVersion);
			break;
		case PAYPASS_TACDENIAL:
			iRet = GetBcdData(pszXmlOneLine, "TACDenial", stAidNode->aucTACDenial);
			break;
		case PAYPASS_TACONLINE:
			iRet = GetBcdData(pszXmlOneLine, "TACOnline", stAidNode->aucTACOnline);
			break;
		case PAYPASS_TACDEFAULT:
			iRet = GetBcdData(pszXmlOneLine,"TACDefault", stAidNode->aucTACDefault);
			break;
		case PAYPASS_TERMINAL_RISK:
			iRet = GetBcdData(pszXmlOneLine, "TerminalRisk", stAidNode->aucTermRisk);
			break;
		case PAYPASS_CLSS_CVM_LIMIT:
			iRet = GetAmtData(pszXmlOneLine, "ContactlessCVMLimit",stAidNode->aucRDCVMLmt, sizeof(stAidNode->aucRDCVMLmt));
			break;
		case PAYPASS_CLSS_TRANS_LIMIT_NO_ON_DEVICE:
			iRet = GetAmtData(pszXmlOneLine,"ContactlessTransactionLimit_NoOnDevice", stAidNode->aucTxnLmt_NoOnDevice, sizeof(stAidNode->aucTxnLmt_NoOnDevice));
			break;
		case PAYPASS_CLSS_TRANS_LIMIT_ON_DEVICE:
			iRet = GetAmtData(pszXmlOneLine, "ContactlessTransactionLimit_OnDevice", stAidNode->aucTxnLmt_OnDevice, sizeof(stAidNode->aucTxnLmt_OnDevice));
			break;
		case PAYPASS_CLSS_FLOOR_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "ContactlessFloorLimit", &(stAidNode->ulClssFloorLimit));
			break;

		default:
			break;
	}
	
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," GetAidChileNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	
	return iRet;
}


static int PaypassGetConfChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</PAYPASSCONFIGURATION>") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<KernelConfiguration") != NULL)
	{
		return PAYPASS_KERNEL_CONF;
	}
	if(strstr(pszXmlOneLine, "<TornLeftTime") != NULL)
	{
		return PAYPASS_TORN_LEFT_TIME;
	}
	if(strstr(pszXmlOneLine, "<MaximumTornNumber") != NULL)
	{
		return PAYPASS_MAX_TORN_NUM;
	}
	if(strstr(pszXmlOneLine, "<MagneticCVM") != NULL)
	{
		return PAYPASS_MAGENTIC_CVM;
	}
	if(strstr(pszXmlOneLine, "<MageticNoCVM") != NULL)
	{
		return PAYPASS_PAYPASS_MAGENTIC_NO_CVM;
	}
	if(strstr(pszXmlOneLine, "<MobileSupport") != NULL)
	{
		return PAYPASS_MOBILE_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<CardDataInput") != NULL)
	{
		return PAYPASS_CARD_DATA_INPUT;
	}
	if(strstr(pszXmlOneLine, "<CVMCapability_CVMRequired") != NULL)
	{
		return PAYPASS_CVM_CAPA_CVM_REQ;
	}
	if(strstr(pszXmlOneLine, "<CVMCapability_NoCVMRequired") != NULL)
	{
		return PAYPASS_CVM_CAPA_NO_CVM_REQ;
	}
	if(strstr(pszXmlOneLine, "<TerminalType") != NULL)
	{
		return PAYPASS_TERMINAL_TYPE;
	}
	if(strstr(pszXmlOneLine, "<AccoutType") != NULL)
	{
		return PAYPASS_ACCOUNT_TYPE;
	}
	if(strstr(pszXmlOneLine, "<AdditionalTerminalCapability") != NULL)
	{
		return PAYPASS_ADDITION_TEEM_CAP;
	}
	if(strstr(pszXmlOneLine, "<KernelID") != NULL)
	{
		return PAYPASS_KERNEL_ID;
	}
	if(strstr(pszXmlOneLine, "<SecurityCapability") != NULL)
	{
		return PAYPASS_SECURITY_CAPALILITY;
	}
	
	return 0;

}


static int ParsePayPassConfChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	PAYPASS_CONF_T *stConfNode = NULL;
	stConfNode = (PAYPASS_CONF_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = PaypassGetConfChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case PAYPASS_KERNEL_CONF:
			iRet = GetBcdData(pszXmlOneLine, "KernelConfiguration",&(stConfNode->ucKernelConf));
			break;
		case PAYPASS_TORN_LEFT_TIME:
			iRet = GetBcdData(pszXmlOneLine, "TornLeftTime", &(stConfNode->ucTornLeftTime));
			break;
		case PAYPASS_MAX_TORN_NUM:
			iRet = GetUcharData(pszXmlOneLine, "MaximumTornNumber", &(stConfNode->ucMaximumTornNumber));
			break;
		case PAYPASS_MAGENTIC_CVM:
			iRet = GetBcdData(pszXmlOneLine, "MagneticCVM", &(stConfNode->ucMagneticCVM));
			break;
		case PAYPASS_PAYPASS_MAGENTIC_NO_CVM:
			iRet = GetBcdData(pszXmlOneLine, "MageticNoCVM", &(stConfNode->ucMageticNoCVM));
			break;
		case PAYPASS_MOBILE_SUPPORT:
			iRet = GetBcdData(pszXmlOneLine, "MobileSupport", &(stConfNode->ucMobileSupport));
			break;
		case PAYPASS_CARD_DATA_INPUT:
			iRet = GetBcdData(pszXmlOneLine, "CardDataInput",&(stConfNode->ucCardDataInput));
			break;
		case PAYPASS_CVM_CAPA_CVM_REQ:
			iRet = GetBcdData(pszXmlOneLine, "CVMCapability_CVMRequired", &(stConfNode->ucCVMCapability_CVMRequired));
			break;
		case PAYPASS_CVM_CAPA_NO_CVM_REQ:
			iRet = GetBcdData(pszXmlOneLine, "CVMCapability_NoCVMRequired", &(stConfNode->ucCVMCapability_NoCVMRequired));
			break;
		case PAYPASS_TERMINAL_TYPE:
			iRet = GetBcdData(pszXmlOneLine, "TerminalType",&(stConfNode->ucTerminalType));
			break;
		case PAYPASS_ACCOUNT_TYPE:
			iRet = GetBcdData(pszXmlOneLine, "AccoutType", &(stConfNode->ucAccoutType));
			break;
		case PAYPASS_ADDITION_TEEM_CAP:
			iRet = GetBcdData(pszXmlOneLine, "AdditionalTerminalCapability",stConfNode->aucAdditionalTerminalCapability);
			break;
		case PAYPASS_KERNEL_ID:
			iRet = GetBcdData(pszXmlOneLine, "KernelID",&(stConfNode->ucKernelID));
			break;
		case PAYPASS_SECURITY_CAPALILITY:
			iRet = GetBcdData(pszXmlOneLine, "SecurityCapability", &(stConfNode->ucSecurityCapability));
			break;

		default:
			break;
	}
	
	
	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," PaypassGetConfChileNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	
	return iRet;
}

static int PayWaveGetInterWareChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "/Inter_WareFloorlimitByTransactionType") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<TransactionType>") != NULL)
	{
		return PAYWAVE_TRANS_TYPE;
	}
	if(strstr(pszXmlOneLine, "<TerminalFloorLimit>") != NULL)
	{
		return PAYWAVE_TERM_FLOOR_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessCVMLimit>") != NULL)
	{
		return PAYWAVE_CLSS_CVM_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimit>") != NULL)
	{
		return PAYWAVE_CLSS_TRANS_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimit>") != NULL)
	{
		return PAYWAVE_CLSS_FLOOR_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<TerminalFloorLimitSupported>") != NULL)
	{
		return PAYWAVE_TERM_FLOOR_LIMINT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimitSupported>") != NULL)
	{
		return PAYWAVE_CLSS_TRANS_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<CVMLimitSupported>") != NULL)
	{
		return PAYWAVE_CVM_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimitSupported>") != NULL)
	{
		return PAYWAVE_CLSS_FLOOR_LIMIT_SUPPORT;
	}
	return 0;

}


static int ParsePayWaveInterWareChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	PAYWAVE_INTER_WARE_NODE_T *pstInterWareNode = NULL;
	pstInterWareNode = (PAYWAVE_INTER_WARE_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = PayWaveGetInterWareChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case PAYWAVE_TRANS_TYPE:
			iRet = GetBcdData(pszXmlOneLine, "TransactionType", &(pstInterWareNode->ucTransType));
			break;
		case PAYWAVE_TERM_FLOOR_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "TerminalFloorLimit", &(pstInterWareNode->ulTermFLmt));
			break;
		case PAYWAVE_CLSS_CVM_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "ContactlessCVMLimit", &(pstInterWareNode->ulRdCVMLmt));
			break;
		case PAYWAVE_CLSS_TRANS_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "ContactlessTransactionLimit", &(pstInterWareNode->ulcRdClssTxnLmt));
			break;
		case PAYWAVE_CLSS_FLOOR_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "ContactlessFloorLimit", &(pstInterWareNode->ulRdClssFLmt));
			break;
		case PAYWAVE_TERM_FLOOR_LIMINT_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "TerminalFloorLimitSupported",&(pstInterWareNode->ucTermFLmtFlg));
			break;
		case PAYWAVE_CLSS_TRANS_LIMIT_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "ContactlessTransactionLimitSupported", &(pstInterWareNode->ucRdClssTxnLmtFlg));
			break;
		case PAYWAVE_CVM_LIMIT_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "CVMLimitSupported", &(pstInterWareNode->ucRdCVMLmtFlg));
			break;
		case PAYWAVE_CLSS_FLOOR_LIMIT_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "ContactlessFloorLimitSupported", &(pstInterWareNode->ucRdClssFLmtFlg));
			break;
		default:
			break;
	}

	EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," PayWaveGetInterWareChileNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	
	return iRet;
}


//begin to parse aid
static int PayWaveGetAidChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</AID>") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<PartialAIDSelection>") != NULL)
	{
		return PAYWAVE_PARTIALAIDSELECTION;
	}
	if(strstr(pszXmlOneLine, "<ApplicationID>") != NULL)
	{
		return PAYWAVE_APPLICATIONID;
	}
	if(strstr(pszXmlOneLine, "<IfUseLocalAIDName>") != NULL)
	{
		return PAYWAVE_IFUSELOCALNAME;
	}
	if(strstr(pszXmlOneLine, "<LocalAIDName>") != NULL)
	{
		return PAYWAVE_LOACALAIDNAME;
	}
	if(strstr(pszXmlOneLine, "<TerminalAIDVersion>") != NULL)
	{
		return PAYWAVE_TERMINALAIDVERSION;
	}
	if(strstr(pszXmlOneLine, "<CryptogramVersion17Supported>") != NULL)
	{
		return PAYWAVE_CRY_VER17_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ZeroAmountNoAllowed>") != NULL)
	{
		return PAYWAVE_ZERO_AMT_NO_ALLOW;
	}
	if(strstr(pszXmlOneLine, "<StatusCheckSupported>") != NULL)
	{
		return PAYWAVE_STATUS_CHECKSUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ReaderTTQ>") != NULL)
	{
		return PAYWAVE_READER_TTQ;
	}
	if(strstr(pszXmlOneLine, "<INTERWARELIST") != NULL)
	{
		return PAYWAVE_INTERWARELIST_START;
	}
	if(strstr(pszXmlOneLine, "</INTERWARELIST") != NULL)
	{
		return PAYWAVE_INTERWARELIST_END;
	}
	if(strstr(pszXmlOneLine, "<Inter_WareFloorlimitByTransactionType") != NULL)
	{
		return PAYWAVE_INTER_WARE_NODE_START;
	}
	if(strstr(pszXmlOneLine, "</Inter_WareFloorlimitByTransactionType") != NULL)
	{
		return PAYWAVE_INTER_WARE_NODE_END;
	}		
	if(strstr(pszXmlOneLine, "<TransactionType>") != NULL)
	{
		return PAYWAVE_TRANS_TYPE;
	}
	if(strstr(pszXmlOneLine, "<TerminalFloorLimit>") != NULL)
	{
		return PAYWAVE_TERM_FLOOR_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessCVMLimit>") != NULL)
	{
		return PAYWAVE_CLSS_CVM_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimit>") != NULL)
	{
		return PAYWAVE_CLSS_TRANS_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimit>") != NULL)
	{
		return PAYWAVE_CLSS_FLOOR_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<TerminalFloorLimitSupported>") != NULL)
	{
		return PAYWAVE_TERM_FLOOR_LIMINT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimitSupported>") != NULL)
	{
		return PAYWAVE_CLSS_TRANS_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<CVMLimitSupported>") != NULL)
	{
		return PAYWAVE_CVM_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimitSupported>") != NULL)
	{
		return PAYWAVE_CLSS_FLOOR_LIMIT_SUPPORT;
	}
	return 0;

}


static int ParsePayWaveAidChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	PAYWAVE_AID_NODE_T *stAidNode = NULL;
	stAidNode = (PAYWAVE_AID_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = PayWaveGetAidChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case PAYWAVE_PARTIALAIDSELECTION:
			iRet = GetUcharData(pszXmlOneLine, "PartialAIDSelection", &(stAidNode->ucSelFlag));
			break;
		case PAYWAVE_APPLICATIONID:
			iRet = GetBcdData(pszXmlOneLine, "ApplicationID", stAidNode->aucAID);
			stAidNode->ucAidLen = iRet;
			break;
		case PAYWAVE_IFUSELOCALNAME:
			iRet = GetUcharData(pszXmlOneLine, "IfUseLocalAIDName",&(stAidNode->ucIfUseLocalName));
			break;
		case PAYWAVE_LOACALAIDNAME:
			iRet = GetStringData(pszXmlOneLine, "LocalAIDName", stAidNode->aucAppName);
			break;
		case PAYWAVE_TERMINALAIDVERSION:
			iRet = GetBcdData(pszXmlOneLine, "TerminalAIDVersion",stAidNode->aucAppVersion);
			break;
		case PAYWAVE_CRY_VER17_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "CryptogramVersion17Supported",&(stAidNode->ucCrypto17Flg));
			break;
		case PAYWAVE_ZERO_AMT_NO_ALLOW:
			iRet = GetUcharData(pszXmlOneLine, "ZeroAmountNoAllowed", &(stAidNode->ucZeroAmtNoAllowed));
			break;
		case PAYWAVE_STATUS_CHECKSUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "StatusCheckSupported",&(stAidNode->ucStatusCheckFlg));
			break;
		case PAYWAVE_READER_TTQ:
			iRet = GetBcdData(pszXmlOneLine, "ReaderTTQ",stAidNode->aucReaderTTQ);
			break;
		case PAYWAVE_INTERWARELIST_START:
			break;
		case PAYWAVE_INTER_WARE_NODE_START:
			giPayWaveInterWareIndex++;
			break;
		case PAYWAVE_INTERWARELIST_END:
			//giPayWaveInterWareIndex = 0;
			break;
		case PAYWAVE_TRANS_TYPE:	
		case PAYWAVE_TERM_FLOOR_LIMIT:			
		case PAYWAVE_CLSS_CVM_LIMIT:				
		case PAYWAVE_CLSS_TRANS_LIMIT:				
		case PAYWAVE_CLSS_FLOOR_LIMIT:		
		case PAYWAVE_TERM_FLOOR_LIMINT_SUPPORT:		
		case PAYWAVE_CLSS_TRANS_LIMIT_SUPPORT:					
		case PAYWAVE_CVM_LIMIT_SUPPORT:					
		case PAYWAVE_CLSS_FLOOR_LIMIT_SUPPORT:
			iRet = ParsePayWaveInterWareChildNode(pszXmlOneLine, &(stAidNode->stFlmtByTransType[giPayWaveInterWareIndex - 1]));
			break;
			
		default:
			break;
	}
	
	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," PayWaveGetAidChileNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	
	return iRet;
}



static int PayWaveGetProgramIdChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "/PROGRAMID") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<ProgramId>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID;
	}
	if(strstr(pszXmlOneLine, "<TerminalFloorLimit>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_TERM_FLOOR_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessCVMLimit>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_CLSS_CVM_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimit>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_CLSS_TRANS_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimit>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_CLSS_FLOOR_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<TerminalFloorLimitSupported>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_TERM_FLOOR_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimitSupported>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_CLSS_TRANS_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<CVMLimitSupported>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_CVM_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimitSupported>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_CLSS_FLOOR_LIMIT_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<CryptogramVersion17Supported>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_CRYPTO_VERSION_17_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ZeroAmountNoAllowed>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_ZERO_AMOUNT_NO_ALLOWED;
	}
	if(strstr(pszXmlOneLine, "<StatusCheckSupported>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_STATUS_CHECK_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<ReaderTTQ>") != NULL)
	{
		return PAYWAVE_PROGRAM_ID_READER_TTQ;
	}
	return 0;

}


static int ParsePayWaveProgramIdChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	PAYWAVE_PROGREAM_ID_NODE_T *pstPrgIdNode = NULL;
	pstPrgIdNode = (PAYWAVE_PROGREAM_ID_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = PayWaveGetProgramIdChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case PAYWAVE_PROGRAM_ID:
			iRet = GetBcdData(pszXmlOneLine, "ProgramId", pstPrgIdNode->aucProgramId);
			pstPrgIdNode->ucPrgramIdLen = iRet;
			break;
		case PAYWAVE_PROGRAM_ID_TERM_FLOOR_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "TerminalFloorLimit", &(pstPrgIdNode->tPreProcDRL.ulTermFLmt));
			break;
		case PAYWAVE_PROGRAM_ID_CLSS_CVM_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "ContactlessCVMLimit", &(pstPrgIdNode->tPreProcDRL.ulRdCVMLmt));
			break;
		case PAYWAVE_PROGRAM_ID_CLSS_TRANS_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "ContactlessTransactionLimit", &(pstPrgIdNode->tPreProcDRL.ulRdClssTxnLmt));
			break;
		case PAYWAVE_PROGRAM_ID_CLSS_FLOOR_LIMIT:
			iRet = GetUlongData(pszXmlOneLine, "ContactlessFloorLimit", &(pstPrgIdNode->tPreProcDRL.ulRdClssFLmt));
			break;
		case PAYWAVE_PROGRAM_ID_TERM_FLOOR_LIMIT_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "TerminalFloorLimitSupported", &(pstPrgIdNode->tPreProcDRL.ucTermFLmtFlg));
			break;
		case PAYWAVE_PROGRAM_ID_CLSS_TRANS_LIMIT_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "ContactlessTransactionLimitSupported",&(pstPrgIdNode->tPreProcDRL.ucRdClssTxnLmtFlg));
			break;
		case PAYWAVE_PROGRAM_ID_CVM_LIMIT_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "CVMLimitSupported", &(pstPrgIdNode->tPreProcDRL.ucRdCVMLmtFlg));
			break;
		case PAYWAVE_PROGRAM_ID_CLSS_FLOOR_LIMIT_SUPPORT:
			iRet = GetBcdData(pszXmlOneLine, "ContactlessFloorLimitSupported",&(pstPrgIdNode->tPreProcDRL.ucRdClssFLmtFlg));
			break;
		case PAYWAVE_PROGRAM_ID_CRYPTO_VERSION_17_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "CryptogramVersion17Supported", &(pstPrgIdNode->tPreProcDRL.ucCrypto17Flg));
			break;
		case PAYWAVE_PROGRAM_ID_ZERO_AMOUNT_NO_ALLOWED:
			iRet = GetUcharData(pszXmlOneLine, "ZeroAmountNoAllowed", &(pstPrgIdNode->tPreProcDRL.ucCrypto17Flg));
			break;
		case PAYWAVE_PROGRAM_ID_STATUS_CHECK_SUPPORT:
			iRet = GetUcharData(pszXmlOneLine, "StatusCheckSupported", &(pstPrgIdNode->tPreProcDRL.ucStatusCheckFlg));
			break;
		case PAYWAVE_PROGRAM_ID_READER_TTQ:
			iRet = GetBcdData(pszXmlOneLine, "ReaderTTQ", pstPrgIdNode->tPreProcDRL.aucReaderTTQ);
			break;
			
		default:
			break;
	}
	
	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," ParsePayWaveProgramIdChildNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	
	return iRet;
}



static int PayWaveGetTermCapChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</TERMINALCAPABILITIES>") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<TermType>") != NULL)
	{
		return PAYWAVE_TERM_CAP_TERM_TYPE;
	}
	if(strstr(pszXmlOneLine, "<CardDataInput>") != NULL)
	{
		return PAYWAVE_TERM_CAP_CARD_DATA_INPUT;
	}
	if(strstr(pszXmlOneLine, "<CVMSupport>") != NULL)
	{
		return PAYWAVE_TERM_CAP_CVM_SUPPORT;
	}
	if(strstr(pszXmlOneLine, "<OffDataAuth>") != NULL)
	{
		return PAYWAVE_TERM_CAP_OFF_DATA_AUTH;
	}
	if(strstr(pszXmlOneLine, "<TransTypeCapa>") != NULL)
	{
		return PAYWAVE_TERM_CAP_TRANS_TYPE_CAPA;
	}
	if(strstr(pszXmlOneLine, "<TermInput>") != NULL)
	{
		return PAYWAVE_TERM_CAP_TERM_INPUT;
	}
	if(strstr(pszXmlOneLine, "<TermOutput>") != NULL)
	{
		return PAYWAVE_TERM_CAP_TERM_OUTPUT;
	}
		
	return 0;

}

static int ParsePayWaveTermCapChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	PAYWAVE_TERM_CAPABILITY_NODE_T *pstTermCapNode = NULL;
	pstTermCapNode = (PAYWAVE_TERM_CAPABILITY_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = PayWaveGetTermCapChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case PAYWAVE_TERM_CAP_TERM_TYPE:
			iRet = GetBcdData(pszXmlOneLine, "TermType", &(pstTermCapNode->ucTermType));
			break;
		case PAYWAVE_TERM_CAP_CARD_DATA_INPUT:
			iRet = GetBcdData(pszXmlOneLine, "CardDataInput", &(pstTermCapNode->ucCardDataInput));
			break;
		case PAYWAVE_TERM_CAP_CVM_SUPPORT:
			iRet = GetBcdData(pszXmlOneLine, "CVMSupport", &(pstTermCapNode->ucCVMSupport));
			break;
		case PAYWAVE_TERM_CAP_OFF_DATA_AUTH:
			iRet = GetBcdData(pszXmlOneLine, "OffDataAuth", &(pstTermCapNode->ucOffDataAuth));
			break;
		case PAYWAVE_TERM_CAP_TRANS_TYPE_CAPA:
			iRet = GetBcdData(pszXmlOneLine, "TransTypeCapa", &(pstTermCapNode->ucTransTypeCapa));
			break;
		case PAYWAVE_TERM_CAP_TERM_INPUT:
			iRet = GetBcdData(pszXmlOneLine, "TermInput", &(pstTermCapNode->ucTermInput));
			break;
		case PAYWAVE_TERM_CAP_TERM_OUTPUT:
			iRet = GetBcdData(pszXmlOneLine, "TermOutput", &(pstTermCapNode->ucTermOutput));
			break;

		default:
			break;
	}

	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," PayWaveGetTermCapChileNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	
	return iRet;
}


static int ExpressPayGetAidChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</AID>") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<PartialAIDSelection") != NULL)
	{
		return EXPRESSPAY_PARTIAL_AID_SELECTION;
	}
	if(strstr(pszXmlOneLine, "<ApplicationID") != NULL)
	{
		return EXPRESSPAY_APPLICATIONID;
	}
	if(strstr(pszXmlOneLine, "<IfUseLocalAIDName") != NULL)
	{
		return EXPRESSPAY_IF_USE_LOCAL_NAME;
	}
	if(strstr(pszXmlOneLine, "<LocalAIDName") != NULL)
	{
		return EXPRESSPAY_LOCAL_AID_NAME;
	}
	if(strstr(pszXmlOneLine, "<TerminalAIDVersion") != NULL)
	{
		return EXPRESSPAY_TERMINAL_AID_VERSION;
	}
	if(strstr(pszXmlOneLine, "<TACDenial") != NULL)
	{
		return EXPRESSPAY_TACDENIAL;
	}
	if(strstr(pszXmlOneLine, "<TACOnline") != NULL)
	{
		return EXPRESSPAY_TACONLINE;
	}
	if(strstr(pszXmlOneLine, "<TACDefault") != NULL)
	{
		return EXPRESSPAY_TACDEFAULT;
	}
	if(strstr(pszXmlOneLine, "<DDOL") != NULL)
	{
		return EXPRESSPAY_DDOL;
	}
	if(strstr(pszXmlOneLine, "<TDOL") != NULL)
	{
		return EXPRESSPAY_TDOL;
	}
	if(strstr(pszXmlOneLine, "<ExpresspayTerminalCapabilities") != NULL)
	{
		return EXPRESSPAY_TERMINAL_CAPABILITIES;
	}
	if(strstr(pszXmlOneLine, "<ContactlessCVMLimit") != NULL)
	{
		return EXPRESSPAY_CLSS_CVM_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessTransactionLimit") != NULL)
	{
		return EXPRESSPAY_CLSS_TRANS_LIMIT;
	}
	if(strstr(pszXmlOneLine, "<ContactlessFloorLimit") != NULL)
	{
		return EXPRESSPAY_CLSS_FLOOR_LIMIT;
	}
	
	return 0;

}

static int ParseExpressPayAidChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	EXPRESSPAY_AID_NODE_T *stAidNode = NULL;
	stAidNode = (EXPRESSPAY_AID_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = ExpressPayGetAidChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case EXPRESSPAY_PARTIAL_AID_SELECTION:
			iRet = GetUcharData(pszXmlOneLine, "PartialAIDSelection", &(stAidNode->ucSelFlag));
			break;
		case EXPRESSPAY_APPLICATIONID:
			iRet = GetBcdData(pszXmlOneLine, "ApplicationID", stAidNode->aucAID);
		    stAidNode->ucAidLen = iRet;
			break;
		case EXPRESSPAY_IF_USE_LOCAL_NAME:
			iRet = GetUcharData(pszXmlOneLine, "IfUseLocalAIDName", &(stAidNode->ucIfUseLocalName));
			break;
		case EXPRESSPAY_LOCAL_AID_NAME:
			iRet = GetStringData(pszXmlOneLine, "LocalAIDName", stAidNode->aucAppName);
			break;
		case EXPRESSPAY_TERMINAL_AID_VERSION:
			iRet = GetBcdData(pszXmlOneLine, "TerminalAIDVersion", stAidNode->aucVersion);
			break;
		case EXPRESSPAY_TACDENIAL:
			iRet = GetBcdData(pszXmlOneLine, "TACDenial", stAidNode->aucTACDenial);
			break;
		case EXPRESSPAY_TACONLINE:
			iRet = GetBcdData(pszXmlOneLine, "TACOnline", stAidNode->aucTACOnline);
			break;
		case EXPRESSPAY_TACDEFAULT:
			iRet = GetBcdData(pszXmlOneLine, "TACDefault",stAidNode->aucTACDefault);
			break;
		case EXPRESSPAY_DDOL:
			iRet = GetBcdData(pszXmlOneLine, "DDOL", stAidNode->aucDDOL);
			break;
		case EXPRESSPAY_TDOL:
			iRet = GetBcdData(pszXmlOneLine, "TDOL", stAidNode->aucTDOL);
			break;
		case EXPRESSPAY_TERMINAL_CAPABILITIES:
			iRet = GetBcdData(pszXmlOneLine, "ExpresspayTerminalCapabilities", &stAidNode->aucTerminalCapability);
			break;
		case EXPRESSPAY_CLSS_CVM_LIMIT:
			iRet = GetAmtData(pszXmlOneLine, "ContactlessCVMLimit", stAidNode->aucClssCVMLimit, sizeof(stAidNode->aucClssCVMLimit));
			break;
		case EXPRESSPAY_CLSS_TRANS_LIMIT:
			iRet = GetAmtData(pszXmlOneLine, "ContactlessTransactionLimit", stAidNode->aucClssTransLimit, sizeof(stAidNode->aucClssTransLimit));
			break;
		case EXPRESSPAY_CLSS_FLOOR_LIMIT:
			iRet = GetAmtData(pszXmlOneLine,  "ContactlessFloorLimit", stAidNode->aucClssFloorLimit,sizeof(stAidNode->aucClssFloorLimit));
			break;

		default:
			break;
	}
	
	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," ExpressPayGetAidChileNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	return iRet;
}

static int ExpressPayGetConfChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</EXPRESSPAYCONFIGURATION>") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<TerminalSupportOptimizationModeTransaction") != NULL)
	{
		return EXPRESSPAY_TERMINAL_SUPPORT_OPTIMIZATION_MODE_TRANS;
	}
	if(strstr(pszXmlOneLine, "<UnpredictableNumberRange") != NULL)
	{
		return EXPRESSPAY_UNPREDICTABLE_NUMBER_RANGE;
	}
	if(strstr(pszXmlOneLine, "<TerminalTransactionCapability") != NULL)
	{
		return EXPRESSPAY_TERMINAL_TRANS_CAPABILITY;
	}
	if(strstr(pszXmlOneLine, "<DelayAuthorizationSupport") != NULL)
	{
		return EXPRESSPAY_DEALY_AUTHORIZATION_SUPPORT;
	}
	
	return 0;

}


static int ParseExpressPayConfChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	EXPRESSPAY_CONFIG_NODE_T *stConfNode = NULL;
	stConfNode = (EXPRESSPAY_CONFIG_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = ExpressPayGetConfChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case EXPRESSPAY_TERMINAL_SUPPORT_OPTIMIZATION_MODE_TRANS:
			iRet = GetBcdData(pszXmlOneLine, "TerminalSupportOptimizationModeTransaction", &stConfNode->ucTmSupOptTrans);
			break;
		case EXPRESSPAY_UNPREDICTABLE_NUMBER_RANGE:
			iRet = GetBcdData(pszXmlOneLine, "UnpredictableNumberRange", stConfNode->aucUNRange);
			break;
		case EXPRESSPAY_TERMINAL_TRANS_CAPABILITY:
			iRet = GetBcdData(pszXmlOneLine, "TerminalTransactionCapability", stConfNode->aucTmTransCapa);
			break;
		case EXPRESSPAY_DEALY_AUTHORIZATION_SUPPORT:
			iRet = GetBcdData(pszXmlOneLine, "DelayAuthorizationSupport", &stConfNode->ucDelayAuthFlag);
			break;

		default:
			break;
	}
	
	
	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," ExpressPayGetConfChileNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);

	return iRet;
}



static int ClssGetOtherConfChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</OTHERCONFIGURATION>") != NULL)
	{
		return CLSS_XML_FILE_OP_OK;
	}		
	if(strstr(pszXmlOneLine, "<CountryCode>") != NULL)
	{
		return CLSS_OHTER_COUNTRY_CODE;
	}
	if(strstr(pszXmlOneLine, "<CurrencyCode>") != NULL)
	{
		return CLSS_OHTER_CURRENCY_CODE;
	}
	if(strstr(pszXmlOneLine, "<ReferenceCurrencyCode>") != NULL)
	{
		return CLSS_OHTER_REF_CURRENCY_CODE;
	}
	if(strstr(pszXmlOneLine, "<CurrencyExponent>") != NULL)
	{
		return CLSS_OHTER_CURRENCY_EXP;
	}
	if(strstr(pszXmlOneLine, "<ReferenceCurrencyExponent>") != NULL)
	{
		return CLSS_OHTER_REF_CUR_EXP;
	}
	if(strstr(pszXmlOneLine, "<ReferenceCurrenceConverRate>") != NULL)
	{
		return CLSS_OHTER_REF_CUR_CONVER_RATE;
	}
	if(strstr(pszXmlOneLine, "<MerchantCategoryCode>") != NULL)
	{
		return CLSS_OHTER_MER_CAT_CODE;
	}
	if(strstr(pszXmlOneLine, "<MerchantId>") != NULL)
	{
		return CLSS_OHTER_MERCHANT_ID;
	}
	if(strstr(pszXmlOneLine, "<TerminalID>") != NULL)
	{
		return CLSS_OHTER_TERMINAL_ID;
	}
	if(strstr(pszXmlOneLine, "<MerchantName>") != NULL)
	{
		return CLSS_OHTER_MERCHANT_NAME;
	}
	if(strstr(pszXmlOneLine, "<MerchantLocalAddress>") != NULL)
	{
		return CLSS_OHTER_MERCHANT_LOCAL_ADDR;
	}
		
	return 0;

}

static int ParseClssOhterConfChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	CLSS_OTHER_CONF_NODE_T *pstOtherConfNode = NULL;
	pstOtherConfNode = (CLSS_OTHER_CONF_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iChildNode = ClssGetOtherConfChileNode(pszXmlOneLine);
	if(iChildNode < 0)
	{
		return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iChildNode)
	{
		case CLSS_OHTER_COUNTRY_CODE:
			iRet = GetBcdData(pszXmlOneLine, "CountryCode", pstOtherConfNode->aucCountryCode);
			break;
		case CLSS_OHTER_CURRENCY_CODE:
			iRet = GetBcdData(pszXmlOneLine, "CurrencyCode",pstOtherConfNode->aucCurrencyCode);
			break;
		case CLSS_OHTER_REF_CURRENCY_CODE:
			iRet = GetBcdData(pszXmlOneLine, "ReferenceCurrencyCode",pstOtherConfNode->aucRefCurrencyCode);
			break;
		case CLSS_OHTER_CURRENCY_EXP:
			iRet = GetUcharData(pszXmlOneLine, "CurrencyExponent",&(pstOtherConfNode->ucCurrencyExponent));
			break;
		case CLSS_OHTER_REF_CUR_EXP:
			iRet = GetUcharData(pszXmlOneLine, "ReferenceCurrencyExponent", &(pstOtherConfNode->ucRefCurrencyExponent));
			break;
		case CLSS_OHTER_REF_CUR_CONVER_RATE:
			iRet = GetBcdData(pszXmlOneLine, "ReferenceCurrenceConverRate",pstOtherConfNode->aucRefCurrenceConVerRate);
			break;
		case CLSS_OHTER_MER_CAT_CODE:
			iRet = GetBcdData(pszXmlOneLine, "MerchantCategoryCode",pstOtherConfNode->aucMerchantCateCode);
			break;
		case CLSS_OHTER_MERCHANT_ID:
			iRet = GetStringData(pszXmlOneLine, "MerchantId",pstOtherConfNode->aucMerchantId);
			break;
		case CLSS_OHTER_TERMINAL_ID:
			iRet = GetStringData(pszXmlOneLine, "TerminalID",pstOtherConfNode->aucTerminalId);
			break;
		case CLSS_OHTER_MERCHANT_NAME:
			iRet = GetStringData(pszXmlOneLine, "MerchantName",pstOtherConfNode->aucMerChantName);
			break;
		case CLSS_OHTER_MERCHANT_LOCAL_ADDR:
			iRet = GetStringData(pszXmlOneLine, "MerchantLocalAddress",pstOtherConfNode->aucMerChantLocalAddr);
			break;

		default:
			break;
	}
	
	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iChildNode = %d iSize = %d iRet = %d\n",iChildNode, iSize, iRet);
	
	return iRet;
}



static int FindFunctionIndex(const uchar *pszNodeName, NODE_FUNC_TAB_T *vParseListFuncTab, int iSize)
{
	int i = 0;

	for(i = 0; i < iSize; i++)
	{
		if(strcmp(pszNodeName, vParseListFuncTab[i].ucNodeName) == 0)
		{	
			return i;
		}
	}
	return CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
}

static int FindNodePositonInFile(const uchar *pszFileName, const uchar *pszNodeName, int iPos)
{
	int iFd = 0;
	int iFileSize = 0;
	int iSearchLen = 0;
	uchar ucReadBuf[CLSS_FILE_READ_BUF_LEN + 1] = {0};
	uchar ucTempBuf[CLSS_TEMP_BUF_LEN + 1] = {0};
	uchar ucParseBuf[CLSS_FILE_READ_BUF_LEN + 1] = {0};
	uchar *pszStart  = NULL;
	uchar *pszLastEnterPos = NULL;
	int iRet = 0;
	int iOffset = 0;
	ulong uiPosition = 0;
	int iLseek = 0;

	iRet = PdkFile_Exist (pszFileName);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iFd = PdkFile_Open(pszFileName);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(pszFileName);
	//AppLog("FindNodePositonInFile PdkFile_GetSize iFileSize = %d\n", iFileSize);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = PdkFile_Seek (iFd, iPos, SEEK_CUR);
	if(iLseek < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iLseek = %d\n",iLseek);
		return iLseek;
	}

	while(iSearchLen < iFileSize)
	{
		memset(ucReadBuf, 0, sizeof(ucReadBuf));
		iRet = PdkFile_Read(iFd, ucReadBuf, 1024);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
			return iRet;
		}

		//every time ,we should confirm the last line is whole line
		pszLastEnterPos = strrchr(ucReadBuf,'\x0a');
		if(pszLastEnterPos == NULL)
		{
			PdkFile_Close(iFd);
			return CLSS_XML_FILE_OP_ERR_ONELINE_OVER_1024;
		}
		
		memcpy(ucParseBuf, ucReadBuf, pszLastEnterPos - ucReadBuf);

		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"  ucParseBuf = %s\n",ucParseBuf);
		//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__,"  pszNodeName = %s\n",pszNodeName);

		pszStart = strstr(ucParseBuf, pszNodeName);
		if(pszStart == NULL)
		{
		    //the len have search
			iOffset = 1024 - (pszLastEnterPos - ucReadBuf);
			iSearchLen = iSearchLen + (pszLastEnterPos - ucReadBuf);
			iLseek = PdkFile_Seek(iFd, -iOffset, SEEK_CUR);
			if(iLseek < 0)
			{
				PdkFile_Close(iFd);
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iLseek = %d\n",iLseek);
				return iLseek;
			}
			continue;
		}
		else
		{
			uiPosition = iSearchLen + (pszStart - ucParseBuf);
			PdkFile_Close(iFd);
			return uiPosition;
		}
	}
	PdkFile_Close(iFd);
	return CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST;
}

static int ParseList(const uchar *pszFileName, const uchar *pszListStart, const uchar *pszListEnd,  const uchar *pszNodeStart, const uchar *pszNodeEnd, int iStartOffset, int iClssType)
{
	int iRet = 0;
	int iListPos = 0;
	uchar *pszNodeEndTag = NULL;
	uchar *pszListEndTag = NULL;
	int iFd = 0;
	int iFileSize = 0;
	int iLseek = 0;
	uchar aucReadBuf[CLSS_FILE_READ_BUF_LEN + 1] = {0};
	uchar aucParseBuf[CLSS_FILE_READ_BUF_LEN + 1] = {0};
	uchar aucXmlLineBuf[CLSS_FILE_ONELINE_BUF_LEN] = {0};
	uchar *pszStart = NULL;
	uchar *pszLastEnterPos = NULL;
	int iSearchLen = 0;
	int iLen = 0;
	int iHaveGetLen = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iReadLen = 0;
	NODE_FUNC_TAB_T *vParseListFuncTab = NULL;
	int iFunSize = 0;

	PAYPASS_AID_NODE_T stPaypassAidNode = {0};
	PAYPASS_CONF_T stPayPassConf = {0};
	PAYWAVE_AID_NODE_T stPayWaveAidNode = {0};
	PAYWAVE_INTER_WARE_NODE_T stPayWaveInterWare = {0};
	PAYWAVE_PROGREAM_ID_NODE_T stPayWavePrgId = {0};
	PAYWAVE_TERM_CAPABILITY_NODE_T stPayWaveTermCap = {0};

	EXPRESSPAY_AID_NODE_T stExpressPayAidNode = {0};
	EXPRESSPAY_CONFIG_NODE_T stExpressPayConfigNode = {0};
	
	CLSS_OTHER_CONF_NODE_T stClssOtherConf = {0};

	NODE_FUNC_TAB_T vParsePayPassListFuncTab[] =
	{
		{"<AID>", ParsePayPassAidChildNode, &stPaypassAidNode, sizeof(stPaypassAidNode)},
		{"<PAYPASSCONFIGURATION>", ParsePayPassConfChildNode, &stPayPassConf, sizeof(stPayPassConf)},
	};

	NODE_FUNC_TAB_T vParsePayWaveListFuncTab[] =
	{
		{"<AID>", ParsePayWaveAidChildNode, &stPayWaveAidNode, sizeof(stPayWaveAidNode)},
		{"<PROGRAMID>", ParsePayWaveProgramIdChildNode, &stPayWavePrgId, sizeof(stPayWavePrgId)},
		{"<TERMINALCAPABILITIES>", ParsePayWaveTermCapChildNode, &stPayWaveTermCap, sizeof(stPayWaveTermCap)},
	};

	NODE_FUNC_TAB_T vParseExpressPayListFuncTab[] =
	{
		{"<AID>", ParseExpressPayAidChildNode, &stExpressPayAidNode, sizeof(stExpressPayAidNode)},
		{"<EXPRESSPAYCONFIGURATION>", ParseExpressPayConfChildNode, &stExpressPayConfigNode, sizeof(stExpressPayConfigNode)},
	};

	NODE_FUNC_TAB_T vParseCommonListFuncTab[] =
	{
		{"<OTHERCONFIGURATION>", ParseClssOhterConfChildNode, &stClssOtherConf, sizeof(stClssOtherConf)},
	};

	//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iClssType = %d\n",iClssType);

	switch(iClssType)
	{
		case CLSS_PAYPASS:
			vParseListFuncTab = vParsePayPassListFuncTab;
			iFunSize = sizeof(vParsePayPassListFuncTab)/sizeof(vParsePayPassListFuncTab[0]);
			break;
		case CLSS_PAYWAVE:
			vParseListFuncTab = vParsePayWaveListFuncTab;
			iFunSize = sizeof(vParsePayWaveListFuncTab)/sizeof(vParsePayWaveListFuncTab[0]);
			break;

		case CLSS_EXPRESSPAY:
			vParseListFuncTab = vParseExpressPayListFuncTab;
			iFunSize = sizeof(vParseExpressPayListFuncTab)/sizeof(vParseExpressPayListFuncTab[0]);
			break;
			
		case CLSS_COMMON:
			vParseListFuncTab = vParseCommonListFuncTab;
			iFunSize = sizeof(vParseCommonListFuncTab)/sizeof(vParseCommonListFuncTab[0]);
			break;
		default:
			return CLSS_XML_FILE_OP_ERR_CLSS_TYPE_NOT_EXIST;
	}

	if(pszNodeStart == NULL || pszNodeEnd == NULL)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," CLSS_XML_FILE_OP_ERR_INVALID_PARAM NULL\n");
		return CLSS_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	if(pszListStart != NULL)
	{
		iListPos = FindNodePositonInFile(pszFileName, pszListStart, iStartOffset);
		if(iListPos < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iListPos = %d\n",iListPos);
			return iListPos;
		}
		iListPos = iListPos + iStartOffset;
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iListPos = %d\n",iListPos);
	}
	else
	{
		iListPos = FindNodePositonInFile(pszFileName, pszNodeStart, iStartOffset);
		if(iListPos < 0)
		{
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iListPos = %d\n",iListPos);
			return iListPos;
		}
		iListPos = iListPos + iStartOffset;
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iListPos = %d\n",iListPos);
	}
	
	iFd = PdkFile_Open(pszFileName);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(pszFileName);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = PdkFile_Seek (iFd, iListPos, SEEK_CUR);
	if(iLseek < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iLseek = %d\n",iLseek);
		return iFileSize;
	}

	//while((iReadLen + iListPos) < iFileSize)
	while((iReadLen) < iFileSize)
	{
		memset(aucReadBuf, 0, sizeof(aucReadBuf));
		iRet = PdkFile_Read(iFd, aucReadBuf, 1024);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
			return iRet;
		}
		iReadLen +=  iRet;

		//every time ,we should confirm the last line is whole line
		pszLastEnterPos = strrchr(aucReadBuf,'\x0a');
		if(pszLastEnterPos == NULL)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," pszLastEnterPos == NULL\n");
			return CLSS_XML_FILE_OP_ERR_ONELINE_OVER_1024;
		}

		memset(aucParseBuf, 0, sizeof(aucParseBuf));
		memcpy(aucParseBuf, aucReadBuf, pszLastEnterPos - aucReadBuf + 1);
			
		while((iHaveGetLen) < (pszLastEnterPos - aucReadBuf + 1))
		{
			memset(aucXmlLineBuf, 0, sizeof(aucXmlLineBuf));
			//offset mean hava get len,the parsebuf's data every line is whole line   include \r\n
			
			iRet = XmlGetOneLineData(aucParseBuf + iHaveGetLen, &iLen, aucXmlLineBuf);
			
			//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," ucXmlLineBuf = %s\n",aucXmlLineBuf);
			//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iLen = %d\n",iLen);

			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				PdkFile_Close (iFd);
				return iRet;
			}
			iHaveGetLen += iLen;
			
			//init the buf that use to save data
			if(strstr(aucXmlLineBuf, pszNodeStart) != NULL)
			{
				iIndex = FindFunctionIndex(pszNodeStart, vParseListFuncTab, iFunSize);
				if(iIndex == CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD)
				{
					EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
					PdkFile_Close(iFd);
					return CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
				}
				memset(vParseListFuncTab[iIndex].pOutData ,0,vParseListFuncTab[iIndex].iLen);
				//continue;
			}

			//for list
			if((pszListEnd != NULL) && (pszListStart != NULL))
			{
				//AppLog("ParseList  pszListEnd pszListEnd = %s\n", pszListEnd);
				pszListEndTag = strstr(aucXmlLineBuf, pszListEnd);
				if(pszListEndTag != NULL)
				{
					PdkFile_Close(iFd);
					//AppLog("ParseList  pszListEnd pszListEnd = NULL\n");
					return CLSS_XML_FILE_LIST_END;
				}

				pszNodeEndTag = strstr(aucXmlLineBuf, pszNodeEnd);
				if(pszNodeEndTag != NULL)
				{
					//AppLog("ParseList  pszNodeEnd pszNodeEnd != NULL return XML_PARSE_OK\n");
					iRet = SaveNodeStruct(pszNodeStart, iClssType, vParseListFuncTab[iIndex].pOutData, vParseListFuncTab[iIndex].iLen);
					giPayWaveInterWareIndex = 0;
					if(iRet < 0)
					{
						PdkFile_Close(iFd);
						EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
						return iRet;
					}
					//continue;
				}
				else
				{
					iIndex = FindFunctionIndex(pszNodeStart, vParseListFuncTab, iFunSize);
					if(iIndex == CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD)
					{
						EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
						PdkFile_Close(iFd);
						return CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
					}
					
				    if((strstr(aucXmlLineBuf, pszNodeStart) == NULL) && (strstr(aucXmlLineBuf, pszListStart) == NULL))
				    {
						iRet = vParseListFuncTab[iIndex].vFunc(aucXmlLineBuf, vParseListFuncTab[iIndex].pOutData);
						if(iRet < 0)
						{
							EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
							return iRet;
						}
						
				    }
				}
			}
			else//for node
			{
				pszNodeEndTag = strstr(aucXmlLineBuf, pszNodeEnd);
				if(pszNodeEndTag != NULL)
				{
					iRet = SaveNodeStruct(pszNodeStart, iClssType, vParseListFuncTab[iIndex].pOutData, vParseListFuncTab[iIndex].iLen);
					if(iRet < 0)
					{
						EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
						PdkFile_Close(iFd);
						return iRet;
					}
					PdkFile_Close(iFd);
					return CLSS_XML_FILE_NODE_END;
				}
				else
				{
					iIndex = FindFunctionIndex(pszNodeStart, vParseListFuncTab, iFunSize);
					if(iIndex == CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD)
					{
						EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
						PdkFile_Close(iFd);
						return CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
					}
					if(strstr(aucXmlLineBuf, pszNodeStart) == NULL)
					{
						iRet = vParseListFuncTab[iIndex].vFunc(aucXmlLineBuf, vParseListFuncTab[iIndex].pOutData);
						if(iRet < 0)
						{
							EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
							return iRet;
						}			
						
					}
				}
			}

			//have finish one parsebuf, reset data  //dui de wei zhi
			if(iHaveGetLen == (pszLastEnterPos - aucReadBuf + 1))
			{
				iRet = PdkFile_Seek (iFd, iHaveGetLen - 1024, SEEK_CUR);
				//EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
				iHaveGetLen = 0;
				break;
			}
		}
		//parse one aid
	}
	PdkFile_Close(iFd);
	return iRet;
}

static int ParsePayPassAidList(const uchar *pszFileName)
{
	int iRet = 0;
	int iAidIndex = 0;
	int iNodeEnd = -1;
	int iIndex = 0;
	int iStartOffset = 0;
	

	iStartOffset = FindNodePositonInFile(pszFileName, "<PAYPASSPARAM>", 0);
	if(iStartOffset < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iStartOffset = %d\n",iStartOffset);
		return iStartOffset;
	}
	
	iRet = ParseList(pszFileName, "<AIDLIST>", "</AIDLIST>", "<AID>", "</AID>", iStartOffset, CLSS_PAYPASS);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYPASS_AID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		return iIndex;
	}
	gstNoeRecord[iIndex].count = giAidIndex;
	
	iRet = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iRet;
	}
	return iRet;
}

static int ParsePaypassConfList(const uchar *pszFileName)
{
	int iRet = 0;
	int iIndex = 0;
	int iStartOffset = 0;
	int iEndOffset = 0;

	iStartOffset = FindNodePositonInFile(pszFileName, "<PAYPASSPARAM>", 0);
	if(iStartOffset < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iStartOffset = %d\n",iStartOffset);
		return iStartOffset;
	}
	//AppLog("ParsePaypassConfList---- FindNodePositonInFile iStartOffset  = %d \n", iStartOffset);
	
	iRet = ParseList(pszFileName, NULL, NULL, "<PAYPASSCONFIGURATION>", "</PAYPASSCONFIGURATION>", iStartOffset, CLSS_PAYPASS);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYPASS_PAYPASSCONFIGURATION", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	gstNoeRecord[iIndex].count = giPayPassConfIndex;
	iRet = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}



static int ParsePayWaveAidList(const uchar *pszFileName)
{
	int iRet = 0;
	int iAidIndex = 0;
	int iNodeEnd = -1;
	int iIndex = 0;
	int iStartOffset = 0;
	int iEndOffset = 0;

	iStartOffset = FindNodePositonInFile(pszFileName, "<PAYWAVEPARAM>", 0);
	if(iStartOffset < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iStartOffset = %d\n",iStartOffset);
		return iStartOffset;
	}
	
	iRet = ParseList(pszFileName, "<AIDLIST>", "</AIDLIST>", "<AID>", "</AID>", iStartOffset, CLSS_PAYWAVE);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYWAVE_AID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	gstNoeRecord[iIndex].count = giPayWaveAidIndex;

	iIndex = ClssGetRecordIndex("PAYWAVE_INTER_WARE", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	gstNoeRecord[iIndex].count = giPayWaveInterWareIndex;
	
	iRet = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}

static int ParsePayWavePrgIdList(const uchar *pszFileName)
{
	int iRet = 0;
	int iAidIndex = 0;
	int iNodeEnd = -1;
	int iIndex = 0;
	int iStartOffset = 0;
	

	iStartOffset = FindNodePositonInFile(pszFileName, "<PAYWAVEPARAM>", 0);
	if(iStartOffset < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iStartOffset = %d\n",iStartOffset);
		return iStartOffset;
	}
	//AppLog("ParsePayWavePrgIdList---- FindNodePositonInFile iStartOffset  = %d \n", iStartOffset);
	
	iRet = ParseList(pszFileName, "<PROGRAMIDLIST>", "</PROGRAMIDLIST>", "<PROGRAMID>", "</PROGRAMID>", iStartOffset, CLSS_PAYWAVE);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYWAVE_PROGRAMID", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	gstNoeRecord[iIndex].count = giPayWavePrgIdIndex;
	
	iRet = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}

static int ParsePayWaveTermCapList(const uchar *pszFileName)
{
	int iRet = 0;
	int iAidIndex = 0;
	int iNodeEnd = -1;
	int iIndex = 0;
	int iStartOffset = 0;
	

	iStartOffset = FindNodePositonInFile(pszFileName, "<PAYWAVEPARAM>", 0);
	if(iStartOffset < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iStartOffset = %d\n",iStartOffset);
		return iStartOffset;
	}
	//AppLog("ParsePayWaveTermCapList---- FindNodePositonInFile iStartOffset  = %d \n", iStartOffset);
	
	iRet = ParseList(pszFileName, NULL, NULL, "<TERMINALCAPABILITIES>", "</TERMINALCAPABILITIES>", iStartOffset, CLSS_PAYWAVE);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("PAYWAVE_TERM_CAP", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	gstNoeRecord[iIndex].count = giPayWaveTermCapIndex;
	
	iRet = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}

// const uchar ucXmlTagType: 0x00:start-tag, 0x01: end-tag
char *PadXmlNode(const uchar *pszNodeName, const uchar ucXmlTagType)
{	
	if(pszNodeName == NULL || strlen(pszNodeName) == 0)
	{
		return "";
	}

	memset(RecvBuf, 0, sizeof(RecvBuf));
	if(ucXmlTagType == XML_TAG_START)
	{
		sprintf((char *)RecvBuf, "<%s>", pszNodeName);
	}
	else if(ucXmlTagType == XML_TAG_END)
	{
		sprintf((char *)RecvBuf, "</%s>", pszNodeName);
	}

	return RecvBuf;
}

// const uchar ucXmlTagType: 0x00:start-tag, 0x01: end-tag
// uchar *pszBuf: node name after padding
void PadXmlNodeName(const uchar *pszNodeName, const uchar ucXmlTagType, uchar *pszPadNodeName)
{	
	if(pszNodeName == NULL || strlen(pszNodeName) == 0)
	{
		return ;
	}

	if(ucXmlTagType == XML_TAG_START)
	{
		sprintf((char *)pszPadNodeName, "<%s>", pszNodeName);
	}
	else if(ucXmlTagType == XML_TAG_END)
	{
		sprintf((char *)pszPadNodeName, "</%s>", pszNodeName);
	}
}

void UpdateRecordCount(int iIndex, uchar *pszRecordName)
{
	if(0 == strcmp(pszRecordName, CLSS_PAYPASS_AID_NODE))
	{
		gstNoeRecord[iIndex].count = giAidIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_PAYPASS_CONF_NODE))
	{
		gstNoeRecord[iIndex].count = giPayPassConfIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_PAYWAVE_AID_NODE))
	{
		gstNoeRecord[iIndex].count = giPayWaveAidIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_PAYWAVE_INTER_WARE_NODE))
	{
		gstNoeRecord[iIndex].count = giPayWaveInterWareIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_PAYWAVE_PROGRAM_ID_NODE))
	{
		gstNoeRecord[iIndex].count = giPayWavePrgIdIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_PAYWAVE_TERM_CAP_NODE))
	{
		gstNoeRecord[iIndex].count = giPayWaveTermCapIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_EXPRESSPAY_AID_NODE))
	{
		gstNoeRecord[iIndex].count = giClssExpressPayAidIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_EXPRESSPAY_CONFIG_NODE))
	{
		gstNoeRecord[iIndex].count = giClssExpressPayConfigIndex;
	}
	else if(0 == strcmp(pszRecordName, CLSS_OTHER_CONF_NODE))
	{
		gstNoeRecord[iIndex].count = giClssOhterConfIndex;
	}
}

static int ParseClssSpecificKernelParam(const uchar *pszFileName, unsigned int uiClssType
	, const uchar pszNodeList[][XML_NODE_NAME_MAX_LEN], const unsigned int uiNodeListNum, uchar *pszRecordName)
{
	int iRet = 0;
	int iAidIndex = 0;
	int iNodeEnd = -1;
	int iIndex = 0;
	int iStartOffset = 0;
	unsigned char aucListStart[XML_NODE_NAME_MAX_LEN + 1]={0}, aucListEnd[XML_NODE_NAME_MAX_LEN + 1]={0}, aucNodeStart[XML_NODE_NAME_MAX_LEN + 1]={0}, aucNodeEnd[XML_NODE_NAME_MAX_LEN + 1]={0};

	if(uiNodeListNum == 0 || uiClssType == CLSS_END  || pszFileName == NULL || strlen(pszFileName) == 0)
	{
		return -1;
	}

	iStartOffset = FindNodePositonInFile(pszFileName, PadXmlNode(pszNodeList[0], XML_TAG_START), 0);
	if(iStartOffset < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iStartOffset = %d\n",iStartOffset);
		return iStartOffset;
	}

	if(uiNodeListNum == 1)
	{
		PadXmlNodeName(pszNodeList[0], XML_TAG_START, aucNodeStart);
		PadXmlNodeName(pszNodeList[0], XML_TAG_END, aucNodeEnd);
		iRet = ParseList(pszFileName, NULL, NULL, aucNodeStart, aucNodeEnd, iStartOffset, uiClssType);
	}
	else
	{
		PadXmlNodeName(pszNodeList[1], XML_TAG_START, aucListStart);
		PadXmlNodeName(pszNodeList[1], XML_TAG_END, aucListEnd);
		PadXmlNodeName(pszNodeList[2], XML_TAG_START, aucNodeStart);
		PadXmlNodeName(pszNodeList[2], XML_TAG_END, aucNodeEnd);
		iRet = ParseList(pszFileName, aucListStart, aucListEnd, aucNodeStart, aucNodeEnd, iStartOffset, uiClssType);
	}
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex(pszRecordName, sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	
	UpdateRecordCount(iIndex, pszRecordName);
	
	iRet = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}


static int ParseClssOhterConfList(const uchar *pszFileName)
{
	int iRet = 0;
	int iAidIndex = 0;
	int iNodeEnd = -1;
	int iIndex = 0;
	int iStartOffset = 0;
	

	iStartOffset = FindNodePositonInFile(pszFileName, "<OTHERCONFIGURATION>", 0);
	if(iStartOffset < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iStartOffset = %d\n",iStartOffset);
		return iStartOffset;
	}
	//AppLog("ParseClssOhterConfList---- FindNodePositonInFile iStartOffset  = %d \n", iStartOffset);
	
	iRet = ParseList(pszFileName, NULL, NULL, "<OTHERCONFIGURATION>", "</OTHERCONFIGURATION>", iStartOffset, CLSS_COMMON);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = ClssGetRecordIndex("CLSS_OHTER_CONF", sizeof(gstNoeRecord)/sizeof(gstNoeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	gstNoeRecord[iIndex].count = giClssOhterConfIndex;
	
	iRet = SaveParsedDataToFile(CLSS_XML_FILE_HAVE_PARSED, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}


static void InitGloableVar(void)
{
	giAidIndex = 0;
 	giPayPassConfIndex = 0;
	giPayWaveAidIndex = 0;
	giParsePos = 0;
    giPayWavePrgIdIndex = 0;
    giPayWaveTermCapIndex = 0;
    giClssOhterConfIndex = 0;
	giCurrentOffset = sizeof(gstNoeRecord);
    giClssExpressPayAidIndex = 0;
	giClssExpressPayConfigIndex = 0;
}

//init the file whic used to stored the have parsed data,save the node info in the start position of file
static int InitClssParsedFile(const uchar *pszFileName)
{
	int iFd = 0;
	int iRet = 0;

	if(PdkFile_Exist(pszFileName) == PDK_RET_OK)
	{
		PdkFile_Remove (pszFileName);
	}

	iFd = PdkFile_Open(pszFileName);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iFd = %d\n",iFd);
		return iFd;
	}
	
	iRet = PdkFile_Write (iFd, (uchar*)&gstNoeRecord, sizeof(gstNoeRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG,LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		PdkFile_Close(iFd);
		return iRet;
	}
	PdkFile_Close(iFd);
	return iRet;
}


int ParseClssFile(const uchar *pszFileName)
{
	int iRet = 0;

    InitGloableVar();
    iRet = InitClssParsedFile(CLSS_XML_FILE_HAVE_PARSED);
	if(iRet < 0)
	{
		return iRet;
	}
	iRet = ParsePayPassAidList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}
	
	iRet = ParsePaypassConfList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}
			
	iRet = ParsePayWaveAidList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}
	
	iRet = ParsePayWavePrgIdList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}	
	
	iRet = ParsePayWaveTermCapList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}

	iRet = ParseClssSpecificKernelParam(pszFileName, CLSS_EXPRESSPAY, aucExpressPayAidNode, 3, CLSS_EXPRESSPAY_AID_NODE);
	if(iRet < 0)
	{
		return iRet;
	}

	iRet = ParseClssSpecificKernelParam(pszFileName, CLSS_EXPRESSPAY, aucExpressPayConfigNode, 1, CLSS_EXPRESSPAY_CONFIG_NODE);
	if(iRet < 0)
	{
		return iRet;
	}
			
    iRet = ParseClssOhterConfList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}
	
	
	#ifdef CLSS_TEST
	TestGetPayPassAid();
	TestGetPayPassConf();
	TestGetPayWaveAid();
	TestGetPayWavePrgId();
	TestGetPayWaveTermCap();
	TestGetExpressPayAid();
	TestGetExpressPayConfig();
	TestGetOherConf();
	#endif		

	return iRet;
}

