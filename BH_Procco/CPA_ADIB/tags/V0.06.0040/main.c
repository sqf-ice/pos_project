#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

/////////////////////////////////////////////////////////
//#include <vx_project.def>
#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "iso.h"
#include "COMMON.h"
#include "os_interface.h"


BYTE gLanguage;       // Current phone's sequance number like 0,1,2
// --- global data ---
BYTE CommandBuf[60];
SC_STATUS scStatus;
SWORD sw1sw2;

MStateStruct 	 gState;
PTStruct gPTStruct;
CardParam gCardParam;

//struct TransactionStruct gK_PTStruct;

BYTE gOldItem;
BYTE gOldSubItem;
int gTrHandle;
BYTE gATRString[30];
unsigned char selectionCompleted;

BYTE gOperationFlagsSmart1;
BYTE gOperationFlagsSmart2;
BYTE gMerchantOperationFlags;
WORD gMaxTranaction;
BYTE gHostType;

BYTE gbFLagPartialInitState;
BYTE gDebug;
BYTE gDebugType;
BYTE gtransactionType;
BYTE gHardwareType;

BYTE gCurrencySymbol[3];
BYTE gCurrencyExponent;

BYTE gEntryType;
BYTE gPIN[18];


BYTE gAmountDebit[LG_AMOUNT+1];
BYTE gTempBuf[LG_AMOUNT+1];
BYTE gAmountLoyalty[LG_AMOUNT+1];
BYTE gAmountPayment[LG_AMOUNT+1];
BYTE gAmountGeneral[LG_AMOUNT+1];
BYTE gAmountGeneralWithPayment[LG_AMOUNT+1];
BYTE gCounterDebit[3];
BYTE gCounterLoyalty[3];
BYTE gCounterPayment[3];
BYTE gCounterGeneral[3];
BYTE gCounterGeneralWithPayment[3];
BYTE gField55[255];
BYTE gField55Length;
BYTE gKeyCancel;

BYTE gNETSERVERNII[3];
BYTE gPMSNII[3];
BYTE gTMSNII[3];

TransactionStruct TR;
ISODataStruct sendData,receivedData,reversalData, ISOData;
ISOFrame frameToSend,frameReceived;

BYTE  gDispLogo[8][128]={0};
void ApplicationActive( BYTE PartialInitialized);
void SetTermFlags(void);

void M_MerchantMenu(void)
{
    BYTE lbRet;       			// Return of menu
    BYTE lbDuplicateTran;
    BYTE lbEndOfDay;
    BYTE lbParameter;
    BYTE lbPrintEODTicket;
    BYTE lbLists;
    BYTE lbSecurity;
    WORD lwStatus = STAT_NOK;
    HDGStruct lsHDGBuf;

    ReadStateFile();
    
    if (!(gOperationFlagsSmart1 & SC_TERMINAL_ACTIVE))
    {
		//by jarod 20110906
		//ABD said, remove this message

        //DisplayString_S80(DSP_TERMINAL_NOT_ALLOWED, 4000);
    }
    gOldItem=1;
    while(lwStatus!=STAT_OK)
    {
        lbDuplicateTran = 1;
        lbEndOfDay      = 1;
        lbParameter     = 1;
        lbPrintEODTicket= 0;
        lbLists         = 1;
        lbSecurity      = 1;
        
        kbflush();
        lbRet = MerchantMenu(lbDuplicateTran,lbEndOfDay,lbParameter,lbPrintEODTicket,lbLists,lbSecurity);
        HDGRead(&lsHDGBuf);    //add by Allen 20151123
        switch(lbRet)
        {
        case SECOND_COPY:
			if (1 == lsHDGBuf.bLastPrint)          
			{
				PrintTransList(OS_TRUE, OS_FALSE, FILE_LAST_EOD);    
				PaperControl();
				Print_PAX_Slip(OS_TRUE,FILE_RB);
			}
			else 
			{
				Print_PAX_Slip(OS_TRUE, FILE_R1);
			}
            break;
        case BATCH_CLOSE:
            StartBatchClose();
            break;
        case PARAMETER:
            ParameterDownload(OS_FALSE, OS_FALSE, OS_TRUE);
            break;
        case BATCH_SLIP_COPY:
            Print_PAX_Slip(OS_FALSE, FILE_RB);
            break;
        case LISTS:
            ListsMenu();
            break;
        case SECURITY_FUNCT:
            SecurityFunctions();
            break;
        case CANCEL:
            lwStatus=STAT_OK;
            break;            
        }
        memset(&gPTStruct, 0, LG_PTSTRUCT);
    }
}

void M_ApplicationCheck(void)
{
    WORD Status = STAT_OK;
	//by jarod check Security Menu PW
	if(Check_SecurityMenuPW()!=0) return;

    Status = CheckAllFiles();

    if (!(Status == STAT_OK))
    {
        if ( !gState.ApplStateInit)
        {
            ApplicationActive(OS_FALSE);
        }
        else
        {
            if (gbFLagPartialInitState )
            {
                ApplicationActive(OS_TRUE);
            }
        }
    }

	if ( Status == STAT_OK )
	{
		if ( (gState.ApplStateInit == OS_TRUE))
		{
			OS_ClearScreen();
			if (KeyBoardQuestion("APPL ACTIVATE", "ALREADY INIT", "ACTIVATE AGAIN ?", 30)!=KEYENTER)
			{
				return;
			}
		}
        
        OS_ClearScreen();
        Status = HostInfoEntry();
        if ( Status != STAT_OK)
        {
            return;
        }

		Status = ParameterDownload(OS_FALSE, OS_FALSE, OS_TRUE);	// Parameter download with GP
		if (Status == STAT_OK)
		{
			gState.ApplStateInit  = OS_TRUE;
		}
		else
		{
			gState.ApplStateInit  = OS_FALSE;
		}
	}

    SetTermFlags();    
}

void ApplicationActive( BYTE PartialInitialized)
{
    WORD lbStatusWork;
    BYTE Continu;
    APPLCONFIGSTRUCT ApplConfig;

    lbStatusWork = STAT_OK;
    GetApplicationConfig(&ApplConfig);

    do
    {
        Continu = OS_NO;
        OS_ClearScreen();
        ScrPrint(0, 0, CFONT|REVER, "APPL ACTIVATE   ");
        if (KeyBoardQuestion(NULL, ApplConfig.ApplTitle, DSP_ACTIF, 30)==KEYENTER)
        {
            if ( !PartialInitialized )
            {
                lbStatusWork = HostInfoEntry();
            }
            if ( lbStatusWork == STAT_OK)
            {
				if (ParameterDownload(OS_TRUE,OS_FALSE,OS_TRUE)==STAT_OK)
                {
                    gState.ApplStateInit = OS_YES;
                }
                else
                {
                    InitFiles();
                }
            }
            if (!gState.ApplStateInit)
            {
                Continu = OS_YES;
            }
        }
        else
        {
            gState.ApplStateInit = OS_NO;
        }
    }while (Continu);
}


