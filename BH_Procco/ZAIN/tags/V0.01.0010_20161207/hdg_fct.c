#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "defines.h"
#include "os_interface.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "utility.h"


WORD HDGReset(void)
{
	HDGStruct lpStr;	// Buffer used to read from file
	int lbFileHandler;

	lbFileHandler=Open(FILE_GP, O_RDWR);
	if (lbFileHandler < 0)
	{
		if (HDGInit()==STAT_OK)
			return (STAT_NOK);
		else
			return (STAT_INV);
	}

	if (!(Read(lbFileHandler,(BYTE *)&lpStr, (sizeof(HDGStruct)))>0))    // Unexpected error call
	{
		Close(lbFileHandler);

		if (HDGInit()==STAT_OK)
			return(STAT_NOK);
		else
			return(STAT_INV);
	}
	if (memcmp(lpStr.MerchId, UTIL_STARS, LG_MERCHID)==0) //"0000" -> "0090"
	{
		Close(lbFileHandler);
		return (STAT_NOK);
	}

	Close(lbFileHandler);
	return (STAT_OK);
}

WORD HDGInit(void)
{
	HDGStruct lsHDGStruct;
	int lbFileHandler;     // Handler of HDG File

	if (FileInit(FILE_GP)==STAT_NOK)
		return(STAT_NOK);

	lbFileHandler = Open(FILE_GP, O_RDWR);
	if ( lbFileHandler<0 )	// Unexpected error
	{
		return(STAT_NOK);
	}

	gDebug=OS_FALSE;    // set the bit for network terminal
	gDebugType = DEBUG_NONE;
	memset(&lsHDGStruct, 0, sizeof(HDGStruct));
	memset(lsHDGStruct.MerchId,'*',  LG_MERCHID);
	memset(lsHDGStruct.StoreNum,'*', LG_STORENUM);
	memset(lsHDGStruct.TermId,'*',   LG_TERMID);
	memset(lsHDGStruct.MerchData,'*',LG_MERCHDATA);

	memset(lsHDGStruct.NETSERVERTel1,'*', LG_PHONE);
	memset(lsHDGStruct.NETSERVERTel2,'*', LG_PHONE);
	memset(lsHDGStruct.NETSERVERTel3,'*', LG_PHONE);
	//memcpy(lsHDGStruct.NETSERVERNII,"0003", 4);
	memcpy(lsHDGStruct.NETSERVERNII,"0661", 4);

	memset(lsHDGStruct.PMSTel1,'*',LG_PHONE);
	memset(lsHDGStruct.PMSTel2,'*',LG_PHONE);
	memset(lsHDGStruct.PMSTel3,'*',LG_PHONE);
	//memcpy(lsHDGStruct.PMSNII,"0005",4);
	memcpy(lsHDGStruct.PMSNII,"0662",4);


	memset(lsHDGStruct.TMSTel1,'*',LG_PHONE);
	memset(lsHDGStruct.TMSTel2,'*',LG_PHONE);
	memset(lsHDGStruct.TMSTel3,'*',LG_PHONE);
	//memcpy(lsHDGStruct.TMSNII,"0007",4);
	memcpy(lsHDGStruct.TMSNII,"0663",4);


	memset(lsHDGStruct.MerchantTypeVersion,'0',LG_PARMVERSION);
	memset(lsHDGStruct.BinVersion,'0',LG_PARMVERSION);
	memset(lsHDGStruct.BlackCardVersion,'0',14);
	memset(lsHDGStruct.ProductVersion,'0',LG_PARMVERSION);
	memset(lsHDGStruct.SchemeTypeVersion,'0',LG_PARMVERSION);
	memset(lsHDGStruct.RepaymentTenuresVersion,'0',LG_PARMVERSION);

	memset(lsHDGStruct.BatchNum,'0', 6);
	lsHDGStruct.BatchNum[6-1] = '0';
	lsHDGStruct.MaxTrans = 60;
	gMaxTranaction =60;

	lsHDGStruct.OperationFlagsSmart1 = 0;
	gOperationFlagsSmart1 = 0;
	lsHDGStruct.OperationFlagsSmart2 = 0;
	gOperationFlagsSmart2 = 0;

	lsHDGStruct.TransCount=0;
	lsHDGStruct.TraceNum=0;
	lsHDGStruct.bLastPrint = 0;    //add by Allen 20151117

	Seek(lbFileHandler, 0, SEEK_SET);
	if (Write(lbFileHandler,(BYTE *)&lsHDGStruct, sizeof(HDGStruct))!=0)     // Unexpected error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
    
	Close(lbFileHandler);
	return (STAT_OK);
}

