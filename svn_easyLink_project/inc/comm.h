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


#ifndef _COMM_H
#define _COMM_H

#define COMM_ERR_BASE                   -2400
#define COMM_ERR_USER_CANCAL         	COMM_ERR_BASE-1



#define COMM_TYPE_FILE                  "comm_type"

//communication moudle return code
enum
{
	EL_COMM_RET_BASE = 1000,
	EL_COMM_RET_CONNECT_SUCCESS = 0000,		
	EL_COMM_RET_CONNECTED = 1001,
	EL_COMM_RET_DISCONNECT_FAIL = 1002,
	EL_COMM_RET_NOTCONNECTED = 1003,
	EL_COMM_RET_PARAM_FILE_NOT_EXIST = 1004,
}EL_COMM_RETURN_CODE;

enum
{
    COM_BLUETOOTH = 7,
	COM_USB = 11	
}COMM_TYPE;

enum
{
    EL_CMD_NONE = 0x9999,
	EL_CMD_PHYSICAL_CONNECTED = 0xFFFE,
	EL_CMD_PHYSICAL_DISCONNECTED = 0xFFFF
}COMM_ERR;

enum 
{
	EL_CMD_CONNECT = 0x8000,
	EL_CMD_DISCONNECT = 0x8001,
	//reserve
	EL_CMD_MODULE_COMMAND_END = 0x800F
}EL_COMM_MODULE_COMMAND;

enum 
{
	EL_CMD_SHOW_MSG_BOX = 0x8010,
	//reserve
	EL_UI_MODULE_COMMAND_END = 0x801F
}EL_UI_MODULE_COMMAND;


enum 
{
	EL_CMD_PED_GET_PIN_BLOCK = 0x8020,
	EL_CMD_PED_ENCRY_DATA = 0x8021,
	//reserve
	EL_SECURITY_MODULE_COMMAND_END = 0x802F
}EL_SECURITY_MODULE_COMMAND;


enum 
{
	/*EL_CMD_AUTHORIZE_CARD = 0x8030,
	EL_CMD_COMPLETE_ONLINE_TXN = 0x8031,*/

	EL_CMD_START_TRANSACTION = 0x8030,
	EL_CMD_COMPLETE_TRANSACTION = 0x8031,

	//reserve
	EL_TRANSACTION_MODULE_COMMAND_END = 0x803F
}EL_TRANSACTION_MODULE_COMMAND;


enum 
{
	EL_CMD_SET_DATA = 0x8040,
	EL_CMD_GET_DATA = 0x8041,
	//reserve
	EL_PARA_MANAGEMENT_MODULE_COMMAND_END = 0x804F
}EL_PARA_MANAGEMENT_MODULE_COMMAND;

enum 
{
	EL_CMD_FILE_DOWNLOAD = 0x8050,
	//reserve
	EL_FILE_DOWN_MODULE_COMMAND_END = 0x805F
}EL_FILE_DOWN_MODULE_COMMAND;

enum
{
	EL_CMD_SET_COMPATIBLE_MODE = 0x8060,
	EL_CMD_SET_COMPATIBLE_MODE_END = 0x806F
}EL_SET_COMPATIBEL_COMMAND;

enum
{
    EL_CMD_REMOTEDOWNLOAD_TASKINFO = 0x9141,
	EL_CMD_REMOTEDOWNLOAD_SAVEDATA = 0x9142,
	EL_CMD_REMOTEDOWNLOAD_TASKASK  = 0x9143,
	EL_CMD_REMOTEDOWNLOAD_END = 0x91FF
}EL_FILE_DOWLOAD_COMMAND;


enum
{
    EL_CMD_GET_TERM_SN = 0x9034,
	EL_CMD_GET_TERM_EXTSN = 0x9035,
	EL_CMD_GET_TERM_VER_INFO = 0x903D,
	EL_CMD_GET_TERM_INFO = 0x903E,
	EL_TMS_MOUDL_CMD_END = 0x90FF
}EL_TMS_MOUDLE_CMD;


#define EL_LOGGIC_CONNECT    0
#define EL_PHYSIC_CONNECT    1

#define EL_INTERGER_STRING_LEN           10
#define EL_INTERGER_LINE_NUM_LEN         4


typedef uchar* (*RET_TO_STRING_CB)(int iRet);



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void InitComm();
uchar ComDebugTx(char *psStr, ...);
ushort GetCommand(const uchar *pucMsgDataBuf);
int HandleRequest(const uchar *pucMsgDataBuf, uchar *pucMsgResponse, int *piResponseLen, ushort usCommand, uchar *ucSendResFlag);

int Processor(void);
int Connect(void);
int DisConnect(int iConnectType);
int CheckPhysicConnectStatus(uchar iPort);
int isFindFileDownLoadCmd(ushort usCommand);
int SetCompatiblaMode(const uchar *pucMsgDataBuf);
void CommTypeSelect();
int MainMenu();
int ReadCommParam();

void showMessage(char *message);
void CommDispString(void *pszStr, ushort nPosition);
void ComPoseInfoMsg(RET_TO_STRING_CB vFunc, int MoudleReturnCode,int iErrCode, int iLineNum);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#pragma pack()


#endif

