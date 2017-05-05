#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "appLib.h"
#include "os_define.h"
#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "iso.h"
#include "COMMON.h"
#include "vxlibs.h"
#include "utility.h"

static WORD StartApplication(BYTE *FirstChar);
extern void Disp_Mess(BYTE Type,BYTE Timeout);
extern BYTE Enter_data(BYTE *chTemp, BYTE Type);
extern WORD Enter_prod_type(BYTE *psProdType);
extern WORD Enter_scan_barcode(BYTE *psBarcode);
extern WORD Enter_Item_Price(BYTE *psPriceBcd);
extern WORD Enter_Item_num(WORD *pwNum);
extern BYTE AskAnotherGoods(void);

void SaleTransaction(BYTE Manual)
{
    HDGStruct  lsHDGStruct;
    WORD   lwSeqNum;
    BYTE   lbaWorkBuf[16];
    BYTE   lbaWorkBuf1[16];
    BYTE   Bcd[16];
    BYTE lbCount=0;
    GPRSStruct      gprs;

    if (HDGRead(&lsHDGStruct)!=STAT_OK)
    {
        DelayMs(1000);
        PF_CardRemove();
        return;
    }

#ifndef APPL_DEMO
    if (memcmp(lsHDGStruct.BatchNum,"000000",6)==0)
    {
#ifdef DEVELOP_VERSION
        memcpy(lsHDGStruct.BatchNum, "000001", 6);
        HDGWrite(&lsHDGStruct);
#else
        if (GetBatchNumber(OS_TRUE)!=STAT_OK)
        {
            DelayMs(1000);
            PF_CardRemove();
            return;
        }
#endif
    }
#endif
	//memcpy(lsHDGStruct.BatchNum, "000012", 6);
    //    HDGWrite(&lsHDGStruct);
    // Terminal Number
    memcpy(gPTStruct.TerminalID, lsHDGStruct.TermId, LG_TERMID); // 8
    // Sequence number update
    NextSequenceNo(&lwSeqNum);
    
    if ( lwSeqNum > gMaxTranaction )
    {
        OS_Beep(500);
        DisplayString(DSP_TR_FILE_FULL,1000);
        DelayMs(1000);
        PF_CardRemove();
        return;
    }

    //Date
    ConvDate(lbaWorkBuf);
    memcpy(gPTStruct.TransDate,   lbaWorkBuf,   2);
    memcpy(gPTStruct.TransDate+2, lbaWorkBuf+3, 2);
    memcpy(gPTStruct.TransDate+4, lbaWorkBuf+6, 4);
    //Time
    ClkReadTime(lbaWorkBuf);
    memcpy(gPTStruct.TransTime,lbaWorkBuf,6);
    
    //Language Menu
    /*	if (Language()!=STAT_OK)
    return;
    if (gLanguage==ARABIC)	 {
    OS_LogTo(DEBUG_PORT,"step_Disp_Long_Msg"); 			
    Disp_Long_Msg(1);		
    }	
    */
    if (GPRSRead(&gprs)!=STAT_OK)
    {
        return;
    }
    UCL_Gprs_Pre_Connect(gprs.apn, gprs.username, gprs.password);

    if (Manual==OS_TRUE)
    {
        if (StartApplication((PBYTE)" ")!=STAT_OK)
            return;
        memset(gPTStruct.FirstBalance,'0',12);
        memset(gPTStruct.Balance,'0',12);
        //		gPTStruct.PosEntryType=2; //1   1=CHIP 2=MANUAL
        DisplayString((PBYTE)"ONLINE TRANS.",2000);
    }
    else
    {
        if (EMV_StartTransaction(EMV_SATIS, lwSeqNum)!=0)
        {
            PF_CardRemove();
            return;
        }
        //		gPTStruct.PosEntryType=1; //1   1=CHIP 2=MANUAL
    }

    //SeqNum
    L1ConvIntAsc(lwSeqNum, lbaWorkBuf);
    L1CadreStr(lbaWorkBuf, LG_SEQNUM);
    memcpy(gPTStruct.SeqNum, lbaWorkBuf+1, LG_SEQNUM);
    if (memcmp(gPTStruct.TransType,TYPE_73,2)!=0)
    {
        if ((memcmp(gPTStruct.TransType,TYPE_70,2)==0)||
            (memcmp(gPTStruct.TransType,TYPE_71,2)==0))
        {
			HDGStruct  lsHDGStructTmp;                          // add by Allen 20151117
			if (HDGRead(&lsHDGStructTmp)!=STAT_OK)
			{
				DelayMs(1000);
				PF_CardRemove();
				return;
			}	
			memcpy(lsHDGStruct.BatchNum, lsHDGStructTmp.BatchNum, 6);    //end
            if (lsHDGStruct.TraceNum>9999)
            {
                lsHDGStruct.TraceNum=0;
            }
            else
            {
                lsHDGStruct.TraceNum++;
            }

            if (HDGWrite(&lsHDGStruct)!=STAT_OK)
            {
                HDGWrite(&lsHDGStruct);
            }
            L1ConvIntAsc(lsHDGStruct.TraceNum, lbaWorkBuf);
            lbCount= StrLen(lbaWorkBuf);
            memset(lbaWorkBuf1,'0',6);
            memcpy(lbaWorkBuf1+(6-lbCount),lbaWorkBuf,lbCount);
            memcpy(gPTStruct.RefNumber, gPTStruct.RetrivalRef, 12);
            memcpy(gPTStruct.RetrivalRef,lsHDGStruct.BatchNum,6);
            memcpy(gPTStruct.RetrivalRef+6,lbaWorkBuf1,6);
            lbaWorkBuf1[6]=0;
            
            StrAscToBcd(lbaWorkBuf1, Bcd);
            memcpy(gPTStruct.Stan,Bcd,3);
        }
        AddTransaction(&gPTStruct);
        Prepare_Print_SaleOrPaymentSlip(OS_TRUE);
    }
    else
    {
        if (memcmp(gPTStruct.ResponseCode,"00",2)==0)
        {
            Prepare_Print_SaleOrPaymentSlip(OS_FALSE);
            DisplayString((PBYTE)"ACCOUNT MAINT OK",0);
            OS_Beep(1500);
        }
    }

    OS_Beep(500);
    PF_CardRemove();
    return;
}

