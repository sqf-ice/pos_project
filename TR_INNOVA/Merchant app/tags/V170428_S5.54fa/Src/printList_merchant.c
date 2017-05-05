#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "data_define_merchant.h"
#include "framework.h"
#include "transaction.h"
#include "printList_merchant.h"
#include "logo/Logo.h"
#include "BarcodePrint.h"   //added by Allen 2016-11-16


extern unsigned char *gl_pStr_EN[];
#ifdef FARSI
extern unsigned char *gl_pStr_Arabic[];
#endif
static unsigned char sg_bCanCelPrn = FALSE;
static unsigned char sg_bNoPaperCountinue = FALSE;

#define LINE_CNT_MAX	50		 // Added by Kim_LinHB 2013/3/22  v4.56 prnStr buff Line once
#ifdef _S_SERIES_
static unsigned char sg_ucPrintLineCnt = 0; // Added by Kim_LinHB 2013-3-22 v4.56
#endif
// Added by Kim_LinHB 2014-7-23  only for P80
static unsigned char sg_ucPrintDirectly = 1; 
static void SetPrintMode(unsigned char mode); // 0: non-directly 1: directly

static void MyPrnInit(void);
// Modified by Kim_LinHB 2014-5-5
static void PrintBuffer(const unsigned char *pucBuffer,unsigned char Position, unsigned char ucSize, short step);
static uchar PrintStart(unsigned char bForce); // Added by Kim_LinHB 2013/3/22 v4.56
static unsigned char Prn_SaleCodes_FourE_Voucher(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode,int saleIndex);//

// a special case, print string without checking character. p80 can't print bold (the font lib no supported) so use the arabic font to print
// before using this function the ASC characters need to be reversal using vReversalStr().
// Modified by Kim_LinHB 2014-5-5
static void PrintBuffer_AR(const unsigned char *pucBuffer,unsigned char Position, unsigned char ucSize, short step);
// print (No) empty lines
static void Jump_Lines(unsigned char No);

static void Prn_A_LineOf(const unsigned char *str);
// Modified by Kim_LinHB 2014-5-5
static void Prn_Str_AR(const unsigned char *pucBuffer,unsigned char Position, unsigned char ucType, short step);
static unsigned char IsSingle(char p);
static unsigned short GetStrPix(const unsigned char *ucSource, unsigned int uiSingleWidth, 
						unsigned int uiMultiWidth, unsigned int uiSpaceWidth);
//static void vFormattageLongTPE_PC(long *plNum,const unsigned char * pszNum);
//modify amount(long) to fraction e.g. "1234" -> "12.34"
static void vFormattageAmount(unsigned char *pszAmtIn,unsigned char *pszAmtout);

//for ASC + arabic, then asc characters need to be reversal.
static void vReversalStr(const unsigned char *pszIn,unsigned char *pszOut);

static void Prn_Title_DateTime(void);

// Added by Kim_LinHB 2014-2-5 v4.83
static char IsMixStr(const unsigned char *pStr);

enum
{
	LOGO_STC = 0,
	LOGO_VIVA = 1,
	// Added by Kim_LinHB 2014-3-3
	LOGO_VIRGIN,
	LOGO_FRIENDI,
	// Add End
	// Added by Allen 2015-12-30
	LOGO_JAWWY,  
	LOGO_PSN, //added by jeff_xiehuan20170411
	LOGO_XBOX,
	LOGO_GOOGLE,
	LOGO_ITUNS,
	// Add End
};

enum  //added by jeff_xiehuan20170411
{
	PSN_SUB = 0,
	XBOX_GIFT,
	GOOGLE_PlAY,
	ITUNES_GIFT,
};

static void Prn_Logo(unsigned char ucIndex);

static int RemoveBlank(unsigned char* source, unsigned char* dest);  //added by Allen 2016-11-16

static unsigned char Prn_SaleCodes_SAWA(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);
static unsigned char Prn_SaleCodes_MARHABA(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);
static unsigned char Prn_SaleCodes_Quicknet(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);
static unsigned char Prn_SaleCodes_Viva(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);
// Added by Kim_LinHB 2014-3-3
static unsigned char Prn_SaleCodes_Virgin(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);
static unsigned char Prn_SaleCodes_FRiENDi(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);
// Add End

//Added by Allen 2015-12-30
static unsigned char Prn_SaleCodes_JAWWY(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);
//Add End

static unsigned char Prn_SaleCodes_Others(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode);

//added by Allen 2016-11-16
int RemoveBlank(unsigned char* source, unsigned char* dest)
{
	unsigned char *p, *q;
	if(source == NULL || dest == NULL)
		return -1;
	p = source;
	q = dest;
	while(*p != 0)
	{
		if((*p >= '0') && (*p <= '9'))            //modified by jeff_xiehuan10270418 for QRcode with letters
		{
			*q++ = *p++;
		}
		else
		{
			p++;
		}
	}
	p = NULL;
	q = NULL;
	return 0;
}
//add end

static void Prn_Logo(unsigned char ucIndex)
{
	if (gl_MaintenaceParam.bPrintLogo)
	{
		MyPrnInit();
		if(LOGO_STC == ucIndex)
		{
			PrnLeftIndent((384 - 256) / 2);
			PrnLogo(STC_Logo);
		}
		else if(LOGO_VIVA == ucIndex)
		{
			PrnLeftIndent((384 - 180) / 2);
			PrnLogo(VIVA_Logo);
		}
		// Added by Kim_LinHB 2014-3-3
		else if(LOGO_VIRGIN == ucIndex)
		{
			PrnLeftIndent((384 - 122) / 2);
			PrnLogo(Virgin_Logo);
		}
		else if(LOGO_FRIENDI == ucIndex)
		{
			PrnLeftIndent((384 - 158) / 2);
			PrnLogo(FRiENDi_Logo);
		}else if(LOGO_JAWWY == ucIndex)
		{
			PrnLeftIndent((384 - 200) / 2);
			PrnLogo(JAWWY_Logo);
		}
		else if(LOGO_PSN == ucIndex)
		{
			PrnLeftIndent((384 - 250) / 2);
			PrnLogo(Psn_Logo);
		}
		else if(LOGO_XBOX == ucIndex)
		{
			PrnLeftIndent((384 - 250) / 2);
			PrnLogo(Xbox_Logo);
		}
		else if(LOGO_GOOGLE == ucIndex)
		{
			PrnLeftIndent((384 - 250) / 2);
			PrnLogo(Google_Play_Logo);
		}
		else if(LOGO_ITUNS == ucIndex)
		{
			PrnLeftIndent((384 - 250) / 2);
			PrnLogo(ITunes_Logo);
		}
		// Add End
#ifdef _S_SERIES_
// Added by Kim_LinHB 2013-3-23 v4.56
		sg_ucPrintLineCnt += 10; 
// Modified by Kim_LinHB 2013-4-8 v4.57
		PrintStart(FALSE);
#else
		PrnStart();
		DelayMs(50);
#endif
		// Modify End
		// Add End
	}
	
	//Jump_Lines(1);
}

//////////////////////////////////////////////////////////////////////////
static void Prn_Title_DateTime(void)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szDateTime[SIZE_DATE_TIME + 1];

	sg_bCanCelPrn = FALSE;

	MyPrnInit();

	GetCurTime(szDateTime);
	sprintf((char *)szBufferPrint, "%s%2.2s/%2.2s/%2.2s%15.2s:%2.2s", 
		PRE_YEAR, szDateTime, szDateTime + 2, szDateTime + 4, szDateTime + 6, szDateTime + 8);
	PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
	//Jump_Lines(1);
}

void vTrace(unsigned char *pucLabel, void *pucBuffer, unsigned short usBuffLen, unsigned char ucLv, unsigned char type)
{
	sg_bCanCelPrn = FALSE;
	if(gl_MaintenaceParam.szLogLv[0] - '0'>= ucLv)
	{
		do{
			unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
			unsigned short usIndex = 0;
			PrintBuffer("LOG", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			if(pucLabel != NULL && strlen(pucLabel)> 0)
				PrintBuffer(pucLabel, PRN_LEFT, SIZE_NORMAL,0);

			if(pucBuffer != NULL && usBuffLen > 0 && type != TYPE_S)
			{
				int i;
				while(usBuffLen > 8)
				{
					memset(szBufferPrint,0,sizeof(szBufferPrint));

					for (i = 0; i < 8; ++i)
					{
						PubBcd2Asc(&((unsigned char *)pucBuffer)[i + usIndex], 1, &szBufferPrint[3 * i]);
						szBufferPrint[3 * i + 2]=' ';
					}
					szBufferPrint[23] = 0;
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					usIndex += 8;
					usBuffLen -= 8;
				}

				// impression de la derniere ligne
				// impression de la ligne
				memset(szBufferPrint,0,sizeof(szBufferPrint));

				if(TYPE_B == type)
				{
					for(i = 0; i < usBuffLen; ++i)
					{
						PubBcd2Asc(&((unsigned char *)pucBuffer)[i + usIndex], 1, &szBufferPrint[3 * i]);
						szBufferPrint[3 * i + 2]=' ';
					}
					szBufferPrint[usBuffLen * 3]=0;
				}
				else if(TYPE_N == type)
				{
					if(sizeof(int) == usBuffLen)
					{
						int tmp = *(int *)pucBuffer;
						sprintf(szBufferPrint, "%d", tmp);
					}
					else
					{
						unsigned char tmp = *(unsigned char *)pucBuffer;
						sprintf(szBufferPrint, "%d", tmp);
					}
				}
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
			}
			if(pucBuffer != NULL && TYPE_S == type)  // Added by Kim_LinHB 2013-7-10 v4.62
			{
				PrintBuffer(pucBuffer, PRN_LEFT, SIZE_NORMAL,0);
			}
			Jump_Lines(END_JUMP);
			PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
		}while(sg_bNoPaperCountinue);
	}
}

void Prn_ConnParameter(void)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		//Jump_Lines(2);
		PrintBuffer("CONNECT SETTING",PRN_MIDD, SIZE_NORMAL,0);
		//Jump_Lines(1);
		PrintBuffer("GPRS",PRN_LEFT,SIZE_NORMAL,0);
		Prn_A_LineOf("*");
		sprintf(szBufferPrint, "APN: %s",gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		//Jump_Lines(1);
		PrintBuffer("ETH",PRN_LEFT,SIZE_NORMAL,0);
		Prn_A_LineOf("*");
		if(TRUE == gl_MaintenaceParam.stConnParam.stParamEth.bDHCP)
		{
			PrintBuffer("DHCP: TRUE", PRN_LEFT, SIZE_NORMAL,0);
		}
		else
		{
			PrintBuffer("DHCP: FALSE", PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "Terminal IP: %s",gl_MaintenaceParam.stConnParam.stParamEth.szLocalIP);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "Mask Addr: %s",gl_MaintenaceParam.stConnParam.stParamEth.szNetMask);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "Gateway: %s",gl_MaintenaceParam.stConnParam.stParamEth.szGatewayIP);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "DNS: %s",gl_MaintenaceParam.stConnParam.stParamEth.szDNS);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "DNS 2: %s",gl_MaintenaceParam.stConnParam.stParamEth.szDNS_Bak);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		//Jump_Lines(1);
		PrintBuffer("Server",PRN_LEFT,SIZE_NORMAL,0);
		Prn_A_LineOf("*");
		PrintBuffer("Pay Flex primary host:", PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s: %s",gl_MaintenaceParam.stConnParam.szRemoteIP,
			gl_MaintenaceParam.stConnParam.szRemotePort);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("Pay Flex secondary host:", PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s: %s",gl_MaintenaceParam.stConnParam.szRemoteIP_Bak,
			gl_MaintenaceParam.stConnParam.szRemotePort_Bak);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_PbPlace(void)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_RECHARGEMENT_CODES], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		PrintBuffer(gl_pLangSelPiont[STR_RECHARGEMENT_CODES2], PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_Resp(RESPONSE_HEADER_TAG stResp)
{
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		PrintBuffer(gl_pLangSelPiont[STR_RESPONSE_CODE], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer(stResp.szResponseCode, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		PrintBuffer(gl_pLangSelPiont[STR_ERROR_MSG], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer(stResp.szMessage, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		if(0 == strcmp(stResp.szStatus, "4"))
		{
			//Jump_Lines(1);
			PrintBuffer("Please log in", PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}
		if(0 == strcmp(stResp.szStatus, "5"))
		{
			//Jump_Lines(1);
			PrintBuffer("Please run setting download", PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

unsigned char Prn_SaleCodes(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
	unsigned int i ,len = strlen(stSav.stSavParamCategory[stData.ucCategory_Index].szName);
	int saleIndex;
	
	for(i = 0; i < len; ++i)
		szName[i] = toupper(stSav.stSavParamCategory[stData.ucCategory_Index].szName[i]);
	szName[len] = '\0';

#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,Category=%s",__FILE__,__LINE__,__FUNCTION__,szName);
#endif
#ifndef LHB_TEST
 	if(strstr(szName, "SAWA"))
 		return Prn_SaleCodes_SAWA(stSav, stData, Mode);
 	else if(strstr(szName, "MARHABA"))
 		return Prn_SaleCodes_MARHABA(stSav, stData, Mode);
 	else if(strstr(szName, "QUICKNET"))
 		return Prn_SaleCodes_Quicknet(stSav, stData, Mode);
 	else if(strstr(szName, "VIVA"))
		return Prn_SaleCodes_Viva(stSav, stData, Mode);
 	// Added by Kim_LinHB 2014-3-3
	else if(strstr(szName, "VIRGIN"))
		return Prn_SaleCodes_Virgin(stSav, stData, Mode);
	else if(strstr(szName, "FRIENDI"))
		return Prn_SaleCodes_FRiENDi(stSav, stData, Mode);
	// Add End
	//Added by Allen 2015-12-31
	else if(strstr(szName, "JAWWY"))
	{
		return Prn_SaleCodes_JAWWY(stSav, stData, Mode);
	}
	//And End
	//Added by Allen 2016-03-22
	else if(strstr(szName, "SAPPHIRE"))
	{
		return Prn_SaleCodes_JAWWY(stSav, stData, Mode);
	}
	else if(strstr(szName,"PSN") || strstr(szName,"XBOX") || strstr(szName,"GOOGLE") || strstr(szName,"ITUNES"))
	{
		if(strstr(szName,"PSN"))
		{
			saleIndex = PSN_SUB;
		}
		else if(strstr(szName,"XBOX"))
		{
			saleIndex = XBOX_GIFT;
		}
		else if(strstr(szName,"GOOGLE"))
		{
			saleIndex = GOOGLE_PlAY;
		}
		else if(strstr(szName,"ITUNES"))
		{
			saleIndex = ITUNES_GIFT;
		}
		return Prn_SaleCodes_FourE_Voucher(stSav, stData, Mode,saleIndex);
	}
	//And End
	else
		return Prn_SaleCodes_Others(stSav, stData, Mode);
#else
	// modified by Allen 2015-12-30
	if (strstr(szName, "SAWA"))
	{
		return Prn_SaleCodes_Quicknet(stSav, stData, Mode);
	}
	else
	{
		return Prn_SaleCodes_JAWWY(stSav, stData, Mode);	
	}
#endif
}

static unsigned char Prn_SaleCodes_Others(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	// Added by Kim_LinHB 2013-3-22 v4.56
	unsigned char ucCategory_Index = stData.ucCategory_Index; 
	unsigned char ucProducts_Index = stData.ucProducts_Index;
	// Add End
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	
	unsigned char bPrintedActivation = FALSE;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
			Prn_Logo(LOGO_STC);

		Jump_Lines(2);
		Prn_Title_DateTime();
		Jump_Lines(1);

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(3);

		if(PRN_MERCHANT == Mode)
		{
			if(!stData.bOffline)
				PrintBuffer("ONLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			else
				PrintBuffer("OFFLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			//Jump_Lines(1);
		}
		
		// Modified by Kim_LinHB 2013-3-22 v4.56
		if(PRN_CUSTOMER == Mode)
		{
			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{	
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_NORMAL, 0);
			}
			else
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, 
					PRN_MIDD, SIZE_NORMAL, 0);
			Jump_Lines(1);
			vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szBufferPrint);
			strcat(szBufferPrint, " SR");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(2);
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_CATEGORY], 
				stSav.stSavParamCategory[ucCategory_Index].szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_PRODUCT], 
				stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			
			if(!stData.bOffline)
			{
				sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NUM_TRANS], 
					stData.szProvisionID);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
				sprintf(szBufferPrint, "%s %04d", gl_pLangSelPiont[STR_TRACE_NO], 
					stData.usTraceNo);
			}
			else
				sprintf(szBufferPrint, "%s %04d", "Batch No:", 
					stData.usBatchNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			//Jump_Lines(1);
			
			{
				unsigned char stAmt[SIZE_MAX_AMOUNT + 1];
				vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, stAmt);
				sprintf(szBufferPrint, "%s %s %s", gl_pLangSelPiont[STR_VALEUR], 
					stAmt, "SR");
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			}
		}
		// Modify End

		// Modified by Kim_LinHB 2013-3-21 v4.56
		if(PRN_CUSTOMER == Mode)
		{
			PrintBuffer(gl_pLangSelPiont[STR_ACTIVATION_NUMBER], PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(1);
			Prn_A_LineOf("*");
			Jump_Lines(1);
			vReversalStr(stData.stVoucher.szNum, szBufferPrint); // Added by Kim_LinHB 2013-3-20 v4.56 v4.56
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0);
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;
			Jump_Lines(1);
			Prn_A_LineOf("*");
			Jump_Lines(1);
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_SN], stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}

		sprintf(szBufferPrint, "%s %s%2.2s/%2.2s/%2.2s", gl_pLangSelPiont[STR_EXPIRY_DATE], 
			PRE_YEAR, stData.stVoucher.szExpirDate, stData.stVoucher.szExpirDate + 2, 
			stData.stVoucher.szExpirDate + 4);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		//added by Allen 2016-11-16
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end

		if(PRN_CUSTOMER == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_SN], stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		// Modify End
		// Modified by Kim_LinHB 2013-6-6 v4.59
		else if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);
			
			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0;
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}

		Jump_Lines(3);
		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL,0);

		Jump_Lines(2);

		if(PRN_CUSTOMER == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_CUSTOMER_COPY], PRN_MIDD, SIZE_NORMAL,0);
		else if(PRN_MERCHANT == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY2], PRN_MIDD, SIZE_NORMAL,0);
		
		Jump_Lines(END_JUMP + 3);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}

