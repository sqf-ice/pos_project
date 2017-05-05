#include <posapi.h>
#include <posapi_all.h>
#include "appLib.h"
#include "framework.h"
#include "data_define_merchant.h"
#include "connect.h"
#include "calc_merchant.h"

//////////////////////////////////////////////////////////////////////////
const unsigned char *cSTRConnType[] = 
{
	"SIMUL",
// 	"Port Com",
// 	"Modem",
	"ETHERNET",
	"GPRS",
//	"PPP",
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
MAINTENACE_PARAM_EXT gl_MaintenaceParam_Ext; // Added by Kim_LinHB 2013-6-19 v4.61
SAV_PARAM		gl_SavParam;
//STOCK			gl_Stock;			// Removed by Kim_LinHB 2013-8-12 v4.62
TRANS_DATA		gl_LastTran;
unsigned char	gl_ProvisionIdTmp[SIZE_MAX_PROVISION_ID+1];

USER_INFO	gl_Cur_Cashier = { "","" };
unsigned char	gl_bCashier_Login = FALSE;

unsigned char	gl_bOnService = FALSE;

unsigned char gl_bPlanned_SettDownload = FALSE;
unsigned char gl_bPlanned_SalesPolling = FALSE;
unsigned char gl_bPlanned_SoftDownload = FALSE;


unsigned char gl_szFTPErrInfo[256];
FTPFILE_INFO gl_stFTPFileInfo;

#define RESTOCKPINS_FILE_GROUP  "PIN%d.T"
#define PENDINGORD_FILE_GROUP  "PEND%d.P"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static void DataInitDef();

//////////////////////////////////////////////////////////////////////////
// Modified by Kim_LinHB 2013-8-12 v4.62
enCodeReturn  WriteParam(unsigned char type)
{
	int iRet;

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
// Modify End

// Added by Kim_LinHB 2013-8-12 v4.62
enCodeReturn WriteStock_All(const STOCK *stock)
{
	if(-1 != fexist(FILE_STOCK))
	{
		int fd;
		fd = open(FILE_STOCK, O_RDWR);

		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return 0xFF;
		}
		if(stock)
		{
			write(fd, (void *)&stock->stStock, sizeof(stock->stStock));
			write(fd, (void *)&stock->stVoucher, sizeof(stock->stVoucher));
		}
		close(fd);
	}
	else
	{
		int fd;
		fd = open(FILE_STOCK, O_CREATE | O_RDWR);
		if(stock)
		{
			write(fd, (void *)&stock->stStock, sizeof(stock->stStock));
			write(fd, (void *)&stock->stVoucher, sizeof(stock->stVoucher));
		}
		else
		{
			int i;
			STOCK stTmp_stock;
			memset(&stTmp_stock, 0, sizeof(STOCK));
			// Added by Kim_LinHB 2013-3-27 v4.56
			for (i = 0; i < NB_MAX_CODE_STOCK; ++i)
			{
				strcpy(stTmp_stock.stVoucher[i].szExpirDate, "999999999999");
			}
			write(fd, (void *)&stTmp_stock.stStock, sizeof(stTmp_stock.stStock));
			write(fd, (void *)&stTmp_stock.stVoucher, sizeof(stTmp_stock.stVoucher));
		}
		close(fd);
	}
	return OK;
}

enCodeReturn ReadStock_All(STOCK *stock)
{
	int fd;
	//remove(FILE_STOCK); // Removed by Kim_LinHB 2013-3-27 v4.56
	fd = open(FILE_STOCK, O_RDWR);

	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}
	read(fd, (void *)&stock->stStock, sizeof(stock->stStock));
	read(fd, (void *)&stock->stVoucher, sizeof(stock->stVoucher));

	close(fd);
	return OK;
}

enCodeReturn WriteStock(int index, const STOCK_INFO *stock_info, const VOUCHER_INFO *voucher)
{
	int fd;
	fd = open(FILE_STOCK, O_RDWR);

	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}
	seek(fd, index * sizeof(STOCK_INFO), SEEK_SET);
	if(stock_info)
		write(fd, (void *)stock_info, sizeof(STOCK_INFO));
	seek(fd, NB_MAX_CODE_STOCK * sizeof(STOCK_INFO), SEEK_SET);
	seek(fd, index * sizeof(VOUCHER_INFO), SEEK_CUR);
	if(voucher)
		write(fd, (void *)voucher, sizeof(VOUCHER_INFO));

	close(fd);
	return OK;
}

enCodeReturn ReadStock(int index, STOCK_INFO *stock_info, VOUCHER_INFO *voucher)
{
	int fd;
	fd = open(FILE_STOCK, O_RDWR);

	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}
	seek(fd, index * sizeof(STOCK_INFO), SEEK_SET);
	if(stock_info)
		read(fd, (void *)stock_info, sizeof(STOCK_INFO));
	seek(fd, NB_MAX_CODE_STOCK * sizeof(STOCK_INFO), SEEK_SET);
	seek(fd, index * sizeof(VOUCHER_INFO), SEEK_CUR);
	if(voucher)
		read(fd, (void *)voucher, sizeof(VOUCHER_INFO));

	close(fd);
	return OK;
}

