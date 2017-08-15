/*------------------------------------------------------------
* FileName: display.h
* Author: liukai
* Date: 2016-03-24
------------------------------------------------------------*/
#ifndef DISPLAY_H
#define DISPLAY_H

typedef enum _msgType {
	MSGTYPE_NORMAL,
	MSGTYPE_LOADING,
	MSGTYPE_SUCCESS,
	MSGTYPE_WARNING,
	MSGTYPE_FAILURE
}MSGTYPE;

//functions
int Init_Display(void);
void Destroy_Display(void);
void Display_Prompt(const char *psztitle, const char *pszStr, MSGTYPE eMsgType, int iNeedConfirm);
void Display_Last_Result(const char *pszAmount, const char *pszOrderId, int iPaymentStatus);
void Display_PaymentStatus(const char *pszStr);
void Display_QRCode_Screen(const char *pszPosUnitIdQR);
void Beep_Prompt(char cFlag);
void Display_Error_Code(int iErrCode);

#endif
