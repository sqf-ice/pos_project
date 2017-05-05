/*
 * ============================================================================
 * COPYRIGHT
 *              Pax CORPORATION PROPRIETARY INFORMATION
 *   This software is supplied under the terms of a license agreement or
 *   nondisclosure agreement with Pax Corporation and may not be copied
 *   or disclosed except in accordance with the terms in that agreement.
 *      Copyright (C) 2016 Pax Corporation. All rights reserved.
 * Module Date: 2016/10/10     
 * Module Auth: ZhangYuan 
 * Description:   EMV contactless xml file operarte module

 *      
 * ============================================================================
 */


#ifndef _CLSS_EMV_FILE_OP_H
#define _CLSS_EMV_FILE_OP_H
#include "CL_common.h"

//#define CLSS_TEST

#define CLSS_XML_FILE_OP_ERR_BASE					-6000
#define CLSS_XML_FILE_OP_ERR_INVALID_PARAM			(CLSS_XML_FILE_OP_ERR_BASE-1)
#define CLSS_XML_FILE_OP_ERR_CLSS_TYPE_NOT_EXIST	(CLSS_XML_FILE_OP_ERR_BASE-2)

#define CLSS_XML_FILE_OP_ERR_NODE_NOT_EXIST         (CLSS_XML_FILE_OP_ERR_BASE-3)

#define CLSS_XML_FILE_OP_ERR_FUNCTIONE_NOT_FOUNTD   (CLSS_XML_FILE_OP_ERR_BASE-4)
#define CLSS_XML_FILE_OP_ERR_RECORD_NOT_FOUNTD     	(CLSS_XML_FILE_OP_ERR_BASE-5)
#define CLSS_XML_FILE_OP_ERR_AID_OVER_FLOW      	(CLSS_XML_FILE_OP_ERR_BASE-6)
#define CLSS_XML_FILE_OP_ERR_ONELINE_OVER_1024      (CLSS_XML_FILE_OP_ERR_BASE-7)




#define CLSS_XML_FILE_OP_RETURN_BASE				6000
#define CLSS_XML_FILE_OP_NOT_EXIST					(CLSS_XML_FILE_OP_RETURN_BASE+1)
#define CLSS_XML_FILE_OP_NOT_FOUND_NODE				(CLSS_XML_FILE_OP_RETURN_BASE+2)
#define CLSS_XML_FILE_OP_FOUND_NODE					(CLSS_XML_FILE_OP_RETURN_BASE+3)
#define CLSS_XML_FILE_OP_NOT_FOUND_RETURN			(CLSS_XML_FILE_OP_RETURN_BASE+4)
#define CLSS_XML_FILE_LIST_END              	    (CLSS_XML_FILE_OP_RETURN_BASE+5)
#define CLSS_XML_FILE_NODE_END              	    (CLSS_XML_FILE_OP_RETURN_BASE+6)

#define CLSS_XML_FILE_OP_OK                         0

#define CLSS_PRASED_RECORD_MAX_NODE_CNT                   10

#define CLSS_BUF_NODE_VALUE_LEN                1024
#define CLSS_TEMP_BUF_LEN                      2048

#define CLSS_FILE_READ_BUF_LEN                 1024
#define CLSS_FILE_ONELINE_BUF_LEN              1024

#define MAX_TRANSTYPE_NUM   6


#define CLSS_XML_FILE_NAME                			"clss_param.clss"
#define CLSS_XML_FILE_HAVE_PARSED           		"clss_parsed.clss" 


#define CLSS_PAYPASS_AID_NODE                            "PAYPASS_AID"
#define CLSS_PAYPASS_CONF_NODE                           "PAYPASS_PAYPASSCONFIGURATION"
#define CLSS_PAYWAVE_AID_NODE                            "PAYWAVE_AID"
#define CLSS_PAYWAVE_INTER_WARE_NODE                     "PAYWAVE_INTER_WARE"
#define CLSS_PAYWAVE_PROGRAM_ID_NODE                     "PAYWAVE_PROGRAMID"
#define CLSS_PAYWAVE_TERM_CAP_NODE                       "PAYWAVE_TERM_CAP"
#define CLSS_OTHER_CONF_NODE                             "CLSS_OHTER_CONF"

