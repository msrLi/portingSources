/**
 * @file iftconfig.h
 * @brief
 * @author jerry.king
 * @date 2012-12-3 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/
#include "iftosdint.h"
#define  DEF_FONTPATH "/opt/fonts/msyh.ttf"
//#define  DEF_FONTPATH "./mshei.ttf"

#define DEF_HORZ_RESOLUTION 72
#define DEF_VERT_RESOLUTION 72
#define DEF_WORD_POINT 32


extern iftRect gs_defaultCavasRect ;

inline IftEnv* iftosdEnv_get();

inline iftFontInfo *iftFontInfo_hdr();
inline int iftFontInfo_init();
inline iftFontInfo *iftFontInfo_getByPath(char  *pname);

/**
 *
 */
inline iftStyle * iftGlobDefaultStyle();
inline int iftStyle_init(iftStyleSheet *sheet) ;
inline int iftStyle_deinit(iftStyleSheet *sheet);
inline iftStyle * iftStyle_default(iftStyleSheet *sheet);
inline iftStyle * iftStyle_getByID(iftStyleSheet *sheet, iftStyleID id);
inline  int iftStyle_equal(iftStyle * sty, iftOsdServStyleInt * sersty);
inline  iftStyle * iftStyle_find(iftStyleSheet *sheet, iftOsdServStyleInt *servStyle);

inline iftStyle * iftStyle_add(iftStyleSheet *sheet, iftOsdServStyleInt *servStyle);
inline int iftStyle_mov(iftStyleSheet *sheet, iftOsdServStyleInt *servStyle);
inline int iftStyle_movSty(iftStyleSheet *sheet, iftStyle * sty);
