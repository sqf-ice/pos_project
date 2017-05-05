/*
 * ============================================================================
 * COPYRIGHT
 *              Pax CORPORATION PROPRIETARY INFORMATION
 *   This software is supplied under the terms of a license agreement or
 *   nondisclosure agreement with Pax Corporation and may not be copied
 *   or disclosed except in accordance with the terms in that agreement.
 *      Copyright (C) 2016 Pax Corporation. All rights reserved.
 * Module Date:2016/10/08    
 * Module Auth:huangwp     
 * Description:emv xml file operarte module

 *      
 * ============================================================================
 */

#include "..\inc\global.h"


#define   LOG_TAG   "EMV_PARSE"
#ifdef EMV_PARSE_DEBUG		

#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr,...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine,pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)


#endif // EMV_PARSE_DEBUG

//#define EMV_PARSE_TEST


static int giAidIndex = 0;
static int giCapkIndex = 0;
static int giIcsIndex = 0;
static int giRevokeIndex = 0;
static int giTermInfoIndex = 0;
static int giCardSchemConfIndex = 0;

static int giCurrentOffset = 0;

static NODE_RECORD_T stNodeRecord[] =
{
		{"AID",0,0},
		{"CAPK",0,0},
		{"ICS",0,0},
		{"REVOKEDCERTIFICATE",0,0},
		{"TERMINALCONFIGURATION",0,0},
		{"CARDSCHEME",0,0},
};

static int GetRecordIndex(const uchar *pszNodeName, int iSize)
{
	int i = 0;

	for(i = 0; i < iSize; i++)
	{
		if(strcmp(pszNodeName, stNodeRecord[i].ucListNodeName) == 0)
		{
			return i;
		}
	}
	return EMV_XML_FILE_OP_ERR_RECORD_NOT_FOUNTD;
}


static int GetRecordData(NODE_RECORD_T *pststRecord, int iLen)
{
	int iFd = 0;
	int iFileSize = 0;
	int iHavaReadLen = 0;
	int iRet = 0;
	int i = 0;

	iRet = PdkFile_Exist(EMV_XML_FILE_HAVE_PARSED);
	if(iRet < 0 )
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}
	iFd = PdkFile_Open(EMV_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(EMV_XML_FILE_HAVE_PARSED);

	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
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
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			return iRet;
		}
	}

	PdkFile_Close(iFd);
	return 0;
}


int EmvGetNodeNum(const uchar *pszName)
{
	int iIndex = 0;
	int iRet = 0;
	
	NODE_RECORD_T stRecord[EMV_PRASED_RECORD_MAX_NODE_CNT] = {0};
	
	iRet = GetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		return iRet;
	}

	iIndex = GetRecordIndex(pszName, sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		return iIndex;
	}

	return stRecord[iIndex].count;

}

int SaveParsedDataToFile(const uchar *pszFileName, const uchar *pszInData, int iDataSize, int iOffset)
{
	int iFd = 0;
	int iLseek = 0;
	int iRet = 0;
	int iFileSize = 0;
	EMV_AID_NODE_T stAidNode = {0};
	int i = 0;
	
	iFd = PdkFile_Open(pszFileName);

	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
	}

	iFileSize = PdkFile_GetSize(pszFileName);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = PdkFile_Seek (iFd, iOffset, PDK_SEEK_SET);
	if(iLseek < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iLseek = %d\n",iLseek);
		return iLseek;
	}

	iRet = PdkFile_Write (iFd, pszInData, iDataSize);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		PdkFile_Close(iFd);
		return iRet;
	}
    
	iFileSize = PdkFile_GetSize(pszFileName);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}
	
	PdkFile_Close(iFd);
	return iRet;
	
}

int GetEmvAidData(EMV_AID_NODE_T *pstEmvAidNode, int iSize, int iAidIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[EMV_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = GetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("AID", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));

	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iAidIndex > stRecord[iIndex].count)
	{
		return EMV_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(EMV_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(EMV_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iAidIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstEmvAidNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvAidNode.SelFlag = %d\n",pstEmvAidNode->SelFlag);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.AID = ", pstEmvAidNode->AID, sizeof(pstEmvAidNode->AID));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvAidNode.IfUseLocalName = %d\n",pstEmvAidNode->IfUseLocalName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvAidNode.AppName = %s\n",pstEmvAidNode->AppName);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.Version = ", pstEmvAidNode->Version, sizeof(pstEmvAidNode->Version));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.TACDenial = ", pstEmvAidNode->TACDenial, sizeof(pstEmvAidNode->TACDenial));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.TACOnline = ", pstEmvAidNode->TACOnline, sizeof(pstEmvAidNode->TACOnline));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.TACDefault = ", pstEmvAidNode->TACDefault, sizeof(pstEmvAidNode->TACDefault));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvAidNode.FloorLimit = %ld\n",pstEmvAidNode->FloorLimit);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvAidNode.TargetPer = %d\n",pstEmvAidNode->TargetPer);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvAidNode.MaxTargetPer = %d\n",pstEmvAidNode->MaxTargetPer);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.tDOL = ", pstEmvAidNode->tDOL, sizeof(pstEmvAidNode->tDOL));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.dDOL = ", pstEmvAidNode->dDOL, sizeof(pstEmvAidNode->dDOL));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvAidData pstEmvAidNode.RiskManData = ", pstEmvAidNode->RiskManData, sizeof(pstEmvAidNode->RiskManData));

	PdkFile_Close(iFd);

	return 0;
}

#ifdef EMV_PARSE_TEST
int TestGetEmvAid()
{
	int iRet = 0;
	int iNum = 0;
	int i = 0;
	EMV_AID_NODE_T stEmvAidNode = {0};

	iNum = EmvGetNodeNum(EMV_AID_NODE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stEmvAidNode, 0, sizeof(stEmvAidNode));
		iRet = GetEmvAidData(&stEmvAidNode, sizeof(stEmvAidNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif


int GetEmvCapkData(EMV_CAPK_NODE_T *pstEmvCapkNode, int iSize, int iCapkIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[EMV_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = GetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("CAPK", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));

	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iCapkIndex > stRecord[iIndex].count)
	{
		return EMV_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(EMV_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(EMV_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iCapkIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstEmvCapkNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCapkData stCapkNode.RID = ", pstEmvCapkNode->RID, sizeof(pstEmvCapkNode->RID));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCapkData stCapkNode.KeyID = ", &(pstEmvCapkNode->KeyID), sizeof(pstEmvCapkNode->KeyID));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvCapkNode->HashInd = %d\n",pstEmvCapkNode->HashInd);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvCapkNode->ArithInd = %d\n",pstEmvCapkNode->ArithInd);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvCapkNode->ModulLen = %d\n",pstEmvCapkNode->ModulLen);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCapkData stCapkNode.Modul = ", pstEmvCapkNode->Modul, sizeof(pstEmvCapkNode->Modul));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvCapkNode->ExponentLen = %d\n",pstEmvCapkNode->ExponentLen);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCapkData stCapkNode.Exponent = ", pstEmvCapkNode->Exponent, sizeof(pstEmvCapkNode->Exponent));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCapkData stCapkNode.ExpDate = ", pstEmvCapkNode->ExpDate, sizeof(pstEmvCapkNode->ExpDate));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCapkData stCapkNode.CheckSum = ", pstEmvCapkNode->CheckSum, sizeof(pstEmvCapkNode->CheckSum));
	
	PdkFile_Close(iFd);

	return 0;
}

