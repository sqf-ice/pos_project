

#ifndef _APPLIB_H
#define _APPLIB_H
#include <posapi.h>



#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#ifndef uchar
	typedef unsigned char	uchar;
#endif

#ifndef ushort
	typedef unsigned short	ushort;
#endif

#ifndef uint
	typedef unsigned int	uint;
#endif

#ifndef ulong
	typedef unsigned long	ulong;
#endif


// macros for screen functions
#define NUM_MAXZHCHARS      16          // ÿ������ַ���Ŀ(������)
#define NUM_MAXCOLS         128         // ÿ���������
#define NUM_MAXCOLNO        (NUM_MAXCOLS-1)     // ����к�
#define NUM_MAXLINES        6           // �������
#define NUM_MAXLINENO       (NUM_MAXLINES-1)    // ����к�
#define NUM_MAXZHLINES      3           // �����������
#define NUM_MAXCHARS		21

#define DISP_LINE_LEFT		0x80
#define DISP_LINE_RIGHT		0x40
#define DISP_LINE_CENTER	0x20
#define DISP_SCR_CENTER		0x0100
#define DISP_LINE_REVER		0x0200
#define DISP_PART_REVER		0x0400

// menu display mode
#define MENU_NOSPEC		0x00	// �Զ���ʾ
#define MENU_USESNO		0x01	// ʹ�ò˵��Լ��ı��(������MENU_AUTOSNO)
#define MENU_AUTOSNO	0x02	// �Զ����
#define MENU_CENTER		0x10	// �м������ʾ
#define MENU_PROMPT		0x20	// ǿ����ʾ��Ϣ�ڵײ�
#define MENU_ASCII		0x40	// ǿ����ʾС����
#define MENU_CFONT		0x80	// ǿ����ʾ������(������MENU_ASCII)
#define TIMER_TEMPORARY			4       // ��ʱ��Ķ�ʱ��(����ģ����Թ���)
#define TIMER_TMP				3		// added by Idina 2015-08-04


#define     APPLIB_SHIFT_RIGHT             0x00
#define     APPLIB_SHIFT_CENTER            0x01
#define     APPLIB_SHIFT_LEFT              0x02
#define     APPLIB_EFT_REVERSE             0x80


// macros for debug output
#define DEVICE_SCR   0x01
#define DEVICE_PRN   0x02
#define DEVICE_COM1  0x04
#define DEVICE_COM2  0x08

#define ASC_MODE	0x10
#define HEX_MODE	0x20
#define ISO_MODE	0x40
#define TLV_MODE	0x80


// macros for vDes()
#define ONE_DECRYPT     0           // ��DES����
#define ONE_ENCRYPT     1           // ��DES����
#define TRI_DECRYPT     2           // ��DES����(16 bytes key)
#define TRI_ENCRYPT     3           // ��DES����(16 bytes key)
#define TRI_DECRYPT3    4           // ��DES����(24 bytes key)
#define TRI_ENCRYPT3    5           // ��DES����(24 bytes key)

// macros for vCalcMac()
#define MAC_ANSIX99     0           // ANSI9.9 standard MAC algorithm
#define MAC_FAST        1           // HyperCom fast MAC algorithm

#define DISP_LINE_LEFT		0x80
#define DISP_LINE_RIGHT		0x40
#define DISP_LINE_CENTER	0x20
#define DISP_SCR_CENTER		0x0100
#define DISP_LINE_REVER		0x0200
#define DISP_PART_REVER		0x0400



typedef void (*MenuFunc)(void);
typedef struct _tagMenuItem{
	uchar		bVisible;						// TRUE: show it, FALSE: hide
    uchar       szMenuName[NUM_MAXCHARS+1];		// menu name
    MenuFunc    pfMenuFunc;						// callback function(if set)
}MenuItem;

//���ܲ˵���ض���

//ÿ�����ɵ��ַ�������Ӣ���ַ����㣬����ռ2��λ�ã�
#define SM_LINE_WIDTH  16
//���˵�����
#define SM_MAX_LINE    32

/*
 * �����˵���
 */
struct SMART_MENU;

typedef struct MENU_ITEM{
    char                pszText[NUM_MAXZHCHARS];//*pszText;  /*�˵�������ʾ*/
    int                 iValue;      /*��Ӧ�Ľ��׺�*/    
    struct SMART_MENU * pstSubMenu;   /*�Ӳ˵������ޣ���ΪNULL*/
}MENU_ITEM;


typedef struct SMART_MENU{
    MENU_ITEM       *pstContent;
    int             iNum;        /*�˵����� */
    uchar			sMask[SM_MAX_LINE];
}SMART_MENU;



#define  MENU_ICON_UP       7   /*up ����*/
#define  MENU_ICON_DOWN     8   /*down ����*/
    /*action - ָ������,    1:������0:Ϩ��*/
#define  MENU_CLOSEICON     0   /*�ر�ͼ��[�������ͼ��]*/
#define  MENU_OPENICON      1   /* ��ʾͼ��[��Դ�ӡ����IC�������������������ϡ�����]*/