//added by jeff_xiehuan_20170411
static unsigned char Prn_SaleCodes_FourE_Voucher(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode,int saleIndex)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[40] = {0};
	int i;
	//add end
	// Added by Kim_LinHB 2013-3-22 v4.56
	unsigned char ucCategory_Index = stData.ucCategory_Index; 
	unsigned char ucProducts_Index = stData.ucProducts_Index;
	// Add End
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	
	unsigned char bPrintedActivation = FALSE;
	int voucherLen = 0;
	int iTmpLen = 0;
	
	
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
		{
			switch(saleIndex)
			{
			case PSN_SUB:
				Prn_Logo(LOGO_PSN);
				break;
			case XBOX_GIFT:
				Prn_Logo(LOGO_XBOX);
				break;
			case GOOGLE_PlAY:
				Prn_Logo(LOGO_GOOGLE);
				break;
			case ITUNES_GIFT:
				Prn_Logo(LOGO_ITUNS);
				break;
			default:
				break;
			}
			
		}
			

		Jump_Lines(2);
		if(PRN_MERCHANT == Mode)
		{
			if(!stData.bOffline)
				PrintBuffer("ONLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			else
				PrintBuffer("OFFLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			//Jump_Lines(1);
		}

		for(i = 0;i < 200;i++)
		{
			if(stSav.stSavParamProducts[ucProducts_Index].szNameProduct[i] == '\x0D')
			{
				stSav.stSavParamProducts[ucProducts_Index].szNameProduct[i] = 0x00;
				break;
			}
		}
		// Modified by Kim_LinHB 2013-3-22 v4.56
		if(PRN_CUSTOMER == Mode)
		{
			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{	
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_NORMAL, 0);
			}
			else
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, 
					PRN_MIDD, SIZE_NORMAL, 0);
			Jump_Lines(1);
			vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szBufferPrint);
			strcat(szBufferPrint, " SR");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(2);
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_CATEGORY], 
				stSav.stSavParamCategory[ucCategory_Index].szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_PRODUCT], 
				stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			
			if(!stData.bOffline)
			{
				sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NUM_TRANS], 
					stData.szProvisionID);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
				sprintf(szBufferPrint, "%s %04d", gl_pLangSelPiont[STR_TRACE_NO], 
					stData.usTraceNo);
			}
			else
				sprintf(szBufferPrint, "%s %04d", "Batch No:", 
					stData.usBatchNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			//Jump_Lines(1);
			
			{
				unsigned char stAmt[SIZE_MAX_AMOUNT + 1];
				vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, stAmt);
				sprintf(szBufferPrint, "%s %s %s", gl_pLangSelPiont[STR_VALEUR], 
					stAmt, "SR");
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			}
		}
		// Modify End

		// Modified by Kim_LinHB 2013-3-21 v4.56
		if(PRN_CUSTOMER == Mode)
		{
			PrintBuffer(gl_pLangSelPiont[STR_ACTIVATION_NUMBER], PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(1);
			Prn_A_LineOf("*");
			Jump_Lines(1);
#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s,Before voucher=%s,len=%d",__FILE__,__LINE__,__FUNCTION__,stData.stVoucher.szNum
			,strlen(stData.stVoucher.szNum));
#endif
		voucherLen = strlen(stData.stVoucher.szNum);
		   if(voucherLen <= 17)
		   {
				vReversalStr(stData.stVoucher.szNum, szBufferPrint); // Added by Kim_LinHB 2013-3-20 v4.56 v4.56
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0);
		   }
		   else
		   {
			   while(1)
			   {
				   if(voucherLen > 17)
				   {
				       memset(ucTmp,0,sizeof(ucTmp));
					   memcpy(ucTmp,stData.stVoucher.szNum + iTmpLen,17);
					   vReversalStr(ucTmp, szBufferPrint);
					   PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0);
					   voucherLen -= 17;
					   iTmpLen += 17;
				   }
				   else if(voucherLen > 0 || voucherLen <= 17)
				   {
					   memset(ucTmp,0,sizeof(ucTmp));
					   memcpy(ucTmp,stData.stVoucher.szNum + iTmpLen,voucherLen);
					   vReversalStr(ucTmp, szBufferPrint);
					   PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0);
					   break;
				   }
			   }
		   }
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;
			Jump_Lines(1);
			Prn_A_LineOf("*");
			Jump_Lines(1);
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_SN], stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
#if 1

		PrintBuffer("INSTRUCTIONS",PRN_MIDD,SIZE_NORMAL,0);
		switch(saleIndex)
		{
		case PSN_SUB:
			PrintBuffer("How to redeem the PlayStation Network voucher:",PRN_RIGHT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("1.Sign in to PlayStation",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("Network on your PS4,PS3,",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("PSP or PC.",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("2.Head to PlayStation Store",PRN_LEFT,SIZE_NORMAL,0);
			//PrintBuffer("",PRN_RIGHT,SIZE_NORMAL,0);
			PrintBuffer("and select the Redeem Codes",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("icon,Enter the code you",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("have purchased.",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
#ifdef FARSI
			PrintBuffer("ØÑíÞÉ ÇáÅÓÊÎÏÇã: ",PRN_LEFT,SIZE_NORMAL,-3);
			Jump_Lines(1);
			//PrintBuffer("1- ÊÓÌíá ÇáÏÎæá áÍÓÇÈ",PRN_LEFT,SIZE_NORMAL,-3);
			//PrintBuffer("1- ÊÓÌíá ÇáÏÎæá áÍÓÇÈ",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("1- ÊÓÌíá ÇáÏÎæá áÍÓÇÈ",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("PS4, PS3, PSP, PC",PRN_RIGHT,SIZE_NORMAL,-3);
			PrintBuffer("Playstation_StoreÇáÐåÇÈ Åáì -2",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("RedeemæÅÎÊíÇÑ",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("3-ÃÏÎá ÑÞã ÇáÞÓíãÉ.",PRN_LEFT,SIZE_NORMAL,-3);
			Jump_Lines(1);
#endif 
			break;
		case XBOX_GIFT:
			PrintBuffer("Redeem via the Xbox 360 or",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("Xbox One console:",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("1.Press the Xbox one or Xbox 360 Guide button, and then go to the Marketplace area.",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("2.Select Redeem Code and",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("enter the code you have",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("purchased.",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
#ifdef FARSI
			PrintBuffer("ØÑíÞÉ ÇáÅÓÊÎÏÇã:",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("1-ÇáÖÛØ Úáì ÒÑ ÇáÅÑÔÇÏ Ýí",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("xbox 360 Ãæ xbox one",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("æÇáÐåÇÈ Åáì ÎÇäÉ ÇáÓæÞ.",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("2-ÃÎÊÑ ÊÝÚíá ÇáßæÏ æËã Þã",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("ÅÏÎÇá ÑÞã ÇáÞÓíãÉ.",PRN_LEFT,SIZE_NORMAL,-3);
			Jump_Lines(1);
#endif 
			break;
		case GOOGLE_PlAY:
			PrintBuffer("How to redeem the Google Play voucher:",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("1.Go to play.google.com and",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("click \"Redeem\".",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("2.In the resulting screen,",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("enter your 20-digit code.",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
#ifdef FARSI
			PrintBuffer("ØÑíÞÉ ÇáÅÓÊÎÏÇã:",PRN_LEFT,SIZE_NORMAL,-3);
			Jump_Lines(1);
			PrintBuffer("1-ÇáÐåÇÈ Åáì",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("play.google.com",PRN_RIGHT,SIZE_NORMAL,-3);
			PrintBuffer("RedeemæÇÎÊíÇÑ ",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("2-ÃÏÎá ÇáÑÞã ÇáãæÌæÏ Úáì ÇáÞÓíãÉ",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("Ýí ÇáÎÇäÉ ÇáãÍÏÏÉ.",PRN_LEFT,SIZE_NORMAL,-3);
			Jump_Lines(1);
#endif
		
			break;
		case ITUNES_GIFT:
			PrintBuffer("How to redeem the iTunes Gift voucher on computer:",PRN_RIGHT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("1.In the quick link window of",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("your iTunes program, click",PRN_LEFT,SIZE_NORMAL,0);
			PrintBuffer("Redeem.",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
			PrintBuffer("2.Enter the 16-digit code you have purchased.",PRN_LEFT,SIZE_NORMAL,0);
			Jump_Lines(1);
#ifdef FARSI
			PrintBuffer("ØÑíÞÉ ÇáÅÓÊÎÏÇã: ",PRN_LEFT,SIZE_NORMAL,-3);
			Jump_Lines(1);
			PrintBuffer("iTines Store. 1-ÃÎÊíÇÑ ÊØÈíÞ",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("2-ÇáÖÛØ Úáì",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("Redeem",PRN_RIGHT,SIZE_NORMAL,-3);
			PrintBuffer("æÇÏÎÇá ÑÞã ",PRN_LEFT,SIZE_NORMAL,-3);
			PrintBuffer("ÇáÞÓíãÉ Çáãßæä ãä 16 ÎÇäÉ.",PRN_LEFT,SIZE_NORMAL,-3);
			Jump_Lines(1);
#endif 
			break;
		default:
			break;
		}
#endif

		sprintf(szBufferPrint, "%s %s%2.2s/%2.2s/%2.2s", gl_pLangSelPiont[STR_EXPIRY_DATE], 
			PRE_YEAR, stData.stVoucher.szExpirDate, stData.stVoucher.szExpirDate + 2, 
			stData.stVoucher.szExpirDate + 4);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
	
		//RemoveBlank(stData.stVoucher.szNum, ucTmp);
		/*memset(ucTmp,0,sizeof(ucTmp));      //remove barcode printing
		strcpy(ucTmp,stData.stVoucher.szNum);

		PrnLeftIndent(40);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp),70, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}*/
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end

		if(PRN_CUSTOMER == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_SN], stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		
		// Modify End
		// Modified by Kim_LinHB 2013-6-6 v4.59
		else if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);
			
			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0;
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}
		Jump_Lines(1);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);
		Prn_Title_DateTime();
		Jump_Lines(3);
		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL,0);

		Jump_Lines(2);

		if(PRN_CUSTOMER == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_CUSTOMER_COPY], PRN_MIDD, SIZE_NORMAL,0);
		else if(PRN_MERCHANT == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY2], PRN_MIDD, SIZE_NORMAL,0);
		
		Jump_Lines(END_JUMP + 3);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}
// added by Allen 2016-01-15
static unsigned char Prn_SaleCodes_JAWWY(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	unsigned char ucVoucherNum[52+1];   // added by Allen 2016-03-23
	unsigned char *pVoucher = stData.stVoucher.szNum;   // added by Allen 2016-03-23
	unsigned char ucCategory_Index = stData.ucCategory_Index; 
	unsigned char ucProducts_Index = stData.ucProducts_Index;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	//unsigned char bzBufferPrint_Tmp[SIZE_MAX_POLICE_DEMI + 1 + 1];  //added by Allen 2016/03/18
	
	unsigned char bPrintedActivation = FALSE;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
			Prn_Logo(LOGO_JAWWY);

		Prn_Title_DateTime();
		Jump_Lines(1);
		// Modified by Kim_LinHB 2013-3-22 v4.56
		if(PRN_CUSTOMER == Mode)
		{
			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{	
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_NORMAL, 0);
			}
			else
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, 
					PRN_MIDD, SIZE_NORMAL, 0);
			//Jump_Lines(1);
			vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szBufferPrint);
			strcat(szBufferPrint, " SR");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(1);
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_CATEGORY], 
				stSav.stSavParamCategory[ucCategory_Index].szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_PRODUCT], 
				stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			
			if(!stData.bOffline)
			{
				sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NUM_TRANS], 
					stData.szProvisionID);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
				sprintf(szBufferPrint, "%s %04d", gl_pLangSelPiont[STR_TRACE_NO], 
					stData.usTraceNo);
			}
			else
				sprintf(szBufferPrint, "%s %04d", "Batch No:", 
					stData.usBatchNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			//Jump_Lines(1);
			
			{
				unsigned char stAmt[SIZE_MAX_AMOUNT + 1];
				vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, stAmt);
				sprintf(szBufferPrint, "%s %s %s", gl_pLangSelPiont[STR_VALEUR], 
					stAmt, "SR");
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			}
		}

		if(PRN_CUSTOMER == Mode)
		{
			PrintBuffer(gl_pLangSelPiont[STR_RECHARGE_NNUMBER], PRN_MIDD, SIZE_NORMAL,0);
			PrintBuffer("*****************************************", PRN_MIDD, SIZE_DEMI,0);
			//added by Allen 2016-03-23
			memcpy(ucVoucherNum, pVoucher, 5);
			memcpy(ucVoucherNum + 5, "  ", 2);
			pVoucher = pVoucher + 5;
			memcpy(ucVoucherNum + 7, pVoucher, 5);
			memcpy(ucVoucherNum + 12, "  ", 2);
			pVoucher = pVoucher + 5;
			strcpy(ucVoucherNum + 14, pVoucher);	
			//add end
			//vReversalStr(stData.stVoucher.szNum, szBufferPrint); 
			vReversalStr(ucVoucherNum, szBufferPrint);    //modified by Allen 2016-03-23
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0);
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;
			PrintBuffer("*****************************************", PRN_MIDD, SIZE_DEMI,0);
			// added by Allen 2016-01-15
			//Jump_Lines(1);
			//vReversalStr("Open the Jawwy app, go to My Account", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);
			//vReversalStr("select \"Add Money with a Top Up Card\"", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);		
			//vReversalStr("Enter the 14 digit PIN number under Top-Up", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);			
			//vReversalStr("PIN number and click on \"Add Money now\" ", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);				
			//Jump_Lines(1);

			//vReversalStr("To add credit to another account:", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);		
			//vReversalStr("Click on \"Want to add credit to a ", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);				
			//vReversalStr("different account?\" and enter the ", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);				
			//vReversalStr("phone number associated with the ", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);	
			//vReversalStr("account then click Next.", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);			
			//Jump_Lines(1);

			//vReversalStr("Enter the Top-Up PIN number and", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);
			//vReversalStr("click on Redeem.", szBufferPrint); 
			//PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);
			//Jump_Lines(1);
			//add end

			// added by Allen 2016-01-15
			Jump_Lines(1);
#ifdef FARSI  
			PrintBuffer("áÔÍä ÑÕíÏß¡ ÇÝÊÍ ÊØÈíÞ Ìæøí", PRN_LEFT, SIZE_NORMAL,-3);
			PrintBuffer("æÇÖÛØ Úáì\"ÍÓÇÈí\"¡  æÇßÊÈ ÇáÑÞã ", PRN_LEFT, SIZE_NORMAL,-3);
			//PrintBuffer("Çáãßæä ãä 14 ÎÇäÉ ÇáãßÊæÈÚáì", PRN_LEFT, SIZE_NORMAL,-3);
			PrintBuffer("Çáãßæä ãä 14 ÎÇäÉ ÇáãßÊæÈ", PRN_LEFT, SIZE_NORMAL,-3);
			PrintBuffer("Úáì åÐÇ ÇáÅíÕÇá.", PRN_LEFT, SIZE_NORMAL,-3);
			Jump_Lines(1);
#endif

			vReversalStr("To recharge your line, launch the", szBufferPrint); 
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_NORMAL,-3);  
			vReversalStr("Jawwy app, go to \"My Account\"", szBufferPrint); 
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_NORMAL,-3);		
			vReversalStr("and enter the 14 digits printed", szBufferPrint); 		
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_NORMAL,-3);	
			vReversalStr("above.", szBufferPrint); 		
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_NORMAL,-3);	
			Jump_Lines(1);
			//add end
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_VOUCHER_NUMBER], stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}

		sprintf(szBufferPrint, "%s %17s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		//modified by Allen 2016/03/18
	//	vReversalStr(szBufferPrint, bzBufferPrint_Tmp);              
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		//modify end
		//Jump_Lines(1);
		sprintf(szBufferPrint, "%s %16s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		//modified by Allen 2016/03/18
	//	vReversalStr(szBufferPrint, bzBufferPrint_Tmp);              
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		//modify end
		//Jump_Lines(1);
		sprintf(szBufferPrint, "%s %19s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
		//modified by Allen 2016/03/18
	//	vReversalStr(szBufferPrint, bzBufferPrint_Tmp);              
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		//modify end
		//Jump_Lines(1);

		if(PRN_MERCHANT == Mode)
		{
			if(!stData.bOffline)
				PrintBuffer("ONLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			else
				PrintBuffer("OFFLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			//Jump_Lines(1);
		}

		sprintf(szBufferPrint, "%s %8.2s%2.2s/%2.2s/%2.2s", gl_pLangSelPiont[STR_EXPIRY_DATE], 
			PRE_YEAR, stData.stVoucher.szExpirDate, stData.stVoucher.szExpirDate + 2, 
			stData.stVoucher.szExpirDate + 4);
		//modified by Allen 2016/03/18
	//	vReversalStr(szBufferPrint, bzBufferPrint_Tmp);              
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		//modify end
		//Jump_Lines(1);

		//added by Allen 2016-11-16
		Jump_Lines(1);
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		PrnLeftIndent(0);
		Jump_Lines(1);
		//add end

		if(PRN_CUSTOMER == Mode)
		{
			sprintf(szBufferPrint, "%s %13s", gl_pLangSelPiont[STR_VOUCHER_NUMBER], stData.stVoucher.szSerial);
			//modified by Allen 2016/03/18
		//	vReversalStr(szBufferPrint, bzBufferPrint_Tmp);              
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			//modify end
		}
		// Modify End
		// Modified by Kim_LinHB 2013-6-6 v4.59
		else if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);
			
			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0;
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}

		Jump_Lines(1);
		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			Jump_Lines(1);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL,0);

		Jump_Lines(1);

		if(PRN_CUSTOMER == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_CUSTOMER_COPY], PRN_MIDD, SIZE_NORMAL,0);
		else if(PRN_MERCHANT == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY2], PRN_MIDD, SIZE_NORMAL,0);
		
		Jump_Lines(END_JUMP + 3);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}


static unsigned char Prn_SaleCodes_SAWA(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	// Added by Kim_LinHB 2013-3-21 v4.56
	unsigned char ucCategory_Index = stData.ucCategory_Index;
	unsigned char ucProducts_Index = stData.ucProducts_Index; 
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1];
	// Add End
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	
	unsigned char bPrintedActivation = FALSE;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
			Prn_Logo(LOGO_STC);
		//Jump_Lines(1);

		if(PRN_MERCHANT == Mode)
		{
			unsigned char szDateTime[SIZE_DATE_TIME + 1];
			GetCurTime(szDateTime);
			sprintf(szBufferPrint, "Date: %2.2s/%2.2s/%s%2.2s  %2.2s:%2.2s", 
				szDateTime + 4, szDateTime + 2, PRE_YEAR, szDateTime, szDateTime + 6, szDateTime + 8);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			//Jump_Lines(1);
// 			Prn_A_LineOf("*");
// 			//PrintBuffer("Duplicate", PRN_MIDD, SIZE_NORMAL_BOLD,0);
// 			PrintBuffer_AR("Duplicate", PRN_MIDD, SIZE_NORMAL_BOLD,0);
// 			Prn_A_LineOf("*");
		}

		// Modified by Kim_LinHB 2013-3-25 v4.56
		vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szAmt);
		if(PRN_CUSTOMER == Mode)
		{
			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 29)
			{
				sprintf(szBufferPrint, "%29.29s", 
					stSav.stSavParamCategory[ucCategory_Index].szName_AR);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI_BOLD,-5);
				PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR + 29, PRN_MIDD, SIZE_DEMI_BOLD,0);
			}
			else
				PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR, PRN_MIDD, SIZE_DEMI_BOLD,0);

			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName) > 29)
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				sprintf(szBufferPrint, "%29.29s", szName); // Modified by Kim_LinHB 2014-2-5 v4.83
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI_BOLD,0);
				PrintBuffer_AR(szName + 29, PRN_MIDD, SIZE_DEMI_BOLD,0);
			}
			else
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				PrintBuffer_AR(szName, PRN_MIDD, SIZE_DEMI_BOLD,0);
			}
			
			{ // Modified by Kim_LinHB 2014-1-2 v4.79
				char *pDot = strchr(szAmt, '.');
				if(pDot && 0 == atoi(pDot + 1))
				{
					*pDot = 0;
				}
				sprintf(szBufferPrint, "%s %s", "SR", szAmt);
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0); // Modified by Kim_LinHB 2014/11/4
			}
			//Jump_Lines(1);
		}
		else
		{
#ifdef FARSI
			sprintf(szBufferPrint, "%s%40s", "Network:", gl_pStr_Arabic[STR_Op_Min]);
#else
			sprintf(szBufferPrint, "%s", "Network:");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 0)
			{
				if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 29)
				{
					sprintf(szBufferPrint, "%29.29s", 
						stSav.stSavParamCategory[ucCategory_Index].szName_AR);
					PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
					PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR + 29, PRN_MIDD, SIZE_NORMAL,0);
				}
				else
					PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR, PRN_MIDD, SIZE_NORMAL,0);
			}
			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName) > 29)
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				sprintf(szBufferPrint, "%29.29s", szName); // Modified by Kim_LinHB 2014-2-5 v4.83
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
				PrintBuffer_AR(szName + 29, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}
			else
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				PrintBuffer_AR(szName, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}

#ifdef FARSI
			sprintf(szBufferPrint, "%s%34s", "Product name:", "äæÚ ÇáãäÊÌ");
