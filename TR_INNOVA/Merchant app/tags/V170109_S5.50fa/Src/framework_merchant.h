/****************************************************************************
NAME
    framework_merchant.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.31      - created
****************************************************************************/
#ifndef _FRAMEWORK_MERCHANT_H
#define _FRAMEWORK_MERCHANT_H

// Modified by Kim_LinHB 2015/5/28
#define STR_VERSION_SUB "5.50"
#define STR_VERSION_DATE "170109_"

#define PARAMVER "1"
#define FILEVER  0x67	// Modified by Kim_LinHB 2013-9-3 from 0x62 to 0x67 for trigger setting D after remote D

#define PRE_YEAR		"20"

#define	TEST_SN			"29211266" //Modified by wuc 2015.8.26
#define TEST_ID			"29211266"

#ifdef MULTI_LANG
#define STR_VERSION_MULTI "_m"
#else
#define STR_VERSION_MULTI
#endif

#ifdef DUBAI
#define STR_VERSION_EPPCO "EPPCO "
#else
#define STR_VERSION_EPPCO
#endif

#ifdef LHB_DEBUG
// Hidden by Kim_LinHB 2014/10/28
#define DEBUGFLG "_D"
#define SHORTD	"D"
#else
#define DEBUGFLG 
#define SHORTD
#endif // _DEBUG

#include "connect.h"

#ifdef _S_SERIES_
#define TERMTYPE	"S"
#else
#define TERMTYPE	"P"
#endif

#define APP_NAME		"PAYFLEX EVD" DEBUGFLG
#ifdef FARSI
//#define STR_VERSION     STR_VERSION_EPPCO STR_VERSION_SUB TERMTYPE "fa" STR_VERSION_MULTI
#define STR_VERSION     STR_VERSION_DATE TERMTYPE STR_VERSION_SUB "fa" SHORTD
#else
//#define STR_VERSION     STR_VERSION_EPPCO STR_VERSION_SUB "en" STR_VERSION_MULTI
#define STR_VERSION     STR_VERSION_DATE TERMTYPE STR_VERSION_SUB "en" SHORTD
#endif

#define  UNKNOWN_TERMINAL  0 //v4.15

#define _TERMINAL_P60_S_   1
#define _TERMINAL_P70_S_   2
#define _TERMINAL_P60_S1_  3
#define _TERMINAL_P80_     4
#define _TERMINAL_P78_     5
#define _TERMINAL_P90_     6
#define _TERMINAL_S80_	   7  
#define _TERMINAL_SP30_	   8  
#define _TERMINAL_S60_	   9
#define _TERMINAL_S90_	   10

//main menu
#define ID_SaleCodes			10
#define ID_Duplicate			11
#define ID_RestockPins			12
#define ID_Stocks				1	//sub
#define ID_ON_OFFService		13
#define ID_Reports				2	//sub
#define ID_ManagerSettings		14
#define ID_Maintenance			15
#define ID_Transmission			3	//sub
#define ID_Cashier				4	//sub
#define ID_CashOrder			16

//sub menu
#define ID_PrintStock			20
#define ID_DispStock			21
#define ID_PrintGeneral			22
#define ID_PrintDetail			23

#define ID_ReportCashier		30
#define ID_ReportManager		31
#define ID_ReportServer			32

#define ID_SettingsDownload		40
#define ID_SalesPolling			41
#define ID_CancelSale			42
#define ID_Claim				43
#define ID_GetMail				44
#define ID_SoftDownload			45
#define ID_LogoDownload			46
#define ID_Balance				47

#define ID_Login				50
#define ID_Logout				51
#define ID_ChangePwd			52
#define ID_CreateUser			53
#define ID_DelUser				54
#define ID_ViewList				55

// Added by Kim_LinHB 2013-12-4 v4.76
#define ID_RestockPin			61
#define ID_PendingOrder			62

void DispDefScreen(void);
void DispVersion(void);
void DispMainMenu(void);
void DispStock(unsigned short index);

void DispConnection(void);
// Added by Kim_LinHB 2014/11/4
void DispSending(void);
void DispReceiving(void);
// Add End

void Awake_Plan(void);

unsigned char ParamNotSpecified(void);

void UpdateStock();

#endif
