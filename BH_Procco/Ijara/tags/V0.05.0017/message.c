#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.


#include "defines.h"
#include "struct.h"
#include "function.h"
#include "message.h"



const BYTE *AppMsgTable[] =
{
    "025",
    "MERCHANT CB ?",           	// M_MERCH1
    "CARD NOT",                	// M_CARDNOT
    "MERCHANT CB",             	// M_MERCH2
    "TEST CART",						// M_TESTCARD
    "BANK CART",						// M_BANKCARD
    "VISA CART",						// M_VISACARD
    "EURO CART",						// M_EUROCARD
    "AUTO No ",							// M_AUTHNUM
    "DEBIT",								// M_DEBIT
    "CREDIT",							// M_CREDIT
    "CANCELATION",						// M_CANCEL1
    "CANCEL",								// M_CANCEL2
    " OS_NO  ",								// M_YES
    " OS_YES",								// M_NO
    "OPTIONS",							// M_OPTION
    "MERCH CARD",						// M_MERCHCARD
    "SUCCESS",							// M_SUCCESS
    "ERROR",								// M_WARNING
    "ABORTED",							// M_INTERROMPU
    "CONNECTION FAILED",				// M_ECHECCONNEX
    "PB CONNECTION",					// M_PBCONNEX
    "FORCE",								// M_FORCAGE
    "TR UPLOAD",						// M_SETTLECALL
    "DOWNLOAD",							// M_SWDOWNLOAD
    "TREATMENT"							// M_TREATMENT
};

/*MENU*/const BYTE *gListeMenu[] =
{
    "LIST FUNCT.",
    "DETAIL LIST",
    "TOTALS",
    "PARMTR.LIST"
};

/*MENU*/const BYTE *gTermMenu[] =
{
    " TERMINAL",
    "GPRS",
    "Dial Up",
    "RS-232 "
};

/*MENU*/const BYTE *gLanguageMenu[] =
{
    " SELECT LANGUAGE",
    "English",
    "Arabic"
};

/*MENU*/const BYTE *gManagmtMenu[] =
{
    "SECURITY FNCT",
    "HOST INFO",
    "PORT TYPE",
    "HOST PHONES",
    "DEBUG",
    "DEBUG TYPE",
    "GET BATCH ID",
    "GET NII",
    "DELETE BATCH"
};
/*MENU*/const BYTE *gDebugMenu[] =
{
    " DEBUG",
    "OPEN",
    "CLOSE"
};

/*MENU*/const BYTE *gDebugMenuType[] =
{
    "DEBUG TYPE",
    "COM",
    "PRINTER",
    "COM AND PRINTER"
};

/*MENU*/const BYTE *gSecurityFnctMenu[] =
{
	  "SECURITY FNCT",
	  "HOST INFO",
	  "GET BATCH ID",
	  "DELETE BATCH",
	  "DEBUG"
};

//by jarod 2011.06.01
const int gHostErrMsgTableFlag[20] ={4,5,41,43,51,55,57,75,76,96};

const BYTE gHostErrMsgForDisp[15][100] =
{
 	"PICK UP",
	"Do not Honour",
	"Pick up, lost card",
	"Pick up, stolen card",
	"Not sufficient funds",
	"Incorrect PIN",
	"   Transaction not       permitted to      cardholder ",
	" Allowable number of PIN tries exceeded",
	"      Wrong PIN          number of PIN    tries exceeded",
	"System Malfunction"
};
const BYTE gHostErrMsgForPRT[15][100] =
{
	"        PICK UP         ",
	"     Do not Honour      ",
	"   Pick up, lost card   ",
	"  Pick up, stolen card  ",
	"  Not sufficient funds  ",
	"     Incorrect PIN      ",
	"    Transaction not      permitted to cardholder",
	" Allowable number of PIN      tries exceeded     ",
	"Wrong PIN number of PIN      tries exceeded     ",
	"   System Malfunction   "
};
/////////////////////////////////////////////////////////////////////////////
//UTILITY SIGNS////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BYTE UTIL_LINE[] 				                ="------------------------";
BYTE UTIL_SPACES[] 				         	 ="                        ";
BYTE UTIL_EQUALS[] 				         	 ="========================";
BYTE UTIL_STARS[] 				         	 ="************************";
BYTE UTIL_1800[] 				            	 ="1800";
BYTE UTIL_FILLZEROS[]			         	 ="00000000000000000000";


