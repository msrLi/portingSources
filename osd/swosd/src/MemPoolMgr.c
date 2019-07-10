/*
 * MemPoolMgr.c
 *
 *  Created on: 2013-1-15
 *      Author: qxy
 */

#include "MemPoolMgr.h"
#include "iftmmu.h"
#include "debug.h"
SPoolMgr * mpool_create(void* dataptr, int itemlen, int totallen)
{
    if (!dataptr || totallen < sizeof(SPoolMgr) || itemlen < 0)
    {
        return NULL;
    }
    SPoolMgr * mgr = (SPoolMgr*)dataptr;
    mgr->totallen = totallen ;
    mgr->itemlen = itemlen;
    mgr->itemcnt  = (totallen - sizeof(SPoolMgr)) / (itemlen + sizeof(void*)) ;
    mgr->data = ((uint8_t *)mgr->items) + mgr->itemcnt * sizeof(void*);
    mgr->datalen = mgr->itemcnt * itemlen;
    return mgr;
}

int mpool_destroy(SPoolMgr * mgr)
{
    memset(mgr, 0, mgr->totallen);
    return 0;
}
int mpool_clean(SPoolMgr * mgr)
{
    if (!mgr)
    {
        return -1 ;
    }
    memset(mgr->data, 0, mgr->datalen);
    memset(mgr->items, 0, mgr->itemcnt * sizeof(void*));

    return 0 ;
}
void * mpool_attach(SPoolMgr * mgr)
{
    int i  ;
    for (i = 0; i < mgr->itemcnt; i++)
    {
        if (!mgr->items[i])
        {
            mgr->items[i] = mgr->data + mgr->itemlen * i;
            return mgr->items[i];
        }
    }
    return NULL;
}
int mpool_detach(SPoolMgr * mgr, void * item)
{
    int pos = (uint8_t*)item - mgr->data;
    if (pos < 0 || (pos % mgr->itemlen) != 0)
    {
        MYTRACE();
        //item not valid
        return -1;
    }
    int cnt = pos / mgr->itemlen;
    memset(mgr->items[cnt], 0, mgr->itemlen);
    mgr->items[cnt] = NULL ;
    return 0 ;
}
void* mpool_attach_n(SPoolMgr * mgr, int ncnt)
{
    int i = 0 ;
    for (i = 0; i < (mgr->itemcnt - ncnt); i++)
    {
        int j = 0 ;
        int null_cnt = 0 ;
        for (j = 0; j < ncnt; j++)
        {
            if (!mgr->items[i + j])
            {

                null_cnt++;
            }
        }
        if (null_cnt == ncnt)
        {
            //seek ok
            for (j = 0; j < ncnt; j++)
            {
                mgr->items[i + j] = mgr->data + mgr->itemlen * (i + j);
            }
            return mgr->items[i];
        }
    }
    return NULL;
}
int mpool_detach_n(SPoolMgr * mgr, void* item, int ncnt)
{
    int pos = (uint8_t*)item - mgr->data;
    if (pos < 0 || (pos % mgr->itemlen) != 0)
    {
        MYTRACE();
        //item not valid
        return -1;
    }
    int cnt = pos / mgr->itemlen;
    /**
     * count val cnt
     */
    int j = 0 ;
    int val_cnt = 0 ;
    for (j = 0; j < ncnt ; j++)
    {
        if (cnt + j >= mgr->itemcnt)
        {
            break;
        }
        if (mgr->items[cnt + j])
        {
            val_cnt++;
        }
    }
    if (val_cnt != ncnt)
    {
        MYTRACE();
        return -1;
    }
    //seek ok
    for (j = 0; j < ncnt; j++)
    {
        mgr->items[cnt + j] = NULL;
    }
    memset(item, 0, ncnt * mgr->itemlen);
    return 0;
}
