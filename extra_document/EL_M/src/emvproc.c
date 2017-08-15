/*------------------------------------------------------------
* FileName: EmvProc.c
* Author:	ZhangYuan
* Date:		2016-08-08
------------------------------------------------------------*/
#include "..\inc\global.h"


#define LOG_TAG	__FILE__
#ifdef EMV_DEBUG_		

#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr,...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)
#endif //EMV_DEBUG_


#define EMV_PARAM_DEBUG_ 1
#ifdef EMV_PARAM_DEBUG_
#include "..\inc\EmvTest.h"
#endif

//#define DEBUG_			1
#define DEBUG_SERIAL	1
#define PORT_USB		11
#define PORT_BLUETOOTH	7


//#define SECURITY_DEBUG_	// Added by ZhangYuan 2016-11-24

/* removed by Kevin Liu 20160901
DATA_APPPARAM	g_tAppParam;
DATA_TRANSPARAM	g_tTransParam;
*/
extern Clss_TransParam	g_stClssTransParam;

#ifdef EMV_PARAM_TEST_
extern EMVParamInfo		g_stEmvParamInfo;

extern void LoadEmvTLVFromFS(void);
#endif

#ifdef DEBUG_
void AppLog(char *formatString, ...);
void AppHexLog(unsigned char *pucMsg, unsigned char *pucData, int iDataLen);
#endif

static uchar *RetToString(int ReturnCode)
{
	switch(ReturnCode)
	{
	case 0:
		return "Success";
	case EL_TRANS_RET_EMV_USER_CANCEL:
		return "User cancel";
	case EL_TRANS_RET_EMV_DATA_ERR:
		return "ICC card data error";
	case EL_TRANS_RET_EMV_NO_APP:
		return "No IC card APP supported by terminal";
	case EL_TRANS_RET_EMV_FAIL:
		return "Transaction failed";
	case EL_TRANS_RET_EMV_DENIAL:
		return "Transaction denial";
	case EL_TRANS_RET_EMV_DECLINED:
		return "Transaction declined";
	default:
		return "Transaction failed";
	}
}

// ==================== CALLBACK FUNCTIONS ==============================================

int cEMVCandAppSel (int TryCnt, EMV_CANDLIST  List[], int AppNum)
{
	int				i=0, iRet=0, iSelect=0;
	unsigned char 	aucTitle[DISP_MAX_LEN+1]="Select App:";

#ifdef EUI_
	ST_DATA stAppList[MAX_APP_ITEMS+1];
	memset(&stAppList, 0, sizeof(stAppList));

	sprintf((char *)stAppList[0].ucWidgetName, "%s", "title");
	sprintf((char *)stAppList[0].ucWidgetValue, "%s", "PLS SELECT:");
#else
	MenuItem stAppMenu[MAX_APP_ITEMS];	// for D180, the surpported max app number is 5
	memset(&stAppMenu, 0, sizeof(stAppMenu));
#endif

	for(i=0; i<MIN(AppNum, MAX_APP_ITEMS); i++)
	{
#ifdef EUI_
		sprintf((char *)stAppList[i+1].ucWidgetName, "%s%d", "menuitem", i+1);
		sprintf((char *)stAppList[i+1].ucWidgetValue, "%.16s", List[i].aucAppLabel);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucWidgetName[%d]=%s, ucWidgetValue[%d]=%s\n", i+1, stAppList[i+1].ucWidgetName, i+1, stAppList[i+1].ucWidgetValue);
#else
		sprintf((char *)stAppMenu[i].szMenuName, "%.16s", List[i].aucAppLabel);
#endif
	}

#ifdef EUI_
	iRet = EShowMenuList(EL_UI_PAGE_TRANS_APP_SELECT, stAppList, &iSelect, 60*10);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EShowMenuList=%d, iSelect=%d\n", iRet, iSelect);
	if(iRet == RET_OK)
	{
		//		iRet = iSelect;
		return iSelect;
	}
	else if(iRet == EL_UI_RET_USER_CANCEL)
	{
		return EMV_USER_CANCEL;
	}
	else if(iRet == EL_UI_RET_TIME_OUT)
	{
		return EMV_TIME_OUT;
	}
	else
	{
		// TODO, which value should be returned to EMV kernel
		return EMV_TIME_OUT;
	}

#else	
	iRet = SelectMenu(aucTitle, MIN(MAX_APP_ITEMS, AppNum), stAppMenu, 60);
#endif

	return iRet;
}

