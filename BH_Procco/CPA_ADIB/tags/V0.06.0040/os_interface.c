#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.
#include "lng.h"

#include "debug.h"

#include "SLCLOCK.h"
/*#include "include/emv/trap.h"
*/


#include "defines.h"
#include "os_define.h"
#include "os_interface.h"
#include "utility.h"
#include "display.h"
#include "pax_migrate.h"
#include "ucl_main.h"
#include "appLib.h"
#include "globlePara.h"

extern int h_print_port;

int h_font_file = 0;

INT ghConsole = -1;
INT ghClock=-1;

INT ghDebugPort=-1;

CHAR  gDebugBuf[100] = {0};

OPEN_FILED stOpenFiles={0};

COMM_PARAM_STRUCT gstModemInit;


CHAR CRLF[3]="\n";


BYTE OS_DSP_USER_BREAK[]                      ="  USER BREAK   ";
BYTE OS_DSP_TIME_OUT[]                        ="RECEIVE TIMEOUT";
BYTE OS_DSP_LINE_DROPPED[]                    ="LINE DROPPED   ";

char szKeyMap[MAX_ALPNUM_KEYS][CHAR_PER_KEY]={
	"0- +Q",   "1Z.AB",   "2CDEF",  "3GHIJ",  "4KLMN",  "5OPRS",
	"6TUVW",   "7XY*,",   "8'\"#=",  "9:$~^",  "{}[]\\","&%()!"};


#define UP_ARROW			0x02
#define DOWN_ARROW			0x04
#define BOTH_ARROW			0x08
 

//#define KEY_F6      0xE2
//#define KEY_CAN     0x9B

extern int ShowMenu(menu *m, BYTE TOSeconds, BYTE OptByte); // sint==int

char *StrCpy(unsigned char *s1, unsigned char *s2)
{
	return strcpy((char *)s1, (const char *)s2);
}

VOID OS_KeybdFlush(void)
{
    kbflush();
}

INT OS_OpenMagReaderVx(void)
{
    MagClose();
    MagOpen();
    MagReset();
    return  0;
}

/*
OS_STATUS OS_PrintLine(PCHAR SrcBuf)
{
    PrnStr(SrcBuf);
    PrnStr("\n");
	return 0;
}

INT OS_PrinterOpenVx(void)
{
    PrnInit();
	return OS_OK;
}
*/

BYTE KeyBoardQuestion(BYTE * fPrompt, BYTE * fMessage1, BYTE * fMessage2, WORD fTimeOut)
{
	//KEYB_VAL_MASK | KEYB_CANCEL_MASK
	if (fPrompt)
		ScrPrint(0, 0, CFONT|REVER, "%-16.16s", (char *)fPrompt);
	if (fMessage1)
		ScrPrint(0, 3, CFONT, (char *)fMessage1);	
	if (fMessage2)
		ScrPrint(0, 5, CFONT, (char *)fMessage2);

	Kb_Flush();
	return getkey();
}

uchar Kb_Read( void )
{
	if (kbhit() ==0)	
	{
		return getkey();
	}
    return NOKEY;
}


INT OS_ShowMenu(BYTE ItemCount,BYTE **MenuItems,PBYTE FirstItem,BYTE gOldItem,WORD TimeOut,byte byArabicHeader)
{
	menu m;
	m_entry me[50] ={0};
	int i = 0,rc;

	for (i = 0; i < ItemCount; i++)
	{
		me[i].func =NULL;
		me[i].text =(PCHAR) MenuItems[i+1];
		me[i].parameters = 0;
	}

	m.header         = (PCHAR)MenuItems[0];
	m.no_of_entries  = ItemCount;
	m.menu_entries   = &me[0];

	OS_ClearScreen();
	if (byArabicHeader) 
	{
		rc =   ShowMenu((menu *)&m, (BYTE)TimeOut, Opt_LONGMSG|Opt_ARABIC);
	}
	else
	{
		rc =   ShowMenu((menu *)&m, (BYTE)TimeOut, Opt_LONGMSG);
	}
	OS_ClearScreen();
	return rc;
}

