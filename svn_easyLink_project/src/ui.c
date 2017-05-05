//============================================================================
// Name        : ui.c
// Author      :tuxq
// Version     :
// Copyright   : Your copyright notice
// Description : Ansi-style
//============================================================================
#include "../inc/global.h"

#define LOG_TAG	__FILE__
#ifdef UI_DEBUG_		
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else
#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)

#endif // UI_DEBUG_	


extern int BmpConvert(unsigned char *pucFileName, unsigned char *pucConvBmp, unsigned int uiConvBmpSize); // === Added by ZhangYuan 2016-11-15


static ST_INPUTTYPE stTypeList[]={
	{INPUT_TYPE_NUM, NUM},
	{INPUT_TYPE_ALPHA, ALPHA},
	{INPUT_TYPE_DECIMAL, DECIMAL},
	{INPUT_TYPE_PASSWARD, PASSWARD},
	{INPUT_TYPE_PINPAD,PIN_PAD},
};

static uchar *RetToString(int ReturnCode)
{
	switch(ReturnCode)
	{
	case RET_OK:
		return "Success";

	case EL_UI_RET_INVALID_WIDGETNAME:
		return "Invalid Widget Name";

	case EL_UI_RET_TIME_OUT:
		return "Timeout";

	case EL_UI_RET_INVALID_PAGE:
		return "Invalid Page";

	case EL_UI_RET_PARSEUI_FAILED:
		return "Parse UI Failed";

	case EL_UI_RET_VALUESIZEERROR:
		return "Value Size Error";

	case EL_UI_RET_INPUT_TYPE_ERROR:
		return "Input Type Error";

	case EL_UI_RET_INVALID_WIDGETVALUE:
		return "Invalid Widget Value";

	case EL_UI_RET_USER_CANCEL:
		return "User Cancel";

	case EL_UI_RET_MENUITEMNUM_ERROR:
		return "Menu Item Num Error";

	default:
		return "Unkown";
	}
}

static int GetMsgData(uchar *pucMsg, ST_DATA *pstData, uchar ucWidgetNum){
	int iMsgOffset = 0, iLength = 0, i = 0;

	for(i; i<	ucWidgetNum; i++)
	{
		if( pucMsg[iMsgOffset] != 0)
		{
			iLength = pucMsg[iMsgOffset];
			if(iLength > WIDGETNAME_LENGTH_MAX)
			{
				ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETNAME, iLength , __LINE__);
				return EL_UI_RET_INVALID_WIDGETNAME;
			}
			iMsgOffset++;
			memcpy(pstData[i].ucWidgetName, pucMsg + iMsgOffset, iLength);
			iMsgOffset += iLength;
			iLength = pucMsg[iMsgOffset];
			if(iLength > WIDGETVALUE_LENGTH_MAX)
			{
				ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETVALUE, iLength , __LINE__);
				return EL_UI_RET_INVALID_WIDGETVALUE;
			}
			iMsgOffset++;
			memcpy(pstData[i].ucWidgetValue, pucMsg + iMsgOffset, iLength);
			iMsgOffset += iLength;		
		}		
	}
	return RET_OK;
}

static int GetItemNum(uchar *ss,uchar c, int iSize)
{
	int count = 0, i = 0;
	for(i = 0; i < iSize; ss++, i++)
	{
		if( *ss == c)
		{
			count++;
		}
	}
	return count;
}

static int GetInputType(uchar ucTypeIndex){
	int i = 0;
	for(;i < sizeof(stTypeList)/sizeof(ST_INPUTTYPE); i++)
	{
		if(ucTypeIndex == stTypeList[i].ucTypeIndex)
		{
			return stTypeList[i].ucTypeValue;
		}
	}
	ComPoseInfoMsg(RetToString, EL_UI_RET_INPUT_TYPE_ERROR, (int)ucTypeIndex , __LINE__);
	return EL_UI_RET_INPUT_TYPE_ERROR;
}

static void GetSubStr(char * dest, const char *src, uchar ucFont){
	int iLen = strlen(src);
	uchar tempbuff[WIDGETVALUE_LENGTH_MAX] = {0};
	if(ucFont == 0)
	{
		if(iLen > FONT_0_MAX)
		{
			strncpy(tempbuff, src, FONT_0_MAX-3);
			sprintf(dest, "%s...", tempbuff);
		}
		else
		{
			memcpy(dest, src, iLen);
		}
	}
	if(ucFont != 0)
	{
		if(iLen > FONT_1_MAX)
		{
			strncpy(tempbuff, src, FONT_1_MAX-3);
			sprintf(dest, "%s...", tempbuff);
		}
		else
		{
			memcpy(dest, src, iLen);
		}
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "src string = %s \n", src);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "dest string = %s \n",dest);	
}

