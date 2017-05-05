#ifndef _OS_INTERFACE_
#define _OS_INTERFACE_

#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "debug.h"

#include "dssapi.h"
#include "common.h"



// JUSTIFY OPTIONS
#define JUSTIFY_LEFT			0x01
#define JUSTIFY_RIGHT			0x02
#define JUSTIFY_CENTER			0x03
#define SMALL_FONT_LINE_MAX	        21	
#define LARGE_FONT_LINE_MAX	        16	
#define LG_CSTMSG						43 	
#define LG_PADMAX	((unsigned int)200)	
#define HEADER_NOT_INVERT			0x01	
#define HEADER_NONE						0x02	
#define LG_IP_NT			16			// IP Address In Format xxx.xxx.xxx.xxx NULL Terminated.  

#define FILTER_HEAD		0x01 // 
#define FILTER_TAIL		0x02 // 
#define FILTER_ALL		0x03 // 
#define FILTER_FORCE	0x80 // 
#define LG_LINEHEIGHT	18		// 
#define TENMILISEC				10		
#define FIFTYMILISEC			50		
#define HUNDREDMILISEC			100		
#define HALFSECOND				500		
#define ONESECOND				1000	
#define ONEMINUTE				60000	

#define TO_NONE					0xFE
#define TO_DEFAULT				0x05
#define TO_MAX					0xFF
#define TO_NOWAIT				0x00
#define TO_1SEC					0x01
#define TO_2SEC					0x02	
#define TO_3SEC					0x03
#define TO_4SEC					0x04
#define TO_5SEC					0x05
#define TO_10SEC				0x0A
#define TO_15SEC				0x0F
#define TO_20SEC				0x14
#define TO_30SEC				0x1E
#define TO_1MIN					0x3C

typedef struct m_entry
{
    const char *text;                    /* entry - text         */
    char type;                                 /* type - optional      */
    int (*func)(int parameters);   /* user function  */
    int parameters;                            /* parameters for */
    /* user function  */
}m_entry;

typedef struct menu
{
    char    *header;                /* header text                           */
    int     no_of_entries;          /* no of entries                   */
    m_entry *menu_entries;         /* pointer to vector of entries */
    char    mode_flags;             /* flags for menu                  */
    int		selected_item;
} menu;



void OS_DisplayStr(INT iLine, INT iCol, CHAR *szMsg);

//OS_STATUS OS_PrintLine(PCHAR SrcBuf);
INT OS_ClkReadDate(WORD *Week,WORD *Day,WORD *Month,WORD *Year);
INT OS_ClkReadDateTime(PBYTE Input/*WORD *Second,WORD *Minute,WORD *Hour,WORD *Day,WORD *Month,WORD *Year*/);
INT ClkSetDate(PBYTE lpDate);
INT ClkSetTime(PBYTE lTime);
INT ClkUpdateDateTime(PBYTE lDateTime);

WORD StartPrinter(void);
void DispPrnError(BYTE ucErrCode);

INT OS_DeleteFile(PBYTE FileName);

OS_STATUS OS_PortReadString( INT PortNum,INT ReceiveLen, PBYTE Recv,WORD * ReceivedLen,DWORD TimeOut,DWORD What);;
OS_STATUS OS_PortWriteString( INT PortNum, PBYTE SendBuf,DWORD SendLen);
INT OS_MenuFunc(CHAR **labels, INT numLabels);

INT OS_ClearScreen(void);


INT KeyBoardMenu(BYTE ItemCount,const BYTE* MenuItems[],PBYTE FirstItem,BYTE gOldItem,WORD TimeOut,BYTE byArabicHeader);
//OS_STATUS OS_PaperStatus(void);


INT Write(INT FileHandle, PBYTE Buff, INT Len);
INT NCreate(PBYTE FileName, INT Mode);
INT Open(PBYTE FileName, BYTE Mode);
INT Delete(PBYTE FileName );
INT Read(INT FileHandle, PBYTE Buf, INT Len);
INT Seek(INT FileHandle, DWORD Pos, INT Where);
INT Close(INT Handle);
INT Rename(PBYTE Old, PBYTE New);

INT ClkReadDate(PBYTE Time,	INT Mode);
INT ClkReadTime(PBYTE bTime);
void GetPrnDateTime(char *pszOutBuff);


