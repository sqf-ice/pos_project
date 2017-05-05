#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_define.h"
#include "os_interface.h"

#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "utility.h"

#include "SLCLOCK.h"

#include "appLib.h"


static WORD BatchClose(BYTE fbFlagAuto);
static WORD SendBatchHeader_Details(void);
static WORD PrepareBatchSlip(BYTE Success);
static WORD BatchSlipWriteFile(char *pszPrnData);

BYTE StartBatchClose(void)
{
    WORD lwStatus=STAT_NOK ;
    
    gHostType = PMS_HOST;
    if (ConnectHost_New() == STAT_OK)
    {
        lwStatus = BatchClose(OS_FALSE);	
    }
    Comms_Disconnect();

    if (lwStatus!=STAT_OK) 
    {
        PrepareBatchSlip(OS_FALSE);
        PaperControl();
        Print_PAX_Slip(OS_FALSE,FILE_RB);
        return( OS_FALSE );
    }
    else
    {
		FileInit(FILE_LAST_EOD);             // add by Allen 20151116
		FileCopy(FILE_TR, FILE_LAST_EOD);     // add by Allen 20151116
        PrintTransList(OS_TRUE, OS_FALSE, FILE_TR);
        FileInit(FILE_TR);
        PaperControl();
        Print_PAX_Slip(OS_FALSE,FILE_RB);
    }

    return( OS_TRUE );
}

static WORD BatchClose(BYTE fbFlagAuto)
{
    WORD lwStatus;
    
    DisplayString_S80(DSP_BATCH_CLOSING, 0);
    lwStatus = SendBatchHeader_Details();
    return(lwStatus);
}


