/*****************************************************/
/* CL_common.h                                       */
/* Define the common macros and structures           */
/* of Contactless application for all PAX Readers    */
/* Created by Liuxl at July 30, 2009                 */
/* v110 2016.03.22                                  */
/*****************************************************/

#ifndef _CLSS_COMMON_H
#define _CLSS_COMMON_H

//---------------------------------------------------------------------------
/******************************* Return Codes ******************************/
//---------------------------------------------------------------------------
#ifndef _EMV_LIB_H 
#define EMV_OK                          0         //All processing is successful
#define ICC_RESET_ERR                   -1        //IC card reset is failed
#define ICC_CMD_ERR                     -2        //IC card command is failed
#define ICC_BLOCK                       -3        //IC card is blocked
#define EMV_RSP_ERR                     -4        //Status Code returned by IC card is not 9000
#define EMV_APP_BLOCK                   -5        //The Application selected is blocked
#define EMV_NO_APP                      -6        //There is no AID matched between ICC and terminal
#define EMV_USER_CANCEL                 -7        //The Current operation or transaction was canceled by user
#define EMV_TIME_OUT                    -8        //User＊s operation is timeout
#define EMV_DATA_ERR                    -9        //Data error is found
#define EMV_NOT_ACCEPT                  -10       //Transaction is not accepted
#define EMV_DENIAL                      -11       //Transaction is denied
#define EMV_KEY_EXP                     -12       //Certification Authority Public Key is Expired
#define EMV_NO_PINPAD                   -13       //PIN enter is required, but PIN pad is not present or not working
#define EMV_NO_PASSWORD                 -14       //PIN enter is required, PIN pad is present, but there is no PIN entered
#define EMV_SUM_ERR                     -15       //Checksum of CAPK is error
#define EMV_NOT_FOUND                   -16       //Appointed Data Element can＊t be found
#define EMV_NO_DATA                     -17       //The length of the appointed Data Element is 0
#define EMV_OVERFLOW                    -18       //Memory is overflow
#define NO_TRANS_LOG                    -19       //There is no Transaction log
#define RECORD_NOTEXIST                 -20       //Appointed log is not existed
#define LOGITEM_NOTEXIST                -21       //Appointed Label is not existed in current log record
#define ICC_RSP_6985                    -22       //Status Code returned by IC card for GPO is 6985
#define EMV_PARAM_ERR                   -30        
#endif

#define CLSS_USE_CONTACT                -23       //Must use other interface for the transaction
#define EMV_FILE_ERR                    -24       //There is file error found
#define CLSS_TERMINATE                  -25       //Must terminate the transaction
#define CLSS_FAILED                     -26       //Contactless transaction is failed
#define CLSS_DECLINE                    -27       //Transaction should be declined.
#define CLSS_TRY_ANOTHER_CARD       	-28 	  //Try another card (DPAS Only)
#define CLSS_PARAM_ERR                  -30       //Parameter is error = EMV_PARAM_ERR
#define CLSS_WAVE2_OVERSEA              -31       //International transaction(for VISA AP PayWave Level2 IC card use)
#define CLSS_WAVE2_TERMINATED           -32       //Wave2 DDA response TLV format error
#define CLSS_WAVE2_US_CARD              -33       //US card(for VISA AP PayWave L2 IC card use)
#define CLSS_WAVE3_INS_CARD             -34       //Need to use IC card for the transaction(for VISA PayWave IC card use)
#define CLSS_RESELECT_APP               -35       //Select the next AID in candidate list
#define CLSS_CARD_EXPIRED               -36       //IC card is expired
#define EMV_NO_APP_PPSE_ERR             -37       //No application is supported(Select PPSE is error)
#define CLSS_USE_VSDC                   -38       //Switch to contactless PBOC
#define CLSS_CVMDECLINE                 -39       //CVM result in decline for AE
#define CLSS_REFER_CONSUMER_DEVICE      -40       //Status Code returned by IC card is 6986, please see phone
#define CLSS_LAST_CMD_ERR               -41       //The last read record command is error(qPBOC Only)
#define CLSS_API_ORDER_ERR              -42       //APIs are called in wrong order. Please call Clss_GetDebugInfo_xxx to get error codes.
#define CLSS_TORN_CARDNUM_ERR           -43		  //torn log's pan is different from the reselect card's pan	
#define CLSS_TRON_AID_ERR               -44       //torn log's AID is different from the reselect card's AID
#define CLSS_TRON_AMT_ERR               -45       //torn log's amount is different from the reselect card's amount 
#define CLSS_CARD_EXPIRED_REQ_ONLINE    -46       //IC card is expired and should continue go online

