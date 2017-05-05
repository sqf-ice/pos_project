#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "data_define_salesman.h"
#include "framework.h"
#include "transaction.h"

#include "connect.h"
#include "Lang_EN.h"
#ifdef FARSI
#include "Lang_Arabic_Salesman.h"
#endif
#include "Arrow.h"

#include "printList_salesman.h"

//////////////////////////////////////////////////////////////////////////
unsigned char **gl_pLangSelPiont = gl_pStr_EN;

static unsigned char sg_StrYesNo[2][4 + 1];

unsigned char STR_ENLISH[] = "English";
#ifdef FARSI
unsigned char STR_ARABIC[] = "Arabic";
#endif

unsigned char *gl_pLang[] = 
{
	STR_ENLISH,
#ifdef FARSI
	STR_ARABIC,
#endif
};

int MySmartMenuEx(SMART_MENU *pstSmartMenu, unsigned int uiMode, int iTimeout, unsigned char *Key);

static void MenuInit(void);
static void MaintenaceParametres(void);
static void SetMaintenaceParametres(void);
static void SetConnParam(void);
static void SetPPPParam(void);
static void SetGPRSParam(void);
static void SetEthParam(void);
static void SetServerParam(void);

static int prepare_screen_text_cpy(SMART_MENU *pstSmartMenu, int iStartIndex, int iHilightIndex, 
								   unsigned int uiMode, unsigned char ucWithDisp, int *piScrItemNum, int * itemList);
static int GetNextHiliIndex_cpy(SMART_MENU *pstSmartMenu, int iHilightIndex);
static int GetFirstHiliIndex_cpy(SMART_MENU *pstSmartMenu);
static int GetPrevHiliIndex_cpy(SMART_MENU *pstSmartMenu, int iHilightIndex);
static void MapChar2Idx(unsigned char ch, int *piRow, int *piCol);
static void ShowWBuff(unsigned char line, unsigned char ucMode, unsigned char *pszWorkBuff, int iLen);
static void SelConnectType(unsigned char ucType[2]);

static void GetCharFont(unsigned char ucCol, unsigned char ucChar, unsigned char *psFont);

