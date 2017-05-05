#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "transaction.h"
#include "connect.h"
#include "framework.h"
#include "calc_merchant.h"
#include "printList_merchant.h"

#include "LHBXML.h"

#include <ctype.h>

static void Do_SaleCodes(unsigned char Mode);
static void Do_Duplicate(void);
static void Do_RestorePins(unsigned char bAuto);

static unsigned char Do_On_Service(void);
static void Do_Off_Service(void);
static void Do_OnOff_Service(void);

// Modified by Kim_LinHB 2013-12-4 v4.76
static void Do_RestorePin_menu(int sub);
// Added by Kim_LinHB 2013-12-4 v4.76
static void Do_RestorePin_New(unsigned char bAuto);
// Modified by Kim_LinHB 2013-12-16 v4.77 added bAuto
static void Do_PendingOrder(unsigned char bAuto, char bAutoConn); // Modified by Kim_LinHB 2014/11/4

static void Do_Stocks(int sub);
static void Do_PrintStock(unsigned char bDetail);
static void Do_DispStock(void);

static void Do_Reports(int sub);
static void Do_ReportCashier(void);
static void Do_ReportManager(void);
static void Do_ReportServer(void);

static void Do_Transmission(int sub);//done
static void Do_SettDownload(void);//done
static unsigned char Do_SalesPolling(unsigned char bAuto, char bAutoConn); // Modified by Kim_LinHB 2014/11/4
static void Do_CancalSale(void);
static void Do_Claim(void);
// Modified by Kim_LinHB 2014/10/30
static void Do_GetMail(char bAutoConn); // Modified by Kim_LinHB 2014/11/4
static void Do_SoftDownLoad(unsigned char bAuto);
static void Do_LogoDownLoad(void);
static void Do_Balance(void);

static void Do_Cashier(int sub);
static unsigned char Do_Login(void);
static void Do_Logout(void);
static void Do_ChangePwd(void);
static void Do_CreateUser(void);
static void Do_DelUser(void);
static void Do_ViewList(void);

static void Do_CashOrder(void); // Added by Kim_LinHB 2013-5-21 v4.59

static int SelCateory(SAV_PARAM stSav, unsigned char type);
static int SelProduct(SAV_PARAM *pstSav, unsigned char *pszProductId, unsigned int uiCategory_Code);
static unsigned char SetPinsNum(SAV_PARAM stSav, const unsigned char *pszProductId, unsigned int *IONum); // Added by Kim_LinHB 2013-3-21 v4.56
//static char GetPin(TRANS_DATA *pstTrans); // Removed by Kim_LinHB 2013-4-1 v4.57
static const unsigned char* SelUsersList(const USER_INFO *Users, unsigned int iNums);

static void GetProductIndex(SAV_PARAM stSav, TRANS_DATA *pstTrans); // Added by Kim_LinHB 2013-3-22 v4.56

static void RefreshStock(void);
static void Erase_Salepolling(unsigned char type);
static void AscInc(unsigned short *No);

// Added by Kim_LinHB 2013-5-22 v4.59
static void SetProductsQuantity(SAV_PARAM *pstSav, TRANS_DATA_CASH *pstCurTran, unsigned int uiCategory_Code);
static void CalcAmt(TRANS_DATA_CASH *pstCurTran);
// Add End

// Added by Kim_LinHB 2013-7-6 v4.62
static void UpdateProductLsit(SAV_PARAM *dst, const SAV_PARAM src);

// Added by Kim_LinHB 2014-5-12
static int Do_Reversal(RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

// Added by Kim_LinHB 2013-4-1 v4.57
static void Swap(short *a, short *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}
// Add End

const unsigned char c_LANG[][5 + 1] = 
{
	{"en-US"},
	{"tr-TR"},
};

void DoTrans(unsigned int uiType)
{

#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,EntryType=%d",__FILE__,__LINE__,__FUNCTION__,uiType);
#endif
	switch(uiType / 10)
	{
	case 1:
		switch(uiType)
		{
		case ID_SaleCodes:
#ifdef LHB_TEST
	MyTest();
#endif
#ifdef JEFF_TEST
	strcpy(gl_MaintenaceParam.szTermID,"20005556");
	strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "193.169.190.118");
	strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, "cmnet");
	DebugComSend("File_%s,LineNo:%d,func=%s,gl_MaintenaceParam.ucMerchantMode=%d,uiType=%d",__FILE__,__LINE__,__FUNCTION__,gl_MaintenaceParam.ucMerchantMode,uiType);
#endif
			if(OFFLINE == gl_MaintenaceParam.ucMerchantMode)
				Do_SaleCodes(OFFLINE);
			else if (ONLINE == gl_MaintenaceParam.ucMerchantMode)
			{
				// Added by Kim_LinHB 2013-9-11 v4.68
				Do_SalesPolling(TRUE, TRUE);
				// Add End
				Do_SaleCodes(ONLINE);
			}
			else 
				Do_SaleCodes(MIX);

			break;
		case ID_Duplicate:
			Do_Duplicate();
			break;
// 		case ID_RestockPins:
// 			Do_RestorePins(gl_bPlanned_SalesPolling);
// 			break;
		case ID_ON_OFFService:
			Do_OnOff_Service();
			break;
		case ID_CashOrder: // Added by Kim_LinHB 2013-5-22 v4.59
#ifdef LHB_TEST
	MyTest();
#endif
			Do_CashOrder();
			break;
		}
		break;
	case 2:
		Do_Stocks(uiType);
		break;
	case 3:
		Do_Reports(uiType);
		break;
	case 4:
		Do_Transmission(uiType);
		break;
	case 5:
		Do_Cashier(uiType);
		break;
	// Added by Kim_LinHB 2013-12-44 v4.76
	case 6:
		Do_RestorePin_menu(uiType);
		break;
	default:
		break;
	}
}

unsigned short FindCodesCnt(unsigned char type, const unsigned char *ProductId)
{
	unsigned int i;
	unsigned short Cnt = 0;
	unsigned char ucType;
	STOCK stock;

	if(0xFF == type && !ProductId)
		return NB_MAX_CODE_STOCK;

	if(type != 0xFF && !ProductId)
		ucType = 1;
	if(0xFF == type && ProductId)
		ucType = 2;
	if(type != 0xFF && ProductId)
		ucType = 3;

	ReadStock_All(&stock);

	switch(ucType)
	{
	case 1:
		for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
		{
			if(type == stock.stStock[i].ucProductState)
				++Cnt;
		}
		break;
	case 2:
		for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
		{
			if(0 == strcmp(stock.stVoucher[i].szProductId, ProductId))
			{
				++Cnt;
			}
		}
		break;
	case 3:
		for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
		{
			if(type == stock.stStock[i].ucProductState &&
				0 == strcmp(stock.stVoucher[i].szProductId, ProductId))
			{
				++Cnt;
			}
		}
		break;
	}
	return Cnt;
}

void Erase_Report(unsigned char bAuto)
{
	int i;
	STOCK stock;
	if(!bAuto)
	{
		char loop = TIME_RETRY;
		unsigned char ucKey = 0;
		unsigned char szPwd[SIZE_MAX_PWD + 1];
		memset(&szPwd, 0, sizeof(szPwd));
		ScrCls();
		DispString(gl_pLangSelPiont[STR_EFFACEMENT_DES], 1 | DISP_LINE_LEFT);
		DispString(gl_pLangSelPiont[STR_RAPP_VAL_ANN], DISPLAYLINE | DISP_LINE_LEFT);
		ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
		if(0 != ucKey)
			return;

		while(loop > 0)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
			{
				if(strcmp(szPwd, gl_MaintenaceParam.stManage.szPwd))
				{
					--loop;
					DispString("Wrong PWD", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)
						return;
				}
				else
				{
					Do_Logout();
					break;
				}
			}
			else
				return;
		}
	}

	if(-1 != fexist(FILE_TRANS_OL))
		remove(FILE_TRANS_OL);

	if(-1 != fexist(FILE_CASHORDER))
		remove(FILE_CASHORDER);
	
	ScrCls();
	DispString(gl_pLangSelPiont[STR_ACTUALISE_STOCK], DISP_SCR_CENTER);
	ReadStock_All(&stock);
	//Erase_Salepolling(CODE_RESTOCK_SOLD); // Removed by Kim_LinHB 2013-7-5 v4.61
	for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
	{
		if(CODE_RESTOCK_SOLD == stock.stStock[i].ucProductState)
			stock.stStock[i].ucProductState = CODE_RESTOCK_SALESPOLLING;
	}
	WriteStock_All(&stock);
	
	gl_MaintenaceParam.bReportEmpty = TRUE;
	memset(&gl_LastTran, 0, sizeof(TRANS_DATA));
	WriteParam(WRT_MAINTA | WRT_TXN);
}

#ifdef _S_SERIES_
static void Do_SaleCodes(unsigned char Mode)
{
	enum
	{
		STEP_CHECKSETVICE = 0,
		STEP_LOGIN,
		STEP_CATEGORY,
		STEP_PRODUCT,
		STEP_CHECK_DUP,   // added by Allen 2016/03/08
		STEP_OFFLINE,
		STEP_UPLOADATA,
		STEP_PRINT,
		STEP_NEWORDER,
	};

	int iRet, i, j;
	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_CHECKSETVICE;
	int uiCategory_Code = -1;

	unsigned int PinsCnt = 0; // Added by Kim_LinHB 2013-3-21 v4.56
	short sPinIndex[NB_MAX_CODE_STOCK]; // Added by Kim_LinHB 2013-4-2 v4.57
	STOCK stock;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	//gl_bCashier_Login = 1;  //for test

	while(1)
	{
#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,Mode=%d,unStep=%d",__FILE__,__LINE__,__FUNCTION__,Mode,ucStep);
#endif
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_CHECKSETVICE:
			if(gl_bOnService)
				ucStep = STEP_LOGIN;
			else
			{
				DispString(gl_pLangSelPiont[STR_SERVICE],  1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_ARRETE], DISPLAYLINE | DISP_LINE_CENTER);
				PubWaitKey(TIME_DELAY);
				if(OK == Do_On_Service())
					ucStep = STEP_LOGIN;
				else
					return;
			}
			break;
		case STEP_LOGIN:
			if(!gl_bCashier_Login)
				if(Do_Login() != OK)
					return;
			if(MIX == gl_MaintenaceParam.ucMerchantMode)
			{
				unsigned char ucKey;
				unsigned char szMenuBuffer[50];
				ScrCls();
				// Modified by Kim_LinHB 2014/11/4
				DispString(gl_pLangSelPiont[STR_TRANS_MODE], 1 | DISP_LINE_CENTER);
				sprintf(szMenuBuffer, "1. %s", gl_pLangSelPiont[STR_TRANS_OFFLINE]);
				DispString(szMenuBuffer, 4 | DISP_LINE_LEFT);
				sprintf(szMenuBuffer, "2. %s", gl_pLangSelPiont[STR_TRANS_ONLINE]);
				DispString(szMenuBuffer, 6 | DISP_LINE_LEFT);
				do
				{
					ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
				}while(KEY1 != ucKey && KEY2 != ucKey && KEYCANCEL != ucKey && NOKEY != ucKey);
				if(KEY1 == ucKey)
					Mode = OFFLINE;
				else if (KEY2 == ucKey)
				{
					Mode = ONLINE;
					// Added by Kim_LinHB 2013-9-11 v4.68
					Do_SalesPolling(TRUE,TRUE);
					// Add End
				}
				else 
					return;
			}
			ucStep = STEP_CATEGORY;
			break;
		case STEP_CATEGORY:
			if(gl_SavParam.ucCategoryNum <= 0)
			{
				ScrCls();
				DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return;
			}
			uiCategory_Code = SelCateory(gl_SavParam, PRODUCT_ELECTRONIC);

			if(uiCategory_Code > 0)
			{
				ucStep = STEP_PRODUCT;
			}
			else
				return;
			break;
		case STEP_PRODUCT:
			iRet = SelProduct(&gl_SavParam, stCurTran.stVoucher.szProductId, uiCategory_Code);
			if(OK == iRet)
			{
				ucKey = KEYENTER;
				stCurTran.bOffline = Mode;
				//stCurTran.stVoucher.ucCodeCategory = ucCategory_Code; // Removed by Kim_LinHB 2013-3-22 v4.56

				GetProductIndex(gl_SavParam, &stCurTran); // Added by Kim_LinHB 2013-3-22 v4.56

				if(stCurTran.bOffline)
					ucStep = STEP_CHECK_DUP;    // modified by Allen 2016/03/08 from STEP_OFFLINE to STEP_CHECK_DUP
				else
					ucStep = STEP_UPLOADATA;
			}
			else if (-1 == iRet)
			{
				ucStep = STEP_CATEGORY;
			}
			else if (-2 == iRet)
				return;
			break;
		//added by Allen 2016/03/07
		case STEP_CHECK_DUP:
			memset(&stock, 0, sizeof(STOCK));
			ReadStock_All(&stock);
			for(i = 0; i < NB_MAX_CODE_STOCK - 1; i++)
			{
				for(j = i+1; j < NB_MAX_CODE_STOCK; j++)
				{
					if((0 == strcmp(stock.stVoucher[i].szProductId, stock.stVoucher[j].szProductId)) && 
						(0 == strcmp(stock.stVoucher[i].szVoucherId, stock.stVoucher[j].szVoucherId)))
					{
						memset(&stock.stVoucher[j], 0, sizeof(VOUCHER_INFO));
						memset(&stock.stStock[j], 0, sizeof(STOCK_INFO));
					}
				}
			}
			WriteStock_All(&stock);
			ucStep = STEP_OFFLINE;
			//add end
		case STEP_OFFLINE:// Modified by Kim_LinHB 2013-3-21 v4.56
//    			stCurTran.uiVoucherNum = 1;  //for test
//    			sPinIndex[0] = 0;
//    			stCurTran.ucTransType = ID_SaleCodes;
//    			strcpy(stCurTran.szCashier, gl_Cur_Cashier.szName);
//    			ucStep = STEP_PRINT;
//    			iRet = OK;
//    			break;

			stCurTran.usBatchNo = gl_SavParam.usBatchNo;

			stCurTran.uiVoucherNum = 0;

			PinsCnt = FindCodesCnt(CODE_RESTOCK_FREE, stCurTran.stVoucher.szProductId);
			stCurTran.uiVoucherNum = PinsCnt;
			ucKey = SetPinsNum(gl_SavParam, stCurTran.stVoucher.szProductId, 
				&stCurTran.uiVoucherNum);
			if (OK != ucKey)
			{
				if(KEYCANCEL == ucKey)
				{
					ucStep = STEP_PRODUCT;
					break;
				}
				else
					return;
			}

			if(PinsCnt >= stCurTran.uiVoucherNum) // certain event
			{
				// Modified by Kim_LinHB 2013-4-8 v4.57
				unsigned int i, j;
				GetCurTime(stCurTran.szDateTime);
				memset(sPinIndex, -1, sizeof(sPinIndex));
				ReadStock_All(&stock);
				for(i = 0, j = 0; i < NB_MAX_CODE_STOCK && j < stCurTran.uiVoucherNum; ++i)
				{
					if (CODE_RESTOCK_FREE == stock.stStock[i].ucProductState &&
						0 == strcmp(stock.stVoucher[i].szProductId, stCurTran.stVoucher.szProductId))
					{
						if(memcmp(stock.stVoucher[i].szExpirDate, stCurTran.szDateTime, SIZE_DATE_TIME) > 0)
						{
							sPinIndex[j++] = i;
						}
					}
				}
				// Modify End
				stCurTran.ucTransType = ID_SaleCodes;
				strcpy(stCurTran.szCashier, gl_Cur_Cashier.szName);
				ucStep = STEP_PRINT;
				iRet = OK;
			}
			break;
		case STEP_UPLOADATA:
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;
			stCurTran.uiVoucherNum = 1; // Added by Kim_LinHB 2013-3-22 v4.56

			strcpy(stCurTran.szCashier, gl_Cur_Cashier.szName);

			stCurTran.ucTransType = ID_SaleCodes;
			GetCurTime(stCurTran.szDateTime);

#if 0
			iRet = 0;
			strcpy(stCurTran.stVoucher.szNum, "1234 54123 1231");
			strcpy(stCurTran.stVoucher.szExpirDate, "150101");
			strcpy(stCurTran.stVoucher.szSerial, "0909090909");
			ucStep = STEP_PRINT;
			break;
#endif

			iRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK != iRet)
			{
				CloseConn();
				Do_Error(iRet, stRespH);
				return;
			}

			// Added by Kim_LinHB 2014-5-13
			// Modified by Kim_LinHB 2014/10/28
			iRet = Do_Reversal(&stRespH, &stRespB);
			if(OK != iRet)
			{
				CloseConn();
				Do_Error(iRet, stRespH);
				return;
			}

			if(OK == iRet)
			{
				int iRetryCnt = TIME_RETRY;
				do 
				{
					iRet = Get_ProvisionId_XML(gl_Cur_Cashier, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				} while (1 == iRet && iRetryCnt > 0);
			}

			if(OK == iRet)
			{
				int cnt = 3;

				WriteReversal(stCurTran.szProvisionID, stCurTran.ucTransType, stCurTran.szCashier); // Added by Kim_LinHB 2014-5-13
				
				while(cnt-- >0)
				{
					iRet = Merchant_OL_Evoucher_XML(&stCurTran, &stRespH, &stRespB, FALSE);
					if(iRet != 1 && iRet < ENTETE_MESSAGE_RECU_NOK) // Modified by Kim_LinHB 2014-5-8
					{
						if(iRet != OK)
							Do_Error(iRet, stRespH);
						DelReversal(); // got response from host  // Added by Kim_LinHB 2015-01-04
						break;
					}
					Do_Error(iRet, stRespH);
					if(cnt > 0)
					{
						DispString("retry!", 6 | DISP_LINE_RIGHT);
						DelayMs(500);
						ScrClrLine(6,7);
					}

				}
				if(cnt <= 0 && (1 == iRet || iRet >= ENTETE_MESSAGE_RECU_NOK)){
					iRet = Cancellation_XML(stCurTran, TRUE, &stRespH, &stRespB, FALSE);//cancel;
					if(0 <= iRet && iRet < ENTETE_MESSAGE_RECU_NOK && iRet != 1) // can not be 1
					{
						DelReversal();
					}
					if(iRet != OK)
						Do_Error(iRet, stRespH);
				}
			}
			else
			{
				CloseConn();
				Do_Error(iRet, stRespH);
				return;
			}

			CloseConn();

			AscInc(&gl_SavParam.usBatchNo);
			AscInc(&gl_SavParam.usTraceNo);
			WriteParam(WRT_SAVE);

			if(OK == iRet)
				SaveOLTransInfo(&stCurTran);

			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == iRet)
			{
				unsigned char bCanceled = FALSE;
				unsigned int i;			
				for(i = 0; i < stCurTran.uiVoucherNum && !bCanceled; ++i)
				{
					if(stCurTran.bOffline && 0 <= sPinIndex[i] && sPinIndex[i] < NB_MAX_CODE_STOCK)
					{
						STOCK_INFO tmp;
						ReadStock(sPinIndex[i], &tmp, &stCurTran.stVoucher);
						strcpy(tmp.szCashierId, gl_Cur_Cashier.szName);
						tmp.ucProductState = CODE_RESTOCK_SELLING;
						GetCurTime(stCurTran.szDateTime);
						GetCurTime(tmp.szSoldDate);
						WriteStock(sPinIndex[i], &tmp, NULL);
						AscInc(&gl_SavParam.usBatchNo);
					}

					// Modified by Kim_LinHB 2013-4-2 v4.57
					DispString(gl_pLangSelPiont[STR_CUSTOMER_COPY], 1 | DISP_LINE_CENTER);
					bCanceled = Prn_SaleCodes(gl_SavParam, stCurTran, PRN_CUSTOMER);//print
					if(gl_MaintenaceParam.bPrintMerchantCopy)
					{
						DispString(gl_pLangSelPiont[STR_MERCHANT_COPY2], 1 | DISP_LINE_CENTER);
						DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
						PubWaitKey(1);
						DispString("", DISPLAYLINE | DISP_LINE_CENTER);
						Prn_SaleCodes(gl_SavParam, stCurTran, PRN_MERCHANT);//print
					}
					// Modify End

					// Added by Kim_LinHB 2013/3/22 v4.56
					if(stCurTran.bOffline && 0 <= sPinIndex[i] && sPinIndex[i] < NB_MAX_CODE_STOCK)
					{
						//if(!bCanceled) // Removed by Kim_LinHB 2013-8-14 v4.62 printing can not be declined
						{
							STOCK_INFO tmp;
							ReadStock(sPinIndex[i], &tmp, NULL);
							//GetCurTime(tmp.szSoldDate);
							tmp.ucProductState = CODE_RESTOCK_SOLD;
							WriteStock(sPinIndex[i], &tmp, NULL);
						}
					}
				}
			}
			else
			{
				Do_Error((unsigned char)iRet, stRespH);
				ucStep = STEP_NEWORDER; // Added by Kim_LinHB 2013-3-26
				break;
			}

			ucStep = STEP_NEWORDER;
			// Added by Kim_LinHB 2013-3-26
			memcpy(&gl_LastTran, &stCurTran, sizeof(TRANS_DATA));
			gl_MaintenaceParam.bReportEmpty = FALSE;
			ScrCls();
			DispString("Saving", 1 | DISP_LINE_CENTER);
			WriteParam(WRT_MAINTA | WRT_TXN); 
			// Add End

			break;
		case STEP_NEWORDER:
			// Modified by Kim_LinHB 2013-4-8 v4.57
			memset(stCurTran.szAmount, 0, sizeof(stCurTran.szAmount));
			//memset(&stCurTran.stVoucher, 0, sizeof(stCurTran.stVoucher)); // Removed by Kim_LinHB 2013-4-8 v4.57
			stCurTran.uiVoucherNum = 0;
			if(stCurTran.bOffline)
				ucStep = STEP_OFFLINE;
			else
				ucStep = STEP_PRODUCT;
			// Modify End
			//return; //for test
			break;
		}
	}
}
#else
static void Do_SaleCodes(unsigned char Mode)
{
	enum
	{
		STEP_CHECKSETVICE = 0,
		STEP_LOGIN,
		STEP_CATEGORY,
		STEP_PRODUCT,
		STEP_OFFLINE,
		STEP_GETPIN,  // Added by Kim_LinHB 2013-4-10 for loop
		STEP_UPLOADATA,
		STEP_PRINT,
		STEP_NEWORDER,
	};

	int iRet;
	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_CHECKSETVICE;
	int uiCategory_Code = -1;

	unsigned int PinsCnt = 0; // Added by Kim_LinHB 2013-3-21 v4.56
	short sPinIndex; // Modified by Kim_LinHB 2013-5-3 4.58

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	STOCK_INFO stock_info;
	VOUCHER_INFO voucher;

	TRANS_DATA stCurTran;
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_CHECKSETVICE:
			if(gl_bOnService)
				ucStep = STEP_LOGIN;
			else
			{
				DispString(gl_pLangSelPiont[STR_SERVICE],  1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_ARRETE], DISPLAYLINE | DISP_LINE_CENTER);
				PubWaitKey(TIME_DELAY);
				if(OK == Do_On_Service())
					ucStep = STEP_LOGIN;
				else
					return;
			}
			break;
		case STEP_LOGIN:
