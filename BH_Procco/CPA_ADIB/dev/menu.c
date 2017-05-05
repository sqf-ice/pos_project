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
#include "globlePara.h"


extern void FormatAmt(char *pDest, char *pBuf, char *CurDes, BYTE InputType);
extern BYTE AskYesNo(MSG_Struct *stMSG,BYTE Type,BYTE offset);


WORD AmountEntry(BYTE *pBCDAmt)
{
	BYTE	chTemp[13];
	BYTE    ucBCDAmout[13];
	BYTE	szAmoutBuff[13];
//	BYTE	ucLen;
//	BYTE	ucKey;
//	BYTE	DestAmoutPercent[13];
//	BYTE	decimalBuf[13];
//	BYTE	profitRate[7];

	MSG_Struct MainMSGTmp;
    BYTE	cnt;

	OS_ClearScreen();
    memset(&MainMSGTmp, 0, sizeof(MainMSGTmp));
	memset(ucBCDAmout, 0, sizeof(ucBCDAmout));
	memset(szAmoutBuff, 0, sizeof(szAmoutBuff));

	if (gLanguage == ARABIC)
	{
		strcpy(MainMSGTmp.CSTMSG1, "«·„œ›Ê⁄ ·· «Ã—");
		strcpy(MainMSGTmp.CSTMSG2, "„‰ „ ⁄«„· «· „ÊÌ· ");
		strcpy(MainMSGTmp.CSTMSG3, "«·›Ê—Ì");
	}
	else
	{
		strcpy(MainMSGTmp.CSTMSG1, "ADVANCE PAYMENT");
	}

    cnt = InputAmountExt(&MainMSGTmp, ucBCDAmout, 1, 0, 9, gCurrencySymbol);//BCD
	if (!cnt)
    {
		return(STAT_NOK);
    }
	
	PubAsc2Bcd(gPTStruct.EntryAmount, 12, chTemp);
//	EMVSetTLVData(0x9F02, pBCDAmt, 6);
//	EMVSetTLVData(0x9F02, ucBCDAmout, 6);
	EMVSetTLVData(0x9F02, chTemp, 6);

	//===========chenxi modify to Input EngtryAmount=========
	PubBcd2Asc0(ucBCDAmout, 6, szAmoutBuff);

//	if (!strlen(ucBCDAmout))
//	{
//		strcpy(szAmoutBuff, "000000000000");
//	}
//	else
//	{
//		Utils_ConvertBcdToAsc(ucBCDAmout, szAmoutBuff, 6);
//	}

	//======================================================
	strcpy(gPTStruct.AdvancedAmount, szAmoutBuff);
	PubLong2Char((ulong)atol((char *)szAmoutBuff), 4, chTemp);
	EMVSetTLVData(0x81, chTemp, 4);


// 	memcpy(chTemp, gPTStruct.AdvancedAmount, 12);
// 	chTemp[12] = 0;
// 	memcpy(szAmoutBuff, gPTStruct.EntryAmount, 12);
// 	szAmoutBuff[12] = 0;
// 	memset(DestAmoutPercent, 0, sizeof(DestAmoutPercent));
// 	DivStr(szAmoutBuff, chTemp, DestAmoutPercent, 3);

// 	memset(szDispBuff, 0, sizeof(szDispBuff));
// 	FormatAmt(szDispBuff, gPTStruct.EntryAmount, gCurrencySymbol, ASCII_IN);
// 	PubTrimHeadChars(szDispBuff, ' ');
// 	ucLen = strlen(szDispBuff);
// 	ScrCls();
// 	if (ARABIC == gLanguage)
// 	{
// 		ArShowString(0, SHIFT_CENTER, "≈Ã„«·Ì  ﬂ·›?");
// 		ArShowString(2, SHIFT_CENTER, "«·»÷«∆?");
// 	}
// 	else
// 	{
// 		ScrPrint(0, 0, CFONT, "PRODUCTS TOTAL");
// 		ScrPrint(0, 2, CFONT, "COST PRICE");
// 	}
// 	
// 	if (ucLen>16)
// 	{
// 		ScrPrint((BYTE)(64-ucLen*3), 7, ASCII, "%.21s", szDispBuff);
// 	} 
// 	else
// 	{
// 		ScrPrint((BYTE)(64-ucLen*4), 6, CFONT, "%.16s", szDispBuff);
// 	}

// 	while (1)
// 	{
// 		ucKey = GetOneKey(TO_1MIN);
// 		if (ucKey == KEYCANCEL || ucKey == NOKEY)
// 		{
// 			return STAT_NOK;
// 		}
// 		else if (ucKey == KEYENTER)
// 		{
// 			break;
// 		}
// 	}

	//screen 13

	return (STAT_OK);
}



