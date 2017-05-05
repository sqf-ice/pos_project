#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "vxlibs.h"

void      SaleTransaction				(BYTE Manual);
WORD      BalanceList					(void);
//WORD      Last10Transaction			(void);
//Transactions Funct.
WORD      AddTransaction			   (PTStruct *fpPTStruct );
WORD      ReadTransaction			   (PTStruct *fpPTStruct );
WORD      NextSequenceNo			   (WORD *fpSeqNum);
//File Funct.
void      DeleteTempFile			   (void);
void      DeleteAllFile				   (void);
void      PartialFileDelete			   (void);
//Utilty
WORD      PaperControl				   (void);
//WORD      PrintSlipHeader			   (BYTE Print);
void      AmountSeparation			   (BYTE *fpAmount, BYTE *fpFJAmount);
WORD      Print_SaleDraft_Slip(void);
WORD      Prepare_Print_SaleOrPaymentSlip(BYTE PrintMerhantCopy);
WORD      Print_Balance_Slip(char *pszBalance);
WORD      PinEntryForMenu			   (BYTE *Pin);
WORD      DebugMenu                    (void);
WORD	  DebugType					   (void);
WORD      ReadHostPhones		       (BYTE *fpTel1, BYTE *fpTel2, BYTE *fpTel3);
WORD      ParameterDownload            (BYTE fbMerchantPhone,BYTE fAutoParam,BYTE Connect);
WORD      SecurityFunctions			   (void); // Modified by Kim_LinHB 2013-3-11 build 0.05.0009 BYTE to WORD
//WORD      Check_BlackList			   (void);
WORD      Check_TransFileEmpty		   (WORD *fpLastRecNum,BYTE *fpFileName);
WORD      Check_ExpDate	               (BYTE *fpExpDate);
//Entry Funct.
WORD      TerminalType	               (void);
WORD      HostPhonesEntry			   (void);
WORD      HostInfoEntry				   (void);
//Comminication Funct.
WORD ConnectHost_New(void);
void Comms_Disconnect(void);
WORD      Send_ReceiveBuffer           (BYTE *, WORD *, BYTE);
WORD      Old_SendBuffer				   (BYTE *, WORD);
int  SendNChar(BYTE *fpStr, ushort fbLength);

WORD      ReceiveStr                   (BYTE *fpChar,BYTE fbTimeout);
WORD ReceivePacket(BYTE *fpChar,BYTE fbTimeout,WORD *length);

WORD      DeleteBlackList              (BYTE *fpWrkBuf,BYTE RecLength);
WORD      CheckGeneralFiles	           (void);
unsigned char FileCopy(BYTE *srcFile, BYTE *destFile);      //add by Allen 20151116
WORD      CheckTransFiles              (void);
WORD      PrintParameterSlip           (BYTE fbFailed,BYTE fbWrongTerm);

SC_STATUS PF_CardRemove                (void);
BYTE HexToAsc                          (BYTE *fpHex,BYTE *fpAsc,BYTE Length);
void AscToHex                          (char *fpStrAscii, char *fpStrBin,int fbLen);
void ConvAsciiToBcd                    (BYTE * source, BYTE * dest , BYTE len);
void ConvDate                          (BYTE *fpBuffer);
WORD L1ConvAscInt                      (BYTE *fsStr );
void L1ConvIntAsc                      (WORD fwVal, BYTE *fsDest );
void L1CadreStr                        (BYTE *fsStr, WORD fwLgRes );
void L1SupZeros                        (BYTE *fsStr );
void DisplayString                     (BYTE *Str, WORD Timeout);
void DisplayLongString                 (BYTE *Str, WORD Timeout);
void DisplayString_S80				   (BYTE *Str, WORD Timeout);
void DisplayLongString_S80			   (BYTE *Str, WORD Timeout);
int  StrCmp                            (BYTE *X, int Lx, BYTE *Y, int Ly);
void AddBcd1                           (BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R);
void SubstBcd1                         (BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R);
void MulBcd1                           (BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R);
void ConvAscBcd1                       (BYTE *fpAsc, BYTE *fpBcd1, WORD fpLength);
void ConvBcd1Asc                       (BYTE *fpBcd1, BYTE *fpAsc, WORD fpLength);
WORD AscLen                            (BYTE *fpAsc, WORD fwLength);
void AddAscii                          (BYTE *fpAsc1,WORD fwLen1,BYTE *fpAsc2,WORD fwLen2,BYTE *fpResult, WORD fwLenRet);
void SubstAscii                        (BYTE *fpAsc1,WORD fwLen1,BYTE *fpAsc2,WORD fwLen2,BYTE *fpResult, WORD fwLenRet/*, BYTE *fpNegSign*/);
int  AsciiCmp                          (BYTE *fpAsc1, WORD fwLen1,BYTE *fpAsc2, WORD fwLen2);
void ConvAscToBcd                      (BYTE *fpAsc, BYTE *fpBcd1, WORD fpLength);

void lPrtStr				           (BYTE *fpStr);
void lPrtStrCR				           (BYTE *fpStr);
void lPrtSpace				           (BYTE fbNb);
WORD lPrtFF					           (BYTE fbNb);

