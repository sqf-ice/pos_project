#include "vxlibs.h"
#include "Logo.h"
#include "pax_emv.h"
#include "utility.h"
#include "function.h"
#include "appLib.h"

extern char *pszAlphaTbl[];
extern void MapChar2Index(uchar ch, int *piRow, int *piCol);
extern void Bin2Bcd_Byte(BYTE *pBcd, BYTE Bin);



struct sIndex
{
	BYTE RID[5];
	BYTE index;
};


/*
#ifndef DEVELOP_VERSION
typedef struct stEst_CAPK
{
	short inPublicKeyIndex;		 
	char  szCAPKFile[EMV_PK_FILE_NAME+1];
	char  szCAPKExpDate[EMV_PK_EXPDATE_SIZE+1];
}Est_CAPK;

typedef struct stEst_SupApp
{
	char szSupportedAID[EMV_MAX_AID_SIZE+1];
	short inPartialNameAllowedFlag;		 
	char szTermAVN[EMV_AVN_SIZE+1];		 
	char szSecondTermAVN[EMV_AVN_SIZE+1];
	char szRecommenedAppNameAID[MAX_APPNAME_LEN+1];		 // 17
}Est_SupApp;
#endif
*/
/*
#ifndef DEVELOP_VERSION
typedef struct stEst_Rec
{
	long lnNbrOfTransactions;		 
	char szSchemeLabel[EMV_SCHEME_NAME_SIZE+1];
	char szRID[EMV_RID_SIZE+1];		 
	char szCSNList[EMV_CSN_FILE_NAME_SIZE+1];
	Est_CAPK capk[15];
	Est_SupApp sup_app[10];
	short inEMVTableRecord;		
	char szCSNListFile[EMV_CSN_FILE_NAME_SIZE+1];
	char filler[2];
}Est_Rec;
#endif
*/
struct stEST_RecHdr
{
	BYTE vers[10];
	BYTE cont[6];
};


struct stEST_RecHdr EST_RecHdr={
	"\x31\x36\x2F\x30\x31\x2F\x30\x38",
	"\x54\x6F\x79\x67\x61\x72"
};


char EmvMVTFile[]		="MVT.DAT";
char EmvESTFile[]		="EST.DAT";
char AraFontFile[]		= "ARABIA.LNG";

#define SEEK_FIRST           -1
#define SEEK_LAST            -2

struct spsales
{
    char panStr[21];
    char panSeqStr[3];
    char trnAmt[9];
};

struct spsales stSpsales;

#ifdef APP_COMPILE_CPA
EMV_APPLIST gPROCCO_CPA_APP = {
	"PROCCO CPA",
    "\xA0\x00\x00\x04\x31",
	5,
    PART_MATCH,
    0,  // Priority
    0,  // TargetPer
    0,  // MaxTargetPer
    1,  // FloorLimitCheck
    1,  // RandTransSel
    1,  // VelocityCheck
	0,  // FloorLimit
    0,  // Threshold
	"\x00\x10\x00\x00\x00", // TACDenial
	"\xD8\x40\x04\xF8\x00", // TACOnline
	"\xD8\x40\x00\xA8\x00", // TACDefault
	"\x00\x00\x00\x12\x34\x56",
	"\x03\x9F\x37\x04",     // dDOL
	"\x0F\x9F\x02\x06\x5F\x2A\x02\x9A\x03\x9C\x01\x95\x05\x9F\x37\x04", // tDOL
	"\x00\x02",             // Version
	"",
};
#else
EMV_APPLIST gVISA_VSDC_APP = {
	"VISA CREDIT",
    "\xA0\x00\x00\x00\x03\x10\x10",
	7,
    PART_MATCH,
    0,  // Priority
    0,  // TargetPer
    0,  // MaxTargetPer
    1,  // FloorLimitCheck
    1,  // RandTransSel
    1,  // VelocityCheck
	0,  // FloorLimit
    0,  // Threshold
	"\x00\x10\x00\x00\x00", // TACDenial
	"\xD8\x40\x04\xF8\x00", // TACOnline
	"\xD8\x40\x00\xA8\x00", // TACDefault
	"\x00\x00\x00\x12\x34\x56",
	"\x03\x9F\x37\x04",     // dDOL
	"\x0F\x9F\x02\x06\x5F\x2A\x02\x9A\x03\x9C\x01\x95\x05\x9F\x37\x04", // tDOL
	"\x00\x8c",             // Version
	"",
};
#endif


/*unsigned char sAlBarakaLogo_Display[]={
0x04,
0x00,0x68, 
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,
   0xf0,0xf8,0xfc,0x3c,0x3c,0x1c,0x3c,0x3c,0xfc,0xf0,0xf0,0xc0,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x68, 
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x80,0x80,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,
   0x1f,0x3f,0x00,0x00,0x00,0x00,0xe0,0xf8,0xff,0x3f,0x1f,0x87,0xe0,0xf0,0xf8,0x00,
   0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x68, 
   0x00,0x00,0x00,0x00,0x00,0x20,0x10,0x10,0x10,0x10,0x30,0xf0,0xe0,0x00,0x00,0x00,
   0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0x41,0x41,0x41,0xe1,0xff,0xde,0x84,0x00,
   0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xf0,0xf0,0x00,0x00,0x00,0xe0,0xf0,0x10,0x10,
   0x10,0x00,0x00,0x10,0x10,0x10,0x10,0xb0,0xe0,0x00,0x00,0x00,0xff,0xff,0x80,0x80,
   0xc0,0xe0,0x30,0x10,0x00,0x00,0x00,0x00,0x20,0x10,0x10,0x10,0x30,0xf0,0xe0,0x00,
   0x00,0x00,0x00,0x00,0x00,0x1f,0x07,0x03,0xf0,0xfc,0xfe,0xff,0x1f,0x03,0x00,0x00,
   0x00,0xff,0xfe,0xf8,0xe0,0x00,0x00,0x00,
0x00,0x68, 
   0x00,0x00,0x00,0x00,0x0c,0x1e,0x3b,0x21,0x21,0x21,0x11,0x3f,0x3f,0x00,0x00,0x00,
   0x1f,0x3f,0x30,0x20,0x00,0x00,0x3f,0x3f,0x30,0x30,0x30,0x30,0x18,0x1f,0x0f,0x00,
   0x00,0x1e,0x3e,0x31,0x21,0x21,0x11,0x3f,0x3f,0x00,0x00,0x00,0x3f,0x3f,0x00,0x00,
   0x00,0x1e,0x3f,0x31,0x21,0x21,0x11,0x19,0x3f,0x00,0x00,0x00,0x3f,0x3f,0x03,0x01,
   0x03,0x07,0x1e,0x38,0x30,0x20,0x0c,0x3e,0x3f,0x21,0x21,0x21,0x11,0x3f,0x3f,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x0f,0x1f,0x3f,0x3e,0x3c,0x3c,0x3c,
   0x3e,0x3f,0x1f,0x0f,0x03,0x00,0x00,0x00 
};*/

unsigned char sSelektpayNECLogo_Display[]={
	0x04,
	0x00,0x75, 
	0x00,0x00,0x00,0x00,0x40,0xf0,0xf0,0xf0,0xe0,0x00,0x08,0x1e,0x1e,0x1e,0x00,0x00,
	0x1c,0x1e,0x3e,0x1e,0x1c,0x00,0xc0,0xe0,0xe0,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xfc,0xfc,0xfc,
	0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,
	0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,
	0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,
	0xfc,0xfc,0xfc,0xfc,0xfc,
	0x00,0x75, 
	0x00,0x3c,0x3c,0x3c,0x3c,0x00,0x01,0x00,0xc0,0x40,0x40,0xc0,0x00,0xc0,0x40,0x40,
	0xc0,0x80,0xf8,0xf8,0x80,0xc0,0x40,0xc1,0xc1,0x89,0xf8,0xf8,0x00,0xc0,0xc0,0x40,
	0xe0,0xe0,0x40,0x40,0x40,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0xc0,0x40,0xc0,0xc0,
	0x40,0xc0,0x40,0x00,0x40,0xc0,0x40,0x08,0x78,0x78,0x78,0x78,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03,0x03,0xf3,0xe3,0xc7,0x0f,
	0x7f,0x7f,0xff,0xff,0x03,0x7b,0x7b,0x7b,0x7b,0x7b,0x7b,0x7b,0x7b,0x0f,0x0f,0xe3,
	0xf3,0xfb,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,
	0x00,0x75, 
	0x00,0x0f,0x1f,0x1f,0x8f,0xc0,0xc0,0x88,0x09,0x1b,0x1b,0x1f,0x0e,0x0f,0x09,0x19,
	0x19,0x09,0x1f,0x0f,0x1f,0x0f,0x99,0x99,0x89,0x19,0x0f,0x1f,0x0f,0x0f,0x0c,0x18,
	0x0f,0x0f,0x18,0x19,0x40,0x7f,0x7f,0x48,0x08,0x0d,0x0f,0x0e,0x0f,0x0b,0x09,0x0f,
	0x08,0x67,0x7f,0x1e,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0xc0,0xff,0xff,0xff,0xff,
	0xfe,0xf8,0xf1,0xc1,0xc0,0xc0,0xce,0xee,0xce,0xce,0xce,0xce,0xdf,0xff,0xfe,0xe7,
	0xef,0xcf,0xcf,0xcf,0xcf,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,
	0x00,0x75, 
	0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x03,0x00,0x18,0x3c,0x3c,0x3c,0x1c,0x00,0x00,
	0x3c,0x3c,0x3c,0x3c,0x00,0x01,0x07,0x07,0x07,0x07,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x3f,
	0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
	0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
	0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
	0x3f,0x3f,0x3f,0x3f,0x3f 
};
//added by Allen for ZAIN 2016-05-06
unsigned char sZAIN_disp[]={
0x04,
0x00,0x75, 
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x30,0x38,
   0x18,0x1c,0x8c,0x8c,0xce,0xce,0xc6,0xe6,0xe6,0xe7,0x63,0x60,0xf0,0xf0,0xf0,0xf0,
   0xf0,0x70,0x60,0xe0,0xe0,0xe0,0xe0,0xc0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,
0x00,0x75, 
   0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x30,0x00,0x00,0x00,0x00,0x00,0xf0,0xf8,0xfc,
   0xfe,0x3f,0x0f,0xf7,0xfb,0xfd,0xff,0xff,0x8f,0x07,0x07,0x03,0x03,0x03,0x01,0x01,
   0x03,0x03,0x03,0x07,0x8f,0xfe,0xfd,0x01,0x03,0x07,0xff,0xfe,0x78,0x00,0x00,0x00,
   0x00,0x00,0x00,0x18,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0xb8,0xb8,0xf8,0xf8,0xf8,
   0xf8,0xf8,0xf8,0x70,0x00,0x80,0xc0,0xe0,0xf0,0xf0,0xf8,0x78,0x78,0x38,0x38,0x38,
   0x38,0x78,0x78,0xf8,0xf0,0xf0,0xe0,0xc0,0x00,0x00,0x00,0xfb,0xff,0xff,0xff,0xf3,
   0x00,0x00,0xf0,0xf8,0xf8,0xf8,0xf8,0xf8,0xf0,0xe0,0xe0,0xc0,0x80,0x00,0x00,0x00,
   0x00,0xf8,0xf8,0xf8,0xf0,
0x00,0x75, 
   0x00,0x00,0x00,0xe0,0xc0,0x00,0x7f,0xff,0xf8,0xe0,0xc0,0x80,0x00,0x07,0x0f,0x1f,
   0x3f,0x7e,0x7c,0x78,0x71,0xf1,0xf1,0xf1,0xf1,0xe1,0xf1,0xf0,0x70,0x70,0xf0,0xf8,
   0xfc,0xdc,0xde,0xef,0xf7,0xf3,0xf1,0x78,0x3e,0x1f,0x0f,0x03,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x70,0xf8,0xfc,0xfc,0xfe,0xfe,0xff,0xef,0xef,0xe7,0xe7,0xe3,0xe1,
   0xe1,0xe0,0xe0,0xc0,0x00,0x0f,0x3f,0x7f,0x7f,0xfd,0xf0,0xf0,0xe0,0xe0,0xe0,0xe0,
   0xf0,0xf8,0x7c,0x38,0x03,0xff,0xff,0xff,0xff,0x00,0x00,0x7f,0xff,0xff,0xff,0xff,
   0x00,0x00,0x3f,0xff,0xff,0xff,0xff,0x00,0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7e,0xfc,
   0xf8,0xff,0xff,0xff,0x7f,
0x00,0x75, 
   0x00,0x00,0x00,0x01,0x07,0x0e,0x1c,0x38,0x39,0x7b,0x73,0x77,0xff,0xff,0xee,0xee,
   0xee,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0x7e,0x7e,0x7e,0x3f,0x3f,0x3f,0x1f,0x1f,0x0f,
   0x0f,0x07,0x07,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00 
};

//add end 




extern INT ghClock;
int h_modem; //!!!sel??? sonradan ekledim
int     h_clock;                /* CLOCK Handle                     */


typedef struct {
    usint        Version;    /* Structure version               */
    BOOLEAN      IsBatExist; /* Is there a battery sub-system ? */
    BYTE         BatEnergy;  /* Battery energy ( 1% - 100% )    */
    BYTE         BatState;   /* Battery state                   */
    BYTE         Spare[16];  /* Reserved for future use         */
} CpuhwBatSysType;

#define VOID void
#define CHAR char

//int     h_console;              
unsigned char emv_off_pin_try;
unsigned char emv_off_pin_entry;
#define SCREEN_MAX_ROW Get_ScreenMaxRowCol(0)
#define SCREEN_MAX_COL Get_ScreenMaxRowCol(1)
#define ALIGN_LEFT   0 /*!< \brief Align left */ 
#define ALIGN_CENTER 1 /*!< \brief Align center */ 
#define ALIGN_RIGHT  2 /*!< \brief Align right */ 

unsigned char ucDoNotHonorFlag = 0;

usint menu_strnlen(char *Buf, usint MaxLen);

usint strnlen(char *Buf, usint MaxLen);
WORD disp_logo(void);

char Create_Update_ESTFile_Key(struct sIndex *index_arr, int index_len);
char Create_Update_ESTFile_Config(struct sIndex *index_arr, int index_len);
int Create_Update_MVTFile(struct sIndex *index_arr, int *index_len);
void vSetDefaultFunctionPointers(void);
BYTE Get_ScreenMaxRowCol(BYTE RowCol);

extern void AddAllVSDCtestKeys(void);
/*
#ifndef DEVELOP_VERSION

EMVResult usEMVPerformOnlinePIN(void);
EMVResult getLastTxnAmt(Ulong *amt);
EMVResult usEMVPerformSignature(void);
EMVBoolean bIsCardBlackListed(BYTE * pan, unsigned short panLen, BYTE * panSeqNo, unsigned short panSeqLen);

#endif
*/
void usEMVDisplayErrorPrompt(unsigned short errorID);
//short getUsrPin(unsigned char *pin);
short getTermUsrPin(unsigned char *pin);
short readRecord(int fHandle , char *buf);
//int inGetPTermID(char *ptid);
BYTE Emv_ApplSelect(BYTE *pbErr, WORD wTransNo);
void vdPromptManager(unsigned short inCondition);
int inMenuFunc(char **labels, int numLabels);
unsigned short usAmtEntryFunc(unsigned long *lnAmountValue);
void hex2str(BYTE *outp, BYTE *inp, int length);
BYTE Emv_InitiateAppProcessing(void);
BOOLEAN EmvDb_Find(unsigned char  *value, WORD *length, usint usTag);
BYTE EmvDb_StoreData(usint usTag, unsigned char  *value,unsigned short  length);
BYTE Emv_ReadApplicationData(void);
BYTE EMV_GetCardInfo(void);
static BYTE Card_ReadTrackI(BYTE *trackData);
BYTE Emv_AuthOfflineData(void);
BYTE Emv_VerifyCardHolder(void);
int dsp_func(char *value,int mx,int mn,int typ,int punc,int frac);
int SVC_KEY_NUM1(char *value, int type, int mx,int mn);
void display_format(int y,char *msg_str,int align);
BYTE Enter_data(BYTE *chTemp,BYTE Type);
//int i_display_graphic(unsigned char *bp_graphic_name,int b_clr,int i_col,int i_row);
BYTE SendCommand(BYTE *Command, BYTE Len, BYTE *Result);
void FormatAmt(char *pDest, char *pBuf, char *CurDes, BYTE InputType);
void FormatPoints(char *pDest, char *pRes);  //added by Allen 2016-07-12
void GetTotalBalance(char *pPointsBlance);   //added by Allen 2016-07-13
void Disp_Mess(BYTE Type,BYTE Timeout);
BYTE AskYesNo(MSG_Struct *stMSG,BYTE Type,BYTE offset);
BYTE Emv_AnalyzeTerminalAndCardAction( void);

BYTE EMV_Complete(BYTE bCommResult);
BYTE Emv_ProcessOnlineHostData(BYTE bCommResult);

//void DumpBatteryStatus(CpuhwBatSysType batStatus);
void DumpSignalLevel(BYTE byLevel);
void Main_IdleScreen(void);
void Print_test(void);

short terminate_Transaction;
extern void s_GetMatrixDot(uchar *str,uchar *MatrixDot,uint *len, uchar flag);


static s_display_barcode(BYTE *pszBarcode)
{
	ScrClrLine(4, 7);
	ScrPrint(55, 5, ASCII, "(A)");
	ScrPrint((128-8*StrLen(pszBarcode))/2, 6, CFONT, pszBarcode);
}

WORD Enter_prod_type(unsigned char *psProdType)
{
    MSG_Struct  stMsg;

    memset(&stMsg, 0, sizeof(stMsg));
	strcpy(stMsg.CSTMSG1, "TYPE OF GOOD(S)");
	sprintf(stMsg.CSTMSG2, "FREE TEXT UP TO %dCH", (int)LG_PRODUCT_TYPE);

	OS_ClearScreen();
    kbflush();
    if (!InputAlphaNum_PAX(&stMsg, psProdType, 0, LG_PRODUCT_TYPE, OS_TRUE, JUSTIFY_CENTER, 30))
    {
        return STAT_NOK;
    }
    
    return STAT_OK;
}

WORD Enter_scan_barcode(unsigned char *psBarcode)
{
	unsigned char szEnterBuff[32], szRecvBuf[256];
	unsigned char ucKey, ucRet;
	int	iStrlen, iRecvCnt;
	int iLen=0, iLastKey,iIndex;

	OS_ClearScreen();
    ScrPrint(0, 0, CFONT|REVER, "BARCODE        ");
    ScrPrint(0, 2, ASCII, "SCAN BARCODE");
    ScrPrint(0, 3, ASCII, "OR MANUALLY KEY IN");

	PortOpen(PINPAD, "38400,8,n,1");
	OS_KeybdFlush();
	memset(szEnterBuff, 0, sizeof(szEnterBuff));
	memset(szRecvBuf,0,sizeof(szRecvBuf));
	iStrlen = 0;
	iRecvCnt = 0;
	TimerSet(TIMER_TEMPORARY, 600);
	s_display_barcode(szEnterBuff);
	iLastKey = -1;
	iIndex = 0;
	while (1)
	{
		ucKey = Kb_Read();
		if (ucKey!=NOKEY)
		{
			if (ucKey>=KEY0 && ucKey<=KEY9)
			{
				if (iStrlen<13)
				{
					szEnterBuff[iStrlen] = ucKey;
					iStrlen++;
					iLastKey = ucKey - KEY0;
					iIndex   = 0;
				}
			}
			if (ucKey==KEYCLEAR)
			{
				/*if (iStrlen>0)
				{
					szEnterBuff[iStrlen-1] = 0;
					iStrlen--;
				}*/
				if( iStrlen<=0 )
				{
					continue;
				}
				szEnterBuff[--iStrlen] = 0;
				if( iStrlen>0 )
				{
					MapChar2Index(szEnterBuff[iStrlen-1], &iLastKey, &iIndex);
				}
				else
				{
					iLastKey = -1;
					iIndex   = 0;
				}
			}
//by jarod 2011.06.13
//Shouldn¡¯t accept entry of characters other than number (0..9) and letters (A..Z) or (a..z) in the barcode entry.
#if 1
			if( ucKey==KEYALPHA )
			{
				if( iLastKey<0 || iStrlen<1 )
				{
					continue;
				}
				iIndex = (iIndex+1)%strlen(pszAlphaTbl[iLastKey]);
				if((pszAlphaTbl[iLastKey][iIndex]>='A'&&
					pszAlphaTbl[iLastKey][iIndex]<='Z')||
					(pszAlphaTbl[iLastKey][iIndex]>='a'&&
					pszAlphaTbl[iLastKey][iIndex]<='z'))
				szEnterBuff[iStrlen-1] = pszAlphaTbl[iLastKey][iIndex];
			}
#endif
			if (ucKey==KEYCANCEL)
			{
				return STAT_NOK;
			}
			if (ucKey==KEYENTER && iStrlen>0)
			{
				break;
			}
			s_display_barcode(szEnterBuff);
			TimerSet(TIMER_TEMPORARY, 600);
			continue;
		}

		ucRet = PortRecv(PINPAD, szRecvBuf+iRecvCnt, 50);
		if (ucRet == 0)
		{
			iRecvCnt++;
			while (2)
			{
				ucRet = PortRecv(PINPAD, szRecvBuf+iRecvCnt, 50);
				if (ucRet)
				{
					break;
				}
				if (szRecvBuf[iRecvCnt] == 0x0d)
				{
					ucRet = 0;
					szRecvBuf[iRecvCnt] = 0;
					break;
				}
				iRecvCnt++;
			}

			if ((ucRet==0) && (StrLen(szRecvBuf)))
            {
                s_display_barcode(szRecvBuf);
                sprintf(szEnterBuff, "%.13s", szRecvBuf);
				DelayMs(500);
				break;
			}
			TimerSet(TIMER_TEMPORARY, 600);
            iRecvCnt = 0;
            memset(szRecvBuf, 0, sizeof(szRecvBuf));
			continue;
		}

		if (TimerCheck(TIMER_TEMPORARY)==0)
		{
			return STAT_NOK;
		}
	}

	sprintf((char *)psBarcode, "%.13s", (char *)szEnterBuff);
	return STAT_OK;
}
//added by Allen 2016-05-24
WORD EnterPIN(BYTE *PIN)
{
    MSG_Struct MainMSG;
    BYTE cnt;
	ClearCustomMessages(&MainMSG);
    strcpy(MainMSG.CSTMSG1, "PLS ENTER PIN");
	

	OS_ClearScreen();
	cnt = InputPINNum(&MainMSG, PIN);
	if (!cnt)
    {
		return(STAT_NOK);
    }
	return (STAT_OK);
}
//add end

WORD Enter_Item_Price(BYTE *psPriceBcd)
{
    MSG_Struct MainMSG;
    BYTE chTemp[32], cnt;
    
    
    ClearCustomMessages(&MainMSG);
    strcpy(MainMSG.CSTMSG1, "Redemption");//modify by Allen for ZAIN 2016-05-03
	strcpy(MainMSG.CSTMSG2, "Amount:");//added by Allen for ZAIN 2016-05-18
	
	OS_ClearScreen();
    memset(chTemp, 0, sizeof(chTemp));
    cnt = InputAmountExt(&MainMSG, chTemp, 0, 1, 7, gCurrencySymbol);
	if (!cnt)
    {
		return(STAT_NOK);
    }

    memcpy(psPriceBcd, chTemp, 6);
	return (STAT_OK);
}

WORD Enter_Item_num(WORD *pwNum)
{
    MSG_Struct MainMSG;
    BYTE    chTemp[32];
    WORD    wTemp;
    
    ClearCustomMessages(&MainMSG);	
    memset(chTemp, 0, sizeof(chTemp));

    strcpy(MainMSG.CSTMSG1, "  NUM OF ITEMS  ");
 //   strcpy(MainMSG.CSTMSG2, "   [3 DIGITS]   ");
    MainMSG.CSTMSG8[0] = NOCLEARDISP; 
	
    while (1)
    {
	    OS_ClearScreen();
        if (!InputAlphaNum_PAX(&MainMSG, chTemp, 1, 3, FALSE, JUSTIFY_CENTER, /*TO_30SEC*/TO_1MIN))
        {
		    return(STAT_NOK);
        }

        wTemp = (WORD)atoi(chTemp);
        if (wTemp)
        {
            break;
        }
    }
    *pwNum = wTemp;
	return (STAT_OK);
}

BYTE AskAnotherGoods(void)
{
    BYTE    ucKey;

    OS_ClearScreen();

//    ScrPrint(0, 0, CFONT|REVER, " WOULD YOU ADD ");
//    ScrPrint(0, 2, CFONT|REVER, " ANOTHER ITEM ? ");
	ScrPrint(0, 0, CFONT|REVER, "ADD MORE ITEMS ?");

    ScrPrint(0, 4, CFONT, "   [1] - YES");
    ScrPrint(0, 6, CFONT, "   [2] - NO");
    while (1)
    {
        ucKey = GetOneKey(60);
        if ((ucKey==NOKEY) || (ucKey==KEY2))
        {
            return OS_FALSE;
        }
        if (ucKey==KEY1)
        {
            return OS_TRUE;
        }
    }
}


VOID InitPAX(void)
{
	SystemInit();
	//OS_InitClockVx();
	//OS_DisplayOpenVx();
	//OS_PrinterOpenVx();
	//OS_OpenMagReader();
	//OS_ICCOpenVx();
    OS_ClearScreen();

	// Check and init arabic font here.
	gLanguage = ENGLISH;
	if (fexist("AR_PRNDISP.FNT")<0)
	{
		OS_ClearScreen();
		ScrPrint(0, 0, CFONT|REVER, "  MISSING FONT  ");
		ScrPrint(0, 4, CFONT, "PLEASE LOAD     ");
		ScrPrint(0, 6, CFONT, "AR_PRNDISP.FNT  ");
		while (1);
	}
	ArInit(3);

	ConnectInit();  //added by Allen for ZAIN 2016-05-19
	EndOfDayFlagInit();  //added by Allen for ZAIN 2016-05-23

	MagOpen(); //added by Allen for ZAIN 2016-05-13
	MagReset();//added by Allen for ZAIN 2016-05-13
}

#pragma warning (disable: 4700) // Added by Kim_LinHB 2013-3-11 build 0.05.0009
void SLE44x2_AtrTest( void )
{
	SC_READER r;
	CardConnect(r);
}
#pragma warning(default: 4700) // Added by Kim_LinHB 2013-3-11 build 0.05.0009

void ChangePinPadLogo(unsigned char *str, int line, int block)     //added by Allen 20151125
{
	unsigned char MatrixDot[5000],Logo[244],temBuf[16];
	unsigned short DotLen;	
	unsigned char strLen, ucRet;
	int i;

	memset(Logo, 0x00, sizeof(Logo));
	strLen=strlen(str);
	for(i=0; i<strLen; i++)
	{
		s_GetMatrixDot(str+i, temBuf, &DotLen, 1);
		memcpy(&MatrixDot[i*8], temBuf, 8);
		memcpy(&MatrixDot[(strLen+i)*8], temBuf+8, 8);
	}
	
	memcpy(Logo+block*8, MatrixDot, strLen*8);
	memcpy(Logo+122+block*8, MatrixDot+strLen*8, strLen*8);
	ucRet = PPUpdLogo(line,Logo);
	if (ucRet)
	{
		return;
	}
}


