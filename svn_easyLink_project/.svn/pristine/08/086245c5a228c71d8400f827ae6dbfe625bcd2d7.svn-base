/*
* ============================================================================
* COPYRIGHT
*              Pax CORPORATION PROPRIETARY INFORMATION
*   This software is supplied under the terms of a license agreement or
*   nondisclosure agreement with Pax Corporation and may not be copied
*   or disclosed except in accordance with the terms in that agreement.
*      Copyright (C) 2016 Pax Corporation. All rights reserved.
* Module Date:2016/08/09     
* Module Auth:huangwp     
* Description:communication module
*      
* ============================================================================
*/


#include "..\inc\global.h"

#include "..\inc\downkey.h"

#define LOG_TAG	__FILE__
#ifdef COMM_DEBUG		
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else
#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)

#endif // COMM_DEBUG	


uchar RecvBuf[BUF_LEN_2048] = {0};
uchar ResponseBuf[BUF_LEN_2048 + ERR_MSG_OFFSET] = {0};

static ushort usFileDownLoadCommand[] = 
{
	EL_CMD_REMOTEDOWNLOAD_TASKINFO,
	EL_CMD_REMOTEDOWNLOAD_SAVEDATA,
	EL_CMD_REMOTEDOWNLOAD_TASKASK,	
	EL_CMD_GET_TERM_SN,
	EL_CMD_GET_TERM_EXTSN,
	EL_CMD_GET_TERM_VER_INFO,
	EL_CMD_GET_TERM_INFO,
};

static uchar *RetToString(int ReturnCode)
{
	switch(ReturnCode)
	{
	case EL_COMM_RET_CONNECTED:
		return "Connected";
	case EL_COMM_RET_DISCONNECT_FAIL:
		return "Disconnect failed";
	case EL_COMM_RET_NOTCONNECTED:
		return "Not connect";
	case EL_COMM_RET_PARAM_FILE_NOT_EXIST:
		return "Pls DownLoad Param File First!";
	default:
		return "Unkown Error";
	}
}


// Please make sure message is shorter than 16 characters
void showMessage(char *message)
{
	int x;

	x = (128 - (strlen(message) * 6)) / 2;
	ScrTextOut(x, 16, message);
}


void InitUsbCom(void)
{
	uchar ucRet;

	showMessage("Init USB...");

	ucRet = PortClose(COM_USB);
	ucRet = iMsgInit(g_ucCommPort, 9999, 900);

	if(ucRet!= 0x00){
		Lcdprintf("PortOpen:%02x", ucRet);
		getkey();
	}
	g_UsbOpenFlag = 1;
	DelayMs(500);
	ScrCls();
}

uchar ComDebugTx(char *psStr, ...)
{   
	uchar ucRet=0;
#ifdef APP_DEBUG
	va_list ap;  
	char buffer[BUF_LEN_2048];
	int i = 0;

	//TimerSet(13, (short)(1));
	//while(TimerCheck(13)!=0);
	DelayMs(10);
	memset(buffer, 0, sizeof(buffer));

	if(0 == g_UsbOpenFlag)
	{
		ucRet = PortOpen(COM_USB, "115200,8,n,1");
		g_UsbOpenFlag = 1;
	}

	va_start(ap, psStr);       
	vsnprintf(buffer, BUF_LEN_2048, psStr, ap);  
	va_end(ap);  

	if(strlen(buffer) < 999)
	{
		ucRet = PortSends(COM_USB, buffer, strlen(buffer)+1);
	}
	else
	{
		for(i = 0; i < strlen(buffer)/999; i ++)
		{
			ucRet = PortSends(COM_USB, buffer, strlen(buffer +999*i)+1);
			DelayMs(500);
		}
	}

#endif
	return ucRet;
}

