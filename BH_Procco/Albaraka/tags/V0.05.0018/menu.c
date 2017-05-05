#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_define.h"
#include "os_interface.h"
#include "utility.h"
#include "defines.h"
#include "struct.h"
#include "message.h"
#include "function.h"
#include "vxlibs.h"
#include "appLib.h"


extern void FormatAmt(char *pDest, char *pBuf, char *CurDes, BYTE InputType);
extern BYTE AskYesNo(MSG_Struct *stMSG,BYTE Type,BYTE offset);


WORD AmountEntry(BYTE *pBCDAmt)
{
    BYTE    chTemp[32];

    Utils_ConvertBcdToAsc(pBCDAmt, gPTStruct.EntryAmount, 6);
    EMVSetTLVData(0x9F02, pBCDAmt, 6);

	sprintf((char *)chTemp, "%.12s", gPTStruct.EntryAmount);
	PubLong2Char((ulong)atol((char *)chTemp), 4, chTemp);
	EMVSetTLVData(0x81, chTemp, 4);

	return (STAT_OK);
}

WORD TerminalType(void)
{
    BYTE lbMenuLine;
    BYTE FirstItem;
    BYTE OldItem;

	FirstItem = 1;
	if (gbTermFlags&TERMINALINLAN)
		OldItem = 2;
	else
		OldItem = 1;

	lbMenuLine=KeyBoardMenu(3,gTermMenu,&FirstItem,OldItem,200,0);
	switch (lbMenuLine)
	{
		case 1 :
			gbTermFlags&=(~TERMINALINLAN); // reset the bit for single terminal
			gbTermFlags|=TERMINALINGPRS; 
			break;
		case 2 :
			gbTermFlags&=(~TERMINALINLAN); // reset the bit for single terminal
			break;
		case 3 :
			gbTermFlags|=TERMINALINLAN;    // set the bit for network terminal
			break;
		default :
			return(STAT_NOK);
	}
	SetTermFlags();
	return (STAT_OK);
}

WORD TransMenu(BYTE fbBalance,BYTE fbLastTrans,BYTE fbInstallment,BYTE fbLoyalty,BYTE fbPayment,BYTE fbAccountMaint)
{
    BYTE lbMenuLine;
    BYTE FirstItem;
    WORD lwStatus;		// status to return

    BYTE lbTenure;
    BYTE lbBalance;
    BYTE lbLastTrans;
    BYTE lbInstallment;
    BYTE lbLoyalty;
    BYTE lbPayment;
    BYTE lbAccountMaint;
    BYTE lbCounter;		// Number of items in the menu
    BYTE lbBufCounter;	// Number of items in the menu
    BYTE *lpBuf[11];

	// Initialize
	lbTenure		=99;
	lbBalance     =99;
	lbLastTrans   =99;
	lbInstallment =99;
	lbLoyalty     =99;
	lbPayment     =99;
	lbAccountMaint=99;

	// Header of menu
	lbCounter = 0;
	lbBufCounter = 0;
	lpBuf[lbCounter] = DSP_TRANSACTIONS;
	lbBufCounter=12;

	if (fbInstallment)
	{
		lbInstallment = ++lbCounter;
		lpBuf[lbCounter] = DSP_INSTALLMENT;
		lbBufCounter+=12;
	}
	/*if (fbLoyalty)
	{
		lbLoyalty = ++lbCounter;
		lpBuf[lbCounter] = DSP_LOYALTY;
		lbBufCounter+=12;
	}
	if (fbAccountMaint)
	{
		lbAccountMaint = ++lbCounter;
		lpBuf[lbCounter] = DSP_ACCOUNT_MAINT;
		lbBufCounter+=12;
	}*/
	if (fbPayment)
	{
		lbPayment = ++lbCounter;
		lpBuf[lbCounter] = DSP_PAYMENT;
		lbBufCounter+=12;
	}
	if (fbBalance)
	{
		lbBalance = ++lbCounter;
		lpBuf[lbCounter] = DSP_BALANCELIST;
		lbBufCounter+=12;
	}
	/*if (fbLastTrans)
	{
		lbLastTrans = ++lbCounter;
		lpBuf[lbCounter] = DSP_LASTTRANS;
		lbBufCounter+=12;
	}*/

	//Tenure calculation
	lbTenure = ++lbCounter;
	lpBuf[lbCounter] = DSP_TENURE_CALC;
	lbBufCounter += 12;

	
	FirstItem = 1;
	lbMenuLine = KeyBoardMenu(lbCounter,(const BYTE **)lpBuf,&FirstItem,gOldItem,TO_1MIN,0);

	gOldItem=lbMenuLine;
	// Call menu and determine the selected item

	if (lbMenuLine==lbBalance)
	{
		lwStatus = BALANCE_LIST;
	}
	else if (lbMenuLine==lbLastTrans)
	{
		lwStatus = LAST10TRANSACTION;
	}
	else if (lbMenuLine==lbInstallment)
	{
		lwStatus = INSTALLMENT_PLAN;
	}
	else if (lbMenuLine==lbLoyalty)
	{
		lwStatus = LOYALTY;
	}
	else if (lbMenuLine==lbPayment)
	{
		lwStatus = PAYMENT;
	}
	else if (lbMenuLine==lbAccountMaint)
	{
		lwStatus = ACCOUNT_MAINT;
	}
	else if (lbMenuLine == lbTenure)
	{
		lwStatus = TENURE_CALCULATION;
	}
	else
	{
		lwStatus = CANCEL;
	}
	return(lwStatus);
}

