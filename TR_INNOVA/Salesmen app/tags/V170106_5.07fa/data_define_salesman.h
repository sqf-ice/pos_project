/****************************************************************************
NAME
    data_define_salesman.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.14     - created
****************************************************************************/
#ifndef _DATA_DEFINE_SALESMAN_H
#define _DATA_DEFINE_SALESMAN_H

#define FILE_PARAM		"PARAMSALE.DAT"
#define FILE_OFFLINE	"OFFTXNNW.DAT" // Modified by Kim_LinHB 2013-11-20 v4.72 from OFFLTXN
#define FILE_OFFLINE_TMP "OFFLTMP.DAT"
#define FILE_OFF_CANCEL "OFFCANCL.DAT"
#define FILE_MERCHANT	"MERCHANT.DAT"

//#define FILE_BACKUP     "REMOTE_BACKUP_S" //not used
#define FTPFILE			"FTPFILEINFO"
#define FILE_UPDATE		"APP.BIN"

// Added by Kim_LinHB 2013-9-12 v4.67
#define FILEPRODUCT_TEMP	"PRODUCTLIST"


#define SIZE_NUM_SERIE_TPE	8
#define SIZE_CASHIER_NAME	20

#define SIZE_BATCH_NO		10
#define SIZE_DATE_TIME		12
#define SIZE_MAX_CODE_PRODUCT 4
#define SIZE_MAX_CODE_MANAGER 15
//#define SIZE_MAX_ID_MANAGER  4
//#define SIZE_MAX_ID_TERMINAL 8
#define SIZE_MAX_PWD 8
#define SIZE_MIN_PWD 4
#define SIZE_MAX_LIBELLE_PRODUCT 50
#define SIZE_MAX_AMOUNT		15		//money
#define SIZE_MAX_GENCODE	13
#define SIZE_MAX_QUANTITY   5
#define SIZE_MAX_DISCOUNT	5
#define SIZE_NUMERO_SERIE_TERMINAL_PAX	8
#define SIZE_MAX_ADDRESS        100
#define SIZE_MAX_PROVISION_ID 20

#define NB_MAX_MERCHANT			5000
#define NB_MAX_CAISSIERS		15
#define NB_MAX_CATEGORYS		75    //modified by Allen 20151204 from 50 to 75
#define NB_MAX_PRODUCTS      400      // modified by Allen 20151204 from 150 to 400
#define NB_MAX_ORDERS           200

#define MAX_APN				20
#define MAX_USER			50 
#define MAX_PWD				20 
#define MAX_IP				15
#define MAX_PORT			5
#define MAX_PIN				20

#define SIZE_MAX_NUM_PAD 16
#define SIZE_MAX_TYPE_IMPRIMANTE 1

#ifndef MAX_TCP_BUFFER 
#define MAX_TCP_BUFFER	(4096 * 10)
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

typedef struct _tagPendingOrders   ///
{
	unsigned char ucOrderNum;
	struct _tagPendingOrdersInfo
	{
		unsigned char szId[10 + 1]; // Modified by Kim_LinHB 2013-11-14 v4.71 from 5 to 10
		unsigned char szTotAmt[SIZE_MAX_AMOUNT + 1];
		unsigned char szTermID[SIZE_MAX_CODE_MANAGER + 1];
	}PendingOrdersInfo[NB_MAX_PRODUCTS];
}PENDING_ORDERS;

typedef struct _tagSummary  ///
{
	unsigned char szCashTotalAmt_Gross[SIZE_MAX_AMOUNT + 1];
	unsigned char szCashTotalAmt[SIZE_MAX_AMOUNT + 1];
	unsigned char szOffTotalAmt_Gross[SIZE_MAX_AMOUNT + 1];
	unsigned char szOffTotalAmt[SIZE_MAX_AMOUNT + 1];
	// Added by Kim_LinHB 2013-7-22 v4.60
	unsigned char szCreditTotalAmt_Gross[SIZE_MAX_AMOUNT + 1];
	unsigned char szCreditTotalAmt[SIZE_MAX_AMOUNT + 1];
	// Add End
}xSummary;

typedef struct _tagCashierInfo
{
	unsigned char szName[SIZE_CASHIER_NAME + 1];
	unsigned char szPwd[SIZE_MAX_PWD + 1];
}USER_INFO;