uchar ComDebugBtTx(char *psStr, ...)
{   
	uchar ucRet=0;
#ifdef APP_DEBUG
	va_list ap;  
	char buffer[BUF_LEN_2048];
	int i = 0;

	//TimerSet(13, (short)(1));
	//while(TimerCheck(13)!=0);
	DelayMs(10);
	memset(buffer, 0, sizeof(buffer));


	va_start(ap, psStr);       
	vsnprintf(buffer, BUF_LEN_2048, psStr, ap);  
	va_end(ap);  

	if(strlen(buffer) < 999)
	{
		ucRet = PortSends(COM_BLUETOOTH, buffer, strlen(buffer)+1);
	}
	else
	{
		for(i = 0; i < strlen(buffer)/999; i ++)
		{
			ucRet = PortSends(COM_BLUETOOTH, buffer, strlen(buffer +999*i)+1);
			DelayMs(500);
		}
	}

#endif
	return ucRet;
}


ushort GetCommand(const uchar *pucMsgDataBuf)
{
	ushort iCommand = -1;

	iCommand = (pucMsgDataBuf[0] << 8) + pucMsgDataBuf[1];
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iCommand = %x, [%02x %02x]\n", iCommand, pucMsgDataBuf[0], pucMsgDataBuf[1]);
	return iCommand;
}


