/*
 ============================================================================
 Name        : mPayExchange.h
 Author      : PAX
 Version     : 
 Copyright   : PAX Computer Technology(Shenzhen) CO., LTD
 Description : PAX POS Shared Library
 ============================================================================
 */
 
 
#ifndef SHARED_LIBRARY_H_mPayExchange
#define SHARED_LIBRARY_H_mPayExchange

#ifdef __cplusplus
extern "C"
{
#endif

#define MPAY_CONF_DATA_FILE  "/tmp/mobilePay.conf"
#define MPAY_ECR_DATA_FILE   "/tmp/mobilePay.ecr"


typedef enum {
	MPAY_SUCCESS = 0,
	MPAY_ERR_INTERNAL,
	MPAY_ERR_FILE_NOT_EXISTS,
	MPAY_ERR_FILE_ACCESS,
	MPAY_ERR_MORE_SPACE,
	MPAY_ERR_FILE_DATA_INVALID,
	MPAY_ERR_FILE_CHECKSUM
} eMpayErrors;


enum {
	MOBILEPAY_STATUS_NOT_CONFIGURED    = '1',
	MOBILEPAY_STATUS_NOT_REGISTERED    = '2',
	MOBILEPAY_STATUS_NOT_OPERATIVE     = '3',
	MOBILEPAY_STATUS_OPERATIVE         = '4',
	MOBILEPAY_STATUS_OPERATION_PENDING = '5'
} eMPayAppStatus;

enum {
	MOBILEPAY_BOOT_EVENT		 = '1',
	MOBILEPAY_DLLCOMPLETE_EVENT	 = '2',
	MOBILEPAY_PAY_REQUEST		 = '3',
	MOBILEPAY_REFUND_REQUEST	 = '4',
	MOBILEPAY_SHOWLAST_REQUEST	 = '5',
	MOBILEPAY_DOPENDING_REQUEST	 = '6',
	MOBILEPAY_UNREGISTER_REQUEST = '7',
	MOBILEPAY_ENDOFDAY_REQUEST	 = '8'
} eMPayAppReason;


struct mobilePayDataConf {
	// MobilePay configuration
	char szMerchantID[10 + 1];
	char szLocationID[5 + 1];
	char szAPIkey[128];
	char szPoSname[15 + 1];

	// MobilePay server address
	char szIPorURL[128];
	unsigned short usPort;

	// Connection device
	unsigned char ucConnDevice;

	// Wi-Fi connection parameters
	ST_WifiApSet tWiFiSettings;

	// GPRS connection parameters
	char szGPRSapn[50 + 1];
	char szGPRSusername[50 + 1];
	char szGPRSpassword[50 + 1];
	long lGPRSauth;
	int KeepAlive;
};


struct mobilePayDataECR {
	//transaction start (filled by MAINAPP)
	char cReason; 		// �1� boot, �2� configuration end, �3� payment,
						// �4� refund, �5� show last payment result,
						// �6� execute pending ops, '7' unregister
	char szAmount[12 + 1]; // only for payment
	char szOrderID[128];
	char szBulkRef[18 + 1];
	// MobilePay application status (filled by MobilePay app)
	char cAppStatus; 	// �1� not configured, �2� not registered,
						// �3� not operative, �4� operative,
						// �5� operation pending
	//transaction response (filled by MobilePay app)
	char szPaymentStatus[3 + 1];
	char szReminder[12 + 1];
	char szTransactionID[128];
	char szCustomerID[128];
	char szCustomerToken[128];
	char szCustomerReceiptToken[32 + 1];
	// only for denied transactions
	char szStatusCode[2 + 1];
	char szStatusText[256];

	// totals management
	char szTotals[13+1];
	char szPaymentsTotal[12+1];
	char szRefundsTotal[12+1];
};

int __loadMPayFiles(struct mobilePayDataConf *pConf, struct mobilePayDataECR *pECR);
int __storeMPayFiles(struct mobilePayDataConf *pConf, struct mobilePayDataECR *pECR);

#ifdef __cplusplus
};
#endif

#endif /* SHARED_LIBRARY_H_mPayExchange */