INT GetApplicationConfig(APPLCONFIGSTRUCT *pApplConfig);
INT GetFileSize(PBYTE FileName, DWORD *llLogicSize);

INT KeyBoardDataCapture(PBYTE Title, INT Mode, BYTE MinLen, PBYTE MaxLen,PBYTE Input,INT TimeOut, INT Flag);
INT OS_KeybdGet(PBYTE Key, BYTE TimeoutSecond);

INT OS_LogToHex(char *header,void *buf, int buf_size, int device);
INT OS_LogTo(int device, const char* fmt, ...);
BYTE KeyBoardValid( PCHAR fPrompt, PCHAR fMessage1, PCHAR fMessage2, 
                       PCHAR fMsgHotKey1, PCHAR fMsgHotKey2, PCHAR fMsgHotKey3,
                       BYTE fKeyMask, BYTE fTimeOut);
BYTE AlgLuhnKey (BYTE CardNumSize, PBYTE CardNum);

BYTE GetOneKey(BYTE TOSeconds);
char GetOneKeyCheckTime(BYTE TOSeconds, DWORD *pDeciSeconds);   // By Gary.
VOID OS_Beep(INT Time);   // By Gary.

DWORD Scan_MillSec(void);

void Kb_Flush(void);
uchar Kb_Read( void );

void clrscr_info(void);

INT ManRequestMessage(INT Mode1, INT Mode2, INT Len, VOID *Type);
void BfAscii(char *Out, unsigned char * In, int len);

char *StrCpy(unsigned char *s1, unsigned char *s2);
VOID OS_KeybdFlush(void);
void     OS_Sftkey(BYTE *newkey);
void     OS_Sftencrypt(BYTE *cleartext);


void vDisplay_menu (unsigned char ucMenuitems[][22], int iInitItem, int iDiffItem);

//by jarod 2011.06.07
int	Check_SecurityMenuPW();

extern CHAR CRLF[3];

//INT OS_PrinterOpenVx(void);
BYTE KeyBoardQuestion (BYTE * fPrompt, BYTE * fMessage1, BYTE * fMessage2, WORD fTimeOut);

#define Opt_INVERSE			0x01	// 01 - INVERSE SELECTED					
#define Opt_SKIPSINGS		0x02	// 02 - SKIP SINGS ON SELECT <>				
#define Opt_LONGMSG			0x04	// 04 - DISPLAY 21 CAHR WITH INVERSE FONT	
#define Opt_ARABIC			0x08

// Keyboard Return Values	
// #define K_BACK			68		
// #define K_00			    46					
// #define K_UP			    66					
// #define K_DOWN			88					
// #define K_VOID			70					
// #define K_PVOID			65	
// #define K_ENTER			13					
// #define K_ESC			84					
// #define K_MENU			67					
// #define K_CLEAR			65					
// #define K_FUNC			140					
// #define K_PMENU			42	
// #define K_TPAD			111									

typedef enum
{
    eEMV_SUCCESS = 0,
    eNO_CARD_OR_WITHDRAWN,
    eICC_NEEDS_RESET,
    eICC_SOFTWARE_ERROR,
    eICC_COMMUNICATION_ERROR,
    eICC_DATA_PROBLEM,
    eINTERNAL_ERROR,
    eNO_MUTUAL_APP = 0x80,
    eCARD_BLOCKED_INVALIDATED = 0x40,
    eAPP_BLOCKED_INVALIDATED = 0x20,
    ePIN_ALREADY_BLOCKED = 0x10,
} ERROR_DESCRIPTIONS;                 