int main(void)
{
	int iRet;
    long events = 0 ;
    unsigned long ulScanTimer = 0;
    int drw = 0;
    unsigned char key;

    CHAR szTemp[50]={0};
                //char vStr[32];
    int index_len;
    struct sIndex index_arr[32];
	unsigned char szTime[7];

         //HDGStruct lsHDGBuf;    
                //gDebugType = DEBUG_PORT;
	
    InitPAX();    
                // Set the function pointer of EMV Library callback functions
                //vSetDefaultFunctionPointers();

  //  EMVCoreInit();
    //AddAllVSDCtestKeys();
                // Gary: delete below
                //memset(vStr,0,sizeof(vStr));
                //usVXEMVAPGetVersion(vStr);
                //inLoadMVTRec(0);
                //inVXEMVAPSCInit();

	//PPScrClrLine(1);
	//PPScrPrint(1, 0, "SELEKTPAY - NEC");
	//ChangePinPadLogo("WELCOME!", 1, 3);
	//ChangePinPadLogo("SELEKTPAY - NEC", 2, 0);
    
#ifdef OUTPUT_EMV_DEBUG
    EMVSetDebug(1);
#endif
	//DebugComSend("gConnectMode = %d\n", gConnectMode);
    if(gConnectMode == GPRS)     //modified by Allen 2016-05-19
	{
#ifndef APPL_IP
		UCL_MobileInitialize(COMMS_GPRS);		
#endif
	}

    TimerSet(0, 0);
	TimerSet(4, 6000); //added Allen (check EODFlag Once every 10 min)  
    ScrCls();
	//DebugComSend("------------Welcome ZAIN app ------\n");
    while (1) 
    {
        if (TimerCheck(0)==0)
        {
            Main_IdleScreen();
            TimerSet(0, 10);
        }
		//added by Allen 2016-05-23
		if(TimerCheck(4) == 0)
		{
			GetEODFlag();
			GetTime(szTime);
			//DebugComSend("szTime = %02x:%02x\n", szTime[3], szTime[4]);
			if((szTime[2] != gEODParam.szDateTime[2]) && gEODParam.EODFlag == 0)
			{
				gEODParam.EODFlag = 1;
				SaveEODFlag();
			}
			/*modified by jeff_xiehuan20161218*/
			//if(gEODParam.EODFlag == 1 && (szTime[3] == '\x23' && szTime[4] >= '\x30'))
			//{
			//	if(OS_TRUE == StartBatchClose())
			//	{
			//		gEODParam.EODFlag = 0;  //0 - no need to do end of day 
			//		memcpy(gEODParam.szDateTime, szTime, 7);
			//		SaveEODFlag();
			//	}
			//}
			TimerSet(4, 6000);
		}
        //add end
        if (kbhit()==0) 
        {
            if (OS_KeybdGet(&key, 0)==OS_OK)
            {
                switch (key)
                {
                case KEYMENU:
                    M_MerchantMenu();
                    break;	
                case KEYFN:
                    M_ApplicationCheck();
                    break;
//#ifdef DEVELOP_VERSION
				case KEYCANCEL:
                    //add by richard 20150715
					if(gAppEntryMothod == MAINAPP_ENTRY)
					{
				    	return 0;
                    }
                    /*if (GetOneKey(1)==KEYCANCEL)  //remove by richard 20150703  need return to MainApp
					{
						Reboot();
					}*/
//#endif
                }
#if 0       
                // Gary notes : original debug codes ?
                if (key == '1')
                {
                    Reboot();
                }
                if (key == '2')
                {
                    disp_logo();
                }
                OS_ClearScreen();
                if (key == '3')
                {
                    Print_test();
                }
#endif
			    ScrCls();
                kbflush();
                Main_IdleScreen();
            }            
        }
        else if ( MagSwiped()==0 )
        {
			memset(&gPTStruct, 0, LG_PTSTRUCT);     
			memset(&Magmsg, 0, sizeof(ST_MAGCARD));
			OS_ClearScreen();
            OS_Beep(200);
            ScrGotoxy(4, 0);

			ScrDrLogo(sZAIN_disp);

			ScrPrint(0, 4, ASCII, "Welcome to Zain Rewards");

			Magmsg.RetCode = MagRead(Magmsg.track1, Magmsg.track2,Magmsg.track3);
			////DebugComSend("track1  = %s\n", Magmsg.track1);
			////DebugComSend("track2  = %s\n", Magmsg.track2);
			//DebugComSend("track3  = %s\n", Magmsg.track3);
			//Select_Calculation();
			//StartApplication((PBYTE)" ");
			//Parse47_Points(NULL); 
			SaleTransaction(OS_TRUE);
			ScrCls();
            Main_IdleScreen();
        }
		//   removed by Allen 2016-05-06
		/*
        else if ( (key=IccDetect(0))!=0xFF )
        {
            memset(&gPTStruct, 0, LG_PTSTRUCT);            
                        // remove "splitsales.log"
            Create_Update_MVTFile(index_arr, &index_len);     //change some param, and add app 
                        //remove(EmvESTFile);
                        //Create_Update_ESTFile_Key(index_arr, index_len);
                        //Create_Update_ESTFile_Config(index_arr, index_len);            
                        //inLoadMVTRec(0);
                        //inVXEMVAPSCInit();				
                        //SLE44x2_AtrTest();
            OS_ClearScreen();
            OS_Beep(200);
            
			// "Welcome to E-Murabaha service from ADIB"
            // "Welcome to Taqseet Card Services from alBaraka Islamic Bank"
            ScrGotoxy(4, 0);
            //ScrDrLogo(sAlBarakaLogo_Display);
			ScrDrLogo(sZAIN_disp);
			//modify by lqc 20150617
			ScrPrint(0, 4, ASCII, "Welcome to Zain Rewards");
			//ScrPrint(0, 5, ASCII, "Taqseet Card Services");
			//ScrPrint(0, 6, ASCII, "        from");
			//ScrPrint(0, 7, ASCII, "AlBaraka Islamic Bank");

            SaleTransaction(OS_FALSE);

            //test by richard 
            //OS_ClearScreen();
            //iRet = WlPppCheck();
            //ScrPrint(0, 0, ASCII, "richard test");
            //ScrPrint(0, 2, ASCII, "GPRS status:");
            //ScrPrint(0, 6, ASCII, "     %d", iRet);
            //getkey();
            //OS_ClearScreen();

			PF_CardRemove();
			ScrCls();
            Main_IdleScreen();
        }
		*/
		DelayMs(50);
    }
}

void Main_IdleScreen(void)
{
	char Time[50] ={0};	

	OS_ClkReadDateTime((PBYTE)Time);
	ScrPrint(0,  0, CFONT|REVER, "%-16.8s", Time);
	ScrPrint(78, 1, ASCII|REVER, "%.8s", Time+11);

    ScrPrint(0, 2, CFONT, "  Zain Rewards  ");
    ScrPrint(0, 4, CFONT, "SWIPE CARD");
    ScrPrint(0, 6, CFONT, "OR SELECT MENU");
}

BYTE EMV_StartTransaction(BYTE transactionType, WORD wTransNo)
{
    int iRet, iTemp;
	int terminalRecord = 0;
	BYTE byErr=0, rv;
	BYTE buf[30],TmpStr[30],bTranType;
	TERMCONFIGSTRUCT term;	
	HDGStruct lsHDGBuf;
	unsigned short result;
	BYTE i;
	MSG_Struct MainMSG;
	unsigned long ulAmBin;
	usint usTmpLen;
	WORD sw;
#ifdef NEED_DEBUG
    BYTE    debug_buffer[1024];
    int     iDebug;
#endif
    EMV_PARAM   tParam;

	TR.EMVData.trType = 0x00;
	
	if (HDGRead(&lsHDGBuf)!=STAT_OK)
    {
		return(2);
    }

	memset(gCardParam.Balance ,0, 6);

#ifdef APPL_DEMO	
    memcpy(gCardParam.Balance, "\x00\x00\x05\x00\x00\x00", 6);
    memcpy(gPTStruct.Balance,"000005000000",LG_AMOUNT );
    memcpy(gPTStruct.FirstBalance,gPTStruct.Balance,LG_AMOUNT );
    
    memcpy(gCardParam.UpperLimit,"\x00\x00\x05\x00\x00\x00", 6);
    gCardParam.LoyaltyFlag=1;
    
    /*	if (Select_Calculation()!=STAT_OK) { 
    return (2);
    }*/
#endif

    // Gary
    // !!!!!!!! set app/capk data here !!!!!!!

// 	result=inVXEMVAPTransInit(terminalRecord,inGetPTermID);
// 	if (result != SUCCESS) return(2);
// 	if (inVXEMVAPCardInit() != SUCCESS) return(2);	
    if (Emv_ApplSelect(&byErr, wTransNo)!= 0)
    {
        return 2;
    }

    // TERM ID and serial number
	GetTerminalConfig(&term);
// 	EmvDb_StoreData(tagIFD_SERIAL_NUM, term.SerialNum,8);
// 	EmvDb_StoreData(tagTERML_ID, lsHDGBuf.TermId,8);
    memset(&tParam, 0, sizeof(tParam));
    EMVGetParameter(&tParam);
    memcpy(tParam.TermId, lsHDGBuf.TermId,8);
    EMVSetParameter(&tParam);

    // transaction sequence counter
	//memcpy(buf, "\x00\x00\x00\x01", 4);			
	//EmvDb_StoreData(tagTRAN_SEQUENCE_COUNTER, buf,4);
    EMVSetTLVData(0x9F41, "\x00\x00\x00\x01", 4);

    /*
	rv = Emv_InitiateAppProcessing();
	if (rv != EMV_OK)
    { 
            // belows are originally deleted.
            fallback
			if (byEntryType==ENTRY_CHIP || byEntryType==ENTRY_CHIP_AXESS)
            {
				Fast_UpdateTag(tagFALLBACK_FLAG,1, 1, "\x01");
				Fast_UpdateTag(tagFALLBACK_TYPE,1, 1, "\x02");
			}
		return 2; 
	}
    */

	if (Emv_ReadApplicationData() != 0) 
	{
		//EventUtils_RemoveChipCard();
		//EmvIF_ClearDsp(DSP_MERCHANT);
		return 2;
	} 

	if ((i = EMV_GetCardInfo()) != 0)
    {
		if (i==1)
		{
			DisplayString(DSP_INVALID_CARD,3000);
			return (2);
		}
		return (2);
	}

    // PAX EMV lib processing step : data authentication
    iRet = EMVCardAuth();
    if (iRet!=0)
    {
        return 2;
    }

    //----------------------------------------------------
    // Gary : PROCCO do not need blacklist	
// 	if ( Check_BlackList()==STAT_OK)
//     {
// 		//ONLINE  
// 		memcpy(gPTStruct.TransType,TYPE_73,2);
// 		memset(gPTStruct.MonthlyInstAmount,'0',LG_AMOUNT );
// 		memset(gPTStruct.EntryAmount,'0',LG_AMOUNT);
// 		memset(gPTStruct.TotalAmount,'0',LG_AMOUNT);
// 		memset(gPTStruct.BcdAmount,'\x0',6);		
// 	}
// 	else
//  {
		if (Select_Calculation()!=STAT_OK)//leeDeug
        { 
			return (2);
		}
//	}
	 
    // Gary : done by EMVProcTrans()
// 	if (Emv_AuthOfflineData() == EMV_TERMINATE_TRN)
//     {
//         return 2;
//     }

	//store amount 

    // Gary : done by callback function : cEMVInputAmount()
// 	memcpy(buf,gPTStruct.TotalAmount,12);
// 	buf[12]=0;	
// 	Utils_ConvertAscToBcd(buf, TmpStr, 12, 0, PAD_NONE,0);
//                 OS_LogToHex("!!!!tagAUTH_AMOUNT_NUM", TmpStr, 6,DEBUG_PORT);   
// 	EmvDb_StoreData(tagAUTH_AMOUNT_NUM,TmpStr,6);

    // Gary : done by callback function : cEMVInputAmount()
// 	ulAmBin=atol(buf);
// 	memcpy(buf,&ulAmBin,4);
// 	EmvDb_StoreData(tagAUTH_AMOUNT_BIN,buf,4);
	
    bTranType = 0;
	EmvDb_StoreData(tagTRAN_TYPE, &bTranType, 1);

    // Gary : done by EMVProcTrans()
// 	result=inVXEMVAPProcRisk(bIsCardBlackListed);
// 	if (result != SUCCESS)
//  {
//      return (2);
//  }

    //force online 
// 	usTmpLen = usEMVSetForceOnlineBit();
// 	if (usTmpLen != EMV_SUCCESS)
//  {
//      return (2);
//  }
    EMVGetParameter(&tParam);
    tParam.ForceOnline = 1;
    EMVSetParameter(&tParam);

// 	result=Emv_VerifyCardHolder();
// 
//     {
// 	    unsigned short  d_length;
// 	    BYTE dummy[50];	
// 	    memset(dummy,0,50);
// 	    EmvDb_Find(dummy,&d_length, tagTVR);
//     OS_LogToHex("TVR", dummy, d_length,DEBUG_PORT); 
//     }
// 
// 	if ( result== EMV_TERMINATE_TRN)
//         return (2);
    
	// Modified by Kim_LinHB 2013-10-14 0.05.0014 remove 0 for declining Bypass
    

// 	switch(Emv_AnalyzeTerminalAndCardAction())
//     {
// 		case EMV_TERMINATE_TRN: 
// 		case EMV_DECLINED:
// 			DisplayString(DSP_REJECTED,3000);
// 			return(2);
// 		case EMV_ONLINE:
// 			break;
// 		case EMV_APPROVED:
// 			//??????
// 		
// 			return(2);
// 		case EMV_FALLBACK:
// 			return(2);
// 		default: /* EMV_TERMINATE_TRN */
// 			DisplayString(DSP_REJECTED,3000);
// 			return(2);
// 	}

//remove by richard 20150703
/*
    if (memcmp(gPTStruct.TransType, TYPE_70, 2)==0)
    {
        // Screen 12
        if (AlBarakaMerchantConfirm(1)!=STAT_OK)
        {
            return 2;
        }

        // Screen 13
        if (AlBarakaMerchantConfirm(2)!=STAT_OK)
        {
            return 2;
        }

        // Screen 14
        if (AlBarakaMerchantConfirm(3)!=STAT_OK)
        {
            return 2;
        }
    }*/  

    OS_ClearScreen();
   if (memcmp(gPTStruct.TransType,TYPE_72,2)==0)
    {
    	iRet = (int)ISO_SendReceive(OS_TRUE, ISO_PAYMENT);
		Comms_Disconnect();
		if(iRet == STAT_OK)
			iRet = EMV_OK;
		//else
			//iRet = EMV_DENIAL;  //EMV_DENIAL
        
	}
    else
    {
		if (PPScrCls())
		{
			EMVSetPCIModeParam(1, (uchar *)"4,5,6,7,8", 120000);
			usePinPad = 0;
		}
		else
		{
			usePinPad = 1;
		}
		PinCount = 0;		
		iRet = EMVProcTrans();
        //DebugComSend("EMV iRet:%d", iRet);
    }
    //OS_ClearScreen();
    //ScrPrint(0, 0, ASCII, "richard test");
    //ScrPrint(0, 2, ASCII, "send the value to me");
    //ScrPrint(0, 6, ASCII, "return value: %d", iRet);
    //getkey();
    //OS_ClearScreen();

    switch(iRet)
    {
    case EMV_OK:    // approved online or offline
         break;
    case EMV_TIME_OUT:
    case EMV_USER_CANCEL:
    case ICC_CMD_ERR:
        return 2;
    default:
        if(1 == ucDoNotHonorFlag)   //modified by richard 20150818
        {
            ucDoNotHonorFlag = 0;
            return 2;
        }
        
        OS_ClearScreen();
        DisplayLongString_S80(DSP_REJECTED, 5000);
		Print_HostErr_Slip(PRT_REJECTED);

#ifdef DEVELOP_VERSION
            EMVGetTLVData(0x95, buf, &iTemp);
            ScrPrint(0, 5, ASCII, "DEBUG:\nREJECT: %d\n TVR=%02X%02X%02X%02X%02X", iRet, buf[0], buf[1], buf[2], buf[3], buf[4]);
            GetOneKey(15);
#endif
        return 2;
    }

//screen 7
	//Disp_Mess(4,0);

//screen 8
	////if (gLanguage==ARABIC) 
	////if (!Disp_Long_Msg(2))
    ////{
    ////    return(2);
    ////}

	////Disp_Long_Msg(6);
	////Disp_Long_Msg(3);

#ifdef SELMA
	                    else
                        {
		                    
		                    ClearCustomMessages(&MainMSG);	
		                    strcpy(MainMSG.CSTMSG1,"ADIB would like to ");
		                    strcpy(MainMSG.CSTMSG2,"buy the following");
		                    strcpy(MainMSG.CSTMSG3,"Item with the details");
		                    sprintf(MainMSG.CSTMSG4,"code:%s",gPTStruct.product[0].barcode);
		                    memset(MainMSG.CSTMSG4+strlen(MainMSG.CSTMSG4),0x20,21-strlen(MainMSG.CSTMSG4));
		                    memcpy(buf,gPTStruct.TotalAmount,12);buf[12]=0;
		                    FormatAmt(TmpStr, buf, gCurrencySymbol, ASCII_IN);	
		                    sprintf(MainMSG.CSTMSG5,"Price:%.15s",TmpStr+1);
		                    
		                    if (!AskYesNo(&MainMSG,2,0)) return(2);
	                    }	
                    //screen 9
	                    if (gLanguage==ARABIC) 
		                    
	                    else
                        {
		                    ClearCustomMessages(&MainMSG);	
		                    strcpy(MainMSG.CSTMSG1,"As a representative");
		                    strcpy(MainMSG.CSTMSG2,"to ADIB, I hereby");
		                    strcpy(MainMSG.CSTMSG3,"confirm the receiving");
		                    strcpy(MainMSG.CSTMSG4,"of the Items from");
		                    strcpy(MainMSG.CSTMSG5,"the merchant");
		                    
		                    sprintf(MainMSG.CSTMSG6,"code:%s",gPTStruct.product[0].barcode);
		                    memset(MainMSG.CSTMSG6+strlen(MainMSG.CSTMSG6),0x20,21-strlen(MainMSG.CSTMSG4));		
		                    
		                    if (!AskYesNo(&MainMSG,1,0))
                                return(2);
	                    
	                    //screen 11
		                    Disp_Mess(3,2);
		                    
	                    //screen 12
		                    ClearCustomMessages(&MainMSG);
		                    sprintf(MainMSG.CSTMSG1,"code:%.16s",gPTStruct.product[0].barcode);	
		                    memcpy(buf,gPTStruct.EntryAmount,12);buf[12]=0;
		                    FormatAmt(TmpStr, buf, gCurrencySymbol, ASCII_IN);	
		                    sprintf(MainMSG.CSTMSG2,"Principle Price      ");
		                    sprintf(MainMSG.CSTMSG3,"%.21s",TmpStr+1);
	                    
		                    Utils_ConvertBcdToAsc(gPTStruct.ProfitAmountBcd,buf,6);
		                    FormatAmt(TmpStr, buf, gCurrencySymbol, ASCII_IN);	
		                    sprintf(MainMSG.CSTMSG4,"Profit Amount        "); 
		                    sprintf(MainMSG.CSTMSG5,"%.21s",TmpStr+1);
	                    
		                    memcpy(buf,gPTStruct.TotalAmount,12);buf[12]=0;
		                    FormatAmt(TmpStr, buf, gCurrencySymbol, ASCII_IN);	
		                    sprintf(MainMSG.CSTMSG6,"Total Amount         ");
		                    sprintf(MainMSG.CSTMSG7,"%.21s",TmpStr+1);
		                    
		                    memcpy(buf,gPTStruct.MonthlyInstAmount,12);buf[12]=0;
		                    FormatAmt(TmpStr, buf, gCurrencySymbol, ASCII_IN);	
		                    sprintf(MainMSG.CSTMSG8,"Instalment Value     ");
		                    sprintf(MainMSG.CSTMSG9,"%.21s",TmpStr+1);
	                    
		                    if (!AskYesNo(&MainMSG,2,0)) return(2);
	                    }
#endif

	//ask merch PIN
	////if (!Get_MPIN())
    ////{
	////	return (2);
    ////}
	
	////Disp_Long_Msg(4);
		
	////if (!Get_CPIN())
    ////{
	////	return (2);
    ////}

    // Gary : what is this for ????
#ifndef DEVELOP_VERSION
	//Print_test();
#endif

	////Disp_Long_Msg(5);
    //Prepare_Print_Slip(OS_TRUE);	
    ////return (2);
	return 0;
}	