/////////////////////////////////////////////////////////////////////////////
//DEBUG MESSAGES///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BYTE DEB_READ_BUFFER[] 		           	 ="READ BUFFER";
BYTE DEB_MAC_BUFFER[] 		           	 ="MAC BUFFER";
BYTE DEB_MAC[] 		                  	 ="MAC";
BYTE DEB_SCR_BUFFER[] 		           	 ="SCR BUFFER";
BYTE DEB_COEFFICIENTS[]	               	 ="COEFFICIENTS";
BYTE DEB_PAN_BUFFER[]	               	 ="PAN BUFFER";
BYTE DEB_INV_PAN_BUFFER[]	           	 ="INV PAN BUFFER";
BYTE DEB_ARQC[]	                     	 ="ARQC";
BYTE DEB_ARC_ARQC[]	                  	 ="ARC+ARQC";
BYTE DEB_CARD_CR_KEY1_0[]	           	 ="CARD CR KEY1-0";
BYTE DEB_CARD_CR_KEY2_0[]	           	 ="CARD CR KEY2-0";
BYTE DEB_CARD_CR_KEY1_1[]	           	 ="CARD CR KEY1-1";
BYTE DEB_CARD_CR_KEY2_1[]	           	 ="CARD CR KEY2-1";
BYTE DEB_CR_SESSION_KEY_L[]	         	 ="CR SESSION KEY L";
BYTE DEB_CR_SESSION_KEY_R[]	         	 ="CR SESSION KEY R";
BYTE DEB_CARD_SC_KEY1[]	               	 ="CARD SC KEY1";
BYTE DEB_CARD_SC_KEY2[]	               	 ="CARD SC KEY2";
BYTE DEB_SC_SESSION_KEY_L[]	         	 ="SC SESSION KEY L";
BYTE DEB_SC_SESSION_KEY_R[]	         	 ="SC SESSION KEY R";
BYTE DEB_ENCRYPTED_PIN[]	           	 ="ENCRYPTED PIN";
BYTE DEB_CARD_MAC_KEY1[]	           	 ="CARD MAC KEY1";
BYTE DEB_CARD_MAC_KEY2[]	           	 ="CARD MAC KEY2";
BYTE DEB_MAC_SESSION_KEY_L[]	       	 ="MAC SESSION KEY L";
BYTE DEB_MAC_SESSION_KEY_R[]	       	 ="MAC SESSION KEY R";
BYTE DEB_PAN[]	                       	 ="PAN";
BYTE DEB_EXPIRY_DATE[]	               	 ="EXPIRY DATE";
BYTE DEB_SW_BAL[]                      	 ="SW BAL";
BYTE DEB_BALANCE[]                     	 ="BALANCE";
BYTE DEB_SW_UPPER_LIM[]                	 ="SW UPPER LIM";
BYTE DEB_UPPER_LIMIT[]                 	 ="UPPER LIMIT";
BYTE DEB_SW_PARAMETER[]                	 ="SW PARAMETER";
BYTE DEB_PARAMETERS[]                  	 ="PARAMETERS ";
BYTE DEB_SCRIPT_RESULTS_72[]           	 ="SCRIPT RESULTS-72";
BYTE DEB_UNPREDICTABLE_NUMBER[]        	 ="UNPREDICTABLE NUMBER";
BYTE DEB_CAPK_INDEX[]                  	 ="CAPK Index";
BYTE DEB_CVM_RESULTS[]                 	 ="CVM RESULTS";
BYTE DEB_AIP[]                         	 ="AIP";
BYTE DEB_CID_1[]                       	 ="CID 1";
BYTE DEB_ISSUER_APP_DATA_1[]           	 ="Issuer App. Data 1";
BYTE DEB_TVR_1[]                       	 ="TVR 1";
BYTE DEB_TSI_1[]                       	 ="TSI 1";
BYTE DEB_PAN_SEQUENCE_NUMBER[]         	 ="Pan Seq. Number";
BYTE DEB_PINTRYCOUNT[]                 	 ="PinTryCount";
BYTE DEB_ISS_AUTH_DATA[]               	 ="ISS AUTH DATA";
BYTE DEB_NO_ONLINE_CAPABILITY[]        	 ="No-Online Capability\n";
BYTE DEB_AIP_FINAL[]                   	 ="AIP Final";
BYTE DEB_CID_FINAL[]                   	 ="CID Final";
BYTE DEB_ISSUER_APP_DATA_FINAL[]       	 ="Issuer App. Data Final";
BYTE DEB_TVR_FINAL[]                   	 ="TVR Final";
BYTE DEB_TSI_FINAL[]                   	 ="TSI Final";
BYTE DEB_DATA_AUTHENTICATION[]         	 ="Data Authentication (-)\n";
BYTE DEB_SDA_SUCCESS[]                 	 ="SDA SUCCESS\n";
BYTE DEB_SDA_ERROR[]                   	 ="SDA ERROR\n";
BYTE DEB_CARD_REFUSED[]                	 ="CARD REFUSED!!!\n";
BYTE DEB_SDA_FAIL[]                    	 ="SDA FAIL\n";
BYTE DEB_SDA_FAIL_CAPK[]               	 ="SDA FAIL (IN CAPK ERR)\n";
BYTE DEB_SDA_STAT_UNKNOW[]             	 ="SDA STAT UNKNOWN\n";
BYTE DEB_DDA_SUCCESS[]                 	 ="DDA SUCCESS\n";
BYTE DEB_DDA_ERROR_CARD[]              	 ="DDA ERROR(CARD COMM)\n";
BYTE DEB_DDA_ERROR[]                   	 ="DDA ERROR\n";
BYTE DEB_DDA_FAIL[]                    	 ="DDA FAIL\n";
BYTE DEB_DDA_FAIL_CAPK[]               	 ="DDA FAIL (IN CAPK ERR)\n";
BYTE DEB_DDA_STAT_UNKNOWN[]            	 ="DDA STAT UNKNOWN\n";
BYTE DEB_CDA_SUCCESS[]                 	 ="CDA SUCCESS\n";
BYTE DEB_CDA_ERROR_CARD[]              	 ="CDA ERROR(CARD COMM)\n";
BYTE DEB_CDA_ERROR[]                   	 ="CDA ERROR\n";
BYTE DEB_CDA_FAIL[]                    	 ="CDA FAIL\n";
BYTE DEB_CDA_FAIL_CAPK[]               	 ="CDA FAIL (IN CAPK ERR)\n";
BYTE DEB_CDA_STAT_UNKNOWN[]            	 ="CDA STAT UNKNOWN\n";
BYTE DEB_VERIFY_CARDHOLDER_OK[]        	 ="Verify Cardholder OK\n";
BYTE DEB_VERIFY_CARDHOLDER_NOK[]       	 ="Verify Cardhol. NOK (CE)";
BYTE DEB_VERIFY_CARDHOLDER_ERROR[]     	 ="Verify Cardholder ERROR\n";
BYTE DEB_VERIFY_CARDHOLDER_FAIL[]      	 ="Verify Cardholder FAIL\n";
BYTE DEB_VERIFY_CARDHOLDER_NOTRQ[]     	 ="Verify Cardholder NOTRQ\n";
BYTE DEB_VERIFY_CARDHOLDER_[]          	 ="Verify Cardholder ???\n";
BYTE DEB_WARNING_PIN_BLOCKED[]         	 ="WARNING! PIN BLOCKED\n";
BYTE DEB_SIGNATURE_IS_REQUIRED[]       	 ="Signature is required\n";
BYTE DEB_CAA_OK[]                      	 ="CAA OK\n";
BYTE DEB_CAA_COM_ERROR[]               	 ="CAA - COM ERROR\n";
BYTE DEB_CAA_ERROR[]                   	 ="CAA ERROR\n";
BYTE DEB_CAA_BLOCKED[]                 	 ="CAA BLOCKED\n";
BYTE DEB_CAA_NOT_COHERENT[]            	 ="CAA NOT COHERENT\n";
BYTE DEB_CAA_ERROR_WRD[]               	 ="CAA ERROR ( WRD )\n";
BYTE DEB_CAA_CDA_FAILED[]              	 ="CAA CDA FAILED\n";
BYTE DEB_CAA_[]                        	 ="CAA ???\n";
BYTE DEB_CAA_DECISION[]                	 ="CARD DECISION:";
BYTE DEB_ACCEPT[]                      	 ="ACCEPT\n";
BYTE DEB_GO_ONLINE[]                   	 ="GO ONLINE\n";
BYTE DEB_REFERRAL[]                    	 ="REFERRAL\n";
BYTE DEB_REFUSE[]                      	 ="REFUSE\n";
BYTE DEB_AC_1[]                        	 ="AC 1:";
BYTE DEB_AC_2[]                        	 ="AC 2:";
BYTE DEB_ATC[]                         	 ="ATC";
BYTE DEB_DATES_CHECKING_OK[]           	 ="Dates Checking       OK\n";
BYTE DEB_APPLI_IS_EFFECTIVE_YES[]      	 ="Appli is effective  OS_YES\n";
BYTE DEB_APPLI_HAS_EXPIRED_NO[]        	 ="Appli has expired    OS_NO\n";
BYTE DEB_DATES_CHECKING[]              	 ="Dates Checking       OK\n";
BYTE DEB_APPLI_IS_EFFECTIVE_NO[]       	 ="Appli is effective   OS_NO\n";
BYTE DEB_APPLI_HAS_EXPIRED_[]          	 ="Appli has expired   (-)\n";
BYTE DEB_APPLI_HAS_EXPIRED_YES[]       	 ="Appli has expired   OS_YES\n";
BYTE DEB_DATES_CHECKING_ERROR[]        	 ="Dates Checking    ERROR\n";
BYTE DEB_DATES_CHECKING_[]             	 ="Dates Checking ???\n";
BYTE DEB_RANDOM_SEL_IN_PRM_ERR[]       	 =" RANDOM SEL. IN PRM ERR \n";
BYTE DEB_RANDOM_SEL_OK[]               	 ="     RANDOM SEL. OK     \n";
BYTE DEB_RANDOM_SEL_NOK[]              	 ="     RANDOM SEL. NOK    \n";
BYTE DEB_RANDOM_SEL_NOT_REQUIRED[]     	 ="RANDOM SEL. NOT REQUIRED\n";
BYTE DEB_RANDOM_SEL_[]                 	 ="    RANDOM SEL. ???     \n";
BYTE DEB_READ_DATA_OK[]                	 ="      READ DATA OK      ";
BYTE DEB_READ_DATA_CARD_ERROR[]        	 ="  READ DATA CARD ERROR  ";
BYTE DEB_READ_DATA_ERROR[]             	 ="     READ DATA ERROR    ";
BYTE DEB_CARD_REFUSED_[]               	 ="     CARD REFUSED!!!    ";
BYTE DEB_READ_DATA_FILE_ERROR[]        	 ="  READ DATA FILE ERROR  ";
BYTE DEB_READ_DATA_[]                  	 ="      READ DATA ???     ";
BYTE DEB_TAA_OK[]                      	 ="TAA OK\n";
BYTE DEB_TAA_NOK[]                     	 ="TAA NOK\n";
BYTE DEB_TAA_[]                        	 ="TAA ???\n";
BYTE DEB_TERM_DECISION[]               	 ="TERM DECISION:";
BYTE DEB_USAGE_CONTROL_OK[]            	 ="USAGE CONTROL  OK\n";
BYTE DEB_USAGE_CONT_RESTRICTED[]       	 ="USAGE CONT.-RESTRICTED\n";
BYTE DEB_USAGE_CONTROL_[]              	 ="USAGE CONTROL ???\n";
BYTE DEB_COM_ERROR[]                   	 ="\n-------COM ERROR-------\n";
BYTE DEB_VELOCITY_CHECKING_FAIL[]      	 ="Velocity checking  FAIL\n";
BYTE DEB_VELOCITY_CHECKING_OK[]        	 ="Velocity checking    OK\n";
BYTE DEB_VELOCITY_CHECKING_MINUS[]     	 ="Velocity checking      \n";
BYTE DEB_VELOCITY_CHECKING[]           	 ="Velocity checking   (-)\n";
BYTE DEB_VELOCITY_CHECKING_[]          	 ="Velocity Checking ???\n";
BYTE DEB_EXCEED_LOWER_LIMIT_NO[]       	 ="Exceeds Lower Limit  OS_NO\n";
BYTE DEB_EXCEED_LOWER_LIMIT_YES[]      	 ="Exceeds Lower Limit OS_YES\n";
BYTE DEB_EXCEED_UPPER_LIMIT_NO[]       	 ="Exceeds Upper Limit  OS_NO\n";
BYTE DEB_EXCEED_UPPER_LIMIT_YES[]      	 ="Exceeds Upper Limit OS_YES\n";
BYTE DEB_VERSION_SUPPORTED[]           	 ="Version Supported\n";
BYTE DEB_VERSION_NOT_SUPPORTED[]       	 ="Version Not Supported\n";
BYTE DEB_VERSION_CHECK[]               	 ="Version Check ???\n";
BYTE DEB_CARD_HOLDER_NAME[]            	 ="CARD HOLDER NAME";
BYTE DEB_TRACK_I[]                     	 ="TRACK I";
BYTE DEB_TRACK_II[]                    	 ="TRACK II";
BYTE DEB_CRM_COUNTRY_SW[]              	 ="CRM COUNTRY SW";
BYTE DEB_CRM_COUNTRY[]                 	 ="CRM COUNTRY";
BYTE DEB_CRM_CURRENCY_SW[]             	 ="CRM CURRENCY SW";
BYTE DEB_CRM_CURRENCY[]                	 ="CRM CURRENCY";
BYTE DEB_PACKET_RECEIVED[]               ="\nPACKET RECEIVED         ";
BYTE DEB_FRAME_ERROR[]                   ="\nFRAME ERROR\n";
BYTE DEB_FIELD[]                         ="\nField :";
BYTE DEB_FIELD_DATA[]                    ="\nField Data\n";
BYTE DEB_PACKET_SEND[]                   ="\nPACKET SENT             ";
BYTE DEB_TYPE_ERROR[]                    ="\nTYPE ERROR";
BYTE DEB_SIZE_ERROR[]                    ="\nSIZE ERROR";
BYTE DEB_DATA_SIZE_OVER[]                ="\nDATA SIZE OVER\n";

