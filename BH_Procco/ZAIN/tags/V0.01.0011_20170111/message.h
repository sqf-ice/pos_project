#ifndef _MESSAGE_H_
#define _MESSAGE_H_

enum APP_MSG						// application message index
{ M_MERCH1=1,   M_CARDNOT,    M_MERCH2,      M_TESTCARD,
  M_BANKCARD,   M_VISACARD,   M_EUROCARD,    M_AUTHNUM,
  M_DEBIT,      M_CREDIT,     M_CANCEL1,     M_CANCEL2,
  M_YES,        M_NO,         M_OPTION,      M_MERCHCARD,
  M_SUCCESS,    M_WARNING,    M_INTERROMPU,  M_ECHECCONNEX,
  M_PBCONNEX,   M_FORCAGE,    M_SETTLECALL,	 M_SWDOWNLOAD,
  M_TREATMENT
};

//extern HIERARCHY gFunctionMenu[];	// function menu
extern /*MENU*/const BYTE *gLanguageMenu[];
extern /*MENU*/const BYTE *gTermMenu[];
extern /*MENU*/const BYTE *gSurchargeMenu[];
extern /*MENU*/const BYTE *gListeMenu[];
extern /*MENU*/const BYTE *gManagmtMenu[];
extern /*MENU*/const BYTE *gDebugMenu[];
extern /*MENU*/const BYTE *gConnectMenu[];     //added by Allen 2016-05-19
extern /*MENU*/const BYTE *gReversalMenu[];      //added by Allen 2016-07-05
extern /*MENU*/const BYTE *gDebugMenuType[];
extern /*MENU*/const BYTE *gSecurityFnctMenu[];

extern const int gHostErrMsgTableFlag[20];
extern const BYTE gHostErrMsgForDisp[15][100];
extern const BYTE gHostErrMsgForPRT[15][100];
//extern MENU gSyncModeMenu;

extern MENU csMENU_COPY;
extern const BYTE *AppMsgTable[];			// application message table

/////////////////////////////////////////////////////////////////////////////
//UTILITY SIGNS////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE UTIL_LINE[];
extern BYTE UTIL_SPACES[];
extern BYTE UTIL_EQUALS[];
extern BYTE UTIL_STARS[];
extern BYTE UTIL_1800[];
extern BYTE UTIL_FILLZEROS[];
/////////////////////////////////////////////////////////////////////////////
//DEBUG MESSAGES///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