#define CLSS_PAYMENT_NOT_ACCEPT         -200      // Payment Type Not Accepted for flash

//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/******************************* Macros declaration ************************/
//---------------------------------------------------------------------------
#ifndef _EMV_LIB_H 
#define MAX_REVOCLIST_NUM               30        //Maximum number of Issuer Public Key Certificate Recover List for EMV kernel
#define MAX_APP_NUM                     32        //Maximum number of application number for EMV kernel

//Application match mode
#define PART_MATCH                      0x00      //Application Select matching flag(partly matching)  
#define FULL_MATCH                      0x01      //Application Select matching flag(totally matching)

//Transaction type
#define EMV_CASH                        0x01      //Cash
#define EMV_GOODS                       0x02      //Goods
#define EMV_SERVICE                     0x04      //Service
#define EMV_CASHBACK                    0x08      //CashBack
#define EMV_INQUIRY                     0x10      //Inquiry
#define EMV_TRANSFER                    0x20      //Transfer
#define EMV_PAYMENT                     0x40      //Payment
#define EMV_ADMIN                       0x80      //Administer
#define EMV_CASHDEPOSIT                 0x90      //Cash Deposit
#endif                                      	

#define CLSS_MAX_AIDLIST_NUM            32        //Maximum number of application number for contactless kernel
#define CLSS_MAX_KEY_NUM                7         //Maximum number of CAPK number of contactless kernel
                                          	
//CVM rule                                      	
#define RD_CVM_NO                       0x00      //No CVM
#define RD_CVM_SIG                      0x10      //Signature
#define RD_CVM_ONLINE_PIN               0x11      //Online PIN
#define RD_CVM_OFFLINE_PIN              0x12      //Offline PIN
#define RD_CVM_CONSUMER_DEVICE          0x1F      //Refer to consumer device

//When amount > contactless cvm limit, Which CVM mode processed:
#define RD_CVM_REQ_SIG                  0x01
#define RD_CVM_REQ_ONLINE_PIN           0x02

//Online Result
#define REFER_APPROVE                   0x01      //Reference Return code (choose Approved)
#define REFER_DENIAL                    0x02      //Reference Return code (choose Denial)
#define ONLINE_APPROVE                  0x00      //Online Return code (Online Approved) 
#define ONLINE_FAILED                   0x01      //Online Return code (Online Failed)
#define ONLINE_REFER                    0x02      //Online Return code (Online Reference)
#define ONLINE_DENIAL                   0x03      //Online Return code (Online Denial)
#define ONLINE_ABORT                    0x04      //Compatible PBOC(Transaction Terminate)

//Transaction result
#define CLSS_DECLINED                   0x00
#define CLSS_APPROVE                    0x01
#define CLSS_ONLINE_REQUEST             0x02
#define CLSS_TYR_ANOTHER_INTERFACE      0x03
#define CLSS_END_APPLICATIION           0x04

//Kernel ID
#define KERNTYPE_DEF                    0
#define KERNTYPE_JCB                    1
#define KERNTYPE_MC                     2
#define KERNTYPE_VIS                    3
#define KERNTYPE_PBOC                   4
#define KERNTYPE_AE                     5
#define KERNTYPE_ZIP                    6
#define KERNTYPE_FLASH                  7
#define KERNTYPE_EFT                    8
#define KERNTYPE_RFU                    0xFF

//AC Type
#define AC_AAC                          0x00
#define AC_TC                           0x01
#define AC_ARQC                         0x02

//Outcome Parameter Set
#define CLSS_OC_APPROVED                0x10
#define CLSS_OC_DECLINED                0x20
#define CLSS_OC_ONLINE_REQUEST          0x30
#define CLSS_OC_END_APPLICATION         0x40
#define CLSS_OC_SELECT_NEXT             0x50
#define CLSS_OC_TRY_ANOTHER_INTERFACE   0x60
#define CLSS_OC_TRY_AGAIN               0x70
#define CLSS_OC_NA                      0xF0
#define CLSS_OC_A                       0x00
#define CLSS_OC_B                       0x10
#define CLSS_OC_C                       0x20
#define CLSS_OC_D                       0x30
#define CLSS_OC_NO_CVM                  0x00
#define CLSS_OC_OBTAIN_SIGNATURE        0x10
#define CLSS_OC_ONLINE_PIN              0x20
#define CLSS_OC_CONFIRM_CODE_VER        0x30