INT OS_ShowMenu2(BYTE ItemCount,BYTE **MenuItems,PBYTE FirstItem,BYTE gOldItem,WORD TimeOut,byte byArabicHeader)
{
	menu m;
	m_entry me[50] ={0};
	int i = 0,rc;
	
	for (i = 0; i < ItemCount; i++)
	{
		me[i].func =NULL;
		me[i].text =(PCHAR) MenuItems[i+1];
		me[i].parameters = 0;
	}
	
	m.header         = (PCHAR)MenuItems[0];
	m.no_of_entries  = ItemCount;
	m.menu_entries   = &me[0];
	
	OS_ClearScreen();
	if (byArabicHeader) 
	{
		rc =   ShowMenu2((menu *)&m, (BYTE)TimeOut, Opt_LONGMSG|Opt_ARABIC);
	}
	else
	{
		rc =   ShowMenu2((menu *)&m, (BYTE)TimeOut, Opt_LONGMSG);
	}
	OS_ClearScreen();
	return rc;
}

INT KeyBoardMenu(BYTE ItemCount, const BYTE* MenuItems[], PBYTE FirstItem, BYTE gOldItem, WORD TimeOut, BYTE byArabicHeader)
{
	return OS_ShowMenu(ItemCount, (BYTE **)MenuItems, FirstItem, gOldItem, TimeOut, byArabicHeader);
}

INT KeyBoardMenu2(BYTE ItemCount, const BYTE* MenuItems[], PBYTE FirstItem, BYTE gOldItem, WORD TimeOut, BYTE byArabicHeader)
{
	return OS_ShowMenu2(ItemCount, (BYTE **)MenuItems, FirstItem, gOldItem, TimeOut, byArabicHeader);
}

void Kb_Flush(void)
{
    kbflush();
}


INT OS_KeybdGet(PBYTE Key, BYTE TimeoutSecond)
{
	*Key = GetOneKey(TimeoutSecond);	
	if (*Key != NOKEY)
	{
		return OS_OK;
	}
	
	return OS_NOK;
}

void OS_DisplayStr(INT iLine, INT iCol, CHAR *szMsg)
{
	if (iCol==STR_CENTER)
	{
		write_at(szMsg, StrLen ((PBYTE)szMsg), (21 - StrLen ((PBYTE)szMsg)) / 2 + 1, iLine);
	}
	else
	{
		write_at(szMsg, StrLen ((PBYTE)szMsg), iCol, iLine);
	}
}

INT OS_ClkReadDate(WORD *Week,WORD *Day,WORD *Month,WORD *Year)
{
	char ReadTime[7];

	memset(ReadTime, 0, sizeof(ReadTime));
	GetTime(ReadTime);
	
	*Week	= ReadTime[6];
	*Day	= ReadTime[2];
	*Month	= ReadTime[1];
	*Year	= 2000 + 10*(ReadTime[0] &0xF0)/16 + (ReadTime[0] &0x0F);

	return 0;
}

INT OS_ClkReadDateTime(PBYTE Input)
{
	char ReadTime[7];

    memset(ReadTime, 0, sizeof(ReadTime));
	GetTime(ReadTime);
	sprintf((char *)Input, "%02X:%02X:%02X   %02X/%02X/%02X",
            ReadTime[3], ReadTime[4], ReadTime[5], ReadTime[2], ReadTime[1], ReadTime[0]);    // HH:MM:SS date/month/year
    return 0;
}


INT ClkSetDate(PBYTE lpDate)
{
	char	sBuffer[16];

    GetTime(sBuffer);
    asc2bcd(sBuffer, lpDate, 6);
    SetTime(sBuffer);

	return OS_OK;
}

void OS_Sftkey(BYTE *newkey)
{
}

void OS_Sftencrypt(BYTE *cleartext)
{
}

INT ClkSetTime(PBYTE lTime)
{
	char	sBuffer[16];

    GetTime(sBuffer);
    asc2bcd(sBuffer+3, lTime, 6);
    SetTime(sBuffer);

	return OS_OK;
}