static int SubHandle(const uchar *pucMsgDataBuf, uchar *pucMsgResponse, int *piResponseLen, ushort usCommand, uchar *ucSendResFlag)
{
	int iRet = 0;
	char pDispStr[DISP_MAX_LEN+1] = "PLS DOWN PARAM FILE";

	switch(usCommand)
	{
	case EL_CMD_CONNECT:
		iRet = Connect();
		break;
	case EL_CMD_DISCONNECT:
		iRet = DisConnect(EL_LOGGIC_CONNECT);
		break;
	case EL_CMD_FILE_DOWNLOAD:
		iRet = DownloadFile(pucMsgDataBuf);
		break;
	case EL_CMD_SET_COMPATIBLE_MODE:
		iRet = SetCompatiblaMode(pucMsgDataBuf);
		break;

	case EL_CMD_REMOTEDOWNLOAD_TASKINFO:
		iRet = RemoteDownload_TaskInfo(pucMsgDataBuf, pucMsgResponse, piResponseLen);
		break;

	case EL_CMD_REMOTEDOWNLOAD_SAVEDATA:
		iRet = RemoteDownload_SaveData(pucMsgDataBuf, pucMsgResponse, piResponseLen);
		if(iRet != 0)
		{
			ScrCls();
			g_IdeUi = 0x00;
		}
		break;

	case EL_CMD_REMOTEDOWNLOAD_TASKASK:
		iRet = RemoteDownload_TaskAsk(pucMsgDataBuf, pucMsgResponse, piResponseLen, ucSendResFlag);
		break;

	case EL_CMD_GET_TERM_SN:
		iRet = GetTermSN(pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
		break;

	case EL_CMD_GET_TERM_EXTSN:
		iRet = GetTermExtSN(pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
		break;
	case EL_CMD_GET_TERM_VER_INFO:
		iRet = GetTermVerInfo(pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
		break;

	case EL_CMD_GET_TERM_INFO:
		iRet = GetTerminalInfo(pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
		break;
	case EL_CMD_SHOW_MSG_BOX:
	case EL_CMD_PED_GET_PIN_BLOCK:
	case EL_CMD_PED_ENCRY_DATA:
	case EL_CMD_START_TRANSACTION:
	case EL_CMD_COMPLETE_TRANSACTION:
	case EL_CMD_SET_DATA:
	case EL_CMD_GET_DATA:
		{
			ScrCls();
			ScrPrint(0,2,0,"%s",pDispStr);
			PubWaitKey(3);
			ScrCls();
			ComPoseInfoMsg(RetToString, EL_COMM_RET_PARAM_FILE_NOT_EXIST, 0, __LINE__);
			iRet = EL_COMM_RET_PARAM_FILE_NOT_EXIST;
		}
		break;

	default:
		return EL_CMD_NONE;
	}
	return iRet;
}

int HandleRequest(const uchar *pucMsgDataBuf, uchar *pucMsgResponse, int *piResponseLen, ushort usCommand, uchar *ucSendResFlag)
{
	int ReturnCode = 0;
	int i = 0;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iCommand = %x\n", usCommand);

	if(g_UiFileExist == 0)
	{
		ReturnCode = SubHandle(pucMsgDataBuf, pucMsgResponse, piResponseLen, usCommand, ucSendResFlag);
	}
	else
	{
		switch(usCommand)
		{
		case EL_CMD_CONNECT:
			ReturnCode = Connect();
			break;

		case EL_CMD_DISCONNECT:
			ReturnCode = DisConnect(EL_LOGGIC_CONNECT);
			break;

		case EL_CMD_SHOW_MSG_BOX:
			ReturnCode = ShowMsgBox(pucMsgDataBuf);
			break;

		case EL_CMD_PED_GET_PIN_BLOCK:
			ReturnCode = GetPinBlock((char *)pucMsgDataBuf, (char *)pucMsgResponse, (unsigned int *)piResponseLen);
			break;

		case EL_CMD_PED_ENCRY_DATA:
			ReturnCode = EncryptData((char *)pucMsgDataBuf, (char *)pucMsgResponse, (unsigned int *)piResponseLen);
			break;

		case EL_CMD_START_TRANSACTION:
			ReturnCode = StartTransaction();
			break;

		case EL_CMD_COMPLETE_TRANSACTION:
			ReturnCode = CompleteTransaction();
			break;

		case EL_CMD_SET_DATA:
			ReturnCode = SetData(pucMsgDataBuf, pucMsgResponse, (unsigned int *)piResponseLen);
			break;

		case EL_CMD_GET_DATA:
			ReturnCode = GetData(pucMsgDataBuf, pucMsgResponse, (unsigned int *)piResponseLen);
			break;

		case EL_CMD_FILE_DOWNLOAD:
			ReturnCode = DownloadFile(pucMsgDataBuf);
			if(ReturnCode != 0)	// Added by ZhangYuan 2016-10-27	
			{
				g_IdeUi = 0x00;	
				ScrCls();
			}
			break;

		case EL_CMD_SET_COMPATIBLE_MODE:
			ReturnCode = SetCompatiblaMode(pucMsgDataBuf);
			break;

		case EL_CMD_REMOTEDOWNLOAD_TASKINFO:
			ReturnCode = RemoteDownload_TaskInfo(pucMsgDataBuf, pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
			break;

		case EL_CMD_REMOTEDOWNLOAD_SAVEDATA:
			ReturnCode = RemoteDownload_SaveData(pucMsgDataBuf, pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
			if(ReturnCode != 0)
			{
				ScrCls();
				g_IdeUi = 0x00;
			}
			break;

		case EL_CMD_REMOTEDOWNLOAD_TASKASK:
			ReturnCode = RemoteDownload_TaskAsk(pucMsgDataBuf, pucMsgResponse - ERR_MSG_OFFSET, piResponseLen, ucSendResFlag);
			break;

		case EL_CMD_GET_TERM_SN:
			ReturnCode = GetTermSN(pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
			break;

		case EL_CMD_GET_TERM_EXTSN:
			ReturnCode = GetTermExtSN(pucMsgResponse- ERR_MSG_OFFSET, piResponseLen);
			break;
		case EL_CMD_GET_TERM_VER_INFO:
			ReturnCode = GetTermVerInfo(pucMsgResponse - ERR_MSG_OFFSET, piResponseLen);
			break;

		case EL_CMD_GET_TERM_INFO:
			ReturnCode = GetTerminalInfo(pucMsgResponse- ERR_MSG_OFFSET, piResponseLen);
			break;

		default:
			return EL_CMD_NONE;

		}
	}
	//send response
	return ReturnCode;
}

//added by Kevinliu 20161024
#define SYS_SLEEP_TIMER_NO		1
static void SleepCounterCreate(void)
{
	unsigned long ulTmp=0;

	PubBcd2Long(g_tAppParam.ucSleepModeTimeout, 2, &ulTmp);
	//100ms / number
	TimerSet(SYS_SLEEP_TIMER_NO, ulTmp);
}
static void SleepCounterReset(void)
{
	unsigned long ulTmp=0;

	PubBcd2Long(g_tAppParam.ucSleepModeTimeout, 2, &ulTmp);
	//100ms / number
	TimerSet(SYS_SLEEP_TIMER_NO,  ulTmp);
}
static void SleepCounterCheck(void)
{
	uchar ucDownCtrl[5]={0};
	unsigned long ulTmp=0;

	if ( TimerCheck(SYS_SLEEP_TIMER_NO) <= 0 ) {
		PubBcd2Long(g_tAppParam.ucSleepModeTimeout, 2, &ulTmp);
		//if ucSleepModeTimeout = 0, no sleep.
		if ( ulTmp != 0 ) {
			ucDownCtrl[0]='1';
			ucDownCtrl[1]='1';
			ucDownCtrl[2]=0x00;  //for RTC wake up
			//	ucDownCtrl[3]='0';	 //Display The final interface after sleep
			ucDownCtrl[3] = NULL;	// Clear interface after sleep
			SysSleep(ucDownCtrl);
		}
		TimerSet(SYS_SLEEP_TIMER_NO,  ulTmp);
	}
}

int Processor(void)
{
	int iRet = -1;
	int ResponsLen = 0;
	ushort iCommand = 0;
	//	uchar RecvBuf[BUF_LEN_2048] = {0};
	//	uchar ResponseBuf[BUF_LEN_2042] = {0};
	int iReturnCode = -1;
	uchar SendResponseFlag = 0;
	uchar ucKey = 0;
	unsigned long ulTmp=0;
	uchar ucPhyConnTag = 1;

	SleepCounterCreate();	//added by kevinliu 10161024
	while(1)
	{   
		SleepCounterCheck();	//added by kevinliu 10161024
		if((!g_IdeUi) && (ucPhyConnTag==1))
		{
			//CommDispString("Welcome!",2|DISP_LINE_CENTER);
			iRet = EShowMsgBox(EL_COMM_WELCOME_PAGE, NULL, 0);
			if(iRet !=0)
			{
				CommDispString("Welcome!",2|DISP_LINE_CENTER);
			}
		}

		if(0 == kbhit() )
		{
			SleepCounterReset();		//added by kevinliu 10161024
			if(getkey() == KEYENTER)
			{
				ucKey = PubWaitKey(1);
				if(ucKey == KEYCANCEL)
				{
					//return 0;
					uchar ucCommPort=g_ucCommPort;

					MainMenu();
					if(ucCommPort != g_ucCommPort)
					{
						InitComm();
					}
				}	
			}
		}

		iRet = CheckPhysicConnectStatus(g_ucCommPort);
		if(iRet == EL_CMD_PHYSICAL_DISCONNECTED || iRet == -1)
		{
			g_ConnectFlag = 0;
			ucPhyConnTag = 0;
			continue;
		}
		ucPhyConnTag = 1;

		memset(RecvBuf,0,sizeof(RecvBuf));

		iRet = iReceiveRequest(sizeof(RecvBuf), RecvBuf, &iCommand, 1000);
		if(iRet < 0)
		{
			continue;
		}
		SleepCounterReset();		//added by kevinliu 10161024

		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iReceiveRequest = %d\n", iRet);		
		if(!g_ConnectFlag && (isFindFileDownLoadCmd(iCommand) != PROTOCOL_REMOTEDOWNLOAD_COMMAND))
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Processor have not connect\n");
			if(iCommand != EL_CMD_CONNECT)
			{    
				iReturnCode = EL_COMM_RET_NOTCONNECTED;
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Not connect\n");

				memset(ResponseBuf,0,sizeof(ResponseBuf));
				ResponsLen = 0;
				ComPoseInfoMsg(RetToString, iReturnCode, 0, __LINE__);
				memcpy(ResponseBuf, g_pucErrMsg, ERR_MSG_OFFSET);
				iSendResponse(ResponseBuf, ResponsLen + ERR_MSG_OFFSET, iReturnCode, iCommand);
				continue;
			}
		}

		memset(ResponseBuf,0,sizeof(ResponseBuf));
		ResponsLen = 0;
		iReturnCode = HandleRequest(RecvBuf, ResponseBuf + ERR_MSG_OFFSET, &ResponsLen, iCommand, &SendResponseFlag);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ResponsLen = %d, HandleRequest = %d\n", ResponsLen, iReturnCode);		
		if(EL_CMD_NONE == iReturnCode)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "Processor command error\n");
		}
		if(!SendResponseFlag)
		{
			iSendResponse(ResponseBuf, ResponsLen + ERR_MSG_OFFSET, iReturnCode, iCommand);
			SleepCounterReset();		//added by kevinliu 10161024
		}
		else
		{
			SendResponseFlag = 0;
		}
		if(iCommand == EL_CMD_DISCONNECT)
		{
			iRet = DisConnect(EL_PHYSIC_CONNECT);
			if(iRet < 0)
			{
				return iRet;
			}
		}

		// Added by zhangyuan 2016-09-27 reboot after download the app/monitor/font ========
		if(g_ucRebootFlag == 0x01)
		{
			Beep();
			Reboot();
		}
		// Added end =======================================================================
	}

	return 0;
}


int Connect(void)
{
	if(!g_ConnectFlag)
	{
		g_ConnectFlag = 1;
		return EL_COMM_RET_CONNECT_SUCCESS;
	}
	else
	{
		ComPoseInfoMsg(RetToString, EL_COMM_RET_CONNECTED, 0, __LINE__);
		return EL_COMM_RET_CONNECTED; 
	}
}

int DisConnect(int iConnectType)
{
	int iRet = -1;
	if(iConnectType == EL_LOGGIC_CONNECT)
	{
		g_ConnectFlag = 0;
		iRet = 0;
	}
	else
	{
		switch(g_ucCommPort)
		{
		case COM_BLUETOOTH:
			iRet = BtIoCtrl(eBtCmdDropLink, 0, 0, 0, 0);
			if(iRet < 0)
			{
				return EL_COMM_RET_DISCONNECT_FAIL;
			}
			DelayMs(500);
			iRet = BtIoCtrl(eBtCmdReset, 0, 0, 0, 0);
			if(iRet < 0)
			{
				return EL_COMM_RET_DISCONNECT_FAIL;
			}
			break;

		case COM_USB:
			iRet = PortReset(COM_USB);
			break;
		default:
			break;
		}
	}
	if(iRet < 0)
	{
		return EL_COMM_RET_DISCONNECT_FAIL;
	} 	 
	return EL_COMM_RET_CONNECT_SUCCESS;
}


int CheckPhysicConnectStatus(uchar iPort)
{
	int iRet = -1;

	switch(iPort)
	{
	case COM_BLUETOOTH:
		{
			iRet = BtIoCtrl(T_eBtCmdGetLinkStatus, NULL, 0, NULL, 0);
			if (1 == iRet)
			{
				return EL_CMD_PHYSICAL_CONNECTED;
			}

			iRet=BtIoCtrl(T_eBtCmdGetPasskeyPairRequest, NULL, 0, NULL, 0);

			if(iRet == 1)
			{
				iRet = BtPairKey();
				ScrCls();
				return iRet;
			}
			return EL_CMD_PHYSICAL_DISCONNECTED;
		}
	case COM_USB:
		{
			iRet = PortTxPoolCheck(COM_USB);
			if(0x03 == iRet)
			{
				return EL_CMD_PHYSICAL_DISCONNECTED;
			}
			return 0;
		}
	default:
		return -1;
	}
}


int isFindFileDownLoadCmd(ushort usCommand)
{
	int index = 0;

	int Len = 0;
	Len = sizeof(usFileDownLoadCommand);

	for(index = 0; index < Len/2; index++)
	{
		if(usCommand == usFileDownLoadCommand[index])
		{
			return PROTOCOL_REMOTEDOWNLOAD_COMMAND;
		}
	}

	return -1;
}

int SetCompatiblaMode(const uchar *pucMsgDataBuf)
{
	//ProtocolType = PROTOCOL_COMPATIBLE_MODE;
	if(!pucMsgDataBuf[0])
	{
		ProtocolType = PROTOCOL_NORMAL_MODE;
	}
	else
	{
		ProtocolType = PROTOCOL_COMPATIBLE_MODE;
	}
	return EL_RET_OK;
}

int ReadCommParam()
{
	int iFd = 0;
	int iRet = 0;
	int iWriteLen = 0;
	int iReadLen = 0;

	if(PdkFile_Exist (COMM_TYPE_FILE) != PDK_RET_OK)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "file [comm_type] not exist\n");
		g_ucCommPort = COM_BLUETOOTH;
		iFd = PdkFile_Open (COMM_TYPE_FILE);
		if(iFd < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "file Not Exsit, PdkFile_Open = %d\n", iFd);
			return iFd;
		}

		iWriteLen = PdkFile_Write (iFd, &g_ucCommPort, 1);
		if(iWriteLen < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write = %d\n", iWriteLen);
			PdkFile_Close(iFd);
			return iWriteLen;
		}
		PdkFile_Close(iFd);
	}
	else
	{
		iFd = PdkFile_Open (COMM_TYPE_FILE);
		if(iFd < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open = %d\n", iFd);
			return iFd;
		}
		iReadLen = PdkFile_Read (iFd, &g_ucCommPort, 1);
		if(iReadLen < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Read = %d\n", iReadLen);
			PdkFile_Close(iFd);
			return iFd;
		}		
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_ucCommPort = %d\n", g_ucCommPort);
	}
	return 0;

}

static int SaveCommParam()
{
	int iFd = 0;
	int iWriteLen = 0;
	uchar ucWriteBuf[10] = {0};

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_ucCommPort = %d\n", g_ucCommPort);

	memcpy(ucWriteBuf, &g_ucCommPort, 1);

	iFd = PdkFile_Open (COMM_TYPE_FILE);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Open = %d\n", iFd);
		return iFd;
	}

	iWriteLen = PdkFile_Write (iFd, ucWriteBuf, 1);
	if(iWriteLen < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PdkFile_Write = %d\n", iWriteLen);
		PdkFile_Close(iFd);
		return iWriteLen;
	}
	PdkFile_Close(iFd);
	return 0;
}

