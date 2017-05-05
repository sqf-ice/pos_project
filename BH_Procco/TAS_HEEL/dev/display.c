#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_define.h"

#include "utility.h"
#include "display.h"
#include "pax_migrate.h"

// show at central of line
void ShowCText(short y, char * str)
{
	short shLen=0, x = 0;
	shLen = strlen(str);
	x =  (DISP_WIDTH - shLen)/2;
	if(x == 0)
		x = 1;
	write_at(str, shLen, (char)x, (char)y);
}

static void DisplayError(const char *input)
{
	int x;

	// ClearScreenInfo(); use below instead:
    ScrClrLine(4, 7);

	x=StrLen((unsigned char *)input);
	if (x<DISP_WIDTH)
        x=((DISP_WIDTH-x)/2);
	else
        x=1;

	display_at(x, 5, (char *)input, CLR_LINE);
	DelayMs(2000);
} 

static void Change_Font(int isType)
{
}

// end of file