extern WORD SelectLanguage(void);

WORD Select_Calculation(void)
{
	int iRet;
    BYTE lbaWorkBuf[16];
    BYTE Tutar[12];
    WORD lwStatus;
    BYTE lpLine[40];
    BYTE Balance[20];
	BYTE MaxAmount[20];
	BYTE buffer[12];
    int ii, cnt, cnt2,lbHandler,ret;
    MSG_Struct MainMSG;

#ifdef APPL_ARABIC
    if (SelectLanguage()!=STAT_OK)
    {
        return(STAT_NOK);
    }
#endif
    
    //TransactionType
    if (TransactionTypeMenu()!=STAT_OK)
    {
        return(STAT_NOK);
    }

//     if (gLanguage==ARABIC)
//     {
//         OS_LogTo(DEBUG_PORT,"step_Disp_Long_Msg"); 			
//         Disp_Long_Msg(1);		
//     }	
    
    // ????
#ifndef APPL_DEMO
    /*if (memcmp(gPTStruct.TransType,TYPE_70,2)==0)
    {*/
    gHostType = AUTH_HOST;
    if (memcmp(gPTStruct.TransType,TYPE_72,2)!=0)
    {
        PrepareField55(OS_FALSE,OS_FALSE);
        memset(gPTStruct.MonthlyInstAmount,'0',LG_AMOUNT );
        memset(gPTStruct.EntryAmount,'0',LG_AMOUNT);
        memset(gPTStruct.TotalAmount,'0',LG_AMOUNT);
        memset(gPTStruct.BcdAmount,'\x0',6);
		memset(gPTStruct.ProductCode, '\x0', 2);
    
		if (memcmp(gPTStruct.TransType,TYPE_70,2)!=0 &&
			memcmp(gPTStruct.TransType,TYPE_75,2)!=0)
		{
			lwStatus=ISO_SendReceive(OS_TRUE,ISO_GETBALANCE);
			if (lwStatus!=STAT_OK)
			{
				Comms_Disconnect();
				return(STAT_NOK);
			}
			Comms_Disconnect();
		}
    }
    
    //get balance
    //}
#endif

    if (memcmp(gPTStruct.TransType,TYPE_74,2)==0)
    {
        BalanceList();
        return(STAT_NOK);
    }

    //Amount Entry
    //memset(gPTStruct.EntryAmount, ' ', LG_AMOUNT); // Hided by Kim_LinHB 2013-3-14 build 0.05.0009
    //not Account Maint
    //Amount Entry
    if (memcmp(gPTStruct.TransType,TYPE_73,2)==0)
    {
        memset(gPTStruct.EntryAmount,'0',LG_AMOUNT);
    }
    else if ((memcmp(gPTStruct.TransType,TYPE_72,2)==0) ||
    			(memcmp(gPTStruct.TransType,TYPE_75,2)==0))
    {
        ClearCustomMessages(&MainMSG);
        strcpy(MainMSG.CSTMSG1, "ENTER PAY AMOUNT");
	    
	    OS_ClearScreen();
        memset(Balance, 0, sizeof(Balance));
        if (!InputAmountExt(&MainMSG, Balance, 0, 1, 9, gCurrencySymbol))
        {
		    return(STAT_NOK);
        }

        if (AmountEntry(Balance)!=STAT_OK)
        {
            return STAT_NOK;
        }
        PubBcd2Asc(Balance, LG_AMOUNT/2, gPTStruct.EntryAmount);
        memcpy(gPTStruct.TotalAmount, gPTStruct.EntryAmount, LG_AMOUNT);
    }
    else
    {
        // 2011-3-24
        ii = 0;
        while (1)
        {
            //if (!Enter_data(gPTStruct.product[ii].prodinfo, 6))
            //{
            //    return STAT_NOK;
            //}            
            if (Enter_prod_type(gPTStruct.product[ii].proddesc))
            {
                return(STAT_NOK);
            }            
            if (Enter_scan_barcode(gPTStruct.product[ii].barcode))
            {
                return STAT_NOK;
            }
            if (Enter_Item_Price(gPTStruct.product[ii].price_bcd))
            {
                return STAT_NOK;
            }
            if (Enter_Item_num(&gPTStruct.product[ii].numbers))
            {
                return STAT_NOK;
            }

            ii++;
            
            if (ii>=NUM_PRODUCTS_PERTRAN)
            {
                break;
            }
            if (!AskAnotherGoods())
            {
                break;
            }
        }
                
        memset(Balance, 0, sizeof(Balance));
        for (cnt=0; cnt<ii; cnt++)
        {
            for (cnt2=0; cnt2<(int)gPTStruct.product[cnt].numbers; cnt2++)
            {
                PubBcdAdd(gPTStruct.product[cnt].price_bcd, Balance, 6, Balance);
            }
        }

        AmountEntry(Balance);
    }

    //Loyalty               
    if (memcmp(gPTStruct.TransType,TYPE_71,2)==0)
    {
        memcpy(gPTStruct.TotalAmount,gPTStruct.EntryAmount,12);
    }

    //Installment
    if ((memcmp(gPTStruct.TransType,TYPE_70,2)==0) ||
    		(memcmp(gPTStruct.TransType,TYPE_75,2)==0))
    {
        //Read Available  Installment
		// lwStatus = ReadAvailableInstallment(); // Hided by Kim_LinHB 2013-3-18 build 0.05.0009
		// Added by Kim_LinHB 2013-3-13 build 0.05.0009
		lwStatus = (int)ISO_SendReceive(OS_TRUE, ISO_GETBALANCE); 
		// Add End

//by jarod 2011.0627
//根据不同情况显示错误信息
        if (lwStatus==STAT_INV)
        {
            return(STAT_NOK);
        }
        
        if (lwStatus==STAT_OK)
        {
            //Scheme Menu
			// Hided by Kim_LinHB 2013-3-19 build 0.05.0009
//             if (SchemeMenu()!=STAT_OK)
//             {
//                 return(STAT_NOK);
//             }
                        
            //Calculate Installment Amount and Display Menu
            lwStatus = Calculate_DisplayMenu();

            if (lwStatus==STAT_INV)
            {
                return(STAT_NOK);
            }
        }
        
        //ReadAvailableInstallment = STAT_NOK yada 
        //Calculate_DisplayMenu = STAT_NOK Account Maint Islemi
        if (lwStatus!=STAT_OK)
        {
            // If Ins.Balance, Trans Type Acc.Maint.
			AvailableInstallmentStruct lStruct;

			lbHandler = filesize(FILE_PE);
			lbHandler = Open( FILE_PE, O_RDWR);
			ScrCls();
            ScrPrint(0, 0, CFONT, DSP_W_Entered_Amount_Maximum);
			if (lbHandler >= 0)
			{
				do
				{
					ret = Read( lbHandler, (BYTE *)&lStruct, LG_AVAILABLEINSTALLMENTSTRUCT);
				} while (ret);
				memset(MaxAmount,0,sizeof(MaxAmount));
				MaxAmount[0]='0';
				MaxAmount[1]='0';
				StrBcdToAsc(5,lStruct.FinishAmount,MaxAmount);
				MaxAmount[12]=0;

				memset(lpLine,0,sizeof(lpLine));
				FormatAmt(lpLine, MaxAmount, gCurrencySymbol, ASCII_IN);
				if (StrLen(lpLine)>16)
				{
					ScrPrint(0, 6, CFONT, "%16.16s", lpLine);
				} 
				else
				{
					ScrPrint(0, 7, ASCII, "%21.21s", lpLine);
				}

			}
            DelayMs(10000);
            return(STAT_NOK); //!!!sel???	
            
            memcpy(gPTStruct.TransType,TYPE_73,2);
            memset(gPTStruct.MonthlyInstAmount,'0',LG_AMOUNT );
            memset(gPTStruct.EntryAmount,'0',LG_AMOUNT);
            memset(gPTStruct.TotalAmount,'0',LG_AMOUNT);
            memset(gPTStruct.BcdAmount,'\x0',6);
            return(STAT_OK);            
        }
    }
    
    //Installment + Loyalty
    /*!!!sel??? product menu	
    if ((memcmp(gPTStruct.TransType,TYPE_70,2)==0)||
    (memcmp(gPTStruct.TransType,TYPE_71,2)==0))
    {
    
      //Select Product
      if (ProductMenu()!=STAT_OK)
      {
      return(STAT_NOK);
      }
      //Are You Sure ?
      if (AreYouSure()!=STAT_OK)
      {
      return(STAT_NOK);
      }
      }
    */	
    if (memcmp(gPTStruct.TransType,TYPE_70,2)==0)
    {
        memcpy(lbaWorkBuf, gPTStruct.MonthlyInstAmount, LG_AMOUNT);
    }
    else
    {
        memcpy(lbaWorkBuf,gPTStruct.EntryAmount,LG_AMOUNT);
        memcpy(gPTStruct.TotalAmount,gPTStruct.EntryAmount,LG_AMOUNT);
    }
    
    lbaWorkBuf[LG_AMOUNT] = 0;
    StrTrim(lbaWorkBuf, lbaWorkBuf, ' ');
    StrLeftPad(lbaWorkBuf,LG_AMOUNT,lbaWorkBuf,'0');
    lbaWorkBuf[LG_AMOUNT] = 0;

    memset(Tutar, 0, sizeof(Tutar));
    Utils_ConvertAscToBcd(lbaWorkBuf, Tutar, LG_AMOUNT, 0, PAD_NONE,0);	
    memcpy(gPTStruct.BcdAmount, Tutar, LG_AMOUNT/2);
    
    return (STAT_OK);
}

