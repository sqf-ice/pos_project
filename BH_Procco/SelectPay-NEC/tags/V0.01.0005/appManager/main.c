
#include <posapi.h>

#define _PAX_MANAGER
#define APP_MNG_NAME "APP MANAGER"

extern int ReadAppInfo(unsigned char AppNo, APPINFO* AppInfo);
extern int RunApp(unsigned char AppNo);
extern int  DoEvent(unsigned char AppNo, ST_EVENT_MSG *msg);


/* -------------------------------------------------------------------------- */
const APPINFO AppInfo={
    APP_MNG_NAME,
		"PAX_MANAGER",
		"1.2",
		"APPI",
		"SelectPay App Manager",
		"",
		0,
		0,
		0,
		""
};

/* -------------------------------------------------------------------------- */

int event_main(ST_EVENT_MSG *msg)
{	
    return 0;
}

int OS_ClkReadDateTime(char* Input)
{
	char ReadTime[7];

    memset(ReadTime, 0, sizeof(ReadTime));
	GetTime(ReadTime);
	sprintf((char *)Input, "%02X:%02X:%02X   %02X/%02X/%02X",
            ReadTime[3], ReadTime[4], ReadTime[5], ReadTime[2], ReadTime[1], ReadTime[0]);    // HH:MM:SS date/month/year
    return 0;
}

void Main_IdleScreen(void)
{
	char Time[50] ={0};	

	//OS_ClkReadDateTime((char*)Time);
	//ScrPrint(0,  0, CFONT|REVER, "%-16.8s", Time);
	//ScrPrint(78, 1, ASCII|REVER, "%.8s", Time+11);
	ScrPrint(0,  0, CFONT|REVER, "   App.Manager  ");

    ScrPrint(0, 4, CFONT, "INSERT CARD");
    ScrPrint(0, 6, CFONT, "OR SELECT MENU");
}

void App_SelectScreen(void)
{
	int i,j,AppNum;
	APPINFO	ai[25];
	//unsigned char keyValue = 0;
	ST_EVENT_MSG  stEvent ;
	
	for(i = 1,AppNum = 0; i < 24; i++)
	{
		if(0 == ReadAppInfo(i, &ai[AppNum]))
		{
			AppNum++;			
		}
	}
	
	if(!AppNum)
	{
		ScrPrint(0,4,CFONT,"No subapp");
		//for(;;);
		DelayMs(2000);
		return;
	}
	
	while(1)
	{
		ScrPrint(0, 0, CFONT|REVER, "   SELECT APP   ");
		j=0;
		if(j<AppNum)   ScrPrint(0,2,CFONT,"1-%s",ai[j].AppName);
		if(j+1<AppNum) ScrPrint(0,4,CFONT,"2-%s",ai[j+1].AppName);
		if(j+2<AppNum) ScrPrint(0,6,CFONT,"3-%s",ai[j+2].AppName);
		
		// key press event
		if (kbhit() == 0x00)
		{
			memset(&stEvent, 0, sizeof(ST_EVENT_MSG));
			stEvent.MsgType = KEYBOARD_MSG;
			stEvent.KeyValue = getkey();
			switch (stEvent.KeyValue)
			{
			case KEY1:
				//RunApp(ai[0].AppNum);
				//ret = DoEvent(ai[0].AppNum,&stEvent);
				if(AppNum >= 1)
				{
					//RunApp(ai[0].AppNum);
					DoEvent(ai[0].AppNum,&stEvent);
					ScrCls();
				}
				break;
			case KEY2:
				if(AppNum >= 2)
				{
					DoEvent(ai[1].AppNum,&stEvent);
					ScrCls();
				}
				break;
			case KEY3:
				if(AppNum >= 3)
				{
					DoEvent(ai[2].AppNum,&stEvent);
					ScrCls();
				}
				break;	
			case KEYCANCEL:
				ScrCls();
				return;
			default:
				break;
			}
			continue;
		}
	}
	
}

int main(void)
{
	unsigned char keyValue ;

	SystemInit();
	//ScrCls();
	//ScrPrint(0, 0, CFONT|REVER, "Multi-App Manage");

	kbflush();
	while( 1 )
	{
		ScrCls();
		Main_IdleScreen();

        while( 1 )
		{
			Main_IdleScreen();
			// Process iccdetect card  event
			if (IccDetect(0)==0)
			{
				Beef(7, 200);
				ScrCls();
				App_SelectScreen();
				continue;
			}
		
			// key press event
			if (kbhit() == 0x00)
			{
				keyValue = getkey();
				switch (keyValue)
				{
				case KEYMENU:
					ScrCls();
					App_SelectScreen();
					break;
				default:
					break;
				}
				continue;
			}
		}
		
	}
	
	return 0;
}


/* -------------------------------------------------------------------------- */


//subapp1:



