
#ifndef _DOWNKEY_H
#define _DOWNKEY_H

#include <posapi.h>
#include <posapi_all.h>
#include "appLib.h"

#define STX			0x02
#define	ETX			0x03

#define	IPEK_LEN	16
#define KSN_LEN		8

#define MAST_KEY	0
#define DUKPT_KEY	1
#define TPK_KEY     2
#define TDK_KEY     3
#define TAK_KEY     4



//#define DEBUG_OUTPUT_PRN

extern int gliPortNum;

int DownloadKey(void);
int SendRespPack(uchar *sContent,int iLen);
int TestDukptEncryPin();



#endif 
//end of file
