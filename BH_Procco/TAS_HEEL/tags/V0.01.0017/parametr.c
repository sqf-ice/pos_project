#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "debug.h"

#include "os_define.h"
#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"
#include "utility.h"
#include "Apiman.h"
#include "ucl_main.h"
#include "SLCLOCK.h"
#include "appLib.h"


static WORD ParameterRequest      (BYTE *fpWrkBuf,BYTE fAutoParam);
static WORD GeneralParameter      (BYTE *lpStr);
static WORD MerchantTypeList      (BYTE *fpWrkBuf, WORD *fpBlockNo);
static WORD BinList               (BYTE *fpWrkBuf, WORD *fpBlockNo);
static WORD KeyList               (BYTE *fpWrkBuf, WORD *fpBlockNo);
static WORD BlackCardList         (BYTE *fpWrkBuf, WORD *fpBlockNo,BYTE BlackList_DownType);
static WORD ProductList           (BYTE *fpWrkBuf, WORD *fpBlockNo);
static WORD SchemeList            (BYTE *fpWrkBuf, WORD *fpBlockNo);
static WORD RepaymentTenuresList  (BYTE *fpWrkBuf, WORD *fpBlockNo);
static void HCCPBinToBCD          (BYTE *fpStrBin, BYTE *fpStrAscii, BYTE fbLen);
static WORD AddBlackList          (BYTE *fpWrkBuf, BYTE RecLength);

WORD PrintParameterSlip(BYTE fbFailed,BYTE fbWrongTerm)
{
    HDGStruct lsHDGBuf;
    BYTE lpTime[13];
    BYTE lpLine[27];
    char sPrnBuffer1[LG_PRN_BUFFER];
    
    PrintBuffer_Init(sPrnBuffer1, sizeof(sPrnBuffer1));
    PrintBuffer_SelectFont(sPrnBuffer1, 16);    // modified by Allen 20151118
    PrnHead(sPrnBuffer1);
    
	PrintBuffer_SelectFont(sPrnBuffer1, 24);
    PrintBuffer_Eng(sPrnBuffer1, "\n");

    if (fbWrongTerm)
    {
        PrintBuffer_Eng(sPrnBuffer1, PRT_TERMINAL_SERIAL);
        PrintBuffer_Eng(sPrnBuffer1, "\n");
        PrintBuffer_Eng(sPrnBuffer1, PRT_NUMBER_WRONG);
        PrintBuffer_Eng(sPrnBuffer1, "\n\n\n\n");
    }
    else
    {
        PrintBuffer_Eng(sPrnBuffer1, (PCHAR)PRT_PDLD_PARAMDLOAD);
        
        if ( fbFailed )
        {
            PrintBuffer_Eng(sPrnBuffer1, "\n        %s\n", PRT_FAILED);
			PrintBuffer_Eng(sPrnBuffer1, "\n\n\n\n");
        }
        else
        {
            PrintBuffer_Eng(sPrnBuffer1, "\n        %s\n", PRT_END_SUCCESS);
            //Read General Parameter File
            if (HDGRead(&lsHDGBuf)!=STAT_OK)
            {
                return(STAT_NOK);
            }
            PrintBuffer_Eng(sPrnBuffer1, "\n\n\n\n ");
        }
    }

    // DateTime
    memset(lpLine, 0, sizeof(lpLine));
    ConvDate(lpTime); //DD-MM-YYYY
    memcpy(lpLine,lpTime,10);
    memset(lpLine+10,' ',9);
    ClkReadTime(lpTime);
    memcpy(lpLine+19,lpTime,2);
    lpLine[21]=':';
    memcpy(lpLine+22,lpTime+2,2);
    PrintBuffer_Eng(sPrnBuffer1, (PCHAR)lpLine);
	PrintBuffer_Eng(sPrnBuffer1, "                \n");
	PrintBuffer_Eng(sPrnBuffer1, "                \n");
	PrintBuffer_Eng(sPrnBuffer1, "                \n");
	PrintBuffer_Eng(sPrnBuffer1, "                \n");

    PrnInit();
    PrintBuffer_SendToPrinter(sPrnBuffer1);
    return StartPrinter();
}

GprsParamStruct	gpsGprsStr;
EthernetParamStruct epsEtStr;	
TcpipParamStruct tcpIpStr;		

WORD ParameterDownload(BYTE fbMerchantPhones, BYTE fAutoParam, BYTE Connect)
{
    WORD lwStatus;
    
    gHostType=PARAM_HOST;
    if(Connect==OS_TRUE)
    {
        if (ConnectHost_New() != STAT_OK)
        {
            return(STAT_NOK);
        }	
    }

    OS_ClearScreen(); 
    lwStatus = StartParameterDload(fAutoParam);
    Comms_Disconnect();
    
    if (lwStatus==STAT_OK)
    {
        PrintParameterSlip(OS_FALSE, OS_FALSE);
    }
    else if (lwStatus==STAT_EOT)//EOT Gelmesi Durumu
    {
        PrintParameterSlip(OS_FALSE, OS_TRUE);
    }
    else
    {
        PrintParameterSlip(OS_TRUE, OS_FALSE);
    }
    
    return(lwStatus);
}

