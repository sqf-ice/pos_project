/*------------------------------------------------------------
 * FileName: flow.c
 * Author: zhouhong
 * Date: 2017-05-27
------------------------------------------------------------*/
#include "global.h"

int CardProccess()
{
	int iRet = 0;
	int iKey = -1;
	long lAmount = atol(g_tRequestVCInfo.szAmount);

	memset(&g_tM1CardInfo, 0, sizeof(M1_CARD_INFO));

	//open M1 card
	iRet = OpenMifare1Card();
	if (iRet != VC_OK) {
		Pax_Log(LOG_INFO, "%s - %d, OpenMifare1Card failed!", __FUNCTION__, __LINE__);
		return iRet;
	}

	//detect M1 card
	while (1) {
		iRet = DetectMifare1Card(&g_tM1CardInfo);
		if (iRet == VC_OK){
			/* success message*/
			break;
		}

		//user cancel
		if (!XuiHasKey())
		{
			continue;
		}
		iKey = XuiGetKey();

		if (iKey == XUI_KEYCANCEL)
		{
			iRet = CloseMifare1Card();
			if (iRet != VC_OK) {
				Pax_Log(LOG_INFO, "%s - %d, DetectMifare1Card user canceled!", __FUNCTION__, __LINE__);
				return iRet;
			}
			return VC_ERR_USERCANCEL;
		}
	}

	//get card Information
	iRet = GetMifareCardInfo(&g_tM1CardInfo);
	if (iRet != VC_OK) {
		Pax_Log(LOG_INFO, "%s - %d, GetMifareCardInfo failed!", __FUNCTION__, __LINE__);
		return ERR_M1_GET_CARD_INFO;
	}

	//if card is valid
	iRet = Mifare1CardVerify(&g_tM1CardInfo);
	if(iRet != VC_OK) {
		Pax_Log(LOG_INFO, "%s - %d, Mifare1CardVerify failed!", __FUNCTION__, __LINE__);
		return iRet;
	}

	//check balance
	if(atol(g_tM1CardInfo.szCardBalance) < lAmount){
		Pax_Log(LOG_INFO, "%s - %d, Card balance is too low!", __FUNCTION__, __LINE__);
		return  ERR_M1_LOW_BALANCE;
	}

	//do deduction
	if((iRet = Mifare1CardDebit(&g_tM1CardInfo, lAmount)) != VC_OK ) {
		Pax_Log(LOG_INFO, "%s - %d, Mifare1CardDebit failed!", __FUNCTION__, __LINE__);
		return ERR_M1_DEBIT;
	}

	if((iRet = Mifare1CardVerifyReadback(&g_tM1CardInfo, lAmount)) != VC_OK ) {
		Pax_Log(LOG_INFO, "%s - %d, Mifare1CardVerifyReadback failed!", __FUNCTION__, __LINE__);
		return iRet;
	}

	iRet = CloseMifare1Card();
	if (iRet != VC_OK) {
		Pax_Log(LOG_INFO, "%s - %d, CloseMifare1Card failed!", __FUNCTION__, __LINE__);
		return iRet;
	}

	return VC_OK;
}

int CheckOlineProccess()
{
	long lRetcode = 0;
	lRetcode = OnlineCheck();
	return lRetcode;
}
