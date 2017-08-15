/*
 ============================================================================
 Name        : LOG.c
 Author      : PAX
 Version     : 
 Copyright   : PAX Computer Technology(Shenzhen) CO., LTD
 Description : PAX POS Shared Library
 ============================================================================
 */

#include "global.h"

#define LOG_VER "V1.0.0"

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __TIME_T
#define __TIME_T
typedef long     time_t;
#endif

#ifndef O_RDWR
#define O_RDWR           02
#endif
#define O_CREAT        0100

#  define R_OK	4		/* Test for read permission.  */
#  define W_OK	2		/* Test for write permission.  */
#  define X_OK	1		/* Test for execute permission.  */
#  define F_OK	0		/* Test for existence.  */

extern int errno;
typedef struct
{
	char cCurLevel;
	char cCurLogNum;
	char cMaxLogNum;
	int iLogMaxLen;
	char szLastDelLogDate[8+1];
}ST_LOG_INFO;

static ST_LOG_INFO glLogInfo;
char g_szErrInfo[256];
char g_szVerInfo[50];

static int ReadLogInfo(void);
static int WriteLogInfo(void);
static void CalDate(char *pszInDate, int iDate, char *pszOutDate);
static void GetCurDate(char *pszDateTime);
static int GetFileInfo(const char *pszDate);
static int GetFirstLog(char *pszLastestDate);
static int DelOneFile(const char *pszDate);
static int ChkLogStatus(FILE *fp);


static void GetCurDate(char *pszDateTime)
{
	time_t lClock;
	struct tm *pstTm;

	lClock = time(NULL);
	pstTm = localtime(&lClock);
	sprintf(pszDateTime, "%04d%02d%02d%02d%02d%02d", pstTm->tm_year+1900, pstTm->tm_mon+1,
			pstTm->tm_mday, pstTm->tm_hour, pstTm->tm_min, pstTm->tm_sec);

	return ;
}

static void GetCurDate2(char *pszDateTime)
{
	time_t lClock;
	struct tm *pstTm;

	lClock = time(NULL);
	pstTm = localtime(&lClock);

	struct timeval tv;
	gettimeofday(&tv,NULL);

	sprintf(pszDateTime, "%04d%02d%02d%02d%02d%02d%03ld", pstTm->tm_year+1900, pstTm->tm_mon+1,
			pstTm->tm_mday, pstTm->tm_hour, pstTm->tm_min, pstTm->tm_sec, tv.tv_usec/1000);

	return ;
}


static void CalDate(char *pszInDate, int iDate, char *pszOutDate)
{
	time_t lClockIn, lClockOut;
	struct tm stTmIn, *pstTmOut;
	char szBuff[10];

	memset(&stTmIn, 0, sizeof(struct tm));
	lClockIn = time(NULL);
	pstTmOut = localtime(&lClockIn);
	memcpy(&stTmIn, pstTmOut, sizeof(struct tm));
	memset(szBuff, 0,sizeof(szBuff));
	memcpy(szBuff, pszInDate, 4);
	stTmIn.tm_year = atoi((char *)szBuff)-1900;
	memset(szBuff, 0,sizeof(szBuff));
	memcpy(szBuff, pszInDate+4, 2);
	stTmIn.tm_mon = atoi((char *)szBuff)-1;
	memset(szBuff, 0,sizeof(szBuff));
	memcpy(szBuff, pszInDate+6, 2);
	stTmIn.tm_mday = atoi((char *)szBuff);
	lClockIn = mktime(&stTmIn);
	lClockOut = lClockIn+iDate*24*3600;
	pstTmOut = localtime(&lClockOut);
	sprintf(pszOutDate, "%04d%02d%02d", pstTmOut->tm_year+1900, pstTmOut->tm_mon+1, pstTmOut->tm_mday);

	return ;
}


static int GetFileInfo(const char *pszDate)
{
	char szFilePath[256];
	int iFileLen;
	FILE *fp;

	iFileLen = 0;
//	sprintf(szFilePath, "./data/log/%8.8s.log", pszDate);
	sprintf(szFilePath, "/data/public/log/%8.8s.log", pszDate);
	fp = fopen(szFilePath,"rb");
	if (fp == NULL)
	{
		 return 0;
	}
	else
	{
		fseek(fp, 0, SEEK_END);
		iFileLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
	}

	return iFileLen;
}
static int DelOneFile(const char *pszDate)
{
	char szFilePath[30];
	int iRet;

//	sprintf(szFilePath, "./data/log/%8.8s.log", pszDate);
	sprintf(szFilePath, "/data/public/log/%8.8s.log", pszDate);
	iRet = remove(szFilePath);
	if (iRet < 0)
	{
		// file not exist
		if (errno == ENOENT)
		{
			return 0;
		}
		sprintf(g_szErrInfo, "Remove file[%s] error,iRet=[%d],errno=[%d]", szFilePath, iRet, errno);
		return LOG_ERR_REMOVE_FILE;
	}
	return iRet;
}