#else
			sprintf(szBufferPrint, "%s", "Product name:");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
			
			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_NORMAL, 0);
			}
			else
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, PRN_MIDD, SIZE_NORMAL,0);
		}
		// Modify End

		if(PRN_CUSTOMER == Mode)
		{

#ifdef FARSI
			sprintf(szBufferPrint, "%s%26s","Activation Number", "ÑÞã ÇáÊÝÚíá");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
		
			PrintBuffer("*****************************************", PRN_MIDD, SIZE_DEMI,0);

			vReversalStr(stData.stVoucher.szNum, szBufferPrint); // Added by Kim_LinHB 2013-3-20 v4.56 v4.56
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0); // Modified by Kim_LinHB 2014/11/4
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;

			PrintBuffer("*****************************************", PRN_MIDD, SIZE_DEMI,0);

			sprintf(szBufferPrint, "%s%30s", "INSTRUCTIONS", "ÇáÊÚáíãÇÊ");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,-5);

			PrintBuffer("áÔÍä ÇáÈØÇÞÉ", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PrintBuffer("\"ÇÖÛØ ãä ÇáíÓÇÑ ááíãíä\"", PRN_LEFT, SIZE_DEMI,0);
			PrintBuffer("ÇÖÛØ ÅÊÕÇá #ÑÞã ÇáåæíÉ*ÑÞã ÇáÊÝÚíá*155*", PRN_LEFT, SIZE_DEMI,-5);

			PrintBuffer("áãÚÑÝÉ ÇáÑÕíÏ:", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PrintBuffer("\"ÇÖÛØ ãä ÇáíÓÇÑ ááíãíä\"", PRN_LEFT, SIZE_DEMI,0);
			PrintBuffer("ÇÖÛØ ÅÊÕÇá #166*", PRN_LEFT, SIZE_DEMI,0);
			PrintBuffer("áãÒíÏ ãä ÇáãÚáæãÇÊ ÇáÑÌÇÁ ÅÊÕá Úáí 1500", PRN_LEFT, SIZE_DEMI,-5);

#endif
			//PrintBuffer("Recharge:", PRN_LEFT, SIZE_LARGE_BOLD,0);
			// Modified by Kim_LinHB 2014-1-7 v4.79 "To Recharge"
			PrintBuffer_AR(":Recharge To", PRN_RIGHT, SIZE_DEMI_BOLD,-5);
			PrintBuffer("*155*activation Number*ID or Iqama No.# Press Call", PRN_LEFT, SIZE_NORMAL,0);
			//PrintBuffer("Balance:", PRN_LEFT, SIZE_LARGE_BOLD,0);
			PrintBuffer_AR(":Balance", PRN_RIGHT, SIZE_DEMI_BOLD,-5);
			PrintBuffer("*166# Dail\nFor more Info Call 1500", PRN_LEFT, SIZE_NORMAL,0);
		}

#ifdef FARSI
		sprintf(szBufferPrint, "%s%32s", "Expiration date", "ÊÇÑíÎ ÇáÕáÇÍíÉ");
#else
		strcpy(szBufferPrint, "%s", "Expiration date");
#endif
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

		sprintf(szBufferPrint, "%2.2s/%2.2s/%s%2.2s", 
				stData.stVoucher.szExpirDate + 4, stData.stVoucher.szExpirDate + 2,
				PRE_YEAR, stData.stVoucher.szExpirDate);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
		//added by Allen 2016-11-16
		Jump_Lines(1);
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end

#ifdef FARSI
		sprintf(szBufferPrint, "%s%32s", "Serial Number", "ÇáÑÞã ÇáÊÓáÓáí");
#else 
		sprintf(szBufferPrint, "%s","Serial Number");
#endif
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
		PrintBuffer(stData.stVoucher.szSerial, PRN_MIDD, SIZE_NORMAL_BOLD,0);

		// Added by Kim_LinHB 2014-1-2 v4.79
		// Modified by Kim_LinHB 2014-4-29
		if(PRN_CUSTOMER == Mode)
		{
			sprintf(szBufferPrint, "%s%*s", gl_pLangSelPiont [STR_ID_TERMINAL],  29 - strlen(gl_pLangSelPiont [STR_ID_TERMINAL]), gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}

		// Added by Kim_LinHB 2013-6-6 v4.59
		if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);

			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0; 
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}

		sprintf(szBufferPrint, "%2.2s/%2.2s/%s%2.2s%15.2s:%2.2s", 
			stData.szDateTime + 4, stData.szDateTime + 2, PRE_YEAR, stData.szDateTime,
			stData.szDateTime + 6, stData.szDateTime + 8);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
		

		//PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		//PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		//vReversalStr("INNOVA " APP_NAME, szBufferPrint);
		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			Jump_Lines(2);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end
		// Modified by Kim_LinHB 2014-1-7 v4.80
		vReversalStr("Powered by INNOVA", szBufferPrint);
		PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI_BOLD,0);

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}

static unsigned char Prn_SaleCodes_MARHABA(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	// Added by Kim_LinHB 2013-3-22 v4.56
	unsigned char ucCategory_Index = stData.ucCategory_Index;
	unsigned char ucProducts_Index = stData.ucProducts_Index;
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1]; 
	// Add End
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	unsigned char bPrintedActivation = FALSE;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
			Prn_Logo(LOGO_STC);
		Jump_Lines(2);

		if(PRN_MERCHANT == Mode)
		{
			unsigned char szDateTime[SIZE_DATE_TIME + 1];
			GetCurTime(szDateTime);
			sprintf(szBufferPrint, "Date: %2.2s/%2.2s/%s%2.2s  %2.2s:%2.2s", 
				szDateTime + 4, szDateTime + 2, PRE_YEAR, szDateTime, szDateTime + 6, szDateTime + 8);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			//Jump_Lines(1);
// 			Prn_A_LineOf("*");
// 			//PrintBuffer("Duplicate", PRN_MIDD, SIZE_NORMAL_BOLD,0);
// 			PrintBuffer_AR("Duplicate", PRN_MIDD, SIZE_NORMAL_BOLD,0);
// 			Prn_A_LineOf("*");
		}

		// Modified by Kim_LinHB 2013-3-22 v4.56
		vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szAmt);
		
		if(PRN_CUSTOMER == Mode)
		{
			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 29)
			{
				sprintf(szBufferPrint, "%29.29s", 
					stSav.stSavParamCategory[ucCategory_Index].szName_AR);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
				PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR + 29, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}
			else
				PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			
			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName) > 29)
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				sprintf(szBufferPrint, "%29.29s", szName); // Modified by Kim_LinHB 2014-2-5 v4.83
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
				PrintBuffer_AR(szName + 29, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}
			else
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				PrintBuffer_AR(szName, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}
			
#ifdef FARSI
			sprintf(szBufferPrint, "%s%26s", "Lowest Prices For", "ãßÇáãÇÊ ÏæáíÉ");
#else
			sprintf(szBufferPrint, "%s", "Lowest Prices For");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

#ifdef FARSI
			sprintf(szBufferPrint, "%s%26s", "International Calls", "ÈÃÑÎÕ ÇáÃÓÚÇÑ");
#else
			sprintf(szBufferPrint, "%s", "International Calls");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

			sprintf(szBufferPrint, "%s %s", "SR", szAmt);
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}
		else
		{
#ifdef FARSI
			sprintf(szBufferPrint, "%s%40s", "Network:", gl_pStr_Arabic[STR_Op_Min]);
#else
			sprintf(szBufferPrint, "%s", ":Network:");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 0)
			{
				if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 29)
				{
					sprintf(szBufferPrint, "%29.29s", 
						stSav.stSavParamCategory[ucCategory_Index].szName_AR);
					PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
					PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR + 29, PRN_MIDD, SIZE_NORMAL_BOLD,0);
				}
				else
					PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}
			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName) > 29)
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				sprintf(szBufferPrint, "%29.29s", szName); // Modified by Kim_LinHB 2014-2-5 v4.83
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
				PrintBuffer_AR(szName + 29, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}
			else
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				PrintBuffer_AR(szName, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}

#ifdef FARSI
			sprintf(szBufferPrint, "%s%34s", "Product name:", "äæÚ ÇáãäÊÌ");
#else
			sprintf(szBufferPrint, "%s", "Product name:");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_NORMAL, 0);
			}else
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, PRN_MIDD, SIZE_NORMAL,0);
		}
		// Modify End

		if(PRN_CUSTOMER == Mode)
		{

#ifdef FARSI
			sprintf(szBufferPrint, "%s%26s", "Card Number", "ÑÞã ÇáÊÝÚíá");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

			Prn_A_LineOf("*");

			vReversalStr(stData.stVoucher.szNum, szBufferPrint); // Added by Kim_LinHB 2013-3-20 v4.56
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0);
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;
			
			Prn_A_LineOf("*");

			sprintf(szBufferPrint, "%s%30s", "Instructions", "ÇáÊÚáíãÇÊ");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

			sprintf(szBufferPrint, "%s%20s", "To make a Call dail", "áÅÌÑÇÁ ãßÇáãÉ ÇÊÕá");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
			PrintBuffer_AR("8007008000", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			//Jump_Lines(1);

			PrintBuffer("ááÏÚã ÇáÝäí ÇáÑÌÇÁ ÇáÇÊÕÇá ÈÜ", PRN_LEFT, SIZE_DEMI,0);
			PrintBuffer("For Technical Support call", PRN_LEFT, SIZE_NORMAL,0);
			PrintBuffer_AR("907", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
#endif
		}

#ifdef FARSI
		sprintf(szBufferPrint, "%s%32s",  "Expiration date", "ÊÇÑíÎ ÇáÕáÇÍíÉ");
#else
		sprintf(szBufferPrint, "%s", "Expiration date");
#endif
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

		sprintf(szBufferPrint, "%2.2s/%2.2s/%s%2.2s", 
				stData.stVoucher.szExpirDate + 4, stData.stVoucher.szExpirDate + 2,
				PRE_YEAR, stData.stVoucher.szExpirDate);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
		//added by Allen 2016-11-16
		Jump_Lines(1);
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end		

#ifdef FARSI
		sprintf(szBufferPrint, "%s%32s", "Serial Number", "ÇáÑÞã ÇáÊÓáÓáí");
#else
		sprintf(szBufferPrint, "%s", "Serial Number");
#endif
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
		PrintBuffer(stData.stVoucher.szSerial, PRN_MIDD, SIZE_NORMAL,0);

		// Added by Kim_LinHB 2013-6-6 v4.59
		if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);

			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0; 
			}
			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0; 
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0; 
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}

		// Added by Kim_LinHB 2014-4-29
		if(PRN_CUSTOMER == Mode)
		{
			sprintf(szBufferPrint, "%s%*s", gl_pLangSelPiont [STR_ID_TERMINAL],  29 - strlen(gl_pLangSelPiont [STR_ID_TERMINAL]), gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}

		sprintf(szBufferPrint, "%2.2s/%2.2s/%s%2.2s%15.2s:%2.2s",
			stData.szDateTime + 4, stData.szDateTime + 2, PRE_YEAR, stData.szDateTime,
			stData.szDateTime + 6, stData.szDateTime + 8);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			Jump_Lines(2);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		vReversalStr("INNOVA " APP_NAME, szBufferPrint);
		PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);

		PrnStep(5);
		Jump_Lines(END_JUMP + 1);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}

// Added by Kim_LinHB 2013-4-2 v4.57
// Modified by Kim_LinHB 2014-4-30
// Modified by Kim_LinHB 2014-5-5
static unsigned char Prn_SaleCodes_Quicknet(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	unsigned char ucCategory_Index = stData.ucCategory_Index;
	unsigned char ucProducts_Index = stData.ucProducts_Index; 
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1];
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	unsigned char bPrintedActivation = FALSE;

#ifdef _S_SERIES_
	const short step = -5;
#else
	const short step = -9;
#endif

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
		{
			Prn_Logo(LOGO_STC);
		}

		if(PRN_MERCHANT == Mode)
		{
			unsigned char szDateTime[SIZE_DATE_TIME + 1];
			GetCurTime(szDateTime);
			sprintf(szBufferPrint, "Date: %2.2s/%2.2s/%s%2.2s  %2.2s:%2.2s", 
				szDateTime + 4, szDateTime + 2, PRE_YEAR, szDateTime, szDateTime + 6, szDateTime + 8);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			//Jump_Lines(1);
 			Prn_A_LineOf("*");
 			PrintBuffer_AR("Duplicate", PRN_MIDD, SIZE_NORMAL_BOLD,0);
 			Prn_A_LineOf("*");
		}

		vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szAmt);
		if(PRN_CUSTOMER == Mode)
		{
			unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
			vReversalStr("QUICKnet Express", szName);
			PrintBuffer_AR(szName, PRN_MIDD, SIZE_DEMI,step);

			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,step);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_DEMI, step);
			}
			else
			{
				vReversalStr(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, szName);
				PrintBuffer_AR(szName, PRN_MIDD, SIZE_DEMI,step);
			}
			PrintBuffer_AR("ßæíß äÊ ÇßÓÈÑÓ", PRN_MIDD, SIZE_DEMI_BOLD,step);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szNameProduct_AR, PRN_MIDD, SIZE_DEMI_BOLD,step);

			sprintf(szBufferPrint, "%s %s", "SR", szAmt);
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI,step);
		}
		else
		{
#ifdef FARSI
			sprintf(szBufferPrint, "%s %42s", "Network:", gl_pStr_Arabic[STR_Op_Min]);
#else
			sprintf(szBufferPrint, "%s", "Network:");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 0)
			{
				if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName_AR) > 29)
				{
					sprintf(szBufferPrint, "%29.29s", 
						stSav.stSavParamCategory[ucCategory_Index].szName_AR);
					PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
					PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR + 29, PRN_MIDD, SIZE_NORMAL,0);
				}
				else
					PrintBuffer(stSav.stSavParamCategory[ucCategory_Index].szName_AR, PRN_MIDD, SIZE_NORMAL,0);
			}
			if(strlen(stSav.stSavParamCategory[ucCategory_Index].szName) > 29)
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				sprintf(szBufferPrint, "%29.29s", szName); // Modified by Kim_LinHB 2014-2-5 v4.83
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);
				PrintBuffer_AR(szName + 29, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}
			else
			{
				unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
				vReversalStr(stSav.stSavParamCategory[ucCategory_Index].szName, szName);
				PrintBuffer_AR(szName, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			}

#ifdef FARSI
			sprintf(szBufferPrint, "%s%34s", "Product name:", "äæÚ ÇáãäÊÌ");
#else
			sprintf(szBufferPrint, "%s", "Product name:");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_NORMAL, 0);
			}
			else
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, PRN_MIDD, SIZE_NORMAL,0);
		}

		if(PRN_CUSTOMER == Mode)
		{
#ifdef FARSI
			sprintf(szBufferPrint, "%s%26s", "Activation Number", "ÑÞã ÇáÊÝÚíá");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,step);

			//Prn_A_LineOf("*");
			PrintBuffer("*****************************************", PRN_MIDD, SIZE_DEMI,/*-3*/0);   // modified by Allen 2016-01-27
			vReversalStr(stData.stVoucher.szNum, szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,/*-6*/0);   // modified by Allen 2016-01-27
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;
			PrintBuffer("*****************************************", PRN_MIDD, SIZE_DEMI,/*-3*/0);  // modified by Allen 2016-01-27
			//Prn_A_LineOf("*");

			PrintBuffer("ÚÒíÒí ÇáÚãíá :", PRN_LEFT, SIZE_DEMI_BOLD,step);
			PrintBuffer("íãßäß ÇÚÇÏÉ ÔÍä ÑÕíÏ ÔÑíÍÉ", PRN_LEFT, SIZE_DEMI_BOLD,step);
			PrintBuffer("ÇáÇäÊÑäÊ \"ßæíß äÊ\" ÚÈÑ :", PRN_LEFT, SIZE_DEMI_BOLD,step);

			PrintBuffer("- (ÇÑÓÇá ÑÓÇáÉ äÕíÉ Çáì 1500 ÊÍÊæí Úáì", PRN_LEFT, SIZE_DEMI,step);
			PrintBuffer("#ÑÞã ÇáåæíÉ*ÑÞã ÇÚÇÏÉ ÇáÔÍä*155*)", PRN_LEFT, SIZE_DEMI,step);
			PrintBuffer("(ãä ÇáíÓÇÑ Çáì Çáíãíä )", PRN_LEFT, SIZE_DEMI,step);
			PrintBuffer("-  www.aljawal.net.sa ÒíÇÑÉ ÈæÇÈÉ ÇáÌæÇá", PRN_LEFT, SIZE_DEMI,step);
			PrintBuffer("- my.stc.com.sa ÒíÇÑÉ ÈæÇÈÉ ÎÏãÇÊí", PRN_LEFT, SIZE_DEMI,step);
			PrintBuffer("ãæÏã ßæíß äÊ ", PRN_LEFT, SIZE_DEMI,step);

#endif

			vReversalStr("Dear Customer:", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI_BOLD,step);
			vReversalStr("To recharge your \"QUICKnet\"", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI,step);
			vReversalStr("Internet Sim card:", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI,step);
			vReversalStr("-Send a SMS to 1500 including", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI,step);
			vReversalStr(" *155*Voucher Number*ID No.#", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI,step);
			vReversalStr("-Visit the STC portal www.aljawal.net.sa", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI,step);
			vReversalStr("-Go to My STC at my.stc.com.sa", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI,step);
			vReversalStr("-Use QUICKnet modem", szBufferPrint);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_DEMI,step);
		}

#ifdef FARSI
		sprintf(szBufferPrint, "%s%32s", "Expiration date", "ÊÇÑíÎ ÇáÕáÇÍíÉ");
#else
		sprintf(szBufferPrint, "%s" ,"Expiration date");
#endif
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI, step);

		sprintf(szBufferPrint, "%2.2s/%2.2s/%s%2.2s", 
			stData.stVoucher.szExpirDate + 4, stData.stVoucher.szExpirDate + 2,
			PRE_YEAR, stData.stVoucher.szExpirDate);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,-3);
		//added by Allen 2016-11-16
		Jump_Lines(1);
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end
#ifdef FARSI
		sprintf(szBufferPrint, "%s%32s", "Serial Number", "ÇáÑÞã ÇáÊÓáÓáí");
#else
		sprintf(szBufferPrint, "%s", "Serial Number");
#endif
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,step);
		PrintBuffer(stData.stVoucher.szSerial, PRN_MIDD, SIZE_DEMI,-3);

		// Added by Kim_LinHB 2013-6-6 v4.59
		if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);

			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0;
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0; 
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}

		// Added by Kim_LinHB 2014-4-29
		if(PRN_CUSTOMER == Mode)
		{
			unsigned char szTmp[50];
			sprintf(szBufferPrint, "%s%*s", gl_pLangSelPiont [STR_ID_TERMINAL],  39 - strlen(gl_pLangSelPiont [STR_ID_TERMINAL]), gl_MaintenaceParam.szTermID);
			vReversalStr(szBufferPrint, szTmp);
			PrintBuffer_AR(szTmp, PRN_RIGHT, SIZE_DEMI_BOLD,step);
		}

		sprintf(szBufferPrint, "%2.2s/%2.2s/%s%2.2s%20.2s:%2.2s", 
			stData.szDateTime + 4, stData.szDateTime + 2, PRE_YEAR, stData.szDateTime,
			stData.szDateTime + 6, stData.szDateTime + 8);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			Jump_Lines(2);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_DEMI,0);
		//vReversalStr("INNOVA " APP_NAME, szBufferPrint);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL,0);

#ifdef _S_SERIES_
		PrnStep(5);
		Jump_Lines(END_JUMP);
#else
		Jump_Lines(END_JUMP -2);
#endif
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}
// Add End

// Added by Kim_LinHB 2013-6-19 v4.61
static unsigned char Prn_SaleCodes_Viva(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	unsigned char ucCategory_Index = stData.ucCategory_Index;
	unsigned char ucProducts_Index = stData.ucProducts_Index; 
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1];
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char bPrintedActivation = FALSE;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		//if(PRN_CUSTOMER == Mode)
		{
			Prn_Logo(LOGO_VIVA);
			Jump_Lines(1);
		}

		if(PRN_MERCHANT == Mode)
		{
			unsigned char szDateTime[SIZE_DATE_TIME + 1];
			Jump_Lines(5);
			GetCurTime(szDateTime);
			sprintf(szBufferPrint, "Date: %2.2s/%2.2s/%s%2.2s  %2.2s:%2.2s", 
				szDateTime + 4, szDateTime + 2, PRE_YEAR, szDateTime, szDateTime + 6, szDateTime + 8);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			Jump_Lines(3);
		}

		vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szAmt);
	//	if(PRN_CUSTOMER == Mode)
		{
			unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];
			Jump_Lines(2);
			vReversalStr("VIVA EXPRESS", szName);
			PrintBuffer_AR(szName, PRN_MIDD, SIZE_NORMAL_BOLD,0);

			if(strlen(stSav.stSavParamProducts[ucProducts_Index].szNameProduct) > 29)
			{
				sprintf(szBufferPrint, "%29.29s", stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
				PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct + 29, 
					PRN_MIDD, SIZE_NORMAL, 0);
			}
			else
				PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szNameProduct, PRN_MIDD, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "%s %s", "SR", szAmt);
			PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_DEMI_BOLD,0);
			
			if(PRN_CUSTOMER == Mode)
			{
				Jump_Lines(2);
				PrintBuffer("This is your PIN Number:", PRN_MIDD, SIZE_NORMAL,0);
				Jump_Lines(2);
				//strcpy(stData.stVoucher.szNum, "123453"); //for test
#ifdef FARSI
				vReversalStr(stData.stVoucher.szNum, szBufferPrint);
				PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_LARGE_BOLD,0);
#else
				PrintBuffer(stData.stVoucher.szNum, PRN_MIDD, SIZE_LARGE_BOLD,0);