//Transaction Path
#define CLSS_PATH_NORMAL                0
#define CLSS_VISA_MSD                   1         // scheme_visa_msd_20
#define CLSS_VISA_QVSDC                 2         // scheme_visa_wave3
#define CLSS_VISA_VSDC                  3         // scheme_visa_full_vsdc
#define CLSS_VISA_CONTACT               4       	
#define CLSS_MC_MAG                     5       	
#define CLSS_MC_MCHIP                   6       	
#define CLSS_VISA_WAVE2                 7         // SCHEME_VISA_WAVE_2
#define CLSS_JCB_WAVE2                  8         // SCHEME_JCB_WAVE_2
#define CLSS_VISA_MSD_CVN17             9
#define CLSS_VISA_MSD_LEGACY            10
//JCB(J/Speedy): MCHIP[EMV];MAG;LEGACY Mode
#define CLSS_JCB_MAG                    11
#define CLSS_JCB_EMV                    12
#define CLSS_JCB_LEGACY                 13
//Discover(DPAS/ZIP): EMV;MAG;ZIP Mode
#define CLSS_DPAS_MAG                   14
#define CLSS_DPAS_EMV                   15
#define CLSS_DPAS_ZIP                   16

// Visa Scheme ID
#define SCHEME_VISA_WAVE_2              0x16
#define SCHEME_VISA_WAVE_3              0x17
#define SCHEME_VISA_MSD_20              0x18
#define SCHEME_JCB_WAVE_1               0x60
#define SCHEME_JCB_WAVE_2               0x61
#define SCHEME_JCB_WAVE_3               0x62 
//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/******************************* Structure declaration *********************/
//---------------------------------------------------------------------------
#ifndef _EMV_LIB_H 
typedef struct {
	unsigned char RID[5];                 //Registered Application Provider Identifier
	unsigned char KeyID;                  //Key Index
	unsigned char HashInd;                //Hash arithmetic indicator
	unsigned char ArithInd;               //RSA arithmetic indicator
	unsigned char ModulLen;               //Length of Module
	unsigned char Modul[248];             //Module
	unsigned char ExponentLen;            //Length of exponent
	unsigned char Exponent[3];            //Exponent
	unsigned char ExpDate[3];             //Expiration Date (YYMMDD) 
	unsigned char CheckSum[20];           //Check Sum of Key
}EMV_CAPK;

typedef  struct 
{
	unsigned char ucRid[5];               //RID
	unsigned char ucIndex;                //CAPK Index
	unsigned char ucCertSn[3];            //Issuer Certificate Serial Number
}EMV_REVOCLIST;

typedef struct CLSS_TRANSPARAM
{
	unsigned long ulAmntAuth;             //Authority Amount(unsigned long)
	unsigned long ulAmntOther;            //Other Amount(unsigned long) 
	unsigned long ulTransNo;              //Transaction Sequence Counter(4 BYTE)
	unsigned char ucTransType;            //Transaction Type '9C'
	unsigned char aucTransDate[4];        //Transaction Date  YYMMDD
	unsigned char aucTransTime[4];        //Transaction Time  HHMMSS
}Clss_TransParam;

typedef struct{
	unsigned char AppName[33];       //Local application name. The string ends with '\x00' and is 32 bytes in maximum.
	unsigned char AID[17];           //Application ID. 
	unsigned char AidLen;            //the length of AID
    unsigned char SelFlag;           //Application selection flag (partial matching PART_MATCH or full matching FULL_MATCH)      
	unsigned char Priority;          //priority indicator
	unsigned char TargetPer;         //Target percent (0 每 99) (provided by acquirer)
	unsigned char MaxTargetPer;      //Max target percent(0 每 99) (provided by acquirer)
 	unsigned char FloorLimitCheck;   //Check the floor limit or not (1: check, 0 : not checkㄛdefault:1)
	unsigned char RandTransSel;      //Perform random transaction selection or not (1: perform, 0 : not perform, default : 1)
	unsigned char VelocityCheck;     //Perform velocity check or not (1 : perform, 0 not perform, default : 1)
    unsigned long FloorLimit;        //Floor limit (provided by acquirer)
	unsigned long Threshold;         //Threshold (provided by acquirer)
 	unsigned char TACDenial[6];      //Terminal action code - denial
	unsigned char TACOnline[6];      //Terminal action code 每 online
	unsigned char TACDefault[6];     //Terminal action code 每 default
    unsigned char AcquierId[6];      //Acquirer identifier 
	unsigned char dDOL[256];         //terminal default DDOL
	unsigned char tDOL[256];         //terminal default TDOL
	unsigned char Version[3];        //application version
	unsigned char RiskManData[10];   //Risk management data
}EMV_APPLIST;