extern BYTE DEB_READ_BUFFER[];
extern BYTE DEB_MAC_BUFFER[];
extern BYTE DEB_MAC[];
extern BYTE DEB_SCR_BUFFER[];
extern BYTE DEB_COEFFICIENTS[];
extern BYTE DEB_PAN_BUFFER[];
extern BYTE DEB_INV_PAN_BUFFER[];
extern BYTE DEB_ARQC[];
extern BYTE DEB_ARC_ARQC[];
extern BYTE DEB_CARD_CR_KEY1_0[];
extern BYTE DEB_CARD_CR_KEY2_0[];
extern BYTE DEB_CARD_CR_KEY1_1[];
extern BYTE DEB_CARD_CR_KEY2_1[];
extern BYTE DEB_CR_SESSION_KEY_L[];
extern BYTE DEB_CR_SESSION_KEY_R[];
extern BYTE DEB_CARD_SC_KEY1[];
extern BYTE DEB_CARD_SC_KEY2[];
extern BYTE DEB_SC_SESSION_KEY_L[];
extern BYTE DEB_SC_SESSION_KEY_R[];
extern BYTE DEB_ENCRYPTED_PIN[];
extern BYTE DEB_CARD_MAC_KEY1[];
extern BYTE DEB_CARD_MAC_KEY2[];
extern BYTE DEB_MAC_SESSION_KEY_L[];
extern BYTE DEB_MAC_SESSION_KEY_R[];
extern BYTE DEB_PAN[];
extern BYTE DEB_EXPIRY_DATE[];
extern BYTE DEB_SW_BAL[];
extern BYTE DEB_BALANCE[];
extern BYTE DEB_SW_UPPER_LIM[];
extern BYTE DEB_UPPER_LIMIT[];
extern BYTE DEB_SW_PARAMETER[];
extern BYTE DEB_PARAMETERS[];
extern BYTE DEB_SCRIPT_RESULTS_72[];
extern BYTE DEB_UNPREDICTABLE_NUMBER[];
extern BYTE DEB_CAPK_INDEX[];
extern BYTE DEB_CVM_RESULTS[];
extern BYTE DEB_AIP[];
extern BYTE DEB_CID_1[];
extern BYTE DEB_ISSUER_APP_DATA_1[];
extern BYTE DEB_TVR_1[];
extern BYTE DEB_TSI_1[];
extern BYTE DEB_PAN_SEQUENCE_NUMBER[];
extern BYTE DEB_PINTRYCOUNT[];
extern BYTE DEB_ISS_AUTH_DATA[];
extern BYTE DEB_NO_ONLINE_CAPABILITY[];
extern BYTE DEB_AIP_FINAL[];
extern BYTE DEB_CID_FINAL[];
extern BYTE DEB_ISSUER_APP_DATA_FINAL[];
extern BYTE DEB_TVR_FINAL[];
extern BYTE DEB_TSI_FINAL[];
extern BYTE DEB_DATA_AUTHENTICATION[];
extern BYTE DEB_SDA_SUCCESS[];
extern BYTE DEB_SDA_ERROR[];
extern BYTE DEB_CARD_REFUSED[];
extern BYTE DEB_SDA_FAIL[];
extern BYTE DEB_SDA_FAIL_CAPK[];
extern BYTE DEB_SDA_STAT_UNKNOW[];
extern BYTE DEB_DDA_SUCCESS[];
extern BYTE DEB_DDA_ERROR_CARD[];
extern BYTE DEB_DDA_ERROR[];
extern BYTE DEB_DDA_FAIL[];
extern BYTE DEB_DDA_FAIL_CAPK[];
extern BYTE DEB_DDA_STAT_UNKNOWN[];
extern BYTE DEB_CDA_SUCCESS[];
extern BYTE DEB_CDA_ERROR_CARD[];
extern BYTE DEB_CDA_ERROR[];
extern BYTE DEB_CDA_FAIL[];
extern BYTE DEB_CDA_FAIL_CAPK[];
extern BYTE DEB_CDA_STAT_UNKNOWN[];
extern BYTE DEB_VERIFY_CARDHOLDER_OK[];
extern BYTE DEB_VERIFY_CARDHOLDER_NOK[];
extern BYTE DEB_VERIFY_CARDHOLDER_ERROR[];
extern BYTE DEB_VERIFY_CARDHOLDER_FAIL[];
extern BYTE DEB_VERIFY_CARDHOLDER_NOTRQ[];
extern BYTE DEB_VERIFY_CARDHOLDER_[];
extern BYTE DEB_WARNING_PIN_BLOCKED[];
extern BYTE DEB_SIGNATURE_IS_REQUIRED[];
extern BYTE DEB_CAA_OK[];
extern BYTE DEB_CAA_COM_ERROR[];
extern BYTE DEB_CAA_ERROR[];
extern BYTE DEB_CAA_BLOCKED[];
extern BYTE DEB_CAA_NOT_COHERENT[];
extern BYTE DEB_CAA_ERROR_WRD[];
extern BYTE DEB_CAA_CDA_FAILED[];
extern BYTE DEB_CAA_[];
extern BYTE DEB_CAA_DECISION[];
extern BYTE DEB_ACCEPT[];
extern BYTE DEB_GO_ONLINE[];
extern BYTE DEB_REFERRAL[];
extern BYTE DEB_REFUSE[];
extern BYTE DEB_AC_1[];
extern BYTE DEB_AC_2[];
extern BYTE DEB_ATC[];
extern BYTE DEB_DATES_CHECKING_OK[];
extern BYTE DEB_APPLI_IS_EFFECTIVE_YES[];
extern BYTE DEB_APPLI_HAS_EXPIRED_NO[];
extern BYTE DEB_DATES_CHECKING[];
extern BYTE DEB_APPLI_IS_EFFECTIVE_NO[];
extern BYTE DEB_APPLI_HAS_EXPIRED_[];
extern BYTE DEB_APPLI_HAS_EXPIRED_YES[];
extern BYTE DEB_DATES_CHECKING_ERROR[];
extern BYTE DEB_DATES_CHECKING_[];
extern BYTE DEB_RANDOM_SEL_IN_PRM_ERR[];
extern BYTE DEB_RANDOM_SEL_OK[];
extern BYTE DEB_RANDOM_SEL_NOK[];
extern BYTE DEB_RANDOM_SEL_NOT_REQUIRED[];
extern BYTE DEB_RANDOM_SEL_[];
extern BYTE DEB_READ_DATA_OK[];
extern BYTE DEB_READ_DATA_CARD_ERROR[];
extern BYTE DEB_READ_DATA_ERROR[];
extern BYTE DEB_CARD_REFUSED_[];
extern BYTE DEB_READ_DATA_FILE_ERROR[];
extern BYTE DEB_READ_DATA_[];
extern BYTE DEB_TAA_OK[];
extern BYTE DEB_TAA_NOK[];
extern BYTE DEB_TAA_[];
extern BYTE DEB_TERM_DECISION[];
extern BYTE DEB_USAGE_CONTROL_OK[];
extern BYTE DEB_USAGE_CONT_RESTRICTED[];
extern BYTE DEB_USAGE_CONTROL_[];
extern BYTE DEB_COM_ERROR[];
extern BYTE DEB_VELOCITY_CHECKING_FAIL[];
extern BYTE DEB_VELOCITY_CHECKING_OK[];
extern BYTE DEB_VELOCITY_CHECKING_MINUS[];
extern BYTE DEB_VELOCITY_CHECKING[];
extern BYTE DEB_VELOCITY_CHECKING_[];
extern BYTE DEB_EXCEED_LOWER_LIMIT_NO[];
extern BYTE DEB_EXCEED_LOWER_LIMIT_YES[];
extern BYTE DEB_EXCEED_UPPER_LIMIT_NO[];
extern BYTE DEB_EXCEED_UPPER_LIMIT_YES[];
extern BYTE DEB_VERSION_SUPPORTED[];
extern BYTE DEB_VERSION_NOT_SUPPORTED[];
extern BYTE DEB_VERSION_CHECK[];
extern BYTE DEB_CARD_HOLDER_NAME[];
extern BYTE DEB_TRACK_I[];
extern BYTE DEB_TRACK_II[];
extern BYTE DEB_CRM_COUNTRY_SW[];
extern BYTE DEB_CRM_COUNTRY[];
extern BYTE DEB_CRM_CURRENCY_SW[];
extern BYTE DEB_CRM_CURRENCY[];
extern BYTE DEB_PACKET_RECEIVED[];
extern BYTE DEB_FRAME_ERROR[];
extern BYTE DEB_FIELD[];
extern BYTE DEB_FIELD_DATA[];
extern BYTE DEB_PACKET_SEND[];
extern BYTE DEB_TYPE_ERROR[];
extern BYTE DEB_SIZE_ERROR[];
extern BYTE DEB_DATA_SIZE_OVER[];