//ÿ����ʾһ���˵��ȱʡ��ʾ������
#define SM_1ITEM_PER_LINE 0x00000001

//ʹ�����¼�ͷ��ȷ�ϼ�ѡ��ȱʡ�����ּ�ѡ��
#define SM_ARROW_SEL      (0x00000001<<1)

//��0��ʼ����˵��SM_ARROW_SEL��λʱ��Ч��ȱʡ��1��ʼ��
#define SM_ZERO_START     (0x00000001<<2)

//������ʾ����
#define SM_MIDDLE_TITLE   (0x00000001<<3)

//������ʾ�˵���
#define SM_MIDDLE_ITEM    (0x00000001<<4)

//��ȡ����ֱ�ӷ���
#define SM_DIRECT_RETURN  (0x00000001<<5)

#define SM_ON   1
#define SM_OFF  0


// Support right-to-left language
// This is the lib's default right-to-left display function
// But for language like Farsi, should use external function:
//     customize the function in your application like this:
//     
//
typedef uchar (*FNPTR_ShowStringRightToLeft)(uchar lineno, uchar Mode, uchar *str);

typedef struct _tagAPPLIB_CONFIG 
{
    uchar   bStrFromRightToLeft;
    FNPTR_ShowStringRightToLeft pFnShowLeftRightString;
}APPLIB_CONFIG;

uchar LibDefault_DispLeftRightString(uchar lineno, uchar Mode, uchar *str);


// AppLib's default config:
//     display direction is from left to right
//     right-to-left display function is set to lib's default : LibDefault_DispLeftRightString() )
static APPLIB_CONFIG sg_stAppLibConfig = {0, LibDefault_DispLeftRightString};


#define DISP_MAX_LEN 21				// added by idina 201506-18 




#define MAX(a, b)       ( (a)>=(b) ? (a) : (b) )
#define MIN(a, b)       ( (a)<=(b) ? (a) : (b) )

// ========= Added by Idina 2015-09-14 ============== //
#define MPOS_INPUT_ALLOW_ZERO_IN		0x80
#define MPOS_INPUT_BIG_IN				0x40
#define MPOS_INPUT_AMOUNT_IN			0x20
#define MPOS_INPUT_NUM_IN				0x01    // ��������
#define MPOS_INPUT_ALPHA_IN				0x02    // ��ĸ���ֻ������
#define MPOS_INPUT_PASS_IN				0x04    // ��������
#define MPOS_INPUT_CARRY_IN				0x08    // �д�������
#define MPOS_INPUT_ECHO_IN				0x10    // ���Դ�������,���������ԭ����
#define MPOS_INPUT_LEN_GETSTRING		512     // for PubGetString() use

// ========= Added end ==========================  //

ulong PubAsc2Long(uchar *psString, uint uiLength, ulong *pulOut);
void PubLong2Char(ulong ulSource, uint uiTCnt, uchar *psTarget);
ulong PubChar2Long(uchar *psSource, uint uiSCnt, ulong *pulTarget);
void PubLong2Bcd(ulong ulSource, uint uiTCnt, uchar *psTarget);
ulong PubBcd2Long(uchar *psSource, uint uiSCnt, ulong *pulTarget);


void PubBeepErr(void);
void setCustomizedFont(int fontType);

void PubDispString(void *pszStr, ushort nPosition);
uchar PubGetString(uchar ucMode, uchar ucMinlen, uchar ucMaxlen, uchar *pszOut, uchar ucTimeOut); // added by Idina 2015-09-14

void PubShowTitle(uchar bClrScreen, uchar *pszTitle);
uchar PubKeyPressed(void);
uchar PubWaitKey(short iWaitTime);
int PubGetMenu(uchar *pszPrompt, MenuItem *pstMenu, uchar ucMode, ushort nTimeOut);
void PubBcd2Asc0(uchar *psIn, uint uiLength, uchar *pszOut);
void PubTrimTailChars(uchar *pszString, uchar ucRemoveChar);
void PubBcd2Asc(uchar *psIn, uint uiLength, uchar *psOut);

int GetPanFromTrack2(char *cardno ,uchar *pszPAN);
int PubExtractPAN(uchar *pszPAN, uchar *pszOutPan);
void PubAsc2Bcd(uchar *psIn, uint uiLength, uchar *psOut);
void PubBcd2Asc0(uchar *psIn, uint uiLength, uchar *pszOut);
 void PubTrimTailChars(uchar *pszString, uchar ucRemoveChar);


int SelectMenu(uchar *pszPrompt, int iItemNum, MenuItem *pstMenu, unsigned short uiTimeOut); // added by idina 2015-06-18

