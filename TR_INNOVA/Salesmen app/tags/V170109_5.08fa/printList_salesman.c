#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "data_define_salesman.h"
#include "framework.h"
#include "printList_salesman.h"

static unsigned char sg_bCanCelPrn = FALSE;
static unsigned char sg_bNoPaperCountinue = FALSE;

#define LINE_CNT_MAX	50		 // Added by Kim_LinHB 2013/3/22  v4.55 prnStr buff Line once
static unsigned char sg_ucPrintLineCnt = 0; // Added by Kim_LinHB 2013-3-22 v4.55

static void MyPrnInit(void);
static void PrintBuffer(const unsigned char *pucBuffer,unsigned char Position, unsigned char ucSize);
static void PrintStart(unsigned char bForce); // Added by Kim_LinHB 2013/3/22 v4.55
// print (No) empty lines
static void Jump_Lines(unsigned char No);

static void Prn_A_LineOf(const unsigned char *str);
static void Prn_Str_AR(const unsigned char *pucBuffer, unsigned char Position, unsigned char ucType);
static unsigned char IsSingle(char p);
static unsigned short GetStrPix(const unsigned char *ucSource, unsigned int uiSingleWidth, 
						unsigned int uiMultiWidth, unsigned int uiSpaceWidth);
//static void vFormattageLongTPE_PC(long *plNum,const unsigned char * pszNum);
//modify amount(long) to fraction e.g. "1234" -> "12.34"
static void vFormattageAmount(unsigned char *pszAmtIn,unsigned char *pszAmtout);


static void Prn_Title_DateTime(void);

//////////////////////////////////////////////////////////////////////////
// modified by Allen 2015-12-30 , added "JAWWY" logo, version 5.00 
static void Prn_Title_DateTime(void)     
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szDateTime[SIZE_DATE_TIME + 1];

	sg_bCanCelPrn = FALSE;



	PrintBuffer(APP_NAME, PRN_MIDD, SIZE_NORMAL_BOLD);
	//Jump_Lines(1);

	GetCurTime(szDateTime);
	sprintf((char *)szBufferPrint, "%s%2.2s/%2.2s/%2.2s%15.2s:%2.2s", 
		PRE_YEAR, szDateTime, szDateTime + 2, szDateTime + 4, szDateTime + 6, szDateTime + 8);
	PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);
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
			//Jump_Lines(5);
			PrintBuffer("LOG", PRN_MIDD, SIZE_NORMAL_BOLD);
			if(pucLabel != NULL && strlen(pucLabel)> 0)
				PrintBuffer(pucLabel, PRN_LEFT, SIZE_NORMAL);

			if(pucBuffer != NULL && usBuffLen > 0)
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
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
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
				else
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
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			}
			Jump_Lines(END_JUMP);
			PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
		}while(sg_bNoPaperCountinue);
	}
}