WORD MerchantMenu(BYTE fbDuplicateTran,BYTE fbEndOfDay,BYTE fbParametre,BYTE fbPrintEODTicket,BYTE fbListe,BYTE fbSecurity)
{
    BYTE lbMenuLine;
    BYTE FirstItem;
    WORD lwStatus;		// status to return

    BYTE lbDuplicateTran;
    BYTE lbEndOfDay;
    BYTE lbParameter;
    BYTE lbPrintEODTicket;
    BYTE lbLists;
    BYTE lbSecurity;
    BYTE lbCounter;		// Number of items in the menu
    BYTE *lpBuf[10];

	gLanguage=ENGLISH;
	// Initialize
	lbDuplicateTran=99;
	lbEndOfDay=99;
	lbParameter=99;
	lbPrintEODTicket=99;
	lbLists=99;
	lbSecurity=99;

	// Header of menu
	lbCounter = 0;
	lpBuf[lbCounter] = DSP_TRANSACTION;

	if (fbEndOfDay)
	{
		lbEndOfDay = ++lbCounter;
		lpBuf[lbCounter] = DSP_ENDOFDAY;
	}

	if (fbParametre)
	{
		lbParameter = ++lbCounter;
		lpBuf[lbCounter] = DSP_PARAMETER;
	}
	if (fbPrintEODTicket)
	{
		lbPrintEODTicket = ++lbCounter;
		lpBuf[lbCounter] = DSP_PRINTEODTICKET;
	}
	if (fbListe)
	{
		lbLists = ++lbCounter;
		lpBuf[lbCounter] = DSP_LISTS;
	}
	if (fbDuplicateTran)
	{
		lbDuplicateTran = ++lbCounter;
		lpBuf[lbCounter] = DSP_DUPLICATETRAN;
	}
	if (fbSecurity)
	{
		lbSecurity = ++lbCounter;
		lpBuf[lbCounter] = DSP_SECURITY;
	}

	FirstItem = 1;
	lbMenuLine=KeyBoardMenu(lbCounter,(const BYTE **)lpBuf,&FirstItem,gOldItem,200,0);
	gOldItem=lbMenuLine;
	// Call menu and determine the selected item
	if (lbMenuLine==lbDuplicateTran)
	{
		lwStatus = SECOND_COPY;
	}
	else if (lbMenuLine==lbEndOfDay)
	{
		lwStatus = BATCH_CLOSE;
	}
	else if (lbMenuLine==lbParameter)
	{
		lwStatus = PARAMETER;
	}
	else if (lbMenuLine==lbPrintEODTicket)
	{
		lwStatus = BATCH_SLIP_COPY;
	}
	else if (lbMenuLine==lbLists)
	{
		lwStatus = LISTS;
	}
	else if (lbMenuLine==lbSecurity)
	{
		lwStatus = SECURITY_FUNCT;
	}
	else
	{
		lwStatus = CANCEL;
	}
	return(lwStatus);
}

WORD ListsMenu(void)
{
    BYTE lbMenuLine;
    BYTE FirstItem;
    WORD lwStatus = STAT_NOK;

	FirstItem = 1;
	gOldSubItem=1;
	while(lwStatus!=STAT_OK)
    {
        lbMenuLine=KeyBoardMenu(3,gListeMenu,&FirstItem,gOldSubItem,200,0);
        gOldSubItem=lbMenuLine;
        switch (lbMenuLine)
        {
            case 1 :
                PrintTransList(OS_TRUE,OS_TRUE, FILE_TR);
                break;
            case 2 :
				PrintTransList(OS_FALSE,OS_TRUE, FILE_TR);
				break;
			case 3 :
				ParameterList();
				break;
			default :
				lwStatus = STAT_OK;
				break;
        }
	}
	return (STAT_OK);
}


WORD DebugMenu(void)
{
    BYTE lbMenuLine;
    BYTE FirstItem;
    BYTE OldItem;

	FirstItem = 1;
	if (gDebug==OS_FALSE)
		OldItem = 2;
	else
		OldItem = 1;

	lbMenuLine=KeyBoardMenu(2,gDebugMenu,&FirstItem,OldItem,200,0);
	
	switch (lbMenuLine)
	{
		case 1 :
			gDebug=OS_TRUE; // reset the bit for single terminal
			gDebugType=DEBUG_PRINTER;
			break;
		case 2 :
			gDebug=OS_FALSE;    // set the bit for network terminal
			break;
		default :
			return(STAT_NOK);
	}
	SetTermFlags();
	return (STAT_OK);
}

WORD DebugType(void)
{
	BYTE lbMenuLine;
	BYTE FirstItem;
	BYTE OldItem;

	FirstItem = 1;
	if (gDebug==OS_FALSE)
		OldItem = 2;
	else
		OldItem = 1;

	lbMenuLine=KeyBoardMenu(3,gDebugMenuType,&FirstItem,OldItem,200,0);

	switch (lbMenuLine)
	{
	case 1 :
		gDebugType=DEBUG_PORT; // reset the bit for single terminal
		break;
	case 2 :
		gDebugType=DEBUG_PRINTER;    // set the bit for network terminal
		break;
	case 3:
		gDebugType=DEBUG_PORT_AND_PRINTER;    // set the bit for network terminal
		break;
	default :
		gDebugType = DEBUG_PRINTER;
		return(STAT_NOK);
	}
	SetTermFlags();
	return (STAT_OK);
}


