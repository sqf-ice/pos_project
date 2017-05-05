#ifndef _STRUCT_H_
#define _STRUCT_H_

typedef struct  
{
	BYTE	TermFlags;					//terminal flags
	BYTE  ApplStateInit;        /* Initialized application indicator (OS_YES/OS_NO)*/
}MStateStruct;
#define LG_MSTATESTRUCT	       (sizeof(MStateStruct))

typedef struct  
{
	BYTE SeqNum[LG_SEQNUM];      	   //4
	BYTE TransDate[8];	               //8  DDMMYYYY
	BYTE TransTime[6];	               //6   HHMMSS
	BYTE Pan[LG_PAN];				   //20
	BYTE TransType[LG_TRANSTYPE];      //2
	BYTE Balance[LG_AMOUNT];           //12
	BYTE ExpDate[4];                   //4
	BYTE TrackISO2[LG_TRACKISO2];      //41
	BYTE ProductCode[2];               //2
	BYTE TrackI[25];                   //25
	BYTE SchemeProfit[2];              //2
	BYTE SchemeDescription[12];        //12
	BYTE SchemeDescriptionArabic[12];  //12 //150
	BYTE AvailableInstallmentFlag1;    //1
	BYTE AvailableInstallmentFlag2;    //1
	BYTE EntryAmount[LG_AMOUNT];       //12
	BYTE TotalInstallmentCount;        //1
	BYTE MonthlyInstAmount[LG_AMOUNT]; //12
	BYTE TotalAmount[LG_AMOUNT];       //12
	BYTE ProfitAmountBcd[6];           //6
	BYTE PSN;                          //1
	BYTE CurrencyCode[2];              //2
	BYTE CountryCode[2];               //2
	BYTE ResponseCode[2];              //2
	BYTE Cvv[3];                       //3

	BYTE Stan[3];                      //3
	BYTE RetrivalRef[12];              //12
	BYTE BcdPan[11];                   //11
	BYTE BcdAmount[6];                 //6
	BYTE BcdExpDate[2];                //2
	BYTE BcdDateTime[5];               //5
	BYTE BcdPosEntryCode[2];           //2
	
	BYTE AIP[2];                       //2  
	BYTE TVR[5];                       //5
	BYTE ATC[2];                       //2  //255
	BYTE CVMR[3];                      //3
	BYTE cardVersion[2];               //2
	BYTE terminalCapabilities[3];      //3
	BYTE AC[8];                        //8
	BYTE CID;                          //1 
	BYTE issuerApplicationData[32];    //32
	BYTE UN[4];                        //4  //308
}TransStruct;
#define LG_TRANSSTRUCT	       (sizeof(TransStruct))

typedef struct  
{
#define LG_PRODUCT_TYPE 25
	BYTE    barcode[13+1];
	BYTE    proddesc[LG_PRODUCT_TYPE+1];
    BYTE    price_bcd[6];
    WORD  numbers;
}PRODUCT;

typedef struct 
{
	BYTE SeqNum[LG_SEQNUM];      	   //4
	BYTE TransDate[8];	               //8  DDMMYYYY
	BYTE TransTime[6];	               //6   HHMMSS
	BYTE Pan[LG_PAN];				      //20
	BYTE TransType[LG_TRANSTYPE];      //2
	BYTE Balance[LG_AMOUNT];           //12
	BYTE ExpDate[4];                   //4
	BYTE TrackISO2[LG_TRACKISO2];      //41
	BYTE ProductCode[2];               //2
	BYTE TrackI[25];                   //25
	BYTE SchemeProfit[2];              //2
	BYTE SchemeDescription[12];        //12
	BYTE SchemeDescriptionArabic[12];  //12
	BYTE AvailableInstallmentFlag1;    //1
	BYTE AvailableInstallmentFlag2;    //1
	BYTE EntryAmount[LG_AMOUNT];       //12
	BYTE TotalInstallmentCount;        //1
	BYTE MonthlyInstAmount[LG_AMOUNT]; //12
	BYTE TotalAmount[LG_AMOUNT];       //12
	BYTE ProfitAmountBcd[6];           //6
	BYTE PSN;                          //1
	BYTE CurrencyCode[2];              //2
	BYTE CountryCode[2];               //2
	BYTE ResponseCode[2];              //2
	BYTE Cvv[3];                       //3

	BYTE Stan[3];                      //3
	BYTE RetrivalRef[12];              //12
	BYTE RefNumber[12];
	BYTE BcdPan[11];                   //11
	BYTE BcdAmount[6];                 //6
	BYTE BcdExpDate[2];                //2
	BYTE BcdDateTime[5];               //5
	BYTE BcdPosEntryCode[2];           //2

	BYTE AIP[2];                       //2  
	BYTE TVR[5];                       //5
	BYTE ATC[2];                       //2  
	BYTE CVMR[3];                      //3
	BYTE cardVersion[2];               //2
	BYTE terminalCapabilities[3];      //3
	BYTE AC[8];                        //8
	BYTE CID;                          //1 
	BYTE issuerApplicationData[32];    //32
	BYTE UN[4];                        //4

///////////////////////////////////////////////////////////////////////////////
	BYTE ProductDescription[12];       //12
	BYTE ProductDescriptionArabic[12]; //12
	BYTE  MerchId[LG_MERCHID];          //8
	BYTE  StoreNum[LG_STORENUM];        //8
	BYTE  TerminalID[LG_TERMID];        //8
	BYTE	AuthResponse[LG_AUTHRESPONSE];//16

	BYTE  CardBinFlags;                 //1
	BYTE  CardType[2];                  //2
	BYTE	MerchData[LG_MERCHDATA];      //96    //default: 0000000...000h
	BYTE  FirstBalance[LG_AMOUNT];      //12
	BYTE  MerchantOperationFlags;       //1
//	BYTE  PosEntryType;                 //1   1=CHIP 2=MANUAL
	BYTE  Tr2Len;
	BYTE name_surname[40];
#define NUM_PRODUCTS_PERTRAN    5
    PRODUCT product[NUM_PRODUCTS_PERTRAN];

	// Added by Kim_LinHB 2013-3-19 build 0.05.0009
	BYTE MaxTenure;						
	BYTE HMinTenure;					
	BYTE HMaxTenure;
	BYTE HMinTotal[LG_AMOUNT];
	BYTE HMaxTotal[LG_AMOUNT];
	unsigned long lTenure;
	// Add End

	BYTE PIN[LG_PIN];  //added by Allen 2016-08-26
} PTStruct;
#define LG_PTSTRUCT	  (sizeof(PTStruct))