void Prn_Resp(RESPONSE_HEADER_TAG stResp)
{
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();
		//Jump_Lines(1);
		PrintBuffer(gl_pLangSelPiont[STR_RESPONSE_CODE], PRN_LEFT, SIZE_NORMAL);
		PrintBuffer(stResp.szResponseCode, PRN_MIDD, SIZE_NORMAL_BOLD);
		PrintBuffer(gl_pLangSelPiont[STR_ERROR_MSG], PRN_LEFT, SIZE_NORMAL);
		PrintBuffer(stResp.szMessage, PRN_MIDD, SIZE_NORMAL_BOLD);
		/*   removed by Allen 2016-09-06
		if(0 == strcmp(stResp.szStatus, "5"))
		{
			//Jump_Lines(1);
			PrintBuffer("Please run setting download", PRN_MIDD, SIZE_NORMAL_BOLD);  
		}
		*/
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
void Prn_Order(TRANS_DATA stCurTran, unsigned char Mode)
{
	unsigned long ulTotalQty_All = 0;
	unsigned int i, ii;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	xMerchantList stMerhchant;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		memset(&stMerhchant, 0, sizeof(xMerchantList));

		ulTotalQty_All = 0;

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		if(Mode != PRN_PREVIEW)
		{
			sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_SALESMAN_NAME], gl_MaintenaceParam.szSalesmanName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		}

		if (TYPE_BARCODE == stCurTran.ucIdentifyType)
		{
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_No_SERIE], stCurTran.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}
		else
		{
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_ID_TERMINAL], stCurTran.szTermCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_MERCHANT_ID], stCurTran.szMerchantCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}
		

		if(Mode != PRN_PREVIEW)
		{
			sprintf(szBufferPrint,"%s\n", gl_pLangSelPiont[STR_MERCHANT_NAME]);
			for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
			{
				GetMerchant(FILE_MERCHANT, i, &stMerhchant);
				if(0 == strcmp(stCurTran.szMerchantID, stMerhchant.szMerchantID))
				{
					sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_MERCHANT_NAME], 
						stMerhchant.szName);
					break;
				}
			}
			
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		}

		Jump_Lines(1);

		switch(Mode)
		{
		case PRN_PREVIEW:
			PrintBuffer(gl_pLangSelPiont[STR_ORDER_PREVIEW], PRN_MIDD, SIZE_NORMAL_BOLD);
			break;
		case PRN_SALESMAN:
			PrintBuffer(gl_pLangSelPiont[STR_SALESMAN_COPY], PRN_MIDD, SIZE_NORMAL_BOLD);
			Jump_Lines(1);
			if (ID_CashOrder == stCurTran.ucTransType ||
				ID_CreditOrder == stCurTran.ucTransType)
			{
				if(ID_CashOrder == stCurTran.ucTransType)
					PrintBuffer("CASH", PRN_MIDD, SIZE_NORMAL_BOLD);
				else
					PrintBuffer("CREDIT", PRN_MIDD, SIZE_NORMAL_BOLD);
				PrintBuffer(gl_pLangSelPiont[STR_ORDER_VALIDATED], PRN_MIDD, SIZE_NORMAL_BOLD);
				PrintBuffer(stCurTran.szProvisionID, PRN_MIDD, SIZE_NORMAL_BOLD);
				PrintBuffer("(SR)", PRN_MIDD, SIZE_NORMAL_BOLD);/////////////
			}
			else
				PrintBuffer("OFFLINE PHYSICAL SALE", PRN_MIDD, SIZE_NORMAL_BOLD);
			
			break;
		case PRN_MERCHANT:
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY], PRN_MIDD, SIZE_NORMAL_BOLD);
			Jump_Lines(1);
			if (ID_CashOrder == stCurTran.ucTransType ||
				ID_CreditOrder == stCurTran.ucTransType)
			{
				if(ID_CashOrder == stCurTran.ucTransType)
					PrintBuffer("CASH", PRN_MIDD, SIZE_NORMAL_BOLD);
				else
					PrintBuffer("CREDIT", PRN_MIDD, SIZE_NORMAL_BOLD);
				PrintBuffer(gl_pLangSelPiont[STR_ORDER_VALIDATED], PRN_MIDD, SIZE_NORMAL_BOLD);
				PrintBuffer(stCurTran.szProvisionID, PRN_MIDD, SIZE_NORMAL_BOLD);
				PrintBuffer("(SR)", PRN_MIDD, SIZE_NORMAL_BOLD);/////////////
			}
			else
				PrintBuffer("OFFLINE PHYSICAL SALE", PRN_MIDD, SIZE_NORMAL_BOLD);
			break;
		}
		

		//Jump_Lines(1);

		for (i = 0, ii = 0; ii < stCurTran.uiProductNum && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			unsigned long ulTotalQty = 0;
			unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
			sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
			
			for(j = 0; j < stCurTran.uiProductNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == stCurTran.stProducts[j].uiCodeCategory)
				{
					unsigned int k;
					unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
					if(TRUE == bFirst)
					{
						sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						sprintf(szBufferPrint,"%-12s%5s%12s",
							gl_pLangSelPiont[STR_PXUNITAIRE], 
							gl_pLangSelPiont[STR_NB],
							gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					for(k = 0; k < gl_SavParam.uiProductsNum; ++k)
					{
						if(0 == strcmp(stCurTran.stProducts[j].szCodeProduct, gl_SavParam.stSavParamProducts[k].szCodeProduct))
						{
							PrintBuffer(gl_SavParam.stSavParamProducts[k].szNameProduct, 
									PRN_LEFT, SIZE_NORMAL_BOLD);
							sprintf(szBufferPrint, "-%d.%02d%s", 
								atoi(gl_SavParam.stSavParamProducts[k].szDiscount) / 100 ,
								atoi(gl_SavParam.stSavParamProducts[k].szDiscount) % 100,
								gl_pLangSelPiont[STR_PERC_DISCOUNT]);
							PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
							vFormattageAmount(gl_SavParam.stSavParamProducts[k].szAmount, szAmount);
							sprintf(szBufferPrint,"%s %s",szAmount, "SR");
							sprintf(szBufferPrint,"%-10s %5s",szBufferPrint, stCurTran.stProducts[j].szQuantity);
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
								PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
								sprintf(szBufferPrint,"%sSR",szAmount);
								PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
							}
							else
							{
								sprintf(szBufferPrint,"%s %10sSR",szBufferPrint, szAmount);
								PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
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
					sprintf(szBufferPrint, "%-10s %5d", gl_pLangSelPiont[STR_TOTAL],ulTotalQty);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
					sprintf(szBufferPrint, "%s %s", szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
				}
				else
				{
					sprintf(szBufferPrint, "%-10s %5d %10s%s", gl_pLangSelPiont[STR_TOTAL],ulTotalQty, szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
				}
				Jump_Lines(1);
				ulTotalQty_All += ulTotalQty;
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				ulTotalQty = 0;
			}
		}

		//Jump_Lines(1);
		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "%-21s%8d", gl_pLangSelPiont[STR_PINS_QUANTITY], ulTotalQty_All);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_FACE_VALUE], PRN_LEFT, SIZE_NORMAL_BOLD);
		vFormattageAmount(stCurTran.szAmountWithoutDiscount, szBufferPrint);
		sprintf(szBufferPrint, "%s SR", szBufferPrint);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_PAID], PRN_LEFT, SIZE_NORMAL_BOLD);
		vFormattageAmount(stCurTran.szAmountWithDiscount, szBufferPrint);
		sprintf(szBufferPrint, "%s SR", szBufferPrint);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);

		//Jump_Lines(1);
		if (PRN_MERCHANT == Mode)
			PrintBuffer(gl_pLangSelPiont[STR_SIGNATURE], PRN_LEFT, SIZE_NORMAL_BOLD);
		else
			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_SIGNATURE], PRN_LEFT, SIZE_NORMAL_BOLD);

		Jump_Lines(3);	// for SIGNATURE
		Prn_A_LineOf("*");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_Visit(TRANS_DATA stCurTran)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	int index;
	xMerchantList stMerchant;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		memset(&stMerchant, 0, sizeof(xMerchantList));

		index = -1;

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		if(ID_OfflineVisit == stCurTran.ucTransType)
		{
			PrintBuffer("OFFLINE VISIT", PRN_MIDD, SIZE_NORMAL_BOLD);
			sprintf(szBufferPrint, "%s%d", "Batch Number: ", stCurTran.usBatchNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}
		else
			PrintBuffer("VISIT", PRN_MIDD, SIZE_NORMAL_BOLD);

		if (TYPE_BARCODE == stCurTran.ucIdentifyType)
		{
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_No_SERIE], stCurTran.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}
		else
		{
			unsigned int i;
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_TERMINAL_ID], stCurTran.szTermCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_MERCHANT_ID],stCurTran.szMerchantCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			
			for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
			{
				GetMerchant(FILE_MERCHANT, i, &stMerchant);
				if(0 == strcmp(stCurTran.szMerchantID, stMerchant.szMerchantID))
				{
					sprintf(szBufferPrint,"%s\n%s", gl_pLangSelPiont[STR_MERCHANT_NAME],
						stMerchant.szName);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
					index = i;
					break;
				}
			}
		}

		//Jump_Lines(1);

		PrintBuffer("CLOSED SHOP", PRN_MIDD, SIZE_NORMAL_BOLD);
		if(ID_OfflineVisit == stCurTran.ucTransType)
		{
			sprintf(szBufferPrint,"%04d", gl_SavParam.uiCurOfflineNo);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);
		}
		else
		{
			PrintBuffer("INFORMATION SEND", PRN_MIDD, SIZE_NORMAL_BOLD);
			
			if(index != -1)
				PrintBuffer(stMerchant.szSerial, PRN_MIDD, SIZE_NORMAL_BOLD);
		}

		Prn_A_LineOf("*");

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
void Prn_CancelLastOrder(TRANS_DATA stCurTran)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint, "%s%d", "Batch Number: ", stCurTran.usBatchNo);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		if(ID_OfflineVisit == stCurTran.ucTransType)
		{
			PrintBuffer("CLOSED SHOP CANCELED", PRN_MIDD, SIZE_NORMAL_BOLD);
			sprintf(szBufferPrint,"%04d", gl_SavParam.uiCurOfflineNo);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);
		}
		else
		{
			sprintf(szBufferPrint, "%s %s", gl_pLangSelPiont[STR_ORDER], gl_pLangSelPiont[STR_TICKET_ABANDON]);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);
		}
		Prn_A_LineOf("*");

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
void Prn_BatchUpload(unsigned char bPreview, unsigned int totRecord, unsigned int sold, unsigned int visited)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned int uiSaleNo = 0, uiCancelSaleNo = 0, uiCloseNo = 0, uiCancelCloseNo = 0;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		uiSaleNo = 0; uiCancelSaleNo = 0; uiCloseNo = 0; uiCancelCloseNo = 0;

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_TERMINAL_ID], gl_MaintenaceParam.szTermID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint, "%s%d", "Batch Number: ", gl_SavParam.usBatchNo);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		Jump_Lines(1);

		PrintBuffer("OFFLINE BATCH", PRN_MIDD, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "%s %s", "UPLOAD", gl_pLangSelPiont[STR_SUMMARY]);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);

		//Jump_Lines(1);

		{
			int fd, fdCancel;
			TRANS_DATA tmp;
			long len = 0;
			
			if(bPreview)
			{
				fd = open(FILE_OFFLINE, O_RDWR);
				len = filesize(FILE_OFFLINE);
				while(fd >= 0)
				{
					if(len <= 0) // done
					{
						close(fd);
						break;
					}
					read(fd, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
					len -= sizeof(TRANS_DATA) - sizeof(tmp.stProducts);
					if(ID_OfflineVisit == tmp.ucTransType)
						++uiCloseNo;
					else
					{
						len -= tmp.uiProductNum * sizeof(PRODUCT_INFO);
						++uiSaleNo;
						read(fd, (void *)tmp.stProducts, tmp.uiProductNum * sizeof(PRODUCT_INFO));
					}
				}
			}

			fdCancel = open(FILE_OFF_CANCEL, O_RDWR);
			len = filesize(FILE_OFF_CANCEL);
			while(fdCancel >= 0)
			{
				if(len <= 0) // done
				{
					close(fdCancel);
					break;
				}
				read(fdCancel, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
				len -= sizeof(TRANS_DATA) - sizeof(tmp.stProducts);
				if(ID_OfflineVisit == tmp.ucTransType)
					++uiCancelCloseNo;
				else
					++uiCancelSaleNo;
			}
			close(fd);
			close(fdCancel);
		}

		if(bPreview)
		{
			sprintf(szBufferPrint, "Sale:%-*.03d", 29 -strlen("Sale:"), uiSaleNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Cancel Sale:%-*.03d", 29 -strlen("Cancel Sale:"), uiCancelSaleNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Shop Close:%-*.03d", 29 -strlen("Shop Close:"), uiCloseNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Cancel Shop Close:%-*.03d", 29 -strlen("Cancel Shop Close:"), uiCancelCloseNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Total Record:%-*.03d", 29 -strlen("Total Record:"), uiSaleNo + uiCloseNo + uiCancelCloseNo + uiCancelSaleNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Total Uploaded:%-*.03d", 29 -strlen("Total Uploaded:"), 0);
		}
		else
		{
			sprintf(szBufferPrint, "Sale:%-*.03d", 29 -strlen("Sale:"), sold);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Cancel Sale:%-*.03d", 29 -strlen("Cancel Sale:"), uiCancelSaleNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Shop Close:%-*.03d", 29 -strlen("Shop Close:"), visited);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Cancel Shop Close:%-*.03d", 29 -strlen("Cancel Shop Close:"), uiCancelCloseNo);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Total Record:%-*.03d", 29 -strlen("Total Record:"), totRecord);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			//Jump_Lines(1);
			sprintf(szBufferPrint, "Total Uploaded:%-*.03d", 29 -strlen("Total Uploaded:"), sold + visited);
		}
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		Prn_A_LineOf("-");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_Consign_info(SAV_PARAM stTmpS)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_TERMINAL_ID], gl_MaintenaceParam.szTermID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint, "%s%d", "Batch Number: ", gl_SavParam.usBatchNo);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		Jump_Lines(1);

		PrintBuffer("CONSIGNMENT INFORMATION", PRN_MIDD, SIZE_NORMAL_BOLD);

		//Jump_Lines(2);

		for (i = 0; i < stTmpS.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			for(j = 0; j < stTmpS.uiProductsNum; ++j)
			{
				if(stTmpS.stSavParamCategory[i].uiCodeCategory == stTmpS.stSavParamProducts[j].uiCodeCategory &&
					PRODUCT_PHYSICAL == stTmpS.stSavParamCategory[i].ucCategoryType)
				{
					if(TRUE == bFirst)
					{
						PrintBuffer(stTmpS.stSavParamCategory[i].szName, PRN_LEFT, SIZE_NORMAL_BOLD);
						sprintf(szBufferPrint,"%-11s %8s %8s",
							"Average", 
							"Current", 
							"Order"); // normal  29 temp
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					PrintBuffer(stTmpS.stSavParamProducts[j].szNameProduct, 
						PRN_LEFT, SIZE_NORMAL_BOLD);
					sprintf(szBufferPrint, "%-11s %8s %8s",
						(strlen(stTmpS.stSavParamProducts[j].szAverage)>0?(char *)stTmpS.stSavParamProducts[j].szAverage:"0"),
						(strlen(stTmpS.stSavParamProducts[j].szQuantity)>0?(char *)stTmpS.stSavParamProducts[j].szQuantity:"0"),
						(strlen(stTmpS.stSavParamProducts[j].szOrderNum)>0?(char *)stTmpS.stSavParamProducts[j].szOrderNum:"0"));
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
				}
			}
			if(bFirst != TRUE)
			{
				//Prn_A_LineOf("-");
				Jump_Lines(1);
 			}
		}
		if(0 == stTmpS.uiProductsNum)
		{
			Prn_A_LineOf("*");
			PrintBuffer("Empty", PRN_MIDD, DEMI_SIZE);
			Prn_A_LineOf("*");
		}

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_ConsignOrder(TRANS_DATA stCurTran, unsigned char bPreview)
{
	unsigned int i, ii;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned long ulTotalQty_All = 0;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		ulTotalQty_All = 0;

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		//Jump_Lines(1);
		sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_SALESMAN_NAME], gl_MaintenaceParam.szSalesmanName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		Jump_Lines(1);

		if(bPreview)
		{	
			PrintBuffer("CONSIGN ORDER", PRN_MIDD, SIZE_NORMAL_BOLD);
			PrintBuffer("PREVIEW", PRN_MIDD, SIZE_NORMAL_BOLD);
		}
		else
		{
			PrintBuffer("CONSIGN ORDER", PRN_MIDD, SIZE_NORMAL_BOLD);
			PrintBuffer(gl_pLangSelPiont[STR_ORDER_VALIDATED], PRN_MIDD, SIZE_NORMAL_BOLD);
			PrintBuffer(stCurTran.szProvisionID, PRN_MIDD, SIZE_NORMAL_BOLD);
		}

		//Jump_Lines(1);

		for (i = 0, ii = 0; ii < stCurTran.uiProductNum && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			unsigned long ulTotalQty = 0;
			unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
			sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
			for(j = 0; j < stCurTran.uiProductNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == stCurTran.stProducts[j].uiCodeCategory)
				{
					unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
					unsigned int k; 
					if(TRUE == bFirst)
					{
						sprintf(szBufferPrint,"%s%s",gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						sprintf(szBufferPrint,"%-12s%5s%12s",
							gl_pLangSelPiont[STR_PXUNITAIRE], 
							gl_pLangSelPiont[STR_NB],
							gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					for(k = 0; k < gl_SavParam.uiProductsNum; ++k)
					{
						if(0 == strcmp(stCurTran.stProducts[j].szCodeProduct, gl_SavParam.stSavParamProducts[k].szCodeProduct))
						{
							PrintBuffer(gl_SavParam.stSavParamProducts[k].szNameProduct, 
								PRN_LEFT, SIZE_NORMAL_BOLD);
							vFormattageAmount(gl_SavParam.stSavParamProducts[k].szAmount, szAmount);
							sprintf(szBufferPrint,"%s %s",szAmount, "SR");
							sprintf(szBufferPrint,"%-10s %5s",szBufferPrint, stCurTran.stProducts[j].szQuantity);
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
								PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
								sprintf(szBufferPrint,"%sSR",szAmount);
								PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
							}
							else
							{
								sprintf(szBufferPrint,"%s %10sSR",szBufferPrint, szAmount);
								PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
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
					sprintf(szBufferPrint, "%-10s %5d", gl_pLangSelPiont[STR_TOTAL],ulTotalQty);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
					sprintf(szBufferPrint, "%s %s", szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
				}
				else
				{
					sprintf(szBufferPrint, "%-10s %5d %10s%s", gl_pLangSelPiont[STR_TOTAL],ulTotalQty, szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
				}
				Jump_Lines(1);
				ulTotalQty_All += ulTotalQty;
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				ulTotalQty = 0;
			}
		}

		//Jump_Lines(1);
		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "%-21s%8d", gl_pLangSelPiont[STR_PINS_QUANTITY], ulTotalQty_All);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_FACE_VALUE], PRN_LEFT, SIZE_NORMAL_BOLD);
		vFormattageAmount(stCurTran.szAmountWithoutDiscount, szBufferPrint);
		sprintf(szBufferPrint, "%s SR", szBufferPrint);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
		Prn_A_LineOf("*");
	// 	PrintBuffer("Paid:", PRN_LEFT, SIZE_NORMAL_BOLD);
	// 	vFormattageAmount(stCurTran.szAmountWithDiscount, szBufferPrint);
	// 	sprintf(szBufferPrint, "%s SR", szBufferPrint);
	// 	PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);

	//	Jump_Lines(1);
	//	PrintBuffer("Merchant Signature:", PRN_LEFT, SIZE_NORMAL_BOLD);

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
void Prn_RetConsignOrder(TRANS_DATA stCurTran, unsigned char bPreview)
{
	unsigned int i, ii;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned long ulTotalQty_All = 0;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		ulTotalQty_All = 0;
		
		Prn_Title_DateTime();

		sg_bCanCelPrn = FALSE;

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_SALESMAN_NAME], gl_MaintenaceParam.szSalesmanName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		Jump_Lines(1);

		PrintBuffer("RETURN CONSIGN ORDER", PRN_MIDD, SIZE_NORMAL_BOLD);
		PrintBuffer("VALIDATED", PRN_MIDD, SIZE_NORMAL_BOLD);
		PrintBuffer(stCurTran.szProvisionID, PRN_MIDD, SIZE_NORMAL_BOLD);

		//Jump_Lines(1);

		for (i = 0, ii = 0; ii < stCurTran.uiProductNum && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			unsigned long ulTotalQty = 0;
			unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
			sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
			
			for(j = 0; j < stCurTran.uiProductNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == stCurTran.stProducts[j].uiCodeCategory)
				{
					unsigned char szAmount[SIZE_MAX_AMOUNT + 1];
					unsigned int k;
					if(TRUE == bFirst)
					{
						sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						sprintf(szBufferPrint,"%-12s%5s%12s",
							gl_pLangSelPiont[STR_PXUNITAIRE], 
							gl_pLangSelPiont[STR_NB],
							gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					
					for(k = 0; k < gl_SavParam.uiProductsNum; ++k)
					{
						if(0 == strcmp(stCurTran.stProducts[j].szCodeProduct, gl_SavParam.stSavParamProducts[k].szCodeProduct))
						{
							PrintBuffer(gl_SavParam.stSavParamProducts[k].szNameProduct, 
								PRN_LEFT, SIZE_NORMAL_BOLD);
							vFormattageAmount(gl_SavParam.stSavParamProducts[k].szAmount, szAmount);
							sprintf(szBufferPrint,"%s %s",szAmount, "SR");
							sprintf(szBufferPrint,"%-10s %5s",szBufferPrint, stCurTran.stProducts[j].szQuantity);
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
								PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
								sprintf(szBufferPrint,"%sSR",szAmount);
								PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
							}
							else
							{
								sprintf(szBufferPrint,"%s %10sSR",szBufferPrint, szAmount);
								PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
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
					sprintf(szBufferPrint, "%-10s %5d", gl_pLangSelPiont[STR_TOTAL],ulTotalQty);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
					sprintf(szBufferPrint, "%s %s", szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
				}
				else
				{
					sprintf(szBufferPrint, "%-10s %5d %10s%s", gl_pLangSelPiont[STR_TOTAL],ulTotalQty, szAmount, "SR");
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
				}
				Jump_Lines(1);
				ulTotalQty_All += ulTotalQty;
				sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
				ulTotalQty = 0;
			}
		}

		//Jump_Lines(1);
		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_SUMMARY], PRN_MIDD, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "%-21s%8d", gl_pLangSelPiont[STR_PINS_QUANTITY], ulTotalQty_All);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_FACE_VALUE], PRN_LEFT, SIZE_NORMAL_BOLD);
		vFormattageAmount(stCurTran.szAmountWithoutDiscount, szBufferPrint);
		sprintf(szBufferPrint, "%s SR", szBufferPrint);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);
		Prn_A_LineOf("*");

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}


void Prn_PendingOrders(TRANS_DATA stCurTran, PENDING_ORDERS stPO)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		if (TYPE_BARCODE == stCurTran.ucIdentifyType)
		{
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_No_SERIE], stCurTran.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}
		else
		{
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_TERMINAL_ID],stCurTran.szTermCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_MERCHANT_ID],stCurTran.szMerchantCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}

		Jump_Lines(1);

		PrintBuffer(gl_pLangSelPiont[STR_PENDING_ORDER], PRN_MIDD, SIZE_NORMAL_BOLD);
		//Jump_Lines(1);

		sprintf(szBufferPrint, "%-7s %9s %10s", "Order Id", "Terminal", "Order Amt");
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		Prn_A_LineOf("-");
		for (i = 0; i < stPO.ucOrderNum; ++i)
		{
			vFormattageAmount(stPO.PendingOrdersInfo[i].szTotAmt, szTotalAmt);
			// Modified by Kim_LinHB 2013-4-7 v4.56
			if(strlen(stPO.PendingOrdersInfo[i].szId) +
				strlen(stPO.PendingOrdersInfo[i].szTermID) + 
				strlen(szTotalAmt) < 26)
			{
				sprintf(szBufferPrint, "%-s %s %sSR",	stPO.PendingOrdersInfo[i].szId,
														stPO.PendingOrdersInfo[i].szTermID,
														szTotalAmt);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

			}
			else
			{
				sprintf(szBufferPrint, "%-5s %s",	stPO.PendingOrdersInfo[i].szId,
					stPO.PendingOrdersInfo[i].szTermID);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
				sprintf(szBufferPrint, "%sSR",	szTotalAmt);
				PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
			}
			// Modify End
		}

		Prn_A_LineOf("*");

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_SalesmanInfo(void)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		PrintBuffer("SALESMAN INFO", PRN_MIDD, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_SALESMAN_ID], PRN_LEFT, SIZE_NORMAL);
		PrintBuffer(gl_MaintenaceParam.szSalesmanID, PRN_RIGHT, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_SALESMAN_NAME], PRN_LEFT, SIZE_NORMAL);
		PrintBuffer(gl_MaintenaceParam.szSalesmanName, PRN_RIGHT, SIZE_NORMAL_BOLD);
		//Jump_Lines(1);
		PrintBuffer(gl_pLangSelPiont[STR_SALESMAN_CREDIT], PRN_LEFT, SIZE_NORMAL);
		vFormattageAmount(gl_MaintenaceParam.szSalesmanCredit, szBufferPrint);
		sprintf(szBufferPrint, "%s %s", szBufferPrint, "SR");
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_PHYSICAL_CARDS_STOCK], PRN_LEFT, SIZE_NORMAL);
		vFormattageAmount(gl_MaintenaceParam.szPhysicalStock, szBufferPrint);
		sprintf(szBufferPrint, "%s %s", szBufferPrint, "SR");
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_SalesmanRoute(ROUTE_INFO stRoute, unsigned char bDuplicate)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned int uiNameList;
	xMerchantList stMerchant, stList;
	const unsigned char Lists[] = "MerhcantT.dat";

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();
		memset(&stMerchant, 0, sizeof(xMerchantList));
		memset(&stList, 0, sizeof(xMerchantList));

		PrintBuffer(gl_pLangSelPiont[STR_SALESMAN_ID], PRN_LEFT, SIZE_NORMAL);
		PrintBuffer(gl_MaintenaceParam.szSalesmanID, PRN_RIGHT, SIZE_NORMAL_BOLD);

		sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_SALESMAN_NAME], gl_MaintenaceParam.szSalesmanName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		Jump_Lines(1);

		PrintBuffer("SALESMAN ROUTE", PRN_MIDD, SIZE_NORMAL_BOLD);
		if(bDuplicate)
			PrintBuffer("DUPLICATE", PRN_MIDD, SIZE_NORMAL_BOLD);
		//Jump_Lines(1);

		sprintf(szBufferPrint, "%s%2.2s/%2.2s/%2.2s", PRE_YEAR, stRoute.szMerchantListDate, 
			stRoute.szMerchantListDate+2, stRoute.szMerchantListDate+4);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);
		//Jump_Lines(1);

		Prn_A_LineOf("-");

		uiNameList = 0;
		for(i = 0; i < stRoute.uiMerchantListNum; ++i)
		{
			unsigned int j;
			unsigned char bNew = TRUE;
			GetMerchant(FILE_MERCHANT, i, &stMerchant);
			for(j = 0; j < uiNameList; ++j)
			{
				GetMerchant(Lists, j, &stList);
				if(0 == strcmp(stMerchant.szName, stList.szName))
				{
					bNew = FALSE;
					break;
				}
			}
			if(bNew)
			{
				WriteMerchant(Lists, stMerchant);
				++uiNameList;
			}
		}
		for(i = 0; i < uiNameList; ++i)
		{
			GetMerchant(Lists, i, &stList);
			sprintf(szBufferPrint, "%d. %s", i + 1, stList.szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			Prn_A_LineOf("-");
			//Jump_Lines(1);
		}
		remove(Lists);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
void Prn_Discount(void)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		PrintBuffer("DISCOUNT INFO", PRN_MIDD, SIZE_NORMAL_BOLD);

		for (i = 0; i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			for(j = 0; j < gl_SavParam.uiProductsNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamProducts[j].uiCodeCategory)
				{
					if(TRUE == bFirst)
					{
						sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					PrintBuffer(gl_SavParam.stSavParamProducts[j].szNameProduct, 
						PRN_LEFT, SIZE_NORMAL_BOLD);
					sprintf(szBufferPrint, "-%d.%02d%s", 
						atoi(gl_SavParam.stSavParamProducts[j].szDiscount) / 100 ,
						atoi(gl_SavParam.stSavParamProducts[j].szDiscount) % 100,
						gl_pLangSelPiont[STR_PERC_DISCOUNT]);
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
				}
			}
			if(bFirst != TRUE)
			{
				Jump_Lines(1);
			}
		}

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_LstConsignOrderRep(SAV_PARAM stRecord)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

// 		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
// 		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
// 
// 		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_TERMINAL_ID], gl_MaintenaceParam.szTermID);	
// 		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
// 
// 		sprintf(szBufferPrint, "%s%d", "Batch Number: ", gl_SavParam.usBatchNo);
// 		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		PrintBuffer("LAST CONSIGN ORDER", PRN_MIDD, SIZE_NORMAL_BOLD);

		Jump_Lines(1);
		sprintf(szBufferPrint, "%s: %s", "ORDER NUMBER", stRecord.szOrderId);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		Jump_Lines(1);

		for (i = 0; i < stRecord.ucCategoryNum; ++i)
		{
			unsigned int j;

			sprintf(szBufferPrint,"%-s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
			sprintf(szBufferPrint,"%-10s%5s%14s",
				"Product", 
				gl_pLangSelPiont[STR_NB],
				gl_pLangSelPiont[STR_AMOUNT]); // normal  29 temp
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			Prn_A_LineOf("-");

			for(j = 0; j < stRecord.uiProductsNum; ++j)
			{
				if(stRecord.stSavParamProducts[j].uiCodeCategory == i)
				{
					vFormattageAmount(stRecord.stSavParamProducts[j].szAmount,
						szAmount);
					
					if(strlen(stRecord.stSavParamProducts[j].szNameProduct) > 13)
					{
						PrintBuffer(stRecord.stSavParamProducts[j].szNameProduct, PRN_LEFT, SIZE_NORMAL);
						sprintf(szBufferPrint, "%5s %12sSR", 
							stRecord.stSavParamProducts[j].szQuantity, szAmount);
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
					}
					else
					{
						sprintf(szBufferPrint, "%-13s %5s %7sSR", stRecord.stSavParamProducts[j].szNameProduct,
							stRecord.stSavParamProducts[j].szQuantity, szAmount);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
					}
				}
			}
			Jump_Lines(1);
		}

		Prn_A_LineOf("-");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_PhysicalStockReport(void)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_TERMINAL_ID], gl_MaintenaceParam.szTermID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint, "%s%d", "Batch Number: ", gl_SavParam.usBatchNo);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		Jump_Lines(1);

		PrintBuffer("OFFLINE PHYSICAL STOCK", PRN_MIDD, SIZE_NORMAL_BOLD);
		PrintBuffer("REPORT", PRN_MIDD, SIZE_NORMAL_BOLD);
		
		//Jump_Lines(2);

		for (i = 0; i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			if(gl_SavParam.stSavParamCategory[i].ucCategoryType != PRODUCT_PHYSICAL)
				continue;

			for(j = 0; j < gl_SavParam.uiProductsNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamProducts[j].uiCodeCategory)
				{
					if(0 == strlen(gl_SavParam.stSavParamProducts[j].szSoldQuantity))
						strcpy(gl_SavParam.stSavParamProducts[j].szSoldQuantity, "0");
					if(0 == strlen(gl_SavParam.stSavParamProducts[j].szQuantity))
						strcpy(gl_SavParam.stSavParamProducts[j].szQuantity, "0");

					// Added by Kim_LinHB 2013-7-16 v4.60
					if(atol(gl_SavParam.stSavParamProducts[j].szQuantity) <= 0 &&
						atol(gl_SavParam.stSavParamProducts[j].szSoldQuantity) <= 0)
						continue;
					// Add End

					if(TRUE == bFirst)
					{
						PrintBuffer(gl_SavParam.stSavParamCategory[i].szName, PRN_LEFT, SIZE_NORMAL_BOLD);
						sprintf(szBufferPrint,"%-13s%9s%7s",
							gl_pLangSelPiont[STR_SOUS_TITRE_PRODUCT], 
							gl_pLangSelPiont[STR_VEND], 
							gl_pLangSelPiont[STR_DISP]); // normal  29 temp
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}

 					if(strlen(gl_SavParam.stSavParamProducts[j].szNameProduct) > 15)
					{
						PrintBuffer(gl_SavParam.stSavParamProducts[j].szNameProduct, PRN_LEFT, SIZE_NORMAL);
						sprintf(szBufferPrint, "%5s %7s", 
							gl_SavParam.stSavParamProducts[j].szSoldQuantity,
							gl_SavParam.stSavParamProducts[j].szQuantity);
						PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
					}
					else
					{
						sprintf(szBufferPrint, "%-15s %5s %7s", gl_SavParam.stSavParamProducts[j].szNameProduct,
							gl_SavParam.stSavParamProducts[j].szSoldQuantity,
							gl_SavParam.stSavParamProducts[j].szQuantity);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
					}
				}
			}
			if(bFirst != TRUE)
			{
				//Prn_A_LineOf("-");
				Jump_Lines(1);
			}
		}

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
void Prn_OrderReport(const unsigned char *from, const unsigned char *to, unsigned char Type, OrderReport stOR)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		PrintBuffer("ORDER REPORTS", PRN_MIDD, SIZE_NORMAL_BOLD);
		if(ORDER_CASH == Type)
			PrintBuffer("-CASH ORDER-", PRN_MIDD, SIZE_NORMAL_BOLD);
		else if(ORDER_CREDIT_TRANSFER == Type)
			PrintBuffer("-CREDIT TRANSFER ORDER-", PRN_MIDD, SIZE_NORMAL_BOLD);
		else if(ORDER_CREDIT == Type)
			PrintBuffer("-CREDIT ORDER-", PRN_MIDD, SIZE_NORMAL_BOLD);
		//Jump_Lines(1);

		sprintf(szBufferPrint, " %s  %2.2s-%2.2s-%4.4s %2.2s:%2.2s:%2.2s", gl_pLangSelPiont[STR_DU], 
			from + 6, from + 4, from, from + 8, from + 10, from + 12);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, " %s   %2.2s-%2.2s-%4.4s %2.2s:%2.2s:%2.2s", gl_pLangSelPiont[STR_AU], 
			to + 6, to + 4, to, to + 8, to + 10, to + 12);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);

		Jump_Lines(1);

		sprintf(szBufferPrint, "%s: %s", "SALESMAN ID", stOR.szSalesmanId);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		sprintf(szBufferPrint, "%s: %s", "SALESMAN NAME", stOR.szSalesmanName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		Jump_Lines(1);

		sprintf(szBufferPrint, "%-12s%17s", "Order Serial", "Total");
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		Prn_A_LineOf("-");

		for(i = 0; i < stOR.uiOrderCnt; ++i)
		{
			PrintBuffer(stOR.Order[i].szMerchantName, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint, "%s: %s", "MERCHANT ID", stOR.Order[i].szMerchantId);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			vFormattageAmount(stOR.Order[i].szTotalAmt, szAmt);
			sprintf(szBufferPrint, "%-6s%20s SR", stOR.Order[i].szId, szAmt);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			Prn_A_LineOf("-");
		}
		vFormattageAmount(stOR.szTotAmt, szAmt);
		sprintf(szBufferPrint, "%-6s: %18s SR", gl_pLangSelPiont[STR_TOTAL], szAmt);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
// Added by Kim_LinHB 2013-7-30 v4.60
void Prn_OrderReport_Prod(const unsigned char *from, const unsigned char *to, OrderReport_Prod stOR)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1];
	unsigned char szAmtDiscount[SIZE_MAX_AMOUNT + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		PrintBuffer("ORDER REPORTS", PRN_MIDD, SIZE_NORMAL_BOLD);
		PrintBuffer("BY PRODUCT", PRN_MIDD, SIZE_NORMAL_BOLD);

		sprintf(szBufferPrint, "  %-s  %2.2s-%2.2s-%4.4s %2.2s:%2.2s:%2.2s", gl_pLangSelPiont[STR_DU], 
			from + 6, from + 4, from, from + 8, from + 10, from + 12);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "   %-s   %2.2s-%2.2s-%4.4s %2.2s:%2.2s:%2.2s", gl_pLangSelPiont[STR_AU], 
			to + 6, to + 4, to, to + 8, to + 10, to + 12);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		Jump_Lines(1);

		PrintBuffer("ID:", PRN_LEFT, SIZE_NORMAL);
		PrintBuffer(stOR.szSalesmanId, PRN_RIGHT, SIZE_NORMAL);

		PrintBuffer("NAME:", PRN_LEFT, SIZE_NORMAL);
		PrintBuffer(stOR.szSalesmanName, PRN_RIGHT, SIZE_NORMAL);

		PrintBuffer("CREDIT:", PRN_LEFT, SIZE_NORMAL);
		vFormattageAmount(stOR.szCredit, szAmt);
		sprintf(szBufferPrint, "%s SR", szAmt);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);

		Jump_Lines(1);
		Prn_A_LineOf("-");

		for(i = 0; i < stOR.uiGroupCnt; ++i)
		{
			unsigned int j, Cnt;
			PrintBuffer(stOR.Groups[i].szTitle, PRN_MIDD, SIZE_NORMAL);
			sprintf(szBufferPrint, "%d ORDER(S)", stOR.Groups[i].uiTotalOrders);
			PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL);
			Jump_Lines(1);
			sprintf(szBufferPrint, "Network: %s", stOR.Groups[i].szName);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			Jump_Lines(1);
			for(j = 0, Cnt = 0; j < stOR.uiProductCnt && Cnt < stOR.Groups[i].uiProductTypes; ++j)
			{
				if(stOR.Products[j].groupIndex == i)
				{
					unsigned char *p = strchr(stOR.Products[j].szName, '(');
					if(p)
					{
						*p = 0;
						PrintBuffer(stOR.Products[j].szName, PRN_LEFT, SIZE_NORMAL);
						*p = '(';
						PrintBuffer(p, PRN_LEFT, SIZE_NORMAL);
					}
					else
						PrintBuffer(stOR.Products[j].szName, PRN_LEFT, SIZE_NORMAL);
					vFormattageAmount(stOR.Products[j].szTotAmt, szAmt);
					sprintf(szBufferPrint, "X %d = %s SR",stOR.Products[j].uiTotalOrders,
														  szAmt);
					PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
					++Cnt;
				}
			}
			Jump_Lines(1);
			strcpy(szAmt, stOR.Groups[i].szTotAmt);
			strcpy(szAmtDiscount, stOR.Groups[i].szDiscount);
			PubAddHeadChars(szAmt, SIZE_MAX_AMOUNT, '0');
			PubAddHeadChars(szAmtDiscount, SIZE_MAX_AMOUNT, '0');

			PubAscSub(szAmt,szAmtDiscount,strlen(szAmt),szAmtDiscount);
			PubTrimHeadChars(szAmtDiscount, '0');
			PrintBuffer("TOTAL:", PRN_LEFT, SIZE_NORMAL);
			vFormattageAmount(stOR.Groups[i].szTotAmt, szAmt);
			sprintf(szBufferPrint, "%s SR", szAmt);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
			PrintBuffer("TOTAL FACE VALUE:", PRN_LEFT, SIZE_NORMAL);
			vFormattageAmount(szAmtDiscount, szAmt);
			sprintf(szBufferPrint, "%s SR", szAmt);
			PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
			Prn_A_LineOf("-");
		}

		PrintBuffer("TOTAL", PRN_MIDD, SIZE_NORMAL);
		sprintf(szBufferPrint, "%d ORDER(S)", stOR.uiTotalOrders);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL);
		Jump_Lines(1);

		strcpy(szAmt, stOR.szTotAmt);
		strcpy(szAmtDiscount, stOR.szDiscount);
		PubAddHeadChars(szAmt, SIZE_MAX_AMOUNT, '0');
		PubAddHeadChars(szAmtDiscount, SIZE_MAX_AMOUNT, '0');

		PubAscSub(szAmt,szAmtDiscount,strlen(szAmt),szAmtDiscount);
		PubTrimHeadChars(szAmtDiscount, '0');
		PrintBuffer("TOTAL:", PRN_LEFT, SIZE_NORMAL);
		vFormattageAmount(stOR.szTotAmt, szAmt);
		sprintf(szBufferPrint, "%s SR", szAmt);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
		PrintBuffer("TOTAL FACE VALUE:", PRN_LEFT, SIZE_NORMAL);
		vFormattageAmount(szAmtDiscount, szAmt);
		sprintf(szBufferPrint, "%s SR", szAmt);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}