#define CLSS_EXPRESSPAY_AID_NODE						"EXPRESSPAY_AID"
#define CLSS_EXPRESSPAY_CONFIG_NODE						"EXPRESSPAY_CONFIG"


typedef int (*CLSS_PARSE_CB)(const uchar *pucXmlOneLine, void *pucOutData);

typedef struct Clss_Parse
{
	int NodeType;
	CLSS_PARSE_CB ClssParseCb;
	
}CLSS_PARSE;

typedef enum {
	CLSS_PAYPASS = 1,		
	CLSS_PAYWAVE,		
	CLSS_EXPRESSPAY,
	CLSS_COMMON,	

	CLSS_END,
}CLSS_TYPE;


typedef enum{
	CLSS_PAYPASS_PARAM = 1,
	CLSS_PAYPASS_AIDLIST,
	CLSS_PAYPASS_CONFIG,
	CLSS_PAYPASS_AIDLIST_END,

	CLSS_PAYWAVE_PARAM,
	CLSS_PAYWAVE_AID,
	CLSS_PAYWAVE_AID_INTERWARE,
	CLSS_PAYWAVE_PROGRAM_ID,
	CLSS_PAYWAVE_TERM_CAPABILITY,

	CLSS_EXPRESSPAY_AIDLIST,
	CLSS_EXPRESSPAY_CONFIG,
	
	CLSS_OTHER_CONFIG,

	CLSS_PARAM_END,
}CLSS_NODE_TYPE;

typedef enum{
	PAYPASS_PARTIALAIDSELECTION = 10,	// PartialAIDSelection
	PAYPASS_APPLICATIONID,				// ApplicationID
	PAYPASS_IFUSELOCALNAME,				// IfUseLocalAIDName
	PAYPASS_LOACALAIDNAME,				// LocalAIDName
	PAYPASS_TERMINALAIDVERSION,			// TerminalAIDVersion
	PAYPASS_MAG_APP_VERSION_NUM,		// MagneticApplicationVersionNumber
	PAYPASS_TACDENIAL,					// TACDenial
	PAYPASS_TACONLINE,					// TACOnline
	PAYPASS_TACDEFAULT,					// TACDefault
	PAYPASS_TERMINAL_RISK,				// TerminalRisk
	PAYPASS_CLSS_CVM_LIMIT,				// ContactlessCVMLimit
	PAYPASS_CLSS_TRANS_LIMIT_NO_ON_DEVICE,	// ContactlessTransactionLimit_NoOnDevice
	PAYPASS_CLSS_TRANS_LIMIT_ON_DEVICE,		// ContactlessTransactionLimit_OnDevice
	PAYPASS_CLSS_FLOOR_LIMIT,				// ContactlessFloorLimit
}PAYPASS_AID_CHILD_NODE;


//CAP:Capability
typedef enum{
	PAYPASS_KERNEL_CONF = 30,	
	PAYPASS_TORN_LEFT_TIME,				
	PAYPASS_MAX_TORN_NUM,				
	PAYPASS_MAGENTIC_CVM,				
	PAYPASS_PAYPASS_MAGENTIC_NO_CVM,			
	PAYPASS_MOBILE_SUPPORT,		
	PAYPASS_CARD_DATA_INPUT,					
	PAYPASS_CVM_CAPA_CVM_REQ,					
	PAYPASS_CVM_CAPA_NO_CVM_REQ,					
	PAYPASS_TERMINAL_TYPE,				
	PAYPASS_ACCOUNT_TYPE,				
	PAYPASS_ADDITION_TEEM_CAP,	
	PAYPASS_KERNEL_ID,		
	PAYPASS_SECURITY_CAPALILITY,			
}PAYPASS_CONF_CHILD_NODE;


typedef enum{
	PAYWAVE_PARTIALAIDSELECTION = 50,	
	PAYWAVE_APPLICATIONID,				
	PAYWAVE_IFUSELOCALNAME,				
	PAYWAVE_LOACALAIDNAME,				
	PAYWAVE_TERMINALAIDVERSION,			
	PAYWAVE_CRY_VER17_SUPPORT,		
	PAYWAVE_ZERO_AMT_NO_ALLOW,					
	PAYWAVE_STATUS_CHECKSUPPORT,					
	PAYWAVE_READER_TTQ,	
	PAYWAVE_INTERWARELIST_START,
	PAYWAVE_INTERWARELIST_END,
	PAYWAVE_INTER_WARE_NODE_START,
	PAYWAVE_INTER_WARE_NODE_END ,
}PAYWAVE_AID_CHILD_NODE;