WORD AmountEntry2(BYTE *pBCDAmt)
{
    BYTE    chTemp[32];
//	BYTE	szDispBuff[32];
//	BYTE	ucLen;
//	BYTE	ucKey;
	
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
	/*
	if (fbLoyalty)
	{
		lbLoyalty = ++lbCounter;
		lpBuf[lbCounter] = DSP_LOYALTY;
		lbBufCounter+=12;
	}*/

	/*
	if (fbPayment)
	{
		lbPayment = ++lbCounter;
		lpBuf[lbCounter] = DSP_PAYMENT;
		lbBufCounter+=12;
	}
	*/
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

	// released by Kim 2014-4-15
	// Modified by Kim_LinHB 2014-5-21  from item #3 to item #4
	if (fbAccountMaint)
	{
		lbAccountMaint = ++lbCounter;
		lpBuf[lbCounter] = DSP_ACCOUNT_MAINT;
		lbBufCounter+=12;
	}

	
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
	BYTE 	gLanguageBak = ENGLISH;

	if (gLanguage == ARABIC)
	{
		gLanguageBak = ARABIC;
		gLanguage=ENGLISH;
	}

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

	if (gLanguageBak == ARABIC)
	{
		gLanguage = ARABIC;
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

WORD GetAgentPIN (void)
{
	BYTE uRet;
	BYTE pszOut[12];
	int  fp;

	OS_ClearScreen();
	ScrPrint(0, 2, CFONT, "SET AGENT PIN");
	ScrPrint(0, 4, CFONT, "INPUT PIN:");
	ScrGotoxy(0, 6);
	memset(pszOut, 0, sizeof(pszOut));
	uRet = GetString(pszOut, 0x65, 4, 4);
	if(uRet == 0)
	{
		if ((fp = open("AGENTPIN", O_RDWR|O_CREATE)) < 0)
		{
			Close(fp);
			return STAT_NOK;
		}
		else
		{
			write(fp, pszOut+1, 4);
			Close(fp);

			OS_ClearScreen();
			ScrPrint((uchar)(64-4*7), 3, CFONT, "SUCCESS");
			DelayMs(2000);
		}
	}

	return STAT_OK;
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
				//memcpy(gPTStruct.ProductDescription, MENU[lbMenuLine], 12);
				memcpy(gPTStruct.ProductDescription, MENU[lbMenuLine], 16);		//for test20130201
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
//	BYTE buff[128];
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
		//Tek Tip Bilet varsa menu gîrÅntÅlenmeden gec
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
	lbRet = TransMenu(lbBalance,lbLastTrans,lbInstallment,lbLoyalty,lbPayment,lbAccountMaint);
	switch(lbRet)
	{
		case INSTALLMENT_PLAN:
			memcpy(gPTStruct.TransType,TYPE_70,2);
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
			memcpy(gPTStruct.TransType,TYPE_75,2);
			return(STAT_OK);
		case CANCEL:
			return(STAT_NOK);
	}

	return(STAT_NOK);
}


WORD Calculate_DisplayMenu(void)
{
#define NUM_OPTIONS     16			//12  for test
//	BYTE temp[6];
    BYTE Buffer[LG_CSTMSG];
	BYTE BufferTmp[LG_CSTMSG];
    BYTE Amount[15];
    BYTE RateAmount[15];
    BYTE Bcd1[15];
    BYTE Bcd2[LG_CSTMSG];
//	BYTE remain[7];
    BYTE BcdProfit[15];
    BYTE MonhthlyInstAmount[NUM_OPTIONS][LG_AMOUNT+1];
    BYTE TotalInstallmentAmount [NUM_OPTIONS][LG_AMOUNT+1];
    BYTE TotalProfitAmount[NUM_OPTIONS][LG_AMOUNT+1];
    BYTE TotalInstallmentCount[NUM_OPTIONS+1];  // Modified by Kim_LinHB 2013-10-11 0.06.0033 from LG_AMOUNT to NUM_OPTIONS
	BYTE AdvancedAmount[LG_AMOUNT + 1];
    BYTE RateValue=0;		//Tenure
    unsigned long Year=0;
    BYTE i=0;
	BYTE j=0;
    BYTE old=0;
    BYTE MENU[20][43];
    BYTE lbCounter=0;
    BYTE *lpBuf[20];
    BYTE FirstItem,m;
    BYTE lbMenuLine;
    MSG_Struct MainMSG;
    BYTE bRet;
	BYTE ucKey;

	BYTE profit[6];
	BYTE pszOut[12];
//	BYTE profitRate[2];
//	BYTE ascRate[6];
//	BYTE yearTmp;
//	BYTE monthTmp;
//	BYTE dateTmp;

	BYTE DivAmout[13], ByDivAmout[13], PercentAmout[13];//, DecimalAmout[13];

	BYTE szDispBuff[32];
	BYTE ucLen;
	DWORD	ulRet;
	int fp;

	memset(TotalProfitAmount, 0, sizeof(TotalProfitAmount));
	memset(TotalInstallmentAmount, 0, sizeof(TotalInstallmentAmount));

	memset(MonhthlyInstAmount, 0, sizeof(MonhthlyInstAmount));
	for (i=0; i<NUM_OPTIONS; i++)
	{
		memset(MonhthlyInstAmount[i],'0',LG_AMOUNT);
		TotalInstallmentCount[i]=0;
	}
#ifdef ADIB_CHANGES
// Modified by Kim_LinHB 2013-3-19 build 0.06.0018 change from hardcode to test macro
#ifdef LJ_TEST
// 	//for test20121212
 	memcpy(gPTStruct.HMinTotal, "000000100000", 12);
 	memcpy(gPTStruct.HMaxTotal, "000002000000", 12);//for test20121212
 	memcpy(gPTStruct.SchemeProfit, "\x12\x00", 2);
 	gPTStruct.HMinTenure = 0;
 	gPTStruct.HMaxTenure = 8;
 	gPTStruct.lTenure = 0x1F;
 	memcpy(gPTStruct.Balance, "000000200000", 12);
#endif

	PubAscSub(gPTStruct.EntryAmount, gPTStruct.AdvancedAmount, 12, AdvancedAmount); // Modified by Kim_LinHB 2013-3-19 build 0.06.0018
	if( memcmp(gPTStruct.EntryAmount, gPTStruct.AdvancedAmount,12) < 0 || // Added by Kim_LinHB 2014-3-13
		memcmp(AdvancedAmount, gPTStruct.HMinTotal,12)<0 ||
		memcmp(gPTStruct.EntryAmount, gPTStruct.HMaxTotal,12)>0) // Modified by Kim_LinHB 2013-3-15 build 0.06.0016
	{
		memset(szDispBuff, 0, sizeof(szDispBuff));
		FormatAmt(szDispBuff, gPTStruct.HMinTotal, gCurrencySymbol, ASCII_IN);
		PubTrimHeadChars(szDispBuff, ' ');

		ScrCls();
		ucLen = strlen(szDispBuff);
		ScrPrint(0, 2, ASCII, "MIN: %s", szDispBuff);

		memset(szDispBuff, 0, sizeof(szDispBuff));
		FormatAmt(szDispBuff, gPTStruct.HMaxTotal, gCurrencySymbol, ASCII_IN);
		PubTrimHeadChars(szDispBuff, ' ');
		ScrPrint(0, 3, ASCII, "MAX: %s", szDispBuff);//DSP_AMOUNT_MAX_LIMIT

		ucLen = strlen("The amount should be");//DSP_AMOUNT_MIN_LIMIT
		ScrPrint(0, 5, ASCII, "The amount should be");
		ScrPrint(0, 6, ASCII, "between MIN and MAX");

		DelayMs(3000);
		return(STAT_INV);	
	}	
#endif

/************************************************************************
*	Principal = Amount
*	Yearly Profit Flat Rate = gPTStruct.SchemeProfit
*
*	Installment = (Principal + Profit) / Tenure
*	Profit = (Principal * Monthly Profit Flat Rate * Tenure)
*************************************************************************/

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
	
	//Monthly Profit Flat Rate
	memset(profit, 0, sizeof(profit));	
	memcpy(profit+4, gPTStruct.SchemeProfit, 2);							//Yearly Flat Rate	e.g 948 -->9.48% --> 4 bit decimal
	
	//Mult.  Principal * Monthly Profit Flat Rate
	memset(Bcd1, 0, sizeof(Bcd1));
	MulAsciiBCD(Bcd1, profit, Amount, 6, BCD_IN);							//profit = Yearly Flat Rate * amount(2 bit decimal)  

	/*
	amount = 40000 --> 400.00
	scheme = \x12\x00 --> 1200 --> 00.1200
	40000 * 1200 = 48 00 00 00 --> 48 00 00.00 extend 10000 times
	*/
	
	// div 100
	//Div.10000
	//memset(profit, 0, sizeof(profit));	
	//memcpy(profit+3, "\x01\x00\x00", 3);
	
	//strcpy((char *)DivAmout, "000000010000");	//10000-->AED 10000.00
	strcpy((char *)DivAmout, "000000010000");
	memset(ByDivAmout, 0, sizeof(ByDivAmout));
	Utils_ConvertBcdToAsc(Bcd1, ByDivAmout, 6);		//ByDivAmout --> out	ascii 

	memset(PercentAmout, 0, sizeof(PercentAmout));
	ulRet = DivStr(DivAmout, ByDivAmout, PercentAmout, 1);
	memset(Bcd2, 0, sizeof(Bcd2));
	sprintf((char *)PercentAmout, "%012d", ulRet);
	PubAsc2Bcd(PercentAmout, 12, Bcd2);

	//DivAsciiBCD(Bcd2, Bcd1, profit, 6, BCD_IN, remain);
	//DivBcdNum (Bcd1 , 10000 , Bcd2      , 7);
	memcpy(RateAmount,Bcd2,6);												//RateAmount = profit / 10000

/************************************************************************
* Principal = Amount                                                   
* Yearly Profit Flat Rate = gPTStruct.SchemeProfit                     
* Yearly Profit RateAmount = Principal * Yearly Profit Flat Rate / 10000
************************************************************************/

	//by jarod 20111013
	//for test
// 	gPTStruct.AvailableInstallmentFlag2 = 1;
// 	gPTStruct.AvailableInstallmentFlag2= gPTStruct.AvailableInstallmentFlag2&1;
	for (i=0; i<NUM_OPTIONS; i++)
	{
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

#ifdef ADIB_CHANGES	
		if (RateValue<gPTStruct.HMinTenure || RateValue>gPTStruct.HMaxTenure)
			RateValue=0;
#else
		if (RateValue > gPTStruct.MaxTenure)
			RateValue=0;
#endif	

/******************************************************************
* 		Principal = Amount
* 		Yearly Profit Flat Rate = gPTStruct.SchemeProfit
*		
* 		Installment = (Principal + Profit) / Tenure
* 		Profit = (Principal * Monthly Profit Flat Rate * Tenure)
*******************************************************************/		
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
				memset(Bcd1,0,6);
				memset(profit,0,6);
				profit[5]=bytebin2bcd(RateValue);//Tenure
/********************************************************************
*   			Principal = Amount
*				Yearly Profit Flat Rate = gPTStruct.SchemeProfit
*				Yearly Profit	RateAmount = ( (Principal *  Yearly Profit Flat Rate) / 10000 ) * Tenure                                    
*********************************************************************/				
				MulAsciiBCD(Bcd1, profit, RateAmount, 6, BCD_IN);			//Tenure*RateAmount	
				memset(profit,0,6);
				
				//by jarod 20111024
				//annual profit
				//profit[5]=bytebin2bcd(12);
				//DivAsciiBCD(Bcd2, Bcd1, profit, 6, BCD_IN, remain);			//Total Tenure Profit = Yearly Profit / 12

				memset(ByDivAmout, 0, sizeof(ByDivAmout));
				Utils_ConvertBcdToAsc(Bcd1, ByDivAmout, 6);
				strcpy(DivAmout, "000000000012");
				memset(PercentAmout, 0, sizeof(PercentAmout));
				ulRet = DivStr(DivAmout, ByDivAmout, PercentAmout, 1);//Total Tenure Profit = Yearly Profit / 12
				sprintf((char *)PercentAmout, "%012d", ulRet);
				PubAsc2Bcd(PercentAmout, 12, Bcd2);
			
				//Add. TotalInstallmentAmt = Principal + Tenure Profit
				memcpy(BcdProfit,Bcd2,6);									//BcdProfit = Total Tenure Profit
				AddAsciiBCD(Bcd1, Amount, Bcd2, 6, BCD_IN);					//Amount(BCD format) = gPTStruct.EntryAmount 
	
				memset(Buffer, 0, sizeof(Buffer));
				StrBcdToAsc(6, Bcd1, Buffer);
				//Buffer[12] = 0;												//Total Amount with surcharge
				memcpy(&TotalInstallmentAmount[j], Buffer, LG_AMOUNT);		//TotalInstallment Amount

				//memset(Bcd2,0,6);
				//memset(profit,0,6);
				//profit[5]=bytebin2bcd(RateValue);			
				//Bcd1 = TotalInsAmt
				//PubBcd2Asc0(Bcd1, 6, ByDivAmout);							//RateValue = Tenure
				strcpy(ByDivAmout, Buffer);									//RateValue = Tenure
				sprintf((char *)DivAmout, "%012d", RateValue);				//Bcd1 = BCD format TotalInstallment Amount
				memset(PercentAmout, 0, sizeof(PercentAmout));
				ulRet = DivStr(DivAmout, ByDivAmout, PercentAmout, 1);//month Amount with surcharge
				//strcpy(Buffer, PercentAmout);
				sprintf((char *)Buffer, "%012d", ulRet);


				//DivAsciiBCD(Bcd2, Bcd1, profit, LG_TRAMT, BCD_IN, remain);	//Bcd1 = BCD format TotalInstallment Amount
				//StrBcdToAsc(LG_AMOUNT, Bcd2, Buffer);						//month Amount with surcharge	
				
				//by jarod 20111020
				//Buffer[LG_AMOUNT]=0;

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

			gPTStruct.Balance[LG_AMOUNT] = 0;
			if(memcmp(Buffer, gPTStruct.Balance, LG_AMOUNT) <= 0)
			{
//by jarod  2011.06.09
// 				memcpy(MonhthlyInstAmount[i],Buffer,LG_AMOUNT);
// 				TotalInstallmentCount[i]=RateValue;
// 				memcpy(TotalProfitAmount[i],BcdProfit,6);

				memcpy(MonhthlyInstAmount[j],Buffer,LG_AMOUNT);
				TotalInstallmentCount[j]=RateValue;
				memcpy(TotalProfitAmount[j],BcdProfit,6);					//BcdProfit = Total Tenure Profit
				j++;
			}			
		}
	}

TENURE_MENU:
    memset(&MENU,0,sizeof(MENU));
	lbCounter=0;
	
//	strcpy(MENU[lbCounter++], _T("SELECT TENURE"));
	if (ARABIC == gLanguage)
	{
		strcpy(MENU[lbCounter], "«Œ — „œ… «·”œ«œ\nÊ„»·€ «·ﬁ”ÿ «·‘Â—Ì");
	}
	else
	{
		strcpy(MENU[lbCounter], "SELECT A TENURE &\nMONTHLY INST. AMOUNT");
	}

//	lbCounter++;
//	strcpy(MENU[lbCounter], "MONTHLY INST. AMOUNT");

	for (i=0; i<NUM_OPTIONS; i++)
	{
		if(memcmp(MonhthlyInstAmount[i], UTIL_FILLZEROS,12) != 0)
		{
			++lbCounter;
			memset(Buffer,0,sizeof(Buffer));
			Buffer[0]=(TotalInstallmentCount[i]/10)+'0';
			Buffer[1]=(TotalInstallmentCount[i]%10)+'0';

			memcpy(RateAmount, MonhthlyInstAmount[i], LG_AMOUNT);
			RateAmount[LG_AMOUNT]=0;
            
			FormatAmt(Bcd2, RateAmount, gCurrencySymbol, ASCII_IN);	
			//by jarod 2011.05.27	
//			sprintf(Buffer+2,"M-%.15.15s",Bcd2);
			sprintf(Buffer+2,"M @ %-15.15s", Bcd2);
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
		//OS_ShowMenu(ItemCount, (BYTE **)MenuItems, FirstItem, gOldItem, TimeOut, byArabicHeader);
		lbMenuLine = (BYTE)KeyBoardMenu2(lbCounter, (const BYTE **)lpBuf, &FirstItem,
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

            //Print_SaleDraft_Slip();				//printout 1		20130123

			memset(&MainMSG, 0, sizeof(MainMSG));
			ClearCustomMessages(&MainMSG);	
		
			//screen 15
//			ScrCls();
			if (ARABIC == gLanguage)
			{
				memset(&stPageContent, 0, sizeof(ST_PAGECONTENT));
				strcpy((char *)stPageContent.stContent[0].stLineContent[0].content, "·ﬁœ «Œ —   ﬁ”Ìÿ ");//pos screen1
				strcpy((char *)stPageContent.stContent[0].stLineContent[1].content, "„œÌÊ‰Ì… «·„—«»Õ…");
				strcpy((char *)stPageContent.stContent[0].stLineContent[2].content, "(«· Ì ” »—„Â« „⁄");
				strcpy((char *)stPageContent.stContent[0].stLineContent[3].content, "«·„’—›) Ê›ﬁ «·¬ Ì ");

				strcpy((char *)stPageContent.stContent[1].stLineContent[0].content, "(Ì—ÃÏ «Œ Ì«—");
				strcpy((char *)stPageContent.stContent[1].stLineContent[1].content, "«·ŒÌ«— —ﬁ„ (1)");
				strcpy((char *)stPageContent.stContent[1].stLineContent[2].content, "›Ì Õ«· «·„Ê«›ﬁ…");
				strcpy((char *)stPageContent.stContent[1].stLineContent[3].content, "⁄·Ï „œ… «·”œ«œ");

				//screen 16
				strcpy((char *)stPageContent.stContent[2].stLineContent[0].content, "Ê„»·€ «·ﬁ”ÿ ");
				strcpy((char *)stPageContent.stContent[2].stLineContent[1].content, "«·‘Â—Ì √œ‰«Â ÊÌ⁄œ");
				strcpy((char *)stPageContent.stContent[2].stLineContent[2].content, "–·ﬂ „Ê«›ﬁ… „‰");
				strcpy((char *)stPageContent.stContent[2].stLineContent[3].content, "ﬁ»·ﬂ„ √Ì÷« ⁄·Ï ");

				//screen 16-5
				strcpy((char *)stPageContent.stContent[3].stLineContent[0].content, "≈—”«· \"√„— ");
				strcpy((char *)stPageContent.stContent[3].stLineContent[1].content, "«·‘—«¡\" ≈·Ï ");
				strcpy((char *)stPageContent.stContent[3].stLineContent[2].content, "«·„’—›)");
				//strcpy((char *)stPageContent.stContent[3].stLineContent[3].content, "Ê„»·€ «·ﬁ”ÿ ");

				/*
				//screen 17-2
				strcpy((char *)stPageContent.stContent[4].stLineContent[0].content, "«·‘Â—Ì √œ‰«Â ÊÌ⁄œ");
				strcpy((char *)stPageContent.stContent[4].stLineContent[1].content, "–·ﬂ „Ê«›ﬁ… „‰");
				strcpy((char *)stPageContent.stContent[4].stLineContent[2].content, "ﬁ»·ﬂ„ √Ì÷« ⁄·Ï ");
				strcpy((char *)stPageContent.stContent[4].stLineContent[3].content, "≈—”«· \"√„— ");

				//screen 17-6
				strcpy((char *)stPageContent.stContent[5].stLineContent[0].content, "«·‘—«¡\" ≈·Ï ");
				strcpy((char *)stPageContent.stContent[5].stLineContent[1].content, "«·„’—›)");
				*/


				//screen 18
				memset(Buffer,0,sizeof(Buffer));
				Buffer[0]=(TotalInstallmentCount[lbMenuLine]/10)+'0';
				Buffer[1]=(TotalInstallmentCount[lbMenuLine]%10)+'0';
				
				memcpy(RateAmount, MonhthlyInstAmount[lbMenuLine], LG_AMOUNT);
				RateAmount[LG_AMOUNT]=0;
				
				FormatAmt(Bcd2, RateAmount, gCurrencySymbol, ASCII_IN);
				sprintf(Buffer+2,"M @ %-15s",Bcd2);
				//			sprintf(Buffer+2,"M-%.15s",Bcd2);
				//sprintf((char *)BufferTmp, "%30.30s", Buffer);
				stPageContent.stContent[4].stLineContent[0].ucMode = ARABIC_ASCII;
				strcpy((char *)stPageContent.stContent[4].stLineContent[0].content, Buffer);
				strcpy((char *)stPageContent.stContent[4].stLineContent[1].content, "1- „Ê«›ﬁ ");
				strcpy((char *)stPageContent.stContent[4].stLineContent[2].content, "2- €Ì— „Ê«›ﬁ ");

				//ulRet = DisplayContent(7, 6);

				ulRet = DisplayContent(5, 4);
				if (ulRet == STAT_NOK )
				{
					return STAT_NOK;
				}
				else if (ulRet == KEY2)
				{
					goto TENURE_MENU;
				}

// 				ArShowString(0, SHIFT_RIGHT, "·ﬁ?«Œ —? ﬁ”Ì?");		//pos screen1
// 				//screen 16
// 				//screen 16-5
//  			//screen 17-2
// 				//screen 17-6

			}
			else
			{
				memset(&stPageContent, 0, sizeof(ST_PAGECONTENT));
				strcpy((char *)stPageContent.stContent[0].stLineContent[0].content, "YOU HAVE SELECTED TO");
				strcpy((char *)stPageContent.stContent[0].stLineContent[1].content, "PAY THE TOTAL SELLING");
				strcpy((char *)stPageContent.stContent[0].stLineContent[2].content, "PRICE OF THE MURABAHA");
				strcpy((char *)stPageContent.stContent[0].stLineContent[3].content, "CONTRACT THAT YOU");
				strcpy((char *)stPageContent.stContent[0].stLineContent[4].content, "WILL CONCLUDE WITH");
				strcpy((char *)stPageContent.stContent[0].stLineContent[5].content, "THE BANK AS FOLLOWS");
				strcpy((char *)stPageContent.stContent[0].stLineContent[6].content, "(IF YOU AGREE WITH");
				strcpy((char *)stPageContent.stContent[0].stLineContent[7].content, "THE FOLLOWING TENURE");

				strcpy((char *)stPageContent.stContent[1].stLineContent[0].content, "AND MONTHLY");
				strcpy((char *)stPageContent.stContent[1].stLineContent[1].content, "INSTALLMENT AMOUNT");
				strcpy((char *)stPageContent.stContent[1].stLineContent[2].content, "PLEASE SELECT OPTION");
				strcpy((char *)stPageContent.stContent[1].stLineContent[3].content, "\"1\" BELOW: ");

				memset(Buffer,0,sizeof(Buffer));
				Buffer[0]=(TotalInstallmentCount[lbMenuLine]/10)+'0';
				Buffer[1]=(TotalInstallmentCount[lbMenuLine]%10)+'0';
				
				memcpy(RateAmount, MonhthlyInstAmount[lbMenuLine], LG_AMOUNT);
				RateAmount[LG_AMOUNT]=0;
				
				FormatAmt(Bcd2, RateAmount, gCurrencySymbol, ASCII_IN);
				sprintf(Buffer+2,"M @ %-15s",Bcd2);
				//			sprintf(Buffer+2,"M-%.15s",Bcd2);
				strcpy((char *)stPageContent.stContent[1].stLineContent[5].content, Buffer);

				strcpy((char *)stPageContent.stContent[2].stLineContent[2].content, "1-YES, SEND THE ORDER");
				strcpy((char *)stPageContent.stContent[2].stLineContent[3].content, "TO PURCHASE");
				strcpy((char *)stPageContent.stContent[2].stLineContent[4].content, "2-NO, PROVIDE THE");
				strcpy((char *)stPageContent.stContent[2].stLineContent[5].content, "OPTIONS AGAIN.");
				
				ulRet = DisplayContent(3, 2);
				if (ulRet == STAT_NOK )
				{
					return STAT_NOK;
				}
				else if (ulRet == KEY2)
				{
					goto TENURE_MENU;
				}
			}




// 			//screen 18		
// 			//screen 19
	

			bRet = AskYesNo2(&MainMSG,1,4);
			if(bRet == 1)
			{
//				ScrCls();
				if (ARABIC == gLanguage)
				{
					//screen 30
					memset(&stPageContent, 0, sizeof(ST_PAGECONTENT));
					strcpy((char *)stPageContent.stContent[0].stLineContent[0].content, "√ƒﬂœ »’› Ì „„À·«");
					strcpy((char *)stPageContent.stContent[0].stLineContent[1].content, "··„’—› √‰‰Ì");
					strcpy((char *)stPageContent.stContent[0].stLineContent[2].content, "»≈œŒ«·Ì ·—ﬁ„");
					strcpy((char *)stPageContent.stContent[0].stLineContent[3].content, "«· ⁄—Ì› «·‘Œ’Ì");

					strcpy((char *)stPageContent.stContent[1].stLineContent[0].content, "·„„À· «·„’—›");
					strcpy((char *)stPageContent.stContent[1].stLineContent[1].content, "«·Œ«’ »Ì ﬁœ ﬁ„  ");
					strcpy((char *)stPageContent.stContent[1].stLineContent[3].content, "»«·¬ Ì:");

					//screen 31
					strcpy((char *)stPageContent.stContent[2].stLineContent[0].content, "1- » ⁄ÌÌ‰ Ê›—“");
					strcpy((char *)stPageContent.stContent[2].stLineContent[1].content, "«·»÷«⁄… »–« Â«");
					strcpy((char *)stPageContent.stContent[2].stLineContent[2].content, "ﬁ»· Ê»⁄œ ≈»—«„ ");
					strcpy((char *)stPageContent.stContent[2].stLineContent[3].content, "⁄ﬁœ «·‘—«¡ («–« ");

					strcpy((char *)stPageContent.stContent[3].stLineContent[0].content, "·„ Ìﬂ‰ ·Â« —ﬁ„ ");//pos screen 25
					strcpy((char *)stPageContent.stContent[3].stLineContent[1].content, "Œ«’)");
					strcpy((char *)stPageContent.stContent[3].stLineContent[2].content, "2- „⁄«Ì‰… «·»÷«⁄…. ");
					strcpy((char *)stPageContent.stContent[3].stLineContent[3].content, "Ê«· √ﬂœ „‰");

					//screen 32
					strcpy((char *)stPageContent.stContent[4].stLineContent[0].content, "”·«„ Â« ");
					strcpy((char *)stPageContent.stContent[4].stLineContent[1].content, "3-  ”·„ «·»÷«⁄… ");
					strcpy((char *)stPageContent.stContent[4].stLineContent[2].content, " ”·„« ÕﬁÌﬁÌ« √Ê ");
					strcpy((char *)stPageContent.stContent[4].stLineContent[3].content, "Õﬂ„Ì« ");

					strcpy((char *)stPageContent.stContent[5].stLineContent[0].content, "4- √‰ «·»÷«∆⁄ ");//pos screen 27
					strcpy((char *)stPageContent.stContent[5].stLineContent[1].content, "«·„–ﬂÊ—… Â‰« ÂÌ");
					strcpy((char *)stPageContent.stContent[5].stLineContent[2].content, "‰›” «·»÷«∆⁄ ");
					strcpy((char *)stPageContent.stContent[5].stLineContent[3].content, "«·Ê«—œ  ›’Ì·Â« ");

					//screen 33
					strcpy((char *)stPageContent.stContent[6].stLineContent[0].content, "›Ì ⁄ﬁœ «·‘—«¡");//pos screen 28

					if (DisplayContent(7, NO_SELECT_PAGE) != STAT_OK)
					{
						return STAT_NOK;
					}

// 					//screen 30					
// 					//screen 31
// 					ArShowString(0, SHIFT_RIGHT, "·„ Ìﬂ?·Â?—ﬁ?");//pos screen 25

// 
// 					//screen 32					
// 					//screen 33
					//screen 34
					if ((fp = open("AGENTPIN", O_RDWR)) < 0)
					{
						Close(fp);
						return STAT_NOK;
					}
					else
					{
						if (read(fp, pszOut+6, 4) != 4)
						{
							Close(fp);
							return STAT_NOK;
						}
						Close(fp);
					}

					for (i = 0; i < 3; i++)
					{
						ScrCls();
						ArShowString(0, SHIFT_CENTER, "—ﬁ„ «· ⁄—Ì› «·‘Œ’Ì");
						ArShowString(2, SHIFT_CENTER, "·„„À· «·„’—›");//pos screen 29
						ScrGotoxy(52, 6);
						if( 0 != GetString(pszOut, 0x6D, 4, 4) )
						{
							return STAT_INV;
						}
						if(memcmp(pszOut+1,pszOut+6,4) != 0)
						{
							OS_ClearScreen();
							ScrPrint(0, 3, ASCII, "PIN ERR, RETRY");
							DelayMs(1000);
							continue;
						}
						else
						{
							break;
						}
					}

					if (i >= 3)
					{
						return STAT_NOK;
					}

					Print_Confirmation();	//printout 2

					for (i = 0; i < gPTStruct.TotItems; i++)
					{

						//screen 35
						memset(&stPageContent, 0, sizeof(ST_PAGECONTENT));
						strcpy((char *)stPageContent.stContent[0].stLineContent[0].content, "⁄ﬁœ «·»Ì⁄ ");
						strcpy((char *)stPageContent.stContent[0].stLineContent[1].content, "»«·„—«»Õ… ");
						strcpy((char *)stPageContent.stContent[0].stLineContent[3].content, "⁄“Ì“Ì „ ⁄«„·");//pos screen 30

						strcpy((char *)stPageContent.stContent[1].stLineContent[0].content, "«· „ÊÌ· «·›Ê—Ì");//pos screen 31
						strcpy((char *)stPageContent.stContent[1].stLineContent[2].content, "»«⁄ »„ÊÃ»Â «·„’—›");
						strcpy((char *)stPageContent.stContent[1].stLineContent[3].content, "«·»÷«∆⁄ «·Ê«—œ… ");

						//screen 36
						strcpy((char *)stPageContent.stContent[2].stLineContent[0].content, " ›«’Ì·Â« √œ‰«Â ");//pos screen 32
						strcpy((char *)stPageContent.stContent[2].stLineContent[1].content, "(√Ê Õ’… ‘«∆⁄… „‰");
						strcpy((char *)stPageContent.stContent[2].stLineContent[2].content, "«·»÷«∆⁄ ›Ì Õ«· ");
						strcpy((char *)stPageContent.stContent[2].stLineContent[3].content, " ÕœÌœ ‰”» Â«");

						//screen 37
						strcpy((char *)stPageContent.stContent[3].stLineContent[0].content, "›Ì„« Ì·Ì) Ê›ﬁ«");//pos screen 33
						strcpy((char *)stPageContent.stContent[3].stLineContent[1].content, "·‘—Êÿ Ê√Õﬂ«„");
						strcpy((char *)stPageContent.stContent[3].stLineContent[2].content, "»—‰«„Ã «· „ÊÌ· ");
						strcpy((char *)stPageContent.stContent[3].stLineContent[3].content, "«·›Ê—Ì «· Ì");

						strcpy((char *)stPageContent.stContent[4].stLineContent[0].content, "ﬁ—√ Â« Ê›Â„ Â« ");//pos screen 34
						strcpy((char *)stPageContent.stContent[4].stLineContent[1].content, "ÊÊ«›ﬁ  ⁄·ÌÂ« ");
						strcpy((char *)stPageContent.stContent[4].stLineContent[2].content, "(  ÊÃœ ‰”Œ… „‰Â«");
						strcpy((char *)stPageContent.stContent[4].stLineContent[3].content, "⁄·Ï «·„Êﬁ⁄");

						strcpy((char *)stPageContent.stContent[5].stLineContent[0].content, "«·≈·ﬂ —Ê‰Ì ··„’—›");
						strcpy((char *)stPageContent.stContent[5].stLineContent[1].content, ")");

						strcpy((char *)stPageContent.stContent[6].stLineContent[0].content, "‰Ê⁄ «·»÷«⁄…:");//pos screen 36
						sprintf((char *)BufferTmp, "%-16.16s", gPTStruct.ProductDescription);
						//PubTrimTailChars(BufferTmp, ' ');
						sprintf((char *)Buffer, "%-16.16s", BufferTmp);
						stPageContent.stContent[6].stLineContent[1].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[6].stLineContent[1].content, Buffer);


						strcpy((char *)stPageContent.stContent[6].stLineContent[2].content, "Ê’› «·»÷«⁄…:");
						sprintf((char *)Buffer, "%-16.16s", gPTStruct.product[i].proddesc);	
						PubTrimTailChars(Buffer, ' ');
						//bRet = gPTStruct.ProductCode[0] * 10 + gPTStruct.ProductCode[1];
						//sprintf((char *)Buffer, "%-16d", bRet);
						stPageContent.stContent[6].stLineContent[3].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[6].stLineContent[3].content, (const char *)Buffer);


						strcpy((char *)stPageContent.stContent[7].stLineContent[0].content, "ﬂ„Ì… «·»÷«⁄…:");//pos screen 37
						sprintf((char *)Buffer, "%-16d", gPTStruct.product[i].numbers);//gPTStruct.TotItems
						stPageContent.stContent[7].stLineContent[1].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[7].stLineContent[1].content, Buffer);
						strcpy((char *)stPageContent.stContent[7].stLineContent[2].content, "—ﬁ„ —„“ «·‘—Ìÿ:");
						sprintf(BufferTmp, "%-16.16s", gPTStruct.product[i].barcode);
						sprintf((char *)Buffer, "%-16.16s", BufferTmp);
						stPageContent.stContent[7].stLineContent[3].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[7].stLineContent[3].content, Buffer);

						//screen 39
						strcpy((char *)stPageContent.stContent[8].stLineContent[0].content, "≈Ã„«·Ì «· ﬂ·›…:");//pos screen 38
						Utils_ConvertBcdToAsc(gPTStruct.product[i].price_bcd, Amount, 6);
						Amount[12] = 0;

						memset(BufferTmp, 0, sizeof(BufferTmp));
						FormatAmt(BufferTmp, Amount, gCurrencySymbol, ASCII_IN);
						PubTrimHeadChars(BufferTmp, ' ');
						sprintf((char *)Buffer, "%-16.16s", BufferTmp);
						
						//======chenxi modify to slove the problem of Pic 1842
						stPageContent.stContent[8].stLineContent[1].ucMode = ARABIC_CFONT;
						//=================
						strcpy((char *)stPageContent.stContent[8].stLineContent[1].content, Buffer);
						strcpy((char *)stPageContent.stContent[8].stLineContent[2].content, "«·‘—«¡ ·Õ’…");
						strcpy((char *)stPageContent.stContent[8].stLineContent[3].content, "‘«∆⁄… ﬁœ—Â« ");

						//DivAsciiBCD(profitRate, gPTStruct.SchemeProfit, Bcd2, 2, BCD_IN, remain);
						memset(DivAmout, 0, sizeof(DivAmout));
						memset(ByDivAmout, 0, sizeof(ByDivAmout));
						memset(PercentAmout, 0, sizeof(PercentAmout));
						//memcpy(DivAmout, gPTStruct.EntryAmount, 12);
						//memcpy(ByDivAmout, gPTStruct.AdvancedAmount, 12);

						PubAscSub(gPTStruct.EntryAmount, gPTStruct.AdvancedAmount, 12, ByDivAmout);

						//memcpy(ByDivAmout, gPTStruct.AdvancedAmount, 12);		// for test2013-01-29 percentage = EntryAmount - AdvancedAmount/ EntryAmount;
						memcpy(DivAmout, gPTStruct.EntryAmount, 12);

						ulRet = DivStr(DivAmout, ByDivAmout, PercentAmout, 5);	//DivAsciiBCD
						//ulRet = atol(PercentAmout);
						sprintf((char *)BufferTmp, "%s%d.%02d", "%", ulRet/100, ulRet%100);
						sprintf((char *)Buffer, "%16.16s", BufferTmp);
						stPageContent.stContent[9].stLineContent[0].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[9].stLineContent[0].content, Buffer);	//pos screen 39
						strcpy((char *)stPageContent.stContent[9].stLineContent[1].content, "„‰ «·»÷«∆⁄ ");
						strcpy((char *)stPageContent.stContent[9].stLineContent[2].content, "„»·€ «·—»Õ:");
						

						////////////////
						memset(RateAmount, 0, sizeof(RateAmount));
						//chenxi modify 1843

						Utils_ConvertBcdToAsc(gPTStruct.ProfitAmountBcd, RateAmount, 6);

						memset(BufferTmp, 0, sizeof(BufferTmp));
						FormatAmt(BufferTmp, RateAmount, gCurrencySymbol, ASCII_IN);
						PubTrimHeadChars(BufferTmp, ' ');
						sprintf(Buffer, "%-16.16s", BufferTmp);
						stPageContent.stContent[9].stLineContent[3].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[9].stLineContent[3].content, Buffer);

						//screen 40
						strcpy((char *)stPageContent.stContent[10].stLineContent[0].content, "«·À„‰ «·≈Ã„«·Ì:");//pos screen 40
						//FormatAmt(BufferTmp, gPTStruct.EntryAmount, gCurrencySymbol, ASCII_IN);
						//sprintf((char *)Buffer, "%-16.16s", BufferTmp);
						

						//total selling price = total product price + profit
//						ScrCls();
//						ScrPrint(0, 2, 0, "1:%s", RateAmount);
//						ScrPrint(0, 3, 0, "2:%s", gPTStruct.EntryAmount);
//						getkey();

						//============chenxi modify for pic 1843, selling price
						PubAscAdd(ByDivAmout, RateAmount, 12, Amount);	
						//PubAscAdd(gPTStruct.EntryAmount, RateAmount, 12, Amount);	
						//========================================================
//						ulRet = atol(RateAmount) + atol(gPTStruct.EntryAmount);
//						sprintf((char *)Amount, "%012d", ulRet);
						FormatAmt(Buffer, Amount, gCurrencySymbol, ASCII_IN);

						stPageContent.stContent[10].stLineContent[1].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[10].stLineContent[1].content, Buffer);
						strcpy((char *)stPageContent.stContent[10].stLineContent[2].content, "«·ﬁ”ÿ «·‘Â—Ì:");
						FormatAmt(BufferTmp, gPTStruct.MonthlyInstAmount, gCurrencySymbol, ASCII_IN);
						sprintf((char *)Buffer, "%-16.16s", BufferTmp);
						stPageContent.stContent[10].stLineContent[3].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[10].stLineContent[3].content, Buffer);

						strcpy((char *)stPageContent.stContent[11].stLineContent[0].content, "⁄œœ «·√ﬁ”«ÿ:");//pos screen 41
					//	memset(Buffer, 0, sizeof(Buffer));
						sprintf((char *)Buffer, "%-16d", gPTStruct.TotalInstallmentCount);
					//	sprintf((char *)Buffer, "%-2d", gPTStruct.TotalInstallmentCount);
						stPageContent.stContent[11].stLineContent[1].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[11].stLineContent[1].content, Buffer);
						strcpy((char *)stPageContent.stContent[11].stLineContent[2].content, " «—ÌŒ «” Õﬁ«ﬁ ");
						strcpy((char *)stPageContent.stContent[11].stLineContent[3].content, "ﬂ· ﬁ”ÿ:");

						//screen 41
						strcpy((char *)stPageContent.stContent[12].stLineContent[0].content, "25 „‰ ﬂ· ‘Â— ");//pos screen 42
						strcpy((char *)stPageContent.stContent[12].stLineContent[1].content, " «—ÌŒ «” Õﬁ«ﬁ ");
						strcpy((char *)stPageContent.stContent[12].stLineContent[2].content, "√Ê· ﬁ”ÿ:");
						//yearTmp = (AutoPassword[0] - 0x30) * 10 + (AutoPassword[1] - 0x30);//year
						//monthTmp = (AutoPassword[2] - 0x30) * 10 + (AutoPassword[3] - 0x30);//Month
	//					dateTmp = (AutoPassword[4] - 0x30) * 10 + (AutoPassword[5] - 0x30);//date
						//dateTmp = 25;
						// Modified by Kim_LinHB 2013-9-6 0.06.0028
						sprintf(Buffer, "%s%4.4s%d%06d", "20", AutoPassword, 25, 0);//YYYYMMDDhhmmss
						if (gPTStruct.TotalInstallmentCount > 0)
						{
							BYTE lWrkBuf2[100];
							PubCalDateTime(Buffer, lWrkBuf2, 1, "MM");
							strcpy(Buffer, lWrkBuf2);
						}
						//DD/MM/YY
						sprintf((char *)BufferTmp, "%2.2s/%2.2s/%4.4s", &Buffer[6], &Buffer[4], &Buffer[0]);
						// Modify End
						sprintf((char *)Buffer, "%-16.16s", BufferTmp);
						stPageContent.stContent[12].stLineContent[3].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[12].stLineContent[3].content, Buffer);

						strcpy((char *)stPageContent.stContent[13].stLineContent[0].content, " «—ÌŒ «” Õﬁ«ﬁ ");//pos screen 43
						strcpy((char *)stPageContent.stContent[13].stLineContent[1].content, "¬Œ— ﬁ”ÿ:");
						
						// Modified by Kim_LinHB 2013-9-6 0.06.0028
						sprintf(Buffer, "%s%4.4s%d%06d", "20", AutoPassword, 25, 0);//YYYYMMDDhhmmss
						if (gPTStruct.TotalInstallmentCount > 0)
						{
							BYTE lWrkBuf2[100];
							PubCalDateTime(Buffer, lWrkBuf2, gPTStruct.TotalInstallmentCount, "MM");
							strcpy(Buffer, lWrkBuf2);
						}
						//DD/MM/YY
						sprintf((char *)BufferTmp, "%2.2s/%2.2s/%4.4s", &Buffer[6], &Buffer[4], &Buffer[0]);
						// Modify End
						sprintf((char *)Buffer, "%-16.16s", BufferTmp);
						stPageContent.stContent[13].stLineContent[2].ucMode = ARABIC_CFONT;
						strcpy((char *)stPageContent.stContent[13].stLineContent[2].content, Buffer);
						
						if (DisplayContent(14, NO_SELECT_PAGE) != STAT_OK)
						{
							return STAT_NOK;
						}
					}

					//screen 35


 					//pos screen 30

 					//pos screen 31
 
					//screen 36

 					//pos screen 32

 					//pos screen 33

					//screen 37

					//pos screen 34		
 					//screen 38

						
					//pos screen 36

					//pos screen 37


					//screen 39
	
					//pos screen 38
					//pos screen 39

					//screen 40					
					//screen 41
					//pos screen 42


					ScrCls();					
					ArShowString(0, SHIFT_RIGHT, "1- ﬁ»·  «·‘—«¡");//pos screen 44
					ArShowString(2, SHIFT_RIGHT, "„‰ «·„’—› ");
					ArShowString(4, SHIFT_RIGHT, "2- ·„ √ﬁ»· «·‘—«¡");
					ArShowString(6, SHIFT_RIGHT, "„‰ «·„’—›");
					while (1)
					{
						ucKey = GetOneKey(TO_1MIN);
						if (ucKey == KEY1)
						{
							break;
						}
						else if (ucKey == KEY2 || ucKey == NOKEY)
						{
							return STAT_INV;
						}
					}
				}
				else
				{
					memset(&stPageContent, 0, sizeof(ST_PAGECONTENT));
					strcpy((char *)stPageContent.stContent[0].stLineContent[0].content, "I HEREBY CONFIRM IN");
					strcpy((char *)stPageContent.stContent[0].stLineContent[1].content, "MY CAPACITY AS ADIB");
					strcpy((char *)stPageContent.stContent[0].stLineContent[2].content, "REPRESENTATIVE, BY");
					strcpy((char *)stPageContent.stContent[0].stLineContent[3].content, "ENTERING MY ADIB");
					strcpy((char *)stPageContent.stContent[0].stLineContent[4].content, "REPRESENTATIVE PIN");
					strcpy((char *)stPageContent.stContent[0].stLineContent[5].content, "THAT I HAVE PERFORMED");
					strcpy((char *)stPageContent.stContent[0].stLineContent[6].content, "THE FOLLOWING:");

// 					//screen 30


					strcpy((char *)stPageContent.stContent[1].stLineContent[0].content, "1- SORTED AND");
					strcpy((char *)stPageContent.stContent[1].stLineContent[1].content, "ALLOCATED THE");
					strcpy((char *)stPageContent.stContent[1].stLineContent[2].content, "PURCHASED PRODUCTS");
					strcpy((char *)stPageContent.stContent[1].stLineContent[3].content, "BEFORE AND AFTER");
					strcpy((char *)stPageContent.stContent[1].stLineContent[4].content, "CONCLUDING THE");
					strcpy((char *)stPageContent.stContent[1].stLineContent[5].content, "PURCHASE CONTRACT (IF");
					strcpy((char *)stPageContent.stContent[1].stLineContent[6].content, "THEY DO NOT HAVE");
					strcpy((char *)stPageContent.stContent[1].stLineContent[7].content, "SERIAL NUMBERS).");

					//screen 31
					strcpy((char *)stPageContent.stContent[2].stLineContent[0].content, "2- INSPECTED THE");
					strcpy((char *)stPageContent.stContent[2].stLineContent[1].content, "PRODUCTS AND");
					strcpy((char *)stPageContent.stContent[2].stLineContent[2].content, "CONFIRMED THAT THEY");
					strcpy((char *)stPageContent.stContent[2].stLineContent[3].content, "ARE IN GOOD"); 
					strcpy((char *)stPageContent.stContent[2].stLineContent[4].content, "CONDITIONS.");

					strcpy((char *)stPageContent.stContent[2].stLineContent[5].content, "3- RECEIVED THE");
					strcpy((char *)stPageContent.stContent[2].stLineContent[6].content, "PRODUCTS PHYSICALLY");
					strcpy((char *)stPageContent.stContent[2].stLineContent[7].content, "OR CONSTRUCTIVELY.");


					strcpy((char *)stPageContent.stContent[3].stLineContent[0].content, "4- THE PRODUCTS");
					strcpy((char *)stPageContent.stContent[3].stLineContent[1].content, "MENTIONED HEREIN ARE");
					strcpy((char *)stPageContent.stContent[3].stLineContent[2].content, "THE SAME PRODUCTS");
					strcpy((char *)stPageContent.stContent[3].stLineContent[3].content, "WHOSE DETAILED ARE");
					strcpy((char *)stPageContent.stContent[3].stLineContent[4].content, "SPECIFIED IN THE");
					strcpy((char *)stPageContent.stContent[3].stLineContent[5].content, "PURCHASE CONTRACT.");

					if (DisplayContent(4, NO_SELECT_PAGE) != STAT_OK)
					{
						return STAT_NOK;
					}
// 					//screen 31
					//screen 34
					if ((fp = open("AGENTPIN", O_RDWR)) < 0)
					{
						Close(fp);
						return STAT_NOK;
					}
					else
					{
						if (read(fp, pszOut+6, 4) != 4)
						{
							Close(fp);
							return STAT_NOK;
						}
						Close(fp);
					}

					for (i = 0; i < 3; i++)
					{
						ScrCls();
						ScrPrint((BYTE)(64 - 17 * 3), 0, ASCII, "PLEASE ENTER YOUR");
						ScrPrint((BYTE)(64 - 14 * 3), 1, ASCII, "ADIB REP. PIN:");
						ScrGotoxy(52,6);

						
						if( 0 != GetString(pszOut, 0x6D, 4, 4) )
						{
								return STAT_INV;
						}

						if(memcmp(pszOut+1,pszOut+6,4) != 0)
						{
							OS_ClearScreen();
							ScrPrint(0, 3, ASCII, "PIN ERR, RETRY");
							DelayMs(1000);
							continue;
						}
						else
						{
							break;
						}
					}
			
					if (i >= 3)
					{
						return STAT_NOK;
					}


					Print_Confirmation();	//printout 2

 					//screen 35

					for (i = 0; i < gPTStruct.TotItems; i++)
					{
						memset(&stPageContent, 0, sizeof(ST_PAGECONTENT));
						sprintf((char *)Buffer, "%.17s", "MURABAHA SALE");
						strcpy((char *)stPageContent.stContent[0].stLineContent[0].content, Buffer);
						sprintf((char *)Buffer, "%.14s", "CONTRACT");
						strcpy((char *)stPageContent.stContent[0].stLineContent[1].content, Buffer);
						strcpy((char *)stPageContent.stContent[0].stLineContent[2].content, "DEAR CARDHOLDER,");
						strcpy((char *)stPageContent.stContent[0].stLineContent[3].content, "THE BANK HEREBY OFFER");
						strcpy((char *)stPageContent.stContent[0].stLineContent[4].content, "TO SELL TO YOU THE");
						strcpy((char *)stPageContent.stContent[0].stLineContent[5].content, "FOLLOWING PRODUCTS");
						strcpy((char *)stPageContent.stContent[0].stLineContent[6].content, "(OR A COMMON SHARE OF");
						strcpy((char *)stPageContent.stContent[0].stLineContent[7].content, "THE PRODUCTS, IF");

						strcpy((char *)stPageContent.stContent[1].stLineContent[0].content, "SPECIFIED BELOW) IN");
						strcpy((char *)stPageContent.stContent[1].stLineContent[1].content, "ACCORDANCE WITH THE");
						strcpy((char *)stPageContent.stContent[1].stLineContent[2].content, "TERMS AND CONDITIONS");
						strcpy((char *)stPageContent.stContent[1].stLineContent[3].content, "OF THE INSTANT");
						strcpy((char *)stPageContent.stContent[1].stLineContent[4].content, "FINANCE PROGRAM, THAT");
						strcpy((char *)stPageContent.stContent[1].stLineContent[5].content, "YOU HAVE READ,");
						strcpy((char *)stPageContent.stContent[1].stLineContent[6].content, "UNDERSTOOD & ACCEPTED");
						strcpy((char *)stPageContent.stContent[1].stLineContent[7].content, "(A COPY OF WHICH IS");//for test error
// 						if (DisplayContent(2) != STAT_OK)
// 						{
// 							return STAT_NOK;
// 						}

//						memset(&stPageContent, 0, sizeof(ST_PAGECONTENT));
						strcpy((char *)stPageContent.stContent[2].stLineContent[0].content, "AVAILABLE ON ADIB'S");
						strcpy((char *)stPageContent.stContent[2].stLineContent[1].content, "INTERNET WEBSITE).");

						strcpy((char *)stPageContent.stContent[2].stLineContent[2].content, "PRODUCT TYPE:");
						sprintf((char *)Buffer, "%16.16s", gPTStruct.ProductDescription);
						PubTrimTailChars(Buffer, ' ');
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[2].stLineContent[3].content, BufferTmp);

						//bRet = gPTStruct.ProductCode[0] * 10 + gPTStruct.ProductCode[1];
						//sprintf((char *)Buffer, "%21d", bRet);
						strcpy((char *)stPageContent.stContent[2].stLineContent[4].content, "DESCRIPTION:");
						sprintf((char *)BufferTmp, "%16.16s", gPTStruct.product[i].proddesc);	
						PubTrimTailChars(BufferTmp, ' ');
						// Modified by Kim_LinHB 2014-5-29
						sprintf((char *)Buffer, "%21.21s", BufferTmp);
						strcpy((char *)stPageContent.stContent[2].stLineContent[5].content, Buffer);

						strcpy((char *)stPageContent.stContent[2].stLineContent[6].content, "QUANTITY:");
						sprintf((char *)Buffer, "%21d", gPTStruct.product[i].numbers);//gPTStruct.TotItems
						strcpy((char *)stPageContent.stContent[2].stLineContent[7].content, Buffer);

// 						//screen36
						strcpy((char *)stPageContent.stContent[3].stLineContent[0].content, "BARCODE NUMBER:");
						sprintf((char *)Buffer, "%16.16s", gPTStruct.product[i].barcode);
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[3].stLineContent[1].content, BufferTmp);
						strcpy((char *)stPageContent.stContent[3].stLineContent[2].content, "PRICE: ");
						Utils_ConvertBcdToAsc(gPTStruct.product[i].price_bcd, Amount, 6);
						Amount[12] = 0;
						memset(Buffer, 0, sizeof(Buffer));
						FormatAmt(Buffer, Amount, gCurrencySymbol, ASCII_IN);
						PubTrimHeadChars(Buffer, ' ');
