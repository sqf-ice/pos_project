#include <posapi.h>
#include <posapi_all.h>
#include "appLib.h"
#include "framework.h"
#include "data_define_salesman.h"
#include "connect.h"
#include "calc_salesman.h"

//////////////////////////////////////////////////////////////////////////
const unsigned char *cSTRConnType[] = 
{
	"SIMUL",
	//"REAL V22 BIS",
	//"REAL V22",
	"ETHERNET",
	"GPRS",
	//"PPP",
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//					global
//////////////////////////////////////////////////////////////////////////
unsigned char gl_bFirstRun;
unsigned char gl_bSettDown = 0;
unsigned int  gl_TermMode;
unsigned char gl_TermName[8+1];	
unsigned char gl_TermSN[32+1];

MAINTENACE_PARAM gl_MaintenaceParam;
MAINTENACE_PARAM_EXT gl_MaintenaceParam_Ext; // Added by Kim_LinHB 2013-7-12
SAV_PARAM		gl_SavParam;
TRANS_DATA		gl_LastTran;
xSummary gl_Summary_tmp;
unsigned char	gl_ProvisionIdTmp[SIZE_MAX_PROVISION_ID+1];

unsigned char gl_szFTPErrInfo[256];
FTPFILE_INFO gl_stFTPFileInfo;

unsigned char gl_bPlanned_SoftDownload = FALSE;
unsigned char gl_bPlanned_SettDownload = FALSE;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static void DataInitDef();

//////////////////////////////////////////////////////////////////////////
enCodeReturn  WriteParam(unsigned char type)
{
	int iRet;
	//remove(FILE_PARAM); // Removed by Kim_LinHB 2013-7-12 v4.60

	gl_bFirstRun = 0;
	gl_MaintenaceParam.ucVersion = FILEVER;
	strcpy(gl_MaintenaceParam_Ext.version, STR_VERSION_SUB);

	iRet = PubFileWrite(FILE_PARAM, 0, (void *)&gl_bFirstRun, 1);
	if((type & WRT_ALL) || (type & WRT_MAINTA))
		iRet = PubFileWrite(FILE_PARAM, 1, (void *)&gl_MaintenaceParam, sizeof(MAINTENACE_PARAM));
	if((type & WRT_ALL) || (type & WRT_TXN))
		iRet = PubFileWrite(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM), (void *)&gl_LastTran, sizeof(TRANS_DATA));
	if((type & WRT_ALL) || (type & WRT_SAVE))
		iRet = PubFileWrite(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA), 
						(void *)&gl_SavParam, sizeof(SAV_PARAM));

	if((type & WRT_ALL) || (type & WRT_MAINTA))
		iRet = PubFileWrite(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA) + sizeof(SAV_PARAM), 
					(void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT));

	if(iRet != OK)
		return 0xFF;

	return OK;
}