extern BYTE DEB_REVERSAL_OPEN_FAILG[];
extern BYTE DEB_REVERSAL_NOT_PRESENT[];
extern BYTE DEB_REVERSAL_CREATE_FAIL[];
extern BYTE DEB_REVERSAL_PRESENT[];
extern BYTE DEB_REVERSAL_READ_FAIL[];
extern BYTE DEB_REVERSAL_OPEN_FAILA[];
extern BYTE DEB_REVERSAL_RECORDING_FAIL[];

/////////////////////////////////////////////////////////////////////////////
//MESSAGE AND TRANSACTION TYPES////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE TYPE_01[];
extern BYTE TYPE_03[];

extern BYTE TYPE_70[];//Installment
extern BYTE TYPE_71[];//Loyalty
extern BYTE TYPE_72[];//Payment
extern BYTE TYPE_73[];//Account Maint
extern BYTE TYPE_74[];//get balance
extern BYTE TYPE_75[];
extern BYTE TYPE_76[]; //void     added by Allen 2016-05-12

/////////////////////////////////////////////////////////////////////////////
//FILE NAMES///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE FILE_MP[LG_FILENAME+1];		  	   //name of the merchant parameter file
extern BYTE FILE_GP[LG_FILENAME+1];			   //name of the general parameter file
extern BYTE FILE_DP[LG_FILENAME+1];			   //name of the download parameter file
extern BYTE FILE_TR[LG_FILENAME+1];			   //name of the TRansaction file
extern BYTE FILE_LAST_EOD[LG_FILENAME+1];
extern BYTE FILE_RB[LG_FILENAME+1];			   //name of the Batch Reprint file
extern BYTE FILE_R1[LG_FILENAME+1];			   //name of the transaction ticket 1 file
extern BYTE FILE_R2[LG_FILENAME+1];			   //name of the transaction ticket 2 file
extern BYTE FILE_R3[LG_FILENAME+1];			  //name of the transaction ticket 3 file
extern BYTE FILE_RP[];							   //name of temporary file for PDload
extern BYTE FILE_STATE[LG_FILENAME+1];	  	   //name of the module state file

