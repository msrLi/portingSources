/**
 * @file iftCore.h
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

#ifndef IFTMMU_H_
#define IFTMMU_H_
#include "iftosdint.h"
#define IFT_MEM_NUM 1

#define DEBUG_MALLOC_SET 1

void iftMemReport(void);
#define iftMemAlloc(size) iftMemKAlloc(size,0)
#define iftMemFree(ptr) iftMemKFree(ptr,0)


void* iftMemKAlloc(int32_t size, int32_t kind);

void iftMemKFree(void * ptr, int32_t kind);

void  memtest();
#endif /* IFTMMU_H_ */
