#include <posapi.h>
#include <posapi_all.h>

#include "data_define_merchant.h"
#include "framework.h"
#include "connect.h"

const APPINFO	AppInfo = 
{
	APP_NAME,
	"00001",
	STR_VERSION,  
	"PAX TECHNOLOGY",
	"APPLICATION PROGRAM",
	"\x02\x08\x01\x01\x00\x59\x01",
	0xFF,
	0xFF,
	0x00,
	""
};

 
int event_main(ST_EVENT_MSG *msg)
{
	return 0;
}

void MyTest()
{
	//offline
	strcpy(gl_MaintenaceParam.szTermID, TEST_ID);

	strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "193.169.190.118");
	//strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "192.168.1.3");
	strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, "cmnet");
	
	//strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "172.16.20.76");
	gl_MaintenaceParam.stConnParam.stParamEth.bDHCP = 0;
	strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szLocalIP, "172.16.20.211");
	strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szNetMask, "255.255.255.0");
	strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szGatewayIP, "172.16.20.1");
	strcpy(gl_MaintenaceParam.stConnParam.stParamEth.szDNS, "172.16.20.1");
	strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, "6062");
	gl_MaintenaceParam.ucConnType[0] = ETHERNET;
	gl_MaintenaceParam.ucConnType[1] = ETHERNET;
	//gl_MaintenaceParam.stFTP.bForceToDownload = 1;
}

int main(void)
{
	unsigned char buffer[100];

	if(SystemInit() == 0)					// if first time, do something ?
	{
		;///
	}

#ifdef ForAlperTest
	kbmute(0);
#endif

	memset(buffer,0,sizeof(buffer));
	GetTermInfo(buffer);           //.04.54.20.03.0D = P80 //.06.54.20.03.0D = P90
	gl_TermMode = UNKNOWN_TERMINAL;
	memset(gl_TermName,0,sizeof(gl_TermName));

	if (buffer[0]==_TERMINAL_P80_)
	{
		gl_TermMode = buffer[0];
		strcpy((char *)gl_TermName,"P80");
	}
	else if (buffer[0]==_TERMINAL_P90_)
	{
		gl_TermMode = buffer[0];
		strcpy((char *)gl_TermName,"P90");
	}
	else if(buffer[0]==_TERMINAL_S80_)
	{
		gl_TermMode = buffer[0];
		strcpy((char *)gl_TermName,"S80");
	}
	else if(buffer[0]==_TERMINAL_S90_)
	{
		gl_TermMode = buffer[0];
		strcpy((char *)gl_TermName,"S90");
	}

	memset(gl_TermSN,0,sizeof(gl_TermSN));
	GetSerialNumber(gl_TermSN);

#ifdef FARSI  //v3.8	
	if (ArInit(3)!=AR_SUCCESS)
	{
		while(1)
		{
			ScrCls();
			ScrPrint(0,0,0,"Error:AR_PRNDISP.FNT error\nPls download ParamFile");
			getkey();
		}
	}
#endif

	gl_bFirstRun = CheckFirstRun();
	DataInit();
	LangInit();
	
#ifdef LHB_TEST
	MyTest();
#endif 

	ScrCls();					//clear screen first

	// Added by Kim_LinHB 2013-8-16
	UpdateStock();

	while(1)
	{
		if(GPRS == gl_MaintenaceParam.ucConnType[0] ||
			GPRS == gl_MaintenaceParam.ucConnType[1])
		{
			DispWNetSignal();
			GetOperatorName();
		}

		Awake_Plan();
		
		if(gl_MaintenaceParam.bWelcomePage)
 			DispDefScreen();
		else	
		{
			UpdateApp();
			DispMainMenu();
		}
	} 
	return 0;
}

