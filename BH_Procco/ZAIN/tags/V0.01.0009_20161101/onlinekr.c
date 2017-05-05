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
ISOTransactionsDescriptor Transactions[13]=
{                                               //Trans Type                                        //HSM
//PAYMENT----------------NET SERVER  
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x02,0x00},{0x24,0x00,0x00},{0x72,0x24,0x04,0x80,0x20,0x82,0x80,0x00} },// PAYMENT
//ACCONT MAINT-----------NET SERVER
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x01,0x00},{0x00,0x00,0x00},{0x72,0x24,0x04,0x80,0x20,0x80,0x80,0x02} },// ACCOUNT MAINT
//ACCONT MAINT ADVICE----NET SERVER
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x01,0x20},{0x91,0x00,0x00},{0x72,0x24,0x04,0x80,0x08,0x80,0x80,0x02} },// ACCOUNT MAINT ADVICE
//REVERSAL---------------NET SERVER
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x04,0x00},{0x00,0x00,0x00},{0x70,0x24,0x25,0x80,0x00,0xE0,0x80,0x04} },// REVERSAL'da Currency Code 槆in
//RECONCILIATION---------NET SERVER
	{ {0x60},{0x00,0x03},{0x00,0x00},{0x05,0x00},{0x92,0x00,0x00},{0x22,/*0x20*/0x38,0x04,0x80,0x00,0x80,0x00,0x12} },// RECONCILIATION
//BATCH UPLOAD-----------PMS                                      
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x03,0x20},{0x24,0x00,0x00},{0x70,0x3C,0x04,0x80,0x08,0x80,0x80,0x10} },// BATCH UPLOAD
//INSTALLMENT ADVICE-----PMS
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x02,0x20},{0x00,0x00,0x00},{0x70,0x3C,0x04,0x80,0x0A,0x82,0x80,0x00} },// INSTALLMENT ADVICE
//LOYALTY ADVICE---------PMS
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x02,0x20},{0x16,0x00,0x00},{0x70,0x3C,0x04,0x80,0x08,0x82,0x80,0x00} },// LOYALTY ADVICE
//RECONCILIATION ADVICE--PMS
	{ {0x60},{0x00,0x05},{0x00,0x00},{0x05,0x20},{0x92,0x00,0x00},{0x20,0x38,0x20,0x80,0x00,0x80,0x00,0x12} }, // RECONCILIATION ADVICE
//BALANCE
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x01,0x00},{0x30,0x00,0x00},{0x72,0x2C,0x04,0x80,0x20,0x82,0x80,0x02} },// BALANCE INQ // Modified by Kim_LinHB 2013-3-13 build 0.05.0009 add F47
//RETAIL
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x02,0x00},{0x00,0x00,0x00},{0x72,0x24,0x04,0x80,0x20,0x82,0x80,0x02} },// 
//void/reversal  for reward
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x04,0x00},{0x24,0x00,0x00},{0x70,0x3C,0x05,0x80,0x08,0x80,0x80,0x10}}, //added by Allen 2016-05-30
//void/reversal  for redeem
	{ {0x60},{0x06,0x61},{0x00,0x00},{0x02,0x20},{0x02,0x00,0x00},{0x70,0x3C,0x05,0x80,0x08,0x80,0x80,0x00}}, //added by Allen 2016-07-04
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
int Parse47_MST(/*the arguments are uncertain for now*/unsigned char *err_code); // Added by richard 2015-7-17 
int Parse47_Points(unsigned char *err_code, unsigned char Type);      //added by Allen 2016-06-16
extern unsigned char ucDoNotHonorFlag;  // Added by richard 2015-8-18 


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

	//DebugComSend("ISOData.Field39 = %02s\n", ISOData.Field39);
	//DebugComSend("Recp_Code = %d\n", Recp_Code);

	for(i=0;i<10;i++)
	{
		if(gHostErrMsgTableFlag[i]==Recp_Code)
		{
			if((Recp_Code == 1 || Recp_Code == 9) && (memcmp(gPTStruct.TransType,TYPE_76,2)!=0))  //added by Allen 2016-09-28
				return;
			Print_HostErr_Slip((BYTE *)gHostErrMsgForDisp[i]);
			DisplayLongString((BYTE *)gHostErrMsgForDisp[i],2000*100);
			ScrCls();
		}
	}
}