BYTE DEB_REVERSAL_OPEN_FAILG[]           ="\nREVERSAL OPEN FAIL-G\n";
BYTE DEB_REVERSAL_NOT_PRESENT[]          ="\nREVERSAL NOT PRESENT\n";
BYTE DEB_REVERSAL_CREATE_FAIL[]          ="\nREVERSAL CREATE FAIL\n";
BYTE DEB_REVERSAL_PRESENT[]              ="\nREVERSAL PRESENT\n";
BYTE DEB_REVERSAL_READ_FAIL[]            ="\nREVERSAL READ FAIL\n";
BYTE DEB_REVERSAL_OPEN_FAILA[]           ="\nREVERSAL OPEN FAIL-A\n";
BYTE DEB_REVERSAL_RECORDING_FAIL[]       ="\nREVERSAL RECORDING FAIL\n";
/////////////////////////////////////////////////////////////////////////////
//MESSAGE AND TRANSACTION TYPES////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BYTE TYPE_01[] 				          	 ="01";
BYTE TYPE_03[] 				          	 ="03";

BYTE TYPE_70[] 				          	 ="70";//Installment Sale
BYTE TYPE_71[] 				          	 ="71";//Loyalty
BYTE TYPE_72[] 				          	 ="72";//Payment
BYTE TYPE_73[] 				          	 ="73";//Account Maint
BYTE TYPE_74[] 				          	 ="74";//get balance
BYTE TYPE_75[]						 ="75";

/////////////////////////////////////////////////////////////////////////////
//FILE NAMES///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BYTE FILE_STATE[LG_FILENAME+1]		   	 = "STAT"; //name of the module state file
BYTE FILE_MP[LG_FILENAME+1]			   	 = "MPMP"; //name of the merchant parameter file
BYTE FILE_GP[LG_FILENAME+1]			   	 = "GPGP"; //name of the general parameter file
BYTE FILE_DP[LG_FILENAME+1]			   	 = "DPDP"; //name of the download parameter file
BYTE FILE_TR[LG_FILENAME+1]			   	 = "TRAN"; //name of the TRansaction file
BYTE FILE_LAST_EOD[LG_FILENAME+1]	 = "EOD"; 
BYTE FILE_RB[LG_FILENAME+1]			   	 = "RBRB"; //name of the Batch Reprint file
BYTE FILE_R1[LG_FILENAME+1]			   	 = "R1R1"; //name of the transaction ticket 1 file
BYTE FILE_R2[LG_FILENAME+1]			   	 = "R2R2"; //name of the transaction ticket 2 file
BYTE FILE_R3[LG_FILENAME+1]				 = "R3R3"; //name of the transaction ticket 3 file

BYTE FILE_RP[LG_FILENAME+1] 		     = "RPRP"; // name of temporary file for PDload
BYTE FILE_BC[LG_FILENAME+1]			   	 = "BCBC"; //Black Card File
BYTE FILE_BT[LG_FILENAME+1]			   	 = "BTBT"; //Black Card Temporary File

BYTE FILE_PC[LG_FILENAME+1]			   	 = "PCPC"; //Product File
BYTE FILE_PT[LG_FILENAME+1]			   	 = "PTPT"; //Product Temporary File

BYTE FILE_ST[LG_FILENAME+1]			   	 = "STST"; //Scheme Type
BYTE FILE_TS[LG_FILENAME+1]			   	 = "TSTS"; //Scheme Type Temporary File

BYTE FILE_PE[LG_FILENAME+1]			   	 = "RPTE"; //Repayment Tenures
BYTE FILE_TP[LG_FILENAME+1]			   	 = "TRPT"; //Repayment Tenures Temporary File

BYTE FILE_BIN[LG_FILENAME+1]		     = "BINF"; //Bin Listesi
BYTE FILE_TEMPBIN[LG_FILENAME+1]	     = "BINT"; //Bin Listesi Temp

BYTE FILE_MERCH[LG_FILENAME+1]		   	 = "MERC"; //Merchant Id
BYTE FILE_TEMPMERCH[LG_FILENAME+1]	   	 = "TMER"; //Merchant Id Temp

BYTE FILE_REVERSE[LG_FILENAME+1]       	 = "RVRS"; // Reverse File


BYTE csFILE_EMV_KEYS[LG_FILENAME+1]	     = "KEYS"; //
BYTE csFILE_EMV_TEMP_KEYS[LG_FILENAME+1] = "TKEY"; //

BYTE FILE_GPRS_PAR[LG_FILENAME+1] = "GPRS"; //

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//PRINT MESSAGES///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BYTE PRT_DESCRIPTION_OF_GOODS[]        	 ="  DESCRIPTION OF GOODS  ";
BYTE PRT_EMPTY[] 		                 ="          EMPTY         ";
BYTE PRT_SALE[]	                     	 ="SALE       : ";
BYTE PRT_SCHEME_TYPE[]	               	 ="      SCHEME TYPE       ";
BYTE PRT_REPRINT[] 			             ="SECOND COPY ";

BYTE PRT_KEEPTHISPAPER[]	             ="THANK YOU FOR USING YOUR"
"FLAIR CARD. THIS IS YOUR"
"CONTRACT. PLEASE KEEP IT"
"   WITH YOU FOR FUTURE  "
"        REFERENCE.      ";

BYTE PRT_SIGN[]			               	 ="SIGNATURE:..............";
BYTE PRT_GOODS_RECEIVED[]				 =" I ACCEPT THIS CONTRACT                         ";

BYTE PRT_GOODS_RECEIVED_LOYALTY[]		 ="   PLEASE RETAIN THIS   "
" RECEIPT AS CONFIRMATION"
"    OF YOUR REWARDS.    ";
BYTE PRT_GOODS_RECEIVED_PAYMENT[]		 ="   PLEASE RETAIN THIS   "
" RECEIPT AS CONFIRMATION"
"    OF YOUR PAYMENT.    ";
BYTE PRT_GOODS_RECEIVED_FOOTER[]		 ="*FLAIR CARDHOLDER TERMS "
"  AND CONDITIONS APPLY. ";

BYTE PRT_BATYPE[] 			             ="   BATCH ";
BYTE PRT_FAILED[]			             ="  ERROR     \x12";
BYTE PRT_CARD_NUMBER[]                 	 ="CARD NO:";
BYTE PRT_EXPIRY_DATE[] 			         ="EXPIRY DATE      :";
BYTE PRT_BALANCE_LIST_MSG[] 			 ="    BALANCE ENQUIRY     ";
BYTE PRT_PDLD_PARAMDLOAD[]		         ="   PARAMETER DOWNLOAD   ";
BYTE PRT_PURCHASE_AMOUNT[]			     ="Purchase";
BYTE PRT_PAYMENT_AMOUNT[]			     ="Payment:";
BYTE PRT_PARMVERSION[]	               	 ="Parameter Vers. :";
BYTE PRT_DIALMODE[]		               	 ="Phone Dial Type :";
BYTE PRT_MAXTRANS[]		               	 ="Maximum Transac.:";	
BYTE PRT_INSTALLMENT[]			         ="     INSTALMENT SALE    ";
BYTE PRT_LOYALTY[]			             ="        REWARDS         ";
BYTE PRT_PAYMENT[]			             ="        PAYMENT         ";
BYTE PRT_ACCOUNT_MAINT[]			     ="   ACCOUNT MAINTENANCE  ";

