
#include "..\inc\downkey.h"
#include "..\inc\comm.h"

//#define DOWNLOAD_KEY_TEST
//#define DOWN_KEY_DEBUG


#define LOG_TAG	__FILE__
#ifdef DOWN_KEY_DEBUG		
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else
#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)

#endif // DOWN_KEY_DEBUG	


#define MODULUS_LEN1	(2048/8)
#define MODULUS_LEN2	(1024/8)
#define MODULUS_LEN3	(512/8)	


int gliPortNum = COM_USB;

static int ConnectHost(void);
static int DataRecv(uchar *psRecvBuf);
//static int SendRespPack(uchar *sContent,int iLen);

static int GenRSAKeyPair(R_RSA_PUBLIC_KEY *pstPubKey, R_RSA_PRIVATE_KEY *pstPriKey);
static int SendRSAPubKey(R_RSA_PUBLIC_KEY *pstPubKey);
static int RSADecryption(R_RSA_PRIVATE_KEY *pstPriKey, uchar *psDataIn, uchar *psDataOut);

static int GetDukptKey(uchar *psSour, const int iIndex, R_RSA_PRIVATE_KEY *pstPriKey);
static int GetMastKey(uchar *psSour, const int iIndex, R_RSA_PRIVATE_KEY *pstPriKey);
static int GetTWKKey(uchar *psSour, const int iIndex, R_RSA_PRIVATE_KEY *pstPriKey,int iKeyType);
static int GetKeyIndex(int iKeyFlag);

extern void AppHexLogBt(unsigned char *pucMsg, unsigned char *pucData, int iDataLen);