void DataInit()
{
	ReadFTPFileInfo();
	// added by Allen 2015-12-22 
	memset(&gl_SavParam, 0, sizeof(SAV_PARAM));
	// Added by Kim_LinHB 2013-9-3 v4.65 for setting download after remote download
	if(gl_stFTPFileInfo.lCurrLen == gl_stFTPFileInfo.lTotalLen)
	{
		remove(FILE_UPDATE);
	}
	// Add End
	memset(gl_ProvisionIdTmp, 0 ,sizeof(gl_ProvisionIdTmp));

	if(gl_bFirstRun){
// 		if(-1 != fexist(FILE_PARAM))
// 			remove(FILE_PARAM);
		if(-1 != fexist(FILE_OFFLINE))
			remove(FILE_OFFLINE);
		if(-1 != fexist(FILE_OFF_CANCEL))
			remove(FILE_OFF_CANCEL);
		DataInitDef();
		WriteParam(WRT_ALL);
	}
	else
	{
		unsigned int i;
		unsigned char buffer[sizeof(MAINTENACE_PARAM)];

		int fd = open(FILE_PARAM,O_RDWR);
		if(fd < 0)
		{
			close(fd);
			DataInitDef();
			return;
		}
		seek(fd, 0, SEEK_SET);
		read(fd, buffer, 1);
		read(fd, (void *)&gl_MaintenaceParam, sizeof(gl_MaintenaceParam));

		if(gl_MaintenaceParam.ucVersion < 0x60)
		{
			SAV_PARAM_OLD tmp2;
			TRANS_DATA_OLD lastTran_old;
			read(fd, (void *)&lastTran_old, sizeof(lastTran_old));
			memcpy(&gl_LastTran, &lastTran_old, sizeof(lastTran_old));
			{
				for(i = 0; i < lastTran_old.ucProductNum; ++i)
				{
					gl_LastTran.stProducts[i].uiCodeCategory = lastTran_old.stProducts[i].ucCodeCategory;
					strcpy(gl_LastTran.stProducts[i].szCodeProduct, lastTran_old.stProducts[i].szCodeProduct);
					strcpy(gl_LastTran.stProducts[i].szQuantity, lastTran_old.stProducts[i].szQuantity);
				}
			}

			read(fd, (void *)&tmp2, sizeof(tmp2) - sizeof(tmp2.stSavParamCategory) - sizeof(tmp2.stSavParamProducts) -
										sizeof(tmp2.stRoute));

			for (i = 0; i < tmp2.ucCategoryNum; ++i)
				read(fd, (void *)&tmp2.stSavParamCategory[i], sizeof(tmp2.stSavParamCategory)/NB_MAX_CATEGORYS);
			for (i = 0; i < tmp2.ucProductsNum; ++i)
				read(fd, (void *)&tmp2.stSavParamProducts[i], sizeof(tmp2.stSavParamProducts)/NB_MAX_PRODUCTS);

			read(fd, (void *)&tmp2.stRoute, sizeof(tmp2.stRoute));
			//.....
			close(fd);

			// Added by Kim_LinHB 2013-7-12 v4.60 for v4.57-v4.59
			gl_SavParam.usTraceNo = tmp2.usTraceNo;
			gl_SavParam.usBatchNo = tmp2.usBatchNo;
			gl_SavParam.uiMerchantListIndex = tmp2.uiMerchantListIndex;
			gl_SavParam.uiProductsListIndex = tmp2.uiProductsListIndex;
			gl_SavParam.uiCurOfflineNo = tmp2.ucCurOfflineNo;
			gl_SavParam.bIsSetup = tmp2.bIsSetup;

			memcpy(gl_SavParam.ucKey, tmp2.ucKey, 16);
			strcpy(gl_SavParam.szOrderId, tmp2.szOrderId);

			gl_SavParam.ucCategoryNum = tmp2.ucCategoryNum;
			gl_SavParam.uiProductsNum = tmp2.ucProductsNum;

			for(i = 0; i < tmp2.ucCategoryNum; ++i)
			{
				gl_SavParam.stSavParamCategory[i].uiCodeCategory = tmp2.stSavParamCategory[i].ucCodeCategory;
				strcpy(gl_SavParam.stSavParamCategory[i].szName, tmp2.stSavParamCategory[i].szName);
				gl_SavParam.stSavParamCategory[i].ucCategoryType = tmp2.stSavParamCategory[i].ucCategoryType;
				gl_SavParam.stSavParamCategory[i].ucNumLogo = tmp2.stSavParamCategory[i].ucNumLogo;
			}

			for(i = 0; i < tmp2.ucProductsNum; ++i)
			{
				gl_SavParam.stSavParamProducts[i].uiCodeCategory = tmp2.stSavParamProducts[i].ucCodeCategory;
				gl_SavParam.stSavParamProducts[i].ucType = tmp2.stSavParamProducts[i].ucType;
				strcpy(gl_SavParam.stSavParamProducts[i].szCodeProduct, tmp2.stSavParamProducts[i].szCodeProduct);
				gl_SavParam.stSavParamProducts[i].bCredit = tmp2.stSavParamProducts[i].bCredit;
				strcpy(gl_SavParam.stSavParamProducts[i].sReserved, tmp2.stSavParamProducts[i].sReserved);
				strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct, tmp2.stSavParamProducts[i].szNameProduct);
				strcpy(gl_SavParam.stSavParamProducts[i].szAmount, tmp2.stSavParamProducts[i].szAmount);
				strcpy(gl_SavParam.stSavParamProducts[i].szQuantity, tmp2.stSavParamProducts[i].szQuantity);
				strcpy(gl_SavParam.stSavParamProducts[i].szSoldQuantity, tmp2.stSavParamProducts[i].szSoldQuantity);
				strcpy(gl_SavParam.stSavParamProducts[i].szAverage, tmp2.stSavParamProducts[i].szAverage);
				strcpy(gl_SavParam.stSavParamProducts[i].szOrderNum, tmp2.stSavParamProducts[i].szOrderNum);
				strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, tmp2.stSavParamProducts[i].szDiscount);
			}
			memcpy(&gl_SavParam.stRoute, &tmp2.stRoute, sizeof(tmp2.stRoute));
			// Add End

			// Added by Kim_LinHB 2013-9-3 v4.65 for setting download after remote download
			memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
			//strcpy(gl_MaintenaceParam.szTimeSettDownload, "0000");
			// Add End
		}
		else
		{
			SAV_PARAM_OLD2 sav_old2;
			memset(&sav_old2, '\0', sizeof(sav_old2));
			close(fd);
			
			if(gl_MaintenaceParam.ucVersion < FILEVER)
			{
				TRANS_DATA_OLD lastTran_old;
				PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD), 
					(void *)&sav_old2, sizeof(sav_old2));
				PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD) + sizeof(sav_old2),
					(void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT)); // Added by Kim_LinHB 2013-7-12 v4.60
				PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM), 
					(void *)&lastTran_old, sizeof(lastTran_old));
				memcpy(&gl_LastTran, &lastTran_old, sizeof(lastTran_old));
				{
					for(i = 0; i < lastTran_old.ucProductNum; ++i)
					{
						gl_LastTran.stProducts[i].uiCodeCategory = lastTran_old.stProducts[i].ucCodeCategory;
						strcpy(gl_LastTran.stProducts[i].szCodeProduct, lastTran_old.stProducts[i].szCodeProduct);
						strcpy(gl_LastTran.stProducts[i].szQuantity, lastTran_old.stProducts[i].szQuantity);
					}
				}
				
				// Added by Kim_LinHB 2013-11-20 v4.72
				gl_SavParam.usTraceNo = sav_old2.usTraceNo;
				gl_SavParam.usBatchNo = sav_old2.usBatchNo;
				gl_SavParam.uiMerchantListIndex = sav_old2.uiMerchantListIndex;
				gl_SavParam.uiProductsListIndex = sav_old2.uiProductsListIndex;
				gl_SavParam.uiCurOfflineNo = sav_old2.uiCurOfflineNo;
				gl_SavParam.bIsSetup = sav_old2.bIsSetup;

				memcpy(gl_SavParam.ucKey, sav_old2.ucKey, 16);
				strcpy(gl_SavParam.szOrderId, sav_old2.szOrderId);

				gl_SavParam.ucCategoryNum = sav_old2.ucCategoryNum;
				gl_SavParam.uiProductsNum = sav_old2.ucProductsNum;

				for(i = 0; i < sav_old2.ucCategoryNum; ++i)
				{
					gl_SavParam.stSavParamCategory[i].uiCodeCategory = sav_old2.stSavParamCategory[i].ucCodeCategory;
					strcpy(gl_SavParam.stSavParamCategory[i].szName, sav_old2.stSavParamCategory[i].szName);
					gl_SavParam.stSavParamCategory[i].ucCategoryType = sav_old2.stSavParamCategory[i].ucCategoryType;
					gl_SavParam.stSavParamCategory[i].ucNumLogo = sav_old2.stSavParamCategory[i].ucNumLogo;
				}

				for(i = 0; i < sav_old2.ucProductsNum; ++i)
				{
					gl_SavParam.stSavParamProducts[i].uiCodeCategory = sav_old2.stSavParamProducts[i].ucCodeCategory;
					gl_SavParam.stSavParamProducts[i].ucType = sav_old2.stSavParamProducts[i].ucType;
					strcpy(gl_SavParam.stSavParamProducts[i].szCodeProduct, sav_old2.stSavParamProducts[i].szCodeProduct);
					gl_SavParam.stSavParamProducts[i].bCredit = sav_old2.stSavParamProducts[i].bCredit;
					strcpy(gl_SavParam.stSavParamProducts[i].sReserved, sav_old2.stSavParamProducts[i].sReserved);
					strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct, sav_old2.stSavParamProducts[i].szNameProduct);
					strcpy(gl_SavParam.stSavParamProducts[i].szAmount, sav_old2.stSavParamProducts[i].szAmount);
					strcpy(gl_SavParam.stSavParamProducts[i].szQuantity, sav_old2.stSavParamProducts[i].szQuantity);
					strcpy(gl_SavParam.stSavParamProducts[i].szSoldQuantity, sav_old2.stSavParamProducts[i].szSoldQuantity);
					strcpy(gl_SavParam.stSavParamProducts[i].szAverage, sav_old2.stSavParamProducts[i].szAverage);
					strcpy(gl_SavParam.stSavParamProducts[i].szOrderNum, sav_old2.stSavParamProducts[i].szOrderNum);
					strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, sav_old2.stSavParamProducts[i].szDiscount);
				}
				memcpy(&gl_SavParam.stRoute, &sav_old2.stRoute, sizeof(sav_old2.stRoute));

				// Added by Kim_LinHB 2013-9-3 v4.65 for setting download after remote download
				memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
				// Add End
			}
			else
			{
				SAV_PARAM_OLD3 *sav_old3;
				long len = filesize(FILE_PARAM);
				if ((sav_old3 = (SAV_PARAM_OLD3 *)malloc(sizeof(SAV_PARAM_OLD3))) == NULL)
				{
					while(1)
					{
						ScrCls();
						ScrPrint(0,0,0,"Error:Not enought memory");
						getkey();
					}
				}
				memset(sav_old3, '\0', sizeof(SAV_PARAM_OLD3));
				if(1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD) + sizeof(sav_old2) + sizeof(MAINTENACE_PARAM_EXT) == len )
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD) + sizeof(sav_old2),
					(void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT)); // Added by Kim_LinHB 2013-7-12 v4.60
				else if (1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD2) + sizeof(SAV_PARAM_OLD3 ) + sizeof(MAINTENACE_PARAM_EXT) == len)
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD2) + sizeof(SAV_PARAM_OLD3),
					(void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT)); // Added by Kim_LinHB 2013-7-12 v4.60
				else
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA) + sizeof(SAV_PARAM),
					(void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT)); // Added by Kim_LinHB 2013-7-12 v4.60
				//check gl_MaintenaceParam_Ext.version
				if(strcmp(gl_MaintenaceParam_Ext.version, "4.72") < 0)
				{
					TRANS_DATA_OLD lastTran_old;
					TRANS_DATA_OLD2 lastTran_old2;
					memset(&lastTran_old, '\0', sizeof(lastTran_old));
					memset(&lastTran_old2, '\0', sizeof(lastTran_old2));
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD), 
						(void *)&sav_old2, sizeof(sav_old2));
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM), 
						(void *)&lastTran_old, sizeof(lastTran_old));
					memcpy(&lastTran_old2, &lastTran_old, sizeof(lastTran_old));  // modified by Allen 20151208
					memcpy(&gl_LastTran, &lastTran_old2, sizeof(lastTran_old2));// added by Allen 20151208
					{
						for(i = 0; i < lastTran_old.ucProductNum; ++i)
						{
							lastTran_old2.stProducts[i].uiCodeCategory = lastTran_old.stProducts[i].ucCodeCategory;
							strcpy(lastTran_old2.stProducts[i].szCodeProduct, lastTran_old.stProducts[i].szCodeProduct);
							strcpy(lastTran_old2.stProducts[i].szQuantity, lastTran_old.stProducts[i].szQuantity);
						}
						for(i = 0; i < lastTran_old2.ucProductNum; ++i)  // added by Allen 20151208 v4.88
						{
							memcpy(&gl_LastTran.stProducts[i], &lastTran_old2.stProducts[i], sizeof(PRODUCT_INFO));
						}
					}
					// Added by Kim_LinHB 2013-11-20 v4.72
					sav_old3->usTraceNo = sav_old2.usTraceNo;
					sav_old3->usBatchNo = sav_old2.usBatchNo;
					sav_old3->uiMerchantListIndex = sav_old2.uiMerchantListIndex;
					sav_old3->uiProductsListIndex = sav_old2.uiProductsListIndex;
					sav_old3->uiCurOfflineNo = sav_old2.uiCurOfflineNo;
					sav_old3->bIsSetup = sav_old2.bIsSetup;
					memcpy(sav_old3->ucKey, sav_old2.ucKey, 16);
					strcpy(sav_old3->szOrderId, sav_old2.szOrderId);
					sav_old3->ucCategoryNum = sav_old2.ucCategoryNum;
					sav_old3->ucProductsNum = sav_old2.ucProductsNum;
					gl_SavParam.usTraceNo = sav_old3->usTraceNo;
					gl_SavParam.usBatchNo = sav_old3->usBatchNo;
					gl_SavParam.uiMerchantListIndex = sav_old3->uiMerchantListIndex;
					gl_SavParam.uiProductsListIndex = sav_old3->uiProductsListIndex;
					gl_SavParam.uiCurOfflineNo = sav_old3->uiCurOfflineNo;
					gl_SavParam.bIsSetup = sav_old3->bIsSetup;

					memcpy(gl_SavParam.ucKey, sav_old3->ucKey, 16);
					strcpy(gl_SavParam.szOrderId, sav_old3->szOrderId);

					gl_SavParam.ucCategoryNum = sav_old3->ucCategoryNum;
					gl_SavParam.uiProductsNum = sav_old3->ucProductsNum;

					for(i = 0; i < sav_old2.ucCategoryNum; ++i)
					{
						sav_old3->stSavParamCategory[i].uiCodeCategory = sav_old2.stSavParamCategory[i].ucCodeCategory;
						strcpy(sav_old3->stSavParamCategory[i].szName, sav_old2.stSavParamCategory[i].szName);
						sav_old3->stSavParamCategory[i].ucCategoryType = sav_old2.stSavParamCategory[i].ucCategoryType;
						sav_old3->stSavParamCategory[i].ucNumLogo = sav_old2.stSavParamCategory[i].ucNumLogo;
					}
					for(i = 0; i < sav_old3->ucCategoryNum; ++i)
					{
						memcpy(&gl_SavParam.stSavParamCategory[i], &sav_old3->stSavParamCategory[i], sizeof(xParamCategory));
					}

					for(i = 0; i < sav_old2.ucProductsNum; ++i)
					{
						sav_old3->stSavParamProducts[i].uiCodeCategory = sav_old2.stSavParamProducts[i].ucCodeCategory;
						sav_old3->stSavParamProducts[i].ucType = sav_old2.stSavParamProducts[i].ucType;
						strcpy(sav_old3->stSavParamProducts[i].szCodeProduct, sav_old2.stSavParamProducts[i].szCodeProduct);
						sav_old3->stSavParamProducts[i].bCredit = sav_old2.stSavParamProducts[i].bCredit;
						strcpy(sav_old3->stSavParamProducts[i].sReserved, sav_old2.stSavParamProducts[i].sReserved);
						strcpy(sav_old3->stSavParamProducts[i].szNameProduct, sav_old2.stSavParamProducts[i].szNameProduct);
						strcpy(sav_old3->stSavParamProducts[i].szAmount, sav_old2.stSavParamProducts[i].szAmount);
						strcpy(sav_old3->stSavParamProducts[i].szQuantity, sav_old2.stSavParamProducts[i].szQuantity);
						strcpy(sav_old3->stSavParamProducts[i].szSoldQuantity, sav_old2.stSavParamProducts[i].szSoldQuantity);
						strcpy(sav_old3->stSavParamProducts[i].szAverage, sav_old2.stSavParamProducts[i].szAverage);
						strcpy(sav_old3->stSavParamProducts[i].szOrderNum, sav_old2.stSavParamProducts[i].szOrderNum);
						strcpy(sav_old3->stSavParamProducts[i].szDiscount, sav_old2.stSavParamProducts[i].szDiscount);
					}
					for(i = 0; i < sav_old3->ucProductsNum; ++i)
					{
						memcpy(&gl_SavParam.stSavParamProducts[i], &sav_old3->stSavParamProducts[i], sizeof(xParamProducts));
					}
					memcpy(&sav_old3->stRoute, &sav_old2.stRoute, sizeof(sav_old2.stRoute));
					memcpy(&gl_SavParam.stRoute, &sav_old3->stRoute, sizeof(sav_old3->stRoute));
					memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
					WriteParam(WRT_ALL);
				}
				else if (strcmp(gl_MaintenaceParam_Ext.version, "4.89") < 0)     // added by Allen 20151207 v4.88
				{
					TRANS_DATA_OLD2 *lastTran_old2;
					if ((lastTran_old2 = (TRANS_DATA_OLD2 *)malloc(sizeof(TRANS_DATA_OLD2))) == NULL)
					{
						while(1)
						{
							ScrCls();
							ScrPrint(0,0,0,"Error:Not enought memory");
							getkey();
						}
					}
					memset(lastTran_old2, '\0', sizeof(TRANS_DATA_OLD2));
				
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA_OLD2), 
						(void *)sav_old3, sizeof(SAV_PARAM_OLD3));
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM), 
						(void *)lastTran_old2, sizeof(TRANS_DATA_OLD2));	
					memcpy(&gl_LastTran, lastTran_old2, sizeof(TRANS_DATA_OLD2));  // zheli you wenti 
					{
						for(i = 0; i < lastTran_old2->ucProductNum; ++i)
						{
							//gl_LastTran.stProducts[i].uiCodeCategory = lastTran_old2.stProducts[i].uiCodeCategory;
							//strcpy(gl_LastTran.stProducts[i].szCodeProduct, lastTran_old2.stProducts[i].szCodeProduct);
							//strcpy(gl_LastTran.stProducts[i].szQuantity, lastTran_old2.stProducts[i].szQuantity);
							memcpy(&gl_LastTran.stProducts[i], &lastTran_old2->stProducts[i], sizeof(PRODUCT_INFO));
						}
					}
					gl_SavParam.usTraceNo = sav_old3->usTraceNo;
					gl_SavParam.usBatchNo = sav_old3->usBatchNo;
					gl_SavParam.uiMerchantListIndex = sav_old3->uiMerchantListIndex;
					gl_SavParam.uiProductsListIndex = sav_old3->uiProductsListIndex;
					gl_SavParam.uiCurOfflineNo = sav_old3->uiCurOfflineNo;
					gl_SavParam.bIsSetup = sav_old3->bIsSetup;
					memcpy(gl_SavParam.ucKey, sav_old3->ucKey, 16);
					strcpy(gl_SavParam.szOrderId, sav_old3->szOrderId);
					gl_SavParam.ucCategoryNum = sav_old3->ucCategoryNum;
					gl_SavParam.uiProductsNum = sav_old3->ucProductsNum;
					for(i = 0; i < sav_old3->ucCategoryNum; ++i)
					{
		/*				gl_SavParam.stSavParamCategory[i].uiCodeCategory = sav_old3.stSavParamCategory[i].uiCodeCategory;
						strcpy(gl_SavParam.stSavParamCategory[i].szName, sav_old3.stSavParamCategory[i].szName);
						gl_SavParam.stSavParamCategory[i].ucCategoryType = sav_old3.stSavParamCategory[i].ucCategoryType;
						gl_SavParam.stSavParamCategory[i].ucNumLogo = sav_old3.stSavParamCategory[i].ucNumLogo;*/
						memcpy(&gl_SavParam.stSavParamCategory[i], &sav_old3->stSavParamCategory[i], sizeof(xParamCategory));
					}
					for(i = 0; i < sav_old3->ucProductsNum; ++i)
					{
			/*			gl_SavParam.stSavParamProducts[i].uiCodeCategory = sav_old3.stSavParamProducts[i].uiCodeCategory;
						gl_SavParam.stSavParamProducts[i].ucType = sav_old3.stSavParamProducts[i].ucType;
						strcpy(gl_SavParam.stSavParamProducts[i].szCodeProduct, sav_old3.stSavParamProducts[i].szCodeProduct);
						gl_SavParam.stSavParamProducts[i].bCredit = sav_old3.stSavParamProducts[i].bCredit;
						strcpy(gl_SavParam.stSavParamProducts[i].sReserved, sav_old3.stSavParamProducts[i].sReserved);
						strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct, sav_old3.stSavParamProducts[i].szNameProduct);
						strcpy(gl_SavParam.stSavParamProducts[i].szAmount, sav_old3.stSavParamProducts[i].szAmount);
						strcpy(gl_SavParam.stSavParamProducts[i].szQuantity, sav_old3.stSavParamProducts[i].szQuantity);
						strcpy(gl_SavParam.stSavParamProducts[i].szSoldQuantity, sav_old3.stSavParamProducts[i].szSoldQuantity);
						strcpy(gl_SavParam.stSavParamProducts[i].szAverage, sav_old3.stSavParamProducts[i].szAverage);
						strcpy(gl_SavParam.stSavParamProducts[i].szOrderNum, sav_old3.stSavParamProducts[i].szOrderNum);
						strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, sav_old3.stSavParamProducts[i].szDiscount);*/
						memcpy(&gl_SavParam.stSavParamProducts[i], &sav_old3->stSavParamProducts[i], sizeof(xParamProducts));
						//PubDebugTx("	Quantity[%s] = %s\n", gl_SavParam.stSavParamProducts[i].szCodeProduct, gl_SavParam.stSavParamProducts[i].szQuantity);
					}
					
					memcpy(&gl_SavParam.stRoute, &sav_old3->stRoute, sizeof(sav_old3->stRoute));
					memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);

					WriteParam(WRT_ALL);
					free(lastTran_old2);
				}
				else
				{
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA), 
						(void *)&gl_SavParam, sizeof(gl_SavParam));
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM), 
						(void *)&gl_LastTran, sizeof(gl_LastTran));
				}
				free(sav_old3);
			}
		}
	}
}

