/**
 * @file iftosdint.h
 * @brief
 * @author jerry.king
 * @date 2012-11-30 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef IFTOSDINT_H_
#define IFTOSDINT_H_

/**
 * freetype
 */
#include <ft2build.h>
#include FT_FREETYPE_H

#include FT_IMAGE_H
#include FT_STROKER_H
#include FT_BBOX_H
#include FT_OUTLINE_H
//#include
#include "iftosd.h"
#include "iftosdusr.h"
#include "iftsysTypes.h"
#include "osdFontGen.h"

#include "MemPoolMgr.h"

#include "debug.h"
#include "iftmmu.h"
#include <string.h>
#include <assert.h>
#include "iutil_type_int.h"
/*
 * Many fonts can share the same underlying face data; this
 * structure references that.  Note that many faces may in fact
 * live in the same font file; that is irrelevant to this structure
 * which is concerned only with the individual faces themselves
 */
#define DEF_SMEM_EQU(a,b,stru) (memcmp((a),(b),sizeof(stru))==0)
#define IFT_PT2PIX(pt, resolution) ((pt)*(resolution)/(72))
#define IFT_PIX2PT(pix, resolution) ((pix)*(72)/(resolution))

#define IFT_F26DOT6_TO_PIX_CELING(t)   (((t)+63)>>6)
#define IFT_F26DOT6_CELING(t)  (((t)+63)&-64)

#define IFT_F26DOT6_TO_PIX_FLOOR(t) ((t)>>6)
#define IFT_F26DOT6_FLOOR(t)  ((t)&-64)

#define IFT_F26DOT6_TO_PIX_ROUND(t)   (((t)+32)>>6)
#define IFT_F26DOT6_ROUND(t) (((t)+32)&-64)
#define IFT_PIX_TO_F26DOT6(t) ((t)<<6)

typedef struct _iftFtFile
{
    int32_t ref; /* number of font infos using this file */
    struct _iftFtFile *next;
    char *fpname; /* file name */
    int32_t id; /* font index within that file */

    FT_F26Dot6 xsize; /* current xsize setting */
    FT_F26Dot6 ysize; /* current ysize setting */
    FT_Matrix matrix; /* current matrix setting */

    int32_t lock; /* lock count; can't unload unless 0 */
    FT_Face face; /* pointer to face; only valid when lock */
} iftFtFile;

/*
 * This structure holds the data extracted from a pattern
 * needed to create a unique font object.
 */

typedef struct _iftFontInfo
{
    /*
     * Hash value (not include in hash value computation)
     */
    int32_t key;
    struct _iftFontInfo *next;
    iftFtFile *file; /* face source */

    int rgba; /* subpixel order */
    int lcd_filter; /* lcd filter */
    FT_Int load_flags; /* glyph load flags */
    FT_Matrix matrix; /* glyph transformation matrix */
    iftBool transform; /* non-identify matrix? */
    iftBool render; /* whether to use the Render extension */

    iftBool antialias; /**<  Whether glyphs can be antialiased //¿¹¾â³Ý*/
    iftBool embolden; /**<  Rasterizer should synthetically embolden the font*/
    iftBool outline; /**<  Whether the glyphs are outlines   //Ãè±ßÐ§¹û*/
    iftBool hinting; /**< Whether the rasterizer should use hinting //×ÖÌåÎ¢µ÷*/
    iftBool verticallayout; /**<  Use vertical layout*/
    iftBool autohint; /**<  Use autohinter instead of normal hinter*/

    iftBool scalable; /**<   Whether glyphs can be scaled    //Ëõ·Å*/

    /*
     * Internal fields
     */
    int32_t spacing;
    iftBool minspace;
    int32_t char_width;
} iftFontInfo;
//===================================================

//===================================================

//char ÃèÊö
typedef struct iftCharSpec
{
    iftChar32 ucs4; //unicode code
    SCoordinate2D coo;
} iftCharSpec;

typedef struct iftCharfontSpec
{
    iftCharSpec ch;
    iftFontCtrlInt *font;
} iftCharfontSpec;
typedef struct iftGlyphSpec
{
    uint32_t glyidx;
    SCoordinate2D coo;
} iftGlyphSpec;
typedef struct iftGlyphFontSpec
{
    iftGlyphSpec gly;
    iftFontCtrlInt *font;
} iftGlyphFontSpec;
//===================================================
/**
 *
 */
#define DEF_UCS4META_BMPBUF_LEN 0x2000

#define DEF_UCS4META_MAGIC 0x1122AABB
#define  UCS4HASHMETA_VALID(pt) ((pt)->magic==DEF_UCS4META_MAGIC)




