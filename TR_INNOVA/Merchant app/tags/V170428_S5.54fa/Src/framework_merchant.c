#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "data_define_merchant.h"
#include "framework.h"
#include "transaction.h"
#include "calc_merchant.h"

#include "connect.h"
#include "Lang_EN.h"
#ifdef FARSI
#include "Lang_Arabic_Merchant.h"
#endif
#include "Arrow.h"

#include "printList_merchant.h"

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

static void ManagerSetting(void);
static void ManagerPrnSet(void);
static void MaintenaceParametres(void);
static void SetMaintenaceParametres(void);
static void SetConnParam(void);
static void SetPPPParam(void);
static void SetGPRSParam(void);
static void SetEthParam(void);
static void SetServerParam(void);

static void SetPlanParam(void);
static void SetPrinterParam(void);

static int prepare_screen_text_cpy(SMART_MENU *pstSmartMenu, int iStartIndex, int iHilightIndex, 
								   unsigned int uiMode, unsigned char ucWithDisp, int *piScrItemNum, int * itemList);
static int GetNextHiliIndex_cpy(SMART_MENU *pstSmartMenu, int iHilightIndex);
static int GetFirstHiliIndex_cpy(SMART_MENU *pstSmartMenu);
static int GetPrevHiliIndex_cpy(SMART_MENU *pstSmartMenu, int iHilightIndex);
static void MapChar2Idx(unsigned char ch, int *piRow, int *piCol);
static void ShowWBuff(unsigned char line, unsigned char ucMode, unsigned char *pszWorkBuff, int iLen);
static void SelConnectType(unsigned char ucType[2]);

static void GetCharFont(unsigned char ucCol, unsigned char ucChar, unsigned char *psFont);

