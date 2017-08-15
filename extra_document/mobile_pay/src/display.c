/*------------------------------------------------------------
* FileName: display.c
* Author: liukai
* Date: 2016-03-24
------------------------------------------------------------*/

#include "global.h"

XuiImg *g_ptMpLogo=NULL;
XuiFont *g_ptXuiFont=NULL;
XuiFont *g_ptStBarFont=NULL;
XuiWindow *g_ptGifLoader=NULL;

static int sg_iClockX=0;
static int sg_iClockY=0;
static int sg_iClockWidth[3]={0};
static int sg_iClockHeight=0;
static ST_TIME sg_tOldTime={0};
static int sg_iGprsOpened=0;
static int sg_iWifiOpened=0;

static int GuiInit(int iSBHeight)
{
	char szValue[32]={0};
	char szRotateStr[32]={0};
	char szSBStr[32]={0};
	int iRet=0;
	char *xui_argv[10];
	int  xui_argc=0;

	iRet = OsRegGetValue("ro.fac.lcd.rotate", szValue);
	if (iRet > 0) {
		snprintf(szRotateStr, sizeof(szRotateStr), "ROTATE=%s", szValue);
	}
	else {
		strcpy(szRotateStr, "ROTATE=0");
	}

	if (iSBHeight > 0) {
		snprintf(szSBStr, sizeof(szSBStr), "STATUSBAR=%d", iSBHeight);
	}
	else {
		strcpy(szSBStr, "STATUSBAR=0");
	}

	xui_argv[0] = szRotateStr;
	xui_argv[1] = szSBStr;
	xui_argv[2] = NULL;
	xui_argc = 2;

	iRet = XuiOpen(xui_argc, xui_argv);
	if (iRet == XUI_RET_OK) {
		return MP_OK;
	}
	else {
		return MP_ERR;
	}
}

static void GuiDeinit(void)
{
	XuiClose();
}

