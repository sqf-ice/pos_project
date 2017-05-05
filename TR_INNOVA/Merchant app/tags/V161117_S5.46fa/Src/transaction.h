/****************************************************************************
NAME
    transaction.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.31      - created
****************************************************************************/
#ifndef _TRANSACTION_H
#define _TRANSACTION_H

enum
{
	en_US,
	tr_TR,
};

enum
{
	PRODUCT_ALL = 0,
	PRODUCT_PHYSICAL,
	PRODUCT_ELECTRONIC,
};

extern const unsigned char c_LANG[][5 + 1];

typedef struct _tagRequestHeaderTag
{
	//int MsgType;
	unsigned char szMsgType[2 + 1];
	unsigned char szSerialNumber[20 + 1];
	unsigned char szUserID[8 + 1];
	unsigned char szPWD[8 + 1];
	//int ParamVer;
	unsigned char szParamVer[10 + 1];
	//int BatchNo
	unsigned char szBatchNo[10 + 1];
	//int TraceNo
	unsigned char szTraceNo[10 + 1];
	unsigned char szTermDateTime[19 + 1];
	unsigned char szTermId[10 + 1];
	unsigned char szLangCode[5 + 1];
}REQUEST_HEADER_TAG;
typedef struct _tagRequestBodyTag
{
	unsigned char szAppVer[8 + 1];

	//unsigned char szDepositTypeID[?? + 1];
	unsigned char szDepositTypeID[10 + 1];
	//int MsgType;
	unsigned char szMsgType[10 +1 ];

	unsigned char szModel[8 + 1];
	unsigned char szOSVer[8 + 1];

	//long ProvisionID;
	unsigned char szProvisionID[20 + 1];
	//unsigned char szTotalAmount[?? + 1];
	unsigned char szTotalAmount[10+1];
	//int TotalCount;
	unsigned char szTotalCount[10+1];
	//unsigned char ucTransDateTime[?? + 1];
	unsigned char szTransDateTime[19 + 1];
	//int UtilityCompanyCode;
	unsigned char szUtilityCompanyCode[10+1];
}REQUEST_BODY_TAG;


typedef struct _tagResponsetHeaderTag
{
	unsigned char szResponseCode[4 + 1];
	unsigned char szStatus[1 + 1];
	unsigned char szMessage[255 + 1];
	unsigned char szHoseDateTime[19 + 1];
}RESPONSE_HEADER_TAG;
typedef struct _tagResponseBodyTag
{
	//long Amount;
	unsigned char szAmount[10 + 1];
	unsigned char szPkgDescription[50+1];
	//int PkgID
	unsigned char szPkgID[10 + 1];
	unsigned char szParamChanged[1 + 1];
	//int ParamVer;
	unsigned char szParamVer[10 + 1];
	//int BatchNo;
	unsigned char szBatchNo[10 + 1];
	//int MaxReqVoucherCnt;
	unsigned char szMaxReqVoucherCnt[10 + 1];
	//int MaxNotifyVoucherCnt;
	unsigned char szMaxNotifyVoucherCnt[10 + 1];

}RESPONSE_BODY_TAG;

typedef struct _tagXReponseECC//¶©µ¥ÇëÇóµÄresponse
{
	unsigned char	ucOrder;
	unsigned char	ucCodeResponse;
	unsigned char	szNumOrder[4];
	unsigned char	szSalesmanName[24+1];
	unsigned char	szCommercantName[24+1];
}XReponseECC;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void DoTrans(unsigned int uiType);
void Do_Error(unsigned char Ret, RESPONSE_HEADER_TAG stRespH);
unsigned short FindCodesCnt(unsigned char type, const unsigned char *ProductId);
void Erase_Report(unsigned char bAuto);

unsigned char IsSalesPollingEmpty(unsigned int *puiSold, unsigned int *puiExpiry, unsigned int *puiBurned, unsigned int *puiSelling);

void SortStock();

#endif
