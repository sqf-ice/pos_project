/*------------------------------------------------------------
* FileName: parammgmt.h
* Author: liukai
* Date: 2016-08-03
------------------------------------------------------------*/
#ifndef PARAMMGMT_H
#define PARAMMGMT_H


//return code
#define EL_PARAM_RET_BASE			5000
#define EL_PARAM_RET_ERR_DATA		(EL_PARAM_RET_BASE + 1)
#define EL_PARAM_RET_INVALID_PARAM	(EL_PARAM_RET_BASE + 2)
#define EL_PARAM_RET_PARTIAL_FAILED	(EL_PARAM_RET_BASE + 3)
#define EL_PARAM_RET_ALL_FAILED		(EL_PARAM_RET_BASE + 4)
#define EL_PARAM_RET_SIZE_SMALL		(EL_PARAM_RET_BASE + 5)
#define EL_PARAM_RET_API_ORDER_ERR	(EL_PARAM_RET_BASE + 6)
#define EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED	(EL_PARAM_RET_BASE + 7)

#define CONFIGURATION_TAG_LENGTH 2
#define TRANS_PARAM_FILE "transParamFile"

#define PARAMMGMT_DATATYPE_EMV			1
#define PARAMMGMT_DATATYPE_CONFIG		2

#define MAX_TLVDATA_LEN		1024

typedef enum ETransInputType {
	TRANS_TYPE_NONE	= 0,
	TRANS_TYPE_MAGNETIC = 1,
	TRANS_TYPE_FALLBACK_MAGNETIC = 2,	//added by Kevin Liu 20160901
	TRANS_TYPE_EMV_CONTACT = 3,	
	TRANS_TYPE_CONTACTLESS = 4,
} TRANSINPUTTYPE;

typedef enum ECLSSKernelType {
	CLSS_KERNEL_DEF = 0,
	CLSS_KERNEL_JCB = 1,
	CLSS_KERNEL_MC = 2,
	CLSS_KERNEL_VISA = 3,
	CLSS_KERNEL_PBOC = 4,
	CLSS_KERNEL_AE = 5,
	CLSS_KERNEL_ZIP = 6,
	CLSS_KERNEL_FLASH = 7,
	CLSS_KERNEL_EFT = 8,
	CLSS_KERNEL_RFU = 0xFF,
} CLSSKERNELTYPE;

typedef enum ETransPathType {
	TRANS_PATH_TYPE_CLSS_PATH_NORMAL = 0,
	TRANS_PATH_TYPE_CLSS_VISA_MSD = 1,
	TRANS_PATH_TYPE_CLSS_VISA_QVSDC = 2,
	TRANS_PATH_TYPE_CLSS_VISA_VSDC = 3,
	TRANS_PATH_TYPE_CLSS_VISA_CONTACT = 4,
	TRANS_PATH_TYPE_CLSS_MC_MAG = 5,
	TRANS_PATH_TYPE_CLSS_MC_MCHIP = 6,
	TRANS_PATH_TYPE_CLSS_VISA_WAVE2 = 7,
	TRANS_PATH_TYPE_CLSS_JCB_WAVE2 = 8,
	TRANS_PATH_TYPE_CLSS_VISA_MSD_CVN17 = 9,
	TRANS_PATH_TYPE_CLSS_VISA_MSD_LEGACY = 10,
} TRANSPATHTYPE;



typedef enum EOnlineAuthResult {
	ONLINE_AUTH_RESULT_APPROVED = 0,
	ONLINE_AUTH_RESULT_DECLINED = 1,
	ONLINE_AUTH_RESULT_CONNECT_HOST_FAILED = 2,
} ONLINEAUTHRESULT;

/*
typedef enum EOnlineAuthResult {
	ONLINE_AUTH_RESULT_APPROVED = 0,
	ONLINE_AUTH_RESULT_FAILED = 1,
	ONLINE_AUTH_RESULT_REFER = 2,
	ONLINE_AUTH_RESULT_DENIAL = 2,
	ONLINE_AUTH_RESULT_ABORT = 2,
} ONLINEAUTHRESULT;
*/