static void UpdateStatusbar(void)
{
	static int s_icount = 0;
	char szCurESSID[64]={0};
	int iCurRSSI=0;
	int iRet=0;
	int iBatteryLevel=0;
	ST_TIME tNewTime;

	XuiColor colorClock;
	char buf[16]={0};
	char *batteryc[] = {
			"./res/battery0c.png",
			"./res/battery25c.png",
			"./res/battery50c.png",
			"./res/battery75c.png",
			"./res/battery100c.png"
	};
	char *batterys[] = {
			"./res/battery0.png",
			"./res/battery25.png",
			"./res/battery50.png",
			"./res/battery75.png",
			"./res/battery100.png",
			"./res/nobattery.png"
	};
	char *wifisignals[] = {
			"./res/wifi0.png",
			"./res/wifi50.png",
			"./res/wifi100.png"
	};
	char *netsignals[] = {
			"./res/empty.png",
			"./res/mobile1.png",
			"./res/mobile2.png",
			"./res/mobile3.png",
			"./res/mobile4.png",
			"./res/mobile5.png"
	};

	colorClock.r = 0xb8;
	colorClock.g = 0xb8;
	colorClock.b = 0xb8;
	colorClock.a = 0xff;

	OsGetTime(&tNewTime);
	if (memcmp(&sg_tOldTime, &tNewTime, sizeof(sg_tOldTime)) != 0) {
		if ( (sg_tOldTime.Hour == tNewTime.Hour) && (sg_tOldTime.Minute == tNewTime.Minute) ) {
			if ( tNewTime.Second % 2 ) {
				XuiCanvasDrawText(XuiStatusbarCanvas(), sg_iClockX + sg_iClockWidth[1], sg_iClockY, sg_iClockHeight, g_ptStBarFont, XUI_TEXT_NORMAL, colorClock, ":");
			} else {
				XuiClearArea(XuiStatusbarCanvas(), sg_iClockX + sg_iClockWidth[1], sg_iClockY, sg_iClockWidth[2], sg_iClockHeight);
			}
		} else {
			snprintf(buf, sizeof(buf), "%02d:%02d", tNewTime.Hour, tNewTime.Minute);
			XuiClearArea(XuiStatusbarCanvas(), sg_iClockX, 0, sg_iClockWidth[0], sg_iClockHeight+2);
			XuiCanvasDrawText(XuiStatusbarCanvas(), sg_iClockX, sg_iClockY, sg_iClockHeight, g_ptStBarFont,
					XUI_TEXT_NORMAL, colorClock, buf);
		}
		sg_tOldTime = tNewTime;

//		if ( gl_ucConnDevice == 'W' ) {
		if ( sg_iWifiOpened ) {
			iRet = OsWifiCheck(szCurESSID, NULL, &iCurRSSI);
			if ( !iRet ) {
				if (iCurRSSI > -33) {
					XuiSetStatusbarIcon(3, wifisignals[2]);
				} else if (iCurRSSI > -66) {
					XuiSetStatusbarIcon(3, wifisignals[1]);
				} else {
					XuiSetStatusbarIcon(3, wifisignals[0]);
				}
			} else {
				XuiSetStatusbarIcon(3, wifisignals[0]);
			}
		}
//			}
//		} else if ( gl_ucConnDevice == 'G' ) {
			if ( sg_iGprsOpened ) {
				iRet = OsWlGetSignal();
				if ( iRet >= 0 ) {
					XuiSetStatusbarIcon(0, netsignals[iRet]);
				}
			}
//		}

		//battery
		iBatteryLevel = OsCheckBattery();
		switch( iBatteryLevel ) {
		case BATTERY_LEVEL_0:
			XuiSetStatusbarIcon(7, batterys[0]);
			break;
		case BATTERY_LEVEL_1:
			XuiSetStatusbarIcon(7, batterys[1]);
			break;
		case BATTERY_LEVEL_2:
			XuiSetStatusbarIcon(7, batterys[2]);
			break;
		case BATTERY_LEVEL_3:
			XuiSetStatusbarIcon(7, batterys[3]);
			break;
		case BATTERY_LEVEL_4:
			XuiSetStatusbarIcon(7, batterys[4]);
			break;
		case BATTERY_LEVEL_CHARGE:
			XuiSetStatusbarIcon(7, batteryc[s_icount]);
			s_icount++;
			if (s_icount >= sizeof(batteryc)/sizeof(batteryc[0])) {
				s_icount = 0;
			}
			break;
		case BATTERY_LEVEL_COMPLETE:
			XuiSetStatusbarIcon(7, batteryc[4]);
			break;
		default:
			break;
		}
	}

}

static void *thread_StatusBarRefresh(void)
{
	while(1) {
		OsSleep(500);
		UpdateStatusbar();
	}
}

int Init_Display(void)
{
	int iRet=0;
	XuiColor tColorTitleBg;
	pthread_t thread;
	char szCurESSID[64]={0};
	int iCurRSSI=0;
	XuiWindow *ptRootCanvas=NULL;

	GuiInit(18);
	ptRootCanvas = XuiRootCanvas();
	g_ptXuiFont = XuiCreateFont("./res/PTS75F.ttf", 0, 0);
	g_ptStBarFont = XuiCreateFont("./res/ProFontWindows.ttf", 0, 0);
	tColorTitleBg.r = 0xff;
	tColorTitleBg.g = 0xff;
	tColorTitleBg.b = 0xff;
	tColorTitleBg.a = 0xff;
	XuiCanvasSetBackground(ptRootCanvas, XUI_BG_CENTER, NULL, tColorTitleBg);

	sg_iClockHeight = 14;
	sg_iClockWidth[0] = XuiTextWidth(g_ptStBarFont, sg_iClockHeight, "88:88");
	sg_iClockWidth[1] = XuiTextWidth(g_ptStBarFont, sg_iClockHeight, "88");
	sg_iClockWidth[2] = XuiTextWidth(g_ptStBarFont, sg_iClockHeight, ":");

	sg_iClockX = XUI_CENTER_X(0, XuiStatusbarCanvas()->width, sg_iClockWidth[0]);
	sg_iClockY = XUI_CENTER_Y(0, XuiStatusbarCanvas()->height, sg_iClockHeight);

	iRet = OsWlCheck();
	Pax_Log(LOG_INFO, "OsWlCheck ret = %d", iRet);
	if ( RET_OK == iRet ) {
		sg_iGprsOpened = 1;
	}
	iRet = OsWifiCheck(szCurESSID, NULL, &iCurRSSI);
	Pax_Log(LOG_INFO, "OsWifiCheck ret = %d", iRet);
	if ( RET_OK == iRet ) {
		sg_iWifiOpened = 1;
	}
	UpdateStatusbar();
	pthread_create(&thread, NULL, thread_StatusBarRefresh, NULL);

	return MP_OK;
}