// Added by Kim_LinHB 2013-7-8 v4.62
static void Upload_Stock();
//////////////////////////////////////////////////////////////////////////
MENU_ITEM PrintMenu[]= 
{
	{ "GENERAL STOCK", ID_PrintGeneral,	NULL },
	{ "STOCK DETAILED",ID_PrintDetail,	NULL },
	{ "",			 0,					NULL },
};
SMART_MENU PrintMenuS = {NULL, sizeof(PrintMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM StockMenu[]= 
{
	{ "Print Stock", ID_PrintStock,	&PrintMenuS },
	{ "Display Stock", ID_DispStock,NULL },
	{ "",			 0,				NULL },
};
SMART_MENU StockMenuS = {NULL, sizeof(StockMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM ReportsMenu[] = 
{
	{ "Cashier", ID_ReportCashier,	NULL },
	{ "Manager", ID_ReportManager,	NULL },
	//{ "Server", ID_ReportServer,	NULL },
	{ "",				  0,		NULL },
};
SMART_MENU ReportsMenuS = {NULL, sizeof(ReportsMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM TransmissionMenu[]=
{
    { "Setting Download",	ID_SettingsDownload,NULL},    
	{ "Sales Polling",		ID_SalesPolling,	NULL},
	{ "Cancel a sale",		ID_CancelSale,		NULL},
	{ "Claim",				ID_Claim,			NULL},
	{ "Get Mail",			ID_GetMail,			NULL},
	{ "Soft Download",		ID_SoftDownload,	NULL},
	{ "Logo Download",		ID_LogoDownload,	NULL},
	{ "Balance",			ID_Balance,			NULL},
//  { "Set Auto        ", ID_Config_Auto,		NULL },
//  { "First Init. Auto", ID_First_Init__Auto,	NULL },
	{ "",				  0,					NULL },
};
SMART_MENU TransmissionMenuS = {NULL, sizeof(TransmissionMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM CashierMenu[]=
{
	{ "Log in",			ID_Login,		NULL},
	{ "Log out",		ID_Logout,		NULL}, 
	//{ "Change Password",	ID_ChangePwd,	NULL},
	//{ "Create",			ID_CreateUser,	NULL},
	//{ "Delete",			ID_DelUser,		NULL},
	//{ "View List",		ID_ViewList,	NULL},
	{ "",				  0,			NULL },
};
SMART_MENU CashierMenuS = {NULL, sizeof(CashierMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM RestockMenu[]=
{
	{ "   RestockPin   ",		ID_RestockPin,		NULL},
	{ " Pending Order  ",		ID_PendingOrder,		NULL}, 
	{ "",				  0,			NULL },
};
SMART_MENU RestockMenuS = {NULL, sizeof(RestockMenu) / sizeof(MENU_ITEM),{SM_ON}};

MENU_ITEM MainMenu[]= 
{
	{ "   Sale Codes   ", ID_SaleCodes,			NULL },
	{ "   Duplicate    ", ID_Duplicate,			NULL },
	{ "   Cash Order   ", ID_CashOrder,			NULL },
	{ "  RestockPins   ", ID_RestockPins,		&RestockMenuS },
	{ "     Stocks     ", ID_Stocks,			&StockMenuS },
	{ " ON/OFF Service ", ID_ON_OFFService,		NULL },
	{ "    Reports     ", ID_Reports,			&ReportsMenuS },
	{ "Manager Settings", ID_ManagerSettings,	NULL },
	{ "  Maintenance   ", ID_Maintenance,		NULL },
	{ "  Transmission  ", ID_Transmission,		&TransmissionMenuS },
	{ "    Cashier     ", ID_Cashier,			&CashierMenuS },
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
	// Added by Kim_LinHB 2013-5-6 v4.58
	if(-1 != fexist("PARAM_Mer.dat"))
	{
		FileCpy(FILE_PARAM, "PARAM_Mer.dat");
		remove("PARAM_Mer.dat");
	}
	if(-1 != fexist("USERS_Mer.dat"))
	{
		FileCpy(FILE_USERS, "USERS_Mer.dat");
		remove("USERS_Mer.dat");
	}
	if(-1 != fexist("USERS_MerT.dat"))
	{
		FileCpy(FILE_USERS_TMP, "USERS_MerT.dat");
		remove("USERS_MerT.dat");
	}
	if(-1 != fexist("TRANS_tmp.dat"))
	{
		FileCpy(FILE_TRANS_OL, "TRANS_tmp.dat");
		remove("TRANS_tmp.dat");
	}
	if(-1 != fexist("STOCK_Mer.dat"))
	{
		FileCpy(FILE_STOCK_OLD1, "STOCK_Mer.dat");
		remove("STOCK_Mer.dat");
	}
	// Add End

	// Added by Kim_LinHB 2013-7-5 v4.61
//#ifndef _S_SERIES_
	if(-1 != fexist("PARAM_MER.DAT"))
	{
		FileCpy(FILE_PARAM, "PARAM_MER.DAT");
		remove("PARAM_MER.DAT");
	}
	if(-1 != fexist("STOCK_MER.DAT"))
	{
		FileCpy(FILE_STOCK_OLD1, "STOCK_MER.DAT");
		remove("STOCK_MER.DAT");
	}
	if(-1 != fexist("USERS_MER.DAT"))
	{
		FileCpy(FILE_USERS, "USERS_MER.DAT");
		remove("USERS_MER.DAT");
	}
	if(-1 != fexist("USERS_MERT.DAT"))
	{
		FileCpy(FILE_USERS_TMP, "USERS_MERT.DAT");
		remove("USERS_MERT.DAT");
	}
	if(-1 != fexist("TRANS_TMP.DAT"))
	{
		FileCpy(FILE_TRANS_OL, "TRANS_TMP.DAT");
		remove("TRANS_TMP.DAT");
	}
	if(-1 != fexist("REMOTE_BACKUP_M"))
	{
		remove("REMOTE_BACKUP_M");
	}
//#endif
	// Add End

	// Added by Kim_LinHB 2013-11-21 v4.73
	if(-1 != fexist("CASHORDER.DAT"))
	{
		int fd;
		unsigned int i;
		TRANS_DATA_CASH_OLD stTemp;
		TRANS_DATA_CASH	stCurr;
		long Len = filesize("CASHORDER.DAT");
		fd = open("CASHORDER.DAT", O_RDWR);

		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return 0xFF;
		}

		seek(fd, 0, SEEK_SET);

		while(Len > 0)
		{
			read(fd, (void *)&stTemp, sizeof(stTemp));
			memcpy(&stCurr, &stTemp, sizeof(stTemp));
			for (i = 0; i <stTemp.ucProductNum; ++i)
			{
				stCurr.stProducts[i].uiCodeCategory = stTemp.stProducts[i].ucCodeCategory;
				strcpy(stCurr.stProducts[i].szCodeProduct, stTemp.stProducts[i].szCodeProduct);
				strcpy(stCurr.stProducts[i].szQuantity, stTemp.stProducts[i].szQuantity);
			}
			SaveCashOrder(&stCurr);
			Len -= sizeof(stTemp);
		}
		close(fd);
		remove("CASHORDER.DAT");
	}
	// Add End

	if(-1 != fexist(FILE_PARAM))
	{
		return 0;
	}
	else
	{
		Beep();
		return 1;
	}
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
//#ifndef ForAlperTest // Removed by Kim_LinHB 2013-3-23 v4.56
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
		ScrPrint(14,DISPLAYLINE + 2,CFONT | REVER, ":CODE");
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
			DoTrans(ID_SaleCodes);
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
		// Added by Kim_LinHB 2014-5-15
		// Modified by Kim_LinHB 2014/10/28 || to &&
#if defined(LHB_TEST) && defined(LHB_DEBUG)
		else if(ucKey == KEY1){
			// Added by Kim_LinHB 2014-5-20
			ScrCls();
			if(strlen(gl_MaintenaceParam.szLastSoftDownLoad_Time) > 0)
			{
				unsigned char szLastSoftDownLoad_Time[SIZE_DATE_TIME + 1 + 2];
				unsigned char szLastSoftDownLoad_Time2[SIZE_DATE_TIME + 1 + 2];

				ScrPrint(0,0,1,"Last Time\n%s", gl_MaintenaceParam.szLastSoftDownLoad_Time);

				sprintf(szLastSoftDownLoad_Time, "%s%s", PRE_YEAR, gl_MaintenaceParam.szLastSoftDownLoad_Time);
				PubCalDateTime(szLastSoftDownLoad_Time, szLastSoftDownLoad_Time2, 24, "hh");
				ScrPrint(0,4,1,"Next Time\n%s",szLastSoftDownLoad_Time2 + 2);
			}
			PubWaitKey(10);
		}
#endif
	}
}

void DispVersion()
{
	// Modified by Kim_LinHB 2014-5-20
	ScrCls();
	ScrPrint(0,0,0,"SOFTWARE VERSION\n [%s]",STR_VERSION);
	ScrPrint(0,3,0,"LANG [%s]",gl_pLang[gl_MaintenaceParam.ucLang]);
	
	// Modified by Kim_LinHB 2013-11-14 4.73
	if(strlen(gl_MaintenaceParam.szLastSalesPolling_Time) > 0)
	{
		unsigned char szLastSalesPolling_Time[SIZE_DATE_TIME + 1 + 2];
		unsigned char szLastSalesPolling_Time2[SIZE_DATE_TIME + 1 + 2];
		int iDuration = atoi(gl_MaintenaceParam_Ext.szDuration);
		sprintf(szLastSalesPolling_Time, "%s%s", PRE_YEAR, gl_MaintenaceParam.szLastSalesPolling_Time);
		PubCalDateTime(szLastSalesPolling_Time, szLastSalesPolling_Time2, iDuration, "mm");
		ScrPrint(0,5,0,"next polling:\n%2.2s-%2.2s %2.2s:%2.2s",
				szLastSalesPolling_Time2 + 4,
				szLastSalesPolling_Time2 + 6,
				szLastSalesPolling_Time2 + 8,
				szLastSalesPolling_Time2 + 10);
	}
	PubWaitKey(TIME_DELAY);
}

void DispMainMenu(void)
{
	int iRet = -1;
	unsigned char ucKey = 0;

#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,gl_MaintenaceParam.stManage.szPwd=%s,gl_MaintenaceParam.szMaintenancePwd=%s",
			__FILE__,__LINE__,__FUNCTION__,gl_MaintenaceParam.stManage.szPwd,gl_MaintenaceParam.szMaintenancePwd);
#endif
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

		if ((0 == gl_SavParam.ucCategoryNum && 
			iRet != ID_Maintenance &&
			iRet != ID_ManagerSettings &&
			iRet != ID_SettingsDownload &&
			iRet != ID_SoftDownload &&
			ucKey != KEYMENU))
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
			else if (ID_ManagerSettings == iRet)
				ManagerSetting();
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

void DispStock(unsigned short index)
{
	unsigned char szBufferDisplay[16+1];

	if(0 == gl_SavParam.ucProductsNum)
		return;
	ScrCls();
	ScrSetIcon(ICON_UP, OPENICON);
	ScrSetIcon(ICON_DOWN, OPENICON);
	if(0 == index)
		ScrSetIcon(ICON_UP, CLOSEICON);

	if(0 == gl_SavParam.ucProductsNum)
		ScrSetIcon(ICON_DOWN, CLOSEICON);
	if(gl_SavParam.ucProductsNum - 1== index)
		ScrSetIcon(ICON_DOWN, CLOSEICON);


	DispString(gl_SavParam.stSavParamProducts[index].szNameProduct, 1 | DISP_LINE_LEFT);

// 	if(gl_SavParam.stSavParamProducts[index].usNbMaxRestock != 0)
// 	{
// 		sprintf(szBufferDisplay,"%s:%d %s:%d",gl_pLangSelPiont[STR_DISP],
// 			FindCodesCnt(CODE_RESTOCK_FREE, gl_SavParam.stSavParamProducts[index].szCodeProduct),
// 			gl_pLangSelPiont[STR_VEND],FindCodesCnt(CODE_RESTOCK_SOLD, NULL));//Avail. :12 
// 	}
// 	else
	{
		sprintf(szBufferDisplay,"%s(%d-%d)", gl_pLangSelPiont[STR_ON_LINE], 
			FindCodesCnt(CODE_RESTOCK_FREE, gl_SavParam.stSavParamProducts[index].szCodeProduct),
			FindCodesCnt(CODE_RESTOCK_SOLD, gl_SavParam.stSavParamProducts[index].szCodeProduct) +
			FindCodesCnt(CODE_RESTOCK_SALESPOLLING, gl_SavParam.stSavParamProducts[index].szCodeProduct));
	}

	DispString(szBufferDisplay, DISPLAYLINE | DISP_LINE_LEFT);
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

void DispSending(void)
{
	ScrCls();
	DispString(gl_pLangSelPiont[STR_SENDING], 1 | DISP_LINE_CENTER);
	DispString(". . . . . .", DISPLAYLINE | DISP_LINE_CENTER);
}
void DispReceiving(void)
{
	ScrCls();
	DispString(gl_pLangSelPiont[STR_RECEIVING], 1 | DISP_LINE_CENTER);
	DispString(". . . . . .", DISPLAYLINE | DISP_LINE_CENTER);
}

void GetCurTime(unsigned char *stCurTime)
{
	unsigned char timeBCD[SIZE_DATE_TIME / 2 + 1];
	GetTime(timeBCD);
	PubBcd2Asc0(timeBCD, 6, stCurTime);	// YYMMDDHHMMSS
}

unsigned char ParamNotSpecified(void)
{
	if(strlen(gl_MaintenaceParam.szTermID) > 0 &&
		CheckIP(gl_MaintenaceParam.stConnParam.szRemoteIP) &&
		atoi(gl_MaintenaceParam.stConnParam.szRemotePort) <= 65535)
	{
		return FALSE;
	}
	else
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_PARAMETRES_APPLI], 1 | DISP_LINE_CENTER);
		DispString(gl_pLangSelPiont[STR_INCOMPLETS], DISPLAYLINE | DISP_LINE_CENTER);
		Beep_Speaker(BEEP_ERR);
		PubWaitKey(TIME_DELAY); //2 sec
	}
	return TRUE;
}


void Awake_Plan()
{
	unsigned char stCurTime[SIZE_DATE_TIME + 2 + 1];
	unsigned char stLastSP[SIZE_DATE_TIME + 2 + 1];
	unsigned char stLastSD[SIZE_DATE_TIME + 2 + 1];
	long lOffset = 0;
	unsigned char bNegative = 0;
	int iDuration = atoi(gl_MaintenaceParam_Ext.szDuration);

	strcpy(stCurTime, PRE_YEAR);
	GetCurTime(stCurTime + 2);

	gl_bPlanned_SettDownload = FALSE;
	// Modified by Kim_LinHB 2013-3-20 v4.56 Remove did_settDownLoad
	if(0 == strlen(gl_MaintenaceParam.szLastSettDownLoad_Time))
	{
		GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time);
		// Modified by Kim_LinHB 2014-2-10 v4.84
		AdjustDate(gl_MaintenaceParam.szLastSettDownLoad_Time, -2);
		memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, gl_MaintenaceParam.szTimeSettDownload, 4);
		WriteParam(WRT_MAINTA);
	}

	sprintf(stLastSD, "%s%s", PRE_YEAR, gl_MaintenaceParam.szLastSettDownLoad_Time);
	lOffset = PubCalInterval(stLastSD, stCurTime, "hh", &bNegative);
	if(!bNegative && lOffset >= 24)
	{
		gl_bPlanned_SettDownload = TRUE;
		DoTrans(ID_SettingsDownload);

		GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time);
		memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, gl_MaintenaceParam.szTimeSettDownload, 4);
		if(memcmp(stCurTime + 8, gl_MaintenaceParam.szTimeSettDownload, 4) < 0)
		{
			// Modified by Kim_LinHB 2014-2-10 v4.84
			// case gl_MaintenaceParam.szTimeSettDownload was updated during this setting download
			AdjustDate(gl_MaintenaceParam.szLastSettDownLoad_Time, -1);
		}
		WriteParam(WRT_MAINTA);
		gl_bPlanned_SettDownload = FALSE;
	}
	// Modify End
	
	gl_bPlanned_SalesPolling = FALSE;
	// Modified by Kim_LinHB 2013-11-14	v4.73
	if(iDuration > 0 && strlen(gl_MaintenaceParam.szLastSalesPolling_Time) > 0)
	{
		sprintf(stLastSP, "%s%s", PRE_YEAR, gl_MaintenaceParam.szLastSalesPolling_Time);
		lOffset = PubCalInterval(stLastSP, stCurTime, "mm", &bNegative);
		if(!bNegative && lOffset >= iDuration)
		{
			gl_bPlanned_SalesPolling = TRUE;
			PubCalDateTime(stLastSP, stCurTime, lOffset / iDuration * iDuration, "mm");
			strcpy(gl_MaintenaceParam.szLastSalesPolling_Time, stCurTime + 2);
			WriteParam(WRT_MAINTA);
		}
	}

	// Modify End
	
	if(gl_bPlanned_SalesPolling)
	{
		if (!gl_MaintenaceParam.bReportEmpty &&
			gl_MaintenaceParam.bAutoPrintReports)
		{
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PRINT], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_ON_GOING], DISPLAYLINE | DISP_LINE_CENTER);
			// print merchant report
			Prn_ReportCommer();

// 			if(gl_MaintenaceParam.bPrintReportSale)
// 			{
// 				Prn_ReportCashier();  // current  cashier or all?
// 			}

			if(gl_MaintenaceParam.bPrintReportDaily)
			{						
				Prn_SalePolling(CODE_FIN_DAILY);				
			}

			Erase_Report(TRUE); // Removed by Kim_LinHB 2013-3-27 v4.56
		}

		//if(gl_MaintenaceParam.bFullAuto)
		//	DoTrans(ID_RestockPins);
		//else
			DoTrans(ID_SalesPolling);
		gl_bPlanned_SalesPolling = FALSE;
	}
	// Modified End
}