WORD StartParameterDload(BYTE fAutoParam )
{
    WORD lwStatus;
    BYTE lpWrkBuf[/*257*/1024];
    WORD lwPrevBlockNo;
    
    BYTE MerchantTypeVersion[3];
    BYTE BinVersion[3];
    BYTE BlackListVersion[14];
    BYTE BlackList_DownType;
    BYTE ProductVersion[3];
    BYTE SchemeTypeVersion[3];
    BYTE RepaymentTenuresVersion[3];
    HDGStruct lsHDGBuf;
    
    lwPrevBlockNo=9;
    //////////////////////////////////////////////////////////////////////////////
    ///Parameter Request///////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    DisplayLongString(DSP_PARAM_REQUEST,0);
	//by jarod  download para
	lwStatus = ParameterRequest(lpWrkBuf, fAutoParam);
    if (lwStatus != STAT_OK)
    {
        return(lwStatus);
    }

    //////////////////////////////////////////////////////////////////////////////
    ///Version Control////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    DisplayString(DSP_PARAM_DLOADING,0);
    
    if (lpWrkBuf[1]!='P')
    {
        return(STAT_INV);
    }
    
    // New Parameter Versions
    memcpy(MerchantTypeVersion,    lpWrkBuf+2,   LG_PARMVERSION);
    memcpy(BinVersion,             lpWrkBuf+2+3, LG_PARMVERSION);
    memcpy(BlackListVersion,       lpWrkBuf+2+6, 14);
    BlackList_DownType            =lpWrkBuf[2+20];
    memcpy(ProductVersion,         lpWrkBuf+2+21,LG_PARMVERSION);
    memcpy(SchemeTypeVersion,      lpWrkBuf+2+24,LG_PARMVERSION);
    memcpy(RepaymentTenuresVersion,lpWrkBuf+2+27,LG_PARMVERSION);
    
    lpWrkBuf[0] = ACK;
    lpWrkBuf[1] = 0;
    if ((Send_ReceiveBuffer(lpWrkBuf, (WORD *)NULL, WAIT_MODE))!=STAT_OK)
	{
        return(STAT_INV);
	}
    if (lpWrkBuf[0]==EOT)
	{
        return(STAT_INV);
	}

    //////////////////////////////////////////////////////////////////////////////
    ///General Parameters/////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    DisplayString(DSP_OTHER_PARAMS_DLOADING, 500);

	lwStatus = GeneralParameter(lpWrkBuf); 
    if(lwStatus != STAT_OK)
        return(STAT_INV);
    lpWrkBuf[0]=ACK;
    lpWrkBuf[1]=0;
    lwStatus = Send_ReceiveBuffer(lpWrkBuf, (WORD *) NULL, NO_WAIT_MODE);
    if (lwStatus != STAT_OK)
        return(STAT_INV);
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
        return(STAT_NOK);

    //////////////////////////////////////////////////////////////////////////////
    ///Merchant Types/////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (lpWrkBuf[2] == 'M')
    {
		DisplayString("1. LOAD MERCH TYPE", 1000);
        lwStatus = MerchantTypeList(lpWrkBuf, &lwPrevBlockNo);
        if (lwStatus != STAT_OK)
            return(lwStatus);
        
        if (Delete(FILE_MERCH)!=0)
            return (STAT_INV);
        if (Rename(FILE_TEMPMERCH,FILE_MERCH)!=0)
            return (STAT_INV);
        
        if (HDGRead(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
        memcpy(lsHDGBuf.MerchantTypeVersion,MerchantTypeVersion,LG_PARMVERSION);
        if (HDGWrite(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
    }

    //////////////////////////////////////////////////////////////////////////////
    ///Bin List///////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (lpWrkBuf[2] == 'B')
    {
		DisplayString("2. LOAD BIN", 1000);
        lwStatus = BinList(lpWrkBuf, &lwPrevBlockNo);
        if (lwStatus != STAT_OK)
            return(lwStatus);
        if (Delete(FILE_BIN)!=0)
            return (STAT_INV);
        if (Rename(FILE_TEMPBIN,FILE_BIN)!=0)
            return (STAT_INV);
        
        if (HDGRead(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
        memcpy(lsHDGBuf.BinVersion,BinVersion,LG_PARMVERSION);
        if (HDGWrite(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
    }

    //////////////////////////////////////////////////////////////////////////////
    ///M-Chip Black List//////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (lpWrkBuf[2] == 'Y')
    {
        // Gary delet black list processing
        /*
	     DisplayString("3. LOAD BLACK", 1000);
        lwStatus = BlackCardList(lpWrkBuf, &lwPrevBlockNo,BlackList_DownType);
        if (lwStatus != STAT_OK)
            return(lwStatus);
        if(BlackList_DownType=='F')
        {
            if (Delete(FILE_BC)!=0)
                return (STAT_INV);
            if (Rename(FILE_BT,FILE_BC)!=0)
                return (STAT_INV);
        }
        */
        if (HDGRead(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
        memcpy(lsHDGBuf.BlackCardVersion,BlackListVersion,14);
        if (HDGWrite(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
    }

    //////////////////////////////////////////////////////////////////////////////
    ///Product Lists /////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (lpWrkBuf[2] == 'P')
    {
		DisplayString("4. LOAD PRODUCT", 1000);
        lwStatus = ProductList(lpWrkBuf, &lwPrevBlockNo);
        if (lwStatus != STAT_OK)
            return(lwStatus);
        if (Delete(FILE_PC)!=0)
            return (STAT_INV);
        if (Rename(FILE_PT,FILE_PC)!=0)
            return (STAT_INV);
        if (HDGRead(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
        memcpy(lsHDGBuf.ProductVersion,ProductVersion,LG_PARMVERSION);
        if (HDGWrite(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
    }

    //////////////////////////////////////////////////////////////////////////////
    ///Scheme Type ///////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (lpWrkBuf[2] == 'S')
    {
		//DisplayString("5. LOAD SCHEME", 1000);
        lwStatus = SchemeList(lpWrkBuf, &lwPrevBlockNo);
        if (lwStatus != STAT_OK)
            return(lwStatus);
        if (Delete(FILE_ST)!=0)
            return (STAT_INV);
        if (Rename(FILE_TS,FILE_ST)!=0)
            return (STAT_INV);
        if (HDGRead(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
        memcpy(lsHDGBuf.SchemeTypeVersion,SchemeTypeVersion,LG_PARMVERSION);
        if (HDGWrite(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
    }

    //////////////////////////////////////////////////////////////////////////////
    ///Repayment Tenures//////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (lpWrkBuf[2] == 'R')
    {
		DisplayString("6. LOAD TENURE", 1000);
        lwStatus = RepaymentTenuresList(lpWrkBuf, &lwPrevBlockNo);
        if (lwStatus != STAT_OK)
            return(lwStatus);
        
        if (Delete(FILE_PE)!=0)
            return (STAT_INV);
        if (Rename(FILE_TP,FILE_PE)!=0)
            return (STAT_INV);
        if (HDGRead(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
        memcpy(lsHDGBuf.RepaymentTenuresVersion,RepaymentTenuresVersion,LG_PARMVERSION);
        if (HDGWrite(&lsHDGBuf)!=STAT_OK)
            return(STAT_NOK);
    }

    //////////////////////////////////////////////////////////////////////////////
    ///Key List///////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (lpWrkBuf[2] == 'K')
    {
#ifdef DEVELOP_VERSION
		DisplayString("7. LOAD KEY", 1000);
#endif
        lwStatus = KeyList(lpWrkBuf, &lwPrevBlockNo);
        if (lwStatus != STAT_OK)
            return(lwStatus);
        if (Delete(csFILE_EMV_KEYS)!=0)
        {
            return (STAT_INV);
        }
        if (Rename(csFILE_EMV_TEMP_KEYS,csFILE_EMV_KEYS)!=0)
        {
            return (STAT_INV);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    //////Parameter Ok////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    DisplayString(DSP_PARAMETER_OK, 1000);
    return STAT_OK;
}

static WORD ParameterRequest(BYTE *fpWrkBuf,BYTE fAutoParam)
{
    WORD lbLen;
    HDGStruct lsHDGBuf;
    WORD lwStatus;
    TERMCONFIGSTRUCT term;
    APPLCONFIGSTRUCT ApplConfig;
    WORD  Count;
    
    GetTerminalConfig(&term);
    Count = StrLen(term.SerialNum);
    if(Count<LG_SERIALNUM)
    {
        memcpy(term.SerialNum,UTIL_FILLZEROS,LG_SERIALNUM);
    }
    
    lbLen=0;
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
    {
        return(STAT_NOK);
    }
    
    fpWrkBuf[0] = '=';
    lbLen++;
    
    StrCpy(fpWrkBuf+lbLen,TYPE_01);
    lbLen += 2;
    
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.MerchId, 12);
    lbLen = lbLen + 12;
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.StoreNum, LG_STORENUM);
    lbLen += LG_STORENUM;
    memcpy(fpWrkBuf+lbLen, lsHDGBuf.TermId,LG_TERMID);
    lbLen += LG_TERMID;
    
    memcpy(fpWrkBuf+lbLen, term.SerialNum,LG_SERIALNUM);
    lbLen += LG_SERIALNUM;
    
    //Terminal Type
    fpWrkBuf[lbLen++] = '0';
    //Application Version
    GetApplicationConfig(&ApplConfig);
    fpWrkBuf[lbLen++] = ApplConfig.ApplCompatibility[0];
    fpWrkBuf[lbLen++] = ApplConfig.ApplCompatibility[1];
    fpWrkBuf[lbLen++] = ApplConfig.ApplVersion[0];
    fpWrkBuf[lbLen++] = ApplConfig.ApplVersion[1];
    
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.MerchantTypeVersion,3);
    lbLen += 3;
    //Bin Version
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.BinVersion,3);
    lbLen += 3;
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.BlackCardVersion,14);
    lbLen += 14;
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.ProductVersion,3);
    lbLen += 3;
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.SchemeTypeVersion,3);
    lbLen += 3;
    memcpy(fpWrkBuf+lbLen,lsHDGBuf.RepaymentTenuresVersion,3);
    lbLen += 3;
    
    //Parameter Download Type
    if(fAutoParam==OS_TRUE)
        fpWrkBuf[lbLen++] = 'A';
    else
        fpWrkBuf[lbLen++] = 'M';
    fpWrkBuf[lbLen]=0;   // End of String
    
    lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *) NULL, NO_WAIT_MODE);
    if (lwStatus!=STAT_OK)
        return(STAT_NOK);
    
    if (fpWrkBuf[0]==EOT)
        return(STAT_EOT);
    
    return(STAT_OK);
}

static WORD GeneralParameter(BYTE *fpWrkBuf)
{
    BYTE lpDate[7];
    BYTE lpTime[20];
	BYTE CurrenyCode[10];
	BYTE CurrCode[]="0048";
    WORD lbLen;
    HDGStruct lsHDGBuf;
    HDDStruct lsHDDBuf;
    WORD lwStatus;
    WORD lwDummy;
    //WORD lwTemp;
    WORD lwMaxTrans;
    ALARM_STRUCT lsAlarm;
    
    lbLen = 0;
    
    if (fpWrkBuf[1] != 'I')
        return(STAT_INV);
    
    lbLen = 2;
    // Date and Time
    // DD
    memcpy(lpDate+4,fpWrkBuf+lbLen, 2);
    lbLen+= 2;
    // MM
    memcpy(lpDate+2,fpWrkBuf+lbLen, 2);
    lbLen+= 2;
    // YY
    memcpy(lpDate,fpWrkBuf+lbLen, 2);
    lbLen+= 2;
    // HH
    memcpy(lpTime,fpWrkBuf+lbLen, 2);
    lbLen+= 2;
    // MM
    memcpy(lpTime+2,fpWrkBuf+lbLen, 2);
    lbLen+= 2;
    // SS
    memcpy(lpTime+4, fpWrkBuf+lbLen, 2);
    lbLen+= 2;
    lpTime[6] = 0;
    lpDate[6] = 0;
    
    if (Check_TransFileEmpty(&lwDummy,FILE_TR))
    {
        ClkSetDate(lpDate);
        ClkSetTime(lpTime);
    }

	memset(&lsHDGBuf, 0, sizeof(HDGStruct));
    if (HDGRead(&lsHDGBuf)!=STAT_OK)
        return(STAT_NOK);
    
    if (HDDRead(&lsHDDBuf)!=STAT_OK)
        return(STAT_NOK);
    
    // Batch Number
    //	if (Check_TransFileEmpty(&lwTemp,FILE_TR) == STAT_NOK)
    //		memcpy(lsHDGBuf.BatchNum, fpWrkBuf+lbLen,3);
    lbLen+= 3;

    memcpy(lsHDGBuf.NETSERVERTel1,fpWrkBuf+lbLen, LG_PHONE);
    lbLen+= LG_PHONE;
    memcpy(lsHDGBuf.NETSERVERTel2,fpWrkBuf+lbLen, LG_PHONE);
    lbLen+= LG_PHONE;
    memcpy(lsHDGBuf.NETSERVERTel3, fpWrkBuf+lbLen,LG_PHONE);
    lbLen+= LG_PHONE;
    
    memcpy(lsHDGBuf.PMSTel1,fpWrkBuf+lbLen, LG_PHONE);
    lbLen+= LG_PHONE;
    memcpy(lsHDGBuf.PMSTel2,fpWrkBuf+lbLen, LG_PHONE);
    lbLen+= LG_PHONE;
    memcpy(lsHDGBuf.PMSTel3,fpWrkBuf+lbLen,LG_PHONE);
    lbLen+= LG_PHONE;
    
    memcpy(lsHDGBuf.TMSTel1,fpWrkBuf+lbLen, LG_PHONE);
    lbLen+= LG_PHONE;
    memcpy(lsHDGBuf.TMSTel2,fpWrkBuf+lbLen, LG_PHONE);
    lbLen+= LG_PHONE;
    memcpy(lsHDGBuf.TMSTel3,fpWrkBuf+lbLen,LG_PHONE);
    lbLen+= LG_PHONE;
    
    lsHDGBuf.OperationFlagsSmart1=fpWrkBuf[lbLen++];
    gOperationFlagsSmart1 = lsHDGBuf.OperationFlagsSmart1;
    
    lsHDGBuf.OperationFlagsSmart2 =fpWrkBuf[lbLen++];
    gOperationFlagsSmart2 = lsHDGBuf.OperationFlagsSmart2;
    
    ////////////////////////////////////////////////
    ///EMV Parameters//////////////////////////////
    ////////////////////////////////////////////////
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,1);
    lsHDGBuf.maxTargetPercentage=lpTime[0];
    lbLen+=2;
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,1);
    lsHDGBuf.targetPercentage=lpTime[0];
    lbLen+=2;
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,5);
    memcpy(lsHDGBuf.defaultTAC,lpTime,5);
    lbLen+=10;
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,5);
    memcpy(lsHDGBuf.denialTAC,lpTime,5);
    lbLen+=10;
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,5);
    memcpy(lsHDGBuf.onlineTAC,lpTime,5);
    lbLen+=10;
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,2);
    memcpy(lsHDGBuf.terminalCountryCode,lpTime,2);
    lbLen+= 4;
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,6);
    memcpy(lsHDGBuf.floorLimitDomestic,lpTime,6);
    memcpy(lsHDGBuf.floorLimitInternational,lpTime,6);
    lbLen+=12;
    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,6);
    memcpy(lsHDGBuf.thresholdValue,lpTime,6);
    lbLen+=12;

    AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)lpTime,2);
    memcpy(lsHDGBuf.trCurrencyCode,lpTime,2);
    lbLen+=4;
    
    lsHDGBuf.TransactionCurrencyExponent=fpWrkBuf[lbLen]-'0';
    lbLen++;
    gCurrencyExponent=lsHDGBuf.TransactionCurrencyExponent;
    
    memcpy(lsHDGBuf.trCurrencySymbol,fpWrkBuf+lbLen,3);
    lbLen+=3;
    memcpy(gCurrencySymbol,lsHDGBuf.trCurrencySymbol,3);
    
    
    fpWrkBuf[0]=ACK;
    fpWrkBuf[1]=0;
    lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *) NULL, WAIT_MODE);
    if (lwStatus!=STAT_OK)
        return(STAT_INV);
    
    if (fpWrkBuf[0]==EOT)
        return(STAT_INV);
    
    if (fpWrkBuf[1]!='O')
        return(STAT_INV);
    
    lbLen = 2;
    // Merchant Print Data
    memcpy(lsHDGBuf.MerchData, fpWrkBuf+lbLen,LG_MERCHDATA);
    lbLen += LG_MERCHDATA;
    // Max Transactions
    lwMaxTrans = (fpWrkBuf[lbLen++] - '0') * 100;
    lwMaxTrans += (fpWrkBuf[lbLen++] - '0') * 10;
    lwMaxTrans += fpWrkBuf[lbLen++] - '0';
    if(lwMaxTrans>999)
        lwMaxTrans=999;
    
    lsHDGBuf.MaxTrans = lwMaxTrans;
    gMaxTranaction=lsHDGBuf.MaxTrans;

    // Delete old Batch Close Alarm
    ClkReadDate(lpTime,F_YYMMDD);
    
    memcpy(lsAlarm.Date,lpTime,LG_TRANSDATE);
    memcpy(lsAlarm.Hour,lsHDDBuf.BCStartTime,LG_TRANSTIME);
    memcpy(lsAlarm.Hour+4,"00",2);
    lsAlarm.WakeUpMsg = ALARM_BATCHCLOSE;
    ManRequestMessage(TIME_MASK,TIME_DELETE_ALARM,sizeof(ALARM_STRUCT),&lsAlarm);
    // Delete old Parameter Download Alarm
    memcpy(lsAlarm.Hour,lsHDDBuf.ParmStartTime,LG_TRANSTIME);
    memcpy(lsAlarm.Hour+4,"00",2);
    lsAlarm.WakeUpMsg = ALARM_PARAM_DOWNLOAD;
    ManRequestMessage(TIME_MASK,TIME_DELETE_ALARM,sizeof(ALARM_STRUCT),&lsAlarm);
    
    // Batch Close Start Time
    memcpy(lsHDDBuf.BCStartTime, fpWrkBuf+lbLen, LG_TRANSTIME);
    lbLen += LG_TRANSTIME;
    if(memcmp(lsHDDBuf.BCStartTime, UTIL_SPACES, LG_TRANSTIME)!=0)
    {
        ClkReadDate(lpTime,F_YYMMDD);
        memcpy(lsAlarm.Date,lpTime,LG_TRANSDATE);
        memcpy(lsAlarm.Hour,lsHDDBuf.BCStartTime,LG_TRANSTIME);
        memcpy(lsAlarm.Hour+4,"00",2);
        ClkReadTime(lpTime);
        // If Batch close time is earlier than current time,
        // increase date to start Batch Close tomorrow.
        if (AsciiCmp(lsAlarm.Hour, LG_TIME, lpTime, LG_TIME) == -1)
            PIncreaseDate(lsAlarm.Date, 1);
        lsAlarm.WakeUpMsg = ALARM_BATCHCLOSE;
        ManRequestMessage(TIME_MASK,TIME_SET_ALARM,sizeof(ALARM_STRUCT),&lsAlarm);
    }
    memcpy(lsHDDBuf.ParmStartTime, fpWrkBuf+lbLen, LG_TRANSTIME);
    lbLen += LG_TRANSTIME;
    
    if(memcmp(lsHDDBuf.ParmStartTime, UTIL_SPACES, LG_TRANSTIME)!=0)
    {
        // Set new Alarms for Parameter Download
        ClkReadDate(lpTime,F_YYMMDD);
        memcpy(lsAlarm.Date,lpTime,LG_TRANSDATE);
        memcpy(lsAlarm.Hour,lsHDDBuf.ParmStartTime,LG_TRANSTIME);
        memcpy(lsAlarm.Hour+4,"00",2);
        ClkReadTime(lpTime);
        // If Batch close time is earlier than current time,
        // increase date to start Batch Close tomorow.
        if (AsciiCmp(lsAlarm.Hour, LG_TIME, lpTime, LG_TIME) == -1)
            PIncreaseDate(lsAlarm.Date, 1);
        
        lsAlarm.WakeUpMsg = ALARM_PARAM_DOWNLOAD;
        ManRequestMessage(TIME_MASK,TIME_SET_ALARM,sizeof(ALARM_STRUCT),&lsAlarm);
    }

    // Number of Attempts
    lsHDDBuf.BCAttempts = fpWrkBuf[lbLen] - '0';
    lsHDDBuf.ParmAttempts = fpWrkBuf[lbLen] - '0';
    lsHDDBuf.MaxAttempts = fpWrkBuf[lbLen++] - '0';
    
    // Interval Between Calls
    lwMaxTrans = (fpWrkBuf[lbLen++] - '0') * 100;
    lwMaxTrans += (fpWrkBuf[lbLen++] - '0') * 10;
    lwMaxTrans += fpWrkBuf[lbLen++] - '0';
    lsHDDBuf.AttemptInterval = lwMaxTrans;
    
    
    if (HDGWrite(&lsHDGBuf)!=STAT_OK)
        return(STAT_NOK);
    
    if (HDDWrite(&lsHDDBuf)!=STAT_OK)
        return(STAT_NOK);
    
    return(STAT_OK);
}

static WORD MerchantTypeList(BYTE *fpWrkBuf, WORD *fpBlockNo)
{
    WORD lbLen;
    WORD lwStatus;
    BYTE lbCounter;
    BYTE UpdateBuf[25];
    int  lbFileHandler;
    BYTE AttemptCount[3];
    BYTE Count=0;
    BYTE Count1=0;
    
    OS_ClearScreen();
    AttemptCount[0] = '0';
    
    if (FileInit(FILE_TEMPMERCH)==STAT_NOK)
        return(STAT_NOK);
    
    *fpBlockNo=fpWrkBuf[1]-'0';
    
    while (fpWrkBuf[2] == 'M')
    {
        DisplayString(DSP_MERCHANT_TYPES_LOADING,0);
        if(Count == 9)
        {
            AttemptCount[0] = Count1 + '1';
            Count1++;
            Count=0;
            AttemptCount[1] = Count + '0';
        }
        else
        {
            AttemptCount[1] = Count + '1';
            Count++;
        }
        AttemptCount[2] = 0;
        OS_DisplayStr(3, 8, AttemptCount);
        
        // Open Temp Card Type File
        lbFileHandler = Open(FILE_TEMPMERCH, O_RDWR);
        if (lbFileHandler < 0)
            return(STAT_NOK);

        seek(lbFileHandler, 0, SEEK_END);
        
        lbLen = 3;
        while (fpWrkBuf[lbLen])
        {
            HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 2);
            UpdateBuf[2]=0;
            lbLen+=2;
            UpdateBuf[2]=fpWrkBuf[lbLen++];

            if (Write(lbFileHandler,UpdateBuf, 3)!=0)    // Write record in Temp file
                return(STAT_NOK);
        }
        Close(lbFileHandler);
        lbCounter = 0;
        do
        {
            fpWrkBuf[0]=ACK;
            fpWrkBuf[1]=0;
            lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *)NULL, NO_WAIT_MODE);
            if (lwStatus!=STAT_OK)
                return(STAT_INV);
            if (fpWrkBuf[0]==EOT)
                return(STAT_OK);
            if (fpWrkBuf[0]==ACK)
                return(STAT_OK);
            
            if (((fpWrkBuf[1]-'0')!=(((*fpBlockNo)+1)%10)) &&
                ((fpWrkBuf[1]-'0')!=*fpBlockNo))
                return(STAT_INV);
        } while (((fpWrkBuf[1]-'0')==*fpBlockNo) && (lbCounter++<MAX_COMM_RETRY));
        
        if (lbCounter == MAX_COMM_RETRY)
            return(STAT_INV);
        
        *fpBlockNo=fpWrkBuf[1]-'0';
    }
    return(STAT_OK);
}


