/**
 * @file osdFontGen.h
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

#ifndef OSDFONTGEN_H_
#define OSDFONTGEN_H_
//struct _iftFtgen
//{
//
//};
#include "iftOsdServiceInt.h"
typedef struct iftParagInt
{
    iftParaLayout super;
    int32_t charspace; //字间距
} iftParagInt;
typedef struct iftFontCtrlInt
{
    iftFontCtrl super; //superLevel
    int32_t ascent; //升部
    int32_t descent; //降部
    int32_t height; //字高
    int32_t maxAdvanceWidth; //步进
    //iftColor fcolor;
} iftFontCtrlInt;
typedef enum FTSRC_TYPE
{
    FTSRC_TYPE_FILE,
    FTSRC_TYPE_MEM
} FTSRC_TYPE;
typedef struct iftStyle
{
    struct iftStyle * next;
    iftStyleID sid;
    FTSRC_TYPE ftsrcType;
    int32_t horz_resolution;
    int32_t vert_resolution;
    int32_t fontLineHight;
    union FontSrc
    {
        char *path; //font from file
        uint8_t *MemPtr; //font from mem
    } ftsrc;
    iftParagInt lineconf;
    iftFontCtrlInt fontconf;
} iftStyle;

//typedef struct _iftSurface *iftSuface_H;

typedef struct iftRenderFontSet *iftRenderFontSet_H;
typedef struct iftFtgen *iftFtgen_H;

void iftFtgen_runOnce();
/**
 * init syswide conf
 */
int32_t iftFtgen_setEnv(IftEnv *env);

/**
 * get font osd gen handle
 * @ drawWidthPix [in]
 * @ drawHightPix [in]
 *
 */
iftFtgen_H iftFtgen_hand(iftRange *rgnod);
/**
 * set rect of service //one canvas one  session
 */
int32_t iftFtgen_defStyle(iftFtgen_H hfg, iftOsdServStyleInt * servStyle);
int32_t iftFtgen_curStyle(iftFtgen_H hfg, iftOsdServStyleInt * servStyle);

// /**
//  * return style id
//  */
int32_t iftFtgen_setStyle(iftFtgen_H hfg, iftOsdServStyleInt * servStyle);
inline const iftStyle* iftFtgen_orgiStyle(iftFtgen_H hfg);

int32_t iftFtgen_setRect(iftFtgen_H hfg, int32_t rcWidthPix, int32_t rcHightPix);
//
/**
 * init  param
 */
int32_t iftFtgen_sessionEnter(iftFtgen_H hfg);

/**
 *
 */
int32_t iftFtgen_sessionExit(iftFtgen_H hfg);
//
/**
 *
 */

// int32_t iftFtgen_renderFontTableSetopt(iftFtgen_H hfg,iftRect *srect); //default use RBGA
/**
 *
 */
int32_t iftFtgen_lineString(iftFtgen_H hfg, int32_t line, char* utf8str, int32_t len, E_CHAR_SET chset);

int32_t iftFtgen_wordBuf(iftFtgen_H hfg, int32_t lineNum, iftParaLineIdx *lines, const int8_t *wordBuf, E_CHAR_SET chset);

int32_t iftFtgen_dumpGreyFrame(iftFtgen_H hfg, SFrameData **ppFtFrm);
int32_t iftFtgen_FreeDumpGreyFrame(SFrameData **ppFtFrm);
int32_t iftFtgen_dumpGrayFrame2pool(iftFtgen_H hfg, void* frampool, int poolsize);

int32_t iftFtgen_destory(iftFtgen_H hfg);

/**
 *
 */

//draw to mem
//draw bitmap
extern int osdFontGenTest(int argc, char** argv);

#endif /* OSDFONTGEN_H_ */
