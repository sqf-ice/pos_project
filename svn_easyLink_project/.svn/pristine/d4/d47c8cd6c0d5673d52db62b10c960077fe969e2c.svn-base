/*
 * ============================================================================
 * COPYRIGHT
 *              Pax CORPORATION PROPRIETARY INFORMATION
 *   This software is supplied under the terms of a license agreement or
 *   nondisclosure agreement with Pax Corporation and may not be copied
 *   or disclosed except in accordance with the terms in that agreement.
 *      Copyright (C) 2016 Pax Corporation. All rights reserved.
 * Module Date:2016/08/09     
 * Module Auth:huangwp     
 * Description:xmlparse module

 *      
 * ============================================================================
 */
#include <stdio.h>
#include <String.h>
#include "..\inc\xmlparse.h"
#include "..\inc\debug.h"

#define   LOG_TAG   "XML_PARSE"
#ifdef XML_DEBUG		

#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine,, pucFmtStr,...)		\
	El_Log(enLogPrio, pucTag, pucFunc, iLine,, pucFmtStr, ##__VA_ARGS__)

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine,, pucMsg, pucHexData, iHexDataLen)	\
	El_Log_Hex(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)

#else

#define EL_LOG_HEX(enLogPrio, pucTag, pucFunc, iLine, pucMsg, pucHexData, iHexDataLen)
#define EL_LOG(enLogPrio, pucTag, pucFunc, iLine, pucFmtStr, ...)


#endif // XML_DEBUG


int IsXmlLineHaveAttr(const uchar *pszXmlOneLine )
{
	uchar *pszEndTag = NULL;
	uchar *pszStart = NULL;
	uchar *pszEnd = NULL;
	uchar *Attr = NULL;

	if(pszXmlOneLine == NULL)
	{
		return XML_ERR_INVALID_PARAM;
	}
	
	pszEndTag = strstr(pszXmlOneLine, "</");
	pszStart = strchr(pszXmlOneLine, '<');
    pszEnd =  strchr(pszXmlOneLine, '>');

	if(pszEndTag == NULL || pszStart == NULL || pszEnd == NULL)
	{
		return XML_ERR_FROMAT_ERR;
	}

	//it is a end Node
	if(pszStart == pszEndTag)
	{
	    return XML_NODE_END;	
	}
	Attr = strchr(pszXmlOneLine, ' ');
	
	if(Attr != NULL)
	{
        return XML_NODE_HAVE_ATTR;
	}
	else
	{
		return XML_NODE_NO_ATTR;
	}
	
}


static int ParseXml(const uchar *pszXmlOneLine, XmlNode *Xml, uchar *pszOutValue)
{
     uchar *pszStart = NULL;
	 uchar *pszEnd = NULL;
	 int iRet = -1;

	if(pszXmlOneLine == NULL || Xml == NULL)

	{
		return XML_ERR_INVALID_PARAM;
	}
 
	 pszStart = strchr(pszXmlOneLine, '<');
     pszEnd =  strchr(pszXmlOneLine, '>');
	 if(Xml->GetAttrFunc != NULL)
	 {   

		  EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"\n");
		  //memset(AttrValue, 0, sizeof(AttrValue));
		  iRet = Xml->GetAttrFunc(pszXmlOneLine, Xml->AttrName, pszOutValue);
		  if(iRet != XML_PARSE_OK)
		  {
		      return iRet;
		  }

		  EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"Attr name = %s   Attr valude = %s\n", Xml->AttrName, pucOutValue);
	 }
	 else
	 {
		  //memset(NodeValue, 0, sizeof(NodeValue));
		  iRet = Xml->GetValueFunc(pszXmlOneLine, Xml->NodeName, pszOutValue);
		  if(iRet != XML_PARSE_OK)
		  {
		      return iRet;
		  }
		  EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"Node Name = %s  NodeValue valude = %s\n", Xml->NodeName, pucOutValue);
	 }
	 
	 return XML_PARSE_OK;
	 	
}


static int GetXmlAttrValue(const uchar *pszXmlOneLine, const uchar *pszAttrName, uchar *pszOutValue)
{
	uchar *pszStart = NULL;
    uchar *pszEndOne = NULL;
    uchar *pszEndTwo = NULL;
    uchar *pszEnd = NULL;
    int iLen = 0;
    uchar ucTemp[128] = {0};

    if(pszXmlOneLine == NULL || pszAttrName == NULL)
    {
	    return XML_ERR_INVALID_PARAM;
    }
   
    strcat(ucTemp,pszAttrName);
    strcat(ucTemp, "=\"");
   	
    pszStart =  strstr(pszXmlOneLine,ucTemp);
    if(pszStart == NULL)
    {
        EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetXmlAttrValue----no this attr in this line\n");
	    return XML_ERR_ATTR_NULL;
	   
    }
    EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetXmlAttrValue---start = %s\n",pucStart);
   
    iLen = strlen(pszAttrName);
   
    pszEndOne = strstr(pszStart, "\" ");
    if(pszEndOne == NULL)
    {   
        EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetXmlAttrValue----EndOne is NULL  can not find space\n");
	    pszEndTwo = strchr(pszStart, '>');
	    if(pszEndTwo == NULL)
        {
      	    EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"GetXmlAttrValue----GetXmlAttrValue----ENDtwo in NULL  format error\n");
		    return XML_ERR_FROMAT_ERR;
   	    }
  	    else
  	    {
  	        EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"EndTwo=%s\n",pucEndTwo);
		    pszEnd = pszEndTwo;
   	    }
    }
    else
    {
        EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucEndOne=%s\n",pucEndOne);
	    pszEnd = pszEndOne + 1;
    }
	
    memcpy(pszOutValue,pszStart+iLen+2,pszEnd-pszStart-iLen-3);

    EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucOutValue=%s\n",pucOutValue);
    return XML_PARSE_OK;
}