#endif
				bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;

				Jump_Lines(1);
				PrintBuffer_AR("Only Recharge for Voucher This", PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer_AR("Card Recharge Prepaid", PRN_MIDD, SIZE_NORMAL,0);
				PrintBuffer_AR("VIVA", PRN_MIDD, SIZE_LARGE_BOLD,0);
			}
			Jump_Lines(1);
		}

		//strcpy(gl_MaintenaceParam_Ext.szHeader1, "shdfidsojfi"); //for test
		PrintBuffer("Merchant name:", PRN_LEFT, SIZE_NORMAL,0);
		if(strlen(gl_MaintenaceParam_Ext.szHeader1) > 0)
			PrintBuffer(gl_MaintenaceParam_Ext.szHeader1, PRN_MIDD, SIZE_NORMAL,0);
		else
			Jump_Lines(1);

		//strcpy(gl_MaintenaceParam_Ext.szHeader2, "asdsad"); //for test
		PrintBuffer("Location:", PRN_LEFT, SIZE_NORMAL,0);
		if(strlen(gl_MaintenaceParam_Ext.szHeader2) > 0)
		PrintBuffer(gl_MaintenaceParam_Ext.szHeader2, PRN_MIDD, SIZE_NORMAL,0);
		else
			Jump_Lines(1);

		//strcpy(gl_MaintenaceParam_Ext.szHeader3, "123123321"); //for test
		sprintf(szBufferPrint, "%s %s", "Phone no:", gl_MaintenaceParam_Ext.szHeader3);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		sprintf(szBufferPrint, "%s %s", "TID:", gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		if(!stData.bOffline)
		{
			sprintf(szBufferPrint, "%s %s", "Trans:", 
				stData.szProvisionID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		else
		{
			sprintf(szBufferPrint, "%s %s %s", "Trans:", 
				stData.stVoucher.szVoucherId, "(Offline)");
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}

		sprintf(szBufferPrint, "%s %s", "Serial:", stData.stVoucher.szSerial);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		sprintf(szBufferPrint, "%s%2.2s/%2.2s/%s%2.2s%7s%2.2s:%2.2s", 
			"Date:", stData.szDateTime + 4, stData.szDateTime + 2, PRE_YEAR, stData.szDateTime,
			"Time:",stData.szDateTime + 6, stData.szDateTime + 8);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		sprintf(szBufferPrint, "Expiry date: %2.2s/%2.2s/%s%2.2s", 
			stData.stVoucher.szExpirDate + 4, stData.stVoucher.szExpirDate + 2,
			PRE_YEAR, stData.stVoucher.szExpirDate);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		//added by Allen 2016-11-16
		Jump_Lines(1);
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end

		sprintf(szBufferPrint, "%s %s", "Customer Care No:", "124");
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		
		// Added by Kim_LinHB 2013-6-6 v4.59
		if(PRN_MERCHANT == Mode)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(2);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);

			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0;
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			if(!stData.bOffline)
			{
				PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
				vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
				sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
				PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			}
			else
				Jump_Lines(3);
		}	

		Jump_Lines(1);
		if(PRN_CUSTOMER == Mode)
		{
			//Jump_Lines(2);
			// Added by Kim_LinHB 2014-4-29
			sprintf(szBufferPrint, "%s%*s", gl_pLangSelPiont [STR_ID_TERMINAL],  29 - strlen(gl_pLangSelPiont [STR_ID_TERMINAL]), gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "%2.2s/%2.2s/%s%2.2s%15.2s:%2.2s", 
				stData.szDateTime + 4, stData.szDateTime + 2, PRE_YEAR, stData.szDateTime,
				stData.szDateTime + 6, stData.szDateTime + 8);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(1);
		}

		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			Jump_Lines(2);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end
		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_MIDD, SIZE_NORMAL,0);
		vReversalStr("INNOVA " APP_NAME, szBufferPrint);
		PrintBuffer_AR(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD,0);

		Jump_Lines(1);
		if(PRN_MERCHANT == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY2], PRN_MIDD, SIZE_NORMAL,0);

		//Jump_Lines(6);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}
// Add End

// Added by Kim_LinHB 2014-3-3
// Modified by Kim_LinHB 2014-08-14 v4.95
// Modified by Kim_LinHB 2014-08-15
static unsigned char Prn_SaleCodes_Virgin(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	unsigned char ucCategory_Index = stData.ucCategory_Index;
	unsigned char ucProducts_Index = stData.ucProducts_Index;
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1]; 

	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	unsigned char bPrintedActivation = FALSE;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
		{
			Prn_Logo(LOGO_VIRGIN); // Modified by Kim_LinHB 2014/9/12 v4.96 new logo
			Jump_Lines(1);
		}


		if(PRN_CUSTOMER == Mode)
		{
			vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szAmt);
			if (0 == atoi(&szAmt[strlen(szAmt) - 2]))
			{
				szAmt[strlen(szAmt) - 3] = 0;
			}
			sprintf(szBufferPrint, "%.12s %s", szAmt, "SAR");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_LARGE,0);
		}
		else if(PRN_MERCHANT == Mode)
		{
			Prn_Title_DateTime();
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		Jump_Lines(1);

		if(PRN_MERCHANT == Mode)
		{
			if(!stData.bOffline)
				PrintBuffer("ONLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			else
				PrintBuffer("OFFLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}

		if(PRN_CUSTOMER == Mode)
		{
			unsigned char szBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
			unsigned char szDateTime[SIZE_DATE_TIME + 1];

			Prn_A_LineOf("-");
#ifdef FARSI
			sprintf(szBufferPrint, "%s%31s", "To Top-up enter:", "áÅÚÇÏÉ ÇáÔÍä:"); // Modified by Kim_LinHB 2014/11/26
#else
			sprintf(szBufferPrint, "%s", "To Top-up enter:");
#endif
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);

			vReversalStr(stData.stVoucher.szNum, szBuff);
			sprintf(szBufferPrint, "#%s*101*   ", szBuff);
			SetPrintMode(0);
			PrnLeftIndent(261);
			PrnLogo(TeleIcon2);
			PrnStep(-25);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			SetPrintMode(1);
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;
			//Jump_Lines(1);  //removed by Allen 2016-11-25
			Prn_A_LineOf("-");
			Jump_Lines(1);

			sprintf(szBufferPrint, "%-46s#102*", "áãÚÑÝÉ ÇáÑÕíÏ:");
			SetPrintMode(0);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_DEMI,0);
			PrnStep(-30);
			PrnLeftIndent(50);
			PrnLogo(TeleIcon);
			SetPrintMode(1);

			sprintf(szBufferPrint, "%-47s#112*", "áÅÎÊíÇÑ ÈÇÞÉ ÇáÈíÇäÇÊ:");
			SetPrintMode(0);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_DEMI,0);
			PrnStep(-30);
			PrnLeftIndent(50);
			PrnLogo(TeleIcon);
			SetPrintMode(1);
			PrnStep(5);

			sprintf(szBufferPrint, "%-*s*102#", 27 - 5, "Check your balance:");
			SetPrintMode(0);
			PrnLeftIndent(285);//300
			PrnLogo(TeleIcon2);
			PrnStep(-25);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			SetPrintMode(1);

			sprintf(szBufferPrint, "%-*s*112#", 27 - 5, "Choose a data package:");
			SetPrintMode(0);
			PrnLeftIndent(285);//300
			PrnLogo(TeleIcon2);
			PrnStep(-25);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			SetPrintMode(1);


			Prn_A_LineOf("-");
			PrintBuffer_AR(":us with Connect", PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			PrnStep(5);

			sprintf(szBufferPrint, "Call member care:%*s", 29-strlen("Call member care:"), "1789"); // Modified by Kim_LinHB 2014/11/17
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "Facebook:%*s", 29-strlen("Facebook:"), "VirginMobileKSA");
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "Twitter:%*s", 29-strlen("Twitter:"), "@VirginMobileKSA");
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "www:%*s", 29-strlen("www:"), "virginmobile.sa");
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			Jump_Lines(1);
			Prn_A_LineOf("-");


			GetCurTime(szDateTime);
			sprintf(szBufferPrint, "Date:%2.2s/%2.2s/%s%2.2s    Time:%2.2s:%2.2s", 
				szDateTime + 4, szDateTime + 2, PRE_YEAR, szDateTime, szDateTime + 6, szDateTime + 8);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s%*s", "Terminal ID:", 29 - strlen(gl_pLangSelPiont [STR_ID_TERMINAL]), gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szDateTime, "%2.2s/%2.2s/%s%2.2s", 
				stData.stVoucher.szExpirDate + 4, stData.stVoucher.szExpirDate + 2,
				PRE_YEAR, stData.stVoucher.szExpirDate);
			sprintf(szBufferPrint, "%s%17s", "Expiry Date:", szDateTime);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_CATEGORY], 
				stSav.stSavParamCategory[ucCategory_Index].szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_PRODUCT], 
				stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			if(!stData.bOffline)
			{
				sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NUM_TRANS], 
					stData.szProvisionID);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
				sprintf(szBufferPrint, "%s %04d", gl_pLangSelPiont[STR_TRACE_NO], 
					stData.usTraceNo);
			}
			else
			{
				sprintf(szBufferPrint, "%s %04d", "Batch No:", stData.usBatchNo);
			}
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);


			{
				unsigned char stAmt[SIZE_MAX_AMOUNT + 1];
				vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, stAmt);
				sprintf(szBufferPrint, "%s %s %s", gl_pLangSelPiont[STR_VALEUR], stAmt, "SR");
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			}

			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_SN], stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "%s %s%2.2s/%2.2s/%2.2s", gl_pLangSelPiont[STR_EXPIRY_DATE], 
				PRE_YEAR, stData.stVoucher.szExpirDate, stData.stVoucher.szExpirDate + 2, 
				stData.stVoucher.szExpirDate + 4);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		//added by Allen 2016-11-16
		Jump_Lines(1);
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end
		//added by Allen 2016-11-25
		if(PRN_CUSTOMER == Mode)
		{
			sprintf(szBufferPrint, "%s%20s", "Card S/N:", stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(1);
			Prn_A_LineOf("-");		
		}
		//add end
		if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);

			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0;
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}

		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			Jump_Lines(2);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end

		if(PRN_MERCHANT == Mode)
		{
			PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
			PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);

			Jump_Lines(1);
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY2], PRN_MIDD, SIZE_NORMAL,0);
		}
		else{
			PrintBuffer("Phone Fuel", PRN_MIDD, SIZE_LARGE,0);
		}

		Jump_Lines(END_JUMP + 3);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}

// Modified by Kim_LinHB 2014-7-23
static unsigned char Prn_SaleCodes_FRiENDi(SAV_PARAM stSav, TRANS_DATA stData, unsigned char Mode)
{
	//added by Allen 2016-11-16
	unsigned char ucRet;
	unsigned char ucTmp[30] = {0};
	//add end
	unsigned char ucCategory_Index = stData.ucCategory_Index;
	unsigned char ucProducts_Index = stData.ucProducts_Index;
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1]; 

	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	unsigned char bPrintedActivation = FALSE;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		if(PRN_CUSTOMER == Mode)
		{
			Prn_Logo(LOGO_FRIENDI);
			Jump_Lines(1);
		}
		

		if(PRN_CUSTOMER == Mode)
		{
			vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, szAmt);
			if (0 == atoi(&szAmt[strlen(szAmt) - 2]))
			{
				szAmt[strlen(szAmt) - 3] = 0;
			}
			sprintf(szBufferPrint, "%.12s %s", szAmt, "SAR");
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_LARGE,0);

			Jump_Lines(1);
			Prn_A_LineOf("-");
		}
		else if(PRN_MERCHANT == Mode)
		{
			Prn_Title_DateTime();
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_CASHIER_Min], stData.szCashier);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		Jump_Lines(1);

		if(PRN_MERCHANT == Mode)
		{
			if(!stData.bOffline)
				PrintBuffer("ONLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			else
				PrintBuffer("OFFLINE", PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}

		if(PRN_CUSTOMER == Mode)
		{
			unsigned char szBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
			unsigned char szDateTime[SIZE_DATE_TIME + 1];

			vReversalStr(stData.stVoucher.szNum, szBuff);
			sprintf(szBufferPrint, "#%s*101*   ", szBuff);
			SetPrintMode(0);
			PrnLeftIndent(261);//300
			PrnLogo(TeleIcon2);
			PrnStep(-25);
			PrintBuffer_AR(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			SetPrintMode(1);
			bPrintedActivation = PrintStart(TRUE) == 0x00 ? TRUE : FALSE;
			//Jump_Lines(1);    //removed by Allen 2016-11-25 
			Prn_A_LineOf("-");
			Jump_Lines(1);
			
			sprintf(szBufferPrint, "%s   *101#      %s", "To Recharge", "áÅÚÇÏÉ ÇáÔÍä");
			SetPrintMode(0);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
			PrnStep(-30);
			PrnLeftIndent((384 - 20) / 2 + 25);
			PrnLogo(TeleIcon);
			SetPrintMode(1);

			sprintf(szBufferPrint, "%s  *102#      %s", "Check Balance", "áãÚÑÝÉ ÇáÑÕíÏ");
			SetPrintMode(0);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
			PrnStep(-30);
			PrnLeftIndent((384 - 20) / 2 + 25);
			PrnLogo(TeleIcon);
			SetPrintMode(1);

			sprintf(szBufferPrint, "%s   *108#      %s", "Choose Data", "ÈÇÞÇÊ ÇáÈíÇäÇÊ");
			SetPrintMode(0);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_DEMI,0);
			PrnStep(-30);
			PrnLeftIndent((384 - 20) / 2 + 25);
			PrnLogo(TeleIcon);
			SetPrintMode(1);
			Jump_Lines(1);

			Prn_A_LineOf("-");
			PrintBuffer_AR(":us with Connect", PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			PrnStep(5);

			sprintf(szBufferPrint, "customer care:%*s", 29-strlen("customer care:"), "166000");
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "Facebook:%*s", 29-strlen("Facebook:"), "friendimobileksa");
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "www:%*s", 29-strlen("www:"), "friendimobile.com");
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			Jump_Lines(1);
			Prn_A_LineOf("-");

 			
 			GetCurTime(szDateTime);
 			sprintf(szBufferPrint, "Date:%2.2s/%2.2s/%s%2.2s    Time:%2.2s:%2.2s", 
 				szDateTime + 4, szDateTime + 2, PRE_YEAR, szDateTime, szDateTime + 6, szDateTime + 8);
 			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s%*s", "Terminal ID:", 29 - strlen(gl_pLangSelPiont [STR_ID_TERMINAL]), gl_MaintenaceParam.szTermID);
 			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szDateTime, "%2.2s/%2.2s/%s%2.2s", 
				stData.stVoucher.szExpirDate + 4, stData.stVoucher.szExpirDate + 2,
				PRE_YEAR, stData.stVoucher.szExpirDate);
			sprintf(szBufferPrint, "%s%17s", "Expiry Date:", szDateTime);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
		}
		else if(PRN_MERCHANT == Mode)
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_CATEGORY], 
				stSav.stSavParamCategory[ucCategory_Index].szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_PRODUCT], 
				stSav.stSavParamProducts[ucProducts_Index].szNameProduct);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			if(!stData.bOffline)
			{
				sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NUM_TRANS], 
					stData.szProvisionID);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
				sprintf(szBufferPrint, "%s %04d", gl_pLangSelPiont[STR_TRACE_NO], 
					stData.usTraceNo);
			}
			else
			{
				sprintf(szBufferPrint, "%s %04d", "Batch No:", stData.usBatchNo);
			}
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);


			{
				unsigned char stAmt[SIZE_MAX_AMOUNT + 1];
				vFormattageAmount(stSav.stSavParamProducts[ucProducts_Index].szAmount, stAmt);
				sprintf(szBufferPrint, "%s %s %s", gl_pLangSelPiont[STR_VALEUR], stAmt, "SR");
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			}
		
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_SN], stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "%s %s%2.2s/%2.2s/%2.2s", gl_pLangSelPiont[STR_EXPIRY_DATE], 
				PRE_YEAR, stData.stVoucher.szExpirDate, stData.stVoucher.szExpirDate + 2, 
				stData.stVoucher.szExpirDate + 4);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		//added by Allen 2016-11-16
		Jump_Lines(1);
		RemoveBlank(stData.stVoucher.szNum, ucTmp);
		PrnLeftIndent(70);
		ucRet = PrnCode128(ucTmp, strlen(ucTmp), 150, 2);
		if(ucRet != 0)
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%d",ucRet);
			PubWaitKey(TIME_DELAY);	
			return ucRet;
		}
		Jump_Lines(1);
		PrnLeftIndent(0);
		//add end
		//added by Allen 2016-11-25
		if(PRN_CUSTOMER == Mode)
		{
			sprintf(szBufferPrint, "%s%20s", "Card S/N:", stData.stVoucher.szSerial);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);
			Jump_Lines(1);
			Prn_A_LineOf("-");
		}
		//add end
		if(PRN_MERCHANT == Mode && !stData.bOffline)
		{
			unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];

			Jump_Lines(1);
			strcpy(szAmountWithoutDiscount, stSav.stSavParamProducts[ucProducts_Index].szAmount);
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			PubAscMul(stSav.stSavParamProducts[ucProducts_Index].szAmount,
				stSav.stSavParamProducts[ucProducts_Index].szDiscount, 
				szDiscount);

			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscMul(szAmountWithoutDiscount, "100", szAmountWithoutDiscount);
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}
			PubAddHeadChars(szDiscount, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmountWithoutDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmountWithoutDiscount,szDiscount,strlen(szAmountWithoutDiscount),
				szAmountWithDiscount);

			if(strlen(szAmountWithoutDiscount) > 2)
			{
				szAmountWithoutDiscount[strlen(szAmountWithoutDiscount) - 2] = 0;
			}
			if(strlen(szAmountWithDiscount) > 2)
			{
				szAmountWithDiscount[strlen(szAmountWithDiscount) - 2] = 0;
			}
			if(strlen(szDiscount) > 2)
			{
				szDiscount[strlen(szDiscount) - 2] = 0;
			}

			PrintBuffer("Gross Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithoutDiscount, '0');
			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Net Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szAmountWithDiscount, '0');
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Discount Amount", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PubTrimHeadChars(szDiscount, '0');
			vFormattageAmount(szDiscount, szDiscount);
			sprintf(szBufferPrint, "%s SR", szDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer("Balance", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(stData.stVoucher.szCurrDeposit, szAmountWithoutDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			Jump_Lines(1);
		}
		//added by Allen 2016-11-16
#ifdef FARSI  
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, " ") != 0)
		{
			Jump_Lines(2);
			PrintBuffer_AR(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage_AR, PRN_LEFT, SIZE_NORMAL, -3);
			Jump_Lines(2);		
		}
#endif
		if(strcmp(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, " ") != 0)
		{
			PrintBuffer(stSav.stSavParamProducts[ucProducts_Index].szSlipMessage, PRN_LEFT, SIZE_NORMAL, 0);
			Jump_Lines(1);		
		}
		//add end

		if(PRN_MERCHANT == Mode)
		{
			PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
			PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);

			Jump_Lines(1);
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY2], PRN_MIDD, SIZE_NORMAL,0);
		}
		// Removed by Kim_LinHB 2014/9/12 v4.96
// 		else{ // Added by Kim_LinHB 2014-09-04
// 			PrintBuffer("Phone Fuel", PRN_MIDD, SIZE_LARGE,0);
// 		
		Jump_Lines(END_JUMP + 3);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue && !bPrintedActivation);
	return sg_bCanCelPrn;
}
// Add End

