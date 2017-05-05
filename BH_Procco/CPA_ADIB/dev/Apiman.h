/*
*%-----------------------------------------------------------------------------
*% File........... : APIMAN.H
*% Project........ : SIGMA PORTABLE
*% Created ....... : 24.06.1996
*% Version ....... : 1.00
*% Module ........ : Application manager
*% Sub-module .... :
*% Description ... : AM API functions prototypes
*%-----------------------------------------------------------------------------

  *% Revision 1.1  2010/12/07 07:33:28  hejj
  *% 2010-12-7 15:32 by Gary
  *%
  *%-----------------------------------------------------------------------------
*/
#ifndef _APIMAN_H
#define _APIMAN_H

#include "common.h"
//#include "trap.h"
//!!!sel???#include "ecrlib.h"	

/* $HeaderFileStart --------------------------------------------------------*/
/* $HeaderFileEnd ----------------------------------------------------------*/

/* $GlobalDataStart --------------------------------------------------------*/

typedef struct
{
    BYTE buf[17];
} AID_STRUCT;

typedef unsigned char BCD6[6];

/* the following structure describe the basic entity of
a heriarchy for the menus */
typedef struct
{
    OS_WORD level;                  // depth for the label
    char far *name;              // name of the label
    char (far *function)( void); // function to execute when option is selected
} HIERARCHY;

/* messages IDs and structures for application access */
/* ALARMS */
typedef struct
{
    BYTE Date[LG_DATE + 1];
    BYTE Hour[LG_TIME + 1];
    OS_WORD WakeUpMsg;
} ALARM_STRUCT;

/* ALARM_SESSION */
#define MAX_KEY 40


/* initialisation type */
#define INIT_BOOT     0
#define INIT_DOWNLOAD 1
#define INIT_TELEINIT 2
#define INIT_ACTIVATE 3
#define INIT_RELOCATE 4

/* returned status */
#define INIT_FAIL 0
#define INIT_OK   1

/* CARD RECOGNITION */
typedef struct
{
    BYTE DataType;               // Magnetic, cheque, manual, other
    BYTE CardType;               // Free for application use
    BYTE CardBuffer[41];         // Track data or ATR
    BYTE Buffer[256];            // reserved for future use (TLV objects only!)
} CARD_DATA_STRUCT;

/* Definition for the Smart Card File */
#define SCF_NAME        "$SCF"    // SC File: Name
#define SCF_KEY_OFF      0        // SC File: Key Offset
#define SCF_KEY_LEN      3        // SC File: Key Length
#define SCF_RESET_TYPE  "SRT"     // SC File: Key for Reset Type record
#define SCF_RESET_DATA  "SRD"     // SC File: Key for Reset Data record
/* Reset type (saved in the record for which
* the key is SCF_RESET_TYPE) */
#define RESET_ASYNC    0
#define RESET_SYNC_1   1

/* data type */
#define CARD_SYNC     0
#define CARD_ASYNC    1
#define CARD_CHECK    2
#define CARD_MAG      3
#define CARD_MANUAL   4
#define CARD_ECR      5
// #define CARD_ICC      6    no longer supported
#define CARD_LOYALTY  255

/* EXECUTE TRANSACTION */
typedef struct
{
    CARD_DATA_STRUCT* CardData;
    BOOLEAN           TipFlag;         // OS_TRUE or OS_FALSE
    BCD6              Amount;          // amount in BCD
    BYTE              TransactionType; // transaction type
    BYTE              ServerId;        // server identifier
} EXEC_TRANSACTION_STRUCT;      // to be replaced by START_TRAN_STRUCT

typedef struct
{
    CARD_DATA_STRUCT* CardData;
    BOOLEAN           TipFlag;         // OS_TRUE or OS_FALSE
    BCD6              Amount;          // amount in BCD
    BYTE              TransactionType; // transaction type
    BYTE              ServerId;        // server identifier
    OS_WORD              CurrencyCode;    // ISO currency code
    AMOUNT_CONFIG     AmtCfg;          // currency format
    BYTE              MerchantId;      // merchant (deal) identifier
    OS_WORD              LanguageCode;    // ISO language code
    char              LangAbbr[2+1];   // ISO language abbr
    BYTE              TreatmentType;   // RFU
    char              Appname[6+1];    // name of application (normally blank)
    OS_WORD              BitMap;          // See values below
} START_TRAN_STRUCT;