BYTE PRT_PARAMLIST[]		             ="     PARAMETER  LIST    ";
BYTE PRT_NO[] 						     ="OS_NO:";
BYTE PRT_TOTAL[] 			             ="Total  :";
BYTE PRT_TOTYPE[] 			             ="         TOTALS         ";
BYTE PRT_LOTYPE1[] 			             =" DETAIL TRANSACTION LIST";

BYTE PRT_LOSMARTDEBIT[] 			     ="INST";
BYTE PRT_LOSMARTLOYALTY[] 			     ="LYLT";
BYTE PRT_LOSMARTPAYMENT[] 			     ="PYMT";
//BYTE PRT_LOSMARTACCOUNT_MAINT[] 		 ="AMNT";

BYTE PRT_BATCHSTART[]		             ="End Of Day Start :";
BYTE PRT_PARMSTART[] 		             ="Parm.Down. Start :";
BYTE PRT_MINIMUM_LIMIT[] 		         ="MINIMUM LIM.";
BYTE PRT_END_SUCCESS[]				     ="  SUCCESSFUL";
BYTE PRT_BATCH[]                	     ="BATCH OS_NO   :";
BYTE PRT_SLIPNO[]                	     ="SLIP OS_NO    :";
BYTE PRT_AUTH_PHONE_1[] 		         ="NET SERV1:";
BYTE PRT_AUTH_PHONE_2[] 		         ="NET SERV2:";
BYTE PRT_AUTH_PHONE_3[] 		         ="NET SERV3:";
BYTE PRT_PMS_PHONE_1[] 		         	 ="PMS-1    :";
BYTE PRT_PMS_PHONE_2[] 		         	 ="PMS-2    :";
BYTE PRT_PMS_PHONE_3[] 		         	 ="PMS-3    :";
BYTE PRT_PARAM_PHONE_1[] 		         ="TMS-1    :";
BYTE PRT_PARAM_PHONE_2[] 		         ="TMS-2    :";
BYTE PRT_PARAM_PHONE_3[] 		         ="TMS-3    :";
BYTE PRT_SALE_UNIT[] 		             ="INSTALMENT UNIT     :";
BYTE PRT_LOYALTY_UNIT[] 		         ="LOYALTY UNIT        :";
BYTE PRT_PAYMENT_UNIT[] 		         ="PAYMENT UNIT        :";
//BYTE PRT_ACCOUNT_MAINT_UNIT[] 		 ="ACCOUNT MAINT. UNIT :";

BYTE PRT_FINISH_BALANCE[] 		         ="FIN.BL.:";
BYTE PRT_START_BALANCE[] 		         ="STR.BL.:";
BYTE PRT_REJECT[] 		               	 ="REJ.";
BYTE PRT_TERMINAL_SERIAL[]               ="     TERMINAL SERIAL    ";
BYTE PRT_NUMBER_WRONG[]                  ="      NUMBER WRONG      ";

BYTE PRT_SLIPHEADERINST[]            	 ="  This Flair receiving  "
"   agent has purchased  "
"  items detailed on the "
"associated cash register"
" listing and sold these "
" to you under the terms "
" of the transaction, the"
"conditions of Flair Card"
" issuance to you and on "
"  the following basis.  ";
BYTE PRT_SLIPHEADERLOYALTY[]          	 ="   We will credit your  "
" Flair Rewards *Account "
"with points to the value"
" of your purchase below.";
BYTE PRT_SLIPHEADERPAYMENT[]          	 ="  We have accepted the  "
"   following amount as  "
"  payment towards your  "
"  Flair card *Account.  ";

BYTE PRT_FLAIRINSTALLMENT[]			     ="  Flair Instalment Sale ";
BYTE PRT_FLAIRLOYALTY[]			         ="  Flair Rewards Receipt ";
BYTE PRT_FLAIRPAYMENT[]			         ="  Flair Payment Receipt ";
BYTE PRT_FLAIRACCOUNT_MAINT[]			 ="   Flair Acount Maint.  ";
BYTE PRT_INST[]			                 ="Amount :";
BYTE PRT_TENURE[]                        ="Months :";

BYTE PRT_REJECTED[]                      ="Card is blocked";   
/////////////////////////////////////////////////////////////////////////////
//DISPLAY MESSAGES///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BYTE DSP_WITH_PROCCO_CARD[] 		     ="     PLEASE      USE FLAIR CARD ";
BYTE DSP_LUHN_ERROR[] 		             ="   LUHN CHECK       ERROR  !    ";
BYTE DSP_CALL_CENTER[] 		             ="   CALL CENTER  ";
BYTE DSP_INVALID_CARD[] 		       	 ="! INVALID CARD !";
BYTE DSP_REJECTED[] 		           	 =" Card is blocked please contact  the call centre";
BYTE DSP_SCRIPT_ERROR[] 		       	 =" SCRIPT ERROR   ";
BYTE DSP_PLEASE_WAIT[]                 	 ="   PLEASE WAIT  ";
BYTE DSP_BATCH_OPEN[]			      	 ="    YOU MUST     MAKE END OF DAY";
BYTE DSP_CONNECTED[]			       	 ="    CONNECTED   ";
BYTE DSP_DIALING[] 			           	 ="  .PHONE DIALING";
BYTE DSP_INSERT_CARD[]                 	 ="   INSERT CARD  ";
BYTE DSP_REMOVE_CARD[] 			       	 ="   REMOVE CARD  ";
BYTE DSP_WARNING[]			          	 ="WARNING";
BYTE DSP_DIALPROBLEM[]			      	 ="   UNCONNECTED      TELEPHONE   ";
BYTE DSP_NOCONNECTION[]		           	 ="COULDN'T CONNECT                ";
BYTE DSP_NO_ENQ[] 			           	 ="    CENTRAL      NOT RESPONDING ";
BYTE DSP_BUSY[]			               	 ="      BUSY      ";
BYTE DSP_NO_ANSWER[] 		           	 ="  CENTRAL PHONE   NOT ANSWERING ";
BYTE DSP_PRODUCT_UNIT[] 		       	 =" PRODUCT UNIT>20";
BYTE DSP_SCHEME_UNIT[] 		           	 =" SCHEME UNIT>20 ";
BYTE DSP_SCHEME_TYPE[] 		           	 ="SCHEME TYPE     ";
BYTE DSP_TRANSACTIONS[] 		       	 =" TRANSACTIONS";
BYTE DSP_TRANSACTION[]  		       	 =" MERCHANT       ";
BYTE DSP_DUPLICATETRAN[]               	 ="SECOND COPY  ";
BYTE DSP_ENDOFDAY[]                    	 ="END OF DAY   ";
BYTE DSP_PARAMETER[]                   	 ="PARAM.DOWNL. ";
BYTE DSP_PRINTEODTICKET[]              	 ="EOD 2.COPY   ";
BYTE DSP_LISTS[]                       	 ="LIST.FUNCT.  ";
BYTE DSP_SECURITY[]                      ="SECURITY FNCT";
BYTE DSP_BALANCELIST[]                 	 ="BALANCE LIST ";
BYTE DSP_LASTTRANS[]                   	 ="LAST 10 TRANS";
BYTE DSP_INSTALLMENT[]                 	 ="RETAIL TRANSACTION/*INSTALMENT  */ ";
BYTE DSP_LOYALTY[]                     	 ="LOYALTY      ";
BYTE DSP_PAYMENT[]                     	 ="PAYMENT      ";
BYTE DSP_ACCOUNT_MAINT[]               	 ="ACCOUNT MAINT";
BYTE DSP_PRODUCT_TYPE[] 		       	 ="PRODUCT TYPE    ";
BYTE DSP_TENURE_CALC[]				="TENURE CALCULATION";