void Destroy_Display(void)
{
	if ( g_ptMpLogo )
		XuiImgFree(g_ptMpLogo);
	if ( g_ptXuiFont )
		XuiDestroyFont(g_ptXuiFont);
	if ( g_ptStBarFont )
		XuiDestroyFont(g_ptStBarFont);
	GuiDeinit();
}

void Beep_Prompt(char cFlag)
{
	if ( cFlag == 0 ) {
		OsBeep(5, 100);
	}
	else {
		OsBeep(1, 100);
		OsSleep(1 * 100);
		OsBeep(1, 100);
		OsSleep(1 * 100);
		OsBeep(1, 100);
	}
}

void Display_Prompt(const char *psztitle, const char *pszStr, MSGTYPE eMsgType, int iNeedConfirm)
{
	int iPromptHeight=0;
	int iConfirmHeight=0;
	int iStrWidth=0;
	int iStrX=0, iStrY=0;
	int iPosX=0, iPosY=0;
	int iGreyAreaHeight=0;
	int iGifSize=0;
	char *pszWaitPrompt="Please wait.";
	char szConfirmPrompt[24]={0};
	XuiColor tColorLightGrey = {245, 245,245,255};
	XuiColor tColorBlack = {0, 0, 0, 255};
	ST_TIMER tTimer;
	int iCountDown=0;

	if ( g_ptGifLoader != NULL ) {
		XuiDestroyWindow(g_ptGifLoader);
		g_ptGifLoader = NULL;
	}

	if ( XuiRootCanvas()->width == 320) {
		iGifSize = 110;
		iGreyAreaHeight = 31;
	} else {
		iGifSize = 150;
		iGreyAreaHeight = 41;
	}

	iPromptHeight = 16;
	iConfirmHeight = 16;
	XuiClearArea(XuiRootCanvas(), 0, 0, XuiRootCanvas()->width, XuiRootCanvas()->height);
	//auto fone size
	while(1) {
		iStrWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, pszStr);
		if ( iStrWidth >= XuiRootCanvas()->width ) {
			iPromptHeight--;
		} else {
			break;
		}
	}

	if ( eMsgType == MSGTYPE_NORMAL ) {
		iStrX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
		iStrY = XUI_CENTER_Y(0, XuiRootCanvas()->height, iPromptHeight);
		XuiCanvasDrawText(XuiRootCanvas(), iStrX, iStrY, iPromptHeight, g_ptXuiFont, 0, tColorBlack, pszStr);
	} else{
		XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, XuiRootCanvas()->width, iGreyAreaHeight, tColorLightGrey, 0, 1);
		XuiCanvasDrawRect(XuiRootCanvas(), 0, iGreyAreaHeight, XuiRootCanvas()->width, 1, tColorBlack, 0, 1);

		iStrWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, psztitle);
		iStrX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
		iStrY = XUI_CENTER_Y(0, iGreyAreaHeight, iPromptHeight);
		XuiCanvasDrawText(XuiRootCanvas(), iStrX, iStrY, iPromptHeight, g_ptXuiFont, 0, tColorBlack, psztitle);

		iPosX = XUI_CENTER_X(0, XuiRootCanvas()->width, iGifSize);
		iPosY = XUI_CENTER_Y(0, XuiRootCanvas()->height, iGifSize);

		if ( eMsgType == MSGTYPE_LOADING ) {
			g_ptGifLoader = XuiCreateGif(XuiRootCanvas(), iPosX, iPosY, iGifSize, iGifSize, Add_Prefix_Res_Path("loader_loading.gif"));
		} else if ( eMsgType == MSGTYPE_SUCCESS ) {
			g_ptGifLoader = XuiCreateGif(XuiRootCanvas(), iPosX, iPosY, iGifSize, iGifSize, Add_Prefix_Res_Path("loader_success.gif"));
			Beep_Prompt(0);
		} else if ( eMsgType == MSGTYPE_WARNING ) {
			g_ptGifLoader = XuiCreateGif(XuiRootCanvas(), iPosX, iPosY, iGifSize, iGifSize, Add_Prefix_Res_Path("loader_warning.gif"));
		} else {
			g_ptGifLoader = XuiCreateGif(XuiRootCanvas(), iPosX, iPosY, iGifSize, iGifSize, Add_Prefix_Res_Path("loader_fail.gif"));
			Beep_Prompt(1);
		}

		//do not check needconfirm flag
		XuiShowWindow(g_ptGifLoader, 1, 0);
		iStrWidth = XuiTextWidth(g_ptXuiFont, iConfirmHeight, pszStr);
		//auto fone size
		while(1) {
			iStrWidth = XuiTextWidth(g_ptXuiFont, iConfirmHeight, pszStr);
			if ( iStrWidth >= XuiRootCanvas()->width ) {
				iConfirmHeight--;
			} else {
				break;
			}
		}
		iStrX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
		XuiCanvasDrawText(XuiRootCanvas(), iStrX, XuiRootCanvas()->height - 2 * iConfirmHeight - 2 - 20, iConfirmHeight, g_ptXuiFont, 0, tColorBlack, pszStr);

		if ( eMsgType == MSGTYPE_LOADING ) {
			iStrWidth = XuiTextWidth(g_ptXuiFont, iConfirmHeight, pszWaitPrompt);
			iStrX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
			XuiCanvasDrawText(XuiRootCanvas(), iStrX, XuiRootCanvas()->height - iConfirmHeight - 20, iConfirmHeight, g_ptXuiFont, 0, tColorBlack, pszWaitPrompt);
		} else {
			OsSleep(3 * 1000);
		}
	}
}

