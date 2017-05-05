#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_interface.h"
#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "COMMON.h"

BYTE gbTermFlags;

WORD ResetStateFile(void)
{
    WORD lwStatus;
    int Handle;
    
    // if the file is not yet created then create the file
    Handle = Open(FILE_STATE, O_RDWR);
    if (Handle < 0)
    {
        lwStatus = InitStateFile();
        if (lwStatus == STAT_OK)
        {
            return STAT_NOK;
        }
        else
        {
            return STAT_INV;
        }
    }

    if (Read(Handle,(BYTE *)&gState,LG_MSTATESTRUCT)>0)
    {
        Close(Handle);
        return STAT_OK;
    }

    Close(Handle);
    lwStatus = InitStateFile();
    if (lwStatus == STAT_OK)
    {
        return STAT_NOK;
    }
    else
    {
        return STAT_INV;
    }
}

WORD InitStateFile(void)
{
    if (FileInit(FILE_STATE)==STAT_NOK)
        return STAT_NOK;
    
    gState.ApplStateInit = OS_NO;
    gState.TermFlags = 0;
    gbTermFlags = 0;
    SetTermFlags();
    
    return STAT_OK;
}

INT ReadStateFile(void)
{
    int lbHandler;
    
    lbHandler = Open(FILE_STATE, O_RDWR);
    
    memset(&gState,0,sizeof(gState));
    
    if (lbHandler >= 0)
    {
        if (Read(lbHandler, (BYTE*)&gState, LG_MSTATESTRUCT) > 0)
        {
            gbTermFlags = gState.TermFlags;            
        }
        
        Close(lbHandler);
        return OS_OK;        
    }
    
    return OS_NOK;
}

void SetTermFlags(void)
{
    int lbHandler;
    
    gState.TermFlags = gbTermFlags;
    lbHandler = Open( FILE_STATE, O_RDWR);
    if (lbHandler >= 0)
    {
        Write( lbHandler, (BYTE*)&gState, LG_MSTATESTRUCT);
        Close(lbHandler);
    }
}