typedef struct _tagMaintenaceParametres_OLD //before v4.60
{
	unsigned char ucVersion;
	unsigned char szManagerID[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szTermID[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szSalesmanID[SIZE_MAX_CODE_MANAGER + 1];
	//unsigned char szDefDiscount[SIZE_MAX_DISCOUNT + 1];
	unsigned char szSalesmanName[MAX_USER + 1]; 
	unsigned char szSalesmanCredit[SIZE_MAX_AMOUNT + 1];
	unsigned char szPhysicalStock[SIZE_MAX_AMOUNT + 1];
	unsigned char szDealerCode[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szMaintenancePwd[SIZE_MAX_PWD + 1];
	unsigned char szNetworkPwd[SIZE_MAX_PWD + 1];
	USER_INFO	  stUser;
	unsigned char ucLang;
	//unsigned char bChangeDiscount;
	unsigned char bPrintBarCode;
	unsigned char bPrintLogo;
	unsigned char bCancelSale;
	unsigned char bScanSerialNumber;
	unsigned char bInputSerial;
	unsigned char bDoublePaper;

	unsigned char szLogLv[1 + 1];
	unsigned char ucConnType[2];
	unsigned char ucConnSel;

	//	unsigned char did_SoftDownLoad; // Removed by Kim_LinHB 2013-3-20 v4.55 and all positions in src
	unsigned char szLastSoftDownLoad_Time[SIZE_DATE_TIME + 1];
	FTP_PARAM		stFTP;

	CONN_PARAM stConnParam;
}MAINTENACE_PARAM_OLD;

typedef struct _tagMaintenaceParametres
{
	unsigned char ucVersion;
	unsigned char szManagerID[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szTermID[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szSalesmanID[SIZE_MAX_CODE_MANAGER + 1];
	//unsigned char szDefDiscount[SIZE_MAX_DISCOUNT + 1];
	unsigned char szSalesmanName[MAX_USER + 1]; 
	unsigned char szSalesmanCredit[SIZE_MAX_AMOUNT + 1];
	unsigned char szPhysicalStock[SIZE_MAX_AMOUNT + 1];
	unsigned char szDealerCode[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szMaintenancePwd[SIZE_MAX_PWD + 1];
	unsigned char szNetworkPwd[SIZE_MAX_PWD + 1];
	USER_INFO	  stUser;
	unsigned char ucLang;
	//unsigned char bChangeDiscount;
	unsigned char bPrintBarCode;
	unsigned char bPrintLogo;
	unsigned char bCancelSale;
	unsigned char bScanSerialNumber;
	unsigned char bInputSerial;
	unsigned char bDoublePaper;
	
	unsigned char szLogLv[1 + 1];
	unsigned char ucConnType[2];
	unsigned char ucConnSel;

	unsigned char szLastSettDownLoad_Time[SIZE_DATE_TIME + 1];
	unsigned char szTimeSettDownload[4 + 1];

//	unsigned char did_SoftDownLoad; // Removed by Kim_LinHB 2013-3-20 v4.55 and all positions in src
	unsigned char szLastSoftDownLoad_Time[SIZE_DATE_TIME + 1];
	FTP_PARAM		stFTP;

	CONN_PARAM stConnParam;
}MAINTENACE_PARAM;

typedef struct _tagMaintenaceParametres_EXT
{	// total 1024
	unsigned char version[10]; // Added by Kim_LinHB 2013-9-3 v4.65
	unsigned char cConnectionTimeout; // Added by Kim_LinHB 2013-11-11 v4.70 sec
	unsigned char sReserved[1013];
}MAINTENACE_PARAM_EXT;  // Added by Kim_LinHB 2013-7-12 v4.60

typedef struct _tagxParamCategory_OLD
{
	unsigned char ucCodeCategory;			// Code Category
	unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];	// Name
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucCategoryType;
	unsigned char ucNumLogo;
}xParamCategory_OLD;

// Modified by Kim_LinHB 2013-11-20 v4.72
typedef struct _tagxParamCategory
{
	unsigned int uiCodeCategory;			// Code Category
	unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT+1];	// Name
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucCategoryType;
	unsigned char ucNumLogo;
}xParamCategory;

typedef struct _tagxParamProducts_OLD//以这个结构体为单位, 下载产品表并进行存放
{
	unsigned char ucCodeCategory;			// Code Category
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucType;			//temp
	unsigned char szCodeProduct[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	//unsigned char szGenCode[SIZE_MAX_GENCODE + 1];		// Gencode servant a l'impression du code barre 条码
	unsigned char bCredit;
	unsigned char sReserved[SIZE_MAX_GENCODE];
	unsigned char szNameProduct[SIZE_MAX_LIBELLE_PRODUCT + 1]; // Name du product
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];	// Amount
	unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
	unsigned char szSoldQuantity[SIZE_MAX_QUANTITY + 1];
	unsigned char szAverage[SIZE_MAX_QUANTITY + 1];
	unsigned char szOrderNum[SIZE_MAX_QUANTITY + 1];
	unsigned char szDiscount[SIZE_MAX_DISCOUNT + 1];		//Discount
}xParamProducts_OLD;

// Modified by Kim_LinHB 2013-11-20 v4.72
typedef struct _tagxParamProducts//以这个结构体为单位, 下载产品表并进行存放
{
	unsigned int uiCodeCategory;			// Code Category
	/*	
		1-Physical Product, 
		2-E-Voucher
	*/
	unsigned char ucType;			//temp
	unsigned char szCodeProduct[10 + 1];	// Code product
	//unsigned char szGenCode[SIZE_MAX_GENCODE + 1];		// Gencode servant a l'impression du code barre 条码
	unsigned char bCredit;
	unsigned char sReserved[SIZE_MAX_GENCODE];
	unsigned char szNameProduct[SIZE_MAX_LIBELLE_PRODUCT + 1]; // Name du product
	unsigned char szAmount[SIZE_MAX_AMOUNT + 1];	// Amount
	unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
	unsigned char szSoldQuantity[SIZE_MAX_QUANTITY + 1];
	unsigned char szAverage[SIZE_MAX_QUANTITY + 1];
	unsigned char szOrderNum[SIZE_MAX_QUANTITY + 1];
	unsigned char szDiscount[SIZE_MAX_DISCOUNT + 1];		//Discount
}xParamProducts;

typedef struct _tagxMerchantLists
{
	unsigned char szMerchantID[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szMerchantCode[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szSerial[SIZE_MAX_CODE_MANAGER + 1]; // Merchant Terminal Id
	unsigned char szTermCode[SIZE_MAX_CODE_MANAGER + 1]; //SIZE_MAX_ID_TERMINAL
	unsigned char szName[MAX_USER + 1];
	unsigned char szAddress[SIZE_MAX_ADDRESS + 1];
}xMerchantList;

typedef struct _tagProductsInfo_OLD
{
	unsigned char ucCodeCategory;	// Code Category
	unsigned char szCodeProduct[SIZE_MAX_CODE_PRODUCT + 1];	// Code product
	unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
}PRODUCT_INFO_OLD;

// Modified by Kim_LinHB 2013-11-20 v4.72
typedef struct _tagProductsInfo
{
	unsigned int uiCodeCategory;	// Code Category
	unsigned char szCodeProduct[10 + 1];	// Code product
	unsigned char szQuantity[SIZE_MAX_QUANTITY + 1];
}PRODUCT_INFO;

typedef struct _tagRoute
{
	unsigned char szMerchantListDate[SIZE_DATE_TIME + 1];
	unsigned int  uiMerchantListNum;			
	//xMerchantList stMerchantLists[NB_MAX_MERCHANT];
}ROUTE_INFO;

typedef struct _tagxSavParametres_OLD // before v4.60
{
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned int  uiMerchantListIndex;
	unsigned int  uiProductsListIndex; // Added by Kim_LinHB 2013-4-2 v4.56
	unsigned char  ucCurOfflineNo;
	unsigned char bIsSetup;

	unsigned char ucKey[16];
	unsigned char szOrderId[SIZE_MAX_CODE_MANAGER];

	unsigned char ucCategoryNum;
	unsigned char ucProductsNum;

	xParamCategory_OLD stSavParamCategory[50];
	xParamProducts_OLD stSavParamProducts[150];
	ROUTE_INFO	   stRoute;
}SAV_PARAM_OLD;

typedef struct _tagxSavParametres_OLD2
{
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned int  uiMerchantListIndex;
	unsigned int  uiProductsListIndex; // Added by Kim_LinHB 2013-4-2 v4.56
	unsigned int  uiCurOfflineNo;      // Modified by Kim_LinHB 2013-7-12 v4.60
	unsigned char bIsSetup;

	unsigned char ucKey[16];
	unsigned char szOrderId[SIZE_MAX_CODE_MANAGER];

	unsigned char ucCategoryNum;
	unsigned char ucProductsNum;
	
	xParamCategory_OLD stSavParamCategory[50];
	xParamProducts_OLD stSavParamProducts[150];    //zheli he old3 you bianhua 
	ROUTE_INFO	   stRoute;
}SAV_PARAM_OLD2;

// Modified by Kim_LinHB 2013-11-20 v4.72
typedef struct _tagxSavParametres_OLD3
{
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned int  uiMerchantListIndex;
	unsigned int  uiProductsListIndex; // Added by Kim_LinHB 2013-4-2 v4.56
	unsigned int  uiCurOfflineNo;      // Modified by Kim_LinHB 2013-7-12 v4.60
	unsigned char bIsSetup;
	unsigned char ucKey[16];
	unsigned char szOrderId[SIZE_MAX_CODE_MANAGER];
	unsigned char ucCategoryNum;
	unsigned char ucProductsNum;
	xParamCategory stSavParamCategory[50];
	xParamProducts stSavParamProducts[150];
	ROUTE_INFO	   stRoute;
}SAV_PARAM_OLD3;
typedef struct _tagxSavParametres
{
	unsigned short usTraceNo;
	unsigned short usBatchNo;
	unsigned int  uiMerchantListIndex;
	unsigned int  uiProductsListIndex; // Added by Kim_LinHB 2013-4-2 v4.56
	unsigned int  uiCurOfflineNo;      // Modified by Kim_LinHB 2013-7-12 v4.60
	unsigned char bIsSetup;

	unsigned char ucKey[16];
	unsigned char szOrderId[SIZE_MAX_CODE_MANAGER];

	unsigned char ucCategoryNum;
	unsigned int uiProductsNum;  // modified by Allen from unsigned char to int 

	xParamCategory stSavParamCategory[NB_MAX_CATEGORYS];
	xParamProducts stSavParamProducts[NB_MAX_PRODUCTS];
	ROUTE_INFO	   stRoute;
}SAV_PARAM;

typedef struct _tagxTransData_OLD
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

	unsigned char ucProductNum;
	PRODUCT_INFO_OLD  stProducts[150];
}TRANS_DATA_OLD;

// Modified by Kim_LinHB 2013-11-20 v4.72
typedef struct _tagxTransData_OLD2
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
	unsigned char ucProductNum;
	PRODUCT_INFO  stProducts[150];
}TRANS_DATA_OLD2;
typedef struct _tagxTransData
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

	unsigned int uiProductNum;   // modified by Allen 2015-01-05
	PRODUCT_INFO  stProducts[NB_MAX_PRODUCTS];
}TRANS_DATA;

enum{
	ORDER_CASH = 1,
	ORDER_CREDIT_TRANSFER,
	ORDER_CREDIT,
}; // Added by Kim_LinHB 2013-7-22 v4.60
typedef struct _tagOrderReport
{
	unsigned char szMerchantId[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szSalesmanId[SIZE_MAX_CODE_MANAGER + 1];
	unsigned char szMerchantName[MAX_USER + 1];
	unsigned char szMerchantTownName[MAX_USER + 1];
	unsigned char szSalesmanName[MAX_USER + 1];
	unsigned char szTotAmt[SIZE_MAX_AMOUNT + 1];
	unsigned int  uiOrderCnt;
	struct _tagOrder
	{
		unsigned char szId[SIZE_MAX_CODE_MANAGER + 1];
		unsigned char szMerchantId[SIZE_MAX_CODE_MANAGER + 1];
		unsigned char szMerchantName[MAX_USER + 1];
		unsigned char szTotalAmt[SIZE_MAX_AMOUNT + 1];
	}Order[NB_MAX_ORDERS];
}OrderReport;

// Added by Kim_LinHB 2013-7-30 v4.60
typedef struct _tagOrderReport_Prod
{
	unsigned char szSalesmanId[100 + 1];
	unsigned char szSalesmanName[MAX_USER + 1];
	unsigned char szCredit[SIZE_MAX_AMOUNT + 1];
	unsigned char szTotAmt[SIZE_MAX_AMOUNT + 1];
	unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
	unsigned int  uiTotalOrders;
	unsigned int  uiGroupCnt;
	unsigned int  uiProductCnt;
	struct _tagGroups
	{
		unsigned char szTitle[SIZE_MAX_LIBELLE_PRODUCT + 1];
		unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT + 1];
		unsigned char szTotAmt[SIZE_MAX_AMOUNT + 1];
		unsigned char szDiscount[SIZE_MAX_AMOUNT + 1];
		unsigned int  uiTotalOrders;
		unsigned int  uiProductTypes;
	}Groups[NB_MAX_CATEGORYS];
	struct _tagProd
	{
		unsigned int groupIndex;
		unsigned char szName[SIZE_MAX_LIBELLE_PRODUCT + 1];
		unsigned char szTotAmt[SIZE_MAX_AMOUNT + 1];
		unsigned int  uiTotalOrders;
	}Products[NB_MAX_PRODUCTS];
}OrderReport_Prod;
// Add End

// Added by Kim_LinHB 2013-10-28 v4.68
typedef struct _tagCONSIGNMENT_LIST
{
	unsigned int num;
	struct _tagConsigOrderList
	{
		unsigned char Id[10 + 1];
		unsigned char Type;
		unsigned char CreateDate[22 + 1]; //"MM/DD/YYYY hh:mm:ss AM(/PM)"
		unsigned char TotalPrice[SIZE_MAX_AMOUNT + 1];
	}ConsigOrderList[NB_MAX_PRODUCTS];
}CONSIGNMENT_LIST;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

extern const unsigned char *cSTRConnType[];

extern unsigned char gl_bFirstRun;
extern unsigned char gl_bSettDown;
extern unsigned int  gl_TermMode;
extern unsigned char gl_TermName[8+1];
extern unsigned char gl_TermSN[32+1];

extern MAINTENACE_PARAM gl_MaintenaceParam;
extern MAINTENACE_PARAM_EXT gl_MaintenaceParam_Ext; // Added by Kim_LinHB 2013-7-12 v4.60
extern SAV_PARAM		gl_SavParam;
extern TRANS_DATA		gl_LastTran;
extern xSummary gl_Summary_tmp;
extern unsigned char	gl_ProvisionIdTmp[SIZE_MAX_PROVISION_ID+1];

extern unsigned char gl_bPlanned_SoftDownload;
extern unsigned char gl_bPlanned_SettDownload;

enum
{
	SIMUL = 0, 
	//REALV22BIS, 
	//REALV22, 
	ETHERNET, 
	GPRS, 
	//PPP,
	ENDFLG,
};

enum
{
	TYPE_BARCODE,
	TYPE_MANUAL_ID,
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
void DataInit(void);
enCodeReturn SaveOfflineTrans(const TRANS_DATA *pstCurTrans, const unsigned char *FileName);
unsigned char FileCpy(const unsigned char *dstFile, const unsigned char *srcFile);
unsigned char GetOfflineLastTrans(TRANS_DATA *pstCurTrans);
unsigned char GetOfflineOrder(unsigned int index, TRANS_DATA *pstCurTrans);
unsigned char DelOfflineOrder(unsigned int index);
unsigned char DeleteOfflineLastTrans(void);
unsigned char SaveOffCancel(const TRANS_DATA *pstCurTrans);

unsigned int GetOfflineTransCnt(unsigned char type);

unsigned char WriteMerchant(const unsigned char *fileName, xMerchantList stMerchant);
unsigned char GetMerchant(const unsigned char *fileName, unsigned int index, \
						  xMerchantList *pstMerchant);

#define SeverError "200"
extern unsigned char gl_szFTPErrInfo[256];
extern FTPFILE_INFO gl_stFTPFileInfo;
int PubFtpFileRead(unsigned char *pszFileName, long lOffset, void *psData, int iLen);
int PubFtpFileWrite(unsigned char *pszFileName, long lOffset, void *psData, int iLen);
int ReadFTPFileInfo(void);
int WriteFTPFileInfo(void);

#endif