#ifdef EMV_PARSE_TEST
int TestGetEmvCapk()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	EMV_CAPK_NODE_T stEmvCapkNode = {0};

	iNum = EmvGetNodeNum(EMV_CAPK_NODE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stEmvCapkNode, 0, sizeof(stEmvCapkNode));
		GetEmvCapkData(&stEmvCapkNode, sizeof(stEmvCapkNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif

int GetEmvIcsData(EMV_ICS_NODE_T *pstEmvIcsNode, int iSize, int iIcsIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[EMV_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = GetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("ICS", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iIndex = %d\n",iIndex);
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d, stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iIcsIndex > stRecord[iIndex].count)
	{
		return EMV_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(EMV_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(EMV_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iIcsIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstEmvIcsNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvIcsNode->Type = %s\n",pstEmvIcsNode->Type);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvIcsData stIcsNode.TerminalType = ", &(pstEmvIcsNode->TerminalType), sizeof(pstEmvIcsNode->TerminalType));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvIcsData stIcsNode.CardDataInputCapability = ", &(pstEmvIcsNode->CardDataInputCapability), sizeof(pstEmvIcsNode->CardDataInputCapability));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvIcsData stIcsNode.CVMCapability = ", &(pstEmvIcsNode->CVMCapability), sizeof(pstEmvIcsNode->CVMCapability));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvIcsData stIcsNode.SecurityCapability = ", &(pstEmvIcsNode->SecurityCapability), sizeof(pstEmvIcsNode->SecurityCapability));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvIcsData stIcsNode.TerminalType = ",pstEmvIcsNode->AdditionalTerminalCapabilities, sizeof(pstEmvIcsNode->AdditionalTerminalCapabilities));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvIcsNode->GetDataForPINTryCounter = %d\n",pstEmvIcsNode->GetDataForPINTryCounter);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvIcsNode->BypassPINEntry = %d\n",pstEmvIcsNode->BypassPINEntry);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvIcsNode->SubsequentBypassPINEntry  = %d\n",pstEmvIcsNode->SubsequentBypassPINEntry);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvIcsNode->ExceptionFileSupported  = %d\n",pstEmvIcsNode->ExceptionFileSupported);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvIcsNode->ForcedOnlineCapability  = %d\n",pstEmvIcsNode->ForcedOnlineCapability);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvIcsNode->IssuerReferralsSupported  = %d\n",pstEmvIcsNode->IssuerReferralsSupported);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvIcsData stIcsNode.ConfigurationCheckSum = ",pstEmvIcsNode->ConfigurationCheckSum, sizeof(pstEmvIcsNode->ConfigurationCheckSum));

	PdkFile_Close(iFd);

	return 0;
}

#ifdef EMV_PARSE_TEST
int TestGetEmvIcs()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	EMV_ICS_NODE_T stEmvIcsNode = {0};

	iNum = EmvGetNodeNum(EMV_ICS_NODE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stEmvIcsNode, 0, sizeof(stEmvIcsNode));
		iRet = GetEmvIcsData(&stEmvIcsNode, sizeof(stEmvIcsNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif



int GetEmvRevokeData(EMV_REVOCLIST_T *pstEmvRevokeNode, int iSize, int iRevokeIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[EMV_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = GetRecordData(stRecord, sizeof(stRecord));

	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("REVOKEDCERTIFICATE", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));

	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iIndex);
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iRevokeIndex > stRecord[iIndex].count)
	{
		return EMV_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(EMV_XML_FILE_HAVE_PARSED);

	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(EMV_XML_FILE_HAVE_PARSED);

	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iRevokeIndex*iSize;
	//iRet = PdkFile_Seek (iFd, iBaseOffset, SEEK_CUR);
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);

	if(iRet < 0)
	{
		PdkFile_Close(iFd);	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
	iRet = PdkFile_Read(iFd, (uchar*)pstEmvRevokeNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvRevokeData stRevokeNode.ucRid = ",pstEmvRevokeNode->ucRid, sizeof(pstEmvRevokeNode->ucRid));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvRevokeData stRevokeNode.ucIndex = ",&(pstEmvRevokeNode->ucIndex), sizeof(pstEmvRevokeNode->ucIndex));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvRevokeData stRevokeNode.ucCertSn = ",pstEmvRevokeNode->ucCertSn, sizeof(pstEmvRevokeNode->ucCertSn));

	PdkFile_Close(iFd);

	return 0;
}

#ifdef EMV_PARSE_TEST
int TestGetEmvRevoke()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	EMV_REVOCLIST_T stEmvRevokNode = {0};

	iNum = EmvGetNodeNum(EMV_REVOKEDCERTIFICATE_NODE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stEmvRevokNode, 0, sizeof(stEmvRevokNode));
		iRet = GetEmvRevokeData(&stEmvRevokNode, sizeof(stEmvRevokNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n", iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif

int GetEmvCardSchemConfData(EMV_CARD_SCHEM_CONF_T *pstEmvCardSchemConfNode, int iSize, int iCardSchemConfIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[EMV_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = GetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("CARDSCHEME", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		return iIndex;
	}


//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetRecordData stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);


	if(iCardSchemConfIndex > stRecord[iIndex].count)
	{
		return EMV_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(EMV_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(EMV_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iCardSchemConfIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd); 
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}	
		
	iRet = PdkFile_Read(iFd, (uchar*)pstEmvCardSchemConfNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd); 
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}


	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCardSchemConfData pstEmvCardSchemConfNode.ucRid =  ",pstEmvCardSchemConfNode->ucRid, sizeof(pstEmvCardSchemConfNode->ucRid));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pstEmvCardSchemConfNode.ucIcsType = %s\n",pstEmvCardSchemConfNode->ucIcsType);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvCardSchemConfData pstEmvCardSchemConfNode.ucAid =  ",pstEmvCardSchemConfNode->ucAid, sizeof(pstEmvCardSchemConfNode->ucAid));

	PdkFile_Close(iFd);

	return 0;
}

#ifdef EMV_PARSE_TEST
int TestGetCardShcemConf()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	EMV_CARD_SCHEM_CONF_T stEmvCardSchemConfNode = {0};

	iNum = EmvGetNodeNum(EMV_CARD_SCHEM_CONF_NODE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
		memset(&stEmvCardSchemConfNode, 0, sizeof(stEmvCardSchemConfNode));
		iRet = GetEmvCardSchemConfData(&stEmvCardSchemConfNode, sizeof(stEmvCardSchemConfNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif



int GetEmvTermInfoData(EMV_TERMINALINFO_NODE_T *pstEmvTermInfoNode, int iSize, int iTermInfoIndex)
{
	int iFd = 0;
	int iFileSize = 0;
	int iRet = 0;
	int i = 0;
	int iBaseOffset = 0;
	int iOffset = 0;
	int iIndex = 0;
	int iLseek = 0 ;

	NODE_RECORD_T stRecord[EMV_PRASED_RECORD_MAX_NODE_CNT] = {0};

	iRet = GetRecordData(stRecord, sizeof(stRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("TERMINALCONFIGURATION", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		return iIndex;
	}

//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].iStartPos = %d,stRecord[%d].count = %d\n",iIndex,stRecord[iIndex].iStartPos,iIndex,stRecord[iIndex].count);
//	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"stRecord[%d].ucListNodeName = %s\n",iIndex,stRecord[iIndex].ucListNodeName);

	if(iTermInfoIndex > stRecord[iIndex].count)
	{
		return EMV_XML_FILE_OP_ERR_AID_OVER_FLOW;
	}
	
	iFd = PdkFile_Open(EMV_XML_FILE_HAVE_PARSED);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(EMV_XML_FILE_HAVE_PARSED);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = stRecord[iIndex].iStartPos + iTermInfoIndex*iSize;
	iRet = PdkFile_Seek (iFd, iLseek, SEEK_SET);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);	
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	} 	
	
	iRet = PdkFile_Read(iFd, (uchar*)pstEmvTermInfoNode, iSize);
	if(iRet < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseTerminalInfoList.aucMerchName = %s\n",pstEmvTermInfoNode->aucMerchName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseTerminalInfoList.aucMerchantLocation = %s\n",pstEmvTermInfoNode->aucMerchantLocation);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseTerminalInfoList.aucMerchId = %s\n",pstEmvTermInfoNode->aucMerchId);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseTerminalInfoList.aucTermId = %s\n",pstEmvTermInfoNode->aucTermId);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvTermInfoData ParseTerminalInfoList.aucCountryCode = ",pstEmvTermInfoNode->aucCountryCode, sizeof(pstEmvTermInfoNode->aucCountryCode));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvTermInfoData ParseTerminalInfoList.aucTransCurrCode = ",pstEmvTermInfoNode->aucTransCurrCode, sizeof(pstEmvTermInfoNode->aucTransCurrCode));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvTermInfoData ParseTerminalInfoList.aucReferCurrCode = ",pstEmvTermInfoNode->aucReferCurrCode, sizeof(pstEmvTermInfoNode->aucReferCurrCode));
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetEmvTermInfoData ParseTerminalInfoList.aucMerchCateCode = ",pstEmvTermInfoNode->aucMerchCateCode, sizeof(pstEmvTermInfoNode->aucMerchCateCode));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseTerminalInfoList.ucTerminalCurrencyExponent = %d\n",pstEmvTermInfoNode->ucTerminalCurrencyExponent);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseTerminalInfoList.ucReferenceCurrencyExponent = %d\n",pstEmvTermInfoNode->ucReferenceCurrencyExponent);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseTerminalInfoList.ucConversionRatio = %d\n",pstEmvTermInfoNode->ucConversionRatio);

	PdkFile_Close(iFd);

	return 0;
}

#ifdef EMV_PARSE_TEST
int TestGetEmvTermInfo()
{
	int iRet = 0;
	int i = 0;
	int iNum = 0;
	EMV_TERMINALINFO_NODE_T stEmvTermInfoNode = {0};

	iNum = EmvGetNodeNum(EMV_TERMINALCONFIGURATION_NODE);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iNum = %d\n",iNum);
	if(iNum < 0)
	{
		return iNum;
	}

	for(i = 0; i < iNum; i++)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"i = %d\n",i);
		memset(&stEmvTermInfoNode, 0, sizeof(stEmvTermInfoNode));
		iRet = GetEmvTermInfoData(&stEmvTermInfoNode, sizeof(stEmvTermInfoNode), i);
		if(iRet < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet,i);
			return iRet;
		}
	}
	return 0;
}
#endif



