#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.
#include "emvlib.h" // use the path that contains the PAX EMV lib files, but don't copy them to project folder.

#include "pax_migrate.h"
#include "defines.h"

/*
BUILD 0.06.0022
	Modify  THANK YOU FOR USING ALBARAKA TAQSEET CARD to THANK YOU FOR USING ADIB INSTANT FINANCE
*/

const APPINFO AppInfo =
{
	"PROCCO S80",
	"PROCCO_2010_01",
	"0." APP_VERSION_1 ".00" APP_VERSION_2,
	"PAX/PROCCO",
	"VSDC/CPA ACQUIRING APP FOR PROCCO",
	"",
	0xFF,
	0xFF,
	0x01,
	""
};

extern INT Close(INT Handle); // Added by Kim_LinHB 2013-3-15 build 0.06.0016

int event_main(ST_EVENT_MSG *pstEventMsg)
{
    SystemInit();
	return 0;
}


void display_at(BYTE a, BYTE b, char *pszString, BYTE mode)
{
    if (!(mode & NO_CLEAR))
    {
        ScrClrLine(b, b);
    }

    ScrPrint(a*6, b, ASCII, pszString);
}

void write_at(char *pszString, int iLen, char xx, char yy)
{

    ScrPrint(xx*6, yy, ASCII, pszString);
}

void displayBig_at(BYTE a, BYTE b, char *pszString, BYTE mode)
{
    if (!(mode & NO_CLEAR))
    {
        ScrClrLine(b, b);
    }

    ScrPrint(a*8, b, CFONT, pszString);
}

void writeBig_at(char *pszString, int iLen, char xx, char yy)
{

    ScrPrint(xx*8, yy, CFONT, pszString);
}

void display(char *pszString)
{
    Lcdprintf(pszString);
}

int pax_migrate_rename(char *pszOldname, char *pszNewName)
{
	unsigned char buffer[1024];
	int file_OldName,file_NewName;
	long len;

    if (pszOldname==NULL || *pszOldname==0)
    {
        return -1;
    }
    if (pszNewName==NULL || *pszNewName==0)
    {
        return -1;    
    }

	len = filesize(pszOldname);
	if ( (len<0) || (len>=freesize()) )
	{
		return -2;
	}

	file_OldName = open(pszOldname, O_RDWR);
	if (file_OldName < 0)
	{
		return -3;
	}

	if (fexist(pszNewName)>=0)
	{
		if(remove(pszNewName)<0){
			Close(file_OldName); // Added by Kim_LinHB 2014-5-29
			return -1;
		}
	}

	file_NewName = open(pszNewName, O_RDWR|O_CREATE);
	if (file_NewName < 0)
	{
		Close(file_OldName); // Added by Kim_LinHB 2014-5-29
		return -4;
	}

	seek(file_OldName, 0, SEEK_SET);
	seek(file_NewName, 0, SEEK_SET);

	while(1)
	{
		if (len<=sizeof(buffer))
		{	
			read(file_OldName,  buffer, len);
			write(file_NewName, buffer, len);
			break;
		}
		else
		{
			read(file_OldName,  buffer, sizeof(buffer));
			write(file_NewName, buffer, sizeof(buffer));
			len -= sizeof(buffer);
		}
	}

	Close(file_NewName);
	Close(file_OldName);
	remove(pszOldname);

	return 0;
}

void error_tone(void)
{
	Beef(1, 200);
	DelayMs(200);
}

// end of file