typedef enum
{ 
    EMV_FAIL = 0,
#if 0
#ifndef DEVELOP_VERSION
    EMV_OK,
    EMV_TERMINATE_TRN,
    EMV_ONLINE,
    EMV_APPROVED,
    EMV_DECLINED,
    EMV_FALLBACK,
    EMV_REF_APPROVED,
    EMV_PIN_BLOCKED,
    EMV_PIN_CANCEL,           /* PIN entry is canceled by either cardholder or merchant */
    EMV_HASH_FAIL,						/* Hash verification failed */
    
    /* EXTERNAL AUTHENTICATE returns */
    EXTAUTH_6300,             /* External Authenticate failed */
    EXTAUTH_6985,             /* Command not supported */
    
    /* SELECT returns */
    SELECT_6283,    /* File Invalidated */
    SELECT_6A81,    /* Function not supported */
    SELECT_6A82,    /* File not found */
    SELECT_XXXX,    /* Non-recognized return */
    
    /* VERIFY returns */
    VERIFY_6983,    /* Authentication method blocked */
    VERIFY_6984,    /* Reference data invalidated */
    VERIFY_63C0,    /* Left retries on least significant byte */
    
    /* READ RECORD returns */
    READREC_6A83,   /* Record not found */
    READREC_XXXX,   /* Non-recognized return */
    
    /* GET DATA returns */
    GETDATA_6A81,  
    GETDATA_6A86,  
    
    /* GET PROCESSING OPTIONS returns */
    GETPROC_6985,    /* Conditions of use not satisfied */
    
    /* GENERATE AC returns */
    GENERATE_6985,   /* Too many GENERATE AC commands */
    EMV_CARD_INFO_ERROR
#endif
#endif
} EmvStatus;

#define tagAUTH_AMOUNT_BIN           0x8100  /*     b            4  */
#define tagAFL                       0x9400 /*    var        -252  */
#define tagAID_CARD                  0x4F00  /*     b         5-16  */
#define tagAIP                       0x8200  /*     b            2  */
#define tagAPPL_LABEL                0x5000  /*    ans**         1-16  */
#define tagPAN                       0x5A00  /*    cn -19      -10  */
#define tagAPPL_PRIORITY_IND         0x8700  /*     b            1  */
#define tagAPPL_TEMPLATE             0x6100  /*     b         -252  */
#define tagAUTH_CODE                 0x8900  /*    var           6  */
#define tagAUTH_RESPONSE_CODE        0x8A00  /*    an            2  */
#define tagCDOL1                     0x8C00  /*     b         -252  */
#define tagCDOL2                     0x8D00  /*     b         -252  */
#define tagCERT_AUTH_PKI             0x8F00  /*     b            1  */
#define tagCVM_LIST                  0x8E00  /*     b         -252  */
#define tagCOMMAND_TEMPLATE          0x8300  /*     b          var  */
#define tagDF_NAME                   0x8400  /*     b         5-16  */
#define tagDDF_NAME                  0x9D00  /*     b         5-16  */
#define tagDIR_DISCR_TEMPLATE        0x7300  /*    var        -252  */
#define tagFCI_PROPRIETARY_TEMPLATE  0xA500  /*    var         var  */
#define tagFCI_TEMPLATE              0x6F00  /*    var        -252  */
#define tagISSUER_PK_REMAINDER       0x9200  /*     b    Ni-Nca+36  */
#define tagISSUER_SCRIPT_COMMAND     0x8600  /*     b         -261  */
#define tagISSUER_AUTH_DATA          0x9100  /*     b         8-16  */
#define tagISSUER_PK_CERTIFICATE     0x9000  /*     b          Nca  */
#define tagISSUER_SCRIPT_TEMPLATE_1  0x7100  /*     b          var  */
#define tagISSUER_SCRIPT_TEMPLATE_2  0x7200  /*     b          var  */
#define tagRESPONSE_MSG_TEMPLATE1    0x8000  /*    var         var  */
#define tagRESPONSE_MSG_TEMPLATE2    0x7700  /*    var         var  */
#define tagSFI                       0x8800  /*     b            1  */
#define tagSIGNED_STATIC_APPL_DATA   0x9300  /*     b           Ni  */
#define tagTVR                       0x9500  /*     b            5  */
#define tagTRACK2_EQUIVALENT_DATA    0x5700  /*     b          -19  */
#define tagTDOL                      0x9700  /*     b         -252  */
#define tagTC_HASH_VALUE             0x9800  /*     b           20  */
#define tagTRAN_DATE                 0x9A00  /*     n 6          3  */
#define tagTRAN_PIN_DATA             0x9900  /*     b          var  */
#define tagTRAN_TYPE                 0x9C00  /*     n 2          1  */
#define tagTSI                       0x9B00  /*     b            2  */
#define tagAEF_DATA_TEMPLATE         0x7000  /* var              */