void UpdateApp(void)
{
	unsigned char stCurTime[SIZE_DATE_TIME + 2 +1];
	unsigned char stLastSD[SIZE_DATE_TIME + 2 + 1];
	long lOffset = 0;
	unsigned char bNegative = 0;

	strcpy(stCurTime, PRE_YEAR);
	GetCurTime(stCurTime + 2);

	gl_bPlanned_SettDownload = FALSE;
	// Modified by Kim_LinHB 2014-2-6 v4.84
	if(0 == gl_SavParam.bIsSetup)
	{
		if(0 == strlen(gl_MaintenaceParam.szLastSoftDownLoad_Time) && 
			strlen(gl_MaintenaceParam.stFTP.szTimeSoftDownLoad) > 0)
		{
			GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
			// Modified by Kim_LinHB 2014-2-10 v4.84
			AdjustDate(gl_MaintenaceParam.szLastSoftDownLoad_Time, -1);
			memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
			WriteParam(WRT_MAINTA);
		}
		sprintf(stLastSD, "%s%10.10s00", PRE_YEAR, gl_MaintenaceParam.szLastSoftDownLoad_Time);
		lOffset = PubCalInterval(stLastSD, stCurTime, "hh", &bNegative);

		if(!bNegative && lOffset >= 24)
		{
			gl_bPlanned_SoftDownload = TRUE;
			DoTrans(ID_SoftDownload);
			// case soft download failed
			GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
			memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
// 			if(memcmp(stCurTime + 8, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4) < 0)
// 				PubAscDec(gl_MaintenaceParam.szLastSoftDownLoad_Time, 6);
			WriteParam(WRT_MAINTA);
			gl_bPlanned_SoftDownload = FALSE;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//				the following is static func
//////////////////////////////////////////////////////////////////////////
static void ManagerSetting(void)
{
	enum
	{
		STEP_START = 0,
		STEP_CHGPWD_M,
		STEP_CHGPWD_C,
		STEP_CREATE,
		STEP_DEL,
		STEP_VIEWLIST,
		STEP_PRNSET,
		STEP_CANCELSALE,
	//	STEP_STD,
		STEP_PRINT,
		STEP_END,
	};
	unsigned char ucKey;
	unsigned char ucSel = STEP_START + 1;
	unsigned char szPwd[SIZE_MAX_PWD + 1];
	char cLoop = TIME_RETRY;
	memset(&szPwd, 0, sizeof(szPwd));

	while(cLoop > 0)
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], 1 | DISP_LINE_LEFT);
		ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwd, TIME_OUT_STANDARD_30S, TRUE);

		if(ucKey != KEYENTER)
			return;

		if(strcmp(szPwd, gl_MaintenaceParam.stManage.szPwd))
		{
			--cLoop;
			ScrCls();
			DispString("Wrong Pwd", DISP_SCR_CENTER);
			Beep_Speaker(BEEP_ERR);
			PubWaitKey(TIME_DELAY);
			if(cLoop <= 0)
				return;
		}
		else
			break;
	}

	while(1)
	{
		ScrCls();
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
		case STEP_CHGPWD_M:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_MODIFICATION], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PASSWORD_MANAGER], DISPLAYLINE | DISP_LINE_CENTER);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				USER_INFO stByWho, stWho;
				unsigned char szPwdNew[SIZE_MAX_PWD + 1];
				unsigned char szPwdNew2[SIZE_MAX_PWD + 1];

				RESPONSE_HEADER_TAG stRespH;
				RESPONSE_BODY_TAG stRespB;
				memset(&stRespH, 0, sizeof(RESPONSE_HEADER_TAG));
				memset(&stRespB, 0, sizeof(RESPONSE_BODY_TAG));

				memset(&stByWho, 0, sizeof(USER_INFO));
				memset(&stWho, 0, sizeof(USER_INFO));
				memset(szPwdNew, 0, sizeof(szPwdNew));
				memset(szPwdNew2, 0, sizeof(szPwdNew2));

				ScrCls();
				DispString("New Pwd:", 1 | DISP_LINE_LEFT);
				ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew, TIME_OUT_STANDARD_30S, TRUE);
				
				if(KEYENTER == ucKey)
				{
					ScrCls();
					DispString("New Pwd again:", 1 | DISP_LINE_LEFT);
					ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, szPwdNew2, TIME_OUT_STANDARD_30S, TRUE);
				}

				if (KEYENTER == ucKey && 0 == strcmp(szPwdNew, szPwdNew2))
				{
					int iRetryCnt = TIME_RETRY;
					unsigned char ucRet;
					strcpy(stByWho.szName, gl_MaintenaceParam.stManage.szName);
					strcpy(stByWho.szPwd, gl_MaintenaceParam.stManage.szPwd);
					strcpy(stWho.szName, gl_MaintenaceParam.stManage.szName);
					strcpy(stWho.szPwd, szPwdNew);
					do 
					{
						ucRet = Change_Pwd_XML(stByWho, stWho, &stRespH, &stRespB, TRUE);
						--iRetryCnt;
					} while (1 == ucRet && iRetryCnt > 0);


					ScrCls();
					if(OK == ucRet)
					{
						strcpy(gl_MaintenaceParam.stManage.szPwd, stWho.szPwd);
						WriteParam(WRT_MAINTA);
						DispString("Success.", DISP_SCR_CENTER);
					}
					else
						Do_Error(ucRet, stRespH);
				}
				else
				{
					ScrCls();
					DispString("Don't Match.", DISP_SCR_CENTER);
					Beep_Speaker(BEEP_ERR);
				}
				PubWaitKey(TIME_DELAY);
			}
			break;
		case STEP_CHGPWD_C:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Change PWD", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				DoTrans(ID_ChangePwd);
			}
			break;
		case STEP_CREATE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Create New Cashier", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				DoTrans(ID_CreateUser);
			}
			break;
		case STEP_DEL:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Del Cashier", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				DoTrans(ID_DelUser);
			}
			break;
		case STEP_VIEWLIST:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("View Cashier List", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				DoTrans(ID_ViewList);
			}
			break;
		case STEP_PRNSET:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_MENU_PRINT], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				ucKey = 0;
				ManagerPrnSet();
			}
			break;
		case STEP_CANCELSALE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_ANNUL_VENTE], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bCancelSale,TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