void Get_EMV_tags(void)
{
	BYTE chTmpStr[100];
	WORD usTmpLength;

	if (usEMVGetTLVFromColxn(tagAPPL_CRYPTOGRAM, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.AC,chTmpStr,8);
		
	if (usEMVGetTLVFromColxn(tagCRYPTOGRAM_INFO, chTmpStr,&usTmpLength) == EMV_OK)
		gPTStruct.CID=chTmpStr[0];

	if (usEMVGetTLVFromColxn(tagISSUER_APPL_DATA, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.issuerApplicationData,chTmpStr,32);    // 2011-3-21 : 12->32
		
	if (usEMVGetTLVFromColxn(tagUNPREDICTABLE_NUMBER, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.UN,chTmpStr,4);
		
	if (usEMVGetTLVFromColxn(tagATC, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.ATC,chTmpStr,2);
		
	if (usEMVGetTLVFromColxn(tagTVR, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.TVR,chTmpStr,5);
		
	if (usEMVGetTLVFromColxn(tagTRAN_CURR_CODE, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.CurrencyCode,chTmpStr,2);

	if (usEMVGetTLVFromColxn(tagAIP, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.AIP,chTmpStr,2);

	if (usEMVGetTLVFromColxn(tagTERML_COUNTRY_CODE, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.CountryCode,chTmpStr,2);

	if (usEMVGetTLVFromColxn(tagTERML_CAPABILITIES, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.terminalCapabilities,chTmpStr,3);
		
	if (usEMVGetTLVFromColxn(tagAID_CARD, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(TR.EMVData.DFName,chTmpStr,7);
		
	if (usEMVGetTLVFromColxn(tagCARD_APPL_VERSION, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.cardVersion,chTmpStr,2);		
		
	if (usEMVGetTLVFromColxn(tagCVM_RESULT, chTmpStr,&usTmpLength) == EMV_OK)
		memcpy(gPTStruct.CVMR,chTmpStr,3);
}	

 
void PrepareField55(BYTE Advice, BYTE Batch)
{
	BYTE tempBuffer[32];
	TERMCONFIGSTRUCT term;
	WORD usTmpLength;
	gField55Length=0;

//9F26
	memcpy(gField55+gField55Length,"\x9F\x26\x08",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.AC,8);gField55Length=gField55Length+8;
//9F27
	memcpy(gField55+gField55Length,"\x9F\x27\x01",3); gField55Length=gField55Length+3;
	gField55[gField55Length]=gPTStruct.CID; gField55Length=gField55Length+1;
//9F10
	memcpy(gField55+gField55Length,"\x9F\x10\x20",3); gField55Length=gField55Length+3;  // 0x12->0z20
	memcpy(gField55+gField55Length,gPTStruct.issuerApplicationData,0x20/*TR.EMVData.issuerApplicationDataLen*/);gField55Length=gField55Length+0x20/*TR.EMVData.issuerApplicationDataLen*/;
//9F37
	memcpy(gField55+gField55Length,"\x9F\x37\x04",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.UN, 4); gField55Length=gField55Length+4;
//9F36
	memcpy(gField55+gField55Length,"\x9F\x36\x02",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.ATC, 2); gField55Length=gField55Length+2;
//95
	memcpy(gField55+gField55Length,"\x95\x05",2); gField55Length=gField55Length+2;
	memcpy(gField55+gField55Length,gPTStruct.TVR, 5); gField55Length=gField55Length+5;
//9A
	/*ClkReadDate(tempBuffer,F_YYMMDD);
	tempBuffer[6]=0;*/
	memcpy(gField55+gField55Length,"\x9A\x03",2); gField55Length=gField55Length+2;

    // 2011-3-16
    /*
	if (Batch==OS_TRUE)
	{
		memcpy(tempBuffer  ,gPTStruct.TransDate+6,2);
		memcpy(tempBuffer+2,gPTStruct.TransDate+2,2);
		memcpy(tempBuffer+4,gPTStruct.TransDate  ,2);
		tempBuffer[6]=0;
		Utils_ConvertAscToBcd(tempBuffer, gField55+gField55Length, 6, 0, PAD_NONE,0);
	} 
 	else
    */
    {
		usEMVGetTLVFromColxn(tagTRAN_DATE, tempBuffer,&usTmpLength);
		memcpy(gField55+gField55Length,tempBuffer,3);
	}		

 	gField55Length=gField55Length+3;
		
//9C
	memcpy(gField55+gField55Length,"\x9C\x01",2); gField55Length=gField55Length+2;
	if (Batch==OS_TRUE)
	{
		memcpy(gField55+gField55Length,"\x00", 1);
		gField55Length=gField55Length+1;
	}
	else
	{
		gField55[gField55Length]=TR.EMVData.trType;
		//memcpy(gField55+gField55Length,"\x91", 1); 
		gField55Length=gField55Length+1;
	}

//9F02
    // 2011-3-16
	memcpy(gField55+gField55Length,"\x9F\x02\x06",3); gField55Length=gField55Length+3;
    /*
	if (Batch==OS_TRUE)
	{
		memcpy(gField55+gField55Length,gPTStruct.BcdAmount,6); 
		gField55Length=gField55Length+6;
	}
	else
	{
		memcpy(gField55+gField55Length,"\x00\x00\x00\x00\x00\x00",6); 
		gField55Length=gField55Length+6;
	}*/
	usEMVGetTLVFromColxn(tagAUTH_AMOUNT_NUM, tempBuffer, &usTmpLength);
	memcpy(gField55+gField55Length,tempBuffer,6);
    gField55Length=gField55Length+6;

//9F03
	memcpy(gField55+gField55Length,"\x9F\x03\x06",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,"\x00\x00\x00\x00\x00\x00",6); 
	gField55Length=gField55Length+6;
//5F2A
	memcpy(gField55+gField55Length,"\x5F\x2A\x02",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.CurrencyCode,2); gField55Length=gField55Length+2;
//82  
	memcpy(gField55+gField55Length,"\x82\x02",2); gField55Length=gField55Length+2;
	memcpy(gField55+gField55Length,gPTStruct.AIP, 2); gField55Length=gField55Length+2;
//9F1A
	memcpy(gField55+gField55Length,"\x9F\x1A\x02",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.CountryCode,2); gField55Length=gField55Length+2;
//9F33
	memcpy(gField55+gField55Length,"\x9F\x33\x03",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.terminalCapabilities,3); gField55Length=gField55Length+3;
//4F
	memcpy(gField55+gField55Length,"\x4F\x07",2); gField55Length=gField55Length+2;
	memcpy(gField55+gField55Length,TR.EMVData.DFName,7); gField55Length=gField55Length+7;
	//memcpy(gField55+gField55Length,"\xA0\x00\x00\x00\x04\x30\x60",7); gField55Length=gField55Length+7;
//9F08
	memcpy(gField55+gField55Length,"\x9F\x08\x02",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.cardVersion,2); gField55Length=gField55Length+2;
//9F34
	memcpy(gField55+gField55Length,"\x9F\x34\x03",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,gPTStruct.CVMR,3); gField55Length=gField55Length+3;
//9F35
	memcpy(gField55+gField55Length,"\x9F\x35\x01",3); gField55Length=gField55Length+3;
	memcpy(gField55+gField55Length,"\x22",1); gField55Length=gField55Length+1;
//9F1E
    GetTerminalConfig(&term);
    if (StrLen(term.SerialNum)<LG_SERIALNUM)
    {
        memcpy(term.SerialNum,UTIL_FILLZEROS,LG_SERIALNUM);
    }
    memcpy(gField55+gField55Length,"\x9F\x1E\x08",3); gField55Length=gField55Length+3;
    memcpy(gField55+gField55Length,term.SerialNum,8); gField55Length=gField55Length+8;
//9F09
	//Uygulama Versiyonu
	memcpy(gField55+gField55Length,"\x9F\x09\x02\x00\x02",5); gField55Length=gField55Length+5;
//CB
	if (1)//(Batch==OS_FALSE)
	{
		memcpy(gField55+gField55Length,"\xCB\x06",2); gField55Length=gField55Length+2;
		memcpy(gField55+gField55Length,gCardParam.UpperLimit, 6); gField55Length=gField55Length+6;
	}
}

usint menu_strnlen(char *Buf, usint MaxLen)
{
	usint i;

	if (!Buf)
		return 0;
	for (i = 0; i < MaxLen; ++i) {
		if (Buf[i] == 0x00 || Buf[i] == 0xFF)
			break;
	}
	return (usint)i;
}


int h_print_port=0;

static BYTE prnInversMode = 0; 
unsigned char printBuf[256 + 1];

#define LINE_CHARS	42
#define LINE_BYTES	10

#define RED_STRING            3  /* only one control byte  */
#define ONE_AND_HALF_HIGH_STRING   0x15
#define STRING_FORMAT_SET     6  /* 3 bytes: control byte,LineWidth,LeftMargin*/




unsigned char  wellcome[3][128]={
	{
0X00, 0X38, 0X20, 0X20, 0X30, 0X28, 0X3C, 0X00, 0X70, 0X70, 
0X28, 0X00, 0X00, 0X00, 0XE8, 0X18, 0X0A, 0X08, 0X00, 0X2C, 
0X38, 0XC0, 0X50, 0X30, 0X3C, 0X3C, 0X30, 0X00, 0X00, 0X3E, 
0X70, 0X40, 0X40, 0X3E, 0XC0, 0X50, 0XB0, 0X80, 0X78, 0X00, 
0X00, 0X3E, 0X20, 0XA0, 0X20, 0X38, 0X00, 0XE8, 0X18, 0X08, 
0X08, 0X3C, 0X3C, 0X30, 0X00, 0X00, 0X00, 0X7E, 0X40, 0X40, 
0X3E, 0X00, 0X7E, 0X70, 0X40, 0X40, 0X3E, 0X2E, 0X20, 0X3E, 
0X38, 0X20, 0X24, 0X24, 0X38, 0X00, 0X80, 0X80, 0X78, 0X80, 
0XB0, 0XA8, 0X78, 0XF0, 0X80, 0X84, 0XF8, 0X00, 0X70, 0X70, 
0X28, 0X00, 0X3C, 0X3C, 0X30, 0X00, 0X00, 0XC0, 0X50, 0XF0, 
0X80, 0X84, 0XF8, 0X00, 0X00, 0X00, 0XC0, 0X50, 0XF0, 0X80, 
0X80, 0XF8, 0X20, 0X30, 0X30, 0X28, 0X38, 0X80, 0X80, 0X78, 
0X38, 0X20, 0X20, 0X30, 0X28, 0X3C, 0X00, 0X00},
{ 
0XFC, 0XB0, 0XE0, 0X00, 0XE0, 0X80, 0X80, 0X80, 0XE0, 0X01, 
0XC1, 0XC0, 0XA0, 0X00, 0X00, 0X01, 0X01, 0XF9, 0X00, 0X00, 
0XF8, 0X01, 0XF8, 0X00, 0X00, 0XE0, 0X80, 0XE0, 0XE0, 0X00, 
0XC0, 0X00, 0X00, 0XF8, 0XF9, 0X00, 0X40, 0XC0, 0XC0, 0XC0, 
0XA0, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X01, 
0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 
0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X02, 0X00, 0X00, 0X00, 
0X01, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X01, 0X01, 
0X00, 0X00, 0X03, 0X02, 0X02, 0X03, 0X00, 0X00, 0X00, 0X00, 
0X01, 0X01, 0X01, 0X00, 0X00, 0X03, 0X01, 0X00, 0X01, 0X01, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}
}; 

unsigned char  adib_1Display[8][128]={
	{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XF8, 0X00, 0X00, 
0X80, 0X40, 0XC0, 0X00, 0X50, 0X80, 0X00, 0X00, 0X80, 0X40, 
0XC0, 0X3C, 0XC0, 0X00, 0XC0, 0X00, 0X00, 0X80, 0X90, 0X40, 
0X40, 0XC0, 0X60, 0XA8, 0XE0, 0X10, 0X50, 0X80, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X10, 0X50, 0X80, 
0X3C, 0XC0, 0X00, 0XF8, 0X00, 0X00, 0X00, 0XC0, 0XA4, 0XF4, 
0X00, 0X00, 0X50, 0XC0, 0X00, 0X3C, 0XC0, 0X3C, 0XC0, 0X00, 
0XF8, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X40, 0X80, 0X00, 
0XF8, 0X00, 0X00, 0X40, 0X80, 0X10, 0X50, 0X80, 0X00, 0X40, 
0X40, 0X50, 0X80, 0X00, 0X00, 0XF8, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X40, 0X40, 0X40, 0X80, 
0X00, 0X00, 0X00, 0X00, 0X40, 0X80, 0X00, 0X40, 0X80, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, 
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X01, 0X01, 
0X03, 0X05, 0X07, 0X01, 0X01, 0X09, 0X08, 0X08, 0X09, 0X05, 
0X03, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 
0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X00, 0X00, 
0X00, 0X0F, 0X10, 0X30, 0X36, 0X0D, 0X01, 0X01, 0X01, 0X01, 
0X01, 0X01, 0X00, 0X03, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X01, 0X01, 0X01, 0X00, 0X01, 0X01, 0X01, 0X01, 0X01, 0X00, 
0X03, 0X00, 0X00, 0X00, 0X08, 0X08, 0X08, 0X04, 0X03, 0X00, 
0X01, 0X01, 0X05, 0X05, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 
0X01, 0X01, 0X01, 0X01, 0X00, 0X03, 0X00, 0X00, 0X00, 0X0F, 
0X10, 0X10, 0X16, 0X0D, 0X01, 0X01, 0X01, 0X01, 0X05, 0X01, 
0X01, 0X08, 0X08, 0X08, 0X04, 0X03, 0X05, 0X05, 0X01, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, 
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0XFE, 0X12, 0X12, 0X12, 0X12, 0X00, 0XF8, 0X08, 
0X08, 0X08, 0XF0, 0X00, 0XF0, 0X08, 0X08, 0X90, 0XF8, 0X00, 
0XFE, 0X00, 0XFA, 0X00, 0X90, 0X28, 0X28, 0X48, 0X90, 0X00, 
0XFE, 0X10, 0X08, 0X08, 0XF0, 0X00, 0X00, 0X00, 0X00, 0X40, 
0X40, 0X40, 0X00, 0X08, 0X04, 0XFE, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, 
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X01, 0X01, 0X01, 0X01, 0X01, 0X00, 0X01, 0X00, 
0X00, 0X00, 0X01, 0X00, 0X04, 0X05, 0X05, 0X04, 0X03, 0X00, 
0X01, 0X00, 0X01, 0X00, 0X00, 0X01, 0X01, 0X01, 0X00, 0X00, 
0X01, 0X00, 0X00, 0X00, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X80, 0X81, 0X80, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, 
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XF0, 0X00, 0X00, 0X60, 
0XD0, 0X10, 0X10, 0X54, 0X18, 0X80, 0X80, 0X80, 0X44, 0X38, 
0X10, 0X1E, 0X12, 0X12, 0X08, 0X00, 0X00, 0X00, 0X00, 0X10, 
0X10, 0X10, 0X00, 0X41, 0X60, 0X50, 0X48, 0X47, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X90, 0X30, 0X3C, 0X40, 0X7C, 0X7C, 0X78, 0X40, 0X68, 
0X50, 0X50, 0X70, 0X40, 0X60, 0X78, 0X00, 0X79, 0X03, 0XE3, 
0X80, 0X80, 0XF8, 0X60, 0X50, 0X60, 0X78, 0X40, 0X40, 0X50, 
0X70, 0X40, 0X00, 0X00, 0X78, 0X40, 0X40, 0X70, 0X08, 0X20, 
0XC0, 0XC0, 0XE0, 0X08, 0X20, 0XC0, 0X40, 0X60, 0X60, 0X20, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X70, 0X40, 0X40, 0X7C, 0X60, 0X70, 0X80, 0XB8, 0X78, 
0X30, 0X28, 0X30, 0X28, 0X38, 0X3C, 0X00, 0X3C, 0X00, 0X30, 
0X28, 0X28, 0X20, 0X30, 0X00, 0X3C, 0X38, 0X00, 0X00, 0XBC, 
0X90, 0X60, 0X20, 0X28, 0X78, 0X20, 0X80, 0XBC, 0X00, 0X60, 
0XE0, 0XE0, 0X60, 0X3E, 0X3C, 0X00, 0X00, 0X30, 0X38, 0X00, 
0X20, 0X3C, 0XA0, 0X30, 0X3C, 0X3C, 0X00, 0X00, 0X18, 0X1E, 
0X20, 0X3E, 0X3E, 0X3C, 0X20, 0X34, 0X28, 0X28, 0X38, 0XA0, 
0X30, 0X3C, 0X00, 0X3D, 0X00, 0XE0, 0X30, 0X60, 0X70, 0X20, 
0X3C, 0X3C, 0X3C, 0X28, 0X38, 0XA0, 0X30, 0X00, 0X00, 0XE0, 
0X30, 0X28, 0X34, 0X34, 0X30, 0X28, 0X30, 0X3C, 0X21, 0X61, 
0X70, 0X3E, 0X3E, 0X01, 0X01, 0XE0, 0X88, 0X80, 0XF0, 0X80, 
0XBC, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, 
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}
};

unsigned char  adib_2Display[8][128]={
{
0X00, 0X00, 0X70, 0X18, 0X14, 0X1A, 0X1A, 0X10, 0X1C, 0X18, 
0X14, 0X18, 0X00, 0X00, 0X0C, 0X0F, 0X10, 0X1E, 0X1E, 0X1F, 
0X1E, 0X00, 0X5E, 0X5C, 0X3C, 0X18, 0X14, 0X18, 0X00, 0X00, 
0X40, 0X5C, 0XBC, 0X18, 0X16, 0X18, 0X00, 0X00, 0X30, 0X70, 
0X70, 0XB0, 0X9E, 0X1E, 0X10, 0X14, 0X1C, 0X1E, 0X00, 0X1E, 
0X00, 0X18, 0X1C, 0X00, 0X00, 0X5E, 0X48, 0X30, 0X10, 0X14, 
0X3C, 0X90, 0XC6, 0X6C, 0X30, 0X0E, 0X80, 0X1E, 0X00, 0X70, 
0X50, 0X70, 0XF0, 0X9E, 0X10, 0X18, 0X18, 0X08, 0X00, 0X00, 
0X70, 0X18, 0X14, 0X1A, 0X9A, 0X14, 0X9C, 0X9E, 0X1E, 0X1F, 
0X1E, 0X00, 0X5E, 0X5C, 0X3C, 0X18, 0X14, 0X18, 0X00, 0X00, 
0X40, 0X5C, 0XBC, 0X80, 0X00, 0X0C, 0X0F, 0X10, 0X10, 0X14, 
0X1C, 0X10, 0X50, 0X98, 0X00, 0X5E, 0X48, 0X30, 0X18, 0X14, 
0X18, 0X1E, 0X1E, 0X10, 0X50, 0X18, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X38, 0X22, 0X20, 0X3C, 0X18, 
0X1C, 0X28, 0XCC, 0XA0, 0X2E, 0X9E, 0X00, 0X00, 0X38, 0X8C, 
0XCA, 0XCD, 0X0D, 0X08, 0X0E, 0X18, 0X1C, 0X28, 0X0C, 0X00, 
0X00, 0X06, 0X87, 0X88, 0X0C, 0X0A, 0X0C, 0X8A, 0X8E, 0X0F, 
0X88, 0X0F, 0X0F, 0X00, 0X00, 0X38, 0X22, 0X20, 0X3C, 0X20, 
0XAE, 0X1E, 0X0A, 0X0D, 0X0D, 0X0A, 0X0E, 0X00, 0X00, 0X08, 
0X0F, 0X8F, 0X0F, 0X80, 0X00, 0X06, 0X07, 0X08, 0X98, 0X1C, 
0X08, 0X0F, 0X0F, 0X0F, 0X0F, 0X08, 0X08, 0X0C, 0X08, 0XCA, 
0X0E, 0X00, 0X00, 0X06, 0X07, 0X88, 0X0F, 0X0F, 0X09, 0X0E, 
0X47, 0X08, 0X08, 0X0C, 0X0C, 0X84, 0X00, 0X80, 0X30, 0X38, 
0X1C, 0X0C, 0X40, 0XCF, 0X00, 0X3C, 0XA5, 0X2C, 0X2C, 0X08, 
0X0C, 0X0C, 0X0C, 0X0F, 0X0F, 0X00, 0X00, 0X38, 0X28, 0X38, 
0XB8, 0X0F, 0X08, 0X0C, 0X8C, 0X04, 0X00, 0X00},
{ 
0X00, 0X00, 0XC0, 0XC0, 0XC0, 0XFC, 0XC6, 0X05, 0X07, 0X84, 
0XE6, 0X04, 0XC7, 0X07, 0X04, 0X07, 0X80, 0X00, 0X04, 0X07, 
0X87, 0X07, 0XC0, 0X80, 0XC6, 0X05, 0XC5, 0X04, 0X06, 0X84, 
0X07, 0X05, 0X06, 0X86, 0X00, 0X00, 0X00, 0X67, 0XC7, 0X90, 
0X17, 0X57, 0X0F, 0X00, 0X00, 0X80, 0X88, 0X0E, 0X08, 0X08, 
0X0F, 0X8C, 0X0E, 0X10, 0X17, 0X8F, 0X86, 0X05, 0X06, 0X05, 
0XE7, 0XC7, 0X00, 0XC7, 0X80, 0X8E, 0X08, 0X08, 0X0F, 0X04, 
0X05, 0X07, 0X04, 0X86, 0X85, 0X26, 0XE0, 0X00, 0X03, 0X03, 
0X04, 0X04, 0X86, 0X86, 0X02, 0XC7, 0X04, 0X84, 0X87, 0XC4, 
0XC6, 0X05, 0X07, 0X14, 0X86, 0X07, 0X00, 0X07, 0X80, 0X1C, 
0XD1, 0X90, 0XDE, 0X07, 0XC4, 0X04, 0X07, 0X87, 0X06, 0X14, 
0X06, 0X80, 0X00, 0X10, 0X12, 0X6C, 0XC4, 0X85, 0X0F, 0X44, 
0X07, 0X04, 0X05, 0X87, 0X87, 0X00, 0X00, 0X00},
{ 
0X00, 0X02, 0X03, 0X03, 0X03, 0X03, 0X03, 0X00, 0X00, 0X01, 
0X01, 0X02, 0X03, 0X40, 0X00, 0X8E, 0X88, 0X48, 0X8F, 0X03, 
0X02, 0XC3, 0XF2, 0X03, 0X03, 0X80, 0X03, 0X40, 0XCE, 0XE8, 
0X08, 0X0F, 0XE3, 0XC2, 0X83, 0X00, 0X00, 0XC0, 0XF3, 0X03, 
0X00, 0XC8, 0X09, 0X86, 0X42, 0XC2, 0X07, 0X42, 0XC0, 0X00, 
0X0E, 0X08, 0X08, 0X4F, 0X08, 0X9B, 0XF7, 0X02, 0X03, 0XE2, 
0XC3, 0X83, 0X02, 0X83, 0X02, 0X03, 0XC2, 0XF3, 0X03, 0XF3, 
0XF0, 0X20, 0XC8, 0XEB, 0X07, 0X00, 0X83, 0X00, 0XE8, 0X09, 
0XE6, 0X02, 0X82, 0X87, 0XE2, 0XE3, 0XE2, 0X42, 0XC3, 0X03, 
0X03, 0X80, 0XA0, 0X8E, 0X88, 0X08, 0X8F, 0X83, 0X82, 0XF3, 
0XE2, 0X03, 0X03, 0X00, 0X03, 0X00, 0X0E, 0XE8, 0X08, 0X8F, 
0X83, 0X82, 0X03, 0X00, 0X00, 0X80, 0X43, 0XC3, 0XE0, 0XE8, 
0XF9, 0XE6, 0X02, 0XE2, 0XC7, 0XC2, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X07, 0X04, 0X04, 0X07, 0X01, 0X01, 0X01, 0X00, 
0X00, 0X00, 0X00, 0X01, 0X01, 0X01, 0X01, 0X01, 0X81, 0X21, 
0X01, 0XC1, 0XC1, 0XA1, 0XC1, 0XB0, 0XE0, 0XF0, 0X00, 0XF1, 
0X01, 0X81, 0X21, 0X01, 0XC1, 0XC1, 0XA1, 0XC1, 0X01, 0X01, 
0X60, 0X78, 0X87, 0X84, 0XC4, 0XFF, 0XF9, 0XF1, 0X81, 0X81, 
0XC1, 0XF1, 0X05, 0XF1, 0X00, 0X60, 0X78, 0X80, 0X81, 0XC1, 
0X81, 0XC1, 0XC1, 0XC0, 0X81, 0XE1, 0XF1, 0X01, 0XF1, 0X00, 
0X01, 0X80, 0XC1, 0XC1, 0X01, 0XE1, 0XE1, 0XF1, 0X81, 0X80, 
0XC0, 0XC7, 0XC4, 0XA5, 0XE5, 0X01, 0X01, 0XD1, 0XA1, 0XA1, 
0XE1, 0XF0, 0XF0, 0XFF, 0XF5, 0X07, 0X07, 0X61, 0X79, 0X81, 
0X81, 0XC0, 0XC0, 0X40, 0XF7, 0X81, 0X81, 0XE1, 0X81, 0XC9, 
0XA1, 0XE1, 0X80, 0XC5, 0XF5, 0X03, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X03, 0X02, 
0X02, 0X03, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X03, 0X02, 0X02, 0X03, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X01, 0X01, 0X00, 0X00, 0X00, 0X00, 0X02, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X02, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X03, 0X03, 0X01, 0X00, 0X02, 0X02, 0X01, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X02, 0X00, 0X00, 0X00, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00},
{ 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00} 
};	
unsigned char  idle_logo[8][128]={
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, 
{
0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 
0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 
0X80, 0X80, 0X80, 0X80, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00},
{ 
0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X9F, 0XEF, 0XFF, 0XFF, 
0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 
0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00},
{ 
0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X0F, 0X0F, 0X1F, 0X2F, 
0XB7, 0XFB, 0XFD, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 
0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XF0, 0XF0, 0XC0, 0XC0, 
0XC0, 0XC0, 0XC0, 0XC0, 0XF8, 0XF8, 0XF8, 0XF8, 0XD0, 0XF8, 
0XF8, 0XF8, 0XF8, 0XCC, 0XFE, 0XFE, 0XFE, 0XFE, 0XC0, 0XC0, 
0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XFE, 0XFE, 0XFE, 
0XFE, 0XFE, 0XFE, 0X00, 0X60, 0XF0, 0XD0, 0XC0, 0XC0, 0XC0, 
0XC0, 0XC0, 0XF0, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 0XF8, 
0XFE, 0XFE, 0XDE, 0XD8, 0XDE, 0XDE, 0XD8, 0XF8, 0XF8, 0XF0, 
0XF8, 0XD8, 0XF8, 0XF8, 0XC0, 0XF8, 0XFB, 0XFF, 0XFF, 0X00, 
0XE0, 0XF0, 0XD0, 0XC0, 0XC0, 0XC0, 0XF8, 0XFF, 0XFF, 0XFF, 
0XFC, 0XF8, 0XF8, 0XD0, 0XF8, 0XF8, 0XD8, 0XD8, 0XD8, 0XD8, 
0XD8, 0XF8, 0XF8, 0XD0, 0XF8, 0XF8, 0XF8, 0XF8},
{ 
0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFC, 0XF0, 0XC0, 0X9C, 0X3A, 
0X7E, 0XFF, 0XFF, 0XFD, 0XFD, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 
0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0X80, 0XE0, 0XF0, 0XFC, 
0XFF, 0XF3, 0XC3, 0XFC, 0XFC, 0XFC, 0XB0, 0XF0, 0XF0, 0XF0, 
0XF0, 0X00, 0XF0, 0XF0, 0X00, 0X00, 0X00, 0XFC, 0XFC, 0XFC, 
0X8C, 0X9C, 0XFC, 0XF8, 0XFC, 0XFC, 0XB4, 0XF3, 0XF3, 0XF2, 
0XF0, 0XF0, 0XF0, 0XF0, 0XFC, 0XFC, 0XFC, 0X30, 0XF3, 0XF3, 
0XFF, 0XFF, 0X00, 0X00, 0X00, 0XFC, 0XFC, 0XF3, 0XF3, 0XF3, 
0XF0, 0XFC, 0XFC, 0XFC, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 
0XF0, 0XF3, 0XF3, 0XF1, 0X33, 0XF3, 0XFF, 0XFC, 0XFC, 0XF0, 
0XB0, 0X30, 0XB0, 0XF0, 0X00, 0X00, 0XFC, 0XFC, 0XFC, 0XEC, 
0XFF, 0XFB, 0XE1, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0X30, 
0X30, 0XF0, 0XFC, 0XFC, 0XEC, 0XF0, 0XB0, 0X30},
{ 
0X00, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 
0X7E, 0X7E, 0X7D, 0X7D, 0X7B, 0X7B, 0X77, 0X77, 0X7F, 0X7F, 
0X7F, 0X7F, 0X7F, 0X7F, 0X00, 0X00, 0X7B, 0X7B, 0X70, 0X78, 
0X70, 0X79, 0X7B, 0X73, 0X7B, 0X7B, 0X03, 0XC3, 0XF1, 0X71, 
0X73, 0X63, 0X73, 0X73, 0X70, 0X70, 0X78, 0X7B, 0X73, 0X03, 
0X03, 0X01, 0X01, 0X01, 0X03, 0X03, 0X01, 0X03, 0X03, 0X03, 
0X01, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X01, 
0X03, 0X03, 0X00, 0X00, 0X00, 0X03, 0X03, 0X01, 0X03, 0X03, 
0X03, 0X01, 0X03, 0X03, 0X01, 0X03, 0X03, 0X03, 0X03, 0X03, 
0X01, 0X03, 0X03, 0X03, 0X00, 0X03, 0X03, 0X03, 0X03, 0X01, 
0X03, 0X03, 0X03, 0X01, 0X00, 0X00, 0X03, 0X03, 0X03, 0X03, 
0X03, 0X01, 0X01, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X00, 
0X03, 0X03, 0X03, 0X03, 0X00, 0X01, 0X03, 0X03}, 
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, 
{
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}
}; 


WORD disp_logo(void)
{
    // Is this for selecting language ?
    BYTE key;
/*
#ifndef DEVELOP_VERSION
    int i;
    int b_line=0;
    
    ScrCls();	
    window(1,1,21,16);	
    
    gotoxy(1,1);		
    //unsigned char  lang2[6][128]={
    //unsigned char  lang1[7][128]={			
    for (i = 0; i < 7; i++) {
        putpixelcol((const char *)&adib_1Display[i], 128);  
        gotoxy(0,i + 2);
    }
    for (i = 0; i < 7; i++) {
        putpixelcol((const char *)&adib_2Display[i], 128);  
        gotoxy(0,i + 8);
    }
#endif
*/
    while(TRUE)
    {
        key = GetOneKey(3);	
        switch (key)
        {
        case KEY1:
            gLanguage=ENGLISH;
            return(STAT_OK);            
        case KEY2:	
            gLanguage=ARABIC;
            return(STAT_OK);
        case KEYCANCEL:
        case NOKEY:
            return(STAT_NOK);
        }
    }
    return(STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: SearchMVTIndex
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */
// Gary delete this useless function
/*
short SearchMVTIndex(struct sIndex *index_arr, BYTE *RID, int index_len)
{
	int i;

	for(i=0;i<index_len;i++)
		if (!memcmp(index_arr[i].RID, RID, 5)) return i;

	return -1;
}*/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Create_Update_ESTFile_Key
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */
char Create_Update_ESTFile_Key(struct sIndex *index_arr, int index_len)
{
    return 0;
// Gary delete this useless function
    /*
	int rec_len_o;
	int fp,i,j,fpread;
	struct stEst_Rec est_rec;
	char find_ok;
	char tmp1[24],tmp2[24],key_file[24];
	int index,rec;
	long offset;
	KeyInfoStruct caTemp;
	sint siLastSeq;
	
	rec_len_o = sizeof(struct stEst_Rec);

	if ((fpread = open(csFILE_EMV_KEYS, O_RDWR))<0)
        return FALSE;

	siLastSeq=lseek(fpread, 0, SEEK_END);


	if ((fp = open(EmvESTFile, O_RDWR))<0) // Opening EST.DAT
	{
		if ((fp = open(EmvESTFile, O_RDWR|O_CREAT))<0)
        {
            close(fp);
            return FALSE;
        } // Checking/Creating EST.DAT
		write(fp,(char *)&EST_RecHdr,sizeof(EST_RecHdr));		
	}

	lseek(fpread,0,SEEK_SET);
	for (j = 0; j < siLastSeq; j++)
	{
		find_ok  =FALSE;
		if (read(fpread,(char *)&caTemp,sizeof(KeyInfoStruct)) != sizeof(KeyInfoStruct))
		{
            close(fp);
            close(fpread);
            return FALSE;
        }
		rec = 0;
		index=0;
 
		lseek(fp,16L,SEEK_SET);
		memcpy(tmp2,APPL_AID,5);
		HexToAsc(tmp1, tmp2 ,5);
		while(read(fp,(char *)&est_rec,rec_len_o) == rec_len_o)
		{
			if (memcmp(est_rec.szRID,tmp1,10) == 0)
            {
                find_ok=TRUE;
                break;
            }
			rec++;
		}
		if (find_ok != TRUE)
		{
			memset(&est_rec,0,sizeof(est_rec));
			est_rec.lnNbrOfTransactions = 0; 
			est_rec.inEMVTableRecord = SearchMVTIndex(index_arr, tmp2, index_len); 
			if (tmp2[4] == 0x03)
                strcpy(est_rec.szSchemeLabel,"Visa");
			else if (tmp2[4] == 0x04)
                strcpy(est_rec.szSchemeLabel,"MasterCard");
			else
                strcpy(est_rec.szSchemeLabel,"New Scheme");
			HexToAsc(est_rec.szRID,tmp2,5);

			HexToAsc(&est_rec.szCSNListFile[0],tmp2,5);
			memcpy(&est_rec.szCSNListFile[10],".CSN",4);

			HexToAsc(&est_rec.capk[index].szCAPKFile[0],tmp2,5);
			memcpy(&est_rec.capk[index].szCAPKFile[10],".",1);
			HexToAsc(&est_rec.capk[index].szCAPKFile[11],&caTemp.CAPKIndex,1);
			memcpy(&est_rec.capk[index].szCAPKExpDate,"311210",6);
			est_rec.capk[index].inPublicKeyIndex = caTemp.CAPKIndex; 

			write(fp,(char *)&est_rec,sizeof(est_rec));
		}
		else
		{
			memset(key_file,0,sizeof(key_file));
			HexToAsc(&key_file[0],tmp2,5);
			memcpy(&key_file[10],".",1);
			HexToAsc(&key_file[11],&caTemp.CAPKIndex,1);
			for(i=0;i<15;i++)
			{
				if (memcmp(key_file,est_rec.capk[i].szCAPKFile,13) == 0 || est_rec.capk[i].szCAPKFile[0] == 0)
                    break;
				index++;
			}
			if (index <15)
			{
				offset = 16;
				offset += rec * sizeof(est_rec);
				memcpy(&est_rec.capk[index].szCAPKFile[0],key_file,13);
				memcpy(&est_rec.capk[index].szCAPKExpDate,"311210",6);
				est_rec.capk[index].inPublicKeyIndex = caTemp.CAPKIndex; 

				lseek(fp,(long)offset,SEEK_SET);
				write(fp,(char *)&est_rec,sizeof(est_rec));
			}
		}
	}

	close(fp);
	close(fpread);
	return TRUE;
    */
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Create_Update_ESTFile_Config
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */
char Create_Update_ESTFile_Config(struct sIndex *index_arr, int index_len)
{
    return 0;
    // Gary delete this useless function
    /*
	int rec_len_o;
	int fp,i,j;
	struct stEst_Rec est_rec;
	char find_ok, tmpl[40],tmp2[20],tmp[20],tmp2_long[20];
	int rec,index;
	long offset;

	rec_len_o = sizeof(struct stEst_Rec);

	if ((fp = open(EmvESTFile, O_RDWR))<0)
	{
		if ((fp = open(EmvESTFile, O_RDWR|O_CREAT))<0)
        {
            close(fp);
            return FALSE;
        }
		write(fp,(char *)&EST_RecHdr,sizeof(EST_RecHdr));
	}

	find_ok  =FALSE;
	rec = 0;
	index=0;
	
	lseek(fp,16L,SEEK_SET);
	memcpy(tmp2,APPL_AID,5);
	memcpy(tmp2_long,APPL_AID_LONG,7);
	
	HexToAsc(tmpl, tmp2,5);
	while(read(fp,(char *)&est_rec,rec_len_o) == rec_len_o)
	{
		if (memcmp(est_rec.szRID,tmpl,10) == 0)
		{
			find_ok=TRUE;
			break;
		}
		rec++;
	}
	if (find_ok != TRUE)
	{
		memset(&est_rec,0,sizeof(est_rec));
		est_rec.lnNbrOfTransactions = 0;
		est_rec.inEMVTableRecord = SearchMVTIndex(index_arr, tmp2, index_len); 
		if (tmp2[4] == 0x03) strcpy(est_rec.szSchemeLabel,"Visa");
		else if (tmp2[4] == 0x04) strcpy(est_rec.szSchemeLabel,"MasterCard");
		else strcpy(est_rec.szSchemeLabel,"New Scheme");
		HexToAsc(est_rec.szRID,tmp2,5);

		HexToAsc(&est_rec.szCSNListFile[0],tmp2,5);
		memcpy(&est_rec.szCSNListFile[10],".CSN",4);

		HexToAsc(&est_rec.sup_app[0].szSupportedAID,tmp2_long,7);
		est_rec.sup_app[0].inPartialNameAllowedFlag = 2; 
		memcpy(tmp,"\x00\x02",2);
		HexToAsc(&est_rec.sup_app[0].szTermAVN,tmp,2);
		HexToAsc(&est_rec.sup_app[0].szSecondTermAVN,tmp,2);

		if (tmp2[4] == 0x03) strcpy((char *)&est_rec.sup_app[0].szRecommenedAppNameAID,"VISA APP");
		else if (tmp2[4] == 0x04) strcpy((char *)&est_rec.sup_app[0].szRecommenedAppNameAID,"MC APP");
		else strcpy((char *)&est_rec.sup_app[0].szRecommenedAppNameAID,"NON APP");

		write(fp,(char *)&est_rec,sizeof(est_rec));
	}
	else
	{
		for(i=0;i<10;i++)
		{
			HexToAsc(tmpl, tmp2_long,7);
			if (memcmp(tmpl,est_rec.sup_app[i].szSupportedAID,14) == 0
				 || est_rec.sup_app[i].szSupportedAID[0] == 0) break;
			index++;
		}
		if (index <10)
		{
			offset = 16;
			offset += rec * sizeof(est_rec);
			HexToAsc(&est_rec.sup_app[index].szSupportedAID,tmp2_long,7);
			est_rec.sup_app[index].inPartialNameAllowedFlag = 2; 
			memcpy(tmp,"\x00\x02",2);
			HexToAsc(&est_rec.sup_app[index].szTermAVN,tmp,2);		
			memcpy(tmp,"\x00\x03",2);		
			HexToAsc(&est_rec.sup_app[index].szSecondTermAVN,tmp,2);				
			
			if (tmp2[4] == 0x03) strcpy((char *)&est_rec.sup_app[index].szRecommenedAppNameAID,"VISA APP");
			else if (tmp2[4] == 0x04) strcpy((char *)&est_rec.sup_app[index].szRecommenedAppNameAID,"MC APP");
			else strcpy((char *)&est_rec.sup_app[index].szRecommenedAppNameAID,"NON APP");

			lseek(fp,(long)offset,SEEK_SET);
			write(fp,(char *)&est_rec,sizeof(est_rec));
		}
	}
	close(fp);

    return TRUE;
    */
}



/* --------------------------------------------------------------------------
 * FUNCTION NAME: Create_Update_MVTFile
 *
 * DESCRIPTION  : 
 *
 * NOTES        : 
 *
 * RETURN       : TRUE/FALSE
 * ------------------------------------------------------------------------ */
int Create_Update_MVTFile(struct sIndex *index_arr, int *index_len)
{
	char buf[32],tmp2[20],tmpBuf[50];
    int iRet;
    EMV_PARAM   stEmvPara;
    EMV_APPLIST stEMVApp;
	HDGStruct lsHDGBuf;
    
    //Gary note: EmvMVTFile is written but never read out.
    /*
	int fp, i, usLen;
	char tmpl[64];
	struct TagMVT_REC mvt_rec;

    remove(EmvMVTFile);
    
	memcpy(tmp2,APPL_AID,5);
	HexToAsc(tmpl, tmp2,5);

	*index_len = 0;
	if ((fp = open(EmvMVTFile, O_RDWR))<0)
	{
		if ((fp = open(EmvMVTFile, O_RDWR|O_CREATE))<0)
        {
            close(fp);
            return FALSE;
        }
		write(fp,(char *)&EST_RecHdr,sizeof(EST_RecHdr));
	}
	lseek(fp,16L,SEEK_SET);
    */

	if (HDGRead(&lsHDGBuf)!=STAT_OK)
		return FALSE;
	
    //===================================================================================

    // Gary note: TODO : still other params will be set in other places, in below comment out codes
    memset(&stEmvPara, 0, sizeof(stEmvPara));
    EMVGetParameter(&stEmvPara);

    stEmvPara.TransCurrExp = 2;
    stEmvPara.ReferCurrExp = 2;
#ifdef DEVELOP_VERSION
    memcpy(stEmvPara.CountryCode,   "\x04\x14", 2);
    memcpy(stEmvPara.TransCurrCode, "\x04\x14", 2);
#else
    memcpy(stEmvPara.CountryCode,   lsHDGBuf.terminalCountryCode, 2);
    memcpy(stEmvPara.TransCurrCode, lsHDGBuf.trCurrencyCode,      2);
#endif
    memcpy(stEmvPara.CountryCode,   lsHDGBuf.terminalCountryCode, 2);
    memcpy(stEmvPara.TransCurrCode, lsHDGBuf.trCurrencyCode,      2);
    stEmvPara.ForceOnline = 1;
    memcpy(stEmvPara.Capability,    "\xE0\xF0\xC8",         3); // "\x20\xB0\xC8"
    memcpy(stEmvPara.ExCapability,  "\xE0\x00\xB0\x00\x00", 5);     // ???? code table = 9 when byte4==A1 ????
    memcpy(stEmvPara.MerchCateCode, "\x00\x52", 2);
    stEmvPara.TerminalType = 0x22;
    
    // Below are for reference
// 	memcpy(mvt_rec.szDefaultDDOL,"9F3704",6);
// 	memcpy(mvt_rec.szDefaultTDOL,"9F02065F2A029A039C0195059F3704",30);

    // 2011-3-4 Gary
    stEmvPara.TransType = EMV_GOODS;

    EMVSetParameter(&stEmvPara);

    //===================================================================================

    memset(&stEMVApp, 0, sizeof(stEMVApp));
#ifdef APP_COMPILE_CPA
    stEMVApp = gPROCCO_CPA_APP;
#else
    stEMVApp = gVISA_VSDC_APP;
#endif

    /*
	memset(&mvt_rec,0,sizeof(mvt_rec));	
	mvt_rec.inSchemeReference	= -1; 
	mvt_rec.inIssuerReference	= -1; 
	mvt_rec.inTRMDataPresent	=  1; 
	mvt_rec.inNextRecord		= -1;	
	if (SearchMVTIndex(index_arr, tmp2, *index_len) < 0)
	{
		memcpy(index_arr[*index_len].RID, tmp2, 5);
		(*index_len)++;
	}
	mvt_rec.ulEMVCounter = 0; 
    */

	HexToAsc(lsHDGBuf.thresholdValue,tmpBuf,6);
	tmpBuf[12]=0;
    stEMVApp.Threshold = atol((PCHAR)tmpBuf);
	//mvt_rec.lnEMVRSThreshold = atol((PCHAR)tmpBuf);
	
	HexToAsc(lsHDGBuf.floorLimitDomestic,tmpBuf,6);
	tmpBuf[12]=0;
    stEMVApp.FloorLimit = atol((PCHAR)tmpBuf);
    stEMVApp.FloorLimitCheck = 1;
	//mvt_rec.lnEMVFloorLimit = atol((PCHAR)tmpBuf);
	
    stEMVApp.TargetPer = lsHDGBuf.targetPercentage;
    stEMVApp.MaxTargetPer = lsHDGBuf.maxTargetPercentage;
	//mvt_rec.inEMVTargetRSPercent = lsHDGBuf.targetPercentage;
	//mvt_rec.inEMVMaxTargetRSPercent = lsHDGBuf.maxTargetPercentage;

	//mvt_rec.inMerchantForcedOnlineFlag = 1;
	//mvt_rec.inBlackListedCardSupportFlag = 0;
    //mvt_rec.inEMVFallbackAllowed = 1;
	//mvt_rec.inEMVTermCurExp = 2; 
	//mvt_rec.inModifyCandListFlag = 0;
	
	lsHDGBuf.denialTAC[2] = lsHDGBuf.denialTAC[2] | 0x20; 
    memcpy(stEMVApp.TACDenial,  lsHDGBuf.denialTAC,  5);
    memcpy(stEMVApp.TACOnline,  lsHDGBuf.onlineTAC,  5);
    memcpy(stEMVApp.TACDefault, lsHDGBuf.defaultTAC, 5);
	//HexToAsc(lsHDGBuf.defaultTAC,mvt_rec.szEMVTACDefault,5);
	//HexToAsc(lsHDGBuf.denialTAC,mvt_rec.szEMVTACDenial,5);
	//HexToAsc(lsHDGBuf.onlineTAC,mvt_rec.szEMVTACOnline,5);

    // see 
    //memcpy(mvt_rec.szDefaultDDOL,"9F3704",6);
    //memcpy(mvt_rec.szDefaultTDOL,"9F02065F2A029A039C0195059F3704",30);

    // Gary : No sense to PAX
 	//strcpy(mvt_rec.szRFU1, "\x54\x6F\x79\x67\x61\x72\x20\x2D\x2D\x31\x36\x2F\x30\x31\x2F\x30\x38");
 	//memcpy(mvt_rec.szRFU2, tmp2, 5);
	//mvt_rec.inEMVAutoSelectAppln = 0; 
	
    // Gary : Already on above EMV parameter
	//HexToAsc(lsHDGBuf.terminalCountryCode,mvt_rec.szEMVTermCountryCode,2);
	//HexToAsc(lsHDGBuf.trCurrencyCode,mvt_rec.szEMVTermCurrencyCode,2);
	//memcpy(mvt_rec.szEMVMerchantCategoryCode,"52",2);
	//tmp2[0]=0x52;
	//memcpy(mvt_rec.szEMVTerminalCategoryCode,tmp2,1);
	//memcpy(mvt_rec.szEMVTermCapabilities,"60F0C0",6);
	//memcpy(mvt_rec.szEMVTermAddCapabilities,"E000B0A100",10);
	//memcpy(mvt_rec.szEMVTermType,"22",2);
	//write(fp,(char *)&mvt_rec,sizeof(mvt_rec));
	//close(fp);

    // Save into EMV lib
    iRet = EMVAddApp(&stEMVApp);
    if (iRet)
    {
        DelayMs(1);
    }
	return iRet;
}


void vSetDefaultFunctionPointers(void)
{
    // belows are for non-PAX emv lib, disabled.
    /*
	inVxEMVAPSetFunctionality(PERFORM_ONLINE_PIN, (void *)&usEMVPerformOnlinePIN);
	inVxEMVAPSetFunctionality(GET_LAST_TXN_AMT, (void *)&getLastTxnAmt);
	inVxEMVAPSetFunctionality(DISPLAY_ERROR_PROMPT, (void *)&usEMVDisplayErrorPrompt);
	inVxEMVAPSetFunctionality(GET_USER_PIN, (void *)&getUsrPin);
	inVxEMVAPSetFunctionality(PERFORM_SIGNATURE, (void *)&usEMVPerformSignature);
    */
}

// Gary : This may be Vx670 EMV core callback function for input PIN
/*
short getTermUsrPin(unsigned char *pin)
{
    char str[15+1];
    int size=0;	
    char msg_line3[16+1];
    char msg_line1[22];
    uchar ucBeep = 0;
    BYTE byRet;
    //return (0);
    read(ghConsole, msg_line1, 21);
    
    memset(msg_line3,0,sizeof(msg_line3));
    memset(msg_line1,0,sizeof(msg_line1));
    
    if (emv_off_pin_try)
        strcpy(msg_line1,"REENTER");
    
    emv_off_pin_entry=1;		
    OS_ClearScreen();
    memset(str,0,sizeof(str));
    
    enable_key_beeps();
    
    size=dsp_func((char *)str,12,4,3,0,0 );  
    
    key_beeps(1);
    
    emv_off_pin_try++;	
    if (size == -1)
    {
        emv_off_pin_entry=2;		
        return E_USR_PIN_CANCELLED;
    }
    else if (size == -2)
    {
        terminate_Transaction = TRUE;		
        return E_PIN_REQD;
    }
    size=strlen(str);
    
    memcpy(pin, str, size);
    pin[size] = 0;
    
    return (size);
}
*/

// Gary : This may be Vx670 EMV core callback function for velocity check
/*
unsigned long ulCheckSplitSalesAmt(void)
{
    char panStr[21] = {0};
	char panSeqStr[3] = {0};
	BYTE pan[10] = {0};
	BYTE panSeq[4] = {0};
	unsigned short len;
	int fHandle = 0;
	char buf[150] = {0};
	char * temp;
	unsigned short retVal;
	unsigned long mainRet = 0;
	char * pSeploc; 

	usEMVGetTLVFromColxn(0x5a00, pan, &len);
    HexToAsc((BYTE *) panStr, pan, len);
    
    while( (temp =  (char*)strrchr((char*)panStr,'F')) != NULL) 
	panStr[temp - panStr] = 0;

	usEMVGetTLVFromColxn(0x5f34, panSeq, &len);
   	HexToAsc((BYTE *) panSeqStr, panSeq, len);

	memset(&stSpsales,0x00, sizeof(stSpsales));
	fHandle = open ("splitsales.log", O_RDWR|O_CREATE);
 	if (fHandle < 0) return(SUCCESS);
	memset(buf, 0x00, sizeof(buf));
	retVal = readRecord(fHandle, buf);
	
	//check for read() returning 0 for EOF. 
	while(retVal != 0)
	{
		if ( (pSeploc = strchr( buf, ',' )) != NULL)
		{
			memcpy(stSpsales.panStr,buf, (pSeploc-buf));
			memcpy(stSpsales.panSeqStr, &buf[pSeploc-buf+1],2);
			memcpy(stSpsales.trnAmt, &buf[pSeploc-buf+4],12);

			if ( !(memcmp (panStr,stSpsales.panStr,strlen(panStr))))
				if ( !(memcmp (panSeqStr,stSpsales.panSeqStr,strlen(panSeqStr))))
				{
					close (fHandle);
					mainRet = SUCCESS;
					mainRet = atol (stSpsales.trnAmt);
					return (mainRet);
				}
		}
		retVal = readRecord(fHandle, buf);
		if (retVal == EMV_FALSE)
		{	
			close (fHandle);
			return(SUCCESS); 
		}
	}
	close (fHandle);
	return(SUCCESS);
}
*/

// Gary : This may be Vx670 EMV core function
/*
short readRecord(int fHandle , char *buf)
{
	char readBytes = 0;
    short retVal;
    short i = 0;

    while(readBytes != '\r')
    {
		retVal = read(fHandle, &readBytes, 1);
        if (retVal == 0) return 0 ;
		if (readBytes != '\n') buf[i++] = readBytes;
		if (retVal < 0) { close(fHandle); return(EMV_FALSE); }
	}
	buf[i] = '\0';
    return (i);
}
*/

// int inGetPTermID(char *ptid)
// {
// 	char    tempPTID [EMV_TERM_ID_SIZE + 2];
// 
// 	SVC_INFO_PTID (tempPTID);
// 	strncpy (ptid, tempPTID+1, EMV_TERM_ID_SIZE);
// 	ptid[EMV_TERM_ID_SIZE+1] = 0;
// 
// 	return (EMV_SUCCESS);
// }

BYTE Emv_ApplSelect(BYTE *pbErr, WORD wTransNo)
{
//	short autoFlag;
	int result;
//	srAIDListStatus paidStatus;
//	unsigned short aidcount;
//	unsigned short blockAidCount;

    // Gary add note :
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //!!!!!!!! setup EMV app data first !!!!!!!!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Gary : use common numeric is enough
    result = EMVAppSelect(CUSTOMER_CARD, (ULONG)wTransNo);
    if (result!=EMV_OK)
    {
        if (result==EMV_APP_BLOCK)
        {
            *pbErr = eAPP_BLOCKED_INVALIDATED;
        }
        if (result==EMV_NO_APP)
        {
			*pbErr = eNO_MUTUAL_APP;
        }
        
		if ((result != ICC_BLOCK) &&
            (result != ICC_RESET_ERR) &&
            (result != ICC_CMD_ERR) &&
            (result != EMV_USER_CANCEL) &&
            (result != EMV_TIME_OUT))
		{
			return -1;  // EMV_FALLBACK;
		}
		else if (result == ICC_BLOCK)
        {
            *pbErr = eCARD_BLOCKED_INVALIDATED;
        }

		return -2;  // EMV_FAIL;
    }
    return 0;


    // Gary: use PAX's method

	/*
	// Get the Auto APP selection flag from the default table.
	autoFlag = inGetAutoSelectApplnFlag();
	if ((result = inVXEMVAPSelectApplication(autoFlag, inMenuFunc, vdPromptManager, usAmtEntryFunc, NULL)) != SUCCESS)
	{
			if (result == APPL_BLOCKED) *pbErr = eAPP_BLOCKED_INVALIDATED;

		// Fix for VISA ADVT card 47. If the AID is blocked and not added to the collection, check the staus by
		// calling usEMVGetAllAIDStatus().
		if (result == CANDIDATELIST_EMPTY)
		{
			*pbErr = eNO_MUTUAL_APP;
			usEMVGetAllAIDStatus(&paidStatus, &aidcount, &blockAidCount);

			if (blockAidCount > 0) { *pbErr = eAPP_BLOCKED_INVALIDATED; return EMV_FAIL; }
			//else
			//	if (blockAidCount == 0) result =CARD_BLOCKED;
		}
		if ((result != CARD_BLOCKED) && (result != CARD_REMOVED)&&(result != TRANS_CANCELLED) &&(result != INVALID_PARAMETER))
		{	
			return(EMV_FALLBACK);
		}
		else if (result == CARD_BLOCKED) *pbErr = eCARD_BLOCKED_INVALIDATED;
		return (EMV_FAIL);
	}
	return EMV_OK;
    */
}

// Gary: EMV related. use PAX's method
/*
void vdPromptManager(unsigned short inCondition)
{
	char msg[22];
	BYTE tvr[5];
	BYTE tsi[2];
	char outp[11];
	unsigned short len;

	memset(msg, 0, sizeof(msg));
	switch (inCondition)
	{
	case APPROVED:
		strcpy(msg, "APPROVED");
		break;
	case DECLINED:
		strcpy(msg, "DECLINED");
		break;
	case OFFLINE_APPROVED:
		strcpy(msg, "OFFLINE APPROVED");
		break;
	case OFFLINE_DECLINED:
		strcpy(msg, "OFFLINE DECLINED");
		break;
	case FAILURE:
		strcpy(msg, "FAILURE");
		break;
	case EMV_CHIP_ERROR:
		strcpy(msg, "EMV CHIP ERROR");
		break;
	case CARD_REMOVED:
		strcpy(msg, "CARD REMOVED");
		break;
	case CARD_BLOCKED:
		strcpy(msg, "CARD BLOCKED");
		break;
	case APPL_BLOCKED:
		strcpy(msg, "APPL BLOCKED");
		break;
		//case NO_AID_MATCH:
		//    strcpy(msg, "DESTEKLENMEYEN UYGUL");
		//    break;
	case CHIP_ERROR:
		strcpy(msg, "CHIP ERROR");
		break;
	case BAD_DATA_FORMAT:
		strcpy(msg, "BAD DATA FORMAT");
		break;
	case APPL_NOT_AVAILABLE:
		strcpy(msg, "APPL NOT AVAILABLE");
		break;
	case TRANS_CANCELLED:
		strcpy(msg, "TRANS CANCELLED");
		return;
		break;
	case EASY_ENTRY_APPL:
		strcpy(msg, "EASY ENTRY APPL");
		break;
	case ICC_DATA_MISSING:
		strcpy(msg, "ICC DATA MISSING");
		break;
	case CONFIG_FILE_NOT_FOUND:
		strcpy(msg, "CONFIG FILE NOT FOUND");
		break;
	case FAILED_TO_CONNECT:
		strcpy(msg, "FAILED TO CONNECT");
		break;
	case SELECT_FAILED:
		strcpy(msg, "SELECT FAILED");
		break;
	case USE_CHIP:
		strcpy(msg, "USE CHIP");
		break;
	case REMOVE_CARD:
		strcpy(msg, "REMOVE CARD");
		break;

	default:
		strcpy(msg, "DECLINED");
		break;
	}

	write_at(msg, strlen(msg), 1, 4);
	error_tone();

	//	if (debug_mode)
	{
		OS_ClearScreen();
		usEMVGetTLVFromColxn(0x9500, tvr, &len);
		hex2str((BYTE *) outp, tvr, len);
		outp[10] = '\0';
		write_at("TVR ", 4, 1, 6);
		write_at(outp, 10, 5, 6);

		usEMVGetTLVFromColxn(0x9b00, tsi, &len);
		hex2str((BYTE *) outp, tsi, len);
		outp[4] = '\0';
		write_at("TSI ", 4, 1, 7);
		write_at(outp, 4, 5, 7);
	}

	DelayMs(2000);
}
*/

// Gary: EMV app selection callback. use PAX's method
/*
int inMenuFunc(char **labels, int numLabels)
{
#ifdef SELMA	
	int i, rv;

	menu  _menu;
	entry _entry[10];
	char  _text[10][64];

	ZERO(_menu);
	ZERO(_entry);
	ZERO(_text);

	if (numLabels > cntof(_entry)){return 0;}

	while(1)
	{
		rv = mx_async_proc(ASYNC_QUERY, ASYNC_TASK_AMOUNT, 0, 0);
		// ...
		if (rv <=    ASYNC_STAT_EMPTY){break;}
		if (rv >= ASYNC_STAT_COMPLETE){break;}

		struct timespec tim = {0,0};
		tim.tv_nsec = 100 * 1000000;
		nanosleep(&tim, 0);
	}

	global_lock(0);

	for(i = 0; i < numLabels; i++)
	{
		_entry[i].type = 0;
		_entry[i].text = _text[i];
		snprintf(_text[i], lenof(_text[0]), "%s", labels[i]);
	}

	_menu.menu_entries = _entry;
	_menu.header = "SELECT ?";
	_menu.no_of_entries = numLabels;
	_menu.mode_flags = DEFAULT_MODE | MENU_TIMEOUT_30_SEC;
	
	rv = ShowMenu(&_menu, 30, Opt_INVERSE |   Opt_LONGMSG);
	printf("inMenuFunc = %d\n", rv);
	 return rv;
#else	 
	 return 1;
#endif	 
}
*/

// Gary: EMV related. use PAX's method
/*
unsigned short usAmtEntryFunc(unsigned long *lnAmountValue)
{
    //PDOL 9F1A02
    long loAmtBin=0;
    unsigned short usTagL;
    *lnAmountValue=loAmtBin;     
    return EMV_SUCCESS;

  // this part is originally deleted. Gary.
/*	if (Emv_Amount((EMVLanguages *)&emvLanguages)==TRUE){
		if (usEMVGetTLVFromColxn(tagAUTH_AMOUNT_BIN, (byte *)&loAmtBin, &usTagL)== EMV_SUCCESS) {    
			*lnAmountValue=loAmtBin;     
			return EMV_SUCCESS;
		}
	} 
	return EMV_FAIL;*
}
*/

void hex2str(BYTE *outp, BYTE *inp, int length)
{
	int i;

	for(i=0;i<length;i++)
	{
		if ((inp[i] >> 4) > 9)
			outp[i*2] = (BYTE) ((inp[i]>>4) + 'A' - 10);
		else
			outp[i*2] = (BYTE) ((inp[i]>>4) + '0');

		if ((inp[i] & 0x0f) > 9)
			outp[i*2+1] = (BYTE) ((inp[i] & 0x0f) + 'A' - 10);
		else
			outp[i*2+1] = (BYTE) ((inp[i] & 0x0f) + '0');

	}
	outp[i*2] = 0;
}

// Gary: EMV related. use PAX's method
/*
BYTE Emv_InitiateAppProcessing(void)
{
	int result;

	inVXEMVAPGetCardConfig(-1,-1 );
	if ((result = inVXEMVAPProcessAFL()) != SUCCESS &&(result != TAG_ALREADY_PRESENT)) 
	{
		return (EMV_FAIL);
	}

	return EMV_OK;
}
*/

BOOLEAN EmvDb_Find(unsigned char  *value, WORD *length, usint usTag)
{
	int result = 0;

	if ((result = usEMVGetTLVFromColxn(usTag, value, length)) != EMV_OK)
	{
		return FALSE;
	}
	return TRUE;
}

BYTE EmvDb_StoreData(usint usTag, unsigned char  *value, unsigned short  length)
{
    if ((usTag % 256)==0)
    {
        usTag /= 256;   // 0x9C00 -->> 0x9C
    }
    return EMVSetTLVData(usTag, value, length);

// 	unsigned short result;
// 
// 	result = usEMVAddTLVToCollxn(usTag, value, length);
// 	if (result == E_TAG_ALREADY_PRESENT) result = usEMVUpdateTLVInCollxn(usTag, value, length);
// 	if (result != EMV_SUCCESS) return EMV_FAIL;
// 	return EMV_OK;
}

BYTE Emv_ReadApplicationData(void)
{
// 	BYTE pan[32], track2[64];
// 	usint panlen, track2len, i;

    if (EMVReadAppData()!=EMV_OK)
    {
        return 1;
    }

    // Gary : this is done within PAX emv lib
// 	if (usEMVGetTLVFromColxn(tagTRACK2_EQUIVALENT_DATA, track2, &track2len) == EMV_OK )
// 	{
// 		if (usEMVGetTLVFromColxn(tagPAN, pan, &panlen) == EMV_OK )
// 		{	
// 			for(i=0;i<panlen;i++)
//             {
// 				if (pan[i] == 0xFF) 
// 				{
// 					break;
// 				}
// 				else if (i == panlen-1)
// 				{
// 					if ((pan[i] & 0x0F) == 0x0F)
// 					{
// 						if ((pan[i] & 0xF0) != (track2[i] & 0xF0)) {return EMV_TERMINATE_TRN; }
// 						else continue;
// 					}
// 				}
// 				else if (pan[i] != track2[i]) 
// 				{
// 					return EMV_TERMINATE_TRN;
// 				}
//             }
// 		}
// 	}
 
	return 0;
}

#define L_TRACKII 41


BYTE EMV_GetCardInfo(void)
{
    WORD len;
    BYTE tagBuffer[50];
    BYTE PAN[21];
    BYTE checkTrackII=OS_FALSE;
    BYTE tempBuffer[L_TRACKII];
    BYTE *location;
    BYTE Buffer[15];
    BYTE Panlen;


    ////////////////////////////////////////////////////////
    //TRACK I Card Holder Name//////////////////////////////
    ////////////////////////////////////////////////////////
    len = 0;
	memset(tagBuffer, 0, sizeof(tagBuffer));	
	if (EmvDb_Find(tagBuffer, &len, tagCARDHOLDER_NAME))
    {
        if (len>sizeof(gPTStruct.name_surname)-1)
        {
            len = sizeof(gPTStruct.name_surname)-1;
        }
    	memcpy(gPTStruct.name_surname, tagBuffer, len);
    }
	gPTStruct.name_surname[len]=0;

	if ((tagBuffer[0]!=0x00) || (tagBuffer[1]!=0x00))
	{
		if (len > 45) len = 45;
		tagBuffer[0]=0x5E;
		tagBuffer[len+1]=0x5E;
		tagBuffer[len+2]=0;
		Card_ReadTrackI(tagBuffer);
	}

    ////////////////////////////////////////////////////////
    //PAN SeqNum////////////////////////////////////////////
    ////////////////////////////////////////////////////////
	if (EmvDb_Find(tagBuffer, &len, tagPAN_SEQUENCE_NUMBER))
    {
		gPTStruct.PSN = tagBuffer[0];
	}	
	else
    {
		gPTStruct.PSN = 0xFF;
    }

    ////////////////////////////////////////////////////////
    //TRACK2////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
	memset(tagBuffer, 0, sizeof(tagBuffer));
	if (EmvDb_Find(tagBuffer, &len, tagTRACK2_EQUIVALENT_DATA) )
    {
        if (tagBuffer[0]!=0x00)
        {
            gPTStruct.Tr2Len=len*2;
            memcpy(gPTStruct.TrackISO2,tagBuffer,len);
            if ((tagBuffer[len  - 1] & 0x0F) == 0x0F)
                gPTStruct.Tr2Len--;
            
            if ((2*len) > L_TRACKII)
                len = L_TRACKII/2;
            
            Utils_ConvertBcdToAsc(tagBuffer,tempBuffer,len);
            tempBuffer[2*len]=0;
            
            location = (PBYTE)strchr((const char *)tempBuffer, 'D');
            if ((location == NULL) || ((location - tempBuffer) > 19 ))
                return 1;
            //memcpy(gPTStruct.TrackISO2,tempBuffer,2*len);
            
            memcpy(PAN, tempBuffer, (OS_WORD)(location - tempBuffer));
            PAN[(OS_WORD)(location - tempBuffer)]=0;
            checkTrackII = OS_TRUE;
        }
    }

    ////////////////////////////////////////////////////////
    //PAN///////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
	memset(tagBuffer, 0, sizeof(tagBuffer));
	EmvDb_Find(tagBuffer, &len, tagPAN);

	if ((tagBuffer[0]!=0x00))
	{
		BYTE i;
		if ((2*len) > LG_PAN)
            len = LG_PAN/2;

		Panlen=(2*len);
		
		Bin2Bcd_Byte(PAN, Panlen);
		gPTStruct.BcdPan[0]=PAN[1];
		memcpy(gPTStruct.BcdPan+1,tagBuffer,len);
		Utils_ConvertBcdToAsc(tagBuffer, gPTStruct.Pan, len);
		gPTStruct.Pan[2*len]=0;
		for (i = 0; i < 2*len; i++) if ((gPTStruct.Pan[i]>0x39) || (gPTStruct.Pan[i]<0x30))
		{
			gPTStruct.Pan[i]=0;
			break;
		}
		
		if (BinCheck()!=STAT_OK)
		{
			DisplayString(DSP_WITH_PROCCO_CARD, 3000);
			return 2;
		}
		else
		{
			if ( AlgLuhnKey((2*len), tempBuffer) != tempBuffer[(2*len)-1] )
			{
				DisplayString(DSP_LUHN_ERROR, 3000);
				return 2;
			}
		}
	}

    ////////////////////////////////////////////////////////
    //EXP DATE
    ////////////////////////////////////////////////////////
	EmvDb_Find(tagBuffer,&len, tagAPPL_EXPIRATION_DATE);
	if (tagBuffer[0] != 0x00)
	{	
		tagBuffer[2]=0;
		memcpy(gPTStruct.BcdExpDate,tagBuffer,2);  // Exp Date
		StrBcdToAsc(4,tagBuffer,tempBuffer);
		memcpy(gPTStruct.ExpDate,tempBuffer,4);
	}

    ////////////////////////////////////////////////////////
    ///MERCHANT ID CONTROL//////////////////////////////////
    ////////////////////////////////////////////////////////
	if (ReadMerchantId()!=STAT_OK)
	{
		DisplayString(DSP_W_UNAVAILABLE_MERCHANT_ID, 3000);
		return 1;
	}

	return 0;
}

static BYTE Card_ReadTrackI(BYTE *trackData)
{
    BYTE trackI[100];
    BYTE *start;
    BYTE *end;
    BYTE len;

	gPTStruct.TrackI[0]=0;
	StrCpy(trackI, trackData);

	if (StrLen(trackI) > 100)
        return(0);

//                    	if (gDebug) Utils_DebugPrint(DEB_TRACK_I, trackI, StrLen(trackI));

	start = (PBYTE)strchr((const char *)trackI, 0x5E);
	end   = (PBYTE)strchr((const char *)start+1 , 0x5E);

	if ((start == NULL) || (end == NULL))
        return(0);

	if ((end - start - 1)> 24 )
        len = 24;
	else
        len = (OS_WORD)(end-start-1);

	memcpy(trackI, start+1, len);
	trackI[len]=0;

	start = (PBYTE)strchr((const char *)trackI, 0x2F);
	end   = (PBYTE)strchr((const char *)start+1,0x20);

	if (end != NULL)
        end[0]=0;

	if (start != NULL )
	{
		StrCpy(gPTStruct.TrackI, start+1);
		strcat((char *)gPTStruct.TrackI, " ");
		start[0]=0;
		strcat((char *)gPTStruct.TrackI, (const char *)trackI);
	}
	else
	{
		StrCpy(gPTStruct.TrackI, trackI);
	}
	return (1);
}


BOOLEAN bIsCardBlackListed(BYTE * pan, unsigned short panLen, BYTE * panSeqNo, unsigned short panSeqLen)
{
    // Gary delete
    // PROCCO do not need any kind of black-list
    return FALSE;
}

/*
// Gary : done by PAX EMV lib
BYTE Emv_AuthOfflineData(void)
{
	int result;

	unsigned char pdata[256], buf[512];
	unsigned short pRetLen;
	unsigned char pstCapk[256]={0};
	unsigned char pstCapkExp[128]={0};
	unsigned char aid_card[32]={0};
	unsigned char pki[32]={0};
	short pCapkLen, pCapkExplen;
	unsigned char chTmpBuff2[512]={0};
	
	memset(aid_card, 0, 32);
	memset(pki, 0, 32);

	usEMVGetTLVFromColxn(tagAID_CARD, aid_card,&pRetLen);
	usEMVGetTLVFromColxn(tagCERT_AUTH_PKI, pki,&pRetLen);

	usEMVGetTLVFromColxn(tagAID_TERMINAL, chTmpBuff2,&pRetLen);

	usEMVUpdateTLVInCollxn(tagTERML_CAPABILITIES, "\xE0\xF0\xC8", 3);



	if ((result = getTerminalParam(TAC_DEFAULT, pdata, &pRetLen)) != EMV_SUCCESS) return (EMV_TERMINATE_TRN);

	if ((result = getTerminalParam(TAC_ONLINE, pdata, &pRetLen)) != EMV_SUCCESS) return (EMV_TERMINATE_TRN);

	if ((result = getTerminalParam(TAC_DENIAL, pdata, &pRetLen)) != EMV_SUCCESS) return (EMV_TERMINATE_TRN);

	if ((result = getTerminalParam(TAG_97_TDOL, pdata, &pRetLen)) != EMV_SUCCESS) return (EMV_TERMINATE_TRN);

	if ((result = getTerminalParam(TAG_9F49_DDOL, pdata, &pRetLen)) != EMV_SUCCESS) return (EMV_TERMINATE_TRN);
	
		// Data authentication
	result = inVXEMVAPDataAuthentication(NULL);

	if (result != SUCCESS)
	{
		return (EMV_TERMINATE_TRN);
	}
	return EMV_OK;
}
*/

/*
BYTE Emv_VerifyCardHolder(void)
{
#ifndef DEVELOP_VERSION
	unsigned short length, i;
	BYTE value[32], buf[64];
 	unsigned short result;
	
	if (usEMVGetTLVFromColxn(tagCVM_LIST, value, &length) != EMV_SUCCESS) return EMV_TERMINATE_TRN;


	for (i=8; i<length-2; i+=2) if ((value[i] & 0x1F) == 0x1F) value[i] &= 0x00;
#ifdef APPL_TEST
memcpy(value,"\x00\x00\x00\x00\x00\x00\x00\x00\x1E\x03\x02\x03\x1F\x00",14);
length=14;
OS_LogToHex("tagCVM_LIST ", value, length,DEBUG_PORT); 
#endif

	if (usEMVUpdateTLVInCollxn(tagCVM_LIST, value, length) != EMV_SUCCESS) return EMV_TERMINATE_TRN;
	// Cardholder verification
	terminate_Transaction = FALSE;

	
	result=inVXEMVAPVerifyCardholder(NULL);
	if (!result && !terminate_Transaction)
	{
		return EMV_OK;
	}
	else 
	{
		return EMV_TERMINATE_TRN;
	}
#endif
}
*/
 
int dsp_func(char *value,int mx,int mn,int typ,int punc,int frac)
{
    // Gary add note : this function is deleted since PAX use its own EMV lib
    /*
	int k, m, iRetVal;
	usint usTmpLen,usTxType=0;
	window(1,1,SCREEN_MAX_COL,SCREEN_MAX_ROW);
	 
    
    OS_ClearScreen();
    display_format(	SCREEN_MAX_ROW/2-2+1,"ENTER",ALIGN_CENTER);
    display_format(SCREEN_MAX_ROW/2-1+1,"CUSTOMER PIN",ALIGN_CENTER);
    k= (SCREEN_MAX_COL-mx)/2+1+1;
    if (k<1) k=1;
    m=k+mx;
    if (m>SCREEN_MAX_COL) m=SCREEN_MAX_COL;

    window(k,SCREEN_MAX_ROW/2+1,m,SCREEN_MAX_ROW/2+1);

    iRetVal=SVC_KEY_NUM1(value,typ,mx,mn);
    window(1,1,SCREEN_MAX_COL,SCREEN_MAX_ROW);
    if (iRetVal>0) return 0;
    return iRetVal;
    */
	return 0;
}

BYTE Get_ScreenMaxRowCol(BYTE RowCol)
{
    return RowCol ? 128 : 64;
}

int SVC_KEY_NUM1(char *value, int type, int mx,int mn)
{
    return 0;
    // Gary add note : this function is deleted since PAX use its own EMV lib

    /*
    char bufnum[30];
    char bufdsp[30];
    char bufkey[2];
    int i, ser, j,len;
	long event;
	long Key_entry;			   
	long Key_idle;			  

	read( ghConsole, bufnum, sizeof(bufnum) );

	memset(bufnum,0,sizeof(bufnum));
	memset(bufkey,0,sizeof(bufkey));
    j = 0;

	Key_entry=set_itimeout(h_clock,30,TM_SECONDS);
	Key_idle=set_timer(30000,EVT_TIMER);

	while(1)
    {
        event = wait_event();
        if ( EVT_TIMER & event )
        {
            if (!CHK_TIMEOUT(h_clock,Key_entry))
            {
                return -2;
            }
        }
        if ( EVT_KBD & event )
        {
            i = read(ghConsole, bufkey, 1);
            bufkey[0]&=0x7F;
            
            if ((unsigned char)bufkey[0] == ESC)
            {
                value[0] = 0;
                clr_timer(Key_idle);
                return -1;
            }
            ser = 0;
            
            if ((ser=((bufkey[0]>=KEY0)&&(bufkey[0]<=KEY9)&&((j+1)<=mx)))!=0) 
            {
                bufkey[1] = 0;
                strcat(bufnum, bufkey);
                j++;
            }
            else if ((ser=((bufkey[0]==KEY_BS)))!=0) // agt13082006
            {
                if ( j > 0 )
                {
                    bufkey[1] = 0;
                    bufnum[strlen(bufnum)-1] = 0;
                    j--;
                }
                else
                {
                    memset(bufnum,0,sizeof(bufnum));
                    memset(bufkey,0,sizeof(bufkey));
                    j = 0;
                    continue;
                }
            }

            len =strlen(bufnum);
	        if (ser)
            {
                if (type == 3)
                {
                    memset(bufdsp,'*',len);
                    bufdsp[len]=0;
                }
                else
                {
                    strcpy(bufdsp,bufnum);
                }
                
                display_at (0,7, bufdsp, CLR_EOL);
            }
            
            if (bufkey[0]==KEY_CR)
            {
                if (len >= mn)
                {
                    strcpy(value,bufnum);
                    clr_timer(Key_idle);
                    return 1;
                }
            }
        }
    }
    */
}

void display_format(int y,char *msg_str,int align)
{
    int x;
	 char TmpBuf[250]={0};
	 usint usLen=strlen(msg_str);	
	 
	 if (usLen > 250) {return;}
		
	 memcpy(TmpBuf,msg_str,usLen);

    if (align == ALIGN_LEFT)
    {
        x=1;
    }
    else if (align == ALIGN_RIGHT)
    {
        x=22-usLen;
    }
    else 
    {
        x=usLen;
        if (x<21) x= ((21-x)/2) + 1;
        else x=1;

    }
    display_at(x,y,(char *)TmpBuf,CLR_LINE);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
\brief InputAlphaNum().                                                                    
Reads AlphaNumeric and writes as ASCII in RetBuf.
Returns 0x00 if cancel, exit or TO, other: cnt of entered vals.
Function button changes Alpha-Num state.
Screen Formatting:	Header(Line1)->CSTMSG1(BOLD, INVERT);	
Line2->CSTMSG2(Default: GÝRÝNÝZ); 
Line3->For alpha input allowed mode: 'A' or 'N' appears to indicate the state.
"AA/YY" or "GG/AA/YYYY" or "SS:DD" guides appear for revelant Date/Time entries.
Line4->CSTMSG3
'1' key gives signs in alpha state.
If MaskOk is set, stars will be displayed instead of entered chars.
InputAlignment should be JUSTIFY_LEFT, JUSTIFY_RIGHT or JUSTIFY_CENTER
CSTMSG4: Default Str. If given (not NULL), shown initially at input line, if entered, copied to RetBuf.
If first byte of CSTMSG4 (CSTMSG4[0]) is: 
0x01 -> Manual PAN Entry, Following default string will not be deleted, rather appended.
0x02 -> Date Entry format MM/YY, Default string deleted on any entry. 
0x03 -> Date Entry format DD/MM/YYYY, Default string deleted on any entry.
0x04 -> Time Entry format HH:MM, Default string deleted on any entry.
0x05 -> IP Entry format NNN.NNN.NNN.NNN
*/
#define NON_KB_VAL		0xFE
#define NUM_STATE		0x00
#define ALPHA_STATE		0x01
#define NR_OF_SIGNS		26												// Number of special signs in alpha mode
#define DSPL			21												// Display Buffer Length
// FormatFlag values:
#define NOFORMAT		0x00											// 0x00: No special format.
#define DESTROYDEFAULT	0x01											// 0x01: Destroy if input.
#define PANENTRYFORMAT	0x02											// 0x02: Pan Entry Format.
#define DATE_MMYY		0x04											// 0x04: Date Entry, MM/YY Format.
#define DATE_DDMMYYYY	0x08											// 0x08: Date Entry, DD/MM/YYYY Format.
#define TIME_HHMM		0x10											// 0x10: Time Entry, HH:MM Format.
#define IPENTRYFORMAT	0x20											// 0x20: IP Entry, NNN.NNN.NNN.NNN
#define NOCLEARDISP		0x40											// 0x40: Dont clear display
BYTE InputAlphaNum(MSG_Struct *stMSG, char *RetBuf, BYTE MinLen, BYTE MaxLen, BYTE AlphaOk, BYTE MaskOk, BYTE InputAlignment, BYTE TOSeconds)
{
	static const char AlphaTab[10][3] = {
        {'-', 'S', 'P'}, {'Q', 'Z', '.'}, {'A', 'B', 'C'},
        {'D', 'E', 'F'}, {'G', 'H', 'I'}, {'J', 'K', 'L'},
        {'M', 'N', 'O'}, {'P', 'R', 'S'}, {'T', 'U', 'V'},
        {'W', 'X', 'Y'}
    };
	static const char SignTab[NR_OF_SIGNS] = {
        'Q','Z','.', ' ', ',', ':', ';', '(', ')', '-', '+', '/', '*', '@', 
	    '&', '%', '!', '?', '~', '_', '#', '<', '>', '[', ']', '\\'
    };
	BYTE FormatFlag = NOFORMAT;		
	char RetChar = 0;
	char cnt = 0;
	BYTE StrLen = strlen(stMSG->CSTMSG1), TempLen = 0;
	BYTE LineMax, Offset,Src;
	BYTE AlphaNumState = NUM_STATE;				// Default state.
	BYTE AlphaSituation = 0x00;					// 0,1 or 2 for 0-9      (ABC for 2) (" .,:;()-+/*@&%!?~_#<>[]\" for 1)
	DWORD   DeciSecond = 0;
	BYTE PrevChar = NOKEY;

    OS_ClearScreen();

	// If special format PAN, IP, DATE entry, Alpha input is disabled.
	if ((stMSG->CSTMSG4[0] == 0x01) || (stMSG->CSTMSG4[0] == 0x02) ||
        (stMSG->CSTMSG4[0] == 0x03) || (stMSG->CSTMSG4[0] == 0x04) || (stMSG->CSTMSG4[0] == 0x05))
    {
		AlphaOk = FALSE;
    }

	// If IP Entry format, get fixed length of 12. (4x3)
	if (stMSG->CSTMSG4[0] == 0x05)
    {
		MaxLen = MinLen = 12;										// Max and min entry is set to 12.
		InputAlignment = JUSTIFY_CENTER;							// These entries are allowed only with Center alignment.
		MaskOk = FALSE;												// Masking is not allowed in these entries.
	}
	// If DATE entry in format MM/YY.
	if ((stMSG->CSTMSG4[0] == 0x02) || (stMSG->CSTMSG4[0] == 0x04))
    {
		MaxLen = MinLen = 0x04;										// Max and min entry is set to 4.
		InputAlignment = JUSTIFY_CENTER;							// These entries are allowed only with Center alignment.
		MaskOk = FALSE;												// Masking is not allowed in these entries.
	}
	// If DATE entry in format DD/MM/YYYY.
	if (stMSG->CSTMSG4[0] == 0x03)
    {
		MaxLen = MinLen = 0x08;										// Max and min entry is set to 8.
		InputAlignment = JUSTIFY_CENTER;							// This entry is allowed only with Center alignment.
		MaskOk = FALSE;												// Masking is not allowed in this entry.
	}

	// HEADER - LINE 1 ----------------------------------------------------------------------------------------
	if (stMSG->CSTMSG1[0])		// Header Line, if exists
    {
		if (StrLen >= SMALL_FONT_LINE_MAX)
        {
			StrLen = SMALL_FONT_LINE_MAX;
			LineMax = SMALL_FONT_LINE_MAX;
		}
        else if (StrLen > LARGE_FONT_LINE_MAX)
        {
			LineMax = SMALL_FONT_LINE_MAX;
		}
        else
        {
			LineMax = LARGE_FONT_LINE_MAX;
		}
		memset(stMSG->CSTMSG8, ' ', LG_CSTMSG);					// Fill Viewable area with spaces
		memcpy(&stMSG->CSTMSG8[(int)((LineMax - StrLen) / 2)], stMSG->CSTMSG1, StrLen);	// Center Justify to CSTMSG8
		if (stMSG->CSTMSG4[0] != NOCLEARDISP)
        {
			Display_Inverst(stMSG->CSTMSG8, 0);
		}
	}

	// LINE 2 -------------------------------------------------------------------------------------------------
	if (!stMSG->CSTMSG2[0])											// Input Type Name
    {
		memcpy(stMSG->CSTMSG2, "ENTER  ", 7);						// Default String for "Input Type Name" is "GÝRÝNÝZ"
    }
	StrLen = (BYTE)strlen(stMSG->CSTMSG2);							// Find length of the string
	if (StrLen > DSPL)
    {
		StrLen = DSPL;
    }

	memset(stMSG->CSTMSG8, ' ', DSPL);								// Fill Viewable area with spaces
	if (gLanguage==ARABIC)
    {
//OS_LogTo(DEBUG_PORT,"step_INPUT"); 			
		memcpy(&stMSG->CSTMSG8[(int)((16 - StrLen) )], stMSG->CSTMSG2, StrLen);// Center Justify to CSTMSG8


		write_at(stMSG->CSTMSG8, 16, 1, 1);		
	}	
	else
    {
		memcpy(&stMSG->CSTMSG8[(int)((DSPL - StrLen) / 2)], stMSG->CSTMSG2, StrLen);// Center Justify to CSTMSG8
	 	if (stMSG->CSTMSG4[0] != NOCLEARDISP)
        {
			display_at(0, 2, stMSG->CSTMSG8, NO_CLEAR );
		}
	}
	
	// LINE 3 -------------------------------------------------------------------------------------------------     
	if (stMSG->CSTMSG3[0])		// Input Type Name
    { 
		StrLen = (BYTE)strlen(stMSG->CSTMSG3);						// Find length of the string
		if (StrLen > DSPL)
			StrLen = DSPL;
		memset(stMSG->CSTMSG8, ' ', DSPL);							// Fill Viewable area with spaces
		
		if (gLanguage==ARABIC)
        {
			memcpy(&stMSG->CSTMSG8[(int)((16 - StrLen))], stMSG->CSTMSG3, StrLen);	// Center Justify to CSTMSG8
			write_at(stMSG->CSTMSG8, 16, 1, 2);
		}	
		else
        {
			memcpy(&stMSG->CSTMSG8[(int)((DSPL - StrLen) / 2)], stMSG->CSTMSG3, StrLen);	// Center Justify to CSTMSG8
			display_at(0, 3, stMSG->CSTMSG8, NO_CLEAR);
		}	
	}
	//set_font("Nadir1.fon");
	
	memset(stMSG->CSTMSG3, ' ', 26);								// Init Alpha-Num indicating line.

	while (RetChar != NOKEY)
    {
	    // LINE 4 SHOW ALPHA-NUM IF ALPHA ALLOWED, "AA/YY" OR "GG/AA/YYYY" STRINGS FOR PROPER ENTRY TYPE ------
	    if (AlphaOk)
        {													// If alpha input allowed.
		     if (AlphaNumState == ALPHA_STATE) 			
			    display_at(0, 6, "         (A)", NO_CLEAR );	// Alpha input mode
		    else			
			    display_at(0, 6, "         (N)", NO_CLEAR );	// Numeric input mode
	    }
        else if ((stMSG->CSTMSG4[0] == 0x02) || (FormatFlag & DATE_MMYY))	// MM/YY Entry.
        {
		    memcpy (stMSG->CSTMSG3, "        AA/YY", 13);
		    display_at(0, 6, stMSG->CSTMSG3, NO_CLEAR);
		    memset(stMSG->CSTMSG3, ' ', 26);
	    }
        else if ((stMSG->CSTMSG4[0] == 0x03) || (FormatFlag & DATE_DDMMYYYY))// DD/MM/YYYY Entry.
        {
		    memcpy (stMSG->CSTMSG3, "     GG/AA/YYYY", 15);
		    display_at(0, 6, stMSG->CSTMSG3, NO_CLEAR);
		    memset(stMSG->CSTMSG3, ' ', 26);
	    }
        else if ((stMSG->CSTMSG4[0] == 0x04) || (FormatFlag & TIME_HHMM))	// HH:MM Entry.
        {
		    memcpy (stMSG->CSTMSG3, "        SS:DD", 13);
		    display_at(0, 6, stMSG->CSTMSG3, NO_CLEAR);
		    memset(stMSG->CSTMSG3, ' ', 26);
	    }

	    // LINE 5 INPUT LINE, DEFAULT STRING ------------------------------------------------------------------
	    if (stMSG->CSTMSG4[0])	// If default string exists
        {
		    if (stMSG->CSTMSG4[0] == 0x01)	// Pan entry, do not delete Default string...
            {
			    FormatFlag = PANENTRYFORMAT;
			    memcpy(stMSG->CSTMSG7, &stMSG->CSTMSG4[1], (LG_CSTMSG - 1)); // Use default string for once, copy to input buffer.
		    }
            else if (stMSG->CSTMSG4[0] == 0x02)	// Date Entry, MM/YY Format. 
            {
			    memcpy(stMSG->CSTMSG7, &stMSG->CSTMSG4[1], (LG_CSTMSG - 1));// Use default string for once, copy to input buffer.
			    FormatFlag = DATE_MMYY;									// Special date format.
			    FormatFlag |= DESTROYDEFAULT;							// Flag default string, so if entry made, will be destroyed.
		    }
            else if (stMSG->CSTMSG4[0] == 0x03)	// Date Entry, DD/MM/YYYY Format. 
            {
			    memcpy(stMSG->CSTMSG7, &stMSG->CSTMSG4[1], (LG_CSTMSG - 1));// Use default string for once, copy to input buffer.
			    FormatFlag = DATE_DDMMYYYY;								// Special date format.
			    FormatFlag |= DESTROYDEFAULT;							// Flag default string, so if entry made, will be destroyed.
		    }
            else if (stMSG->CSTMSG4[0] == 0x04)	// Time Entry, HH:MM Format. 
            {
			    memcpy(stMSG->CSTMSG7, &stMSG->CSTMSG4[1], (LG_CSTMSG - 1));// Use default string for once, copy to input buffer.
			    FormatFlag = TIME_HHMM;									// Special time format.
			    FormatFlag |= DESTROYDEFAULT;							// Flag default string, so if entry made, will be destroyed.
		    }
            else if (stMSG->CSTMSG4[0] == 0x05)	// IP Entry Format.    ___.___.___.___
            {
			    FormatFlag = IPENTRYFORMAT;	// Special IP Entry format.
			    if (stMSG->CSTMSG4[1])
                {								// If Default IP Exists.
				    memset(stMSG->CSTMSG7, 0, LG_CSTMSG);
				    memcpy(&stMSG->CSTMSG7[0], &stMSG->CSTMSG4[1], 3);
				    memcpy(&stMSG->CSTMSG7[3], &stMSG->CSTMSG4[5], 3);
				    memcpy(&stMSG->CSTMSG7[6], &stMSG->CSTMSG4[9], 3);
				    memcpy(&stMSG->CSTMSG7[9], &stMSG->CSTMSG4[13], 3);				
				    FormatFlag |= DESTROYDEFAULT;	// Flag default string, so if entry made, will be destroyed.
			    }
		    }
            else if (stMSG->CSTMSG4[0] == 0x40)
            {
			    memcpy(stMSG->CSTMSG7, &stMSG->CSTMSG4[1], (LG_CSTMSG - 1));// Use default string for once, copy to input buffer.
			    FormatFlag = DESTROYDEFAULT;							// Flag default string, so if entry made, will be destroyed.
		    }
            else	// Delete Default string mode...
            {
			    memcpy(stMSG->CSTMSG7, &stMSG->CSTMSG4[1], LG_CSTMSG);// Use default string for once, copy to input buffer.
			    FormatFlag = DESTROYDEFAULT;					// Flag default string, so if entry made, will be destroyed.
		    }

		    memset(stMSG->CSTMSG4, 0, LG_CSTMSG);					// Clear not to repeat.
		    cnt = strlen(stMSG->CSTMSG7);							// Set initial length.
		    if (MaskOk)
			    memset(stMSG->CSTMSG6, '*', cnt);					// Fill with stars
		    else
			    memcpy(stMSG->CSTMSG6, stMSG->CSTMSG7, LG_CSTMSG);	// Copy input str to input display str.	
	    }
	 cnt = strlen(stMSG->CSTMSG7);	
	    if (FormatFlag & IPENTRYFORMAT)	// IP Entry special formatting.
        {
		    memcpy(stMSG->CSTMSG6, "___.___.___.___", 15);
		    memcpy(stMSG->CSTMSG6, stMSG->CSTMSG7, ((cnt > 3) ? 3 : cnt));// Copy input str to input display str.	
		    if (cnt > 3)
			    memcpy(&stMSG->CSTMSG6[4], &stMSG->CSTMSG7[3], ((cnt > 6) ? 3 : (cnt - 3)));	// Copy rest of the input str to input display str.	
		    if (cnt > 6)
			    memcpy(&stMSG->CSTMSG6[8], &stMSG->CSTMSG7[6], ((cnt > 9) ? 3 : (cnt - 6)));	// Copy rest of the input str to input display str.	
		    if (cnt > 9)
			    memcpy(&stMSG->CSTMSG6[12], &stMSG->CSTMSG7[9], (cnt - 9));	// Copy rest of the input str to input display str.	
	    }
        else if ((FormatFlag & PANENTRYFORMAT) && (cnt > 4))	// PAN entry special formatting.
        {
		    memcpy(stMSG->CSTMSG6, stMSG->CSTMSG7, 4);					// Copy input str to input display str.	
		    memset(&stMSG->CSTMSG6[4], 0, LG_CSTMSG - 4);
		    stMSG->CSTMSG6[4] = 0x20;
		    memcpy(&stMSG->CSTMSG6[5], &stMSG->CSTMSG7[4], cnt);		// Copy rest of the input str to input display str.	
		    if (cnt > 8)
            {
			    memset(&stMSG->CSTMSG6[9], 0, LG_CSTMSG - 9);
			    stMSG->CSTMSG6[9] = 0x20;
			    memcpy(&stMSG->CSTMSG6[10], &stMSG->CSTMSG7[8], cnt - 8);// Copy rest of the input str to input display str.	
		    }
		    if (cnt > 12)
            {
			    memset(&stMSG->CSTMSG6[14], 0, LG_CSTMSG - 14);
			    stMSG->CSTMSG6[14] = 0x20;
			    memcpy(&stMSG->CSTMSG6[15], &stMSG->CSTMSG7[12], cnt - 12);	// Copy rest of the input str to input display str.	
		    }
		    if (cnt > 16)
            {
			    memset(&stMSG->CSTMSG6[19], 0, LG_CSTMSG - 19);
			    stMSG->CSTMSG6[19] = 0x20;
			    memcpy(&stMSG->CSTMSG6[20], &stMSG->CSTMSG7[16], cnt - 16);	// Copy rest of the input str to input display str.	
		    }
	    }
        else if (FormatFlag & DATE_MMYY)	// Date Entry, MM/YY Format.
        {
		    if (stMSG->CSTMSG7[0])	// If default string exists
            {
			    stMSG->CSTMSG6[0] = stMSG->CSTMSG7[0];					// 0
			    if (cnt > 1)
				    stMSG->CSTMSG6[1] = stMSG->CSTMSG7[1];				// 1
			    else
				    stMSG->CSTMSG6[1] = '.';							// 1
			    stMSG->CSTMSG6[2] = '/';								// 2 Seperator character '/'.
			    if (cnt > 2)
				    stMSG->CSTMSG6[3] = stMSG->CSTMSG7[2];				// 3
			    else
				    stMSG->CSTMSG6[3] = '.';							// 3
			    if (cnt > 3)
				    stMSG->CSTMSG6[4] = stMSG->CSTMSG7[3];				// 4
			    else
				    stMSG->CSTMSG6[4] = '.';							// 4
		    } else
            {
			    memcpy(stMSG->CSTMSG6, "../..", 5);						// Blank MM/YY input.
            }
	    }
        else if (FormatFlag & DATE_DDMMYYYY)	// Date Entry, DD/MM/YYYY Format. 
        {
		    if (stMSG->CSTMSG7[0])	// If default string exists
            {
			    stMSG->CSTMSG6[0] = stMSG->CSTMSG7[0];					// 0
		        if (cnt > 1)
			        stMSG->CSTMSG6[1] = stMSG->CSTMSG7[1];					// 1
		        else
			        stMSG->CSTMSG6[1] = '.';								// 1
		        stMSG->CSTMSG6[2] = '/';									// 2 Seperator character '/'.
		        if (cnt > 2)
			        stMSG->CSTMSG6[3] = stMSG->CSTMSG7[2];					// 3
		        else
			        stMSG->CSTMSG6[3] = '.';								// 3
		        if (cnt > 3)
			        stMSG->CSTMSG6[4] = stMSG->CSTMSG7[3];					// 4
		        else
			        stMSG->CSTMSG6[4] = '.';								// 4
		        stMSG->CSTMSG6[5] = '/';									// 5 Seperator character '/'.
		        if (cnt > 4)
			        stMSG->CSTMSG6[6] = stMSG->CSTMSG7[4];					// 6
		        else
			        stMSG->CSTMSG6[6] = '.';								// 6
		        if (cnt > 5)
			        stMSG->CSTMSG6[7] = stMSG->CSTMSG7[5];					// 7
		        else
			        stMSG->CSTMSG6[7] = '.';								// 7
		        if (cnt > 6)
			        stMSG->CSTMSG6[8] = stMSG->CSTMSG7[6];					// 8
		        else
			        stMSG->CSTMSG6[8] = '.';								// 8
		        if (cnt > 7)
			        stMSG->CSTMSG6[9] = stMSG->CSTMSG7[7];					// 9
		        else
			        stMSG->CSTMSG6[9] = '.';								// 9
		    }
            else
            {
			    memcpy(stMSG->CSTMSG6, "../../....", 10);				// Blank DD/MM/YYYY input.
            }
	    }
        else if (FormatFlag & TIME_HHMM)	// TIME Entry, HH:MM Format.
        {
		    if (stMSG->CSTMSG7[0])	// If default string exists
            {
			    stMSG->CSTMSG6[0] = stMSG->CSTMSG7[0];					// 0
			    if (cnt > 1)
				    stMSG->CSTMSG6[1] = stMSG->CSTMSG7[1];				// 1
			    else
				    stMSG->CSTMSG6[1] = '.';							// 1
			    stMSG->CSTMSG6[2] = ':';								// 2 Seperator character '/'.
			    if (cnt > 2)
				    stMSG->CSTMSG6[3] = stMSG->CSTMSG7[2];				// 3
			    else
				    stMSG->CSTMSG6[3] = '.';							// 3
			    if (cnt > 3)
				    stMSG->CSTMSG6[4] = stMSG->CSTMSG7[3];				// 4
			    else
				    stMSG->CSTMSG6[4] = '.';							// 4
		    }
            else
            {
			    memcpy(stMSG->CSTMSG6, "..:..", 5);			// Blank HH:MM input.
            }
	    }

	    StrLen = (BYTE)strlen(stMSG->CSTMSG6);							// Find length of the string
	    if (StrLen > DSPL)
		    TempLen = DSPL;
	    else
		    TempLen = StrLen;
	    memset(stMSG->CSTMSG8, 0x20, DSPL);								// Fill Viewable area with spaces
	    if (InputAlignment == JUSTIFY_LEFT)
        {
		    memcpy(stMSG->CSTMSG8, &stMSG->CSTMSG6[(StrLen > DSPL) ? (StrLen - DSPL) : 0], TempLen);	// Left Justify to CSTMSG8
	    }
        else if (InputAlignment == JUSTIFY_RIGHT)
        {
		    memcpy(&stMSG->CSTMSG8[DSPL - StrLen], &stMSG->CSTMSG6[(StrLen > DSPL) ? (StrLen - DSPL) : 0], TempLen);	// Right Justify to CSTMSG8
	    }
        else
        {	// if (InputAlignment == JUSTIFY_CENTER)
		    memcpy(&stMSG->CSTMSG8[(StrLen >= DSPL) ? 0 : ((int)((DSPL - StrLen) / 2))], &stMSG->CSTMSG6[(StrLen > DSPL) ? (StrLen - DSPL) : 0], TempLen);	// Center Justify to CSTMSG8
	    }

	    display_at(0, 7, stMSG->CSTMSG8, NO_CLEAR );
	    if ( (RetChar=GetOneKeyCheckTime(TOSeconds, &DeciSecond)) != NOKEY )
        {
		    if ((FormatFlag & DESTROYDEFAULT) && (RetChar != KEYENTER))		// Default string on display, destroy. Return if enter pressed.
            {	
			    memset(stMSG->CSTMSG7, 0x00, LG_CSTMSG);				// Reset input buffer.
			    memset(stMSG->CSTMSG6, 0x00, LG_CSTMSG);				// Reset input display buffer.
			    cnt = 0;
			    FormatFlag &= ~DESTROYDEFAULT;							// Reset Destroy default string flag.
		    }
//by jarod 2011.06.02
		    if ( RetChar==KEYALPHA )	// FN button pressed, 
            {							
			    if (AlphaOk)
                {											// Change AlphaNumState if Alpha allowed
				    if (AlphaNumState == ALPHA_STATE)
                    {
					    AlphaNumState = NUM_STATE;
				    }
                    else
                    {
					    AlphaNumState = ALPHA_STATE;
					    PrevChar = NOKEY;							// Reset Alpha vars
					    AlphaSituation = 0;
				    }
			    }
		    }
            else
            {
			    if (AlphaNumState == NUM_STATE)	// Numerical input state
                {
				    PrevChar = NOKEY;								// Reset Alpha vars
				    AlphaSituation = 0;
				    if ((RetChar >= KEY0) && (RetChar <= KEY9))
                    {
					    if (FormatFlag & IPENTRYFORMAT)	// IP Entry special formatting, linit with "255"
                        {
						    if (((cnt % 3) == 0) && (RetChar > '2')) 
							    continue;
						    else if (((cnt % 3) == 1)  && (stMSG->CSTMSG7[cnt - 1] == '2') && (RetChar > '5'))
							    continue;
						    else if (((cnt % 3) == 2)  && (stMSG->CSTMSG7[cnt - 2] == '2') && (stMSG->CSTMSG7[cnt - 1] == '5') && (RetChar > '5'))
							    continue;
					    }
					    if (cnt < MaxLen)	// Maksimum girilebilir basamak.
                        {
						    stMSG->CSTMSG7[cnt++] = RetChar;
					    }
				    }
                    else if (RetChar==KEYCLEAR)	// BACKSPACE ---
                    {
					    if (cnt)
						    cnt--;
					    stMSG->CSTMSG7[cnt] = 0x00;						// CSTMSG7 Buffer for plain input string
				    }
//                  else if (RetChar == 65)
//                  {
// 					    cnt = 0x00;										// Reset cnt
// 					    memset(stMSG->CSTMSG7, 0x00, LG_CSTMSG);		// CSTMSG7 Buffer for plain input string
// 				    }
                    else if (RetChar==KEYCANCEL)
                    {
					    return 0x00;
				    }
			    }
                else	// (AlphaNumState == ALPHA_STATE)			// Alpha input state.
                {
				    if ( (RetChar >= KEY1) && (RetChar <= KEY9) )	// Alphabetical
                    {
					    if (cnt < MaxLen)	// Maksimum girilebilir basamak.
                        {
						    if (PrevChar == RetChar)    // if repeatly press same key
                            {
							    if (DeciSecond <= 10)	// and if within 1 second.
                                {
								    cnt--;
								    stMSG->CSTMSG7[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];	
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= 3;				// To maintain cyclic consistency
							    }
                                else
                                {
								    AlphaSituation = 0;					// TO, Reset AlphaSituation
								    PrevChar = RetChar;
								    stMSG->CSTMSG7[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= 3;				// To maintain cyclic consistency
							    }
						    }
                            else
                            {
							    AlphaSituation = 0;						// Reset AlphaSituation
							    PrevChar = RetChar;
							    stMSG->CSTMSG7[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];
							    AlphaSituation++;						// Increase Next Situation.
							    AlphaSituation %= 3;					// To maintain cyclic consistency
						    }
					    }
                        else	// Reached Maximum Input, accept only if no TO.
                        {
						    if ((DeciSecond <= 10) && (PrevChar == RetChar))	// If not TO and same key
                            {
							    cnt--;									// No TO, Proceed on prev char.
							    stMSG->CSTMSG7[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];	
							    AlphaSituation++;						// Increase Next Situation.
							    AlphaSituation %= 3;					// To maintain cyclic consistency
						    }
                            else
                            {
							    PrevChar = NON_KB_VAL;
							    AlphaSituation = 0;
						    }
					    }
				    }
                    else if (RetChar == KEY0)	// Signs.
                    {
					    if (cnt < MaxLen)
                        {
						    if (PrevChar == RetChar)
                            {
							    if (DeciSecond <= 10)	// Has TO ocurred?
                                {
								    cnt--;								// No TO, Proceed on prev char.
								    stMSG->CSTMSG7[cnt++] = SignTab[AlphaSituation];
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= NR_OF_SIGNS;		// To maintain cyclic consistency
							    }
                                else	// Next Sign
                                {
								    AlphaSituation = 0;					// TO, Reset AlphaSituation
								    PrevChar = RetChar;
								    stMSG->CSTMSG7[cnt++] = SignTab[AlphaSituation];
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= NR_OF_SIGNS;		// To maintain cyclic consistency
							    }
						    }
                            else
                            {
							    AlphaSituation = 0;						// Reset AlphaSituation
							    PrevChar = RetChar;
							    stMSG->CSTMSG7[cnt++] = SignTab[AlphaSituation];// Girilen deðeri al.
							    AlphaSituation++;						// Increase Next Situation.
							    AlphaSituation %= NR_OF_SIGNS;			// To maintain cyclic consistency
						    }
					    }
                        else
                        {
						    PrevChar = NON_KB_VAL;
						    AlphaSituation = 0;
					    }
				    }
                    else if (RetChar==KEYCLEAR)	// BACKSPACE ---
                    {						
					    if (cnt)
						    cnt--;
					    stMSG->CSTMSG7[cnt] = 0x00;						// CSTMSG7 Buffer for plain input string
					    PrevChar = NON_KB_VAL;							// Reset Alpha vars
					    AlphaSituation = 0;
				    }
                    else if (RetChar==KEYCANCEL)	// CANCEL, EXIT ---
                    {
					    return 0x00;
				    }
			    }

			    if (RetChar == KEYENTER)	// ENTER ---
                {
				    if (cnt >= MinLen)
                    {
					    if (FormatFlag & IPENTRYFORMAT)
                        {
						    memcpy(RetBuf, stMSG->CSTMSG7, 3);
						    RetBuf[3] = '.';
						    memcpy(&RetBuf[4], &stMSG->CSTMSG7[3], 3);
						    RetBuf[7] = '.';
						    memcpy(&RetBuf[8], &stMSG->CSTMSG7[6], 3);
						    RetBuf[11] = '.';
						    memcpy(&RetBuf[12], &stMSG->CSTMSG7[9], 3);
					    }
                        else
                        {
						    memcpy(RetBuf, stMSG->CSTMSG7, cnt);		// Copy Input to RetBuf
                        }
					    if (!cnt)	// If entered with no input (ZeroInputAllowed case)
                        {
						    RetBuf[0] = 0x00;							// Clear first val, which will be read.
						    return 0x01;							
					    }
					    return ((cnt == 0) ? 1 : cnt);					// Return counter.
				    }
                    else
                    {
					    PrevChar = NON_KB_VAL;							// Reset Alpha vars
					    AlphaSituation = 0;
				    }
			    }
		    } 
	    } 
        else
        {
		    return 0x00;
        }// TIMEOUT ---

	    if (MaskOk)
        {													// Should displayed be masked with stars
		    memset(stMSG->CSTMSG6, 0, LG_CSTMSG);						// Mask display
		    memset(stMSG->CSTMSG6, '*', cnt);
	    }
        else			// Do not mask display
        {
		    memcpy(stMSG->CSTMSG6, stMSG->CSTMSG7, LG_CSTMSG);
        }
	}

	return 0x00;
}




static void ConvertToDisplay(BYTE *pszIn, BYTE ucMode, BYTE FormatFlag, BYTE *pszOut)
{
    if (FormatFlag==IPENTRYFORMAT)
    {
        StrCpy(pszOut, "___.___.___.___");
        memcpy(pszOut+0, pszIn,   MIN(StrLen(pszIn), 3));
        memcpy(pszOut+4, pszIn+3, MIN(StrLen(pszIn)-3, 3));
        memcpy(pszOut+8, pszIn+6, MIN(StrLen(pszIn)-6, 3));
        memcpy(pszOut+12, pszIn+9, MIN(StrLen(pszIn)-9, 3));
    }
    else
    {
        strcpy(pszOut, pszIn);
    }
}

BYTE InputAlphaNum_PAX(MSG_Struct *stMSG, char *RetBuf, BYTE MinLen, BYTE MaxLen,
                       BYTE AlphaOk, BYTE InputAlignment, BYTE TOSeconds)
{
	static const char AlphaTab[10][3] = {
        {'-', 'S', 'P'}, {'Q', 'Z', '.'}, {'A', 'B', 'C'},
        {'D', 'E', 'F'}, {'G', 'H', 'I'}, {'J', 'K', 'L'},
        {'M', 'N', 'O'}, {'P', 'R', 'S'}, {'T', 'U', 'V'},
        {'W', 'X', 'Y'}
    };
	static const char SignTab[NR_OF_SIGNS] = {
        'Q','Z','.', ' ', ',', ':', ';', '(', ')', '-', '+', '/', '*', '@', 
	    '&', '%', '!', '?', '~', '_', '#', '<', '>', '[', ']', '\\'
    };
	BYTE FormatFlag = NOFORMAT;		
	char RetChar = 0;
	char cnt = 0;
	BYTE StrLen = strlen(stMSG->CSTMSG1), TempLen = 0;
	BYTE LineMax, Offset,Src;
	BYTE AlphaNumState = NUM_STATE;				// Default state.
	BYTE AlphaSituation = 0x00;					// 0,1 or 2 for 0-9      (ABC for 2) (" .,:;()-+/*@&%!?~_#<>[]\" for 1)
	DWORD   DeciSecond = 0;
	BYTE PrevChar = NOKEY;
    BYTE    sEnterBuffer[128], sDispBuff[128];
    int iDispLen;

    OS_ClearScreen();
    memset(sEnterBuffer, 0, sizeof(sEnterBuffer));

	// HEADER - LINE 1 ----------------------------------------------------------------------------------------
	if (stMSG->CSTMSG1[0])		// Header Line, if exists
    {
		if (StrLen >= SMALL_FONT_LINE_MAX)
        {
			LineMax = SMALL_FONT_LINE_MAX;
		}
        else if (StrLen > LARGE_FONT_LINE_MAX)
        {
			LineMax = SMALL_FONT_LINE_MAX;
		}
        else
        {
			LineMax = LARGE_FONT_LINE_MAX;
		}
        if (LineMax==LARGE_FONT_LINE_MAX)
        {
            ScrPrint(0, 0, CFONT|REVER, "%-16.16s", stMSG->CSTMSG1);
        }
        else
        {
            ScrPrint(0, 0, ASCII|REVER, "%-21.21s", stMSG->CSTMSG1);
        }
	}

	// LINE 2 -------------------------------------------------------------------------------------------------
	if (!stMSG->CSTMSG2[0])											// Input Type Name
    {
		memcpy(stMSG->CSTMSG2, "ENTER  ", 7);						// Default String for "Input Type Name" is "GÝRÝNÝZ"
    }
	StrLen = (BYTE)strlen(stMSG->CSTMSG2);							// Find length of the string
	if (StrLen > SMALL_FONT_LINE_MAX)
    {
		StrLen = SMALL_FONT_LINE_MAX;
    }
	
//by jarod 2011.05.26
//    if (StrLen>LARGE_FONT_LINE_MAX)
//    {
//        ScrPrint(0, 2, ASCII, "%-21.21s",stMSG->CSTMSG2);
//    }
//    else
//    {
//        ScrPrint(0, 2, CFONT, "%-16.16s", stMSG->CSTMSG2);
//    }
	
	// LINE 3 -------------------------------------------------------------------------------------------------     
	if (stMSG->CSTMSG3[0])		// Input Type Name
    { 
		StrLen = (BYTE)strlen(stMSG->CSTMSG3);						// Find length of the string
		if (StrLen > SMALL_FONT_LINE_MAX)
        {
			StrLen = SMALL_FONT_LINE_MAX;
        }
        ScrPrint(0, 4, ASCII, "%21.21s", stMSG->CSTMSG3);
	}

	// LINE 4 INPUT LINE, DEFAULT STRING ------------------------------------------------------------------
	if (stMSG->CSTMSG4[0])	// If default string exists
    {
		FormatFlag |= DESTROYDEFAULT;// Flag default string, so if entry made, will be destroyed.
		strcpy(sEnterBuffer, stMSG->CSTMSG4);// Copy input str to input display str.
	}

	while (RetChar != NOKEY)
    {
	    // LINE 4 SHOW various input state --------------------------------------------------------------------
	    if (AlphaOk)// If alpha input allowed.
        {													
            if (AlphaNumState == ALPHA_STATE) 			
                display_at(0, 5, "         (A)", NO_CLEAR );// Alpha input mode
            else			
                display_at(0, 5, "         (N)", NO_CLEAR );// Numeric input mode
        }
        else if (0)
        {
            // reserved.
	    }

	    if (FormatFlag & IPENTRYFORMAT)	// IP Entry special formatting.
        {
            // reserved.
	    }
        else if ((FormatFlag & PANENTRYFORMAT) && (cnt > 4))// PAN entry special formatting.
        {
            // reserved.
	    }
        else if (FormatFlag & DATE_MMYY)// Date Entry, MM/YY Format.
        {
            // reserved.
	    }
        else if (FormatFlag & DATE_DDMMYYYY)// Date Entry, DD/MM/YYYY Format. 
        {
            // reserved.
	    }
        else if (FormatFlag & TIME_HHMM)	// TIME Entry, HH:MM Format.
        {
            // reserved.
	    }

        ScrClrLine(6, 7);
        ConvertToDisplay(sEnterBuffer, InputAlignment, FormatFlag, sDispBuff);
        iDispLen = strlen(sDispBuff);
        cnt = strlen(sEnterBuffer);
	    if (InputAlignment == JUSTIFY_LEFT)
        {
            if (iDispLen<=16)
            {
                ScrPrint(0, 6, CFONT, "%s", sDispBuff);
            }
            else if (iDispLen<=21)
            {
                ScrPrint(0, 7, ASCII, "%s", sDispBuff);
            }
            else
            {
                ScrPrint(0, 6, ASCII, "%.21s", sDispBuff);
                ScrPrint(0, 7, ASCII, "%.21s", sDispBuff+21);
            }
	    }
        else if (InputAlignment == JUSTIFY_RIGHT)
        {
            if (iDispLen<=16)
            {
                ScrPrint(0, 6, CFONT, "%16.16s", sDispBuff);
            }
            else if (iDispLen<=21)
            {
                ScrPrint(0, 7, ASCII, "%21.21s", sDispBuff);
            }
            else
            {
                ScrPrint(0, 6, ASCII, "%21.*s", iDispLen-21, sDispBuff);
                ScrPrint(0, 7, ASCII, "%21.21s", sDispBuff+iDispLen-21);
            }
        }
        else// JUSTIFY_CENTER
        {
            if (iDispLen<=16)
            {
                ScrPrint((128-iDispLen*8)/2, 6, CFONT, "%.16s", sDispBuff);
            }
            else if (iDispLen<=21)
            {
                ScrPrint((128-iDispLen*6)/2, 7, ASCII, "%.21s", sDispBuff);
            }
            else
            {
                ScrPrint((128-(iDispLen-21)*6)/2, 6, ASCII, "%.*s", iDispLen-21, sDispBuff);
                ScrPrint(1, 7, ASCII, "%.21s", sDispBuff+iDispLen-21);
            }
        }

	    if ( (RetChar=GetOneKeyCheckTime(TOSeconds, &DeciSecond)) != NOKEY )
        {
		    if ((FormatFlag & DESTROYDEFAULT) && (RetChar != KEYENTER))// Default string on display, destroy. Return if enter pressed.
            {
                memset(sEnterBuffer, 0, sizeof(sEnterBuffer));
			    cnt = 0;
			    FormatFlag &= ~DESTROYDEFAULT;// Reset Destroy default string flag.
		    }

		    if ( RetChar==KEYALPHA )	// FN button pressed, 
            {							
			    if (AlphaOk)
                {											// Change AlphaNumState if Alpha allowed
				    if (AlphaNumState == ALPHA_STATE)
                    {
					    AlphaNumState = NUM_STATE;
				    }
                    else
                    {
					    AlphaNumState = ALPHA_STATE;
					    PrevChar = NOKEY;							// Reset Alpha vars
					    AlphaSituation = 0;
				    }
			    }
		    }
            else
            {
			    if (AlphaNumState==NUM_STATE)	// Numerical input state
                {
				    PrevChar = NOKEY;// Reset Alpha vars
				    AlphaSituation = 0;
				    if ((RetChar >= KEY0) && (RetChar <= KEY9))
                    {
					    if (cnt < MaxLen)
                        {
						    sEnterBuffer[cnt++] = RetChar;
                            sEnterBuffer[cnt]   = 0x00;
					    }
				    }
                    else if (RetChar==KEYCLEAR)
                    {
					    if (cnt)
                        {
						    cnt--;
                        }
					    sEnterBuffer[cnt] = 0x00;
				    }
                    else if (RetChar==KEYCANCEL)
                    {
					    return 0x00;
				    }
			    }
                else// Alpha input state.
                {
				    if ( (RetChar >= KEY1) && (RetChar <= KEY9) )	// Alphabetical
                    {
					    if (cnt < MaxLen)	// Maksimum girilebilir basamak.
                        {
						    if (PrevChar==RetChar)    // if repeatly press same key
                            {
							    if (DeciSecond <= 10)	// and if within 1 second.
                                {
								    cnt--;
								    sEnterBuffer[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];	
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= 3;				// To maintain cyclic consistency
							    }
                                else
                                {
								    AlphaSituation = 0;					// TO, Reset AlphaSituation
								    PrevChar = RetChar;
								    sEnterBuffer[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= 3;				// To maintain cyclic consistency
							    }
						    }
                            else
                            {
							    AlphaSituation = 0;						// Reset AlphaSituation
							    PrevChar = RetChar;
							    sEnterBuffer[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];
							    AlphaSituation++;						// Increase Next Situation.
							    AlphaSituation %= 3;					// To maintain cyclic consistency
						    }
					    }
                        else	// Reached Maximum Input, accept only if no TO.
                        {
						    if ((DeciSecond <= 10) && (PrevChar==RetChar))	// If not TO and same key
                            {
							    cnt--;									// No TO, Proceed on prev char.
							    sEnterBuffer[cnt++] = AlphaTab[RetChar - KEY0][AlphaSituation];	
							    AlphaSituation++;						// Increase Next Situation.
							    AlphaSituation %= 3;					// To maintain cyclic consistency
						    }
                            else
                            {
							    PrevChar = NOKEY;
							    AlphaSituation = 0;
						    }
					    }
				    }
                    else if (RetChar == KEY0)	// Signs.
                    {
					    if (cnt < MaxLen)
                        {
						    if (PrevChar==RetChar)
                            {
							    if (DeciSecond <= 10)	// Has TO ocurred?
                                {
								    cnt--;								// No TO, Proceed on prev char.
								    sEnterBuffer[cnt++] = SignTab[AlphaSituation];
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= NR_OF_SIGNS;		// To maintain cyclic consistency
							    }
                                else	// Next Sign
                                {
								    AlphaSituation = 0;					// TO, Reset AlphaSituation
								    PrevChar = RetChar;
								    sEnterBuffer[cnt++] = SignTab[AlphaSituation];
								    AlphaSituation++;					// Increase Next Situation.
								    AlphaSituation %= NR_OF_SIGNS;		// To maintain cyclic consistency
							    }
						    }
                            else
                            {
							    AlphaSituation = 0;						// Reset AlphaSituation
							    PrevChar = RetChar;
							    sEnterBuffer[cnt++] = SignTab[AlphaSituation];// Girilen deðeri al.
							    AlphaSituation++;						// Increase Next Situation.
							    AlphaSituation %= NR_OF_SIGNS;			// To maintain cyclic consistency
						    }
					    }
                        else
                        {
						    PrevChar = NOKEY;
						    AlphaSituation = 0;
					    }
				    }
                    else if (RetChar==KEYCLEAR)	// BACKSPACE ---
                    {						
					    if (cnt)
                        {
						    cnt--;
                        }
					    sEnterBuffer[cnt] = 0x00;
					    PrevChar = NOKEY;// Reset Alpha vars
					    AlphaSituation = 0;
				    }
                    else if (RetChar==KEYCANCEL)	// CANCEL, EXIT ---
                    {
					    return 0x00;
				    }
			    }

			    if (RetChar == KEYENTER)	// ENTER ---
                {
				    if (cnt >= MinLen)
                    {
					    if (FormatFlag & IPENTRYFORMAT)
                        {
						    memcpy(RetBuf, sEnterBuffer, 3);
						    RetBuf[3] = '.';
						    memcpy(&RetBuf[4], &sEnterBuffer[3], 3);
						    RetBuf[7] = '.';
						    memcpy(&RetBuf[8], &sEnterBuffer[6], 3);
						    RetBuf[11] = '.';
						    memcpy(&RetBuf[12], &sEnterBuffer[9], 3);
					    }
                        else
                        {
						    strcpy(RetBuf, sEnterBuffer);		// Copy Input to RetBuf
                        }
					    if (!cnt)	// If entered with no input (ZeroInputAllowed case)
                        {
						    RetBuf[0] = 0x00;// Clear first val, which will be read.
						    return 0x01;							
					    }
					    return ((cnt==0) ? 1 : cnt);					// Return counter.
				    }
                    else
                    {
					    PrevChar = NOKEY;							// Reset Alpha vars
					    AlphaSituation = 0;
				    }
			    }
		    } 
	    } 
        else
        {
		    return 0x00;// TIMEOUT ---
        }
	}

	return 0x00;
}

BYTE Enter_data(BYTE *chTemp,BYTE Type)
{
	MSG_Struct MainMSG;
	BYTE Min,Max,Alpha,Mask;
	char msg_prod_type[12]={35,92,1,81,23,112,140,32,91,128,120,0};
	char msg_prod_desc[12]={35,92,1,81,23,112,140,32,102,76,127,0};
	char msg_barcode[17]={102,130,64,93,27,112,140,32,86,130,64,72,32,118,104,63,0};
	char msg_MPIN1[17]={102,130,64,93,27,112,140,118,104,63,32,114,45,59,2,0};
	char msg_MPIN2[17]={99,64,77,117,112,140,114,131,108,128,112,0};
	char msg_CPIN1[17]={118,104,63,32,114,45,59,2,32,60,131,108,3,27,113,112,0};
	char msg_CPIN2[17]={132,77,46,73,112,140,32,102,130,64,93,27,112,140,0};



	ClearCustomMessages(&MainMSG);
	strcpy(MainMSG.CSTMSG4+1, chTemp);
	strcpy(MainMSG.CSTMSG7, chTemp);
	strcpy(MainMSG.CSTMSG6, chTemp);
	strcpy(MainMSG.CSTMSG2," ");
	switch (Type)
    {
		case 0:	//EntryType = IPADR;
			MainMSG.CSTMSG4[0] = 0x05;
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			memcpy(MainMSG.CSTMSG2, "TCP/IP", 6);
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=Max=16;
			Alpha=0;
			Mask=0;
			break;
		case 1:	//APN
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			memcpy(MainMSG.CSTMSG2, "APN", 3);
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=1;
			Max=LG_APN;
			Alpha=1;
			Mask=0;
			break;
		case 2:	//user
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			memcpy(MainMSG.CSTMSG2, "USER", 4);
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=0;
			Max=LG_USER;	//modified by Kevin v0.05.0016 20150617
			Alpha=1;
			Mask=0;
			break;
		case 3:	//pwd
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			memcpy(MainMSG.CSTMSG2, "PASSWORD", 8);
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=0;
			Max=LG_PASSWORD;		//modified by Kevin v0.05.0016 20150617
			Alpha=1;
			Mask=0;
			break;
		case 4:	//port
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			memcpy(MainMSG.CSTMSG2, "PORT", 4);
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=1;
			Max=5;
			Alpha=0;
			Mask=0;
			break;			
		case 5:   //barcode--Already use another way. abandon it.
			/*
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			if (gLanguage==ARABIC)
            {
				memcpy(MainMSG.CSTMSG2,msg_barcode,16);
//OS_LogTo(DEBUG_PORT,"step_BARCODE"); 		
			}				
			else
            {
				memcpy(MainMSG.CSTMSG2, "MANUALLY ENTER",14);
				memcpy(MainMSG.CSTMSG3, "THE BARCODE    ",15);
			}	
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=1;
			Max=13;
			Alpha=0;
			Mask=0;
			*/
			break;			
		case 6:
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			if (gLanguage==ARABIC)
            {
//OS_LogTo(DEBUG_PORT,"step_prodtype"); 										
				memcpy(MainMSG.CSTMSG2,msg_prod_type,11);
			}	
			else
            {
				memcpy(MainMSG.CSTMSG2, "PRODUCT TYPE", 12);
            }
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=3;
			Max=20;
			Alpha=1;
			Mask=0;
			break;		
		case 7:
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			if (gLanguage==ARABIC)
            {
//OS_LogTo(DEBUG_PORT,"step_proddesc"); 					
				memcpy(MainMSG.CSTMSG2,msg_prod_desc,11);
			}	
			else
				memcpy(MainMSG.CSTMSG2, "PROD. DESCR.", 12);
			
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=0;
			Max=50;
			Alpha=1;
			Mask=0;
			break;				
		case 8:   //merchant PIN
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			if (gLanguage==ARABIC)
            {
				memcpy(MainMSG.CSTMSG2,msg_MPIN1,16);
				memcpy(MainMSG.CSTMSG3, msg_MPIN2,16);
//OS_LogTo(DEBUG_PORT,"step_MPIN"); 		
			}				
			else
            {
				memcpy(MainMSG.CSTMSG2, "ENTER",14);
				memcpy(MainMSG.CSTMSG3, "MERCHANT PIN   ",15);
			}	
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=4;
			Max=4;
			Alpha=0;
			Mask=1;
			break;			
		case 9:   //cust PIN
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			if (gLanguage==ARABIC)
            {
				memcpy(MainMSG.CSTMSG2, msg_CPIN1,16);
				memcpy(MainMSG.CSTMSG3, msg_CPIN2,14);
//OS_LogTo(DEBUG_PORT,"step_MPIN"); 		
			}				
			else
            {
				memcpy(MainMSG.CSTMSG2, "ENTER",14);
				memcpy(MainMSG.CSTMSG3, "CUSTOMER PIN   ",15);
			}	
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=4;
			Max=4;
			Alpha=0;
			Mask=1;
			break;
		//added by Allen 2016-05-19
		case 10:
			memset(MainMSG.CSTMSG2,0, LG_CSTMSG);
			memcpy(MainMSG.CSTMSG2, "Tel.No", 6);
			MainMSG.CSTMSG8[0] = NOCLEARDISP; 
			Min=7;
			Max=20;
			Alpha=0;
			Mask=0;
			break;		
		//add end
	}
	return (InputAlphaNum(&MainMSG, chTemp, Min, Max, Alpha, Mask, JUSTIFY_CENTER, /*TO_30SEC*/TO_1MIN));
}	



#define SELECT_SIGN_1		0x3E //'>'
#define SELECT_SIGN_2		0xA  //'<'
#define CHOICES_PER_PAGE64	4
#define CHOICES_PER_PAGE128	7
#define CHOICES_PER_PAGE128_TITLE	6
#define LG_MENUINDEX		50

int ShowMenu(menu *m, BYTE TOSeconds, BYTE OptByte)
{
	MSG_Struct stMSG;
	BYTE ChoicesPerPage;
	BYTE CurrentChoice = 0, CurrentFrameStart = 0, KeyGot, LineMax;
	BYTE StrLen = (BYTE)menu_strnlen(m->header,LG_CSTMSG);
	BYTE PrevNumKey = 0, PrevTouchSel = 0;
	ulong CurTimeStamp = 0;
	ulong SeqKeyTO = 0;													// Sequential key press timeout.
	ulong TouchPadTO = 0;												// Touchpad double click timeout.
	char IndexTab[LG_MENUINDEX][3] = {	" 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", " 10", 
										" 11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
										"21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
										"31", "32", "33", "34", "35", "36", "37", "38", "39", "40",
										"41", "42", "43", "44", "45", "46", "47", "48", "49", "50"	};
	BYTE DownCalib =  0;	// 8000/8010
	BYTE Font;
	BOOLEAN TouchProc = TRUE;

	DownCalib = 0;
	ChoicesPerPage = CHOICES_PER_PAGE128_TITLE;

	OS_ClearScreen();
						
	// Menu Header --------------------------------------------------------------------------------------------
	if (StrLen >= SMALL_FONT_LINE_MAX)
    {
		StrLen = SMALL_FONT_LINE_MAX;
		LineMax = SMALL_FONT_LINE_MAX;
	}
	else if (StrLen > LARGE_FONT_LINE_MAX)
    {
		LineMax = SMALL_FONT_LINE_MAX;
    }
	else
    {
		LineMax = LARGE_FONT_LINE_MAX;
    }

	for (;;)
    {
		OS_ClearScreen();	// Gary add, 2011-1-26
		ClearCustomMessages(&stMSG);											// Clear custom messages.
		
		// Header Line ----------------------------------------------------------------------------------------
		memset(stMSG.CSTMSG1, ' ', LineMax);								// Fill Viewable area with spaces
		memcpy(&stMSG.CSTMSG1[(LineMax - StrLen) / 2], m->header, StrLen);	// Center Justify to CSTMSG1

#if 0
//#ifndef DEVELOP_VERSION // Gary TODO : select suitable fonts
		if (LineMax == LARGE_FONT_LINE_MAX)
		{
			Font = ??;										// Set large fonts
		}
		else
		{
			Font = ??;										// Set small fonts
		}
#else
        Font = 0;
#endif

		if (OptByte & Opt_ARABIC)
        {
            // ???? change fonts here
			//display_at(0, 1, stMSG.CSTMSG1, NO_CLEAR);
		//	display_at(0, 0, stMSG.CSTMSG1, NO_CLEAR);
            		PubShowTitle(FALSE, stMSG.CSTMSG1);
		}
		else
        {
			//display_at(0, 1, stMSG.CSTMSG1, NO_CLEAR);
			//display_at(0, 0, stMSG.CSTMSG1, NO_CLEAR);
			PubShowTitle(FALSE, stMSG.CSTMSG1);
        }

#if 0
//#ifndef DEVELOP_VERSION
        Font = ??;	// Gary note: TODO : resolve the font	// Init for choice lines.
#endif

		// Menu Entry 1 -----------------------------------------------------------------------------------
		if (OptByte & Opt_LONGMSG)
        {	//@bm new
			strcpy(stMSG.CSTMSG2, "  .                  ");		
			memcpy(&stMSG.CSTMSG2[0], IndexTab[CurrentFrameStart], 2);
			strcpy(&stMSG.CSTMSG2[3], m->menu_entries[CurrentFrameStart].text);
		}
        else
        {
			strcpy(stMSG.CSTMSG2, "   .                 ");		
			memcpy(&stMSG.CSTMSG2[1], IndexTab[CurrentFrameStart], 2);
			strcpy(&stMSG.CSTMSG2[4], m->menu_entries[CurrentFrameStart].text);
		}

		stMSG.CSTMSG2[menu_strnlen((char *)m->menu_entries[CurrentFrameStart].text,LG_CSTMSG) + 4] = ' ';
		if (CurrentChoice == 0)
        {			
			if (!(OptByte & Opt_SKIPSINGS))
            {
				stMSG.CSTMSG2[0] = SELECT_SIGN_1;
			}				
		}
		//display_at(0,3, stMSG.CSTMSG2, NO_CLEAR );
	//	display_at(0, 1, stMSG.CSTMSG2, NO_CLEAR );
		display_at(0, 2, stMSG.CSTMSG2, NO_CLEAR );

		// Menu Entry 2 -----------------------------------------------------------------------------------
		if ((CurrentFrameStart + 1) < m->no_of_entries)
        {
			if (OptByte & Opt_LONGMSG)
            {
				strcpy(stMSG.CSTMSG3, "  .                  ");
				memcpy(&stMSG.CSTMSG3[0], IndexTab[CurrentFrameStart + 1], 2);
				strcpy(&stMSG.CSTMSG3[3], m->menu_entries[CurrentFrameStart + 1].text);
			}
            else
            {
				strcpy(stMSG.CSTMSG3, "   .                 ");
				memcpy(&stMSG.CSTMSG3[1], IndexTab[CurrentFrameStart + 1], 2);
				strcpy(&stMSG.CSTMSG3[4], m->menu_entries[CurrentFrameStart + 1].text);
			}
			stMSG.CSTMSG3[menu_strnlen((char *)m->menu_entries[CurrentFrameStart + 1].text,LG_CSTMSG) + 4] = ' ';
			if (CurrentChoice == 1)
            {
				if (!(OptByte & Opt_SKIPSINGS))
                {
					stMSG.CSTMSG3[0] = SELECT_SIGN_1;
				}
			}
			//display_at( 0,4, stMSG.CSTMSG3, NO_CLEAR );
		//	display_at(0, 2, stMSG.CSTMSG3, NO_CLEAR );	
			display_at(0, 3, stMSG.CSTMSG3, NO_CLEAR );	
		}
        else
        {
			memset(stMSG.CSTMSG3, 0, LG_CSTMSG);
			//display_at( 0,4, stMSG.CSTMSG3, NO_CLEAR );
			//display_at(0, 2, stMSG.CSTMSG3, NO_CLEAR );
			display_at(0, 3, stMSG.CSTMSG3, NO_CLEAR );
		}

		// Menu Entry 3 -----------------------------------------------------------------------------------
		if ((CurrentFrameStart + 2) < m->no_of_entries)
        {			
			if (OptByte & Opt_LONGMSG)
            {
				strcpy(stMSG.CSTMSG4, "  .                  ");
				memcpy(&stMSG.CSTMSG4[0], IndexTab[CurrentFrameStart + 2], 2);
				strcpy(&stMSG.CSTMSG4[3], m->menu_entries[CurrentFrameStart + 2].text);
			}
            else
            {
				strcpy(stMSG.CSTMSG4, "   .                 ");
				memcpy(&stMSG.CSTMSG4[1], IndexTab[CurrentFrameStart + 2], 2);
				strcpy(&stMSG.CSTMSG4[4], m->menu_entries[CurrentFrameStart + 2].text);			
			}
			stMSG.CSTMSG4[menu_strnlen((char *)m->menu_entries[CurrentFrameStart + 2].text,LG_CSTMSG) + 4] = ' ';
			if (CurrentChoice == 2)
            {				
				if (!(OptByte & Opt_SKIPSINGS))
                {
					stMSG.CSTMSG4[0] = SELECT_SIGN_1;
				}
			}
			//display_at( 0,5, stMSG.CSTMSG4, NO_CLEAR );
			//display_at(0, 3, stMSG.CSTMSG4, NO_CLEAR );	
			display_at(0, 4, stMSG.CSTMSG4, NO_CLEAR );	
		}
        else
        {
			memset(stMSG.CSTMSG4, 0, LG_CSTMSG);
			//display_at( 0,5, stMSG.CSTMSG4, NO_CLEAR );
			//display_at(0, 3, stMSG.CSTMSG4, NO_CLEAR );
			display_at(0, 4, stMSG.CSTMSG4, NO_CLEAR );
		}

		// Menu Entry 4 -----------------------------------------------------------------------------------
		if ((CurrentFrameStart + 3) < m->no_of_entries)
        {
			if (OptByte & Opt_LONGMSG)
            {
				if((CurrentFrameStart+3)>8)
				{
					strcpy(stMSG.CSTMSG5, "   .                  ");
					memcpy(&stMSG.CSTMSG5[0], IndexTab[CurrentFrameStart + 3], 3);
					strcpy(&stMSG.CSTMSG5[4], m->menu_entries[CurrentFrameStart + 3].text);
				}
				else
				{
					strcpy(stMSG.CSTMSG5, "  .                  ");
					memcpy(&stMSG.CSTMSG5[0], IndexTab[CurrentFrameStart + 3], 2);
					strcpy(&stMSG.CSTMSG5[3], m->menu_entries[CurrentFrameStart + 3].text);
				}
			} else {
				strcpy(stMSG.CSTMSG5, "   .                 ");
				memcpy(&stMSG.CSTMSG5[1], IndexTab[CurrentFrameStart + 3], 2);
				strcpy(&stMSG.CSTMSG5[4], m->menu_entries[CurrentFrameStart + 3].text);
			}
			stMSG.CSTMSG5[menu_strnlen((char *)m->menu_entries[CurrentFrameStart + 3].text,LG_CSTMSG) + 4] = ' ';
			if (CurrentChoice == 3)
            {
				if (!(OptByte & Opt_SKIPSINGS))
                {
					stMSG.CSTMSG5[0] = SELECT_SIGN_1;
				}
			}
			//display_at( 0,6, stMSG.CSTMSG5, NO_CLEAR );
			//display_at(0, 4, stMSG.CSTMSG5, NO_CLEAR );	
			display_at(0, 5, stMSG.CSTMSG5, NO_CLEAR );
		}
        else
        {
			memset(stMSG.CSTMSG5, 0, LG_CSTMSG);
			//display_at( 0,6, stMSG.CSTMSG5, NO_CLEAR );
			//display_at(0, 4, stMSG.CSTMSG5, NO_CLEAR );
			display_at(0, 5, stMSG.CSTMSG5, NO_CLEAR );
		}
		
		// Menu Entry 5 -----------------------------------------------------------------------------------
		if ((CurrentFrameStart + 4) < m->no_of_entries)
        {
			if (OptByte & Opt_LONGMSG)//??
            {
				if((CurrentFrameStart+4)>8)
				{
					strcpy(stMSG.CSTMSG6, "   .                  ");
					memcpy(&stMSG.CSTMSG6[0], IndexTab[CurrentFrameStart + 4], 3);
					strcpy(&stMSG.CSTMSG6[4], m->menu_entries[CurrentFrameStart + 4].text);
				}
				else
				{
					strcpy(stMSG.CSTMSG6, "  .                  ");
					memcpy(&stMSG.CSTMSG6[0], IndexTab[CurrentFrameStart + 4], 2);
					strcpy(&stMSG.CSTMSG6[3], m->menu_entries[CurrentFrameStart + 4].text);
				}
			}
            else
            {
				strcpy(stMSG.CSTMSG6, "   .                 ");
				memcpy(&stMSG.CSTMSG6[1], IndexTab[CurrentFrameStart + 4], 2);
				strcpy(&stMSG.CSTMSG6[4], m->menu_entries[CurrentFrameStart + 4].text);				
			}
			stMSG.CSTMSG6[menu_strnlen((char *)m->menu_entries[CurrentFrameStart + 4].text,LG_CSTMSG) + 4] = ' ';
			if (CurrentChoice == 4)
            {
				if (!(OptByte & Opt_SKIPSINGS))
                {
					stMSG.CSTMSG6[0] = SELECT_SIGN_1;
				}
			}
			//display_at( 0,7, stMSG.CSTMSG6, NO_CLEAR );
			//display_at(0, 5, stMSG.CSTMSG6, NO_CLEAR );	
			display_at(0, 6, stMSG.CSTMSG6, NO_CLEAR );
		}
		else
        {
			memset(stMSG.CSTMSG6, 0, LG_CSTMSG);
			//display_at(0,7, stMSG.CSTMSG6, NO_CLEAR);
			//display_at(0, 5, stMSG.CSTMSG6, NO_CLEAR);
			display_at(0, 6, stMSG.CSTMSG6, NO_CLEAR );
		}
		// IF 8000/8010, 1(HEADER) + 8(CHOICES) LINES EXIST PER PAGE. *****************************************
		if (ChoicesPerPage == CHOICES_PER_PAGE128_TITLE)
        {
			// Menu Entry 6 -----------------------------------------------------------------------------------
			if ((CurrentFrameStart + 5) < m->no_of_entries)
            {
				if (OptByte & Opt_LONGMSG)
                {
					strcpy(stMSG.CSTMSG7, "  .                  ");
					memcpy(&stMSG.CSTMSG7[0], IndexTab[CurrentFrameStart + 5], 2);
					strcpy(&stMSG.CSTMSG7[3], m->menu_entries[CurrentFrameStart + 5].text);
				}
                else
                {
					strcpy(stMSG.CSTMSG7, "   .                 ");
					memcpy(&stMSG.CSTMSG7[1], IndexTab[CurrentFrameStart + 5], 2);
					strcpy(&stMSG.CSTMSG7[4], m->menu_entries[CurrentFrameStart + 5].text);				
				}
				stMSG.CSTMSG7[menu_strnlen((char *)m->menu_entries[CurrentFrameStart + 5].text,LG_CSTMSG) + 4] = ' ';
				if (CurrentChoice == 5)
                {
					if (!(OptByte & Opt_SKIPSINGS))
                    {
						stMSG.CSTMSG7[0] = SELECT_SIGN_1;
					}
				}
				//display_at(0,8, stMSG.CSTMSG7, NO_CLEAR );
				//display_at(0, 6, stMSG.CSTMSG7, NO_CLEAR );	
				display_at(0, 7, stMSG.CSTMSG7, NO_CLEAR );
			}
			else
            {
				memset(stMSG.CSTMSG7, 0, LG_CSTMSG);
				//display_at(0,8, stMSG.CSTMSG7, NO_CLEAR );
				//display_at(0, 6, stMSG.CSTMSG7, NO_CLEAR );
				display_at(0, 7, stMSG.CSTMSG7, NO_CLEAR );
			}

//			// Menu Entry 7 -----------------------------------------------------------------------------------
//			if ((CurrentFrameStart + 6) < m->no_of_entries)
//            {
//				if (OptByte & Opt_LONGMSG)
//                {
//					strcpy(stMSG.CSTMSG8, "  .                  ");
//					memcpy(&stMSG.CSTMSG8[0], IndexTab[CurrentFrameStart + 6], 2);
//					strcpy(&stMSG.CSTMSG8[3], m->menu_entries[CurrentFrameStart + 6].text);
//				}
//                else
//                {
//					strcpy(stMSG.CSTMSG8, "   .                 ");
//					memcpy(&stMSG.CSTMSG8[1], IndexTab[CurrentFrameStart + 6], 2);
//					strcpy(&stMSG.CSTMSG8[4], m->menu_entries[CurrentFrameStart + 6].text);				
//				}
//				stMSG.CSTMSG8[menu_strnlen((char *)m->menu_entries[CurrentFrameStart + 6].text,LG_CSTMSG) + 4] = ' ';
//				if (CurrentChoice == 6)
//                {
//					if (!(OptByte & Opt_SKIPSINGS))
//                    {
//						stMSG.CSTMSG8[0] = SELECT_SIGN_1;
//					}
//				}
//				//display_at( 0, 9, stMSG.CSTMSG8, NO_CLEAR );
//				display_at(0, 7, stMSG.CSTMSG8, NO_CLEAR );
//			}
//			else
//            {
//				memset(stMSG.CSTMSG8, 0, LG_CSTMSG);
//				//display_at( 0, 9, stMSG.CSTMSG8, NO_CLEAR );
//				display_at(0, 7, stMSG.CSTMSG8, NO_CLEAR );
// 			}
		}		
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// DRAWING DONE, GET CHOICE
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		KeyGot = GetOneKey(TOSeconds);
		switch (KeyGot)
        {
			case KEYCANCEL:				// EXIT
				ScrCls();
				return 0;
			case KEYDOWN:				// Down Arrow
				CurrentChoice++;
				if ((CurrentFrameStart + CurrentChoice) >= m->no_of_entries)
                {
					CurrentChoice = 0;
					CurrentFrameStart = 0;
				}
				else if (CurrentChoice == ChoicesPerPage)
                {
					CurrentChoice = 0;
					CurrentFrameStart += ChoicesPerPage;
					if (CurrentFrameStart >= m->no_of_entries)
                    {
						CurrentChoice = 0;
						CurrentFrameStart = 0;
					}
				}
				break;
			case KEYUP:					// Up Arrow
				if (CurrentChoice == 0)
                {	
					if (CurrentFrameStart >= ChoicesPerPage)
                    {
						CurrentChoice = ChoicesPerPage - 1;
						CurrentFrameStart -= ChoicesPerPage;
					}
					else
                    {		
						if (!(m->no_of_entries % ChoicesPerPage))
                        {	
							CurrentChoice = ChoicesPerPage - 1;
							CurrentFrameStart = m->no_of_entries - ChoicesPerPage;
						}
						else
                        {
							CurrentChoice = ((m->no_of_entries % ChoicesPerPage) - 1);
							CurrentFrameStart = m->no_of_entries - (m->no_of_entries % ChoicesPerPage);
						}
					}
				}
				else
                {
					CurrentChoice--;
                }
				break;
			//case KEY0:    remove by Allen 2016-07-14:  i find that if i enter a menu and i press key 0 , the application will crash. so just remove it . 
			case KEY1:
			case KEY2:
			case KEY3:
			case KEY4:
			case KEY5:
			case KEY6:
			case KEY7:
			case KEY8:
			case KEY9:
				if ((Scan_MillSec() - SeqKeyTO) <= (HALFSECOND))
                {		// If no TO, sequential key press.
					KeyGot -= KEY0;
					KeyGot += PrevNumKey;
					PrevNumKey = 0;
					SeqKeyTO = 0;										// Reset timer.
					if (KeyGot <= m->no_of_entries) 
					{					// Valid entry
						CurrentFrameStart = (KeyGot - (KeyGot % ChoicesPerPage));
						if (!(KeyGot % ChoicesPerPage))
						{
							CurrentFrameStart -= ChoicesPerPage;
						}
						CurrentChoice = (KeyGot - CurrentFrameStart - 1);
					}
				}
				else
				{													// TO
					if (KeyGot != KEY0) 
					{									// If not '0'
						KeyGot -= KEY0;									// Calibrate from ascii to numerical value.
						if (KeyGot <= m->no_of_entries) 
						{				// Valid entry
							SeqKeyTO = Scan_MillSec();					// Restart Sequential key press timer.
							PrevNumKey = KeyGot * 10;
							CurrentFrameStart = (KeyGot - (KeyGot % ChoicesPerPage));
							if (!(KeyGot % ChoicesPerPage))
							{
								CurrentFrameStart -= ChoicesPerPage;
							}
							CurrentChoice = (KeyGot - CurrentFrameStart - 1);
						}
					}
				}
				break;
         
			case KEYENTER:
				OS_ClearScreen();
				return (sint)(CurrentFrameStart + CurrentChoice + 1);       
		}	
	}

	return 0;
}

#define LG_AMTDEFAULTBUF	12
#define NOCLEARDISP			0x40											// 0x40: Dont clear display
BYTE InputAmountExt(MSG_Struct * stMSG, BYTE * RetBuf, BOOLEAN ZeroInputOK, sint minlen, sint maxlen, char *CurDes)
{
	char    RetChar = (char)0xFF;
	BYTE    cnt;
	BYTE    ucLen;
    char    szEnterBuff[32], szDispBuff[32];
	
	OS_ClearScreen();
	
	if (stMSG->CSTMSG1[0])// Header Line, if exists
	{
        ucLen = StrLen(stMSG->CSTMSG1);
        if (ucLen>LARGE_FONT_LINE_MAX)
        {
            ScrPrint((128-ucLen*6)/2, 0, ASCII|REVER, "%.21s", stMSG->CSTMSG1);
        }
        else
        {
            ScrPrint((128-ucLen*8)/2, 0, CFONT|REVER, "%.16s", stMSG->CSTMSG1);
        }
	}
		
	if (stMSG->CSTMSG2[0])	// Input Type Name 
    {
        ucLen = StrLen(stMSG->CSTMSG2);
        if (ucLen>LARGE_FONT_LINE_MAX)
        {
            ScrPrint((128-ucLen*6)/2, 2, ASCII|REVER, "%.21s", stMSG->CSTMSG2);  //modified by Allen for ZAIN 2016-05-18
        }
        else
        {
            ScrPrint((128-ucLen*8)/2, 2, CFONT|REVER, "%.16s", stMSG->CSTMSG2); //modified by Allen for ZAIN 2016-05-18
        }
	}
	
	if (stMSG->CSTMSG3[0])	// If exists
    {
        ucLen = StrLen(stMSG->CSTMSG3);
        if (ucLen>LARGE_FONT_LINE_MAX)
        {
            ScrPrint((128-ucLen*6)/2, 2, ASCII, "%.21s", stMSG->CSTMSG3);
        }
        else
        {
            ScrPrint((128-ucLen*8)/2, 2, CFONT, "%.16s", stMSG->CSTMSG3);
        }
	}

	memset(szEnterBuff, 0, sizeof(szEnterBuff));
    Utils_ConvertBcdToAsc(RetBuf, szEnterBuff, 6);
    if (strcmp(szEnterBuff, "000000000000")==0)
    {
        memset(szEnterBuff, 0, sizeof(szEnterBuff));
    }
    cnt = StrLen(szEnterBuff);
	while (RetChar != 0)
    {
		//DebugComSend("RetChar = %02x\n", RetChar);
        memset(szDispBuff, 0, sizeof(szDispBuff));
		FormatAmt(szDispBuff, szEnterBuff, CurDes, ASCII_IN);	// Format string in YTL style to CSTMSG6
		PubTrimHeadChars(szDispBuff, ' ');
		
		ScrClrLine(6, 7);
        ucLen = strlen(szDispBuff);
	//	/DebugComSend("szDispBuff = %s\n", szDispBuff);
	//	DebugComSend("szEnterBuff = %s\n", szEnterBuff);
	//	DebugComSend("ucLen = %d\n", ucLen);
		if (ucLen>16)
		{
			ScrPrint((128-ucLen*6)/2, 7, ASCII, "%.21s", szDispBuff);
		} 
		else
		{
			ScrPrint((128-ucLen*8)/2, 6, CFONT, "%.16s", szDispBuff);
		}

		if (RetChar = GetOneKey(TO_1MIN))
        {
			if ((RetChar >= KEY0) && (RetChar <= KEY9))
            {
				//DebugComSend("Key = %02x\n", RetChar);
				//DebugComSend("cnt = %d\n", cnt);
				if (cnt < maxlen)
                {
					if ((cnt != 0) || (RetChar != '0'))
                    {
						szEnterBuff[cnt++] = RetChar;
                    }
				}
			}
			else if (RetChar==KEYCLEAR) 
			{
				if (cnt)
				{
					szEnterBuff[--cnt] = 0x00;	
				}
			}
			else if ((RetChar==KEYCANCEL) || (RetChar==NOKEY)) 
			{
				OS_ClearScreen();
				return 0x00;
			} 
			else if (RetChar==KEYENTER)
			{
				if (cnt)
				{
                    ucLen = StrLen(szEnterBuff);
                    if (ucLen<LG_AMOUNT)
                    {
                        memmove(szEnterBuff+LG_AMOUNT-ucLen, szEnterBuff, ucLen+1);
                        memset(szEnterBuff, '0', LG_AMOUNT-ucLen);
                    }
                    Utils_ConvertAscToBcd(szEnterBuff, RetBuf, 12, 0, PAD_LEFT, '0');
					//Utils_ConvertAscToBcd("28737", RetBuf, 8, 0, PAD_LEFT, 'F');
					////DebugComSend("RetBufPAD_LEFT = %02x  %02x  %02x  %02x\n", *RetBuf, *(RetBuf+1), *(RetBuf+2), *(RetBuf+3));
					//Utils_ConvertAscToBcd("28737", RetBuf, 8, 0, PAD_RIGHT, 'F');
					OS_ClearScreen();
					//DebugComSend("RetBufPAD_LEFT = %02x  %02x  %02x  %02x\n", *RetBuf, *(RetBuf+1), *(RetBuf+2), *(RetBuf+3));
					return cnt;
				}
				if (!ZeroInputOK) 
				{
					if (!cnt)
                    {
						continue;
                    }
				}
				if (cnt < minlen)
				{
					continue;
				}
				Utils_ConvertAscToBcd(szEnterBuff, RetBuf, 12, 6, PAD_LEFT, 0);
				OS_ClearScreen();
				return ((cnt == 0) ? 1 : cnt);
			}
		} 
		else 
		{
			OS_ClearScreen();
			return 0x00;	
		}
	}

	return 0x00;
}

//added by Allen for ZAIN 2016-05-16
BYTE InputInvoiceNum(MSG_Struct * stMSG, unsigned char *pOutString)
{
	char szDisplayBuff[9] = {0};
	char Key;
	char *pDisPly = szDisplayBuff;
	int ucLen, cnt = 0;

	OS_ClearScreen();
	
	if (stMSG->CSTMSG1[0])// Header Line, if exists
	{
        ucLen = StrLen(stMSG->CSTMSG1);
        if (ucLen>LARGE_FONT_LINE_MAX)
        {
            ScrPrint((128-ucLen*6)/2, 0, ASCII|REVER, "%.21s", stMSG->CSTMSG1);
        }
        else
        {
            ScrPrint((128-ucLen*8)/2, 0, CFONT|REVER, "%.16s", stMSG->CSTMSG1);
        }
	}
	memcpy(szDisplayBuff, "00000000", 8);
	while(1)
	{
		ScrClrLine(6, 7);
		ScrPrint((128-8*8)/2, 6, CFONT, "%.8s", szDisplayBuff);  //change the ASCII to CFONT when this project finished 
		if(Key = GetOneKey(TO_1MIN))
		{
			if(Key >= KEY0 && Key <= KEY9 && cnt < 8)
			{
				*pDisPly++ = Key;
				cnt++;
			}
			else if(Key == KEYCLEAR && cnt > 0)
			{
				*(--pDisPly) = 0x30;
				cnt--;
			}
			else if (Key == KEYCANCEL) 
			{
				OS_ClearScreen();
				return 0x00;
			} 
			else if (Key == KEYENTER && cnt != 0)
			{
				memcpy(pOutString, szDisplayBuff, 8);
				return cnt;
			}
		}
	}
	return 0x00;
}

BYTE InputReferenceNum(MSG_Struct * stMSG, unsigned char *pOutString)
{
	char szDisplayBuff[13] = {0};
	char Key;
	char *pDisPly = szDisplayBuff;
	int ucLen, cnt = 0;

	OS_ClearScreen();
	
	if (stMSG->CSTMSG1[0])// Header Line, if exists
	{
        ucLen = StrLen(stMSG->CSTMSG1);
        if (ucLen>LARGE_FONT_LINE_MAX)
        {
            ScrPrint((128-ucLen*6)/2, 0, ASCII|REVER, "%.21s", stMSG->CSTMSG1);
        }
        else
        {
            ScrPrint((128-ucLen*8)/2, 0, CFONT|REVER, "%.16s", stMSG->CSTMSG1);
        }
	}

	if (stMSG->CSTMSG2[0])  ///here
	{
        ucLen = StrLen(stMSG->CSTMSG2);
        if (ucLen>LARGE_FONT_LINE_MAX)
        {
            ScrPrint((128-ucLen*6)/2, 2, ASCII|REVER, "%.21s", stMSG->CSTMSG2);
        }
        else
        {
            ScrPrint((128-ucLen*8)/2, 2, CFONT|REVER, "%.16s", stMSG->CSTMSG2);
        }
	}
	memcpy(szDisplayBuff, "000000000000", 12);
	while(1)
	{
		ScrClrLine(6, 7);
		ScrPrint((128-12*8)/2, 6, CFONT, "%.12s", szDisplayBuff);  //change the ASCII to CFONT when this project finished 
		if(Key = GetOneKey(TO_1MIN))
		{
			if(Key >= KEY0 && Key <= KEY9 && cnt < 12)
			{
				*pDisPly++ = Key;
				cnt++;
			}
			else if(Key == KEYCLEAR && cnt > 0)
			{
				*(--pDisPly) = 0x30;
				cnt--;
			}
			else if (Key == KEYCANCEL) 
			{
				OS_ClearScreen();
				return 0x00;
			} 
			else if (Key == KEYENTER && cnt != 0)
			{
				memcpy(pOutString, szDisplayBuff, 12);
				return cnt;
			}
		}
	}
	return 0x00;
}

BYTE InputPINNum(MSG_Struct * stMSG, unsigned char *pOutString)
{
	char *szDisplayBuff[5] = {"", "*", "**", "***", "****"};
	char szInputBuff[5] = {0};
	char Key;
	char *pDisPly = szInputBuff;
	int ucLen, cnt = 0;

	OS_ClearScreen();
	
	if (stMSG->CSTMSG1[0])// Header Line, if exists
	{
        ucLen = StrLen(stMSG->CSTMSG1);
        if (ucLen>LARGE_FONT_LINE_MAX)
        {
            ScrPrint(0, 3, ASCII, "%.21s", stMSG->CSTMSG1);
        }
        else
        {
            ScrPrint(0, 3, CFONT, "%.16s", stMSG->CSTMSG1);
        }
	}
	while(1)
	{
		ScrClrLine(6, 7);
		ScrPrint((128-4*8)/2, 6, CFONT, "%.4s", szDisplayBuff[cnt]);  //change the ASCII to CFONT when this project finished 
		if(Key = GetOneKey(TO_1MIN))
		{
			if(Key >= KEY0 && Key <= KEY9 && cnt < 4)
			{
				*pDisPly++ = Key;
				cnt++;
			}
			else if(Key == KEYCLEAR && cnt > 0)
			{
				*(--pDisPly) = 0x30;
				cnt--;
			}
			else if (Key == KEYCANCEL) 
			{
				OS_ClearScreen();
				return 0x00;
			} 
			else if (Key == KEYENTER && cnt == 4)
			{
				memcpy(pOutString, szInputBuff, 4);
				return cnt;
			}
		}
	}
	return 0x00;
}
 //add end
#define LARGE_FONT_MAX	9

BYTE ShowMessage(MSG_Struct *stMSG, BYTE TOSeconds, BYTE Mode, BYTE OptByte, BYTE Arabic, BYTE SMALL_FONT_MAX)
{
	BYTE LineMax;
	BYTE StrLen;
	BYTE i, j;


	if (Mode & 0xF0)	// Check buzzer off option
    {
		Mode &= 0x0F;		// Clear Buzzer Option.
    }
	else
    {
		if ((Mode == ERROR_MODE) || (Mode == WARNING_MODE)) 
			PubBeepErr();// beep(BUZZER_WARN);	
		else
			Beep();//beep(BUZZER_BEEP);
	}

	if (!(OptByte & MSG_DONT_CLEAR_SCR))
    {
		OS_ClearScreen();
	}

	if (Mode == WARNING_MODE)
    {
		memcpy(stMSG->CSTMSG4, "WARNING", 7);
		Mode = HEADER_MODE;
	}
    else if (Mode == ERROR_MODE)
    {
		memcpy(stMSG->CSTMSG4, "ERROR", 5);
		Mode = HEADER_MODE;
	}

	if (Mode == HEADER_MODE)
    {
		StrLen = strlen(stMSG->CSTMSG4);
		if (StrLen >= SMALL_FONT_MAX)
        {
			StrLen = SMALL_FONT_MAX;
			LineMax = SMALL_FONT_MAX;
		}
        else if (StrLen > LARGE_FONT_MAX)
        {
			LineMax = SMALL_FONT_MAX;
		}
        else
        {
			LineMax = SMALL_FONT_MAX;
		}

        memset(stMSG->CSTMSG8, 0, LG_CSTMSG);
		memset(stMSG->CSTMSG8, 0x20, LineMax);							// Fill Viewable area with spaces
		if (LineMax == LARGE_FONT_MAX)
        {
			j = 2;
		}
        else	// if (LineMax == SMALL_FONT_MAX)
        {
			j = 4;
		}

        memset(stMSG->CSTMSG8, 0, LG_CSTMSG);
		memset(&stMSG->CSTMSG8,0x20,LG_CSTMSG);
		memcpy(&stMSG->CSTMSG8[(LineMax - StrLen + j) / 2], stMSG->CSTMSG4, StrLen);	// Center Justify to CSTMSG8
		Display_Inverst(stMSG->CSTMSG8,0);
	}

	if (stMSG->CSTMSG1[0])		// If Line1 exists
    {
		StrLen = strlen(stMSG->CSTMSG1);
                                                OS_LogTo(DEBUG_PORT,"step_welcome %d",StrLen); 			
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
        memset(stMSG->CSTMSG11, 0, LG_CSTMSG);
		memset(stMSG->CSTMSG11, 0x20, SMALL_FONT_MAX);
		if (Arabic) 
			memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen )], stMSG->CSTMSG1, StrLen);	
		else
			memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG1, StrLen);	// Center Justify to CSTMSG7
		//display_at( 0,1, stMSG->CSTMSG11, NO_CLEAR );
		write_at(stMSG->CSTMSG11, SMALL_FONT_MAX, 1, 1);
	}

	if (stMSG->CSTMSG2[0])		// If Line2 exists
    {
		StrLen = strlen(stMSG->CSTMSG2);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
        memset(stMSG->CSTMSG11, 0, LG_CSTMSG);
		memset(stMSG->CSTMSG11, 0x20, SMALL_FONT_MAX);
		if (Arabic)
			memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen )], stMSG->CSTMSG2, StrLen);	
		else
			memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG2, StrLen);	// Center Justify to CSTMSG6
		write_at (stMSG->CSTMSG11, SMALL_FONT_MAX, 1, 2);
		//display_at( 0,2, stMSG->CSTMSG11, NO_CLEAR );
	}

	if (stMSG->CSTMSG3[0])	// If Line3 exists
    {
        StrLen = strlen(stMSG->CSTMSG3);
        if (StrLen > SMALL_FONT_MAX)
            StrLen = SMALL_FONT_MAX;
        memset(stMSG->CSTMSG11, 0, LG_CSTMSG);
        memset(stMSG->CSTMSG11, 0x20, SMALL_FONT_MAX);
        if (Arabic)
            memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen )], stMSG->CSTMSG3, StrLen);	
        else
            memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG3, StrLen);	// Center Justify to CSTMSG5
        //display_at( 0,3, stMSG->CSTMSG11, NO_CLEAR );
        write_at (stMSG->CSTMSG11, SMALL_FONT_MAX, 1, 3);
    }

    if (stMSG->CSTMSG4[0])		// If Line4 exists
    {
        StrLen = strlen(stMSG->CSTMSG4);
        if (StrLen > SMALL_FONT_MAX)
            StrLen = SMALL_FONT_MAX;
        memset(stMSG->CSTMSG11, 0, LG_CSTMSG);
        memset(stMSG->CSTMSG11, 0x20, SMALL_FONT_MAX);
        if (Arabic)
            memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen )], stMSG->CSTMSG4, StrLen);	
        else
            memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG4, StrLen);	
        //display_at( 0,4, stMSG->CSTMSG11, NO_CLEAR );
        write_at (stMSG->CSTMSG11, SMALL_FONT_MAX, 1, 4);
    }
    
    if (stMSG->CSTMSG5[0])		// If Line5 exists
    {
        StrLen = strlen(stMSG->CSTMSG5);
        if (StrLen > SMALL_FONT_MAX)
            StrLen = SMALL_FONT_MAX;
        memset(stMSG->CSTMSG11, 0, LG_CSTMSG);
        memset(stMSG->CSTMSG11, 0x20, SMALL_FONT_MAX);
        if (Arabic)
        {
            //memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen )], stMSG->CSTMSG5, StrLen);	
            memset(stMSG->CSTMSG11, 0, SMALL_FONT_MAX);
            memcpy(stMSG->CSTMSG11, stMSG->CSTMSG5, StrLen);	
        }	
        else
        {
            memcpy(&stMSG->CSTMSG11[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG5, StrLen);
        }
        //display_at( 0,5, stMSG->CSTMSG11, NO_CLEAR );
        write_at (stMSG->CSTMSG11, SMALL_FONT_MAX, 1, 5);
    }

	if (TOSeconds == TO_NOWAIT) 
		return 1;

	i = GetOneKey(TOSeconds);
	if (!(OptByte & YES_NO_MODE))
    {
		return i;	// Return the pressed key.
	}
	else
    {
		if ((i==KEYCANCEL) || (i==NOKEY))		// CANCEL EXIT or TO.
			return NOKEY;
		else
			return i;
	}		
}

void Disp_Mess(BYTE Type,BYTE Timeout)
{
#ifndef DEVELOP_VERSION
	MSG_Struct stMSG;
    // Gary : belows may be some kind of Arabic string ????
	char  s1_line1[17]={35,116,60,45,32,132,100,32,118,109,22,32,7,124,2,0};
	char  s1_line2[17]={35,42,22,140,64,117,112,140,0};
	char  s1_line3[17]={122,116,32,35,131,120,64,27,109,112,10,140,0};
	char  s1_line4[17]={132,23,88,128,22,2,32,99,64,77,116,0};
	char  s1_line5[17]={32,32,32,32,32,32,32,32,32,32,132,116,7,68,10,140,0};
	char  screen7[4][17]={
		{63,116,2,32,111,1,68,63,4,32,118,27,130,0},
		{19,112,4,32,20,140,64,73,112,140,32,63,116,2,0},
		{20,1,37,64,112,140,32,99,64,77,117,112,140,0},
		{63,1,89,27,120,10,140,0}};
	
	BYTE byArabicFl=0;
 
	ClearCustomMessages(&stMSG);
	switch (Type) 
	{
		case 1:
			/*strcpy(stMSG.CSTMSG1, "Welcome to E-Murabaha");
			strcpy(stMSG.CSTMSG2, "Service from ADIB");*/
            strcpy(stMSG.CSTMSG1, "( DISP_MESS:1 )");
			/*
            strcpy(stMSG.CSTMSG1, s1_line1);
            strcpy(stMSG.CSTMSG2, s1_line2);
            strcpy(stMSG.CSTMSG3, s1_line3);
            strcpy(stMSG.CSTMSG4, s1_line4);
            strcpy(stMSG.CSTMSG5, s1_line5);
			*/
			byArabicFl=1;
			break;
		case 2:	
			strcpy(stMSG.CSTMSG1, "Tenures options are");
			strcpy(stMSG.CSTMSG2, "being calculated");
			strcpy(stMSG.CSTMSG3, "by ADIB...");
			byArabicFl=0;
			break;
		case 3:
			strcpy(stMSG.CSTMSG1, "The customer contract");
			strcpy(stMSG.CSTMSG2, "is being prepareted");
			strcpy(stMSG.CSTMSG3, "by ADIB Plese wait");
			byArabicFl=0;
			break;
		case 4:
			if (gLanguage==ARABIC)
            {
                strcpy(stMSG.CSTMSG1,screen7[0]);
                strcpy(stMSG.CSTMSG2,screen7[1]);
                strcpy(stMSG.CSTMSG3,screen7[2]);
                strcpy(stMSG.CSTMSG4,screen7[3]);
				byArabicFl=1;
			}
			else
            {	
				strcpy(stMSG.CSTMSG1,"CUSTOMER'S ORDER TO");
				strcpy(stMSG.CSTMSG2,"PURCHASE IS BEING");
				strcpy(stMSG.CSTMSG3,"SENT TO THE BANK,");
				strcpy(stMSG.CSTMSG4,"PLEASE WAIT...");
				byArabicFl=0;
			}
			break;
		default :
			return;	
	}

	ShowMessage(&stMSG, Timeout, MESSAGE_MODE, 0, byArabicFl, (byArabicFl)?16:21);
#endif
}	

BYTE AskYesNo(MSG_Struct *stMSG, BYTE Type,BYTE offset) 
{ 
	BYTE key,StrLen,linemsg[50],SMALL_FONT_MAX;
	BYTE Buffer[LG_CSTMSG];		

	char msg_yes[17]={32,32,32,32,32,32,32,32,32,106,100,140,128,116,139,49};
	char msg_no[17]= {32,32,32,32,106,100,140,128,116,32,64,131,96,139,50};

	memset(Buffer,0,sizeof(Buffer));	
	OS_ClearScreen();
	memset(linemsg, 0, 50);

	if (gLanguage==ARABIC)
    {
		SMALL_FONT_MAX=16;	
	}

	if (offset==0)
    {
		SMALL_FONT_MAX=21;
	}

	if (stMSG->CSTMSG1[0])	// If Line1 exists
    {
		StrLen = strlen(stMSG->CSTMSG1);
		
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG1, StrLen);	// Center Justify to CSTMSG7
		write_at(  linemsg,SMALL_FONT_MAX,1,1);
		//display_at( 0,1, linemsg, NO_CLEAR );		
	}

	if (stMSG->CSTMSG2[0])	// If Line2 exists
    {
		StrLen = strlen(stMSG->CSTMSG2);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG2, StrLen);	// Center Justify to CSTMSG7
		write_at(  linemsg,SMALL_FONT_MAX,1,2);
		//display_at( 0,2, linemsg, NO_CLEAR );		
	}

	if (stMSG->CSTMSG3[0])	// If Line3 exists
    {
		StrLen = strlen(stMSG->CSTMSG3);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG3, StrLen);	// Center Justify to CSTMSG7
		write_at(  linemsg,SMALL_FONT_MAX,1,3);
		//display_at( 0,3, linemsg, NO_CLEAR );		
	}

	if (stMSG->CSTMSG4[0])	// If Line4 exists
    {
		StrLen = strlen(stMSG->CSTMSG4);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG4, StrLen);	// Center Justify to CSTMSG7
		write_at(  linemsg,SMALL_FONT_MAX,1,4);
		//display_at( 0,4, linemsg, NO_CLEAR );
	}

	if (stMSG->CSTMSG5[0])
    {											// If Line5 exists
		StrLen = strlen(stMSG->CSTMSG5);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG5, StrLen);	// Center Justify to CSTMSG7