static int SetShowAlign(unsigned char ucXLocation, char * pszInfo, int iAlign, int iFontSize)
{
	int iXLocation =0;
	int iLen = 0;

	iLen = strlen(pszInfo);	
	if(pszInfo != NULL)
	{
		if(iAlign == ALIGN_LEFT)
		{
			iXLocation = ucXLocation;
		}

		if(iAlign == ALIGN_CENTER)
		{
			if(iFontSize == 0)
			{
				iXLocation = ucXLocation + (128 - 6*iLen)/2;
			}
			if(iFontSize == 1)
			{
				iXLocation = ucXLocation + (128 - 8*iLen)/2;
			}
		}

		if(iAlign == ALIGN_RIGHT)
		{
			if(iFontSize == 0)
			{
				iXLocation = 128 - 6*iLen;
			}
			if(iFontSize == 1)
			{
				iXLocation = 128 - 8*iLen;
			}
		}
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iLen = %d, iAlign = %d, iFontSize=%d, iXLocation = %d \n", iLen, iAlign, iFontSize, iXLocation);

	return iXLocation;
}


/* fontType: 	 0 - 6x8/16x16		21个宽x 高
1 - 8x16/16x16		16个
2 - 12x24/24x24		10个
*/
static void SetCustomizedFont(int iFontType)
{
	ST_FONT singleFont, doubleFont;

	memset(&singleFont, 0, sizeof(ST_FONT));
	memset(&doubleFont, 0, sizeof(ST_FONT));

	singleFont.CharSet = CHARSET_WEST;
	doubleFont.CharSet = CHARSET_GB2312;

	if (0 == iFontType)
	{
		singleFont.Width = 6;
		singleFont.Height = 8;
		doubleFont.Width = 16;
		doubleFont.Height = 16;
	}
	else if (0 != iFontType)
	{
		singleFont.Width = 8;
		singleFont.Height = 16;
		doubleFont.Width = 16;
		doubleFont.Height = 16;
	}

	ScrSelectFont(&singleFont, &doubleFont);
}


static int MenuProcess(int iItemNum, ST_MENU_WIDGET* pstMenu, unsigned short usTimeOut){
	int		iTotalPage, iCurrentPage;
	uchar	ucKey;

	if ( (0 >= iItemNum) || (NULL == pstMenu) )
	{
		return -1;
	}

	iTotalPage = (iItemNum+1)/2; 	// Get the total page to show all items
	iCurrentPage = 0;

	while (1)
	{
		ScrClrLine(2, 5);	// clear the items on previous page
		ScrClrRect(91, 0, 127, 16);		// clear the "up" "down" promption on previous page
		if (iCurrentPage*2 < iItemNum)
		{
			ScrPrint(0, 2, pstMenu[iCurrentPage*2].fontsize, "1.%s", pstMenu[iCurrentPage*2].value);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "menuitem1 = %s \n", pstMenu[iCurrentPage*2].value);
		}
		if (iCurrentPage*2+1 < iItemNum)
		{
			ScrPrint(0, 4, pstMenu[iCurrentPage*2].fontsize, "2.%s", pstMenu[iCurrentPage*2+1].value);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "menuitem2 = %s \n", pstMenu[iCurrentPage*2+1].value);
		}

		// take the last 36 dots of line 0 and line 1 to display prompt to tell user page down or page up.
		// 36 dots comes from:  strlen("6-DOWN") is 6 characters. With ASCII font, it will take 6*6 dots to display this.
		if (iTotalPage > 1)
		{	
			if (0 != iCurrentPage)
			{
				ScrPrint(91, 0, ASCII, "6-UP");
			}
			if ((iTotalPage-1) != iCurrentPage)
			{
				ScrPrint(91, 1, ASCII, "9-DOWN");
			}
		}

		kbflush();
		ucKey = PubWaitKey(usTimeOut);
		if (KEY1 == ucKey)
		{
			if (iCurrentPage*2 < iItemNum)
			{
				return iCurrentPage * 2;
			}
		}
		else if (KEY2 == ucKey)
		{
			if (iCurrentPage * 2 + 1 < iItemNum)
			{
				return iCurrentPage * 2 + 1;
			}
		}
		else if (KEY9 == ucKey)	// key 6 to page down. if it's the last page, then turn to the first page
		{
			if ((iTotalPage-1) == iCurrentPage)
			{
				iCurrentPage = 0;
			}
			else
			{
				iCurrentPage++;
			}
			continue;
		}
		else if (KEY6 == ucKey)	// key 3 to page up. if it's the first page, then turn to the last page
		{
			if (0 == iCurrentPage)
			{
				iCurrentPage = iTotalPage - 1;
			}
			else
			{
				iCurrentPage--;
			}
			continue;
		}
		else if (NOKEY == ucKey)
		{
			return -2;
		}
		else if (KEYCANCEL == ucKey)
		{
			return -3;
		}
	} 

	return -1;
}

