#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "appLib.h"
#include "os_define.h"
#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "utility.h"
#include "iso.h"
#include "SLCLOCK.h"
#include "utility.h"

ISOFieldDescriptor Fields[64]=
{
//1///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_NONE,    FIELD_NOT_USED ,          0 ,                 0        ,              0 },  // #01   Extended BitMap
	{TYPE_BCD   ,  VARIABLE_SIZE   ,  19 ,  sendData.Field2  ,   receivedData.Field2 } ,  // #02   Primary Account Number
	{TYPE_BCD   ,  FIXED_SIZE      ,   6 ,  sendData.Field3  ,   receivedData.Field3 } ,  // #03   Processing Code
	{TYPE_BCD   ,  FIXED_SIZE      ,  12 ,  sendData.Field4  ,   receivedData.Field4 } ,  // #04   Transaction Amount
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #05   Settlement Amount
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #06   CardHolder Billing Amount
	{TYPE_BCD   ,  FIXED_SIZE      ,  10 ,  sendData.Field7  ,   receivedData.Field7 } ,  // #07   Transmission DTS
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #08   CardHolder Billing Fee
//2///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #09   Settlement Conversion Rate
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #10   CardHolder Billing Conv. Rate
	{TYPE_BCD   ,  FIXED_SIZE      ,   6 , sendData.Field11  ,  receivedData.Field11 } ,  // #11   System Trace Audit Number
	{TYPE_BCD   ,  FIXED_SIZE      ,   6 , sendData.Field12  ,  receivedData.Field12 } ,  // #12   Local Transaction Time (HHMMSS)
	{TYPE_BCD   ,  FIXED_SIZE      ,   4 , sendData.Field13  ,  receivedData.Field13 } ,  // #13   Local Transaction Date (DDMM)
	{TYPE_BCD   ,  FIXED_SIZE      ,   4 , sendData.Field14  ,  receivedData.Field14 } ,  // #14   Expiration Date (YYMM)
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #15   Settlement Date (MMDD)
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #16   Conversion Date (MMDD)
//3///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #17   Capture Date (MMDD)
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #18   Merchant Type (Category Code)
	{TYPE_BCD   ,  FIXED_SIZE      ,   3 , sendData.Field19  ,  receivedData.Field19 } ,  // #19   Acquiring Ins. Country Code
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #20   PAN Extended Country Code
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #21   Forwarding Ins. Country Code
	{TYPE_BCD   ,  FIXED_SIZE      ,   3 , sendData.Field22  ,  receivedData.Field22 } ,  // #22   POS Entry Mode
	{TYPE_BCD   ,  FIXED_SIZE      ,   3 , sendData.Field23  ,  receivedData.Field23 } ,  // #23   Card Sequence Number
	{TYPE_BCD   ,  FIXED_SIZE      ,   3 , sendData.Field24  ,  receivedData.Field24 } ,  // #24   NII
//4///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_BCD   ,  FIXED_SIZE      ,   2 , sendData.Field25  ,  receivedData.Field25 } ,  // #25   POS Condition Code
	{TYPE_BCD   ,  FIXED_SIZE      ,   2 , sendData.Field26  ,  receivedData.Field26 } ,  // #26   PIN Capture Code
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #27   Auth. ID Response Length
	{TYPE_2ASCII,  FIXED_SIZE      ,   9 , sendData.Field28  ,  receivedData.Field28	} ,  // #28   Transaction Fee
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #29   Settlement Fee
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #30   Transaction Processing Fee
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #31   Settlement Processing Fee
	{TYPE_BCD   ,  VARIABLE_SIZE	,	11 , sendData.Field32  ,  receivedData.Field32  } ,  // #32   Acquirer Identification Code
//5///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #33   Forwarder Identification Code
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #34   PAN Extended
	{TYPE_1ASCII,  VARIABLE_SIZE   ,  37 , sendData.Field35  ,  receivedData.Field35 } ,  // #35   Track II Data
//	{TYPE_2ASCII,  VARIABLE_SIZE   ,  37 , sendData.Field35  ,  receivedData.Field35 } ,  // #35   Track II Data
//	{TYPE_BCD   ,  VARIABLE_SIZE   ,  37 , sendData.Field35  ,  receivedData.Field35 } ,  // #35   Track II Data
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #36   Track III Data*
	{TYPE_2ASCII,  FIXED_SIZE      ,  12 , sendData.Field37  ,  receivedData.Field37 } ,  // #37   Retrieval Reference Number
	{TYPE_2ASCII,  FIXED_SIZE      ,   6 , sendData.Field38  ,  receivedData.Field38 } ,  // #38   Authoriztion ID Response
	{TYPE_2ASCII,  FIXED_SIZE      ,   2 , sendData.Field39  ,  receivedData.Field39 } ,  // #39   Response Code
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #40   Service Restriction Code
//6///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_2ASCII,  FIXED_SIZE      ,   8 , sendData.Field41  ,  receivedData.Field41 } ,  // #41   Terminal ID
	{TYPE_2ASCII,  FIXED_SIZE      ,  15 , sendData.Field42  ,  receivedData.Field42 } ,  // #42   Card Acceptor ID
	{TYPE_2ASCII,  FIXED_SIZE      ,  40 , sendData.Field43  ,  receivedData.Field43 } ,  // #43   Card Acceptor Name/Location
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #44   Additional Response Data*
	{TYPE_2ASCII,  FIXED_SIZE	   ,  76 , sendData.Field45  ,  receivedData.Field45  } ,  // #45   Track I Data*
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #46   Additional Data ISO*
	{TYPE_2ASCII,  VARIABLE_SIZE   , 230 , sendData.Field47  ,  receivedData.Field47 } ,  // #47   Additional Data National*
	{TYPE_2ASCII,  VARIABLE_SIZE   , 230 , sendData.Field48  ,  receivedData.Field48 } ,  // #48   Additional Data Private*
//7///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_BCD   ,  FIXED_SIZE      ,   3 , sendData.Field49  ,  receivedData.Field49 } ,  // #49   Transaction Currency Code
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #50   Settlement Currency Code
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #51   CardHolder Billing Currency Code
	{TYPE_BINARY,  FIXED_SIZE      ,  64 , sendData.Field52  ,  receivedData.Field52 } ,  // #52   PIN Data
	{TYPE_BCD   ,  FIXED_SIZE	   ,  16 , sendData.Field53  ,  receivedData.Field53  } ,  // #53   Security Control Info
	{TYPE_2ASCII  ,  VARIABLE_SIZE        ,   120 ,  sendData.Field54 ,  receivedData.Field54 } ,  // #54   Additional Amounts
	{TYPE_2ASCII,  VARIABLE_SIZE   , 230 , sendData.Field55  ,  receivedData.Field55 } ,  // #55   Reserved ISO*
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #56   Reserved ISO*
//8///////////////////////////////////////////////////////////////////////////////////////
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #57   Reserved National*
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,              0 } ,  // #58   HSM Data
	{TYPE_2ASCII,  VARIABLE_SIZE   , 230 , sendData.Field59  ,  receivedData.Field59 } ,  // #59   Reserved National*
	{TYPE_2ASCII,  VARIABLE_SIZE   , 230 , sendData.Field60  ,  receivedData.Field60 } ,  // #60   Reserved Private*
	{TYPE_NONE  ,  FIELD_NOT_USED        ,   0 ,                       0  ,             0  } ,  // #61   Reserved Private*
	{TYPE_2ASCII,  VARIABLE_SIZE   , 230 , sendData.Field62  ,  receivedData.Field62 } ,  // #62   Reserved Private*
	{TYPE_2ASCII,  VARIABLE_SIZE   , 230 , sendData.Field63  ,  receivedData.Field63 } ,  // #63   Reserved Private*
	{TYPE_BINARY,  FIXED_SIZE	   ,  32 , sendData.Field64  ,  receivedData.Field64  } ,  // #64   MAC
};

//NII
ISOTransactionsDescriptor Transactions[11]=
{                                               //Trans Type                                        //HSM
//PAYMENT----------------NET SERVER
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x02,0x00},{0x24,0x00,0x00},{0x72,0x24,0x04,0x80,0x20,0x80,0x80,0x00} },// PAYMENT
//ACCONT MAINT-----------NET SERVER
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x01,0x00},{0x00,0x00,0x00},{0x72,0x24,0x04,0x80,0x20,0x80,0x80,0x02} },// ACCOUNT MAINT
//ACCONT MAINT ADVICE----NET SERVER
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x01,0x20},{0x91,0x00,0x00},{0x72,0x24,0x04,0x80,0x08,0x80,0x80,0x02} },// ACCOUNT MAINT ADVICE
//REVERSAL---------------NET SERVER
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x04,0x00},{0x00,0x00,0x00},{0x70,0x24,0x25,0x80,0x00,0xE0,0x80,0x04} },// REVERSAL'da Currency Code ˜‡in
//RECONCILIATION---------NET SERVER
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x05,0x00},{0x92,0x00,0x00},{0x22,/*0x20*/0x38,0x04,0x80,0x00,0x80,0x00,0x12} },// RECONCILIATION
//BATCH UPLOAD-----------PMS                                      
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x03,0x20},{0x24,0x00,0x00},{0x70,0x3C,0x04,0x80,0x08,0x80,0x80,0x10} },// BATCH UPLOAD
//INSTALLMENT ADVICE-----PMS
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x02,0x20},{0x00,0x00,0x00},{0x70,0x3C,0x04,0x80,0x0A,0x82,0x82,0x00} },// INSTALLMENT ADVICE
//LOYALTY ADVICE---------PMS
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x02,0x20},{0x16,0x00,0x00},{0x70,0x3C,0x04,0x80,0x08,0x82,0x80,0x00} },// LOYALTY ADVICE
//RECONCILIATION ADVICE--PMS
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x05,0x20},{0x92,0x00,0x00},{0x20,0x38,0x20,0x80,0x00,0x80,0x00,0x12} }, // RECONCILIATION ADVICE
//BALANCE
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x01,0x00},{0x30,0x00,0x00},{0x72,0x2C,0x04,0x80,0x20,0x82,0x80,0x02} },// BALANCE INQ // Modified by Kim_LinHB 2013-3-13 build 0.05.0009 add F47
//RETAIL
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x02,0x00},{0x00,0x00,0x00},{0x72,0x24,0x04,0x80,0x20,0x82,0x80,0x02} },// 

};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
unsigned char ISO_Transaction (BYTE trType);
unsigned char ISO_GetReversal (void);
unsigned char ISO_AddReversal (void);
unsigned char ISO_AddField    (unsigned char **frameLocator ,short int fieldNo ,unsigned char *data);
unsigned char ISO_GetField    (unsigned char **frameLocator ,short int fieldNo, unsigned char *data);
int			  ISO_BuildFrame  (BYTE trType);
int           ISO_ResolveFrame(void);
unsigned char ISO_Send        (int ,unsigned char, unsigned char *,BYTE trType);
unsigned char ISO_Receive     (short int, unsigned char, unsigned char * );
WORD          ScriptParser    (void);
void ParseF54(void);
int Parse47(/*the arguments are uncertain for now*/unsigned char *err_code); // Added by Kim_LinHB 2013-3-5 build 0.05.0009