typedef struct 
{
	BYTE MerchId[LG_MERCHID];	                  //default: '**********'
	BYTE StoreNum[LG_STORENUM];
	BYTE TermId[LG_TERMID];	                     //default: '********'
	BYTE MerchData[LG_MERCHDATA];                //default: 0000000...000h
	BYTE TMSTel1[LG_PHONE+1];		               //default: '****************'
	BYTE TMSTel2[LG_PHONE+1];		               //default: '****************'
	BYTE TMSTel3[LG_PHONE+1];	          	      //default: '****************'
	BYTE TMSNII[4];	          	 

	BYTE MerchantTypeVersion[LG_PARMVERSION];		//default: '000'
	BYTE BinVersion[LG_PARMVERSION];		         //default: '000'
	BYTE BlackCardVersion[14];		               //default: '000'
	BYTE ProductVersion[LG_PARMVERSION];		   //default: '000'
	BYTE SchemeTypeVersion[LG_PARMVERSION];		//default: '000'
	BYTE RepaymentTenuresVersion[LG_PARMVERSION];//default: '000'

	BYTE BatchNum[6];		            //default: '000001'
	WORD MaxTrans;				                     //default: 60
	BYTE MaxAmountSize;				               //default: 9
	BYTE FirstTelNo;					               //default: 1
	BYTE OperationFlagsSmart1;
	BYTE OperationFlagsSmart2;
//	BYTE SlipMessage[LG_TICKETLINE];
	BYTE NETSERVERTel1[LG_PHONE+1];		         //default: '****************'
	BYTE NETSERVERTel2[LG_PHONE+1];		         //default: '****************'
	BYTE NETSERVERTel3[LG_PHONE+1];	          	//default: '****************'
	BYTE NETSERVERNII[4];	          	 
	BYTE PMSTel1[LG_PHONE+1];		               //default: '****************'
	BYTE PMSTel2[LG_PHONE+1];		               //default: '****************'
	BYTE PMSTel3[LG_PHONE+1];	          	      //default: '****************'
	BYTE PMSNII[4];	          
	BYTE bLastPrint;        //add by Allen 20151117
	WORD TransCount;
	WORD TraceNum;
	BYTE MinimumLimit[12];
//EMV
	BYTE maxTargetPercentage;//2
	BYTE targetPercentage;  //2
	BYTE defaultTAC[5];
	BYTE denialTAC[5];
	BYTE onlineTAC[5];
	BYTE terminalCountryCode[2];
	BYTE floorLimitDomestic[6];  // 12 BYTE
	BYTE floorLimitInternational[6]; // 12 BYTE
	BYTE thresholdValue[6];
	BYTE trCurrencyCode[2];
	BYTE TransactionCurrencyExponent;
	BYTE trCurrencySymbol[3];
///ISO
//	BYTE MacKey[9];
//	BYTE PinKey[9];
} HDGStruct;
#define	LG_HDGSTRUCT	       (sizeof(HDGStruct))