//						bRet = strlen(Buffer);
						sprintf(BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[3].stLineContent[3].content, BufferTmp);
						
						//chenxi find pic 1842
						strcpy((char *)stPageContent.stContent[3].stLineContent[4].content, "TOTAL PRODUCTS COST");
						strcpy((char *)stPageContent.stContent[3].stLineContent[5].content, "PRICE:");
						memset(Buffer, 0, sizeof(Buffer));
						//===========chenxi modify here, total cost price = total price - advanment============
						memset(ByDivAmout, 0, sizeof(ByDivAmout));
						PubAscSub(gPTStruct.EntryAmount, gPTStruct.AdvancedAmount, 12, ByDivAmout);
						FormatAmt(Buffer, ByDivAmout, gCurrencySymbol, ASCII_IN);
						//FormatAmt(szDispBuff, gPTStruct.EntryAmount, gCurrencySymbol, ASCII_IN);
						//=====================================================================================
						//FormatAmt(Buffer, gPTStruct.EntryAmount, gCurrencySymbol, ASCII_IN);
						PubTrimHeadChars(Buffer, ' ');
//						bRet = strlen(Buffer);
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[3].stLineContent[6].content, BufferTmp);
					
						//chenxi find pic 1841
						strcpy((char *)stPageContent.stContent[4].stLineContent[0].content, "THE PURCHASED SHARE");
						strcpy((char *)stPageContent.stContent[4].stLineContent[1].content, "OF THE PRODUCTS IS");