extern BYTE FILE_BC[LG_FILENAME+1];          //Black Card File
extern BYTE FILE_BT[LG_FILENAME+1];          //Black Card Temporary File

extern BYTE FILE_PC[LG_FILENAME+1];          //Product File
extern BYTE FILE_PT[LG_FILENAME+1];          //Product Temporary File

extern BYTE FILE_ST[LG_FILENAME+1];          //Scheme Type
extern BYTE FILE_TS[LG_FILENAME+1];          //Scheme Type Temporary File

extern BYTE FILE_PE[LG_FILENAME+1];          //Repayment Tenures
extern BYTE FILE_TP[LG_FILENAME+1];          //Repayment Tenures Temporary File

extern BYTE FILE_BIN[LG_FILENAME+1];         //Kredi Kart?Bin Listesi
extern BYTE FILE_TEMPBIN[LG_FILENAME+1];     //Kredi Kart?Bin Listesi Temp

extern BYTE FILE_MERCH[LG_FILENAME+1];       //
extern BYTE FILE_TEMPMERCH[LG_FILENAME+1];   //

extern BYTE FILE_REVERSE[LG_FILENAME+1];     // Reverse File

extern BYTE FILE_REF[LG_FILENAME+1];// save refNum added by jeff_xiehuan20161216
extern BYTE csFILE_EMV_KEYS[LG_FILENAME+1]; //
extern BYTE csFILE_EMV_TEMP_KEYS[LG_FILENAME+1]; //

extern BYTE FILE_GPRS_PAR[LG_FILENAME+1]; //
//added by Allen 2016-05-19
extern BYTE FILE_DIAL_UP_PAR[LG_FILENAME+1]; //
extern BYTE FILE_CONNECT_MODE[LG_FILENAME+1]; //
extern BYTE FILE_END_OF_DAY[LG_FILENAME+1]; 
//add end
/////////////////////////////////////////////////////////////////////////////
//PRINT MESSAGES///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE PRT_DESCRIPTION_OF_GOODS[];
extern BYTE PRT_EMPTY[];
extern BYTE PRT_SALE[];
extern BYTE PRT_SCHEME_TYPE[];
extern BYTE PRT_REPRINT[];
extern BYTE PRT_KEEPTHISPAPER[];
extern BYTE PRT_SIGN[];
extern BYTE PRT_GOODS_RECEIVED[];
extern BYTE PRT_GOODS_RECEIVED_LOYALTY[];
extern BYTE PRT_GOODS_RECEIVED_PAYMENT[];
extern BYTE PRT_GOODS_RECEIVED_FOOTER[];
extern BYTE PRT_BATYPE[];
extern BYTE PRT_FAILED[];
extern BYTE PRT_CARD_NUMBER[];
extern BYTE PRT_EXPIRY_DATE[];
extern BYTE PRT_BALANCE_LIST_MSG[];
extern BYTE PRT_PDLD_PARAMDLOAD[];
extern BYTE PRT_PURCHASE_AMOUNT[];
extern BYTE PRT_PAYMENT_AMOUNT[];
extern BYTE PRT_PARMVERSION[];
extern BYTE PRT_DIALMODE[];
extern BYTE PRT_MAXTRANS[];