static WORD SendBatchHeader_Details(void)
{
    int  NumBytes;
    BYTE AttemptCount[5]={0};
    BYTE Count=0;
    BYTE lWrkBuf[10]={0};
    WORD sw;
    BYTE lCounterInstallment=0;
    BYTE lCounterLoyalty=0;
    BYTE lCounterPayment=0;
    BYTE lCounterGeneral=0;
    BYTE lCounterGeneralWithPayment=0;
    BYTE Len=0;
    
    BYTE lpWrkBuf[/*257*/1024];
    HDGStruct lsHDGBuf;
    
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
    {
        return(STAT_INV);
    }
    
    memset(gAmountDebit,   ' ', LG_AMOUNT+1);
    memset(gAmountLoyalty, ' ', LG_AMOUNT+1);
    memset(gAmountPayment, ' ', LG_AMOUNT+1);
    memset(gAmountGeneral, ' ', LG_AMOUNT+1);
    memset(gAmountGeneralWithPayment,' ',LG_AMOUNT+1);
    lCounterInstallment=0;
    lCounterLoyalty=0;
    lCounterPayment=0;
    lCounterGeneral=0;
    lCounterGeneralWithPayment=0;
    
	gTrHandle=Open(FILE_TR, O_RDWR);
    if (gTrHandle >= 0)
    {
		if ( Seek(gTrHandle, 0, SEEK_SET) >= 0 )
        {
            do
            {
                NumBytes = Read(gTrHandle, (BYTE *)&gPTStruct, LG_TRANSSTRUCT);
                if (NumBytes>0)
                {
                    if(memcmp(gPTStruct.TransType,TYPE_70,2)==0)//INSTALLMENT
                    {
                        AddAscii(gPTStruct.EntryAmount, LG_AMOUNT, gAmountDebit, LG_AMOUNT, gAmountDebit, LG_AMOUNT);
                        lCounterInstallment++;
                        AddAscii(gPTStruct.EntryAmount, LG_AMOUNT, gAmountGeneral, LG_AMOUNT, gAmountGeneral, LG_AMOUNT);
                        AddAscii(gPTStruct.EntryAmount, LG_AMOUNT, gAmountGeneralWithPayment, LG_AMOUNT, gAmountGeneralWithPayment, LG_AMOUNT);
                        lCounterGeneral++;
                        lCounterGeneralWithPayment++;
                    }
                    if(memcmp(gPTStruct.TransType,TYPE_71,2)==0)//LOYALTY
                    {
                        AddAscii(gPTStruct.TotalAmount, LG_AMOUNT, gAmountDebit, LG_AMOUNT, gAmountDebit, LG_AMOUNT);
                        lCounterInstallment++;
                        AddAscii(gPTStruct.TotalAmount, LG_AMOUNT, gAmountLoyalty, LG_AMOUNT, gAmountLoyalty, LG_AMOUNT);
                        lCounterLoyalty++;
                        AddAscii(gPTStruct.TotalAmount, LG_AMOUNT, gAmountGeneral, LG_AMOUNT, gAmountGeneral, LG_AMOUNT);
                        AddAscii(gPTStruct.TotalAmount, LG_AMOUNT, gAmountGeneralWithPayment, LG_AMOUNT, gAmountGeneralWithPayment, LG_AMOUNT);
                        lCounterGeneral++;
                        lCounterGeneralWithPayment++;
                    }
                    if(memcmp(gPTStruct.TransType,TYPE_72,2)==0)//PAYMENT
                    {
                        AddAscii(gPTStruct.TotalAmount, LG_AMOUNT, gAmountPayment, LG_AMOUNT, gAmountPayment, LG_AMOUNT);
                        lCounterPayment++;
                        AddAscii(gPTStruct.TotalAmount, LG_AMOUNT, gAmountGeneralWithPayment, LG_AMOUNT, gAmountGeneralWithPayment, LG_AMOUNT);
                        lCounterGeneralWithPayment++;
                    }
                }
            }
            while ( NumBytes > 0 );
        }
        Close(gTrHandle);
    }

    gAmountDebit[12]=0;
    StrTrim(gAmountDebit,gAmountDebit,' ');
    StrLeftPad(gAmountDebit,12,gAmountDebit,'0');
    itoa(lCounterInstallment,(PCHAR)lWrkBuf, 10);
    lWrkBuf[3]=0;
    StrTrim(lWrkBuf,lWrkBuf,' ');
   // StrLeftPad(lWrkBuf,3+1,gCounterDebit,'0');
   StrLeftPad(gCounterDebit,3,lWrkBuf,'0');
    
    gAmountLoyalty[12]=0;
    StrTrim(gAmountLoyalty,gAmountLoyalty,' ');
    StrLeftPad(gAmountLoyalty,12,gAmountLoyalty,'0');
    itoa(lCounterLoyalty,    (PCHAR)lWrkBuf, 10);
    lWrkBuf[3]=0;
    StrTrim(lWrkBuf,lWrkBuf,' ');
    StrLeftPad(lWrkBuf,3+1,gCounterLoyalty,'0');
    
    gAmountPayment[12]=0;
    StrTrim(gAmountPayment,gAmountPayment,' ');
    StrLeftPad(gAmountPayment,12,gAmountPayment,'0');
	memset(lWrkBuf, 0, sizeof(lWrkBuf));
    itoa(lCounterPayment,   (PCHAR) lWrkBuf, 10);
    lWrkBuf[3]=0;
    StrTrim(lWrkBuf,lWrkBuf,' ');
   // StrLeftPad(lWrkBuf,3+1,gCounterPayment,'0');
   StrLeftPad(gCounterPayment,3,lWrkBuf,'0');
    
    gAmountGeneral[12]=0;
    StrTrim(gAmountGeneral,gAmountGeneral,' ');
    StrLeftPad(gAmountGeneral,12,gAmountGeneral,'0');
    lCounterGeneral++;//For Rec.Adv.
    itoa(lCounterGeneral,    (PCHAR)lWrkBuf, 10);
    lWrkBuf[3]=0;
    StrTrim(lWrkBuf,lWrkBuf,' ');
    StrLeftPad(lWrkBuf,3+1,gCounterGeneral,'0');
    
    gAmountGeneralWithPayment[12]=0;
    StrTrim(gAmountGeneralWithPayment,gAmountGeneralWithPayment,' ');
    StrLeftPad(gAmountGeneralWithPayment,12,gAmountGeneralWithPayment,'0');
    lCounterGeneralWithPayment++;//For Rec.Adv.
    itoa(lCounterGeneralWithPayment,(PCHAR)lWrkBuf,10);
    lWrkBuf[3]=0;
    StrTrim(lWrkBuf,lWrkBuf,' ');
    StrLeftPad(lWrkBuf,3+1,gCounterGeneralWithPayment,'0');
    
    
    if(lCounterPayment>0)
    {
        //RECONCILIATION
        sw = ISO_SendReceive(OS_FALSE,ISO_RECONCILIATION);
        if(sw==STAT_NOK)
        {
            return(STAT_NOK);
        }

        //FOR PMS SECOND CONNECT
        Comms_Disconnect();
        if (ConnectHost_New() != STAT_OK)
        {
            return(STAT_NOK);
        }
    }
    else
    {
        sw = STAT_NOK; //Response Code OK -- FULL
    }
    
    if ((lCounterGeneral>1) || (sw!=STAT_OK))//Send FULL if There are only Payments(ISO_RECONCILIATION Not Approved)
    {
		gTrHandle = Open(FILE_TR, O_RDWR);
        if (gTrHandle >= 0)
        {
            if (Seek(gTrHandle, 0, SEEK_SET) >= 0)
            {
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Len=0;
                //NII-PMS
                memset(lpWrkBuf,0,/*255*/1024);
                memcpy(lpWrkBuf+Len,"\x60",1);
                Len=Len+1;
                memcpy(lpWrkBuf+Len,gPMSNII,2);
                Len=Len+2;
                memcpy(lpWrkBuf+Len,"\x00\x00\x01\x50\x56\x3A\x31\x30\x3B\x4D\x53\x3A\x31\x30\x32\x34\x3B\x4E\x4E\x3A",20);
                Len=Len+20;;
                memcpy(lpWrkBuf+Len,lsHDGBuf.TermId,8);
                Len=Len+8;
                memcpy(lpWrkBuf+Len,"\x3B\x4E\x4E\x3B",4);
                Len=Len+4;
                SendNChar(lpWrkBuf,Len);
//#ifdef OUTPUT_ISO8583
//#endif
if(gDebug)
{
                PubDebugOutput("lCounterGeneral>1, send:", lpWrkBuf, Len, DEVICE_PRN, HEX_MODE);
}
                
                memset(lpWrkBuf, 0, /*255*/1024);
                if(ReceiveStr(lpWrkBuf,MAX_RECV_TIMER)!=STAT_OK)
                {
                    Close(gTrHandle);
                    return(STAT_INV);
                }

                //MK For ACK Ignore
                if(lpWrkBuf[5]==5)
                {
                    memset(lpWrkBuf, 0, /*255*/1024);
                    if(ReceiveStr(lpWrkBuf,MAX_RECV_TIMER)!=STAT_OK)
                    {
                        Close(gTrHandle);
                        return(STAT_INV);
                    }
                }

                if (!((lpWrkBuf[5]==1)&&
                    (lpWrkBuf[6]=='N')&&
                    (lpWrkBuf[7]=='N')))
                {
                    Close(gTrHandle);
                    return(STAT_INV);
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Len=0;
                memset(lpWrkBuf,0,/*255*/1024);
                //NII-PMS
                memcpy(lpWrkBuf+Len,"\x60",1);
                Len=Len+1;
                memcpy(lpWrkBuf+Len,gPMSNII,2);
                Len=Len+2;
                memcpy(lpWrkBuf+Len,"\x00\x00",2);
                Len=Len+2;
                if(sw==STAT_OK)//Response Code OK -- PARTIAL
                    memcpy(lpWrkBuf+Len,"\x01SB:3:",6);
                else //-- FULL
                    memcpy(lpWrkBuf+Len,"\x01SB:2:",6);
                Len=Len+6;
                
                memcpy(lpWrkBuf+Len,lsHDGBuf.BatchNum,6);
                Len=Len+6;
                memcpy(lpWrkBuf+Len,":",1);
                Len=Len+1;
                memcpy(lpWrkBuf+Len,lsHDGBuf.TermId,8);
                Len=Len+8;
                memcpy(lpWrkBuf+Len,";",1);
                Len=Len+1;
                SendNChar(lpWrkBuf,Len);
                if (gDebug)
                    Utils_DebugPrint((PCHAR)"Batch Request(SB)->:",lpWrkBuf,Len);
                
                memset(lpWrkBuf,0,/*255*/1024);
                if(ReceiveStr(lpWrkBuf,MAX_RECV_TIMER)!=STAT_OK)
                {
                    Close(gTrHandle);
                    return(STAT_INV);
                }
                
                if(lpWrkBuf[5]!=5)
                {
                    Close(gTrHandle);
                    return(STAT_INV);
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                do
                {
                    NumBytes = Read( gTrHandle, (BYTE *)&gPTStruct, LG_TRANSSTRUCT);
                    if ( NumBytes>0)
                    {
                        Count++;
                        itoa(Count,(PCHAR)AttemptCount, 10);
                        //AttemptCount[3];
                        StrTrim(AttemptCount,AttemptCount,' ');
                        StrLeftPad(AttemptCount,3+1,AttemptCount,'0');
                        AttemptCount[3] = 0;
                        OS_DisplayStr(3, 13, AttemptCount);
                        if (sw==STAT_OK)//Response Code OK -- PARTIAL
                        {
                            if(memcmp(gPTStruct.TransType,TYPE_70,2)==0)//INSTALLMENT
                            {
                                PrepareField55(OS_TRUE,OS_TRUE);
                                if(ISO_SendReceive(OS_FALSE,ISO_INSTALLMENT_ADVICES)!=STAT_OK)
                                {
                                    Close(gTrHandle);
                                    return(STAT_NOK);
                                }
                            }
                            if(memcmp(gPTStruct.TransType,TYPE_71,2)==0)//LOYALTY
                            {
                                if(ISO_SendReceive(OS_FALSE,ISO_LOYALTY_ADVICE)!=STAT_OK)
                                {
                                    Close(gTrHandle);
                                    return(STAT_NOK);
                                }
                            }
                        }
                        else//Response Code NOK -- FULL
                        {
                            if(memcmp(gPTStruct.TransType,TYPE_72,2)==0)//PAYMENT
                            {
                                if(ISO_SendReceive(OS_FALSE,ISO_BATCH_UPLOAD_ADVICE)!=STAT_OK)
                                {
                                    Close(gTrHandle);
                                    return(STAT_NOK);
                                }
                            }
                            if(memcmp(gPTStruct.TransType,TYPE_70,2)==0)//INSTALLMENT
                            {
                                PrepareField55(OS_TRUE,OS_TRUE);
                                if(ISO_SendReceive(OS_FALSE,ISO_INSTALLMENT_ADVICES)!=STAT_OK)
                                {
                                    Close(gTrHandle);
                                    return(STAT_NOK);
                                }
                            }
                            if(memcmp(gPTStruct.TransType,TYPE_71,2)==0)//LOYALTY
                            {
                                if(ISO_SendReceive(OS_FALSE,ISO_LOYALTY_ADVICE)!=STAT_OK)
                                {
                                    Close(gTrHandle);
                                    return(STAT_NOK);
                                }
                            }
                        }
                        ////////////////////////////////////////////////////////////
                        ////////////////////////////////////////////////////////////
                        ////////////////////////////////////////////////////////////
                    }
                }
                while ( NumBytes > 0 );                
            }
            Close(gTrHandle);
        }
        
        Count++;
        itoa(Count,(PCHAR)AttemptCount, 10);
        //AttemptCount[3];
        StrTrim(AttemptCount,AttemptCount,' ');
        StrLeftPad(AttemptCount,3+1,AttemptCount,'0');
        AttemptCount[3] = 0;
        OS_DisplayStr(3, 13, AttemptCount);
        if(ISO_SendReceive(OS_FALSE,ISO_RECONCILIATION_ADVICES)!=STAT_OK)
        {
            return(STAT_NOK);
        }

        Len=0;
        memset(lpWrkBuf,0,/*255*/1024);
        //NII-PMS
        memcpy(lpWrkBuf+Len,"\x60",1);Len=Len+1;
        memcpy(lpWrkBuf+Len,gPMSNII,2);Len=Len+2;
        memcpy(lpWrkBuf+Len,"\x00\x00\x01",3);Len=Len+3;
        memcpy(lpWrkBuf+Len,"EB:",3);Len=Len+3;
        memcpy(lpWrkBuf+Len,lsHDGBuf.BatchNum,6);Len=Len+6;//Batch Id
        memcpy(lpWrkBuf+Len,":",1);Len=Len+1;
        if(sw==STAT_OK)//Response Code OK -- PARTIAL
        {
            memcpy(lpWrkBuf+Len,gCounterGeneral,3);Len=Len+3;
        }
        else
        {
            memcpy(lpWrkBuf+Len,gCounterGeneralWithPayment,3);Len=Len+3;
        }
        
        memcpy(lpWrkBuf+Len,":",1);Len=Len+1;
        
        if(sw==STAT_OK)//Response Code OK -- PARTIAL
        {
            memcpy(lpWrkBuf+Len,gAmountGeneral,(12-gCurrencyExponent));Len=Len+(12-gCurrencyExponent);
            memcpy(lpWrkBuf+Len,".",1);Len=Len+1;
            memcpy(lpWrkBuf+Len,gAmountGeneral+(12-gCurrencyExponent),gCurrencyExponent);Len=Len+gCurrencyExponent;
        }
        else
        {
            memcpy(lpWrkBuf+Len,gAmountGeneralWithPayment,(12-gCurrencyExponent));Len=Len+(12-gCurrencyExponent);
            memcpy(lpWrkBuf+Len,".",1);Len=Len+1;
            memcpy(lpWrkBuf+Len,gAmountGeneralWithPayment+(12-gCurrencyExponent),gCurrencyExponent);Len=Len+gCurrencyExponent;
        }
        memcpy(lpWrkBuf+Len,"\x3B",1);Len=Len+1;
        SendNChar(lpWrkBuf,Len);
        if (gDebug)
            Utils_DebugPrint((PCHAR)"End Of Batch(EB):->",lpWrkBuf,Len);

        memset(lpWrkBuf,0,/*255*/1024);
        if(ReceiveStr(lpWrkBuf,MAX_RECV_TIMER)!=STAT_OK)
            return(STAT_INV);
        if(lpWrkBuf[5]!=5)
            return(STAT_INV);
        
        Len=0;
        memset(lpWrkBuf,0,/*255*/1024);
        //NII-PMS
        memcpy(lpWrkBuf+Len,"\x60",1);Len=Len+1;
        memcpy(lpWrkBuf+Len,gPMSNII,2);Len=Len+2;
        memcpy(lpWrkBuf+Len,"\x00\x00\x01\x02",4);Len=Len+4;
        SendNChar(lpWrkBuf,Len);
        if (gDebug)
            Utils_DebugPrint((PCHAR)"Disconnect:->",lpWrkBuf,Len);
        
        //MK For ACK Ignore
        ReceiveStr(lpWrkBuf, MAX_RECV_TIMER);        
    }

    PrepareBatchSlip(OS_TRUE);
    //Update Batch Number
    memset(gTempBuf,' ',6+1);
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
        return(STAT_INV);
    
    AddAscii(lsHDGBuf.BatchNum, 6, (PCHAR)"000001", 6, gTempBuf, 6);
    gTempBuf[6]=0;
    StrTrim(gTempBuf,gTempBuf,' ');
    StrLeftPad(gTempBuf,6+1,gTempBuf,'0');
    
    memcpy( lsHDGBuf.BatchNum, gTempBuf+1, 6);
    
    if (HDGWrite(&lsHDGBuf)!=STAT_OK)
        return(STAT_NOK);
    
    DelayMs(1000);
    
    return(STAT_OK);
}

static WORD PrepareBatchSlip(BYTE Success/*BYTE *fpWrkBuf, BYTE *fpRecordCounter,BYTE *fpTotalAmount*/)
{
    HDGStruct lsHDGBuf;
    //BYTE lpTemp[27];
    BYTE lpDate[13], lpTime[13];
	BYTE szBuffer[128];
    char    szTempBuff[LG_PRN_BUFFER];
    int  Count=0;
    
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
    {
        return(STAT_INV);
    }

    PrintBuffer_Init(szTempBuff, sizeof(szTempBuff));
    if(!Success)       //modify by Allen  20151118
    {
        PrintBuffer_SelectFont(szTempBuff, 16);
		PrnHead(szTempBuff);
		PrintBuffer_Eng(szTempBuff, "\n");
    }
    PrintBuffer_SelectFont(szTempBuff, 24);

	sprintf(szBuffer, "%s %s %.6s", PRT_BATYPE, PRT_NO, lsHDGBuf.BatchNum);
	PrintBuffer_Eng(szTempBuff, "%s\n\n", szBuffer);
    
    // line Date and Time
	// DD/MM/YY HH:MM
    ClkReadDate(lpDate,F_DDMMYY);
	ClkReadTime(lpTime);
	sprintf(szBuffer, "%.2s/%.2s/%.2s %.2s:%.2s", lpDate, lpDate+2, lpDate+4, lpTime, lpTime+2);
	PrintBuffer_Eng(szTempBuff, "%s\n\n", szBuffer);

    if(Success==OS_TRUE)
        StrCpy(szBuffer, (BYTE *)" BATCH CLOSE SUCCESSFUL \n");
    else
        StrCpy(szBuffer, (BYTE *)"BATCH CLOSE UNSUCCESSFUL\n");
	PrintBuffer_Eng(szTempBuff, "%s\n", szBuffer);

    PrintBuffer_Eng(szTempBuff, "*******************************");
	PrintBuffer_Eng(szTempBuff, "                \n");
	PrintBuffer_Eng(szTempBuff, "                \n");
	PrintBuffer_Eng(szTempBuff, "                \n");
	PrintBuffer_Eng(szTempBuff, "                \n");

    BatchSlipWriteFile(szTempBuff);
    return(STAT_OK);
}

static WORD BatchSlipWriteFile(char *pszPrnData)
{
    int lbHandler;
    
    if ( FileInit(FILE_RB) == STAT_INIT_OK )
    {
        lbHandler=Open(FILE_RB, O_RDWR);
        if (lbHandler >= 0)
        {
            if (Write(lbHandler, pszPrnData, strlen(pszPrnData))==0)
            {
                Close(lbHandler);
                return ( STAT_OK );
            }
            Close(lbHandler);
        }
    }
    
    return ( STAT_NOK );
}

// end of file