int cEMVInputAmount(unsigned long *AuthAmt, unsigned long *CashBackAmt)
{
	return 0;
}

int cEMVGetHolderPwd(int TryFlag, int RemainCnt, unsigned char *pin)
{
	int				iRet=0, iLen=0;
#ifdef SECURITY_DEBUG_
	unsigned char 	aucShiftedPan[19+1];
#endif
#ifndef EUI_
	char			szPromptMsg[DISP_MAX_LEN+1]="Enter PIN:";
#endif

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TryFlag=%d, RemainCnt=%d, PIN[0]=%02x\n", TryFlag, RemainCnt, pin != NULL ? pin[0] : 0xff);
	if(pin == NULL)		// online PIN process
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Online PIN processing...\n");

#ifdef SECURITY_DEBUG_
		memset(aucShiftedPan, 0, sizeof(aucShiftedPan));
		iRet = PubExtractPAN(g_tTransParam.ucPAN, aucShiftedPan);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PubExtractPAN=%d, PAN: %s\n", iRet, g_tTransParam.ucPAN);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "aucShiftedPan: ", aucShiftedPan, strlen(g_tTransParam.ucPAN));
		if( iRet!=0 )
		{
			// TODO  which error number should be returned
			return EMV_NO_PINPAD;
		}

#ifndef EUI_		
		ScrCls();
		PubDispString(szPromptMsg,0|DISP_LINE_LEFT|DISP_LINE_REVER);
		ScrGotoxy(100, 4);
#endif
		iRet =  PedGetPinBlock(1, (uchar *)"0,4,5,6,7,8,9,10,11,12", aucShiftedPan, g_tTransParam.ucPinBlock, 0x00, 60000); // ISO9564 format 0
#else // #ifndef SECURITY_DEBUG_
		iRet = PedEncryptPin(g_tTransParam.ucPAN, g_tTransParam.ucPinBlock, g_tTransParam.ucKsn);
#endif
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PedGetPinBlock=%d, PAN: %s, KeyType=%02x, KeyIndex=%02x\n", iRet, g_tTransParam.ucPAN, g_tAppParam.ucDataEncryptionType[0], g_tAppParam.ucDataEncryptionKeyIdx[0]);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PINBLOCK: ", g_tTransParam.ucPinBlock, 8);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "KSN: ", g_tTransParam.ucKsn, 10);
		if (PED_RET_OK == iRet)
		{
			g_tTransParam.ucOnlinePinInput[0] = MODE_ONLINE_PIN_INPUT;
			return EMV_OK;
		}
		else
		{
			if (PED_RET_ERR_NO_PIN_INPUT == iRet)
			{	// PIN Bypass
				return EMV_NO_PASSWORD;
			}
			else if (PED_RET_ERR_INPUT_CANCEL == iRet)
			{	// User cancel
				return EMV_USER_CANCEL;
			}
			else if (PED_RET_ERR_INPUT_TIMEOUT == iRet)
			{	// PIN entry time out
				return EMV_TIME_OUT;
			}
			else
			{
				return EMV_NO_PINPAD;
			}
		}	
	}
	else	// offline PIN process
	{
#ifdef EUI_
		unsigned char aucRespond[256]={0};
#endif

		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Offline PIN processing...\n");
		if (1 == RemainCnt)
		{
#ifdef EUI_
			EShowInputBox(EL_UI_PAGE_TRANS_LAST_ENTER_PIN, NULL , aucRespond, sizeof(aucRespond), 15);
#else
			char szWarnMsg[DISP_MAX_LEN+1]="LAST ENTER PIN:";
			ScrCls();	// For D180, there are only 6 lines (in small font, and 3 lines in big font)
			PubDispString(szWarnMsg, 2|DISP_LINE_LEFT);
#endif
		}
#ifdef EUI_
		EShowInputBox(EL_UI_PAGE_TRANS_ENTER_PIN, NULL , aucRespond, sizeof(aucRespond), 0);
#else		
		//ScrCls();
		PubDispString(szPromptMsg,0|DISP_LINE_LEFT|DISP_LINE_REVER);
#endif
		if (0 != TryFlag)
		{
#ifdef EUI_
			EShowMsgBox(EL_UI_PAGE_TRANS_PIN_ERR_TRY_AGAIN, NULL, 10); 
#else
			char szWarnMsg[DISP_MAX_LEN+1]="PIN ERR,RETRY";
			PubDispString(szWarnMsg, 2|DISP_LINE_LEFT);
#endif
		}

		if (pin[0]==EMV_PED_TIMEOUT)
		{
#ifdef EUI_
			EShowMsgBox(EL_UI_PAGE_TRANS_TIMEOUT, NULL, 15);
#else			
			char szWarnMsg[DISP_MAX_LEN+1]="TIMEOUT";
			// EMV core has processed PIN entry and it's timeout
			ScrCls();
			PubDispString(szWarnMsg, 2|DISP_LINE_LEFT);
#endif
			return EMV_TIME_OUT;
		}
		else if (pin[0]==EMV_PED_WAIT)
		{
#ifdef EUI_	
			EShowInputBox(EL_UI_PAGE_TRANS_ENTER_PIN, NULL , aucRespond, sizeof(aucRespond), 0);
#else
			// PED is called too frequently
			DelayMs(1000);
			ScrCls();
			ScrGotoxy(100, 4);
#endif
			return EMV_OK;
		}
		else if (pin[0]==EMV_PED_FAIL)
		{
#ifdef EUI_ 
			EShowMsgBox(EL_UI_PAGE_TRANS_PED_ERR, NULL, 15);
#else
			char szWarnMsg[DISP_MAX_LEN+1]="PED ERROR";
			// EMV core has processed PIN entry and PED failed.
			ScrClrLine(2, 7);
			PubDispString(szWarnMsg, 2|DISP_LINE_LEFT);
			PubBeepErr();
#endif
			return EMV_NO_PINPAD;
		}
		else
		{
			// EMV PIN not processed yet. So just display.
#ifdef EUI_	
			EShowInputBox(EL_UI_PAGE_TRANS_ENTER_PIN, NULL , aucRespond, sizeof(aucRespond), 0);
#else
			ScrGotoxy(100, 4);
#endif
			return EMV_OK;
		}
	}

	return 0;
}

