/**
 * @file debug.h
 * @brief Debugging and logging prints
 * @author Philip Tan
 * @date 2012-05-31 Created
**/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
******************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif


void debugSetIdent(const char* ident);
const char* debugGetIdent();
void debugSetLevel(int level);
int debugGetLevel();
void debugSetOutputs(int outputs);
int debugGetOutputs();
void debugPrintf(int level, const char* fmt, ...);
typedef enum
{
    DBG_LEV_CRIT = 1,
    DBG_LEV_ERR = 2,
    DBG_LEV_WAR = 3,
    DBG_LEV_INF = 4,
    DBG_LEV_DBG = 7,
} EDebugLevel;

typedef enum
{
    DBG_OUT_CONSOLE = 1,
    DBG_OUT_SYSLOG = 2,
} EDebugOutput;

#define DEBUG
#define MYTRACE(t) \
        dprintf(DBG_LEV_INF,"trace" t);


#define CON_MARK  "\033[1;32;40m [%s:%d] \033[1;31;40m%s(): \033[0m "
#define LOG_MARK  "[%s:%d] %s(): "
#define dprintf(level,fmt,args...) \
    debugPrintf(level,CON_MARK#fmt,__FILE__,__LINE__,__FUNCTION__,##args);



#ifdef __cplusplus
}//extern "C" {
#endif

#endif