// 						memset(Bcd2, 0, 2);
// 						Bcd2[1] = bytebin2bcd(12);
// 						DivAsciiBCD(profitRate, gPTStruct.SchemeProfit, Bcd2, 2, BCD_IN, remain);
// 						memset(ascRate, 0, sizeof(ascRate));
// 						Utils_ConvertBcdToAsc(profitRate, ascRate, 2);
// 						if (ascRate[0] == '0')
// 						{
// 							ascRate[0] = ascRate[1];
// 							ascRate[1] = '.';
// 						}
// 						else
// 						{
// 							ascRate[4] = ascRate[3];
// 							ascRate[3] = ascRate[2];
// 							ascRate[2] ='.';
// 						}

						memset(DivAmout, 0, sizeof(DivAmout));
						memset(ByDivAmout, 0, sizeof(ByDivAmout));
						memset(PercentAmout, 0, sizeof(PercentAmout));
						//memcpy(DivAmout, gPTStruct.EntryAmount, 12);
						//memcpy(ByDivAmout, gPTStruct.AdvancedAmount, 12);
						
						PubAscSub(gPTStruct.EntryAmount, gPTStruct.AdvancedAmount, 12, ByDivAmout);
						//memcpy(ByDivAmout, gPTStruct.AdvancedAmount, 12);		// for test2013-01-29 percentage = EntryAmount - AdvancedAmount/ EntryAmount;
						memcpy(DivAmout, gPTStruct.EntryAmount, 12);

						ulRet = DivStr(DivAmout, ByDivAmout, PercentAmout, 5);	//DivAsciiBCD
						//ulRet = atol(PercentAmout);
						//sprintf((char *)PercentAmout, "%012d", ulRet);
						sprintf((char *)Buffer, "IS %d.%02d%s", ulRet/100, ulRet%100, "% OF THE");//gPTStruct.SchemeProfit
						strcpy((char *)stPageContent.stContent[4].stLineContent[2].content, Buffer);
						strcpy((char *)stPageContent.stContent[4].stLineContent[3].content, "TOTAL PRODUCTS.");
						strcpy((char *)stPageContent.stContent[4].stLineContent[4].content, "PROFIT AMOUNT:");
						memset(RateAmount, 0, sizeof(RateAmount));
						memset(Buffer, 0, sizeof(Buffer));
						Utils_ConvertBcdToAsc(gPTStruct.ProfitAmountBcd, RateAmount, 6);
						FormatAmt(Buffer, RateAmount, gCurrencySymbol, ASCII_IN);
						PubTrimHeadChars(Buffer, ' ');
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[4].stLineContent[5].content, BufferTmp);

						//chenxi find pic 1843
						strcpy((char *)stPageContent.stContent[4].stLineContent[6].content, "TOTAL SELLING PRICE:");
						memset(Buffer, 0, sizeof(Buffer));
						//total selling price = total product price + profit
						//ulRet = atol(RateAmount) + atol(gPTStruct.EntryAmount); //flow out
						//sprintf((char *)Amount, "%012d", ulRet);
						//==========chenxi modify selling price==================================================
						PubAscAdd(ByDivAmout, RateAmount, 12, Amount);
						//PubAscAdd(gPTStruct.EntryAmount, RateAmount, 12, Amount);	//20130202	
						FormatAmt(Buffer, Amount, gCurrencySymbol, ASCII_IN);
						//=========================================================================
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[4].stLineContent[7].content, BufferTmp);
						
