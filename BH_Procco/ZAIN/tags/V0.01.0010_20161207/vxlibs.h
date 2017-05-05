/*!
  @file
  \brief Utility Library
*/

#ifndef _VXLIBS_H_
#define _VXLIBS_H_
#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.
#include "emvlib.h" // use the path that contains the PAX EMV lib files, but don't copy them to project folder.

#include "os_interface.h"
#include "Farsi.h"
#include "fstruct.h"
#include "lng.h"
#include "debug.h"

#include "os_define.h"
#include "OS_TYPES.h"
#include "os_interface.h"

#include "defines.h"
#include "struct.h"
#include "message.h"

#include "commsdlm_vx.h"
#include "ucl_main.h"
#include "pax_migrate.h"
#include "PUB_SC.h"
extern int h_clock;


#define ENXIO      6    /*  No such device */
#define ENOSPC     28   /*! No space left or other write error on device */ 
#define KEY_CANCEL  27
#define INVALID_PRN_TYPE		-22
#define VXLIB_ERRNO				0
#define KEYIO_ALIGN_LEFT        0
#define KEYIO_ALIGN_RIGHT       1
#define KEYIO_ALIGN_CENTER      2

#define LG_GETKEYBOARD			24


#define MSG_DONT_CLEAR_SCR		0x01
#define YES_NO_MODE					0x02

// NURIT //////////////////////////////////////////////////////////////////////////////////////////
/*
#define ON                   1
#define OFF                  0
#define PASS                 1
#define FAIL                 0
#define OK                   1
#define FAILURE             -2 */
#define WARNING             -1

/* Control bytes for Printer_SetMode function */
#define STANDART_HIGH_STRING  0
#define PRINTER_BLOCKING_OUT_OF_PAPER  1 /* If out of paper block terminal */
/*#define TOTAL_RED_ENABLE    1*/  /* only one control byte  */
/*#define TOTAL_RED_DISABLE   2*/ /* only one control byte  */
#define RED_STRING            3  /* only one control byte  */
#define TURN_TO_RED           4  /* only one control byte  */
/*#define TURN_TO_BLACK       5*/  /* only one control byte  */
#define STRING_FORMAT_SET     6  /* 3 bytes: control byte,LineWidth,LeftMargin*/
#define DOUBLE_HIGH_STRING    7  /* FOR FUTURE USE */
#define ONE_AND_HALF_HIGH_STRING   0x15
/*#define START_CONDENSED       9*/  /* FOR FUTURE USE */
/*#define STOP_CONDENSED        10*/ /* FOR FUTURE USE */
#define CHANGE_FONT           11 /* Change byte ,FontType */
#define CHANGE_LANG           12 /* Change byte, Language */
#define UART_A                    0     /* index of 1st UART */
/*          control   characters            */
#define START_HALF_CUTER     0x0f /* only one control byte  */
#define START_CUTER          8  /* only one control byte  */
#define WIDE_CH              0x0E       /* large  size characters               */
#define NORM_CH              0x14       /* normal size characters (default)     */
#define GRAPHICS             0x1F       /* next byte is graphics                */
// NURIT //////////////////////////////////////////////////////////////////////////////////////////
						
// Amount Formatting Options.															
#define BCD_IN				0x01														
#define ASCII_IN			0x02														
#define YTL_STYLE			0x00														
#define TL_STYLE			0x01														

// ShowMessage() Function Options.														
#define MESSAGE_MODE		0x00														
#define WARNING_MODE		0x01														
#define ERROR_MODE			0x02														
#define HEADER_MODE			0x03														
#define BUZZER_OFF			0x10

// GLOBAL VARIABLES DEFINED IN THIS MODULE												
#define LG_CSTMSG	 43																	


typedef struct {																		
	char CSTMSG1[LG_CSTMSG];	
	char CSTMSG2[LG_CSTMSG];	
	char CSTMSG3[LG_CSTMSG];	
	char CSTMSG4[LG_CSTMSG];	
	char CSTMSG5[LG_CSTMSG];	
	char CSTMSG6[LG_CSTMSG];	
	char CSTMSG7[LG_CSTMSG];	
	char CSTMSG8[LG_CSTMSG];	
	char CSTMSG9[LG_CSTMSG];	
	char CSTMSG10[LG_CSTMSG];	
	char CSTMSG11[LG_CSTMSG];	
} MSG_Struct;
																			
extern MSG_Struct MainMSG;																		