//		PubWaitKey(1000);
		write_at(  linemsg,SMALL_FONT_MAX,1,5);
		
		//by jarod
//		StrCpy(Buffer,linemsg);
//		memmove(Buffer+2,Buffer+3,sizeof(Buffer)-2);
//		memset(Buffer+2,'M',1);
//		
 		write_at(  Buffer,SMALL_FONT_MAX,1,5);

		//display_at( 0,5, linemsg, NO_CLEAR );		
	}

	if (stMSG->CSTMSG6[0])	// If Line6 exists
    {
		StrLen = strlen(stMSG->CSTMSG6);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG6, StrLen);	// Center Justify to CSTMSG7
		write_at(  linemsg,SMALL_FONT_MAX,1,6+offset);
		//display_at( 0,6+offset, linemsg, NO_CLEAR );		
	}

	if (stMSG->CSTMSG7[0])	// If Line7 exists
    {
		StrLen = strlen(stMSG->CSTMSG7);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG7, StrLen);	// Center Justify to CSTMSG7
		write_at(  linemsg,SMALL_FONT_MAX,1,7+offset);
		//display_at( 0,7+offset, linemsg, NO_CLEAR );		
	}

	if (stMSG->CSTMSG8[0])	// If Line8 exists
    {
		StrLen = strlen(stMSG->CSTMSG8);
		if (StrLen > SMALL_FONT_MAX)
			StrLen = SMALL_FONT_MAX;
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG8, StrLen);	// Center Justify to CSTMSG7
		
		display_at( 0,8, linemsg, NO_CLEAR );
	}

	if (stMSG->CSTMSG9[0])		// If Line9 exists
    {
		StrLen = strlen(stMSG->CSTMSG9);
		if (StrLen > SMALL_FONT_MAX)
		{
			StrLen = SMALL_FONT_MAX;
		}
		memset(linemsg, 0x20, SMALL_FONT_MAX);

		memcpy(&linemsg[(SMALL_FONT_MAX - StrLen ) / 2], stMSG->CSTMSG9, StrLen);	// Center Justify to CSTMSG7
		display_at( 0,9, linemsg, NO_CLEAR );
	}

	if (!Type) 
	{
		return (1);
	}

	switch (Type)
    {
		case 1:	
			display_at( 0,11+offset, _T("1-YES"), NO_CLEAR );
			display_at( 0,12+offset, _T("2-NO"), NO_CLEAR );
			break;
		case 2:	
			display_at( 0,11+offset, _T("1-ACCEPT"), NO_CLEAR );
			display_at( 0,12+offset, _T("2-REJECT"), NO_CLEAR );
			break;
	}		

	while(TRUE)
    {
		key = GetOneKey(3);	
		switch (key)
        {
			case '1':
				return (1);
			case '2':
			case KEYCANCEL:
				return (0);
		}
	}
}   