//by jarod 2011.06.01
void ISO_DisPlayErrMsg(byte*Field_Attr)
{
	unsigned long Recp_Code;
	int i;

	//add by Allen   20151104
	if (ISOData.Field39[0] == '0' && ISOData.Field39[1] == '0'){
		return ;
	}

	Recp_Code=(ISOData.Field39[1] & 0xF0) >>4;
	Recp_Code*=16;
	Recp_Code+=ISOData.Field39[0] & 0x0F;
	Recp_Code-=48;

	for(i=0;i<10;i++)
	{
		if(gHostErrMsgTableFlag[i]==Recp_Code)
		{
			Print_HostErr_Slip((BYTE *)gHostErrMsgForDisp[i]);
			DisplayLongString((BYTE *)gHostErrMsgForDisp[i],2000*100);
			ScrCls();
		}
	}
}

WORD ISO_SendReceive(BYTE Connect,BYTE Type)
{
	int iRet, length;
    HDGStruct lrHDGBuf;
    BYTE lbTmpTotal[25];
    BYTE lbTmpTotal1[25];
    BYTE Bcd[73];
    int  lbCount;
    BYTE TmpStr[40];
	BYTE tmp[25] = {0};
	BYTE Bcd_tmp[20] = {0};
    
    HDGRead(&lrHDGBuf);
    
    //////Trace Number----------------------------------------------------------------------
    if(lrHDGBuf.TraceNum>9999)
        lrHDGBuf.TraceNum=0;
    else
        lrHDGBuf.TraceNum++;
    
    if(Type!=ISO_BATCH_UPLOAD_ADVICE)
    {
        if (HDGWrite(&lrHDGBuf)!=STAT_OK)
        {
            return(STAT_NOK);
        }
    }

    ///////////////////////////////////////////////////
    ///Card Number
    ///////////////////////////////////////////////////
	//memcpy(ISOData.Field2,gPTStruct.BcdPan,10);
	//Modified by Allen 2016-08-25
	length = strlen(gPTStruct.Pan);
	sprintf(tmp, "%d%s", length, gPTStruct.Pan);
	Utils_ConvertAscToBcd(tmp, Bcd_tmp, strlen(tmp), 0, PAD_NONE,0);
	memcpy(ISOData.Field2, Bcd_tmp,strlen(Bcd_tmp));
	//end
    ///////////////////////////////////////////////////
    //////Amount----------------------------------------------------------------------
    ///////////////////////////////////////////////////
    memcpy(lbTmpTotal,gPTStruct.EntryAmount,LG_AMOUNT);
    lbTmpTotal[LG_AMOUNT] = 0;
    Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 12, 0, PAD_NONE,0);
    memcpy(gPTStruct.BcdAmount, Bcd, 6);
	//memcpy(ISOData.Field4,gPTStruct.BcdAmount,6); // Hided by Kim_LinHB 2013-3-4 build 0.05.0009
	// Added by Kim_LinHB 2013-3-4 build 0.05.0009
	if (Type==ISO_GETBALANCE)	
	{
		memcpy(ISOData.Field4,"\x00\x00\x00\x00\x00\x00",6);
	}
	else
	{
		memcpy(ISOData.Field4,gPTStruct.BcdAmount,6);
	}
	// Add End
    
	
    ///////////////////////////////////////////////////
    ///Date and Time-------------------------------------------------------------
    ///////////////////////////////////////////////////
    if((Type==ISO_INSTALLMENT_ADVICES)||
       (Type==ISO_LOYALTY_ADVICE)||
       (Type==ISO_BATCH_UPLOAD_ADVICE)) //Orginal Date Time
    {
        memcpy(lbTmpTotal,gPTStruct.TransDate+2,2);
        memcpy(lbTmpTotal+2,gPTStruct.TransDate  ,2);
        memcpy(lbTmpTotal+4  ,gPTStruct.TransDate+6,2);
        lbTmpTotal[6]=0;
        Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 6, 0, PAD_NONE,0);
        //StrAscToBcd(lbTmpTotal, Bcd);
    }
    else
    {
        ClkReadDate(lbTmpTotal,F_MMDDYY);
        lbTmpTotal[6]=0;
        Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 6, 0, PAD_NONE,0);	
        //StrAscToBcd(lbTmpTotal, Bcd);
    }

    memcpy(ISOData.Field7,Bcd,2);
	if((Type==ISO_BATCH_UPLOAD_ADVICE)) //Orginal Date Time
	{
	   memcpy(ISOData.Field13,gPTStruct.BcdDateTime,2);   //
	}
	else
    {
		memcpy(ISOData.Field13,Bcd,2);   //
    }
		
    ///////////////////////////////////////////////////
    ///Year
    ///////////////////////////////////////////////////
	ISOData.year=Bcd[2];

	ClkReadTime(lbTmpTotal);
	lbTmpTotal[6]=0;
	Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 6, 0, PAD_NONE,0);	
	//StrAscToBcd(lbTmpTotal, Bcd);

	if((Type==ISO_INSTALLMENT_ADVICES)||
	   (Type==ISO_LOYALTY_ADVICE)||
	   (Type==ISO_BATCH_UPLOAD_ADVICE)) //Orginal Date Time
	{
		memcpy(lbTmpTotal,gPTStruct.TransTime,6);
		lbTmpTotal[6]=0;
		Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 6, 0, PAD_NONE,0);	
		//StrAscToBcd(lbTmpTotal, Bcd);
		memcpy(ISOData.Field7+2,Bcd,5);
	}
	else
	{
		memcpy(ISOData.Field7+2,Bcd,3);
	}


	if((Type==ISO_BATCH_UPLOAD_ADVICE)) //Orginal Date Time
	{
		memcpy(ISOData.Field12,gPTStruct.BcdDateTime+2,3);   //
	}
	else
    {
		memcpy(ISOData.Field12,Bcd,3);   //
    }
		
    ///////////////////////////////////////////////////
    ///Stan Number   
    ///////////////////////////////////////////////////
	if(Type!=ISO_BATCH_UPLOAD_ADVICE) //Orginal Stan
	{
		L1ConvIntAsc(lrHDGBuf.TraceNum, lbTmpTotal);
		lbCount= StrLen(lbTmpTotal);
		memset(lbTmpTotal1,'0',6);
		memcpy(lbTmpTotal1+(6-lbCount),lbTmpTotal,lbCount);
		lbTmpTotal1[6]=0;
		Utils_ConvertAscToBcd(lbTmpTotal1, Bcd, 6, 0, PAD_NONE,0);	
		//StrAscToBcd(lbTmpTotal1, Bcd);
		memcpy(gPTStruct.Stan,Bcd,3);
	}
	memcpy(ISOData.Field11,gPTStruct.Stan,3);
	
    ///////////////////////////////////////////////////
    ///Exp Date
    ///////////////////////////////////////////////////
	memcpy(ISOData.Field14,gPTStruct.BcdExpDate,2);
	
    ///////////////////////////////////////////////////
    ///Country Code
    ///////////////////////////////////////////////////
	memcpy(ISOData.Field19,gPTStruct.CountryCode,2);
	
    ///////////////////////////////////////////////////
    ///Pos Entry Mode -----------------------------------------------------------
    ///////////////////////////////////////////////////
	if((Type==ISO_PAYMENT)||
	   (Type==ISO_BATCH_UPLOAD_ADVICE)||
	   (Type==ISO_LOYALTY_ADVICE)||
	   (Type==ISO_ACCOUNTMAINTENANCE)||
	   (Type==ISO_ACCOUNTMAINTENANCE_ADVICE) ||
	   (Type==ISO_GETBALANCE)
// 	   ||
// 	   (Type==ISO_RETAIL)      // Hided by Kim_LinHB 2013-3-4 build 0.05.0009
	   )
    {
		//memcpy(ISOData.Field22,"\x00\x52",2);
		memcpy(ISOData.Field22,"\x09\x02",2);   //modified by Allen 2016-09-06 902 means full track read , SBT.
    }

	if((Type==ISO_INSTALLMENT_ADVICES)||
	   (Type==ISO_RECONCILIATION_ADVICES)
	   ||						//
	   (Type==ISO_RETAIL)		// Added by Kim_LinHB 2013-3-4 build 0.05.0009
	   )
    {
		//memcpy(ISOData.Field22,"\x00\x51",2);
		memcpy(ISOData.Field22,"\x09\x01",2);    //modified by Allen 2016-09-06
    }
	else if(Type==ISO_RECONCILIATION)
    {
		memcpy(ISOData.Field22,"\x00\x12",2);
    }
		
    ///////////////////////////////////////////////////
    ///Card Sequence Number-----------------------------------------------------------------------
    ///////////////////////////////////////////////////
	//removed by Allen 2016-08-25  swipe card no need field 23
	/*
    memcpy(ISOData.Field23,"\x00\x03",2);
    ISOData.Field23[1]=gPTStruct.PSN;
   */
    ///////////////////////////////////////////////////
    ///NII----------------------------------------------------------------------------------------
    ///////////////////////////////////////////////////

	//memcpy(ISOData.Field24,"\x00\x03",2);
	//by jarod 20110803
	memcpy(ISOData.Field24,gNETSERVERNII,2);

	///////////////////////////////////////////////////
    ///Pos Condition Code-------------------------------------------------------------
    ///////////////////////////////////////////////////
	memcpy(ISOData.Field25,"\x00\x00",2);
    ///////////////////////////////////////////////////
    ///Track2 Data------------------------------------------------------------
    ///////////////////////////////////////////////////
	
    sprintf(lbTmpTotal,"%02d",gPTStruct.Tr2Len);
    /*lbTmpTotal[0]=(((lbCount)/10)+'0');
    lbTmpTotal[1]=(((lbCount)%10)+'0');
    lbTmpTotal[2]=0;*/
	
	
    Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 2, 0, PAD_NONE,0);	
    //StrAscToBcd(lbTmpTotal, Bcd);
    
    //Utils_ConvertBcdToAsc(gPTStruct.TrackISO2,TmpStr,19);		
    //TmpStr[16]=0x3D;
    
    ISOData.Field35[0]= Bcd[0];
    memcpy(ISOData.Field35+1,gPTStruct.TrackISO2, gPTStruct.Tr2Len);
	
    ///////////////////////////////////////////////////
    ////Retrival Ref---------------------------------------------------------------------
    ///////////////////////////////////////////////////
	memcpy(ISOData.Field37,gPTStruct.RetrivalRef,12);

    ///////////////////////////////////////////////////
    ////Response Code--------------------------------------------------------------------
    ///////////////////////////////////////////////////
	memcpy(ISOData.Field39,"Y1",2);

    ///////////////////////////////////////////////////
    ////Terminal Id----------------------------------------------------------------------
    ///////////////////////////////////////////////////
	memcpy(ISOData.Field41,lrHDGBuf.TermId,8);

    ///////////////////////////////////////////////////
    ////ICC Related Data-----------------------------------------------------------------
    ///////////////////////////////////////////////////
	if(Type==ISO_LOYALTY_ADVICE)
	{
		lbCount=0;
		memcpy(ISOData.Field47+lbCount,"\x00\x14", 2);     lbCount=lbCount+2;
		memcpy(ISOData.Field47+lbCount,"\xE2\x0C", 2);     lbCount=lbCount+2;
		memcpy(ISOData.Field47+lbCount,"\xC1\x04\x54", 3); lbCount=lbCount+3;
		memcpy(ISOData.Field47+lbCount,"POS", 3);          lbCount=lbCount+3;
		memcpy(ISOData.Field47+lbCount,"\xCA\x04\x54", 3); lbCount=lbCount+3;
			memcpy(lbTmpTotal,gPTStruct.ProductCode, 2);
			lbTmpTotal[2]=0;
			StrBcdToAsc(4,lbTmpTotal,lbTmpTotal1);
		memcpy(ISOData.Field47+lbCount,lbTmpTotal1+1, 3); lbCount=lbCount+3;
	}
	else if(Type==ISO_INSTALLMENT_ADVICES || Type == ISO_RETAIL)
	{
		lbCount=0;
		//memcpy(ISOData.Field47+lbCount,"\x00\x41", 2);                   lbCount=lbCount+2;
		//memcpy(ISOData.Field47+lbCount,"\xE2\x27", 2);                   lbCount=lbCount+2;
		if(Type != ISO_RETAIL)    // modified by Allen 2016-08-30 added tag C9 for PIN data , so the length is different
		{
			memcpy(ISOData.Field47+lbCount,"\x00\x41", 2);                   lbCount=lbCount+2;
			memcpy(ISOData.Field47+lbCount,"\xE2\x27", 2);                   lbCount=lbCount+2;			
		}
		else
		{
			memcpy(ISOData.Field47+lbCount,"\x00\x46", 2);                   lbCount=lbCount+2;
			memcpy(ISOData.Field47+lbCount,"\xE2\x2C", 2);                   lbCount=lbCount+2;					
		}
		//modify end
        //---------------------------------------------------------------------------------		
		memcpy(ISOData.Field47+lbCount,"\xC1\x04\x54", 3);               lbCount=lbCount+3;
		memcpy(ISOData.Field47+lbCount,"POS", 3);          lbCount=lbCount+3;
        //---------------------------------------------------------------------------------
		//memcpy(ISOData.Field47+lbCount,"\xC8\x07\x46", 3);               lbCount=lbCount+3;
		if (Type != ISO_RETAIL) //// Modified by Kim_LinHB 2013-3-8 build 0.05.0009 add "if (Type != ISO_RETAIL)"
		{
			memcpy(ISOData.Field47+lbCount,"\xC2\x07\x46", 3);               lbCount=lbCount+3;
			memcpy(lbTmpTotal,gPTStruct.EntryAmount,LG_AMOUNT);		
			lbTmpTotal[LG_AMOUNT]=0;
			Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 12, 0, PAD_NONE,0);
			memcpy(ISOData.Field47+lbCount,Bcd, 6);          lbCount=lbCount+6;
		}
		//memcpy(ISOData.Field47+lbCount,"\x00\x00\x00\x09\x00\x00", 6);          lbCount=lbCount+6;
        //---------------------------------------------------------------------------------
		//memcpy(ISOData.Field47+lbCount,"\xC9\x03\x46", 3);               lbCount=lbCount+3;
		memcpy(ISOData.Field47+lbCount,"\xC3\x03\x46", 3); lbCount=lbCount+3;
		StrBcdToAsc(4, gPTStruct.CurrencyCode,lbTmpTotal);
 		//memcpy(lbTmpTotal1,lbTmpTotal+1, 3); 
 		//lbTmpTotal1[3]='F';
		lbTmpTotal1[4]=0;
		memset(lbTmpTotal,0,5);
		AscToHex((PCHAR)lbTmpTotal1, (PCHAR)lbTmpTotal,2);
