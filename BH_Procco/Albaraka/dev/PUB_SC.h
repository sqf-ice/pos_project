/******************************* pub_sc.h ***************************


PURPOSE: This file contains function prototypes common to the
application and OS. Included are types and other definitions
needed by the functions.

The order of this header file is :
1. defines
2. enums
3. structs
4. function prototypes

GLOBAL FUNCTIONS:
PROGRAMMER:

REVISIONS:

****************************************************************************/

/* This file contains definitions common to the application and OS. */

#ifndef __PUB_SC_H__
#define __PUB_SC_H__


#include "OS_TYPES.H"
#include "os_define.h"


#define SC_API_INT			0xD9

// Timeout definitions 

#define NO_TIMEOUT         0UL


// Smart card command defines 

#define  SC_WRITE_COMMAND  1
#define  SC_READ_COMMAND   0


// Sc State

#define SC_ENABLE          OS_TRUE
#define SC_DISABLE         OS_FALSE


// Smart card event masks

#define  SC_RX             0x01
#define  SC_STATUS_EVT     0x02
#define  SC_CUSTOMER_IN    0x04
#define  SC_CUSTOMER_OUT   0x08
#define  SC_SECURITY_IN    0x10
#define  SC_SECURITY_OUT   0x20
#define  SC_CUSTOMER_VCC   0x40
#define  SC_SECURITY_VCC   0x80
#define  SC_SAM_IN         0x100
#define  SC_SAM_OUT        0x200
#define  SC_RXTIMEOUT_EVT  0x800
#define  SC_TIMEOUT_EVT    0x800


// Constants defining the Command Case (T=0 & T=1 protocols) 
#define C_CASE_0       0
#define C_CASE_1       1
#define C_CASE_2S      0x2
#define C_CASE_3S      0x3
#define C_CASE_4S      0x4
                                   // Followings Cases are specific, see
                                   // SC API documentation.
#define C_CASE_3SU            0x83 // Special Case Format to T=1
#define C_CASE_INIT           0xFF // Specifics initializations
#define C_INIT_CLAGETRESPONSE 0xFE // Specific to T=0
#define C_CASE_4CB            0x4C // Commande BA Test FIME


// ATR MAXIMUM LENGTH
// ATR Format description  :
//    - TS                 : 01 byte
//    - T0                 : 01 byte
//    - TA1 to TC4         : 15 bytes max
//    - n Historical bytes : 15 bytes max
//    - TCK (T=1 only)     : 01 byte
// --------------------------------------
#define ATR_MAXLEN           33 // bytes


// C-APDU / R-APDU MAXIMUM LENGTH
// Note, this define represents the longest APDU, which is
// the Response Case 4S for T=0 (see SCLIB Users Guide).

#define APDU_MAXLEN          266


// SCLIB general Information 

typedef struct
{
  EVT_ID   sc_evt;
  char     version[VERSION_STR_SIZE];
} SC_INFO_STRUCT;


// SC interfaces ID

typedef enum
{
  SECURITY_SC_RDR  = 1,       // security SC reader
  CUSTOMER_SC_RDR  = 2,       // customer SC reader
  SIM_1_SC_RDR     = 3,       // sim SC reader 1
  SIM_2_SC_RDR     = 4,       // sim SC reader 2
  SIM_3_SC_RDR     = 5,       // sim SC reader 3
  SIM_4_SC_RDR     = 6,       // sim SC reader 4
  PINPAD_SC_RDR    = 0x1002,  // pinpad SC reader
  MCUSTOMER_SC_RDR = 0x2002   // force terminal customer SC reader
} SC_READER;


// enum for the SC_RESET_MODE values 

typedef enum
{
  SC_WARM_RESET       = 0,
  SC_COLD_RESET       = 1
} SC_RESET_MODE;


// enum for the Sc Working Mode SC_WM values

typedef enum
{
  SC_WM_NORMAL        = 0,      // Working Normal 
  SC_WM_SPP           = 1       // A Smart pinpad is present. It is take into
                                // account by the SCLIB.
} SC_WM;


// SC_SETUP_ST structure is a parameter of SC_Setup() function

typedef struct
{
  SC_WM               wWorkMode;         
} SC_SETUP_ST;

// enum for the SC_SYNC_ORDER values 

typedef enum
{
  SYNC_OUT_ORDER      = 1,
  SYNC_IN_ORDER       = 2
} SC_SYNC_ORDER;


typedef enum
{
  SYNC_READ,
  SYNC_PRESENT,
  SYNC_EW_TIME,
  SYNC_ERASE,
  SYNC_AUTH,
  SYNC_WRITE,
} SC_SYNC_COMMANDS;


// type which allows to point directly to the SW1-SW2 Smart Card status 

typedef struct
{
  BYTE      bSw1;
  BYTE      bSw2;
} SW1SW2;


// Status words SW1-SW2 access

typedef union
{
  OS_WORD      wSw;
  SW1SW2    wSwByte;
} SWORD;


// Synchronous card meaning only.


