/*****************************************************/
/* EmvIIApi.h                                        */
/* Define the Application Program Interface          */
/* of EMV L2 for All PAX POS terminals     		 */
/*****************************************************/
#ifndef _EMV_II_API_H
#define _EMV_II_API_H


//---------------------------------------------------------------------------
/******************************* Return Codes ******************************/
//---------------------------------------------------------------------------
#ifndef _CLSS_COMMON_H
#define EMV_OK                          0         //Success
#define ICC_RESET_ERR                   -1        //IC card reset is failed
#define ICC_CMD_ERR                     -2        //IC card command is failed
#define ICC_BLOCK                       -3        //IC card is blocked 
#define EMV_RSP_ERR                     -4        //Return code of IC card command is error
#define EMV_APP_BLOCK                   -5        //EMV Application is blocked
#define EMV_NO_APP                      -6        //There is no EMV Application in the IC card 
#define EMV_USER_CANCEL                 -7        //User canceled current operation or transaction
#define EMV_TIME_OUT                    -8        //User¡¯s operation is timeout
#define EMV_DATA_ERR                    -9        //IC card data error
#define EMV_NOT_ACCEPT                  -10       //Transaction is not accepted
#define EMV_DENIAL                      -11       //Transaction is denied
#define EMV_KEY_EXP                     -12       //Key is Expired
#define EMV_NO_PINPAD                   -13       //No PIN Pad or PIN Pad can¡¯t be used 
#define EMV_NO_PASSWORD                 -14       //No PIN Pad or PIN Pad can¡¯t be used 
#define EMV_SUM_ERR                     -15       //Sum of Key verify is error
#define EMV_NOT_FOUND                   -16       //Appointed Data Element can¡¯t be found
#define EMV_NO_DATA                     -17       //There is no data in the appointed Data Element
#define EMV_OVERFLOW                    -18       //Memory is overflow
#define NO_TRANS_LOG                    -19       //There is no Transaction log
#define RECORD_NOTEXIST                 -20       //Appointed log is not exist
#define LOGITEM_NOTEXIST                -21       //Appointed Label is not exist in current log record
#define ICC_RSP_6985                    -22       //Return Status Code of GPO is '6985'
#define CLSS_USE_CONTACT                -23
#define EMV_FILE_ERR                    -24       //File error
#define EMV_PARAM_ERR                   -30       //Parameter error.


#endif //_CLSS_COMMON_H
#define EMV_NEXT_CVM                    -53
#define EMV_PIN_BLOCK                   -54
#define EMV_QUIT_CVM                    -57
#define EMV_PIN_TRYAGAIN                -58
#define EMV_SELECT_NEXT                 -59
#define EMV_TERMINATE                   -60
//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/******************************* Macros declaration ************************/
//---------------------------------------------------------------------------

#ifndef _CLSS_COMMON_H
#define MAX_REVOCLIST_NUM               30        //Maximum number of Issuer Public Key Certificate Recover List for EMV kernel

#define PART_MATCH                      0x00      //Application selection matching flag (partial matching).
#define FULL_MATCH                      0x01      //Application selection matching flag (full matching).

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

//Online Result
#define REFER_APPROVE                   0x01      //Reference Return code (choose Approved)
#define REFER_DENIAL                    0x02      //Reference Return code (choose Denial)
#define ONLINE_APPROVE                  0x00      //Online Return code (Online Approved) 
#define ONLINE_FAILED                   0x01      //Online Return code (Online Failed)
#define ONLINE_REFER                    0x02      //Online Return code (Online Reference)
#define ONLINE_DENIAL                   0x03      //Online Return code (Online Denial)
#define ONLINE_ABORT                    0x04      //Compatible PBOC(Transaction Terminate)

//AC Type
#define AC_AAC                          0x00      //The AC type is AAC
#define AC_TC                           0x01      //The AC type is TC
#define AC_ARQC                         0x02      //The AC type is ARQC
#endif  //_CLSS_COMMON_H                                         	

#define AC_AAC_HOST                     0x03      //The AC type is AAC since the online result is ONLINE_DENIAL.


#define EMV_PED_TIMEOUT                 0x01      // PCI verify offline PIN, PIN input timeout.
#define EMV_PED_WAIT                    0x02      // PCI verify offline PIN, PIN input interval not enough.
#define EMV_PED_FAIL                    0x03      // PCI verify offline PIN, other failure.

