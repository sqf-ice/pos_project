#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_define.h"
#include "os_interface.h"
#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "SLCLOCK.h"
#include "utility.h"
#include "appLib.h"


static WORD s_PrintTransHeader(BYTE *fpTicketType);
static WORD s_IslemDetaylari(TransStruct *fpTransInfo);
static void s_IslemTipineGoreToplamlar(int lInstalmentCount,
                                     int lLoyaltyCount,
                                     int lPaymentCount,
                                     int lReversalCount,                                     
                                     BYTE lInstalmentAmount[LG_AMOUNT+1],
                                     BYTE lLoyaltyAmount[LG_AMOUNT+1],
                                     BYTE lPaymentAmount[LG_AMOUNT+1],
                                     BYTE lReversalAmount[LG_AMOUNT+1]
                                     );

WORD PrintTransList(BYTE fbListing,BYTE Message , BYTE *Filename)       
{
    TransStruct lrTransBuf;
    WORD lwLastSeq;
    BYTE *lpTitle;
    int  NumBytes;
    
    int  lGeneralTotalCount = 0;
    int  lInstalmentCount   = 0;
    int  lLoyaltyCount      = 0;
    int  lPaymentCount      = 0;
	int  lReversalCount     = 0;   //added by Allen 2016-09-20 for reversal transaction 
    //int  lAccountMaintCount = 0;
    
    BYTE lGeneralTotalAmount[LG_AMOUNT+1];
    BYTE lInstalmentAmount[LG_AMOUNT+1];
    BYTE lLoyaltyAmount[LG_AMOUNT+1];
    BYTE lPaymentAmount[LG_AMOUNT+1];
	BYTE lReversalAmount[LG_AMOUNT+1];   //added by Allen 2016-09-20 for reversal transaction 
    //BYTE lAccountMaintAmount[LG_AMOUNT+1];
    
    memset(lInstalmentAmount,      ' ',LG_AMOUNT);
    lInstalmentAmount[LG_AMOUNT] = 0;
    memset(lLoyaltyAmount,         ' ',LG_AMOUNT);
    lLoyaltyAmount[LG_AMOUNT] = 0;
    memset(lPaymentAmount,         ' ',LG_AMOUNT);
    lPaymentAmount[LG_AMOUNT] = 0;
    //	memset(lAccountMaintAmount,' ',LG_AMOUNT);
    memset(lGeneralTotalAmount,    ' ',LG_AMOUNT);
    lGeneralTotalAmount[LG_AMOUNT] = 0;
	//added by Allen 2016-09-20 
	memset(lReversalAmount,      ' ',LG_AMOUNT);
    lReversalAmount[LG_AMOUNT] = 0;
	//add end
    
    // full
    if (Check_TransFileEmpty(&lwLastSeq, Filename) == STAT_NOK)
    {
        if(Message==OS_TRUE)
		{
			DisplayString(DSP_NO_TRANSACTION,3000);
		}
        return STAT_NOK;
    }
    else
    {
        if (!fbListing)
            lpTitle = PRT_TOTYPE;
        else
            lpTitle = PRT_LOTYPE1;
    }
    
    //********************* Print header ****************************
    if (PaperControl() != STAT_OK)
	{
        return(STAT_NOK);
	}
    
    if (s_PrintTransHeader(lpTitle) == STAT_INV)
	{
        return STAT_INV;
	}
    
    // Open TR file
    gTrHandle=Open(Filename, O_RDWR);
    if (gTrHandle >= 0)
    {
        if ( Seek(gTrHandle, 0, SEEK_SET) >= 0 )
        {
            do
            {
                NumBytes = Read( gTrHandle, (BYTE *)&lrTransBuf, LG_TRANSSTRUCT);
                if ( NumBytes>0)
                {  //.000
                    if(memcmp(TYPE_70,lrTransBuf.TransType,2)==0)
                    {
                        AddAscii( lrTransBuf.MonthlyInstAmount, LG_AMOUNT, lGeneralTotalAmount, LG_AMOUNT, lGeneralTotalAmount, LG_AMOUNT);
                        lGeneralTotalCount += 1;
                    }
                    else
                    {
                        AddAscii( lrTransBuf.TotalAmount, LG_AMOUNT, lGeneralTotalAmount, LG_AMOUNT, lGeneralTotalAmount, LG_AMOUNT);
                        lGeneralTotalCount += 1;
                    }
                    
                    if (memcmp(TYPE_70,lrTransBuf.TransType,2)==0)//Smart (Instalment)
                    {
                        lInstalmentCount++; 
                        //AddAscii( lrTransBuf.MonthlyInstAmount, LG_AMOUNT, lInstalmentAmount, LG_AMOUNT, lInstalmentAmount, LG_AMOUNT );
                        AddAscii( lrTransBuf.EntryAmount, LG_AMOUNT, lInstalmentAmount, LG_AMOUNT, lInstalmentAmount, LG_AMOUNT );
                    }
                    else if (memcmp(TYPE_71,lrTransBuf.TransType,2)==0)//Smart
                    {
                        lLoyaltyCount++;
                        AddAscii( lrTransBuf.TotalAmount, LG_AMOUNT, lLoyaltyAmount, LG_AMOUNT, lLoyaltyAmount, LG_AMOUNT );
                    }
                    else if (memcmp(TYPE_72,lrTransBuf.TransType,2)==0)//Smart
                    {
                        lPaymentCount++;
                        AddAscii( lrTransBuf.TotalAmount, LG_AMOUNT, lPaymentAmount, LG_AMOUNT, lPaymentAmount, LG_AMOUNT );
                    }
					//added by Allen 2016-09-20	
					else if(memcmp(TYPE_76,lrTransBuf.TransType,2)==0)
					{
						lReversalCount++;
						AddAscii( lrTransBuf.TotalAmount, LG_AMOUNT, lReversalAmount, LG_AMOUNT, lReversalAmount, LG_AMOUNT );
					}
                    //					else if (memcmp(TYPE_73,lrTransBuf.TransType,2)==0)//Smart
                    //					{
                    //						lAccountMaintCount++;
                    //						AddAscii( lrTransBuf.TotalAmount, LG_AMOUNT, lAccountMaintAmount, LG_AMOUNT, lAccountMaintAmount, LG_AMOUNT );
                    //					}
                    if (fbListing)
					{
                        s_IslemDetaylari( &lrTransBuf );
					}
                }
            }while ( NumBytes > 0 );
            
            if (NumBytes == 0 )
            {
                s_IslemTipineGoreToplamlar(lInstalmentCount,lLoyaltyCount,
                                            lPaymentCount,lReversalCount,
                                            lInstalmentAmount,lLoyaltyAmount,
                                            lPaymentAmount,lReversalAmount);
            }

            PrnInit();
            //PrnStr("\n\n\n\n\n");   remove by richard 20150703
            PrnStart();
        }
    }
    close(Filename);     // add by Allen 20151116
    return(STAT_OK);
}


