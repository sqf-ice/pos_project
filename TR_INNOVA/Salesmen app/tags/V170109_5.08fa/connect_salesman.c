#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "framework.h"
#include "connect.h"
#include "data_define_salesman.h"

#include "printList_salesman.h"
#include "LHBXML.h"

#include "IMSI_List.h"
#include "WlExt.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static int sg_iSocket = -1;
static unsigned char sg_bConnected = FALSE;

static unsigned char ReadBarCode(unsigned char* pBuffer, int iTimeout);


static int GetSocket(void);
static void SetSocket(int sk);

static int ConnectSub(int iTimeout, unsigned char type);
static int FTPConnectSub(int iTimeout, unsigned char type);
static int TCPSocketProcess(CONN_PARAM *connParam, unsigned short uiTimeOutSec);
static int GPRSSocketProcess(CONN_PARAM *connParam, unsigned short uiTimeOutSec);
static int WirelessDial(WIRELESS_PARAM *pstParam);
static int GPRSConnect(WIRELESS_PARAM wlParam);
// static int ConnectPPP(CONN_PARAM *connParam, unsigned short usLocalPort);

static int TCPTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec);
static int GPRSTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec);
//static int PPPTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec);

static int TcpRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec);
static int GPRSRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec);
//static int PPPRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec);

static int TcpClose(void);
static int GPRSClose(void);
//static int PPPClose(void);

#ifdef _S_SERIES_
static int SxxDhcpStart(unsigned char ucForceStart, unsigned char ucTimeOutSec);
static unsigned char SxxSocketCheck(int sk);
static int SxxTcpConnect(unsigned char *pszIP, unsigned short sPort, unsigned short iTimeoutSec);
static int SxxTcpTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec);
static int SxxTcpRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec);
static int SxxTcpClose(void);
#else
static unsigned char SearchOK(int TimeOut);
static int P80DhcpStart(void);
static int P80TcpTxd(const unsigned char *psTxdData, unsigned short uiDataLen);
static int P80TcpRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec);
#endif

static unsigned char * Nstrstr(unsigned char *psStr, unsigned char *psDes, int iNum);

static void MatchOperatorName(const unsigned char *MCC_MNC, unsigned char *Name);
static int FTPCmd(const unsigned char *psCmd, const unsigned char *psPara, unsigned char *psReadStr);
static int FTPGetPort(unsigned char *pszStr, unsigned short *pusPort);
struct WlInfo gl_SimMesg;  //add by jeff_xiehuan20170105

void DispWNetSignal(void)
{
	unsigned char ucLevel;
	int iRet;

#ifdef _S_SERIES_
	iRet = WlGetSignal(&ucLevel);
#else
	iRet = WNetCheckSignal(&ucLevel);
#endif
	if(iRet == RET_OK)
	{
		if(ucLevel > 5) 
			ucLevel = 0;
		else 
			ucLevel = 5 - ucLevel;
	}
	ScrSetIcon(ICON_SIGNAL, CLOSEICON);
	ScrSetIcon(ICON_SIGNAL, (unsigned char)(INITSIGNAL + ucLevel));
}

static int CheckValidSimMesg(const char *data)
{
	int i;
	int iCount;

	iCount = 0;
	i = 0;
	while(data[i])
	{
		if(data[i] >= '0' && data[i] <= '9')
		{
			iCount++;
			
		}
		i++;
	}
	if(iCount < 5)
	{
		return -1;
	}
	return 0;
}

static void GenSimMsg(const char* text,char *output)  //add by jeff_xiehuan20170105
{
	int i;
	int j;
	int flag;

	i = 0;
	j = 0;
	flag = 0;
	if(gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel] == GPRS)
	{
		while(text[i])
		{
			if(text[i] >= '0' && text[i] <= '9')
			{
				output[j] = text[i];
				j++;
				flag = 1;
			}
			else
			{

				if(flag == 1)
				{
					break;
				}
			}
			i++;
				
		}
		
	}
}

void GetOperatorName()
{
	int iRet;
	int iRetCCID;
	unsigned char szISMI[120];
	char szCCID[151];

	memset(szISMI, 0, 120);
	memset(szCCID,0,sizeof(szCCID));


#ifdef _S_SERIES_
	if(WlPppCheck() != 0)
	{
		WlSelSim(0);
		iRet=WlInit(gl_MaintenaceParam.stConnParam.stParamGPRS.szSimPin);
	}
#else
	iRet = WNetInit();
#endif

#ifdef _S_SERIES_
	WlOpenPort();
	iRet = WlSendCmd("at+cimi\r", szISMI, 120, 3*1000, 0);
           /*add by jeff_xiehuan_20170106 for getting IMSI_KEY and CCID_KEY*/
	iRetCCID = WlSendCmd("AT+CCID \r",szCCID,sizeof(szCCID)-1,1000,50);
	memset(&gl_SimMesg,0,sizeof(gl_SimMesg));
	if(CheckValidSimMesg(szCCID) == -1)
	{
		  Dll_WlGetInfo(CCID_KEY,&gl_SimMesg);
	}
	else
	{
		 GenSimMsg(szCCID,gl_SimMesg.ccid);
	}
	if(CheckValidSimMesg(szISMI) != -1)
	{
		 GenSimMsg(szISMI,gl_SimMesg.imsi);
	}
	//PubDebugTx("gl_SimMesg.IMSI=%s,gl_Simmesg.CCID_KEY=%s,gl_SimMesg.module_name=%s",gl_SimMesg.imsi,gl_SimMesg.ccid,gl_SimMesg.module_name);
	WlClosePort();
#else
	iRet = WNetSendCmd("at+cimi\r", szISMI, 120, 3*1000);
#endif
	if(0 == iRet)
	{
		MatchOperatorName(szISMI, gl_MaintenaceParam.stConnParam.stParamGPRS.szOperatorName);
		WriteParam(WRT_MAINTA);
	}
}


unsigned char CheckIP(const unsigned char *IP)
{
	int iIPNum;
	char *tokenPtr;
	char stIPTemp[50];
	const char *cSepartor = ".";  //IPV6 ? ":" : "."
	unsigned char ucRet;
	unsigned char ucPart = 0;

	if(NULL == IP)
	{
		return FALSE;
	}

	memset(stIPTemp, 0, sizeof(stIPTemp));
	strcpy(stIPTemp, IP);

	ucRet = FALSE;
	tokenPtr=strtok(stIPTemp, cSepartor);
	while(NULL != tokenPtr)
	{
		iIPNum = atoi(tokenPtr);
		// if(IPV4)
		if(iIPNum < 0 || iIPNum > 255)
		{
			return FALSE;
		}
		else
		{
			++ucPart;
			ucRet = TRUE;
		}
		//else if IPV6
		//   if(iIPNum < 0 || iIPNum > 0xFFFF){return}
		tokenPtr=strtok(NULL, cSepartor);
	}//end while(tokenPtr)
	if(ucPart < 4)
		return FALSE;
	return ucRet;
}

int FTPLogin(FTP_PARAM stFTPParam)
{
	unsigned char sReadBuff[512];
	int iRet;

	if(!sg_bConnected)
		return FTPSeverERR;

	memset(sReadBuff, 0, sizeof(sReadBuff));
	iRet = FTPCmd(NULL, NULL, sReadBuff);
	if (iRet != 220)
	{
		sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
		memcpy(gl_szFTPErrInfo, SeverError, 3);
		return FTPSeverERR;
	}

	iRet = ReadFTPFileInfo();
	if (iRet != OK)
	{
		return iRet;
	}
	if (strcmp(gl_stFTPFileInfo.szFileVer, gl_MaintenaceParam.stFTP.stApp.szFileVer))// if download param, still use stApp
	{
		strcpy(gl_stFTPFileInfo.szFileVer, gl_MaintenaceParam.stFTP.stApp.szFileVer);
		gl_stFTPFileInfo.lCurrLen = 0l;
		gl_stFTPFileInfo.lTotalLen = 0l;
	}

	WriteFTPFileInfo();

	memset(sReadBuff, 0, sizeof(sReadBuff));
	iRet = FTPCmd("USER ", gl_MaintenaceParam.stFTP.szUser, sReadBuff);
	if (iRet != 331)
	{
		sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
		memcpy(gl_szFTPErrInfo, SeverError, 3);
		return FTPSeverERR;
	}

	memset(sReadBuff, 0, sizeof(sReadBuff));
	iRet = FTPCmd("PASS ", gl_MaintenaceParam.stFTP.szPwd, sReadBuff);
	if (iRet != 230)
	{
		sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
		memcpy(gl_szFTPErrInfo, SeverError, 3);
		return FTPSeverERR;
	}

	return OK;
}