INT ClkUpdateDateTime(PBYTE lDateTime)
{
	char	sBuffer[7] = "";
	char lbaWorkBuf[16] = "";

    GetTime(sBuffer);
    memcpy(sBuffer+1, lDateTime, 5);
    SetTime(sBuffer);
    
	PubBcd2Asc(lDateTime+2, 3, gPTStruct.TransTime);
	return OS_OK;
}

INT OS_DeleteFile(PBYTE FileName)
{
	WORD lwStatus;
	lwStatus=remove((PCHAR)FileName);
	if (lwStatus<0)      
	{
		return NO_ENTRY;
	}
    else
	{
	    return 0;
	}
}

INT OS_ClearScreen(void)
{
	ScrCls();
	return OS_OK;
}


/************************************************************************/
/*         axalto to vx platform                                        */
/************************************************************************/
VOID CloseFile(PCHAR FileName)
{
	INT i= 0;
	INT j= 0;
	INT total = 0;

	for ( i=0; i<MAX_OPEN_FILE; i++)
	{
		j=memcmp(&stOpenFiles.gFiles[i][0], FileName, LG_FILENAME);
		if (!j)
		{
			Close(stOpenFiles.ghFiles[i][0]);
			memset(&stOpenFiles.gFiles[i][0],0,sizeof(stOpenFiles.gFiles[i][0]));
			stOpenFiles.ghFiles[i][0] = 0;
			total++;
		}
	}
	stOpenFiles.gTotalOpenedFiled -= total;
}

VOID AddFile(PBYTE FileName, INT Handle)
{
	INT i= 0;
	INT total = 0;

	for ( i = 0; i< MAX_OPEN_FILE; i++)
    {
		if (stOpenFiles.ghFiles[i][0]==0)
            break;
    }

	if (i==MAX_OPEN_FILE)
	{
        Utils_DebugPrintMsg("TOO MUCH OPENED FILE\n");	
		return;
	}
	
	memcpy(&stOpenFiles.gFiles[i][0],FileName,LG_FILENAME);
	stOpenFiles.ghFiles[i][0] = Handle;
	stOpenFiles.gTotalOpenedFiled++;
}

INT Delete(PBYTE FileName)
{
	CloseFile((PCHAR)FileName);
	return OS_DeleteFile(FileName);
}

INT NCreate(PBYTE FileName, INT Mode)
{
	INT FileMode = 0;
	INT hFileHandle;

	CloseFile((PCHAR)FileName);

	hFileHandle = open((char *)FileName, O_RDWR|O_CREATE);	
	if (hFileHandle < 0)
	{
		return hFileHandle;
	}

	AddFile(FileName, hFileHandle);
	return hFileHandle;
}

INT Open(PBYTE FileName, BYTE Mode)
{
	INT FileHandle;

	CloseFile((PCHAR)FileName);
	
	FileHandle =  open((char *)FileName, Mode);
	if (FileHandle >= 0) 
	{
        AddFile((PBYTE)FileName, FileHandle);
	}

	return FileHandle;
}

INT Read(INT FileHandle, PBYTE Buf, INT Len)
{
	return  read(FileHandle, (PCHAR)Buf, Len);
}

INT Write(INT FileHandle, PBYTE Buf, INT Len)
{
	INT WriteLen;
	WriteLen =  write(FileHandle, (PCHAR)Buf, Len);

	if (WriteLen==Len)
		return 0;

	return OS_NOK;
}

INT Seek(INT FileHandle, DWORD Pos, INT Where)
{
    // Gary
	//return  lseek(FileHandle, Pos, Where);
    return seek(FileHandle, Pos, Where);
}

INT Close(INT Handle)
{
	if (Handle >= 0)
		return close(Handle);
	return -1;
}

INT Rename(PBYTE Old, PBYTE New)
{
	CloseFile((PCHAR)Old);
	return pax_migrate_rename((PCHAR)Old, (PCHAR)New);
}

INT GetFileSize(PBYTE FileName, DWORD *llLogicSize)
{
    if (fexist(FileName)<0)
    {
        return NO_ENTRY;
    }

    *llLogicSize = filesize(FileName);
	return STAT_OK;
}

