/*
*%-----------------------------------------------------------------------------
*% File........... : MsgStrct.h
*% Class.......... : /
*% Description ... : This file contains the declaration of the structures of
*%                   data associated with each Message. The application will
*%                   provide a structure with each associated message
*% Project........ : 8021/001 EMVCO library
*% Created ....... : 05 March 2000
*% Author ........ : Guillaume REYDELLET
*%                 : Point of Sales Terminals
*%                 : SCHLUMBERGER Technologies
*%                 : 50, Avenue Jean Jaures, 92542 Montrouge Cedex, FRANCE
*% Version ....... : 0.01
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%-----------------------------------------------------------------------------
*%
*% $Revision ------------------------------------------------------------------
*% REV   DATE      BY    DESCRIPTION
*% ----  ----      ----  ------------------------------------------------------
*% 0.01  05.03.00  GR    Initial Version
*%
*%-----------------------------------------------------------------------------
*/

#ifndef __STRUCT_H__
   #define __STRUCT_H__                      // this avoids multiple inclusions

//------------------------------- INCLUDES ------------------------------------
#include "emvdef.h"
#include "CmdStrct.h"
#include "pub_sc.h"
//-----------------------------------------------------------------------------


//--------------------------- TYPES DEFINITIONS -------------------------------


/*-----------------------------------------------------------------------------
* Structure provided with the SELECT_PSE service of EMVCO_ProcessMessage
* This service is used during applicatiobn selection process with PSE
* It enables to send a SELECT PSE command; the returned DF Name ( which must be PSE )
* is checked in this function.
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_SELECT_STRUCT  cmdSelectData;
	unsigned char*     pbCodeTableDataObject;
	unsigned char*     pbLanguageDataObject;
	unsigned char      bDirectorySFI; // SFI of directory file
}
SELECT_PSE_DATA_STRUCT; /* data required for SELECT_AID service */


/*-----------------------------------------------------------------------------
* Structure provided with the SELECT_PSE service of EMVCO_ProcessMessage
* This service is used during applicatiobn selection process with PSE
* It enables to send a SELECT PSE command; the returned DF Name ( which must be PSE )
* is checked in this function.
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_SELECT_STRUCT  cmdSelectData;
	unsigned char      bDirectorySFI; // SFI of directory file
}
SELECT_DDF_DATA_STRUCT; /* data required for SELECT_AID service */


/*-----------------------------------------------------------------------------
* Structure provided with the SELECT_AID service of EMVCO_ProcessMessage
* This service is used during applicatiobn selection process
* It enables to send a SELECT command for an AID
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_SELECT_STRUCT cmdSelectData;
	unsigned char*    pbDfNameObject;
	unsigned char*    pbLabelObject;
	unsigned char*    pbPriority;
	unsigned char*    pbCodeTableObject;
	unsigned char*    pbPreferredNameObject;
}
SELECT_AID_DATA_STRUCT; /* data required for SELECT_AID service */


/*-----------------------------------------------------------------------------
* Structure provided with the FINAL_SELECTION service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_SELECT_STRUCT  cmdSelectData;     // data required to send the SELECT command
	EMVICC_CARD_DATA*  pEmvIccData;       // OUTPUT : contains the data from SELECT R-APDU
}
FINAL_SELECTION_DATA_STRUCT; /* data required for FINAL_SELECTION service */


/*-----------------------------------------------------------------------------
* Structure provided with the INITIATE_APPLICATION service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_GPO_STRUCT        cmdGpoData;                 // data required to send the GET PROCESSING OPTIONS COMMAND see CmdStrct.h
	EMV_APP_SPECIFICATION emvAppSpecification;        // specification to meet to perform Initiate Application
	BOOLEAN               boReturnProprietaryData;    // indicates whether unknwon tag shall be returned or ignored
	EMVICC_CARD_DATA*     pEmvIccData;                // OUTPUT : AIP and AFL
	unsigned char*        pbProprietaryData;          // OUTPUT : Proprietary data found in r-apdu
	unsigned int          iProprietaryDataLength;     // INPUT  : max size of data; 
						          // OUTPUT : length of received data 
}
INITIATE_APPLICATION_DATA_STRUCT; /* data required for INITIATE_APPLICATION service */