// Functions.
void ClearDsp(void); 
BOOLEAN HexDump(void *buf, usint buf_size); 
//byte InputAmount(MSG_Struct *stMSG,byte *RetBuf, BOOLEAN ZeroInputOK);
BYTE InputAlphaNum(MSG_Struct *stMSG,char *RetBuf, BYTE MinLen, BYTE MaxLen, BYTE AlphaOk, BYTE MaskOk, BYTE InputAlignment, BYTE TOSeconds); 
BYTE InputAlphaNum_PAX(MSG_Struct *stMSG, char *RetBuf, BYTE MinLen, BYTE MaxLen,
                       BYTE AlphaOk, BYTE InputAlignment, BYTE TOSeconds);

void ShowScreen(MSG_Struct *stMSG, BYTE Line2Font, BYTE Line3Font, BYTE Line4Font, BYTE Line5Font,BYTE ShowByte); 
//	ShowMenu OptByte;	Descriptions											
#define Opt_INVERSE			0x01	// 01 - INVERSE SELECTED					
#define Opt_SKIPSINGS		0x02	// 02 - SKIP SINGS ON SELECT <>				
#define Opt_LONGMSG			0x04	// 04 - DISPLAY 21 CAHR WITH INVERSE FONT	
#define Opt_REMAPTR			0x08	// 08 - CONVERT MESSAGE TO TURKISH CHARS	
#define Opt_FIVE_SUBITEM	0x10	// 10 - SHOW 5 SUBITEMS PER PAGE			
#define Opt_TYPE8000		0x20	// 20 - POS TYPE 8000 						
int ShowMenu(menu *m, BYTE TOSeconds, BYTE OptByte); 
BYTE ShowMessage(MSG_Struct *stMSG,BYTE TOSeconds, BYTE Mode, BYTE OptByte,BYTE Arabic,BYTE SMALL_FONT_MAX);

BYTE GetOneKey(BYTE TOSeconds); 
void GetRandomNumber(BYTE *pDest); 

void InitUart(usint BaudRate, BYTE DataBits, char Parity, BYTE CommPort); 
void Debug_SetFont(BYTE FontType); 
void Debug_GrafPrint2(char *, BYTE); 
void PrintImage(BYTE *cPrintBuf, int MargenIzq, int Ancho, int NumLineas); 
void FormatAndPrint(char *String, BYTE Justify, BYTE FontSize, BYTE Length); 

BYTE InputAmountExt(MSG_Struct * stMSG, BYTE * RetBuf, BOOLEAN ZeroInputOK, sint minlen, sint maxlen, char *CurDes);	

BYTE InputInvoiceNum(MSG_Struct * stMSG, unsigned char *pOutString);  //added by Allen for ZAIN 2016-05-16
BYTE InputPINNum(MSG_Struct * stMSG, unsigned char *pOutString);  //added by Allen 2016-05-24
BYTE InputReferenceNum(MSG_Struct * stMSG, unsigned char *pOutString);  //added by Allen 2016-05-24
void FormatPoints(char *pDest, char *pRes);  // added by Allen 2016-07-12
void GetTotalBalance(char *pPointsBlance);   //added by Allen 2016-07-13
void FormatAmt(char *pDest, char *pBuf, char *CurDes, BYTE InputType);


// VERIFONE CONVERT FUNCTIONS /////////////////////////////////////////////////////////////////////
sint Printer_WaitForGraphicsReady(void);
void Cpuhw_SwitchTo(usint device);
void Display_ClrDsp(void);
//sint Uart_Init(usint UartIndx, comm_param *UartParam);
sint Uart_Flush(usint UartIndx);
void Printer_LineSpacing(BYTE space, BOOLEAN fast);
void PrntUtil_UpperCase(void);
#define LG_LINE	42
void AdjustAndPrint(char *fpStr1, int fbLen1, char *fpStr2, int fbLen2);
void PrntUtil_Cr( int n );
void ApplMngr_RestartAppl (void);
void PrntUtil_Int(int Data, char*  Buf);
ULONG Scan_MillSec(void);
void GrphText_Cls( BOOLEAN DisplayIt );
void GrphText_InvertLine(BYTE Line,BOOLEAN DisplayIt);
void GrphDisp_SetPixel(BYTE row, BYTE col,BYTE Toggle);
void GrphText_DrawDisp( void );
void GrphText_ClrLine(BYTE Line,BOOLEAN  DisplayIt);
void GrphText_Write(BYTE Line,sint Offset,char *data,BYTE FontType,BYTE length,BOOLEAN DisplayIt,BOOLEAN  RevVideo);


