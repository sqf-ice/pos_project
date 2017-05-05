/*************************************************************/
/* EmvApi.h                                                  */
/* Define the Application Program Interface                  */
/* of EMV L2 for PAX A serial POS terminals, such as D180    */
/*************************************************************/
 
#ifndef _EMV_LIB_H
#define _EMV_LIB_H

//#include "..\inc\posapi.h"// for D180
#include <posapi.h>

#include "..\inc\EmvIIApi.h"

#define _PCI_PINPAD_


//---------------------------------------------------------------------------
/******************************* Macros declaration ************************/
//---------------------------------------------------------------------------
#ifndef _CLSS_COMMON_H
#define MAX_APP_NUM                     32       //Maximum number of application list.
#define MAX_KEY_NUM                     1        //Maximum number of public key stored in CA public key list of kernel
#endif
//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/******************************* Structure declaration *********************/
//---------------------------------------------------------------------------
#ifndef _CLSS_COMMON_H
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
	unsigned char ucRid[5];               // Registered Application Provider Identifier
	unsigned char ucIndex;                // Certification Authenticate Public Key Index.
	unsigned char ucCertSn[3];            // Issuer Certificate Serial Number.
}EMV_REVOCLIST;
#endif //_CLSS_COMMON_H

typedef struct{
	unsigned char AppName[33];            //Local application name. The string ends with '\x00' and is 32 bytes in maximum.
	unsigned char AID[17];                //Application ID. 
	unsigned char AidLen;                 //the length of AID
	unsigned char SelFlag;                //Application selection flag (partial matching PART_MATCH or full matching FULL_MATCH)      
	unsigned char Priority;               //priority indicator
	unsigned char TargetPer;              //Target percent (0 每 99) (provided by acquirer)
	unsigned char MaxTargetPer;           //Max target percent(0 每 99) (provided by acquirer)
 	unsigned char FloorLimitCheck;        //Check the floor limit or not (1: check(default), 0 : not check)
	unsigned char RandTransSel;           //Perform random transaction selection or not (1: perform(default), 0 : not perform)
	unsigned char VelocityCheck;          //Perform velocity check or not (1 : perform(default), 0 not perform)
	unsigned long FloorLimit;             //Floor limit (provided by acquirer)
	unsigned long Threshold;              //Threshold (provided by acquirer)
 	unsigned char TACDenial[6];           //Terminal action code - denial
	unsigned char TACOnline[6];           //Terminal action code 每 online
	unsigned char TACDefault[6];          //Terminal action code 每 default
	unsigned char AcquierId[6];           //Acquirer identifier 
	unsigned char dDOL[256];              //terminal default DDOL
	unsigned char tDOL[256];              //terminal default TDOL
	unsigned char Version[3];             //application version
	unsigned char RiskManData[10];        //Risk management data
}EMV_APPLIST;

typedef struct{
	unsigned char aucAppPreName[17];      //Application preferred name, ending with '\0'.
	unsigned char aucAppLabel[17];        //Application label, end with '\0'.
	unsigned char aucIssDiscrData[244];   //Data in template "BFOC" or "73", in the format of length+value, 
	                                      //where 1st byte for length and other bytes for value.
	unsigned char aucAID[17];             //AID of ICC
	unsigned char ucAidLen;               //Length of AID of ICC
}APPLABEL_LIST;

typedef struct{
	unsigned char MerchName[256];         //Merchant name
	unsigned char MerchCateCode[2];       //Merchant catalog code
	unsigned char MerchId[15];            //Merchant identification
	unsigned char TermId[8];              //Terminal identification
	unsigned char TerminalType;           //Terminal type
	unsigned char Capability[3];          //Terminal capabilities
	unsigned char ExCapability[5];        //Additional terminal capabilities
	unsigned char TransCurrExp;           //Transaction currency exponent
	unsigned char ReferCurrExp;           //Reference currency exponent     
	unsigned char ReferCurrCode[2];       //Reference currency code
	unsigned char CountryCode[2];         //Terminal country code 
	unsigned char TransCurrCode[2];       //Transaction currency code 
	unsigned long ReferCurrCon;           //the conversion quotients between transaction currency 
	                                      //and reference currency (default : 1000)
	                                      //(the exchange rate of transaction currency to reference currency *1000)
	unsigned char TransType;              //Set current transaction type
	                                      //(EMV_CASH EMV_GOODS EMV_SERVICE EMV_GOODS&EMV_CASHBACK EMV_SERVICE&EMV_CASHBACK)
	unsigned char ForceOnline;            //Merchant force online (1 means always require online transaction)
	unsigned char GetDataPIN;             //Read the IC card PIN retry counter before verify the PIN or not 
	                                      //(1 : read(default), 0 : not read)
	unsigned char SurportPSESel;          //Support PSE selection mode or not (1:support(default), 0:not support)
}EMV_PARAM;

typedef struct  
{
	unsigned char ucUseTermAIPFlg;        //0-TRM is based on AIP of card(default),1-TRM is based on AIP of Terminal
	unsigned char aucTermAIP[2];          //The bit4 of byte1 decide whether force to perform TRM: "08 00"-Yes;"00 00"-No(default)
	unsigned char ucBypassAllFlg;         //whether bypass all other pin when one pin has been bypassed 1-Yes,0-No
}EMV_EXTMPARAM;