// Add End
void Prn_OnlineStockReport(void)
{
	unsigned int i, ii;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_SALESMAN_NAME], gl_MaintenaceParam.szSalesmanName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		PrintBuffer("SALESMAN PHYSICAL CARDS", PRN_MIDD, SIZE_NORMAL_BOLD);
		PrintBuffer("STOCK", PRN_MIDD, SIZE_NORMAL_BOLD);

		//Jump_Lines(1);

		for (i = 0, ii = 0; ii < gl_SavParam.uiProductsNum && i < gl_SavParam.ucCategoryNum; ++i)
		{
			unsigned int j;
			unsigned char bFirst =TRUE;
			unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
			sprintf(szTotalAmt, "%.*d", SIZE_MAX_AMOUNT, 0);
			
			for(j = 0; j < gl_SavParam.uiProductsNum; ++j)
			{
				if(gl_SavParam.stSavParamCategory[i].uiCodeCategory == gl_SavParam.stSavParamProducts[j].uiCodeCategory 
					// Removed by Kim_LinHB 2013-7-31 v4.60
					//&&
					//PRODUCT_PHYSICAL == gl_SavParam.stSavParamCategory[i].ucCategoryType
					// Remove End
					)
				{
					// Added by Kim_LinHB 2013-7-16 v4.60
					if(atol(gl_SavParam.stSavParamProducts[j].szQuantity) <= 0)
						continue;
					// Add End

					if(TRUE == bFirst)
					{
						sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_NOM_CATEGORY], gl_SavParam.stSavParamCategory[i].szName);
						PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
						Prn_A_LineOf("-");
						bFirst = FALSE;
					}
					PrintBuffer(gl_SavParam.stSavParamProducts[j].szNameProduct, 
						PRN_LEFT, SIZE_NORMAL_BOLD);
					sprintf(szBufferPrint, "%-17s%12d", "Current Stock:", atoi(gl_SavParam.stSavParamProducts[j].szQuantity));
					PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
					++ii;    ///////...........
				}
			}
			if(TRUE != bFirst)
			{
				Jump_Lines(1);
			}
		}

		//Jump_Lines(1);
		//Prn_A_LineOf("*");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_SummaryReport(const unsigned char *from, const unsigned char *to, xSummary stSummary)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;

		Prn_Title_DateTime();

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_TICKET_TERMINAL_ID], gl_MaintenaceParam.szTermID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		Jump_Lines(1);

		PrintBuffer("SUMMARY REPORT", PRN_MIDD, SIZE_NORMAL_BOLD);

		//Jump_Lines(1);

		sprintf(szBufferPrint, " %s  %2.2s/%2.2s/%4.4s %2.2s:%2.2s", gl_pLangSelPiont[STR_DU], 
			from + 6, from + 4, from, from + 8, from + 10);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, " %s   %2.2s/%2.2s/%4.4s %2.2s:%2.2s", gl_pLangSelPiont[STR_AU], 
			to + 6, to + 4, to, to + 8, to + 10);
		PrintBuffer(szBufferPrint, PRN_MIDD, SIZE_NORMAL_BOLD);

		sprintf(szBufferPrint, "%9s %6s %12s", "Trantype", "Net", "Gross");
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);
		Prn_A_LineOf("-");

		PrintBuffer("Cash Order", PRN_LEFT, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "%8d.%02d %8d.%02d ", 
			atoi(stSummary.szCashTotalAmt) / 100, atoi(stSummary.szCashTotalAmt) % 100,
			atoi(stSummary.szCashTotalAmt_Gross) / 100, atoi(stSummary.szCashTotalAmt_Gross) % 100);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
		Jump_Lines(1);

		PrintBuffer("Offline Cash Order", PRN_LEFT, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "%8d.%02d %8d.%02d ", 
			atoi(stSummary.szOffTotalAmt) / 100, atoi(stSummary.szOffTotalAmt) % 100,
			atoi(stSummary.szOffTotalAmt_Gross) / 100, atoi(stSummary.szOffTotalAmt_Gross) % 100);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
		Jump_Lines(1);

		// Added by Kim_LinHB 2013-7-22 v4.60
		PrintBuffer("Credit Order", PRN_LEFT, SIZE_NORMAL_BOLD);
		sprintf(szBufferPrint, "%8d.%02d %8d.%02d ", 
			atoi(stSummary.szCreditTotalAmt) / 100, atoi(stSummary.szCreditTotalAmt) % 100,
			atoi(stSummary.szCreditTotalAmt_Gross) / 100, atoi(stSummary.szCreditTotalAmt_Gross) % 100);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL);
		// Add End

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_SettDownload(unsigned char Ret, RESPONSE_HEADER_TAG stResp)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		sprintf(szBufferPrint, "%s%s", gl_pLangSelPiont[STR_ID_COMM], gl_MaintenaceParam.szManagerID);
		PrintBuffer(szBufferPrint, PRN_LEFT,SIZE_NORMAL);

		sprintf(szBufferPrint, "%s%s", gl_pLangSelPiont[STR_ID_TERMINAL_Min], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT,SIZE_NORMAL);
		Jump_Lines(1);
		
		PrintBuffer(gl_pLangSelPiont[STR_TELEPARAMETRAGE_PRINT], PRN_MIDD, SIZE_NORMAL_BOLD);

		if(Ret != ErreurAuthentification)
		{
			Prn_A_LineOf("*");
	 		
			if (OK == Ret)
				PrintBuffer(gl_pLangSelPiont[STR_SUCCES], PRN_MIDD, SIZE_NORMAL_BOLD);
			else
				PrintBuffer(gl_pLangSelPiont[STR_ECHEC], PRN_MIDD, SIZE_NORMAL_BOLD);

			sprintf(szBufferPrint, "%s%d", gl_pLangSelPiont[STR_Version_Category], gl_SavParam.ucCategoryNum);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

			sprintf(szBufferPrint, "%s%d", gl_pLangSelPiont[STR_Version_Product], gl_SavParam.uiProductsNum);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

			Prn_A_LineOf("*");
		}
		if (OK != Ret)
		{
			// Print de l'abandon du rechargement
			Prn_A_LineOf("*");
			PrintBuffer(gl_pLangSelPiont[STR_TICKET_ABANDON],PRN_LEFT, SIZE_NORMAL_BOLD);

			//v4.38
			if(strlen(stResp.szMessage) > 0)
				PrintBuffer(stResp.szMessage, PRN_LEFT, SIZE_NORMAL_BOLD);

			// Code Retour
			sprintf(szBufferPrint,"%s:%d", gl_pLangSelPiont[STR_ERROR], Ret);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

			Prn_A_LineOf("*");
		}
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_ConnParameter(void)
{
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		PrintBuffer("CONNECT STEEING",PRN_MIDD, SIZE_NORMAL);
		//Jump_Lines(1);
		PrintBuffer("GPRS",PRN_LEFT,SIZE_NORMAL);
		Prn_A_LineOf("*");
		sprintf(szBufferPrint, "APN: %s",gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		
		//Jump_Lines(1);
		PrintBuffer("ETH",PRN_LEFT,SIZE_NORMAL);
		Prn_A_LineOf("*");
		if(TRUE == gl_MaintenaceParam.stConnParam.stParamEth.bDHCP)
		{
			PrintBuffer("DHCP: TRUE", PRN_LEFT, SIZE_NORMAL);
		}
		else
		{
			PrintBuffer("DHCP: FALSE", PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint, "Terminal IP: %s",gl_MaintenaceParam.stConnParam.stParamEth.szLocalIP);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint, "Mask Addr: %s",gl_MaintenaceParam.stConnParam.stParamEth.szNetMask);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint, "Gateway: %s",gl_MaintenaceParam.stConnParam.stParamEth.szGatewayIP);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint, "DNS: %s",gl_MaintenaceParam.stConnParam.stParamEth.szDNS);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint, "DNS 2: %s",gl_MaintenaceParam.stConnParam.stParamEth.szDNS_Bak);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}
		//Jump_Lines(1);
		PrintBuffer("Server",PRN_LEFT,SIZE_NORMAL);
		Prn_A_LineOf("*");
		PrintBuffer("Pay Flex primary host:", PRN_LEFT, SIZE_NORMAL);
		sprintf(szBufferPrint, "%s: %s",gl_MaintenaceParam.stConnParam.szRemoteIP,
				gl_MaintenaceParam.stConnParam.szRemotePort);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		PrintBuffer("Pay Flex secondary host:", PRN_LEFT, SIZE_NORMAL);
		sprintf(szBufferPrint, "%s: %s",gl_MaintenaceParam.stConnParam.szRemoteIP_Bak,
				gl_MaintenaceParam.stConnParam.szRemotePort_Bak);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

