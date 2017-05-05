#ifndef _LHB_MALLOC_H_
#define _LHB_MALLOC_H_

//256*8 bits = 2048
#define MAX_MEM_SIZE	2048 * 10    //total size of memory
#define MAX_FLAG_COUNT  256 * 10     //flag of memory
#define MAX_PRT_COUNT  128     //flag of memory

void LmemInit(void);

void *Lmalloc(unsigned int Size);
void Lfree(void *ptr);

#endif