/*-----------------------------------------------------------------------------
* Structure provided with the READ_CARD_DATA service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_READ_RECORD_STRUCT cmdReadRecordData;          // data required to send the READ RECORD command, see CmdStrct.h
	EMV_APP_SPECIFICATION  emvAppSpecification;        // specification to meet to perform Read Application Data
	EMVICC_CARD_DATA*      pEmvIccData;                // pointer on Card Data structure
	unsigned int           iMaxStaticDataLength;       // INPUT  : size of static data buffer
							   // INPUT  : AFL
							   // OUTPUT : Card Data elements from records
	unsigned char*         pbStaticDataToAuthenticate; // OUTPUT : data participating in Data Authentication
	unsigned int*          piStaticDataLength;         // OUTPUT : length of data participating ...
							   // ACHTUNG : if *piStaticDataLength is equal to
							   // iMaxStaticDataLength + 1, it means that the static
							   // data records werre more than iMaxStaticDataLength; then,
							   // the contents of pbStaticDataToAuthenticate is not
							   // valis.
}
READ_CARD_DATA_STRUCT; /* data required for READ_CARD_DATA service */



//-*-*-* Modification TML802100100030-01 for EMV Pack 3.00 PB03
/*-----------------------------------------------------------------------------
* Structure provided with the READ_CARD_DATA_2 service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	READ_CARD_DATA_STRUCT  ReadCardDataStruct;         // I/O: "standard" Read Card Data service (without proprietary data)

	BOOLEAN                boReturnProprietaryData;    // indicates whether unknwon tag shall be returned or ignored
	unsigned int           iProprietaryDataLength;     // INPUT  : max size of data;
							   // OUTPUT : length of received proprietary data
	unsigned char*         pbProprietaryData;          // OUTPUT : Proprietary data found in r-apdu
}
READ_CARD_DATA_STRUCT_2; /* data required for READ_CARD_DATA service */
//-*-*-* End of Modification TML802100100030-01 for EMV Pack 3.00 PB03



/*-----------------------------------------------------------------------------
* Structure provided with the STATIC_DATA_AUTHENTICATION service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	unsigned char*              pbCAPK;                      // Certification Authority Public Key Modulus
	unsigned int                iCAPKLength;                 // Length of Certification Authority Public Key Modulus
	unsigned long               lCAPKExponent;               // Certification Authority Public Key Exponent in DWORD format
	EMV_REVOCATED_CERTIFICATES* pbRevocationList;            // INPUT  : chained list of serial numbers of revocated certificates
	unsigned char*              pbTodaysDate;                // Date, 3 unsigned chars YYMMDD
	unsigned char*              pbAuthInputList;             // concatenation of records + Tag list to be authenticated
	unsigned int                iAuthListLength;             // Length of static data to authentictae  
	EMVICC_CARD_DATA*           pEmvIccData;                 // pointer on Card Data structure
	unsigned char*              pbHashBuffer;                // buffer used to store the data to be hashed
	unsigned char*              pbRecoveredIssuerDataBuffer; // OUTPUT : data recovered from Issuer PK Certificate
	unsigned char*              pbRecoveredStaticDataBuffer; // OUTPUT : data recovered from Signed Static Data
	unsigned char*              pbIssuerPK;                  // OUTPUT : Issuer Public Key Modulus
	unsigned int*               piIssuerPKLength;            // OUTPUT : Issuer Public Key Modulus Length; 0 if not recovered
}                                 
STATIC_AUTH_DATA_STRUCT; /* data required for STATIC_DATA_AUTHENTICATION service */
                                  
                                  
/*-----------------------------------------------------------------------------
* Structure provided with the DYNAMIC_DATA_AUTHENTICATION service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_INT_AUTH_STRUCT         cmdIntAuthData;               // data required to send the INTERNAL AUTHENTICATE command, see CmdStrct.h
	BOOLEAN                     boReturnProprietaryData;      // indicates whether unknwon tag shall be returned or ignored
	unsigned char*              pbCAPK;                       // Certification Authority Public Key Modulus
	unsigned int                iCAPKLength;                  // Length of Certification Authority Public Key Modulus
	unsigned long               lCAPKExponent;                // Certification Authority Public Key Exponent in DWORD format
	EMV_REVOCATED_CERTIFICATES* pbRevocationList;             // INPUT  : chained list of serial numbers of revocated certificates
	unsigned char*              pbTodaysDate;                 // Date, 3 unsigned chars YYMMDD
	unsigned char*              pbAuthInputList;              // concatenation of records + Tag list to be authenticated	
	unsigned int                iAuthListLength;              // Length of static data to authentictae  
	EMVICC_CARD_DATA*           pEmvIccData;                  // pointer on Card Data structure
	unsigned char*              pbHashBuffer;                 // buffer used to store the data to be hashed
	unsigned char*              pbRecoveredIssuerDataBuffer;  // OUTPUT : data recovered from Issuer PK Certificate
	unsigned char*              pbRecoveredIcckeyDataBuffer;  // OUTPUT : data recovered from Icc PK Certificate
	unsigned char*              pbRecoveredDynamicDataBuffer; // OUTPUT : data recovered from Dynamic Signature
	unsigned char*              pbIssuerPK;                   // OUTPUT : Issuer Public Key Modulus
	unsigned int*               piIssuerPKLength;             // OUTPUT : Issuer Public Key Modulus Length
	unsigned char*              pbIccPK;                      // OUTPUT : ICC Public Key Modulus
	unsigned int*               piIccPKLength;                // OUTPUT : ICC Public Key Modulus Length 
	unsigned char*              pbProprietaryData;            // OUTPUT : Proprietary data found in r-apdu
	unsigned int                iProprietaryDataLength;       // INPUT  : max size of data; 
				              		         // OUTPUT : length of received data
}                                      
DYNAMIC_AUTH_DATA_STRUCT; /* data required for DYNAMIC_DATA_AUTHENTICATION service */
                                       