void cEMVVerifyPINOK(void)
{
#ifdef EUI_
	EShowMsgBox(EL_UI_PAGE_TRANS_PIN_VERIFY_OK, NULL, 10);
#else
	char pDispStr[DISP_MAX_LEN+1]="PIN VERIFY OK!";
	ScrCls();
	PubDispString(pDispStr, 2|DISP_LINE_LEFT);
	DelayMs(50);
#endif

	return;
}

int  cCertVerify(void)
{
	// Card holder certificate verification. This is only be used for PBOC3.0
	return 0;
}

int  cEMVSetParam(void)
{
	//	Note: the information below come from D180 EMV API document	
	//	1.	该接口用于在最终选择执行之后，取处理选项命令发送前，根据最终选择的AID名称或具体的应用需要，设置与该AID或与当前交易对应的参数。
	//	2.	应用程序在该接口中调用 EMVSetTLVData 接口设置所需的参数。
	//	3.	当该函数返回值不为EMV_OK时，内核将立即退出交易处理过程。

	int iRet=0, iAidLen=0;
	unsigned char aucAid[17]={0};

#ifndef _WIN32
	iRet=EMVGetTLVData(0x9F06, aucAid, &iAidLen);
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVGetTLVData=%d, TAG: 9F06\n");
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "AID: ", aucAid, iAidLen);
	if(iRet != EMV_OK)
	{
		return 0;
	}

	EMVLoadCardSchemeConfig(aucAid);

	return 0;
}


