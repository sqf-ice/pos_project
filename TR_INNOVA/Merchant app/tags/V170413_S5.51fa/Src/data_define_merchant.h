/****************************************************************************
NAME
    data_define_merchant.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.31     - created
****************************************************************************/
#ifndef _DATA_DEFINE_MERCHANT_H
#define _DATA_DEFINE_MERCHANT_H

#define FILE_PARAM		"PARAMMER.DAT"
#define FILE_STOCK_OLD1	"STOCKMER.DAT"
#define FILE_STOCK		"STOCK1.DAT"
#define FILE_USERS		"USERSMER.DAT"
#define FILE_USERS_TMP	"USERSMERT.DAT"
#define FILE_TRANS_OL	"TRANSTMP.DAT"
#define FTPFILE			"FTPFILEINFO"
#define FILE_BACKUP     "REMOTEBACKUPM" //dont use // Modified by Kim_LinHB 2013-7-6 v4.62

#define FILE_REVERSAL	"REVERSAL"	// Added by Kim_LinHB 2014-5-12

// Added by Kim_LinHB 2013-5-22 v4.59
#define FILE_CASHORDER  "CASHORDER2.DAT" // Modified by Kim_LinHB 2013-11-21 v4.73 from CASHORDER
// Add End
#define FILE_UPDATE		"APP.BIN"

// Added by Kim_LinHB 2013-9-12 v4.68
#define FILEPRODUCT_TEMP	"PRODUCTLIST"

#define SIZE_NUM_SERIE_TPE	8

#define SIZE_CASHIER_NAME	20
#define SIZE_MAX_PWD	8
#define SIZE_MIN_PWD	4

#define SIZE_BATCH_NO		10
#define SIZE_TIME			4
#define SIZE_DATE_TIME		12
#define SIZE_MAX_CODE_PRODUCT 4
#define SIZE_MAX_CODE_MANAGER 15

#define SIZE_MAX_VOUCHER_ID	6
#define SIZE_MAX_VOUCHER_SERIAL 16
#define SIZE_MAX_VOUCHER_NUM 50

#define SIZE_MAX_ID_MANAGER  4
#define SIZE_MAX_ID_TERMINAL 8
#define SIZE_MAX_LIBELLE_PRODUCT 50
#define SIZE_MAX_AMOUNT		15		//money
#define SIZE_MAX_GENCODE	13
#define SIZE_MAX_QUANTITY   5
#define SIZE_MAX_DISCOUNT	5
#define SIZE_MAX_SLIPMESSAGE 200    //added by Allen 2016-11-16
#define SIZE_NUMERO_SERIE_TERMINAL_PAX	8

#define SIZE_MAX_PROVISION_ID 20

#define NB_MAX_CODE_STOCK		744
#define NB_MAX_CAISSIERS		15
#define NB_MAX_CATEGORYS		50
#define NB_MAX_PRODUCTS         150 // Modified by Kim_LinHB 2013-4-2 v4.57 from 100 to 150

#define MAX_APN				20
#define MAX_USER			50 
#define MAX_PWD				20 
#define MAX_IP				15
#define MAX_PORT			5
#define MAX_PIN				20

#define SIZE_MAX_NUM_PAD 16
#define SIZE_MAX_TYPE_IMPRIMANTE 1

#ifndef MAX_TCP_BUFFER
#define MAX_TCP_BUFFER	(2048 * 10)
#endif