void Prn_CreditTransferOrder(TRANS_DATA stCurTran, unsigned char Mode)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	xMerchantList stMerhchant;

	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		memset(&stMerhchant, 0, sizeof(xMerchantList));


		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_SALESMAN_NAME], gl_MaintenaceParam.szSalesmanName);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		if (TYPE_BARCODE == stCurTran.ucIdentifyType)
		{
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_No_SERIE], stCurTran.szSerial);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}
		else
		{
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_ID_TERMINAL], stCurTran.szTermCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
			sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_MERCHANT_ID], stCurTran.szMerchantCode);
			PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
		}


		sprintf(szBufferPrint,"%s\n", gl_pLangSelPiont[STR_MERCHANT_NAME]);
		for(i = 0; i < gl_SavParam.stRoute.uiMerchantListNum; ++i)
		{
			GetMerchant(FILE_MERCHANT, i, &stMerhchant);
			if(0 == strcmp(stCurTran.szMerchantID, stMerhchant.szMerchantID))
			{
				sprintf(szBufferPrint,"%s\n %s\n", gl_pLangSelPiont[STR_MERCHANT_NAME], 
					stMerhchant.szName);
				break;
			}
		}

		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL_BOLD);

		Jump_Lines(1);

 		switch(Mode)
 		{
 		case PRN_SALESMAN:
 			PrintBuffer(gl_pLangSelPiont[STR_SALESMAN_COPY], PRN_MIDD, SIZE_NORMAL_BOLD);
 			break;
 		case PRN_MERCHANT:
 			PrintBuffer(gl_pLangSelPiont[STR_MERCHANT_COPY], PRN_MIDD, SIZE_NORMAL_BOLD);
 			break;
 		}
		PrintBuffer("CREDIT TRANSFER ORDER", PRN_MIDD, SIZE_NORMAL_BOLD);
		PrintBuffer(gl_pLangSelPiont[STR_ORDER_VALIDATED], PRN_MIDD, SIZE_NORMAL);
		PrintBuffer(stCurTran.szProvisionID, PRN_MIDD, SIZE_NORMAL_BOLD);

		Prn_A_LineOf("*");

		PrintBuffer(gl_pLangSelPiont[STR_AMOUNT], PRN_LEFT, SIZE_NORMAL_BOLD);
		vFormattageAmount(stCurTran.szAmountWithoutDiscount, szBufferPrint);
		sprintf(szBufferPrint, "%s SR", szBufferPrint);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);

		PrintBuffer(gl_pLangSelPiont[STR_CURRENT_DEPOSIT], PRN_LEFT, SIZE_NORMAL_BOLD);
		vFormattageAmount(stCurTran.szAmountWithDiscount, szBufferPrint);
		sprintf(szBufferPrint, "%s SR", szBufferPrint);
		PrintBuffer(szBufferPrint, PRN_RIGHT, SIZE_NORMAL_BOLD);

		Prn_A_LineOf("*");
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}

