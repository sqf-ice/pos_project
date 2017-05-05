/*------------------------------------------------------------
* FileName:	debug.h
* Author:	ZhangYuan
* Date:		2017-04-12
------------------------------------------------------------*/

#include <Stdio.h>
#include <Stdlib.h>
#include <String.h>
#include <posapi.h>

#include "../inc/comm.h"
#include "../inc/debug.h"

#define BUF_LEN_2048 2048

extern uchar g_ucCommPort;

void El_Log(LOG_T enLogPrio, unsigned char * pucTag, unsigned char *pucFunc, int iLine, char *formatString, ...)
{
	unsigned char aucTimeBuf[32]={0};
	unsigned char aucBuf[BUF_LEN_2048]={0};
	int iLen=0, iSendLen=980, iOffset=0, iCommPort=0;
	va_list ap;

	if(g_ucCommPort == COM_BT)
	{
		iCommPort = COM_USB;
		PortOpen(iCommPort, "115200,8,n,1");		
	}
	else
	{
		iCommPort = COM_BT;
	}

	GetTime(aucTimeBuf);
	sprintf(aucBuf, "%02x/%02x/%02x %02x:%02x:%02x [%s]-[%s]-[%d]: ", aucTimeBuf[0], aucTimeBuf[1], aucTimeBuf[2], aucTimeBuf[3], aucTimeBuf[4], aucTimeBuf[5], pucTag, pucFunc, iLine);

	va_start(ap, formatString);
	vsprintf(aucBuf+strlen(aucBuf), formatString, ap);
	va_end(ap);

	iLen = strlen(aucBuf);
	while(iOffset < iLen)
	{
		iSendLen = (iLen - iOffset < 1000) ? (iLen - iOffset) : 980;
		PortSends(iCommPort, aucBuf + iOffset, iSendLen);
		iOffset += iSendLen;
		if(iOffset < iLen)
		{
			DelayMs(50);
		}
	}

	//PortSend(PORT_USB, '\n');
}

void El_Log_Hex(LOG_T enLogPrio, unsigned char * pucTag, unsigned char *pucFunc, int iLine, unsigned char *pucMsg, unsigned char *pucHexData, int iHexDataLen)
{
	unsigned char aucTimeBuf[8]={0}, aucBuf[8+1]={0};
	unsigned char aucMsg[1024]={0};
	int i=0, iCommPort=0;

	if(g_ucCommPort == COM_BT)
	{
		iCommPort = COM_USB;
		PortOpen(iCommPort, "115200,8,n,1");		
	}
	else
	{
		iCommPort = COM_BT;
	}

	GetTime(aucTimeBuf);
	sprintf(aucMsg, "%02x/%02x/%02x %02x:%02x:%02x [%s]-[%s]-[%d] %s: ", aucTimeBuf[0], aucTimeBuf[1], aucTimeBuf[2], aucTimeBuf[3], aucTimeBuf[4], aucTimeBuf[5], pucTag, pucFunc, iLine, pucMsg);
	PortSends(iCommPort, aucMsg, strlen(aucMsg));

	for(i=0; i<iHexDataLen; i++)
	{
		memset(aucBuf, 0, sizeof(aucBuf));
		sprintf(aucBuf, "%02x ", pucHexData[i]);
		PortSends(iCommPort, aucBuf, 3);
	}

	PortSend(iCommPort, '\n');
}