static int InputProcess(ST_INPUTBOX_WIDGET *pstInputboxWidget, uchar *pucResponse,  int iRspBuffLen, unsigned short usTimeout){
	int iRet = 0;
	uchar ucInputType = 0, bChkTimer = 0;
	memcpy(pucResponse, pstInputboxWidget->value, iRspBuffLen);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usTimeout = %d, stInputboxWidget.type = %02x\n", usTimeout, pstInputboxWidget->type);
	ucInputType = GetInputType(pstInputboxWidget->type);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "GetInputType = %02x\n", ucInputType);
	if(pstInputboxWidget->textalign == ALIGN_LEFT)
	{
		ucInputType |= 0x04 ;
	}
	if(pstInputboxWidget->textalign == ALIGN_RIGHT)
	{
		ucInputType &= 0xFB ;
	}
	if(pstInputboxWidget->fontsize != 0)
	{
		ucInputType |= 0x40;
	}
	if(pstInputboxWidget->fontsize == 0)
	{
		ucInputType &= 0xBF;
	}
	if(pstInputboxWidget->mode == 1)
	{
		ucInputType &= 0xFE;
	}
	if(pstInputboxWidget->mode == 0)
	{
		ucInputType |= 0x01;
	}
	ScrGotoxyEx(pstInputboxWidget->x, pstInputboxWidget->y);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucInputType final= 0x%02x\n", ucInputType);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stInputboxWidget.minlen = %d\n", pstInputboxWidget->minlen);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stInputboxWidget.maxlen = %d\n", pstInputboxWidget->maxlen);	

	if (usTimeout >= 0)
	{
		bChkTimer = TRUE;
		TimerSet(TIMER_TEMPORARY, (short)(usTimeout)*2);
	}
	else
	{
		bChkTimer = FALSE;
	}
	while (1)
	{
		if(pstInputboxWidget->type != INPUT_TYPE_PINPAD)
		{
			iRet = GetString(pucResponse, ucInputType, pstInputboxWidget->minlen, MIN(pstInputboxWidget->maxlen, iRspBuffLen));
			break;
		}
		else	// if INPUT_TYPE_PINPAD, just return
		{
			break;
		}

		if (usTimeout != 0 && bChkTimer && TimerCheck(TIMER_TEMPORARY)==0)
		{
			ScrCls();
			return EL_UI_RET_TIME_OUT;
		}
	}

	return RET_OK;
}

static int SetElement(const uchar *buff,  const ST_DATA *pucMsg, uchar *pucKeyAccept){
	int   iRet = 0, xmlOffset = 0, iMsgOffset = 0, iLength = 0, iFd = 0, iFileSize = 0;
	uchar ucXLocation = 0;

	uchar ucTextBuff[WIDGETVALUE_LENGTH_MAX + 1]={0};
	uchar ucPicBuff[1024]={0};

	uchar XmlTag = 1;

	ST_TEXTBOX_WIDGET stTextboxWidget;
	ST_PICTURE_WIDGET stPictureWidget;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucMsg = %s\n", pucMsg==NULL ? "pucMsg == NULL" : pucMsg);

	ScrCls();
	while(XmlTag == 1)
	{
		switch(buff[xmlOffset])
		{
		case TEXTBOX:
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TextBox, xmlOffset = %d, buff[%d]=%d\n", xmlOffset, xmlOffset, buff[xmlOffset]);
			memset(ucTextBuff, 0, sizeof(ucTextBuff));
			xmlOffset++;
			memcpy(&stTextboxWidget, buff +xmlOffset, sizeof(ST_TEXTBOX_WIDGET));

			*pucKeyAccept = stTextboxWidget.keyaccept;
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "*pucKeyAccept = %d\n", *pucKeyAccept);					
			SetCustomizedFont(stTextboxWidget.fontsize);
			ScrAttrSet(stTextboxWidget.mode);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iMsgOffset = %d \n", iMsgOffset);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "strlen(pucMsg[iMsgOffset].ucWidgetName) = %d \n", strlen(pucMsg[iMsgOffset].ucWidgetName));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucMsg[iMsgOffset].ucWidgetName = %s \n", pucMsg[iMsgOffset].ucWidgetName);		
			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(stTextboxWidget.name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(stTextboxWidget.name))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETNAME, strlen(pucMsg[iMsgOffset].ucWidgetName) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(stTextboxWidget.value))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETVALUE, strlen(pucMsg[iMsgOffset].ucWidgetValue) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucTextBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(ucTextBuff));
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucTextBuff = %s\n", ucTextBuff);
					if(ucTextBuff  !=  NULL)
					{
						memcpy(stTextboxWidget.value, ucTextBuff, sizeof(ucTextBuff));
						memset(ucTextBuff, 0, sizeof(ucTextBuff));							
					}
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stTextboxWidget.value = %s\n", stTextboxWidget.value);	
					iMsgOffset++;
				}			
			}

			GetSubStr(ucTextBuff, stTextboxWidget.value, stTextboxWidget.fontsize);
			ucXLocation = SetShowAlign(stTextboxWidget.x, ucTextBuff, stTextboxWidget.textalign, stTextboxWidget.fontsize);
			if(ucXLocation == EL_UI_RET_VALUESIZEERROR)
			{
				ComPoseInfoMsg(RetToString, EL_UI_RET_VALUESIZEERROR, ucXLocation , __LINE__);
				return EL_UI_RET_VALUESIZEERROR;
			}
			if(stTextboxWidget.mode == 1)
			{
				if(stTextboxWidget.fontsize == 0 && stTextboxWidget.y == 0){
					ScrPrint(0, 0, ASCII | REVER, "%*s", FONT_0_MAX, "");
				}
				if(stTextboxWidget.fontsize == 1 && stTextboxWidget.y == 0){
					ScrPrint(0, 0, CFONT | REVER, "%*s", FONT_1_MAX, "");
				}
			}
			ScrTextOut(ucXLocation, stTextboxWidget.y, ucTextBuff);
			xmlOffset += sizeof(ST_TEXTBOX_WIDGET);
			break;

		case PICTUREBOX:
			memset(ucTextBuff, 0, sizeof(ucTextBuff));
			memset(ucPicBuff, 0, sizeof(ucPicBuff));
			xmlOffset ++;
			memcpy(&stPictureWidget, buff +xmlOffset, sizeof(ST_PICTURE_WIDGET));
			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(stPictureWidget.name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(stPictureWidget.name))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETNAME, strlen(pucMsg[iMsgOffset].ucWidgetName) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(stPictureWidget.value))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETVALUE, strlen(pucMsg[iMsgOffset].ucWidgetValue) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucTextBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(ucTextBuff));
					if(ucTextBuff  !=  NULL)
					{
						memcpy(stPictureWidget.value, ucTextBuff, sizeof(ucTextBuff));
						memset(ucTextBuff, 0, sizeof(ucTextBuff));							
					}
					iMsgOffset++;	
				}
			}													

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PictureBox, ucWidgetName = %s\n", stPictureWidget.name);					
			iRet = BmpConvert(stPictureWidget.value, ucPicBuff, sizeof(ucPicBuff));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "BmpToUiLogo = %d\n", iRet);
			if(iRet == 0)
			{
				ScrDrLogoxy(stPictureWidget.x, stPictureWidget.y, ucPicBuff);
			}
			xmlOffset += sizeof(ST_TEXTBOX_WIDGET);
			break;

		case INPUTBOX:
			ScrCls();
			ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_PAGE, EL_UI_RET_INVALID_PAGE , __LINE__);
			return EL_UI_RET_INVALID_PAGE;

		case MENUITEM:
			ScrCls();
			ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_PAGE, EL_UI_RET_INVALID_PAGE , __LINE__);
			return EL_UI_RET_INVALID_PAGE;						

		default:
			XmlTag = 0;
			break;
		}
		if(XmlTag == 0)
		{
			break;
		}
	}

	return 0;
}