//-*-*-* Modification TML802100100028-01 for EMV Pack 2.06 PB07                                       
/*-----------------------------------------------------------------------------
* Structure provided with the COMBI_DDA_GENERATE_AC service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	unsigned char* 		pbIccPK;			// INPUT:
	unsigned int   		iIccPKLength;			// INPUT:
	EMVICC_CARD_DATA *	pEmvIccData;			// I/O: pointer on Card Data structure
	unsigned char* 		pbRecoveredData;		// INPUT: working buffer to store RSA-recovered data
	unsigned char* 		pbHashBuffer;			// INPUT: contains DOLs + GenACRAPDU for 2nd hash algo
	unsigned int   		iHashDataLength;     		// INPUT: size of pbHashBuffer
	unsigned char* 		pbUnpredictableNumber;		// INPUT: 4-byte Terminal random number
}                                      
COMBI_DDA_GENAC_DATA_STRUCT; /* data required for COMBI_DDA_GENERATE_AC service */
//-*-*-* End of Modification TML802100100028-01 for EMV Pack 2.06 PB07                                       
                                       
/*-----------------------------------------------------------------------------
* Structure provided with the CHECK_VERSION_NUMBERS service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	unsigned char*      pbMinTerminalVersionNumber; // minimum version number supported by the terminal ( 9F09 data field )
	unsigned char*      pbMaxTerminalVersionNumber; // minimum version number supported by the terminal ( 9F09 data field )
	EMVICC_CARD_DATA*   pEmvIccData;                // pointer on Card Data structure	
}
CHECK_VERSIONS_DATA_STRUCT; /* data required for CHECK_VERSION_NUMBERS service */


/*-----------------------------------------------------------------------------
* Structure provided with the CHECK_APPLICATION_DATES service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	unsigned char*      pbDate;                   // transaction datein YYMMDD format ( 3 unsigned chars )
	EMVICC_CARD_DATA*   pEmvIccData;              // pointer on Card Data structure
	BOOLEAN             boIsApplicationEffective; // OUTPUT : indicates whether application is effective
	BOOLEAN             boIsApplicationExpired;   // OUTPUT : indicates whether application is expired
}
CHECK_DATES_DATA_STRUCT; /* data required for CHECK_APPLICATION_DATES service */


