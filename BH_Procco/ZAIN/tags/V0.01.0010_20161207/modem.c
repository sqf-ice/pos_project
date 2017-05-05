#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.



#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "OS_TYPES.h"
#include "os_interface.h"
#include "pax_migrate.h"
#include "commsdlm_vx.h"
#include "utility.h"
#include "ucl_main.h"

#define _fstrchr strchr


static WORD Old_ReceiveBuffer(BYTE *fpStr, BYTE fbWaitFlag, WORD *fwBlockNum);

extern TcpipParamStruct tcpIpStr;		
extern GprsParamStruct	gpsGprsStr;
extern EthernetParamStruct epsEtStr;	
extern TcpipParamStruct tcpIpStr;	


WORD Old_SendBuffer(BYTE *fpStr, WORD fwBlockNum)
{
    WORD lb,lin;
    BYTE lbLrc,MessageLen;
    BYTE laBuf[16];
    BYTE lbFullMessage[/*256+1*/1024]; // 1 for Sending message length

	memset(lbFullMessage,0, /*256*/1024);  // fill send buffer with 0
	// BlockNum is a three digit number so it cannot be greater than 999.
	if (fwBlockNum<1000)
	{
		lin=StrLen(fpStr);                     // Get the length of string..
		if (lin>1)
		{                     // If there is more than 1 byte of data..
			if (fwBlockNum > 0)
			{              // If there is blocknum then put
				L1ConvIntAsc(fwBlockNum, laBuf+2);
				laBuf[0]=STX;
				laBuf[1]='=';
				lbLrc=laBuf[1]^laBuf[2];
				laBuf[3]=0;
			}
			else
			{
				laBuf[0]=STX;
				laBuf[1]=0;
				lbLrc=0;
			}

			for (lb=0 ; lb < lin ; lbLrc^=fpStr[lb++] );  //calculate LRC
			memcpy(lbFullMessage, laBuf, StrLen(laBuf));
			strcat((PCHAR)lbFullMessage, (PCHAR)fpStr);

			lbLrc^=laBuf[0]=ETX;

			laBuf[1]=lbLrc;
			laBuf[2]=0;

			strcat((PCHAR)lbFullMessage, (PCHAR)laBuf);

			MessageLen = StrLen(lbFullMessage);

			// If LRC is zero we must increase the length by one in
			// order to be able to send LRC in the message

			if (lbLrc == 0)
				MessageLen++;

			if (gDebug) Utils_DebugPrint((PBYTE)"Send----------->:",lbFullMessage, MessageLen);
			SendNChar(lbFullMessage, MessageLen);
		}
		else                  // If there is only one byte to send
		{                     // it will be ACK, NAK or EOT..
			lbFullMessage[0] = fpStr[0];
			if (gDebug) Utils_DebugPrint((PBYTE)"Send----------->:",lbFullMessage,1);
			SendNChar(lbFullMessage, 1);
		}
		return (STAT_OK);
	}
	else
	{
		return (STAT_NOK);
	}
}