extern BYTE PRT_INSTALLMENT[];
extern BYTE PRT_LOYALTY[];
extern BYTE PRT_PAYMENT[];
extern BYTE PRT_ACCOUNT_MAINT[];

extern BYTE PRT_PARAMLIST[];
extern BYTE PRT_SN[];
extern BYTE PRT_NO[];
extern BYTE PRT_TOTAL[];
extern BYTE PRT_TOTYPE[];
extern BYTE PRT_LOTYPE1[];
extern BYTE PRT_LOSMARTDEBIT[];
extern BYTE PRT_LOSMARTLOYALTY[];
extern BYTE PRT_LOSMARTPAYMENT[];
//added by Allen 2016-05-25
extern BYTE PRT_LOSMARTREWARD[];
extern BYTE PRT_LOSMARTREDEEM[];
extern BYTE PRT_LOSMARTREVERSAL[];
//add end
//extern BYTE PRT_LOSMARTACCOUNT_MAINT[];

extern BYTE PRT_BATCHSTART[];
extern BYTE PRT_PARMSTART[];
extern BYTE PRT_MINIMUM_LIMIT[];
extern BYTE PRT_END_SUCCESS[];

extern BYTE PRT_BATCH[];
extern BYTE PRT_SLIPNO[];

extern BYTE PRT_CARD[];

extern BYTE PRT_AUTH_PHONE_1[];
extern BYTE PRT_AUTH_PHONE_2[];
extern BYTE PRT_AUTH_PHONE_3[];

extern BYTE PRT_PMS_PHONE_1[];
extern BYTE PRT_PMS_PHONE_2[];
extern BYTE PRT_PMS_PHONE_3[];

extern BYTE PRT_PARAM_PHONE_1[];
extern BYTE PRT_PARAM_PHONE_2[];
extern BYTE PRT_PARAM_PHONE_3[];

extern BYTE PRT_SALE_UNIT[];
extern BYTE PRT_LOYALTY_UNIT[];
extern BYTE PRT_PAYMENT_UNIT[];
extern BYTE PRT_REVERSALNUIT[];   //added by Allen 2016-09-20
//extern BYTE PRT_ACCOUNT_MAINT_UNIT[];

extern BYTE PRT_FINISH_BALANCE[];
extern BYTE PRT_START_BALANCE[];
extern BYTE PRT_REJECT[];
extern BYTE PRT_TERMINAL_SERIAL[];
extern BYTE PRT_NUMBER_WRONG[];
extern BYTE PRT_SLIPHEADERINST[];
extern BYTE PRT_SLIPHEADERLOYALTY[];
extern BYTE PRT_SLIPHEADERPAYMENT[];

extern BYTE PRT_FLAIRINSTALLMENT[];
extern BYTE PRT_FLAIRLOYALTY[];
extern BYTE PRT_FLAIRPAYMENT[];
extern BYTE PRT_FLAIRACCOUNT_MAINT[];
extern BYTE PRT_INST[];
extern BYTE PRT_TENURE[];
extern BYTE PRT_REJECTED[];
/////////////////////////////////////////////////////////////////////////////
//DISPLAY MESSAGES///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE DSP_WITH_PROCCO_CARD[];
extern BYTE DSP_LUHN_ERROR[];
extern BYTE DSP_CALL_CENTER[];
extern BYTE DSP_INVALID_CARD[];
extern BYTE DSP_REJECTED[];
extern BYTE DSP_SCRIPT_ERROR[];
extern BYTE DSP_PLEASE_WAIT[];
extern BYTE DSP_BATCH_OPEN[];
extern BYTE DSP_CONNECTED[];
extern BYTE DSP_DIALING[];
extern BYTE DSP_INSERT_CARD[];
extern BYTE DSP_REMOVE_CARD[];
extern BYTE DSP_TXN_FAILED[];    //added by Allen 2016-08-25
extern BYTE DSP_WARNING[];
extern BYTE DSP_DIALPROBLEM[];
extern BYTE DSP_NOCONNECTION[];
extern BYTE DSP_NO_ENQ[];
extern BYTE DSP_BUSY[];
extern BYTE DSP_NO_ANSWER[];
extern BYTE DSP_PRODUCT_UNIT[];
extern BYTE DSP_SCHEME_UNIT[];
extern BYTE DSP_SCHEME_TYPE[];
extern BYTE DSP_SLIP_NUMBER[];
extern BYTE DSP_TRANSACTIONS[];
extern BYTE DSP_TRANSACTION[];
extern BYTE DSP_DUPLICATETRAN[];
extern BYTE DSP_ENDOFDAY[];
extern BYTE DSP_PARAMETER[];
extern BYTE DSP_PRINTEODTICKET[];
extern BYTE DSP_LISTS[];
extern BYTE DSP_SECURITY[];