WORD AlBarakaMerchantConfirm(BYTE step)
{
    int iTotalGoods, iNowPage;
    BYTE    ucKey;
    char    szBuffer[64];

    for (iTotalGoods=0; iTotalGoods<sizeof(gPTStruct.product)/sizeof(PRODUCT); iTotalGoods++)
    {
        if (gPTStruct.product[iTotalGoods].numbers==0)
        {
            break;
        }
    }
    if (iTotalGoods<1)
    {
        return STAT_NOK;
    }

    
    iNowPage = 1;
    while (1)
    {
        OS_ClearScreen();
        if (step==1)
        {
            sprintf(szBuffer, "%.19s", gPTStruct.MerchData+50);
            PubTrimHeadChars(szBuffer, ' ');
            PubTrimTailChars(szBuffer, ' ');
            ScrPrint(0, 0, ASCII, "AlBaraka like to buy");
            ScrPrint(0, 1, ASCII, "below items from");
            ScrPrint(0, 2, ASCII, "[%.19s]", szBuffer);
        } 
        else if (step==2)
        {
			//MerchData Format as Follow:
			//         PROCCO                 PAX Dev.                Merchant Name             TEST    
            sprintf(szBuffer, "%.19s", gPTStruct.MerchData+50);
            PubTrimHeadChars(szBuffer, ' ');
            PubTrimTailChars(szBuffer, ' ');
            ScrPrint(0, 0, ASCII, "[%.19s]", szBuffer);
            ScrPrint(0, 1, ASCII, "would like to sell");
            ScrPrint(0, 2, ASCII, "following to AlBaraka");
        } 
        else
        {
            sprintf(szBuffer, "%.19s", gPTStruct.name_surname);
            PubTrimHeadChars(szBuffer, ' ');
            PubTrimTailChars(szBuffer, ' ');
            ScrPrint(0, 0, ASCII, "AlBaraka like to sell");
            ScrPrint(0, 1, ASCII, "below items to");
            ScrPrint(0, 2, ASCII, "[%.19s]", szBuffer);
        }
        ScrPrint(0, 3, ASCII, "TYPE:%16.16s",    gPTStruct.product[iNowPage-1].proddesc);
        ScrPrint(0, 4, ASCII, "CODE:   %13.13s", gPTStruct.product[iNowPage-1].barcode);
        memset(szBuffer, 0, sizeof(szBuffer));
        FormatAmt(szBuffer, gPTStruct.product[iNowPage-1].price_bcd, gCurrencySymbol, BCD_IN);
        PubTrimHeadChars(szBuffer, ' ');
        ScrPrint(0, 5, ASCII, "PRICE:%15.15s", szBuffer);
        sprintf(szBuffer, "%d", (int)gPTStruct.product[iNowPage-1].numbers);
        ScrPrint(0, 6, ASCII, "TOTAL NUMBERS:%7.7s", szBuffer);
        if (iNowPage==iTotalGoods)
        {
            // the "confirm" display and acceptance only available in end page.
            ScrPrint(0, 7, ASCII, "%21.21s", "1-ACCEPT 2-REJECT");
            ScrPrint(0, 7, ASCII, "%d/%d", iNowPage, iTotalGoods);
        }
        else
        {
            ScrPrint(0, 7, ASCII, "%21.21s", "ENTER:NEXT");
            ScrPrint(0, 7, ASCII, "%d/%d", iNowPage, iTotalGoods);
        }

        ucKey = GetOneKey(60);
        if ((ucKey==KEYENTER) || (ucKey==KEYDOWN))
        {
            if (iNowPage==iTotalGoods)
            {
                iNowPage = 1;
            }
            else
            {
                iNowPage++;
            }
        }
        if (ucKey==KEYUP)
        {
            if (iNowPage==1)
            {
                iNowPage = iTotalGoods;
            }
            else
            {
                iNowPage--;
            }
        }
        if ((ucKey==KEY1) || (ucKey==KEY2))
        {
            // the "confirm" display and acceptance only available in end page.
            if (iNowPage==iTotalGoods)
            {
                return ((ucKey==KEY1) ? STAT_OK : STAT_NOK);
            }
        }
        if ((ucKey==KEYCANCEL) || (ucKey==NOKEY))
        {
            return STAT_NOK;
        }
    }
    
}

