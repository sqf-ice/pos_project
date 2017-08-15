/*------------------------------------------------------------
* FileName:	debug.h
* Author:	ZhangYuan
* Date:		2017-04-12
------------------------------------------------------------*/

#ifndef _DEBUG_H_
#define _DEBUG_H_

typedef enum{ 
	LOG_DEBUG, /* Display debugging message*/ 
	LOG_INFO, /* Display prompt message*/ 
	LOG_WARN, /* Display warning message*/ 
	LOG_ERROR, /* Display error message*/ 
} LOG_T;


void El_Log(LOG_T enLogPrio, unsigned char * pucTag, unsigned char *pucFunc, int iLine, char *formatString, ...);
void El_Log_Hex(LOG_T enLogPrio, unsigned char * pucTag, unsigned char *pucFunc, int iLine, unsigned char *pucMsg, unsigned char *pucHexData, int iHexDataLen);


#endif