unsigned char FileCpy(const unsigned char *dstFile, const unsigned char *srcFile)
{
#define filePiece 2048
	int fd, fd_tmp;
	long fileSize = 0;
	char buf[filePiece];
	if(fexist((char *)dstFile) > 0)
		remove(dstFile);
	fd = open((char *)srcFile, O_RDWR);
	if(fd < 0)
	{
		close(fd);
		return 0xFF;
	}
	fd_tmp = open((char *)dstFile,O_CREATE | O_RDWR);
	if(fd < 0)
	{
		close(fd);
		return 0xFF;
	}

	fileSize = filesize((char *)srcFile);

	if(fileSize>filePiece)//如果2M以上就分次复制
	{
		int n = fileSize / filePiece;// n 块
		int leavesize = fileSize % filePiece;
		int i;
		for(i = 0;i < n; ++i)
		{
			read(fd, buf,sizeof(buf));
			write(fd_tmp,buf,sizeof(buf));
		}
		read(fd, buf,leavesize);
		write(fd_tmp,buf,leavesize);
	}
	else //否则直接复制
	{
		read(fd, buf,fileSize);
		write(fd_tmp,buf,fileSize);
	}
	close(fd);
	close(fd_tmp);
#undef filePiece
	return OK;
}

enCodeReturn SaveOfflineTrans(const TRANS_DATA *pstCurTrans, const unsigned char *FileName)
{
	unsigned int i;
	int fd = open((char *)FileName,O_CREATE | O_RDWR);
	if(fd < 0)
	{
		close(fd);
		return 0xFF;
	}

	seek(fd, 0, SEEK_END);
	write(fd, (void *)pstCurTrans, sizeof(TRANS_DATA) - sizeof(pstCurTrans->stProducts));
	if (pstCurTrans->uiProductNum > NB_MAX_PRODUCTS)
	{
		close(fd);
		return PB_SIZE_PARAMETRE;
	}
	for (i = 0; i < pstCurTrans->uiProductNum; ++i)
	{
		write(fd, (void *)&pstCurTrans->stProducts[i], sizeof(PRODUCT_INFO));
	}
	close(fd);
	return OK;
}

