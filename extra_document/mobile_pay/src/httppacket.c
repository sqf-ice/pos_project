/*------------------------------------------------------------
* FileName: HttpPacket.c
* Author: liukai
* Date: 2016-02-22
------------------------------------------------------------*/

#include "HttpPacket.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "jansson.h"
#include "jansson_config.h"
#include <string.h>
#include <osal.h>
#include "global.h"
#include <time.h>

static REQUESTCMD sg_eRequestCmd;
static PACKELEMENT *sg_ptPackElement;
static PACKELEMENT *sg_ptUnPackElement;

/* internal functions */
static int Init_Request(REQUESTCMD ePackCmd);
static int Calculate_PayStart_HMAC(void);

//administration packet elements
//DATATYPE_STRING,
//DATATYPE_INTEGER,
//DATATYPE_NUMBER
//***************************************************************
const PACKELEMENT request_registerPosElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"Name", 				DATATYPE_STRING,		g_tPosInfo.Name},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_updateRegisteredPosNameElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"Name",				DATATYPE_STRING,		g_tPosInfo.Name},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_unregisterPosElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_assignPosUnitIdPosElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"PoSUnitId", 			DATATYPE_STRING,		g_tPosInfo.PoSUnitId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_unAssignPosUnitIdToPosElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"PoSUnitId", 			DATATYPE_STRING,		g_tPosInfo.PoSUnitId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_readPosAssignPosUnitIdElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_readPosUnitAssignedPosIdElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSUnitId", 			DATATYPE_STRING,		g_tPosInfo.PoSUnitId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_getUniquePosIdElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"", DATATYPE_NONE, NULL}
};

//payment packet elements
const PACKELEMENT request_paymentStartElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"OrderId", 			DATATYPE_STRING,		g_tTxnInfo.OrderId},
		{"Amount", 				DATATYPE_STRING,		g_tTxnInfo.Amount},
		{"BulkRef", 			DATATYPE_STRING,		g_tTxnInfo.BulkRef},
		{"Action", 				DATATYPE_STRING,		g_tTxnInfo.Action},
		{"CustomerTokenCalc", 	DATATYPE_STRING,		g_tTxnInfo.CustomerTokenCalc},
		{"HMAC", 				DATATYPE_STRING,		g_tTxnInfo.HMAC},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_paymentGetStatusElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"OrderId", 			DATATYPE_STRING,		g_tTxnInfo.OrderId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_paymentCancelElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT request_paymentRefundElements[] = {
		{"MerchantId", 			DATATYPE_STRING,		g_tPosInfo.MerchantId},
		{"LocationId", 			DATATYPE_STRING,		g_tPosInfo.LocationId},
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"OrderId", 			DATATYPE_STRING,		g_tTxnInfo.OrderId},
		{"Amount", 				DATATYPE_STRING,		g_tTxnInfo.Amount},
		{"BulkRef", 			DATATYPE_STRING,		g_tTxnInfo.BulkRef},
		{"", DATATYPE_NONE, NULL}
};