#define OK 0
typedef enum enCodeReturn //返回码
{
	ENTETE_MESSAGE_RECU_NOK = 6,	
	TYPE_MESSAGE_RECU_NOK,	
	PB_SIZE_PARAMETRE,	
	PB_AMOUNT_RECHARGE,	
	NoReponse,
	TimeOutInterCaracteres,	
	IncidentReception,	
	PbSendPortSerie,	
	PBInitPortSerie,	
	PbConnection = 15,	
	PbResTpac,	
	IpduIncorrect,	
	SizeRecueIncorrecte,	
	ERREUR_ACQUIT,	
	ERREUR_LRC,	
	ERREUR_STX,	
	ERREUR_ETX,	
	ENQ_NON_RECU,	
	IncidentOS,	
	PlusDeCodeDispo =25,	
	TicketNonPresent,	
	PbSizeBufferTicket,	
	PBReceptionCode,	
	PlaceInsufficient,	
	ProductInexistant,	
	CategoryInexistant,	
	StockServerEpuise,	//服务器端 库存耗尽
	ErreurAuthentification,	//认证失败
	TimeOutLecturePortSerie,
	ErreurRepIdentification =35,	
	PasDeTransaction,
	TypeTrameInconnue,
	TrameNonValable,
	TrameNonValide,
	PBLogiciel,
	NoByteAvailable,
	NbMaxEssaisAtteint,
	TrameInattendue,
	TrameLTEmiseRejettee,
	TrameHorsSequence = 45,
	Timer403Echu,
	TimerEchu,
	TrameIncomprehensible,
	SizeMaxAtteinte,
	SizeMaxDepassee,
	IncoherenceDistant,
	IncidentLecturePortSerie,
	SettingsDownloadAFaire,
	DuplicataNonDisponible,
	ServiceArrete=55,			// 55
	PbRecuperationTicket,
	TerminalNonAutorise,
	CancelSelectOp,
	CancelSelectProd,
	PbDemandeVersion,
	CreditEpuise,
	PbOrdreAttendu,
	PbReceptionLogo,
	CancelEnvoiSerie,
	PbEnregistrementSerialNumber=65,//65
	PbSalesPolling,
	PbPlaceMem,//存储空间不够
	PbIdentMessage,
	InitMerchantNonDisponible,
	ReclamationNonDisponible,
	CancelUtilisateur,
	PbNumeroTelephone,
	PbCarteMag,
	// Add yju gestion gprs externe
	GPRSWrongPin,
	GPRSNoModemResponse=75,	// 75
	GPRSConnectionServerKO,
	InputParamKO,
	// Fin Add Yju
	PbIndexDebut,
	PbLgRecup,
	PbSeparateur,
	PbInputAmount,
	PbConfirmationTopUP,
	//v4.7
	TCPIPPortOpenERR,
	TCPIPSetLocalIPERR,
	TCPIPSetGateWayIPERR=85,   // 85
	TCPIPSetMaskERR,
	TCPIPSetRemoteIPERR,
	TCPIPSetRemotePortERR,
	TCPIPConnectERR,
	TCPIPStartDHCPERR,  //v4.12

	GPRSInitERR,
	GPRSDialERR,
	GPRSConnectERR,

	PPPDialERR,
	DIALLineERR=95,      //95
	DIALLineBusy,  
	DIALNoTone,
	PPPLoginERR,
	PPPOpenERR,

	CancelSelectRaccourci,		//v4.8
	PlusDePlaceEquipe,
	InputAnnulee,					//v4.31

	// Add yju ECC code retours
	ECC_AuthentificationSalesmanKO = 105,
	ECC_ProductCommandeInconnu,
	ECC_StockServerInsuffisant,
	ECC_CreditsInsuffisants,
	ECC_PB_Logiciel,
	ECC_TerminalInexistant,
	TypeMessageRecuIncorrect,
	PBReceptionQuantite,
	EchecScan,
	DiscountDifferent,

	REPSONSENOZERO = 151,
	COMPRESSERROR,
	UMCOMPRESSERROR,

	FTPSeverERR=200,
	FTPERROR,
	FTPPPPDialERR,
	FTPDIALLineERR,
	FTPDIALLineBusy,   
	FTPDIALNoTone,
	FTPPPPLoginERR,
	FTPPPPOpenERR,
	FTPPPPReadERR,
	FTPPPPWriteERR,
	FTPPPPCloseERR,//210

	FTPWNETSimERR,
	FTPWNETPinERR,
	FTPWNETUserPassERR,
	FTPWNETDialERR,
	FTPWNETConnectERR,
	FTPWNETTCPConnectERR,
	FTPWNETTxdERR,
	FTPWNETRxdERR,
	FTPWNETCloseERR,

	FTPTCPInitERR,//220
	FTPTCPConnectERR,
	FTPTCPSendERR,
	FTPTCPRecvERR,

	FTPRecvDataLenERR,
	FTPRecvDataTimeout,
	FTPSendDataLenERR,
	FTPSendDataTimeout,

	FTPFileOpenERR,
	FTPFileNoExist,
	FTPFileMemOverflow,//230
	FTPFileGetSizeERR,
	FTPFileSeekERR,
	FTPFileReadERR,
	FTPFileReadLenERR,
	FTPFileWriteERR,
	NoRecentReleaseFound	//4.32, 236
}enCodeReturn;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef struct _tagConfigEthernet{
	unsigned char	bDHCP;
	unsigned char	szLocalIP[MAX_IP+1];
	unsigned char	szNetMask[MAX_IP + 1];
	unsigned char	szGatewayIP[MAX_IP + 1];
	unsigned char	szDNS[MAX_IP+1];
	unsigned char	szDNS_Bak[MAX_IP+1];
}ETH_PARAM;