unsigned char cEMVIccIsoCommand(unsigned char slot, APDU_SEND *ApduSend, APDU_RESP *ApduRecv)
{
	unsigned char status = IccIsoCommand(slot, ApduSend, ApduRecv);
#ifdef EMV_DEBUG_OUTPUT_LOG
	SendICCLog(ApduSend, ApduRecv);
#endif
	if (0x00 == status)
	{
		return 0x00;
	}
	else
	{
		return 0x01;
	}
}

// 由于新架构的EMV库是基于Public库，为了节省空间，EMVAddApp仅添加了AID列表到内核，AID相关参数并未保存，内核仅在最终应用选择后保存一份AID相关参数（即最终应用选择选中的AID对应的参数）用于交易处理。
// 应用层需要在最终应用选择之后（可以放在cEMVSetParam中），调用EMVModFinalAppPara(&tEMV_APP);将最终选中的AID对应的结构体参数重新设置到内核。
int EMVSetFinalApp2Kernel()
{
	int				iRet=0, iAidLen=0;
	unsigned char	aucAid[17+1];

#ifndef _WIN32
	iRet=EMVGetTLVData(0x9F06, aucAid, &iAidLen);
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVGetTLVData=%d, Tag: 9F06\n", iRet);
	if(iRet == EMV_OK)
	{
		EMV_APPLIST stEmvApp;

		memset(&stEmvApp, 0, sizeof(EMV_APPLIST));

#ifdef EMV_PARAM_TEST_
		iRet = EMVLoadSpecificApp(aucAid, iAidLen, &stEmvApp);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVLoadSpecificApp=%d\n", iRet);
		if(iRet == 0)
		{
#ifndef _WIN32
			iRet = EMVModFinalAppPara(&stEmvApp);
#endif
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVModFinalAppPara=%d\n", iRet);
		}
#else
		// TODO Load specific APP from table file system;
#endif
	}

	return iRet;
}

int EMVSetCAPK2Kernel()
{
	int				iRet=0, iAidLen=0, iLen=0;
	unsigned char	aucAid[17+1], ucCapkKeyIdx=0x00;
	EMV_CAPK		stEmvCapk;
	EMV_REVOCLIST	stEmvRevocList;	// Added by ZhangYuan 2016-11-16

	// Application ID
	memset(aucAid, 0, sizeof(aucAid));
#ifndef _WIN32
	iRet = EMVGetTLVData(0x4F, aucAid, &iAidLen);	// AID
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVGetTLVData=%d, Tag: 4F, Len=%d, Value:%02x %02x %02x %02x %02x\n", iRet, iAidLen, aucAid[0], aucAid[1], aucAid[2], aucAid[3], aucAid[4]);
	if(iRet != EMV_OK)
	{
		return EL_TRANS_RET_EMV_DATA_ERR;
	}

#ifndef _WIN32
	iRet = EMVGetTLVData(0x8F, &ucCapkKeyIdx, &iLen);
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVGetTLVData=%d, Tag: 8F, Len=%d, Value=%02x\n", iRet, iLen, ucCapkKeyIdx);
	if(iRet != EMV_OK)
	{
		return EL_TRANS_RET_EMV_DATA_ERR;
	}	

	// Load specific CAPK from table file system and set it into kernel
#ifdef EMV_PARAM_TEST_
	memset(&stEmvCapk, 0, sizeof(stEmvCapk));
	iRet = EMVLoadSpecificCapk(aucAid, ucCapkKeyIdx, &stEmvCapk);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "CAPK.RID: ", stEmvCapk.RID, 5);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "CAPK.KeyID: %02x\n", stEmvCapk.KeyID);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVLoadSpecificCapk=%d\n", iRet);
	if(iRet == 0)
	{
#ifndef _WIN32
		iRet = EMVAddCAPK(&stEmvCapk);
#endif
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVAddCAPK=%d\n", iRet);
	}
#else
	// TODO
#endif

	// Added by ZhangYuan 2016-11-16, add revoked certificate
#ifndef _WIN32
	EMVDelAllRevocList();
#endif

	memset(&stEmvRevocList, 0, sizeof(EMV_REVOCLIST));
#ifdef EMV_PARAM_TEST_
	iRet = EMVLoadSpecificRevoke(aucAid, ucCapkKeyIdx, &stEmvRevocList);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVLoadSpecificRevoke=%d\n", iRet);
	if(iRet == EMV_OK)
	{
#ifndef _WIN32
		EMVAddRevocList(&stEmvRevocList);
#endif
	}