static WORD BinList(BYTE *fpWrkBuf, WORD *fpBlockNo)
{
    WORD lbLen;
    WORD lwStatus;
    BYTE lbCounter;
    BYTE UpdateBuf[25];
    int  lbFileHandler;
    BYTE AttemptCount[3];
    BYTE Count=0;
    BYTE Count1=0;
    BinStruct lsBinBuf;
    
    OS_ClearScreen();
    AttemptCount[0] = '0';
    
    if (FileInit(FILE_TEMPBIN)==STAT_NOK)
        return(STAT_NOK);
    
    *fpBlockNo=fpWrkBuf[1]-'0';
    
    while (fpWrkBuf[2] == 'B')
    {
        DisplayString (DSP_BIN_LOADING,0);
        if(Count == 9)
        {
            AttemptCount[0] = Count1 + '1';
            Count1++;
            Count=0;
            AttemptCount[1] = Count + '0';
        }
        else
        {
            AttemptCount[1] = Count + '1';
            Count++;
        }
        AttemptCount[2] = 0;
        OS_DisplayStr(3, 8, AttemptCount);
        
        // Open Temp Card Type File
        lbFileHandler=Open(FILE_TEMPBIN, O_RDWR);
        if (!(lbFileHandler>0))
            return(STAT_NOK);

        seek(lbFileHandler, 0, SEEK_END);
        
        lbLen = 3;
        while (fpWrkBuf[lbLen])
        {
            HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 3);
            UpdateBuf[3]=0;
            StrBcdToAsc(6,UpdateBuf,lsBinBuf.Bin);
            lbLen+=3;
            lsBinBuf.CardBinFlags = fpWrkBuf[lbLen++];

            if (Write(lbFileHandler,(BYTE *)&lsBinBuf, LG_BINSTRUCT)!=0)    // Write record in Temp file
                return(STAT_NOK);
        }
        Close(lbFileHandler);
        lbCounter = 0;
        do
        {
            fpWrkBuf[0]=ACK;
            fpWrkBuf[1]=0;
            lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *)NULL, NO_WAIT_MODE);
            if (lwStatus!=STAT_OK)
                return(STAT_INV);
            if (fpWrkBuf[0]==EOT)
                return(STAT_OK);
            if (fpWrkBuf[0]==ACK)
                return(STAT_OK);
            
            if (((fpWrkBuf[1]-'0')!=(((*fpBlockNo)+1)%10)) &&
                ((fpWrkBuf[1]-'0')!=*fpBlockNo))
                return(STAT_INV);
        } while (((fpWrkBuf[1]-'0')==*fpBlockNo) && (lbCounter++<MAX_COMM_RETRY));
        
        if (lbCounter == MAX_COMM_RETRY)
            return(STAT_INV);
        
        *fpBlockNo=fpWrkBuf[1]-'0';
    }
    return(STAT_OK);
}

