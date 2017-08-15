/*
 ============================================================================
 Name        : LOG.h
 Author      : PAX
 Version     : 1.0.0
 Copyright   : PAX Computer Technology(Shenzhen) CO., LTD
 Description : PAX POS Shared Library
 ============================================================================
 */
 
 
#ifndef SHARED_LIBRARY_H_LOG
#define SHARED_LIBRARY_H_LOG

#define T_LOG	3
#define W_LOG	2
#define E_LOG	1

#define LOG_OK					0
#define LOG_ERR_BASE 			10900
enum{
	LOG_ERR_INVALID_PARA	= LOG_ERR_BASE,
	LOG_ERR_OPEN_FILE,
	LOG_ERR_READ_FILE,
	LOG_ERR_WRITE_FILE,
	LOG_ERR_LEVEL_LOW,
	LOG_ERR_FILE_NOEXIST,
	LOG_ERR_TOO_LONG,
	LOG_ERR_NO_LOGFILE,
	LOG_ERR_REMOVE_FILE,
	LOG_ERR_MAKE_DIR,
	LOG_ERR_SYSTEM,
};

#define LOG_FILE_PATH "/data/public/loginfo.dat"
#define LOG_FILE_FOLDER "/data/public/log"

#ifdef __cplusplus
extern "C"
{
#endif


int LogInit(char cLogLevel, char cLogSaveDate, int iLogMaxLen);

int LogWrite(char cLogLevel, const char *pszParaInfo);
int LogWriteHex(char cLogLevel, const char *pszFileName, int iLineNo, const char *psParaInfo, int iParaLen);
int LogQueAll(char * pszLogFileOut);
int LogDelOne(const char *pszDate);
int LogDelAll(void);
const char* LogGetLastErrMsg(void);
const char* LogGetVer(void);

#define _LOG(cLogLevel, pszParaInfo) LogWrite((char)cLogLevel, (const char *)pszParaInfo)

#ifdef __cplusplus
};
#endif

#endif /* SHARED_LIBRARY_H_LOG */