#ifndef LHB_TEST
			if(!gl_bCashier_Login)
				if(Do_Login() != OK)
					return;
#endif
			if(MIX == gl_MaintenaceParam.ucMerchantMode)
			{
				unsigned char ucKey;
				unsigned char szMenuBuffer[50];
				ScrCls();
				// Modified by Kim_LinHB 2014/11/4
				DispString(gl_pLangSelPiont[STR_TRANS_MODE], 1 | DISP_LINE_CENTER);
				sprintf(szMenuBuffer, "1. %s", gl_pLangSelPiont[STR_TRANS_OFFLINE]);
				DispString(szMenuBuffer, 4 | DISP_LINE_LEFT);
				sprintf(szMenuBuffer, "2. %s", gl_pLangSelPiont[STR_TRANS_ONLINE]);
				DispString(szMenuBuffer, 6 | DISP_LINE_LEFT);
				do
				{
					ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
				}while(KEY1 != ucKey && KEY2 != ucKey && KEYCANCEL != ucKey && NOKEY != ucKey);
				if(KEY1 == ucKey)
					Mode = OFFLINE;
				else if (KEY2 == ucKey)
					Mode = ONLINE;
				else 
					return;
			}
			ucStep = STEP_CATEGORY;
			break;
		case STEP_CATEGORY:

#ifdef LHB_TEST
			gl_SavParam.ucCategoryNum = 1;
			gl_SavParam.stSavParamCategory[0].uiCodeCategory = 30;
			gl_SavParam.stSavParamCategory[0].ucCategoryType = 2;
			strcpy(gl_SavParam.stSavParamCategory[0].szName, "QUICKNET EXPRESS");

			gl_SavParam.ucProductsNum = 1;
			gl_SavParam.stSavParamProducts[0].ucType = 2;
			gl_SavParam.stSavParamProducts[0].uiCodeCategory = 30;
			gl_SavParam.stSavParamProducts[0].ucCategory_Index = 0;
			strcpy(gl_SavParam.stSavParamProducts[0].szCodeProduct, "155");
			strcpy(gl_SavParam.stSavParamProducts[0].szAmount, "5000");
			strcpy(gl_SavParam.stSavParamProducts[0].szNameProduct, "10GB/4 months");
			strcpy(gl_SavParam.stSavParamProducts[0].szNameProduct_AR, "10 GB \\ 4 ????");
#endif

			if(gl_SavParam.ucCategoryNum <= 0)
			{
				ScrCls();
				DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return;
			}
			uiCategory_Code = SelCateory(gl_SavParam, PRODUCT_ELECTRONIC);

			if(uiCategory_Code > 0)
			{
				ucStep = STEP_PRODUCT;
			}
			else
				return;
			break;
		case STEP_PRODUCT:
			iRet = SelProduct(&gl_SavParam, stCurTran.stVoucher.szProductId, uiCategory_Code);
			if(OK == iRet)
			{
				ucKey = KEYENTER;
				stCurTran.bOffline = Mode;
				//stCurTran.stVoucher.ucCodeCategory = ucCategory_Code; // Removed by Kim_LinHB 2013-3-22 v4.56

				GetProductIndex(gl_SavParam, &stCurTran); // Added by Kim_LinHB 2013-3-22 v4.56

				if(stCurTran.bOffline)
					ucStep = STEP_OFFLINE;
				else
					ucStep = STEP_UPLOADATA;
			}
			else if (-1 == iRet)
			{
				ucStep = STEP_CATEGORY;
			}
			else if (-2 == iRet)
				return;
			break;
		case STEP_OFFLINE:// Modified by Kim_LinHB 2013-3-21 v4.56
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;

			stCurTran.uiVoucherNum = 0;

			PinsCnt = FindCodesCnt(CODE_RESTOCK_FREE, stCurTran.stVoucher.szProductId);
			stCurTran.uiVoucherNum = PinsCnt;
			ucKey = SetPinsNum(gl_SavParam, stCurTran.stVoucher.szProductId, 
				&stCurTran.uiVoucherNum);
			if (OK != ucKey)
			{
				if(KEYCANCEL == ucKey)
				{
					ucStep = STEP_PRODUCT;
					break;
				}
				else
					return;
			}

			if(PinsCnt >= stCurTran.uiVoucherNum) // certain event
			{
				ucStep = STEP_GETPIN;
				stCurTran.ucTransType = ID_SaleCodes;
				strcpy(stCurTran.szCashier, gl_Cur_Cashier.szName);
			}
			break;
		case STEP_GETPIN:
			{
				unsigned int i;
				sPinIndex = -1;
				GetCurTime(stCurTran.szDateTime);
				for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
				{
					ReadStock(i, &stock_info, &voucher);
					if (CODE_RESTOCK_FREE == stock_info.ucProductState &&
						0 == strcmp(voucher.szProductId, stCurTran.stVoucher.szProductId))
					{
						if(memcmp(voucher.szExpirDate, stCurTran.szDateTime, SIZE_DATE_TIME) > 0)
						{
							sPinIndex = i;
							break;
						}
					}
				}
				// Modify End
				ucStep = STEP_PRINT;
				iRet = OK;
			}
			break;
			// Modify End
		case STEP_UPLOADATA:
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;
			stCurTran.uiVoucherNum = 1; // Added by Kim_LinHB 2013-3-22 v4.56

			strcpy(stCurTran.szCashier, gl_Cur_Cashier.szName);

			stCurTran.ucTransType = ID_SaleCodes;
			GetCurTime(stCurTran.szDateTime);

#ifdef LHB_TEST
			iRet = 0;
			strcpy(stCurTran.stVoucher.szNum, "1234 54123 1231");
			strcpy(stCurTran.stVoucher.szExpirDate, "150101");
			strcpy(stCurTran.stVoucher.szSerial, "0909090909");
			ucStep = STEP_PRINT;
			break;
#endif

			iRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK != iRet)
			{
				CloseConn();
				Do_Error(iRet, stRespH);
				return;
			}

			// Modified by Kim_LinHB 2014/10/29
			iRet = Do_Reversal(&stRespH, &stRespB);
			if(OK != iRet)
			{
				CloseConn();
				Do_Error(iRet, stRespH);
				return;
			}

			if(OK == iRet)
			{
				int iRetryCnt = TIME_RETRY;
				do 
				{
					iRet = Get_ProvisionId_XML(gl_Cur_Cashier, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				} while (1 == iRet && iRetryCnt > 0);
			}

			if(OK == iRet)
			{
				int cnt = 3;

				WriteReversal(stCurTran.szProvisionID, stCurTran.ucTransType, stCurTran.szCashier); // Added by Kim_LinHB 2014-5-13

				while(cnt-- >0)
				{
					// Modified by Kim_LinHB 2014-5-8
					iRet = Merchant_OL_Evoucher_XML(&stCurTran, &stRespH, &stRespB, FALSE);
					if(iRet != 1 && iRet < ENTETE_MESSAGE_RECU_NOK)
					{
						if(iRet != OK)
							Do_Error(iRet, stRespH);
						DelReversal(); // got response from host  // Added by Kim_LinHB 2015-01-04
						break;
					}
					Do_Error(iRet, stRespH);
					if(cnt > 0)
					{
						DispString("retry!", 6 | DISP_LINE_RIGHT);
						DelayMs(500);
						ScrClrLine(6,7);
					}
				}
 				if(cnt <= 0 && (1 == iRet || iRet >= ENTETE_MESSAGE_RECU_NOK))
				{
					iRet = Cancellation_XML(stCurTran, TRUE, &stRespH, &stRespB, FALSE);//cancel;
					if(0 <= iRet && iRet < ENTETE_MESSAGE_RECU_NOK && iRet != 1) // can not be 1
					{
						DelReversal();
					}
					if(iRet != OK)
						Do_Error(iRet, stRespH);
				}
			}
			else
			{
				CloseConn();
				Do_Error(iRet, stRespH);
				return;
			}

			CloseConn();

			AscInc(&gl_SavParam.usBatchNo);
			AscInc(&gl_SavParam.usTraceNo);
			WriteParam(WRT_SAVE);

			if(OK == iRet)
				SaveOLTransInfo(&stCurTran);
			
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == iRet)
			{
				unsigned char bCanceled = FALSE;
				if(stCurTran.bOffline && 0 <= sPinIndex && sPinIndex < NB_MAX_CODE_STOCK)
				{
					ReadStock(sPinIndex, &stock_info, &stCurTran.stVoucher);
					strcpy(stock_info.szCashierId, gl_Cur_Cashier.szName);
					stock_info.ucProductState = CODE_RESTOCK_SELLING;
					GetCurTime(stCurTran.szDateTime);
					GetCurTime(stock_info.szSoldDate);
					WriteStock(sPinIndex, &stock_info, NULL);
					AscInc(&gl_SavParam.usBatchNo);
				}
					
				// Modified by Kim_LinHB 2013-4-2 v4.57
				DispString(gl_pLangSelPiont[STR_CUSTOMER_COPY], 1 | DISP_LINE_CENTER);
				bCanceled = Prn_SaleCodes(gl_SavParam, stCurTran, PRN_CUSTOMER);//print
				if(gl_MaintenaceParam.bPrintMerchantCopy)
				{
					DispString(gl_pLangSelPiont[STR_MERCHANT_COPY2], 1 | DISP_LINE_CENTER);
					DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
					PubWaitKey(1);
					DispString("", DISPLAYLINE | DISP_LINE_CENTER);
					Prn_SaleCodes(gl_SavParam, stCurTran, PRN_MERCHANT);//print
				}
				// Modify End

				// Removed by Kim_LinHB 2013-8-14 v4.62
				// Added by Kim_LinHB 2013/3/22 v4.56
 				if(stCurTran.bOffline && 0 <= sPinIndex && sPinIndex < NB_MAX_CODE_STOCK)
 				{
 					//if(!bCanceled)
 					{
 						//GetCurTime(stock_info.szSoldDate);
  						stock_info.ucProductState = CODE_RESTOCK_SOLD;
 						WriteStock(sPinIndex, &stock_info, NULL);
 					}
  				}
				// Add End
				// Remove End
				if(!bCanceled && --stCurTran.uiVoucherNum > 0)
				{
					ucStep = STEP_GETPIN;
					break;
				}
			}
			else
			{
				Do_Error((unsigned char)iRet, stRespH);
				ucStep = STEP_NEWORDER; // Added by Kim_LinHB 2013-3-26
				break;
			}
			
			ucStep = STEP_NEWORDER;
			// Added by Kim_LinHB 2013-3-26
			memcpy(&gl_LastTran, &stCurTran, sizeof(TRANS_DATA));
			gl_MaintenaceParam.bReportEmpty = FALSE;
			ScrCls();
			DispString("Saving", 1 | DISP_LINE_CENTER);
			WriteParam(WRT_MAINTA | WRT_TXN); 
			// Add End
			
			break;
		case STEP_NEWORDER:
			// Modified by Kim_LinHB 2013-4-8 v4.57
			memset(stCurTran.szAmount, 0, sizeof(stCurTran.szAmount));
			//memset(&stCurTran.stVoucher, 0, sizeof(stCurTran.stVoucher)); // Removed by Kim_LinHB 2013-4-8 v4.57
			stCurTran.uiVoucherNum = 0;
			if(stCurTran.bOffline)
				ucStep = STEP_OFFLINE;
			else
				ucStep = STEP_PRODUCT;
			// Modify End
			break;
		}
	}
}
#endif