WORD InitStateFile                     (void);
INT ReadStateFile                     (void);
WORD ResetStateFile                    (void);
void SetTermFlags                      (void);

void PIncreaseDate                     (BYTE *fpDate, WORD fwDays);
WORD PLenStr                           (BYTE *fpStr, BYTE fbLen);

// hdd_fct.c files
WORD HDDRead                           (HDDStruct * fpHDDBuf);
WORD HDDWrite                          (HDDStruct * fpHDDBuf);
WORD HDDInit                           (void);
// hdg_fct.c files
WORD HDGReset                          (void);
WORD HDGWrite                          (HDGStruct * fpHDGBuf);
WORD HDGRead                           (HDGStruct * fpHDGBuf);
WORD HDGInit                           (void);

WORD GPRSInit(void);
WORD GPRSRead(GPRSStruct *GPRSStruct);
WORD GPRSWrite(GPRSStruct *GPRSStruct);

// hc_auth.c files
void HCSetPTBuf                        (void);

WORD FileInit                          (BYTE *fpFileName);
WORD FileReset                         (BYTE *fpFileName);
void ConvDecToHexStr                   (unsigned long DecDebit,BYTE *HexDebit);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE EMV_StartTransaction              (BYTE transactionType, WORD wTransNo);
BYTE EMV_GetAIDField                   (void/*BYTE checkVISA, BYTE isDebit*/);
BYTE EMV_GetCardInfo                   (void);
void SetHardwareType                   (void);
BYTE Card_CheckSmart                   (BYTE *resetBuffer, BYTE startUpSequence,BYTE EMVTransType);
BYTE Card_InitializeEmvTransaction     (BYTE startUpSequence,BYTE EMVTransType);
BYTE Card_DeclareAID                   (BYTE startUpSequence);

WORD ProductMenu                       (void);
WORD Language                          (void);
WORD SchemeMenu                        (void);
WORD AmountEntry					   (BYTE *pBCDAmt);
WORD ReadAvailableInstallment          (void);
WORD Select_Calculation                (void);

WORD AlBarakaMerchantConfirm(BYTE step);

WORD TransactionTypeMenu               (void);
WORD Calculate_DisplayMenu             (void);
WORD AreYouSure                        (void);
WORD ReadMerchantId                    (void);
void Utils_DebugPrint                  (BYTE *header, void *data, WORD len);
void Utils_DebugPrintMsg(char *pszData);
BYTE *Utils_ConvertBcdToBinary         (BYTE *in, WORD len, BYTE isWord);
unsigned long Utils_ConvertBcdToULong  (BYTE *BCD, BYTE len, BYTE isCompressed);
void Utils_ConvertAscToBcd             (BYTE *ASC, BYTE *BCD,  unsigned int fpLength,unsigned int lenOut, BYTE padType, BYTE padChar);
//void Utils_ConvertBcdToAsc             (BYTE *BCD, BYTE *ASCII,unsigned int fpLength);
unsigned long atoul                    (BYTE *buffer);
WORD ISO_SendReceive				   (BYTE Connect,BYTE Type);
void PrepareField55                    (BYTE Advice, BYTE Batch);
WORD GetBatchNumber                    (BYTE Ask);
WORD      StartParameterDload	       (BYTE fAutoParam);
WORD      TransMenu               (BYTE fbBalance,BYTE fbLastTrans,BYTE fbInstallment,BYTE fbLoyalty,BYTE fbPayment,BYTE fbAccountMaintenance);
void      M_ApplicationCheck(void);
void      M_MerchantMenu				   (void);
BYTE      StartBatchClose					   (void);
WORD	  Print_PAX_Slip( WORD fwRePrint, BYTE *fpFileName);
WORD   PrintTransList	            (BYTE fbListing,BYTE Message, BYTE *Filename);   // modified by Allen 20151116
WORD	  BinCheck              (void);
WORD      InitFiles             (void);
WORD      MerchantMenu                 (BYTE fbDuplicateTran,BYTE fbEndOfDay,BYTE fbParametre,BYTE fbPrintEODTicket,BYTE fbListe,BYTE fbSecurity);
WORD	  PrintTransSlip(BYTE PrintMerhantCopy);
WORD      ListsMenu				   (void);
BYTE      ParameterList	         (void);
WORD      CheckAllFiles        (void);

BYTE Disp_Long_Msg(BYTE Type);
BYTE Get_MPIN(void);
BYTE Get_CPIN(void);
BYTE AskYesNo2(MSG_Struct *stMSG,BYTE Type,BYTE offset);

void PrintBuffer_Init(char *pBuffer, unsigned int uiBuffSize);
void PrintBuffer_SelectFont(char *pBuffer, int font_size);
void PrintBuffer_Eng(char *pBuffer, char *pszStr, ...);
void PrintBuffer_Logo(char *pBuffer, BYTE *Logo);
void PrintBuffer_Arabic(char *pBuffer, char *pszAction, char *pszStr);
void PrintBuffer_Step(char *pBuffer, int iStep);
void PrintBuffer_SendToPrinter(char *pszBuffer);

WORD PrnHead(char *pszBuffer);

WORD Print_HostErr_Slip(BYTE* Errmsg);
WORD Print_TenureOption_Slip(BYTE ItemCount,const BYTE* MenuItems[]);
#endif