#endif

	return iRet;
}

// Added by ZhangYuan 2016-10-17 Load EMV TLV data from file, and set them into the EMV kernel
void LoadEmvTLVFromFS(void)
{
	int iRet=0, i=0, iTagListLen=0, iTagLen=0, iDataLen=0, iLengthLen=0, iOffset=0, iLengthSubsequentByte=0, iCount=0;
	unsigned short usTag=0;
	unsigned char aucBuf[4096+1], aucData[256+1];

	memset(aucBuf, 0, sizeof(aucBuf));
	iRet = GetAllTLVDataFromMem(aucBuf, &iTagListLen);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetAllTLVDataFromFS ret = %d\n", iRet);
	if(iRet != EMV_OK)
	{
		return;
	}

	while(iOffset < iTagListLen)
	{		
		//calculate tag length, refer to EMV tag Field structure
		if ( 0x1F == (aucBuf[iOffset] & 0x1F) ) {
			if ( 0x80 == (aucBuf[iOffset + 1] & 0x80) ) {
				iTagLen = 3;
			} else {
				iTagLen = 2;
				usTag = aucBuf[iOffset] * 256 + aucBuf[iOffset + 1];
			}
		} else {
			iTagLen = 1;
			usTag = aucBuf[iOffset];
		}

		//check length
		if ( (iOffset + iTagLen) > iTagListLen ) {
			break;
		}

		//calculate data length, refer to EMV tag Field structure
		iDataLen = 0;
		iLengthSubsequentByte = 0;
		if ( 0x80 == (aucBuf[iOffset + iTagLen] & 0x80) ) {
			iLengthSubsequentByte = aucBuf[iOffset + iTagLen] & 0x7F;
			for ( i = iLengthSubsequentByte - 1; i > 0; i++ ) {
				iDataLen += aucBuf[iOffset + iTagLen + i] + 256 * iCount;
				iCount++;
			}
		} else {
			iDataLen = aucBuf[iOffset + iTagLen];
		}

		//check length
		iLengthLen = 1 + iLengthSubsequentByte;
		if ( (iOffset + iTagLen + iLengthLen + iDataLen) > iTagListLen) {
			break;
		}

		//get data for EMV and Paywave
		memset(aucData, 0, sizeof(aucData));
		memcpy(aucData, aucBuf + iOffset + iTagLen + iLengthLen, iDataLen);

#ifndef _WIN32
		iRet = EMVSetTLVData(usTag, aucData, iDataLen);
#endif
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVSetTLVData=%d, iDataLen=%d\n", iRet, iDataLen);
		iOffset = iOffset + iTagLen + iLengthLen + iDataLen;
	}
}
// added end LoadEmvTLVFromFile

// ==================== ================================================================


// 只处理基本数据元素Tag,不包括结构/模板类的Tag
// Build basic TLV data, exclude structure/template.
void BuildTLVString(ushort uiEmvTag, uchar *psData, int iLength, uchar **ppsOutData)
{
	uchar	*psTemp;

	if( iLength<0 )
	{
		return;
	}

	// 设置TAG
	// write tag
	psTemp = *ppsOutData;
	if( uiEmvTag & 0xFF00 )
	{
		*psTemp++ = (uchar)(uiEmvTag >> 8);
	}
	*psTemp++ = (uchar)uiEmvTag;

	// 设置Length
	// write length
	if( iLength<=127 )	// 目前数据长度均小余127字节,但仍按标准进行处理
	{
		*psTemp++ = (uchar)iLength;
	}
	else
	{	// EMV规定最多255字节的数据
		*psTemp++ = 0x81;
		*psTemp++ = (uchar)iLength;
	}

	// 设置Value
	// write value
	if( iLength>0 )
	{
		memcpy(psTemp, psData, iLength);
		psTemp += iLength;
	}

	*ppsOutData = psTemp;
}