static void Do_CashOrder(void)
{
	enum
	{
		STEP_INPUTMANAGERPWD = 0,
		STEP_JUDGE,
		STEP_QUANTITY,
		STEP_CHECKORDER,
		STEP_INPUTPWD,
		STEP_UPLOADATA,
		STEP_PRINT,
	};

	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_INPUTMANAGERPWD;
	
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	unsigned char szPwd[SIZE_MAX_PWD + 1];

	TRANS_DATA_CASH stCurTran;
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA_CASH));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	memset(szPwd, 0, sizeof(szPwd));

	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_INPUTMANAGERPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucStep = STEP_JUDGE;
			else if(KEYCANCEL == ucKey)
				return;
			break;
		case STEP_JUDGE:
			if (0 == strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
			{
				//Do_Logout();
				loop = TIME_RETRY;
				ucStep = STEP_QUANTITY;
				// Added by Kim_LinHB 2013-9-11 v4.68
				Do_SalesPolling(TRUE,TRUE);
				// Add End
			}
			else
			{
				--loop;
				ScrCls();
				DispString("Wrong Pwd", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				if(loop <= 0)
					return;
				else
					ucStep = STEP_INPUTMANAGERPWD;
			}
			break;
		case STEP_QUANTITY:
			{
				int uiCategory_Code = -1;
				if(gl_SavParam.ucCategoryNum <= 0)
				{
					ScrCls();
					DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_OUT_STANDARD_30S);
					return;
				}

				while(1){
					uiCategory_Code = SelCateory(gl_SavParam, PRODUCT_ELECTRONIC);
					if(-1 == uiCategory_Code)
					{
						if(stCurTran.ucProductNum > 0)
							ucStep = STEP_CHECKORDER;
						else
							ucKey = 0xff;
						break;
					}
					else if(-2 == uiCategory_Code)
					{
						ucKey = 0xFF;
						break;
					}
					else
					{
						stCurTran.ucTransType = ID_CashOrder;
						SetProductsQuantity(&gl_SavParam, &stCurTran, uiCategory_Code);
					}
				}
			}
			break;
		case STEP_CHECKORDER:
			ScrSetIcon(ICON_UP, OPENICON);
			CalcAmt(&stCurTran);
			DispString(gl_pLangSelPiont[STR_VALIDE_COMMANDE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);

			ucStep = STEP_INPUTPWD;

			if(KEYENTER == ucKey)
				Prn_CashOrder(stCurTran, PRN_PREVIEW);//print
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S,FALSE);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stManage.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
				{
					loop = TIME_RETRY;
					ucStep = STEP_UPLOADATA;
				}
			}
			else
				return;
			break;
		case STEP_UPLOADATA:
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;
			stCurTran.ucTransType = ID_CashOrder;
			GetCurTime(stCurTran.szDateTime);

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK != ucRet)
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}

			// Modified by Kim_LinHB 2014/10/29
			ucRet = Do_Reversal(&stRespH, &stRespB);
			if(OK != ucRet)
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}

			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Get_ProvisionId_XML(gl_MaintenaceParam.stManage, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);
			}

			if(OK == ucRet)
			{
				int cnt = 3;

				WriteReversal(stCurTran.szProvisionID, stCurTran.ucTransType, ""); // Added by Kim_LinHB 2014-5-13

				// Modified by Kim_LinHB 2014-5-8
				stCurTran.ucTransType = ID_CashOrder;
				while(cnt-- >0)
				{
					ucRet = CashOrder_XML(stCurTran, stCurTran.szDateTime, &stRespH, &stRespB, FALSE);
					if(ucRet != 1 && ucRet < ENTETE_MESSAGE_RECU_NOK)
					{
						if(ucRet != OK)
							Do_Error(ucRet, stRespH);
						DelReversal(); // got response from host  // Added by Kim_LinHB 2015-01-04
						break;
					}
					Do_Error(ucRet, stRespH);
					if(cnt > 0)
					{
						DispString("retry!", 6 | DISP_LINE_RIGHT);
						DelayMs(500);
						ScrClrLine(6,7);
					}
				}
				if(cnt <= 0 && (1 == ucRet || ucRet >= ENTETE_MESSAGE_RECU_NOK))
				{
					TRANS_DATA stCurTmp;
					memset(&stCurTmp, 0, sizeof(stCurTmp));
					strcpy(stCurTmp.szProvisionID, stCurTran.szProvisionID);
					stCurTmp.usTraceNo = stCurTran.usTraceNo;
					strcpy(stCurTmp.szDateTime, stCurTran.szDateTime);
					stCurTmp.ucTransType = stCurTran.ucTransType;
					ucRet = Cancellation_XML(stCurTmp, TRUE, &stRespH, &stRespB, FALSE);//cancel;
					if(0 <= ucRet && ucRet < ENTETE_MESSAGE_RECU_NOK && ucRet != 1) // can not be 1
					{
						DelReversal();
					}
					if(ucRet != OK)
						Do_Error(ucRet, stRespH);
				}
			}
			else
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}

			CloseConn();

			AscInc(&gl_SavParam.usBatchNo);
			AscInc(&gl_SavParam.usTraceNo);
			WriteParam(WRT_SAVE);

			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
			{
				SaveCashOrder(&stCurTran);
				//WriteParam();

				DispString(gl_pLangSelPiont[STR_SALESMAN_COPY], 1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
				Prn_CashOrder(stCurTran, PRN_CUSTOMER);//print
				DispString(gl_pLangSelPiont[STR_MERCHANT_COPY], 1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				Prn_CashOrder(stCurTran, PRN_MERCHANT);//print
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			return;
		}

		if(0xFF == ucKey)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_EXIT_ORDER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0 == PubYesNo(TIME_OUT_STANDARD_30S))
			{
				return;
			}
		}
	}
}

static void Do_Duplicate(void)
{
	enum
	{
		STEP_CHECKTRANS = 0,
// 		STEP_INPUTID,
// 		STEP_INPUTPWD,
// 		STEP_VERIFY,
		STEP_PRINT,
	};

	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_CHECKTRANS;
	//unsigned char bVerify = FALSE;
	//char loop = TIME_RETRY;
//	unsigned int kk;  // Removed by Kim_LinHB 2013-3-22 v4.56

	TRANS_DATA stTempTran;
	USER_INFO stCurUser;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	memset(&szPwd, 0, sizeof(szPwd));
	memset(&stCurUser, 0, sizeof(USER_INFO));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_CHECKTRANS:
			if(0 == gl_LastTran.ucTransType)
			{
				ucKey = 0xFF;
				DispString(gl_pLangSelPiont[STR_PAS_D_OPERATION], DISP_SCR_CENTER);
				PubWaitKey(TIME_DELAY);
				return;
			}
			else
			{
				//ucStep = STEP_INPUTID;
				ucStep = STEP_PRINT;
			}
			break;
// 		case STEP_INPUTID:
// 			PubShowTitle(TRUE, "Cashier");
// 			DispString("Username:", 2 | DISP_LINE_LEFT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN, 0, SIZE_CASHIER_NAME, stCurUser.szName, TIME_OUT_STANDARD_30S);
// 			ucStep = STEP_INPUTPWD;
// 			break;
// 		case STEP_INPUTPWD:
// 			PubShowTitle(TRUE, "Cashier");
// 			DispString("Password:", 2 | DISP_LINE_LEFT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, 0, SIZE_MAX_PWD, stCurUser.szPwd, TIME_OUT_STANDARD_30S);
// 			ucStep = STEP_VERIFY;
// 			break;
// 		case STEP_VERIFY:
// 			if(OK == ReadUsersPwd(stCurUser.szName, szPwd))
// 			{
// 				if(0 == strcmp(szPwd, stCurUser.szPwd))
// 				{
// 					bVerify = TRUE;
// 				}
// 			}
// 
// 			if(bVerify)
// 			{
// 				memcpy(&stTempTran, &gl_LastTran, sizeof(TRANS_DATA));
// 				ucStep = STEP_PRINT;
// 			}
// 			else
// 			{
// 				DispString("Wrong ID or PWD", DISP_SCR_CENTER);
// 				Beep_Speaker(BEEP_ERR);
// 				if(loop > 0)
// 				{
// 					--loop;
// 					ucStep = STEP_INPUTPWD;
// 				}
// 				else
// 					ucKey = 0xFF;
// 				PubWaitKey(TIME_DELAY);
// 			}
// 			break;
		case STEP_PRINT:
			memcpy(&stTempTran, &gl_LastTran, sizeof(TRANS_DATA));
			// Modified by Kim_LinHB 2013-3-22 v4.56
			DispString(gl_pLangSelPiont[STR_MERCHANT_COPY2], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
			Prn_SaleCodes(gl_SavParam, stTempTran, PRN_MERCHANT);//print
			// Modify End
			return;
		}
		
		if(0xFF == ucKey)
			return;
	}
}

//not used since v4.76
#if 0
static void Do_RestorePins(unsigned char bAuto)
{
	unsigned int i; b
	unsigned char ucRet = OK, ucKey = 0;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	unsigned char bContinue = FALSE;
	unsigned int Count = 0;
	unsigned char loop = TIME_RETRY;
	STOCK stock;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	char bValid = TRUE;

	TRANS_DATA stCurTran;
	memset(&stCurTran, 0 ,sizeof(stCurTran));
	stCurTran.ucTransType = ID_RestockPins;

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	if(ONLINE == gl_MaintenaceParam.ucMerchantMode)
		return;

	ScrCls();
	if(!bAuto)
	{
		while(loop > 0)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(0xFF == ucKey || KEYCANCEL == ucKey)
				return;
			if(KEYENTER == ucKey)
			{
				if (strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
				{
					--loop;
					ScrCls();
					DispString("Wrong Pwd", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)
						return;
				}
				else
				{
					//Do_Logout();
					break;
				}
			}
		}
	}

	// Added by Kim_LinHB 2013-9-11 v4.68
	ucRet= Do_SalesPolling(TRUE);
	// Add End
	if(OK == ucRet)
	{
		unsigned int uiEmpty = FindCodesCnt(CODE_RESTOCK_EMPTY, NULL);

		ScrCls();
		DispString(gl_pLangSelPiont[STR_RECHARGEMENT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_PASSWORDS], DISPLAYLINE | DISP_LINE_CENTER);

		if(0 == uiEmpty)
			ucRet = PlaceInsufficient;

		if(OK == ucRet)
		{
			if(OK == ucRet)
				ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

			do{
				memset(stCurTran.szProvisionID, 0, sizeof(stCurTran.szProvisionID));

				// Modified by Kim_LinHB 2013-8-28 v4.64 from strlen > atol
				if(atol(gl_MaintenaceParam.szLastProvisionID) > 0)
					strcpy(stCurTran.szProvisionID, gl_MaintenaceParam.szLastProvisionID);
			
				if(OK == ucRet && 0 == atol(stCurTran.szProvisionID))
				{
					int iRetryCnt = TIME_RETRY;
					do 
					{
						ucRet = Get_ProvisionId_XML(gl_MaintenaceParam.stManage, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
						--iRetryCnt;
					} while (1 == ucRet && iRetryCnt > 0);

					if(OK == ucRet)
					{
						// Added by Kim_LinHB 2013-8-10 v4.62
						strcpy(gl_MaintenaceParam.szLastProvisionID, stCurTran.szProvisionID);
						WriteParam(WRT_MAINTA);
						// Add End
					}
				}
				

				if(OK == ucRet)
				{
					int cnt = 3;
					unsigned char tmp[15];

					while(cnt-- >0)
					{
						ucRet = ReStockPins_XML(stCurTran.szProvisionID, &bContinue, &Count, &stRespH, &stRespB, FALSE);
						if(ucRet != 1)
							break;
					}
// 					if(cnt <= 0)
// 						Cancellation_XML(stCurTran, &stRespH, &stRespB, TRUE);//cancel;

					if(OK == ucRet || PbSalesPolling == ucRet) // Modified by Kim_LinHB 2013-8-14 v4.62 PbSalesPolling
					{
						memset(gl_MaintenaceParam.szLastProvisionID, 0, sizeof(gl_MaintenaceParam.szLastProvisionID));
						WriteParam(WRT_MAINTA);
					}

					sprintf(tmp, "%s:%lu", "RESTOCKED", Count);
					DispString(tmp, (DISPLAYLINE + 2) | DISP_LINE_RIGHT);
					DelayMs(200);
				}
				else
				{
					CloseConn();
					Do_Error(ucRet, stRespH);
					//return;
				}
			}while(OK == ucRet && bContinue);

			CloseConn();
			WriteParam(WRT_MAINTA);
		}

		ScrCls();
// 		if(OK == ucRet ||
// 			PbSalesPolling == ucRet)
		if(Count > 0)
		{
			bValid = Prn_SalePolling(CODE_RESTOCK_DOWNLOADING);
			if(bValid)
				SortStock();
		}

		if(ucRet != OK)
		{
			if(PbSalesPolling == ucRet ||
				PlaceInsufficient == ucRet)
				Prn_PbPlace();
			else
			{
				Do_Error(ucRet, stRespH);
			}
		}
		if(bValid) // Modified by Kim_LinHB 2013-9-22 v4.69
		{
			ReadStock_All(&stock);
			for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
			{
				if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState)
					stock.stStock[i].ucProductState = CODE_RESTOCK_FREE;
			}
			WriteStock_All(&stock);
		}
	}
}
#endif

static void Do_RestorePin_New(unsigned char bAuto)
{
	unsigned int i;
	unsigned char ucRet = OK, ucKey = 0;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	unsigned char bContinue = FALSE;
	unsigned int Count = 0;
	unsigned char loop = TIME_RETRY;
	STOCK stock;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	char bValid = TRUE;

	TRANS_DATA stCurTran;
	memset(&stCurTran, 0 ,sizeof(stCurTran));
	stCurTran.ucTransType = ID_RestockPin;

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	if(ONLINE == gl_MaintenaceParam.ucMerchantMode)
		return;

	ScrCls();
	if(!bAuto)   // ÑéÖ¤ÃÜÂë
	{
		while(loop > 0)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(0xFF == ucKey || KEYCANCEL == ucKey)
				return;
			if(KEYENTER == ucKey)
			{
				if (strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
				{
					--loop;
					ScrCls();
					DispString("Wrong Pwd", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)
						return;
				}
				else
				{
					//Do_Logout();
					break;
				}
			}
		}
	}
	ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
	if(OK == ucRet)
	{	
		// Added by Kim_LinHB 2013-9-11 v4.68
		ucRet= Do_SalesPolling(TRUE, FALSE);
		// Add End
	}
	if(OK == ucRet)
	{
		unsigned int uiEmpty = FindCodesCnt(CODE_RESTOCK_EMPTY, NULL);

		ScrCls();
		DispString(gl_pLangSelPiont[STR_RECHARGEMENT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_PASSWORDS], DISPLAYLINE | DISP_LINE_CENTER);

		if(0 == uiEmpty)
			ucRet = PlaceInsufficient;

		if(OK == ucRet)
		{
			//if(OK == ucRet)
			//	ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

			do{
				memset(stCurTran.szProvisionID, 0, sizeof(stCurTran.szProvisionID));

				// Modified by Kim_LinHB 2013-8-28 v4.64 from strlen > atol
				if(atol(gl_MaintenaceParam.szLastProvisionID) > 0)
					strcpy(stCurTran.szProvisionID, gl_MaintenaceParam.szLastProvisionID);

				if(OK == ucRet && 0 == atol(stCurTran.szProvisionID))
				{
					int iRetryCnt = TIME_RETRY;
					do 
					{
						ucRet = Get_ProvisionId_XML(gl_MaintenaceParam.stManage, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
						--iRetryCnt;
					} while (1 == ucRet && iRetryCnt > 0);

					if(OK == ucRet)
					{
						// Added by Kim_LinHB 2013-8-10 v4.62
						strcpy(gl_MaintenaceParam.szLastProvisionID, stCurTran.szProvisionID);
						WriteParam(WRT_MAINTA);
						// Add End
					}
				}

				if(OK == ucRet)
				{
					int cnt = 3;
					unsigned char tmp[15];

					while(cnt-- >0)
					{
						ucRet = ReStockPin_New_XML(stCurTran.szProvisionID, &bContinue, &Count, &stRespH, &stRespB, FALSE);
						if(ucRet != 1)
							break;
					}
					// 					if(cnt <= 0)
					// 						Cancellation_XML(stCurTran, &stRespH, &stRespB, TRUE);//cancel;

					if(OK == ucRet || PbSalesPolling == ucRet) // Modified by Kim_LinHB 2013-8-14 v4.62 PbSalesPolling
					{
						memset(&stock, 0, sizeof(stock));
						ReadStock_All(&stock);
						for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
						{
							if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState)
								stock.stStock[i].ucProductState = CODE_RESTOCK_FREE;
						}

						// if power off here, status will be: has downloading, has provid, marched provid order num > 0, downloading -> free, remove provid, print
						WriteStock_All(&stock);
						// if power off here, status will be: no downloading, has provid, marched provid order num > 0, remove provid, print

                        DelayMs(100);  //add by richard 20151127
						//ÕâÀïÓÐÒ»¸öprovisionIDÇå0
                        memset(gl_MaintenaceParam.szLastProvisionID, 0, sizeof(gl_MaintenaceParam.szLastProvisionID));    
						WriteParam(WRT_MAINTA);
						// if power off here, status will be: no downloading, no provid, marched provid order num > 0, print
					}

					sprintf(tmp, "%s:%lu", "RESTOCKED", Count);
					DispString(tmp, (DISPLAYLINE + 2) | DISP_LINE_RIGHT);
					DelayMs(200);
				}
				else
				{
					//CloseConn();
					Do_Error(ucRet, stRespH);
					//return;
				}
			}while(OK == ucRet && bContinue);

			//CloseConn();
			WriteParam(WRT_MAINTA);
		}

		ScrCls();
		// 		if(OK == ucRet ||
		// 			PbSalesPolling == ucRet)
		if(Count > 0)
		{
			//bValid = Prn_SalePolling(CODE_RESTOCK_DOWNLOADING);
			bValid = Prn_RestockPin();
			if(bValid)
				SortStock();
		}

		if(ucRet != OK)
		{
			if(PbSalesPolling == ucRet ||
				PlaceInsufficient == ucRet)
				Prn_PbPlace();
			else
			{
				Do_Error(ucRet, stRespH);
			}
		}
		if(bValid) // Modified by Kim_LinHB 2013-9-22 v4.69
		{
			ReadStock_All(&stock);
			for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
			{
				if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState)
					stock.stStock[i].ucProductState = CODE_RESTOCK_FREE;
			}

			// modified by Allen 20151203
			WriteStock_All(&stock);

			//memset(gl_MaintenaceParam.szLastProvisionID, 0, sizeof(gl_MaintenaceParam.szLastProvisionID));
			//WriteStock_All(&stock);
			//WriteParam(WRT_MAINTA);
			if ((OK == ucRet && 0 == bContinue) || (PbSalesPolling == ucRet) || (PlaceInsufficient == ucRet))
			{
				memset(gl_MaintenaceParam.szLastProvisionID, 0, sizeof(gl_MaintenaceParam.szLastProvisionID));
				WriteParam(WRT_MAINTA);			
			}
			//end
		}

		Do_GetMail(FALSE); // Added by Kim_LinHB 2014-1-23 v4.81

		Do_PendingOrder(TRUE,FALSE); // Added by Kim_LinHB 2013-12-16 v4.77
	}
	CloseConn();
}

