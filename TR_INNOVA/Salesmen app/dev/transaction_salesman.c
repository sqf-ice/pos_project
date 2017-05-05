#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "transaction.h"
#include "connect.h"
#include "framework.h"
#include "calc_salesman.h"
#include "printList_salesman.h"

#include "LHBXML.h"

static void Do_RecordVisit(void); // done
static void Do_ConsignOrder(void);//done
static void Do_ReturnConsignOrder(void);//done
static void Do_CancelLastOrder(void);//done

static void Do_Order(int sub);//done
static void Do_CashOrder(void);//done
static void Do_CreditTransfer(void); //done

static void Do_Offline(int sub);//done
static void Do_OfflineOrder(void);//done
static void Do_Offline_Visit(void);//done
static void Do_OfflineCancelLastOrder(void);//done
static int Do_OfflineBatchUpload(uchar bAuto);//done

static void Do_CashCollection(int sub);//nedd msg detial

static void Do_BankDeposit(int sub);//nedd msg detial

static void Do_Information(int sub);//done
static void Do_PendingOrders(void);//done
static void Do_SalesmanInfo(void);//done
static void Do_SalesmanRoute(void);//done
static void Do_PrintDiscount(void);//done
static void Do_ChangePwd(void);  //done

static void Do_Report(int sub);
static void Do_OfflineBatchUploadRep(void);
static void Do_OfflinePhysicalStockRep(void);
static void Do_OrderRep(void);
static void Do_OrderRep_Prod(void);
static void Do_OnlineStocksRep(void);
static void Do_LastConsignOrderRep(void);
static void Do_SummaryRep(void);

static void Do_Transmission(int sub);//done
static void Do_SettDownload(void);//done
static void Do_SoftDownload(unsigned char bAuto); //done


static void Do_Consignment(int sub);//done
static void Do_CreditOrder(void);//done
static void Do_ReturnOrder(void);//done
static void Do_ConsignmentConfirm(void);
static void Do_PendinConsignOrder(void);


static void Do_Error(unsigned char Ret, RESPONSE_HEADER_TAG stRespH);

static int SelCateory(SAV_PARAM stSav, unsigned char type);
static void SetProductsQuantity(SAV_PARAM *pstSav, TRANS_DATA *pstCurTran, \
								 unsigned int uiCategory_Code, unsigned char bCredit);
static void SaveProducts(TRANS_DATA stCunsignedurTran);
static void CalcAmt(TRANS_DATA *pstCurTran);
static void AscInc(unsigned short *No);

// Added by Kim_LinHB 2013-7-22 v4.60
static unsigned char SelTerminal(ROUTE_INFO route, const unsigned char *MID, \
								  xMerchantList *stMerchant);

// Added by Kim_LinHB 2013-7-6 v4.59
static void UpdateProductLsit(SAV_PARAM *dst, const SAV_PARAM src);

// Added by Kim_LinHB 2013-10-28 v4.68
static int SelConsignOrderID(CONSIGNMENT_LIST stOrderList, unsigned char *bDisplay);
static int SelConsignOperation();

const unsigned char c_LANG[][5 + 1] = 
{
	{"en-US"},
	{"tr-TR"},
};