// 						//screen 37
						strcpy((char *)stPageContent.stContent[5].stLineContent[0].content, "MONTHLY INSTALLMENT");
						strcpy((char *)stPageContent.stContent[5].stLineContent[1].content, "AMOUNT:");
						memset(Buffer, 0, sizeof(Buffer));
						FormatAmt(Buffer, gPTStruct.MonthlyInstAmount, gCurrencySymbol, ASCII_IN);
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[5].stLineContent[2].content, BufferTmp);
						strcpy((char *)stPageContent.stContent[5].stLineContent[3].content, "# OF INSTALLMENTS:");
						sprintf((char *)Buffer, "%-2d", gPTStruct.TotalInstallmentCount);
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[5].stLineContent[4].content, BufferTmp);

						strcpy((char *)stPageContent.stContent[5].stLineContent[5].content, "EACH MONTHLY");
						strcpy((char *)stPageContent.stContent[5].stLineContent[6].content, "INSTALLMENT AMOUNT");

						strcpy((char *)stPageContent.stContent[6].stLineContent[0].content, "DUE DATE:");
						//yearTmp = (AutoPassword[0] - 0x30) * 10 + (AutoPassword[1] - 0x30);//year
						//monthTmp = (AutoPassword[2] - 0x30) * 10 + (AutoPassword[3] - 0x30);//Month