static WORD BlackCardList(BYTE *fpWrkBuf, WORD *fpBlockNo,BYTE BlackList_DownType)
{
    // Gary : delete blacklist processing
    /*
    WORD lbLen;
    WORD lwStatus;
    BYTE lbCounter;
    BYTE UpdateBuf[25];
    int  lbFileHandler = 0;
    BYTE AttemptCount[3];
    BYTE Count=0;
    BYTE Count1=0;
    BYTE OperationType = 0;
    
    OS_ClearScreen();
    AttemptCount[0] = '0';
    if (FileInit(FILE_BT)==STAT_NOK)
        return(STAT_NOK);
    *fpBlockNo=fpWrkBuf[1]-'0';
    
    while (fpWrkBuf[2] == 'Y')
    {
        DisplayString (DSP_BLACK_CARD_LOADING,0);
        if(Count == 9)
        {
            AttemptCount[0] = Count1 + '1';
            Count1++;
            Count=0;
            AttemptCount[1] = Count + '0';
        }
        else
        {
            AttemptCount[1] = Count + '1';
            Count++;
        }
        AttemptCount[2] = 0;
        OS_DisplayStr(3, 8, AttemptCount);
        
        lbLen = 3;
        OperationType = fpWrkBuf[lbLen++];
        if(BlackList_DownType=='F')
        {
            // Open Temp Card Type File
            lbFileHandler=Open(FILE_BT, O_RDWR);
            if (lbFileHandler < 0)
                return(STAT_NOK);
            seek(lbFileHandler, 0, SEEK_END);

            while ((fpWrkBuf[lbLen])&&(lbLen<248))
            {
                HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 8);
                UpdateBuf[8]=0;
                lbLen += 8;
            
                if (Write(lbFileHandler,UpdateBuf , 8)!=0)    // Write record in Temp file
                {
                    Close(lbFileHandler);
                    return(STAT_NOK);
                }
            }
            Close(lbFileHandler);
        }
        else if(BlackList_DownType=='U')
        {
            while ((fpWrkBuf[lbLen])&&(lbLen<248))
            {
                HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 8);
                UpdateBuf[8]=0;
                lbLen += 8;
                if (OperationType == 'A')
                {
                    lwStatus=AddBlackList(UpdateBuf,8);
                    if (lwStatus!=STAT_OK)
                        return(STAT_INV);
                }
                else if (OperationType == 'D')
                {
                    lwStatus=DeleteBlackList(UpdateBuf,8);
                    if (lwStatus!=STAT_OK)
                        return(STAT_INV);
                }
            }
        }
        lbCounter = 0;
        do
        {
            fpWrkBuf[0]=ACK;
            fpWrkBuf[1]=0;
            lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *)NULL, NO_WAIT_MODE);
            if (lwStatus!=STAT_OK)
            {
                return(STAT_INV);
            }
            if (fpWrkBuf[0]==EOT)
            {
                return(STAT_OK);
            }
            if (fpWrkBuf[0]==ACK)
            {
                return(STAT_OK);
            }
            if (((fpWrkBuf[1]-'0')!=(((*fpBlockNo)+1)%10)) &&
                ((fpWrkBuf[1]-'0')!=*fpBlockNo))
                return(STAT_INV);
            
        } while (((fpWrkBuf[1]-'0')==*fpBlockNo) &&
            (lbCounter++<MAX_COMM_RETRY));
        
        if (lbCounter == MAX_COMM_RETRY)
            return(STAT_INV);
        
        *fpBlockNo=fpWrkBuf[1]-'0';
    }
    */

    return(STAT_OK);
}

