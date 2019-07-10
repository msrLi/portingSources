/**
 * @file iftosd.h
 * @brief
 * @author jerry.king
 * @date 2012-11-28 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef __IFTOSD_H__
#define __IFTOSD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "iftsysTypes.h"
#include "iftOsdService.h"

typedef enum E_FRAME_TYPE
{
    E_Frame_YUV420SP_UV,  //NV12
    E_Frame_YUV420P_I420, //I420
    E_Frame_GRAY256,
    E_Frame_RGB_24,
    E_Frame_RGB_1555,
    E_Frame_RGB_4444,
    E_Frame_YUV420SP_VU  //NV21
} E_FRAME_TYPE;

typedef struct SFrameParam
{
    //int32_t reload;
    int32_t pixWidth;
    int32_t pixHight;
    int32_t pixLinePitch; //line pitch (build by memory save)
    E_FRAME_TYPE type;
    int32_t MaxFontAdvance; //if E_FRAME_TYPE = E_Frame_GRAY256 //as fontdata
} SFrameParam;

typedef enum E_FRMDATA_TYPE
{
    E_FRMDATA_TYPE_UNION,//Y和UV在一起存储
    E_FRMDATA_TYPE_SPAN, //Y和UV跨越存储()
} E_FRMDATA_TYPE;

typedef struct SUndata
{
    uint32_t len;
    uint8_t *d;
} SUndata;

typedef struct SFrameData
{
    SCoordinate2D bgn_point;
    SCoordinate2D end_point;
    struct SFrameParam* params;
    E_FRMDATA_TYPE dtype;
    char rongyu[4096];
    union
    {
        SUndata span[3];
        SUndata un;
    } data;
} SFrameData;

#define DEF_IFTOSD_VER  3
/////////////////////////////////////////////////////////////
typedef struct SOsdUserInt* iftOsdHandle ;//iftOsd;

/**
 * @brief Set osdfont env
 * @param[in] dbgLevel  default set 3
 * @param[in] iftosd_ver osd version set DEF_IFTOSD_VER
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_init(int32_t dbgLevel, int32_t iftosd_ver);
int32_t iftosd_handle_minsize();
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_handle_create(iftOsdHandle hand, iftRange *rgpools);

int32_t iftosd_defstyle_get(iftOsdHandle hand, iftOsdServStyle * servStyle);

int32_t iftosd_setOutline(iftOsdServStyle *style, int outlinePix);
int32_t iftosd_unsetOutline(iftOsdServStyle *style);

int32_t iftosd_overly_oneframe_Text2BitMap(iftOsdHandle hand, iftOsdText *text, SFrameData *oneFrame, E_CHAR_SET charset);
int32_t iftosd_overly_oneframe_MaxBitMap(iftOsdHandle hand, iftOsdText *text, int lines);
int32_t iftosd_overly_oneframe_DrawBitMap(iftOsdHandle hand, iftOsdText *text, SFrameData *oneFrame);


int32_t iftosd_overly_oneframe(iftOsdHandle hand, iftOsdText *text, SFrameData *oneFrame, E_CHAR_SET charset);
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_handle_destroy(iftOsdHandle hand);


#ifdef __cplusplus
} //extern "C" {
#endif

#endif /* __IFTOSD_H__ */
