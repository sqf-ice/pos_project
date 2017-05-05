#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_define.h"
#include "os_interface.h"
#include "defines.h"
#include "struct.h"
#include "utility.h"
#include "message.h"
#include "SLCLOCK.h"



#define IR_MAGIC	32

extern PTStruct gPTStruct;

// Gary : delete blacklist processing
// WORD Check_BlackList(void)
// {
//     return STAT_NOK;
// }


// Gary : This function is never called.
WORD Check_ExpDate(BYTE *fpExpDate)
{
    BYTE lbCardYear;           // Year of ExpDate
    BYTE lbCardMonth;          // Month of ExpDate
    BYTE lpCurrentTime[13];    // Actual time DDMMYYHHMMSS
    BYTE lbActualYear;         // Actual year
    BYTE lbActualMonth;        // Actual Month
    BYTE Statu=OS_FALSE;
    
    // Calculate Year and month of exp date
    lbCardYear = (fpExpDate[0]-'0')*10 + (fpExpDate[1]-'0');
    lbCardMonth = (fpExpDate[2]-'0')*10 + (fpExpDate[3]-'0');
    
    // Calculate current year and month
    ClkReadDate(lpCurrentTime, F_DDMMYY);
    
    lbActualYear = (lpCurrentTime[4]-'0')*10 + (lpCurrentTime[5]-'0');
    lbActualMonth = (lpCurrentTime[2]-'0')*10 + (lpCurrentTime[3]-'0');
    
    if ( (lbActualMonth<1) || (lbActualMonth>12) )     // Wrong Month
        Statu=OS_TRUE;
    
    if ( (lbCardMonth<1) || (lbCardMonth>12) )     // Wrong Month
        Statu=OS_TRUE;
    
    if (lbCardYear < 60) // 21st Century Fox
        lbCardYear += 100;
    
    if (lbActualYear < 60) // 21st Century Fox
        lbActualYear += 100;
    
    if (lbCardYear < lbActualYear)   // Card Expired
        Statu=OS_TRUE;
    
    if ((lbCardYear == lbActualYear) && (lbCardMonth < lbActualMonth)) // Card Expired this year
    {
        Statu=OS_TRUE;
    }
    
    //Exp.Date
    if(Statu==OS_TRUE)
    {
        DisplayString(DSP_W_EXPIRY_DATE, 3000);
        return(STAT_NOK);
    }

    // Card is not expired
    return STAT_OK;
}


WORD Check_TransFileEmpty( WORD *fpLastRecNum,BYTE *fpFileName )
{
    DWORD llLogicSize;
    
    *fpLastRecNum=0;
    
    if (GetFileSize(fpFileName,&llLogicSize) != NO_ENTRY)
    {        
        if ( llLogicSize >= (sizeof(TransStruct)))
        {            
            *fpLastRecNum = (llLogicSize / (sizeof(TransStruct))) - 1;
            return (STAT_OK);
        }
        else
        {
            return (STAT_NOK);
        }
    }
    
    return (STAT_INV);
}

WORD NextSequenceNo( WORD *fpSeqNum)
{
    WORD  lwStatus;
    WORD  fpLastRecNum;
    
    lwStatus = Check_TransFileEmpty( &fpLastRecNum,FILE_TR );
    if ( lwStatus == STAT_OK )   /* if not empty */
    {
        *fpSeqNum = fpLastRecNum + 1 + 1;
    }
    else
    {
        if ( lwStatus == STAT_NOK )   /* if empty */
            *fpSeqNum = 1;
        else
            return( STAT_INV );
    }
    return( STAT_OK );
}

WORD PaperControl( void )
{
    // Gary : This function originally intended for paper-checking before print
    // N/A for PAX S80
    return STAT_OK;
}

WORD BinCheck(void)
{
    int  fileHandle;
    int  lbStatus;
    BYTE laToReadBuf[25];
    
    fileHandle = Open(FILE_BIN, O_RDWR);
    if (fileHandle < 0)
        return ( STAT_NOK );
    
    if (Seek (fileHandle, 0, SEEK_SET) < 0 )
    {
        Close(fileHandle);
        return ( STAT_NOK );
    }
    
    lbStatus = Read(fileHandle, laToReadBuf, LG_BINSTRUCT);
    while (lbStatus>0)
    {
        if (memcmp( gPTStruct.Pan,laToReadBuf,  6)==0)
        {
            gPTStruct.CardBinFlags=laToReadBuf[6];
            Close(fileHandle);
            return ( STAT_OK );
        }
        lbStatus=Read(fileHandle, laToReadBuf, LG_BINSTRUCT);
    }//while
    
    Close(fileHandle);
    
#ifdef APPL_TEST
    return ( STAT_OK );	
#endif

    DisplayString (DSP_CARD_BIN_NOT_FOUND,2000);
    
    return (STAT_NOK);
}

// end of file