WORD HDGWrite(HDGStruct * fpHDGBuf)
{
	int lbFileHandler;

	lbFileHandler=Open(FILE_GP, O_RDWR);
	if (lbFileHandler < 0)	// Unexpected error
	{
		return(STAT_NOK);
	}
	if (Write(lbFileHandler,(BYTE *)fpHDGBuf, sizeof(HDGStruct))!=0)     // Write error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
	Close(lbFileHandler);
	return (STAT_OK);
}

WORD HDGRead(HDGStruct *fpHDGBuf)
{
	int lbHandler;
	BYTE lbTmpTotal[5];

	// Open HDG File
	lbHandler = Open(FILE_GP, O_RDWR);
	if (lbHandler < 0)	// Unexpected error
    {
		return STAT_NOK;
    }

	// Read first record
	if ( Read(lbHandler,(BYTE *)fpHDGBuf, sizeof(HDGStruct)) != sizeof(HDGStruct) )// Read error
	{
		Close(lbHandler);
		return STAT_NOK;
	}

	gOperationFlagsSmart1 = fpHDGBuf->OperationFlagsSmart1;
	gOperationFlagsSmart2 = fpHDGBuf->OperationFlagsSmart2;
	gMaxTranaction = fpHDGBuf->MaxTrans;
	memcpy(gPTStruct.MerchData , fpHDGBuf->MerchData , 96);
	memcpy(gCurrencySymbol,fpHDGBuf->trCurrencySymbol,3);
	gCurrencyExponent = fpHDGBuf->TransactionCurrencyExponent;

	memcpy(gPTStruct.MerchId,     fpHDGBuf->MerchId,  LG_MERCHID); // 12
	memcpy(gPTStruct.StoreNum,    fpHDGBuf->StoreNum, LG_STORENUM); // 8
	memcpy(gPTStruct.TerminalID,  fpHDGBuf->TermId,   LG_TERMID); // 8
	memcpy(gPTStruct.CountryCode, fpHDGBuf->terminalCountryCode,2);
	memcpy(gPTStruct.CurrencyCode,fpHDGBuf->trCurrencyCode,2);

	memcpy(lbTmpTotal,fpHDGBuf->NETSERVERNII,4);
	lbTmpTotal[4]=0;
	//by jarod 20110805
	//StrAscToBcd(lbTmpTotal, gNETSERVERNII);
	StrAscToHex(lbTmpTotal, gNETSERVERNII);

	memcpy(lbTmpTotal,fpHDGBuf->PMSNII,4);
	lbTmpTotal[4]=0;
	//by jarod 20110805
	//StrAscToBcd(lbTmpTotal, gPMSNII);
	StrAscToHex(lbTmpTotal, gPMSNII);
	
	memcpy(lbTmpTotal,fpHDGBuf->TMSNII,4);
	lbTmpTotal[4]=0;
	//by jarod 20110805
	//StrAscToBcd(lbTmpTotal, gTMSNII);
	StrAscToHex(lbTmpTotal, gTMSNII);

	
	Close(lbHandler);
	return STAT_OK;
}


WORD GPRSInit(void)
{
	GPRSStruct GPStruct;
	int lbFileHandler;     // Handler of HDG File

	if (FileInit(FILE_GPRS_PAR)==STAT_NOK)
		return(STAT_NOK);

	lbFileHandler=Open(FILE_GPRS_PAR, O_RDWR);
	if (lbFileHandler < 0)	// Unexpected error
	{
		return(STAT_NOK);
	}
	memset(&GPStruct,0,sizeof(GPRSStruct));

	Seek(lbFileHandler,0,SEEK_SET);
	if (Write(lbFileHandler,(BYTE *)&GPStruct, sizeof(GPRSStruct))!=0)     // Unexpected error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
	Close(lbFileHandler);
	return (STAT_OK);
}


WORD GPRSRead(GPRSStruct *GPStruct)
{
	int lbHandler;     // Handler o

	// Open HDG File
	lbHandler=Open(FILE_GPRS_PAR, O_RDWR);
	if (lbHandler < 0)	// Unexpected error
    {
		return (STAT_NOK);
    }

	// Read first record
	if (!(Read(lbHandler,(BYTE *)GPStruct, (sizeof(GPRSStruct)))>0))// Read error
	{
		Close(lbHandler);
		return(STAT_NOK);
	}
	Close(lbHandler);
	return (STAT_OK);
}


WORD GPRSWrite(GPRSStruct *GPStruct)
{
	int lbFileHandler;

	lbFileHandler=Open(FILE_GPRS_PAR, O_RDWR);
	if (lbFileHandler < 0)	// Unexpected error
	{
		return(STAT_NOK);
	}

	if (Write(lbFileHandler,(BYTE *)GPStruct, sizeof(GPRSStruct))!=0)     // Write error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
	
	Close(lbFileHandler);
	return (STAT_OK);
}