/*-----------------------------------------------------------------------------
* Structure provided with the CHECK_USAGE_CONTROL service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	EMV_TRANSACTION_TYPE emvTransactionType;    // transaction type ( 1 unsigned char, data field )
	BOOLEAN              boCashBack;            // Indicates whether a cashback amount is present
	unsigned char*       pbTerminalCountryCode; // pointer on Terminal Country Code ( data field, 2 unsigned chars )
	BOOLEAN              boATM;                 // indicates whether the Terminal is an ATM or not
	EMVICC_CARD_DATA*    pEmvIccData;           // pointer on Card Data structure
}
USAGE_CONTROL_DATA_STRUCT; /* data required for CHECK_USAGE_CONTROL service */


/*-----------------------------------------------------------------------------
* Structure provided with the GET_PINPK service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_GET_CHALLENGE_STRUCT    cmdGetChallData;              // data required to send the GET CHALLENGE command, see CmdStrct.h
	unsigned char*              pbCAPK;                       // Certification Authority Public Key Modulus
	unsigned int                iCAPKLength;                  // Length of Certification Authority Public Key Modulus
	unsigned long               lCAPKExponent;                // Certification Authority Public Key Exponent in DWORD format
	EMV_REVOCATED_CERTIFICATES* pbRevocationList;             // INPUT  : chained list of serial numbers of revocated certificates
	unsigned char*              pbTodaysDate;                 // Date, 3 unsigned chars YYMMDD
	unsigned char*              pbAuthInputList;              // concatenation of records + Tag list to be authenticated	
	unsigned int                iAuthListLength;              // Length of static data to authentictae  
	EMVICC_CARD_DATA*           pEmvIccData;                  // pointer on Card Data structure
	unsigned char*              pbHashBuffer;                 // buffer used to store recovered data and hash input data   
	unsigned char*              pbRecoveredIssuerDataBuffer;  // OUTPUT : data recovered from Issuer PK Certificate
	unsigned char*              pbIssuerPK;                   // OUTPUT : Issuer Public Key Modulus
	unsigned int*               piIssuerPKLength;             // OUTPUT : Issuer Public Key Modulus Length
	unsigned char*              pbRecoveredIcckeyDataBuffer;  // OUTPUT : data recovered from Icc PK Certificate
	unsigned char*              pbIccPK;                      // OUTPUT : ICC Public Key Modulus
	unsigned int*               piIccPKLength;                // OUTPUT : ICC Public Key Modulus Length 	
	unsigned long               lIccPKExponent;               // OUTPUT : ICC Public Key Exponent in DWORD format 
	unsigned char*              pbRecoveredPinkeyDataBuffer;  // OUTPUT : data recovered from PIN PK Certificate
	unsigned char*              pbPinPK;                      // OUTPUT : PIN Public Key Modulus
	unsigned int*               piPinPKLength;                // OUTPUT : PIN Public Key Modulus Length
	unsigned long               lPinPKExponent;               // OUTPUT : PIN Public Key Exponent in DWORD format 
	unsigned char*              pbChallenge;                  // OUTPUT : card unpredictable number
}
GET_PINPK_DATA_STRUCT; /* data required for GET_PINPK service */


//-*-*-* Modification TML802100100028-01 for EMV Pack 2.06 PB07
/*-----------------------------------------------------------------------------
* Structure provided with the GET_ICCPK service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	unsigned char*              pbCAPK;                       // Certification Authority Public Key Modulus
	unsigned int                iCAPKLength;                  // Length of Certification Authority Public Key Modulus
	unsigned long               lCAPKExponent;                // Certification Authority Public Key Exponent in DWORD format
	EMV_REVOCATED_CERTIFICATES* pbRevocationList;             // INPUT  : chained list of serial numbers of revocated certificates
	unsigned char*              pbTodaysDate;                 // Date, 3 unsigned chars YYMMDD
	unsigned char*              pbAuthInputList;              // concatenation of records + Tag list to be authenticated	
	unsigned int                iAuthListLength;              // Length of static data to authentictae  
	EMVICC_CARD_DATA*           pEmvIccData;                  // pointer on Card Data structure
	unsigned char*              pbHashBuffer;                 // buffer used to store the data to be hashed
	unsigned char*              pbRecoveredIssuerDataBuffer;  // OUTPUT : data recovered from Issuer PK Certificate
	unsigned char*              pbRecoveredIcckeyDataBuffer;  // OUTPUT : data recovered from Icc PK Certificate
	unsigned char*              pbIssuerPK;                   // OUTPUT : Issuer Public Key Modulus
	unsigned int*               piIssuerPKLength;             // OUTPUT : Issuer Public Key Modulus Length
	unsigned char*              pbIccPK;                      // OUTPUT : ICC Public Key Modulus
	unsigned int*               piIccPKLength;                // OUTPUT : ICC Public Key Modulus Length 
				              		          // OUTPUT : length of received data
}                                      
GET_ICCPK_DATA_STRUCT; /* dat       a required for GET_ICCPK service */
//-*-*-* End of Modification TML802100100028-01 for EMV Pack 2.06 PB07                                       
                                       