// 		case STEP_STD:
// 			ScrSetIcon(ICON_UP, OPENICON);
// 			ScrSetIcon(ICON_DOWN, OPENICON);
// 	
// 			break;
		case STEP_PRINT:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_PRINT_PARAM], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_CENTER);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			break;
		}
		if(0xFF == ucKey || KEYCANCEL == ucKey)
		{
			ParamNotSpecified();
			return;
		}
		else if(KEYUP ==ucKey)
		{
			ScrSetIcon(ICON_DOWN, OPENICON);
			if (STEP_START + 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				--ucSel;
		}
		else if(KEYDOWN == ucKey)
		{
			ScrSetIcon(ICON_UP, OPENICON);
			if (STEP_END - 1 == ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				++ucSel;
		}
		else if(KEYENTER == ucKey)
		{
			if(STEP_PRINT == ucSel)
			{
				ScrCls();
				DispString(gl_pLangSelPiont[STR_PRINT_PARAM], 1 | DISP_LINE_LEFT);
				DispString(gl_pLangSelPiont[STR_ON_GOING], DISPLAYLINE | DISP_LINE_CENTER);
				Prn_ManagerSetting();
			}
			WriteParam(WRT_MAINTA);
		}
	}
}

static void ManagerPrnSet(void)
{
	enum
	{
		STEP_START = 0,
		//STEP_COMMAND,
		STEP_MERCHANTCOPY,
		STEP_DETAIL,
		STEP_AUTO_REP,
		STEP_DAILY,
		//STEP_SALE_REP,
		STEP_DOUBLE,
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
// 		case STEP_COMMAND:
// 			ScrSetIcon(ICON_UP, CLOSEICON);
// 			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_PRINT_COMANDE], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
// 				&gl_MaintenaceParam.bPrintCommande,TIME_OUT_STANDARD_30S);
// 			break;
		case STEP_MERCHANTCOPY:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("MERCHANT COPY:", 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bPrintMerchantCopy,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_DETAIL:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_IMP_DETAIL_RECH], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bPrintDetailRestock,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_AUTO_REP:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_IMP_REPORT_AUTO], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bAutoPrintReports,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_DAILY:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_IMP_RAPP_DAILY], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bPrintReportDaily,TIME_OUT_STANDARD_30S, TRUE);
			break;