//added by Allen for ZAIN 2016-05-19
WORD Dial_upInit(void)
{
	Dial_upStruct DialStruct;
	int lbFileHandler;     // Handler of HDG File

	if (FileInit(FILE_DIAL_UP_PAR)==STAT_NOK)
		return(STAT_NOK);

	lbFileHandler=Open(FILE_DIAL_UP_PAR, O_RDWR);
	if (lbFileHandler < 0)	// Unexpected error
	{
		return(STAT_NOK);
	}
	memset(&DialStruct, 0 ,sizeof(Dial_upStruct));

	Seek(lbFileHandler,0,SEEK_SET);
	if (Write(lbFileHandler,(BYTE *)&DialStruct, sizeof(Dial_upStruct))!=0)     // Unexpected error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
	Close(lbFileHandler);
	return (STAT_OK);	
}

WORD Dial_upRead(Dial_upStruct *DialStruct)
{
	int lbHandler;     // Handler o

	// Open HDG File
	lbHandler=Open(FILE_DIAL_UP_PAR, O_RDWR);
	if (lbHandler < 0)	// Unexpected error
    {
		return (STAT_NOK);
    }

	// Read first record
	if (!(Read(lbHandler,(BYTE *)DialStruct, (sizeof(Dial_upStruct)))>0))// Read error
	{
		Close(lbHandler);
		return(STAT_NOK);
	}
	Close(lbHandler);
	return (STAT_OK);
}


WORD Dial_upWrite(Dial_upStruct *DialStruct)
{
	int lbFileHandler;

	lbFileHandler=Open(FILE_DIAL_UP_PAR, O_RDWR);
	if (lbFileHandler < 0)	// Unexpected error
	{
		return(STAT_NOK);
	}

	DialStruct->stPara.DP = 0;
	DialStruct->stPara.CHDT = 0;
	DialStruct->stPara.DT1 = 20;
	DialStruct->stPara.DT2 = 10;
	DialStruct->stPara.HT = 70;
	DialStruct->stPara.WT = 5;
	DialStruct->stPara.SSETUP = 0x05; 
	DialStruct->stPara.DTIMES = 1;
	DialStruct->stPara.TimeOut = 6;
	DialStruct->stPara.AsMode = 0x00;
	DialStruct->ucSendMode = CM_SYNC;

	if (Write(lbFileHandler,(BYTE *)DialStruct, sizeof(Dial_upStruct))!=0)     // Write error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
	
	Close(lbFileHandler);
	return (STAT_OK);
}

WORD SaveConnectMode(void)
{
	int lbFileHandler;

	lbFileHandler=Open(FILE_CONNECT_MODE, O_RDWR);
	if (lbFileHandler < 0)	// Unexpected error
	{
		return(STAT_NOK);
	}

	if (Write(lbFileHandler, &gConnectMode, sizeof(BYTE))!=0)     // Write error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
	
	Close(lbFileHandler);
	return (STAT_OK);
}

WORD GetConnectMode(void)
{
	int lbHandler;     // Handler o

	// Open HDG File
	lbHandler=Open(FILE_CONNECT_MODE, O_RDWR);
	if (lbHandler < 0)	// Unexpected error
    {
		return (STAT_NOK);
    }

	// Read first record
	if (!(Read(lbHandler, &gConnectMode, sizeof(BYTE))>0))// Read error
	{
		Close(lbHandler);
		return(STAT_NOK);
	}
	Close(lbHandler);
	return (STAT_OK);
}

WORD SaveEODFlag()
{
	int lbFileHandler;

	lbFileHandler=Open(FILE_END_OF_DAY, O_RDWR);
	if (lbFileHandler < 0)	// Unexpected error
	{
		return(STAT_NOK);
	}

	if (Write(lbFileHandler, &gEODParam, sizeof(EODPARAM))!=0)     // Write error
	{
		Close(lbFileHandler);
		return(STAT_NOK);
	}
	
	Close(lbFileHandler);
	return (STAT_OK);
}

WORD GetEODFlag(void)
{
	int lbHandler;     // Handler o

	// Open HDG File
	lbHandler=Open(FILE_END_OF_DAY, O_RDWR);
	if (lbHandler < 0)	// Unexpected error
    {
		return (STAT_NOK);
    }

	// Read first record
	if (!(Read(lbHandler, &gEODParam, sizeof(EODPARAM))>0))// Read error
	{
		Close(lbHandler);
		return(STAT_NOK);
	}
	Close(lbHandler);
	return (STAT_OK);
}
//add end