/**
 * @file iftCore.c
 * @brief
 * @author jerry.king
 * @date 2012-12-4 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#include "iftmmu.h"


typedef struct SMemUnit
{
    uint32_t mflag : 8;
    uint32_t mlen : 24;
    uint8_t data[0];
} SMemUnit;
#define DEF_MEM_MEMUSIZE 4


static struct
{
    const char *name;
    int alloc_count;
    int alloc_mem;
    int free_count;
    int free_mem;
} iftInUse[IFT_MEM_NUM] =
{
    {
        "default",
        0,
        0
    },
};

static int iftAllocCount, iftAllocMem;
static int iftFreeCount, iftFreeMem;

static const int iftMemNotice = 500 * 1024 * 1024;

static int iftAllocNotify, iftFreeNotify;

void iftMemReport(void)
{
    int i;
    printf("ift Memory Usage:\n");
    printf("\t   Which       Alloc           Free\n");
    printf("\t           count   bytes   count   bytes\n");
    for (i = 0; i < IFT_MEM_NUM; i++)
        printf("\t%8.8s[%8u][%8u][%8u][%8u]\n", iftInUse[i].name, iftInUse[i].alloc_count, iftInUse[i].alloc_mem,
               iftInUse[i].free_count, iftInUse[i].free_mem);
    printf("\t%8.8s[%8u][%8u][%8u][%8u]\n", "Total", iftAllocCount, iftAllocMem, iftFreeCount, iftFreeMem);
    iftAllocNotify = 0;
    iftFreeNotify = 0;
}


inline void *
iftMemKAlloc(int32_t size, int32_t kind)
{

#if !DEBUG_MALLOC_SET
    return malloc(size);
#else
    void * ret = NULL;
    if ((ret = malloc(size + DEF_MEM_MEMUSIZE)) != NULL)
    {
        ((SMemUnit *)ret)->mflag = 0xFF ;
        ((SMemUnit *)ret)->mlen = size;
        iftInUse[kind].alloc_count++;
        iftInUse[kind].alloc_mem += size;
        iftAllocCount++;
        iftAllocMem += size;
        iftAllocNotify += size;
        if (iftAllocNotify > iftMemNotice) { iftMemReport(); }
    }
    return (char*)ret + DEF_MEM_MEMUSIZE ;
#endif
}

void iftMemKFree(void * ptr, int32_t kind)
{
#if !DEBUG_MALLOC_SET
    free(ptr);
#else
    if (!ptr) { return ; }
    SMemUnit * punit = (SMemUnit *)((uint8_t*)ptr - 4);
    if (punit->mflag != 0xFF && punit->mlen <= 0)
    {
        dprintf(DBG_LEV_CRIT, "NO ACCESS FREEMEM!!CHECK!");
        return ;
    }
    int32_t size = punit->mlen ;
    {
        iftInUse[kind].free_count++;
        iftInUse[kind].free_mem += size;
        iftFreeCount++;
        iftFreeMem += size;
        iftFreeNotify += size;
        if (iftFreeNotify > iftMemNotice) { iftMemReport(); }
    }
    free(punit);
#endif
}

void  memtest()
{
    void *p = iftMemAlloc(32);
    iftMemFree(p);
    return ;
}