BYTE Emv_AnalyzeTerminalAndCardAction(void)
{
    // Gary add note : this function is disabled since PAX has its own EMV lib
    // deleted
    return 0;
}
    
BYTE EMV_Complete(BYTE bCommResult)
{
    // Gary add note : this function is never called
    // deleted
    return 0;
}	
    
BYTE Emv_ProcessOnlineHostData(BYTE bCommResult)
{
    // Gary add note : this function is never called
    // deleted
    return 0;
}
//added by Allen 2016-07-12
void FormatPoints(char *pDest, char *pRes)
{
	char *pPointsBegin, *pPointsEnd;
	char cTmp[10] = {0};
	char cTmp1[5+1] = {0};
	int ilength;
	if(pRes == NULL)
	{
		return;
	}
	ilength = strlen(pRes);
	if(ilength <= 3)
	{
		pPointsBegin = cTmp1;
		memcpy(pPointsBegin, "0.000", 5);
		memcpy(pPointsBegin + 5 - ilength, pRes, ilength);
		strcpy(pDest, cTmp1); 
		return;
	}

	//DebugComSend("pRes = %s \n", pRes); 
	pPointsBegin = pPointsEnd = pRes;
	while(*pPointsEnd != 0)
		pPointsEnd++;
	pPointsEnd = pPointsEnd - 3;
	//DebugComSend("pPointsEnd = %s \n", pPointsEnd); 
	memcpy(cTmp, pPointsBegin, pPointsEnd - pPointsBegin);
	//DebugComSend("pDest1 = %s \n", pDest); 
	/*
	memcpy(pDest+(pPointsEnd-pPointsBegin), '.', 1);
	DebugComSend("pDest2 = %s \n", pDest); 
	strcpy(pDest+(pPointsEnd-pPointsBegin)+1, pPointsEnd);
	*/
	sprintf(pDest, "%s%c%s", cTmp, '.', pPointsEnd);
}
//add end
void FormatAmt(char *pDest, char *pBuf, char *CurDes, BYTE InputType)  
{
	BYTE cnt;
	BYTE CurDesLen = 3;
	BYTE CurShort = 0;
	char AsciiInBuf[LG_AMOUNT+2];
	char AsciiOutBuf[2*LG_AMOUNT];
	BYTE i;


	if (InputType == BCD_IN)
    {
        memset(AsciiInBuf, 0, sizeof(AsciiInBuf));
		BfAscii(AsciiInBuf, castptr(BYTE *, pBuf), 6);
		pBuf = AsciiInBuf;
	}
    if (InputType==ASCII_IN)
    {
        sprintf(AsciiInBuf, "%.12s", pBuf);
        i = strlen(AsciiInBuf);
		//DebugComSend("AsciiInBuf = %s\n", AsciiInBuf);
		//DebugComSend("AsciiInBuf len = %d\n", i);
        if (i<LG_AMOUNT)
        {
            memmove(AsciiInBuf+12-i, AsciiInBuf, i+1);
            memset(AsciiInBuf, '0', 12-i);
        }
		pBuf = AsciiInBuf;
    }

    memset(AsciiOutBuf, 0, sizeof(AsciiOutBuf));
    sprintf(AsciiOutBuf, "000,000,000.000");
    memcpy(AsciiOutBuf,    pBuf,    3);
    memcpy(AsciiOutBuf+4,  pBuf+3,  3);
    memcpy(AsciiOutBuf+8,  pBuf+6,  3);
    memcpy(AsciiOutBuf+12, pBuf+9,  3);
	/*
    memcpy(AsciiOutBuf,    pBuf,    1);
    memcpy(AsciiOutBuf+2,  pBuf+1,  3);
    memcpy(AsciiOutBuf+6,  pBuf+4,  3);

   // memcpy(AsciiOutBuf+12, pBuf+9,  3);
*/
    // trim heading '0' and ','
    i = 0;
	while ((AsciiOutBuf[i] == '0') || (AsciiOutBuf[i] == ','))
    {
		if (AsciiOutBuf[i+1]=='.')
        {
			break;
        }
		AsciiOutBuf[i] = 0x20;
		i++;
	}

	PubTrimHeadChars(AsciiOutBuf, ' ');

#if 0
	if (CurDes[0] == ' ')
    {
		memcpy(&AsciiOutBuf[16], &CurDes[1], 2);
		pDest[0] = ' ';
		memcpy(&pDest[1], AsciiOutBuf, 18);
	}
	else if (CurDes[2] == ' ')
    {
		memcpy(&AsciiOutBuf[16], CurDes, 2);
		pDest[0] = ' ';
		memcpy(&pDest[1], AsciiOutBuf, 18);
	}
	else
    {
		memcpy(&AsciiOutBuf[16], CurDes, 3);
		memcpy(pDest, AsciiOutBuf, 19);
    }
#else
#ifndef ALLEN_TEST
	if (CurDes[0] == ' ')
	{
		pDest[0] = ' ';
		memcpy(&pDest[1], &CurDes[1], 2);
		pDest[3] = ' ';
		memcpy(&pDest[4], AsciiOutBuf, 16);
	}
	else if (CurDes[2] == ' ')
    {
		pDest[0] = ' ';
		memcpy(&pDest[1], CurDes, 2);
		pDest[3] = ' ';
		memcpy(&pDest[4], AsciiOutBuf, 16);
	}
	else
    {
		memcpy(pDest, CurDes, 3);
		pDest[3] = 0x20;
		memcpy(pDest+4, AsciiOutBuf, 16);
    }
#else
	sprintf(pDest, "%s%s", "BHD ", AsciiOutBuf);
#endif
#endif
}
  