#define MAX_APP_ITEMS                   5


#define EMV_SCRIPT_PROC_UNIONPAY        1         //PBOC unionpay
#define EMV_SCRIPT_PROC_NORMAL          0

//CVM type
#define EMV_CVM_FAIL_CVM                0x00
#define EMV_CVM_PLAIN_PIN               0x01
#define EMV_CVM_ONLINE_PIN              0x02
#define EMV_CVM_PLAIN_PIN_SIG           0x03
#define EMV_CVM_ENCIPH_PIN              0x04
#define EMV_CVM_ENCIPH_PIN_SIG          0x05
#define EMV_CVM_SIGNATURE               0x1E
#define EMV_CVM_NO_CVM                  0x1F
#define EMV_CVM_CERTIFICATE             0x20
#define EMV_CVM_NULL                    0xFF

#define EMV_LAST_TRANS_AMOUNT           0xFF01
//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/******************************* Structure declaration *********************/
//---------------------------------------------------------------------------
#ifndef _CLSS_COMMON_H
//for clss pboc
typedef struct
{
    unsigned long ulAmntAuth;
    unsigned long ulAmntOther;
    unsigned long ulTransNo;
    unsigned char ucTransType;
    unsigned char aucTransDate[4];
    unsigned char aucTransTime[4];
}Clss_TransParam;
#endif //_CLSS_COMMON_H

typedef struct {
    unsigned char aucRID[5];
    unsigned char ucKeyID;
    unsigned char ucHashInd;
    unsigned char ucArithInd;
    unsigned char ucModulLen;
    unsigned char aucModul[248];
    unsigned char ucExponentLen;
    unsigned char aucExponent[3];
    unsigned char aucExpDate[3];
    unsigned char aucCheckSum[20];
}EMV_II_CAPK;


typedef  struct
{
    unsigned char aucRid[5];
    unsigned char ucIndex;
    unsigned char aucCertSn[3];
    unsigned char aucRFU[3];
}EMV_II_REVOCLIST;


// for PBOC EC
typedef struct
{
    unsigned long ulECTTLVal;
    unsigned char ucECTSIFlg;
    unsigned char ucECTSIVal;
    unsigned char ucECTTLFlg;
    unsigned char aucRFU[1];
}EMV_II_TMECPARAM;

typedef struct{
    unsigned long ulReferCurrCon;
    unsigned char aucMerchName[256];
    unsigned char aucMerchCateCode[2];
    unsigned char aucMerchId[15];  
    unsigned char aucTermId[8];
    unsigned char ucTerminalType;
    unsigned char aucCapability[3];
    unsigned char aucExCapability[5];
    unsigned char ucTransCurrExp;
    unsigned char ucReferCurrExp;
    unsigned char aucReferCurrCode[2];
    unsigned char aucCountryCode[2];
    unsigned char aucTransCurrCode[2];
    unsigned char ucTransType;
    unsigned char ucForceOnline;
    unsigned char ucGetDataPIN;
    unsigned char ucSurportPSESel;
    unsigned char ucUseTermAIPFlg;
    unsigned char aucTermAIP[2];
    unsigned char ucBypassAllFlg;
    unsigned char ucBypassPin;
    unsigned char ucBatchCapture;
    unsigned char ucAdviceFlg;
    unsigned char ucScriptMethod;
    unsigned char ucForceAccept;
    unsigned char aucRFU[1];
}EMV_II_TERMPARAM;


typedef struct{
    unsigned char aucAppPreName[17];
    unsigned char aucAppLabel[17];
    unsigned char aucIssDiscrData[244];
    unsigned char aucAID[17];
    unsigned char ucAidLen;
    unsigned char ucPriority;
    unsigned char aucAppName[33];
    unsigned char aucRFU[2];
}EMV_II_CANDLIST;


typedef struct{
    unsigned long ulFloorLimit;
    unsigned long ulThreshold;
    unsigned char ucTargetPer;
    unsigned char ucMaxTargetPer;
    unsigned char ucFloorLimitCheck;
    unsigned char ucRandTransSel;
    unsigned char ucVelocityCheck;
    unsigned char aucTACDenial[6];
    unsigned char aucTACOnline[6];
    unsigned char aucTACDefault[6];
    unsigned char aucAcquierId[6];
    unsigned char aucdDOL[256];
    unsigned char auctDOL[256];
    unsigned char aucVersion[3];
    unsigned char aucRiskManData[10];
    unsigned char aucRFU[2];
}EMV_II_AIDPARAM;