static int SetInputboxElement(const uchar *buff,  const ST_DATA *pucMsg, uchar *pucKeyAccept, ST_INPUTBOX_WIDGET *pstInputboxWidget){
	int   iRet = 0, xmlOffset = 0, iMsgOffset = 0, iLength = 0, iFd = 0, iFileSize = 0;
	uchar ucXLocation = 0;

	uchar ucTextBuff[WIDGETVALUE_LENGTH_MAX + 1]={0};
	uchar ucPicBuff[1024]={0};

	uchar XmlTag = 1;

	ST_TEXTBOX_WIDGET stTextboxWidget;
	ST_PICTURE_WIDGET stPictureWidget;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucMsg = %s\n", pucMsg==NULL ? "pucMsg == NULL" : pucMsg);	

	ScrCls();
	while(XmlTag == 1)
	{
		switch(buff[xmlOffset])
		{
		case TEXTBOX:
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "TextBox, xmlOffset = %d, buff[%d]=%d\n", xmlOffset, xmlOffset, buff[xmlOffset]);
			memset(ucTextBuff, 0, sizeof(ucTextBuff));
			xmlOffset++;
			memcpy(&stTextboxWidget, buff +xmlOffset, sizeof(ST_TEXTBOX_WIDGET));

			*pucKeyAccept = stTextboxWidget.keyaccept;
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "*pucKeyAccept = %d\n", *pucKeyAccept);

			SetCustomizedFont(stTextboxWidget.fontsize);
			ScrAttrSet(stTextboxWidget.mode);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "*iMsgOffset = %d, strlen(pucMsg[iMsgOffset].ucWidgetName) = %d, pucMsg[iMsgOffset].ucWidgetName = %s\n", iMsgOffset, strlen(pucMsg[iMsgOffset].ucWidgetName), pucMsg[iMsgOffset].ucWidgetName);	
			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(stTextboxWidget.name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(stTextboxWidget.name))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETNAME, strlen(pucMsg[iMsgOffset].ucWidgetName) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(stTextboxWidget.value))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETVALUE, strlen(pucMsg[iMsgOffset].ucWidgetValue) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucTextBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(ucTextBuff));
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucTextBuff = %s\n", ucTextBuff);
					if(ucTextBuff  !=  NULL)
					{
						memcpy(stTextboxWidget.value, ucTextBuff, sizeof(ucTextBuff));
						memset(ucTextBuff, 0, sizeof(ucTextBuff));							
					}
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stTextboxWidget.value = %s\n", stTextboxWidget.value);
					iMsgOffset++;
				}			
			}

			GetSubStr(ucTextBuff, stTextboxWidget.value, stTextboxWidget.fontsize);
			ucXLocation = SetShowAlign(stTextboxWidget.x, ucTextBuff, stTextboxWidget.textalign, stTextboxWidget.fontsize);
			if(ucXLocation == EL_UI_RET_VALUESIZEERROR)
			{
				ComPoseInfoMsg(RetToString, EL_UI_RET_VALUESIZEERROR, ucXLocation , __LINE__);
				return EL_UI_RET_VALUESIZEERROR;
			}
			if(stTextboxWidget.mode == 1)
			{
				if(stTextboxWidget.fontsize == 0 && stTextboxWidget.y == 0)
				{
					ScrPrint(0, 0, ASCII | REVER, "%*s", 21, "");
				}
				if(stTextboxWidget.fontsize == 1 && stTextboxWidget.y == 0)
				{
					ScrPrint(0, 0, CFONT | REVER, "%*s", 16, "");
				}
			}
			ScrTextOut(ucXLocation, stTextboxWidget.y, ucTextBuff);
			xmlOffset += sizeof(ST_TEXTBOX_WIDGET);
			break;

		case PICTUREBOX:
			memset(ucTextBuff, 0, sizeof(ucTextBuff));
			memset(ucPicBuff, 0, sizeof(ucPicBuff));
			xmlOffset ++;
			memcpy(&stPictureWidget, buff +xmlOffset, sizeof(ST_PICTURE_WIDGET));
			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(stPictureWidget.name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(stPictureWidget.name))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETNAME, strlen(pucMsg[iMsgOffset].ucWidgetName) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(stPictureWidget.value))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETVALUE, strlen(pucMsg[iMsgOffset].ucWidgetValue) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucTextBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(ucTextBuff));
					if(ucTextBuff  !=  NULL)
					{
						memcpy(stPictureWidget.value, ucTextBuff, sizeof(ucTextBuff));
						memset(ucTextBuff, 0, sizeof(ucTextBuff));							
					}
					iMsgOffset++;	
				}
			}

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "PictureBox, ucWidgetName = %s\n", stPictureWidget.name);
			iRet = BmpConvert(stPictureWidget.value, ucPicBuff, sizeof(ucPicBuff));
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "BmpToUiLogo = %d\n", iRet);
			if(iRet == 0){
				ScrDrLogoxy(stPictureWidget.x, stPictureWidget.y, ucPicBuff);
			}
			xmlOffset += sizeof(ST_TEXTBOX_WIDGET);
			break;

		case INPUTBOX:
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "InputBox, buff[%d]=%d\n", xmlOffset, buff[xmlOffset]);
			memset(ucTextBuff, 0, sizeof(ucTextBuff));
			xmlOffset ++;
			memcpy(pstInputboxWidget, buff +xmlOffset, sizeof(ST_INPUTBOX_WIDGET));
			EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stInputBoxWidget: ", pstInputboxWidget, sizeof(ST_INPUTBOX_WIDGET));
			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(pstInputboxWidget->name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(pstInputboxWidget->name))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETNAME, strlen(pucMsg[iMsgOffset].ucWidgetName) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(pstInputboxWidget->value))
					{
						ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_WIDGETVALUE, strlen(pucMsg[iMsgOffset].ucWidgetValue) , __LINE__);
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucTextBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(ucTextBuff));
					if(ucTextBuff  !=  NULL)
					{
						memcpy(pstInputboxWidget->value, ucTextBuff, sizeof(ucTextBuff));
						memset(ucTextBuff, 0, sizeof(ucTextBuff));							
					}
					iMsgOffset++;								
				}
			}
			xmlOffset += sizeof(ST_INPUTBOX_WIDGET);
			break;

		case MENUITEM:	
			ScrCls();
			return EL_UI_RET_INVALID_PAGE;						

		default:
			XmlTag = 0;
			break;
		}
		if(XmlTag == 0){
			break;
		}
	}

	return 0;
}