// Modified by Kim_LinHB 2013-3-23 v4.56
// Modified by Kim_LinHB 2013-9-22 v4.69 stStock[].productIndex are incorrect sometimes.
// modified atoi() to strcmp(), productIndex, categoryCode to int
char Prn_SalePolling(unsigned char type)
{
	typedef struct _tagTmp
	{
		unsigned char Id[SIZE_MAX_CODE_PRODUCT + 1];
		unsigned int cnt;
		int productIndex;
		int categoryCode;
	}TMP_Stock;
	TMP_Stock stStock[NB_MAX_CODE_STOCK];
	unsigned int i, ii, productCnt;
	STOCK stock;
	char bValid = TRUE; // Added by Kim_LinHB 2013-9-22 v4.69

	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
	unsigned char ucType = type;

	unsigned long ulTotalQty_All = 0;
	unsigned char szTotalAmt_All[SIZE_MAX_AMOUNT + 1];

#ifdef JEFF_TEST
		DebugComSend("File_%s,LineNo:%d,func=%s",__FILE__,__LINE__,__FUNCTION__);
#endif
	memset(stStock, 0, sizeof(stStock));
	for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
	{
		stStock[i].productIndex = -1;
		stStock[i].categoryCode = -1;
	}
	productCnt = 0;

	if(CODE_FIN_DAILY == type)
		ucType = CODE_RESTOCK_SOLD; // daily report just print sold not sale polling

	ReadStock_All(&stock);
	for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
	{
		unsigned char bNew = TRUE;
		unsigned int j;
		if(ucType == stock.stStock[i].ucProductState)
		{
			for(j = 0; j < productCnt; ++j)
			{
				if(0 == strcmp(stStock[j].Id, stock.stVoucher[i].szProductId))
				{	
					++stStock[j].cnt;
					bNew = FALSE;
					break;
				}
			}
			if (bNew)
			{
				strcpy(stStock[productCnt].Id, stock.stVoucher[i].szProductId);
				stStock[productCnt].cnt = 1;
				++productCnt;
			}
		}
	}
	if(CODE_FIN_DAILY == type && gl_MaintenaceParam.ucMerchantMode != OFFLINE)
	{
		TRANS_DATA stTmp;
		int fd = open(FILE_TRANS_OL, O_RDWR);
		if(fd >= 0)
		{
			long len = filesize(FILE_TRANS_OL);
			memset(&stTmp, 0, sizeof(TRANS_DATA));
			seek(fd , 0, SEEK_SET);
			while(len > 0)
			{
				unsigned char bNew = TRUE;
				unsigned int j;
				read(fd, (void *)&stTmp, sizeof(TRANS_DATA));
				
				for(j = 0; j < productCnt; ++j)
				{
					if(0 == strcmp(stStock[j].Id, stTmp.stVoucher.szProductId))
					{	
						++stStock[j].cnt;
						bNew = FALSE;
						break;
					}
				}
				if (bNew)
				{
					strcpy(stStock[productCnt].Id, stTmp.stVoucher.szProductId);
					stStock[productCnt].cnt = 1;
					++productCnt;
				}
				len -= sizeof(TRANS_DATA);
			}
			close(fd);
		}
		else
			close(fd);
	}

	for(i = 0; i < productCnt; ++i)
	{
		unsigned int j;
		for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
		{
			if(0 == strcmp(stStock[i].Id, gl_SavParam.stSavParamProducts[j].szCodeProduct))
			{
				stStock[i].productIndex = j;
				stStock[i].categoryCode = gl_SavParam.stSavParamProducts[j].uiCodeCategory;
				break;
			}
		}
	}
	
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();
		
		ulTotalQty_All = 0;
		sprintf(szTotalAmt_All, "%.*d", SIZE_MAX_AMOUNT, 0);

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		switch(type)
		{
		case CODE_RESTOCK_DOWNLOADING:
			PrintBuffer(gl_pLangSelPiont[STR_RECHARGEMENT_CODES], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			break;
		case CODE_RESTOCK_SALESPOLLING:
			PrintBuffer(gl_pLangSelPiont[STR_SALESPOLLING2], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			break;
		case CODE_RESTOCK_EXPIRED:
			PrintBuffer(gl_pLangSelPiont[STR_PINS_EXPIRED], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			break;
		case CODE_RESTOCK_BURNED:
			PrintBuffer(gl_pLangSelPiont[STR_PINS_USED], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			break;
 		case CODE_RESTOCK_SELLING:
 			PrintBuffer(gl_pLangSelPiont[STR_ANOMALIE_VENTE], PRN_MIDD, SIZE_NORMAL_BOLD,0);
 			break;
		case CODE_FIN_DAILY:
			PrintBuffer(gl_pLangSelPiont[STR_REPORT_DAILY_PRINT], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			break;
		default:
			return bValid;
		}

		//Jump_Lines(1);

		for (i = 0, ii = 0; ii < productCnt && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned char bFirst =TRUE;
			unsigned int j;
			unsigned long ulTotalQty = 0;
			unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
			sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
			for(j = 0; j < productCnt; ++j)
			{
				// Modified by Kim_LinHB 2013-9-22 v4.69
				if((int)gl_SavParam.stSavParamCategory[i].uiCodeCategory == stStock[j].categoryCode
					&& stStock[j].categoryCode != -1 && stStock[j].productIndex != -1)
				{
					if(TRUE == bFirst)
					{
						if(CODE_RESTOCK_DOWNLOADING == ucType || 
							CODE_FIN_DAILY == type || 
							gl_MaintenaceParam.bPrintDetailRestock)
						{
							sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							//PrintBuffer(gl_pLangSelPiont[STR_SOUS_TITRE_PRODUCT], PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%-10s%7s%11s ",
								gl_pLangSelPiont[STR_PXUNITAIRE], 
								gl_pLangSelPiont[STR_NB],
								gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							Prn_A_LineOf("-");
						}
						bFirst = FALSE;
					}
					if((ucType != CODE_RESTOCK_FREE && gl_MaintenaceParam.bPrintDetailRestock) ||
						CODE_RESTOCK_DOWNLOADING == ucType ||
						CODE_FIN_DAILY == type)
					{
						PrintBuffer(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szNameProduct, 
							PRN_LEFT, SIZE_NORMAL_BOLD, 0);
					}
					vFormattageAmount(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, szAmount);
					sprintf(szBufferPrint,"%-10s %5d",szAmount, stStock[j].cnt);
					{
						unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
						sprintf(szQuantity, "%d", stStock[j].cnt);
						PubAscMul(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, 
									szQuantity, szAmount);
						PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
						PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
						SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
						if(atol(szAmount) != 0)
							PubTrimHeadChars(szAmount, '0');
						else
							strcpy(szAmount, "0");
					}
					ulTotalQty += stStock[j].cnt;
					
					if(CODE_RESTOCK_DOWNLOADING == ucType || 
						CODE_FIN_DAILY == type ||
						gl_MaintenaceParam.bPrintDetailRestock)
					{
						vFormattageAmount(szAmount, szAmount);
						if(strlen(szAmount) > 10)
						{
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%sSR",szAmount);
							PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
						}
						else
						{
							unsigned char szTmpBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
							sprintf(szTmpBuff,"%s %10sSR",szBufferPrint, szAmount);
							PrintBuffer(szTmpBuff, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						}
					}
					++ii;    ///////...........
				}
			}
			if(TRUE != bFirst) // print total
			{
				if(CODE_RESTOCK_DOWNLOADING == ucType || 
					CODE_FIN_DAILY == type ||
					gl_MaintenaceParam.bPrintDetailRestock)
				{
					Prn_A_LineOf("-");
					if(atol(szTotalAmt) != 0)
						PubTrimHeadChars(szTotalAmt, '0');
					else
						strcpy(szTotalAmt, "0");
					vFormattageAmount(szTotalAmt, szAmount);
					if(strlen(szAmount) > 10)
					{
						sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL], ulTotalQty);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint, "%sSR", szAmount);
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
					}
					else
					{
						sprintf(szBufferPrint, "%-10s %5lu %10sSR", gl_pLangSelPiont[STR_TOTAL], ulTotalQty, szAmount);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					}
					Jump_Lines(1);
				}
				ulTotalQty_All += ulTotalQty;
				PubAddHeadChars(szTotalAmt_All, SIZE_MAX_AMOUNT, '0');
				PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
				SafeAscAdd(szTotalAmt_All, szTotalAmt, SIZE_MAX_AMOUNT);
				if(atol(szTotalAmt_All) != 0)
					PubTrimHeadChars(szTotalAmt_All, '0');
				else
					strcpy(szTotalAmt_All, "0");
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				ulTotalQty = 0;
			}
		}

		// Added by Kim_LinHB 2013-9-22 v4.69
		if(CODE_RESTOCK_DOWNLOADING == type)
		{
			char bFirst = TRUE;
			for(i = 0; i < productCnt; ++i)
			{
				if(-1 == stStock[i].productIndex)
				{
					if(bFirst)
					{
						Prn_A_LineOf("-");
						PrintBuffer("Unknown product(s), Please run setting download,", PRN_LEFT, SIZE_NORMAL,0);
						PrintBuffer("and run restock pins again.", PRN_LEFT, SIZE_NORMAL,0);
						Jump_Lines(1);
						PrintBuffer_AR("receipt invalid", PRN_MIDD, SIZE_LARGE_BOLD,0);
						Jump_Lines(1);
						bFirst = FALSE;
						bValid = FALSE;
					}
					sprintf(szBufferPrint, "Id:%s, QTY:%d", stStock[i].Id, stStock[i].cnt);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
				}
			}
			Jump_Lines(1);
		}
		// Add End

		//Jump_Lines(1);
		if(type != CODE_RESTOCK_DOWNLOADING && gl_SavParam.stClaim.usNbClaim != 0)
		{
			//Prn_A_LineOf("-");
			PrintBuffer(gl_pLangSelPiont[STR_RECLAMATION], PRN_LEFT, SIZE_NORMAL_BOLD,0);

			sprintf(szAmount, "%ld", gl_SavParam.stClaim.ulClaimAmount);
			vFormattageAmount(szAmount, szAmount);
			if(strlen(szAmount) > 10)
			{
				sprintf(szBufferPrint, "%-10s %5d", gl_pLangSelPiont[STR_TOTAL], gl_SavParam.stClaim.usNbClaim);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
				sprintf(szBufferPrint, "%sSR", szAmount);
				PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			}
			else
			{
				sprintf(szBufferPrint, "%-10s %5d %10sSR", gl_pLangSelPiont[STR_TOTAL], 
					gl_SavParam.stClaim.usNbClaim, szAmount);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
			}

			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		}
		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		if(CODE_FIN_DAILY == type)
			sprintf(szBufferPrint, "%-21s%8lu", "Pins Qty Sold:", ulTotalQty_All);
		else
			sprintf(szBufferPrint, "%-21s%8lu", gl_pLangSelPiont[STR_Nb_Codes_Telecol], ulTotalQty_All);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

		vFormattageAmount(szTotalAmt_All, szAmount);
		sprintf(szBufferPrint, "%-15s%s %s", gl_pLangSelPiont[STR_Amount_Tot], 
			szAmount, "SR");
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);


		if(CODE_FIN_DAILY == type && MIX == gl_MaintenaceParam.ucMerchantMode)
		{
			unsigned int cnt[NB_MAX_PRODUCTS];
			TRANS_DATA_CASH stTmp;
			unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
			unsigned long ulTotalQty_Cash = 0;
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szTmpAmt[SIZE_MAX_AMOUNT+1];

			long len = filesize(FILE_CASHORDER); // Modified by Kim_LinHB 2013-9-24 v4.70
			int fd = open(FILE_CASHORDER, O_RDWR);

			memset(cnt, 0 , sizeof(cnt));
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szAmountWithoutDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			if(fd >= 0)
			{
				seek(fd , 0, SEEK_SET);
				while(len > 0)
				{
					read(fd, (void *)&stTmp, sizeof(TRANS_DATA_CASH));

					for(i = 0; i < stTmp.ucProductNum; ++i)
					{
						unsigned int j;
						for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
						{
							if(0 == strcmp(stTmp.stProducts[i].szCodeProduct, gl_SavParam.stSavParamProducts[j].szCodeProduct))
							{	
								cnt[j] += atoi(stTmp.stProducts[i].szQuantity);
								break;
							}
						}
					}
					len -= sizeof(TRANS_DATA_CASH);
					strcpy(szTmpAmt, stTmp.szAmountWithDiscount);
					PubAddHeadChars(szTmpAmt, SIZE_MAX_AMOUNT, '0');
					SafeAscAdd(szAmountWithDiscount,
						szTmpAmt,
						SIZE_MAX_AMOUNT);
					strcpy(szTmpAmt, stTmp.szAmountWithoutDiscount);
					PubAddHeadChars(szTmpAmt, SIZE_MAX_AMOUNT, '0');
					SafeAscAdd(szAmountWithoutDiscount,
						szTmpAmt,
						SIZE_MAX_AMOUNT);
				}
				close(fd);
			}
			else
				close(fd);

			Jump_Lines(1);
			PrintBuffer("Cash Order", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			Prn_A_LineOf("*");
			for (i = 0; i < gl_SavParam.ucCategoryNum; ++i)
			{
				unsigned int j;
				unsigned char bFirst =TRUE;
				unsigned long ulTotalQty = 0;
				unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);

				for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
				{
					if(0 == cnt[j])
						continue;

					if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamProducts[j].uiCodeCategory)
					{
						unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
						if(TRUE == bFirst)
						{
							sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%-12s%5s%12s",
								gl_pLangSelPiont[STR_PXUNITAIRE], 
								gl_pLangSelPiont[STR_NB],
								gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
							Prn_A_LineOf("-");
							bFirst = FALSE;
						}
						PrintBuffer(gl_SavParam.stSavParamProducts[j].szNameProduct, 
							PRN_LEFT, SIZE_NORMAL_BOLD, 0);
						sprintf(szBufferPrint, "-%d.%02d%s", 
							atoi(gl_SavParam.stSavParamProducts[j].szDiscount) / 100 ,
							atoi(gl_SavParam.stSavParamProducts[j].szDiscount) % 100,
							gl_pLangSelPiont[STR_PERC_DISCOUNT]);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
						vFormattageAmount(gl_SavParam.stSavParamProducts[j].szAmount, szAmount);
						{
							unsigned char szTempBuff[200];
							sprintf(szTempBuff,"%s %s",szAmount, "SR");
							sprintf(szBufferPrint,"%-10s %5d",szTempBuff, cnt[j]);
						}
						sprintf(szQuantity, "%d", cnt[j]);
						PubAscMul(gl_SavParam.stSavParamProducts[j].szAmount, 
							szQuantity, szAmount);
						PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
						SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
						if(atol(szAmount) != 0)
							PubTrimHeadChars(szAmount, '0');
						else
							strcpy(szAmount, "0");
						ulTotalQty += cnt[j];
						vFormattageAmount(szAmount, szAmount);
						if(strlen(szAmount) > 10)
						{
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%sSR",szAmount);
							PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
						}
						else
						{
							unsigned char szTempBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
							sprintf(szTempBuff,"%s %10sSR",szBufferPrint, szAmount);
							PrintBuffer(szTempBuff, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						}
					}
				}
				if(TRUE != bFirst) // print total
				{
					unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
					Prn_A_LineOf("-");
					if(atol(szTotalAmt) != 0)
						PubTrimHeadChars(szTotalAmt, '0');
					else
						strcpy(szTotalAmt, "0");
					vFormattageAmount(szTotalAmt, szAmount);
					if(strlen(szAmount) > 10)
					{
						sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL],ulTotalQty);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint, "%s %s", szAmount, "SR");
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
					}
					else
					{
						sprintf(szBufferPrint, "%-10s %5lu %10s%s", gl_pLangSelPiont[STR_TOTAL],ulTotalQty, szAmount, "SR");
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					}
					Jump_Lines(1);
					ulTotalQty_Cash += ulTotalQty;
					sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
					ulTotalQty = 0;
				}
			}
			PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			sprintf(szBufferPrint, "%-21s%8lu", gl_pLangSelPiont[STR_PINS_QUANTITY], ulTotalQty_Cash);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

			// Removed by Kim_LinHB 2013-9-24 v4.70
// 			PrintBuffer(gl_pLangSelPiont[STR_FACE_VALUE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
// 			if(atol(szAmountWithoutDiscount) != 0)
// 				PubTrimHeadChars(szAmountWithoutDiscount, '0');
// 			else
// 				strcpy(szAmountWithoutDiscount, "0");
// 			vFormattageAmount(szAmountWithoutDiscount, szAmountWithoutDiscount);
// 			sprintf(szBufferPrint, "%s SR", szAmountWithoutDiscount);
// 			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

			PrintBuffer(gl_pLangSelPiont[STR_PAID], PRN_LEFT, SIZE_NORMAL_BOLD,0);
			if(atol(szAmountWithDiscount) != 0)
				PubTrimHeadChars(szAmountWithDiscount, '0');
			else
				strcpy(szAmountWithDiscount, "0");
			vFormattageAmount(szAmountWithDiscount, szAmountWithDiscount);
			sprintf(szBufferPrint, "%s SR", szAmountWithDiscount);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
		}

		Prn_A_LineOf("*");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
	return bValid;
}
// Modify End

char Prn_RestockPin()
{
	typedef struct _tagTmp
	{
		unsigned char Id[SIZE_MAX_CODE_PRODUCT + 1];
		unsigned int cnt;
		int productIndex;
		int categoryCode;
	}TMP_Stock;
	TMP_Stock stStock[NB_MAX_CODE_STOCK];
	unsigned int i, ii, productCnt;
	STOCK_NEW stock;
	char bValid = TRUE; // Added by Kim_LinHB 2013-9-22 v4.69

	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];

	unsigned long ulTotalQty_All = 0;
	unsigned char szTotalAmt_All[SIZE_MAX_AMOUNT + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		ulTotalQty_All = 0;
		sprintf(szTotalAmt_All, "%.*d", SIZE_MAX_AMOUNT, 0);

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_RECHARGEMENT_CODES], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(1);

		memset(stStock, 0, sizeof(stStock));
		memset(&stock, 0, sizeof(stock));
		while(OK == ReadStockOrder(&stock))
		{
			if(stock.num == 0){
				DelStockOrder();
				continue;
			}
			for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
			{
				stStock[i].productIndex = -1;
				stStock[i].categoryCode = -1;
			}
			productCnt = 0;

			for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
			{
				unsigned char bNew = TRUE;
				unsigned int j;
				for(j = 0; j < productCnt; ++j)
				{
					if(0 == strcmp(stStock[j].Id, stock.stVoucher[i].szProductId))
					{	
						++stStock[j].cnt;
						bNew = FALSE;
						break;
					}
				}
				if (bNew)
				{
					strcpy(stStock[productCnt].Id, stock.stVoucher[i].szProductId);
					stStock[productCnt].cnt = 1;
					++productCnt;
				}
			}

			Jump_Lines(1);
			sprintf(szBufferPrint, "%s %s", "ORDER ID:", stock.szOrderId);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

			if(atol(stock.szOrderId) > 0)
			{
				PrintBuffer("CreatedDate:", PRN_LEFT, SIZE_NORMAL,0);
				sprintf(szBufferPrint, "%s%2.2s/%2.2s/%2.2s%4.2s:%2.2s", 
					PRE_YEAR, stock.szOrderDate, stock.szOrderDate + 2, stock.szOrderDate + 4, stock.szOrderDate + 6, 
					stock.szOrderDate + 8);
				PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL,0);
			}
			Jump_Lines(1);

			for(i = 0; i < productCnt; ++i)
			{
				unsigned int j;
				for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
				{
					if(0 == strcmp(stStock[i].Id, gl_SavParam.stSavParamProducts[j].szCodeProduct))
					{
						stStock[i].productIndex = j;
						stStock[i].categoryCode = gl_SavParam.stSavParamProducts[j].uiCodeCategory;
						break;
					}
				}
			}

			for (i = 0, ii = 0; ii < productCnt && i < gl_SavParam.ucCategoryNum; ++i)
			{
				unsigned char bFirst =TRUE;
				unsigned int j;
				unsigned long ulTotalQty = 0;
				unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				for(j = 0; j < productCnt; ++j)
				{
					// Modified by Kim_LinHB 2013-9-22 v4.69
					if((int)gl_SavParam.stSavParamCategory[i].uiCodeCategory == stStock[j].categoryCode
						&& stStock[j].categoryCode != -1 && stStock[j].productIndex != -1)
					{
						if(TRUE == bFirst)
						{
							sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							//PrintBuffer(gl_pLangSelPiont[STR_SOUS_TITRE_PRODUCT], PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%-10s%7s%11s ",
								gl_pLangSelPiont[STR_PXUNITAIRE], 
								gl_pLangSelPiont[STR_NB],
								gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							Prn_A_LineOf("-");

							bFirst = FALSE;
						}

						PrintBuffer(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szNameProduct, 
							PRN_LEFT, SIZE_NORMAL_BOLD, 0);

						vFormattageAmount(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, szAmount);
						sprintf(szBufferPrint,"%-10s %5d",szAmount, stStock[j].cnt);
						{
							unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
							sprintf(szQuantity, "%d", stStock[j].cnt);
							PubAscMul(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, 
								szQuantity, szAmount);
							PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
							PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
							SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
							if(atol(szAmount) != 0)
								PubTrimHeadChars(szAmount, '0');
							else
								strcpy(szAmount, "0");
						}
						ulTotalQty += stStock[j].cnt;

						vFormattageAmount(szAmount, szAmount);
						if(strlen(szAmount) > 10)
						{
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%sSR",szAmount);
							PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
						}
						else
						{
							unsigned char szTmpBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
							sprintf(szTmpBuff,"%s %10sSR",szBufferPrint, szAmount);
							PrintBuffer(szTmpBuff, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						}
						++ii;    ///////...........
					}
				}
				if(TRUE != bFirst) // print total
				{
					Prn_A_LineOf("-");
					if(atol(szTotalAmt) != 0)
						PubTrimHeadChars(szTotalAmt, '0');
					else
						strcpy(szTotalAmt, "0");
					vFormattageAmount(szTotalAmt, szAmount);
					if(strlen(szAmount) > 10)
					{
						sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL], ulTotalQty);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint, "%sSR", szAmount);
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
					}
					else
					{
						sprintf(szBufferPrint, "%-10s %5lu %10sSR", gl_pLangSelPiont[STR_TOTAL], ulTotalQty, szAmount);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					}
					Jump_Lines(1);

					ulTotalQty_All += ulTotalQty;
					PubAddHeadChars(szTotalAmt_All, SIZE_MAX_AMOUNT, '0');
					PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
					SafeAscAdd(szTotalAmt_All, szTotalAmt, SIZE_MAX_AMOUNT);
					if(atol(szTotalAmt_All) != 0)
						PubTrimHeadChars(szTotalAmt_All, '0');
					else
						strcpy(szTotalAmt_All, "0");
					sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
					ulTotalQty = 0;
				}
			}
			DelStockOrder();
			Prn_A_LineOf("*");
		}

		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		sprintf(szBufferPrint, "%-21s%8lu", gl_pLangSelPiont[STR_Nb_Codes_Telecol], ulTotalQty_All);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

		vFormattageAmount(szTotalAmt_All, szAmount);
		sprintf(szBufferPrint, "%-15s%s %s", gl_pLangSelPiont[STR_Amount_Tot], 
			szAmount, "SR");
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

		Prn_A_LineOf("*");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
	return bValid;
}