INT ClkReadDate(PBYTE Time,	INT Mode)
{
	WORD iWeek,iDay,iMonth,iYear;
	CHAR szTemp[7] ={0};

	OS_ClkReadDate(&iWeek, &iDay, &iMonth, &iYear);
	iYear-=2000;
	
	if(Mode==F_DDMMYY)
	{
		sprintf((char *)Time,"%02x%02x%02d",iDay, iMonth, iYear);
	}
	else if ( Mode==F_YYMMDD)
	{
		sprintf((char *)Time,"%02d%02x%02x",iYear, iMonth, iDay );
	}
	else if ( Mode==F_MMDDYY)
	{
		sprintf((char *)Time,"%02x%02x%02d",iMonth, iDay, iYear);
	}
	else 
	{
		sprintf((char *)Time, "( DATE TIME ERROR )");
		return STAT_NOK;
	}
	return STAT_OK;
}

void GetPrnDateTime(char *pszOutBuff)
{
    char    szBuff[64];

    memset(szBuff, 0, sizeof(szBuff));
    ClkReadDate(szBuff,F_DDMMYY);
    memcpy(pszOutBuff, szBuff, 2);
    strcat(pszOutBuff, "/");
    memcpy(pszOutBuff+strlen(pszOutBuff), szBuff+2, 2);
    strcat(pszOutBuff, "/");
    memcpy(pszOutBuff+strlen(pszOutBuff), szBuff+4, 2);
    strcat(pszOutBuff, "   ");

    memset(szBuff, 0, sizeof(szBuff));
    ClkReadTime(szBuff);
    memcpy(pszOutBuff+strlen(pszOutBuff), szBuff, 2);
    strcat(pszOutBuff, ":");
    memcpy(pszOutBuff+strlen(pszOutBuff), szBuff+2, 2);
}

INT ClkReadTime(PBYTE bTime)
{
	char ReadTime[7] = "";

    memset(ReadTime, 0, sizeof(ReadTime));
	GetTime(ReadTime);
	sprintf((char *)bTime, "%02X%02X%02X",
		ReadTime[3], ReadTime[4], ReadTime[5]);    // HH:MM:SS date/month/year
    return 0;
}

INT ManRequestMessage(INT Mode1, INT Mode2, INT Len, VOID *Type)
{
	return 1;
}

INT GetTerminalConfig(TERMCONFIGSTRUCT *term)
{
	CHAR Hardware[20]={0};
	CHAR SerialNum[20]={0};
	CHAR Temp[40]={0};  // Modified by Kim_LinHB 2013-3-15 build 0.06.0016
	INT i =0, j = 1;

    // Gary change
    /*
	SVC_INFO_SERLNO(Temp) ;

	for (i = 0; i < strlen(Temp); i++ )
	{
		if (Temp[i] != '-')
		{
			SerialNum[j] = Temp[i];
			j++;
		}
	}
	
	SVC_INFO_MODELNO(Hardware);
	SerialNum[0] = Hardware[1];
    */

    memset(Temp, 0, sizeof(Temp));
#ifdef USE_TEST_HOSTINFOENTRY
	strcpy(Temp,"29003208");  //29003700  0.06.0031
#else
	ReadSN(Temp);
#endif
	i=strlen(Temp);
    if (i < LG_SERIALNUM)
    {
        StrLeftPad(Temp, LG_SERIALNUM, Temp,'0');
    }

	strcpy(term->SerialNum, Temp);
	sprintf(term->Hardware, "%.2s", Hardware);

	term->DialMode = 'T';

	return STAT_OK;
}

void BfAscii(char *Out, unsigned char * In, int len)
{
	int i;
	unsigned char TempByte;

	for (i = 0; i < len; i++)
    {
		TempByte = ((In[i] & 0xF0) >> 4);
		*Out = 0x30 + TempByte;
		if ((TempByte >= 0x0A) && (TempByte <= 0x0F))
			*Out += 0x07;
		Out++;
		TempByte = (In[i] & 0x0F);
		*Out = 0x30 + TempByte;
		if ((TempByte >= 0x0A) && (TempByte <= 0x0F))
			*Out += 0x07;
		Out++;
	}
	return;
}

