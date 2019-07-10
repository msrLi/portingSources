/**
 * @file debug.c
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

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include "debug.h"

static int debugLevel = DBG_LEV_WAR;//DBG_LEV_INF;//Forest
static int debugOutputs = DBG_OUT_CONSOLE;
static char debugIdent[64];

void debugSetIdent(const char* ident)
{
    strcpy(debugIdent, ident);
}

const char* debugGetIdent()
{
    return debugIdent;
}

void debugSetLevel(int level)
{
    debugLevel = level;
}


int debugGetLevel()
{
    return debugLevel;
}

void debugSetOutputs(int outputs)
{
    debugOutputs = outputs;
    if ((debugOutputs & DBG_OUT_SYSLOG) == DBG_OUT_SYSLOG)
    {
        openlog(debugIdent, LOG_NDELAY, LOG_DAEMON);
    }
}


int debugGetOutputs()
{
    return debugOutputs;
}

void debugPrintf(int level, const char* fmt, ...)
{
    if ((level & 0x07) > debugLevel) { return; }

    static char buf[1024];
    static char tmstamp[32];

    va_list va;
    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t t = tv.tv_sec;
    int tn = strftime(tmstamp, sizeof(tmstamp), "%T.", localtime(&t));
    sprintf(tmstamp + tn, "%3.3lu",  tv.tv_usec / 1000);


    if ((debugOutputs & DBG_OUT_CONSOLE) == DBG_OUT_CONSOLE)
    {
        printf("%s%s\n", tmstamp, buf);
    }
    if ((debugOutputs & DBG_OUT_SYSLOG) == DBG_OUT_SYSLOG)
    {
        syslog(level, "%s", strchr(buf, '"'));
    }
}