typedef enum{
	PAYWAVE_TRANS_TYPE = 70,	
	PAYWAVE_TERM_FLOOR_LIMIT,				
	PAYWAVE_CLSS_CVM_LIMIT,				
	PAYWAVE_CLSS_TRANS_LIMIT,				
	PAYWAVE_CLSS_FLOOR_LIMIT,			
	PAYWAVE_TERM_FLOOR_LIMINT_SUPPORT,		
	PAYWAVE_CLSS_TRANS_LIMIT_SUPPORT,					
	PAYWAVE_CVM_LIMIT_SUPPORT,					
	PAYWAVE_CLSS_FLOOR_LIMIT_SUPPORT,
}PAYWAVE_INTERWARE_CHILD_NODE;


typedef enum{
	PAYWAVE_PROGRAM_ID = 90,	
	PAYWAVE_PROGRAM_ID_TERM_FLOOR_LIMIT,				
	PAYWAVE_PROGRAM_ID_CLSS_CVM_LIMIT,				
	PAYWAVE_PROGRAM_ID_CLSS_TRANS_LIMIT,
	PAYWAVE_PROGRAM_ID_CLSS_FLOOR_LIMIT,
	PAYWAVE_PROGRAM_ID_TERM_FLOOR_LIMIT_SUPPORT,
	PAYWAVE_PROGRAM_ID_CLSS_TRANS_LIMIT_SUPPORT,
	PAYWAVE_PROGRAM_ID_CVM_LIMIT_SUPPORT,
	PAYWAVE_PROGRAM_ID_CLSS_FLOOR_LIMIT_SUPPORT,
	PAYWAVE_PROGRAM_ID_CRYPTO_VERSION_17_SUPPORT,
	PAYWAVE_PROGRAM_ID_ZERO_AMOUNT_NO_ALLOWED,
	PAYWAVE_PROGRAM_ID_STATUS_CHECK_SUPPORT,
	PAYWAVE_PROGRAM_ID_READER_TTQ,
}PAYWAVE_PROGRAM_ID_CHILD_NODE;


typedef enum{
	PAYWAVE_TERM_CAP_TERM_TYPE = 110,	
	PAYWAVE_TERM_CAP_CARD_DATA_INPUT,				
	PAYWAVE_TERM_CAP_CVM_SUPPORT,				
	PAYWAVE_TERM_CAP_OFF_DATA_AUTH,
	PAYWAVE_TERM_CAP_TRANS_TYPE_CAPA,
	PAYWAVE_TERM_CAP_TERM_INPUT,
	PAYWAVE_TERM_CAP_TERM_OUTPUT,
}PAYWAVE_TERM_CAP_CHILD_NODE;

typedef enum{
	CLSS_OHTER_COUNTRY_CODE = 110,	
	CLSS_OHTER_CURRENCY_CODE,				
	CLSS_OHTER_REF_CURRENCY_CODE,				
	CLSS_OHTER_CURRENCY_EXP,
	CLSS_OHTER_REF_CUR_EXP,
	CLSS_OHTER_REF_CUR_CONVER_RATE,
	CLSS_OHTER_MER_CAT_CODE,
	CLSS_OHTER_MERCHANT_ID,
	CLSS_OHTER_TERMINAL_ID,
	CLSS_OHTER_MERCHANT_NAME,
	CLSS_OHTER_MERCHANT_LOCAL_ADDR,
}CLSS_OTHER_CONF_CHILD_NODE;

typedef enum{
	EXPRESSPAY_PARTIAL_AID_SELECTION = 120,
	EXPRESSPAY_APPLICATIONID,
	EXPRESSPAY_IF_USE_LOCAL_NAME,
	EXPRESSPAY_LOCAL_AID_NAME,
	EXPRESSPAY_TERMINAL_AID_VERSION,
	EXPRESSPAY_TACDENIAL,
	EXPRESSPAY_TACONLINE,
	EXPRESSPAY_TACDEFAULT,
	EXPRESSPAY_DDOL,
	EXPRESSPAY_TDOL,
	EXPRESSPAY_TERMINAL_CAPABILITIES,
	EXPRESSPAY_CLSS_CVM_LIMIT,
	EXPRESSPAY_CLSS_TRANS_LIMIT,
	EXPRESSPAY_CLSS_FLOOR_LIMIT,
	
}EXPRESSPAY_AID_CHILD_NODE;