int DownloadKey(void)
{
	int		iRet, iLoop, iKeyFlag, iKeyIndex;
	uchar	sRecvBuf[2048], ucLrc;
	uchar ucKey = 0;

	R_RSA_PRIVATE_KEY	stPriKey = {0};
	R_RSA_PUBLIC_KEY	stPubKey = {0};

	while(1)
	{


		ScrCls();

		iRet = ConnectHost();
		if (iRet <= 0)
		{
			ScrCls();
			ScrPrint(0, 6, CFONT, "Connect err!");
			DelayMs(1000);
			return iRet;
		}
		else
		{
			ScrCls();
			PubShowMsg(2, "Connect OK!");
			DelayMs(500);
			SendRespPack("1000", 4);
		}

		memset(&stPriKey, 0, sizeof(R_RSA_PRIVATE_KEY));
		memset(&stPubKey, 0, sizeof(R_RSA_PUBLIC_KEY));
		iRet = GenRSAKeyPair(&stPubKey, &stPriKey);

		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "DownloadKey stPubKey.bits = %d",stPubKey.bits);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "----------------------------------------------\n");
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "DownloadKey stPubKey.modulus",stPubKey.modulus,MAX_RSA_MODULUS_LEN);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "----------------------------------------------\n");
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "DownloadKey stPubKey.exponent",stPubKey.exponent,MAX_RSA_MODULUS_LEN);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "----------------------------------------------\n");

		if (0 != iRet)
		{
			ScrCls();
			ScrPrint(0, 4, CFONT, "Gen RSA Key err!");
			kbflush();
			getkey();
			return iRet;
		}

		iRet = SendRSAPubKey(&stPubKey);
		if (0 != iRet)
		{
			ScrCls();
			ScrPrint(0, 4, CFONT, "Send PubKey err!");
			kbflush();
			getkey();
			return iRet;
		}

		// Receive Key.....
		PubShowTitle(TRUE, "Downloading...");
		memset(sRecvBuf, 0, sizeof(sRecvBuf));
		iRet = DataRecv(sRecvBuf);
		if (iRet <= 0)
		{
			ScrCls();
			PubShowMsg(6, "Recv err!");
			kbflush();
			getkey();
			return iRet;
		}
		// Check if receive data correctly
		if (STX != sRecvBuf[0])
		{
			ScrCls();
			PubShowMsg(6, "Recv err!");
			kbflush();
			getkey();
			return -1;	
		}
		ucLrc = sRecvBuf[iRet-1];
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "lrc = %02x\n", ucLrc);

		for (iLoop=1; iLoop<(iRet-1); iLoop++)
		{		
			ucLrc ^= sRecvBuf[iLoop];
		}		
		if (ucLrc != 0)
		{
			ScrCls();
			PubShowMsg(6, "Recv err!");
			kbflush();
			getkey();
			return -1;
		}

		if (memcmp(sRecvBuf+4,"21",2) == 0)
		{	
			// check key type
			if (0 == memcmp(sRecvBuf+6, "\x01\x01", 2))
			{
				iKeyFlag = DUKPT_KEY;
			}
			else if (0 == memcmp(sRecvBuf+6, "\x02\x02", 2))
			{
				iKeyFlag = MAST_KEY;
			}
			else if (0 == memcmp(sRecvBuf+6, "\x03\x03", 2))
			{
				iKeyFlag = TPK_KEY;
			}
			else if (0 == memcmp(sRecvBuf+6, "\x04\x04", 2))
			{
				iKeyFlag = TDK_KEY;
			}
			else if (0 == memcmp(sRecvBuf+6, "\x05\x05", 2))
			{
				iKeyFlag = TAK_KEY;
			}

			iKeyIndex = GetKeyIndex(iKeyFlag); // 输入密钥索引号
			if (iKeyIndex == -1)
			{
				return -1;
			}

			if (iKeyFlag == DUKPT_KEY)
			{
				iRet = GetDukptKey(sRecvBuf, iKeyIndex, &stPriKey);
			}
			else if (iKeyFlag == MAST_KEY)
			{
				iRet = GetMastKey(sRecvBuf, iKeyIndex, &stPriKey);
			}
			else if(iKeyFlag == TPK_KEY || iKeyFlag == TDK_KEY || iKeyFlag == TAK_KEY)
			{
				iRet = GetTWKKey(sRecvBuf, iKeyIndex, &stPriKey,iKeyFlag);
			}

			if(iRet == 0)
			{
				SendRespPack("2100", 4);// send response pack to source POS
				ScrCls();
				ScrPrint(0, 0, CFONT, "Get Key OK");
				ScrPrint(0, 2, CFONT, "Continue?");
			}
			else
			{
				ScrCls();
				Lcdprintf("Write Key iRet = %d\n", iRet);
				getkey();		
			}

			if (iRet == 0 && iKeyFlag == DUKPT_KEY)
			{	
#ifdef DOWNLOAD_KEY_TEST
				TestDukptEncryPin();
#endif
			}
			else if(iRet == 0 && iKeyFlag == MAST_KEY)
			{
#ifdef DOWNLOAD_KEY_TEST
				TestMasterKey();
#endif
			}
			else if(iRet == 0 && iKeyFlag == TPK_KEY)
			{
#ifdef DOWNLOAD_KEY_TEST
				TestTPKKey();
#endif
			}
			else if(iRet == 0 && iKeyFlag == TDK_KEY)
			{
#ifdef DOWNLOAD_KEY_TEST
				TestTDKKey();
#endif
			}
			else if(iRet == 0 && iKeyFlag == TAK_KEY)
			{
#ifdef DOWNLOAD_KEY_TEST
				TestTAKKey();
#endif
			}

		}

		kbflush();
		while(1)
		{
			ucKey = getkey();
			if(ucKey == KEYCANCEL)
			{
				return 0;
			}
			else if(ucKey  == KEYENTER)
			{
				break;
			}
		}

	}
	return 0;
}

int ConnectHost(void)
{
	uchar	sBuff[1024] = {0};
	int		iRet;

	// Waite for connection command 
	ScrCls();
	PubShowTitle(FALSE, "Connect Host");
	memset(sBuff, 0, sizeof(sBuff));
	iRet = DataRecv(sBuff);
	if (iRet <= 0)
	{
		return iRet;
	}

	// Check if connection is OK
	if (STX != sBuff[0])
	{	
		return -1;	
	}
	if (memcmp(sBuff+4, "10", 2)==0)
	{
		return iRet;
	}
	else
	{
		return -1;
	}
}