// Added by Kim_LinHB 2013-10-28 v4.68
void Prn_Consignment(CONSIGNMENT_LIST stOrderList, unsigned char OrderStatus, int OrderIndex, int Operation)
{
	unsigned int i;
	unsigned char szBufferPrint[SIZE_MAX_POLICE_DEMI + 1 + 1];
	unsigned char szAmt[SIZE_MAX_AMOUNT + 1];
	
	do{
		sg_bNoPaperCountinue = FALSE;
		sg_bCanCelPrn = FALSE;
		Prn_Title_DateTime();

		
		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_SALESMAN_ID], gl_MaintenaceParam.szSalesmanID);	
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		sprintf(szBufferPrint,"%s%s", gl_pLangSelPiont[STR_ID_TERMINAL], gl_MaintenaceParam.szTermID);
		PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

		Jump_Lines(1);

		if(ID_ConsignConfirm == OrderStatus)
		{
			switch(Operation)
			{
			case 0:
				PrintBuffer("CONSIGNMENT REJECTED", PRN_MIDD, SIZE_NORMAL_BOLD);
				break;
			case 1:
				PrintBuffer("CONSIGNMENT APPROVED", PRN_MIDD, SIZE_NORMAL_BOLD);
				break;
			case 2:
				PrintBuffer("CONSIGNMENT INFORMATION", PRN_MIDD, SIZE_NORMAL_BOLD);
				break;
			}

			Jump_Lines(1);
			if(OrderIndex >= 0)
			{
				sprintf(szBufferPrint, "ORDER ID: %s", stOrderList.ConsigOrderList[OrderIndex].Id);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

				sprintf(szBufferPrint, "%-17s%12s", "Created date", "Total Amount");
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
				Prn_A_LineOf("*");
				vFormattageAmount(stOrderList.ConsigOrderList[OrderIndex].TotalPrice, szAmt);
				sprintf(szBufferPrint, "%-10.10s%16s SR", 
					stOrderList.ConsigOrderList[OrderIndex].CreateDate,
					szAmt);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
				if(strlen(stOrderList.ConsigOrderList[OrderIndex].CreateDate) > 16)
					PrintBuffer(stOrderList.ConsigOrderList[OrderIndex].CreateDate + 10, PRN_LEFT, SIZE_NORMAL);
				Prn_A_LineOf("*");
			}
		}
		else if(ID_PendingConsignOrder == OrderStatus)
		{
			PrintBuffer("CONSIGNMENT INFORMATION", PRN_MIDD, SIZE_NORMAL_BOLD);
			PrintBuffer("PENDING ON STORE KEEPER", PRN_MIDD, SIZE_NORMAL_BOLD);
			Jump_Lines(1);

			for(i = 0; i < stOrderList.num; ++i)
			{
				sprintf(szBufferPrint, "ORDER ID: %s", stOrderList.ConsigOrderList[i].Id);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);

				sprintf(szBufferPrint, "%-17s%12s", "Created date", "Total Amount");
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
				Prn_A_LineOf("*");
				vFormattageAmount(stOrderList.ConsigOrderList[i].TotalPrice, szAmt);
				sprintf(szBufferPrint, "%-10.10s%16s SR", 
					stOrderList.ConsigOrderList[i].CreateDate,
					szAmt);
				PrintBuffer(szBufferPrint, PRN_LEFT, SIZE_NORMAL);
				if(strlen(stOrderList.ConsigOrderList[i].CreateDate) > 16)
					PrintBuffer(stOrderList.ConsigOrderList[i].CreateDate + 10, PRN_LEFT, SIZE_NORMAL);
				Prn_A_LineOf("*");
				Jump_Lines(1);
			}
		}
		Jump_Lines(END_JUMP);
		PrintStart(TRUE); // Added by Kim_LinHB 2013/3/22 v4.55
	}while(sg_bNoPaperCountinue);
}