//////////////////////////////////////////////////////////////////////////
MENU_ITEM OrderMenu[]= 
{
	{ "Cash Order",	 ID_CashOrder,		NULL },
	{ "Credit Order",ID_CreditOrder,	NULL },
	{ "Return Order",ID_ReturnOrder,	NULL },
	{ "Credit Transfer",ID_CreditTransfer,	NULL },
	{ "",				0,					NULL },
};
SMART_MENU OrderMenuS = {NULL, sizeof(OrderMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM OfflineMenu[] = 
{
	{ "     Order      ", ID_OfflineOrder,				NULL },
	{ " Offline Visit  ", ID_OfflineVisit,				NULL },
	{ "Cancl Last Order", ID_OfflineCancelLastOrder,	NULL },
	{ "  Batch Upload  ", ID_OfflineBatchUpload,		NULL },
	{ "",				  0,							NULL },
};
SMART_MENU OfflineMenuS = {NULL, sizeof(OfflineMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM CashCollectionMenu[] = 
{
	{ "  By Merchant   ", ID_CashCollectionByMerchant,	NULL },
	{ "    By Order    ", ID_CashCollectionByOrder,		NULL },
	{ "",				  0,							NULL },
};
SMART_MENU CashCollectionMenuS = {NULL, sizeof(CashCollectionMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM BankDepositMenu[] = 
{
	{ "By Order Number ", ID_BankDepositByOrderNumber,			NULL },
	{ " Order By Batch ", ID_BankDepositOrderNumberByBatch,		NULL },
	{ " Cash. By Batch ", ID_BankDepositCashCollectionByBatch,	NULL },
	{ "",				  0,									NULL },
};
SMART_MENU BankDepositMenuS = {NULL, sizeof(BankDepositMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM InformationMenu[] =
{
	{ "Pending Orders ?", ID_Pending_Orders_ECC,	NULL },
    { " Salesman Info. ", ID_Salesman_Infos_ECC,	NULL },
	{ " Salesman Route ", ID_SalesmanRoute,			NULL },
	{ " Print Discount ", ID_PrintDiscount,			NULL },
	{ "  Change   PWD  ", ID_ChangePWD,				NULL },
	{ "",				  0,						NULL },
};
SMART_MENU InformationMenuS = {NULL, sizeof(InformationMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM ReportMenu[] =
{
	{ "Off Batch Upload", ID_Offline_Batch_Upload_Rep,	NULL },
	{ "Off Physic Stock", ID_Offline_Physical_Stock_Rep,NULL },
	{ " Order   Report ", ID_Order_Rep,				NULL },
	{ "OrderReport_Prod", ID_Order_Rep_Prod,		NULL },
	{ " Online  Stocks ", ID_Online_Stocks_Rep,		NULL },
	{ "LastConsignOrder", ID_Last_Consign_Order_Rep,NULL },
	{ "    Summary     ", ID_Summary_Rep,			NULL },
	{ "",				  0,						NULL },
};
SMART_MENU ReportMenuS = {NULL, sizeof(ReportMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM SERVER_CALLMenu[]=
{
    { "Setting Download", ID_SettingsDownload,	NULL },    
	{ "Soft Download   ", ID_SoftDownload,		NULL },
//    { "Set Auto        ", ID_Config_Auto,		NULL },
//   { "First Init. Auto", ID_First_Init__Auto,	NULL },
	{ "",				  0,					NULL },
};
SMART_MENU SERVER_CALLMenuS = {NULL, sizeof(SERVER_CALLMenu) / sizeof(MENU_ITEM),{SM_ON}};

// Added by Kim_LinHB 2013-10-28 v4.68
MENU_ITEM ConsignmentMenu[]=
{
	{ " Consign Order  ", ID_ConsignOrder,	NULL },    
	{ "Ret Consig Order", ID_ReturnConsignOrder,	NULL },
	{ "Consign Confirm ", ID_ConsignConfirm,		NULL },
	{ "Pendin ConsigOrd", ID_PendingConsignOrder,	NULL },
	{ "",				  0,					NULL },
};
SMART_MENU ConsignmentMenuS = {NULL, sizeof(ConsignmentMenu) / sizeof(MENU_ITEM),{SM_ON}};


MENU_ITEM MainMenu[]= 
{
	{ "     Order      ", ID_Order,				&OrderMenuS },
	{ "    Offline     ", ID_Offline,			&OfflineMenuS },    
	{ "Cash Collection ", ID_CashCollection,	&CashCollectionMenuS },   
	{ "  Bank Deposit  ", ID_BankDeposit,		&BankDepositMenuS },
	{ "  Record Visit  ", ID_RecordVisit,		NULL },
	{ "  Consignment   ", ID_Consignment,		&ConsignmentMenuS },
	{ "Cancl Last Order", ID_CancelLastOrder,	NULL },
	{ "  Information   ", ID_Information,		&InformationMenuS },
	{ "    Reports     ", ID_Reports,			&ReportMenuS },
//	{ "    Settings    ", ID_Settings,			NULL },
	{ "   Maintenance  ", ID_Maintenance,		NULL },
	{ "  Transmission  ", ID_Transmission,		&SERVER_CALLMenuS },
	{ "",				  0,					NULL},
};
SMART_MENU MainMenuS = {NULL, sizeof(MainMenu) / sizeof(MENU_ITEM),{SM_ON}};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ScrPrintStr(unsigned char col,unsigned char row,unsigned char mode, char *str,...)//v3.8
{
	va_list marker; 
	unsigned char mybuf[2048];

	mybuf[0]=0; 
	va_start( marker, str); 
	vsprintf(mybuf,/*(const char*)*/str,marker); 

	if ((strlen(mybuf) == 17) && (mybuf[16]==' ') ) //v4.8
	{
		mybuf[16] = 0;
	}

#ifdef FARSI  //v4.2
	if (IfArabicString(mybuf)) //v4.16
	{
		ArShowString(row,mode,mybuf);	
	}
	else
	{
		ScrPrint(col*8,row,mode,mybuf);
	}
#else
	ScrPrint(col*8,row,mode,mybuf);				//v4.6
#endif
}

void DispString(void *pszStr, unsigned short nPosition)
{
#ifdef FARSI  //v4.2
	if (IfArabicString(pszStr)) //v4.16
	{
		unsigned char mode  = nPosition & 0x00f0;
		unsigned char line  = nPosition & 0x000f;
		unsigned char size = 3;
		switch(mode)
		{
		case DISP_LINE_CENTER:
			mode = SHIFT_CENTER; break;
		case DISP_LINE_LEFT:
			mode = SHIFT_RIGHT; break;
		case DISP_LINE_RIGHT:
			mode = SHIFT_LEFT; break;
		}

		if(DISP_LINE_REVER == (nPosition & 0x0f00) || DISP_PART_REVER == (nPosition & 0x0f00))
			mode |= EFT_REVERSE;

		if(strlen(pszStr) > 16)
			size = 1;
		if (ArInit(size)!=AR_SUCCESS)
		{
			while(1)
			{
				ScrCls();
				ScrPrint(0,0,0,"Error:AR_PRNDISP.FNT error\nPls download ParamFile");
				getkey();
			}
		}
		ArShowString(line, mode, pszStr);	
	}
	else
	{
		if(strlen(pszStr) <= 16 || DISP_SCR_CENTER == nPosition)
			PubDispString(pszStr, nPosition);
		else
		{
			unsigned char line  = nPosition & 0x000f;
			unsigned char rever = 0x00;
			// Modified by Kim_LinHB 2013-4-3 v4.56
			if((nPosition & DISP_LINE_REVER) || (nPosition & DISP_PART_REVER))
				rever = REVER;
			ScrPrint(0, line, ASCII | rever, "%s", pszStr);
		}
	}
#else
	if(strlen(pszStr) <= 16)
		PubDispString(pszStr, nPosition);
	else
	{
		unsigned char line  = nPosition & 0x000f;
		unsigned char rever = 0x00;
		// Modified by Kim_LinHB 2013-4-3 v4.56
		if((nPosition & DISP_LINE_REVER) || (nPosition & DISP_PART_REVER))
			rever = REVER;
		ScrPrint(0, line | rever, ASCII, "%s", pszStr);
	}
#endif
}


void LangInit()
{
	switch(gl_MaintenaceParam.ucLang)
	{
#ifdef FARSI
	case LANG_ARABIC:
		gl_pLangSelPiont = gl_pStr_Arabic;
		break;
#endif
	case LANG_EN:
	default:
		gl_pLangSelPiont = gl_pStr_EN;
		break;
	}

	strcpy(sg_StrYesNo[0], gl_pLangSelPiont[STR_NON]);
	strcpy(sg_StrYesNo[1], gl_pLangSelPiont[STR_OUI]);

	MenuInit();
}


unsigned char CheckFirstRun(void)
{
	remove("REMOTE_BACKUP_S"); //not used

	// Added by Kim_LinHB 2013-7-12 V4.60
	if(-1 != fexist("PARAM_Sale.dat"))
	{
		FileCpy(FILE_PARAM, "PARAM_Sale.dat");
		remove("PARAM_Sale.dat");
	}
	if(-1 != fexist("OFFL_TXN_Sale.dat"))
	{
		FileCpy(FILE_OFFLINE, "OFFL_TXN_Sale.dat");
		remove("OFFL_TXN_Sale.dat");
	}
	if(-1 != fexist("OFFL_TMP_Sale.dat"))
	{
		FileCpy(FILE_OFFLINE_TMP, "OFFL_TMP_Sale.dat");
		remove("OFFL_TMP_Sale.dat");
	}
	if(-1 != fexist("OFF_CANCl_Sale.dat"))
	{
		FileCpy(FILE_OFF_CANCEL, "OFF_CANCl_Sale.dat");
		remove("OFF_CANCl_Sale.dat");
	}
	if(-1 != fexist("Merchant.dat"))
	{
		FileCpy(FILE_MERCHANT, "Merchant.dat");
		remove("Merchant.dat");
	}

	// Added by Kim_LinHB 2013-11-20 v4.72 cuz structure changed
	if(-1 != fexist("OFFLTXN.DAT"))
	{
		unsigned int i;
		unsigned int cnt = 0;
		TRANS_DATA_OLD tmp;
		TRANS_DATA_OLD2 tmp2;     // added by Allen 20151208 v4.89
		TRANS_DATA stCurTran;
		int fd = open("OFFLTXN.DAT", O_RDWR);
		long Len = filesize("OFFLTXN.DAT");
		if(fd < 0)
		{
			close(fd);
			return 0;
		}
		seek(fd, 0, SEEK_SET);
		while(Len > 0)
		{
			read(fd, (void *)&tmp, sizeof(tmp) - sizeof(tmp.stProducts));
			Len -= (sizeof(tmp) - sizeof(tmp.stProducts));
			memcpy(&tmp2, &tmp, sizeof(tmp));
			memcpy(&stCurTran, &tmp2, sizeof(tmp2));
			for (i = 0; i <tmp.ucProductNum; ++i)
			{
				read(fd, (void *)&tmp.stProducts[i], sizeof(tmp.stProducts)/ NB_MAX_PRODUCTS);
				tmp2.stProducts[i].uiCodeCategory = tmp.stProducts[i].ucCodeCategory;
				strcpy(tmp2.stProducts[i].szCodeProduct, tmp.stProducts[i].szCodeProduct);
				strcpy(tmp2.stProducts[i].szQuantity, tmp.stProducts[i].szQuantity);
			}
			for (i = 0; i <tmp2.ucProductNum; ++i)
			{
				memcpy(&stCurTran.stProducts[i], &tmp2.stProducts[i], sizeof(PRODUCT_INFO));
			}
			Len -= tmp.ucProductNum * sizeof(tmp.stProducts)/ NB_MAX_PRODUCTS;
			SaveOfflineTrans(&stCurTran, FILE_OFFLINE);
		}
		close(fd);
		remove("OFFLTXN.DAT");
	}

	if(-1 != fexist(FILE_PARAM)) 
	{// Check if version is older than 4.59, MAINTENACE_PARAM is smaller
		uchar bFirst;
		int fd;
		MAINTENACE_PARAM_OLD tmp;
		SAV_PARAM_OLD tmp2;
		TRANS_DATA_OLD tmp3;
		long sizeFile = filesize(FILE_PARAM);
		long sizeBuf_OLD = 1 + sizeof(tmp) + sizeof(tmp3);

		fd = open(FILE_PARAM,O_RDWR);
		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return 1;
		}

		read(fd, &bFirst, 1);
		read(fd, (void *)&tmp, sizeof(tmp));
		
		if(tmp.ucVersion < FILEVER)
		{
			read(fd, (void *)&tmp3, sizeof(tmp3));
			read(fd, (void *)&tmp2, sizeof(tmp2) - sizeof(tmp2.stSavParamCategory) - sizeof(tmp2.stSavParamProducts) -
				sizeof(tmp2.stRoute));

			sizeBuf_OLD += (sizeof(tmp2) - sizeof(tmp2.stSavParamCategory) - sizeof(tmp2.stSavParamProducts) -
				sizeof(tmp2.stRoute));
			sizeBuf_OLD +=  (tmp2.ucCategoryNum * sizeof(tmp2.stSavParamCategory)/ NB_MAX_CATEGORYS)+ 
				(tmp2.ucProductsNum * sizeof(tmp2.stSavParamProducts) / NB_MAX_PRODUCTS);
			read(fd, (void *)&tmp2.stSavParamCategory, tmp2.ucCategoryNum * sizeof(tmp2.stSavParamCategory)/ NB_MAX_CATEGORYS);
			read(fd, (void *)&tmp2.stSavParamProducts, tmp2.ucProductsNum * sizeof(tmp2.stSavParamProducts) / NB_MAX_PRODUCTS);
			read(fd, (void *)&tmp2.stRoute, sizeof(tmp2.stRoute));
			sizeBuf_OLD += sizeof(tmp2.stRoute);

			close(fd);

			if(sizeFile == sizeBuf_OLD) // old version, data conversion for v4.56
			{
				strcpy(gl_MaintenaceParam.szManagerID, tmp.szManagerID);
				strcpy(gl_MaintenaceParam.szTermID, tmp.szTermID);
				strcpy(gl_MaintenaceParam.szSalesmanID, tmp.szSalesmanID);
				strcpy(gl_MaintenaceParam.szSalesmanName, tmp.szSalesmanName);
				strcpy(gl_MaintenaceParam.szSalesmanCredit, tmp.szSalesmanCredit);
				strcpy(gl_MaintenaceParam.szPhysicalStock, tmp.szPhysicalStock);
				strcpy(gl_MaintenaceParam.szDealerCode, tmp.szDealerCode);
				strcpy(gl_MaintenaceParam.szMaintenancePwd, tmp.szMaintenancePwd);
				strcpy(gl_MaintenaceParam.szNetworkPwd, tmp.szNetworkPwd);
				memcpy(&gl_MaintenaceParam.stUser, &tmp.stUser, sizeof(tmp.stUser));
				gl_MaintenaceParam.ucLang = tmp.ucLang;
				gl_MaintenaceParam.bPrintBarCode = tmp.bPrintBarCode;
				gl_MaintenaceParam.bPrintLogo = tmp.bPrintLogo;
				gl_MaintenaceParam.bCancelSale = tmp.bCancelSale;
				gl_MaintenaceParam.bScanSerialNumber = tmp.bScanSerialNumber;
				gl_MaintenaceParam.bInputSerial = tmp.bInputSerial;
				gl_MaintenaceParam.bDoublePaper = tmp.bDoublePaper;
				strcpy(gl_MaintenaceParam.szLogLv, tmp.szLogLv);
				memcpy(&gl_MaintenaceParam.ucConnType, &tmp.ucConnType, 2);
				gl_MaintenaceParam.ucConnSel = tmp.ucConnSel;

				// init the missing parts
				memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
				strcpy(gl_MaintenaceParam.szTimeSettDownload, "0000");

				strcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time, tmp.szLastSoftDownLoad_Time);
				memcpy(&gl_MaintenaceParam.stFTP, &tmp.stFTP, sizeof(tmp.stFTP));
				memcpy(&gl_MaintenaceParam.stConnParam, &tmp.stConnParam, sizeof(tmp.stConnParam));

				memcpy(&gl_LastTran, &tmp3, sizeof(tmp3));
				{
					int i = 0;
					for(i = 0; i < tmp3.ucProductNum; ++i)
					{
						gl_LastTran.stProducts[i].uiCodeCategory = tmp3.stProducts[i].ucCodeCategory;
						strcpy(gl_LastTran.stProducts[i].szCodeProduct, tmp3.stProducts[i].szCodeProduct);
						strcpy(gl_LastTran.stProducts[i].szQuantity, tmp3.stProducts[i].szQuantity);
					}
				}
				gl_SavParam.usTraceNo = tmp2.usTraceNo;
				gl_SavParam.usBatchNo = tmp2.usBatchNo;
				gl_SavParam.uiMerchantListIndex = tmp2.uiMerchantListIndex;
				gl_SavParam.uiProductsListIndex = tmp2.uiProductsListIndex;
				gl_SavParam.uiCurOfflineNo = tmp2.ucCurOfflineNo;
				gl_SavParam.bIsSetup = tmp2.bIsSetup;

				memcpy(gl_SavParam.ucKey, tmp2.ucKey, 16);
				strcpy(gl_SavParam.szOrderId, tmp2.szOrderId);

				gl_SavParam.ucCategoryNum = tmp2.ucCategoryNum;
				gl_SavParam.uiProductsNum = tmp2.ucProductsNum;

				{
					int i = 0;
					for(i = 0; i < tmp2.ucCategoryNum; ++i)
					{
						gl_SavParam.stSavParamCategory[i].uiCodeCategory = tmp2.stSavParamCategory[i].ucCodeCategory;
						strcpy(gl_SavParam.stSavParamCategory[i].szName, tmp2.stSavParamCategory[i].szName);
						gl_SavParam.stSavParamCategory[i].ucCategoryType = tmp2.stSavParamCategory[i].ucCategoryType;
						gl_SavParam.stSavParamCategory[i].ucNumLogo = tmp2.stSavParamCategory[i].ucNumLogo;
					}

					for(i = 0; i < tmp2.ucProductsNum; ++i)
					{
						gl_SavParam.stSavParamProducts[i].uiCodeCategory = tmp2.stSavParamProducts[i].ucCodeCategory;
						strcpy(gl_SavParam.stSavParamProducts[i].szCodeProduct, tmp2.stSavParamProducts[i].szCodeProduct);
						gl_SavParam.stSavParamProducts[i].bCredit = tmp2.stSavParamProducts[i].bCredit;
						strcpy(gl_SavParam.stSavParamProducts[i].sReserved, tmp2.stSavParamProducts[i].sReserved);
						strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct, tmp2.stSavParamProducts[i].szNameProduct);
						strcpy(gl_SavParam.stSavParamProducts[i].szAmount, tmp2.stSavParamProducts[i].szAmount);
						strcpy(gl_SavParam.stSavParamProducts[i].szQuantity, tmp2.stSavParamProducts[i].szQuantity);
						strcpy(gl_SavParam.stSavParamProducts[i].szSoldQuantity, tmp2.stSavParamProducts[i].szSoldQuantity);
						strcpy(gl_SavParam.stSavParamProducts[i].szAverage, tmp2.stSavParamProducts[i].szAverage);
						strcpy(gl_SavParam.stSavParamProducts[i].szOrderNum, tmp2.stSavParamProducts[i].szOrderNum);
						strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, tmp2.stSavParamProducts[i].szDiscount);
					}
				}
				memcpy(&gl_SavParam.stRoute, &tmp2.stRoute, sizeof(tmp2.stRoute));

				WriteParam(WRT_ALL);
			}
		}

		return 0;
	}
	else
	{
		Beep();
		return 1;
	}
	// Add End
}	

void GetSerialNumber(unsigned char *pszSerialNumberASC)
{
	ReadSN(pszSerialNumberASC);

#ifdef _WIN32
	strcpy(pszSerialNumberASC,TEST_SN);
#endif

#ifdef LHB_TEST
	strcpy(pszSerialNumberASC,TEST_SN);
#endif

	return;
}

/*enCodeRetour*/ void GetInfoPOS(unsigned char *pszInfoPOS)
{
	//enCodeRetour enCr = OK;

    // put the OS
    // put the terminal type

#ifdef SBCE
    strcpy(pszInfoPOS, "SBCE_BANKSYS");
#elif SMASH
    strcpy(pszInfoPOS, "SMASH_BANKSYS");
#else
    strcpy(pszInfoPOS, "NEW_POS");
#endif
/*
	out_info[0]
	终端型号【0x00~0xFF】1－P60-S 2－P70 3－P60-S1 4－P80 5－P78 6－P90 7－S80 8－SP30 9－S60 10－S90 11－S78 12-MT30 13－T52 14-S58 0x80－R50 0x81－P50 0x82－P58 0x83－R30 0x84－R50-M 0x86－T60
*/
    //return enCr;
}

void Beep_Speaker(unsigned char type)
{
//#ifndef ForAlperTest // Removed by Kim_LinHB 2013-3-23 v4.55
	switch(type)
	{
	case BEEP_OK:
		PubBeepOk();break;
	case BEEP_ERR:
		PubBeepErr();break;
	default:
		Beep();break;	// the same as the button voice
	}
//#endif
}

void DispDefScreen(void)
{
	unsigned char timeACSII[SIZE_DATE_TIME + 1];
	unsigned char ucKey;

	memset( timeACSII, 0, sizeof(timeACSII));
	ScrCls();

	while(1)
	{
		GetCurTime(timeACSII);
		UpdateApp();
		ScrCls();
		DispString(APP_NAME, 0 | DISP_LINE_CENTER);

		if(GPRS == gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
			DispString(gl_MaintenaceParam.stConnParam.stParamGPRS.szOperatorName, DISPLAYLINE - 2 | DISP_LINE_CENTER);

		ScrGotoxy(4, DISPLAYLINE + 2);
		ScrDrLogo(sArrow_Up);
		ScrPrint(14,DISPLAYLINE + 2,CFONT | REVER, ":ORDER");
		ScrGotoxy(128 - 8 * 5 - 10 - 4, DISPLAYLINE + 2);
		ScrDrLogo(sArrow_Down);
		ScrPrint(128 - 8 * 5 - 4,DISPLAYLINE + 2,CFONT | REVER, ":MENU");
		ScrPrint(20, DISPLAYLINE, ASCII, "%2.2s/%2.2s/%2.2s  %2.2s:%2.2s",
			timeACSII + 4, timeACSII + 2, timeACSII, timeACSII + 6, timeACSII + 8);

		ucKey = PubWaitKey(60 - atoi(timeACSII + 10));

		if (ucKey == NOKEY)
		{	
			ParamNotSpecified();
			break;
		}
		else if (ucKey == KEYUP)
		{
			DispOrderMenu();
			break;
		}
		else if (ucKey == KEYDOWN)
		{
			DispMainMenu();
			break;
		}
		else if (ucKey == KEYMENU)
		{
			DispVersion();
			break;
		}
#ifdef LHB_TEST
		else if(ucKey == KEY1)
		{
			PubAscDec(gl_MaintenaceParam.szLastSoftDownLoad_Time, 6);
			memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
			break;
		}
		else if(ucKey == KEY2)
		{
			PubAscDec(gl_MaintenaceParam.szLastSettDownLoad_Time, 6);
			memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, gl_MaintenaceParam.szTimeSettDownload, 4);
			break;
		}
#endif
	}
}

void DispVersion()
{
	ScrCls();
	ScrPrint(0,0,1,"SOFTWARE VERSION\n[v%s]",STR_VERSION);
	ScrPrint(0,4,1,"Language\n[%s]",gl_pLang[gl_MaintenaceParam.ucLang]);
	PubWaitKey(TIME_DELAY);
}

void DispMainMenu(void)
{
	int iRet = -1;
	unsigned char ucKey = 0;

	while(1)
	{
		MenuInit();
		iRet = MySmartMenuEx(&MainMenuS, SM_ARROW_SEL | SM_MIDDLE_ITEM, TIME_OUT_STANDARD_30S, &ucKey);
	
		if(ucKey != KEYMENU && 
			iRet != ID_Maintenance &&
			iRet != ID_SettingsDownload)
		{
			if(ParamNotSpecified())
				return;
		}

 		if ((0 == gl_SavParam.ucCategoryNum ||
 			0 == strlen(gl_SavParam.stRoute.szMerchantListDate)) &&
 			iRet != ID_Maintenance &&
 			iRet != ID_SettingsDownload &&
 			iRet != ID_SoftDownload &&
 			ucKey != KEYMENU)
 		{
 			ScrCls();
 			DispString("Pls Run Sett Download", DISP_SCR_CENTER);
 			PubWaitKey(TIME_DELAY);
 			return;
 		}

		if(iRet >= 0)
		{
			if(ID_Maintenance == iRet)
				MaintenaceParametres();
			else 
				DoTrans(iRet);
		}
		else
		{
			if(KEYMENU == ucKey)
			{
				gl_MaintenaceParam.ucLang = 
					(LANG_END - 1 == gl_MaintenaceParam.ucLang ? 
						LANG_EN : gl_MaintenaceParam.ucLang + 1);
				
				ScrCls();
				DispString("Language Changed:", 1 | DISP_LINE_LEFT);
				DispString(gl_pLang[gl_MaintenaceParam.ucLang], DISPLAYLINE | DISP_LINE_LEFT);
				LangInit();
				WriteParam(WRT_MAINTA);
				Beep_Speaker(BEEP_OK);
				PubWaitKey(TIME_DELAY);

			}
			if(0 == kbhit() && KEYCANCEL == getkey())	
			{
				;//	cancel
			}
			//timeout
			return ;
		}
	}
}

void DispOrderMenu(void)
{
	int iRet;
	unsigned char ucKey = 0;

	iRet = MySmartMenuEx(&OrderMenuS, SM_ARROW_SEL | SM_MIDDLE_ITEM, TIME_OUT_STANDARD_30S, &ucKey);

	if(ParamNotSpecified())
		return;

	if(iRet >= 0)
	{
		DoTrans(iRet);
	}
	else
	{
		if(KEYMENU == ucKey)
		{
			gl_MaintenaceParam.ucLang = 
				(LANG_END - 1 == gl_MaintenaceParam.ucLang ? 
					LANG_EN : gl_MaintenaceParam.ucLang + 1);
			LangInit();
			ScrCls();
			DispString("Language Changed:", 1 | DISP_LINE_LEFT);
			DispString(gl_pLang[gl_MaintenaceParam.ucLang], DISPLAYLINE | DISP_LINE_LEFT);
			WriteParam(WRT_MAINTA);
			Beep_Speaker(BEEP_OK);
			PubWaitKey(TIME_DELAY);

		}
		if(0 == kbhit() && KEYCANCEL == getkey())	
		{
			;//	cancel
		}
		//timeout
		return ;
	}
}

void DispConnection(void)
{
	ScrCls();
	DispString(gl_pLangSelPiont[STR_CONNECTION], 1 | DISP_LINE_CENTER);
	switch(gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
	{
	case ETHERNET:
		DispString(gl_pLangSelPiont[STR_ETHERNET], DISPLAYLINE | DISP_LINE_LEFT);
		break;
	case GPRS:
		DispString(gl_pLangSelPiont[STR_GPRS], DISPLAYLINE | DISP_LINE_LEFT);
		break;
	default:
		DispString("Unknown",DISPLAYLINE | DISP_LINE_LEFT);
		break;
	}
}

void GetCurTime(unsigned char *stCurTime)
{
	unsigned char timeBCD[SIZE_DATE_TIME / 2 + 1];
	GetTime(timeBCD);
	PubBcd2Asc0(timeBCD, 6, stCurTime);	// YYMMDDHHMMSS
}

unsigned char ParamNotSpecified(void)
{
	if(strlen(gl_MaintenaceParam.szSalesmanID) > 0 &&
		strlen(gl_MaintenaceParam.szTermID) > 0 &&
		CheckIP(gl_MaintenaceParam.stConnParam.szRemoteIP) &&
		atoi(gl_MaintenaceParam.stConnParam.szRemotePort) <= 65535
		//strlen(gl_MaintenaceParam.szManagerID) > 0
		)
	{
		return FALSE;
	}
	else
	{
		ScrCls();
		DispString("Parameter", 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_INCOMPLETS], DISPLAYLINE | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_ERR);
		PubWaitKey(TIME_DELAY); //2 sec
	}
	return TRUE;
}


// Added by Kim_LinHB 2013-5-9 v4.56
void Awake_Plan()
{
	unsigned char stCurTime[SIZE_DATE_TIME + 2 + 1];
	unsigned char stLastSD[SIZE_DATE_TIME + 2 + 1];
	long offset = 0;
	unsigned char bNegative = 0;

	strcpy(stCurTime, PRE_YEAR);
	GetCurTime(stCurTime + 2);

	gl_bPlanned_SettDownload = FALSE;

	// Modified by Kim_LinHB 2013-11-26 v4.74
	if(0 == strlen(gl_MaintenaceParam.szLastSettDownLoad_Time))
	{
		GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time);
		PubAscDec(gl_MaintenaceParam.szLastSettDownLoad_Time, 6); // do twice to make sure setting download will be triggered absolutely
		PubAscDec(gl_MaintenaceParam.szLastSettDownLoad_Time, 6);
		memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, gl_MaintenaceParam.szTimeSettDownload, 4);
		WriteParam(WRT_MAINTA);
	}

	sprintf(stLastSD, "%s%s", PRE_YEAR, gl_MaintenaceParam.szLastSettDownLoad_Time);
	offset = PubCalInterval(stLastSD, stCurTime, "hh", &bNegative);
	if(!bNegative && offset >= 24)
	{
		gl_bPlanned_SettDownload = TRUE;
		DoTrans(ID_SettingsDownload);

		GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time);
		memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, gl_MaintenaceParam.szTimeSettDownload, 4);
		if(memcmp(stCurTime + 8, gl_MaintenaceParam.szTimeSettDownload, 4) < 0) // power on case
			PubAscDec(gl_MaintenaceParam.szLastSettDownLoad_Time, 6);
		WriteParam(WRT_MAINTA);
		gl_bPlanned_SettDownload = FALSE;
	}
	// Modify End
}
// Add End

void UpdateApp(void)
{
	// Modified by Kim_LinHB 2013-11-26 v4.74
	unsigned char stCurTime[SIZE_DATE_TIME + 2 +1];
	unsigned char stLastSD[SIZE_DATE_TIME + 2 + 1];
	long offset = 0;
	unsigned char bNegative = 0;

	strcpy(stCurTime, PRE_YEAR);
	GetCurTime(stCurTime + 2);

	gl_bPlanned_SettDownload = FALSE;

#ifdef LHB_TEST
	ScrCls();
	ScrPrint(0,0,1,"%s", gl_MaintenaceParam.szLastSettDownLoad_Time);
	ScrPrint(0,2,1,"%s", gl_MaintenaceParam.szLastSoftDownLoad_Time);
	getkey();
#endif

	if(0 == strlen(gl_MaintenaceParam.szLastSoftDownLoad_Time))
	{
		GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
		PubAscDec(gl_MaintenaceParam.szLastSoftDownLoad_Time, 6);
		PubAscDec(gl_MaintenaceParam.szLastSoftDownLoad_Time, 6);
		memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
		WriteParam(WRT_MAINTA);
	}

	sprintf(stLastSD, "%s%s", PRE_YEAR, gl_MaintenaceParam.szLastSoftDownLoad_Time);
	offset = PubCalInterval(stLastSD, stCurTime, "hh", &bNegative);
	if(!bNegative && offset >= 24)
	{
		gl_bPlanned_SoftDownload = TRUE;
		DoTrans(ID_SoftDownload);
		GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
		memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
		if(memcmp(stCurTime + 8, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4) < 0) // power on case
			PubAscDec(gl_MaintenaceParam.szLastSoftDownLoad_Time, 6);
		WriteParam(WRT_MAINTA);
		gl_bPlanned_SoftDownload = FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//				the following is static func
//////////////////////////////////////////////////////////////////////////

static void MaintenaceParametres(void)
{
	unsigned char ps[SIZE_MAX_PWD + 1];
	unsigned char ucKey;
	char loop = TIME_RETRY;

	while(loop > 0)
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
		do 
		{
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, ps, TIME_OUT_STANDARD_30S);
			if(0xFF == ucKey)
				return;
			if(KEYENTER == ucKey)
				break;
		} while (1);
		
		if(0 == strcmp(ps, gl_MaintenaceParam.szMaintenancePwd))
		{
			Beep_Speaker(BEEP_OK);
			SetMaintenaceParametres();
			return;
		}
		if (0 == strcmp(ps, gl_MaintenaceParam.szNetworkPwd))
		{
			Beep_Speaker(BEEP_OK);
			SelConnectType(gl_MaintenaceParam.ucConnType);
			if(GPRS == gl_MaintenaceParam.ucConnType[0] ||
				GPRS == gl_MaintenaceParam.ucConnType[1])
				GetOperatorName();
			return;
		}
		--loop;
		PubShowTwoMsg(3, gl_pLangSelPiont[STR_PASSWORD_DISPLAY], gl_pLangSelPiont[STR_ECHEC]);
		Beep_Speaker(BEEP_ERR);
		PubWaitKey(TIME_OUT_STANDARD_30S);
	}
}

static void SetMaintenaceParametres(void)
{
	enum
	{
		STEP_START = 0,
		STEP_MANAGERID,
		STEP_TERMID,
		STEP_SALESMANID,
		STEP_CONNMENU,
// 		STEP_DISCOUNT,
// 		STEP_CHGDISCOUNT,
		STEP_LOGLV,
		STEP_RESET,
		STEP_END
	};
	unsigned char ucKey;
	unsigned char ucSel = STEP_START + 1;
	
	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
		case STEP_MANAGERID:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_IDENT_MANAGER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				gl_MaintenaceParam.szManagerID,TIME_OUT_STANDARD_30S);
			break;
		case STEP_TERMID:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_ID_TERMINAL], 1 | DISP_LINE_LEFT);

			// Modified by Kim_LinHB 2013-3-25 v4.55
			// from NUM_IN to ALPHA_IN
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				gl_MaintenaceParam.szTermID, TIME_OUT_STANDARD_30S);
			// Modify End
			break;
		case STEP_SALESMANID:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_SALESMAN_ID], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				gl_MaintenaceParam.szSalesmanID,TIME_OUT_STANDARD_30S);
			break;
		case STEP_CONNMENU:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_MENU_CONNECTION], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				ucKey = 0;
				SetConnParam();
			}
			break;