INT OS_LogTo(int device, const char* fmt, ...)
{
	return 1;
}

INT OS_LogToHex(char *header,void *buf, int buf_size, int device)
{
	return OS_TRUE;
}

INT GetApplicationConfig(APPLCONFIGSTRUCT *pApplConfig)
{
	//by jarod 
	//Change version from 1.22 to 1.30
	// Modified by Kim_LinHB 2013-9-5 0.06.0028
	StrCpy(pApplConfig->ApplVersion,        (PBYTE)APP_VERSION_2);
	StrCpy(pApplConfig->ApplCompatibility,  (PBYTE)APP_VERSION_1);
	StrCpy(pApplConfig->ApplTitle,          (PBYTE)"BAH APPLICATION");

	return STAT_OK;
}

INT DeleteKey(INT Handler,PBYTE Record,RECORD_DEF *RecDef,INT Pos)
{
    // Gary : delete blacklist processing
    return 0;
}

DWORD Scan_MillSec(void)
{
/*
#ifndef DEVELOP_VERSION
	return read_ticks();
#endif
*/
	return 0;
}

void Display_Inverst(char *gelen,char clr_flag)
{
	char tmp_str[22];

	if (clr_flag)
    {
		ScrCls();
    }

//	gotoxy(1,1);
    ScrGotoxy(0,0);

// 	if (set_font("nadir2.fon"))
// 		return;
    ScrFontSet(0);

	memset(tmp_str,30,21);
	tmp_str[21]=0;
	display(tmp_str);

	memset(tmp_str, 0x0, 22);
	memcpy(tmp_str,gelen, 21);
	display(tmp_str);

	memset(tmp_str,31,21);
	tmp_str[21]=0;
	display(tmp_str);

// 	if (set_font("nadir1.fon"))
// 		return;
    ScrFontSet(1);
}


BYTE GetOneKey(BYTE TOSeconds)
{
	BYTE ret = 0;

	if ((TOSeconds != TO_NONE) && (TOSeconds > TO_30SEC))			// If "Wait Forever" not intentionally selected,
	{
		TOSeconds = /*TO_30SEC*/TO_1MIN;										//     maximum timeout is 30 seconds by default.
	}

    TimerSet(0, TOSeconds*10);
    while (1)
    {
		ret = Kb_Read();
		if (ret!=NOKEY)
		{
			return ret;
		}

		if (TOSeconds != 0)
        {									// If TO_NONE, wait forever. 													// Calculate if TO occured.
			if (TimerCheck(0)==0)
			{
                return NOKEY;
			}
		}
    }
}

char GetOneKeyCheckTime(BYTE TOSeconds, DWORD *pDeciSeconds)
{
    char    chKey;
	ushort	usTimerLeft;

    chKey = GetOneKey(TOSeconds);
	usTimerLeft = TimerCheck(0);
    *pDeciSeconds = TOSeconds*10 - usTimerLeft;
    return chKey;
}

VOID OS_Beep(INT Time)
{
	Beef(7, Time);
}

char chGetKeyPress(void)
{
	Kb_Flush();
    return getkey();
}