WORD ISO_SendReceive(BYTE Connect,BYTE Type)        //根据交易类型不同，组成不同的数据包，并且发送/接收
{
	int iRet;
	int length;
    HDGStruct lrHDGBuf;
    BYTE lbTmpTotal[25];
    BYTE lbTmpTotal1[25];
    BYTE Bcd[73];
	BYTE BcdInvNum[4];   //added by Allen 2016-05-26 Invoice Number(max len 8) ->BcdInvNum
    int  lbCount;
    BYTE TmpStr[40];
	BYTE tmp[25] = {0};
	BYTE Bcd_tmp[20] = {0};
    
    HDGRead(&lrHDGBuf);
    
    //////Trace Number----------------------------------------------------------------------
    if(lrHDGBuf.TraceNum>9999)
       // lrHDGBuf.TraceNum=0;      //这里可以为0吗？
	    lrHDGBuf.TraceNum = 0;   // modified by Allen 20151109
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
	length = strlen(gPTStruct.Pan);
	sprintf(tmp, "%d%s", length, gPTStruct.Pan);
	Utils_ConvertAscToBcd(tmp, Bcd_tmp, strlen(tmp), 0, PAD_NONE,0);
	memcpy(ISOData.Field2, Bcd_tmp,strlen(Bcd_tmp));
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
	   (Type==ISO_BATCH_UPLOAD_ADVICE) || //Orginal Date Time
	   (Type==ISO_VOIDREVERSAL_REWARD) ||
	   (Type==ISO_VOIDREVERSAL_REDEEM))   //added by Allen 2016-05-30
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

	if((Type!=ISO_VOIDREVERSAL_REWARD) && (Type!=ISO_VOIDREVERSAL_REDEEM))  //added by Allen 2016-05-30  ISO_VOIDREVERSAL no need field7
	{
		memcpy(ISOData.Field7,Bcd,2);		
	}

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
	   (Type==ISO_BATCH_UPLOAD_ADVICE) || //Orginal Date Time
	   (Type==ISO_VOIDREVERSAL_REWARD) ||
	   (Type==ISO_VOIDREVERSAL_REDEEM))  //added by Allen 2016-05-30   
	{
		memcpy(lbTmpTotal,gPTStruct.TransTime,6);
		lbTmpTotal[6]=0;
		Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 6, 0, PAD_NONE,0);	
		//StrAscToBcd(lbTmpTotal, Bcd);
		if((Type!=ISO_VOIDREVERSAL_REWARD) && (Type!=ISO_VOIDREVERSAL_REDEEM))  //added by Allen 2016-05-30
		{
			memcpy(ISOData.Field7+2,Bcd,5);
		}
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
	if(Type!=ISO_BATCH_UPLOAD_ADVICE && Type!=ISO_VOIDREVERSAL_REWARD && Type!=ISO_VOIDREVERSAL_REDEEM) //Orginal Stan  //modified by Allen (added "Type!=ISO_VOIDREVERSAL")
	{
		L1ConvIntAsc(lrHDGBuf.TraceNum, lbTmpTotal);     //通过交易序列号来做11域的值
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
	   (Type==ISO_GETBALANCE)||
	   (Type==ISO_VOIDREVERSAL_REWARD ||
	   (Type==ISO_VOIDREVERSAL_REDEEM))  //added by Allen 2016-05-30
// 	   ||
// 	   (Type==ISO_RETAIL)      // Hided by Kim_LinHB 2013-3-4 build 0.05.0009
	   )
    {
		//memcpy(ISOData.Field22,"\x00\x52",2);
		memcpy(ISOData.Field22,"\x09\x02",2);   //modified by Allen 2016-05-23 \x22 means swipe card and the terminal can not get the PIN 
    }

	if((Type==ISO_INSTALLMENT_ADVICES)||
	   (Type==ISO_RECONCILIATION_ADVICES)
	   ||						//
	   (Type==ISO_RETAIL)		// Added by Kim_LinHB 2013-3-4 build 0.05.0009
	   )
    {
		//memcpy(ISOData.Field22,"\x00\x51",2);
		memcpy(ISOData.Field22,"\x09\x01",2);    //modified by Allen 2016-05-30
    }
	else if(Type==ISO_RECONCILIATION)
    {
		memcpy(ISOData.Field22,"\x00\x12",2);
    }
		
    ///////////////////////////////////////////////////
    ///Card Sequence Number-----------------------------------------------------------------------
    ///////////////////////////////////////////////////
	/*           //removed by Allen 2016-05-30 swipe card no need field23
    memcpy(ISOData.Field23,"\x00\x03",2);
    ISOData.Field23[1]=gPTStruct.PSN;
   */
    ///////////////////////////////////////////////////
    ///NII----------------------------------------------------------------------------------------
    ///////////////////////////////////////////////////

	//memcpy(ISOData.Field24,"\x00\x03",2);
	//by jarod 20110803  
	if(Type!=ISO_VOIDREVERSAL_REWARD)
	{
		memcpy(ISOData.Field24,gNETSERVERNII,2);     //Field 24 is mandatory for online authorized transactions and shall contain ?02?value.
	}
	else
	{
		memcpy(ISOData.Field24,"\x04\x00",2);
	}
	

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
    
  //  Utils_ConvertBcdToAsc(gPTStruct.TrackISO2,TmpStr,19);		
   // TmpStr[16]=0x3D;
    
    ISOData.Field35[0]= Bcd[0];
    memcpy(ISOData.Field35+1, gPTStruct.TrackISO2, gPTStruct.Tr2Len);
	
    ///////////////////////////////////////////////////
    ////Retrival Ref---------------------------------------------------------------------
    ///////////////////////////////////////////////////
	if(Type != ISO_VOIDREVERSAL_REWARD && Type!=ISO_VOIDREVERSAL_REDEEM)      //modified by Allen 2016-07-04
	{
		memcpy(ISOData.Field37,gPTStruct.RetrivalRef,12);
	}
	else
	{
		memcpy(ISOData.Field37,gPTStruct.RefNumber,12);
	}
	
    ///////////////////////////////////////////////////
    ////Response Code--------------------------------------------------------------------
    ///////////////////////////////////////////////////
	if(Type!=ISO_VOIDREVERSAL_REWARD && Type!=ISO_VOIDREVERSAL_REDEEM)  // modified by Allen 2016-05-30  ISO_VOIDREVERSAL no need field39
	{
		memcpy(ISOData.Field39,"Y1",2);
	}
	
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
		memcpy(ISOData.Field47+lbCount,"\x00\x41", 2);                   lbCount=lbCount+2;
		memcpy(ISOData.Field47+lbCount,"\xE2\x27", 2);                   lbCount=lbCount+2;
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

		//lbTmpTotal[0]=(gPTStruct.TotalInstallmentCount/10)+'0';
		//lbTmpTotal[1]=(gPTStruct.TotalInstallmentCount%10)+'0';lbTmpTotal[2]=0;
		//modified by richard 20150727
		lbTmpTotal[0]='0';
		lbTmpTotal[1]='1';lbTmpTotal[2]=0;
		Utils_ConvertAscToBcd(lbTmpTotal, lbTmpTotal1, 2, 0, PAD_NONE,0);
		//StrAscToBcd(lbTmpTotal,lbTmpTotal1);
		ISOData.Field47[lbCount]=lbTmpTotal1[0]; lbCount=lbCount+1;
        //---------------------------------------------------------------------------------
		memset(Bcd_tmp, 0, sizeof(Bcd_tmp));
		memcpy(ISOData.Field47+lbCount,"\xC6\x03\x54", 3);               lbCount=lbCount+3;  //modified by Allen 2016-06-27 use tag C6 store PIN value
		Utils_ConvertAscToBcd(gPTStruct.PIN, Bcd_tmp, 4, 0, PAD_NONE,0);
		memcpy(ISOData.Field47+lbCount,Bcd_tmp, 2);    lbCount=lbCount+2;
		//memcpy(ISOData.Field47+lbCount,"\x00\x00\x00\x00\x00\x00", 6);    lbCount=lbCount+6;
		//memcpy(ISOData.Field47+lbCount,gPTStruct.ProfitAmountBcd, 6);    lbCount=lbCount+6;
        //---------------------------------------------------------------------------------
        
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
			memcpy(ISOData.Field47+lbCount,"\x39\x39\x39", 3); lbCount=lbCount+3;
		}
		else
		{
			memcpy(ISOData.Field47+lbCount,"\x30\x30\x35", 3); lbCount=lbCount+3;
		}
        
        //---------------------------------------------------------------------------------
		// Added by Kim_LinHB 2013-3-5 build 0.05.0009
		if(Type == ISO_RETAIL)
		{
			memcpy(ISOData.Field47+lbCount,"\xC8\x07\x46", 3);    lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,"000000", 6);          lbCount=lbCount+6;
		}

		// Add End
	}
	// Added by Kim_LinHB 2013-3-5 build 0.05.0009
	//modified by Allen 2016-07-11
	else if((Type==ISO_GETBALANCE))
	{
		if(memcmp(gPTStruct.TransType,TYPE_72,2)!=0)
		{
			//modified by richard 20150717, change the message 
			//of ISO_GETBALANCE to get mini-statement
			lbCount=0;
			memcpy(ISOData.Field47+lbCount,"\x00\x14", 2);     lbCount=lbCount+2;
			memcpy(ISOData.Field47+lbCount,"\xE2\x0C", 2);     lbCount=lbCount+2;

			memcpy(ISOData.Field47+lbCount,"\xC1\x04\x54", 3); lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,"MST", 3);          lbCount=lbCount+3;
			//added by Allen 
			//add end
	/*
			memcpy(ISOData.Field47+lbCount,"\xC3\x03\x46", 3); lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,gPTStruct.CurrencyCode, 2);       lbCount=lbCount+2;

			memcpy(ISOData.Field47+lbCount,"\xC8\x07\x46", 3);               lbCount=lbCount+3;
			memcpy(lbTmpTotal,gPTStruct.EntryAmount,LG_AMOUNT);		
			lbTmpTotal[LG_AMOUNT]=0;
			Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 12, 0, PAD_NONE,0);
			memcpy(ISOData.Field47+lbCount,Bcd, 6);          lbCount=lbCount+6;
	*/
			memcpy(ISOData.Field47+lbCount,"\xCA\x04\x54", 3); lbCount=lbCount+3;
			if(!bBalanceFromOptions)
			{
	// 			memset(lbTmpTotal, '0', 4);
	// 			if(0x00 == gPTStruct.ProductCode[1])
	// 				memcpy(lbTmpTotal + 2,gPTStruct.ProductCode, 1);
	// 			else
	// 				memcpy(lbTmpTotal + 1,gPTStruct.ProductCode, 2);
	// 			memcpy(ISOData.Field47+lbCount,lbTmpTotal, 3); lbCount=lbCount+3;
				memcpy(ISOData.Field47+lbCount,"\x39\x39\x39", 3); lbCount=lbCount+3;
			}
			else
				//memcpy(ISOData.Field47+lbCount,"\x00\x00\x00", 3); lbCount=lbCount+3;
				memcpy(ISOData.Field47+lbCount,"\x39\x39\x39", 3); lbCount=lbCount+3;
				
		}
		else
		{
			//modified by richard 20150717, change the message 
			//of ISO_GETBALANCE to get mini-statement
			lbCount=0;
			memcpy(ISOData.Field47+lbCount,"\x00\x1B", 2);     lbCount=lbCount+2;
			memcpy(ISOData.Field47+lbCount,"\xE2\x13", 2);     lbCount=lbCount+2;

			memcpy(ISOData.Field47+lbCount,"\xC1\x04\x54", 3); lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,"MST", 3);          lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,"\xC9\x05\x54", 3); lbCount=lbCount+3;

			Utils_ConvertAscToBcd(gPTStruct.InvoiceNum, BcdInvNum, 8, 0, PAD_RIGHT, 'F');	
			memcpy(ISOData.Field47+lbCount,BcdInvNum, 4); lbCount=lbCount+4;

	/*
			memcpy(ISOData.Field47+lbCount,"\xC3\x03\x46", 3); lbCount=lbCount+3;
			memcpy(ISOData.Field47+lbCount,gPTStruct.CurrencyCode, 2);       lbCount=lbCount+2;

			memcpy(ISOData.Field47+lbCount,"\xC8\x07\x46", 3);               lbCount=lbCount+3;
			memcpy(lbTmpTotal,gPTStruct.EntryAmount,LG_AMOUNT);		
			lbTmpTotal[LG_AMOUNT]=0;
			Utils_ConvertAscToBcd(lbTmpTotal, Bcd, 12, 0, PAD_NONE,0);
			memcpy(ISOData.Field47+lbCount,Bcd, 6);          lbCount=lbCount+6;
	*/
			memcpy(ISOData.Field47+lbCount,"\xCA\x04\x54", 3); lbCount=lbCount+3;
			if(!bBalanceFromOptions)
			{
	// 			memset(lbTmpTotal, '0', 4);
	// 			if(0x00 == gPTStruct.ProductCode[1])
	// 				memcpy(lbTmpTotal + 2,gPTStruct.ProductCode, 1);
	// 			else
	// 				memcpy(lbTmpTotal + 1,gPTStruct.ProductCode, 2);
	// 			memcpy(ISOData.Field47+lbCount,lbTmpTotal, 3); lbCount=lbCount+3;
				memcpy(ISOData.Field47+lbCount,"\x39\x39\x39", 3); lbCount=lbCount+3;
			}
			else
				//memcpy(ISOData.Field47+lbCount,"\x00\x00\x00", 3); lbCount=lbCount+3;
				memcpy(ISOData.Field47+lbCount,"\x39\x39\x39", 3); lbCount=lbCount+3;
			
		}
	}
	// Modified End
	//added by Allen 2016-05-26
	else if((Type==ISO_PAYMENT))
	{
		lbCount=0;
		memcpy(ISOData.Field47+lbCount,"\x00\x09", 2);     lbCount=lbCount+2;
		memcpy(ISOData.Field47+lbCount,"\xE2\x07", 2);     lbCount=lbCount+2;	
		memcpy(ISOData.Field47+lbCount,"\xC9\x04\x54", 3); lbCount=lbCount+3;

		Utils_ConvertAscToBcd(gPTStruct.InvoiceNum, BcdInvNum, 8, 0, PAD_RIGHT, 'F');	
		memcpy(ISOData.Field47+lbCount,BcdInvNum, 4); lbCount=lbCount+4;
	}
	//add end

    ///////////////////////////////////////////////////
    ///Currency Code
    ///////////////////////////////////////////////////
	//DebugComSend("gPTStruct.CurrencyCode  = %02x%02x\n", gPTStruct.CurrencyCode[0], gPTStruct.CurrencyCode[1]);
    memcpy(ISOData.Field49,gPTStruct.CurrencyCode,2);          
