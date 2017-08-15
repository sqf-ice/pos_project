/*------------------------------------------------------------
* FileName: main.c
* Author: liukai
* Date: 2016-02-22
------------------------------------------------------------*/

#include "global.h"

static int Load_Paramters(MOBILEPAYDATACONF *ptMPconfig, MOBILEPAYDATAECR *ptMPdata)
{
	ST_TIME tTime;
	TXNTOTALS tTxnTotals;

	if ( (ptMPconfig == NULL) || (ptMPdata == NULL) ) {
		return MP_ERR_INVALID_PARAM;
	}

	//debug
	Pax_Log(LOG_INFO, "%s - %d load", __FUNCTION__, __LINE__);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->cReason = (%c)", __FUNCTION__, __LINE__, ptMPdata->cReason);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szAmount = (%s)", __FUNCTION__, __LINE__, ptMPdata->szAmount);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szOrderID = (%s)", __FUNCTION__, __LINE__, ptMPdata->szOrderID);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szBulkRef = (%s)", __FUNCTION__, __LINE__, ptMPdata->szBulkRef);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->cAppStatus = (%c)", __FUNCTION__, __LINE__, ptMPdata->cAppStatus);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szCustomerID = (%s)", __FUNCTION__, __LINE__, ptMPdata->szCustomerID);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szCustomerReceiptToken = (%s)", __FUNCTION__, __LINE__, ptMPdata->szCustomerReceiptToken);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szCustomerToken = (%s)", __FUNCTION__, __LINE__, ptMPdata->szCustomerToken);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szPaymentStatus = (%s)", __FUNCTION__, __LINE__, ptMPdata->szPaymentStatus);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szReminder = (%s)", __FUNCTION__, __LINE__, ptMPdata->szReminder);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szStatusCode = (%s)", __FUNCTION__, __LINE__, ptMPdata->szStatusCode);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szStatusText = (%s)", __FUNCTION__, __LINE__, ptMPdata->szStatusText);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szTransactionID = (%s)", __FUNCTION__, __LINE__, ptMPdata->szTransactionID);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szPaymentsTotal = (%s)", __FUNCTION__, __LINE__, ptMPdata->szPaymentsTotal);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szRefundsTotal = (%s)", __FUNCTION__, __LINE__, ptMPdata->szRefundsTotal);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szTotals = (%s)", __FUNCTION__, __LINE__, ptMPdata->szTotals);

	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szMerchantID = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szMerchantID);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szLocationID = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szLocationID);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szName = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szPoSname);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szAPIkey = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szAPIkey);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szIPorURL = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szIPorURL);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->usPort = (%d)", __FUNCTION__, __LINE__, ptMPconfig->usPort);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->ucConnDevice = (%c)", __FUNCTION__, __LINE__, ptMPconfig->ucConnDevice);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->lGPRSauth = (%d)", __FUNCTION__, __LINE__, ptMPconfig->lGPRSauth);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szGPRSapn = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szGPRSapn);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szGPRSpassword = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szGPRSpassword);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->szGPRSusername = (%s)", __FUNCTION__, __LINE__, ptMPconfig->szGPRSusername);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->tWiFiSettings.Essid = (%s)", __FUNCTION__, __LINE__, ptMPconfig->tWiFiSettings.Essid);
	Pax_Log(LOG_INFO, "%s - %d ptMPconfig->tWiFiSettings.Essid = (%d)", __FUNCTION__, __LINE__, ptMPconfig->tWiFiSettings.AuthMode);

	if ( strlen(ptMPconfig->szMerchantID) == 0 || strlen(ptMPconfig->szLocationID) == 0 ||
			strlen(ptMPconfig->szAPIkey) == 0 || strlen(ptMPconfig->szPoSname) == 0 ||
			strlen(ptMPconfig->szIPorURL) == 0 ) {
		return MP_ERR_INVALID_PARAM;
	}

	if ( ptMPdata->cReason == MOBILEPAY_PAY_REQUEST || ptMPdata->cReason == MOBILEPAY_REFUND_REQUEST ) {
		memset(&g_tTxnInfo, 0, sizeof(g_tTxnInfo));
		memcpy(g_tTxnInfo.Amount, ptMPdata->szAmount, sizeof(g_tTxnInfo.Amount));
		memcpy(g_tTxnInfo.OrderId, ptMPdata->szOrderID, sizeof(g_tTxnInfo.OrderId));
		memcpy(g_tTxnInfo.Action, "Start", 6);		//default
		g_tTxnInfo.CustomerTokenCalc[0] = '0';		//default
	}

	memset(&g_tPosInfo, 0, sizeof(g_tPosInfo));
	Load_OwnData_Config(&g_tPosInfo);

	memset(&tTxnTotals, 0, sizeof(tTxnTotals));
	Load_OwnData_TransTotals(&tTxnTotals);
	//generate bulkref
	OsGetTime(&tTime);
	memset(ptMPdata->szBulkRef, 0, sizeof(ptMPdata->szBulkRef));
	snprintf(ptMPdata->szBulkRef, sizeof(ptMPdata->szBulkRef), "%5s%02d%02d%02d%07d", ptMPconfig->szLocationID, tTime.Year - 2000, tTime.Month, tTime.Day, tTxnTotals.Counter);
	memcpy(g_tTxnInfo.BulkRef, ptMPdata->szBulkRef, sizeof(g_tTxnInfo.BulkRef));

	if ( strcmp(g_tPosInfo.MerchantId, ptMPconfig->szMerchantID) != 0 ||
			strcmp(g_tPosInfo.LocationId, ptMPconfig->szLocationID) != 0 ||
			strcmp(g_tPosInfo.APIkey, ptMPconfig->szAPIkey) != 0 ) {
		memset(&g_tPosInfo, 0, sizeof(g_tPosInfo));
		g_tPosInfo.AppStatus = MOBILEPAY_STATUS_NOT_CONFIGURED;
	}

	gl_ucConnDevice = ptMPconfig->ucConnDevice;
	memcpy(g_tPosInfo.IPorURL, ptMPconfig->szIPorURL, sizeof(ptMPconfig->szIPorURL));
	g_tPosInfo.usPort = ptMPconfig->usPort;

	return MP_OK;
}