// 		case STEP_SALE_REP:
// 			ScrSetIcon(ICON_UP, OPENICON);
// 			ScrSetIcon(ICON_DOWN, OPENICON);
// 			DispString(gl_pLangSelPiont[STR_IMP_RAPP_VENTE], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
// 				&gl_MaintenaceParam.bPrintReportSale,TIME_OUT_STANDARD_30S);
// 			break;
		case STEP_DOUBLE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_DOUBLEPAPER], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bDoublePaper,TIME_OUT_STANDARD_30S, TRUE);
			break;
		}

		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
		else if(KEYENTER == ucKey)
		{
			WriteParam(WRT_MAINTA);
		}
	}
}


static void MaintenaceParametres(void)
{
	unsigned char ps[SIZE_MAX_PWD + 1];
	unsigned char ucKey;
	char cLoop = TIME_RETRY;
	
	while(cLoop > 0)
	{
		ScrCls();
		DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
		do 
		{
			ucKey = GetInput(DISPLAYLINE, NUM_IN | PASS_IN, SIZE_MIN_PWD, SIZE_MAX_PWD, ps, TIME_OUT_STANDARD_30S, FALSE);
			if(0xFF == ucKey || KEYCANCEL == ucKey)
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
		--cLoop;
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
		STEP_TERMID,
		STEP_CONNMENU,
		STEP_PLANMENU,
		STEP_PRINTMENU,
		STEP_UPLOAD_STOCK,
		STEP_FULLAUTO,
		STEP_MANAGERSETT,
		STEP_CLAIM,
		STEP_OFFLINE,
		STEP_SOFTMASTER,
		STEP_PRNTYPE,
		STEP_LOGLV,
		STEP_RESET,
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
		case STEP_TERMID:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_ID_TERMINAL], 1 | DISP_LINE_LEFT);

			// Modified by Kim_LinHB 2013-3-25 v4.56
			// from NUM_IN to ALPHA_IN,  From SIZE_MAX_ID_TERMINAL to SIZE_MAX_CODE_MANAGER
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_CODE_MANAGER, 
				gl_MaintenaceParam.szTermID,TIME_OUT_STANDARD_30S, TRUE);
			// Modify End
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
		case STEP_PLANMENU:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("PLANNING MENU", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				ucKey = 0;
				SetPlanParam();
			}
			break;
		case STEP_PRINTMENU:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("PRINTING MENU", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_RIGHT);
			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				ucKey = 0;
				SetPrinterParam();
			}
			break;
		case STEP_UPLOAD_STOCK:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("UPLOAD STOCK?", 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_CANCEL], DISPLAYLINE | DISP_LINE_CENTER);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
				Upload_Stock();
			break;
		case STEP_FULLAUTO:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("FULL AUTO :", 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bFullAuto,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_MANAGERSETT:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("MANAGER SETTINGS", 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bManagerSett,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_CLAIM:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Claim", 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bClaim,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_OFFLINE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			ScrPrint(0,1,CFONT, "Cur Mode: %d", gl_MaintenaceParam.ucMerchantMode + 1);
			DispString("1.ONLINE ", 4 | DISP_LINE_CENTER);
			DispString("2.OFFLINE 3.MIX", 6 | DISP_LINE_CENTER);
			do
			{
				ucKey = PubWaitKey(TIME_OUT_STANDARD_30S);
			}while( KEY1 != ucKey && KEY2 != ucKey && KEY3 != ucKey && 
					KEYUP != ucKey && KEYDOWN != ucKey && KEYCANCEL != ucKey && 
					NOKEY != ucKey);
			if( KEY1 == ucKey ||
				KEY2 == ucKey ||
				KEY3 == ucKey)
			{
				gl_MaintenaceParam.ucMerchantMode = ucKey - KEY1;
				WriteParam(WRT_MAINTA);
				ScrClrLine(4,7);
				DispString(gl_pLangSelPiont[STR_PARAM_ENREGISTRE], 4 | DISP_LINE_LEFT);
				DelayMs(1000);
			}
			break;
		case STEP_SOFTMASTER:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("SOFT MASTER", 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bWelcomePage,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_PRNTYPE:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString("Printer Type", 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 1, 1, 
				gl_MaintenaceParam.szPrintertype,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_LOGLV:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_INFOS_TRACE], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN, 1, 1, 
				gl_MaintenaceParam.szLogLv,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_RESET:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_REINIT_APPLI], 1 | DISP_LINE_LEFT);
			DispString(gl_pLangSelPiont[STR_VAL_POUR_ENTRER], DISPLAYLINE | DISP_LINE_CENTER);

			ucKey = YesNo(TIME_OUT_STANDARD_30S);
			if(KEYENTER == ucKey)
			{
				gl_bFirstRun = 1;
				DataInit();
				ScrCls();

				DispString("Reset Ok", DISP_SCR_CENTER);
				Beep_Speaker(BEEP_OK);
				PubWaitKey(TIME_DELAY);
			}
			break;
		}
		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
			WriteParam(WRT_MAINTA);
		}
	}
}

static void SetConnParam()
{
	enum
	{
		STEP_START = 0,
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
		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
				gl_MaintenaceParam.stConnParam.szNumPad,TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PAD2:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_NUMERO_PAD_SECOURS], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN,0, SIZE_MAX_NUM_PAD, 
				gl_MaintenaceParam.stConnParam.szNumPadSec,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.szRemoteIP,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.szRemotePort,TIME_OUT_STANDARD_30S, TRUE);
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
			DispString("Secondary host:", 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[1])
				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
				stTmp.szRemoteIP_Bak,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.szRemotePort_Bak,TIME_OUT_STANDARD_30S, TRUE);
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
				gl_MaintenaceParam.stConnParam.stParamGPRS.szUID,TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PWD:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | PASS_IN,0, MAX_PWD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szPwd,TIME_OUT_STANDARD_30S, TRUE);
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
				&gl_MaintenaceParam.stConnParam.stParamGPRS.ucAuth,TIME_OUT_STANDARD_30S, TRUE);
			strcpy((char *)&sg_StrYesNo[0], gl_pLangSelPiont[STR_NON]);
			strcpy((char *)&sg_StrYesNo[1], gl_pLangSelPiont[STR_OUI]);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		}
		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
	}
}