void DumpBatteryStatus(CpuhwBatSysType batStatus)
{
    // Gary deleted unreferenced function
}
  
void DumpSignalLevel(BYTE byLevel)
{
    // Gary deleted unreferenced function
}

#ifdef APPL_ARABIC      
WORD SelectLanguage(void)
{
	char    line[LG_CSTMSG];
	char    Araline[17]= _T_NOOP("Arabic -1"); //Mandy need translation of Arabic
	BYTE    key;

	OS_ClearScreen();
    ScrPrint(0, 0, CFONT, "SELECT PRINTOUT");
    ScrPrint(0, 2, CFONT, "LANGUAGE:");

	ArShowString(4, SHIFT_LEFT, Araline);                                                                                                                                                                         
	ScrPrint(0, 6, CFONT, "2- English");
	
    while (1)
    {
        key = GetOneKey(TO_1MIN);
        if ((key==KEY1) || (key==KEYATM3))
        {
			gLanguage = ARABIC;
            OS_ClearScreen();
            return STAT_OK;
        }
        if ((key==KEY2) || (key==KEYATM4))
        {
            OS_ClearScreen();
            gLanguage = ENGLISH;
            return STAT_OK;
        }
        if (key==KEYCANCEL || key==NOKEY)
        {
            OS_ClearScreen();
            return STAT_NOK;
        }
        PubBeepErr();
    }
}	
#endif      