static int SaveNodeStruct(const uchar *pszNodeName, void *data, int iLen)
{
	int iOffset = 0;
	int iBaseOffset = 0;
	int iRet = 0;
	int iSaveLen = 0;

	if(strcmp(pszNodeName, "<AID>") == 0)
	{
		EMV_AID_NODE_T *stAidNode = NULL;
		stAidNode = (EMV_AID_NODE_T *)data;

//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giAidIndex = %d\n",giAidIndex);
//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giCurrentOffset = %d\n",giCurrentOffset);

		iSaveLen = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)stAidNode, iLen, giCurrentOffset);
		if(iSaveLen < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iSaveLen = %d\n",iSaveLen);
			return iSaveLen;
		}
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iSaveLen = %d\n",iSaveLen);
		if(giAidIndex == 0)
		{
			iRet = GetRecordIndex("AID", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			if(iRet < 0)
			{
				return iRet;
			}
			stNodeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giAidIndex++;
	}
	else if(strcmp(pszNodeName, "<CAPK>") == 0)
	{
		EMV_CAPK_NODE_T *stCapkNode = NULL;
		stCapkNode = (EMV_CAPK_NODE_T *)data;

//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iCapkIndex = %d\n",giCapkIndex);
//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giCurrentOffset = %d\n",giCurrentOffset);


		iSaveLen = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)stCapkNode, iLen, giCurrentOffset);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iSaveLen = %d\n",iSaveLen);
		if(iSaveLen < 0)
		{
			return iSaveLen;
		}
		
		if(giCapkIndex == 0)
		{
			iRet = GetRecordIndex("CAPK", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			if(iRet < 0)
			{
				return iRet;
			}
			stNodeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giCapkIndex++;
	}
	else if(strcmp(pszNodeName, "<ICS>") == 0)
	{
		EMV_ICS_NODE_T *stIcsNode = NULL;
		stIcsNode = (EMV_ICS_NODE_T *)data;

//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giIcsIndex = %d\n",giIcsIndex);
//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giCurrentOffset = %d\n",giCurrentOffset);


		iSaveLen = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)stIcsNode, iLen, giCurrentOffset);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		if(iSaveLen < 0)
		{
			return iSaveLen;
		}
		
		if(giIcsIndex == 0)
		{
			iRet = GetRecordIndex("ICS", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			if(iRet < 0)
			{
				return iRet;
			}
			
			stNodeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giIcsIndex++;
	}
	else if(strcmp(pszNodeName, "<REVOKEDCERTIFICATE>") == 0)
	{
		EMV_REVOCLIST_T *stRevokeNode = NULL;
		stRevokeNode = (EMV_REVOCLIST_T *)data;

		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giRevokeIndex = %d\n",giRevokeIndex);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giCurrentOffset = %d\n",giCurrentOffset);

		iSaveLen = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)stRevokeNode, iLen, giCurrentOffset);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iSaveLen = %d\n",iSaveLen);
		if(iSaveLen < 0)
		{
			return iSaveLen;
		}
		
		if(giRevokeIndex == 0)
		{
			iRet = GetRecordIndex("REVOKEDCERTIFICATE", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			if(iRet < 0)
			{
				return iRet;
			}
			stNodeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giRevokeIndex++;
	}
	else if(strcmp(pszNodeName, "<CARDSCHEME>") == 0)
	{
		EMV_CARD_SCHEM_CONF_T *stCardShcemConfNode = NULL;
		stCardShcemConfNode = (EMV_CARD_SCHEM_CONF_T *)data;

//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giCardSchemConfIndex = %d\n",giCardSchemConfIndex);
//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"sgiCurrentOffset = %d\n",giCurrentOffset);

		iSaveLen = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)stCardShcemConfNode, iLen, giCurrentOffset);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iSaveLen = %d\n",iSaveLen);
		if(iSaveLen < 0)
		{
			return iSaveLen;
		}
		
		if(giCardSchemConfIndex == 0)
		{
			iRet = GetRecordIndex("CARDSCHEME", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			if(iRet < 0)
			{
				return iRet;
			}
			stNodeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giCardSchemConfIndex++;
	}
		
	else if(strcmp(pszNodeName, "<TERMINALCONFIGURATION>") == 0)
	{
		EMV_TERMINALINFO_NODE_T *stTermInfoNode = NULL;
		stTermInfoNode = (EMV_TERMINALINFO_NODE_T *)data;

//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giTermInfoIndex = %d\n",giTermInfoIndex);
//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"giCurrentOffset = %d\n",giCurrentOffset);



		iSaveLen = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)stTermInfoNode, iLen, giCurrentOffset);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iSaveLen = %d\n",iSaveLen);

		if(iSaveLen < 0)
		{
			return iSaveLen;
		}
		
		if(giTermInfoIndex == 0)
		{
			iRet = GetRecordIndex("TERMINALCONFIGURATION", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetRecordIndex = %d\n",GetRecordIndex);
			if(iRet < 0)
			{
				return iRet;
			}
			stNodeRecord[iRet].iStartPos = giCurrentOffset;
		}
		giCurrentOffset += iSaveLen;
		giTermInfoIndex++;
	}
	return giCurrentOffset;
}


