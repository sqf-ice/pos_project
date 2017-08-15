/*------------------------------------------------------------
* FileName: dataexchange.c
* Author: liukai
* Date: 2016-03-24
------------------------------------------------------------*/

#include "global.h"

POSINFO g_tPosInfo;
TXNINFO g_tTxnInfo;
ERRINFO g_tErrInfo;

int Load_OwnData_Config(POSINFO *ptPosInfo)
{
	int iFd=0, iLen=0;

	if ( access(FILE_SYS_PARAM, F_OK) < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_NOT_EXIST;
	}
	if ( Filesize(FILE_SYS_PARAM) != sizeof(POSINFO) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_INVLIDE_DATA;
	}
	iFd = open(FILE_SYS_PARAM, O_RDONLY);
	if ( iFd < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_OPEN_FAIL;
	}
	iLen = read(iFd, (unsigned char*)ptPosInfo, sizeof(POSINFO));
	if ( iLen != sizeof(POSINFO) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		close(iFd);
		return FILE_ERR_INVLIDE_DATA;
	}
	close(iFd);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->APIkey = %s", __FUNCTION__, __LINE__, ptPosInfo->APIkey);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->LocationId = %s", __FUNCTION__, __LINE__, ptPosInfo->LocationId);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->MerchantId = %s", __FUNCTION__, __LINE__, ptPosInfo->MerchantId);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->MerchantKey = %s", __FUNCTION__, __LINE__, ptPosInfo->MerchantKey);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->Name = %s", __FUNCTION__, __LINE__, ptPosInfo->Name);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->PoSId = %s", __FUNCTION__, __LINE__, ptPosInfo->PoSId);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->AppStatus = %c", __FUNCTION__, __LINE__, ptPosInfo->AppStatus);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->PoSUnitId = %s", __FUNCTION__, __LINE__, ptPosInfo->PoSUnitId);

	return MP_OK;
}

int Save_OwnData_Config(POSINFO *ptPosInfo)
{
	int iFd=0, iLen=0;

	remove(FILE_SYS_PARAM);
	iFd = open(FILE_SYS_PARAM, O_CREAT | O_WRONLY, S_IRWXU|S_IRWXG|S_IRWXO);
	if ( iFd < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_OPEN_FAIL;
	}
	iLen = write(iFd, (unsigned char*)ptPosInfo, sizeof(POSINFO));
	if ( iLen != sizeof(POSINFO) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		close(iFd);
		return FILE_ERR_INVLIDE_DATA;
	}
	close(iFd);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->APIkey = %s", __FUNCTION__, __LINE__, ptPosInfo->APIkey);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->LocationId = %s", __FUNCTION__, __LINE__, ptPosInfo->LocationId);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->MerchantId = %s", __FUNCTION__, __LINE__, ptPosInfo->MerchantId);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->MerchantKey = %s", __FUNCTION__, __LINE__, ptPosInfo->MerchantKey);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->Name = %s", __FUNCTION__, __LINE__, ptPosInfo->Name);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->PoSId = %s", __FUNCTION__, __LINE__, ptPosInfo->PoSId);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->AppStatus = %c", __FUNCTION__, __LINE__, ptPosInfo->AppStatus);
	Pax_Log(LOG_INFO, "%s - %d ptPosInfo->PoSUnitId = %s", __FUNCTION__, __LINE__, ptPosInfo->PoSUnitId);

	return MP_OK;
}

