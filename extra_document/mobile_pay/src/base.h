/*------------------------------------------------------------
* FileName: base.h
* Author: liukai
* Date: 2016-03-24
------------------------------------------------------------*/
#ifndef BASE_H
#define BASE_H

//functions
int FileExist(const char *pszFilename);
long Filesize(const char *pszFilename);
char *Strrpl(char *pszStrIn, char *pszStrOut, int iOutlen, const char *pszSrc, char *pszDst);
void DeleChar(char *pszStr, char ch);
int PaxGetSN (char *pszSerialNo, int iMaxLen);
int PaxSNtoPosUnitId(char *pszSerno, char *pszPosUnitID);
int PaxGenUuid(char *pszPosUnitID, int iPosUnitIDLen, char *pszBeaconUuid);
unsigned char PaxCharToHex (char c);
int PaxCharsToHex (unsigned char *pucBhex, char *pszBasc, unsigned int nc);
void Set_Prefix_Res_Path(const char *pszStr);
const char *Add_Prefix_Res_Path(const char *pszStr);
int Pax_LogInit(const char* pszTag);
int Pax_Log(LOG_T Prio, const char *fmt, ...);

#endif