//extern BYTE DSP_BALANCELIST[];
extern BYTE DSP_CHECK_BALANCE[];

//added by Allen for ZAIN 2016-05-12
extern BYTE DSP_POINTS_REWARD_SALE[];
extern BYTE DSP_REDEEM[];
extern BYTE DSP_VOID_REVERSAL[];
//add end


extern BYTE DSP_LASTTRANS[];

//extern BYTE DSP_INSTALLMENT[];
extern BYTE DSP_WITHDRAW_DEBIT[];//add by lqc 20150619

extern BYTE DSP_LOYALTY[];

//extern BYTE DSP_PAYMENT[];
extern BYTE DSP_DEPOSIT_CREDIT[];//add by lqc 20150617

extern BYTE DSP_ACCOUNT_MAINT[];
extern BYTE DSP_PRODUCT_TYPE[];
extern BYTE DSP_K_CANCEL[];
extern BYTE DSP_K_CAN[];
extern BYTE DSP_K_OK[];
extern BYTE DSP_K_DEL[];
extern BYTE DSP_K_UP[];
extern BYTE DSP_K_DOWN[];
extern BYTE DSP_K_CANCEL_KEY[];
extern BYTE DSP_K_PRINT_KEY[];
extern BYTE DSP_K_NO[];
extern BYTE DSP_K_YES[];

extern BYTE DSP_TERMINAL_NOT_ALLOWED[];
extern BYTE DSP_MEMORY_FULL[];
extern BYTE DSP_ACTIF[];
extern BYTE DSP_PLEASE_PUT_TERM[];
extern BYTE DSP_IN_MAIN_UNIT[];
extern BYTE DSP_PRINTER_ERROR[];
extern BYTE DSP_SAME_TRANSACTION[];
extern BYTE DSP_NO_PAPER[];
extern BYTE DSP_NO_TRANSACTION[];
extern BYTE DSP_SECOND_COPY_PRINT[];
extern BYTE DSP_THIRD_COPY_PRINT[];
extern BYTE DSP_PRINTING[];
extern BYTE DSP_PHONE_1[];
extern BYTE DSP_PHONE_2[];
extern BYTE DSP_PHONE_3[];
extern BYTE DSP_HOST_PHONE1[];
extern BYTE DSP_HOST_PHONE2[];
extern BYTE DSP_HOST_PHONE3[];
extern BYTE DSP_TR_FILE_FULL[];
extern BYTE DSP_PARAM_REQUEST[];
extern BYTE DSP_PARAM_DLOADING[];
extern BYTE DSP_OTHER_PARAMS_DLOADING[];
extern BYTE DSP_ENTER_PIN[];
extern BYTE DSP_BATCH_CLOSING[];

extern BYTE DSP_AVAIL_REPAYMENT_TENURES_LOADING[];
extern BYTE DSP_SCHEME_LIST_LOADING[];
extern BYTE DSP_PRODUCT_LIST_LOADING[];
extern BYTE DSP_BLACK_CARD_LOADING[];
extern BYTE DSP_CARD_TYPE_LOADING[];
extern BYTE DSP_BIN_LOADING[];
extern BYTE DSP_MERCHANT_TYPES_LOADING[];
extern BYTE DSP_PARAMETER_OK[];
extern BYTE DSP_AMOUNT_MIN_LIMIT[];
extern BYTE DSP_SENDING[];
extern BYTE DSP_RECEIVING[];
extern BYTE DSP_CARD_BIN_NOT_FOUND[];
extern BYTE DSP_CARD_TYPE_NOT_FOUND[];
extern BYTE DSP_INST_COUNTER[];
extern BYTE DSP_PAN[];
extern BYTE DSP_CVC[];
extern BYTE DSP_REVERSAL_SENDING[];
extern BYTE DSP_FILE_EMPTY[];
extern BYTE DSP_PIN_TITLE[];
extern BYTE DSP_PIN_PROMPT[];