static int SetMenuElement(const uchar *buff, const ST_DATA *pucMsg, uchar *pucKeyAccept, ST_MENU_WIDGET *pstMenuItem, int *piMenuItemNum){
	int   iRet = 0, xmlOffset = 0, iMsgOffset = 0, iLength = 0;
	uchar ucMenuIndex = 0;
	uchar ucXLocation = 0;
	uchar ucTextBuff[WIDGETVALUE_LENGTH_MAX + 1]={0};
	uchar ucPicBuff[1024]={0};
	uchar ucMenuItemBuff[WIDGETVALUE_LENGTH_MAX + 1]={0};

	uchar XmlTag = 1;

	ST_TEXTBOX_WIDGET stTextboxWidget;
	ST_PICTURE_WIDGET stPictureWidget;
	ST_MENU_WIDGET stMenuWidget;	

	ScrCls();
	while(XmlTag == 1)
	{
		switch(buff[xmlOffset])
		{
		case TEXTBOX:
			memset(ucTextBuff, 0, sizeof(ucTextBuff));
			xmlOffset++;

			memcpy(&stTextboxWidget, buff +xmlOffset, sizeof(ST_TEXTBOX_WIDGET));

			//	strcpy(pucKeyAccept, &stTextboxWidget.keyaccept);//wrong
			pucKeyAccept = &stTextboxWidget.keyaccept;

			SetCustomizedFont(stTextboxWidget.fontsize);
			ScrAttrSet(stTextboxWidget.mode);
			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(stTextboxWidget.name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(stTextboxWidget.name))
					{
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(stTextboxWidget.value))
					{
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucTextBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(ucTextBuff));
					if(ucTextBuff  !=  NULL)
					{
						memcpy(stTextboxWidget.value, ucTextBuff, sizeof(ucTextBuff));
						memset(ucTextBuff, 0, sizeof(ucTextBuff));							
					}	
					iMsgOffset++;	
				}
			}


			GetSubStr(ucTextBuff, stTextboxWidget.value, stTextboxWidget.fontsize);
			ucXLocation = SetShowAlign(stTextboxWidget.x, ucTextBuff, stTextboxWidget.textalign, stTextboxWidget.fontsize);
			if(ucXLocation == EL_UI_RET_VALUESIZEERROR)
			{
				return EL_UI_RET_VALUESIZEERROR;
			}
			if(stTextboxWidget.mode == 1)
			{
				if(stTextboxWidget.fontsize == 0 && stTextboxWidget.y == 0)
				{
					ScrPrint(0, 0, ASCII | REVER, "%*s", 21, "");
				}
				if(stTextboxWidget.fontsize == 1 && stTextboxWidget.y == 0)
				{
					ScrPrint(0, 0, CFONT | REVER, "%*s", 16, "");
				}
			}
			ScrTextOut(ucXLocation, stTextboxWidget.y, ucTextBuff);
			xmlOffset += sizeof(ST_TEXTBOX_WIDGET);
			break;

		case PICTUREBOX:
			memset(ucTextBuff, 0, sizeof(ucTextBuff));
			memset(ucPicBuff, 0, sizeof(ucPicBuff));
			xmlOffset ++;
			memcpy(&stPictureWidget, buff +xmlOffset, sizeof(ST_PICTURE_WIDGET));
			memcpy(&stPictureWidget, buff +xmlOffset, sizeof(ST_PICTURE_WIDGET));
			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(stPictureWidget.name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(stPictureWidget.name))
					{
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(stPictureWidget.value))
					{
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucTextBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(ucTextBuff));
					if(ucTextBuff  !=  NULL)
					{
						memcpy(stPictureWidget.value, ucTextBuff, sizeof(ucTextBuff));
						memset(ucTextBuff, 0, sizeof(ucTextBuff));							
					}
					iMsgOffset++;	
				}
			}					

			iRet = BmpConvert(stPictureWidget.value, ucPicBuff, sizeof(ucPicBuff));
			if(iRet == 0)
			{
				ScrDrLogoxy(stPictureWidget.x, stPictureWidget.y, ucPicBuff);
			}
			xmlOffset += sizeof(ST_TEXTBOX_WIDGET);
			break;	

		case INPUTBOX:
			ScrCls();
			return EL_UI_RET_INVALID_PAGE;

		case MENUITEM:					
			memset(ucMenuItemBuff, 0, sizeof(ucMenuItemBuff));
			xmlOffset++;
			memcpy(&stMenuWidget, buff +xmlOffset, sizeof(ST_MENU_WIDGET));

			if(pucMsg[iMsgOffset].ucWidgetName != NULL)
			{
				if(strcmp(stMenuWidget.name, pucMsg[iMsgOffset].ucWidgetName) == 0)
				{
					if (strlen(pucMsg[iMsgOffset].ucWidgetName) > sizeof(stMenuWidget.name))
					{
						return EL_UI_RET_INVALID_WIDGETNAME;
					}
					if (strlen(pucMsg[iMsgOffset].ucWidgetValue) > sizeof(stMenuWidget.value))
					{
						return EL_UI_RET_INVALID_WIDGETVALUE;
					}
					memcpy(ucMenuItemBuff, pucMsg[iMsgOffset].ucWidgetValue, sizeof(stMenuWidget.value));
					if(ucMenuItemBuff  !=  NULL)
					{
						memcpy(stMenuWidget.value,ucMenuItemBuff, sizeof(ucMenuItemBuff));
						memset(ucMenuItemBuff, 0, sizeof(ucMenuItemBuff));							
					}

					GetSubStr(ucMenuItemBuff, stMenuWidget.value, stMenuWidget.fontsize);
					memcpy(pstMenuItem[ucMenuIndex].name, stMenuWidget.name, sizeof(stMenuWidget.name));
					memcpy(pstMenuItem[ucMenuIndex].value, ucMenuItemBuff, WIDGETVALUE_LENGTH_MAX);
					pstMenuItem[ucMenuIndex].textalign = stMenuWidget.textalign;
					pstMenuItem[ucMenuIndex].fontsize = stMenuWidget.fontsize;
					pstMenuItem[ucMenuIndex].x = stMenuWidget.x;
					pstMenuItem[ucMenuIndex].y = stMenuWidget.y;

					iMsgOffset++;	

					ucMenuIndex ++;
				}
			}

			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stMenuWidget.name=%s, stMenuWidget.value = %s, *piMenuItemNum = %d, ucMenuIndex = %d \n", stMenuWidget.name, stMenuWidget.value, *piMenuItemNum, ucMenuIndex);

			xmlOffset += sizeof(ST_MENU_WIDGET);
			break;	

		default:
			XmlTag = 0;
			break;
		}
	}

	*piMenuItemNum = ucMenuIndex;

	return 0;

}