// Added by Kim_LinHB 2013-12-5 v4.76
enCodeReturn UpdateStockOrder(const STOCK_NEW *stock)
{
	char bFound = TRUE;
	unsigned char file[255];
	int i = 0;
	while(bFound && i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, RESTOCKPINS_FILE_GROUP, i++);
		if(-1 == fexist(file))
		{
			bFound = FALSE;
		}
		else	// for the case that one Order is split into 2 or more than 2 pieces
		{
			int fd;
			STOCK_NEW temp;
			fd = open(file, O_RDWR);

			if(fd < 0)
			{
				close(fd);
				ScrCls();
				PubDispString("File Error", 3 | DISP_LINE_CENTER);
				PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
				while(1);
				return 0xFF;
			}
			read(fd, (void *)&temp, sizeof(STOCK_NEW));
			if(0 == strcmp(temp.szOrderId, stock->szOrderId))
			{
				seek(fd, 0, SEEK_SET);
				write(fd, (void *)stock, sizeof(STOCK_NEW));
				close(fd);
				return OK;
			}

			close(fd);
		}
	}

	if(!bFound)
	{
		int fd;
		fd = open(file, O_CREATE |O_RDWR);

		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return 0xFF;
		}
		write(fd, (void *)stock, sizeof(STOCK_NEW));
		
		close(fd);
		return OK;
	}
	else
		return 0xFF;
}

enCodeReturn ReadStockOrder(STOCK_NEW *stock)
{
	char bFound = FALSE;
	unsigned char file[255];
	int i = 0;
	while(!bFound && i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, RESTOCKPINS_FILE_GROUP, i++);
		if(-1 != fexist(file))
		{
			bFound = TRUE;
		}
	}

	if(bFound)
	{
		int fd;
		fd = open(file, O_RDWR);

		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return 0xFF;
		}
		read(fd, (void *)stock, sizeof(STOCK_NEW));

		close(fd);
		return OK;
	}
	else
		return 0xFF;
}

// Added by Kim_LinHB 2013-12-17 v4.78
enCodeReturn GetStockOrderById(const unsigned char *OrderId, STOCK_NEW *stock)
{
	unsigned char file[255];
	int i = 0;
	STOCK_NEW tempStock;
	while(i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, RESTOCKPINS_FILE_GROUP, i++);
		if(-1 != fexist(file))
		{
			int fd;
			fd = open(file, O_RDWR);

			if(fd < 0)
			{
				close(fd);
				ScrCls();
				PubDispString("File Error", 3 | DISP_LINE_CENTER);
				PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
				while(1);
				return 0xFF;
			}
			read(fd, (void *)&tempStock, sizeof(STOCK_NEW));
			if(0 == strcmp(tempStock.szOrderId, OrderId))
			{
				memcpy(stock, &tempStock, sizeof(STOCK_NEW));
				close(fd);
				return OK;
			}

			close(fd);
		}
	}

	return 0xFF;
}

enCodeReturn GetStockOrderByProvisionId(const unsigned char *ProvisionId, STOCK_NEW *stock)
{
	unsigned char file[255];
	int i = 0;
	STOCK_NEW tempStock;
	while(i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, RESTOCKPINS_FILE_GROUP, i++);
		if(-1 != fexist(file))
		{
			int fd;
			fd = open(file, O_RDWR);

			if(fd < 0)
			{
				close(fd);
				ScrCls();
				PubDispString("File Error", 3 | DISP_LINE_CENTER);
				PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
				while(1);
				return 0xFF;
			}
			read(fd, (void *)&tempStock, sizeof(STOCK_NEW));
			if(atol(tempStock.szProvisionID) > 0 && 0 == strcmp(tempStock.szProvisionID, ProvisionId))
			{
				memcpy(stock, &tempStock, sizeof(STOCK_NEW));
				close(fd);
				return OK;
			}

			close(fd);
		}
	}

	return 0xFF;
}


enCodeReturn DelStockOrder()
{
	char bFound = FALSE;
	unsigned char file[255];
	int i = 0;
	while(!bFound && i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, RESTOCKPINS_FILE_GROUP, i++);
		if(-1 != fexist(file))
		{
			bFound = TRUE;
		}
	}

	if(bFound)
	{
		remove(file);
		return OK;
	}
	return 0xFF;
}

enCodeReturn WritePendingOrder(PENDING_ORD order)
{
	char bFound = TRUE;
	unsigned char file[255];
	int i = 0;
	while(bFound && i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, PENDINGORD_FILE_GROUP, i++);
		if(-1 == fexist(file))
		{
			bFound = FALSE;
		}
	}

	if(!bFound)
	{
		int fd;
		fd = open(file, O_CREATE |O_RDWR);

		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return 0xFF;
		}
		write(fd, (void *)&order, sizeof(order));

		close(fd);
		return OK;
	}
	else
		return 0xFF;
}
enCodeReturn ReadPendingOrder(PENDING_ORD *order)
{
	char bFound = FALSE;
	unsigned char file[255];
	int i = 0;
	while(!bFound && i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, PENDINGORD_FILE_GROUP, i++);
		if(-1 != fexist(file))
		{
			bFound = TRUE;
		}
	}

	if(bFound)
	{
		int fd;
		fd = open(file, O_RDWR);

		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return 0xFF;
		}
		read(fd, (void *)order, sizeof(PENDING_ORD));

		close(fd);
		return OK;
	}
	else
		return 0xFF;
}
enCodeReturn DelPendingOrder()
{
	char bFound = FALSE;
	unsigned char file[255];
	int i = 0;
	while(!bFound && i < NB_MAX_CODE_STOCK)
	{
		sprintf(file, PENDINGORD_FILE_GROUP, i++);
		if(-1 != fexist(file))
		{
			bFound = TRUE;
		}
	}

	if(bFound)
	{
		remove(file);
		return OK;
	}
	return 0xFF;
}
// Add End

