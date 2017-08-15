/*------------------------------------------------------------
 * FileName: aes.c
 * Author: zhouhong
 * Date: 2017-06-02
------------------------------------------------------------*/
#include "global.h"

//generate 32 bytes key with  saltKey, using PKCS5_PBKDF2_HMAC_SHA1 for about 1000 round
int AES_GetKey(unsigned char *pszPassword, int iPasswordLen, unsigned char *pszSaltKey,
		unsigned char *pszKey, int iKeyLen)
{
	int iRet = 0;
	int iRounds = 1000;

	if((pszPassword == NULL) || (pszSaltKey == NULL) || (pszKey == NULL) || (iKeyLen != 32)) {
		return VC_ERR_INVALID_PARAM;
	}

	/*
	 * Rfc2898DeriveBytes in c# and PKCS5_PBKDF2_HMAC_SHA1 for about 1000 rounds is the same.
	 */
	iRet = PKCS5_PBKDF2_HMAC_SHA1(pszPassword, iPasswordLen,
			pszSaltKey, strlen(pszSaltKey), iRounds, iKeyLen, pszKey);
	if (iRet != 1) {
		Pax_Log(LOG_INFO, "%s - %d, iRet:%d, PKCS5_PBKDF2_HMAC_SHA1 failed", __FUNCTION__, __LINE__, iRet);
		return VC_ERR;
	}

	Pax_Log(LOG_INFO, "%s - %d, Key: ", __FUNCTION__, __LINE__);
	DumpBuffer(LOG_DEBUG, pszKey, 32);

	Pax_Log(LOG_INFO, "AES_GetKey successfully!");

	return VC_OK;
}

//encryption : use openssl to to encryption
int AES_Encrypt(unsigned char *pszKey, int iKeyLen, unsigned char *pszIVKey,
		const unsigned char *pucDataIn, int iDataInLen,unsigned char *pucDataOut)
{
	Pax_Log(LOG_INFO, "%s - %d, pucPlaintext: ", __FUNCTION__, __LINE__);
	DumpBuffer(LOG_DEBUG, pucDataIn, iDataInLen);

	EVP_CIPHER_CTX stEncryptCtx;
	int iOutLen = 0;
	int iFinalLen = 0;

	if((pucDataIn == NULL) || (iDataInLen%16 != 0) || (pucDataOut==NULL)) {
		return VC_ERR_INVALID_PARAM;
	}

	//init EncryptCtx
	EVP_CIPHER_CTX_init(&stEncryptCtx);
	EVP_EncryptInit_ex(&stEncryptCtx, EVP_aes_256_cbc(), NULL, pszKey, pszIVKey);

	/* update ciphertext, iCipherLen is filled with the length of ciphertext generated,
	 *iOutLen is the size of plaintext in bytes */
	EVP_EncryptUpdate(&stEncryptCtx, pucDataOut, &iOutLen, pucDataIn, iDataInLen);
	Pax_Log(LOG_INFO, "%s - %d, 1.iOutLen: %d", __FUNCTION__, __LINE__, iOutLen);
	/* update ciphertext with the final remaining bytes */
	if(iOutLen < iDataInLen) {
		EVP_EncryptFinal_ex(&stEncryptCtx, pucDataOut+iOutLen, &iFinalLen);
		iOutLen += iFinalLen;
	}

	Pax_Log(LOG_INFO, "%s - %d, 2.iOutLen: %d", __FUNCTION__, __LINE__, iOutLen);
	Pax_Log(LOG_INFO, "%s - %d, pucCiphertext: ", __FUNCTION__, __LINE__);
	DumpBuffer(LOG_DEBUG, pucDataOut, iOutLen);

	EVP_CIPHER_CTX_cleanup(&stEncryptCtx);

	return VC_OK;
}