static WORD ProductList(BYTE *fpWrkBuf, WORD *fpBlockNo)
{
    WORD lbLen;
    WORD lwStatus;
    BYTE lbCounter;
    BYTE UpdateBuf[28];
    int  lbFileHandler=0;
    BYTE AttemptCount[3];
    BYTE Count=0;
    BYTE Count1=0;
    
    
    OS_ClearScreen();
    AttemptCount[0] = '0';
    
    if (FileInit(FILE_PT)==STAT_NOK)
        return(STAT_NOK);
    
    *fpBlockNo=fpWrkBuf[1]-'0';
    
    while (fpWrkBuf[2] == 'P')
    {
        DisplayString (DSP_PRODUCT_LIST_LOADING,0);
        if(Count == 9)
        {
            AttemptCount[0] = Count1 + '1';
            Count1++;
            Count=0;
            AttemptCount[1] = Count + '0';
        }
        else
        {
            AttemptCount[1] = Count + '1';
            Count++;
        }
        AttemptCount[2] = 0;
        OS_DisplayStr(3, 8, AttemptCount);
        
        // Open Temp Card Type File
        lbFileHandler=Open(FILE_PT, O_RDWR);
        if (lbFileHandler < 0)
            return(STAT_NOK);

        seek(lbFileHandler, 0, SEEK_END);
        
        lbLen = 3;
        while (fpWrkBuf[lbLen])
        {
            HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 2);
            memcpy(UpdateBuf+2, fpWrkBuf+lbLen+2, 24);
            UpdateBuf[26]=0;
            lbLen += 26;
        
            if (Write(lbFileHandler,UpdateBuf , 26)!=0)    // Write record in Temp file
                return(STAT_NOK);
        }
        Close(lbFileHandler);
        lbCounter = 0;
        do
        {
            fpWrkBuf[0]=ACK;
            fpWrkBuf[1]=0;
            lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *)NULL, NO_WAIT_MODE);
            
            if (lwStatus!=STAT_OK)
            {
                return(STAT_INV);
            }
            if (fpWrkBuf[0]==EOT)
            {
                return(STAT_OK);
            }
            if (fpWrkBuf[0]==ACK)
            {
                return(STAT_OK);
            }
            if (((fpWrkBuf[1]-'0')!=(((*fpBlockNo)+1)%10)) &&
                ((fpWrkBuf[1]-'0')!=*fpBlockNo))
            {
                return(STAT_INV);
            }
            
        } while (((fpWrkBuf[1]-'0')==*fpBlockNo) &&
            (lbCounter++<MAX_COMM_RETRY));
        
        if (lbCounter == MAX_COMM_RETRY)
            return(STAT_INV);
        
        *fpBlockNo=fpWrkBuf[1]-'0';
    }
    return(STAT_OK);
}