int FTPGetFile(unsigned char *pszPCFile, unsigned char *pszPOSFile, unsigned char ucLineNo)
{
	unsigned char szFileName[256];
	unsigned char szPOSFileName[100];
	unsigned char szCmdBuf[100];
	unsigned char sReadBuff[1024*8];
	unsigned char *psFlag;
	unsigned char ucResumeFlag;
	unsigned short usLocalProt;
	unsigned int uiRecvLen;
	int iCnt;
	int iRet;
	int iFileRet;
	long lFileTotalLen;
	long lReadLen;
	int iBlockLen;

	CONN_PARAM stConnParam_DATA;
	int iSocket_CMD;
	int iSocket_DATA = -1;

	if (pszPCFile == NULL)
	{
		sprintf(gl_szFTPErrInfo, "%03d Get File Para ERR", FTPERROR);
		return FTPERROR;
	}

	iSocket_CMD = GetSocket();
	if (iSocket_CMD < 0)
	{
		sprintf(gl_szFTPErrInfo, "%03d SOCKET ERR", FTPERROR); // Added by Kim_LinHB 2013-4-3 v4.57
		return FTPERROR;
	}

	memset(szFileName, 0, sizeof(szFileName));
	strcpy(szFileName, pszPCFile);
	iCnt = 1;
	while(1)
	{
		psFlag = Nstrstr(szFileName, "/", iCnt);
		if (!psFlag)
		{
			break;
		}
		iCnt++;
	}

	if (iCnt == 1)
	{
		psFlag = szFileName;
	}

	if (iCnt > 1)
	{
		iCnt--;
		psFlag = Nstrstr(szFileName, "/", iCnt);
		*psFlag = '\0';
		memset(sReadBuff, 0, sizeof(sReadBuff));
		iRet = FTPCmd("CWD ", szFileName, sReadBuff);
		*psFlag = '/';
		if (iRet != 250)
		{
			sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
			memcpy(gl_szFTPErrInfo, SeverError, 3);
			return FTPSeverERR;
		}
		psFlag++;
	}
	else
	{
		memset(sReadBuff, 0, sizeof(sReadBuff));
		iRet = FTPCmd("CWD /", NULL, sReadBuff);
	}

	memset(sReadBuff, 0, sizeof(sReadBuff));
	iRet = FTPCmd("SIZE ", psFlag, sReadBuff);
	if (iRet != 213)
	{
		sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
		memcpy(gl_szFTPErrInfo, SeverError, 3);
		return FTPSeverERR;
	}
	lFileTotalLen = atol(&sReadBuff[4]);	

	if (memcmp(psFlag, gl_stFTPFileInfo.szFileName, strlen(psFlag)))
	{
		gl_stFTPFileInfo.lCurrLen = 0l;
		gl_stFTPFileInfo.lTotalLen = 0l;
		memset(gl_stFTPFileInfo.szFileName, 0, sizeof(gl_stFTPFileInfo.szFileName));
		strcpy(gl_stFTPFileInfo.szFileName, psFlag);
		WriteFTPFileInfo();
	}

	if (lFileTotalLen != gl_stFTPFileInfo.lTotalLen)
	{
		gl_stFTPFileInfo.lCurrLen = 0l;
		gl_stFTPFileInfo.lTotalLen = lFileTotalLen;
		WriteFTPFileInfo();
	}

	if (lFileTotalLen - gl_stFTPFileInfo.lCurrLen >= freesize())
	{
		sprintf(gl_szFTPErrInfo, "%03d File Memory Overflow[%ld][%ld]", \
			FTPFileMemOverflow, freesize(), lFileTotalLen);
		return FTPFileMemOverflow;
	}

	memset(sReadBuff, 0, sizeof(sReadBuff));
	iRet = FTPCmd("TYPE I", NULL, sReadBuff);
	if (iRet != 200)
	{
		sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
		memcpy(gl_szFTPErrInfo, SeverError, 3);
		return FTPSeverERR;
	}

	memset(sReadBuff, 0, sizeof(sReadBuff));
	iRet = FTPCmd("PASV ", NULL, sReadBuff);
	if (iRet != 227)
	{
		sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
		memcpy(gl_szFTPErrInfo, SeverError, 3);
		return FTPSeverERR;
	}
	else
	{
		FTPGetPort(sReadBuff, &usLocalProt);
		memcpy(&stConnParam_DATA, &gl_MaintenaceParam.stConnParam, sizeof(CONN_PARAM));
		strcpy(stConnParam_DATA.szRemoteIP, gl_MaintenaceParam.stFTP.szRemoteIP);
		sprintf(stConnParam_DATA.szRemotePort, "%d", usLocalProt);
		stConnParam_DATA.ucIPUsing = 0;
		iRet = OK;
		SetSocket(-1);
	}

	if(0 == gl_MaintenaceParam_Ext.cConnectionTimeout)
		gl_MaintenaceParam_Ext.cConnectionTimeout = TIME_OUT_CONN;

	switch (gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
	{
	case ETHERNET:
		iRet = TCPSocketProcess(&stConnParam_DATA, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(iRet != 0)
		{
			vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			iRet = TCPIPConnectERR;
		}
		break;
	case  GPRS:
		iRet = GPRSSocketProcess(&stConnParam_DATA, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(iRet != 0)
		{
			vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			iRet = GPRSConnectERR;
		}
		break;
	}
	if (iRet != OK)
	{
		SetSocket(iSocket_CMD);
		return iRet;
	}

	iSocket_DATA = GetSocket();
	SetSocket(iSocket_CMD);
	ucResumeFlag = 0;
	if (gl_stFTPFileInfo.lCurrLen)
	{
		memset(szCmdBuf, 0, sizeof(szCmdBuf));
		memset(sReadBuff, 0, sizeof(sReadBuff));
		sprintf(szCmdBuf, "%ld", gl_stFTPFileInfo.lCurrLen);
		iRet = FTPCmd("REST ", szCmdBuf, sReadBuff);
		if (iRet == 350)
		{
			lFileTotalLen -= gl_stFTPFileInfo.lCurrLen;
			ucResumeFlag = 1;
		}
		else
		{
			gl_stFTPFileInfo.lCurrLen = 0l;
		}
	}

	memset(sReadBuff, 0, sizeof(sReadBuff));
	iRet = FTPCmd("RETR ", psFlag, sReadBuff);
	if (iRet != 150 && iRet != 125)
	{
		SetSocket(iSocket_DATA);
		TcpClose();
		sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
		memcpy(gl_szFTPErrInfo, SeverError, 3);
		return FTPSeverERR;
	}
	memset(szPOSFileName, 0, sizeof(szPOSFileName));
	if (pszPOSFile == NULL)
	{
		strcpy(szPOSFileName, psFlag);
	}
	else
	{
		strcpy(szPOSFileName, pszPOSFile);
	}

	SetSocket(iSocket_DATA);
	lReadLen = 0l;
	iCnt = 0;
	while(lReadLen < lFileTotalLen)
	{
		memset(sReadBuff, 0, sizeof(sReadBuff));
		iBlockLen = (4096 < (lFileTotalLen - lReadLen)) ? 
			4096 : (lFileTotalLen - lReadLen);
		iRet = Recv(iBlockLen, sReadBuff, &uiRecvLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if (iRet != OK && iRet != NoReponse)
		{
			SetSocket(iSocket_DATA);
			TcpClose();
			SetSocket(iSocket_CMD);
			sprintf(gl_szFTPErrInfo, "Error: %d", iRet);
			return iRet;
		}

		if (iRet == OK)
		{
			lReadLen += uiRecvLen;
			iFileRet = PubFtpFileWrite(szPOSFileName, gl_stFTPFileInfo.lCurrLen, \
				sReadBuff, uiRecvLen);
			if (iFileRet != OK)
			{
				SetSocket(iSocket_DATA);
				TcpClose();
				SetSocket(iSocket_CMD);
				sprintf(gl_szFTPErrInfo, "Error: %d", iFileRet);
				return iFileRet;
			}
			gl_stFTPFileInfo.lCurrLen += uiRecvLen;
			WriteFTPFileInfo();
			if (ucLineNo < 6 && ucLineNo > 0)
			{
				if (ucResumeFlag ==1)
				{
					DispRate(ucLineNo, gl_stFTPFileInfo.lTotalLen, gl_stFTPFileInfo.lCurrLen, 0);
					ucResumeFlag = 0;
				}
				else
				{
					DispRate(ucLineNo, gl_stFTPFileInfo.lTotalLen, gl_stFTPFileInfo.lCurrLen, \
						gl_stFTPFileInfo.lCurrLen - uiRecvLen);
				}
				//ScrPrint(0, 2, ASCII, "Recv[%ld][%ld]", lReadLen, lFileTotalLen);
			}		
			iCnt = 0;
		}

		if (iRet == NoReponse && uiRecvLen == 0)
		{
			iCnt++;
		}
		if (iCnt > 5)
		{
			SetSocket(iSocket_DATA);
			TcpClose();
			SetSocket(iSocket_CMD);
			sprintf(gl_szFTPErrInfo, "%03d Recv Data Timeout", FTPRecvDataTimeout);
			return FTPRecvDataTimeout;
		}

		if (lReadLen >= lFileTotalLen)
		{
			break;
		}
	}

	SetSocket(iSocket_DATA);
	TcpClose();

	SetSocket(iSocket_CMD);
	memset(sReadBuff, 0, sizeof(sReadBuff));
	FTPCmd(NULL, NULL, sReadBuff);
	gl_stFTPFileInfo.lCurrLen = 0;
	gl_stFTPFileInfo.lTotalLen =0;
	memset(gl_stFTPFileInfo.szFileName, 0, sizeof(gl_stFTPFileInfo.szFileName));
	WriteFTPFileInfo();

	return OK;
}

static unsigned char ReadBarCode(unsigned char* pBuffer, int iTimeout)
{
	int i = 0;

	pBuffer[0] = 0;
	if (PortRecv(COM_BARCODE, pBuffer, iTimeout) != 0)
	{
		return BARCODE_TIMEOUT;
	}
	for (i = 1; i < MAX_BARCODE; i++)
	{
		pBuffer[i] = 0;
		pBuffer[i + 1] = 0;
		if (PortRecv(COM_BARCODE, pBuffer + i, 200) != 0)
		{
			return BARCODE_OK;
		}
		if(0x00 == kbhit() && KEYCANCEL == getkey())
			return BARCODE_CANCEL;
	}
	return BARCODE_OVERFLOW;
}

unsigned char ScanBarCode(unsigned char *pszBarCode, unsigned char *pucKey)
{

	unsigned char buffer[MAX_BARCODE + 1];
	unsigned char ucRet = 0, ucKey;
	unsigned char ucArray[5][MAX_BARCODE + 1];//??
	unsigned char ucRetry = 0;

	memset(ucArray, 0, sizeof (ucArray));

#ifdef LHB_TEST
	if (PortOpen(COM_BARCODE, "38400,8,n,1") != 0)
#else
	if (PortOpen(COM_BARCODE, "9600,8,e,1") != 0)
#endif
	{
		ScrPrint(0, 3, 0, "Open RS232 Error");
		getkey();
		return 0xFF;
	}

	ucRetry = 0;

	while (1)
	{
		memset(buffer, 0, sizeof (buffer));

		ucRet = ReadBarCode(buffer, 1000);

		if (ucRet == BARCODE_OK)
		{
			Beep();

			memmove(ucArray[1], ucArray[0], 4 * sizeof(buffer));

			memcpy(ucArray[0], buffer, sizeof(buffer));

			vTrace("BarCode", buffer, MAX_BARCODE, TRACE_WARNING, TYPE_B);

			if (strlen(buffer) <= 8 + 1)
			{
				vTrace("BarCode <= 8", buffer, strlen(buffer), TRACE_WARNING, TYPE_B);

				if(strlen(buffer) > 0)
					memcpy(pszBarCode, buffer, strlen(buffer) - 1);
				break;
			}
			else
			{
				vTrace("BarCode > 8", buffer, strlen(buffer), TRACE_WARNING, TYPE_B);

				memcpy(pszBarCode, &buffer[strlen(buffer) - 8 - 1], 8);

				break;
			}
			vTrace("BarCode final", pszBarCode, strlen(pszBarCode), TRACE_WARNING, TYPE_B);
		}
		else
		{
			if (BARCODE_CANCEL == ucRet)
			{
				ucRet = 0xFF;

				*pucKey = KEYCANCEL;
				break;
			}
			if (ucRet == BARCODE_TIMEOUT)
			{
				vTrace("Scan TimeOut", buffer, MAX_BARCODE, TRACE_ERROR, TYPE_B);
			}
			else if (ucRet == BARCODE_OVERFLOW)
			{
				vTrace("Scan Overflow", buffer, MAX_BARCODE, TRACE_ERROR, TYPE_B);
			}
			else
			{
				vTrace("Unknowned error", buffer, MAX_BARCODE, TRACE_ERROR, TYPE_B);
			}

			vTrace("Error", &ucRet, sizeof (ucRet), TRACE_ERROR, TYPE_N);
		}

		if (BARCODE_TIMEOUT == ucRet)
		{
			ucKey = PubWaitKey(5);

			if (KEYCANCEL == ucKey)
			{
				vTrace("Cancel Utilisateur", NULL, 0, TRACE_WARNING, 0);
				ucRet = 0xFF;

				*pucKey = ucKey;
				break;
			}
			else
			{
				vTrace("Res Input Touche", &ucKey, 1, TRACE_WARNING, TYPE_B);
			}

		}

		if (ucRetry >= 10)
		{
			Beep();
			Beep();

			ScrCls();
			DispString(gl_pLangSelPiont[STR_TRY_AGAIN], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL],  4 | DISP_LINE_RIGHT);

			// Confirmation l'utilisateur
			do
			{
				ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			}while ( (ucKey !=KEYENTER) &&
				(ucKey != KEYCANCEL) &&
				(ucKey != KEYF1) &&
				(ucKey != KEYF2) &&
				(ucKey != KEYF5)
				);

			if (ucKey != KEYENTER)
			{
				ucRet = 0xFF;

				*pucKey = ucKey;
				break;
			}

			ucRetry = 0;
			ScrCls();
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);
		}
		++ucRetry;
	}

	return ucRet;
}

unsigned char Connect(unsigned short uiTCPTimeOutSec)
{
	int iRet = OK;
	if(sg_bConnected)
		return OK;

	if(0 == uiTCPTimeOutSec )
		uiTCPTimeOutSec = TIME_OUT_CONN;

	gl_MaintenaceParam.ucConnSel = 0;
	iRet = ConnectSub(uiTCPTimeOutSec, gl_MaintenaceParam.ucConnType[0]);
	if(0 == iRet)
	{
		return OK;
	}

	gl_MaintenaceParam.ucConnSel = 1;
	iRet = ConnectSub(uiTCPTimeOutSec, gl_MaintenaceParam.ucConnType[1]);
	if(0 == iRet)
	{
		return OK;
	}
	return (unsigned char)iRet;
}

unsigned char FTPConnect(unsigned short uiTCPTimeOutSec)
{
	int iRet = OK;
	if(sg_bConnected)
		return OK;

	if(0 == uiTCPTimeOutSec)
		uiTCPTimeOutSec = TIME_OUT_CONN;

	gl_MaintenaceParam.ucConnSel = 0;
	iRet = FTPConnectSub(uiTCPTimeOutSec, gl_MaintenaceParam.ucConnType[0]);
	if(0 == iRet)
	{
		return OK;
	}

	gl_MaintenaceParam.ucConnSel = 1;
	iRet = FTPConnectSub(uiTCPTimeOutSec, gl_MaintenaceParam.ucConnType[1]);
	if(0 == iRet)
	{
		return OK;
	}
	return (unsigned char)iRet;
}

unsigned char Send(const unsigned char *pData, unsigned int len, unsigned short uiTimeOutSec)
{
	int iRet = OK;

	if(0 == uiTimeOutSec)
		uiTimeOutSec = TIME_OUT_SENDRECV;

	vTrace("Txd()", NULL, 0, TRACE_WARNING, 0);
	switch(gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
	{
	case SIMUL:
		iRet = InputParamKO;
		break;
	case ETHERNET:
		iRet = TCPTxd(pData, len, uiTimeOutSec);
		if (iRet !=OK)
		{
			CloseConn();
		}
		break;
	case  GPRS:
		iRet = GPRSTxd(pData, len, uiTimeOutSec);
		if (iRet !=OK)
		{
			CloseConn();
		}
		break;
// 	case  PPP:
// 		iRet = PPPTxd(pData, len, uiTimeOutSec);
// 		if (iRet !=OK)
// 		{
// 			CloseConn();
// 		}
	}
	if(0 == iRet)
		return OK;
	else
		vTrace("! Txd KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
	return (unsigned char)iRet;
}
unsigned char Recv(unsigned int expLen, unsigned char *pDataOut, unsigned int *lenOut, unsigned short uiTimeOutSec)
{
	int iRet = OK;

	if(0 == uiTimeOutSec)
		uiTimeOutSec = TIME_OUT_SENDRECV;

	vTrace("Rxd()", NULL, 0, TRACE_WARNING, 0);
	switch(gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
	{
	case SIMUL:
		iRet = InputParamKO;
		break;
	case ETHERNET:
		iRet = TcpRxd(pDataOut, expLen, lenOut, uiTimeOutSec); // fix Kim 130816 v4.62 from MAX_TCP_BUFFER to explen
		break;
	case  GPRS:
		iRet = GPRSRxd(pDataOut, expLen, lenOut, uiTimeOutSec); // fix Kim 130816 v4.62 from MAX_TCP_BUFFER to explen
		break;
// 	case  PPP:
// 		iRet = PPPRxd(pDataOut, MAX_TCP_BUFFER, lenOut, uiTimeOutSec);
// 		break;
	}
	if(0 == iRet)
		return OK;
	else
		vTrace("! Rxd KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
	return (unsigned char)iRet;
}
unsigned char CloseConn(void)
{
	int iRet = OK;
	vTrace("CloseConn()", NULL, 0, TRACE_WARNING, 0);
	switch(gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
	{
	case SIMUL:
		iRet = InputParamKO;
		break;
	case ETHERNET:
		iRet = TcpClose();
		break;
	case  GPRS:
		iRet = GPRSClose();
		break;
// 	case PPP:
// 		iRet = PPPClose();
// 		break;
	}
	gl_MaintenaceParam.ucConnSel = 0;
	sg_bConnected = FALSE;
	sg_iSocket = -1;
	if(0 == iRet)
		return OK;
	else
		vTrace("! CloseConn KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
	return (unsigned char)iRet;
}

//FALSE--normal exit, close all
//TRUE--abnormal exit, close all
unsigned char FTPCloseConn(unsigned char bQuitFlag)
{

	unsigned char sReadBuff[256];
	int iRet;

	if (!bQuitFlag)
	{
		memset(sReadBuff, 0, sizeof(sReadBuff));
		iRet = FTPCmd("QUIT", NULL, sReadBuff);
	}

	CloseConn();
	if (bQuitFlag == 0)
	{
		if (iRet != 221)
		{
			sprintf(gl_szFTPErrInfo, "%s", sReadBuff);
			memcpy(gl_szFTPErrInfo, SeverError, 3);
			return FTPSeverERR;
		}
	}

	return OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static int GetSocket(void)
{
	return sg_iSocket;
}
static void SetSocket(int sk)
{
	sg_iSocket = sk;
}

static int ConnectSub(int iTimeout, unsigned char type)
{
	int iRet = OK;
	switch(type)
	{
	case SIMUL:
		iRet = InputParamKO;
		break;
	case ETHERNET:
		vTrace("SocketProcess()", NULL, 0, TRACE_WARNING, 0);
		gl_MaintenaceParam.stConnParam.ucIPUsing = 0;
		iRet = TCPSocketProcess(&gl_MaintenaceParam.stConnParam, iTimeout);
		if(iRet != 0)
		{
			if(strlen(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak) > 0 &&
				strlen(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 0)
			{
				gl_MaintenaceParam.stConnParam.ucIPUsing = 1;
				iRet = TCPSocketProcess(&gl_MaintenaceParam.stConnParam, iTimeout);
				if(iRet != 0)
				{
					vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
					iRet = TCPIPConnectERR;
				}
			}
			else
			{
				vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
				iRet = TCPIPConnectERR;
			}
		}
		break;
	case  GPRS:
		vTrace("GPRSConnect()", NULL, 0, TRACE_WARNING, 0);
		iRet = GPRSConnect(gl_MaintenaceParam.stConnParam.stParamGPRS);
		if(iRet != 0)
		{
			vTrace("! GPRSConnect KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			iRet = GPRSDialERR;
			break;
		}
		DelayMs(500);
		vTrace("SocketProcess()", NULL, 0, TRACE_WARNING, 0);
		gl_MaintenaceParam.stConnParam.ucIPUsing = 0;
		iRet = GPRSSocketProcess(&gl_MaintenaceParam.stConnParam, iTimeout);
		if(iRet != 0)
		{
			if(strlen(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak) > 0 &&
				strlen(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 0)
			{
				DelayMs(500);
				gl_MaintenaceParam.stConnParam.ucIPUsing = 1;
				iRet = GPRSSocketProcess(&gl_MaintenaceParam.stConnParam, iTimeout);
				if(iRet != 0)
				{
					vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
					iRet = GPRSConnectERR;
				}
			}
			else
			{
				vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
				iRet = GPRSConnectERR;
			}
		}
		break;
// 	case PPP:
// 		gl_MaintenaceParam.stConnParam.ucTeleUsing = 0;
// 		gl_MaintenaceParam.stConnParam.ucIPUsing = 0;
// 		iRet = ConnectPPP(&gl_MaintenaceParam.stConnParam, 1250);
// 		if(iRet != OK)
// 		{
// 			if(strlen(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak) > 0 &&
// 				strlen(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 0)
// 			{
// 				gl_MaintenaceParam.stConnParam.ucIPUsing = 1;
// 				iRet = ConnectPPP(&gl_MaintenaceParam.stConnParam, 1250);
// 				if(OK == iRet)
// 				{
// 					break;
// 				}
// 			}
// 		}
// 
// 		gl_MaintenaceParam.stConnParam.ucTeleUsing = 1;
// 		gl_MaintenaceParam.stConnParam.ucIPUsing = 0;
// 		iRet = ConnectPPP(&gl_MaintenaceParam.stConnParam, 1250);
// 		if(iRet != OK)
// 		{
// 			if(strlen(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak) > 0 &&
// 				strlen(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 0)
// 			{
// 				gl_MaintenaceParam.stConnParam.ucIPUsing = 1;
// 				iRet = ConnectPPP(&gl_MaintenaceParam.stConnParam, 1250);
// 			}
// 		}
//  	break;
	}
	if(0 == iRet)
	{
		sg_bConnected = TRUE;
		return OK;
	}
	return (unsigned char)iRet;
}

static int FTPConnectSub(int iTimeout, unsigned char type)
{
	int iRet = OK;

	CONN_PARAM stConn;

	memcpy(&stConn, &gl_MaintenaceParam.stConnParam, sizeof(CONN_PARAM));

	strcpy(stConn.szRemoteIP, gl_MaintenaceParam.stFTP.szRemoteIP);
	strcpy(stConn.szRemotePort, gl_MaintenaceParam.stFTP.szRemotePort);
	stConn.ucIPUsing = 0;

	switch(type)
	{
	case SIMUL:
		iRet = InputParamKO;
		break;
	case ETHERNET:
		vTrace("SocketProcess()", NULL, 0, TRACE_WARNING, 0);
		iRet = TCPSocketProcess(&stConn, iTimeout);
		if(iRet != 0)
		{
			vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			iRet = TCPIPConnectERR;
		}
		break;
	case  GPRS:
		vTrace("GPRSConnect()", NULL, 0, TRACE_WARNING, 0);
		iRet = GPRSConnect(gl_MaintenaceParam.stConnParam.stParamGPRS);
		if(iRet != 0)
		{
			vTrace("! GPRSConnect KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			iRet = GPRSDialERR;
			break;
		}
		DelayMs(500);
		vTrace("SocketProcess()", NULL, 0, TRACE_WARNING, 0);
		iRet = GPRSSocketProcess(&stConn, iTimeout);
		if(iRet != 0)
		{
			vTrace("! SocketProcess KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			iRet = GPRSConnectERR;
		}
		break;
	}
	if(0 == iRet)
	{
		sg_bConnected = TRUE;
		return OK;
	}
	return (unsigned char)iRet;
}

static int TCPSocketProcess(CONN_PARAM *connParam, unsigned short uiTimeOutSec)
{
	int iRet;
	unsigned char ip[MAX_IP + 1];
	unsigned char port[MAX_PORT + 1];

#ifdef _S_SERIES_
	iRet = SxxSocketCheck(sg_iSocket);
	if (RET_TCPOPENED == iRet)
		return OK;

	if(connParam->stParamEth.bDHCP)
	{
		vTrace("SxxDhcpStart()", NULL, 0, TRACE_WARNING, 0);
		iRet = SxxDhcpStart(FALSE, TIME_OUT_STANDARD_30S);
		if (iRet != 0)
		{	
			vTrace("! SxxDhcpStart KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			return TCPIPStartDHCPERR;
		}
	}
	else
	{
		if(FALSE == CheckIP(connParam->stParamEth.szLocalIP) ||
			EthSet(connParam->stParamEth.szLocalIP, NULL, NULL, NULL) != 0)
		{
			return TCPIPSetLocalIPERR;
		}
		if(FALSE == CheckIP(connParam->stParamEth.szNetMask) ||
			EthSet(NULL, connParam->stParamEth.szNetMask, NULL, NULL) != 0)
		{
			return TCPIPSetMaskERR;
		}
		if(FALSE == CheckIP(connParam->stParamEth.szGatewayIP) ||
			EthSet(NULL, NULL, connParam->stParamEth.szGatewayIP, NULL) != 0)
		{
			iRet = TCPIPSetGateWayIPERR;
		}
		
// 		iRet = EthSet(NULL, NULL, NULL, connParam->stParamEth.szDNS);
// 		if(iRet != 0)
// 			iRet = EthSet(NULL, NULL, NULL, connParam->stParamEth.szDNS_Bak);
// 		if(iRet != 0)
// 			return ;
	}
	
	if (0 == connParam->ucIPUsing)
	{
		if(FALSE == CheckIP(connParam->szRemoteIP))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP);
		strcpy(port, connParam->szRemotePort);

	}
	else
	{
		if(FALSE == CheckIP(connParam->szRemoteIP_Bak))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort_Bak) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP_Bak);
		strcpy(port, connParam->szRemotePort_Bak);
	}
	
	connParam->ucIPUsing = 0;  // reset for next trans connect
	//connecting
	{
		int ii =4;
		while (ii-->0)
		{
			iRet = SxxTcpConnect(ip, (unsigned short)atoi(port), uiTimeOutSec);
			if (0 == iRet)
			{
				break;
			}
		}
		if (iRet != 0)
		{
			NetCloseSocket(sg_iSocket);
			return iRet;
		}
	}
#else
	unsigned char i;
	unsigned char sndbuff[50];
	unsigned char rcvbuff[50];

	if (0 == connParam->ucIPUsing)
	{
		if(FALSE == CheckIP(connParam->szRemoteIP))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP);
		strcpy(port, connParam->szRemotePort);

	}
	else
	{
		if(FALSE == CheckIP(connParam->szRemoteIP_Bak))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort_Bak) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP_Bak);
		strcpy(port, connParam->szRemotePort_Bak);
	}

	connParam->ucIPUsing = 0;

	iRet = PortOpen(LANPORT, "115200,8,n,1");
	PortReset(LANPORT);

	if(iRet != 0x00)
	{
		return TCPIPPortOpenERR;				//v4.7
	}

	memset(sndbuff, 0x00, sizeof(sndbuff));
	if(sizeof(sndbuff) > (strlen(ip) + 1))
	{
		sndbuff[0] = 'R';
		memcpy(sndbuff + 1, ip, strlen(ip));
	}
	else
		return TCPIPSetRemoteIPERR;			//v4.7 v4.15

	for(i = 0; i < 2; ++i)
	{
		int idx = 0;
		while(sndbuff[idx])
		{
			PortSend(LANPORT, sndbuff[idx]);
			idx ++;
		}
		PortSend(LANPORT, 0x0a);
		if(SearchOK(2))
			continue;
		else
			break;
	}
	if(2 == i)
		return TCPIPSetRemoteIPERR;			//v4.7 v4.15

	memset(sndbuff, 0x00, sizeof(sndbuff));
	if(sizeof(sndbuff) > (strlen(port) + 1))
	{
		sndbuff[0] = 'P';
		memcpy(sndbuff + 1, port, strlen(port));
	}
	else
		return TCPIPSetRemotePortERR;			//v4.7

	for(i = 0; i < 2; i ++)
	{
		int idx = 0;
		while(sndbuff[idx])
		{
			PortSend(LANPORT, sndbuff[idx++]);
		}
		PortSend(LANPORT, 0x0a);
		if(SearchOK(3))
			continue;
		else
			break;
	}
	if(2 == i)
		return TCPIPSetRemotePortERR;			//v4.7

	if (connParam->stParamEth.bDHCP)
	{
		iRet = P80DhcpStart();
		if (iRet != OK)							
			return TCPIPStartDHCPERR;
	}
	else
	{
		memset(sndbuff, 0x00, sizeof(sndbuff));
		if(sizeof(sndbuff) > (strlen(connParam->stParamEth.szLocalIP) + 1))
		{
			sndbuff[0] = 'L';
			memcpy(sndbuff + 1, connParam->stParamEth.szLocalIP, strlen(connParam->stParamEth.szLocalIP));
		}
		else
			return TCPIPSetLocalIPERR;					//v4.7

		while(!PortRecv(LANPORT, rcvbuff, 500));
		for(i = 0; i < 1; ++i)
		{
			int idx = 0;
			while(sndbuff[idx])
			{
				PortSend(LANPORT, sndbuff[idx++]);
			}
			PortSend(LANPORT, 0x0a);
			if(SearchOK(2))
				continue;
			else
				break;
		}
		if(1 == i)
		{
			TcpClose();
			memset(sndbuff, 0x00, sizeof(sndbuff));
			if(sizeof(sndbuff) > (strlen(connParam->stParamEth.szLocalIP) + 1))
			{
				sndbuff[0] = 'L';
				memcpy(sndbuff + 1, connParam->stParamEth.szLocalIP, strlen(connParam->stParamEth.szLocalIP));
			}
			else
				return TCPIPSetLocalIPERR;

			while(!PortRecv(LANPORT, rcvbuff, 500));
			for(i = 0; i < 2; ++i)
			{
				int idx = 0;
				while(sndbuff[idx])
				{
					PortSend(LANPORT, sndbuff[idx++]);
				}
				PortSend(LANPORT, 0x0a);
				if(SearchOK(2))
					continue;
				else
					break;
			}
			if(2 == i)
			{
				return TCPIPSetLocalIPERR;
			}
		}

		//set MASK
		if(strlen(connParam->stParamEth.szNetMask)>0)
		{
			memset(sndbuff, 0x00, sizeof(sndbuff));
			sndbuff[0] = 'M';
			memcpy(sndbuff + 1, connParam->stParamEth.szNetMask, strlen(connParam->stParamEth.szNetMask));
			for(i = 0; i < 2; ++i)
			{
				int idx = 0;
				while(sndbuff[idx])
				{
					PortSend(LANPORT, sndbuff[idx++]);
				}
				PortSend(LANPORT, 0x0a);
				if(SearchOK(2))
					continue;
				else
					break;
			}
			if(2 == i)
			{
				return TCPIPSetMaskERR;	
			}
		}

		//set GateWay
		if(strlen(connParam->stParamEth.szGatewayIP)>0)
		{
			memset(sndbuff, 0x00, sizeof(sndbuff));
			sndbuff[0] = 'G';
			memcpy(sndbuff + 1, connParam->stParamEth.szGatewayIP, strlen(connParam->stParamEth.szGatewayIP));
			for(i = 0; i < 2; ++i)
			{
				int idx = 0;
				while(sndbuff[idx])
				{
					PortSend(LANPORT, sndbuff[idx++]);
				}
				PortSend(LANPORT, 0x0a);
				if(SearchOK(2))
					continue;
				else
					break;
			}
			if(2 == i)
			{
				return TCPIPSetGateWayIPERR;
			}
		}
	}	

	for(i = 0; i < 2; ++i)
	{
		PortSend(LANPORT, 'E');
		iRet = SearchOK(8);
		if(2 == iRet)
			break;
		if(0 == iRet)
			break;
	}
	if((2 == i) || iRet)
	{
		return TCPIPConnectERR;
	}

	for(i = 0; i < 2; ++i)
	{
		PortSend(LANPORT, 'C');
		iRet = SearchOK(8);
		if(2 == iRet)
			break;
		if(0 == iRet)
			break;
	}
	if((2 == i) || iRet)
	{
		return TCPIPConnectERR;
	}
	
#endif
	return OK;
}

static int GPRSConnect(WIRELESS_PARAM wlParam)
{
	int iRet;
#ifdef _S_SERIES_
	if(WlPppCheck() != 0)
	{
		iRet=WlSelSim(0);
		if (iRet)
		{
			ScrClrLine(4,7);
			ScrPrint(0, 6, ASCII, "WlSelSim=%d", iRet);
			PubWaitKey(2);
			return iRet;
		}
		iRet=WlInit(wlParam.szSimPin);
		if(-212 == iRet)
		{
			iRet = 0;
		}
		if (iRet != 0)
		{
			ScrPrint(0, 6, ASCII, "WNET INIT %d  ", iRet);
			PubWaitKey(2);
			return GPRSInitERR;
		}
	}
#else
	int i;
	iRet = WNetInit();
	if( iRet != 0x00 )
	{
		ScrClrLine(4,7);
		ScrPrintStr(0,4,CFONT,"WNet:%d",iRet);
		PubWaitKey(TIME_DELAY);
		return GPRSInitERR;
	}

	//v4.20-->
	iRet = WNetCheckSim();
	if (iRet != RET_OK && iRet != RET_NEEDPIN)
	{
		ScrClrLine(4,7);
		ScrPrintStr(0,4,CFONT,"WNetCheckSim Error[%d]", iRet);
		PubWaitKey(TIME_DELAY);
		return GPRSInitERR;
	}
	if (RET_NEEDPIN == iRet)
	{
		iRet = WNetSimPin(wlParam.szSimPin);
		if (iRet != RET_OK)
		{
			ScrClrLine(4,7);
			ScrPrintStr(0,4,CFONT,"WNetSimPin Error[%d]", iRet);
			PubWaitKey(TIME_DELAY);
			return GPRSInitERR;
		}
	}
	//v4.20<--

	for(i=0; i<10; ++i)
	{
		iRet = WNetUidPwd(wlParam.szUID, wlParam.szPwd);
		if( OK == iRet)
		{
			break;
		}
		DelayMs(500);
	}

	if(10 == i)
		return GPRSInitERR;	
#endif

	DispWNetSignal();
	iRet = WirelessDial(&wlParam);
	if (iRet)
	{
		GPRSClose();
		ScrPrint(0 ,7, ASCII, "Connect:%d", iRet);
		PubWaitKey(2);
		return GPRSDialERR;
	}
	
	return OK;
}

static int GPRSSocketProcess(CONN_PARAM *connParam, unsigned short uiTimeOutSec)
{
	int iRet;
	unsigned char ip[MAX_IP + 1];
	unsigned char port[MAX_PORT + 1];

#ifdef _S_SERIES_
	iRet = SxxSocketCheck(sg_iSocket);
	if (RET_TCPOPENED == iRet)
	{
		return OK;
	}

	if (0 == connParam->ucIPUsing)
	{
		if(FALSE == CheckIP(connParam->szRemoteIP))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP);
		strcpy(port, connParam->szRemotePort);

	}
	else
	{
		if(FALSE == CheckIP(connParam->szRemoteIP_Bak))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort_Bak) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP_Bak);
		strcpy(port, connParam->szRemotePort_Bak);
	}

	connParam->ucIPUsing = 0;

	//connecting
	{
		int ii =4;
		while (ii-->0)
		{
			iRet = SxxTcpConnect(ip, (unsigned short)atoi(port), uiTimeOutSec);
			if (0 == iRet)
			{
				break;
			}
		}
		if (iRet != 0)
		{
			NetCloseSocket(sg_iSocket);
			return iRet;
		}
	}
	return OK;
#else
//	iRet = WNetTcpCheck();
//	if(RET_TCPOPENED == iRet) 
//		return OK;

	if (0 == connParam->ucIPUsing)
	{
		if(FALSE == CheckIP(connParam->szRemoteIP))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP);
		strcpy(port, connParam->szRemotePort);

	}
	else
	{
		if(FALSE == CheckIP(connParam->szRemoteIP_Bak))
			return TCPIPSetRemoteIPERR;

		if(atoi(connParam->szRemotePort_Bak) > 65535)
			return TCPIPSetRemotePortERR;

		strcpy(ip, connParam->szRemoteIP_Bak);
		strcpy(port, connParam->szRemotePort_Bak);
	}

	connParam->ucIPUsing = 0;

	iRet = WNetTcpConnect(ip, port);				
	if(RET_TCPOPENED == iRet) 
		return OK;
	else 
	{	int j;
		if( RET_NORSP == iRet)
		{
			for(j=0; j<10; ++j)
			{
				DelayMs(500);
				iRet = WNetTcpCheck();
				if(RET_TCPOPENED == iRet)
					return OK;
				if(RET_TCPOPENING == iRet)
					continue;
				else
				{
					break;
				}
			}
		}
	}
	WNetTcpClose();
	return GPRSConnectERR;
#endif
}

// static int ConnectPPP(CONN_PARAM *connParam, unsigned short usLocalPort)
// {
// 	unsigned char ucRet;
// 	int iRet;
// 	int iDialTimes;
// 	COMM_PARA tPPPModemPara;
// 
// 	tPPPModemPara.CHDT = 0;
// 	tPPPModemPara.DT1 = 5; 
// 	tPPPModemPara.DT2 = 7;
// 	tPPPModemPara.HT = 70;
// 	tPPPModemPara.WT = 5;
// 	tPPPModemPara.SSETUP = 0x87;
// 	tPPPModemPara.DTIMES = 1;
// 	tPPPModemPara.TimeOut = 0;
// 	tPPPModemPara.AsMode = 0xd0;
// 	tPPPModemPara.DP = 0;
// 
// 	iDialTimes = 0;
// 	while (iDialTimes < 2)
// 	{
// 		ModemReset();
// 		while (1)
// 		{			
// 			ucRet = ModemCheck();   
// 			if (ucRet != 0x0A)
// 				break;
// 		}
// 
// 		if (0x00 == ucRet)  
// 			break;
// 		else
// 		{
// 			if(0 == connParam->ucTeleUsing)
// 				ucRet = ModemDial(&tPPPModemPara,connParam->szNumPad,1);
// 			else
// 				ucRet = ModemDial(&tPPPModemPara,connParam->szNumPadSec,1);
// 		}
// 		if (ucRet != 0x00)
// 		{
// 			ModemReset();
// 			++iDialTimes;
// 		}
// 	}
// 
// 	connParam->ucTeleUsing = 0;
// 	if (ucRet != 0x00 && iDialTimes >= 2)
// 	{
// 		if (0x33 == ucRet || 0x03 == ucRet)			//v4.7
// 			return DIALLineERR;
// 		else if (0x0d == ucRet)
// 			return DIALLineBusy;
// 		else if (0x05 == ucRet)
// 			return DIALNoTone;	
// 		else
// 			return PPPDialERR;
// 	}
// 
// 	ScrClrLine(DISPLAYLINE, 7);
// 	PubDispString(gl_pLangSelPiont[STR_IDENTIFICATION], DISPLAYLINE | DISP_LINE_CENTER);
// 
// #ifdef _S_SERIES_
// 	vTrace("PppLogin()", NULL, 0, TRACE_WARNING);
// 
// 	iRet = PPPLogin(connParam->stParamGPRS.szUID, connParam->stParamGPRS.szPwd, PPP_ALG_ALL, 0);
// 	if (iRet != 0)
// 	{
// 		vTrace("! PppLogin KO", &iRet, sizeof(iRet), TRACE_ERROR);
// 		OnHook();		//v4.6
// 		return PPPLoginERR;
// 	}
// 
// 	ScrClrLine(DISPLAYLINE, 7);
// 	PubDispString(gl_pLangSelPiont[STR_SERVEUR], DISPLAYLINE | DISP_LINE_CENTER);
// 
// 	vTrace("NetTcpConnect()", NULL, 0, TRACE_ERROR);
// 
// 	if(0 == connParam->ucIPUsing)
// 		iRet = SxxTcpConnect(connParam->szRemoteIP, (unsigned short)atoi(connParam->szRemotePort), TIME_OUT_CONN);
// 	else
// 		iRet = SxxTcpConnect(connParam->szRemoteIP_Bak, (unsigned short)atoi(connParam->szRemotePort_Bak), TIME_OUT_CONN);
// 
// 	connParam->ucIPUsing = 0;
// 	if(iRet != OK)
// 	{
// 		vTrace("! NetTcpConnect KO", &iRet, sizeof(iRet), TRACE_ERROR);
// 		OnHook();		//v4.6
// 		return PPPOpenERR;
// 	}
// #else
// 	vTrace("NpppLogin()", NULL, 0, TRACE_ERROR);
// 
// 	iRet = NpppLogin(connParam->stParamGPRS.szUID, connParam->stParamGPRS.szPwd);
// 	if (iRet != 0)
// 	{
// 		vTrace("! NpppLogin KO", &iRet, sizeof(iRet), TRACE_ERROR);
// 		OnHook();		//v4.6
// 		return PPPLoginERR;
// 	}
// 
// 	ScrClrLine(DISPLAYLINE, 7);
// 	PubDispString(gl_pLangSelPiont[STR_SERVEUR], DISPLAYLINE | DISP_LINE_CENTER);
// 
// 	vTrace("NpppOpen()", NULL, 0, TRACE_ERROR);
// 
// 	if(0 == connParam->ucIPUsing)
// 		iRet = NpppOpen(connParam->szRemoteIP, (unsigned short)atoi(connParam->szRemotePort), usLocalPort, 0);
// 	else
// 		iRet = NpppOpen(connParam->szRemoteIP_Bak, (unsigned short)atoi(connParam->szRemotePort_Bak), usLocalPort, 0);
// 
// 	connParam->ucIPUsing = 0;
// 	if (iRet < 0)
// 	{
// 		vTrace("! NpppOpen KO", &iRet, sizeof(iRet), TRACE_ERROR);
// 		OnHook();		//v4.6
// 		return PPPOpenERR;
// 	}
// 
// 	sg_iSocket = iRet;
// #endif
// 
// 	return OK;
// }

static int WirelessDial(WIRELESS_PARAM *pstParam)
{
	int i,iRet,iTimeoutNum;

	iTimeoutNum = 0;
#ifdef _S_SERIES_
	do {
		iTimeoutNum++;
		for (i = 0;i < 3; ++i)
		{
			WlPppLogin(pstParam->szAPN,pstParam->szUID,
				pstParam->szPwd, 0x1/*pstParam->ucAuth*/,0,0);
			DelayMs(2000);
			while (1 == WlPppCheck())
			{
				iRet = OK;
				if (0 == kbhit() && KEYCANCEL == getkey())
				{
					break;
				}
				DelayMs(200);
				continue;
			}
			if (0 == WlPppCheck())
			{
				iRet = OK;
				break;
			}
			ScrPrint(0,6,ASCII,"PPP Logon time:%d",i);

		}
		if (iRet != OK && 1 == iTimeoutNum)
		{
			ScrPrint(0,7,ASCII,"WlSwitchPower!");
			WlSwitchPower(0);
			DelayMs(10*1000);
			WlSwitchPower(1);
		}
	}
	while (iRet != OK && 1 == iTimeoutNum);
#else
	for(i = 0; i < 3; ++i)
	{
		iRet = WNetDial(pstParam->szAPN, NULL, NULL);
		if(0 == iRet)
			break;
		else
		{
			DelayMs(50);
			continue;
		}
	}

	if(iRet != 0)
		return GPRSDialERR;

	if(0 == gl_MaintenaceParam_Ext.cConnectionTimeout)
		gl_MaintenaceParam_Ext.cConnectionTimeout = TIME_OUT_CONN;

	TimerSet(TIMER_TEMPORARY,(unsigned short) (gl_MaintenaceParam_Ext.cConnectionTimeout * 10));
	while(TimerCheck(TIMER_TEMPORARY))
	{
		iRet = WNetCheck();
		if(RET_LINKOPENED == iRet)
			return OK;  //RET_LINKOPENED
		if(RET_NORSP == iRet)
		{
			DelayMs(1000);
			continue;
		}
		if( iRet != 0x0a )
			break;
	}

	if(RET_NORSP == iRet) 
		return NoReponse;
	else 
		return GPRSDialERR;
#endif
	return iRet;
}

static int TCPTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec)
{
#ifdef _S_SERIES_
	return SxxTcpTxd(psTxdData, uiDataLen, uiTimeOutSec);
#else
	return P80TcpTxd(psTxdData, uiDataLen);
#endif
}

static int GPRSTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec)
{
#ifdef _S_SERIES_
	return SxxTcpTxd(psTxdData, uiDataLen, uiTimeOutSec);
#else
	// Modified by Kim_LinHB 2013-6-25
	unsigned char   ucRet, *psTemp;
	unsigned short	uiCnt, uiPackLen;

	psTemp    = (unsigned char *)psTxdData;
	uiPackLen = 1024;
	for(uiCnt=0; uiCnt<uiDataLen; uiCnt+=uiPackLen)
	{
		ucRet = WNetTcpTxd(psTemp+uiCnt, (unsigned short)(uiCnt+uiPackLen<=uiDataLen ? uiPackLen : uiDataLen-uiCnt));
		if( ucRet!=RET_OK )
		{
			return ucRet;
		}
	}

	return 0;
#endif
}

static int PPPTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec)
{
#ifdef _S_SERIES_
	return SxxTcpTxd(psTxdData, uiDataLen, uiTimeOutSec);
#else
	int iRet = NpppWrite(sg_iSocket, (unsigned char *)psTxdData, uiDataLen);
	if (uiDataLen == iRet)
		iRet = 0;
	else
		iRet = -1;
	return iRet;
#endif
}

static int TcpRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec)
{
#ifdef _S_SERIES_
	return SxxTcpRxd(psRxdData, uiExpLen, puiOutLen, uiTimeOutSec);
#else
	return P80TcpRxd(psRxdData, uiExpLen, puiOutLen, uiTimeOutSec);
#endif
}

static int GPRSRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec)
{
#ifdef _S_SERIES_
	return SxxTcpRxd(psRxdData, uiExpLen, puiOutLen, uiTimeOutSec);
#else
	int iRet, iCnt, iOffset=0;
	while(1)
	{
		iRet=WNetTcpRxd(psRxdData+iOffset, (unsigned short *)&iCnt, uiTimeOutSec * 1000);
		if (iRet != 0)
		{
			if (RET_NORSP == iRet)
			{
				if(psRxdData[0] != 0)
					iRet = 0;
				else
					return NoReponse;
			}
			else
				return iRet;

			break;
		}

		iOffset+=iCnt;
		if (iOffset>=uiExpLen)break;
	}
	*puiOutLen = iOffset;
	return OK;
#endif
}