static WORD StartApplication(BYTE *FirstChar)
{
    BYTE  Buffer[20+1];
    BYTE  fbLength = 18;
    
    if (*FirstChar!=0)
    {
    }
    
    if ((!(gOperationFlagsSmart1 & SC_TERMINAL_ACTIVE  ))||
        (!(gOperationFlagsSmart1 & SC_MANUAL_CARD_ENTRY)))
    {
        DisplayString(DSP_TERMINAL_NOT_ALLOWED, 4000);
        return (STAT_NOK);
    }
    
    Buffer[0] = 0;
    fbLength = 18;
    if (KeyBoardDataCapture(DSP_PAN, IN_PHONE, 16, &fbLength, Buffer, 200, 0) != INPUT_OK)
        return (STAT_NOK);
    
    Buffer[18] = 0;
    if (!(StrIsDigit(Buffer)))
    {
        DisplayString(DSP_W_0_9, 3000);
        return (STAT_NOK);
    }
    memcpy(gPTStruct.Pan,Buffer,18);
    gPTStruct.Pan[18]=0;
    
    Buffer[0] = 0;
    fbLength = 3;
    if (KeyBoardDataCapture(DSP_CVC, IN_PHONE, 3, &fbLength, Buffer, 200, 0) != INPUT_OK)
        return (STAT_NOK);
    
    Buffer[3] = 0;
    if (!(StrIsDigit(Buffer)))
    {
        DisplayString(DSP_W_0_9, 3000);
        return (STAT_NOK);
    }
    memcpy(gPTStruct.Cvv,Buffer,3);
    
// Gary : delete blacklist processing
//     if ( Check_BlackList()==STAT_OK)
//         return (STAT_NOK);
    
    if (BinCheck()!=STAT_OK)
    {
        DisplayString(DSP_WITH_PROCCO_CARD, 3000);
        return (STAT_NOK);
    }
    else
    {
        if ( AlgLuhnKey(StrLen(gPTStruct.Pan), gPTStruct.Pan) != gPTStruct.Pan[StrLen(gPTStruct.Pan)-1] )
        {
            DisplayString(DSP_LUHN_ERROR, 3000);
            return (STAT_NOK);
        }
    }
    if (ReadMerchantId()!=STAT_OK)
    {
        DisplayString(DSP_W_UNAVAILABLE_MERCHANT_ID, 3000);
        return (STAT_NOK);
    }
    
    memset(gPTStruct.ExpDate,'0',4);
    memset(gPTStruct.Balance,'9',12);
    gMerchantOperationFlags   = 0x1;//
    gPTStruct.CardBinFlags    = 0x1;//
    gCardParam.InstallmentFlag= 1;  //
    
    gEntryType=MANUAL_ENTRY;
    if (Select_Calculation()!=STAT_OK)
        return (STAT_NOK);
    return (STAT_OK);
}