//extern BYTE DSP_TENURE_CALC[];
extern BYTE DSP_END_OF_SHIFT[];

extern BYTE DSP_AMOUNT_MIN_LIMIT_ERR[];

/////////////////////////////////////////////////////////////////////////////
//ARABIC ////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE PDS_ARABIC_MERCHANT_ID[];
extern BYTE PDS_ARABIC_STORE_NO[];
extern BYTE PDS_ARABIC_TERMINAL_ID[];
extern BYTE PDS_ARABIC_AUTH_CODE[];

extern BYTE PRT_ARABIC_INSTALLMENT[];
extern BYTE PRT_ARABIC_LOYALTY[];
extern BYTE PRT_ARABIC_PAYMENT[];
extern BYTE PRT_ARABIC_ACCOUNT_MAINT[];
extern BYTE PRT_ARABIC_SIGN[];
extern BYTE PRT_ARABIC_GOODS_RECEIVED[];
extern BYTE PRT_ARABIC_KEEPTHISPAPER[];
extern BYTE PRT_ARABIC_BATCH[];
extern BYTE PRT_ARABIC_SLIPNO[];
extern BYTE PRT_ARABIC_TOTAL[];


extern BYTE PRT_ARABIC_DESCRIPTION_OF_GOODS[];
extern BYTE PRT_ARABIC_CARD_NUMBER[];

extern BYTE PRT_ARABIC_SLIPHEADERINST[];
extern BYTE PRT_ARABIC_SLIPHEADERLOYALTY[];
extern BYTE PRT_ARABIC_SLIPHEADERPAYMENT[];
extern BYTE PRT_ARABIC_FLAIRINSTALLMENT[];
extern BYTE PRT_ARABIC_FLAIRLOYALTY[];
extern BYTE PRT_ARABIC_FLAIRPAYMENT[];
extern BYTE PRT_ARABIC_FLAIRACCOUNT_MAINT[];
extern BYTE PRT_ARABIC_INST[];
extern BYTE PRT_ARABIC_TENURE[];
extern BYTE PRT_ARABIC_PURCHASE_AMOUNT[];  
extern BYTE PRT_ARABIC_PAYMENT_AMOUNT[];   
extern BYTE PRT_ARABIC_GOODS_RECEIVED_LOYALTY[];
extern BYTE PRT_ARABIC_GOODS_RECEIVED_PAYMENT[];
extern BYTE PRT_ARABIC_GOODS_RECEIVED_FOOTER[];

/////////////////////////////////////////////////////////////////////////////
//WARNING////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE DSP_W_INSUFFICIENT_BALANCE[];
extern BYTE DSP_W_UNAVAILABLE_MERCHANT_ID[];
extern BYTE DSP_W_CARD_BLACK_LIST[];
extern BYTE DSP_W_EXPIRY_DATE[];
extern BYTE DSP_W_CURRENCY_CODE[];
extern BYTE DSP_W_0_9[];
extern BYTE DSP_W_Entered_Amount_Maximum[];
/////////////////////////////////////////////////////////////////////////////
//DISPLAY & PRINT MESSAGES///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern BYTE PDS_MERCHANT_ID[];
extern BYTE PDS_STORE_NO[];
extern BYTE PDS_TERMINAL_ID[];
extern BYTE PDS_BALANCE[];
extern BYTE PDS_AUTH_CODE[];
extern BYTE PDS_TMS_NII[];
extern BYTE PDS_PMS_NII[];
extern BYTE PDS_NETSERVER_NII[];
extern BYTE PDS_BATCH_ID[];
extern BYTE DSP_APN[];
extern BYTE DSP_HOST_IP[];
extern BYTE DSP_HOST_PORT[];



#endif