typedef enum{
	EXPRESSPAY_TERMINAL_SUPPORT_OPTIMIZATION_MODE_TRANS = 130,
	EXPRESSPAY_UNPREDICTABLE_NUMBER_RANGE,
	EXPRESSPAY_TERMINAL_TRANS_CAPABILITY,
	EXPRESSPAY_DEALY_AUTHORIZATION_SUPPORT,
	
}EXPRESSPAY_CONFIG_NODE;



typedef enum {
	KERNEL_CONFIG = 1,			// KernelConfiguration
	TORN_LEFT_TIME,				// TornLeftTime
	MAX_TORN_NUM,				// MaximumTornNumber
	MAG_CVM,					// MagneticCVM
	MAG_NO_CVM,					// MagneticNoCVM
	MOBILE_SUPPORT,				// MobileSupport
	CARD_DATA_INPUT,			// CardDataInput
	CVM_CAPABILITY_CVM_REQUIRED,	// CVMCapability_CVMRequired
	CVM_CAPABILITY_NO_CVM_REQUIRED,	// CVMCapability_NoCVMRequired
	TERMINAL_TYPE,					// TerminalType
	ACCOUNT_TYPE,					// AccoutType
	ADDITIONAL_TERMINAL_CAPABILITY,	// AdditionalTerminalCapability
	KERNEL_ID,						// KernelID
	SECURITY_CAPABILITY,			// SecurityCapability
}PAYPASS_CONFIG_CHILD_NODE;


typedef struct PayPassAidNode
{
	uchar ucSelFlag;
	uchar aucAID[17];
	uchar ucAidLen;
	uchar ucIfUseLocalName;
	uchar aucAppName[33]; 
	uchar aucVersion[3];
	uchar aucMagAppVersion[2];
	uchar aucTACDenial[5];
	uchar aucTACOnline[5]; 	 
	uchar aucTACDefault[5];
	ulong ulClssFloorLimit;
	uchar aucTermRisk[8]; 
	uchar aucTxnLmt_NoOnDevice[6];
	uchar aucTxnLmt_OnDevice[6];
	uchar aucRDCVMLmt[6];
	
}PAYPASS_AID_NODE_T;

typedef struct PayPassConf
{
	uchar ucKernelConf;
	uchar ucTornLeftTime;
	uchar ucMaximumTornNumber;
	uchar ucMagneticCVM; 
	uchar ucMageticNoCVM;
	uchar ucMobileSupport;
	uchar ucCardDataInput;
	uchar ucCVMCapability_CVMRequired;
	uchar ucCVMCapability_NoCVMRequired;
	uchar ucTerminalType;
	uchar ucAccoutType;
	uchar aucAdditionalTerminalCapability[5];
	uchar ucKernelID;
	uchar ucSecurityCapability;
}PAYPASS_CONF_T;


typedef struct{
	uchar      ucTransType;
	uchar      ucTermFLmtFlg;
	uchar      ucRdClssTxnLmtFlg;
	uchar      ucRdCVMLmtFlg;
	uchar      ucRdClssFLmtFlg;
	ulong 	   ulTermFLmt; 		
	ulong 	   ulcRdClssTxnLmt;  	
	ulong 	   ulRdCVMLmt;    	
	ulong 	   ulRdClssFLmt;	
}PAYWAVE_INTER_WARE_NODE_T;//Inter_WareFlmtByTransType;