WORD ProductMenu(void)
{
    int lbHandler;
    BYTE *lpBuf[20];
    BYTE MENU[20][16];
    BYTE MENUArabic[20][16];
    BYTE PRODCODE[20][2];
    BYTE FirstItem;
    BYTE lbMenuLine;
    BYTE lbCounter=0;
    BYTE Buffer[15];
    BYTE m;
    ProductStruct lStruct;

	while(OS_TRUE)
	{
		lbHandler = Open( FILE_PC, O_RDWR);
		if (lbHandler >= 0)
		{
			// Header of menu
			lbCounter = 0;
			memcpy(MENU[lbCounter],DSP_PRODUCT_TYPE,16);
			memcpy(MENUArabic[lbCounter],DSP_PRODUCT_TYPE,16);
			while(OS_TRUE)
			{
				if ( Read( lbHandler, (BYTE *)&lStruct, LG_PRODUCTSTRUCT) == 0 )
				{
					break;
				}
				memset(Buffer,' ',13);
				memcpy(Buffer,lStruct.ProductDescription,12);
				Buffer[12]=' ';
				Buffer[13]=0;
				++lbCounter;
				if(lbCounter>20)
				{
					DisplayString(DSP_PRODUCT_UNIT,3000);
					Close(lbHandler);
					return(STAT_NOK);
				}
				StrCpy(MENU[lbCounter],Buffer);
				memset(Buffer,' ',13);
				memcpy(Buffer,lStruct.ProductDescriptionArabic,12);
				Buffer[12]=' ';
				Buffer[13]=0;
				StrCpy(MENUArabic[lbCounter],Buffer);
				memcpy(PRODCODE[lbCounter],lStruct.ProductCode,2);
			}
		}
		else
		{
			return(STAT_NOK);
		}
		if(lbCounter>=1)
		{
			for(m=0;m<=lbCounter;m++)
				lpBuf[m] = MENU[m];

			FirstItem = 1;
			lbMenuLine=KeyBoardMenu(lbCounter,(const BYTE **)lpBuf,&FirstItem,1,200,0);
			if ((lbMenuLine>=1)&&(lbMenuLine<=20))
			{
				memcpy(gPTStruct.ProductCode, PRODCODE[lbMenuLine], 2);
				memcpy(gPTStruct.ProductDescription, MENU[lbMenuLine], 12);
				memcpy(gPTStruct.ProductDescriptionArabic, MENUArabic[lbMenuLine], 12);
				Close(lbHandler);
				return(STAT_OK);
			}
			else
			{
				Close(lbHandler);
				return(STAT_NOK);
			}
		}
		//Tek Tip Urun varsa menu goruntulenmeden gec
		else
		{
			Close(lbHandler);
			return(STAT_NOK);
		}
	}
}


WORD SchemeMenu(void)
{
    int lbHandler;
//    BYTE *lpBuf[20];
    BYTE MENU[20][16];
    BYTE MENUArabic[20][16];
    BYTE PRODCODE[20][2];
//    BYTE FirstItem;
    BYTE lbMenuLine;
    BYTE lbCounter=0;
    BYTE Buffer[15];
	BYTE buff[128];
	BYTE Min[12],Max[12];

	int i=0;
//    BYTE m;
	//by jarod 2011.06.17
    SchemeStruct lStruct[3];
    unsigned long   ulAmt;

	memset(&MENU,0,sizeof(MENU));
	memset(&MENUArabic,0,sizeof(MENUArabic));	// Gary add 2011-1-26
	while(OS_TRUE)
	{
		lbHandler = Open(FILE_ST, O_RDWR);
		if (lbHandler >= 0)
		{
			// Header of menu
			lbCounter = 0;
			memcpy(MENU[lbCounter],      DSP_SCHEME_TYPE,15);
			memcpy(MENUArabic[lbCounter],DSP_SCHEME_TYPE,16);
			while(OS_TRUE)
			{
//by jarod 2011.06.17
//New Calculation
				if ( Read( lbHandler, (BYTE *)&lStruct, LG_SCHEMESTRUCT*3) == 0 )
				{
					break;
				}
				memset(Buffer, 0, sizeof(Buffer));
				memset(Buffer,' ',13);
				for(i=0;i<3;i++)
				{
					PubBcd2Asc(lStruct[i].SchemeMin,5,Min);
					PubBcd2Asc(lStruct[i].SchemeMax,5,Max);

 					if(memcmp(gPTStruct.EntryAmount+2,Min,10)>=0 &&
 						memcmp(gPTStruct.EntryAmount+2,Max,10)<=0)
					{
						memcpy(Buffer,lStruct[i].SchemeDescription,12);
						Buffer[12]=' ';
						Buffer[13]=0;
						++lbCounter;
						if(lbCounter>20)
						{
							DisplayString(DSP_SCHEME_UNIT,3000);
							Close(lbHandler);
							return(STAT_NOK);
						}
						StrCpy(MENU[lbCounter],Buffer);

						memset(Buffer,' ',13);
						memcpy(Buffer,lStruct[i].SchemeDescriptionArabic,12);
						Buffer[12]=' ';
						Buffer[13]=0;
						StrCpy(MENUArabic[lbCounter],Buffer);
						memcpy(PRODCODE[lbCounter],lStruct[i].SchemeProfit,2);
					}				
				}
				break;
			}
		}
		else
		{
			Close(lbHandler);
			return(STAT_NOK);
		}
		Close(lbHandler);

        if (lbCounter<1)
        {
            return STAT_NOK;
        }

        sprintf((char *)Buffer, "%.12s", gPTStruct.EntryAmount);
        ulAmt = atol(Buffer+2);
        if (ulAmt<=1500*1000)
        {
            lbMenuLine = 1;
        }
        else if (ulAmt<=3000*1000)
        {
            lbMenuLine = 2;
        }
        else
        {
            lbMenuLine = 3;
        }

        if (lbMenuLine>lbCounter)
        {
            lbMenuLine =lbCounter;
        }
		memcpy(gPTStruct.SchemeProfit, PRODCODE[lbMenuLine], 2);
		memcpy(gPTStruct.SchemeDescription, MENU[lbMenuLine], 12);
		memcpy(gPTStruct.SchemeDescriptionArabic, MENUArabic[lbMenuLine], 12);
		return(STAT_OK);

        /*
		if(lbCounter>1)
		{
			for(m=0;m<=lbCounter;m++)
            {
				lpBuf[m] = MENU[m];
            }

			FirstItem = 1;
			lbMenuLine=KeyBoardMenu(lbCounter,(const BYTE **)lpBuf,&FirstItem,1,200,0);
			if ((lbMenuLine>=1)&&(lbMenuLine<=20))
			{
				memcpy(gPTStruct.SchemeProfit, PRODCODE[lbMenuLine], 2);
				memcpy(gPTStruct.SchemeDescription, MENU[lbMenuLine], 12);
				memcpy(gPTStruct.SchemeDescriptionArabic, MENUArabic[lbMenuLine], 12);
				return(STAT_OK);
			}
			else
			{
				return(STAT_NOK);
			}
		}
		//Tek Tip Bilet varsa menu g”rntlenmeden gec
		else
		{
			memcpy(gPTStruct.SchemeProfit, PRODCODE[1], 2);
			memcpy(gPTStruct.SchemeDescription, MENU[1], 12);
			memcpy(gPTStruct.SchemeDescriptionArabic, MENUArabic[1], 12);
			return(STAT_OK);
		}
        */
	}
}