static WORD s_IslemDetaylari(TransStruct *fpTransInfo)
{
    BYTE lbFormatedAmount[17];
    BYTE lpLine[27];
    char    szTempBuff[LG_PRN_BUFFER];

    PrintBuffer_Init(szTempBuff, sizeof(szTempBuff));
    PrintBuffer_SelectFont(szTempBuff, 24);
    
    memset(lpLine, 0, sizeof(lpLine));
    memcpy(lpLine,fpTransInfo->SeqNum,LG_SEQNUM);
    lpLine[LG_SEQNUM]='-';
    
    if (memcmp(fpTransInfo->TransType,TYPE_70,2)==0)
    {
        memcpy(lpLine+5,PRT_LOSMARTREWARD,6);
    }
    else if (memcmp(fpTransInfo->TransType,TYPE_71,2)==0)
    {
        memcpy(lpLine+5,PRT_LOSMARTLOYALTY,6);
    }
    else if (memcmp(fpTransInfo->TransType,TYPE_72,2)==0)
    {
        memcpy(lpLine+5,PRT_LOSMARTREDEEM,6);
    }
	//added by Allen 2016-09-20
	else if (memcmp(fpTransInfo->TransType,TYPE_76,2)==0)
	{
		memcpy(lpLine+5,PRT_LOSMARTREVERSAL,9);
	}
	//add end
    //		else if (memcmp(fpTransInfo->TransType,TYPE_73,2)==0)
    //			memcpy(lpLine+5,PRT_LOSMARTACCOUNT_MAINT,4);
	//modified by Allen 2016-09-20
	if(memcmp(fpTransInfo->TransType,TYPE_76,2)==0)    
	{
		lpLine[15]='-';
		memcpy(lpLine+12,fpTransInfo->TransDate,2);
		lpLine[18]='/';
		memcpy(lpLine+15,fpTransInfo->TransDate+2,2);
		lpLine[21]='/';
		memcpy(lpLine+18,fpTransInfo->TransDate+6,2);
		lpLine[24]='-';
		memcpy(lpLine+20,fpTransInfo->TransTime, 2);
		lpLine[27]=':';
		memcpy(lpLine+28,fpTransInfo->TransTime+2,2);
		PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);	
	}
	else
	{
		lpLine[11]='-';
		memcpy(lpLine+12,fpTransInfo->TransDate,2);
		lpLine[14]='/';
		memcpy(lpLine+15,fpTransInfo->TransDate+2,2);
		lpLine[17]='/';
		memcpy(lpLine+18,fpTransInfo->TransDate+6,2);
		lpLine[20]='-';
		memcpy(lpLine+20,fpTransInfo->TransTime, 2);
		lpLine[23]=':';
		memcpy(lpLine+24,fpTransInfo->TransTime+2,2);
		PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);	
	}
	//modified by Allen 2016-09-20
    
    memset(lpLine, 0, sizeof(lpLine));
    memcpy(lpLine, (fpTransInfo->Pan),16);
    PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);
    
    //	if (memcmp(fpTransInfo->TransType,TYPE_73,2)!=0)
    //	{
    // Amount,
    memset(lbFormatedAmount,' ',17);
    if (memcmp(fpTransInfo->TransType,TYPE_70,2)==0)
        AmountSeparation(fpTransInfo->EntryAmount, lbFormatedAmount);
    else
        AmountSeparation(fpTransInfo->TotalAmount, lbFormatedAmount);
    
    memset(lpLine,' ',LG_TICKETLINE);
    memcpy(lpLine+8, lbFormatedAmount,16);
    PrintBuffer_Eng(szTempBuff, (PCHAR)lpLine);
    //	}

    PrnInit();
    PrintBuffer_SendToPrinter(szTempBuff);
	StartPrinter();
    return( STAT_OK );
}

