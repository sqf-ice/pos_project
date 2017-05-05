/****************************************************************************
NAME
    calc_merchant.h

DESCRIPTION
	send & recv XML msg 

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.14      - created
****************************************************************************/
#ifndef _CALC_SALESMAN_H
#define _CALC_SALESMAN_H

#include "transaction.h"
#include "data_define_salesman.h"
#define STX 2
#define ETX 3

#define MSG_PARAM		"01"
#define MSG_CATPACKLSIT	"12"
#define MSG_PROVISIONID "11"
#define MSG_PHYSCSTOCK	"13"
#define MSG_SALESMANIFO	"14"
#define MSG_SALESVISIT	"16"
#define MSG_SALESMERLIST "15"
#define MSG_CASHORDER	"17"
#define MSG_CONSIGNORD	"18"
#define MSG_RETCONSORD	"19"
#define MSG_CONSIGNINQ	"20"
#define MSG_CANCEL		"22"
#define MSG_GETUSERS	"25"
#define MSG_CHANGEUSERPWD	"26" // Added by Kim_LinHB 2013-7-18 v4.60
#define MSG_PENDORDER	"32"
#define MSG_LSTCONSORD	"33"
#define MSG_SUMMATY		"34"
#define MSG_ORDERREPORT	"35"
#define MSG_CREDITTRANSFER "36"
#define MSG_CREDITORDER	"38"
#define MSG_ORDERREPORTPROD	"40"
// Added by Kim_LinHB 2013-10-28 v4.68
#define MSG_INQCONSIGNCONFIRM	"41"
#define MSG_UPDATECONSIGNCONFIRM	"42"

void GenerationdPOSKey(unsigned char *pszTermSN,unsigned char *pEncryptPOSKey);
void EncrypteCode(unsigned char *pszIn, unsigned char *pszKey, unsigned char *pszOut);
void ConvertKey(unsigned char *key);

unsigned short CalcCRC(unsigned char *pData, unsigned short usLen);
void Short2Ascii(unsigned short usNum,unsigned char *pAscii);
//////////////////////////////////////////////////////////////////////////
// 5.1
enCodeReturn Parameter_XML(RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
						   unsigned char bAutoConnected);
// 5.3
enCodeReturn Get_ProvisionId_XML(unsigned char *pwd, RESPONSE_HEADER_TAG *respH, \
								 RESPONSE_BODY_TAG *respB, unsigned char *pszProvisionId, \
								 unsigned char bAutoConnected);
// 5.2
enCodeReturn CategoryPackageList_XML(unsigned char *IsContinueDownload, unsigned int RequestIndex,\
									 RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
									 unsigned char bAutoConnected,SAV_PARAM *pstSav);
// 5.4
enCodeReturn PhysicalStock_XML(unsigned char *pwd, RESPONSE_HEADER_TAG *respH, \
							   RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.5
enCodeReturn SalesmanInfo_XML(unsigned char *pwd, RESPONSE_HEADER_TAG *respH, \
							  RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.6	Save Salesman Visit
enCodeReturn SaveSalesmanVisit_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, \
								   RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.7
enCodeReturn SalesmanMerchantList_XML(unsigned char *pwd, unsigned char *timeASCII, \
									  ROUTE_INFO *pstRoute, unsigned char *IsContinueDownload, \
									  unsigned int RequestIndex,RESPONSE_HEADER_TAG *respH, \
									  RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.8 5.9
enCodeReturn Order_XML(TRANS_DATA stData, unsigned char *szDate,unsigned char *type,
					   RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.10
enCodeReturn SalesmanConsignOrder_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, \
									  RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.11
enCodeReturn ReturnConsignOrder_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, \
									RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.12
enCodeReturn ConsignInquiry_XML(unsigned char *pwd, unsigned char *IsContinueDownload, unsigned int RequestIndex,\
								RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
								unsigned char bAutoConnected,SAV_PARAM *pstSav);

enCodeReturn PendingOrders_XML(TRANS_DATA stCur, PENDING_ORDERS *pendingOrder, \
							   RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
							   unsigned char bAutoConnected);

enCodeReturn LastConsignOrder_XML(unsigned char *pwd, SAV_PARAM *record, \
								  RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
								  unsigned char bAutoConnected);

enCodeReturn Summary_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, \
						 xSummary *Summary, RESPONSE_HEADER_TAG *respH, \
						 RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// 5.15
enCodeReturn Cancellation_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, \
							  RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);

enCodeReturn Get_User_XML(RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
						  unsigned char bAutoConnected);

enCodeReturn Change_Pwd_XML(USER_INFO ByWho, USER_INFO Who,\
							RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
							unsigned char bAutoConnected);

enCodeReturn OrderReports_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, \
							  unsigned char Type, OrderReport *pstOR, RESPONSE_HEADER_TAG *respH, \
							  RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);

// Added by Kim_LinHB 2013-7-30 v4.60
enCodeReturn OrderReports_Prod_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, \
								   OrderReport_Prod *pstOR, \
								   RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);
// Add End

enCodeReturn CreditTransferOrder_XML(TRANS_DATA stData, unsigned char *pwd, RESPONSE_HEADER_TAG *respH, \
									 RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);

// Added by Kim_LinHB 2013-10-28 v4.68
enCodeReturn Inq_For_Consig_Recv_Confirm_XML(unsigned char *pwd, unsigned char OrderStatus,\
											unsigned char OperationType, RESPONSE_HEADER_TAG *respH, \
											RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected,\
											CONSIGNMENT_LIST *pstOrderList);

enCodeReturn Update_For_Consign_Recv_Confirm_XML(CONSIGNMENT_LIST stOrderList, int OrderIndex,\
												int Operation, RESPONSE_HEADER_TAG *respH, \
												RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected);

#endif