/* Transaction type */
#define TRANS_TIP              0   // reserved
#define TRANS_DEBIT            1   // Sale
#define TRANS_CREDIT           2   // Refund
#define TRANS_CANCEL           3   // Void the previous transaction
#define TRANS_DEBIT_ONLY       4   // reserved
#define TRANS_CREDIT_ONLY      5   // reserved
#define TRANS_UNKNOWN          6   // reserved
#define TRANS_AUTH_ONLY        7   // Authorize but do not move funds (no cashflow)
#define TRANS_COMPLETE         8   // Complete the authorization by moving funds (debit)
#define TRANS_FORCE            9   // Force a transaction into the batch
//#define TRANS_CANCEL_LAST     10   // Cancel previous transaction
#define TRANS_CANCEL_ANY      10   // Void some transaction (must select)
#define TRANS_LOYALTY_AWARD   20   // Add loyalty points
#define TRANS_LOYALTY_REDEEM  21   // Subtract loyalty points

// START_TRAN_STRUCT.BitMap
#define TRAN_OPTION_FORCE 0x0001   // Force transaction online
#define TRAN_OPTION_RFU2  0x0002
#define TRAN_OPTION_RFU3  0x0004
#define TRAN_OPTION_RFU4  0x0008
#define TRAN_OPTION_RFU5  0x0010
#define TRAN_OPTION_RFU6  0x0020
#define TRAN_OPTION_RFU7  0x0040
#define TRAN_OPTION_RFU8  0x0080

/* unknown server id */
#define SERVER_UNKNOWN   0xFF
/* sometimes means "default server" or "not defined" or "mono-server mode"*/
#define SERVER_DEFAULT   0      /*  real default server; can't be deleted */
#define MERCHANT_UNKNOWN 0xFF /* "not yet defined" */
#define MERCHANT_DEFAULT 0x01 /* default merchant; can't be deleted */
#define MERCHANT_ALL     0xFE /* all merchants together */
#define CONTRACT_UNKNOWN "???"  /* Contract number when it is unknown by AM */
//#define CONTRACT_NONE    '\0' /* First char of a not defined contract */

/* parameter N */
typedef struct
{
    BYTE Bitmap;                 // specify command and fields to modify
    BYTE Pad[MAX_PHONENUM + 1];  // PAD number
    BYTE Number[LG_NUMBER + 1];  // Center or RTC + scenario
    BOOLEAN TranspacFlag;        // Transpac use flag
    BYTE Date[LG_DATE + 1];      // date of downloading
    BYTE Time[LG_TIME + 1];      // time of downloading
    BYTE NbRetry;                // nb retry if fail
    OS_WORD Delay;                  // delay between 2 retries
} DLSTRUCT;                     // Used by DownloadAt()

typedef struct
{
    BYTE Bitmap;                 // specify command and fields to modify
    char PhoneNumber[LG_PHONENUM+1];      // PAD number
    char NetworkAddress[LG_ADDRESS];     // Center or RTC address
    char Scenario[LG_SCENARIO+1];          // Scenario
    BOOLEAN TranspacFlag;        // Transpac use flag
    BYTE Date[LG_DATE + 1];      // date of downloading
    BYTE Time[LG_TIME + 1];      // time of downloading
    BYTE NbRetry;                // nb retry if fail
    OS_WORD Delay;                  // delay between 2 retries
} DLSTRUCT3;                    // Used by DownloadAt2()

/* bits 0 and 1 of Bitmap is for command */
#define DL_COMMAND_MASK (3)

#define DL_READ         (0)
#define DL_DELETE       (1)
#define DL_WRITE        (2)
#define DL_REPLACE_PAD  (3)

/* bit 2 for return status field */
#define DL_DATA_OK (1<<2)