//begin to parse aid
static int GetAidChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "</AID>") != NULL)
	{
		return EMV_XML_FILE_OP_OK;
	}
		
	if(strstr(pszXmlOneLine, "<PartialAIDSelection") != NULL)
	{
		return PARTIALAIDSELECTION;
	}
	if(strstr(pszXmlOneLine, "<ApplicationID") != NULL)
	{
		return APPLICATIONID;
	}
	if(strstr(pszXmlOneLine, "<IfUseLocalAIDName") != NULL)
	{
		return IFUSELOCALNAME;
	}
	if(strstr(pszXmlOneLine, "<LocalAIDName") != NULL)
	{
		return LOACALAIDNAME;
	}
	if(strstr(pszXmlOneLine, "<TerminalAIDVersion") != NULL)
	{
		return TERMINALAIDVERSION;
	}
	if(strstr(pszXmlOneLine, "<TACDenial") != NULL)
	{
		return TACDENIAL;
	}
	if(strstr(pszXmlOneLine, "<TACOnline") != NULL)
	{
		return TACONLINE;
	}
	if(strstr(pszXmlOneLine, "<TACDefault") != NULL)
	{
		return TACDEFAULT;
	}
	if(strstr(pszXmlOneLine, "<FloorLimit") != NULL)
	{
		return FLOORLIMIT;
	}
	if(strstr(pszXmlOneLine, "<Threshold") != NULL)
	{
		return THRESHOLD;
	}
	if(strstr(pszXmlOneLine, "<TargetPercentage") != NULL)
	{
		return TARGETPERCENTAGE;
	}
	if(strstr(pszXmlOneLine, "<MaxTargetPercentage") != NULL)
	{
		return MAXTARGETPERCENTAGE;
	}
	if(strstr(pszXmlOneLine, "<TerminalDefaultTDOL") != NULL)
	{
		return TERMINALDEFAULTTDOL;
	}
	if(strstr(pszXmlOneLine, "<TerminalDefaultDDOL") != NULL)
	{
		return TERMINALDEFAULTDDOL;
	}
	if(strstr(pszXmlOneLine, "<TerminalRiskManagementData") != NULL)
	{
		return TERMINALRISKMANAGEMENTDATA;
	}
	else
	{
		return 0;
	}

}

//static int ParseEmvAidChildNode(const uchar *pszXmlOneLine, EMV_AID_NODE_T *pstAidNode)
static int ParseEmvAidChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iAidChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	EMV_AID_NODE_T *stAidNode = NULL;
	stAidNode = (EMV_AID_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iAidChildNode = GetAidChileNode(pszXmlOneLine);
	if(iAidChildNode < 0)
	{
		return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iAidChildNode)
	{
		case PARTIALAIDSELECTION:
			iRet = GetUcharData(pszXmlOneLine,"PartialAIDSelection", &(stAidNode->SelFlag));
			break;
		case APPLICATIONID:
			iRet = GetBcdData(pszXmlOneLine, "ApplicationID", stAidNode->AID);
			stAidNode->AidLen = iRet;
			break;
		case IFUSELOCALNAME:
			iRet = GetIntegerData(pszXmlOneLine, "IfUseLocalAIDName", &(stAidNode->IfUseLocalName));
			break;
		case LOACALAIDNAME:
			iRet = GetStringData(pszXmlOneLine, "LocalAIDName",stAidNode->AppName);
			break;
		case TERMINALAIDVERSION:
			iRet = GetBcdData(pszXmlOneLine, "TerminalAIDVersion", stAidNode->Version);
			break;
		case TACDENIAL:
			iRet = GetBcdData(pszXmlOneLine, "TACDenial", stAidNode->TACDenial);
			break;
		case TACONLINE:
			iRet = GetBcdData(pszXmlOneLine, "TACOnline", stAidNode->TACOnline);
			break;
		case TACDEFAULT:
			iRet = GetBcdData(pszXmlOneLine, "TACDefault", stAidNode->TACDefault);
			break;
		case FLOORLIMIT:
			iRet = GetUlongData(pszXmlOneLine, "FloorLimit", &(stAidNode->FloorLimit));
			break;
		case THRESHOLD:
			iRet = GetUlongData(pszXmlOneLine, "Threshold",&(stAidNode->Threshold));
			break;
		case TARGETPERCENTAGE:
			iRet = GetUcharData(pszXmlOneLine, "TargetPercentage", &(stAidNode->TargetPer));
			break;
		case MAXTARGETPERCENTAGE:
			iRet = GetUcharData(pszXmlOneLine,"MaxTargetPercentage", &(stAidNode->MaxTargetPer));
			break;
		case TERMINALDEFAULTTDOL:
			iRet = GetBcdData(pszXmlOneLine, "TerminalDefaultTDOL", stAidNode->tDOL);
			break;
		case TERMINALDEFAULTDDOL:
			iRet = GetBcdData(pszXmlOneLine, "TerminalDefaultDDOL", stAidNode->dDOL);
			break;

		case TERMINALRISKMANAGEMENTDATA:
			iRet = GetBcdData(pszXmlOneLine, "TerminalRiskManagementData",stAidNode->RiskManData);
			break;

		default:
			break;
	}
	
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetAidChileNode = %d iSize = %d iRet = %d\n",iAidChildNode, iSize, iRet);
	
	return iRet;
}



//parse capk

static int GetCapkChileNode(const uchar *pszXmlOneLine)
{
	
	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "<RID") != NULL)
	{
		return RID;
	}
	if(strstr(pszXmlOneLine, "<KeyID") != NULL)
	{
		return KEYID;
	}
	if(strstr(pszXmlOneLine, "<HashArithmeticIndex") != NULL)
	{
		return HASHARITHMETICINDEX;
	}
	if(strstr(pszXmlOneLine, "<RSAArithmeticIndex") != NULL)
	{
		return RSAARITHMRTICINDEX;
	}
	if(strstr(pszXmlOneLine, "<ModuleLength") != NULL)
	{
		return MODULELENGTH;
	}
	if(strstr(pszXmlOneLine, "<Module") != NULL)
	{
		return MODULE;
	}
	if(strstr(pszXmlOneLine, "<ExponentLength") != NULL)
	{
		return EXPONENTLENGTH;
	}
	if(strstr(pszXmlOneLine, "<Exponent") != NULL)
	{
		return EXPONENT;
	}
	if(strstr(pszXmlOneLine, "<ExpireDate") != NULL)
	{
		return EXPIREDATE;
	}
	if(strstr(pszXmlOneLine, "<CheckSum") != NULL)
	{
		return CHECKSUM;
	}
	else
	{
		return 0;
	}

}

//static int ParseEmvCapkChildNode(const uchar *pszXmlOneLine, EMV_CAPK_NODE_T *pstCapkNode)
static int ParseEmvCapkChildNode(const uchar *pszXmlOneLine, void *Data)