//by jarod C303460048
//		memcpy(ISOData.Field47+lbCount,lbTmpTotal, 2);       lbCount=lbCount+2;
//		memcpy(ISOData.Field47+lbCount,"\x00\x48", 2);       lbCount=lbCount+2;
		memcpy(ISOData.Field47+lbCount,gPTStruct.CurrencyCode, 2);       lbCount=lbCount+2;
        //---------------------------------------------------------------------------------	
		/*memcpy(ISOData.Field47+lbCount,"\xC2\x07\x46", 3);               lbCount=lbCount+3;
		memcpy(lbTmpTotal,gPTStruct.MonthlyInstAmount,LG_AMOUNT);
		lbTmpTotal[LG_AMOUNT] = 0;
		StrTrim(lbTmpTotal,lbTmpTotal1,' ');
		StrLeftPad(lbTmpTotal1,LG_AMOUNT+1,lbTmpTotal1,'0');
		lbTmpTotal1[LG_AMOUNT]=0;
		Utils_ConvertAscToBcd(lbTmpTotal1, Bcd, 12, 0, PAD_NONE,0);
		//StrAscToBcd(lbTmpTotal1, Bcd);
		memcpy(gPTStruct.BcdAmount, Bcd, 6);

		memcpy(ISOData.Field47+lbCount,gPTStruct.BcdAmount, 6);          lbCount=lbCount+6;
        //---------------------------------------------------------------------------------	
		memcpy(ISOData.Field47+lbCount,"\xC3\x03\x46", 3);               lbCount=lbCount+3;
		StrBcdToAsc(4, gPTStruct.CurrencyCode,lbTmpTotal);
		memcpy(lbTmpTotal1,lbTmpTotal+1, 3); 
		lbTmpTotal1[3]='F';
		lbTmpTotal1[4]=0;
		memset(lbTmpTotal,0,5);
		AscToHex((PCHAR)lbTmpTotal1, (PCHAR)lbTmpTotal,2);
		memcpy(ISOData.Field47+lbCount,lbTmpTotal, 2);       lbCount=lbCount+2;
		*/
        //---------------------------------------------------------------------------------
		memcpy(ISOData.Field47+lbCount,"\xC4\x02\x44", 3);               lbCount=lbCount+3;

		lbTmpTotal[0]=(gPTStruct.TotalInstallmentCount/10)+'0';
		lbTmpTotal[1]=(gPTStruct.TotalInstallmentCount%10)+'0';lbTmpTotal[2]=0;
		Utils_ConvertAscToBcd(lbTmpTotal, lbTmpTotal1, 2, 0, PAD_NONE,0);
		//StrAscToBcd(lbTmpTotal,lbTmpTotal1);
		ISOData.Field47[lbCount]=lbTmpTotal1[0]; lbCount=lbCount+1;
        //---------------------------------------------------------------------------------
		memcpy(ISOData.Field47+lbCount,"\xC6\x07\x46", 3);               lbCount=lbCount+3;    
		//Utils_ConvertAscToBcd(gPTStruct.PIN, Bcd_tmp, 4, 0, PAD_NONE,0);
		//memcpy(ISOData.Field47+lbCount,Bcd_tmp, 2);    lbCount=lbCount+2;
		//memcpy(ISOData.Field47+lbCount,"\x00\x00\x00\x03\x00\x00", 6);    lbCount=lbCount+6;
		memcpy(ISOData.Field47+lbCount,gPTStruct.ProfitAmountBcd, 6);    lbCount=lbCount+6;
        //---------------------------------------------------------------------------------

		// Added by Kim_LinHB 2013-3-5 build 0.05.0009
		if(Type == ISO_RETAIL)
		{
			memcpy(ISOData.Field47+lbCount,"\xC8\x07\x46", 3);    lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,"000000", 6);          lbCount=lbCount+6;
		}
		// Add End

		//added by Allen 2016-08-30   Ahmed : for taqseet please use F52 and F47 tag C9 store the PIN data.
		if(Type == ISO_RETAIL)
		{
			memcpy(ISOData.Field47+lbCount,"\xC9\x03\x54", 3);               lbCount=lbCount+3;    
			Utils_ConvertAscToBcd(gPTStruct.PIN, Bcd_tmp, 4, 0, PAD_NONE,0);
			memcpy(ISOData.Field47+lbCount,Bcd_tmp, 2);    lbCount=lbCount+2;
		}
		//add end

        //memcpy(ISOData.Field47+lbCount,gPTStruct.CurrencyCode, 2);       lbCount=lbCount+2;
		memcpy(ISOData.Field47+lbCount,"\xCA\x04\x54", 3);               lbCount=lbCount+3;
		memcpy(lbTmpTotal,gPTStruct.ProductCode, 2);
		lbTmpTotal[2]=0;
		StrBcdToAsc(4,lbTmpTotal,lbTmpTotal1);
		//memcpy(ISOData.Field47+lbCount,lbTmpTotal1+1, 3); lbCount=lbCount+3;
		//memcpy(ISOData.Field47+lbCount,"\x30\x30\x35", 3); lbCount=lbCount+3; // Hided by Kim_LinHB 2013-3-5 build 0.05.0009
		// Added by Kim_LinHB 2013-3-5 build 0.05.0009
		if (Type == ISO_RETAIL)
		{
			memcpy(ISOData.Field47+lbCount,lbTmpTotal1+1, 3); lbCount=lbCount+3;
		}
		else
		{
			memcpy(ISOData.Field47+lbCount,"\x30\x30\x35", 3); lbCount=lbCount+3;
		}
		// Add End
	}
	// Added by Kim_LinHB 2013-3-5 build 0.05.0009
	else if((Type==ISO_GETBALANCE))
	{
		lbCount=0;
		memcpy(ISOData.Field47+lbCount,"\x00\x28", 2);     lbCount=lbCount+2;
		memcpy(ISOData.Field47+lbCount,"\xE2\x1A", 2);     lbCount=lbCount+2;

		memcpy(ISOData.Field47+lbCount,"\xC1\x04\x54", 3); lbCount=lbCount+3;
		memcpy(ISOData.Field47+lbCount,"POS", 3);          lbCount=lbCount+3;

		memcpy(ISOData.Field47+lbCount,"\xC3\x03\x46", 3); lbCount=lbCount+3;
		memcpy(ISOData.Field47+lbCount,gPTStruct.CurrencyCode, 2);       lbCount=lbCount+2;

		memcpy(ISOData.Field47+lbCount,"\xC8\x07\x46", 3);               lbCount=lbCount+3;
		memcpy(lbTmpTotal,gPTStruct.EntryAmount,LG_AMOUNT);		
		lbTmpTotal[LG_AMOUNT]=0;
		Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 12, 0, PAD_NONE,0);
		memcpy(ISOData.Field47+lbCount,Bcd, 6);          lbCount=lbCount+6;

		memcpy(ISOData.Field47+lbCount,"\xCA\x04\x54", 3); lbCount=lbCount+3;
		if(!bBalanceFromOptions)
		{
// 			memset(lbTmpTotal, '0', 4);
// 			if(0x00 == gPTStruct.ProductCode[1])
// 				memcpy(lbTmpTotal + 2,gPTStruct.ProductCode, 1);
// 			else
// 				memcpy(lbTmpTotal + 1,gPTStruct.ProductCode, 2);
// 			memcpy(ISOData.Field47+lbCount,lbTmpTotal, 3); lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,"\x30\x30\x35", 3); lbCount=lbCount+3;
		}
		else
			memcpy(ISOData.Field47+lbCount,"\x00\x00\x00", 3); lbCount=lbCount+3;

	}
	// Add End

    ///////////////////////////////////////////////////
    ///Currency Code
    ///////////////////////////////////////////////////
    memcpy(ISOData.Field49,gPTStruct.CurrencyCode,2);          
