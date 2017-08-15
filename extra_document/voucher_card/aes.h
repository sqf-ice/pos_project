/*------------------------------------------------------------
* FileName: aes.h
* Author: zhouhong
* Date: 2017-06-02
------------------------------------------------------------*/
#ifndef AES_H
#define AES_H

#define AES_BLOCK_SIZE 16

//0x02:CBC decrypt, 0x03:CBC encrypt
typedef enum _AES_Mode {
	AES_CBC_DECRYPT = 0x02,
	AES_CBC_ENCRYTP = 0x03
} AES_MODE;

/**
 * use openssl function to generate 32 bytes key with  saltKey, use PKCS5_PBKDF2_HMAC_SHA1(...) for about 1000 rounds
 * same with below C# code
 * byte[] keyBytes = new Rfc2898DeriveBytes(key, Encoding.ASCII.GetBytes(SaltKey)).GetBytes(256 / 8);
 *
 * input parameters
 * @param pszPassword             user password
 * @param iPasswordLen            length of user password
 * @param pszSaltKey              saltKey that used to generate key
 *
 * output parameters
 * @param pszKey                  generated 32 bytes key
 * @param iKeyLen                 length of key (right one is 32 bytes)
 *
 * @retval  VC_OK                  operation completed
 * @retval  VC_ERR_INVALID_PARAM   invalid input parameter
 * @retval  ...                    some other errors
 */
int AES_GetKey(unsigned char *pszPassword, int iPasswordLen, unsigned char *pszSaltKey,
		unsigned char *pszKey, int iKeyLen);

/**
 * encryption : use openssl to to encryption, 256 bit key£¬cbc mode
 * same as the .Net 2.0 Rijndael AES algorithm,
 * new RijndaelManaged() { Mode = CipherMode.CBC, Padding = PaddingMode.Zeros };   (in c#)
 *
 * input parameters
 * @param pszKey                  32 bytes key value
 * @param iKeyLen                 lenght of key, it is 32
 * @param pszIVKey                invector key
 * @param pucDataIn               Plaintext that need to be encrypt
 * @param iDataInLen              length(byte) of Plaintext (intput data), max length is 1024 bytes, (iDataInLen % 16 == 0)
 *
 * output parameters
 * @param pucDataOut               Ciphertext after encryption
 *
 * @retval  VC_OK                  operation completed
 * @retval  VC_ERR_INVALID_PARAM   invalid input parameter
 */
int AES_Encrypt(unsigned char *pszKey, int iKeyLen, unsigned char *pszIVKey,
		const unsigned char *pucDataIn, int iDataInLen,unsigned char *pucDataOut);


/**
 * encryption : use openssl to to decryption, 256 bit key£¬cbc mode
 * same as the .Net 2.0 Rijndael AES algorithm,
 * new RijndaelManaged() { Mode = CipherMode.CBC, Padding = PaddingMode.Zeros };   (in c#)
 *
 * input parameters
 * @param pszKey                  32 bytes key value
 * @param iKeyLen                 lenght of key, it is 32
 * @param pszIVKey                invector key
 * @param pucDataIn               Ciphertext that need to be encrypt
 * @param iDataInLen              length of ciphertext (intput data)
 *
 * output parameters
 * @param pucDataOut               Plaintext after decryption
 *
 * @retval  VC_OK                  operation completed
 * @retval  VC_ERR_INVALID_PARAM   invalid input parameter
 */
int AES_Decrypt(unsigned char *pszKey, int iKeyLen, unsigned char *pszIVKey,
		const unsigned char *pucDataIn, int iDataInLen,unsigned char *pucDataOut);

/**
 * encryption : use ped aes to do encryption/decryption, 256 bit key£¬cbc mode
 * the same with the .Net 2.0 Rijndael AES algorithm,
 * new RijndaelManaged() { Mode = CipherMode.CBC, Padding = PaddingMode.Zeros };   (in c#)
 *
 * input parameters
 * @param pszKey                  32 bytes key value
 * @param iKeyLen                 lenght of key, it is 32
 * @param pszIVKey                invector key
 * @param pucDataIn               input data that need to be encrypted/decrypted
 * @param iDataInLen              length of input data (intput data)
 * @param iMode                   0x02:CBC decrypt, 0x03:CBC encrypt
 *                                AES_CBC_DECRYPT = 0x02,
 *	                              AES_CBC_ENCRYTP = 0x03
 *
 * output parameters
 * @param pucDataOut               Plaintext after decryption
 *
 * @retval  VC_OK                  operation completed
 * @retval  VC_ERR_INVALID_PARAM   invalid input parameter
 *
 */
int AES_En_Decrypt(unsigned char *pszKey, int iKeyLen, unsigned char *pszIVKey,
		const unsigned char *pucDataIn, int iDataInLen,unsigned char *pucDataOut, int iMode);

#endif