static void Do_PendingOrder(unsigned char bAuto, char bAutoConn)
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_UPLOADATA,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = (FALSE == bAuto ? STEP_INPUTPWD : STEP_UPLOADATA); // Modified by Kim_LinHB 2013-12-16 v4.77
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	bAutoConn = (FALSE == bAuto?TRUE:bAutoConn); // Added by Kim_LinHB 2014/11/4

	memset(&szPwd, 0, sizeof(szPwd));
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szPwd, gl_MaintenaceParam.stManage.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOADATA;
			}
			break;
		case STEP_UPLOADATA:
			do 
			{
				ucRet = PendingOrder_XML(&stRespH, &stRespB, bAutoConn);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);

			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
				Prn_PendingOrder();//print
			else
			{
				Do_Error(ucRet, stRespH);
			}
			return;
		}

		if(0xFF == ucKey)
			return;
	}
}

static void Do_RestorePin_menu(int sub)
{
	switch(sub)
	{
	case ID_RestockPin:
		Do_RestorePin_New(gl_bPlanned_SalesPolling);
		break;
	case ID_PendingOrder:
		Do_PendingOrder(FALSE,TRUE); // Modified by Kim_LinHB 2013-12-16 v4.77
		break;
	}
}

static unsigned char Do_On_Service(void)
{
	unsigned char ucRet = 0xFF;
	unsigned char ucKey = 0;
	ScrCls();
	DispString(gl_pLangSelPiont[STR_DEMARRER_SERVICE], 1 | DISP_LINE_CENTER);
	DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
	kbflush();

	ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
	if(0 == ucKey)
	{
		char loop = TIME_RETRY;
		unsigned char ps[SIZE_MAX_PWD + 1];
		while(loop > 0)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_CENTER);
			ucKey =  GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, ps, TIME_OUT_STANDARD_30S, FALSE);
			if(0xFF == ucKey || KEYCANCEL == ucKey)
				return 0xFF;
			if(KEYENTER == ucKey)
			{
				if(0 == strcmp(ps, gl_MaintenaceParam.stManage.szPwd))
				{
					ScrCls();
					DispString(gl_pLangSelPiont[STR_SERVICE],  1 | DISP_LINE_CENTER);
					DispString(gl_pLangSelPiont[STR_DEMARRE], DISPLAYLINE | DISP_LINE_CENTER);
					gl_bOnService =TRUE;
					Beep_Speaker(BEEP_OK);
					PubWaitKey(TIME_DELAY);
					ucRet = OK;
					break;
				}
				else
				{
					--loop;
					ScrCls();
					DispString("WRONG PWD.", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)
						return 0xFF;
				}
			}
		}
	}
	return ucRet;
}
static void Do_Off_Service(void)
{
	enum
	{
		STEP_LOCK = 0,
		STEP_INPUTPWD,
		STEP_PRINT,
		STEP_ERASE,
	};

	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_LOCK;
	char loop = TIME_RETRY;

	unsigned char szPwd[SIZE_MAX_PWD + 1];

	memset(szPwd, 0, sizeof(szPwd));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_LOCK:
			DispString(gl_pLangSelPiont[STR_ARRETER_SERVICE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
				ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
			{
				if(0 == strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
				{
					Do_Logout();
					ScrCls();
					DispString(gl_pLangSelPiont[STR_SERVICE],  1 | DISP_LINE_CENTER);
					DispString(gl_pLangSelPiont[STR_ARRETE], DISPLAYLINE | DISP_LINE_CENTER);
					gl_bOnService =FALSE;
					ucStep = STEP_PRINT;
					PubWaitKey(TIME_DELAY);
				}
				else
				{
					--loop;
					ScrCls();
					DispString("Wrong Pwd", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)
						return;
				}
			}
			break;
		case STEP_PRINT:
			DispString(gl_pLangSelPiont[STR_PRINT],  1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_REPORT_DAILY], DISPLAYLINE | DISP_LINE_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
			{
				Prn_SalePolling(CODE_FIN_DAILY);
				ucStep = STEP_ERASE;
			}
			break;
		case STEP_ERASE:
			DispString(gl_pLangSelPiont[STR_EFFACEMENT_DES],  1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_REPORTS], DISPLAYLINE | DISP_LINE_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
			{
				Erase_Report(FALSE);
				return;
			}
			break;
		}
		

		if(0xFF == ucKey || KEYCANCEL == ucKey)
		{
			return;
		}
	}
}
static void Do_OnOff_Service(void)
{
	if(gl_bOnService)
	{
		Do_Off_Service();
	}
	else
	{
		Do_On_Service();
	}
}

//////////////////////////////////////////////////////////////////////////

static void Do_PrintStock(unsigned char bDetail)
{
	ScrCls();
	DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
	DispString(gl_pLangSelPiont[STR_ON_GOING], DISPLAYLINE | DISP_LINE_CENTER);

	Prn_Stock(bDetail);
}
static void Do_DispStock(void)
{
	unsigned short usIndex = 0;
	unsigned char ucKey = 0;

	while(gl_SavParam.ucProductsNum > 0)	
	{
		ScrCls();
		kbflush();
		
		DispStock(usIndex);
		
		ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);

		if(NOKEY == ucKey || KEYCANCEL == ucKey)
			break;

		if(KEYUP == ucKey)
		{
			if(usIndex > 0)
				--usIndex;
			else
				Beep();
		}
		if(KEYDOWN == ucKey)
		{
			if(usIndex < gl_SavParam.ucProductsNum - 1)
				++usIndex;
			else
				Beep();
		}
	}
}
static void Do_Stocks(int sub)
{
	if(ONLINE == gl_MaintenaceParam.ucMerchantMode)
		return;

	ScrCls();
	DispString(gl_pLangSelPiont[STR_ACTUALISE_STOCK], DISP_SCR_CENTER);

	RefreshStock();
	switch(sub)
	{
	case ID_PrintGeneral:
		Do_PrintStock(FALSE);
		break;
	case ID_PrintDetail:
		Do_PrintStock(TRUE);
		break;
	case ID_DispStock:
		Do_DispStock();
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

static void Do_ReportCashier(void)
{
	enum
	{
		STEP_CHECKTRANS = 0,
		STEP_INPUTID,
		STEP_INPUTPWD,
		STEP_VERIFY,
		STEP_PRINT,
	};

	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_CHECKTRANS;
	unsigned char bVerify = FALSE;
	char loop = TIME_RETRY;

	USER_INFO stCurUser;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	memset(&szPwd, 0, sizeof(szPwd));
	memset(&stCurUser, 0, sizeof(USER_INFO));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_CHECKTRANS:
			if(gl_MaintenaceParam.bReportEmpty)
			{
				ucKey = 0xFF;
				DispString(gl_pLangSelPiont[STR_PAS_D_OPERATION], DISP_SCR_CENTER);
				PubWaitKey(TIME_DELAY);
				return;
			}
			else
			{
				ucStep = STEP_INPUTID;
			}
			break;
		case STEP_INPUTID:
			if(gl_bCashier_Login)
			{
				ucStep = STEP_INPUTPWD;
				strcpy(stCurUser.szName, gl_Cur_Cashier.szName);
				break;
			}
			DispString("Cashier ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN, 0, SIZE_CASHIER_NAME, stCurUser.szName, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_CASHIER], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurUser.szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucStep = STEP_VERIFY;
			break;
		case STEP_VERIFY:
			if(OK == ReadUsersPwd(stCurUser.szName, szPwd))
			{
				if(0 == strcmp(szPwd, stCurUser.szPwd))
				{
					bVerify = TRUE;
				}
			}

			if(bVerify)
			{
				ucStep = STEP_PRINT;
			}
			else
			{
				DispString("Wrong ID or PWD", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				if(loop > 0)
				{
					--loop;
					ucStep = STEP_INPUTPWD;
				}
				else
					ucKey = 0xFF;
				PubWaitKey(TIME_DELAY);
			}
			break;
		case STEP_PRINT:
			DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_ON_GOING], 1 | DISP_LINE_CENTER);
			Prn_ReportCashier(stCurUser);
			return;
		}

		if(0xFF == ucKey || KEYCANCEL == ucKey)
			return;
	}
}
static void Do_ReportManager(void)
{
	enum
	{
		STEP_CHECKTRANS = 0,
		STEP_INPUTPWD,
		STEP_VERIFY,
		STEP_PRINT,
	};

	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_CHECKTRANS;
	char loop = TIME_RETRY;

	unsigned char szPwd[SIZE_MAX_PWD + 1];
	memset(&szPwd, 0, sizeof(szPwd));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_CHECKTRANS:
			if(gl_MaintenaceParam.bReportEmpty)
			{
				ucKey = 0xFF;
				DispString(gl_pLangSelPiont[STR_PAS_D_OPERATION], DISP_SCR_CENTER);
				PubWaitKey(TIME_DELAY);
				return;
			}
			else
			{
				ucStep = STEP_INPUTPWD;
			}
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucStep = STEP_VERIFY;
			break;
		case STEP_VERIFY:

			if(0 == strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
			{
				//Do_Logout();
				ucStep = STEP_PRINT;
			}
			else
			{
				DispString("Wrong PWD", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				if(loop > 0)
				{
					--loop;
					ucStep = STEP_INPUTPWD;
				}
				else
					ucKey = 0xFF;
				PubWaitKey(TIME_DELAY);
			}
			break;
		case STEP_PRINT:
			DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_ON_GOING], 1 | DISP_LINE_CENTER);
			Prn_ReportCommer();
			if(!gl_MaintenaceParam.bAutoPrintReports)
				Erase_Report(FALSE);
			return;
		}

		if(0xFF == ucKey || KEYCANCEL == ucKey)
			return;
	}
}
static void Do_ReportServer(void)
{

}
static void Do_Reports(int sub)
{
	switch(sub)
	{
	case ID_ReportCashier:
		Do_ReportCashier();
		break;
	case ID_ReportManager:
		Do_ReportManager();
		break;
	case ID_ReportServer:
		Do_ReportServer();
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
static void Do_SettDownload()
{
	unsigned char ucRet = 0;
	unsigned char bChargementLogo = FALSE;
	unsigned char ucStep = 0;
	
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	SAV_PARAM st_sav_param_temp;

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	memset(&st_sav_param_temp, 0, sizeof(SAV_PARAM));

	// Modif gpx 
	bChargementLogo = gl_SavParam.bDownloadLogoToDo;
	
 	ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	ScrCls();
	DispString(gl_pLangSelPiont[STR_TELEPARAMETRAGE], 1 | DISP_LINE_LEFT);
	
	//ÏÂÔØ Parameter
	if(OK == ucRet)
	{
		int iRetryCnt = TIME_RETRY;
		++ucStep;
		DispString(gl_pLangSelPiont[STR_PARAM_DOWNLOAD], DISPLAYLINE | DISP_LINE_CENTER);
		do 
		{
			ucRet = Parameter_XML(&stRespH, &stRespB, FALSE);
			--iRetryCnt;
		} while (1 == ucRet && iRetryCnt > 0);
	}

	//  ÏÂÔØ  ²úÆ·Ä¿Â¼
	if(OK == ucRet)
	{
		int iRetryCnt = TIME_RETRY;
		unsigned int RequestIndex = gl_SavParam.uiProductsListIndex;
		unsigned char bContinue = FALSE;
		int fd;
		// Modified by Kim_LinHB 2013-9-12 v4.68
		if(0 == gl_SavParam.uiProductsListIndex)
		{
			remove(FILEPRODUCT_TEMP);
			fd = open(FILEPRODUCT_TEMP, O_CREATE | O_RDWR);
			if(fd < 0)
			{
				ScrCls();
				DispString("Open File error", DISP_SCR_CENTER);
				return;
			}
		}
		else
		{
			fd = open(FILEPRODUCT_TEMP, O_RDWR);
			if(fd < 0)
			{
				ScrCls();
				DispString("Open File error", DISP_SCR_CENTER);
				return;
			}
			seek(fd ,0, SEEK_SET);
			read(fd, (void *)&st_sav_param_temp, sizeof(SAV_PARAM));
		}

		++ucStep;
		DispString(gl_pLangSelPiont[STR_TABLE_PRODUCTS], DISPLAYLINE |DISP_LINE_CENTER);

		do
		{
			iRetryCnt = TIME_RETRY;
			do{
				ucRet = CategoryPackageList_XML(&bContinue, RequestIndex, &stRespH, &stRespB, FALSE, &st_sav_param_temp);
				--iRetryCnt;
			}while(1 == ucRet && iRetryCnt > 0);
			if(OK == ucRet)
				++RequestIndex;
		}while (OK == ucRet && bContinue);

		if(OK ==ucRet && !bContinue)
		{	
			gl_SavParam.uiProductsListIndex = 0;
			close(fd);
			remove(FILEPRODUCT_TEMP);
		}
		else
		{
			gl_SavParam.uiProductsListIndex = RequestIndex;
			seek(fd ,0, SEEK_SET);
			write(fd, (void *)&st_sav_param_temp, sizeof(SAV_PARAM));
			close(fd);
		}
	}

	//  ÏÂÔØ  ¿â´æ
	if(OK == ucRet)
	{
		int iRetryCnt = TIME_RETRY;
		++ucStep;
		if(-1 != fexist(FILE_USERS_TMP))
			remove(FILE_USERS_TMP);
		DispString("Table Users", DISPLAYLINE |DISP_LINE_CENTER);
		do 
		{
			if(-1 != fexist(FILE_USERS))
				remove(FILE_USERS);
			ucRet = Get_Users_XML(&stRespH, &stRespB, FALSE);
			--iRetryCnt;
		} while (1 == ucRet && iRetryCnt > 0);
	}

	if (OK == ucRet)
	{
		++ucStep;
		gl_bSettDown = 1;
	}

	ScrCls();
	if(OK == ucRet)
	{
		if(1 == gl_SavParam.bIsSetup)
		{
			gl_SavParam.bIsSetup = 0;
		}

		UpdateProductLsit(&gl_SavParam, st_sav_param_temp);

		DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_TELEPARAMETRAGE], 4 | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_OK);
		//GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time); // Hided by Kim_LinHB 2013-3-20 v4.56
	}
	else
	{
		DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_ABANDON], 4 | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_ERR);
	}

	//if(gl_bPlanned_SettDownload)
	//	GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time); // Added by Kim_LinHB 2013-3-20 v4.56
	WriteParam(WRT_ALL);
	Prn_SettDownload(ucRet, stRespH);

	Do_GetMail(FALSE); // Added by Kim_LinHB 2014-1-23 v4.81
	CloseConn(); // Added by Kim_LinHB 2014/11/4

	// Added by Kim_LinHB 2013-9-11 v4.68
	if(OK == ucRet)
	{
#ifdef LHB_TEST
		strcpy(gl_MaintenaceParam.stFTP.szUser, "lhb");
		strcpy(gl_MaintenaceParam.stFTP.szPwd, "11223344");
		strcpy(gl_MaintenaceParam.stFTP.szRemoteIP, "192.168.1.3");
		strcpy(gl_MaintenaceParam.stFTP.szRemotePort, "21");
		strcpy(gl_MaintenaceParam.stFTP.szFilePath, "/merchant/");
		strcpy(gl_MaintenaceParam.stFTP.stApp.szFileName, "Merchant_4.92.bin");
		strcpy(gl_MaintenaceParam.stFTP.stApp.szFileVer, "4.92");
		strcpy(gl_MaintenaceParam.stFTP.stParam[0].szFileName, "AR_PRNDISP.FNT");
		strcpy(gl_MaintenaceParam.stFTP.stParam[0].szFileVer, "4.92");
		gl_MaintenaceParam.stFTP.bForceToDownload = 0;
#endif
		if(gl_MaintenaceParam.stFTP.bForceToDownload)
			Do_SoftDownLoad(FALSE);  // Modified by Kim_LinHB 2013-9-26 v4.71
	}

	// Add End
}