static int PPPRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec)
{
	int iRet;
	int iLen;
	unsigned char sBuffer[1024];

	*puiOutLen = 0;
	memset(sBuffer,0,sizeof(sBuffer));
	iLen = sizeof(sBuffer);

	TimerSet(TIMER_TEMPORARY, (unsigned short)(uiTimeOutSec *10));

	while (1)
	{
		if (0 == TimerCheck(TIMER_TEMPORARY))
		{
			TcpClose();
			OnHook();
			return TimeOutLecturePortSerie;
		}
#ifdef _S_SERIES_
		iRet = SxxTcpRxd(sBuffer, sizeof(sBuffer), &iLen, uiTimeOutSec);

		if (0 == iRet)
		{
			iRet = iLen;
			break;
		}
		if (iRet < 0)
		{
			TcpClose();
			OnHook();						//v4.6
			return SizeRecueIncorrecte;
		}
#else	
		iRet = NpppRead(sg_iSocket, sBuffer, (unsigned short)iLen);

		if (iRet > 0)
		{
			break;
		}
		if (iRet < 0)
		{
			NpppClose(sg_iSocket);
			OnHook();						//v4.6
			return SizeRecueIncorrecte;
		}
#endif
	}

	if(iRet > 0)
	{
		if (iRet > uiExpLen)
		{
			return SizeRecueIncorrecte;
		}

		memcpy(psRxdData,sBuffer,iRet);  
		*puiOutLen = iRet;
		return OK;
	} 

	return iRet;
}