int ValidCardNo(unsigned char *pucCardNo)
{
	uchar	bFlag, ucTemp, ucResult;
	uchar	*pszTemp;

	if(pucCardNo == NULL || strlen(pucCardNo) == 0)
	{
		return -1;
	}

	// (2121算法)
	bFlag    = FALSE;
	pszTemp  = &pucCardNo[strlen((char *)pucCardNo)-1];
	ucResult = 0;
	while( pszTemp>=pucCardNo )
	{
		ucTemp = (*pszTemp--) & 0x0F;
		if( bFlag )    ucTemp *= 2;
		if( ucTemp>9 ) ucTemp -= 9;
		ucResult = (ucTemp + ucResult) % 10;
		bFlag = !bFlag;
	}

	if( ucResult!=0 )
	{
#ifdef EUI_
		EShowMsgBox(EL_UI_PAGE_TRANS_INVALID_CARD, NULL, 0);
#else
		ScrCls();
		PubDispString("INVALID CARD", 4|DISP_LINE_LEFT);
		PubBeepErr();
		DelayMs(150);
#endif	
		return -1;
	}

	return 0;
}

//modified by Kevin Liu 20161107
int ValidCard()
{
	int iRet=0, iTag57Len=0;
	unsigned char	aucTag57[80+1]={0}, ucPAN[20+1]={0}, ucExpDate[4+1]={0};
	char	*p=NULL, *pszTemp=NULL;

	memset(aucTag57, 0, sizeof(aucTag57));
	memset(ucPAN, 0, sizeof(ucPAN));

#ifndef _WIN32
	iRet = EMVGetTLVData(0x57, aucTag57, &iTag57Len);
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVGetTLVData=%d, Tag: 57, Len: %d\n", iRet, iTag57Len);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Tag 57, Value: ", aucTag57, iTag57Len);
	if(iRet == EMV_OK)
	{
		unsigned char aucTrack2[37+1];
		unsigned char aucTrack3[107+1];
		int iLoop=0;

		memset(aucTrack2, 0, sizeof(aucTrack2));
		memset(aucTrack3, 0, sizeof(aucTrack3));
		PubBcd2Asc(aucTag57, iTag57Len, aucTrack2);

		//modified by Kevin Liu 20161107
		/*			
		for (iLoop=0; ; iLoop++)
		{
		if ( ('D' == aucTrack2[iLoop]) || (iLoop>=19) )
		break;
		aucPan[iLoop] = aucTrack2[iLoop];
		}
		*/
		PubTrimTailChars(aucTrack2, 'F');	// erase padded 'F' chars
		for ( iLoop=0; aucTrack2[iLoop] != '\0'; iLoop++ )		// convert 'D' to '='
		{
			if ( aucTrack2[iLoop] == 'D' )
			{
				aucTrack2[iLoop] = '=';
				break;
			}
		}

		GetPanFromTrack(aucTrack2, aucTrack3, ucPAN, ucExpDate);

	}

	iRet = ValidCardNo(ucPAN);

	//added be kevinliu 20161024
	memcpy(g_tTransParam.ucPAN, ucPAN, strlen(ucPAN));
	memcpy(g_tTransParam.ucExpDate, ucExpDate, strlen(ucExpDate));
	SetConfigurationData(CMD_TXN_EXPIRE_DATE, ucExpDate, strlen(ucExpDate));	// Added by ZhangYuan 2016-12-19

	return iRet;
}

// ==================== Transaction processing ==============================================
static int EMVInitTransaction()
{
	int iRet=0;

	EMVSetPARAM();
	iRet=EMVLoadAppFromFile();

	return iRet;
}

