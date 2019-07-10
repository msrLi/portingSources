/**
 * @file iftsysTypes.h
 * @brief
 * @author jerry.king
 * @date 2012-11-29 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef IFTSYSTYPES_H_
#define IFTSYSTYPES_H_

#include <stdint.h>
#include <inttypes.h>

typedef int     iftBool;
const static int iftTURE  = 1;
const static int iftFALSE = 0;

typedef int32_t iftChar32;
typedef int32_t iftStyleID;

#define  INVALD_IFTSTYLEID 0

#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
///////////////////////////////////////////////////////
///memery config
#define USE_META_BBOX_LEN(pix_len) ((pix_len)*(pix_len)+(128))

/**
 * define MAX font pixels ,
 */
#define MAX_FONT_PIX_SIZE 110

#define DEF_GRAYFRAMPOOL_SIZE 0xC00000//2592*2048
//#define DEF_GRAYFRAMPOOL_SIZE   1600*1200
#define MAX_OUTLINE_PIXSIZE     5
#define DEF_MEMMETA_CNT         0x200

#define DEF_MEMMETA_SIZE USE_META_BBOX_LEN(MAX_FONT_PIX_SIZE+MAX_OUTLINE_PIXSIZE*2)

#define IFT_MAX_FTGEN_STRU_SIZE 256

/**
 * mem map if iftosd
 */
/*
 |**********|*************|*****************|************************************\S
 graypool  graypool_inner

 *                        |--grapool inner  |inner hand size|
 */
#define IFT_MIN_MMU_ALLOC_SIZE (DEF_GRAYFRAMPOOL_SIZE*2 +IFT_MAX_FTGEN_STRU_SIZE \
    + DEF_MEMMETA_SIZE*DEF_MEMMETA_CNT)

///////////////////////////////////////////////////////
typedef struct SCoordinate2D //
{
    int16_t x; //x
    int16_t y; //y
} SCoordinate2D;

// Try to figure out what endian this machine is using. Note that the test
// below might fail for cross compilation; additionally, multi-byte
// characters are implementation-defined in C preprocessors.

//#if (('1234' >> 24) == '1')
//#elif (('4321' >> 24) == '1')
//#define ITE_BIG_ENDIAN
//#else
//#error "Couldn't determine the endianness!"
//#endif


typedef struct iftColor
{
    //#ifdef ITE_BIG_ENDIAN
    //    uint8_t a, r, g, b;
    //#else // Litle_ENDIAN
    uint8_t b, g, r, a;
    //#endif // BIG_ENDIAN
} iftColor;

typedef struct iftColorYUV
{
    //#ifdef ITE_BIG_ENDIAN
    //    uint8_t a, y, u, v;
    //#else // Litle_ENDIAN
    uint8_t v, u, y, a;
    //#endif // BIG_ENDIAN
} iftColorYUV;
// A simple 32-bit pixel.

typedef struct iftRect
{
    SCoordinate2D orgi;
    int16_t wide;
    int16_t hight;
    int16_t semi;
    int16_t position;
    int16_t maxFontWidth;
    int16_t resv;
} iftRect;
typedef struct iftRange
{
    void* data;
    int32_t len;
} iftRange;
typedef enum E_CHAR_SET
{
    E_CHAR_SET_DEFAULT  = 0,
    E_CHAR_SET_UTF8     = 1,
    E_CHAR_SET_GB2312   = 2
} E_CHAR_SET;
typedef int32_t IFT_HANDLE;
#endif /* IFTSYSTYPES_H_ */