WORD GetBatchNumber(BYTE Ask)
{
    BYTE Len=0;
    BYTE lpWrkBuf[/*257*/1024];
    HDGStruct lsHDGBuf;
    HDGStruct lsHDGBuff;
    BYTE lpDisplay[21] = "";
    /*#ifdef APPL_TEST
    return( STAT_OK );
#endif*/
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
        return(STAT_INV);
    
    if (Ask==OS_TRUE)
    {
        if (KeyBoardValid((PBYTE)"  ! CAUTION !   ",(PBYTE)"  REQUESTING BATCH ", (PBYTE)" NUMBER FROM PMS", (PBYTE)"CANCEL",0, (PBYTE)" OK ", KEYB_VAL_MASK|KEYB_CANCEL_MASK, 20) == KEYCANCEL)
            return(STAT_NOK);
    }
    gHostType=PMS_HOST;
    if (ConnectHost_New() == STAT_OK)
    {
    	ScrFontSet(CFONT);
        DisplayString((PBYTE)"     LOADING.   ",0);
        Len=0;
        memset(lpWrkBuf,0,/*255*/1024);
        //NII-PMS
        memcpy(lpWrkBuf+Len,"\x60",1);Len=Len+1;

        memcpy(lpWrkBuf+Len,gPMSNII,2);Len=Len+2;
        memcpy(lpWrkBuf+Len,"\x00\x00\x01",3);Len=Len+3;
        //memcpy(lpWrkBuf+Len,"\x60\x00\x05\x00\x00\x01",6);Len=Len+6;
        memcpy(lpWrkBuf+Len,"BI:",3);Len=Len+3;
        memcpy(lpWrkBuf+Len,lsHDGBuf.TermId,8);Len=Len+8;
        memcpy(lpWrkBuf+Len,";",1);Len=Len+1;
        
        SendNChar(lpWrkBuf,Len);
        if (gDebug) Utils_DebugPrint((PBYTE)"Get Batch Id(BI)->:",lpWrkBuf,Len);
        memset(lpWrkBuf,0,/*255*/1024);
        if (ReceiveStr(lpWrkBuf,MAX_RECV_TIMER)!=STAT_OK)
        {
            DisplayString((PBYTE)" RECEIVE ERROR  ", 5);    // Gary add
            Comms_Disconnect();
            return(STAT_INV);
        }
        
                    if (gDebug) Utils_DebugPrint((PBYTE)"Current Batch<-",lpWrkBuf,Len);

        if ((lpWrkBuf[5]==1)&&
            (lpWrkBuf[6]=='B')&&
            (lpWrkBuf[7]=='I')&&
            (lpWrkBuf[8]==':'))
        {
            //Comms_Disconnect();
            if (lpWrkBuf[10]==';') Len=1;
            else if (lpWrkBuf[11]==';') Len=2;
            else if (lpWrkBuf[12]==';') Len=3;
            else if (lpWrkBuf[13]==';') Len=4;
            else if (lpWrkBuf[14]==';') Len=5;
            else if (lpWrkBuf[15]==';') Len=6;
            
            memset(gTempBuf,' ',6+1);
            memcpy(gTempBuf,lpWrkBuf+9,Len);
            gTempBuf[6]=0;
            StrTrim(gTempBuf,gTempBuf,' ');
            StrLeftPad(gTempBuf,6,gTempBuf,'0');
            memcpy( lsHDGBuf.BatchNum, gTempBuf, 6);
            if (HDGWrite(&lsHDGBuf)!=STAT_OK)
                DisplayString((PBYTE)"  UNSUCCESSFUL  ",0);
            //return(STAT_NOK);


			if (HDGRead(&lsHDGBuff)!=STAT_OK)
				return(STAT_INV);
            
			
			DisplayString((PBYTE)"   SUCCESSFUL   ",0);
            sprintf(lpDisplay, "BATCH NUMBER: %.6s", lsHDGBuf.BatchNum);
            OS_DisplayStr(6, STR_CENTER, lpDisplay);
            OS_Beep(1500);
        }
        Len=0;
        memset(lpWrkBuf,0,/*255*/1024);
        //NII-PMS
        memcpy(lpWrkBuf+Len,"\x60",1);Len=Len+1;
        memcpy(lpWrkBuf+Len,gPMSNII,2);Len=Len+2;
        memcpy(lpWrkBuf+Len,"\x00\x00\x01\x02",4);Len=Len+4;
        memcpy(lpWrkBuf+Len,"\x60\x00\x05\x00\x00\x01\x02",7);Len=Len+7;
        SendNChar(lpWrkBuf,Len);
                                if (gDebug) Utils_DebugPrint((PBYTE)"Disconnect:->",lpWrkBuf,Len);
        Comms_Disconnect();
        return( STAT_OK );
    }

    OS_ClearScreen();
    DisplayString((PBYTE)" CONNECT FAILED ", 5);    // Gary add
    Comms_Disconnect();
    
    return( STAT_NOK );
}