/*-----------------------------------------------------------------------------
* Structure provided with the VERIFY_PIN service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_VERIFY_STRUCT  cmdVerifyData;     // data required to send the VERIFY command, see CmdStrct.h 
	EMVICC_CARD_DATA*  pEmvIccData;       // PIN Try Counter is updated
}
VERIFY_PIN_DATA_STRUCT; /* data required for VERIFY_PIN service */


/*-----------------------------------------------------------------------------
* Structure provided with the ENCIPHER_AND_VERIFY_PIN service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	GET_PINPK_DATA_STRUCT    getpinkeyData;         // I / O data required to revcover the PIN encipherment key
	VERIFY_PIN_DATA_STRUCT   verifypinData;         // data required to SEND and analyse the verify command
	unsigned char*           pbPlaintextPINBlock;   // Captured PIN in EMV plaintext format; size must be 8
	unsigned char*           pbEncipheredPINBuffer; // buffer to store the Encipher PIN
}
ENCIPHER_PIN_DATA_STRUCT; /* data required for ENCIPHER_AND_VERIFY_PIN service */


/*-----------------------------------------------------------------------------
* Structure provided with the RANDOM_SELECTION service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	unsigned char  bTargetPercent;    // Target percent to be used for Random selection ( 0 to 99 )
	unsigned char  bMaxTargetPercent; // Maximum Target percent to be used for Biased Random selection ( 0 to 99 )
	unsigned char* pbThresholdValue;  // pointer on Threshold value for Biased Random Selection ( binary Amount, 4 unsigned chars )
	unsigned char* pbFloorLimit;      // pointer on Terminal Floor Limit ( binary amount, 4 unsigned chars )
	unsigned char* pbAmount;          // pointer on transaction amount ( binary amount, 4 unsigned chars )  
	unsigned char  bRandomNumber;     // Random number ( 1 to 99 ) generated by the terminal
}
RANDOM_SELECTION_DATA_STRUCT; /* data required for RANDOM_SELECTION service */


/*-----------------------------------------------------------------------------
* Structure provided with the VELOCITY_CHECKING service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_GET_DATA_STRUCT getdataData;   // data required to send the GET DATA command
	EMVICC_CARD_DATA*   pEmvIccData;   // pointer on Card Data structure :
					   // INPUTS  : Upper and Lower offline limits
					   // OUTPUTS : ATC and Last ATC Register
	BOOLEAN             boNewCard;     // OUTPUT : indicates whether New Card or not                         
}
VELOCITY_CHECKING_DATA_STRUCT; /* data required for VELOCITY_CHECKING service */


/*-----------------------------------------------------------------------------
* Structure provided with the TERMINAL_ACTION_ANALYSIS service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	EFT_CRYPTOGRAM_TYPE emvDecision;      // OUTPUT : Decision after Action Codes Analysis
	unsigned char*      TVR;              // pointer on the five TVR unsigned chars
	unsigned char*      pbDenialIAC;      // pointer on 5 unsigned chars Issuer Action Code Denial
	unsigned char*      pbOnlineIAC;      // pointer on 5 unsigned chars Issuer Action Code Online
	unsigned char*      pbDefaultIAC;     // pointer on 5 unsigned chars Issuer Action Code Default
	unsigned char*      pbDenialTAC;      // pointer on 5 unsigned chars Terminal Action Codes Denial
	unsigned char*      pbOnlineTAC;      // pointer on 5 unsigned chars Terminal Action Codes Online
	unsigned char*      pbDefaultTAC;     // pointer on 5 unsigned chars Terminal Action Codes Default
	BOOLEAN             boOnlinCapable;   // Terminal has Online Capability or not
}
TAA_DATA_STRUCT; /* data required for TERMINAL_ACTION_ANALYSIS service */