static int TcpClose(void)
{
#ifdef _S_SERIES_
	return NetCloseSocket(sg_iSocket);
#else 
	if (GPRS == gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
	{
		WNetTcpClose();
	}
	else
	{
		PortSend(LANPORT,'\\');		//build2
		PortSend(LANPORT,'\x1b');		//build2
		PortClose(LANPORT);
	}
	return OK;
#endif
}

static int GPRSClose(void)
{
#ifdef _S_SERIES_
	TcpClose();
	WlPppLogout();
	return OK;
	//WlClosePort();
#else
	WNetMTcpClose(sg_iSocket);
	return WNetClose();
#endif
}

static int PPPClose(void)
{
	int iRet;
#ifdef _S_SERIES_
	iRet = TcpClose();
#else
	iRet = NpppClose(sg_iSocket);
#endif
	OnHook();
	return iRet;
}

#ifdef _S_SERIES_
static int SxxDhcpStart(unsigned char ucForceStart, unsigned char ucTimeOutSec)
{
	int	iRet;
	if (ucForceStart && (DhcpCheck()==0))
	{
		DhcpStop();
	}

	if (ucForceStart || (DhcpCheck()!=0))
	{
		iRet = DhcpStart();
		if (iRet < 0)
		{
			return iRet;
		}

		TimerSet(TIMER_TEMPORARY, (unsigned short)(ucTimeOutSec*10));
		while (TimerCheck(TIMER_TEMPORARY)!=0)
		{
			DelayMs(200);
			iRet = DhcpCheck();
			if (0 == iRet)
			{
				return OK;
			}
		}

		return iRet;
	}
	return OK;
}

static unsigned char SxxSocketCheck(int sk)
{
	int event;
	if(sk<0) return RET_TCPCLOSED;

	event = Netioctl(sk, CMD_EVENT_GET, 0);
	if(event<0)
	{
		NetCloseSocket(sk);
		return RET_TCPCLOSED;
	}	

	if(event&(SOCK_EVENT_CONN|SOCK_EVENT_WRITE|SOCK_EVENT_READ))
	{
		return RET_TCPOPENED;
	}
	else if(event&(SOCK_EVENT_ERROR))
	{
		NetCloseSocket(sk);
		return RET_TCPCLOSED;
	}
	return RET_TCPOPENING;
}

static int SxxTcpConnect(unsigned char *pszIP, unsigned short sPort, unsigned short iTimeoutSec)
{
	int		iRet;
	struct net_sockaddr stServer_addr;

	// Setup socket
	iRet = NetSocket(NET_AF_INET, NET_SOCK_STREAM, 0);
	if (iRet < 0)
	{
		return iRet;	
	}
	sg_iSocket = iRet;

	// Bind IP
	iRet = SockAddrSet(&stServer_addr, pszIP, sPort);
	if (iRet!=0)
	{
		return iRet;
	}

	// set connection timeout
	iRet = Netioctl(sg_iSocket, CMD_IO_SET, 0);//block
	if (iRet < 0)
	{
		return iRet;
	}

	if (iTimeoutSec<3)
	{
		iTimeoutSec = 3;
	}
	iRet = Netioctl(sg_iSocket, CMD_TO_SET, iTimeoutSec*1000);
	if (iRet < 0)
	{
		return iRet;
	}

	// Connect to remote IP

	iRet = NetConnect(sg_iSocket, &stServer_addr, sizeof(NET_SOCKADDR));
	if (iRet!=0)
	{
		NetCloseSocket(sg_iSocket);
	}
	return iRet;
}

static int SxxTcpTxd(const unsigned char *psTxdData, unsigned short uiDataLen, unsigned short uiTimeOutSec)
{
	int iRet;
	int iSendLen;
	int iSumLen;

	iRet = Netioctl(sg_iSocket, CMD_TO_SET, uiTimeOutSec*1000);
	if (iRet < 0)
	{
		return iRet;
	}

	iSumLen = 0;
	while(1)
	{
		if (uiDataLen > MAX_TCP_BUFFER)
		{
			iSendLen = MAX_TCP_BUFFER;
			uiDataLen = uiDataLen - MAX_TCP_BUFFER;
		}
		else
		{
			iSendLen = uiDataLen;
		}	

		iRet = NetSend(sg_iSocket, (unsigned char *)psTxdData+iSumLen, iSendLen, 0);
		if (iRet < 0)
		{
			return iRet;
		}
		if (iRet != iSendLen)
		{
			return -1;
		}
		iSumLen = iSumLen + iSendLen;
		if (iSendLen <= MAX_TCP_BUFFER)
		{
			break;
		}	
	}
	return 0;
}

static int SxxTcpRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec)
{
	int iRet, iCnt, iOffset=0;
	unsigned char bFirst = TRUE;

	DelayMs(200);
	iRet = 	Netioctl(sg_iSocket, CMD_TO_SET, uiTimeOutSec*1000);
	if (iRet < 0)
	{
		*puiOutLen = iOffset;
		return iRet;
	}

// 	while(1)
// 	{
// 		iCnt=NetRecv(sg_iSocket,psRxdData+iOffset,uiExpLen,0);
// 		if (iCnt<0)
// 		{
// 			iRet = iCnt;
// 			if (-13 == iCnt)
// 			{
// 				// Modified by Kim_LinHB 2013-8-19 v4.60
// 				if(iOffset > 0)
// 					iRet = 0;
// 				// Modify End
// 			}
// 			break;
// 		}
// 
// 		// Modified by Kim_LinHB 2013-8-19 v4.60
// 		if(bFirst)
// 		{
// 			bFirst = FALSE;
// 			iRet = 	Netioctl(sg_iSocket, CMD_TO_SET, 500);
// 			if (iRet < 0)
// 			{
// 				break;
// 			}
// 		}
// 		// Modify End
// 
// 		iRet = OK;
// 		iOffset+=iCnt;
// 		if (iOffset>=uiExpLen)break;
// 	}
// 
// 	*puiOutLen = iOffset;
// 	return iRet;
	iRet = NetRecv(sg_iSocket, psRxdData, uiExpLen, 0);
	if (iRet < 0)
	{
		*puiOutLen = 0;
		if(NET_ERR_TIMEOUT == iRet)
			iRet = NoReponse;
		return iRet;
	}
	*puiOutLen = iRet;
	return OK;
}
#else
static unsigned char SearchOK(int TimeOut)
{
	unsigned char ch;
	unsigned short FlagI, FlagJ;

	FlagI = 0;
	FlagJ = 0;
	while(1)
	{
		if(PortRecv(LANPORT, &ch, (unsigned short)(TimeOut*1000)))
		{
			return 1;
		}
		if(ch == 'O')
		{
			FlagI = 1;
			FlagJ = 0;
			continue;
		}
		if(ch == 'K' && FlagI)
		{
			return 0;
		}
		if(ch == 'E')
		{
			FlagI = 0;
			FlagJ = 1;
			continue;
		}
		if(ch == 'R' && FlagJ)
		{
			return 2;
		}
		FlagI = 0;
		FlagJ = 0;
	}
	return OK; // can't be here
}