/********************************************************************************
����: ���ܲ˵�
�������: pstSmartMenu   - ���ܲ˵�ָ��
          uiMode - �˵���ʾģʽ ����Ϊ���²�������ϣ���Ϊ0��ȡ����ȱʡ��ʽ
                 SM_1ITEM_PER_LINE ÿ����ʾһ���˵���
                ��ȱʡ��ʾ����,��SM_ARROW_SELģʽ��ǿ��ÿ��һ����
                 SM_ARROW_SEL  ʹ�����¼�ͷ��ȷ�ϼ�ѡ��ȱʡ�����ּ�ѡ��
                 SM_ZERO_START  ��0��ʼ����˵���(SM_ARROW_SELģʽ����Ч)
                ��ȱʡ��1��ʼ��
                 SM_MIDDLE_TITLE   ������ʾ���� (ȱʡ����)
                 SM_MIDDLE_ITEM  ������ʾ�˵���(ȱʡ����)
        iTimeout - ��ʱʱ�䣨����Ϊ��λ������Ϊ���ʾ�޳�ʱ 
�������: ��
����:    >0   ѡ���Ľ��׺�
         -1   ȡ����������ʱ��
 ********************************************************************************/
int PubSmartMenuEx(SMART_MENU *pstSmartMenu, uint uiMode, int iTimeout);


/********************************************************************************
 * ������: PubSmInit
 * ��  ��: ��ֻ�ܲ˵����ƺ�����
 * ��  ��: pstMenuItem	- ��ʼ�˵�����
 * ��  ��: pstSmartMenu	- ���ܲ˵�ָ��
 * ����ֵ: ��
 ********************************************************************************/
void PubSmInit(SMART_MENU *pstSmartMenu, MENU_ITEM *pstMenuItem);


/************************************************************************
����: ���˵����ƿ��ز˵���Ŀ
�������: pszName		- �˵�������
          iFlag			- ���� SM_ON / SM_OFF
�������: pstSmartMenu	- �˵�ָ��
����:��
************************************************************************/
void PubSmMask(SMART_MENU *pstSmartMenu, const char *pszName, int iFlag);



#ifdef _POS_DEBUG

/****************************************************************************
���ܣ� 	��ʾ�ļ���������
���������	pszFileName:       ��ʾ���ļ���
                ulLineNo��         ��ʾ������
���������	��
���أ�  	��
****************************************************************************/
void DispAssert(uchar *pszFileName, ulong ulLineNo);

    // debug macro for boolean expression
	#define _POS_DEBUG_WAIT		15
    #define PubASSERT(expr)                                           \
                if( !(expr) ){                                      \
                    DispAssert((uchar *)__FILE__, (ulong)(__LINE__));       \
                }

    // print string debug information
    #define PubTRACE0(sz)                                             \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, "%s", sz);                    \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE1(sz, p1)                                         \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, sz, p1);                      \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE2(sz, p1, p2)                                     \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, sz, p1, p2);                  \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE3(sz, p1, p2, p3)                                 \
            {                                                       \
                ScrCls();                                           \
                ScrPrint(0, 0, CFONT, sz, p1, p2, p3);              \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACEHEX(t, s, l)    DispHexMsg((t), (s), (l), _POS_DEBUG_WAIT)

#else /* _POS_DEBUG */

    #define PubASSERT(expr)
    #define PubTRACE0(sz)
    #define PubTRACE1(sz, p1)
    #define PubTRACE2(sz, p1, p2)
    #define PubTRACE3(sz, p1, p2, p3)
    #define PubTRACEHEX(t, s, l)

#endif /* _POS_DEBUG */


/****************************************************************************
 Function:      �๦��DES����/��������
 Param In:
    ucMode      DES����/����ģʽ:
                ONE_ENCRYPT --> ��DES����
                ONE_DECRYPT --> ��DES����
                TRI_ENCRYPT --> ��DES����
                TRI_DECRYPT --> ��DES����
    psData:     ������/���ܵ�����(8�ֽ�)
    psKey:      DES�������Կ(�������Ȼ�����������,����ucModeȷ��)
 Param Out:
    psResult    DES����Ľ��(8�ֽ�)
 Return Code:   none
****************************************************************************/
void PubDes(uchar ucMode, uchar *psData, uchar *psKey, uchar *psResult);	//

/****************************************************************************
 Function:      ����MAC���㺯��
 Param In:
    ucMode      ����MAC����ģʽ:
                MAC_FAST    --> ����MAC����(HyperCom���ٷ�ʽ)
                MAC_ANSIX99 --> ANSI x9.9 ��ʽ
    psKey:      MAC�������Կ(8�ֽ�)
    psMsg:      ����MAC������
    uiLength:   ����MAC�����ݵĳ���
 Param Out:
    psMAC       ���ĵ�MAC(8�ֽ�)
 Return Code:   none
****************************************************************************/
void PubCalcMac(uchar ucMode, uchar *psKey, uchar *psMsg, uint uiLen, uchar *psMAC);
	
/****************************************************************************
���ܣ� 	        ����LRC
�������	psData    ��Ҫ���������
                uiLength  ���ݵĳ���
�������	*pucInit  LRCֵ
���أ�  	��
****************************************************************************/
void PubCalcLRC(uchar *psData, ushort uiLength, uchar *pucInit);

uchar PubYesNo(ushort nWaitTime);

void PubShowMsg(uchar ucLine, uchar *pszMsg);

#endif