#endif

//pre-processing information of each AID
typedef struct CLSS_PREPROCINFO
{
	unsigned long ulTermFLmt;
	unsigned long ulRdClssTxnLmt;
	unsigned long ulRdCVMLmt;
	unsigned long ulRdClssFLmt;
	unsigned char aucAID[17];       
	unsigned char ucAidLen; 
	unsigned char ucKernType; 
	// PayWave
	unsigned char ucCrypto17Flg;
	unsigned char ucZeroAmtNoAllowed;      
	unsigned char ucStatusCheckFlg;    
	unsigned char aucReaderTTQ[5];     
	// Common
	unsigned char ucTermFLmtFlg; 
	unsigned char ucRdClssTxnLmtFlg; 
	unsigned char ucRdCVMLmtFlg;   
	unsigned char ucRdClssFLmtFlg; 	 
	unsigned char aucRFU[2];
}Clss_PreProcInfo;

//pre-processing result flag in kernel
typedef struct CLSS_PREPROC_INTER_FLAG_INFO
{
	unsigned char aucAID[17];       
	unsigned char ucAidLen; 
	// PayWave
	unsigned char ucZeroAmtFlg;           //0-Transaction amount is not equal to 0; 1- Transaction amount is equal to 0
	unsigned char ucStatusCheckFlg;       //Status Check Flag
	unsigned char aucReaderTTQ[5];        //Terminal Transaction Qualifiers , used for VISA/qPBOC, tag equal to＊9F66＊
	unsigned char ucCLAppNotAllowed;      //1-this AID can＊t progress contactless transaction
	// Common
	unsigned char ucTermFLmtExceed; 
	unsigned char ucRdCLTxnLmtExceed; 
	unsigned char ucRdCVMLmtExceed;  
	unsigned char ucRdCLFLmtExceed;  
	unsigned char ucTermFLmtFlg;
	unsigned char aucTermFLmt[5];
	unsigned char aucRFU[2];
}Clss_PreProcInterInfo;

typedef struct  
{
	unsigned long ulReferCurrCon;         //Reference currency code and transaction code＊s
	                                      //transform modulus (transaction currency to Reference currency exchange rate *1000)
	unsigned short usMchLocLen;           //Merchant name and location data field＊s length
	unsigned char aucMchNameLoc[257];     //Merchant name and location(1-256 Byte)
	unsigned char aucMerchCatCode[2];     //Merchant classify code'9F15'(2 Byte)
	unsigned char aucMerchantID[15];      //Merchant Identifier (15 Byte)
	unsigned char AcquierId[6];           //Acquirer Identifier
	unsigned char aucTmID[8];             //Terminal Identifier (Terminal Number)
	unsigned char ucTmType;               //Terminal Type
	unsigned char aucTmCap[3];            //Terminal capability
	unsigned char aucTmCapAd[5];          //Terminal additional capability
	unsigned char aucTmCntrCode [2];      //Terminal country code
	unsigned char aucTmTransCur[2];       //Terminal Transaction currency code '5F2A'(2 Byte)
	unsigned char ucTmTransCurExp;        //Terminal Transaction currency exponent '5F36'(1Byte)
	unsigned char aucTmRefCurCode[2];     //Terminal Transaction Reference currency code'9F3C'(2 Byte)
	unsigned char ucTmRefCurExp;          //Terminal Transaction Reference currency exponent'9F3D' (1Byte)
	unsigned char aucRFU[3];
}Clss_ReaderParam;

//	Clss Terminal AID List Struct
typedef struct
{
	unsigned char ucAidLen;
	unsigned char aucAID[17];
	unsigned char ucSelFlg;               //1-Partial match ;0-Exact match
	unsigned char ucKernType;
}ClssTmAidList;