static void SetGPRSParam()
{
	enum
	{
		STEP_START = 0,
		STEP_APN,
		STEP_UID,
		STEP_PWD,
		STEP_PIN,
		//STEP_RESET,
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
		case STEP_APN:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_APN], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, SIZE_MAX_NUM_PAD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN,TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_UID:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_USERNAME], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, ALPHA_IN | CARRY_IN,0, MAX_USER, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szUID,TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PWD:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_PASSWORD], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | PASS_IN,0, MAX_PWD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szPwd,TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
		case STEP_PIN:
			ScrSetIcon(ICON_UP, OPENICON);
			//ScrSetIcon(ICON_DOWN, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_PASSWORD_PIN], 1 | DISP_LINE_LEFT);
			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | PASS_IN,0, MAX_PWD, 
				gl_MaintenaceParam.stConnParam.stParamGPRS.szSimPin,TIME_OUT_STANDARD_30S, TRUE);
			if(KEYENTER == ucKey)
				WriteParam(WRT_MAINTA);
			break;
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
		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
	}
}

static void SetEthParam()
{
	enum
	{
		STEP_START = 0,
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
		kbflush();
		ScrSetIcon(ICON_UP, CLOSEICON);
		ScrSetIcon(ICON_DOWN, CLOSEICON);
		switch(ucSel)
		{
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
				&gl_MaintenaceParam.stConnParam.stParamEth.bDHCP,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.stParamEth.szLocalIP,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.stParamEth.szNetMask,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.stParamEth.szGatewayIP,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.stParamEth.szDNS,TIME_OUT_STANDARD_30S, TRUE);
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
				stTmp.stParamEth.szDNS_Bak,TIME_OUT_STANDARD_30S, TRUE);
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
		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
	}
}

static void SetPlanParam(void)
{
	enum
	{
		STEP_START = 0,
		//STEP_TIME_SALESPOLLING,
		STEP_TIME_SETTINGSDOWNLOAD,
		STEP_TIME_SOFTWLOAD,
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
		// Removed by Kim_LinHB 2013-11-14 v4.73
// 		case STEP_TIME_SALESPOLLING:
// 			if (gl_MaintenaceParam.ucMerchantMode != ONLINE)
// 			{
// 				unsigned char szTimeSalesPolling[4 + 1];
// 				strcpy(szTimeSalesPolling, gl_MaintenaceParam.szTimeSalesPolling);
// 				ScrSetIcon(ICON_UP, CLOSEICON);
// 				ScrSetIcon(ICON_DOWN, OPENICON);
// 				DispString(gl_pLangSelPiont[STR_TIME_SALESPOLLING], 1 | DISP_LINE_LEFT);
// 				ucKey = GetInputTime(DISPLAYLINE, szTimeSalesPolling, TIME_GET_TIME, TIME_OUT_STANDARD_30S);
// 				if (KEYENTER == ucKey)
// 				{
// 					unsigned char szCur[SIZE_DATE_TIME + 1];
// 					GetCurTime(szCur);
// 					if(memcmp(szCur + 6, szTimeSalesPolling, 4) < 0)
// 					{
// 						unsigned char szLast[SIZE_DATE_TIME + 1 + 2];
// 						unsigned long timeL;
// 						sprintf(szLast, "%s%s", PRE_YEAR, szCur);
// 						timeL = PubTime2Long(szLast);
// 						timeL -= (60 * 60 * 24);
// 						PubLong2Time(timeL, szLast);
// 						memcpy(gl_MaintenaceParam.szLastSalesPolling_Time, szLast + 2, 6);
// 					}
// 					else
// 						memcpy(gl_MaintenaceParam.szLastSalesPolling_Time, szCur, 6);
// 					memcpy(&gl_MaintenaceParam.szLastSalesPolling_Time[6], szTimeSalesPolling, 4);
// 					strcpy(gl_MaintenaceParam.szTimeSalesPolling, szTimeSalesPolling);
// 				}
// 			}
// 			else
// 				ucKey = KEYDOWN;
// 			break;
		case STEP_TIME_SETTINGSDOWNLOAD:
			{
				unsigned char szTimeSettingDL[4 + 1];
				strcpy(szTimeSettingDL, gl_MaintenaceParam.szTimeSettDownload);
				if(gl_MaintenaceParam.ucMerchantMode != ONLINE)
					ScrSetIcon(ICON_UP, OPENICON);
				else
					ScrSetIcon(ICON_UP, CLOSEICON);
				ScrSetIcon(ICON_DOWN, OPENICON);

				DispString(gl_pLangSelPiont[STR_TIME_SETTINGSDOWNLOAD], 1 | DISP_LINE_LEFT);
				ucKey = GetInputTime(DISPLAYLINE, szTimeSettingDL, TIME_GET_TIME, TIME_OUT_STANDARD_30S);
				if (KEYENTER == ucKey)	
				{
					// Modified by Kim_LinHB 2013-11-26 v4.74
					GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time);

					strcpy(gl_MaintenaceParam.szTimeSettDownload, szTimeSettingDL);
					if(memcmp(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, szTimeSettingDL, 4) <= 0)
					{
						// Modified by Kim_LinHB 2014-2-10 v4.84
						AdjustDate(gl_MaintenaceParam.szLastSettDownLoad_Time, -1);
					}
					memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, szTimeSettingDL, 4);
				}
				// Modify End
			}
			break;
#ifndef XENTA_MODUL
		case STEP_TIME_SOFTWLOAD:
			{
				unsigned char szTimeSoftwDL[4 + 1];
				strcpy(szTimeSoftwDL, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad);
				ScrSetIcon(ICON_UP, OPENICON);
				ScrSetIcon(ICON_DOWN, CLOSEICON);
				DispString(gl_pLangSelPiont[STR_TIME_SOFTWAREDOWNLOAD], 1 | DISP_LINE_LEFT);
				ucKey = GetInputTime(DISPLAYLINE, szTimeSoftwDL, TIME_GET_TIME, TIME_OUT_STANDARD_30S);
				if (KEYENTER == ucKey)
				{
					// Modified by Kim_LinHB 2013-11-26 v4.74
					GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);

					strcpy(gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, szTimeSoftwDL);
					if(memcmp(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, szTimeSoftwDL, 4) <= 0)
					{	
						// Modified by Kim_LinHB 2014-2-10 v4.84
						AdjustDate(gl_MaintenaceParam.szLastSoftDownLoad_Time, -1);
					}
					memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, szTimeSoftwDL, 4);
				}
			}
			break;