static int P80DhcpStart(void) //v4.12
{
	unsigned char ucRet;
	int iCnt = 0;
	unsigned char sRecvBuff[200];

	while(!PortRecv(LANPORT, sRecvBuff, 500));
	PortSend(LANPORT, 'Z');
	memset(sRecvBuff, 0, sizeof(sRecvBuff));
	TimerSet(TIMER_TEMPORARY, 10);
	while(1)
	{	
		if (!TimerCheck(TIMER_TEMPORARY))
			break;

		ucRet = PortRecv(LANPORT, &sRecvBuff[iCnt], 50);
		if (0 == ucRet)
		{
			++iCnt;
			if (strstr(sRecvBuff, "\r\n1"))
			{
				return OK;
			}
			if ((strstr(sRecvBuff, "\r\n0")))
			{
				break;
			}
		}
	}	

	while(!PortRecv(LANPORT, sRecvBuff, 500));
	PortSend(LANPORT, 'Y');
	ucRet = SearchOK(2);
	if(ucRet != 0)
	{
		return TCPIPStartDHCPERR;
	}

	while(!PortRecv(LANPORT, sRecvBuff, 500));
	PortSend(LANPORT, 'N');
	iCnt = 0;
	memset(sRecvBuff, 0, sizeof(sRecvBuff));
	TimerSet(TIMER_TEMPORARY, 80);
	while(1)
	{	
		if (!TimerCheck(TIMER_TEMPORARY))
		{
			return TCPIPStartDHCPERR;
		}
		ucRet = PortRecv(LANPORT, &sRecvBuff[iCnt], 50);
		if (ucRet == 0)
		{
			iCnt++;
			if (strstr(sRecvBuff, "\r\nM") && Nstrstr(sRecvBuff, ".", 12) )
			{
				return OK;
			}
		}
	}	
}