int Load_OwnData_TransInfo(TXNINFO *ptTxnInfo)
{
	int iFd=0, iLen=0;

	if ( ptTxnInfo == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	if ( access(FILE_TRANS_INFO, F_OK) < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_NOT_EXIST;
	}
	if ( Filesize(FILE_TRANS_INFO) != sizeof(TXNINFO) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_INVLIDE_DATA;
	}
	iFd = open(FILE_TRANS_INFO, O_RDONLY);
	if ( iFd < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_OPEN_FAIL;
	}

	iLen = read(iFd, (unsigned char*)ptTxnInfo, sizeof(TXNINFO));
	if ( iLen != sizeof(TXNINFO) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		close(iFd);
		return FILE_ERR_INVLIDE_DATA;
	}

	Pax_Log(LOG_INFO, "%s - %d ptTxnInfo->Amount=%s", __FUNCTION__, __LINE__, ptTxnInfo->Amount);
	Pax_Log(LOG_INFO, "%s - %d ptTxnInfo->OrderId=%s", __FUNCTION__, __LINE__, ptTxnInfo->OrderId);
	Pax_Log(LOG_INFO, "%s - %d ptTxnInfo->PaymentStatus=%d", __FUNCTION__, __LINE__, ptTxnInfo->PaymentStatus);
	close(iFd);

	return MP_OK;
}

int Save_OwnData_TransInfo(TXNINFO *ptTxnInfo)
{
	int iFd=0, iLen=0;

	if ( ptTxnInfo == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	remove(FILE_TRANS_INFO);
	iFd = open(FILE_TRANS_INFO, O_CREAT | O_WRONLY, S_IRWXU|S_IRWXG|S_IRWXO);
	if ( iFd < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_OPEN_FAIL;
	}

	iLen = write(iFd, (unsigned char*)ptTxnInfo, sizeof(TXNINFO));
	if ( iLen != sizeof(TXNINFO) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		close(iFd);
		return FILE_ERR_INVLIDE_DATA;
	}

	Pax_Log(LOG_INFO, "%s - %d ptTxnInfo->Amount=%s", __FUNCTION__, __LINE__, ptTxnInfo->Amount);
	Pax_Log(LOG_INFO, "%s - %d ptTxnInfo->OrderId=%s", __FUNCTION__, __LINE__, ptTxnInfo->OrderId);
	Pax_Log(LOG_INFO, "%s - %d ptTxnInfo->PaymentStatus=%d", __FUNCTION__, __LINE__, ptTxnInfo->PaymentStatus);
	close(iFd);

	return MP_OK;
}

int Load_OwnData_TransTotals(TXNTOTALS *ptTxnTotals)
{
	int iFd=0, iLen=0;

	if ( ptTxnTotals == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	if ( access(FILE_TRANS_TOTAL, F_OK) < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_NOT_EXIST;
	}
	if ( Filesize(FILE_TRANS_TOTAL) != sizeof(TXNTOTALS) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_INVLIDE_DATA;
	}
	iFd = open(FILE_TRANS_TOTAL, O_RDONLY);
	if ( iFd < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_OPEN_FAIL;
	}

	iLen = read(iFd, (unsigned char*)ptTxnTotals, sizeof(TXNTOTALS));
	if ( iLen != sizeof(TXNTOTALS) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		close(iFd);
		return FILE_ERR_INVLIDE_DATA;
	}

	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->PaymentTotal=%s", __FUNCTION__, __LINE__, ptTxnTotals->PaymentTotal);
	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->RefundTotal=%s", __FUNCTION__, __LINE__, ptTxnTotals->RefundTotal);
	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->TransTotal=%s", __FUNCTION__, __LINE__, ptTxnTotals->TransTotal);
	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->Counter=%d", __FUNCTION__, __LINE__, ptTxnTotals->Counter);
	close(iFd);

	return MP_OK;
}

int Save_OwnData_TransTotals(TXNTOTALS *ptTxnTotals)
{
	int iFd=0, iLen=0;

	if ( ptTxnTotals == NULL ) {
		return MP_ERR_INVALID_PARAM;
	}

	remove(FILE_TRANS_TOTAL);
	iFd = open(FILE_TRANS_TOTAL, O_CREAT | O_WRONLY, S_IRWXU|S_IRWXG|S_IRWXO);
	if ( iFd < 0 ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		return FILE_ERR_OPEN_FAIL;
	}

	iLen = write(iFd, (unsigned char*)ptTxnTotals, sizeof(TXNTOTALS));
	if ( iLen != sizeof(TXNTOTALS) ) {
		Pax_Log(LOG_ERROR, "%s - %d", __FUNCTION__, __LINE__);
		close(iFd);
		return FILE_ERR_INVLIDE_DATA;
	}

	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->PaymentTotal=%s", __FUNCTION__, __LINE__, ptTxnTotals->PaymentTotal);
	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->RefundTotal=%s", __FUNCTION__, __LINE__, ptTxnTotals->RefundTotal);
	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->TransTotal=%s", __FUNCTION__, __LINE__, ptTxnTotals->TransTotal);
	Pax_Log(LOG_INFO, "%s - %d ptTxnTotals->Counter=%d", __FUNCTION__, __LINE__, ptTxnTotals->Counter);
	close(iFd);

	return MP_OK;
}

