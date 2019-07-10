/**
 * @file iftOsdService.h
 * @brief
 * @author jerry.king
 * @date 2012-12-19 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef IFTOSDSERVICE_H_
#define IFTOSDSERVICE_H_
#include "iftsysTypes.h"
/**
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

#define INVALD_OSDFONT_USER -1
/**
 * word pool alloc table
 */
#define  WORDPOOL_USR_BYTES     (32*256) //
#define  WORDPOOL_USR_LINES     (32)

#define MAX_PARA_LINECNT 12
#define MAX_LAYOUT_CANVACNT 3
typedef uint16_t iftCharPoolPos; // distance from wordpool start

typedef enum E_OSDROLE
{
    E_OSDROLE_SYSUsr = 0,
    E_OSDROLE_DSPUsr // for dsp usr
} E_OSDROLE;
/**
 * coordinate origin style
 */
typedef enum E_COOR_ORIG_STYLE
{
    E_ORIG_STYLE_UPLEFT  = 0,
    E_ORIG_STYLE_UPRIGHT = 1,
    //    E_ORIG_STYLE_DWLEFT  = 2,
    //    E_ORIG_STYLE_DWRIGHT = 3
} E_COOR_ORIG_STYLE;

typedef struct iftParaRegion
{
    iftCharPoolPos idxPtr;
    uint16_t len;
} iftParaRegion;

typedef struct iftParaLineIdx
{
    iftCharPoolPos linepos;
    uint16_t len; //line
} iftParaLineIdx;

//48*4 BYTE
typedef struct iftCanvas
{
    iftRect rect; //canvas rect
    E_COOR_ORIG_STYLE coosty;
    iftColor bgColor; //background color
    int32_t lineNum; //lines 0<=lineNum<=MAX_PARA_LINECNT
    iftParaLineIdx line[MAX_PARA_LINECNT];
    int32_t resv[31];
} iftCanvas;
//48*4 BYTE
typedef struct iftOsdServStyle
{
    int32_t     pt; //font pounds 1pt=64pix
    iftColor    color; //font color
    int32_t     resv1[30];
    int32_t     linegap; //line space
    int32_t     resv2[15];
} iftOsdServStyle;
//(8+48+48*3=200)*4 BYTE
typedef struct iftCanvasLayout
{
    iftOsdServStyle style;
    int32_t width; //osd drawing background width /pixel
    int32_t height; //osd drawing background height/pixel
    int32_t resv1[5];
    int32_t canvCnt;
    iftCanvas canv[MAX_LAYOUT_CANVACNT];
} iftCanvasLayout;

#define  DEF_OSDCOMM_VER 2
//(8+200+48)*4+24*256 = 7*1024=7168(BYTE)
typedef struct iftOsdText
{
    int32_t s_ver; //sturct ver =DEF_DSPCOMM_VER
    iftBool bEanble; // check fontcomm be valid
    E_OSDROLE role; //where data come from .
    int32_t userID; //  set INVALD_OSDFONT_USER when init, server assign registed userID
    int32_t resv1[4];
    iftCanvasLayout cavLay[WORDPOOL_USR_LINES];
    int32_t resv2[47];
    int16_t maxFontWidth;
    int16_t refreshFont;
    int16_t fg_mode;
    int16_t bg_mode;
    int16_t idx;
    int16_t position;
    int32_t poolsize;
    int32_t reload[WORDPOOL_USR_LINES];
    int8_t  wordBuf[WORDPOOL_USR_BYTES];
} iftOsdText;


#ifdef __cplusplus
} //extern "C" {
#endif

#endif /* IFTOSDSERVICE_H_ */
