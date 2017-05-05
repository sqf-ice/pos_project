/****************************************************************************
NAME
    framework.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.14      - created
****************************************************************************/
/*
4.55 
	1.fix GetRandomKey()
	2.modify method of daily update check 
	3.when setup, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad will with default value "0000"
	4.print faster
	5.set Terminal ID automatically, support this alphanumeric for terminal Id 
*/
/*
4.56
	1. update method of get product list, from once request to reuest in loop mode
	2. allow alpha character in terminal id 
	3. fix bug: after remote download, data back up is incorrect
	4. Modify: enlarge font size on pending order slip
	5. fix bug: when using GPRS, remote softw download is failed.
	6. some s90, printout is not clear,  add PrnSetGray(180);
	
v4.58
	1. fix: remote download is not supported
	2. printing is very slow(reduce printer buffer from 100 lines to 50 lines)
	3. add : new function credit transfer order
	4. update: loop of communication(Txd & Rxd), cuz buffer of API is limited. 

v4.59
	1. modify logic of updating product list
	2. update structure of file 

v4.60
	1. add maintenace_ext for Reserving
	2. increase offline cnt from uchar to uint
	3. add credit order
	4. add change pwd
	5. add new barcode scanning type, the barcode can be MID, 
		then POS will show a terminal list which belongs to the MID.
	6. add credit order report in order report list 
	7. add credit order summary in summary order list
	8. fix: memory crash when remote download

v4.61
	1. fix online stock report won't print receipt if it successes.

v4.62
	1. roll back to hide e-products on Offline Physical Report
	2. fix Order report by prod doesn't print product details.
	3. add: after remote download, setting download is necessary

v4.65
	1. fix: after remote download, setting download is necessary
	2. add:version under structure EXT, cuz the version increases too fast

v4.70
	1. timeout is set in setting download, not hardcoded. default vaule is 15 sec.
*/
#ifndef _FRAMEWORK_H
#define _FRAMEWORK_H

//////////////////////////////////////////////////////////////////////////
#define APP_ECC				//switch
#define MULTI_LANG
//////////////////////////////////////////////////////////////////////////
#define _S_SERIES_			//switch  necessary
//////////////////////////////////////////////////////////////////////////

#define ForAlperTest		//un-hide when test mode (mute)
//#define LHB_DEBUG			//un-hide when debug mode (with dump & no "no paper" check)
//#define LHB_TEST			//un-hide to set parameter with default values

#ifdef MULTI_LANG
#define FARSI
#include "Farsi.h"
#endif

//////////////////////////////////////////////////////////////////////////

enum
{
	LANG_EN = 0,
#ifdef FARSI
	LANG_ARABIC,
#endif
	LANG_END,//
};