//	memcpy(ISOData.Field49,"\x00\x48",2);

    ///////////////////////////////////////////////////
    ///PIN
    ///////////////////////////////////////////////////
	/*if (Type == ISO_GETBALANCE)															
    memset(sendData.Field52,0x00,8);*/
	//added by Allen 2016-09-02
	if (Type == ISO_RETAIL)  //TYPE70
	{
		memset(tmp, 0, sizeof(tmp));
		memset(Bcd_tmp, 0, sizeof(Bcd_tmp));
		Utils_ConvertAscToBcd(gPTStruct.PIN, tmp, 4, 0, PAD_NONE,0);    // we send the clear PIN data to host 
		sprintf(Bcd_tmp, "\x00\x00\x00\x00\x00\x00%s", tmp);
		//DebugComSend("Field 52 = %02x%02x%02x%02x%02x%02x%02x%02x\n", Bcd_tmp[0],Bcd_tmp[1],Bcd_tmp[2],Bcd_tmp[3],Bcd_tmp[4],Bcd_tmp[5],Bcd_tmp[6],Bcd_tmp[7]);
		memcpy(ISOData.Field52, Bcd_tmp, 8);
	}
    //add end
    ///////////////////////////////////////////////////
    ///ICC Related Data-----------------------------------------------------------------
    ///////////////////////////////////////////////////
	//removed by Allen 2016-08-25   swipe card no need field 55
	/*
    sprintf(Bcd,"%04d",gField55Length);
    Utils_ConvertAscToBcd(Bcd, ISOData.Field55, 4, 0, PAD_NONE,0);	
    //StrAscToBcd(Bcd, ISOData.Field55);
    memcpy(ISOData.Field55+2,gField55, gField55Length);
	*/
    ///////////////////////////////////////////////////
    ///Field-60 Original Data Elements
    ///////////////////////////////////////////////////
	if(Type==ISO_BATCH_UPLOAD_ADVICE) //Orginal MTID
	{
		memcpy(ISOData.Field60,"\x00\x04", 2);
		memcpy(ISOData.Field60+2,"0200", 4);
	}
	else if((Type==ISO_RECONCILIATION)||
			 (Type==ISO_RECONCILIATION_ADVICES))
	{
		ISOData.Field60[0]=0x00;
		ISOData.Field60[1]=0x06;
		memcpy(ISOData.Field60+2,lrHDGBuf.BatchNum, 6);
	}

    ///////////////////////////////////////////////////
    ///Field-63 Additional Data
    ///////////////////////////////////////////////////
	if(Type==ISO_PAYMENT)
	{
		ISOData.Field63[0]=0x00;
		if(StrLen(gPTStruct.Pan)==18)
			ISOData.Field63[1]=0x23;
		else
			ISOData.Field63[1]=0x21;
		memcpy(ISOData.Field63+2,"01860", 5);
		ISOData.Field63[3]=(((StrLen(gPTStruct.Pan)+2)/10)+'0');
		ISOData.Field63[4]=(((StrLen(gPTStruct.Pan)+2)%10)+'0');
		memcpy(ISOData.Field63+7, gPTStruct.Pan, StrLen(gPTStruct.Pan));
	}
	else if((Type==ISO_RECONCILIATION)||
	        (Type==ISO_RECONCILIATION_ADVICES))
	{
		HexToAsc(gPTStruct.CurrencyCode,lbTmpTotal1,2);
		ISOData.Field63[0]=0x00;
		if(Type==ISO_RECONCILIATION)
		//	ISOData.Field63[1]=0x26;//1 ADET
	//	else if(Type==ISO_RECONCILIATION_ADVICES)
			ISOData.Field63[1]=0x47;//2 ADET
		if(Type==ISO_RECONCILIATION)
		//	memcpy(ISOData.Field63+2,"02328", 5);//1 ADET
		//else if(Type==ISO_RECONCILIATION_ADVICES)
			memcpy(ISOData.Field63+2,"04428", 5);//23 ADET
		
		//if(Type==ISO_RECONCILIATION_ADVICES)
		{
			//INSTALLMENT
			memcpy(ISOData.Field63+7 ,"D1O", 3);
			memcpy(ISOData.Field63+10,gCounterDebit, 3);
			memcpy(ISOData.Field63+10+3,lbTmpTotal1+1, 3);
			memcpy(ISOData.Field63+10+6,gAmountDebit, 12);
		}

		//PAYMENT
		memcpy(ISOData.Field63+28 ,"C1O", 3);
		memcpy(ISOData.Field63+31,gCounterPayment, 3);
		memcpy(ISOData.Field63+31+3,lbTmpTotal1+1, 3);
		memcpy(ISOData.Field63+31+6,gAmountPayment, 12);
	}
	else if(Type==ISO_RETAIL || Type==ISO_GETBALANCE)
	{
		ISOData.Field63[0]=0x00;
		ISOData.Field63[1]=0x07;
		memcpy(ISOData.Field63+2,"00462", 5);
		memcpy(ISOData.Field63+7,"27", 2);
	}
	else if(Type==ISO_ACCOUNTMAINTENANCE_ADVICE)
	{
		ISOData.Field63[0]=0x00;
		ISOData.Field63[1]=0x15;
		memcpy(ISOData.Field63+2,"012SR", 5);
			//Issuer Script Result
			memcpy(Bcd,TR.EMVData.issuerScriptResults[1],5);
			Bcd[5]=0;
			StrBcdToAsc(10,Bcd,lbTmpTotal);
		memcpy(ISOData.Field63+7,lbTmpTotal, 10);
	}

    ///////////////////////////////////////////////////
    ///Field-64 MAC
    ///////////////////////////////////////////////////
	memcpy(ISOData.Field64,"\x11\x11\x11\x11", 4);

    ////////////////////////////////////////////////////////////////////////////////
    ///Connect
    ////////////////////////////////////////////////////
	if(Connect==OS_TRUE)
	{
		if (ConnectHost_New() != STAT_OK)
			return(STAT_NOK);
	}

    ////////////////////////////////////////////////////////////////////////////////
    ///Send / Receive
    ////////////////////////////////////////////////////
	if(ISO_Transaction(Type)==ISO_STAT_OK)
	{
        /////////////////////////////////////////////
        /////////////////////////////////////////////
		if((Type == ISO_BATCH_UPLOAD_ADVICE)||
		   (Type == ISO_INSTALLMENT_ADVICES)||
		   (Type == ISO_LOYALTY_ADVICE)||
		   (Type == ISO_RECONCILIATION_ADVICES))
		{
			return(STAT_OK);
		}
		else
		{
            /////////////////////////////////////////////
            /////////////////////////////////////////////
            		if(strlen(ISOData.Field13))
            			memcpy(gPTStruct.BcdDateTime,  ISOData.Field13,2);
			if(strlen(ISOData.Field12))
				memcpy(gPTStruct.BcdDateTime+2,ISOData.Field12,3);
			ClkUpdateDateTime(gPTStruct.BcdDateTime);
			memcpy(gPTStruct.ResponseCode,ISOData.Field39,2);
			
			memcpy(gPTStruct.RetrivalRef,ISOData.Field37,12);
			memcpy(lbTmpTotal1,gPTStruct.RetrivalRef,12);
			lbTmpTotal1[12]=0;
			if((ISOData.Field39[0]!='0')||(ISOData.Field39[1]!='0'))
			{
//MK 02112007 Lost and Stolen Card
				ISO_DisPlayErrMsg(ISOData.Field39);
				if(Type==ISO_RETAIL)
				{
					if(((ISOData.Field39[0]=='4')&&(ISOData.Field39[1]=='1'))||
					   ((ISOData.Field39[0]=='4')&&(ISOData.Field39[1]=='3')))
					{
						/*   removed by Allen 2016-09-02  swipe card have no Field 55
						if(ScriptParser()!=STAT_OK)
						{
							DisplayString((PCHAR)"SCRIPT ERROR",2000);
							return(STAT_INV);
						}
						*/
						OS_ClearScreen();
						OS_DisplayStr( 1, 2, PDS_AUTH_CODE );
						OS_DisplayStr( 2, 2, lbTmpTotal1 );
						return(STAT_OK);
					}
				}
				return(STAT_INV);
			}
			else
			{
				if(Type==ISO_RETAIL)
				{
					/*   removed by Allen 2016-09-02  swipe card have no Field 55
					if(ScriptParser()!=STAT_OK)
					{
						DisplayString((PCHAR)"SCRIPT ERROR",2000);
						return(STAT_INV);
					}
					*/
				}else
				{
					ISO_DisPlayErrMsg(ISOData.Field39);
				}
				if((Type==ISO_PAYMENT)||
				   (Type==ISO_RETAIL)||
			   	   (Type==ISO_ACCOUNTMAINTENANCE_ADVICE))
				{
					OS_ClearScreen();
					OS_DisplayStr( 1, 2, PDS_AUTH_CODE );
					OS_DisplayStr( 2, 2, lbTmpTotal1 );
				}
				if(Type==ISO_GETBALANCE)
				{
					unsigned char err_code;
					ParseF54();
					// Added by Kim_LinHB 2013-3-8 build 0.05.0009
					//memcpy(gPTStruct.Balance, "001000000000", LG_AMOUNT); // Added by Kim_LinHB 2013-3-19 build 0.05.0009 for test
					if(!bBalanceFromOptions)
					{
						if(Parse47(&err_code) != STAT_OK)
						{
							return(STAT_INV);
						}
					}
					// Add End
				}
			}
			return(STAT_OK);
		}
	}
	else
	{
		return(STAT_NOK);
	}
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
unsigned char ISO_Transaction(BYTE trType)
{
    //ISOFrame frame;
    unsigned char retVal;
    int      ISOPacketLength;
    unsigned char sendRetry;
    unsigned char STAN[3];
    unsigned char TID[8];
    BYTE retryCount=3;
    unsigned char sendMessage[32];
    unsigned char receiveMessage[32];

	memcpy(sendMessage,   DSP_SENDING,32);
	memcpy(receiveMessage,DSP_RECEIVING,32);
	memset(&frameToSend, 0, sizeof(ISOFrame));
	if (trType != ISO_REVERSAL)
	{
		switch ( ISO_GetReversal())
		{
			case REVERSAL_PRESENT:
                //retVal = ISO_Transaction (REVERSAL);
                retVal= ISO_STAT_OK;
				memset(&frameToSend, 0, sizeof(ISOFrame));
				break;

			case REVERSAL_NOT_PRESENT:
                retVal = ISO_STAT_OK;
                break;
			default:
                return (ISO_APP_ERROR);
		}
	}

	if (trType == ISO_REVERSAL )
    {
        sendData = reversalData;
    }
	else
	{
		sendData = ISOData;
		receivedData = ISOData;
	}

	if ( retVal != ISO_STAT_OK)
    {
		DebugComSend("ISO_REVERSAL_PENDING\n");
        return (ISO_REVERSAL_PENDING);
    }

	ISOPacketLength = ISO_BuildFrame(trType);

	if ((ISOPacketLength == ISO_FRAME_ERROR) || (ISOPacketLength < 15 ))
	{
		DebugComSend("ISOPacketLength = %d\n", ISOPacketLength);
		return (ISO_FRAME_FAULT);/*at least header must be sent */
	}
    /*!!!sel??? sonra ac
    if ((frameToSend.msgType[0] == 0x02) || (frameToSend.msgType[0] == 0x04))
    {
	    retVal = ISO_AddReversal ();
	    switch (retVal)
	    {
		    case ISO_STAT_OK   :break;

		    default            :return (ISO_APP_ERROR);
	    }
    }
    */

	if (trType != ISO_REVERSAL)
	{
		memcpy(STAN, ISOData.Field11, 3);
		memcpy(TID , ISOData.Field42, 8);
	}
	else
	{
		memcpy(STAN, reversalData.Field11, 3);
		memcpy(TID , reversalData.Field42, 8);
	}

	sendRetry = 1;
	retVal = ISO_STAT_OK;

	while (retryCount > 0)
	{
		memset (&frameReceived, 0, sizeof(ISOFrame)); //clear receive buffer on every send/receive;
		if ((retVal == ISO_COMM_ERROR ) || (retVal == ISO_NO_REPLY))
        {
            retryCount--; // if error on transmission, retry transaction
        }
	/*	else             removed by Allen 20151112
		{*/
			if (sendRetry == 1)
			{
				retVal = ISO_Send(ISOPacketLength, PORT_1,sendMessage,trType);
				DebugComSend("ISO_Send = %d\n", retVal);
			}
			else
			{
                retVal = ISO_STAT_OK; // Packet sent before just try to receive;
			}

			if (retVal == ISO_STAT_OK) // if packet sent successfully
			{
                sendRetry = 0;
				OS_ClearScreen();	// Gary 2011-1-26
                retVal = ISO_Receive(30, PORT_1, receiveMessage);
                if (retVal == ISO_STAT_OK) // if packet received successfully first resolve fields
                {
                    /////////////////////////////////////////////
                    /////////////////////////////////////////////
                    //TPDU
                    if((trType == ISO_BATCH_UPLOAD_ADVICE)||
                        (trType == ISO_INSTALLMENT_ADVICES)||
                        (trType == ISO_LOYALTY_ADVICE)||
                        (trType == ISO_RECONCILIATION_ADVICES))
                    {
                        return (retVal);
                    }
                    /////////////////////////////////////////////
                    /////////////////////////////////////////////
                    if ( ISO_ResolveFrame() == ISO_OK) // check incoming packet to be correct
					{     // If packet is structurely correct, check incoming STAN, Terminal ID with outgoing message
						if (memcmp(sendData.Field11,receivedData.Field11,3) != 0)
//						if ((trType != ISO_REVERSAL)&&(memcmp(sendData.Field11,STAN,3) != 0))
							return (ISO_COMM_ERROR);
//						if ((trType == ISO_REVERSAL)&&(memcmp(receivedData.Field11,STAN,3) != 0))
//							return (ISO_COMM_ERROR);
//						if ((trType != ISO_REVERSAL)&&(memcmp(sendData.Field42,TID,8) != 0))
//							return (ISO_COMM_ERROR);
						if ((trType == ISO_REVERSAL)&&(memcmp(receivedData.Field42,TID,8) != 0))
						{
							return (ISO_COMM_ERROR);
						}
						
						{
							// if this is a reversal tran check the response (authorization code) for automatic reversal handling!!!
							if (( trType == ISO_REVERSAL) && ( memcmp ( receivedData.Field39, "00" , 2) != 0))
                            {
                                return (ISO_REVERSAL_PENDING);
                            }
							if (trType != ISO_REVERSAL )
                            {
                                /***/ISOData = receivedData; // if this is not a reversal tr insert info to ISO fields
                            }
//							if ((trType == ISO_REVERSAL) && ( memcmp ( receivedData.Field39, "00" , 2) != 0)) return (ISO_REVERSAL_PENDING);
							if ((frameToSend.msgType[0] == 0x02) || (frameToSend.msgType[0] == 0x04)) // if financial tr or reversal!!!
							{ // If successful tr is either REVERSAL or financial transaction , delete inserted reversal
								switch (ISO_GetReversal())
								{
									case ISO_APP_ERROR:
                                        return (ISO_APP_ERROR);
									default:
                                        break;
								}
							}
							return (ISO_STAT_OK);
						}
					}
					else
                    {
                        retVal = ISO_NO_REPLY;
                    }
				}
				else
                {
                    retVal = ISO_NO_REPLY;
                }
			}
			else
			{
				// Delete REVERSAL Info hence packet cannot be sent
				switch (ISO_GetReversal())
				{
					case ISO_APP_ERROR:
                        return (ISO_APP_ERROR);
					default:
                        break;
				}
			}
		//}
	}
	return (retVal);
}

