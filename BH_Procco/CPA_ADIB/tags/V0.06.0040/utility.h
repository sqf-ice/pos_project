#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "common.h"
#include "vxlibs.h"

#define LG_TRAMT 6


int mCmmnUtil_AsciiToBcd(void *voFromAsc, BYTE *byToAsc, int siFromLen);

int StrCmp(BYTE *X, int Lx, BYTE *Y, int Ly);
void AddBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R);
void SubstBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R);
void MulBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R);
void ConvAscBcd1(BYTE *fpAsc, BYTE *fpBcd1, WORD fpLength);
void ConvBcd1Asc(BYTE *fpBcd1, BYTE *fpAsc, WORD fpLength);
WORD AscLen(BYTE *fpAsc, WORD fwLength);
void AddAscii(BYTE *fpAsc1,WORD fwLen1,BYTE *fpAsc2,WORD fwLen2,BYTE *fpResult, WORD fwLenRet);
void SubstAscii(BYTE *fpAsc1,WORD fwLen1,BYTE *fpAsc2,WORD fwLen2,BYTE *fpResult, WORD fwLenRet);
int AsciiCmp(BYTE *fpAsc1, WORD fwLen1,BYTE *fpAsc2, WORD fwLen2);
void ConvAscToBcd(BYTE *fpAsc, BYTE *fpBcd1, WORD fpLength);
void ConvDecToHexStr(unsigned long DecDebit,BYTE *HexDebit);
void ConvDate(BYTE *fpBuffer);
BYTE HexToAsc(BYTE *fpHex,BYTE *fpAsc,BYTE Length);
void AscToHex(char *fpStrAscii, char *fpStrBin,int fbLen);
void  ConvAsciiToBcd(BYTE * source, BYTE * dest , BYTE len);
WORD L1ConvAscInt( BYTE *fsStr );
void L1ConvIntAsc( WORD fwVal, BYTE *fsDest );
void L1CadreStr( BYTE *fsStr, WORD fwLgRes );
void L1SupZeros( BYTE *fsStr );
void DisplayString(BYTE *Str, WORD Timeout);
void lPrtStr(BYTE *fpStr);
void lPrtStrCR(BYTE *fpStr);
void lPrtSpace(BYTE fbNb);
WORD lPrtFF(BYTE fbNb);
void AmountSeparation(BYTE *fpAmount, BYTE *fpFJAmount);
WORD PLenStr(BYTE *fpStr, BYTE fbLen);
void PIncreaseDate(BYTE *fpDate, WORD fwDays);
void Utils_DebugPrint (BYTE *header, void *data, WORD len);
void Utils_DebugPrintMsg(char *pszData);
BYTE Utils_ConvertBcdToBinary(BYTE *in, WORD len, BYTE isWord, BYTE *pOut);
unsigned long Utils_ConvertBcdToULong(BYTE *BCD, BYTE len, BYTE isCompressed);
void Utils_ConvertAscToBcd(BYTE *ASC, BYTE *BCD, unsigned int fpLength,unsigned int lenOut, BYTE padType, BYTE padChar);
unsigned long atoul(BYTE *buffer);
void Utils_ConvertBcdToAsc(BYTE *pBCD, BYTE *pASCII, unsigned int fpLength);

void StrAscToBcd(BYTE *fpAsc, BYTE *fpBcd1);		//incorrect algorithm
void StrBcdToAsc(INT SourceLen, BYTE *Source, BYTE *Dest);
void StrAscToHex(BYTE *fpAsc, BYTE *fpHex1);


VOID StrLeftPad(BYTE *Dest,INT LenToPad,PBYTE Source,CHAR PaddingChar);
char *strrev(char *str);
VOID StrRightPad(PBYTE Dest,INT LenToPad,PBYTE Source,CHAR PaddingChar);

VOID AddBcdNum(PBYTE Input1,PBYTE Input2, PBYTE OutBuffer, INT Len);
void Util_AddBcd(PBYTE X, INT Lx, PBYTE Y, INT Ly, PBYTE R);
VOID DivBcdNum(PBYTE Input1,DWORD Input2, PBYTE OutBuffer, INT Len);
VOID MulBcdNum(PBYTE Input1,DWORD Input2, PBYTE OutBuffer, INT Len);

int Util_BcdAscii(char *dst,  unsigned char *src, int l);

void Utils_StringCenter(char *dest,char *source,char pCh,int Len);
INT StrLen(PBYTE Src);
void StrTrim(PBYTE Source,PBYTE Dest, CHAR ch);
void itoa(int n,char *a ,int mode);
INT  StrIsDigit(BYTE *Input);

void AscHex(uchar * Out, char *In, uint len);
BYTE IsNulls(void *Buf, ushort Len);
void ClearCustomMessages(MSG_Struct *stMSG);
ushort strnlen(char *Buf, ushort MaxLen);
void BinToHex(char *po_hex, BYTE *pi_bin, int pi_len);

void MulAsciiBCD(BYTE * pDest, BYTE * pNum1, BYTE * pNum2, BYTE Len, BYTE InputType);
void DivAsciiBCD(BYTE * pDest, BYTE * pNum1, BYTE * pNum2, BYTE Len, BYTE InputType, BYTE * pRemain);//do not support float/double
uchar bytebin2bcd(uchar x);
uchar AddAsciiBCD(uchar * pDest, uchar * pNum1, uchar * pNum2, uchar Len, uchar InputType);

//valueStr includes 2 decimal digits
// in case of mode = 0, this API is used for dividing between amount and amount
//  in case of mode = 0, this API is used for dividing between amount and common integer digit. e.g 00 00 00 12 00 00 / 12
unsigned long DivStr(unsigned char * DivStr, unsigned char * byDivStr, unsigned char *valueStr, int iPrecision);						

#endif