/* questions */
#define DATE_TIME_CHANGE     0 // Is it ok to change the date?
#define SECURITY             1 // Not used
#define TERMINAL_NUM_CHANGE  2 // Is it ok to change the term num?
#define TERM_NUM_VALID       3 // Is the term num valid?
#define DEACTIVATION         4 // Can you be deactivated?
#define DOWNLOAD             5 // Can you be moved in memory?
#define DATE_VALID           6 // Is the new date valid?
#define SECURITY_DATE        7 // Perform security for date change.
#define SECURITY_TERMINAL    8 // Perform security for term num change.
#define SECURITY_DOWNLOAD    9 // Perform security for download.
#define RESUBMIT_CURRENCY   10 // Call AM_NewCurrency() again
#define TIME_VALID          11 // Is the new time valid?
#define DATE_TIME_VALID     12 // Are the new date and time valid?
#define EMV_TRANSAC         13 // Is OK for the EMV transaction?
#define NO_QUESTION        255 // Ignore question

/* responses */
#define NOT_APPROVED 0
#define APPROVED     1
#define NEW_AID      2
#define NO_CURRENCY  3

#define DELETE_OK        0
#define DELETE_CANCELLED 1

/* TOTALS */
#define SIMPLE_TOTALS 0
#define NET_TOTALS    1

/* PROCESS MESSAGE TYPES */
typedef enum
{
    INITIALIZE,
        WAKEUP_ALARM,
        CARD_PRECISION,
        EXECUTE_TRANSACTION,      // Old entry always supported by AM, new
        // applications must use START_TRANSACTION
        // instead
        DUPLICATE,
        QUESTION,
        EXECUTE_MENU,
        EXECUTE_EOD,
        TELEINIT_DAT,
        EXECUTE_TOTALS,
        PLANNED_ACTIONS,
        CANCEL_LAST_TRANSACTION,
        DELETE_APPLICATION,
        RESET_ALARMS,
        RESET_TOTALS,
        TRANSACTION_END,
        START_TRANSACTION,         // Must be used by new applications (replace
        // EXECUTE_TRANSACTION)
        GET_DATAS,                 // Entry to get application datas
        TRANS_MENU,			         // AB 26.01.99
        ECR_QUERY,                 // for ECR specific applications
        COM_ENTRY,                 // for COM specific applications
        EMV_PRECISION,             // for EMV specific applications
        HELP,             //
        
        ALARM_SESSION,             // for alarm session
        END_ALARM_SESSION,         // to stop alarm session
        CALL_APP_REQUEST,          // for application communication
        CALL_APP_ANSWER,           // for application communication
        KEY_0_MENU,
        KEY_1_MENU,
        KEY_2_MENU,
        KEY_3_MENU,
        KEY_4_MENU,
        KEY_5_MENU,
        KEY_6_MENU,
        KEY_7_MENU,
        KEY_8_MENU,
        KEY_9_MENU,
} PROCESS_MSG_ID;

// API Entry type
typedef OS_WORD( far * API_ENTRY)(void );

/* MASKS for manager task */
#define ALG_MASK     ( 1 << 0 )
#define TIME_MASK    ( 1 << 1 )
#define MAG_MASK     ( 1 << 2 )
#define ICC_MASK     ( 1 << 3 )
#define KEYB_MASK    ( 1 << 4 )
#define DISP_MASK    ( 1 << 5 )
#define ECR_MASK     ( 1 << 6 )
#define PRT_MASK     ( 1 << 7 )
#define COM_MASK     ( 1 << 8 )

/* user service mask used by applications */
#define USER_SERVICE 128

/* service returned when error */
#define TASK_ERROR 255

/*******************************KEYBOARD*************************************/
#define KEYB_WAIT_PRESSED 0
#define KEYB_GET_AMOUNT   1

/* status */
#define KEYB_OK      0
#define KEYB_ERROR   1
#define KEYB_TIMEOUT 2

/*******************************TIMER****************************************/
#define TIME_ON           0
#define TIME_OFF          1
#define TIME_SET_ALARM    2
#define TIME_DELETE_ALARM 3