//	memcpy(ISOData.Field49,"\x00\x48",2);
	
	/*if (Type == ISO_GETBALANCE)															
    memset(sendData.Field52,0x00,8);*/
/*       the PIN is not actual PIN , the servers need to compare it manually , that why we use the F47.c6
	//added by Allen 2016-05-24
	if (Type == ISO_RETAIL)  //TYPE70
	{
		memcpy(ISOData.Field52, gPTStruct.PIN, 4);
	}
	//add end
  */  
    ///////////////////////////////////////////////////
    ///ICC Related Data-----------------------------------------------------------------
    ///////////////////////////////////////////////////
	/*   removed by Allen swipe card no need field55  2016-05-30
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
	else if(Type == ISO_VOIDREVERSAL_REWARD)
	{
		memcpy(ISOData.Field60,"\x00\x04", 2);
		//memcpy(ISOData.Field60+2,gPTStruct.EntryAmount, 12);
		memcpy(ISOData.Field60+2,"0200", 4);
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


	//DebugComSend("------------------package completed!----------------------\n");
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
	iRet = ISO_Transaction(Type);                       //问题还是就出在这个函数！！
	//OS_ClearScreen();                                        //add by Allen 20151109
	//ScrPrint(0, 0, ASCII, "Allen test");
	//ScrPrint(0, 2, ASCII, "ISO_Transaction: %x", iRet);
	//getkey();
	//OS_ClearScreen();
	//DebugComSend("------------------ISO_Transaction completed!----------------------\n");
	if(iRet == ISO_STAT_OK)               //有可能是这里根本没有返回OK！
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
			//DebugComSend("1111111111111 \n");
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

            //test by richard 20151030
            //OS_ClearScreen();
            //ScrPrint(0, 0, ASCII, "richard test");
            //ScrPrint(0, 2, ASCII, "response CODE:");
            //ScrPrint(0, 6, ASCII, " field39: %c%c", ISOData.Field39[0], ISOData.Field39[1]);
            //getkey();
            //OS_ClearScreen();

			if((ISOData.Field39[0]!='0')||(ISOData.Field39[1]!='0'))
			{
				//DebugComSend("222222222222222\n");
				//DebugComSend("ISOData.Field39 = %c%c\n", ISOData.Field39[0], ISOData.Field39[1]);
//MK 02112007 Lost and Stolen Card
                if((ISOData.Field39[0]=='5')&&(ISOData.Field39[1]=='1') || (ISOData.Field39[0]=='0')&&(ISOData.Field39[1]=='5'))
                {
                    ucDoNotHonorFlag = 1;
                }
				ISO_DisPlayErrMsg(ISOData.Field39);
				if(Type==ISO_RETAIL)
				{
					if(((ISOData.Field39[0]=='4')&&(ISOData.Field39[1]=='1'))||
					   ((ISOData.Field39[0]=='4')&&(ISOData.Field39[1]=='3')))
					{
					/*	
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
				//DebugComSend("3333333333\n");
				return(STAT_INV);
			}
			else
			{
				unsigned char err_code;
				//DebugComSend("4444444444444\n");
				if(Type==ISO_RETAIL)
				{
					/*
					if(ScriptParser()!=STAT_OK)
					{
						DisplayString((PCHAR)"SCRIPT ERROR",2000);
						return(STAT_INV);
					}
					*/
				}
				else
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
				//added by Allen 2016-06-16
				if((Type==ISO_GETBALANCE)
					|| (Type==ISO_VOIDREVERSAL_REWARD) || (Type==ISO_RETAIL)
					|| (Type==ISO_VOIDREVERSAL_REDEEM))
				{
					//DebugComSend("55555555555\n");
					Parse47_Points(&err_code, Type);
					//ParseF54();
				}
				//add end
				if(Type==ISO_GETBALANCE)
				{
					//ParseF54();    //removed by Allen 2016-06-28dongle 
					// Added by Kim_LinHB 2013-3-8 build 0.05.0009
					//memcpy(gPTStruct.Balance, "001000000000", LG_AMOUNT); // Added by Kim_LinHB 2013-3-19 build 0.05.0009 for test
					if(!bBalanceFromOptions)
					{
						if(Parse47(&err_code) != STAT_OK)
						{
							return(STAT_INV);
						}
					}
                    else 
                    {
                        //modified by richard 20150723
                         if(Parse47_MST(&err_code) != STAT_OK)
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
	//DebugComSend("------------------1111111111----------------------\n");
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
        return (ISO_REVERSAL_PENDING);
    }
	//DebugComSend("------------------22222222222----------------------\n");
	ISOPacketLength = ISO_BuildFrame(trType);     //在这里组好要发送的包， ISOPacketLength保存着包的长度
	//DebugComSend("ISOPacketLength = %d\n", ISOPacketLength);
	if ((ISOPacketLength == ISO_FRAME_ERROR) || (ISOPacketLength < 15 ))
	{
		return (ISO_FRAME_FAULT);/*at least header must be sent */
	}
	//DebugComSend("------------------333333333333----------------------\n");
    if ((frameToSend.msgType[0] == 0x02) || (frameToSend.msgType[0] == 0x04))
    {
	    retVal = ISO_AddReversal ();
	    switch (retVal)
	    {
		    case ISO_STAT_OK :
				break;

		    default:
				return (ISO_APP_ERROR);
	    }
    }

	//DebugComSend("------------------44444444444----------------------\n");
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
	//	else           removed by Allen 20151110
	//	{
			if (sendRetry == 1)
			{
				retVal = ISO_Send(ISOPacketLength, PORT_1,sendMessage,trType);
			}
			else
			{
                retVal = ISO_STAT_OK; // Packet sent before just try to receive;
			}
			//OS_ClearScreen();                                          //add by Allen 20151110
			//ScrPrint(0, 0, ASCII, "Allen test");
			//ScrPrint(0, 2, ASCII, "send = %x", retVal);
			//getkey();
			//OS_ClearScreen();			
			
			if (retVal == ISO_STAT_OK) // if packet sent successfully
			{
                sendRetry = 0;
				OS_ClearScreen();	// Gary 2011-1-26
				//DebugComSend("------------------send package success!----------------------\n");
				retVal = ISO_Receive(30, PORT_1, receiveMessage);
				//OS_ClearScreen();                                          //add by Allen 20151110
				//ScrPrint(0, 0, ASCII, "Allen test");
				//ScrPrint(0, 2, ASCII, "Recv = %x", retVal);
				//getkey();
				//OS_ClearScreen();		

                if (retVal == ISO_STAT_OK) // if packet received successfully first resolve fields
                {
                    /////////////////////////////////////////////
                    /////////////////////////////////////////////
                    //TPDU
					//DebugComSend("------------------receive package success!----------------------\n");
                    if((trType == ISO_BATCH_UPLOAD_ADVICE)||
                        (trType == ISO_INSTALLMENT_ADVICES)||
                        (trType == ISO_LOYALTY_ADVICE)||
                        (trType == ISO_RECONCILIATION_ADVICES))
                    {
                        return (retVal);
                    }
                    /////////////////////////////////////////////
                    /////////////////////////////////////////////
                    if ( ISO_ResolveFrame() == ISO_OK) // check incoming packet to be correct   在这里将接受的域的值拷贝到相印的域中
					{     // If packet is structurely correct, check incoming STAN, Terminal ID with outgoing message
						if (memcmp(sendData.Field11,receivedData.Field11,3) != 0)
						{
							//OS_ClearScreen();                                          //add by Allen 20151109
							//ScrPrint(0, 0, ASCII, "Allen test");
							//ScrPrint(0, 2, ASCII, "SField11 = %x %x %x", sendData.Field11[0], sendData.Field11[1], sendData.Field11[2]);
							//ScrPrint(0, 6, ASCII, "RFiele11 = %x %x %x", receivedData.Field11[0], receivedData.Field11[1], receivedData.Field11[2]);
							//getkey();
							//OS_ClearScreen();
//						if ((trType != ISO_REVERSAL)&&(memcmp(sendData.Field11,STAN,3) != 0))					
							return (ISO_COMM_ERROR);       //这里的错误
						}
//						if ((trType == ISO_REVERSAL)&&(memcmp(receivedData.Field11,STAN,3) != 0))
//							return (ISO_COMM_ERROR);
//						if ((trType != ISO_REVERSAL)&&(memcmp(sendData.Field42,TID,8) != 0))
//							return (ISO_COMM_ERROR);
						if ((trType == ISO_REVERSAL)&&(memcmp(receivedData.Field42,TID,8) != 0))
						{
							return (ISO_COMM_ERROR);       //或者是这里的错误
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
	fileHandle = Open (FILE_REVERSE, O_CREATE|O_RDWR);
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


	//DebugComSend("Fields[%d].data = %s\n", fieldNo, data);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////       WARNING!!! Modifying frame pointer (*frameLocator) on this module will cause accessive damage!!!        ////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// if field is not present on the bitmap pass it over
	if (!(((frameToSend.bitMap[fieldNo/8-((fieldNo%8) ? 0:1)]) >> ((8-fieldNo%8)%8)) & 0x01)) return (ISO_INSERTED);

	 fieldNo--;


	// if field definitions are wrong return error
	if (Fields[fieldNo].type == TYPE_NONE) 
	{
		//DebugComSend("Fields[%d].type = TYPE_NONE\n", fieldNo);
		return (ISO_TYPE_ERROR);
	}
	if (Fields[fieldNo].sizeRange == FIELD_NOT_USED) 
	{
		//DebugComSend("Fields[%d].sizeRange = FIELD_NOT_USED\n", fieldNo);
		return (ISO_SIZE_ERROR);
	}

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
			//DebugComSend("Fields[%d]  ISO_APP_ERROR\n", fieldNo);
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
		//DebugComSend("length = %d\n", length);
		//if given data size is bigger than size declared return error
		if (length > Fields[fieldNo].fldSize)
		{
			///DebugComSend("Fields[%d].fldSize = ISO_SIZE_ERROR\n", fieldNo);
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
			//DebugComSend("Fields[%d].fldSize111 = ISO_SIZE_ERROR\n", fieldNo);
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
			//DebugComSend("Fields[%d].fldSize222 = ISO_SIZE_ERROR\n", fieldNo);
            return (ISO_SIZE_ERROR);
        }
        length = (((lengthAscii[0])&0x0F)*100 + ((lengthAscii[1]>>4)&0x0F)*10 + (lengthAscii[1] & 0x0F));
        
        //if given data size is bigger than size declared return error
        if (length > Fields[fieldNo].fldSize)
        {
            sprintf(buffer, "SIZE ERROR2, FIELD %d", (int)fieldNo);
            Utils_DebugPrintMsg(buffer);
			//DebugComSend("Fields[%d].fldSize333 = ISO_SIZE_ERROR\n", fieldNo);
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
		//DebugComSend("Fields[%d]11    ISO_TYPE_ERROR \n", fieldNo);
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

int ISO_BuildFrame (BYTE trType)     //将所有FrameToSend中的元素 赋值  ，返回整个要发送的Send 的长度
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
	
		retVal = ISO_AddField(&frameLocator , fieldNo, Fields[fieldNo-1].ptrSendData);    //把Fields[fieldNo-1].ptrSendData的值拷贝给frameLocator
		if ( retVal != ISO_INSERTED )
		{
			return (ISO_FRAME_ERROR);
		}					
	} 
																	 //   1	+   2    +    2   +  2  +  8   = 15 bytes
	return ( (int)frameLocator - (int)frameToSend.Data + 15 /* TPDU+DESTADDR+ORIGADDR+MSGID+BITMAP*/);
}



int ISO_ResolveFrame ()    //其实就是将接收到的数据包中关域0-64个的值拷贝到相印的域中
{
    unsigned char *frameLocator = (unsigned char *)frameReceived.Data;
    unsigned char retVal;
    short int     fieldNo;

	for ( fieldNo = 1; fieldNo < 65 ; fieldNo++)
	{
		retVal = ISO_GetField(&frameLocator , fieldNo, (unsigned char *)Fields[fieldNo-1].ptrReceivedData);    // 把frameLocator的值拷贝到Fields[fieldNo-1].ptrReceivedData
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
   //DebugComSend("SendBuf = %s\n", TempBuf); 
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

	//DebugComSend("ParseF54 = %s \n", ISOData.Field54);  
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
			memcpy(gPTStruct.Balance,ISOData.Field54+2+4+3+1,12);   // balance amount
			memset(out,0,13);
			memcpy(out,gPTStruct.Balance,12);
			//DebugComSend("gPTStruct.Balance:%s \n", out); 
			mCmmnUtil_AsciiToBcd(out, gCardParam.UpperLimit, 12);  
		}
	}
}				

//added by Allen 2016-11-01
int RemoveZeroPoints(unsigned char *EarnedPoints)
{
	int i;
	unsigned char *pErnPoins;
	if(EarnedPoints == NULL)
		return -1;
	pErnPoins = EarnedPoints;
	//DebugComSend("pErnPoins = %s \n", pErnPoins); 
	while(*pErnPoins != 0)
		pErnPoins++;
	pErnPoins--;
	for(i = 0; i < 3; i++)
	{
		//DebugComSend("EarnedPoints = %s \n", EarnedPoints); 
		if(*pErnPoins == '0' && i != 2)
		{
			*pErnPoins = 0;
			pErnPoins--;
		}
		else if(*pErnPoins == '0' && i == 2)
		{
			*(pErnPoins - 1) = 0;
			return 0;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
//add end

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

//added by Allen 2016-07-12
int CaculatePoints(unsigned char *ConversionRate, unsigned char *EarnedPoints)
{
	int iConversionRate, i;
	double dAmount, dEarnedPoints;
	unsigned char *pAmountBegin, *pErnPoins;
	unsigned char ucAmount[13] = {0};
	if(ConversionRate == NULL)
	{
		return -1;
	}
	iConversionRate = atoi(ConversionRate);
	//DebugComSend("iConversionRate = %d \n", iConversionRate); 
	pAmountBegin = gPTStruct.EntryAmount;
	//pAmountEnd = pAmountBegin + 9;
	while(*pAmountBegin == '0' && *(pAmountBegin + 3) != 0)
		pAmountBegin++;
	FormatPoints(ucAmount, pAmountBegin);
	//memcpy(ucAmount, pAmountBegin, pAmountEnd - pAmountBegin);
	//DebugComSend("ucAmount = %s \n", ucAmount); 
	dAmount = strtod(ucAmount, NULL);
	//DebugComSend("dAmount = %lf \n", dAmount); 
	dEarnedPoints = dAmount * iConversionRate / 100;
	//DebugComSend("iEarnedPoints = %.3lf \n", dEarnedPoints); 
	sprintf(EarnedPoints, "%.3lf", dEarnedPoints);
	//DebugComSend("EarnedPoints = %s \n", EarnedPoints); 
	RemoveZeroPoints(EarnedPoints);
	/*
	pErnPoins = EarnedPoints;
	//DebugComSend("pErnPoins = %s \n", pErnPoins); 
	while(*pErnPoins != 0)
		pErnPoins++;
	pErnPoins--;
	for(i = 0; i < 3; i++)
	{
		//DebugComSend("pErnPoins = %s \n", pErnPoins); 
		if(*pErnPoins == '0' && i != 2)
		{
			*pErnPoins = 0;
			pErnPoins--;
		}
		else if(*pErnPoins == '0' && i == 2)
		{
			*(pErnPoins - 1) = 0;
			return 0;
		}
		else
		{
			return 0;
		}
	}
	*/
	return 0;
}

void GetTotalBalance(char *pPointsBlance)
{	
	double dOriginPoints, dEarnedPoints;
	if(pPointsBlance == NULL)
		return;
	if(gPTStruct.PointsBalance[0] == 0 || gPTStruct.EarnedPoints[0] == 0)
		return;
	dOriginPoints = strtod(gPTStruct.PointsBalance, NULL);
	dEarnedPoints = strtod(gPTStruct.EarnedPoints, NULL);
	sprintf(pPointsBlance, "%.3lf", dOriginPoints + dEarnedPoints);
	return;
}

//add end
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

int Parse47_MST(unsigned char *err_code)
{
	unsigned int len47, lenF0, lenTag;
	BYTE *F0, *Tag;
	BYTE Value[50]; // 50 temp

	memset(Value, 0, sizeof(Value));
    memset(mst_info, 0, sizeof(mst_info));

	Utils_ConvertBcdToAsc( ISOData.Field47, Value, 2);
	Value[4]=0;
	len47 = atoi(Value);
	//len47 = 13;
	//DebugComSend("\nlen47 %d", len47);

    memset(Value, 0, sizeof(Value));

    if ( F0 = FindStr(ISOData.Field47, len47, "\xF0", 1))
	{
		lenF0 = *(F0 + 1);
		//lenF0 = 12;
        //DebugComSend("\nget F0 lenF0:%d", lenF0);
	}
	else
    {
        
        //DebugComSend("get F0 error");
        //DebugComSend("Field47: %s", ISOData.Field47);
        return STAT_OK;   
    }

	F0 += 2;	//skip "\xF0+1bit(len)"
	lenTag = 0;
 
	//F0.D1 will contain profit percentage. (for eg. 948 is 9.48 %)
	if ( Tag = FindStr(F0, lenF0, "\xD1", 1))
	{
		lenTag = *(Tag + 1);

        //get transtype
        mst_info[0].transType = *(Tag + 3);
        //DebugComSend("\nget D1 transType:%d \n", mst_info[0].transType);

        //get transAmount
        Utils_ConvertBcdToAsc((Tag + 4), Value, 4);
        sprintf(mst_info[0].transAmount, "0000%-8.8s", Value);
        //DebugComSend("get D1 transAmount:%s \n", mst_info[0].transAmount);

        //get transDate
        Utils_ConvertBcdToAsc((Tag + 8), Value, 4);
        sprintf(mst_info[0].transDate, "%-8.8s", Value);
        //DebugComSend("get D1 transDate:%s \n", mst_info[0].transDate);  

        //get transTime
        Utils_ConvertBcdToAsc((Tag + 12), Value, 2);
        Value[4] = 0;
        sprintf(mst_info[0].transTime, "%-4.4s", Value);
        //DebugComSend("get D1 transTime:%s \n", mst_info[0].transTime);  
	}

    F0 += lenTag;
    lenTag = 0;
    //F0.D2
    memset(Value, 0, sizeof(Value));
	if ( Tag = FindStr(F0, lenF0, "\xD2", 1))
	{
		lenTag = *(Tag + 1);

        //get transtype
        mst_info[1].transType = *(Tag + 3);

        //get transAmount
        Utils_ConvertBcdToAsc((Tag + 4), Value, 4);
        sprintf(mst_info[1].transAmount, "0000%-8.8s", Value);

        //get transDate
        Utils_ConvertBcdToAsc((Tag + 8), Value, 4);
        sprintf(mst_info[1].transDate, "%-8.8s", Value);

        //get transTime
        Utils_ConvertBcdToAsc((Tag + 12), Value, 2);
        Value[4] = 0;
        sprintf(mst_info[1].transTime, "%-4.4s", Value);
	}

    
    F0 += lenTag;
    lenTag = 0;
    //F0.D3
    memset(Value, 0, sizeof(Value));
	if ( Tag = FindStr(F0, lenF0, "\xD3", 1))
	{
		lenTag = *(Tag + 1);

        //get transtype
        mst_info[2].transType = *(Tag + 3);

        //get transAmount
        Utils_ConvertBcdToAsc((Tag + 4), Value, 4);
        sprintf(mst_info[2].transAmount, "0000%-8.8s", Value);

        //get transDate
        Utils_ConvertBcdToAsc((Tag + 8), Value, 4);
        sprintf(mst_info[2].transDate, "%-8.8s", Value);

        //get transTime
        Utils_ConvertBcdToAsc((Tag + 12), Value, 2);
        Value[4] = 0;
        sprintf(mst_info[2].transTime, "%-4.4s", Value);

	}

    F0 += lenTag;
    lenTag = 0;
    //F0.D4
    memset(Value, 0, sizeof(Value));
	if ( Tag = FindStr(F0, lenF0, "\xD4", 1))
	{
		lenTag = *(Tag + 1);

        //get transtype
        mst_info[3].transType = *(Tag + 3);

        //get transAmount
        Utils_ConvertBcdToAsc((Tag + 4), Value, 4);
        sprintf(mst_info[3].transAmount, "0000%-8.8s", Value);

        //get transDate
        Utils_ConvertBcdToAsc((Tag + 8), Value, 4);
        sprintf(mst_info[3].transDate, "%-8.8s", Value);

        //get transTime
        Utils_ConvertBcdToAsc((Tag + 12), Value, 2);
        Value[4] = 0;
        sprintf(mst_info[3].transTime, "%-4.4s", Value);
	}
    

    F0 += lenTag;
    lenTag = 0;
    //F0.D5
    memset(Value, 0, sizeof(Value));
	if ( Tag = FindStr(F0, lenF0, "\xD5", 1))
	{
		lenTag = *(Tag + 1);

        //get transtype
        mst_info[4].transType = *(Tag + 3);

        //get transAmount
        Utils_ConvertBcdToAsc((Tag + 4), Value, 4);
        sprintf(mst_info[4].transAmount, "0000%-8.8s", Value);

        //get transDate
        Utils_ConvertBcdToAsc((Tag + 8), Value, 4);
        sprintf(mst_info[4].transDate, "%-8.8s", Value);

        //get transTime
        Utils_ConvertBcdToAsc((Tag + 12), Value, 2);
        Value[4] = 0;
        sprintf(mst_info[4].transTime, "%-4.4s", Value); 
	}

    memset(ISOData.Field47, 0, sizeof(ISOData.Field47));

    return STAT_OK;
    
}

// Add End


//added by Allen 2016-06-16
int Parse47_Points(unsigned char *err_code, unsigned char Type)
{
	unsigned int len47, lenF0, lenTag;
	BYTE *F0, *Tag;
	BYTE Value[50]; // 50 temp
	BYTE ConversionRate[6] = {0};
	BYTE PointsBalance[13] = {0};

	memset(Value, 0, sizeof(Value));

	//DebugComSend("F47(1):%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x \n",  ISOData.Field47[2],ISOData.Field47[3], ISOData.Field47[4], ISOData.Field47[5], ISOData.Field47[6], ISOData.Field47[7], ISOData.Field47[8], ISOData.Field47[9], ISOData.Field47[10],  ISOData.Field47[11]); 
	//DebugComSend("F47(2):%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x \n",  ISOData.Field47[12], ISOData.Field47[13], ISOData.Field47[14], ISOData.Field47[15], ISOData.Field47[16], ISOData.Field47[17], ISOData.Field47[18], ISOData.Field47[19], ISOData.Field47[20], ISOData.Field47[21]); 
	Utils_ConvertBcdToAsc( ISOData.Field47, Value, 2);
	Value[4]=0;
	len47 = atoi(Value);
	//DebugComSend("len F47:%d \n",  len47); 
	memset(Value, 0, sizeof(Value));

	if ( F0 = FindStr(ISOData.Field47 + 2, len47, "\xF0", 1))
	{
		lenF0 = *(F0 + 1);
	}
	else
		return STAT_NOK;

	F0 += 2;	//skip "\xF0+1bit(len)"
	//DebugComSend("ISOF47--F0\n");  
	//F0.D1 will contain point balance.
	memset(Value, 0, sizeof(Value));
	if ( Tag = FindStr(F0, lenF0, "\xD1", 1))
	{
		lenTag = *(Tag + 1) - 1;
		Utils_ConvertBcdToAsc((Tag + 3), Value, lenTag);
		sprintf(PointsBalance, "%*.*s",lenTag*2, lenTag*2, Value);
		if(PointsBalance[lenTag*2-1] == 'F')
		{
			PointsBalance[lenTag*2-1] = 0;
		}
		//memset(PointsBalance, 0, sizeof(PointsBalance));
		//memcpy(PointsBalance, "120", 3);
		FormatPoints(gPTStruct.PointsBalance, PointsBalance);
		RemoveZeroPoints(gPTStruct.PointsBalance);
		//DebugComSend("get D1 PointsBalance:%s \n", gPTStruct.PointsBalance); 
	}

	//F0.D2 will contain amount balance
	memset(Value, 0, sizeof(Value));
	if ( Tag = FindStr(F0, lenF0, "\xD2", 1))
	{
		lenTag = *(Tag + 1) - 1;
		Utils_ConvertBcdToAsc((Tag + 3), Value, lenTag);
		sprintf(gPTStruct.Balance, "%*.*s",lenTag*2, lenTag*2, Value);
		if(gPTStruct.Balance[lenTag*2-1] == 'F')
		{
			gPTStruct.Balance[lenTag*2-1] = 0;
			lenTag = lenTag*2 - 1;
		}
		else
		{
			lenTag = lenTag*2;
		}
		if (lenTag<LG_AMOUNT)
        {
            memmove(gPTStruct.Balance+12-lenTag, gPTStruct.Balance, lenTag+1);
            memset(gPTStruct.Balance, '0', 12-lenTag);
        }
		//DebugComSend("get D2 Balance:%s \n", gPTStruct.Balance); 
	}

	//F0.D3 will contain reversal points or earned points.
	memset(Value, 0, sizeof(Value));
	if ( Tag = FindStr(F0, lenF0, "\xD3", 1))
	{
		lenTag = *(Tag + 1) - 1;
		Utils_ConvertBcdToAsc((Tag + 3), Value, lenTag);
		if((Type == ISO_GETBALANCE && memcmp(gPTStruct.TransType,TYPE_76,2)==0) || Type==ISO_VOIDREVERSAL_REDEEM)  
		{
			//sprintf(gPTStruct.ReversalPoints, "%*.*s",lenTag*2, lenTag*2, Value);
			sprintf(ConversionRate, "%*.*s",lenTag*2, lenTag*2, Value);
			if(ConversionRate[lenTag*2-1] == 'F')
			{
				ConversionRate[lenTag*2-1] = 0;
			}
			CaculatePoints(ConversionRate, gPTStruct.ReversalPoints);
			//DebugComSend("get D3 ReversalPoints:%s \n", gPTStruct.ReversalPoints); 
		}
		else if((Type == ISO_GETBALANCE) && (memcmp(gPTStruct.TransType,TYPE_72,2)==0))  // for balance inquiry D3 means conversion rate
		{
			sprintf(ConversionRate, "%*.*s",lenTag*2, lenTag*2, Value);
			if(ConversionRate[lenTag*2-1] == 'F')
			{
				ConversionRate[lenTag*2-1] = 0;
			}
			CaculatePoints(ConversionRate, gPTStruct.EarnedPoints);
			//DebugComSend("get D3 EarnedPoints:%s \n", gPTStruct.EarnedPoints); 
		}
		//DebugComSend("get D3 Points:%s \n", gPTStruct.ReversalPoints); 
	}

	return STAT_OK;
}