void Display_Last_Result(const char *pszAmount, const char *pszOrderId, int iPaymentStatus)
{
	int iPromptHeight=0;
	int iConfirmHeight=0;
	int iStrWidth=0, titleWidth=0;
	int iStrX=0;
	char szPrompt[48]={0};
	char szPromptTitle[12]={0};
	XuiColor tColorBlack = {0, 0, 0, 255};

	iPromptHeight = 20;
	iConfirmHeight = 18;
	XuiClearArea(XuiRootCanvas(), 0, 40, XuiRootCanvas()->width, XuiRootCanvas()->height - 80);

	iStrWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, "Last Transaction");
	iStrX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
	XuiCanvasDrawText(XuiRootCanvas(), iStrX, 45, iPromptHeight, g_ptXuiFont, 0, tColorBlack, "Last Transaction");

	//OrderId
	memset(szPrompt, 0, sizeof(szPrompt));
	memset(szPromptTitle, 0, sizeof(szPromptTitle));
	memcpy(szPromptTitle, "OrderId:", 9);
	snprintf(szPrompt, sizeof(szPrompt), "%s", pszOrderId);
	//auto fone size
	while(1) {
		titleWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, szPromptTitle);
		iStrWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, szPrompt);
		if ( iStrWidth >= XuiRootCanvas()->width - titleWidth ) {
			iPromptHeight--;
		}
		else {
			break;
		}
	}
	XuiCanvasDrawText(XuiRootCanvas(), 2, 80 + iPromptHeight + 2, iPromptHeight, g_ptXuiFont, 0, tColorBlack, szPromptTitle);
	XuiCanvasDrawText(XuiRootCanvas(), XuiRootCanvas()->width - iStrWidth - 2, 80 + iPromptHeight + 2, iPromptHeight, g_ptXuiFont, 0, tColorBlack, szPrompt);
	//Amount
	memset(szPrompt, 0, sizeof(szPrompt));
	memset(szPromptTitle, 0, sizeof(szPromptTitle));
	memcpy(szPromptTitle, "Amount:", 8);
	titleWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, szPromptTitle);
	XuiCanvasDrawText(XuiRootCanvas(), 2, 80, iPromptHeight, g_ptXuiFont, 0, tColorBlack, szPromptTitle);
	snprintf(szPrompt, sizeof(szPrompt), "%s kr.", pszAmount);
	iStrWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, szPrompt);
	XuiCanvasDrawText(XuiRootCanvas(), XuiRootCanvas()->width - iStrWidth - 2, 80, iPromptHeight, g_ptXuiFont, 0, tColorBlack, szPrompt);

	//Payment Result
	memset(szPrompt, 0, sizeof(szPrompt));
	memset(szPromptTitle, 0, sizeof(szPromptTitle));
	memcpy(szPromptTitle, "Payment Result:", 16);
	titleWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, szPromptTitle);
	XuiCanvasDrawText(XuiRootCanvas(), 2, 80 + 2 * (iPromptHeight + 2), iPromptHeight, g_ptXuiFont, 0, tColorBlack, szPromptTitle);

	switch (iPaymentStatus) {
	case PAYMENT_STATUS_IDLE:
		snprintf(szPrompt, sizeof(szPrompt), "%s", "IDLE");
		break;
	case PAYMENT_STATUS_ISSUED:
		snprintf(szPrompt, sizeof(szPrompt), "%s", "WAIT ACCEPT");
		break;
	case PAYMENT_STATUS_AWAIT_CHECKIN:
		snprintf(szPrompt, sizeof(szPrompt), "%s", "WAIT CHECKIN");
		break;
	case PAYMENT_STATUS_CANCEL:
		snprintf(szPrompt, sizeof(szPrompt), "%s", "CANCELED");
		break;
	case PAYMENT_STATUS_ERROR:
		snprintf(szPrompt, sizeof(szPrompt), "%s", "ERROR");
		break;
	case PAYMENT_STATUS_PAYMENT_ACCEPTED:
		snprintf(szPrompt, sizeof(szPrompt), "%s", "WAIT CONFIRM");
		break;
	case PAYMENT_STATUS_DONE:
		snprintf(szPrompt, sizeof(szPrompt), "%s", "PAID");
		break;
	default:
		break;
	}

	iStrWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, szPrompt);
	XuiCanvasDrawText(XuiRootCanvas(), XuiRootCanvas()->width - iStrWidth - 2, 80 + 2 * (iPromptHeight + 2), iPromptHeight, g_ptXuiFont, 0, tColorBlack, szPrompt);


	iStrWidth = XuiTextWidth(g_ptXuiFont, iConfirmHeight, "PLEASE CONFIRM");
	iStrX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
	XuiCanvasDrawText(XuiRootCanvas(), iStrX, XuiRootCanvas()->height - iConfirmHeight - 2 -18, iConfirmHeight, g_ptXuiFont, 0, tColorBlack, "PLEASE CONFIRM");

	while(1) {
		if (XuiHasKey()) {
			if (XuiGetKey() == KEYENTER)
				break;
		}
	}
}