static WORD s_PrintTransHeader( BYTE *fpTicketType )
{
    BYTE lpTime[64];
    BYTE lpLine[64];
    char    szTempBuff[LG_PRN_BUFFER];

    PrintBuffer_Init(szTempBuff, sizeof(szTempBuff));
    PrintBuffer_Eng(szTempBuff, "\n");
    PrintBuffer_SelectFont(szTempBuff, 16);   //add by richard 20150701
    
    if ( PrnHead(szTempBuff)==STAT_OK)
    {
        //		lPrtFF(1);
        //"DD-MM-YYYY         HH:MM"
        //""
        //"(PCHAR)fpTicketType"
        //""

        PrintBuffer_SelectFont(szTempBuff, 24);   //add by richard 20150701
        memset(lpLine, 0, sizeof(lpLine));
        ConvDate(lpTime); //DD-MM-YYYY
        memcpy(lpLine,lpTime,10);
        memset(lpLine+10,' ',9);
        ClkReadTime(lpTime);
        memcpy(lpLine+19,lpTime,2);
        lpLine[21]=':';
        memcpy(lpLine+22,lpTime+2,2);
        PrintBuffer_Eng(szTempBuff, "%s\n\n", (PCHAR)lpLine);
        PrintBuffer_Eng(szTempBuff, "   %s\n\n", (PCHAR)fpTicketType);
    
        PrnInit();
        PrintBuffer_SendToPrinter(szTempBuff);
		return StartPrinter();
    }
    return( STAT_INV );
}