static WORD SchemeList(BYTE *fpWrkBuf, WORD *fpBlockNo)
{
    WORD lbLen;
    WORD lwStatus;
    BYTE lbCounter;
    BYTE UpdateBuf[128];
    int  lbFileHandler=0;
    BYTE AttemptCount[3];
    BYTE Count=0;
    BYTE Count1=0;
    BYTE BUFF[128];
    
	//by jarod print SchemeList For test
#ifdef OUTPUT_ISO8583
	BYTE lpTime[13];
	BYTE lpLine[27];
	char sPrnBuffer1[LG_PRN_BUFFER];

	PrintBuffer_Init(sPrnBuffer1, sizeof(sPrnBuffer1));
	PrintBuffer_SelectFont(sPrnBuffer1, 24);
	PrnHead(sPrnBuffer1);

	PrintBuffer_Eng(sPrnBuffer1, "     PARAMETER DOWNLOAD TEST  \n\n");

	memcpy(BUFF,fpWrkBuf,sizeof(BUFF));
	PrintBuffer_Eng(sPrnBuffer1,"%s",BUFF);
	PrintBuffer_Eng(sPrnBuffer1, "\n\n\n\n");

	// DateTime
	memset(lpLine, 0, sizeof(lpLine));
	ConvDate(lpTime); //DD-MM-YYYY
	memcpy(lpLine,lpTime,10);
	memset(lpLine+10,' ',9);
	ClkReadTime(lpTime);
	memcpy(lpLine+19,lpTime,2);
	lpLine[21]=':';
	memcpy(lpLine+22,lpTime+2,2);
	PrintBuffer_Eng(sPrnBuffer1, "%s\n",(PCHAR)lpLine);
	PrintBuffer_Eng(sPrnBuffer1, "                \n");
	PrintBuffer_Eng(sPrnBuffer1, "                \n");
	PrintBuffer_Eng(sPrnBuffer1, "                \n");
	PrintBuffer_Eng(sPrnBuffer1, "                \n");

	PrnInit();
	PrintBuffer_SendToPrinter(sPrnBuffer1);
	StartPrinter();
#endif

    OS_ClearScreen();
    AttemptCount[0] = '0';
    
    if (FileInit(FILE_TS)==STAT_NOK)
        return(STAT_NOK);
    
    *fpBlockNo=fpWrkBuf[1]-'0';
    
    while (fpWrkBuf[2] == 'S')
    {
        DisplayString (DSP_SCHEME_LIST_LOADING,0);
        if(Count == 9)
        {
            AttemptCount[0] = Count1 + '1';
            Count1++;
            Count=0;
            AttemptCount[1] = Count + '0';
        }
        else
        {
            AttemptCount[1] = Count + '1';
            Count++;
        }
        AttemptCount[2] = 0;
        OS_DisplayStr(3, 8, AttemptCount);
        
        // Open Temp Card Type File
        lbFileHandler=Open(FILE_TS, O_RDWR);
        if (lbFileHandler < 0)
            return(STAT_NOK);

        seek(lbFileHandler, 0, SEEK_END);
        
        lbLen = 3;

		while (fpWrkBuf[lbLen])
        {
//by jarod 2011.06.17
//The data format as follow:
// 30 1C SchemeProfit
// 1C 1C 1C 1C 1C StartAmount
// 1C 1C 26 1C 1C FinishAmount
// 32 30 25 20 20 20 20 20 20 20 20 20 SchemeDescription
// 54 65 73 74 20 32 30 25 20 20 20 20 SchemeDescriptionArabic 
#if 1
			HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 2);
			lbLen+=2;
			HCCPBinToBCD(fpWrkBuf+lbLen,UpdateBuf+2,5);
			lbLen+=5;
			HCCPBinToBCD(fpWrkBuf+lbLen,UpdateBuf+7,5);
			lbLen+=5;
            memcpy(UpdateBuf+12, fpWrkBuf+lbLen, 24);
			UpdateBuf[36]=0;
			lbLen+=24;
			if (Write(lbFileHandler,UpdateBuf , 36)!=0)     // Write record in Temp file
			{
				Close(lbFileHandler);
				return(STAT_NOK);
			}        
#else
			HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 2);

            memcpy(UpdateBuf+2, fpWrkBuf+lbLen+2, 24);
            UpdateBuf[26]=0;
            lbLen += 26;
            
            if (Write(lbFileHandler,UpdateBuf , 26)!=0)     // Write record in Temp file
            {
                Close(lbFileHandler);
                return(STAT_NOK);
            }        
#endif
        }
        Close(lbFileHandler);
        lbCounter = 0;
        do
        {
            fpWrkBuf[0]=ACK;
            fpWrkBuf[1]=0;
            lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *)NULL, NO_WAIT_MODE);
            
            if (lwStatus!=STAT_OK)
            {
                return(STAT_INV);
            }
            if (fpWrkBuf[0]==EOT)
            {
                return(STAT_OK);
            }
            if (fpWrkBuf[0]==ACK)
            {
                return(STAT_OK);
            }
            if (((fpWrkBuf[1]-'0')!=(((*fpBlockNo)+1)%10)) &&
                ((fpWrkBuf[1]-'0')!=*fpBlockNo))
            {
                return(STAT_INV);
            }
            
        } while (((fpWrkBuf[1]-'0')==*fpBlockNo) && (lbCounter++<MAX_COMM_RETRY));
        
        if (lbCounter == MAX_COMM_RETRY)
            return(STAT_INV);
        
        *fpBlockNo=fpWrkBuf[1]-'0';
    }

    return(STAT_OK);
}