//reply elements
//*******************************************************************
const PACKELEMENT reply_registerPosElements[] = {
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_updateRegisteredPosNameElements[] = {
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_unregisterPosElements[] = {
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_assignPosUnitIdPosElements[] = {
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_unAssignPosUnitIdToPosElements[] = {
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_readPosAssignPosUnitIdElements[] = {
		{"PoSUnitId", 			DATATYPE_STRING,		g_tPosInfo.PoSUnitId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_readPosUnitAssignedPosIdElements[] = {
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_getUniquePosIdElements[] = {
		{"PoSId", 				DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"", DATATYPE_NONE, NULL}
};

//payment packet elements
const PACKELEMENT reply_paymentStartElements[] = {
		{"ReCalc", 					DATATYPE_INTEGER,		&g_tTxnInfo.ReCalc},
		{"CustomerToken", 			DATATYPE_STRING,		g_tTxnInfo.CustomerToken},
		{"CustomerReceiptToken", 	DATATYPE_STRING,		g_tTxnInfo.CustomerReceiptToken},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_paymentGetStatusElements[] = {
		{"PoSId", 					DATATYPE_STRING,		g_tPosInfo.PoSId},
		{"PaymentStatus", 			DATATYPE_INTEGER,		&g_tTxnInfo.PaymentStatus},
		{"OrderId", 				DATATYPE_STRING,		g_tTxnInfo.OrderId},
		{"TransactionId", 			DATATYPE_STRING,		g_tTxnInfo.TransactionId},
		{"Amount", 					DATATYPE_NUMBER,		&g_tTxnInfo.ReplyAmount},
		{"CustomerId", 				DATATYPE_STRING,		g_tTxnInfo.CustomerId},
		{"CustomerToken", 			DATATYPE_STRING,		g_tTxnInfo.CustomerToken},
		{"CustomerReceiptToken", 	DATATYPE_STRING,		g_tTxnInfo.CustomerReceiptToken},
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_paymentCancelElements[] = {
		{"", DATATYPE_NONE, NULL}
};
const PACKELEMENT reply_paymentRefundElements[] = {
		{"Remainder", 				DATATYPE_NUMBER,		&g_tTxnInfo.Remainder},
		{"TransactionId", 			DATATYPE_STRING,		g_tTxnInfo.TransactionId},
		{"", DATATYPE_NONE, NULL}
};
//************************************************************************

int Get_Request_Type(void)
{
	return sg_eRequestCmd;
}

PACKELEMENT *Get_Pack_Element(void)
{
	return sg_ptPackElement;
}
PACKELEMENT *Get_Unpack_Element(void)
{
	return sg_ptUnPackElement;
}

static int Init_Request(REQUESTCMD ePackCmd)
{
	sg_eRequestCmd = ePackCmd;

	switch(ePackCmd) {
	case CMD_ADMIN_REGISTER_POS:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_REGISTER_POS Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_registerPosElements;
		sg_ptUnPackElement = reply_registerPosElements;
		break;
	case CMD_ADMIN_UPDATE_REGISTERED_POS_NAME:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_UPDATE_REGISTERED_POS_NAME Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_updateRegisteredPosNameElements;
		sg_ptUnPackElement = reply_updateRegisteredPosNameElements;
		break;
	case CMD_ADMIN_UNREGISTER_POS:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_UNREGISTER_POS Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_unregisterPosElements;
		sg_ptUnPackElement = reply_unregisterPosElements;
		break;
	case CMD_ADMIN_ASSIGN_POS_UNITID_TO_POS:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_ASSIGN_POS_UNITID_TO_POS Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_assignPosUnitIdPosElements;
		sg_ptUnPackElement = reply_assignPosUnitIdPosElements;
		break;
	case CMD_ADMIN_UNASSIGN_POS_UNITID_TO_POS:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_UNASSIGN_POS_UNITID_TO_POS Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_unAssignPosUnitIdToPosElements;
		sg_ptUnPackElement = reply_unAssignPosUnitIdToPosElements;
		break;
	case CMD_ADMIN_READ_POS_ASSIGNED_POS_UNITID:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_READ_POS_ASSIGNED_POS_UNITID Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_readPosAssignPosUnitIdElements;
		sg_ptUnPackElement = reply_readPosAssignPosUnitIdElements;
		break;
	case CMD_ADMIN_READ_POS_UNIT_ASSIGNED_POS_ID:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_READ_POS_UNIT_ASSIGNED_POS_ID Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_readPosUnitAssignedPosIdElements;
		sg_ptUnPackElement = reply_readPosUnitAssignedPosIdElements;
		break;
	case CMD_ADMIN_GET_UNIQUE_POS_ID:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_ADMIN_GET_UNIQUE_POS_ID Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_getUniquePosIdElements;
		sg_ptUnPackElement = reply_getUniquePosIdElements;
		break;

	case CMD_PAYMENT_START:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_PAYMENT_START Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_paymentStartElements;
		sg_ptUnPackElement = reply_paymentStartElements;
		break;
	case CMD_PAYMENT_GET_STATUS:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_PAYMENT_GET_STATUS Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_paymentGetStatusElements;
		sg_ptUnPackElement = reply_paymentGetStatusElements;
		break;
	case CMD_PAYMENT_CANCEL:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_PAYMENT_CANCEL Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_paymentCancelElements;
		sg_ptUnPackElement = reply_paymentCancelElements;
		break;
	case CMD_PAYMENT_REFUND:
		Pax_Log(LOG_INFO, "\n\n%s - %d CMD_PAYMENT_REFUND Request", __FUNCTION__, __LINE__);
		sg_ptPackElement = request_paymentRefundElements;
		sg_ptUnPackElement = reply_paymentRefundElements;
		break;
	default:
		return MP_ERR;
	}

	return MP_OK;
}

static int Base64_Encode(const char *szStr, int iStrLen, char *pszEncode, int iEncodeLen)
{
	int iLen=0;
    BIO *bmem=NULL,*b64=NULL;
    BUF_MEM *bptr=NULL;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, szStr, iStrLen); //encode
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    if( bptr->length > iEncodeLen ) {
        Pax_Log( LOG_INFO, "encode_len too small\n" );
        return HTTPPACKET_ERR;
    }
    memcpy(pszEncode, bptr->data, bptr->length);
    iLen = bptr->length;
    pszEncode[bptr->length] = '\0';
    BIO_free_all( b64 );

    return iLen;
}

/*
 * the HMAC_SHA256 transform looks like:
 * SHA256(K XOR opad, SHA256(K XOR ipad, text))
 * where K is an n byte key
 * ipad is the byte 0x36 repeated 64 times
 * opad is the byte 0x5c repeated 64 times
 * and text is the data being protected
 */
static int Calculate_HMAC(const unsigned char *pucContent, int iContentLen,
		const unsigned char *pucapiKey, int iApiKeyLen, char *pszEncodeData, int iBufSize)
{
	int i=0, iLen=0;
	unsigned char key_inner[65]={0};
	unsigned char key_outer[65]={0};
	unsigned char temp[SHA256_DIGEST_LENGTH]={0};
	unsigned char encode_inner[SHA256_DIGEST_LENGTH]={0};
	unsigned char encode_outer[SHA256_DIGEST_LENGTH]={0};
	unsigned char bufferIn[1024]={0};
	unsigned char bufferOut[1024]={0};

	/* if key is longer than 64 bytes reset it to key=sha256(key) */
	if ( iApiKeyLen > 64 ) {
		SHA256( pucapiKey, iApiKeyLen, temp );
		pucapiKey = temp;
		iApiKeyLen = SHA256_DIGEST_LENGTH;
	}
	/* start out by storing key in pads */
	memset(key_inner, 0, sizeof(key_inner));
	memset(key_outer, 0, sizeof(key_outer));
	memcpy(key_inner, pucapiKey, iApiKeyLen);
	memcpy(key_outer, pucapiKey, iApiKeyLen);
	/* XOR key with ipad and opad values */
	for ( i = 0; i < sizeof(key_inner)-1; i++ ) {
		key_inner[i] ^= 0x36;
		key_outer[i] ^= 0x5c;
	}
	/* perform inner SHA256*/
	memset(bufferIn, 0x00, sizeof(bufferIn));
	memcpy(bufferIn, key_inner, 64);
	memcpy(bufferIn + 64, pucContent, iContentLen);
	SHA256(bufferIn, 64 + iContentLen, encode_inner);
	/* perform outer SHA256*/
	memset(bufferOut, 0x00, sizeof(bufferOut));
	memcpy(bufferOut, key_outer, 64);
	memcpy(bufferOut + 64, encode_inner, SHA256_DIGEST_LENGTH);
	SHA256(bufferOut, 64 + SHA256_DIGEST_LENGTH, encode_outer);
	iLen = sizeof(encode_outer);
	if(iLen >= iBufSize) {
		return MP_ERR;
	}
	memcpy(pszEncodeData, encode_outer, iLen);

	return iLen;
}

static int Get_Time_Stamp_UTC_String(char *pszTimeStamp, int iBufSize)
{
	int iLen=0;
	time_t timep;
	struct tm *p=NULL;

	if ( (pszTimeStamp == NULL) || (iBufSize < 0) ) {
		return MP_ERR_INVALID_PARAM;
	}
	time(&timep);
	p=localtime(&timep);
	timep = mktime(p);
	iLen = snprintf((char *)pszTimeStamp, iBufSize, "%ld", timep);
	if ( (iLen < 0) || (iLen >= iBufSize) ) {
		Pax_Log(LOG_INFO, "%s - %d, snprintf Failed!", __FUNCTION__, __LINE__);
		return MP_ERR;
	}

	return iLen;
}

static int Get_Request_Url(char *szUrl, int iBufSize)
{
	int iLen=0;
	char szCmd[24]={0};
	REQUESTCMD eRequestCmd;

	if ( (szUrl == NULL) || (iBufSize < 0) ) {
		return MP_ERR_INVALID_PARAM;
	}
	memset(szCmd, 0, sizeof(szCmd));
	eRequestCmd = Get_Request_Type();
	switch(eRequestCmd) {
	case CMD_ADMIN_REGISTER_POS:
		strcat(szCmd, "RegisterPoS");
		break;
	case CMD_ADMIN_UPDATE_REGISTERED_POS_NAME:
		strcat(szCmd, "UpdateRegisteredPoSName");
		break;
	case CMD_ADMIN_UNREGISTER_POS:
		strcat(szCmd, "UnRegisterPoS");
		break;
	case CMD_ADMIN_ASSIGN_POS_UNITID_TO_POS:
		strcat(szCmd, "AssignPoSUnitIdToPos");
		break;
	case CMD_ADMIN_UNASSIGN_POS_UNITID_TO_POS:
		strcat(szCmd, "UnAssignPoSUnitIdToPoS");
		break;
	case CMD_ADMIN_READ_POS_ASSIGNED_POS_UNITID:
		strcat(szCmd, "ReadPoSAssignPoSUnitId");
		break;
	case CMD_ADMIN_READ_POS_UNIT_ASSIGNED_POS_ID:
		strcat(szCmd, "ReadPoSUnitAssignedPoSId");
		break;
	case CMD_ADMIN_GET_UNIQUE_POS_ID:
		strcat(szCmd, "GetUniquePoSId");
		break;


	case CMD_PAYMENT_START:
		strcat(szCmd, "PaymentStart");
		break;
	case CMD_PAYMENT_GET_STATUS:
		strcat(szCmd, "GetPaymentStatus");
		break;
	case CMD_PAYMENT_CANCEL:
		strcat(szCmd, "PaymentCancel");
		break;
	case CMD_PAYMENT_REFUND:
		strcat(szCmd, "PaymentRefund");
		break;
	default:
		return MP_ERR;
	}
	iLen = snprintf(szUrl, iBufSize, "%s%s", g_tPosInfo.IPorURL, szCmd);
	if ( (iLen < 0) || (iLen >= iBufSize) ) {
		Pax_Log(LOG_INFO, "%s - %d, snprintf Failed!", __FUNCTION__, __LINE__);
		return MP_ERR;
	}

	return iLen;
}

static int Get_Request_Json(char *pszJsonData, int iBufSize)
{
	int i=0, iLen=0;
	json_t *pObjectmsg=NULL;
	char *pcResult=NULL;
	PACKELEMENT *ptPackElement=NULL;

	if ( (pszJsonData == NULL) || (iBufSize < 0) ) {
		return MP_ERR_INVALID_PARAM;
	}
	pObjectmsg = json_object();
	ptPackElement = Get_Pack_Element();
	for ( i=0; ptPackElement[i].data != NULL; i++ ) {
		json_object_set_new (pObjectmsg, ptPackElement[i].element, json_string(ptPackElement[i].data));
	}
	pcResult = json_dumps(pObjectmsg, JSON_PRESERVE_ORDER);
	//option 1: replace the space before "value"	TODO
//	if( strrpl(pcResult, pszJsonData, iBufSize, " \"", "\"") == NULL ) {
//		free(pcResult);
//		return MP_ERR;
//	}

	//option 2: no need to replace, it works too
	iLen = strlen(pcResult);
	if( iLen >= iBufSize ) {
		free(pcResult);
		return MP_ERR;
	}
	memcpy(pszJsonData, pcResult, iLen);

	free(pcResult);
	json_decref(pObjectmsg);

	return iLen;
}

static int Get_Request_AuthHeader(const char *pszUrl, const char *pszJsonData,
		const char *pszTimeStamp, char *pszSendBuf, int iBufSize)
{
	int iLen=0;
	char szTmp1[1024]={0}, szTmp2[1024]={0};	//TODO size
//	POSINFO posInfo;

//	Get_Pos_Info(&posInfo);
	if ( (pszUrl == NULL) || (pszJsonData == NULL) || (pszTimeStamp == NULL) || (pszSendBuf == NULL) || (iBufSize < 0) ) {
		Pax_Log(LOG_INFO, "%s - %d, Get Request Infomations Failed!", __FUNCTION__, __LINE__);
		return MP_ERR_INVALID_PARAM;
	}
	iLen = snprintf(szTmp1, sizeof(szTmp1), "%s %s %s", pszUrl, pszJsonData, pszTimeStamp);
	if ( (iLen < 0) || (iLen >= iBufSize) ) {
		Pax_Log(LOG_INFO, "%s - %d, Catenate Request Failed!", __FUNCTION__, __LINE__);
		return MP_ERR;
	}
	iLen = Calculate_HMAC(szTmp1, iLen, g_tPosInfo.APIkey, strlen(g_tPosInfo.APIkey), szTmp2, sizeof(szTmp2));
	if ( iLen < 0 ) {
		Pax_Log(LOG_INFO, "%s - %d, Calculate_HMAC failed", __FUNCTION__, __LINE__);
		return MP_ERR;
	}

	memset(szTmp1, 0, sizeof(szTmp1));
	iLen = Base64_Encode(szTmp2, iLen, szTmp1, sizeof(szTmp1));
	if ( iLen < 0 ) {
		Pax_Log(LOG_INFO, "%s - %d, Base64_encode failed", __FUNCTION__, __LINE__);
		return MP_ERR;
	}
	//BASE64 will always add a '\n' at the end of string.
	DeleChar(szTmp1, '\n');

	iLen = snprintf(pszSendBuf, iBufSize, "%s %s %s", "Authorization:", szTmp1, pszTimeStamp);
	if ( (iLen < 0) || (iLen >= iBufSize) ) {
		Pax_Log(LOG_INFO, "%s - %d, Catenate Request Failed!", __FUNCTION__, __LINE__);
		return MP_ERR;
	}
	return iLen;
}

static int Get_Response_Json(void *data, size_t size, size_t nmemb, void *buffer_in)
{
	//TODO size control
    if ( buffer_in != NULL ) {
    	memcpy(buffer_in, data, size * nmemb);
    	Pax_Log(LOG_INFO, "%s - %d Get_Response_Json buffer_in = %s", __FUNCTION__, __LINE__, buffer_in);
    	return size * nmemb;
    }
    return MP_OK;
}

static int Send_Recv_Process(char *pszUrl, char *pszAuthHeader, char *pszJsonData, long *lRetcode, char *pszRetJsonData)
{
	CURL *pCurl=NULL;
	CURLcode eRes;
	struct curl_slist *ptHeaders=NULL;
	CURLM *pMultiHandle = NULL;				//multi fix cancel delay problem when getStatus by Kevin Liu 20160606
	int iStillRunning;						//multi
	struct CURLMsg *ptM=NULL;					//multi
	int iMsgq = 0;							//multi
	CURLMcode eCurlmCode;					//multi
	CURL *pCurlFromMulti=NULL;				//multi
	int iRc = -1;							//multi
	fd_set tFdread;							//multi
	fd_set tFdwrite;							//multi
	fd_set tFdexcep;							//multi
	int iMaxfd = -1;							//multi
	struct timeval tv;						//multi
	int iRet=0;

	if ( (pszUrl == NULL) || (pszAuthHeader == NULL) || (pszJsonData == NULL) ) {
		return MP_ERR_INVALID_PARAM;
	}
	curl_global_init(CURL_GLOBAL_ALL);

	pMultiHandle = curl_multi_init();		//multi

	pCurl = curl_easy_init();
	if ( pCurl == NULL ) {
		curl_multi_cleanup(pMultiHandle);
		curl_global_cleanup();
		return MP_ERR;
	}

	Pax_Log(LOG_INFO, "%s - %d pszUrl = %s", __FUNCTION__, __LINE__, pszUrl);
	Pax_Log(LOG_INFO, "%s - %d pszAuthHeader = %s", __FUNCTION__, __LINE__, pszAuthHeader);
	Pax_Log(LOG_INFO, "%s - %d pszJsonData = %s", __FUNCTION__, __LINE__, pszJsonData);

	curl_easy_setopt(pCurl, CURLOPT_URL, pszUrl);
	ptHeaders = curl_slist_append(ptHeaders, "Content-Type: application/json");
	ptHeaders = curl_slist_append(ptHeaders, pszAuthHeader);
	curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, ptHeaders);
	curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pszJsonData);
	curl_easy_setopt(pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1);	//debug
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, Get_Response_Json);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pszRetJsonData);
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10);
	//for https
	if ( strncmp(pszUrl, "https", 5) == 0 ) {
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 2L); //modified by Kevin Liu 20160707, from 1L to 2L
	}

	curl_multi_add_handle(pMultiHandle, pCurl);			//multi

	eCurlmCode = CURLM_CALL_MULTI_PERFORM;				//multi

	while(CURLM_CALL_MULTI_PERFORM == eCurlmCode) {		//multi
		eCurlmCode = curl_multi_perform(pMultiHandle, &iStillRunning);
	}
	if (eCurlmCode != CURLM_OK) {						//multi
		curl_easy_cleanup(pCurl);
		curl_multi_cleanup(pMultiHandle);
		curl_global_cleanup();
		return eCurlmCode;
	}

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	while (iStillRunning > 0) {
		FD_ZERO(&tFdread);
		FD_ZERO(&tFdwrite);
		FD_ZERO(&tFdexcep);
		curl_multi_fdset(pMultiHandle, &tFdread, &tFdwrite, &tFdexcep, &iMaxfd);
		iRc = select(iMaxfd + 1, &tFdread, &tFdwrite, &tFdexcep, &tv);
		if ( -1 == iRc ) {
			curl_easy_cleanup(pCurl);
			curl_multi_cleanup(pMultiHandle);
			curl_global_cleanup();
			return MP_ERR;
		} else {
			while (1) {
				eCurlmCode = curl_multi_perform(pMultiHandle, &iStillRunning);
				if ( eCurlmCode != CURLM_CALL_MULTI_PERFORM ) {
					break;
				}
			}
		}
		if ( XuiHasKey() ) {
			if ( XuiGetKey() == KEYCANCEL ) {
				if ( CMD_PAYMENT_GET_STATUS == Get_Request_Type()) {
					curl_easy_cleanup(pCurl);
					curl_multi_cleanup(pMultiHandle);
					curl_global_cleanup();
					return MP_ERR_USERCANCEL;
				}
			}
		}
	}
	ptM = curl_multi_info_read(pMultiHandle, &iMsgq);
	if ( ptM && (ptM->data.result != CURLE_OK) ) {
		curl_easy_cleanup(pCurl);
		curl_multi_cleanup(pMultiHandle);
		curl_global_cleanup();
		return ptM->data.result;
	}
	if ( ptM && (ptM->msg == CURLMSG_DONE) ) {
		pCurlFromMulti = ptM->easy_handle;
	} else {
		curl_easy_cleanup(pCurl);
		curl_multi_cleanup(pMultiHandle);
		curl_global_cleanup();
		return MP_ERR;
	}

	eRes = curl_easy_getinfo(pCurlFromMulti, CURLINFO_RESPONSE_CODE , lRetcode);
	if ( eRes != CURLE_OK ) {
		curl_easy_cleanup(pCurl);
		curl_multi_cleanup(pMultiHandle);
		curl_global_cleanup();
		return eRes;
	}
	curl_easy_cleanup(pCurl);
	curl_multi_cleanup(pMultiHandle);
	curl_global_cleanup();

	return MP_OK;
}

static int Result_Process(long lRetcode, char *pszRetJsonData)
{
	int i=0;
	void *pIter=NULL;
	const char *pszKey=NULL, *pszValueChar=NULL;
	json_t *ptRoot=NULL, *ptValue=NULL;
	json_error_t error;
	PACKELEMENT *ptUnpackElement=NULL;
	char *pszValueString=NULL;
	int *piValueInteger=NULL;
	double *pdValueDouble=NULL;

	if ( pszRetJsonData == NULL ) {
		Pax_Log(LOG_INFO, "%s - %d", __FUNCTION__, __LINE__);
		return MP_ERR_INVALID_PARAM;
	}
	if ( (lRetcode == HTTP_OK) || (lRetcode == HTTP_ERR) ) {
		ptRoot = json_loads(pszRetJsonData, 0, &error);
		if ( ptRoot == NULL ) {
			Pax_Log(LOG_INFO, "%s - %d", __FUNCTION__, __LINE__);
			return MP_ERR;
		}
		pIter = json_object_iter(ptRoot);
		while(pIter)
		{
			//TODO optimize the data save method
			pszKey = json_object_iter_key(pIter);
			ptValue = json_object_iter_value(pIter);
			Pax_Log(LOG_INFO, "%s - %d, Key: %s, Value: %s", __FUNCTION__, __LINE__, pszKey, json_string_value(ptValue));
			if ( lRetcode == HTTP_OK ) {
				ptUnpackElement = Get_Unpack_Element();
				for ( i=0; ptUnpackElement[i].data != NULL; i++ ) {
					if ( 0 == strcmp(pszKey, ptUnpackElement[i].element) ) {
						if ( ptUnpackElement[i].dataType == DATATYPE_STRING ) {
							pszValueChar = json_string_value(ptValue);
							if ( pszValueChar != NULL ) {
								pszValueString = (char *)ptUnpackElement[i].data;
								memcpy(pszValueString, pszValueChar, strlen(pszValueChar));
								pszValueString[strlen(pszValueChar)] = '\0';
								Pax_Log(LOG_INFO, "element = %s string = %s", pszKey, pszValueString);
							}
						} else if ( ptUnpackElement[i].dataType == DATATYPE_INTEGER ) {
							if ( ptValue != NULL ) {
								piValueInteger = (int *)ptUnpackElement[i].data;
								*piValueInteger = json_integer_value(ptValue);
								Pax_Log(LOG_INFO, "element = %s int = %d", pszKey, *piValueInteger);
								Pax_Log(LOG_INFO, "element = %s paymentStatus = %d", pszKey, g_tTxnInfo.PaymentStatus);
							}
						} else if ( ptUnpackElement[i].dataType == DATATYPE_NUMBER ) {
							if ( ptValue != NULL ) {
								pdValueDouble = (double *)ptUnpackElement[i].data;
								*pdValueDouble = json_number_value(ptValue);
								Pax_Log(LOG_INFO, "element = %s double = %02f", pszKey, *pdValueDouble);
								Pax_Log(LOG_INFO, "element = %s replyAmount = %02f", pszKey, g_tTxnInfo.ReplyAmount);
							}
						} else {

						}
					}
				}
			}
			else {	//http status code = HTTP_ERR
				//"StatusCode":10,
				//"StatusText":"Missing or invalid parameters"
				if ( 0 == strcmp(pszKey, "StatusCode") ) {
					if ( ptValue != NULL ) {
						g_tErrInfo.StatusCode = json_number_value(ptValue);
					}
				}
				else if ( 0 == strcmp(pszKey, "StatusText") ) {
					pszValueChar = json_string_value(ptValue);
					if ( pszValueChar != NULL ) {
						memset(g_tErrInfo.StatusText, 0 ,sizeof(g_tErrInfo.StatusText));
						memcpy(g_tErrInfo.StatusText, pszValueChar, strlen(pszValueChar));
					}
				}
			}
			pIter = json_object_iter_next(ptRoot, pIter);
		}
		json_decref(ptRoot);
	}
	else if ( lRetcode == HTTP_AUTH_ERR ) {
		Pax_Log(LOG_INFO, "%s - %d Http code = 401! Response\n\n", __FUNCTION__, __LINE__);
	}
	else {
		Pax_Log(LOG_INFO, "%s - %d, lRetcode = %d Response\n\n", __FUNCTION__, __LINE__, lRetcode);
		return MP_ERR;
	}

	return MP_OK;
}

int Request_Process(REQUESTCMD ePackCmd)
{
	int iRet=0, iLen=0, iUrlLen=0, iJsonDataLen=0, iTimeStampLen=0;
	long lRetcode=0;
	char szUrl[128]={0}, szJsonData[512]={0}, szTimeStamp[24]={0}, szAuthHeader[1024]={0}, szRetJsonData[1024]={0};

	// 1.Init the request
	Init_Request(ePackCmd);

	if ( ePackCmd == CMD_PAYMENT_START ) {
		Calculate_PayStart_HMAC();
	}

	// 2.generate the request informations
	iUrlLen = Get_Request_Url(szUrl, sizeof(szUrl));
	iJsonDataLen = Get_Request_Json(szJsonData, sizeof(szJsonData));
	iTimeStampLen = Get_Time_Stamp_UTC_String(szTimeStamp, sizeof(szTimeStamp));
	if ( (iUrlLen < 0) || (iJsonDataLen < 0) || (iTimeStampLen < 0) ) {
		Pax_Log(LOG_INFO, "%s - %d, Get_Request Info failed", __FUNCTION__, __LINE__);
		return MP_ERR;
	}
	iLen = Get_Request_AuthHeader(szUrl, szJsonData, szTimeStamp, szAuthHeader, sizeof(szAuthHeader));
	if ( iLen < 0 ) {
		Pax_Log(LOG_INFO, "%s - %d, Get_Request_AuthHeader failed", __FUNCTION__, __LINE__);
		return MP_ERR;
	}

	// 3.send and receive packets
	iRet = Send_Recv_Process(szUrl, szAuthHeader, szJsonData, &lRetcode, szRetJsonData);
	if (iRet) {
		Pax_Log(LOG_INFO, "%s - %d Send_Recv_Process failed iRet = %d", __FUNCTION__, __LINE__, iRet);
		if ( iRet == CURLE_OPERATION_TIMEDOUT ) {
			return MP_ERR_TIMEOUT;
		} else if ( iRet == CURLE_COULDNT_RESOLVE_HOST ) {
			return MP_ERR_NETWORK;
		} else if ( iRet == MP_ERR_USERCANCEL ) {
			return MP_ERR_USERCANCEL;
		}
		return MP_ERR;
	}
	// 4.get the response code and parse the response Json data
	iRet = Result_Process(lRetcode, szRetJsonData);
	if (iRet) {
		Pax_Log(LOG_INFO, "%s - %d Result_Process failed", __FUNCTION__, __LINE__);
		return MP_ERR;
	}

	return lRetcode;
}

static int Calculate_PayStart_HMAC(void)
{
	int iLen=0;
//	POSINFO posInfo;
//	TXNINFO txnInfo;
	char szPlainText[128]={0}, szHmacTmp[128]={0};

//	Get_Pos_Info(&posInfo);
//	Get_Txn_Info(&txnInfo);
	iLen = snprintf(szPlainText, sizeof(szPlainText), "%s%s#%s#%s#%s#%s#",
			g_tPosInfo.MerchantId, g_tPosInfo.LocationId, g_tPosInfo.PoSId, g_tTxnInfo.OrderId, g_tTxnInfo.Amount, g_tTxnInfo.BulkRef);
	if ( (iLen < 0) || (iLen >= sizeof(szPlainText)) ) {
		Pax_Log(LOG_ERROR, "%s - %d, Catenate szPlainText Failed!", __FUNCTION__, __LINE__);
		return MP_ERR;
	}
	iLen = Calculate_HMAC(szPlainText, iLen, g_tPosInfo.MerchantKey, strlen(g_tPosInfo.MerchantKey), szHmacTmp, sizeof(szHmacTmp));
	if ( iLen < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d, Calculate_HMAC failed", __FUNCTION__, __LINE__);
		return MP_ERR;
	}
	iLen = Base64_Encode(szHmacTmp, iLen, g_tTxnInfo.HMAC, sizeof(g_tTxnInfo.HMAC));
	if ( iLen < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d, Calculate_HMAC failed", __FUNCTION__, __LINE__);
		return MP_ERR;
	}
	DeleChar(g_tTxnInfo.HMAC, '\n');

//	Set_Pos_Info(posInfo);
//	Set_Txn_Info(txnInfo);
	return MP_OK;
}





