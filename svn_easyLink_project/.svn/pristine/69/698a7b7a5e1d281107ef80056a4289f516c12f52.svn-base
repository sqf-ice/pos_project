/*****************************************************/
/* ClEntryApi.h                                      */
/* Define the Application Program Interface          */
/* of Entry Point for all PAX Readers                */
/* Created by Liuxl at July 30, 2009                 */
/*****************************************************/

#ifndef _CLSS_ENTRY_LIB_H
#define _CLSS_ENTRY_LIB_H


#include "CL_common.h"


#ifndef _APDU_DEFINE_L2
#define _APDU_DEFINE_L2
typedef struct
{
    unsigned char       Command[4];
    unsigned short      Lc;
    unsigned char       DataIn[512];
    unsigned short      Le;
}APDU_SEND_L2;

typedef struct
{
    unsigned short      LenOut;
    unsigned char       DataOut[512];
    unsigned char       SWA;
    unsigned char       SWB;
}APDU_RESP_L2;
#endif

/* Candidate list */
typedef struct CLSS_TERMCANDLIST_STRUCT
{      
	unsigned char   ucIccAidLen;     
	unsigned char   aucIccAID[17]; //4F 
	unsigned char   ucAppLabelLen;
	unsigned char   aucAppLabel[17];//50
	unsigned char   ucPreferNameLen;
	unsigned char   aucPreferName[17];//9F12
	unsigned char   ucPriority; //81
	unsigned char   ucKernType;
	unsigned char   ucTmAidLen;     
	unsigned char   aucTmAID[17]; 
	//The presence of both of the following data elements identifies the AID as relating to a U.S. debit or prepaid program:
	unsigned char   aucIssCountryCode[2];// Issuer Country Code (2 digit alpha) (tag '5F55') with a value of '5553' ("US")
	unsigned char   aucIssIDNum[3];// Issuer Identification Number (IIN) (tag '42')
	unsigned char   aucRFU[5];
}Clss_CandList; 
int Clss_CoreInit_Entry(void);
int  Clss_ReadVerInfo_Entry(char *paucVer);

int Clss_SetPreProcInfo_Entry(Clss_PreProcInfo *ptPreProcInfoIn);
int Clss_PreTransProc_Entry(Clss_TransParam *pstTransParam);
int Clss_AppSlt_Entry(int Slot, int ReadLogFlag);
int Clss_FinalSelect_Entry(unsigned char *pucKernType, unsigned char *pucDtOut, int *pnDtLen);
int Clss_GetPreProcInterFlg_Entry(Clss_PreProcInterInfo *ptInfo);
//for paypass 3.0
int Clss_GetErrorCode_Entry(int *pnErrorCode);
int Clss_SetMCVersion_Entry(unsigned char ucVer);
//for PayWave
int Clss_SetExtendFunction_Entry(unsigned char *paucExFunc);

void Clss_DelAllAidList_Entry(void);
int Clss_DelAidList_Entry(unsigned char  *pucAID, unsigned char  ucAidLen);
int Clss_AddAidList_Entry(unsigned char *pucAID, unsigned char ucAidLen, unsigned char ucSelFlg, unsigned char ucKernType);
int Clss_DelCurCandApp_Entry(void);

int Clss_GetFinalSelectData_Entry(unsigned char *paucDtOut, int *pnDtLen);

void Clss_DelAllPreProcInfo(void);
int Clss_DelPreProcInfo_Entry(unsigned char  *pucAID, unsigned char  ucAidLen);

//unsigned char cPiccIsoCommand_Entry(unsigned char cid, APDU_SEND *ApduSend, APDU_RESP *ApduRecv);
typedef int  (*LCP_ADDAPDUTOTRANSLOG_ENTRY)(APDU_SEND_L2*, APDU_RESP_L2*);
int Clss_SetCBFun_AddAPDUToTransLog_Entry(LCP_ADDAPDUTOTRANSLOG_ENTRY cClssAddAPDUToTransLog);

int clss_AppSelect_Entry_UnlockApp(Clss_TransParam *ptTransParam, ClssTmAidList *ptTermAid);

int Clss_GetCandList_Entry(Clss_CandList *patCandList, int *pnCandListNum);
int Clss_SetCandList_Entry(Clss_CandList *patCandList, int nCandListNum);
#endif
