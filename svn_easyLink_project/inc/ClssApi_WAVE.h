/*********************************************************/
/* ClssApi_WAVE.h                                        */
/* Define the Application Program Interface              */
/* of VISA payWave for PAX PaxMe platform POS terminals  */
/* Created by Liuxl at July 30, 2009                     */
/*********************************************************/

#ifndef _CLSS_PAYWAVE_H
#define _CLSS_PAYWAVE_H

#include "CL_common.h"

// Parameter processing
int  Clss_CoreInit_Wave(void);
int  Clss_ReadVerInfo_Wave(char *paucVer);
int  Clss_GetTLVData_Wave(unsigned short Tag, unsigned char *DtOut, int *nDtLen);
int  Clss_SetTLVData_Wave(unsigned short usTag, unsigned char *pucDtIn, int nDtLen);
 
int Clss_SetReaderParam_Wave(Clss_ReaderParam *ptParam);
int Clss_GetReaderParam_Wave(Clss_ReaderParam *ptParam);

void Clss_SetRdSchemeInfo_Wave(unsigned char ucNum, Clss_SchemeID_Info *pInfo);
void Clss_GetSchemeInfo_Wave(unsigned char *pNum, Clss_SchemeID_Info *pInfo);

int  Clss_SetEMVUnknowTLVData_Wave(unsigned short usTag, unsigned char *pDatIn, int nLen);

//CAPK
int  Clss_AddCAPK_Wave(EMV_CAPK  *capk );
int  Clss_GetCAPK_Wave(int Index, EMV_CAPK  *capk );
int  Clss_DelCAPK_Wave(unsigned char KeyID, unsigned char *RID);
void Clss_DelAllCAPK_Wave(void);

// Revocation list
int  Clss_AddRevocList_Wave(EMV_REVOCLIST *pRevocList);
int  Clss_DelRevocList_Wave(unsigned char ucIndex, unsigned char *pucRID);
void Clss_DelAllRevocList_Wave(void);
 
 //Transaction processing
int Clss_SetDRLParam_Wave(Clss_ProgramID tDRLParam);
int Clss_SetVisaAidParam_Wave(Clss_VisaAidParam *ptParam);
int Clss_SetTransData_Wave(Clss_TransParam *ptTransParam, Clss_PreProcInterInfo *ptInfo);
int Clss_SetFinalSelectData_Wave(unsigned char *paucDtIn, int nDtLen);
int Clss_Proctrans_Wave(unsigned char *pucTransPath, unsigned char *pucACType);

// only for qVSDC and wave2
int Clss_CardAuth_Wave(unsigned char *pucACType, unsigned char *pucDDAFailFlg);
void Clss_SetDDAOmitFlag_Wave(unsigned char ucOmitFlag);
int Clss_GetCvmType_Wave(unsigned char *pucType);
int Clss_ProcRestric_Wave(void);

//Issuer Update Processing
int Clss_IssuerAuth_Wave (unsigned char *pIAuthData, int nIAuthDataLen);
int Clss_IssScriptProc_Wave(unsigned char *pucScript, int nScriptLen);

int Clss_nGetTrack1MapData_Wave(unsigned char *pTrackData, int *pLen); 
int Clss_nGetTrack2MapData_Wave(unsigned char *pTrackData, int *pLen); 

unsigned char Clss_GetMSDType_Wave(unsigned char *ucMSDType);

//call back function
//unsigned char cPiccIsoCommand_Wave(unsigned char cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv);

//Debug interface
//#ifdef _LINUX_TERM
//int cClssGetUnknowTLVData_Wave(unsigned short usTag, unsigned char *pucData, int nLen);
//#else
//void Clss_SetDebug_Wave(int EnableFlag);
//void Clss_GetICCStatus_Wave(unsigned char *SWA, unsigned char *SWB);
//#endif

int Clss_GetDebugInfo_Wave(int nExpAssistInfoLen, unsigned char *paucAssistInfo, int *pnErrorCode);

int Clss_SetExtendFunc_Wave(unsigned char *paucFuncValue);

#endif