WORD ReadAvailableInstallment(void)
{
    int lbHandler;
    BYTE Buffer[15];
	BYTE lpLine[40];
    AvailableInstallmentStruct lStruct;

	while(OS_TRUE)
	{
		lbHandler = filesize(FILE_PE);
		lbHandler = Open( FILE_PE, O_RDWR);
		if (lbHandler >= 0)
		{
			while(OS_TRUE)
			{
				if ( Read( lbHandler, (BYTE *)&lStruct, LG_AVAILABLEINSTALLMENTSTRUCT) == 0 )
				{
//					DisplayString((PBYTE)_T("INCORRECT AMOUNT"),3000);
					Close(lbHandler);
					return(STAT_NOK);
				}
				memset(Buffer, 0, sizeof(Buffer));
				
				// Gary ???? is it a bug ?
				//StrBcdToAsc(10,lStruct.StartAmount,Buffer);
				Buffer[0] = '0';
				Buffer[1] = '0';
				StrBcdToAsc(5,lStruct.StartAmount,Buffer+2);

				if(memcmp(gPTStruct.EntryAmount,Buffer,10)>=0)
				{
					// Gary ???? is it a bug ?
					//memset(Buffer,0,12);
					//StrBcdToAsc(10,lStruct.FinishAmount,Buffer);
					memset(Buffer, 0, sizeof(Buffer));
					StrBcdToAsc(5,lStruct.FinishAmount,Buffer);
					if(memcmp(gPTStruct.EntryAmount+2,Buffer,10)<=0)
					{
						gPTStruct.AvailableInstallmentFlag1=lStruct.AvailableInstallmentFlag1;
						gPTStruct.AvailableInstallmentFlag2=lStruct.AvailableInstallmentFlag2;
						if((gPTStruct.AvailableInstallmentFlag1&1) ||
							(gPTStruct.AvailableInstallmentFlag1&2) ||
							(gPTStruct.AvailableInstallmentFlag1&4) ||
							(gPTStruct.AvailableInstallmentFlag1&8) ||
							(gPTStruct.AvailableInstallmentFlag1&16)||
							(gPTStruct.AvailableInstallmentFlag1&32)||
							(gPTStruct.AvailableInstallmentFlag2&1) ||
							(gPTStruct.AvailableInstallmentFlag2&2) ||
							(gPTStruct.AvailableInstallmentFlag2&4) ||
							(gPTStruct.AvailableInstallmentFlag2&8) ||
							(gPTStruct.AvailableInstallmentFlag2&16)||
							(gPTStruct.AvailableInstallmentFlag2&32))
						{
							Close(lbHandler);
							return(STAT_OK);
						}
						else
						{
							DisplayString(DSP_AMOUNT_MIN_LIMIT,3000);
							Close(lbHandler);
							return(STAT_INV);
						}
					}
				}
				else
				{
					memset(lpLine,0,sizeof(lpLine));
					Buffer[12]=0;
					FormatAmt(lpLine, Buffer, gCurrencySymbol, ASCII_IN);
					ScrCls();
					ScrPrint(0, 0, CFONT, DSP_AMOUNT_MIN_LIMIT_ERR);
					if (StrLen(lpLine)>16)
					{
						ScrPrint(0, 6, CFONT, "%16.16s", lpLine);
					} 
					else
					{
						ScrPrint(0, 7, ASCII, "%21.21s", lpLine);
					}
					DelayMs(10000);

					Close(lbHandler);
					return(STAT_INV);
				}
			}
		}
		else
// 		{
// 			DisplayString((PBYTE)_T("INCORRECT AMOUNT"),3000);
// 			return(STAT_INV);
// 		}
			//by jarod 2011.07.04
			return(STAT_INV);
	}
}