typedef struct _tagWIRELESS_PARAM
{
	unsigned char szAPN[MAX_APN+1];  //Dial Num CDMA: #777; GPRS: cmnet
	unsigned char szUID[MAX_USER+1];
	unsigned char szPwd[MAX_PWD+1];
	unsigned char szSimPin[MAX_PIN+1];     // SIM card PIN
	unsigned char szOperatorName[MAX_USER+1];
	unsigned char ucAuth;
}WIRELESS_PARAM;

typedef struct _tagFTPFILE_INFO
{
	unsigned char szFileVer[10];
	unsigned char szFileName[100];
	long lTotalLen;
	long lCurrLen;
}FTPFILE_INFO;

typedef struct _tagFTP{
	unsigned char szRemoteIP[MAX_IP+1];
	unsigned char szRemotePort[MAX_PORT+1];
	unsigned char szUser[MAX_USER+1];
	unsigned char szPwd[MAX_PWD+1];

	unsigned char szFilePath[255];
	FTPFILE_INFO stApp;
	FTPFILE_INFO stParam[1]; //tmp quantity: 1 AR_PRNDISP.FNT

	unsigned char szTimeSoftDownLoad[5 + 1];
	unsigned char bForceToDownload;
}FTP_PARAM;

typedef struct _tagInfoMaintenance
{
	unsigned char ucTeleUsing;
	unsigned char szNumPad[SIZE_MAX_NUM_PAD+1];
	unsigned char szNumPadSec[SIZE_MAX_NUM_PAD+1];
	unsigned char ucIPUsing;
	unsigned char szRemoteIP[MAX_IP+1];
	unsigned char szRemotePort[MAX_PORT+1];
	unsigned char szRemoteIP_Bak[MAX_IP+1];
	unsigned char szRemotePort_Bak[MAX_PORT+1];
	WIRELESS_PARAM	stParamGPRS;
	ETH_PARAM		stParamEth;
}CONN_PARAM;

typedef struct _tagCashierInfo
{
	unsigned char szName[SIZE_CASHIER_NAME + 1];
	unsigned char szPwd[SIZE_MAX_PWD + 1];
}USER_INFO;

// Added by Kim_LinHB 2013-5-21 v4.59
enum{
	ONLINE = 0,
	OFFLINE = 1,
	MIX = 2,
};