// Returns number of characters input
int inGetStringWithPrompt(char *prompt, char *string, int maxlen)
{
	char key;
	char buf[50];
	short i;

	OS_ClearScreen();
	
	i = 0;
	if (*prompt)
    {
		//write_at(prompt, strlen(prompt), 1, 1);
        if (strlen(prompt)>16)
        {
            ScrPrint(0, 2, ASCII, "%.21s", prompt);
        } 
        else
        {
            ScrPrint(0, 2, CFONT, "%.16s", prompt);
        }
    }

    memset(buf, 0, sizeof(buf));
	if (*string)
	{
		strcpy(buf, string);
		i = strlen(buf);
		//write_at(buf, strlen(buf), 1, 2);
	}

    while (1)
    {
        i = strlen(buf);
        ScrClrLine(4, 7);
        if (i>16)
        {
            ScrPrint(0, 4, CFONT, "%-16.16s", buf);
            ScrPrint(0, 6, CFONT, "%-16.16s", buf+16);
        } 
        else
        {
            ScrPrint(0, 6, CFONT, "%-16.16s", buf);
        }

        key = chGetKeyPress();
		if (key==KEYCANCEL)
		{
			return 0;
		}
        else if (key==KEYENTER)
        {
            break;
        }
		else if (key==KEYCLEAR)
		{
			if (i==0)
            {
				error_tone();
            }
			else
			{
				buf[i-1] = 0;
			}
		}
		else if ((key >= KEY0) && (key <= KEY9))
		{
			if (i >= maxlen)
            {
				error_tone();
            }
			else
			{
				buf[i] = key;
                buf[i+1] = 0;
			}
		}
		else
		{
			error_tone();
		}
    }

	// Modified to copy only the specified length. ravi_B3
	strncpy(string, (char*)buf, maxlen);
	return strlen((char*)buf);
}

INT KeyBoardDataCapture(PBYTE Title, INT Mode, BYTE MinLen, PBYTE MaxLen, PBYTE Input, INT TimeOut, INT Flag)
{
	char chTemp[50]={0};
	
	OS_ClearScreen();	
	*MaxLen = inGetStringWithPrompt((PCHAR)Title,(PCHAR)Input, (INT)*MaxLen);
	OS_ClearScreen();
	
	if (*MaxLen==0)
    {
		return INPUT_NOK;
    }
	
	return INPUT_OK;
}

BYTE KeyBoardValid( PCHAR fPrompt, PCHAR fMessage1, PCHAR fMessage2, 
					   PCHAR fMsgHotKey1, PCHAR fMsgHotKey2, PCHAR fMsgHotKey3,
					   BYTE fKeyMask, BYTE fTimeOut)
{
    char szTemp[50] ={0};
    int len = 0,templen;
    BYTE Key = 0;
    char szMsg1[50]={0};
    char szMsg2[50]={0};
    
    strcpy(szMsg1, fMessage1);
    strcpy(szMsg2, fMessage2);
    
    StrTrim((PBYTE)szMsg1,(PBYTE)szMsg1,' ');
    StrTrim((PBYTE)szMsg2,(PBYTE)szMsg2,' ');
    
    memset(szTemp,0x20,23);
    szTemp[22] = 0;
    
    
    OS_ClearScreen();
    if (strlen(szMsg1)>16 || strlen(szMsg2)>16)
    {
        ScrPrint(0, 3, ASCII, "%.21s", szMsg1);
        ScrPrint(0, 4, ASCII, "%.21s", szMsg2);
    }
    else
    {
        ScrPrint(0, 2, CFONT, szMsg1);
        ScrPrint(0, 4, CFONT, szMsg2);
    }
    
    templen = 0;
    memset(szTemp, ' ', 21);
    if (*fMsgHotKey1)
    {
        templen += strlen(fMsgHotKey1);
        strncpy(szTemp, fMsgHotKey1, templen);
        len = templen;
    }
    
    if (*fMsgHotKey3)
    {
        if (strlen(fMsgHotKey3)+templen > 16)
        {
            templen += strlen(fMsgHotKey3);
            strncpy(szTemp+21-strlen(fMsgHotKey3), fMsgHotKey3, strlen(fMsgHotKey3));
        }
        else
        {
            templen += strlen(fMsgHotKey3);
            strncpy(szTemp+16-strlen(fMsgHotKey3), fMsgHotKey3, strlen(fMsgHotKey3));
        }
    }

    if (templen>16)
    {
        ScrPrint(0, 7, ASCII, szTemp);
    }
    else
    {
        ScrPrint(0, 6, CFONT, szTemp);
    }
    
    do 
    {
        if (OS_KeybdGet(&Key,fTimeOut)== OS_OK)
        {
            if (fKeyMask & KEYB_CANCEL_MASK)
            {
                if (Key==KEYCANCEL) 
                    break;
            }
            if (fKeyMask & KEYB_VAL_MASK)
            {
                if (Key==KEYENTER) 
                    break;
            }                    
        }
        else
        {            
            break;
        }        
    } while(1);
    
    return Key;
}