static void DispInfo(short iTimeout)
{
	uchar	szBuff[32], szVersion[8 + 1];
	char pDispStr[DISP_MAX_LEN+1]="firmware version:";
	ScrCls();

	PubDispString("TERMINFO", 0|DISP_LINE_REVER|DISP_LINE_CENTER);

	// App Info
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char*)szBuff,"%s V%s",AppInfo.AppName,AppInfo.AppVer);
	//PubDispString(szBuff,0|DISP_LINE_CENTER);
	ScrPrint(0,2,0,"%s",szBuff);
	memset(szBuff,0,sizeof(szBuff));
	PubBcd2Asc((uchar *)AppInfo.DownloadTime,4, szBuff);
	ScrPrint(0,3,0,"%s%s","Date:", szBuff);
	//PubDispString(pDispStr,3|DISP_LINE_LEFT);
	//ScrPrint(0,4,0,"%s",pDispStr);

	// Firmware version Info
	memset(szBuff, 0, sizeof(szBuff));
	memset(szVersion, 0, sizeof(szVersion));
	ReadVerInfo(szVersion);
	sprintf(szBuff, "%s%u.%02u", pDispStr, szVersion[1], szVersion[2]);
	ScrPrint(0,4,0,"%s", szBuff);

	// timeout control
	PubWaitKey(iTimeout);
	ScrCls();	
}
// added end