//////////////////////////////////////////////////////////////////////////
// Modified by Kim_LinHB 2013-3-23 v4.55
static void MyPrnInit(void)
{
	if(0 == sg_ucPrintLineCnt)
	{
		while(0x01 == PrnStatus()); // Added by Kim_LinHB 2013-3-27 v4.55
		PrnInit();
		if(_TERMINAL_S90_ == gl_TermMode)
			PrnSetGray(180); // Added by Kim_LinHB 2013-4-17 v4.56
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
	}
}

static ST_FONT sg_SmallFont = {CHARSET_WEST, 6, 12, 0, 0};
static ST_FONT sg_SmallFont_Bold = {CHARSET_WEST, 6, 12, 1, 0};
static ST_FONT sg_NormalFont = {CHARSET_WEST, 12, 24, 0, 0};
static ST_FONT sg_NormalFont_Bold = {CHARSET_WEST, 12, 24, 1, 0};
static ST_FONT sg_LargeFont = {CHARSET_WEST, 16, 32, 0, 0};
static ST_FONT sg_LargeFont_Bold = {CHARSET_WEST, 16, 32, 0, 0};


void PrintBuffer(const unsigned char *pucBuffer,unsigned char Position, unsigned char ucType)
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
		PrnLeftIndent(0); // Added by Kim_LinHB 2013-3-23 v4.55
		PrnSpaceSet(1,0); //v4.35  PrnSpaceSet(2,0)
		ucSpaceWidth = 1;
		switch(ucType)
		{
		case DEMI_SIZE:
			PrnSelectFont(&sg_SmallFont, NULL);
			ucSingleWidth = sg_SmallFont.Width;
			ucMultiWidth = sg_SmallFont.Width;
			break;
		case DEMI_SIZE_BOLD:
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
		case SIZE_LARGE_BOLD:
			PrnSpaceSet(6,0);   //v4.35 
			PrnSelectFont(&sg_LargeFont, NULL);
			ucSingleWidth = sg_LargeFont.Width;
			ucMultiWidth = sg_LargeFont.Width;
			ucSpaceWidth = 6;
			break;
		}

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
						}while (ucKey != KEYENTER && ucKey != KEYCANCEL);

						sg_ucPrintLineCnt = 0; // Added by Kim_LinHB 2013-3-23 v4.55
						if(KEYCANCEL == ucKey)
						{
							sg_bCanCelPrn = TRUE;
							return;
						}
						if(KEYENTER == ucKey)
						{
							sg_bNoPaperCountinue = TRUE;
						}
					}
					else
						break;
				}