void Prn_Stock(unsigned char bDetail)
{
	unsigned int i, ii, total = 0;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	STOCK stock;
	ReadStock_All(&stock);

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		total = FindCodesCnt(CODE_RESTOCK_FREE, NULL) + FindCodesCnt(CODE_RESTOCK_SOLD, NULL) +
				FindCodesCnt(CODE_RESTOCK_SALESPOLLING, NULL);

		Prn_Title_DateTime();
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_ETAT_DU_STOCK], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		//Jump_Lines(1);

		for (i = 0, ii = 0; ii < total && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned char bFirst = TRUE;
			unsigned int j;
			for(j = 0; ii < total && j < gl_SavParam.ucProductsNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamProducts[j].uiCodeCategory)
				{
					unsigned int cntSold = 0, cntAvail = 0;
					cntSold = FindCodesCnt(CODE_RESTOCK_SOLD, gl_SavParam.stSavParamProducts[j].szCodeProduct) +
								FindCodesCnt(CODE_RESTOCK_SALESPOLLING, gl_SavParam.stSavParamProducts[j].szCodeProduct);
					cntAvail = FindCodesCnt(CODE_RESTOCK_FREE, gl_SavParam.stSavParamProducts[j].szCodeProduct);

					// Added by Kim_LinHB 2013-7-24 v4.62
					if(cntSold <= 0 && cntAvail <= 0)
						continue;
					// Add End

					ii += (cntAvail + cntSold); // Added by Kim_LinHB 2013-8-20 v4.62

					if(bFirst)
					{
						PrintBuffer(gl_SavParam.stSavParamCategory[i].szName, PRN_MIDD, SIZE_NORMAL_BOLD,0);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					PrintBuffer(gl_SavParam.stSavParamProducts[j].szNameProduct, 
						PRN_LEFT, SIZE_NORMAL_BOLD, 0);
					if(bDetail)
					{
						unsigned int k;
						sprintf(szBufferPrint, "%s:%lu", gl_pLangSelPiont[STR_DISP], cntAvail);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
						for(k = 0; cntAvail > 0 && k < NB_MAX_CODE_STOCK; ++k)
						{
							if(CODE_RESTOCK_FREE == stock.stStock[k].ucProductState &&
								0 == strcmp(stock.stVoucher[k].szProductId, gl_SavParam.stSavParamProducts[j].szCodeProduct))
							{
								PrintBuffer(stock.stVoucher[k].szSerial, PRN_MIDD, SIZE_NORMAL,0);
							}
						}
						sprintf(szBufferPrint, "%s:%u", gl_pLangSelPiont[STR_VEND], cntSold);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
						// Added by Kim_LinHB 2013-8-14 v4.62
						for(k = 0; cntSold > 0 && k < NB_MAX_CODE_STOCK; ++k)
						{
							if((CODE_RESTOCK_SOLD == stock.stStock[k].ucProductState ||
								CODE_RESTOCK_SALESPOLLING == stock.stStock[k].ucProductState) &&
								0 == strcmp(stock.stVoucher[k].szProductId, gl_SavParam.stSavParamProducts[j].szCodeProduct))
							{
								PrintBuffer(stock.stVoucher[k].szSerial, PRN_MIDD, SIZE_NORMAL,0);
							}
						}
						// Add End
					}
					else
					{
						sprintf(szBufferPrint, "%s(%s:%u %s:%u)", gl_pLangSelPiont[STR_ON_LINE],
								gl_pLangSelPiont[STR_DISP], cntAvail,
								gl_pLangSelPiont[STR_VEND], cntSold);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
					}
				}
			}
			if(!bFirst)
				Prn_A_LineOf("-");
		}

		if(0 == total)
			PrintBuffer("Empty", PRN_MIDD, SIZE_NORMAL,0);

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

char Prn_PendingOrder()
{
	typedef struct _tagTmp
	{
		int PId;
		int productIndex;
		int categoryCode;
	}TMP_Stock;
	TMP_Stock stStock[NB_MAX_CODE_STOCK];
	unsigned int i, ii;
	PENDING_ORD stOrder;
	char bValid = TRUE; // Added by Kim_LinHB 2013-9-22 v4.69

	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];

	unsigned long ulTotalQty_All = 0;
	unsigned char szTotalAmt_All[SIZE_MAX_AMOUNT + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		ulTotalQty_All = 0;
		sprintf(szTotalAmt_All, "%.*d", SIZE_MAX_AMOUNT, 0);

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_PENDING_ORDER], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(1);

		memset(stStock, 0, sizeof(stStock));
		memset(&stOrder, 0, sizeof(stOrder));
		while(OK == ReadPendingOrder(&stOrder))
		{
			for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
			{
				stStock[i].productIndex = -1;
				stStock[i].categoryCode = -1;
			}

			Jump_Lines(1);
			sprintf(szBufferPrint, "%s %s", "ORDER ID:", stOrder.szOrderId);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

			if(atol(stOrder.szOrderId) > 0)
			{
				PrintBuffer("CreatedDate:", PRN_LEFT, SIZE_NORMAL,0);
				sprintf(szBufferPrint, "%s%2.2s/%2.2s/%2.2s%4.2s:%2.2s", 
					PRE_YEAR, stOrder.szDate, stOrder.szDate + 2, stOrder.szDate + 4,
					stOrder.szDate + 6, stOrder.szDate + 8);
				PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL,0);
			}
			Jump_Lines(1);

			for(i = 0; i < stOrder.num; ++i)
			{
				unsigned int j;
				for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
				{
					if(0 == strcmp(stOrder.stOrder[i].PId, gl_SavParam.stSavParamProducts[j].szCodeProduct))
					{
						stStock[i].productIndex = j;
						stStock[i].categoryCode = gl_SavParam.stSavParamProducts[j].uiCodeCategory;
						break;
					}
				}
			}

			for (i = 0, ii = 0; ii < stOrder.num && i < gl_SavParam.ucCategoryNum; ++i)
			{
				unsigned char bFirst =TRUE;
				unsigned int j;
				unsigned long ulTotalQty = 0;
				unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				for(j = 0; j < stOrder.num; ++j)
				{
					// Modified by Kim_LinHB 2013-9-22 v4.69
					if((int)gl_SavParam.stSavParamCategory[i].uiCodeCategory == stStock[j].categoryCode
						&& stStock[j].categoryCode != -1 && stStock[j].productIndex != -1)
					{
						if(TRUE == bFirst)
						{
							sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							//PrintBuffer(gl_pLangSelPiont[STR_SOUS_TITRE_PRODUCT], PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%-10s%7s%11s ",
								gl_pLangSelPiont[STR_PXUNITAIRE], 
								gl_pLangSelPiont[STR_NB],
								gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							Prn_A_LineOf("-");

							bFirst = FALSE;
						}

						PrintBuffer(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szNameProduct, 
							PRN_LEFT, SIZE_NORMAL_BOLD, 0);

						vFormattageAmount(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, szAmount);
						sprintf(szBufferPrint,"%-10s %5d",szAmount, stOrder.stOrder[j].num);
						{
							unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
							sprintf(szQuantity, "%d", stOrder.stOrder[j].num);
							PubAscMul(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, 
								szQuantity, szAmount);
							PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
							PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
							SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
							if(atol(szAmount) != 0)
								PubTrimHeadChars(szAmount, '0');
							else
								strcpy(szAmount, "0");
						}
						ulTotalQty += stOrder.stOrder[j].num;

						vFormattageAmount(szAmount, szAmount);
						if(strlen(szAmount) > 10)
						{
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%sSR",szAmount);
							PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
						}
						else
						{
							unsigned char szTmpBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
							sprintf(szTmpBuff,"%s %10sSR",szBufferPrint, szAmount);
							PrintBuffer(szTmpBuff, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						}
						++ii;    ///////...........
					}
				}
				if(TRUE != bFirst) // print total
				{
					Prn_A_LineOf("-");
					if(atol(szTotalAmt) != 0)
						PubTrimHeadChars(szTotalAmt, '0');
					else
						strcpy(szTotalAmt, "0");
					vFormattageAmount(szTotalAmt, szAmount);
					if(strlen(szAmount) > 10)
					{
						sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL], ulTotalQty);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint, "%sSR", szAmount);
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
					}
					else
					{
						sprintf(szBufferPrint, "%-10s %5lu %10sSR", gl_pLangSelPiont[STR_TOTAL], ulTotalQty, szAmount);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					}
					Jump_Lines(1);

					ulTotalQty_All += ulTotalQty;
					PubAddHeadChars(szTotalAmt_All, SIZE_MAX_AMOUNT, '0');
					PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
					SafeAscAdd(szTotalAmt_All, szTotalAmt, SIZE_MAX_AMOUNT);
					if(atol(szTotalAmt_All) != 0)
						PubTrimHeadChars(szTotalAmt_All, '0');
					else
						strcpy(szTotalAmt_All, "0");
					sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
					ulTotalQty = 0;
				}
			}
			DelPendingOrder();
			Prn_A_LineOf("*");
		}

		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		sprintf(szBufferPrint, "%-21s%8lu", gl_pLangSelPiont[STR_Nb_Codes_Telecol], ulTotalQty_All);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

		vFormattageAmount(szTotalAmt_All, szAmount);
		sprintf(szBufferPrint, "%-15s%s %s", gl_pLangSelPiont[STR_Amount_Tot], 
			szAmount, "SR");
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

		Prn_A_LineOf("*");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
	return bValid;
}

void Prn_ReportCashier(USER_INFO stCur)
{
	typedef struct _tagTmp
	{
		unsigned char Id[SIZE_MAX_CODE_PRODUCT + 1];
		unsigned int cnt;
		unsigned char productIndex;
		unsigned char categoryCode;
	}TMP_Stock;
	TMP_Stock stStock[NB_MAX_CODE_STOCK];
	unsigned int i, ii, productCnt = 0;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned long ulTotalQty_All = 0;
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
	unsigned char szTotalAmt_All[SIZE_MAX_AMOUNT + 1];
	STOCK stock;

	ReadStock_All(&stock);
	memset(stStock, 0, sizeof(stStock));
	productCnt = 0;
	for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
	{
		unsigned char bNew = TRUE;
		unsigned int j;
		if(CODE_RESTOCK_SOLD == stock.stStock[i].ucProductState &&
			0 == strcmp(stCur.szName, stock.stStock[i].szCashierId))
		{
			for(j = 0; j < productCnt; ++j)
			{
				if(0 == strcmp(stStock[j].Id, stock.stVoucher[i].szProductId))
				{	
					++stStock[j].cnt;
					bNew = FALSE;
					break;
				}
			}
			if (bNew)
			{
				strcpy(stStock[productCnt].Id, stock.stVoucher[i].szProductId);
				stStock[productCnt].cnt = 1;
				++productCnt;
			}
		}
	}
	if(gl_MaintenaceParam.ucMerchantMode != OFFLINE){
		TRANS_DATA stTmp;
		int fd = open(FILE_TRANS_OL, O_RDWR);
		memset(&stTmp, 0, sizeof(TRANS_DATA));
		if(fd >= 0)
		{
			long len = filesize(FILE_TRANS_OL);
			seek(fd , 0, SEEK_SET);
			while(len > 0)
			{
				unsigned char bNew = TRUE;
				unsigned int j;
				read(fd, (void *)&stTmp, sizeof(TRANS_DATA));

				if(0 == strcmp(stCur.szName, stTmp.szCashier))
				{
					for(j = 0; j < productCnt; ++j)
					{
						if(0 == strcmp(stStock[j].Id, stTmp.stVoucher.szProductId))
						{	
							++stStock[j].cnt;
							bNew = FALSE;
							break;
						}
					}
					if (bNew)
					{
						strcpy(stStock[productCnt].Id, stTmp.stVoucher.szProductId);
						stStock[productCnt].cnt = 1;
						++productCnt;
					}
				}
				len -= sizeof(TRANS_DATA);
			}
			close(fd);
		}
		else
			close(fd);
	}

	for(i = 0; i < productCnt; ++i)
	{
		unsigned int j;
		for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
		{
			if(0 == strcmp(stStock[i].Id, gl_SavParam.stSavParamProducts[j].szCodeProduct))
			{
				stStock[i].productIndex = j;
				stStock[i].categoryCode = gl_SavParam.stSavParamProducts[j].uiCodeCategory;
				break;
			}
		}
	}


	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		ulTotalQty_All = 0;
		sprintf(szTotalAmt_All, "%.*d", SIZE_MAX_AMOUNT, 0);

		Prn_Title_DateTime();
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_REPORT], PRN_MIDD, SIZE_LARGE_BOLD,0);
		PrintBuffer(gl_pLangSelPiont[STR_CASHIER], PRN_MIDD, SIZE_LARGE_BOLD,0);

		//Jump_Lines(1);

		sprintf(szBufferPrint,"%s: %s", gl_pLangSelPiont[STR_CASHIER],stCur.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

		//Jump_Lines(1);


		for (i = 0, ii = 0; ii < productCnt && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned char bFirst =TRUE;
			unsigned int j;
			unsigned long ulTotalQty = 0;
			unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
			sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
			for(j = 0; j < productCnt; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == stStock[j].categoryCode)
				{
					unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
					if(TRUE == bFirst)
					{
						sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						//PrintBuffer(gl_pLangSelPiont[STR_SOUS_TITRE_PRODUCT], PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint,"%-10s%7s%11s ",
							gl_pLangSelPiont[STR_PXUNITAIRE], 
							gl_pLangSelPiont[STR_NB],
							gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					PrintBuffer(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szNameProduct, 
						PRN_LEFT, SIZE_NORMAL_BOLD, 0);
					vFormattageAmount(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, szAmount);
					sprintf(szBufferPrint,"%-10s %5d",szAmount, stStock[j].cnt);
					{
						unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
						sprintf(szQuantity, "%d", stStock[j].cnt);
						PubAscMul(gl_SavParam.stSavParamProducts[stStock[j].productIndex].szAmount, 
							szQuantity, szAmount);
						PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
						PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
						SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
						if(atol(szAmount) != 0)
							PubTrimHeadChars(szAmount, '0');
						else
							strcpy(szAmount, "0");
					}
					ulTotalQty += stStock[j].cnt;
					vFormattageAmount(szAmount, szAmount);
					if(strlen(szAmount) > 10)
					{
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint, "%sSR", szAmount);
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
					}
					else
					{
						unsigned char szTmpBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
						sprintf(szTmpBuff,"%s%10s SR",szBufferPrint, szAmount);
						PrintBuffer(szTmpBuff, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					}
					
					++ii;    ///////...........
				}
			}
			if(TRUE != bFirst) // print total
			{
				unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
				Prn_A_LineOf("-");
				if(atol(szTotalAmt) != 0)
					PubTrimHeadChars(szTotalAmt, '0');
				else
					strcpy(szTotalAmt, "0");
				vFormattageAmount(szTotalAmt, szAmount);
				if(strlen(szAmount) > 10)
				{
					sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL], ulTotalQty);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					sprintf(szBufferPrint, "%sSR", szAmount);
					PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
				}
				else
				{
					sprintf(szBufferPrint, "%-10s %5lu %10sSR", gl_pLangSelPiont[STR_TOTAL], 
						ulTotalQty, szAmount);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
				}
				Jump_Lines(1);
				ulTotalQty_All += ulTotalQty;
				PubAddHeadChars(szTotalAmt_All, SIZE_MAX_AMOUNT, '0');
				PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
				SafeAscAdd(szTotalAmt_All, szTotalAmt, SIZE_MAX_AMOUNT);
				if(atol(szTotalAmt_All) != 0)
					PubTrimHeadChars(szTotalAmt_All, '0');
				else
					strcpy(szTotalAmt_All, "0");
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				ulTotalQty = 0;
			}
		}

		//Jump_Lines(1);
		Prn_A_LineOf("*");

		if(0 == productCnt)
		{
			PrintBuffer("Empty", PRN_MIDD, SIZE_NORMAL,0);
		}
		else
		{
			vFormattageAmount(szTotalAmt_All, szAmount);
			if(strlen(szAmount) > 10)
			{
				sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL], ulTotalQty_All);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
				sprintf(szBufferPrint, "%sSR", szAmount);
				PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			}
			else
			{
				sprintf(szBufferPrint, "%-10s %5lu %10sSR", gl_pLangSelPiont[STR_TOTAL], 
					ulTotalQty_All, szAmount);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
			}
		}

		Prn_A_LineOf("*");
		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}