int EShowMsgBox(const uchar *pucPageName,  const ST_DATA  *pucMsg, const ushort usTimeout)
{
	int   iRet = 0;
	uchar ucKeyAccept = BUTTON_NONE, ucKeyValue = 0;
	uchar buff[1024]={0};

	//	uchar widgetIndex0 = TEXTBOX;//for test
	//	ST_TEXTBOX_WIDGET stTextboxWidget0 = {"title", 0, 1, "init", 0, 2, 0, 1};//for test

	if(pucPageName == NULL || strlen(pucPageName) == 0)	// Added by ZhangYuan 2016-11-17, return error, if page name is null
	{
		ComPoseInfoMsg(RetToString, EL_UI_RET_INVALID_PAGE, EL_UI_RET_INVALID_PAGE , __LINE__);
		return EL_UI_RET_INVALID_PAGE;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucPageName=%s, pucMsg = %s\n", pucPageName, pucMsg==NULL ? "pucMsg == NULL" : pucMsg);	

	//show one messages
	//	memcpy(buff, &widgetIndex0, sizeof(uchar));
	//	memcpy(buff+sizeof(uchar), &stTextboxWidget0, sizeof(ST_TEXTBOX_WIDGET));

	iRet = ParseUi(pucPageName,buff,sizeof(buff));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ParseUi=%d, pucPageName = %s\n", iRet, pucPageName);	
	if(iRet != 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ParseUi=%d, pucPageName = %s\n", iRet, pucPageName);
		ComPoseInfoMsg(RetToString, EL_UI_RET_PARSEUI_FAILED, iRet , __LINE__);
		return EL_UI_RET_PARSEUI_FAILED;
	}

	TimerSet(TIMER_TEMPORARY,usTimeout);

	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "XML buff: ", buff, sizeof(buff));
	iRet = SetElement(buff, pucMsg, &ucKeyAccept);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "SetElement = %d\n", iRet);
	if(iRet != 0)
	{
		return iRet;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucKeyAccept = %d\n", ucKeyAccept);
	while(TimerCheck(TIMER_TEMPORARY))
	{
		switch(ucKeyAccept)
		{
		case BUTTON_NONE:
			break;

		case BUTTON_CANCEL:
			if(!kbhit())
			{
				ucKeyValue = getkey();
			}
			if(ucKeyValue == KEYCANCEL)
			{
				ScrCls();
				return RET_OK;
			}
			break;

		case BUTTON_ENTER:
			if(!kbhit())
			{
				ucKeyValue = getkey();
			}
			if(ucKeyValue == KEYENTER)
			{
				ScrCls();
				return RET_OK;
			}
			break;

		case BUTTON_ENTER_CANCEL:
			if(!kbhit())
			{
				ucKeyValue = getkey();
			}
			if(ucKeyValue == KEYCANCEL)
			{
				ScrCls();
				return RET_OK;
			}
			if(ucKeyValue == KEYENTER)
			{
				ScrCls();
				return RET_OK;
			}
			break;

		default:
			break;		
		}

	}

	if(usTimeout != 0 && !TimerCheck(TIMER_TEMPORARY))
	{
		ScrCls();
		ComPoseInfoMsg(RetToString, EL_UI_RET_TIME_OUT, EL_UI_RET_TIME_OUT , __LINE__);
		return EL_UI_RET_TIME_OUT;
	}
	else
	{
		return RET_OK;
	}
}