void CommTypeSelect()
{
	int iRet = 0;
	uchar ch = 0;

	while(1)
	{
		ScrCls();
		PubDispString("SETCOMM", 0|DISP_LINE_REVER|DISP_LINE_CENTER);
		ScrClrLine(2, 5);

		if(g_ucCommPort == COM_USB)
		{
			ScrPrint(0, 2, CFONT, "1-USB(*)");
			ScrPrint(0, 4, CFONT, "2-BLUETOOTH");
		}
		else if(g_ucCommPort == COM_BLUETOOTH)
		{
			ScrPrint(0, 2, CFONT, "1-USB");
			ScrPrint(0, 4, CFONT, "2-BLUETOOTH(*)");
		}
		else
		{
			ScrPrint(0, 2, CFONT, "1-USB");
			ScrPrint(0, 4, CFONT, "2-BLUETOOTH");
		}

		ch = getkey();
		switch(ch)
		{
		case KEY1:
			g_ucCommPort = COM_USB;
			SaveCommParam();
			return;
		case KEY2:
			g_ucCommPort = COM_BLUETOOTH;
			SaveCommParam();
			return;
		case KEYCANCEL:
			return;
		default:
			break;
		}
	}

}


#if 0
int MainMenu()
{
	int iRet = 0;
	uchar ch = 0;

	while(1)
	{
		ScrCls();
		PubDispString("MENU", 0|DISP_LINE_REVER|DISP_LINE_CENTER);
		ScrClrLine(2, 5);
		ScrPrint(0, 2, CFONT, "1-SETCOMM");
		ScrPrint(0, 4, CFONT, "2-TERMINFO");


		ch = getkey();

		switch(ch)
		{
		case KEY1:
			CommTypeSelect();
			break;
		case KEY2:
			DispInfo(10);
			break;
		case KEYCANCEL:
			ScrCls();
			return COMM_ERR_USER_CANCAL;
		default:
			break;					
		}
	}
}
#endif

