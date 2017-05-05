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
 * Description:protocol module

 *      
 * ============================================================================
 */


#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#define	MSG_RECV_CHAR_TIMEOUT		2 //ms 
#define	MSG_RECV_ACK_TIMEOUT		5000 //ms 

#define MSG_FRAME_RETRY 3

#define MSG_RET_OK 0
#define MSG_RET_TIMEOUT -10001
#define MSG_RET_OUTOFSTEP -10002
#define MSG_RET_HAS_SYN    -10003
#define MSG_RET_CHANNEL_FAIL    -10004
#define MSG_RET_BUFFER_OVERFLOW    -10005
#define MSG_RET_RCV_FAIL -10006
#define MSG_RET_LRC_ERR -10007
#define MSG_RET_ERROR -10008
#define MSG_RET_SYN_FAIL -10009
#define MSG_RET_RETRY_FAIL -10010
#define MSG_RET_NOT_INIT -10011



#define MSG_FRAME_LEN 980
#define MSG_PACKAGE_LEN 2048

#define MSG_FRAME_RETRY 3

#define PANO_CONFIRM  0000
#define PANO_SYN	0001
#define PANO_OUTOFSTEP 	0002


#define MSG_PROTOCOL_VER 0X01
#define MSG_STX 0X02
#define MSG_ETX_END 0X03
#define MSG_ETX_CONINUE 0X017
#define MSG_ACK 0X06
#define MSG_NAK 0X15

#define ERR_MSG_OFFSET     64



#define MSG_PANO_BASE 1000

#define MSG_RET_HAS_NEXT_FRAME 1

#define MSG_FRAMME_HEAD_TAIL_LEN 10

#define PROTOCOL_NORMAL_MODE          0
#define PROTOCOL_COMPATIBLE_MODE      1

//protocol type
#define PROTOCOL_NORMAL_COMMAND           1
#define PROTOCOL_REMOTEDOWNLOAD_COMMAND   2

#define EL_PROTOCOL_COMMANS  0x80
#define EL_PROTOCOL_DOWNLOAD  0x90
#define EL_GET_INFO_COMMANS  0x91



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * ============================================================================
 * Function:    iMsgRcv
 * Description: receive data
 * Returns:     
 * Parameter:   [in]int iMsgDataLen
 				[in]uchar*pucMsgData
 				[in]ulong ulTimeOutMs
 * ============================================================================
 */
int iMsgRcv(uint uiMsgDataBufLen, uchar *pucMsgDataBuf, ushort *usCommand, ulong ulTimeOutMs, uchar *pucHeadData);


/*
 * ============================================================================
 * Function:    iMsgInit
 * Description: init communicat port and set package and frame size
 * Returns:     
 * Parameter:   [in]int iport
 				[in]int iPaSize
 				[in]int iFrSize
 * ============================================================================
 */

int iMsgInit(int iPort,int iPaSize,int iFrSize);


int iReceiveRequest(uint uiMsgDataBufLen, uchar *pucMsgDataBuf, ushort *usCommand, ushort ulTimeOutMs);
int iSendResponse(const uchar *pucMsgResponse , int iResponseLen, int iReturnCode, ushort usCommand);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#pragma pack()


#endif

