/*------------------------------------------------------------
* FileName: dataexchange.h
* Author: liukai
* Date: 2016-03-24
------------------------------------------------------------*/
#ifndef DATAEXCHANGE_H
#define DATAEXCHANGE_H

#define FILE_SYS_PARAM		"./data/sysparam.dat"
#define FILE_TRANS_INFO		"./data/transinfo.dat"
#define FILE_TRANS_TOTAL	"./data/transtotal.dat"
#define FILE_QRCODE_IMG		"./data/MerchantNum.png"

#define FILE_ERR					-1
#define FILE_ERR_INVLIDE_PARAM		-2
#define FILE_ERR_NOT_EXIST			-3
#define FILE_ERR_INVLIDE_DATA		-4
#define FILE_ERR_OPEN_FAIL			-5

//TODO wether need every parameters here
typedef struct _posInfo {
	char PoSId[36+1];			//35  TODO In specification is 35bytes, but actually it's 36bytes
	char LocationId[5+1];		//5
	char MerchantId[10+1];		//10
	char Name[15+1];			//POS NAME 15
	char PoSUnitId[15+1];		//15
	char APIkey[128];
	char MerchantKey[128];		//size TODO
	char AppStatus;
	// MobilePay server address
	char IPorURL[128];
	unsigned short usPort;
} POSINFO;

typedef struct _txnInfo {
	char OrderId[32+1];			//32 max 18 stored
	char Amount[12+1];			//18,2
	char BulkRef[18+1];			//18
	char Action[5+1];			//only "start" now
	char CustomerTokenCalc[1+1];
	char HMAC[64];				//TODO check the length
	//the following use for receive data from host
	double ReplyAmount;
	int PaymentStatus;
	char TransactionId[20+1];
	char CustomerId[32+1];
	int ReCalc;
	char CustomerToken[32+1];
	char CustomerReceiptToken[32+1];
	double Remainder;
} TXNINFO;

//added by Kevin Liu 20160616
typedef struct _txnTotals {
	int Counter;
	char TransTotal[13+1];
	char PaymentTotal[12+1];
	char RefundTotal[12+1];
} TXNTOTALS;

typedef struct _errInfo {
	int StatusCode;
	char StatusText[256];
} ERRINFO;

extern POSINFO g_tPosInfo;
extern TXNINFO g_tTxnInfo;
extern ERRINFO g_tErrInfo;

//functions
int Load_OwnData_Config(POSINFO *ptPosInfo);
int Save_OwnData_Config(POSINFO *ptPosInfo);
int Load_OwnData_TransInfo(TXNINFO *ptTxnInfo);
int Save_OwnData_TransInfo(TXNINFO *ptTxnInfo);
int Load_OwnData_TransTotals(TXNTOTALS *ptTxnTotals);
int Save_OwnData_TransTotals(TXNTOTALS *ptTxnTotals);

#endif
