/*
 * Mifare.c
 *
 *  Created on: 2017-5-23
 *      Author: zhouhong
 */

#include "global.h"

unsigned char *g_pszIvKey = "4f$g-C<rBy$A+n@G";
unsigned char *g_pszSaltKey = "2t_j=VZk";

int OpenMifare1Card() {
	return OsPiccOpen();
}

int CloseMifare1Card() {
	return OsPiccClose();
}

int DetectMifare1Card(M1_CARD_INFO* pstM1CardInfo) {
	int iRet = 0;

	if (pstM1CardInfo == NULL) {
		return VC_ERR_INVALID_PARAM;
	}

	iRet = OsPiccPoll(&(pstM1CardInfo->cCardType), pstM1CardInfo->sATQx);//¼ì²é¿¨Æ¬£¬·µ»Ø¿¨Æ¬ÀàÐÍcMyCardTypeºÍ  ATQx
	if(iRet == PCD_ERR_WTO_FLAG || iRet == PCD_ERR_COLL_FLAG || iRet) {
//		Pax_Log(LOG_ERROR, "%s - %d, OsPiccPoll iRet :%d", __FUNCTION__, __LINE__, iRet);
		return iRet;
	}
	else {
		Pax_Log(LOG_DEBUG, "OsPiccPoll iRet :%d, CardType:%c, ATQx: %02x, %02x", iRet, pstM1CardInfo->cCardType, pstM1CardInfo->sATQx[0], pstM1CardInfo->sATQx[1]);
	}

	//·À³å×²£¬·µ»ØUID SerialNoºÍÆäËûOther	nRet = OsPiccAntiSel(cMyCardType, SerialNo, atqx[0], Other);
	iRet = OsPiccAntiSel(pstM1CardInfo->cCardType, pstM1CardInfo->sUID, pstM1CardInfo->sATQx[0], pstM1CardInfo->sSAK);
	if(!iRet) {
		Pax_Log(LOG_DEBUG, "OsPiccAntiSel nRet :%d, CardType: %c, pucSAK: %02x%02x", iRet, pstM1CardInfo->cCardType, pstM1CardInfo->sSAK[0], pstM1CardInfo->sSAK[1]);
		Pax_Log(LOG_DEBUG,"UID : ");
		DumpBuffer(LOG_DEBUG, pstM1CardInfo->sUID, sizeof(pstM1CardInfo->sUID));
	}
	else {
		Pax_Log(LOG_ERROR, "%s - %d, OsPiccAntiSel nRet :%d",  __FUNCTION__, __LINE__, iRet);
		return iRet;
	}

	if(pstM1CardInfo->sSAK[0] == 0x20 ) {
		iRet = OsPiccActive(pstM1CardInfo->cCardType, pstM1CardInfo->sRats);//¼¤»î¿¨Æ¬£¬·µ»ØRATSÊý¾ÝaucRats
		OsLog(LOG_DEBUG, "OsPiccActive iRet :%d", iRet);
		if(iRet) {
			Pax_Log(LOG_ERROR, "OsPiccActive iRet :%d", iRet);
			return iRet;
		}
	}

	Pax_Log(LOG_DEBUG, "Detect Success!");

	return VC_OK;
}

static int ProOprBlock(uchar* pucUID, M1_OPR_CODE eOprCode, uchar ucBlockNo, uchar *pucBlockInfo, uchar ucPasswdType, uchar ucDesBlkNo)
{
	int iRet = 0;
	uchar sPwdMifare[6];//The authentication password:6byte

	if ( (pucUID == NULL) || (pucBlockInfo == NULL) ) {
		return VC_ERR_INVALID_PARAM;
	}

	if (ucPasswdType == 0){
		memcpy(sPwdMifare,"\x00\x00\x00\x00\x00\x00", sizeof(sPwdMifare));
	}
	else{
		memcpy(sPwdMifare,"\xff\xff\xff\xff\xff\xff", sizeof(sPwdMifare));
	}

	iRet = OsMifareAuthority(pucUID, ucBlockNo, 'A', sPwdMifare);//¿¨ºÅ£¬¿éºÅ£¬ÃÜÂëÀàÐÍ£¬ÃÜÂë
	if (iRet){
		Pax_Log(LOG_DEBUG, "OsMifareAuthority ucDesBlkNo %d, iRet :%d", ucDesBlkNo, iRet);
		return iRet;
	}

	iRet = OsMifareOperate(eOprCode, ucBlockNo, pucBlockInfo, ucDesBlkNo);//²Ù×÷Âë£¬¿éºÅ£¬¶ÁÐ´¿éÖÐÖµ
	if (iRet){
		Pax_Log(LOG_DEBUG, "OsMifareOperate ucDesBlkNo %d, iRet :%d", ucBlockNo, iRet);
		return iRet;
	}

	return VC_OK;
}