enCodeReturn SaveOLTransInfo(const TRANS_DATA *pstCur)
{
	int fd;
	if(!pstCur)
	{
		remove(FILE_TRANS_OL);
		return OK;
	}
	// Added by Kim_LinHB 2013-9-3 v4.67
	if(filesize(FILE_TRANS_OL) > sizeof(TRANS_DATA) * 100)
		remove(FILE_TRANS_OL);
	// Add End

	fd = open(FILE_TRANS_OL, O_CREATE | O_RDWR);

	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}
	seek(fd, 0, SEEK_END);
	write(fd, (void *)pstCur, sizeof(TRANS_DATA));

	close(fd);
	return OK;
}
// Add End

enCodeReturn FindOLTranWithProvisionId(TRANS_DATA *pstTran)
{
	int fd;
	TRANS_DATA stTemp;
	long Len = filesize(FILE_TRANS_OL);
	fd = open(FILE_TRANS_OL, O_RDWR);

	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}

	seek(fd, 0, SEEK_SET);

	while(Len > 0)
	{
		read(fd, (void *)&stTemp, sizeof(TRANS_DATA));
		if(0 == strcmp(stTemp.szProvisionID, pstTran->szProvisionID))
		{
			memcpy(pstTran, &stTemp, sizeof(TRANS_DATA));
			return OK;
		}
		Len -= sizeof(TRANS_DATA);
	}

	close(fd);
	return 0xFF;
}

enCodeReturn DelOLTranByProvisionId(TRANS_DATA stTran)
{
	int fd_src, fd_dst;
	TRANS_DATA stTemp;
	long Len;
	
	fd_dst = open("TranOL_TMP", O_CREATE | O_RDWR);
	Len = filesize(FILE_TRANS_OL);
	fd_src = open(FILE_TRANS_OL, O_RDWR);

	if(fd_src < 0 || fd_dst < 0)
	{
		close(fd_src);
		close(fd_dst);
		remove("TranOL_TMP");
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}

	seek(fd_src, 0, SEEK_SET);

	while(Len > 0)
	{
		read(fd_src, (void *)&stTemp, sizeof(TRANS_DATA));
		if(strcmp(stTemp.szProvisionID, stTran.szProvisionID))
			write(fd_dst, (void *)& stTemp, sizeof(TRANS_DATA));
		Len -= sizeof(TRANS_DATA);
	}

	close(fd_src);
	close(fd_dst);
	FileCpy(FILE_TRANS_OL, "TranOL_TMP");
	remove("TranOL_TMP");
	return OK;
}

enCodeReturn SaveCashOrder(const TRANS_DATA_CASH *pstCur)
{
	int fd;
	if(!pstCur)
	{
		remove(FILE_CASHORDER);
		return OK;
	}

	fd = open(FILE_CASHORDER, O_CREATE | O_RDWR);

	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}
	seek(fd, 0, SEEK_END);
	write(fd, (void *)pstCur, sizeof(TRANS_DATA_CASH));

	close(fd);
	return OK;
}

enCodeReturn FindCashOrderWithProvisionId(TRANS_DATA_CASH *pstTran)
{
	int fd;
	TRANS_DATA_CASH stTemp;
	long Len = filesize(FILE_CASHORDER);
	fd = open(FILE_CASHORDER, O_RDWR);

	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}

	seek(fd, 0, SEEK_SET);

	while(Len > 0)
	{
		read(fd, (void *)&stTemp, sizeof(TRANS_DATA_CASH));
		if(0 == strcmp(stTemp.szProvisionID, pstTran->szProvisionID))
		{
			memcpy(pstTran, &stTemp, sizeof(TRANS_DATA_CASH));
			return OK;
		}
		Len -= sizeof(TRANS_DATA_CASH);
	}

	close(fd);
	return 0xFF;
}

enCodeReturn DelCashOrderByProvisionId(TRANS_DATA_CASH stTran)
{
	int fd_src, fd_dst;
	TRANS_DATA_CASH stTemp;
	long Len;

	fd_dst = open("TranOL_TMP", O_CREATE | O_RDWR);
	Len = filesize(FILE_CASHORDER);
	fd_src = open(FILE_CASHORDER, O_RDWR);

	if(fd_src < 0 || fd_dst < 0)
	{
		close(fd_src);
		close(fd_dst);
		remove("TranOL_TMP");
		return 0xFF;
	}

	seek(fd_src, 0, SEEK_SET);

	while(Len > 0)
	{
		read(fd_src, (void *)&stTemp, sizeof(TRANS_DATA_CASH));
		if(strcmp(stTemp.szProvisionID, stTran.szProvisionID))
			write(fd_dst, (void *)&stTemp, sizeof(TRANS_DATA_CASH));
		Len -= sizeof(TRANS_DATA_CASH);
	}

	close(fd_src);
	close(fd_dst);
	FileCpy(FILE_CASHORDER, "TranOL_TMP");
	remove("TranOL_TMP");
	return OK;
}

