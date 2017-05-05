#ifndef _DEFINES_H_
#define _DEFINES_H_


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!! HERE DETERMINES THE CURRENCT COMPILE TARGET : CPA OR VSDC !!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ONLY ONE OF BELOW IS ENABLED
#define APP_COMPILE_CPA
//#define APP_COMPILE_EMV

#pragma warning(disable: 4101)  // Added by Kim_LinHB 2013-3-11 build 0.05.0009

// Added by Kim_LinHB 2013-10-8 0.05.0011
#define APP_VERSION_1 "01"
#define APP_VERSION_2 "05"
// Add End

#include "PUB_SC.h"

#define ENGLISH           1
#define ARABIC            2


#define EMV_SATIS         1
/************************************************************************/
/*                          vx start                                    */
/************************************************************************/

#define  LG_FILENAME	  4




#define STR_CENTER		-1




/************************************************************************/
/*                       vx end                                         */
/************************************************************************/





//Menuler
#define INSTALLMENT_PLAN         2
#define PAYMENT                  3
#define LOYALTY                  4
#define ACCOUNT_MAINT            5
#define BALANCE_LIST  			 6
#define LAST10TRANSACTION		 7
#define SECOND_COPY		         8
#define BATCH_CLOSE              9
#define PARAMETER               0x0a
#define BATCH_SLIP_COPY			0x0b
#define LISTS   				0x0c
#define SECURITY_FUNCT			0x0d
//#define REVERSAL                14
//#define RECONCILIATION          15
#define CANCEL						  14
#define TENURE_CALCULATION		15
//
#define SOH           0x01	     /*  1 */
#define STX           0x02	     /*  2 */
#define ETX           0x03	     /*  3 */
#define EOT           0x04	     /*  4 */
#define ENQ           0x05	     /*  5 */
#define ACK           0x06	     /*  6 */
#define NAK           0x15	     /* 21 */
#define DISCONNECT    0x53	     /* 83 */


#define BITSPERBYTE	(8*sizeof(BYTE))


#define SWAPWORD(Word) (((Word>>BITSPERBYTE)&(0x00FF))|((Word<<BITSPERBYTE)&(0xFF00)))

#define SWAPLONG(Long) (((Long>>(3*BITSPERBYTE))&(0x000000FF))| \
								((Long>>BITSPERBYTE)&(0x0000FF00))|     \
								((Long<<BITSPERBYTE)&(0x00FF0000))|     \
								((Long<<(3*BITSPERBYTE))&(0xFF000000)))
// Select
#define PF_LG_AUTH_DATA			         6
#define PF_LG_CHALLENGE				      8
#define PF_LG_CHV_KEY_VALUE		      8
#define PF_LG_SERIAL_NUMBER		      8
//#define PF_LG_OTHER_RESP	          255
#define PF_LG_SERIAL_NUMBER		      8

#define PF_NEXT_MODE				      0x02
#define PF_ABSOLUTE_MODE		      0x04


#define LG_AMOUNT		12
#define LG_MAXTOTAL	   LG_AMOUNT + ((LG_AMOUNT/3)-1)
#define NBTOTALS 10

#define PAD_NONE         0x00
#define PAD_LEFT         0xAA
#define PAD_RIGHT        0x55




#define LG_PIN 				8		// Length of the pn for the swipe card

#define DECLINED			0
#define NOTDECLINED		   	1

#define MAX_NUMBER 			3
//-----------------------------------------------------------------------
//-----                    FILE LENGTHS                             -----
//-----------------------------------------------------------------------
#define CNT_BTICKETLINE 	    40     // Batch Close Ticket line count
#define LG_RBFILE    		    66 * LG_PAX_TKLINE

#define NBLINEENDOFSLIP			 5
#define MAX_CALL_ATTEMPT		 3
#define CALL_ATTEMPT_INTERVAL	 3	/* mn */
//----- CardBinFlags mask -----------------
#define DISPLAY					 0x10
//------  communication flags  ----------
#define WAIT_MODE              0x00
#define NO_WAIT_MODE           0x01

#define FONT0			             1
#define cwTIMEOUT     	       1000// 50 Seconds timeout for keyboard entries

#define LG_TRACKISO2           41
#define LG_PAN			          20
#define LG_TICKETLINE		    24
#define LG_PRN_BUFFER           64*1024

#define LG_PAX_TKLINE			28

#define LG_TRANSDATE			    6 // DDMMYY
#define LG_TRANSTIME			    4 // HHMM
#define LG_DATETIME		       12
#define LG_DATE				    6

#define LG_TIME					6


#define LG_MERCHID		       12
#define LG_TERMID			    8
#define LG_STORENUM			    8
#define LG_SERIALNUM	        10

#define LG_SEQNUM				    4
#define LG_TRANSTYPE			    2
#define LG_RESPONSECODE			 2

#define LG_PHONE			       14
#define LG_PARMVERSION	   	 3
#define LG_BATCHNUM		   	 3
#define LG_AUTHRESPONSE		    16 // 40
#define LG_MERCHDATA		       96


#define LG_APN				24		//modified by Kevin v0.05.0015 20150616
#define LG_USER				24		//added by Kevin v0.05.0016 20150617
#define LG_PASSWORD			24		//added by Kevin v0.05.0016 20150617
#define LG_IP					16

#define STAT_NO_CONNECTION 	0xC4

#define STAT_DIAL_PROBLEM		0x05
#define STAT_EOT_RECEIVED		0x06
#define STAT_BUSY					0x07
#define STAT_NO_ENQ				0x08


// Constants for : Precision
#define MAX_COMM_RETRY       3     // maximum number of communication
#define MAX_RECV_TIMER      20     // seconds
#define PRECISION_NULL      0x00

