/****************************************************************************
NAME
    framework_salesman.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.14      - created
****************************************************************************/
#ifndef _FRAMEWORK_SALESMAN_H
#define _FRAMEWORK_SALESMAN_H


#define STR_VERSION_SUB "5.08"
#define STR_VERSION_DATE "170109_"

#define PARAMVER "1"
#define FILEVER  0x65

#define PRE_YEAR		"20"

#define TEST_SN			"29003209"

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
#define DEBUGFLG "_D"
#else
#define DEBUGFLG 
#endif // _DEBUG

#define APP_NAME		"PayFlex" DEBUGFLG
#ifdef FARSI
//#define STR_VERSION     STR_VERSION_EPPCO STR_VERSION_SUB "fa" STR_VERSION_MULTI
#define STR_VERSION     STR_VERSION_DATE STR_VERSION_SUB "fa" DEBUGFLG
#else
//#define STR_VERSION     STR_VERSION_EPPCO STR_VERSION_SUB "en" STR_VERSION_MULTI
#define STR_VERSION     STR_VERSION_DATE STR_VERSION_SUB "en" DEBUGFLG
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


#define	ID_Order				0		//sub
#define ID_Offline				1		//sub
#define ID_CashCollection		2		//sub
#define ID_BankDeposit			3		//sub
#define ID_RecordVisit			10
#define ID_Consignment			11		//sub  // Modified by Kim_LinHB 2013-10-28 v4.68
#define ID_CancelLastOrder		13
#define ID_Information			4		//sub
#define ID_Reports				5		//sub
#define ID_Settings				15
#define ID_Maintenance			16				/* Maintenance */
#define ID_Transmission			6		//sub	/* Server Call */

#define ID_CashOrder			20
#define ID_CreditOrder			21
#define ID_ReturnOrder			22
#define ID_CreditTransfer		23

#define ID_OfflineOrder				30
#define ID_OfflineVisit				31
#define ID_OfflineCancelLastOrder	32
#define ID_OfflineBatchUpload		33

#define ID_CashCollectionByMerchant 40
#define ID_CashCollectionByOrder	41

#define ID_BankDepositByOrderNumber			50
#define ID_BankDepositOrderNumberByBatch	51
#define ID_BankDepositCashCollectionByBatch 52

#define	ID_Pending_Orders_ECC		60
#define	ID_Salesman_Infos_ECC		61
#define ID_SalesmanRoute			62
#define ID_PrintDiscount			63
#define ID_ChangePWD				64

#define ID_Offline_Batch_Upload_Rep 70
#define ID_Offline_Physical_Stock_Rep 71
#define	ID_Order_Rep				72
#define	ID_Online_Stocks_Rep		73
#define ID_Last_Consign_Order_Rep	74
#define ID_Summary_Rep				75
#define ID_Order_Rep_Prod			76

#define ID_SettingsDownload			80   /* SettingsDownload */
#define ID_SoftDownload				81  /* Telech Appli */
//#define ID_Config_Auto				82
//#define ID_First_Init__Auto			83

// Modified by Kim_LinHB 2013-10-28 v4.68
#define ID_ConsignOrder			110
#define ID_ReturnConsignOrder	111
// Added by Kim_LinHB 2013-10-29 v4.68
#define ID_ConsignConfirm		112
#define	ID_PendingConsignOrder	113


void DispDefScreen(void);
void DispVersion(void);
void DispMainMenu(void);
void DispOrderMenu(void);

void DispConnection(void);

unsigned char ParamNotSpecified(void);

// Added by Kim_LinHB 2013-5-9 v4.56
void Awake_Plan(void);

#endif
