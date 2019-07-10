/**
 * @file iftstring.h
 * @brief
 * @author jerry.king
 * @date 2012-12-5 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef IFTSTRING_H_
#define IFTSTRING_H_
#include "iftsysTypes.h"
#define FC_UTF8_MAX_LEN 6

inline int32_t iftGb2312ToUcs4(const char *src_orig, iftChar32 *dst, int len);

int32_t iftUtf8ToUcs4(const char *src_orig, iftChar32 *dst, int len);

int32_t iftUtf8Len(const char *string, int len, int *nchar, int *wchar);

int32_t iftUcs4ToUtf8(iftChar32 ucs4, int8_t dest[FC_UTF8_MAX_LEN]);

#endif /* IFTSTRING_H_ */