static int GetFirstLog(char *pszLastestDate)
{
	int iFileLen;
	int iCnt;
	char szDateBuff[8+1];
	char szCurDateTime[14+1];

	GetCurDate(szCurDateTime);
	iCnt = 0;
	while(1)
	{
		iCnt++;
		CalDate(glLogInfo.szLastDelLogDate, iCnt, szDateBuff);
		if (memcmp(szDateBuff, szCurDateTime, 8) > 0)
		{
			break;
		}
		iFileLen = GetFileInfo(szDateBuff);
		if (iFileLen > 0)
		{
			memcpy(pszLastestDate, szDateBuff, 8);
			*(pszLastestDate+8) = 0x00;
			return LOG_OK;
		}
	}
	memcpy(pszLastestDate, "00000000", 8);
	*(pszLastestDate+8) = 0x00;

	return LOG_ERR_NO_LOGFILE;
}


static int ChkLogStatus(FILE *fp)
{
	char szLastestDate[8+1];
	int iRet;
	int iFileLen;

	fseek(fp, 0, SEEK_END);
	iFileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (iFileLen == 0)
	{
		glLogInfo.cCurLogNum++;
		WriteLogInfo();
		if (glLogInfo.cCurLogNum > glLogInfo.cMaxLogNum)
		{
			memset(szLastestDate, 0, sizeof(szLastestDate));
			GetFirstLog(szLastestDate);
			iRet = DelOneFile(szLastestDate);
			if (iRet != 0)
			{
				return iRet;
			}
			if (glLogInfo.cCurLogNum > 0)
			{
				glLogInfo.cCurLogNum --;
			}
			memcpy(glLogInfo.szLastDelLogDate, szLastestDate, 8);
			WriteLogInfo();
			return LOG_OK;
		}
	}
	else
	{
		if (iFileLen > glLogInfo.iLogMaxLen)
		{
			sprintf(g_szErrInfo, "Current log file too long[%d>%d]", iFileLen, glLogInfo.iLogMaxLen);
			return LOG_ERR_TOO_LONG;
		}
	}

	return LOG_OK;
}

