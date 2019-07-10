/**
 * @file YUVUtils.cpp
 * @brief
 * @author jerry.king
 * @date 2012-12-16 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#include "YUVUtils.h"

static int yuvtab[9][256];
static int bYuvTabInit = 0;
#define DEF_DEGREE 1000
void ift_YUVTabInit(void)
{
    int i;
    for (i = 0; i < 256; i++)
    {
        yuvtab[0][i] = 0.257 * i; //YR
        yuvtab[1][i] = 0.504 * i; //YG
        yuvtab[2][i] = 0.098 * i; //YB
        yuvtab[3][i] = 0.148 * i; //UR
        yuvtab[4][i] = 0.291 * i; //UG
        yuvtab[5][i] = 0.439 * i; //UB
        yuvtab[6][i] = 0.368 * i; //VG
        yuvtab[7][i] = 0.071 * i; //VB
    }

}

inline void IFT_RGB2YUV(uint8_t r, uint8_t g, uint8_t b, uint8_t *y, uint8_t *u, uint8_t *v)
{
    if (!bYuvTabInit) { ift_YUVTabInit(); }
    *y = yuvtab[0][r] + yuvtab[1][g] + yuvtab[2][b] + 16;
    *u = -yuvtab[3][r] - yuvtab[4][g] + yuvtab[5][b] + 128;
    *v = yuvtab[5][r] - yuvtab[6][g] - yuvtab[7][b]  + 128;
}

inline void IFT_RGB2YUV_Color(iftColor *cl_src_rgb, iftColorYUV * cl_des_yuv)
{

    IFT_RGB2YUV(cl_src_rgb->r, cl_src_rgb->g, cl_src_rgb->b, &cl_des_yuv->y, &cl_des_yuv->u, &cl_des_yuv->v);
    cl_des_yuv->a = cl_src_rgb->a ;
}
