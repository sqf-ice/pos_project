/*------------------------------------------------------------
* FileName: command.c
* Author: liukai
* Date: 2016-04-19
------------------------------------------------------------*/
#include "global.h"

static int Process_PaymentStatus(int iPaymentStatus, int *piEndFlag)
{
	if ( piEndFlag == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	switch (iPaymentStatus) {
	case PAYMENT_STATUS_IDLE:
		Display_PaymentStatus("IDLE(10)");
		*piEndFlag = 0;
		break;
	case PAYMENT_STATUS_ISSUED:
		Display_PaymentStatus("WAITING FOR ACCEPT(20)");
		*piEndFlag = 0;
		break;
	case PAYMENT_STATUS_AWAIT_CHECKIN:
		Display_PaymentStatus("WAITING FOR CHECKIN(30)");
		*piEndFlag = 0;
		break;
	case PAYMENT_STATUS_CANCEL:
		Display_Prompt("PAYMENT FAILED", "Canceled By Customer(40).", MSGTYPE_FAILURE, 0);
		*piEndFlag = 1;
		break;
	case PAYMENT_STATUS_ERROR:
		Display_Prompt("PAYMENT FAILED", "Payment Error(50).", MSGTYPE_FAILURE, 0);
		*piEndFlag = 1;
		break;
	case PAYMENT_STATUS_PAYMENT_ACCEPTED:
		Display_PaymentStatus("WAITING FOR CONFIRMATION(80)");
		*piEndFlag = 0;
		break;
	case PAYMENT_STATUS_DONE:
		Display_Prompt("PAYMENT APPROVED", "Transaction Approved(100).", MSGTYPE_SUCCESS, 0);
		*piEndFlag = 1;
		break;
	default:
		return MP_ERR;
	}

	return MP_OK;
}

static int Check_Network_Status(void)
{
	int iRet=0;
	char szAddr[16]={0};
	char *p1=NULL, *p2=NULL;
	char szUrl[64]={0};

	Display_Prompt("CHECK NETWORK", "Checking network.", MSGTYPE_LOADING, 0);

	p1 = strstr(g_tPosInfo.IPorURL, "://");
	if ( p1 == NULL ) {		//maybe it's IP address.
		return MP_ERR;
	} else {
		p2 = strchr(p1 + strlen("://"), '/');
		memcpy(szUrl, p1 + strlen("://"), p2 - (p1 + strlen("://")));
		iRet = OsNetDns(szUrl, szAddr, 5000);
		Pax_Log(LOG_INFO, "szUrl = %s, szAddr = %s iRet = %d", szUrl, szAddr, iRet);
		if ( (iRet == 0) && (strlen(szAddr) != 0) ) {
			return MP_OK;
		}
	}
	return MP_ERR;
}

static int Process_Operative_Status(void)
{
	int iRet=0, iLen=0;
	int iRetConfigFile=0;
	char szMerchantIso[12]={0};
	char szMerchantIdPart[7]={0};
	char szSerno[10+1]={0};
	char szPosUnitID[15+1]={0};
	MOBILEPAYDATAECR tMPdata;
	MOBILEPAYDATACONF tMPconfig;

	memset(&tMPconfig, 0, sizeof(tMPconfig));
	iRetConfigFile = __loadMPayFiles(&tMPconfig, &tMPdata);
	if ( iRetConfigFile != MPAY_SUCCESS ) {
		return MP_ERR;
	}

#ifndef TEST_POSUNITID
	memset(szSerno, 0, sizeof(szSerno));
	memset(szPosUnitID, 0, sizeof(szPosUnitID));
	PaxGetSN(szSerno, sizeof(szSerno));
	PaxSNtoPosUnitId(szSerno, szPosUnitID);
	if ( strcmp(szPosUnitID, g_tPosInfo.PoSUnitId) != 0 ) {
		memset(g_tPosInfo.PoSUnitId, 0, sizeof(g_tPosInfo.PoSUnitId));
		memcpy(g_tPosInfo.PoSUnitId, szPosUnitID, sizeof(szPosUnitID));
		g_tPosInfo.AppStatus = MOBILEPAY_STATUS_NOT_CONFIGURED;
	}
#else
	if ( strcmp(TEST_POSUNITID, g_tPosInfo.PoSUnitId) != 0 ) {
		memset(g_tPosInfo.PoSUnitId, 0, sizeof(g_tPosInfo.PoSUnitId));
		memcpy(g_tPosInfo.PoSUnitId, TEST_POSUNITID, sizeof(TEST_POSUNITID));
		g_tPosInfo.AppStatus = MOBILEPAY_STATUS_NOT_CONFIGURED;
	}
#endif

	//check name changes.
	if ( strcmp(g_tPosInfo.Name, tMPconfig.szPoSname) != 0 ) {
		memset(g_tPosInfo.Name, 0, sizeof(g_tPosInfo.Name));
		memcpy(g_tPosInfo.Name, tMPconfig.szPoSname, sizeof(tMPconfig.szPoSname));
		if ( g_tPosInfo.AppStatus == MOBILEPAY_STATUS_NOT_OPERATIVE || g_tPosInfo.AppStatus == MOBILEPAY_STATUS_OPERATIVE ) {
			Display_Prompt("UPDATE POS NAME", "Updating POS name.", MSGTYPE_LOADING, 0);
			iRet = Request_Process(CMD_ADMIN_UPDATE_REGISTERED_POS_NAME);
			if ( iRet != HTTP_OK ) {
				if ( iRet == HTTP_ERR ) {
					Display_Error_Code(g_tErrInfo.StatusCode);
				} else if ( iRet == MP_ERR_TIMEOUT ) {
					Display_Prompt("UPDATE NAME FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
				} else if ( iRet == MP_ERR_NETWORK ) {
					Display_Prompt("UPDATE NAME FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				} else {
					Display_Prompt("UPDATE NAME FAILED", "Update POS Name Failed.", MSGTYPE_FAILURE, 0);
				}
				Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
				return MP_ERR;
			}
		}
	}

	while(g_tPosInfo.AppStatus != MOBILEPAY_STATUS_OPERATIVE) {
		switch(g_tPosInfo.AppStatus) {
		case MOBILEPAY_STATUS_NOT_CONFIGURED:
			if ( strlen(tMPconfig.szMerchantID) != 0 && strlen(tMPconfig.szLocationID) != 0 && strlen(tMPconfig.szAPIkey) != 0 ) {
				memcpy(g_tPosInfo.MerchantId, tMPconfig.szMerchantID, sizeof(tMPconfig.szMerchantID));
				memcpy(g_tPosInfo.LocationId, tMPconfig.szLocationID, sizeof(tMPconfig.szLocationID));
				memcpy(g_tPosInfo.APIkey, tMPconfig.szAPIkey, sizeof(tMPconfig.szAPIkey));
				memcpy(g_tPosInfo.Name, tMPconfig.szPoSname, sizeof(tMPconfig.szPoSname));
			}
			//calculate MerchantKey
			memset(szMerchantIso, 0, sizeof(szMerchantIso));
			memset(szMerchantIdPart, 0, sizeof(szMerchantIdPart));
			strncpy(szMerchantIdPart, g_tPosInfo.MerchantId + 3, 6);
			iLen = OsCodeConvert("UTF-8", "ISO-8859-1", szMerchantIdPart, szMerchantIso, sizeof(szMerchantIso));
			SHA256(szMerchantIso, iLen, g_tPosInfo.MerchantKey);

			g_tPosInfo.AppStatus = MOBILEPAY_STATUS_NOT_REGISTERED;
			Save_OwnData_Config(&g_tPosInfo);
			break;

		case MOBILEPAY_STATUS_NOT_REGISTERED:
			//check whether the PosUnitId have Assigned PosId, if have, turn to MOBILEPAY_STATUS_OPERATIVE.
			Display_Prompt("READ ASSIGNED POSID", "Reading assigned POSId.", MSGTYPE_LOADING, 0);
			iRet = Request_Process(CMD_ADMIN_READ_POS_UNIT_ASSIGNED_POS_ID);
			if ( iRet != HTTP_OK ) {
				if ( iRet == HTTP_ERR ) {
					//means haven't assigned posId.
				} else if ( iRet == MP_ERR_TIMEOUT ) {
					Display_Prompt("READ POSID FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				} else if ( iRet == MP_ERR_NETWORK ) {
					Display_Prompt("READ POSID FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				} else {
//					Display_Prompt(NULL, "NO ASSIGNED POSID", MSGTYPE_LOADING, 0);
				}
			}
			else {
				if ( strlen(g_tPosInfo.PoSId) != 0 ) {
					g_tPosInfo.AppStatus = MOBILEPAY_STATUS_OPERATIVE;
					break;
				}
			}
			Display_Prompt("REGISTER POS", "Registering POS.", MSGTYPE_LOADING, 0);
			iRet = Request_Process(CMD_ADMIN_REGISTER_POS);
			if ( iRet != HTTP_OK ) {
				if ( iRet == HTTP_ERR ) {
					Display_Error_Code(g_tErrInfo.StatusCode);
				} else if ( iRet == MP_ERR_TIMEOUT ) {
					Display_Prompt("REGISTER FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
				} else if ( iRet == MP_ERR_NETWORK ) {
					Display_Prompt("REGISTER FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				} else {
					Display_Prompt("REGISTER FAILED", "Register POS Failed.", MSGTYPE_FAILURE, 0);
				}
				Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
				return MP_ERR;
			}
			g_tPosInfo.AppStatus = MOBILEPAY_STATUS_NOT_OPERATIVE;
			Save_OwnData_Config(&g_tPosInfo);
			break;

		case MOBILEPAY_STATUS_NOT_OPERATIVE:
			Display_Prompt("ASSIGN UNIT ID", "Assigning unit Id.", MSGTYPE_LOADING, 0);
			iRet = Request_Process(CMD_ADMIN_ASSIGN_POS_UNITID_TO_POS);
			if ( iRet != HTTP_OK ) {
				if ( iRet == HTTP_ERR ) {
					Display_Error_Code(g_tErrInfo.StatusCode);
				} else if ( iRet == MP_ERR_TIMEOUT ) {
					Display_Prompt("ASSIGN UNITID FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
				} else if ( iRet == MP_ERR_NETWORK ) {
					Display_Prompt("ASSIGN UNITID FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				} else {
					Display_Prompt("ASSIGN UNITID FAILED", "Assign PosunitId Failed.", MSGTYPE_FAILURE, 0);
				}
				Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
				return MP_ERR;
			}
			g_tPosInfo.AppStatus = MOBILEPAY_STATUS_OPERATIVE;
			break;

		case MOBILEPAY_STATUS_OPERATIVE:
			return MP_OK;
		default:
			return MP_ERR;
		}
	}
	Save_OwnData_Config(&g_tPosInfo);
	return MP_OK;
}

int Command_Boot(MOBILEPAYDATAECR *ptMPdata)
{
	int iRet=0;

	if ( ptMPdata == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	memset(&g_tTxnInfo, 0, sizeof(g_tTxnInfo));
	iRet = Load_OwnData_TransInfo(&g_tTxnInfo);
	if ((iRet == 0) && g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_IDLE &&
				g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_DONE &&
				g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_CANCEL &&
				g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_ERROR ) {
		ptMPdata->cAppStatus = MOBILEPAY_STATUS_OPERATION_PENDING;
	}
	return MP_OK;
}

int Command_Config(MOBILEPAYDATACONF *ptMPconfig)
{
	if ( ptMPconfig == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	Init_Display();

	//removed by Kevin Liu 20160621
//	if ( Check_Network_Status() != 0 ) {
//		Display_Prompt("NETWORK ERROR", "Network Error.", MSGTYPE_FAILURE, 0);
//		return MP_ERR_NETWORK;
//	}
	if ( Process_Operative_Status() != 0 ) {
		Display_Prompt("REGISTER ERROR", "Register Error.", MSGTYPE_FAILURE, 0);
		return MP_ERR_REGISTER;
	}
	Display_Prompt("OPERATIVE SUCCESS", "Operative Success.", MSGTYPE_SUCCESS, 0);

	return MP_OK;
}

static int Payment_Cancel(ST_TIMER *ptTimer, char *pszPosUnitIdQR, int *piBreakFlag)
{
	int iRet=0;
	int iRemainTime=0;
	char szCanelPrompt[32]={0};

	//when the status is ACCEPTED, cannot cancel.
	iRemainTime = OsTimerCheck(ptTimer) / 1000;
	if ( g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_PAYMENT_ACCEPTED || iRemainTime <= 0 ) {
		Display_Prompt("PAYMENT CANCEL", "Canceling Payment.", MSGTYPE_LOADING, 0);
		iRet = Request_Process(CMD_PAYMENT_CANCEL);
		if ( iRet != HTTP_OK ) {
			if ( iRet == HTTP_ERR ) {
				Pax_Log(LOG_INFO, "%s - %d errInfo.StatusCode = %d", __FUNCTION__, __LINE__, g_tErrInfo.StatusCode);
				if ( g_tErrInfo.StatusCode == 50 ) {
					Display_Prompt("CANCEL FAILED", "Cancel Failed. Try Later(MAX 90s).", MSGTYPE_FAILURE, 0);
					Display_QRCode_Screen(pszPosUnitIdQR);
				} else {
					Display_Error_Code(g_tErrInfo.StatusCode);
					return MP_ERR;
				}
			} else if ( iRet == MP_ERR_TIMEOUT ) {
				Display_Prompt("CANCEL FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
				return MP_ERR;
			} else if ( iRet == MP_ERR_NETWORK ) {
				Display_Prompt("CANCEL FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
				return MP_ERR;
			} else {
				Display_Prompt("CANCEL FAILED", "Payment Cancel Failed.", MSGTYPE_FAILURE, 0);
				return MP_ERR;
			}
		} else {
			Display_Prompt("CANCEL SUCCESS", "Cancel Success.", MSGTYPE_SUCCESS, 0);
//			remove(FILE_TRAN_LOG);
			g_tTxnInfo.PaymentStatus = PAYMENT_STATUS_CANCEL;
			Save_OwnData_TransInfo(&g_tTxnInfo);
			*piBreakFlag = 1;
			return MP_OK;
		}
	} else {
		Beep_Prompt(1);
		while  ( iRemainTime - OsTimerCheck(ptTimer) / 1000 <= 3 ) {
			snprintf(szCanelPrompt, sizeof(szCanelPrompt), "PAYMENT IN PROGRESS(WAIT:%ds)", (int)(OsTimerCheck(ptTimer) / 1000));
			Display_PaymentStatus(szCanelPrompt);
			if ( OsTimerCheck(ptTimer) / 1000 <= 0) {
				break;
			}
			OsSleep(1000);
		}
	}
	return MP_OK;
}

int Command_Payment(MOBILEPAYDATACONF *ptMPconfig, MOBILEPAYDATAECR *ptMPdata)
{
	int iRet=0, iEndFlag=0, prevStatus=0;
	char szPosUnitIdQR[64]={0};
	char szBeaconUuid[12+1]={0};
	ST_TIMER tTimer1 = {0,0,0};
	ST_TIMER tTimer2 = {0,0,0};
	int iBreakFlag=0;
	double dPaymentTotal=0;
	double dRefundTotal=0;
	double dTransTotal=0;
	int iGetStatusRetryCount=0;
	char szCountPrompt[3][4]={"1st", "2nd", "3rd"};
	char szRetryPrompt[36]={0};
	TXNTOTALS tTxnTotals;

	if ( (ptMPconfig == NULL) || (ptMPdata == NULL) ) {
		return MP_ERR_INVALID_PARAM;
	}

	Init_Display();

	//removed by Kevin Liu 20160621
//	if ( Check_Network_Status() != 0 ) {
//		Display_Prompt("NETWORK ERROR", "Network Error.", MSGTYPE_FAILURE, 0);
//		return MP_ERR_NETWORK;
//	}
	if ( Process_Operative_Status() != 0 ) {
		Display_Prompt("REGISTER ERROR", "Register Error.", MSGTYPE_FAILURE, 0);
		return MP_ERR_REGISTER;
	}

	//send request, show preparing
	while(1) {
		Display_Prompt("PAYMENT START", "Starting payment.", MSGTYPE_LOADING, 0);
		iRet = Request_Process(CMD_PAYMENT_START);
		if ( iRet == HTTP_OK ) {
			break;
		} else if ( iRet == HTTP_ERR ) {
			if ( g_tErrInfo.StatusCode == 50 ) {
				Display_Prompt("PAYMENT IN PROGRESS", "Canceling undone payment.", MSGTYPE_LOADING, 0);
				iRet = Request_Process(CMD_PAYMENT_CANCEL);
				if ( iRet == HTTP_OK ) {
					continue;
				} else if ( iRet == HTTP_ERR ) {
					Display_Error_Code(g_tErrInfo.StatusCode);
					return MP_ERR;
				} else if ( iRet == MP_ERR_TIMEOUT ) {
					Display_Prompt("CANCEL FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				} else if ( iRet == MP_ERR_NETWORK ) {
					Display_Prompt("CANCEL FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				} else {
					Display_Prompt("CANCEL FAILED", "Payment Cancel Failed.", MSGTYPE_FAILURE, 0);
					return MP_ERR;
				}
			} else {
				Display_Error_Code(g_tErrInfo.StatusCode);
				return MP_ERR;
			}
		} else if ( iRet == MP_ERR_TIMEOUT ) {
			Display_Prompt("PAYMENT FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
			return MP_ERR;
		} else if ( iRet == MP_ERR_NETWORK ) {
			Display_Prompt("PAYMENT FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
			return MP_ERR;
		} else {
			Display_Prompt("PAYMENT FAILED", "Payment Start Failed.", MSGTYPE_FAILURE, 0);
			return MP_ERR;
		}
	}

	//save the transaction informations first, if error in the following flow, cancel it in next boot.
	Pax_Log(LOG_INFO, "%s - %d tTxnInfo.OrderId=%s", __FUNCTION__, __LINE__, g_tTxnInfo.OrderId);
	Save_OwnData_TransInfo(&g_tTxnInfo);

	ptMPdata->cAppStatus = MOBILEPAY_STATUS_OPERATION_PENDING;
	__storeMPayFiles(ptMPconfig, ptMPdata);

	//Beacon and QRCode
	memset(szPosUnitIdQR, 0, sizeof(szPosUnitIdQR));
	memset(szBeaconUuid, 0, sizeof(szBeaconUuid));
	snprintf(szPosUnitIdQR, sizeof(szPosUnitIdQR), QRCODE_FORMAT, g_tPosInfo.PoSUnitId);
	Display_QRCode_Screen(szPosUnitIdQR);

	OsTimerSet(&tTimer1, 0);
	while(1) {
		//check payment status per second
		if ( OsTimerCheck(&tTimer1) <= 0 ) {
			iRet = Request_Process(CMD_PAYMENT_GET_STATUS);
			if ( iRet != HTTP_OK ) {
				if ( iRet == HTTP_ERR ) {
					Display_Error_Code(g_tErrInfo.StatusCode);
					return MP_ERR;
				} else if ( iRet == MP_ERR_USERCANCEL ) {
					iRet = Payment_Cancel(&tTimer2, szPosUnitIdQR, &iBreakFlag);
					if ( iRet == MP_OK ) {
						if ( iBreakFlag ) {
							break;
						}
					} else {
						return MP_ERR;
					}
				} else if ( (iRet == MP_ERR_TIMEOUT) || (iRet == MP_ERR_NETWORK) ) {
					if ( iGetStatusRetryCount < 2 ) {
						memset(szRetryPrompt, 0, sizeof(szRetryPrompt));
						snprintf(szRetryPrompt, sizeof(szRetryPrompt), "REQUEST TIMEOUT(%s RETRY)", szCountPrompt[iGetStatusRetryCount]);
						Pax_Log(LOG_INFO, "%s - %d, %s", __FUNCTION__, __LINE__, szRetryPrompt);
						iGetStatusRetryCount++;
						Display_PaymentStatus(szRetryPrompt);
					} else {
						Display_Prompt("GET STATUS FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
						return MP_ERR;
					}
				} else {
					if ( iGetStatusRetryCount < 2 ) {
						memset(szRetryPrompt, 0, sizeof(szRetryPrompt));
						snprintf(szRetryPrompt, sizeof(szRetryPrompt), "GET STATUS ERROR(%s RETRY)", szCountPrompt[iGetStatusRetryCount]);
						Pax_Log(LOG_INFO, "%s - %d, %s", __FUNCTION__, __LINE__, szRetryPrompt);
						iGetStatusRetryCount++;
						Display_PaymentStatus(szRetryPrompt);
					} else {
						Display_Prompt("GET STATUS FAILED", "Get Status Error.", MSGTYPE_FAILURE, 0);
						return MP_ERR;
					}
				}

			} else {
				iGetStatusRetryCount = 0;
				//process status code.
				iRet = Process_PaymentStatus(g_tTxnInfo.PaymentStatus, &iEndFlag);
				if ( (iRet == 0) && iEndFlag ) {
					//added by Kevin Liu 20160616
					if ( g_tTxnInfo.PaymentStatus == PAYMENT_STATUS_DONE ) {
						memset(&tTxnTotals, 0, sizeof(tTxnTotals));
						Load_OwnData_TransTotals(&tTxnTotals);
						Pax_Log(LOG_INFO, "%s - %d, PaymentTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.PaymentTotal);
						Pax_Log(LOG_INFO, "%s - %d, Amount = (%s)", __FUNCTION__, __LINE__, g_tTxnInfo.Amount);
						Pax_Log(LOG_INFO, "%s - %d, atof PaymentTotal = (%.2f)", __FUNCTION__, __LINE__, atof(tTxnTotals.PaymentTotal));
						Pax_Log(LOG_INFO, "%s - %d, atof Amount = (%.2f)", __FUNCTION__, __LINE__, atof(g_tTxnInfo.Amount));

						dPaymentTotal = atof(g_tTxnInfo.Amount) + atof(tTxnTotals.PaymentTotal);
						dRefundTotal = atof(tTxnTotals.RefundTotal);
						snprintf(tTxnTotals.PaymentTotal, sizeof(tTxnTotals.PaymentTotal), "%.2f", dPaymentTotal);

						memset(tTxnTotals.TransTotal, 0, sizeof(tTxnTotals.TransTotal));
						if ( dPaymentTotal >= dRefundTotal ) {
							dTransTotal = dPaymentTotal - dRefundTotal;
							tTxnTotals.TransTotal[0] = ' ';
							snprintf(tTxnTotals.TransTotal + 1, sizeof(tTxnTotals.TransTotal), "%.2f", dTransTotal);
						} else {
							dTransTotal = dRefundTotal - dPaymentTotal;
							tTxnTotals.TransTotal[0] = '-';
							snprintf(tTxnTotals.TransTotal + 1, sizeof(tTxnTotals.TransTotal), "%.2f", dTransTotal);
						}

						Pax_Log(LOG_INFO, "%s - %d, totalAmount = (%.2f)", __FUNCTION__, __LINE__, dPaymentTotal);
						Pax_Log(LOG_INFO, "%s - %d, PaymentTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.PaymentTotal);
						Pax_Log(LOG_INFO, "%s - %d, RefundTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.RefundTotal);

						Save_OwnData_TransTotals(&tTxnTotals);
					}
					Save_OwnData_TransInfo(&g_tTxnInfo);
//					Save_OwnData_TransTotals(&tTxnTotals);
					break;
				}

				OsTimerSet(&tTimer1, 1000);
				//Status changed
				if ( g_tTxnInfo.PaymentStatus != prevStatus ) {
					if ( g_tTxnInfo.PaymentStatus == PAYMENT_STATUS_PAYMENT_ACCEPTED ) {
						OsTimerSet(&tTimer2, 90 * 1000);		//count down time for cancel payment
					}
					prevStatus = g_tTxnInfo.PaymentStatus;
					Save_OwnData_TransInfo(&g_tTxnInfo);
				}
			}
		} else {
			if ( XuiHasKey() ) {
				if ( XuiGetKey() == KEYCANCEL ) {
					iRet = Payment_Cancel(&tTimer2, szPosUnitIdQR, &iBreakFlag);
					if ( iRet == MP_OK ) {
						if ( iBreakFlag ) {
							break;
						}
					} else {
						return MP_ERR;
					}
				}
			}
		}
	}
	ptMPdata->cAppStatus = g_tPosInfo.AppStatus;
	return MP_OK;
}

int Command_Refund(MOBILEPAYDATACONF *ptMPconfig)
{
	int iRet=0;
	double dRefundTotal=0;
	double dPaymentTotal=0;
	double dTransTotal=0;
	TXNTOTALS tTxnTotals;

	if ( ptMPconfig == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	Init_Display();

	//removed by Kevin Liu 20160621
//	if ( Check_Network_Status() != 0 ) {
//		Display_Prompt("NETWORK ERROR", "Network Error.", MSGTYPE_FAILURE, 0);
//		return MP_ERR_NETWORK;
//	}
	if ( Process_Operative_Status() != 0 ) {
		Display_Prompt("REGISTER FAILED", "Register Error.", MSGTYPE_FAILURE, 0);
		return MP_ERR_REGISTER;
	}

	Display_Prompt("PAYMENT REFUND", "Refunding Payment.", MSGTYPE_LOADING, 0);
	iRet = Request_Process(CMD_PAYMENT_REFUND);
	if ( iRet != HTTP_OK ) {
		if ( iRet == HTTP_ERR ) {
			Display_Error_Code(g_tErrInfo.StatusCode);
		} else if ( iRet == MP_ERR_TIMEOUT ) {
			Display_Prompt("REFUND FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
		} else if ( iRet == MP_ERR_NETWORK ) {
			Display_Prompt("REFUND FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
		} else {
			Display_Prompt("REFUND FAILED", "Refund Failed.", MSGTYPE_FAILURE, 0);
		}
		return MP_ERR;
	}
	Display_Prompt("REFUND SUCCESS", "Refund Success.", MSGTYPE_SUCCESS, 0);

	memset(&tTxnTotals, 0, sizeof(tTxnTotals));
	Load_OwnData_TransTotals(&tTxnTotals);
	//added by Kevin Liu 20160616
	Pax_Log(LOG_INFO, "%s - %d, RefundTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.RefundTotal);
	Pax_Log(LOG_INFO, "%s - %d, Amount = (%s)", __FUNCTION__, __LINE__, g_tTxnInfo.Amount);
	Pax_Log(LOG_INFO, "%s - %d, atof RefundTotal = (%.2f)", __FUNCTION__, __LINE__, atof(tTxnTotals.RefundTotal));
	Pax_Log(LOG_INFO, "%s - %d, atof Amount = (%.2f)", __FUNCTION__, __LINE__, atof(g_tTxnInfo.Amount));

	dRefundTotal = atof(g_tTxnInfo.Amount) + atof(tTxnTotals.RefundTotal);
	dPaymentTotal = atof(tTxnTotals.PaymentTotal);
	snprintf(tTxnTotals.RefundTotal, sizeof(tTxnTotals.RefundTotal), "%.2f", dRefundTotal);

	memset(tTxnTotals.TransTotal, 0, sizeof(tTxnTotals.TransTotal));
	if ( dPaymentTotal >= dRefundTotal ) {
		dTransTotal = dPaymentTotal - dRefundTotal;
		tTxnTotals.TransTotal[0] = ' ';
		snprintf(tTxnTotals.TransTotal + 1, sizeof(tTxnTotals.TransTotal), "%.2f", dTransTotal);
	} else {
		dTransTotal = dRefundTotal - dPaymentTotal;
		tTxnTotals.TransTotal[0] = '-';
		snprintf(tTxnTotals.TransTotal + 1, sizeof(tTxnTotals.TransTotal), "%.2f", dTransTotal);
	}

	Pax_Log(LOG_INFO, "%s - %d, dRefundTotal = (%.2f)", __FUNCTION__, __LINE__, dRefundTotal);
	Pax_Log(LOG_INFO, "%s - %d, PaymentTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.PaymentTotal);
	Pax_Log(LOG_INFO, "%s - %d, RefundTotal = (%s)", __FUNCTION__, __LINE__, tTxnTotals.RefundTotal);

	Save_OwnData_TransTotals(&tTxnTotals);
	return MP_OK;
}

int Command_Show_Last_Txn(void)
{
	int iRet=0;
	TXNINFO tTxnInfo;

	Init_Display();

	memset(&tTxnInfo, 0, sizeof(tTxnInfo));
	iRet = Load_OwnData_TransInfo(&tTxnInfo);
	if ( iRet ) {
		Pax_Log(LOG_INFO, "%s - %d, Load_OwnData_TransLog iRet = %d", __FUNCTION__, __LINE__, iRet);
		Display_Prompt("NO LOGS", "Log Not Exist.", MSGTYPE_FAILURE, 0);
	} else {
		Display_Last_Result(tTxnInfo.Amount, tTxnInfo.OrderId, tTxnInfo.PaymentStatus);
	}
	return MP_OK;
}

int Command_Execute_Pending_Operation(MOBILEPAYDATACONF *ptMPconfig, MOBILEPAYDATAECR *ptMPdata)
{
	int iRet=0, iEndFlag=0;

	if ( (ptMPconfig == NULL) || (ptMPdata == NULL) ) {
		return MP_ERR_INVALID_PARAM;
	}

	Init_Display();

	ptMPdata->cAppStatus = MOBILEPAY_STATUS_OPERATION_PENDING;
	__storeMPayFiles(ptMPconfig, ptMPdata);

	//removed by Kevin Liu 20160621
//	if ( Check_Network_Status() != 0 ) {
//		Display_Prompt("NETWORK ERROR", "Network Error.", MSGTYPE_FAILURE, 0);
//		return MP_ERR_NETWORK;
//	}
	if ( Process_Operative_Status() != 0 ) {
		Display_Prompt("REGISTER FAILED", "Register Error.", MSGTYPE_FAILURE, 0);
		return MP_ERR_REGISTER;
	}

	Pax_Log(LOG_INFO, "%s - %d TRANSTYPE_BOOT", __FUNCTION__, __LINE__);
	//check previous transaction status. TODO
	memset(&g_tTxnInfo, 0, sizeof(g_tTxnInfo));
	iRet = Load_OwnData_TransInfo(&g_tTxnInfo);
	if ((iRet == 0) && g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_IDLE &&
			g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_DONE &&
			g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_CANCEL &&
			g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_ERROR ) {
		Display_Prompt("UNDONE PAYMENT", "Getting status.", MSGTYPE_LOADING, 0);
		iRet = Request_Process(CMD_PAYMENT_GET_STATUS);
		if ( iRet != HTTP_OK ) {
			if ( iRet == HTTP_ERR ) {
				Display_Error_Code(g_tErrInfo.StatusCode);
			} else if ( iRet == MP_ERR_TIMEOUT ) {
				Display_Prompt("GET STATUS FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
			} else if ( iRet == MP_ERR_NETWORK ) {
				Display_Prompt("GET STATUS FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
			} else {
				Display_Prompt("GET STATUS FAILED", "Get Status Error.", MSGTYPE_FAILURE, 0);
			}
			return MP_ERR;
		}
		//check status after GetStatus
		if (g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_IDLE &&
				g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_DONE &&
				g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_CANCEL &&
				g_tTxnInfo.PaymentStatus != PAYMENT_STATUS_ERROR ) {

			Display_Prompt("UNPAID PAYMENT", "Canceling unpaid payment.", MSGTYPE_LOADING, 0);
			iRet = Request_Process(CMD_PAYMENT_CANCEL);
			if ( iRet != HTTP_OK ) {
				if ( iRet == HTTP_ERR ) {
					Display_Error_Code(g_tErrInfo.StatusCode);
				} else if ( iRet == MP_ERR_TIMEOUT ) {
					Display_Prompt("CANCEL FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
				} else if ( iRet == MP_ERR_NETWORK ) {
					Display_Prompt("CANCEL FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
				} else {
					Display_Prompt("CANCEL FAILED", "Payment Cancel Failed.", MSGTYPE_FAILURE, 0);
				}
				return MP_ERR;
			}
			Display_Prompt("CANCEL SUCCESS", "Cancel Success.", MSGTYPE_SUCCESS, 0);
			g_tTxnInfo.PaymentStatus = PAYMENT_STATUS_CANCEL;
			Save_OwnData_TransInfo(&g_tTxnInfo);
		}
		else if ( g_tTxnInfo.PaymentStatus == PAYMENT_STATUS_DONE ||
				g_tTxnInfo.PaymentStatus == PAYMENT_STATUS_CANCEL ||
				g_tTxnInfo.PaymentStatus == PAYMENT_STATUS_ERROR ) {
			iRet = Process_PaymentStatus(g_tTxnInfo.PaymentStatus, &iEndFlag);
			if ( iEndFlag ) {
				Save_OwnData_TransInfo(&g_tTxnInfo);
			}
		}
		else if ( g_tTxnInfo.PaymentStatus == PAYMENT_STATUS_IDLE ) {
			Display_Prompt("CANCEL SUCCESS", "Already Canceled.", MSGTYPE_SUCCESS, 0);
//			remove(FILE_TRAN_LOG);
			g_tTxnInfo.PaymentStatus = PAYMENT_STATUS_CANCEL;
			Save_OwnData_TransInfo(&g_tTxnInfo);
		}
		else {
			return MP_ERR;
		}
	}
	else {
		Display_Prompt("PAYMENT DONE", "No Undone Payment.", MSGTYPE_SUCCESS, 0);
	}

	ptMPdata->cAppStatus = g_tPosInfo.AppStatus;
	return MP_OK;
}

int Command_Unregister(MOBILEPAYDATACONF *ptMPconfig)
{
	int iRet=0;
	int iRetOwnFile=0;

	if ( ptMPconfig == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	Init_Display();

	//removed by Kevin Liu 20160621
//	if ( Check_Network_Status() != 0 ) {
//		Display_Prompt("NETWORK ERROR", "Network Error.", MSGTYPE_FAILURE, 0);
//		return MP_ERR_NETWORK;
//	}
	memset(&g_tPosInfo, 0, sizeof(g_tPosInfo));
	iRetOwnFile = Load_OwnData_Config(&g_tPosInfo);
	if ( (iRetOwnFile == 0) && ((g_tPosInfo.AppStatus == MOBILEPAY_STATUS_NOT_OPERATIVE) || (g_tPosInfo.AppStatus == MOBILEPAY_STATUS_OPERATIVE)) ) {
		Display_Prompt("UNREGISTER POS", "Unregistering POS.", MSGTYPE_LOADING, 0);
		iRet = Request_Process(CMD_ADMIN_UNREGISTER_POS);
		if ( iRet != HTTP_OK ) {
			if ( iRet == HTTP_ERR ) {
				Display_Error_Code(g_tErrInfo.StatusCode);
			} else if ( iRet == MP_ERR_TIMEOUT ) {
				Display_Prompt("UNREGISTER POS FAILED", "Request timeout.", MSGTYPE_FAILURE, 0);
			} else if ( iRet == MP_ERR_NETWORK ) {
				Display_Prompt("UNREGISTER POS FAILED", "Bad Connection.", MSGTYPE_FAILURE, 0);
			} else {
				Display_Prompt("UNREGISTER POS FAILED", "Unregister POS Failed.", MSGTYPE_FAILURE, 0);
			}
			return MP_ERR;
		}
		Display_Prompt("UNREGISTER SUCCESS", "Unregister Success.", MSGTYPE_SUCCESS, 0);
	} else {
		Display_Prompt("UNREGISTER SUCCESS", "Not Registered.", MSGTYPE_SUCCESS, 0);
	}

	g_tPosInfo.AppStatus = MOBILEPAY_STATUS_NOT_CONFIGURED;
	remove(FILE_SYS_PARAM);
	remove(FILE_TRANS_INFO);
	remove(FILE_TRANS_TOTAL);

	return MP_OK;
}

int Command_End_Of_Day(void)
{
	TXNTOTALS tTxnTotals;

	memset(&tTxnTotals, 0, sizeof(tTxnTotals));

	Load_OwnData_TransTotals(&tTxnTotals);
	memset(tTxnTotals.PaymentTotal, 0, sizeof(tTxnTotals.PaymentTotal));
	memset(tTxnTotals.RefundTotal, 0, sizeof(tTxnTotals.RefundTotal));
	memset(tTxnTotals.TransTotal, 0, sizeof(tTxnTotals.TransTotal));
	tTxnTotals.Counter++;
	Save_OwnData_TransTotals(&tTxnTotals);

	return MP_OK;
}