#endif

#ifdef FARSI
 				if(IfArabicString(pucBuffer))
 					Prn_Str_AR(pucBuffer, Position, ucType + 1);
 				else
 				{
 					if(uiPixelsNums <= 300)
 						PrnLeftIndent(uiPixelsNums);
 					else
 					{
 						unsigned char spaces[50];
 						sprintf(spaces, "%*.s", uiPixelsNums / (ucSingleWidth + ucSpaceWidth)," ");
 						PrnStr((char *)spaces);
 					}
 					ucRet = PrnStr((char *)pucBuffer);
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
				++sg_ucPrintLineCnt;
			}
			PrintStart(FALSE);
		}
	}
	//return 0;
}

// Added by Kim_LinHB 2013/3/22 v4.55
static void PrintStart(unsigned char bForce)
{
	unsigned char ucRet = 0x00;
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

	if(ucRet != 0x00)
	{
#if !defined(LHB_TEST)
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
			}while (ucKey != KEYENTER && ucKey != KEYCANCEL);
			if(KEYCANCEL == ucKey)
			{
				sg_bCanCelPrn = TRUE;
				return;
			}
			if(KEYENTER == ucKey)
			{
				sg_bNoPaperCountinue = TRUE;
			}
		}
		else
		{
			ScrCls();
			ScrPrint(0,0,CFONT, "Print error:%02x",ucRet);
			PubWaitKey(TIME_DELAY);
		}