{
	int iCapkChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	EMV_CAPK_NODE_T *stCapkNode = NULL;
	stCapkNode = (EMV_CAPK_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iCapkChildNode = GetCapkChileNode(pszXmlOneLine);

	
	if(iCapkChildNode < 0)
	{
		return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iCapkChildNode)
	{
		case RID:
			iRet = GetBcdData(pszXmlOneLine, "RID",stCapkNode->RID);
			break;
		case KEYID:
			iRet = GetBcdData(pszXmlOneLine, "KeyID", &(stCapkNode->KeyID));
			break;
		case HASHARITHMETICINDEX:
			iRet = GetUcharData(pszXmlOneLine, "HashArithmeticIndex",&(stCapkNode->HashInd));
			break;
		case RSAARITHMRTICINDEX:
			iRet = GetUcharData(pszXmlOneLine, "RSAArithmeticIndex", &(stCapkNode->ArithInd));
			break;
		case MODULELENGTH:
			iRet = GetUcharData(pszXmlOneLine, "ModuleLength", &(stCapkNode->ModulLen));
			break;
		case MODULE:
			iRet = GetBcdData(pszXmlOneLine,"Module", stCapkNode->Modul);
			break;
		case EXPONENTLENGTH:
			iRet = GetUcharData(pszXmlOneLine, "ExponentLength", &(stCapkNode->ExponentLen));
			break;
		case EXPONENT:
			iRet = GetBcdData(pszXmlOneLine, "Exponent", stCapkNode->Exponent);
			break;
		case EXPIREDATE:
			iRet = GetBcdData(pszXmlOneLine, "ExpireDate",stCapkNode->ExpDate);
			break;
		case CHECKSUM:
			iRet = GetBcdData(pszXmlOneLine, "CheckSum",stCapkNode->CheckSum);
			break;

		default:
			break;
	}
	
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetCapkChileNode = %d iSize = %d iRet = %d\n",iCapkChildNode, iSize, iRet);
	
	return iRet;
}


//begine to parse ics

static int GetIcsChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "<Type") != NULL)
	{
		return TYPE;
	}
	if(strstr(pszXmlOneLine, "<TerminalType") != NULL)
	{
		return TERMINALTYPE;
	}
	if(strstr(pszXmlOneLine, "<CardDataInputCapability") != NULL)
	{
		return CARDDATAINPUTCAPABILITY;
	}
	if(strstr(pszXmlOneLine, "<CVMCapability") != NULL)
	{
		return CVMCAPABILITY;
	}
	if(strstr(pszXmlOneLine, "<SecurityCapability") != NULL)
	{
		return SECURITYCAPABILITY;
	}
	if(strstr(pszXmlOneLine, "<AdditionalTerminalCapabilities") != NULL)
	{
		return ADDITIONALTERMINALCAPABILITIES;
	}
	if(strstr(pszXmlOneLine, "<GetDataForPINTryCounter") != NULL)
	{
		return GETDATAFORPINTRYCOUNTER;
	}
	if(strstr(pszXmlOneLine, "<BypassPINEntry") != NULL)
	{
		return BYPASSPINENTRY;
	}
	if(strstr(pszXmlOneLine, "<SubsequentBypassPINEntry") != NULL)
	{
		return SUBSEQUENBYPASSPINENTRY;
	}
	if(strstr(pszXmlOneLine, "<ExceptionFileSupported") != NULL)
	{
		return EXCEPTIONFILESUPPORTED;
	}
	if(strstr(pszXmlOneLine, "<ForcedOnlineCapability") != NULL)
	{
		return FORCEDONLINECAPABILITY;
	}
	if(strstr(pszXmlOneLine, "<IssuerReferralsSupported") != NULL)
	{
		return ISSUERFEFERRALSSUPPORTED;
	}
	if(strstr(pszXmlOneLine, "<ConfigurationCheckSum") != NULL)
	{
		return CONFIGURATIONCHECKSUM;
	}
	else
	{
		return 0;
	}

}


static int ParseEmvIcsChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iIcsChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;
	
	EMV_ICS_NODE_T *stIcsNode = NULL;
	stIcsNode = (EMV_ICS_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iIcsChildNode = GetIcsChileNode(pszXmlOneLine);
	if(iIcsChildNode < 0)
	{
		return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iIcsChildNode)
	{
		case TYPE:
			iRet = GetStringData(pszXmlOneLine, "Type", stIcsNode->Type);
			break;
		case TERMINALTYPE:
			iRet = GetBcdData(pszXmlOneLine, "TerminalType",&(stIcsNode->TerminalType));
			break;
		case CARDDATAINPUTCAPABILITY:
			iRet = GetBcdData(pszXmlOneLine, "CardDataInputCapability", &(stIcsNode->CardDataInputCapability));
			break;
		case CVMCAPABILITY:
			iRet = GetBcdData(pszXmlOneLine, "CVMCapability", &(stIcsNode->CVMCapability));
			break;
		case SECURITYCAPABILITY:
			iRet = GetBcdData(pszXmlOneLine, "SecurityCapability", &(stIcsNode->SecurityCapability));
			break;
		case ADDITIONALTERMINALCAPABILITIES:
			iRet = GetBcdData(pszXmlOneLine, "AdditionalTerminalCapabilities", stIcsNode->AdditionalTerminalCapabilities);
			break;
		case GETDATAFORPINTRYCOUNTER:
			iRet = GetIntegerData(pszXmlOneLine, "GetDataForPINTryCounter", &(stIcsNode->GetDataForPINTryCounter));
			break;
		case BYPASSPINENTRY:
			iRet = GetIntegerData(pszXmlOneLine, "BypassPINEntry", &(stIcsNode->BypassPINEntry));
			break;
		case SUBSEQUENBYPASSPINENTRY:
			iRet = GetIntegerData(pszXmlOneLine, "SubsequentBypassPINEntry", &(stIcsNode->SubsequentBypassPINEntry));
			break;
		case EXCEPTIONFILESUPPORTED:
			iRet = GetIntegerData(pszXmlOneLine, "ExceptionFileSupported", &(stIcsNode->ExceptionFileSupported));
			break;
		case FORCEDONLINECAPABILITY:
			iRet = GetIntegerData(pszXmlOneLine, "ForcedOnlineCapability", &(stIcsNode->ForcedOnlineCapability));
			break;
		case ISSUERFEFERRALSSUPPORTED:
			iRet = GetIntegerData(pszXmlOneLine, "IssuerReferralsSupported", &(stIcsNode->IssuerReferralsSupported));
			break;
		case CONFIGURATIONCHECKSUM:
			iRet = GetBcdData(pszXmlOneLine, "ConfigurationCheckSum", stIcsNode->ConfigurationCheckSum);
			
			break;
		default:
			break;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetIcsChileNode = %d iSize = %d iRet = %d\n",iIcsChildNode, iSize, iRet);
	
	return iRet;
}


//begine to parse REVOCATIONLIST
static int GetRevoketionChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "<RID") != NULL)
	{
		return REVOKERID;
	}
	if(strstr(pszXmlOneLine, "<KeyID") != NULL)
	{
		return REVOKEKEYID;
	}
	if(strstr(pszXmlOneLine, "<CertificateSN") != NULL)
	{
		return CERTIFICATESN;
	}
	else
	{
		return 0;
	}

}



static int ParseEmvRevokeCertificateChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iRevokeChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;

	EMV_REVOCLIST_T *stRevokeNode = NULL;
	stRevokeNode = (EMV_REVOCLIST_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iRevokeChildNode = GetRevoketionChileNode(pszXmlOneLine);

	
	if(iRevokeChildNode < 0)
	{
		return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iRevokeChildNode)   	
	{
		case REVOKERID:
			iRet = GetBcdData(pszXmlOneLine, "RID", stRevokeNode->ucRid);
			break;
		case REVOKEKEYID:
			iRet = GetBcdData(pszXmlOneLine, "KeyID", &(stRevokeNode->ucIndex));
			break;
		case CERTIFICATESN:
			iRet = GetBcdData(pszXmlOneLine, "CertificateSN", stRevokeNode->ucCertSn);
			break;

		default:
			break;
	}
	
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetRevoketionChileNode = %d iSize = %d iRet = %d\n",iRevokeChildNode, iSize, iRet);
	
	return iRet;
}


//begine to parse terminal info
static int GetTerminalInfoChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "<MerchantID") != NULL)
	{
		return MERCHANTID;
	}
	if(strstr(pszXmlOneLine, "<TerminalID") != NULL)
	{
		return TERMINALID;
	}
	if(strstr(pszXmlOneLine, "<TerminalCountryCode") != NULL)
	{
		return TERMINALCOUNTRYCODE;
	}
	if(strstr(pszXmlOneLine, "<TerminalCurrencyCode") != NULL)
	{
		return TERMINALCURRENCYCODE;
	}
	if(strstr(pszXmlOneLine, "<TerminalCurrencyExponent") != NULL)
	{
		return TERMINALCURRENTCYEXPONENT;
	}
	if(strstr(pszXmlOneLine, "<ReferenceCurrencyCode") != NULL)
	{
		return REFERENCECURRENCYCODE;
	}
	if(strstr(pszXmlOneLine, "<ReferenceCurrencyExponent") != NULL)
	{
		return REFERENCECURRENTEXPONENT;
	}
	if(strstr(pszXmlOneLine, "<ConversionRatio") != NULL)
	{
		return CONVERSIONRATIO;
	}
	if(strstr(pszXmlOneLine, "<MerchantName") != NULL)
	{
		return MECHANTNAME;
	}
	if(strstr(pszXmlOneLine, "<MerchantLocation") != NULL)
	{
		return MERCHANTLOCATION;
	}
	if(strstr(pszXmlOneLine, "<MerchantCategoryCode") != NULL)
	{
		return MERCHANTCATEGORYCODE;
	}
	else
	{
		return 0;
	}

}


static int ParseEmvTerminalInfoChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iTermInfoChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;

	EMV_TERMINALINFO_NODE_T *stTermInfoNode = NULL;
	stTermInfoNode = (EMV_TERMINALINFO_NODE_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iTermInfoChildNode = GetTerminalInfoChileNode(pszXmlOneLine);

	if(iTermInfoChildNode < 0)
	{
		return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iTermInfoChildNode)
	{
		case MERCHANTID:
			iRet = GetStringData(pszXmlOneLine, "MerchantID", stTermInfoNode->aucMerchId);
			break;
		case TERMINALID:
			iRet = GetStringData(pszXmlOneLine, "TerminalID", stTermInfoNode->aucTermId);
			break;
		case TERMINALCOUNTRYCODE:
			iRet = GetBcdData(pszXmlOneLine, "TerminalCountryCode", stTermInfoNode->aucCountryCode);
			break;
		case TERMINALCURRENCYCODE:
			iRet = GetBcdData(pszXmlOneLine, "TerminalCurrencyCode", stTermInfoNode->aucTransCurrCode);
			break;
		case TERMINALCURRENTCYEXPONENT:
			iRet = GetUcharData(pszXmlOneLine, "TerminalCurrencyExponent", &(stTermInfoNode->ucTerminalCurrencyExponent));
			break;
		case REFERENCECURRENCYCODE:
			iRet = GetBcdData(pszXmlOneLine, "ReferenceCurrencyCode", stTermInfoNode->aucReferCurrCode);
			break;
		case REFERENCECURRENTEXPONENT:
			iRet = GetUcharData(pszXmlOneLine, "ReferenceCurrencyExponent", &(stTermInfoNode->ucReferenceCurrencyExponent));
			break;
		case CONVERSIONRATIO:
			iRet = GetUcharData(pszXmlOneLine, "ConversionRatio", &(stTermInfoNode->ucConversionRatio));
			break;
		case MECHANTNAME:
			iRet = GetStringData(pszXmlOneLine, "MerchantName", stTermInfoNode->aucMerchName);
			break;
		case MERCHANTLOCATION:
			iRet = GetStringData(pszXmlOneLine, "MerchantLocation", stTermInfoNode->aucMerchantLocation);
			break;
		case MERCHANTCATEGORYCODE:
			iRet = GetBcdData(pszXmlOneLine, "MerchantCategoryCode", stTermInfoNode->aucMerchCateCode);
			break;

		default:
			break;
	}
	
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetTerminalInfoChileNode = %d iSize = %d iRet = %d\n",iTermInfoChildNode, iSize, iRet);

	return iRet;
}

static int GetCardSchemConfChileNode(const uchar *pszXmlOneLine)
{
	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pszXmlOneLine, "<RID") != NULL)
	{
		return CARD_SCHEME_CONF_RID;
	}
	if(strstr(pszXmlOneLine, "<ICSTYPE") != NULL)
	{
		return CARD_SCHEME_ICS_TYPE;
	}
	if(strstr(pszXmlOneLine, "<AID") != NULL)
	{
		return CARD_SCHEME_ICS_AID;
	}
	else
	{
		return 0;
	}

}



static int ParseEmvCardSchemConfChildNode(const uchar *pszXmlOneLine, void *Data)
{
	int iCardShcemConfChildNode = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;

	EMV_CARD_SCHEM_CONF_T *stCardSchemConfNode = NULL;
	stCardSchemConfNode = (EMV_CARD_SCHEM_CONF_T *)Data;

	if(pszXmlOneLine == NULL)
	{
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iCardShcemConfChildNode = GetCardSchemConfChileNode(pszXmlOneLine);

	
	if(iCardShcemConfChildNode < 0)
	{
		return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
	}

	switch(iCardShcemConfChildNode)   	
	{
		case CARD_SCHEME_CONF_RID:
			iRet = GetBcdData(pszXmlOneLine, "RID", stCardSchemConfNode->ucRid);
			break;
		case CARD_SCHEME_ICS_TYPE:
			iRet = GetStringData(pszXmlOneLine, "ICSTYPE", stCardSchemConfNode->ucIcsType);
			break;
		case CARD_SCHEME_ICS_AID:
			iRet = GetBcdData(pszXmlOneLine, "AID", stCardSchemConfNode->ucAid);
			break;

		default:
			break;
	}
	
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetCardSchemConfChileNode = %d iSize = %d iRet = %d\n",iCardShcemConfChildNode, iSize, iRet);

	return iRet;
}



//static int FindFunctionIndex(const uchar *pszXmlOneLine, const uchar *pszNodeName)
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
	return EMV_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
}