BYTE DSP_K_CANCEL[] 			       	 ="CANCEL";
BYTE DSP_K_CAN[]                       	 ="CAN";
BYTE DSP_K_OK[] 			           	 =" OK";
BYTE DSP_K_DEL[] 			           	 =" DEL ";
BYTE DSP_K_UP[] 			           	 =" UP";
BYTE DSP_K_DOWN[] 			          	 ="DOWN";
BYTE DSP_K_CANCEL_KEY[]                	 ="  CANCEL";
BYTE DSP_K_PRINT_KEY[]                 	 =" PRINT ";
BYTE DSP_K_NO[] 			           	 =" NO ";
BYTE DSP_K_YES[] 			           	 =" YES";

BYTE DSP_TERMINAL_NOT_ALLOWED[]		     ="NOT ALLOWED THIS   TRANSACTION  ";
BYTE DSP_MEMORY_FULL[] 			         ="  MEMORY FULL,     PLEASE EOD   ";
BYTE DSP_ACTIF[]				         ="ACTIVE ?";

BYTE DSP_PLEASE_PUT_TERM[]             	 ="PLEASE PUT TERM.";
BYTE DSP_IN_MAIN_UNIT[]                	 ="  IN MAIN UNIT  ";
BYTE DSP_PRINTER_ERROR[] 		         =" PAPER NOT FOUND                ";
BYTE DSP_SAME_TRANSACTION[]            	 ="    SAME WITH   LAST TRANSACTION";
BYTE DSP_NO_PAPER[] 				     =" PAPER NOT FOUND                ";
BYTE DSP_NO_TRANSACTION[] 		         =" OS_NO TRANSACTION ";
BYTE DSP_SECOND_COPY_PRINT[] 			 ="CUSTOMER CPY PNT";
BYTE DSP_THIRD_COPY_PRINT[]			="MERCHANT CPY PNT";
BYTE DSP_PRINTING[] 				     ="    PRINTING    ";
BYTE DSP_PHONE_1[]                     	 ="AUTH.PHN.OS_NO 1:  ";
BYTE DSP_PHONE_2[]                     	 ="AUTH.PHN.OS_NO 2:  ";
BYTE DSP_PHONE_3[]                     	 ="AUTH.PHN.OS_NO 3:  ";
BYTE DSP_HOST_PHONE1[]			         ="PAR.PHONE 1 :   ";
BYTE DSP_HOST_PHONE2[]			         ="PAR.PHONE 2 :   ";
BYTE DSP_HOST_PHONE3[]			         ="PAR.PHONE 3 :   ";
BYTE DSP_TR_FILE_FULL[]   		         ="  BATCH FULL,      PLEASE EOD   ";
BYTE DSP_PARAM_REQUEST[]  			     ="  SEND REQUEST  ";
BYTE DSP_PARAM_DLOADING[] 			     ="     LOADING         PARAMETERS   ";
BYTE DSP_OTHER_PARAMS_DLOADING[]           ="     LOADING         PARAMETERS   ";
BYTE DSP_ENTER_PIN[]				       ="PIN ? ";
BYTE DSP_BATCH_CLOSING[] 		           ="   END OF DAY   ";

BYTE DSP_AVAIL_REPAYMENT_TENURES_LOADING[] ="   AVAIL.REPAYMENT      TENURES LOADING";
BYTE DSP_SCHEME_LIST_LOADING[]             ="   SCHEME LIST       LOADING    ";
BYTE DSP_PRODUCT_LIST_LOADING[]            ="  PRODUCT LIST       LOADING    ";
BYTE DSP_BLACK_CARD_LOADING[]              ="   BLACK CARD        LOADING    ";
BYTE DSP_BIN_LOADING[]                     ="       BIN           LOADING    ";
BYTE DSP_MERCHANT_TYPES_LOADING[]          =" MERCHANT TYPES      LOADING    ";
BYTE DSP_PARAMETER_OK[]                    ="  PARAMETER OK. ";
BYTE DSP_AMOUNT_MIN_LIMIT[]                ="AMOUNT<MIN.LIMIT";
BYTE DSP_SENDING[]                         ="     SENDING                    ";
BYTE DSP_RECEIVING[]                       ="    RECEIVING                   ";
BYTE DSP_CARD_BIN_NOT_FOUND[]              ="    CARD BIN        NOT FOUND   ";
BYTE DSP_INST_COUNTER[]                    ="INST.COUNTER: ";
BYTE DSP_PAN[]                             ="PAN ?";
BYTE DSP_CVC[]                             ="CVC ?";
BYTE DSP_REVERSAL_SENDING[]                ="    REVERSAL        SENDING.    ";
BYTE DSP_FILE_EMPTY[]                      ="   FILE EMPTY   ";
BYTE DSP_PIN_TITLE[]                       ="OFFLINE PIN    ";
BYTE DSP_PIN_PROMPT[]                      ="Pin       ( ) :";
BYTE DSP_AMOUNT_MIN_LIMIT_ERR[]                =" Entered amount is less than the minimum allowed";





