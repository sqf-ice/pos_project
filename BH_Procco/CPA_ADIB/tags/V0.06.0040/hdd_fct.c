#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "defines.h"
#include "os_interface.h"
#include "struct.h"
#include "function.h"
#include "message.h"

WORD HDDInit(void)
{
    HDDStruct lsHDDStruct;
    int lbFileHandler;     		// Handler of HDD File
    
    if (FileInit(FILE_DP)==STAT_NOK)
        return(STAT_NOK);
    
    lbFileHandler=Open(FILE_DP, O_RDWR);
    if (lbFileHandler<0)  // Unexpected error
    {
        return(STAT_NOK);
    }

    memset(&lsHDDStruct, 0, sizeof(HDDStruct));
    memcpy(lsHDDStruct.BCStartTime, UTIL_1800, LG_TRANSTIME);
    memcpy(lsHDDStruct.ParmStartTime, UTIL_1800, LG_TRANSTIME);
    lsHDDStruct.ParmAttempts 	= MAX_CALL_ATTEMPT;
    lsHDDStruct.BCAttempts 		= MAX_CALL_ATTEMPT;
    lsHDDStruct.MaxAttempts -= MAX_CALL_ATTEMPT;
    lsHDDStruct.AttemptInterval = CALL_ATTEMPT_INTERVAL;
    
    if (Write(lbFileHandler, (BYTE *)&lsHDDStruct, LG_HDDSTRUCT)!=0)     // Write error
    {
        Close(lbFileHandler);
        return(STAT_NOK);
    }
    Close(lbFileHandler);
    return (STAT_OK);
}


WORD HDDWrite(HDDStruct * fpHDDBuf)
{
    int lbFileHandler;    
    
    // Open HDD File
    lbFileHandler=Open(FILE_DP, O_RDWR);
    if (lbFileHandler < 0)
    {
        return(STAT_NOK);
    }

    // Write first record
    if (Write(lbFileHandler,(BYTE *)fpHDDBuf, LG_HDDSTRUCT)!=0)     // Write error
    {
        Close(lbFileHandler);
        return(STAT_NOK);
    }
    Close(lbFileHandler);
    return (STAT_OK);
}

WORD HDDRead(HDDStruct *fpHDDBuf)
{
    int lbFileHandler;
    
    // Open HDD File
    lbFileHandler=Open(FILE_DP, O_RDWR);
    if (lbFileHandler < 0)
    {
        return(STAT_NOK);
    }
    // Read first record
    if (Read(lbFileHandler,(BYTE*)fpHDDBuf, LG_HDDSTRUCT) <=0)     // Read error
    {
        Close(lbFileHandler);
        return(STAT_NOK);
    }
    Close(lbFileHandler);
    return (STAT_OK);
}