void DoTrans(unsigned int uiType)
{
	switch(uiType / 10)
	{
	case 1:// temporarily without sub menu
		switch(uiType)
		{
		case ID_RecordVisit:
			Do_RecordVisit();
			break;
		case ID_CancelLastOrder:
			// Added by Kim_LinHB 2013-9-11 v4.67
			if(gl_SavParam.uiCurOfflineNo > 0)
			{
				if(Do_OfflineBatchUpload(TRUE) != OK)
					return;
				ScrCls();
			}
			// Add End
			Do_CancelLastOrder();
			break;
		}
		break;
	case 2:// Order
		Do_Order(uiType);
		break;
	case 3:// Offline
		Do_Offline(uiType);
		break;
	case 4:// Cash Collection
		Do_CashCollection(uiType);
		break;
	case 5:// Bank Deposit
		Do_BankDeposit(uiType);
		break;
	case 6:// Information
		Do_Information(uiType);
		break;
	case 7:
		Do_Report(uiType);
		break;
	case 8:// Transmission
		Do_Transmission(uiType);
		break;
	case 11://Consignment
		Do_Consignment(uiType);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
static void Do_RecordVisit(void) // Online Visit
{
	enum
	{
		STEP_BARCODE = 0,
		STEP_MERCHANTCODE,
		STEP_TERMINALCODE,
		STEP_DISPLAY,
		STEP_VALIDATE,
		STEP_INPUTPWD,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_BARCODE;
	char loop = TIME_RETRY;
	unsigned char bManualInputOK = 0x00;  
	// 0x00 : false 0x01 : merchant id  is inputed 0x02 : terminal id is inputed
	//0x03 : true
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	xMerchantList stMerchant;
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char IsSN = TRUE;
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char bMatch = FALSE;
	// Add End
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stMerchant, 0 ,sizeof(xMerchantList));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	
	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_BARCODE:
			if(!gl_MaintenaceParam.bScanSerialNumber)
			{
				DispString("Disable Scan.", DISP_SCR_CENTER);
				ucStep = STEP_MERCHANTCODE;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);

			memset(szSerial, 0, sizeof(szSerial));
			bMatch = FALSE;
			ucRet = ScanBarCode(szSerial, &ucKey);

			if (0 == ucRet)
			{
				// Modified by Kim_LinHB 2013-7-22 v4.60
				unsigned int i;
				stCurTran.ucIdentifyType = TYPE_BARCODE;
				if(0 == memcmp(szSerial, "00", 2))
				{
					IsSN = FALSE;
					memmove(szSerial, szSerial + 2, sizeof(szSerial) - 2);
				}
				else
					IsSN = TRUE;
				if(IsSN)
				{
					for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
					{
						GetMerchant(FILE_MERCHANT, i, &stMerchant);
						if(0 == strcmp(szSerial, stMerchant.szSerial))
						{
							strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
							strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
							strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
							strcpy(stCurTran.szSerial, stMerchant.szSerial);
							bMatch = TRUE;
							break;
						}
					}
				}
				else
				{
					bMatch = SelTerminal(gl_SavParam.stRoute, szSerial, &stMerchant);
					if(bMatch)
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
						strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
					}
				}
				// Modify End
				ucStep = STEP_DISPLAY;
			}
			else
			{
				memset(szSerial, 0, sizeof(szSerial));
				stCurTran.ucIdentifyType = TYPE_MANUAL_ID;
				ucStep = STEP_MERCHANTCODE;
			}
			break;
		case STEP_MERCHANTCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Merchant ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szMerchantCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x01;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_BARCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_TERMINALCODE;
			break;
		case STEP_TERMINALCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Terminal ID:", 1 | DISP_LINE_LEFT);
			// Modified by Kim_LinHB 2013-4-2 v4.56
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szTermCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x02;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_MERCHANTCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_VALIDATE;
			break;
		case STEP_DISPLAY:
			ScrSetIcon(ICON_DOWN, OPENICON);

			if(TYPE_BARCODE == stCurTran.ucIdentifyType)
			{
				if(IsSN)
					DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				else
					DispString("Merchant ID", 1 | DISP_LINE_LEFT);

				DispString(szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				if(!bMatch)	
					DispString("not in the list", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			}
			else
			{
				unsigned int i;
				bMatch = FALSE;
				DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				if(0 == strlen(stCurTran.szMerchantCode) ||
					0 == strlen(stCurTran.szTermCode))
				{
					i = gl_SavParam.stRoute.uiMerchantListNum; // jump over the loop
				}
				for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
				{
					GetMerchant(FILE_MERCHANT, i, &stMerchant);
					if(0 == strcmp(stCurTran.szMerchantCode, stMerchant.szMerchantCode) &&
						0 == strcmp(stCurTran.szTermCode, stMerchant.szTermCode))
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
						bMatch = TRUE;
						break;
					}
				}
				if(bMatch)
					DispString(stCurTran.szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				else
				{
					DispString("MISMATCH", DISPLAYLINE | DISP_LINE_LEFT);
					DispString("cancel to re-enter", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
				}
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYCANCEL == ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					ucStep = STEP_MERCHANTCODE;
				bManualInputOK = 0x00;
				break;
			}
			else if((KEYENTER == ucKey || KEYDOWN == ucKey) && bMatch)
				ucStep = STEP_VALIDATE;
			else if(KEYUP ==ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_VALIDATE:
			ScrSetIcon(ICON_UP, OPENICON);
			DispString(gl_pLangSelPiont[STR_VALIDATE_ORDER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
			
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(!bMatch)
					ucStep = STEP_MERCHANTCODE;
				else
					ucStep = STEP_INPUTPWD;
			}
			else if(KEYUP == ucKey)
			{				
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYDOWN == ucKey)
				Beep_Speaker(BEEP_ERR);
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;
			stCurTran.ucTransType = ID_RecordVisit;

			DispString("SEND RECORD VISIT", 1 | DISP_LINE_CENTER);
			DispString("INFORMATIONS", DISPLAYLINE | DISP_LINE_CENTER);

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Get_ProvisionId_XML(stCurTran.szSalesman_PWD, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);
			}

			if(OK == ucRet)
			{
				int cnt = 3;
				while(cnt-- >0)
				{
					GetCurTime(stCurTran.szDateTime);
					ucRet = SaveSalesmanVisit_XML(stCurTran,&stRespH, &stRespB, FALSE);
					if(ucRet != 1)
						break;
				}
// 				if(cnt <= 0)
// 					Cancellation_XML(stCurTran, &stRespH, &stRespB, TRUE);//cancel;
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
			if(0 == strcmp(gl_LastTran.szMerchantID, stCurTran.szMerchantID))
				ucRet = OK;
			
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
				Prn_Visit(stCurTran);//print 
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



static void Do_CancelLastOrder(void)
{
	enum
	{
		STEP_CHECK = 0,
		STEP_INPUTPWD,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_CHECK;
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	memcpy(&stCurTran, &gl_LastTran ,sizeof(TRANS_DATA));
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_CHECK:
			if(0 == stCurTran.ucTransType)
			{
				DispString(gl_pLangSelPiont[STR_PB_ORDER_EMPTY], DISP_SCR_CENTER);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return;
			}
			if(gl_MaintenaceParam.bCancelSale)
				ucStep = STEP_INPUTPWD;
			else
			{
				ScrCls();
				DispString(gl_pLangSelPiont[STR_NOT_ALLOWED_FOR], 1 | DISP_LINE_CENTER);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return;
			}
				break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{	
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;

			stCurTran.ucTransType = gl_LastTran.ucTransType;

			DispConnection();

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

// 			if(OK == ucRet)
// 			{
// 				int iRetryCnt = TIME_RETRY;
// 				do{
// 					ucRet = Get_ProvisionId_XML(stCurTran.szSalesman_PWD, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
// 					--iRetryCnt;
// 				}while(1 == ucRet && iRetryCnt > 0);
// 			}

			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do 
				{
					ucRet = Cancellation_XML(stCurTran, &stRespH, &stRespB, FALSE);
					--iRetryCnt;
				} while (1 == ucRet && iRetryCnt > 0);
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
				memset(&gl_LastTran, 0, sizeof(TRANS_DATA));
				SaveProducts(stCurTran);
				WriteParam(WRT_SAVE | WRT_TXN);
				Prn_CancelLastOrder(stCurTran);//print 
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			return;
			break;
		}
		if(0xFF == ucKey)
			return;
	}
}
//////////////////////////////////////////////////////////////////////////
static void Do_CashOrder(void)
{
	enum
	{
		STEP_BARCODE = 0,
		STEP_MERCHANTCODE,
		STEP_TERMINALCODE,
		STEP_DISPLAY,
		STEP_QUANTITY,
		//STEP_DISCOUNT,
		STEP_CHECKORDER,
		STEP_INPUTPWD,
		STEP_UPLOADATA,
		STEP_PRINT,
		STEP_NEWORDER,
	};

	unsigned char ucRet, ucKey = 0;
	unsigned char ucDone = 0x00;
	unsigned char ucStep = STEP_BARCODE;
	unsigned char bManualInputOK = 0x00;  // 0x00 : false 0x01 : merchant id  is inputed 0x02 : terminal id is inputed
								//0x03 : true
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	xMerchantList stMerchant;
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char IsSN = TRUE;
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char bMatch = FALSE;
	// Add End
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stMerchant, 0, sizeof(xMerchantList));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	
	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_BARCODE:
			if(!gl_MaintenaceParam.bScanSerialNumber)
			{
				DispString("Disable Scan.", DISP_SCR_CENTER);
				ucStep = STEP_MERCHANTCODE;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);

			memset(szSerial, 0, sizeof(szSerial));
			bMatch = FALSE;
			ucRet = ScanBarCode(szSerial, &ucKey);

			if (0 == ucRet)
			{
				// Modified by Kim_LinHB 2013-7-22 v4.60
				unsigned int i;
				stCurTran.ucIdentifyType = TYPE_BARCODE;
				if(0 == memcmp(szSerial, "00", 2))
				{
					IsSN = FALSE;
					memmove(szSerial, szSerial + 2, sizeof(szSerial) - 2);
				}
				else
					IsSN = TRUE;
				if(IsSN)
				{
					for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
					{
						GetMerchant(FILE_MERCHANT, i, &stMerchant);
						if(0 == strcmp(szSerial, stMerchant.szSerial))
						{
							strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
							strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
							strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
							strcpy(stCurTran.szSerial, stMerchant.szSerial);
							bMatch = TRUE;
							break;
						}
					}
				}
				else
				{
					bMatch = SelTerminal(gl_SavParam.stRoute, szSerial, &stMerchant);
					if(bMatch)
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
						strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
					}
				}
				// Modify End
				ucStep = STEP_DISPLAY;
			}
			else
			{
				memset(szSerial, 0, sizeof(szSerial));
				stCurTran.ucIdentifyType = TYPE_MANUAL_ID;
				ucStep = STEP_MERCHANTCODE;
			}
			break;
		case STEP_MERCHANTCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Merchant ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szMerchantCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x01;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_BARCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_TERMINALCODE;
			break;
		case STEP_TERMINALCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Terminal ID:", 1 | DISP_LINE_LEFT);
			// Modified by Kim_LinHB 2013-4-2 v4.56
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szTermCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x02;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_MERCHANTCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_QUANTITY;
			break;
		case STEP_DISPLAY: // Modified by Kim_LinHB 2013-7-18 v4.60
			ScrSetIcon(ICON_DOWN, OPENICON);

			if(TYPE_BARCODE == stCurTran.ucIdentifyType)
			{
				if(IsSN)
					DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				else
					DispString("Merchant ID", 1 | DISP_LINE_LEFT);

				DispString(szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				if(!bMatch)	
					DispString("not in the list", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			}
			else
			{
				unsigned int i;
				bMatch = FALSE;
				DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				if(0 == strlen(stCurTran.szMerchantCode) ||
					0 == strlen(stCurTran.szTermCode))
				{
					i = gl_SavParam.stRoute.uiMerchantListNum; // jump over the loop
				}
				for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
				{
					GetMerchant(FILE_MERCHANT, i, &stMerchant);
					if(0 == strcmp(stCurTran.szMerchantCode, stMerchant.szMerchantCode) &&
						0 == strcmp(stCurTran.szTermCode, stMerchant.szTermCode))
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
						bMatch = TRUE;
						break;
					}
				}
				if(bMatch)
					DispString(stCurTran.szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				else
				{
					DispString("MISMATCH", DISPLAYLINE | DISP_LINE_LEFT);
					DispString("cancel to re-enter", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
				}
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYCANCEL == ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					ucStep = STEP_MERCHANTCODE;
				bManualInputOK = 0x00;
				break;
			}
			else if((KEYENTER == ucKey || KEYDOWN == ucKey) && bMatch)
				ucStep = STEP_QUANTITY;
			else if(KEYUP ==ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_QUANTITY:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_QUANTITE], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
 				int uiCategory_Code = -1;
 				if(0 == gl_SavParam.ucCategoryNum)
 				{
 					ScrCls();
 					DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
 					PubWaitKey(TIME_OUT_STANDARD_30S);
 					return;
 				}
 
 				while(1){
 					uiCategory_Code = SelCateory(gl_SavParam, PRODUCT_ALL);
 					if(-1 == uiCategory_Code)
 					{
 						if(stCurTran.uiProductNum > 0)
 							ucDone |= 0x01;
 						//ucStep = STEP_DISCOUNT;
						ucStep = STEP_CHECKORDER;
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
 						SetProductsQuantity(&gl_SavParam, &stCurTran, uiCategory_Code, FALSE);
					}
 				}
			}
			else if(KEYUP == ucKey)
			{
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYDOWN == ucKey)
			{	
				//ucStep = STEP_DISCOUNT;
				if(stCurTran.uiProductNum > 0)
					ucStep = STEP_CHECKORDER;
			}
			break;
// 		case STEP_DISCOUNT:
// 			ScrSetIcon(ICON_UP, OPENICON);
// 			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_PERC_DISCOUNT], 1 | DISP_LINE_LEFT);
// //			hide 2013-1-11 
// //			reason:During Order process, discount rate shouldn't be asked.
// // 			if(TRUE == gl_MaintenaceParam.bChangeDiscount)
// // 				ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | DISCNT_IN,0, SIZE_MAX_ID_TERMINAL, 
// // 				gl_MaintenaceParam.szDefDiscount,TIME_OUT_STANDARD_30S);
// // 			else
// 			{
// 				unsigned char szDefDiscount[SIZE_MAX_DISCOUNT + 1];
// 				sprintf(szDefDiscount, "%d.%d", atoi(gl_MaintenaceParam.szDefDiscount) / 100, atoi(gl_MaintenaceParam.szDefDiscount) % 100);
// 				DispString(szDefDiscount, DISPLAYLINE | DISP_LINE_LEFT);
// 				ucKey = YesNo(TIME_OUT_STANDARD_30S);
// 			}
// 			if(KEYENTER == ucKey || KEYDOWN == ucKey)
// 				ucStep = STEP_CHECKORDER;
// 			else if(KEYUP == ucKey)
// 				ucStep = STEP_QUANTITY;
// 			break;
		case STEP_CHECKORDER:
			ScrSetIcon(ICON_UP, OPENICON);
			CalcAmt(&stCurTran);
			DispString(gl_pLangSelPiont[STR_VALIDE_COMMANDE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);

			ucStep = STEP_INPUTPWD;
			if(!bMatch)
			{
				ucStep = STEP_MERCHANTCODE;
				break;
			}
			if(!(ucDone & 0x01))
			{
				ucStep = STEP_QUANTITY;
				break;
			}

			if(KEYENTER == ucKey)
				Prn_Order(stCurTran, PRN_PREVIEW);//print
			else if(KEYUP == ucKey)
				//ucStep = STEP_DISCOUNT;
				ucStep = STEP_QUANTITY;
			else if(KEYDOWN == ucKey)
			{
				ucStep = STEP_CHECKORDER;
				Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
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
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;
			stCurTran.ucTransType = ID_CashOrder;

			DispConnection();

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Get_ProvisionId_XML(stCurTran.szSalesman_PWD, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);
			}

			if(OK == ucRet)
			{
				int cnt = 3;

				stCurTran.ucTransType = ID_CashOrder;
				while(cnt-- >0)
				{
					GetCurTime(stCurTran.szDateTime);
					ucRet = Order_XML(stCurTran, stCurTran.szDateTime, MSG_CASHORDER, &stRespH, &stRespB, FALSE);
					if(ucRet != 1)
						break;
				}
				if(cnt <= 0 && 1 == ucRet)
					Cancellation_XML(stCurTran, &stRespH, &stRespB, FALSE);//cancel;
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
				memcpy(&gl_LastTran, &stCurTran, sizeof(TRANS_DATA));
				SaveProducts(stCurTran);
				WriteParam(WRT_SAVE | WRT_TXN);

				DispString(gl_pLangSelPiont[STR_SALESMAN_COPY], 1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
				Prn_Order(stCurTran, PRN_SALESMAN);//print
				DispString(gl_pLangSelPiont[STR_MERCHANT_COPY], 1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				Prn_Order(stCurTran, PRN_MERCHANT);//print
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			ucStep = STEP_NEWORDER;
			break;
		case STEP_NEWORDER:
			DispString("NEXT ORDER ?", 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0 != PubYesNo(TIME_OUT_STANDARD_30S))
				return;
			else
			{
				loop = TIME_RETRY;
				stCurTran.uiProductNum = 0;
				memset(stCurTran.szAmountWithDiscount, 0, sizeof(stCurTran.szAmountWithDiscount));
				memset(stCurTran.szAmountWithoutDiscount, 0, sizeof(stCurTran.szAmountWithoutDiscount));
				memset(stCurTran.stProducts, 0, sizeof(stCurTran.stProducts));
				memset(stCurTran.szSalesman_PWD, 0, sizeof(stCurTran.szSalesman_PWD));
				ucStep = STEP_QUANTITY;
				ucDone &= 0xFE;
			}
			break;
		}

		if(0xFF == ucKey)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_EXIT_ORDER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0 == PubYesNo(TIME_OUT_STANDARD_30S))
			{
				DataInit();
				return;
			}
			else
			{
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
		}
	}
}

static void Do_CreditOrder(void)
{
	enum
	{
		STEP_BARCODE = 0,
		STEP_MERCHANTCODE,
		STEP_TERMINALCODE,
		STEP_DISPLAY,
		STEP_QUANTITY,
		//STEP_DISCOUNT,
		STEP_CHECKORDER,
		STEP_INPUTPWD,
		STEP_UPLOADATA,
		STEP_PRINT,
		STEP_NEWORDER,
	};

	unsigned char ucRet, ucKey = 0;
	unsigned char ucDone = 0x00;
	unsigned char ucStep = STEP_BARCODE;
	unsigned char bManualInputOK = 0x00;  // 0x00 : false 0x01 : merchant id  is inputed 0x02 : terminal id is inputed
	//0x03 : true
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	xMerchantList stMerchant;
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char IsSN = TRUE;
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char bMatch = FALSE;
	// Add End
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stMerchant, 0, sizeof(xMerchantList));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_BARCODE:
			if(!gl_MaintenaceParam.bScanSerialNumber)
			{
				DispString("Disable Scan.", DISP_SCR_CENTER);
				ucStep = STEP_MERCHANTCODE;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);

			memset(szSerial, 0, sizeof(szSerial));
			bMatch = FALSE;
			ucRet = ScanBarCode(szSerial, &ucKey);

			if (0 == ucRet)
			{
				// Modified by Kim_LinHB 2013-7-22 v4.60
				unsigned int i;
				stCurTran.ucIdentifyType = TYPE_BARCODE;
				if(0 == memcmp(szSerial, "00", 2))
				{
					IsSN = FALSE;
					memmove(szSerial, szSerial + 2, sizeof(szSerial) - 2);
				}
				else
					IsSN = TRUE;
				if(IsSN)
				{
					for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
					{
						GetMerchant(FILE_MERCHANT, i, &stMerchant);
						if(0 == strcmp(szSerial, stMerchant.szSerial))
						{
							strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
							strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
							strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
							strcpy(stCurTran.szSerial, stMerchant.szSerial);
							bMatch = TRUE;
							break;
						}
					}
				}
				else
				{
					bMatch = SelTerminal(gl_SavParam.stRoute, szSerial, &stMerchant);
					if(bMatch)
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
						strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
					}
				}
				// Modify End
				ucStep = STEP_DISPLAY;
			}
			else
			{
				memset(szSerial, 0, sizeof(szSerial));
				stCurTran.ucIdentifyType = TYPE_MANUAL_ID;
				ucStep = STEP_MERCHANTCODE;
			}
			break;
		case STEP_MERCHANTCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Merchant ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szMerchantCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x01;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_BARCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_TERMINALCODE;
			break;
		case STEP_TERMINALCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Terminal ID:", 1 | DISP_LINE_LEFT);
			// Modified by Kim_LinHB 2013-4-2 v4.56
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szTermCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x02;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_MERCHANTCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_QUANTITY;
			break;
		case STEP_DISPLAY:
			ScrSetIcon(ICON_DOWN, OPENICON);

			if(TYPE_BARCODE == stCurTran.ucIdentifyType)
			{
				if(IsSN)
					DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				else
					DispString("Merchant ID", 1 | DISP_LINE_LEFT);

				DispString(szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				if(!bMatch)	
					DispString("not in the list", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			}
			else
			{
				unsigned int i;
				bMatch = FALSE;
				DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				if(0 == strlen(stCurTran.szMerchantCode) ||
					0 == strlen(stCurTran.szTermCode))
				{
					i = gl_SavParam.stRoute.uiMerchantListNum; // jump over the loop
				}
				for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
				{
					GetMerchant(FILE_MERCHANT, i, &stMerchant);
					if(0 == strcmp(stCurTran.szMerchantCode, stMerchant.szMerchantCode) &&
						0 == strcmp(stCurTran.szTermCode, stMerchant.szTermCode))
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
						bMatch = TRUE;
						break;
					}
				}
				if(bMatch)
					DispString(stCurTran.szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				else
				{
					DispString("MISMATCH", DISPLAYLINE | DISP_LINE_LEFT);
					DispString("cancel to re-enter", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
				}
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYCANCEL == ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					ucStep = STEP_MERCHANTCODE;
				bManualInputOK = 0x00;
				break;
			}
			else if((KEYENTER == ucKey || KEYDOWN == ucKey) && bMatch)
				ucStep = STEP_QUANTITY;
			else if(KEYUP ==ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_QUANTITY:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_QUANTITE], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				int uiCategory_Code = -1;
				if(0 == gl_SavParam.ucCategoryNum)
				{
					ScrCls();
					DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_OUT_STANDARD_30S);
					return;
				}

				while(1){
					uiCategory_Code = SelCateory(gl_SavParam, PRODUCT_ALL);
					if(-1 == uiCategory_Code)
					{
						if(stCurTran.uiProductNum > 0)
							ucDone |= 0x01;
						//ucStep = STEP_DISCOUNT;
						ucStep = STEP_CHECKORDER;
						break;
					}
					else if(-2 == uiCategory_Code)
					{
						ucKey = 0xFF;
						break;
					}
					else
					{
						stCurTran.ucTransType = ID_CreditOrder;
						SetProductsQuantity(&gl_SavParam, &stCurTran, uiCategory_Code, TRUE);
					}
				}
			}
			else if(KEYUP == ucKey)
			{
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYDOWN == ucKey)
			{
				if(stCurTran.uiProductNum > 0)
					ucStep = STEP_CHECKORDER;
			}
			break;

		case STEP_CHECKORDER:
			ScrSetIcon(ICON_UP, OPENICON);
			CalcAmt(&stCurTran);
			DispString(gl_pLangSelPiont[STR_VALIDE_COMMANDE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);

			ucStep = STEP_INPUTPWD;
			if(!bMatch)
			{
				ucStep = STEP_MERCHANTCODE;
				break;
			}
			if(!(ucDone & 0x01))
			{
				ucStep = STEP_QUANTITY;
				break;
			}

			if(KEYENTER == ucKey)
				Prn_Order(stCurTran, PRN_PREVIEW);//print
			else if(KEYUP == ucKey)
				//ucStep = STEP_DISCOUNT;
				ucStep = STEP_QUANTITY;
			else if(KEYDOWN == ucKey)
			{
				ucStep = STEP_CHECKORDER;
				Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
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
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;
			stCurTran.ucTransType = ID_CreditOrder;

			DispConnection();

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Get_ProvisionId_XML(stCurTran.szSalesman_PWD, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);
			}

			if(OK == ucRet)
			{
				int cnt = 3;

				stCurTran.ucTransType = ID_CreditOrder;
				while(cnt-- >0)
				{
					GetCurTime(stCurTran.szDateTime);
					ucRet = Order_XML(stCurTran, stCurTran.szDateTime, MSG_CREDITORDER, &stRespH, &stRespB, FALSE);
					if(ucRet != 1)
						break;
				}
				if(cnt <= 0 && 1 == ucRet)
					Cancellation_XML(stCurTran, &stRespH, &stRespB, FALSE);//cancel;
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
				memcpy(&gl_LastTran, &stCurTran, sizeof(TRANS_DATA));
				SaveProducts(stCurTran);
				WriteParam(WRT_SAVE | WRT_TXN);

				DispString(gl_pLangSelPiont[STR_SALESMAN_COPY], 1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
				Prn_Order(stCurTran, PRN_SALESMAN);//print
				DispString(gl_pLangSelPiont[STR_MERCHANT_COPY], 1 | DISP_LINE_CENTER);
				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				Prn_Order(stCurTran, PRN_MERCHANT);//print
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			ucStep = STEP_NEWORDER;
			break;
		case STEP_NEWORDER:
			DispString("NEXT ORDER ?", 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0 != PubYesNo(TIME_OUT_STANDARD_30S))
				return;
			else
			{
				loop = TIME_RETRY;
				stCurTran.uiProductNum = 0;
				memset(stCurTran.szAmountWithDiscount, 0, sizeof(stCurTran.szAmountWithDiscount));
				memset(stCurTran.szAmountWithoutDiscount, 0, sizeof(stCurTran.szAmountWithoutDiscount));
				memset(stCurTran.stProducts, 0, sizeof(stCurTran.stProducts));
				memset(stCurTran.szSalesman_PWD, 0, sizeof(stCurTran.szSalesman_PWD));
				ucStep = STEP_QUANTITY;
				ucDone &= 0xFE;
			}
			break;
		}

		if(0xFF == ucKey)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_EXIT_ORDER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0 == PubYesNo(TIME_OUT_STANDARD_30S))
			{
				DataInit();
				return;
			}
			else
			{
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
		}
	}
}
static void Do_ReturnOrder(void)
{

}

static void Do_CreditTransfer(void)
{
	enum
	{
		STEP_BARCODE = 0,
		STEP_MERCHANTCODE,
		STEP_TERMINALCODE,
		STEP_DISPLAY,
		STEP_AMOUNT,
		STEP_INPUTPWD,
		STEP_UPLOADATA,
		STEP_PRINT,
	};

	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_BARCODE;
	unsigned char bManualInputOK = 0x00;  // 0x00 : false 0x01 : merchant id  is inputed 0x02 : terminal id is inputed
	//0x03 : true
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	xMerchantList stMerchant;
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char IsSN = TRUE;
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char bMatch = FALSE;
	// Add End
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stMerchant, 0, sizeof(xMerchantList));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_BARCODE:
			if(!gl_MaintenaceParam.bScanSerialNumber)
			{
				DispString("Disable Scan.", DISP_SCR_CENTER);
				ucStep = STEP_MERCHANTCODE;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);

			memset(szSerial, 0, sizeof(szSerial));
			bMatch = FALSE;
			ucRet = ScanBarCode(szSerial, &ucKey);

			if (0 == ucRet)
			{
				//// Modified by Kim_LinHB 2013-7-22 v4.60
				unsigned int i;
				stCurTran.ucIdentifyType = TYPE_BARCODE;
				if(0 == memcmp(szSerial, "00", 2))
				{
					IsSN = FALSE;
					memmove(szSerial, szSerial + 2, sizeof(szSerial) - 2);
				}
				else
					IsSN = TRUE;
				if(IsSN)
				{
					for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
					{
						GetMerchant(FILE_MERCHANT, i, &stMerchant);
						if(0 == strcmp(szSerial, stMerchant.szSerial))
						{
							strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
							strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
							strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
							strcpy(stCurTran.szSerial, stMerchant.szSerial);
							bMatch = TRUE;
							break;
						}
					}
				}
				else
				{
					bMatch = SelTerminal(gl_SavParam.stRoute, szSerial, &stMerchant);
					if(bMatch)
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
						strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
					}
				}
				// Modify End
				ucStep = STEP_DISPLAY;
			}
			else
			{
				memset(szSerial, 0, sizeof(szSerial));
				stCurTran.ucIdentifyType = TYPE_MANUAL_ID;
				ucStep = STEP_MERCHANTCODE;
			}
			break;
		case STEP_MERCHANTCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Merchant ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szMerchantCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x01;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_BARCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_TERMINALCODE;
			break;
		case STEP_TERMINALCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Terminal ID:", 1 | DISP_LINE_LEFT);
			// Modified by Kim_LinHB 2013-4-2 v4.56
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szTermCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x02;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_MERCHANTCODE;
			break;
		case STEP_DISPLAY:
			ScrSetIcon(ICON_DOWN, OPENICON);

			if(TYPE_BARCODE == stCurTran.ucIdentifyType)
			{
				if(IsSN)
					DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				else
					DispString("Merchant ID", 1 | DISP_LINE_LEFT);

				DispString(szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				if(!bMatch)	
					DispString("not in the list", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			}
			else
			{
				unsigned int i;
				bMatch = FALSE;
				DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				if(0 == strlen(stCurTran.szMerchantCode) ||
					0 == strlen(stCurTran.szTermCode))
				{
					i = gl_SavParam.stRoute.uiMerchantListNum; // jump over the loop
				}
				for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
				{
					GetMerchant(FILE_MERCHANT, i, &stMerchant);
					if(0 == strcmp(stCurTran.szMerchantCode, stMerchant.szMerchantCode) &&
						0 == strcmp(stCurTran.szTermCode, stMerchant.szTermCode))
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
						bMatch = TRUE;
						break;
					}
				}
				if(bMatch)
					DispString(stCurTran.szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				else
				{
					DispString("MISMATCH", DISPLAYLINE | DISP_LINE_LEFT);
					DispString("cancel to re-enter", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
				}
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYCANCEL == ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					ucStep = STEP_MERCHANTCODE;
				bManualInputOK = 0x00;
				break;
			}
			else if((KEYENTER == ucKey || KEYDOWN == ucKey) && bMatch)
				ucStep = STEP_AMOUNT;
			else if(KEYUP ==ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_AMOUNT:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_AMOUNT], 1 | DISP_LINE_LEFT);

			ucKey = PubGetAmount(NULL, 2, 0, 12, 
				stCurTran.szAmountWithoutDiscount,TIME_OUT_STANDARD_30S, GA_SEPARATOR);
			if(0 == ucKey)
				ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
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
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;
			stCurTran.ucTransType = ID_CreditTransfer;

			DispConnection();

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Get_ProvisionId_XML(stCurTran.szSalesman_PWD, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);
			}

			if(OK == ucRet)
			{
				int cnt = 3;

				stCurTran.ucTransType = ID_CreditTransfer;
				while(cnt-- >0)
				{
					GetCurTime(stCurTran.szDateTime);
					ucRet = CreditTransferOrder_XML(stCurTran, stCurTran.szAmountWithDiscount, 
													&stRespH, &stRespB, FALSE);
					if(ucRet != 1)
						break;
				}
				if(cnt <= 0 && 1 == ucRet)
					Cancellation_XML(stCurTran, &stRespH, &stRespB, FALSE);//cancel;
			}
			else
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}

			CloseConn();
			AscInc(&gl_SavParam.usTraceNo);
			WriteParam(WRT_SAVE);

			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
			{
				memcpy(&gl_LastTran, &stCurTran, sizeof(TRANS_DATA));

 				DispString(gl_pLangSelPiont[STR_SALESMAN_COPY], 1 | DISP_LINE_CENTER);
//				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
				Prn_CreditTransferOrder(stCurTran, PRN_SALESMAN);//print
  				DispString(gl_pLangSelPiont[STR_MERCHANT_COPY], 1 | DISP_LINE_CENTER);
  				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
  				PubWaitKey(TIME_DELAY);
  				Prn_CreditTransferOrder(stCurTran, PRN_MERCHANT);//print
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

static void Do_Order(int sub)
{
	//////////////////////////////////////////////////////////////////////////
	if(!gl_bSettDown)
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_PAS_DE_TELEPARA], DISP_SCR_CENTER);
		PubWaitKey(TIME_DELAY);
		return;
	}
	// Added by Kim_LinHB 2013-9-11 v4.67
	if(gl_SavParam.uiCurOfflineNo > 0)
	{
		if(Do_OfflineBatchUpload(TRUE) != OK)
			return;
		ScrCls();
	}
	// Add End

	switch(sub)
	{
	case ID_CashOrder:
		Do_CashOrder();
		break;
	case ID_CreditOrder:
		Do_CreditOrder();
		break;
	case ID_ReturnOrder:
		Do_ReturnOrder();
		break;
	case ID_CreditTransfer:
		Do_CreditTransfer();
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

static void Do_OfflineOrder()
{
	enum
	{
		STEP_BARCODE = 0,
		STEP_MERCHANTCODE,
		STEP_TERMINALCODE,
		STEP_DISPLAY,
		STEP_QUANTITY,
		//STEP_DISCOUNT,
		STEP_CHECKORDER,
		STEP_INPUTPWD,
		STEP_SAVEDATA,
		STEP_NEWORDER,
	};
	//5.4	GetPhysicalStock  before
	int i;
	unsigned char ucRet, ucKey = 0;
	unsigned char ucDone = 0x00;
	unsigned char ucStep = STEP_BARCODE;
	unsigned char bManualInputOK = 0x00;  // 0x00 : false 0x01 : merchant id  is inputed 0x02 : terminal id is inputed
									//0x03 : true
	char loop = TIME_RETRY;
	TRANS_DATA stCurTran;
	xMerchantList stMerchant;
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char IsSN = TRUE;
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char bMatch = FALSE;
	// Add End
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stMerchant, 0, sizeof(xMerchantList));
	
	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_BARCODE:
			if(!gl_MaintenaceParam.bScanSerialNumber)
			{
				DispString("Disable Scan.", DISP_SCR_CENTER);
				ucStep = STEP_MERCHANTCODE;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);

			memset(szSerial, 0, sizeof(szSerial));
			bMatch = FALSE;
			ucRet = ScanBarCode(szSerial, &ucKey);

			if (0 == ucRet)
			{
				// Modified by Kim_LinHB 2013-7-22 v4.60
				unsigned int i;
				stCurTran.ucIdentifyType = TYPE_BARCODE;
				if(0 == memcmp(szSerial, "00", 2))
				{
					IsSN = FALSE;
					memmove(szSerial, szSerial + 2, sizeof(szSerial) - 2);
				}
				else
					IsSN = TRUE;
				if(IsSN)
				{
					for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
					{
						GetMerchant(FILE_MERCHANT, i, &stMerchant);
						if(0 == strcmp(szSerial, stMerchant.szSerial))
						{
							strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
							strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
							strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
							strcpy(stCurTran.szSerial, stMerchant.szSerial);
							bMatch = TRUE;
							break;
						}
					}
				}
				else
				{
					bMatch = SelTerminal(gl_SavParam.stRoute, szSerial, &stMerchant);
					if(bMatch)
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
						strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
					}
				}
				// Modify End
				ucStep = STEP_DISPLAY;
			}
			else
			{
				memset(szSerial, 0, sizeof(szSerial));
				stCurTran.ucIdentifyType = TYPE_MANUAL_ID;
				ucStep = STEP_MERCHANTCODE;
			}
			break;
		case STEP_MERCHANTCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Merchant ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szMerchantCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x01;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_BARCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_TERMINALCODE;
			break;
		case STEP_TERMINALCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Terminal ID:", 1 | DISP_LINE_LEFT);
			// Modified by Kim_LinHB 2013-4-2 v4.56
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szTermCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x02;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_MERCHANTCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_QUANTITY;
			break;
		case STEP_DISPLAY:
			ScrSetIcon(ICON_DOWN, OPENICON);

			if(TYPE_BARCODE == stCurTran.ucIdentifyType)
			{
				if(IsSN)
					DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				else
					DispString("Merchant ID", 1 | DISP_LINE_LEFT);

				DispString(szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				if(!bMatch)	
					DispString("not in the list", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			}
			else
			{
				unsigned int i;
				bMatch = FALSE;
				DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				if(0 == strlen(stCurTran.szMerchantCode) ||
					0 == strlen(stCurTran.szTermCode))
				{
					i = gl_SavParam.stRoute.uiMerchantListNum; // jump over the loop
				}
				for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
				{
					GetMerchant(FILE_MERCHANT, i, &stMerchant);
					if(0 == strcmp(stCurTran.szMerchantCode, stMerchant.szMerchantCode) &&
						0 == strcmp(stCurTran.szTermCode, stMerchant.szTermCode))
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
						bMatch = TRUE;
						break;
					}
				}
				if(bMatch)
					DispString(stCurTran.szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				else
				{
					DispString("MISMATCH", DISPLAYLINE | DISP_LINE_LEFT);
					DispString("cancel to re-enter", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
				}
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYCANCEL == ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					ucStep = STEP_MERCHANTCODE;
				bManualInputOK = 0x00;
				break;
			}
			else if((KEYENTER == ucKey || KEYDOWN == ucKey) && bMatch)
				ucStep = STEP_QUANTITY;
			else if(KEYUP ==ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_QUANTITY:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_QUANTITE], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
 				int uiCategory_Code = -1;
 				if(0 == gl_SavParam.ucCategoryNum)
 				{
 					ScrCls();
 					DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
 					PubWaitKey(TIME_OUT_STANDARD_30S);
 					return;
 				}
 
 				while(1){
 					uiCategory_Code = SelCateory(gl_SavParam, PRODUCT_PHYSICAL);
 					if(-1 == uiCategory_Code)
 					{
 						if(stCurTran.uiProductNum > 0)
 							ucDone |= 0x01;
 						//ucStep = STEP_DISCOUNT;
						ucStep = STEP_CHECKORDER;
 						break;
 					}
 					else if(-2 == uiCategory_Code)
 					{
 						ucKey = 0xFF;
 						break;
 					}
 					else
					{
						stCurTran.ucTransType = ID_OfflineOrder;
 						SetProductsQuantity(&gl_SavParam, &stCurTran, uiCategory_Code,FALSE);
					}
 				}
			}
			else if(KEYUP == ucKey)
			{
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYDOWN == ucKey)
			{
				//ucStep = STEP_DISCOUNT;
				if(stCurTran.uiProductNum > 0)
					ucStep = STEP_CHECKORDER;
			}
			break;
// 		case STEP_DISCOUNT:
// 			ScrSetIcon(ICON_UP, OPENICON);
// 			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_PERC_DISCOUNT], 1 | DISP_LINE_LEFT);
// //			hide 2013-1-11 
// //			reason:During Order process, discount rate shouldn't be asked.
// // 			if(TRUE == gl_MaintenaceParam.bChangeDiscount)
// // 				ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | DISCNT_IN,0, SIZE_MAX_ID_TERMINAL, 
// // 				gl_MaintenaceParam.szDefDiscount,TIME_OUT_STANDARD_30S);
// // 			else
// 			{
// 				unsigned char szDefDiscount[SIZE_MAX_DISCOUNT + 1];
// 				sprintf(szDefDiscount, "%d.%d", atoi(gl_MaintenaceParam.szDefDiscount) / 100, atoi(gl_MaintenaceParam.szDefDiscount) % 100);
// 				DispString(szDefDiscount, DISPLAYLINE | DISP_LINE_LEFT);
// 				ucKey = YesNo(TIME_OUT_STANDARD_30S);
// 			}
// 			if(KEYENTER == ucKey || KEYDOWN == ucKey)
// 				ucStep = STEP_CHECKORDER;
// 			else if(KEYUP == ucKey)
// 				ucStep = STEP_QUANTITY;
// 			break;
		case STEP_CHECKORDER:
			ScrSetIcon(ICON_UP, OPENICON);
			CalcAmt(&stCurTran);
			DispString(gl_pLangSelPiont[STR_VALIDE_COMMANDE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			
			ucStep = STEP_INPUTPWD;
			if(!bMatch)
			{
				ucStep = STEP_MERCHANTCODE;
				break;
			}
			if(!(ucDone & 0x01))
			{
				ucStep = STEP_QUANTITY;
				break;
			}

			if(KEYENTER == ucKey)
			{
				DispString(gl_pLangSelPiont[STR_ORDER_PREVIEW], 1 | DISP_LINE_CENTER);
				Prn_Order(stCurTran, PRN_PREVIEW);//print
			}
			else if(KEYUP == ucKey)
				//ucStep = STEP_DISCOUNT;
				ucStep = STEP_QUANTITY;
			else if(KEYDOWN == ucKey)
			{
				ucStep = STEP_CHECKORDER;
				Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_SAVEDATA;
			}
			break;
		case STEP_SAVEDATA:
			++gl_SavParam.uiCurOfflineNo;
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.ucTransType = ID_OfflineOrder;
			GetCurTime(stCurTran.szDateTime);

			AscInc(&gl_SavParam.usBatchNo);
			SaveProducts(stCurTran);
			WriteParam(WRT_SAVE);
			SaveOfflineTrans(&stCurTran, FILE_OFFLINE);

			DispString(gl_pLangSelPiont[STR_SALESMAN_COPY], 1 | DISP_LINE_CENTER);
			Prn_Order(stCurTran, PRN_SALESMAN);//print
			ScrCls();
			DispString(gl_pLangSelPiont[STR_MERCHANT_COPY], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
			PubWaitKey(TIME_OUT_STANDARD_30S);
			Prn_Order(stCurTran, PRN_MERCHANT);//print
			ucStep = STEP_NEWORDER;
			break;
		case STEP_NEWORDER:
			DispString("NEXT ORDER ?", 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0 != PubYesNo(TIME_OUT_STANDARD_30S))
				return;
			else
			{
				loop = TIME_RETRY;
				stCurTran.uiProductNum = 0;
				memset(stCurTran.szAmountWithDiscount, 0, sizeof(stCurTran.szAmountWithDiscount));
				memset(stCurTran.szAmountWithoutDiscount, 0, sizeof(stCurTran.szAmountWithoutDiscount));
				memset(stCurTran.stProducts, 0, sizeof(stCurTran.stProducts));
				memset(stCurTran.szSalesman_PWD, 0, sizeof(stCurTran.szSalesman_PWD));
				ucStep = STEP_QUANTITY;
				ucDone &= 0xFE;
			}
			break;
		}

		if(0xFF == ucKey)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_EXIT_ORDER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0 == PubYesNo(TIME_OUT_STANDARD_30S))
			{
				DataInit();
				return;
			}
			else
			{
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
		}
	}
}

static void Do_Offline_Visit()
{
	enum
	{
		STEP_BARCODE = 0,
		STEP_MERCHANTCODE,
		STEP_TERMINALCODE,
		STEP_DISPLAY,
		STEP_VALIDATE,
		STEP_INPUTPWD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_BARCODE;
	unsigned char bManualInputOK = 0x00;  // 0x00 : false 0x01 : merchant id  is inputed 0x02 : terminal id is inputed
									//0x03 : true
	char loop = TIME_RETRY;
	TRANS_DATA stCurTran;
	xMerchantList stMerchant;
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char IsSN = TRUE;
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char bMatch = FALSE;
	// Add End
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stMerchant, 0, sizeof(xMerchantList));

	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_BARCODE:
			if(!gl_MaintenaceParam.bScanSerialNumber)
			{
				DispString("Disable Scan.", DISP_SCR_CENTER);
				ucStep = STEP_MERCHANTCODE;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);

			memset(szSerial, 0, sizeof(szSerial));
			bMatch = FALSE;
			ucRet = ScanBarCode(szSerial, &ucKey);

			if (0 == ucRet)
			{
				// Modified by Kim_LinHB 2013-7-22 v4.60
				unsigned int i;
				stCurTran.ucIdentifyType = TYPE_BARCODE;
				if(0 == memcmp(szSerial, "00", 2))
				{
					IsSN = FALSE;
					memmove(szSerial, szSerial + 2, sizeof(szSerial) - 2);
				}
				else
					IsSN = TRUE;
				if(IsSN)
				{
					for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
					{
						GetMerchant(FILE_MERCHANT, i, &stMerchant);
						if(0 == strcmp(szSerial, stMerchant.szSerial))
						{
							strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
							strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
							strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
							strcpy(stCurTran.szSerial, stMerchant.szSerial);
							bMatch = TRUE;
							break;
						}
					}
				}
				else
				{
					bMatch = SelTerminal(gl_SavParam.stRoute, szSerial, &stMerchant);
					if(bMatch)
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
						strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
					}
				}
				// Modify End
				ucStep = STEP_DISPLAY;
			}
			else
			{
				memset(szSerial, 0, sizeof(szSerial));
				stCurTran.ucIdentifyType = TYPE_MANUAL_ID;
				ucStep = STEP_MERCHANTCODE;
			}
			break;
		case STEP_MERCHANTCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Merchant ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szMerchantCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x01;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_BARCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_TERMINALCODE;
			break;
		case STEP_TERMINALCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Terminal ID:", 1 | DISP_LINE_LEFT);
			// Modified by Kim_LinHB 2013-4-2 v4.56
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szTermCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x02;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_MERCHANTCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_VALIDATE;
			break;
		case STEP_DISPLAY:
			ScrSetIcon(ICON_DOWN, OPENICON);

			if(TYPE_BARCODE == stCurTran.ucIdentifyType)
			{
				if(IsSN)
					DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				else
					DispString("Merchant ID", 1 | DISP_LINE_LEFT);

				DispString(szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				if(!bMatch)	
					DispString("not in the list", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			}
			else
			{
				unsigned int i;
				bMatch = FALSE;
				DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				if(0 == strlen(stCurTran.szMerchantCode) ||
					0 == strlen(stCurTran.szTermCode))
				{
					i = gl_SavParam.stRoute.uiMerchantListNum; // jump over the loop
				}
				for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
				{
					GetMerchant(FILE_MERCHANT, i, &stMerchant);
					if(0 == strcmp(stCurTran.szMerchantCode, stMerchant.szMerchantCode) &&
						0 == strcmp(stCurTran.szTermCode, stMerchant.szTermCode))
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
						bMatch = TRUE;
						break;
					}
				}
				if(bMatch)
					DispString(stCurTran.szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				else
				{
					DispString("MISMATCH", DISPLAYLINE | DISP_LINE_LEFT);
					DispString("cancel to re-enter", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
				}
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYCANCEL == ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					ucStep = STEP_MERCHANTCODE;
				bManualInputOK = 0x00;
				break;
			}
			else if((KEYENTER == ucKey || KEYDOWN == ucKey) && bMatch)
				ucStep = STEP_VALIDATE;
			else if(KEYUP ==ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_VALIDATE:
			ScrSetIcon(ICON_UP, OPENICON);
			DispString(gl_pLangSelPiont[STR_VALIDATE_ORDER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISPLAYLINE | DISP_LINE_CENTER);
			
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				ucStep = STEP_INPUTPWD;
				if(!bMatch)
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
			{				
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYDOWN == ucKey)
				Beep_Speaker(BEEP_ERR);
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_PRINT;
			}
			break;
		case STEP_PRINT:
			++gl_SavParam.uiCurOfflineNo;
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.ucTransType = ID_OfflineVisit;
			GetCurTime(stCurTran.szDateTime);

			AscInc(&gl_SavParam.usBatchNo);
			WriteParam(WRT_SAVE);

			ucRet = SaveOfflineTrans(&stCurTran, FILE_OFFLINE);
			if(OK == ucRet)
			{
				Prn_Visit(stCurTran);//print
			}
			else
				vTrace("SaveOfflineError:\n", &ucRet, sizeof(ucRet), TRACE_NONE, TYPE_N);//print error
			return;
		}

		if(0xFF == ucKey)
			return;
	}
}

static void Do_OfflineCancelLastOrder()
{
	enum
	{
		STEP_PRINT,
		STEP_CANCEL_CHECK,
		STEP_INPUTPWD,
		STEP_SHOW,
	};
	unsigned char ucKey = 0;
	unsigned char ucStep = STEP_PRINT;
	char loop = TIME_RETRY;
	TRANS_DATA stCurTran;
	memset(&stCurTran, 0 ,sizeof(stCurTran));
	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_PRINT:
			if(OK != GetOfflineLastTrans(&stCurTran))
			{
				DispString(gl_pLangSelPiont[STR_PB_ORDER_EMPTY], DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return;
			}
			//print
			ucStep = STEP_CANCEL_CHECK;
			break;
		case STEP_CANCEL_CHECK:
			DispString(gl_pLangSelPiont[STR_ANNULER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_DERNIERE_VENTE], DISPLAYLINE | DISP_LINE_CENTER);
			if(0 != PubYesNo(TIME_OUT_STANDARD_30S))
				return;
			if (gl_MaintenaceParam.bCancelSale)
				ucStep = STEP_INPUTPWD;
			else
			{
				ScrCls();
				DispString(gl_pLangSelPiont[STR_NOT_ALLOWED_FOR], 1 | DISP_LINE_CENTER);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return;
			}
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_SHOW;
			}
			break;
		case STEP_SHOW:
			{
				unsigned char szCurDate[SIZE_DATE_TIME + 1 + 2];
				unsigned char szTranDate[SIZE_DATE_TIME + 1 + 2];
				unsigned long ulCurDate, ulTranDate;

				strcpy(szCurDate, PRE_YEAR);
				strcpy(szTranDate, PRE_YEAR);
				strcat(szTranDate, stCurTran.szDateTime);
				GetCurTime(szCurDate + 2);
				ulCurDate = PubTime2Long(szCurDate);
				ulTranDate = PubTime2Long(szTranDate);
				if(ulCurDate - ulTranDate >= TIME_CANCL_LIMIT * 60)
				{
					ScrCls();
					DispString("Too Old", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					return;
				}
			}
			if(OK == DeleteOfflineLastTrans())
			{
				--gl_SavParam.uiCurOfflineNo;
				stCurTran.usBatchNo = gl_SavParam.usBatchNo;
				stCurTran.ucTransType = ID_OfflineCancelLastOrder;
				GetCurTime(stCurTran.szDateTime);

				AscInc(&gl_SavParam.usBatchNo);
				//SaveProducts(stCurTran);
				WriteParam(WRT_SAVE);
				SaveOffCancel(&stCurTran);
				DispString(gl_pLangSelPiont[STR_ABANDON], DISPLAYLINE | DISP_LINE_CENTER);
				Prn_CancelLastOrder(stCurTran);//print
			}
			else
				vTrace("DeleteOfflineLastTrans:\nerror", NULL, 0, TRACE_NONE, 0);
			PubWaitKey(TIME_OUT_STANDARD_30S);
			return;
		}
		if(0xFF == ucKey)
			return;
	}
}

static int Do_OfflineBatchUpload(uchar bAuto)
{
	enum
	{
		STEP_INPUTPWD,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_INPUTPWD;
	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	int Done = 0;
	int orderCnt = 0;
	int visitCnt = 0;

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	if(bAuto)
	{
		strcpy(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd); // Added by Kim_LinHB 2013-7-24 v4.60
		ucStep = STEP_UPLOAD;
	}

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return -1;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			if(0 == gl_SavParam.uiCurOfflineNo)
			{
				ScrCls();
				if(!bAuto)
				{
					DispString("No Offline Tran.", DISP_SCR_CENTER);
					vTrace("No Offline Tran.", NULL, 0, TRACE_NONE, 0);
					PubWaitKey(TIME_DELAY);
				}
				return OK;
			}
			else
			{
				TRANS_DATA tmp;
				unsigned char szDateTime[SIZE_DATE_TIME + 1];
				memset(&tmp, 0, sizeof(TRANS_DATA));

				tmp.usBatchNo = gl_SavParam.usBatchNo;
				tmp.ucTransType = ID_OfflineBatchUpload;

				if(bAuto)
				{
					ScrCls();
					DispString("Upload Offline", DISP_SCR_CENTER);
				}
				// Removed by Kim_LinHB 2013-9-11 v4.67
				//else
				//	DispConnection();
				// Remove End


				ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

				if (ucRet != OK)
				{
					CloseConn();
					Do_Error(ucRet, stRespH);
					return -1;
				}
				orderCnt = GetOfflineTransCnt(ID_OfflineOrder);
				if (ucRet != OK)
				{
					CloseConn();
					Do_Error(ucRet, stRespH);
					return -1;
				}
				if(orderCnt > 0)
				{
					int ii;

					for(ii = 0; ii < orderCnt; ++ii)
					{
						int cnt = 3;
						int iRetryCnt = TIME_RETRY;
						tmp.usTraceNo = gl_SavParam.usTraceNo;
						AscInc(&gl_SavParam.usTraceNo);
						WriteParam(WRT_SAVE);
						GetOfflineOrder(ii - Done, &tmp);
						do 
						{
							ucRet = Get_ProvisionId_XML(szSalesman_PWD, &stRespH, &stRespB, tmp.szProvisionID, FALSE);
							--iRetryCnt;
						} while (1 == ucRet && iRetryCnt > 0);
						if(ucRet != 0)
						{
							CloseConn();
							Do_Error(ucRet, stRespH);
							return -1;
						}
						tmp.ucTransType = ID_OfflineOrder;
						while(cnt-- >0)
						{
							GetCurTime(szDateTime);
							ucRet = Order_XML(tmp, szDateTime, MSG_CASHORDER, &stRespH, &stRespB,FALSE);
							if(ucRet != 1)
								break;
						}
						if(cnt <= 0 && 1 == ucRet)
							Cancellation_XML(tmp, &stRespH, &stRespB, FALSE);//cancel;
						
						if(OK == ucRet)
						{
							DelOfflineOrder(ii - Done);
							++Done;
							--gl_SavParam.uiCurOfflineNo;
						}
					}
				
					if (ucRet != OK)
					{
						CloseConn();
						Do_Error(ucRet, stRespH);
						return -1;
					}
				}

				visitCnt = GetOfflineTransCnt(ID_OfflineVisit);
				if(visitCnt > 0)
				{
					if(OK == ucRet)
					{
						int iRetryCnt = TIME_RETRY;
						tmp.usTraceNo = gl_SavParam.usTraceNo;
						AscInc(&gl_SavParam.usTraceNo);
						WriteParam(WRT_SAVE);
						do 
						{
							ucRet = Get_ProvisionId_XML(szSalesman_PWD, &stRespH, &stRespB, tmp.szProvisionID, FALSE);
							--iRetryCnt;
						} while (1 == ucRet && iRetryCnt > 0);
					}
					if(ucRet != 0)
					{
						CloseConn();
						Do_Error(ucRet, stRespH);
						return -1;
					}
					tmp.ucTransType = ID_OfflineVisit;
					GetCurTime(tmp.szDateTime);
					if(OK == ucRet)
					{
						int cnt = 3;

						while(cnt-- >0)
						{
							ucRet = SaveSalesmanVisit_XML(tmp, &stRespH, &stRespB, FALSE);// Offline visit upload
							if(ucRet != 1)
								break;
						}
// 						if(cnt <= 0)
// 							Cancellation_XML(tmp, &stRespH, &stRespB, TRUE);//cancel;
						PubDebugTx("vvv\n");
						if (ucRet != OK)
						{
							CloseConn();
							Do_Error(ucRet, stRespH);
							return -1;
						}
						else
							gl_SavParam.uiCurOfflineNo -= visitCnt;
					}
				}
			}
			
			CloseConn();
			AscInc(&gl_SavParam.usBatchNo);
			WriteParam(WRT_SAVE);
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
			{
				Prn_BatchUpload(FALSE, orderCnt + visitCnt, Done, visitCnt);//print
				gl_SavParam.uiCurOfflineNo = 0;
				WriteParam(WRT_SAVE);
				remove(FILE_OFFLINE);
				remove(FILE_OFF_CANCEL);
				DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
				PubWaitKey(TIME_OUT_STANDARD_30S);
				return OK;
			}
			else
			{
				Do_Error(ucRet, stRespH);
				return -1;
			}
			
		}
		if(0xFF == ucKey)
			return -1;
	}
}

static void Do_Offline(int sub)
{
	switch(sub)
	{
	case ID_OfflineOrder:
		// Modified by Kim_LinHB 2013-9-5 v4.66
		if(GetOfflineTransCnt(ID_OfflineOrder) > 100)
		{
			ScrCls();
			DispString("please upload beach", 1 | DISP_LINE_CENTER);
			DispString("offline orders", DISPLAYLINE | DISP_LINE_CENTER);
			DispString("are full", DISPLAYLINE + 2| DISP_LINE_CENTER);
			Beep_Speaker(BEEP_ERR);
			PubWaitKey(TIME_DELAY); //2 sec
			return;
		}
		// Add End
		Do_OfflineOrder();
		break;
	case ID_OfflineVisit:
		// Modified by Kim_LinHB 2013-9-5 v4.66
		if(GetOfflineTransCnt(ID_OfflineVisit) > 100)
		{
			ScrCls();
			DispString("please upload beach", 1 | DISP_LINE_CENTER);
			DispString("offline orders", DISPLAYLINE | DISP_LINE_CENTER);
			DispString("are full", DISPLAYLINE + 2| DISP_LINE_CENTER);
			Beep_Speaker(BEEP_ERR);
			PubWaitKey(TIME_DELAY); //2 sec
			return;
		}
		// Add End
		Do_Offline_Visit();
		break;
	case ID_OfflineCancelLastOrder:
		Do_OfflineCancelLastOrder();
		break;
	case ID_OfflineBatchUpload:
		Do_OfflineBatchUpload(FALSE);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

static void Do_CashCollection(int sub)
{

}
static void Do_BankDeposit(int sub)
{

}

//////////////////////////////////////////////////////////////////////////
static void Do_PendingOrders(void)
{
	enum
	{
		STEP_BARCODE = 0,
		STEP_CHECKBARCODE,
		STEP_MERCHANTCODE,
		STEP_TERMINALCODE,
		STEP_DISPLAY,
		STEP_CHECKORDER,
		STEP_INPUTPWD,
		STEP_UPLOADATA,
		STEP_PRINT,
	};
	//5.4	GetPhysicalStock  before
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_BARCODE;
	unsigned char bManualInputOK = 0x00;  // 0x00 : false 0x01 : merchant id  is inputed 0x02 : terminal id is inputed
								//0x03 : true
	
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	PENDING_ORDERS stPendOrder;
	xMerchantList stMerchant;
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char IsSN = TRUE;
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char bMatch = FALSE;
	// Add End
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stMerchant, 0, sizeof(xMerchantList));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	
	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucStep)
		{
		case STEP_BARCODE:
			if(!gl_MaintenaceParam.bScanSerialNumber)
			{
				DispString("Disable Scan.", DISP_SCR_CENTER);
				ucStep = STEP_MERCHANTCODE;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString(gl_pLangSelPiont[STR_SCAN_BARCODE], 1 | DISP_LINE_LEFT);

			memset(szSerial, 0, sizeof(szSerial));
			bMatch = FALSE;
			ucRet = ScanBarCode(szSerial, &ucKey);

			if (0 == ucRet)
			{
				// Modified by Kim_LinHB 2013-7-22 v4.60
				unsigned int i;
				stCurTran.ucIdentifyType = TYPE_BARCODE;
				if(0 == memcmp(szSerial, "00", 2))
				{
					IsSN = FALSE;
					memmove(szSerial, szSerial + 2, sizeof(szSerial) - 2);
				}
				else
					IsSN = TRUE;
				if(IsSN)
				{
					for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
					{
						GetMerchant(FILE_MERCHANT, i, &stMerchant);
						if(0 == strcmp(szSerial, stMerchant.szSerial))
						{
							strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
							strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
							strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
							strcpy(stCurTran.szSerial, stMerchant.szSerial);
							bMatch = TRUE;
							break;
						}
					}
				}
				else
				{
					bMatch = SelTerminal(gl_SavParam.stRoute, szSerial, &stMerchant);
					if(bMatch)
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szMerchantCode, stMerchant.szMerchantCode);
						strcpy(stCurTran.szTermCode, stMerchant.szTermCode);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
					}
				}
				// Modify End
				ucStep = STEP_DISPLAY;
			}
			else
			{
				memset(szSerial, 0, sizeof(szSerial));
				stCurTran.ucIdentifyType = TYPE_MANUAL_ID;
				ucStep = STEP_MERCHANTCODE;
			}
			break;
		case STEP_MERCHANTCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Merchant ID:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szMerchantCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x01;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_TERMINALCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_BARCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_TERMINALCODE;
			break;
		case STEP_TERMINALCODE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(!gl_MaintenaceParam.bInputSerial)
			{
				DispString("Disable Input.", DISP_SCR_CENTER);
				ucStep = STEP_DISPLAY;
				PubWaitKey(TIME_DELAY);
				break;
			}
			DispString("Terminal ID:", 1 | DISP_LINE_LEFT);
			// Modified by Kim_LinHB 2013-4-2 v4.56
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				stCurTran.szTermCode,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				bManualInputOK |= 0x02;

				if(0x03 == bManualInputOK)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
			else if(KEYUP == ucKey)
				ucStep = STEP_MERCHANTCODE;
			else if(KEYDOWN == ucKey)
				ucStep = STEP_CHECKORDER;
			break;
		case STEP_DISPLAY:
			ScrSetIcon(ICON_DOWN, OPENICON);

			if(TYPE_BARCODE == stCurTran.ucIdentifyType)
			{
				if(IsSN)
					DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				else
					DispString("Merchant ID", 1 | DISP_LINE_LEFT);
				
				DispString(szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				if(!bMatch)	
					DispString("not in the list", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			}
			else
			{
				unsigned int i;
				bMatch = FALSE;
				DispString(gl_pLangSelPiont[STR_MERCHANT_SERIAL], 1 | DISP_LINE_LEFT);
				if(0 == strlen(stCurTran.szMerchantCode) ||
					0 == strlen(stCurTran.szTermCode))
				{
					i = gl_SavParam.stRoute.uiMerchantListNum; // jump over the loop
				}
				for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
				{
					GetMerchant(FILE_MERCHANT, i, &stMerchant);
					if(0 == strcmp(stCurTran.szMerchantCode, stMerchant.szMerchantCode) &&
						0 == strcmp(stCurTran.szTermCode, stMerchant.szTermCode))
					{
						strcpy(stCurTran.szMerchantID, stMerchant.szMerchantID);
						strcpy(stCurTran.szSerial, stMerchant.szSerial);
						bMatch = TRUE;
						break;
					}
				}
				if(bMatch)
					DispString(stCurTran.szSerial, DISPLAYLINE | DISP_LINE_LEFT);
				else
				{
					DispString("MISMATCH", DISPLAYLINE | DISP_LINE_LEFT);
					DispString("cancel to re-enter", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
				}
			}

			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			if(KEYCANCEL == ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					ucStep = STEP_MERCHANTCODE;
				bManualInputOK = 0x00;
				break;
			}
			else if((KEYENTER == ucKey || KEYDOWN == ucKey) && bMatch)
				ucStep = STEP_CHECKORDER;
			else if(KEYUP ==ucKey)
			{
				if(TYPE_BARCODE == stCurTran.ucIdentifyType)
					ucStep = STEP_BARCODE;
				else
					Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_CHECKORDER:
			ScrSetIcon(ICON_UP, OPENICON);
			CalcAmt(&stCurTran);
			DispString(gl_pLangSelPiont[STR_VALIDE_COMMANDE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			
			ucStep = STEP_INPUTPWD;
			if(!bMatch)
			{
				ucStep = STEP_MERCHANTCODE;
				break;
			}

			if(KEYENTER == ucKey)
				ucStep = STEP_INPUTPWD;
			if(KEYUP == ucKey)
				ucStep = STEP_DISPLAY;
			if(KEYDOWN == ucKey)
			{
				ucStep = STEP_CHECKORDER;
				Beep_Speaker(BEEP_ERR);
			}
			if(KEYCANCEL == ucKey)
			{
				ucKey = 0xFF;
				Beep_Speaker(BEEP_ERR);
			}
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
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
			
			DispConnection();

			do 
			{
				ucRet = PendingOrders_XML(stCurTran, &stPendOrder, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
				Prn_PendingOrders(stCurTran, stPendOrder);//print
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
				DataInit();
				return;
			}
			else
			{
				if(bMatch)
					ucStep = STEP_DISPLAY;
				else
					ucStep = STEP_MERCHANTCODE;
			}
		}
	}
}
static void Do_SalesmanInfo(void)
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_INPUTPWD;
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	memset(&szSalesman_PWD, 0 ,sizeof(szSalesman_PWD));

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
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:

			DispConnection();

			do 
			{
				ucRet = SalesmanInfo_XML(szSalesman_PWD, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			//print
			if(OK == ucRet)
				Prn_SalesmanInfo();
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
static void Do_SalesmanRoute(void)
{
	enum
	{
		STEP_ENTER = 0,
		STEP_SHOWDATE,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_ENTER;
	unsigned char timeASCII[SIZE_DATE_TIME + 1 + 2];
	unsigned char bDuplicate = FALSE;
	
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_ENTER:
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
				ucStep = STEP_SHOWDATE;
			break;
 		case STEP_SHOWDATE:
 			strcpy(timeASCII, PRE_YEAR);
 			GetCurTime(timeASCII + 2);
//  		DispString("YYYY/MM/DD", 1 | DISP_LINE_LEFT);
//  		ucKey = GetInputTime(DISPLAYLINE, timeASCII + 2, TIME_GET_DATE, TIME_OUT_STANDARD_30S);
//  		if(KEYENTER == ucKey)
 			if(0 == strncmp(timeASCII + 2, gl_SavParam.stRoute.szMerchantListDate, 6))
 				bDuplicate = TRUE;
 
 			ucStep = STEP_UPLOAD;
 			break;
		case STEP_UPLOAD:
			ucRet = OK;
// 			DispConnection();
// 
// 			{
// 				unsigned int RequestIndex = gl_SavParam.uiMerchantListIndex;
// 				unsigned char bContinue = FALSE;
// 
// 				if(0 == RequestIndex)
// 					gl_SavParam.stRoute.ucMerchantListNum = 0;
// 				do
// 				{
// 					iRetryCnt = TIME_RETRY;
// 					do 
// 					{
// 						ucRet = SalesmanMerchantList_XML("", timeASCII + 2, &gl_SavParam.stRoute, 
// 							&bContinue, RequestIndex, &stRespH, &stRespB, TRUE);
// 						--iRetryCnt;
// 					} while (1 == ucRet && iRetryCnt > 0);
// 					if(OK == ucRet)
// 						++RequestIndex;
// 				}while (OK == ucRet && bContinue);
// 
// 				if(OK ==ucRet && !bContinue)
// 					gl_SavParam.uiMerchantListIndex = 0;
// 				else
// 					gl_SavParam.uiMerchantListIndex = RequestIndex;
// 				WriteParam();
// 			}
			
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
			{
				if(bDuplicate)
					DispString("Duplicate", DISPLAYLINE | DISP_LINE_CENTER);
				Prn_SalesmanRoute(gl_SavParam.stRoute, bDuplicate);//print 
			}
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
static void Do_PrintDiscount(void)
{
	unsigned char ucKey;
	DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
	ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
	if(0 == ucKey)
	{
		Prn_Discount();//print
	}
}
// Added by Kim_LinHB 2013-7-18 v4.60
static void Do_ChangePwd(void)
{
	enum
	{
		STEP_SURE = 0,
		STEP_INPUTPWD,
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
	unsigned char ucSel = STEP_SURE;
	char loop = TIME_RETRY;
	char ucRetry = TIME_RETRY;

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
		case STEP_SURE:
			DispString("Change PWD", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				ucSel = STEP_INPUTPWD;
			else
				return;
			break;
		case STEP_INPUTPWD:
			DispString("Current Pwd", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stByWho.szPwd, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				ucSel = STEP_JUDGE;
			break;
		case STEP_JUDGE:
			if (0 == strcmp(gl_MaintenaceParam.stUser.szPwd, stByWho.szPwd))
			{
				strcpy(stByWho.szName,gl_MaintenaceParam.stUser.szName);
				ucSel = STEP_INPUTNEWPWD;
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
		case STEP_INPUTNEWPWD:
			DispString("New Pwd:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				ucSel = STEP_INPUTNEWPWDAGAIN;
			else
				return;
			break;
		case STEP_INPUTNEWPWDAGAIN:
			DispString("New Pwd again:", 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew2, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				ucSel = STEP_VERIFY;
			else if(KEYCANCEL == ucKey)
				ucSel = STEP_INPUTNEWPWD;
			else
				return;
			break;
		case STEP_VERIFY:
			if (0 == strcmp(szPwdNew, szPwdNew2))
			{
				strcpy(stWho.szName, stByWho.szName);
				strcpy(stWho.szPwd, szPwdNew);
				ucSel = STEP_UPLOAD;
			}
			else
			{
				ScrCls();
				DispString("Don't Match.", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_ERR);
				PubWaitKey(TIME_DELAY);
				--ucRetry;
				if(ucRetry > 0)
					ucSel = STEP_INPUTNEWPWD;
				else
					return;
			}
			break;
		case STEP_UPLOAD:
			DispConnection();
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
				DispString("Table Users", DISPLAYLINE |DISP_LINE_CENTER);
				do 
				{
					ucRet = Get_User_XML(&stRespH, &stRespB, TRUE);
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
				Beep_Speaker(BEEP_OK);
				WriteParam(WRT_MAINTA);
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
// Add End

static void Do_Information(int sub)
{
	switch(sub)
	{
	case ID_Pending_Orders_ECC:
		Do_PendingOrders();
		break;
	case ID_Salesman_Infos_ECC:
		Do_SalesmanInfo();
		break;
	case ID_SalesmanRoute:
		Do_SalesmanRoute();
		break;
	case ID_PrintDiscount:
		Do_PrintDiscount();
		break;
	case ID_ChangePWD:
		Do_ChangePwd();
		break;
	}
}
//////////////////////////////////////////////////////////////////////////


static void Do_OfflineBatchUploadRep(void)
{
	unsigned char ucKey;
	kbflush();
	DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
	ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
	if(0 == ucKey)
	{
		if(gl_SavParam.uiCurOfflineNo > 0)
		{
			Prn_BatchUpload(TRUE, 0, 0, 0);
		}
		else
		{
			ScrCls();
			DispString("No Offline Tran.", DISP_SCR_CENTER);
			vTrace("No Offline Tran.", NULL, 0, TRACE_NONE, 0);
			PubWaitKey(TIME_DELAY);
		}
	}
}
static void Do_OfflinePhysicalStockRep(void)
{
	unsigned char ucKey;
	kbflush();
	DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
	ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
	if(0 == ucKey)
	{
		Prn_PhysicalStockReport();//print
	}
}
static void Do_OrderRep(void)
{
	enum
	{
		STEP_ENTER = 0,
		STEP_FROM,
		STEP_TO,
		STEP_CHECK,
		STEP_INPUTPWD,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_ENTER;
	unsigned char from[SIZE_DATE_TIME + 1 + 2];
	unsigned char to[SIZE_DATE_TIME + 1 + 2];
	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	char loop = TIME_RETRY;
	OrderReport stOR_Cash, stOR_Credit, stOR_Credit_Transfer;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	memset(&szSalesman_PWD, 0 ,sizeof(szSalesman_PWD));
	memset(from, 0, sizeof(from));
	memset(to, 0,  sizeof(to));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_ENTER:
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
				ucStep = STEP_FROM;
			break;
		case STEP_FROM:
			DispString("From", 1 | DISP_LINE_CENTER);
			GetCurTime(from + 2);
			memcpy(from, PRE_YEAR, 2);
			memcpy(from + 8, "0000", 4); // hhmm

			ucKey = GetInputTime(DISPLAYLINE, from + 2, TIME_GET_DATE | TIME_GET_TIME, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				ucStep = STEP_TO;
			break;
		case STEP_TO:
			DispString("To", 1 | DISP_LINE_CENTER);
			GetCurTime(to + 2);
			memcpy(to, PRE_YEAR, 2);
			memcpy(to + 8, "2359", 4); // hhmm

			ucKey = GetInputTime(DISPLAYLINE, to + 2, TIME_GET_DATE | TIME_GET_TIME, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(strcmp(to, from) >= 0)
					ucStep = STEP_CHECK;
				else
				{
					ScrCls();
					DispString("From > to", DISP_SCR_CENTER);
					PubWaitKey(TIME_DELAY);
					ucStep = STEP_FROM;
				}
			}
			break;
		case STEP_CHECK:
			DispString("Get report?", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			strcat(from, "00"); // ss
			strcat(to, "59"); // ss
			iRetryCnt = TIME_RETRY;
			do 
			{
				ucRet = OrderReports_XML(szSalesman_PWD, from + 2, to + 2, ORDER_CASH, &stOR_Cash, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);

			// Added by Kim_LinHB 2013-6-5
			iRetryCnt = TIME_RETRY;
			do 
			{
				ucRet = OrderReports_XML(szSalesman_PWD, from + 2, to + 2, ORDER_CREDIT_TRANSFER, &stOR_Credit_Transfer, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			// Add End

			// Added by Kim_LinHB 2013-7-22 v4.60
			iRetryCnt = TIME_RETRY;
			do 
			{
				ucRet = OrderReports_XML(szSalesman_PWD, from + 2, to + 2, ORDER_CREDIT, &stOR_Credit, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			// Add End
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			//print
			if(OK == ucRet)
			{
				Prn_OrderReport(from, to, ORDER_CASH, stOR_Cash);
				PubWaitKey(1);
				Prn_OrderReport(from, to, ORDER_CREDIT_TRANSFER, stOR_Credit_Transfer);
				PubWaitKey(1);
				Prn_OrderReport(from, to, ORDER_CREDIT, stOR_Credit); // Added by Kim_LinHB 2013-7-22 v4.60
			}
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
// Added by Kim_LinHB 2013-7-30 v4.60
static void Do_OrderRep_Prod(void)
{
	enum
	{
		STEP_ENTER = 0,
		STEP_FROM,
		STEP_TO,
		STEP_CHECK,
		STEP_INPUTPWD,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_ENTER;
	unsigned char from[SIZE_DATE_TIME + 1 + 2];
	unsigned char to[SIZE_DATE_TIME + 1 + 2];
	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	char loop = TIME_RETRY;
	OrderReport_Prod stOR;

	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	memset(&szSalesman_PWD, 0 ,sizeof(szSalesman_PWD));
	memset(from, 0, sizeof(from));
	memset(to, 0,  sizeof(to));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_ENTER:
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
				ucStep = STEP_FROM;
			break;
		case STEP_FROM:
			DispString("From", 1 | DISP_LINE_CENTER);
			GetCurTime(from + 2);
			memcpy(from, PRE_YEAR, 2);
			memcpy(from + 8, "0000", 4); // hhmm

			ucKey = GetInputTime(DISPLAYLINE, from + 2, TIME_GET_DATE | TIME_GET_TIME, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				ucStep = STEP_TO;
			break;
		case STEP_TO:
			DispString("To", 1 | DISP_LINE_CENTER);
			GetCurTime(to + 2);
			memcpy(to, PRE_YEAR, 2);
			memcpy(to + 8, "2359", 4); // hhmm

			ucKey = GetInputTime(DISPLAYLINE, to + 2, TIME_GET_DATE | TIME_GET_TIME, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(strcmp(to, from) >= 0)
					ucStep = STEP_CHECK;
				else
				{
					ScrCls();
					DispString("From > to", DISP_SCR_CENTER);
					PubWaitKey(TIME_DELAY);
					ucStep = STEP_FROM;
				}
			}
			break;
		case STEP_CHECK:
			DispString("Get report?", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			strcat(from, "00"); // ss
			strcat(to, "59"); // ss
			iRetryCnt = TIME_RETRY;

			//for test
			//strcpy(stOR.szSalesmanId, "001919191919");
			//strcpy(stOR.szSalesmanName, "EVD Test - S90");
			//stOR.uiTotalOrders = 9;
			//strcpy(stOR.szTotAmt, "14000");
			//strcpy(stOR.szDiscount, "3000");
			//stOR.uiGroupCnt = 2;
			//stOR.uiProductCnt = 4;

			//stOR.Groups[0].uiTotalOrders = 4;
			//strcpy(stOR.Groups[0].szTitle, "ELECTRONIC(S) PRODUCT(S)");
			//strcpy(stOR.Groups[0].szName, "SAWA EXPRESS");
			//strcpy(stOR.Groups[0].szTotAmt, "6000");
			//strcpy(stOR.Groups[0].szDiscount, "1000");
			//stOR.Groups[0].uiProductTypes = 2;

			//stOR.Products[0].groupIndex = 0;
			//strcpy(stOR.Products[0].szName, "TEST ALIJAWAL 10 (10.00 SR)");
			//strcpy(stOR.Products[0].szTotAmt, "2000");
			//stOR.Products[0].uiTotalOrders = 2;

			//stOR.Products[1].groupIndex = 0;
			//strcpy(stOR.Products[1].szName, "TEST ALIJAWAL 20 (20.00 SR)");
			//strcpy(stOR.Products[1].szTotAmt, "4000");
			//stOR.Products[1].uiTotalOrders = 2;

			//stOR.Groups[1].uiTotalOrders = 5;
			//strcpy(stOR.Groups[1].szTitle, "PHYSICAL(S) PRODUCT(S)");
			//strcpy(stOR.Groups[1].szName, "SAWA PHYSICAL");
			//strcpy(stOR.Groups[1].szTotAmt, "8000");
			//strcpy(stOR.Groups[1].szDiscount, "2000");
			//stOR.Groups[0].uiProductTypes = 2;

			//stOR.Products[2].groupIndex = 1;
			//strcpy(stOR.Products[2].szName, "SAWA PHYSICAL 10 (10.00 SR)");
			//strcpy(stOR.Products[2].szTotAmt, "2000");
			//stOR.Products[2].uiTotalOrders = 2;

			//stOR.Products[3].groupIndex = 1;
			//strcpy(stOR.Products[3].szName, "SAWA PHYSICAL 20 (20.00 SR)");
			//strcpy(stOR.Products[3].szTotAmt, "6000");
			//stOR.Products[3].uiTotalOrders = 3;

			//ucRet = 0;
			//ucStep = STEP_PRINT;
			//break;

			do 
			{
				ucRet = OrderReports_Prod_XML(szSalesman_PWD, from + 2, to + 2, &stOR, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);

			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			//print
			if(OK == ucRet)
			{
				Prn_OrderReport_Prod(from, to, stOR);
			}
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
// Add End
static void Do_OnlineStocksRep(void)
{
	enum
	{
		STEP_ENTER = 0,
		STEP_INPUTPWD,
		STEP_CHECK_OFFLINE,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_ENTER;
	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	SAV_PARAM st_sav_param_temp;

	memset(&st_sav_param_temp, 0, sizeof(SAV_PARAM));

	memset(&szSalesman_PWD, 0 ,sizeof(szSalesman_PWD));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_ENTER:
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
				ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_CHECK_OFFLINE;
			}
			break;
		case STEP_CHECK_OFFLINE:
			if(gl_SavParam.uiCurOfflineNo > 0)
			{ // Modified by Kim_LinHB 2013-7-12 v4.60 force to upload 
				DispString("Please Upload", 2 |DISP_LINE_CENTER);
				DispString("Offline Order", 4 |DISP_LINE_CENTER);
				PubWaitKey(TIME_DELAY);
				return;
			}
			ucStep = STEP_UPLOAD;
			break;
		case STEP_UPLOAD:
			DispConnection();
			DelayMs(1000);

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			ScrCls();
			DispString(gl_pLangSelPiont[STR_TABLE_PRODUCTS], DISPLAYLINE |DISP_LINE_CENTER);
			{
				// Modified by Kim_LinHB 2013-9-12 v4.67
				unsigned int RequestIndex = gl_SavParam.uiProductsListIndex;
				unsigned char bContinue = FALSE;
				int fd;
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

				do
				{
					iRetryCnt = TIME_RETRY;     //1
					do{
						ucRet = CategoryPackageList_XML(&bContinue, RequestIndex, &stRespH, &stRespB, FALSE, &st_sav_param_temp);
						--iRetryCnt;
					}while(1 == ucRet && iRetryCnt > 0);
					if(OK == ucRet)
						++RequestIndex;
				}while (OK == ucRet && bContinue);

				//PubDebugTx("ucRet 1= %d\n", ucRet);
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
			CloseConn();

			if(OK == ucRet && 0 == gl_SavParam.uiProductsListIndex)
			{
				iRetryCnt = TIME_RETRY;
				//PubDebugTx("aaaa\n");
				UpdateProductLsit(&gl_SavParam, st_sav_param_temp); // Added by Kim_LinHB 2013-7-6 v4.59

			//	PubDebugTx("cccc\n");
				DispString(gl_pLangSelPiont[STR_ACTUALISE_STOCK], DISPLAYLINE |DISP_LINE_CENTER);

				do    //2
				{
					ucRet = PhysicalStock_XML("", &stRespH, &stRespB, TRUE);
					--iRetryCnt;
				} while (1 == ucRet && iRetryCnt > 0);
				//PubDebugTx("ttttt\n");
			}
			//PubDebugTx("ucRet 2= %d\n", ucRet);
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			//print 
			// Modified by Kim_LinHB 2013-8-23 v4.61
			if(OK == ucRet && 0 == gl_SavParam.uiProductsListIndex)
			{
				Prn_OnlineStockReport();
			}
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
static void Do_LastConsignOrderRep(void)
{
	enum
	{
		STEP_ENTER = 0,
		STEP_INPUTPWD,
		STEP_CHECK_OFFLINE,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_ENTER;
	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	SAV_PARAM stRecord;
	memset(&stRecord, 0, sizeof(SAV_PARAM));
	memset(&szSalesman_PWD, 0 ,sizeof(szSalesman_PWD));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_ENTER:
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
				ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{	
				if(0 != strcmp(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			do 
			{
				ucRet = LastConsignOrder_XML(szSalesman_PWD, &stRecord, &stRespH, &stRespB, TRUE);// LstConsignOrderRep()
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			//print 
			if(OK == ucRet)
				Prn_LstConsignOrderRep(stRecord);
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
static void Do_SummaryRep(void)
{
	enum
	{
		STEP_ENTER = 0,
		STEP_FROM,
		STEP_TO,
		STEP_CHECK,
		STEP_INPUTPWD,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_ENTER;
	unsigned char from[SIZE_DATE_TIME + 1 + 2];
	unsigned char to[SIZE_DATE_TIME + 1 + 2];
	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	char loop = TIME_RETRY;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	xSummary stSummary;
	memset(&szSalesman_PWD, 0 ,sizeof(szSalesman_PWD));
	memset(from, 0, sizeof(from));
	memset(to, 0,  sizeof(to));

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		int iRetryCnt = TIME_RETRY;
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_ENTER:
			DispString(gl_pLangSelPiont[STR_PRESS_ENTER], DISP_SCR_CENTER);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			if(0 == ucKey)
				ucStep = STEP_FROM;
			break;
		case STEP_FROM:
			DispString("From", 1 | DISP_LINE_CENTER);
			GetCurTime(from + 2);
			memcpy(from, PRE_YEAR, 2);
			memcpy(from + 8, "0000", 4); // hhmm

			ucKey = GetInputTime(DISPLAYLINE, from + 2, TIME_GET_DATE | TIME_GET_TIME, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				ucStep = STEP_TO;
			break;
		case STEP_TO:
			DispString("To", 1 | DISP_LINE_CENTER);
			GetCurTime(to + 2);
			memcpy(to, PRE_YEAR, 2);
			memcpy(to + 8, "2359", 4); // hhmm

			ucKey = GetInputTime(DISPLAYLINE, to + 2, TIME_GET_DATE | TIME_GET_TIME, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(strcmp(to, from) >= 0)
					ucStep = STEP_CHECK;
				else
				{
					ScrCls();
					DispString("From > to", DISP_SCR_CENTER);
					PubWaitKey(TIME_DELAY);
					ucStep = STEP_FROM;
				}
			}
			break;
		case STEP_CHECK:
			DispString("Get report?", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubYesNo(TIME_OUT_STANDARD_30S);
			ucStep = STEP_INPUTPWD;
			break;
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:

			strcat(from, "00");
			strcat(to, "59");
			DispConnection();

			do 
			{
				ucRet = Summary_XML(szSalesman_PWD, from + 2, to + 2, &stSummary, &stRespH, &stRespB, TRUE);
				--iRetryCnt;
			} while (1 == ucRet && iRetryCnt > 0);
			
			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			//print 
			if(OK == ucRet)
				Prn_SummaryReport(from, to, stSummary);
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

static void Do_Report(int sub)
{
	switch(sub)
	{
	case ID_Offline_Batch_Upload_Rep:
		Do_OfflineBatchUploadRep();
		break;
	case ID_Offline_Physical_Stock_Rep:
		Do_OfflinePhysicalStockRep();
		break;
	case ID_Order_Rep:
		Do_OrderRep();
		break;
	case ID_Order_Rep_Prod:
		Do_OrderRep_Prod();
		break;
	case ID_Online_Stocks_Rep:
		Do_OnlineStocksRep();
		break;
	case ID_Last_Consign_Order_Rep:
		Do_LastConsignOrderRep();
		break;
	case ID_Summary_Rep:
		Do_SummaryRep();
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
static void Do_SettDownload()
{
	unsigned char ucRet = OK;
	unsigned char ucStep = 0;
	
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;
	SAV_PARAM st_sav_param_temp;

	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	memset(&st_sav_param_temp, 0, sizeof(SAV_PARAM));

	ScrCls();
	kbflush();

	DispConnection();

	ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
	ScrCls();
	DispString(gl_pLangSelPiont[STR_TELEPARAMETRAGE], 1 | DISP_LINE_LEFT);

	//下载 Parameter
	if(OK == ucRet)
	{
		int iRetryCnt = TIME_RETRY;
		++ucStep;
		DispString(gl_pLangSelPiont[STR_PARAM_DOWNLOAD], DISPLAYLINE | DISP_LINE_CENTER);
		
		do{
			ucRet = Parameter_XML(&stRespH, &stRespB, FALSE);
			--iRetryCnt;
		}while(1 == ucRet && iRetryCnt > 0);
	}

	//  下载  产品目录
	if(OK == ucRet)
	{
		// Modified by Kim_LinHB 2013-4-2 v4.56 isd 1.3.6 to loop mode
		// Modified by Kim_LinHB 2013-9-12 v4.67
		int iRetryCnt = TIME_RETRY;
		unsigned int RequestIndex = gl_SavParam.uiProductsListIndex;
		unsigned char bContinue = FALSE;
		int fd;
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
		if(gl_SavParam.uiCurOfflineNo > 0)
		{
			DispString("Upload Offline", 1 |DISP_LINE_LEFT);
			if(Do_OfflineBatchUpload(TRUE) != OK)
				return;
			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			ScrCls();
		}

		DispString(gl_pLangSelPiont[STR_TABLE_PRODUCTS], DISPLAYLINE |DISP_LINE_CENTER);

		//PubDebugTx("Cnum2 = %d\n", gl_SavParam.ucCategoryNum);
		//PubDebugTx("Pnum2 = %d\n", gl_SavParam.ucProductsNum);
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

		//PubDebugTx("Cnum3 = %d\n", gl_SavParam.ucCategoryNum);
		//PubDebugTx("Pnum3 = %d\n", gl_SavParam.ucProductsNum);

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
		// Modify End
	}

	//  下载  库存
	if(OK == ucRet)
	{
		int iRetryCnt = TIME_RETRY;
		++ucStep;
		//PubDebugTx("CnumTmp = %d\n", st_sav_param_temp.ucCategoryNum);
		//PubDebugTx("PnumTmp = %d\n", st_sav_param_temp.ucProductsNum);
		UpdateProductLsit(&gl_SavParam, st_sav_param_temp); // Added by Kim_LinHB 2013-7-6 v4.59

		DispString(gl_pLangSelPiont[STR_TELEPARAMETRAGE], 1 | DISP_LINE_LEFT);
		if(0 == gl_SavParam.uiCurOfflineNo) // Added by Kim_LinHB 2013-7-12 v4.60
		{
			DispString(gl_pLangSelPiont[STR_ACTUALISE_STOCK], DISPLAYLINE |DISP_LINE_CENTER);
			do{
				ucRet = PhysicalStock_XML("", &stRespH, &stRespB, FALSE);
				--iRetryCnt;
			}while(1 == ucRet && iRetryCnt > 0);
		}
	}
	//PubDebugTx("Cnum4 = %d\n", gl_SavParam.ucCategoryNum);
	//PubDebugTx("Pnum4 = %d\n", gl_SavParam.ucProductsNum);
	//  下载  SalesmanMerchantList
	if(OK == ucRet)
	{
		int iRetryCnt;
		unsigned char szDateTime[SIZE_DATE_TIME + 1];
		unsigned int RequestIndex = gl_SavParam.uiMerchantListIndex;
		unsigned char bContinue = FALSE;

		if(0 == RequestIndex)
		{
			remove(FILE_MERCHANT);
			gl_SavParam.stRoute.uiMerchantListNum = 0;
		}
		GetCurTime(szDateTime);
		DispString("Sale Mer List", DISPLAYLINE |DISP_LINE_CENTER);

		++ucStep;
		do
		{
			iRetryCnt = TIME_RETRY;
			do{
				ucRet = SalesmanMerchantList_XML("", szDateTime, &gl_SavParam.stRoute, 
					&bContinue, RequestIndex, &stRespH, &stRespB, FALSE);
				--iRetryCnt;
			}while(1 == ucRet && iRetryCnt > 0);
			if(OK == ucRet)
				++RequestIndex;
		}while (OK == ucRet && bContinue);

		if(OK ==ucRet && !bContinue)
			gl_SavParam.uiMerchantListIndex = 0;
		else
			gl_SavParam.uiMerchantListIndex = RequestIndex;
	}

	if(OK == ucRet)
	{
		int iRetryCnt = TIME_RETRY;
		++ucStep;
		DispString("User", DISPLAYLINE |DISP_LINE_CENTER);
		do{
			ucRet = Get_User_XML(&stRespH, &stRespB, FALSE);
			--iRetryCnt;
		}while(1 == ucRet && iRetryCnt > 0);
	}

	if (OK == ucRet)
	{
		++ucStep;
		gl_bSettDown = 1;
	}

	CloseConn();

	ScrCls();
	if(OK == ucRet)
	{
		if(1 == gl_SavParam.bIsSetup)
		{
			gl_SavParam.bIsSetup = 0;
		}

		DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_TELEPARAMETRAGE], 4 | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_OK);
	}
	else
	{
		DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_ABANDON], 4 | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_ERR);
	}

	//if(gl_bPlanned_SettDownload)
	//GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time); // Added by Kim_LinHB 2013-3-20 v4.56
	WriteParam(WRT_ALL);
	Prn_SettDownload(ucRet, stRespH);
	// Added by Kim_LinHB 2013-9-11 v4.67
	if(OK == ucRet)
	{
		if(gl_MaintenaceParam.stFTP.bForceToDownload)
			Do_SoftDownload(TRUE);
	}
	// Add End
}

static void Do_SoftDownload(unsigned char bAuto)
{
	int iRet;
// Modified by Kim_LinHB 2013-9-11 v4.67
	ScrCls();
	if(strcmp(gl_MaintenaceParam.stFTP.stApp.szFileVer, STR_VERSION_SUB) > 0)	
	{
		gl_MaintenaceParam.stFTP.bForceToDownload = 0;
		WriteParam(WRT_MAINTA);
		if(gl_SavParam.uiCurOfflineNo > 0)
		{
			ScrCls();
			DispString("running", 1 | DISP_LINE_CENTER);
			DispString("Batch upload", DISPLAYLINE | DISP_LINE_CENTER);
			if(Do_OfflineBatchUpload(TRUE) != OK)
			{
				DispString("Failed", DISPLAYLINE + 2 | DISP_LINE_CENTER);
				PubWaitKey(TIME_DELAY);
				return;
			}
		}
		DispString("New Ver Available", 1 | DISP_LINE_CENTER | DISP_LINE_REVER);
		DispString("Update", 3 | DISP_LINE_LEFT);
		DelayMs(2000);
	}
	else
	{
		if(!bAuto)
		{
			DispString("Update", 0 | DISP_LINE_CENTER | DISP_LINE_REVER);
			ScrPrint(10, 3, ASCII, "Your ver is already \nthe latest ver.");
			PubWaitKey(3);
		}
		else
		{
			GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
			memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
			WriteParam(WRT_MAINTA);
		}
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

 	DispConnection();
	
	iRet = FTPConnect(gl_MaintenaceParam_Ext.cConnectionTimeout);
	if (iRet != OK)
	{
		ScrCls();
		DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
		vTrace("!! FTPConnect KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
		FTPCloseConn(TRUE);
		PubWaitKey(TIME_DELAY);
		return;
	}

	iRet = FTPLogin(gl_MaintenaceParam.stFTP);
	if (iRet != OK)
	{
		ScrCls();
		DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
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
			DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
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
			DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
			vTrace("!! FTPGetFile Param KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);

			FTPCloseConn(TRUE);
			PubWaitKey(TIME_DELAY);
			return;
		}
	}

	vTrace("-> DownloadSoftware Done OK", NULL, 0, TRACE_WARNING, TYPE_N);

	FTPCloseConn(FALSE);
	if(bAuto)
	{
		GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
		memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
	}

	// Added by Kim_LinHB 2013-9-3 v4.65 for setting download after remote download
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
#ifdef LHB_TEST
		ScrPrint(0,0, CFONT, "%d", iRet);
		getkey();
#endif
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

static void Do_Transmission(int sub)
{
	switch(sub)
	{
	case ID_SettingsDownload:
	#ifdef LHB_TEST
		MyTest();   //hide
	#endif
		Do_SettDownload();
		break;
	case ID_SoftDownload:
		Do_SoftDownload(gl_bPlanned_SoftDownload);
		break;
		//case ID_Config_Auto:
		//	break;
		//case ID_First_Init__Auto:
		//	break;
	}
}


static void Do_ConsignOrder(void)
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_UPDATA,
		STEP_CHANGE,
		STEP_CHECKORDER,
		STEP_INPUTPWD2,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucSel_Chg = 0;
	unsigned char ucStep = STEP_INPUTPWD;
	unsigned char bFirst = TRUE;
	unsigned char bFirstChg = TRUE;
	char loop = TIME_RETRY;
	SAV_PARAM stTmpS;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	memset(&stTmpS, 0, sizeof(SAV_PARAM));
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{	
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPDATA;
			}
			break;
		case STEP_UPDATA:
			stCurTran.ucTransType = ID_ConsignOrder;

			DispString("GET CONSIGNMENT", 1 | DISP_LINE_CENTER);
			DispString("INFORMATIONS", DISPLAYLINE | DISP_LINE_CENTER);

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt;
				unsigned int RequestIndex = 0;
				unsigned char bContinue = FALSE;

				do
				{
					iRetryCnt = TIME_RETRY;
					do{
						ucRet = ConsignInquiry_XML(stCurTran.szSalesman_PWD, &bContinue, RequestIndex, 
							&stRespH, &stRespB, FALSE, &stTmpS);
						--iRetryCnt;
					}while(1 == ucRet && iRetryCnt > 0);
					if(OK == ucRet)
						++RequestIndex;
				}while (OK == ucRet && bContinue);

				if(ucRet != OK || bContinue)
				{
					CloseConn();
					Do_Error(ucRet, stRespH);
					return;
				}
			}
			else
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}
			CloseConn();

			{
				int kk, cnt = 0;
				for(kk = 0; kk < stTmpS.uiProductsNum; ++kk)
				{
					if(atoi(stTmpS.stSavParamProducts[kk].szOrderNum) > 0)
					{
						stCurTran.stProducts[cnt].uiCodeCategory = stTmpS.stSavParamProducts[kk].uiCodeCategory;
						strcpy(stCurTran.stProducts[cnt].szCodeProduct, stTmpS.stSavParamProducts[kk].szCodeProduct);
						strcpy(stCurTran.stProducts[cnt].szQuantity, stTmpS.stSavParamProducts[kk].szOrderNum);
						++cnt;
					}
				}
				stCurTran.uiProductNum = cnt;
			}
			ucStep = STEP_CHANGE;
			break;
		case STEP_CHANGE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(bFirst)
			{
				DispString("Print", 0 | DISP_LINE_CENTER | (0 == ucSel_Chg ? DISP_LINE_REVER : 0));
				DispString("change order", 2 | DISP_LINE_CENTER | (1 == ucSel_Chg ? DISP_LINE_REVER : 0));
			}
			else
			{
				DispString("change order", 0 | DISP_LINE_CENTER | (0 == ucSel_Chg ? DISP_LINE_REVER : 0));
				DispString("send order", 2 | DISP_LINE_CENTER | (1 == ucSel_Chg ? DISP_LINE_REVER : 0));
			}

			ucKey = YesNo(TIME_OUT_STANDARD_30S);

			if(KEYUP == ucKey)
				ucSel_Chg = 0 == ucSel_Chg ? 1 : 0;
			else if(KEYDOWN == ucKey)
				ucSel_Chg = 1 == ucSel_Chg ? 0 : 1;
			if(KEYENTER == ucKey)
			{
				if(0 == ucSel_Chg && bFirst)
				{
					Prn_Consign_info(stTmpS);//print
					bFirst = FALSE;
				}
				else if((0 == ucSel_Chg && !bFirst) ||
					(1 == ucSel_Chg && bFirst))
				{
					int uiCategory_Code = -1;
					if(0 == stTmpS.ucCategoryNum)
					{
						ScrCls();
						DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
						Beep_Speaker(BEEP_ERR);
						PubWaitKey(TIME_OUT_STANDARD_30S);
						return;
					}

					if(bFirstChg)
					{
						stCurTran.uiProductNum = 0;
						memset(stCurTran.stProducts, 0, sizeof(stCurTran.stProducts));
					}

					bFirstChg = FALSE;
					bFirst = FALSE;
					while(1){
						uiCategory_Code = SelCateory(stTmpS, PRODUCT_PHYSICAL);
						if(-1 == uiCategory_Code)
							break;
						else if(-2 == uiCategory_Code)
						{
							//ucKey = 0xFF;
							break;
						}
						else
						{
							stCurTran.ucTransType = ID_ConsignOrder;
							SetProductsQuantity(&stTmpS, &stCurTran, uiCategory_Code, FALSE);
						}
					}
				}
				else if (1 == ucSel_Chg && !bFirst)
				{
					CalcAmt(&stCurTran);
					ucStep = STEP_CHECKORDER;
				}
			}
			break;
		case STEP_CHECKORDER:
			DispString(gl_pLangSelPiont[STR_VALIDE_COMMANDE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);

			ucStep = STEP_INPUTPWD2;

			if(KEYENTER == ucKey)
				Prn_ConsignOrder(stCurTran, TRUE);//print

			break;
		case STEP_INPUTPWD2:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{	
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			stCurTran.ucTransType = ID_ConsignOrder;
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;

			DispConnection();

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Get_ProvisionId_XML(stCurTran.szSalesman_PWD, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);
			}

			if(OK == ucRet)
			{
				int cnt = 3;
				while(cnt-- >0)
				{
					GetCurTime(stCurTran.szDateTime);
					ucRet = SalesmanConsignOrder_XML(stCurTran, &stRespH, &stRespB, FALSE);
					if(ucRet != 1)
						break;
				}
				if(cnt <= 0 && 1 == ucRet)
					Cancellation_XML(stCurTran, &stRespH, &stRespB, FALSE);//cancel;
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
				memcpy(&gl_LastTran, &stCurTran, sizeof(TRANS_DATA));
				//SaveProducts(stCurTran);
				WriteParam(WRT_TXN);
				Prn_ConsignOrder(stCurTran, FALSE);//print
			}
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

static void Do_ReturnConsignOrder(void)
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_UPDATA,
		STEP_CHANGE,
		STEP_CHECKORDER,
		STEP_INPUTPWD2,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucSel_Chg = 0;
	unsigned char ucStep = STEP_INPUTPWD;
	char loop = TIME_RETRY;
	unsigned char bFirst = TRUE;
	unsigned char bFirstChg = TRUE;
	SAV_PARAM stTmpS;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	TRANS_DATA stCurTran;
	memset(&stTmpS, 0, sizeof(SAV_PARAM));
	memset(&stCurTran, 0 ,sizeof(TRANS_DATA));
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_INPUTPWD:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPDATA;
			}
			break;
		case STEP_UPDATA:
			stCurTran.ucTransType = ID_ReturnConsignOrder;

			DispString("GET CONSIGNMENT", 1 | DISP_LINE_CENTER);
			DispString("INFORMATIONS", DISPLAYLINE | DISP_LINE_CENTER);

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt;
				unsigned int RequestIndex = 0;
				unsigned char bContinue = FALSE;

				do
				{
					iRetryCnt = TIME_RETRY;
					do{
						ucRet = ConsignInquiry_XML(stCurTran.szSalesman_PWD, &bContinue, RequestIndex, 
							&stRespH, &stRespB, FALSE, &stTmpS);
						--iRetryCnt;
					}while(1 == ucRet && iRetryCnt > 0);
					if(OK == ucRet)
						++RequestIndex;
				}while (OK == ucRet && bContinue);

				if(ucRet != OK || bContinue)
				{
					CloseConn();
					Do_Error(ucRet, stRespH);
					return;
				}
			}
			else
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}
			CloseConn();

			ucStep = STEP_CHANGE;
			break;
		case STEP_CHANGE:
			if(bFirst)
			{
				DispString("Print", 0 | DISP_LINE_CENTER | (0 == ucSel_Chg ? DISP_LINE_REVER : 0));
				DispString("change order", 2 | DISP_LINE_CENTER | (1 == ucSel_Chg ? DISP_LINE_REVER : 0));
			}
			else
			{
				DispString("change order", 0 | DISP_LINE_CENTER | (0 == ucSel_Chg ? DISP_LINE_REVER : 0));
				DispString("send order", 2 | DISP_LINE_CENTER | (1 == ucSel_Chg ? DISP_LINE_REVER : 0));
			}

			ucKey = YesNo(TIME_OUT_STANDARD_30S);

			if(KEYUP == ucKey)
				ucSel_Chg = 0 == ucSel_Chg ? 1 : 0;
			else if(KEYDOWN == ucKey)
				ucSel_Chg = 2 == ucSel_Chg ? 0 : 1;
			if(KEYENTER == ucKey)
			{
				if(0 == ucSel_Chg && bFirst)
				{
					Prn_Consign_info(stTmpS);//print
					bFirst = FALSE;
				}
				else if((0 == ucSel_Chg && !bFirst) ||
					(1 == ucSel_Chg && bFirst))
				{
					int ucCategory_Code = -1;
					if(0 == stTmpS.ucCategoryNum)
					{
						ScrCls();
						DispString("Empty Category", DISPLAYLINE | DISP_LINE_CENTER);
						Beep_Speaker(BEEP_ERR);
						PubWaitKey(TIME_OUT_STANDARD_30S);
						return;
					}

					if(bFirstChg)
					{
						stCurTran.uiProductNum = 0;
						memset(stCurTran.stProducts, 0, sizeof(stCurTran.stProducts));
					}

					bFirstChg = FALSE;
					bFirst = FALSE;
					while(1){
						ucCategory_Code = SelCateory(stTmpS, PRODUCT_PHYSICAL);
						if(-1 == ucCategory_Code)
							break;
						else if(-2 == ucCategory_Code)
						{
							//ucKey = 0xFF;
							break;
						}
						else
						{
							stCurTran.ucTransType = ID_ReturnConsignOrder;
							SetProductsQuantity(&stTmpS, &stCurTran, ucCategory_Code, FALSE);
						}
					}
				}
				else if(1 == ucSel_Chg && !bFirst)
				{
					CalcAmt(&stCurTran);
					ucStep = STEP_CHECKORDER;
				}
			}
			break;
		case STEP_CHECKORDER:
			DispString(gl_pLangSelPiont[STR_VALIDE_COMMANDE], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);

			ucStep = STEP_INPUTPWD2;

			if(KEYENTER == ucKey)
				Prn_RetConsignOrder(stCurTran, TRUE);//print

			break;
		case STEP_INPUTPWD2:
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, stCurTran.szSalesman_PWD, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(stCurTran.szSalesman_PWD, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPLOAD;
			}
			break;
		case STEP_UPLOAD:
			stCurTran.ucTransType = ID_ReturnConsignOrder;
			stCurTran.usBatchNo = gl_SavParam.usBatchNo;
			stCurTran.usTraceNo = gl_SavParam.usTraceNo;

			DispConnection();

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Get_ProvisionId_XML(stCurTran.szSalesman_PWD, &stRespH, &stRespB, stCurTran.szProvisionID, FALSE);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);
			}

			if(OK == ucRet)
			{
				int cnt = 3;
				while(cnt-- >0)
				{
					GetCurTime(stCurTran.szDateTime);
					ucRet = ReturnConsignOrder_XML(stCurTran, &stRespH, &stRespB, FALSE);
					if(ucRet != 1)
						break;
				}
				if(cnt <= 0 && 1 == ucRet)
					Cancellation_XML(stCurTran, &stRespH, &stRespB, FALSE);//cancel;
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
				memcpy(&gl_LastTran, &stCurTran, sizeof(TRANS_DATA));
				//SaveProducts(stCurTran);
				WriteParam(WRT_TXN);
				Prn_RetConsignOrder(stCurTran, FALSE);//print 
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			return;
			break;
		}
		if(0xFF == ucKey)
			return;
	}
}
// Added by Kim_LinHB 2013-10-28 v4.68
static void Do_ConsignmentConfirm(void)
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_UPDATA,
		STEP_SHOWLIST,
		STEP_FUNCCHOOSE,
		STEP_INPUTPWD2,
		STEP_CONFIRM,
		STEP_UPLOAD,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	int iOrderSel = -1;
	int iFuncSel = -1;
	unsigned char ucStep = STEP_INPUTPWD;
	char loop = TIME_RETRY;

	unsigned char szPwd[SIZE_MAX_PWD + 1];
	CONSIGNMENT_LIST	stOrderList;
	unsigned char bDisplay[NB_MAX_PRODUCTS]; // Added by Kim_LinHB 2013-11-4 v4.69
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	memset(&stOrderList, 0, sizeof(CONSIGNMENT_LIST));
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));
	memset(&bDisplay, 1, sizeof(bDisplay));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_INPUTPWD:
			memset(szPwd, 0, sizeof(szPwd));
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szPwd, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPDATA;
			}
			break;
		case STEP_UPDATA:

			DispString("GET CONSIGNMENT", 1 | DISP_LINE_CENTER);
			DispString("INFORMATIONS", DISPLAYLINE | DISP_LINE_CENTER);

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Inq_For_Consig_Recv_Confirm_XML(szPwd, 0, 4, 
						&stRespH, &stRespB, FALSE, &stOrderList);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);

				if(ucRet != OK)
				{
					CloseConn();
					Do_Error(ucRet, stRespH);
					return;
				}
			}
			else
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}
			CloseConn();

			ucStep = STEP_SHOWLIST;
			break;
		case STEP_SHOWLIST:

			iOrderSel = SelConsignOrderID(stOrderList, bDisplay);
			if(-1 == iOrderSel)
			{
				ucKey = 0xFF;
			}
			else
			{
				ucStep = STEP_FUNCCHOOSE;
			}
			break;
		case STEP_FUNCCHOOSE:
			iFuncSel = SelConsignOperation();
			
			if(0 == iFuncSel || 1 == iFuncSel)
			{
				ucStep = STEP_INPUTPWD2;
			}
			else if(2 == iFuncSel)
			{
				ucStep = STEP_PRINT;
			}
			else
			{
				ucStep = STEP_SHOWLIST;
			}
			break;
		case STEP_INPUTPWD2:
			memset(szPwd, 0, sizeof(szPwd));
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szPwd, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_CONFIRM;
			}
			break;
		case STEP_CONFIRM:
			if(1 == iFuncSel)
				DispString("Approve", 1 | DISP_LINE_CENTER);
			else
				DispString("Reject", 1 | DISP_LINE_CENTER);
			DispString("ConsignOrder?", 3 | DISP_LINE_CENTER);
			if(0 == PubYesNo(TIME_OUT_STANDARD_30S))
			{
				ucStep = STEP_UPLOAD;
			}
			else
			{
				iFuncSel = -1;
				ucStep = STEP_FUNCCHOOSE;
			}
			break;
		case STEP_UPLOAD:
			DispConnection();

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

			if(OK == ucRet)
			{
				int cnt = 3;
				while(cnt-- >0)
				{
					ucRet = Update_For_Consign_Recv_Confirm_XML(stOrderList, iOrderSel, iFuncSel, &stRespH, &stRespB, FALSE);
					if(ucRet != 1)
						break;
				}
			}
			else
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}

			CloseConn();
			AscInc(&gl_SavParam.usTraceNo);
			WriteParam(WRT_SAVE);

			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
			{
				Prn_Consignment(stOrderList, ID_ConsignConfirm, iOrderSel, iFuncSel);//print 
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			// Modified by Kim_LinHB 2013-11-4 v4.69
			if(2 == iFuncSel || ucRet != OK)
				ucStep = STEP_FUNCCHOOSE;
			else
			{
				bDisplay[iOrderSel] = 0;
				ucStep = STEP_SHOWLIST;
			}
			iFuncSel = -1;
			break;
		}
		if(0xFF == ucKey)
			return;
	}
}
// Added by Kim_LinHB 2013-10-28 v4.68
static void Do_PendinConsignOrder(void)
{
	enum
	{
		STEP_INPUTPWD = 0,
		STEP_UPDATA,
		STEP_PRINT,
	};
	unsigned char ucRet, ucKey = 0;
	unsigned char ucStep = STEP_INPUTPWD;
	char loop = TIME_RETRY;

	unsigned char szPwd[SIZE_MAX_PWD + 1];
	CONSIGNMENT_LIST	stOrderList;
	RESPONSE_HEADER_TAG stRespH;
	RESPONSE_BODY_TAG stRespB;

	memset(&stOrderList, 0, sizeof(CONSIGNMENT_LIST));
	memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
	memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

	while(1)
	{
		ScrCls();
		kbflush();
		switch(ucStep)
		{
		case STEP_INPUTPWD:
			memset(szPwd, 0, sizeof(szPwd));
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(0 != strcmp(szPwd, gl_MaintenaceParam.stUser.szPwd))
				{
					--loop;
					DispString(gl_pLangSelPiont[STR_PASSWORD_ERREUR], 1 | DISP_LINE_LEFT);
					DispString(gl_pLangSelPiont[STR_PASSWORD_DISPLAY], DISPLAYLINE | DISP_LINE_CENTER);
					Beep_Speaker(BEEP_ERR);
					PubWaitKey(TIME_DELAY);
					if(loop <= 0)return;
				}
				else
					ucStep = STEP_UPDATA;
			}
			break;
		case STEP_UPDATA:

			DispString("GET CONSIGNMENT", 1 | DISP_LINE_CENTER);
			DispString("INFORMATIONS", DISPLAYLINE | DISP_LINE_CENTER);

			ucRet = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == ucRet)
			{
				int iRetryCnt = TIME_RETRY;
				do{
					ucRet = Inq_For_Consig_Recv_Confirm_XML(szPwd, 1, 4, 
						&stRespH, &stRespB, FALSE, &stOrderList);
					--iRetryCnt;
				}while(1 == ucRet && iRetryCnt > 0);

				if(ucRet != OK)
				{
					CloseConn();
					Do_Error(ucRet, stRespH);
					return;
				}
			}
			else
			{
				CloseConn();
				Do_Error(ucRet, stRespH);
				return;
			}
			CloseConn();

			ucStep = STEP_PRINT;
			break;
		case STEP_PRINT:
			if(OK == ucRet)
			{
				Prn_Consignment(stOrderList, ID_PendingConsignOrder, -1, -1);//print 
			}
			else
			{
				Do_Error(ucRet, stRespH);
			}
			return;
			break;
		}
		if(0xFF == ucKey)
			return;
	}
}
// Added by Kim_LinHB 2013-10-28 v4.68
static void Do_Consignment(int sub)
{
	switch(sub)
	{
	case ID_ConsignOrder:
		// Added by Kim_LinHB 2013-9-11 v4.67
		if(gl_SavParam.uiCurOfflineNo > 0)
		{
			if(Do_OfflineBatchUpload(TRUE) != OK)
				return;
			ScrCls();
		}
		// Add End
		Do_ConsignOrder();
		break;
	case ID_ReturnConsignOrder:
		// Added by Kim_LinHB 2013-9-11 v4.67
		if(gl_SavParam.uiCurOfflineNo > 0)
		{
			if(Do_OfflineBatchUpload(TRUE) != OK)
				return;
			ScrCls();
		}
		// Add End
		Do_ReturnConsignOrder();
		break;
	case ID_ConsignConfirm:
		Do_ConsignmentConfirm();
		break;
	case ID_PendingConsignOrder:
		Do_PendinConsignOrder();
		break;
	}
}




static void Do_Error(unsigned char Ret, RESPONSE_HEADER_TAG stRespH)
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
			//DispString(gl_pLangSelPiont[STR_PAS_DE_TELEPARA], DISP_SCR_CENTER);
			Do_SettDownload();    //modified by Allen 2016-09-06 Alper: when we return '5' as status on response message , app should automatically  trigger Setting download function . 
		}
	}
	PubWaitKey(TIME_DELAY);
}

static int SelCateory(SAV_PARAM stSav, unsigned char type)
{
	unsigned char ucKey;
	unsigned char ucPageSel = 0, ucLineSel = 0;
	unsigned char ucPage = 0;
	unsigned char ucLastPage = 0;
	unsigned char ucIndex[NB_MAX_CATEGORYS];

	if(PRODUCT_ALL == type)  //e-vouchers and physical vouchers
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
			if(type == stSav.stSavParamCategory[i].ucCategoryType)
			{
				ucIndex[ucLastPage] = i;
				++ucLastPage;
			}
		}
		
	}
	// type ...
	
	if(0 == ucLastPage)
	{
		DispString("Empty Category", 1 | DISP_SCR_CENTER);
		PubYesNo(TIME_OUT_STANDARD_30S);
		return -1;  // mean no key 
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
				Beep_Speaker(BEEP_ERR);
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
				Beep_Speaker(BEEP_ERR);
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

// Modified by Kim_LinHB 2013-7-11 v4.60 add bCredit
static void SetProductsQuantity(SAV_PARAM *pstSav, TRANS_DATA *pstCurTran, unsigned int uiCategory_Code, unsigned char bCredit)
{
	unsigned char ucKey;
	unsigned int uiSel = 0;    // modified by Allen from unsigned char to unsigned int
	unsigned int uiIndex[NB_MAX_PRODUCTS];   // modified by Allen from unsigned char to unsigned int
	unsigned int MaxSel = 0;    // modified by Allen from unsigned char to unsigned int
	PRODUCT_INFO  stProducts[NB_MAX_PRODUCTS];
	for(uiSel = 0; uiSel < pstSav->uiProductsNum; ++uiSel)
	{
		if(uiCategory_Code == pstSav->stSavParamProducts[uiSel].uiCodeCategory)
		{	
			unsigned int i = 0;
			if(bCredit && !pstSav->stSavParamProducts[uiSel].bCredit)
				continue;
			uiIndex[MaxSel] = uiSel;
			strcpy(stProducts[MaxSel].szCodeProduct, pstSav->stSavParamProducts[uiSel].szCodeProduct);
			strcpy(stProducts[MaxSel].szQuantity, "0");
			for(i = 0; i < pstCurTran->uiProductNum; ++i)
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
		uiSel = 0;
		while(1)
		{
			int iTmpQty = 0;
			ScrCls();
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			if(0 == uiSel)
				ScrSetIcon(ICON_UP, CLOSEICON);
			if(MaxSel - 1 == uiSel)
				ScrSetIcon(ICON_DOWN, CLOSEICON);

			ScrPrint(0,1, ASCII, pstSav->stSavParamProducts[uiIndex[uiSel]].szNameProduct);
		
			iTmpQty = atoi(stProducts[uiSel].szQuantity);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 0, SIZE_MAX_QUANTITY, 
				stProducts[uiSel].szQuantity,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				int iProductQty = atoi(stProducts[uiSel].szQuantity);
				// Modified by Kim_LinHB 2013-9-11 v4.67 add ID_ReturnConsignOrder back
				if((ID_OfflineOrder == pstCurTran->ucTransType ||
					ID_ReturnConsignOrder == pstCurTran->ucTransType)&&
					iProductQty > 
						atoi(pstSav->stSavParamProducts[uiIndex[uiSel]].szQuantity))
				{
					ScrClrLine(DISPLAYLINE,DISPLAYLINE);
					DispString("Check stock", DISPLAYLINE | DISP_LINE_CENTER);
					sprintf(stProducts[uiSel].szQuantity, "%d", iTmpQty);
					while(1)
					{
						unsigned char tmp = PubWaitKey(TIME_OUT_STANDARD_30S);
						if(NOKEY == tmp)
						{
							ucKey =0xFF;
							break;
						}
						else if(KEYENTER == tmp)
							break;
					}
				}
				else
				{
					unsigned int i;
					unsigned char bNew = TRUE;
					for(i = 0; i < pstCurTran->uiProductNum; ++i)
					{
						if(0 == strcmp(stProducts[uiSel].szCodeProduct,pstCurTran->stProducts[i].szCodeProduct))
						{
							if(0 == iProductQty) //delete
							{
								memset(&pstCurTran->stProducts[i], 0, sizeof(PRODUCT_INFO));
								memmove(&pstCurTran->stProducts[i], &pstCurTran->stProducts[i + 1],
										(pstCurTran->uiProductNum -i - 1) *sizeof(PRODUCT_INFO));
								--pstCurTran->uiProductNum;
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
							pstCurTran->stProducts[pstCurTran->uiProductNum].uiCodeCategory = 
							pstSav->stSavParamProducts[uiIndex[uiSel]].uiCodeCategory;
							memcpy(pstCurTran->stProducts[pstCurTran->uiProductNum].szCodeProduct,
								pstSav->stSavParamProducts[uiIndex[uiSel]].szCodeProduct, 5);
							sprintf(pstCurTran->stProducts[pstCurTran->uiProductNum].szQuantity, "%d", iProductQty);
							++pstCurTran->uiProductNum;
						}
					}

					if(MaxSel - 1 == uiSel)
						Beep_Speaker(BEEP_ERR);
					else
						++uiSel;
				}
			}
			else if(KEYUP ==ucKey)
			{
				if (0 == uiSel)
					Beep_Speaker(BEEP_ERR);
				else
					--uiSel;
			}
			else if(KEYDOWN == ucKey)
			{
				if (MaxSel - 1 == uiSel)
					Beep_Speaker(BEEP_ERR);
				else
					++uiSel;
			}
			else if(0xFF == ucKey)
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

static void SaveProducts(TRANS_DATA stCurTran)
{
	unsigned short i;
	
	for(i = 0; i < stCurTran.uiProductNum; ++i)
	{
		int j;
		for(j = 0; j < gl_SavParam.uiProductsNum; ++j)
		{
			if(0 == strcmp(gl_SavParam.stSavParamProducts[j].szCodeProduct, stCurTran.stProducts[i].szCodeProduct))
			{
				if(ID_CancelLastOrder == stCurTran.ucTransType// ||
					//ID_OfflineCancelLastOrder == stCurTran.ucTransType
					)
				{
					sprintf(gl_SavParam.stSavParamProducts[j].szQuantity,
						"%d", atoi(gl_SavParam.stSavParamProducts[j].szQuantity)
						+ atoi(stCurTran.stProducts[i].szQuantity));
					if(ID_CancelLastOrder == stCurTran.ucTransType// ||
						//ID_OfflineCancelLastOrder == stCurTran.ucTransType
						)
					{
						sprintf(gl_SavParam.stSavParamProducts[j].szSoldQuantity,
							"%d", atoi(gl_SavParam.stSavParamProducts[j].szSoldQuantity)
							- atoi(stCurTran.stProducts[i].szQuantity));
					}
				}
				else if(ID_CashOrder == stCurTran.ucTransType ||
					ID_OfflineOrder == stCurTran.ucTransType ||
					ID_CreditOrder == stCurTran.ucTransType)
				{
					if(atoi(gl_SavParam.stSavParamProducts[j].szQuantity) >= atoi(stCurTran.stProducts[i].szQuantity))
						sprintf(gl_SavParam.stSavParamProducts[j].szQuantity,
							"%d", atoi(gl_SavParam.stSavParamProducts[j].szQuantity)
							- atoi(stCurTran.stProducts[i].szQuantity));
					else
						sprintf(gl_SavParam.stSavParamProducts[j].szQuantity, "%d", 0);
					if(strlen(gl_SavParam.stSavParamProducts[j].szSoldQuantity) == 0)
						strcpy(gl_SavParam.stSavParamProducts[j].szSoldQuantity, "0");
					sprintf(gl_SavParam.stSavParamProducts[j].szSoldQuantity,
						"%d", atoi(gl_SavParam.stSavParamProducts[j].szSoldQuantity)
						+ atoi(stCurTran.stProducts[i].szQuantity));
				}
				break;
			}
		}
	}
}

static void CalcAmt(TRANS_DATA *pstCurTran)
{
	unsigned int i = 0;
	unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
	unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

	sprintf(pstCurTran->szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
	sprintf(pstCurTran->szAmountWithoutDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

	for(i = 0; i < pstCurTran->uiProductNum; ++i)
	{
		unsigned int j;
		for(j = 0; j < gl_SavParam.uiProductsNum; ++j)
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
		sprintf(szAmountWithDiscount, "%.*s", strlen(szAmountWithDiscount) - 2, szAmountWithDiscount);
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

// Added by Kim_LinHB 2013-7-6 v4.62
// Modified by Kim_LinHB 2013-7-31 v4.62
// Modified by Kim_LinHB 2013-9-12 v4.67
static void UpdateProductLsit(SAV_PARAM *dst, const SAV_PARAM src)
{
	int i, j;
	SAV_PARAM tmp;   // modified by Allen 20151211 

	memcpy(&tmp, &src, sizeof(SAV_PARAM));

	for(i = 0; i < tmp.uiProductsNum; ++i)
	{
		for(j = 0; j < dst->uiProductsNum; ++j)
		{
			if(0 == strcmp(dst->stSavParamProducts[j].szCodeProduct,
				tmp.stSavParamProducts[i].szCodeProduct))
			{
				memcpy(tmp.stSavParamProducts[i].szQuantity, dst->stSavParamProducts[j].szQuantity, SIZE_MAX_QUANTITY + 1);
				memcpy(tmp.stSavParamProducts[i].szAverage, dst->stSavParamProducts[j].szAverage, SIZE_MAX_QUANTITY + 1);

				memcpy(tmp.stSavParamProducts[i].szOrderNum, dst->stSavParamProducts[j].szOrderNum, SIZE_MAX_QUANTITY + 1);
				memcpy(tmp.stSavParamProducts[i].szSoldQuantity, dst->stSavParamProducts[j].szSoldQuantity, SIZE_MAX_QUANTITY + 1);
				break;
			}
		}
	}

	// Modified by Kim_LinHB 2013-11-19 v4.72 add &
	memcpy(dst->stSavParamCategory, tmp.stSavParamCategory, NB_MAX_CATEGORYS * sizeof(xParamCategory));
	memcpy(dst->stSavParamProducts, tmp.stSavParamProducts, NB_MAX_PRODUCTS * sizeof(xParamProducts));
	dst->ucCategoryNum = tmp.ucCategoryNum;
	dst->uiProductsNum = tmp.uiProductsNum;
	// Modify End
	// Add End
}

// Added by Kim_LinHB 2013-7-22 v4.60
static unsigned char SelTerminal(ROUTE_INFO route, const unsigned char *MID, \
								 xMerchantList *stMerchant)
{
	unsigned char ucKey;
	unsigned char ucPageSel = 0, ucLineSel = 0;
	unsigned char ucPage = 0;
	unsigned int  uiLastPage = 0;
	xMerchantList tmp[1024];

	unsigned int i;
	for(i = 0; i < route.uiMerchantListNum; ++i)
	{
		GetMerchant(FILE_MERCHANT, i, &tmp[uiLastPage]);
		if(0 == strcmp(MID, tmp[uiLastPage].szMerchantID))
			++uiLastPage;
		else
			memset(&tmp[uiLastPage], 0, sizeof(xMerchantList));
	}


	if(0 == uiLastPage)
		return FALSE;  // mean no key 

	ucPage = uiLastPage / 4 + 1;
	uiLastPage %= 4;

	while(1)
	{
		unsigned int itemNo;
		unsigned int i;
		unsigned int uiNumSel  = ucPageSel * 4 + ucLineSel;
		ScrCls();
		ScrSetIcon(ICON_UP, OPENICON);
		ScrSetIcon(ICON_DOWN, OPENICON);
		if(0 == uiNumSel)
			ScrSetIcon(ICON_UP, CLOSEICON);
		if((ucPage - 1) * 4 + uiLastPage - 1 == uiNumSel)
			ScrSetIcon(ICON_DOWN, CLOSEICON);

		if(ucPageSel < ucPage - 1)
			itemNo = 4;
		else
			itemNo = uiLastPage;
		for(i = 0; i < itemNo; ++i)
			DispString(tmp[ucPageSel * 4 + i].szTermCode, i * 2 | DISP_LINE_LEFT | (i == ucLineSel ? DISP_LINE_REVER : 0));


		ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
		if(KEYENTER == ucKey)
		{
			memcpy(stMerchant, &tmp[ucPageSel * 4 + ucLineSel], sizeof(xMerchantList));
			return TRUE;
		}
		else if(KEYUP == ucKey)
		{
			if(0 == uiNumSel)
				Beep_Speaker(BEEP_ERR);
			else
			{
				--uiNumSel;
				ucPageSel = uiNumSel / 4;
				ucLineSel = uiNumSel % 4;
			}
		}
		else if(KEYDOWN == ucKey)
		{
			if((ucPage - 1) * 4 + uiLastPage - 1 == uiNumSel)
				Beep_Speaker(BEEP_ERR);
			else
			{
				++uiNumSel;
				ucPageSel = uiNumSel / 4;
				ucLineSel = uiNumSel % 4;
			}
		}
		else if(KEYCANCEL == ucKey)
			return FALSE;
		else if(NOKEY == ucKey)
			return FALSE;
	}
	return FALSE;  // mean no key 
}
// Add End

// Added by Kim_LinHB 2013-10-28 v4.68
// Modified by Kim_LinHB 2013-11-4 v4.69
static int SelConsignOrderID(CONSIGNMENT_LIST stOrderList, unsigned char *bDisplay)
{
	unsigned char ucKey;
	unsigned char ucPageSel = 0, ucLineSel = 0;
	unsigned char ucPage = 0;
	unsigned char ucLastPage = 0;
	unsigned char ucIndex[NB_MAX_PRODUCTS];
	unsigned char szDispBuffer[100];

	unsigned int i;
	for(i = 0; i < stOrderList.num; ++i)
	{
		if(1 == bDisplay[i])
		{
			ucIndex[ucLastPage] = i;
			++ucLastPage;
		}
	}
		
	if(0 == ucLastPage)
	{
		DispString("NO CONSIGN ORDER", 1 | DISP_SCR_CENTER);
		PubYesNo(TIME_OUT_STANDARD_30S);
		return -1;  // mean no key 
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
		{
			sprintf(szDispBuffer, "%s %s", stOrderList.ConsigOrderList[ucIndex[ucPageSel * 4 + i]].Id, stOrderList.ConsigOrderList[ucIndex[ucPageSel * 4 + i]].CreateDate);
			DispString(szDispBuffer, i * 2 | DISP_LINE_LEFT | (i == ucLineSel ? DISP_LINE_REVER : 0));
		}

		ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
		if(KEYENTER == ucKey)
			return ucIndex[ucPageSel * 4 + ucLineSel];
		else if(KEYUP == ucKey)
		{
			if(0 == ucNumSel)
				Beep_Speaker(BEEP_ERR);
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
				Beep_Speaker(BEEP_ERR);
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
			return -1;
	}
	return -1;  // mean no key
}

static int SelConsignOperation()
{
	struct _tagPrompt{
		unsigned char szPrompt[16];
		int value;
	};
	unsigned char ucSel = 0, ucKey;
	int i;

	const int size = 4;

	struct _tagPrompt stPrompt[] = {
		{"Confirm", 1},
		{"Reject", 0},
		{"Print", 2},
		{"Back", -1},
	};

	while(1)
	{
		ScrCls();
		ScrCls();
		ScrSetIcon(ICON_UP, OPENICON);
		ScrSetIcon(ICON_DOWN, OPENICON);
		
		for(i = 0; i < size; ++i)
			DispString(stPrompt[i].szPrompt, i * 2 | DISP_LINE_LEFT | (i == ucSel ? DISP_LINE_REVER : 0));

		ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
		if(KEYENTER == ucKey)
			return stPrompt[ucSel].value;
		else if(KEYUP == ucKey)
		{
			if(0 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
			{
				--ucSel;
			}
		}
		else if(KEYDOWN == ucKey)
		{
			if(size - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
			{
				++ucSel;
			}
		}
		else if(KEYCANCEL == ucKey)
			return -1;
		else if(NOKEY == ucKey)
			return -1;
	}
}