unsigned char ISO_GetReversal()
{
    int fileHandle;
    int readHnd;

	PrnInit();

	fileHandle = Open(FILE_REVERSE, O_RDWR|O_CREATE);
	if ( fileHandle < 0 )
	{
		return (ISO_APP_ERROR);
	}

	readHnd = Read( fileHandle ,(BYTE *)&reversalData , sizeof(ISODataStruct));
	if (readHnd == 0)
	{
		return (REVERSAL_NOT_PRESENT);
	}
	else if (readHnd > 0 )
	{
		fileHandle = NCreate(FILE_REVERSE, -1);
		if ( fileHandle <=0 )
		{
			return (ISO_APP_ERROR);
		}

		Close(fileHandle);
		return (REVERSAL_PRESENT);
	}

	StartPrinter();
	Close(fileHandle);
	return (ISO_APP_ERROR);
}

unsigned char ISO_AddReversal ()
{
    int fileHandle;

	PrnInit();
	fileHandle = Open (FILE_REVERSE,O_CREATE);
	if ( fileHandle < 0 )
	{
		return (ISO_APP_ERROR);
	}

	if (!(Write( fileHandle , (BYTE *)&sendData , sizeof(ISODataStruct) ) ))
	{
		return (ISO_STAT_OK);
	}

	StartPrinter();
	return (ISO_APP_ERROR);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FUNCTIONS/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char  ISO_AddField(/*ISOFrame* frame ,*/unsigned char **frameLocator ,short int fieldNo ,unsigned char *data)
{
    unsigned char lengthAscii[2];
    unsigned char lengthBcd;
    short int length;
    BYTE buffer[50];


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////       WARNING!!! Modifying frame pointer (*frameLocator) on this module will cause accessive damage!!!        ////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// if field is not present on the bitmap pass it over
	if (!(((frameToSend.bitMap[fieldNo/8-((fieldNo%8) ? 0:1)]) >> ((8-fieldNo%8)%8)) & 0x01)) return (ISO_INSERTED);

	 fieldNo--;


	// if field definitions are wrong return error
	if (Fields[fieldNo].type == TYPE_NONE) return (ISO_TYPE_ERROR);
	if (Fields[fieldNo].sizeRange == FIELD_NOT_USED) return (ISO_SIZE_ERROR);

	 // if field size is FIXED add fixed number of bytestream to the framebuffer
	if (Fields[fieldNo].sizeRange == FIXED_SIZE)
	{

		if (Fields[fieldNo].type == TYPE_BCD)
		{
			length = Fields[fieldNo].fldSize/2+Fields[fieldNo].fldSize%2;
		}
		else if (Fields[fieldNo].type == TYPE_BINARY)
		{
			length = Fields[fieldNo].fldSize/8;
		}
		else if (Fields[fieldNo].type == TYPE_2ASCII) length = Fields[fieldNo].fldSize;
		else {
			return (ISO_APP_ERROR);
		}	
		memcpy((unsigned char *)*frameLocator,(unsigned char *)data,length);
		*frameLocator+=length;
		return (ISO_INSERTED);
	 }

	// if field has variable size calculate the size and add it into the buffer

	if (Fields[fieldNo].type == TYPE_BCD)
	{

//      Must Be In XX Format
		lengthBcd = data[0];

		length = ((lengthBcd >> 4 )&0x0F)*10 + (lengthBcd & 0x0F);

		//if given data size is bigger than size declared return error
		if (length > Fields[fieldNo].fldSize)
		{
            return (ISO_SIZE_ERROR);
		}

		memcpy((unsigned char *)*frameLocator,(unsigned char *)data,(length/2+length%2)+1);

		*frameLocator+=((length/2+length%2)+1);
		return (ISO_INSERTED);
	}
	else if (Fields[fieldNo].type == TYPE_1ASCII)
	{
//      Must Be In XX Format
		lengthBcd = data[0];
		length = ((lengthBcd >> 4 )&0x0F)*10 + (lengthBcd & 0x0F);
//length =37;

		//if given data size is bigger than size declared return error
		if (length > Fields[fieldNo].fldSize)
		{
            sprintf(buffer, "SIZE ERROR6, FIELD %d", (int)fieldNo);
            Utils_DebugPrintMsg(buffer);
			return (ISO_SIZE_ERROR);
		}
/*		if (length%2) length++;
		memcpy((unsigned char *)*frameLocator,(unsigned char *)data,length/2+1);
		*frameLocator+=(length/2+1);*/
		memcpy((unsigned char *)*frameLocator,(unsigned char *)data,length+1);
		*frameLocator+=(length+1);
		return (ISO_INSERTED);
    }
    else if (Fields[fieldNo].type == TYPE_2ASCII)
    {
        memcpy( lengthAscii , data ,2 );
        if ((lengthAscii[0])&0xF0)
        {
            sprintf(buffer, "SIZE ERROR6, FIELD %d", (int)fieldNo);
            Utils_DebugPrintMsg(buffer);
            return (ISO_SIZE_ERROR);
        }
        length = (((lengthAscii[0])&0x0F)*100 + ((lengthAscii[1]>>4)&0x0F)*10 + (lengthAscii[1] & 0x0F));
        
        //if given data size is bigger than size declared return error
        if (length > Fields[fieldNo].fldSize)
        {
            sprintf(buffer, "SIZE ERROR2, FIELD %d", (int)fieldNo);
            Utils_DebugPrintMsg(buffer);
            return (ISO_SIZE_ERROR);
        }
        memcpy((unsigned char *)*frameLocator,(unsigned char *)data,length+2);
        *frameLocator+=(length+2);
        return (ISO_INSERTED);
    }
    else if (Fields[fieldNo].type == TYPE_BINARY)
    {
        if (data[0] & 0xF0) return (ISO_SIZE_ERROR);
        memcpy( lengthAscii , (unsigned char *)data ,2 );
        length = ((lengthAscii[0])&0x0F)*100 + (((lengthAscii[1])>>4)&0x0F)*10 + (lengthAscii[1])&0x0F;
        length = length /8;
        
        memcpy((unsigned char *)*frameLocator,(unsigned char *)data,length+2);
        
        *frameLocator+=(length+2);
        return (ISO_INSERTED);
        
    }
    else
    {
        sprintf(buffer, "TYPE ERROR8, FIELD %d", (int)fieldNo);
        Utils_DebugPrintMsg(buffer);
        return (ISO_TYPE_ERROR);
    }
}



unsigned char  ISO_GetField(unsigned char **frameLocator ,short int fieldNo, unsigned char *data)
{
    unsigned char lengthAscii[2];
    unsigned char lengthBcd;
    int length;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////       WARNING!!! Modifying frame pointer (*frameLocator) on this module will cause accessive damage!!!        ////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// if field is not present on the bitmap pass it over
	if (!(((frameReceived.bitMap[fieldNo/8-((fieldNo%8) ? 0:1)]) >> ((8-fieldNo%8)%8)) & 0x01)) return (ISO_INSERTED);
    fieldNo--;

	// if field definitions are wrong return error
	if (Fields[fieldNo].type == TYPE_NONE) return (ISO_TYPE_ERROR);
	if (Fields[fieldNo].sizeRange == FIELD_NOT_USED) return (ISO_SIZE_ERROR);


	// if field size is FIXED receive fixed number of bytestream from framebuffer
	if (Fields[fieldNo].sizeRange == FIXED_SIZE)
	{

		if (Fields[fieldNo].type == TYPE_BCD)
		{
			length = Fields[fieldNo].fldSize/2+Fields[fieldNo].fldSize%2;
		}
		else if (Fields[fieldNo].type == TYPE_BINARY)
		{
			length = Fields[fieldNo].fldSize/8;
		}
		else if (Fields[fieldNo].type == TYPE_2ASCII) length = Fields[fieldNo].fldSize;
		else return (ISO_APP_ERROR);

		memcpy((unsigned char *)data,(unsigned char *)*frameLocator,length);
		*frameLocator+=length;
		return (ISO_INSERTED);
    }

//      if field has variable size calculate the size and get from  framebuffer
	    
	if (Fields[fieldNo].type == TYPE_BCD)
	{
		memcpy(&lengthBcd,(unsigned char *)*frameLocator,1);


		length = ((lengthBcd>>4)&0x0F)*10 + (lengthBcd & 0x0F);
		if (length > Fields[fieldNo].fldSize) return (ISO_SIZE_ERROR);

		memcpy((unsigned char *)data, (unsigned char *)*frameLocator, length/2+length%2+1);


        *frameLocator+=(length/2+length%2+1);

		return (ISO_INSERTED);
	}
	else if (Fields[fieldNo].type == TYPE_1ASCII)
	{
		memcpy(&lengthBcd,(unsigned char *)*frameLocator,1);
		length = ((lengthBcd>>4)&0x0F)*10 + (lengthBcd & 0x0F);
		if (length > Fields[fieldNo].fldSize) return (ISO_SIZE_ERROR);


		memcpy((unsigned char *)data, (unsigned char *)*frameLocator, length+1);
		*frameLocator+=(length+1);

		return (ISO_INSERTED);
	}
	else if (Fields[fieldNo].type == TYPE_2ASCII)
	{
		memcpy((unsigned char *)lengthAscii,(unsigned char *)*frameLocator,2);

//      First Nibble Not Important! Must Be In 0XXX Format
		length = ((lengthAscii[0])&0x0F)*100 + ((lengthAscii[1]>>4)&0x0F)*10 + (lengthAscii[1] & 0x0F);
//      If data size is bigger than size declared return error
		if (length > Fields[fieldNo].fldSize) return (ISO_SIZE_ERROR);

		memcpy((unsigned char *)data, (unsigned char *)*frameLocator, length+2);
		*frameLocator+=(length+2);

		return (ISO_INSERTED);
	}
	else if (Fields[fieldNo].type == TYPE_BINARY)
	{
		memcpy((unsigned char *)lengthAscii,(unsigned char *)*frameLocator,2);

//      First Nibble Not Important! Must Be In 0XXX Format
		length = ((lengthAscii[0])&0x0F)*100 + ((lengthAscii[1]>>4)&0x0F)*10 + (lengthAscii[1] & 0x0F);
//      If data size is bigger than size declared return error
		if (length > Fields[fieldNo].fldSize) return (ISO_SIZE_ERROR);

		length = length /8;
		memcpy((unsigned char *)data, (unsigned char *)*frameLocator, length+2);
		*frameLocator+=(length+2);

		return (ISO_INSERTED);
	}
	else return (ISO_TYPE_ERROR);
}

int ISO_BuildFrame (BYTE trType)
{
    unsigned char *frameLocator = (unsigned char *)frameToSend.Data;
    unsigned char retVal;
    short int     fieldNo;
    //unsigned char MACResult[10];


	frameToSend.TPDU = Transactions[trType].TPDU;
    //NII-NETSERVER
    if((trType == ISO_BATCH_UPLOAD_ADVICE)||
        (trType == ISO_INSTALLMENT_ADVICES)||
        (trType == ISO_LOYALTY_ADVICE)||
        (trType == ISO_RECONCILIATION_ADVICES))
    {
        memcpy(frameToSend.destAddr, gPMSNII,2);
    }
    else
    {
		//by jarod 20110803
        memcpy(frameToSend.destAddr, gNETSERVERNII,2);
    }

    //memcpy(frameToSend.destAddr, Transactions[trType].destAddr,2);
    memcpy(frameToSend.origAddr, Transactions[trType].origAddr,2);
	memcpy(frameToSend.msgType , Transactions[trType].msgType ,2);
	memcpy(frameToSend.bitMap  , Transactions[trType].bitMap  ,8);
	memcpy (sendData.Field3, Transactions[trType].processingCode, 3);


	for ( fieldNo = 1; fieldNo < 65 ; fieldNo++)
	{
//		if(fieldNo==64)
//		{
//			CalculateMAC(MACResult,((int)frameLocator - (int)frameToSend.Data));
//			memcpy(sendData.Field64,MACResult, 4);
//		}
	
		retVal = ISO_AddField(&frameLocator , fieldNo, Fields[fieldNo-1].ptrSendData);
		if ( retVal != ISO_INSERTED )
		{
			return (ISO_FRAME_ERROR);
		}					
	}
																	 //   1	+   2    +    2   +  2  +  8   = 15 bytes
	return ( (int)frameLocator - (int)frameToSend.Data + 15 /* TPDU+DESTADDR+ORIGADDR+MSGID+BITMAP*/);
}



int ISO_ResolveFrame ()
{
    unsigned char *frameLocator = (unsigned char *)frameReceived.Data;
    unsigned char retVal;
    short int     fieldNo;

	for ( fieldNo = 1; fieldNo < 65 ; fieldNo++)
	{
		retVal = ISO_GetField(&frameLocator , fieldNo, (unsigned char *)Fields[fieldNo-1].ptrReceivedData);
		if ( retVal != ISO_INSERTED )
		{
			return (ISO_FRAME_ERROR);
		}
	}
	return (ISO_OK);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                              These Parts Are Specially Coded For Schlumberger Terminals                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char ISO_Send(int length,unsigned char portNum, unsigned char *message,BYTE trType )
{
    char dispBuffer[17];
    TERMCONFIGSTRUCT tconf;
    unsigned char TempBuf[1000];
    WORD Len=0;


	GetTerminalConfig(&tconf);

	dispBuffer[16]=0;
	/*OS_ClearScreen();
	memcpy(dispBuffer, message, 16);
	OS_DisplayStr( 1, 0, dispBuffer);
	memcpy(dispBuffer, message+16, 16);
	OS_DisplayStr( 2, 0, dispBuffer);*/

	//	OS_DisplOS_ClearScreenayFlush();
	OS_ClearScreen();
	if (message!=NULL)
	{
		ScrPrint(0, 3, CFONT, "%.16s", message);
		if (StrLen(message)>16)
		{
			ScrPrint(0, 5, CFONT, "%.16s", message+16);
		}
	}

    /////////////////////////////////////////////
    /////////////////////////////////////////////
    //TPDU

    if((trType == ISO_BATCH_UPLOAD_ADVICE)||
        (trType == ISO_INSTALLMENT_ADVICES)||
        (trType == ISO_LOYALTY_ADVICE)||
        (trType == ISO_RECONCILIATION_ADVICES))
    {
        Len=0;
        memcpy(TempBuf,(BYTE *)&frameToSend,5);Len=Len+5;
        memcpy(TempBuf+Len,"\x0A",1);Len=Len+1;
        memcpy(TempBuf+Len,(BYTE *)&frameToSend+5,length-5);Len=Len+(length-5);
        length=Len;
    }
    else
    {
    /////////////////////////////////////////////
    /////////////////////////////////////////////
        memcpy(TempBuf,(BYTE *)&frameToSend,length);
    }

    if ((gDebug))
	{
//TPDU
		Utils_DebugPrint(DEB_PACKET_SEND,(BYTE *)&TempBuf, (short int) length);
		//Utils_DebugPrint(DEB_PACKET_SEND,(BYTE *)&frameToSend, (short int) length);
	}
    
    if (SendNChar((BYTE  *)&TempBuf, length))
	{
		PubBeepErr();
        return (ISO_COMM_ERROR);
	}
	
//#ifdef OUTPUT_ISO8583
    if(gDebug)
	{
		PubDebugOutput("SEND", TempBuf, length, DEVICE_PRN, ISO_MODE);
	}

    return (ISO_STAT_OK);
}

unsigned char ISO_Receive(short int timeOut,unsigned char portNum, unsigned char *message )
{
    WORD                length;
    OS_STATUS           stat;
    char                dispBuffer[17];
    TERMCONFIGSTRUCT    tconf;

	GetTerminalConfig(&tconf);

	dispBuffer[16]=0;
//	OS_DisplOS_ClearScreenayFlush();
	if (message!=NULL)
	{
		ScrPrint(0, 3, CFONT, "%.16s", message);
		if (StrLen(message)>16)
		{
			ScrPrint(0, 5, CFONT, "%.16s", message+16);
		}
	}

	while (1)
	{
		memset(&frameReceived, 0, sizeof(frameReceived));
		length = 0;

        stat = ReceivePacket((BYTE *)&frameReceived, (BYTE)timeOut, &length);
        if (stat==OS_OK)
        {
//#ifdef OUTPUT_ISO8583
//#endif
			if(gDebug)
			{
				//BYTE sBuffer[ISO_MAX_LEN]; // never be used
				PubDebugOutput("RECEIVE", (BYTE *)&frameReceived, length, DEVICE_PRN, ISO_MODE);
			}
            if (length < 15 )
			{
				if(memcmp( (BYTE *)&frameReceived+5 ,"\x05", 1) == 0)
                {
					return (ISO_STAT_OK);
                }
			}
			else
            {
                return (ISO_STAT_OK);
            }
		}
		else if (stat==OS_COM_TIMEOUT)
        {
			PubBeepErr();
			return (ISO_NO_REPLY);
        }
		else
        {
            PubBeepErr();
			return (ISO_COMM_ERROR);
        }
	}
}


WORD ScriptParser(void)
{
    BYTE Len=0;
    TR.EMVData.issuerAuthenticationDataLength = 0;

	//if (gDebug) Utils_DebugPrint("Field 55 :",ISOData.Field55,ISOData.Field55[1]);
	Len=Len+2;
	if(ISOData.Field55[Len]==0x71)
	{
		Len++;
		TR.EMVData.issuerScriptTemplateLength[0]=ISOData.Field55[Len];
		Len++;
		memcpy(&TR.EMVData.issuerScriptTemplate[0],ISOData.Field55+Len,TR.EMVData.issuerScriptTemplateLength[0]);
		Len=Len+TR.EMVData.issuerScriptTemplateLength[0];
		if(ISOData.Field55[Len]==0x91)
		{
			Len++;
			TR.EMVData.issuerAuthenticationDataLength=ISOData.Field55[Len];
			Len++;
			memcpy(TR.EMVData.issuerAuthenticationData,ISOData.Field55+Len,TR.EMVData.issuerAuthenticationDataLength);
			Len=Len+TR.EMVData.issuerAuthenticationDataLength;
		}
		else
        {
			return(STAT_NOK);
        }
	}
	else if(ISOData.Field55[Len]==0x72)
	{
		Len++;
		TR.EMVData.issuerScriptTemplateLength[1]=ISOData.Field55[Len]+2;
		Len++;
		memcpy(&TR.EMVData.issuerScriptTemplate[1],ISOData.Field55+Len-2,TR.EMVData.issuerScriptTemplateLength[1]);
		Len=Len+TR.EMVData.issuerScriptTemplateLength[1]-2;
		if(ISOData.Field55[Len]==0x91)
		{
			Len++;
			TR.EMVData.issuerAuthenticationDataLength=ISOData.Field55[Len];
			Len++;
			memcpy(TR.EMVData.issuerAuthenticationData,ISOData.Field55+Len,TR.EMVData.issuerAuthenticationDataLength);
			Len=Len+TR.EMVData.issuerAuthenticationDataLength;
		}
		else
        {
			return(STAT_NOK);
        }
	}
	else if(ISOData.Field55[Len]==0x91) // 2011-3-22
	{
		Len++;
		TR.EMVData.issuerAuthenticationDataLength=ISOData.Field55[Len];
		Len++;
		memcpy(TR.EMVData.issuerAuthenticationData,ISOData.Field55+Len,TR.EMVData.issuerAuthenticationDataLength);
		Len=Len+TR.EMVData.issuerAuthenticationDataLength;
	}
	else
    {
		return(STAT_NOK);
    }

	return(STAT_OK);
}

void ParseF54(void)
{
	unsigned int length;
	BYTE out[15];

	Utils_ConvertBcdToAsc( ISOData.Field54, out, 2);
	out[4]=0;
	length = atoi(out);
	if (length!=20)
    {
		return;
    }
		
	if (!memcmp(ISOData.Field54+2,"0002",4))
    {
		 //currency code check 
		if(ISOData.Field54[9]==0x43)
        {		
			memcpy(gPTStruct.Balance,ISOData.Field54+2+4+3+1,LG_AMOUNT);
			memset(out,0,13);
			memcpy(out,gPTStruct.Balance,12);
			mCmmnUtil_AsciiToBcd(out, gCardParam.UpperLimit, 12);
		}
	}
}				

// Added by Kim_LinHB 2013-3-5 build 0.05.0009
static unsigned char *FindStr(const unsigned char *Str, unsigned int Str_Len,\
							  const unsigned char *Sub_Str, unsigned int Sub_Str_Len)
{
	unsigned int i;
	unsigned int Cnt;
	if( !Str || !Sub_Str || 
		0 == Str_Len || 0 == Sub_Str_Len || 
		Str_Len < Sub_Str_Len )
		return NULL;
	
	Cnt = 0;
	for(i = 0; i < Str_Len; ++i)
	{
		if (Str[i] == Sub_Str[Cnt])
			++Cnt;
		else 
			Cnt = 0;
		
		if(Sub_Str_Len == Cnt)
			return (unsigned char *)(Str + i - Cnt + 1);
	}
	return NULL;
}

int Parse47(unsigned char *err_code)
{
	unsigned int len47, lenF0, lenTag, lenMinAmt, lenMaxAmt, lenD7, lenD8, lenD9, lenDA;
	BYTE *F0, *Tag, *D7, *D8, *D9, *DA;
	BYTE Value[50]; // 50 temp

	memset(Value, 0, sizeof(Value));

	Utils_ConvertBcdToAsc( ISOData.Field47, Value, 2);
	Value[4]=0;
	len47 = atoi(Value);

	memset(Value, 0, sizeof(Value));

	if ( F0 = FindStr(ISOData.Field47 + 2, len47, "\xF0", 1))
	{
		lenF0 = *(F0 + 1);
	}
	else
		return STAT_NOK;

	F0 += 2;	//skip "\xF0+1bit(len)"

	//F0.D1 will contain profit percentage. (for eg. 948 is 9.48 %)
	if ( Tag = FindStr(F0, lenF0, "\xD1", 1))
	{
		lenTag = *(Tag + 1) - 1;
		// do something
		sprintf(Value, "%*.*s", lenTag, lenTag, Tag + 3);
		Utils_ConvertAscToBcd(Value, gPTStruct.SchemeProfit, 4, 0, PAD_NONE,0);
	}
	else
		return STAT_NOK;

	//F0.D2 will contain Min Finance amount
	//( for displaying in error message only if the transaction is rejected)
	if ( Tag = FindStr(F0, lenF0, "\xD2", 1))
	{
		lenMinAmt = *(Tag + 1) - 1;
		// do something
		sprintf(gPTStruct.HMinTotal, "%*.*s", lenMinAmt, lenMinAmt, Tag + 3);
	}
	else
		return STAT_NOK;

	//F0.D3 will contain Max Finance amount 
	//( for displaying in error message only if the transaction is rejected)
	if ( Tag = FindStr(F0, lenF0, "\xD3", 1))
	{
		lenMaxAmt = *(Tag + 1) - 1;
		// do something
		sprintf(gPTStruct.HMaxTotal, "%*.*s", lenMaxAmt, lenMaxAmt, Tag + 3);
	}
	else
		return STAT_NOK;

	//F0.D4 will contain Min Tenure, will be 0 if the amount is outside the min and max finance,
	//it this case the error message should show that that transaction is not allowed as the 
	//amount is out of the min amount (F0.D2) and max amount (F0.D3).	
	if ( Tag = FindStr(F0, lenF0, "\xD4", 1))
	{
		uchar Min[LG_AMOUNT];
		uchar Max[LG_AMOUNT];
		lenTag = *(Tag + 1) - 1;
		// do something

		PubAscMul(gPTStruct.HMinTotal, "100", Min);
		PubAscMul(gPTStruct.HMaxTotal, "100", Max);
		PubAddHeadChars(Min, LG_AMOUNT, '0');
		PubAddHeadChars(Max, LG_AMOUNT, '0');

		memcpy(Value, Tag + 3, lenTag);
		if (0 == memcmp(Value, "0", 1) && 
			(memcmp(gPTStruct.EntryAmount, Min, LG_AMOUNT) < 0 || 
			memcmp(gPTStruct.EntryAmount, Max, LG_AMOUNT) > 0))
		{
			ScrCls();
			ScrPrint(0,0, CFONT, "Transaction is\nnot allowed. Amt\nis out of range.");
			ScrPrint(0,7, ASCII, "Min:%s",gPTStruct.HMinTotal);
			ScrPrint(50,7, ASCII, "Max:%s",gPTStruct.HMaxTotal);
			DelayMs(2000);
			return STAT_IMPOSSIBLE;
		}
		else
		{
			gPTStruct.HMinTenure = atoi(Value);
		}
	}
	else
	{	
		//	do something
		gPTStruct.HMinTenure = 0;
	}

	//F0.D5 will contain Max Tenure
	if ( Tag = FindStr(F0, lenF0, "\xD5", 1))
	{
		lenTag = *(Tag + 1) - 1;
		// do something
		sprintf(Value, "%*.*s", lenTag, lenTag, Tag + 3);
		gPTStruct.HMaxTenure = atoi(Value);
	}
	else
	{	
		//	do something
		gPTStruct.HMaxTenure = 0;
	}

	//F0.D6 will contain the applicable/allowed tenure to be shown in the POS
	//(for eg. 18175327 means (1000101010101010101011111, will allow tenure) 
	//	1,3,6,9,12,18,24,30,36,42,48,54,60,72 to be shown. The tenures are multiple 
	//	of 3 starting from right. 1 is allowed and 0 not allowed.	
	if ( Tag = FindStr(F0, lenF0, "\xD6", 1))
	{
		lenTag = *(Tag + 1) - 1;
		// do something
		sprintf(Value, "%*.*s", lenTag, lenTag, Tag + 3);
		gPTStruct.lTenure = (unsigned long)atol(Value);
	}
	else
		return STAT_NOK;

	
// 	?If there is any promotion it 
// 		will contain the profit rate. This profit rate should be applied according to 
// 		following criteria	
	if ( D7 = FindStr(F0, lenF0, "\xD7", 1))
	{
		lenD7 = *(D7 + 1) - 1;
		// do something
		if ( D8 = FindStr(F0, lenF0, "\xD8", 1))
		{
// 			o	If D8,D9 and/or DA exists then the promotion profit rate will be applied only 
// 				to the tenures in D8 and D9. For tenures greater than D9 POS will display the 
// 				normal profit rate and tenure options
// 			?F0.D8 may contain the min Tenure if D7 exists. 
			lenD8 = *(D8 + 1) - 1;
			//	do something
		}
		else
		{
//			o	If D8,D9 and/or DA doesn't exist than it should be applied for all tenures 
//				as in D4 and D5.
			//	do something
		}

		if ( D9 = FindStr(F0, lenF0, "\xD9", 1))
		{
// 			o	If D8,D9 and/or DA exists then the promotion profit rate will be applied only 
// 				to the tenures in D8 and D9. For tenures greater than D9 POS will display the 
// 				normal profit rate and tenure options
// 			?F0.D9 may contain the max Tenure if D7 exists. 
			lenD9 = *(D9 + 1) - 1;
			//	do something
		}
		else
		{
//			o	If D8,D9 and/or DA doesn't exist than it should be applied for all tenures 
//				as in D4 and D5.
			//	do something
		}

		if ( DA = FindStr(F0, lenF0, "\xDA", 1))
		{
// 			o	If D8,D9 and/or DA exists then the promotion profit rate will be applied only 
// 				to the tenures in D8 and D9. For tenures greater than D9 POS will display the 
// 				normal profit rate and tenure options
// 			?F0.DA may contain the available tenures for promotion.
// 				for e.g. 18175327 means (1000101010101010101011111) if user selected 
// 				one of the tenures   1,3,6,9,12,18,24,30,36,42,48,54,60,72 then he should 
// 				be allowed to use the promotion profit.
			lenDA = *(DA + 1) - 1;
			//	do something
		}
		else
		{
//			o	If D8,D9 and/or DA doesn't exist than it should be applied for all tenures 
//				as in D4 and D5.
			//	do something
		}
	}
	else
	{
//		?F0.D7 if doesn't exist then there is no promotion.
		//	do something
	}

	//F0.DB contains the error code; if 0 means success, if other than 0 means 
	//the transaction is rejected due to business rules.
	if ( Tag = FindStr(F0, lenF0, "\xDB", 1))
	{
		lenTag = *(Tag + 1) - 1;
		memcpy(Value, Tag + 2, lenTag);
		Value[lenTag] = '\0';
		if (0 != atoi(Value))
			return STAT_NOK;
	}
	else
	{	
		//	do something
	}

	return STAT_OK;
}
// Add End