void DataInit()
{
	// Add End
	ReadFTPFileInfo();
	if(gl_stFTPFileInfo.lCurrLen == gl_stFTPFileInfo.lTotalLen)
	{
		remove(FILE_UPDATE);
	}
	memset(gl_ProvisionIdTmp, 0 ,sizeof(gl_ProvisionIdTmp));

	// Added by Kim_LinHB 2013-7-2 v4.61
	if(-1 != fexist(FILE_STOCK_OLD1))
	{
		typedef struct _tagStock_OLD
		{
			STOCK_INFO		stStock[NB_MAX_CODE_STOCK];
			VOUCHER_INFO_OLD	stVoucher[NB_MAX_CODE_STOCK];
		}STOCK_OLD;
		int i;
		STOCK_OLD tmp;
		STOCK stock;

		int fd = open(FILE_STOCK_OLD1,O_RDWR);
		if(fd < 0)
		{
			close(fd);
			ScrCls();
			PubDispString("File Error", 3 | DISP_LINE_CENTER);
			PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
			while(1);
			return;
		}
		read(fd, (void *)&tmp.stStock, sizeof(tmp.stStock));
		read(fd, (void *)&tmp.stVoucher, sizeof(tmp.stVoucher));

		for (i = 0; i < NB_MAX_CODE_STOCK; ++i)
		{
			memcpy(&stock.stStock[i], &tmp.stStock[i], sizeof(STOCK_INFO));
			strcpy(stock.stVoucher[i].szSlipMsg, tmp.stVoucher[i].szSlipMsg);
			strcpy(stock.stVoucher[i].szCurrDeposit, tmp.stVoucher[i].szCurrDeposit);
			strcpy(stock.stVoucher[i].szProductId, tmp.stVoucher[i].szProductId);
			strcpy(stock.stVoucher[i].szVoucherId, tmp.stVoucher[i].szVoucherId);
			strcpy(stock.stVoucher[i].szSerial, tmp.stVoucher[i].szSerial);
			strcpy(stock.stVoucher[i].szNum, tmp.stVoucher[i].szNum);
			strcpy(stock.stVoucher[i].szExpirDate, tmp.stVoucher[i].szExpirDate);
		}
		WriteStock_All(&stock);
		close(fd);
		remove(FILE_STOCK_OLD1);
	}
	// Add End

	if(gl_bFirstRun)
	{
		// Added by Kim_LinHB 2013-5-2 v4.58, for updating from version too old
// 		if(-1 != fexist(FILE_PARAM))
// 			remove(FILE_PARAM);
 		if(-1 != fexist(FILE_USERS))
 			remove(FILE_USERS);
 		if(-1 != fexist(FILE_USERS_TMP))
 			remove(FILE_USERS_TMP);
 		if(-1 != fexist(FILE_TRANS_OL))
 			remove(FILE_TRANS_OL);
 		if(-1 != fexist(FILE_CASHORDER))
 			remove(FILE_CASHORDER);

		DataInitDef();
		WriteParam(WRT_ALL);
		WriteStock_All(NULL);
	}
	else
	{
		int i; // Modified by Kim_LinHB 2013-7-4
		unsigned char buffer[sizeof(MAINTENACE_PARAM)];
		int fd = open(FILE_PARAM,O_RDWR);
		if(fd < 0)
		{
			close(fd);
			DataInitDef();
			return;
		}
		seek(fd, 0, SEEK_SET);
		read(fd,buffer, 1);
		read(fd, (void *)&gl_MaintenaceParam, sizeof(MAINTENACE_PARAM));
		
		if(gl_MaintenaceParam.ucVersion < 0x62)
		{
			SAV_PARAM_OLD tmp2;
			read(fd, (void *)&tmp2, sizeof(tmp2) - sizeof(tmp2.stSavParamCategory) - sizeof(tmp2.stSavParamProducts));

			for (i = 0; i < tmp2.ucCategoryNum; ++i)
				read(fd, (void *)&tmp2.stSavParamCategory[i], sizeof(tmp2.stSavParamCategory)/NB_MAX_CATEGORYS);
			for (i = 0; i < tmp2.ucProductsNum; ++i)
				read(fd, (void *)&tmp2.stSavParamProducts[i], sizeof(tmp2.stSavParamProducts)/NB_MAX_PRODUCTS);
		
			read(fd, (void *)&gl_LastTran, sizeof(TRANS_DATA));
			read(fd, (void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT)); // Added by Kim_LinHB 2013-6-19 v4.61
		
			// Removed by Kim_LinHB 2013-11-14 v4.73
			// Modified by Kim_LinHB 2013-8-2 v4.62
			//if(0 == strlen(gl_MaintenaceParam.szLastSalesPolling_Time))
			//	GetCurTime(gl_MaintenaceParam.szLastSalesPolling_Time);
			//memcpy(&gl_MaintenaceParam.szLastSalesPolling_Time[6], gl_MaintenaceParam.szTimeSalesPolling, 4);
			// Modify End
			//.....
			close(fd);
			remove(FILE_PARAM);

			// Added by Kim_LinHB 2013-11-21 v4.73
			gl_SavParam.usTraceNo = tmp2.usTraceNo;
			gl_SavParam.usBatchNo = tmp2.usBatchNo;
			gl_SavParam.bIsSetup = tmp2.bIsSetup;
			gl_SavParam.bDownloadLogoToDo = tmp2.bDownloadLogoToDo;

			memcpy(gl_SavParam.ucKey, tmp2.ucKey, 16);
			memcpy(&gl_SavParam.stClaim, &tmp2.stClaim,sizeof(tmp2.stClaim));

			gl_SavParam.uiProductsListIndex = tmp2.uiProductsListIndex;

			gl_SavParam.ucCategoryNum = tmp2.ucCategoryNum;
			gl_SavParam.ucProductsNum = tmp2.ucProductsNum;

			for(i = 0; i < tmp2.ucCategoryNum; ++i)
			{
				gl_SavParam.stSavParamCategory[i].uiCodeCategory = tmp2.stSavParamCategory[i].ucCodeCategory;
				strcpy(gl_SavParam.stSavParamCategory[i].szName, tmp2.stSavParamCategory[i].szName);
				strcpy(gl_SavParam.stSavParamCategory[i].szName_AR, tmp2.stSavParamCategory[i].szName_AR);
				gl_SavParam.stSavParamCategory[i].ucCategoryType = tmp2.stSavParamCategory[i].ucCategoryType;
				gl_SavParam.stSavParamCategory[i].ucNumLogo = tmp2.stSavParamCategory[i].ucNumLogo;
			}

			for(i = 0; i < tmp2.ucProductsNum; ++i)
			{
				gl_SavParam.stSavParamProducts[i].uiCodeCategory = tmp2.stSavParamProducts[i].ucCodeCategory;
				gl_SavParam.stSavParamProducts[i].ucCategory_Index = tmp2.stSavParamProducts[i].ucCategory_Index;
				gl_SavParam.stSavParamProducts[i].ucType = tmp2.stSavParamProducts[i].ucType;
				strcpy(gl_SavParam.stSavParamProducts[i].szCodeProduct, tmp2.stSavParamProducts[i].szCodeProduct);
				strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct, tmp2.stSavParamProducts[i].szNameProduct);
				strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct_AR, tmp2.stSavParamProducts[i].szNameProduct_AR);
				strcpy(gl_SavParam.stSavParamProducts[i].szAmount, tmp2.stSavParamProducts[i].szAmount);
				strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, tmp2.stSavParamProducts[i].szDiscount);
				gl_SavParam.stSavParamProducts[i].usNbMaxRestock = tmp2.stSavParamProducts[i].usNbMaxRestock;
			}
			// Add End

			// Added by Kim_LinHB 2013-9-3 v4.67 for setting download after remote download
			memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
			// Add End
			WriteParam(WRT_ALL);
		}
		else
		{
			long len;
			SAV_PARAM_OLD sav_old2;
			close(fd);
			len = filesize(FILE_PARAM);
			PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM), 
				(void *)&gl_LastTran, sizeof(TRANS_DATA));
			if(1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA) + sizeof(sav_old2) + sizeof(MAINTENACE_PARAM_EXT) == len )
				PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA) + sizeof(sav_old2),
				(void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT));
			else
				PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA) + sizeof(SAV_PARAM),
				(void *)&gl_MaintenaceParam_Ext, sizeof(MAINTENACE_PARAM_EXT));

			//if(0 == strlen(gl_MaintenaceParam.szLastSalesPolling_Time))
			//	GetCurTime(gl_MaintenaceParam.szLastSalesPolling_Time);
			//memcpy(&gl_MaintenaceParam.szLastSalesPolling_Time[6], gl_MaintenaceParam.szTimeSalesPolling, 4);
			if(gl_MaintenaceParam.ucVersion < FILEVER)
			{
				PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA), 
					(void *)&sav_old2, sizeof(sav_old2));
				// Added by Kim_LinHB 2013-11-21 v4.73
				gl_SavParam.usTraceNo = sav_old2.usTraceNo;
				gl_SavParam.usBatchNo = sav_old2.usBatchNo;
				gl_SavParam.bIsSetup = sav_old2.bIsSetup;
				gl_SavParam.bDownloadLogoToDo = sav_old2.bDownloadLogoToDo;

				memcpy(gl_SavParam.ucKey, sav_old2.ucKey, 16);
				memcpy(&gl_SavParam.stClaim, &sav_old2.stClaim,sizeof(sav_old2.stClaim));

				gl_SavParam.uiProductsListIndex = sav_old2.uiProductsListIndex;

				gl_SavParam.ucCategoryNum = sav_old2.ucCategoryNum;
				gl_SavParam.ucProductsNum = sav_old2.ucProductsNum;

				for(i = 0; i < sav_old2.ucCategoryNum; ++i)
				{
					gl_SavParam.stSavParamCategory[i].uiCodeCategory = sav_old2.stSavParamCategory[i].ucCodeCategory;
					strcpy(gl_SavParam.stSavParamCategory[i].szName, sav_old2.stSavParamCategory[i].szName);
					strcpy(gl_SavParam.stSavParamCategory[i].szName_AR, sav_old2.stSavParamCategory[i].szName_AR);
					gl_SavParam.stSavParamCategory[i].ucCategoryType = sav_old2.stSavParamCategory[i].ucCategoryType;
					gl_SavParam.stSavParamCategory[i].ucNumLogo = sav_old2.stSavParamCategory[i].ucNumLogo;
				}

				for(i = 0; i < sav_old2.ucProductsNum; ++i)
				{
					gl_SavParam.stSavParamProducts[i].uiCodeCategory = sav_old2.stSavParamProducts[i].ucCodeCategory;
					gl_SavParam.stSavParamProducts[i].ucCategory_Index = sav_old2.stSavParamProducts[i].ucCategory_Index;
					gl_SavParam.stSavParamProducts[i].ucType = sav_old2.stSavParamProducts[i].ucType;
					strcpy(gl_SavParam.stSavParamProducts[i].szCodeProduct, sav_old2.stSavParamProducts[i].szCodeProduct);
					strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct, sav_old2.stSavParamProducts[i].szNameProduct);
					strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct_AR, sav_old2.stSavParamProducts[i].szNameProduct_AR);
					strcpy(gl_SavParam.stSavParamProducts[i].szAmount, sav_old2.stSavParamProducts[i].szAmount);
					strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, sav_old2.stSavParamProducts[i].szDiscount);
					gl_SavParam.stSavParamProducts[i].usNbMaxRestock = sav_old2.stSavParamProducts[i].usNbMaxRestock;
				}
				// Add End
				// Added by Kim_LinHB 2013-9-3 v4.67 for setting download after remote download
				memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
				WriteParam(WRT_ALL);
				// Add End
			}
			else
			{
				if(strcmp(gl_MaintenaceParam_Ext.version, "4.73") < 0)
				{
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA), 
						(void *)&sav_old2, sizeof(sav_old2));
					// Added by Kim_LinHB 2013-11-21 v4.73
					gl_SavParam.usTraceNo = sav_old2.usTraceNo;
					gl_SavParam.usBatchNo = sav_old2.usBatchNo;
					gl_SavParam.bIsSetup = sav_old2.bIsSetup;
					gl_SavParam.bDownloadLogoToDo = sav_old2.bDownloadLogoToDo;

					memcpy(gl_SavParam.ucKey, sav_old2.ucKey, 16);
					memcpy(&gl_SavParam.stClaim, &sav_old2.stClaim,sizeof(sav_old2.stClaim));

					gl_SavParam.uiProductsListIndex = sav_old2.uiProductsListIndex;

					gl_SavParam.ucCategoryNum = sav_old2.ucCategoryNum;
					gl_SavParam.ucProductsNum = sav_old2.ucProductsNum;

					for(i = 0; i < sav_old2.ucCategoryNum; ++i)
					{
						gl_SavParam.stSavParamCategory[i].uiCodeCategory = sav_old2.stSavParamCategory[i].ucCodeCategory;
						strcpy(gl_SavParam.stSavParamCategory[i].szName, sav_old2.stSavParamCategory[i].szName);
						strcpy(gl_SavParam.stSavParamCategory[i].szName_AR, sav_old2.stSavParamCategory[i].szName_AR);
						gl_SavParam.stSavParamCategory[i].ucCategoryType = sav_old2.stSavParamCategory[i].ucCategoryType;
						gl_SavParam.stSavParamCategory[i].ucNumLogo = sav_old2.stSavParamCategory[i].ucNumLogo;
					}

					for(i = 0; i < sav_old2.ucProductsNum; ++i)
					{
						gl_SavParam.stSavParamProducts[i].uiCodeCategory = sav_old2.stSavParamProducts[i].ucCodeCategory;
						gl_SavParam.stSavParamProducts[i].ucCategory_Index = sav_old2.stSavParamProducts[i].ucCategory_Index;
						gl_SavParam.stSavParamProducts[i].ucType = sav_old2.stSavParamProducts[i].ucType;
						strcpy(gl_SavParam.stSavParamProducts[i].szCodeProduct, sav_old2.stSavParamProducts[i].szCodeProduct);
						strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct, sav_old2.stSavParamProducts[i].szNameProduct);
						strcpy(gl_SavParam.stSavParamProducts[i].szNameProduct_AR, sav_old2.stSavParamProducts[i].szNameProduct_AR);
						strcpy(gl_SavParam.stSavParamProducts[i].szAmount, sav_old2.stSavParamProducts[i].szAmount);
						strcpy(gl_SavParam.stSavParamProducts[i].szDiscount, sav_old2.stSavParamProducts[i].szDiscount);
						gl_SavParam.stSavParamProducts[i].usNbMaxRestock = sav_old2.stSavParamProducts[i].usNbMaxRestock;
					}
					// Add End
					memset(gl_MaintenaceParam.szLastSettDownLoad_Time, 0, SIZE_DATE_TIME + 1);
					WriteParam(WRT_ALL);
				}
				else
				{
					PubFileRead(FILE_PARAM, 1 + sizeof(MAINTENACE_PARAM) + sizeof(TRANS_DATA), 
						(void *)&gl_SavParam, sizeof(SAV_PARAM));
				}
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
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
		return 0xFF;
	}
	fd_tmp = open((char *)dstFile,O_CREATE | O_RDWR);
	if(fd < 0)
	{
		close(fd);
		ScrCls();
		PubDispString("File Error", 3 | DISP_LINE_CENTER);
		PubDispString("Please reboot", 5 | DISP_LINE_CENTER);
		while(1);
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

enCodeReturn WriteUsers(const unsigned char *Name, const unsigned char *Pwd)
{
	USER_INFO tmp;
	unsigned char bWrited = FALSE;

	int fd = open(FILE_USERS, O_CREATE | O_RDWR);
	long Len = filesize(FILE_USERS);

	if(fd < 0 || !Name)
	{
		close(fd);
		return 0xFF;
	}

	memset(&tmp, 0, sizeof(USER_INFO));
	seek(fd, 0, SEEK_SET);
	while(Len > 0)
	{
		read(fd, (void *)&tmp, sizeof(USER_INFO));
		if(0 == strcmp(tmp.szName, Name))
		{
			if(Pwd)
				strcpy(tmp.szPwd, Pwd);
			seek(fd, 0 - sizeof(USER_INFO), SEEK_CUR);
			write(fd, (void *)&tmp, sizeof(USER_INFO));
			bWrited = TRUE;
			break;
		}

		Len -=sizeof(USER_INFO);
	}
	//file end

	if(FALSE == bWrited)
	{
		memset(&tmp, 0, sizeof(USER_INFO));
		strcpy(tmp.szName, Name);
		strcpy(tmp.szPwd, Pwd);//Pwd not null
		write(fd, (void *)&tmp, sizeof(USER_INFO));
	}

	close(fd);
	return OK;
}
enCodeReturn ReadUsersPwd(const unsigned char *Name, unsigned char *Pwd)
{
	USER_INFO tmp;
	unsigned char bRead = FALSE;

	int fd = open(FILE_USERS, O_RDWR);
	long  Len = filesize(FILE_USERS);

	if(fd < 0 || !Name)
	{
		close(fd);
		return 0xFF;
	}

	memset(&tmp, 0, sizeof(USER_INFO));
	seek(fd, 0, SEEK_SET);
	while(Len > 0)
	{
		read(fd, (void *)&tmp, sizeof(USER_INFO));
		if(0 == strcmp(tmp.szName, Name))
		{
			if(Pwd)
				strcpy(Pwd, tmp.szPwd);
			bRead = TRUE;
			break;
		}
		Len -= sizeof(USER_INFO);
	}
	//file end

	close(fd);

	if(FALSE == bRead)
		return 0xFF;
	return OK;
}
enCodeReturn DelUsers(const unsigned char *Name)
{
	long  Len;
	USER_INFO tmp;
	int fd_dst, fd_src;
	if(OK != ReadUsersPwd(Name, tmp.szPwd))
		return 0xFF;

	FileCpy(FILE_USERS_TMP, FILE_USERS);
	remove(FILE_USERS);

	fd_dst = open(FILE_USERS, O_CREATE | O_RDWR);
	Len = filesize(FILE_USERS_TMP);
	fd_src = open(FILE_USERS_TMP, O_RDWR);

	if(fd_dst < 0 || fd_src < 0 || !Name)
	{
		close(fd_dst);
		close(fd_src);
		return 0xFF;
	}

	seek(fd_dst, 0, SEEK_SET);
	seek(fd_src, 0, SEEK_SET);
	while(Len > 0)
	{
		read(fd_src, (void *)&tmp, sizeof(USER_INFO));
		if(0 != strcmp(tmp.szName, Name))
		{
			write(fd_dst, (void *)&tmp, sizeof(USER_INFO));
		}
		Len -= sizeof(USER_INFO);
	}
	close(fd_dst);
	close(fd_src);
	remove(FILE_USERS_TMP);
	return OK;
}

enCodeReturn GetUsersList(USER_INFO *Users, unsigned int *iNums)
{
	long  i;
	USER_INFO tmp;
	
	int fd = open(FILE_USERS, O_RDWR);
	long  Len = filesize(FILE_USERS);

	if(fd < 0 || !Users || !iNums)
	{
		close(fd);
		return 0xFF;
	}

	*iNums = 0;
	memset(&tmp, 0, sizeof(USER_INFO));
	seek(fd, 0, SEEK_SET);
	i = 0;
	while(Len > 0)
	{
		read(fd, (void *)&tmp, sizeof(USER_INFO));
		strcpy(Users[i].szName, tmp.szName);
		++i;
		if(i >= 15)
			break;
		Len -= sizeof(USER_INFO);
	}
	//file end
	*iNums = i;

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

// Added by Kim_LinHB 2014-5-12
int WriteReversal(const unsigned char *pszProvisionId, unsigned char ucMessageType, const unsigned char *pszUser)
{
	ReversalInfo tmp;

	int fd = open(FILE_REVERSAL, O_CREATE | O_RDWR);
	if(fd < 0)
	{
		close(fd);
		return 0xFF;
	}

	memset(&tmp, 0, sizeof(ReversalInfo));
	seek(fd, 0, SEEK_SET);
	strcpy(tmp.szProvisionId, pszProvisionId);
	tmp.ucMessageType = ucMessageType;
	strcpy(tmp.szUser, pszUser);
	write(fd, (void *)&tmp, sizeof(ReversalInfo));
	close(fd);
	return OK;
}

int ReadReversal(ReversalInfo *pstData)
{
	ReversalInfo tmp;
	long len = filesize(FILE_REVERSAL);
	int fd;
	if(len > sizeof(ReversalInfo))
	{
		DelReversal(); // Added by Kim_LinHB 2015-01-20 v5.06
		return 0xFF;
	}

	fd = open(FILE_REVERSAL, O_RDWR);
	if(fd < 0)
	{
		close(fd);
		return 0xFF;
	}

	memset(&tmp, 0, sizeof(ReversalInfo));
	seek(fd, 0, SEEK_SET);
	if(sizeof(ReversalInfo) != read(fd, (void *)&tmp, sizeof(ReversalInfo)))
	{
		close(fd);
		return 0xFF;
	}
	close(fd);
	memcpy(pstData, &tmp, sizeof(ReversalInfo));
	// Added by Kim_LinHB 2015-01-20 v5.06
	if (atol(pstData->szProvisionId) > 0 &&
		strlen(pstData->szUser) > 0 &&
		(pstData->ucMessageType == ID_SaleCodes || 
		pstData->ucMessageType == ID_CashOrder))
	{
		return OK;
	}
	
	DelReversal(); // Added by Kim_LinHB 2015-01-20 v5.06
	return 0xFF;
}

int DelReversal()
{
	return remove(FILE_REVERSAL);
}

//////////////////////////////////////////////////////////////////////////
//					the following is static func
//////////////////////////////////////////////////////////////////////////

static void DataInitDef()
{
	memset(&gl_MaintenaceParam, 0, sizeof(MAINTENACE_PARAM));
	memset(&gl_MaintenaceParam_Ext, 0, sizeof(MAINTENACE_PARAM_EXT)); // Added by Kim_LinHB 2013-6-19 v4.61
	strcpy(gl_MaintenaceParam.szLogLv ,"0");
	strcpy(gl_MaintenaceParam.szMaintenancePwd, "2049");
	strcpy(gl_MaintenaceParam.szNetworkPwd, "8844");
	strcpy(gl_MaintenaceParam.szPrintertype, "1");
	// Removed by Kim_LinHB 2013-11-14 v4.73
	//strcpy(gl_MaintenaceParam.szTimeSalesPolling, "1800");
	strcpy(gl_MaintenaceParam.szTimeSettDownload, "0000");
	strcpy(gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, "0000");
	strcpy(gl_MaintenaceParam.stManage.szName, "manager");
	strcpy(gl_MaintenaceParam.stManage.szPwd, "12345");
	gl_MaintenaceParam.usMaxRequestVoucherCount = NB_MAX_CODE_STOCK;
	gl_MaintenaceParam.usMaxNotifyVoucherCount = NB_MAX_CODE_STOCK;

	gl_MaintenaceParam.bReportEmpty = TRUE;
	gl_MaintenaceParam.ucVersion = FILEVER;
	
	gl_MaintenaceParam.ucLang = LANG_EN;
	
	gl_MaintenaceParam.bFullAuto = FALSE;
	gl_MaintenaceParam.bManagerSett = TRUE;
	gl_MaintenaceParam.bClaim = TRUE;
	gl_MaintenaceParam.bWelcomePage = TRUE;
	gl_MaintenaceParam.ucMerchantMode = OFFLINE;
	// Added by Kim_LinHB 2013-4-9 v4.57
	gl_MaintenaceParam.bAutoPrintReports = TRUE;
	gl_MaintenaceParam.bPrintReportDaily = TRUE;
	// Add End
	// Modified by Kim_LinHB 2013-11-14 v4.73
	//GetCurTime(gl_MaintenaceParam.szLastSalesPolling_Time);
	memset(gl_MaintenaceParam.szLastSalesPolling_Time, 0, sizeof(gl_MaintenaceParam.szLastSalesPolling_Time)); //mmss
	gl_MaintenaceParam.bPrintMerchantCopy = TRUE; // Added by Kim_LinHB 2013-3-26 v4.56
	
	strcpy(gl_MaintenaceParam.stConnParam.szNumPad, gl_pLangSelPiont[STR_DEFAUT_NUMERO_PAD_PRINC]);
	strcpy(gl_MaintenaceParam.stConnParam.szNumPadSec, gl_pLangSelPiont[STR_DEFAUT_NUMERO_PAD_SEC]);

	gl_MaintenaceParam.ucConnSel = 0;
	gl_MaintenaceParam.stConnParam.ucTeleUsing = 0;
	gl_MaintenaceParam.stConnParam.stParamEth.bDHCP = TRUE;

	strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, "DATASIMCARD.COM");
#ifndef LHB_TEST
	strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP,"193.169.190.115"); //Modified by Allen 2016/03/16
	strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, "6062");
	gl_MaintenaceParam.ucConnType[0] = gl_MaintenaceParam.ucConnType[1] = GPRS;//Modified by Allen 2016/03/16 from ETHERNET to GPRS