int OpenPort(void)
{
	uchar	ucRet, ucKey;

	ScrCls();
	ScrPrint(0, 2, CFONT, "Open Port...");
	DelayMs(200);
	ucRet = PortOpen(COM_USB, "115200,8,N,1");
	if (ucRet == 0x00)
	{
		ScrCls();
		ScrPrint(0, 4, CFONT, "Port Open Ok!");
		DelayMs(200);
		PortReset(0);
		return 0;
	}
	else
	{
		ScrCls();
		ScrPrint(0, 4, CFONT, "Port Open Err!");
		kbflush();
		ucKey = getkey();
		if (ucKey == KEYCANCEL)
		{
			return -1;
		}
		return -2;
	}
}

// Destroy All Keys
int DestroyKey(void)
{
	uchar ucKey;
	int iRet;

	ScrCls();
	//	iRet = InputErasePwd();
	// 	if (iRet != 0)
	// 	{
	// 		return iRet;
	// 	}

	ScrCls();
	PubShowMsg(0, "Erase All Key?");
	PubShowMsg(2, "<enter>:YES");
	PubShowMsg(4, "<cancer>:NO");

	kbflush();
	do 
	{
		ucKey = getkey();
	} while (ucKey!=KEYENTER && ucKey!=KEYCANCEL && ucKey!=KEYFN);	

	if (ucKey == KEYENTER)
	{
		ScrCls();
		PubShowMsg(4, "Erasing...");
		iRet = PedErase();
		if (iRet == 0)
		{
			ScrCls();
			PubShowMsg(4, "Erase Key OK!");
			DelayMs(1000);
		}
		else
		{
			ScrCls();
			PubShowMsg(4, "Key Not Erased!");
			DelayMs(1000);
		}
	}
	// 	else if (ucKey == KEYFN)
	// 	{
	//		InputNewErasePwd();
	// 	}

	return 0;
}