static int MifareCardBlockProc(uchar* pucUID, M1_OPR_CODE eOprCode,  uchar *pucBlockInfo, uchar ucBlockNo)
{
	int iRet = 0;

	if ((pucUID == NULL) || (pucBlockInfo == NULL)) {
		return VC_ERR_INVALID_PARAM;
	}

	iRet = ProOprBlock(pucUID, eOprCode, ucBlockNo, pucBlockInfo, 1, NULL);
	Pax_Log(LOG_ERROR, "ProOprBlock iRet :%d",iRet);
	if (iRet) {
		return iRet;
	}

	Pax_Log(LOG_DEBUG,"BlockNo : %d, Block Info:", ucBlockNo);
	DumpBuffer(LOG_DEBUG, pucBlockInfo, BLOCK_SIZE);

	return VC_OK;
}

int GetMifareCardInfo(M1_CARD_INFO* pstM1CardInfo)
{
	Pax_Log(LOG_ERROR, "\n---- GetMifareCardInfo ---- \n");

	int iRet = 0;
    unsigned int uiPasswordLen = 0;
	uchar szPassword[8+1] = {0};
    uchar sKey[32] = {0};
    uchar sDataOut[48] = {0};

	if (pstM1CardInfo == NULL) {
		return VC_ERR_INVALID_PARAM;
	}

	iRet = MifareCardBlockProc(pstM1CardInfo->sUID, M1_READ, pstM1CardInfo->sucBlocksInfo, 4);
	if (iRet != VC_OK) {
		return iRet;
	}

	iRet = MifareCardBlockProc(pstM1CardInfo->sUID, M1_READ, pstM1CardInfo->sucBlocksInfo+16, 5);
	if (iRet != VC_OK) {
		return iRet;
	}

	iRet = MifareCardBlockProc(pstM1CardInfo->sUID, M1_READ, pstM1CardInfo->sucBlocksInfo+ 16*2, 6);
	if (iRet != VC_OK) {
		return iRet;
	}

	uiPasswordLen = PaxHexToChars(szPassword, pstM1CardInfo->sUID, 4);
	iRet = AES_GetKey(szPassword, uiPasswordLen, g_pszSaltKey, sKey, 32);
	if(iRet != VC_OK) {
		return iRet;
	}
	iRet =  AES_Decrypt(sKey, 32, g_pszIvKey,(const unsigned char *)pstM1CardInfo->sucBlocksInfo, 48, sDataOut);
	if(iRet != VC_OK) {
		Pax_Log(LOG_INFO, "%s - %d, AES_Decrypt failed!", __FUNCTION__, __LINE__);
		return iRet;
	}
	memcpy(pstM1CardInfo->sucBlocksInfo, sDataOut, 48);

	pstM1CardInfo->cVersion = pstM1CardInfo->sucBlocksInfo[0];
	pstM1CardInfo->cCardStatus = pstM1CardInfo->sucBlocksInfo[1];

	//get card number
	uchar *pTmp = &(pstM1CardInfo->sucBlocksInfo[16]);
	while(*pTmp == 0) {
		pTmp++;
	}
	memcpy(pstM1CardInfo->szCardNumber, pTmp, 16 - (pTmp-&(pstM1CardInfo->sucBlocksInfo[16])));
	memcpy(pstM1CardInfo->szCardBalance, &(pstM1CardInfo->sucBlocksInfo[16*2]), 16);

	Pax_Log(LOG_ERROR, "%s - %d, cVersion: %d", __FUNCTION__, __LINE__, pstM1CardInfo->cVersion);
	Pax_Log(LOG_ERROR, "%s - %d, cCardStatus: %d", __FUNCTION__, __LINE__, pstM1CardInfo->cCardStatus);
	Pax_Log(LOG_ERROR, "%s - %d, szCardNumber: %s", __FUNCTION__, __LINE__, pstM1CardInfo->szCardNumber);
	Pax_Log(LOG_ERROR, "%s - %d, szCardBalance: %ld", __FUNCTION__, __LINE__, atol(pstM1CardInfo->szCardBalance));

	return VC_OK;
}

int Mifare1CardVerify(M1_CARD_INFO* pstM1CardInfo)
{
	Pax_Log(LOG_ERROR, "\n---- Mifare1CardVerify ---- \n");

	int iRet = 0;
	if (pstM1CardInfo == NULL) {
		return VC_ERR_INVALID_PARAM;
	}

	if(pstM1CardInfo->cCardStatus == 0x00) {
		return ERR_M1_NOT_ACTIVE;
	}

	if (pstM1CardInfo->cCardStatus != 0x01) {
		return ERR_M1_INVALID;
	}

	return VC_OK;
}