#else
	strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "193.169.190.118");
	//strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "95.0.181.83");
	strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, "cmnet");

	//strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "172.16.20.155");
	strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, "6062");
	gl_MaintenaceParam.ucConnType[0] = ETHERNET;
	//gl_MaintenaceParam.ucConnType[1] = ETHERNET;
	//gl_MaintenaceParam.stFTP.bForceToDownload = 1;
#endif

	gl_MaintenaceParam_Ext.cConnectionTimeout = TIME_OUT_CONN;

	{ // Added by Kim_LinHB 2013-3-25 v4.56
		unsigned char tmp[32];
		GetSerialNumber(tmp);
		strcpy(gl_MaintenaceParam.szTermID, tmp);
	} // Add End
	

	gl_MaintenaceParam.stConnParam.stParamGPRS.ucAuth = 0x01;//PAP

	strcpy(gl_MaintenaceParam.stFTP.stApp.szFileVer, STR_VERSION_SUB);

	memset(&gl_SavParam, 0, sizeof(SAV_PARAM));
	gl_SavParam.usBatchNo = 0;
	gl_SavParam.usTraceNo = 0;

	gl_SavParam.bIsSetup = TRUE;

	memset(&gl_LastTran, 0, sizeof(TRANS_DATA));

	// it's in the Ingevoucher Origin, Don't understand how it work, so it's here and useless
	// and the value is saved in gl_SavParam.ucKey
	GenerationdPOSKey(gl_TermSN, gl_SavParam.ucKey);
}