// 		case STEP_DISCOUNT:
// 			ScrSetIcon(ICON_UP, OPENICON);
// 			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_PERC_DISCOUNT], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | DISCNT_IN,0, SIZE_MAX_DISCOUNT, 
// 				gl_MaintenaceParam.szDefDiscount,TIME_OUT_STANDARD_30S);
// 			break;
// 		case STEP_CHGDISCOUNT:
// 			ScrSetIcon(ICON_UP, OPENICON);
// 			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_CHANGE_DISCOUNT], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
// 				&gl_MaintenaceParam.bChangeDiscount,TIME_OUT_STANDARD_30S);
// 			break;
		case STEP_LOGLV:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_INFOS_TRACE], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 1, 1, 
				gl_MaintenaceParam.szLogLv,TIME_OUT_STANDARD_30S);
			break;
		case STEP_RESET:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_REINIT_APPLI], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_POUR_ENTRER], DISPLAYLINE | DISP_LINE_CENTER);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			break;
		}
		if(0xFF == ucKey)
		{
			ParamNotSpecified();
			return;
		}
		else if(KEYUP ==ucKey)
		{
			if (STEP_START + 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				--ucSel;
		}
		else if(KEYDOWN == ucKey)
		{
			if (STEP_END - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				++ucSel;
		}
		else if(KEYENTER == ucKey)
		{
			if(STEP_RESET == ucSel)
			{
				gl_bFirstRun = 1;
				DataInit();
				ScrCls();
				DispString("Reset Ok", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_OK);
				PubWaitKey(TIME_DELAY);
			}
			else
				WriteParam(WRT_MAINTA);
		}
		ucKey = 0;
	}
}