INT OS_PrintBmp(BYTE  *bitmap, BYTE width,
                BYTE height8, const char  *text)
{
#if 0
	int rc; 
	int h_logofile;

	p3300_init(h_print_port, 6);
	DelayMs(200);

	h_logofile = open ((char *)bitmap, O_RDWR);
	if( h_logofile < 1 )
    {
		DelayMs (500);
		return OS_NOK;
	} 

	rc =  p3300_dnld_graphic_file( h_print_port, h_logofile );
	if( rc ) 
	{
		DelayMs (500);
		return OS_NOK;
	} 
	close( h_logofile );
	p3300_print_graphic( h_print_port, 0, 60);

	return OS_OK;
#endif

	return OS_OK;
}

BYTE AlgLuhnKey(BYTE CardNumSize, PBYTE CardNum)
{

	BYTE i, len;
	BYTE luhn = 0, c;

	len = CardNumSize;
	if (len < 2)
		return 0;
	len--;						/* skip last char in PAN - the luhn number */
	for (i = 0; i < len; i++) 
	{
		c = CardNum[len - i - 1];
		if (!isdigit(c))
			return 0xff;
		c -= '0';
		if ((i & 1)==0) {
			c *= 2;
			if (c >= 10)
				c -= 9;
		}
		luhn += c;
		luhn %= 10;
	}
	luhn = (10 - luhn) % 10 +'0';
	return luhn;
}

/************************************************************************/
/*          vx platform                                                 */
/************************************************************************/


WORD StartPrinter(void)
{
	uchar	ucRet;

    while (1)
    {
	    ScrClrLine(0, 7);
	    PubDispString(_T("PRINTING..."), 4|DISP_LINE_LEFT);
        ucRet = PrnStart();
        if( ucRet==PRN_OK || ucRet==PRN_CANCEL )
        {
            return (ucRet==PRN_OK ? STAT_OK : STAT_NOK);
        }
        if (ucRet==PRN_NO_PAPER)
        {
	        ScrClrLine(2, 7);
	        PubDispString(_T("NO PAPER."), 2|DISP_LINE_LEFT);
	        PubDispString(_T("[ENTER ] RETRY"), 4|DISP_LINE_LEFT);
	        PubDispString(_T("[CANCEL] QUIT"),  6|DISP_LINE_LEFT);
            return STAT_NO_PAPER;
        }

        DispPrnError(ucRet);
        PubWaitKey(3);
        return STAT_NOK;
    }

/*
	while( 1 )
	{
		ucRet = PrnStatus();
		if( ucRet==PRN_OK || ucRet==PRN_CANCEL )
		{
			return (ucRet==PRN_OK ? 0 : 1);
		}
		else if( ucRet==PRN_BUSY )
		{
			DelayMs(500);
			continue;
		}
		ScrClrLine(2, 7);
		DispPrnError(ucRet);
		if( ucRet!=PRN_NO_PAPER )
		{
			PubWaitKey(3);
			break;
		}
		
		ucKey = PubWaitKey(60);
		if( ucKey==KEYCANCEL || ucKey==NOKEY )
		{
			break;
		}
		ScrClrLine(2, 7);
		PubDispString(_T("PRINTING..."), 4|DISP_LINE_LEFT);
		PrnStart();
	}	
	return 0;
    */
}

void DispPrnError(uchar ucErrCode)
{
	switch( ucErrCode )
	{
	case PRN_BUSY:
		PubDispString(_T("PRINTER BUSY"), 4|DISP_LINE_CENTER);
		break;
		
	case PRN_NO_PAPER:
		PubDispString(_T("OUT OF PAPER"), 4|DISP_LINE_CENTER);
		break;
		
	case PRN_DATA_ERR:
		PubDispString(_T("PRN DATA ERROR"), 4|DISP_LINE_CENTER);
		break;
		
	case PRN_ERR:
		PubDispString(_T("PRINTER ERROR"), 4|DISP_LINE_CENTER);
		break;
		
	case PRN_NO_DOT:
		PubDispString(_T("FONT MISSING"), 4|DISP_LINE_CENTER);
		break;
		
	case PRN_DATA_BIG:
		PubDispString(_T("PRN OVERFLOW"), 4|DISP_LINE_CENTER);
		break;
		
	default:
		PubDispString(_T("PRINT FAILED"), 4|DISP_LINE_CENTER);
		break;
	}
	PubBeepErr();
}
/************************************************************************/
/*          vx platform end                                             */
/************************************************************************/