static WORD RepaymentTenuresList(BYTE *fpWrkBuf, WORD *fpBlockNo)
{
    WORD lbLen;
    WORD lwStatus;
    BYTE lbCounter;
    BYTE UpdateBuf[25];
    int  lbFileHandler;
    BYTE AttemptCount[3];
    BYTE Count=0;
    BYTE Count1=0;
    
    
    OS_ClearScreen();
    AttemptCount[0] = '0';
    
    if (FileInit(FILE_TP)==STAT_NOK)
        return(STAT_NOK);
    
    *fpBlockNo=fpWrkBuf[1]-'0';
    
    while (fpWrkBuf[2] == 'R')
    {
        DisplayString(DSP_AVAIL_REPAYMENT_TENURES_LOADING,0);
        if(Count == 9)
        {
            AttemptCount[0] = Count1 + '1';
            Count1++;
            Count=0;
            AttemptCount[1] = Count + '0';
        }
        else
        {
            AttemptCount[1] = Count + '1';
            Count++;
        }
        AttemptCount[2] = 0;
        OS_DisplayStr(3, 8, AttemptCount);
        
        // Open Temp Card Type File
        lbFileHandler=Open(FILE_TP, O_RDWR|O_CREATE);
        if (lbFileHandler < 0)
		{
            return(STAT_NOK);
		}

        seek(lbFileHandler, 0, SEEK_END);
        
        lbLen = 3;
        while (fpWrkBuf[lbLen])
        {
            HCCPBinToBCD(fpWrkBuf+lbLen, UpdateBuf, 10);
            memcpy(UpdateBuf+10, fpWrkBuf+lbLen+10, 2);
            UpdateBuf[12]=0;
            lbLen += 12;
        
            if (Write(lbFileHandler,UpdateBuf , 12)!=0)    // Write record in Temp file
            {
                Close(lbFileHandler);
                return(STAT_NOK);
            }
        }
        Close(lbFileHandler);
        lbCounter = 0;
        do
        {
            fpWrkBuf[0]=ACK;
            fpWrkBuf[1]=0;
            lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *)NULL, NO_WAIT_MODE);
            
            if (lwStatus!=STAT_OK)
            {
                return(STAT_INV);
            }
            if (fpWrkBuf[0]==EOT)
            {
                return(STAT_OK);
            }
            if (fpWrkBuf[0]==ACK)
            {
                return(STAT_OK);
            }
            if (((fpWrkBuf[1]-'0')!=(((*fpBlockNo)+1)%10)) &&
                ((fpWrkBuf[1]-'0')!=*fpBlockNo))
            {
                return(STAT_INV);
            }
            
        } while (((fpWrkBuf[1]-'0')==*fpBlockNo) && (lbCounter++<MAX_COMM_RETRY));
        
        if (lbCounter == MAX_COMM_RETRY)
            return(STAT_INV);
        
        *fpBlockNo=fpWrkBuf[1]-'0';
    }
    return(STAT_OK);
}

static void HCCPBinToBCD(BYTE *fpStrBin, BYTE *fpStrAscii, BYTE fbLen)
{
    BYTE lbNumber;
    BYTE liCounter;
    BYTE Hex=0;
    
    for (liCounter =0; liCounter<fbLen; liCounter++)
    {
        lbNumber = fpStrBin[liCounter] - 0x1C;
        Hex=0;
        Hex=((lbNumber/10)*0x10)+(lbNumber%10);
        fpStrAscii[liCounter]=Hex;
    }
}

static WORD AddBlackList(BYTE *fpWrkBuf, BYTE RecLength)
{
    // Gary : delete blacklist processing
    return STAT_OK;
}

WORD DeleteBlackList(BYTE *fpWrkBuf,BYTE RecLength)
{
    // Gary : delete blacklist processing
    return STAT_OK;
}

#ifdef NEED_DEBUG
static void Debug__PrintCAPK(EMV_CAPK *pCAPK)
{
    int ii;

    PrnInit();
    PrnStr("\n\n");
    PrnStr("RID:   %02X%02X%02X%02X%02X\n", pCAPK->RID[0], pCAPK->RID[1], pCAPK->RID[2], pCAPK->RID[3], pCAPK->RID[4]);
    PrnStr("KEYID: %02X\n", pCAPK->KeyID);
    PrnStr("MODLEN:%d\n", (int)(pCAPK->ModulLen));
    PrnStr("MOD:\n");
    for (ii=0; ii<pCAPK->ModulLen; ii++)
    {
        PrnStr("%02X ", pCAPK->Modul[ii]);
        if ((ii%10)==9)
        {
            PrnStr("\n");
        }
    }
    PrnStr("\n");
    PrnStr("EXPLEN:%02X\nEXP:   %02X%02X%02X\n", pCAPK->ExponentLen, pCAPK->Exponent[0], pCAPK->Exponent[1], pCAPK->Exponent[2]);
    PrnStr("\n\n\n\n");
    PrnStart();
}
#endif


