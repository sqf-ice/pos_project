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
extern WORD EnterPIN(BYTE *PIN);   //added by Allen 2016-05-24
extern WORD Enter_Item_num(WORD *pwNum);
extern BYTE AskAnotherGoods(void);

int gl_iSendTagC5 = 0;
BYTE gl_ucCashAmount[12];
BYTE gl_ucCashPrintAmount[20];
BYTE gl_ucTotalPrintAmount[20];
//added by Allen for ZAIN 2016-05-13

int GetPanFromTrack(uchar *pszPAN, uchar *pszExpDate)
{
	int		iPanLen;
	char	*p, *pszTemp;


	if( strlen((char *)Magmsg.track2)>0 )
	{
		pszTemp = (char *)Magmsg.track2;
	}
	else if( strlen((char *)Magmsg.track3)>0 )
	{
		pszTemp = (char *)&Magmsg.track3[2];
	}
	else
	{	
		return -1;
	}

	p = strchr((char *)pszTemp, '=');
	if( p==NULL )
	{
		return -1;
	}
	iPanLen = p - pszTemp;
	if( iPanLen<13 || iPanLen>19 )
	{
		return -1;
	}

	sprintf((char *)pszPAN, "%.*s", iPanLen, pszTemp);
	if( pszTemp==(char *)Magmsg.track2 )
	{
		sprintf((char *)pszExpDate, "%.4s", p+1);
	}
	else
	{
		sprintf((char *)pszExpDate, "0000");
	}

	return 0;
}

//added by Allen for ZAIN 2016-05-06
int ProcMagMsg(void)
{
	int iRet;
	unsigned char TmpStr[40];
	memset(TmpStr, 0, 40);
	iRet = GetPanFromTrack(gPTStruct.Pan, gPTStruct.ExpDate);
	if(strlen(Magmsg.track2) > 0)
	{
		gPTStruct.Tr2Len = strlen(Magmsg.track2);
		memcpy(Magmsg.track2, "6396800301808335", 16);
		strcpy(gPTStruct.TrackISO2, Magmsg.track2);
		strcpy(gPTStruct.TrackI, Magmsg.track1);   //added by Allen 2016-09-19
	}
	return iRet;
}
//add end