static WORD Old_ReceiveBuffer(BYTE *fpStr, BYTE fbWaitFlag, WORD *fwBlockNum)
{
    BYTE lbLrc;
    BYTE *tempstr;
    char lbRetry;
    WORD lb;
    WORD lwStatus;

	*fwBlockNum = 0;
	lbRetry = 0;
	lwStatus = STAT_NOK;	// start with status=NOK

	do
	{	// start loop for max retries
		// Now wait in a loop until ACK or NAK or STX or SOH or EOT arrives
		// or timeout occurs.
		memset(fpStr,0,/*256*/1024);

		if ( ReceiveStr(fpStr, MAX_RECV_TIMER) == STAT_OK)
		{
			if ( (fpStr[0] == ACK) &&
			  (fbWaitFlag != WAIT_MODE))
			{
				lwStatus = STAT_OK;    // Return status
				fpStr[0] = ACK;
				fpStr[1] = 0;
			}
			else if( (fpStr[0] == ACK) &&
						(fbWaitFlag == WAIT_MODE)&&
						(StrLen(fpStr)==1))
			{
				lbRetry++;               // increment retry counter
				lwStatus = STAT_GARBAGE;    // Wait for data
			}
			else if (fpStr[0] == NAK)
			{
				lwStatus = STAT_OK;    // Return status
				fpStr[0] = NAK;
				fpStr[1] = 0;
			}
			else if (fpStr[0] == EOT)
			{
				lwStatus = STAT_OK;    // Return status
				fpStr[0] = EOT;
				fpStr[1] = 0;
				return(STAT_OK);
			}
			else if (fpStr[0] == ENQ)
			{
				lbRetry++;               // increment retry counter
				lwStatus = STAT_GARBAGE;
			}
			else
			{
				tempstr = (PBYTE)_fstrchr((PCHAR)fpStr, STX); // Strip garbage from beginning of the buffer
				if (tempstr)
				{
					if (/*(*/(tempstr[0] == STX) ||
//						((tempstr[0] == SOH) && (tempstr[4] == STX))) &&
						 (tempstr[StrLen(tempstr)-2] == ETX))
					{
						lbLrc=0;

//						if (tempstr[0] == SOH)
//						{
//							tempstr[4] = 0;           // Replace STX by NULL for convertion
//							*fwBlockNum = L1ConvAscInt(tempstr+1);
//							tempstr[4] = STX;         // restore STX
//						}

						for (lb=1 ; lb < (WORD)StrLen(tempstr)-1; lb++ )  //calculate LRC
						{
							lbLrc^= tempstr[lb];
						}
						lbLrc ^= tempstr[StrLen(tempstr)-1];

						if (!lbLrc)
						{				// Frame LRC OK !
							lwStatus=STAT_OK;
							tempstr[StrLen(tempstr)-2] = 0;
							StrCpy(fpStr, &tempstr[1]);
							return(STAT_OK);
						}
						else
						{
							DelayMs(1000);
							lwStatus=STAT_NOK;
						}
					}
                    else
					{
						// Data starts with STX but does not end with ETX
						lwStatus = STAT_NOK;    // Return status
						// Wait for 1 sec in order to get the rest of the data
						DelayMs(1000);
						lbRetry++;
					}
				}
				else
				{
					// Data does not start with STX
					lwStatus = STAT_NOK;
					lbRetry++;
					DelayMs(1000);
				}
			}
			// STAT_NOK means that either data is not correct or LRC is wrong.
			// If status is STAT_NOK then send NAK for retry..

			if (lwStatus == STAT_NOK)
			{
				fpStr[0] = NAK;
				fpStr[1] = 0;
				SendNChar(fpStr, 1);	// Send NAK
			}
		}
		else
		{
			return(STAT_NOK);
		}

	}while ((lbRetry < MAX_COMM_RETRY) && ((lwStatus==STAT_TIMEOUT)||(lwStatus==STAT_NOK)||(lwStatus==STAT_GARBAGE))); // end of main while loop..

	return(lwStatus);
}


WORD Send_ReceiveBuffer(BYTE *fpBuf, WORD *fpBlockNum, BYTE fbFlag)
{
    WORD lwStatus;
    BYTE lbRetry;
    WORD lwBlockNum;
    WORD lwSBlockNum;
    BYTE laSaveBuf[/*257*/1024];


	lbRetry = 0;

	if (fpBlockNum == NULL)
	{
		lwBlockNum = 0;
	}
	else
	{
		lwBlockNum = *fpBlockNum;
	}

	lwSBlockNum=lwBlockNum;
	memcpy(laSaveBuf, fpBuf, /*256*/1024);

	// main loop
	do
	{
		memcpy(fpBuf, laSaveBuf, /*256*/1024);
		// do a send...
		if ((lwStatus = Old_SendBuffer(fpBuf, lwSBlockNum)) == STAT_OK)
		{
			// if the send is successful then do a receive..
			memset(fpBuf,0, /*256*/1024);  // fill receive buffer with 0
			if ((lwStatus = Old_ReceiveBuffer(fpBuf, fbFlag, &lwBlockNum)) == STAT_OK)
			{
				// If the receive is successful then check for NAK..
				if (fpBuf[0] != NAK)
				{
					// If received message is not a NAK then check blocknums..
					if ((lwBlockNum) && (lwSBlockNum) &&
						(lwBlockNum != lwSBlockNum+1))
					{
						// If blocknums are incorrect, retry
						lwStatus=STAT_NOK;
					}
				}
				else
				{
					// Received NAK, retry
					lwStatus=STAT_NOK;
				}
			}
			else
			// if the receive returned STAT_NOK that means there was a
			// communication error so there will be no retry..
			if (lwStatus == STAT_NOK)
			{
				lbRetry=MAX_COMM_RETRY;
			}
			// if the receive returned a timeout error then there will be
			// a retry of the send..
			// if the send is not successful there is nothing to do,
			// just exit..
		}
		lbRetry++;

	} while ((lwStatus != STAT_OK) && (lbRetry < MAX_COMM_RETRY));

	return(lwStatus);
}

int SendNChar(BYTE *fpStr,usint fbLength)
{
	// Send
	if(gConnectMode == GPRS)
	{
		return UCL_Tcpip_Send(&tcpIpStr, fpStr, fbLength, 3);
	}
	else
	{
		return PSTNTxd(fpStr, fbLength, 3, CM_SYNC);
	}
}