int DataRecv(uchar *psRecvBuf)
{
	int iRet;
	uchar ucKey;

	kbflush();
	while (1)
	{
		ScrClrLine(2, 7);
		PubShowMsg(4, "Receiving...");

		if (0x00 == kbhit())
		{
			ucKey = getkey();
			if (KEYCANCEL == ucKey)
			{
				return -1;
			}
		}

		iRet = PortRecvs((uchar)gliPortNum, psRecvBuf, 300, 1000);
		if (iRet > 0)
		{
			PortReset((uchar)gliPortNum);
			break;
		}

	}// while(1

	return iRet;
}

int GetKeyIndex(int iKeyFlag)
{
	uchar	ucRet, sIndex[5];
	int		iIndex;
	int iErrTag = 0;
	uchar ucKey = 0;
	int iRet = 0;

	while(1)
	{
		memset(sIndex, 0, sizeof(sIndex));
		ScrCls();
		kbflush();
		ScrPrint(0, 0, CFONT, "Please input ");
		if (iKeyFlag == DUKPT_KEY)
		{
			ScrPrint(0, 2, CFONT, "Dukpt Index(1-10)");
			ucRet = PubGetString(MPOS_INPUT_NUM_IN, 1, 2, sIndex, -1);
		}
		else if (iKeyFlag == MAST_KEY)
		{
			ScrPrint(0, 2, CFONT, "Mas Index(1-100)");
			ucRet = PubGetString(MPOS_INPUT_NUM_IN, 1, 3, sIndex, -1);
		}
		else if (iKeyFlag == TPK_KEY)
		{
			ScrPrint(0, 2, CFONT, "TPK Index(1-100)");
			ucRet = PubGetString(MPOS_INPUT_NUM_IN, 1, 3, sIndex, -1);
		}
		else if (iKeyFlag == TDK_KEY)
		{
			ScrPrint(0, 2, CFONT, "TDK Index(1-100)");
			ucRet = PubGetString(MPOS_INPUT_NUM_IN, 1, 3, sIndex, -1);
		}
		else if (iKeyFlag == TAK_KEY)
		{
			ScrPrint(0, 2, CFONT, "TAK Index(1-100)");
			ucRet = PubGetString(MPOS_INPUT_NUM_IN, 1, 3, sIndex, -1);
		}

		if (ucRet == 0xFF)
		{
			return -1;
		}

		iIndex = atoi(sIndex);
		if((iKeyFlag == DUKPT_KEY) && (iIndex < 1 || iIndex > 10))
		{

			iErrTag = 1;
		}
		else if((iKeyFlag == MAST_KEY) && (iIndex < 1 || iIndex > 100))
		{
			iErrTag = 1;
		}
		else if((iKeyFlag == TPK_KEY) && (iIndex < 1 || iIndex > 100))
		{
			iErrTag = 1;
		}
		else if((iKeyFlag == TDK_KEY) && (iIndex < 1 || iIndex > 100))
		{
			iErrTag = 1;
		}
		else if((iKeyFlag == TAK_KEY) && (iIndex < 1 || iIndex > 100))
		{
			iErrTag = 1;
		}

		if(1 == iErrTag)
		{
			ScrCls();
			kbflush();
			PubShowMsg(0, "Index Err,ReType?");
			PubShowMsg(2, "<enter>:YES");
			PubShowMsg(4, "<cancer>:NO");
			ucKey = PubYesNo(30);
			if(ucKey == 0)
			{
				iErrTag = 0;
				continue;
			}
			else
			{
				//return iRet;
				return - 1;
			}
		}
		else
		{
			return iIndex;
		}

	}
	return iIndex;
}

