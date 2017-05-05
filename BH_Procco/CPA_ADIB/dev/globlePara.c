#include "globlePara.h"
#include "os_interface.h"
#include "COMMON.H"
#include "vxlibs.h"
void DisplayPage(/*BYTE ucpageNum, */ST_CONTENT * pstContent)
{

	ScrCls();
	if (ARABIC == gLanguage)
	{
		if (pstContent->stLineContent[0].content != 0)
		{
			if (pstContent->stLineContent[0].ucMode == ARABIC_CFONT)
			{
				ScrPrint(0, 0, CFONT, "%s", pstContent->stLineContent[0].content);
			}
			else if (pstContent->stLineContent[0].ucMode == ARABIC_ASCII)
			{
				ScrPrint(0, 0, ASCII, "%s", pstContent->stLineContent[0].content);
			}
			else
			{
				ArShowString(0, pstContent->stLineContent[0].ucAlignFlag, pstContent->stLineContent[0].content);
			}
		}
		if (pstContent->stLineContent[1].content != 0)
		{
			if (pstContent->stLineContent[1].ucMode == ARABIC_CFONT)
			{
				ScrPrint(0, 2, CFONT, "%s", pstContent->stLineContent[1].content);
			}
			else if (pstContent->stLineContent[1].ucMode == ARABIC_ASCII)
			{
				ScrPrint(0, 2, ASCII, "%s", pstContent->stLineContent[1].content);
			}
			else
			{
				ArShowString(2, pstContent->stLineContent[1].ucAlignFlag, pstContent->stLineContent[1].content);
			}
		}
		if (pstContent->stLineContent[2].content != 0)
		{
			if (pstContent->stLineContent[2].ucMode == ARABIC_CFONT)
			{
				ScrPrint(0, 4, CFONT, "%s", pstContent->stLineContent[2].content);
			}
			else if (pstContent->stLineContent[2].ucMode == ARABIC_ASCII)
			{
				ScrPrint(0, 4, ASCII, "%s", pstContent->stLineContent[2].content);
			}
			else
			{
				ArShowString(4, pstContent->stLineContent[2].ucAlignFlag, pstContent->stLineContent[2].content);
			}
			
		}
		if (pstContent->stLineContent[3].content != 0)
		{
			if (pstContent->stLineContent[3].ucMode == ARABIC_CFONT)
			{
				ScrPrint(0, 6, CFONT, "%s", pstContent->stLineContent[3].content);
			}
			else if (pstContent->stLineContent[3].ucMode == ARABIC_ASCII)
			{
				ScrPrint(0, 6, ASCII, "%s", pstContent->stLineContent[3].content);
			}
			else
			{
				ArShowString(6, pstContent->stLineContent[3].ucAlignFlag, pstContent->stLineContent[3].content);
			}
		}
	}
	else
	{
		if (pstContent->stLineContent[0].content != 0)
		{
			ScrPrint(0, 0, ASCII, "%s", pstContent->stLineContent[0].content);
		}
		if (pstContent->stLineContent[1].content != 0)
		{
			ScrPrint(0, 1, ASCII, "%s", pstContent->stLineContent[1].content);
		}
		if (pstContent->stLineContent[2].content != 0)
		{
			ScrPrint(0, 2, ASCII, "%s", pstContent->stLineContent[2].content);
		}
		if (pstContent->stLineContent[3].content != 0)
		{
			ScrPrint(0, 3, ASCII, "%s", pstContent->stLineContent[3].content);
		}
		if (pstContent->stLineContent[4].content != 0)
		{
			ScrPrint(0, 4, ASCII, "%s", pstContent->stLineContent[4].content);
		}
		if (pstContent->stLineContent[5].content != 0)
		{
			ScrPrint(0, 5, ASCII, "%s", pstContent->stLineContent[5].content);
		}
		if (pstContent->stLineContent[6].content != 0)
		{
			ScrPrint(0, 6, ASCII, "%s", pstContent->stLineContent[6].content);
		}
		if (pstContent->stLineContent[7].content != 0)
		{
			ScrPrint(0, 7, ASCII, "%s", pstContent->stLineContent[7].content);
		}
	}

}

DWORD DisplayContent(unsigned char ucNum, unsigned char ucSelectPageNum)
{
	BYTE	ucCurrentPage = 0;
	BYTE	ucKey;

	stPageContent.ucMaxpageNum = ucNum;
	while (1)
	{
		DisplayPage(&stPageContent.stContent[ucCurrentPage]);
		ucKey = GetOneKey(TO_1MIN);

		
		if (ucKey == KEYCANCEL || ucKey == NOKEY)
		{
			return STAT_NOK;
		}
// 		else if (ucKey == KEYENTER)
// 		{
// 			if (ucCurrentPage < stPageContent.ucMaxpageNum - 1)
// 			{
// 				ucCurrentPage++;
// //				continue;
// 			}
// 			//if ((ucCurrentPage > stPageContent.ucMaxpageNum - 1) && (ucSelectPageNum == NO_SELECT_PAGE))
// 			else if (ucSelectPageNum == NO_SELECT_PAGE)
// 			{
// 				break;
// 			}
// 		}
		else if (ucKey == KEYUP && ucCurrentPage != 0)
		{
			ucCurrentPage--;
			continue;
		}
		else if (ucKey == KEYENTER || ucKey == KEYDOWN)
		{
			if (ucCurrentPage < stPageContent.ucMaxpageNum - 1)
			{
				ucCurrentPage++;
			}
			else
			{
				if (ucSelectPageNum == NO_SELECT_PAGE)
				{
					break;
				}
			}
			
// 			if (ucCurrentPage >= stPageContent.ucMaxpageNum)
// 			{
// 				break;
// 			}
//			continue;
		}
		else if ((ucKey == KEY1 || ucKey == KEY2) && 
			ucSelectPageNum != NO_SELECT_PAGE && 
			ucCurrentPage >= ucSelectPageNum) // Modified by Kim_LinHB 2013-5-2 build 0.06.0024 from == to >= 
		{
			if (ucKey == KEY1)
			{
				return KEY1;
			}
			else if (ucKey == KEY2)
			{
				return KEY2;
			}
		}
	}
	
	return STAT_OK;
}