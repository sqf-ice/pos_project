/*------------------------------------------------------------
* FileName: transmain.h
* Author: liukai
* Date: 2016-08-15
------------------------------------------------------------*/

#define EL_MAG_RET_READ_ERR	-1
#define EL_MAG_RET_RESWIPE		-2
#define EL_MAG_RET_INSERT_CARD	-3


//***************************************************************************************
//Transaction return code
//***************************************************************************************
#define EL_TRANS_RET_BASE						4000
#define EL_TRANS_RET_ICC_RESET_ERR				(EL_TRANS_RET_BASE + 1)
#define EL_TRANS_RET_ICC_CMD_ERR				(EL_TRANS_RET_BASE + 2)
#define EL_TRANS_RET_ICC_BLOCK					(EL_TRANS_RET_BASE + 3)
#define EL_TRANS_RET_EMV_RSP_ERR				(EL_TRANS_RET_BASE + 4)
#define EL_TRANS_RET_EMV_APP_BLOCK				(EL_TRANS_RET_BASE + 5)
#define EL_TRANS_RET_EMV_NO_APP					(EL_TRANS_RET_BASE + 6)
#define EL_TRANS_RET_EMV_USER_CANCEL			(EL_TRANS_RET_BASE + 7)
#define EL_TRANS_RET_EMV_TIME_OUT				(EL_TRANS_RET_BASE + 8)
#define EL_TRANS_RET_EMV_DATA_ERR				(EL_TRANS_RET_BASE + 9)
#define EL_TRANS_RET_EMV_NOT_ACCEPT				(EL_TRANS_RET_BASE + 10)
#define EL_TRANS_RET_EMV_DENIAL					(EL_TRANS_RET_BASE + 11)
#define EL_TRANS_RET_EMV_KEY_EXP				(EL_TRANS_RET_BASE + 12)
#define EL_TRANS_RET_EMV_NO_PINPAD				(EL_TRANS_RET_BASE + 13)
#define EL_TRANS_RET_EMV_NO_PASSWORD			(EL_TRANS_RET_BASE + 14)
#define EL_TRANS_RET_EMV_SUM_ERR				(EL_TRANS_RET_BASE + 15)
#define EL_TRANS_RET_EMV_NOT_FOUND				(EL_TRANS_RET_BASE + 16)
#define EL_TRANS_RET_EMV_NO_DATA				(EL_TRANS_RET_BASE + 17)
#define EL_TRANS_RET_EMV_OVERFLOW				(EL_TRANS_RET_BASE + 18)
#define EL_TRANS_RET_NO_TRANS_LOG				(EL_TRANS_RET_BASE + 19)
#define EL_TRANS_RET_RECORD_NOTEXIST			(EL_TRANS_RET_BASE + 20)
#define EL_TRANS_RET_LOGITEM_NOTEXIST			(EL_TRANS_RET_BASE + 21)
#define EL_TRANS_RET_ICC_RSP_6985				(EL_TRANS_RET_BASE + 22)
#define EL_TRANS_RET_CLSS_USE_CONTACT			(EL_TRANS_RET_BASE + 23)
#define EL_TRANS_RET_EMV_FILE_ERR				(EL_TRANS_RET_BASE + 24)
#define EL_TRANS_RET_CLSS_TERMINATE				(EL_TRANS_RET_BASE + 25)
#define EL_TRANS_RET_CLSS_FAILED				(EL_TRANS_RET_BASE + 26)
#define EL_TRANS_RET_CLSS_DECLINE				(EL_TRANS_RET_BASE + 27)
#define EL_TRANS_RET_CLSS_TRY_ANOTHER_CARD		(EL_TRANS_RET_BASE + 28)
//#define EL_TRANS_RET_						(EL_TRANS_RET_BASE + 29)
#define EL_TRANS_RET_PARAM_ERR					(EL_TRANS_RET_BASE + 30)
#define EL_TRANS_RET_CLSS_WAVE2_OVERSEA			(EL_TRANS_RET_BASE + 31)
#define EL_TRANS_RET_CLSS_WAVE2_TERMINATED		(EL_TRANS_RET_BASE + 32)
#define EL_TRANS_RET_CLSS_WAVE2_US_CARD			(EL_TRANS_RET_BASE + 33)
#define EL_TRANS_RET_CLSS_WAVE3_INS_CARD		(EL_TRANS_RET_BASE + 34)
#define EL_TRANS_RET_CLSS_RESELECT_APP			(EL_TRANS_RET_BASE + 35)
#define EL_TRANS_RET_CLSS_CARD_EXPIRED			(EL_TRANS_RET_BASE + 36)
#define EL_TRANS_RET_EMV_NO_APP_PPSE_ERR		(EL_TRANS_RET_BASE + 37)
#define EL_TRANS_RET_CLSS_USE_VSDC				(EL_TRANS_RET_BASE + 38)
#define EL_TRANS_RET_CLSS_CVMDECLINE			(EL_TRANS_RET_BASE + 39)
#define EL_TRANS_RET_CLSS_REFER_CONSUMER_DEVICE	(EL_TRANS_RET_BASE + 40)
#define EL_TRANS_RET_CLSS_LAST_CMD_ERR			(EL_TRANS_RET_BASE + 41)
#define EL_TRANS_RET_CLSS_API_ORDER_ERR			(EL_TRANS_RET_BASE + 42)
#define EL_TRANS_RET_EMV_FAIL					(EL_TRANS_RET_BASE + 43)	// by ZhangYuan
#define EL_TRANS_RET_EMV_DECLINED				(EL_TRANS_RET_BASE + 44)


#define EL_TRANS_RET_END						(EL_TRANS_RET_BASE + 999)


//******************************************************************************************


//functions
/*************************************************************************
Function: 						StartTransaction
Parameters Input:
Parameters Output:
Return Code:					Find the definations.
Description:					Start transaction function, external interface.
*************************************************************************/
int StartTransaction(void);

/*************************************************************************
Function: 						CompleteTransaction
Parameters Input:
Parameters Output:
Return Code:					Find the definations.
Description:					complete transaction function, external interface.
*************************************************************************/
int CompleteTransaction(void);

int GetPanFromTrack(const uchar *pucTrackTwoData, const uchar *pucTrackThreeData, uchar *pszPAN, uchar *pszExpDate);