int Mifare1CardDebit(M1_CARD_INFO* pstM1CardInfo, long lAmount)
{
	Pax_Log(LOG_ERROR, "\n---- Mifare1CardDebit ---- \n");

	int iRet = 0;
    unsigned int uiPasswordLen = 0;
	uchar szPassword[8+1] = {0};
    uchar sKey[32] = {0};
    uchar sDataOut[48] = {0};
    long iNewBalance = 0;
    int iNewBalLen = 0;

	if (pstM1CardInfo == NULL) {
		return VC_ERR_INVALID_PARAM;
	}

	pstM1CardInfo->lOldBalance = atol(pstM1CardInfo->szCardBalance);
	iNewBalance = atol(pstM1CardInfo->szCardBalance) - lAmount;
	Pax_Log(LOG_ERROR, "%s - %d, old szCardBalance: %s", __FUNCTION__, __LINE__, pstM1CardInfo->szCardBalance);
	snprintf(pstM1CardInfo->szCardBalance, sizeof(pstM1CardInfo->szCardBalance), "%016ld", iNewBalance);
	Pax_Log(LOG_ERROR, "%s - %d, new szCardBalance: %s",__FUNCTION__, __LINE__, pstM1CardInfo->szCardBalance);
	memcpy(&(pstM1CardInfo->sucBlocksInfo[16*2]), pstM1CardInfo->szCardBalance, 16);

	uiPasswordLen = PaxHexToChars(szPassword, pstM1CardInfo->sUID, 4);
	iRet = AES_GetKey(szPassword, uiPasswordLen, g_pszSaltKey, sKey, 32);
	if(iRet != VC_OK) {
		return iRet;
	}
	iRet =  AES_Encrypt(sKey, 32, g_pszIvKey,(const unsigned char *)pstM1CardInfo->sucBlocksInfo, 48, sDataOut);
	if(iRet != VC_OK) {
		Pax_Log(LOG_INFO, "%s - %d, AES_Encrypt failed!", __FUNCTION__, __LINE__);
		return iRet;
	}
	memcpy(pstM1CardInfo->sucBlocksInfo, sDataOut, 48);

	//write encrypted data to mifare card
	iRet = MifareCardBlockProc(pstM1CardInfo->sUID, M1_WRITE, pstM1CardInfo->sucBlocksInfo, 4);
	if (iRet != VC_OK) {
		return iRet;
	}

	iRet = MifareCardBlockProc(pstM1CardInfo->sUID, M1_WRITE, pstM1CardInfo->sucBlocksInfo+16, 5);
	if (iRet != VC_OK) {
		return iRet;
	}

	iRet = MifareCardBlockProc(pstM1CardInfo->sUID, M1_WRITE, pstM1CardInfo->sucBlocksInfo+ 16*2, 6);
	if (iRet != VC_OK) {
		return iRet;
	}

	return VC_OK;
}

int Mifare1CardVerifyReadback(M1_CARD_INFO* pstM1CardInfo,  long lAmount)
{
	Pax_Log(LOG_ERROR, "\n---- Mifare1CardVerifyReadback ---- \n");

	int iRet = 0;

	if (pstM1CardInfo == NULL) {
		return VC_ERR_INVALID_PARAM;
	}

	memset(pstM1CardInfo->sucBlocksInfo, 0 , sizeof(pstM1CardInfo->sucBlocksInfo));
	memset(pstM1CardInfo->szCardBalance, 0 , sizeof(pstM1CardInfo->szCardBalance));
	iRet = GetMifareCardInfo(pstM1CardInfo);
	if(iRet != VC_OK) {
		return VC_ERR;
	}

	Pax_Log(LOG_ERROR, "%s - %d, lOldBalance: %ld", __FUNCTION__, __LINE__, pstM1CardInfo->lOldBalance);
	Pax_Log(LOG_ERROR, "%s - %d, lAmount: %ld", __FUNCTION__, __LINE__, lAmount);
	Pax_Log(LOG_ERROR, "%s - %d, new szCardBalance: %ld", __FUNCTION__, __LINE__, atol(pstM1CardInfo->szCardBalance));

	if (atol(pstM1CardInfo->szCardBalance) != (pstM1CardInfo->lOldBalance - lAmount)) {
		return ERR_M1_VERIFY_READBACK;
	}
	Pax_Log(LOG_ERROR, "\n---- Mifare1CardVerifyReadback ok---- \n");

	return VC_OK;
}



