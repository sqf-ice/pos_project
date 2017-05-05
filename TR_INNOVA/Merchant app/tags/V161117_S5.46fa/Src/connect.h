/****************************************************************************
NAME
    connect.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.31      - created
****************************************************************************/
#ifndef _CONNECT_H
#define _CONNECT_H

#include "framework.h"
#include "data_define_merchant.h"

#define COM_BARCODE 0
#define BARCODE_OK			0
#define BARCODE_TIMEOUT		1
#define BARCODE_OVERFLOW	2
#define BARCODE_CANCEL		4
#define MAX_BARCODE 30

#ifdef _S_SERIES_
#define TIME_OUT_CONN		15
#define TIME_OUT_SENDRECV	15
#else
#define TIME_OUT_CONN		20
#define TIME_OUT_SENDRECV	20
#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void GetOperatorName(void);
void DispWNetSignal(void);
unsigned char ScanBarCode(unsigned char *pszBarCode, unsigned char *pucKey);

unsigned char Connect(unsigned short uiTCPTimeOutSec);
unsigned char FTPConnect(unsigned short uiTCPTimeOutSec);
unsigned char Send(const unsigned char *pData, unsigned int len, unsigned short uiTimeOutSec);
unsigned char Recv(unsigned int expLen, unsigned char *pDataOut, unsigned int *lenOut, unsigned short uiTimeOutSec);
unsigned char CloseConn(void);
unsigned char FTPCloseConn(unsigned char bQuitFlag);

unsigned char CheckIP(const unsigned char *IP);
int FTPLogin(FTP_PARAM stFTPParam);
int FTPGetFile(unsigned char *pszPCFile, unsigned char *pszPOSFile, unsigned char ucLineNo);

// Added by Kim_LinHB 2013-7-8 v4.62
int FTPCheckFile(unsigned char *pszPCFile);
int FTPPutFile(unsigned char *pszPCFile, unsigned char *pszPOSFile, unsigned char ucLineNo);

#endif
