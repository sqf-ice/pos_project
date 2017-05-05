/*
 * ============================================================================
 * COPYRIGHT
 *              Pax CORPORATION PROPRIETARY INFORMATION
 *   This software is supplied under the terms of a license agreement or
 *   nondisclosure agreement with Pax Corporation and may not be copied
 *   or disclosed except in accordance with the terms in that agreement.
 *      Copyright (C) 2016 Pax Corporation. All rights reserved.
 * Module Date:2016/09/21    
 * Module Auth:huangwp     
 * Description:UI xml file operarte module

 *      
 * ============================================================================
 */

#include "..\inc\global.h"

#define LOG_TAG	__FILE__
#ifdef UI_XML_FILE_OP_DEBUG		

#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr,...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)


#endif // TRANS_DEBUG_		


UI_PARSE_FUNC pUiParseAttrFunTable[]=
{

	{TEXTBOX, ParseTextBox},
	{PICTUREBOX, ParsePictureBox},
	{INPUTBOX, ParseInputBox},
	{MENUITEM, ParseMenu},		
};

int ParseTextBox(const uchar *pucXmlOneLine, uchar *pucOutData, int iOffset)
{
    int iRet = 0;
	ST_TEXTBOX_WIDGET st_TextBoxData = {0};
	uchar pucBuf[UI_ATTR_VALUE_LEN] = {0};

	pucOutData[iOffset] = TEXTBOX;

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_NAME, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_TextBoxData.name, pucBuf, sizeof(st_TextBoxData.name));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_X, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_TextBoxData.x = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_Y, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_TextBoxData.y = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_VALUE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_TextBoxData.value, pucBuf, sizeof(st_TextBoxData.value));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_FONTSIZE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_TextBoxData.fontsize= (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_TEXTALIGN, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_TextBoxData.textalign= (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_MODE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_TextBoxData.mode= (uchar)atoi(pucBuf);
	//PubAsc2Bcd(pucBuf + 2, strlen((const char *)pucBuf) - 2, &(st_TextBoxData.mode));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_KEYACCECPT, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_TextBoxData.keyaccept= (uchar)atoi(pucBuf);


	memcpy(pucOutData + iOffset + 1,&st_TextBoxData, sizeof(st_TextBoxData));
	

	return (sizeof(st_TextBoxData) + sizeof(uchar));
	//return UI_XML_FILE_OP_OK;
}
	
int ParseInputBox(const uchar *pucXmlOneLine, uchar *pucOutData, int iOffset)
{
    int iRet = 0;
	uchar pucBuf[UI_ATTR_VALUE_LEN] = {0};

   	ST_INPUTBOX_WIDGET st_InputBoxData = {0};

	pucOutData[iOffset] = INPUTBOX;

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_NAME, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_InputBoxData.name, pucBuf, sizeof(st_InputBoxData.name));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_X, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.x = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_Y, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.y = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_VALUE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_InputBoxData.value, pucBuf, sizeof(st_InputBoxData.value));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_FONTSIZE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.fontsize= (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_TEXTALIGN, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.textalign= (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_MODE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.mode= (uchar)atoi(pucBuf);
	//PubAsc2Bcd(pucBuf + 2, strlen((const char *)pucBuf) - 2, &(st_InputBoxData.mode));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_TYPE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.type= (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_KEYACCECPT, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.keyaccept= (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_MINLEN, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.minlen= (uchar)atoi(pucBuf);

	
	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_MAXLEN, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_InputBoxData.maxlen= (uchar)atoi(pucBuf);

	memcpy(pucOutData + iOffset + 1,&st_InputBoxData, sizeof(st_InputBoxData));

	//return UI_XML_FILE_OP_OK;
	return (sizeof(st_InputBoxData) + sizeof(uchar));
}

int ParsePictureBox(const uchar *pucXmlOneLine, uchar *pucOutData, int iOffset)
{

    int iRet = 0;
	uchar pucBuf[UI_PICTURE_BOX_VALUE_LEN] = {0};

	ST_PICTURE_WIDGET st_PictureBoxData = {0};

	pucOutData[iOffset] = PICTUREBOX;

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_NAME, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_PictureBoxData.name, pucBuf, sizeof(st_PictureBoxData.name));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_X, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_PictureBoxData.x = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_Y, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_PictureBoxData.y = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_VALUE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_PictureBoxData.value, pucBuf, sizeof(st_PictureBoxData.value));

	memcpy(pucOutData + iOffset + 1,&st_PictureBoxData, sizeof(st_PictureBoxData));
	
	return (sizeof(ST_PICTURE_WIDGET) + sizeof(uchar));
	 
}