/////////////////////////////////////////////////////////////////////////////
//DISPLAY WARNING MESSAGES///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BYTE DSP_W_INSUFFICIENT_BALANCE[]          ="  INSUFFICIENT       BALANCE    ";
BYTE DSP_W_UNAVAILABLE_MERCHANT_ID[]       ="   UNAVAILABLE     MERCHANT-ID  ";
BYTE DSP_W_CARD_BLACK_LIST[]        	   ="   CARD BLACK         LIST !    ";
BYTE DSP_W_EXPIRY_DATE[]	               ="   UNAVAILABLE     EXPIRY DATE  ";
BYTE DSP_W_CURRENCY_CODE[]	               ="   UNAVAILABLE    CURRENCY CODE ";
BYTE DSP_W_0_9[]	                       ="   THIS VALUE         0 - 9     ";
BYTE DSP_W_Entered_Amount_Maximum[]        =" Entered Amount  Exceeds Maximum     Allowed    ";
/////////////////////////////////////////////////////////////////////////////
//DISPLAY & PRINT MESSAGES///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BYTE PDS_MERCHANT_ID[]			           ="MERCHANT NO:";
BYTE PDS_STORE_NO[]			               ="STORE NO   :";
BYTE PDS_TERMINAL_ID[]			           ="TERMINAL NO:";
BYTE PDS_BALANCE[]                         ="BALANCE: ";
BYTE PDS_AUTH_CODE[] 		               ="REF.NUMBER :";
BYTE PDS_TMS_NII[] 					= "TMS NII :";
BYTE PDS_PMS_NII[]					= "PMS NII :";
BYTE PDS_NETSERVER_NII[]				= "NETSERVER NII :";
BYTE PDS_BATCH_ID[]					= "BATCH ID :";



/////////////////////////////////////////////////////////////////////////////
// ARABIC ???? //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BYTE PRT_ARABIC_FLAIRPAYMENT[]		       ="\x20\x20\x20\x20\x20\xce\xe6\xa7\xa1\x20\x9c\xa1\xe5\x20\xdc\x9f\x99\xe6\xd8\x20\x20\x20\x20\x20";
BYTE PRT_ARABIC_SLIPHEADERPAYMENT[]        ="\x20\x20\xad\xa7\x9f\x8f\xa7\xae\x20\xa0\xa7\x8e\xa8\xa7\xae\x20\xdc\xe0\x8e\xa4\x20\xdd\x8f\x20"
"\x20\x20\xaf\xa4\x9f\xd4\x8e\x8e\x20\xdd\xa6\x8e\x9f\x97\x93\xa7\x2a\x20\xaf\x9b\xa1\xe5\xa6\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2e\xce\xe6\xa7\xa1\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20";
BYTE PDS_ARABIC_MERCHANT_ID[]		       =":\x20\xce\x91\x9f\x8f\xa7\xc4\x20\xdd\xa4\xce";//12*********
BYTE PDS_ARABIC_STORE_NO[]			       =":\x20\xce\x91\x8f\xa8\xa7\xc4\x20\xdd\xa4\xce";//12*********
BYTE PDS_ARABIC_TERMINAL_ID[]		       =":\x20\xcf\x9f\xab\x91\xa7\xc4\x20\xdd\xa4\xce";//12*********

//BYTE PRT_ARABIC_BATCH[]                  =":\x20\x20\xb0\x9b\xa1\xe5\xa7\xc4\x20\xdd\xa4\xce";//13*********
BYTE PRT_ARABIC_BATCH[]                    =":\xce\xda\xdd\x20\xc4\xdc\xdd\xd6\xc4\xdd\xdc\xc6";//13*********

BYTE PDS_ARABIC_AUTH_CODE[] 		       =":\x20\x9c\x91\xce\xa8\xa7\xc4\x20\xdd\xa4\xce";//12
BYTE PRT_ARABIC_SLIPNO[]                   =":\x20\xc6\xce\xe0\x8f\x9f\xa2\xa7\xc4\x20\xdd\xa4\xce";//14
BYTE PRT_ARABIC_CARD_NUMBER[]              =":\x20\x20\xb0\xa4\x9f\xd4\x8e\xa7\xc4\x20\xdd\xa4\xce";//14*********
BYTE PRT_ARABIC_PAYMENT_AMOUNT[]	       =":\x20\xb0\x9b\xa1\xe5\xa7\xc4";   //8*********
BYTE PRT_ARABIC_PAYMENT[]			       ="\x20\x20\x20\x20\x20\x20\x20\x20\x20\xb0\x9b\xa1\xe5\xa7\xc4\x20\x20\x20\x20\x20\x20\x20\x20\x20";//24*********
BYTE PRT_ARABIC_GOODS_RECEIVED_PAYMENT[]   ="\xe5\xe6\xa6\x8b\x8f\xa6\x20\xdc\x9f\x99\xe6\xe3\xc4\x20\xc4\xa9\xab\x8e\x20\xd5\xa2\x8f\x93\xc0"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2e\xdb\x8f\x9b\xa1\xe5\xa7\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20";
BYTE PRT_ARABIC_GOODS_RECEIVED_FOOTER[]    ="\x20\x20\xc7\x9f\xa2\x99\xae\xe0\xa8\xe0\x20\xd4\xe0\xce\x98\x20\xda\x9f\x8e\xd4\xa9\xae\x20\x20"
"\x20\x20\x20\x2e\xce\xe6\xa7\xa1\x20\xaf\xa4\x9f\xd4\x8e\x20\xdc\x99\x9f\x93\x2a\x20\x20\x20\x20";
BYTE PRT_ARABIC_FLAIRLOYALTY[]		       ="\x20\x20\x20\x20\xce\xe6\xa7\xa1\x20\xc7\x8c\xa1\x9f\xa6\xa8\x20\xdc\x9f\x99\xe6\xd8\x20\x20\x20";