static int FindNodePositonInFile(const uchar *pszFileName, const uchar *pszNodeName, int iPos)
{
	int iFd = 0;
	int iFileSize = 0;
	int iSearchLen = 0;
	uchar ucReadBuf[EMV_FILE_READ_BUF_LEN + 1] = {0};
	uchar ucTempBuf[EMV_TEMP_BUF_LEN + 1] = {0};
	uchar ucParseBuf[EMV_FILE_READ_BUF_LEN + 1] = {0};
	uchar *pszStart  = NULL;
	uchar *pszLastEnterPos = NULL;
	int iRet = 0;
	int iOffset = 0;
	ulong uiPosition = 0;
	int iLseek = 0;

	iRet = PdkFile_Exist (pszFileName);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iFd = PdkFile_Open(pszFileName);
	if(iFd < 0)
	{
		return iFd;
	}

	iFileSize = PdkFile_GetSize(pszFileName);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = PdkFile_Seek (iFd, iPos, SEEK_CUR);
	if(iLseek < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iLseek = %d\n",iLseek);
		return iLseek;
	}

	while(iSearchLen < iFileSize)
	{
		memset(ucReadBuf, 0, sizeof(ucReadBuf));
		iRet = PdkFile_Read(iFd, ucReadBuf, 1024);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			return iRet;
		}

		//every time ,we should confirm the last line is whole line
		pszLastEnterPos = strrchr(ucReadBuf,'\x0a');
		if(pszLastEnterPos == NULL)
		{
			PdkFile_Close(iFd);
			return EMV_XML_FILE_OP_ERR_ONELINE_OVER_1024;
		}

		memcpy(ucParseBuf, ucReadBuf, pszLastEnterPos - ucReadBuf);
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
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iLseek = %d\n",iLseek);
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
	return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
}


//static int ParseList(const uchar *pszFileName, void *OutData, const uchar *pszListStart, const uchar *pszListEnd,  const uchar *pszNodeStart, const uchar *pszNodeEnd, int *iFlag)
static int ParseList(const uchar *pszFileName, const uchar *pszListStart, const uchar *pszListEnd,  const uchar *pszNodeStart, const uchar *pszNodeEnd)
{
	int iRet = 0;
	int iListPos = 0;
	uchar *pszNodeEndTag = NULL;
	uchar *pszListEndTag = NULL;
	int iFd = 0;
	int iFileSize = 0;
	int iLseek = 0;
	uchar aucReadBuf[EMV_FILE_READ_BUF_LEN + 1] = {0};
	uchar aucParseBuf[EMV_FILE_READ_BUF_LEN + 1] = {0};
	uchar aucXmlLineBuf[EMV_FILE_ONELINE_BUF_LEN] = {0};
	uchar *pszLastEnterPos = NULL;
	int iLen = 0;
	int iHaveGetLen = 0;
	int iIndex = 0;
	int iReadLen = 0;

	EMV_AID_NODE_T stAidNode = {0};
	EMV_CAPK_NODE_T stCapkNode = {0};
	EMV_ICS_NODE_T stIcsNode = {0};
	EMV_REVOCLIST_T stRevokeNode = {0};
	EMV_TERMINALINFO_NODE_T stTermInfoNode = {0};
	EMV_CARD_SCHEM_CONF_T stCardSchemConf = {0};
	

	NODE_FUNC_TAB_T vParseListFuncTab[] =
	{
		{"<AID>", ParseEmvAidChildNode, &stAidNode, sizeof(stAidNode)},
		{"<CAPK>", ParseEmvCapkChildNode, &stCapkNode, sizeof(stCapkNode)},
		{"<ICS>", ParseEmvIcsChildNode, &stIcsNode, sizeof(stIcsNode)},
		{"<REVOKEDCERTIFICATE>", ParseEmvRevokeCertificateChildNode, &stRevokeNode, sizeof(stRevokeNode)},
		{"<TERMINALCONFIGURATION>", ParseEmvTerminalInfoChildNode, &stTermInfoNode, sizeof(stTermInfoNode)},
		{"<CARDSCHEME>", ParseEmvCardSchemConfChildNode, &stCardSchemConf, sizeof(stCardSchemConf)},
	};

	if(pszNodeStart == NULL || pszNodeEnd == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseList EMV_XML_FILE_OP_ERR_INVALID_PARAM NULL\n");
		return EMV_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	if(pszListStart != NULL)
	{
		iListPos = FindNodePositonInFile(pszFileName, pszListStart, 0);
		if(iListPos < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iListPos = %d\n",iListPos);
			return iListPos;
		}
	}
	else
	{
		iListPos = FindNodePositonInFile(pszFileName, pszNodeStart, 0);
		if(iListPos < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iListPos = %d\n",iListPos);
			return iListPos;
		}
	}
	
	iFd = PdkFile_Open(pszFileName);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(pszFileName);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = PdkFile_Seek (iFd, iListPos, SEEK_CUR);
	if(iLseek < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iLseek = %d\n",iLseek);
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
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			return iRet;
		}
		iReadLen +=  iRet;

		//every time ,we should confirm the last line is whole line
		pszLastEnterPos = strrchr(aucReadBuf,'\x0a');
		if(pszLastEnterPos == NULL)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pszLastEnterPos == NULL\n");
			return EMV_XML_FILE_OP_ERR_ONELINE_OVER_1024;
		}
	
		memset(aucParseBuf, 0, sizeof(aucParseBuf));
		memcpy(aucParseBuf, aucReadBuf, pszLastEnterPos - aucReadBuf + 1);
		
//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseList  aucParseBuf = %s\n",aucParseBuf);
//		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"ParseList  pszNodeStart = %s\n",pszNodeStart);
			
		while((iHaveGetLen) < (pszLastEnterPos - aucReadBuf + 1))
		{
			memset(aucXmlLineBuf, 0, sizeof(aucXmlLineBuf));
			//offset mean hava get len,the parsebuf's data every line is whole line   include \r\n	
			iRet = XmlGetOneLineData(aucParseBuf + iHaveGetLen, &iLen, aucXmlLineBuf);
			
//			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," XmlGetOneLineData ucXmlLineBuf = %s\n",aucXmlLineBuf);
//			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," XmlGetOneLineData iLen = %d\n",iLen);

			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," XmlGetOneLineData iRet = %d\n",iRet);
				PdkFile_Close (iFd);
				return iRet;
			}
			iHaveGetLen += iLen;
			