#endif	
		}
		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
			if (STEP_END - 1== ucSel)
				Beep_Speaker(BEEP_ERR);
			else
				++ucSel;
		}
		else if(KEYENTER == ucKey)
		{
			WriteParam(WRT_MAINTA);
		}
	}
}
static void SetPrinterParam(void)
{
	enum
	{
		STEP_START = 0,
		STEP_CODE_BARRE,
		STEP_LOGO,
		//STEP_NAMESOFT,
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
		case STEP_CODE_BARRE:
			ScrSetIcon(ICON_UP, CLOSEICON);
			ScrSetIcon(ICON_DOWN, OPENICON);
			DispString(gl_pLangSelPiont[STR_PRINT_CODE_BARRE], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bPrintBarCode,TIME_OUT_STANDARD_30S, TRUE);
			break;
		case STEP_LOGO:
			ScrSetIcon(ICON_UP, OPENICON);
			ScrSetIcon(ICON_DOWN, CLOSEICON);
			DispString(gl_pLangSelPiont[STR_PRINT_LOGO], 1 | DISP_LINE_LEFT);

			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
				&gl_MaintenaceParam.bPrintLogo,TIME_OUT_STANDARD_30S, TRUE);
			break;
// 		case STEP_NAMESOFT:
//			ScrSetIcon(ICON_UP, OPENICON);
//			ScrSetIcon(ICON_DOWN, CLOSEICON);
// 			DispString(gl_pLangSelPiont[STR_IMP_NOM_APPLI], 1 | DISP_LINE_LEFT);
// 
// 			ucKey = GetInput(DISPLAYLINE, YESNO_IN,1, 1, 
// 				&gl_MaintenaceParam.bPrintBarCode,TIME_OUT_STANDARD_30S);
// 			break;
		}

		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
		else if(KEYENTER == ucKey)
		{
			WriteParam(WRT_MAINTA);
		}
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
 				stTmp.szRemoteIP,TIME_OUT_STANDARD_30S, TRUE);
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
 				stTmp.szRemotePort,TIME_OUT_STANDARD_30S, TRUE);
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
			DispString("Secondary host:", 1 | DISP_LINE_LEFT);
			if(FALSE == bIPOK[1])
 				DispString(gl_pLangSelPiont[STR_INVALID_ENTRY], DISPLAYLINE + 2 | DISP_LINE_RIGHT);
			
 			ucKey = GetInput(DISPLAYLINE, NUM_IN | CARRY_IN | POINT_IN, 0, MAX_IP, 
 				stTmp.szRemoteIP_Bak,TIME_OUT_STANDARD_30S, TRUE);
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
 				stTmp.szRemotePort_Bak,TIME_OUT_STANDARD_30S, TRUE);
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
		if(0xFF == ucKey || KEYCANCEL == ucKey)
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
unsigned char GetInput(unsigned char line, unsigned char ucMode, unsigned char ucMinlen, unsigned char ucMaxlen, \
			   unsigned char *pszOut, unsigned char ucTimeOut,unsigned char bWithTip)
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
			return ucKey;
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
				if(bWithTip)
				{
					DispString(gl_pLangSelPiont[STR_PARAM_ENREGISTRE], line | DISP_LINE_LEFT);
					DelayMs(1000);
					ScrClrLine(DISPLAYLINE,DISPLAYLINE);
				}
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
		else if(KEYUP == ucKey || KEYDOWN == ucKey) // because of the Planning Menu
			return ucKey;
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
		//++line; // Removed by Kim_LinHB 2013-3-25 v4.56
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
			// Added by Kim_LinHB 2013-5-3 v4.58
			for (i = 0; i < StockMenuS.iNum; ++i)
			{
				StockMenu_Arabic[i].pstSubMenu = StockMenu[i].pstSubMenu;
			}
			// Add End
			PubSmInit(&PrintMenuS, PrintMenu_Arabic);
			PubSmInit(&StockMenuS, StockMenu_Arabic);
			PubSmInit(&ReportsMenuS, ReportsMenu_Arabic);
			PubSmInit(&TransmissionMenuS, TransmissionMenu_Arabic);
			PubSmInit(&CashierMenuS, CashierMenu_Arabic);
			PubSmInit(&RestockMenuS, RestockMenu_Arabic); // Added by Kim_LinHB 2013-12-4 v4.76
			PubSmInit(&MainMenuS, MainMenu_Arabic);
		}
		break;
#endif
	default:
		PubSmInit(&PrintMenuS, PrintMenu);
		PubSmInit(&StockMenuS, StockMenu);
		PubSmInit(&ReportsMenuS, ReportsMenu);
		PubSmInit(&TransmissionMenuS, TransmissionMenu);
		PubSmInit(&CashierMenuS, CashierMenu);
		PubSmInit(&RestockMenuS, RestockMenu); // Added by Kim_LinHB 2013-12-4 v4.76
		PubSmInit(&MainMenuS, MainMenu);
		break;
	}
	MainMenuS.sMask[2] = SM_OFF; //  cash order
	if(OFFLINE == gl_MaintenaceParam.ucMerchantMode)
	{
		MainMenuS.sMask[3] = SM_ON;	//	restockpins
		MainMenuS.sMask[4] = SM_ON;	//	stock
		TransmissionMenuS.sMask[1] = SM_ON;		//	sales polling
		TransmissionMenuS.sMask[7] = SM_OFF;	//	balance
	}
	else if(ONLINE == gl_MaintenaceParam.ucMerchantMode)
	{	
		MainMenuS.sMask[3] = SM_OFF;
		MainMenuS.sMask[4] = SM_OFF;
		TransmissionMenuS.sMask[1] = SM_OFF;
		TransmissionMenuS.sMask[7] = SM_ON;
	}
	else
	{
		MainMenuS.sMask[2] = SM_ON; //  cash order
		MainMenuS.sMask[3] = SM_ON;
		MainMenuS.sMask[4] = SM_ON;
		TransmissionMenuS.sMask[1] = SM_ON;
		TransmissionMenuS.sMask[7] = SM_ON;
	}

	if (gl_MaintenaceParam.bManagerSett)
		MainMenuS.sMask[7] = SM_ON;		//	manager setting
	else
		MainMenuS.sMask[7] = SM_OFF;

	if(!gl_bCashier_Login)
	{
		CashierMenuS.sMask[0] = SM_ON;	//	log in
		CashierMenuS.sMask[1] = SM_OFF; //	log out
	}
	else
	{	
		CashierMenuS.sMask[0] = SM_OFF;
		CashierMenuS.sMask[1] = SM_ON;
	}

	TransmissionMenuS.sMask[6] = SM_OFF;	//	logo download
}

