#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "LHBXML.h"

#define false 0
#define true 1

const xml_t XML_NULL = { (unsigned char)NULL, (unsigned char)NULL, (unsigned char)NULL, (unsigned char)NULL };
static unsigned char sg_xmlValBuf[100];

void XmlGetChild(const xml_t *fatherXML, const unsigned char *name, unsigned int childXMLIndex, xml_t *childXML)
{
	unsigned int i = 0;
	unsigned char buffer[100];
	sprintf(buffer,"<%s", name);

	while(1)
	{
		if(0 == i)
		{	
			childXML->start = strstr(fatherXML->start, buffer);
			if(!childXML->start || childXML->start > fatherXML->end)
			{
				memcpy(childXML, &XML_NULL, sizeof(xml_t));
				return;
			}
			if(isalnum(*(childXML->start + strlen(buffer))))
				childXML->start = strstr(childXML->start + strlen(buffer), buffer);
		}
		else
		{
			sprintf(buffer,"<%s", name);
			childXML->start = strstr(childXML->end, buffer);
		}

		if(!childXML->start || childXML->start > fatherXML->end)
		{
			memcpy(childXML, &XML_NULL, sizeof(xml_t));
			return;
		}
		childXML->end = strchr(childXML->start, '>');
		if (childXML->end)
		{
			if(childXML->end > fatherXML->end)
			{	
				memcpy(childXML, &XML_NULL, sizeof(xml_t));
				//childXML = (xml_t *)&XML_NULL;
				return;
			}
			if('/' == *(childXML->end - 1))
				++childXML->end;
			else
			{
				sprintf(buffer, "</%s>", name);
				childXML->end = strstr(childXML->start, buffer) + strlen(buffer);
				if(!childXML->end)
				{
					memcpy(childXML, &XML_NULL, sizeof(xml_t));
					//childXML = (xml_t *)&XML_NULL;
					return;
				}
			}
		}
		else
		{
			memcpy(childXML, &XML_NULL, sizeof(xml_t));
			//childXML = (xml_t *)&XML_NULL;
			return;
		}

		strcpy(childXML->name, name);
		childXML->father = (xml_t *)fatherXML;
		if(childXMLIndex == i)
			return;
		++i;
	}
	return;
}

const unsigned char* XmlGetValue(const xml_t *xml, const unsigned char *Attr)
{
	unsigned char *start, *end;
	start = strstr(xml->start, Attr);
	end = strchr(xml->start, '>');

	while(1)
	{
		if(!start || start + strlen(Attr) > end)
			return "";

		if (isalnum(*(start - 1)) || isalnum(*(start + strlen(Attr) + 1)))
		{
			start = strstr(start + strlen(Attr), Attr);
			if(!start || start + strlen(Attr) > end)
				return "";
		}
		else
			break;
	}
	
	start += strlen(Attr);
	start = strchr(start, '=') + 1;

	start = strchr(start, '\"') + 1;
	end = strchr(start, '\"');
	memset(sg_xmlValBuf, 0, sizeof(sg_xmlValBuf));
	if(start < end)
		memcpy(sg_xmlValBuf, start, end - start);
	else
		strcpy(sg_xmlValBuf, "");
	return sg_xmlValBuf;
}

const unsigned char* XmlGetText(const xml_t *xml)
{
	unsigned char *start, *end;
	start = strchr(xml->start, '>');
	if(!start || start > xml->end)
		return "";

	//while(!isalnum(*start))
		++start;
	
	end = strchr(start, '<');
	if(!end || end > xml->end)
		return "";

	memset(sg_xmlValBuf, 0, sizeof(sg_xmlValBuf));
	if(start < end)
		memcpy(sg_xmlValBuf, start, end - start);
	else
		strcpy(sg_xmlValBuf, "");
	return sg_xmlValBuf;
}