static int P80TcpTxd(const unsigned char *psTxdData, unsigned short uiDataLen)
{
	unsigned int i;

	for(i = 0; i < uiDataLen; ++i)
	{
		if( psTxdData[i]=='\\' )  //build2
		{
			if( PortSend(LANPORT, '\\') )
			{
				return 0xFF;
			}
		}

		if(PortSend(LANPORT, psTxdData[i]))
		{
			return 0xFF;
		}
	}

	PortSend(LANPORT,'\\');		//build2
	PortSend(LANPORT,'\x1C');		//build2

	return OK;
}

static int P80TcpRxd(unsigned char *psRxdData, unsigned short uiExpLen, unsigned int *puiOutLen, unsigned short uiTimeOutSec)
{
	unsigned int i = 0;
	unsigned char ucRet;

	// Receive response message
	if (uiTimeOutSec <= 60)
	{
		uiTimeOutSec = 60;
	}
	ucRet = PortRecv(LANPORT, psRxdData + i, uiTimeOutSec * 10);	/* block timeout n*1s */

	if(ucRet != 0) //v4.22
	{
		*puiOutLen = 0;
		return NoReponse;
	}

	++i;

	if (uiExpLen == i)
	{
		*puiOutLen = i;
		return OK;
	}

	while(1)
	{
		ucRet = PortRecv(LANPORT, psRxdData + i, 500);	/* timeout 500ms  old=100*/  //build1
		if(0 == ucRet)
		{
			++i;
			if (uiExpLen == i)
			{
				break;
			}
			continue;
		}
		else
			break;				//8.07
	}

	*puiOutLen = i;
	return OK;
}
#endif