#define tagACQUIRER_ID               0x9F01  /*     n            6  */
#define tagADDIT_TERML_CAPABILITIES  0x9F40  /*     b            5  */
#define tagAUTH_AMOUNT_NUM           0x9F02  /*   n12            6  */
#define tagOTHER_AMOUNT_BIN          0x9F04  /*     b            4  */
#define tagOTHER_AMOUNT_NUM          0x9F03  /*   n12            6  */
#define tagAMOUNT_IN_REF_CURRENCY    0x9F3A  /*     b            4  */
#define tagAPPL_CRYPTOGRAM           0x9F26  /*     b            8  */
#define tagAPPL_CURRENCY_CODE        0x9F42  /*     n 3          2  */
#define tagAPPL_CURRENCY_EXPONENT    0x9F44  /*     n 1          1  */
#define tagAPPL_DISCRETIONARY_DATA   0x9F05  /*     b         1-32  */
#define tagAPPL_EFFECTIVE_DATE       0x5F25  /*     n 6          3  */
#define tagAPPL_EXPIRATION_DATE      0x5F24  /*     n 6          3  */
#define tagAID_TERMINAL              0x9F06  /*     b         5-16  */
#define tagAPPL_PREF_NAME            0x9F12  /*    ans** 1-16    1-16  */
#define tagPAN_SEQUENCE_NUMBER       0x5F34  /*     n 2          1  */
#define tagAPPL_REF_CURR             0x9F3B  /*     n 3        2-8  */
#define tagAPPL_REF_CURR_EXPONENT    0x9F43  /*     n 1        1-4  */
#define tagATC                       0x9F36  /*     b            2  */
#define tagAPPL_USAGE_CONTROL        0x9F07  /*     b            2  */
#define tagCARD_APPL_VERSION         0x9F08  /*     b            2  */
#define tagTERM_APPL_VERSION         0x9F09  /*     b            2  */
#define tagCARDHOLDER_NAME           0x5F20  /*    ans 2-26   2-26  */
#define tagCRDHLDR_NAME_EXT          0x9F0B  /*    ans 27-45 27-45  */
#define tagCVM_RESULT                0x9F34  /*     b            3  */
#define tagCERT_AUTH_PKI_TERMINAL    0x9F22  /*     b            1  */
#define tagCRYPTOGRAM_INFO           0x9F27  /*     b            1  */
#define tagDATA_AUTH_CODE            0x9F45  /*     b            2  */
#define tagDDOL                      0x9F49  /*     b         -252  */
#define tagFCI_ISSUER_DISCR          0xBF0C  /*    var        -222  */
#define tagICC_DYNAMIC_NUMBER        0x9F4C  /*     b          2-8  */
#define tagPIN_PK_CERTIFICATE        0x9F2D  /*     b           Ni  */
#define tagPIN_PK_EXPONENT           0x9F2E  /*     b          1,3  */
#define tagPIN_PK_REMAINDER          0x9F2F  /*     b    Npe-Ni+42  */
#define tagICC_PK_CERTIFICATE        0x9F46  /*     b           Ni  */
#define tagICC_PK_EXPONENT           0x9F47  /*     b          1-3  */
#define tagICC_PK_REMAINDER          0x9F48  /*     b    Nic-Ni+42  */
#define tagIFD_SERIAL_NUM            0x9F1E  /*    an            8  */
#define tagISSUER_ACTION_DEFAULT     0x9F0D  /*     b            5  */
#define tagISSUER_ACTION_DENIAL      0x9F0E  /*     b            5  */
#define tagISSUER_ACTION_ONLINE      0x9F0F  /*     b            5  */
#define tagISSUER_APPL_DATA          0x9F10  /*     b          -32  */
#define tagISSUER_CODE_TABLE_INDEX   0x9F11  /*     n 2          1  */
#define tagISSUER_COUNTRY_CODE       0x5F28  /*     n 3          2  */
#define tagISSUER_SCRIPT_ID          0x9F18  /*     b            4  */
#define tagISSUER_PK_EXPONENT        0x9F32  /*     b          1,3  */
#define tagISSUER_URL                0x5F50  /*     ans         var  */
#define tagLANGUAGE_PREFERENCE       0x5F2D  /*    an 2        2-8  */
#define tagLAST_ONLINE_ATC_REGISTER  0x9F13  /*     b            2  */
#define tagLOG_ENTRY                 0x9F4D  /*     b            2  */
#define tagLOG_FORMAT                0x9F4F  /*     b          var  */
#define tagMIN_CONS_OFFLINE_LIMIT    0x9F14  /*     b            1  */
#define tagMERCH_CATEGORY_CODE       0x9F15  /*     n            2  */
#define tagMERCH_ID                  0x9F16  /*   ans           15  */
#define tagMERCH_NAME_LOCATION       0x9F4E  /*   ans          var  */
#define tagPIN_TRY_COUNTER           0x9F17  /*     b            1  */
#define tagPOS_ENTRY_MODE            0x9F39  /*     n2           1  */
#define tagPDOL                      0x9F38  /*     b          var  */
#define tagSERVICE_CODE              0x5F30  /*     n 3          2  */
#define tagSIGNED_DYNAMIC_APPL_DATA  0x9F4B  /*     b          Nic  */
#define tagSTATIC_DATA_AUTH_TAG_LIST 0x9F4A  /*    --          var  */
#define tagTERML_CAPABILITIES        0x9F33  /*     b            3  */
#define tagTERML_COUNTRY_CODE        0x9F1A  /*     n            2  */
#define tagTERML_FLOOR_LIMIT         0x9F1B  /*     b            4  */
#define tagTERML_ID                  0x9F1C  /*    an            8  */
#define tagTERML_RISK_MANAG_DATA     0x9F1D  /*     b          1-8  */
#define tagTERML_TYPE                0x9F35  /*     n            1  */
#define tagTRACK1_DISC_DATA          0x9F1F  /*    ans         var  */
#define tagTRACK2_DISC_DATA          0x9F20  /*     cn         var  */
#define tagTRAN_CURR_CODE            0x5F2A  /*     n 3          2  */
#define tagTRAN_CURR_EXPONENT        0x5F36  /*     b            1  */
#define tagTRAN_REF_CURR_CODE        0x9F3C  /*     n 3          2  */
#define tagTRAN_REF_CURR_EXPONENT    0x9F3D  /*     n 1          1  */
#define tagTRAN_SEQUENCE_COUNTER     0x9F41  /*     n 4-8      2-4  */
#define tagTRAN_TIME                 0x9F21  /*     n 6          3  */
#define tagUNPREDICTABLE_NUMBER      0x9F37  /*     b            4  */
#define tagMAX_CONS_OFFLINE_LIMIT    0x9F23  /*     b            1  */

