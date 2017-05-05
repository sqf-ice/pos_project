#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.


#include "os_define.h"
#include "os_interface.h"
#include "defines.h"
#include "struct.h"
#include "utility.h"
#include "function.h"
#include "message.h"



extern BYTE Enter_data(BYTE *chTemp, BYTE Type);

WORD HostInfoEntry(void)
{
    HDGStruct lrHDGStruct;    // Merchant Parameter Structure
    BYTE lpMerchId[LG_MERCHID+1]={0};    // Merchant ID
    BYTE lpStoreNum[LG_STORENUM+1]={0};  //
    BYTE lpTermId[LG_TERMID+1]={0};      // Terminal ID
    BYTE lpHostTel[LG_PHONE+1]={0};        // Host Phone
    BYTE lpAPN[LG_APN+1]={0};        // APN
    BYTE lpIP[LG_IP+1]={0};        // IP    
    BYTE lpUser[32] = {0};
    BYTE lpPwd[32] = {0};
	BYTE lpPort[4 + 1] = {0}; // Modified by Kim_LinHB 2013-3-5 build 0.05.0009 from 4 to 5 cuz using atoi below
    BYTE lpTmsNii[5] = {0};
    BYTE lpPmsNii[5] = {0};
    BYTE lpNetServerNii[5] = {0};
    
    WORD lwDummy;
    BYTE fbLength=0;
    
    HDGStruct g;
    GPRSStruct GPRSInp;


//by jarod
//Abd required to remove this 
//Abd said:why do we need to do end of day to access the host info
//we dont require end of day for that
#if 0
	// if transaction file is not empty display error message
    if (Check_TransFileEmpty(&lwDummy,FILE_TR)!=STAT_NOK)
    {
        DisplayString_S80(DSP_BATCH_OPEN, 3000);
        return(STAT_NOK);
    }
#endif

    if (HDGRead(&lrHDGStruct) != STAT_OK)
        return(STAT_NOK);
    
    //******************* get merchant params *******************
    if (memcmp(lrHDGStruct.MerchId, UTIL_STARS, LG_MERCHID)==0)
        memset(lpMerchId, 0, LG_MERCHID);
    else
        memcpy(lpMerchId, lrHDGStruct.MerchId, LG_MERCHID);
    
    if (memcmp(lrHDGStruct.StoreNum, UTIL_STARS, LG_STORENUM)==0)
        memset(lpStoreNum, 0, LG_STORENUM);
    else
        memcpy(lpStoreNum, lrHDGStruct.StoreNum, LG_STORENUM);
    
    
    if (memcmp(lrHDGStruct.TermId, UTIL_STARS, LG_TERMID)==0)
        memset(lpTermId, 0, LG_TERMID);
    else
        memcpy(lpTermId, lrHDGStruct.TermId, LG_TERMID);
    
    if (memcmp(lrHDGStruct.TMSTel1, UTIL_STARS,LG_PHONE)==0)
        memset(lpHostTel, 0,LG_PHONE);
    else
        memcpy(lpHostTel, lrHDGStruct.TMSTel1, LG_PHONE);

        memcpy(lpTmsNii, lrHDGStruct.TMSNII, 4);
        memcpy(lpPmsNii, lrHDGStruct.PMSNII, 4);
        memcpy(lpNetServerNii, lrHDGStruct.NETSERVERNII, 4);
#ifdef USE_TEST_HOSTINFOENTRY
	ScrCls();
	ScrPrint(0, 0, ASCII|REVER, "    PROPAY DEBUG     ");
	ScrPrint(0, 1, ASCII, "COPY HARD-CODE DATA..");
	ScrPrint(0, 4, ASCII, "MERCH ID : 101");
	ScrPrint(0, 5, ASCII, "STORE ID : 1");
	//ScrPrint(0, 6, ASCII, "TERM ID  : 01010102");//01010103
	ScrPrint(0, 6, ASCII, "TERM ID  : 09020202");//01010103
	DelayMs(3000);
	strcpy(lpMerchId, "01101");//101
	strcpy(lpStoreNum, "1");
	//strcpy(lpTermId, "01010102");//01010103
	strcpy(lpTermId, "03010101");//01010103
	
	ScrCls();

#else
    // Get Merchant ID
    fbLength=LG_MERCHID;
    if (KeyBoardDataCapture(PDS_MERCHANT_ID, IN_NUM|IN_RESET, fbLength, &fbLength, lpMerchId, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}
    
    fbLength=LG_STORENUM;
    if (KeyBoardDataCapture(PDS_STORE_NO, IN_NUM|IN_RESET, fbLength, &fbLength, lpStoreNum, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}
    
    // Get Terminal ID
    fbLength = LG_TERMID;
    if (KeyBoardDataCapture(PDS_TERMINAL_ID, IN_NUM|IN_RESET, fbLength, &fbLength, lpTermId, 200,0) != INPUT_OK)
    {
        return(STAT_NOK);
    }

     //------------------TMS NII---------------
	 fbLength=4;
    if (KeyBoardDataCapture(PDS_TMS_NII, IN_NUM|IN_RESET, fbLength, &fbLength, lpTmsNii, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}

    	//------------------PMS NII---------------
	 fbLength=4;
    if (KeyBoardDataCapture(PDS_PMS_NII, IN_NUM|IN_RESET, fbLength, &fbLength, lpPmsNii, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}

    	 //------------------NETSERVER NII---------------
	 fbLength=4;
    if (KeyBoardDataCapture(PDS_NETSERVER_NII, IN_NUM|IN_RESET, fbLength, &fbLength, lpNetServerNii, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}
    
	//by jarod 20110804
#if 0
	strcpy(lpMerchId, "101");
	strcpy(lpStoreNum, "1");
	//strcpy(lpTermId, "01010103");
	strcpy(lpTermId, "03010101");
#endif

#endif
    
    /*	if(!(TerminalType() == STAT_OK))
    return(STAT_NOK);
    */
    
    //******************* get GPRS params *******************
    GPRSRead(&GPRSInp);
     	
#ifdef USE_TEST_HOSTINFOENTRY
	ScrCls();
	ScrPrint(0, 0, ASCII|REVER, "    PROPAY DEBUG     ");
	ScrPrint(0, 1, ASCII, "COPY HARD-CODE DATA..");
	ScrPrint(0, 4, ASCII, "APN = CMNET");
	ScrPrint(0, 5, ASCII, "IP  = 87.236.48.229");
	ScrPrint(0, 6, ASCII, "PORT= 6666");
	strcpy(GPRSInp.ip_address, "87.236.48.229");
	GPRSInp.hport = 6666;
	strcpy(GPRSInp.apn, "CMNET");
	strcpy(GPRSInp.username, "");
	strcpy(GPRSInp.password, "");
	DelayMs(3000);
	ScrCls();

#else
	memcpy(lpAPN, GPRSInp.apn, strlen(GPRSInp.apn));
     	memcpy(lpUser, GPRSInp.username, strlen(GPRSInp.username));
     	memcpy(lpPwd, GPRSInp.password, strlen(GPRSInp.password));
     	memcpy(lpIP, GPRSInp.ip_address, strlen(GPRSInp.ip_address));
     	sprintf(lpPort, "%04d", GPRSInp.hport);

    //--------------------APN---------------------    
    if (!(fbLength=Enter_data(lpAPN,1)))
        return(STAT_NOK);
    memset(GPRSInp.apn,0,sizeof(GPRSInp.apn));
    memcpy(GPRSInp.apn,lpAPN,fbLength);

    //--------------------user---------------------     
    if (!(fbLength=Enter_data(lpUser,2)))
        return(STAT_NOK);    
    memset(GPRSInp.username,0,sizeof(GPRSInp.username));
    memcpy(GPRSInp.username,lpUser,fbLength);
    
    //--------------------password--------------------
    if (!(fbLength=Enter_data(lpPwd,3)))
        return(STAT_NOK);
    memset(GPRSInp.password,0,sizeof(GPRSInp.password));
    memcpy(GPRSInp.password,lpPwd,fbLength);

    //--------------------IP--------------------
    fbLength=LG_IP;
    if (!Enter_data(lpIP,0))
        return(STAT_NOK);
    memset(GPRSInp.ip_address,0,sizeof(GPRSInp.ip_address));
    memcpy(GPRSInp.ip_address,lpIP,strlen(lpIP));

    //--------------------port--------------------
    if (!(fbLength=Enter_data(lpPort,4)))
        return(STAT_NOK);
    GPRSInp.hport=atoi(lpPort);
    
	//by jarod 20110804
#if 0
	strcpy(GPRSInp.ip_address, "87.236.48.229");
	GPRSInp.hport = 6666;
	strcpy(GPRSInp.apn, "CMNET");
	strcpy(GPRSInp.username, "");
	strcpy(GPRSInp.password, "");
#endif

#endif

	
	//************************************************************
    StrLeftPad(lpMerchId, LG_MERCHID, lpMerchId, '0');
    StrLeftPad(lpStoreNum, LG_STORENUM, lpStoreNum, '0');
    StrLeftPad(lpTermId, LG_TERMID, lpTermId, '0');
    
    //******************* Update Merchant File *******************
    memcpy(lrHDGStruct.MerchId,	lpMerchId, LG_MERCHID);
    memcpy(lrHDGStruct.StoreNum,	lpStoreNum,LG_STORENUM);
    memcpy(lrHDGStruct.TermId, 	lpTermId,  LG_TERMID);
    memcpy(lrHDGStruct.TMSTel1, lpHostTel, LG_PHONE);
    memcpy(lrHDGStruct.TMSTel2, lpHostTel, LG_PHONE);
    memcpy(lrHDGStruct.TMSTel3, lpHostTel, LG_PHONE);
#ifndef USE_TEST_HOSTINFOENTRY
    memcpy(lrHDGStruct.TMSNII, lpTmsNii, 4);
    memcpy(lrHDGStruct.PMSNII, lpPmsNii, 4);
    memcpy(lrHDGStruct.NETSERVERNII, lpNetServerNii, 4);
#endif
    if (HDGWrite(&lrHDGStruct)!=STAT_OK)
        return(STAT_NOK);    
    
    HDGRead(&g);
    if (GPRSWrite(&GPRSInp)!=STAT_OK)
        return(STAT_NOK);
    
    return (STAT_OK);
}

//added by Allen 2016-05-19
WORD SetupDial_upParam(void)
{
	int iRet;
	HDGStruct lrHDGStruct;    // Merchant Parameter Structure
    BYTE lpTel_No[20] = {0};
    BYTE lpTmsNii[5] = {0};
    BYTE lpPmsNii[5] = {0};
    BYTE lpNetServerNii[5] = {0};
    
    WORD lwDummy;
    BYTE fbLength=0;

	Dial_upStruct DIALInp;

	FileReset(FILE_DIAL_UP_PAR);

	//******************* get DIAL_UP params *******************
	if (Dial_upRead(&DIALInp) != STAT_OK)
		return(STAT_NOK);
	//DebugComSend("DIALInp.tel = %s\n", DIALInp.szTelNo);
	memcpy(lpTel_No, DIALInp.szTelNo, strlen(DIALInp.szTelNo));
	//--------------------Tel_No---------------------    
    if (!(fbLength=Enter_data(lpTel_No,10)))
        return(STAT_NOK);
    memset(DIALInp.szTelNo,0,sizeof(DIALInp.szTelNo));
    memcpy(DIALInp.szTelNo,lpTel_No,fbLength);


	if (HDGRead(&lrHDGStruct) != STAT_OK)
		return(STAT_NOK);
    memcpy(lpTmsNii, lrHDGStruct.TMSNII, 4);
    memcpy(lpPmsNii, lrHDGStruct.PMSNII, 4);
    memcpy(lpNetServerNii, lrHDGStruct.NETSERVERNII, 4);

     //------------------TMS NII---------------
	 fbLength=4;
    if (KeyBoardDataCapture(PDS_TMS_NII, IN_NUM|IN_RESET, fbLength, &fbLength, lpTmsNii, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}

    	//------------------PMS NII---------------
	 fbLength=4;
    if (KeyBoardDataCapture(PDS_PMS_NII, IN_NUM|IN_RESET, fbLength, &fbLength, lpPmsNii, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}

    	 //------------------NETSERVER NII---------------
	 fbLength=4;
    if (KeyBoardDataCapture(PDS_NETSERVER_NII, IN_NUM|IN_RESET, fbLength, &fbLength, lpNetServerNii, 200,0) != INPUT_OK)
	{
        return(STAT_NOK);
	}	

#ifndef USE_TEST_HOSTINFOENTRY
    memcpy(lrHDGStruct.TMSNII, lpTmsNii, 4);
    memcpy(lrHDGStruct.PMSNII, lpPmsNii, 4);
    memcpy(lrHDGStruct.NETSERVERNII, lpNetServerNii, 4);
#endif
    if (HDGWrite(&lrHDGStruct)!=STAT_OK)
        return(STAT_NOK);    

    if (Dial_upWrite(&DIALInp)!=STAT_OK)
        return(STAT_NOK);
    
    return (STAT_OK);
}
//add end

WORD ReadHostPhones(BYTE *fpTel1, BYTE *fpTel2, BYTE *fpTel3)
{
    HDGStruct lsGenparm;
    
    if (HDGRead(&lsGenparm)!=STAT_OK)			// Read Error
    {
        return(STAT_NOK);
    }
    
    if(gHostType==AUTH_HOST)
    {
        memcpy( fpTel1, lsGenparm.NETSERVERTel1, LG_PHONE );
        memcpy( fpTel2, lsGenparm.NETSERVERTel2, LG_PHONE );
        memcpy( fpTel3, lsGenparm.NETSERVERTel3, LG_PHONE );
    }
    else if(gHostType==PMS_HOST)
    {
        memcpy( fpTel1, lsGenparm.PMSTel1, LG_PHONE );
        memcpy( fpTel2, lsGenparm.PMSTel2, LG_PHONE );
        memcpy( fpTel3, lsGenparm.PMSTel3, LG_PHONE );
    }
    else
    {
        memcpy( fpTel1, lsGenparm.TMSTel1, LG_PHONE );
        memcpy( fpTel2, lsGenparm.TMSTel2, LG_PHONE );
        memcpy( fpTel3, lsGenparm.TMSTel3, LG_PHONE );
    }
    
    fpTel1[PLenStr(fpTel1,LG_PHONE)] = 0;
    fpTel2[PLenStr(fpTel2,LG_PHONE)] = 0;
    fpTel3[PLenStr(fpTel3,LG_PHONE)] = 0;
    
    fpTel1[LG_PHONE] = 0;
    fpTel2[LG_PHONE] = 0;
    fpTel3[LG_PHONE] = 0;

    return(STAT_OK);    
}



WORD SecurityFunctions(void) // Modified by Kim_LinHB 2013-3-11 build 0.05.0009
{
	BYTE lbMenuLine;
	BYTE FirstItem;
	WORD lwLastSeq;
	WORD lwStatus = STAT_NOK;

	FirstItem = 1;
	gOldSubItem=1;
	//by jarod check Security Menu PW
	if(Check_SecurityMenuPW()!=0) return STAT_NOK;
	while(lwStatus!=STAT_OK)
	{
		lbMenuLine=KeyBoardMenu(6,gSecurityFnctMenu,&FirstItem,gOldSubItem,200,0);  //modified by Allen for ZAIN 2016-05-19
		gOldSubItem=lbMenuLine;
		switch (lbMenuLine)
		{
			case 1:
				HostInfoEntry();
				break;
			case 2 :
				GetBatchNumber(OS_TRUE);
				break;
			case 3 :
				if (Check_TransFileEmpty(&lwLastSeq, FILE_TR) == STAT_NOK)
   	 			{
   	 				DisplayString(DSP_NO_TRANSACTION,3000);
        				break;
    				}
				PrintTransList(OS_TRUE, OS_FALSE, FILE_TR);    // modified by Allen 20151116
			       FileInit(FILE_TR);
			       PaperControl();
				break;
			case 4 :
				DebugMenu();
				break;
			//added by Allen 2016-05-19
			case 5:
				SetupDial_upParam();
				break;
			case 6:
				ConnectMode();
				break;
			//add end
			default :
				lwStatus = STAT_OK;
				break;
		}
	}
	return (STAT_OK);
}