// ====== Added by zhangyuan 2016-08-08 =============	
#define APPPARAM_PIN_ENCRYPT_TYPE_TDES		0x01
#define APPPARAM_PIN_ENCRYPT_TYPE_DUKPT		0x02

#define APPPARAM_DATA_ENCRYPT_TYPE_NONE		0x00
#define APPPARAM_DATA_ENCRYPT_TYPE_TDES		0x01
#define APPPARAM_DATA_ENCRYPT_TYPE_RSA		0x02
#define APPPARAM_DATA_ENCRYPT_TYPE_MAC		0x03

#define APPPARAM_FALLBACK_NOT_ALLOW			0x00
#define APPPARAM_FALLBACK_ALLOW				0x01
// ======= Added end ================================

#define CMD_TERM_SN						"\x01\x01"
#define CMD_TERM_MODEL_CODE				"\x01\x02"
#define CMD_TERM_PRINTER_INFO			"\x01\x03"
#define CMD_TERM_MODEM_EXIST			"\x01\x04"
#define CMD_TERM_USB_EXIST				"\x01\x05"
#define CMD_TERM_LAN_EXIST				"\x01\x06"
#define CMD_TERM_GPRS_EXIST				"\x01\x07"
#define CMD_TERM_CDMA_EXIST				"\x01\x08"
#define CMD_TERM_WIFI_EXIST				"\x01\x09"
#define CMD_TERM_RF_EXIST				"\x01\x10"
#define CMD_TERM_IC_EXIST				"\x01\x11"
#define CMD_TERM_MAG_EXIST				"\x01\x12"
#define CMD_TERM_TILT_EXIST				"\x01\x13"
#define CMD_TERM_WCDMA_EXIST				"\x01\x14"
#define CMD_TERM_TOUCHSCREEN_EXIST				"\x01\x15"
#define CMD_TERM_COLORSCREEN_EXIST				"\x01\x16"
#define CMD_TERM_SCREEN_SIZE				"\x01\x17"
#define CMD_TERM_APP_NAME				"\x01\x18"
#define CMD_TERM_APP_VERSION				"\x01\x19"
#define CMD_TERM_APPSO_NAME				"\x01\x20"
#define CMD_TERM_APPSO_VERSION				"\x01\x21"
#define CMD_TERM_PUBSO_NAME				"\x01\x22"
#define CMD_TERM_PUBSO_VERSION				"\x01\x23"
#define CMD_TERM_BATTERY_INFO				"\x01\x24"
#define CMD_TERM_REST_FS_SIZE				"\x01\x25"
#define CMD_TERM_OS_NAME				"\x01\x26"
#define CMD_TERM_OS_VERSION				"\x01\x27"
#define CMD_TERM_PCI_EXIST				"\x01\x28"
#define CMD_TERM_TIME					"\x01\x29"			//added by kevinliu 20161025

#define CMD_APP_SLEEP_TIMEOUT				"\x02\x01"
#define CMD_APP_PIN_ENCRYP_TYPE			"\x02\x02"
#define CMD_APP_PIN_ENCRYP_KEY_ID			"\x02\x03"
#define CMD_APP_PIN_BLOCK_MODE			"\x02\x04"
#define CMD_APP_DATA_ENCRYP_TYPE			"\x02\x05"
#define CMD_APP_DATA_ENCRYP_KEY_ID		"\x02\x06"
#define CMD_APP_FALLBACK_ALLOW		"\x02\x07"
#define CMD_APP_PAN_MASK_START_POS		"\x02\x08"

#define CMD_TXN_CUR_TXN_TYPE				"\x03\x01"
#define CMD_TXN_CUR_CLSS_TYPE				"\x03\x02"
#define CMD_TXN_PATH_TYPE					"\x03\x03"
#define CMD_TXN_TRACK_ONE_DATA			"\x03\x04"
#define CMD_TXN_TRACK_TWO_DATA			"\x03\x05"
#define CMD_TXN_TRACK_THREE_DATA			"\x03\x06"
#define CMD_TXN_EXPIRE_DATE				"\x03\x07"