void Display_PaymentStatus(const char *pszStr)
{
	int iPromptHeight=0;
	int iStrWidth=0;
	int iStrX=0, iStrY=0;
	XuiColor tColorBlack = {0, 0, 0, 255};

	if ( XuiRootCanvas()->width == 320) {
		iPromptHeight = 14;
		iStrY = 110;
	} else {
		iPromptHeight = 18;
		iStrY = 160;
	}
	XuiClearArea(XuiRootCanvas(), 0, iStrY, XuiRootCanvas()->width, iPromptHeight + 2);

	//auto fone size
	while(1) {
		iStrWidth = XuiTextWidth(g_ptXuiFont, iPromptHeight, pszStr);
		if ( iStrWidth >= XuiRootCanvas()->width ) {
			iPromptHeight--;
		}
		else {
			break;
		}
	}
	iStrX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
	XuiCanvasDrawText(XuiRootCanvas(), iStrX, iStrY, iPromptHeight, g_ptXuiFont, 0, tColorBlack, pszStr);
}

void Display_QRCode_Screen(const char *pszPosUnitIdQR)
{
	XuiImg *ptBGImg, *ptQRcodeImg;
	int iPosX=0, iPosY=0;
	QRcode *tQRcode;
	int iAmountHeight=0, iCurrencyHeight=0;
	int iStrWidth=0;
	char szAmountPrompt[12+1]={0};
	XuiColor tColorBlack = {0, 0, 0, 255};
	XuiColor tColorGreen = {116,179,67,255};

	if ( g_ptGifLoader != NULL ) {
		XuiDestroyWindow(g_ptGifLoader);
		g_ptGifLoader = NULL;
	}

	if ( XuiRootCanvas()->width == 320 ) {
		iAmountHeight = 30;
		iCurrencyHeight = 16;
		iPosY = 70;
	} else {
		iAmountHeight = 30;
		iCurrencyHeight = 16;
		iPosY = 120;
	}

	XuiClearArea(XuiRootCanvas(), 0, 0, XuiRootCanvas()->width, XuiRootCanvas()->height);

	ptBGImg = XuiImgLoadFromFile(Add_Prefix_Res_Path("payment.png"));
	XuiCanvasDrawImg(XuiRootCanvas(), 0, 0, ptBGImg->width, ptBGImg->height, XUI_BG_NORMAL, ptBGImg);

	memcpy(szAmountPrompt, g_tTxnInfo.Amount, sizeof(g_tTxnInfo.Amount));
	*strchr(szAmountPrompt, '.') = ',';
	iStrWidth = XuiTextWidth(g_ptXuiFont, iAmountHeight, szAmountPrompt);
	iPosX = XUI_CENTER_X(0, XuiRootCanvas()->width, iStrWidth);
	XuiCanvasDrawText(XuiRootCanvas(), iPosX, iPosY, iAmountHeight, g_ptXuiFont, 0, tColorGreen, szAmountPrompt);

	iStrWidth = XuiTextWidth(g_ptXuiFont, iCurrencyHeight, CURRENCY_NAME);
	XuiCanvasDrawText(XuiRootCanvas(), XuiRootCanvas()->width - iStrWidth - 15, iPosY + (iAmountHeight-iCurrencyHeight), iCurrencyHeight, g_ptXuiFont, 0, tColorBlack, CURRENCY_NAME);
	//display qrcode
	tQRcode = QRcode_encodeString(pszPosUnitIdQR, 1, QR_ECLEVEL_L, QR_MODE_8, 1);
//	writePNG(tQRcode, FILE_QRCODE_IMG);
	qrToPNG(tQRcode, FILE_QRCODE_IMG, 2, 2);

	ptQRcodeImg = XuiImgLoadFromFile(FILE_QRCODE_IMG);
	Pax_Log(LOG_INFO, "ptQRcodeImg.width = %d, ptQRcodeImg.height = %d", ptQRcodeImg->width, ptQRcodeImg->height);
	XuiCanvasDrawImg(XuiRootCanvas(), XuiRootCanvas()->width - ptQRcodeImg->width - 15, XuiRootCanvas()->height - ptQRcodeImg->height - 33, ptQRcodeImg->width, ptQRcodeImg->height, XUI_BG_NORMAL, ptQRcodeImg);
	Display_PaymentStatus("WAITING FOR CHECKIN(30)");

	XuiImgFree(ptQRcodeImg);
	ptQRcodeImg = NULL;
}