typedef struct  
{
	BYTE BCStartTime[LG_TRANSTIME];		   //'HHMM' default: 18:00
	BYTE BCAttempts;				            // default: 3
	BYTE ParmStartTime[LG_TRANSTIME];	   //'HHMM' default: 18:00
	BYTE ParmAttempts;				         // default: 3
	BYTE MaxAttempts;				         // default: 3
	WORD AttemptInterval;			         // default: 3 (mn)
}HDDStruct;
#define	LG_HDDSTRUCT	       (sizeof(HDDStruct))

extern PTStruct gPTStruct;
extern MStateStruct gState;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE trType;
	BYTE AIP[2];
	BYTE ATC[2];
	BYTE CID;
	BYTE CVMR[3];
	BYTE issuerApplicationData[32];
	OS_WORD issuerApplicationDataLen;
	BYTE issuerScriptCount[2];
	BYTE issuerScriptResults[2][50];
	BYTE TVR[5];
	BYTE AC[8];
	BYTE UN[4];
	BYTE DFName[32];
	BYTE DFNameLength;
	BYTE terminalCapabilities[3]; //3
	BYTE terminalType;
	BYTE issuerAuthenticationData[16];
	BYTE issuerAuthenticationDataLength;
	BYTE CAPK;
	BYTE minVersion[2];
	BYTE maxVersion[2];
	BYTE additionalTerminalCapabilities[5];
	BYTE floorLimitDomestic[6];
	BYTE floorLimitInternational[6];
	BYTE floorLimitPresent;
	BYTE floorLimit[4];
	BYTE targetPercentage;
	BYTE maxTargetPercentage;
	BYTE thresholdValue[6];
	BYTE denialTAC[5];
	BYTE onlineTAC[5];
	BYTE defaultTAC[5];
	BYTE onlineCapability;
	BYTE defaultDDOL[50];
	WORD defaultDDOLLength;
	BYTE defaultTDOL[50];
	WORD defaultTDOLLength;
	BYTE issuerScriptTemplateLength[2];
	BYTE issuerScriptTemplate[2][130];
	BYTE visaCheck;
	BYTE RID[5];
	BYTE pinTryCount;
	BYTE cardVersion[2];
}EMVDataStruct;

#define L_LINE 24
#define L_ENCRYPTEDPIN 8
#define L_RESPONSECODE 2

#define L_EXPDATE 4
#define CARD_PROC_FLAGS 4
#define TERM_PROC_FLAGS 6

typedef struct
{
	BYTE transactionType;
	BYTE encryptedPIN[L_ENCRYPTEDPIN];
//	BYTE cardProcessingFlags[CARD_PROC_FLAGS];
	BYTE transDTS[8];
	EMVDataStruct EMVData;
}TransactionStruct;
extern TransactionStruct TR;

typedef struct 
{
	BYTE ProductCode[2];
	BYTE ProductDescription[12];
	BYTE ProductDescriptionArabic[12];
}ProductStruct;
#define LG_PRODUCTSTRUCT	           (sizeof(ProductStruct))

typedef struct 
{
	BYTE SchemeProfit[2];
	BYTE SchemeMin[5];
	BYTE SchemeMax[5];
	BYTE SchemeDescription[12];
	BYTE SchemeDescriptionArabic[12];
}SchemeStruct;
#define LG_SCHEMESTRUCT	              (sizeof(SchemeStruct))

typedef struct  
{
	BYTE StartAmount[5];
	BYTE FinishAmount[5];
	BYTE AvailableInstallmentFlag1;
	BYTE AvailableInstallmentFlag2;
}AvailableInstallmentStruct;
#define LG_AVAILABLEINSTALLMENTSTRUCT (sizeof(AvailableInstallmentStruct))

typedef struct 
{
	BYTE Bin[6];
	BYTE CardBinFlags;
}BinStruct;
#define LG_BINSTRUCT		 (sizeof(BinStruct))


typedef struct  
{
	BYTE UpperLimit[6];  //CB
	BYTE Balance[6];     //9F50
	BYTE CurrencyCode[2];//C9
	BYTE InstallmentFlag;//CB =0 NOT ALLOWED
	BYTE LoyaltyFlag;
}CardParam;
#define LG_CARDPARAM (sizeof(CardParam))
extern CardParam gCardParam;



typedef struct 
{
//	BYTE RID[5];
	BYTE CAPKIndex;
	BYTE exponent[3];
	WORD keyLength;
	BYTE keyLocationPointer[490];
}KeyInfoStruct;
#define LG_EMVKEYSSTRUCT	 (sizeof(KeyInfoStruct))


typedef struct 
{
	char apn[32];													//modified by Kevin v0.05.0015 20150616			
	char username[32];			/* username */							
	char password[32];			/* password */	
	char ip_address[16];		/* IP Address, such as "192.168.1.104" */	
	ushort hport;
} GPRSStruct;
#define	LG_GPRSSTRUCT	       (sizeof(GPRSStruct))

#endif