int SendRespPack(uchar *sContent,int iLen)
{
	uchar	buffer[100];
	uchar ucRet = 0;

	PortReset((uchar)gliPortNum);
#if 0
	PortSend((uchar)gliPortNum, STX);
	sprintf(buffer, "%03c", iLen+0x30);
	PortSend((uchar)gliPortNum, buffer[0]);
	PortSend((uchar)gliPortNum, buffer[1]);
	PortSend((uchar)gliPortNum, buffer[2]);
	buffer[0] ^= buffer[1];
	buffer[0] ^= buffer[2];
	for (iLoop=0; iLoop<iLen; iLoop++)
	{
		buffer[0] ^= sContent[iLoop];
		PortSend((uchar)gliPortNum, sContent[iLoop]);

	}	
	PortSend((uchar)gliPortNum, ETX);
	buffer[0] ^= ETX;
	PortSend((uchar)gliPortNum,buffer[0]);
#endif
#if 1
	if(strcmp(sContent,"1000")==0)
	{
		memcpy(buffer, "\x02\x30\x30\x34\x31\x30\x30\x30\x03", 9);//huangwp
	}
	else if(strcmp(sContent,"2100")==0)
	{
		memcpy(buffer, "\x02\x30\x30\x34\x32\x31\x30\x30\x03", 9);
	}

	ucRet = PortSends((uchar)gliPortNum, buffer, 9);
	if (ucRet != 0) 
	{
		ScrCls();
		ScrPrint(0, 4, CFONT, "PortSends err:%d", ucRet);
		ScrPrint(0, 6, CFONT, "Press<enter>");
		kbflush();
		getkey();
		return ucRet;
	}
#endif

	return 0;
}

// Generate RSA Key Pair
int GenRSAKeyPair(R_RSA_PUBLIC_KEY *pstPubKey, R_RSA_PRIVATE_KEY *pstPriKey)
{
	int iRet;

	kbflush();
	ScrCls();	
	//ScrPrint(0, 2, CFONT, "Randomly Gen");
	//ScrPrint(0, 4, CFONT, "RSA Key");
	//ScrPrint(0, 6, CFONT, "Please Waiting..");
	ScrPrint(0, 0, CFONT, "Randomly Gen");
	ScrPrint(0, 2, CFONT, "RSA Key");
	ScrPrint(0, 4, CFONT, "Please Waiting..");


	iRet = RSAKeyPairGen(pstPubKey, pstPriKey, 1024, 0);
	if (0 != iRet)
	{
		kbflush();
		ScrCls();
#ifdef DEBUG_OUTPUT_PRN
		ScrPrint(0, 6, CFONT, "Gen Key Fail %d", iRet); 
#else
		ScrPrint(0, 6, CFONT, "Gen Key Fail!");
#endif

		getkey();
		return iRet;
	}

	ScrCls();
	ScrPrint(0, 4, CFONT, "Verify KeyPair..");
	iRet = RSAKeyPairVerify(*pstPubKey, *pstPriKey);
	if (0 != iRet)
	{
#ifdef DEBUG_OUTPUT_PRN
		ScrPrint(0, 6, CFONT, "Verify Fail %d", iRet); 
#else
		ScrPrint(0, 6, CFONT, "Gen Key Fail!");
#endif
		kbflush();
		getkey();
		return iRet;			
	}

	return 0;
}

//send RSA Public Key
int SendRSAPubKey(R_RSA_PUBLIC_KEY *pstPubKey)
{
	int		iKeyLen, iSendLen=0, iLoop, iRet;
	uchar	sSendBuff[5000];

	memset(sSendBuff, 0, sizeof(sSendBuff));
	iKeyLen = sizeof(R_RSA_PUBLIC_KEY);

	memcpy(sSendBuff, "\x02\x30\x30\x30\x33\x32", 6);  // 长度"\x30\x30\x30"暂时先不管
	iSendLen += 6;
	memcpy(sSendBuff+iSendLen, (uchar *)pstPubKey, iKeyLen);
	iSendLen += iKeyLen;
	sSendBuff[iSendLen] = '\x03';
	iSendLen += 1;

	sSendBuff[iSendLen] = '\x00';
	for (iLoop=1; iLoop<iSendLen; iLoop++) // calculate LRC
	{
		sSendBuff[iSendLen] ^= sSendBuff[iLoop]; 
	}
	iSendLen += 1;

	PortReset((uchar)gliPortNum);

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "SendRSAPubKey pstPubKey: ",(uchar *)pstPubKey,256);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "----------------------------------------------\n");

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "SendRSAPubKey sSendBuff: ",sSendBuff,300);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "----------------------------------------------\n");

	iRet = PortSends((uchar)gliPortNum, sSendBuff, iSendLen);

	return iRet;
}

