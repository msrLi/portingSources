/**
 * @file iftstring.c
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

#include "iftstring.h"
#include "gb2312.h"
#include "ascii.h"
#include "euc_cn.h"
#include <stdlib.h>
inline int32_t iftGb2312ToUcs4(const char *src_orig, iftChar32 *dst, int len)
{
    if (!src_orig || !dst || len <= 0)
    {
        return -1 ;
    }
    // uint16_t st = 0x496C;
    return euc_cn_mbtowc((ucs4_t*)dst, (const unsigned char*)src_orig, 4);
}
#if 0
int unicode_to_utf8(uint16_t *in, int insize, uint8_t *out, int *outsize)
{
    int i = 0;
    // int outsize = 0;
    int charscount = 0;
    uint8_t *result = NULL;
    uint8_t *tmp = NULL;

    charscount = insize / sizeof(uint16_t);
    //result = (uint8_t *)malloc(charscount * 3 + 1);
    //memset(result, 0, charscount * 3 + 1);
    tmp = result;

    for (i = 0; i < charscount; i++)
    {
        uint16_t unicode = in[i];

        if (unicode >= 0x0000 && unicode <= 0x007f)
        {
            *tmp = (uint8_t)unicode;
            tmp += 1;
            outsize += 1;
        }
        else if (unicode >= 0x0080 && unicode <= 0x07ff)
        {
            *tmp = 0xc0 | (unicode >> 6);
            tmp += 1;
            *tmp = 0x80 | (unicode & (0xff >> 2));
            tmp += 1;
            outsize += 2;
        }
        else if (unicode >= 0x0800 && unicode <= 0xffff)
        {
            *tmp = 0xe0 | (unicode >> 12);
            tmp += 1;
            *tmp = 0x80 | (unicode >> 6 & 0x00ff);
            tmp += 1;
            *tmp = 0x80 | (unicode & (0xff >> 2));
            tmp += 1;
            outsize += 3;
        }

    }

    *tmp = '\0';
    *out = result;
    return 0;
}

int utf8_to_unicode(uint8_t *in, uint16_t **out, int *outsize)
{
    uint8_t *p = in;
    uint16_t *result = NULL;
    int resultsize = 0;
    uint8_t *tmp = NULL;

    result = (uint16_t *)malloc(strlen(in) * 2 + 2); /* should be enough */
    memset(result, 0, strlen(in) * 2 + 2);
    tmp = (uint8_t *)result;

    while (*p)
    {
        if (*p >= 0x00 && *p <= 0x7f)
        {
            *tmp = *p;
            tmp++;
            *tmp = '\0';
            resultsize += 2;
        }
        else if ((*p & (0xff << 5)) == 0xc0)
        {
            uint16_t t = 0;
            uint8_t t1 = 0;
            uint8_t t2 = 0;

            t1 = *p & (0xff >> 3);
            p++;
            t2 = *p & (0xff >> 2);

            *tmp = t2 | ((t1 & (0xff >> 6)) << 6);//t1 >> 2;
            tmp++;

            *tmp = t1 >> 2;//t2 | ((t1 & (0xff >> 6)) << 6);
            tmp++;

            resultsize += 2;
        }
        else if ((*p & (0xff << 4)) == 0xe0)
        {
            uint16_t t = 0;
            uint8_t t1 = 0;
            uint8_t t2 = 0;
            uint8_t t3 = 0;

            t1 = *p & (0xff >> 3);
            p++;
            t2 = *p & (0xff >> 2);
            p++;
            t3 = *p & (0xff >> 2);

            //Little Endian
            *tmp = ((t2 & (0xff >> 6)) << 6) | t3;//(t1 << 4) | (t2 >> 2);
            tmp++;

            *tmp = (t1 << 4) | (t2 >> 2);//((t2 & (0xff >> 6)) << 6) | t3;
            tmp++;
            resultsize += 2;
        }

        p++;
    }

    *tmp = '\0';
    tmp++;
    *tmp = '\0';
    resultsize += 2;

    *out = result;
    *outsize = resultsize;
    return 0;
}
#endif

int32_t iftUtf8ToUcs4(const char *src_orig, iftChar32 *dst, int len)
{
    const char *src = src_orig;
    int8_t s;
    int extra;
    iftChar32 result;

    if (len == 0) { return 0; }

    s = *src++;
    len--;

    if (!(s & 0x80))
    {
        result = s;
        extra = 0;
    }
    else if (!(s & 0x40))
    {
        return -1;
    }
    else if (!(s & 0x20))
    {
        result = s & 0x1f;
        extra = 1;
    }
    else if (!(s & 0x10))
    {
        result = s & 0xf;
        extra = 2;
    }
    else if (!(s & 0x08))
    {
        result = s & 0x07;
        extra = 3;
    }
    else if (!(s & 0x04))
    {
        result = s & 0x03;
        extra = 4;
    }
    else if (!(s & 0x02))
    {
        result = s & 0x01;
        extra = 5;
    }
    else
    {
        return -1;
    }
    if (extra > len) { return -1; }

    while (extra--)
    {
        result <<= 6;
        s = *src++;

        if ((s & 0xc0) != 0x80) { return -1; }

        result |= s & 0x3f;
    }
    *dst = result;
    return src - src_orig;
}
int32_t FcUtf8Len(const char *string, int len, int *nchar, int *wchar)
{
    int n;
    int clen;
    iftChar32 c;
    iftChar32 max;

    n = 0;
    max = 0;
    while (len)
    {
        clen = iftUtf8ToUcs4(string, &c, len);
        if (clen <= 0) /* malformed UTF8 string */
        {
            return -1;
        }
        if (c > max) { max = c; }
        string += clen;
        len -= clen;
        n++;
    }
    *nchar = n;
    if (max >= 0x10000) { *wchar = 4; }
    else if (max > 0x100) { *wchar = 2; }
    else
    {
        *wchar = 1;
    }
    return 0;
}

int32_t iftUcs4ToUtf8(iftChar32 ucs4, int8_t dest[FC_UTF8_MAX_LEN])
{
    int bits;
    int8_t *d = dest;

    if (ucs4 < 0x80)
    {
        *d++ = ucs4;
        bits = -6;
    }
    else if (ucs4 < 0x800)
    {
        *d++ = ((ucs4 >> 6) & 0x1F) | 0xC0;
        bits = 0;
    }
    else if (ucs4 < 0x10000)
    {
        *d++ = ((ucs4 >> 12) & 0x0F) | 0xE0;
        bits = 6;
    }
    else if (ucs4 < 0x200000)
    {
        *d++ = ((ucs4 >> 18) & 0x07) | 0xF0;
        bits = 12;
    }
    else if (ucs4 < 0x4000000)
    {
        *d++ = ((ucs4 >> 24) & 0x03) | 0xF8;
        bits = 18;
    }
    else if (ucs4 < 0x80000000)
    {
        *d++ = ((ucs4 >> 30) & 0x01) | 0xFC;
        bits = 24;
    }
    else
    {
        return 0;
    }

    for (; bits >= 0; bits -= 6)
    {
        *d++ = ((ucs4 >> bits) & 0x3F) | 0x80;
    }
    return d - dest;
}