// start transaction for EMV chip card
// from App selection to 1st GAC
int EMVStartTransaction(unsigned char *pucACType)
{
	int				iRet=0, iLen=0;
	//	unsigned char	ucACType=AC_AAC;
	//	unsigned char 	aucAmount[12+1];
	unsigned char ucTemp[32]={0};

	pucACType[0] = AC_AAC;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Begin ----------------------------------------------------------------\n");
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TransAmount=%d, Cashback=%d, TransNo=%d\n", g_stClssTransParam.ulAmntAuth, g_stClssTransParam.ulAmntOther, g_stClssTransParam.ulTransNo);
	//added by Kevin Liu 20160902
	memset(ucTemp, 0, sizeof(ucTemp));
	ucTemp[0] = TRANS_TYPE_EMV_CONTACT;
	SetConfigurationData(CMD_TXN_CUR_TXN_TYPE, ucTemp, 1);

	EMVInitTransaction();	// deleted by ZhangYuan 2016-11-24, init EMV in StartTransaction,  added by Kevin 20161212.

#ifndef _WIN32
	iRet = EMVAppSelect(ICC_USER, g_stClssTransParam.ulTransNo);
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVAppSelect=%d\n", iRet);
	if( iRet==EMV_DATA_ERR || iRet==EMV_NO_APP || iRet==EMV_RSP_ERR || iRet==ICC_RSP_6985 || iRet == ICC_RESET_ERR || iRet==ICC_CMD_ERR) // Add ICC_RESET_ERR   ICC_CMD_ERR
	{
		if(g_tAppParam.ucFallbackAllowFlag[0] == APPPARAM_FALLBACK_ALLOW)
		{
			g_ucIsFallBack = TRUE;	//added by Kevin Liu 20160902
			return EL_EMV_RET_FALLBACK;
		}

		if(iRet == EMV_DATA_ERR)
		{
			ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_DATA_ERR, iRet, __LINE__);
			return EL_TRANS_RET_EMV_DATA_ERR;
		}
		else 
		{
			ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_NO_APP, iRet, __LINE__);
			return EL_TRANS_RET_EMV_NO_APP;
		}
	}
	else if( iRet==EMV_TIME_OUT || iRet==EMV_USER_CANCEL )
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_USER_CANCEL, iRet, __LINE__);
		return EL_TRANS_RET_EMV_USER_CANCEL;
	}
	else if( iRet!=EMV_OK )
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_FAIL, iRet, __LINE__);
		return EL_TRANS_RET_EMV_FAIL;
	}

#ifdef EUI_
	EShowMsgBox(EL_UI_PAGE_TRANS_PROCESSING, NULL, 0);	// Added by ZhangYuan 2016-11-24
#else
	ScrCls();
	PubDispString("PROCESSING,WAIT..", 0|DISP_LINE_CENTER);
#endif


#ifndef _WIN32
	iRet = EMVReadAppData();
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVReadAppData=%d\n", iRet);
	if( iRet==EMV_TIME_OUT || iRet==EMV_USER_CANCEL )
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_USER_CANCEL, iRet, __LINE__);
		return EL_TRANS_RET_EMV_USER_CANCEL;
	}
	else if( iRet!=EMV_OK )
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_FAIL, iRet, __LINE__);
		return EL_TRANS_RET_EMV_FAIL;
	}

	// valid card number if needed
	iRet = ValidCard();
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ValidCard=%d\n", iRet);
	if(iRet != 0)
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_DATA_ERR, iRet, __LINE__);
		return EL_TRANS_RET_EMV_DATA_ERR;
	}

	// Added by ZhangYuan 2016-10-17 Load EMV TLV data from file, and set them into the EMV kernel
	LoadEmvTLVFromFS();
	// Added end

	EMVSetFinalApp2Kernel();

	EMVSetCAPK2Kernel();

#ifndef _WIN32
	iRet = EMVCardAuth();
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVCardAuth=%d\n", iRet);
	if(iRet != EMV_OK)
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_FAIL, iRet, __LINE__);
		return EL_TRANS_RET_EMV_FAIL;
	}

#ifdef DEBUG_
	{
		int		tempret=0, errorcode=0;
		uchar	testassistinfo[512];

		memset(testassistinfo, 0, sizeof(testassistinfo));
#ifndef _WIN32
		tempret = EMVGetDebugInfo(512, testassistinfo, &errorcode);
#endif
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVGetDebug Info = %d\n", tempret);
		if(tempret == EMV_OK)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "L2 error code = %d\n", errorcode);
		}
	}
#endif