//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/****************************** Function declaration ***********************/
//---------------------------------------------------------------------------
int EMV_II_CoreInit(void);
int EMV_II_ReadVerInfo(char *paucVer);
int EMV_II_GetErrorCode(void);

void EMV_II_SetTermParam(EMV_II_TERMPARAM *ptTermParam);
void EMV_II_GetTermParam(EMV_II_TERMPARAM *ptTermParam);

int EMV_II_SetTLVDataList(unsigned char *pucTLVDatas, unsigned int unDataLen);
int EMV_II_CheckTagListValid(unsigned char *paucTLVDataList, unsigned int unTLVDataListLen, unsigned char *paucWrongTag, unsigned int *punTagLen);
int EMV_II_GetTLVDataList(unsigned char *pucTagList, unsigned char ucTagListLen, 
		unsigned int unExpectDataLen,unsigned char *pucDataOut, unsigned int *punActualDataOutLen);


void EMV_II_DelAllAidList(void);

int EMV_II_AddAidList(unsigned char *paucAID, unsigned char ucAidLen, unsigned char ucSelFlg);
int EMV_II_GetAidList(int nIndex, unsigned char *paucAID, unsigned char *pucAidLen, unsigned char *pucSelFlg);

int EMV_II_GetAidParam(EMV_II_AIDPARAM *ptAidParam);
int EMV_II_SetAidParam(EMV_II_AIDPARAM *ptAidParam);


int EMV_II_AddCAPK(EMV_II_CAPK *ptcapk);
int EMV_II_GetCAPK(int nIndex, EMV_II_CAPK *ptcapk);

void EMV_II_DelAllCAPK(void);


int EMV_II_AddRevocList(EMV_II_REVOCLIST *ptRevocList);

int EMV_II_DelAllRevocList(void);


int EMV_II_AppSelect(EMV_II_CANDLIST *ptCandList,int *pnAppNum);
int EMV_II_FinalSelect(unsigned char ucSelAppNo, EMV_II_CANDLIST *ptCandList,int *pnAppNum);

int EMV_II_InitApp(EMV_II_CANDLIST *ptCandList,int *pnAppNum );
int EMV_II_ReadAppData(void);
int EMV_II_CardAuth(void);
int EMV_II_ProcRestric(void);
int EMV_II_RiskManagement(unsigned char ucBlackCardFlg);


int EMV_II_StartCVM(unsigned char *pucCVMType,unsigned char *pucPINCnt);
int EMV_II_CompleteCVM(int nCVMResult,unsigned char *paucPINData,unsigned char *pucPINTryCnt);

int EMV_II_TermActAnalyse(unsigned char *pucACType);

int EMV_II_CompleteTrans(int nCommuStatus, unsigned char *paucScriptIn, 
		int *pnScriptLenIn,unsigned char *paucScriptRstOut, int *pnRstLenOut,unsigned char *pucACTypeOut);

int EMV_II_GetParamFlag(unsigned char ucParam, int *pnFlg);

int EMV_II_GetCardECBalance_PBOC(unsigned long *pulECBalance);
int EMV_II_SetTmECParam_PBOC(EMV_II_TMECPARAM *ptPBOCECParam);
int EMV_II_GetTmECParam_PBOC(EMV_II_TMECPARAM *ptPBOCECParam);

int EMV_II_InitTLVDataClss_PBOC(void);

int EMV_II_SwitchClss_PBOC(Clss_TransParam *ptTransParam,unsigned char *paucSelData,
						   int nSelLen, unsigned char *paucGPOData, int nGPOLen);

int EMV_II_SetAppSelectForLog_PBOC(unsigned char ucReadLogFlag, unsigned char ucBlockAddFlg);

int EMV_II_ReadLogRecord_PBOC(unsigned char ucLogType,int nRecordNo,unsigned char *paucLogDataOut, int *pnLenOut);

int EMV_II_GetDebugInfo(int nExpAssistInfoLen, unsigned char *paucAssistInfo, int *pnErrorCode);
//---------------------------------------------------------------------------
/******************************* End ***************************************/
//---------------------------------------------------------------------------
#endif //_EMV_II_API_H