typedef struct 
{
	unsigned char  ucBypassPin;           // 0-Not supported,1-Supported(default)
	unsigned char  ucBatchCapture;        // 0-Online data capture,1-batch capture(default)
	void *pvoid;
}EMV_MCKPARAM;

typedef struct 
{
	unsigned char ucECTSIFlg;             // TSI value is exist or not
	unsigned char ucECTSIVal;             // TSI value
	unsigned char ucECTTLFlg;             // TTL value is exist or not
	unsigned long ulECTTLVal;             // TTL value
}EMV_TMECPARAM;

typedef struct{
	unsigned char aucAppPreName[17];	//Application Prefer Name
	unsigned char aucAppLabel[17];		//Application Label
	unsigned char aucIssDiscrData[244]; //tag 'BF0C', the first byte is the length.
	unsigned char aucAID[17];			//Card AID
	unsigned char ucAidLen;				//Card AID Length
	unsigned char ucPriority;           //Priority
	unsigned char aucAppName[33];	    //The Local Application name
}EMV_CANDLIST;

//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/****************************** Function declaration ***********************/
//---------------------------------------------------------------------------
//Parameter and Data Management
int  EMVCoreInit(void);
void EMVGetParameter(EMV_PARAM *tParam);
void EMVSetParameter(EMV_PARAM *tParam);
int  EMVGetTLVData(unsigned short Tag, unsigned char *DataOut, int *OutLen);
int  EMVSetTLVData(unsigned short Tag, unsigned char *DataIn, int DataLen);
int  EMVGetScriptResult(unsigned char *Result, int *RetLen);
int  EMVSetPCIModeParam(unsigned char ucPciMode, unsigned char *pucExpectPinLen,
						unsigned long ulTimeoutMs);
int  EMVReadVerInfo(char *paucVer);
void EMVSetScriptProcMethod(unsigned char ucScriptMethodIn);
int  EMVGetMCKParam(EMV_MCKPARAM *pMCKParam);
int  EMVSetMCKParam(EMV_MCKPARAM *pMCKParam);
void EMVSetConfigFlag(int nConfigflag);
int  EMVGetParamFlag(unsigned char ucParam, int *pnFlg);

//Terminal Application List Management
int  EMVAddApp(EMV_APPLIST *App);
int  EMVGetApp(int Index, EMV_APPLIST *App);
int  EMVDelAllApp(void);
int  EMVModFinalAppPara(EMV_APPLIST *ptEMV_APP);
int  EMVGetFinalAppPara(EMV_APPLIST *ptEMV_APP);
int  EMVGetLabelList(APPLABEL_LIST  *ptAppLabel, int *pnAppNum);

//Certification Authority (CA) Public Key Management
int  EMVAddCAPK(EMV_CAPK *capk);
int  EMVGetCAPK(int Index, EMV_CAPK *capk);
int  EMVDelAllCAPK(void);

//Terminal Revoked Issuer Public Key Certification List Management
int  EMVAddRevocList(EMV_REVOCLIST *pRevocList);
void EMVDelAllRevocList(void);

//Transaction Processing
int  EMVAppSelect(int Slot,unsigned long TransNo);
int  EMVReadAppData(void);
int  EMVCardAuth(void);
int  EMVSetAmount(unsigned char * szAuthAmount, unsigned char * szCashBackAmount);
int  EMVStartTrans(unsigned long ulAuthAmt, unsigned long ulCashBackAmt, 
				   unsigned char *pACType); 
int  EMVCompleteTrans(int nCommuStatus, unsigned char *paucScript, 
					  int *pnScriptLen, unsigned char *pACType); 

//Read Transaction log (for PBOC2.0)
int  EMVAppSelectForLog(int Slot, unsigned char ucFlg);
int  ReadLogRecord(int RecordNo);
int  GetLogItem(unsigned short Tag, unsigned char *TagData, int *TagLen);

// For CLSS PBOC
void EMVInitTLVData(void);
int  EMVSwitchClss(Clss_TransParam *ptTransParam,unsigned char *pucSelData, int nSelLen, 
				   unsigned char *pucGPOData, int nGPOLen);
int  EMVSetTmECParam(EMV_TMECPARAM *tParam);
int  EMVGetCardECBalance(unsigned long *plBalance);
//Read Loading Log (PBOC3.0)
int  EMVReadSingleLoadLog(int nRecordNoIn);
int  EMVGetSingleLoadLogItem(unsigned short usTagIn, unsigned char *paucDataOut, int *pnLenOut);
int  EMVReadAllLoadLogs(unsigned char *paucLogDataOut, int *pnLenOut);
int  EMVGetLogData(unsigned char *paucLogDataOut, int *pnLenOut);
int  EMVClearTransLog(void);

//Callback Functions
int  cEMVCandAppSel(int TryCnt, EMV_CANDLIST List[], int AppNum);  
int  cEMVInputAmount(unsigned long *AuthAmt, unsigned long *CashBackAmt);
int  cEMVGetHolderPwd(int TryFlag, int RemainCnt, unsigned char *pin);
void cEMVVerifyPINOK(void);
int  cCertVerify(void);//for PBOC
int  cEMVSetParam(void);

//Debug Interface
int  EMVGetVerifyICCStatus(unsigned char *pucSWA, unsigned char *pucSWB);
int  EMVGetDebugInfo(int nExpAssistInfoLen, unsigned char *paucAssistInfo, int *pnErrorCode);
//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------
#endif //_EMV_LIB_H