static int ReadLogInfo(void)
{
	int iFileId;
	int iRet;

//	iFileId = open("./data/loginfo.dat", O_RDWR);
	iFileId = open(LOG_FILE_PATH, O_RDWR);
	if (iFileId < 0)
	{
		if(access(LOG_FILE_PATH, F_OK) < 0)
		{
			char szCurDateTime[14+1];
			char szUpDate[8+1];

//			if(access("./data/log", W_OK|F_OK) < 0)
			if(access(LOG_FILE_FOLDER, W_OK|F_OK) < 0)
			{
//				iRet = mkdir("./data/log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				iRet = mkdir(LOG_FILE_FOLDER, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				if (iRet < 0)
				{
					sprintf(g_szErrInfo, "Make dir[./data/log] error,iRet=[%d],errno=[%d]", iFileId, errno);
					return LOG_ERR_MAKE_DIR;
				}
			}

			memset(&glLogInfo, 0, sizeof(ST_LOG_INFO));
			glLogInfo.cCurLevel  = E_LOG;
			glLogInfo.cCurLogNum = 0;
			glLogInfo.cMaxLogNum = 30;
			glLogInfo.iLogMaxLen = 100*1024;
			GetCurDate(szCurDateTime);
			CalDate(szCurDateTime, -1, szUpDate);
			memcpy(glLogInfo.szLastDelLogDate, szUpDate, 8);
			iRet = WriteLogInfo();
			return iRet;
		}
		else
		{
			sprintf(g_szErrInfo, "Open file[./loginfo.dat] error,iRet=[%d],errno=[%d]", iFileId, errno);
			return LOG_ERR_OPEN_FILE;
		}
	}

	iRet = read(iFileId, &glLogInfo, sizeof(ST_LOG_INFO));
	if (iRet != sizeof(ST_LOG_INFO))
	{
		sprintf(g_szErrInfo, "Read file[./loginfo.dat] error,iRet=[%d],errno=[%d]", iRet, errno);
		close(iFileId);
		return LOG_ERR_READ_FILE;
	}
	close(iFileId);

	return LOG_OK;
}
static int WriteLogInfo(void)
{
	int iFileId;
	int iRet;

//	iFileId = open("./data/loginfo.dat", O_RDWR | O_CREAT, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
	iFileId = open(LOG_FILE_PATH, O_RDWR | O_CREAT, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
	if (iFileId < 0)
	{
		sprintf(g_szErrInfo, "Open file[./data/loginfo.dat] error,iRet=[%d],errno=[%d]!", iFileId, errno);
		return LOG_ERR_OPEN_FILE;
	}
	iRet = write(iFileId, &glLogInfo, sizeof(ST_LOG_INFO));
	if (iRet != sizeof(ST_LOG_INFO))
	{
		sprintf(g_szErrInfo, "Write file[./data/loginfo.dat] error,iRet=[%d],errno=[%d]", iRet, errno);
		close(iFileId);
		return LOG_ERR_WRITE_FILE;
	}
	close(iFileId);
	return LOG_OK;
}

int LogInit(char ucLogLevel, char ucLogSaveDate, int iLogMaxLen)
{
	int iRet;

	iRet = ReadLogInfo();
	if (iRet != LOG_OK)
		return iRet;

	if (ucLogLevel >= E_LOG && ucLogLevel <= T_LOG)
	{
		glLogInfo.cCurLevel = ucLogLevel;
	}
	else
	{
		glLogInfo.cCurLevel = E_LOG;
	}
	if (ucLogSaveDate >= 0)
		glLogInfo.cMaxLogNum = ucLogSaveDate;
	if (iLogMaxLen >= 0)
		glLogInfo.iLogMaxLen = iLogMaxLen;

	iRet = WriteLogInfo();
	if (iRet != LOG_OK)
		return iRet;

	return LOG_OK;
}


int LogWrite(char cLogLevel, const char *pszParaInfo)
{
	FILE *fp;
	char szDateTime[14+3+1];
	char szBuff[20];
	char szLogPath[50];
	int iRet;

	if ((cLogLevel&0x7f) > glLogInfo.cCurLevel)
	{
		sprintf(g_szErrInfo, "Current Level[%d] > system level[%d]", cLogLevel, glLogInfo.cCurLevel);
		return LOG_ERR_LEVEL_LOW;
	}

	if ((cLogLevel&0x7f) == T_LOG)
	{
		strcpy(szBuff, "[trace]");
	}

	if ((cLogLevel&0x7f) == W_LOG)
	{
		strcpy(szBuff, "[warning]");
	}

	if ((cLogLevel&0x7f) == E_LOG)
	{
		strcpy(szBuff, "[error]");
	}
	GetCurDate2(szDateTime);
//	sprintf(szLogPath, "./data/log/%8.8s.log",szDateTime);
	sprintf(szLogPath, "/data/public/log/%8.8s.log",szDateTime);

	fp = fopen(szLogPath, "a+");
	if (fp == NULL)
	{
		sprintf(g_szErrInfo, "fopen file[%s] error,errno=[%d]", szLogPath, errno);
		return LOG_ERR_OPEN_FILE;
	}
	iRet = ChkLogStatus(fp);
	if (iRet != LOG_OK)
	{
		fclose(fp);
		return iRet;
	}

	if (!(cLogLevel&0x80))
	{
		fprintf(fp, "%2.2s:%2.2s:%2.2s:%3.3s %s:", szDateTime+8, szDateTime+10, szDateTime+12, szDateTime+14, szBuff);
	}
	fprintf(fp, pszParaInfo);
	fprintf(fp, "\n");
	fflush(fp);
	fclose(fp);

	return LOG_OK;
}

int LogQueAll(char * pszLogFileOut)
{
	int iRet;
	int iCnt;
	int iFileLen;
	char szDateBuff[8+1];
	char szCurDateTime[14+1];
	char szBuff[50];

	if (pszLogFileOut == NULL)
	{
		strcpy(g_szErrInfo, "QueAll parameter 1[pszLogFileOut] is NULL");
		return LOG_ERR_INVALID_PARA;
	}

	iRet = ReadLogInfo();
	if (iRet != 0)
		return iRet;

	GetCurDate(szCurDateTime);
	iCnt = 0;
	while(1)
	{
		iCnt++;
		CalDate(glLogInfo.szLastDelLogDate, iCnt, szDateBuff);
		if (memcmp(szDateBuff, szCurDateTime, 8) > 0)
		{
			break;
		}
		iFileLen = GetFileInfo(szDateBuff);
		if (iFileLen > 0)
		{
			sprintf(szBuff, "%8.8s[%d];", szDateBuff, iFileLen);
			strcat(pszLogFileOut, szBuff);
		}
	}
	return LOG_OK;
}
int LogDelOne(const char *pszDate)
{
	int iFileLen;
	int iRet;
	char szLastestDate[8+1];

	if (pszDate == NULL)
	{
		strcpy(g_szErrInfo, "DelOne parameter 1[pszDate] is NULL");
		return LOG_ERR_INVALID_PARA;
	}

	memset(szLastestDate, 0, sizeof(szLastestDate));
	GetFirstLog(szLastestDate);
	if (memcmp(pszDate, szLastestDate, 8) == 0)
	{
		iRet = DelOneFile(szLastestDate);
		if (iRet != 0)
			return iRet;
		if (glLogInfo.cCurLogNum > 0)
		{
			glLogInfo.cCurLogNum --;
		}
		memcpy(glLogInfo.szLastDelLogDate, szLastestDate, 8);
		WriteLogInfo();
	}
	else
	{
		iFileLen = GetFileInfo(pszDate);
		if (iFileLen <= 0)
		{
			return LOG_ERR_FILE_NOEXIST;
		}
		iRet = DelOneFile(pszDate);
		if (iRet != 0)
			return iRet;
		if (glLogInfo.cCurLogNum > 0)
		{
			glLogInfo.cCurLogNum--;
		}
		WriteLogInfo();
	}
	return LOG_OK;
}
int LogDelAll(void)
{
	int iRet;
	int iLastRet;
	int iCnt;
	int iFileLen;
	char szDateBuff[8+1];
	char szCurDateTime[14+1];

	iRet = ReadLogInfo();
	if (iRet != 0)
		return iRet;

	iLastRet = 0;
	GetCurDate(szCurDateTime);
	iCnt = 0;
	while(1)
	{
		iCnt++;
		CalDate(glLogInfo.szLastDelLogDate, iCnt, szDateBuff);
		if (memcmp(szDateBuff, szCurDateTime, 8) > 0)
		{
			break;
		}
		iFileLen = GetFileInfo(szDateBuff);
		if (iFileLen > 0)
		{
			iRet = DelOneFile(szDateBuff);
			if (iRet != 0)
			{
				iLastRet = iRet;
			}
			if (glLogInfo.cCurLogNum > 0)
			{
				glLogInfo.cCurLogNum --;
			}
			memcpy(glLogInfo.szLastDelLogDate, szDateBuff, 8);
		}
	}

	WriteLogInfo();
	return iLastRet;
}



const char* LogGetVer(void)
{
	memset(g_szVerInfo, 0, sizeof(g_szVerInfo));
	sprintf(g_szVerInfo, "%s", LOG_VER);
//	sprintf(g_szVerInfo+10, "%s %s", __DATE__, __TIME__);
	return &g_szVerInfo[0];
}

const char* LogGetLastErrMsg(void)
{
	return &g_szErrInfo[0];
}

int Pax_LogInit(const char* pszTag)
{
#ifdef DEBUG_LOG_OPEN
    OsLogSetTag(pszTag);
#ifdef DEBUG_LOG_WRITE_FILE
    LogInit(T_LOG, 0xFF, 0xFFFFFFFF);
#endif

#endif
    return 0;
}

int Pax_Log(LOG_T Prio, const char *fmt, ...)
{
#ifdef DEBUG_LOG_OPEN
    va_list vaMarker;
    char buf[2048]={0};
    int logLevel=0;

    memset(buf, 0, sizeof(buf));
    va_start(vaMarker, fmt);
    vsnprintf(buf, sizeof(buf), (const char*)fmt, vaMarker);
    va_end(vaMarker);

    OsLog(Prio, buf);

#ifdef DEBUG_LOG_WRITE_FILE
    if ( Prio == LOG_WARN ) {
    	logLevel = W_LOG;
    } else if ( Prio == LOG_ERROR ) {
    	logLevel = E_LOG;
    } else {
    	logLevel = T_LOG;
    }
    _LOG(logLevel, buf);
#endif

#endif
    return 0;
}

