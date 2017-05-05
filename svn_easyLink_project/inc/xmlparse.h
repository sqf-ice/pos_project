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


#ifndef _XMLPARSE_H
#define _XMLPARSE_H
#include "../inc/constant.h"

#define XML_TAG_START	0x00
#define XML_TAG_END		0x01


#define XML_ERR_BASE     			   -3000
#define XML_ERR_INVALID_PARAM          (XML_ERR_BASE-1)
#define XML_ERR_FROMAT_ERR             (XML_ERR_BASE-2)
#define XML_ERR_NODE_NOT_FOUND         (XML_ERR_BASE-3)
#define XML_ERR_ATTR_NULL        	   (XML_ERR_BASE-4)


#define XML_PARSE_OK                   0


#define XML_NODE_NO_ATTR          1
#define XML_NODE_END              2    //</Node>
#define XML_NODE_HAVE_ATTR        3

#define XML_RETURNCODE_BASE     		 100
#define XML_LINE_END_TAG_NULL            (XML_RETURNCODE_BASE+1)

#define XML_READ_TEMP_BUF_SIZE            1536
#define XML_READ_BUF_SIZE            1024
#define XML_XML_FILE_LINE_BUF_SIZE    512
#define XML_VALUE_BUF                1024


#define XML_NODE_NAME_LEN             64


typedef int (GetAttrValue)(const uchar *pszXmlOneLine, const uchar *AttrName, uchar *OutValue);
typedef int (GetNodeValue)(const uchar *pszXmlOneLine, const uchar *NodeName, uchar *OutValue);
typedef int (*PARSE_CB)(const uchar *pucXmlOneLine, void *Data);

typedef struct NodeFuncTab
{
	uchar ucNodeName[32];
	PARSE_CB vFunc;
	void *pOutData;
	int iLen;
}NODE_FUNC_TAB_T;

typedef struct Record
{
	uchar ucListNodeName[32];
	int count;
	int iStartPos;
}NODE_RECORD_T;


typedef struct XmlNode
{
    uchar ParentNode[32];
    uchar NodeName[32];
	uchar AttrName[32];
	GetAttrValue *GetAttrFunc;
	GetNodeValue *GetValueFunc;

}XmlNode;

typedef struct TextBox
{
    uint NodeIndex;
	uchar AttrName[32];
}stTextBox;



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int XmlParseGetAttrValue(const uchar *pszXmlOneLine, uchar *pucAttrName, uchar *pucOutValue);
int XmlParseGetNodeValue(const uchar *pszXmlOneLine, uchar *pucNodeName, uchar *pucOutValue);
int XmlGetOneLineData(const uchar *pucBufIn, int *OutLen, char *pucOutOneLineData);
int IsXmlLineHaveAttr(const uchar *pszXmlOneLine);

int GetUcharData(const uchar *pszXmlOneLine, const uchar *NodeName, uchar *pszOutData);
int GetBcdData(const uchar *pszXmlOneLine,const uchar *NodeName, uchar *pszOutData);
int GetIntegerData(const uchar *pszXmlOneLine, const uchar *NodeName, int *piData);
int GetStringData(const uchar *pszXmlOneLine, const uchar *NodeName, uchar *pszOutData);
int GetUlongData(const uchar *pszXmlOneLine, const uchar *NodeName, ulong *pulData);
int GetAmtData(const uchar *pszXmlOneLine, const uchar *NodeName, uchar *pszOutData, unsigned int uiDataBufSize);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#pragma pack()


#endif