int GetDukptKey(uchar *psSour, const int iIndex, R_RSA_PRIVATE_KEY *pstPriKey)
{
	int		iRet;

	uchar	sIPEK[100], sKSN[100];
	uchar	sEncryKey[2048], sDecryKey[2048];
	ST_KCV_INFO	stKcv;

	// check if key type is right
	if (memcmp(psSour+6, "\x01\x01", 2) != 0)
	{
		return -1;
	}

	memcpy(sEncryKey, psSour+8, MAX_RSA_MODULUS_LEN);
	iRet = RSADecryption(pstPriKey, sEncryKey, sDecryKey);
	if (0 != iRet)
	{
		return iRet;
	}

	memset(sIPEK, 0, sizeof(sIPEK));
	memset(sKSN, 0, sizeof(sKSN));
	memcpy(sIPEK, sDecryKey+2, 16);
	memcpy(sKSN, sDecryKey+18, 8);

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Recv psSour: ",psSour,100);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "=============================\n");
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sDecryKey: ",sDecryKey,100);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "=============================\n");
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sIPEK = \n",sIPEK,32);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "=============================\n");
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sDecryKey: ",sDecryKey,32);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "=============================\n");


	memset(&stKcv, 0, sizeof(ST_KCV_INFO));
	stKcv.iCheckMode = 0;
	iRet = PedWriteTIK((uchar)iIndex, 0, 16, sIPEK, sKSN, &stKcv);
	if (iRet != 0)
	{
		ScrCls();
		ScrPrint(0, 2, CFONT, "Save Key Failed");		
		ScrPrint(0, 4, CFONT, "PedWriteKey:%d", iRet);
		kbflush();
		getkey();
		return iRet;
	}

	return iRet;
}

int GetMastKey(uchar *psSour, const int iIndex, R_RSA_PRIVATE_KEY *pstPriKey)
{
	int		iRet;

	uchar	sMasterKey[100];
	uchar	sEncryKey[2048], sDecryKey[2048];
	ST_KCV_INFO stKcv;
	ST_KEY_INFO stKey;

	// check if key type is right
	if (memcmp(psSour+6, "\x02\x02", 2) != 0)
	{
		return -1;
	}

	memcpy(sEncryKey, psSour+8, MAX_RSA_MODULUS_LEN);
	iRet = RSADecryption(pstPriKey, sEncryKey, sDecryKey);
	if (0 != iRet)
	{
		return iRet;
	}

	memset(sMasterKey, 0, sizeof(sMasterKey));
	memcpy(sMasterKey, sDecryKey+2, 16);

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sMasterKey: ",sMasterKey,16);

	// 	// get Master Key
	// 	memset(sMasterKey, 0, sizeof(sMasterKey));
	// 	memset(sMastKeyTemp, 0, sizeof(sMastKeyTemp));
	// 	memcpy(sMastKeyTemp, psSour+8, 16);
	// //	DecryptKeyTDES(16, sMastKeyTemp, sMasterKey);!!!!!RSA解密

#ifdef DEBUG_OUTPUT_PRN
	PubDebugOutput("Recv", psSour, 400, DEVICE_PRN, HEX_MODE);
	PubDebugOutput("DecryKey", sDecryKey, 400, DEVICE_PRN, HEX_MODE);
	PubDebugOutput("MasterKey", sMasterKey, 100, DEVICE_PRN, HEX_MODE);
#endif

	memset(&stKcv, 0, sizeof(ST_KCV_INFO));
	memset(&stKey, 0, sizeof(ST_KEY_INFO));

	stKey.ucSrcKeyType = PED_TDK;
	stKey.ucSrcKeyIdx = 0;
	stKey.ucDstKeyType = PED_TMK;
	stKey.ucDstKeyIdx = iIndex;
	stKey.iDstKeyLen = 16;
	memcpy(stKey.aucDstKeyValue, sMasterKey, stKey.iDstKeyLen);

	stKcv.iCheckMode = 0;

	iRet = PedWriteKey(&stKey, &stKcv);
	if (iRet != 0)
	{
		ScrCls();
		ScrPrint(0, 2, CFONT, "Save Key Failed");		
		ScrPrint(0, 4, CFONT, "PedWriteKey:%d", iRet);
		kbflush();
		getkey();
		return iRet;
	}

	return iRet;
}

