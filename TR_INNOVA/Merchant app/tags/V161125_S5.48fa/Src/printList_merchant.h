/****************************************************************************
NAME
    printList_merchant.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.31      - created
****************************************************************************/
#ifndef _PRINTLIST_MERCHANT_H
#define _PRINTLIST_MERCHANT_H

#include "transaction.h"
#include "data_define_merchant.h"

#define PAPER_PIXEL		384
#define LEFT_INDENT_OF_PRINT 45
#define END_JUMP	6  
#define PRN_LEFT	0x00
#define PRN_MIDD	0x01
#define PRN_RIGHT	0x02

#define SIZE_DEMI		0	
#define SIZE_DEMI_BOLD	1
#define SIZE_NORMAL		2
#define SIZE_NORMAL_BOLD	3
#define SIZE_LARGE		4
#define SIZE_LARGE_BOLD	5

#define SIZE_MAX_POLICE_NORMALE 24
#define SIZE_MAX_POLICE_DEMI 800//42
#define SIZE_MAX_POLICE_LARGE 12

enum
{
	PRN_CUSTOMER,
	PRN_MERCHANT,
	PRN_PREVIEW,
};

void vTrace(unsigned char *pucLabel, void *pucBuffer, unsigned short usBuffLen, \
			unsigned char ucLv, unsigned char type);

void Prn_Resp(RESPONSE_HEADER_TAG stResp);

unsigned char Prn_SaleCodes(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);


char Prn_SalePolling(unsigned char type);

// Added by Kim_LinHB 2013-12-4 v4.76
char Prn_RestockPin();
char Prn_PendingOrder();

void Prn_SettDownload(unsigned char Ret, RESPONSE_HEADER_TAG stResp);

void Prn_ConnParameter(void);

void Prn_PbPlace(void);

void Prn_Stock(unsigned char bDetail);

void Prn_ReportCashier(USER_INFO stCur);
void Prn_ReportCommer(void);

void Prn_ManagerSetting(void);

void Prn_CancelSale(TRANS_DATA stTran, unsigned char bPreview);

void Prn_Reclamation(TRANS_DATA stTran);

void Prn_Balance(unsigned char *pValue);

void Prn_Mail(const MAIL *pstMail, unsigned short Size);

// Added by Kim_LinHB 2013-5-22 v4.59
void Prn_CashOrder(TRANS_DATA_CASH stCurTran, unsigned char Mode);

void Prn_CancelCashOrder(TRANS_DATA_CASH stTran, unsigned char bPreview);
// Add End

#endif