BYTE PRT_ARABIC_SLIPHEADERLOYALTY[]        ="\x20\x20\x20\xc5\x9f\x97\x93\x2a\x20\xaf\xe6\xa7\x9b\x8f\x8e\x20\xdd\xe0\xa5\xa9\x97\x20\x20\x20"
"\x20\xaf\xa1\x9f\x8a\x8d\x8e\x20\xce\xe6\xa7\xa2\x8e\x20\xdd\xa6\x8f\x9f\x8b\xa1\x9f\xa6\xa8\x20"
"\x20\x20\x20\x20\x20\xaf\xa8\xe6\xa4\x20\xb1\xa7\xc2\x20\xd4\x9f\xa5\xa9\xa7\xae\x20\x20\x20\x20"
"\x20\xdf\x9f\xa9\xe5\xc0\x20\xaf\xa9\xe6\x8e\xa8\xa7\xae\x20\xdd\xa6\x8f\x9f\xe6\xce\x8f\x98\xa8";
BYTE PRT_ARABIC_LOYALTY[]			       ="\x20\x20\x20\x20\x20\x20\x20\x20\xc7\x8c\xa1\x9f\xa6\xa8\xa7\xae\x20\x20\x20\x20\x20\x20\x20\x20";//24
BYTE PRT_ARABIC_PURCHASE_AMOUNT[]	       =":\xc7\x9f\xe6\xce\x8f\x98\xa8";   //8
BYTE PRT_ARABIC_GOODS_RECEIVED_LOYALTY[]   ="\xe5\xe6\xa6\x8b\x8f\xa6\x20\xdc\x9f\x99\xe6\xe3\xc4\x20\xc4\xa9\xab\x8e\x20\xd5\xa2\x8f\x93\xc0"
"\x20\x20\x20\x20\x20\x20\x20\x2e\xdd\xa6\x8f\x8b\xa1\x9f\xa6\xa8\xa7\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20";
BYTE PRT_ARABIC_SIGN[]			           ="----------------:\x9c\xe6\xa4\xe0\x8f\xa7\xc4";//24

BYTE PRT_ARABIC_GOODS_RECEIVED[]	       ="\x20\x20\x20\x20\x20\xe5\xa5\x9b\xa7\xae\x20\xae\xa9\xaa\x20\xc7\xa7\x8e\xa4\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20";
BYTE PRT_ARABIC_DESCRIPTION_OF_GOODS[]     ="\x20\x20\x20\x20\x20\x20\x9c\xa7\x97\xa7\xc4\x20\xc7\x9f\xa2\x99\xc4\xe0\xa8\x20\x20\x20\x20\x20";//24
BYTE PRT_ARABIC_FLAIRINSTALLMENT[]	       ="\xd4\xe6\x97\xa5\x8f\xa7\xa7\x20\xce\xe6\xa7\xa1\x20\xdd\x9f\xd5\xa9\x8e\x20\x9c\xe6\x8e\xa7\xae";

BYTE PRT_ARABIC_SLIPHEADERINST[]		   ="\x20\xe0\x20\xdd\xb5\x8f\x97\xb5\xa7\x20\xce\xe6\xa7\xa1\x20\xdc\xe6\xa6\xe0\x20\xae\xa9\xaa\x20"
"\x20\x20\x20\x9c\xa7\x97\xa7\xae\x20\x9a\xae\xce\x98\x8e\x20\xdd\x9f\xa4\x20\xe5\xa4\x20\x20\x20"
"\x20\x20\xc6\xe5\xce\xae\xe0\xa7\xae\xe0\x20\xae\xe5\xa5\xa9\x20\xaf\xa9\xe6\x8e\xa8\xa7\xae\x20"
"\x20\xaf\xa7\x91\x97\xa8\xa7\xae\x20\xaf\xa8\xb3\x9f\xa5\xa7\x9f\x8e\x20\xb5\xe6\x99\xa2\x8f\x20"
"\x20\x20\xdf\xa9\xaa\x20\xd6\x9f\x8e\x20\xe5\xa4\xe0\x20\xaf\xa5\xa1\xce\xa8\xa7\xae\xe0\x20\x20"
"\x20\xdd\x9f\xa6\x93\xc0\x20\xdc\x8a\x20\xad\xa1\x20\xdd\xa6\xe6\xa7\xd8\x20\x9c\xa7\x97\xa7\xae"
"\x20\x20\xaf\xa4\x9f\xd4\x8e\x20\xd4\xe0\xce\x98\xe0\x20\xb2\xdc\xa8\x9f\x9b\x8f\xa7\xae\x20\x20"
"\x9a\x9f\xa9\x8e\xe0\x20\xdd\xa6\xe6\xa7\xd8\x20\xc6\xce\xe5\x9f\x99\xa7\xae\x20\xce\xe6\xa7\xa1"
"\x20\x20\x20\x20\xaf\xe6\xa7\x9f\x8f\xa7\xae\x20\xd0\x97\xb6\xae\x20\xb1\xa7\x9a\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20";
BYTE PRT_ARABIC_TENURE[]                   =":\x20\x20\xc6\xe5\xa8\xa7\xae";
BYTE PRT_ARABIC_INST[]			           =":\x20\x20\xd4\x97\xa5\xa7\xae";
BYTE PRT_ARABIC_TOTAL[] 			       =":\xad\xa7\x9f\xa8\x91\xe3\xae";//8

BYTE PRT_ARABIC_INSTALLMENT[]		       ="\x20\x20\x20\x20\x20\xd4\xe6\x97\xa5\x8f\xa7\x9f\x8e\x20\x9c\xe6\x8e\xa7\xae\x20\x20\x20\x20\x20";//24

BYTE PRT_ARABIC_KEEPTHISPAPER[]	           ="\x20\x20\x20\x20\xb0\xa4\x9f\xd4\x8e\x20\xdd\xa6\xa8\xc4\xe5\x95\x8f\x97\xb4\x20\xae\xce\xa6\x98"
"\x20\x20\xe0\x91\xce\xa9\x20\xdd\xa6\xe5\xa5\x9a\x20\xae\xa9\xaa\x20\xce\xe6\xa7\xa1\x20\x20\x20"
"\x20\x20\x20\xaf\x9b\x91\xae\xce\xa8\xa7\xa7\x20\xac\x8e\x20\xd5\x9f\xa2\x8f\x93\xb4\xae\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\xb5\x8e\xa5\x8f\x97\xa8\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20";

BYTE PRT_ARABIC_ACCOUNT_MAINT[]		       ="      \xc5\x9f\x97\x91\xa7\xc4 \xb0\xa9\x9f\xe6\x99      ";    //24
BYTE PRT_ARABIC_FLAIRACCOUNT_MAINT[]       ="   Flair Acount Maint.  ";

BYTE DSP_APN[]			         ="APN :   ";
BYTE DSP_HOST_IP[]			     ="HOST IP:   ";
BYTE DSP_HOST_PORT[]			 ="HOST PORT:   ";