void XmlAddChild(const unsigned char *childName, const unsigned char **Attr, unsigned int AttrNum, const unsigned char **Val, 
	xml_t *fatherXML, xml_t *childXML)
{
	unsigned char buffer[2048];
	unsigned char *pp = buffer;
	unsigned int iRestLen = 0;

	xml_t *p = fatherXML;
	int iTabNum = 0;

	unsigned char bFatherWithoutChild = false;

	unsigned int iNum = 0;
	memset(buffer, 0, sizeof(buffer));
	strcpy(childXML->name, childName);

	if(NULL != fatherXML->father)
	{
		bFatherWithoutChild = (0 == memcmp(fatherXML->end - 2, "/>", 2) ? true :false);

		while(p->father)
		{
			p = p->father;
			++iTabNum;
		}
		iRestLen = p->end - fatherXML->end;
		if(!bFatherWithoutChild)
			iRestLen += strlen(fatherXML->name) + 3; // </%s>

// 		memset(pp, 0x09, iTabNum);
// 		pp += iTabNum;
	}

	sprintf(pp, "<%s ", childName);
	pp += strlen(childName) + 2; // 2: < + 0x20

	for(iNum = 0; iNum < AttrNum; ++iNum)
	{
		sprintf(pp, "%s=\"%s\" ", Attr[iNum], Val[iNum]);
		pp += strlen(Attr[iNum]) + strlen(Val[iNum]) + 4; // 4: = + " + " + 0x20
	}

	memcpy(pp, "/>", 2);
	pp += 2;

	if(NULL == fatherXML->father)
	{
		memcpy(fatherXML->start, buffer, pp - buffer);
		fatherXML->end = fatherXML->start + (pp - buffer);
		XmlGetChild(fatherXML, childName, 0, childXML);
		return;
	}

	if(bFatherWithoutChild)
	{
		unsigned char *newEnd = fatherXML->end - 3;
		unsigned int tmpLen;

		memset(pp, 0x09, iTabNum - 1);
		pp += iTabNum - 1;
		sprintf(pp, "</%s>", fatherXML->name);
		pp += strlen(fatherXML->name) + 3;

		while(1)
		{
			if(isspace(*newEnd))
				--newEnd;
			else
			{
				++newEnd;
				tmpLen = newEnd - fatherXML->end + 1;
				*newEnd = '>';
				break;
			}
		}
		memmove(newEnd + 1 + (pp - buffer), fatherXML->end, iRestLen);
		memcpy(newEnd + 1, buffer, pp - buffer);
		fatherXML->end = newEnd + 1 + (pp -buffer);

		p = fatherXML;
		while(p->father)
		{
			p->father->end += tmpLen + (pp -buffer);
			p = p->father;
		}
	}
	else
	{
		fatherXML->end -= strlen(fatherXML->name) + 3;
		memmove(fatherXML->end + (pp - buffer), fatherXML->end, iRestLen);
		memcpy(fatherXML->end, buffer, pp - buffer);
		fatherXML->end += (pp -buffer) + strlen(fatherXML->name) + 3;

		p = fatherXML;
		while(p->father)
		{
			p->father->end += (pp - buffer);
			if(NULL == p->father)
				*(p->end) = '\0';
			p = p->father;
		}
	}
	iNum = 0;
	while(1)
	{
		XmlGetChild(fatherXML, childName, iNum, childXML);
		if(!childXML->start)
		{
			XmlGetChild(fatherXML, childName, iNum - 1, childXML);
			break;
		}
		++iNum;
	}
	return;
}

void XmlAddAddr(xml_t *xml, const unsigned char *Attr,const unsigned char *Val)
{
	unsigned int len;
	unsigned char *newEnd = strchr(xml->start, '>') - 1;
	xml_t *p = xml;
	unsigned char tempBuffer[100];

	while('/' == *newEnd || 0x20 == *newEnd)
		--newEnd;

	++newEnd;

	sprintf(tempBuffer, " %s=\"%s\"", Attr, Val);
	len = strlen(tempBuffer);

	while(p->father)
		p = p->father;

	memmove(newEnd + len, newEnd, p->end - newEnd);
	memcpy(newEnd, tempBuffer, len);

	p = xml;
	while(p)
	{
		p->end += len;
		if(NULL == p->father)
			*(p->end) = '\0';
		p = p->father;
	}
}

void XmlAddText(xml_t *xml, const unsigned char *Text)
{
	unsigned char buffer[2048] = {"\0"};
	unsigned char *pp = buffer;
	unsigned int iRestLen = 0;

	xml_t *p = xml;
	int iTabNum = 0;

	unsigned char bFatherWithoutChild = false;

	if(NULL != xml->father)
	{
		bFatherWithoutChild = (0 == memcmp(xml->end - 2, "/>", 2) ? true :false);

		while(p->father)
		{
			p = p->father;
			++iTabNum;
		}
		iRestLen = p->end - xml->end;
		if(!bFatherWithoutChild)
			iRestLen += strlen(xml->name) + 3; // </%s>

		memset(pp, 0x09, iTabNum);
		pp += iTabNum;
	}

	memcpy(pp, Text, strlen(Text));
	pp += strlen(Text); // 2: < + 0x20

	if(NULL == xml->father)
	{
		xml->start;
		memcpy(xml->start, buffer, pp - buffer);
		xml->end = xml->start + (pp - buffer);
		return;
	}

	if(bFatherWithoutChild)
	{
		unsigned char *newEnd = xml->end - 3;
		unsigned int tmpLen;

		memset(pp, 0x09, iTabNum - 1);
		pp += iTabNum - 1;
		sprintf(pp, "</%s>", xml->name);
		pp += strlen(xml->name) + 3;

		while(1)
		{
			if(isspace(*newEnd))
				--newEnd;
			else
			{
				++newEnd;
				tmpLen = newEnd - xml->end + 1;
				*newEnd = '>';
				break;
			}
		}
		memmove(newEnd + 1 + (pp - buffer), xml->end, iRestLen);
		memcpy(newEnd + 1, buffer, pp - buffer);
		xml->end = newEnd + 1 + (pp -buffer);

		p = xml;
		while(p->father)
		{
			p->father->end += tmpLen + (pp -buffer);
			p = p->father;
		}
	}
	else
	{
		xml->end -= strlen(xml->name) + 3;
		memmove(xml->end + (pp - buffer), xml->end, iRestLen);
		memcpy(xml->end, buffer, pp - buffer);
		xml->end += (pp -buffer) + strlen(xml->name) + 3;

		p = xml;
		while(p->father)
		{
			p->father->end += (pp - buffer);
			if(NULL == p->father)
				*(p->end) = '\0';
			p = p->father;
		}
	}
}
