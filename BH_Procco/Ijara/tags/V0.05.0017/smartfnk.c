#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "debug.h"

#include "TYPEDEF.h"
#include "COMMON.h"
#include "DSSAPI.h"
#include "Apiman.h"

#include "os_define.h"
#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "commsdlm_vx.h"


SC_STATUS PF_Reset(SC_READER reader, SC_RESET_MODE mode, SC_TYPE type, BYTE *ATRString);


SC_STATUS SC_CardDetect(SC_READER fwReader, BOOLEAN fwControl)
{
    // Gary
    if (IccDetect(CUSTOMER_CARD)==0)
    {
        return SC_OK;
    }
    else
    {
        return SC_CARD_REMOVED;
    }

    /*
    char retVal;
    
    retVal = Get_Card_State(CUSTOMER_CARD);    
    if (retVal == E_SLOT_INITIALIZE)
    {
        Init_CardSlot(CUSTOMER_CARD);
    }
    
    if (inVXEMVAPCardPresent() == SUCCESS)
        return( SC_OK );
    else	
        return( SC_CARD_REMOVED );
    */
}

void PowerDown_CardSlot(unsigned long ulSlotNo)
{
    // Gary
    IccClose((BYTE)ulSlotNo);
    DelayMs(500);

    /*
    int iStatus;

    iStatus = Terminate_CardSlot(ulSlotNo, SWITCH_OFF_CARD);
    if(iStatus != CARDSLOT_SUCCESS)
    {
        display_at (1, 4, "   PWROFF Fail  ", CLR_LINE);
    }
    //	display_at (1, 4, "     Closing ICC     ", CLR_LINE);
    iStatus = Close_CardSlot(ulSlotNo);
    if(iStatus != CARDSLOT_SUCCESS)
        display_at (1, 4, "    Close ICC Fail   ", CLR_LINE);
    DelayMs(500);
    */
}

SC_STATUS SC_CardDisconnect( SC_READER fwReader )
{
    PowerDown_CardSlot(CUSTOMER_CARD);
    return SC_OK; 
}

BOOLEAN   SC_CardInserted( SC_READER card_slot, DWORD timeout )
{
#if 0
    int retVal;
    
    retVal = Get_Card_State (CUSTOMER_CARD);
    
    if (retVal == E_SLOT_INITIALIZE)
    {
        Init_CardSlot (CUSTOMER_CARD);
    }
    
    if ( retVal == CARD_PRESENT)
    {
        return( SC_OK );
    }
    else	
    {
        return( SC_CARD_REMOVED );	
    }
#endif
	return SC_OK;
}

SC_STATUS SC_CardReset( SC_READER fwReader, SC_RESET_MODE fwMode,
                            SC_TYPE fwType, BYTE *fpBuffer )
{
    // Gary
    if (IccInit(CUSTOMER_CARD, fpBuffer)!=0)
    {
        return SC_OK;
    }
    else
    {
        return SC_ERROR;
    }

    /*
    unsigned long slotNo;
    unsigned long Value;
    unsigned char retval;
    char retVal;
    
    slotNo = CUSTOMER_CARD;    
    Value = slotNo;
    
    retVal = Get_Card_State (CUSTOMER_CARD);
    if (retVal == E_SLOT_INITIALIZE)
    {
        Init_CardSlot (CUSTOMER_CARD);
        display_at (1, 4, "   Init_CardSlot   ", CLR_LINE);
    }
    
    retval = (byte)IFD_Set_Capabilities( Tag_Open_ICC, (BYTE*)(&Value));	
    if(retval != IFD_Success)
    {
        return ( SC_CARD_REMOVED );
    }
    
    Value = slotNo;
    retval = (byte)IFD_Set_Capabilities ( Tag_Select_ICC , (BYTE*)(&Value));
    if(retval != IFD_Success) 
    {
        return ( SC_BAD_CARD_TYPE );
    }
    retval = (byte)IFD_Power_ICC (IFD_POWER_UP);
    if(retval != IFD_Success)
    {
        return ( SC_BAD_CARD_TYPE );
    }
    retval = (byte)IFD_Power_ICC (IFD_RESET);
    if(retval != IFD_Success)
    {
        return ( SC_BAD_CARD_TYPE );
    }
    retval = (byte)IFD_Get_Capabilities( Tag_ATR_String , fpBuffer );
    
    if(retval != IFD_Success) 
        return SC_CARD_REMOVED;
    
    return( SC_OK );
    */
}


SC_STATUS CardConnect(SC_READER reader)
{
    BYTE lWorkBuf[30];
    
    scStatus = SC_CardDisconnect(reader);
    if (scStatus == SC_OK)
    {
        scStatus = SC_CardDetect(reader, OS_TRUE);
    }    
    
    scStatus = PF_Reset(reader, SC_COLD_RESET, ASYNC_CARD, gATRString);
    if (scStatus != SC_OK)
    {
        return scStatus;
    }
    
    HexToAsc(gATRString+1,lWorkBuf,14);
    lWorkBuf[24]=0;
    
    // Gary : ???? cannot understand below
    if((memcmp(lWorkBuf,"3B79940000",10)==0)&&//PAYF
        (memcmp(lWorkBuf,"3B69000059",10)==0))//PAYF
    {
        return(scStatus);	//!!!sel??? sonra kontrol et ATR kontrolu
        return(SC_ERROR);
    }
    return(scStatus);
}


SC_STATUS PF_Reset(SC_READER reader, SC_RESET_MODE mode, SC_TYPE type, BYTE *ATRString)
{
    scStatus = SC_CardReset(reader, mode, type, ATRString);    
    if (scStatus != SC_OK)
    {
        scStatus = SC_CardReset(reader, mode, type, ATRString);
    }
    
    return(scStatus);
}

SC_STATUS PF_CardRemove(void)
{
    OS_ClearScreen();

	if (IccDetect(0)==0xFF)
	{
		return SC_CARD_REMOVED;
	}
    ScrPrint(0, 4, CFONT, DSP_REMOVE_CARD);
    while(IccDetect(0)!=0xFF)
    {
		DelayMs(500);
    }

    return SC_OK;
}