static int GetTWKKey(uchar *psSour, const int iIndex, R_RSA_PRIVATE_KEY *pstPriKey,int iKeyType)
{
	int		iRet;
	uchar	sKey[100];
	uchar	sEncryKey[2048], sDecryKey[2048];
	ST_KCV_INFO stKcv;
	ST_KEY_INFO stKey;
	uchar ucKey = 0;
	int iKeyIndex = -1;

	memcpy(sEncryKey, psSour+8, MAX_RSA_MODULUS_LEN);
	iRet = RSADecryption(pstPriKey, sEncryKey, sDecryKey);
	if (0 != iRet)
	{
		return iRet;
	}

	memset(sKey, 0, sizeof(sKey));
	memcpy(sKey, sDecryKey+2, 16);

	memset(&stKcv, 0, sizeof(ST_KCV_INFO));
	memset(&stKey, 0, sizeof(ST_KEY_INFO));

	switch(iKeyType)
	{
	case TPK_KEY:
		stKey.ucDstKeyType = PED_TPK;
		break;
	case TDK_KEY:
		stKey.ucDstKeyType = PED_TDK;
		break;
	case TAK_KEY:
		stKey.ucDstKeyType = PED_TAK;
		break;
	default:
		return -1;		
	}

	ScrCls();
	ScrPrint(0, 0, CFONT, "1:Need TMK");	
	ScrPrint(0, 2, CFONT, "2:No Need TMK");
	ucKey = getkey();
	if(ucKey == KEY1)
	{
		stKey.ucSrcKeyType = PED_TMK;

		iKeyIndex = GetKeyIndex(MAST_KEY); // 输入密钥索引号
		if (iKeyIndex == -1)
		{
			return -1;
		}
		stKey.ucSrcKeyIdx = iKeyIndex;
	}
	else if(ucKey = KEY2)
	{
		stKey.ucSrcKeyType = PED_TDK;
		stKey.ucSrcKeyIdx = 0;

	}


	stKey.ucDstKeyIdx = iIndex;
	stKey.iDstKeyLen = 16;
	memcpy(stKey.aucDstKeyValue, sKey, stKey.iDstKeyLen);

	stKcv.iCheckMode = 0;

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "sKey: ", sKey, 16);

	iRet = PedWriteKey(&stKey, &stKcv);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PedWriteKey = %d\n", iRet);
	if (iRet != 0)
	{
		ScrCls();
		ScrPrint(0, 2, CFONT, "Save Key Failed");		
		ScrPrint(0, 4, CFONT, "PedWriteKey:%d", iRet);
		kbflush();
		getkey();
		return iRet;
	}

	return iRet;


}


int RSADecryption(R_RSA_PRIVATE_KEY *pstPriKey, uchar *psDataIn, uchar *psDataOut)
{
	int		iModuleLen, iRet;
	uchar   priModulus[MAX_RSA_MODULUS_LEN];
	uchar   priExponent[MAX_RSA_MODULUS_LEN];

	iModuleLen = pstPriKey->bits/8;
	memcpy(priModulus,&(pstPriKey->modulus[256-iModuleLen]),iModuleLen);  
	memcpy(priExponent,&(pstPriKey->exponent[256-iModuleLen]),iModuleLen);

	iRet = RSARecover(priModulus, iModuleLen, priExponent, iModuleLen, psDataIn, psDataOut);
	if (0 != iRet)
	{
		return iRet;
	}

	return 0;
}

#ifdef DOWNLOAD_KEY_TEST
int TestDukptEncryPin()
{
	int iRet = 0;
	uchar aucTestData[] = "1234567890ABCDEF";
	uchar szInputCardNo[]="1234012345678901";
	uchar szShiftCardNo[19+1];
	uchar szKsnOutPut[12] = {0};
	uchar szPinBlockOut[12] = {0};
	uchar ucKey = 0;


	while(1)
	{
		ScrCls();
		Lcdprintf("input pin");
		iRet=PubExtractPAN(szInputCardNo,szShiftCardNo);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PubExtractPAN=%d, szInputCardNo=%s, szShiftCardNo=%s\n", iRet, szInputCardNo, szShiftCardNo);
		iRet=PedGetPinDukpt(1, "0,4,5,6,7,8,9,10,11,12",szShiftCardNo, szKsnOutPut, szPinBlockOut,0,60000);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PedGetPinDukpt = %d",iRet);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "szKsnOutPut: ", szKsnOutPut,10);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "szPinBlockOut: ", szPinBlockOut,10);
		ucKey =getkey();
		if(ucKey == KEYCANCEL)
		{
			break;
		}
		else
		{
			continue;
		}

	}

	return 0;
}