//			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," XmlGetOneLineData iHaveGetLen = %d,(pszLastEnterPos - ucReadBuf + 1) = %d\n",iHaveGetLen, pszLastEnterPos - aucReadBuf + 1);
			
			//init the buf that use to save data
			if(strstr(aucXmlLineBuf, pszNodeStart) != NULL)
			{
				iIndex = FindFunctionIndex(pszNodeStart, vParseListFuncTab, sizeof(vParseListFuncTab)/sizeof(vParseListFuncTab[0]));
				
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
				
				if(iIndex == EMV_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD)
				{
					PdkFile_Close(iFd);
					return EMV_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
				}
				memset(vParseListFuncTab[iIndex].pOutData ,0,vParseListFuncTab[iIndex].iLen);
				//continue;
			}

			//for list
			if((pszListEnd != NULL) && (pszListStart != NULL))
			{
				//EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," pszListEnd = %s\n",pszListEnd);
				
				pszListEndTag = strstr(aucXmlLineBuf, pszListEnd);
				if(pszListEndTag != NULL)
				{
					PdkFile_Close(iFd);
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pszListEndTag pszListEndTag != NULL\n");
					return EMV_XML_FILE_LIST_END;
				}
				
				//EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pszNodeEnd pszNodeEnd = %s\n",pszNodeEnd);
				
				pszNodeEndTag = strstr(aucXmlLineBuf, pszNodeEnd);
				if(pszNodeEndTag != NULL)
				{
					 EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pszNodeEndTag pszNodeEndTag != NULL return XML_PARSE_OK\n");
					 //print and save data to struct
					 iRet = SaveNodeStruct(pszNodeStart, vParseListFuncTab[iIndex].pOutData, vParseListFuncTab[iIndex].iLen);
					 if(iRet < 0)
					 {
					 	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
					 	return iRet;
					 }
					//continue;
				}
				else
				{
					iIndex = FindFunctionIndex(pszNodeStart, vParseListFuncTab, sizeof(vParseListFuncTab)/sizeof(vParseListFuncTab[0]));
					
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
					
					if(iIndex == EMV_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD)
					{
						PdkFile_Close(iFd);
						return EMV_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
					}
					if((strstr(aucXmlLineBuf, pszNodeStart) == NULL) && (strstr(aucXmlLineBuf, pszListStart) == NULL))
				    {
						iRet = vParseListFuncTab[iIndex].vFunc(aucXmlLineBuf, vParseListFuncTab[iIndex].pOutData);
						if(iRet < 0)
						{
							EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
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
					iRet = SaveNodeStruct(pszNodeStart, vParseListFuncTab[iIndex].pOutData, vParseListFuncTab[iIndex].iLen);
					if(iRet < 0)
					{
						EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
						PdkFile_Close(iFd);
						return iRet;
					}
					PdkFile_Close(iFd);
					return EMV_XML_FILE_NODE_END;
					//continue;
				}
				else
				{
					iIndex = FindFunctionIndex(pszNodeStart, vParseListFuncTab, sizeof(vParseListFuncTab)/sizeof(vParseListFuncTab[0]));
					if(iIndex == EMV_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD)
					{
						PdkFile_Close(iFd);
						return EMV_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD;
					}
					if(strstr(aucXmlLineBuf, pszNodeStart) == NULL)
					{
						iRet = vParseListFuncTab[iIndex].vFunc(aucXmlLineBuf, vParseListFuncTab[iIndex].pOutData);
						if(iRet < 0)
						{
							EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
							return iRet;
						}
					}
				}
			}

			//have finish one parsebuf, reset data  //dui de wei zhi
			if(iHaveGetLen == (pszLastEnterPos - aucReadBuf + 1))
			{
				iRet = PdkFile_Seek (iFd, iHaveGetLen - 1024, SEEK_CUR);
				iHaveGetLen = 0;
				
//				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
//				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iFileSize = %d\n",iFileSize);
//				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," (iReadLen + iListPos) = %d\n",(iReadLen + iListPos));s
				break;
			}
		}
		//parse one aid
	}
	PdkFile_Close(iFd);
	return iRet;
}
	
static int ParseAidList(const uchar *pszFileName)
{
	int iRet = 0;
	int iAidIndex = 0;
	int iNodeEnd = -1;
	int iIndex = 0;
	
	iRet = ParseList(pszFileName, "<AIDLIST>", "</AIDLIST>", "<AID>", "</AID>");
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("AID", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	stNodeRecord[iIndex].count = giAidIndex;
	
	iRet = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)&stNodeRecord, sizeof(stNodeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}

static int ParseCapkList(const uchar *pszFileName)
{
	int iRet = 0;
	int iIndex = 0;
	
	iRet = ParseList(pszFileName, "<CAPKLIST>", "</CAPKLIST>", "<CAPK>", "</CAPK>");
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("CAPK", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	stNodeRecord[iIndex].count = giCapkIndex;
	
	iRet = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)&stNodeRecord, sizeof(stNodeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}

static int ParseIcsList(const uchar *pszFileName)
{
	int iRet = 0;
	int iIndex = 0;
	
	iRet = ParseList(pszFileName, "<ICSCONFIGURATION>", "</ICSCONFIGURATION>", "<ICS>", "</ICS>");
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("ICS", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	stNodeRecord[iIndex].count = giIcsIndex;

	iRet = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)&stNodeRecord, sizeof(stNodeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}

static int ParseRevokeCertificateList(const uchar *pszFileName)
{
	int iRet = 0;
	int iIndex = 0;
	
	iRet = ParseList(pszFileName, "<REVOCATIONLIST>", "</REVOCATIONLIST>", "<REVOKEDCERTIFICATE>", "</REVOKEDCERTIFICATE>");
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("REVOKEDCERTIFICATE", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	stNodeRecord[iIndex].count = giRevokeIndex;
		
	iRet = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)&stNodeRecord, sizeof(stNodeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}


//this is option node, so if iRet < 0 ,do not return err
static int ParseCardSchemConfList(const uchar *pszFileName)
{
	int iRet = 0;
	int iIndex = 0;

	iRet = ParseList(pszFileName, "<CARDSCHEMECONFIGRATION>", "</CARDSCHEMECONFIGRATION>", "<CARDSCHEME>", "</CARDSCHEME>");
	if(iRet < 0)
	{
	    //this is option node, so if iRet < 0 ,do not return err
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		//return iRet;
	}

	iIndex = GetRecordIndex("CARDSCHEME", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		//return iIndex;
	}
	stNodeRecord[iIndex].count = giCardSchemConfIndex;
		
	iRet = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)&stNodeRecord, sizeof(stNodeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		//return iRet;
	}
	//return iRet;
	return 0;
}


static int ParseTerminalInfoList(const uchar *pszFileName)
{
	int iRet = 0;
	int iIndex = 0;
	
	iRet = ParseList(pszFileName, NULL, NULL, "<TERMINALCONFIGURATION>", "</TERMINALCONFIGURATION>");
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}

	iIndex = GetRecordIndex("TERMINALCONFIGURATION", sizeof(stNodeRecord)/sizeof(stNodeRecord[0]));
	if(iIndex < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iIndex = %d\n",iIndex);
		return iIndex;
	}
	stNodeRecord[iIndex].count = giTermInfoIndex;
		
	iRet = SaveParsedDataToFile(EMV_XML_FILE_HAVE_PARSED, (uchar*)&stNodeRecord, sizeof(stNodeRecord), 0);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		return iRet;
	}
	return iRet;
}

static void InitGloableVar(void)
{
	giAidIndex = 0;
    giCapkIndex = 0;
    giIcsIndex = 0;
    giRevokeIndex = 0;
    giTermInfoIndex = 0;
	giCardSchemConfIndex = 0;
	giCurrentOffset = sizeof(stNodeRecord);
}

//init the file whic used to stored the have parsed data,save the node info in the start position of file
static int InitEmvParsedFile(const uchar *pszFileName)
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
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iFd = %d\n",iFd);
		return iFd;
	}
	
	iRet = PdkFile_Write (iFd, (uchar*)&stNodeRecord, sizeof(stNodeRecord));
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__," iRet = %d\n",iRet);
		PdkFile_Close(iFd);
		return iRet;
	}
	PdkFile_Close(iFd);
	return iRet;
}

int ParseEmvFile(const uchar *pszFileName)
{
	int iRet = 0;
	
	InitGloableVar();
	iRet = InitEmvParsedFile(EMV_XML_FILE_HAVE_PARSED);
	if(iRet < 0)
	{
		return iRet;
	}

	iRet = ParseAidList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}


	iRet = ParseCapkList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}


	iRet = ParseIcsList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}


	iRet = ParseRevokeCertificateList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}


	iRet = ParseTerminalInfoList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}

	iRet = ParseCardSchemConfList(pszFileName);
	if(iRet < 0)
	{
		return iRet;
	}



#ifdef EMV_PARSE_TEST
	TestGetEmvAid();
	TestGetEmvCapk();
	TestGetEmvIcs();
	TestGetEmvRevoke();
	TestGetEmvTermInfo();
	TestGetCardShcemConf();
#endif	

	return iRet;
}


