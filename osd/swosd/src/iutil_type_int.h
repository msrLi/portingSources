/*
 *filename:     iutil_type_int.h
 *author:       jerryking
 *created on:   2013/05/03
 *purpose:
 *
 */
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/
#ifndef iutil_type_int_h__
#define iutil_type_int_h__
#include <string.h>

#define MAX(a,b) (a)>(b)?(a):(b)
#define MIN(a,b) (a)<(b)?(a):(b)

#define DEF_REGION_IN(dt,begin,end) (((dt) >= (begin)) && ((dt) <= (end)))

#define MOD_FLOOR(x,mod) (((x)/(mod))*(mod))

#define MOD_CEILING_DIV(x,mod) (((x)+(mod)-1)/(mod))

#define MOD_CEILING(x,mod) (MOD_CEILING_DIV(x,mod)*(mod))

#define MOD_ADD(x,mod,val)  ( ((x)+(val))%(mod) )

#define MOD_MINUS(x,mod,val) ( ((x)+(mod)-(val))%(mod) )

#ifndef offsetof
#define offsetof(type, member)   (unsigned int)&(((type *)0)->member)
#endif //offsetof
/**
 * get type ponter by member
 */
#ifndef container_of
#define container_of(mempointer,type,member) (type*)((void*)(mempointer)-(offsetof(type,member)))
#endif // container_of
#endif // iutil_type_int_h__