typedef struct{
	uchar    aucAID[17];
	uchar	 ucAidLen;
	uchar    ucSelFlag;
	uchar 	 ucIfUseLocalName;
	uchar 	 aucAppName[16+1];
	uchar    aucAppVersion[2];
	uchar    ucCrypto17Flg;
	uchar    ucZeroAmtNoAllowed;
	uchar    ucStatusCheckFlg;
	uchar    aucReaderTTQ[5];
	PAYWAVE_INTER_WARE_NODE_T stFlmtByTransType[MAX_TRANSTYPE_NUM];
}PAYWAVE_AID_NODE_T;
/*
typedef struct{
	uchar    aucProgramId[17];
	uchar    ucPrgramIdLen;
	ulong    ulTerminalFloorLimit;
	ulong 	 ulContactlessCVMLimit;
	ulong 	 ulContactlessTransactionLimit;
	ulong    ulContactlessFloorLimit;
	uchar    ucTerminalFloorLimitSupported;
	uchar    ucContactlessTransactionLimitSupported;
	uchar    ucCVMLimitSupported;
	uchar    ucContactlessFloorLimitSupported;
	uchar 	 ucCryptogramVersion17Supported;
	uchar 	 ucZeroAmountNoAllowed;
	uchar    ucStatusCheckSupported;
	uchar    aucReaderTTQ[5];
	
}PAYWAVE_PROGREAM_ID_NODE_T;
*/

typedef struct{
	uchar    aucProgramId[17];
	uchar    ucPrgramIdLen;
	Clss_PreProcInfo tPreProcDRL;
	
}PAYWAVE_PROGREAM_ID_NODE_T;



typedef struct{ 
	uchar    ucTermType;
	uchar    ucCardDataInput;
	uchar    ucCVMSupport;
	uchar    ucOffDataAuth;
	uchar 	 ucTransTypeCapa;
	uchar 	 ucTermInput;
	uchar    ucTermOutput; 	
}PAYWAVE_TERM_CAPABILITY_NODE_T;

typedef struct{ 
	uchar    aucCountryCode[2];
	uchar    aucCurrencyCode[2];
	uchar    aucRefCurrencyCode[2];
	uchar    ucCurrencyExponent;
	uchar 	 ucRefCurrencyExponent;
	uchar 	 aucRefCurrenceConVerRate[2];
	uchar    aucMerchantCateCode[2];
	uchar    aucMerchantId[32];
	uchar    aucTerminalId[32];
	uchar    aucMerChantName[32];
	uchar    aucMerChantLocalAddr[32];
}CLSS_OTHER_CONF_NODE_T;

typedef struct {
	uchar ucSelFlag;
	uchar aucAID[17];
	uchar ucAidLen;
	uchar ucIfUseLocalName;
	uchar aucAppName[33]; 
	uchar aucVersion[3];
	uchar aucTACDenial[5];
	uchar aucTACOnline[5]; 	 
	uchar aucTACDefault[5];
	uchar aucDDOL[256];
	uchar aucTDOL[256];
	uchar aucTerminalCapability;
	uchar aucClssCVMLimit[6];
	uchar aucClssTransLimit[6];
	uchar aucClssFloorLimit[6];
	
} EXPRESSPAY_AID_NODE_T;

typedef struct {
	uchar ucTmSupOptTrans; 	// TerminalSupportOptimizationModeTransaction
	uchar aucUNRange[2]; 	//Unpredictable Number Range
	uchar aucTmTransCapa[4];	// TerminalTransactionCapability
	uchar ucDelayAuthFlag;	// DelayAuthorizationSupport
	
} EXPRESSPAY_CONFIG_NODE_T;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int ParseClssFile(const uchar *pucFileName);
int ClssGetNodeNum(const uchar *pucName);

int GetPayPassAidData(PAYPASS_AID_NODE_T *pstPayPassAidNode, int iSize, int iAidIndex);
int TestGetPayPassAid();

int GetPayPassConfData(PAYPASS_CONF_T *pstPayPassConfNode, int iSize, int iConfIndex);
int TestGetPayPassConf();


int GetPayWaveAidData(PAYWAVE_AID_NODE_T *pstPayWaveAidNode, int iSize, int iPayWaveAidIndex);
int TestGetPayWaveAid();

int GetPayWaveProgramIdData(PAYWAVE_PROGREAM_ID_NODE_T *pstPayWavePrgIdNode, int iSize, int iPrgIdIndex);
int TestGetPayWavePrgId();

int GetPayWaveTermCapData(PAYWAVE_TERM_CAPABILITY_NODE_T *pstPayWaveTermCapNode, int iSize, int iTermCapIndex);
int TestGetPayWaveTermCap();

int GetClssOtherCofData(CLSS_OTHER_CONF_NODE_T *pstOtherConfNode, int iSize, int iOtherCofIndex);
int TestGetOherConf();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#pragma pack()



#endif