int EShowInputBox(const uchar *pucPageName, const ST_DATA  *pucMsg, uchar *pucResponse,const int iRspBuffLen, const ushort usTimeout)
{
	int   iRet = 0; 
	uchar  ucKeyAccept = BUTTON_NONE, ucKeyValue = 0;
	ST_INPUTBOX_WIDGET stInputboxWidget;
	uchar buff[1024]={0};

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucMsg=%s\n", pucMsg == NULL ? "NULL" : pucMsg);
	memset (&stInputboxWidget, 0, sizeof(ST_INPUTBOX_WIDGET));
	iRet = ParseUi(pucPageName,buff,sizeof(buff));
	if(iRet != 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ParseUi=%d\n", iRet);
		return EL_UI_RET_PARSEUI_FAILED;
	}

	iRet = SetInputboxElement(buff, pucMsg, &ucKeyAccept, &stInputboxWidget);
	if(iRet != 0)
	{
		return iRet;
	}
	iRet = InputProcess(&stInputboxWidget, pucResponse, iRspBuffLen, usTimeout);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "InputProcess = %d, pucResponse = %d, %s\n", iRet, pucResponse[0], &pucResponse[1]);
	if(iRet != 0)
	{
		return iRet;
	}
	else
	{
		return RET_OK;
	}
}