typedef struct _tagMaintenaceParametres
{
	unsigned char ucVersion;

	USER_INFO stManage;

// 	unsigned char did_settDownLoad;  // Removed by Kim_LinHB 2013-3-20 v4.56 and all positions in src
// 	unsigned char did_SalesPolling;
	unsigned char szLastSettDownLoad_Time[SIZE_DATE_TIME + 1];
	unsigned char szLastSalesPolling_Time[SIZE_DATE_TIME + 1];

	unsigned char szTermID[SIZE_MAX_CODE_MANAGER + 1]; // Modified by Kim_LinHB 2013-3-25 v4.56 size 8 + 1 to 15 + 1
	unsigned char szMaintenancePwd[20 + 1];
	unsigned char szNetworkPwd[20 + 1];
	unsigned char szDealerCode[10 + 1];
	unsigned short usMaxRequestVoucherCount;
	unsigned short usMaxNotifyVoucherCount;
	unsigned char szNotifySoldVouchersTime[SIZE_TIME + 1];   // not used
	unsigned char ucLastReceivedMailId;

	unsigned char bPrintCommande;
	unsigned char bPrintDetailRestock;
	unsigned char bAutoPrintReports;
	unsigned char bPrintReportDaily;
	unsigned char bPrintReportSale;
	unsigned char bDoublePaper;

	unsigned char bPrintMerchantCopy; // Added by Kim_LinHB 2013-3-26 v4.56

	unsigned char ucLang;
	unsigned char bFullAuto;
	unsigned char bManagerSett;
	unsigned char bClaim;
	//unsigned char bOffline;  // Removed by Kim_LinHB 2013-5-21 from MIX mode v4.59
	unsigned char ucMerchantMode; // Added by Kim_LinHB 2013-5-21 from MIX mode v4.59
	unsigned char bWelcomePage;

	unsigned char bPrintBarCode;
	unsigned char bPrintLogo;
	unsigned char bCancelSale;
	unsigned char bScanSerialNumber;
	unsigned char bInputSerial;
	
	unsigned char szPrintertype[1 + 1];
	unsigned char szLogLv[1 + 1];
	unsigned char ucConnType[2];
	unsigned char ucConnSel;

	unsigned char szTimeSalesPolling[4 + 1];	//not used
	unsigned char szTimeSettDownload[4 + 1];

	unsigned char bReportEmpty;
	unsigned char szLastProvisionID[SIZE_MAX_PROVISION_ID+1];

//	unsigned char did_SoftDownLoad; // Removed by Kim_LinHB 2013-3-20 v4.56 and all positions in src
	unsigned char szLastSoftDownLoad_Time[SIZE_DATE_TIME + 1];
	FTP_PARAM		stFTP;

	CONN_PARAM stConnParam;
}MAINTENACE_PARAM;

typedef struct _tagMaintenaceParametres_EXT
{	// total 1024
	unsigned char szHeader1[100];
	unsigned char szHeader2[100];
	unsigned char szHeader3[100];
	unsigned char version[10]; // Added by Kim_LinHB 2013-9-3 v4.67
	unsigned char cConnectionTimeout; // Added by Kim_LinHB 2013-11-11 v4.72
	unsigned char szDuration[SIZE_TIME + 1];  // Added by Kim_LinHB 2013-11-14 v4.73 minute
	unsigned char sReserved[708];
}MAINTENACE_PARAM_EXT;  // Added by Kim_LinHB 2013-6-19 v4.61

typedef struct _tagxParamCategory_OLD
{
	unsigned char ucCodeCategory;			// Code Category
	unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];	// Name
	unsigned char szName_AR[SIZE_MAX_LIBELLE_PRODUCT+1];
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucCategoryType;
	unsigned char ucNumLogo;
}xParamCategory_OLD;

// Modified by Kim_LinHB 2013-11-21 v4.73
typedef struct _tagxParamCategory
{
	unsigned int uiCodeCategory;			// Code Category
	unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];	// Name
	unsigned char szName_AR[SIZE_MAX_LIBELLE_PRODUCT+1];
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucCategoryType;
	unsigned char ucNumLogo;
}xParamCategory;