/*-----------------------------------------------------------------------------
* Structure provided with the DEFAULT_TAA service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	EFT_CRYPTOGRAM_TYPE emvDecision;      // OUTPUT : Decision after Action Codes Analysis
	unsigned char*      TVR;              // pointer on the five TVR unsigned chars
	unsigned char*      pbDefaultIAC;     // pointer on 5 unsigned chars Issuer Action Code Default
	unsigned char*      pbDefaultTAC;     // pointer on 5 unsigned chars Terminal Action Codes Default
}
DEFAULTTAA_DATA_STRUCT; /* data required for DEFAULT_TAA service */


/*-----------------------------------------------------------------------------
 Structure provided with the CARD_ACTION_ANALYSIS and COMPLETION services
 of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	CMD_GENERATE_AC_STRUCT genacData;                  // data required to send the GENERATE AC command
	BOOLEAN                boFirstIssuance;            // indicates the first or second issuance of GENERATE AC
	BOOLEAN                boReturnProprietaryData;    // indicates whether unknwon tag shall be returned or ignored
	unsigned int           iProprietaryDataLength;     // INPUT  : max size of data;
							   // OUTPUT : length of received proprietary data
	CARD_CRYPTOGRAM_TYPE   CardCryptogramType;         // OUTPUT : cryptogram type returned by the card
	BOOLEAN                boAdviceRequired;           // OUTPUT : Indicates whether Advice is required by the card
	CAA_REASON             GenacReason;                // OUTPUT : indicates the Card requested cryptogram reason
	unsigned char*         pbProprietaryData;          // OUTPUT : Proprietary data found in r-apdu
	EMVICC_CARD_DATA*      pEmvIccData;                // OUTPUT : pointer on Card Data structure;
							   // -> Application Cryptogram
							   // -> Cryptogram Information
							   // -> ATC
	/* HERE BEGIN EMV 2000 SPECIFIC MEMBERS */
	//-*-*-* Modification TML802100100028-01 for EMV Pack 2.06 PB07
	unsigned char *		pbHashBuffer;		   // I/O : CombiDDAGenAC hash buffer: I: contains PDOL + CDOL(s)
							   //                O: data objects from R-APDU are concatenated to DOLs
	unsigned int *		piHashBufferSize;	   // I/O : CombiDDAGenAC hash buffer size, updated with R-APDU objts size
	//-*-*-* End of Modification TML802100100028-01 for EMV Pack 2.06 PB07
	//-*-*-* Modification TML802100100028-01 for EMV Pack 2.06 PB18
	SPECIFIC_CRYPTOGRAM_TYPE   SpecificCryptogramType; // OUTPUT : payment system specific cryptogram type returned by the card
	//-*-*-* End of Modification TML802100100028-01 for EMV Pack 2.06 PB18
}
CAA_DATA_STRUCT; /* data required for CARD_ACTION_ANALYSIS and COMPLETION services */


/*-----------------------------------------------------------------------------
* Structure provided with the SCRIPT_PROCESSING service of EMVCO_ProcessMessage
-----------------------------------------------------------------------------*/
typedef struct
{
	EMV_CARD_PROTOCOL   emvProtocol;
	SC_READER           scReader;
	unsigned char*      pbScriptIdentifier; // pointer on four bytes script identifier
	unsigned int        iNbCommands;        // number of commands to send
	unsigned char**     pCommands;          // array of commands adresses
	unsigned int*       piCommandsLengths;  // array of commands lengthes
	unsigned char*      pbScriptResults;    // OUTPUT : Script results; array of 18 array of 5 unsigned chars
}
SCRIPT_DATA_STRUCT; /* data required for SCRIPT_PROCESSING service */

//-----------------------------------------------------------------------------


#endif


/* end of file MsgStrct.h */