static void SetConnParam()
{
	enum
	{
		STEP_START = 0,
// 		STEP_PAD,
// 		STEP_PAD2,
// 		STEP_PPP,
		STEP_PRINT,
		STEP_GPRS,
		STEP_ETH,
		STEP_SERVERINFO,
		STEP_END,
	};
	unsigned char ucKey;
	unsigned char ucSel = STEP_START + 1;
	
	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
// 		case STEP_PAD:
//			ScrSetIcon(ICON_UP, CLOSEICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_NUMERO_PAD], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_NUM_PAD, 
// 				gl_MaintenaceParam.stConnParam.szNumPad,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 				WriteParam();
// 			break;
// 		case STEP_PAD2:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_NUMERO_PAD_SECOURS], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_NUM_PAD, 
// 				gl_MaintenaceParam.stConnParam.szNumPadSec,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 				WriteParam();
// 			break;
// 		case STEP_PPP:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_MENU_PPP], 1 | DISP_LINE_LEFT);//////////////////
// 			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
// 			ucKey = YesNo(TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 				SetPPPParam();
// 			break;
		case STEP_PRINT:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Prn conn sett.?", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				Prn_ConnParameter();
			break;
		case STEP_GPRS:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_PARAMETRES_GPRS], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				SetGPRSParam();
			break;
		case STEP_ETH:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_MENU_ETHERNET], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				SetEthParam();
			break;
		case STEP_SERVERINFO:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString("SERVER INFO", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				SetServerParam();
			break;
		}
		if(0xFF == ucKey)
			return;
		else if(KEYUP ==ucKey)
		{
			if (STEP_START + 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				--ucSel;
		}
		else if(KEYDOWN == ucKey)
		{
			if (STEP_END - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				++ucSel;
		}
		ucKey = 0;
	}
}
static void SetPPPParam()
{
	enum
	{
		STEP_START = 0,
		STEP_PAD,
		STEP_PAD2,
		STEP_IP,
		STEP_PORT,
		STEP_IP_BAK,
		STEP_PORT_BAK,
		STEP_UID,
		STEP_PWD,
		STEP_AUTH,
		STEP_END,
	};
	unsigned char bIPOK[2] = {TRUE, TRUE}, bPortOK[2] = {TRUE, TRUE};
	unsigned char ucKey;
	unsigned char ucSel = STEP_START + 1;
	CONN_PARAM stTmp;
	memcpy(&stTmp, &gl_MaintenaceParam.stConnParam, sizeof(CONN_PARAM));
	
	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
		case STEP_PAD:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_NUMERO_PAD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_NUM_PAD, 
				gl_MaintenaceParam.stConnParam.szNumPad,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PAD2:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_NUMERO_PAD_SECOURS], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_NUM_PAD, 
				gl_MaintenaceParam.stConnParam.szNumPadSec,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_IP:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Primary host :", 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[0])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.szRemoteIP,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(FALSE == CheckIP(stTmp.szRemoteIP))
				{
					Beep_Speaker(BEEP_ERR);
					bIPOK[0] = FALSE;
				}
				else
				{
					bIPOK[0] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, stTmp.szRemoteIP);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_PORT:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_IP_PORT], 1 | DISP_LINE_LEFT);
			if(FALSE == bPortOK[0])
				DispString("illegal IP Port", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
				stTmp.szRemotePort,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(atoi(stTmp.szRemotePort) > 65535)
				{
					Beep_Speaker(BEEP_ERR);
					bPortOK[0] =FALSE;
				}
				else
				{
					bPortOK[0] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, stTmp.szRemotePort);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_IP_BAK:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Secondary  host :", 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[1])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.szRemoteIP_Bak,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(strlen(stTmp.szRemoteIP_Bak) > 0 && 
					FALSE == CheckIP(stTmp.szRemoteIP_Bak))
				{
					Beep_Speaker(BEEP_ERR);
					bIPOK[1] = FALSE;
				}
				else
				{
					bIPOK[1] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak, stTmp.szRemoteIP_Bak);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_PORT_BAK:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("IP Port 2 :", 1 | DISP_LINE_LEFT);
			if(FALSE == bPortOK[1])
				DispString("illegal IP Port", DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
				stTmp.szRemotePort_Bak,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(strlen(stTmp.szRemotePort_Bak) > 0 &&
					atoi(stTmp.szRemotePort_Bak) > 65535)
				{
					Beep_Speaker(BEEP_ERR);
					bPortOK[1] =FALSE;
				}
				else
				{
					bPortOK[1] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemotePort_Bak, stTmp.szRemotePort_Bak);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_UID:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_USERNAME], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, MAX_USER, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szUID,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PWD:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | PASS_IN,0, MAX_PWD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szPwd,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_AUTH:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_AUTHENTI], 1 | DISP_LINE_LEFT);
			strcpy((char *)&sg_StrYesNo[0], gl_pLangSelPiont[STR_AUTHENTI_PAP]);
			strcpy((char *)&sg_StrYesNo[1], gl_pLangSelPiont[STR_AUTHENTI_SHAP]);
			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.stConnParam.stParamGPRS.ucAuth,TIME_OUT_STANDARD_30S);
			strcpy((char *)&sg_StrYesNo[0], gl_pLangSelPiont[STR_NON]);
			strcpy((char *)&sg_StrYesNo[1], gl_pLangSelPiont[STR_OUI]);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		}
		if(0xFF == ucKey)
			return;
		else if(KEYUP ==ucKey)
		{
			if (STEP_START + 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				--ucSel;
		}
		else if(KEYDOWN == ucKey)
		{
			if (STEP_END - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				++ucSel;
		}
		ucKey = 0;
	}
}

static void SetGPRSParam()
{
	enum
	{
		STEP_START = 0,
		STEP_APN,
// 		STEP_IP,
// 		STEP_PORT,
// 		STEP_IP_BAK,
// 		STEP_PORT_BAK,
		STEP_UID,
		STEP_PWD,
		STEP_PIN,
// 		STEP_AUTH,
// 		STEP_PRINT,
// 		STEP_RESET,
		STEP_END,
	};
	unsigned char ucKey;
	unsigned char ucSel = STEP_START + 1;
	
	while(1)
	{
		ScrCls();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
		case STEP_APN:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_APN], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_NUM_PAD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
// 		case STEP_IP:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString("Primary host :", 1 | DISP_LINE_LEFT);
// 			if(FALSE == bIPOK)
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
// 				gl_MaintenaceParam.stConnParam.szRemoteIP,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(FALSE == CheckIP(gl_MaintenaceParam.stConnParam.szRemoteIP))
// 				{
// 					bIPOK = FALSE;
// 				}
// 				else
// 				{
// 					bIPOK = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
// 		case STEP_PORT:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_IP_PORT], 1 | DISP_LINE_LEFT);
// 			if(FALSE == bPortOK)
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
// 				gl_MaintenaceParam.stConnParam.szRemotePort,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(atoi(gl_MaintenaceParam.stConnParam.szRemotePort) > 65535)
// 					bPortOK =FALSE;
// 				else
// 				{
// 					bPortOK = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
// 		case STEP_IP_BAK:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString("Secondary  host :", 1 | DISP_LINE_LEFT);
// 			if(FALSE == bIPOK)
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
// 				gl_MaintenaceParam.stConnParam.szRemoteIP_Bak,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(strlen(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak) > 0 &&
// 					FALSE == CheckIP(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak))
// 				{
// 					bIPOK = FALSE;
// 				}
// 				else
// 				{
// 					bIPOK = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
// 		case STEP_PORT_BAK:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString("IP Port 2 :", 1 | DISP_LINE_LEFT);
// 			if(FALSE == bPortOK)
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
// 				gl_MaintenaceParam.stConnParam.szRemotePort_Bak,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(strlen(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 0 &&
// 					atoi(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 65535)
// 				{
// 					bPortOK =FALSE;
// 				}
// 				else
// 				{
// 					bPortOK = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
		case STEP_UID:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_USERNAME], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, MAX_USER, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szUID,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PWD:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | PASS_IN,0, MAX_PWD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szPwd,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PIN:
			ScrSetIcon(ICON_UP, OPENICON);
			//ScrSetIcon(ICON_DOWN, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_PASSWORD_PIN], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | PASS_IN,0, MAX_PWD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szSimPin,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
// 		case STEP_AUTH:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_AUTHENTI], 1 | DISP_LINE_LEFT);
// 			strcpy((char *)&sg_StrYesNo[0], gl_pLangSelPiont[STR_AUTHENTI_PAP]);
// 			strcpy((char *)&sg_StrYesNo[1], gl_pLangSelPiont[STR_AUTHENTI_SHAP]);
// 			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
// 				&gl_MaintenaceParam.stConnParam.stParamGPRS.ucAuth,TIME_OUT_STANDARD_30S);
// 			strcpy((char *)&sg_StrYesNo[0], gl_pLangSelPiont[STR_NON]);
// 			strcpy((char *)&sg_StrYesNo[1], gl_pLangSelPiont[STR_OUI]);
// 			if(KEYENTER == ucKey)
// 				WriteParam();
// 			break;
// 		case STEP_PRINT:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_PRINT_GPRS_CONFIG], 1 | DISP_LINE_LEFT);
// 			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
// 			ucKey = YesNo(TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 				;//PRINT
// 			break;
// 		case STEP_RESET:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, CLOSEICON);
// 			DispString(gl_pLangSelPiont[STR_RESET_GPRS_CONFIG], 1 | DISP_LINE_LEFT);
// 			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
// 			ucKey = YesNo(TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 				memset(&gl_MaintenaceParam.stConnParam.stParamGPRS, 0, sizeof(WIRELESS_PARAM));
// 			break;
		}
		if(0xFF == ucKey)
			return;
		else if(KEYUP ==ucKey)
		{
			if (STEP_START + 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				--ucSel;
		}
		else if(KEYDOWN == ucKey)
		{
			if (STEP_END - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				++ucSel;
		}
		ucKey = 0;
	}
}

static void SetEthParam()
{
	enum
	{
		STEP_START = 0,
// 		STEP_IP,
// 		STEP_PORT,
// 		STEP_IP_BAK,
// 		STEP_PORT_BAK,
		STEP_DHCP,
		STEP_LOCAL,
		STEP_MASK,
		STEP_GATEWAY,
		STEP_DNS,
		STEP_DNS2,
//		STEP_RESET,
		STEP_END,
	};
	unsigned char bIPOK[6];
	unsigned char ucKey;
	unsigned char ucSel = STEP_START + 1;

	CONN_PARAM stTmp;
	memcpy(&stTmp, &gl_MaintenaceParam.stConnParam, sizeof(CONN_PARAM));
	memset(bIPOK , TRUE, 6);
	
	while(1)
	{
		ScrCls();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
// 		case STEP_IP:
//			ScrSetIcon(ICON_UP, CLOSEICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString("Primary host :", 1 | DISP_LINE_LEFT);
// 			if(FALSE == bIPOK[0])
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
// 				gl_MaintenaceParam.stConnParam.szRemoteIP,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(FALSE == CheckIP(gl_MaintenaceParam.stConnParam.szRemoteIP))
// 				{
// 					bIPOK[0] = FALSE;
// 				}
// 				else
// 				{
// 					bIPOK[0] = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
// 		case STEP_PORT:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_IP_PORT], 1 | DISP_LINE_LEFT);
// 			if(FALSE == bPortOK)
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
// 				gl_MaintenaceParam.stConnParam.szRemotePort,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(atoi(gl_MaintenaceParam.stConnParam.szRemotePort) > 65535)
// 					bPortOK =FALSE;
// 				else
// 				{
// 					bPortOK = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
// 		case STEP_IP_BAK:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString("Secondary  host :", 1 | DISP_LINE_LEFT);
// 			if(FALSE == bIPOK[0])
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
// 				gl_MaintenaceParam.stConnParam.szRemoteIP_Bak,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(strlen(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak) > 0 &&
// 					FALSE == CheckIP(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak))
// 				{
// 					bIPOK[0] = FALSE;
// 				}
// 				else
// 				{
// 					bIPOK[0] = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
// 		case STEP_PORT_BAK:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString("IP Port 2 :", 1 | DISP_LINE_LEFT);
// 			if(FALSE == bPortOK)
// 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
// 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
// 				gl_MaintenaceParam.stConnParam.szRemotePort_Bak,TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 			{
// 				if(strlen(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 0 &&
// 					atoi(gl_MaintenaceParam.stConnParam.szRemotePort_Bak) > 65535)
// 				{
// 					bPortOK =FALSE;
// 				}
// 				else
// 				{
// 					bPortOK = TRUE;
// 					WriteParam();
// 				}
// 			}
// 			break;
		case STEP_DHCP:
			if(TRUE == gl_MaintenaceParam.stConnParam.stParamEth.bDHCP)
			{
				ScrSetIcon(ICON_UP, CLOSEICON);
				ScrSetIcon(ICON_DOWN, CLOSEICON);
			}
			else
			{
				//ScrSetIcon(ICON_UP, OPENICON);
				ScrSetIcon(ICON_DOWN, CLOSEICON);
				ScrSetIcon(ICON_DOWN, OPENICON);
			}
			DispString(gl_pLangSelPiont[STR_DHCP], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.stConnParam.stParamEth.bDHCP,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
// 		case STEP_RESET:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, CLOSEICON);
// 			DispString(gl_pLangSelPiont[STR_RESET_GPRS_CONFIG], 1 | DISP_LINE_LEFT);  ///
// 			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
// 			ucKey = YesNo(TIME_OUT_STANDARD_30S);
// 			if(KEYENTER == ucKey)
// 				memset(&gl_MaintenaceParam.stConnParam.stParamEth, 0, sizeof(ETH_PARAM));
// 			break;
		case STEP_LOCAL:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_IP_POS], 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[1])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.stParamEth.szLocalIP,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(FALSE == CheckIP(stTmp.stParamEth.szLocalIP))
				{
					Beep_Speaker(BEEP_ERR);
					bIPOK[1] = FALSE;
				}
				else
				{
					bIPOK[1] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szLocalIP,
						stTmp.stParamEth.szLocalIP);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_MASK:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_MASQUE_SS_RESEAU], 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[2])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.stParamEth.szNetMask,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(FALSE == CheckIP(stTmp.stParamEth.szNetMask))
				{
					Beep_Speaker(BEEP_ERR);
					bIPOK[2] = FALSE;
				}
				else
				{
					bIPOK[2] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szNetMask,
						stTmp.stParamEth.szNetMask);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_GATEWAY:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_PASSERELLE], 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[3])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.stParamEth.szGatewayIP,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(FALSE == CheckIP(stTmp.stParamEth.szGatewayIP))
				{
					Beep_Speaker(BEEP_ERR);
					bIPOK[3] = FALSE;
				}
				else
				{
					bIPOK[3] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szGatewayIP,
						stTmp.stParamEth.szGatewayIP);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_DNS:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_DNS_1], 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[4])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.stParamEth.szDNS,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(FALSE == CheckIP(stTmp.stParamEth.szDNS))
				{
					Beep_Speaker(BEEP_ERR);
					bIPOK[4] = FALSE;
				}
				else
				{
					bIPOK[4] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szDNS,
						stTmp.stParamEth.szDNS);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		case STEP_DNS2:
			ScrSetIcon(ICON_UP, OPENICON);
			//ScrSetIcon(ICON_DOWN, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON); //no reset item
			DispString(gl_pLangSelPiont[STR_DNS_2], 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[5])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.stParamEth.szDNS_Bak,TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				if(FALSE == CheckIP(stTmp.stParamEth.szDNS_Bak))
				{
					Beep_Speaker(BEEP_ERR);
					bIPOK[5] = FALSE;
				}
				else
				{
					bIPOK[5] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szDNS_Bak,
						stTmp.stParamEth.szDNS_Bak);
					WriteParam(WRT_MAINTA);
				}
			}
			break;
		}
		if(0xFF == ucKey)
			return;
		else if(KEYUP ==ucKey)
		{
			if (STEP_START + 1 == ucSel)
			{
				Beep_Speaker(BEEP_ERR);
			}
			else
			{
				if(TRUE == gl_MaintenaceParam.stConnParam.stParamEth.bDHCP/* && STEP_END - 1 == ucSel*/)
					ucSel = STEP_DHCP;
				else
					--ucSel;
			}
		}
		else if(KEYDOWN == ucKey)
		{
			if (STEP_END - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
			{
				if(TRUE == gl_MaintenaceParam.stConnParam.stParamEth.bDHCP && STEP_DHCP == ucSel)
				{
					//ucSel = STEP_RESET;
					Beep_Speaker(BEEP_ERR); // no reset item
				}
				else
				{
					++ucSel;
				}
			}
		}
		ucKey = 0;
	}
}

