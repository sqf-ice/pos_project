#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "Common.h"										// TCP/IP 					

//#include VERITYPESDEF_H
#include "typedefs_vx.h"
//#include "vxlibs.h"

#include "ucl_main.h"          
#include "UCL_AppObj.h"
#include "commsdlm_vx.h"

#include "debug.h"
#include "appLib.h"
//#include GDMDLM_H

//extern int     h_modem;
extern int ghClock;

extern INT StrLen(PBYTE Src);
extern INT OS_ClearScreen(void);

int sock_handle=-1;
int ucl_debug_mode=0;               /* Debug Mode Flag          */
int successCommGPRS = 0;
int successCommWifi = 0;
int successCommEthernet = 0;


char * RemoveZerosInIpEntry(const char* src);

static int sg_iSocket = -1;

//************************************************************************************/


void UCL_Gprs_GetSignal(uchar bBlockMode)
{
    int ret;
    uchar   signal_level;

    do
    {
        ret = WlGetSignal(&signal_level);
        if (ret==0)
        {
            // show signal here
            ScrSetIcon(ICON_SIGNAL, 1+5-signal_level);
            return;
        }
        if (ret!=WL_RET_ERR_REGING)
        {
            break;
        }
    }while (bBlockMode);
}

static int Wait_WlPppCheck(void)
{
    int ret;
    while (1)
    {
        ret = WlPppCheck();
        if (ret==0) // PPP is ready
        {
            return 0;
        }
        if ((ret!=1) && (ret!=WL_RET_ERR_DIALING) && (ret!=WL_RET_ERR_REGING))  // not busy
        {
            return ret;
        }
    }
}

static int Try_WlPppLogin(uchar *APNIn, uchar *UidIn, uchar *PwdIn, long Auth, int TimeOutSec, int AliveInterval)
{
    int ii, ret;

    UCL_Gprs_GetSignal(1);

    // 检查是否已有连接
    ret = Wait_WlPppCheck();
    if (ret==0)
    {
        return 0;
    }

    // 建立PPP
    if (TimeOutSec<0 || TimeOutSec>30)
    {
        TimeOutSec = 30;
    }
    for (ii=0; ii<3; ii++)
    {
        ret = WlPppLogin(APNIn, UidIn, PwdIn, Auth, TimeOutSec*1000, AliveInterval);
        if (ret==0)
        {
            return 0;
        }
    }

    // 重启模块
    WlSwitchPower(0);
    DelayMs(1000);
    WlSwitchPower(1);
    WlInit(NULL);
    DelayMs(7000);
    UCL_Gprs_GetSignal(1);

    // 重试PPP
    for (ii=0; ii<3; ii++)
    {
        ret = WlPppLogin(APNIn, UidIn, PwdIn, Auth, TimeOutSec*1000, AliveInterval);
        if (ret==0)
        {
            return 0;
        }
    }

    return ret;
}

int Try_Gprs_NectConnect(char *ip_str, short port, int *pSock, int TimeOutSec)
{
    int ii, ret;
    NET_SOCKADDR server_addr;

    if (TimeOutSec<0 || TimeOutSec>55)
    {
        TimeOutSec = 55;
    }
	for (ii=0; ii<3; ii++)
	{
		*pSock = NetSocket(NET_AF_INET, NET_SOCK_STREAM, 0);    
		if (*pSock < 0)
        {
			if (2 == ii)    // modified by Allen 20151112
			{
			return -1;
			}
			else
			{
				continue;
			}
        }
        Netioctl(*pSock, CMD_TO_SET, TimeOutSec*1000);
		SockAddrSet(&server_addr, ip_str, port);
        ScrPrint(0,7,0,"%s:%d",ip_str, (int)(ushort)port);
		ret = NetConnect(*pSock, &server_addr, sizeof(server_addr)); 
		if (ret==0)
        {
            return 0;
        }

        NetCloseSocket(*pSock);
        *pSock = -1;
		DelayMs(500);
	}
    return ret;
}

int UCL_Gprs_Pre_Connect(BYTE *APNIn, BYTE *UidIn, BYTE *PwdIn)
{
    int     ret;

    ret = WlPppCheck();
    if ((ret==1) || (ret==0))
    {
        return 0;
    }

    if ((UidIn!=NULL) && (StrLen(UidIn)==0))
    {
        UidIn = NULL;
    }
    if ((PwdIn!=NULL) && (StrLen(PwdIn)==0))
    {
        PwdIn = NULL;
    }
    WlPppLogin(APNIn, UidIn, PwdIn, 0xFF, 0, 0);
    ret = Wait_WlPppCheck();   //add by richard 20151102
    if (ret==0)
    {
    return 0;
    }
    return ret;     //modified by Allen 20151112
}

