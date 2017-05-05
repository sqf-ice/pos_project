#ifndef __LIB_H__
#define __LIB_H__



#include <string.h>


// CURRENTLY SUPPORTED TERMINAL TYPES
#define DIALUP  0  // Terminal Working Via Telephone Line
#define RS232   1  // Terminal Working Via One Of Its COM Ports by RS-232 ( No Protocol Conversion - In Data is Sent Exactly )

#define NETWORK 3


// Library function prototypes
#define ISO_MAX_LEN   (512-15)   // +15 bytes header = 512byte = max. modem output string


//#define ASYNC_OPERATION 0xAA
//#define SYNC_OPERATION  0X55

#define TYPE_NONE     0
#define TYPE_2ASCII   1
#define TYPE_BCD      2
#define TYPE_BINARY   3
#define TYPE_1ASCII   4

#define VARIABLE_SIZE   0
#define FIXED_SIZE      1
#define FIELD_NOT_USED        2


// Transaction Codes         Warning! Must Be on the same order as defined on Transactions Table!!!
//#define PAYMENT              0
//#define ACCOUNT_MAINT        1
//#define REVERSAL             2
//#define BATCH_CLOSE          3

// return codes
#define ISO_OK            1
#define ISO_FRAME_ERROR  -1
 
#define ISO_STAT_OK            0x00
#define ISO_INSERTED           0x00
#define ISO_COMM_ERROR         0x80
#define ISO_SIZE_ERROR         0x81
#define ISO_TYPE_ERROR         0x82
#define ISO_APP_ERROR          0x83
#define ISO_FRAME_FAULT        0x84
#define ISO_NO_REPLY           0x85
#define ISO_MODEM_NOK          0x86
#define ISO_REVERSAL_PENDING   0x87
#define ISO_USER_BREAK         0x88
#define ISO_STAT_NOK           0x89


#define REVERSAL_NOT_PRESENT 0xF0
#define REVERSAL_PRESENT     0x0F

typedef struct
{
	unsigned char TPDU;
	unsigned char destAddr[2];
	unsigned char origAddr[2];
	unsigned char msgType[2];
	unsigned char bitMap[8];
	unsigned char Data[ISO_MAX_LEN];
}ISOFrame;

typedef struct
{
	unsigned char TPDU;
	unsigned char destAddr[2];
	unsigned char origAddr[2];
	unsigned char msgType[2];
	unsigned char processingCode[3];
	unsigned char bitMap[8];
}ISOTransactionsDescriptor;


typedef struct 
{ 
	unsigned char type;             // must be TYPE_ASCII,TYPE_BCD or TYPE_BIN. TYPE_NONE if field not present
	unsigned char sizeRange;        // VAR_UP_BOUNDED variable upper bounded to n, VAR_NO_BOUNDED variable, FIXED_SIZE exact size to n
	short int     fldSize;          // 0 = field has no effect , n = field has exact size or bounded to n
												// if type = TYPE_BINARY n = length in bytes
									 // if type = TYPE_ASCII  n = length in bytes
									 // if type = TYPE_BCD    n = length in nibbles
	unsigned char *ptrSendData;     // pointer to SendData Field(n)
	unsigned char *ptrReceivedData;	// pointer to ReceivedData Field(n)
}ISOFieldDescriptor;