//#define USE_META_BBOX_LEN(pix_len) ((pix_len)*(pix_len)+(256))
//
//#define DEF_MEMMETA_SIZE USE_META_BBOX_LEN(MAX_FONT_PIX_SIZE)
//
#define DEF_PIX_MIN -1024
#define DEF_PIX_MAX 1024
typedef struct SPixBbox
{
    int16_t xMin;
    int16_t xMax;
    int16_t yMin;
    int16_t yMax;
} SPixBbox;
//ucs4
typedef struct iftFontGlyphUnit
{
    int32_t refcnt;
    FT_Glyph_Metrics glyphmetrics; //glyph item
    SPixBbox bbox_pix;
    const iftFontCtrlInt * fontctrl;
    SCoordinate2D bmpbuf_oo;
    int bmpbuffMaxsize;
    int bmpMaxRows;
    FT_Bitmap bmp; //copy from glyph and clone bmp data
    uint8_t bmpbuf[0];
} iftFontGlyphUnit;
/*
 * A hash table translates Unicode values into glyph indicies
 */
typedef struct iftUcs4HashMeta
{
    int magic;  //must DEF_UCS4META_MAGIC
    iftChar32 ucs4;
    struct iftUcs4HashMeta *next ;
    iftFontGlyphUnit meta;
} iftUcs4HashMeta;
#define  UCS4HASH_METRIC 0x80
#define  UCS4HASH_MASK  0x7F
#define  UCS4HASH_VAL(t) ((t)&UCS4HASH_MASK)

typedef iftUcs4HashMeta * PTR_UcsHashMap[UCS4HASH_METRIC];
typedef struct iftUcs4HashTable
{
    PTR_UcsHashMap map;
    int32_t metacnt;
    SPoolMgr *ucs4pool;
} iftUcs4HashTable;

//hash_table
//fixed

typedef struct _iftFontUnit
{
    struct _iftFontUnit *next;
    iftCharfontSpec chfont;
    const iftFontGlyphUnit * glyph;
} iftFontUnit;
typedef struct iftFontUnitHdr
{
    iftFontUnit *head;
    iftFontUnit *tail;
} iftFontUnitHdr;
//===================================================
// advertising by string /fontinfo /lineconf,charconf. {lineconf,charconf,by user set}

//===================================================
//draw
//surface
typedef enum
{
    RGBA,
    YUV444
} ESurfaceType;
typedef struct _iftSurface
{
    iftRect rect;
    ESurfaceType stype; //
    uint32_t *data;
    int datalen;
} iftSurface;

typedef struct iftLineMemUnit
{
    iftFontUnitHdr lineHdr;
    int32_t widePix;
    int32_t fontcnt;
} iftLineMemUnit;
#define DEF_MAX_LINEMEMUNIT_CNT 32
#define DEF_MAX_FONTUNIT_SIZE 0x800
typedef struct iftRenderFontTable
{
    iftLineMemUnit *lmu;
    int32_t maxLines;
    SPoolMgr *lmuPool;
    SPoolMgr *fontUnitPool;
} iftRenderFontTable;

int iftFontUnitHdr_pushback(iftFontUnitHdr *hdr, iftFontUnit *hash);
iftFontUnit * iftFontUnitHdr_popfront(iftFontUnitHdr *hdr);

/**
 * @brief lib freetype ctx
 *
 */
typedef struct SftCoreCtx
{
    FT_Library lib;
    FT_Face face;
    FT_GlyphSlot slot;
} SftCoreCtx;
typedef struct SServCtx
{
    iftRect rect; //carven rect coodi=(0.0)
    iftStyle *pstl; //point to use id
    iftRenderFontTable renderTable;
} SServCtx;
typedef enum E_FTGEN_STAGE
{
    E_FTGEN_STAGE_NOMAL,
    E_FTGEN_STAGE_SESSION
} E_FTGEN_STAGE;

typedef struct iftStyleSheet
{
    iftStyle *front;
    iftStyle *tail;
    int cnt;
} iftStyleSheet;
typedef enum  E_GRAY256SUB_TYPE
{
    E_GRAY256SUB_TYPE_NORMAL = 1,
    E_GRAY256SUB_TYPE_OUTLINE = 2,
    E_GRAY256SUB_TYPE_Embolden = 3,
} E_GRAY256SUB_TYPE;

#define GRAY256_LEVEL_CNT 16
typedef union SGray256Sub
{
    unsigned char val;
    struct
    {
        unsigned char level: 4;
        unsigned char type: 4;
    } sub;
} SGray256Sub;

typedef struct iftFtgen
{
    int ftgenid;
    iftUcs4HashTable ucs4hash;
    iftStyleSheet stylsheet;
    //  iftStyle *defStyle;
    iftFontInfo *fontInfo;
    SServCtx servCtx;
    SftCoreCtx coreCtx;

    E_FTGEN_STAGE stage; //½×¶Î
} iftFtgen;

#endif /* IFTOSDINT_H_ */
