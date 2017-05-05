#include <posapi.h>
#include <posapi_all.h>

#include "data_define_salesman.h"
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
	strcpy(gl_MaintenaceParam.szManagerID, "4444");
	strcpy(gl_MaintenaceParam.szTermID, "29003209");
	
	strcpy(gl_MaintenaceParam.szSalesmanID, "150150150");
	//strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "95.0.181.83");
	strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "193.169.190.118");
	strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, "6062");
	strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, "CMNET");
	gl_MaintenaceParam.ucConnType[0] = ETHERNET;
	gl_MaintenaceParam.ucConnType[1] = ETHERNET;

	strcpy(gl_MaintenaceParam.szTimeSettDownload, "1300");
	strcpy(gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, "1250");
	//strcpy(gl_MaintenaceParam.stFTP.szUser, "ftpuser");
	//strcpy(gl_MaintenaceParam.stFTP.szPwd, "1q2w3eASD");
	strcpy(gl_MaintenaceParam.stFTP.szUser, "lhb");
	strcpy(gl_MaintenaceParam.stFTP.szPwd, "11223344");
	//strcpy(gl_MaintenaceParam.stFTP.szRemoteIP, "193.169.190.55");
	strcpy(gl_MaintenaceParam.stFTP.szRemoteIP, "192.168.1.3");
	strcpy(gl_MaintenaceParam.stFTP.szRemotePort, "21");
	strcpy(gl_MaintenaceParam.stFTP.szFilePath, "/salesman/");
	strcpy(gl_MaintenaceParam.stFTP.stApp.szFileName, "salesman_4.74.bin");
	strcpy(gl_MaintenaceParam.stFTP.stApp.szFileVer, "4.75");
	strcpy(gl_MaintenaceParam.stFTP.stParam[0].szFileName, "AR_PRNDISP.FNT");
	strcpy(gl_MaintenaceParam.stFTP.stParam[0].szFileVer, "4.75");
	gl_MaintenaceParam.stFTP.bForceToDownload = 0;
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
	MyTest();   //hide
#endif

	ScrCls();					//clear screen first

	while(1)
	{
		if(GPRS == gl_MaintenaceParam.ucConnType[0] ||
			GPRS == gl_MaintenaceParam.ucConnType[1])
		{
			DispWNetSignal();
			GetOperatorName();
		}
		Awake_Plan();
		//UpdateApp();
 		DispDefScreen();
	} 
	return 0;
}