//  The following SC_OPTIMIZE feed the CM parameter of the Smart Card
//  driver function 'Smard Card Connection' (INT 55H Funct 01H). The
//  actual hex values are used, so DO NOT CHANGE the enum values unles
//  the driver changes.  This parameter specifies which type of card to
//  attempt to read and in which order allowing for faster turnaround
//  if the application knows what type of card it is expecting.
 
typedef enum
{
   ASYNC_SYNC           = 0x00,  // async card then sync card              
   SYNC_ASYNC           = 0x01,  // sync card first then async card        
   SYNC_ONLY            = 0x02,  // sync card only                         
   ASYNC_ONLY           = 0x03,  // async card only                        
   SYNC_EExK            = 0x10,  // attempt connection to sync card of     
                                 // family EExK                            
   SYNC_Fxxx            = 0x11,  // attempt connection to sync card of     
                                 // family Fxxx                            
}  SC_OPTIMIZE;


// Type of protocol to apply to async cards; used with 'Smart Card Connection'
// (INT 55H, Funct 01H)

typedef enum
{
   T0_PROTOCOL          = 0x00,
   T1_PROTOCOL          = 0x01
}  SC_PROTOCOL;


// Following are card types.
// !!!! WARNING !!!!
// From now on, only asynchronous cards are supported by the mother SCLIB.
// Synchronous enums stay here only to keep the compatibility with released
// synchronous cards. SC_TYPE enum is no longer used by new synchronous libs.

typedef enum
{
   ASYNC_CARD      = 0,
   FE417_CARD      = 1,  // ALSO SLE4404
   VISA_SYNC_CARD  = 2,
   EEX_SYNC_CARD   = 3,
   T2G_CARD_5      = 4,
   T2G_CARD_6      = 5,
   GPM896_CARD     = 6,
   UNRECOG_CARD    = 7,
   SLE4428_CARD    = 8,  // ALSO SLE4418
   GPM103_CARD     = 9,  // ALSO SLE4406
} SC_TYPE;


// Standard to apply at the connection

typedef enum
{
  ISO = 0,  // ISO/IEC 7816 standard
  EMV = 1,  // EMV'96 standard
} SC_STANDARD;


// Smartcard error codes

typedef enum
{
   SC_OK = 1,              // success. no error encountered
   SC_ERROR,               // Error occurred with the SC rdr hardware
   SC_CARD_REMOVED,        // card not present
   SC_ERROR_VCC_DROP,      // VCC power down during SC communication
   SC_TIMEOUT,             // internal timeout during card access
   SC_ERROR_CARD_UNKNOWN,  // card type not handled
   SC_ERROR_BAD_PARAM,     // Bad input parameter
   SC_ERROR_PROTO,         // Protocol error
   SC_ERROR_SYNC1,
   SC_ERROR_SYNC2,
   SC_ERROR_SYNC3,
   SC_NO_BAY,
   SC_BAD_CARD_TYPE,
   SC_REJECT,
   SC_RESET_OK,
   SC_WARM_REJECT,
   SC_PARITY
} SC_STATUS;


#ifdef __cplusplus
   extern "C" OS_WORD gSCLIBIndex;          //      index of Smart Card library in AM library table
#else
   extern OS_WORD gSCLIBIndex;          //      index of Smart Card library in AM library table
#endif

// Smart Card API routines
#ifdef __cplusplus
   extern "C" {
#endif

  SC_STATUS SC_CardDetect    ( SC_READER fwReader, BOOLEAN fwControl );
  SC_STATUS CardConnect(SC_READER reader);
  BOOLEAN   SC_CardInserted  ( SC_READER card_slot, DWORD timeout );
  BOOLEAN   SC_CardRemoved   ( SC_READER card_slot, DWORD timeout );
  SC_STATUS SC_SetFrameSize  ( SC_READER fwReader, BYTE bFrameSize );
  SC_STATUS SC_ResetCard     ( SC_READER fwReader, SC_RESET_MODE fwMode,
                               SC_STANDARD fwSpec, BYTE *fpBuffer );
  SC_STATUS SC_CardCommand   ( SC_READER fwReader, BYTE *fpBuffer,
                               OS_WORD fwLength, SWORD *fpSw1Sw2 );
  SC_STATUS SC_CommandCard   ( SC_READER, BYTE *, OS_WORD, OS_WORD, SWORD *);
                               
  SC_STATUS SC_CardDisconnect( SC_READER fwReader );
  BOOLEAN   SC_RxReady       ( DWORD timeout );
  BOOLEAN   SC_AllEvtSet     ( APP_EVT_MASK evt_list, DWORD tick_timeout );
  BOOLEAN   SC_AnyEvtSet     ( APP_EVT_MASK evt_list, DWORD tick_timeout );
  void      SC_EvtClear      ( APP_EVT_MASK evt_list );
  EVT_ID    SC_EvtID         ( void );
  void      TickPause        ( unsigned tick_timeout);
  SC_STATUS SC_Select        ( SC_READER fwReader );
  SC_STATUS SC_Setup         ( SC_SETUP_ST * );
  SC_STATUS SC_ForceATR      ( SC_READER fwReader, BYTE *fpBuffer );

#ifdef __cplusplus
  }
#endif

#endif