typedef struct 
{
	unsigned char trType;               //   original transaction indicator
	unsigned char year;
//	unsigned char Field1  [1];           //
	unsigned char Field2  [1+ (19/2)+1];    //   PAN - Used in manual entries
	unsigned char Field3  [3];          //   Processing Code
	unsigned char Field4  [6];          //   Transaction Amount
//	unsigned char Field5  [6];
//	unsigned char Field6  [6];
	unsigned char Field7  [5];          //   Transmission DTS
//	unsigned char Field8  [4];
//	unsigned char Field9  [4];
//	unsigned char Field10 [4];
	unsigned char Field11 [3];          //   STAN - System Trace Audit Number
	unsigned char Field12 [3];          //   Transaction Local Time in HHMMSS format
	unsigned char Field13 [2];          //   Transaction Local Date in DDMM format
	unsigned char Field14 [2];          //   Card Expiry Date in MMYY format - Used in manual Entries
//	unsigned char Field15 [2];
//	unsigned char Field16 [2];
//	unsigned char Field17 [2];
//	unsigned char Field18 [2];
	unsigned char Field19 [2];           //    Country Code, Acquiring Ins.    
//	unsigned char Field20 [2];
//	unsigned char Field21 [2];
	unsigned char Field22 [2];          //    POS Entry Mode 1-Manual 2-Magnetic
	unsigned char Field23 [2];          // Card Sequence Number
	unsigned char Field24 [2];          //    NII
	unsigned char Field25 [2];          //    POS Condition Code !!!Realy 1 byte , used 1st byte
	unsigned char Field26 [2];
//	unsigned char Field27 [2];
	unsigned char Field28 [9];
//	unsigned char Field29 [4];
//	unsigned char Field30 [4];
//	unsigned char Field31 [4];
	unsigned char Field32 [1+(11/2)+1];
//	unsigned char Field33 [1+(11/2)+1];
//	unsigned char Field34 [1+28/2];
	unsigned char Field35 [2+ 37 ]; //   Track II Data
//	unsigned char Field36 [2+52];
	unsigned char Field37 [12];          //   Retrieval Reference No
	unsigned char Field38 [6];           //   Authorisation ID Response
	unsigned char Field39 [2];           //   Response Code
//	unsigned char Field40 [3];
	unsigned char Field41 [8];           //   Terminal ID
	unsigned char Field42 [15];          //   Card Acquirer ID ( Merchant ID )
	unsigned char Field43 [40];          //   Card Acquirer Name
//	unsigned char Field44 [1+50];
	unsigned char Field45 [1+152/2];          //   Track I Data
//	unsigned char Field46 [2+300];
	unsigned char Field47 [2+230];
	unsigned char Field48 [2+300];       //   Additional Data
	unsigned char Field49 [2];           //   Transaction Currency Code
//	unsigned char Field50 [2];
//	unsigned char Field51 [2];
	unsigned char Field52 [64/8];            //   PIN Data
	unsigned char Field53 [18/2];
	unsigned char Field54 [120];        //   Additional Amounts
	unsigned char Field55 [2+230];
//	unsigned char Field56 [2+300];
//	unsigned char Field57 [2+300];
//	unsigned char Field58 [2+60];
	unsigned char Field59 [2+300];
	unsigned char Field60 [2+230];        //   Private Use
//	unsigned char Field61 [2+300];        //   Private Use
	unsigned char Field62 [2+230];        //   Private Use
	unsigned char Field63 [2+230];        //   Private Use
	unsigned char Field64 [32/8];        //   Private Use ( Generally MAC )

}ISODataStruct;
// Warning! On redefining this struct put 1/2 bytes more on the maximum size when field has variable size!!!
// Variable size fields will have [0X XX <DATASTREAM>] format where XXX indicates size according to type
// For example;        17 for a TYPE_BCD    field represents 17 nibbles in 9 bytes.
//                     0124 for a TYPE_ASCII  field represents 124 ascii characters in 124 bytes
//                     0064 for a TYPE_BIN    field represnts  64 bits in 8 bytes
// Warning as the ISO8583 specifications describes an ISO Packet can contain maximum 230 data bytes
// in one frame!!! So , fields having variable lengths ( as coded 230 length max. on the Fields table
// on ISO8583.c module ) can have maximum 230 bytes, though they are described to have max 999 bytes length!!!
// These fields are defined as unsigned char Fieldn [2 + 230] 2bytes length + 230 bytes data!!!!
// Be careful that BCD fields have 1 byte length indicator so can reach max 99 bytes!!!

typedef struct _tagMINSTATEMENT
{
    char transType;
    char transAmount[12];
    char transDate[8];
    char transTime[4];
    
}MST_INFO;

extern ISODataStruct sendData,receivedData,reversalData,ISOData/*,ISOBackUpData*/;
extern ISOFrame frameToSend,frameReceived;
extern MST_INFO mst_info[5];


#endif                                                    