WORD ReceiveStr(BYTE *fpChar,BYTE fbTimeout)
{
    WORD lwResult;
    WORD length=0;
    usint buffer_size;
    sint ret;


	memset(fpChar,0,/*255*/1024);	

	lwResult = STAT_NOK;
	buffer_size=550;
	//modified by Allen 2016-05-23
	if(gConnectMode == GPRS)
	{
		ret=UCL_Tcpip_Receive(&tcpIpStr, fpChar, &buffer_size,30);
	}
	else
	{
		ret = PSTNRxd(fpChar, 0, 30, &buffer_size, CM_SYNC);
	}
	//modify end
	if(ret ==COMMS_SUCCESS)
    {
		length=buffer_size;
		lwResult = STAT_OK;	
	}	
	
	return(lwResult);
}

void Comms_Disconnect(void)
{
	if(gConnectMode == GPRS)
	{
		UCL_Tcpip_Disconnect(&tcpIpStr);
	}
	else
	{
		PSTNOnHook(0);
	}
}

	

WORD ConnectHost_New(void)     //初始化GPRS参数，并连接
{ 
	unsigned int  lwStatus; 
	GPRSStruct GPRSInp;
	Dial_upStruct   DialInp;  //added by Allen 2016-05-23


	OS_ClearScreen();
	ScrPrint(0, 2, CFONT, "   CONNECTING");
	ScrPrint(0, 4, CFONT, " PLEASE WAIT...");
	//DebugComSend("Enter ConnectHost_New\n");
//#ifndef APPL_IP	//ivy
	//Modified by Allen 2016-05-23
	if(gConnectMode == GPRS)
	{
		if (GPRSRead(&GPRSInp)!=STAT_OK)
		{
			return(STAT_NOK);
		}
	}
	else
	{
		if (Dial_upRead(&DialInp)!=STAT_OK)
		{
			//DebugComSend("Dial_upRead error\n");
			return(STAT_NOK);
		}
	}
//modify end
//#endif

    memset(&epsEtStr,0,sizeof(epsEtStr));
    memset(&tcpIpStr,0,sizeof(tcpIpStr));
    
    strcpy(gpsGprsStr.apn,      GPRSInp.apn);
    strcpy(gpsGprsStr.username, GPRSInp.username); 
    strcpy(gpsGprsStr.password, GPRSInp.password);
    strcpy(gpsGprsStr.host_ip,  GPRSInp.ip_address);
    strcpy(gpsGprsStr.pri_dns,          "0.0.0.0");
    strcpy(gpsGprsStr.sec_dns,          "0.0.0.0");
    strcpy(gpsGprsStr.gateway_address,  "000.000.000.000");
    gpsGprsStr.hport = GPRSInp.hport;
    gpsGprsStr.LoginTimeout=60;
    
    tcpIpStr.host_port = GPRSInp.hport;
    strcpy(tcpIpStr.host_ip,GPRSInp.ip_address);

    epsEtStr.use_dhcp=TRUE;		  
    strcpy(epsEtStr.ip_address, "0.0.0.0");
    strcpy(epsEtStr.netmask,    "0.0.0.0");
    strcpy(epsEtStr.host_ip,    GPRSInp.ip_address);
    strcpy(epsEtStr.pri_dns,    "0.0.0.0");
    strcpy(epsEtStr.sec_dns,    "0.0.0.0");

#ifdef APPL_IP 
    lwStatus = UCL_Tcpip_Connect(&tcpIpStr, 15);
#else
	if(gConnectMode == GPRS)
	{
		lwStatus = UCL_Gprs_Full_Connect(&gpsGprsStr, &tcpIpStr);
	}
	else
	{
		lwStatus = PSTNDial(DM_DIAL, &DialInp.stPara, DialInp.szTelNo, DialInp.ucSendMode);
	}
	//DebugComSend("lwStatus = %d\n", lwStatus);
#endif
	if (lwStatus==COMMS_SUCCESS)
    {
		return (STAT_OK);
    }

	return (STAT_NOK);
//#endif
}

WORD ReceivePacket(BYTE *fpChar,BYTE fbTimeout,WORD *length)
{
    usint buffer_size;
    sint ret;

	buffer_size=550;
//modified by Allen 2016-05-23
	if(gConnectMode == GPRS)
	{
		ret = UCL_Tcpip_Receive(&tcpIpStr, fpChar, &buffer_size, 30);	
	}
	else
	{
		ret = PSTNRxd(fpChar, 0, 30, &buffer_size, CM_SYNC);
	}
	//modify end
	//DebugComSend("ReceiveBuf = %s\n", fpChar); 

	if (ret==0)
    {
		*length = buffer_size;
		return OS_OK;	
	}
	else if (ret==NET_ERR_TIMEOUT || ret == ERR_COMM_TIMEOUT)  //modified by Allen 2016-05-23
	{
		return OS_COM_TIMEOUT;
	}
	else
	{
		return OS_NOK;
	}
}