static unsigned char Do_SalesPolling(unsigned char bAuto, char bAutoConn)
{
	unsigned char ucRet = OK;
	unsigned int uiSoldCnt = 0, uiExpiry = 0, uiBurned = 0, uiSelling = 0;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	ScrCls();
	DispString("Checking", DISP_SCR_CENTER);
#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,Step0",__FILE__,__LINE__,__FUNCTION__);
#endif
	if(IsSalesPollingEmpty(&uiSoldCnt, &uiExpiry, &uiBurned, &uiSelling))
	{
#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,Step1,bAuto=%d",__FILE__,__LINE__,__FUNCTION__,bAuto);
#endif
		if (!bAuto)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PAS_D_OPERATION], DISP_SCR_CENTER); //"No Transaction"
			Beep_Speaker(BEEP_ERR);
			PubWaitKey(TIME_DELAY);
		}
		//GetCurTime(gl_MaintenaceParam.szLastSalesPolling_Time); // Added by Kim_LinHB 2013-3-20 v4.56
		return OK;
	}

	// Removed by Kim_LinHB 2013-11-14 v4.73
// 	if(!bAuto)
// 	{
// 		char loop = TIME_RETRY;
// 		unsigned char ucKey = 0;
// 		unsigned char szPwd[SIZE_MAX_PWD + 1];
// 		memset(szPwd, 0, sizeof(szPwd));
// 		
// 		while(loop > 0)
// 		{
// 			ScrCls();
// 			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(strcmp(szPwd, gl_MaintenaceParam.stManage.szPwd))
// 				{
// 					--loop;
// 					ScrCls();
// 					DispString("Wrong PWD", DISP_SCR_CENTER);
// 					Beep_Speaker(BEEP_ERR);
// 					PubWaitKey(TIME_DELAY);
// 					if(loop <= 0)
// 						return 0xFF;
// 				}
// 				else
// 				{
// 					Do_Logout();
// 					break;
// 				}
// 			}
// 			else
// 				return 0xFF;
// 		}
// 	}

	ScrCls();
	DispString(gl_pLangSelPiont[STR_SALESPOLLING], 1 | DISP_LINE_CENTER);
#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,Step2",__FILE__,__LINE__,__FUNCTION__);
#endif
	if(OK == ucRet)
	{
		int cnt = 3;
		while(cnt-- >0)
		{
			ucRet = SalesPolling_XML(bAuto, uiSoldCnt + uiExpiry, &stRespH, &stRespB, bAutoConn);
			if(ucRet != 1)
				break;
		}
 		if(cnt <= 0)
		{
			TRANS_DATA stCurTran;
			stCurTran.ucTransType = ID_SalesPolling;
 			//Cancellation_XML(stCurTran, &stRespH, &stRespB, TRUE);//cancel;
		}

	}

	ScrCls();
	if(OK == ucRet)
	{
#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,Step3,uiSoldCnt=%d,uiExpiry=%d,uiBurned=%d,uiSelling=%d"
			,__FILE__,__LINE__,__FUNCTION__,uiSoldCnt,uiExpiry,uiBurned,uiSelling);