WORD TransactionTypeMenu(void)
{
    BYTE lbRet;       			// Return of menu
    BYTE lbInstallment;
    BYTE lbLoyalty;
    BYTE lbPayment;
    BYTE lbAccountMaint;
    BYTE lbBalance;
    BYTE lbLastTrans;

	if(memcmp(gCardParam.UpperLimit,"\x0\x0\x0\x0\x0\x0",6)==0)
    {
		lbInstallment =  0;
    }
	else
	{
		lbInstallment   =  (gMerchantOperationFlags& 0x1) > 0;
		lbInstallment   &= (gPTStruct.CardBinFlags & 0x1) > 0;
		lbInstallment   &= (gCardParam.InstallmentFlag==1);
	}

	lbLoyalty       =  (gMerchantOperationFlags& 0x2) > 0;
	lbLoyalty       &= (gPTStruct.CardBinFlags & 0x2) > 0;
	lbLoyalty       &= (gCardParam.LoyaltyFlag==1);

	lbPayment       =  (gMerchantOperationFlags& 0x4) > 0;
	lbPayment       &= (gPTStruct.CardBinFlags & 0x4) > 0;

	lbAccountMaint	 =  (gMerchantOperationFlags& 0x8) > 0;
	lbAccountMaint	 &= (gPTStruct.CardBinFlags & 0x8) > 0;

	lbBalance       = 1;
	lbLastTrans     = 0;
    //!!!sel??? sonra at	
    lbInstallment=lbLoyalty=lbPayment=lbAccountMaint=1;
/*	if((lbInstallment==0)&&(lbLoyalty==0)&&(lbPayment==0)&&(lbAccountMaint==0))
	{
		DisplayString(DSP_TERMINAL_NOT_ALLOWED, 2000);
		return(STAT_NOK);
	}
	else*/
    if((lbInstallment==1)&&(lbLoyalty==0)&&(lbPayment==0)&&(lbAccountMaint==0))
	{
		memcpy(gPTStruct.TransType,TYPE_70,2);
		return(STAT_OK);
	}
	else if((lbInstallment==0)&&(lbLoyalty==1)&&(lbPayment==0)&&(lbAccountMaint==0))
	{
		memcpy(gPTStruct.TransType,TYPE_71,2);
		return(STAT_OK);
	}
	else if((lbInstallment==0)&&(lbLoyalty==0)&&(lbPayment==1)&&(lbAccountMaint==0))
	{
		memcpy(gPTStruct.TransType,TYPE_72,2);
		return(STAT_OK);
	}
	else if((lbInstallment==0)&&(lbLoyalty==0)&&(lbPayment==0)&&(lbAccountMaint==1))
	{
		memcpy(gPTStruct.TransType,TYPE_73,2);
		return(STAT_OK);
	}

	gOldItem=1;
	bBalanceFromOptions = TRUE; //// Added by Kim_LinHB 2013-3-8 build 0.05.0009 
	lbRet = TransMenu(lbBalance,lbLastTrans,lbInstallment,lbLoyalty,lbPayment,lbAccountMaint);
	switch(lbRet)
	{
		case INSTALLMENT_PLAN:
			memcpy(gPTStruct.TransType,TYPE_70,2);
			bBalanceFromOptions = FALSE; //// Added by Kim_LinHB 2013-3-8 build 0.05.0009 
			return(STAT_OK);
		case LOYALTY:
			memcpy(gPTStruct.TransType,TYPE_71,2);
			return(STAT_OK);
		case PAYMENT:
			memcpy(gPTStruct.TransType,TYPE_72,2);
			return(STAT_OK);
		case ACCOUNT_MAINT:
			memcpy(gPTStruct.TransType,TYPE_73,2);
			return(STAT_OK);
		case BALANCE_LIST :
			memcpy(gPTStruct.TransType,TYPE_74,2);
			return(STAT_OK);
// 			BalanceList();
// 			return(STAT_NOK);
		case LAST10TRANSACTION:
//			Last10Transaction();
			return(STAT_NOK);
		case TENURE_CALCULATION:
			bBalanceFromOptions = FALSE; //// Added by Kim_LinHB 2013-3-20 build 0.05.0010
			memcpy(gPTStruct.TransType,TYPE_75,2);
			return(STAT_OK);
		case CANCEL:
			return(STAT_NOK);
	}

	return(STAT_NOK);
}