#ifndef _WIN32
	EMVSetPCIModeParam(1, (uchar *)"0,4,5,6,7,8,9,10,11,12", 120000);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "before EMVStartTrans TransAmount=%d, Cashback=%d, TransNo=%d\n", g_stClssTransParam.ulAmntAuth, g_stClssTransParam.ulAmntOther, g_stClssTransParam.ulTransNo);
	iRet = EMVStartTrans(g_stClssTransParam.ulAmntAuth, g_stClssTransParam.ulAmntOther, pucACType);
#endif
	if(iRet != EMV_OK)
	{
		if(iRet == EMV_DENIAL || iRet == EMV_NOT_ACCEPT )
		{
			ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_DENIAL, iRet, __LINE__);
			return EL_TRANS_RET_EMV_DENIAL;
		}
		else
		{
			ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_FAIL, iRet, __LINE__);
			return EL_TRANS_RET_EMV_FAIL;
		}
	}

	// ACType processing
	if(pucACType[0] == AC_AAC)
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_DECLINED, iRet, __LINE__);
		iRet = EL_TRANS_RET_EMV_DECLINED;
	}
	else if(pucACType[0] == AC_ARQC)
	{
		//		iRet = EL_EMV_RET_TRANS_REQUEST_GO_ONLINE; //
#ifdef EUI_
		EShowMsgBox(EL_UI_PAGE_TRANS_NEED_ONLINE, NULL, 15);
#else
		ScrCls();
		PubDispString("NEED ONLINE", 0|DISP_LINE_CENTER);
		PubDispString("PLS NOT REMOVE", 2|DISP_LINE_CENTER);
		PubWaitKey(3);
#endif
	}
	//else
	//{
	//	// TC
	//	iRet = EL_EMV_RET_TRANS_APPROVE;
	//}

	SetConfigurationData(CMD_TXN_CARD_PROCESSING_RESULT, pucACType, 1);	 // Added by ZhangYuan 2017-04-17

	ComPoseInfoMsg(RetToString, 0, 0, __LINE__);
	return 0;
}

int EMVCompleteTransaction()
{
	int				iRet=0, iLen=0, iIssuerScriptLen=0, iOnlineResult=0;
	unsigned char	aucIssuerScript[1024+1], ucACType=AC_AAC;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Begin ------------\n");
	memset(aucIssuerScript, 0, sizeof(aucIssuerScript));
	iIssuerScriptLen = g_tTransParam.ucIssuerScriptLen[0] * 256 + g_tTransParam.ucIssuerScriptLen[1];
	memcpy(aucIssuerScript, g_tTransParam.ucIssuerScript, iIssuerScriptLen);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iIssuerScriptLen = %d\n", iIssuerScriptLen);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "aucIssuerScript: ", aucIssuerScript, iIssuerScriptLen);

	switch(g_tTransParam.ucOnlineResult[0])	// Added by ZhangYuan 2016-12-07
	{
	case 0x00:
		iOnlineResult = ONLINE_APPROVE;
		break;

	case 0x01:
		iOnlineResult = ONLINE_DENIAL;
		break;

	case 0x02:
		iOnlineResult = ONLINE_FAILED;
		break;

	default:
		iOnlineResult = ONLINE_DENIAL;
		break;
	}

#ifndef _WIN32
	iRet = EMVCompleteTrans(iOnlineResult, aucIssuerScript, &iIssuerScriptLen, &ucACType);
#endif
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "EMVCompleteTrans=%d, ucACType=%02x, g_tTransParam.ucOnlineResult[0]=%02x, iOnlineResult=%d\n", iRet, ucACType, g_tTransParam.ucOnlineResult[0], iOnlineResult);
	if(iRet != EMV_OK)
	{
		if(iRet == EMV_DENIAL || iRet == EMV_NOT_ACCEPT )
		{
			ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_DENIAL, iRet, __LINE__);
			return EL_TRANS_RET_EMV_DENIAL;
		}
		else
		{
			ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_FAIL, iRet, __LINE__);
			return EL_TRANS_RET_EMV_FAIL;
		}
	}

	if(ucACType == AC_AAC)
	{
		ComPoseInfoMsg(RetToString, EL_TRANS_RET_EMV_DECLINED, iRet, __LINE__);
		return EL_TRANS_RET_EMV_DECLINED;
	}

	ComPoseInfoMsg(RetToString, 0, 0, __LINE__);
	return iRet;
}
