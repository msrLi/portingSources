/**
 * @file tgadump.c
 * @brief
 * @author jerry.king
 * @date 2012-12-6 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#include "tgadump.h"

iftBool WriteTGA(const char *filename, const Pixel32 *pxl, uint16_t width, uint16_t height)
{
    FILE  *pf = fopen(filename, "w+b");
    if (pf)
    {
        TGAHeader header;
        memset(&header, 0, sizeof(TGAHeader));
        header.imageType = 2;
        header.width = width;
        header.height = height;
        header.depth = 32;
        header.descriptor = 0x20;

        fwrite((const char *) &header, 1, sizeof(TGAHeader), pf);
        fwrite((const char *) pxl, sizeof(Pixel32), width * height, pf);
        fflush(pf);
        fclose(pf);
        return 1;
    }
    return 0;
}