static int Set_Parameters(MOBILEPAYDATACONF *ptMPconfig, MOBILEPAYDATAECR *ptMPdata)
{
	char *pszZeroAmount = "0.00";
	TXNTOTALS tTxnTotals;

	if ( (ptMPconfig == NULL) || (ptMPdata == NULL) ) {
		return MP_ERR_INVALID_PARAM;
	}

	if ( ptMPdata->cReason == MOBILEPAY_PAY_REQUEST || ptMPdata->cReason == MOBILEPAY_REFUND_REQUEST ) {
		snprintf(ptMPdata->szPaymentStatus, sizeof(ptMPdata->szPaymentStatus), "%d", g_tTxnInfo.PaymentStatus);		//TODO
		memcpy(ptMPdata->szAmount, g_tTxnInfo.Amount, sizeof(g_tTxnInfo.Amount));
		snprintf(ptMPdata->szReminder, sizeof(ptMPdata->szReminder), "%0.2lf", g_tTxnInfo.Remainder);
		memcpy(ptMPdata->szTransactionID, g_tTxnInfo.TransactionId, sizeof(g_tTxnInfo.TransactionId));
		memcpy(ptMPdata->szCustomerID, g_tTxnInfo.CustomerId, sizeof(g_tTxnInfo.CustomerId));
		memcpy(ptMPdata->szCustomerToken, g_tTxnInfo.CustomerToken, sizeof(g_tTxnInfo.CustomerToken));
		memcpy(ptMPdata->szCustomerReceiptToken, g_tTxnInfo.CustomerReceiptToken, sizeof(g_tTxnInfo.CustomerReceiptToken));
	}

	memset(&tTxnTotals, 0, sizeof(tTxnTotals));
	Load_OwnData_TransTotals(&tTxnTotals);
	//added by Kevin Liu 20160616
	if ( strlen(tTxnTotals.PaymentTotal) == 0 ) {
		memcpy(tTxnTotals.PaymentTotal, pszZeroAmount, strlen(pszZeroAmount));
	}
	if ( strlen(tTxnTotals.RefundTotal) == 0 ) {
		memcpy(tTxnTotals.RefundTotal, pszZeroAmount, strlen(pszZeroAmount));
	}
	if ( strlen(tTxnTotals.TransTotal) == 0 ) {
		memcpy(tTxnTotals.TransTotal, pszZeroAmount, strlen(pszZeroAmount));
	}
	memcpy(ptMPdata->szPaymentsTotal, tTxnTotals.PaymentTotal, sizeof(tTxnTotals.PaymentTotal));
	memcpy(ptMPdata->szRefundsTotal, tTxnTotals.RefundTotal, sizeof(tTxnTotals.RefundTotal));
	memcpy(ptMPdata->szTotals, tTxnTotals.TransTotal, sizeof(tTxnTotals.TransTotal));
	Save_OwnData_TransTotals(&tTxnTotals);

	snprintf(ptMPdata->szStatusCode, sizeof(ptMPdata->szStatusCode), "%d", g_tErrInfo.StatusCode);
	memcpy(ptMPdata->szStatusText, g_tErrInfo.StatusText, sizeof(g_tErrInfo.StatusText));
	if ( ptMPdata->cAppStatus == '\0' ) {
		Pax_Log(LOG_INFO, "%s - %d ptMPdata->cAppStatus = g_tPosInfo.AppStatus = %c", __FUNCTION__, __LINE__, g_tPosInfo.AppStatus);
		ptMPdata->cAppStatus = g_tPosInfo.AppStatus;
	}
	//debug
	Pax_Log(LOG_INFO, "%s - %d save", __FUNCTION__, __LINE__);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->cReason = (%c)", __FUNCTION__, __LINE__, ptMPdata->cReason);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szAmount = (%s)", __FUNCTION__, __LINE__, ptMPdata->szAmount);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szOrderID = (%s)", __FUNCTION__, __LINE__, ptMPdata->szOrderID);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szBulkRef = (%s)", __FUNCTION__, __LINE__, ptMPdata->szBulkRef);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->cAppStatus = (%c)", __FUNCTION__, __LINE__, ptMPdata->cAppStatus);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szCustomerID = (%s)", __FUNCTION__, __LINE__, ptMPdata->szCustomerID);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szCustomerReceiptToken = (%s)", __FUNCTION__, __LINE__, ptMPdata->szCustomerReceiptToken);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szCustomerToken = (%s)", __FUNCTION__, __LINE__, ptMPdata->szCustomerToken);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szPaymentStatus = (%s)", __FUNCTION__, __LINE__, ptMPdata->szPaymentStatus);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szReminder = (%s)", __FUNCTION__, __LINE__, ptMPdata->szReminder);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szStatusCode = (%s)", __FUNCTION__, __LINE__, ptMPdata->szStatusCode);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szStatusText = (%s)", __FUNCTION__, __LINE__, ptMPdata->szStatusText);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szTransactionID = (%s)", __FUNCTION__, __LINE__, ptMPdata->szTransactionID);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szPaymentsTotal = (%s)", __FUNCTION__, __LINE__, ptMPdata->szPaymentsTotal);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szRefundsTotal = (%s)", __FUNCTION__, __LINE__, ptMPdata->szRefundsTotal);
	Pax_Log(LOG_INFO, "%s - %d ptMPdata->szTotals = (%s)", __FUNCTION__, __LINE__, ptMPdata->szTotals);
	Pax_Log(LOG_INFO, "%s - %d tTxnTotals->szPaymentsTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.PaymentTotal);
	Pax_Log(LOG_INFO, "%s - %d tTxnTotals->szRefundsTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.RefundTotal);
	Pax_Log(LOG_INFO, "%s - %d tTxnTotals->szTotals = (%s)", __FUNCTION__, __LINE__, tTxnTotals.TransTotal);
	return MP_OK;
}

int main(void)
{
	int iRet=0;
	MOBILEPAYDATAECR tMPdata;
	MOBILEPAYDATACONF tMPconfig;
	char szTerminalType[20]={0};

	Pax_LogInit("GOAPPIFIED");

	//check device type
	OsRegGetValue("ro.fac.mach", szTerminalType);
	if(0 == strcmp(szTerminalType, "d200")) {
		Set_Prefix_Res_Path("./res/320/");
	} else if(0 == strcmp(szTerminalType, "s920")) {
		Set_Prefix_Res_Path("./res/240/");
	} else {
		return 0;
	}

	iRet = __loadMPayFiles(&tMPconfig, &tMPdata);
	if ( iRet == MPAY_SUCCESS ) {
		iRet = Load_Paramters(&tMPconfig, &tMPdata);
		if ( iRet != MP_OK ) {
			Init_Display();
			Display_Prompt("TECHNICAL FAILED", "Load Config Error.", MSGTYPE_FAILURE, 0);
			return 0;
		}
		switch ( tMPdata.cReason ) {
		case MOBILEPAY_BOOT_EVENT:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_BOOT_EVENT", __FUNCTION__, __LINE__);
			iRet = Command_Boot(&tMPdata);
			break;
		case MOBILEPAY_DLLCOMPLETE_EVENT:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_DLLCOMPLETE_EVENT", __FUNCTION__, __LINE__);
			iRet = Command_Config(&tMPconfig);
			break;
		case MOBILEPAY_PAY_REQUEST:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_PAY_REQUEST", __FUNCTION__, __LINE__);
			iRet = Command_Payment(&tMPconfig, &tMPdata);
			break;
		case MOBILEPAY_REFUND_REQUEST:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_REFUND_REQUEST", __FUNCTION__, __LINE__);
			iRet = Command_Refund(&tMPconfig);
			break;
		case MOBILEPAY_SHOWLAST_REQUEST:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_SHOWLAST_REQUEST", __FUNCTION__, __LINE__);
			iRet = Command_Show_Last_Txn();
			break;
		case MOBILEPAY_DOPENDING_REQUEST:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_DOPENDING_REQUEST", __FUNCTION__, __LINE__);
			iRet = Command_Execute_Pending_Operation(&tMPconfig, &tMPdata);
			break;
		case MOBILEPAY_UNREGISTER_REQUEST:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_UNREGISTER_REQUEST", __FUNCTION__, __LINE__);
			iRet = Command_Unregister(&tMPconfig);
			break;
			//added by Kevin Liu 20160616
		case MOBILEPAY_ENDOFDAY_REQUEST:
			Pax_Log(LOG_INFO, "%s - %d MOBILEPAY_ENDOFDAY_REQUEST", __FUNCTION__, __LINE__);
			iRet = Command_End_Of_Day();
			break;
		default:
			Init_Display();
			Display_Prompt("TECHNICAL FAILED", "Unknown Command.", MSGTYPE_FAILURE, 0);
			break;
		}
	} else {
		Init_Display();
		Display_Prompt("TECHNICAL FAILED", "File Not Exist.", MSGTYPE_FAILURE, 0);
	}

	Set_Parameters(&tMPconfig, &tMPdata);
	iRet = __storeMPayFiles(&tMPconfig, &tMPdata);
	if ( iRet != MPAY_SUCCESS ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		Display_Prompt("TECHNICAL FAILED", "Save ECR Data Failed.", MSGTYPE_FAILURE, 0);
	}

	Destroy_Display();
	return 0;
}
