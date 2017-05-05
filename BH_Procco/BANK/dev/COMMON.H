/*
*%-----------------------------------------------------------------------------
*% File........... : COMMON.H
*% Project........ : SIGMA PORTABLE
*% Created ....... : 05.09.1996
*% Author ........ :
*%                 : T.T.S.
*%                 : SCHLUMBERGER Technologies
*%                 : 50, Avenue Jean Jaures, 92542 Montrouge Cedex, FRANCE
*% Version ....... : 1.00
*% Module ........ : All
*% Sub-module .... : Common parameters
*% Description ... : Common constant defines
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%-----------------------------------------------------------------------------
*% $Revision: 1.1 $
*% $Archive:   Z:/Dev/Core/MCS/AGP2/AM/CORE/Include/COMMON.H_v  $
*% $Log: COMMON.H,v $
*% Revision 1.1  2012/01/06 01:48:07  yucong
*% *** empty log message ***
*%
*% Revision 1.1  2011/08/18 09:05:14  yucong
*% *** empty log message ***
*%
*% Revision 1.3  2010/12/20 07:45:37  hejj
*% *** empty log message ***
*%
*% Revision 1.1  2010/12/07 07:33:30  hejj
*% 2010-12-7 15:32 by Gary
*%
* 
*    Rev 1.1   Nov 30 2001 11:52:28   meyer
* Added ECR_NUM define
* 
*    Rev 1.0   01 Mar 2001 14:57:18   kersaho
* First Release
*
*    Rev 1.3   29 Oct 1999 15:18:46   Toma
* Fixed bug :when removed SPP library
*
*    Rev 1.2   Oct 29 1999 14:09:32   sfwext6
*
*
*    Rev 1.1   06 Oct 1999 11:59:28   Toma
* Added AM_GetPPConfig prototype
*
*    Rev 1.0   Apr 06 1999 20:00:00   BRCAROTHERS
* Checkin of v2.16 files for new LAN configuration.
*
*    Rev 1.10   11 Sep 1998 16:29:46   DORF
* Multi-Currency/Mutli-Merchant
*
*    Rev 1.9   22 May 1998 15:07:44   DORF
* Return TipFlag from GetConfig2()
*
*    Rev 1.8   15 May 1998 17:15:02   LOPEZ
* Cleared compiler warnings.
*
*    Rev 1.7   23 Apr 1998 15:12:22   dave
* Updated header
*%-----------------------------------------------------------------------------
*/
#ifndef _COMMON_H
#define _COMMON_H

//  #include "os_ser.h"
#include "typedef.h"

#define PINPAD_PORT PORT_4      //The COMM port on which is connected
//the Smart PinPad
#define PINPAD_SCREEN_COLUMNS 16  //The Smart Pinpad screen size
#define SMARTPINPAD_SCREEN_SIZE 32
extern OS_WORD answerPP;       //Answer to the PP_Check request

/* $HeaderFileStart --------------------------------------------------------*/
/* $HeaderFileEnd ----------------------------------------------------------*/

/* $GlobalDataStart --------------------------------------------------------*/

// Constants for date and time
#define   LG_DATE         6      // Date
#define   LG_TIME         6      // Time HHMMSS
#define   LG_HHMM         4      // Time HHMM

// Constants for phone number
//#define   LG_PHONENUM      10      // Telephone Number (PAD, RTC number)
//#define   MIN_PHONENUM   1      // Telephone Number (PAD, RTC number)
#define   MAX_PHONENUM   15      // Telephone Number (PAD, RTC number)

// Constants for MDP number
#define   LG_NUMBER      14      // MDP X.25 address in DLSTRUCT

// Terminal defines
#define LG_PRTLINE      24      // 24 chars on a printer line

// Amount defines
#define LG_AMOUNT       12
#define LG_AMT_BCD       6

// File defines
#define LG_FILENAME      4

// Divers defines
#define SPACE          0x20

/* merchant messages */
#define LG_HEADER 24
#define LG_FOOTER 24
#define NUM_HEADER 5
#define NUM_FOOTER 2

// Amount configuration structure
typedef struct
{
    BYTE Separator;
    BYTE NbDeci;
    BYTE DeciPoint;
    BYTE LgMax;
    BYTE CurrencySymbol[4];          // ISO currency code, USD, FRF, etc.
} AMOUNT_CONFIG;


//---------------------------------------------------------------------------

//******************************************************************
// Ce qui suit, doit etre detruit (mis dans DSS ou AM):

#define LG_APPLTITLE   16
#define LG_APPLVERSION   2
// #define LG_APPLCHK      4