int ParseMenu(const uchar *pucXmlOneLine, uchar *pucOutData, int iOffset)
{
    int iRet = 0;
	uchar pucBuf[UI_ATTR_VALUE_LEN] = {0};
	ST_MENU_WIDGET st_MenuData = {0};

	pucOutData[iOffset] = MENUITEM;

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_NAME, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_MenuData.name, pucBuf, sizeof(st_MenuData.name));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_X, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_MenuData.x = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_Y, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_MenuData.y = (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_VALUE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	memcpy(st_MenuData.value, pucBuf, sizeof(st_MenuData.value));

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_FONTSIZE, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_MenuData.fontsize= (uchar)atoi(pucBuf);

	memset(pucBuf, 0, sizeof(pucBuf));
	iRet = XmlParseGetAttrValue(pucXmlOneLine, UI_PAGE_ATTRA_TEXTALIGN, pucBuf);
	if(iRet < 0)
	{
		return iRet;
	}
	st_MenuData.textalign= (uchar)atoi(pucBuf);
	
	memcpy(pucOutData + iOffset + 1,&st_MenuData, sizeof(st_MenuData));

	return (sizeof(ST_MENU_WIDGET) + sizeof(uchar));

}


static int GetUiBoxType(uchar *pucXmlOneLine)
{
	if(pucXmlOneLine == NULL)
	{
		return UI_XML_FILE_OP_ERR_INVALID_PARAM;
	}
	if(strstr(pucXmlOneLine, UI_TEXT_BOX) != NULL)
	{
		return TEXTBOX;
	}
	if(strstr(pucXmlOneLine, UI_PICTURE_BOX) != NULL)
	{
		return PICTUREBOX;
	}
	if(strstr(pucXmlOneLine, UI_INPUT_BOX) != NULL)
	{
		return INPUTBOX;
	}
	if(strstr(pucXmlOneLine, UI_MENU_ITEM_BOX) != NULL)
	{
		return MENUITEM;
	}
	else
	{
		return UI_XML_FILE_OP_ERR_INVALID_PARAM;
	}

}

static int ParseUiAttr(uchar *pucXmlOneLine, uchar *pucOutData, int iOffset)
{
    int iUiBoxType = 0;
	int iRet = 0;
	int iSize = 0;
	int i = 0;

	if(pucXmlOneLine == NULL)
	{
		return UI_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	iUiBoxType = GetUiBoxType(pucXmlOneLine);
	iSize = sizeof(pUiParseAttrFunTable);
	
	
	if(iUiBoxType < 0)
	{
		return UI_XML_FILE_OP_ERR_INVALID_PARAM;
	}

	for(i = 0; i < iSize; i++)
	{
	    if(pUiParseAttrFunTable[i].iType == iUiBoxType)
	    {
			iRet  = pUiParseAttrFunTable[i].vFunc(pucXmlOneLine, pucOutData, iOffset);
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d i = %d\n",iRet, i);
				return iRet;
			}
	   	}
	}

	return iRet;
}