enum
{
	TRACE_NONE = 0,
	TRACE_ERROR,
	TRACE_WARNING,
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define TIMERNO 4
#define	TIME_OUT_STANDARD_30S	30  //(Sec)
#define TIME_DELAY				2	//(Sec)
#define TIME_CANCL_LIMIT		15	//(Min)
#define TIME_RETRY	3

#define DISPLAYLINE		4
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

enum
{
	STR_DEFAUT_NUMERO_PAD_PRINC = 0,
	STR_DEFAUT_NUMERO_PAD_SEC,
	STR_DEFAUT_ADRESSE_X25,
	STR_AUTHENTIFICATION,
	STR_EUR,
	STR_BLANC,
	STR_TAB,
	STR_AUTHENTI_PAP,
	STR_AUTHENTI_SHAP,
	STR_VIDE,
	STR_ETOILES,
	STR_GPRS,
	STR_PPP,
	STR_ETOILES2,
	STR_TIRETS,
	STR_CHOIX_OPERATEUR,
	STR_PASSWORD_DISPLAY,
	STR_ERROR,
	STR_COUPER_LE_TICKET,
	STR_PRESS_ENTER,
	STR_VAL_CANCEL,
	STR_VAL_CANCEL_COR,
	STR_IDENT_MANAGER,
	STR_PASSWORD_SECRET,
	STR_PASSWORDS_CASHIERS,
	STR_PASSWORD_CASHIER,
	STR_TIME_SOFTWAREDOWNLOAD,
	STR_TIME_SALESPOLLING,
	STR_TIME_SETTINGSDOWNLOAD,
	STR_PRINT_PARAM,
	STR_NUMERO_PAD,
	STR_NUMERO_PAD_SECOURS,
	STR_MENU_PPP,
	STR_NUM_TLCHGT,
	STR_ID_TERMINAL,
	STR_TYPE_IMPRIMANTE,
	STR_REINIT_APPLI,
	STR_INFOS_TRACE,
	STR_NUMEROTATION,
	STR_FIN_CONNEXION,
	STR_PRINT,
	STR_ON_GOING,
	STR_TICKET_CLIENT,
	STR_TICKET_MANAGER,
	STR_DUPLICATA2,
	STR_NON_DISPONIBLE,
	STR_PAS_D_OPERATION,
	STR_DONE,
	STR_DEJA_PRESENT,
	STR_REPORT_COMMER,
	STR_REPORT_CASHIER,
	STR_REPORT_DAILY,
	STR_EFFACEMENT_DES,
	STR_RAPP_VAL_ANN,
	STR_PASSWORD_MANAGER,
	STR_ENTRER_CODE,
	STR_REPORTS,
	STR_APPLICATION,
	STR_INITIALISEE,
	STR_PARAMETRES_APPLI,
	STR_INCOMPLETS,
	STR_TICKET_ABANDON,
	STR_ATTENTE_REPONSE,
	STR_SERVEUR,
	STR_MEMOIRE_PLEINE,
	STR_ACTIVER_TELECH,
	STR_ANNUL_TELECH,
	STR_TELECHARGEMENT,
	STR_ACTIVE,
	STR_DESACTIVE,
	STR_MODIFICATION,
	STR_ENTRER_LE_CODE,
	STR_CONNEXION_OK,
	STR_NUMEROTATION_OK,
	STR_CONNEXION,
	STR_SERVEUR_KO,
	STR_NUMERO_OCCUPE,
	STR_NUMERO_INTERDIT,
	STR_PAS_DE_PORTEUSE,
	STR_LINE_ABSENTE,
	STR_ABANDON,
	STR_PROBLEME_MODEM,
	STR_PRODUCT,
	STR_GESTION_RECH,
	STR_INGEVOUCHER,
	STR_RECHARGEMENT,
	STR_LANCER_RECHARGE,
	STR_PASSWORDS,
	STR_TELEPARAMETRAGE,
	STR_TELEPARAMETRAGE_PRINT,
	STR_A_FAIRE,
	STR_DEJA_EFECTUE,
	STR_TABLE_OPERATEURS,
	STR_TABLE_PRODUCTS,
	STR_TABLE_TICKET,
	STR_TABLE_MAINTENANCE,
	STR_TABLE_TERMINAL,
	STR_TABLE_HORODATAGE,
	STR_ECHEC,
	STR_SUCCES,
	STR_IDENTIFICATION,
	STR_VENTE,
	STR_SALESPOLLING,
	STR_INFO_TELECOL,
	STR_DEMANDE_VERSION,
	STR_REEL,
	STR_REEL_V22,
	STR_SIMUL,
	STR_PINS_EXPIRED,
	STR_PINS_EXPIRED2,
	STR_PINS_USED,
	STR_PINS_USED2,
	STR_ANOMALIE_VENTE,
	STR_ANOMALIE_VENTE2,
	STR_INTERDIT,
	STR_COMMANDE,
	STR_PARAM_ENREGISTRE,
	STR_VAL_POUR_ENTRER,
	STR_NOM_CATEGORY,
	STR_NOM_PRODUCT,
	STR_ID_TERMINAL_Min,
	STR_ID_COMM,
	STR_NUM_TRANS,
	STR_VALEUR,
	STR_No_SERIE,
	STR_FIN_VALIDITE,
	STR_PASSWORD_ERREUR,
	STR_Op_Min,
	STR_RECAPITULATIF,
	STR_NB_CODES_CHARGES,
	STR_NB_CODES_A_CHARGER,
	STR_Amount_Tot,
	STR_TOTAL,
	STR_CASHIER_Min,
	STR_Nb_Codes_Telecol,
	STR_Envoi,
	STR_SALESPOLLING2,
	STR_NB_CODES,
	STR_TYPE_IMPRIMANTE_Min,
	STR_Serv_MDP,
	STR_HOUR,
	STR_ETAT_DU_STOCK,
	STR_DISP,
	STR_VEND,
	STR_IDENT,
	STR_Version_Ticket,
	STR_Version_Category,
	STR_Version_Product,
	STR_Version_Maintenance,
	STR_Version_Terminal,
	STR_Version_Horodatage,
	STR_RECHARGEMENT_CODES,
	STR_REPORT,
	STR_MANAGER,
	STR_CASHIER,
	STR_CASHIER_NB_AMOUNT,
	STR_PRODUCT_NB_AMOUNT,
	STR_PXUNITAIRE,
	STR_NB,
	STR_AMOUNT,
	STR_DUPLICATA,
	STR_TELECHARGEMENT_APPLI,
	STR_RECHARGEMENT_CODES_2,
	STR_SALESPOLLING_2,
	STR_PAS_DE_TELECH,
	STR_PAS_DE_RECHARGE,
	STR_PAS_DE_TELECOL,
	STR_PAS_DE_TELEPARA,
	STR_PLUS_DE_CODE,
	STR_DISPONIBLE,
	STR_ON_LINE,
	STR_TITRE_TICKET_PARAMETRE,
	STR_SOUS_TITRE_MANAGER,
	STR_SOUS_TITRE_PRODUCT,
	STR_SOUS_TITRE_TERMINAL,
	STR_SOUS_TITRE_CONNEXION,
	STR_SOUS_TITRE_TELECHARGEMENT,
	STR_SOUS_TITRE_CASHIERS,
	STR_SERVICE,
	STR_DEMARRE,
	STR_ARRETE,
	STR_DEMARRER_SERVICE,
	STR_ARRETER_SERVICE,
	STR_VISUALISE_STOCK,
	STR_PRINT_STOCK,
	STR_CHARGER_CLE,
	STR_CHARGEMENT_CLE,
	STR_CREDIT_EPUISE,
	STR_PASSWORD_UPPER,
	STR_REPORT_DAILY_PRINT,
	STR_STOCK_SERVEUR,
	STR_EPUISE,
	STR_NOUVEAU_CODE,
	STR_CONFIRMER_CODE,
	STR_AUTRE_VENTE,
	STR_PRINT_CODE_BARRE,
	STR_PRINT_LOGO,
	STR_OUI,
	STR_NON,
	STR_VALIDE_COMMANDE,
	STR_CHARGEMENT_LOGO,
	STR_MASTER_APPLI,
	STR_TYPE_CNX_TELECH,
	STR_RTC,
	STR_X25,
	STR_PRINT_COMANDE,
	STR_INFOS_CODES,
	STR_ENVOI_NUM_SERIE,
	STR_ENVOI_RECLAMATION,
	STR_FULL_AUTO,
	STR_IMP_TELECOL,
	STR_RECHARGEMENT_CODES2,
	STR_RECHARGEMENT_CODES3,
	STR_IMP_NOM_APPLI,
	STR_IMP_REPORT_AUTO,
	STR_PRINT_ZERO,
	STR_IMP_DETAIL_RECH,
	STR_IMP_RAPP_DAILY,
	STR_MODE_DEMO,
	STR_RECLAMATION,
	STR_RECPT_MESSAGE,
	STR_CONFIRM_MESSAGE,
	STR_INIT_MANAGER,
	STR_ANNULER,
	STR_DERNIERE_VENTE,
	STR_ENVOI_ANNULATION,
	STR_PROPOSITION,
	STR_ANNUL_VENTE,
	STR_REFUS,
	STR_Vendu_le,
	STR_CONFIRMATION,
	STR_DIFFERENT,
	STR_STOCK,
	STR_STOCK_GENERAL,
	STR_STOCK_DESIZE,
	STR_ACTUALISE_STOCK,
	STR_IMP_RAPP_VENTE,
	STR_DIAL_GPRS,
	STR_CONNEXION_KO,
	STR_INVALID_ENTRY,
	STR_LIMIT_PORT_NUMBER,
	STR_APN,
	STR_IP_ADDRESS,
	STR_IP_PORT,
	STR_LOCAL_IP_PORT,
	STR_USERNAME,
	STR_PASSWORD,
	STR_AUTHENTI,
	STR_PRINT_GPRS_CONFIG,
	STR_RESET_GPRS_CONFIG,
	STR_PASSWORD_PIN,
	STR_PARAMETRES_GPRS,
	STR_GPRS_EFFACES,
	STR_NOT_SPECIFIED,
	STR_CONNECTION,
	STR_SERIE,
	STR_STANDARD,
	STR_ITEM_CODE,
	STR_ITEM_MENU,
	STR_ENREGISTRER,
	STR_VOTRE_TPE,
	STR_VERIFICATION,
	STR_NOUVEAU_SMS,
	STR_INITIALISATION,
	STR_LOGICIEL,
	STR_A_ECHOUER,
	STR_VEUILLEZ_REDEMMARER,
	STR_SERVEUR_CENTRE,
	STR_CONFIG_AUTO,
	STR_MENU_PLANNING,
	STR_MENU_PRINT,
	STR_MENU_CONNECTION,
	STR_PLUS_DE_PAPIER,
	STR_PAPIER_REMPLACE,
	STR_FIN_PRINT,
	STR_TICKET,
	STR_DHCP,
	STR_IP_POS,
	STR_MASQUE_SS_RESEAU,
	STR_PASSERELLE,
	STR_DNS_1,
	STR_DNS_2,
	STR_ETHERNET_SETTINGS,
	STR_MENU_ETHERNET,
	STR_ETHERNET,
	STR_VEUILLEZ,
	STR_PATIENTER,
	STR_MULTISALE,
	STR_ARRETER_VENTE,
	STR_REPRENDRE_VENTE,
	STR_DOUBLEPAPER,
	STR_REPORT_DISPO,
	STR_REPORT_BY_SALER,
	STR_REPORT_BY_NETWORK,
	STR_GLOBAL_CAISSIER_REPORT,
	STR_REPORT_NETWORK,
	STR_TEMPORAIRE,
	STR_REPORT_VENTES,
	STR_CHOIX_RACCOURCI,
	STR_PRINT_RACCOURCI,
	STR_TITRE_LISTE_RACCOURCI,
	STR_USE_MODE_DEGRADE,
	STR_GESTION_EQUIPE,
	STR_EQUIPE_EN_COURS,
	STR_DEMARRER_SERVICE_EQUIPE,
	STR_ARRETER_SERVICE_EQUIPE,
	STR_PLEASE_PRINT,
#ifndef DUBAI
	STR_SHIFT_REPORT,
	STR_MENU_REPORT_SHIFT,
	STR_MENU_REPORT__CURRENT_SHIFT,
#else
	STR_SHIFT_REPORT,
	STR_MENU_REPORT_SHIFT,
	STR_MENU_REPORT__CURRENT_SHIFT,
#endif
	STR_SHIFT_CASHIER_REPORT,
	STR_EQUIPE,
	STR_DU,
	STR_AU,
	STR_INACTIF,
	STR_NO_CURRENT_SHIFT,
	STR_NAME,
	STR_SOLDE,
	STR_SITUATION_CLIENT,
	STR_RESTOCK_CAPACITY,
	STR_CREDIT_CLIENT,
	STR_CREDIT_AUTORISE,
	STR_QUANTITE_OK,
	STR_QUANTITE,
	STR_INVOICED_AMOUNT,
	STR_MERCHANT_ID,
	STR_SALESMAN_ID,
	STR_CHECK_ORDER,
	STR_CHECKING_ORDER,
	STR_VALIDATE_ORDER,
	STR_INCOMPLETE_ORDER,
	STR_PB_ORDER_EMPTY,
	STR_PB_ORDER_AMOUNT,
	STR_PB_ORDER_MERCHANT_ID,
	STR_PB_ORDER_TERMINAL_ID,
	STR_PB_ORDER_SALESMAN_ID,
	STR_PB_ORDER_PASSWORD,
	STR_EXIT_ORDER,
	STR_SEND_ORDER,
	STR_ORDER_VALIDATED,
	STR_ORDER_REFUSED,
	STR_ORDER_FAILED,
	STR_PENDING_ORDER,
	STR_NO_ORDER,
	STR_ORDER_NUMBER,
	STR_ORDER,
	STR_TICKET_MERCHANT_ID,
	STR_TICKET_SALESMAN_ID,
	STR_TICKET_TERMINAL_ID,
	STR_SALESMAN_COPY,
	STR_MERCHANT_COPY,
	STR_SUMMARY,
	STR_PINS_QUANTITY,
	STR_FACE_VALUE,
	STR_PAID,
	STR_MERCHANT_SIGNATURE,
	STR_SIGNATURE,
	STR_BAD_SALESMAN,
	STR_AUTHENTIFICATION_MAJ,
	STR_PRODUCT_UNAVAILABLE,
	STR_FOR_THIS_MERCHANT,
	STR_SERVER_STOCK_EMPTY,
	STR_INSUFFICIENT_CREDIT,
	STR_BAD_MERCHANT_ID,
	STR_OR_TERMINAL_ID,
	STR_ORDER_MODE,
	STR_STOCK2,
	STR_PERC_DISCOUNT,
	STR_SCAN_BARCODE,
	STR_TRY_AGAIN,
	STR_PLEASE_INPUT,
	STR_VALID_POURC,
	STR_MERCHANT_SERIAL,
	STR_SERIAL_NUMBER,
	STR_ORDER_PREVIEW,
	STR_OPERATION_REFUSED,
	STR_OPERATION_FAILED,
	STR_NO,
	STR_DTU,
	STR_TELEPHONE,
	//STR_AMOUNT,
	STR_CONFIRMER_TRANSACTION,
	STR_IMP_CONFIRM_DTU,
	STR_PASSWORD_PARTIE_1,
	STR_PASSWORD_PARTIE_2,
	STR_RESET_CODE,
	STR_CHANGE_DISCOUNT,
	STR_SALESMAN_NAME,
	STR_MERCHANT_NAME,
	STR_SOFT_DOWNLOAD,
	STR_REASON,
	STR_NOT_ALLOWED_FOR,
	STR_THIS_MERCHANT,
	STR_GET_SALESMAN,
	STR_INFORMATIONS,
	STR_REEL_V32BIS,
	STR_PLEASE_CHECK,
	STR_PENDING_ORDERS,
	STR_PARAM_DOWNLOAD,
	STR_SALESMAN_CREDIT,
	STR_PHYSICAL_CARDS_STOCK,
	STR_CASH_ORDER,
	STR_CONSIGN_ORDER,
	STR_RETURN_ORDER,
	STR_ACTIVATION_NUMBER,
	STR_EXPIRY_DATE,
	STR_SN,
	STR_CURRENT_DEPOSIT,
	STR_SR,
	STR_CUSTOMER_COPY,
	STR_MERCHANT_COPY2,
	STR_POWERED_BY,
	STR_TRACE_NO,
	STR_RESPONSE_CODE,
	STR_ERROR_MSG,
	STR_PINS_RESTOCKED,
};

extern unsigned char **gl_pLangSelPiont;
extern unsigned char *gl_pLang[];

unsigned char CheckFirstRun(void);
void ScrPrintStr(unsigned char col,unsigned char row,unsigned char mode, char *str,...);
void DispString(void *pszStr, unsigned short nPosition);
void LangInit(void);

#define YESNO_IN	0x20
#define POINT_IN	0x40
#define DISCNT_IN	0x80
/****************************************************************************
 Function:          get string
 Param in:
   line
   ucMode
   **
	NUM_IN		0x01
	ALPHA_IN	0x02
	PASS_IN		0x04
	CARRY_IN	0x08
	ECHO_IN		0x10
	YESNO_IN	0x20
	POINT_IN	0x40
	DISCNT_IN	0x80
   **
   ucMinlen
   ucMaxlen
   ucTimeOut
 Param out:
   pszOut			
 Return Code:
	KEYENTER
	0xFF
	KEYUP
	KEYDOWN
****************************************************************************/
unsigned char GetInput(unsigned char line, unsigned char ucMode, unsigned char ucMinlen, \
					   unsigned char ucMaxlen, unsigned char *pszOut, unsigned char ucTimeOut);
enum
{
	TIME_GET_TIME = 0X01,
	TIME_GET_DATE = 0X02,
};
/****************************************************************************
 Function:          Get date time
 Param in:
   line
   Mode
   **
	TIME_GET_TIME
	TIME_GET_DATE
   **
   ucTimeOut
 Param out:
   DateTime			
 Return Code:
	KEYENTER
	0xFF
	KEYUP
	KEYDOWN
****************************************************************************/
unsigned char GetInputTime(unsigned char line, unsigned char *DateTime, unsigned char Mode, \
						   unsigned char ucTimeOut);

/****************************************************************************
 Function:          Get yes or no
 Param in:
   nWaitTime
 Param out:
 	
 Return Code:
	KEYENTER
	0xFF
	KEYUP
	KEYDOWN
****************************************************************************/
unsigned char YesNo(unsigned short nWaitTime);

/****************************************************************************
 Function:          Get Current date & time (ASC 12 bits)
 Param in:
 
 Param out:
 	stCurTime
 Return Code:
	
****************************************************************************/
void GetCurTime(unsigned char *stCurTime);
void GetSerialNumber(unsigned char *pszSerialNumberASC);
void GetInfoPOS(unsigned char *pszInfoPOS);
void UpdateApp(void);

enum
{
	BEEP_ERR = 0,
	BEEP_OK,
	BEEP_NORMAL,
};
void Beep_Speaker(unsigned char type);
void DispRate(unsigned char ucLineNo, long lTotal, long lCurrent, long lOld);

#ifdef APP_ECC
#include "framework_salesman.h"
#else
#include "framework_merchant.h"
#endif

#endif