static void SetServerParam(void)
{

	enum
	{
		STEP_START = 0,
 		STEP_IP,
 		STEP_PORT,
 		STEP_IP_BAK,
 		STEP_PORT_BAK,
		STEP_END,
	};
	unsigned char bIPOK[2] = {TRUE, TRUE}, bPortOK[2] = {TRUE, TRUE};
	unsigned char ucKey;
	unsigned char ucSel = STEP_START + 1;
	CONN_PARAM stTmp;
	memcpy(&stTmp, &gl_MaintenaceParam.stConnParam, sizeof(CONN_PARAM));
	
	while(1)
	{
		ScrCls();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
		case STEP_IP:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
 			DispString("Primary host :", 1 | DISP_LINE_LEFT);
 			if(FALSE == bIPOK[0])
 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);

 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
 				stTmp.szRemoteIP,TIME_OUT_STANDARD_30S);
 			if(KEYENTER == ucKey)
 			{
 				if(FALSE == CheckIP(stTmp.szRemoteIP))
 				{
					Beep_Speaker(BEEP_ERR);
 					bIPOK[0] = FALSE;
 				}
 				else
 				{
 					bIPOK[0] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, stTmp.szRemoteIP);
 					WriteParam(WRT_MAINTA);
 				}
 			}
 			break;
 		case STEP_PORT:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
 			DispString(gl_pLangSelPiont[STR_IP_PORT], 1 | DISP_LINE_LEFT);
 			if(FALSE == bPortOK[0])
 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
 				stTmp.szRemotePort,TIME_OUT_STANDARD_30S);
 			if(KEYENTER == ucKey)
 			{
 				if(atoi(stTmp.szRemotePort) > 65535)
				{
					Beep_Speaker(BEEP_ERR);
					bPortOK[0] =FALSE;
				}
 				else
 				{
 					bPortOK[0] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, stTmp.szRemotePort);
 					WriteParam(WRT_MAINTA);
 				}
 			}
 			break;
 		case STEP_IP_BAK:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
 			DispString("Secondary  host :", 1 | DISP_LINE_LEFT);
 			if(FALSE == bIPOK[1])
 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			
 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
 				stTmp.szRemoteIP_Bak,TIME_OUT_STANDARD_30S);
 			if(KEYENTER == ucKey)
 			{
 				if(strlen(stTmp.szRemoteIP_Bak) > 0 &&
 					FALSE == CheckIP(stTmp.szRemoteIP_Bak))
 				{
					Beep_Speaker(BEEP_ERR);
 					bIPOK[1] = FALSE;
 				}
 				else
 				{
 					bIPOK[1] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak, stTmp.szRemoteIP_Bak);
 					WriteParam(WRT_MAINTA);
 				}
 			}
 			break;
 		case STEP_PORT_BAK:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
 			DispString("IP Port 2 :", 1 | DISP_LINE_LEFT);
 			if(FALSE == bPortOK[1])
 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);

 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, MAX_PORT, 
 				stTmp.szRemotePort_Bak,TIME_OUT_STANDARD_30S);
 			if(KEYENTER == ucKey)
 			{
 				if(strlen(stTmp.szRemotePort_Bak) > 0 &&
 					atoi(stTmp.szRemotePort_Bak) > 65535)
 				{
					Beep_Speaker(BEEP_ERR);
 					bPortOK[1] =FALSE;
 				}
 				else
 				{
 					bPortOK[1] = TRUE;
					strcpy(gl_MaintenaceParam.stConnParam.szRemotePort_Bak, stTmp.szRemotePort_Bak);
 					WriteParam(WRT_MAINTA);
 				}
 			}
 			break;
		}
		if(0xFF == ucKey)
			return;
		else if(KEYUP ==ucKey)
		{
			if (STEP_START + 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				--ucSel;
		}
		else if(KEYDOWN == ucKey)
		{
			if (STEP_END - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				++ucSel;
		}
		ucKey = 0;
	}
}

