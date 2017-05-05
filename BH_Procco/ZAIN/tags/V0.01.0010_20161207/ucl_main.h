

#ifndef __UCL_MAIN_H
#define __UCL_MAIN_H

//#include VERITYPESDEF_H
#include "typedefs_vx.h"
#include "commsdlm_vx.h"
//added by Allen 2016-05-20
#define DM_PREDIAL			0		//
#define DM_DIAL				1		//

#define CM_RAW				0		
#define CM_SYNC				1		
#define CM_ASYNC			2	
//add end
#define TCPMAXSENDLEN 10240
#define	LEN_MAX_COMM_DATA	1024
extern int sock_handle;
extern int ucl_debug_mode;               /* Debug Mode Flag          */
extern int successCommGPRS;
extern int successCommWifi;
extern int successCommEthernet;

unsigned short bcd2short(unsigned char *source,int len);
void bcd2str(unsigned char *dest,unsigned char *source,int len);
void asc2bcd(unsigned char *dest,unsigned char *source,int len);

/****************************************************************************
 功    能：	先判断PPP链路是否联通，若没有联通先进行PPP链接，成功后再进行TCP连
            接；若已联通则直接进行TCP连接。
 传入参数：
			prm：用于初始化的参数
 			iTimeOut：进行PPP连接时的超时时间，进行TCP连接时无用，单位：秒。
			iAliveInterval：PPP链路保持激活的时间，单位：秒。
			ucPredial：是否预拨号
 传出参数：
			无
 返 回 码：	0   成功
            <0  失败
****************************************************************************/
//sint UCL_Gprs_Connect(GprsParamStruct *prm,int iTimeOut, int iAliveInterval, uchar ucPredial);

int UCL_Gprs_Pre_Connect(BYTE *APNIn, BYTE *UidIn, BYTE *PwdIn);
int UCL_Gprs_Full_Connect(GprsParamStruct *gprs, TcpipParamStruct* tcp);

void UCL_Gprs_Disconnect(GprsParamStruct *prm);
//sint UCL_Gprs_Send(unsigned char * inbuffer, short len);
//sint UCL_Gprs_Receive(unsigned char * outBuffer, unsigned short *rcv_len, sint Timeout );
//uchar SocketCheck(int sk);

sint UCL_Tcpip_Connect(TcpipParamStruct* prm, ushort uiTimeOutSec);
int UCL_Tcpip_Disconnect(TcpipParamStruct* prm);
sint UCL_Tcpip_Receive(TcpipParamStruct* prm, byte *buffer, usint *buffer_size,ushort uiTimeOutSec);
int UCL_Tcpip_Send(TcpipParamStruct* prm, byte *buffer, usint buffer_size,ushort uiTimeOutSec);

void dump2CommPort(char *caption, unsigned char *data, int dataLen);
void dumpData(char *caption, char perip, unsigned char charPerLine, unsigned char *data, int dataLen);
int xprintf(int peripheral, char* format, ...);

short UCL_GetSignalStatus(int);
short UCL_MobileInitialize(short);
void SXXWlDispSignal(void);

//added by Allen 2015-05-20
void CalcCRC32(uchar *psData, ushort uiLength, uchar sCRC[4]);
int UpdateTimer(uchar ucTimerNo, ushort *puiTickCnt);
int PSTNDial(uchar ucDialMode, COMM_PARA *CommParam, char *pTelNo, char pMode);
int Conv2AsyncTxd(uchar *psTxdData, ushort uiDataLen);
int PSTNTxd(uchar *psTxdData, ushort uiDataLen, ushort uiTimeOutSec, char pMode);
int PSTNRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen, char pMode);
int PSTNSyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
int PSTNAsyncRxd(uchar *psRxdData, ushort uiExpLen, ushort uiTimeOutSec, ushort *puiOutLen);
int PSTNOnHook(uchar bReleaseAll);
//add end


#endif