// VERIFONE CPECIFIC /////////////////////////////////////////////////////////////////////
int OpenPinter (void);
int ClosePrinter(void);
int OpenDisplay (void);
void new_despmsg(BYTE *bp_mensaje, int i_alineacion,int i_ren, int i_clr);
int inSetFontFile (const char *pszFontFile);
void DisplayLogo(void);
void TimeShow(void);
void Display_Logo( void );
void Print_Logo( void );
void Display_Inverst(char *gelen,char clr_flag);
int Print_Main(void);
int Pinpad_text(void);
void PinPadGetSerial(void);
void p300_bprint(int h_printerw, char *prt_str);
void IppInit(void);

int TestPrint_Hex(unsigned char *str,int len);



typedef struct
{
	BYTE b_len;
	BYTE b_col;
	BYTE b_row;
	BYTE bp_line[127];
} S_GRA;


#define	EX_CARD				0	
#define	SAM1				1
#define	SAM2				2
#define	SAM3				3
#define	SAM4				4
#define	MAX_SMART_PORT_NUM	5

/* first 30 definitions from UNIX */ 
#define EPERM			1		/*!< \brief Operation not permitted */ 
#define ENOENT			2		/*!< \brief No such file or directory */ 
#define ESRCH			3		/*!< \brief No such process */ 
#define EINTR			4		/*!< \brief Interrupted system call */ 
#define EIO				5		/*!< \brief I/O error */ 
#define ENXIO			6		/*!< \brief No such device or address */ 
#define E2BIG			7		/*!< \brief Argument list too long */ 
#define ENOEXEC			8		/*!< \brief Exec format error */ 
#define EBADF			9		/*!< \brief Bad file number */ 
#define ECHILD			10		/*!< \brief No child processes */ 
#define EAGAIN			11		/*!< \brief Try again */ 
#define ENOMEM			12		/*!< \brief Out of memory */ 
#define EACCES			13		/*!< \brief Permission denied */ 
#define EFAULT			14		/*!< \brief Bad address */ 
#define ENOTBLK			15		/*!< \brief Block device required */ 
#define EBUSY			16		/*!< \brief Device or resource busy */ 
#define EEXIST			17		/*!< \brief File exists */ 
#define EXDEV			18		/*!< \brief Cross-device link */ 
#define ENODEV			19		/*!< \brief No such device */ 
#define ENOTDIR			20		/*!< \brief Not a directory */ 
#define EISDIR			21		/*!< \brief Is a directory */ 
#define EINVAL			22		/*!< \brief Invalid argument */ 
#define ENFILE			23		/*!< \brief File table overflow */ 
#define EMFILE			24		/*!< \brief Too many open files */ 
#define ENOTTY			25		/*!< \brief Not a typewriter */ 
#define ETXTBSY			26		/*!< \brief Text file busy */ 
#define EFBIG			27		/*!< \brief File too large */ 
#define ENOSPC			28		/*!< \brief No space left on device */ 
#define ESPIPE			29		/*!< \brief Illegal seek */ 
#define EROFS			30		/*!< \brief Read-only file system */ 
#define EABORT			31		/*!< \brief operation aborted */ 
#define ESUPP			32		/*!< \brief function/command not supported */ 
#define EFRAME			33		/*!< \brief character framing error (serial) */ 
#define EBSIZE			34		/*!< \brief wrong buffer size */ 
#define ESEEK			35		/*!< \brief wrong seek position */ 
#define ENOLINE			36		/*!< \brief no line = no dial tone */ 
#define EINUSE			37		/*!< \brief line in use */ 
#define ENOCARR			38		/*!< \brief no carrier */ 
#define ENOBASE			40		/*!< \brief no contact with base required for operation */ 
#define ESTATE			41		/*!< \brief Not correct state */ 
#ifndef ETIMEOUT
#define ETIMEOUT		42		/*!< \brief Timeout occured */ 
#endif
#define EEOT			43		/*!< \brief EOT received */ 
#define ENAK			44		/*!< \brief NAK received */ 
#define EENQ			45		/*!< \brief ENQ received */ 
#define RETX			46		/*!< \brief ETX received at the end of packet*/ 
#define RETB			47		/*!< \brief ETB received at the end of packet*/ 
#define REOT			43		/*!< \brief EOT received at the end of packet*/ 
#define ENOFS			51		/*!< \brief no file system */ 
#define ENOREC			52		/*!< \brief no record */ 
#define EBADFILE		53		/*!< \brief file has wrong data (corrupted) */ 
#define EFILEEXISTS		54		/*!< \brief file alreay in ramdisk W/ correct version and CRC */ 
#define EREADER			60		/*!< \brief wrong reader used */ 
#define ENOPOWER		61		/*!< \brief card not powered */ 
#define EBADCARD		62		/*!< \brief card is not ICC */ 
#define ECLASS			63		/*!< \brief wrong class byte */ 
#define EINSTR			64		/*!< \brief wrong instruction byte */ 
#define EPARAM			65		/*!< \brief wrong P1 or P2 value */ 
#define ECRYPT			66		/*!< \brief wrong cryptogram presented */ 
#define ENOSIM			71		/*!< \brief no sim in socket */ 
#define EBADPIN			72		/*!< \brief PIN code required */ 
#define EBADPUK			73		/*!< \brief PUK code required */ 
#define EFALLBACK		80		/*!< \brief EMV error, fallback mode */ 
#define EAPPPROC		81		/*!< \brief EMV application processing error */ 
#define EAMOUNT			82		/*!< \brief EMV enter amount error */ 
#define ECARDHDLR		83		/*!< \brief EMV card holder verification aborted */ 
#define ESTACK			84		/*!< \brief stack overflow error */ 
#define EPARSE			86		/*!< \brief Parsing error. */ 
#define ECANCEL			87		// Go back one menu degree 
#define ENOHANDSHAKE	90 
#define ENOANSWER		91 
#define ENODIALTONE		92 
#define ENORINGBACK		93 
#define EWAITFORRING	94 
#define EASKPARAM   95 