BYTE Disp_Long_Msg(BYTE Type)
{
    /*
	char line[17];
	char  msg1[25][17]={
	{114,116,1,138,32,1,120,66,130,66,92},
	{58,35,104,1,85,23,112,140 },
	{118,130,60,105,27,22,32,118,109,116,1,131,104,32,119,4},
	{132,100,32,20,60,23,113,112,32,35,104,1,86,23,112,140},
	{114,130,128,117,27,112,140,32,25,140,20,140,64,37,4},	
	{35,42,22,140,64,117,112,1,22},
	{19,113,92,32,118,109,27,105,100,140,128,116,127},
	{128,124,32,132,112,1,27,112,140,20,140,64,37,10,140},
	{19,113,92,32,118,109,121,116,32,35,105,100,140,128,116}, // scr2
	{35,104,7,92,32,129,2,32,135,69,100},
	{119,128,109,26,32,60,104,32,35,130,60,104,1,93,26},
	{127,32,118,109,121,131,22,32,35,117,133,1,104},
	{119,3,73,22,32,64,37,1,27,112,140,32,122,131,22},
	{114,42,116,32,35,92,1,81,23,112,140},
	{140,61,4,127,32,47,114,130,128,117,27,112,140},
	{118,27,93,100,59,32,60,104,32,118,27,121,108},
	{122,117,31,112,140,32,122,116,32,140,20,66,37},	//scr3
	{119,128,93,100,60,27,68,32,127,2,64,37,1,27,113,112},
	{126,112,32,122,117,31,112,140,122,116,140,20,66,37}, 
	{118,27,105,100,140,127,32,60,105,100},
	{32,110,113,26,32,135,69,100,32,19,113,92},
	{35,77,138,32,119,3,73,22,32,35,104,7,93,112,140},
	{35,92,1,81,23,112,140,122,116,35,93,133,1,72},	
	{35,23,69,121,112,140,32,129,127,1,69,26,83,105,100},
	{122,117,31,112,140,32,122,116,32,35,131,104,1,23,112,140}};
	
	char  msg2[24][17]={
	{20,140,64,73,112,1,22,32,21,1,38,130,10,140},	//scr1
	{19,112,4,32,99,64,77,117,112,140,122,116,41},	
	{40,64,37,1,27,112,140},
	{99,64,77,116,32,106,100,140,127,32,126,23,37,128,117,22},
	{132,116,11,68,10,140,32,132,23,88,128,22,2},
	{ 35,92,1,81,23,112,140,20,140,64,72,32,19,113,92},
	{1,125,113,131,76,1,101,26,32,34,59,63,140,128,112,140},
	{118,109,121,116,32,123,1,120,59,2},
	{35,92,1,81,23,112,32,91,128,120},//scr2
	{0x00},//prodtype
	{35,92,1,81,23,112,140,32,102,76,127},
	{0x00},//prod desc
	{102,130,64,93,27,112,140,86,130,64,72,32,118,104,63},
	{0x00},//barcode
	{132,112,1,117,37,10,140,32,122,117,31,112,140},
	{0x00},//price
	{122,116,41,94,131,23,112,140,32,111,128,23,104},	//scr3
	{19,112,4,32,64,37,1,27,112,140},
	{40,99,64,77,117,112,140},
	{94,131,23,112,140,32,1,121,113,23,104,139,49}, //opt1
	{106,100,127,32,99,64,77,117,113,112},
	{ 1,105,22,1,68,32,63,128,108,62,117,112,140},
	{94,131,23,112,140,32,114,23,105,120,32,6,139,50},
	{99,64,77,117,113,112}};

	char  msg6[18][17]={
	{114,22,1,104,32,24,113,85,112,140,140,62,124,139},		//scr4
	{99,64,84,32,122,116,20,1,97,112,11,112},  
	{132,23,88,128,22,2,32,99,64,77,116},
	{115,1,130,2,32,111,7,45,32,132,116,7,68,10,140},
	{126,46,130,63,1,26,32,122,116},
	{34,140,64,27,73,117,112,140,32,35,92,1,81,23,113,112},
	{114,131,108,127,32,111,128,46,130,32,127},
	{118,113,69,27,130,32,119,2,32,99,64,77,117,112,140},		
	{34,140,64,27,73,117,112,140,32,35,92,1,81,23,112,140},//scr5
	{122,92,32,35,22,1,131,121,112,1,22,41}, 
	{1,117,113,69,26,32,99,64,77,117,112,140},
	{99,64,77,27,112,140,32,127,32,1,131,105,131,105,138},
	{94,120,1,116,32,129,2,32,119,127,59,32,1,125,22},
	{64,37,1,27,112,140,32,99,64,84,122,116},
	{35,131,113,117,92,32,28,117,26,32,20,140,128,68},
	{114,116,1,42,112,32,94,131,23,112,140},				
	{118,112,32,115,2,32,35,104,1,85,23,112,140},	//scr6
	{118,27,26}};
	
	char  msg3[13][17]={
	{99,64,77,117,112,32,114,131,108,128,108},
	{32,127,32,132,116,7,68,10,140,32,114,131,108,128,22,2},
	{35,131,77,46,73,112,140,132,27,101,77,22},
	{58,132,26,12,1,22,32,28,117,104,32,132,121,120,3,22},
	{32},
	{35,92,1,81,23,112,140,32,122,131,131,93,27,22,139,49},
	{1,125,26,140,62,22,32,34,140,64,27,73,117,112,140},
	{32},
	{127,32,35,92,1,81,23,112,140,116,139,50}, 
	{1,125,27,116,7,68,32,122,116,32,60,108,3,27,112,140},
	{35,92,1,81,23,112,140,32,118,113,69,26,139,51},
	{127,2,1,131,105,131,105,138,32,1,117,113,69,26},
	{1,131,117,109,138}};
	
	char  msg4[10][17]={
	{94,131,23,112,1,22,32,21,1,38,130,10,140},	
	{122,116,32,35,42,22,140,64,117,112,1,22},
	{114,116,1,42,112,32,99,64,77,117,112,140}, 
	{58,35,104,1,85,23,112,140},
	{99,64,77,116,32,106,100,140,127,32,126,23,37,128,117,22},
	{132,116,7,68,10,140,32,132,23,88,128,22,2},
	{35,92,1,81,23,112,140,32,94,131,22,32,19,113,92},
	{1,125,113,131,76,1,101,26,32,34,59,63,140,128,112,140},
	{114,116,1,42,112,32,132,113,130,32,1,117,131,100},
	{35,104,1,85,23,112,140}};
	
	char  msg5[11][17]={
	{99,64,77,117,113,112,32,114,131,108,128,108},
	{35,131,77,46,73,112,140,32,132,27,101,77,22,127},
	{28,117,104,32,132,121,120,2,32,60,108,16,2},
	{35,92,1,81,23,112,140,32,118,131,113,69,27,22},
	{35,104,1,85,23,112,140,32,114,116,1,42,112},
	{118,131,113,69,27,112,140,32,118,26,139,49}, 
	{1,131,105,131,105,138,32,1,117,113,69,26},
	{118,131,113,69,27,112,140,32,118,26,139,50},	
	{1,131,117,109,138,32,1,117,113,69,26},
	{118,131,113,69,27,112,140,118,27,130,32,118,112,139,51},
	{1,104,7,84,4}};
		
	BYTE i,LineNum,StrLen,maxval,OldLine;
	char key=0,ret,dummy[17];
	BYTE buf[30],TmpStr[30];
	LineNum=1;

	set_font("myfont.fon");

	OS_ClearScreen();	
	switch(Type)
    {
		case 1: 
			maxval=25;
			break;
		case 2:
			maxval=24;
			sprintf(msg2[9],"%.16s",gPTStruct.product[0].prodinfo);
			sprintf(msg2[11],"%.16s",gPTStruct.product[0].proddesc);
			sprintf(msg2[13],"%.16s",gPTStruct.product[0].barcode);
	
			memcpy(buf,gPTStruct.TotalAmount,12);buf[12]=0;
			FormatAmt(TmpStr, buf, gCurrencySymbol, ASCII_IN);	
			sprintf(msg2[15],"%.16s",TmpStr);			
			break;
		case 3: 
			maxval=13;
			break;	
		case 4: 
			maxval=10;
			break;					
		case 5: 
			maxval=11;
			break;	
		case 6:
			maxval=18;
			break;	
	}	
OS_LogTo(DEBUG_PORT,"step_lm %d",Type); 		
	i=0;
	while(key!=KEYCANCEL)
    {
		switch(Type)
        {
			case 1: 
				sprintf(dummy,"%s",msg1[i]);
				break;
			case 2:
				sprintf(dummy,"%s",msg2[i]);
				break;
			case 3:
				sprintf(dummy,"%s",msg3[i]);
				break;
			case 4:
				sprintf(dummy,"%s",msg4[i]);
				break;
			case 5:
				sprintf(dummy,"%s",msg5[i]);
				break;
			case 6:
				sprintf(dummy,"%s",msg6[i]);
				break;
		}	

		StrLen=strlen(dummy);
		memset(line,0x20,16);
		memcpy(&line[16 - StrLen ], dummy, StrLen);	
		write_at(  line,16,1,LineNum);
		LineNum++;
		if (i<maxval)
			i++;
				
		if (LineNum>8 || (i==maxval))
        {
			OldLine=LineNum;
			while(TRUE)
            {
				key=GetOneKey(3);
				switch(key) 
				{
					case KEYUP:
						if (i==maxval)
                        {
							i-=LineNum;
							i-=7;	
						}
						else
                        {	
							if (i>15) i-=16;
							else i-=8;
						}	
						break;
					case KEY1:
						if (Type==2)
                        {
							OS_ClearScreen();
							set_font("Nadir1.fon");
							return(1);	
						}	
						break;
					case KEY2:
						if (Type==2)
                        {
							OS_ClearScreen();
							set_font("Nadir1.fon");
							return(0);		
						}	
						break;
					case KEYENTER:
						if (i==maxval && Type!=2)
                        {
							OS_ClearScreen();
							set_font("Nadir1.fon");
							return(0);
						}
						break;	
					default:
						break;
				}	
				if (key) break;
			}	
			LineNum=1;	
			OS_ClearScreen();	
		}

		if (i==maxval)
        {
			i-=(OldLine-1);			
		}
	}	

	set_font("Nadir1.fon");

#endif
    */
	OS_ClearScreen();	
	return(0);
}	