static int GetXmlNodeValue(const uchar *pszXmlOneLine, const uchar *pszNodeName, uchar *pszOutValue)
{
	uchar *pszStart = NULL;
	uchar *pszEnd = NULL;

    if(pszXmlOneLine == NULL || pszNodeName == NULL)
	{
		return XML_ERR_INVALID_PARAM;
	}

    if(strstr(pszXmlOneLine, pszNodeName) == NULL)
    {
       EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"no this node in this line\n");
	   return XML_ERR_NODE_NOT_FOUND;
    }
	
	pszStart = strchr(pszXmlOneLine, '>');
	if(pszStart == NULL)
	{
	    EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucStart is NULL\n");
		return XML_ERR_FROMAT_ERR;
	}
	
	pszEnd = strstr(pszXmlOneLine, "</");
	if(pszEnd == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucEnd is NULL\n");
		return XML_ERR_FROMAT_ERR;
	}
	
	memcpy(pszOutValue, pszStart+1, pszEnd-pszStart-1);

    EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucOutValue = %s \n",pucOutValue);
	return XML_PARSE_OK;

}

int XmlParseGetNodeValue(const uchar *pszXmlOneLine, uchar *pszNodeName, uchar *pszOutValue)
{
	XmlNode Xml = {0};
    int iLen = 0;
	int iRet = 0;

	if(pszXmlOneLine == NULL || pszNodeName == NULL)
	{
		return XML_ERR_INVALID_PARAM;
	}

	iLen = strlen(pszXmlOneLine);
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"strlen(pucXmlOneLine) Len = %d \n",iLen);

	strcpy(Xml.NodeName, pszNodeName);
	Xml.GetValueFunc = GetXmlNodeValue;
	Xml.GetAttrFunc = NULL;
    iRet = ParseXml(pszXmlOneLine, &Xml, pszOutValue);
	if(iRet != XML_PARSE_OK)
	{
		return iRet;
	}
	return XML_PARSE_OK;
}


int XmlParseGetAttrValue(const uchar *pszXmlOneLine, uchar *pszAttrnem, uchar *pszOutValue)
{
	//uchar ucBuffer[512] = {0};
	XmlNode Xml;
	int iRet = 0;

	if(pszXmlOneLine == NULL || pszAttrnem == NULL)
	{
		return XML_ERR_INVALID_PARAM;
	}
	
    memset(&Xml, 0, sizeof(Xml));
	//strcpy(ucBuffer, pszXmlOneLine);
	strcpy(Xml.AttrName, pszAttrnem);
	Xml.GetAttrFunc = GetXmlAttrValue;
	Xml.GetValueFunc = NULL;
	iRet = ParseXml(pszXmlOneLine, &Xml, pszOutValue);
	if(iRet != XML_PARSE_OK)
	{
		return iRet;
	}
	
	return XML_PARSE_OK;
}

int XmlGetOneLineData(const uchar *pszBufIn, int *OutLen, char *pszOutOneLineData)
{
    uchar *pucFirstTag = NULL;
	int Length = 0;

	if(NULL == pszBufIn)
	{
	    return XML_ERR_INVALID_PARAM;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucBufIn = %s \n",pucBufIn);
	//pucFirstTag = strstr(pszBufIn, "\r\n");
	pucFirstTag = strchr(pszBufIn, '\n');
	if(pucFirstTag == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"XML_LINE_END_TAG_NULL\n");
		return XML_LINE_END_TAG_NULL;
	}
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucFirstTag = %s\n",pucFirstTag);
	
	Length = pucFirstTag - pszBufIn;

	//'\0x0a' len = 1, so add 1;
	//*OutLen = Length + 2;
	*OutLen = Length + 1;
	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"Lenth = %d\n",Length);

	memcpy(pszOutOneLineData,pszBufIn, Length);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucOutOneLineData = %s\n",pucOutOneLineData);
	return 0;
	
}


int GetUcharData(const uchar *pszXmlOneLine, const uchar *NodeName, uchar *pszOutData)
{
    int iRet = 0;
	uchar aucBuf[XML_VALUE_BUF] = {0};

	if(pszXmlOneLine == NULL || NodeName == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucXmlOneLine == NULL || NodeName == NULL\n");
		return -1;
	}

	memset(aucBuf, 0, sizeof(aucBuf));
	iRet = XmlParseGetNodeValue(pszXmlOneLine, NodeName, aucBuf);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n", iRet);
		return iRet;
	}
	*pszOutData = (uchar)atoi(aucBuf);
	return iRet;
}


