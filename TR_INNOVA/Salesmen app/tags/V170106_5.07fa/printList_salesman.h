/****************************************************************************
NAME
    printList_salesman.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.14      - created
****************************************************************************/
#ifndef _PRINTLIST_SALESMAN_H
#define _PRINTLIST_SALESMAN_H

#include "transaction.h"
#include "data_define_salesman.h"

#define PAPER_PIXEL		384
#define LEFT_INDENT_OF_PRINT 45
#define END_JUMP	7 // Modified by Kim_LinHB 2013-3-23 v4.55 6 to 7
#define PRN_LEFT	0x00
#define PRN_MIDD	0x01
#define PRN_RIGHT	0x02

//      total 53
#define DEMI_SIZE			0	
#define DEMI_SIZE_BOLD	1
//		total 29
#define SIZE_NORMAL		2
#define SIZE_NORMAL_BOLD	3
//		total 17
#define SIZE_LARGE		4
#define SIZE_LARGE_BOLD	5

#define SIZE_MAX_POLICE_NORMALE 24
#define SIZE_MAX_POLICE_DEMI 800//42
#define SIZE_MAX_POLICE_LARGE 12

enum
{
	PRN_SALESMAN,
	PRN_MERCHANT,
	PRN_PREVIEW,
};

enum
{
	TYPE_N = 1,
	TYPE_B,
};
void vTrace(unsigned char *pucLabel, void *pucBuffer, unsigned short usBuffLen, \
			unsigned char ucLv, unsigned char type);

void Prn_Resp(RESPONSE_HEADER_TAG stResp);

void Prn_Order(TRANS_DATA stCurTran, unsigned char Mode);

void Prn_Visit(TRANS_DATA stCurTran);
void Prn_CancelLastOrder(TRANS_DATA stCurTran);

void Prn_BatchUpload(unsigned char bPreview, unsigned int totRecord, \
					 unsigned int sold, unsigned int visited);

void Prn_Consign_info(SAV_PARAM stTmpS);
void Prn_ConsignOrder(TRANS_DATA stCurTran, unsigned char bPreview);
void Prn_RetConsignOrder(TRANS_DATA stCurTran, unsigned char bPreview);

void Prn_PendingOrders(TRANS_DATA stCurTran, PENDING_ORDERS stPO);
void Prn_SalesmanInfo(void);
void Prn_SalesmanRoute(ROUTE_INFO stRoute, unsigned char bDuplicate);
void Prn_Discount(void);

void Prn_LstConsignOrderRep(SAV_PARAM stRecord);
void Prn_PhysicalStockReport(void);
void Prn_OrderReport(const unsigned char *from, const unsigned char *to, unsigned char Type, OrderReport stOR);
// Added by Kim_LinHB 2013-7-30 v4.60
void Prn_OrderReport_Prod(const unsigned char *from, const unsigned char *to, OrderReport_Prod stOR);
// Add End
void Prn_OnlineStockReport(void);

void Prn_SummaryReport(const unsigned char *from, const unsigned char *to, xSummary stSummary);

void Prn_SettDownload(unsigned char Ret, RESPONSE_HEADER_TAG stResp);

void Prn_ConnParameter(void);

void Prn_CreditTransferOrder(TRANS_DATA stCurTran, unsigned char Mode);

// Added by Kim_LinHB 2013-10-28 v4.68
void Prn_Consignment(CONSIGNMENT_LIST stOrderList, unsigned char OrderStatus, int OrderIndex, int Operation);

#endif
