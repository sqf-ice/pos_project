
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
* Description:bluetooth communicate set
*      
* ============================================================================
*/

#include "..\inc\global.h"


#define LOG_TAG	__FILE__
#ifdef BT_DEBUG		

#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr,...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)


#endif // BT_DEBUG	


int BtOpen(void)
{
	int iRet = -1;
	int i = 0;

	PortClose(COM_BLUETOOTH);

	if (g_BtOpenFlag == 1)
	{
		return BT_RET_ERROR_NOTCLOSE;
	}
	iRet = iMsgInit(g_ucCommPort, MSG_PACKAGE_LEN, MSG_FRAME_LEN);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iMsgInit = %d, gucCommPort = %d\n", iRet, g_ucCommPort);
	if (iRet != 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iMsgInit = %d, gucCommPort = %d\n", iRet, g_ucCommPort);
		return iRet;
	}

	//ScrSetIcon(ICON_BT,1);
	PortReset(g_ucCommPort);
	g_BtOpenFlag = 1;
	return BT_RET_OK;
}

static int getPassword(uchar *password)
{
	int i=0;
	uchar ucKey, szMaskPass[7]={0};

	ScrClrLine(2,5);
	while(1)
	{
		ScrTextOut(70, 30, szMaskPass);
		if(BtIoCtrl(T_eBtCmdGetPasskeyPairRequest, NULL, 0, NULL, 0) != 1)
		{
			return 2;
		}
		if(kbhit()!=NOKEY)
		{
			ucKey = getkey();
		}
		else
			continue;
		if((ucKey>=KEY0) && (ucKey<=KEY9))
		{
			if(i==6)
			{
				Beep();
				continue;
			}
			password[i]=ucKey;
			szMaskPass[i] = ucKey;

			i++;
			ScrClrLine(2,5);
			continue;
		}
		switch(ucKey)
		{
		case KEYCLEAR:if(i==0)
						  Beep();
					  else
					  {
						  password[i-1]='\0';
						  szMaskPass[i-1]='\0';
						  i--;
						  ScrClrLine(2,5);
					  }
					  break;
		case KEYENTER:if(i!=6) 
					  {
						  Beep();
						  break;
					  }
					  else
						  return 0;
		case KEYCANCEL:return 1;
		default:return -1;
		}
	}
	return -1;
}

int BtPairKey(void)
{
	int		iRet;
	uchar	szPassword[7]={0};

	T_ConfigArg	stArgIn;
	char pDispStr[DISP_MAX_LEN+1] = "ENTER PAIR KEY";

	//pArgIn->addr=0x00b;
	memset(&stArgIn, 0, sizeof(stArgIn));
	stArgIn.len = 6;
	stArgIn.szBuf = szPassword;	

	KbLock(2);	// Added by Idina 2015-10-21 enable keyboard pad

	ScrCls();
	PubDispString(pDispStr, 0|DISP_LINE_CENTER|DISP_LINE_REVER);
	iRet = getPassword(szPassword);
	if(0 != iRet)
	{
		memset(pDispStr,0,DISP_MAX_LEN+1);
		if(iRet == 1)
		{
			memcpy(pDispStr,"USER CANCELED",strlen("USER CANCELED"));
			ScrCls();
			PubDispString(pDispStr,2|DISP_LINE_CENTER);
			// Just enter CANCEL key,the D180 will receive the pair request again,could not cancel really.But if D180 send the  wrong pass key,the pair request will be finished.
			memcpy(szPassword, "000000", strlen("000000")+1);
			iRet = BtIoCtrl(T_eBtCmdSetPasskeyPairPassword, &stArgIn, sizeof(T_ConfigArg), 0,0);
			//ScrCls();
		}
		else if(iRet == 2)
		{
			memcpy(pDispStr,"TIME OUT",strlen("TIME OUT"));
			ScrCls();
			PubDispString(pDispStr,2|DISP_LINE_CENTER);
			PubWaitKey(1);
			//memcpy(szPassword,"000000",strlen("000000")+1);
			//BtIoCtrl(eBtCmdSetPasskeyPairPassword, &stArgIn, sizeof(T_ConfigArg), 0,0);
			//ScrCls();
		}	
		return 0;
	}
	memset(pDispStr,0,DISP_MAX_LEN+1);
	memcpy(pDispStr,"BT Pairing...",strlen("BT Pairing..."));
	ScrCls();
	PubDispString(pDispStr,2|DISP_LINE_CENTER);
	iRet = BtIoCtrl(T_eBtCmdSetPasskeyPairPassword, &stArgIn, sizeof(T_ConfigArg), 0,0);
	//ScrCls();
	//ScrPrint(0,3,CFONT,"ret:%d",iRet);
	ScrCls();
	memset(pDispStr,0,DISP_MAX_LEN+1);
	if (iRet == 0)
	{
		memcpy(pDispStr,"Pair success...",strlen("Pair success..."));
	}
	else
	{
		memcpy(pDispStr,"Pair failed...",strlen("Pair failed..."));
	}
	PubDispString(pDispStr,2|DISP_LINE_CENTER);
	PubWaitKey(1);
	return 0;
}


int InitBlueTooth(void)
{
	int iRet = 0;
	T_ConfigArg arg;
	uchar buf[55] = {0};
	uchar TermSN[51] = {0};

	showMessage("Init bluetooth...");

	iRet = BtOpen();
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "BtOpen = %d\n", iRet);
	if (iRet != BT_RET_OK)
	{
		ScrCls();
		Lcdprintf("open bt fail, ret = %d\n", iRet);
		getkey();		
		return iRet;
	}

	memset(TermSN,0,sizeof(TermSN));
	memset(buf,0,sizeof(buf));
	ReadSN(TermSN);
	sprintf(buf,"D180-%s",TermSN);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "InitBlueTooth, buf = %s\n", buf);

	arg.addr = 0x0b;
	arg.len = strlen(buf)+1;
	arg.szBuf = buf;
	iRet = BtIoCtrl(T_eBtCmdSetConfig, &arg, sizeof(arg), NULL, 0);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "eBtCmdSetConfig, iRet = %d\n", iRet);

	iRet = BtIoCtrl(T_eBtCmdDisablePIN, NULL, 0, NULL, 0);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "eBtCmdDisablePIN, iRet = %d\n", iRet);

	iRet = BtIoCtrl(T_eBtCmdEnablePasskey,NULL,0,NULL,0);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "eBtCmdEnablePasskey, iRet = %d\n", iRet);

	ScrCls();

	return 0;
}