#define EASKPIN	    9997	 
#define ENEEDUPLOAD	9998	 
#define EOTHER			9999	/*!< \brief other error */ 
#define ETXTBSY			26		/*!< \brief Text file busy */ 
#define HOST_NO 10 
#define CONN_NO 20 
#define DOWNLOAD_SUCCESS 0x10 
#define MODENCC_AUTO		0 
#define MODENCC_PARAM		1 
#define MODENCC_APP		2 
#define MODENCC_APPPAR	3 
#define MODENCC_NOS		4 

#define		EOT		0x04	// End Of Transmission


#define	ICC_OK				          0

#define      ICC_SOCKET_EMPTY         1
#define      ICC_BAD_CARD             2
#define      ICC_NEEDS_RESET          3
#define      ICC_SOFTWARE_ERROR       4
#define      ICC_CARD_BLOCKED         5
#define      ICC_NO_ATR               6     /* card didn't response to ATR at all (Maybe Vcc is incorrect) */
#define      ICC_ATR_TOUT             7     /* timeout after any ATR byte  */
#define      ICC_ATR_TCK_ERR          8     /* ATR checksum is bad or not exist */
#define      ICC_PPS_NOT_NEED         9     /* doesn't need PPS after ATR */
#define      ICC_IFS_NOT_NEED        10     /* doesn't need IFS after ATR */
#define      ICC_POWER_FAIL          11     /* smart card VCC is OFF */
#define      ICC_PARITY_ERROR        12     /* parity error in any byte */



#define TIMER_EXPIRED		0	

#define SECOND		100			//!< \brief secund definition (100)
#define SECOND2		(2L*SECOND)	//!< \brief 2 secund definition
#define SECOND3		(3L*SECOND)	//!< \brief 3 secund definition
#define SECOND5		(5L*SECOND)	//!< \brief 5 secund definition
#define SECOND10	(10L*SECOND)	//!< \brief 5 secund definition
#define MINUTE		(60L*SECOND)	//!< \brief minute definition
#define HOUR		(60L*MINUTE)	//!< \brief hour definition
#define MAX_BUF_SIZE 1024	/*!< \brief Size of IO buffer */ 
// short fldLength = sizeof(unsigned short);

extern int h_print_port;
extern int h_com_port;

extern unsigned char emv_off_pin_try;
extern unsigned char emv_off_pin_entry;

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL (void*)0
#endif
#endif

void display_format(int y,char *msg_str,int align);

extern char EmvKeyFile[];
extern char EmvAppFile[];
extern char EmvParFile[];
extern char KeyTmpFile[];
extern char EmvMVTFile[];
extern char EmvESTFile[];

//int i_display_graphic(unsigned char *bp_graphic_name,int b_clr,int i_col,int i_row);
int getpan(char *msg,char *szPan, int maxSize);
void SetPrinterDoubleWidth(void);
void SetPrinterNormal(void);
void SetPrinter24Columns(void);
void SetPrinter42Columns(void);
BYTE AskYesNo(MSG_Struct *stMSG, BYTE Type,BYTE offset);
BYTE GetOneKeyOne(void);
#endif	// _VXLIBS_H_