int TestTPKKey()
{
	int iRet = 0;
	uchar aucTestData[] = "1234567890ABCDEF";
	uchar szInputCardNo[]="1234012345678901";
	uchar szShiftCardNo[19+1];
	uchar szKsnOutPut[12] = {0};
	uchar szPinBlockOut[12] = {0};
	uchar ucKey = 0;

	/*
	ScrCls();
	ScrPrint(0, 0, CFONT, "1:Encrypted");	
	ScrPrint(0, 0, CFONT, "2:UnEncrypted");
	*/
	while(1)
	{
		ScrCls();
		Lcdprintf("input pin");
		iRet=PubExtractPAN(szInputCardNo,szShiftCardNo);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PubExtractPAN=%d, szInputCardNo=%s, szShiftCardNo=%s\n", iRet, szInputCardNo, szShiftCardNo);
		iRet=PedGetPinBlock(1, "0,4,5,6,7,8,9,10,11,12",szShiftCardNo, szPinBlockOut,0,60000);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PedGetPinBlock = %d\n", iRet);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "szPinBlockOut: ", szPinBlockOut, 10);
		ucKey =getkey();
		if(ucKey == KEYCANCEL)
		{
			break;
		}
		else
		{
			continue;
		}

	}
	return 0;
}

int TestTDKKey()
{
	int iRet = 0;
	uchar aucTestData[] = "\x12\x34\x56\x78\x90\xAB\xCD\xEF";
	uchar szInputCardNo[]="1234012345678901";
	uchar szShiftCardNo[19+1];
	uchar szKsnOutPut[12] = {0};
	uchar szPinBlockOut[12] = {0};
	uchar ucKey = 0;
	uchar szDataOut[16+1] = {0};

	while(1)
	{
		ScrCls();
		//iRet=PedGetPinBlock(6, "0,4,5,6,7,8,9,10,11,12",szShiftCardNo, szPinBlockOut,0,60000);
		iRet = PedCalcDES(1, aucTestData, 16, szDataOut, 0x00);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Encrypt, PedCalcDES = %d\n", iRet);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Encrypt, szDataOut: ", szDataOut, 16);

		memset(szDataOut,0,sizeof(szDataOut));
		iRet = PedCalcDES(1, aucTestData, 16, szDataOut, 0x00);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Decrypt, PedCalcDES = %d\n", iRet);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Decrypt, szDataOut: ", szDataOut, 16);


		ucKey = getkey();
		if(ucKey == KEYCANCEL)
		{
			break;
		}
		else
		{
			continue;
		}

	}
	return 0;
}

int TestTAKKey()
{
	int iRet = 0;
	uchar aucTestData[] = "\x12\x34\x56\x78\x90\xAB\xCD\xEF";
	//uchar aucTestData[] = "1234567890ABCDEF";
	uchar szInputCardNo[]="1234012345678901";
	uchar szShiftCardNo[19+1];
	uchar szKsnOutPut[12] = {0};
	uchar szPinBlockOut[12] = {0};
	uchar ucKey = 0;
	uchar szDataOut[16+1] = {0};

	while(1)
	{
		ScrCls();
		//iRet=PedGetPinBlock(6, "0,4,5,6,7,8,9,10,11,12",szShiftCardNo, szPinBlockOut,0,60000);
		iRet = PedGetMac(1, aucTestData, 8, szDataOut, 0x00);			
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PedGetMac = %d\n", iRet);
		EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "szDataOut: ", szDataOut, 16);

		ucKey = getkey();
		if(ucKey == KEYCANCEL)
		{
			break;
		}
		else
		{
			continue;
		}

	}
	return 0;
}





int TestMasterKey()
{
	return 0;
}
#endif
// end of file