void Prn_ReportCommer(void)
{
	typedef struct _tagTmp
	{
		unsigned char Id[SIZE_MAX_CODE_PRODUCT + 1];
		unsigned int cnt;
		int productIndex;
		int categoryCode;
	}TMP_Stock;
	TMP_Stock stStock[NB_MAX_CODE_STOCK];
	USER_INFO szUsers[20];
	unsigned int i, ii, productCnt = 0, userNums = 0;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];

	unsigned long ulTotalQty_All = 0;
	unsigned char szTotalAmt_All[SIZE_MAX_AMOUNT + 1];
	STOCK stock;
	ReadStock_All(&stock);

	memset(stStock, 0, sizeof(stStock));
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		productCnt = 0; userNums = 0;
		ulTotalQty_All = 0;
		sprintf(szTotalAmt_All, "%.*d", SIZE_MAX_AMOUNT, 0);

		memset(stStock, 0, sizeof(stStock));
		memset(szUsers, 0, sizeof(szUsers));

		Prn_Title_DateTime();
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_REPORT], PRN_MIDD, SIZE_LARGE_BOLD,0);
		PrintBuffer(gl_pLangSelPiont[STR_MANAGER], PRN_MIDD, SIZE_LARGE_BOLD,0);
		//Jump_Lines(1);
		
		if(OK != GetUsersList(szUsers, &userNums))
		{
			Prn_A_LineOf("*");
			PrintBuffer("Empty", PRN_MIDD, SIZE_NORMAL,0);
		}
		else
		{
			sprintf(szBufferPrint,"%-9s%8s%11s ",
				gl_pLangSelPiont[STR_CASHIER], 
				gl_pLangSelPiont[STR_NB],
				gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
			Prn_A_LineOf("-");

			for(ii = 0; ii < userNums; ++ii)
			{
				unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
				unsigned long ulTotalQty = 0;
				unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				memset(stStock, 0, sizeof(stStock));
				productCnt = 0;
				// Modified by Kim_LinHB 2013-3-23 v4.56
				if(gl_MaintenaceParam.ucMerchantMode != ONLINE)
				{
					for(i = 0; i < NB_MAX_CODE_STOCK; ++i)
					{
						unsigned char bNew = TRUE;
						unsigned int j;
						if(CODE_RESTOCK_SOLD == stock.stStock[i].ucProductState &&
							0 == strcmp(szUsers[ii].szName, stock.stStock[i].szCashierId))
						{
							for(j = 0; j < productCnt; ++j)
							{
								if(0 == strcmp(stStock[j].Id, stock.stVoucher[i].szProductId))
								{	
									++stStock[j].cnt;
									bNew = FALSE;
									break;
								}
							}
							if (bNew)
							{
								strcpy(stStock[productCnt].Id ,stock.stVoucher[i].szProductId);
								stStock[productCnt].cnt = 1;
								++productCnt;
							}
						}
					}
				}
				// Modify End
				if(gl_MaintenaceParam.ucMerchantMode != OFFLINE)
				{
					TRANS_DATA stTmp;
					int fd = open(FILE_TRANS_OL, O_RDWR);
					if(fd >= 0)
					{
						long len = filesize(FILE_TRANS_OL);
						seek(fd , 0, SEEK_SET);
						while(len > 0)
						{
							unsigned char bNew = TRUE;
							unsigned int j;
							read(fd, (void *)&stTmp, sizeof(TRANS_DATA));

							if(0 == strcmp(szUsers[ii].szName, stTmp.szCashier))
							{
								for(j = 0; j < productCnt; ++j)
								{
									if(0 == strcmp(stStock[j].Id ,stTmp.stVoucher.szProductId))
									{	
										++stStock[j].cnt;
										bNew = FALSE;
										break;
									}
								}
								if (bNew)
								{
									strcpy(stStock[productCnt].Id, stTmp.stVoucher.szProductId);
									stStock[productCnt].cnt = 1;
									++productCnt;
								}
							}
							len -= sizeof(TRANS_DATA);
						}
						close(fd);
					}
					else
						close(fd);
				}

				for(i = 0; i < productCnt; ++i)
				{
					unsigned int j;
					for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
					{
						if(0 == strcmp(stStock[i].Id, gl_SavParam.stSavParamProducts[j].szCodeProduct))
						{
							stStock[i].productIndex = j;
							stStock[i].categoryCode = gl_SavParam.stSavParamProducts[j].uiCodeCategory;
							break;
						}
					}
				}

				for(i = 0; i < productCnt; ++i)
				{
					sprintf(szQuantity, "%d", stStock[i].cnt);
					PubAscMul(gl_SavParam.stSavParamProducts[stStock[i].productIndex].szAmount, 
						szQuantity, szAmount);
					PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
					PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
					SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
					if(atol(szAmount) != 0)
						PubTrimHeadChars(szAmount, '0');
					else
						strcpy(szAmount, "0");
					ulTotalQty += stStock[i].cnt;
				}
				if(atol(szTotalAmt) != 0)
					PubTrimHeadChars(szTotalAmt, '0');
				else
					strcpy(szTotalAmt, "0");
				vFormattageAmount(szTotalAmt, szAmount);
				if(strlen(szAmount) > 10)
				{
					// Modified by Kim_LinHB 2013-4-3 v4.57 
					//fix from gl_pLangSelPiont[STR_TOTAL] to szUsers[ii].szName
					sprintf(szBufferPrint, "%-10s %5lu", szUsers[ii].szName, ulTotalQty);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					sprintf(szBufferPrint, "%sSR", szAmount);
					PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
				}
				else
				{
					sprintf(szBufferPrint, "%-10s %5lu %10sSR", szUsers[ii].szName, 
						ulTotalQty, szAmount);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
				}
				ulTotalQty_All += ulTotalQty;
				PubAddHeadChars(szTotalAmt_All, SIZE_MAX_AMOUNT, '0');
				PubAddHeadChars(szTotalAmt, SIZE_MAX_AMOUNT, '0');
				SafeAscAdd(szTotalAmt_All, szTotalAmt, SIZE_MAX_AMOUNT);
				if(atol(szTotalAmt_All) != 0)
					PubTrimHeadChars(szTotalAmt_All, '0');
				else
					strcpy(szTotalAmt_All, "0");
			}
			Prn_A_LineOf("*");
			vFormattageAmount(szTotalAmt_All, szAmount);
			if(strlen(szAmount) > 10)
			{
				sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL], ulTotalQty_All);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
				sprintf(szBufferPrint, "%sSR", szAmount);
				PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			}
			else
			{
				sprintf(szBufferPrint, "%-10s %5lu %10sSR", gl_pLangSelPiont[STR_TOTAL], 
					ulTotalQty_All, szAmount);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
			}
		}

		if(MIX == gl_MaintenaceParam.ucMerchantMode)
		{
			unsigned int cnt[NB_MAX_PRODUCTS];
			TRANS_DATA_CASH stTmp;
			unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
			unsigned long ulTotalQty_Cash = 0;
			unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];
			unsigned char szTmpAmt[SIZE_MAX_AMOUNT+1];//add by ivy 2013/06/09 v4.60

			long len = filesize(FILE_CASHORDER); // Modified by Kim_LinHB 2013-9-24 v4.70
			int fd = open(FILE_CASHORDER, O_RDWR);

			memset(cnt, 0 , sizeof(cnt));
			sprintf(szAmountWithDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);
			sprintf(szAmountWithoutDiscount, "%.*d", SIZE_MAX_AMOUNT, 0);

			if(fd >= 0)
			{
				seek(fd , 0, SEEK_SET);
				while(len > 0)
				{
					read(fd, (void *)&stTmp, sizeof(TRANS_DATA_CASH));

					for(i = 0; i < stTmp.ucProductNum; ++i)
					{
						unsigned int j;
						for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
						{
							if(0 == strcmp(stTmp.stProducts[i].szCodeProduct, gl_SavParam.stSavParamProducts[j].szCodeProduct))
							{	
								cnt[j] += atoi(stTmp.stProducts[i].szQuantity);
								break;
							}
						}
					}
					len -= sizeof(TRANS_DATA_CASH);
					//modify by ivy 2013/06/09 v4.60
					strcpy(szTmpAmt, stTmp.szAmountWithDiscount);
					PubAddHeadChars(szTmpAmt, SIZE_MAX_AMOUNT, '0');
					SafeAscAdd(szAmountWithDiscount, 
						szTmpAmt,
						SIZE_MAX_AMOUNT);
					strcpy(szTmpAmt, stTmp.szAmountWithoutDiscount);
					PubAddHeadChars(szTmpAmt, SIZE_MAX_AMOUNT, '0');
					SafeAscAdd(szAmountWithoutDiscount,
						szTmpAmt,
						SIZE_MAX_AMOUNT);
					//modify end
				}
				close(fd);
			}
			else
				close(fd);

			Jump_Lines(1);
			PrintBuffer("Cash Order", PRN_MIDD, SIZE_NORMAL_BOLD,0);
			Prn_A_LineOf("*");
			for (i = 0; i < gl_SavParam.ucCategoryNum; ++i)
			{
				unsigned int j;
				unsigned char bFirst =TRUE;
				unsigned long ulTotalQty = 0;
				unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);

				for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
				{
					if(0 == cnt[j])
						continue;

					if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamProducts[j].uiCodeCategory)
					{
						unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
						if(TRUE == bFirst)
						{
							sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%-12s%5s%12s",
								gl_pLangSelPiont[STR_PXUNITAIRE], 
								gl_pLangSelPiont[STR_NB],
								gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
							Prn_A_LineOf("-");
							bFirst = FALSE;
						}
						PrintBuffer(gl_SavParam.stSavParamProducts[j].szNameProduct, 
								PRN_LEFT, SIZE_NORMAL_BOLD, 0);
						sprintf(szBufferPrint, "-%d.%02d%s", 
							atoi(gl_SavParam.stSavParamProducts[j].szDiscount) / 100 ,
							atoi(gl_SavParam.stSavParamProducts[j].szDiscount) % 100,
							gl_pLangSelPiont[STR_PERC_DISCOUNT]);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
						vFormattageAmount(gl_SavParam.stSavParamProducts[j].szAmount, szAmount);
						{
							unsigned char szTmpBuff[200];
							sprintf(szTmpBuff,"%s %s",szAmount, "SR");
							sprintf(szBufferPrint,"%-10s %5d",szTmpBuff, cnt[j]);
						}
						sprintf(szQuantity, "%d", cnt[j]);
						PubAscMul(gl_SavParam.stSavParamProducts[j].szAmount, 
							szQuantity, szAmount);
						PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
						SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
						if(atol(szAmount) != 0)
							PubTrimHeadChars(szAmount, '0');
						else
							strcpy(szAmount, "0");
						ulTotalQty += cnt[j];
						vFormattageAmount(szAmount, szAmount);
						if(strlen(szAmount) > 10)
						{
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							sprintf(szBufferPrint,"%sSR",szAmount);
							PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
						}
						else
						{
							unsigned char szTmpBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
							sprintf(szTmpBuff,"%s %10sSR",szBufferPrint, szAmount);
							PrintBuffer(szTmpBuff, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						}
					}
				}
				if(TRUE != bFirst) // print total
				{
					unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
					Prn_A_LineOf("-");
					if(atol(szTotalAmt) != 0)
						PubTrimHeadChars(szTotalAmt, '0');
					else
						strcpy(szTotalAmt, "0");
					vFormattageAmount(szTotalAmt, szAmount);
					if(strlen(szAmount) > 10)
					{
						sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL],ulTotalQty);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint, "%s %s", szAmount, "SR");
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
					}
					else
					{
						sprintf(szBufferPrint, "%-10s %5lu %10s%s", gl_pLangSelPiont[STR_TOTAL],ulTotalQty, szAmount, "SR");
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					}
					Jump_Lines(1);
					ulTotalQty_Cash += ulTotalQty;
					sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
					ulTotalQty = 0;
				}
			}
			PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			sprintf(szBufferPrint, "%-21s%8lu", gl_pLangSelPiont[STR_PINS_QUANTITY], ulTotalQty_Cash);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

			//add by ivy 2013/06/09 v4.60
			if(atol(szAmountWithoutDiscount) != 0)
				PubTrimHeadChars(szAmountWithoutDiscount, '0');
			else
				strcpy(szAmountWithoutDiscount, "0");
			//add end
			// Removed by Kim_LinHB 2013-9-24 v4.70