static void s_IslemTipineGoreToplamlar(int lInstalmentCount,
                                     int lLoyaltyCount,
                                     int lPaymentCount,
                                     int lReversalCount,
                                     
                                     BYTE lInstalmentAmount[LG_AMOUNT+1],
                                     BYTE lLoyaltyAmount[LG_AMOUNT+1],
                                     BYTE lPaymentAmount[LG_AMOUNT+1],
                                     BYTE lReversalAmount[LG_AMOUNT+1]
                                     )
{    
    BYTE lbaConversion[20];
    BYTE lpLine[28];
    char    szTempBuff[LG_PRN_BUFFER];
    
    PrintBuffer_Init(szTempBuff, sizeof(szTempBuff));

    if((lInstalmentCount>0)||(lLoyaltyCount>0)||(lPaymentCount>0)/*||(lAccountMaintCount>0)*/)
    {
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)UTIL_EQUALS);
    }
    
    if(lInstalmentCount>0)
    {
        memset(lbaConversion,' ',17);
        memset(lpLine, 0, sizeof(lpLine));
        memcpy(lpLine, PRT_SALE_UNIT, 21);
        L1ConvIntAsc( lInstalmentCount, lbaConversion );
        L1CadreStr( lbaConversion, 3 );
        memcpy(lpLine+21, lbaConversion, 4);
        PubTrimTailChars(lpLine, ' ');
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);

        memset(lpLine, 0, sizeof(lpLine));
        memset(lpLine,' ',24);
        memset(lbaConversion,' ',17);
        memcpy(lbaConversion, lInstalmentAmount, LG_AMOUNT);
        AmountSeparation(lInstalmentAmount, lbaConversion);
        strcpy(lpLine+8, lbaConversion);
        PubTrimTailChars(lpLine, ' ');
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);
    }

    if(lLoyaltyCount>0)
    {
        memset(lbaConversion,' ',17);
        memcpy(lpLine, PRT_LOYALTY_UNIT, 21);
        L1ConvIntAsc( lLoyaltyCount, lbaConversion );
        L1CadreStr( lbaConversion, 3 );
        memcpy(lpLine+21, lbaConversion, 3);
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);

        memset(lpLine,' ',24);
        memset(lbaConversion,' ',17);
        memcpy( lbaConversion, lLoyaltyAmount,LG_AMOUNT );
        
        AmountSeparation(lLoyaltyAmount, lbaConversion);
        memcpy(lpLine+8, lbaConversion,16);
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);
    }

    if(lPaymentCount>0)
    {
        memset(lbaConversion,' ',17);
        memset(lpLine, 0, sizeof(lpLine));
        memcpy(lpLine, PRT_PAYMENT_UNIT, 21);
        L1ConvIntAsc( lPaymentCount, lbaConversion );
        L1CadreStr( lbaConversion, 3 );
        memcpy(lpLine+21, lbaConversion, 4);
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);

        memset(lpLine,' ',24);
        memset(lbaConversion,' ',17);
        memcpy( lbaConversion, lPaymentAmount,LG_AMOUNT );
        
        AmountSeparation(lPaymentAmount, lbaConversion);
        memcpy(lpLine+8, lbaConversion,16);
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);
    }
	//added by Allen 2016-09-20
    if(lReversalCount>0)
    {
        memset(lbaConversion,' ',17);
        memset(lpLine, 0, sizeof(lpLine));
        memcpy(lpLine, PRT_REVERSALNUIT, 21);
        L1ConvIntAsc( lReversalCount, lbaConversion );
        L1CadreStr( lbaConversion, 3 );
        memcpy(lpLine+21, lbaConversion, 4);
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);

        memset(lpLine,' ',24);
        memset(lbaConversion,' ',17);
        memcpy( lbaConversion, lReversalAmount,LG_AMOUNT );
        
        AmountSeparation(lReversalAmount, lbaConversion);
        memcpy(lpLine+8, lbaConversion,16);
        PrintBuffer_Eng(szTempBuff, "%s\n", (PCHAR)lpLine);
    }
	//add end

    PrintBuffer_Eng(szTempBuff, "\n\n\n\n");

    PrnInit();
    PrintBuffer_SendToPrinter(szTempBuff);
    StartPrinter();
}