// Constants for wake up alarm
#define ALARM_BATCHCLOSE    	0x01//0x0F
#define ALARM_PARAM_DOWNLOAD	0x02//0xF0

//Negatif status
#define STAT_CANCEL			0x8001
#define STAT_TIMEOUT			0x8002
#define STAT_GARBAGE			0x8003
#define STAT_FORCAGE       0x6002
#define STAT_EOT			   0x01

// gbTermFlags values
#define TERMINALINLAN  		0x40   // Terminal is in LAN terminal mode
#define TERMINALINGPRS  		0x80   // Terminal is in LAN terminal mode


#define STAT_NOK		0x8000
#define STAT_NO_PAPER		0x8002
#define STAT_INV	   	0x04
#define STAT_INIT_OK		0x80
#define STAT_EOF		0x20
#define STAT_MERCH_NOK		0x80
#define STAT_PARAM_NOK		0x81

//Smart Kart Yetkileri 1
#define SC_TERMINAL_ACTIVE    0x01
//#define SC_EXPDATE_CONTROL    0x02
#define SC_MANUAL_CARD_ENTRY  0x04
//Smart Kart Yetkileri 1
#define SC_SECOND_COPY_PRINT  0x01

#define SC_CASHIER_TRANS      0x01
#define SC_PIN_TRANS          0x02

#define PARAM_HOST            1
#define AUTH_HOST             2
#define PMS_HOST              3

#define EMV_CARD              1

#define ISO_PAYMENT                   0
#define ISO_ACCOUNTMAINTENANCE        1
#define ISO_ACCOUNTMAINTENANCE_ADVICE 2
#define ISO_REVERSAL                  3
#define ISO_RECONCILIATION            4
#define ISO_BATCH_UPLOAD_ADVICE       5
#define ISO_INSTALLMENT_ADVICES       6
#define ISO_LOYALTY_ADVICE            7
#define ISO_RECONCILIATION_ADVICES    8
#define ISO_GETBALANCE        9
#define ISO_RETAIL        10



#define MANUAL_ENTRY           0
#define CHIP_READ              0

extern BYTE gLanguage;       // Current phone's sequance number like 0,1,2
//Global
extern BYTE CommandBuf[60];
extern SWORD sw1sw2;

extern BYTE gbFLagPartialInitState;		//Set if the application is not
													//totally initialized.
extern BYTE gbTermFlags;				   //Terminal flags
extern BYTE gOldItem;
extern BYTE gOldSubItem;

// --- File Handles ---
extern int  gTrHandle;
extern BYTE gATRString[30];

extern BYTE gOperationFlagsSmart1;
extern BYTE gOperationFlagsSmart2;
extern BYTE gMerchantOperationFlags;
extern WORD gMaxTranaction;
extern BYTE gHostType;

extern SC_STATUS scStatus;
extern BYTE gDebug;
extern BYTE gDebugType;
extern BYTE gCardType;
extern BYTE gtransactionType;
extern BYTE gHardwareType;
extern BYTE gCurrencySymbol[3];
extern BYTE gCurrencyExponent;
extern BYTE gEntryType;
extern BYTE gPIN[18];
//extern BYTE gMacKey[9];
//extern BYTE gPinKey[9];

extern BYTE gAmountDebit[LG_AMOUNT+1];
extern BYTE gTempBuf[LG_AMOUNT+1];

extern BYTE gAmountLoyalty[LG_AMOUNT+1];
extern BYTE gAmountPayment[LG_AMOUNT+1];
extern BYTE gAmountGeneral[LG_AMOUNT+1];
extern BYTE gAmountGeneralWithPayment[LG_AMOUNT+1];
extern BYTE gCounterDebit[3];
extern BYTE gCounterLoyalty[3];
extern BYTE gCounterPayment[3];
extern BYTE gCounterGeneral[3];
extern BYTE gCounterGeneralWithPayment[3];
extern BYTE gField55[255];
extern BYTE gField55Length;
extern BYTE gKeyCancel;
extern BYTE gNETSERVERNII[3];
extern BYTE gPMSNII[3];
extern BYTE gTMSNII[3];


extern unsigned char selectionCompleted;

extern unsigned char bBalanceFromOptions;	// Added by Kim_LinHB 2013-3-8 build 0.05.0009

typedef struct 
{
	PCHAR Item[40];
}MENU;

#define MESSAGE_MODE		0x00	
#define WARNING_MODE		0x01	
#define ERROR_MODE			0x02	
#define HEADER_MODE			0x03	

// BUZZER TYPES						
#define BUZZER_BEEP				100	
#define BUZZER_WARN				300	
#define MSG_DONT_CLEAR_SCR		0x01
#define YES_NO_MODE					0x02


#define ARC_NO_CONNECTION                    0xFF  /* Indicates that terminal is unable to go online */
#define ARC_APPROVED                         0x00
#define ARC_DECLINED                         0x51

#define APPL_AID        "\xA0\x00\x00\x04\x31"
#define APPL_AID_LONG   "\xA0\x00\x00\x04\x31\x00\x01"
#define APPL_TEST

//#define APPL_IP   // USE Ethernet leeDebug
#define APPL_ARABIC
//#define APPL_DEMO  //richard test balance
//#define APPL_CARD

/*#define APPL_AID "\xA0\x00\x00\x00\x04"
#define APPL_AID_LONG "\xA0\x00\x00\x00\x04\x10\x10"*/

/*#define APPL_AID "\xA0\x00\x00\x00\x03"
#define APPL_AID_LONG "\xA0\x00\x00\x00\x03\x10\x10"*/

//add by richard to support the application exit
#define SELF_ENTRY   0
#define MAINAPP_ENTRY 1
extern char gAppEntryMothod;



#endif