// Connect PPP then TCP
int UCL_Gprs_Full_Connect(GprsParamStruct *gprs, TcpipParamStruct* tcp)
{
    int     ret;

	UCL_Gprs_Disconnect(gprs);      //add by Allen 20151111
    ret = Wait_WlPppCheck();
    if (ret!=0)
    {
        ret = Try_WlPppLogin(gprs->apn, gprs->username, gprs->password, 0xFF, 15, 0);
        if (ret!=0)
        {
            return COMMS_DISCONNECTED;
        }
    }

    ret = Try_Gprs_NectConnect(tcp->host_ip, tcp->host_port, &sg_iSocket, 15);	
    if (ret==0)
    {
        return COMMS_SUCCESS;
    }

	WlPppLogout();

    ret = Try_WlPppLogin(gprs->apn, gprs->username, gprs->password, 0xFF, 15, 0);
    if (ret!=0)
    {
        return COMMS_DISCONNECTED;
    }

    ret = Try_Gprs_NectConnect(tcp->host_ip, tcp->host_port, &sg_iSocket, 15);	
    if (ret==0)
    {
        return COMMS_SUCCESS;
    }

    return COMMS_CONNECT_ERROR;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: UCL_Gprs_Disconnect
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */
void UCL_Gprs_Disconnect(GprsParamStruct *prm) 
{	
	int iRet;
	WlPppLogout(); 
	iRet = NetCloseSocket(sg_iSocket);
}



//************************************************************************************/
//************************************************************************************/
sint UCL_Tcpip_Connect(TcpipParamStruct* prm, ushort uiTimeOutSec)
{
	sint		iRet;
	struct net_sockaddr stServer_addr;
	uchar szPort[5] = "";
	
	UCL_Tcpip_Disconnect(prm);
	iRet = DhcpStart();
	if (iRet < 0)
	{
		return iRet;
	}

	// Setup socket
	iRet = NetSocket(NET_AF_INET, NET_SOCK_STREAM, 0);
	if (iRet < 0)
	{
		return iRet;	
	}
	sg_iSocket = iRet;

	// Bind IP
	iRet = SockAddrSet(&stServer_addr, prm->host_ip, prm->host_port);
	if (iRet!=0)
	{
		return iRet;
	}
	iRet = Netioctl(sg_iSocket, CMD_IO_SET, 0);   // add by Allen 20151112
	if (iRet!=0)
	{
		return iRet;
	}

	// set connection timeout
	if (uiTimeOutSec<3)
	{
		uiTimeOutSec = 3;
	}
	iRet = Netioctl(sg_iSocket, CMD_TO_SET, uiTimeOutSec*1000);
	if (iRet!=0)
	{
		return iRet;
	}

	// Connect to remote IP
	iRet = NetConnect(sg_iSocket, &stServer_addr, sizeof(stServer_addr));
	if (iRet!=0)
	{
		NetCloseSocket(sg_iSocket);
	}

	return iRet;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: UCL_Tcpip_Disconnect
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */


int UCL_Tcpip_Disconnect(TcpipParamStruct* prm)
{
	int iRet;

    if (sg_iSocket<0)
    {
        return 0;
    }

	iRet = NetCloseSocket(sg_iSocket);
	if (iRet < 0)
	{
		return iRet;
	}
	return 0;
}
/* --------------------------------------------------------------------------
*
* FUNCTION NAME: UCL_Tcpip_Send
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */

int UCL_Tcpip_Send(TcpipParamStruct* prm, byte *buffer, usint buffer_size,ushort uiTimeOutSec)
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
		if (buffer_size > TCPMAXSENDLEN)
		{
			iSendLen = TCPMAXSENDLEN;
			buffer_size = buffer_size - TCPMAXSENDLEN;
		}
		else
		{
			iSendLen = buffer_size;
		}

		iRet = NetSend(sg_iSocket, buffer+iSumLen, iSendLen, 0);
		if (iRet < 0)
		{
			return iRet;
		}
		if (iRet != iSendLen)
		{
			return -1;
		}
		iSumLen = iSumLen + iSendLen;
		if (iSendLen <= TCPMAXSENDLEN)
		{
			break;
		}	
	}
	return 0;
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: UCL_Tcpip_Receive
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */
sint UCL_Tcpip_Receive(TcpipParamStruct* prm, byte *buffer, usint *buffer_size, ushort uiTimeOutSec)
{
	int iRet;

    if (uiTimeOutSec<0 || uiTimeOutSec>120)
    {
        uiTimeOutSec = 120;
    }

	DelayMs(200);    // add by Allen 20151112
	iRet = 	Netioctl(sg_iSocket, CMD_TO_SET, uiTimeOutSec*1000);
	if (iRet < 0)
	{
        return iRet;
	}

#ifdef WIN32
    DelayMs(2000);
#endif
    DelayMs(1000);
    iRet = NetRecv(sg_iSocket, buffer, LEN_MAX_COMM_DATA, 0);
    if (iRet < 0)
    {
        ScrPrint(0, 7, ASCII, "ERR [%d]", iRet);DelayMs(1000);
        return iRet;
    }
    *buffer_size = iRet;
    return 0;
}

unsigned short bcd2short(unsigned char *source,int len)
{
	short shVal;
	unsigned char tmp_str[20];
	
	memset(tmp_str,0,sizeof(tmp_str));
	bcd2str(tmp_str,source,len);

	shVal =  atoi(tmp_str);

	return shVal;
}

void bcd2str(unsigned char *dest,unsigned char *source,int len)
{
	int i,k;
	k=(len+1)/2;

	if (len % 2) k=1;
	else k=0;

	for (i=0;i<len;i++) 
	{
		if (k) {
			*dest = (*source++ & 0x0f) + 0x30;
			k=0;
		}
		else {
			*dest = (*source >> 4)     + 0x30;
			k=1;
		}
		if (*dest < 0x30 || *dest > 0x39)
			*dest = 0;
		dest++;
	}
	*dest=0;
}

void asc2bcd(unsigned char *dest,unsigned char *source,int len)
{
	int i,k;

	if (len % 2) 
	{
		k=1;
		*dest = 0;
	}
	else k=0;

	for (i=0; i<len; i++) 
	{
		if(k) 
		{
			*dest++ += (source[i] & 0x0F);
			k=0;
		}
		else 
		{
			*dest	= (source[i] << 4);
			k=1;
		}
	}
}



short UCL_GetSignalStatus(int ConnType)
{
	uchar ucRet, ucLevel;	
	ucRet = WlGetSignal(&ucLevel);
	return ucRet;
}

short UCL_MobileInitialize(short ConnType)
{
	int iRet;
	char Time[50] ={0};

	OS_ClearScreen();
	
//	OS_ClkReadDateTime((PBYTE)Time);
//	ScrPrint(0,  0, CFONT|REVER, "%-16.8s", Time);
//	ScrPrint(78, 1, ASCII|REVER, "%.8s", Time+11);

	//sprintf(Time, "%s", "INIT GPRS");
	ScrPrint(0, 0, CFONT|REVER, "    INIT GPRS   ");
	sprintf(Time, "%s", "PLEASE WAIT...");
	ScrPrint(64-strlen(Time)*4, 4, CFONT, "%s", Time);
//	sprintf(Time, "%s", "PLEASE WAIT");
//	ScrPrint(64-strlen(Time)*8, 4, CFONT, "PLEASE WAIT");

	iRet = WlInit(NULL);
	if (iRet == -212)	//模块已初始化
	{
		iRet = 0;
	}
	SXXWlDispSignal();
	
	return iRet;
}

void SXXWlDispSignal(void)
{
	uchar	ucRet, ucLevel;
	
	
	ucRet = WlGetSignal(&ucLevel);
	if( ucRet!=RET_OK )
	{
		ScrSetIcon(ICON_SIGNAL, CLOSEICON);
		return;
	}
	
	ScrSetIcon(ICON_SIGNAL, (uchar)(5-ucLevel));
}

char * RemoveZerosInIpEntry(const char* src)
{
	static char dstIp[20] = {0};
	char tmpBuf[10] = {0};
	int idx = 0,i,j;
	int numCount = 0;
	int hane[4];
	int srcLen = strlen(src);

	memset((void *)&hane[0],0,4*4);
	memset(dstIp,0,20);

	for(i=0;i<4;i++)
	{
		memset(tmpBuf,0,10);

		for(j=0;j<3;j++)
		{
			if(idx >= srcLen)
				break;

			if(src[idx + 1] == '.')
			{				
				tmpBuf[numCount++] = src[idx++];
				tmpBuf[numCount] = '\0';
				numCount = 0;
				idx++;
				break;
			}
			else
			{
				tmpBuf[numCount++] = src[idx++];
			}
		}

		hane[i] = atoi(tmpBuf);

		if(idx >= srcLen)
			break;
	}

	sprintf(dstIp,"%d.%d.%d.%d",hane[0],hane[1],hane[2],hane[3]);
	
	return dstIp;
}