static unsigned char * Nstrstr(unsigned char *psStr, unsigned char *psDes, int iNum)
{
	unsigned char *pstFlag;

	if (iNum <= 0)
		return NULL;

	if (iNum > 0)
	{
		pstFlag = strstr(psStr, psDes);
		if (!pstFlag)
			return NULL;

		if (1 == iNum)
			return pstFlag;
		else
		{
			--iNum;
			return (Nstrstr(pstFlag+strlen(psDes), psDes, iNum));	
		}		
	}
	return NULL;
}

static void MatchOperatorName(const unsigned char *MCC_MNC, unsigned char *Name)
{
	unsigned int size = sizeof(IMSILIST) / sizeof(IMSI_INFO);
	unsigned int i;

	unsigned char tmp[120], *p = tmp;
	memset(tmp, 0, 120);

	for(i = 0; i < strlen(MCC_MNC); ++i)
	{
		if(MCC_MNC[i] != 0x0D && MCC_MNC[i] != 0x0A)
			*(p++) = MCC_MNC[i];
	}

	for(i = 0; i < size; ++i)
	{
		if(0 == memcmp(IMSILIST[i].Code, tmp, 5))
		{
			strcpy(Name, IMSILIST[i].Operator);
			return;
		}
	}
	strcpy(Name, "UNKNOWN");
}