void SaleTransaction(BYTE Manual)
{    
    HDGStruct  lsHDGStruct;
    WORD   lwSeqNum;
    BYTE   lbaWorkBuf[16];
    BYTE   lbaWorkBuf1[16];
    BYTE   Bcd[16];
    BYTE lbCount=0;
    GPRSStruct      gprs;
	Dial_upStruct   Dial;  //added by Allen 2016-05-23
	struct saveRetrivalRef stTempRef;
	int iRet = 0;
	
    if (HDGRead(&lsHDGStruct)!=STAT_OK)
    {
        DelayMs(1000);
        PF_TxnFailed();  
        return;
    }	

#ifndef ALLEN_TEST
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
            PF_TxnFailed();
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
        PF_TxnFailed();
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
	//modified by Allen 2016-05-23
	if(gConnectMode == GPRS)
	{
		if (GPRSRead(&gprs)!=STAT_OK)
		{
			return;
		}
		UCL_Gprs_Pre_Connect(gprs.apn, gprs.username, gprs.password);    //  连接PPP
	}
	else
	{
		if(Dial_upRead(&Dial) != STAT_OK)
		{
			return;
		}
		PSTNDial(DM_PREDIAL, &Dial.stPara, Dial.szTelNo, Dial.ucSendMode);
	}
	//modify end
	//OS_ClearScreen();                                        //add by Allen 20151110
	//ScrPrint(0, 0, ASCII, "Allen test");
	//ScrPrint(0, 2, ASCII, "GPRS : %d", iRet);
	//getkey();
	//OS_ClearScreen();	
#endif
    if (Manual==OS_TRUE)
    {
        if (StartApplication((PBYTE)" ")!=STAT_OK)
            return;
        //memset(gPTStruct.FirstBalance,'0',12);   //removed by Allen 2016-08-25
        //memset(gPTStruct.Balance,'0',12);     //removed by Allen 2016-08-25
        //		gPTStruct.PosEntryType=2; //1   1=CHIP 2=MANUAL
        DisplayString((PBYTE)"ONLINE TRANS.",2000);
    }
    else
    {	
        if (EMV_StartTransaction(EMV_SATIS, lwSeqNum)!=0)
        {
            PF_TxnFailed();
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
            (memcmp(gPTStruct.TransType,TYPE_71,2)==0))// || 
			//(memcmp(gPTStruct.TransType,TYPE_72,2)==0))  //add by jeff_xiehuan20161215
        {
			HDGStruct  lsHDGStructTmp;                          // add by Allen 20151117
			if (HDGRead(&lsHDGStructTmp)!=STAT_OK)
			{
				DelayMs(1000);
				PF_TxnFailed();
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

            if (HDGWrite(&lsHDGStruct)!=STAT_OK)       //这里batch id 还是为0的，这里又把0给写了进去。所以导致type70的交易每一次都要
            {												    					//重新读取batch id  
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
        AddTransaction(&gPTStruct);//只是将gPTStruct写到文件中
	//ComOutput("",0,"Add REF?purchase.c,line:%dtran type is %c%cgPTStruct.Ref=%s",__LINE__,gPTStruct.TransType[0],gPTStruct.TransType[1],gPTStruct.RefNumber);
		if(memcmp(gPTStruct.TransType,TYPE_70,2)==0 || memcmp(gPTStruct.TransType,TYPE_72,2)==0)
		{
			memset(&stTempRef,0,sizeof(struct saveRetrivalRef));
			
			if(memcmp(gPTStruct.TransType,TYPE_70,2)==0)
			{
				memcpy(stTempRef.retrivalRef,gPTStruct.RefNumber,12);
				stTempRef.tranType = ISO_VOIDREVERSAL_REDEEM;
			}
			else if(memcmp(gPTStruct.TransType,TYPE_72,2)==0)
			{
				memcpy(stTempRef.retrivalRef,gPTStruct.RetrivalRef,12);
				stTempRef.tranType = ISO_VOIDREVERSAL_REWARD;
			}
			iRet = AddRefNum(&stTempRef);//added by jeff_xiehuan
		}
		else if (memcmp(gPTStruct.TransType,TYPE_76,2)==0)
		 {
			iRet = CheckAndUpdateRetrivalRef(gPTStruct.RetrivalRef,1);//added by jeff_xiehuan,update the retRef file.
			//ComOutput("",0,"fileName:purchase.c,line:%dgReversalType is %d",__LINE__,gReversalType);
		 }
		//ComOutput(&gPTStruct,sizeof(PTStruct),"firstgPd\n");
	
		
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
    return;
}

extern WORD SelectLanguage(void);      
//extern WORD PrepareBatchSlip(BYTE Success/*BYTE *fpWrkBuf, BYTE *fpRecordCounter,BYTE *fpTotalAmount*/);

WORD Select_Calculation(void)       //貌似会输入金额然后计算成BCD码
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
    unsigned char errCode;
	BYTE tempBuf[10];
	BYTE szDispBuf[20];
	BYTE tempInputAmount[20];
	BYTE ucInputSubCashAmount[20];
	MSG_Struct stInputCashWarn;
	MSG_Struct stInputCash;
	BYTE ucCashBalance[20];
	BYTE ucCashBalanceBcd[10];
	BYTE ucGetCovAmount[20];

	//remove by lqc 20150617
//#ifdef APPL_ARABIC
//    if (SelectLanguage()!=STAT_OK)
//    {
//        return(STAT_NOK);
//    }
//#endif
    
    //TransactionType
    if (TransactionTypeMenu()!=STAT_OK)//leeDebug
    {
        return(STAT_NOK);
    }
/*
	ScrCls();
	Lcdprintf("%s %s", "TransType:", gPTStruct.TransType);//leeDebug
	getkey();
*/

#if 0  //added by jeff_xiehuan20161206
	memset(tempBuf,0,sizeof(tempBuf));
	memcpy(tempBuf,gPTStruct.TransType,2);

	ScrPrint(0,0,0,"TransType=%s",tempBuf);
	getkey();
#endif
#ifndef APPL_DEMO
    /*if (memcmp(gPTStruct.TransType,TYPE_70,2)==0)
    {*/
    gHostType = AUTH_HOST;
	//added by Allen 2016-07-04
	if(memcmp(gPTStruct.TransType,TYPE_76,2)==0)
	{
		iRet = ReversalMode();
		if(iRet != STAT_OK)
		{
			return(STAT_NOK);
		}
	}
	//add end

    if (memcmp(gPTStruct.TransType,TYPE_72,2)!=0)
    {
        //PrepareField55(OS_FALSE,OS_FALSE);
        memset(gPTStruct.MonthlyInstAmount,'0',LG_AMOUNT );
        memset(gPTStruct.EntryAmount,'0',LG_AMOUNT);
        memset(gPTStruct.TotalAmount,'0',LG_AMOUNT);
        memset(gPTStruct.BcdAmount,'\x0',6);
		memset(gPTStruct.ProductCode, '\x0', 2);
    
		if (memcmp(gPTStruct.TransType,TYPE_70,2)!=0 &&
			memcmp(gPTStruct.TransType,TYPE_75,2)!=0 && 
			memcmp(gPTStruct.TransType,TYPE_76,2)!=0)    //added by Allen for ZAIN 2016-05-18
		{
			lwStatus=ISO_SendReceive(OS_TRUE,ISO_GETBALANCE);     //  有可能是从这里近的函数
			/*ScrPrint(0,0,0,"TYpe_72 lwStatus = %d",lwStatus);
			getkey();*/
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
        //richard test print slip
/*        do
        {
           iRet = getkey();
           switch(iRet)
           {
             case KEY1:
                //PrepareBatchSlip(OS_TRUE);
                PrintTransList(OS_TRUE, OS_FALSE);
                FileInit(FILE_TR);
                PaperControl();
                Print_PAX_Slip(OS_FALSE,FILE_RB);
                break;
                break;
             case KEY2:
                strcpy(gPTStruct.TransType , TYPE_70);
                Prepare_Print_SaleOrPaymentSlip(OS_TRUE);  //OS_TRUE
                break;
             case KEY3:
                PrintParameterSlip(OS_FALSE, OS_FALSE);
                break;
             case KEY4:
                strcpy(gPTStruct.TransType , TYPE_72);
                Prepare_Print_SaleOrPaymentSlip(OS_TRUE);
                break;
             default:
                break;
           }
        
        }while(KEYCANCEL != iRet);

        //Utils_ConvertBcdToAsc("F0D10A46010350000020150714", ISOData.Field47, 13);
       //memcpy(ISOData.Field47,"\x00\x16\xF0\x0E\xD1\x0C\x46\x03\x03\x50\x00\x00\x20\x15\x07\x14\x01\x35\xD2\x0C\x46\x02\x23\x51\x11\x11\x20\x15\x07\x14\x01\x35\xD3\x0C\x46\x02\x00\x90\x76\x00\x20\x15\x07\x14\x01\x35\xD4\x0C\x46\x01\x43\x20\x00\x11\x20\x15\x07\x14\x01\x35\xD5\x0C\x46\x02\x00\x00\x00\x12\x20\x15\x07\x14\x01\x35",74);
        memcpy(ISOData.Field47,"\x00\x16\x0E",04);

        //DebugComSend("Field47:");
        for(iRet=0; iRet<74; iRet++)
        {
            DebugComSend("%x", ISOData.Field47[iRet]);
        }
        //sprintf(, "%x", );
        Parse47_MST(&errCode);
 
  */      
        BalanceList();
        return(STAT_NOK);
    }
//#ifndef ALLEN_TEST
    //Amount Entry
    //memset(gPTStruct.EntryAmount, ' ', LG_AMOUNT); // Hided by Kim_LinHB 2013-3-14 build 0.05.0009
    //not Account Maint
    //Amount Entry
    if (memcmp(gPTStruct.TransType,TYPE_73,2)==0)
    {
        memset(gPTStruct.EntryAmount,'0',LG_AMOUNT);
    }
    else if ((memcmp(gPTStruct.TransType,TYPE_72,2)==0) ||
    			(memcmp(gPTStruct.TransType,TYPE_75,2)==0) ||
				(memcmp(gPTStruct.TransType,TYPE_76,2)==0))     //modified by Allen for ZAIN 2016-07-04
    {
        ClearCustomMessages(&MainMSG);
		if((memcmp(gPTStruct.TransType,TYPE_76,2)==0))
		{
			strcpy(MainMSG.CSTMSG1, "Amount:");
		}
		else
		{
			strcpy(MainMSG.CSTMSG1, "Invoice Amount:");//modified by Allen for ZAIN 2016-06-03
		} 
	    
	    OS_ClearScreen();
        memset(Balance, 0, sizeof(Balance));
        if (!InputAmountExt(&MainMSG, Balance, 0, 1, 7, gCurrencySymbol))  //modified by Allen for ZAIN 2016-06-03
        {
		    return(STAT_NOK);
        }

        if (AmountEntry(Balance)!=STAT_OK)
        {
            return STAT_NOK;
        }
        memcpy(gPTStruct.TotalAmount, gPTStruct.EntryAmount, LG_AMOUNT);
    }
    else if(memcmp(gPTStruct.TransType,TYPE_70,2)==0)
    {
       //added by jeff_xiehuan20161206
		
		gl_iSendTagC5 = 0;
		lwStatus=ISO_SendReceive(OS_TRUE,ISO_GETBALANCE); 
		/*ScrPrint(0,2,0x00,"the lwStatus is %d",lwStatus);
		getkey();*/
		if (lwStatus!=STAT_OK)
		{
				
			Comms_Disconnect();
			return(STAT_NOK);
		}
		memset(ucGetCovAmount,0,sizeof(ucGetCovAmount));
	    memcpy(ucGetCovAmount,gPTStruct.Balance,12);
		memset(szDispBuf,0,sizeof(szDispBuf));
		PubConvAmount((uchar*)"BHD",ucGetCovAmount,3,0,szDispBuf,GA_SEPARATOR);
	/*	ScrPrint(0,0,0x00,"the balance is %s",szDispBuf);
		getkey();*/

		//memset(gPTStruct.Balance,'0',12);
		//memcpy((char*)gPTStruct.Balance + 8,"2345",4);  //for test
		//memset(ucGetCovAmount,0,sizeof(ucGetCovAmount));
		//memcpy(ucGetCovAmount,gPTStruct.Balance,12);
		//memset(szDispBuf,0,sizeof(szDispBuf));
		//PubConvAmount((uchar*)"BHD",ucGetCovAmount,3,0,szDispBuf,GA_SEPARATOR);
		if (Enter_Item_Price(gPTStruct.product[ii].price_bcd))
		{
			return STAT_NOK;
		}
		 
        AmountEntry(gPTStruct.product[ii].price_bcd);
		memset(tempInputAmount,0,sizeof(tempInputAmount));
		memcpy(tempInputAmount,gPTStruct.EntryAmount,12);

		/*OS_ClearScreen();
		ScrPrint(0,0,0x00,"EntryAmount=%s",gPTStruct.EntryAmount);
		ScrPrint(0,2,0x00,"gPTStruct.Balance=%s",gPTStruct.Balance);
		getkey();*/
#if 1
		
	   if(memcmp(gPTStruct.Balance,gPTStruct.EntryAmount,12) < 0)
	   {
		   BYTE  ucKey;
		   int i = 0;
		  // BYTE tempPrint[100];

		   ucKey = 0;
		   //ClearCustomMessages(&stInputCashWarn);
		   gl_iSendTagC5 = 1;
		   OS_ClearScreen();
		   ScrPrint(0,0,0,"%s",szDispBuf);
		   OS_ClearScreen();
		   ScrPrint(0,0,0,"The balance is only");
		   ScrPrint(0,1,1,"%s",szDispBuf);
		   ScrPrint(0,3,0,"amount available to");
		   ScrPrint(0,4,0,"redeem,do you wish to");
		   ScrPrint(0,5,0,"input cash amount");
		   ScrPrint(0,6,1,"     Y/N");
		   ucKey = GetOneKeyOne();
		   if(ucKey != 1)
		   {
			   return STAT_NOK;
		   }
		   memset(ucInputSubCashAmount,0,sizeof(ucInputSubCashAmount));
		   PubAscSub(tempInputAmount,gPTStruct.Balance,12,ucInputSubCashAmount);
			do
			{
				ClearCustomMessages(&stInputCash);
				memset(ucCashBalanceBcd,0,sizeof(ucCashBalanceBcd));
				memset(ucCashBalance,0,sizeof(ucCashBalance));
				OS_ClearScreen();
				strcpy(stInputCash.CSTMSG1,"Input cash amount");
				if (!InputAmountExt(&stInputCash, ucCashBalanceBcd, 0, 1, 7, (char*)gCurrencySymbol))
				{
					return(STAT_NOK);
				}
				Utils_ConvertBcdToAsc(ucCashBalanceBcd,ucCashBalance,6);
				
			}while( (memcmp(ucCashBalance,ucInputSubCashAmount,12) < 0) ||  (memcmp(ucCashBalance,tempInputAmount,12) >= 0));
			memset(gl_ucCashAmount,0,sizeof(gl_ucCashAmount));
			memset(gPTStruct.EntryAmount,0,sizeof(gPTStruct.EntryAmount));
			memcpy(gl_ucCashAmount,ucCashBalanceBcd,6);
			PubAscSub(tempInputAmount,ucCashBalance,12,gPTStruct.EntryAmount);


			memset(ucGetCovAmount,0,sizeof(ucGetCovAmount));  //for print receipt,added by jeff_xiehuan20161207
			memset(gl_ucCashPrintAmount,0,sizeof(gl_ucCashPrintAmount));
		    memcpy(ucGetCovAmount,ucCashBalance,12);
		    PubConvAmount((uchar*)"BHD",ucGetCovAmount,3,0,gl_ucCashPrintAmount,GA_SEPARATOR);

			memset(ucGetCovAmount,0,sizeof(ucGetCovAmount));  //for print receipt,added by jeff_xiehuan20161207
			memset(gl_ucTotalPrintAmount,0,sizeof(gl_ucTotalPrintAmount));
		    memcpy(ucGetCovAmount,tempInputAmount,12);
		    PubConvAmount((uchar*)"BHD",ucGetCovAmount,3,0,gl_ucTotalPrintAmount,GA_SEPARATOR);

	/*		memset(tempPrint,0,sizeof(tempPrint));
		   for(i = 0;i < 20;i++)
		   {
			   sprintf(tempPrint + i*3,"%02X ",*(gl_ucCashAmount + i));
		   }
		    OS_ClearScreen();
		    ScrPrint(0,2,0,"gl_ucCashAmount = %s",tempPrint);
			getkey();*/
		/*OS_ClearScreen();
		ScrPrint(0,0,0x00,"EntryAmount=%s",gPTStruct.EntryAmount);
		ScrPrint(0,2,0x00,"ucCashBalance=%s",ucCashBalance);
		getkey();*/
	   }
#endif
        //added by Allen 2016-05-24(enter PIN)
	   //This PIN is going to be sent in clear text format to the host using field ??
		if(EnterPIN(gPTStruct.PIN))
		{
			return STAT_NOK;
		}
		//DebugComSend("gPTStruct.PIN = %s\n", gPTStruct.PIN);
    }
//#endif
//added by Allen for ZAIN 2016-05-16 
	 if (memcmp(gPTStruct.TransType,TYPE_72,2)==0)
	 {
		strcpy(MainMSG.CSTMSG1, "Invoice Number:");
		if(!(InputInvoiceNum(&MainMSG, gPTStruct.InvoiceNum)))
		{
			return STAT_NOK;
		}
		//DebugComSend("InvoiceNum = %s\n", gPTStruct.InvoiceNum);
		lwStatus=ISO_SendReceive(OS_TRUE,ISO_GETBALANCE); 
#if 0  //added by jeff_xiehuan 20161207
		ScrPrint(0,2,0,"TYPE_72_Bal_Ret=%d",lwStatus);
		getkey();

#endif
		if (lwStatus!=STAT_OK)
		{
			Comms_Disconnect();
			return(STAT_NOK);
		}
	 }



	 if (memcmp(gPTStruct.TransType,TYPE_76,2)==0)
	 {
		strcpy(MainMSG.CSTMSG1, "Original Txn");
		strcpy(MainMSG.CSTMSG2, "Reference Number");
		if(!(InputReferenceNum(&MainMSG, gPTStruct.RefNumber)))
		{
			return STAT_NOK;
		}              
		  
	   // iRet = CheckRetrivalRef(gPTStruct.RefNumber);  //added by jeff_xiehuan 20161215
		iRet = CheckAndUpdateRetrivalRef(gPTStruct.RefNumber,0);
		ComOutput("",0,"fileName:purchase.c,line:%d now the tran type is %c%c",__LINE__,gPTStruct.TransType[0],gPTStruct.TransType[1]);
		if(iRet == STAT_NOK)
		{
			//ComOutput(gPTStruct.RefNumber,12,"gPTStruct.RefNumber%d",1);
			ScrCls();
			ScrPrint(0,0,1,"Unable to void");
			ScrPrint(0,2,0,"transaction");
			ScrPrint(0,4,0,"already posted");
			GetOneKey(10);
			return STAT_NOK;
		}
		else if(iRet == STAT_HAS_VOID)
		{
			ScrCls();
			ScrPrint(0,0,1,"Unable to void");
			ScrPrint(0,2,0,"transaction");
			ScrPrint(0,4,0,"already void");
			GetOneKey(10);
			return STAT_NOK;
		}
	 }
//add end 
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
		//lwStatus = (int)ISO_SendReceive(OS_TRUE, ISO_GETBALANCE);  //remove by richard 20150727 
		// Add End
//        ScrCls();
//        Lcdprintf("%s %x", "lwStatus:", lwStatus);//leeDebug
//        getkey();

        

//by jarod 2011.0627
//根据不同情况显示错误信息
/*        if (lwStatus==STAT_INV)
        {
            return(STAT_NOK);
        }
*/
//remove by richard 20150703
/*        if (lwStatus==STAT_OK)
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
        }    */   
/*
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
        }*/ 
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
    else if(memcmp(gPTStruct.TransType,TYPE_76,2)!=0)
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
	int PanLen;
    BYTE  fbLength = 18;
	int usRet;
    
    if (*FirstChar!=0)
    {
    }

	ProcMagMsg();

	
	memcpy(gPTStruct.Pan, "6396800301808335", 16);
	gPTStruct.Pan[16] = 0;

	PanLen = strlen(gPTStruct.Pan);
	Utils_ConvertAscToBcd(gPTStruct.Pan, gPTStruct.BcdPan, PanLen, 0, PAD_NONE, 0);

	Utils_ConvertAscToBcd(gPTStruct.ExpDate, gPTStruct.BcdExpDate, 4, 0, PAD_NONE, 0);


   /*    removed by Allen for ZAIN 2016-05-16   
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
	*/
    
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
    
  //  memset(gPTStruct.ExpDate,'0',4);    //moved by Allen for ZAIN 2016-05-16
    memset(gPTStruct.Balance,'9',12);
    gMerchantOperationFlags   = 0x1;//
    gPTStruct.CardBinFlags    = 0x1;//
    gCardParam.InstallmentFlag= 1;  //
    
 //   gEntryType=MANUAL_ENTRY;
	 gEntryType=SWIPE_CARD;      //modified by Allen for ZAIN 2016-05-16

    if (Select_Calculation()!=STAT_OK)
        return (STAT_NOK);
	//DebugComSend("------------------Select_Calculation completed!----------------------\n");
	//added by Allen for ZAIN 2016-05-18
	if (memcmp(gPTStruct.TransType,TYPE_70,2)==0)
	{
		gHostType=AUTH_HOST;
		//PrepareField55(OS_FALSE,OS_TRUE);	// removed by Allen 2016-05-30  Swipe card no need field55
		//DebugComSend("TYPE_70 -> ISO_RETAIL\n");
		usRet = ISO_SendReceive(OS_TRUE, ISO_RETAIL);
		if (usRet!=STAT_OK)
		{
			Comms_Disconnect();
			return ONLINE_FAILED;
		}
	}
    else if (memcmp(gPTStruct.TransType,TYPE_72,2)==0)
    {
		gHostType=AUTH_HOST;
		//PrepareField55(OS_FALSE,OS_TRUE);	// removed by Allen 2016-05-30  Swipe card no need field55
		//DebugComSend("TYPE_72  -> ISO_PAYMENT\n");
		usRet = ISO_SendReceive(OS_FALSE, ISO_PAYMENT);
		if (usRet!=STAT_OK)
		{
			Comms_Disconnect();
			return ONLINE_FAILED;
		}
    }
	else if(memcmp(gPTStruct.TransType,TYPE_76,2)==0)
	{
		gHostType=AUTH_HOST;
		//PrepareField55(OS_FALSE,OS_TRUE);	// removed by Allen 2016-05-30  Swipe card no need field55
		//DebugComSend("TYPE_76  -> ISO_VOIDREVERSAL\n");
		if(gReversalType == ISO_VOIDREVERSAL_REWARD)
		{
			usRet=ISO_SendReceive(OS_TRUE,ISO_GETBALANCE);    
			if (usRet!=STAT_OK)
			{
				Comms_Disconnect();
				return(STAT_NOK);
			}
			usRet = ISO_SendReceive(OS_FALSE, ISO_VOIDREVERSAL_REWARD);
		}
		else if(gReversalType == ISO_VOIDREVERSAL_REDEEM)
		{
			usRet = ISO_SendReceive(OS_TRUE, ISO_VOIDREVERSAL_REDEEM);
		}
		if (usRet!=STAT_OK)
		{
			Comms_Disconnect();
			return ONLINE_FAILED;
		}
	}
	//DebugComSend("------------------ISO_SendReceive completed!----------------------\n");
    // set response code
	//memcpy(psRspCode, gPTStruct.ResponseCode, 2);
	//add end
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
