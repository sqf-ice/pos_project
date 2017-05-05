/****************************************************************************
NAME
    calc_merchant.h

DESCRIPTION
	send & recv XML msg 

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.31      - created
****************************************************************************/
#ifndef _CALC_MERCHANT_H
#define _CALC_MERCHANT_H

#include "transaction.h"
#include "data_define_merchant.h"
#define STX 2
#define ETX 3

#define MSG_PARAM			"01"
#define MSG_CATPACKLSIT		"12"
#define MSG_PROVISIONID		"11"

#define MSG_MEROLEVOUCHER	"21"
#define MSG_CANCEL			"22"
#define MSG_RESTOCKPIN		"23"
#define MSG_SALASPOLLING	"24"
#define MSG_GETUSERS		"25"
#define MSG_CHANGEUSERPWD	"26"
#define MSG_CREATECASHIER	"27"
#define	MSG_DELCASHIER		"28"
#define MSG_GETBALANCE		"29"
#define MSG_CLAIM			"30"
#define MSG_GETMAIL			"31"
#define MSG_MERCASHORDER	"37"
// Added by Kim_LinHB 2013-12-4 v4.76
#define MSG_RESTOCKPIN_NEW	"43"
#define MSG_PENDINGORDER	"44"

void GenerationdPOSKey(unsigned char *pszTermSN,unsigned char *pEncryptPOSKey);
void EncrypteCode(unsigned char *pszIn, unsigned char *pszKey, unsigned char *pszOut);
void ConvertKey(unsigned char *key);

unsigned short CalcCRC(unsigned char *psData, unsigned short usLen);
void Short2Ascii(unsigned short usNum,unsigned char *pAscii);

//////////////////////////////////////////////////////////////////////////
// 5.1
enCodeReturn Parameter_XML(RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);
// 5.3
enCodeReturn Get_ProvisionId_XML(USER_INFO stCur, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char *pszProvisionId, unsigned char bAutoConnected);
// 5.2
enCodeReturn CategoryPackageList_XML(unsigned char *IsContinueDownload, unsigned int uiRequestIndex,RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected,SAV_PARAM *pstSav);

enCodeReturn Merchant_OL_Evoucher_XML(TRANS_DATA *pstCur, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

enCodeReturn ReStockPins_XML(unsigned char *pszProvisionId, unsigned char *IsContinueDownload, unsigned int *puiCount, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

// Added by Kim_LinHB 2013-12-4 v4.76
enCodeReturn ReStockPin_New_XML(unsigned char *pszProvisionId, unsigned char *IsContinueDownload, unsigned int *puiCount, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);
enCodeReturn PendingOrder_XML(RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

enCodeReturn SalesPolling_XML(unsigned char bAuto, unsigned int uiRest, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

enCodeReturn Claim_XML(unsigned char  *pszSN, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

enCodeReturn Get_Balance_XML(unsigned char *pszValue, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

enCodeReturn Get_Mail_XML(MAIL *pstMail, unsigned short *pusNums, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

// Modified by Kim_LinHB 2014-08-14 v4.95
enCodeReturn Cancellation_XML(TRANS_DATA stData, unsigned char IsAutomatically, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

enCodeReturn Get_Users_XML(RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);
enCodeReturn Change_Pwd_XML(USER_INFO stByWho, USER_INFO stWho,RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);
enCodeReturn Create_User_XML(USER_INFO stWho,RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);
enCodeReturn Del_User_XML(USER_INFO stWho,RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);

// Added by Kim_LinHB 2013-5-22 v4.59
enCodeReturn CashOrder_XML(TRANS_DATA_CASH stData, unsigned char *pszDate, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB, unsigned char bAutoConnected);
// Add End
#endif
