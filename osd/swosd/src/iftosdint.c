/**
 * @file iftosdint.c
 * @brief
 * @author jerry.king
 * @date 2012-12-5 Created
**/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
******************************************************************/
#include "iftosdint.h"
#include "DspOsdCmd.h"
int iftFontUnitHdr_pushback(iftFontUnitHdr *hdr, iftFontUnit *hash)
{
    assert(hash->next == NULL) ;
    if (hdr->head == hdr->tail && hdr->tail == NULL)
    {
        hdr->head  = hash ;
        hdr->tail  = hash ;
    }
    else
    {
        hdr->tail->next = hash ;
        hdr->tail = hash;
    }
    return 0 ;
}
iftFontUnit * iftFontUnitHdr_popfront(iftFontUnitHdr *hdr)
{
    iftFontUnit * cur = hdr->head ;

    if (hdr->head == hdr->tail)
    {
        hdr->head = NULL;
        hdr->tail = NULL;
    }
    else
    {
        if (hdr->head)
        {
            hdr->head = hdr->head->next;
        }
    }
    return cur;
}