int EShowMenuList(const uchar *pucPageName, const ST_DATA  *pucMsg, int *piSelect, const ushort usTimeout)
{
	int  iRet = 0;
	uchar ucKeyAccept = BUTTON_NONE, ucKeyValue = 0;
	uchar buff[1024]={0};
	ST_MENU_WIDGET stMenuItem[32];
	int iMenuItemNum = 0;
	int iSelected = 0;

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "pucMsg=%s\n", pucMsg == NULL ? "NULL" : pucMsg);

	iRet = ParseUi(pucPageName, buff, sizeof(buff));
	if(iRet != 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ParseUi=%d\n", iRet);
		return EL_UI_RET_PARSEUI_FAILED;
	}

	memset(&stMenuItem, 0 , 32 * sizeof(ST_MENU_WIDGET));
	iRet = SetMenuElement(buff, pucMsg, &ucKeyAccept ,stMenuItem, &iMenuItemNum);
#ifdef UI_DEBUG_
	{
		int i=0;
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "iMenuItemNum=%d\n", iMenuItemNum);
		for(i=0; i<iMenuItemNum; i++)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stMenuItem[%d].name=%s, stMenuItem[%d].value=%s\n", i, stMenuItem[i].name, i, stMenuItem[i].value);
		}
	}
#endif
	if(iRet != 0)
	{
		return iRet;
	}
	if(iMenuItemNum == 0)
	{
		ScrCls();
		return EL_UI_RET_MENUITEMNUM_ERROR;
	}

	iSelected = MenuProcess(MIN(32, iMenuItemNum), stMenuItem, usTimeout);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "MenuProcess, iSelected=%d\n", iSelected);
	if(-2 == iSelected)
	{
		return EL_UI_RET_TIME_OUT;
	}
	else if(-3 == iSelected)
	{
		return EL_UI_RET_USER_CANCEL;
	}
	else
	{
		*piSelect = iSelected;
		return RET_OK;
	}
}

int ShowMsgBox(const uchar * pucRequest)
{
	int iRet = 0;
	uint uiReqOffset = 0;
	uint uiLength = 0;
	uchar ucPageName[PAGENAME_LENGTH_MAX]={0};
	uchar ucMsgBuff[1024]={0};
	ushort usTimeout = 0;
	uint uiDataLength = 0;
	uchar ucWidgetNum = WIDGET_NUM_MAX;
	ST_DATA stData[WIDGET_NUM_MAX];

	uchar ucRespond[32] = {0};//just for test EShowInputBox
	int iSelected = 0;//just for test EShowMenuList

	memset(ucPageName, 0, sizeof(ucPageName));
	memset(ucMsgBuff,  0, sizeof(ucMsgBuff));
	memset(stData, 0, 10*sizeof(ST_DATA));

	uiDataLength = (uint)pucRequest[uiReqOffset]<<24 | (uint)pucRequest[uiReqOffset+1]<<16 | (uint)pucRequest[uiReqOffset+2]<<8 |(uint)pucRequest[uiReqOffset+3];
	uiReqOffset += sizeof(uint);

	uiLength = pucRequest[uiReqOffset];
	uiReqOffset++;

	memcpy(ucPageName, pucRequest+ uiReqOffset, uiLength);
	uiReqOffset +=  uiLength;

	usTimeout =  (ushort)pucRequest[uiReqOffset] << 8 | (ushort)pucRequest[uiReqOffset + 1];
	uiReqOffset +=  sizeof(ushort);


	uiLength = uiDataLength - pucRequest[4] -1-sizeof(ushort);


	memcpy(ucMsgBuff, pucRequest+ uiReqOffset, uiLength);
	iRet = GetMsgData(ucMsgBuff, stData, WIDGET_NUM_MAX);
	if(iRet != 0)
	{
		return iRet;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiDataLength= %d\n", uiDataLength);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucPageName Length= %d\n", uiLength);	
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucPageName=%s\n", ucPageName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "usTimeout=%d\n", usTimeout);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucMsgBuff=%s\n", ucMsgBuff);
	EL_LOG_HEX(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "ucMsgBuff: ", ucMsgBuff,uiLength);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "uiLength=%d\n", uiLength);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "strlen(stData[0].ucWidgetName)=%d\n", strlen(stData[0].ucWidgetName));
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stData[0].ucWidgetName=%s\n", stData[0].ucWidgetName);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__, "stData[0].ucWidgetValue=%s\n", stData[0].ucWidgetValue);


	if(strlen(stData[0].ucWidgetName) > 0)
	{
		iRet = EShowMsgBox(ucPageName,  stData, usTimeout);
		//		iRet = EShowMenuList(ucPageName,  stData, &iSelected ,usTimeout);//for test menulist
		//		iRet = EShowInputBox(ucPageName,  stData, ucRespond, 32, usTimeout);//for test inputbox
	}
	else
	{
		iRet = EShowMsgBox(ucPageName, NULL, usTimeout);
		//		iRet = EShowMenuList(ucPageName, NULL, &iSelected, usTimeout);//for test menulist
		//		iRet = EShowInputBox(ucPageName, NULL, ucRespond, 32, usTimeout);//for test inputbox
	}

	if(iRet != 0)
	{
		return iRet;
	}

	return 0;
}