#define CMD_TXN_ONLINE_RESULT				"\x03\x08"
#define CMD_TXN_RSP_CODE					"\x03\x09"
#define CMD_TXN_AUTH_CODE					"\x03\x10"
#define CMD_TXN_AUTH_DATA					"\x03\x11"
#define CMD_TXN_AUTH_DATA_LEN				"\x03\x12"
#define CMD_TXN_ISSUER_SCRIPT				"\x03\x13"
#define CMD_TXN_ISSUER_SCRIPT_LEN			"\x03\x14"
#define CMD_TXN_ONLINE_PIN_INPUT			"\x03\x15"
#define CMD_TXN_PIN_BLOCK					"\x03\x16"
#define CMD_TXN_KSN						"\x03\x17"

#define CMD_TXN_ICS_TYPE					"\x03\x18"
#define CMD_TXN_MASKEDPAN					"\x03\x19"

#define CMD_TXN_CVM_TYPE					"\x03\x1A"	// Added by ZhangYuan 2016-12-19, only for EMV contactless card
#define CMD_TXN_CARD_PROCESSING_RESULT		"\x03\x1B"  // Added by ZhangYuan 2017-04-17, only for EMV contact, EMV contactless card
//#define CMD_TXN_PAN_DATA					"\x03\x1C"	// Added by ZhangYuan 2017-04-17


typedef struct STerminalInfo {
	unsigned char ucTermSN[32+1];
	unsigned char ucTermModelCode[6+1];
	unsigned char ucTermPrinterInfo[1+1];
	unsigned char ucTermModemExist[1+1];
	unsigned char ucTermUSBExist[1+1];
	unsigned char ucTermLANExist[1+1];
	unsigned char ucTermGPRSExist[1+1];
	unsigned char ucTermCDMAExist[1+1];
	unsigned char ucTermWIFIExist[1+1];
	unsigned char ucTermRFExist[1+1];
	unsigned char ucTermICExist[1+1];
	unsigned char ucTermMAGExist[1+1];
	unsigned char ucTermTILTExist[1+1];
	unsigned char ucTermWCDMAExist[1+1];
	unsigned char ucTermTOUCHSCRExist[1+1];
	unsigned char ucTermCOLORSCRExist[1+1];
	unsigned char ucTermScrSize[4+1];
	
	unsigned char ucTermAppName[33+1];
	unsigned char ucTermAppVer[33+1];
	unsigned char ucTermAppSoName[33+1];
	unsigned char ucTermAppSoVer[33+1];
	unsigned char ucTermPubSoName[33+1];
	unsigned char ucTermPubSoVer[33+1];
	unsigned char ucTermBatteryInfo[1+1];
	unsigned char ucTermRestFSSize[12+1];
	unsigned char ucTermOSName[24+1];
	unsigned char ucTermOSVer[8+1];

	unsigned char ucTermPCIExist[1+1];

	unsigned char ucTermTime[7+1];		//added by kevinliu 20161025  YYMMDDHHMMSSWW
}  DATA_TERMINFO;

typedef struct SAppParam {
	unsigned char ucSleepModeTimeout[2+1];
	unsigned char ucPINEncryptionType[1+1];
	unsigned char ucPINEncryptionKeyIdx[1+1];
	unsigned char ucPINBlockMode[1+1]; 
	unsigned char ucDataEncryptionType[1+1];
	unsigned char ucDataEncryptionKeyIdx[2+1];
	unsigned char ucPANMaskStartPos[1+1];

	unsigned char ucFallbackAllowFlag[1+1];	// added by zhangyuan
} DATA_APPPARAM;