typedef struct VISA_AID_PARAM
{
	unsigned long ulTermFLmt;             //Floor Limit. the same as contact EMV Floor Limit
	unsigned char ucDomesticOnly;         //01(default):only supports domestic cl transaction
	                                      //00 or not present: supports international cl transaction
	unsigned char ucCvmReqNum;
	unsigned char aucCvmReq[5];           //whether a CVM is required when the amount is higher than the Contactless CVM Required Limit.
	                                      //01-Signature 02-Online PIN	
	unsigned char ucEnDDAVerNo;           //0x00-Default value, Reader support all DDA version IC card offline transaction
	                                      //0x01-Only support'01' version of DDA＊s IC card offline transaction
}Clss_VisaAidParam;

typedef struct
{
	unsigned char ucSchemeID;             //Reference Application type parameter Macro definition
	unsigned char ucSupportFlg;           //1- supported; 0-not support
	unsigned char ucRFU[2];
}Clss_SchemeID_Info;

// PBOC
typedef struct PBOC_AID_PARAM
{
	unsigned long ulTermFLmt;             //Floor Limit. the same as contact EMV Floor Limit
	unsigned char aucRFU[4];
}Clss_PbocAidParam;

// MasterCard PayPass
typedef struct  MC_AID_PARAM
{
	unsigned long FloorLimit;             //Floor Limit. the same as contact EMV Floor Limit
	unsigned long Threshold;              //Threshold
	unsigned short usUDOLLen;       
	unsigned char uDOL[256];              //Terminal Default UDOL
	unsigned char TargetPer;              //Target Percentage
	unsigned char MaxTargetPer;           //Maximum Target Percentage
	unsigned char FloorLimitCheck;        //Weather process the Floor Limit(1-process,0-no process)
	unsigned char RandTransSel;           //Weather process the Random Transaction Selection(1-process)
	unsigned char VelocityCheck;          //Weather process the Velocity Checking
	unsigned char TACDenial[6];           //Terminal Action Code (Denial)
	unsigned char TACOnline[6];           //Terminal Action Code (Online)
	unsigned char TACDefault[6];          //Terminal Action Code (Default)
	unsigned char AcquierId[6];           //Acquirer Identity
	unsigned char dDOL[256];              //Terminal Default DDOL
	unsigned char tDOL[256];              //Terminal Default TDOL
	unsigned char Version[3];             //Application Version Number
	unsigned char ForceOnline;            //Transaction is forced Online by Merchant (1- the Transaction is always Online)
	unsigned char MagAvn[3];              //Application Version Number for MagStripe
	unsigned char ucMagSupportFlg;        //1-Reader support MagStripe 0-No support
	unsigned char ucRFU;
}Clss_MCAidParam; 

typedef struct CLSS_PROGRAMID_INFO
{
	unsigned long ulRdClssTxnLmt;         //Reader Contactless Transaction Limit
	unsigned long ulRdCVMLmt;             //Reader CVM Limit
	unsigned long ulRdClssFLmt;           //Reader Contactless Floor Limit
	unsigned long ulTermFLmt;             //Terminal Floor Limit
	unsigned char aucProgramId[17];       //Application Program ID
	unsigned char ucPrgramIdLen;          //Length of Application Program ID	
	unsigned char ucRdClssFLmtFlg;        //ulRdClssFLmt is present or not
	unsigned char ucRdClssTxnLmtFlg;      //ulRdClssTxnLmt is present or not
	unsigned char ucRdCVMLmtFlg;          //ulRdCVMLmt is present or not
	unsigned char ucTermFLmtFlg;          //ulTermFLmt is present or not	
	unsigned char ucStatusCheckFlg;       //Support Status Check or not
	unsigned char ucAmtZeroNoAllowed;     //Amount 0 Check
	unsigned char aucRFU[4];
}Clss_ProgramID;

// for qPBOC
typedef struct PBOC_TORN_CONFIG
{
	long lTornMaxLifeTime;				 //Torn Max life Time.
	unsigned short usTornLogMaxNum;      //Max number for torn Log.
	unsigned short usTornSupport;        //1 -Support torn Log; 0 - Not Support.
	unsigned char ucTornRFU[4];          //RFU
}Clss_PbocTornConfig;
//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------
#endif//_CLSS_COMMON_H
