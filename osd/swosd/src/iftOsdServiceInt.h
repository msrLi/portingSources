#ifndef  IFTOSDSERVICEINT_H
#define IFTOSDSERVICEINT_H
#include "iftOsdService.h"
typedef enum E_ROTATE
{
    E_ROTATE_D0 = 0, //rotate 0 degrees.
    E_ROTATE_D90, //rotate 90 degrees.
    E_ROTATE_D180 //rotate 180 degrees.
} E_ROTATE;

typedef enum E_ALIGN_TYPE
{
    E_ALIGN_RIGHT, //
    E_ALIGN_LEFT
} E_ALIGN_TYPE;

//16*4BYTE
typedef struct iftFontExtra
{
    iftBool bOutline;
    int32_t outLinePix; //pixel
    iftBool bRotate;
    int32_t rotateDegree;
    int32_t resv[12];
} iftFontExtra;

//32*4byte
typedef struct iftFontCtrl
{
    int32_t         pt; //font pounds
    iftColor        color;
    iftFontExtra    extra;
    int32_t         resv[14];
} iftFontCtrl;

//16*4 BYTE
typedef struct iftParaLayout
{
    int32_t         linegap; //line space pixel
    E_ALIGN_TYPE    align;
    iftBool         blineWrap;
    int32_t         resv[13];
} iftParaLayout;

//(32+16)*4BYTE =48
typedef struct iftOsdServStyleInt
{
    iftFontCtrl     fctrl;
    iftParaLayout   parag;
} iftOsdServStyleInt;

//48*4 BYTE
typedef struct iftCanvasInt
{
    iftRect             rect; //canvas rect
    E_COOR_ORIG_STYLE   coosty;
    iftColor            bgColor; //background color
    int32_t             lineNum; //lines 0<=lineNum<=MAX_PARA_LINECNT
    iftParaLineIdx      line[MAX_PARA_LINECNT];
    int16_t             fg_mode;
    int16_t             bg_mode;
    int16_t             position;
    int16_t             maxFontWidth;
    int32_t             resv1[2];
    iftBool             bReverseColor; //reverse bgColor and fontColor
    E_ROTATE            rotate; //rotate canvas
    iftBool             bSetfontColor; // =0 use default color =1 usefontColor
    iftColor            fontColor; //canvas font color ={0,0,0,0} for use default OR CanvasLaout
    iftBool             bSetParaLayout; // =0 use default ParaLayout =1 use parag value
    iftParaLayout       parag;
    int32_t             resv2[6];
} iftCanvasInt;

#endif //IFTOSDSERVICEINT_H