int GetBcdData(const uchar *pszXmlOneLine,const uchar *NodeName, uchar *pszOutData)
{
    int iRet = 0;
	uchar aucBuf[XML_VALUE_BUF] = {0};

	if(pszXmlOneLine == NULL || NodeName == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucXmlOneLine == NULL || NodeName == NULL\n");
		return -1;
	}

	memset(aucBuf, 0, sizeof(aucBuf));
	iRet = XmlParseGetNodeValue(pszXmlOneLine, NodeName, aucBuf);

	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n", iRet);
		return iRet;
	}

	iRet = (uchar)strlen((const char *)aucBuf) / 2;
	PubAsc2Bcd(aucBuf, strlen((const char *)aucBuf), pszOutData);

	return iRet;
}

int GetIntegerData(const uchar *pszXmlOneLine, const uchar *NodeName, int *piData)
{
    int iRet = 0;
	uchar aucBuf[XML_VALUE_BUF] = {0};

	if(pszXmlOneLine == NULL || NodeName == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucXmlOneLine == NULL || NodeName == NULL\n");
		return -1;
	}

	memset(aucBuf, 0, sizeof(aucBuf));
	iRet = XmlParseGetNodeValue(pszXmlOneLine, NodeName, aucBuf);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n", iRet);
		return iRet;
	}
	*piData = atoi(aucBuf);
	return iRet;
}

int GetStringData(const uchar *pszXmlOneLine, const uchar *NodeName, uchar *pszOutData)
{
    int iRet = 0;
	uchar aucBuf[XML_VALUE_BUF] = {0};

	if(pszXmlOneLine == NULL || NodeName == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucXmlOneLine == NULL || NodeName == NULL\n");
		return -1;
	}

	memset(aucBuf, 0, sizeof(aucBuf));
	iRet = XmlParseGetNodeValue(pszXmlOneLine, NodeName, aucBuf);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n", iRet);
		return iRet;
	}

	strcpy(pszOutData, aucBuf);

	return iRet;
}

int GetUlongData(const uchar *pszXmlOneLine, const uchar *NodeName, ulong *pulData)
{
    int iRet = 0;
	uchar aucBuf[XML_VALUE_BUF] = {0};

	if(pszXmlOneLine == NULL || NodeName == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pucXmlOneLine == NULL || NodeName == NULL\n");
		return -1;
	}

	memset(aucBuf, 0, sizeof(aucBuf));
	iRet = XmlParseGetNodeValue(pszXmlOneLine, NodeName, aucBuf);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n", iRet);
		return iRet;
	}

	PubAsc2Long(aucBuf, strlen((const char *)aucBuf), pulData);
	return iRet;
}


// Added by ZhangYuan 2016-12-23
void xmlGetElementAmt(unsigned char *pszStrAmt, unsigned char *pszBcdAmt, unsigned int uiBcdAmtBufLen)
{
	int iTmp=0, iStrLen=0;
	unsigned char aucBuf[512+1]={0};

	iTmp = uiBcdAmtBufLen * 2;
	if(pszStrAmt == NULL || strlen(pszStrAmt) == 0 || (iTmp > 512))
	{
		return;
	}

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"[%s]-[%s]-[%d]:pszStrAmt=%s, uiBcdAmtBufLen=%d\n",__FILE__,__FUNCTION__,__LINE__,pszStrAmt, uiBcdAmtBufLen);

	memset(aucBuf, '0', iTmp);
	iStrLen = strlen(pszStrAmt);
	memcpy(aucBuf+iTmp-iStrLen, pszStrAmt, iStrLen);
	PubAsc2Bcd(aucBuf, iTmp, pszBcdAmt);

	EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pszStrAmt=%s\n",__FILE__,__FUNCTION__,__LINE__,pszStrAmt);
	EL_LOG_HEX(LOG_DEBUG,  LOG_TAG, __FUNCTION__, __LINE__,"[xmlGetELementAmt] pszBcdAmt: ", pszBcdAmt, uiBcdAmtBufLen);

}


int GetAmtData(const uchar *pszXmlOneLine, const uchar *NodeName, uchar *pszOutData, unsigned int uiDataBufSize)
{
    int iRet = 0;
	uchar aucBuf[XML_VALUE_BUF] = {0};

	if(pszXmlOneLine == NULL || NodeName == NULL)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"pszXmlOneLine == NULL || NodeName == NULL\n",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}

	memset(aucBuf, 0, sizeof(aucBuf));
	iRet = XmlParseGetNodeValue(pszXmlOneLine, NodeName, aucBuf);
	if(iRet < 0)
	{
		EL_LOG(LOG_DEBUG, LOG_TAG, __FUNCTION__, __LINE__,"iRet = %d\n",__FILE__,__FUNCTION__,__LINE__, iRet);
		return iRet;
	}

	xmlGetElementAmt(aucBuf, pszOutData, uiDataBufSize);

	return iRet;
}
 