int DownKeyProcess(void)
{
	int iRet = 0, iTmp=0;
	int iMenuItem = 0;
	unsigned char 	aucTitle[DISP_MAX_LEN+1]="KEY INJECTION";


	MenuItem stAppMenu[] = 
	{
		{1,"Download Key",NULL},
		{1,"Erase All Key",NULL},
	};	// for D180, the surpported max app number is 5

	while (1)
	{
		iRet = OpenPort();
		if (iRet == -1)
		{
			PortClose(COM_USB);
			return 0;
		}
		else if (0 == iRet)
		{
			break;
		}

		PortClose(COM_USB); //³¢ÊÔ¹Ø±Õ´®¿ÚÖØÐÂ´ò¿ª
	}// ´ò¿ª´®¿Ú


	iMenuItem = SelectMenu(aucTitle, MIN(MAX_APP_ITEMS, sizeof(stAppMenu)/sizeof(stAppMenu[0])), stAppMenu, 0);

	switch(iMenuItem)
	{
	case 0:
		iTmp = g_ucCommPort;
		g_ucCommPort = COM_USB;
		iRet = DownloadKey();
		g_ucCommPort = iTmp;
		break;
	case 1:
		iRet = DestroyKey();
	default:
		break;					
	}
	ScrCls();

	return iRet;
}

