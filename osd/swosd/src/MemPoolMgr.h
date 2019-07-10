/*
 * MemPoolMgr.h
 *
 *  Created on: 2013-1-15
 *      Author: qxy
 */

#ifndef MEMPOOLMGR_H_
#define MEMPOOLMGR_H_
#include <stdint.h>
#include <inttypes.h>


typedef struct SPoolMgr
{

    int totallen;
    int itemlen; //byte
    int itemcnt;
    int datalen; //byte
    uint8_t *data;
    void* items[0];
} SPoolMgr;
SPoolMgr * mpool_create(void* dataptr, int itemlen, int totallen);
int mpool_destroy(SPoolMgr * mgr);
void* mpool_attach(SPoolMgr * mgr);
int mpool_detach(SPoolMgr * mgr, void* item);
int mpool_clean(SPoolMgr * mgr);

/**
 *
 */
void* mpool_attach_n(SPoolMgr * mgr, int ncnt);
int mpool_detach_n(SPoolMgr * mgr, void* item, int ncnt);

#endif /* MEMPOOLMGR_H_ */