/*
WORD PrintSlipHeader(BYTE Print)
{
    // in this app, always Print==TRUE

    HDGStruct fpToHDGStruct;
    BYTE lbLine[LG_MERCHDATA+1];
    
    if ( HDGRead( &fpToHDGStruct ) == STAT_OK )
    {
        // Merchant data (4 lines max)
        // 5th line empty
        PrnStr("%.24s\n", fpToHDGStruct.MerchData);
        PrnStr("%.24s\n", fpToHDGStruct.MerchData+24);
        PrnStr("%.24s\n", fpToHDGStruct.MerchData+48);
        PrnStr("%.24s\n\n", fpToHDGStruct.MerchData+72);
        
        // merchant ID
        PrnStr("%-12.12s    %.*s\n", PDS_MERCHANT_ID, LG_MERCHID, fpToHDGStruct.MerchId);

        // Store NO.
        PrnStr("%-12.12s    %.*s\n", PDS_STORE_NO, LG_STORENUM, fpToHDGStruct.StoreNum);

        // TERM ID
        PrnStr("%-12.12s    %.*s\n", PDS_TERMINAL_ID, LG_TERMID, fpToHDGStruct.TermId);
        
        lPrtFF(1);
        return( STAT_OK );
    }

    return( STAT_INV );
}
*/

BYTE ParameterList(void)
{
    BYTE lpLine[27];
    HDDStruct lsHDDBuf;
    HDGStruct lsHDGBuf;
    BYTE lpMaxTrans[5];
    char    szTempBuff[LG_PRN_BUFFER];
    
	PrintBuffer_Init(szTempBuff, sizeof(szTempBuff));
    PrintBuffer_SelectFont(szTempBuff, 16);
    PrnHead(szTempBuff);

    PrintBuffer_SelectFont(szTempBuff, 24);
    // Fill tickets
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
	{
        return(STAT_INV);
	}
    // Read parameters
    if (HDDRead(&lsHDDBuf)!=STAT_OK)
	{
        return(STAT_INV);
	}
    
    PrintBuffer_Eng(szTempBuff, "\n");
    // 9th line Parametre Listesi
    // 10th line empty
    PrintBuffer_Eng(szTempBuff, "%s\n\n", PRT_PARAMLIST);
    // 11th line Date & Time
    memset(lpLine, 0, sizeof(lpLine));
    GetPrnDateTime(lpLine);
    PrintBuffer_Eng(szTempBuff, "%s\n", lpLine);
    
    // 12th line
    L1ConvIntAsc(lsHDGBuf.MaxTrans, lpMaxTrans);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_MAXTRANS, lpMaxTrans);
    // 15th line
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %.*s\n", PRT_PARMVERSION, LG_PARMVERSION, lsHDGBuf.BinVersion);
    // 16th line
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_AUTH_PHONE_1, lsHDGBuf.NETSERVERTel1);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_AUTH_PHONE_2, lsHDGBuf.NETSERVERTel2);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_AUTH_PHONE_3, lsHDGBuf.NETSERVERTel3);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_PMS_PHONE_1, lsHDGBuf.PMSTel1);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_PMS_PHONE_2, lsHDGBuf.PMSTel2);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_PMS_PHONE_3, lsHDGBuf.PMSTel3);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_PARAM_PHONE_1, lsHDGBuf.TMSTel1);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_PARAM_PHONE_2, lsHDGBuf.TMSTel2);
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %s\n", PRT_PARAM_PHONE_3, lsHDGBuf.TMSTel3);
    // th line Batch Close start time
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %.2s:%.2s\n", PRT_BATCHSTART, lsHDDBuf.BCStartTime, lsHDDBuf.BCStartTime+2);
    // th line Parameter Download Start Time
    PrintBuffer_Eng(szTempBuff, "%-17.17s   %.2s:%.2s\n\n\n", PRT_PARMSTART, lsHDDBuf.ParmStartTime, lsHDDBuf.ParmStartTime+2);
    
    PrnInit();
    PrintBuffer_SendToPrinter(szTempBuff);
    return StartPrinter();
}
