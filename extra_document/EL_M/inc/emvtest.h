/*------------------------------------------------------------
* FileName: EmvTest.h
* Author:	ZhangYuan
* Date:		2016-08-10
------------------------------------------------------------*/

#ifndef EMV_TEST_H_
#define EMV_TEST_H_

#define EMV_PARAM_TEST_ 1

#ifdef EMV_PARAM_TEST_


#define DEBUG_			1
#define DEBUG_SERIAL	1
#define PORT_USB		11
#define PORT_BLUETOOTH	7


typedef struct EMVParamInfo {
	unsigned char 	ucFlag;	// 0-not set, 1-set

	unsigned short 	usAidNum;
	unsigned short 	usAidCurBlkIdx;
	
	unsigned short 	usCapkNum;
	unsigned short 	usCapkCurBlkIdx;
	unsigned short 	usCapkCurBlkPos;

	unsigned short	usRevokeCapkNum;
	unsigned short	usRevokeCapkBlkIdx;
	unsigned short	usRevokeCapkBlkPos;
	
	unsigned short	usParamBlkIdx;

	unsigned short	usExtmParamBlkIdx;
	unsigned short	usMckParamBlkIdx;
} EMVParamInfo;

typedef struct EMVAPPINFO {
	unsigned char 	aucAid[17];
	unsigned short 	usAidBlkIdx;
} EMVAppInfo;

typedef struct EMVCAPKINFO {
	unsigned char 	aucRid[5];
	unsigned char 	ucKeyId;
	unsigned short 	usCapkBlkIdx;
	unsigned short 	usCapkBlkPos;
} EMVCapkInfo;

void WriteEMVPARA(void);	

int EMVSetPARAM(void);

int EMVLoadAppFromFile();
int EMVLoadSpecificApp(unsigned char *pucAid, int iAidLen, EMV_APPLIST *pstEmvApp);
int EMVLoadSpecificCapk(unsigned char *pucRid, unsigned char ucKeyIndex, EMV_CAPK *pstEmvCapk);
int EMVLoadSpecificRevoke(unsigned char *pucRid, unsigned char ucKeyIndex, EMV_REVOCLIST *ptEmvRevokNode);
void EMVLoadCardSchemeConfig(unsigned char *pucRid);

void AppLog(char *formatString, ...);
void AppStrLog(char *formatString, ...);
void AppHexLog(unsigned char *pucMsg, unsigned char *pucData, int iDataLen);

#endif	// endif EMV_PARAM_TEST_ 

#endif