//						dateTmp = (AutoPassword[4] - 0x30) * 10 + (AutoPassword[5] - 0x30);//date
						//dateTmp = 25;
						sprintf((char *)Buffer, "%dth OF EACH MONTH", 25);
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[6].stLineContent[1].content, BufferTmp);

						strcpy((char *)stPageContent.stContent[6].stLineContent[2].content, "FIRST INSTALLMENT");
						strcpy((char *)stPageContent.stContent[6].stLineContent[3].content, "DUE DATE:");
						// Modified by Kim_LinHB 2013-9-6 0.06.0028
						sprintf(BufferTmp, "%s%4.4s%d%06d", "20", AutoPassword, 25, 0);//YYYYMMDDhhmmss
						if (gPTStruct.TotalInstallmentCount > 0)
						{
							BYTE lWrkBuf2[100];
							PubCalDateTime(BufferTmp, lWrkBuf2, 1, "MM");
							strcpy(BufferTmp, lWrkBuf2);
						}
						//DD/MM/YY
						sprintf((char *)Buffer, "%2.2s/%2.2s/%4.4s", &BufferTmp[6], &BufferTmp[4], &BufferTmp[0]);
						// Modify End
						sprintf((char *)BufferTmp, "%21.21s", Buffer);

						strcpy((char *)stPageContent.stContent[6].stLineContent[4].content, BufferTmp);
						strcpy((char *)stPageContent.stContent[6].stLineContent[5].content, "LAST INSTALLMENT");
						strcpy((char *)stPageContent.stContent[6].stLineContent[6].content, "DUE DATE:");
						
						// Modified by Kim_LinHB 2013-9-6 0.06.0028
						sprintf(BufferTmp, "%s%4.4s%d%06d", "20", AutoPassword, 25, 0);//YYYYMMDDhhmmss
						if (gPTStruct.TotalInstallmentCount > 0)
						{
							BYTE lWrkBuf2[100];
							PubCalDateTime(BufferTmp, lWrkBuf2, gPTStruct.TotalInstallmentCount, "MM");
							strcpy(BufferTmp, lWrkBuf2);
						}
						//DD/MM/YY
						sprintf((char *)Buffer, "%2.2s/%2.2s/%4.4s", &BufferTmp[6], &BufferTmp[4], &BufferTmp[0]);
						// Modify End
						sprintf((char *)BufferTmp, "%21.21s", Buffer);
						strcpy((char *)stPageContent.stContent[6].stLineContent[7].content, BufferTmp);

						if (DisplayContent(7, NO_SELECT_PAGE) != STAT_OK)
						{
							return STAT_NOK;
						}

// 						//screen 38
					}

					//screen 42
					ScrCls();
					ScrPrint(0, 0, ASCII, "1- I, THE CARDHOLDER");
					ScrPrint(0, 1, ASCII, "HEREBY ACCEPT TO BUY");
					ScrPrint(0, 2, ASCII, "FROM ADIB.");
					ScrPrint(0, 4, ASCII, "2- I, THE CARDHOLDER");
					ScrPrint(0, 5, ASCII, "DECLINE TO BUY FROM");
					ScrPrint(0, 6, ASCII, "ADIB.");
					while (1)
					{
						ucKey = GetOneKey(TO_1MIN);
						if (ucKey == KEY1)
						{
							break;
						}
						else if (ucKey == KEY2 || ucKey == NOKEY)
						{
							return STAT_INV;
						}
					}
				}

//				Print_Confirmation();//printout #2

				return(STAT_OK); 
			}
			else if(bRet == 2)
			{
				//goto TENURE_MENU;
				//REJECTED TO SELL TO ADIB
				return STAT_INV;
			}
		}
	}
    else
    {
        OS_ClearScreen();
        ScrPrint(16, 3, CFONT, "NO TENURE");
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