#define MODE_ONLINE_PIN_INPUT	0x01	// added by zhangyuan
typedef struct STransParam {
	unsigned char ucCurrentTxnType[1+1];
	unsigned char ucCurrentCLSSType[1+1];
	unsigned char ucCurrentPathType[1+1];
	unsigned char ucCurrentCVMType[1+1];	// Added by ZhangYuan 2016-12-19
	unsigned char ucCardProcessResult[1+1];	// Added by ZhangYuan 2017-04-17
	
	//unsigned char ucFallbackAllowFlag[1+1];		// noted by zhangyuan
	unsigned char ucTrackOneData[79+1];
	unsigned char ucTrackTwoData[40+1];
	unsigned char ucTrackThreeData[107+1];
	
	unsigned char ucPAN[19+1];	//internal use, added by Kevin Liu 20161024
	unsigned char ucExpDate[4+1];	//added by Kevin Liu 20161107
	
	unsigned char ucOnlineResult[1+1];		//modified by Kevin Liu 20160902
	unsigned char ucRspCode[2+1];			//added by Kevin Liu
	unsigned char ucAuthCode[6+1];
	unsigned char ucAuthData[16+1];
	unsigned char ucAuthDataLen[2+1];
	unsigned char ucIssuerScript[1024+1];
	unsigned char ucIssuerScriptLen[2+1];

	// --- Added by zhangyuan 2016-08-08 --------
	unsigned char ucOnlinePinInput[1+1];		// Online PIN input: 0-no, 1-yes;
	unsigned char ucPinBlock[8+1];
	unsigned char ucKsn[10+1];
	// --- Added end ----------------------------

	unsigned char ucICSType[128+1];	//added by Kevin Liu 20161110, modified by ZhangYuan, enlarge to 128 bytes
	unsigned char ucMaskedPAN[32+1];
	
} DATA_TRANSPARAM;


//permission control
#define DATA_PERMISSION_WRITE			0x01
#define DATA_PERMISSION_READ			0x02
#define DATA_PERMISSION_READ_WRITE	0x03

typedef struct SMatchList {
	unsigned char ucTag[2+1];
	unsigned char *pIndicator;
	unsigned int uiBufLen;
	unsigned int uiDataLen;
	unsigned char ucPermission;
} DATA_MATCHLIST;

extern DATA_TERMINFO g_tTermInfo;
extern DATA_APPPARAM g_tAppParam;
extern DATA_TRANSPARAM g_tTransParam;
extern DATA_MATCHLIST g_tMatchList[];

//functions
/*************************************************************************
Function: 					SetConfigurationData
Parameters Input:				pucTag, pucData, iDataLen
Parameters Output:			
Return Code:					Find the definations.
Description:					Please use this function to set the data in the DATA_MATCHLIST.
*************************************************************************/
int SetConfigurationData(const unsigned char *pucTag, const unsigned char *pucData, int iDataLen);


/*************************************************************************
Function: 					GetConfigurationData
Parameters Input:				pucData
Parameters Output:			pucSuccessTagListOut, puiSuccessTagListLenOut
Return Code:					Find the definations.
Description:					Get configuration data from DATA_MATCHLIST.
*************************************************************************/
int GetConfigurationData(const unsigned char *pucTag, unsigned char *pucDataOut, int *piDataLenOut);


/*************************************************************************
Function: 					SetData
Parameters Input:				pucData
Parameters Output:			pucSuccessTagListOut, puiSuccessTagListLenOut
Return Code:					Find the definations.
Description:					set EMV and Contactless kernel tags and Configuration Data.
*************************************************************************/
int SetData(const unsigned char *pucData, unsigned char *pucSuccessTagListOut, unsigned int *puiSuccessTagListLenOut);


/*************************************************************************
Function: 					GetData
Parameters Input:				pucData
Parameters Output:			pucTLVListOut, puiTLVListLenOut
Return Code:					Find the definations.
Description:					get EMV and Contactless kernel tags and Configuration Data.
*************************************************************************/
int GetData(const unsigned char *pucData, unsigned char *pucTLVListOut, unsigned int *puiTLVListLenOut);

/*************************************************************************
Function: 					LoadTLVDataFromFSToMem
Parameters Input:				
Parameters Output:			
Return Code:					Find the definations.
Description:					load the TLV data from file system to memory.
*************************************************************************/
int LoadTLVDataFromFSToMem(void);


/*************************************************************************
Function: 					GetAllTLVDataFromMem
Parameters Input:				iDataOutBufLen
Parameters Output:			pucDataOutput, piDataLen
Return Code:					Find the definations.
Description:					get all EMV from memory what saved in SetData and loaded by LoadTLVDataFromFSToMem.
*************************************************************************/
int GetAllTLVDataFromMem(unsigned char *pucDataOut, int *piDataLen);


#endif