static WORD KeyList(BYTE *fpWrkBuf, WORD *fpBlockNo)
{    
    WORD lbLen;
    WORD lwStatus;
    BYTE lbCounter;
    BYTE UpdateBuf[500];
    int  lbFileHandler=0;
    BYTE AttemptCount[3];
    BYTE Count=0;
    BYTE Count1=0;
    
    KeyInfoStruct lsKeyInfoBuf;
    byte LengthOfCAPKExponent,i;
    int fp;
    char tmp_str[40];
    char tmpbuf[4096];

    EMV_CAPK    stPaxCAPK;
    int         iRet;

    OS_ClearScreen();
    AttemptCount[0] = '0';
    
    if (FileInit(csFILE_EMV_TEMP_KEYS)==STAT_NOK)
        return(STAT_NOK);
    
    *fpBlockNo=fpWrkBuf[1]-'0';
    
    while (fpWrkBuf[2] == 'K')
    {                  
        DisplayString ((PBYTE)"KEY LIST LOADING",0);
        if(Count == 9)
        {
            AttemptCount[0] = Count1 + '1';
            Count1++;
            Count=0;
            AttemptCount[1] = Count + '0';
        }
        else
        {
            AttemptCount[1] = Count + '1';
            Count++;
        }
        AttemptCount[2] = 0;
        OS_DisplayStr(3, 8, AttemptCount);
        
        // Open Temp Card Type File
        lbFileHandler = Open(csFILE_EMV_TEMP_KEYS, O_RDWR|O_CREATE);
        if ( lbFileHandler<0 )
        {
            return(STAT_NOK);
        }
        seek(lbFileHandler, 0, SEEK_END);
        
        lbLen = 3;
        
        AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)UpdateBuf,1);
        lsKeyInfoBuf.CAPKIndex=UpdateBuf[0];
        lbLen=lbLen+2;
        
        AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)UpdateBuf,3);
        memcpy(lsKeyInfoBuf.exponent,UpdateBuf,3);
        lbLen=lbLen+6;
        
        lsKeyInfoBuf.keyLength = (fpWrkBuf[lbLen++] - '0') * 100;
        lsKeyInfoBuf.keyLength += (fpWrkBuf[lbLen++] - '0') * 10;
        lsKeyInfoBuf.keyLength += fpWrkBuf[lbLen++] - '0';
        lsKeyInfoBuf.keyLength = lsKeyInfoBuf.keyLength/2;
        
        AscToHex((PCHAR)fpWrkBuf+lbLen,(PCHAR)UpdateBuf,lsKeyInfoBuf.keyLength);        
        memcpy(lsKeyInfoBuf.keyLocationPointer,UpdateBuf,lsKeyInfoBuf.keyLength);
        lbLen=lbLen+lsKeyInfoBuf.keyLength*2;

        // EMV lib CAPK writing
        memset(&stPaxCAPK, 0, sizeof(stPaxCAPK));
        memcpy(stPaxCAPK.RID, APPL_AID, 5);
        stPaxCAPK.KeyID = lsKeyInfoBuf.CAPKIndex;
        stPaxCAPK.HashInd = 0x01;
        stPaxCAPK.ArithInd = 0x01;
        stPaxCAPK.ModulLen = lsKeyInfoBuf.keyLength;
        /*
        for (iRet=0; iRet<stPaxCAPK.ModulLen; iRet++)
        {
            stPaxCAPK.Modul[stPaxCAPK.ModulLen+248-1-iRet] = lsKeyInfoBuf.keyLocationPointer[iRet];
        }
        for (iRet=0; iRet<stPaxCAPK.ModulLen; iRet++)
        {
            stPaxCAPK.Modul[stPaxCAPK.ModulLen+stPaxCAPK.ModulLen-1-iRet] = lsKeyInfoBuf.keyLocationPointer[iRet];
        }
        */
        memcpy(stPaxCAPK.Modul, lsKeyInfoBuf.keyLocationPointer, stPaxCAPK.ModulLen);
        stPaxCAPK.Exponent[0] = lsKeyInfoBuf.exponent[2];
        stPaxCAPK.Exponent[1] = lsKeyInfoBuf.exponent[1];
        stPaxCAPK.Exponent[2] = lsKeyInfoBuf.exponent[0];
        stPaxCAPK.ExponentLen = ((memcmp(stPaxCAPK.Exponent+1, "\x00\x00", 2)==0) ? 1 : 3);
        memcpy(stPaxCAPK.ExpDate, "\x49\x12\x31", 3);


		{
            // TMS do not send checksum to POS.
			BYTE	buff[2048];
			int		len;
			
			memset(buff, 0, sizeof(buff));
			memcpy(buff, stPaxCAPK.RID, 5);
			buff[5] = stPaxCAPK.KeyID;
			len = 6;
			memcpy(buff + len, stPaxCAPK.Modul, stPaxCAPK.ModulLen);
			len += stPaxCAPK.ModulLen;
			memcpy(buff + len, stPaxCAPK.Exponent, stPaxCAPK.ExponentLen);
			len += stPaxCAPK.ExponentLen;
			Hash(buff, len, stPaxCAPK.CheckSum);
		}

#ifdef NEED_DEBUG
        if (stPaxCAPK.KeyID==0xAA)
        {
            Debug__PrintCAPK(&stPaxCAPK);
        }
#endif

        iRet = EMVAddCAPK(&stPaxCAPK);
        if (iRet!=0)
        {
            OS_ClearScreen();
            ScrPrint(0, 4, ASCII, "PK SAVE ERROR : %d", iRet);
            getkey();
        }
        
        if (Write(lbFileHandler,(BYTE *)&lsKeyInfoBuf, LG_EMVKEYSSTRUCT)!=0)
        {
            Close(lbFileHandler);
            return(STAT_NOK);
        }
        /* Vx Key File Start */		
        memset(tmp_str, 0, sizeof(tmp_str));
        memcpy(tmpbuf,APPL_AID,5);
        HexToAsc(tmpbuf, tmp_str, 5);
        tmp_str[10]='.';
        HexToAsc(&lsKeyInfoBuf.CAPKIndex, tmp_str+11, 1);
        remove(tmp_str);
        if ((fp = open(tmp_str, O_CREATE| O_RDWR))<0)
		{
			close(fp);
			return(STAT_NOK);
		}
        
        sprintf(tmp_str,"%03d",(unsigned int)lsKeyInfoBuf.keyLength);
        write(fp,tmp_str,strlen((char*)tmp_str));	// Len 
        
        HexToAsc(lsKeyInfoBuf.keyLocationPointer, tmpbuf, lsKeyInfoBuf.keyLength);
        write(fp,(char*)tmpbuf,lsKeyInfoBuf.keyLength*2); // Key Value 
        
        LengthOfCAPKExponent=3;
        for (i=0;i<3;i++)
        {
            if (lsKeyInfoBuf.exponent[i]!=0) 
                break;
            LengthOfCAPKExponent--;
        }	
        sprintf(tmp_str,"%02d",(unsigned int)LengthOfCAPKExponent);
        write(fp,tmp_str,strlen(tmp_str));
        
        HexToAsc(lsKeyInfoBuf.exponent+(3-LengthOfCAPKExponent), tmpbuf, LengthOfCAPKExponent);
        write(fp,(char*)tmpbuf,LengthOfCAPKExponent*2); // Exponent
        
        close(fp);
        /* Mx Key File End */
        
        
        lbCounter = 0;
        do
        {
            fpWrkBuf[0]=ACK;
            fpWrkBuf[1]=0;
            lwStatus = Send_ReceiveBuffer(fpWrkBuf, (WORD *)NULL, NO_WAIT_MODE);
            if (lwStatus!=STAT_OK)
            {
                Close(lbFileHandler);
                return(STAT_INV);
            }
            if (fpWrkBuf[0]==EOT)
            {
                Close(lbFileHandler);
                return(STAT_OK);
            }
            if (fpWrkBuf[0]==ACK)
            {
                Close(lbFileHandler);
                return(STAT_OK);
            }
            
            if (((fpWrkBuf[1]-'0')!=(((*fpBlockNo)+1)%10)) &&
                ((fpWrkBuf[1]-'0')!=*fpBlockNo))
            {
                Close(lbFileHandler);
                return(STAT_INV);
            }
        } while (((fpWrkBuf[1]-'0')==*fpBlockNo) && (lbCounter++<MAX_COMM_RETRY));
        
        if (lbCounter == MAX_COMM_RETRY)
        {
            Close(lbFileHandler);
            return(STAT_INV);
        }
        
        *fpBlockNo=fpWrkBuf[1]-'0';
    }
    
    Close(lbFileHandler);
    return(STAT_OK);
}