static int FindPagePositonInFile(const uchar *pucFileName, const uchar *pucPageName, int iPos)
{
	int iFd = 0;
	int iFileSize = 0;
	int iSearchLen = 0;
	uchar ucReadBuf[EMV_FILE_READ_BUF_LEN + 1] = {0};
	uchar ucTempBuf[EMV_TEMP_BUF_LEN + 1] = {0};
	uchar ucParseBuf[EMV_FILE_READ_BUF_LEN + 1] = {0};
	uchar *pucStart  = NULL;
	uchar *pucLastEnterPos = NULL;
	int iRet = 0;
	int iOffset = 0;
	ulong uiPosition = 0;
	int iLseek = 0;

	
	iRet = PdkFile_Exist (pucFileName);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
		return iRet;
	}

	iFd = PdkFile_Open(pucFileName);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(pucFileName);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = PdkFile_Seek (iFd, iPos, SEEK_CUR);
	//ComDebugTx("FindNodePositonInFile PdkFile_Seek iPos PdkFile_Seek = %d\n", iLseek);
	if(iLseek < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iLseek = %d\n",iLseek);
		return iLseek;
	}

	while(iSearchLen < iFileSize)
	{
		memset(ucReadBuf, 0, sizeof(ucReadBuf));
		iRet = PdkFile_Read(iFd, ucReadBuf, 1024);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			return iRet;
		}

		//every time ,we should confirm the last line is whole line
		pucLastEnterPos = strrchr(ucReadBuf,'\x0a');
		if(pucLastEnterPos == NULL)
		{
			PdkFile_Close(iFd);
			return EMV_XML_FILE_OP_ERR_ONELINE_OVER_1024;
		}
		
		memcpy(ucParseBuf, ucReadBuf, pucLastEnterPos - ucReadBuf);	
		pucStart = strstr(ucParseBuf, pucPageName);
		if(pucStart == NULL)
		{
		    //the len have search
			iOffset = 1024 - (pucLastEnterPos - ucReadBuf);
			iSearchLen = iSearchLen + (pucLastEnterPos - ucReadBuf);
			iLseek = PdkFile_Seek(iFd, -iOffset, SEEK_CUR);
			if(iLseek < 0)
			{
				PdkFile_Close(iFd);
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iLseek = %d\n",iLseek);
				return iLseek;
			}
			continue;
		}
		else
		{
			uiPosition = iSearchLen + (pucStart - ucParseBuf);
			PdkFile_Close(iFd);
			return uiPosition;
		}
	}
	PdkFile_Close(iFd);
	return EMV_XML_FILE_OP_ERR_NODE_NOT_EXIST;
}

