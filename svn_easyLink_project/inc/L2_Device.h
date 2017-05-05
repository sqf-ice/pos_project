/*
 ============================================================================
 Name        : Device.h
 Author      : PAX
 Version     : 
 Copyright   : PAX Computer Technology(Shenzhen) CO., LTD
 Description : PAX POS Library
 ============================================================================
 */
 
 
#ifndef L2_DEVICE_H
#define L2_DEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _APDU_DEFINE_L2
#define _APDU_DEFINE_L2
typedef struct
{
    unsigned char       Command[4];
    unsigned short      Lc;
    unsigned char       DataIn[512];
    unsigned short      Le;
}APDU_SEND_L2;

typedef struct
{
    unsigned short      LenOut;
    unsigned char       DataOut[512];
    unsigned char       SWA;
    unsigned char       SWB;
}APDU_RESP_L2;
#endif

#ifndef _RSA_PINKEY_L2
#define _RSA_PINKEY_L2
typedef struct
{
	unsigned int  modlen;          // Module length of public key
	unsigned char mod[256];        // Module of public key, significant byte ahead, filling 0 prefix
	unsigned char exp[4];          // Exponent of public key, significant byte ahead, filling 0 prefix
	unsigned char iccrandomlen;    // Length of random data from IC
	unsigned char iccrandom[8];    // Random data from ICC
}RSA_PINKEY_L2;

#endif

//return code-------------------------------------------------
#define DEVICE_PROC_OK                    0
#define DEVICE_PROC_ERROR                 -1
#define DEVICE_PARAM_ERROR                -2

//return code of PCD-------------------------------------------------
#define DEVICE_PICC_OK                    0
#define DEVICE_PICC_USER_CANCEL           0x01
#define DEVICE_PICC_PROTOCOL_ERROR        0x02
#define DEVICE_PICC_TRANSMIT_ERROR        0x03
#define DEVICE_PICC_TIME_OUT_ERROR        0x04
#define DEVICE_PICC_INSERTED_ICCARD       0x05 // ICC is detected
#define DEVICE_PICC_SWIPED_MAGCARD        0x06 //Mag Card is detected
#define DEVICE_PICC_OTHER_ERR             0xFF

//Return code of PED-------------------------------------------------
#define DEVICE_PED_OK                     0
#define DEVICE_PEDERR_INPUT_CANCEL        -201
#define DEVICE_PEDERR_NO_ICC              -202
#define DEVICE_PEDERR_ICC_NO_INIT         -203
#define DEVICE_PEDERR_NO_PIN_INPUT        -204
#define DEVICE_PEDERR_WAIT_INTERVAL       -205
#define DEVICE_PEDERR_INPUT_TIMEOUT       -206
#define DEVICE_PEDERR_OTHER               -207

void DEVICE_GetTime(unsigned char *time);
void DEVICE_ReadSN(unsigned char *SerialNo);
void DEVICE_GetRand(unsigned char *buf, int len);
void DEVICE_TimerSet(unsigned char *pucTimerNo, unsigned short usTimeMS);
unsigned short DEVICE_TimerCheck(unsigned char TimerNo);
void DEVICE_DelayMs(unsigned short usTimeMS);

int  DEVICE_SetPinInputParam (unsigned char *pucExpectPinLen,unsigned long ulTimeoutMs);
int  DEVICE_PedVerifyPlainPin (unsigned char *ucIccRespOut, unsigned char ucMode);
int  DEVICE_PedVerifyCipherPin (RSA_PINKEY_L2 *tRsaPinKeyIn,unsigned char *ucIccRespOut,unsigned char ucMode);

void DEVICE_DES(unsigned char *input, unsigned char *output, unsigned char *deskey, int mode);
int DEVICE_AES(unsigned char *Input, unsigned char *Output, unsigned char *AesKey, int KeyLen, int Mode);
void DEVICE_Hash(unsigned char* DataIn, unsigned int DataInLen, unsigned char* DataOut);
int DEVICE_RSARecover(unsigned char *pbyModule, unsigned int dwModuleLen,
					  unsigned char *pbyExp, unsigned int dwExpLen,
					  unsigned char *pbyDataIn, unsigned char *pbyDataOut);
int DEVICE_SM2Verify(unsigned char *paucPubkeyIn,unsigned char *paucMsgIn,int nMsglenIn,
							   unsigned char *paucSignIn, int nSignlenIn);
int DEVICE_SM3(unsigned char *paucMsgIn, int nMsglenIn,unsigned char *paucResultOut);

unsigned char	DEVICE_SetControlParam (unsigned char *pucParam);
int  DEVICE_SetCancelKey(unsigned char ucKeyValue);

// the definition of transaction interface
#define DEVICE_CLSS_TXNIF      0xff
#define DEVICE_CONTACT_TXNIF   0x00

//set the transaction interface
int DEVICE_IccSetTxnIF(unsigned char ucTxnIF);
unsigned char DEVICE_IccGetTxnIF(void);
//set the ICC slot for both ICC Command and PICC command
void DEVICE_SetIccSlot(unsigned char ucSlot);
int DEVICE_IccReset(void);
//perform contact command communication for both contact and contactless transaction
unsigned char DEVICE_IccCommand(APDU_SEND_L2 *ptApduSend, APDU_RESP_L2 *ptApduRecv);

int DEVICE_FInitiate(void);
int DEVICE_FWriteData(int nFileIndex, unsigned char *paucDataIn, unsigned int unDataInLen);
int DEVICE_FReadData(int nFileIndex, unsigned char *paucDataOut, unsigned int unDataExceptLen);
int DEVICE_FRemove(int nFileIndex);
#ifdef __cplusplus
};
#endif

#endif /* L2_DEVICE_H */