//decryption : use openssl to to decryption
int AES_Decrypt(unsigned char *pszKey, int iKeyLen, unsigned char *pszIVKey,
		const unsigned char *pucDataIn, int iDataInLen,unsigned char *pucDataOut)
{
	Pax_Log(LOG_INFO, "%s - %d, pucCiphertext: ", __FUNCTION__, __LINE__);
	DumpBuffer(LOG_DEBUG, pucDataIn, iDataInLen);

	EVP_CIPHER_CTX stDecryptCtx;
	int iOutLen = 0;
	int iFinalLen = 0;

	if((pucDataIn == NULL) || (iDataInLen%16 != 0) || (pucDataOut==NULL)) {
		return VC_ERR_INVALID_PARAM;
	}

	EVP_CIPHER_CTX_init(&stDecryptCtx);
	EVP_DecryptInit_ex(&stDecryptCtx, EVP_aes_256_cbc(), NULL, pszKey, pszIVKey);
	EVP_DecryptUpdate(&stDecryptCtx, pucDataOut, &iOutLen, pucDataIn, iDataInLen);
	Pax_Log(LOG_INFO, "%s - %d, 1.iOutLen: %d", __FUNCTION__, __LINE__, iOutLen);
	if(iOutLen < iDataInLen) {
		EVP_DecryptFinal_ex(&stDecryptCtx, pucDataOut+iOutLen, &iFinalLen);
		iOutLen += iFinalLen;
	}
	Pax_Log(LOG_INFO, "%s - %d, 2.iOutLen: %d", __FUNCTION__, __LINE__, iOutLen);
	Pax_Log(LOG_INFO, "%s - %d, pucPlaintext: ", __FUNCTION__, __LINE__);
	DumpBuffer(LOG_DEBUG, pucDataOut, iOutLen);

	EVP_CIPHER_CTX_cleanup(&stDecryptCtx);

	return VC_OK;
}

//use PED to do encryption/decryption
//iMode : 0x02:CBC decrypt, 0x03:CBC encrypt
int AES_En_Decrypt(unsigned char *pszKey, int iKeyLen, unsigned char *pszIVKey,
		const unsigned char *pucDataIn, int iDataInLen,unsigned char *pucDataOut, int iMode) {

	Pax_Log(LOG_INFO, "%s - %d, pucDataIn: ", __FUNCTION__, __LINE__);
	DumpBuffer(LOG_DEBUG, pszKey, iKeyLen);
	DumpBuffer(LOG_DEBUG, pucDataIn, iDataInLen);

	int iRet = 0;
	unsigned char sKeyBlock[184] = {0};
	int iKeyIdx = 1;

	if((pszKey == NULL) || (pszIVKey == NULL) || (pucDataIn == NULL) || (pucDataOut == NULL)) {
		return VC_ERR_INVALID_PARAM;
	}

	if((iRet = OsPedOpen()) != RET_OK) {
		Pax_Log(LOG_INFO, "%s - %d, OsPedOpen failed!", __FUNCTION__, __LINE__);
		return iRet;
	}

	sKeyBlock[0] = 0x03;
	sKeyBlock[2] = 0;           //SrcKeyIdx
	sKeyBlock[3] = iKeyIdx;     //DstKeyIdx
	sKeyBlock[11]  = PED_TAESK; //DstKeyType
	sKeyBlock[12] = iKeyLen;    //DstKeyLen
	memcpy(sKeyBlock+13, pszKey, iKeyLen); //DstKeyValue

	if((iRet = OsPedWriteAesKey(sKeyBlock)) != RET_OK) {
		Pax_Log(LOG_INFO, "%s - %d, iRet: %d, OsPedWriteAesKey failed!", __FUNCTION__, __LINE__, iRet);
		return iRet;
	}

	iRet = OsPedAes(iKeyIdx, pszIVKey, pucDataIn, iDataInLen, pucDataOut, iMode);
	if(iRet != RET_OK) {
		Pax_Log(LOG_INFO, "%s - %d, OsPedAes failed!", __FUNCTION__, __LINE__);
		return iRet;
	}
	OsPedClose();

	Pax_Log(LOG_INFO, "%s - %d, pucDataOut: ", __FUNCTION__, __LINE__);
	DumpBuffer(LOG_DEBUG, pucDataOut, iDataInLen);

	Pax_Log(LOG_INFO, "Mode: %d successfully!", iMode);

	return VC_OK;
}