#endif
	}
}
// Add End

static void Jump_Lines(unsigned char No)
{
	unsigned char buff[50];
	memset(buff, 0, sizeof(buff));
	memset(buff, '\n', No);
	PrintBuffer(buff, PRN_LEFT, SIZE_NORMAL);
}

//	打印分割线
static void Prn_A_LineOf(const unsigned char *str)
{
	int i = 0;
	unsigned char buff[50];

	memset(buff, 0, sizeof(buff));

	strcpy(buff, str);
	for (i = 1; i < 29; ++i)
		strcat(buff, str);

	PrintBuffer(buff, PRN_LEFT, SIZE_NORMAL);
}

static void Prn_Str_AR(const unsigned char *pucBuffer, unsigned char Position, unsigned char ucType)
{
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
}


static unsigned char IsSingle(char p)
{
	if(~(p >> 8) == 0)  
		return 0;//代表不是汉字
	return 1;  
}

//	获取打印字符串所占像素数
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

//static void vFormattageLongTPE_PC(long *plNum,const unsigned char * pszNum)
//{
//	long lNum;
//	lNum = pszNum[0];
//	lNum<<=8;
//	lNum += pszNum[1];
//	lNum<<=8;
//	lNum += pszNum[2];
//	lNum<<=8;
//	lNum += pszNum[3];
//
//	*plNum = lNum;
//}

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

	strcpy(pszAmtout, amt);
	// 	}
	// 	else
	// 	{
	// 		memcpy(pszAmtout,pszAmtIn,(ucLen - 2));
	// 		strcat(pszAmtout,"  ");
	// 	}
}
