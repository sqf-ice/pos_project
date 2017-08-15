/*
 * Mifare1.h
 *
 *  Created on: 2017-5-23
 *      Author: zhouhong
 */

#ifndef MIFARE1_H_
#define MIFARE1_H_

#include "global.h"

#define BLOCK_SIZE 16
#define DATA_STRUCTURE_VERSION 5

typedef enum _M1OprCode {
	M1_READ = 'R',
	M1_WRITE = 'W'
} M1_OPR_CODE;

typedef enum _M1Card_status {
	M1_STATUS_CLOSED = 0,
	M1_STATUS_ACTIVE = 1
} M1_CARD_STATUS;

typedef struct _M1CardInfo {
	char  cCardType;
	uchar sUID[4];             //¿¨ID£¬¼´¿¨µÄÐòÁÐºÅ, first 4 bytes
	uchar sATQx[16];
	uchar sRats[254];
	uchar sSAK[2];
	uchar sucBlocksInfo[48];   //sector 1, block 0, 1, 2
	char cVersion;             //data structure version
	char cCardStatus;          //0x00==closed, 0x01==Active
	uchar sPIN[4];
	uchar szSecurityCode[5+1];
	uchar szCardNumber[16+1];
	uchar szCardBalance[16+1];
	long lOldBalance;
} M1_CARD_INFO;

M1_CARD_INFO g_tM1CardInfo;

int OpenMifare1Card();
int CloseMifare1Card();

/**
 * Detect card that show on teminal and get part of card information
 * output parameters
 * @param pstM1CardInfo	            card information read from card and decryped
 *
 * @retval VC_OK                    operation completed
 * @retval VC_ERR_INVALID_PARAM     invalid input parameter
 * @retval ...                      some other errors
 */
int DetectMifare1Card(M1_CARD_INFO* pstMifare1CardInfo);

/**
 * Get card information and decrypt data
 * input parameters
 * output parameters
 * @param pstM1CardInfo	            card information read from card and decryped
 *
 * @retval VC_OK                    operation completed
 * @retval VC_ERR_INVALID_PARAM     invalid input parameter
 * @retval ...                      some other errors
 */
int GetMifareCardInfo(M1_CARD_INFO* pstM1CardInfo);


int Mifare1CardVerify(M1_CARD_INFO* pstM1CardInfo);

/**
 * Subtract amount from card balance and encrypt data and write to card
 * input parameters
 * @param pstM1CardInfo	    card information
 * @param lAmount           amount subtracted from card balance
 *
 * @retval VC_OK                    operation completed
 * @retval VC_ERR_INVALID_PARAM     invalid input parameter
 * @retval ...                      some other errors
 */
int Mifare1CardDebit(M1_CARD_INFO* pstM1CardInfo, long lAmount);

/**
 * Check whether debit card successfully
 * input parameters
 * @param pstM1CardInfo	    card information
 * @param lAmount           amount subtracted from card balance
 *
 * @retval VC_OK                    operation completed
 * @retval VC_ERR_INVALID_PARAM     invalid input parameter
 * @retval VC_ERR                   some other errors
 * @retval ERR_M1_VERIFY_READBACK   subtract amount from card balance failed
 */
int Mifare1CardVerifyReadback(M1_CARD_INFO* pstM1CardInfo, long lAmount);

#endif /* MIFARE1_H_ */