#endif
		if(uiSoldCnt > 0)
		{
			int i;
			STOCK tmp;
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_SALESPOLLING], DISPLAYLINE | DISP_LINE_CENTER);
			// Added by Kim_LinHB 2013-7-6 v4.62
			ReadStock_All(&tmp);
			for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
			{
				if(CODE_RESTOCK_SOLD == tmp.stStock[i].ucProductState)
					tmp.stStock[i].ucProductState = CODE_RESTOCK_SALESPOLLING;
			}
			WriteStock_All(&tmp);
			// Add End
			Prn_SalePolling(CODE_RESTOCK_SALESPOLLING);
			Erase_Salepolling(CODE_RESTOCK_SALESPOLLING);
		}
		if(uiExpiry > 0)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PINS_EXPIRED], DISPLAYLINE | DISP_LINE_CENTER);
			Prn_SalePolling(CODE_RESTOCK_EXPIRED);
			Erase_Salepolling(CODE_RESTOCK_EXPIRED);
		}
		if(uiBurned > 0)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PINS_USED], DISPLAYLINE | DISP_LINE_CENTER);
			Prn_SalePolling(CODE_RESTOCK_BURNED);
			Erase_Salepolling(CODE_RESTOCK_BURNED);
		}
 		if(uiSelling > 0)
 		{
 			ScrCls();
 			DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
 			DispString(gl_pLangSelPiont[STR_ANOMALIE_VENTE], DISPLAYLINE | DISP_LINE_CENTER);
 			Prn_SalePolling(CODE_RESTOCK_SELLING);
 			Erase_Salepolling(CODE_RESTOCK_SELLING);
 		}
		// Added by Kim_LinHB 2013-7-6 v4.62
		SortStock();
		// Add End
		//GetCurTime(gl_MaintenaceParam.szLastSalesPolling_Time); // Hided by Kim_LinHB 2013-3-20 v4.56
		gl_MaintenaceParam.bReportEmpty = TRUE; // Added by Kim_LinHB 2013-4-3 v4.57
		WriteParam(WRT_MAINTA);
	}
	else
	{
		Do_Error(ucRet, stRespH);
	}

	//if(gl_bPlanned_SalesPolling)
	//GetCurTime(gl_MaintenaceParam.szLastSalesPolling_Time); // Added by Kim_LinHB 2013-3-20 v4.56
	if(ucRet != OK)
		ucRet = PbSalesPolling;
	return ucRet;
}
static void Do_CancalSale()
{
	unsigned char ucRet = OK, ucKey = 0;
	unsigned long ulCurDate, ulTranDate;
	unsigned char szCurDate[SIZE_DATE_TIME + 1 + 2];
	unsigned char szTranDate[SIZE_DATE_TIME + 1 + 2];

	TRANS_DATA stTran;
	TRANS_DATA_CASH stTranCash;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	unsigned char bOrder = FALSE;
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	memset(&stTran, 0, sizeof(TRANS_DATA));
	memset(&stTranCash, 0, sizeof(TRANS_DATA_CASH));

	if (!gl_MaintenaceParam.bCancelSale)  //v3.0
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_ANNUL_VENTE], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_NON_DISPONIBLE], DISPLAYLINE | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_ERR);
		PubWaitKey(TIME_DELAY);
		return;
	}

	if(!gl_bCashier_Login)
		if(Do_Login() != OK)
			return;

	if(0 == filesize(FILE_TRANS_OL) && 0 == filesize(FILE_CASHORDER)) // Modified by Kim_LinHB 2013-6-17 v4.60
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_PAS_D_OPERATION], DISP_SCR_CENTER);
		PubWaitKey(TIME_DELAY);
		return;
	}

	ScrCls();
	DispString("Provision Id:", 1 | DISP_LINE_LEFT);
	ucKey = GetInput(DISPLAYLINE, NUM_IN, 0, SIZE_MAX_PROVISION_ID, stTran.szProvisionID, TIME_OUT_STANDARD_30S, TRUE);
	if(ucKey != KEYENTER)
		return;

	if(FindOLTranWithProvisionId(&stTran)!= OK)
	{
		strcpy(stTranCash.szProvisionID, stTran.szProvisionID);
		if(FindCashOrderWithProvisionId(&stTranCash)!= OK)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PAS_D_OPERATION], DISP_SCR_CENTER);
			PubWaitKey(TIME_DELAY);
			return;
		}
		else
		{
			bOrder = TRUE;
			strcpy(stTran.szProvisionID, stTranCash.szProvisionID);
			stTran.usTraceNo = stTranCash.usTraceNo;
			strcpy(stTran.szDateTime, stTranCash.szDateTime);
			stTran.ucTransType = stTranCash.ucTransType;
		}
	}

	strcpy(szCurDate, PRE_YEAR);
	strcpy(szTranDate, PRE_YEAR);
	strcat(szTranDate, stTran.szDateTime);
	GetCurTime(szCurDate + 2);
	ulCurDate = PubTime2Long(szCurDate);
	ulTranDate = PubTime2Long(szTranDate);

	if(ulCurDate > ulTranDate &&
		ulCurDate - ulTranDate > 60 * TIME_CANCL_LIMIT)
	{
		ScrCls();
		DispString("Too Old", DISP_SCR_CENTER);
		Beep_Speaker(BEEP_ERR);
		PubWaitKey(TIME_DELAY);
		return;
	}

	if(!bOrder)
		Prn_CancelSale(stTran, TRUE);
	else
		Prn_CancelCashOrder(stTranCash, TRUE);

	ScrCls();
	DispString(gl_pLangSelPiont[STR_ANNULER], 1 | DISP_LINE_CENTER);
	DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
	if(0 == PubYesNo(TIME_OUT_STANDARD_30S))
	{
		int iRetryCnt = TIME_RETRY;
		do 
		{
			ucRet = Cancellation_XML(stTran, FALSE, &stRespH, &stRespB, TRUE);//cancel;
			--iRetryCnt;
		} while (1 == ucRet && iRetryCnt > 0);

		ScrCls();
		if(OK == ucRet)
		{
			if(!bOrder)
			{
				DelOLTranByProvisionId(stTran);
				Prn_CancelSale(stTran, FALSE);
			}
			else
			{
				DelCashOrderByProvisionId(stTranCash);
				Prn_CancelCashOrder(stTranCash, FALSE);
			}
		}
		else
		{
			Do_Error(ucRet, stRespH);
		}
	}
}
static void Do_Claim()
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_CATEGORY,
		STEP_PRODUCT,
		STEP_SERIAL,
		STEP_UPLOADATA,
		STEP_PRINT,
		STEP_NEWORDER,
	};

	int iRet;
	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_CATEGORY;
	char loop = TIME_RETRY;
	int uiCategory_Code = -1;
	STOCK_INFO stock_info;
	VOUCHER_INFO voucher;

	unsigned char szPwd[SIZE_MAX_PWD + 1];

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	memset(szPwd, 0, sizeof(szPwd));
	
	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
			{
				if(0 == strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
				{
					//Do_Logout();
					ucStep = STEP_CATEGORY;
				}
				else
				{
					--loop;
					ScrCls();
					DispString("Wrong Pwd", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)
						return;
				}
			}
			break;
		case STEP_CATEGORY:
			if(gl_SavParam.ucCategoryNum <= 0)
			{
				ScrCls();
				DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return;
			}
			uiCategory_Code = SelCateory(gl_SavParam, PRODUCT_ELECTRONIC);
			if(uiCategory_Code > 0)
				ucStep = STEP_PRODUCT;
			else
				return;
			break;
		case STEP_PRODUCT:
			iRet = SelProduct(&gl_SavParam, stCurTran.stVoucher.szProductId, uiCategory_Code);
			if(OK == iRet)
			{
				ucKey = KEYENTER;
				ucStep = STEP_SERIAL;
			}
			else if (-1 == iRet)
			{
				ucStep = STEP_CATEGORY;
			}
			else if (-2 == iRet)
				return;
			break;
		case STEP_SERIAL:
			DispString(gl_pLangSelPiont[STR_No_SERIE], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN, 0, SIZE_MAX_VOUCHER_SERIAL, 
				stCurTran.stVoucher.szSerial, TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				ucStep = STEP_UPLOADATA;
			else if (KEYCANCEL == ucKey)
				ucStep = STEP_PRODUCT;
			break;
		case STEP_UPLOADATA:
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;

			stCurTran.ucTransType = ID_Claim;
			GetCurTime(stCurTran.szDateTime);

			iRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

			if(OK == iRet)
			{
				int cnt = 3;
				while(cnt-- >0)
				{
					iRet = Claim_XML(stCurTran.stVoucher.szSerial, &stRespH, &stRespB, FALSE);
					if(iRet != 1)
						break;
				}
// 				if(cnt <= 0)
// 					Cancellation_XML(stCurTran, &stRespH, &stRespB, TRUE);//cancel;
			}

			CloseConn();
			if(OK == iRet)
			{
				unsigned int i;
				for(i = 0; i < gl_SavParam.ucProductsNum; ++i)
				{
					if(0 == strcmp(stCurTran.stVoucher.szProductId, gl_SavParam.stSavParamProducts[i].szCodeProduct))
					{
						gl_SavParam.stClaim.ulClaimAmount += atoi(gl_SavParam.stSavParamProducts[i].szAmount);
						++gl_SavParam.stClaim.usNbClaim;
						break;
					}
				}

				for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
				{
					ReadStock(i, &stock_info, &voucher);
					if(0 == strcmp(stCurTran.stVoucher.szSerial, voucher.szSerial))
					{
						stock_info.ucProductState = CODE_RESTOCK_BURNED;
						WriteStock(i, &stock_info, NULL);
						break;
					}
				}
			}
			WriteParam(WRT_SAVE);

			strcpy(stCurTran.szCashier, gl_Cur_Cashier.szName);
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == iRet)
			{
				DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_ENVOI_RECLAMATION], DISPLAYLINE | DISP_LINE_CENTER);
				Prn_Reclamation(stCurTran);//print
			}
			else
			{
				Do_Error(iRet, stRespH);
			}
			return;
		}

		if(0xFF == ucKey)
			return;
	}
}
static void Do_GetMail(char bAutoConn)
{
	unsigned char ucRet = OK;
	MAIL stMail[20];
	unsigned short Size = 0;
	int iRetryCnt = TIME_RETRY;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

#ifdef LHB_TEST
// 	int CCnt = 0;
// 	strcpy(stMail[0].szCaption, "STC");
// 	stMail[0].usContentNums = 11;
// 	strcpy(stMail[0].szContent[CCnt++], "ÒÏ ãÈíÚÇÊß¡ íÒíÏ ÑÈÍß");
// 	strcpy(stMail[0].szContent[CCnt++], "\xC5\xDF\xD3\xC8\xD1\xED\xD3\x20\x20\x53\x54\x43\x20\xD2\xCF\x20\xE3\xC8\xED\xDA\xC7\xCA\xDF");
// 	strcpy(stMail[0].szContent[CCnt++], "ãÚ ÞÓÇÆ?ÔÍ?");
// 	strcpy(stMail[0].szContent[CCnt++], "?ÇáÝÇÆÒ ÈæÇÍÏÉ ãä 12  ?);
// 	strcpy(stMail[0].szContent[CCnt++], "íÇÑÉ ßí?ÈíßÇäÊ?);
// 	strcpy(stMail[0].szContent[CCnt++], "Sell Big. Win");
// 	strcpy(stMail[0].szContent[CCnt++], "Big! Make big sales on");
// 	strcpy(stMail[0].szContent[CCnt++], "EVD data           vo");
// 	strcpy(stMail[0].szContent[CCnt++], "uchers and you could wi");
// 	strcpy(stMail[0].szContent[CCnt++], "n big with 12 Kia Pican");
// 	strcpy(stMail[0].szContent[CCnt], "tos to be won!");
// 	Prn_Mail((MAIL *)&stMail, 1);
// 	return;
#endif

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

// 	if(!gl_bCashier_Login)
// 	{
// 		DispString("Pls log in", DISP_SCR_CENTER);
// 		PubWaitKey(TIME_DELAY);
// 		return;
// 	}
	
	do 
	{
		ucRet = Get_Mail_XML((MAIL *)&stMail, &Size, &stRespH, &stRespB, bAutoConn);
		--iRetryCnt;
	} while (1 == ucRet && iRetryCnt > 0);

	ScrCls();
	if(OK == ucRet)
	{
		DispString(gl_pLangSelPiont[STR_TICKET_CLIENT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_ON_GOING], DISPLAYLINE | DISP_LINE_CENTER);
		Prn_Mail((MAIL *)&stMail, Size);//print
	}
	else
	{
		Do_Error(ucRet, stRespH);
	}

}
static void Do_SoftDownLoad(unsigned char bAuto)
{
	int iRet;

	ScrCls();
// Modified by Kim_LinHB 2013-9-11 v4.68
	if(strcmp(gl_MaintenaceParam.stFTP.stApp.szFileVer, STR_VERSION_SUB) > 0)	
	{
		gl_MaintenaceParam.stFTP.bForceToDownload = 0;
		WriteParam(WRT_MAINTA);
		DispString("New Ver Available", 1 | DISP_LINE_CENTER | DISP_LINE_REVER);
		DispString("Update", 3 | DISP_LINE_LEFT);
		DelayMs(2000);
	}
	else
	{
		// Modified by Kim_LinHB 2014-2-6 v4.84
		if(!bAuto)
		{
			DispString("Update", 0 | DISP_LINE_CENTER | DISP_LINE_REVER);
		}
		else
		{
			GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
			memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
			WriteParam(WRT_MAINTA);
			DispString("Auto Update", 0 | DISP_LINE_CENTER | DISP_LINE_REVER);
		}
		ScrPrint(10, 3, ASCII, "Your ver is already \nthe latest ver.");
		PubWaitKey(3);
		return;
	}

	if(!bAuto && (-1 != fexist(FILE_UPDATE)))
	{
		unsigned char bOK = FALSE;
		DispString("1 Continue(Def)", 1| DISP_LINE_LEFT);
		DispString("2 Restart",		  3| DISP_LINE_LEFT);
		while(!bOK)
		{
			unsigned char ucKey = PubWaitKey(10);
			switch(ucKey)
			{
			case KEY1:
			case KEYENTER:
			case NOKEY:
				bOK = TRUE;
				break;
			case KEY2:
				remove(FTPFILE);
				remove(FILE_UPDATE);
				bOK = TRUE;
				break;
			case KEYCANCEL:
				return;
			default:
				continue;
			}
		}
	}
// Modify End

	iRet = FTPConnect(gl_MaintenaceParam_Ext.cConnectionTimeout);
	if (iRet != OK)
	{
		ScrCls();
		//DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
		ScrPrint(0, 3 , 1, "Error:%d", iRet);
		vTrace("!! FTPConnect KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
		FTPCloseConn(TRUE);
		PubWaitKey(TIME_DELAY);
		return;
	}

	iRet = FTPLogin(gl_MaintenaceParam.stFTP);
	if (iRet != OK)
	{
		ScrCls();
		//DispString(gl_szFTPErrInfo, DISP_SCR_CENTER); // Removed by Kim_LinHB 2013-7-10 v4.62
		ScrPrint(0, 2, 1, "%s", gl_szFTPErrInfo); // Added by Kim_LinHB 2013-7-10 v4.62
		vTrace("!! FTPLogin KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
		FTPCloseConn(TRUE);
		PubWaitKey(TIME_DELAY);
		return;
	}

	if (fexist(FILE_UPDATE) < 0 || 
		0 == strcmp(gl_stFTPFileInfo.szFileName, gl_MaintenaceParam.stFTP.stApp.szFileName))
	{
		unsigned char szPath[255];
		if (gl_MaintenaceParam.stFTP.szFilePath[strlen(gl_MaintenaceParam.stFTP.szFilePath) - 1] != '/')
			sprintf(szPath, "%s/%s", gl_MaintenaceParam.stFTP.szFilePath, gl_MaintenaceParam.stFTP.stApp.szFileName);
		else
			sprintf(szPath, "%s%s", gl_MaintenaceParam.stFTP.szFilePath, gl_MaintenaceParam.stFTP.stApp.szFileName);
		ScrCls();
		DispString("Downloading", 0 | DISP_LINE_CENTER | DISP_LINE_REVER);
		DispString(gl_MaintenaceParam.stFTP.stApp.szFileName, 3 | DISP_LINE_LEFT);
		iRet = FTPGetFile(szPath, FILE_UPDATE, 5);
		if (iRet != OK)
		{
			ScrCls();
			//DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
			ScrPrint(0, 2, 1, "%s", gl_szFTPErrInfo);
			vTrace("!! FTPGetFile App KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			FTPCloseConn(TRUE);
			PubWaitKey(TIME_DELAY);
			return;
		}
	}

	if (fexist("AR_PRNDISP.FNT") < 0 || 
		0 == strcmp(gl_stFTPFileInfo.szFileName, "AR_PRNDISP.FNT"))
	{
		unsigned char szPath[255];
		if (gl_MaintenaceParam.stFTP.szFilePath[strlen(gl_MaintenaceParam.stFTP.szFilePath) - 1] != '/')
			sprintf(szPath, "%s/%s", gl_MaintenaceParam.stFTP.szFilePath, gl_MaintenaceParam.stFTP.stParam[0].szFileName);
		else
			sprintf(szPath, "%s%s", gl_MaintenaceParam.stFTP.szFilePath, gl_MaintenaceParam.stFTP.stParam[0].szFileName);
		ScrCls();
		DispString("Downloading", 0 | DISP_LINE_CENTER | DISP_LINE_REVER);
		DispString(gl_MaintenaceParam.stFTP.stParam[0].szFileName, 3 | DISP_LINE_LEFT);
		iRet = FTPGetFile(	szPath, "AR_PRNDISP.FNT", 5);
		if (iRet != 0)
		{
			ScrCls();
			//DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
			ScrPrint(0, 2, 1, "%s", gl_szFTPErrInfo);
			vTrace("!! FTPGetFile Param KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);

			FTPCloseConn(TRUE);
			PubWaitKey(TIME_DELAY);
			return;
		}
	}

	vTrace("-> DownloadSoftware Done OK", NULL, 0, TRACE_WARNING, TYPE_N);

	FTPCloseConn(FALSE);
	if(bAuto)	// Modified by Kim_LinHB 2013-9-26 v4.71 avoid effect of forcetodownload == 1
	{
		GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
		memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
	}
	// Added by Kim_LinHB 2013-9-3 v4.67 for setting download after remote download
	memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
	//strcpy(gl_MaintenaceParam.szTimeSettDownload, "0000");
	// Add End
	WriteParam(WRT_MAINTA);

#ifndef _WIN32
	iRet = FileToApp(FILE_UPDATE);
	if(iRet != 0)
	{
		ScrCls();
		DispString("FileToApp KO", DISP_SCR_CENTER);
		vTrace("!! FileToApp KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
		PubWaitKey(TIME_DELAY);
		remove(FILE_UPDATE);
		return;
	}

	iRet = FileToParam("AR_PRNDISP.FNT", APP_NAME, 1);
	if(iRet != 0)
	{
		ScrCls();
		DispString("FileToParam KO", DISP_SCR_CENTER);
		vTrace("!! FileToParam KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
		PubWaitKey(TIME_DELAY);
		remove("AR_PRNDISP.FNT");
		return;
	}
#endif

	while(1)
	{
		ScrCls();
		DispString("Update Completed", 1 | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_NORMAL);
		DispString("Pls ReBoot!", DISPLAYLINE | DISP_LINE_CENTER);
#ifdef _S_SERIES_
		Reboot();
#endif
#ifndef _WIN32
		DelayMs(1000);
		__asm__("SWI 0x03");  //v4.11
#endif
		getkey();
	}

	return;
}

static void Do_LogoDownLoad()
{

}

static void Do_Balance()
{
	unsigned char ucRet = OK, ucKey = 0;
	unsigned char szValue[12 + 1];
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	char loop = TIME_RETRY;
	int iRetryCnt = TIME_RETRY;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	memset(szPwd, 0, sizeof(szPwd));

	if(OFFLINE == gl_MaintenaceParam.ucMerchantMode)
		return;

	while(loop > 0)
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

		ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
		if(KEYENTER == ucKey)
		{
			if(strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
			{
				--loop;
				ScrCls();
				DispString("Wrong Pwd", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				if(loop <= 0)
					return;
			}
			else
			{
				Do_Logout();
				break;
			}
		}
		else
			return;
	}

	do 
	{
		ucRet = Get_Balance_XML(szValue, &stRespH, &stRespB, TRUE);
		--iRetryCnt;
	} while (1 == ucRet && iRetryCnt > 0);

	ScrCls();
	if(OK == ucRet)
	{
		DispString(gl_pLangSelPiont[STR_TICKET_CLIENT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_ON_GOING], DISPLAYLINE | DISP_LINE_CENTER);
		//WriteParam();
		Prn_Balance(szValue);//print
	}
	else
	{
		Do_Error(ucRet, stRespH);
	}
}

static void Do_Transmission(int sub)
{
	switch(sub)
	{
	case ID_SettingsDownload:
#ifdef LHB_TEST
	MyTest();
#endif
		Do_SettDownload();
		break;
	case ID_SalesPolling:
		Do_SalesPolling(gl_bPlanned_SalesPolling,TRUE);
		break;
	case ID_CancelSale:
		Do_CancalSale();
		break;
	case ID_Claim:
		// Added by Kim_LinHB 2013-9-11 v4.68
		Do_SalesPolling(TRUE,TRUE);
		// Add End
		Do_Claim();
		break;
	case ID_GetMail:
		Do_GetMail(TRUE);
		break;
	case ID_SoftDownload:
		Do_SoftDownLoad(gl_bPlanned_SoftDownload);
		break;
	case ID_LogoDownload:
		Do_LogoDownLoad();
		break;
	case ID_Balance:
		Do_Balance();
		break;
	//case ID_Config_Auto:
	//	break;
	//case ID_First_Init__Auto:
	//	break;
	}
}
//////////////////////////////////////////////////////////////////////////

static unsigned char Do_Login(void)
{
	enum
	{	STEP_CHECKSERVICE = 0,
		STEP_INPUTID,
		STEP_INPUTPWD,
		STEP_CHECK,
	};
	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_CHECKSERVICE;
	USER_INFO stTempUser;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	unsigned char bMatch = FALSE;
	char loop = TIME_RETRY;

	memset(&stTempUser, 0, sizeof(USER_INFO));
	memset(szPwd, 0, sizeof(szPwd));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_CHECKSERVICE:
			if(gl_bOnService)
				ucStep = STEP_INPUTID;
			else
			{
				if(OK == Do_On_Service())
					ucStep = STEP_INPUTID;
				else
					ucKey = 0xFF;
			}
			break;
		case STEP_INPUTID:
			PubShowTitle(TRUE, gl_pLangSelPiont[STR_CASHIER_LOGIN]);
			DispString(gl_pLangSelPiont[STR_USERNAME], 2 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN, 0, SIZE_CASHIER_NAME, stTempUser.szName, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucStep = STEP_INPUTPWD;
			else if(KEYCANCEL == ucKey)
				ucKey = 0xFF;
			break;
		case STEP_INPUTPWD:
			PubShowTitle(TRUE, gl_pLangSelPiont[STR_CASHIER_LOGIN]);
			DispString(gl_pLangSelPiont[STR_PASSWORD], 2 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stTempUser.szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucStep = STEP_CHECK;
			else if (KEYCANCEL == ucKey)
				ucStep = STEP_INPUTID;
			break;
		case STEP_CHECK:
			if(OK == ReadUsersPwd(stTempUser.szName,szPwd))
			{
				if (0 == strcmp(stTempUser.szPwd, szPwd))
					bMatch = TRUE;	
			}
			if(bMatch)
			{
				DispString("Logged in", DISP_SCR_CENTER);
				memcpy(&gl_Cur_Cashier, &stTempUser, sizeof(USER_INFO));
				gl_bCashier_Login = TRUE;
				Beep_Speaker(BEEP_OK);
				PubWaitKey(TIME_DELAY);
				return OK;
			}
			else
			{
				--loop;
				DispString("Wrong ID or PWD", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				if (loop <= 0)
					return !OK;
				else
					ucStep = STEP_INPUTPWD;
			}
		}
		if(0xFF == ucKey)
			return !OK;
	}
}
static void Do_Logout(void)
{
	if(gl_bCashier_Login)
	{
		ScrCls();
		DispString(gl_Cur_Cashier.szName, 1 | DISP_LINE_CENTER);
		DispString("Logged out", DISPLAYLINE | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_OK);
		memset(&gl_Cur_Cashier, 0, sizeof(USER_INFO));
		gl_bCashier_Login = FALSE;
		PubWaitKey(TIME_DELAY);
	}
}
static void Do_ChangePwd(void)
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_JUDGE,
		STEP_INPUTID,
		STEP_INPUTNEWPWD,
		STEP_INPUTNEWPWDAGAIN,
		STEP_VERIFY,
		STEP_UPLOAD,
		STEP_SYNC,
		STEP_CHANGE,
	};
	unsigned char ucRet, ucKey;
	unsigned char ucSel = STEP_INPUTPWD;
	char loop = TIME_RETRY;

	USER_INFO stByWho, stWho;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	unsigned char szPwdNew[SIZE_MAX_PWD + 1];
	unsigned char szPwdNew2[SIZE_MAX_PWD + 1];

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	memset(&stByWho, 0, sizeof(USER_INFO));
	memset(&stWho, 0, sizeof(USER_INFO));
	memset(szPwd, 0, sizeof(szPwd));
	memset(szPwdNew, 0, sizeof(szPwdNew));
	memset(szPwdNew2, 0, sizeof(szPwdNew2));

	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucSel)
		{
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stByWho.szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucSel = STEP_JUDGE;
			break;
		case STEP_JUDGE:
			if (0 == strcmp(gl_MaintenaceParam.stManage.szPwd, stByWho.szPwd))
			{
				Do_Logout();
				strcpy(stByWho.szName,gl_MaintenaceParam.stManage.szName);
				ucSel = STEP_INPUTID;
			}
			else
			{
				--loop;
				ScrCls();
				DispString("Wrong Pwd", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				if (loop <= 0)
					return;
				else
					ucSel = STEP_INPUTPWD;
			}
			break;
		case STEP_INPUTID://just for manager
			DispString("Cashier ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 0, SIZE_CASHIER_NAME, stWho.szName, TIME_OUT_STANDARD_30S, FALSE);
			if (KEYENTER == ucKey)
			{
				if(OK == ReadUsersPwd(stWho.szName,NULL))
				{
				ucSel = STEP_INPUTNEWPWD;
				}
				else
				{
					ScrCls();
					DispString("INVALID CASHIER.", DISP_SCR_CENTER);
					PubWaitKey(TIME_DELAY);
					return;
				}
			}
			else if(KEYCANCEL == ucKey)
				ucKey = 0xFF;
			break;
		case STEP_INPUTNEWPWD:
			DispString("New Pwd:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucSel = STEP_INPUTNEWPWDAGAIN;
			else if(KEYCANCEL == ucKey)
				ucSel = STEP_INPUTID;
			break;
		case STEP_INPUTNEWPWDAGAIN:
			DispString("New Pwd again:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew2, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucSel = STEP_VERIFY;
			else if(KEYCANCEL == ucKey)
				ucSel = STEP_INPUTID;
			break;
		case STEP_VERIFY:
			if (0 == strcmp(szPwdNew, szPwdNew2))
			{
				strcpy(stWho.szPwd, szPwdNew);
				ucSel = STEP_UPLOAD;
			}
			else
			{
				ScrCls();
				DispString("Don't Match.", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				return;
			}
			break;
		case STEP_UPLOAD:
			do 
			{
				ucRet = Change_Pwd_XML(stByWho, stWho, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);

			ucSel = STEP_SYNC;
			break;
		case STEP_SYNC:
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				if(-1 != fexist(FILE_USERS_TMP))
					remove(FILE_USERS_TMP);
				DispString("Table Users", DISPLAYLINE |DISP_LINE_CENTER);
				do 
				{
					if(-1 != fexist(FILE_USERS))
						remove(FILE_USERS);
					ucRet = Get_Users_XML(&stRespH, &stRespB, TRUE);
					--iRetryCnt;
				} while (1 == ucRet && iRetryCnt > 0);
				ucSel = STEP_CHANGE;
			}
			else
			{
				Do_Error(ucRet, stRespH);
				PubWaitKey(TIME_DELAY);
				return;
			}
			break;
		case STEP_CHANGE:
			if(OK == ucRet)
			{
				DispString("Success.", DISP_SCR_CENTER);
// 				WriteUsers(stWho.szName, stWho.szPwd);
// 				if (gl_bCashier_Login)
// 					strcpy(gl_Cur_Cashier.szPwd, stWho.szPwd);
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			PubWaitKey(TIME_DELAY);
			return;
		}
		if(0xFF == ucKey)
			return;
	}
}
static void Do_CreateUser(void)
{
	enum
	{
		//STEP_INPUTMANAGERPWD = 0,
		//STEP_JUDGE,
		STEP_INPUTID = 0,
		STEP_INPUTNEWPWD,
		STEP_INPUTNEWPWDAGAIN,
		STEP_VERIFY,
		STEP_UPLOAD,
		STEP_SYNC,
		STEP_CHANGE,
	};
	unsigned char ucRet, ucKey;
	unsigned char ucSel = STEP_INPUTID;//STEP_INPUTMANAGERPWD;
	//char loop = TIME_RETRY;

	USER_INFO stWho;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	unsigned char szPwdNew[SIZE_MAX_PWD + 1];
	unsigned char szPwdNew2[SIZE_MAX_PWD + 1];

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	memset(&stWho, 0, sizeof(USER_INFO));
	memset(szPwd, 0, sizeof(szPwd));
	memset(szPwdNew, 0, sizeof(szPwdNew));
	memset(szPwdNew2, 0, sizeof(szPwdNew2));

	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucSel)
		{
// 		case STEP_INPUTMANAGERPWD:
// 			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, 0, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 				ucSel = STEP_JUDGE;
// 			break;
// 		case STEP_JUDGE:
// 			if (0 == strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
// 			{
// 				Do_Logout();
// 				ucSel = STEP_INPUTID;
// 			}
// 			else
// 			{
// 				--loop;
// 				ScrCls();
// 				DispString("Wrong Pwd", DISP_SCR_CENTER);
// 				Beep_Speaker(BEEP_ERR);
// 				PubWaitKey(TIME_DELAY);
// 				if(loop <= 0)
// 					return;
// 				else
// 					ucSel = STEP_INPUTMANAGERPWD;
// 			}
// 			break;
		case STEP_INPUTID:
			DispString("Cashier ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 0, SIZE_CASHIER_NAME, stWho.szName, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
			{
				if (OK == ReadUsersPwd(stWho.szName,NULL))
				{
					strcpy(stWho.szName, "");
					ScrCls();
					DispString("CASHIER EXISTS.", DISP_SCR_CENTER);
					PubWaitKey(TIME_DELAY);	
				}
				else
					ucSel = STEP_INPUTNEWPWD;
			}
			else if(KEYCANCEL == ucKey)
				ucKey = 0xFF;
			break;
		case STEP_INPUTNEWPWD:
			DispString("Cashier Pwd:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucSel = STEP_INPUTNEWPWDAGAIN;
			else if(KEYCANCEL == ucKey)
				ucSel = STEP_INPUTID;
			break;
		case STEP_INPUTNEWPWDAGAIN:
			DispString("Pwd again:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew2, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucSel = STEP_VERIFY;
			else if(KEYCANCEL == ucKey)
				ucSel = STEP_INPUTID;
			break;
		case STEP_VERIFY:
			if (0 == strcmp(szPwdNew, szPwdNew2))
			{
				strcpy(stWho.szPwd, szPwdNew);
				ucSel = STEP_UPLOAD;
			}
			else
			{
				ScrCls();
				DispString("Don't Match.", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				return;
			}
			break;
		case STEP_UPLOAD:
			do 
			{
				ucRet = Create_User_XML(stWho, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			
			ucSel = STEP_SYNC;
			break;
		case STEP_SYNC:
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				if(-1 != fexist(FILE_USERS_TMP))
					remove(FILE_USERS_TMP);
				DispString("Table Users", DISPLAYLINE |DISP_LINE_CENTER);
				do 
				{
					if(-1 != fexist(FILE_USERS))
						remove(FILE_USERS);
					ucRet = Get_Users_XML(&stRespH, &stRespB, TRUE);
					--iRetryCnt;
				} while (1 == ucRet && iRetryCnt > 0);
				ucSel = STEP_CHANGE;
			}
			else
			{
				Do_Error(ucRet, stRespH);
				PubWaitKey(TIME_DELAY);
				return;
			}
			break;
		case STEP_CHANGE:
			if(OK == ucRet)
			{
				DispString("Success.", DISP_SCR_CENTER);
				//WriteUsers(stWho.szName, stWho.szPwd);
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			PubWaitKey(TIME_DELAY);
			return;
		}
		if(0xFF == ucKey)
			return;
	}
}
static void Do_DelUser(void)
{
	enum
	{
		STEP_INPUTMANAGERPWD = 0,
		STEP_JUDGE,
		STEP_INPUTID,
		STEP_VERIFY,
		STEP_UPLOAD,
		STEP_SYNC,
		STEP_CHANGE,
	};
	unsigned char ucRet, ucKey;
	unsigned char ucSel = STEP_INPUTMANAGERPWD;
	char loop = TIME_RETRY;

	USER_INFO stWho;
	unsigned char szPwd[SIZE_MAX_PWD + 1];

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	memset(&stWho, 0, sizeof(USER_INFO));
	memset(szPwd, 0, sizeof(szPwd));

	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucSel)
		{
		case STEP_INPUTMANAGERPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucSel = STEP_JUDGE;
			break;
		case STEP_JUDGE:
			if (0 == strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
			{
				Do_Logout();
				ucSel = STEP_INPUTID;
			}
			else
			{
				--loop;
				ScrCls();
				DispString("Wrong Pwd", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				if(loop <= 0)
					return;
				else
					ucSel = STEP_INPUTMANAGERPWD;
			}
			break;
		case STEP_INPUTID:
			DispString("Cashier ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN, 0, SIZE_CASHIER_NAME, stWho.szName, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
			{
				if (OK == ReadUsersPwd(stWho.szName,NULL))
					ucSel = STEP_VERIFY;	
				else
				{
					ScrCls();
					DispString("INVALID CASHIER.", DISP_SCR_CENTER);
					PubWaitKey(TIME_DELAY);
					return;
				}
			}
			else if(KEYCANCEL == ucKey)
				ucKey = 0xFF;
			break;
		case STEP_VERIFY:
			if (0 == strcmp(gl_Cur_Cashier.szName, stWho.szName))
			{
				ScrCls();
				DispString("User Logged in", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				return;
			}
			else
				ucSel = STEP_UPLOAD;
			break;
		case STEP_UPLOAD:
			do 
			{
				ucRet = Del_User_XML(stWho, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			
			ucSel = STEP_SYNC;
			break;
		case STEP_SYNC:
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				if(-1 != fexist(FILE_USERS_TMP))
					remove(FILE_USERS_TMP);
				DispString("Table Users", DISPLAYLINE |DISP_LINE_CENTER);
				do 
				{
					if(-1 != fexist(FILE_USERS))
						remove(FILE_USERS);
					ucRet = Get_Users_XML(&stRespH, &stRespB, TRUE);
					--iRetryCnt;
				} while (1 == ucRet && iRetryCnt > 0);
				ucSel = STEP_CHANGE;
			}
			else
			{
				Do_Error(ucRet, stRespH);
				PubWaitKey(TIME_DELAY);
				return;
			}
			break;
		case STEP_CHANGE:
			if(OK == ucRet)
			{
				DispString("Success.", DISP_SCR_CENTER);
				//DelUsers(stWho.szName);
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			PubWaitKey(TIME_DELAY);
			return;
		}
		if(0xFF == ucKey || KEYCANCEL == ucKey)
			return;
	}
}
static void Do_ViewList(void)
{
	enum
	{
		STEP_INPUTMANAGERPWD = 0,
		STEP_JUDGE,
		STEP_VIEW,
	};
	unsigned char ucKey;
	unsigned char ucSel = STEP_INPUTMANAGERPWD;
	char loop = TIME_RETRY;

	USER_INFO szUsers[15]; // 15 ?
	unsigned int iNums = 0;
	unsigned char szPwd[SIZE_MAX_PWD + 1];

	memset(szUsers, 0, sizeof(szUsers));
	memset(szPwd, 0, sizeof(szPwd));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucSel)
		{
		case STEP_INPUTMANAGERPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
				ucSel = STEP_JUDGE;
			break;
		case STEP_JUDGE:
			if (0 == strcmp(gl_MaintenaceParam.stManage.szPwd, szPwd))
			{
				Do_Logout();
				ucSel = STEP_VIEW;
			}
			else
			{
				--loop;
				ScrCls();
				DispString("Wrong Pwd", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				if(loop <= 0)
					return;
				else
					ucSel = STEP_INPUTMANAGERPWD;

			}
			break;
		case STEP_VIEW:
			if (OK == GetUsersList(szUsers, &iNums))
			{
				SelUsersList(szUsers, iNums);
			}
			return;
		}
		if(0xFF == ucKey || KEYCANCEL == ucKey)
			return;
	}
}
static void Do_Cashier(int sub)
{
	switch(sub)
	{
	case ID_Login:
		Do_Login();
		break;
	case ID_Logout:
		Do_Logout();
		break;
	case ID_ChangePwd:
		Do_ChangePwd();
		break;
	case ID_CreateUser:
		Do_CreateUser();
		break;
	case ID_DelUser:
		Do_DelUser();
		break;
	case ID_ViewList:
		Do_ViewList();
		break;
	}
}

void Do_Error(unsigned char Ret, RESPONSE_HEADER_TAG stRespH)
{
	unsigned int iStatus = atoi(stRespH.szStatus);
	ScrCls();
	if(Ret > 5)
	{
		unsigned char tmp[5];
		DispString("Error:", 1 | DISP_LINE_CENTER);
		sprintf(tmp, "%d", Ret);
		DispString(tmp, DISPLAYLINE | DISP_LINE_CENTER);
	}
	else
	{
		DispString(stRespH.szResponseCode, 1 | DISP_LINE_CENTER);
		DispString(stRespH.szMessage, DISPLAYLINE | DISP_LINE_LEFT);
		if(iStatus > 2 && iStatus < 5)
			Prn_Resp(stRespH);
		if(5 == iStatus)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PAS_DE_TELEPARA], DISP_SCR_CENTER);
		}
	}
	PubWaitKey(TIME_DELAY);
}

//////////////////////////////////////////////////////////////////////////


static int SelCateory(SAV_PARAM stSav, unsigned char type)
{
	unsigned char ucKey;
	unsigned char ucPageSel = 0, ucLineSel = 0;
	unsigned char ucPage = 0;
	unsigned char ucLastPage = 0;
	unsigned char ucIndex[NB_MAX_CATEGORYS];

	if(PRODUCT_ALL == type)
	{
		int i;
		ucLastPage = stSav.ucCategoryNum;
		for(i = 0; i < stSav.ucCategoryNum; ++i)
			ucIndex[i] = i;
	}
	else
	{
		unsigned int i;
		for(i = 0; i < stSav.ucCategoryNum; ++i)
		{
			if(&gl_SavParam != &stSav)
			{
				unsigned int j;
				for(j = 0; j < gl_SavParam.ucCategoryNum; ++j)
				{
					if(stSav.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamCategory[j].uiCodeCategory &&
						type == gl_SavParam.stSavParamCategory[j].ucCategoryType)
					{
						ucIndex[ucLastPage] = i;
						++ucLastPage;
						break;
					}
				}
			}
			else
			{
				if(type == stSav.stSavParamCategory[i].ucCategoryType)
				{
					ucIndex[ucLastPage] = i;
					++ucLastPage;
				}
			}
		}
		
	}
	// type ...

	if(0 == ucLastPage)
	{
		DispString("Empty Category", DISP_SCR_CENTER);
		PubYesNo(TIME_OUT_STANDARD_30S);
		return -2;  // mean no key 
	}

	ucPage = ucLastPage / 4 + 1;
	ucLastPage %= 4;

	while(1)
	{
		unsigned int itemNo;
		unsigned int i;
		unsigned char ucNumSel  = ucPageSel * 4 + ucLineSel;
		ScrCls();
		ScrSetIcon(ICON_UP, OPENICON);
		ScrSetIcon(ICON_DOWN, OPENICON);
		if(0 == ucNumSel)
			ScrSetIcon(ICON_UP, CLOSEICON);
		if((ucPage - 1) * 4 + ucLastPage - 1 == ucNumSel)
			ScrSetIcon(ICON_DOWN, CLOSEICON);

		if(ucPageSel < ucPage - 1)
			itemNo = 4;
		else
			itemNo = ucLastPage;
		for(i = 0; i < itemNo; ++i)
			DispString(stSav.stSavParamCategory[ucIndex[ucPageSel * 4 + i]].szName, i * 2 | DISP_LINE_LEFT | (i == ucLineSel ? DISP_LINE_REVER : 0));


		ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
		if(KEYENTER == ucKey)
			return stSav.stSavParamCategory[ucIndex[ucPageSel * 4 + ucLineSel]].uiCodeCategory;
		else if(KEYUP == ucKey)
		{
			if(0 == ucNumSel)
				Beep();
			else
			{
				--ucNumSel;
				ucPageSel = ucNumSel / 4;
				ucLineSel = ucNumSel % 4;
			}
		}
		else if(KEYDOWN == ucKey)
		{
			if((ucPage - 1) * 4 + ucLastPage - 1 == ucNumSel)
				Beep();
			else
			{
				++ucNumSel;
				ucPageSel = ucNumSel / 4;
				ucLineSel = ucNumSel % 4;
			}
		}
		else if(KEYCANCEL == ucKey)
			return -1;
		else if(NOKEY == ucKey)
			return -2;
	}
	return -1;  // mean no key 
}

// Modified by Kim_LinHB 2013-3-22 v4.56
static int SelProduct(SAV_PARAM *pstSav, unsigned char *pszProductId, unsigned int uiCategory_Code)
{
	unsigned char ucKey;
	unsigned char ucPageSel = 0, ucLineSel = 0;
	unsigned char ucPage = 0;
	unsigned char ucLastPage = 0;
	unsigned char ucIndex[NB_MAX_PRODUCTS];
	unsigned int uiSel = 0, MaxSel = 0;

	for(uiSel = 0; uiSel < pstSav->ucProductsNum; ++uiSel)
	{
		if(uiCategory_Code == pstSav->stSavParamProducts[uiSel].uiCodeCategory)
		{	
			ucIndex[MaxSel] = uiSel;
			++MaxSel;
		}
	}

	if(MaxSel > 0){
		uiSel = 0;

		ucPage = MaxSel / 4 + 1;
		ucLastPage = MaxSel % 4;
		
		while(1)
		{
			unsigned char ucNumSel  = ucPageSel * 4 + ucLineSel;
			ScrCls();
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(0 == ucNumSel)
				ScrSetIcon(ICON_UP, CLOSEICON);
			if((ucPage - 1) * 4 + ucLastPage - 1 == ucNumSel)
				ScrSetIcon(ICON_DOWN, CLOSEICON);

			if(ucPageSel < ucPage - 1){
				DispString(pstSav->stSavParamProducts[ucIndex[ucPageSel * 4 + 0]].szNameProduct, 0 | DISP_LINE_LEFT | (0 == ucLineSel ? DISP_LINE_REVER : 0));
				DispString(pstSav->stSavParamProducts[ucIndex[ucPageSel * 4 + 1]].szNameProduct, 2 | DISP_LINE_LEFT | (1 == ucLineSel ? DISP_LINE_REVER : 0));
				DispString(pstSav->stSavParamProducts[ucIndex[ucPageSel * 4 + 2]].szNameProduct, 4 | DISP_LINE_LEFT | (2 == ucLineSel ? DISP_LINE_REVER : 0));
				DispString(pstSav->stSavParamProducts[ucIndex[ucPageSel * 4 + 3]].szNameProduct, 6 | DISP_LINE_LEFT | (3 == ucLineSel ? DISP_LINE_REVER : 0));
			}
			else{
				int i;
				for(i = 0; i < ucLastPage; ++i)
					DispString(pstSav->stSavParamProducts[ucIndex[ucPageSel * 4 + i]].szNameProduct, i * 2 | DISP_LINE_LEFT | (i == ucLineSel ? DISP_LINE_REVER : 0));
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				strcpy(pszProductId, pstSav->stSavParamProducts[ucIndex[ucPageSel * 4 + ucLineSel]].szCodeProduct);
				return OK;
			}
			else if(KEYUP == ucKey)
			{
				if(0 == ucNumSel)
					Beep();
				else
				{
					--ucNumSel;
					ucPageSel = ucNumSel / 4;
					ucLineSel = ucNumSel % 4;
				}
			}
			else if(KEYDOWN == ucKey)
			{
				if((ucPage - 1) * 4 + ucLastPage - 1 == ucNumSel)
					Beep();
				else
				{
					++ucNumSel;
					ucPageSel = ucNumSel / 4;
					ucLineSel = ucNumSel % 4;
				}
			}
			else if(KEYCANCEL == ucKey)
				return -1;
			else if(NOKEY == ucKey)
				return -2;
		}
	}
	else
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_EPUISE], DISP_SCR_CENTER);
		PubWaitKey(TIME_OUT_STANDARD_30S);
		return -1;
	}
	return -2;  // mean no key 
}

// Added by Kim_LinHB 2013-3-21 v4.56
static unsigned char SetPinsNum(SAV_PARAM stSav, const unsigned char *pszProductId, unsigned int *IONum)
{
	unsigned char ucKey;
	unsigned int i, index = -1;
	unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
	unsigned int iNum = 0, oldNum = 0;

	strcpy(szQuantity, "1");

	for(i = 0; i < stSav.ucProductsNum; ++i)
	{
		if(0 == strcmp(pszProductId, stSav.stSavParamProducts[i].szCodeProduct))
		{	
			index = i;
			break;
		}
	}

	if(-1 == index)
		return 0xFF;

	while(1)
	{
		ScrCls();
		oldNum = atoi(szQuantity);
		DispString(stSav.stSavParamProducts[index].szNameProduct, 1 | DISP_LINE_LEFT);
		ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 0, SIZE_MAX_QUANTITY, 
			szQuantity,TIME_OUT_STANDARD_30S, FALSE);
		if(KEYENTER == ucKey)
		{
			iNum = atoi(szQuantity);
			// Added by Kim_LinHB 2013-3-27 v4.56			
			if(0 == iNum)
			{
				ScrClrLine(DISPLAYLINE,DISPLAYLINE);
				DispString("QTY can't be 0.", DISPLAYLINE | DISP_LINE_CENTER);
				while(1)
				{
					unsigned char tmp = PubWaitKey(TIME_OUT_STANDARD_30S);
					if(NOKEY == tmp)
					{
						ucKey =0xFF;
						break;
					}
					else if(KEYENTER == tmp)
					{
						sprintf(szQuantity, "%lu", oldNum);
						break;
					}
				}
			}
			// Add End
			else if(*IONum < iNum)
			{
				ScrClrLine(DISPLAYLINE,DISPLAYLINE);
				DispString("Not Enough Pins", DISPLAYLINE | DISP_LINE_CENTER);
				while(1)
				{
					unsigned char tmp = PubWaitKey(TIME_OUT_STANDARD_30S);
					if(NOKEY == tmp)
					{
						ucKey =0xFF;
						break;
					}
					else if(KEYENTER == tmp)
					{
						sprintf(szQuantity, "%lu", oldNum);
						break;
					}
				}
			}
			else
			{
				// Removed by Kim_LinHB 2013-4-1 v4.57
// 				unsigned char tmp[16], ucKey_temp = 0xFF;
// 				ScrCls();
// 				sprintf(tmp, "%d %s", iNum, "Pins");
// 				DispString(tmp, 1 | DISP_LINE_LEFT);
// 				DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
// 
// 				ucKey_temp = PubYesNo(TIME_OUT_STANDARD_30S);
// 
// 				if(0 == ucKey_temp)
				// Remove End
				{
					*IONum = iNum;
					return OK;
				}
			}
		}
		else if(0xFF == ucKey || KEYCANCEL == ucKey)
			return ucKey;
	}
}
// Add End

static const unsigned char* SelUsersList(const USER_INFO *Users, unsigned int iNums)
{
	unsigned char ucKey;
	unsigned char ucPageSel = 0, ucLineSel = 0;
	unsigned char ucPage = 0;
	unsigned char ucLastPage = 0;
	unsigned char ucIndex[15];

	unsigned int i;
	ucLastPage = iNums;
	for(i = 0; i < iNums; ++i)
		ucIndex[i] = i;

	ucPage = ucLastPage / 4 + 1;
	ucLastPage %= 4;

	while(ucPage > 0)
	{
		unsigned int itemNo;
		unsigned int i;
		ScrCls();
		ScrSetIcon(ICON_UP, OPENICON);
		ScrSetIcon(ICON_DOWN, OPENICON);

		if(0 == ucPageSel * 4 + ucLineSel)
			ScrSetIcon(ICON_UP, CLOSEICON);
		
		if(iNums - 1 <= (unsigned int)(ucPageSel * 4 + ucLineSel))
			ScrSetIcon(ICON_DOWN, CLOSEICON);

		if(ucPageSel < ucPage - 1)
			itemNo = 4;
		else
			itemNo = ucLastPage;
		for(i = 0; i < itemNo; ++i)
			DispString((void *)Users[ucIndex[ucPageSel * 4 + i]].szName, i * 2 | DISP_LINE_LEFT | (i == ucLineSel ? DISP_LINE_REVER : 0));

		ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
		if(KEYENTER == ucKey)
			return Users[ucIndex[ucPageSel * 4 + ucLineSel]].szName;
		else if(KEYUP == ucKey)
		{
			unsigned char ucNumSel  = ucPageSel * 4 + ucLineSel;
			if(0 == ucNumSel)
				Beep();
			else
			{
				--ucNumSel;
				ucPageSel = ucNumSel / 4;
				ucLineSel = ucNumSel % 4;
			}
		}
		else if(KEYDOWN == ucKey)
		{
			unsigned char ucNumSel  = ucPageSel * 4 + ucLineSel;
			if((ucPage - 1) * 4 + ucLastPage - 1 == ucNumSel)
				Beep();
			else
			{
				++ucNumSel;
				ucPageSel = ucNumSel / 4;
				ucLineSel = ucNumSel % 4;
			}
		}
		else if(KEYCANCEL == ucKey)
			return NULL;
		else if(NOKEY == ucKey)
			return NULL;
	}
	DispString("Empty List", DISP_SCR_CENTER);
	PubYesNo(TIME_OUT_STANDARD_30S);
	return NULL;  // mean no key 
}

// Added by Kim_LinHB 2013-3-22 v4.56
static void GetProductIndex(SAV_PARAM stSav, TRANS_DATA *pstTrans)
{
	unsigned int i;
	pstTrans->ucCategory_Index = NB_MAX_CATEGORYS;
	pstTrans->ucProducts_Index = NB_MAX_PRODUCTS;

	// Modified by Kim_LinHB 2013-4-1 v4.57 
	// fix bug stSav.ucCategoryNum to stSav.ucProductsNum
	for (i = 0; i < stSav.ucProductsNum; ++i)
	{
		if (0 == strcmp(stSav.stSavParamProducts[i].szCodeProduct, pstTrans->stVoucher.szProductId))
		{
			pstTrans->ucProducts_Index = i;
			pstTrans->ucCategory_Index = stSav.stSavParamProducts[i].ucCategory_Index;
			break;
		}
	}
}
// Add End

static void RefreshStock()
{
	IsSalesPollingEmpty(NULL, NULL, NULL, NULL);
}

unsigned char IsSalesPollingEmpty(unsigned int *puiSold, unsigned int *puiExpiry, unsigned int *puiBurned, unsigned int *puiSelling)
{
	unsigned int i;
	unsigned char szCurDate[SIZE_DATE_TIME + 1];
	STOCK stock;

	unsigned int uiSoldCnt = 0, uiExpiry = 0, uiBurned = 0, uiSelling = 0;

	GetCurTime(szCurDate);
	ReadStock_All(&stock);
	for(i = 0; i < NB_MAX_CODE_STOCK; ++i)  // didn't use FindCodeCnt
	{
		if(CODE_RESTOCK_SOLD == stock.stStock[i].ucProductState ||
			CODE_RESTOCK_SALESPOLLING == stock.stStock[i].ucProductState)
			++uiSoldCnt;
		if(CODE_RESTOCK_BURNED == stock.stStock[i].ucProductState)
			++uiBurned;
		if(CODE_RESTOCK_EXPIRED == stock.stStock[i].ucProductState)
			++uiExpiry;
 		if(CODE_RESTOCK_SELLING == stock.stStock[i].ucProductState)
 			++uiSelling;
		// Removed by Kim_LinHB 2013-9-22 v4.69 cuz added update stock when power on
// 		if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState)
// 		{
// 			stock.stStock[i].ucProductState = CODE_RESTOCK_FREE;
// 		}

		if(CODE_RESTOCK_FREE == stock.stStock[i].ucProductState)
		{
			if(memcmp(stock.stVoucher[i].szExpirDate, szCurDate, SIZE_DATE_TIME) <= 0 &&
				strlen(stock.stVoucher[i].szExpirDate) > 0) // Added by Kim_LinHB 2013-5-3 v4.58
			{
				stock.stStock[i].ucProductState = CODE_RESTOCK_EXPIRED;
				++uiExpiry;
			}
		}
	}

	WriteStock_All(&stock);

	if(0 == uiSoldCnt &&
		0 == uiBurned &&
		0 == uiExpiry &&
		0 == uiSelling)
		return TRUE;

	if(puiSold)
		*puiSold = uiSoldCnt;

	if(puiExpiry)
		*puiExpiry = uiExpiry;

	if(puiBurned)
		*puiBurned = uiBurned;

	if(puiSelling)
		*puiSelling = uiSelling;

	return FALSE;
}

static void Erase_Salepolling(unsigned char type)
{
	unsigned int i;
	unsigned char ucType = type;
	STOCK stock;

	if(CODE_FIN_DAILY == type)
	{	
		remove(FILE_TRANS_OL);
		remove(FILE_CASHORDER); // Added by Kim_LinHB 2013-6-17 v4.60
		ucType = CODE_RESTOCK_SOLD;
	}

	ScrCls();
	DispString(gl_pLangSelPiont[STR_ACTUALISE_STOCK], DISP_SCR_CENTER);
	ReadStock_All(&stock);
	for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
	{
		if(ucType == stock.stStock[i].ucProductState)
		{
			memset(&stock.stStock[i], 0, sizeof(STOCK_INFO));
			memset(&stock.stVoucher[i], 0, sizeof(VOUCHER_INFO));
			strcpy(stock.stVoucher[i].szExpirDate, "999999999999");
			stock.stStock[i].ucProductState = CODE_RESTOCK_EMPTY;
		}
	}
	WriteStock_All(&stock);
}

static void AscInc(unsigned short *No)
{
	if (No)
	{
		++*No;
		if (*No >= 10000)
		{
			*No = 0;
		}
	}
}

void SortStock()
{
	int i, j;
	STOCK stock;
	STOCK_INFO stStock;
	VOUCHER_INFO stVoucher;

	memset(&stStock, 0, sizeof(STOCK_INFO));
	memset(&stVoucher, 0, sizeof(VOUCHER_INFO));
	ReadStock_All(&stock);
	for(j = 0; j < NB_MAX_CODE_STOCK; ++j)
	{
		for(i = 0; i < NB_MAX_CODE_STOCK - j - 1; ++i)
		{
			if(memcmp(stock.stVoucher[i].szExpirDate, stock.stVoucher[i + 1].szExpirDate, 12) > 0)
			{
				memcpy(&stStock, &stock.stStock[i], sizeof(STOCK_INFO));
				memcpy(&stVoucher, &stock.stVoucher[i], sizeof(VOUCHER_INFO));

				memcpy(&stock.stStock[i], &stock.stStock[i + 1], sizeof(STOCK_INFO));
				memcpy(&stock.stVoucher[i], &stock.stVoucher[i + 1], sizeof(VOUCHER_INFO));

				memcpy(&stock.stStock[i + 1], &stStock, sizeof(STOCK_INFO));
				memcpy(&stock.stVoucher[i + 1], &stVoucher, sizeof(VOUCHER_INFO));
			}
		}
	}
	WriteStock_All(&stock);
}

static void SetProductsQuantity(SAV_PARAM *pstSav, TRANS_DATA_CASH *pstCurTran, unsigned int uiCategory_Code)
{
	unsigned char ucKey, ucSel = 0;
	unsigned char ucIndex[NB_MAX_PRODUCTS];
	unsigned char MaxSel = 0;
	PRODUCT_INFO  stProducts[NB_MAX_PRODUCTS];
	for(ucSel = 0; ucSel < pstSav->ucProductsNum; ++ucSel)
	{
		if(uiCategory_Code == pstSav->stSavParamProducts[ucSel].uiCodeCategory)
		{	
			unsigned int i = 0;
			ucIndex[MaxSel] = ucSel;
			strcpy(stProducts[MaxSel].szCodeProduct, pstSav->stSavParamProducts[ucSel].szCodeProduct);
			strcpy(stProducts[MaxSel].szQuantity, "0");
			for(i = 0; i < pstCurTran->ucProductNum; ++i)
			{
				if(0 == strcmp(stProducts[MaxSel].szCodeProduct,pstCurTran->stProducts[i].szCodeProduct))
				{
					strcpy(stProducts[MaxSel].szQuantity, pstCurTran->stProducts[i].szQuantity);
					break;
				}
			}
			++MaxSel;
		}
	}

	if(MaxSel > 0){
		ucSel = 0;
		while(1)
		{
			int iTmpQty = 0;
			ScrCls();
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(0 == ucSel)
				ScrSetIcon(ICON_UP, CLOSEICON);
			if(MaxSel - 1 == ucSel)
				ScrSetIcon(ICON_DOWN, CLOSEICON);

			ScrPrint(0,1, ASCII, pstSav->stSavParamProducts[ucIndex[ucSel]].szNameProduct);

			iTmpQty = atoi(stProducts[ucSel].szQuantity);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 0, SIZE_MAX_QUANTITY, 
				stProducts[ucSel].szQuantity,TIME_OUT_STANDARD_30S, FALSE);
			if(KEYENTER == ucKey)
			{
				unsigned int i;
				unsigned char bNew = TRUE;
				int iProductQty = atoi(stProducts[ucSel].szQuantity);
				
				for(i = 0; i < pstCurTran->ucProductNum; ++i)
				{
					if(0 == strcmp(stProducts[ucSel].szCodeProduct,pstCurTran->stProducts[i].szCodeProduct))
					{
						if(0 == iProductQty) //delete
						{
							memset(&pstCurTran->stProducts[i], 0, sizeof(PRODUCT_INFO));
							memmove(&pstCurTran->stProducts[i], &pstCurTran->stProducts[i + 1],
								(pstCurTran->ucProductNum -i - 1) *sizeof(PRODUCT_INFO));
							--pstCurTran->ucProductNum;
						}
						else
						{
							sprintf(pstCurTran->stProducts[i].szQuantity, "%d", iProductQty);
						}
						bNew = FALSE;
						break;
					}
				}
				if(TRUE == bNew)
				{
					if(iProductQty != 0)
					{
						pstCurTran->stProducts[pstCurTran->ucProductNum].uiCodeCategory = 
							pstSav->stSavParamProducts[ucIndex[ucSel]].uiCodeCategory;
						memcpy(pstCurTran->stProducts[pstCurTran->ucProductNum].szCodeProduct,
							pstSav->stSavParamProducts[ucIndex[ucSel]].szCodeProduct, 5);
						sprintf(pstCurTran->stProducts[pstCurTran->ucProductNum].szQuantity, "%d", iProductQty);
						++pstCurTran->ucProductNum;
					}
				}

				if(MaxSel - 1 == ucSel)
					Beep_Speaker(BEEP_ERR);
				else
					++ucSel;
			}
			else if(KEYUP ==ucKey)
			{
				if (0 == ucSel)
					Beep_Speaker(BEEP_ERR);
				else
					--ucSel;
			}
			else if(KEYDOWN == ucKey)
			{
				if (MaxSel - 1 == ucSel)
					Beep_Speaker(BEEP_ERR);
				else
					++ucSel;
			}
			else if(0xFF == ucKey || KEYCANCEL == ucKey)
				return;
		}
	}
	else
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_EPUISE], DISP_SCR_CENTER);
		PubWaitKey(TIME_OUT_STANDARD_30S);
	}
}

static void CalcAmt(TRANS_DATA_CASH *pstCurTran)
{
	unsigned int i = 0;
	unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
	unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

	sprintf(pstCurTran->szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
	sprintf(pstCurTran->szAmountWithoutDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

	for(i = 0; i < pstCurTran->ucProductNum; ++i)
	{
		unsigned int j;
		for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
		{
			if(0 == strcmp(pstCurTran->stProducts[i].szCodeProduct, gl_SavParam.stSavParamProducts[j].szCodeProduct))
			{
				PubAscMul(gl_SavParam.stSavParamProducts[j].szAmount,
					pstCurTran->stProducts[i].szQuantity, 
					szAmountWithoutDiscount);
				PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

				PubAscMul(szAmountWithoutDiscount,
					gl_SavParam.stSavParamProducts[j].szDiscount, 
					szAmountWithDiscount);
				break;
			}
		}

		PubAddHeadChars(szAmountWithDiscount, SIZE_MAX_AMOUNT, '0');

		PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
		if(strlen(szAmountWithDiscount) > 2)
		{
			szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
		}
		PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
		PubAddHeadChars(szAmountWithDiscount, SIZE_MAX_AMOUNT, '0');

		PubAscSub(szAmountWithoutDiscount,szAmountWithDiscount,strlen(szAmountWithoutDiscount),
			szAmountWithDiscount);

		SafeAscAdd(pstCurTran->szAmountWithDiscount,
			szAmountWithDiscount,
			SIZE_MAX_AMOUNT);
		SafeAscAdd(pstCurTran->szAmountWithoutDiscount,
			szAmountWithoutDiscount,
			SIZE_MAX_AMOUNT);
	}
	if(atol(pstCurTran->szAmountWithoutDiscount) != 0)
		PubTrimHeadChars(pstCurTran->szAmountWithoutDiscount, '0');
	else
		strcpy(pstCurTran->szAmountWithoutDiscount, "0");
	if(atol(pstCurTran->szAmountWithDiscount) != 0)
		PubTrimHeadChars(pstCurTran->szAmountWithDiscount, '0');
	else
		strcpy(pstCurTran->szAmountWithDiscount, "0");
	sprintf(pstCurTran->szAmountWithoutDiscount, "%.*s", strlen(pstCurTran->szAmountWithoutDiscount) - 2, 
		pstCurTran->szAmountWithoutDiscount);
	sprintf(pstCurTran->szAmountWithDiscount, "%.*s", strlen(pstCurTran->szAmountWithDiscount) - 2, 
		pstCurTran->szAmountWithDiscount);
}

// Added by Kim_LinHB 2013-7-6 v4.62
// Modified by Kim_LinHB 2013-7-31 v4.62
// Modified by Kim_LinHB 2013-9-12 v4.68
static void UpdateProductLsit(SAV_PARAM *dst, const SAV_PARAM src)
{
	int i, j;

	for(j = 0; j < src.ucCategoryNum; ++j)
	{
		if(dst->stSavParamCategory[gl_LastTran.ucCategory_Index].uiCodeCategory ==
			src.stSavParamCategory[j].uiCodeCategory)
		{
			gl_LastTran.ucCategory_Index = j;
			break;
		}
	}
	for(j = 0; j < src.ucProductsNum; ++j)
	{
		if(0 == strcmp(dst->stSavParamProducts[gl_LastTran.ucProducts_Index].szCodeProduct,
			src.stSavParamProducts[j].szCodeProduct))
		{
			gl_LastTran.ucProducts_Index = j;
			break;
		}
	}
	
	memcpy(dst->stSavParamCategory, &src.stSavParamCategory, NB_MAX_CATEGORYS * sizeof(xParamCategory));
	memcpy(dst->stSavParamProducts, &src.stSavParamProducts, NB_MAX_PRODUCTS * sizeof(xParamProducts));
	dst->ucCategoryNum = src.ucCategoryNum;
	dst->ucProductsNum = src.ucProductsNum;
	// Modify End

	for(i = 0; i < dst->ucProductsNum; ++i)
	{
		for(j = 0; j < dst->ucCategoryNum; ++j)
		{
			if(dst->stSavParamProducts[i].uiCodeCategory == dst->stSavParamCategory[j].uiCodeCategory)
			{
				dst->stSavParamProducts[i].ucCategory_Index = j;
				break;
			}
		}
	}
	// Add End
}

// Added by Kim_LinHB 2014-5-12
// Modified by Kim_LinHB 2014/10/29
static int Do_Reversal(RESPONSE_HEADER_TAG *pstRespH, \
					   RESPONSE_BODY_TAG *pstRespB)
{
	int iRet = OK;
	ReversalInfo stReversal, stAllZero;
	TRANS_DATA stTrans;
	unsigned int nIndex = 0;

	if(-1 == fexist(FILE_REVERSAL))
		return OK;

	memset(&stAllZero, 0, sizeof(ReversalInfo));

	stTrans.usTraceNo = gl_SavParam.usTraceNo;
#ifdef  LHB_DEBUG
	PubDebugOutput(NULL, "searching reversal record",strlen("searching reversal record"),DEVICE_COM1,ASC_MODE);
#endif
	if(OK == ReadReversal(&stReversal))
	{
#ifdef  LHB_DEBUG
		PubDebugOutput(NULL, "got reversal record",strlen("got reversal record"),DEVICE_COM1,ASC_MODE);
#endif
		ScrCls();
		DispString("REVERSAL...", DISP_SCR_CENTER);

		strcpy(stTrans.szProvisionID, stReversal.szProvisionId);
		stTrans.ucTransType = stReversal.ucMessageType;
		strcpy(stTrans.szCashier, stReversal.szUser);

		GetCurTime(stTrans.szDateTime);
		iRet = Cancellation_XML(stTrans, TRUE, pstRespH, pstRespB, FALSE);
		AscInc(&stTrans.usTraceNo);
		if(OK == iRet){
			DelReversal();
		}
		ScrCls();
	}
#ifdef  LHB_DEBUG
	PubDebugOutput(NULL, "reversal(done)",strlen("reversal(done)"),DEVICE_COM1,ASC_MODE);
#endif
	gl_SavParam.usTraceNo = stTrans.usTraceNo;
	WriteParam(WRT_SAVE);

	return iRet;
}

//end of file