#define     ICC_OK				     0
#define     ICC_SOCKET_EMPTY         1
#define     ICC_BAD_CARD             2
#define     ICC_NEEDS_RESET          3
#define     ICC_SOFTWARE_ERROR       4
#define     ICC_CARD_BLOCKED         5
#define     ICC_NO_ATR               6     /* card didn't response to ATR at all (Maybe Vcc is incorrect) */
#define     ICC_ATR_TOUT             7     /* timeout after any ATR byte  */
#define     ICC_ATR_TCK_ERR          8     /* ATR checksum is bad or not exist */
#define     ICC_PPS_NOT_NEED         9     /* doesn't need PPS after ATR */
#define     ICC_IFS_NOT_NEED         10    /* doesn't need IFS after ATR */
#define     ICC_POWER_FAIL           11    /* smart card VCC is OFF */
#define     ICC_PARITY_ERROR         12    /* parity error in any byte */

// Amount Formatting Options.															
#define BCD_IN				0x01														
#define ASCII_IN			0x02														

#define CPUHW_BATSYS_CHARGE         0    /* Battery is charging    */
#define CPUHW_BATSYS_DISCHARGE      1    /* Battery is discharging */
#define CPUHW_BATSYS_NO_CHARGE      2    /* Battery is idle        */
#define CPUHW_BATSYS_NO_BAT         3    /* Battery does not exist */

//PAX Printer 
#define PRN_NO_PAPER	2
#define PRN_DATA_ERR	3
#define PRN_ERR			4
#define PRN_NO_DOT		0xFC
#define PRN_DATA_BIG	0xFE
#define PRN_CANCEL		0xA1


#endif



