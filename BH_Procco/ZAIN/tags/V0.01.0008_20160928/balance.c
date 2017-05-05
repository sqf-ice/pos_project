#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_define.h"
#include "os_interface.h"

#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "utility.h"

extern void FormatAmt(char *pDest, char *pBuf, char *CurDes, BYTE InputType);
extern void FormatPoints(char *pDest, char *pRes);  //added by Allen 2016-07-12
extern void GetTotalBalance(char *pPointsBlance);   //added by Allen 2016-07-13

WORD BalanceList(void)
{
    BYTE lpLine[27];
    BYTE Balance[20];
    BYTE lbFormatedBalance[12+8];   //modified by Allen 2016-06-07 from LG_AMOUNT  to 12
    BYTE Key;
    WORD Len=0;
    WORD Count=0;
    
	memset(lpLine,0,sizeof(lpLine));
	memset(Balance, 0, sizeof(Balance));
	memcpy(Balance, gPTStruct.Balance, 12);
	//DebugComSend("BalanceList : Balance:%s \n", Balance); 
    Balance[12]=0;
	FormatAmt(lpLine, Balance, gCurrencySymbol, ASCII_IN);	

	OS_ClearScreen();
// 	OS_DisplayStr(0, 0, PDS_BALANCE);
// 	OS_DisplayStr(2, 0, lpLine);
    ScrPrint(0, 0, CFONT|REVER, "%-16.16s", PDS_BALANCE);
    ScrPrint(0, 2, CFONT,       "%16.16s", lpLine);
	if (StrLen(lpLine)>16)
	{
		ScrPrint(0, 4, CFONT, "%16.16s", lpLine+16);
	}

	memset(lbFormatedBalance, 0, sizeof(lbFormatedBalance));
	memcpy(lbFormatedBalance,lpLine,16);
	
    // Gary
    ScrPrint(0, 4, CFONT, "%16.16s", DSP_K_CANCEL_KEY); // next to ATM3
    ScrPrint(0, 6, CFONT, "%16.16s", DSP_K_PRINT_KEY);  // next to ATM4

    while (1)
    {
        Key = 0;
        OS_KeybdGet(&Key,  60);
        PrnInit();
        switch (Key)
        {
        case KEYENTER:
        case KEYATM4:
        case KEYF4:
            Print_Balance_Slip(lbFormatedBalance);
            return STAT_OK;
        default:
            return STAT_NOK;
        }
    }
    return STAT_NOK;
}

// end of file