unsigned int GetOfflineTransCnt(unsigned char type)
{
	unsigned int i;
	unsigned int cnt = 0;
	TRANS_DATA tmp;
	int fd = open(FILE_OFFLINE, O_RDWR);
	long Len = filesize(FILE_OFFLINE);
	if(fd < 0)
	{
		close(fd);
		return 0;
	}

	seek(fd, 0, SEEK_SET);
	while(Len > 0)
	{
		read(fd, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
		Len -= (sizeof(TRANS_DATA) - sizeof(tmp.stProducts));

		for (i = 0; i <tmp.uiProductNum; ++i)
		{
			read(fd, (void *)&tmp.stProducts[i], sizeof(PRODUCT_INFO));
		}
		Len -= tmp.uiProductNum * sizeof(PRODUCT_INFO);
		if(tmp.ucTransType == type)
			++cnt;
	}
	close(fd);
	return cnt;
}

unsigned char SaveOffCancel(const TRANS_DATA *pstCurTrans)
{
	int fd = open(FILE_OFF_CANCEL,O_CREATE | O_RDWR);
	if(fd < 0)
	{
		close(fd);
		return 0xFF;
	}

	seek(fd, 0, SEEK_END);
	write(fd, (void *)pstCurTrans, sizeof(TRANS_DATA) - sizeof(pstCurTrans->stProducts));
	close(fd);
	return OK;
}

unsigned char GetOfflineOrder(unsigned int index, TRANS_DATA *pstCurTrans)
{
	unsigned int i;
	unsigned int num = 0;
	TRANS_DATA tmp;
	int fd = open(FILE_OFFLINE, O_RDWR);
	if(fd < 0 || 0 == gl_SavParam.uiCurOfflineNo)
	{
		close(fd);
		return 0xFF;
	}

	memset(&tmp, 0, sizeof(TRANS_DATA));
	seek(fd, 0, SEEK_SET);
	for (i = 0; i < gl_SavParam.uiCurOfflineNo; ++i)
	{
		read(fd, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
		read(fd, (void *)tmp.stProducts, tmp.uiProductNum * sizeof(PRODUCT_INFO));
		if(ID_OfflineOrder != tmp.ucTransType)
			continue;
		++num;
		if(index + 1 == num)
			break;
	}
	close(fd);
	memcpy(pstCurTrans, &tmp, sizeof(TRANS_DATA));
	return OK;
}

unsigned char DelOfflineOrder(unsigned int index)
{
	unsigned int i;
	unsigned int num = 0;
	TRANS_DATA tmp;
	int fd_dst, fd_src;
	
	memset(&tmp, 0, sizeof(TRANS_DATA));
	remove(FILE_OFFLINE_TMP);
	fd_src = open(FILE_OFFLINE, O_RDWR);
	fd_dst = open(FILE_OFFLINE_TMP, O_CREATE | O_RDWR);
	if(fd_src < 0 || fd_dst < 0 || 0 == gl_SavParam.uiCurOfflineNo)
	{
		close(fd_src);
		close(fd_dst);
		return 0xFF;
	}

	seek(fd_src, 0, SEEK_SET);
	for (i = 0; i < gl_SavParam.uiCurOfflineNo; ++i)
	{
		read(fd_src, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
		read(fd_src, (void *)tmp.stProducts, tmp.uiProductNum * sizeof(PRODUCT_INFO));
		if(ID_OfflineOrder != tmp.ucTransType)
		{
			write(fd_dst, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
			write(fd_dst, (void *)tmp.stProducts, tmp.uiProductNum * sizeof(PRODUCT_INFO));
			continue;
		}

		if(num != index)
		{
			write(fd_dst, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
			write(fd_dst, (void *)tmp.stProducts, tmp.uiProductNum * sizeof(PRODUCT_INFO));	
		}
		++num;
	}
	
	close(fd_src);
	close(fd_dst);
	remove(FILE_OFFLINE);
	FileCpy(FILE_OFFLINE, FILE_OFFLINE_TMP);
	remove(FILE_OFFLINE_TMP);
	return OK;
}

unsigned char GetOfflineLastTrans(TRANS_DATA *pstCurTrans)
{
	unsigned int i;
	unsigned char num = 0; //order product list
	int fd = open(FILE_OFFLINE, O_RDWR);
	if(fd < 0)
	{
		close(fd);
		return 0xFF;
	}

	seek(fd, 0, SEEK_SET);
	if(gl_SavParam.uiCurOfflineNo > 0)
	{
		for (i = 0; i < gl_SavParam.uiCurOfflineNo - 1; ++i)
		{
			seek(fd, sizeof(TRANS_DATA) - sizeof(PRODUCT_INFO) * NB_MAX_PRODUCTS - 1, SEEK_CUR);
			read(fd, &num, 1);
			seek(fd, sizeof(PRODUCT_INFO) * num, SEEK_CUR);
		}
	}
	else
	{	
		close(fd);
		return 1;
	}
	read(fd, (void *)pstCurTrans, sizeof(TRANS_DATA) - sizeof(pstCurTrans->stProducts));
	for (i = 0; i < pstCurTrans->uiProductNum; ++i)
	{
		read(fd, (void *)&pstCurTrans->stProducts[i], sizeof(PRODUCT_INFO));
	}
	close(fd);
	return OK;
}

unsigned char DeleteOfflineLastTrans()
{
	unsigned int i;
	TRANS_DATA tmp;
	int fd = open(FILE_OFFLINE, O_RDWR);
	int fd2= open(FILE_OFFLINE_TMP, O_CREATE | O_RDWR);
	if(fd < 0 || fd2 < 0)
	{
		close(fd);
		close(fd2);
		return 0xFF;
	}

	seek(fd, 0, SEEK_SET);
	if(gl_SavParam.uiCurOfflineNo > 0)
	{
		for (i = 0; i < gl_SavParam.uiCurOfflineNo - 1; ++i)
		{
			read(fd, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
			write(fd2, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
			read(fd, (void *)tmp.stProducts, sizeof(PRODUCT_INFO) * tmp.uiProductNum);
			write(fd2, (void *)tmp.stProducts, sizeof(PRODUCT_INFO) * tmp.uiProductNum);
		}
	
		close(fd);
		close(fd2);
		remove(FILE_OFFLINE);
		FileCpy(FILE_OFFLINE, FILE_OFFLINE_TMP);
		remove(FILE_OFFLINE_TMP);
		--gl_SavParam.uiCurOfflineNo;
	}
	else
	{
		close(fd);
		close(fd2);
		remove(FILE_OFFLINE_TMP);
		return 0xFF;
	}
	return OK;
}

unsigned char WriteMerchant(const unsigned char *fileName, xMerchantList stMerchant)
{
	int fd;
	fd = open((char *)fileName, O_CREATE | O_RDWR);

	if(fd < 0)
		return 0xFF;

	seek(fd, 0, SEEK_END);
	write(fd, (void *)&stMerchant, sizeof(xMerchantList));
	close(fd);
	return OK;
}

unsigned char GetMerchant(const unsigned char *fileName, unsigned int index, xMerchantList *pstMerchant)
{
	int fd;
	fd = open((char *)fileName, O_RDWR);

	if(fd < 0)
	{
		memset(pstMerchant, 0, sizeof(xMerchantList));
		return 0xFF;
	}

	seek(fd, index * sizeof(xMerchantList), SEEK_SET);
	read(fd, (void *)pstMerchant, sizeof(xMerchantList));
	close(fd);
	return OK;
}


int PubFtpFileRead(unsigned char *pszFileName, long lOffset, void *psData, int iLen)
{
	int	iRet, fd, iReadBytes;

	if (pszFileName == NULL || iLen < 0)
	{
		sprintf(gl_szFTPErrInfo, "%03d Read File Para Error", FTPERROR);
		return FTPERROR;
	}

	fd = open((char *)pszFileName, O_RDWR);
	if (fd < 0)
	{
		sprintf(gl_szFTPErrInfo, "%03d File Open Error", FTPFileOpenERR);
		return FTPFileOpenERR;
	}

	iRet = seek(fd, lOffset, SEEK_SET);
	if (iRet < 0)
	{
		close(fd);
		sprintf(gl_szFTPErrInfo, "%03d File Seek Error", FTPFileSeekERR);
		return FTPFileSeekERR;
	}

	iReadBytes = read(fd, psData, iLen);
	close(fd);
	if (iReadBytes > 0 && iReadBytes != iLen)
	{
		sprintf(gl_szFTPErrInfo, "%03d File Read Error", FTPFileReadLenERR);
		return FTPFileReadLenERR;

	}
	if (iReadBytes <= 0)
	{
		sprintf(gl_szFTPErrInfo, "%03d File Read Error", FTPFileReadERR);
		return FTPFileReadERR;
	}

	return OK;
}


int PubFtpFileWrite(unsigned char *pszFileName, long lOffset, void *psData, int iLen)
{
	int	 iRet;
	int  fd, iWriteBytes;

	if (pszFileName == NULL || iLen < 0)
	{
		sprintf(gl_szFTPErrInfo, "%03d File Para Error", FTPERROR);
		return FTPERROR;
	}

	fd = open((char *)pszFileName, O_CREATE | O_RDWR);
	if (fd < 0)
	{
		sprintf(gl_szFTPErrInfo, "%03d Write File Open Error", FTPFileOpenERR);
		return FTPFileOpenERR;
	}

	iRet = seek(fd, lOffset, SEEK_SET);
	if (iRet < 0)
	{
		sprintf(gl_szFTPErrInfo, "%03d File Seek Error", FTPFileSeekERR);
		return FTPFileSeekERR;
	}

	iWriteBytes = write(fd, psData, iLen);
	close(fd);
	if (iWriteBytes!=iLen)
	{
		sprintf(gl_szFTPErrInfo, "%03d File Write Error", FTPFileWriteERR);
		return FTPFileWriteERR;
	}

	return OK;
}


int ReadFTPFileInfo(void)
{
	int iRet;

	if(fexist(FTPFILE) < 0)
	{
		memset(&gl_stFTPFileInfo, 0, sizeof(FTPFILE_INFO));
		return (WriteFTPFileInfo());
	}


	iRet = PubFtpFileRead(FTPFILE, 0, (void *)&gl_stFTPFileInfo, sizeof(FTPFILE_INFO));
	if (iRet != OK)
	{
		return iRet;
	}

	return OK;
}


int WriteFTPFileInfo(void)
{
	return PubFtpFileWrite(FTPFILE, 0, (char *)&gl_stFTPFileInfo, sizeof(FTPFILE_INFO));
}



//////////////////////////////////////////////////////////////////////////
//					the following is static func
//////////////////////////////////////////////////////////////////////////

static void DataInitDef()
{
	int i;
	memset(&gl_MaintenaceParam, 0, sizeof(MAINTENACE_PARAM));
	memset(&gl_MaintenaceParam_Ext, 0, sizeof(MAINTENACE_PARAM_EXT)); // Added by Kim_LinHB 2013-7-12 v4.60

	//gl_MaintenaceParam.bChangeDiscount = 0;
	strcpy(gl_MaintenaceParam.szLogLv ,"0");
	//strcpy(gl_MaintenaceParam.szDefDiscount,"425");
	
	gl_MaintenaceParam.ucLang = LANG_EN;
	strcpy(gl_MaintenaceParam.szMaintenancePwd, "2049");
	strcpy(gl_MaintenaceParam.szNetworkPwd, "8844");
	strcpy(gl_MaintenaceParam.stUser.szName, "salesman");
	strcpy(gl_MaintenaceParam.stUser.szPwd,"12345");
	strcpy(gl_MaintenaceParam.szTimeSettDownload, "0000");
	strcpy(gl_MaintenaceParam.stConnParam.szNumPad, gl_pLangSelPiont[STR_DEFAUT_NUMERO_PAD_PRINC]);
	strcpy(gl_MaintenaceParam.stConnParam.szNumPadSec, gl_pLangSelPiont[STR_DEFAUT_NUMERO_PAD_SEC]);
	
	gl_MaintenaceParam.ucConnSel = 0;
	gl_MaintenaceParam.stConnParam.ucTeleUsing = 0;
	gl_MaintenaceParam.stConnParam.stParamEth.bDHCP = 1;

	strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, "DATASIMCARD.COM");
	strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP,"193.169.190.115");
	strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, "6062");
	gl_MaintenaceParam.ucConnType[0] = gl_MaintenaceParam.ucConnType[1] = GPRS;

	gl_MaintenaceParam_Ext.cConnectionTimeout = TIME_OUT_CONN;

	{ // Added by Kim_LinHB 2013-3-25 v4.55
		unsigned char tmp[32];
		GetSerialNumber(tmp);
		strcpy(gl_MaintenaceParam.szTermID, tmp);
	} // Add End

	gl_MaintenaceParam.stConnParam.stParamGPRS.ucAuth = 0x01;
	gl_MaintenaceParam.bInputSerial = 1;

	strcpy(gl_MaintenaceParam.stFTP.stApp.szFileVer, STR_VERSION_SUB);
	strcpy(gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, "0000"); // Added by Kim_LinHB 2013-3-20 v4.55 

	memset(&gl_LastTran, 0, sizeof(TRANS_DATA));
	memset(&gl_SavParam, 0, sizeof(SAV_PARAM));
	for (i = 0; i < NB_MAX_PRODUCTS; ++i)
	{
		strcpy(gl_SavParam.stSavParamProducts[i].szAmount, "0");
		strcpy(gl_SavParam.stSavParamProducts[i].szAverage, "0");
		strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, "0");
		strcpy(gl_SavParam.stSavParamProducts[i].szSoldQuantity, "0");
		strcpy(gl_SavParam.stSavParamProducts[i].szQuantity, "0");
		strcpy(gl_SavParam.stSavParamProducts[i].szOrderNum, "0");
	}
	gl_SavParam.usBatchNo = 0;
	gl_SavParam.usTraceNo = 0;
	
	gl_SavParam.bIsSetup = 1;

	if (fexist(FILE_MERCHANT))
	{
		remove(FILE_MERCHANT);
	}

	// it's in the Ingevoucher Origin, Don't understand how it work, so it's here and useless
	// and the value is saved in gl_SavParam.ucKey 
	GenerationdPOSKey(gl_TermSN, gl_SavParam.ucKey);  
}
