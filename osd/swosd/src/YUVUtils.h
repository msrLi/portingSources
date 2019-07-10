/**
 * @file YUVUtils.h
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

#ifndef YUVUTILS_H_
#define YUVUTILS_H_
#include "iftsysTypes.h"
//void ift_YUVTabInit(void);
inline void IFT_RGB2YUV(uint8_t r, uint8_t g, uint8_t b, uint8_t *y, uint8_t *u, uint8_t *v);
inline void IFT_RGB2YUV_Color(iftColor *cl_src_rgb, iftColorYUV * cl_des_yuv);
#endif /* YUVUTILS_H_ */