static void SelConnectType(unsigned char ucType[2])
{
	unsigned char bSec = 0;
	unsigned char ucKey;
	int iSel = ucType[bSec];
	ScrCls();
	DispString(gl_pLangSelPiont[STR_SOUS_TITRE_CONNEXION], 1 | DISP_LINE_CENTER);

	while(1)
	{
		if(2 == bSec)
		{
			bSec = 1;
			ScrCls();
			DispString(gl_pLangSelPiont[STR_USE_MODE_DEGRADE], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			if(0x00 == PubYesNo(TIME_OUT_STANDARD_30S))
			{
				iSel = ucType[bSec];
				ScrCls();
				DispString(gl_pLangSelPiont[STR_USE_MODE_DEGRADE], 1 | DISP_LINE_CENTER);
				
			}
			else
				return;
		}
		ScrClrLine(DISPLAYLINE, DISPLAYLINE);
		DispString((void *)cSTRConnType[iSel], DISPLAYLINE | DISP_LINE_LEFT);

		ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
		if(KEYUP == ucKey)
			//iSel = 0 == iSel ? PPP : iSel - 1;
			iSel = (0 == iSel) ? ENDFLG - 1 : iSel - 1;
		else if(KEYDOWN == ucKey)
			//iSel = PPP == iSel ? 0 : iSel + 1;
			iSel = (ENDFLG - 1 == iSel) ? 0 : iSel + 1;
		else if(KEYENTER == ucKey)
		{
			WriteParam(WRT_MAINTA);
			ScrClrLine(DISPLAYLINE, DISPLAYLINE);
			DispString(gl_pLangSelPiont[STR_PARAM_ENREGISTRE], DISPLAYLINE | DISP_LINE_CENTER);
			Beep_Speaker(BEEP_OK);
			PubWaitKey(TIME_DELAY);
			ucType[bSec] = iSel;
			
			if(0 == bSec)
				bSec = 2;
			else
				return;
		}
		else if(NOKEY == ucKey || KEYCANCEL == ucKey)
			return;
	}
}

//return keyval
//		 0xff
unsigned char GetInput(unsigned char line, unsigned char ucMode, unsigned char ucMinlen, unsigned char ucMaxlen, unsigned char *pszOut, unsigned char ucTimeOut)
{
	unsigned char	ucKey, szWorkBuff[50 + 1];
	unsigned char	ucYN;
	static char *pszAlphaTbl[10] =
	{
		"0,*# ~`!@$%^&-+=(){}[]<>_|\\:;\"\'?/",
		"1QZ.", "2ABC", "3DEF",
		"4GHI", "5JKL", "6MNO",
		"7PRS", "8TUV", "9WXY",
	};
	int		iLen, iIndex, iLastKey;

	if (ucMaxlen < ucMinlen)
	{
		return 0xFF;
	}
	memset(szWorkBuff, 0, sizeof(szWorkBuff));

	PubASSERT( (ucMode & NUM_IN) || (ucMode & PASS_IN) || (ucMode & ALPHA_IN) );
	PubASSERT( !((ucMode & CARRY_IN) && (ucMode & ECHO_IN)) );
	if( (ucMode & CARRY_IN) || (ucMode & ECHO_IN) )
	{
		sprintf((char *)szWorkBuff, "%.*s", 512, pszOut);
	}
	if(ucMode & YESNO_IN)
	{
		ucYN = *pszOut;
		sprintf((char *)szWorkBuff, "%.*s", 512, FALSE == *pszOut ? sg_StrYesNo[0] : sg_StrYesNo[1]);
	}
	if(ucMode & DISCNT_IN)
	{
		sprintf((char *)szWorkBuff, "%.*s", 512, pszOut);
	}

	iLen = strlen((char *)szWorkBuff);

	if( iLen>0 )
	{
		MapChar2Idx(szWorkBuff[iLen-1], &iLastKey, &iIndex);
	}
	else
	{
		iLastKey = -1;
		iIndex   = 0;
	}

	while( 1 )
	{
		ShowWBuff(line, ucMode, szWorkBuff, iLen);
		ucKey = PubWaitKey(ucTimeOut);
		if( ucKey == KEYCANCEL || ucKey==NOKEY )
		{
			return 0xFF;
		}
		else if( ucKey==KEYENTER )
		{
			if(ucMode & YESNO_IN)
			{	
				*pszOut = ucYN;
			}
			else
			{
				if( iLen<(int)ucMinlen )
				{
					continue;
				}
				sprintf((char *)pszOut, "%s", szWorkBuff);
			}
			if(!(ucMode & PASS_IN))
			{
				DispString(gl_pLangSelPiont[STR_PARAM_ENREGISTRE], line | DISP_LINE_LEFT);
				DelayMs(1000);
				ScrClrLine(DISPLAYLINE,DISPLAYLINE);
			}
			return ucKey;
		}
		else if( ucKey>=KEY0 && ucKey<=KEY9 )
		{
			if(ucMode & YESNO_IN)
				continue;
			// save key in data
			if( iLen<(int)ucMaxlen )
			{
				iLastKey = ucKey - KEY0;
				iIndex   = 0;
				if((ucMode & DISCNT_IN) && SIZE_MAX_DISCOUNT - 1 == iLen)
				{
					if(0 != memcmp(szWorkBuff, "1000", SIZE_MAX_DISCOUNT - 1) || KEY0 != ucKey)// 100%
						continue;
				}
				if(ucMode & DISCNT_IN)
				{
					if(0 == atoi(szWorkBuff) && KEY0 == ucKey)
						continue;
				}

				if(ucMode & POINT_IN)
				{
					if(iLen >= 3)
						if(szWorkBuff[iLen - 3] != '.' && 
							szWorkBuff[iLen - 2] != '.' &&
							szWorkBuff[iLen - 1] != '.'
							)// not dot 
							continue;
				}

				szWorkBuff[iLen++] = ucKey;
				szWorkBuff[iLen]   = 0;
				
			}
		}
#ifdef _P80_
		else if( ucKey==KEYCLEAR )
		{
			szWorkBuff[0] = 0;
			iLen          = 0;
			iLastKey      = -1;
		}
		else if( ucKey==KEYF5 )	// backspace
#else
		else if( ucKey==KEYCLEAR )
#endif
		{
			if(ucMode & YESNO_IN)
			{
				ucYN = FALSE == ucYN  ? TRUE : FALSE;
				iLen = sizeof(sg_StrYesNo[0]);
				sprintf((char *)szWorkBuff, "%.*s", 512, FALSE == ucYN ? sg_StrYesNo[0] : sg_StrYesNo[1]);
				continue;
			}
			if( iLen<=0 )
			{
				continue;
			}
			szWorkBuff[--iLen] = 0;
			if( iLen>0 )
			{
				MapChar2Idx(szWorkBuff[iLen-1], &iLastKey, &iIndex);
			}
			else
			{
				iLastKey = -1;
				iIndex   = 0;
			}
		}
		else if( ucKey==KEYALPHA )
		{
			if((ucMode & NUM_IN ) && (ucMode & POINT_IN))
			{
				if(iLen > 1)
					if('.' == szWorkBuff[iLen - 1])
						continue;
				szWorkBuff[iLen++] = '.';
			}
			else if(ucMode & ALPHA_IN)
			{
				if( !(ucMode & ALPHA_IN) || iLastKey<0 || iLen<1 )
				{
					continue;
				}
				iIndex = (iIndex+1)%strlen(pszAlphaTbl[iLastKey]);
				szWorkBuff[iLen-1] = pszAlphaTbl[iLastKey][iIndex];
			}
		}
		// above from PubGetString
		else if(KEYUP == ucKey)
		{
			//if(!(ucMode & PASS_IN))
				return KEYUP;
		}
		else if(KEYDOWN == ucKey)
		{
			//if(!(ucMode & PASS_IN))
				return KEYDOWN;
		}
		else
			continue;//return ucKey;
	}

	return 0xFF;
}

unsigned char GetInputTime(unsigned char line, unsigned char *DateTime, unsigned char Mode, unsigned char ucTimeOut)
{
	unsigned char	ucKey, szWorkBuff[17 + 1], underLine[17 + 1];
	int		iLen, iIndex, iLastKey;
	unsigned char   month[13]= {0,31,28,31,30,31,30,31,31,30,31,30,31};
	unsigned char   mm;

	memset(szWorkBuff, 0, sizeof(szWorkBuff));
	memset(underLine, 0, sizeof(underLine));
	switch(Mode)
	{
	case TIME_GET_TIME:
		sprintf((char *)szWorkBuff, "%2.2s:%2.2s", 
			DateTime, DateTime + 2);
		iLen = strlen((char *)szWorkBuff);
		break;
	case TIME_GET_DATE:
		sprintf((char *)szWorkBuff, "%s%2.2s/%2.2s/%2.2s", 
			PRE_YEAR, DateTime, DateTime + 2, DateTime + 4);
		iLen = strlen((char *)szWorkBuff);
		mm = atoi(&szWorkBuff[5]);
		break;
	case TIME_GET_TIME | TIME_GET_DATE:
		sprintf((char *)szWorkBuff, "%s%2.2s/%2.2s/%2.2s %2.2s:%2.2s", 
			PRE_YEAR, DateTime, DateTime + 2, DateTime + 4, DateTime + 6, DateTime + 8);
		iLen = strlen((char *)szWorkBuff);
		mm = atoi(&szWorkBuff[5]);
		break;
	}
	

	if( iLen>0 )
	{
		MapChar2Idx(szWorkBuff[iLen-1], &iLastKey, &iIndex);
	}
	else
	{
		iLastKey = -1;
		iIndex   = 0;
	}

	while( 1 )
	{
		ScrClrLine(line,line + 2);
		DispString(szWorkBuff, line | DISP_LINE_CENTER);
		switch(Mode)
		{
		case TIME_GET_TIME:
			sprintf(underLine, "%5s"," ");
			break;
		case TIME_GET_DATE:
			sprintf(underLine, "%10s"," ");
			break;
		case TIME_GET_TIME | TIME_GET_DATE:
			sprintf(underLine, "%16s"," ");
			break;
		}
		underLine[iLen] = '-';
		DispString(underLine, line + 2 | DISP_LINE_CENTER);
		ucKey = PubWaitKey(ucTimeOut);
		if( ucKey == KEYCANCEL || ucKey==NOKEY )
		{
			return 0xFF;
		}
		else if( ucKey==KEYENTER )
		{
			switch(Mode)
			{
			case TIME_GET_TIME:
				sprintf((char *)DateTime, "%2.2s%2.2s", 
					szWorkBuff, szWorkBuff + 3);		// 00:00
				break;
			case TIME_GET_DATE:
				sprintf((char *)DateTime, "%2.2s%2.2s%2.2s", 
					szWorkBuff + 2, szWorkBuff + 5,szWorkBuff + 8);	//121215
				break;
			case TIME_GET_TIME | TIME_GET_DATE:
				sprintf((char *)DateTime, "%2.2s%2.2s%2.2s%2.2s%2.2s", 
					szWorkBuff + 2, szWorkBuff + 5,szWorkBuff + 8,szWorkBuff + 11,szWorkBuff + 14);  //1212150000
				break;
			}
			DispString(gl_pLangSelPiont[STR_PARAM_ENREGISTRE], line | DISP_LINE_LEFT);
			DelayMs(1000);
			ScrClrLine(DISPLAYLINE,DISPLAYLINE);
			return ucKey;
		}
		else if( ucKey>=KEY0 && ucKey<=KEY9 )
		{
			// save key in data
			switch(Mode)
			{
			case TIME_GET_TIME:
				if(iLen < 5)
				{
					iLastKey = ucKey - KEY0;
					iIndex   = 0;

					if(0 == iLen && iLastKey > 2)
						continue;
					if(1 == iLen && (szWorkBuff[0] - '0') * 10 + iLastKey > 23)
						continue;
					if(3 == iLen && iLastKey > 5)
						continue;
					if(4 == iLen && (szWorkBuff[3] - '0') * 10 + iLastKey > 59)
						continue;

					szWorkBuff[iLen++] = ucKey;
					if(2 == iLen)
						++iLen;
				}
				break;
			case TIME_GET_DATE:
				if( iLen<10 )
				{
					iLastKey = ucKey - KEY0;
					iIndex   = 0;

					if(5 == iLen && iLastKey > 1)
						continue;
					if(6 == iLen && (mm = (szWorkBuff[5] - '0') * 10 + iLastKey) > 12)
						continue;
					if(9 == iLen)
					{
						int year = atoi(szWorkBuff);
						if(0 == year%400 || (0 == year%4 && year%100 != 0))
						{	
							month[2] = 29;
							if((szWorkBuff[8] - '0') * 10 + iLastKey > month[mm])
								continue;
						}
						else
						{
							month[2] = 28;
							if((szWorkBuff[8] - '0') * 10 + iLastKey > month[mm])
								continue;
						}
					}
					
					szWorkBuff[iLen++] = ucKey;
					if(4 == iLen || 7 == iLen)
						++iLen;
				}
				break;
			case TIME_GET_TIME | TIME_GET_DATE:
				if( iLen< 16 )
				{
					iLastKey = ucKey - KEY0;
					iIndex   = 0;

					if(5 == iLen && iLastKey > 1)
						continue;
					if(6 == iLen && (mm = (szWorkBuff[5] - '0') * 10 + iLastKey) > 12)
						continue;
					if(9 == iLen)
					{
						int year = atoi(szWorkBuff);
						if(0 == year%400 || (0 == year%4 && year%100 != 0))
						{	
							month[2] = 29;
							if((szWorkBuff[8] - '0') * 10 + iLastKey > month[mm])
								continue;
						}
						else
						{
							month[2] = 28;
							if((szWorkBuff[8] - '0') * 10 + iLastKey > month[mm])
								continue;
						}
					}
					if(11 == iLen && iLastKey > 2)
						continue;
					if(12 == iLen && (szWorkBuff[11] - '0') * 10 + iLastKey > 23)
						continue;
					if(14 == iLen && iLastKey > 5)
						continue;
					if(15 == iLen && (szWorkBuff[14] - '0') * 10 + iLastKey > 59)
						continue;

					szWorkBuff[iLen++] = ucKey;
					if(4 == iLen || 7 ==iLen || 10 == iLen || 13 == iLen)
						++iLen;
				}
				break;
			}
		}
#ifdef _P80_
		else if( ucKey==KEYCLEAR )
		{
			szWorkBuff[0] = 0;
			iLen          = 0;
			iLastKey      = -1;
		}
		else if( ucKey==KEYF5 )	// backspace
#else
		else if( ucKey==KEYCLEAR )
#endif
		{
			if( iLen<=0 )
			{
				continue;
			}

			switch(Mode)
			{
			case TIME_GET_TIME:
				if(3 == iLen)
					--iLen;
				szWorkBuff[--iLen] = '0';
				break;
			case TIME_GET_DATE:
				if(5 == iLen || 8 ==iLen)
					--iLen;

				if(10 == iLen || 7 == iLen)
					szWorkBuff[--iLen] = '1';
				else
					szWorkBuff[--iLen] = '0';
				break;
			case TIME_GET_TIME | TIME_GET_DATE:
				if(5 == iLen || 8 ==iLen || 11 == iLen || 14 == iLen)
					--iLen;

				if(10 == iLen || 7 == iLen)
					szWorkBuff[--iLen] = '1';
				else
					szWorkBuff[--iLen] = '0';
				break;
			}

			if( iLen>0 )
			{
				MapChar2Idx(szWorkBuff[iLen-1], &iLastKey, &iIndex);
			}
			else
			{
				iLastKey = -1;
				iIndex   = 0;
			}
		}
		else
			continue;
	}

	return 0xFF;
}

unsigned char YesNo(unsigned short nWaitTime)
{
	unsigned char   ucTmp;

	while (1)
	{
		ucTmp = PubWaitKey(nWaitTime);
		if (ucTmp==KEYENTER)
		{
			return  KEYENTER;
		}
		if (ucTmp==KEYCANCEL || ucTmp==NOKEY)
		{
			return  0xFF;
		}
		if(KEYUP == ucTmp)
			return KEYUP;
		if(KEYDOWN == ucTmp)
			return KEYDOWN;
	}
}


int MySmartMenuEx(SMART_MENU *pstSmartMenu, unsigned int uiMode, int iTimeout, unsigned char *Key)
{

	int i;
	int iStartIndex;
	int iEndIndex;
	unsigned char ucKey;    
	int ScrIndex[10];	// 记录每一屏的第一个菜单项在sm->content中的索引    
	int iCurScr = 0;	// Current Screen sequence 当前的菜单所在的屏数   
	int iHilightIndex;	// Current highlighted item 当前的高亮项目索引
	int iRet;    
	unsigned char ucStartKey;
	int iScrItemNum;
	int itemList[16];
	int iTargetIdx;

	for(i = 0; i<16; i++) itemList[i] = 0;

	if (pstSmartMenu == NULL)
	{
		return -1;
	}

	if(pstSmartMenu->pstContent == NULL) 
	{
		PubShowTwoMsg(2, "SMART MENU", "NOT INIT'ED");
		getkey();
		return -1;
	}

	if(uiMode & SM_ZERO_START)
	{
		ucStartKey = KEY0;
	}
	else
	{
		ucStartKey = KEY1;
	}

	//Process title 处理标题
	if(pstSmartMenu->pstContent[0].iValue < 0) 
	{
		iStartIndex = 1;
	}
	else
	{
		iStartIndex = 0;
	}

	//Locate first available item 找到第一个有效的菜单项
	iStartIndex = GetFirstHiliIndex_cpy(pstSmartMenu);
	if(iStartIndex < 0) 
	{
		return -1;
	}

	iHilightIndex = iStartIndex;

	//Find the first menu item in each screen. 确定每一屏的第一个菜单项的索引
	for(i = 0; i < 10; i++)
	{
		ScrIndex[i] = -1;
	}

	ScrIndex[0] = iStartIndex;
	i = 1;
	while(iStartIndex < pstSmartMenu->iNum) 
	{
		iEndIndex = prepare_screen_text_cpy(pstSmartMenu, iStartIndex, iHilightIndex, uiMode, 0, &iScrItemNum, itemList);
		ScrIndex[i++] = iEndIndex;
		iStartIndex = iEndIndex;
	}

	iStartIndex = ScrIndex[0];
	iHilightIndex =iStartIndex;
	iCurScr = 0;
	while(1) 
	{
		iStartIndex = ScrIndex[iCurScr];
		iEndIndex = prepare_screen_text_cpy(pstSmartMenu, iStartIndex, iHilightIndex, uiMode, 1, &iScrItemNum, itemList);

		if(uiMode & SM_ARROW_SEL) 
		{
			if(iHilightIndex > ScrIndex[0]) 
			{
				ScrSetIcon(MENU_ICON_UP, MENU_OPENICON);
			}
			else 
			{
				ScrSetIcon(MENU_ICON_UP, MENU_CLOSEICON);
			}

			if((ScrIndex[iCurScr + 1] >= pstSmartMenu->iNum )
				&& (iHilightIndex == iStartIndex + iScrItemNum -1 ))
			{
				ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
			}
			else 
			{
				ScrSetIcon(MENU_ICON_DOWN, MENU_OPENICON);
			}
		}
		else 
		{
			if(iCurScr > 0) 
			{
				ScrSetIcon(MENU_ICON_UP, MENU_OPENICON);
			}
			else 
			{
				ScrSetIcon(MENU_ICON_UP, MENU_CLOSEICON);
			}

			if(ScrIndex[iCurScr + 1] >= pstSmartMenu->iNum )
			{
				ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
			}
			else 
			{
				ScrSetIcon(MENU_ICON_DOWN, MENU_OPENICON);
			}
		}

		if (iTimeout==0)
		{
			ucKey = PubWaitKey(-1);
		}
		else
		{
			ucKey = PubWaitKey((short)iTimeout);
		}

		if (ucKey == KEYCANCEL || ucKey == NOKEY)
		{
			ScrSetIcon(MENU_ICON_UP,   MENU_CLOSEICON);
			ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
			return -1;
		}

		if(Key)
			*Key = ucKey;

		if(uiMode & SM_ARROW_SEL) 
		{
			if(ucKey == KEYENTER) 
			{
				if(pstSmartMenu->pstContent[iHilightIndex].pstSubMenu) 
				{
					iRet = MySmartMenuEx(pstSmartMenu->pstContent[iHilightIndex].pstSubMenu, uiMode, iTimeout, Key);
					if(uiMode & SM_DIRECT_RETURN) 
					{
						ScrSetIcon(MENU_ICON_UP,   MENU_CLOSEICON);
						ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
						return iRet;
					}
					else if(iRet > 0)
					{
						ScrSetIcon(MENU_ICON_UP,   MENU_CLOSEICON);
						ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
						return iRet;
					}
				}
				else 
				{
					ScrSetIcon(MENU_ICON_UP,   MENU_CLOSEICON);
					ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
					return pstSmartMenu->pstContent[iHilightIndex].iValue;
				}
			}
			else if(ucKey == KEYUP) 
			{
				iHilightIndex = GetPrevHiliIndex_cpy(pstSmartMenu, iHilightIndex);
				//If Still can show 本屏还能显示
				if(iHilightIndex >= iStartIndex) 
				{
					continue;
				}
				//本屏不能显示
				else 
				{
					if(iCurScr > 0) 
					{
						for(i = iCurScr; i > 0; i++) 
						{
							if(iHilightIndex >= ScrIndex[i -1] && iHilightIndex < ScrIndex[i]) 
							{
								iCurScr = i - 1;
								break;
							}
						}
					}
					continue;
				}
			}
			else if(ucKey == KEYDOWN) 
			{
				iHilightIndex = GetNextHiliIndex_cpy(pstSmartMenu, iHilightIndex);

				for(i = iCurScr + 1; i < 10; i++) 
				{
					if(ScrIndex[i] == -1) 
					{
						iCurScr = i -1;
						break;
					}
					if(iHilightIndex >= ScrIndex[i -1] && iHilightIndex < ScrIndex[i]) 
					{
						iCurScr = i - 1;
						break;
					}   
				}
				continue;
			}
			else if(KEYMENU == ucKey)
				return -1;
		}
		else 
		{
			if(ucKey == KEYUP) 
			{
				if(iCurScr > 0) 
				{
					iCurScr--;
				}

				iStartIndex = ScrIndex[iCurScr];
			}
			else if(ucKey == KEYDOWN || ucKey == KEYENTER) 
			{
				if(iEndIndex < pstSmartMenu->iNum) 
				{
					iCurScr++;
					iStartIndex = iEndIndex;    
				}
			}
			else if(ucKey >= ucStartKey && ucKey < ucStartKey + iScrItemNum ) 
			{
				iTargetIdx = itemList[ucKey - ucStartKey];
				if(pstSmartMenu->pstContent[iTargetIdx].pstSubMenu)
				{
					iRet = MySmartMenuEx(pstSmartMenu->pstContent[iTargetIdx].pstSubMenu, uiMode, iTimeout, Key);
					if(uiMode & SM_DIRECT_RETURN) 
					{
						ScrSetIcon(MENU_ICON_UP,   MENU_CLOSEICON);
						ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
						return iRet;
					}
					else if(iRet > 0)
					{
						ScrSetIcon(MENU_ICON_UP,   MENU_CLOSEICON);
						ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
						return iRet;
					}
				}
				else 
				{
					ScrSetIcon(MENU_ICON_UP,   MENU_CLOSEICON);
					ScrSetIcon(MENU_ICON_DOWN, MENU_CLOSEICON);
					return pstSmartMenu->pstContent[iTargetIdx].iValue;
				}
			}
			else if(KEYMENU == ucKey)
				return -1;
		}
	}	
}

void DispRate(unsigned char ucLineNo, long lTotal, long lCurrent, long lOld)
{
	unsigned int uiPercent;
	unsigned int uiOldPer;
	unsigned char ucCol;
	unsigned char ucOldCol;
	unsigned char sRateBuff[300];
	unsigned char ucChar;	

	if (lTotal <= 0l || lCurrent > lTotal || lCurrent < 0l)
	{
		return;
	}

	if (lOld >= lCurrent || lOld < 0l)
	{
		lOld = 0l;
	}

	if (lOld == 0l)
	{
		ScrClrLine((unsigned char)(ucLineNo%8), (unsigned char)(ucLineNo%8+1));
		for (ucCol = 0; ucCol < 128; ucCol++)
		{
			ScrPlot(ucCol, (unsigned char)((ucLineNo%8)*8-1), 1);
			ScrPlot(ucCol, (unsigned char)((ucLineNo%8+2)*8), 1);
		}
		for (ucCol = 0; ucCol < 17; ucCol++)
		{
			ScrPlot(0, (unsigned char)((ucLineNo%8)*8+ucCol), 1);
			ScrPlot(127, (unsigned char)((ucLineNo%8)*8+ucCol), 1);
		}
	}

	uiOldPer = (lOld*100)/lTotal;
	uiPercent = (lCurrent*100)/lTotal;
	ucOldCol = (unsigned char)(uiOldPer*128/100);
	ucCol = (unsigned char)(uiPercent*128/100);
	memset(sRateBuff, 0, sizeof(sRateBuff));

	sRateBuff[0] = 0x02;
	sRateBuff[1] = (ucCol-ucOldCol)/256;
	sRateBuff[2] = (ucCol-ucOldCol)%256;
	memset(&sRateBuff[3], '\xff', (unsigned char)(ucCol-ucOldCol));
	memmove(&sRateBuff[3+ucCol-ucOldCol], &sRateBuff[1], ucCol-ucOldCol+2);
	ScrGotoxy(ucOldCol, ucLineNo);
	ScrDrLogo(sRateBuff);

	memset(sRateBuff, 0, sizeof(sRateBuff));
	if (ucCol < 52)
	{
		if (uiPercent >= 10)
		{
			ucChar = uiPercent/10;
			GetCharFont(0, ucChar, sRateBuff);
			ScrGotoxy(52, ucLineNo);
			ScrDrLogo(sRateBuff);
		}


		ucChar = uiPercent%10;
		GetCharFont(0, ucChar, sRateBuff);
		ScrGotoxy(60, ucLineNo);
		ScrDrLogo(sRateBuff);

		ucChar = '%';
		GetCharFont(0, ucChar, sRateBuff);
		ScrGotoxy(68, ucLineNo);
		ScrDrLogo(sRateBuff);
		//		ScrPrint(52, ucLineNo, CFONT, "%2d%%", uiPercent);
	}
	else if (ucCol >= 52 && ucCol < 60)
	{
		ucChar = uiPercent/10;
		GetCharFont((unsigned char)(ucCol-52), ucChar, sRateBuff);
		ScrGotoxy(52, ucLineNo);
		ScrDrLogo(sRateBuff);

		ucChar = uiPercent%10;
		GetCharFont(0, ucChar, sRateBuff);
		ScrGotoxy(60, ucLineNo);
		ScrDrLogo(sRateBuff);

		ucChar = '%';
		GetCharFont(0, ucChar, sRateBuff);
		ScrGotoxy(68, ucLineNo);
		ScrDrLogo(sRateBuff);
		//		ScrPrint(60, ucLineNo, CFONT, "%1d%%", uiPercent%10);
	}
	else if (ucCol >= 60 && ucCol < 68)
	{
		ucChar = uiPercent/10;
		GetCharFont(8, ucChar, sRateBuff);
		ScrGotoxy(52, ucLineNo);
		ScrDrLogo(sRateBuff);
		//		ScrPrint(52, ucLineNo, CFONT|REVER, "%1d", uiPercent/10);
		ucChar = uiPercent%10;
		GetCharFont((unsigned char)(ucCol-60), ucChar, sRateBuff);
		ScrGotoxy(60, ucLineNo);
		ScrDrLogo(sRateBuff);
		//		ScrPrint(68, ucLineNo, CFONT, "%%");
		ucChar = '%';
		GetCharFont(0, ucChar, sRateBuff);
		ScrGotoxy(68, ucLineNo);
		ScrDrLogo(sRateBuff);
	}
	else if (ucCol >= 68 && ucCol < 76)
	{
		ucChar = uiPercent/10;
		GetCharFont(8, ucChar, sRateBuff);
		ScrGotoxy(52, ucLineNo);
		ScrDrLogo(sRateBuff);

		ucChar = uiPercent%10;
		GetCharFont(8, ucChar, sRateBuff);
		ScrGotoxy(60, ucLineNo);
		ScrDrLogo(sRateBuff);
		//		ScrPrint(52, ucLineNo, CFONT|REVER, "%2d", uiPercent);
		ucChar = '%';
		GetCharFont((unsigned char)(ucCol-68), ucChar, sRateBuff);
		ScrGotoxy(68, ucLineNo);
		ScrDrLogo(sRateBuff);
	}
	else
	{
		if (uiPercent >= 100)
		{
			ucChar = 1;
			GetCharFont(8, ucChar, sRateBuff);
			ScrGotoxy(44, ucLineNo);
			ScrDrLogo(sRateBuff);
		}
		ucChar = uiPercent/10;
		if (ucChar>= 10)
		{
			ucChar = ucChar%10;
		}
		GetCharFont(8, ucChar, sRateBuff);
		ScrGotoxy(52, ucLineNo);
		ScrDrLogo(sRateBuff);

		ucChar = uiPercent%10;
		GetCharFont(8, ucChar, sRateBuff);
		ScrGotoxy(60, ucLineNo);
		ScrDrLogo(sRateBuff);

		ucChar = '%';
		GetCharFont(8, ucChar, sRateBuff);
		ScrGotoxy(68, ucLineNo);
		ScrDrLogo(sRateBuff);
		//		ScrPrint(52, ucLineNo, CFONT|REVER, "%2d%%", uiPercent);
	}

	return;
}

const unsigned char ASC8X16FONT[]=
{
0xf0,0xf8,0x0c,0xc4,0x0c,0xf8,0xf0,0x00,0x03,0x07,0x0c,0x08,0x0c,0x07,0x03,0x00,
0x00,0x10,0x18,0xfc,0xfc,0x00,0x00,0x00,0x00,0x08,0x08,0x0f,0x0f,0x08,0x08,0x00,
0x08,0x0c,0x84,0xc4,0x64,0x3c,0x18,0x00,0x0e,0x0f,0x09,0x08,0x08,0x0c,0x0c,0x00,
0x08,0x0c,0x44,0x44,0x44,0xfc,0xb8,0x00,0x04,0x0c,0x08,0x08,0x08,0x0f,0x07,0x00,
0xc0,0xe0,0xb0,0x98,0xfc,0xfc,0x80,0x00,0x00,0x00,0x00,0x08,0x0f,0x0f,0x08,0x00,
0x7c,0x7c,0x44,0x44,0x44,0xc4,0x84,0x00,0x04,0x0c,0x08,0x08,0x08,0x0f,0x07,0x00,
0xf0,0xf8,0x4c,0x44,0x44,0xc0,0x80,0x00,0x07,0x0f,0x08,0x08,0x08,0x0f,0x07,0x00,
0x0c,0x0c,0x04,0x84,0xc4,0x7c,0x3c,0x00,0x00,0x00,0x0f,0x0f,0x00,0x00,0x00,0x00,
0xb8,0xfc,0x44,0x44,0x44,0xfc,0xb8,0x00,0x07,0x0f,0x08,0x08,0x08,0x0f,0x07,0x00,
0x38,0x7c,0x44,0x44,0x44,0xfc,0xf8,0x00,0x00,0x08,0x08,0x08,0x0c,0x07,0x03,0x00,
0x30,0x30,0x00,0x80,0xc0,0x60,0x30,0x00,0x0c,0x06,0x03,0x01,0x00,0x0c,0x0c,0x00
};

static void GetCharFont(unsigned char ucCol, unsigned char ucChar, unsigned char *psFont)
{
	int iCnt;

	if (ucChar == '%')
	{
		ucChar = 0x0a;
	}
	psFont[0] = 0x02;
	psFont[1] = psFont[11] = 0x00;
	psFont[2] = psFont[12] = 0x08;

	for (iCnt = 0; iCnt < 8; iCnt++)
	{
		if (iCnt < ucCol)
		{
			psFont[3+iCnt] = ~ASC8X16FONT[ucChar*16+iCnt];
			psFont[3+10+iCnt] = ~ASC8X16FONT[ucChar*16+iCnt+8];
		}
		else
		{
			psFont[3+iCnt] = ASC8X16FONT[ucChar*16+iCnt];
			psFont[3+10+iCnt] = ASC8X16FONT[ucChar*16+iCnt+8];
		}
	}

	return;
}

// Prepare the current screen in smart menu
// 返回下一屏第一个菜单项在sm->content中的索引（如果下一屏已经没有菜单，则返回sm->num）
// 参数 pstSmartMenu - 菜单
//      iStartIndex - 本屏第一个菜单项在sm->content中的索引
//      iHilightIndex - 需要高亮显示的菜单项在sm->content中的索引（只在SM_ARROW_SEL模式下有效）
//      uiMode-显示模式
//      ucWithDisp - 1 显示 0 不显示，只计算
//      piScrItemNum  本屏的菜单项个数，用于输出
static int prepare_screen_text_cpy(SMART_MENU *pstSmartMenu, int iStartIndex, int iHilightIndex, 
							   unsigned int uiMode, unsigned char ucWithDisp, int *piScrItemNum, int * itemList)
{
	int iCurLine = 0;
	int iNumIndex;
	int i;
	int iCurItem;
	int iFlag = 0;
	char buf[SM_LINE_WIDTH * 2 +1];
	char buf1[SM_LINE_WIDTH * 2 +1];
	int iHilightLine;
	int iEndIndex;
	char menu_text[4][SM_LINE_WIDTH + 1];
	unsigned short nDispMode;
	unsigned short nReverFlag;
	int k = 0;

	memset(buf, 0x00, sizeof(buf));  
	memset(buf1, 0x00, sizeof(buf)); 

	*piScrItemNum = 0;

	for(i = 0; i < 4; i++) 
	{
		memset(menu_text[i], 0, sizeof(menu_text[i]));
	}

	//Title 标题
	if(pstSmartMenu->pstContent[0].iValue < 0) 
	{
		if (strlen(pstSmartMenu->pstContent[0].pszText) > SM_LINE_WIDTH)
		{
			memcpy(menu_text[0], pstSmartMenu->pstContent[0].pszText, SM_LINE_WIDTH);
		}
		else
		{
			strcpy(menu_text[0], pstSmartMenu->pstContent[0].pszText);
		}
		iCurLine = 1;
	}

	if(uiMode & SM_ZERO_START)
	{
		iNumIndex = 0;
	}
	else
	{
		iNumIndex = 1;
	}

	iCurItem = iStartIndex;

	iEndIndex = pstSmartMenu->iNum;

	for(i = iCurItem; i < pstSmartMenu->iNum; i++) 
	{	
		if(pstSmartMenu->sMask[i] == SM_OFF) 
		{
			continue;
		}

		if(i == iHilightIndex)
		{
			iHilightLine = iCurLine;
		}

		itemList[k++] = i;

		if(uiMode & SM_ARROW_SEL)
		{
			sprintf(buf, "%s", pstSmartMenu->pstContent[i].pszText);
		}
		else
		{
			sprintf(buf, "%d.%s", iNumIndex, pstSmartMenu->pstContent[i].pszText);
		}

		/*处理超长的菜单项,但未考虑半个汉字问题:-(*/
		if(strlen(buf) > 16) 
		{
			buf[16] = 0;         
		}

		iNumIndex++;

		//使用方向键选择，强制每行一条
		if((uiMode & SM_1ITEM_PER_LINE) || (uiMode & SM_ARROW_SEL)) 
		{
			strcpy(menu_text[iCurLine], buf);
			(*piScrItemNum) ++;
			iCurLine++;
			if(iCurLine == 4) 
			{
				iEndIndex = i+1;
				break;
			}
		}
		else 
		{
			if(iFlag == 0) 
			{ //cur_line行当前无内容
				strcpy(menu_text[iCurLine], buf);
				(*piScrItemNum) ++;
				iFlag = 1;
			}
			else 
			{          //cur_line已有内容
				if(strlen(buf) + strlen(menu_text[iCurLine]) <= 16)
				{//要加入的项可以放入当前行
					strcpy(buf1, menu_text[iCurLine]);
					memset(menu_text[iCurLine], ' ', 16);
					memcpy(menu_text[iCurLine], buf1, strlen(buf1));
					memcpy(menu_text[iCurLine]+16-strlen(buf), buf, strlen(buf));
					menu_text[iCurLine][16] = 0;
					iCurLine++;
					(*piScrItemNum) ++;
					if(iCurLine == 4)
					{
						iEndIndex = i+1;
						break;
					}
					iFlag = 0;
				}
				else
				{ //另起一行
					if(iCurLine == 3)
					{
						iEndIndex = i;
						break;
					}
					iCurLine++;
					strcpy(menu_text[iCurLine], buf);
					(*piScrItemNum) ++;
					iFlag = 1;
				}
			}
		}
	}

	if(ucWithDisp) 
	{
		for(i = 0; i < 4; i++) 
		{
			if(pstSmartMenu->pstContent[0].iValue < 0 && i ==0) 
			{	
				nDispMode = 0;
				if (uiMode & SM_MIDDLE_TITLE)
				{
					nDispMode |= DISP_LINE_CENTER;
				}

				DispString(menu_text[i],(unsigned short)((i*2)|nDispMode|DISP_LINE_REVER));
			}
			else
			{
				nDispMode = 0;
				if (uiMode & SM_MIDDLE_ITEM)
				{
					nDispMode |= DISP_LINE_CENTER;
				}

				if ((unsigned char)((i == iHilightLine) && (uiMode & SM_ARROW_SEL)))
				{
					nReverFlag = DISP_LINE_REVER;
				}
				else
				{
					nReverFlag = 0;
				}

				DispString(menu_text[i],(unsigned short)((i*2)|nDispMode|nReverFlag));
			}
		}
	}

	//返回下一个有效菜单索引，如果没有就返回sm->num
	for(i = iEndIndex ; i < pstSmartMenu->iNum; i++) 
	{
		if(pstSmartMenu->sMask[i] == SM_ON) 
			break;
	}

	return i;
}

static int GetNextHiliIndex_cpy(SMART_MENU *pstSmartMenu, int iHilightIndex)
{
	int i = 0;

	for(i = iHilightIndex+1; i < pstSmartMenu->iNum; i++) 
	{
		if(pstSmartMenu->sMask[i] == SM_ON) 
		{
			return i;
		}
	}
	return iHilightIndex;

}

static int GetFirstHiliIndex_cpy(SMART_MENU *pstSmartMenu)
{
	int i = 0;
	int j;
	if(pstSmartMenu->pstContent->iValue < 0) i = 1;

	for(j = i; j < pstSmartMenu->iNum; j++) 
	{
		if(pstSmartMenu->sMask[j] == SM_ON) 
		{
			return j;
		}
	}

	return -1;
}

static int GetPrevHiliIndex_cpy(SMART_MENU *pstSmartMenu, int iHilightIndex)
{
	int iLow = 0;
	int i;

	if(pstSmartMenu->pstContent[0].iValue < 0)
	{
		iLow = 1;
	}

	if(iHilightIndex == iLow) 
	{
		return iHilightIndex;
	}

	for(i = iHilightIndex -1; i >= iLow; i--) 
	{
		if(pstSmartMenu->sMask[i] == SM_ON) 
		{
			return i;
		}
	}

	return iHilightIndex;
}

static void MapChar2Idx(unsigned char ch, int *piRow, int *piCol)
{
	int		i;
	char	*p;
	static char *pszAlphaTbl[10] =
	{
		"0,*# ~`!@$%^&-+=(){}[]<>_|\\:;\"\'?/",
		"1QZ.", "2ABC", "3DEF",
		"4GHI", "5JKL", "6MNO",
		"7PRS", "8TUV", "9WXY",
	};

	for(i=0; i<sizeof(pszAlphaTbl)/sizeof(pszAlphaTbl[0]); i++)
	{
		for(p=pszAlphaTbl[i]; *p; p++)
		{
			if( *p==toupper(ch) )
			{
				*piRow = i;
				*piCol = p-pszAlphaTbl[i];
				break;
			}
		}
	}
}

static void ShowWBuff(unsigned char line, unsigned char ucMode, unsigned char *pszWorkBuff, int iLen)
{
	int		iLineNum, iCnt, iLeftBytes;
	unsigned char	sTemp[NUM_MAXZHCHARS];

	iLineNum = (iLen+NUM_MAXZHCHARS-1)/NUM_MAXZHCHARS;
	if( iLineNum>=NUM_MAXZHLINES )
	{
		ScrCls();
	}
	else
	{
		line = MIN(line, (NUM_MAXLINENO-iLineNum*2+1));
		ScrClrLine(line, line + 1);
	}

	memset(sTemp, '*', sizeof(sTemp));
	for(iCnt=1; iCnt<=3; iCnt++)
	{
		if( (iCnt*NUM_MAXZHCHARS)<=iLen )
		{
			iLeftBytes = NUM_MAXZHCHARS;

			if( ucMode & PASS_IN )
			{
				ScrPrint(0, line, CFONT, "%-.*s", iLeftBytes, sTemp);
			}
			else
			{
				if(ucMode & DISCNT_IN)
					ScrPrint(0, line, CFONT, "%d,%02d", atoi(pszWorkBuff) / 100, atoi(pszWorkBuff) % 100);
				else
					ScrPrintStr(0, line, CFONT, "%-.*s", iLeftBytes, pszWorkBuff + iLen - iCnt*NUM_MAXZHCHARS);
			}
		}
		else
		{
			iLeftBytes = iLen - (iCnt-1)*NUM_MAXZHCHARS;
			if( iLeftBytes<=0 && iCnt>1 )
			{
				break;
			}
			if( ucMode & PASS_IN )
			{
				ScrPrint(0, line, CFONT, "%-*.*s", NUM_MAXZHCHARS, iLeftBytes, sTemp);
			}
			else
			{
				if(ucMode & DISCNT_IN)
					ScrPrint(0, line, CFONT, "%d,%02d", atoi(pszWorkBuff) / 100, atoi(pszWorkBuff) % 100);
				else
					ScrPrintStr(0, line, CFONT, "%-*.*s", NUM_MAXZHCHARS, iLeftBytes, pszWorkBuff);
			}
			break;
		}
		//++line; // Removed by Kim_LinHB 2013-3-25 v4.55
	}
}


static void MenuInit(void)
{
	switch(gl_MaintenaceParam.ucLang)
	{
#ifdef FARSI
	case LANG_ARABIC:
		{
			int i;
			for (i = 0; i < MainMenuS.iNum; ++i)
			{
				MainMenu_Arabic[i].pstSubMenu = MainMenu[i].pstSubMenu;
			}
			PubSmInit(&OrderMenuS, OrderMenu_Arabic);
			PubSmInit(&OfflineMenuS, OfflineMenu_Arabic);     
			PubSmInit(&CashCollectionMenuS, CashCollectionMenu_Arabic);  
			PubSmInit(&BankDepositMenuS, BankDepositMenu_Arabic);
			PubSmInit(&InformationMenuS, InformationMenu_Arabic);
			PubSmInit(&ReportMenuS, ReportMenu_Arabic);
			PubSmInit(&SERVER_CALLMenuS, SERVER_CALLMenu_Arabic);
			PubSmInit(&ConsignmentMenuS, ConsignmentMenu_Arabic); // Added by Kim_LinHB 2013-10-28 v4.68
			PubSmInit(&MainMenuS, MainMenu_Arabic);
		}
		break;
#endif
	default:
		PubSmInit(&OrderMenuS, OrderMenu);
		PubSmInit(&OfflineMenuS, OfflineMenu);   
		PubSmInit(&CashCollectionMenuS, CashCollectionMenu);  
		PubSmInit(&BankDepositMenuS, BankDepositMenu);
		PubSmInit(&InformationMenuS, InformationMenu);
		PubSmInit(&ReportMenuS, ReportMenu);
		PubSmInit(&SERVER_CALLMenuS, SERVER_CALLMenu);
		PubSmInit(&ConsignmentMenuS, ConsignmentMenu); // Added by Kim_LinHB 2013-10-28 v4.68
		PubSmInit(&MainMenuS, MainMenu);
		break;
	}

	OrderMenuS.sMask[2] = SM_OFF;   //	Return Order

	OfflineMenuS.sMask[2] = SM_OFF; //	offline Cancel order   

	MainMenuS.sMask[2] = SM_OFF;	//	Cash Collection   
	MainMenuS.sMask[3] = SM_OFF;	//	Bank Deposit
	MainMenuS.sMask[1] = SM_OFF;	//	Offline   added by Allen 2016-09-20  Alper: disenable Offline menu.

	ConsignmentMenuS.sMask[3] = SM_OFF; // pending consignment order
	//SERVER_CALLMenuS.sMask[1] = SM_OFF; // Softw Download 
}