void Display_Error_Code(int iErrCode)
{
	int iRequestType = Get_Request_Type();

	switch (iErrCode) {
	//paymentRefund: Order cannot be found
	case 1:
		if ( iRequestType == CMD_PAYMENT_REFUND ) {
			Display_Prompt("REFUND FAILED", "Order Not Found.", MSGTYPE_FAILURE, 0);
		}
		break;
	//paymentRefund: Amount is larger than remaining refundable amount on transaction
	case 2:
		if ( iRequestType == CMD_PAYMENT_REFUND ) {
			Display_Prompt("REFUND FAILED", "Amount Too Large.", MSGTYPE_FAILURE, 0);
		}
		break;
	//paymentRefund: Transaction is already refunded
	case 3:
		if ( iRequestType == CMD_PAYMENT_REFUND ) {
			Display_Prompt("REFUND FAILED", "Already Refunded.", MSGTYPE_FAILURE, 0);
		}
		break;
	//Missing or invalid parameters
	case 10:
		Display_Prompt("TECHNICAL FAILED", "Miss Or Invalid Parameter.", MSGTYPE_FAILURE, 0);
		break;
	//GetUniquePoSId: The Key: MerchantId does not exist
	case 20:
		if ( iRequestType == CMD_ADMIN_GET_UNIQUE_POS_ID ) {
			Display_Prompt("REGISTER FAILED", "MerchantId Not Exist.", MSGTYPE_FAILURE, 0);
		}
		break;
	//The key ��MerchantId, LocationId and PoSId�� does not exist
	case 30:
		Display_Prompt("TECHNICAL FAILED", "Parameter Not Exist.", MSGTYPE_FAILURE, 0);
		break;
	//ReadPoSAssignPoSUnitId: Current PoS has no PoSUnitId assigned
	case 31:
		if ( iRequestType == CMD_ADMIN_READ_POS_ASSIGNED_POS_UNITID ) {
			Display_Prompt("READ UNITID ERROR", "No PosunitId Assigned.", MSGTYPE_FAILURE, 0);
		}
		break;
	//Payment: Payment already exists and has been paid
	case 40:
		if ( iRequestType == CMD_PAYMENT_START ) {
			Display_Prompt("PAYMENT FAILED", "Payment Exist And Paid.", MSGTYPE_FAILURE, 0);
		}
		break;
	//Payment: Payment already in progress
	//PaymentCancel: Action not possible
	case 50:
		if ( iRequestType == CMD_PAYMENT_START ) {
			Display_Prompt("PAYMENT FAILED", "Payment In Progress.", MSGTYPE_FAILURE, 0);
		}
		else if ( iRequestType == CMD_PAYMENT_CANCEL ) {
			Display_Prompt("CANCEL FAILED", "Cancel Failed Try Later(MAX 90s).", MSGTYPE_FAILURE, 0);
		}
		break;
	//paymentRefund: Technical error (refund cannot be performed)
	case 99:
		if ( iRequestType == CMD_PAYMENT_REFUND ) {
			Display_Prompt("REFUND FAILED", "TECHNICAL ERROR", MSGTYPE_FAILURE, 0);
		}
		break;
	default:
		break;
	}
}