// Added by Kim_LinHB 2013-7-8 v4.62
static void Upload_Stock()
{
	int iRet;

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
		//DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
		ScrPrint(0, 2, 1, "%s", gl_szFTPErrInfo);
		vTrace("!! FTPLogin KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
		FTPCloseConn(TRUE);
		PubWaitKey(TIME_DELAY);
		return;
	}

	{
		unsigned char szPath[255];
		unsigned char szCurDate[SIZE_DATE_TIME + 1];
		unsigned char SN[32 + 1];
		int cLoop = 0;
		GetCurTime(szCurDate);
		memset(SN, 0, sizeof (SN));
		GetSerialNumber(SN);

		while(1)
		{
			sprintf(szPath, "%2.2s%6.6s_%8.8s_%d.DAT",
				PRE_YEAR, szCurDate, SN, cLoop++);
			
			iRet = FTPCheckFile(szPath);
			if(iRet != OK) // do not exist, not suitable
				break;
		}
		
		ScrCls();
		DispString("Uploading", 0 | DISP_LINE_CENTER | DISP_LINE_REVER);
		DispString(szPath, 3 | DISP_LINE_LEFT);
		iRet = FTPPutFile(szPath, FILE_STOCK, 5);
		if (iRet != OK)
		{
			ScrCls();
			//DispString(gl_szFTPErrInfo, DISP_SCR_CENTER);
			ScrPrint(0, 2, 1, "%s", gl_szFTPErrInfo);
			vTrace("!! FTPPutFile KO", &iRet, sizeof(iRet), TRACE_ERROR, TYPE_N);
			FTPCloseConn(TRUE);
			PubWaitKey(TIME_DELAY);
			return;
		}
	}

	FTPCloseConn(FALSE);
	ScrCls();
	DispString("Upload Done", DISP_SCR_CENTER);
	vTrace("-> Upload Done", NULL, 0, TRACE_WARNING, TYPE_N);
	Beep_Speaker(BEEP_OK);
	PubWaitKey(TIME_DELAY);
	return;
}
// Add End

// Added by Kim_LinHB 2013-8-16 v4.62  for power off when restockpin
// Modified by Kim_LinHB 2015-07-29 v5.09
void UpdateStock()
{
	int iStatus = 0;
	int iAvailableNum = 0;
	STOCK_NEW stock_new;
	STOCK stock;
	int i, iRet = -1;

	int num = FindCodesCnt(CODE_RESTOCK_DOWNLOADING, NULL);

	memset(&stock, 0, sizeof(STOCK));
	memset(&stock_new, 0, sizeof(STOCK_NEW));

	while(OK == DelPendingOrder());

	if(strlen(gl_MaintenaceParam.szLastProvisionID) > 0)
	{
		iRet = GetStockOrderByProvisionId(gl_MaintenaceParam.szLastProvisionID, &stock_new);
	}

	ReadStock_All(&stock);
	for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
	{
		if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState &&
			strlen(stock.stVoucher[i].szVoucherId) > 0 &&
			strlen(stock.stVoucher[i].szSerial) > 0 &&
			strlen(stock.stVoucher[i].szProductId) > 0 &&
			strlen(stock.stVoucher[i].szNum) > 0)
		{
			 ++iAvailableNum;
		}
	}

	if(-1 == iRet) //download success
	{
		iStatus = 0;
	}
	if(0xFF == iRet) //UpdateStockOrder szProvisionID fail
	{
		iStatus = 1;
	}
	else if((0 == num || iAvailableNum != num) && (0 == stock_new.num)) //UpdateStockOrder szProvisionID suc, but WriteStock_All fail
	{
		iStatus = 2;
	}
	else if((num > 0 && iAvailableNum == num) && (0 == stock_new.num)) //WriteStock_All suc, but UpdateStockOrder num fail
	{
		iStatus = 3;
	}
	else if((num > 0 && iAvailableNum == num) && (stock_new.num > 0)) //UpdateStockOrder num suc, but update stock status fail
	{
		iStatus = 4;
	}
    else if(num == 0  && stock_new.num > 0)//Update stock status suc, but clear szLastProvisionID fail
	{
		iStatus = 5;
	}
    else if((iAvailableNum != num) && stock_new.num > 0)
    {
        iStatus = 6;
    }
    else 
    {
        iStatus = 7;
    }

	switch(iStatus)
	{
	case 2:
    case 6:
		/*ScrCls();
		ScrPrint(0,0,0, "stock was damaged, doing clean");
		PubWaitKey(3);*/
		for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
		{
			if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState)
			{
				memset(&stock.stStock[i], 0, sizeof(STOCK_INFO));
				memset(&stock.stVoucher[i], 0, sizeof(VOUCHER_INFO));
				strcpy(stock.stVoucher[i].szExpirDate, "999999999999");
				stock.stStock[i].ucProductState = CODE_RESTOCK_EMPTY;
			}
		}
		WriteStock_All(&stock);
		iAvailableNum = num = 0;
		break;
	case 3:
		/*ScrCls();
		ScrPrint(0,0,0, "updating order");
		PubWaitKey(3);*/
		for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
		{
			if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState)
			{
				memcpy(&stock_new.stVoucher[stock_new.num++], &stock.stVoucher[i], sizeof(VOUCHER_INFO));
			}
		}
		UpdateStockOrder(&stock_new);
	case 4:
		/*ScrCls();
		ScrPrint(0,0,0, "updating stock status");
		PubWaitKey(3);*/
		for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
		{
			if(CODE_RESTOCK_DOWNLOADING == stock.stStock[i].ucProductState)
			{
				stock.stStock[i].ucProductState = CODE_RESTOCK_FREE;
			}
		}
		WriteStock_All(&stock);
		SortStock();
	case 5:
		/*ScrCls();
		ScrPrint(0,0,0, "download suc, clean LastProvisionID");
		PubWaitKey(3);*/
		memset(gl_MaintenaceParam.szLastProvisionID, 0, sizeof(gl_MaintenaceParam.szLastProvisionID));
		WriteParam(WRT_MAINTA);
		break;
	default:
		break;
	}

	if(0 == ReadStockOrder(&stock_new))
		Prn_RestockPin();

	// Added by Kim_LinHB 2015-07-30
	/*ScrCls();
	kbflush();
	ScrPrint(0, 1, 0, "Last Id :%s", gl_MaintenaceParam.szLastProvisionID);
	ScrPrint(0, 2, 0, "order num:%d", stock_new.num);
	ScrPrint(0, 3, 0, "num: %d", num);
	ScrPrint(0, 4, 0, "available num:%d", iAvailableNum);
	ScrPrint(0, 5, 0, "press any key");
	getkey();*/ //Removed by wuc 2015.10.28
}
// Add End

// Added by Kim_LinHB 2014-2-10 v4.84
void AdjustDate(unsigned char *pszYYMMDDhhmmss, short day)
{	
	ulong ulTime;
	unsigned char szYYYYMMDDhhmmss[SIZE_DATE_TIME + 1 + 2];
	if(!pszYYMMDDhhmmss)
		return;
	sprintf(szYYYYMMDDhhmmss, "%s%12s", PRE_YEAR, pszYYMMDDhhmmss);
	ulTime = PubTime2Long(szYYYYMMDDhhmmss); //sec
	ulTime += (day*24*60*60); //+- "day" days
	PubLong2Time(ulTime, szYYYYMMDDhhmmss);
	strcpy(pszYYMMDDhhmmss, szYYYYMMDDhhmmss + 2);
}