int ParseUi(const uchar *pucPageName, uchar *pucOutData, int iOutBufSize)
{
	int iRet = 0;
	int iListPos = 0;
	int iFd = 0;
	int iFileSize = 0;
	int iLseek = 0;
	uchar aucReadBuf[UI_READ_BUF_SIZE + 1] = {0};
	uchar aucParseBuf[UI_READ_BUF_SIZE + 1] = {0};
	uchar aucXmlLineBuf[UI_READ_BUF_SIZE] = {0};
	uchar ucPageParentNodeLine[UI_XML_FILE_LINE_BUF_SIZE] = {0};
	uchar *pucLastEnterPos = NULL;
	uchar *pucEndTag = NULL;
	int iLen = 0;
	int iHaveGetLen = 0;
	int iOffset = 0;
	int iReadLen = 0;
	int iHavaParseLen = 0;

	if(pucPageName != NULL)
	{
		strcat(ucPageParentNodeLine, UI_PAGE_ROOT_NODE_NAME);
		strcat(ucPageParentNodeLine, "\"");
		strcat(ucPageParentNodeLine, pucPageName);
		strcat(ucPageParentNodeLine, "\">");
		
		iListPos = FindPagePositonInFile(UI_FILE_NAME, ucPageParentNodeLine, 0);
		if(iListPos < 0)
		{
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iListPos = %d\n",iListPos);
			return iListPos;
		}
	}
	else
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucPageName == UI_XML_FILE_OP_ERR_PAGE_IS_NULL\n");
		return UI_XML_FILE_OP_ERR_PAGE_IS_NULL;	
		
	}
	
	iFd = PdkFile_Open(UI_FILE_NAME);
	if(iFd < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFd = %d\n",iFd);
		return iFd;
	}

	iFileSize = PdkFile_GetSize(UI_FILE_NAME);
	if(iFileSize < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iFileSize = %d\n",iFileSize);
		return iFileSize;
	}

	iLseek = PdkFile_Seek (iFd, iListPos, SEEK_CUR);
	if(iLseek < 0)
	{
		PdkFile_Close(iFd);
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iLseek = %d\n",iLseek);
		return iFileSize;
	}

	while((iReadLen + iListPos) < iFileSize)
	{
		memset(aucReadBuf, 0, sizeof(aucReadBuf));
		iRet = PdkFile_Read(iFd, aucReadBuf, 1024);
		if(iRet < 0)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet);
			return iRet;
		}
		iReadLen +=  iRet;

		//every time ,we should confirm the last line is whole line
		pucLastEnterPos = strrchr(aucReadBuf,'\x0a');
		if(pucLastEnterPos == NULL)
		{
			PdkFile_Close(iFd);
			EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucLastEnterPos == NULL\n");
			return EMV_XML_FILE_OP_ERR_ONELINE_OVER_1024;
		}
		memset(aucParseBuf, 0, sizeof(aucParseBuf));
		memcpy(aucParseBuf, aucReadBuf, pucLastEnterPos - aucReadBuf + 1);
			
		while((iHaveGetLen) < (pucLastEnterPos - aucReadBuf + 1))
		{
			memset(aucXmlLineBuf, 0, sizeof(aucXmlLineBuf));
			//offset mean hava get len,the parsebuf's data every line is whole line   include \r\n
			iRet = XmlGetOneLineData(aucParseBuf + iHaveGetLen, &iLen, aucXmlLineBuf);
			if(iRet < 0)
			{
				EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet)
				PdkFile_Close (iFd);
				return iRet;
			}
			iHaveGetLen += iLen;

			//init the buf that use to save data
			if(strstr(aucXmlLineBuf, ucPageParentNodeLine) != NULL)
			{
				memset(pucOutData , 0, iOutBufSize);
				//continue;
			}
			
			pucEndTag = strstr(aucXmlLineBuf, "</Page>");
			if(pucEndTag != NULL)
			{
				PdkFile_Close(iFd);
				return XML_PARSE_OK;
			}

			iRet = IsXmlLineHaveAttr(aucXmlLineBuf);
			
			if(iRet == XML_NODE_NO_ATTR)
			{
				continue;
			}
			else if(iRet == XML_NODE_END)
			{
				break;
			}
			else if(iRet == XML_NODE_HAVE_ATTR)
			{
			    iRet = ParseUiAttr(aucXmlLineBuf, pucOutData, iHavaParseLen);
				if(iRet < 0)
				{
					PdkFile_Close (iFd);
					return iRet;
				}
				iHavaParseLen = iRet + iHavaParseLen;

				if(iHavaParseLen > iOutBufSize)
				{
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet)
					return UI_XML_FILE_OP_ERR_BUF_IS_FULL;
				}
			}		
           

			//have finish one parsebuf, reset data  //dui de wei zhi
			if(iHaveGetLen == (pucLastEnterPos - aucReadBuf + 1))
			{
				iRet = PdkFile_Seek (iFd, iHaveGetLen - 1024, SEEK_CUR);
				if(iRet < 0)
				{
					EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",iRet)
					return iRet;
				}
				iHaveGetLen = 0;
				break;
			}
		}
		//parse one aid
	}
	PdkFile_Close(iFd);
	return XML_PARSE_OK;
}