int MainMenu()
{
	int iMenuItem = 0;
	unsigned char 	aucTitle[DISP_MAX_LEN+1]="MainMenu";


	MenuItem stAppMenu[] = 
	{
		{1,"SETCOMM",NULL},
		{1,"TERMINFO",NULL},
		{1,"KeyInject",NULL},
	};	// for D180, the surpported max app number is 5

	iMenuItem = SelectMenu(aucTitle, MIN(MAX_APP_ITEMS, sizeof(stAppMenu)/sizeof(stAppMenu[0])), stAppMenu, 0);

	switch(iMenuItem)
	{
	case 0:
		CommTypeSelect();
		break;
	case 1:
		DispInfo(10);
		break;
	case 2:
		DownKeyProcess();
		break;
	default:
		break;					
	}
	ScrCls();
	return 0;
}


void InitComm()
{
	uchar ucRet = -1;

	if(g_UsbOpenFlag == 1)
	{
		PortClose(COM_USB);
		g_UsbOpenFlag = 0;
	}
	if(g_BtOpenFlag == 1)
	{
		PortClose(COM_BLUETOOTH);
		g_BtOpenFlag = 0;
	}

	ReadCommParam();
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "g_ucCommPort = %d\n", g_ucCommPort);
	switch(g_ucCommPort)
	{
	case COM_USB:
		InitUsbCom();
		break;
	case COM_BLUETOOTH:
		InitBlueTooth();
		break;
	default:
		break;
	}
}



void CommDispString(void *pszStr, ushort nPosition)
{
	uchar mode, x, y, ucLen, buffer[60];
	uchar ucReverFlag;

	if (pszStr == NULL)
	{
		return;
	}
	mode  = nPosition & 0x00f0;
	y     = nPosition & 0x000f;

	ucLen = strlen(pszStr);

	if (ucLen > NUM_MAXZHCHARS)
	{
		ucLen = NUM_MAXZHCHARS;
	}

	switch(mode)
	{
	case DISP_LINE_CENTER:
		//ScrClrLine(y, (uchar) (y + 1));
		x = (64 - ucLen * 4);
		break;

	case DISP_LINE_RIGHT:
		x = (128 - ucLen * 8);
		break;

	case DISP_LINE_LEFT:
	default:
		x = 0;
		break;
	}

	if (nPosition & DISP_SCR_CENTER)
	{
		//ScrCls();
		x = (64 - ucLen * 4);
		y = 3;
	}
	ucReverFlag = 0;

	if (nPosition & DISP_LINE_REVER || nPosition & DISP_PART_REVER)
	{
		ucReverFlag = REVER;
	}
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, pszStr, ucLen);
	//ScrClrLine(y, (uchar)(y + 1));

	if (nPosition & DISP_LINE_REVER)
	{
		ScrPrint(0, y, CFONT | REVER, "%*s", NUM_MAXZHCHARS, "");
	}
	ScrPrint(x, y, (uchar)(CFONT | ucReverFlag), "%s", (char *)buffer);
}


//the pucErrMsg's max len is 59 bytes
void ComPoseInfoMsg(RET_TO_STRING_CB vFunc, int MoudleReturnCode, int iErrCode, int iLineNum)
{
	int iOffset = 0;
	iOffset = sizeof(int);
	memset(g_pucErrMsg , 0, sizeof(g_pucErrMsg));

	sprintf(g_pucErrMsg,"%.*s%s%d%s%d%s", ERR_MSG_OFFSET-EL_INTERGER_STRING_LEN-EL_INTERGER_LINE_NUM_LEN-12,vFunc(MoudleReturnCode),"(Err:", iErrCode,",Line:",iLineNum,")");
	memcpy(ResponseBuf, g_pucErrMsg, ERR_MSG_OFFSET);

}