#define LG_DIALPREFIX   3    // Dial prefix
#define LG_MANUFACTURER 2    // Manufacturer
#define LG_HARDWARE     2    // Hardware type
#define LG_TERMNUM      2    // Terminal number
#define LG_ECRNUM       3    // Cash RegisterNumber for GIE CB
#define LG_SERIALNUM    10    // Terminal serialnumber

// These are in pub_os.h, remove soon
//#define DIAL_TONE      0x54
//#define DIAL_PULSE     0x50
//#define DIAL_NO_WAIT   0x01

typedef struct
{
    BYTE DialMode;                          // 'P'=pulse, 'T'=tone
    BYTE DialPrefix[LG_DIALPREFIX + 1];     // dial prefix ou ""
    BYTE Manufacturer[LG_MANUFACTURER + 1]; // Maker Id = "36"(Schlumberger)
    BYTE Hardware[LG_HARDWARE + 1];         // Hardware type = (Magic)
    BYTE TermNum[LG_TERMNUM + 1];           // Terminal number
    BYTE SerialNum[LG_SERIALNUM + 1];       // Terminal serial number
} TERMCONFIGSTRUCT;

#define LG_PREFIX    10
#define LG_PHONENUM  25
#define LG_POSTFIX   10
#define LG_ADDRESS   13
#define LG_SCENARIO   5

typedef struct
{
    char DialMode;                     // 'P'=pulse, 'T'=tone
    char DialPrefix[LG_PREFIX + 1];    // Before dialing phone number
    char DialPostfix[LG_POSTFIX + 1];  // After dialing phone number
    char filler[11];
} DIAL_CONFIG;

#define LG_AMVERSION    2               // Application Manager Version
typedef struct
{
    BYTE AMVersion[LG_AMVERSION + 1]; // Application Manager Version
    OS_WORD Currency;                  // currency type
} AMCONFIGSTRUCT;

typedef struct
{
    OS_WORD          VersionCore;                    // Core Version
    OS_WORD          VersionCustom;                  // Custom Version
    OS_WORD          VersionCustomID;                // CAM ID
    OS_WORD          Currency;                       // ISO Currency Code
    AMOUNT_CONFIG AmtCfg;                         // Amount configuration
    char          Language[2+1];                  // ISO Language code with, "en", "fr"
    BYTE          Header[NUM_HEADER][LG_HEADER+1];// Receipt headers
    BYTE          Footer[NUM_FOOTER][LG_FOOTER+1];// Receipt footers
    OS_WORD          DispFont;                       // Display font
    OS_WORD          MenuFont;                       // Display font for menus
    BYTE          TipFlag;                        // 0=false, 1=true
    BYTE          ServerID;                       // Current server ID
    BYTE          MerchantID;                     // Current merchant ID (deal)
    BOOLEAN       DoubleReceipt;                  // Printer paper type
    BYTE          filler[4];                      // RFU
} AMCONFIGSTRUCT2;


#ifdef __cplusplus
extern "C"
{
#endif
    BOOLEAN GetTerminalConfig( TERMCONFIGSTRUCT * fTermConfig);
    BOOLEAN AM_GetDialConfig( DIAL_CONFIG * fDialConfig);
    BOOLEAN GetAMConfig( AMCONFIGSTRUCT * fAMConfig);
    BOOLEAN GetAMConfig2( AMCONFIGSTRUCT2 * fAMConfig);
    BYTE AM_SmartPinPadMode();
    
#ifdef __cplusplus
}
#endif


//******************************************************************


// ----------------------------------------------------------------------
// Application Manager Status and Generic Library Status defines
// Used values of Application Manager status and Generic Library Status :
//                Positif status = 0x0000 to 0x1FFF
//                Negatif status = 0x8000 to 0x9FFF
// ----------------------------------------------------------------------

// Positif status
#define STAT_OK             0x0000
#define STAT_CLEAR          0x0001

// Negatif status
#define STAT_NOK            0x8000
#define STAT_CANCEL         0x8001
#define STAT_TIMEOUT        0x8002
#define STAT_PROGERROR      0x8003
#define STAT_INVALID        0x8004
#define STAT_IMPOSSIBLE     0x8005

// Handy Macros--BMC
#define SEC            T_SEC
#define MSEC           1

#define ALWAYS         1                              // use in while loops
#define OS_FOREVER     INFINITE_TIMEOUT               // use in OS_ functions
#define NOW            ZERO_TIMEOUT
// delay foreground task for msd milliseconds



#endif

/*$--- End of File : common.h ---*/