typedef struct _tagxParamProducts_OLD
{
	unsigned char ucCodeCategory;			// Code Category  need to increase to short 4.63
	unsigned char ucCategory_Index;			// for print // Added by Kim_LinHB 2013-3-22 v4.56 need to increase to short 4.63
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucType;			//temp
	unsigned char szCodeProduct[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szNameProduct[SIZE_MAX_LIBELLE_PRODUCT + 1]; // Name du product
	unsigned char szNameProduct_AR[SIZE_MAX_LIBELLE_PRODUCT+1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];	// Amount
	unsigned char szDiscount[SIZE_MAX_DISCOUNT + 1];	// Added by Kim_LinHB 2013-5-21 v4.59
 	unsigned short usNbMaxRestock; // not used // 补货门槛值 Number de recharge max souhaite 下面这3个都是在 Manager settings -> max pin qty中进行设置的, 低于了这个usNbMaxRestock就要进行recharement
	// Removed by Kim_LinHB 2013-3-22 v4.56
// 	unsigned char ucSeuilPreAlert;	// Seuil de prealerte   ou definition TOP UP ou definition PRODUCT PROMO  预警的门槛值 或者 
// 	unsigned char ucSeuilRestock;
	// Remove End
}xParamProducts_OLD;

// Modified by Kim_LinHB 2013-11-21 v4.73
typedef struct _tagxParamProducts_OLD2
{
	unsigned int uiCodeCategory;			// Code Category  need to increase to short 4.63
	unsigned char ucCategory_Index;			// for print // Added by Kim_LinHB 2013-3-22 v4.56 need to increase to short 4.63
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucType;			//temp
	unsigned char szCodeProduct[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szNameProduct[SIZE_MAX_LIBELLE_PRODUCT + 1]; // Name du product
	unsigned char szNameProduct_AR[SIZE_MAX_LIBELLE_PRODUCT+1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];	// Amount
	unsigned char szDiscount[SIZE_MAX_DISCOUNT + 1];	// Added by Kim_LinHB 2013-5-21 v4.59
 	unsigned short usNbMaxRestock; // not used // 补货门槛值 Number de recharge max souhaite 下面这3个都是在 Manager settings -> max pin qty中进行设置的, 低于了这个usNbMaxRestock就要进行recharement
	// Removed by Kim_LinHB 2013-3-22 v4.56
// 	unsigned char ucSeuilPreAlert;	// Seuil de prealerte   ou definition TOP UP ou definition PRODUCT PROMO  预警的门槛值 或者 
// 	unsigned char ucSeuilRestock;
	// Remove End
}xParamProducts_OLD2;

// Modified by Allen 2016-11-16 v5.45
typedef struct _tagxParamProducts
{
	unsigned int uiCodeCategory;			// Code Category  need to increase to short 4.63
	unsigned char ucCategory_Index;			// for print // Added by Kim_LinHB 2013-3-22 v4.56 need to increase to short 4.63
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucType;			//temp
	unsigned char szCodeProduct[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szNameProduct[SIZE_MAX_LIBELLE_PRODUCT + 1]; // Name du product
	unsigned char szNameProduct_AR[SIZE_MAX_LIBELLE_PRODUCT+1];
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];	// Amount
	unsigned char szDiscount[SIZE_MAX_DISCOUNT + 1];	// Added by Kim_LinHB 2013-5-21 v4.59
 	unsigned short usNbMaxRestock; // not used // 补货门槛值 Number de recharge max souhaite 下面这3个都是在 Manager settings -> max pin qty中进行设置的, 低于了这个usNbMaxRestock就要进行recharement
	//added by Allen 2016-11-16
	unsigned char szSlipMessage[SIZE_MAX_SLIPMESSAGE + 1];
	unsigned char szSlipMessage_AR[SIZE_MAX_SLIPMESSAGE + 1];
	//add end
}xParamProducts;

typedef struct _tagxClaim
{
	unsigned short usNbClaim;
	unsigned long ulClaimAmount;
}xClaim;

typedef struct _tagSavParametres_OLD
{
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned char bIsSetup;
	unsigned char bDownloadLogoToDo;

	unsigned char ucKey[16];
	xClaim		  stClaim;

	unsigned int uiProductsListIndex; // Added by Kim_LinHB 2013-4-2 v4.57
	unsigned char ucCategoryNum; //need to increase to short 4.63
	unsigned char ucProductsNum; //need to increase to short 4.63
	xParamCategory_OLD stSavParamCategory[NB_MAX_CATEGORYS];
	xParamProducts_OLD stSavParamProducts[NB_MAX_PRODUCTS];	
}SAV_PARAM_OLD;

// Modified by Kim_LinHB 2013-11-21 v4.73
typedef struct _tagSavParametres_OLD2
{
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned char bIsSetup;
	unsigned char bDownloadLogoToDo;

	unsigned char ucKey[16];
	xClaim		  stClaim;

	unsigned int uiProductsListIndex; // Added by Kim_LinHB 2013-4-2 v4.57
	unsigned char ucCategoryNum; //need to increase to short 4.63
	unsigned char ucProductsNum; //need to increase to short 4.63
	xParamCategory stSavParamCategory[NB_MAX_CATEGORYS];
	xParamProducts_OLD2 stSavParamProducts[NB_MAX_PRODUCTS];	
}SAV_PARAM_OLD2;

//modified by Allen 2016-11-16  v5.45
typedef struct _tagSavParametres
{
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned char bIsSetup;
	unsigned char bDownloadLogoToDo;

	unsigned char ucKey[16];
	xClaim		  stClaim;

	unsigned int uiProductsListIndex; // Added by Kim_LinHB 2013-4-2 v4.57
	unsigned char ucCategoryNum; //need to increase to short 4.63
	unsigned char ucProductsNum; //need to increase to short 4.63
	xParamCategory stSavParamCategory[NB_MAX_CATEGORYS];
	xParamProducts stSavParamProducts[NB_MAX_PRODUCTS];	
}SAV_PARAM;

enum
{
	CODE_RESTOCK_EMPTY = 0,
	CODE_RESTOCK_DOWNLOADING,
	CODE_RESTOCK_FREE,
	CODE_RESTOCK_SELLING,
	CODE_RESTOCK_SOLD = 4,
	CODE_RESTOCK_SALESPOLLING = 5, // Added by Kim_LinHB 2013-7-5 v4.61
	CODE_RESTOCK_BURNED = 6,
	CODE_RESTOCK_EXPIRED = 8,  //
	//CODE_RESTOCK_NON_DECRYPTABLE,//
	CODE_FIN_DAILY = 10,
	// 0XFF		ALL
};

typedef struct _tagStockInfo
{
	unsigned char ucProductState;
	unsigned char ucProduct_Index; // Added by Kim_LinHB 2013/3/22 v4.56 for offline  // not used
	unsigned char szCashierId[SIZE_CASHIER_NAME + 1];	//	Numero du cashier
	unsigned char szTraceNo[SIZE_BATCH_NO + 1];		// TraceNo de transaction   //not used
	unsigned char szSoldDate[SIZE_DATE_TIME + 1];	// DDMMYYYYhhmm en BCD
}STOCK_INFO;

typedef struct _tagVoucherInfo_OLD
{
	//	unsigned char ucIndex;		// Removed by Kim_LinHB 2013-4-2 v4.57
	//	unsigned char ucCodeCategory;	// Code Category // Removed by Kim_LinHB 2013-3-22 v4.56
	unsigned char szSlipMsg[50];
	unsigned char szCurrDeposit[SIZE_MAX_AMOUNT + 1]; // Modified by Kim_LinHB 2013-4-10 v4.57
	unsigned char szProductId[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szVoucherId[SIZE_MAX_VOUCHER_ID + 1];
	unsigned char szSerial[SIZE_MAX_VOUCHER_SERIAL + 1];
	unsigned char szNum[SIZE_MAX_VOUCHER_NUM + 1];
	unsigned char szExpirDate[SIZE_DATE_TIME + 1];
}VOUCHER_INFO_OLD;

typedef struct _tagVoucherInfo
{
//	unsigned char ucIndex;		// Removed by Kim_LinHB 2013-4-2 v4.57
//	unsigned char ucCodeCategory;	// Code Category // Removed by Kim_LinHB 2013-3-22 v4.56
	unsigned char szSlipMsg[50];
	unsigned char szCurrDeposit[SIZE_MAX_AMOUNT + 1]; // Modified by Kim_LinHB 2013-4-10 v4.57
	unsigned char szProductId[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szVoucherId[100 + 1]; // Modified by Kim_LinHB 2013-7-2 from 6 to 100 v4.61
	unsigned char szSerial[50 + 1]; // Modified by Kim_LinHB 2013-7-3 from 16 to 50 v4.61
	unsigned char szNum[SIZE_MAX_VOUCHER_NUM + 1];
	unsigned char szExpirDate[SIZE_DATE_TIME + 1];
}VOUCHER_INFO;


typedef struct _tagStock
{
	STOCK_INFO		stStock[NB_MAX_CODE_STOCK];
	VOUCHER_INFO	stVoucher[NB_MAX_CODE_STOCK];
}STOCK;

// Added by Kim_LinHB 2013-12-4 v4.76
typedef struct _tagStock_New
{
	unsigned int num;
	unsigned char szExpireDate[SIZE_DATE_TIME + 1];
	unsigned char szOrderDate[SIZE_DATE_TIME + 1];
	unsigned char szOrderId[10];
	unsigned char szProvisionID[SIZE_MAX_PROVISION_ID+1];
	VOUCHER_INFO	stVoucher[NB_MAX_CODE_STOCK];
}STOCK_NEW;

typedef struct _tagPendingOrd
{
	unsigned char szOrderId[10];
	unsigned char szDate[SIZE_DATE_TIME + 1];
	unsigned int num;
	struct _tag_PO_Product{
		unsigned char PId[SIZE_MAX_CODE_PRODUCT + 1];
		unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT + 1];
		int num;
	}stOrder[NB_MAX_CODE_STOCK];
}PENDING_ORD;


typedef struct _tagxTransData
{
	unsigned char ucTransType;
	unsigned char bOffline;
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned char szDateTime[SIZE_DATE_TIME + 1];

	unsigned char szAmount[SIZE_MAX_AMOUNT+1];
	unsigned char szProvisionID[SIZE_MAX_PROVISION_ID+1];
	unsigned char szCashier[SIZE_CASHIER_NAME + 1];

	// Added by Kim_LinHB 2013-3-22 v4.56
	unsigned char ucCategory_Index; //for print
	unsigned char ucProducts_Index; //for print
	unsigned int uiVoucherNum;
	// Add End
	VOUCHER_INFO  stVoucher;
}TRANS_DATA;

typedef struct _tagProductsInfo_OLD
{
	unsigned char ucCodeCategory;	// Code Category need to increase to short 4.63
	unsigned char szCodeProduct[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
}PRODUCT_INFO_OLD;

// Modified by Kim_LinHB 2013-11-21 v4.73
typedef struct _tagProductsInfo
{
	unsigned int uiCodeCategory;	// Code Category need to increase to short 4.63
	unsigned char szCodeProduct[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
}PRODUCT_INFO;

typedef struct _tagxTransDataCash_OLD
{
	unsigned char ucTransType;
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned char szDateTime[SIZE_DATE_TIME + 1];
	unsigned char ucIdentifyType;

	unsigned char szMerchantID[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szMerchantCode[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1]; // Merchant Terminal Id
	unsigned char szTermCode[SIZE_MAX_CODE_MANAGER + 1];

	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];
	unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
	unsigned char szProvisionID[SIZE_MAX_PROVISION_ID+1];

	unsigned char ucProductNum; //need to increase to short 4.63
	PRODUCT_INFO_OLD  stProducts[NB_MAX_PRODUCTS];
}TRANS_DATA_CASH_OLD;

// Modified by Kim_LinHB 2013-11-21 v4.73
typedef struct _tagxTransDataCash
{
	unsigned char ucTransType;
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned char szDateTime[SIZE_DATE_TIME + 1];
	unsigned char ucIdentifyType;

	unsigned char szMerchantID[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szMerchantCode[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1]; // Merchant Terminal Id
	unsigned char szTermCode[SIZE_MAX_CODE_MANAGER + 1];

	unsigned char szSalesman_PWD[SIZE_MAX_PWD + 1];
	unsigned char szAmountWithoutDiscount[SIZE_MAX_AMOUNT+1];
	unsigned char szAmountWithDiscount[SIZE_MAX_AMOUNT+1];
	unsigned char szProvisionID[SIZE_MAX_PROVISION_ID+1];

	unsigned char ucProductNum; //need to increase to short 4.63
	PRODUCT_INFO  stProducts[NB_MAX_PRODUCTS];
}TRANS_DATA_CASH;


typedef struct _tagMail
{
	unsigned char szModifiedOn[SIZE_DATE_TIME + 1];
	unsigned char szCaption[100 + 1];
	unsigned short usContentNums;
	unsigned char szContent[20][255 + 1];
}MAIL;

// Added by Kim_LinHB 2014-5-12
typedef struct _tagReversalInfo{
	unsigned char ucMessageType;
	unsigned char szProvisionId[SIZE_MAX_PROVISION_ID+1];
	unsigned char szUser[SIZE_CASHIER_NAME + 1];
}ReversalInfo;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

extern const unsigned char *cSTRConnType[];

extern unsigned char gl_bFirstRun;
extern unsigned char gl_bSettDown;
extern unsigned int  gl_TermMode;
extern unsigned char gl_TermName[8+1];
extern unsigned char gl_TermSN[32+1];

extern MAINTENACE_PARAM gl_MaintenaceParam;
extern MAINTENACE_PARAM_EXT gl_MaintenaceParam_Ext; // Added by Kim_LinHB 2013-6-19 v4.61
extern SAV_PARAM		gl_SavParam;
//extern STOCK			gl_Stock;    // Removed by Kim_LinHB 2013-8-12 v4.62
extern TRANS_DATA		gl_LastTran;
extern unsigned char	gl_ProvisionIdTmp[SIZE_MAX_PROVISION_ID+1];

extern USER_INFO	gl_Cur_Cashier;
extern unsigned char gl_bCashier_Login;

extern unsigned char gl_bOnService;

extern unsigned char gl_bPlanned_SettDownload;
extern unsigned char gl_bPlanned_SalesPolling;
extern unsigned char gl_bPlanned_SoftDownload;

enum
{
	SIMUL = 0, 
// 	PORTCOM,
// 	PORTMODEM,
	ETHERNET, 
	GPRS, 
//	PPP,
	ENDFLG,
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

enum{
	WRT_MAINTA = 0x01,
	WRT_SAVE = 0x02,
	WRT_TXN = 0x04,
	WRT_ALL = 0xFF,
};
enCodeReturn WriteParam(unsigned char type);
enCodeReturn WriteStock_All(const STOCK *stock);
enCodeReturn ReadStock_All(STOCK *stock);
enCodeReturn WriteStock(int index, const STOCK_INFO *stock_info, const VOUCHER_INFO *voucher);
enCodeReturn ReadStock(int index, STOCK_INFO *stock_info, VOUCHER_INFO *voucher);

// Added by Kim_LinHB 2013-12-4 v4.76
enCodeReturn UpdateStockOrder(const STOCK_NEW *stock);
enCodeReturn ReadStockOrder(STOCK_NEW *stock);
enCodeReturn GetStockOrderById(const unsigned char *OrderId, STOCK_NEW *stock); // Added by Kim_LinHB 2013-12-17 v4.78
enCodeReturn GetStockOrderByProvisionId(const unsigned char *ProvisionId, STOCK_NEW *stock);
enCodeReturn DelStockOrder();

enCodeReturn WritePendingOrder(PENDING_ORD order);
enCodeReturn ReadPendingOrder(PENDING_ORD *order);
enCodeReturn DelPendingOrder();

enCodeReturn SaveOLTransInfo(const TRANS_DATA *pstCur);
enCodeReturn FindOLTranWithProvisionId(TRANS_DATA *pstTran);
enCodeReturn DelOLTranByProvisionId(TRANS_DATA stTran);

enCodeReturn WriteUsers(const unsigned char *Name, const unsigned char *Pwd);
enCodeReturn ReadUsersPwd(const unsigned char *Name, unsigned char *Pwd);
enCodeReturn DelUsers(const unsigned char *Name);
enCodeReturn GetUsersList(USER_INFO *Users, unsigned int *iNums);

// Added by Kim_LinHB 2013-5-22 v4.59
enCodeReturn SaveCashOrder(const TRANS_DATA_CASH *pstCur);
enCodeReturn FindCashOrderWithProvisionId(TRANS_DATA_CASH *pstTran);
enCodeReturn DelCashOrderByProvisionId(TRANS_DATA_CASH stTran);
// Add End

void DataInit(void);

unsigned char FileCpy(const unsigned char *dstFile, const unsigned char *srcFile);

#define SeverError "200"
extern unsigned char gl_szFTPErrInfo[256];
extern FTPFILE_INFO gl_stFTPFileInfo;
int PubFtpFileRead(unsigned char *pszFileName, long lOffset, void *psData, int iLen);
int PubFtpFileWrite(unsigned char *pszFileName, long lOffset, void *psData, int iLen);
int ReadFTPFileInfo(void);
int WriteFTPFileInfo(void);

// Added by Kim_LinHB 2014-5-12
int WriteReversal(const unsigned char *pszProvisionId, unsigned char ucMessageType, const unsigned char *pszUser);
int ReadReversal(ReversalInfo *pstData);
int DelReversal(void);

#endif

