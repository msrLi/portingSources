/**
 * @file iftFrameOperate.h
 * @brief
 * @author jerry.king
 * @date 2012-12-17 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef IFTFRAMEOPERATE_H_
#define IFTFRAMEOPERATE_H_
#include "iftosdint.h"
enum
{
    FONT_FG_MODE_BLACK = 0,
    FONT_FG_MODE_WHITE,
    FONT_FG_MODE_ADAPTIVE,
    FONT_FG_MODE_COLOR
};

enum
{
    FONT_BG_MODE_NORMAL = 0,
    FONT_BG_MODE_SEMI
};

typedef struct osdoption
{
    int line;       //the line number
    int mark;       //if reload the color
    int fg_mode;    //!0,dark; 1,white; 2,Adaptive; 3,color; 4,black
    int bg_mode;    //!0,normal, 1,Semitransparent
} osdoption;


int32_t iftFrmOper_overlay_outline(iftColor *fontColor, iftCanvasInt *canv, SFrameData *fontFrame,
                                   SFrameData *backgroundFrame, iftFontExtra *font_ext_conf, iftRange * innerProcRange);
int32_t iftFrmOper_overlay(iftColor *fontColor, iftCanvasInt *canv, SFrameData *fontFrame,
                           SFrameData *backgroundFrame);

/**
 * get banding frame
 * return banding data alloc
 */
#define MAX_BANDING_PIXS 10
const SFrameData * banding_frame_gray_data_alloc(int banding_pixs,  SFrameData *fontFrame, SFrameData *bandingFontFrame, iftRange * innerProcRange);
int  banding_frame_gray_data_free(SFrameData * frame);

#endif /* IFTFRAMEOPERATE_H_ */