BYTE Get_MPIN(void)
{
	char mPIN[5];

	if (!Enter_data(mPIN,8))
		return(0);
	if (memcmp(mPIN,"1234",4))
		return(0);

	return(1);		
}	

BYTE Get_CPIN(void)
{
	char mPIN[5],i=0;

	while (i<3)
    {
		if (Enter_data(mPIN,9))
        {
			if (memcmp(mPIN,"1234",4))
				i++;
			else	
				return(1);
		}
	}	
	return(0);		
}	

BYTE AskYesNo2(MSG_Struct *stMSG,BYTE Type,BYTE offset) 
{ 
	BYTE key,StrLen,linemsg[50],SMALL_FONT_MAX;
	SMALL_FONT_MAX = 21;

	memset(linemsg, 0, sizeof(linemsg));	
	OS_ClearScreen(); 
		
	if (stMSG->CSTMSG1[0])	// If Line1 exists
    {
		ScrPrint(0, 0, ASCII, "%.21s", stMSG->CSTMSG1);
	}

	if (stMSG->CSTMSG2[0])		// If Line2 exists
    {
		ScrPrint(0, 1, ASCII, "%.21s", stMSG->CSTMSG2);	
	}
	
	if (stMSG->CSTMSG3[0])	// If Line3 exists
    {
		ScrPrint(0, 2, ASCII, "%.21s", stMSG->CSTMSG3);	
	}
	
	if (stMSG->CSTMSG4[0])	// If Line4 exists
    {
		ScrPrint(0, 3, ASCII, "%.21s", stMSG->CSTMSG4);
	}

	if (stMSG->CSTMSG5[0])	// If Line4 exists
    {
//		PubWaitKey(1000);
		ScrPrint(0, 4, ASCII, "%.21s", stMSG->CSTMSG5);
	}

	if (stMSG->CSTMSG6[0])	// If Line4 exists
    {
		ScrPrint(0, 5, ASCII, "%.21s", stMSG->CSTMSG6);
	}
	
	if (!Type) 
	{
		return (1);
	}

	switch (Type)
    {
	case 1:
		ScrPrint(0, 6, ASCII, "1-YES");
		ScrPrint(0, 7, ASCII, "2-NO");
		break;

	case 2:
        ScrPrint(0, 6, ASCII, "1-ACCEPT");
        ScrPrint(0, 7, ASCII, "2-REJECT");
		break;
	}

	while(TRUE)
    {
		key = GetOneKey(TO_1MIN);	
		switch (key)
        {
			case '1':
				return (1);
			case '2':	
				return (2);
			case KEYCANCEL:
			case NO_KEY:
				return (0);	
		}
	}
}


void PrintSaleContract_Eng(PTStruct *pTran)
{
}


BYTE GetOneKeyOne(void)
{
	BYTE key;

	key = 0;
  while(1)
    {
		key = GetOneKey(60);	
		switch (key)
        {
		   case KEYENTER :
				return (1);
			case KEYCANCEL:
			case NOKEY:
				return (0);	
		}
	}
  return 0;
}

void ComOutput( unsigned char *buf,int len,char *string, ...)
{
	int i;
	BYTE printBuf[3000];
	va_list varg;
	char varBuf[100];
	

	if(NULL == string || buf == NULL)
	{
		ScrCls();
		ScrPrint(0,0,1,"ComOutput para error");
		getkey();
		ScrCls();
	}
	memset(printBuf,0,sizeof(printBuf));
	memset(varBuf,0,sizeof(varBuf));
	for(i = 0;i < len;i++)
	{
		sprintf(printBuf + i*3,"%02X ",buf[i]);
	}
	PortOpen(0,"115200,8,N,1");
	va_start(varg,string);
	vsprintf(varBuf,string,varg);
	for(i = 0;i < strlen(varBuf);i++)
	{
		PortSend(0,varBuf[i]);
	}
	PortSend(0,':');
	for(i = 0;i < strlen(printBuf);i++)
	{
		PortSend(0,printBuf[i]);
	}
	PortSend(0,'\r');
	PortSend(0,'\n');
	PortClose(0);
}