WORD Calculate_DisplayMenu(void)
{
#define NUM_OPTIONS     16 // Modified by Kim_LinHB 2013-3-20 build 0.05.0010
    BYTE Buffer[LG_CSTMSG];
    BYTE Amount[15];
    BYTE RateAmount[15];
    BYTE Bcd1[15];
    BYTE Bcd2[LG_CSTMSG],remain[7];
    BYTE BcdProfit[15];
    BYTE MonhthlyInstAmount[NUM_OPTIONS][LG_AMOUNT+1];
    BYTE TotalInstallmentAmount [NUM_OPTIONS][LG_AMOUNT+1];
    BYTE TotalProfitAmount[NUM_OPTIONS][LG_AMOUNT+1];
    BYTE TotalInstallmentCount[NUM_OPTIONS+1]; // Modified by Kim_LinHB 2013-10-11 0.05.0013 from LG_AMOUNT to NUM_OPTIONS
    BYTE RateValue=0;
    unsigned long Year=0;
    BYTE i=0;
	BYTE j=0;
    BYTE old=0;
    BYTE MENU[20][22];
    BYTE lbCounter=0;
    BYTE *lpBuf[20];
    BYTE FirstItem,m;
    BYTE lbMenuLine;
    MSG_Struct MainMSG;
    BYTE bRet;

	BYTE profit[6];

	memset(MonhthlyInstAmount, 0, sizeof(MonhthlyInstAmount));
	for (i=0; i<NUM_OPTIONS; i++)
	{
		memset(MonhthlyInstAmount[i],'0',LG_AMOUNT);
		TotalInstallmentCount[i]=0;
	}

	//Total Amount
	memset(Buffer, 0, sizeof(Buffer));
	memcpy(Buffer, gPTStruct.EntryAmount, LG_AMOUNT);
	Buffer[LG_AMOUNT] = 0;
	StrTrim(Buffer,Buffer,' ');
	StrLeftPad(Buffer, LG_AMOUNT, Buffer, '0');
	Buffer[LG_AMOUNT]=0;
	//StrAscToBcd(Buffer,Amount);
	memset(Amount, 0, sizeof(Amount));
	mCmmnUtil_AsciiToBcd(Buffer, Amount, LG_AMOUNT);
	
	//Mult.
	memset(profit, 0, sizeof(profit));	
	memcpy(profit+4, gPTStruct.SchemeProfit, 2);
	
	memset(Bcd1, 0, sizeof(Bcd1));
	MulAsciiBCD(Bcd1, profit, Amount, 6, BCD_IN);


	//Div.10000
	memset(profit, 0, sizeof(profit));	
	memcpy(profit+3, "\x01\x00\x00", 3);
	DivAsciiBCD(Bcd2, Bcd1, profit, 6, BCD_IN, remain);
	//DivBcdNum (Bcd1 , 10000 , Bcd2      , 7);
	memcpy(RateAmount,Bcd2,6);
	for (i=0; i<NUM_OPTIONS; i++)
	{
		// Hided by Kim_LinHB 2013-3-19 build 0.05.0009
// 		RateValue=0;
// 		if((gPTStruct.AvailableInstallmentFlag1&1)&& (i== 0))
// 			RateValue = 1;
// 		else if((gPTStruct.AvailableInstallmentFlag1&2)&& (i== 1))
// 			RateValue = 3;
// 		else if((gPTStruct.AvailableInstallmentFlag1&4)&& (i== 2))
// 			RateValue = 6;
// 		else if((gPTStruct.AvailableInstallmentFlag1&8)&& (i== 3))
// 			RateValue = 12;
// 		else if((gPTStruct.AvailableInstallmentFlag1&16)&&(i== 4))
// 			RateValue = 18;
// 		else if((gPTStruct.AvailableInstallmentFlag1&32)&&(i== 5))
// 			RateValue = 24;
// 		else if((gPTStruct.AvailableInstallmentFlag2&1)&& (i== 6))
// 			RateValue = 30;
// 		else if((gPTStruct.AvailableInstallmentFlag2&2)&& (i== 7))
// 			RateValue = 36;
// 		else if((gPTStruct.AvailableInstallmentFlag2&4)&& (i== 8))
// 			RateValue = 42;
// 		else if((gPTStruct.AvailableInstallmentFlag2&8)&& (i== 9))
// 			RateValue = 48;
// 		else if((gPTStruct.AvailableInstallmentFlag2&16)&&(i==10))
// 			RateValue = 54;
// 		else if((gPTStruct.AvailableInstallmentFlag2&32)&&(i==11))
// 			RateValue = 60;
		// Added by Kim_LinHB 2013-3-19 build 0.05.0009
		RateValue=0;
		if((gPTStruct.lTenure & 0x01) && i == 0)
			RateValue = 1;
		else if((gPTStruct.lTenure & 0x02) && i == 1)
			RateValue = 3;
		else if((gPTStruct.lTenure & 0x04) && i == 2)
			RateValue = 6;
		else if((gPTStruct.lTenure & 0x08) && i == 3)
			RateValue = 9;
		else if((gPTStruct.lTenure & 0x10) && i == 4)
			RateValue = 12;
		else if((gPTStruct.lTenure & 0x20) && i == 5)
			RateValue = 15;
		else if((gPTStruct.lTenure & 0x40) && i == 6)
			RateValue = 18;
		else if((gPTStruct.lTenure & 0x80) && i == 7)
			RateValue = 21;
		else if((gPTStruct.lTenure & 0x0100) && i == 8)
			RateValue = 24;
		else if((gPTStruct.lTenure & 0x0200) && i == 9)
			RateValue = 27;
		else if((gPTStruct.lTenure & 0x0400) && i == 10)
			RateValue = 30;
		else if((gPTStruct.lTenure & 0x0800) && i == 11)
			RateValue = 33;
		else if((gPTStruct.lTenure & 0x1000) && i == 12)
			RateValue = 36;
		else if((gPTStruct.lTenure & 0x2000) && i == 13)
			RateValue = 39;
		else if((gPTStruct.lTenure & 0x4000) && i == 14)
			RateValue = 42;
		else if((gPTStruct.lTenure & 0x8000) && i == 15)
			RateValue = 45;
		else if((gPTStruct.lTenure & 0x010000) && i == 16)
			RateValue = 48;
		else if((gPTStruct.lTenure & 0x020000) && i == 17)
			RateValue = 51;
		else if((gPTStruct.lTenure & 0x040000) && i == 18)
			RateValue = 54;
		else if((gPTStruct.lTenure & 0x080000) && i == 19)
			RateValue = 57;
		else if((gPTStruct.lTenure & 0x100000) && i == 20)
			RateValue = 60;
		else if((gPTStruct.lTenure & 0x200000) && i == 21)
			RateValue = 63;
		else if((gPTStruct.lTenure & 0x400000) && i == 22)
			RateValue = 66;
		else if((gPTStruct.lTenure & 0x800000) && i == 23)
			RateValue = 69;
		else if((gPTStruct.lTenure & 0x01000000) && i == 24)
			RateValue = 72;
		else if((gPTStruct.lTenure & 0x02000000) && i == 25)
			RateValue = 75;
		else if((gPTStruct.lTenure & 0x04000000) && i == 26)
			RateValue = 78;
		else if((gPTStruct.lTenure & 0x08000000) && i == 27)
			RateValue = 81;
		else if((gPTStruct.lTenure & 0x10000000) && i == 28)
			RateValue = 84;
		else if((gPTStruct.lTenure & 0x20000000) && i == 29)
			RateValue = 87;
		else if((gPTStruct.lTenure & 0x40000000) && i == 30)
			RateValue = 90;
		else if((gPTStruct.lTenure & 0x80000000) && i == 31)
			RateValue = 93;
		// Add End
		
		if(RateValue>0)
		{
			if(RateValue==1)
			{
				StrBcdToAsc(LG_AMOUNT, Amount, Buffer);
				memcpy(&TotalInstallmentAmount[j],Buffer,LG_AMOUNT);
				memset(BcdProfit,0,6);
			}
			else
			{
				// Added by Kim_LinHB 2013-3-20 build 0.05.0010
				if(gPTStruct.HMinTenure > 0 && gPTStruct.HMaxTenure > 0)
				{
					if(RateValue < gPTStruct.HMinTenure ||
						RateValue > gPTStruct.HMaxTenure)
						continue;
				}
				// Add End

				memset(Bcd1,0,6);
				memset(profit,0,6);
				profit[5]=bytebin2bcd(RateValue);

				MulAsciiBCD(Bcd1, profit, RateAmount, 6, BCD_IN);	
				memset(profit,0,6);
				profit[5]=bytebin2bcd(12);
				DivAsciiBCD(Bcd2, Bcd1, profit, 6, BCD_IN, remain);
			
				memcpy(BcdProfit,Bcd2,6);
				AddAsciiBCD(Bcd1, Amount, Bcd2, 6, BCD_IN);
	
				StrBcdToAsc(LG_AMOUNT, Bcd1, Buffer);//Total Amount with surcharge

				memcpy(&TotalInstallmentAmount[j],Buffer,LG_AMOUNT);

				memset(Bcd2,0,6);
				memset(profit,0,6);
				profit[5]=bytebin2bcd(RateValue);
				DivAsciiBCD(Bcd2, Bcd1, profit, LG_TRAMT, BCD_IN, remain);	
				StrBcdToAsc(LG_AMOUNT, Bcd2, Buffer);		//month Amount with surcharge	

			/* old calc

				memset(Bcd1,0,8);
				AddBcdNum(Amount,Bcd2,Bcd1, 6); //(Amount+Bcd2=Bcd1
				StrBcdToAsc( 14, Bcd1, Buffer);//Total Amount with surcharge
				memcpy(&TotalInstallmentAmount[i],Buffer+2,12);
				Bcd1[7]=0;
				DivBcdNum( Bcd1, RateValue, Bcd2, 8);
				if(Bcd2[7]>=0x50)//Round
					AddBcdNum(Bcd2,(PBYTE)"\x0\x0\x0\x0\x0\x0\x1",Bcd2, 7);
				StrBcdToAsc( 14, Bcd2, Buffer);
				*/
			}

			if(memcmp(Buffer,gPTStruct.Balance,LG_AMOUNT)<=0)
			{
//by jarod  2011.06.09
// 				memcpy(MonhthlyInstAmount[i],Buffer,LG_AMOUNT);
// 				TotalInstallmentCount[i]=RateValue;
// 				memcpy(TotalProfitAmount[i],BcdProfit,6);

				memcpy(MonhthlyInstAmount[j],Buffer,LG_AMOUNT);
				TotalInstallmentCount[j]=RateValue;
				memcpy(TotalProfitAmount[j],BcdProfit,6);
				j++;
			}			
		}
	}

TENURE_MENU:
    memset(&MENU,0,sizeof(MENU));
	lbCounter=0;
	
	strcpy(MENU[lbCounter], _T("SELECT TENURE"));

	for (i=0; i<NUM_OPTIONS; i++)
	{
		if(memcmp(MonhthlyInstAmount[i],UTIL_FILLZEROS,12)!=0)
		{
			++lbCounter;
			memset(Buffer,0,sizeof(Buffer));
			Buffer[0]=(TotalInstallmentCount[i]/10)+'0';
			Buffer[1]=(TotalInstallmentCount[i]%10)+'0';

			memcpy(RateAmount,MonhthlyInstAmount[i],LG_AMOUNT);RateAmount[LG_AMOUNT]=0;
            
			FormatAmt(Bcd2, RateAmount, gCurrencySymbol, ASCII_IN);	
//by jarod 2011.05.27	
			sprintf(Buffer+2,"M-%.15s",Bcd2);
			StrCpy(MENU[lbCounter],Buffer);
		}
	}


	if(lbCounter>=1)
    {
        for(m=0;m<=lbCounter;m++)
        {
            lpBuf[m] = MENU[m];
        }

		FirstItem = 1;

		lbMenuLine = (BYTE)KeyBoardMenu(lbCounter, (const BYTE **)lpBuf, &FirstItem,
                                        1, 200, ((gLanguage==ARABIC)?1:0));
		if ((lbMenuLine>=1)&&(lbMenuLine<=lbCounter))
        {
        		if(memcmp(gPTStruct.TransType, TYPE_75, 2) == 0)
        		{
					ScrCls();
					if (KeyBoardValid((PBYTE)"  ! CAUTION !   ",(PBYTE)" AVAILAEL TENURE", (PBYTE)" OPTIONS", (PBYTE)"CANCEL",0, (PBYTE)"PRINT", KEYB_VAL_MASK|KEYB_CANCEL_MASK, 20) == KEYENTER)
					{
						Print_TenureOption_Slip(lbCounter,(const BYTE **)lpBuf);
					}
					return (STAT_INV);	
        		}
        			
			//delete this : "bugfix : lbMenuLine-1 -->> lbMenuLine"
			//by jarod  2011.06.09
            lbMenuLine-=1;
			memcpy(gPTStruct.MonthlyInstAmount, MonhthlyInstAmount[lbMenuLine],     LG_AMOUNT);
			memcpy(gPTStruct.TotalAmount,       TotalInstallmentAmount[lbMenuLine], LG_AMOUNT);
			gPTStruct.TotalInstallmentCount = TotalInstallmentCount[lbMenuLine];
			memcpy(gPTStruct.ProfitAmountBcd,   TotalProfitAmount[lbMenuLine],      LG_AMOUNT/2);

            Print_SaleDraft_Slip();

			memset(&MainMSG, 0, sizeof(MainMSG));
			ClearCustomMessages(&MainMSG);	
		
			strcpy(MainMSG.CSTMSG1,_T("YOU SELECTED THE"));
			strcpy(MainMSG.CSTMSG2,_T("FOLLOWING TENURE"));
			strcpy(MainMSG.CSTMSG3,_T("OPTION"));
			
			
			memset(Buffer,0,sizeof(Buffer));
			Buffer[0]=(TotalInstallmentCount[lbMenuLine]/10)+'0';
			Buffer[1]=(TotalInstallmentCount[lbMenuLine]%10)+'0';
			memcpy(RateAmount,MonhthlyInstAmount[lbMenuLine],LG_AMOUNT);RateAmount[LG_AMOUNT]=0;
            memset(Bcd2, 0, sizeof(Bcd2));
			FormatAmt(Bcd2, RateAmount, gCurrencySymbol, ASCII_IN);
//by jarod 2011.05.27	
			sprintf(Buffer+2,"M-%.15s",Bcd2);	
			strcpy(MainMSG.CSTMSG5, Buffer);	

			bRet = AskYesNo2(&MainMSG,1,4);
			if(bRet == 1)
			{
				return(STAT_OK); 
			}
			else if(bRet == 2)
			{
				goto TENURE_MENU;
			}
		}
	}
    else
    {
        OS_ClearScreen();
		// Modified by Kim_LinHB 2013-3-20 build 0.05.0010
		ScrPrint(20, 3, CFONT, "Insufficient");
		ScrPrint(40, 5, CFONT, "Balance");
		// Modified End
        GetOneKey(5);
        return STAT_INV;
    }
	
	return(STAT_INV);
}

/*
WORD AreYouSure(void)
{
	return(STAT_OK);
}
*/

WORD ReadMerchantId(void)
{
    int lbHandler,lbStatus;
    BYTE ReadBuffer[25];
    BYTE Buffer[25];

	lbHandler = Open(FILE_MERCH, O_RDWR);
	if (lbHandler >= 0)
	{
		if (Seek (lbHandler, 0, SEEK_SET) < 0 )
		{
			Close(lbHandler);
			return ( STAT_NOK );
		}

		memset(ReadBuffer,0,9);
		lbStatus=Read(lbHandler, ReadBuffer, 3);
		while (lbStatus>0)
		{
			memset(Buffer,0,9);
			gMerchantOperationFlags = ReadBuffer[2];
			ReadBuffer[2]=0;
			StrBcdToAsc(4,ReadBuffer,Buffer);
			if(memcmp(gPTStruct.Pan+6,Buffer+1,3)==0)
			{
				return(STAT_OK);
			}
			lbStatus=Read(lbHandler, ReadBuffer, 3);
		}
	}
#ifdef APPL_TEST
return ( STAT_OK );	
#endif
		
	return(STAT_NOK);
}