// 			PrintBuffer(gl_pLangSelPiont[STR_FACE_VALUE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
// 			vFormattageAmount(szAmountWithoutDiscount, szBufferPrint);
// 			sprintf(szBufferPrint, "%s SR", szBufferPrint);
// 			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
			//add by ivy 2013/06/09 v4.60
			if(atol(szAmountWithDiscount) != 0)
				PubTrimHeadChars(szAmountWithDiscount, '0');
			else
				strcpy(szAmountWithDiscount, "0");
			//add end
			PrintBuffer(gl_pLangSelPiont[STR_PAID], PRN_LEFT, SIZE_NORMAL_BOLD,0);
			vFormattageAmount(szAmountWithDiscount, szBufferPrint);
			strcat(szBufferPrint, " SR");
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
		}
		Prn_A_LineOf("*");
		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_ManagerSetting(void)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_MANAGER], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		PrintBuffer(gl_pLangSelPiont[STR_TITRE_TICKET_PARAMETRE], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		//Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_ID_COMM], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		PrintBuffer(gl_MaintenaceParam.stManage.szName, PRN_LEFT, SIZE_NORMAL,0);
		//Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_SOUS_TITRE_PRODUCT], PRN_LEFT, SIZE_NORMAL_BOLD,0);

		for (i = 0; i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned char bFirst = TRUE;
			unsigned int j;

			for(j = 0; j < gl_SavParam.ucProductsNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamProducts[j].uiCodeCategory)
				{
					if(bFirst)
					{
						sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					PrintBuffer(gl_SavParam.stSavParamProducts[j].szNameProduct, 
						PRN_LEFT, SIZE_NORMAL_BOLD, 0);
				}
			}
			if(!bFirst)
				Jump_Lines(1);
		}

		PrintBuffer(gl_pLangSelPiont[STR_SOUS_TITRE_TERMINAL], PRN_LEFT, SIZE_NORMAL_BOLD,0);

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_TYPE_IMPRIMANTE_Min], gl_MaintenaceParam.szPrintertype);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_CancelSale(TRANS_DATA stTran, unsigned char bPreview)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		if(bPreview)
		{
			PrintBuffer(gl_pLangSelPiont[STR_PROPOSITION], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(gl_pLangSelPiont[STR_ENVOI_ANNULATION], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}
		else
		{
			PrintBuffer(gl_pLangSelPiont[STR_RECAPITULATIF], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(gl_pLangSelPiont[STR_ANNUL_VENTE], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}

		//Jump_Lines(1);

		
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NOM_CATEGORY], 
			gl_SavParam.stSavParamCategory[stTran.ucCategory_Index].szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer(gl_SavParam.stSavParamProducts[stTran.ucProducts_Index].szNameProduct, 
			PRN_LEFT, SIZE_NORMAL, 0);
		
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NUM_TRANS], 
			stTran.szProvisionID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %04d", gl_pLangSelPiont[STR_TRACE_NO], 
			stTran.usTraceNo);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer(gl_pLangSelPiont[STR_No_SERIE], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer(stTran.stVoucher.szSerial, PRN_MIDD, SIZE_NORMAL,0);

		sprintf(szBufferPrint, "%s %s%2.2s/%2.2s/%2.2s%2.2s:%2.2s", gl_pLangSelPiont[STR_Vendu_le], 
			PRE_YEAR, stTran.szDateTime + 4, stTran.szDateTime + 2, stTran.szDateTime, 
			stTran.szDateTime + 6, stTran.szDateTime + 8);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_CancelCashOrder(TRANS_DATA_CASH stTran, unsigned char bPreview)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		if(bPreview)
		{
			PrintBuffer(gl_pLangSelPiont[STR_PROPOSITION], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(gl_pLangSelPiont[STR_ENVOI_ANNULATION], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}
		else
		{
			PrintBuffer(gl_pLangSelPiont[STR_RECAPITULATIF], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(gl_pLangSelPiont[STR_ANNUL_VENTE], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		}

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_NUM_TRANS], 
			stTran.szProvisionID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %04d", gl_pLangSelPiont[STR_TRACE_NO], 
			stTran.usTraceNo);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

		// Removed by Kim_LinHB 2013-9-24 v4.70
// 		PrintBuffer(gl_pLangSelPiont[STR_FACE_VALUE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
// 		vFormattageAmount(stTran.szAmountWithoutDiscount, szBufferPrint);
// 		sprintf(szBufferPrint, "%s SR", szBufferPrint);
// 		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

		PrintBuffer(gl_pLangSelPiont[STR_PAID], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		vFormattageAmount(stTran.szAmountWithDiscount, szBufferPrint);
		strcat(szBufferPrint, " SR");
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_Reclamation(TRANS_DATA stTran)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_ENVOI_RECLAMATION], PRN_MIDD, SIZE_NORMAL_BOLD,0);

		//Jump_Lines(1);

		for(i = 0; i < gl_SavParam.ucProductsNum; ++i)
		{
			if(0 == strcmp(stTran.stVoucher.szProductId, gl_SavParam.stSavParamProducts[i].szCodeProduct))
			{
				PrintBuffer(gl_pLangSelPiont[STR_PRODUCT], PRN_LEFT, SIZE_NORMAL_BOLD,0);
				PrintBuffer(gl_SavParam.stSavParamProducts[i].szNameProduct, 
					PRN_MIDD, SIZE_NORMAL, 0);
				PrintBuffer(gl_pLangSelPiont[STR_No_SERIE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
				PrintBuffer(stTran.stVoucher.szSerial, PRN_MIDD, SIZE_NORMAL,0);
				break;
			}
		}

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_Balance(unsigned char *pValue)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_COMM], gl_MaintenaceParam.stManage.szName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont [STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
		Jump_Lines(1);

		//Prn_A_LineOf("*");
		PrintBuffer(gl_pLangSelPiont[STR_SOLDE], PRN_MIDD, SIZE_NORMAL_BOLD,0);

		PrintBuffer(gl_pLangSelPiont[STR_ID_COMM], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		PrintBuffer(gl_MaintenaceParam.stManage.szName, PRN_MIDD, SIZE_NORMAL,0);

		PrintBuffer(gl_pLangSelPiont[STR_NAME], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		// what?
		PrintBuffer(gl_pLangSelPiont[STR_CREDIT_AUTORISE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		PrintBuffer("0 SR", PRN_MIDD, SIZE_NORMAL,0);

		PrintBuffer(gl_pLangSelPiont[STR_SOLDE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		vFormattageAmount(pValue, szBufferPrint);
		strcat(szBufferPrint, " SR");
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL,0);

		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_Mail(const MAIL *pstMail, unsigned short Size)
{
	//unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned int i;
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		// Removed by Kim_LinHB 2014-1-23 v4.82
		//Prn_Title_DateTime();

		//PrintBuffer("MAIL", PRN_MIDD, SIZE_NORMAL_BOLD,0);

		for(i = 0; i < Size; ++i)
		{
			unsigned int j;
			// Removed by Kim_LinHB 2014-1-23 v4.82
// 			sprintf(szBufferPrint, "%s:%s%2.2s/%2.2s/%2.2s %2.2s:%2.2s", "Modified on", PRE_YEAR,
// 				pstMail[i].szModifiedOn,
// 				pstMail[i].szModifiedOn + 2,
// 				pstMail[i].szModifiedOn + 4,
// 				pstMail[i].szModifiedOn + 6,
// 				pstMail[i].szModifiedOn + 8);
// 			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			PrintBuffer(pstMail[i].szCaption, PRN_MIDD, SIZE_NORMAL,0);
			for(j = 0; j < pstMail[i].usContentNums; ++j)
			{
				PrintBuffer(pstMail[i].szContent[j], PRN_LEFT, SIZE_NORMAL,0);
			}
			Prn_A_LineOf("-- ");
		}
		// Removed by Kim_LinHB 2014-1-23 v4.82
		//PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		//PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

void Prn_CashOrder(TRANS_DATA_CASH stCurTran, unsigned char Mode)
{
	unsigned long ulTotalQty_All = 0;
	unsigned int i, ii;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		ulTotalQty_All = 0;

		PrintBuffer("CASH ORDER", PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(1);

		switch(Mode)
		{
		case PRN_PREVIEW:
			PrintBuffer(gl_pLangSelPiont[STR_ORDER_PREVIEW], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			break;
		case PRN_CUSTOMER:
			PrintBuffer(gl_pLangSelPiont[STR_CUSTOMER_COPY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(gl_pLangSelPiont[STR_ORDER_VALIDATED], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(stCurTran.szProvisionID, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer("(SR)", PRN_MIDD, SIZE_NORMAL_BOLD,0);/////////////
			break;
		case PRN_MERCHANT:
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(gl_pLangSelPiont[STR_ORDER_VALIDATED], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer(stCurTran.szProvisionID, PRN_MIDD, SIZE_NORMAL_BOLD,0);
			PrintBuffer("(SR)", PRN_MIDD, SIZE_NORMAL_BOLD,0);/////////////
			break;
		}

		for (i = 0, ii = 0; ii < stCurTran.ucProductNum && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			unsigned long ulTotalQty = 0;
			unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
			sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);

			for(j = 0; j < stCurTran.ucProductNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == stCurTran.stProducts[j].uiCodeCategory)
				{
					unsigned int k;
					unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
					if(TRUE == bFirst)
					{
						sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
						sprintf(szBufferPrint,"%-12s%5s%12s",
							gl_pLangSelPiont[STR_PXUNITAIRE], 
							gl_pLangSelPiont[STR_NB],
							gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					for(k = 0; k < gl_SavParam.ucProductsNum; ++k)
					{
						if(0 == strcmp(stCurTran.stProducts[j].szCodeProduct, gl_SavParam.stSavParamProducts[k].szCodeProduct))
						{
							PrintBuffer(gl_SavParam.stSavParamProducts[k].szNameProduct, 
								PRN_LEFT, SIZE_NORMAL_BOLD, 0);
							sprintf(szBufferPrint, "-%d.%02d%s", 
								atoi(gl_SavParam.stSavParamProducts[k].szDiscount) / 100 ,
								atoi(gl_SavParam.stSavParamProducts[k].szDiscount) % 100,
								gl_pLangSelPiont[STR_PERC_DISCOUNT]);
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
							vFormattageAmount(gl_SavParam.stSavParamProducts[k].szAmount, szAmount);
							{
								unsigned char szTmpBuff[200];
								sprintf(szTmpBuff,"%s %s",szAmount, "SR");
								sprintf(szBufferPrint,"%-10s %5s",szTmpBuff, stCurTran.stProducts[j].szQuantity);
							}
							PubAscMul(gl_SavParam.stSavParamProducts[k].szAmount, 
								stCurTran.stProducts[j].szQuantity, szAmount);
							PubAddHeadChars(szAmount, SIZE_MAX_AMOUNT, '0');
							SafeAscAdd(szTotalAmt, szAmount, SIZE_MAX_AMOUNT);
							if(atol(szAmount) != 0)
								PubTrimHeadChars(szAmount, '0');
							else
								strcpy(szAmount, "0");
							ulTotalQty += atoi(stCurTran.stProducts[j].szQuantity);
							vFormattageAmount(szAmount, szAmount);
							if(strlen(szAmount) > 10)
							{
								PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
								sprintf(szBufferPrint,"%sSR",szAmount);
								PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
							}
							else
							{
								unsigned char szTmpBuff[SIZE_MAX_POLICE_DEMI + 1 + 1];
								sprintf(szTmpBuff,"%s %10sSR",szBufferPrint, szAmount);
								PrintBuffer(szTmpBuff, PRN_LEFT, SIZE_NORMAL_BOLD,0);
							}

							++ii;    ///////...........
							break;
						}
					}
				}
			}
			if(TRUE != bFirst) // print total
			{
				unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
				Prn_A_LineOf("-");
				if(atol(szTotalAmt) != 0)
					PubTrimHeadChars(szTotalAmt, '0');
				else
					strcpy(szTotalAmt, "0");
				vFormattageAmount(szTotalAmt, szAmount);
				if(strlen(szAmount) > 10)
				{
					sprintf(szBufferPrint, "%-10s %5lu", gl_pLangSelPiont[STR_TOTAL],ulTotalQty);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
					sprintf(szBufferPrint, "%s %s", szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
				}
				else
				{
					sprintf(szBufferPrint, "%-10s %5lu %10s%s", gl_pLangSelPiont[STR_TOTAL],ulTotalQty, szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);
				}
				Jump_Lines(1);
				ulTotalQty_All += ulTotalQty;
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				ulTotalQty = 0;
			}
		}

		//Jump_Lines(1);
		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD,0);
		sprintf(szBufferPrint, "%-21s%8lu", gl_pLangSelPiont[STR_PINS_QUANTITY], ulTotalQty_All);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

		// Removed by Kim_LinHB 2013-9-24 v4.70
// 		PrintBuffer(gl_pLangSelPiont[STR_FACE_VALUE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
// 		vFormattageAmount(stCurTran.szAmountWithoutDiscount, szBufferPrint);
// 		sprintf(szBufferPrint, "%s SR", szBufferPrint);
// 		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);

		PrintBuffer(gl_pLangSelPiont[STR_PAID], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		vFormattageAmount(stCurTran.szAmountWithDiscount, szBufferPrint);
		strcat(szBufferPrint, " SR");
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD,0);
		

		//Jump_Lines(1);
		if (PRN_MERCHANT == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_SIGNATURE], PRN_LEFT, SIZE_NORMAL_BOLD,0);
		else
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_SIGNATURE], PRN_LEFT, SIZE_NORMAL_BOLD,0);

		Jump_Lines(3);	// for SIGNATURE
		Prn_A_LineOf("*");
		// Added by Kim_LinHB 2013-9-24 v4.70
		if(PRN_MERCHANT == Mode && filesize(FILE_CASHORDER) > sizeof(TRANS_DATA_CASH) * 100)
		{
			PrintBuffer("REPORT FILE FOR CASH ORDER IS FULL", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PrintBuffer("PLEASE RUN ERASE REPORT", PRN_LEFT, SIZE_NORMAL_BOLD,0);
			PrintBuffer("WHEN TURNING OFF SERVICE", PRN_LEFT, SIZE_NORMAL_BOLD,0);
		}
		// Add End
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_SettDownload(unsigned char Ret, RESPONSE_HEADER_TAG stResp)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	sg_ucPrintLineCnt = 0;   //added by Allen 2016-11-25 
	
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint, "%s%s", gl_pLangSelPiont[STR_ID_TERMINAL_Min], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT,SIZE_NORMAL,0);
		Jump_Lines(1);
		
		PrintBuffer(gl_pLangSelPiont[STR_TELEPARAMETRAGE_PRINT], PRN_MIDD, SIZE_NORMAL_BOLD,0);

		if(Ret != ErreurAuthentification)
		{
			Prn_A_LineOf("*");
	 		
			if (OK == Ret)
				PrintBuffer(gl_pLangSelPiont[STR_SUCCES], PRN_MIDD, SIZE_NORMAL_BOLD,0);
			else
				PrintBuffer(gl_pLangSelPiont[STR_ECHEC], PRN_MIDD, SIZE_NORMAL_BOLD,0);

			sprintf(szBufferPrint, "%s%d", gl_pLangSelPiont[STR_Version_Category], gl_SavParam.ucCategoryNum);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			sprintf(szBufferPrint, "%s%d", gl_pLangSelPiont[STR_Version_Product], gl_SavParam.ucProductsNum);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);

			if(fexist(FILE_USERS) >= 0)
				sprintf(szBufferPrint, "%s%d", "Release Users      :", filesize(FILE_USERS)/sizeof(USER_INFO) + 1);
			else
				sprintf(szBufferPrint, "%s%d", "Release Users      :", 0);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL,0);
			Prn_A_LineOf("*");
		}
		if (OK != Ret)
		{
			// Print de l'abandon du rechargement
			Prn_A_LineOf("*");
			PrintBuffer(gl_pLangSelPiont[STR_TICKET_ABANDON],PRN_LEFT, SIZE_NORMAL_BOLD,0);

			//v4.38
			if(strlen(stResp.szMessage) > 0)
				PrintBuffer(stResp.szMessage, PRN_LEFT, SIZE_NORMAL_BOLD,0);

			// Code Retour
			sprintf(szBufferPrint,"%s:%d", gl_pLangSelPiont[STR_ERROR], Ret);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD,0);

			Prn_A_LineOf("*");
		}
		PrintBuffer(gl_pLangSelPiont[STR_POWERED_BY], PRN_LEFT, SIZE_NORMAL,0);
		PrintBuffer("INNOVA " APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD,0);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.56
	}while(sg_bNoPaperCountinue);
}

//////////////////////////////////////////////////////////////////////////

static void MyPrnInit(void)
{
	// Modified by Kim_LinHB 2013-3-22 v4.56
#ifdef _S_SERIES_
	if(0 == sg_ucPrintLineCnt)
#else
	if(1 == sg_ucPrintDirectly)
#endif
	{
		PrnInit();
	}
	// Modify End
	//v4.35	PrnSpaceSet(2,0);  
	if (gl_MaintenaceParam.bDoublePaper)		//v4.13
	{
		PrnLeftIndent(LEFT_INDENT_OF_PRINT); 
		if (_TERMINAL_P80_ == gl_TermMode)   //v4.29
		{
			PrnSetGray(3);  //50-100 means 50%--100% of Gray(3)  PrnSetGray(100)=PrnSetGray(3)  //v4.15
		}
		else if (_TERMINAL_P90_ == gl_TermMode)   //v4.30
		{
			PrnSetGray(3);  //50-500 means 50%--500% of Gray(1)   PrnSetGray(300)=PrnSetGray(3)
		}		
		else
		{
			PrnSetGray(3);
		}
	}
	// Added by Kim_LinHB 2013-3-26 v4.56
	else
	{
		PrnLeftIndent(0);
	}
	// Add End
}

static ST_FONT sg_SmallFont = {CHARSET_GREEK, 8, 16, 0, 0}; // Modified by Kim_LinHB 2014-4-30
static ST_FONT sg_SmallFont_Bold = {CHARSET_RUSSIAN, 8, 16, 0, 0}; // Modified by Kim_LinHB 2014-4-30
static ST_FONT sg_NormalFont = {CHARSET_WEST, 12, 24, 0, 0};
static ST_FONT sg_NormalFont_Bold = {CHARSET_WEST, 12, 24, 1, 0};
static ST_FONT sg_LargeFont = {CHARSET_WEST, 12, 24, 0, 0};
static ST_FONT sg_LargeFont_Bold = {CHARSET_WEST, 12, 24, 1, 0};

void PrintBuffer(const unsigned char *pucBuffer,unsigned char Position, unsigned char ucType , short step)
{
	unsigned char ucSingleWidth, ucMultiWidth, ucSpaceWidth;
	unsigned short uiStrLen = 0;
	unsigned short uiPixelsNums = 0;
	unsigned char ucRet = 0;

	if(sg_bNoPaperCountinue)
		return;

	if(FALSE == sg_bCanCelPrn)
	{
		MyPrnInit();
		PrnSpaceSet(1,0); //v4.35  PrnSpaceSet(2,0)
		PrnDoubleWidth(0,0);
		PrnDoubleHeight(0,0);
		ucSpaceWidth = 1;
#ifdef _S_SERIES_
		switch(ucType)
		{
		case SIZE_DEMI:
			PrnSelectFont(&sg_SmallFont, NULL);
			ucSingleWidth = sg_SmallFont.Width;
			ucMultiWidth = sg_SmallFont.Width;
			break;
		case SIZE_DEMI_BOLD:
			PrnSelectFont(&sg_SmallFont_Bold, NULL);
			ucSingleWidth = sg_SmallFont_Bold.Width;
			ucMultiWidth = sg_SmallFont_Bold.Width;
			break;
		case SIZE_NORMAL:
			PrnSelectFont(&sg_NormalFont, NULL);
			ucSingleWidth = sg_NormalFont.Width;
			ucMultiWidth = sg_NormalFont.Width;
			break;
		case SIZE_NORMAL_BOLD:
			//PrnSpaceSet(6,0);   //v4.29
			//PrnFontSet(2,1);
			PrnSelectFont(&sg_NormalFont_Bold, NULL);
			ucSingleWidth = sg_NormalFont_Bold.Width;
			ucMultiWidth = sg_NormalFont_Bold.Width;
			break;
		case SIZE_LARGE:
			PrnSpaceSet(6,0);   //v4.35 
			PrnSelectFont(&sg_LargeFont, NULL);
			ucSingleWidth = sg_LargeFont.Width * 2;
			ucMultiWidth = sg_LargeFont.Width * 2;
			PrnDoubleWidth(1,0);
			PrnDoubleHeight(1,0);
			ucSpaceWidth = 6;
			break;
		case SIZE_LARGE_BOLD:
			PrnSpaceSet(6,0);   //v4.35 
			PrnSelectFont(&sg_LargeFont_Bold, NULL);
			ucSingleWidth = sg_LargeFont_Bold.Width * 2;
			ucMultiWidth = sg_LargeFont_Bold.Width * 2;
			PrnDoubleWidth(1,0);
			PrnDoubleHeight(1,0);
			ucSpaceWidth = 6;
			break;
		}
#else
		switch(ucType)
		{
		case SIZE_DEMI:
			PrnFontSet(0, 0);
			ucSingleWidth = 8;
			ucMultiWidth = 16;
		case SIZE_DEMI_BOLD:
			PrnFontSet(0, 0);
			ucSingleWidth = 8;
			ucMultiWidth = 16;
			break;
		case SIZE_NORMAL:
			PrnFontSet(1, 1);
			ucSingleWidth = 12;
			ucMultiWidth = 24;
		case SIZE_NORMAL_BOLD:
			PrnFontSet(1, 1);
			ucSingleWidth = 12;
			ucMultiWidth = 24;
			break;
		case SIZE_LARGE:
		case SIZE_LARGE_BOLD:
			PrnSpaceSet(6,0);   //v4.35 
			PrnFontSet(1, 1);
			ucSingleWidth = 12;
			ucMultiWidth = 24;
			PrnDoubleWidth(1,0);
			PrnDoubleHeight(1,0);
			ucSpaceWidth = 6;
			break;
		}
#endif

		uiStrLen = GetStrPix(pucBuffer, ucSingleWidth, ucMultiWidth, ucSpaceWidth);
		switch(Position)
		{
		case PRN_MIDD:
			if(uiStrLen < PAPER_PIXEL)
				uiPixelsNums = (PAPER_PIXEL - uiStrLen)/2;// ucSingleWidth /2;
			break;
		case PRN_RIGHT:
			if(uiStrLen < PAPER_PIXEL)
				uiPixelsNums = (PAPER_PIXEL - uiStrLen);// / ucSingleWidth;
			break;
		case PRN_LEFT:
		default:
			uiPixelsNums = 0;
			break;
		}

		if(pucBuffer != NULL) 
		{
			if(strlen(pucBuffer) > 0)
			{
#if !defined(LHB_TEST)
//	check "no paper" status
				while(1)
				{
					if(PrnStatus() != 0x00)
					{
						unsigned char ucKey;
						ScrCls();
						DispString(gl_pLangSelPiont[STR_PLUS_DE_PAPIER], 1 | DISP_LINE_CENTER);
						DispString(gl_pLangSelPiont[STR_PAPIER_REMPLACE], DISPLAYLINE | DISP_LINE_CENTER);

						do
						{
							ucKey = getkey();
						}while (ucKey != KEYENTER);

#ifdef _S_SERIES_
						sg_ucPrintLineCnt = 0; // Added by Kim_LinHB 2013-3-23 v4.56
#endif
						if(KEYCANCEL == ucKey)
						{
							sg_bCanCelPrn = TRUE;
							return;
						}
						if(KEYENTER == ucKey)
						{
							sg_bNoPaperCountinue = TRUE;
							ScrCls();
						}
					}
					else
						break;
				}
#endif

#ifdef FARSI
				if(IfArabicString(pucBuffer))
				{
					// Modified by Kim_LinHB 2014-2-5 v4.83
					if(!IsMixStr(pucBuffer))
						Prn_Str_AR(pucBuffer, Position, ucType + 1, step);
					else
					{
						char szTmpBuffer[1024+1];
						vReversalStr(pucBuffer, szTmpBuffer);
						Prn_Str_AR(szTmpBuffer, Position, ucType + 1, step);
					}
				}
				else
				{
					// api limit(0 - 300)
					if(uiPixelsNums <= 300)
						PrnLeftIndent(uiPixelsNums);
					else
					{
						unsigned char spaces[50];
						sprintf(spaces, "%*.s", uiPixelsNums / (ucSingleWidth + ucSpaceWidth)," ");
						PrnStr((char *)spaces);
					}
					ucRet = PrnStr((char *)pucBuffer);
					PrnStep(step);
				}
#else
				if(uiPixelsNums <= 300)
					PrnLeftIndent(uiPixelsNums);
				else
				{
					unsigned char spaces[50];
					sprintf(spaces, "%*.s", uiPixelsNums / (ucSingleWidth + ucSpaceWidth)," ");
					PrnStr((char *)spaces);
				}
				ucRet = PrnStr((char *)pucBuffer);
#endif
#ifdef _S_SERIES_
				++sg_ucPrintLineCnt;
			}
			// Modified by Kim_LinHB 2013-4-8 v4.57
			PrintStart(FALSE);
			// Modify End
#else
			}
			PrintStart(TRUE);
#endif
		}
	}
	//return 0;
}

static void PrintBuffer_AR(const unsigned char *pucBuffer,unsigned char Position, unsigned char ucSize, short step)
{
	if(sg_bNoPaperCountinue)
		return;

	if(FALSE == sg_bCanCelPrn)
	{
		MyPrnInit();
		if(pucBuffer != NULL) 
		{
			Prn_Str_AR(pucBuffer, Position, ucSize + 1, step);
#ifdef _S_SERIES_
// Added by Kim_LinHB 2013/3/22 v4.56
			++sg_ucPrintLineCnt;
// Modified by Kim_LinHB 2013-4-8 v4.57
			PrintStart(FALSE);
#else
			PrintStart(TRUE);
			DelayMs(50);
#endif
			// Modify End
			// Add End
		}
	}
}

// Added by Kim_LinHB 2013/3/22 v4.56
static uchar PrintStart(unsigned char bForce)
{
	unsigned char ucRet = 0x00;
#ifdef _S_SERIES_
	
	if (bForce)
	{
		if(sg_ucPrintLineCnt > 0)
		{
			ucRet = PrnStart();
			sg_ucPrintLineCnt = 0;
		}
	}
	else
	{
		if(sg_ucPrintLineCnt >= LINE_CNT_MAX)
		{
			ucRet = PrnStart();
			sg_ucPrintLineCnt = 0;
		}
	}
#else
	if(1 == sg_ucPrintDirectly)
		ucRet = PrnStart();
#endif

	if(ucRet != 0x00)
	{
#if !defined(LHB_DEBUG) || !defined(LHB_TEST)
		//	check "no paper" status
		if(0x02 == ucRet)
		{
			unsigned char ucKey;
			ScrCls();
			DispString(gl_pLangSelPiont[STR_PLUS_DE_PAPIER], 1 | DISP_LINE_CENTER);
			DispString(gl_pLangSelPiont[STR_PAPIER_REMPLACE], DISPLAYLINE | DISP_LINE_CENTER);

			do
			{
				ucKey = getkey();
			}while (ucKey != KEYENTER);
#ifdef _S_SERIES_
			sg_ucPrintLineCnt = 0;
#endif

			if(KEYCANCEL == ucKey)
			{
				sg_bCanCelPrn = TRUE;
				return;
			}
			if(KEYENTER == ucKey)
			{
				sg_bNoPaperCountinue = TRUE;
				ScrCls();
			}
		}
		else
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print erro:\n%02x",ucRet);
			PubWaitKey(TIME_DELAY);
		}
#endif
	}
	return ucRet;
}
// Add End

static void Jump_Lines(unsigned char No)
{
	unsigned char buff[50];
	memset(buff, 0, sizeof(buff));
	memset(buff, '\n', No);
	PrintBuffer(buff, PRN_LEFT, SIZE_NORMAL,0);
}

//	´òÓ¡·Ö¸îÏß
static void Prn_A_LineOf(const unsigned char *str)
{
	int i = 0;
	int len; // Added by Kim_LinHB 2013-3-22 v4.56
	unsigned char buff[50];

	// Modified by Kim_LinHB 2013-3-22 v4.56
	memset(buff, 0, sizeof(buff));
	len = strlen(str);
	len = len > 0 ? len : 1;
	len = 29 / len;

	strcpy(buff, str);
	for (i = 1; i < len; ++i)
		strcat(buff, str);
	// Modify End

	PrintBuffer(buff, PRN_LEFT, SIZE_NORMAL,0);
}

static void Prn_Str_AR(const unsigned char *pucBuffer, unsigned char Position, unsigned char ucType, short step)
{
#ifdef FARSI
	if(0 == ucType || ucType >20)
		ucType = 3;
	if (ArInit(ucType)!=AR_SUCCESS)
	{
		while(1)
		{
			ScrCls();
			ScrPrint(0,0,0,"Error:AR_PRNDISP.FNT error\nPls download ParamFile");
			getkey();
		}
	}
	ArPrintString(Position, (unsigned char *)pucBuffer);
	PrnStep(step);
#endif
}


static unsigned char IsSingle(char p)
{
	if(~(p >> 8) == 0)  
		return 0;//´ú±í²»ÊÇºº×Ö
	return 1;  
}

//	»ñÈ¡´òÓ¡×Ö·û´®ËùÕ¼ÏñËØÊý
static unsigned short GetStrPix(const unsigned char *ucSource, unsigned int uiSingleWidth, unsigned int uiMultiWidth, unsigned int uiSpaceWidth)
{
	unsigned int sum = 0 , i = 0;
	for (i = 0 ; i < strlen((char *)ucSource) ; /*NULL*/) {
		if(IsSingle(ucSource[i])){
			++i;
			sum += uiSingleWidth + uiSpaceWidth;
		}else{
			i += 2;
			sum += uiMultiWidth + uiSpaceWidth;
		}
	}
	return (unsigned short)sum;
}

static void vFormattageAmount(unsigned char *pszAmtIn,unsigned char *pszAmtout)
{
	unsigned char ucLen = 0;
	unsigned char amt[SIZE_MAX_AMOUNT + 3 + 1];

	ucLen = strlen(pszAmtIn);

	if(ucLen > 2)
	{
	//sprintf(amt, "%d.%02d", atol(pszAmtIn) / 100, atol(pszAmtIn) % 100);
		sprintf(amt, "%.*s.%s", ucLen - 2, pszAmtIn, pszAmtIn + ucLen - 2);
	}
	else
	{
		sprintf(amt, "0.%s", pszAmtIn);
	}

	// Modified by Kim_LinHB 2013-7-3 v4.61
	if(0 == atoi(pszAmtIn))
		strcpy(pszAmtout, "0.00");
	else
		strcpy(pszAmtout, amt);
	// Modify End
}


static void vReversalStr_sub(const unsigned char *pszIn,unsigned char *pszOut)
{
	unsigned char namerev1[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char namerev2[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char *pIndex = namerev1;
	// Modified by Kim_LinHB 2013-3-23 v 4.56 unsigned int to int
	int start = strlen(pszIn) - 1; 
	int i;
	// Modify End

	strcpy(namerev1, pszIn);
	strcpy(namerev2, "");

	for(i = start; i > 0; --i)
	{
		if((isdigit(namerev1[i]) && !isdigit(namerev1[i - 1])) ||
		    (isalpha(namerev1[i]) && !isalpha(namerev1[i - 1])) ||
			(!isalnum(namerev1[i]) && isalnum(namerev1[i - 1])))
		{
			strcat(namerev2, &namerev1[i]);
			namerev1[i] = '\0';
		}
		if(1 == i)
			strcat(namerev2, &namerev1[i - 1]);
	}

	strcpy(pszOut, namerev2);
}

static void vReversalStr(const unsigned char *pszIn,unsigned char *pszOut)
{
	unsigned char namerev1[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char namerev2[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char namerev3[SIZE_MAX_POLICE_DEMI + 1 + 1]; // Added by Kim_LinHB 2014-2-12 v4.84
	unsigned char hasArabicTmp = FALSE;
	unsigned char *pIndex = namerev1;

	strcpy(namerev1, pszIn);
	strcpy(namerev2, "");
	memset(namerev3, 0, sizeof(namerev3)); // Added by Kim_LinHB 2014-2-12 v4.84
	while(1)
	{
		pIndex = strrchr(namerev1, ' ');
		if(pIndex)
		{
			//do something to reversal pIndex + 1
			// Added by Kim_LinHB 2014-2-12 v4.84
			if(IfArabicString(pIndex + 1))
			{
				if(!hasArabicTmp)
				{
					strcpy(namerev3, pIndex);
					hasArabicTmp = TRUE;
				}
				else
				{
					memmove(namerev3 + strlen(pIndex), namerev3, strlen(namerev3));
					memcpy(namerev3, pIndex, strlen(pIndex));
				}
			}
			else
			{
				if(hasArabicTmp)
				{
					strcat(namerev2, namerev3);
					strcat(namerev2, " ");
					memset(namerev3, 0, sizeof(namerev3));
					hasArabicTmp = FALSE;
				}
			// Add End v4.84
				vReversalStr_sub(pIndex + 1, pIndex + 1);
				strcat(namerev2, pIndex + 1);
				strcat(namerev2, " ");
			}
			*(pIndex)= '\0';
		}
		else
		{
			//do something to reversal namerev1
			if(hasArabicTmp)
			{
				strcat(namerev2, namerev3);
				strcat(namerev2, " ");
				memset(namerev3, 0, sizeof(namerev3));
				hasArabicTmp = FALSE;
			}
			vReversalStr_sub(namerev1, namerev1);
			strcat(namerev2, namerev1);
			break;
		}
	}
	strcpy(pszOut, namerev2);
}

// Added by Kim_LinHB 2014-2-5 v4.83
static char IsMixStr(const unsigned char *pStr)
{
	char hasLatin = 0, hasArabic = 0;
	if(!pStr)
	{
		return 0;
	}

	while(1)
	{
		if (*pStr >= 0x80)
		{
			hasArabic = 1;
		}
		else if((*pStr >= 'A' && *pStr <= 'Z') ||
				(*pStr >= 'a' && *pStr <= 'z'))
		{
			hasLatin = 1;
		}
		if (*pStr == 0)
		{
			break;			
		}
		pStr++;
	}
	if(hasArabic && hasLatin)
		return 1;
	else
		return 0;
}


static void SetPrintMode(unsigned char mode)
{
#ifndef _S_SERIES_	
	if (0 == sg_ucPrintDirectly && mode)
	{
		sg_ucPrintDirectly = 1;
		PrintStart(TRUE);
		MyPrnInit();
	}

	sg_ucPrintDirectly = mode ? 1 : 0;
#endif
}

