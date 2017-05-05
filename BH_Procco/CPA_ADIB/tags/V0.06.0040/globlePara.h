#ifndef GLOBLEPARA_H
#define GLOBLEPARA_H

#define NO_SELECT_PAGE	0xFF
#define ARABIC_ASCII	0x01
#define ARABIC_CFONT	0x02

unsigned char AutoPassword[10 + 1]; // Modified by Kim_LinHB 2014-5-29 from 10 to 11

typedef struct st_line
{
	unsigned char content[21+1];	//STRING
	unsigned char ucAlignFlag;		//LEFT RIGHT CENTER
	unsigned char ucMode;			//ASCII OR ARABIC
}ST_LINE;

typedef struct st_content
{
	ST_LINE stLineContent[8];
}ST_CONTENT;

typedef struct st_pageContent
{
	ST_CONTENT			stContent[20];
	unsigned char		ucMaxpageNum;
}ST_PAGECONTENT;

ST_PAGECONTENT stPageContent;

void DisplayPage(ST_CONTENT * pstContent);
/*
*	ucSelectPageNum is equal to array index.
*/
unsigned long DisplayContent(unsigned char ucNum, unsigned char ucSelectPageNum);

#endif
