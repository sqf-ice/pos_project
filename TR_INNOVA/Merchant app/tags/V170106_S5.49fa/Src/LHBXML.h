/****************************************************************************
NAME
    LHBXML.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2012.12.14      - created
****************************************************************************/
#ifndef _LINXML_H
#define _LINXML_H

#define XMLMAXSIZE (1024 * 10)

// the structure is not been designed completely, is should be designed as a tree, but it's just a single linked list.
// I don't want to use dynamic memory allocation, and it works in alpha v0.1, so I don't want to upgrade it to a tree now.
typedef struct xml_t
{
	unsigned char name[20];
	unsigned char *start;
	unsigned char *end;
	struct xml_t *father;
}xml_t;

// alpha v0.1
void XmlGetChild(const xml_t *fatherXML, const unsigned char *name, \
				 unsigned int childXMLIndex, xml_t *childXML);
const unsigned char* XmlGetValue(const xml_t *xml, const unsigned char *Attr);
void XmlAddChild(const unsigned char *childName, const unsigned char **Attr, \
				 unsigned int AttrNum, const unsigned char **Val, \
				 xml_t *fatherXML, xml_t *childXML);
void XmlAddAddr(xml_t *xml, const unsigned char *Attr,const unsigned char *Val);

#undef XmlAddChildNULL
#define XmlAddChildNULL(childName, fatherXML, childXML)  XmlAddChild((const unsigned char *)childName, NULL,0,NULL, (xml_t *)fatherXML, (xml_t *)childXML)

// alpha v0.2
const unsigned char* XmlGetText(const xml_t *xml);
//were not been designed logically, haven't tested yet.
void XmlAddText(xml_t *xml, const unsigned char *Text);

#endif
