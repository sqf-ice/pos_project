#include <stdio.h>
#include <string.h>
#include "LHBmalloc.h"


typedef struct _tagPosition
{
	void* ptr;
	unsigned int len;
}Position;

unsigned char gl_LMEM_buffer[MAX_MEM_SIZE];

Position gl_LMEM_Position[MAX_PRT_COUNT];

unsigned char gl_LMEM_Flag[MAX_FLAG_COUNT] = {0x00}; // 0 : enable

/// above total 15 KB .....

void LmemInit(void)
{
	memset(gl_LMEM_buffer, 0, sizeof(gl_LMEM_buffer));
	memset(gl_LMEM_Flag, 0, sizeof(gl_LMEM_Flag));
	memset(gl_LMEM_Position, 0, sizeof(gl_LMEM_Position));
}

void *Lmalloc(unsigned int uiSize)
{
	unsigned int uiCount = 0;
	unsigned int i;
	unsigned int uiStartBit = 0;
	unsigned char bOk = 0;
	Position *ptr = gl_LMEM_Position;

	if(0 == uiSize)
	{	return NULL;}

	for(i = 0; !bOk && i < MAX_FLAG_COUNT; ++i)
	{
		unsigned int j;
		for(j = 0; j < 8; ++j)
		{
			if(0 == (gl_LMEM_Flag[i] & (0x01 << j))) // empty bit
			{
				if(0 == uiCount)
				{	uiStartBit = i * 8 + j;}
				++uiCount;
			}
			else
			{	uiCount = 0;}

			if(uiCount >= uiSize)
			{
				bOk = 1;
				break;
			}
		}
	}

	if(bOk)
	{
		bOk =0;

		for(i = 0; i < MAX_PRT_COUNT; ++i)
		{
			if(!gl_LMEM_Position[i].ptr)
			{
				ptr = &gl_LMEM_Position[i];
				bOk = 1;
				break;
			}
		}
	}

	if(bOk)
	{
		unsigned int uiBit = uiStartBit % 8;
		unsigned int uiByte = uiStartBit / 8;

		uiCount = 0;
		while(uiCount < uiSize)
		{
			for(; uiBit < 8 ;++uiBit)
			{
				gl_LMEM_Flag[uiByte] |= (0x01 << uiBit);
				++uiCount;
				if(uiCount >= uiSize)
				{
					ptr->ptr = &gl_LMEM_buffer[uiStartBit];
					ptr->len = uiSize;
					return ptr->ptr;
				}
			}
			++uiByte;
			uiBit = 0;
		}
	}
	return NULL;
}

void Lfree(void *ptr)
{
	unsigned int i;

	for(i = 0; i < MAX_PRT_COUNT; ++i)
	{
		if(gl_LMEM_Position[i].ptr == ptr)
		{
			unsigned long ulStartBit = (unsigned char *)gl_LMEM_Position[i].ptr - gl_LMEM_buffer;
			unsigned int uiBit = ulStartBit % 8;
			unsigned int uiByte = ulStartBit / 8;
			unsigned int uiSize = gl_LMEM_Position[i].len;

			unsigned int uiCount = 0;
			while(uiCount < uiSize)
			{
				for(; uiBit < 8 ;++uiBit)
				{
					gl_LMEM_Flag[uiByte] &= (0xFE << uiBit);
					++uiCount;
					if(uiCount >= uiSize)
					{
						gl_LMEM_Position[i].ptr = NULL;
						gl_LMEM_Position[i].len = 0;
						break;
					}
				}
				++uiByte;
				uiBit = 0;
			}
			break;
		}
	}
}