/*******************************ALGORITHM************************************/


/*******************************DISPLAY**************************************/
#define DISPLAY_MESSAGE 0
#define DISPLAY_BITMAP  1

// Display Attributes
#define SMALL_FONT   0x01       // OS, Small Font (default)
#define LARGE_FONT   0x02       // OS, Large Font
#define USER_FONT1   0x04       // AM, Big numbers (time)
#define USER_FONT2   0x05
#define USER_FONT3   0x06
#define OS_REVERSE    (1<<3)
#define UNDERLINE  (1<<4)
#define CLEAR      (1<<7)


/*******************************PRINTER**************************************/
#define PRINT_HEADER       0
#define PRINT_BODY         1
#define PRINT_BOLD         2
#define PRINT_FOOTER       3

#define PRT_NOT_OK          0
#define PRT_OK              1
#define PRT_END_ERROR       2

/*******************************MAGNETIC*************************************/
#define MAG_WAIT_SWIPED    0

/*******************************ICC******************************************/
#define ICC_WAIT_DELAY_IN    0
#define ICC_WAIT_IN          1
#define ICC_TIMEOUT          2
#define ICC_WAIT_OUT         3

/*******************************COM******************************************/
#define COM_WAIT      1

/*******************************ECR******************************************/

/*******************************DISPLAY**************************************/
#define CENTERED_LINE1      0x01
#define CENTERED_LINE2      0x02
#define CENTERED_LINE3      0x04
#define CENTERED_LINE4      0x08
#define CLEAR_DISPLAY       0x80


/*********** AM API ***********/
typedef struct
{
    DWORD KeyPressed;
    DWORD KeyEnterPressed;
    DWORD LinePrinted;
    DWORD CardSwiped;
    DWORD CardInserted;
    DWORD Connection;
    DWORD CoverOpen;
    DWORD SwipeError;
    DWORD InsertError;
    DWORD Call;
    DWORD RadioReset;
    DWORD ChannelChange;
    DWORD TranspacError;
    DWORD BatteryWarning;
} MAINT_STRUCT;

/* $GlobalDataEnd ----------------------------------------------------------*/

/* $GlobalFnStart -----Function prototypes ---------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif
    /* start of API functions */
    BOOLEAN AM_UseAPI(char *, OS_WORD *);
    
#define GetSoftIndex(name, index) AM_UseAPI(name,index)
    
    int  ManSprintf( char far* dest, char far* formatstr, ...);
    // ManSprintf() is deprecated--use ManSprintfn() instead!
    int ManSscanf( char far *, char far *,...);
    void ManGetAppList( void **);
    void ManGetLibList( void **);
    BOOLEAN DownLoadAt( DLSTRUCT* fDownload);
    // Do not use AM_DownloadAt(). For this AM version, use DownloadAt() instead
    BOOLEAN AM_DownloadAt( DLSTRUCT3* fDownload);
    BOOLEAN GetServerName( BYTE fId, BYTE * fName);
    BOOLEAN ServerSelect( BYTE * fId);
    BOOLEAN AM_MerchantSelect( BYTE * fId, BOOLEAN autoselect, BOOLEAN all);
    BOOLEAN AM_MerchantName( BYTE fId, BYTE * fName);
    BOOLEAN GetMerchantName( BYTE * fName);
    BOOLEAN GetMaintInfo( MAINT_STRUCT * fInfo);
    //  BOOLEAN GetPortInfo( PORT_ASSIGN_STRUCT* ports);
    
    BOOLEAN AM_GetAppInfo( char* appname, APPLCONFIGSTRUCT* fConf, BYTE* fOption);
    
    typedef enum
    {
        CALL_APP_OK,
            CALL_APP_NOTFOUND,
            CALL_APP_BADPARAM,
            CALL_APP_NOTFREE   
    }AM_CALL_APP_STATUS ;
    
#ifdef __cplusplus
}
#endif

/* $GlobalFnEnd ------------------------------------------------------------*/
#endif
/*$--- End of file : apiman.h ---*/