/******************************************************************************
Routine Name	: vDisplay_menu()
Description		: This will display the menuitems on the screen 
Inputs			: ucMenuitems, iInitItem, iDiffItem
Outputs			: 
Returns			: 
*******************************************************************************/
void vDisplay_menu(unsigned char ucMenuitems[][22], int iInitItem, int iDiffItem)
{
	int i;
	char chDisplayFlag = 0;

	if(chDisplayFlag)
	{
		for (i = 1; i < 5; i++)
			display_at (1, i, " ", CLR_LINE);

		if (iDiffItem==1)
		{
			display_at (1, 1,(char *) ucMenuitems[iInitItem-1], CLR_LINE);
		}
		else if (iDiffItem==2)
		{
			display_at (1, 1, (char *) ucMenuitems[iInitItem-1], CLR_LINE);
			display_at (1, 3, (char *) ucMenuitems[iInitItem], CLR_LINE);
		}
		else if (iDiffItem > 2)
		{
			display_at (1, 1, (char *) ucMenuitems[iInitItem-1], CLR_LINE);
			display_at (1, 2, (char *) ucMenuitems[iInitItem], CLR_LINE);
			display_at (1, 3, (char *) ucMenuitems[iInitItem+1], CLR_LINE);
		}
	}
	else
	{
		for (i = 3; i < 8; i++)
			display_at (1, i, " ", CLR_LINE);

		if (iDiffItem==1)
		{
			display_at (1, 3,(char *) ucMenuitems[iInitItem-1], CLR_LINE);
		}
		else if (iDiffItem==2)
		{
			display_at (1, 3, (char *) ucMenuitems[iInitItem-1], CLR_LINE);
			display_at (1, 5, (char *) ucMenuitems[iInitItem], CLR_LINE);
		}
		else if (iDiffItem > 2)
		{
			display_at (1, 3, (char *) ucMenuitems[iInitItem-1], CLR_LINE);
			display_at (1, 5, (char *) ucMenuitems[iInitItem], CLR_LINE);
			display_at (1, 7, (char *) ucMenuitems[iInitItem+1], CLR_LINE);
		}
	}
}

//by jarod 2011.06.07
int	Check_SecurityMenuPW()
{
	BYTE uRet;
//  BYTE timebuffer[7];
	BYTE pszOut[12];
// 	BYTE GenPW[4];
// 	BYTE Month,Day,Hour,uRet;
// 
// 	memset(timebuffer,0,sizeof(timebuffer));
// 	memset(pszOut,0,sizeof(pszOut));
// 	GetTime(timebuffer);
// 
// 	Month=timebuffer[1];
// 	Day=timebuffer[2];
// 	Hour=timebuffer[3];
// 
// 	if((Month/16)>0)	Month=(Month/16)*10+Month%16;
// 	if((Day/16)>0)	Day=(Day/16)*10+Day%16;
// 
// 	sprintf(AutoPassword,"%02d%02d%02d%02X", Month, Day, Month+Day, Hour);

	OS_ClearScreen();
	ScrPrint(0, 3, ASCII, " Input Security Code");

	ScrGotoxy(0,6);
	memset(pszOut, 0, sizeof(pszOut));
	uRet = GetString(pszOut, 0x6D, 4, 4);
	if(uRet != 0) return -1;
	if(memcmp(pszOut + 1, AutoPassword + 6, 4) != 0)//AutoPassword + 4
	{
		OS_ClearScreen();
		ScrPrint(0, 3, ASCII, " Security Code Wrong");
		DelayMs(2000);
		return -1;
	}
	return 0;
}