int FTPCmd(const unsigned char *psCmd, const unsigned char *psPara, unsigned char *psReadStr)
{
	int iRet;
	unsigned char szCmdBuf[128];
	unsigned char *psFlagStr;
	unsigned int uiRecvLen;
	int iLen;

	memset(szCmdBuf, 0, sizeof(szCmdBuf));
	if (psCmd) 
	{
		if (!psPara)
			psPara = "";
		sprintf(szCmdBuf, "%s%s\r\n", psCmd, psPara);
		iRet = Send(szCmdBuf, strlen(szCmdBuf), gl_MaintenaceParam_Ext.cConnectionTimeout);
		if (iRet != OK)
			return iRet;

// 		if (ucFTPCommType == FTP_COMM_CDMA)
// 		{
// 			if (strstr(szCmdBuf, "RETR"))
// 			{
// 				return 150;
// 			}
// 		}
	}

	iLen = 0;
	do {
		iRet = Recv(500, psReadStr, &uiRecvLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if (iRet != OK)
			return iRet;
		if (iLen == 0)
		{
			memset(szCmdBuf, 0, sizeof(szCmdBuf));
			memcpy(szCmdBuf, psReadStr, 3);
		}
		iLen += uiRecvLen;

		if (!isdigit(psReadStr[0]) || psReadStr[3] != ' ')
		{
			psFlagStr = strstr(psReadStr, szCmdBuf);
			if (psFlagStr)
			{
				if (*(psFlagStr+3) == ' ')
					memmove(psReadStr, psFlagStr, strlen(psFlagStr));
			}
		}
		else
		{
			psFlagStr = strstr(psReadStr, "\r\n");
			if (psFlagStr)
				*psFlagStr = '\0';
		}
	} while (!isdigit(psReadStr[0]) || psReadStr[3] != ' ');

	psReadStr[3] = '\0';
	iRet = atoi(psReadStr);
	psReadStr[3] = ' ';
	return iRet;
}

int FTPGetPort(unsigned char *pszStr, unsigned short *pusPort)
{
	unsigned char *pstFlagStart;
	unsigned char *pstFlagEnd;

	pstFlagStart = Nstrstr(pszStr, ",", 4);
	if (!pstFlagStart)
	{
		return FTPERROR;
	}
	pstFlagEnd = Nstrstr(pszStr, ",", 5);
	if (!pstFlagEnd)
	{
		return FTPERROR;
	}
	*pstFlagEnd = '\0';
	*pusPort = (atoi(pstFlagStart+1))*256;
	*pstFlagEnd = ',';

	pstFlagStart = strstr(pszStr, ")");
	if (!pstFlagStart)
	{
		return FTPERROR;
	}
	*pstFlagStart = '\0';
	(*pusPort) = (*pusPort) + atoi(pstFlagEnd+1);
	*pstFlagStart = ')';

	return OK;
}
