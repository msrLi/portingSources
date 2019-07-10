/**
 * @file tgadump.h
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

#ifndef TGADUMP_H_
#define TGADUMP_H_
#include <stdio.h>
#include "iftosdint.h"

// TGA Header struct to make it simple to dump a TGA to disc.

typedef iftColor Pixel32;

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

typedef struct TGAHeader
{
    uint8_t idLength, // Length of optional identification sequence.
            paletteType, // Is a palette present? (1=yes)
            imageType; // Image data type (0=none, 1=indexed, 2=rgb,
    // 3=grey, +8=rle packed).
    uint16_t firstPaletteEntry, // First palette index, if present.
             numPaletteEntries; // Number of palette entries, if present.
    uint8_t paletteBits; // Number of bits per palette entry.
    uint16_t x, // Horiz. pixel coord. of lower left of image.
             y, // Vert. pixel coord. of lower left of image.
             width, // Image width in pixels.
             height; // Image height in pixels.
    uint8_t depth, // Image color depth (bits per pixel).
            descriptor; // Image attribute flags.
} TGAHeader;

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

iftBool WriteTGA(const char *filename, const Pixel32 *pxl, uint16_t width, uint16_t height);

#endif /* TGADUMP_H_ */
